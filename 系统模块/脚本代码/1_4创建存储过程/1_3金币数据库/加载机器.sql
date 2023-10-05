
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

-- 加载机器
CREATE PROC GSP_GR_LoadAndroidUser
	@wServerID	SMALLINT,						-- 房间标识
	@dwAndroidCount INT							-- 机器数目
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
		-- 变量定义	
	DECLARE	@return_value int

	EXEC @return_value =  WHQJAccountsDB.dbo.GSP_GR_LoadAndroidUser
		 @wServerID = @wServerID,
		 @dwAndroidCount = @dwAndroidCount 

	RETURN @return_value
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 解锁机器
CREATE PROC GSP_GR_UnlockAndroidUser
	@dwUserID	INT,						-- 用户标识
	@wServerID	SMALLINT,					-- 房间标识	
	@cbAllAndroid TINYINT					-- 所有标识
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义	
	DECLARE	@return_value int

	EXEC @return_value =  WHQJAccountsDB.dbo.GSP_GR_UnlockAndroidUser 
		 @dwUserID=@dwUserID,	
		 @wServerID=@wServerID,
		 @cbAllAndroid=@cbAllAndroid
	RETURN @return_value


END

RETURN 0
----------------------------------------------------------------------------------------------------