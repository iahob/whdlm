USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UseTrumpet]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UseTrumpet]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


CREATE PROC GSP_GP_UseTrumpet
	@dwUserID INT,				-- �û� I D
	@strClientIP NVARCHAR(15),	-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	DECLARE @PropName nvarchar(31)
	DECLARE @GoodsCount INT 
	DECLARE @PropCount  INT
	DECLARE @PropID	 INT	
	DECLARE @SendLoveLiness INT 
	DECLARE @RecvLoveLiness INT
	DECLARE @CurGoodsCount INT 
	
	SET @PropName = '������'
	SET @GoodsCount = 0	
	SET @PropCount = 1	
	SET @PropID = 306	
	SET @SendLoveLiness = 0	
	SET @RecvLoveLiness = 0	
	SET @CurGoodsCount = 0
			
	-- ��ѯ����
	SELECT @GoodsCount = GoodsCount FROM  WHQJAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=@PropID
	IF @GoodsCount = 0 
	BEGIN
		set @strErrorDescribe=N'����û�и������ĵ��ߣ� �޷�ʹ��'
		RETURN 1
	END
	
	--���ٵ���
	SET @CurGoodsCount= @GoodsCount-@PropCount
	if @CurGoodsCount > 0
		update  WHQJAccountsDB.dbo.AccountsPackage set GoodsCount = @CurGoodsCount where UserID=@dwUserID and GoodsID=@PropID
	else 
		delete  WHQJAccountsDB.dbo.AccountsPackage where UserID=@dwUserID and GoodsID=@PropID
	
		
	--���߼�¼
	insert WHQJRecordDB.dbo.RecordUseProperty(SourceUserID, TargetUserID, 
	PropertyID, PropertyName, PropertyCount, LovelinessRcv, LovelinessSend, 
	KindID, ServerID, ClientIP, UseDate)
	values(@dwUserID, 0, @PropID, @PropName, @PropCount, 
	@RecvLoveLiness, @SendLoveLiness, 0, 0, @strClientIP, GETDATE())
	
	SET @strErrorDescribe=N'������ʹ�óɹ�'
END

RETURN 0

GO
