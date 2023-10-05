
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserEnableInsure]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserEnableInsure]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserSaveScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserSaveScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserTakeScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserTakeScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserTransferScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserTransferScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryUserInsureInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryUserInsureInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryTransferRebate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryTransferRebate]
GO

--代币管理
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_GetAllKindMoney]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_GetAllKindMoney]
GO

--财富信息
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_GetUserWealth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_GetUserWealth]
GO

--数字转字符串函数
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSF_GR_Switch_Num]') and OBJECTPROPERTY(ID, N'IsScalarFunction') = 1)
DROP FUNCTION [dbo].[GSF_GR_Switch_Num]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON
GO

----------------------------------------------------------------------------------------------------

-- 开通银行
CREATE PROC GSP_GR_UserEnableInsure
	@dwUserID INT,								-- 用户 I D
	@strLogonPass NCHAR(32),					-- 登录密码
	@strInsurePass NCHAR(32),					-- 银行密码	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	IF NOT EXISTS(SELECT * FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strLogonPass)
	BEGIN
		SET @strErrorDescribe=N'密码验证失败，无法开通银行！'
		RETURN 1		
	END
	
	-- 密码校验
	IF @strInsurePass = @strLogonPass
	BEGIN
		SET @strErrorDescribe=N'密码不能与登录密码相同，无法开通银行！'
		RETURN 1		
	END	
	
	-- 设置密码
	UPDATE  WHQJAccountsDB.dbo.AccountsInfo SET InsurePass=@strInsurePass	WHERE UserID=@dwUserID		

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 存入金币
CREATE PROC GSP_GR_UserSaveScore
	@dwUserID INT,								-- 用户 I D
	@lSaveScore BIGINT,							-- 金币数目
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 金币变量
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT
DECLARE @InsureRevenue BIGINT
DECLARE @VariationScore BIGINT
DECLARE @VariationInsure BIGINT

-- 执行逻辑
BEGIN

	-- 辅助变量
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT

	-- 系统暂停
	SELECT @EnjoinInsure=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 7
	END
 
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

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
			RETURN 1
		END
	END
	
	-- 金币判断
	DECLARE @BankPrerequisite AS INT
	SELECT @BankPrerequisite=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankPrerequisite'
	IF @BankPrerequisite IS NULL SET @BankPrerequisite=0
	IF @lSaveScore<@BankPrerequisite
	BEGIN
		SET @strErrorDescribe=N'存入失败，单次存款不能小于'+dbo.GSF_GR_Switch_Num(@BankPrerequisite)+'！'
		RETURN 4
	END

	DECLARE @BankMax AS INT
	SELECT @BankMax=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankGameMax'
	IF @BankMax IS NULL SET @BankMax=0
	IF @lSaveScore>@BankMax
	BEGIN
		SET @strErrorDescribe=N'存入失败，单次存款不能大于'+dbo.GSF_GR_Switch_Num(@BankMax)+'！'
		RETURN 5
	END

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 游戏信息
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 金币判断
	IF @SourceScore IS NULL OR @SourceScore<@lSaveScore
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'存入失败，携带游戏币不足！'
		RETURN 4
	END

	-- 查询锁定
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- 锁定调整
	IF @LockKindID IS NULL SET @LockKindID=0
	IF @LockServerID IS NULL SET @LockServerID=0

	-- 锁定判断
	IF (@LockKindID<>0 and @LockKindID<>@wKindID) OR (@LockServerID<>0 and @LockServerID<>@wServerID)
	BEGIN

		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 查询类型
		IF @LockKindID<>0 AND @LockServerID<>0
		BEGIN
			-- 查询信息
			DECLARE @KindName NVARCHAR(31)
			DECLARE @ServerName NVARCHAR(31)
			SELECT @KindName=KindName FROM  WHQJPlatformDB.dbo.GameKindItem WHERE KindID=@LockKindID
			SELECT @ServerName=ServerName FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

			-- 错误信息
			IF @KindName IS NULL SET @KindName=N'未知游戏'
			IF @ServerName IS NULL SET @ServerName=N'未知房间'
			SET @strErrorDescribe=N'您正在 [ '+@KindName+N' ] 的 [ '+@ServerName+N' ] 游戏房间中，不能进行当前的银行操作！'
			RETURN 4

		END
		ELSE
		BEGIN
			-- 提示消息
			SELECT [ErrorDescribe]=N'您正在使用网站页面进行银行处理过程中，不能进行当前的银行操作！'
			SET @strErrorDescribe=N'您正在使用网站页面进行银行处理过程中，不能进行当前的银行操作！'
			RETURN 4
		END
	END

	-- 计算变量
	SET @InsureRevenue=0
	SET @VariationScore=-@lSaveScore
	SET @VariationInsure=@lSaveScore

	-- 设置信息
	SET @strErrorDescribe=N'存入成功！'

	-- 更新数据
	UPDATE GameScoreInfo SET Score=Score+@VariationScore, InsureScore=InsureScore+@VariationInsure, Revenue=Revenue+@InsureRevenue	
	WHERE UserID=@dwUserID

	-- 记录日志
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@lSaveScore,@InsureRevenue,0,1,@strClientIP)		

	-- 记录金币流水记录
	DECLARE @DateTime DATETIME
	DECLARE @SerialNumber NVARCHAR(20)
	SET @DateTime = GETDATE()
	SELECT @SerialNumber=dbo.WF_GetSerialNumber()
	
	INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
	VALUES(@SerialNumber,0,@dwUserID,6,@SourceScore,@SourceInsure,@lSaveScore,@strClientIP,@DateTime)	
	
	
	-- 结束事务	
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 输出结果
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, @VariationScore AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 提取金币
CREATE PROC GSP_GR_UserTakeScore
	@dwUserID INT,								-- 用户 I D
	@lTakeScore BIGINT,							-- 金币数目
	@strPassword NCHAR(32),						-- 用户密码
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 金币变量
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT
DECLARE @InsureRevenue BIGINT
DECLARE @VariationScore BIGINT
DECLARE @VariationInsure BIGINT

