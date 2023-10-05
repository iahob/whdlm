
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyUserID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyUserID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyMobile]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyMobile]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_GetOfflineInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_GetOfflineInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyAI]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyAI]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- I D ��¼
CREATE PROC GSP_GR_EfficacyUserID
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

-- ������Ϣ
DECLARE @QQ NVARCHAR(16)
DECLARE @EMail NVARCHAR(16)
DECLARE @SeatPhone NVARCHAR(32)
DECLARE @MobilePhone NVARCHAR(16)
DECLARE @DwellingPlace NVARCHAR(128)
DECLARE @PostalCode NVARCHAR(8)
DECLARE @DayPlayTime INT
DECLARE @DayWinLostScore BIGINT

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @GroupID INT
DECLARE @UserRight INT
DECLARE @Gender TINYINT
DECLARE @MasterRight INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberOrder SMALLINT
DECLARE @GroupName NVARCHAR(31)

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Grade BIGINT
DECLARE @Insure BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @AgentID AS INT
DECLARE @IntegralCount BIGINT 
-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @EnjoinLogon INT

	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ����Ϸ��¼Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ�������Ϸ��¼Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END
 
	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @DynamicPass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE @SpreaderID INT
	SET @AgentID = 0	
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName,@SpreaderID = SpreaderID, @UnderWrite=UnderWrite, @LogonPass=DynamicPass,@FaceID=FaceID, @CustomID=CustomID,
		@Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@UserRight=UserRight,
		@MasterRight=MasterRight, @MasterOrder=MasterOrder, @MemberOrder=MemberOrder, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,@AgentID=AgentID
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	--��ѯ����
	SELECT @AgentID = UserID FROM  WHQJAccountsDB.dbo.AccountsAgent  where UserID = @SpreaderID

	-- ������Ϣ
    SET @QQ =N''
    SET @EMail =N''
    SET @SeatPhone =N''
    SET @MobilePhone =N''
    SET @DwellingPlace =N''
    SET @PostalCode =N''
    SELECT @QQ = QQ,@EMail = EMail,@SeatPhone = SeatPhone,@MobilePhone = MobilePhone,@DwellingPlace = DwellingPlace ,@PostalCode =PostalCode 
    FROM  WHQJAccountsDB.dbo.IndividualDatum WHERE UserID=@dwUserID
    
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
			RETURN 2
		END
	END

	-- �����ж�
	IF @LogonPass IS NULL OR @strPassword IS NULL OR @LogonPass<>@strPassword AND @strClientIP<>N'0.0.0.0'
	BEGIN
		SET @strErrorDescribe=N'��Ǹ,���������ʺ��������ط���¼,�����µ�¼ƽ̨���ٴγ��ԣ�'
		RETURN 3
	END

	-- �̶�����
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SET @strErrorDescribe=N'�����ʺųɹ�ʹ���˹̶�������¼���ܣ�'
		UPDATE  WHQJAccountsDB.dbo.AccountsInfo SET MoorMachine=@MoorMachine, LastLogonMachine=@strMachineID WHERE UserID=@UserID
	END

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��Ϸ��Ϣ
	DECLARE @AllLogonTimes INT
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT
	SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount,
		@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder, @AllLogonTimes=AllLogonTimes
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��Ϣ�ж�
	IF @Score IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @strClientIP, @strMachineID, @strClientIP, @strMachineID)

		-- ��Ϸ��Ϣ
		SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount,
			@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterOrder=MasterOrder, @GameMasterRight=MasterRight, @AllLogonTimes=AllLogonTimes
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- �쳣�ж�
	IF @Score<0
	BEGIN
		-- ��������
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'��Ǹ��֪ͨ�㣬������Ϸ�����ݳ������쳣���������ϵ�ͻ����������˽���ϸ�����'
		RETURN 14
	END

	-- ��ѯ����
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		-- �������
		DELETE GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID

		-- ��ѯ����
		DECLARE @LockKindID INT
		DECLARE @LockServerID INT
		SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

		-- �����ж�
		IF @LockKindID IS NOT NULL AND @LockServerID IS NOT NULL
		BEGIN

			-- ��������
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			-- ��ѯ����
			IF @LockKindID<>0
			BEGIN
				-- ��ѯ��Ϣ
				DECLARE @KindName NVARCHAR(31)
				DECLARE @ServerName NVARCHAR(31)
				SELECT @KindName=KindName FROM  WHQJPlatformDB.dbo.GameKindItem WHERE KindID=@LockKindID
				SELECT @ServerName=ServerName FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

				-- ������Ϣ
				IF @KindName IS NULL SET @KindName=N'δ֪��Ϸ'
				IF @ServerName IS NULL SET @ServerName=N'δ֪����'
				SET @strErrorDescribe=N'������ [ '+@KindName+N' ] �� [ '+@ServerName+N' ] ��Ϸ�����У�����ͬʱ�ٽ������Ϸ���䣡'
				RETURN 4

			END
			ELSE
			BEGIN
				-- ��ʾ��Ϣ
				SELECT [ErrorDescribe]=N'�����ڽ��б��չ�������У���ʱ�޷��������Ϸ���䣡'
				RETURN 4
			END
		END
	END

	-- ��������
	SET @Grade=0
	SET @GroupID=0
	SET @GroupName=''

	-- Ȩ�ޱ�־
	SET @UserRight=@UserRight|@GameUserRight
	SET @UserRight=@UserRight&~256
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- Ȩ�޵ȼ�
	IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder

	-- �����¼
	INSERT RecordUserInout (UserID, EnterScore, EnterGrade, EnterInsure, KindID, ServerID, EnterClientIP, EnterMachine)
	VALUES (@UserID, @Score, @Grade, @Insure,  @wKindID, @wServerID, @strClientIP, @strMachineID)

	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- ��������
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		-- ��������
		INSERT GameScoreLocker (UserID, ServerID, KindID, EnterID, EnterIP, EnterMachine) VALUES (@dwUserID, @wServerID, @wKindID, @InoutIndex, @strClientIP, @strMachineID)
		IF @@ERROR<>0
		BEGIN
			-- ��������
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			-- ������Ϣ
			SET @strErrorDescribe=N'��Ǹ��֪ͨ�㣬��Ϸ����������ʧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
			RETURN 14
		END
	END

	--������Ϣ
	UPDATE GameScoreInfo SET AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), 
		LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID WHERE UserID=@dwUserID

	-- ��¼ͳ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- �����¼
	IF @AllLogonTimes>0
	BEGIN
		UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END
	ELSE
	BEGIN
		UPDATE SystemStreamInfo SET RegisterCount=RegisterCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, RegisterCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END
		
	--��ѯÿ��
	SELECT @DayPlayTime=DayPlayTimeCount FROM WHQJAccountsDB.dbo.AccountsDayPlayTime WHERE UserID=@dwUserID AND DateID=@DateID
	IF @DayPlayTime IS NULL SET @DayPlayTime=0
	
	SELECT @DayWinLostScore =Score FROM StreamScoreInfo WHERE   DateID=@DateID AND UserID = @dwUserID 
	IF @DayWinLostScore IS NULL SET @DayWinLostScore=0

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �������
	SET @IntegralCount = 0
	SELECT  @IntegralCount=IntegralCount FROM GameScoreAttribute WHERE UserID=@dwUserID And ServerID=@wServerID And KindID = @wKindID
	    
	--����Ϸ��ʷ����Ӯ
	DECLARE @HistoryKindIDWin BIGINT
	SELECT @HistoryKindIDWin=SUM(Score) FROM WHQJTreasureDB.dbo.RecordDrawScore WHERE UserID=@UserID AND DrawID IN (SELECT DrawID FROM WHQJTreasureDB.dbo.RecordDrawInfo)
	IF @HistoryKindIDWin IS NULL SET @HistoryKindIDWin=0
	
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @GroupID AS GroupID, @NickName AS NickName, @UnderWrite AS UnderWrite,  @FaceID AS FaceID, 0 AS Dimand,
		@CustomID AS CustomID, @Gender AS Gender, @GroupName AS GroupName, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight,
		@MasterOrder AS MasterOrder, @MemberOrder AS MemberOrder, @Score AS Score, @InoutIndex AS InoutIndex,
		@Insure AS Insure,  @WinCount AS WinCount,  @LostCount AS LostCount,@DrawCount AS DrawCount, @FleeCount AS FleeCount, 		
		@IntegralCount AS IntegralCount,@AgentID AS AgentID,@QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone, @MobilePhone AS MobilePhone,
		@DwellingPlace AS DwellingPlace,@PostalCode AS PostalCode,0 AS GroupIDArray,@HistoryKindIDWin AS HistoryKindIDWin,@DayPlayTime AS DayPlayTime,@DayWinLostScore AS DayWinLostScore

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �ֻ���¼
CREATE PROC GSP_GR_EfficacyMobile
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@cbPersonalServer SMALLINT,					-- ˽�˷���
	@dwGroupID	INT,							--������ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

