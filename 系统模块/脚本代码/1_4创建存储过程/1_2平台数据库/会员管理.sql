
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadMemberParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadMemberParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_MemberGiftQuery]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_MemberGiftQuery]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_MemberQueryInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_MemberQueryInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_MemberDayPresent]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_MemberDayPresent]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_MemberDayGift]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_MemberDayGift]
GO



SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ػ�Ա
CREATE PROC GSP_GP_LoadMemberParameter	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ���ػ�Ա
	SELECT * FROM  WHQJAccountsDB.dbo.MemberProperty

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ���
CREATE PROC GSP_GP_MemberGiftQuery	
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32)					-- ������ʶ
WITH ENCRYPTION AS


-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF not exists(SELECT * FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword)
	BEGIN
		return 1
	END

	-- ��Ա�ȼ�
    DECLARE @CurrMemberOrder SMALLINT
    SET   @CurrMemberOrder = 0
    SELECT @CurrMemberOrder=MemberOrder FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
    
    --��ͨ�鿴
    IF @CurrMemberOrder =0
		SET @CurrMemberOrder =1
 
	-- ��ѯ���
    DECLARE @DayGiftID INT
    SET   @DayGiftID = 0
	SELECT @DayGiftID=DayGiftID FROM  WHQJAccountsDB.dbo.MemberProperty where @CurrMemberOrder =MemberOrder

	--�����Ʒ
	SELECT *  FROM GamePropertySub WHERE OwnerID = @DayGiftID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ��Ա
