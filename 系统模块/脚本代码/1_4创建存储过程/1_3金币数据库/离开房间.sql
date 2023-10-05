
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LeaveGameServer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LeaveGameServer]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 离开房间
CREATE PROC GSP_GR_LeaveGameServer

	-- 用户信息
	@dwUserID INT,								-- 用户 I D
	@dwOnLineTimeCount INT,						-- 在线时间
	@dwDayPlayTime INT,							-- 游戏时间

	-- 系统信息
	@dwInoutIndex INT,							-- 进出索引
	@dwLeaveReason INT,							-- 离开原因

	-- 记录成绩
	@lRecordScore BIGINT,						-- 用户分数
	@lRecordGrade BIGINT,						-- 用户成绩
	@lRecordInsure BIGINT,						-- 用户银行
	@lRecordRevenue BIGINT,						-- 游戏税收
	@lRecordChipSerialScore BIGINT,				-- 用户流水
	@lRecordWinCount INT,						-- 胜利盘数
	@lRecordLostCount INT,						-- 失败盘数
	@lRecordDrawCount INT,						-- 和局盘数
	@lRecordFleeCount INT,						-- 断线数目
	@lRecordUserMedal INT,						-- 用户奖牌
	@lRecordExperience INT,						-- 用户经验
	@lRecordLoveLiness INT,						-- 用户魅力
	@dwRecordPlayTimeCount INT,					-- 游戏时间

	-- 变更成绩
	@lVariationScore BIGINT,					-- 用户分数
	@lVariationGrade BIGINT,					-- 用户成绩
	@lVariationInsure BIGINT,					-- 用户银行
	@lVariationRevenue BIGINT,					-- 游戏税收
	@lVariationChipSerialScore BIGINT,			-- 用户流水
	@lVariationWinCount INT,					-- 胜利盘数
	@lVariationLostCount INT,					-- 失败盘数
	@lVariationDrawCount INT,					-- 和局盘数
	@lVariationFleeCount INT,					-- 断线数目
	@lVariationIntegralCount BIGINT,			-- 游戏积分		
	@lVariationUserMedal INT,					-- 用户奖牌
	@lVariationExperience INT,					-- 用户经验
	@lVariationLoveLiness INT,					-- 用户魅力
	@dwVariationPlayTimeCount INT,				-- 游戏时间

	-- 属性信息
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineSerial NVARCHAR(32),				-- 机器标识
	@cbIsPersonalRoom TINYINT					-- 是否为私人房间	

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
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
	
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 用户积分
	UPDATE GameScoreInfo SET Score=Score+@lVariationScore, InsureScore=InsureScore+@lVariationInsure, Revenue=Revenue+@lVariationRevenue,
		WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount, DrawCount=DrawCount+@lVariationDrawCount,
		FleeCount=FleeCount+@lVariationFleeCount, PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount, OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount
	WHERE UserID=@dwUserID

	DECLARE @DateID	INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)
	-- 存在判断
	IF NOT EXISTS(SELECT * FROM StreamScoreInfo WHERE DateID=@DateID AND UserID=@dwUserID) 
	BEGIN
		-- 插入记录
		INSERT INTO StreamScoreInfo(DateID, UserID, WinCount, LostCount, Revenue, ChipSerialScore, PlayTimeCount, OnlineTimeCount, FirstCollectDate, LastCollectDate)
		VALUES(@DateID, @dwUserID, @lVariationWinCount, @lVariationLostCount, @lVariationRevenue, @lVariationChipSerialScore, @dwVariationPlayTimeCount, @dwOnLineTimeCount, GetDate(), GetDate())		
	END ELSE
	BEGIN
		-- 更新记录
		UPDATE StreamScoreInfo SET WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount, Revenue=Revenue+@lVariationRevenue,ChipSerialScore=ChipSerialScore+@lVariationChipSerialScore,
			   PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount, OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount, LastCollectDate=GetDate()
		WHERE DateID=@DateID AND UserID=@dwUserID		
	END

	-- 锁定解除
	IF @dwLeaveReason<>0x03
	BEGIN
		DELETE GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID
	END

	-- 机器人不记录税收
	IF @strClientIP=N'0.0.0.0'
	BEGIN
		SET @lRecordRevenue=0
	END

	-- 离开记录
	UPDATE RecordUserInout SET LeaveTime=GetDate(), LeaveReason=@dwLeaveReason, LeaveMachine=@strMachineSerial,LeaveClientIP=@strClientIP,
		Score=@lRecordScore, Grade=@lRecordGrade, Insure=@lRecordInsure, Revenue=@lRecordRevenue, WinCount=@lRecordWinCount, LostCount=@lRecordLostCount,
		DrawCount=@lRecordDrawCount, FleeCount=@lRecordFleeCount, 
		PlayTimeCount=@dwRecordPlayTimeCount, OnLineTimeCount=@dwOnLineTimeCount
	WHERE ID=@dwInoutIndex AND UserID=@dwUserID

	--游戏时间
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		IF NOT EXISTS(SELECT UserID FROM WHQJAccountsDB.dbo.AccountsDayPlayTime WHERE DateID=@DateID AND UserID=@dwUserID)
		BEGIN
			INSERT INTO WHQJAccountsDB.dbo.AccountsDayPlayTime(DateID,UserID,DayPlayTimeCount)
			VALUES(@DateID,@dwUserID,@dwDayPlayTime)
		END
		ELSE
		BEGIN
			UPDATE WHQJAccountsDB.dbo.AccountsDayPlayTime SET DayPlayTimeCount=@dwDayPlayTime,CollectDate=GETDATE()
			WHERE DateID=@DateID AND UserID=@dwUserID	
		END
	END
	
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 全局信息
	UPDATE  WHQJAccountsDB.dbo.AccountsInfo SET PlayTimeCount=PlayTimeCount+@dwRecordPlayTimeCount,OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount
	WHERE UserID=@dwUserID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------