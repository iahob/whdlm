
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
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
		-- ��������	
	DECLARE	@return_value int

	EXEC @return_value =  WHQJAccountsDB.dbo.GSP_GR_LoadAndroidUser
		 @wServerID = @wServerID,
		 @dwAndroidCount = @dwAndroidCount 

	RETURN @return_value
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

	-- ��������	
	DECLARE	@return_value int

	EXEC @return_value =  WHQJAccountsDB.dbo.GSP_GR_UnlockAndroidUser 
		 @dwUserID=@dwUserID,	
		 @wServerID=@wServerID,
		 @cbAllAndroid=@cbAllAndroid
	RETURN @return_value


END

RETURN 0
----------------------------------------------------------------------------------------------------