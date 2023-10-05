
USE WHQJPlatformDB
GO

----д˽�˷�������Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_InsertPersonalCellScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_InsertPersonalCellScore]
GO

----д˽�˷�������Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeletePersonalCellScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeletePersonalCellScore]
GO

----д˽�˷�������Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetPersonalCellScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [GSP_MB_GetPersonalCellScore]
GO



SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ����׷�����
CREATE PROC GSP_GR_InsertPersonalCellScore
	@wKindID    INT,							-- ��Ϸ����
	@lCellScore INT,							-- ��Ϸ�׷�
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- �����ǰ������Ҳ�����
	DECLARE @lTempCellScore INT
	SELECT @lTempCellScore =CellScore FROM PersonalCellScore WHERE KindID = @wKindID


	INSERT INTO PersonalCellScore(KindID, CellScore) VALUES (@wKindID, @lCellScore) 

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
-- ɾ���׷�����
CREATE PROC GSP_GR_DeletePersonalCellScore
	@wKindID    INT				   -- ��Ϸ����

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	DELETE FROM PersonalCellScore WHERE KindID = @wKindID

END

RETURN 0

GO


------------------------------------------------------
---���ص׷�����
CREATE PROC GSP_MB_GetPersonalCellScore
	@dwKindID INT								-- ��Ϸ I D
WITH ENCRYPTION AS

BEGIN

	SELECT  CellScore  FROM PersonalCellScore WHERE KindID=@dwKindID

END

return 0
GO