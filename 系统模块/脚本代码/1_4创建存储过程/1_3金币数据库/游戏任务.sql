----------------------------------------------------------------------------------------------------
USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryGameTaskInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryGameTaskInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_GameTaskForward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_GameTaskForward]
GO

----------------------------------------------------------------------------------------------------
-- ��ѯ����
CREATE PROC GSP_GR_QueryGameTaskInfo
	@wKindID	INT,							-- ��Ϸ����
	@wServerID	INT								-- ��Ϸ����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	--����У��
	IF @wServerID=0		RETURN 1

	SELECT * FROM TaskInfo WHERE KindID=@wKindID AND ServerID=@wServerID
	UNION
	SELECT * FROM TaskInfo WHERE KindID=@wKindID AND ServerID=0
	UNION
	SELECT * FROM TaskInfo WHERE KindID=0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- �����ƽ�
CREATE PROC GSP_GR_GameTaskForward
	@dwUserID INT,								-- �û� I D
	@TaskID INT,								-- ���� I D
	@lWinCount INT,								-- Ӯ�־���
	@lLostCount INT,							-- ��־���
	@lDrawCount	INT								-- �;־���
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

BEGIN
	--��ѯ����
	DECLARE @dwID INT 
	DECLARE @TaskInputDate DATETIME
	DECLARE @TimeType TINYINT
	DECLARE @TaskType INT
	DECLARE @dwProgress INT 
	DECLARE @TaskObject INT
	DECLARE @TaskStatus INT
	SELECT @dwID=ID,@TaskType=TaskType,@dwProgress=Progress,@TaskObject=TaskObject,@TaskStatus=TaskStatus,@TimeType=TimeType,@TaskInputDate=CollectDate 
	FROM  WHQJAccountsDB.dbo.AccountsTask	WHERE UserID=@dwUserID AND TaskID=@TaskID AND TaskStatus=0
	
	--���Ҳ������� ���� ���������
	IF @dwID IS NULL RETURN 1
	
	--��������
	DECLARE @dwDateDiff INT
	SET @dwDateDiff = (CASE WHEN @TimeType=0x02 THEN DATEDIFF(DAY,@TaskInputDate,GETDATE())
							WHEN @TimeType=0x04 THEN DATEDIFF(WEEK,@TaskInputDate,GETDATE())
							WHEN @TimeType=0x08 THEN DATEDIFF(MONTH,@TaskInputDate,GETDATE())
						END)
	
	--����ʱ ���¼���
	IF @dwDateDiff>0 UPDATE  WHQJAccountsDB.dbo.AccountsTask SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE ID=@dwID
	
	--�ƽ����� 	
	SET @dwProgress = (CASE WHEN @TaskType=0x01 THEN @dwProgress+@lWinCount+@lLostCount+@lDrawCount		--�ܾ�����
							WHEN @TaskType=0x02 THEN @dwProgress+@lWinCount								--ʤ������
							WHEN @TaskType=0x04 THEN @dwProgress+@lWinCount								--��ʤ����
							WHEN @TaskType=0x08 AND @lWinCount>0 THEN @dwProgress+@lWinCount			--��ʤ����
							WHEN @TaskType=0x07 AND @lWinCount=0 THEN 0
						END)
	--��������״̬
	IF @dwProgress>=@TaskObject SET @TaskStatus=1
	
	--�������� 
	UPDATE  WHQJAccountsDB.dbo.AccountsTask SET Progress=@dwProgress,TaskStatus=@TaskStatus WHERE ID=@dwID
	
END
RETURN 0
GO