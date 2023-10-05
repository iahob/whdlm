
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_SearchUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_SearchUser]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �޸�����
CREATE PROC GSP_GP_SearchUser
	@dwGameID INT						-- �û��˺�
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯ����
	SELECT UserID,GameID,FaceID,CustomID,Gender,MemberOrder,NickName,ISNULL(UnderWrite,N'') AS UnderWrite FROM AccountsInfo WHERE GameID=@dwGameID			
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
