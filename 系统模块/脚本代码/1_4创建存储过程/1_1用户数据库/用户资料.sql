
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryUserIndividual]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyUserIndividual]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_BindSpreader]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_BindSpreader]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UpdatePlaceName]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UpdatePlaceName]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GP_QueryUserIndividual
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @DynamicPass AS NCHAR(32)

	-- ��ѯ�û�
	SELECT @DynamicPass=DynamicPass FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �����ж�
	IF @DynamicPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����û����벻��ȷ��������Ϣ��ѯʧ�ܣ�'
		RETURN 1
	END

	-- ��������
	DECLARE @UserID INT
	DECLARE @QQ NVARCHAR(16)
	DECLARE @EMail NVARCHAR(33)
	DECLARE @UserNote NVARCHAR(256)
	DECLARE @SeatPhone NVARCHAR(32)
	DECLARE @MobilePhone NVARCHAR(16)
	DECLARE @Compellation NVARCHAR(16)
	DECLARE @DwellingPlace NVARCHAR(128)
	DECLARE @PassPortID NVARCHAR(18)
	DECLARE	@Spreader NVARCHAR(31)
	
	SET @QQ=N''	
	SET @EMail=N''	
	SET @UserNote=N''	
	SET @SeatPhone=N''	
	SET @MobilePhone=N''	
	SET @Compellation=N''	
	SET @DwellingPlace=N''	
	SET @PassPortID=N''		
	SET @Spreader=N''

	-- ��ѯ�û�
	SELECT @UserID=UserID, @QQ=QQ, @EMail=EMail, @UserNote=UserNote, @SeatPhone=SeatPhone,@MobilePhone=MobilePhone, @DwellingPlace=DwellingPlace FROM IndividualDatum(NOLOCK) WHERE UserID=@dwUserID

	-- ��ѯ����
	DECLARE @SpreaderID INT	
	SELECT @Compellation=Compellation,@PassPortID = PassPortID,@SpreaderID = SpreaderID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- ��ѯ�ƹ�
	SELECT @Spreader = GAMEID FROM AccountsInfo  WHERE UserID=@SpreaderID

	-- �����Ϣ
	SELECT @dwUserID AS UserID,
	       @Compellation AS Compellation,
	       @PassPortID AS PassPortID, 
	       @QQ AS QQ, 
	       @EMail AS EMail, 
	       @SeatPhone AS SeatPhone,
		   @MobilePhone AS MobilePhone,
		   @DwellingPlace AS DwellingPlace, 
		   @UserNote AS UserNote,
		   @Spreader AS Spreader

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GP_ModifyUserIndividual
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@cbGender TINYINT,							-- �û��Ա�
	@strNickName NVARCHAR(32),					-- �û��ǳ�
	@strUnderWrite NVARCHAR(63),				-- ����ǩ��
	@strCompellation NVARCHAR(16),				-- ��ʵ����
	@strPassPortID NVARCHAR(18),				-- ֤������	
	@strQQ NVARCHAR(16),						-- Q Q ����
	@strEMail NVARCHAR(33),						-- �����ʵ�
	@strSeatPhone NVARCHAR(32),					-- �̶��绰
	@strMobilePhone NVARCHAR(16),				-- �ƶ��绰
	@strDwellingPlace NVARCHAR(128),			-- ��ϸ��ַ
	@strUserNote NVARCHAR(256),					-- �û���ע	
	@strSpreader NVARCHAR(31),					-- �Ƽ��ʺ�				
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @UserID INT
	DECLARE @NickName NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	-- ��ұ���
	DECLARE @SourceScore BIGINT

	-- ��ѯ�û�
	SELECT @UserID=UserID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown
	FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

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
	
	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 3
	END

	-- Ч���ǳ�
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickName)>0)>0
	BEGIN
		SET @strErrorDescribe=N'�����������Ϸ�ǳ������������ַ�����������ǳ������ٴ��޸ģ�'
		RETURN 4
	END

	-- �����ж�
	--IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName AND UserID<>@UserID)
	--BEGIN
	--	SET @strErrorDescribe=N'���ǳ��ѱ��������ʹ���ˣ�������ǳ������ٴ��޸ģ�'
	--	RETURN 4
	--END
		
	-- �޸�����
	UPDATE AccountsInfo SET NickName=@strNickName, UnderWrite=@strUnderWrite, Gender=@cbGender	WHERE UserID=@dwUserID	
		
	-- �ǳƼ�¼