-- ������Ϣ
DECLARE @QQ NVARCHAR(16)
DECLARE @EMail NVARCHAR(16)
DECLARE @SeatPhone NVARCHAR(32)
DECLARE @MobilePhone NVARCHAR(16)
DECLARE @DwellingPlace NVARCHAR(128)
DECLARE @PostalCode NVARCHAR(8)
DECLARE @DayPlayTime INT
DECLARE @DayWinLostScore BIGINT

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @UserRight INT
DECLARE @Gender TINYINT
DECLARE @MasterRight INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberOrder SMALLINT
DECLARE @GroupName NVARCHAR(31)

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Dimand BIGINT
DECLARE @Grade BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @AgentID AS INT
DECLARE @IntegralCount BIGINT 

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @EnjoinLogon BIGINT

	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END	

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ����Ϸ��¼Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE @SpreaderID INT
	SET @AgentID = 0
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName ,@SpreaderID = SpreaderID, @UnderWrite=UnderWrite, @LogonPass=DynamicPass, @FaceID=FaceID, @CustomID=CustomID,
		@Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@UserRight=UserRight,
		@MasterRight=MasterRight, @MasterOrder=MasterOrder, @MemberOrder=MemberOrder, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,@AgentID=AgentID
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	--��ѯ����
	SELECT @AgentID = UserID FROM  WHQJAccountsDB.dbo.AccountsAgent  where UserID = @SpreaderID
	
	-- ������Ϣ
    SET @QQ =N''
    SET @EMail =N''
    SET @SeatPhone =N''
    SET @MobilePhone =N''
    SET @DwellingPlace =N''
    SET @PostalCode =N''
    SELECT @QQ = QQ,@EMail = EMail,@SeatPhone = SeatPhone,@MobilePhone = MobilePhone,@DwellingPlace = DwellingPlace ,@PostalCode =PostalCode 
    FROM  WHQJAccountsDB.dbo.IndividualDatum WHERE UserID=@dwUserID
    
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
			RETURN 2
		END
	END

	-- �����ж�
	IF @LogonPass<>@strPassword AND @strClientIP<>N'0.0.0.0'
	BEGIN
		SET @strErrorDescribe=N'��Ǹ,���������ʺ��������ط���¼,�����µ�¼ƽ̨���ٴγ��ԣ�'
		RETURN 3
	END

	-- ��ʼ����
--	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
--	BEGIN TRAN

	-- ��Ϸ��Ϣ
	DECLARE @AllLogonTimes INT
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT
	SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount,
		@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder, @AllLogonTimes=AllLogonTimes
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��Ϣ�ж�
	IF @Score IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @strClientIP, @strMachineID, @strClientIP, @strMachineID)

		-- ��Ϸ��Ϣ
		SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount,@DrawCount=DrawCount, @DrawCount=DrawCount,
			@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterOrder=MasterOrder, @GameMasterRight=MasterRight, @AllLogonTimes=AllLogonTimes
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- �쳣�ж�
	IF @Score<0
	BEGIN
		-- ��������
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'��Ǹ��֪ͨ�㣬������Ϸ�����ݳ������쳣���������ϵ�ͻ����������˽���ϸ�����'
		RETURN 14
	END

	-- �������
	DELETE GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID

	-- ��ѯ����
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- �����ж�
	IF @LockKindID IS NOT NULL AND @LockServerID IS NOT NULL
	BEGIN

		-- ��������
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ��ѯ����
		IF @LockKindID<>0
		BEGIN
			-- ��ѯ��Ϣ
			DECLARE @KindName NVARCHAR(31)
			DECLARE @ServerName NVARCHAR(31)
			SELECT @KindName=KindName FROM  WHQJPlatformDB.dbo.GameKindItem WHERE KindID=@LockKindID
			SELECT @ServerName=ServerName FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

			-- ������Ϣ
			IF @KindName IS NULL SET @KindName=N'δ֪��Ϸ'
			IF @ServerName IS NULL SET @ServerName=N'δ֪����'
			SET @strErrorDescribe=N'�����ڱ�ķ���Ծ�Ŷ���Ƿ��ȥ��'
			SELECT @LockServerID AS ServerID, @LockKindID AS KindID
			RETURN 100

		END
		ELSE
		BEGIN
			-- ��ʾ��Ϣ
			SELECT [ErrorDescribe]=N'�����ڽ��б��չ�������У���ʱ�޷��������Ϸ���䣡'
			RETURN 4
		END
	END

	-- ��������
	SET @Grade=0
	SET @GroupName=''

	-- Ȩ�ޱ�־
	SET @UserRight=@UserRight|@GameUserRight
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- Ȩ�޵ȼ�
	IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder
	-- ��ѯ��ʯ
	SELECT @Dimand=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID		
	IF @Dimand IS NULL SET @Dimand=0	
	-- �����¼
	INSERT RecordUserInout (UserID, EnterScore, EnterGrade, EnterInsure,  KindID, ServerID, EnterClientIP, EnterMachine)
	VALUES (@UserID, @Score, @Grade, @Insure, @wKindID, @wServerID, @strClientIP, @strMachineID)

	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- ��������
	INSERT GameScoreLocker (UserID, ServerID, KindID, EnterID, EnterIP, EnterMachine) VALUES (@dwUserID, @wServerID, @wKindID, @InoutIndex, @strClientIP, @strMachineID)
	IF @@ERROR<>0
	BEGIN
		-- ��������
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'��Ǹ��֪ͨ�㣬��Ϸ����������ʧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 14
	END

	--������Ϣ
	UPDATE GameScoreInfo SET AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), 
		LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID WHERE UserID=@dwUserID

	-- ��¼ͳ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- �����¼
	IF @AllLogonTimes>0
	BEGIN
		UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END
	ELSE
	BEGIN
		UPDATE SystemStreamInfo SET RegisterCount=RegisterCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, RegisterCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END	

	-- ���ֲ���Ϣ
	DECLARE @GroupIDArray nvarchar(512)
	SELECT @GroupIDArray=GroupIDArray FROM   WHQJGroupDB.dbo.IMUserGroupInfo WHERE UserID=@dwUserID
	IF @GroupIDArray IS NULL SET @GroupIDArray=';'

	--��ѯÿ��
	SELECT @DayPlayTime=DayPlayTimeCount FROM WHQJAccountsDB.dbo.AccountsDayPlayTime WHERE UserID=@dwUserID AND DateID=@DateID
	IF @DayPlayTime IS NULL SET @DayPlayTime=0
	
	SELECT @DayWinLostScore =Score FROM StreamScoreInfo WHERE   DateID=@DateID AND UserID = @dwUserID 
	IF @DayWinLostScore IS NULL SET @DayWinLostScore=0
	DECLARE @lGroupScore BIGINT
	SET @lGroupScore=0

	IF @dwGroupID<>0
	BEGIN
		SELECT @lGroupScore=Score FROM   WHQJGroupDB.dbo.IMGroupMember WHERE UserID=@dwUserID AND GroupID=@dwGroupID

		IF @lGroupScore IS NULL
		BEGIN
			SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵ĳ�Ա��'
			RETURN 15
		END
		
		DECLARE @cbGroupStatus TINYINT
		SELECT @cbGroupStatus=GroupStatus FROM   WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID=@dwGroupID
		IF @cbGroupStatus <> 1
		BEGIN
			SET @strErrorDescribe=N'��Ǹ���ô�������ʱ�޷�ʹ�ã�'
			RETURN 16
		END


		
		IF EXISTS (SELECT 1 FROM   WHQJGroupDB.dbo.IMGroupApplyRecord WHERE GroupID=@dwGroupID AND ApplyerID=@dwUserID AND ApplyType=1 AND ApplyStatus=0)
		BEGIN
			SET @strErrorDescribe=N'���������˳��ô����ˣ���ʱ�޷���Ϸ��'
			RETURN 17
		END

		--���´������볡��
		UPDATE RecordUserInout SET EnterScore = @lGroupScore, GroupID=@dwGroupID WHERE ID=@InoutIndex AND UserID=@dwUserID
	END

	-- ��������
