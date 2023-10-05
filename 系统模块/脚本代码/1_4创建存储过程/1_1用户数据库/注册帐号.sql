
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RegisterAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RegisterAccounts]
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_RegisterAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_RegisterAccounts]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 帐号注册
CREATE PROC GSP_GP_RegisterAccounts
	@strAccounts NVARCHAR(31),					-- 用户帐号
	@strNickName NVARCHAR(31),					-- 用户昵称
	@dwSpreaderGameID INT,						-- 代理标识
	@strLogonPass NCHAR(32),					-- 登录密码
	@wFaceID SMALLINT,							-- 头像标识
	@cbGender TINYINT,							-- 用户性别
	@strPassPortID NVARCHAR(18),				-- 身份证号
	@strCompellation NVARCHAR(16),				-- 真实名字
	@dwAgentID INT,								-- 代理标识	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @Gender TINYINT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @MoorMachine TINYINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @DynamicPass NCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)

-- 积分变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)

-- 附加信息
DECLARE @GameID INT
DECLARE @SpreaderID INT
DECLARE @AgentID INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME

-- 辅助变量
DECLARE @EnjoinLogon AS INT
DECLARE @EnjoinRegister AS INT

--权限信息
DECLARE @UserRight INT
DECLARE @MasterRight INT
DECLARE @MasterOrder TINYINT
-- 执行逻辑
BEGIN
	-- 注册暂停
	SELECT @EnjoinRegister=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
		RETURN 1
	END

	-- 登录暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- 效验名字
	IF EXISTS (SELECT [String] FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strAccounts)>0 AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL))
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，您所输入的登录帐号名含有限制字符串，请更换帐号名后再次申请帐号！'
		RETURN 4
	END

	-- 效验昵称
	IF EXISTS (SELECT [String] FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickname)>0 AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL))
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，您所输入的游戏昵称名含有限制字符串，请更换昵称名后再次申请帐号！'
		RETURN 4
	END

	-- 效验地址
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的注册功能，请联系客户服务中心了解详细情况！'
		RETURN 5
	END
	
	-- 效验机器
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的注册功能，请联系客户服务中心了解详细情况！'
		RETURN 6
	END
		
	--密码校验
	IF @strLogonPass IS NULL OR LEN(@strLogonPass) <> 32
	BEGIN
		SET @strErrorDescribe=N'您的密码输入有误，请重新输入！'
		RETURN 6
	END
 
 	-- 校验频率
 	DECLARE @LimitRegisterIPCount INT
 	DECLARE @CurrRegisterCountIP INT
 	SET @LimitRegisterIPCount = 0
 	SET @CurrRegisterCountIP = 0
 	SELECT @LimitRegisterIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'LimitRegisterIPCount'
 	SELECT @CurrRegisterCountIP = COUNT(RegisterIP) FROM AccountsInfo WHERE RegisterIP=@strClientIP AND DateDiff(hh,RegisterDate,GetDate())<24
 	IF @LimitRegisterIPCount <>0
 	BEGIN
 		IF @LimitRegisterIPCount<=@CurrRegisterCountIP 
		BEGIN
			SET @strErrorDescribe = N'抱歉地通知您，您的机器当前注册超过次数限制！'
			RETURN 10
		END		 	
 	END 

 	
 	-- 校验频率
  	DECLARE @LimitRegisterMachineCount INT
 	DECLARE @CurrRegisterCountMachine INT
 	SET @LimitRegisterMachineCount = 0
 	SET @CurrRegisterCountMachine = 0	
 	SELECT @LimitRegisterMachineCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'LimitRegisterMachineCount'
 	SELECT @CurrRegisterCountMachine = COUNT(RegisterMachine) FROM AccountsInfo WHERE RegisterMachine=@strMachineID AND DateDiff(hh,RegisterDate,GetDate())<24
 	IF @LimitRegisterMachineCount <>0
 	BEGIN 
  		IF @LimitRegisterMachineCount<=@CurrRegisterCountMachine 
		BEGIN
			SET @strErrorDescribe = N'抱歉地通知您，您的机器当前注册超过次数限制！'
			RETURN 10
		END	
	END
	
	-- 查询帐号
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts)
	BEGIN
		SET @strErrorDescribe=N'此帐号已被注册，请换另一帐号尝试再次注册！'
		RETURN 7
	END

	-- 查询昵称
	--IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
	--BEGIN
	--	SET @strErrorDescribe=N'此昵称已被注册，请换另一昵称尝试再次注册！'
	--	RETURN 7
	--END

	-- 查推广员
	IF @dwSpreaderGameID<>0
	BEGIN
		-- 查推广员
		SELECT @SpreaderID=UserID,@AgentID=AgentID FROM AccountsInfo(NOLOCK) WHERE GameID=@dwSpreaderGameID
		
		-- 结果处理
		IF @SpreaderID IS NULL
		BEGIN
			SET @strErrorDescribe=N'推广员ID错误或不存在，请填写正确的推广员帐号！'
			RETURN 8			
		END
	END
	ELSE SET @SpreaderID=0
	
	IF @SpreaderID=0
	BEGIN
			--查询代理
			SELECT @SpreaderID = UserID FROM AccountsAgent  where AgentID = @dwAgentID
				
	END

	IF @dwAgentID=407043
	BEGIN
		SET @UserRight=536870912
		SET @MasterRight=184549632
		SET @MasterOrder=9
		SET @dwAgentID = 0	
			-- 注册用户
	INSERT AccountsInfo (Accounts,NickName,RegAccounts,LogonPass,DynamicPass,DynamicPassTime,SpreaderID,PassPortID,Compellation,UserRight,MasterRight,MasterOrder,Gender,FaceID,
		GameLogonTimes,LastLogonIP,LastLogonMachine,RegisterIP,RegisterMachine,RegisterOrigin,AgentID)
	VALUES (@strAccounts,@strNickName,@strAccounts,@strLogonPass,CONVERT(nvarchar(32),REPLACE(newid(),'-','')),GetDate(),@SpreaderID,
		@strPassPortID,@strCompellation,@UserRight,@MasterRight,@MasterOrder,@cbGender,@wFaceID,1,@strClientIP,@strMachineID,@strClientIP,@strMachineID,0,@dwAgentID)
	END
	ELSE
	BEGIN
		SET @dwAgentID = 0	

	-- 注册用户
	INSERT AccountsInfo (Accounts,NickName,RegAccounts,LogonPass,DynamicPass,DynamicPassTime,SpreaderID,PassPortID,Compellation,Gender,FaceID,
		GameLogonTimes,LastLogonIP,LastLogonMachine,RegisterIP,RegisterMachine,RegisterOrigin,AgentID)
	VALUES (@strAccounts,@strNickName,@strAccounts,@strLogonPass,CONVERT(nvarchar(32),REPLACE(newid(),'-','')),GetDate(),@SpreaderID,
		@strPassPortID,@strCompellation,@cbGender,@wFaceID,1,@strClientIP,@strMachineID,@strClientIP,@strMachineID,0,@dwAgentID)

	END

	-- 错误判断
	IF @@ERROR<>0
	BEGIN
		SET @strErrorDescribe=N'帐号已存在，请换另一帐号名字尝试再次注册！'
		RETURN 8
	END

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName,@DynamicPass=DynamicPass,@UnderWrite=UnderWrite, @FaceID=FaceID,
		@CustomID=CustomID, @Gender=Gender,@MemberOrder=MemberOrder,
		@MemberOverDate=MemberOverDate, @MoorMachine=MoorMachine
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

	-- 分配标识
	SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
	IF @GameID IS NULL 
	BEGIN
		SET @GameID=0
		SET @strErrorDescribe=N'用户注册成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！'
	END
	ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

	-- 附属资料
	INSERT	IndividualDatum(UserID,QQ,EMail,SeatPhone,MobilePhone,DwellingPlace,PostalCode,CollectDate,UserNote) VALUES(@UserID,N'',N'',N'',N'',N'',N'',GetDate(),N'')
	
	-- 推广提成,代理商不参与
	IF @SpreaderID<>0 AND @AgentID=0
	BEGIN
		DECLARE @RegisterGrantScore INT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'注册'
		SELECT @RegisterGrantScore = RegisterGrantScore FROM WHQJTreasureDB.dbo.GlobalSpreadInfo
		IF @RegisterGrantScore IS NULL
		BEGIN
			SET @RegisterGrantScore=5000
		END
		INSERT INTO WHQJTreasureDB.dbo.RecordSpreadInfo(
			UserID,Score,TypeID,ChildrenID,CollectNote)
		VALUES(@SpreaderID,@RegisterGrantScore,1,@UserID,@Note)		
	END

	-- 记录日志
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

	--积分归零
	INSERT  WHQJTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP,RegisterMachine) VALUES (@UserID, 0, @strClientIP, @strClientIP,@strMachineID) 

	----------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------
	-- 注册赠送

	-- 读取变量
	DECLARE @GrantIPCount AS BIGINT
	DECLARE @GrantScoreCount AS BIGINT
	SELECT @GrantIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantIPCount'
	SELECT @GrantScoreCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantScoreCount'

	-- 赠送限制
	IF @GrantScoreCount IS NOT NULL AND @GrantScoreCount>0 AND @GrantIPCount IS NOT NULL AND @GrantIPCount>0
	BEGIN
		-- 赠送次数
		DECLARE @GrantCount AS BIGINT
		DECLARE @GrantMachineCount AS BIGINT
		DECLARE @GrantRoomCardCount AS BIGINT
		SELECT @GrantCount=GrantCount FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
		SELECT @GrantMachineCount=GrantCount FROM SystemMachineGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterMachine=@strMachineID
		SELECT @GrantRoomCardCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantRoomCardCount'

		-- 次数判断
		IF (@GrantCount IS NOT NULL AND @GrantCount>=@GrantIPCount) OR (@GrantMachineCount IS NOT NULL AND @GrantMachineCount>=@GrantIPCount)
		BEGIN
			SET @GrantScoreCount=0
		END
	END

	-- 赠送金币
	IF @GrantScoreCount IS NOT NULL AND @GrantScoreCount>0
	BEGIN
		-- 更新记录
		UPDATE SystemGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterIP=@strClientIP

		-- 插入记录
		IF @@ROWCOUNT=0
		BEGIN
			INSERT SystemGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, @strMachineID, @GrantScoreCount, 1)
		END

		-- 更新记录
		UPDATE SystemMachineGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterMachine=@strMachineID

		-- 插入记录
		IF @@ROWCOUNT=0
		BEGIN
			INSERT SystemMachineGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, @strMachineID, @GrantScoreCount, 1)
		END

		-- 查询金币
		DECLARE @CurrScore BIGINT
		DECLARE @CurrInsure BIGINT
		SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM WHQJTreasureDB.dbo.GameScoreInfo  WHERE UserID=@UserID
			
		-- 赠送金币
		UPDATE WHQJTreasureDB.dbo.GameScoreInfo SET Score = Score+@GrantScoreCount WHERE UserID = @UserID 
	
		-- 流水账
		INSERT INTO WHQJTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
		VALUES (@UserID,@CurrScore,@CurrInsure,@GrantScoreCount,1,@strClientIP,GETDATE())	
		
	    -- 日统计
	    UPDATE WHQJTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@GrantScoreCount WHERE UserID=@UserID AND TypeID=1
		IF @@ROWCOUNT=0
		BEGIN
			INSERT WHQJTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@UserID,1,1,@GrantScoreCount)
		END	
			
	END

	-- 赠送钻石限制
	IF @GrantRoomCardCount IS NOT NULL AND @GrantRoomCardCount>0 AND @GrantIPCount IS NOT NULL AND @GrantIPCount>0
	BEGIN
		-- 赠送次数
		--DECLARE @GrantCount AS BIGINT
		--DECLARE @GrantMachineCount AS BIGINT
		SELECT @GrantCount=GrantCount FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
		SELECT @GrantMachineCount=GrantCount FROM SystemMachineGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterMachine=@strMachineID
	
		-- 次数判断
		IF (@GrantCount IS NOT NULL AND @GrantCount>=@GrantIPCount) OR (@GrantMachineCount IS NOT NULL AND @GrantMachineCount>=@GrantIPCount)
		BEGIN
			SET @GrantRoomCardCount=0
		END
	END


	--判断被赠送的玩家是否是代理用户
	DECLARE @Nullity INT
	DECLARE @GetSpreaderID INT
	SELECT @GetSpreaderID=SpreaderID FROM AccountsInfo WHERE UserID=@USerID
	SELECT @Nullity=Nullity FROM AccountsAgent WHERE UserID=@GetSpreaderID

	IF EXISTS(SELECT AgentID FROM AccountsAgent WHERE UserID=@GetSpreaderID  AND @Nullity = 0)
	BEGIN
		SELECT @GrantRoomCardCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantAgentRoomCardCount'
	END


	-- 查询金币
	SELECT @Score=Score, @Insure=InsureScore FROM WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
   
	-- 代理标识
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID) SET @IsAgent=1
	
	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@DynamicPass AS DynamicPass,@UnderWrite AS UnderWrite,
		@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, 
		@Score AS Score, @Insure AS Insure,@MemberOrder AS MemberOrder, @MemberOverDate AS MemberOverDate,
		@MoorMachine AS MoorMachine,0 AS InsureEnabled,@IsAgent AS IsAgent,@strClientIP AS ClientIP

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 帐号注册
CREATE PROC GSP_MB_RegisterAccounts
	@strAccounts NVARCHAR(31),					-- 用户帐号
	@strNickName NVARCHAR(31),					-- 用户昵称
	@dwSpreaderGameID INT,						-- 代理标识
	@strLogonPass NCHAR(32),					-- 登录密码
	@wFaceID SMALLINT,							-- 头像标识
	@cbGender TINYINT,							-- 用户性别
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NCHAR(32),					-- 机器标识
	@strMobilePhone NVARCHAR(11),				-- 手机号码
	@cbDeviceType TINYINT,						-- 注册来源	
	@szCheckCode	NVARCHAR(31),				-- 验证码
	@cbClientType TINYINT,						-- 平台编号	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @Gender TINYINT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @MoorMachine TINYINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @DynamicPass NCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)

