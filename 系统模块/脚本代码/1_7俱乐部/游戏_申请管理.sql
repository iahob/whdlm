
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ApplyRequest]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ApplyRequest]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ApplyRespond]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ApplyRespond]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ApplyDelete]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ApplyDelete]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_LoadApplyMessage]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_LoadApplyMessage]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 申请请求
CREATE  PROCEDURE dbo.GSP_IM_ApplyRequest
	-- 用户信息	
	@dwGroupID BIGINT,							-- 群组标识	
	@dwApplyerID INT,							-- 用户标识
	@cbApplyType TINYINT,						-- 申请类型
	@strPassword NCHAR(32),						-- 用户密码
	@lInvitationCode BIGINT,					-- 邀请码
	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @CustomID	INT
DECLARE @NickName	NVARCHAR(32)

-- 群组信息
DECLARE @CreaterID  INT
DECLARE @GroupName  NVARCHAR(32)
DECLARE @GroupStatus TINYINT
DECLARE @MemberCount SMALLINT
DECLARE @MaxMemberCount SMALLINT
DECLARE @GroupIDShow TINYINT
-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @CustomID=CustomID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwApplyerID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END	

	-- 帐号关闭
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须到重新开通后才能继续使用！'
		RETURN 3
	END

	---- 密码判断
	--IF @LogonPass<>@strPassword
	--BEGIN
	--	SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
	--	RETURN 4
	--END

	-- 查询群组	
	IF @dwGroupID=0 AND @lInvitationCode>0
	BEGIN
		DECLARE @TempGroupID BIGINT
		DECLARE @SpreadUserID BIGINT
		SELECT @TempGroupID=GroupID,@SpreadUserID=UserID FROM IMGroupInvitationCode WHERE InvitationCode=@lInvitationCode
		IF @TempGroupID IS NULL
		BEGIN
			SET @strErrorDescribe=N'邀请码信息不存在，请查证后再次尝试！'		
			RETURN 5
		END
		
		DECLARE @SpreadRight INT
		SELECT @SpreadRight=MemberRight FROM IMGroupMember WHERE GroupID=@TempGroupID AND UserID=@SpreadUserID
		IF @SpreadRight IS NULL OR (@SpreadRight<>0x01 AND @SpreadRight<>4)
		BEGIN
			SET @strErrorDescribe=N'无效的邀请码，请查证后再次尝试！'		
			RETURN 6
		END

		SELECT @GroupIDShow=GroupIDShow FROM IMGroupBattleRule(NOLOCK) WHERE GroupID=@TempGroupID
		SELECT @CreaterID=CreaterID,@GroupName=GroupName,@GroupStatus=GroupStatus,@MemberCount=MemberCount,@MaxMemberCount=MaxMemberCount FROM IMGroupProperty(NOLOCK) WHERE GroupID=@TempGroupID

		--IF @GroupIDShow<>0
		--BEGIN
		--	SET @strErrorDescribe=N'该大联盟只允许通过ID加入！'		
		--	RETURN 6
		--END

		SET @dwGroupID=@TempGroupID
	END
	ELSE IF @dwGroupID>0
	BEGIN
		SELECT @CreaterID=CreaterID,@GroupName=GroupName,@GroupStatus=GroupStatus,@MemberCount=MemberCount,@MaxMemberCount=MaxMemberCount FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID
		SELECT @GroupIDShow=GroupIDShow FROM IMGroupBattleRule(NOLOCK) WHERE GroupID=@dwGroupID
		
		--IF @GroupIDShow<>1 AND @cbApplyType=0
		--BEGIN
		--	SET @strErrorDescribe=N'该大联盟只允许通过邀请码加入！'		
		--	RETURN 6
		--END
	END
	-- 大联盟不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您申请的大联盟信息不存在，请查证后再次尝试！'		
		RETURN 5
	END

	-- 大联盟已停用
	IF @GroupStatus<>1
	BEGIN
		SET @strErrorDescribe=N'抱歉，您申请的大联盟已被盟主停止使用！'				
		RETURN 7
	END

	-- 读取群组列表
	DECLARE @strGroupIDArray NVARCHAR(1024)
	SELECT @strGroupIDArray=GroupIDArray FROM IMUserGroupInfo(NOLOCK) WHERE UserID=@dwApplyerID
	IF @strGroupIDArray IS NULL 
	BEGIN
		SET @strGroupIDArray=';'
		INSERT INTO IMUserGroupInfo VALUES (@dwApplyerID,@strGroupIDArray)
	END

	-- 查找标识
	DECLARE @strGroupID NVARCHAR(20)
	SET @strGroupID=';'+CONVERT(NVARCHAR(18),@dwGroupID)+';';

	IF @cbApplyType=0
	BEGIN
		-- 存在判断
		IF EXISTS(SELECT * FROM IMGroupApplyRecord(NOLOCK) WHERE GroupID=@dwGroupID AND ApplyerID=@dwApplyerID AND ApplyStatus=0 AND ApplyType=0)
		BEGIN
			SET @strErrorDescribe=N'您已经申请加入该大联盟，不需要重复申请，请耐心等待盟主审核！'		
			RETURN 5					
		END	

		-- 大联盟满员
		IF @MemberCount=@MaxMemberCount
		BEGIN
			SET @strErrorDescribe=N'抱歉，该大联盟已满员，您不能申请加入！'				
			RETURN 8		
		END	

		-- 创建数量
		DECLARE @wCreateGroupCount SMALLINT
		SELECT @wCreateGroupCount=COUNT(GroupID) FROM IMGroupProperty(NOLOCK) WHERE CreaterID=@dwApplyerID AND GroupStatus=1

	
		IF CHARINDEX(@strGroupID,@strGroupIDArray)>0
		BEGIN		
			SET @strErrorDescribe=N'抱歉，你已经是该大联盟的成员！'
			RETURN 9
		END	

		-- 查询配置
		DECLARE @MaxJoinGroupCount SMALLINT		
		SELECT @MaxJoinGroupCount=OptionValue FROM IMGroupOption(NOLOCK) WHERE OptionName=N'MaxJoinGroupCount'
		IF @MaxJoinGroupCount IS NULL SET @MaxJoinGroupCount=5
	
		-- 群组数目
		DECLARE @dwGroupCount SMALLINT	
		SELECT @dwGroupCount=(LEN(@strGroupIDArray)-LEN(REPLACE(@strGroupIDArray, ';', '')))

		-- 调整变量
		IF @dwGroupCount>0 SET @dwGroupCount=@dwGroupCount-1
	
		-- 统计次数	
		DECLARE @ApplyCount SMALLINT
		SELECT @ApplyCount=COUNT(GroupID) FROM IMGroupApplyRecord(NOLOCK) WHERE ApplyerID=@dwApplyerID AND ApplyStatus=0

		-- 配置校验
		IF (@dwGroupCount-@wCreateGroupCount)+@ApplyCount>=@MaxJoinGroupCount
		BEGIN
			SET @strErrorDescribe=N'抱歉，您申请加入的大联盟个数已达到系统配置的最大数目！'		
			RETURN 10				
		END

		---------------------------------------------------------------------------------------------------------------------------

		-- 申请记录
		INSERT INTO IMGroupApplyRecord(GroupID, ApplyerID, ApplyerGameID, ApplyerNickName, ApplyerCustomID, ApplyStatus, CreaterID, GroupName,ApplyType, CollectDateTime,InvitationCode)
		VALUES (@dwGroupID, @dwApplyerID, @GameID, @NickName, @CustomID, 0, @CreaterID, @GroupName,@cbApplyType, GetDate(),@lInvitationCode)	

		-- 记录标识
		DECLARE @RecordID INT
		SELECT 	@RecordID=SCOPE_IDENTITY()

		DECLARE @cbInMode TINYINT
		SELECT @cbInMode=InMode FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
		IF @cbInMode=0
		BEGIN
			-- 加入成功
			UPDATE IMGroupApplyRecord SET ApplyStatus=1,DeleteMask=2 ,OperatorName=N'自动加入' WHERE RecordID=@RecordID
			SET @strErrorDescribe=N'加入成功！'	
		END
		ELSE
		BEGIN
			-- 申请成功
			SET @strErrorDescribe=N'申请加入成功，请耐心等待盟主审核！'	
		END
	END
	ELSE IF @cbApplyType=1
	BEGIN
		-- 存在判断
		IF EXISTS(SELECT * FROM IMGroupApplyRecord(NOLOCK) WHERE GroupID=@dwGroupID AND ApplyerID=@dwApplyerID AND ApplyStatus=0 AND ApplyType=1)
		BEGIN
			SET @strErrorDescribe=N'您已经申请退出，不需要重复申请，请耐心等待盟主审核！'		
			RETURN 5					
		END	

		IF CHARINDEX(@strGroupID,@strGroupIDArray)=0
		BEGIN		
			SET @strErrorDescribe=N'申请退出失败，你不是该大联盟的成员！'
			RETURN 9
		END	

		-- 退出记录
		INSERT INTO IMGroupApplyRecord(GroupID, ApplyerID, ApplyerGameID, ApplyerNickName, ApplyerCustomID, ApplyStatus, CreaterID, GroupName,ApplyType, CollectDateTime)
		VALUES (@dwGroupID, @dwApplyerID, @GameID, @NickName, @CustomID, 0, @CreaterID, @GroupName,@cbApplyType, GetDate())	

		-- 记录标识
		SELECT 	@RecordID=SCOPE_IDENTITY()

		DECLARE @cbOutMode TINYINT
		SELECT @cbOutMode=OutMode FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
		IF @cbOutMode=0
		BEGIN
			-- 退出成功
			UPDATE IMGroupApplyRecord SET ApplyStatus=1,DeleteMask=2,OperatorName=N'自动退出' WHERE RecordID=@RecordID
			SET @strErrorDescribe=N'您已成功退出该大联盟！'	
		END
		ELSE
		BEGIN
			-- 申请成功
			SET @strErrorDescribe=N'申请退出成功，请耐心等待盟主审核！'	
		END
	END
	ELSE
	BEGIN
		SET @strErrorDescribe=N'申请类型错误，请重试！'		
		RETURN 5	
	END


	-- 查询信息
	SELECT * FROM IMGroupApplyRecord WHERE RecordID=@RecordID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 申请响应
