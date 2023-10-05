
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryGroupConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryGroupConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_AppendConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_AppendConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ModifyConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ModifyConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_DeleteConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_DeleteConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryCreate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryCreate]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetFavoriteConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SetFavoriteConfig]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 查询群组玩法
CREATE  PROCEDURE dbo.GSP_IM_QueryGroupConfig
	@dwGroupID BIGINT		--群组标识	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询数据表
	SELECT * FROM IMGroupConfig WHERE GroupID=@dwGroupID

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- 创建玩法
CREATE  PROCEDURE dbo.GSP_IM_AppendConfig
	-- 索引变量
	@dwGroupID   INT,							-- 俱乐部ID
	@dwCreaterID INT,							-- 创建玩家ID
	@strConfigName NVARCHAR(12),				-- 玩法名称
	@wKindID     INT,							-- 类型标识
	@cbPlayMode	 TINYINT,						-- 结算模式
	@lCellScore BIGINT,							-- 单元积分
	@dwDrawCountLimit BIGINT,					-- 局数限制
	@dwDrawTimeLimit BIGINT,					-- 时间限制
	@wJoinCount INT,							-- 参与人数
	@dwRoomTax	INT,							-- 私人房服务比例
	@cbPassword	TINYINT,						-- 密码模式
	@cbPayMode	TINYINT,						-- 支付模式
	@cbGameMode	TINYINT,						-- 游戏模式
	@lEnterScore BIGINT,						-- 进入分
	@lLeaveScore BIGINT,						-- 离开分	
	@cbJoinMode	TINYINT,						-- 参与模式
	@cbTaxMode	TINYINT,						-- 服务比例模式
	@lTaxCount  BIGINT,							-- 服务比例数值
	@cbLessScoreDismiss  TINYINT,				-- 分数不够解散
	@lTimeSecond BIGINT,						-- 时间戳
	@cbGroupPersonalMaxChair TINYINT,			-- 大联盟百人游戏标识
	@strGameRule NVARCHAR(200),					-- 定制规则

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
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

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

	DECLARE @GroupCreaterID INT
	Select @GroupCreaterID=CreaterID FROM   WHQJGroupDB.dbo.IMGroupProperty where GroupID=@dwGroupID
	IF @GroupCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请检查！'
		RETURN 1
	END

	-- 检查权限
	DECLARE @MemberRight INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwCreaterID

	IF @MemberRight IS NULL
		SET @MemberRight=0

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF (@MemberRight&0x01) <> 0
		SET @HaveRight=1
	ELSE
	BEGIN

		IF (@MemberRight&0x02) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwCreaterID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
			IF (@ManagerRight&0x40)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您没有权限进行此操作！'
		RETURN 4		
	END

	IF @cbPlayMode=1 AND NOT EXISTS(SELECT 1 FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND CanUseMedal=1 )
	BEGIN
		SET @strErrorDescribe=N'抱歉，该大联盟无权创建勋章玩法！'
		RETURN 5	
	END

	-- 创建消耗
	DECLARE @CreateConfigDeductIngot BIGINT
	SELECT @CreateConfigDeductIngot=OptionValue FROM IMGroupOption WHERE OptionName=N'CreateConfigDeductIngot'					
	IF @CreateConfigDeductIngot IS NULL SET @CreateConfigDeductIngot=0
	
	-- 查询盟主钻石
	DECLARE @UserIngot BIGINT
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency(NOLOCK) WHERE UserID=@GroupCreaterID
	IF @UserIngot IS NULL SET @UserIngot=0

	DECLARE @MaxUserPay INT
	DECLARE @MaxGroupPay INT	

	SELECT @MaxUserPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @GroupCreaterID AND GroupID=0 AND RoomStatus <> 2
	SELECT @MaxGroupPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE RoomStatus <> 2 
	AND GroupID in(Select GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty where CreaterID=@GroupCreaterID) 
	IF @MaxUserPay IS NULL SET @MaxUserPay=0
	IF @MaxGroupPay IS NULL	SET @MaxGroupPay=0

	IF @UserIngot<@CreateConfigDeductIngot+@MaxUserPay+@MaxGroupPay
	BEGIN
		SET @strErrorDescribe=N'创建失败，盟主钻石少于'+CONVERT(NVARCHAR(16),@CreateConfigDeductIngot+@MaxUserPay+@MaxGroupPay)+',无法创建玩法！'
		RETURN 6		
	END

	DECLARE @valueMin INT
	DECLARE @valueMax INT
	IF @cbTaxMode < 3
	BEGIN

		SELECT @valueMin= OptionValue FROM IMGroupOption WHERE OptionName='FixedMin'
		IF @valueMin IS NULL
			SET @valueMin=0
		
		SELECT @valueMax= OptionValue FROM IMGroupOption WHERE OptionName='FixedMax'
		IF @valueMax IS NULL
			SET @valueMax=100
	END
	ELSE
	BEGIN
		SELECT @valueMin= OptionValue FROM IMGroupOption WHERE OptionName='PercentMin'
		IF @valueMin IS NULL
			SET @valueMin=0
		
		SELECT @valueMax= OptionValue FROM IMGroupOption WHERE OptionName='PercentMax'
		IF @valueMax IS NULL
			SET @valueMax=100	
	END

	IF @lTaxCount<@valueMin OR @lTaxCount>@valueMax
	BEGIN
		SET @strErrorDescribe=N'创建失败， 贡献数值范围'+CONVERT(NVARCHAR(16),@valueMin) +'-'+CONVERT(NVARCHAR(16),@valueMax)+'！'
		RETURN 7		
	END

	-- 插入玩法
	DECLARE @CreateDateTime DATETIME
	SET @CreateDateTime=GetDate()

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond-@CreateConfigDeductIngot WHERE UserID=@dwCreaterID
	-- 群组属性
	INSERT INTO IMGroupConfig(CreaterID, GroupID,ConfigName, KindID,PlayMode, CellScore, DrawCountLimit, DrawTimeLimit,JoinCount,RoomTax, PasswordMode,PayMode,GameMode,JoinMode,EnterScore,LeaveScore,GameRule,TaxMode,TaxCount,LessScoreDismiss,TimeSecond, GroupPersonalMaxChair)
	VALUES (@GameID, @dwGroupID,@strConfigName, @wKindID,@cbPlayMode, @lCellScore, @dwDrawCountLimit,@dwDrawTimeLimit,@wJoinCount,@dwRoomTax, @cbPassword,@cbPayMode,@cbGameMode,@cbJoinMode,@lEnterScore,@lLeaveScore,@strGameRule,@cbTaxMode,@lTaxCount,@cbLessScoreDismiss,@lTimeSecond, @cbGroupPersonalMaxChair)
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- 错误信息
		SET @strErrorDescribe=N'由于系统原因,玩法创建失败[0]，请联系客户服务中心了解详细情况！'
		RETURN 6
	END
	

	-- 查询群组
	DECLARE @dwConfigID BIGINT
	SELECT 	@dwConfigID=SCOPE_IDENTITY()

	SELECT @dwConfigID AS ConfigID,@UserIngot-@CreateConfigDeductIngot AS Diamond

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED		
	
