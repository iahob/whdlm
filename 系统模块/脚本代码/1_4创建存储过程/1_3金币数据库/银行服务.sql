
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserEnableInsure]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserEnableInsure]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserSaveScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserSaveScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserTakeScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserTakeScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserTransferScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserTransferScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryUserInsureInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryUserInsureInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryTransferRebate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryTransferRebate]
GO

--���ҹ���
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_GetAllKindMoney]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_GetAllKindMoney]
GO

--�Ƹ���Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_GetUserWealth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_GetUserWealth]
GO

--����ת�ַ�������
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSF_GR_Switch_Num]') and OBJECTPROPERTY(ID, N'IsScalarFunction') = 1)
DROP FUNCTION [dbo].[GSF_GR_Switch_Num]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON
GO

----------------------------------------------------------------------------------------------------

-- ��ͨ����
CREATE PROC GSP_GR_UserEnableInsure
	@dwUserID INT,								-- �û� I D
	@strLogonPass NCHAR(32),					-- ��¼����
	@strInsurePass NCHAR(32),					-- ��������	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF NOT EXISTS(SELECT * FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strLogonPass)
	BEGIN
		SET @strErrorDescribe=N'������֤ʧ�ܣ��޷���ͨ���У�'
		RETURN 1		
	END
	
	-- ����У��
	IF @strInsurePass = @strLogonPass
	BEGIN
		SET @strErrorDescribe=N'���벻�����¼������ͬ���޷���ͨ���У�'
		RETURN 1		
	END	
	
	-- ��������
	UPDATE  WHQJAccountsDB.dbo.AccountsInfo SET InsurePass=@strInsurePass	WHERE UserID=@dwUserID		

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ������
CREATE PROC GSP_GR_UserSaveScore
	@dwUserID INT,								-- �û� I D
	@lSaveScore BIGINT,							-- �����Ŀ
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��ұ���
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT
DECLARE @InsureRevenue BIGINT
DECLARE @VariationScore BIGINT
DECLARE @VariationInsure BIGINT

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT

	-- ϵͳ��ͣ
	SELECT @EnjoinInsure=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ����Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ�������Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END
 
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END
	
	-- ����ж�
	DECLARE @BankPrerequisite AS INT
	SELECT @BankPrerequisite=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankPrerequisite'
	IF @BankPrerequisite IS NULL SET @BankPrerequisite=0
	IF @lSaveScore<@BankPrerequisite
	BEGIN
		SET @strErrorDescribe=N'����ʧ�ܣ����δ���С��'+dbo.GSF_GR_Switch_Num(@BankPrerequisite)+'��'
		RETURN 4
	END

	DECLARE @BankMax AS INT
	SELECT @BankMax=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankGameMax'
	IF @BankMax IS NULL SET @BankMax=0
	IF @lSaveScore>@BankMax
	BEGIN
		SET @strErrorDescribe=N'����ʧ�ܣ����δ��ܴ���'+dbo.GSF_GR_Switch_Num(@BankMax)+'��'
		RETURN 5
	END

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��Ϸ��Ϣ
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ����ж�
	IF @SourceScore IS NULL OR @SourceScore<@lSaveScore
	BEGIN
		-- ��������
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'����ʧ�ܣ�Я����Ϸ�Ҳ��㣡'
		RETURN 4
	END

	-- ��ѯ����
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- ��������
	IF @LockKindID IS NULL SET @LockKindID=0
	IF @LockServerID IS NULL SET @LockServerID=0

	-- �����ж�
	IF (@LockKindID<>0 and @LockKindID<>@wKindID) OR (@LockServerID<>0 and @LockServerID<>@wServerID)
	BEGIN

		-- ��������
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ��ѯ����
		IF @LockKindID<>0 AND @LockServerID<>0
		BEGIN
			-- ��ѯ��Ϣ
			DECLARE @KindName NVARCHAR(31)
			DECLARE @ServerName NVARCHAR(31)
			SELECT @KindName=KindName FROM  WHQJPlatformDB.dbo.GameKindItem WHERE KindID=@LockKindID
			SELECT @ServerName=ServerName FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

			-- ������Ϣ
			IF @KindName IS NULL SET @KindName=N'δ֪��Ϸ'
			IF @ServerName IS NULL SET @ServerName=N'δ֪����'
			SET @strErrorDescribe=N'������ [ '+@KindName+N' ] �� [ '+@ServerName+N' ] ��Ϸ�����У����ܽ��е�ǰ�����в�����'
			RETURN 4

		END
		ELSE
		BEGIN
			-- ��ʾ��Ϣ
			SELECT [ErrorDescribe]=N'������ʹ����վҳ��������д�������У����ܽ��е�ǰ�����в�����'
			SET @strErrorDescribe=N'������ʹ����վҳ��������д�������У����ܽ��е�ǰ�����в�����'
			RETURN 4
		END
	END

	-- �������
	SET @InsureRevenue=0
	SET @VariationScore=-@lSaveScore
	SET @VariationInsure=@lSaveScore

	-- ������Ϣ
	SET @strErrorDescribe=N'����ɹ���'

	-- ��������
	UPDATE GameScoreInfo SET Score=Score+@VariationScore, InsureScore=InsureScore+@VariationInsure, Revenue=Revenue+@InsureRevenue	
	WHERE UserID=@dwUserID

	-- ��¼��־
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@lSaveScore,@InsureRevenue,0,1,@strClientIP)		

	-- ��¼�����ˮ��¼
	DECLARE @DateTime DATETIME
	DECLARE @SerialNumber NVARCHAR(20)
	SET @DateTime = GETDATE()
	SELECT @SerialNumber=dbo.WF_GetSerialNumber()
	
	INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
	VALUES(@SerialNumber,0,@dwUserID,6,@SourceScore,@SourceInsure,@lSaveScore,@strClientIP,@DateTime)	
	
	
	-- ��������	
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ������
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, @VariationScore AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ȡ���
CREATE PROC GSP_GR_UserTakeScore
	@dwUserID INT,								-- �û� I D
	@lTakeScore BIGINT,							-- �����Ŀ
	@strPassword NCHAR(32),						-- �û�����
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��ұ���
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT
DECLARE @InsureRevenue BIGINT
DECLARE @VariationScore BIGINT
DECLARE @VariationInsure BIGINT

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT

	-- ϵͳ��ͣ
	SELECT @EnjoinInsure=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ����Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ�������Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END
 
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @InsurePass=InsurePass, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	-- �����ж�
	IF @InsurePass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����������벻��ȷ���������������֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- ����ж�
	DECLARE @BankPrerequisite AS INT
	SELECT @BankPrerequisite=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankPrerequisite'
	IF @BankPrerequisite IS NULL SET @BankPrerequisite=0
	IF @lTakeScore<@BankPrerequisite
	BEGIN
		SET @strErrorDescribe=N'ȡ��ʧ�ܣ�����ȡ���С�� '+dbo.GSF_GR_Switch_Num(@BankPrerequisite)+'��'
		RETURN 4
	END

	DECLARE @BankMax AS INT
	SELECT @BankMax=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankGameMax'
	IF @BankMax IS NULL SET @BankMax=0
	IF @lTakeScore>@BankMax
	BEGIN
		SET @strErrorDescribe=N'ȡ��ʧ�ܣ�����ȡ��ܴ��� '+dbo.GSF_GR_Switch_Num(@BankMax)+'��'
		RETURN 5
	END

	-- ���з������
	DECLARE @RevenueRate INT
	SELECT @RevenueRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTake'

	-- �����������
	IF @RevenueRate>300 SET @RevenueRate=300
	IF @RevenueRate IS NULL SET @RevenueRate=1

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��Ϸ��Ϣ
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ����ж�
	IF @SourceInsure IS NULL OR @SourceInsure<@lTakeScore
	BEGIN
		-- ��������
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'ȡ��ʧ�ܣ�������Ϸ�Ҳ��㣡'
		RETURN 4
	END

	-- ��ѯ����
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- �������
	SET @InsureRevenue=@lTakeScore*@RevenueRate/1000
	SET @VariationScore=@lTakeScore-@InsureRevenue
	SET @VariationInsure=-@lTakeScore

	-- ������Ϣ
	SET @strErrorDescribe=N'ȡ���ɹ���'

	-- ��������
	UPDATE GameScoreInfo SET Score=Score+@VariationScore, InsureScore=InsureScore+@VariationInsure, Revenue=Revenue+@InsureRevenue
	WHERE UserID=@dwUserID

	-- ��¼��־
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@lTakeScore,@InsureRevenue,0,2,@strClientIP)	

	--������ˮ��־
	DECLARE @DateTime DATETIME	
	DECLARE @SerialNumber NVARCHAR(20)
	SET @DateTime = GETDATE()
	IF @InsureRevenue>0 -- ȡ�������� ����ʱ ��������ˮ Ϊ ���з����
	BEGIN 
		SELECT @SerialNumber=dbo.WF_GetSerialNumber()	
		INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
		VALUES(@SerialNumber,0,@dwUserID,8,@SourceScore,@SourceInsure,-@InsureRevenue,@strClientIP,@DateTime)
		--SET @SourceInsure=@SourceInsure-@InsureRevenue
	END

	SELECT @SerialNumber=dbo.WF_GetSerialNumber()	
	INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
	VALUES(@SerialNumber,0,@dwUserID,7,@SourceScore,@SourceInsure,@VariationScore,@strClientIP,DATEADD(ms,10,@DateTime))	
	
	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ������
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, @VariationScore AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ת�˽��
CREATE PROC GSP_GR_UserTransferScore
	@dwUserID INT,								-- �û� I D	
	@lTransferScore BIGINT,						-- �����Ŀ
	@strPassword NCHAR(32),						-- ��������
	@dwGameID INT,								-- Game	ID
	@strTransRemark NVARCHAR(32),				-- ת�˱�ע	
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��ұ���
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

