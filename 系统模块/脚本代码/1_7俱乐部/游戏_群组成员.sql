
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[IMF_GetTableID]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[IMF_GetTableID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_AddGroupMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_AddGroupMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_DeleteMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_DeleteMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_DeleteGroupMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_DeleteGroupMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryGroupMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryGroupMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_UpdateMemberBattleCount]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_UpdateMemberBattleCount]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ModifyRight]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ModifyRight]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ModifyInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ModifyInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

CREATE FUNCTION [dbo].[IMF_GetTableID]
(
   @dwGroupID BIGINT		--群组标识    
)
RETURNS INT
AS
BEGIN
    RETURN (@dwGroupID-1)/20000
END

GO

----------------------------------------------------------------------------------------------------

-- 添加成员
CREATE  PROCEDURE dbo.GSP_IM_AddGroupMember
	@dwGroupID BIGINT,			--群组标识
	@dwMemberID INT,			--成员标识	
	@lInvitationCode BIGINT		--邀请码
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- 执行逻辑
BEGIN
	
	-- 获取人数
	DECLARE @wMemberCount SMALLINT
	DECLARE @wMaxMemberCount SMALLINT
	DECLARE @dwCreaterID INT
	SELECT @wMemberCount=MemberCount, @wMaxMemberCount=MaxMemberCount, @dwCreaterID=CreaterID FROM IMGroupProperty WHERE GroupID=@dwGroupID

	-- 人数校验
	IF @wMemberCount IS NULL OR @wMemberCount=@wMaxMemberCount
	BEGIN
		RETURN 1;
	END	

	-- 读取群组列表
	DECLARE @strGroupIDArray NVARCHAR(1024)
	SELECT @strGroupIDArray=GroupIDArray FROM IMUserGroupInfo WHERE UserID=@dwMemberID
	IF @strGroupIDArray IS NULL 
	BEGIN
		SET @strGroupIDArray=';'
		INSERT INTO IMUserGroupInfo VALUES (@dwMemberID,@strGroupIDArray)
	END

	-- 查找标识
	DECLARE @strGroupID NVARCHAR(20)
	SET @strGroupID=';'+CONVERT(NVARCHAR(18),@dwGroupID)+';';	
	IF CHARINDEX(@strGroupID,@strGroupIDArray)>0
	BEGIN		
		RETURN 2
	END	

	DECLARE @nMemberRight INT
	SET @nMemberRight=0
	DECLARE @nAwardPoint INT
	SET @nAwardPoint=0
	if(@dwMemberID=@dwCreaterID)
	BEGIN
		SET @nMemberRight=1
		SELECT @nAwardPoint=OptionValue FROM IMGroupOption WHERE OptionName=N'CreatorPoint'
		IF @nAwardPoint IS NULL
			SET @nAwardPoint=90
	END
	INSERT INTO IMGroupMember(GroupID, UserID,MemberRight,CaptainID,AwardPoint) VALUES( CONVERT(NVARCHAR(18), @dwGroupID), CONVERT(NVARCHAR(12), @dwMemberID),@nMemberRight,0,@nAwardPoint)

	-- 插入成功
	IF @@ERROR=0
	BEGIN
		-- 添加群组标识		
		SET @strGroupIDArray=@strGroupIDArray+CONVERT(NVARCHAR(18),@dwGroupID)+';'
		UPDATE IMUserGroupInfo SET GroupIDArray=@strGroupIDArray WHERE UserID=@dwMemberID

		-- 更新人数
		UPDATE IMGroupProperty SET MemberCount=MemberCount+1 WHERE GroupID=@dwGroupID
		IF NOT EXISTS(SELECT 1 FROM IMGroupInvitationCode WHERE GroupID=@dwGroupID AND UserID=@dwMemberID)
			INSERT INTO IMGroupInvitationCode(GroupID,UserID) VALUES(@dwGroupID,@dwMemberID)

		DECLARE @SpreadGroupID BIGINT
		DECLARE @SpreadUserID BIGINT
		IF @lInvitationCode>0 AND @nMemberRight=0
		BEGIN
			SELECT @SpreadGroupID=GroupID,@SpreadUserID=UserID FROM IMGroupInvitationCode WHERE InvitationCode=@lInvitationCode
			IF @SpreadGroupID IS NOT NULL AND @SpreadGroupID=@dwGroupID
			BEGIN
				DECLARE @SpreadUserRight INT
				SELECT @SpreadUserRight=MemberRight FROM IMGroupMember WHERE GroupID=@SpreadGroupID AND UserID=@SpreadUserID
				IF (@SpreadUserRight&4)<>0
				BEGIN
					UPDATE IMGroupMember SET MemberRight=8,CaptainID=@SpreadUserID WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
					SET @nMemberRight=8
				END
			END
		END

		IF @SpreadUserID IS NULL
			SET @SpreadUserID=0
	END
	
	DECLARE @lMemberInvitationCode BIGINT
	SELECT @lMemberInvitationCode=InvitationCode FROM IMGroupInvitationCode WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	IF @lMemberInvitationCode IS NULL
		SET @lMemberInvitationCode=0

	DECLARE @AwardPoint INT
	SELECT @AwardPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	IF @AwardPoint IS NULL
		SET @AwardPoint=0
	-- 查询记录
	SELECT @dwMemberID AS MemberID,@lMemberInvitationCode AS InvitationCode,@AwardPoint AS AwardPoint, 0 AS BattleCount,@nMemberRight AS MemberRight,@SpreadUserID AS CaptainID, GetDate() AS JoinDateTime, UserID, GameID, Gender, NickName, CustomID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwMemberID 

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除成员
CREATE  PROCEDURE dbo.GSP_IM_DeleteGroupMember
	@dwGroupID BIGINT,		--群组标识
	@dwUserID	INT,		--操作玩家
	@dwMemberID INT			--成员标识	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- 执行逻辑