--	COMMIT TRAN
--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �������
	SET @IntegralCount = 0
	SELECT  @IntegralCount=IntegralCount FROM GameScoreAttribute WHERE UserID=@dwUserID And ServerID=@wServerID And KindID = @wKindID
	
	--����Ϸ��ʷ����Ӯ
	DECLARE @HistoryKindIDWin BIGINT
	SELECT @HistoryKindIDWin=SUM(Score) FROM WHQJTreasureDB.dbo.RecordDrawScore WHERE UserID=@UserID AND DrawID IN (SELECT DrawID FROM WHQJTreasureDB.dbo.RecordDrawInfo)
	IF @HistoryKindIDWin IS NULL SET @HistoryKindIDWin=0
	
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @dwGroupID AS GroupID, @NickName AS NickName, @UnderWrite AS UnderWrite,  @FaceID AS FaceID, @Dimand AS Dimand,
		@CustomID AS CustomID, @Gender AS Gender, @GroupName AS GroupName, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight,
		@MasterOrder AS MasterOrder, @MemberOrder AS MemberOrder, @Score AS Score, @InoutIndex AS InoutIndex,
		@Insure AS Insure,  @WinCount AS WinCount,  @LostCount AS LostCount,@DrawCount AS DrawCount, @FleeCount AS FleeCount, 		
		@IntegralCount AS IntegralCount,@AgentID AS AgentID,@QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone, @MobilePhone AS MobilePhone,
		@DwellingPlace AS DwellingPlace,@PostalCode AS PostalCode, @GroupIDArray AS GroupIDArray, @lGroupScore AS GroupScore,@HistoryKindIDWin AS HistoryKindIDWin,@DayPlayTime AS DayPlayTime,@DayWinLostScore AS DayWinLostScore