END

RETURN 0

GO



----------------------------------------------------------------------------------------------------

-- 修改玩法
CREATE  PROCEDURE dbo.GSP_IM_ModifyConfig
	-- 索引变量
	@dwGroupID   INT,							-- 俱乐部ID
	@dwUserID INT,								-- 修改玩家ID
	@dwConfigID	 INT,							-- 玩法ID
	@strConfigName NVARCHAR(12),				-- 玩法名称
	@wKindID     INT,							-- 类型标识
	@cbPlayMode	 TINYINT,						-- 结算模式
	@lCellScore BIGINT,							-- 单元积分
	@dwDrawCountLimit BIGINT,					-- 局数限制
	@dwDrawTimeLimit BIGINT,					-- 时间限制
	@wJoinGamePeopleCount INT,					-- 参与人数
	@cbPassword	TINYINT,						-- 密码模式
	@cbPayMode	TINYINT,						-- 支付模式
	@cbGameMode	TINYINT,						-- 游戏模式
	@cbJoinMode	TINYINT,						-- 参与模式
	@lEnterScore BIGINT,						-- 进入分
	@lLeaveScore BIGINT,						-- 离开分	
	@cbTaxMode	TINYINT,						-- 服务比例模式
	@lTaxCount  BIGINT,							-- 服务比例数值
	@cbLessScoreDismiss  TINYINT,				-- 分数不够解散
	@strGameRule NVARCHAR(200),					-- 定制规则

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
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

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

	-- 检查权限
	DECLARE @MemberRight INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF (@MemberRight&0x01) <> 0
		SET @HaveRight=1
	ELSE
	BEGIN

		IF (@MemberRight&0x02) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
			IF (@ManagerRight&0x40)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您没有权限修改玩法！'
		RETURN 4		
	END

	DECLARE @ConfigID INT
	SELECT @ConfigID=ConfigID FROM IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID
	IF @ConfigID IS NULL
	BEGIN
		SET @strErrorDescribe=N'玩法不存在，请检查配置！'
		RETURN 5	
	END

	IF @cbPlayMode=1 AND NOT EXISTS(SELECT 1 FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND CanUseMedal=1 )
	BEGIN
		SET @strErrorDescribe=N'抱歉，该大联盟无权设置勋章玩法！'
		RETURN 6	
	END

	DECLARE @valueMin INT
	DECLARE @valueMax INT
	IF @cbTaxMode < 3
	BEGIN

		SELECT @valueMin= OptionValue FROM IMGroupOption WHERE OptionName='FixedMin'
		IF @valueMin IS NULL
			SET @valueMin=0
		
		SELECT @valueMax= OptionValue FROM IMGroupOption WHERE OptionName='FixedMax'
		IF @valueMax IS NULL
			SET @valueMax=100
	END
	ELSE
	BEGIN
		SELECT @valueMin= OptionValue FROM IMGroupOption WHERE OptionName='PercentMin'
		IF @valueMin IS NULL
			SET @valueMin=0
		
		SELECT @valueMax= OptionValue FROM IMGroupOption WHERE OptionName='PercentMax'
		IF @valueMax IS NULL
			SET @valueMax=100	
	END

	IF @lTaxCount<@valueMin OR @lTaxCount>@valueMax
	BEGIN
		SET @strErrorDescribe=N'创建失败， 贡献数值范围'+CONVERT(NVARCHAR(16),@valueMin) +'-'+CONVERT(NVARCHAR(16),@valueMax)+'！'
		RETURN 7		
	END

	-- 修改玩法
	UPDATE IMGroupConfig SET ConfigName=@strConfigName,PlayMode=@cbPlayMode, CellScore=@lCellScore, DrawCountLimit=@dwDrawCountLimit, DrawTimeLimit=@dwDrawTimeLimit, PasswordMode=@cbPassword,
	PayMode=@cbPayMode,GameMode=@cbGameMode,JoinMode=@cbJoinMode,EnterScore=@lEnterScore,LeaveScore=@lLeaveScore,GameRule=@strGameRule,TaxMode=@cbTaxMode,TaxCount=@lTaxCount,LessScoreDismiss=@cbLessScoreDismiss
	 WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID

	Select KindID,TimeSecond From IMGroupConfig  WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除玩法