BEGIN

	DECLARE @UserRight INT
	SELECT @UserRight=MemberRight FROM IMGroupMember WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
	IF (@UserRight&4)<>0
	BEGIN
		--该队长的下属迁移到上级
		DECLARE @dwOldCaptain int
		SELECT @dwOldCaptain=CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID

		IF @dwOldCaptain=0
		BEGIN
			UPDATE IMGroupMember SET MemberRight=0 WHERE CaptainID=@dwMemberID AND GroupID=@dwGroupID AND MemberRight=8
		END

		UPDATE IMGroupMember SET CaptainID=@dwOldCaptain WHERE CaptainID=@dwMemberID AND GroupID=@dwGroupID
	END

	DELETE IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	--DELETE IMGroupInvitationCode WHERE GroupID=@dwGroupID AND Userid=@dwMemberID
	-- 删除成功
	IF @@ROWCOUNT=1
	BEGIN
		-- 读取群组信息
		DECLARE @strGroupIDArray NVARCHAR(1024)
		SELECT @strGroupIDArray=GroupIDArray FROM IMUserGroupInfo WHERE UserID=@dwMemberID

		-- 修改群组信息
		DECLARE @strGroupID NVARCHAR(20)		
		SET @strGroupID=';'+CONVERT(NVARCHAR(18),@dwGroupID)+';';
		SET @strGroupIDArray=REPLACE(@strGroupIDArray,@strGroupID,';');

		-- 更新群组
		UPDATE IMUserGroupInfo SET GroupIDArray=@strGroupIDArray WHERE UserID=@dwMemberID		

		-- 更新人数
		UPDATE IMGroupProperty SET MemberCount=MemberCount-1 WHERE GroupID=@dwGroupID

		--插入删除记录
		IF @dwUserID <> 0
		BEGIN
			DECLARE @UserID		INT
			DECLARE @GameID		INT
			DECLARE @CustomID	INT
			DECLARE @NickName	NVARCHAR(32)
			SELECT @UserID=UserID, @GameID=GameID, @CustomID=CustomID, @NickName=NickName	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwMemberID

			DECLARE @CreaterID  INT
			DECLARE @GroupName  NVARCHAR(32)
			SELECT @CreaterID=CreaterID,@GroupName=GroupName FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

			DECLARE @OperatorGameID		INT
			DECLARE @OperatorNickName	NVARCHAR(32)
			SELECT  @OperatorGameID=GameID,@OperatorNickName=NickName	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

			INSERT INTO IMGroupApplyRecord(GroupID, ApplyerID, ApplyerGameID, ApplyerNickName, ApplyerCustomID, ApplyStatus, CreaterID, GroupName,ApplyType,OperatorID,OperatorName, CollectDateTime)
			VALUES (@dwGroupID, @dwMemberID, @GameID, @NickName, @CustomID, 3, @CreaterID, @GroupName,1,@OperatorGameID,@OperatorNickName, GetDate())	
			DECLARE @RecordID INT
			SELECT 	@RecordID=SCOPE_IDENTITY()
			SELECT * FROM IMGroupApplyRecord WHERE RecordID=@RecordID
		END
	END	
	ELSE
	BEGIN
		return 1
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除成员
CREATE  PROCEDURE dbo.GSP_IM_DeleteMember
	-- 用户信息
	@dwUserID INT,								-- 用户标识	
	@strPassword NCHAR(32),						-- 用户密码

	--群组信息
	@dwGroupID BIGINT,							-- 群组标识
	
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID	INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT	
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
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
	SELECT @CreaterID=CreaterID FROM IMGroupProperty WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要的大联盟不存在，请查证后再次尝试！'
		RETURN 5		
	END

	DECLARE @MemberRight  INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @MemberRight IS NULL
		SET @MemberRight=0

	IF @CreaterID<> @dwUserID
	BEGIN
		IF (@MemberRight&0x02)<>0 
		BEGIN
			DECLARE @ManagerRight INT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF @ManagerRight IS NULL
				SET @ManagerRight=0
			IF (@ManagerRight&0x10)=0 -- 检查用户设置权限
			BEGIN
				SET @strErrorDescribe=N'操作失败，权限不够！'	
				RETURN 4
			END
		END
		ELSE
		BEGIN
			SET @strErrorDescribe=N'操作失败，权限不够！'	
			RETURN 4
		END
	END

	/*
	-- 变量定义	
	DECLARE	@Return_Value INT
	
	-- 添加成员
	EXEC @Return_Value = GSP_IM_DeleteGroupMember
		 @dwGroupID = @dwGroupID,
		 @dwMemberID = @dwMemberID
	RETURN @Return_Value
	*/
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询群组成员
CREATE  PROCEDURE dbo.GSP_IM_QueryGroupMember
	@dwGroupID BIGINT		--群组标识	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- 执行逻辑