-- 执行逻辑
BEGIN

	-- 辅助变量
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT

	-- 系统暂停
	SELECT @EnjoinInsure=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 7
	END
 
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @InsurePass=InsurePass, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

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
			RETURN 1
		END
	END

	-- 密码判断
	IF @InsurePass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的银行密码不正确或者输入有误，请查证后再次尝试！'
		RETURN 3
	END

	-- 金币判断
	DECLARE @BankPrerequisite AS INT
	SELECT @BankPrerequisite=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankPrerequisite'
	IF @BankPrerequisite IS NULL SET @BankPrerequisite=0
	IF @lTakeScore<@BankPrerequisite
	BEGIN
		SET @strErrorDescribe=N'取出失败，单次取款不能小于 '+dbo.GSF_GR_Switch_Num(@BankPrerequisite)+'！'
		RETURN 4
	END

	DECLARE @BankMax AS INT
	SELECT @BankMax=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankGameMax'
	IF @BankMax IS NULL SET @BankMax=0
	IF @lTakeScore>@BankMax
	BEGIN
		SET @strErrorDescribe=N'取出失败，单次取款不能大于 '+dbo.GSF_GR_Switch_Num(@BankMax)+'！'
		RETURN 5
	END

	-- 银行服务比例
	DECLARE @RevenueRate INT
	SELECT @RevenueRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTake'

	-- 服务比例调整
	IF @RevenueRate>300 SET @RevenueRate=300
	IF @RevenueRate IS NULL SET @RevenueRate=1

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 游戏信息
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 金币判断
	IF @SourceInsure IS NULL OR @SourceInsure<@lTakeScore
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'取出失败，银行游戏币不足！'
		RETURN 4
	END

	-- 查询锁定
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- 计算变量
	SET @InsureRevenue=@lTakeScore*@RevenueRate/1000
	SET @VariationScore=@lTakeScore-@InsureRevenue
	SET @VariationInsure=-@lTakeScore

	-- 设置信息
	SET @strErrorDescribe=N'取出成功！'

	-- 更新数据
	UPDATE GameScoreInfo SET Score=Score+@VariationScore, InsureScore=InsureScore+@VariationInsure, Revenue=Revenue+@InsureRevenue
	WHERE UserID=@dwUserID

	-- 记录日志
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@lTakeScore,@InsureRevenue,0,2,@strClientIP)	

	--插入流水日志
	DECLARE @DateTime DATETIME	
	DECLARE @SerialNumber NVARCHAR(20)
	SET @DateTime = GETDATE()
	IF @InsureRevenue>0 -- 取款服务比例 存在时 记入金币流水 为 银行服务费
	BEGIN 
		SELECT @SerialNumber=dbo.WF_GetSerialNumber()	
		INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
		VALUES(@SerialNumber,0,@dwUserID,8,@SourceScore,@SourceInsure,-@InsureRevenue,@strClientIP,@DateTime)
		--SET @SourceInsure=@SourceInsure-@InsureRevenue
	END

	SELECT @SerialNumber=dbo.WF_GetSerialNumber()	
	INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
	VALUES(@SerialNumber,0,@dwUserID,7,@SourceScore,@SourceInsure,@VariationScore,@strClientIP,DATEADD(ms,10,@DateTime))	
	
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 输出结果
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, @VariationScore AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 转账金币
CREATE PROC GSP_GR_UserTransferScore
	@dwUserID INT,								-- 用户 I D	
	@lTransferScore BIGINT,						-- 金币数目
	@strPassword NCHAR(32),						-- 交易密码
	@dwGameID INT,								-- Game	ID
	@strTransRemark NVARCHAR(32),				-- 转账备注	
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 金币变量
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

