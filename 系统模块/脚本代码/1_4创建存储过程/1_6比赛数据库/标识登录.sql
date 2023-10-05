----------------------------------------------------------------------------------------------------
USE WHQJGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyUserID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyUserID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyMobile]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyMobile]
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
	@dwMatchID INT,								-- ������ʶ
	@lMatchNo BIGINT,							-- �������
	@cbMatchType TINYINT,						-- ��������	
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
DECLARE @Insure BIGINT
DECLARE @Dimand BIGINT
DECLARE @Beans decimal(18, 2)
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @AgentID AS INT
DECLARE @IntegralCount BIGINT 

--��ʱِ����
DECLARE @TimeMatchEnter INT

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
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SET @AgentID = 0	
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @UnderWrite=UnderWrite, @LogonPass=DynamicPass, @FaceID=FaceID, @CustomID=CustomID,
		@Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, 
		--@UserMedal=UserMedal, @Experience=Experience, @LoveLiness=LoveLiness,
		@UserRight=UserRight,@MasterRight=MasterRight, @MasterOrder=MasterOrder, @MemberOrder=MemberOrder, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,--@TimeMatchEnter=TimeMatchEnter,
		@AgentID=AgentID
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

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
			RETURN 1
		END
	END

	-- �����ж�
	IF @strPassword IS NULL OR @LogonPass IS NULL OR  @LogonPass<>@strPassword AND @strClientIP<>N'0.0.0.0'
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- �̶�����
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SET @strErrorDescribe=N'�����ʺųɹ�ʹ���˹̶�������¼���ܣ�'
		UPDATE  WHQJAccountsDB.dbo.AccountsInfo SET MoorMachine=@MoorMachine, LastLogonMachine=@strMachineID WHERE UserID=@UserID
	END

	-- ��ѯ����
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		-- �������
		DELETE  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID

		-- ��ѯ����
		DECLARE @LockKindID INT
		DECLARE @LockServerID INT
		SELECT @LockKindID=KindID, @LockServerID=ServerID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@dwUserID

		-- �����ж�
		IF @LockKindID IS NOT NULL AND @LockServerID IS NOT NULL
		BEGIN

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
				SET @strErrorDescribe=N'���Ѿ��� [ '+@KindName+N' ] �� [ '+@ServerName+N' ] ��Ϸ�����У������ٴν������Ϸ���䣡'
				RETURN 4

			END
			ELSE
			BEGIN
				-- ��ʾ��Ϣ
				SELECT [ErrorDescribe]=N'��ǰ��Ϸ�������Ϸ�����ѱ�ϵͳ��������ʱ�޷��������Ϸ���䣡'
				RETURN 4
			END
		END
	END

	-- ��Ϸ��Ϣ
	DECLARE @MatchRight INT
	DECLARE @AllLogonTimes INT	
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT
	SELECT 	@GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder, @AllLogonTimes=AllLogonTimes	
	FROM GameScoreInfo WHERE UserID=@dwUserID	

	-- �����ж�
	IF @GameUserRight IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine) VALUES (@dwUserID, @strClientIP, @strMachineID)

		-- ��Ϸ��Ϣ
		SELECT @GameUserRight=UserRight, @GameMasterOrder=MasterOrder, @GameMasterRight=MasterRight, @AllLogonTimes=AllLogonTimes
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- ��ʱ����
	IF @cbMatchType=0
	BEGIN
		-- ������Ϣ	
		SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @MatchRight=MatchRight
		FROM MatchScoreInfo WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
		
		-- ���·����ʶ
		IF @@Rowcount>0	
		BEGIN
			UPDATE MatchScoreInfo SET ServerID=@wServerID WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
		END					
	END ELSE
	BEGIN
		-- ������Ϣ	
		SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @MatchRight=MatchRight
		FROM MatchScoreInfo WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo				
	END

	-- ��������
	IF @Score IS NULL
	BEGIN
		SELECT @Score=0, @WinCount=0, @LostCount=0,@DrawCount=0, @DrawCount=0, @FleeCount=0,@MatchRight=0
	END

	-- ��������
	SET @GroupID=0
	SET @GroupName=''

	-- Ȩ�ޱ�־
	SET @UserRight=@UserRight|@GameUserRight|@MatchRight
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- Ȩ�޵ȼ�
	IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder

	-- �����¼
