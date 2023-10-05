
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGamePropertyTypeItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGamePropertyTypeItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGamePropertyRelatItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGamePropertyRelatItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGamePropertyItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGamePropertyItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGamePropertySubItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGamePropertySubItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_BuyProperty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_BuyProperty]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_BuyGameProperty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_BuyGameProperty]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadUserGameBuff]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadUserGameBuff]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadUserGameTrumpet]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadUserGameTrumpet]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QuerySendPresent]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QuerySendPresent]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserSendPresentByID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserSendPresentByID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserSendPresentByNickName]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserSendPresentByNickName]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_GetSendPresent]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_GetSendPresent]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_PropertQuerySingle]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_PropertQuerySingle]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载类型
CREATE PROC GSP_GP_LoadGamePropertyTypeItem
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	SELECT * FROM GamePropertyType(NOLOCK) WHERE Nullity=0 AND TagID = 0
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载关系
CREATE PROC GSP_GP_LoadGamePropertyRelatItem
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	SELECT * FROM GamePropertyRelat(NOLOCK) WHERE  TagID = 0
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载道具
CREATE PROC GSP_GP_LoadGamePropertyItem
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 加载道具
	SELECT * FROM GameProperty(NOLOCK) WHERE Nullity=0 AND Kind <> 11
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载道具
CREATE PROC GSP_GP_LoadGamePropertySubItem
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 加载道具
	SELECT * FROM GamePropertySub(NOLOCK) 
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


