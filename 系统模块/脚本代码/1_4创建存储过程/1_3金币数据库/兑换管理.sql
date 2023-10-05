
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadMemberParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadMemberParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_PurchaseMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_PurchaseMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ExchangeScoreByIngot]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ExchangeScoreByIngot]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ExchangeScoreByBeans]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ExchangeScoreByBeans]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ػ�Ա
CREATE PROC GSP_GR_LoadMemberParameter	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ���ػ�Ա
	SELECT MemberName, MemberOrder, MemberPrice, PresentScore FROM MemberType(NOLOCK)

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �����Ա
CREATE PROC GSP_GR_PurchaseMember

	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ
	@cbMemberOrder INT,							-- ��Ա��ʶ
	@PurchaseTime INT,							-- ����ʱ��

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
	DECLARE @CurrMemberOrder SMALLINT	
	SELECT @Nullity=Nullity, @CurrMemberOrder=MemberOrder FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �û��ж�
	IF @CurrMemberOrder IS NULL
	BEGIN
		SET @strNotifyContent=N'�����ʺŲ����ڣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strNotifyContent=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END

	-- ��������	
	DECLARE @MemberName AS NVARCHAR(16)
	DECLARE @MemberPrice AS DECIMAL(18,2)
	DECLARE @PresentScore AS BIGINT
	DECLARE @MemberRight AS INT	

	-- ��ȡ��Ա
	SELECT @MemberName=MemberName,@MemberPrice=MemberPrice, @PresentScore=PresentScore,@MemberRight=UserRight	
	FROM MemberType(NOLOCK) WHERE MemberOrder=@cbMemberOrder	

	-- �����ж�
	IF @MemberName IS NULL
	BEGIN
		SET @strNotifyContent=N'��Ǹ��֪ͨ������������Ļ�Ա�����ڻ�������ά���У�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END

	-- ���ü���
	DECLARE @ConsumeCurrency DECIMAL(18,2)
	SELECT @ConsumeCurrency=@MemberPrice*@PurchaseTime	

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��ȡ��Ϸ��
	DECLARE @Currency DECIMAL(18,2)
--	SELECT @Currency=Currency FROM UserCurrencyInfo(NOLOCK) WHERE UserID=@dwUserID
	IF @Currency IS NULL Set @Currency=0

	-- ��Ϸ���ж�
--	IF @Currency<@ConsumeCurrency
--	BEGIN
--		-- ��������
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

