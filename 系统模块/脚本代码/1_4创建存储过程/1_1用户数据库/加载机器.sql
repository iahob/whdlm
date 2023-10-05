
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_LoadAndroidUser') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_LoadAndroidUser
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_UnLockAndroidUser') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_UnLockAndroidUser
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ػ���
CREATE PROC GSP_GR_LoadAndroidUser
	@wServerID	SMALLINT,						-- �����ʶ
	@dwAndroidCount INT							-- ������Ŀ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ����У��
	IF @wServerID=0 OR @dwAndroidCount=0 
	BEGIN
		RETURN 1
	END

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	BEGIN TRAN
	
	DECLARE @FreeSameRoomHours INT
	SELECT @FreeSameRoomHours = StatusValue FROM SystemStatusInfo WHERE StatusName = N'AIFreeSameRoomHours'
	IF @FreeSameRoomHours IS NULL SET @FreeSameRoomHours=3
	
	-- ��ѯ����
	SELECT UserID INTO #TempAndroids FROM AndroidLockInfo(NOLOCK)
	WHERE UserID IN (SELECT TOP(@dwAndroidCount) UserID FROM AndroidLockInfo WHERE AndroidStatus=0 AND ServiceServerID=0 
	AND (LastServerID=0 OR LastServerID<>@wServerID OR DATEDIFF(hh, LastLeaveTime, GetDate())>=@FreeSameRoomHours) ORDER BY NEWID())  
					
	-- ����״̬
	UPDATE AndroidLockInfo SET AndroidStatus=1,ServiceServerID=@wServerID,LastServerID=@wServerID,LastLockDateTime=GetDate() 
	FROM AndroidLockInfo a,#TempAndroids b WHERE a.UserID = b.UserID

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- ��ѯ����
	SELECT * FROM #TempAndroids

	-- ������ʱ��	
	IF OBJECT_ID('tempdb..#TempAndroids') IS NOT NULL DROP TABLE #TempAndroids
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_UnlockAndroidUser
	@dwUserID	INT,						-- �û���ʶ
	@wServerID	SMALLINT,					-- �����ʶ	
	@cbAllAndroid TINYINT					-- ���б�ʶ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	IF @cbAllAndroid = 1 
	BEGIN
		-- ����״̬
		UPDATE AndroidLockInfo SET AndroidStatus=0, ServiceServerID=0, LastLeaveTime=GetDate()
		WHERE AndroidStatus<>0 AND ServiceServerID=@wServerID
	END
	ELSE
	BEGIN
		-- ����״̬
		UPDATE AndroidLockInfo SET AndroidStatus=0, ServiceServerID=0, LastLeaveTime=GetDate()
		WHERE AndroidStatus<>0 AND ServiceServerID=@wServerID AND UserID=@dwUserID
	END
END

RETURN 0

----------------------------------------------------------------------------------------------------