DECLARE @InsureRevenue BIGINT
DECLARE @VariationInsure BIGINT

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT
	DECLARE @EnjoinTransfer INT

	-- ϵͳ��ͣ
	SELECT @EnjoinInsure=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END	
	
	-- ת����ͣ
	DECLARE @TUserRight INT
	DECLARE @UserOpenTransfer INT
	SELECT @TUserRight=UserRight FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	SET @UserOpenTransfer = @TUserRight&0x00000040	
	IF @UserOpenTransfer=0
	BEGIN
		SELECT @EnjoinTransfer=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferStauts'
		IF @EnjoinTransfer IS NULL SET @EnjoinTransfer=0
		IF @EnjoinTransfer=1 
		BEGIN
			SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferStauts'
			RETURN 3
		END	
	END
	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ����Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ�������Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END

	-- ��������
	DECLARE @ConfigID INT
	DECLARE @ReturnType TINYINT
	DECLARE @ReturnPercent INT
	DECLARE @ReturnDescribe NVARCHAR(32)
	DECLARE @DatetimeNow DATETIME
	DECLARE @SrcMemberOverDate	DATETIME
	DECLARE @SrcMemberOrder		INT
	DECLARE @DstMemberOverDate	DATETIME
	DECLARE @DstMemberOrder		INT	
	
	
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MemberOrder AS TINYINT
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @InsurePass=InsurePass, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MemberOrder=MemberOrder, @MachineID=LastLogonMachine,
	@SrcMemberOrder=MemberOrder,@SrcMemberOverDate=MemberOverDate 
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID and IsAndroid=0


	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	-- �����ж�
	IF @InsurePass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����������벻��ȷ���������������֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- ����ж�
	DECLARE @TransferPrerequisite AS BIGINT
	SELECT @TransferPrerequisite=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPrerequisite'
	IF @TransferPrerequisite IS NULL SET @TransferPrerequisite=0
	IF @lTransferScore<@TransferPrerequisite
	BEGIN
		SET @strErrorDescribe=N'������ת�˵���Ϸ����Ŀ�������� '+dbo.GSF_GR_Switch_Num(@TransferPrerequisite)+'����Ϸ��ת��ʧ�ܣ�'
		RETURN 4
	END

	-- Ŀ���û�
	DECLARE @TargetUserID INT
	DECLARE @UserTransferRight AS INT
	DECLARE @UserRight AS INT
	SELECT @TargetUserID=UserID,@UserRight=UserRight,@DstMemberOverDate =MemberOverDate,@DstMemberOrder=MemberOrder FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE GameID=@dwGameID	
