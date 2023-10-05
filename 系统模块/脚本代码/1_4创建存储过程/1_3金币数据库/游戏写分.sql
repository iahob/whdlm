
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

-- ��Ϸд��
CREATE PROC GSP_GR_WriteGameScore

	-- �û���Ϣ
	@dwUserID INT,								-- �û� I D
	@dwDBQuestID INT,							-- �����ʶ
	@dwInoutIndex INT,							-- ��������

	-- ����ɼ�
	@lVariationScore BIGINT,					-- �û�����
	@lVariationGrade BIGINT,					-- �û��ɼ�
	@lVariationInsure BIGINT,					-- �û�����
	@lVariationRevenue BIGINT,					-- ��Ϸ�������
	@lVariationChipSerialScore BIGINT,			-- �û���ˮ
	@lVariationWinCount INT,					-- ʤ������
	@lVariationLostCount INT,					-- ʧ������
	@lVariationDrawCount INT,					-- �;�����
	@lVariationFleeCount INT,					-- ������Ŀ
	@lVariationIntegralCount BIGINT,			-- ��Ϸ����	
	@lVariationUserMedal INT,					-- �û�����
	@lVariationExperience INT,					-- �û�����
	@lVariationLoveLiness INT,					-- �û�����
	@dwVariationPlayTimeCount INT,				-- ��Ϸʱ��
	@dwVariationGroupID INT,					-- ������ID
	-- ������Ϣ	
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15)					-- ���ӵ�ַ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	IF @dwVariationGroupID <> 0
	BEGIN
		--���ֲ�����
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
		-- �û�����
		UPDATE GameScoreInfo SET Score=Score+@lVariationScore, Revenue=Revenue+@lVariationRevenue, InsureScore=InsureScore+@lVariationInsure,
			WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount, DrawCount=DrawCount+@lVariationDrawCount,
			FleeCount=FleeCount+@lVariationFleeCount,PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount
		WHERE UserID=@dwUserID
	
		-- �������
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
	
		-- �����¼
		DECLARE @DateID INT
		SELECT @DateID=CAST(CAST(GetDate() AS FLOAT) AS INT) 
	
		-- �����ж�
		IF NOT EXISTS(SELECT * FROM StreamScoreInfo WHERE DateID=@DateID AND UserID=@dwUserID) 
		BEGIN
			-- �����¼
			INSERT INTO StreamScoreInfo(DateID, UserID, WinCount, LostCount, Revenue, ChipSerialScore, PlayTimeCount, OnlineTimeCount, FirstCollectDate, LastCollectDate)
			VALUES(@DateID, @dwUserID, @lVariationWinCount, @lVariationLostCount, @lVariationRevenue, @lVariationChipSerialScore,@dwVariationPlayTimeCount, 0, GetDate(), GetDate())		
		END ELSE
		BEGIN
			-- ���¼�¼
			UPDATE StreamScoreInfo SET WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount, Revenue=Revenue+@lVariationRevenue,ChipSerialScore=ChipSerialScore+@lVariationChipSerialScore,
				   PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount, LastCollectDate=GetDate()
			WHERE DateID=@DateID AND UserID=@dwUserID		
		END	


		IF @lVariationUserMedal > 0
		BEGIN		
			-- ��ѯ��ȯ
			DECLARE @CurrMedal BIGINT	
			SELECT @CurrMedal=AwardTicket FROM UserCurrency WHERE UserID=@dwUserID

			IF @CurrMedal IS NULL SET @CurrMedal = 0
	
			-- ���½�ȯ
			UPDATE UserCurrency SET AwardTicket=AwardTicket+@lVariationUserMedal WHERE UserID=@dwUserID


		

			INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
			VALUES (dbo.WF_GetSerialNumber(),0,@dwUserID,4,ISNULL(@CurrMedal, 0),@lVariationUserMedal,@strClientIP,GETDATE())
		END	
	END

	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------