
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_SaveOfflineMessage]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_SaveOfflineMessage]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadOfflineMessage]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadOfflineMessage]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �洢��Ϣ
CREATE PROC GSP_GP_SaveOfflineMessage
	@dwUserID INT,								-- �û� I D
	@wMessageType SMALLINT,						-- ��Ϣ����
	@wDataSize INT,								-- ���ݴ�С
	@szOfflineData varchar(2400),				-- ��������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��������
	INSERT OfflineMessage VALUES(@dwUserID,@wMessageType,@szOfflineData,@wDataSize)
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸�����
CREATE PROC GSP_GP_LoadOfflineMessage
	@dwUserID INT,								-- �û� I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯ����
	SELECT MessageType,DataSize,OfflineData FROM OfflineMessage WHERE UserID=@dwUserID

	-- ɾ������
    DELETE OfflineMessage WHERE UserID=@dwUserID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
