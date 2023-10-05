
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_EfficacyAccounts]
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyAccounts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyOtherPlatform]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyOtherPlatform]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyLogonVisitor]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyLogonVisitor]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 帐号登录
CREATE PROC GSP_GP_EfficacyAccounts
	@strAccounts NVARCHAR(31),					-- 用户帐号
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @AgentID INT
DECLARE @PlayTimeCount INT

-- 财富变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT

-- 扩展信息
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- 辅助变量
DECLARE @EnjoinLogon AS INT

-- 执行逻辑
BEGIN
	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'您好，您所在IP地址的登录功能已被禁用，请联系客服了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'您好，您使用的机器的登录功能已被禁用，请联系客服了解详细情况！'
		RETURN 7
	END
 
	-- 查询用户
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineSerial NCHAR(32)
	DECLARe @strLastLogonIP NCHAR(15)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,@InsurePass=InsurePass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,
		 @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,@strLastLogonIP=LastLogonIP,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@AgentID=AgentID
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 1
	END	
	
	-- 查询代理
	--IF EXISTS (SELECT * FROM AccountsAgent WHERE @UserID = UserID and Nullity=0)
	--BEGIN
		--SET @strErrorDescribe=N'您是代理商帐号，不可登录！'
		--RETURN 1
	--END		
	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时被冻结，请联系客服了解详细情况！'
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
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 1
		END
	END

	-- 密码判断
	IF @LogonPass<>@strPassword OR @strPassword IS NULL OR @LogonPass IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 3
	END

	
	-- 固定机器
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SET @strErrorDescribe=N'您的帐号成功使用了固定机器登录功能！'
		UPDATE AccountsInfo SET MoorMachine=@MoorMachine, LastLogonMachine=@strMachineID WHERE UserID=@UserID
	END

	-- 推广员提成
	IF @SpreaderID<>0 AND @AgentID=0
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'游戏时长达标一次性奖励'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM WHQJTreasureDB.dbo.GlobalSpreadInfo WHERE ID=1
		IF @GrantTime IS NULL
		BEGIN
			SET @GrantTime = 108000 -- 30小时
			SET @GrantScore = 200000
		END	
		
		IF @GrantScore>0 AND @GrantTime>0 AND @PlayTimeCount>=@GrantTime
		BEGIN
			-- 获取提成信息
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM WHQJTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO  WHQJTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- 查询金币
	SELECT @Score=Score, @Insure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	
	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0

	-- 会员等级
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- 删除会员
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- 搜索会员
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- 数据调整
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- 更新数据
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--判断玩家是否在房间中，如果在房间中不更新动态密码
	DECLARE @LockServerID INT
	SELECT @LockServerID = ServerID FROM WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerID IS NOT NULL
	BEGIN
		-- 更新信息
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1,LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			LastLogonMachine=@strMachineID 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
			-- 更新信息
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1,LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID 
		WHERE UserID=@UserID
	END

	-- 动态密码
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- 记录日志
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- 银行标识
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1
	
	-- 代理标识
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF @AgentID>0 SET @IsAgent=1
		
	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender,
		@Score AS Score, @Insure AS Insure, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate, @MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled, @PlatformID AS LogonMode,@IsAgent AS IsAgent
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 帐号登录
CREATE PROC GSP_MB_EfficacyAccounts
	@strAccounts NVARCHAR(31),					-- 用户帐号
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strMobilePhone NVARCHAR(11),				-- 手机号码
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @PlayTimeCount INT

-- 财富变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @lDiamond BIGINT
DECLARE @lSilver BIGINT
DECLARE @nAwardTicket INT

-- 扩展信息
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @MBUserRight INT


DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- 辅助变量
DECLARE @EnjoinLogon AS INT