BEGIN
	
	-- 查询数据表
	SELECT a.UserID, a.GameID, a.NickName, a.Gender, a.CustomID,
	b.BattleCount, b.JoinDateTime, b.Score, b.MemberRight,b.CaptainID,b.FavoriteConfig,b.MemberManagerRight,b.ManagerRightFlag,b.AwardPoint,
	c.InvitationCode,b.UserStatus,b.QQ,b.WX,b.Phone
	FROM WHQJAccountsDB.dbo.AccountsInfo a,IMGroupMember b,IMGroupInvitationCode c
	WHERE a.UserID=b.UserID AND  b.GroupID=@dwGroupID AND b.GroupID=c.GroupID AND b.UserID=c.UserID


END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新约战次数
CREATE  PROCEDURE dbo.GSP_IM_UpdateMemberBattleCount
	@dwGroupID BIGINT,		--群组标识	
	@dwMemberID BIGINT,		--成员标识	
	@wIncBattleCount SMALLINT --开房次数
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 更新脚本		
	UPDATE IMGroupMember SET BattleCount=BattleCount+@wIncBattleCount WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	-- 查询脚本		
	SELECT * FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 修改权限
CREATE  PROCEDURE dbo.GSP_IM_ModifyRight
	-- 用户信息
	@dwOperatorID INT,							-- 用户标识	
	@strPassword NCHAR(32),						-- 用户密码

	--群组信息
	@dwGroupID BIGINT,							-- 群组标识
	@dwMemberID BIGINT,							-- 成员标识
	
	--操作信息
	@nRight INT,								--权限
	@cbMode TINYINT,							--模式：0增加，1移除
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID	INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT	
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwOperatorID

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

	-- 查询盟主
	DECLARE @CreaterID INT
	DECLARE @ManagerCount SMALLINT
	DECLARE @strManageUserIDArray nvarchar(512)
	
	SELECT @CreaterID=CreaterID FROM IMGroupProperty WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要的大联盟不存在，请查证后再次尝试！'
		RETURN 5		
	END
	
	IF @nRight=2--修改管理员
	BEGIN
		IF @dwOperatorID <> @CreaterID
		BEGIN
			SET @strErrorDescribe=N'您不是该大联盟盟主，无法修改权限！'
			RETURN 6	
		END
	
		IF @dwOperatorID = @dwMemberID
		BEGIN
			SET @strErrorDescribe=N'您是该大联盟盟主，不能修改自己权限！'
			RETURN 7	
		END
	END
	ELSE IF @nRight=4--修改队长
	BEGIN
		DECLARE @HaveRight INT 
		SET @HaveRight=0

		IF @CreaterID = @dwOperatorID
			SET @HaveRight=1
		ELSE
		BEGIN
			DECLARE @UserRight BIGINT
			SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperatorID
			IF @UserRight IS NULL
			BEGIN
				SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
				RETURN 6	
			END

			IF (@UserRight&2) <> 0--管理员
			BEGIN
				DECLARE @ManagerRight BIGINT
				DECLARE @Flag TINYINT
				SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperatorID
				IF @Flag<>1
					SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

				IF (@ManagerRight&0x200)<>0 AND @cbMode=0
					SET @HaveRight=1
				ELSE IF (@ManagerRight&0x400)<>0 AND @cbMode=1
					SET @HaveRight=1
			END
			ELSE IF (@UserRight&0x04)<>0 AND @dwOperatorID=(SELECT CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID)
				SET @HaveRight=2
			
		END

		IF @HaveRight=0 
		BEGIN
			SET @strErrorDescribe=N'抱歉，您没有权限进行此操作！'
			RETURN 6		
		END
	END

	
	--判断玩家
	DECLARE @nCurRight INT
	SELECT @nCurRight= MemberRight FROM IMGroupMember WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
	IF  @nCurRight IS NULL 
	BEGIN
		SET @strErrorDescribe=N'该玩家不是大联盟成员，请查证后再次尝试！'
		RETURN 8	
	END
	
	if(@cbMode=0)--添加权限
	BEGIN
		--玩家是否已经具备权限
		IF (@nCurRight&@nRight)<>0
		BEGIN
			SET @strErrorDescribe=N'该玩家已经具备此权限，不能重复添加！'
			RETURN 9	
		END
	
		IF @nRight=2
		BEGIN
			IF (@nCurRight)<>0--玩家是否已经具备其他权限
			BEGIN
				--SET @strErrorDescribe=N'USERID=['+RTRIM(LTRIM(STR(@dwMemberID)))+N' ]已经具备其他权限，无法修改！'
				SET @strErrorDescribe=N'该玩家已经具备其他权限，无法修改！'
				RETURN 9	
			END

			-- 添加玩家权限	
			UPDATE IMGroupMember SET MemberRight=@nRight WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
		END
		ELSE IF(@nRight=4)--队长权限
		BEGIN
			IF(@nCurRight>0 AND @nCurRight<>8)
			BEGIN
				SET @strErrorDescribe=N'该玩家已经具备其他权限，无法修改！'
				RETURN 9	
			END

			UPDATE IMGroupMember SET MemberRight=4 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID

			IF @HaveRight=1
			BEGIN
				UPDATE IMGroupMember SET CaptainID=0,AwardPoint=0 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
			END
			ELSE IF @HaveRight=2
				UPDATE IMGroupMember SET CaptainID=@dwOperatorID,AwardPoint=0 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
		END




	END
	ELSE--移除权限
	BEGIN
		--玩家是否已经具备权限
		IF (@nCurRight&@nRight)=0
		BEGIN
			SET @strErrorDescribe=N'USERID=['+RTRIM(LTRIM(STR(@dwMemberID)))+N','+RTRIM(LTRIM(STR(@nCurRight)))+N' ]不具备此权限'+RTRIM(LTRIM(STR(@nRight)))+N'，无法移除！'
			RETURN 10	
		END
	
		-- 移除玩家权限	
		UPDATE IMGroupMember SET MemberRight=(@nCurRight&~@nRight) WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
		IF @nRight=2
			UPDATE IMGroupMember SET ManagerRightFlag=0,MemberManagerRight=0 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
		ELSE IF(@nRight=4)--移除队长权限
		BEGIN
			UPDATE IMGroupMember SET AwardPoint=0 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
				
			--该队长的下属迁移到上级
			DECLARE @dwOldCaptain int
			SELECT @dwOldCaptain=CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID

			IF @dwOldCaptain<>0
			BEGIN
				UPDATE IMGroupMember SET MemberRight=8 WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
			END
			ELSE
			BEGIN
				UPDATE IMGroupMember SET MemberRight=0 WHERE CaptainID=@dwMemberID AND GroupID=@dwGroupID AND MemberRight=8
			END

			UPDATE IMGroupMember SET CaptainID=@dwOldCaptain WHERE CaptainID=@dwMemberID AND GroupID=@dwGroupID
		END
		ELSE IF (@nRight=8)--移除队员
		BEGIN
			UPDATE IMGroupMember SET CaptainID=0 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
		END
	END

	DECLARE @nMemberRight INT
	SELECT @nMemberRight=MemberRight FROM IMGroupMember WHERE UserID=@dwMemberID AND GroupID=@dwGroupID

	SELECT @nMemberRight AS MemberRight

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 修改权限
CREATE  PROCEDURE dbo.GSP_IM_ModifyInfo
	@dwGroupID BIGINT,							-- 群组标识
	@dwUserID BIGINT,							-- 成员标识
	@cbFlag0 TINYINT,
	@cbFlag1 TINYINT,
	@cbFlag2 TINYINT,
	@szQQ nvarchar(32),							-- 用户QQ
	@szWX nvarchar(32),							-- 用户WX
	@szPhone nvarchar(11),						-- 用户phone
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询盟主
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟不存在，请查证后再次尝试！'
		RETURN 1		
	END

	DECLARE @UserRight BIGINT
	SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @UserRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
		RETURN 2	
	END

	IF @cbFlag0=1
		UPDATE IMGroupMember set QQ=@szQQ WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @cbFlag1=1
		UPDATE IMGroupMember set WX=@szWX WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @cbFlag2=1
		UPDATE IMGroupMember set Phone=@szPhone WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	SELECT @UserRight AS MemberRight
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