--		-- ������Ϣ
--		SET @strNotifyContent=N'�����ϵ���Ϸ�����㣬���ֵ���ٴγ��ԣ�'
--		RETURN 5
--	END

	-- ��ȡ��Ϸ��
	DECLARE @Score BIGINT
	SELECT @Score=Score FROM GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��������
	IF @Score IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @strClientIP, @strMachineID, @strClientIP, @strMachineID)

		-- ��ѯ��Ϸ��
		SELECT @Score=Score	FROM GameScoreInfo WHERE UserID=@dwUserID
	END	

	-- ���ͼ���
	DECLARE @TotalPreSentScore BIGINT
	SELECT @TotalPreSentScore=@PreSentScore*@PurchaseTime		

	-- �һ���־
	INSERT INTO RecordBuyMember(UserID,MemberOrder,MemberMonths,MemberPrice,Currency,PresentScore,BeforeCurrency,BeforeScore,ClinetIP,InputDate)
	VALUES(@dwUserID,@cbMemberOrder,@PurchaseTime,@MemberPrice,@ConsumeCurrency,@TotalPreSentScore,0,@Score,@strClientIP,GETDATE())
	
	-- �仯��־
	INSERT INTO RecordCurrencyChange(UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
	VALUES(@dwUserID,@ConsumeCurrency,10,@Currency,@Currency-@ConsumeCurrency,@strClientIP,GETDATE(),'�һ���Ϸ��')	

	-- ��Ϸ���۷�
--	UPDATE UserCurrencyInfo SET Currency=Currency-@ConsumeCurrency WHERE UserID=@dwUserID

	-- ������Ϸ��
	UPDATE GameScoreInfo SET Score=Score+@TotalPreSentScore WHERE UserID=@dwUserID	

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
		
	-- ��Ա����
	DECLARE @MaxUserRight INT
	DECLARE @MemberValidMonth INT
	DECLARE @MaxMemberOrder TINYINT	 
	DECLARE @MemberOverDate DATETIME
	DECLARE @MemberSwitchDate DATETIME

	-- ��Ч����
	SELECT @MemberValidMonth= @PurchaseTime

	-- ɾ������
	DELETE FROM  WHQJAccountsDB.dbo.AccountsMember
	WHERE UserID=@dwUserID AND MemberOrder=@cbMemberOrder AND MemberOverDate<=GETDATE()

	-- ���»�Ա
	UPDATE  WHQJAccountsDB.dbo.AccountsMember SET MemberOverDate=DATEADD(mm,@MemberValidMonth, MemberOverDate)
	WHERE UserID=@dwUserID AND MemberOrder=@cbMemberOrder
	IF @@ROWCOUNT=0
	BEGIN
		INSERT  WHQJAccountsDB.dbo.AccountsMember(UserID,MemberOrder,UserRight,MemberOverDate)
		VALUES (@dwUserID,@cbMemberOrder,@MemberRight,DATEADD(mm,@MemberValidMonth, GETDATE()))
	END

	-- �󶨻�Ա,(��Ա�������л�ʱ��)
	SELECT @MaxMemberOrder=MAX(MemberOrder),@MemberOverDate=MAX(MemberOverDate),@MemberSwitchDate=MIN(MemberOverDate)
	FROM  WHQJAccountsDB.dbo.AccountsMember WHERE UserID=@dwUserID

	-- ��ԱȨ��
	SELECT @MaxUserRight=UserRight FROM  WHQJAccountsDB.dbo.AccountsMember
	WHERE UserID=@dwUserID AND MemberOrder=@MaxMemberOrder
	
	-- ���ӻ�Ա����Ϣ
	UPDATE  WHQJAccountsDB.dbo.AccountsInfo
	SET MemberOrder=@MaxMemberOrder,UserRight=@MaxUserRight,MemberOverDate=@MemberOverDate,MemberSwitchDate=@MemberSwitchDate
	WHERE UserID=@dwUserID

	-- ��Ա�ȼ�
	SELECT @CurrMemberOrder=MemberOrder FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- ��ѯ��Ϸ��
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM GameScoreInfo WHERE UserID=@dwUserID	

	-- �ɹ���ʾ
	SET @strNotifyContent=N'��ϲ������Ա����ɹ���' 

	-- �����¼
	SELECT @CurrMemberOrder AS MemberOrder,@MaxUserRight AS UserRight,@CurrScore AS CurrScore,0 AS CurrBeans

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �һ���Ϸ��
CREATE PROC GSP_GR_ExchangeScoreByIngot

	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ
	@ExchangeIngot INT,							-- �һ�Ԫ��	

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
	DECLARE @UserIngot INT	
	SELECT @Nullity=Nullity FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �û��ж�
	IF @UserIngot IS NULL
	BEGIN
		SET @strNotifyContent=N'�����ʺŲ����ڣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strNotifyContent=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END

	-- Ԫ���ж�
	IF @UserIngot < @ExchangeIngot
	BEGIN
		SET @strNotifyContent=N'����Ԫ�����㣬������öһ��������ԣ�'
		RETURN 3		
	END

	-- �һ�����
	DECLARE @ExchangeRate INT
	SELECT @ExchangeRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MedalExchangeRate'

	-- ϵͳ����
	IF @ExchangeRate IS NULL
	BEGIN
		SET @strNotifyContent=N'��Ǹ��Ԫ���һ�ʧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4			
	END

	-- ������Ϸ��
	DECLARE @ExchangeScore BIGINT
	SET @ExchangeScore = @ExchangeRate*@ExchangeIngot

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
	
	-- ��ѯ��Ϸ��
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��ѯ��Ϸ��
	DECLARE @CurrBeans decimal(18, 2)
--	SELECT @CurrBeans=Currency FROM UserCurrencyInfo WHERE UserID=@dwUserID	

	-- ������Ϸ��
	IF @CurrBeans IS NULL SET @CurrBeans=0

	-- �����¼
	INSERT WHQJRecordDB.dbo.RecordConvertUserMedal (UserID, CurInsureScore, CurUserMedal, ConvertUserMedal, ConvertRate, IsGamePlaza, ClientIP, CollectDate)
	VALUES(@dwUserID, @InsureScore, @UserIngot, @ExchangeIngot, @ExchangeRate, 0, @strClientIP, GetDate())

	-- �ɹ���ʾ
	SET @strNotifyContent=N'��ϲ������Ϸ�Ҷһ��ɹ���'

	-- �����¼
	SELECT @UserIngot AS CurrIngot,@CurrScore AS CurrScore,@CurrBeans AS CurrBeans

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �һ���Ϸ��
CREATE PROC GSP_GR_ExchangeScoreByBeans

	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ
	@ExchangeBeans decimal(18, 2),				-- �һ���Ϸ��	

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
	SELECT @Nullity=Nullity FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �û��ж�
	IF @Nullity IS NULL
	BEGIN
		SET @strNotifyContent=N'�����ʺŲ����ڣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strNotifyContent=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END

	-- ��ȡ��Ϸ��
	DECLARE @UserBeans decimal(18, 2)
--	SELECT @UserBeans=Currency FROM UserCurrencyInfo WHERE UserID=@dwUserID	

	-- ������Ϸ��
	IF @UserBeans IS NULL SET @UserBeans=0

	-- ��Ϸ���ж�
	IF @UserBeans < @ExchangeBeans
	BEGIN
		SET @strNotifyContent=N'������Ϸ�����㣬������öһ��������ԣ�'
		RETURN 3		
	END

	-- �һ�����
	DECLARE @ExchangeRate INT
	SELECT @ExchangeRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RateGold'

	-- ϵͳ����
	IF @ExchangeRate IS NULL
	BEGIN
		SET @strNotifyContent=N'��Ǹ����Ϸ���һ�ʧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4			
	END

	-- ������Ϸ��
	DECLARE @ExchangeScore BIGINT
	SET @ExchangeScore=@ExchangeRate*@ExchangeBeans

	-- ��������
	UPDATE GameScoreInfo SET Score=Score+@ExchangeScore WHERE UserID=@dwUserID

	-- ��������
	IF @@ROWCOUNT=0
	BEGIN

		-- ��������
		INSERT INTO GameScoreInfo (UserID, Score, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @ExchangeScore, @strClientIP, @strMachineID, @strClientIP, @strMachineID)		
	END	

	-- ������Ϸ��
	SET @UserBeans=@UserBeans-@ExchangeBeans	
--	UPDATE UserCurrencyInfo SET Currency=@UserBeans WHERE UserID=@dwUserID			

	-- ��ѯ��Ϸ��
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM GameScoreInfo WHERE UserID=@dwUserID

	-- �仯��־
	INSERT INTO RecordCurrencyChange(UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
	VALUES(@dwUserID,@ExchangeBeans,10,@UserBeans+@ExchangeBeans,@UserBeans,@strClientIP,GETDATE(),'�һ���Ϸ��')	

	-- �ɹ���ʾ
	SET @strNotifyContent=N'��ϲ������Ϸ�Ҷһ��ɹ���'

	-- �����¼
	SELECT @CurrScore AS CurrScore,@UserBeans AS CurrBeans

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------