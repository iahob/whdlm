
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_EfficacyAccountsChat]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_EfficacyAccountsChat]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadUserGroups]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadUserGroups]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadUserFriends]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadUserFriends]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadFriendRemarks]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadFriendRemarks]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �ʺŵ�¼
CREATE PROC GSP_GP_EfficacyAccountsChat
	@dwUserID INT,								-- �û���ʶ
	@strPassword NCHAR(33),						-- �û�����
	@strClientIP nvarchar(15),	                -- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @GameID INT
DECLARE @NickName NVARCHAR(31)
DECLARE @FaceID INT
DECLARE @CustomID INT
DECLARE @Gender TINYINT
DECLARE @MemberOrder INT
DECLARE @GrowLevel INT
DECLARE @UnderWrite NVARCHAR(32)
DECLARE @Compellation NVARCHAR(16)

--�û�����
DECLARE @QQ NVARCHAR(16)
DECLARE @EMail NVARCHAR(16)
DECLARE @SeatPhone NVARCHAR(32)
DECLARE @MobilePhone NVARCHAR(16)
DECLARE @DwellingPlace NVARCHAR(128)
DECLARE @PostalCode NVARCHAR(8)

--ͷ��URL
DECLARE @FaceUrl NVARCHAR(500)

-- ִ���߼�
BEGIN

	SET @GameID = 0
	SET @UnderWrite=N''
	SET @Compellation=N''
    SET @QQ =N''
    SET @EMail =N''
    SET @SeatPhone =N''
    SET @MobilePhone =N''
    SET @DwellingPlace =N''
    SET @PostalCode =N''


	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	SELECT @UserID= UserID ,@GameID=GameID,@NickName = NickName, @LogonPass= LogonPass , @FaceID=  FaceID,@CustomID= CustomID,@Gender=Gender,
	@MemberOrder =MemberOrder,@UnderWrite=UnderWrite,@Compellation =Compellation FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	--ͷ���ַ
	SELECT @FaceUrl = FaceUrl 	FROM  AccountsFace  WHERE UserID=@UserID
	IF @FaceUrl IS NULL
	SET @FaceUrl = N''
			
	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
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
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 3
	END
	SELECT @QQ = QQ,@EMail = EMail,@SeatPhone = SeatPhone,@MobilePhone = MobilePhone,@DwellingPlace = DwellingPlace ,
	@PostalCode =PostalCode FROM IndividualDatum(NOLOCK)WHERE UserID=@dwUserID
	
	-- �������
	SELECT @UserID AS UserID,@GameID AS GameID, @NickName AS NickName, @FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @MemberOrder AS MemberOrder,
	@UnderWrite AS UnderWrite,@Compellation AS Compellation,@QQ AS QQ, @EMail AS EMail, @SeatPhone AS SeatPhone, @MobilePhone AS MobilePhone,
	@DwellingPlace AS DwellingPlace,@PostalCode AS PostalCode,@GrowLevel AS GrowLevel, @FaceUrl AS FaceUrl

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- ���ط���
CREATE PROC GSP_GP_LoadUserGroups
	@dwUserID INT,								-- �û���ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN 
	-- ������Ϣ
	SELECT GroupID,GroupName FROM AccountsGroup WHERE UserID=@dwUserID ORDER BY GroupID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���غ���
CREATE PROC GSP_GP_LoadUserFriends
	@dwUserID INT,								-- �û���ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN 

	-- ��ѯ��Ϣ
	SELECT a.UserID,a.GameID,a.NickName,a.FaceID,a.CustomID,a.Gender,a.MemberOrder,ISNULL(a.UnderWrite,N'') AS UnderWrite,ISNULL(a.Compellation,N'') AS Compellation,
	b.GroupID,ISNULL(c.QQ,N'') AS QQ,ISNULL(c.EMail,N'') AS EMail,ISNULL(c.SeatPhone,N'')  AS SeatPhone,ISNULL(c.MobilePhone,N'')AS MobilePhone,
	ISNULL(c.DwellingPlace,N'') AS DwellingPlace,ISNULL(c.PostalCode,N'')   AS PostalCode
	FROM AccountsInfo a 
	INNER JOIN AccountsRelation b ON b.MainUserID=@dwUserID	AND b.SubUserID=a.UserID 
	LEFT OUTER JOIN IndividualDatum c ON c.UserID=a.UserID ORDER BY a.UserID
END

GO

----------------------------------------------------------------------------------------------------

-- ���غ���
CREATE PROC GSP_GP_LoadFriendRemarks
	@dwUserID INT,								-- �û���ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN 
	
	DECLARE @UserNote NVARCHAR(256)
	SET @UserNote = ''
	-- ��ѯ��Ϣ
	SELECT @UserNote= UserNote FROM IndividualDatum WHERE UserID=@dwUserID
	SELECT @dwUserID AS UserID,@UserNote AS UserNote

END

GO



