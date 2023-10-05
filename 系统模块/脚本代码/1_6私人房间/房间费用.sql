USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CreateTableFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CreateTableFee]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_AICreateTableFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_AICreateTableFee]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CreateTableQuit]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CreateTableQuit]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_InsertCreateRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_InsertCreateRecord]


---加载私人房间信息
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_QueryPersonalRoomInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_QueryPersonalRoomInfo]
GO

---支付钻石
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_PayRoomCard]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_PayRoomCard]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 创建判断
CREATE PROC GSP_GR_CreateTableFee
	@dwUserID INT,								-- 用户 I D
	@dwServerID INT,							-- 房间 I D
	@dwDrawCountLimit INT,						-- 时间限制
	@dwDrawTimeLimit INT,						-- 局数限制
	@cbPayMode		TINYINT,					-- 创建方式	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@cbPlayMode		TINYINT,					-- 游戏模式 0 积分场  1 金币场
	@dwGroupID		INT,						-- 大联盟ID
	@dwConfigID		INT,						-- 玩法ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	DECLARE @ReturnValue INT
	DECLARE @dUserBeans DECIMAL(18,2)
	DECLARE @dCurBeans DECIMAL(18,2)
	DECLARE @Fee INT
	DECLARE @AAFee INT
	DECLARE @lDiamond bigint
	DECLARE @cbIsJoinGame TINYINT
	DECLARE @MemberOrder TINYINT
	DECLARE @CreateRight TINYINT
	DECLARE @iCanCreateCount INT
	DECLARE @szConfigName nvarchar(31)
	DECLARE @cbConfigTaxMode TINYINT
	DECLARE @dwConfigTaxCount INT
	DECLARE @wKindID INT
	DECLARE @cbLessScoreDismiss TINYINT
	SELECT  @wKindID = KindID FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID = @dwServerID 
	-- 查询费用 及 房主是否参与游戏
	SELECT  @CreateRight = CreateRight FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 

	--用户钻石	
	SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID

		-- 查询锁定
	DECLARE @LockServerID INT
	SELECT @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

		-- 锁定判断
	IF  @LockServerID IS NOT NULL and @LockServerID<>@dwServerID
	BEGIN
		-- 查询信息
		DECLARE @ServerName NVARCHAR(31)
		SELECT @ServerName=ServerName FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

		-- 错误信息
		IF @ServerName IS NULL SET @ServerName=N'未知房间'
		SET @strErrorDescribe=N'您正在 [ '+@ServerName+N' ] 游戏房间中，不能在游戏的同时创建房间！'
		RETURN 2
	END

	--加入方式
	SELECT  @cbIsJoinGame = IsJoinGame,@iCanCreateCount=CanCreateCount FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 
		
	-- 查询费用
	SELECT @Fee=TableFee,@AAFee = AAPayFee FROM  WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID AND PlayMode = @cbPlayMode
	
	IF @cbPlayMode = 0 OR @dwGroupID <>0
	BEGIN
		IF @Fee IS NULL OR @Fee=0
		BEGIN
			SET @strErrorDescribe=N'数据库查询费用失败，请重新尝试！'
			RETURN 3
		END
	END

	--大联盟支付方式
	DECLARE @cbPayType TINYINT
	SET @cbPayType=0
	--查询大联盟
	IF @dwGroupID<>0
	BEGIN
	
		DECLARE @TmpGroupID INT 
		SELECT @TmpGroupID=GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID=@dwGroupID
		IF @TmpGroupID IS NULL
		BEGIN
			SET @strErrorDescribe=N'查询大联盟失败，无法创建大联盟房间，请重新尝试！'
			RETURN 4
		END
		
		--查询成员
		SELECT @TmpGroupID=GroupID FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID = @dwGroupID
		IF @TmpGroupID IS NULL
		BEGIN
			SET @strErrorDescribe=N'你不是该大联盟成员，无法创建大联盟房间！'
			RETURN 5
		END
		
		--验证支付
		SELECT @cbPayType=PayType FROM   WHQJGroupDB.dbo.IMGroupBattleRule WHERE GroupID=@dwGroupID
		IF @cbPayType IS NULL SET @cbPayType=1
		
		
		IF @cbPayMode=1 AND @cbPayType=1
		BEGIN
			SET @strErrorDescribe=N'大联盟不支持玩家支付，请重新选择！'
			RETURN 6
		END
				
		SELECT @szConfigName=ConfigName,@cbConfigTaxMode=TaxMode,@dwConfigTaxCount=TaxCount,@cbLessScoreDismiss=LessScoreDismiss FROM   WHQJGroupDB.dbo.IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID = @dwConfigID
		IF @cbConfigTaxMode IS NULL
		BEGIN
			SET @strErrorDescribe=N'该大联盟不存在此玩法，请重新选择！'
			RETURN 7
		END
	END
	
	IF @cbConfigTaxMode IS NULL
	BEGIN
		SET @cbConfigTaxMode=0
		SET @dwConfigTaxCount=0
		SET @szConfigName=N''
		SET @cbLessScoreDismiss=0
	END

	--查询参与创建所有房间需要的钻石
	DECLARE @MaxUserPay INT
	DECLARE @MaxGroupPay INT	

	IF @dwGroupID=0
	BEGIN
		SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID

		SELECT @MaxUserPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @dwUserID AND GroupID=0 AND RoomStatus <> 2
		SELECT @MaxGroupPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE RoomStatus <> 2 
		AND GroupID in(Select GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty where CreaterID=@dwUserID) 
	END
	ELSE
	BEGIN
		DECLARE @GroupCreaterID INT
		Select @GroupCreaterID=CreaterID FROM   WHQJGroupDB.dbo.IMGroupProperty where GroupID=@dwGroupID
		IF @GroupCreaterID IS NULL
		BEGIN
			SET @strErrorDescribe=N'大联盟信息查询失败，请检查！'
			RETURN 1
		END

		SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID = @GroupCreaterID

		SELECT @MaxUserPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @GroupCreaterID AND GroupID=0 AND RoomStatus <> 2

		SELECT @MaxGroupPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE RoomStatus <> 2 
		AND GroupID in(Select GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty where CreaterID=@GroupCreaterID) 
	END
	
	IF @lDiamond IS NULL SET @lDiamond=0
	IF @MaxUserPay IS NULL SET @MaxUserPay=0
	IF @MaxGroupPay IS NULL	SET @MaxGroupPay=0

	-- 普通金币约战模式不需要消耗钻石
	IF @cbPlayMode = 1 AND @dwGroupID=0
	BEGIN
		IF @cbIsJoinGame IS NULL  SET @cbIsJoinGame = 0
		IF @AAFee IS NULL  SET @AAFee = 0
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@AAFee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
		RETURN 0
	END
	
	--是否AA制
	IF @cbPayMode = 1
	BEGIN
		IF @lDiamond  < @MaxUserPay+@MaxGroupPay + @AAFee
		BEGIN
			SET @strErrorDescribe=N'用户钻石不足，请购买！'
			RETURN 1
		END
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@AAFee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
	END
	ELSE 
	BEGIN
		IF @lDiamond  < @MaxUserPay+@MaxGroupPay + @Fee
		BEGIN
			IF @cbPayType=1
				SET @strErrorDescribe=N'盟主钻石不足，请联系盟主充值！'
			ELSE
			 SET @strErrorDescribe=N'用户钻石不足，请购买！'
			
			RETURN 1
		END
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@Fee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
	END
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 创建判断
CREATE PROC GSP_GR_AICreateTableFee
	@dwUserID INT,								-- 用户 I D
	@dwServerID INT,							-- 房间 I D
	@dwDrawCountLimit INT,						-- 时间限制
	@dwDrawTimeLimit INT,						-- 局数限制
	@cbPayMode		TINYINT,					-- 创建方式	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@cbPlayMode		TINYINT,					-- 游戏模式 0 积分场  1 金币场
	@dwGroupID		INT,						-- 大联盟ID
	@dwConfigID		INT,						-- 玩法ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	DECLARE @ReturnValue INT
	DECLARE @dUserBeans DECIMAL(18,2)
	DECLARE @dCurBeans DECIMAL(18,2)
	DECLARE @Fee INT
	DECLARE @AAFee INT
	DECLARE @lDiamond bigint
	DECLARE @cbIsJoinGame TINYINT
	DECLARE @MemberOrder TINYINT
	DECLARE @CreateRight TINYINT
	DECLARE @iCanCreateCount INT
	DECLARE @szConfigName nvarchar(31)
	DECLARE @cbConfigTaxMode TINYINT
	DECLARE @dwConfigTaxCount INT
	DECLARE @wKindID INT
	DECLARE @cbLessScoreDismiss TINYINT
	SELECT  @wKindID = KindID FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID = @dwServerID 
	-- 查询费用 及 房主是否参与游戏
	SELECT  @CreateRight = CreateRight FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 

	--用户钻石	
	SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID



	--加入方式
	SELECT  @cbIsJoinGame = IsJoinGame,@iCanCreateCount=CanCreateCount FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 
		
	-- 查询费用
	SELECT @Fee=TableFee,@AAFee = AAPayFee FROM  WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID AND PlayMode = @cbPlayMode
	
	IF @cbPlayMode = 0 OR @dwGroupID <>0
	BEGIN
		IF @Fee IS NULL OR @Fee=0
		BEGIN
			SET @strErrorDescribe=N'数据库查询费用失败，请重新尝试！'
			RETURN 3
		END
	END

	--大联盟支付方式
	DECLARE @cbPayType TINYINT
	SET @cbPayType=0
	--查询大联盟
	IF @dwGroupID<>0
	BEGIN
	
		DECLARE @TmpGroupID INT 
		SELECT @TmpGroupID=GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID=@dwGroupID
		IF @TmpGroupID IS NULL
		BEGIN
			SET @strErrorDescribe=N'查询大联盟失败，无法创建大联盟房间，请重新尝试！'
			RETURN 4
		END
		
		--查询成员
		SELECT @TmpGroupID=GroupID FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID = @dwGroupID
		IF @TmpGroupID IS NULL
		BEGIN
			SET @strErrorDescribe=N'你不是该大联盟成员，无法创建大联盟房间！'
			RETURN 5
		END
		
		--验证支付
		SELECT @cbPayType=PayType FROM   WHQJGroupDB.dbo.IMGroupBattleRule WHERE GroupID=@dwGroupID
		IF @cbPayType IS NULL SET @cbPayType=1
		
		
				
		SELECT @szConfigName=ConfigName,@cbConfigTaxMode=TaxMode,@dwConfigTaxCount=TaxCount,@cbLessScoreDismiss=LessScoreDismiss FROM   WHQJGroupDB.dbo.IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID = @dwConfigID
		IF @cbConfigTaxMode IS NULL
		BEGIN
			SET @strErrorDescribe=N'该大联盟不存在此玩法，请重新选择！'
			RETURN 7
		END
	END
	
	IF @cbConfigTaxMode IS NULL
	BEGIN
		SET @cbConfigTaxMode=0
		SET @dwConfigTaxCount=0
		SET @szConfigName=N''
		SET @cbLessScoreDismiss=0
	END
	--查询参与创建所有房间需要的钻石
	DECLARE @MaxPay INT	
	IF @dwGroupID=0
		SELECT @MaxPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @dwUserID AND RoomStatus <> 2
	ELSE
	BEGIN
		SELECT @MaxPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE GroupID = @dwGroupID AND RoomStatus <> 2
	END
	IF @MaxPay IS NULL
	SET @MaxPay=0
	
	-- 获取钻石	
	SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID = (Select CreaterID FROM   WHQJGroupDB.dbo.IMGroupProperty where GroupID=@dwGroupID)

	
	IF @lDiamond IS NULL SET @lDiamond=0

	-- 普通金币约战模式不需要消耗钻石
	IF @cbPlayMode = 1 AND @dwGroupID=0
	BEGIN
		IF @cbIsJoinGame IS NULL  SET @cbIsJoinGame = 0
		IF @AAFee IS NULL  SET @AAFee = 0
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@AAFee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
		RETURN 0
	END
	
	
	--是否AA制
	IF @cbPayMode = 1
	BEGIN
		IF @lDiamond  < @MaxPay + @AAFee
		BEGIN
			SET @strErrorDescribe=N'用户钻石不足，请购买！'
			RETURN 1
		END
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@AAFee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
	END
	ELSE 
	BEGIN
		IF @lDiamond  < @MaxPay + @Fee
		BEGIN
			IF @cbPayType=1
				SET @strErrorDescribe=N'盟主钻石不足，请联系盟主充值！'
			ELSE
			 SET @strErrorDescribe=N'用户钻石不足，请购买！'
			
			RETURN 1
		END
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@Fee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
	END
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------

