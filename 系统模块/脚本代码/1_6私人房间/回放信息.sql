
----------------------------------------------------------------------------------------------------
USE WHQJPlatformDB
GO
----д˽�˷�������Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteRecordBackInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteRecordBackInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ��Ϸд��
CREATE PROC GSP_GR_WriteRecordBackInfo	
	@strID					VARCHAR(22),			-- һ����Ϸ��ʶ	
	@strPersonalRoomGUID	NVARCHAR(31),			-- ˽�˷���Ψһ��ʶ					
	@dwUserID				INT,					-- �û� I D
	@dwPersonalRoomID		INT,					-- Լս����ID
	@lVariationScore		BIGINT,					-- �û�����
	@lGamesNum				INT,					-- ��Ϸ����	
	@cbGameMode				TINYINT,				-- ��Ϸģʽ
	@cbLoopCount			TINYINT					-- ��ϷȦ��
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	INSERT INTO  WHQJPlatformDB.dbo.RecordBackInfo(ID,PersonalRoomGUID ,UserID, RoomID, Score,GamesNum,GameMode,LoopCount,Dates) 
				VALUES (@strID,  @strPersonalRoomGUID, @dwUserID, @dwPersonalRoomID, @lVariationScore, @lGamesNum,@cbGameMode,@cbLoopCount,GETDATE()) 
	
END

RETURN 0

GO