--	INSERT RecordUserInout (UserID, EnterScore, EnterUserMedal,EnterLoveliness, KindID, ServerID, EnterClientIP, EnterMachine)
--	VALUES (@UserID, @Score, @UserMedal, @Loveliness, @wKindID, @wServerID, @strClientIP, @strMachineID)

	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex= 0--SCOPE_IDENTITY()

	-- ��������
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		INSERT  WHQJTreasureDB.dbo.GameScoreLocker (UserID, ServerID, KindID, EnterID, EnterIP, EnterMachine) VALUES (@dwUserID, @wServerID, @wKindID, @InoutIndex, @strClientIP, @strMachineID)
		IF @@ERROR<>0
		BEGIN
			-- ������Ϣ
			SET @strErrorDescribe=N'��Ǹ��֪ͨ�㣬��Ϸ������������ʧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
			RETURN 14
		END
	END

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

	-- ��ѯ����
	SELECT @Insure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID		
	IF @Insure IS NULL SET @Insure=0
	
	-- ��ѯ��ʯ
	SELECT @Dimand=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID		
	IF @Dimand IS NULL SET @Dimand=0

	-- �������
	SET @IntegralCount = 0
	SELECT  @IntegralCount=IntegralCount FROM GameScoreAttribute WHERE UserID=@dwUserID And ServerID=@wServerID And KindID = @wKindID
	
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @GroupID AS GroupID, @NickName AS NickName, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @Dimand AS Dimand, 
		@CustomID AS CustomID, @Gender AS Gender, @GroupName AS GroupName, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight,
		@MasterOrder AS MasterOrder, @MemberOrder AS MemberOrder, @Score AS Score,  0 AS Grade, @Insure AS Insure,  @WinCount AS WinCount,  @LostCount AS LostCount,
		@DrawCount AS DrawCount, @FleeCount AS FleeCount, @UserMedal AS Ingot, @Experience AS Experience, @LoveLiness AS LoveLiness, @InoutIndex AS InoutIndex,
		@IntegralCount AS IntegralCount,@AgentID AS AgentID,@QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone, @MobilePhone AS MobilePhone,
		@DwellingPlace AS DwellingPlace,@PostalCode AS PostalCode,0 AS GroupIDArray,0 AS HistoryKindIDWin,0 AS DayPlayTime,0 AS DayWinLostScore

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
	@dwMatchID INT,								-- ������ʶ
	@lMatchNo BIGINT,							-- �������	
	@cbMatchType TINYINT,						-- ��������	
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

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Dimand BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
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
	SET @AgentID = 0
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @UnderWrite=UnderWrite, @LogonPass=DynamicPass, @FaceID=FaceID, @CustomID=CustomID,
		@Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@UserRight=UserRight,
		-- @UserMedal=UserMedal, @Experience=Experience, @LoveLiness=LoveLiness,		
		@MasterRight=MasterRight, @MasterOrder=MasterOrder, @MemberOrder=MemberOrder, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,@AgentID=AgentID
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

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
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- ��ѯ����
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		-- �������
		DELETE  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID

		-- ��ѯ����
		DECLARE @LockKindID INT
		DECLARE @LockServerID INT
		SELECT @LockKindID=KindID, @LockServerID=ServerID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@dwUserID

		-- �����ж�
		IF @LockKindID IS NOT NULL AND @LockServerID IS NOT NULL
		BEGIN

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
				SELECT [ErrorDescribe]=N'��ǰ��Ϸ�������Ϸ�����ѱ�ϵͳ��������ʱ�޷��������Ϸ���䣡'
				RETURN 4
			END
		END
	END


	-- ��Ϸ��Ϣ
	DECLARE @MatchRight INT
	DECLARE @AllLogonTimes INT	
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder SMALLINT
	SELECT 	@GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder, @AllLogonTimes=AllLogonTimes	
	FROM GameScoreInfo WHERE UserID=@dwUserID	

	-- �����ж�
	IF @GameUserRight IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine) VALUES (@dwUserID, @strClientIP, @strMachineID)

		-- ��Ϸ��Ϣ
		SELECT @GameUserRight=UserRight, @GameMasterOrder=MasterOrder, @GameMasterRight=MasterRight, @AllLogonTimes=AllLogonTimes
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- ��ʱ����
	IF @cbMatchType=0
	BEGIN
		-- ������Ϣ	
		SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @MatchRight=MatchRight
		FROM MatchScoreInfo WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
		
		-- ���·����ʶ
		IF @@Rowcount>0	
		BEGIN
			UPDATE MatchScoreInfo SET ServerID=@wServerID WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
		END					
	END ELSE
	BEGIN
		-- ������Ϣ	
		SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @FleeCount=FleeCount, @MatchRight=MatchRight
		FROM MatchScoreInfo WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo				
	END

	-- ��������
	IF @Score IS NULL
	BEGIN
		SELECT @Score=0, @WinCount=0, @LostCount=0,@DrawCount=0, @DrawCount=0, @FleeCount=0,@MatchRight=0
	END

	-- Ȩ�ޱ�־
	SET @UserRight=@UserRight|@GameUserRight
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- Ȩ�޵ȼ�
	IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder

	-- ��ѯ��ʯ
	SELECT @Dimand=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID		
	IF @Dimand IS NULL SET @Dimand=0

