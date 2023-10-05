
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordDrawInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordDrawInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordDrawScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordDrawScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordGroupDrawInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordGroupDrawInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordGroupDrawScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordGroupDrawScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordGroupRoomInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordGroupRoomInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordGroupRevenue]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordGroupRevenue]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 游戏记录
CREATE PROC GSP_GR_RecordDrawInfo

	-- 房间信息
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D

	-- 桌子信息
	@wTableID INT,								-- 桌子号码
	@wUserCount INT,							-- 用户数目
	@wAndroidCount INT,							-- 机器数目

	-- 服务比例损耗
	@lWasteCount BIGINT,						-- 损耗数目
	@lRevenueCount BIGINT,						-- 游戏服务比例

	-- 统计信息
	@dwPlayTimeCount INT,						-- 游戏时间

	-- 时间信息
	@SystemTimeStart DATETIME,					-- 开始时间
	@SystemTimeConclude DATETIME				-- 结束时间

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 插入记录
	INSERT RecordDrawInfo(KindID,ServerID,TableID,UserCount,AndroidCount,Waste,Revenue,StartTime,ConcludeTime)
	VALUES (@wKindID,@wServerID,@wTableID,@wUserCount,@wAndroidCount,@lWasteCount,@lRevenueCount,@SystemTimeStart,@SystemTimeConclude)
	
	-- 读取记录
	SELECT SCOPE_IDENTITY() AS DrawID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 游戏记录
CREATE PROC GSP_GR_RecordDrawScore

	-- 房间信息
	@dwDrawID INT,								-- 局数标识
	@dwUserID INT,								-- 用户标识
	@wKindID  INT,								-- 游戏标识
	@wChairID INT,								-- 椅子号码

	-- 用户信息
	@dwDBQuestID INT,							-- 请求标识
	@dwInoutIndex INT,							-- 进出索引

	-- 成绩信息
	@lScore BIGINT,								-- 用户积分
	@lGrade BIGINT,								-- 用户成绩
	@lRevenue BIGINT,							-- 用户服务比例	
	@lChipSerialScore BIGINT,					-- 用户流水	
	@dwPlayTimeCount INT,						-- 游戏时间
	@wServerType INT							-- 游戏类型			
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 插入记录
	INSERT RecordDrawScore(DrawID,UserID,ChairID,Score,Grade,Revenue,ChipSerialScore, PlayTimeCount,DBQuestID,InoutIndex)
	VALUES (@dwDrawID,@dwUserID,@wChairID,@lScore,@lGrade,@lRevenue,@lChipSerialScore, @dwPlayTimeCount,@dwDBQuestID,@dwInoutIndex)

		--非约战房间
	IF @wServerType <> 16
	BEGIN
		-- 统计记录
		DECLARE @RecordCount INT
		SELECT @RecordCount = COUNT(*) FROM RecordDrawScoreForWeb  WHERE  UserID = @dwUserID AND KindID = @wKindID

		select @RecordCount as RecordCount

		-- 如果大于或等于30调记录，则删除创建最早的记录
		DECLARE @DeleteDrawID INT

		IF @RecordCount >= 30
		BEGIN
		  SELECT @DeleteDrawID= DrawID FROM RecordDrawScoreForWeb WHERE InsertTime=(SELECT MIN(InsertTime) FROM RecordDrawScoreForWeb WHERE UserID = @dwUserID AND KindID = @wKindID) 
		  DELETE FROM RecordDrawScoreForWeb WHERE DrawID = @DeleteDrawID AND UserID = @dwUserID

	  		-- 插入记录
		  INSERT RecordDrawScoreForWeb(DrawID,UserID,KindID, ChairID,Score,Grade,Revenue,PlayTimeCount,DBQuestID,InoutIndex, InsertTime)
		  VALUES (@dwDrawID,@dwUserID, @wKindID, @wChairID,@lScore,@lGrade,@lRevenue,@dwPlayTimeCount,@dwDBQuestID,@dwInoutIndex, GETDATE())
		END
		ELSE
		BEGIN
		  	-- 插入记录
		  INSERT RecordDrawScoreForWeb(DrawID,UserID,KindID, ChairID,Score,Grade,Revenue,PlayTimeCount,DBQuestID,InoutIndex, InsertTime)
		  VALUES (@dwDrawID,@dwUserID, @wKindID, @wChairID,@lScore,@lGrade,@lRevenue,@dwPlayTimeCount,@dwDBQuestID,@dwInoutIndex, GETDATE())		
		END
	END
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 游戏记录
CREATE PROC GSP_GR_RecordGroupDrawInfo

	-- 房间信息
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D

	-- 桌子信息
	@dwGroupID INT,								-- 俱乐部ID
	@dwRoomID INT,								-- 约战ID
	@dwTurnIndex INT,							-- 当前局数
	-- 时间信息
	@SystemTimeStart DATETIME,					-- 开始时间
	@SystemTimeConclude DATETIME				-- 结束时间

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 插入记录
	INSERT WHQJGroupDB.dbo.RecordGroupDrawInfo(KindID,ServerID,GroupID,RoomID,TurnIndex,StartTime,ConcludeTime)
	VALUES (@wKindID,@wServerID,@dwGroupID,@dwRoomID,@dwTurnIndex,@SystemTimeStart,@SystemTimeConclude)
	
	-- 读取记录
	SELECT SCOPE_IDENTITY() AS DrawID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 对局记录