--	IF @cbByNickName=1
--		SELECT @TargetUserID=UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE NickName=@strNickName
--	ELSE
--		SELECT @TargetUserID=UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE GameID=@strNickName	

	-- ��ѯ�û�
	IF @TargetUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'����Ҫת�˵���ϷID:'+LTRIM(@dwGameID)+'�����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 5
	END

	-- ��ͬ�ж�
	IF @TargetUserID=@dwUserID
	BEGIN
		SET @strErrorDescribe=N'����ʹ���Լ����ʺ�Ϊ�Լ�ת����Ϸ�ң����֤���ٴγ��ԣ�'
		RETURN 6
	END
	
	
	-- ��ת��Ȩ��
	/* DECLARE @SystemTransferRight AS INT
	SELECT @SystemTransferRight=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPower'
	IF @SystemTransferRight IS NULL SET @SystemTransferRight=1
	IF @SystemTransferRight=0 
	BEGIN
		SET @UserTransferRight = 1
	END
	ELSE
	BEGIN
		SET @UserTransferRight = @UserRight&0x4000000
	END
	IF @UserTransferRight =0
	BEGIN
		SET @strErrorDescribe=N'����Ҫת�˵���ϷID��û��ת��Ȩ�ޣ����֤���ٴγ��ԣ�'
		RETURN 7	
	END */

	-- ��Ҳ�ѯ
	DECLARE @TargetScore BIGINT
	DECLARE @TargetInsure BIGINT
	SELECT @TargetScore=Score, @TargetInsure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@TargetUserID

	-- �����ж�
	IF @TargetScore IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID,LastLogonIP,RegisterIP) VALUES (@TargetUserID,@strClientIP,@strClientIP)

		-- ��Ϸ��Ϣ
		SELECT @TargetScore=Score, @TargetInsure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@TargetUserID
	END

	-- ���з������	
	DECLARE @MaxTax BIGINT
	DECLARE @RevenueRate INT
	IF @MemberOrder = 0
	BEGIN
		SELECT @MaxTax=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferMaxTax'
		SELECT @RevenueRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransfer'

		-- �����������
		IF @RevenueRate IS NULL SET @RevenueRate=1
	END 
	ELSE
	BEGIN
		SELECT @MaxTax=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferMaxTax'
		--SELECT @RevenueRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransferMember'		
			SELECT @RevenueRate=InsureRate FROM  WHQJAccountsDB.dbo.MemberProperty where @MemberOrder =MemberOrder
		-- �����������
		IF @RevenueRate IS NULL SET @RevenueRate=0
	END	
	
	-- �����������
	IF @RevenueRate>1000 SET @RevenueRate=1000	

	-- ���б���
	DECLARE @TransferRetention INT	-- ���ٱ���
	DECLARE @SurplusScore BIGINT	-- ת������
	SELECT @TransferRetention=ISNULL(StatusValue,0) FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferRetention'

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��Ϸ��Ϣ
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ����ж�
	IF @SourceInsure IS NULL OR @SourceInsure<@lTransferScore
	BEGIN
		-- ��������
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'����ǰ���е���Ϸ�����㣬��Ϸ��ת��ʧ�ܣ�'
		RETURN 4
	END

	-- ���б���
	IF @TransferRetention<>0
	BEGIN
		SET @SurplusScore=@SourceInsure-@lTransferScore
		IF @SurplusScore<@TransferRetention
		BEGIN
			-- ��������
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			SET @strErrorDescribe=N'�ǳ���Ǹ,ת�˺������е���������'+dbo.GSF_GR_Switch_Num(@TransferRetention)+'���'
			RETURN 7
		END
	END

	-- ��ѯ����
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- ������Ϣ
	SET @strErrorDescribe=N'ת�� '+dbo.GSF_GR_Switch_Num(@lTransferScore)+' ��Ϸ�ҵ� '+LTRIM(@dwGameID)+'�����в����ɹ�������������ʻ���Ϣ��'

	-- �������
	SET @InsureRevenue=@lTransferScore*@RevenueRate/1000
	SET @VariationInsure=-@lTransferScore

	-- ��������ⶥ
	IF @MaxTax<>0
	BEGIN
		IF @InsureRevenue > @MaxTax
			SET @InsureRevenue=@MaxTax
	END

	-- �۳����
	UPDATE GameScoreInfo SET InsureScore=InsureScore+@VariationInsure,Revenue=Revenue+@InsureRevenue WHERE UserID=@dwUserID

	-- ���ӽ��
	UPDATE GameScoreInfo SET InsureScore=InsureScore+@lTransferScore-@InsureRevenue WHERE UserID=@TargetUserID

	-- ��¼��־
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		TargetUserID,TargetGold,TargetBank,SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP,CollectNote)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@TargetUserID,@TargetScore,@TargetInsure,
		@lTransferScore,@InsureRevenue,0,3,@strClientIP,@strTransRemark)
		
	--������ˮ��־
	DECLARE @DateTime DATETIME	
	DECLARE @SerialNumber NVARCHAR(20)
	SET @DateTime = GETDATE()

	IF @InsureRevenue>0 --���з���ѣ���������������ˮ
	BEGIN
		SELECT @SerialNumber=dbo.WF_GetSerialNumber()
		INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
		VALUES(@SerialNumber,0,@dwUserID,8,@SourceScore,@SourceInsure,-@InsureRevenue,@strClientIP,@DateTime)	
		SET @SourceInsure = @SourceInsure - @InsureRevenue
	END
	-- ת������¼�����ˮ
	SELECT @SerialNumber=dbo.WF_GetSerialNumber()
	INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
	VALUES(@SerialNumber,0,@dwUserID,2,@SourceScore,@SourceInsure,-(@lTransferScore-@InsureRevenue),@strClientIP,DATEADD(ms,10,@DateTime))	
	-- ���շ���¼�����ˮ
	SELECT @SerialNumber=dbo.WF_GetSerialNumber()
	INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
	VALUES(@SerialNumber,0,@TargetUserID,3,@TargetScore,@TargetInsure,@lTransferScore-@InsureRevenue,@strClientIP,DATEADD(ms,20,@DateTime))	
	
	SET @SourceInsure = @SourceInsure + @InsureRevenue

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ������
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, 0 AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GR_QueryUserInsureInfo
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @ServerID SMALLINT
DECLARE @UserRight INT

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @LogonPass AS NCHAR(32)
	SELECT @LogonPass=LogonPass FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�������в�ѯ���벻��ȷ��������Ϣ��ѯʧ�ܣ�'
		RETURN 1
	END

	-- ����˰��
	DECLARE @RevenueRateTake AS INT
	DECLARE @RevenueRateTransfer AS INT
	DECLARE @RevenueRateTransferMember AS INT
	DECLARE @TransferPrerequisite AS BIGINT  
	DECLARE	@EnjoinTransfer AS TINYINT
	
	DECLARE @BankOPButton1 BIGINT  
	DECLARE @BankOPButton2 BIGINT  
	DECLARE @BankOPButton3 BIGINT  

	SELECT @RevenueRateTake=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTake'
	SELECT @RevenueRateTransfer=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransfer'
	SELECT @RevenueRateTransferMember=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransferMember'
	SELECT @TransferPrerequisite=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPrerequisite'
	SELECT @EnjoinTransfer=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferStauts'

	SELECT @BankOPButton1=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankOPButton1'
	SELECT @BankOPButton2=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankOPButton2'
	SELECT @BankOPButton3=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankOPButton3'

	-- ��������
	IF @EnjoinTransfer IS NULL SET @EnjoinTransfer=0
	IF @RevenueRateTake IS NULL SET @RevenueRateTake=1
	IF @RevenueRateTransfer IS NULL SET @RevenueRateTransfer=1	
	IF @TransferPrerequisite IS NULL SET @TransferPrerequisite=0	
	IF @RevenueRateTransferMember IS NULL SET @RevenueRateTransferMember=0

	IF @BankOPButton1 IS NULL SET @BankOPButton1=100
	IF @BankOPButton2 IS NULL SET @BankOPButton2=1000
	IF @BankOPButton3 IS NULL SET @BankOPButton3=10000
	
	-- ��ѯ����
	SELECT @ServerID=ServerID FROM GameScoreLocker(NOLOCK) WHERE UserID=@dwUserID

	-- ��ѯ����
	SELECT @Score=Score, @Insure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	--��ѯȨ��
	SELECT @UserRight=UserRight FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @ServerID IS NULL SET @ServerID=0

	-- ������
	SELECT @dwUserID AS UserID, @Score AS Score, @Insure AS Insure, @ServerID AS ServerID, @RevenueRateTake AS RevenueTake, 
		   @RevenueRateTransfer AS RevenueTransfer, @RevenueRateTransferMember AS RevenueTransferMember, @TransferPrerequisite AS TransferPrerequisite,
		   @EnjoinTransfer AS EnjoinTransfer,@UserRight AS UserRight, @BankOPButton1 AS BankOPButton1, @BankOPButton2 AS BankOPButton2, @BankOPButton3 AS BankOPButton3
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GR_QueryTransferRebate
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32)						-- �û�����

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	DECLARE @cbRebateEnabled AS SMALLINT
	DECLARE @lIngot AS INT
	DECLARE @lLoveLiness AS INT
 
	SELECT @cbRebateEnabled=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferRebate'
	IF @cbRebateEnabled IS NULL SET @cbRebateEnabled=0	
		
	-- ������
	SELECT @dwUserID AS UserID, @cbRebateEnabled AS RebateEnabled 