DECLARE @InsureRevenue BIGINT
DECLARE @VariationInsure BIGINT

-- 执行逻辑
BEGIN

	-- 辅助变量
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT
	DECLARE @EnjoinTransfer INT

	-- 系统暂停
	SELECT @EnjoinInsure=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END	
	
	-- 转账暂停
	DECLARE @TUserRight INT
	DECLARE @UserOpenTransfer INT
	SELECT @TUserRight=UserRight FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	SET @UserOpenTransfer = @TUserRight&0x00000040	
	IF @UserOpenTransfer=0
	BEGIN
		SELECT @EnjoinTransfer=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferStauts'
		IF @EnjoinTransfer IS NULL SET @EnjoinTransfer=0
		IF @EnjoinTransfer=1 
		BEGIN
			SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferStauts'
			RETURN 3
		END	
	END
	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 7
	END

	-- 返利配置
	DECLARE @ConfigID INT
	DECLARE @ReturnType TINYINT
	DECLARE @ReturnPercent INT
	DECLARE @ReturnDescribe NVARCHAR(32)
	DECLARE @DatetimeNow DATETIME
	DECLARE @SrcMemberOverDate	DATETIME
	DECLARE @SrcMemberOrder		INT
	DECLARE @DstMemberOverDate	DATETIME
	DECLARE @DstMemberOrder		INT	
	
	
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MemberOrder AS TINYINT
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @InsurePass=InsurePass, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MemberOrder=MemberOrder, @MachineID=LastLogonMachine,
	@SrcMemberOrder=MemberOrder,@SrcMemberOverDate=MemberOverDate 
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID and IsAndroid=0


	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
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
			RETURN 1
		END
	END

	-- 密码判断
	IF @InsurePass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的银行密码不正确或者输入有误，请查证后再次尝试！'
		RETURN 3
	END

	-- 金币判断
	DECLARE @TransferPrerequisite AS BIGINT
	SELECT @TransferPrerequisite=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPrerequisite'
	IF @TransferPrerequisite IS NULL SET @TransferPrerequisite=0
	IF @lTransferScore<@TransferPrerequisite
	BEGIN
		SET @strErrorDescribe=N'从银行转账的游戏币数目不能少于 '+dbo.GSF_GR_Switch_Num(@TransferPrerequisite)+'，游戏币转账失败！'
		RETURN 4
	END

	-- 目标用户
	DECLARE @TargetUserID INT
	DECLARE @UserTransferRight AS INT
	DECLARE @UserRight AS INT
	SELECT @TargetUserID=UserID,@UserRight=UserRight,@DstMemberOverDate =MemberOverDate,@DstMemberOrder=MemberOrder FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE GameID=@dwGameID	