CREATE  PROCEDURE dbo.GSP_GR_RecordGroupDrawScore
	@dwDrawID		INT,			--记录ID
	@dwGroupID		INT,			--俱乐部ID
	@dwRoomID		INT,			--房间号
	@dwUserID		INT,			--玩家标识	
	@wChairID		INT,			--椅子号
	@lScore			BIGINT,			--分数
	@lRevenue		BIGINT			--服务比例
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	declare @curTime datetime
	Set @curTime=GETDATE()
	declare @szUserName nvarchar(31)
	declare @GameID int 
	select @GameID=GameID, @szUserName=NickName from  WHQJAccountsDB.dbo.AccountsInfo where UserID=@dwUserID

	-- 插入记录
	INSERT   WHQJGroupDB.dbo.RecordGroupDrawScore(DrawID,RoomID,UserID,UserName,ChairID,Score,Revenue,InsertTime)
	VALUES (@dwDrawID,@dwRoomID,@dwUserID,@szUserName,@wChairID,@lScore,@lRevenue,@curTime)
	/*
	-- 更新日记录
	declare @dateID int
	Set @dateID=CONVERT(int,CONVERT(nvarchar(8),@curTime,112))--格式20190715
	declare @curScore bigint
	Select @curScore=Score from   WHQJGroupDB.dbo.IMGroupMember where UserID=@dwUserID

	IF Exists(select 1 from   WHQJGroupDB.dbo.RecordUserDayInfo where DateID=@dateID AND UserID=@dwUserID AND GroupID=@dwGroupID)
	BEGIN
		--update   WHQJGroupDB.dbo.RecordUserDayInfo set CurScore=@curScore, DrawCount=DrawCount+1,BigWinnerCount=BigWinnerCount+@cbBigWinner,ScoreCount=ScoreCount+@lScore,RevenueCount=RevenueCount+@lRevenue where DateID=@dateID AND UserID=@dwUserID AND GroupID=@dwGroupID
		update   WHQJGroupDB.dbo.RecordUserDayInfo set CurScore=@curScore, ScoreCount=ScoreCount+@lScore,RevenueCount=RevenueCount+@lRevenue where DateID=@dateID AND UserID=@dwUserID AND GroupID=@dwGroupID
	END
	ELSE
	BEGIN
		insert   WHQJGroupDB.dbo.RecordUserDayInfo (DateID,GroupID,UserID,UserName,CurScore,CreateCount,DrawCount,BigWinnerCount,ScoreCount,RevenueCount,Flag,InsertDate)
		 values(@dateID,@dwGroupID,@dwUserID,@szUserName,@curScore,0,0,0,@lScore,@lRevenue,0,@curTime)
	END
	*/
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 对局记录
CREATE  PROCEDURE dbo.GSP_GR_RecordGroupRoomInfo
	@dwGroupID		INT,			--俱乐部ID
	@dwRoomID		INT,			--房间号
	@cbPlayMode		TINYINT,		--房间类型
	@wKindID		INT,			--游戏ID
	@szServerName	NVARCHAR(32),	--房间名称
	@lMinEnterScore BIGINT,			--入场分
	@wDrawCount		INT,			--游戏局数	
	@dwOwnerID		INT,			--桌主ID
	@dwBigWinnerID	INT,			--大赢家ID
	@lRoomRevenue   BIGINT,			--总贡献
	@SystemTimeStart DATETIME,		-- 开始时间
	@SystemTimeConclude DATETIME,	-- 结束时间
	@cbRoomOver TINYINT
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	declare @szWinnerName nvarchar(31)
	declare @dwWinnerGameID BIGINT
	select @szWinnerName=NickName,@dwWinnerGameID=GameID from WHQJAccountsDB.dbo.AccountsInfo where UserID=@dwBigWinnerID
	IF EXISTS(SELECT * FROM WHQJGroupDB..RecordGroupRoomInfo WHERE GroupID=@dwGroupID AND RoomID=@dwRoomID AND RoomOver=0)
	BEGIN
		UPDATE WHQJGroupDB..RecordGroupRoomInfo SET DrawCount=@wDrawCount,EndTime=@SystemTimeConclude,RoomRevenue=RoomRevenue+@lRoomRevenue,RoomOver=@cbRoomOver
			WHERE GroupID=@dwGroupID AND RoomID=@dwRoomID AND RoomOver=0
	END
	ELSE
	BEGIN
		INSERT INTO WHQJGroupDB..RecordGroupRoomInfo(GroupID,RoomID,PlayMode,KindID,KindName,DrawCount,BigWinnerID,BigWinnerName,MinTableScore,RoomRevenue,StartTime,EndTime,RoomOver)
			VALUES(@dwGroupID,@dwRoomID,@cbPlayMode,@wKindID,@szServerName,@wDrawCount,ISNULL(@dwWinnerGameID, 0),ISNULL(@szWinnerName, ''),@lMinEnterScore,@lRoomRevenue,@SystemTimeStart,@SystemTimeConclude,@cbRoomOver)
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 贡献记录
CREATE  PROCEDURE dbo.GSP_GR_RecordGroupRevenue
	@dwGroupID		INT,			--俱乐部ID
	@dwRoomID		INT,			--房间号
	@dwUserID		INT,			--玩家ID
	@cbPlayMode		TINYINT,		--游戏模式
	@lScore			BIGINT,			--输赢分
	@lRevenue		BIGINT,			--贡献值
	@cbBigWinner	TINYINT,		--是否大赢家
	@TimeEnd		DATETIME,		--结束时间
	@cbRecord		TINYINT			--是否记录过参与次数
 AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	IF @lRevenue<0
		SET @lRevenue=0

	DECLARE @dateID INT
	SET @dateID=CONVERT(INT,CONVERT(nvarchar(8),@TimeEnd,112))--格式20190715

	DECLARE @CurUserScore BIGINT
	select @CurUserScore=Score from   WHQJGroupDB.dbo.IMGroupMember where UserID=@dwUserID AND GroupID=@dwGroupID
	IF @CurUserScore IS NULL
		RETURN 1
	DECLARE @CurGroupScore BIGINT
	select @CurGroupScore=Score from   WHQJGroupDB.dbo.IMGroupWealth where GroupID=@dwGroupID
	IF @CurGroupScore IS NULL
		RETURN 1

	DECLARE @UserDrawCount INT
	SET @UserDrawCount=1
	IF @cbRecord=1
		SET @UserDrawCount=0

	-- 更新玩家日记录
	IF Exists(SELECT 1 FROM   WHQJGroupDB.dbo.RecordUserDayInfo WHERE DateID=@dateID AND UserID=@dwUserID AND GroupID=@dwGroupID)
	BEGIN
		UPDATE   WHQJGroupDB.dbo.RecordUserDayInfo 
		SET DrawCount=DrawCount+@UserDrawCount,BigWinnerCount=BigWinnerCount+@cbBigWinner, ScoreCount=ScoreCount+@lScore+@lRevenue, RevenueCount=RevenueCount+@lRevenue ,CurScore=@CurUserScore-@lRevenue 
		where DateID=@dateID AND UserID=@dwUserID AND GroupID=@dwGroupID
	END
	ELSE
	BEGIN
		DECLARE @szUserName nvarchar(31)
		SELECT @szUserName=NickName FROM WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

		INSERT   WHQJGroupDB.dbo.RecordUserDayInfo (DateID,GroupID,UserID,UserName,CurScore,CreateCount,DrawCount,BigWinnerCount,ScoreCount,RevenueCount,ContributionValue,Flag,InsertTime)
		VALUES(@dateID,@dwGroupID,@dwUserID,@szUserName,@CurUserScore-@lRevenue,0,1,@cbBigWinner,@lScore+@lRevenue,@lRevenue,0,0,@TimeEnd)
	END

	UPDATE   WHQJGroupDB.dbo.IMGroupMember Set BattleCount=BattleCount+@UserDrawCount WHERE UserID=@dwUserID and GroupID=@dwGroupID

	IF @cbPlayMode=1 AND @lRevenue>0
	BEGIN
		UPDATE   WHQJGroupDB.dbo.IMGroupMember Set Score=Score-@lRevenue WHERE UserID=@dwUserID and GroupID=@dwGroupID
		INSERT INTO   WHQJGroupDB.dbo.RecordUserScoreChange(GroupID,UserID,BeforeScore,ChangeType,ChangeScore,AfterScore) 
		VALUES(@dwGroupID,@dwUserID,@CurUserScore,3,-@lRevenue,@CurUserScore-@lRevenue)
	END

	--小队信息统计
	DECLARE @CreatePoint INT
	SELECT @CreatePoint=AwardPoint FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND MemberRight=1

	DECLARE @dwCaptainID INT
	DECLARE @dwMemberRight INT
	DECLARE @dwTeamerID INT
	SET @dwTeamerID=@dwUserID
	SELECT @dwCaptainID=CaptainID,@dwMemberRight=MemberRight FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwTeamerID
	IF @dwCaptainID=0--盟主以及直属队长玩家的贡献
	BEGIN

		DECLARE @TempRevenue BIGINT
		SET @TempRevenue=@lRevenue*(@CreatePoint)/100
		--盟主得到的返利
		UPDATE   WHQJGroupDB.dbo.IMGroupMember SET AwardScore=AwardScore+@TempRevenue WHERE GroupID=@dwGroupID AND MemberRight=1
		--下级贡献的返利
		UPDATE   WHQJGroupDB.dbo.RecordUserDayInfo SET ContributionValue=ContributionValue+@TempRevenue where DateID=@dateID AND UserID=@dwTeamerID AND GroupID=@dwGroupID

		IF @dwMemberRight=4--队长
		BEGIN
			IF Exists(select 1 from   WHQJGroupDB.dbo.RecordTeamDayInfo where DateID=@dateID AND CaptainID=@dwTeamerID AND GroupID=@dwGroupID)
			BEGIN
				update   WHQJGroupDB.dbo.RecordTeamDayInfo 
				set DrawCount=DrawCount+@UserDrawCount,BigWinnerCount=BigWinnerCount+@cbBigWinner,ScoreCount=ScoreCount+@lScore+@lRevenue,RevenueCount=RevenueCount+@lRevenue,ContributionValue=ContributionValue+@TempRevenue
				where DateID=@dateID AND CaptainID=@dwTeamerID AND GroupID=@dwGroupID
			END
			ELSE
			BEGIN
				insert   WHQJGroupDB.dbo.RecordTeamDayInfo (DateID,GroupID,CaptainID,DrawCount,BigWinnerCount,ScoreCount,RevenueCount,ContributionValue,Flag)
				values(@dateID,@dwGroupID,@dwTeamerID,1,@cbBigWinner,@lScore+@lRevenue,@lRevenue,@TempRevenue,0)
			END

		END

	END
	ELSE
	BEGIN

		--该玩家是队长，更新小队记录
		IF 4=(SELECT MemberRight FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID)
		BEGIN
			IF Exists(select 1 from   WHQJGroupDB.dbo.RecordTeamDayInfo where DateID=@dateID AND CaptainID=@dwUserID AND GroupID=@dwGroupID)
			BEGIN
				update   WHQJGroupDB.dbo.RecordTeamDayInfo 
				set DrawCount=DrawCount+@UserDrawCount,BigWinnerCount=BigWinnerCount+@cbBigWinner,ScoreCount=ScoreCount+@lScore+@lRevenue,RevenueCount=RevenueCount+@lRevenue
				where DateID=@dateID AND CaptainID=@dwUserID AND GroupID=@dwGroupID
			END
			ELSE
			BEGIN
				insert   WHQJGroupDB.dbo.RecordTeamDayInfo (DateID,GroupID,CaptainID,DrawCount,BigWinnerCount,ScoreCount,RevenueCount,ContributionValue,Flag)
				values(@dateID,@dwGroupID,@dwUserID,1,@cbBigWinner,@lScore+@lRevenue,@lRevenue,0,0)
			END
		END

		DECLARE @DownPoint INT
		SET @DownPoint=0
		DECLARE @CurPoint INT
		SET @CurPoint=0
		DECLARE @AwardScore BIGINT
		SET @AwardScore=0

		WHILE @dwCaptainID>0
		BEGIN
			--累积局数大赢家信息
			IF Exists(select 1 from   WHQJGroupDB.dbo.RecordTeamDayInfo where DateID=@dateID AND CaptainID=@dwCaptainID AND GroupID=@dwGroupID)
			BEGIN
				update   WHQJGroupDB.dbo.RecordTeamDayInfo 
				set DrawCount=DrawCount+@UserDrawCount,BigWinnerCount=BigWinnerCount+@cbBigWinner,ScoreCount=ScoreCount+@lScore+@lRevenue,RevenueCount=RevenueCount+@lRevenue
				where DateID=@dateID AND CaptainID=@dwCaptainID AND GroupID=@dwGroupID
			END
			ELSE
			BEGIN
				insert   WHQJGroupDB.dbo.RecordTeamDayInfo (DateID,GroupID,CaptainID,DrawCount,BigWinnerCount,ScoreCount,RevenueCount,ContributionValue ,Flag)
				values(@dateID,@dwGroupID,@dwCaptainID,1,@cbBigWinner,@lScore+@lRevenue,@lRevenue,0,0)
			END

			--计算奖励
			IF @cbPlayMode=1 AND @lRevenue>0
			BEGIN
				SELECT @CurPoint=AwardPoint FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID 
				SET @AwardScore=@lRevenue*(@CurPoint-@DownPoint)/100
				IF @AwardScore>0
				BEGIN
					--上级得到的返利
					UPDATE   WHQJGroupDB.dbo.IMGroupMember SET AwardScore=AwardScore+@AwardScore WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID
					--下级贡献的返利
					UPDATE   WHQJGroupDB.dbo.RecordUserDayInfo SET ContributionValue=ContributionValue+@AwardScore WHERE GroupID=@dwGroupID AND UserID=@dwTeamerID AND DateID=@dateID

					--下级队伍贡献的返利
					IF 4=(SELECT MemberRight FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwTeamerID)
					BEGIN
						UPDATE   WHQJGroupDB.dbo.RecordTeamDayInfo SET ContributionValue=ContributionValue+@AwardScore where DateID=@dateID AND CaptainID=@dwTeamerID AND GroupID=@dwGroupID
					END
				END

			END

			SET @DownPoint=@CurPoint
			SET @dwTeamerID=@dwCaptainID
			SELECT @dwCaptainID=CaptainID,@dwMemberRight=MemberRight FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID

		END

		IF @dwCaptainID=0
		BEGIN

			SET @TempRevenue=@lRevenue*(@CreatePoint-@DownPoint)/100
			IF @TempRevenue>0
			BEGIN
				--上级得到的返利
				UPDATE   WHQJGroupDB.dbo.IMGroupMember SET AwardScore=AwardScore+@TempRevenue WHERE GroupID=@dwGroupID AND MemberRight=1
				--下级贡献的返利
				--UPDATE   WHQJGroupDB.dbo.RecordUserDayInfo SET ContributionValue=ContributionValue+@TempRevenue WHERE GroupID=@dwGroupID AND UserID=@dwTeamerID AND DateID=@dateID
				--下级队伍贡献的返利
				UPDATE   WHQJGroupDB.dbo.RecordTeamDayInfo SET ContributionValue=ContributionValue+@TempRevenue where DateID=@dateID AND CaptainID=@dwTeamerID AND GroupID=@dwGroupID

			END
		END
	END


END

RETURN 0

GO