
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordVideoPlayerInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordVideoPlayerInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordVideoDataInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordVideoDataInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_QueryPlayBackCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_QueryPlayBackCode]
GO
SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 录像玩家信息
CREATE PROC GSP_GR_RecordVideoPlayerInfo	
	@dwUserID INT,								-- 用户标识
	@szVideoNumber  VARCHAR(22),				-- 视频ID	
	@lScore			BIGINT,						-- 输赢积分
	@wChairID		INT,						-- 椅子I D
	@wKindID		INT							-- 游戏类型	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 插入记录
	INSERT RecordVideoPlayerInfo(UserID,VideoNumber,Score,ChairID,KindID) VALUES (@dwUserID,@szVideoNumber,@lScore,@wChairID,@wKindID)
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 录像数据
CREATE PROC GSP_GR_RecordVideoDataInfo	
	@szVideoNumber  VARCHAR(22),				-- 视频ID
	@wServerID  int,
	@wTableID	int,
	@szVideoData	varbinary(MAX)						-- 视频数据	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 插入记录
	INSERT RecordVideoInfo(VideoNumber,RoomID,TableId,VideoData) VALUES (@szVideoNumber,@wServerID,@wTableID,@szVideoData)
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 查询回放码
CREATE PROC GSP_MB_QueryPlayBackCode
	@dwUserID		INT,							-- 用户 I D
	@szVideoNumber	varchar(22),					-- 视频I D
	@strErrorDescribe NVARCHAR(127) OUTPUT			-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	declare @bFind	INT
	declare @Code	INT
	SET @Code=0      
	select @Code=PlayBackCode from  [WHQJTreasureDB].[dbo].[RecordVideoPlayerInfo] where VideoNumber=@szVideoNumber and UserID=@dwUserID and DateDiff(d,BuildVideoTime,GetDate()) <= 10
	
	IF @Code=0
	BEGIN
		SET @bFind=0       
		WHILE @bFind=0    
		BEGIN
			SET @Code = cast(ceiling(rand() * 899999) as int)+100000
			IF NOT EXISTS (select * from  [WHQJTreasureDB].[dbo].[RecordVideoPlayerInfo] where PlayBackCode=@Code and DateDiff(d,BuildVideoTime,GetDate()) <= 10)
			BEGIN
				SET @bFind = 1  
				UPDATE [WHQJTreasureDB].[dbo].[RecordVideoPlayerInfo] SET PlayBackCode =@Code WHERE UserID=@dwUserID and VideoNumber=@szVideoNumber   
			END
		END 
	END	
	
	SELECT @Code as Code
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------