CREATE  PROCEDURE dbo.GSP_IM_ApplyRespond
	-- 用户信息
	@dwRecordID INT,							-- 记录标识 
	@dwUserID	 INT,							-- 用户标识
	@dwApplyerID INT,							-- 用户标识	
	@dwGroupID INT,								-- 站点标识		
	@strPassword NCHAR(32),						-- 用户密码
	@cbApplyStatus TINYINT,						-- 申请状态
	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END	

	-- 帐号关闭
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须到重新开通后才能继续使用！'
		RETURN 3
	END

	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END

	-- 查询群主
	DECLARE @CreaterID INT
	DECLARE @MemberCount SMALLINT
	DECLARE @MaxMemberCount SMALLINT
	SELECT @CreaterID=CreaterID, @MemberCount=MemberCount, @MaxMemberCount=MaxMemberCount FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF @CreaterID = @dwUserID
		SET @HaveRight=1
	ELSE
	BEGIN
		DECLARE @UserRight BIGINT
		SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		IF @UserRight IS NULL
		BEGIN
			SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
			RETURN 6	
		END

		IF (@UserRight&2) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x02)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您没有权限进行此操作！'
		RETURN 7		
	END

	-- 查询记录
	DECLARE @GroupID INT
	DECLARE @ApplyerID INT
	DECLARE @ApplyStatus TINYINT	
	DECLARE @ApplyType TINYINT	
	SELECT @GroupID=GroupID,@ApplyerID=ApplyerID,@ApplyStatus=ApplyStatus ,@ApplyType=ApplyType FROM IMGroupApplyRecord(NOLOCK) WHERE RecordID=@dwRecordID

	IF @ApplyerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'系统未查询到相关的申请记录，请联系客户服务中心了解详细情况！'
		RETURN 6		
	END 

	IF @dwApplyerID<>@ApplyerID OR @dwGroupID<>@GroupID
	BEGIN
		SET @strErrorDescribe=N'系统检测到申请记录异常，请联系客户服务中心了解详细情况！'
		RETURN 7
	END	

	-- 大联盟满员
	IF @MemberCount=@MaxMemberCount AND @cbApplyStatus=1 AND @ApplyType=0
	BEGIN
		SET @strErrorDescribe=N'抱歉，您的大联盟已经满员，成员添加失败！'
		RETURN 5		
	END



	-- 群组属性
	UPDATE IMGroupApplyRecord SET ApplyStatus=@cbApplyStatus,DeleteMask=2,OperatorID=@GameID,OperatorName=@NickName WHERE RecordID=@dwRecordID		

	-- 查询记录
	SELECT * FROM IMGroupApplyRecord(NOLOCK) WHERE RecordID=@dwRecordID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 申请删除
