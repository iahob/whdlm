
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

-----不同时间段内AI总数变化
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAITimeChange]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAITimeChange]
GO


-----进入房间权重
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAIWeightIn]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAIWeightIn]
GO


-----进入房间权重
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAIWeightOut]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAIWeightOut]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
------------不同时间段内AI总数变化
CREATE PROC GSP_GP_LoadAITimeChange
	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--选择可用状态俱乐部的AI配置
	SELECT * FROM AITimeChange 

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

------------不同时间段内AI总数变化
CREATE PROC GSP_GP_LoadAIWeightIn
	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--选择可用状态俱乐部的AI配置
	SELECT * FROM AIWeightInRatio 

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

------------不同时间段内AI总数变化
CREATE PROC GSP_GP_LoadAIWeightOut
	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--选择可用状态俱乐部的AI配置
	SELECT * FROM AIWeightOutRatio 

END

RETURN 0

GO
