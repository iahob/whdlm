
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AccountBind]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AccountBind]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AccountBindExists]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AccountBindExists]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �ʺŰ�
CREATE PROC GSP_GP_AccountBind
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
		
	@strBindAccounts NVARCHAR(31),				-- ���ʺ�	
	@strBindPassword NCHAR(32),					-- ������	
	@strBindSpreader NVARCHAR(31),				-- ���ƹ�		
	@cbDeviceType TINYINT,						-- ע����Դ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��������
	DECLARE @BindUserID INT
	
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @PlatformID SMALLINT
	DECLARE @LogonPass AS NCHAR(32)

	-- ��ѯ�û�
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity,@PlatformID=PlatformID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

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
	
	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 3
	END
	
	-- �ο��ж�
	IF @PlatformID<>2
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����οͣ����ʺ�ʧ�ܣ�'
		RETURN 4
	END
			
	-- ����
	IF EXISTS(SELECT * FROM AccountsVisitor WHERE VisitorMachine=@strMachineID )
	BEGIN
		SET @strErrorDescribe=N'�˻����Ѿ����ʺţ������ظ��󶨣�'
		return 5
	END
	
	-- ��ѯ�û�
	IF EXISTS(SELECT * FROM AccountsVisitor WHERE BindAccounts=@strBindAccounts )
	BEGIN
		SET @strErrorDescribe=N'���ʺ��Ѿ��󶨣������ٴΰ󶨣�'
		return 6
	END
	
	-- ��ѯ����
	IF EXISTS(SELECT * FROM AccountsInfo WHERE Accounts=@strBindAccounts )
	BEGIN
		SET @strErrorDescribe=N'�����ʺ��Ѿ�ע�ᣬ���ʺ�ʧ�ܣ�'
		return 7
	END	
	
	-- ���ƹ�Ա
	DECLARE @SpreaderID INT
	IF @strBindSpreader<>''
	BEGIN
		-- ���ƹ�Ա
		SELECT @SpreaderID=UserID FROM AccountsInfo(NOLOCK) WHERE Accounts=@strBindSpreader
		
		-- �������
		IF @SpreaderID IS NULL
		BEGIN
			SET @strErrorDescribe=N'�ƹ�ԱID����򲻴��ڣ�����д��ȷ���ƹ�Ա�ʺţ�'
			RETURN 8			
		END
	END
	ELSE SET @SpreaderID=0
		
	--ע���ʺ�
	INSERT AccountsInfo (Accounts,NickName,RegAccounts,PlatformID,UserUin,LogonPass,InsurePass,SpreaderID,Gender,FaceID,
		GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine,RegisterOrigin)
	VALUES (@strBindAccounts,@strBindAccounts,@strBindAccounts,0,N'',@strBindPassword,N'',@SpreaderID,0,0,
		1,@strClientIP,N'',@strMachineID,@strClientIP,N'',@strMachineID,@cbDeviceType)	
	
	SELECT @BindUserID = UserID   FROM AccountsInfo WHERE Accounts=@strBindAccounts
	IF @BindUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'ע��ʧ�ܣ����֤���ٴγ��ԣ�'
		RETURN 8
	END	

	-- �����ʶ
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
	IF @GameID IS NULL 
	BEGIN
		SET @GameID=0
		SET @strErrorDescribe=N'�û�ע��ɹ�����δ�ɹ���ȡ��Ϸ ID ���룬ϵͳ�Ժ󽫸������䣡'
	END
	ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@BindUserID
		
	-- ���ֹ���
	INSERT WHQJTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP,RegisterMachine) VALUES (@BindUserID, 0, @strClientIP, @strClientIP,@strMachineID) 

	-- ��¼��
	INSERT INTO AccountsVisitor([VisitorUserID],[VisitorMachine],[BindUserID],[BindAccounts],[BindType]) 
	VALUES (@dwUserID,@strMachineID,@BindUserID,@strBindAccounts,0)
	
	-- �ϲ�����
	DECLARE @Score BIGINT
	DECLARE @Insure BIGINT
	
	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	
	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	
	-- �ϲ���	
	UPDATE WHQJTreasureDB.dbo.GameScoreInfo SET Score =  Score + @Score, InsureScore=InsureScore+@Insure WHERE UserID = @BindUserID
	
	-- �����ο�
	UPDATE AccountsInfo SET Nullity = 1  WHERE UserID=@dwUserID

	-- �����Ϣ
	SET @strErrorDescribe=N'�����ʺųɹ���'
	
	RETURN 0

END

GO

----------------------------------------------------------------------------------------------------

-- �ʺŰ�
CREATE PROC GSP_GP_AccountBindExists
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
		
	@strBindAccounts NVARCHAR(31),				-- ���ʺ�	
	@strBindPassword NCHAR(32),					-- ������		

	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��������
	DECLARE @BindUserID INT
	
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @PlatformID SMALLINT
	DECLARE @LogonPass AS NCHAR(32)

	-- ��ѯ�û�
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity,@PlatformID=PlatformID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

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
	
	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 3
	END
	
	-- �ο��ж�
	IF @PlatformID<>2
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����οͣ����ʺ�ʧ�ܣ�'
		RETURN 4
	END
			
	-- ����
	IF EXISTS(SELECT * FROM AccountsVisitor WHERE VisitorMachine=@strMachineID )
	BEGIN
		SET @strErrorDescribe=N'�˻����Ѿ����ʺţ������ظ��󶨣�'
		return 5
	END
	
	-- ��ѯ�û�
	IF EXISTS(SELECT * FROM AccountsVisitor WHERE BindAccounts=@strBindAccounts )
	BEGIN
		SET @strErrorDescribe=N'���ʺ��Ѿ��󶨣������ٴΰ󶨣�'
		return 6
	END
	
	-- ��ѯ����
	IF NOT EXISTS(SELECT * FROM AccountsInfo WHERE Accounts=@strBindAccounts )
	BEGIN
		SET @strErrorDescribe=N'�ʺŲ����ڣ����ʺ�ʧ�ܣ�'
		return 7
	END	
	
	-- ��ѯ����
	IF NOT EXISTS(SELECT * FROM AccountsInfo WHERE Accounts=@strBindAccounts AND @strBindPassword=LogonPass )
	BEGIN
		SET @strErrorDescribe=N'�ʺ�������󣬰��ʺ�ʧ�ܣ�'
		return 7
	END	
	
	SELECT @BindUserID = UserID   FROM AccountsInfo WHERE Accounts=@strBindAccounts
	
	-- ��¼��
	INSERT INTO AccountsVisitor([VisitorUserID],[VisitorMachine],[BindUserID],[BindAccounts],[BindType]) 
	VALUES (@dwUserID,@strMachineID,@BindUserID,@strBindAccounts,1)
	
	-- �ϲ�����
	DECLARE @Score BIGINT
	DECLARE @Insure BIGINT
			
	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	

	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
		
	-- �ϲ���
	UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score =  Score + @Score, InsureScore=InsureScore+@Insure WHERE UserID = @BindUserID
	
	-- �����ο�
	UPDATE AccountsInfo SET Nullity = 1  WHERE UserID=@dwUserID

	-- �����Ϣ
	SET @strErrorDescribe=N'�����ʺųɹ���'
	
	RETURN 0

END

GO