CREATE  PROCEDURE dbo.GSP_IM_ApplyDelete
	-- 用户信息
	@dwMessageID INT,							-- 消息标识 
	@cbMessageType INT,							-- 消息类型 
	@dwUserID INT,								-- 用户标识	
	@strPassword NCHAR(32),						-- 用户密码	
	
	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END	

	-- 帐号关闭
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须到重新开通后才能继续使用！'
		RETURN 3
	END

	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END

	-- 查询消息
	DECLARE @CreaterID INT
	DECLARE @ApplyerID INT
	DECLARE @ApplyStatus TINYINT
	DECLARE @DeleteMask TINYINT
	SELECT @CreaterID=CreaterID,@ApplyerID=ApplyerID,@ApplyStatus=ApplyStatus,@DeleteMask=DeleteMask FROM IMGroupApplyRecord(NOLOCK) WHERE RecordID=@dwMessageID

	-- 消息不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'抱歉，服务未查询到您要删除的消息！'
		RETURN 5		
	END

	-- 申请者删除
	IF @cbMessageType<>1 OR @dwUserID<>@ApplyerID
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是申请者,无法删除消息！'
		RETURN 6		
	END	

	-- 删除消息
	DELETE IMGroupApplyRecord WHERE RecordID=@dwMessageID

	SELECT @CreaterID AS CreaterID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 申请响应
CREATE  PROCEDURE dbo.GSP_IM_LoadApplyMessage
	-- 用户信息	
	@dwUserID INT,								-- 用户标识
	@cbMessageType INT							-- 消息类型		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询消息
	--SELECT * FROM IMGroupApplyRecord(NOLOCK) WHERE CreaterID=@dwUserID 
	--查找自己是盟主或者自己是管理员并且具有审核权限的消息
	SELECT * FROM IMGroupApplyRecord(NOLOCK) WHERE GroupID IN 
	(SELECT A.GroupID FROM IMGroupProperty A,IMGroupBattleRule B,IMGroupMember C WHERE A.GroupID=B.GroupID AND 
	A.GroupID=C.GroupID AND (A.CreaterID=@dwUserID OR (B.ManagerRight&0x02)<>0 /*AND (C.MemberRight&0x02)<>0 AND C.UserID=@dwUserID*/)
	) 
	ORDER BY CollectDateTime 

END

RETURN 0

GO
