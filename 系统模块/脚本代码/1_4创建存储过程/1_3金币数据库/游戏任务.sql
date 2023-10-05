----------------------------------------------------------------------------------------------------
USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryGameTaskInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryGameTaskInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_GameTaskForward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_GameTaskForward]
GO

----------------------------------------------------------------------------------------------------
-- 查询任务
CREATE PROC GSP_GR_QueryGameTaskInfo
	@wKindID	INT,							-- 游戏类型
	@wServerID	INT								-- 游戏房间
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--参数校验
	IF @wServerID=0		RETURN 1

	SELECT * FROM TaskInfo WHERE KindID=@wKindID AND ServerID=@wServerID
	UNION
	SELECT * FROM TaskInfo WHERE KindID=@wKindID AND ServerID=0
	UNION
	SELECT * FROM TaskInfo WHERE KindID=0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 任务推进
CREATE PROC GSP_GR_GameTaskForward
	@dwUserID INT,								-- 用户 I D
	@TaskID INT,								-- 任务 I D
	@lWinCount INT,								-- 赢局局数
	@lLostCount INT,							-- 输局局数
	@lDrawCount	INT								-- 和局局数
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

BEGIN
	--查询任务
	DECLARE @dwID INT 
	DECLARE @TaskInputDate DATETIME
	DECLARE @TimeType TINYINT
	DECLARE @TaskType INT
	DECLARE @dwProgress INT 
	DECLARE @TaskObject INT
	DECLARE @TaskStatus INT
	SELECT @dwID=ID,@TaskType=TaskType,@dwProgress=Progress,@TaskObject=TaskObject,@TaskStatus=TaskStatus,@TimeType=TimeType,@TaskInputDate=CollectDate 
	FROM  WHQJAccountsDB.dbo.AccountsTask	WHERE UserID=@dwUserID AND TaskID=@TaskID AND TaskStatus=0
	
	--查找不到任务 或者 任务已完成
	IF @dwID IS NULL RETURN 1
	
	--更新任务
	DECLARE @dwDateDiff INT
	SET @dwDateDiff = (CASE WHEN @TimeType=0x02 THEN DATEDIFF(DAY,@TaskInputDate,GETDATE())
							WHEN @TimeType=0x04 THEN DATEDIFF(WEEK,@TaskInputDate,GETDATE())
							WHEN @TimeType=0x08 THEN DATEDIFF(MONTH,@TaskInputDate,GETDATE())
						END)
	
	--任务超时 重新计算
	IF @dwDateDiff>0 UPDATE  WHQJAccountsDB.dbo.AccountsTask SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE ID=@dwID
	
	--推进任务 	
	SET @dwProgress = (CASE WHEN @TaskType=0x01 THEN @dwProgress+@lWinCount+@lLostCount+@lDrawCount		--总局任务
							WHEN @TaskType=0x02 THEN @dwProgress+@lWinCount								--胜局任务
							WHEN @TaskType=0x04 THEN @dwProgress+@lWinCount								--首胜任务
							WHEN @TaskType=0x08 AND @lWinCount>0 THEN @dwProgress+@lWinCount			--连胜任务
							WHEN @TaskType=0x07 AND @lWinCount=0 THEN 0
						END)
	--更新任务状态
	IF @dwProgress>=@TaskObject SET @TaskStatus=1
	
	--更新数据 
	UPDATE  WHQJAccountsDB.dbo.AccountsTask SET Progress=@dwProgress,TaskStatus=@TaskStatus WHERE ID=@dwID
	
END
RETURN 0
GO