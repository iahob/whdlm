
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
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
	-- 参数校验
	IF @wServerID=0 OR @dwAndroidCount=0 
	BEGIN
		RETURN 1
	END

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	BEGIN TRAN
	
	DECLARE @FreeSameRoomHours INT
	SELECT @FreeSameRoomHours = StatusValue FROM SystemStatusInfo WHERE StatusName = N'AIFreeSameRoomHours'
	IF @FreeSameRoomHours IS NULL SET @FreeSameRoomHours=3
	
	-- 查询机器
	SELECT UserID INTO #TempAndroids FROM AndroidLockInfo(NOLOCK)
	WHERE UserID IN (SELECT TOP(@dwAndroidCount) UserID FROM AndroidLockInfo WHERE AndroidStatus=0 AND ServiceServerID=0 
	AND (LastServerID=0 OR LastServerID<>@wServerID OR DATEDIFF(hh, LastLeaveTime, GetDate())>=@FreeSameRoomHours) ORDER BY NEWID())  
					
	-- 更新状态
	UPDATE AndroidLockInfo SET AndroidStatus=1,ServiceServerID=@wServerID,LastServerID=@wServerID,LastLockDateTime=GetDate() 
	FROM AndroidLockInfo a,#TempAndroids b WHERE a.UserID = b.UserID

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- 查询机器
	SELECT * FROM #TempAndroids

	-- 销毁临时表	
	IF OBJECT_ID('tempdb..#TempAndroids') IS NOT NULL DROP TABLE #TempAndroids
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
	IF @cbAllAndroid = 1 
	BEGIN
		-- 更新状态
		UPDATE AndroidLockInfo SET AndroidStatus=0, ServiceServerID=0, LastLeaveTime=GetDate()
		WHERE AndroidStatus<>0 AND ServiceServerID=@wServerID
	END
	ELSE
	BEGIN
		-- 更新状态
		UPDATE AndroidLockInfo SET AndroidStatus=0, ServiceServerID=0, LastLeaveTime=GetDate()
		WHERE AndroidStatus<>0 AND ServiceServerID=@wServerID AND UserID=@dwUserID
	END
END

RETURN 0

----------------------------------------------------------------------------------------------------