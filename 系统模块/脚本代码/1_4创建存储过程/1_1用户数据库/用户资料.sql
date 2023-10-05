
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryUserIndividual]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyUserIndividual]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_BindSpreader]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_BindSpreader]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UpdatePlaceName]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UpdatePlaceName]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 查询资料
CREATE PROC GSP_GP_QueryUserIndividual
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @DynamicPass AS NCHAR(32)

	-- 查询用户
	SELECT @DynamicPass=DynamicPass FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 密码判断
	IF @DynamicPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的用户密码不正确，个人信息查询失败！'
		RETURN 1
	END

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @QQ NVARCHAR(16)
	DECLARE @EMail NVARCHAR(33)
	DECLARE @UserNote NVARCHAR(256)
	DECLARE @SeatPhone NVARCHAR(32)
	DECLARE @MobilePhone NVARCHAR(16)
	DECLARE @Compellation NVARCHAR(16)
	DECLARE @DwellingPlace NVARCHAR(128)
	DECLARE @PassPortID NVARCHAR(18)
	DECLARE	@Spreader NVARCHAR(31)
	
	SET @QQ=N''	
	SET @EMail=N''	
	SET @UserNote=N''	
	SET @SeatPhone=N''	
	SET @MobilePhone=N''	
	SET @Compellation=N''	
	SET @DwellingPlace=N''	
	SET @PassPortID=N''		
	SET @Spreader=N''

	-- 查询用户
	SELECT @UserID=UserID, @QQ=QQ, @EMail=EMail, @UserNote=UserNote, @SeatPhone=SeatPhone,@MobilePhone=MobilePhone, @DwellingPlace=DwellingPlace FROM IndividualDatum(NOLOCK) WHERE UserID=@dwUserID

	-- 查询姓名
	DECLARE @SpreaderID INT	
	SELECT @Compellation=Compellation,@PassPortID = PassPortID,@SpreaderID = SpreaderID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- 查询推广
	SELECT @Spreader = GAMEID FROM AccountsInfo  WHERE UserID=@SpreaderID

	-- 输出信息
	SELECT @dwUserID AS UserID,
	       @Compellation AS Compellation,
	       @PassPortID AS PassPortID, 
	       @QQ AS QQ, 
	       @EMail AS EMail, 
	       @SeatPhone AS SeatPhone,
		   @MobilePhone AS MobilePhone,
		   @DwellingPlace AS DwellingPlace, 
		   @UserNote AS UserNote,
		   @Spreader AS Spreader

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新资料
CREATE PROC GSP_GP_ModifyUserIndividual
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@cbGender TINYINT,							-- 用户性别
	@strNickName NVARCHAR(32),					-- 用户昵称
	@strUnderWrite NVARCHAR(63),				-- 个性签名
	@strCompellation NVARCHAR(16),				-- 真实名字
	@strPassPortID NVARCHAR(18),				-- 证件号码	
	@strQQ NVARCHAR(16),						-- Q Q 号码
	@strEMail NVARCHAR(33),						-- 电子邮电
	@strSeatPhone NVARCHAR(32),					-- 固定电话
	@strMobilePhone NVARCHAR(16),				-- 移动电话
	@strDwellingPlace NVARCHAR(128),			-- 详细地址
	@strUserNote NVARCHAR(256),					-- 用户备注	
	@strSpreader NVARCHAR(31),					-- 推荐帐号				
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @NickName NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	-- 金币变量
	DECLARE @SourceScore BIGINT

	-- 查询用户
	SELECT @UserID=UserID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown
	FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
		RETURN 1
	END	

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
	
	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 3
	END

	-- 效验昵称
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickName)>0)>0
	BEGIN
		SET @strErrorDescribe=N'您所输入的游戏昵称名含有限制字符串，请更换昵称名后再次修改！'
		RETURN 4
	END

	-- 存在判断
	--IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName AND UserID<>@UserID)
	--BEGIN
	--	SET @strErrorDescribe=N'此昵称已被其他玩家使用了，请更换昵称名后再次修改！'
	--	RETURN 4
	--END
		
	-- 修改资料
	UPDATE AccountsInfo SET NickName=@strNickName, UnderWrite=@strUnderWrite, Gender=@cbGender	WHERE UserID=@dwUserID	
		
	-- 昵称记录
