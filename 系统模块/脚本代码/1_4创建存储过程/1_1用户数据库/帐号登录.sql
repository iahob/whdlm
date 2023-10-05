
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_EfficacyAccounts]
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyAccounts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyOtherPlatform]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyOtherPlatform]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyLogonVisitor]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyLogonVisitor]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �ʺŵ�¼
CREATE PROC GSP_GP_EfficacyAccounts
	@strAccounts NVARCHAR(31),					-- �û��ʺ�
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @AgentID INT
DECLARE @PlayTimeCount INT

-- �Ƹ�����
DECLARE @Score BIGINT
DECLARE @Insure BIGINT

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'���ã�������IP��ַ�ĵ�¼�����ѱ����ã�����ϵ�ͷ��˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'���ã���ʹ�õĻ����ĵ�¼�����ѱ����ã�����ϵ�ͷ��˽���ϸ�����'
		RETURN 7
	END
 
	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineSerial NCHAR(32)
	DECLARe @strLastLogonIP NCHAR(15)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,@InsurePass=InsurePass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,
		 @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,@strLastLogonIP=LastLogonIP,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@AgentID=AgentID
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 1
	END	
	
	-- ��ѯ����
	--IF EXISTS (SELECT * FROM AccountsAgent WHERE @UserID = UserID and Nullity=0)
	--BEGIN
		--SET @strErrorDescribe=N'���Ǵ������ʺţ����ɵ�¼��'
		--RETURN 1
	--END		
	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ�����ᣬ����ϵ�ͷ��˽���ϸ�����'
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
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	-- �����ж�
	IF @LogonPass<>@strPassword OR @strPassword IS NULL OR @LogonPass IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 3
	END

	
	-- �̶�����
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SET @strErrorDescribe=N'�����ʺųɹ�ʹ���˹̶�������¼���ܣ�'
		UPDATE AccountsInfo SET MoorMachine=@MoorMachine, LastLogonMachine=@strMachineID WHERE UserID=@UserID
	END

	-- �ƹ�Ա���
	IF @SpreaderID<>0 AND @AgentID=0
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM WHQJTreasureDB.dbo.GlobalSpreadInfo WHERE ID=1
		IF @GrantTime IS NULL
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END	
		
		IF @GrantScore>0 AND @GrantTime>0 AND @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM WHQJTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO  WHQJTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	
	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0

	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--�ж�����Ƿ��ڷ����У�����ڷ����в����¶�̬����
	DECLARE @LockServerID INT
	SELECT @LockServerID = ServerID FROM WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerID IS NOT NULL
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1,LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			LastLogonMachine=@strMachineID 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
			-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1,LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID 
		WHERE UserID=@UserID
	END

	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ��¼��־
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1
	
	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF @AgentID>0 SET @IsAgent=1
		
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender,
		@Score AS Score, @Insure AS Insure, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate, @MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled, @PlatformID AS LogonMode,@IsAgent AS IsAgent
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �ʺŵ�¼
CREATE PROC GSP_MB_EfficacyAccounts
	@strAccounts NVARCHAR(31),					-- �û��ʺ�
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strMobilePhone NVARCHAR(11),				-- �ֻ�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @PlayTimeCount INT

-- �Ƹ�����
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @lDiamond BIGINT
DECLARE @lSilver BIGINT
DECLARE @nAwardTicket INT

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @MBUserRight INT


DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END

	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE	@MachineSerial NCHAR(32)	
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,@InsurePass=InsurePass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,
		@MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine,@MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@MBUserRight=UserRight
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

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
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END
		
	-- �����ж�
	IF @LogonPass<>@strPassword OR @strPassword IS NULL OR @LogonPass IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 3
	END

	-- �ƹ�Ա���
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM  WHQJTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM  WHQJTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO WHQJTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID


	-- ��ѯ��ʯ
	SELECT @lDiamond=Diamond, @nAwardTicket=AwardTicket FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID
	
	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @lDiamond IS NULL SET @lDiamond=0
	IF @nAwardTicket IS NULL SET @nAwardTicket=0

	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--�ж�����Ƿ��ڷ����У�����ڷ����в����¶�̬����
	DECLARE @LockServerIDCheck INT
	SELECT @LockServerIDCheck = ServerID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerIDCheck IS NOT NULL
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END


	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ��¼��־
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1
	
	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID and Nullity=0) SET @IsAgent=1

	
	-- ��������ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID=KindID FROM WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0


	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, 	
		@Score AS Score, @Insure AS Insure, @lDiamond AS Diamond, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled, @PlatformID AS LogonMode ,@IsAgent AS IsAgent,
		@LockServerID AS LockServerID,  @wKindID AS KindID, @MBUserRight AS UserRight, @nAwardTicket AS AwardTicket

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- ��������¼
CREATE PROC GSP_MB_EfficacyOtherPlatform
	@cbPlatformID TINYINT,						-- ƽ̨���
	@strUserUin NVARCHAR(32),			        -- �û�Uin
	@strNickName NVARCHAR(32),					-- �û��ǳ�
	@strCompellation NVARCHAR(16),				-- ��ʵ����
	@cbGender TINYINT,							-- �û��Ա�
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strMobilePhone NVARCHAR(11),				-- �ֻ�����
	@cbDeviceType TINYINT,						-- ע����Դ	
	@cbClientType TINYINT,						-- �ͻ�������	
	@DeviceToken  NVARCHAR(65),					-- ���豸��ʶ
	@strFaceUrl NVARCHAR(500),					-- ͷ��URL
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @AgentID INT
DECLARE @PlayTimeCount INT

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @lDiamond BIGINT
DECLARE @nAwardTicket INT
DECLARE @DiamondCount BIGINT
-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN

	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END

	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE	@MachineSerial NCHAR(32)
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@InsurePass=InsurePass,
		@MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@AgentID=AgentID
	FROM AccountsInfo(NOLOCK) WHERE UserUin=@strUserUin AND PlatformID=@cbPlatformID

    DECLARE @strTempName NVARCHAR(31)
	DECLARE @cbNewUser TINYINT
	SET @cbNewUser=0
	-- ע���û�
	IF @UserID IS NULL
	BEGIN
		--У��ͻ�������
		IF @cbClientType NOT IN (0x01, 0x02, 0x03)
		BEGIN
			SET @strErrorDescribe=N'ע��ͻ���������Ч��'
			RETURN 8
		END
	
		-- ����˺�
		SET @strTempName=@strNickName --+CONVERT(nvarchar(8),REPLACE(newid(),'-',''))
		
		-- �����˺�
		DECLARE @strTemp NVARCHAR(31)
		SET @strTemp=CONVERT(NVARCHAR(31),REPLACE(NEWID(),'-','_'))
		
		-- �����ƹ��ѯ
		SELECT @SpreaderID=GameID FROM BindIP WHERE UserIP = @strClientIP
		
		IF @SpreaderID IS NOT NULL AND @SpreaderID<>0
		BEGIN
			DELETE FROM BindIP WHERE GameID=@SpreaderID AND UserIP = @strClientIP
		END
		ELSE 
		BEGIN
			SET @SpreaderID=0
		END
		
		-- ע���û�
		INSERT AccountsInfo (Accounts,NickName,RegAccounts,PlatformID,UserUin,LogonPass,InsurePass,Gender,FaceID,SpreaderID,
			GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine,RegisterOrigin,ClientType)
		VALUES (@strTemp,@strTempName,@strTemp,@cbPlatformID,@strUserUin,N'd1fd5495e7b727081497cfce780b6456',N'',@cbGender,0,@SpreaderID,
			1,@strClientIP,N'',@strMachineID,@strClientIP,N'',@strMachineID,@cbDeviceType, @cbClientType)
		
		-- ��¼��־
		UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

		-- ��ѯ�û�
		SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
			@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown,@MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
			@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@AgentID=AgentID
		FROM AccountsInfo(NOLOCK) WHERE UserUin=@strUserUin AND PlatformID=@cbPlatformID

		-- ����ͷ��
		INSERT AccountsFace (UserID, InsertTime, InsertAddr, InsertMachine,FaceUrl)
		VALUES (@UserID,  @DateID, @strClientIP, '',@strFaceUrl)
		-- �����û�	
		SELECT @CustomID=SCOPE_IDENTITY()
	
		-- �����ʶ
		SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
		IF @GameID IS NULL 
		BEGIN
			UPDATE AccountsInfo SET CustomID = @CustomID WHERE UserID = @UserID
			SET @GameID=0
			SET @strErrorDescribe=N'�û�ע��ɹ�����δ�ɹ���ȡ��Ϸ ID ���룬ϵͳ�Ժ󽫸������䣡'
		END
		ELSE UPDATE AccountsInfo SET GameID=@GameID,CustomID = @CustomID WHERE UserID=@UserID

		
		-- ��ʼ�������Ϣ
		INSERT INTO WHQJTreasureDB.dbo.GameScoreInfo(UserID,Score,RegisterIP) VALUES(@UserID,0,@strClientIP)
		INSERT INTO WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket) VALUES(@UserID,0,0)
		
		-- ע��������ʯ
		DECLARE @PresentDiamond INT
		DECLARE @PresentGold INT
		SELECT @PresentDiamond=DiamondCount, @PresentGold=ScoreCount FROM RegisterGive WITH(NOLOCK) WHERE PlatformType = @cbClientType
		IF (@PresentDiamond IS NULL OR @PresentDiamond<0) SET @PresentDiamond=0
		IF (@PresentGold IS NULL OR @PresentGold<0) SET @PresentGold=0
		
		IF @PresentDiamond>0 OR @PresentGold>0
		BEGIN

			-- ���ͽ��
			UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score = Score+@PresentGold WHERE UserID = @UserID

			-- ������ʯ
			UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond = Diamond+@PresentDiamond WHERE UserID = @UserID
			
			-- ��¼�����ˮ��¼
			DECLARE @DateTime DATETIME
			DECLARE @SerialNumber NVARCHAR(20)
			SET @DateTime = GETDATE()
			SELECT @SerialNumber=dbo.WF_GetSerialNumber()
			
			IF @PresentGold IS NOT NULL AND @PresentGold <> 0

			--�����ˮ
			INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@UserID,1,0,0,@PresentGold,@strClientIP,@DateTime)	

			--��ʯ��ˮ
			INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@UserID,1,0,@PresentDiamond,@strClientIP,GETDATE())
		
		END

		SET @cbNewUser=1
	END	
	ELSE
	BEGIN	   
		-- �޸��ǳ�
		IF @NickName<>@strNickName
		BEGIN
			-- ����˺�			
			SET @NickName=@strNickName
			UPDATE AccountsInfo SET NickName=@strNickName WHERE UserID=@UserID			
		END

		-- �Ա��޸�
		IF @cbGender <> @Gender
		BEGIN
			SET @Gender = @cbGender
			UPDATE AccountsInfo SET Gender=@Gender WHERE UserID=@UserID			
		END
		
		-- ͷ��URL
		DECLARE @FaceUrl NVARCHAR(500)
		SELECT @FaceUrl = FaceUrl FROM AccountsFace WHERE UserID = @UserID
		IF @FaceUrl <> @strFaceUrl					
		BEGIN
			UPDATE AccountsFace SET FaceUrl=@strFaceUrl WHERE UserID=@UserID			
		END
	END

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ������˺�:'+CONVERT(NVARCHAR(31),@GameID)
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
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	-- �ƹ�Ա���
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM  WHQJTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM  WHQJTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO  WHQJTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	-- ��ѯ��ʯ
	SELECT @lDiamond=Diamond, @nAwardTicket=AwardTicket FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID
	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0	
	IF @lDiamond IS NULL SET @lDiamond = 0
	IF @nAwardTicket IS NULL SET @nAwardTicket = 0
	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--�ж�����Ƿ��ڷ����У�����ڷ����в����¶�̬����
	DECLARE @LockServerIDCheck INT
	SELECT @LockServerIDCheck = ServerID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerIDCheck IS NOT NULL
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP, LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	
	-- ��¼��־
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)
	
	-- ��¼UMeng��־	
	IF @cbDeviceType = 17 or @cbDeviceType = 18
	BEGIN
		UPDATE AccountsUmeng SET DeviceType=0,DeviceToken=@DeviceToken,UpdateTime=GETDATE(),UpdateAddress=@strClientIP WHERE UserID=@UserID
		IF @@ROWCOUNT=0 INSERT AccountsUmeng(UserID,GameID,DeviceType,DeviceToken,UpdateTime,UpdateAddress) VALUES (@UserID,@GameID,0,@DeviceToken,GETDATE(),@strClientIP)
	END 
	IF @cbDeviceType = 49 or @cbDeviceType = 50 or @cbDeviceType = 65 or @cbDeviceType = 66
	BEGIN
		UPDATE AccountsUmeng SET DeviceType=1,DeviceToken=@DeviceToken,UpdateTime=GETDATE(),UpdateAddress=@strClientIP WHERE UserID=@UserID
		IF @@ROWCOUNT=0 INSERT AccountsUmeng(UserID,GameID,DeviceType,DeviceToken,UpdateTime,UpdateAddress) VALUES (@UserID,@GameID,1,@DeviceToken,GETDATE(),@strClientIP)
	END
	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1

	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF @AgentID>0 SET @IsAgent=1
	--IF EXISTS (SELECT * FROM WHQJAgentDB.dbo.AgentInfo WHERE UserID=@UserID and Nullity=0) SET @IsAgent=1
	
	-- ��������ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID=KindID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0
	IF @cbNewUser=0
		SET @strClientIP=''
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender,@Score AS Score,@Insure AS Insure,
		@MemberOrder AS MemberOrder,@MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine,@InsureEnabled AS InsureEnabled,
		@PlatformID AS LogonMode,@IsAgent AS IsAgent,@lDiamond AS Diamond,@LockServerID AS LockServerID, @wKindID AS KindID,@nAwardTicket AS AwardTicket,@strClientIP AS ClientIP
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �ο͵�¼
CREATE PROC GSP_MB_EfficacyLogonVisitor
	@cbLogonMode TINYINT,						-- ��½��ʽ
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strMobilePhone NVARCHAR(11),				-- �ֻ�����
	@cbDeviceType TINYINT,						-- ע����Դ		
	@cbClientType TINYINT,						-- ƽ̨���		
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @AgentID INT
DECLARE @PlayTimeCount INT

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @lDiamond BIGINT
DECLARE @nAwardTicket INT
DECLARE @DiamondCount BIGINT

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN

	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END

	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE	@MachineSerial NCHAR(32)
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @InsurePass=InsurePass,
		@MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@AgentID=AgentID
	FROM AccountsInfo(NOLOCK) WHERE RegisterMachine=@strMachineID AND PlatformID=@cbLogonMode

	-- ע���û�
	IF @UserID IS NULL
	BEGIN	

		-- ��ʱ�˺�
		DECLARE @strTemp NVARCHAR(31)
		SET @strTemp=CONVERT(NVARCHAR(31),REPLACE(NEWID(),'-','_'))
		
		-- �����ƹ��ѯ
		SELECT @SpreaderID=GameID FROM BindIP WHERE UserIP = @strClientIP
		
		IF @SpreaderID IS NOT NULL AND @SpreaderID<>0
		BEGIN
			DELETE FROM BindIP WHERE GameID=@SpreaderID AND UserIP = @strClientIP
		END
		ELSE 
		BEGIN
			SET @SpreaderID=0
		END
		
		-- ע���û�
		INSERT AccountsInfo (Accounts,NickName,RegAccounts,PlatformID,UserUin,LogonPass,InsurePass,Gender,FaceID,SpreaderID,
			GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine,RegisterOrigin)
		VALUES (@strTemp,@strTemp,@strTemp,@cbLogonMode,@strMachineID,N'd1fd5495e7b727081497cfce780b6456',N'',0,0,@SpreaderID,
			1,@strClientIP,N'',@strMachineID,@strClientIP,N'',@strMachineID,@cbDeviceType)

		-- ����˺�
		DECLARE @MaxUserID INT
		SELECT @MaxUserID=SCOPE_IDENTITY()
		DECLARE @strTempName NVARCHAR(31)
		SET @strTempName=N'�ο�'+CONVERT(NVARCHAR(31),@MaxUserID)
		
		--�����˺��ǳ�
		UPDATE AccountsInfo SET Accounts=@strTempName,NickName=@strTempName,RegAccounts=@strTempName WHERE UserID=@MaxUserID

		-- ��¼��־
		UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

		-- ��ѯ�û�
		SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass, @InsurePass=InsurePass,
			@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, 
			@MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
			@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@AgentID=AgentID
		FROM AccountsInfo(NOLOCK) WHERE RegisterMachine=@strMachineID AND PlatformID=@cbLogonMode

		-- �����ʶ
		SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
		IF @GameID IS NULL 
		BEGIN
			SET @GameID=0
			SET @strErrorDescribe=N'�û�ע��ɹ�����δ�ɹ���ȡ��Ϸ ID ���룬ϵͳ�Ժ󽫸������䣡'
		END
		ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

		-- ��ʼ�������Ϣ
		INSERT INTO WHQJTreasureDB.dbo.GameScoreInfo(UserID,RegisterIP) VALUES(@UserID,@strClientIP)
		INSERT INTO WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket) VALUES(@UserID,0,0)
		
		-- ע��������ʯ
		DECLARE @PresentDiamond INT
		DECLARE @PresentGold INT
		--SELECT @PresentDiamond=StatusValue FROM SystemStatusInfo WITH(NOLOCK) WHERE StatusName=N'JJRegisterDiamondCount'
		--SELECT @PresentGold=StatusValue FROM SystemStatusInfo WITH(NOLOCK) WHERE StatusName=N'GrantScoreCount'
		SELECT @PresentDiamond=DiamondCount, @PresentGold=ScoreCount FROM RegisterGive WITH(NOLOCK) WHERE PlatformType = @cbClientType
		IF (@PresentDiamond IS NULL OR @PresentDiamond<0) SET @PresentDiamond=0
		IF (@PresentGold IS NULL OR @PresentGold<0) SET @PresentGold=0
		IF @PresentDiamond>0 OR @PresentGold>0
		BEGIN
			-- ���ͽ��
			UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score = Score+@PresentGold WHERE UserID = @UserID 

			-- ������ʯ
			UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond = Diamond+@PresentDiamond WHERE UserID = @UserID
			
			-- ��¼�����ˮ��¼
			DECLARE @DateTime DATETIME
			DECLARE @SerialNumber NVARCHAR(20)
			SET @DateTime = GETDATE()
			SELECT @SerialNumber=dbo.WF_GetSerialNumber()
			
			IF @PresentGold IS NOT NULL AND @PresentGold <> 0

			--�����ˮ
			INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@UserID,1,0,0,@PresentGold,@strClientIP,@DateTime)	

			--��ʯ��ˮ
			INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@UserID,1,0,@PresentDiamond,@strClientIP,GETDATE())
			
			--INSERT INTO WHQJRecordDB.dbo.RecordRegisterGrant(UserID,RegisterIP,RegisterMachine,RegisterOrigin,GrantDiamond,GrantGold) 
			--VALUES(@UserID,@strClientIP,@strMachineID,@cbDeviceType,@PresentDiamond,@PresentGold)
		END
	END	
	
	-- ����
	IF EXISTS(SELECT * FROM AccountsVisitor WHERE VisitorMachine=@strMachineID )
	BEGIN
		SET @strErrorDescribe=N'��¼ʧ�ܣ����ʺ��Ѿ��󶨣�'
		return 10
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
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	
	-- �ƹ�Ա���
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM  WHQJTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM  WHQJTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO  WHQJTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	-- ��ѯ��ʯ
	SELECT @lDiamond=Diamond ,@nAwardTicket=AwardTicket FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@UserID
	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0	
	IF @lDiamond IS NULL SET @lDiamond = 0
	IF @nAwardTicket IS NULL SET @nAwardTicket = 0
	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--�ж�����Ƿ��ڷ����У�����ڷ����в����¶�̬����
	DECLARE @LockServerIDCheck INT
	SELECT @LockServerIDCheck = ServerID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerIDCheck IS NOT NULL
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END


	-- ��¼��־
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1

	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF @AgentID>0 SET @IsAgent=1

	IF @strErrorDescribe IS NULL
	BEGIN
		SET @strErrorDescribe=N'�ο͵�¼�ɹ���'
	END

	-- ��������ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID=KindID FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0
		
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender,@Score AS Score,@Insure AS Insure,
		@MemberOrder AS MemberOrder, @MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled, 
		@PlatformID AS LogonMode,@IsAgent AS IsAgent,@lDiamond AS Diamond,@LockServerID AS LockServerID, @wKindID AS KindID,@nAwardTicket AS AwardTicket

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------