-- 消费道具
CREATE PROC GSP_GP_BuyProperty
	@dwUserID INT,								-- 用户标识
	@dwPropertyID INT,							-- 道具标识
	@cbUseKind SMALLINT,						-- 消耗类型
	@dwPropCount INT,							-- 道具数量
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @ExchangeType TINYINT
	DECLARE @ExchangeRatio INT
	DECLARE @iProPerty BIGINT	
	DECLARE @PropertyName AS NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @CurrMemberOrder SMALLINT	
	
	
	SELECT @Nullity=Nullity FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword

	-- 用户判断
	IF @Nullity IS NULL
	BEGIN
		SET @strErrorDescribe=N'帐号不存在，请联系客户服务中心了解详细情况！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END

	-- 初始信息
	SET	@iProPerty= 0
	SET @ExchangeType = 0
	SET @ExchangeRatio = 0

	-- 道具判断
	SELECT @PropertyName=Name,@ExchangeType=ExchangeType, @ExchangeRatio=ExchangeRatio FROM GameProperty(NOLOCK) WHERE Nullity=0 AND ID=@dwPropertyID  AND ExchangeType = @cbUseKind
	IF @PropertyName IS NULL
	BEGIN
		SET @strErrorDescribe=N'购买的道具商品不存在或者正在维护中！'
		RETURN 3
	END
		
	IF @ExchangeType = 1 AND @cbUseKind=0
	BEGIN
		SET @strErrorDescribe=N'本道具不支持钻石购买方式！'
		RETURN 3
	END
	
	IF @ExchangeType = 0 AND @cbUseKind=1
	BEGIN
		SET @strErrorDescribe=N'本道具不支持金币购买方式！'
		RETURN 3
	END

	IF @dwPropCount < 1
	BEGIN
		SET @strErrorDescribe=N'购买道具数量有误！'
		RETURN 8
	END
	
	-- 消耗计算
	SET @iProPerty=@dwPropCount*@ExchangeRatio
	
	DECLARE @TotalDiamond BIGINT
	DECLARE @Gold AS BIGINT
	-- 道具计算
	IF @cbUseKind=0
	BEGIN
		-- 读取银行
		SELECT @TotalDiamond=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID

		-- 数量判断
		IF @TotalDiamond<@iProPerty OR @TotalDiamond IS NULL
		BEGIN
			-- 错误信息
			SET @strErrorDescribe=N'钻石不足，请选择其他的购买方式或者往银行存入足够的钻石后再次尝试！'
			RETURN 4
		END
		-- 扣掉钻石	
		UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond-@iProPerty WHERE UserID=@dwUserID
	END
	ELSE 
	BEGIN
		--读取金币
		SELECT @Gold=Score FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
				
		-- 数量判断
		IF @Gold<@iProPerty OR @Gold IS NULL
		BEGIN
			-- 错误信息
			SET @strErrorDescribe=N'金币不足，请选择其他的购买方式！'
			RETURN 4
		END
		-- 金币扣费
		UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score=Score-@iProPerty WHERE UserID=@dwUserID	
	END
	
	
	--加入背包
	UPDATE  WHQJAccountsDB.dbo.AccountsPackage SET GoodShowID=0, GoodsCount=GoodsCount+ @dwPropCount, PushTime=GETDATE() WHERE UserID=@dwUserID AND GoodsID=@dwPropertyID
	IF @@ROWCOUNT = 0
	BEGIN
		INSERT  WHQJAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsCount,PushTime)	VALUES(@dwUserID, @dwPropertyID,0,@dwPropCount,GETDATE())
	END

	--插入日志
	DECLARE @DateTime DATETIME
	DECLARE @SerialNumber NVARCHAR(20)
	SET @DateTime = GETDATE()
	SELECT @SerialNumber=dbo.WF_GetSerialNumber()
	
	IF @cbUseKind=0
	BEGIN
		INSERT WHQJRecordDB.dbo.RecordBuyNewProperty(UserID,PropertyID,PropertyName,ExchangeType,ExchangeRatio,BuyNum,BeforeCurrency,Currency,ClinetIP,CollectDate)
		VALUES(@dwUserID,@dwPropertyID,@PropertyName,@ExchangeType,@ExchangeRatio,@dwPropCount,@TotalDiamond,@iProPerty,@strClientIP,GETDATE())
		
		--钻石日志	
		INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
		VALUES(@SerialNumber,0,@dwUserID,9,@TotalDiamond,-@iProPerty,@strClientIP,@DateTime)
	END
	ELSE
	BEGIN
		INSERT WHQJRecordDB.dbo.RecordBuyNewProperty(UserID,PropertyID,PropertyName,ExchangeType,ExchangeRatio,BuyNum,BeforeCurrency,Currency,ClinetIP,CollectDate)
		VALUES(@dwUserID,@dwPropertyID,@PropertyName,@ExchangeType,@ExchangeRatio,@dwPropCount,@Gold,@iProPerty,@strClientIP,GETDATE())

		--金币日志		
		DECLARE @CurInsureScore AS BIGINT
		--读取银行
		SELECT @CurInsureScore=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
		
		INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
		VALUES(@SerialNumber,0,@dwUserID,4,@Gold,@CurInsureScore,-@iProPerty,@strClientIP,@DateTime)		
	END
	
	SET @strErrorDescribe=N'恭喜您！购买' + @PropertyName + N' ×' + CAST(@dwPropCount AS NVARCHAR) +N' ' + N'成功！'
	
	DECLARE @lDiamond BIGINT
	DECLARE @lScore AS BIGINT
	SELECT @lDiamond=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID = @dwUserID
	SELECT @lScore=Score FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID = @dwUserID
	IF @lDiamond IS NULL SET @lDiamond=0
	IF @lScore IS NULL SET @lScore=0
	--输出记录
	SELECT @dwPropertyID AS PropertyID,@dwPropCount AS ItemCount,@lDiamond AS Diamond,@lScore AS Score 
	
	RETURN 0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


