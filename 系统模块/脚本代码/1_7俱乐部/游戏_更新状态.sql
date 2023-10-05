
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
-- 更新在线状态
CREATE  PROCEDURE dbo.GSP_IM_UpdateOnlineStatus
	@dwGroupID INT,							-- 俱乐部标识
	@dwUserID INT,							-- 用户标识
	@cbOnlineStatus TINYINT,				-- 用户标识
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID			INT
DECLARE @Gender			TINYINT
DECLARE @GameID			INT
DECLARE @CustomID		INT
DECLARE @NickName		NVARCHAR(31)

-- 执行逻辑
BEGIN

	IF @cbOnlineStatus = 1
	BEGIN
		-- 更新在线状态
		UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = @cbOnlineStatus, EnterTime = GETDATE(), LeaveTime = GETDATE() WHERE GroupID = @dwGroupID AND UserID = @dwUserID   
	END
	ELSE
	BEGIN
		-- 更新在线状态
		UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = @cbOnlineStatus, LeaveTime = GETDATE(), OnlineTimeCount = OnlineTimeCount + DATEDIFF(SS, EnterTime, GETDATE()) WHERE GroupID = @dwGroupID AND UserID = @dwUserID    
	END
  
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------