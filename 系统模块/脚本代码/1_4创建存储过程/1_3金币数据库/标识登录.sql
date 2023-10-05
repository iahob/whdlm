
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyUserID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyUserID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyMobile]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyMobile]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_GetOfflineInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_GetOfflineInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyAI]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyAI]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- I D 登录
CREATE PROC GSP_GR_EfficacyUserID
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

-- 附属信息
DECLARE @QQ NVARCHAR(16)
DECLARE @EMail NVARCHAR(16)
DECLARE @SeatPhone NVARCHAR(32)
DECLARE @MobilePhone NVARCHAR(16)
DECLARE @DwellingPlace NVARCHAR(128)
DECLARE @PostalCode NVARCHAR(8)
DECLARE @DayPlayTime INT
DECLARE @DayWinLostScore BIGINT

-- 扩展信息
DECLARE @GameID INT
DECLARE @GroupID INT
DECLARE @UserRight INT
DECLARE @Gender TINYINT
DECLARE @MasterRight INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberOrder SMALLINT
DECLARE @GroupName NVARCHAR(31)

-- 积分变量
DECLARE @Score BIGINT
DECLARE @Grade BIGINT
DECLARE @Insure BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @AgentID AS INT
DECLARE @IntegralCount BIGINT 
-- 执行逻辑
BEGIN

	-- 辅助变量
	DECLARE @EnjoinLogon INT

	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏登录权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的游戏登录权限，请联系客户服务中心了解详细情况！'
		RETURN 7
	END
 
	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @DynamicPass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE @SpreaderID INT
	SET @AgentID = 0	
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName,@SpreaderID = SpreaderID, @UnderWrite=UnderWrite, @LogonPass=DynamicPass,@FaceID=FaceID, @CustomID=CustomID,
		@Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@UserRight=UserRight,
		@MasterRight=MasterRight, @MasterOrder=MasterOrder, @MemberOrder=MemberOrder, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,@AgentID=AgentID
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	--查询代理
	SELECT @AgentID = UserID FROM  WHQJAccountsDB.dbo.AccountsAgent  where UserID = @SpreaderID

	-- 附属信息
    SET @QQ =N''
    SET @EMail =N''
    SET @SeatPhone =N''
    SET @MobilePhone =N''
    SET @DwellingPlace =N''
    SET @PostalCode =N''
    SELECT @QQ = QQ,@EMail = EMail,@SeatPhone = SeatPhone,@MobilePhone = MobilePhone,@DwellingPlace = DwellingPlace ,@PostalCode =PostalCode 
    FROM  WHQJAccountsDB.dbo.IndividualDatum WHERE UserID=@dwUserID
    
	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
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
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！'
		RETURN 2
	END	
	
	-- 固定机器
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 2
		END
	END

	-- 密码判断
	IF @LogonPass IS NULL OR @strPassword IS NULL OR @LogonPass<>@strPassword AND @strClientIP<>N'0.0.0.0'
	BEGIN
		SET @strErrorDescribe=N'抱歉,由于您的帐号在其它地方登录,请重新登录平台后再次尝试！'
		RETURN 3
	END

	-- 固定机器
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SET @strErrorDescribe=N'您的帐号成功使用了固定机器登录功能！'
		UPDATE  WHQJAccountsDB.dbo.AccountsInfo SET MoorMachine=@MoorMachine, LastLogonMachine=@strMachineID WHERE UserID=@UserID
	END

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 游戏信息
	DECLARE @AllLogonTimes INT
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT
	SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount,
		@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder, @AllLogonTimes=AllLogonTimes
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 信息判断
	IF @Score IS NULL
	BEGIN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @strClientIP, @strMachineID, @strClientIP, @strMachineID)

		-- 游戏信息
		SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount,
			@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterOrder=MasterOrder, @GameMasterRight=MasterRight, @AllLogonTimes=AllLogonTimes
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- 异常判断
	IF @Score<0
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'抱歉地通知你，您的游戏币数据出现了异常情况，请联系客户服务中心了解详细情况！'
		RETURN 14
	END

	-- 查询锁定
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		-- 锁定解除
		DELETE GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID

		-- 查询锁定
		DECLARE @LockKindID INT
		DECLARE @LockServerID INT
		SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

		-- 锁定判断
		IF @LockKindID IS NOT NULL AND @LockServerID IS NOT NULL
		BEGIN

			-- 结束事务
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			-- 查询类型
			IF @LockKindID<>0
			BEGIN
				-- 查询信息
				DECLARE @KindName NVARCHAR(31)
				DECLARE @ServerName NVARCHAR(31)
				SELECT @KindName=KindName FROM  WHQJPlatformDB.dbo.GameKindItem WHERE KindID=@LockKindID
				SELECT @ServerName=ServerName FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

				-- 错误信息
				IF @KindName IS NULL SET @KindName=N'未知游戏'
				IF @ServerName IS NULL SET @ServerName=N'未知房间'
				SET @strErrorDescribe=N'您正在 [ '+@KindName+N' ] 的 [ '+@ServerName+N' ] 游戏房间中，不能同时再进入此游戏房间！'
				RETURN 4

			END
			ELSE
			BEGIN
				-- 提示消息
				SELECT [ErrorDescribe]=N'您正在进行保险柜处理过程中，暂时无法进入此游戏房间！'
				RETURN 4
			END
		END
	END

	-- 保留变量
	SET @Grade=0
	SET @GroupID=0
	SET @GroupName=''

	-- 权限标志
	SET @UserRight=@UserRight|@GameUserRight
	SET @UserRight=@UserRight&~256
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- 权限等级
	IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder

	-- 进入记录
	INSERT RecordUserInout (UserID, EnterScore, EnterGrade, EnterInsure, KindID, ServerID, EnterClientIP, EnterMachine)
	VALUES (@UserID, @Score, @Grade, @Insure,  @wKindID, @wServerID, @strClientIP, @strMachineID)

	-- 记录标识
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- 插入锁表
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		-- 插入锁表
		INSERT GameScoreLocker (UserID, ServerID, KindID, EnterID, EnterIP, EnterMachine) VALUES (@dwUserID, @wServerID, @wKindID, @InoutIndex, @strClientIP, @strMachineID)
		IF @@ERROR<>0
		BEGIN
			-- 结束事务
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			-- 错误信息
			SET @strErrorDescribe=N'抱歉地通知你，游戏币锁定操作失败，请联系客户服务中心了解详细情况！'
			RETURN 14
		END
	END

	--更新信息
	UPDATE GameScoreInfo SET AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), 
		LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID WHERE UserID=@dwUserID

	-- 登录统计
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 插入记录
	IF @AllLogonTimes>0
	BEGIN
		UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END
	ELSE
	BEGIN
		UPDATE SystemStreamInfo SET RegisterCount=RegisterCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, RegisterCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END
		
	--查询每日
	SELECT @DayPlayTime=DayPlayTimeCount FROM WHQJAccountsDB.dbo.AccountsDayPlayTime WHERE UserID=@dwUserID AND DateID=@DateID
	IF @DayPlayTime IS NULL SET @DayPlayTime=0
	
	SELECT @DayWinLostScore =Score FROM StreamScoreInfo WHERE   DateID=@DateID AND UserID = @dwUserID 
	IF @DayWinLostScore IS NULL SET @DayWinLostScore=0

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 房间参数
	SET @IntegralCount = 0
	SELECT  @IntegralCount=IntegralCount FROM GameScoreAttribute WHERE UserID=@dwUserID And ServerID=@wServerID And KindID = @wKindID
	    
	--该游戏历史总输赢
	DECLARE @HistoryKindIDWin BIGINT
	SELECT @HistoryKindIDWin=SUM(Score) FROM WHQJTreasureDB.dbo.RecordDrawScore WHERE UserID=@UserID AND DrawID IN (SELECT DrawID FROM WHQJTreasureDB.dbo.RecordDrawInfo)
	IF @HistoryKindIDWin IS NULL SET @HistoryKindIDWin=0
	
	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @GroupID AS GroupID, @NickName AS NickName, @UnderWrite AS UnderWrite,  @FaceID AS FaceID, 0 AS Dimand,
		@CustomID AS CustomID, @Gender AS Gender, @GroupName AS GroupName, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight,
		@MasterOrder AS MasterOrder, @MemberOrder AS MemberOrder, @Score AS Score, @InoutIndex AS InoutIndex,
		@Insure AS Insure,  @WinCount AS WinCount,  @LostCount AS LostCount,@DrawCount AS DrawCount, @FleeCount AS FleeCount, 		
		@IntegralCount AS IntegralCount,@AgentID AS AgentID,@QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone, @MobilePhone AS MobilePhone,
		@DwellingPlace AS DwellingPlace,@PostalCode AS PostalCode,0 AS GroupIDArray,@HistoryKindIDWin AS HistoryKindIDWin,@DayPlayTime AS DayPlayTime,@DayWinLostScore AS DayWinLostScore

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 手机登录
CREATE PROC GSP_GR_EfficacyMobile
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@cbPersonalServer SMALLINT,					-- 私人房间
	@dwGroupID	INT,							--大联盟ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

