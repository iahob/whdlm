
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ExchangeRoomCardByScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ExchangeRoomCardByScore]
GO
----------------------------------------------------------------------------------------------------

-- ��ʯ�һ����
CREATE PROC GSP_GR_ExchangeRoomCardByScore

	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ
	@ExchangeRoomCard INT,						-- �һ���ʯ	

	-- ϵͳ��Ϣ	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strNotifyContent NVARCHAR(127) OUTPUT		-- �����Ϣ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ��Ա
	DECLARE @Nullity BIT
	DECLARE @UserDiamond INT	
	SELECT @Nullity=Nullity FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	SELECT @UserDiamond=Diamond FROM UserCurrency WHERE UserID=@dwUserID

	-- �û��ж�
	IF @UserDiamond IS NULL
	BEGIN
		SET @strNotifyContent=N'��û����ʯ�빺��'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strNotifyContent=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END

	-- Ԫ���ж�
	IF @UserDiamond < @ExchangeRoomCard
	BEGIN
		SET @strNotifyContent=N'������ʯ���㣬������öһ���ʯ�����ԣ�'
		RETURN 3		
	END

	-- �һ�����
	DECLARE @ExchangeRate  BIGINT
	--SELECT @ExchangeRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'GeneralRoomCardExchRate'
	SELECT @ExchangeRate=UseResultsGold FROM  WHQJPlatformDB.dbo.GameProperty WHERE ID=501

	-- ϵͳ����
	IF @ExchangeRate IS NULL
	BEGIN
		SET @strNotifyContent=N'��Ǹ����ʯ�һ�ʧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4			
	END

	-- ������Ϸ��
	DECLARE @ExchangeScore BIGINT
	SET @ExchangeScore = @ExchangeRate*@ExchangeRoomCard

	-- ��ѯ����
	DECLARE @InsureScore BIGINT
	SELECT @InsureScore=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��������
	IF @InsureScore IS NULL
	BEGIN
		-- ���ñ���
		SET @InsureScore=0

		-- ��������
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @strClientIP, @strMachineID, @strClientIP, @strMachineID)		
	END	

	-- ��������
	UPDATE GameScoreInfo SET InsureScore=InsureScore+@ExchangeScore WHERE UserID=@dwUserID	
	
	-- ������ʯ
	SET @UserDiamond=@UserDiamond-@ExchangeRoomCard
	UPDATE UserCurrency SET Diamond=@UserDiamond WHERE UserID=@dwUserID			

	-- ��ѯ��Ϸ��
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM GameScoreInfo WHERE UserID=@dwUserID

	

	--��ȡ��Һ���Ϸ��
	DECLARE @lGold  BIGINT
	DECLARE @Currency  DECIMAL
	SELECT  @lGold = Score FROM GameScoreInfo WHERE UserID = @dwUserID

	INSERT INTO WHQJRecordDB..RecordRoomCard(SourceUserID, SBeforeCard, RoomCard, TargetUserID, TBeforeCard,   Gold, Remarks, ClientIP, CollectDate)
	VALUES (@dwUserID, @UserDiamond, @ExchangeRoomCard, 0, 0,@lGold, '�һ���ʯ', @strClientIP, GETDATE())
		

	-- �ɹ���ʾ
	SET @strNotifyContent=N'��ϲ������Ҷһ��ɹ���'

	-- �����¼
	SELECT @UserDiamond AS Diamond,@CurrScore AS CurrScore

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


----------------------------------------------------------------------------------------------------