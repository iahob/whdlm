
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

----д˽�˷�������Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WritePersonalRoomJoinInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WritePersonalRoomJoinInfo]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------------------
-- ��Ϸд��
CREATE PROC GSP_GR_WritePersonalRoomJoinInfo
	-- ϵͳ��Ϣ
	@dwUserID INT,								-- �û� I D
	@dwTableID INT,								-- ���� I D
	@dwChairID INT,								-- ���� I D
	@dwKindID INT,								-- ��Ϸ ����
	@dwPersonalRoomID INT,						-- Լս����ID
	@szRoomGUID NVARCHAR(31),					-- ����Ψһ��ʶ	
	@dwRoomHostID INT							-- ����ID
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- �����ǰ������Ҳ�����
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