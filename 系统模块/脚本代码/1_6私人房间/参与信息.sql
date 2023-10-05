
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

----写私人房配置信息
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WritePersonalRoomJoinInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WritePersonalRoomJoinInfo]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------------------
-- 游戏写分
CREATE PROC GSP_GR_WritePersonalRoomJoinInfo
	-- 系统信息
	@dwUserID INT,								-- 用户 I D
	@dwTableID INT,								-- 桌子 I D
	@dwChairID INT,								-- 椅子 I D
	@dwKindID INT,								-- 游戏 类型
	@dwPersonalRoomID INT,						-- 约战房间ID
	@szRoomGUID NVARCHAR(31),					-- 房间唯一标识	
	@dwRoomHostID INT							-- 房主ID
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 如果当前房间玩家不存在
	DECLARE @dwExistUserID INT
	DECLARE @CellScore	BIGINT
	SELECT @CellScore = CellScore FROM StreamCreateTableFeeInfo WHERE  RoomID = @dwPersonalRoomID AND UserID = @dwRoomHostID	

	SELECT @dwExistUserID = UserID FROM PersonalRoomScoreInfo WHERE PersonalRoomGUID = @szRoomGUID AND UserID = @dwUserID

	IF @dwExistUserID IS NULL
	BEGIN
		INSERT INTO PersonalRoomScoreInfo(UserID, RoomID, Score, WinCount, LostCount, DrawCount, FleeCount, WriteTime, PersonalRoomGUID, ChairID,  KindID, CellScore, StartTime, RoomHostID) 
												VALUES (@dwUserID, @dwPersonalRoomID, 0,0, 0, 0, 0, GETDATE(), @szRoomGUID, @dwChairID, @dwKindID, @CellScore, GETDATE(), @dwRoomHostID )  
												
	  	UPDATE StreamCreateTableFeeInfo SET RoomStatus = 1 WHERE  RoomID = @dwPersonalRoomID AND UserID = @dwRoomHostID		
	END

END

RETURN 0

GO