-- 附属信息
DECLARE @QQ NVARCHAR(16)
DECLARE @EMail NVARCHAR(16)
DECLARE @SeatPhone NVARCHAR(32)
DECLARE @MobilePhone NVARCHAR(16)
DECLARE @DwellingPlace NVARCHAR(128)
DECLARE @PostalCode NVARCHAR(8)
DECLARE @DayPlayTime INT
DECLARE @DayWinLostScore BIGINT

-- 扩展信息
DECLARE @GameID INT
DECLARE @UserRight INT
DECLARE @Gender TINYINT
DECLARE @MasterRight INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberOrder SMALLINT
DECLARE @GroupName NVARCHAR(31)

-- 积分变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Dimand BIGINT
DECLARE @Grade BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @AgentID AS INT
DECLARE @IntegralCount BIGINT 

-- 执行逻辑
BEGIN

	-- 辅助变量
	DECLARE @EnjoinLogon BIGINT

	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END	

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏登录权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE @SpreaderID INT
	SET @AgentID = 0
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName ,@SpreaderID = SpreaderID, @UnderWrite=UnderWrite, @LogonPass=DynamicPass, @FaceID=FaceID, @CustomID=CustomID,
		@Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@UserRight=UserRight,
		@MasterRight=MasterRight, @MasterOrder=MasterOrder, @MemberOrder=MemberOrder, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,@AgentID=AgentID
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	--查询代理
	SELECT @AgentID = UserID FROM  WHQJAccountsDB.dbo.AccountsAgent  where UserID = @SpreaderID
	
	-- 附属信息
    SET @QQ =N''
    SET @EMail =N''
    SET @SeatPhone =N''
    SET @MobilePhone =N''
    SET @DwellingPlace =N''
    SET @PostalCode =N''
    SELECT @QQ = QQ,@EMail = EMail,@SeatPhone = SeatPhone,@MobilePhone = MobilePhone,@DwellingPlace = DwellingPlace ,@PostalCode =PostalCode 
    FROM  WHQJAccountsDB.dbo.IndividualDatum WHERE UserID=@dwUserID
    
	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
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
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！'
		RETURN 2
	END	
	
	-- 固定机器
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 2
		END
	END

	-- 密码判断
	IF @LogonPass<>@strPassword AND @strClientIP<>N'0.0.0.0'
	BEGIN
		SET @strErrorDescribe=N'抱歉,由于您的帐号在其它地方登录,请重新登录平台后再次尝试！'
		RETURN 3
	END

	-- 开始事务