--	IF @NickName<>@strNickName
--	BEGIN
--		INSERT INTO WHQJRecordDB.dbo.RecordAccountsExpend(UserID,ReAccounts,ClientIP)
--		VALUES(@dwUserID,@strNickName,@strClientIP)
--	END

	-- �޸�����
	UPDATE IndividualDatum SET QQ=@strQQ, EMail=@strEMail, SeatPhone=@strSeatPhone,
		MobilePhone=@strMobilePhone, DwellingPlace=@strDwellingPlace, UserNote=@strUserNote WHERE UserID=@dwUserID
		
	-- �޸�����
	IF @@ROWCOUNT=0
	INSERT IndividualDatum (UserID, QQ, EMail, SeatPhone, MobilePhone, DwellingPlace, UserNote)
	VALUES (@dwUserID, @strQQ, @strEMail, @strSeatPhone, @strMobilePhone, @strDwellingPlace, @strUserNote)

	-- ������Ϣ
	IF @@ERROR=0 SET @strErrorDescribe=N'����ǩ���Ѹ���'

	
		
	RETURN 0

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- ���ƹ�
CREATE PROC GSP_GP_BindSpreader
	@dwUserID INT,								-- �û� I D
	@dwSpreaderID INT,							-- �ƹ� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��������
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @ExistSpreadID INT
	-- ��ұ���
	DECLARE @SourceScore BIGINT

	-- ��ѯ�û�
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown, @ExistSpreadID = SpreaderID
	FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

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
	
	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 3
	END
	
	-- �жϸ��˺� �Ƿ��Ѿ����ƹ�
	IF @ExistSpreadID = 0 OR @ExistSpreadID IS NULL
	BEGIN
		SET @strErrorDescribe=N'���˺��Ѿ����ƹ�Ա�������ٴΰ󶨣�'
		RETURN 3
	END

	-- ���ƹ�Ա
	DECLARE @SpreaderID INT	
	IF @dwSpreaderID<>0
	BEGIN
		-- ���ƹ�Ա
		SELECT @SpreaderID=UserID FROM AccountsInfo(NOLOCK) WHERE GameID = @dwSpreaderID 
		
		-- �������
		IF @SpreaderID IS NULL
		BEGIN
			SET @SpreaderID=0
			SET @strErrorDescribe=N'�ƹ�ԱID����򲻴��ڣ�����д��ȷ���ƹ�Ա�ʺţ�'
			RETURN 4
		END
		
		-- ��ѯ�ظ�	
		IF EXISTS (SELECT SpreaderID FROM AccountsInfo WHERE @SpreaderID=SpreaderID and UserID=@dwUserID)
		BEGIN
			SET @SpreaderID=0
			SET @strErrorDescribe=N'�Ѿ��ύ�ƹ�Ա���ύʧ�ܣ�'
			RETURN 5
		END	
	END
	ELSE
	BEGIN
		SET @strErrorDescribe=N'�ƹ�ԱID����򲻴��ڣ�����д��ȷ���ƹ�Ա�ʺţ�'
		RETURN 4
	END
	
	-- ��ȡ�ƹ�Ա��Ӧ����ID
	DECLARE @nAgentID INT

	SELECT @nAgentID = AgentID FROM AccountsInfo WHERE  UserID=@SpreaderID
	IF @nAgentID IS NULL SET @nAgentID = 0

	-- �󶨴���
	UPDATE AccountsInfo SET AgentID = @nAgentID WHERE UserID = @dwUserID

	-- �ƹ����
	IF @SpreaderID<>0 AND @SpreaderID IS NOT NULL
	BEGIN
		DECLARE @RegisterGrantScore INT
		DECLARE @Note NVARCHAR(512)
		DECLARE @dwDiamond INT	
		DECLARE @dwRewardDiamond INT
			
		SET @Note = N'�ƹ�Ա'
		SELECT @RegisterGrantScore = RegisterGrantScore FROM  WHQJTreasureDB.dbo.GlobalSpreadInfo
		IF @RegisterGrantScore IS NULL
		BEGIN
			SET @RegisterGrantScore=5000
		END
		INSERT INTO  WHQJTreasureDB.dbo.RecordSpreadInfo(
			UserID,Score,TypeID,ChildrenID,CollectNote)
		VALUES(@SpreaderID,@RegisterGrantScore,1,@dwUserID,@Note)	
		
		-- �Ƽ���¼
		UPDATE AccountsInfo SET SpreaderID = @SpreaderID WHERE UserID=@dwUserID		
		
		-- ������ʯ
		SELECT @dwRewardDiamond=StatusValue FROM SystemStatusInfo WHERE StatusName = N'JJBindSpreadPresent'
		IF @dwRewardDiamond IS NULL
		BEGIN
			SET @dwRewardDiamond=0
			SET @strErrorDescribe=N'�󶨳ɹ���'
			RETURN 7
		END
		ELSE
		BEGIN
			DECLARE @CurrentDiamond INT
			SELECT @CurrentDiamond=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
			--������ʯ
			IF @CurrentDiamond IS NOT NULL
			BEGIN
				UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@dwRewardDiamond WHERE UserID=@dwUserID
			END
			ELSE
			BEGIN
				SET @CurrentDiamond=0
				INSERT INTO  WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond)
				VALUES(@dwUserID,@dwRewardDiamond)
			END
			
			--������־
			DECLARE @DateTime DATETIME
			--DECLARE @Random VARCHAR(5)
			DECLARE @SerialNumber NVARCHAR(20)
			SET @DateTime = GETDATE()
			--SET @Random = CAST(FLOOR(89999*RAND()+10000) AS VARCHAR(5))
			SELECT @SerialNumber=dbo.WF_GetSerialNumber()
			
			INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
			VALUES(@SerialNumber,0,@dwUserID,4,@CurrentDiamond,@dwRewardDiamond,@strClientIP,@DateTime)
			
			SET @dwDiamond=@CurrentDiamond+@dwRewardDiamond
			SET @strErrorDescribe=N'�󶨳ɹ�,��ϲ���'++CONVERT(NVARCHAR,@dwRewardDiamond)++N'��ʯ����'
			
			SELECT @dwDiamond AS dwDiamond, @dwRewardDiamond AS dwRewardDiamond
			RETURN 0
		END
	END
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- ���µ���
CREATE PROC GSP_GP_UpdatePlaceName
	@dwUserID INT,								-- �û� I D
	@szPlaceName NVARCHAR(33),					-- ���µ���	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- �޸ĵ���
	UPDATE AccountsInfo SET PlaceName=@szPlaceName	WHERE UserID=@dwUserID		

	SET @strErrorDescribe=N'���³ɹ���'
END

RETURN 0
GO