--	IF @cbByNickName=1
--		SELECT @TargetUserID=UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE NickName=@strNickName
--	ELSE
--		SELECT @TargetUserID=UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE GameID=@strNickName	

	-- 查询用户
	IF @TargetUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您所要转账的游戏ID:'+LTRIM(@dwGameID)+'不存在或者输入有误，请查证后再次尝试！'
		RETURN 5
	END

	-- 相同判断
	IF @TargetUserID=@dwUserID
	BEGIN
		SET @strErrorDescribe=N'不能使用自己的帐号为自己转账游戏币，请查证后再次尝试！'
		RETURN 6
	END
	
	
	-- 被转账权限
	/* DECLARE @SystemTransferRight AS INT
	SELECT @SystemTransferRight=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPower'
	IF @SystemTransferRight IS NULL SET @SystemTransferRight=1
	IF @SystemTransferRight=0 
	BEGIN
		SET @UserTransferRight = 1
	END
	ELSE
	BEGIN
		SET @UserTransferRight = @UserRight&0x4000000
	END
	IF @UserTransferRight =0
	BEGIN
		SET @strErrorDescribe=N'您所要转账的游戏ID，没有转账权限，请查证后再次尝试！'
		RETURN 7	
	END */

	-- 金币查询
	DECLARE @TargetScore BIGINT
	DECLARE @TargetInsure BIGINT
	SELECT @TargetScore=Score, @TargetInsure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@TargetUserID

	-- 插入判断
	IF @TargetScore IS NULL
	BEGIN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID,LastLogonIP,RegisterIP) VALUES (@TargetUserID,@strClientIP,@strClientIP)

		-- 游戏信息
		SELECT @TargetScore=Score, @TargetInsure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@TargetUserID
	END

	-- 银行服务比例	
	DECLARE @MaxTax BIGINT
	DECLARE @RevenueRate INT
	IF @MemberOrder = 0
	BEGIN
		SELECT @MaxTax=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferMaxTax'
		SELECT @RevenueRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransfer'

		-- 服务比例调整
		IF @RevenueRate IS NULL SET @RevenueRate=1
	END 
	ELSE
	BEGIN
		SELECT @MaxTax=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferMaxTax'
		--SELECT @RevenueRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransferMember'		
			SELECT @RevenueRate=InsureRate FROM  WHQJAccountsDB.dbo.MemberProperty where @MemberOrder =MemberOrder
		-- 服务比例调整
		IF @RevenueRate IS NULL SET @RevenueRate=0
	END	
	
	-- 服务比例调整
	IF @RevenueRate>1000 SET @RevenueRate=1000	

	-- 银行保留
	DECLARE @TransferRetention INT	-- 至少保留
	DECLARE @SurplusScore BIGINT	-- 转后银行
	SELECT @TransferRetention=ISNULL(StatusValue,0) FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferRetention'

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 游戏信息
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 金币判断
	IF @SourceInsure IS NULL OR @SourceInsure<@lTransferScore
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'您当前银行的游戏币余额不足，游戏币转账失败！'
		RETURN 4
	END

	-- 银行保留
	IF @TransferRetention<>0
	BEGIN
		SET @SurplusScore=@SourceInsure-@lTransferScore
		IF @SurplusScore<@TransferRetention
		BEGIN
			-- 结束事务
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			SET @strErrorDescribe=N'非常抱歉,转账后您银行的余额不能少于'+dbo.GSF_GR_Switch_Num(@TransferRetention)+'金币'
			RETURN 7
		END
	END

	-- 查询锁定
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- 设置信息
	SET @strErrorDescribe=N'转账 '+dbo.GSF_GR_Switch_Num(@lTransferScore)+' 游戏币到 '+LTRIM(@dwGameID)+'的银行操作成功，请查验您的帐户信息！'

	-- 计算变量
	SET @InsureRevenue=@lTransferScore*@RevenueRate/1000
	SET @VariationInsure=-@lTransferScore

	-- 服务比例封顶
	IF @MaxTax<>0
	BEGIN
		IF @InsureRevenue > @MaxTax
			SET @InsureRevenue=@MaxTax
	END

	-- 扣除金币
	UPDATE GameScoreInfo SET InsureScore=InsureScore+@VariationInsure,Revenue=Revenue+@InsureRevenue WHERE UserID=@dwUserID

	-- 增加金币
	UPDATE GameScoreInfo SET InsureScore=InsureScore+@lTransferScore-@InsureRevenue WHERE UserID=@TargetUserID

	-- 记录日志
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		TargetUserID,TargetGold,TargetBank,SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP,CollectNote)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@TargetUserID,@TargetScore,@TargetInsure,
		@lTransferScore,@InsureRevenue,0,3,@strClientIP,@strTransRemark)
		
	--插入流水日志
	DECLARE @DateTime DATETIME	
	DECLARE @SerialNumber NVARCHAR(20)
	SET @DateTime = GETDATE()

	IF @InsureRevenue>0 --银行服务费，如果有则记入金币流水
	BEGIN
		SELECT @SerialNumber=dbo.WF_GetSerialNumber()
		INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
		VALUES(@SerialNumber,0,@dwUserID,8,@SourceScore,@SourceInsure,-@InsureRevenue,@strClientIP,@DateTime)	
		SET @SourceInsure = @SourceInsure - @InsureRevenue
	END
	-- 转出方记录金币流水
	SELECT @SerialNumber=dbo.WF_GetSerialNumber()
	INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
	VALUES(@SerialNumber,0,@dwUserID,2,@SourceScore,@SourceInsure,-(@lTransferScore-@InsureRevenue),@strClientIP,DATEADD(ms,10,@DateTime))	
	-- 接收方记录金币流水
	SELECT @SerialNumber=dbo.WF_GetSerialNumber()
	INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
	VALUES(@SerialNumber,0,@TargetUserID,3,@TargetScore,@TargetInsure,@lTransferScore-@InsureRevenue,@strClientIP,DATEADD(ms,20,@DateTime))	
	
	SET @SourceInsure = @SourceInsure + @InsureRevenue

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 输出结果
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, 0 AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询银行
CREATE PROC GSP_GR_QueryUserInsureInfo
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @ServerID SMALLINT
DECLARE @UserRight INT

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @LogonPass AS NCHAR(32)
	SELECT @LogonPass=LogonPass FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的银行查询密码不正确，银行信息查询失败！'
		RETURN 1
	END

	-- 银行税率
	DECLARE @RevenueRateTake AS INT
	DECLARE @RevenueRateTransfer AS INT
	DECLARE @RevenueRateTransferMember AS INT
	DECLARE @TransferPrerequisite AS BIGINT  
	DECLARE	@EnjoinTransfer AS TINYINT
	
	DECLARE @BankOPButton1 BIGINT  
	DECLARE @BankOPButton2 BIGINT  
	DECLARE @BankOPButton3 BIGINT  

	SELECT @RevenueRateTake=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTake'
	SELECT @RevenueRateTransfer=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransfer'
	SELECT @RevenueRateTransferMember=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransferMember'
	SELECT @TransferPrerequisite=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPrerequisite'
	SELECT @EnjoinTransfer=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferStauts'

	SELECT @BankOPButton1=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankOPButton1'
	SELECT @BankOPButton2=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankOPButton2'
	SELECT @BankOPButton3=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankOPButton3'

	-- 参数调整
	IF @EnjoinTransfer IS NULL SET @EnjoinTransfer=0
	IF @RevenueRateTake IS NULL SET @RevenueRateTake=1
	IF @RevenueRateTransfer IS NULL SET @RevenueRateTransfer=1	
	IF @TransferPrerequisite IS NULL SET @TransferPrerequisite=0	
	IF @RevenueRateTransferMember IS NULL SET @RevenueRateTransferMember=0

	IF @BankOPButton1 IS NULL SET @BankOPButton1=100
	IF @BankOPButton2 IS NULL SET @BankOPButton2=1000
	IF @BankOPButton3 IS NULL SET @BankOPButton3=10000
	
	-- 查询房间
	SELECT @ServerID=ServerID FROM GameScoreLocker(NOLOCK) WHERE UserID=@dwUserID

	-- 查询积分
	SELECT @Score=Score, @Insure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	--查询权限
	SELECT @UserRight=UserRight FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @ServerID IS NULL SET @ServerID=0

	-- 输出结果
	SELECT @dwUserID AS UserID, @Score AS Score, @Insure AS Insure, @ServerID AS ServerID, @RevenueRateTake AS RevenueTake, 
		   @RevenueRateTransfer AS RevenueTransfer, @RevenueRateTransferMember AS RevenueTransferMember, @TransferPrerequisite AS TransferPrerequisite,
		   @EnjoinTransfer AS EnjoinTransfer,@UserRight AS UserRight, @BankOPButton1 AS BankOPButton1, @BankOPButton2 AS BankOPButton2, @BankOPButton3 AS BankOPButton3
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询返利
CREATE PROC GSP_GR_QueryTransferRebate
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32)						-- 用户密码

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	DECLARE @cbRebateEnabled AS SMALLINT
	DECLARE @lIngot AS INT
	DECLARE @lLoveLiness AS INT
 
	SELECT @cbRebateEnabled=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferRebate'
	IF @cbRebateEnabled IS NULL SET @cbRebateEnabled=0	
		
	-- 输出结果
	SELECT @dwUserID AS UserID, @cbRebateEnabled AS RebateEnabled 