--	IF @NickName<>@strNickName
--	BEGIN
--		INSERT INTO WHQJRecordDB.dbo.RecordAccountsExpend(UserID,ReAccounts,ClientIP)
--		VALUES(@dwUserID,@strNickName,@strClientIP)
--	END

	-- 修改资料
	UPDATE IndividualDatum SET QQ=@strQQ, EMail=@strEMail, SeatPhone=@strSeatPhone,
		MobilePhone=@strMobilePhone, DwellingPlace=@strDwellingPlace, UserNote=@strUserNote WHERE UserID=@dwUserID
		
	-- 修改资料
	IF @@ROWCOUNT=0
	INSERT IndividualDatum (UserID, QQ, EMail, SeatPhone, MobilePhone, DwellingPlace, UserNote)
	VALUES (@dwUserID, @strQQ, @strEMail, @strSeatPhone, @strMobilePhone, @strDwellingPlace, @strUserNote)

	-- 设置信息
	IF @@ERROR=0 SET @strErrorDescribe=N'个性签名已更新'

	
		
	RETURN 0

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- 绑定推广
CREATE PROC GSP_GP_BindSpreader
	@dwUserID INT,								-- 用户 I D
	@dwSpreaderID INT,							-- 推广 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @ExistSpreadID INT
	-- 金币变量
	DECLARE @SourceScore BIGINT

	-- 查询用户
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown, @ExistSpreadID = SpreaderID
	FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
		RETURN 1
	END	

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
	
	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 3
	END
	
	-- 判断该账号 是否已经绑定推广
	IF @ExistSpreadID = 0 OR @ExistSpreadID IS NULL
	BEGIN
		SET @strErrorDescribe=N'该账号已经绑定推广员，不能再次绑定！'
		RETURN 3
	END

	-- 查推广员
	DECLARE @SpreaderID INT	
	IF @dwSpreaderID<>0
	BEGIN
		-- 查推广员
		SELECT @SpreaderID=UserID FROM AccountsInfo(NOLOCK) WHERE GameID = @dwSpreaderID 
		
		-- 结果处理
		IF @SpreaderID IS NULL
		BEGIN
			SET @SpreaderID=0
			SET @strErrorDescribe=N'推广员ID错误或不存在，请填写正确的推广员帐号！'
			RETURN 4
		END
		
		-- 查询重复	
		IF EXISTS (SELECT SpreaderID FROM AccountsInfo WHERE @SpreaderID=SpreaderID and UserID=@dwUserID)
		BEGIN
			SET @SpreaderID=0
			SET @strErrorDescribe=N'已经提交推广员，提交失败！'
			RETURN 5
		END	
	END
	ELSE
	BEGIN
		SET @strErrorDescribe=N'推广员ID错误或不存在，请填写正确的推广员帐号！'
		RETURN 4
	END
	
	-- 获取推广员对应代理ID
	DECLARE @nAgentID INT

	SELECT @nAgentID = AgentID FROM AccountsInfo WHERE  UserID=@SpreaderID
	IF @nAgentID IS NULL SET @nAgentID = 0

	-- 绑定代理
	UPDATE AccountsInfo SET AgentID = @nAgentID WHERE UserID = @dwUserID

	-- 推广提成
	IF @SpreaderID<>0 AND @SpreaderID IS NOT NULL
	BEGIN
		DECLARE @RegisterGrantScore INT
		DECLARE @Note NVARCHAR(512)
		DECLARE @dwDiamond INT	
		DECLARE @dwRewardDiamond INT
			
		SET @Note = N'推广员'
		SELECT @RegisterGrantScore = RegisterGrantScore FROM  WHQJTreasureDB.dbo.GlobalSpreadInfo
		IF @RegisterGrantScore IS NULL
		BEGIN
			SET @RegisterGrantScore=5000
		END
		INSERT INTO  WHQJTreasureDB.dbo.RecordSpreadInfo(
			UserID,Score,TypeID,ChildrenID,CollectNote)
		VALUES(@SpreaderID,@RegisterGrantScore,1,@dwUserID,@Note)	
		
		-- 推荐记录
		UPDATE AccountsInfo SET SpreaderID = @SpreaderID WHERE UserID=@dwUserID		
		
		-- 赠送钻石
		SELECT @dwRewardDiamond=StatusValue FROM SystemStatusInfo WHERE StatusName = N'JJBindSpreadPresent'
		IF @dwRewardDiamond IS NULL
		BEGIN
			SET @dwRewardDiamond=0
			SET @strErrorDescribe=N'绑定成功！'
			RETURN 7
		END
		ELSE
		BEGIN
			DECLARE @CurrentDiamond INT
			SELECT @CurrentDiamond=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
			--更新钻石
			IF @CurrentDiamond IS NOT NULL
			BEGIN
				UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@dwRewardDiamond WHERE UserID=@dwUserID
			END
			ELSE
			BEGIN
				SET @CurrentDiamond=0
				INSERT INTO  WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond)
				VALUES(@dwUserID,@dwRewardDiamond)
			END
			
			--插入日志
			DECLARE @DateTime DATETIME
			--DECLARE @Random VARCHAR(5)
			DECLARE @SerialNumber NVARCHAR(20)
			SET @DateTime = GETDATE()
			--SET @Random = CAST(FLOOR(89999*RAND()+10000) AS VARCHAR(5))
			SELECT @SerialNumber=dbo.WF_GetSerialNumber()
			
			INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@dwUserID,4,@CurrentDiamond,@dwRewardDiamond,@strClientIP,@DateTime)
			
			SET @dwDiamond=@CurrentDiamond+@dwRewardDiamond
			SET @strErrorDescribe=N'绑定成功,恭喜获得'++CONVERT(NVARCHAR,@dwRewardDiamond)++N'钻石奖励'
			
			SELECT @dwDiamond AS dwDiamond, @dwRewardDiamond AS dwRewardDiamond
			RETURN 0
		END
	END
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 更新地名
CREATE PROC GSP_GP_UpdatePlaceName
	@dwUserID INT,								-- 用户 I D
	@szPlaceName NVARCHAR(33),					-- 更新地名	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 修改地名
	UPDATE AccountsInfo SET PlaceName=@szPlaceName	WHERE UserID=@dwUserID		

	SET @strErrorDescribe=N'更新成功！'
END

RETURN 0
GO