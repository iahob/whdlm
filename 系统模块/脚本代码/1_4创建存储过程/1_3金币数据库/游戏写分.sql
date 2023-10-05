
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteGameScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 游戏写分
CREATE PROC GSP_GR_WriteGameScore

	-- 用户信息
	@dwUserID INT,								-- 用户 I D
	@dwDBQuestID INT,							-- 请求标识
	@dwInoutIndex INT,							-- 进出索引

	-- 变更成绩
	@lVariationScore BIGINT,					-- 用户分数
	@lVariationGrade BIGINT,					-- 用户成绩
	@lVariationInsure BIGINT,					-- 用户银行
	@lVariationRevenue BIGINT,					-- 游戏服务比例
	@lVariationChipSerialScore BIGINT,			-- 用户流水
	@lVariationWinCount INT,					-- 胜利盘数
	@lVariationLostCount INT,					-- 失败盘数
	@lVariationDrawCount INT,					-- 和局盘数
	@lVariationFleeCount INT,					-- 逃跑数目
	@lVariationIntegralCount BIGINT,			-- 游戏积分	
	@lVariationUserMedal INT,					-- 用户奖牌
	@lVariationExperience INT,					-- 用户经验
	@lVariationLoveLiness INT,					-- 用户魅力
	@dwVariationPlayTimeCount INT,				-- 游戏时间
	@dwVariationGroupID INT,					-- 大联盟ID
	-- 属性信息	
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@strClientIP NVARCHAR(15)					-- 连接地址

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	IF @dwVariationGroupID <> 0
	BEGIN
		--俱乐部分数
		DECLARE @CurScore BIGINT
		SELECT @CurScore = Score FROM   WHQJGroupDB.dbo.IMGroupMember WHERE UserID=@dwUserID and GroupID=@dwVariationGroupID
		IF @CurScore IS NULL
			SET @CurScore=0

		UPDATE   WHQJGroupDB.dbo.IMGroupMember Set Score=Score+@lVariationScore WHERE UserID=@dwUserID and GroupID=@dwVariationGroupID
		INSERT INTO   WHQJGroupDB.dbo.RecordUserScoreChange(GroupID,UserID,BeforeScore,ChangeType,ChangeScore,AfterScore) 
			VALUES(@dwVariationGroupID,@dwUserID,@CurScore,2,@lVariationScore,@CurScore+@lVariationScore)
	END
	ELSE
	BEGIN
		-- 用户积分
		UPDATE GameScoreInfo SET Score=Score+@lVariationScore, Revenue=Revenue+@lVariationRevenue, InsureScore=InsureScore+@lVariationInsure,
			WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount, DrawCount=DrawCount+@lVariationDrawCount,
			FleeCount=FleeCount+@lVariationFleeCount,PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount
		WHERE UserID=@dwUserID
	
		-- 房间参数
		UPDATE GameScoreAttribute SET  
		IntegralCount=IntegralCount+@lVariationIntegralCount,
		WinCount=WinCount+@lVariationWinCount, 
		LostCount=LostCount+@lVariationLostCount, 
		DrawCount=DrawCount+@lVariationDrawCount,
		FleeCount=FleeCount+@lVariationFleeCount 
		WHERE UserID=@dwUserID And ServerID=@wServerID And KindID = @wKindID 
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO GameScoreAttribute (UserID,KindID,ServerID,IntegralCount,WinCount,LostCount,DrawCount,FleeCount) 
			VALUES (@dwUserID,@wKindID,@wServerID,@lVariationIntegralCount,@lVariationWinCount,@lVariationLostCount,@lVariationDrawCount,@lVariationFleeCount)
		END	
	
		-- 变更记录
		DECLARE @DateID INT
		SELECT @DateID=CAST(CAST(GetDate() AS FLOAT) AS INT) 
	
		-- 存在判断
		IF NOT EXISTS(SELECT * FROM StreamScoreInfo WHERE DateID=@DateID AND UserID=@dwUserID) 
		BEGIN
			-- 插入记录
			INSERT INTO StreamScoreInfo(DateID, UserID, WinCount, LostCount, Revenue, ChipSerialScore, PlayTimeCount, OnlineTimeCount, FirstCollectDate, LastCollectDate)
			VALUES(@DateID, @dwUserID, @lVariationWinCount, @lVariationLostCount, @lVariationRevenue, @lVariationChipSerialScore,@dwVariationPlayTimeCount, 0, GetDate(), GetDate())		
		END ELSE
		BEGIN
			-- 更新记录
			UPDATE StreamScoreInfo SET WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount, Revenue=Revenue+@lVariationRevenue,ChipSerialScore=ChipSerialScore+@lVariationChipSerialScore,
				   PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount, LastCollectDate=GetDate()
			WHERE DateID=@DateID AND UserID=@dwUserID		
		END	


		IF @lVariationUserMedal > 0
		BEGIN		
			-- 查询奖券
			DECLARE @CurrMedal BIGINT	
			SELECT @CurrMedal=AwardTicket FROM UserCurrency WHERE UserID=@dwUserID

			IF @CurrMedal IS NULL SET @CurrMedal = 0
	
			-- 更新奖券
			UPDATE UserCurrency SET AwardTicket=AwardTicket+@lVariationUserMedal WHERE UserID=@dwUserID


		

			INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
			VALUES (dbo.WF_GetSerialNumber(),0,@dwUserID,4,ISNULL(@CurrMedal, 0),@lVariationUserMedal,@strClientIP,GETDATE())
		END	
	END

	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------