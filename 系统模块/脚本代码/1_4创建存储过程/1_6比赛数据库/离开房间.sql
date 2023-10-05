
----------------------------------------------------------------------------------------------------

USE WHQJGameMatchDB
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
	@lRecordRevenue BIGINT,						-- ��Ϸ�������
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
	@lVariationRevenue BIGINT,					-- ��Ϸ�������
	@lVariationChipSerialScore BIGINT,			-- �û���ˮ
	@lVariationWinCount INT,					-- ʤ������
	@lVariationLostCount INT,					-- ʧ������
	@lVariationDrawCount INT,					-- �;�����
	@lVariationFleeCount INT,					-- ������Ŀ
	@lVariationIntegralCount BIGINT,				-- ��Ϸ����		
	@lVariationUserMedal INT,					-- �û�����
	@lVariationExperience INT,					-- �û�����
	@lVariationLoveLiness INT,					-- �û�����
	@dwVariationPlayTimeCount INT,				-- ��Ϸʱ��

	-- ������Ϣ
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineSerial NVARCHAR(32),				-- ������ʶ

	@cbPersonalServer SMALLINT					-- ˽�˷���	

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �û�����
	UPDATE GameScoreInfo SET PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount,	OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount
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

	-- �������
	DELETE  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID

	-- �뿪��¼
	UPDATE RecordUserInout SET LeaveTime=GetDate(), LeaveReason=@dwLeaveReason, LeaveMachine=@strMachineSerial, LeaveClientIP=@strClientIP,
		Score=@lRecordScore, Insure=@lRecordInsure, Revenue=@lRecordRevenue, WinCount=@lRecordWinCount, LostCount=@lRecordLostCount,
		DrawCount=@lRecordDrawCount, FleeCount=@lRecordFleeCount, UserMedal=@lRecordUserMedal, Experience=@lRecordExperience, LoveLiness=@lRecordLoveLiness,
		PlayTimeCount=@dwRecordPlayTimeCount, OnLineTimeCount=@dwOnLineTimeCount
	WHERE ID=@dwInoutIndex AND UserID=@dwUserID

	-- ȫ����Ϣ
--	IF @lVariationExperience>0 OR @lVariationLoveLiness>0 OR @lVariationUserMedal>0
--	BEGIN
--		UPDATE  WHQJAccountsDB.dbo.AccountsInfo SET Experience=Experience+@lVariationExperience, LoveLiness=LoveLiness+@lVariationLoveLiness,UserMedal=UserMedal+@lVariationUserMedal
--		WHERE UserID=@dwUserID
--	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------