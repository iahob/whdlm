
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

-- ��Ϸ��¼
CREATE PROC GSP_GR_RecordDrawInfo

	-- ������Ϣ
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D

	-- ������Ϣ
	@wTableID INT,								-- ���Ӻ���
	@wUserCount INT,							-- �û���Ŀ
	@wAndroidCount INT,							-- ������Ŀ

	-- ����������
	@lWasteCount BIGINT,						-- �����Ŀ
	@lRevenueCount BIGINT,						-- ��Ϸ�������

	-- ͳ����Ϣ
	@dwPlayTimeCount INT,						-- ��Ϸʱ��

	-- ʱ����Ϣ
	@SystemTimeStart DATETIME,					-- ��ʼʱ��
	@SystemTimeConclude DATETIME				-- ����ʱ��

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����¼
	INSERT RecordDrawInfo(KindID,ServerID,TableID,UserCount,AndroidCount,Waste,Revenue,StartTime,ConcludeTime)
	VALUES (@wKindID,@wServerID,@wTableID,@wUserCount,@wAndroidCount,@lWasteCount,@lRevenueCount,@SystemTimeStart,@SystemTimeConclude)
	
	-- ��ȡ��¼
	SELECT SCOPE_IDENTITY() AS DrawID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ϸ��¼
CREATE PROC GSP_GR_RecordDrawScore

	-- ������Ϣ
	@dwDrawID INT,								-- ������ʶ
	@dwUserID INT,								-- �û���ʶ
	@wKindID  INT,								-- ��Ϸ��ʶ
	@wChairID INT,								-- ���Ӻ���

	-- �û���Ϣ
	@dwDBQuestID INT,							-- �����ʶ
	@dwInoutIndex INT,							-- ��������

	-- �ɼ���Ϣ
	@lScore BIGINT,								-- �û�����
	@lGrade BIGINT,								-- �û��ɼ�
	@lRevenue BIGINT,							-- �û��������	
	@lChipSerialScore BIGINT,					-- �û���ˮ	
	@dwPlayTimeCount INT,						-- ��Ϸʱ��
	@wServerType INT							-- ��Ϸ����			
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����¼
	INSERT RecordDrawScore(DrawID,UserID,ChairID,Score,Grade,Revenue,ChipSerialScore, PlayTimeCount,DBQuestID,InoutIndex)
	VALUES (@dwDrawID,@dwUserID,@wChairID,@lScore,@lGrade,@lRevenue,@lChipSerialScore, @dwPlayTimeCount,@dwDBQuestID,@dwInoutIndex)

		--��Լս����
	IF @wServerType <> 16
	BEGIN
		-- ͳ�Ƽ�¼
		DECLARE @RecordCount INT
		SELECT @RecordCount = COUNT(*) FROM RecordDrawScoreForWeb  WHERE  UserID = @dwUserID AND KindID = @wKindID

		select @RecordCount as RecordCount

		-- ������ڻ����30����¼����ɾ����������ļ�¼
		DECLARE @DeleteDrawID INT

		IF @RecordCount >= 30
		BEGIN
		  SELECT @DeleteDrawID= DrawID FROM RecordDrawScoreForWeb WHERE InsertTime=(SELECT MIN(InsertTime) FROM RecordDrawScoreForWeb WHERE UserID = @dwUserID AND KindID = @wKindID) 
		  DELETE FROM RecordDrawScoreForWeb WHERE DrawID = @DeleteDrawID AND UserID = @dwUserID

	  		-- �����¼
		  INSERT RecordDrawScoreForWeb(DrawID,UserID,KindID, ChairID,Score,Grade,Revenue,PlayTimeCount,DBQuestID,InoutIndex, InsertTime)
		  VALUES (@dwDrawID,@dwUserID, @wKindID, @wChairID,@lScore,@lGrade,@lRevenue,@dwPlayTimeCount,@dwDBQuestID,@dwInoutIndex, GETDATE())
		END
		ELSE
		BEGIN
		  	-- �����¼
		  INSERT RecordDrawScoreForWeb(DrawID,UserID,KindID, ChairID,Score,Grade,Revenue,PlayTimeCount,DBQuestID,InoutIndex, InsertTime)
		  VALUES (@dwDrawID,@dwUserID, @wKindID, @wChairID,@lScore,@lGrade,@lRevenue,@dwPlayTimeCount,@dwDBQuestID,@dwInoutIndex, GETDATE())		
		END
	END
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ϸ��¼
CREATE PROC GSP_GR_RecordGroupDrawInfo

	-- ������Ϣ
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D

	-- ������Ϣ
	@dwGroupID INT,								-- ���ֲ�ID
	@dwRoomID INT,								-- ԼսID
	@dwTurnIndex INT,							-- ��ǰ����
	-- ʱ����Ϣ
	@SystemTimeStart DATETIME,					-- ��ʼʱ��
	@SystemTimeConclude DATETIME				-- ����ʱ��

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����¼
	INSERT WHQJGroupDB.dbo.RecordGroupDrawInfo(KindID,ServerID,GroupID,RoomID,TurnIndex,StartTime,ConcludeTime)
	VALUES (@wKindID,@wServerID,@dwGroupID,@dwRoomID,@dwTurnIndex,@SystemTimeStart,@SystemTimeConclude)
	
	-- ��ȡ��¼
	SELECT SCOPE_IDENTITY() AS DrawID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- �Ծּ�¼