CREATE  PROCEDURE dbo.GSP_IM_DeleteConfig
	@dwGroupID INT,								-- 群组标识
	@dwUserID INT,								-- 用户标识
	@dwConfigID INT,							-- 玩法标识
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
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

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

	-- 检查权限
	DECLARE @MemberRight INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF (@MemberRight&0x01) <> 0
		SET @HaveRight=1
	ELSE
	BEGIN

		IF (@MemberRight&0x02) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
			IF (@ManagerRight&0x40)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您没有权限修改玩法！'
		RETURN 4		
	END

	DECLARE @ConfigID INT
	SELECT @ConfigID=ConfigID FROM IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID
	IF @ConfigID IS NULL
	BEGIN
		SET @strErrorDescribe=N'玩法不存在，请检查！'
		RETURN 5	
	END

	DELETE FROM IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID
	--清除此玩法的收藏
	UPDATE IMGroupMember SET FavoriteConfig=0 WHERE GroupID=@dwGroupID AND FavoriteConfig=@dwConfigID
END

RETURN 0

GO



----------------------------------------------------------------------------------------------------


-- 删除玩法
CREATE  PROCEDURE dbo.GSP_IM_QueryCreate
	@dwGroupID INT,								-- 群组标识
	@dwConfigID INT,							-- 玩法标识
	@dwUserID INT,								-- 用户标识
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
		-- 查询配置
	DECLARE @MaxNullTableCount SMALLINT		
	SELECT @MaxNullTableCount=OptionValue FROM IMGroupOption(NOLOCK) WHERE OptionName=N'MaxNullTableCount'
	IF @MaxNullTableCount IS NULL SET @MaxNullTableCount=5

	Select @MaxNullTableCount AS MaxNullTableCount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 收藏玩法
CREATE  PROCEDURE dbo.GSP_IM_SetFavoriteConfig
	@dwGroupID INT,								-- 群组标识
	@dwUserID INT,								-- 用户标识
	@dwConfigID INT,							-- 玩法标识
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	IF NOT EXISTS(SELECT 1 FROM IMGroupProperty WHERE GroupID=@dwGroupID)
	BEGIN
		SET @strErrorDescribe=N'大联盟信息错误，请检查！'
		RETURN 1	
	END

	IF NOT EXISTS(SELECT 1 FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe=N'您不是该大联盟成员，设置失败！'
		RETURN 2	
	END

	IF NOT EXISTS(SELECT 1 FROM IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID)
	BEGIN
		SET @strErrorDescribe=N'玩法不存在，请检查！'
		RETURN 3	
	END
	
	UPDATE IMGroupMember SET FavoriteConfig=@dwConfigID WHERE GroupID=@dwGroupID AND UserID=@dwUserID

END

RETURN 0

GO

