
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AddUserFriend]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AddUserFriend]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_DeleteUserFriend]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_DeleteUserFriend]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ��Ӻ���
CREATE PROC GSP_GP_AddUserFriend
	@dwUserID INT,								-- �û� I D
	@dwDestUserID INT,							-- �û� I D
	@cbGroupID SMALLINT,						-- ����ʶ
	@cbDestGroupID SMALLINT,					-- ����ʶ
	@cbLoadUserInfo TINYINT,					-- ���ر�ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����ж�
	IF Exists(SELECT * FROM AccountsRelation WHERE MainUserID=@dwUserID AND SubUserID=@dwDestUserID)
	BEGIN
		UPDATE AccountsRelation SET GroupID=@cbGroupID WHERE MainUserID=@dwUserID AND SubUserID=@dwDestUserID
	END
	ELSE BEGIN
		INSERT INTO AccountsRelation VALUES(@dwUserID,@dwDestUserID,@cbGroupID)		
	END

	-- �����ж�
	IF Exists(SELECT * FROM AccountsRelation WHERE MainUserID=@dwDestUserID AND SubUserID=@dwUserID)
	BEGIN
		UPDATE AccountsRelation SET GroupID=@cbDestGroupID WHERE MainUserID=@dwDestUserID AND SubUserID=@dwUserID
	END
	ELSE BEGIN
		INSERT INTO AccountsRelation VALUES(@dwDestUserID,@dwUserID,@cbGroupID)		
	END

	-- �����û���Ϣ
	IF @cbLoadUserInfo=1 
	BEGIN
				
		-- ��ѯ��Ϣ
		SELECT a.UserID,a.NickName,a.FaceID,a.CustomID,a.Gender,a.MemberOrder,a.UnderWrite,a.Compellation,@cbGroupID as GroupID
		FROM AccountsInfo AS a WHERE UserID = @dwDestUserID
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ɾ������
CREATE PROC GSP_GP_DeleteUserFriend
	@dwUserID INT,								-- �û� I D
	@dwDestUserID INT,							-- �û� I D
	@cbGroupID SMALLINT,						-- ����ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��������
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @LogonPass AS NCHAR(32)

	-- ��ѯ�û�
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity
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
	

	-- �Ƴ�����
	DECLARE @cbMainGroupID SMALLINT
	SELECT @cbMainGroupID=GroupID FROM AccountsRelation WHERE MainUserID=@dwDestUserID AND SubUserID=@dwUserID
	DELETE AccountsRelation WHERE MainUserID=@dwUserID AND SubUserID=@dwDestUserID
	DELETE AccountsRelation WHERE MainUserID=@dwDestUserID AND SubUserID=@dwUserID

	SET @strErrorDescribe=N'�Ƴ��ɹ���'


END

RETURN 0

GO
