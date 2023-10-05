
----------------------------------------------------------------------------------------------------
USE WHQJPlatformDB
GO
----写私人房配置信息
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteRecordBackInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteRecordBackInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 游戏写分
CREATE PROC GSP_GR_WriteRecordBackInfo	
	@strID					VARCHAR(22),			-- 一局游戏标识	
	@strPersonalRoomGUID	NVARCHAR(31),			-- 私人房间唯一标识					
	@dwUserID				INT,					-- 用户 I D
	@dwPersonalRoomID		INT,					-- 约战房间ID
	@lVariationScore		BIGINT,					-- 用户分数
	@lGamesNum				INT,					-- 游戏局数	
	@cbGameMode				TINYINT,				-- 游戏模式
	@cbLoopCount			TINYINT					-- 游戏圈数
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	INSERT INTO  WHQJPlatformDB.dbo.RecordBackInfo(ID,PersonalRoomGUID ,UserID, RoomID, Score,GamesNum,GameMode,LoopCount,Dates) 
				VALUES (@strID,  @strPersonalRoomGUID, @dwUserID, @dwPersonalRoomID, @lVariationScore, @lGamesNum,@cbGameMode,@cbLoopCount,GETDATE()) 
	
END

RETURN 0

GO