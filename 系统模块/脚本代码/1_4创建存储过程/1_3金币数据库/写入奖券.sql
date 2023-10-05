
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteAwardTicket]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteGameScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��Ϸд��
CREATE PROC GSP_GR_WriteAwardTicket

	-- �û���Ϣ
	@dwUserID INT,						-- �û� I D
	@nAwardTicket INT					-- ��Ϸʱ��

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���½�ȯ
	UPDATE UserCurrency SET AwardTicket=AwardTicket+@nAwardTicket WHERE UserID=@dwUserID

	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------