-- 退还费用
CREATE PROC GSP_GR_CreateTableQuit
	@dwUserID INT,								-- 用户 I D
	@dwPersonalRoomID INT,						-- 约战房间ID
	@dwServerID INT,							-- 房间标识
	@dwDrawCountLimit INT,						-- 时间限制
	@dwDrawTimeLimit INT,						-- 局数限制
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	DECLARE @ReturnValue INT
	SET @ReturnValue=0

	DECLARE @lDiamond bigint
	DECLARE @cbJoin TINYINT

	DECLARE @wKindID INT
	SELECT  @wKindID = KindID FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID = @dwServerID 
	-- 查询费用 及 房主是否参与游戏
	SELECT @cbJoin = IsJoinGame FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 

	-- 更新记录
	UPDATE  WHQJPlatformDB..StreamCreateTableFeeInfo   SET  DissumeDate = GETDATE(), RoomStatus = 2  WHERE RoomID = @dwPersonalRoomID AND UserID = @dwUserID

	--必须参与游戏房主解锁
	IF @cbJoin = 1
	BEGIN
		DELETE FROM GameScoreLocker WHERE UserID = @dwUserID
	END

	--用户钻石	
	SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID
	IF @lDiamond IS NULL SET @lDiamond=0

	SELECT @lDiamond AS Diamond