--	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
--	BEGIN TRAN

	-- 游戏信息
	DECLARE @AllLogonTimes INT
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT
	SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount,
		@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder, @AllLogonTimes=AllLogonTimes
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 信息判断
	IF @Score IS NULL
	BEGIN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @strClientIP, @strMachineID, @strClientIP, @strMachineID)

		-- 游戏信息
		SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount,@DrawCount=DrawCount, @DrawCount=DrawCount,
			@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterOrder=MasterOrder, @GameMasterRight=MasterRight, @AllLogonTimes=AllLogonTimes
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- 异常判断
	IF @Score<0
	BEGIN
		-- 结束事务
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'抱歉地通知你，您的游戏币数据出现了异常情况，请联系客户服务中心了解详细情况！'
		RETURN 14
	END

	-- 锁定解除
	DELETE GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID

	-- 查询锁定
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- 锁定判断
	IF @LockKindID IS NOT NULL AND @LockServerID IS NOT NULL
	BEGIN

		-- 结束事务
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 查询类型
		IF @LockKindID<>0
		BEGIN
			-- 查询信息
			DECLARE @KindName NVARCHAR(31)
			DECLARE @ServerName NVARCHAR(31)
			SELECT @KindName=KindName FROM  WHQJPlatformDB.dbo.GameKindItem WHERE KindID=@LockKindID
			SELECT @ServerName=ServerName FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

			-- 错误信息
			IF @KindName IS NULL SET @KindName=N'未知游戏'
			IF @ServerName IS NULL SET @ServerName=N'未知房间'
			SET @strErrorDescribe=N'您正在别的房间对局哦，是否回去？'
			SELECT @LockServerID AS ServerID, @LockKindID AS KindID
			RETURN 100

		END
		ELSE
		BEGIN
			-- 提示消息
			SELECT [ErrorDescribe]=N'您正在进行保险柜处理过程中，暂时无法进入此游戏房间！'
			RETURN 4
		END
	END

	-- 保留变量
	SET @Grade=0
	SET @GroupName=''

	-- 权限标志
	SET @UserRight=@UserRight|@GameUserRight
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- 权限等级
	IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder
	-- 查询钻石
	SELECT @Dimand=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID		
	IF @Dimand IS NULL SET @Dimand=0	
	-- 进入记录
	INSERT RecordUserInout (UserID, EnterScore, EnterGrade, EnterInsure,  KindID, ServerID, EnterClientIP, EnterMachine)
	VALUES (@UserID, @Score, @Grade, @Insure, @wKindID, @wServerID, @strClientIP, @strMachineID)

	-- 记录标识
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- 插入锁表
	INSERT GameScoreLocker (UserID, ServerID, KindID, EnterID, EnterIP, EnterMachine) VALUES (@dwUserID, @wServerID, @wKindID, @InoutIndex, @strClientIP, @strMachineID)
	IF @@ERROR<>0
	BEGIN
		-- 结束事务
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'抱歉地通知你，游戏币锁定操作失败，请联系客户服务中心了解详细情况！'
		RETURN 14
	END

	--更新信息
	UPDATE GameScoreInfo SET AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), 
		LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID WHERE UserID=@dwUserID

	-- 登录统计
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 插入记录
	IF @AllLogonTimes>0
	BEGIN
		UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END
	ELSE
	BEGIN
		UPDATE SystemStreamInfo SET RegisterCount=RegisterCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, RegisterCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END	

	-- 俱乐部信息
	DECLARE @GroupIDArray nvarchar(512)
	SELECT @GroupIDArray=GroupIDArray FROM   WHQJGroupDB.dbo.IMUserGroupInfo WHERE UserID=@dwUserID
	IF @GroupIDArray IS NULL SET @GroupIDArray=';'

	--查询每日
	SELECT @DayPlayTime=DayPlayTimeCount FROM WHQJAccountsDB.dbo.AccountsDayPlayTime WHERE UserID=@dwUserID AND DateID=@DateID
	IF @DayPlayTime IS NULL SET @DayPlayTime=0
	
	SELECT @DayWinLostScore =Score FROM StreamScoreInfo WHERE   DateID=@DateID AND UserID = @dwUserID 
	IF @DayWinLostScore IS NULL SET @DayWinLostScore=0
	DECLARE @lGroupScore BIGINT
	SET @lGroupScore=0

	IF @dwGroupID<>0
	BEGIN
		SELECT @lGroupScore=Score FROM   WHQJGroupDB.dbo.IMGroupMember WHERE UserID=@dwUserID AND GroupID=@dwGroupID

		IF @lGroupScore IS NULL
		BEGIN
			SET @strErrorDescribe=N'抱歉，您不是该大联盟的成员！'
			RETURN 15
		END
		
		DECLARE @cbGroupStatus TINYINT
		SELECT @cbGroupStatus=GroupStatus FROM   WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID=@dwGroupID
		IF @cbGroupStatus <> 1
		BEGIN
			SET @strErrorDescribe=N'抱歉，该大联盟暂时无法使用！'
			RETURN 16
		END


		
		IF EXISTS (SELECT 1 FROM   WHQJGroupDB.dbo.IMGroupApplyRecord WHERE GroupID=@dwGroupID AND ApplyerID=@dwUserID AND ApplyType=1 AND ApplyStatus=0)
		BEGIN
			SET @strErrorDescribe=N'您已申请退出该大联盟，暂时无法游戏！'
			RETURN 17
		END

		--更新大联盟入场分
		UPDATE RecordUserInout SET EnterScore = @lGroupScore, GroupID=@dwGroupID WHERE ID=@InoutIndex AND UserID=@dwUserID
	END

	-- 结束事务
