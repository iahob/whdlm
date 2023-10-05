
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidGetParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidGetParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidAddParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidAddParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidModifyParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidModifyParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidDeleteParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidDeleteParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ȡ����
CREATE PROC GSP_GP_AndroidGetParameter
	@wServerID INT								-- �����ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ӳ���
CREATE PROC GSP_GP_AndroidAddParameter
	@wServerID INT,								-- �����ʶ
	@dwServiceMode INT,							-- ����ģʽ
	@dwAndroidCount INT,						-- ������Ŀ
	@dwSwitchMinInnings INT,					-- ��������
	@dwSwitchMaxInnings INT,					-- ��������
	@dwAIHangingMinRate INT,					-- �һ�����
	@dwAIHangingMaxRate INT,					-- �һ�����
	
	@dwAICarryScoreMinMultiple0 INT,			--��С���뱶��
	@dwAICarryScoreMaxMultiple0 INT,			--��С���뱶��
	@dwAIScoreLevelRate0 INT,					--��������
	@dwAIScoreFloatMinRate0 INT,				--������Χ
	@dwAIScoreFloatMaxRate0 INT,				--������Χ
	
	@dwAICarryScoreMinMultiple1 INT,			--��С���뱶��
	@dwAICarryScoreMaxMultiple1 INT,			--��С���뱶��
	@dwAIScoreLevelRate1 INT,					--��������
	@dwAIScoreFloatMinRate1 INT,				--������Χ
	@dwAIScoreFloatMaxRate1 INT,				--������Χ
	
	@dwAICarryScoreMinMultiple2 INT,			--��С���뱶��
	@dwAICarryScoreMaxMultiple2 INT,			--��С���뱶��
	@dwAIScoreLevelRate2 INT,					--��������
	@dwAIScoreFloatMinRate2 INT,				--������Χ
	@dwAIScoreFloatMaxRate2 INT,				--������Χ
	
	@dwAICarryScoreMinMultiple3 INT,			--��С���뱶��
	@dwAICarryScoreMaxMultiple3 INT,			--��С���뱶��
	@dwAIScoreLevelRate3 INT,					--��������
	@dwAIScoreFloatMinRate3 INT,				--������Χ
	@dwAIScoreFloatMaxRate3 INT					--������Χ
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	IF EXISTS (SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID)
	BEGIN
		-- ���²���
		UPDATE AndroidConfigure SET ServiceMode=@dwServiceMode,AndroidCount=@dwAndroidCount,SwitchMinInnings=@dwSwitchMinInnings,SwitchMaxInnings=@dwSwitchMaxInnings,AIHangingMinRate=@dwAIHangingMinRate,AIHangingMaxRate=@dwAIHangingMaxRate,
		AICarryScoreMinMultiple0=@dwAICarryScoreMinMultiple0,AICarryScoreMaxMultiple0=@dwAICarryScoreMaxMultiple0,AIScoreLevelRate0=@dwAIScoreLevelRate0,dwAIScoreFloatMinRate0=@dwAIScoreFloatMinRate0,dwAIScoreFloatMaxRate0=@dwAIScoreFloatMaxRate0,
		AICarryScoreMinMultiple1=@dwAICarryScoreMinMultiple1,AICarryScoreMaxMultiple1=@dwAICarryScoreMaxMultiple1,AIScoreLevelRate1=@dwAIScoreLevelRate1,dwAIScoreFloatMinRate1=@dwAIScoreFloatMinRate1,dwAIScoreFloatMaxRate1=@dwAIScoreFloatMaxRate1,
		AICarryScoreMinMultiple2=@dwAICarryScoreMinMultiple2,AICarryScoreMaxMultiple2=@dwAICarryScoreMaxMultiple2,AIScoreLevelRate2=@dwAIScoreLevelRate2,dwAIScoreFloatMinRate2=@dwAIScoreFloatMinRate2,dwAIScoreFloatMaxRate2=@dwAIScoreFloatMaxRate2,
		AICarryScoreMinMultiple3=@dwAICarryScoreMinMultiple3,AICarryScoreMaxMultiple3=@dwAICarryScoreMaxMultiple3,AIScoreLevelRate3=@dwAIScoreLevelRate3,dwAIScoreFloatMinRate3=@dwAIScoreFloatMinRate3,dwAIScoreFloatMaxRate3=@dwAIScoreFloatMaxRate3
		WHERE ServerID=@wServerID
	END
	ELSE
	BEGIN
		-- ��������
		INSERT AndroidConfigure(ServerID,ServiceMode,AndroidCount,SwitchMinInnings,SwitchMaxInnings,AIHangingMinRate,AIHangingMaxRate,
		AICarryScoreMinMultiple0,AICarryScoreMaxMultiple0,AIScoreLevelRate0,dwAIScoreFloatMinRate0,dwAIScoreFloatMaxRate0,
		AICarryScoreMinMultiple1,AICarryScoreMaxMultiple1,AIScoreLevelRate1,dwAIScoreFloatMinRate1,dwAIScoreFloatMaxRate1,
		AICarryScoreMinMultiple2,AICarryScoreMaxMultiple2,AIScoreLevelRate2,dwAIScoreFloatMinRate2,dwAIScoreFloatMaxRate2,
		AICarryScoreMinMultiple3,AICarryScoreMaxMultiple3,AIScoreLevelRate3,dwAIScoreFloatMinRate3,dwAIScoreFloatMaxRate3)
		VALUES(@wServerID,@dwServiceMode,@dwAndroidCount,@dwSwitchMinInnings,@dwSwitchMaxInnings,@dwAIHangingMinRate,@dwAIHangingMaxRate,
		@dwAICarryScoreMinMultiple0,@dwAICarryScoreMaxMultiple0,@dwAIScoreLevelRate0,@dwAIScoreFloatMinRate0,@dwAIScoreFloatMaxRate0,
		@dwAICarryScoreMinMultiple1,@dwAICarryScoreMaxMultiple1,@dwAIScoreLevelRate1,@dwAIScoreFloatMinRate1,@dwAIScoreFloatMaxRate1,
		@dwAICarryScoreMinMultiple2,@dwAICarryScoreMaxMultiple2,@dwAIScoreLevelRate2,@dwAIScoreFloatMinRate2,@dwAIScoreFloatMaxRate2,
		@dwAICarryScoreMinMultiple3,@dwAICarryScoreMaxMultiple3,@dwAIScoreLevelRate3,@dwAIScoreFloatMinRate3,@dwAIScoreFloatMaxRate3)
	END
	
	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- �޸Ĳ���
