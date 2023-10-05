
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_UpdateOnlineStatus]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_UpdateOnlineStatus]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------
-- ��������״̬
CREATE  PROCEDURE dbo.GSP_IM_UpdateOnlineStatus
	@dwGroupID INT,							-- ���ֲ���ʶ
	@dwUserID INT,							-- �û���ʶ
	@cbOnlineStatus TINYINT,				-- �û���ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID			INT
DECLARE @Gender			TINYINT
DECLARE @GameID			INT
DECLARE @CustomID		INT
DECLARE @NickName		NVARCHAR(31)

-- ִ���߼�
BEGIN

	IF @cbOnlineStatus = 1
	BEGIN
		-- ��������״̬
		UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = @cbOnlineStatus, EnterTime = GETDATE(), LeaveTime = GETDATE() WHERE GroupID = @dwGroupID AND UserID = @dwUserID   
	END
	ELSE
	BEGIN
		-- ��������״̬
		UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = @cbOnlineStatus, LeaveTime = GETDATE(), OnlineTimeCount = OnlineTimeCount + DATEDIFF(SS, EnterTime, GETDATE()) WHERE GroupID = @dwGroupID AND UserID = @dwUserID    
	END
  
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------