END

RETURN 0

GO

-- ��ѯ����
CREATE PROC GSP_GR_GetAllKindMoney
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	DECLARE @MiddleMoney AS BIGINT
	DECLARE @Type AS TINYINT
	DECLARE @lToScoreRatio AS INT
	DECLARE @lToDiamondRatio AS INT
 	DECLARE @lToBeanRatio AS INT

	DECLARE @lInsure BIGINT
	DECLARE @lDiamond BIGINT

	SET @MiddleMoney = 0
	SET @Type = 0
	SET @lToScoreRatio = 0
	SET @lToDiamondRatio = 0
	SET @lToBeanRatio = 0

	--����ת��ұ任����
	SELECT @lToScoreRatio=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MiddleToScore'
	IF @lToScoreRatio IS NULL SET @lToScoreRatio=0

	--����ת��ʯ�任����
	SELECT @lToDiamondRatio=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MiddleToDiamond'
	IF @lToDiamondRatio IS NULL SET @lToDiamondRatio=0


	-- ͳ�ƽ��
	SELECT @MiddleMoney = SUM(MiddleMoney) FROM MiddleMoney  WHERE UserID = @dwUserID AND MoneyType = 0
	UPDATE GameScoreInfo SET InsureScore = InsureScore +@MiddleMoney * @lToScoreRatio WHERE UserID = @dwUserID
	SELECT @lInsure = InsureScore FROM GameScoreInfo WHERE UserID = @dwUserID

	-- ͳ����ʯ
	SET @MiddleMoney = 0;
	SELECT @MiddleMoney = SUM(MiddleMoney) FROM MiddleMoney  WHERE UserID = @dwUserID AND MoneyType = 1
	UPDATE UserCurrency SET Diamond = Diamond +@MiddleMoney * @lToDiamondRatio WHERE UserID = @dwUserID
	SELECT @lDiamond = Diamond FROM UserCurrency WHERE UserID = @dwUserID

	IF @lInsure IS NULL SET  @lInsure = 0
	IF @lDiamond IS NULL SET @lDiamond = 0

	DELETE FROM MiddleMoney WHERE UserID = @dwUserID
		
	-- ������
	SELECT @lInsure AS Insure, @lDiamond AS Diamond 