END

RETURN 0

GO

---------------------------------------------------------------------------------------------------------------
-- �ֻ���¼
CREATE PROC GSP_GR_GetOfflineInfo
	@dwUserID INT								-- �û� I D
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON


-- ִ���߼�
BEGIN

	
	-- ��ѯ����
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- �жϻ�������
	IF @LockKindID IS NULL OR @LockServerID IS NULL
	BEGIN
		SELECT @LockKindID=KindID, @LockServerID=ServerID FROM  WHQJGameScoreDB.dbo.GameScoreLocker WHERE UserID=@dwUserID

		-- �жϱ�������
		IF @LockKindID IS NULL OR @LockServerID IS NULL
		BEGIN
			SELECT @LockKindID=KindID, @LockServerID=ServerID FROM   WHQJGameMatchDB.dbo.GameScoreLocker WHERE UserID=@dwUserID
		END
	END

	-- �����ж�
	IF @LockKindID IS NULL SET @LockKindID = 0
	IF @LockServerID IS NULL SET @LockServerID = 0
	
	-- �������
	SELECT @LockKindID AS LockKindID, @LockServerID AS ServerID


END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- �ֻ���¼
CREATE PROC GSP_GR_EfficacyAI
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@dwGroupID	INT,							--������ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

-- ������Ϣ
DECLARE @QQ NVARCHAR(16)
DECLARE @EMail NVARCHAR(16)
DECLARE @SeatPhone NVARCHAR(32)
DECLARE @MobilePhone NVARCHAR(16)
DECLARE @DwellingPlace NVARCHAR(128)
DECLARE @PostalCode NVARCHAR(8)
DECLARE @DayPlayTime INT
DECLARE @DayWinLostScore BIGINT

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @UserRight INT
DECLARE @Gender TINYINT
DECLARE @MasterRight INT
DECLARE @MasterOrder SMALLINT
DECLARE @MemberOrder SMALLINT
DECLARE @GroupName NVARCHAR(31)

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Dimand BIGINT
DECLARE @Grade BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @AgentID AS INT
DECLARE @IntegralCount BIGINT 

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @EnjoinLogon BIGINT

	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END	

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ����Ϸ��¼Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE @SpreaderID INT
	SET @AgentID = 0
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName ,@SpreaderID = SpreaderID, @UnderWrite=UnderWrite, @LogonPass=DynamicPass, @FaceID=FaceID, @CustomID=CustomID,
		@Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@UserRight=UserRight,
		@MasterRight=MasterRight, @MasterOrder=MasterOrder, @MemberOrder=MemberOrder, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,@AgentID=AgentID
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	--��ѯ����
	SELECT @AgentID = UserID FROM  WHQJAccountsDB.dbo.AccountsAgent  where UserID = @SpreaderID
	
	-- ������Ϣ
    SET @QQ =N''
    SET @EMail =N''
    SET @SeatPhone =N''
    SET @MobilePhone =N''
    SET @DwellingPlace =N''
    SET @PostalCode =N''
    SELECT @QQ = QQ,@EMail = EMail,@SeatPhone = SeatPhone,@MobilePhone = MobilePhone,@DwellingPlace = DwellingPlace ,@PostalCode =PostalCode 
    FROM  WHQJAccountsDB.dbo.IndividualDatum WHERE UserID=@dwUserID
    
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
			RETURN 2
		END
	END

	-- �����ж�
	IF @LogonPass<>@strPassword AND @strClientIP<>N'0.0.0.0'
	BEGIN
		SET @strErrorDescribe=N'��Ǹ,���������ʺ��������ط���¼,�����µ�¼ƽ̨���ٴγ��ԣ�'
		RETURN 3
	END

	-- ��ʼ����
--	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
--	BEGIN TRAN

	-- ��Ϸ��Ϣ
	DECLARE @AllLogonTimes INT
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT
	SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount,
		@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder, @AllLogonTimes=AllLogonTimes
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��Ϣ�ж�
	IF @Score IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @strClientIP, @strMachineID, @strClientIP, @strMachineID)

		-- ��Ϸ��Ϣ
		SELECT @Score=Score, @Insure=InsureScore, @WinCount=WinCount, @LostCount=LostCount,@DrawCount=DrawCount, @DrawCount=DrawCount,
			@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterOrder=MasterOrder, @GameMasterRight=MasterRight, @AllLogonTimes=AllLogonTimes
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- �쳣�ж�
	IF @Score<0
	BEGIN
		-- ��������
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'��Ǹ��֪ͨ�㣬������Ϸ�����ݳ������쳣���������ϵ�ͻ����������˽���ϸ�����'
		RETURN 14
	END

	-- �������
	DELETE GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID

	-- ��ѯ����
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- �����ж�
	IF @LockKindID IS NOT NULL AND @LockServerID IS NOT NULL
	BEGIN

		-- ��������
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ��ѯ����
		IF @LockKindID<>0
		BEGIN
			-- ��ѯ��Ϣ
			DECLARE @KindName NVARCHAR(31)
			DECLARE @ServerName NVARCHAR(31)
			SELECT @KindName=KindName FROM  WHQJPlatformDB.dbo.GameKindItem WHERE KindID=@LockKindID
			SELECT @ServerName=ServerName FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

			-- ������Ϣ
			IF @KindName IS NULL SET @KindName=N'δ֪��Ϸ'
			IF @ServerName IS NULL SET @ServerName=N'δ֪����'
			SET @strErrorDescribe=N'�����ڱ�ķ���Ծ�Ŷ���Ƿ��ȥ��'
			SELECT @LockServerID AS ServerID, @LockKindID AS KindID
			RETURN 100

		END
		ELSE
		BEGIN
			-- ��ʾ��Ϣ
			SELECT [ErrorDescribe]=N'�����ڽ��б��չ�������У���ʱ�޷��������Ϸ���䣡'
			RETURN 4
		END
	END

	-- ��������
	SET @Grade=0
	SET @GroupName=''

	-- Ȩ�ޱ�־
	SET @UserRight=@UserRight|@GameUserRight
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- Ȩ�޵ȼ�
	IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder
	-- ��ѯ��ʯ
	SELECT @Dimand=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID		
	IF @Dimand IS NULL SET @Dimand=0	
	-- �����¼
	INSERT RecordUserInout (UserID, EnterScore, EnterGrade, EnterInsure,  KindID, ServerID, EnterClientIP, EnterMachine)
	VALUES (@UserID, @Score, @Grade, @Insure, @wKindID, @wServerID, @strClientIP, @strMachineID)

	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- ��������
	INSERT GameScoreLocker (UserID, ServerID, KindID, EnterID, EnterIP, EnterMachine) VALUES (@dwUserID, @wServerID, @wKindID, @InoutIndex, @strClientIP, @strMachineID)
	IF @@ERROR<>0
	BEGIN
		-- ��������
--		ROLLBACK TRAN
--		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'��Ǹ��֪ͨ�㣬��Ϸ����������ʧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 14
	END

	--������Ϣ
	UPDATE GameScoreInfo SET AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), 
		LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID WHERE UserID=@dwUserID

	-- ��¼ͳ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- �����¼
	IF @AllLogonTimes>0
	BEGIN
		UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END
	ELSE
	BEGIN
		UPDATE SystemStreamInfo SET RegisterCount=RegisterCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, RegisterCount) VALUES (@DateID, @wKindID, @wServerID, 1)
	END	

	-- ���ֲ���Ϣ
	DECLARE @GroupIDArray nvarchar(512)
	SELECT @GroupIDArray=GroupIDArray FROM   WHQJGroupDB.dbo.IMUserGroupInfo WHERE UserID=@dwUserID
	IF @GroupIDArray IS NULL SET @GroupIDArray=';'

	--��ѯÿ��
	SELECT @DayPlayTime=DayPlayTimeCount FROM WHQJAccountsDB.dbo.AccountsDayPlayTime WHERE UserID=@dwUserID AND DateID=@DateID
	IF @DayPlayTime IS NULL SET @DayPlayTime=0
	
	SELECT @DayWinLostScore =Score FROM StreamScoreInfo WHERE   DateID=@DateID AND UserID = @dwUserID 
	IF @DayWinLostScore IS NULL SET @DayWinLostScore=0
	DECLARE @lGroupScore BIGINT
	SET @lGroupScore=0

	IF @dwGroupID<>0
	BEGIN
		SELECT @lGroupScore=Score FROM   WHQJGroupDB.dbo.IMGroupMember WHERE UserID=@dwUserID AND GroupID=@dwGroupID

		IF @lGroupScore IS NULL
		BEGIN
			SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵ĳ�Ա��'
			RETURN 15
		END
		
		DECLARE @cbGroupStatus TINYINT
		SELECT @cbGroupStatus=GroupStatus FROM   WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID=@dwGroupID
		IF @cbGroupStatus <> 1
		BEGIN
			SET @strErrorDescribe=N'��Ǹ���ô�������ʱ�޷�ʹ�ã�'
			RETURN 16
		END


		
		IF EXISTS (SELECT 1 FROM   WHQJGroupDB.dbo.IMGroupApplyRecord WHERE GroupID=@dwGroupID AND ApplyerID=@dwUserID AND ApplyType=1 AND ApplyStatus=0)
		BEGIN
			SET @strErrorDescribe=N'���������˳��ô����ˣ���ʱ�޷���Ϸ��'
			RETURN 17
		END

		--���´������볡��
		UPDATE RecordUserInout SET EnterScore = @lGroupScore, GroupID=@dwGroupID WHERE ID=@InoutIndex AND UserID=@dwUserID
	END

	-- ��������
--	COMMIT TRAN
--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- �������
	SET @IntegralCount = 0
	SELECT  @IntegralCount=IntegralCount FROM GameScoreAttribute WHERE UserID=@dwUserID And ServerID=@wServerID And KindID = @wKindID
	
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @dwGroupID AS GroupID, @NickName AS NickName, @UnderWrite AS UnderWrite,  @FaceID AS FaceID, @Dimand AS Dimand,
		@CustomID AS CustomID, @Gender AS Gender, @GroupName AS GroupName, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight,
		@MasterOrder AS MasterOrder, @MemberOrder AS MemberOrder, @Score AS Score, @InoutIndex AS InoutIndex,
		@Insure AS Insure,  @WinCount AS WinCount,  @LostCount AS LostCount,@DrawCount AS DrawCount, @FleeCount AS FleeCount, 		
		@IntegralCount AS IntegralCount,@AgentID AS AgentID,@QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone, @MobilePhone AS MobilePhone,
		@DwellingPlace AS DwellingPlace,@PostalCode AS PostalCode, @GroupIDArray AS GroupIDArray, @lGroupScore AS GroupScore,@DayPlayTime AS DayPlayTime,@DayWinLostScore AS DayWinLostScore


END

RETURN 0

GO
---------------------------------------------------------------------------------------------