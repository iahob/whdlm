
USE WHQJPlatformDB
GO

----写私人房配置信息
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_InsertPersonalCellScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_InsertPersonalCellScore]
GO

----写私人房配置信息
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeletePersonalCellScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeletePersonalCellScore]
GO

----写私人房配置信息
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetPersonalCellScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [GSP_MB_GetPersonalCellScore]
GO



SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 插入底分配置
CREATE PROC GSP_GR_InsertPersonalCellScore
	@wKindID    INT,							-- 游戏类型
	@lCellScore INT,							-- 游戏底分
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 如果当前房间玩家不存在
	DECLARE @lTempCellScore INT
	SELECT @lTempCellScore =CellScore FROM PersonalCellScore WHERE KindID = @wKindID


	INSERT INTO PersonalCellScore(KindID, CellScore) VALUES (@wKindID, @lCellScore) 

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
-- 删除底分配置
CREATE PROC GSP_GR_DeletePersonalCellScore
	@wKindID    INT				   -- 游戏类型

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	DELETE FROM PersonalCellScore WHERE KindID = @wKindID

END

RETURN 0

GO


------------------------------------------------------
---加载底分配置
CREATE PROC GSP_MB_GetPersonalCellScore
	@dwKindID INT								-- 游戏 I D
WITH ENCRYPTION AS

BEGIN

	SELECT  CellScore  FROM PersonalCellScore WHERE KindID=@dwKindID

END

return 0
GO