--	COMMIT TRAN
--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 房间参数
	SET @IntegralCount = 0
	SELECT  @IntegralCount=IntegralCount FROM GameScoreAttribute WHERE UserID=@dwUserID And ServerID=@wServerID And KindID = @wKindID
	
	--该游戏历史总输赢
	DECLARE @HistoryKindIDWin BIGINT
	SELECT @HistoryKindIDWin=SUM(Score) FROM WHQJTreasureDB.dbo.RecordDrawScore WHERE UserID=@UserID AND DrawID IN (SELECT DrawID FROM WHQJTreasureDB.dbo.RecordDrawInfo)
	IF @HistoryKindIDWin IS NULL SET @HistoryKindIDWin=0
	
	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @dwGroupID AS GroupID, @NickName AS NickName, @UnderWrite AS UnderWrite,  @FaceID AS FaceID, @Dimand AS Dimand,
		@CustomID AS CustomID, @Gender AS Gender, @GroupName AS GroupName, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight,
		@MasterOrder AS MasterOrder, @MemberOrder AS MemberOrder, @Score AS Score, @InoutIndex AS InoutIndex,
		@Insure AS Insure,  @WinCount AS WinCount,  @LostCount AS LostCount,@DrawCount AS DrawCount, @FleeCount AS FleeCount, 		
		@IntegralCount AS IntegralCount,@AgentID AS AgentID,@QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone, @MobilePhone AS MobilePhone,
		@DwellingPlace AS DwellingPlace,@PostalCode AS PostalCode, @GroupIDArray AS GroupIDArray, @lGroupScore AS GroupScore,@HistoryKindIDWin AS HistoryKindIDWin,@DayPlayTime AS DayPlayTime,@DayWinLostScore AS DayWinLostScore