END

RETURN 0

GO

-- 查询返利
CREATE PROC GSP_GR_GetAllKindMoney
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	DECLARE @MiddleMoney AS BIGINT
	DECLARE @Type AS TINYINT
	DECLARE @lToScoreRatio AS INT
	DECLARE @lToDiamondRatio AS INT
 	DECLARE @lToBeanRatio AS INT

	DECLARE @lInsure BIGINT
	DECLARE @lDiamond BIGINT

	SET @MiddleMoney = 0
	SET @Type = 0
	SET @lToScoreRatio = 0
	SET @lToDiamondRatio = 0
	SET @lToBeanRatio = 0

	--代币转金币变换比例
	SELECT @lToScoreRatio=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MiddleToScore'
	IF @lToScoreRatio IS NULL SET @lToScoreRatio=0

	--代币转钻石变换比例
	SELECT @lToDiamondRatio=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MiddleToDiamond'
	IF @lToDiamondRatio IS NULL SET @lToDiamondRatio=0


	-- 统计金币
	SELECT @MiddleMoney = SUM(MiddleMoney) FROM MiddleMoney  WHERE UserID = @dwUserID AND MoneyType = 0
	UPDATE GameScoreInfo SET InsureScore = InsureScore +@MiddleMoney * @lToScoreRatio WHERE UserID = @dwUserID
	SELECT @lInsure = InsureScore FROM GameScoreInfo WHERE UserID = @dwUserID

	-- 统计钻石
	SET @MiddleMoney = 0;
	SELECT @MiddleMoney = SUM(MiddleMoney) FROM MiddleMoney  WHERE UserID = @dwUserID AND MoneyType = 1
	UPDATE UserCurrency SET Diamond = Diamond +@MiddleMoney * @lToDiamondRatio WHERE UserID = @dwUserID
	SELECT @lDiamond = Diamond FROM UserCurrency WHERE UserID = @dwUserID

	IF @lInsure IS NULL SET  @lInsure = 0
	IF @lDiamond IS NULL SET @lDiamond = 0

	DELETE FROM MiddleMoney WHERE UserID = @dwUserID
		
	-- 输出结果
	SELECT @lInsure AS Insure, @lDiamond AS Diamond 