-- 执行逻辑
BEGIN
	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的登录功能，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的登录功能，请联系客户服务中心了解详细情况！'
		RETURN 7
	END

	-- 查询用户
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE	@MachineSerial NCHAR(32)	
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,@InsurePass=InsurePass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,
		@MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine,@MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@MBUserRight=UserRight
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

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
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 1
		END
	END
		
	-- 密码判断
	IF @LogonPass<>@strPassword OR @strPassword IS NULL OR @LogonPass IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 3
	END

	-- 推广员提成
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'游戏时长达标一次性奖励'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM  WHQJTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30小时
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- 获取提成信息
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM  WHQJTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO WHQJTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- 查询金币
	SELECT @Score=Score, @Insure=InsureScore FROM WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID


	-- 查询钻石
	SELECT @lDiamond=Diamond, @nAwardTicket=AwardTicket FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID
	
	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @lDiamond IS NULL SET @lDiamond=0
	IF @nAwardTicket IS NULL SET @nAwardTicket=0

	-- 会员等级
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- 删除会员
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- 搜索会员
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- 数据调整
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- 更新数据
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--判断玩家是否在房间中，如果在房间中不更新动态密码
	DECLARE @LockServerIDCheck INT
	SELECT @LockServerIDCheck = ServerID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerIDCheck IS NOT NULL
	BEGIN
		-- 更新信息
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
		-- 更新信息
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END


	-- 动态密码
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- 记录日志
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- 银行标识
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1
	
	-- 代理标识
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID and Nullity=0) SET @IsAgent=1

	
	-- 锁定房间ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID=KindID FROM WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0


	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, 	
		@Score AS Score, @Insure AS Insure, @lDiamond AS Diamond, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled, @PlatformID AS LogonMode ,@IsAgent AS IsAgent,
		@LockServerID AS LockServerID,  @wKindID AS KindID, @MBUserRight AS UserRight, @nAwardTicket AS AwardTicket

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- 第三方登录
CREATE PROC GSP_MB_EfficacyOtherPlatform
	@cbPlatformID TINYINT,						-- 平台编号
	@strUserUin NVARCHAR(32),			        -- 用户Uin
	@strNickName NVARCHAR(32),					-- 用户昵称
	@strCompellation NVARCHAR(16),				-- 真实名字
	@cbGender TINYINT,							-- 用户性别
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strMobilePhone NVARCHAR(11),				-- 手机号码
	@cbDeviceType TINYINT,						-- 注册来源	
	@cbClientType TINYINT,						-- 客户端类型	
	@DeviceToken  NVARCHAR(65),					-- 绑定设备标识
	@strFaceUrl NVARCHAR(500),					-- 头像URL
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @AgentID INT
DECLARE @PlayTimeCount INT

-- 积分变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @lDiamond BIGINT
DECLARE @nAwardTicket INT
DECLARE @DiamondCount BIGINT
-- 扩展信息
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- 辅助变量
DECLARE @EnjoinLogon AS INT