END

RETURN 0

GO

-- ��ѯ����
CREATE PROC GSP_GR_GetUserWealth
	@dwUserID INT,								-- �û� I D
	@strDynamicPassword NCHAR(32),				-- �û�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @lScore BIGINT
	DECLARE @lInsure BIGINT
	DECLARE @lDiamond BIGINT
	DECLARE @lAwardTicket INT

	-- Я�������� ���
	SELECT @lScore = Score, @lInsure = InsureScore FROM GameScoreInfo WHERE UserID = @dwUserID

	-- שʯ�뽱ȯ
	SELECT @lDiamond = Diamond, @lAwardTicket = AwardTicket FROM UserCurrency WHERE UserID = @dwUserID

	IF @lScore IS NULL SET  @lScore = 0
	IF @lInsure IS NULL SET @lInsure = 0
	IF @lDiamond IS NULL SET @lDiamond = 0
	IF @lAwardTicket IS NULL SET @lAwardTicket = 0

	-- ������
	SELECT @lScore AS Score, @lInsure AS Insure, @lDiamond AS Diamond , @lAwardTicket AS AwardTicket

END

RETURN 0

GO


Create function GSF_GR_Switch_Num(@lnum BIGINT)
returns nvarchar(100)
as
BEGIN
	DECLARE @szReS nvarchar(100)
	SET @szReS=N''
	DECLARE @lTemp BIGINT
	DECLARE @cbFlag TINYINT
	SET @cbFlag=0
	IF(@lnum<0)
	BEGIN
		Set @lnum=-1*@lnum
		SET @cbFlag=1;
	END
	IF(@lnum>=100000000)
	BEGIN
		SET @lTemp=@lnum/100000000
		SET @szReS=LTRIM(RTRIM(STR(@lTemp)))+N'��'
		SET @lnum=@lnum-@lTemp*100000000
	END
	IF(@lnum>=10000)
	BEGIN
		SET @lTemp=@lnum/10000
		SET @szReS=@szReS+LTRIM(RTRIM(STR(@lTemp)))+N'��'
		SET @lnum=@lnum-@lTemp*10000
	END
	IF(@lnum>0)
	BEGIN
		SET @szReS=@szReS+LTRIM(RTRIM(STR(@lnum)))
	END

	IF(@cbFlag=1)
		SET @szReS=N'-'+@szReS
	RETURN @szRes
END