END

RETURN 0

GO

-- 查询返利
CREATE PROC GSP_GR_GetUserWealth
	@dwUserID INT,								-- 用户 I D
	@strDynamicPassword NCHAR(32),				-- 用户密码
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量声明
	DECLARE @lScore BIGINT
	DECLARE @lInsure BIGINT
	DECLARE @lDiamond BIGINT
	DECLARE @lAwardTicket INT

	-- 携带与银行 金币
	SELECT @lScore = Score, @lInsure = InsureScore FROM GameScoreInfo WHERE UserID = @dwUserID

	-- 砖石与奖券
	SELECT @lDiamond = Diamond, @lAwardTicket = AwardTicket FROM UserCurrency WHERE UserID = @dwUserID

	IF @lScore IS NULL SET  @lScore = 0
	IF @lInsure IS NULL SET @lInsure = 0
	IF @lDiamond IS NULL SET @lDiamond = 0
	IF @lAwardTicket IS NULL SET @lAwardTicket = 0

	-- 输出结果
	SELECT @lScore AS Score, @lInsure AS Insure, @lDiamond AS Diamond , @lAwardTicket AS AwardTicket

END

RETURN 0

GO


Create function GSF_GR_Switch_Num(@lnum BIGINT)
returns nvarchar(100)
as
BEGIN
	DECLARE @szReS nvarchar(100)
	SET @szReS=N''
	DECLARE @lTemp BIGINT
	DECLARE @cbFlag TINYINT
	SET @cbFlag=0
	IF(@lnum<0)
	BEGIN
		Set @lnum=-1*@lnum
		SET @cbFlag=1;
	END
	IF(@lnum>=100000000)
	BEGIN
		SET @lTemp=@lnum/100000000
		SET @szReS=LTRIM(RTRIM(STR(@lTemp)))+N'亿'
		SET @lnum=@lnum-@lTemp*100000000
	END
	IF(@lnum>=10000)
	BEGIN
		SET @lTemp=@lnum/10000
		SET @szReS=@szReS+LTRIM(RTRIM(STR(@lTemp)))+N'万'
		SET @lnum=@lnum-@lTemp*10000
	END
	IF(@lnum>0)
	BEGIN
		SET @szReS=@szReS+LTRIM(RTRIM(STR(@lnum)))
	END

	IF(@cbFlag=1)
		SET @szReS=N'-'+@szReS
	RETURN @szRes
END