-- 消费道具
CREATE PROC GSP_GP_BuyGameProperty
	@dwUserID INT,								-- 用户标识
	@dwPropertyID INT,							-- 道具标识
	@cbUseKind SMALLINT,						-- 消耗类型
	@dwPropCount INT,							-- 道具数量
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @ExchangeType TINYINT
	DECLARE @ExchangeRatio INT
	DECLARE @iProPerty BIGINT	
	DECLARE @PropertyName AS NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @CurrMemberOrder SMALLINT	
	
	
	SELECT @Nullity=Nullity FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword

	-- 用户判断
	IF @Nullity IS NULL
	BEGIN
		SET @strErrorDescribe=N'帐号不存在，请联系客户服务中心了解详细情况！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END

	-- 初始信息
	SET	@iProPerty= 0
	SET @ExchangeType = 0
	SET @ExchangeRatio = 0

	-- 道具判断
	SELECT @PropertyName=Name,@ExchangeType=ExchangeType, @ExchangeRatio=ExchangeRatio FROM GameProperty(NOLOCK) WHERE Nullity=0 AND ID=@dwPropertyID
	IF @PropertyName IS NULL
	BEGIN
		SET @strErrorDescribe=N'购买的道具商品不存在或者正在维护中！'
		RETURN 3
	END
		
	IF @ExchangeType = 1 AND @cbUseKind=0
	BEGIN
		SET @strErrorDescribe=N'本道具不支持钻石购买方式！'
		RETURN 3
	END
	
	IF @ExchangeType = 0 AND @cbUseKind=1
	BEGIN
		SET @strErrorDescribe=N'本道具不支持金币购买方式！'
		RETURN 3
	END

	IF @dwPropCount < 1
	BEGIN
		SET @strErrorDescribe=N'购买道具数量有误！'
		RETURN 8
	END
	
	-- 消耗计算
	SET @iProPerty=@dwPropCount*@ExchangeRatio
	
	DECLARE @TotalDiamond BIGINT
	DECLARE @InsureGold AS BIGINT
	-- 道具计算
	IF @cbUseKind=0
	BEGIN
		-- 读取银行
		SELECT @TotalDiamond=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID

		-- 数量判断
		IF @TotalDiamond<@iProPerty OR @TotalDiamond IS NULL
		BEGIN
			-- 错误信息
			SET @strErrorDescribe=N'钻石不足，请选择其他的购买方式或者往银行存入足够的钻石后再次尝试！'
			RETURN 4
		END
		-- 扣掉钻石	
		UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond-@iProPerty WHERE UserID=@dwUserID
	END
	ELSE 
	BEGIN
		--读取金币
		SELECT @InsureGold=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
				
		-- 数量判断
		IF @InsureGold<@iProPerty OR @InsureGold IS NULL
		BEGIN
			-- 错误信息
			SET @strErrorDescribe=N'银行金币不足，请选择其他的购买方式！'
			RETURN 4
		END
		-- 金币扣费
		UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET InsureScore=InsureScore-@iProPerty WHERE UserID=@dwUserID	
	END
	
	
	--加入背包
	UPDATE  WHQJAccountsDB.dbo.AccountsPackage SET GoodShowID=0, GoodsCount=GoodsCount+ @dwPropCount, PushTime=GETDATE() WHERE UserID=@dwUserID AND GoodsID=@dwPropertyID
	IF @@ROWCOUNT = 0
	BEGIN
		INSERT  WHQJAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsCount,PushTime)	VALUES(@dwUserID, @dwPropertyID,0,@dwPropCount,GETDATE())
	END

	--插入日志
	DECLARE @DateTime DATETIME
	DECLARE @SerialNumber NVARCHAR(20)
	SET @DateTime = GETDATE()
	SELECT @SerialNumber=dbo.WF_GetSerialNumber()
	
	IF @cbUseKind=0
	BEGIN
		INSERT WHQJRecordDB.dbo.RecordBuyNewProperty(UserID,PropertyID,PropertyName,ExchangeType,ExchangeRatio,BuyNum,BeforeCurrency,Currency,ClinetIP,CollectDate)
		VALUES(@dwUserID,@dwPropertyID,@PropertyName,@ExchangeType,@ExchangeRatio,@dwPropCount,@TotalDiamond,@iProPerty,@strClientIP,GETDATE())
	
		--钻石日志	
		INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
		VALUES(@SerialNumber,0,@dwUserID,9,@TotalDiamond,-@iProPerty,@strClientIP,@DateTime)
	END
	ELSE
	BEGIN
		INSERT WHQJRecordDB.dbo.RecordBuyNewProperty(UserID,PropertyID,PropertyName,ExchangeType,ExchangeRatio,BuyNum,BeforeCurrency,Currency,ClinetIP,CollectDate)
		VALUES(@dwUserID,@dwPropertyID,@PropertyName,@ExchangeType,@ExchangeRatio,@dwPropCount,@InsureGold,@iProPerty,@strClientIP,GETDATE())
		
		--金币日志		
		DECLARE @CurInsureScore AS BIGINT
		DECLARE @CurScore AS BIGINT
		--读取银行
		SELECT @CurScore = Score, @CurInsureScore=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
		
		INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
		VALUES(@SerialNumber,0,@dwUserID,4,@CurScore,@CurInsureScore,-@iProPerty,@strClientIP,@DateTime)		
	END
	
	SET @strErrorDescribe=N'恭喜您！购买' + @PropertyName + N' ×' + CAST(@dwPropCount AS NVARCHAR) +N' ' + N'成功！'
	
	DECLARE @lDiamond BIGINT
	DECLARE @lScore  BIGINT
	DECLARE @lInsureScore  BIGINT
	SELECT @lDiamond=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID = @dwUserID
	SELECT @lInsureScore=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID = @dwUserID
	IF @lDiamond IS NULL SET @lDiamond=0
	IF @lScore IS NULL SET @lScore=0
	--输出记录
	SELECT @dwPropertyID AS PropertyID,@dwPropCount AS ItemCount,@lDiamond AS Diamond,@lScore AS Score , @lInsureScore AS InsureScore
	
	RETURN 0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