CREATE PROC GSP_GP_MemberQueryInfo
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32)					-- ������ʶ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF not exists(SELECT * FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword)
	BEGIN
		return 1
	END
	
	-- ��Ա�ȼ�
    DECLARE @CurrMemberOrder SMALLINT
    SET   @CurrMemberOrder = 0
    SELECT @CurrMemberOrder=MemberOrder FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
    
	DECLARE @Present INT
	SET @Present = 0	
	DECLARE @Gift INT
	SET @Gift = 0		   
    -- ��Ա�ж�
    IF @CurrMemberOrder >0
    BEGIN
		-- ʱ���ѯ
		DECLARE @TodayDateID INT
		SET @TodayDateID=CAST(CAST(GetDate() AS FLOAT) AS INT) 
		
		-- �ͽ���
		DECLARE @TakeGold INT
		-- �ͽ��¼
		SELECT @TakeGold=TakeGold FROM  WHQJAccountsDB.dbo.AccountsMemberDayPresent WHERE UserID=@dwUserID AND TakeDateID=@TodayDateID
		IF @TakeGold IS NULL 
		BEGIN
			SET @Present=1
		END
		ELSE
		BEGIN
			SET @Present=0
		END

		-- �������
		DECLARE @GiftID INT
		-- �����¼
		SELECT @GiftID=GiftID FROM  WHQJAccountsDB.dbo.AccountsMemberDayGift WHERE UserID=@dwUserID AND TakeDateID=@TodayDateID
		IF @GiftID IS NULL 
		BEGIN
			SET @Gift=1
		END
		ELSE
		BEGIN
			SET @Gift=0
		END   
		
		-- ����ͽ����
		DECLARE @CurrPresentGold INT
		DECLARE @CurrPresentGiftID INT
		SELECT @CurrPresentGold =DayPresent,@CurrPresentGiftID= DayGiftID from  WHQJAccountsDB.dbo.MemberProperty WHERE @CurrMemberOrder = MemberOrder
		IF @CurrPresentGold = 0  SET @Present=0
		IF @CurrPresentGiftID = 0  
		BEGIN
		   SET @Gift=0
		END
		ELSE
		BEGIN
			DECLARE @CurrPresentGiftIDCount INT
			SET @CurrPresentGiftIDCount = 0
			--�����Ʒ
			SELECT @CurrPresentGiftIDCount = count(*)  FROM GamePropertySub WHERE OwnerID = @CurrPresentGiftID	
			IF @CurrPresentGiftIDCount = 0 	 SET @Gift=0
		END

    END

	-- �׳�����
	SELECT @Present AS Present	,@Gift AS Gift
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ա�ͽ�
CREATE PROC GSP_GP_MemberDayPresent
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strNotifyContent NVARCHAR(127) OUTPUT        -- ��ʾ����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF not exists(SELECT * FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword)
	BEGIN
		return 1
	END
	
	-- ʱ���ѯ
	DECLARE @TodayDateID INT
	SET @TodayDateID=CAST(CAST(GetDate() AS FLOAT) AS INT) 
	
	-- �ͽ���
	DECLARE @Present INT
	SET @Present = 0	
	DECLARE @TakeGold INT
	
	-- �ͽ��¼
	SELECT @TakeGold=TakeGold FROM  WHQJAccountsDB.dbo.AccountsMemberDayPresent WHERE UserID=@dwUserID AND TakeDateID=@TodayDateID
	IF @TakeGold IS NULL 
		BEGIN
			SET @Present=1
		END
	ELSE
		BEGIN
			SET @Present=0
		END

	-- �ͽ��ж�
    IF @Present = 0
    BEGIN
        SET @strNotifyContent = N'����������ȡ����ȡʧ�ܣ�'
        return 2        
    END
    

    -- ��Ա�ȼ�
    DECLARE @CurrMemberOrder SMALLINT
    SET   @CurrMemberOrder = 0
    SELECT @CurrMemberOrder=MemberOrder FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
    
    -- ��Ա�ж�
    IF @CurrMemberOrder =0
    BEGIN
        SET @strNotifyContent = N'���ڻ����ǻ�Ա��'
        return 3        
    END
    
    --��ѯ�ͽ�
    DECLARE @lRewardGold BIGINT
    SET @lRewardGold = 0
    SELECT @lRewardGold = DayPresent FROM  WHQJAccountsDB.dbo.MemberProperty where @CurrMemberOrder =MemberOrder

	-- ��ѯ���
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo  WHERE UserID=@dwUserID
	
    -- ��ȡ���    
    UPDATE   WHQJTreasureDB.dbo.GameScoreInfo SET Score = Score + @lRewardGold WHERE UserID = @dwUserID
    
     -- ���¼�¼
    IF @Present=1
	BEGIN
		INSERT INTO  WHQJAccountsDB.dbo.AccountsMemberDayPresent(UserID,TakeDateID,TakeGold) VALUES(@dwUserID,@TodayDateID,@lRewardGold)        
	END 
    
    -- ��ˮ��
	INSERT INTO  WHQJTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
	VALUES (@dwUserID,@CurrScore,@CurrInsure,@lRewardGold,9,@strClientIP,GETDATE())	
	
	-- ��ͳ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE  WHQJTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@lRewardGold WHERE UserID=@dwUserID AND TypeID=9
	IF @@ROWCOUNT=0
	BEGIN
		INSERT  WHQJTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,9,1,@lRewardGold)
	END	
	
    -- ��ѯ���
    DECLARE @Score BIGINT
    SELECT @Score=Score FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID = @dwUserID     

	-- �����ʾ
    SET @strNotifyContent = N'��ϲ������ȡ '+CAST(@lRewardGold AS NVARCHAR)+N' ��Ϸ�ң�'
	
	-- �׳�����
	SELECT @Score AS Score
	
	RETURN 0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ա���
CREATE PROC GSP_GP_MemberDayGift
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strNotifyContent NVARCHAR(127) OUTPUT        -- ��ʾ����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF not exists(SELECT * FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword)
	BEGIN
		return 1
	END
	
	-- ʱ���ѯ
	DECLARE @TodayDateID INT
	SET @TodayDateID=CAST(CAST(GetDate() AS FLOAT) AS INT) 
	
	-- �������
	DECLARE @Gift INT
	SET @Gift = 0
	DECLARE @GiftID INT
	
	-- �����¼
	SELECT @GiftID=GiftID FROM  WHQJAccountsDB.dbo.AccountsMemberDayGift WHERE UserID=@dwUserID AND TakeDateID=@TodayDateID
	IF @GiftID IS NULL 
		BEGIN
			SET @Gift=1
		END
	ELSE
		BEGIN
			SET @Gift=0
		END

	-- ����ж�
    IF @Gift = 0
    BEGIN
        SET @strNotifyContent = N'����������ȡ����ȡʧ�ܣ�'
        return 2        
    END
    

    -- ��Ա�ȼ�
    DECLARE @CurrMemberOrder SMALLINT
    SET   @CurrMemberOrder = 0
    SELECT @CurrMemberOrder=MemberOrder FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
    
    -- ��Ա�ж�
    IF @CurrMemberOrder =0
    BEGIN
        SET @strNotifyContent = N'���ڻ����ǻ�Ա��'
        return 3        
    END
    
    --��ѯ���
    DECLARE @DayGiftID BIGINT
    SET @DayGiftID = 0
    SELECT @DayGiftID = DayGiftID FROM  WHQJAccountsDB.dbo.MemberProperty where @CurrMemberOrder =MemberOrder

    -- ��ѯ���
    DECLARE @PROPERTYID BIGINT
    DECLARE @MemberGiftName NVARCHAR(32)
    SELECT  @PROPERTYID = ID,@MemberGiftName= Name FROM GameProperty WHERE @DayGiftID = ID
    
    IF @PROPERTYID IS NULL
    BEGIN
        SET @strNotifyContent = N'û�ҵ������'
        return 4        
    END
    
    IF @PROPERTYID > 400 AND @PROPERTYID <500
    BEGIN
		--���뱳��
		DECLARE @PropID INT
		DECLARE @PropCount INT
		DECLARE @ItemCount INT
		DECLARE auth_cur CURSOR FOR
		SELECT a.ID,[Count] FROM GamePropertySub AS a, GameProperty AS b WHERE a.ID=b.ID AND a.OwnerID = @PROPERTYID
		OPEN auth_cur
		FETCH NEXT FROM auth_cur INTO @PropID,@PropCount
		WHILE (@@fetch_status=0)
		BEGIN
			SELECT @ItemCount=COUNT(*) FROM  WHQJAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=@PropID
			IF @ItemCount = 0
			BEGIN
				INSERT  WHQJAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsSortID,GoodsCount,PushTime)
				VALUES(@dwUserID,@PropID,0,0,@PropCount,GETDATE())
			END
			ELSE
			BEGIN
				UPDATE  WHQJAccountsDB.dbo.AccountsPackage SET GoodsCount=GoodsCount+@PropCount
				WHERE UserID=@dwUserID and GoodsID=@PropID
			END
			FETCH NEXT FROM auth_cur INTO @PropID,@PropCount
		END
		close auth_cur
		deallocate auth_cur
    END
    ELSE
    BEGIN
		UPDATE  WHQJAccountsDB.dbo.AccountsPackage SET GoodShowID=0, GoodsCount=GoodsCount+1, PushTime=GETDATE() WHERE UserID=@dwUserID AND GoodsID=@PROPERTYID
		IF @@ROWCOUNT =0
		BEGIN
			INSERT  WHQJAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsCount,PushTime)
			VALUES(@dwUserID,@PROPERTYID,0,1,GETDATE())
		END
    END	
	     
     -- ���¼�¼
    IF @Gift=1
	BEGIN
		INSERT INTO  WHQJAccountsDB.dbo.AccountsMemberDayGift(UserID,TakeDateID,GiftID) VALUES(@dwUserID,@TodayDateID,@PROPERTYID)        
	END 
	
	-- �����ʾ
    SET @strNotifyContent = N'��ϲ������ȡ '+@MemberGiftName
	
	RETURN 0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------