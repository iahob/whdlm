
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyLogonPassword]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyLogonPassword]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyInsurePassword]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyInsurePassword]
GO

--��¼֮ǰ�޸�����
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyLogonPasswordH5]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyLogonPasswordH5]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �޸�����
CREATE PROC GSP_GP_ModifyLogonPassword
	@dwUserID INT,								-- �û� I D
	@strSrcPassword NCHAR(32),					-- �û�����
	@strNewPassword NCHAR(32),					-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMobilePhone NVARCHAR(11),				-- �ֻ�����
	@szCheckCode NVARCHAR(15),					-- ��֤��
	@cbPlatform TINYINT,						-- ƽ̨����	1 H5ƽ̨�� ����ƽ̨Ŀǰ����Ҫ�õ��˱�ʶ
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
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @LogonPass AS NCHAR(32)

	-- ��ѯ�û�
	SELECT @UserID=UserID,@InsurePass=InsurePass, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown
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
	
	IF @cbPlatform = 1
	BEGIN
		-- ���У�����Ƿ����
		IF (SELECT COUNT(*) FROM CheckCode(NOLOCK) WHERE PhoneNum = @strMobilePhone AND CheckCode = @szCheckCode)  <= 0
		BEGIN
			SET @strErrorDescribe=N'��֤�벻���ڣ�'
			RETURN 1
		END

		-- �����֤���Ƿ����
		DECLARE @CollectTime DATETIME
		DECLARE @ValidTime INT
		SELECT @ValidTime = StatusValue FROM SystemStatusInfo WHERE StatusName = N'CheckCodeValidTime'
		SELECT @CollectTime = CollectTime FROM CheckCode(NOLOCK) WHERE PhoneNum = @strMobilePhone AND CheckCode = @szCheckCode

		IF DATEDIFF(MI, @CollectTime, GETDATE()) > @ValidTime
		BEGIN
			SET @strErrorDescribe=N'��֤����ڣ������»�ȡ��'
			DELETE FROM CheckCode WHERE PhoneNum = @strMobilePhone AND CheckCode = @szCheckCode
		RETURN 1
		END

	END
	ELSE
	BEGIN
		-- �����ж�
		IF @LogonPass<>@strSrcPassword
		BEGIN
			SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
			RETURN 3
		END
	END



	-- �����ж�
	IF @strNewPassword=N''
	BEGIN
		SET @strErrorDescribe=N'���ʺ�����Ϊ�գ���������������Ϊ�գ����֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- ����У��
	IF @strNewPassword = @InsurePass
	BEGIN
		SET @strErrorDescribe=N'���벻��������������ͬ�����֤���ٴγ��ԣ�'
		RETURN 1		
	END	
	
	-- �޸�����
	UPDATE AccountsInfo SET LogonPass=@strNewPassword WHERE UserID=@dwUserID

	-- �޸������¼
	INSERT INTO WHQJRecordDB.dbo.RecordPasswdExpend(OperMasterID,UserID,ReLogonPasswd,ReInsurePasswd,ClientIP)
	VALUES(0,@dwUserID,@strNewPassword,N'',@strClientIP)

	-- ������Ϣ
	IF @@ERROR=0 
	BEGIN
		SET @strErrorDescribe=N'�ʺ������޸ĳɹ������μ��������ʺ����룡'
	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸�����
CREATE PROC GSP_GP_ModifyInsurePassword
	@dwUserID INT,								-- �û� I D
	@strSrcPassword NCHAR(32),					-- �û�����
	@strNewPassword NCHAR(32),					-- �û�����
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
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @LogonPass AS NCHAR(32)

	-- ��ѯ�û�
	SELECT @UserID=UserID, @InsurePass=InsurePass, @Nullity=Nullity, @StunDown=StunDown ,@LogonPass = LogonPass
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
	IF @InsurePass = N'' 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ������û�����ó�ʼ�������룬���ȿ�ͨ���й��ܣ�'
		RETURN 3
	END
	
	-- �����ж�
	IF @InsurePass IS NOT NULL AND @InsurePass<>@strSrcPassword
	BEGIN
		SET @strErrorDescribe=N'���ľ����������������֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- �����ж�
	IF @strNewPassword=N''
	BEGIN
		SET @strErrorDescribe=N'���ʺ�����Ϊ�գ���������������Ϊ�գ����֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- ����У��
	IF @strNewPassword = @LogonPass
	BEGIN
		SET @strErrorDescribe=N'���벻�����¼������ͬ�����֤���ٴγ��ԣ�'
		RETURN 1		
	END	
	
	-- �޸�����
	UPDATE AccountsInfo SET InsurePass=@strNewPassword WHERE UserID=@dwUserID

	-- �޸������¼
	INSERT INTO WHQJRecordDB.dbo.RecordPasswdExpend(OperMasterID,UserID,ReLogonPasswd,ReInsurePasswd,ClientIP)
	VALUES(0,@dwUserID,N'',@strNewPassword,@strClientIP)

	-- ������Ϣ
	IF @@ERROR=0 
	BEGIN
		SET @strErrorDescribe=N'���������޸ĳɹ������μ������±��չ����룡'
	END

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- �޸�����
CREATE PROC GSP_GP_ModifyLogonPasswordH5
	@szAccounts  NCHAR(32),							-- �û� I D
	@strNewPassword NCHAR(32),					-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMobilePhone NVARCHAR(11),				-- �ֻ�����
	@szCheckCode NVARCHAR(6),					-- ��֤��
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
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @LogonPass AS NCHAR(32)


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

	--�ж��˺��Ƿ����
	SELECT @UserID=UserID
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@szAccounts

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڣ����֤���ٴγ��ԣ�'
		RETURN 1
	END	

	--У���˺ź��ֻ����Ƿ�һ��
	IF @strMobilePhone <> @szAccounts
	BEGIN
		SET @strErrorDescribe=N'�˺Ŵ������飡'
		RETURN 1
	END

	-- ���У�����Ƿ����
	IF (SELECT COUNT(*) FROM CheckCode(NOLOCK) WHERE PhoneNum = @strMobilePhone AND CheckCode = @szCheckCode)  <= 0
	BEGIN
		SET @strErrorDescribe=N'��֤�벻���ڣ�'
		RETURN 1

		-- �����֤���Ƿ����
		DECLARE @CollectTime DATETIME
		DECLARE @ValidTime INT
		SELECT @ValidTime = StatusValue FROM SystemStatusInfo WHERE StatusName = N'CheckCodeValidTime'
		SELECT @CollectTime = CollectTime FROM CheckCode(NOLOCK) WHERE PhoneNum = @strMobilePhone AND CheckCode = @szCheckCode

		IF DATEDIFF(MI, @CollectTime, GETDATE()) > @ValidTime
		BEGIN
			SET @strErrorDescribe=N'��֤����ڣ������»�ȡ��'
			DELETE FROM CheckCode WHERE PhoneNum = @strMobilePhone AND CheckCode = @szCheckCode
			RETURN 1
		END
	END

	-- �����ж�
	IF @strNewPassword=N''
	BEGIN
		SET @strErrorDescribe=N'���ʺ�����Ϊ�գ���������������Ϊ�գ����֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- �޸�����
	UPDATE AccountsInfo SET LogonPass=@strNewPassword WHERE Accounts=@strMobilePhone


	-- ������Ϣ
	IF @@ERROR=0 
	BEGIN
		SET @strErrorDescribe=N'�ʺ������޸ĳɹ������μ��������ʺ����룡'
	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