-- �����¼
	INSERT RecordUserInout (UserID, EnterScore, KindID, ServerID, EnterClientIP, EnterMachine)
	VALUES (@UserID, @Dimand, @wKindID, @wServerID, @strClientIP, @strMachineID)	
	
	-- ��¼��ʶ
	DECLARE @InoutIndex BIGINT
	SET @InoutIndex=SCOPE_IDENTITY()

	-- ��������
	IF @strClientIP<>N'0.0.0.0'
	BEGIN
		INSERT  WHQJTreasureDB.dbo.GameScoreLocker (UserID, ServerID, KindID, EnterID, EnterIP, EnterMachine) VALUES (@dwUserID, @wServerID, @wKindID, @InoutIndex, @strClientIP, @strMachineID)
		IF @@ERROR<>0
		BEGIN
			-- ������Ϣ
			SET @strErrorDescribe=N'��Ǹ��֪ͨ�㣬��Ϸ������������ʧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
			RETURN 14
		END
	END

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

	-- ��ѯ����
	SELECT @Insure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID		
	IF @Insure IS NULL SET @Insure=0
	
	-- ��ѯ��Ϸ��
--	SELECT @Beans=Currency FROM  WHQJTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@UserID		
--	IF @Beans IS NULL SET @Beans=0

	-- �������
	SET @IntegralCount = 0
	SELECT  @IntegralCount=IntegralCount FROM GameScoreAttribute WHERE UserID=@dwUserID And ServerID=@wServerID And KindID = @wKindID
	
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @dwGroupID AS GroupID, @NickName AS NickName, @UnderWrite AS UnderWrite, @FaceID AS FaceID, @Dimand AS Dimand, 
		@CustomID AS CustomID, @Gender AS Gender, N'' AS GroupName, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight,
		@MasterOrder AS MasterOrder, @MemberOrder AS MemberOrder, @Score AS Score,  0 AS Grade, @Insure AS Insure,  @WinCount AS WinCount,  @LostCount AS LostCount,
		@DrawCount AS DrawCount, @FleeCount AS FleeCount, @UserMedal AS Ingot, @Experience AS Experience, @LoveLiness AS LoveLiness, @InoutIndex AS InoutIndex,
		@IntegralCount AS IntegralCount,@AgentID AS AgentID,@QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone, @MobilePhone AS MobilePhone,
		@DwellingPlace AS DwellingPlace,@PostalCode AS PostalCode,0 AS GroupIDArray, 0 AS GroupScore,0 AS DayPlayTime,0 AS DayWinLostScore,0 AS HistoryKindIDWin

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------