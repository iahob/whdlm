
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_QueryPersonalRoomUserInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_QueryPersonalRoomUserInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���󷿼���Ϣ
CREATE  PROCEDURE dbo.GSP_GS_QueryPersonalRoomUserInfo
	@dwUserID INT,								-- �����ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	--��ʯ
	DECLARE @lDiamond BIGINT	
	SELECT @lDiamond = Diamond  FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID = @dwUserID
	
	select @lDiamond AS Diamond
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------------------



-----------------------------------------------


