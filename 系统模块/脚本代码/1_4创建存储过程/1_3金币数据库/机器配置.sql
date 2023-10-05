
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_LoadAndroidConfigure') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_LoadAndroidConfigure
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ػ�������
CREATE PROC GSP_GR_LoadAndroidConfigure
	@wServerID INT							-- ���� I D
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM  WHQJAccountsDB.dbo.Androidconfigure
	WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------