-- 执行逻辑
BEGIN

	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的登录功能，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的登录功能，请联系客户服务中心了解详细情况！'
		RETURN 7
	END

	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 查询用户
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE	@MachineSerial NCHAR(32)
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@InsurePass=InsurePass,
		@MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@AgentID=AgentID
	FROM AccountsInfo(NOLOCK) WHERE UserUin=@strUserUin AND PlatformID=@cbPlatformID

    DECLARE @strTempName NVARCHAR(31)
	DECLARE @cbNewUser TINYINT
	SET @cbNewUser=0
	-- 注册用户
	IF @UserID IS NULL
	BEGIN
		--校验客户端类型
		IF @cbClientType NOT IN (0x01, 0x02, 0x03)
		BEGIN
			SET @strErrorDescribe=N'注册客户端类型无效！'
			RETURN 8
		END
	
		-- 玩家账号
		SET @strTempName=@strNickName --+CONVERT(nvarchar(8),REPLACE(newid(),'-',''))
		
		-- 生成账号
		DECLARE @strTemp NVARCHAR(31)
		SET @strTemp=CONVERT(NVARCHAR(31),REPLACE(NEWID(),'-','_'))
		
		-- 下载推广查询
		SELECT @SpreaderID=GameID FROM BindIP WHERE UserIP = @strClientIP
		
		IF @SpreaderID IS NOT NULL AND @SpreaderID<>0
		BEGIN
			DELETE FROM BindIP WHERE GameID=@SpreaderID AND UserIP = @strClientIP
		END
		ELSE 
		BEGIN
			SET @SpreaderID=0
		END
		
		-- 注册用户
		INSERT AccountsInfo (Accounts,NickName,RegAccounts,PlatformID,UserUin,LogonPass,InsurePass,Gender,FaceID,SpreaderID,
			GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine,RegisterOrigin,ClientType)
		VALUES (@strTemp,@strTempName,@strTemp,@cbPlatformID,@strUserUin,N'd1fd5495e7b727081497cfce780b6456',N'',@cbGender,0,@SpreaderID,
			1,@strClientIP,N'',@strMachineID,@strClientIP,N'',@strMachineID,@cbDeviceType, @cbClientType)
		
		-- 记录日志
		UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

		-- 查询用户
		SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
			@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
			@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@AgentID=AgentID
		FROM AccountsInfo(NOLOCK) WHERE UserUin=@strUserUin AND PlatformID=@cbPlatformID

		-- 插入头像
		INSERT AccountsFace (UserID, InsertTime, InsertAddr, InsertMachine,FaceUrl)
		VALUES (@UserID,  @DateID, @strClientIP, '',@strFaceUrl)
		-- 更新用户	
		SELECT @CustomID=SCOPE_IDENTITY()
	
		-- 分配标识
		SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
		IF @GameID IS NULL 
		BEGIN
			UPDATE AccountsInfo SET CustomID = @CustomID WHERE UserID = @UserID
			SET @GameID=0
			SET @strErrorDescribe=N'用户注册成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！'
		END
		ELSE UPDATE AccountsInfo SET GameID=@GameID,CustomID = @CustomID WHERE UserID=@UserID

		
		-- 初始化金币信息
		INSERT INTO WHQJTreasureDB.dbo.GameScoreInfo(UserID,Score,RegisterIP) VALUES(@UserID,0,@strClientIP)
		INSERT INTO WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket) VALUES(@UserID,0,0)
		
		-- 注册赠送钻石
		DECLARE @PresentDiamond INT
		DECLARE @PresentGold INT
		SELECT @PresentDiamond=DiamondCount, @PresentGold=ScoreCount FROM RegisterGive WITH(NOLOCK) WHERE PlatformType = @cbClientType
		IF (@PresentDiamond IS NULL OR @PresentDiamond<0) SET @PresentDiamond=0
		IF (@PresentGold IS NULL OR @PresentGold<0) SET @PresentGold=0
		
		IF @PresentDiamond>0 OR @PresentGold>0
		BEGIN

			-- 赠送金币
			UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score = Score+@PresentGold WHERE UserID = @UserID

			-- 更新钻石
			UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond = Diamond+@PresentDiamond WHERE UserID = @UserID
			
			-- 记录金币流水记录
			DECLARE @DateTime DATETIME
			DECLARE @SerialNumber NVARCHAR(20)
			SET @DateTime = GETDATE()
			SELECT @SerialNumber=dbo.WF_GetSerialNumber()
			
			IF @PresentGold IS NOT NULL AND @PresentGold <> 0

			--金币流水
			INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@UserID,1,0,0,@PresentGold,@strClientIP,@DateTime)	

			--钻石流水
			INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@UserID,1,0,@PresentDiamond,@strClientIP,GETDATE())
		
		END

		SET @cbNewUser=1
	END	
	ELSE
	BEGIN	   
		-- 修改昵称
		IF @NickName<>@strNickName
		BEGIN
			-- 玩家账号			
			SET @NickName=@strNickName
			UPDATE AccountsInfo SET NickName=@strNickName WHERE UserID=@UserID			
		END

		-- 性别修改
		IF @cbGender <> @Gender
		BEGIN
			SET @Gender = @cbGender
			UPDATE AccountsInfo SET Gender=@Gender WHERE UserID=@UserID			
		END
		
		-- 头像URL
		DECLARE @FaceUrl NVARCHAR(500)
		SELECT @FaceUrl = FaceUrl FROM AccountsFace WHERE UserID = @UserID
		IF @FaceUrl <> @strFaceUrl					
		BEGIN
			UPDATE AccountsFace SET FaceUrl=@strFaceUrl WHERE UserID=@UserID			
		END
	END

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！账号:'+CONVERT(NVARCHAR(31),@GameID)
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
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 1
		END
	END

	-- 推广员提成
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'游戏时长达标一次性奖励'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM  WHQJTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30小时
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- 获取提成信息
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM  WHQJTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO  WHQJTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- 查询金币
	SELECT @Score=Score, @Insure=InsureScore FROM WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	-- 查询钻石
	SELECT @lDiamond=Diamond, @nAwardTicket=AwardTicket FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID
	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0	
	IF @lDiamond IS NULL SET @lDiamond = 0
	IF @nAwardTicket IS NULL SET @nAwardTicket = 0
	-- 会员等级
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- 删除会员
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- 搜索会员
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- 数据调整
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- 更新数据
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--判断玩家是否在房间中，如果在房间中不更新动态密码
	DECLARE @LockServerIDCheck INT
	SELECT @LockServerIDCheck = ServerID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerIDCheck IS NOT NULL
	BEGIN
		-- 更新信息
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP, LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
		-- 更新信息
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	
	-- 记录日志
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)
	
	-- 记录UMeng日志	
	IF @cbDeviceType = 17 or @cbDeviceType = 18
	BEGIN
		UPDATE AccountsUmeng SET DeviceType=0,DeviceToken=@DeviceToken,UpdateTime=GETDATE(),UpdateAddress=@strClientIP WHERE UserID=@UserID
		IF @@ROWCOUNT=0 INSERT AccountsUmeng(UserID,GameID,DeviceType,DeviceToken,UpdateTime,UpdateAddress) VALUES (@UserID,@GameID,0,@DeviceToken,GETDATE(),@strClientIP)
	END 
	IF @cbDeviceType = 49 or @cbDeviceType = 50 or @cbDeviceType = 65 or @cbDeviceType = 66
	BEGIN
		UPDATE AccountsUmeng SET DeviceType=1,DeviceToken=@DeviceToken,UpdateTime=GETDATE(),UpdateAddress=@strClientIP WHERE UserID=@UserID
		IF @@ROWCOUNT=0 INSERT AccountsUmeng(UserID,GameID,DeviceType,DeviceToken,UpdateTime,UpdateAddress) VALUES (@UserID,@GameID,1,@DeviceToken,GETDATE(),@strClientIP)
	END
	-- 动态密码
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- 银行标识
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1

	-- 代理标识
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF @AgentID>0 SET @IsAgent=1
	--IF EXISTS (SELECT * FROM WHQJAgentDB.dbo.AgentInfo WHERE UserID=@UserID and Nullity=0) SET @IsAgent=1
	
	-- 锁定房间ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID=KindID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0
	IF @cbNewUser=0
		SET @strClientIP=''
	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender,@Score AS Score,@Insure AS Insure,
		@MemberOrder AS MemberOrder,@MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine,@InsureEnabled AS InsureEnabled,
		@PlatformID AS LogonMode,@IsAgent AS IsAgent,@lDiamond AS Diamond,@LockServerID AS LockServerID, @wKindID AS KindID,@nAwardTicket AS AwardTicket,@strClientIP AS ClientIP
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 游客登录
CREATE PROC GSP_MB_EfficacyLogonVisitor
	@cbLogonMode TINYINT,						-- 登陆方式
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strMobilePhone NVARCHAR(11),				-- 手机号码
	@cbDeviceType TINYINT,						-- 注册来源		
	@cbClientType TINYINT,						-- 平台编号		
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @AgentID INT
DECLARE @PlayTimeCount INT