END

RETURN @ReturnValue
GO


-----------------------------------------------------------------------
-- 创建房间记录
CREATE PROC GSP_GR_InsertCreateRecord
	@dwUserID INT,								-- 用户 I D
	@dwServerID INT,							-- 房间 标识
	@dwPersonalRoomID INT,						-- 约战房间标识
	@lCellScore INT,							-- 房间 底分
	@dwDrawCountLimit INT,						-- 局数限制
	@dwDrawTimeLimit INT,						-- 时间限制
	@szPassWord NVARCHAR(15),					-- 连接地址
	@wJoinGamePeopleCount TINYINT,				-- 人数限制
	@dwRoomTax BIGINT,							-- 私人房间服务比例
	@strClientAddr NVARCHAR(15),
	@cbPayMode TINYINT,							-- 支付模式
	@lNeedRoomCard TINYINT,						-- 理论需要的钻石
	@cbGameMode TINYINT,						-- 游戏模式
	@cbPlayMode		TINYINT,					-- 游戏模式 0 积分场  1 金币场
	@lGroupID		BIGINT,						-- 大联盟ID
	@lMinEnterScore BIGINT,						-- 最低入场分
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	DECLARE @Fee INT
	DECLARE @Nicname NVARCHAR(31)
	DECLARE @lPersonalRoomTax BIGINT

	-- 查询费用 及 房主是否参与游戏
	DECLARE @wKindID INT
	SELECT  @wKindID = KindID FROM  WHQJPlatformDB.dbo.GameRoomInfo  WHERE ServerID = @dwServerID 
	-- 查询费用
	SELECT @Fee=TableFee FROM  WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID AND  PlayMode= @cbPlayMode
	IF @cbPlayMode = 0 OR @lGroupID<>0
	BEGIN
		IF @Fee IS NULL OR @Fee=0
		BEGIN
			SET @strErrorDescribe=N'数据库查询费用失败，请重新尝试！'
			RETURN 3
		END
	END
	ELSE
	BEGIN
		SET @Fee = 0
	END

	-- 获取创建房间玩家的昵称
	SELECT @Nicname =NickName FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID = @dwUserID
	IF @Nicname IS NULL
	SET @Nicname =''
		
	SELECT  @lPersonalRoomTax = PersonalRoomTax FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 
	
	--如果是消耗钻石，查询代理服务比例
	DECLARE @lTaxAgency BIGINT
	SELECT  @lTaxAgency = AgentScale FROM  WHQJAccountsDB.dbo.AccountsAgent WHERE UserID = @dwUserID 
	IF @lTaxAgency IS NOT NULL
	BEGIN
		SET @lPersonalRoomTax = @lTaxAgency
	END

	-- 写入消耗记录
	INSERT INTO WHQJPlatformDB..StreamCreateTableFeeInfo(UserID,NickName, ServerID, RoomID, CellScore, CountLimit,TimeLimit,CreateTableFee,CreateDate, TaxAgency, JoinGamePeopleCount,PayMode, RoomStatus, NeedRoomCard,GameMode,PlayMode,GroupID)
												VALUES(@dwUserID,@Nicname, @dwServerID, @dwPersonalRoomID, @lCellScore,@dwDrawCountLimit, @dwDrawTimeLimit, 0,GETDATE(), @dwRoomTax, @wJoinGamePeopleCount, @cbPayMode, 0, @lNeedRoomCard,@cbGameMode,@cbPlayMode,@lGroupID)	
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------------
-- 请求约战房间信息
CREATE  PROCEDURE dbo.GSP_GS_QueryPersonalRoomInfo
	@dwPersonalRoomID INT,							-- 约战房间标识
	@dwRoomHostID INT,									-- 房主ID
	@strErrorDescribe NVARCHAR(127) OUTPUT				-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	DECLARE @dwUserID			INT
	DECLARE @dwServerID			INT
	DECLARE @dwKindID			INT
	DECLARE @Nicname 			NVARCHAR(31)
	DECLARE @dwPlayTurnCount 	INT
	DECLARE @dwPlayTimeLimit 	INT
	DECLARE @cbIsDisssumRoom 	TINYINT
	DECLARE @sysCreateTime 		DATETIME
	DECLARE @sysDissumeTime 	DATETIME
	DECLARE @lTaxCount 			BIGINT
	DECLARE @lCreateFee 		TINYINT
	DECLARE @bnryRoomScoreInfo 	varbinary(MAX)
	DECLARE @cbPayMode 			TINYINT
	DECLARE @lNeedRoomCard 		TINYINT
	DECLARE @lJoinPeopleCount 	TINYINT
	DECLARE @cbRoomStatus		TINYINT
	DECLARE @cbGameMode 			TINYINT
	-- 加载房间
	SELECT @dwUserID = UserID, @dwPlayTurnCount=CountLimit, @dwPlayTimeLimit = TimeLimit, @sysCreateTime = CreateDate, @sysDissumeTime = DissumeDate, @lTaxCount = TaxRevenue, @lCreateFee = CreateTableFee,
	  @bnryRoomScoreInfo = RoomScoreInfo, @cbPayMode = PayMode, @lNeedRoomCard = NeedRoomCard, @lJoinPeopleCount = JoinGamePeopleCount, @cbRoomStatus = RoomStatus,@dwServerID=ServerID
	  ,@cbGameMode = GameMode
	FROM WHQJPlatformDB..StreamCreateTableFeeInfo WHERE RoomID = @dwPersonalRoomID AND UserID = @dwRoomHostID
	IF @sysDissumeTime IS NULL
	BEGIN
		SET @cbIsDisssumRoom = 0
		SET @sysDissumeTime = @sysCreateTime
	END
	ELSE
	BEGIN
		SET @cbIsDisssumRoom = 1
	END
	--获取游戏ID
	IF @dwServerID IS NOT NULL	SELECT @dwKindID=KindID FROM WHQJPlatformDB..GameRoomInfo WHERE ServerID=@dwServerID
	IF @dwKindID IS NULL SET @dwKindID=0
	
	declare @strRoomScoreInfo varchar(8000),@i int
	select @strRoomScoreInfo='',@i=datalength(@bnryRoomScoreInfo)
	while @i>0
		select @strRoomScoreInfo=substring('0123456789ABCDEF',substring(@bnryRoomScoreInfo,@i,1)/16+1,1)
				+substring('0123456789ABCDEF',substring(@bnryRoomScoreInfo,@i,1)%16+1,1)
				+@strRoomScoreInfo
			,@i=@i-1

	-- 获取玩家昵称
	SELECT @Nicname =NickName FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID = @dwUserID
	IF @Nicname IS NULL
	SET @Nicname =''


	if @dwPlayTurnCount is null
	set @dwPlayTurnCount=0

	if @dwPlayTimeLimit is null
	set @dwPlayTimeLimit=0

	if @cbIsDisssumRoom is null
	set @cbIsDisssumRoom=0

	if @sysDissumeTime is null
	set @sysDissumeTime=GETDATE()

	if @sysCreateTime is null
	set @sysCreateTime=GETDATE()

	if @lTaxCount is null
	set @lTaxCount=0	

	if @lCreateFee is null
	set @lCreateFee=0

	SELECT @Nicname AS UserNicname, @dwPlayTurnCount AS dwPlayTurnCount, @dwPlayTimeLimit AS dwPlayTimeLimit, @cbIsDisssumRoom AS cbIsDisssumRoom, @sysCreateTime AS sysCreateTime, 
	@sysDissumeTime AS sysDissumeTime, @lTaxCount AS lTaxCount, @lCreateFee AS CreateTableFee,@bnryRoomScoreInfo AS RoomScoreInfo, @strRoomScoreInfo AS strRoomScoreInfo,
	@cbPayMode AS PayMode, @lNeedRoomCard AS NeedRoomCard, @lJoinPeopleCount AS JoinGamePeopleCount, @cbRoomStatus AS RoomStatus,@dwKindID AS wKindID,@cbGameMode AS GameMode

