
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryUserGameData]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryUserGameData]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteUserGameData]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteUserGameData]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSF_GR_ExtractValue]') and OBJECTPROPERTY(ID, N'IsScalarFunction') = 1)
DROP FUNCTION [dbo].[GSF_GR_ExtractValue]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ������Ϸ����
CREATE PROC GSP_GR_QueryUserGameData
	@wKindID SMALLINT,							-- ��Ϸ��ʶ
	@dwUserID INT,								-- �û���ʶ
	@strDynamicPass NCHAR(32),					-- �û�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	DECLARE @strPassword NCHAR(32)
	DECLARE @IsAndroid INT
	IF NOT EXISTS ( SELECT * FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND IsAndroid =1)
	BEGIN
		SELECT @strPassword=DynamicPass FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
		IF @strPassword IS NULL
		BEGIN
			SET @strErrorDescribe=N'��Ǹ�����ĸ�����Ϣ�����쳣���齱���ò�ѯʧ�ܣ�'
			RETURN 1
		END

		IF @strPassword<>@strDynamicPass
		BEGIN
			SET @strErrorDescribe=N'��Ǹ�����ĵ�¼���벻��ȷ����Ϸ���ݲ�ѯʧ�ܣ�'
			RETURN 2
		END
	END

	-- ������
	IF NOT EXISTS (SELECT * FROM UserGameInfo(NOLOCK) WHERE KindID=@wKindID AND UserID=@dwUserID)
	BEGIN
		INSERT INTO UserGameInfo (UserID,KindID,UserGameData,LastModifyDate)
		VALUES (@dwUserID,@wKindID,N'',GETDATE())
	END

	SELECT UserGameData FROM UserGameInfo(NOLOCK) WHERE KindID=@wKindID AND UserID=@dwUserID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- д����Ϸ����
CREATE PROC GSP_GR_WriteUserGameData
	@wKindID SMALLINT,							-- ��Ϸ��ʶ
	@dwUserID INT,								-- �û���ʶ
	@strUserGameData NVARCHAR(1024)				-- ��Ϸ����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��������
	IF @wKindID=508
	BEGIN
		DECLARE @nLineGrandTotal INT
		DECLARE @nLineWinMax INT
		SET @nLineGrandTotal=dbo.GSF_GR_ExtractValue(@strUserGameData,N'2:')
		SET @nLineWinMax=dbo.GSF_GR_ExtractValue(@strUserGameData,N'3:')

		-- ��Ϸ����
		UPDATE UserGameInfo SET UserGameData=@strUserGameData,LineGrandTotal=@nLineGrandTotal,LineWinMax=@nLineWinMax,LastModifyDate=GETDATE() WHERE KindID=@wKindID AND UserID=@dwUserID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO UserGameInfo (UserID,KindID,UserGameData,LineGrandTotal,LineWinMax,LastModifyDate)
			VALUES (@dwUserID,@wKindID,@strUserGameData,@nLineGrandTotal,@nLineWinMax,GETDATE())
		END

		-- ÿ�ռ�¼
		UPDATE UserGameInfo_Line SET LineGrandTotal=@nLineGrandTotal,LineWinMax=@nLineWinMax,LastModifyDate=GETDATE() WHERE DateID=@DateID AND UserID=@dwUserID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO UserGameInfo_Line (UserID,DateID,LineGrandTotal,LineWinMax,LastModifyDate)
			VALUES (@dwUserID,@DateID,@nLineGrandTotal,@nLineWinMax,GETDATE())
		END
	END

	-- ������Ϸ
	ELSE
	BEGIN
		UPDATE UserGameInfo SET UserGameData=@strUserGameData,LastModifyDate=GETDATE() WHERE KindID=@wKindID AND UserID=@dwUserID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT INTO UserGameInfo (UserID,KindID,UserGameData,LastModifyDate)
			VALUES (@dwUserID,@wKindID,@strUserGameData,GETDATE())
		END
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
 ---����ʱ��ֵ����ʱ��
CREATE FUNCTION GSF_GR_ExtractValue (
	@strString AS NVARCHAR(1024), 
	@strSplit NVARCHAR(10))
RETURNS  INT
AS  
BEGIN 

	DECLARE @nStartPos INT
	DECLARE @nEndPos INT
	SET @nStartPos=CHARINDEX(@strSplit,@strString,0);
	SET @nEndPos=CHARINDEX(N';',@strString,@nStartPos);

	DECLARE @nValue INT
	IF @nStartPos>0 AND @nEndPos>0
	BEGIN
		DECLARE @strValue NVARCHAR(64)
		SET @strValue=SUBSTRING(@strString,@nStartPos+LEN(@strSplit),(@nEndPos-@nStartPos-LEN(@strSplit)))	
		SET @nValue=CONVERT(INT,@strValue)
	END

	IF @nValue IS NULL SET @nValue=0

	return @nValue

END