
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

-- �뿪����
CREATE PROC GSP_GR_LeaveGameServer

	-- �û���Ϣ
	@dwUserID INT,								-- �û� I D
	@dwOnLineTimeCount INT,						-- ����ʱ��
	@dwDayPlayTime INT,							-- ��Ϸʱ��

	-- ϵͳ��Ϣ
	@dwInoutIndex INT,							-- ��������
	@dwLeaveReason INT,							-- �뿪ԭ��

	-- ��¼�ɼ�
	@lRecordScore BIGINT,						-- �û�����
	@lRecordGrade BIGINT,						-- �û��ɼ�
	@lRecordInsure BIGINT,						-- �û�����
	@lRecordRevenue BIGINT,						-- ��Ϸ˰��
	@lRecordChipSerialScore BIGINT,				-- �û���ˮ
	@lRecordWinCount INT,						-- ʤ������
	@lRecordLostCount INT,						-- ʧ������
	@lRecordDrawCount INT,						-- �;�����
	@lRecordFleeCount INT,						-- ������Ŀ
	@lRecordUserMedal INT,						-- �û�����
	@lRecordExperience INT,						-- �û�����
	@lRecordLoveLiness INT,						-- �û�����
	@dwRecordPlayTimeCount INT,					-- ��Ϸʱ��

	-- ����ɼ�
	@lVariationScore BIGINT,					-- �û�����
	@lVariationGrade BIGINT,					-- �û��ɼ�
	@lVariationInsure BIGINT,					-- �û�����
	@lVariationRevenue BIGINT,					-- ��Ϸ˰��
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

	-- ������Ϣ
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineSerial NVARCHAR(32),				-- ������ʶ
	@cbIsPersonalRoom TINYINT					-- �Ƿ�Ϊ˽�˷���	

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
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
	
	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- �û�����
	UPDATE GameScoreInfo SET Score=Score+@lVariationScore, InsureScore=InsureScore+@lVariationInsure, Revenue=Revenue+@lVariationRevenue,
		WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount, DrawCount=DrawCount+@lVariationDrawCount,
		FleeCount=FleeCount+@lVariationFleeCount, PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount, OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount
	WHERE UserID=@dwUserID

	DECLARE @DateID	INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)
	-- �����ж�
	IF NOT EXISTS(SELECT * FROM StreamScoreInfo WHERE DateID=@DateID AND UserID=@dwUserID) 
	BEGIN
		-- �����¼
		INSERT INTO StreamScoreInfo(DateID, UserID, WinCount, LostCount, Revenue, ChipSerialScore, PlayTimeCount, OnlineTimeCount, FirstCollectDate, LastCollectDate)
		VALUES(@DateID, @dwUserID, @lVariationWinCount, @lVariationLostCount, @lVariationRevenue, @lVariationChipSerialScore, @dwVariationPlayTimeCount, @dwOnLineTimeCount, GetDate(), GetDate())		
	END ELSE
	BEGIN
		-- ���¼�¼
		UPDATE StreamScoreInfo SET WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount, Revenue=Revenue+@lVariationRevenue,ChipSerialScore=ChipSerialScore+@lVariationChipSerialScore,
			   PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount, OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount, LastCollectDate=GetDate()
		WHERE DateID=@DateID AND UserID=@dwUserID		
	END

	-- �������
	IF @dwLeaveReason<>0x03
	BEGIN
		DELETE GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID
	END

	-- �����˲���¼˰��
	IF @strClientIP=N'0.0.0.0'
	BEGIN
		SET @lRecordRevenue=0
	END

	-- �뿪��¼
	UPDATE RecordUserInout SET LeaveTime=GetDate(), LeaveReason=@dwLeaveReason, LeaveMachine=@strMachineSerial,LeaveClientIP=@strClientIP,
		Score=@lRecordScore, Grade=@lRecordGrade, Insure=@lRecordInsure, Revenue=@lRecordRevenue, WinCount=@lRecordWinCount, LostCount=@lRecordLostCount,
		DrawCount=@lRecordDrawCount, FleeCount=@lRecordFleeCount, 
		PlayTimeCount=@dwRecordPlayTimeCount, OnLineTimeCount=@dwOnLineTimeCount
	WHERE ID=@dwInoutIndex AND UserID=@dwUserID

	--��Ϸʱ��
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
	
	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ȫ����Ϣ
	UPDATE  WHQJAccountsDB.dbo.AccountsInfo SET PlayTimeCount=PlayTimeCount+@dwRecordPlayTimeCount,OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount
	WHERE UserID=@dwUserID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------