END

RETURN 0

GO

-------------------------------------
-----------------------------------------------------------------------
-- 创建房间记录
CREATE PROC GSP_GS_PayRoomCard
	@cbPayMode	INT,							-- 支付方式
	@dwUserID INT,								-- 用户 I D
	@dwKindID INT,								-- 房间 标识
	@dwPersonalRoomID INT,						-- 约战房间ID
	@dwDrawCountLimit INT,						-- 局数限制
	@dwDrawTimeLimit INT,						-- 时间限制
	@strClientAddr NVARCHAR(15),				-- 客户端地址
	@dwGroupID	INT,							-- 大联盟ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN		
	DECLARE @Fee INT
	DECLARE @Nicname NVARCHAR(31)
	DECLARE @lPersonalRoomTax BIGINT
	DECLARE @ReturnValue INT

	-- 查询费用 及 房主是否参与游戏	
	DECLARE @wKindID INT

	-- 查询费用
	IF @cbPayMode=1
	BEGIN
		SELECT @Fee=AAPayFee FROM WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @dwKindID 
	END
	ELSE
	BEGIN
		SELECT @Fee=TableFee FROM WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @dwKindID 
	END
	
	IF @Fee IS NULL OR @Fee=0
	BEGIN
		SET @strErrorDescribe=N'数据库查询费用失败，请重新尝试！'
		RETURN 3
	END

	--	查询消耗方式
	SELECT @lPersonalRoomTax = PersonalRoomTax FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 	

	--查询大联盟消费方式
	DECLARE @cbPayType TINYINT
	SET @cbPayType=0
	IF @dwGroupID<>0
	BEGIN
		SELECT @cbPayType=PayType FROM   WHQJGroupDB.dbo.IMGroupBattleRule WHERE GroupID=@dwGroupID
		IF @cbPayType IS NULL SET @cbPayType=1
	END
	
	--查询钻石
	DECLARE @lDiamond BIGINT
	IF @cbPayType<>1
	BEGIN 
		SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID
		IF @lDiamond IS NULL
		BEGIN
			SET @strErrorDescribe=N'用户钻石不足，请充值！'
			RETURN 1
		END
	END
	ELSE
	BEGIN
		SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=(SELECT CreaterID FROM WHQJGroupDB..IMGroupProperty WHERE GroupID=@dwGroupID)
		IF @lDiamond IS NULL
		BEGIN
			SET @strErrorDescribe=N'盟主钻石不足，请联系盟主充值！'
			RETURN 1
		END
		
	END


	-- 写入费用
	IF @lDiamond < @Fee
	BEGIN
		SET @strErrorDescribe=N'您的钻石不足，请先充值。'
		RETURN 4
	END	
	ELSE
	BEGIN
		IF @cbPayType<>1
		BEGIN
			UPDATE WHQJTreasureDB..UserCurrency SET Diamond=@lDiamond-@Fee WHERE UserID=@dwUserID					
		
			--插入日志
			DECLARE @DateTime DATETIME
			DECLARE @SerialNumber NVARCHAR(20)
			SET @DateTime = GETDATE()
			SELECT @SerialNumber=dbo.WF_GetSerialNumber()
		
			--一人付费制
			IF @cbPayMode = 0
			BEGIN
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,10,@lDiamond,-@Fee,@strClientAddr,@DateTime)
			END
			ELSE
			BEGIN
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,11,@lDiamond,-@Fee,@strClientAddr,@DateTime)
			END
		END
		ELSE
		BEGIN
			-- 更新财富信息
			UPDATE WHQJTreasureDB..UserCurrency SET Diamond=@lDiamond-@Fee WHERE UserID=(SELECT CreaterID FROM WHQJGroupDB..IMGroupProperty WHERE GroupID=@dwGroupID)
			
			--插入记录
			INSERT INTO   WHQJGroupDB.dbo.RecordGroupIngotChange(GroupID,BeforeIngot,TradeType,TradeIngot,TradeRoomGUID,CollectDate) 
			VALUES(@dwGroupID,@lDiamond,3,-@Fee,@dwPersonalRoomID,GETDATE())
			
			
			
			--更新用户约战次数（盟主不在统计内）
			IF  EXISTS ( SELECT 1 FROM   WHQJGroupDB.dbo.StreamGroupBalance WHERE GroupID=@dwGroupID AND UserID=@dwUserID)
			BEGIN
				UPDATE   WHQJGroupDB.dbo.StreamGroupBalance SET BattleCount=BattleCount+1 WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			END
			ELSE
			BEGIN			
				INSERT INTO   WHQJGroupDB.dbo.StreamGroupBalance(GroupID,UserID,BattleCount,BalanceCount,CollectDate) 
				VALUES(@dwGroupID,@dwUserID,1,0,GETDATE())
			END
			
			--更新成员约战次数
			--UPDATE   WHQJGroupDB.dbo.IMGroupMember SET BattleCount=BattleCount+1 WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		END

	END			
	
	--一人付费制
	IF @cbPayMode = 0
	BEGIN
		UPDATE WHQJPlatformDB..StreamCreateTableFeeInfo SET CreateTableFee = @Fee,NeedRoomCard=0 WHERE RoomID = @dwPersonalRoomID AND UserID = @dwUserID
	END

	
	SELECT @lDiamond-@Fee AS Diamond,@Fee AS Fee 
END

RETURN 0
GO
