
----------------------------------------------------------------------------------------------------

USE WHQJGameScoreDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载配置
CREATE PROC GSP_GR_LoadParameter
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT							-- 房间 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 变量定义
	DECLARE @ClientVersion AS INT
	DECLARE @ServerVersion AS INT
	DECLARE @DayWinLostScoreLimit AS BIGINT
	DECLARE @DayPlayTimeLimit AS INT
	-- 程序版本
	SELECT @ClientVersion=TableGame.ClientVersion, @ServerVersion=TableGame.ServerVersion
	FROM  WHQJPlatformDB.dbo.GameGameItem TableGame, WHQJPlatformDB.dbo.GameKindItem TableKind
	WHERE TableGame.GameID=TableKind.GameID	AND TableKind.KindID=@wKindID
	--每日限制
	SELECT @DayWinLostScoreLimit=StatusValue
	FROM WHQJAccountsDB.dbo.SystemStatusInfo
	WHERE StatusName=N'DayWinLostScoreLimit'
	
	SELECT @DayPlayTimeLimit=StatusValue
	FROM WHQJAccountsDB.dbo.SystemStatusInfo
	WHERE StatusName=N'DayPlayTimeLimit'

	--参数校正
	IF @ClientVersion IS NULL SET @ClientVersion=0
	IF @ServerVersion IS NULL SET @ServerVersion=0
	IF @DayWinLostScoreLimit IS NULL SET @DayWinLostScoreLimit=0
	IF @DayPlayTimeLimit IS NULL SET @DayPlayTimeLimit=0
	

	SELECT @ClientVersion AS ClientVersion, @ServerVersion AS ServerVersion,@DayWinLostScoreLimit AS DayWinLostScoreLimit, @DayPlayTimeLimit AS DayPlayTimeLimit

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------