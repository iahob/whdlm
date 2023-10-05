
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO



IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RealAuth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RealAuth]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 实名认证
CREATE PROC GSP_GP_RealAuth
	@dwUserID INT,								-- 用户 I D	
	@strPassword NCHAR(32),						-- 用户密码
	@strCompellation NVARCHAR(16),				-- 真实名字
	@strPassPortID NVARCHAR(18),				-- 证件号码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询信息
	DECLARE @DynamicPass NCHAR(32)
	SELECT @DynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@dwUserID

	-- 用户判断
	IF @DynamicPass IS NULL
	BEGIN 
		SET @strErrorDescribe=N'用户信息不存在，认证失败！'
		RETURN 1
	END

	-- 密码判断
	IF @DynamicPass<>@strPassword
	BEGIN 
		SET @strErrorDescribe=N'帐号密码不正确，认证失败！'
		RETURN 3
	END

	--简单验证
	IF LEN(@strPassPortID) = 0 OR LEN(@strCompellation) = 0
	BEGIN
		SET @strErrorDescribe=N'认证失败，请输入有效的证件号码！'
		RETURN 4	
	END	
	
	--重复验证
	IF EXISTS (SELECT UserID FROM WHQJRecordDB.dbo.RecordAuthentPresent WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe=N'已认证成功，请勿重复人认证！'
		RETURN 5		
	END
	
	-- 实名修改
	UPDATE AccountsInfo SET  Compellation=@strCompellation, PassPortID=@strPassPortID 	WHERE UserID=@dwUserID
	
	-- 成功判断
	IF @@ROWCOUNT=0
	BEGIN
		SET @strErrorDescribe=N'执行实名认证错误，请联系客户服务中心！'
		RETURN 6
	END
	
	-- 实名记录
	INSERT INTO WHQJRecordDB.dbo.RecordAuthentPresent(UserID,PassPortID,Compellation,IpAddress) VALUES(@dwUserID,@strPassPortID,@strCompellation,@strClientIP)	
	
	--查询奖励
	DECLARE @SourceDiamond BIGINT
	DECLARE @AuthentPresentCount AS BIGINT
	SELECT @AuthentPresentCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'JJRealNameAuthentPresent'
	IF @AuthentPresentCount IS NULL or @AuthentPresentCount=0
	BEGIN
		SET @strErrorDescribe=N'恭喜您，认证成功！'
		
		SELECT @SourceDiamond=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID	
		SELECT @SourceDiamond AS Diamond
		
		RETURN 0		
	END	
	
		
	--发放奖励
	--DECLARE @DateID INT
	--SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	-- 赠送钻石
	UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@AuthentPresentCount WHERE UserID=@dwUserID	
	SELECT @SourceDiamond=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
	
	--插入日志
	DECLARE @DateTime DATETIME
	--DECLARE @Random VARCHAR(5)
	DECLARE @SerialNumber NVARCHAR(20)
	SET @DateTime = GETDATE()
	--SET @Random = CAST(FLOOR(89999*RAND()+10000) AS VARCHAR(5))
	SELECT @SerialNumber=dbo.WF_GetSerialNumber()
	
	INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
	VALUES(@SerialNumber,0,@dwUserID,6,@SourceDiamond-@AuthentPresentCount,@AuthentPresentCount,@strClientIP,@DateTime)
	-- 钻石变化记录
	--INSERT INTO WHQJRecordDB..RecordGrantDiamond(MasterID, UserID, TypeID, CurDiamond, AddDiamond, ClientIP, CollectDate, CollectNote)
	--	VALUES (0, @dwUserID, 1, 0, @AuthentPresentCount, @strClientIP, GETDATE(), '实名赠送钻石')
	---- 流水账
	--INSERT INTO  WHQJTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
	--VALUES (@dwUserID,@CurrScore,@CurrInsure,@AuthentPresentCount,8,@strClientIP,GETDATE())
	
	-- 日统计
	--UPDATE  WHQJTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@AuthentPresentCount WHERE UserID=@dwUserID AND TypeID=8
	--IF @@ROWCOUNT=0
	--BEGIN
	--	INSERT  WHQJTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,8,1,@AuthentPresentCount)
	--END	
	---- 游戏信息
	--DECLARE @SourceScore BIGINT
	--SELECT @SourceScore=Score FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID	
	--SELECT @SourceScore AS SourceScore
	
	---- 游戏信息			
	SELECT @SourceDiamond AS Diamond
	SET @strErrorDescribe=N'恭喜您，认证成功！奖励钻石'+ CAST(@AuthentPresentCount AS NVARCHAR)  +N'！'							
	
	RETURN 0

END

GO

----------------------------------------------------------------------------------------------------