-- 加载Buff
CREATE PROC GSP_GP_LoadUserGameBuff
	@dwUserID	INT						-- 用户 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--删除过期
	DELETE  WHQJAccountsDB.dbo.AccountsInfoPoperty WHERE UserID=@dwUserID and dateadd(second,UseResultsValidTime,UseTime) < GETDATE()
	--有效查询
	SELECT * FROM  WHQJAccountsDB.dbo.AccountsInfoPoperty  WHERE UserID=@dwUserID and dateadd(second,UseResultsValidTime,UseTime) > GETDATE()
	RETURN 0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

-- 加载Buff
CREATE PROC GSP_GP_LoadUserGameTrumpet
	@dwUserID	INT						-- 用户 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	DECLARE @TrumpetCount INT 
	DECLARE @TyphonCount INT 
	SET @TrumpetCount = 0
	SET @TyphonCount = 0
	SELECT @TrumpetCount=GoodsCount FROM  WHQJAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=307
	SELECT @TyphonCount=GoodsCount FROM  WHQJAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=306
	SELECT @TrumpetCount as TrumpetCount, @TyphonCount as TyphonCount
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

--赠送道具
CREATE PROC GSP_GP_QuerySendPresent
	@dwUserID				INT			-- 用户ID
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	SELECT * FROM  WHQJAccountsDB.dbo.AccountsSendPresent AS a, GameProperty AS b 
	WHERE a.ReceiverUserID=@dwUserID and a.PropID = b.ID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