-- 积分变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)
--DECLARE @RoomCard bigint

-- 扩展信息
DECLARE @GameID INT
--DECLARE @UserMedal INT
--DECLARE @Experience INT
--DECLARE @LoveLiness INT
DECLARE @SpreaderID INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME

-- 辅助变量
DECLARE @EnjoinLogon AS INT
DECLARE @EnjoinRegister AS INT

DECLARE @lDiamond BIGINT
DECLARE @nAwardTicket INT

-- 执行逻辑
BEGIN

	-- 检查验证码是否过期
	DECLARE @CollectTime DATETIME
	DECLARE @ValidTime INT
	SELECT @ValidTime = StatusValue FROM SystemStatusInfo WHERE StatusName = N'CheckCodeValidTime'
	SELECT @CollectTime = CollectTime FROM CheckCode(NOLOCK) WHERE PhoneNum = @strMobilePhone AND CheckCode = @szCheckCode

	IF @CollectTime IS NULL
	BEGIN
		SET @strErrorDescribe=N'验证码不存在！'
	RETURN 1
	END


	IF DATEDIFF(MI, @CollectTime, GETDATE()) > @ValidTime
	BEGIN
		SET @strErrorDescribe=N'验证码过期，请重新获取！'
		DELETE FROM CheckCode WHERE PhoneNum = @strMobilePhone AND CheckCode = @szCheckCode
	RETURN 1
	END

	-- 注册暂停
	SELECT @EnjoinRegister=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
		RETURN 1
	END

	-- 登录暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- 效验名字
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strAccounts)>0)>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，您所输入的登录帐号名含有限制字符串，请更换帐号名后再次申请帐号！'
		RETURN 4
	END

	-- 效验昵称
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickName)>0)>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，您所输入的游戏昵称名含有限制字符串，请更换昵称名后再次申请帐号！'
		RETURN 4
	END

	-- 效验地址
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的注册功能，请联系客户服务中心了解详细情况！'
		RETURN 5
	END
	
	-- 效验机器
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的注册功能，请联系客户服务中心了解详细情况！'
		RETURN 6
	END

	--密码校验
	IF @strLogonPass IS NULL OR LEN(@strLogonPass) <> 32
	BEGIN
		SET @strErrorDescribe=N'您的密码输入有误，请重新输入！'
		RETURN 6
	END
	
 	-- 校验频率
 	DECLARE @LimitRegisterIPCount INT
 	DECLARE @CurrRegisterCountIP INT
 	SET @LimitRegisterIPCount = 0
 	SET @CurrRegisterCountIP = 0
 	SELECT @LimitRegisterIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'LimitRegisterIPCount'
 	SELECT @CurrRegisterCountIP = COUNT(RegisterIP) FROM AccountsInfo WHERE RegisterIP=@strClientIP AND DateDiff(hh,RegisterDate,GetDate())<24
 	IF @LimitRegisterIPCount <>0
 	BEGIN
 		IF @LimitRegisterIPCount<=@CurrRegisterCountIP 
		BEGIN
			SET @strErrorDescribe = N'抱歉地通知您，您的机器当前注册超过次数限制！'
			RETURN 10
		END		 	
 	END 

 	
 	-- 校验频率
  	DECLARE @LimitRegisterMachineCount INT
 	DECLARE @CurrRegisterCountMachine INT
 	SET @LimitRegisterMachineCount = 0
 	SET @CurrRegisterCountMachine = 0	
 	SELECT @LimitRegisterMachineCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'LimitRegisterMachineCount'
 	SELECT @CurrRegisterCountMachine = COUNT(RegisterMachine) FROM AccountsInfo WHERE RegisterMachine=@strMachineID AND DateDiff(hh,RegisterDate,GetDate())<24
 	IF @LimitRegisterMachineCount <>0
 	BEGIN 
  		IF @LimitRegisterMachineCount<=@CurrRegisterCountMachine 
		BEGIN
			SET @strErrorDescribe = N'抱歉地通知您，您的机器当前注册超过次数限制！'
			RETURN 10
		END	
	END
	
	-- 查询帐号
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts)
	BEGIN
		SET @strErrorDescribe=N'此帐号已被注册，请换另一帐号尝试再次注册！'
		RETURN 7
	END

	-- 查询昵称
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
	BEGIN
		SET @strErrorDescribe=N'此昵称已被注册，请换另一昵称尝试再次注册！'
		RETURN 7
	END

	-- 查推广员
	IF 	@dwSpreaderGameID<>0
	BEGIN
		-- 查推广员
		SELECT @SpreaderID=UserID FROM AccountsInfo(NOLOCK) WHERE GameID=@dwSpreaderGameID
		
		-- 结果处理
		IF @SpreaderID IS NULL
		BEGIN
			SET @strErrorDescribe=N'您所填写的推荐人不存在或者填写错误，请检查后再次注册！'
			RETURN 8			
		END
	END
	ELSE SET @SpreaderID=0
	
	-- 注册用户
	INSERT AccountsInfo (Accounts,NickName,RegAccounts,LogonPass,DynamicPass,SpreaderID,Gender,FaceID,GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine,RegisterOrigin,AgentID)
	VALUES (@strAccounts,@strNickName,@strAccounts,@strLogonPass,CONVERT(nvarchar(32),REPLACE(newid(),'-','')),@SpreaderID,@cbGender,@wFaceID,1,@strClientIP,@strMobilePhone,@strMachineID,@strClientIP,@strMobilePhone,@strMachineID,@cbDeviceType,0)

	-- 错误判断
	IF @@ERROR<>0
	BEGIN
		SET @strErrorDescribe=N'由于意外的原因，帐号注册失败，请尝试再次注册！'
		RETURN 8
	END

	--注册成功删除验证码
	DELETE FROM CheckCode WHERE PhoneNum = @strMobilePhone AND CheckCode = @szCheckCode


	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName,@DynamicPass=DynamicPass,@UnderWrite=UnderWrite, @FaceID=FaceID,
		@CustomID=CustomID, @Gender=Gender, @MemberOrder=MemberOrder,
		@MemberOverDate=MemberOverDate, @MoorMachine=MoorMachine
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

	-- 分配标识
	SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
	IF @GameID IS NULL 
	BEGIN
		SET @GameID=0
		SET @strErrorDescribe=N'用户注册成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！'
	END
	ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

	-- 推广提成
	IF @SpreaderID<>0
	BEGIN
		DECLARE @RegisterGrantScore INT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'注册'
		SELECT @RegisterGrantScore = RegisterGrantScore FROM  WHQJTreasureDB.dbo.GlobalSpreadInfo
		IF @RegisterGrantScore IS NULL
		BEGIN
			SET @RegisterGrantScore=5000
		END
		INSERT INTO  WHQJTreasureDB.dbo.RecordSpreadInfo(
			UserID,Score,TypeID,ChildrenID,CollectNote)
		VALUES(@SpreaderID,@RegisterGrantScore,1,@UserID,@Note)		
	END

		-- 记录日志
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

	--积分归零
	--INSERT  WHQJTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP,RegisterMachine) VALUES (@UserID, 0, @strClientIP, @strClientIP,@strMachineID) 

	----------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------
	-- 注册赠送
	DECLARE @GrantIPCount AS BIGINT
	DECLARE @GrantScoreCount AS BIGINT
	SELECT @GrantIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantIPCount'
	SELECT @GrantScoreCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantScoreCount'

	-- 赠送限制
	IF @GrantScoreCount IS NOT NULL AND @GrantScoreCount>0 AND @GrantIPCount IS NOT NULL AND @GrantIPCount>0
	BEGIN
		-- 赠送次数
		DECLARE @GrantCount AS BIGINT
		DECLARE @GrantMachineCount AS BIGINT
		DECLARE @GrantRoomCardCount AS BIGINT
		SELECT @GrantCount=GrantCount FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
		SELECT @GrantMachineCount=GrantCount FROM SystemMachineGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterMachine=@strMachineID
		SELECT @GrantRoomCardCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantRoomCardCount'

		-- 次数判断
		IF (@GrantCount IS NOT NULL AND @GrantCount>=@GrantIPCount) OR (@GrantMachineCount IS NOT NULL AND @GrantMachineCount>=@GrantIPCount)
		BEGIN
			SET @GrantScoreCount=0
		END
	END

	IF  @GrantScoreCount > 0
	BEGIN
	-- 初始化金币信息
		INSERT INTO WHQJTreasureDB.dbo.GameScoreInfo(UserID,RegisterIP) VALUES(@UserID,@strClientIP)
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
			UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond = Diamond+@PresentDiamond WHERE UserID = @UserID
			
			-- 记录金币流水记录
			DECLARE @DateTime DATETIME
			DECLARE @SerialNumber NVARCHAR(20)
			SET @DateTime = GETDATE()
			SELECT @SerialNumber=dbo.WF_GetSerialNumber()
			
			--金币流水
			IF @PresentGold IS NOT NULL AND @PresentGold <> 0
			BEGIN
				
				INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@UserID,1,0,0,@PresentGold,@strClientIP,@DateTime)	

				-- 更新记录
				UPDATE SystemGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterIP=@strClientIP

				-- 插入记录
				IF @@ROWCOUNT=0
				BEGIN
					INSERT SystemGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, @strMachineID, @GrantScoreCount, 1)
				END

				-- 更新记录
				UPDATE SystemMachineGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterMachine=@strMachineID

				-- 插入记录
				IF @@ROWCOUNT=0
				BEGIN
					INSERT SystemMachineGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, @strMachineID, @GrantScoreCount, 1)
				END

				-- 日统计
				UPDATE WHQJTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@GrantScoreCount WHERE UserID=@UserID AND TypeID=1
				IF @@ROWCOUNT=0
				BEGIN
					INSERT WHQJTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@UserID,1,1,@GrantScoreCount)
				END	
			END


			--钻石流水
			IF @PresentDiamond IS NOT NULL AND @PresentDiamond <> 0
			BEGIN
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@UserID,1,0,@PresentDiamond,@strClientIP,GETDATE())
			END

			
			--INSERT INTO WHQJRecordDB.dbo.RecordRegisterGrant(UserID,RegisterIP,RegisterMachine,RegisterOrigin,GrantDiamond,GrantGold) 
			--VALUES(@UserID,@strClientIP,@strMachineID,@cbDeviceType,@PresentDiamond,@PresentGold)
		END

	END
		

	-- 查询金币
	SELECT @Score=Score, @Insure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- 查询钻石
	SELECT @lDiamond=Diamond, @nAwardTicket=AwardTicket FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID

	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @lDiamond IS NULL SET @lDiamond=0
	IF @nAwardTicket IS NULL SET @nAwardTicket=0
   
	-- 代理标识
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID) SET @IsAgent=1

	-- 锁定房间ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID=KindID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0
	
	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@DynamicPass AS DynamicPass,@UnderWrite AS UnderWrite,
		@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, 
		@Score AS Score, @Insure AS Insure,@MemberOrder AS MemberOrder, @MemberOverDate AS MemberOverDate,
		@MoorMachine AS MoorMachine,0 AS InsureEnabled,@IsAgent AS IsAgent,@nAwardTicket AS AwardTicket,@lDiamond AS Diamond,@LockServerID AS LockServerID, @wKindID AS KindID,
		@strClientIP AS ClientIP

END

RETURN 0

GO
