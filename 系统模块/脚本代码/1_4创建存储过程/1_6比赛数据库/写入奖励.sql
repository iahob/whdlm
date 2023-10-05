USE WHQJGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 比赛奖励
CREATE PROC GSP_GR_MatchReward
	@dwUserID INT,								-- 用户 I D	
	@lRewardGold BIGINT,						-- 金币奖励
	@lRewardIngot BIGINT,						-- 元宝钻石
	@dwRewardTicket BIGINT,					-- 奖券奖励
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@strClientIP NVARCHAR(15)					-- 连接地址
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 奖励金币
	IF @lRewardGold>0 OR @lRewardIngot>0 OR @dwRewardTicket>0
	BEGIN
		-- 查询金币
		DECLARE @CurrScore BIGINT
		DECLARE @CurrInsure BIGINT
		DECLARE @CurrDiamond BIGINT
		DECLARE @CurrAwardTicket BIGINT
		SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID
		SELECT @CurrDiamond=Diamond, @CurrAwardTicket = AwardTicket FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID
	
		-- 存在判断
		IF @CurrScore IS NULL OR @CurrInsure IS NULL 
		BEGIN
			-- 调整变量
			SELECT @CurrScore=0,@CurrInsure=0
			
			-- 插入记录
			INSERT WHQJTreasureDB..GameScoreInfo (UserID,Score,LastLogonIP,RegisterIP) 
			VALUES(@dwUserID,@lRewardGold,@strClientIP,@strClientIP)
		END
		

		-- 计算时间
		DECLARE @DateID INT
		SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
		
		-- 变量定义
		DECLARE @TypeID INT
		SET @TypeID=18
		
		-- 更新金币
		UPDATE WHQJTreasureDB..GameScoreInfo SET Score=Score+@lRewardGold WHERE UserID=@dwUserID

		-- 记录金币流水记录
		DECLARE @DateTime DATETIME
		DECLARE @SerialNumber NVARCHAR(20)
		SET @DateTime = GETDATE()
		SELECT @SerialNumber=dbo.WF_GetSerialNumber()

		IF @lRewardGold IS NOT NULL AND @lRewardGold <> 0
		BEGIN
			INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@dwUserID,15,@CurrScore,@CurrInsure,@lRewardGold,@strClientIP,@DateTime)	
		END
		
		IF @lRewardIngot IS NOT NULL AND @lRewardIngot <> 0 
		BEGIN
			-- 存在判断
			IF @CurrDiamond IS NULL
			BEGIN
				-- 插入记录
				INSERT WHQJTreasureDB..UserCurrency (UserID,Diamond) 
				VALUES(@dwUserID,@lRewardIngot)


				--钻石日志	
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,17,@CurrDiamond,@lRewardIngot,@strClientIP,@DateTime)
			END
			ELSE
			BEGIN
				-- 更新钻石和奖券
				UPDATE WHQJTreasureDB..UserCurrency SET Diamond=Diamond+@lRewardIngot WHERE UserID=@dwUserID

							--钻石日志	
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,17,@CurrDiamond,@lRewardIngot,@strClientIP,@DateTime)

			END
		END


		IF @dwRewardTicket IS NOT NULL AND @dwRewardTicket <> 0 
		BEGIN
			-- 存在判断
			IF @dwRewardTicket IS NULL
			BEGIN
				-- 插入记录
				INSERT WHQJTreasureDB..UserCurrency (UserID,AwardTicket) 
				VALUES(@dwUserID,@dwRewardTicket)


				-- 奖劵记录
				INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
				VALUES (@SerialNumber,0,@dwUserID,5,0,@dwRewardTicket,@strClientIP,GETDATE())
			END
			ELSE
			BEGIN
				-- 更新钻石和奖券
				UPDATE WHQJTreasureDB..UserCurrency SET AwardTicket=AwardTicket+@dwRewardTicket WHERE UserID=@dwUserID

				-- 奖劵记录
				INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
				VALUES (@SerialNumber,0,@dwUserID,5,@CurrAwardTicket,@dwRewardTicket,@strClientIP,GETDATE())

			END
		END



	END
	
	-- 查询金币
	DECLARE @CurrGold BIGINT	
	SELECT @CurrGold=Score FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID

	-- 调整数据
	IF @CurrGold IS NULL SET @CurrGold=0		

	-- 抛出数据
	SELECT @CurrGold AS Score
END

RETURN 0
GO