--赠送道具 按照游戏ID
CREATE PROC GSP_GP_UserSendPresentByID
	@dwUserID				INT,			-- 赠送者用户ID
	@dwReceiverGameID		INT,			-- 接收者游戏ID
	@dwPropID				INT,			-- 赠送的道具ID
	@dwPropCount			INT,			-- 赠送的数量
	@strClientIP			NVARCHAR(15),	-- 赠送者的IP 
	@strErrorDescribe NVARCHAR(127) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	--查询用户
	DECLARE @dwReceiverUserID INT
	SELECT @dwReceiverUserID=UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE GameID=@dwReceiverGameID
	IF @dwReceiverUserID is null
	BEGIN
		set @strErrorDescribe = N'查找不到接收者信息'
		RETURN 1
	END

	if @dwPropCount <= 0
	BEGIN
		set @strErrorDescribe = N'输入的道具数量信息有误'
		RETURN 2
	END
	
	IF @dwReceiverUserID=@dwUserID
	BEGIN
		set @strErrorDescribe = N'不能自己赠送给自己'
		RETURN 3
	END
	
	--查找道具
	DECLARE @UserPropCount INT
	SELECT @UserPropCount=GoodsCount FROM  WHQJAccountsDB.dbo.AccountsPackage where UserID=@dwUserID and GoodsID=@dwPropID
	IF @UserPropCount is null or @UserPropCount < @dwPropCount
	BEGIN
		set @strErrorDescribe = N'赠送者的背包没有足够的该道具，不能赠送'
		RETURN 4
	END
	
	--减掉道具
	UPDATE  WHQJAccountsDB.dbo.AccountsPackage set GoodsCount=GoodsCount-@dwPropCount where UserID=@dwUserID and GoodsID=@dwPropID
	
	--插入状态
	INSERT  WHQJAccountsDB.dbo.AccountsSendPresent(UserID, ReceiverUserID, PropID, PropCount, SendTime, PropStatus, ClientIP) 
	VALUES (@dwUserID, @dwReceiverUserID, @dwPropID, @dwPropCount, GETDATE(), 0, @strClientIP)
	
	--赠送记录
	INSERT WHQJRecordDB.dbo.RecordUserSendPresent(UserID,ReceiverUserID,PropID,PropCount,SendTime,PropStatus,ClientIP)
	VALUES(@dwUserID,@dwReceiverUserID,@dwPropID,@dwPropCount,GETDATE(),0,@strClientIP)
	
	set @strErrorDescribe = N'赠送成功'
	
	SELECT @dwReceiverUserID AS RecvUserID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

--赠送道具 按照 昵称
CREATE PROC GSP_GP_UserSendPresentByNickName
	@dwUserID				INT,			-- 赠送者用户ID
	@strReceiverNickName	NVARCHAR(16),	-- 接收者昵称
	@dwPropID				INT,			-- 赠送的道具ID
	@dwPropCount			INT,			-- 赠送的数量
	@strClientIP			NVARCHAR(16),	-- 赠送者的IP 
	@strErrorDescribe NVARCHAR(64) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	--查询用户
	DECLARE @dwReceiverUserID INT
	SELECT @dwReceiverUserID=UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE NickName=@strReceiverNickName
	IF @dwReceiverUserID is null
	BEGIN
		set @strErrorDescribe = N'查找不到接收者信息'
		RETURN 1
	END
	
	if @dwPropCount <= 0
	BEGIN
		set @strErrorDescribe = N'输入的道具数量信息有误'
		RETURN 2
	END
	
	IF @dwReceiverUserID=@dwUserID
	BEGIN
		set @strErrorDescribe = N'不能自己赠送给自己'
		RETURN 3
	END
	
	--查找赠送者背包有没有该数量的道具
	DECLARE @UserPropCount INT
	SELECT @UserPropCount=GoodsCount FROM  WHQJAccountsDB.dbo.AccountsPackage where UserID=@dwUserID and GoodsID=@dwPropID
	IF @UserPropCount is null or @UserPropCount < @dwPropCount
	BEGIN
		set @strErrorDescribe = N'赠送者的背包没有足够的该道具，不能赠送'
		RETURN 4
	END
	
	--减掉赠送者背包的道具
	UPDATE  WHQJAccountsDB.dbo.AccountsPackage set GoodsCount=GoodsCount-@dwPropCount where UserID=@dwUserID and GoodsID=@dwPropID
	
	--写用户赠送状态
	INSERT  WHQJAccountsDB.dbo.AccountsSendPresent(UserID, ReceiverUserID, PropID, PropCount, SendTime, PropStatus, ClientIP) 
	VALUES (@dwUserID, @dwReceiverUserID, @dwPropID, @dwPropCount, GETDATE(), 0, @strClientIP)
	
	--赠送记录
	INSERT WHQJRecordDB.dbo.RecordUserSendPresent(UserID,ReceiverUserID,PropID,PropCount,SendTime,PropStatus,ClientIP)
	VALUES(@dwUserID,@dwReceiverUserID,@dwPropID,@dwPropCount,GETDATE(),0,@strClientIP)
	
	set @strErrorDescribe = N'赠送成功'
	
	SELECT @dwReceiverUserID AS RecvUserID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

