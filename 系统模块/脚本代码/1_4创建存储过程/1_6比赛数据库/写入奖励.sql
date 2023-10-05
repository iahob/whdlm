USE WHQJGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_MatchReward
	@dwUserID INT,								-- �û� I D	
	@lRewardGold BIGINT,						-- ��ҽ���
	@lRewardIngot BIGINT,						-- Ԫ����ʯ
	@dwRewardTicket BIGINT,					-- ��ȯ����
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15)					-- ���ӵ�ַ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- �������
	IF @lRewardGold>0 OR @lRewardIngot>0 OR @dwRewardTicket>0
	BEGIN
		-- ��ѯ���
		DECLARE @CurrScore BIGINT
		DECLARE @CurrInsure BIGINT
		DECLARE @CurrDiamond BIGINT
		DECLARE @CurrAwardTicket BIGINT
		SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID
		SELECT @CurrDiamond=Diamond, @CurrAwardTicket = AwardTicket FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID
	
		-- �����ж�
		IF @CurrScore IS NULL OR @CurrInsure IS NULL 
		BEGIN
			-- ��������
			SELECT @CurrScore=0,@CurrInsure=0
			
			-- �����¼
			INSERT WHQJTreasureDB..GameScoreInfo (UserID,Score,LastLogonIP,RegisterIP) 
			VALUES(@dwUserID,@lRewardGold,@strClientIP,@strClientIP)
		END
		

		-- ����ʱ��
		DECLARE @DateID INT
		SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
		
		-- ��������
		DECLARE @TypeID INT
		SET @TypeID=18
		
		-- ���½��
		UPDATE WHQJTreasureDB..GameScoreInfo SET Score=Score+@lRewardGold WHERE UserID=@dwUserID

		-- ��¼�����ˮ��¼
		DECLARE @DateTime DATETIME
		DECLARE @SerialNumber NVARCHAR(20)
		SET @DateTime = GETDATE()
		SELECT @SerialNumber=dbo.WF_GetSerialNumber()

		IF @lRewardGold IS NOT NULL AND @lRewardGold <> 0
		BEGIN
			INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@dwUserID,15,@CurrScore,@CurrInsure,@lRewardGold,@strClientIP,@DateTime)	
		END
		
		IF @lRewardIngot IS NOT NULL AND @lRewardIngot <> 0 
		BEGIN
			-- �����ж�
			IF @CurrDiamond IS NULL
			BEGIN
				-- �����¼
				INSERT WHQJTreasureDB..UserCurrency (UserID,Diamond) 
				VALUES(@dwUserID,@lRewardIngot)


				--��ʯ��־	
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,17,@CurrDiamond,@lRewardIngot,@strClientIP,@DateTime)
			END
			ELSE
			BEGIN
				-- ������ʯ�ͽ�ȯ
				UPDATE WHQJTreasureDB..UserCurrency SET Diamond=Diamond+@lRewardIngot WHERE UserID=@dwUserID

							--��ʯ��־	
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,17,@CurrDiamond,@lRewardIngot,@strClientIP,@DateTime)

			END
		END


		IF @dwRewardTicket IS NOT NULL AND @dwRewardTicket <> 0 
		BEGIN
			-- �����ж�
			IF @dwRewardTicket IS NULL
			BEGIN
				-- �����¼
				INSERT WHQJTreasureDB..UserCurrency (UserID,AwardTicket) 
				VALUES(@dwUserID,@dwRewardTicket)


				-- ������¼
				INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
				VALUES (@SerialNumber,0,@dwUserID,5,0,@dwRewardTicket,@strClientIP,GETDATE())
			END
			ELSE
			BEGIN
				-- ������ʯ�ͽ�ȯ
				UPDATE WHQJTreasureDB..UserCurrency SET AwardTicket=AwardTicket+@dwRewardTicket WHERE UserID=@dwUserID

				-- ������¼
				INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
				VALUES (@SerialNumber,0,@dwUserID,5,@CurrAwardTicket,@dwRewardTicket,@strClientIP,GETDATE())

			END
		END



	END
	
	-- ��ѯ���
	DECLARE @CurrGold BIGINT	
	SELECT @CurrGold=Score FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID

	-- ��������
	IF @CurrGold IS NULL SET @CurrGold=0		

	-- �׳�����
	SELECT @CurrGold AS Score
END

RETURN 0
GO