END

RETURN 0

GO

---------------------------------------------------------------------------------------------------------------
-- 手机登录
CREATE PROC GSP_GR_GetOfflineInfo
	@dwUserID INT								-- 用户 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON


-- 执行逻辑
BEGIN

	
	-- 查询锁定
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- 判断积分锁表
	IF @LockKindID IS NULL OR @LockServerID IS NULL
	BEGIN
		SELECT @LockKindID=KindID, @LockServerID=ServerID FROM  WHQJGameScoreDB.dbo.GameScoreLocker WHERE UserID=@dwUserID

		-- 判断比赛锁表
		IF @LockKindID IS NULL OR @LockServerID IS NULL
		BEGIN
			SELECT @LockKindID=KindID, @LockServerID=ServerID FROM   WHQJGameMatchDB.dbo.GameScoreLocker WHERE UserID=@dwUserID
		END
	END

	-- 锁定判断
	IF @LockKindID IS NULL SET @LockKindID = 0
	IF @LockServerID IS NULL SET @LockServerID = 0
	
	-- 输出变量
	SELECT @LockKindID AS LockKindID, @LockServerID AS ServerID


END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- 手机登录
CREATE PROC GSP_GR_EfficacyAI
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@dwGroupID	INT,							--大联盟ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

-- 附属信息
DECLARE @QQ NVARCHAR(16)
DECLARE @EMail NVARCHAR(16)
DECLARE @SeatPhone NVARCHAR(32)
DECLARE @MobilePhone NVARCHAR(16)
DECLARE @DwellingPlace NVARCHAR(128)
DECLARE @PostalCode NVARCHAR(8)
DECLARE @DayPlayTime INT
DECLARE @DayWinLostScore BIGINT

-- 扩展信息
DECLARE @GameID INT
DECLARE @UserRight INT
DECLARE @Gender TINYINT
DECLARE @MasterRight INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberOrder SMALLINT
DECLARE @GroupName NVARCHAR(31)

-- 积分变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Dimand BIGINT
DECLARE @Grade BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @AgentID AS INT
DECLARE @IntegralCount BIGINT 