--获取赠送
SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

--赠送道具
CREATE PROC GSP_GP_GetSendPresent
	@dwUserID				INT,			-- 用户ID
	@szPassword			NVARCHAR(33),		-- 用户密码
	@strClientIP		NVARCHAR(15)		-- 客户IP 
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	DECLARE @Password NVARCHAR(33)
	SELECT @Password = LogonPass FROM  WHQJAccountsDB.dbo.AccountsInfo where UserID=@dwUserID
	--查询错误
	IF @Password is null
		RETURN 1
	
	--密码错误
	IF @szPassword != @Password
		RETURN 2
	
	DECLARE @PresentCount INT
	SELECT @PresentCount = COUNT(*) FROM  WHQJAccountsDB.dbo.AccountsSendPresent
	WHERE ReceiverUserID=@dwUserID and PropStatus=0
	--没有物品
	IF @PresentCount = 0
		RETURN 3
	
	--放入背包
	DECLARE @PropID INT
	DECLARE @PropCount INT
	DECLARE @ItemCount INT
	DECLARE auth_cur CURSOR FOR
	SELECT PropID,PropCount FROM  WHQJAccountsDB.dbo.AccountsSendPresent AS a, GameProperty AS b 
	WHERE a.ReceiverUserID=@dwUserID and a.PropID = b.ID and a.PropStatus=0
	OPEN auth_cur
	FETCH NEXT FROM auth_cur INTO @PropID,@PropCount
	WHILE (@@fetch_status=0)
	BEGIN
		SELECT @ItemCount=COUNT(*) FROM  WHQJAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=@PropID
		IF @ItemCount = 0
		BEGIN
			INSERT  WHQJAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsSortID,GoodsCount,PushTime)
			VALUES(@dwUserID,@PropID,0,0,@PropCount,GETDATE())
		END
		ELSE
		BEGIN
			UPDATE  WHQJAccountsDB.dbo.AccountsPackage SET GoodsCount=GoodsCount+@PropCount
			WHERE UserID=@dwUserID and GoodsID=@PropID
		END
		FETCH NEXT FROM auth_cur INTO @PropID,@PropCount
	END
	close auth_cur
	deallocate auth_cur
	
	--更新状态
	UPDATE WHQJRecordDB.dbo.RecordUserSendPresent SET ReceiveTime=GETDATE(), ReceiverClientIP=@strClientIP, PropStatus=1 
	WHERE  ReceiverUserID = @dwUserID and PropStatus=0
	
	--查询返回
	SELECT * FROM  WHQJAccountsDB.dbo.AccountsSendPresent AS a, GameProperty AS b 
	WHERE a.ReceiverUserID=@dwUserID and a.PropID = b.ID and a.PropStatus=0
	
	--删除状态
	DELETE  WHQJAccountsDB.dbo.AccountsSendPresent WHERE  ReceiverUserID=@dwUserID and PropStatus=0
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

--赠送道具
CREATE PROC GSP_GP_PropertQuerySingle
	@dwUserID INT,								-- 用户标识
	@dwPropertyID INT,							-- 道具标识
	@strPassword NCHAR(32)						-- 用户密码
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	DECLARE @Nullity INT
	SELECT @Nullity=Nullity FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND DynamicPass=@strPassword

	-- 用户判断
	IF @Nullity IS NULL
	BEGIN
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		RETURN 2
	END
	DECLARE @GoodsCount INT
	SET @GoodsCount = 0
	SELECT @GoodsCount = GoodsCount	FROM  WHQJAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=@dwPropertyID
	
	SELECT @dwPropertyID AS PropertyID,@GoodsCount AS ItemCount
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------