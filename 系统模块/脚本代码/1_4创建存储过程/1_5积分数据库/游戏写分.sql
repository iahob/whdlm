
----------------------------------------------------------------------------------------------------

USE WHQJGameScoreDB
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

	-- ϵͳ��Ϣ
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
	@lVariationIntegralCount BIGINT,				-- ��Ϸ����	
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

	-- �û�����
	UPDATE GameScoreInfo SET Score=Score+@lVariationScore, WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount,
		DrawCount=DrawCount+@lVariationDrawCount, FleeCount=FleeCount+@lVariationFleeCount, PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount
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
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------