CREATE PROC GSP_GP_AndroidModifyParameter
	@wServerID INT,								-- ���α�ʶ
	@dwServiceMode INT,							-- ����ģʽ
	@dwAndroidCount INT,						-- ������Ŀ
	@dwSwitchMinInnings INT,					-- ��������
	@dwSwitchMaxInnings INT,					-- ��������	
	@dwAIHangingMinRate INT,					-- �һ�����
	@dwAIHangingMaxRate INT,					-- �һ�����
	
	@dwAICarryScoreMinMultiple0 INT,			--��С���뱶��
	@dwAICarryScoreMaxMultiple0 INT,			--��С���뱶��
	@dwAIScoreLevelRate0 INT,					--��������
	@dwAIScoreFloatMinRate0 INT,				--������Χ
	@dwAIScoreFloatMaxRate0 INT,				--������Χ
	
	@dwAICarryScoreMinMultiple1 INT,			--��С���뱶��
	@dwAICarryScoreMaxMultiple1 INT,			--��С���뱶��
	@dwAIScoreLevelRate1 INT,					--��������
	@dwAIScoreFloatMinRate1 INT,				--������Χ
	@dwAIScoreFloatMaxRate1 INT,				--������Χ
	
	@dwAICarryScoreMinMultiple2 INT,			--��С���뱶��
	@dwAICarryScoreMaxMultiple2 INT,			--��С���뱶��
	@dwAIScoreLevelRate2 INT,					--��������
	@dwAIScoreFloatMinRate2 INT,				--������Χ
	@dwAIScoreFloatMaxRate2 INT,				--������Χ
	
	@dwAICarryScoreMinMultiple3 INT,			--��С���뱶��
	@dwAICarryScoreMaxMultiple3 INT,			--��С���뱶��
	@dwAIScoreLevelRate3 INT,					--��������
	@dwAIScoreFloatMinRate3 INT,				--������Χ
	@dwAIScoreFloatMaxRate3 INT					--������Χ
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���²���
	UPDATE AndroidConfigure SET ServiceMode=@dwServiceMode,AndroidCount=@dwAndroidCount,SwitchMinInnings=@dwSwitchMinInnings,SwitchMaxInnings=@dwSwitchMaxInnings,AIHangingMinRate=@dwAIHangingMinRate,AIHangingMaxRate=@dwAIHangingMaxRate,
	AICarryScoreMinMultiple0=@dwAICarryScoreMinMultiple0,AICarryScoreMaxMultiple0=@dwAICarryScoreMaxMultiple0,AIScoreLevelRate0=@dwAIScoreLevelRate0,dwAIScoreFloatMinRate0=@dwAIScoreFloatMinRate0,dwAIScoreFloatMaxRate0=@dwAIScoreFloatMaxRate0,
	AICarryScoreMinMultiple1=@dwAICarryScoreMinMultiple1,AICarryScoreMaxMultiple1=@dwAICarryScoreMaxMultiple1,AIScoreLevelRate1=@dwAIScoreLevelRate1,dwAIScoreFloatMinRate1=@dwAIScoreFloatMinRate1,dwAIScoreFloatMaxRate1=@dwAIScoreFloatMaxRate1,
	AICarryScoreMinMultiple2=@dwAICarryScoreMinMultiple2,AICarryScoreMaxMultiple2=@dwAICarryScoreMaxMultiple2,AIScoreLevelRate2=@dwAIScoreLevelRate2,dwAIScoreFloatMinRate2=@dwAIScoreFloatMinRate2,dwAIScoreFloatMaxRate2=@dwAIScoreFloatMaxRate2,
	AICarryScoreMinMultiple3=@dwAICarryScoreMinMultiple3,AICarryScoreMaxMultiple3=@dwAICarryScoreMaxMultiple3,AIScoreLevelRate3=@dwAIScoreLevelRate3,dwAIScoreFloatMinRate3=@dwAIScoreFloatMinRate3,dwAIScoreFloatMaxRate3=@dwAIScoreFloatMaxRate3
	WHERE ServerID=@wServerID
	
	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ������
CREATE PROC GSP_GP_AndroidDeleteParameter
	@wServerID INT								-- ���α�ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

	-- ɾ������
	DELETE AndroidConfigure WHERE ServerID=@wServerID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------