-- 执行逻辑
BEGIN

	-- 辅助变量
	DECLARE @EnjoinLogon BIGINT

	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END	

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏登录权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE @SpreaderID INT
	SET @AgentID = 0
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName ,@SpreaderID = SpreaderID, @UnderWrite=UnderWrite, @LogonPass=DynamicPass, @FaceID=FaceID, @CustomID=CustomID,
		@Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@UserRight=UserRight,
		@MasterRight=MasterRight, @MasterOrder=MasterOrder, @MemberOrder=MemberOrder, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,@AgentID=AgentID
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	--查询代理
	SELECT @AgentID = UserID FROM  WHQJAccountsDB.dbo.AccountsAgent  where UserID = @SpreaderID
	
	-- 附属信息
    SET @QQ =N''
    SET @EMail =N''
    SET @SeatPhone =N''
    SET @MobilePhone =N''
    SET @DwellingPlace =N''
    SET @PostalCode =N''
    SELECT @QQ = QQ,@EMail = EMail,@SeatPhone = SeatPhone,@MobilePhone = MobilePhone,@DwellingPlace = DwellingPlace ,@PostalCode =PostalCode 
    FROM  WHQJAccountsDB.dbo.IndividualDatum WHERE UserID=@dwUserID
    
	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
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
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！'
		RETURN 2
	END	
	
	-- 固定机器
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 2
		END
	END

	-- 密码判断
	IF @LogonPass<>@strPassword AND @strClientIP<>N'0.0.0.0'
	BEGIN
		SET @strErrorDescribe=N'抱歉,由于您的帐号在其它地方登录,请重新登录平台后再次尝试！'
		RETURN 3
	END

	-- 开始事务
