
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadRealAuth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadRealAuth]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO
----------------------------------------------------------------------------------------------------
-- ���ؽ���
CREATE PROC GSP_GP_LoadRealAuth
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	--��ѯ����
	DECLARE @AuthRealAward AS BIGINT
	SELECT @AuthRealAward=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'AuthentPresent'
	
	DECLARE @AuthentDisable AS BIGINT
	SELECT @AuthentDisable=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'AuthentDisable'	
	-- �׳�����
	SELECT @AuthRealAward AS AuthRealAward ,@AuthentDisable AS AuthentDisable
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------