CREATE  PROCEDURE dbo.GSP_GR_RecordGroupDrawScore
	@dwDrawID		INT,			--��¼ID
	@dwGroupID		INT,			--���ֲ�ID
	@dwRoomID		INT,			--�����
	@dwUserID		INT,			--��ұ�ʶ	
	@wChairID		INT,			--���Ӻ�
	@lScore			BIGINT,			--����
	@lRevenue		BIGINT			--�������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	declare @curTime datetime
	Set @curTime=GETDATE()
	declare @szUserName nvarchar(31)
	declare @GameID int 
	select @GameID=GameID, @szUserName=NickName from  WHQJAccountsDB.dbo.AccountsInfo where UserID=@dwUserID

	-- �����¼
	INSERT   WHQJGroupDB.dbo.RecordGroupDrawScore(DrawID,RoomID,UserID,UserName,ChairID,Score,Revenue,InsertTime)
	VALUES (@dwDrawID,@dwRoomID,@dwUserID,@szUserName,@wChairID,@lScore,@lRevenue,@curTime)
	/*
	-- �����ռ�¼
	declare @dateID int
	Set @dateID=CONVERT(int,CONVERT(nvarchar(8),@curTime,112))--��ʽ20190715
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


-- �Ծּ�¼
CREATE  PROCEDURE dbo.GSP_GR_RecordGroupRoomInfo
	@dwGroupID		INT,			--���ֲ�ID
	@dwRoomID		INT,			--�����
	@cbPlayMode		TINYINT,		--��������
	@wKindID		INT,			--��ϷID
	@szServerName	NVARCHAR(32),	--��������
	@lMinEnterScore BIGINT,			--�볡��
	@wDrawCount		INT,			--��Ϸ����	
	@dwOwnerID		INT,			--����ID
	@dwBigWinnerID	INT,			--��Ӯ��ID
	@lRoomRevenue   BIGINT,			--�ܹ���
	@SystemTimeStart DATETIME,		-- ��ʼʱ��
	@SystemTimeConclude DATETIME,	-- ����ʱ��
	@cbRoomOver TINYINT
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
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


-- ���׼�¼
CREATE  PROCEDURE dbo.GSP_GR_RecordGroupRevenue
	@dwGroupID		INT,			--���ֲ�ID
	@dwRoomID		INT,			--�����
	@dwUserID		INT,			--���ID
	@cbPlayMode		TINYINT,		--��Ϸģʽ
	@lScore			BIGINT,			--��Ӯ��
	@lRevenue		BIGINT,			--����ֵ
	@cbBigWinner	TINYINT,		--�Ƿ��Ӯ��
	@TimeEnd		DATETIME,		--����ʱ��
	@cbRecord		TINYINT			--�Ƿ��¼���������
 AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	IF @lRevenue<0
		SET @lRevenue=0

	DECLARE @dateID INT
	SET @dateID=CONVERT(INT,CONVERT(nvarchar(8),@TimeEnd,112))--��ʽ20190715

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

	-- ��������ռ�¼
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

	--С����Ϣͳ��
	DECLARE @CreatePoint INT
	SELECT @CreatePoint=AwardPoint FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND MemberRight=1

	DECLARE @dwCaptainID INT
	DECLARE @dwMemberRight INT
	DECLARE @dwTeamerID INT
	SET @dwTeamerID=@dwUserID
	SELECT @dwCaptainID=CaptainID,@dwMemberRight=MemberRight FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwTeamerID
	IF @dwCaptainID=0--�����Լ�ֱ���ӳ���ҵĹ���
	BEGIN

		DECLARE @TempRevenue BIGINT
		SET @TempRevenue=@lRevenue*(@CreatePoint)/100
		--�����õ��ķ���
		UPDATE   WHQJGroupDB.dbo.IMGroupMember SET AwardScore=AwardScore+@TempRevenue WHERE GroupID=@dwGroupID AND MemberRight=1
		--�¼����׵ķ���
		UPDATE   WHQJGroupDB.dbo.RecordUserDayInfo SET ContributionValue=ContributionValue+@TempRevenue where DateID=@dateID AND UserID=@dwTeamerID AND GroupID=@dwGroupID

		IF @dwMemberRight=4--�ӳ�
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

		--������Ƕӳ�������С�Ӽ�¼
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
			--�ۻ�������Ӯ����Ϣ
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

			--���㽱��
			IF @cbPlayMode=1 AND @lRevenue>0
			BEGIN
				SELECT @CurPoint=AwardPoint FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID 
				SET @AwardScore=@lRevenue*(@CurPoint-@DownPoint)/100
				IF @AwardScore>0
				BEGIN
					--�ϼ��õ��ķ���
					UPDATE   WHQJGroupDB.dbo.IMGroupMember SET AwardScore=AwardScore+@AwardScore WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID
					--�¼����׵ķ���
					UPDATE   WHQJGroupDB.dbo.RecordUserDayInfo SET ContributionValue=ContributionValue+@AwardScore WHERE GroupID=@dwGroupID AND UserID=@dwTeamerID AND DateID=@dateID

					--�¼����鹱�׵ķ���
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
				--�ϼ��õ��ķ���
				UPDATE   WHQJGroupDB.dbo.IMGroupMember SET AwardScore=AwardScore+@TempRevenue WHERE GroupID=@dwGroupID AND MemberRight=1
				--�¼����׵ķ���
				--UPDATE   WHQJGroupDB.dbo.RecordUserDayInfo SET ContributionValue=ContributionValue+@TempRevenue WHERE GroupID=@dwGroupID AND UserID=@dwTeamerID AND DateID=@dateID
				--�¼����鹱�׵ķ���
				UPDATE   WHQJGroupDB.dbo.RecordTeamDayInfo SET ContributionValue=ContributionValue+@TempRevenue where DateID=@dateID AND CaptainID=@dwTeamerID AND GroupID=@dwGroupID

			END
		END
	END


END

RETURN 0

GO