--	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
--	BEGIN TRAN

	-- 游戏信息
	DECLARE @AllLogonTimes INT
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT
	SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount,
		@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder, @AllLogonTimes=AllLogonTimes
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 信息判断
	IF @Score IS NULL
	BEGIN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @strClientIP, @strMachineID, @strClientIP, @strMachineID)

		-- 游戏信息
		SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount,@DrawCount=DrawCount, @DrawCount=DrawCount,
			@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterOrder=MasterOrder, @GameMasterRight=MasterRight, @AllLogonTimes=AllLogonTimes
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- 异常判断
	IF @Score<0
	BEGIN
		-- 结束事务
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'抱歉地通知你，您的游戏币数据出现了异常情况，请联系客户服务中心了解详细情况！'
		RETURN 14
	END

	-- 锁定解除
	DELETE GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID

	-- 查询锁定
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- 锁定判断
	IF @LockKindID IS NOT NULL AND @LockServerID IS NOT NULL
	BEGIN

		-- 结束事务
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 查询类型
		IF @LockKindID<>0
		BEGIN
			-- 查询信息
			DECLARE @KindName NVARCHAR(31)
			DECLARE @ServerName NVARCHAR(31)
			SELECT @KindName=KindName FROM  WHQJPlatformDB.dbo.GameKindItem WHERE KindID=@LockKindID
			SELECT @ServerName=ServerName FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

			-- 错误信息
			IF @KindName IS NULL SET @KindName=N'未知游戏'
			IF @ServerName IS NULL SET @ServerName=N'未知房间'
			SET @strErrorDescribe=N'您正在别的房间对局哦，是否回去？'
			SELECT @LockServerID AS ServerID, @LockKindID AS KindID
			RETURN 100

		END
		ELSE
		BEGIN
			-- 提示消息
			SELECT [ErrorDescribe]=N'您正在进行保险柜处理过程中，暂时无法进入此游戏房间！'
			RETURN 4
		END
	END

	-- 保留变量
	SET @Grade=0
	SET @GroupName=''

	-- 权限标志
	SET @UserRight=@UserRight|@GameUserRight
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- 权限等级
	IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder
	-- 查询钻石
	SELECT @Dimand=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID		
	IF @Dimand IS NULL SET @Dimand=0	
	-- 进入记录
	INSERT RecordUserInout (UserID, EnterScore, EnterGrade, EnterInsure,  KindID, ServerID, EnterClientIP, EnterMachine)
	VALUES (@UserID, @Score, @Grade, @Insure, @wKindID, @wServerID, @strClientIP, @strMachineID)

	-- 记录标识
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- 插入锁表
	INSERT GameScoreLocker (UserID, ServerID, KindID, EnterID, EnterIP, EnterMachine) VALUES (@dwUserID, @wServerID, @wKindID, @InoutIndex, @strClientIP, @strMachineID)
	IF @@ERROR<>0
	BEGIN
		-- 结束事务
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'抱歉地通知你，游戏币锁定操作失败，请联系客户服务中心了解详细情况！'
		RETURN 14
	END

	--更新信息
	UPDATE GameScoreInfo SET AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), 
		LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID WHERE UserID=@dwUserID

	-- 登录统计
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 插入记录
	IF @AllLogonTimes>0
	BEGIN
		UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END
	ELSE
	BEGIN
		UPDATE SystemStreamInfo SET RegisterCount=RegisterCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, RegisterCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END	

	-- 俱乐部信息
	DECLARE @GroupIDArray nvarchar(512)
	SELECT @GroupIDArray=GroupIDArray FROM   WHQJGroupDB.dbo.IMUserGroupInfo WHERE UserID=@dwUserID
	IF @GroupIDArray IS NULL SET @GroupIDArray=';'

	--查询每日
	SELECT @DayPlayTime=DayPlayTimeCount FROM WHQJAccountsDB.dbo.AccountsDayPlayTime WHERE UserID=@dwUserID AND DateID=@DateID
	IF @DayPlayTime IS NULL SET @DayPlayTime=0
	
	SELECT @DayWinLostScore =Score FROM StreamScoreInfo WHERE   DateID=@DateID AND UserID = @dwUserID 
	IF @DayWinLostScore IS NULL SET @DayWinLostScore=0
	DECLARE @lGroupScore BIGINT
	SET @lGroupScore=0

	IF @dwGroupID<>0
	BEGIN
		SELECT @lGroupScore=Score FROM   WHQJGroupDB.dbo.IMGroupMember WHERE UserID=@dwUserID AND GroupID=@dwGroupID

		IF @lGroupScore IS NULL
		BEGIN
			SET @strErrorDescribe=N'抱歉，您不是该大联盟的成员！'
			RETURN 15
		END
		
		DECLARE @cbGroupStatus TINYINT
		SELECT @cbGroupStatus=GroupStatus FROM   WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID=@dwGroupID
		IF @cbGroupStatus <> 1
		BEGIN
			SET @strErrorDescribe=N'抱歉，该大联盟暂时无法使用！'
			RETURN 16
		END


		
		IF EXISTS (SELECT 1 FROM   WHQJGroupDB.dbo.IMGroupApplyRecord WHERE GroupID=@dwGroupID AND ApplyerID=@dwUserID AND ApplyType=1 AND ApplyStatus=0)
		BEGIN
			SET @strErrorDescribe=N'您已申请退出该大联盟，暂时无法游戏！'
			RETURN 17
		END

		--更新大联盟入场分
		UPDATE RecordUserInout SET EnterScore = @lGroupScore, GroupID=@dwGroupID WHERE ID=@InoutIndex AND UserID=@dwUserID
	END

	-- 结束事务
--	COMMIT TRAN
--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 房间参数
	SET @IntegralCount = 0
	SELECT  @IntegralCount=IntegralCount FROM GameScoreAttribute WHERE UserID=@dwUserID And ServerID=@wServerID And KindID = @wKindID
	
	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @dwGroupID AS GroupID, @NickName AS NickName, @UnderWrite AS UnderWrite,  @FaceID AS FaceID, @Dimand AS Dimand,
		@CustomID AS CustomID, @Gender AS Gender, @GroupName AS GroupName, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight,
		@MasterOrder AS MasterOrder, @MemberOrder AS MemberOrder, @Score AS Score, @InoutIndex AS InoutIndex,
		@Insure AS Insure,  @WinCount AS WinCount,  @LostCount AS LostCount,@DrawCount AS DrawCount, @FleeCount AS FleeCount, 		
		@IntegralCount AS IntegralCount,@AgentID AS AgentID,@QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone, @MobilePhone AS MobilePhone,
		@DwellingPlace AS DwellingPlace,@PostalCode AS PostalCode, @GroupIDArray AS GroupIDArray, @lGroupScore AS GroupScore,@DayPlayTime AS DayPlayTime,@DayWinLostScore AS DayWinLostScore


END

RETURN 0

GO
---------------------------------------------------------------------------------------------