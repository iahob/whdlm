
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO



IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RealAuth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RealAuth]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- ʵ����֤
CREATE PROC GSP_GP_RealAuth
	@dwUserID INT,								-- �û� I D	
	@strPassword NCHAR(32),						-- �û�����
	@strCompellation NVARCHAR(16),				-- ��ʵ����
	@strPassPortID NVARCHAR(18),				-- ֤������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ��Ϣ
	DECLARE @DynamicPass NCHAR(32)
	SELECT @DynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@dwUserID

	-- �û��ж�
	IF @DynamicPass IS NULL
	BEGIN 
		SET @strErrorDescribe=N'�û���Ϣ�����ڣ���֤ʧ�ܣ�'
		RETURN 1
	END

	-- �����ж�
	IF @DynamicPass<>@strPassword
	BEGIN 
		SET @strErrorDescribe=N'�ʺ����벻��ȷ����֤ʧ�ܣ�'
		RETURN 3
	END

	--����֤
	IF LEN(@strPassPortID) = 0 OR LEN(@strCompellation) = 0
	BEGIN
		SET @strErrorDescribe=N'��֤ʧ�ܣ���������Ч��֤�����룡'
		RETURN 4	
	END	
	
	--�ظ���֤
	IF EXISTS (SELECT UserID FROM WHQJRecordDB.dbo.RecordAuthentPresent WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe=N'����֤�ɹ��������ظ�����֤��'
		RETURN 5		
	END
	
	-- ʵ���޸�
	UPDATE AccountsInfo SET  Compellation=@strCompellation, PassPortID=@strPassPortID 	WHERE UserID=@dwUserID
	
	-- �ɹ��ж�
	IF @@ROWCOUNT=0
	BEGIN
		SET @strErrorDescribe=N'ִ��ʵ����֤��������ϵ�ͻ��������ģ�'
		RETURN 6
	END
	
	-- ʵ����¼
	INSERT INTO WHQJRecordDB.dbo.RecordAuthentPresent(UserID,PassPortID,Compellation,IpAddress) VALUES(@dwUserID,@strPassPortID,@strCompellation,@strClientIP)	
	
	--��ѯ����
	DECLARE @SourceDiamond BIGINT
	DECLARE @AuthentPresentCount AS BIGINT
	SELECT @AuthentPresentCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'JJRealNameAuthentPresent'
	IF @AuthentPresentCount IS NULL or @AuthentPresentCount=0
	BEGIN
		SET @strErrorDescribe=N'��ϲ������֤�ɹ���'
		
		SELECT @SourceDiamond=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID	
		SELECT @SourceDiamond AS Diamond
		
		RETURN 0		
	END	
	
		
	--���Ž���
	--DECLARE @DateID INT
	--SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	-- ������ʯ
	UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@AuthentPresentCount WHERE UserID=@dwUserID	
	SELECT @SourceDiamond=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
	
	--������־
	DECLARE @DateTime DATETIME
	--DECLARE @Random VARCHAR(5)
	DECLARE @SerialNumber NVARCHAR(20)
	SET @DateTime = GETDATE()
	--SET @Random = CAST(FLOOR(89999*RAND()+10000) AS VARCHAR(5))
	SELECT @SerialNumber=dbo.WF_GetSerialNumber()
	
	INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
	VALUES(@SerialNumber,0,@dwUserID,6,@SourceDiamond-@AuthentPresentCount,@AuthentPresentCount,@strClientIP,@DateTime)
	-- ��ʯ�仯��¼
	--INSERT INTO WHQJRecordDB..RecordGrantDiamond(MasterID, UserID, TypeID, CurDiamond, AddDiamond, ClientIP, CollectDate, CollectNote)
	--	VALUES (0, @dwUserID, 1, 0, @AuthentPresentCount, @strClientIP, GETDATE(), 'ʵ��������ʯ')
	---- ��ˮ��
	--INSERT INTO  WHQJTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
	--VALUES (@dwUserID,@CurrScore,@CurrInsure,@AuthentPresentCount,8,@strClientIP,GETDATE())
	
	-- ��ͳ��
	--UPDATE  WHQJTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@AuthentPresentCount WHERE UserID=@dwUserID AND TypeID=8
	--IF @@ROWCOUNT=0
	--BEGIN
	--	INSERT  WHQJTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,8,1,@AuthentPresentCount)
	--END	
	---- ��Ϸ��Ϣ
	--DECLARE @SourceScore BIGINT
	--SELECT @SourceScore=Score FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID	
	--SELECT @SourceScore AS SourceScore
	
	---- ��Ϸ��Ϣ			
	SELECT @SourceDiamond AS Diamond
	SET @strErrorDescribe=N'��ϲ������֤�ɹ���������ʯ'+ CAST(@AuthentPresentCount AS NVARCHAR)  +N'��'							
	
	RETURN 0

END

GO

----------------------------------------------------------------------------------------------------