-- 积分变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @lDiamond BIGINT
DECLARE @nAwardTicket INT
DECLARE @DiamondCount BIGINT

-- 扩展信息
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- 辅助变量
DECLARE @EnjoinLogon AS INT

-- 执行逻辑
BEGIN

	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的登录功能，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的登录功能，请联系客户服务中心了解详细情况！'
		RETURN 7
	END

	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 查询用户
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE	@MachineSerial NCHAR(32)
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @InsurePass=InsurePass,
		@MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@AgentID=AgentID
	FROM AccountsInfo(NOLOCK) WHERE RegisterMachine=@strMachineID AND PlatformID=@cbLogonMode

	-- 注册用户
	IF @UserID IS NULL
	BEGIN	

		-- 临时账号
		DECLARE @strTemp NVARCHAR(31)
		SET @strTemp=CONVERT(NVARCHAR(31),REPLACE(NEWID(),'-','_'))
		
		-- 下载推广查询
		SELECT @SpreaderID=GameID FROM BindIP WHERE UserIP = @strClientIP
		
		IF @SpreaderID IS NOT NULL AND @SpreaderID<>0
		BEGIN
			DELETE FROM BindIP WHERE GameID=@SpreaderID AND UserIP = @strClientIP
		END
		ELSE 
		BEGIN
			SET @SpreaderID=0
		END
		
		-- 注册用户
		INSERT AccountsInfo (Accounts,NickName,RegAccounts,PlatformID,UserUin,LogonPass,InsurePass,Gender,FaceID,SpreaderID,
			GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine,RegisterOrigin)
		VALUES (@strTemp,@strTemp,@strTemp,@cbLogonMode,@strMachineID,N'd1fd5495e7b727081497cfce780b6456',N'',0,0,@SpreaderID,
			1,@strClientIP,N'',@strMachineID,@strClientIP,N'',@strMachineID,@cbDeviceType)

		-- 玩家账号
		DECLARE @MaxUserID INT
		SELECT @MaxUserID=SCOPE_IDENTITY()
		DECLARE @strTempName NVARCHAR(31)
		SET @strTempName=N'游客'+CONVERT(NVARCHAR(31),@MaxUserID)
		
		--更新账号昵称
		UPDATE AccountsInfo SET Accounts=@strTempName,NickName=@strTempName,RegAccounts=@strTempName WHERE UserID=@MaxUserID

		-- 记录日志
		UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

		-- 查询用户
		SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass, @InsurePass=InsurePass,
			@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, 
			@MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
			@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@AgentID=AgentID
		FROM AccountsInfo(NOLOCK) WHERE RegisterMachine=@strMachineID AND PlatformID=@cbLogonMode

		-- 分配标识
		SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
		IF @GameID IS NULL 
		BEGIN
			SET @GameID=0
			SET @strErrorDescribe=N'用户注册成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！'
		END
		ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

		-- 初始化金币信息
		INSERT INTO WHQJTreasureDB.dbo.GameScoreInfo(UserID,RegisterIP) VALUES(@UserID,@strClientIP)
		INSERT INTO WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket) VALUES(@UserID,0,0)
		
		-- 注册赠送钻石
		DECLARE @PresentDiamond INT
		DECLARE @PresentGold INT
		--SELECT @PresentDiamond=StatusValue FROM SystemStatusInfo WITH(NOLOCK) WHERE StatusName=N'JJRegisterDiamondCount'
		--SELECT @PresentGold=StatusValue FROM SystemStatusInfo WITH(NOLOCK) WHERE StatusName=N'GrantScoreCount'
		SELECT @PresentDiamond=DiamondCount, @PresentGold=ScoreCount FROM RegisterGive WITH(NOLOCK) WHERE PlatformType = @cbClientType
		IF (@PresentDiamond IS NULL OR @PresentDiamond<0) SET @PresentDiamond=0
		IF (@PresentGold IS NULL OR @PresentGold<0) SET @PresentGold=0
		IF @PresentDiamond>0 OR @PresentGold>0
		BEGIN
			-- 赠送金币
			UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score = Score+@PresentGold WHERE UserID = @UserID 

			-- 更新钻石
			UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond = Diamond+@PresentDiamond WHERE UserID = @UserID
			
			-- 记录金币流水记录
			DECLARE @DateTime DATETIME
			DECLARE @SerialNumber NVARCHAR(20)
			SET @DateTime = GETDATE()
			SELECT @SerialNumber=dbo.WF_GetSerialNumber()
			
			IF @PresentGold IS NOT NULL AND @PresentGold <> 0

			--金币流水
			INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@UserID,1,0,0,@PresentGold,@strClientIP,@DateTime)	

			--钻石流水
			INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@UserID,1,0,@PresentDiamond,@strClientIP,GETDATE())
			
			--INSERT INTO WHQJRecordDB.dbo.RecordRegisterGrant(UserID,RegisterIP,RegisterMachine,RegisterOrigin,GrantDiamond,GrantGold) 
			--VALUES(@UserID,@strClientIP,@strMachineID,@cbDeviceType,@PresentDiamond,@PresentGold)
		END
	END	
	
	-- 检查绑定
	IF EXISTS(SELECT * FROM AccountsVisitor WHERE VisitorMachine=@strMachineID )
	BEGIN
		SET @strErrorDescribe=N'登录失败，此帐号已经绑定！'
		return 10
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
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 1
		END
	END

	
	-- 推广员提成
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'游戏时长达标一次性奖励'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM  WHQJTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30小时
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- 获取提成信息
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM  WHQJTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO  WHQJTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- 查询金币
	SELECT @Score=Score, @Insure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	-- 查询钻石
	SELECT @lDiamond=Diamond ,@nAwardTicket=AwardTicket FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID
	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0	
	IF @lDiamond IS NULL SET @lDiamond = 0
	IF @nAwardTicket IS NULL SET @nAwardTicket = 0
	-- 会员等级
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- 删除会员
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- 搜索会员
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- 数据调整
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- 更新数据
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--判断玩家是否在房间中，如果在房间中不更新动态密码
	DECLARE @LockServerIDCheck INT
	SELECT @LockServerIDCheck = ServerID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerIDCheck IS NOT NULL
	BEGIN
		-- 更新信息
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
		-- 更新信息
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END


	-- 记录日志
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- 动态密码
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- 银行标识
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1

	-- 代理标识
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF @AgentID>0 SET @IsAgent=1

	IF @strErrorDescribe IS NULL
	BEGIN
		SET @strErrorDescribe=N'游客登录成功！'
	END

	-- 锁定房间ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID=KindID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0
		
	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender,@Score AS Score,@Insure AS Insure,
		@MemberOrder AS MemberOrder, @MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled, 
		@PlatformID AS LogonMode,@IsAgent AS IsAgent,@lDiamond AS Diamond,@LockServerID AS LockServerID, @wKindID AS KindID,@nAwardTicket AS AwardTicket

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------