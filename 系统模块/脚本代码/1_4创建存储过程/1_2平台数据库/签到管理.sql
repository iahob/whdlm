
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadSignInReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadSignInReward]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadPackageInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadPackageInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SignInQueryInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SignInQueryInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SignInDone]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SignInDone]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SignInReword]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SignInReword]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SignInCycReword]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SignInCycReword]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ؽ���
CREATE PROC GSP_GP_LoadSignInReward
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT A.PackageID, A.Name,A.PlatformKind,B.TypeID,B.NeedDay,B.Probability,B.SortID FROM GamePackage AS A,GameSignIn AS B  
	WHERE B.TypeID<=1 AND B.Nullity=0 AND A.PackageID=B.PackageID ORDER BY B.SortID ASC  

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- �������
CREATE PROC GSP_GP_LoadPackageInfo
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	--�������
	DECLARE @dwPackageID INT
	DECLARE @dwGoodsCount INT
	DECLARE @strbuf NVARCHAR(100)
	DECLARE @ResourceURL NVARCHAR(100)
	CREATE TABLE #TmpTable (PackageID INT , PackageInfo NVARCHAR(100) , GoodsCount INT , ResourceURL NVARCHAR(100))
	--�����α�
	DECLARE MyCursor CURSOR    
		FOR SELECT  PackageID FROM GamePackage WHERE Nullity=0
	 
	--���α�    
	OPEN MyCursor
	 
	--ѭ���α�
	FETCH NEXT FROM  MyCursor INTO  @dwPackageID
	WHILE @@FETCH_STATUS =0
		BEGIN	
			--��ȡ������Ϣ	
			SELECT @strbuf=STUFF(   (SELECT '��'+(case TypeID when 0 then '���*' when 1 then '��ʯ*' when 3 then '����*'end)				 
											  +convert(nvarchar,GoodsNum)
									 FROM GamePackageGoods  WHERE PackageID=@dwPackageID FOR XML PATH('')),
								 1,1,'') 
							
			--��ȡ������Ϣ							 
			IF EXISTS (SELECT 1  FROM GameProperty AS A,GamePackageGoods AS B WHERE B.PackageID=@dwPackageID AND B.TypeID=2 AND B.PropertyID=A.ID)
			BEGIN
				IF @strbuf IS NULL SET @strbuf=N''
				ELSE SET @strbuf=@strbuf+'��'
				
				SELECT @strbuf=@strbuf+STUFF( (SELECT TOP 1 '��'+A.Name+'*'+convert(nvarchar,B.GoodsNum) 
													FROM GameProperty AS A,GamePackageGoods AS B 
													 WHERE B.PackageID=@dwPackageID AND B.TypeID=2 AND B.PropertyID=A.ID FOR XML PATH('')),
												  1,1,'') 
			END
			
	
			
			-- ������ֵ
			IF @strbuf IS NULL	SET @strbuf='��'
			ELSE 
			BEGIN
				--��ȡ������Ŀ
				SELECT @dwGoodsCount=COUNT(PackageID) FROM GamePackageGoods WHERE PackageID=@dwPackageID
				IF @dwGoodsCount=1 
				BEGIN
					SELECT TOP 1 @ResourceURL=ResourceURL FROM GamePackageGoods WHERE PackageID=@dwPackageID
				END
				ELSE SET @ResourceURL=''
			END
			--�����¼
			INSERT INTO #TmpTable(PackageID,PackageInfo,GoodsCount,ResourceURL)
			VALUES (@dwPackageID,@strbuf,@dwGoodsCount,@ResourceURL)
							
			--��һ����¼
			FETCH NEXT FROM  MyCursor INTO  @dwPackageID
		END    
	 
	--�ر��α�
	CLOSE MyCursor
	--�ͷ��α�
	DEALLOCATE MyCursor
	
	SELECT * FROM #TmpTable
	
	--ɾ����ʱ��
	IF EXISTS (select * from tempdb.dbo.sysobjects where id = object_id(N'tempdb..#TmpTable') and type='U')
	DROP TABLE #TmpTable

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��ѯǩ��
CREATE PROC GSP_GR_SignInQueryInfo
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF NOT EXISTS (SELECT UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND DynamicPass=@strPassword)
	BEGIN
		SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END

	-- ǩ����¼
	DECLARE @wSeriesDate INT
	DECLARE @wSignInCycDate INT 
	DECLARE @StartDateTime DateTime
	DECLARE @LastDateTime DateTime
	DECLARE @GotPackage NVARCHAR(32)
	--��ѯ��¼
	SELECT @StartDateTime=StartDateTime,@LastDateTime=LastDateTime,@wSeriesDate=SeriesDate,@GotPackage=GotPackageInfo FROM  WHQJAccountsDB.dbo.AccountsSignin 	
	WHERE UserID=@dwUserID
	
	--��ʼ����
	IF @StartDateTime IS NULL OR @LastDateTime IS NULL OR @wSeriesDate IS NULL
	BEGIN
		SELECT @StartDateTime=GetDate(),@LastDateTime=GetDate(),@wSeriesDate=0,@GotPackage= N''
		INSERT INTO  WHQJAccountsDB.dbo.AccountsSignin VALUES(@dwUserID,@StartDateTime,@LastDateTime,0,@GotPackage)		
	END

	--��ȡ����
	SELECT TOP 1 @wSignInCycDate=NeedDay FROM GameSignIn WHERE TypeID=1 AND Nullity=0 ORDER BY NeedDay DESC  

	--����ǩ��
	IF @wSeriesDate = @wSignInCycDate  AND  DateDiff(dd,@LastDateTime,GetDate())>0 OR @wSeriesDate>@wSignInCycDate
	BEGIN			
		SET @wSeriesDate = 0
		SET @GotPackage=N''
		UPDATE  WHQJAccountsDB.dbo.AccountsSignin SET StartDateTime=GetDate(),LastDateTime=GetDate(),SeriesDate=0 ,GotPackageInfo=N'' WHERE UserID=@dwUserID									
	END

	-- �����ж�
	DECLARE @TodaySignIned TINYINT
	SET @TodaySignIned = 0
	
	--��ǩ��
	IF DateDiff(dd,@LastDateTime,GetDate()) = 0 	
	BEGIN
		IF @wSeriesDate > 0 SET @TodaySignIned = 1
	END ELSE
	BEGIN		
		--����ǩ���Ͻ� ����ǩ��
		IF DateDiff(dd,@StartDateTime,GetDate()) <> @wSeriesDate OR @wSeriesDate >= @wSignInCycDate 
		BEGIN
			SET @wSeriesDate = 0
			SET @GotPackage=N''
			UPDATE  WHQJAccountsDB.dbo.AccountsSignin SET StartDateTime=GetDate(),LastDateTime=GetDate(),SeriesDate=0 ,GotPackageInfo=N'' WHERE UserID=@dwUserID									
		END
	END

	-- �׳�����
	SELECT @wSeriesDate AS SeriesDate,@TodaySignIned AS TodaySignIned,@GotPackage AS GotPackage
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ǩ��ִ��
CREATE PROC GSP_GR_SignInDone
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF NOT EXISTS(SELECT UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND DynamicPass=@strPassword)
	BEGIN
		SET @strErrorDescribe = N'�û���Ϣ�����ڻ������벻��ȷ��'
		RETURN 1
	END
	
	-- ǩ����¼
	DECLARE @wSeriesDate INT
	DECLARE @wSignInCycDate INT 	
	DECLARE @StartDateTime DateTime
	DECLARE @LastDateTime DateTime
	DECLARE @GotPackage NVARCHAR(32)
	--��ѯ��¼
	SELECT @StartDateTime=StartDateTime,@LastDateTime=LastDateTime,@wSeriesDate=SeriesDate,@GotPackage=GotPackageInfo FROM  WHQJAccountsDB.dbo.AccountsSignin 	
	WHERE UserID=@dwUserID
	
	--��ʼ����
	IF @StartDateTime IS NULL OR @LastDateTime IS NULL OR @wSeriesDate IS NULL
	BEGIN
		SELECT @StartDateTime=GetDate(),@LastDateTime=GetDate(),@wSeriesDate=0,@GotPackage= N''
		INSERT INTO  WHQJAccountsDB.dbo.AccountsSignin VALUES(@dwUserID,@StartDateTime,@LastDateTime,0,@GotPackage)		
	END

	--ǩ���ж�
	IF DateDiff(dd,@LastDateTime,GetDate()) = 0 AND @wSeriesDate > 0
	BEGIN
		SET @strErrorDescribe = N'��Ǹ���������Ѿ�ǩ���ˣ�'
		return 2		
	END

	--��ȡ����
	SELECT TOP 1 @wSignInCycDate=NeedDay FROM GameSignIn WHERE TypeID=1 AND Nullity=0 ORDER BY NeedDay DESC  

	-- ����Խ��
	IF @wSeriesDate > @wSignInCycDate
	BEGIN
		SET @strErrorDescribe = N'����ǩ����Ϣ�����쳣���������ǵĿͷ���Ա��ϵ��'
		return 3				
	END
	
	SELECT @wSeriesDate AS wSeriesDate
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ǩ������
CREATE PROC GSP_GR_SignInReword
	@dwUserID INT,								-- �û� I D
	@dwPackageID INT,							-- �������
	@dwProbability INT,							-- �������
	@dwSeriesDate INT,							-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ�û�
	IF NOT EXISTS(SELECT UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe = N'�û���Ϣ�����ڻ������벻��ȷ��'
		RETURN 1
	END
	
	-- ��ѯ���
	DECLARE @PackageName NVARCHAR(30)
	SELECT @PackageName=Name FROM GamePackage WHERE PackageID=@dwPackageID AND Nullity=0
	
	IF @PackageName IS NULL
	BEGIN
		SET @strErrorDescribe = N'������ڴ���У����Ժ������ԣ�'
		RETURN 1
	END
	
	-- ����ǩ����¼
	SET @dwSeriesDate = @dwSeriesDate+1
	UPDATE  WHQJAccountsDB.dbo.AccountsSignin SET LastDateTime = GetDate(),SeriesDate = @dwSeriesDate WHERE UserID = @dwUserID

	--��ȡ�����Ʒ
	DECLARE @wTypeID TINYINT
	DECLARE @dwPropertyID INT
	DECLARE @dwGoodsNum INT
	
	--�����¼
	DECLARE @PackageInfo NVARCHAR(128)
	SET @PackageInfo=''
	
	--��ˮ��
	DECLARE @SerialNumber NVARCHAR(20)

	--�����α�
	DECLARE MyCursor CURSOR    
		FOR SELECT TypeID, PropertyID,GoodsNum FROM GamePackageGoods WHERE PackageID=@dwPackageID
	 
	--���α�    
	OPEN MyCursor
	 
	--ѭ���α�
	FETCH NEXT FROM  MyCursor INTO @wTypeID, @dwPropertyID,@dwGoodsNum
	WHILE @@FETCH_STATUS =0
		BEGIN
			--���
			IF @wTypeID=0
			BEGIN

				DECLARE @CurScore AS BIGINT
				DECLARE @CurInsureScore AS BIGINT
				--��ȡ����
				SELECT @CUrScore=Score,@CurInsureScore=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

			
				-- ��������
				UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score = @dwGoodsNum + Score WHERE UserID=@dwUserID
		
				--������־
				SELECT @SerialNumber=dbo.WF_GetSerialNumber()
				

				
				--�����ˮ
				IF @CurScore IS NOT NULL AND @CurInsureScore IS NOT NULL
				BEGIN
					INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
					VALUES(@SerialNumber,0,@dwUserID,14,@CurScore,@CurInsureScore,@dwGoodsNum,@strClientIP,GETDATE())	
				END
				
				--�����Ʒ��¼
				SET @PackageInfo=@PackageInfo+'0:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--��ʯ
			ELSE IF @wTypeID=1
			BEGIN	
			
				DECLARE @TotalDiamond BIGINT
				-- ��ȡ��ʯ
				SELECT @TotalDiamond=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
						
				-- ��������
				UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@dwGoodsNum WHERE UserID=@dwUserID
				
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket)	VALUES(@dwUserID, @dwGoodsNum,0)
				END
							

				--������־
				SELECT @SerialNumber=dbo.WF_GetSerialNumber()
				--��ʯ��ˮ
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,16,@TotalDiamond,@dwGoodsNum,@strClientIP,GETDATE())
				
				--�����Ʒ��¼
				SET @PackageInfo=@PackageInfo+'1:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--����
			ELSE IF @wTypeID=2
			BEGIN
				--���뱳��
				UPDATE  WHQJAccountsDB.dbo.AccountsPackage SET GoodShowID=0, GoodsCount=GoodsCount+ @dwGoodsNum, PushTime=GETDATE() WHERE UserID=@dwUserID AND GoodsID=@dwPropertyID
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsCount,PushTime)	VALUES(@dwUserID, @dwPropertyID,0,@dwGoodsNum,GETDATE())
				END
				
				--�����Ʒ��¼
				SET @PackageInfo=@PackageInfo+'2:'+CONVERT(NVARCHAR,@dwPropertyID)+':'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--����
			ELSE IF @wTypeID=3
			BEGIN

				--������¼
				DECLARE @TotalTicket BIGINT
				-- ��ȡ����
				SELECT @TotalTicket=AwardTicket FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID

				-- ��������
				UPDATE  WHQJTreasureDB.dbo.UserCurrency SET AwardTicket=AwardTicket+@dwGoodsNum WHERE UserID=@dwUserID
				
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket)	VALUES(@dwUserID, 0,@dwGoodsNum)
				END
				
				-- ������¼
				INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
				VALUES (dbo.WF_GetSerialNumber(),0,@dwUserID,1,@TotalTicket-@dwGoodsNum,@dwGoodsNum,@strClientIP,GETDATE())
				
				--�����Ʒ��¼
				SET @PackageInfo=@PackageInfo+'3:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'			
			END
			
			FETCH NEXT FROM  MyCursor INTO @wTypeID, @dwPropertyID,@dwGoodsNum
		END    
	 
	--�ر��α�
	CLOSE MyCursor
	--�ͷ��α�
	DEALLOCATE MyCursor
	
	--������־
	INSERT INTO WHQJRecordDB.dbo.RecordGameSignIn(UserID,SignType,PackageName,PackageGoods,Probability,NeedDay,TotalDay,ClinetIP,CollectDate)
	VALUES (@dwUserID,0,@PackageName,@PackageInfo,@dwProbability,1,@dwSeriesDate,@strClientIP,GETDATE())
	
	--�����Ϣ
	DECLARE @Score BIGINT
	DECLARE @Diamond BIGINT
	DECLARE @AwardTicket INT
	
	--��ѯ�Ƹ�
	SELECT @Score=Score FROM WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	SELECT @Diamond=Diamond, @AwardTicket=AwardTicket FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
	
	IF @Score IS NULL SET @Score=0
	IF @Diamond IS NULL SET @Diamond=0
	IF @AwardTicket IS NULL SET @AwardTicket=0
	
	SELECT @Score AS Score,@Diamond AS Diamond,@AwardTicket AS AwardTicket
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����ǩ������
CREATE PROC GSP_GR_SignInCycReword
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@dwPackageID INT,							-- �������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ�û�
	IF NOT EXISTS(SELECT UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND DynamicPass=@strPassword)
	BEGIN
		SET @strErrorDescribe = N'�û���Ϣ�����ڻ������벻��ȷ��'
		RETURN 1
	END
	
	-- ��ѯ���
	DECLARE @PackageName NVARCHAR(30)
	SELECT @PackageName=Name FROM GamePackage WHERE PackageID=@dwPackageID AND Nullity=0
	
	IF @PackageName IS NULL
	BEGIN
		SET @strErrorDescribe = N'������ڴ���У����Ժ������ԣ�'
		RETURN 1
	END
	
	--��ѯ�������ǩ��ʱ��
	DECLARE @wSeriesDate INT
	DECLARE @GotPackageInfo NVARCHAR(32)
	SELECT @wSeriesDate=SeriesDate ,@GotPackageInfo=GotPackageInfo FROM  WHQJAccountsDB.dbo.AccountsSignin WHERE UserID=@dwUserID	
	
	--��ʼ����
	IF @wSeriesDate IS NULL
	BEGIN
		INSERT INTO  WHQJAccountsDB.dbo.AccountsSignin 
		VALUES(@dwUserID,GETDATE(),GETDATE(),0,N'')	
		
		SET @wSeriesDate=0
		SET @GotPackageInfo=N''
	END
	
	--��ȡ�����Ҫ��ʱ��
	DECLARE @wNeedDate INT
	SELECT TOP 1 @wNeedDate=NeedDay FROM GameSignIn WHERE PackageID=@dwPackageID AND TypeID=1 AND Nullity=0 ORDER BY NeedDay DESC  

	IF @wSeriesDate<@wNeedDate
	BEGIN
		SET @strErrorDescribe = N'��Ǹ��������ǩ��ʱ�䲻����������ȡ�����'
		RETURN 3
	END

	--��ѯ�Ƿ�����
	DECLARE @strPackageID NVARCHAR(32)
	DECLARE @nCharIndex1 INT
	DECLARE @nCharIndex2 INT
	
	SET @strPackageID = CONVERT(NVARCHAR,@wNeedDate)+N';'
	SET @nCharIndex1=0
	SET @nCharIndex2=0
	
	SELECT @nCharIndex1=CHARINDEX(@strPackageID, @GotPackageInfo)
	SELECT @nCharIndex2=CHARINDEX(N';'+@strPackageID, @GotPackageInfo)
	
	IF @nCharIndex1=1 OR @nCharIndex2>0
	BEGIN
		SET @strErrorDescribe = N'��Ǹ��������ȡ������������ظ���ȡ�����'
		RETURN 4
	END
	
	--��ȡ�����Ʒ	
	DECLARE @wTypeID TINYINT
	DECLARE @dwPropertyID INT
	DECLARE @dwGoodsNum INT

	--�����¼
	DECLARE @PackageInfo NVARCHAR(128)
	SET @PackageInfo=''
	
	--��ˮ��
	DECLARE @SerialNumber NVARCHAR(20)

	--�����α�
	DECLARE MyCursor CURSOR    
		FOR SELECT TypeID, PropertyID,GoodsNum FROM GamePackageGoods WHERE PackageID=@dwPackageID
	 
	--���α�    
	OPEN MyCursor
	 
	--ѭ���α�
	FETCH NEXT FROM  MyCursor INTO @wTypeID, @dwPropertyID,@dwGoodsNum
	WHILE @@FETCH_STATUS =0
		BEGIN
			--���
			IF @wTypeID=0
			BEGIN
			
				DECLARE @CurScore AS BIGINT
				DECLARE @CurInsureScore AS BIGINT
				--��ȡ����
				SELECT @CUrScore=Score,@CurInsureScore=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

				-- ��������
				UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score = @dwGoodsNum + Score WHERE UserID=@dwUserID
		
				--������־
				SELECT @SerialNumber=dbo.WF_GetSerialNumber()
			
				
				--�����ˮ
				IF @CurScore IS NOT NULL AND @CurInsureScore IS NOT NULL
				BEGIN
					INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
					VALUES(@SerialNumber,0,@dwUserID,14,@CurScore,@CurInsureScore,@dwGoodsNum,@strClientIP,GETDATE())	
				END
				
				--�����Ʒ��¼
				SET @PackageInfo=@PackageInfo+'0:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--��ʯ
			ELSE IF @wTypeID=1
			BEGIN	

				DECLARE @TotalDiamond BIGINT
				-- ��ȡ��ʯ
				SELECT @TotalDiamond=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
						
				-- ��������
				UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@dwGoodsNum WHERE UserID=@dwUserID
				
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket)	VALUES(@dwUserID, @dwGoodsNum,0)
				END
				

				--������־
				SELECT @SerialNumber=dbo.WF_GetSerialNumber()
				--��ʯ��ˮ
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,16,@TotalDiamond,@dwGoodsNum,@strClientIP,GETDATE())
				
				--�����Ʒ��¼
				SET @PackageInfo=@PackageInfo+'1:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--����
			ELSE IF @wTypeID=2
			BEGIN
				--���뱳��
				UPDATE  WHQJAccountsDB.dbo.AccountsPackage SET GoodShowID=0, GoodsCount=GoodsCount+ @dwGoodsNum, PushTime=GETDATE() WHERE UserID=@dwUserID AND GoodsID=@dwPropertyID
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsCount,PushTime)	VALUES(@dwUserID, @dwPropertyID,0,@dwGoodsNum,GETDATE())
				END
				
				--�����Ʒ��¼
				SET @PackageInfo=@PackageInfo+'2:'+CONVERT(NVARCHAR,@dwPropertyID)+':'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--����
			ELSE IF @wTypeID=3
			BEGIN

				--������¼
				DECLARE @TotalTicket BIGINT
				-- ��ȡ����
				SELECT @TotalTicket=AwardTicket FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID

				-- ��������
				UPDATE  WHQJTreasureDB.dbo.UserCurrency SET AwardTicket=AwardTicket+@dwGoodsNum WHERE UserID=@dwUserID
				
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket)	VALUES(@dwUserID, 0,@dwGoodsNum)
				END
				

				-- ������¼
				INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
				VALUES (dbo.WF_GetSerialNumber(),0,@dwUserID,1,@TotalTicket-@dwGoodsNum,@dwGoodsNum,@strClientIP,GETDATE())
				
				--�����Ʒ��¼
				SET @PackageInfo=@PackageInfo+'3:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'	
			END
			
			FETCH NEXT FROM  MyCursor INTO @wTypeID, @dwPropertyID,@dwGoodsNum
		END    
	 
	--�ر��α�
	CLOSE MyCursor
	--�ͷ��α�
	DEALLOCATE MyCursor
	
	--��ȡ��¼
	SET @GotPackageInfo=@GotPackageInfo+CONVERT(NVARCHAR,@wNeedDate)+N';'
	UPDATE WHQJAccountsDB.dbo.AccountsSignin SET GotPackageInfo=@GotPackageInfo WHERE UserID=@dwUserID
	--������־
	INSERT INTO WHQJRecordDB.dbo.RecordGameSignIn(UserID,SignType,PackageName,PackageGoods,Probability,NeedDay,TotalDay,ClinetIP,CollectDate)
	VALUES (@dwUserID,1,@PackageName,@PackageInfo,100,1,@wSeriesDate,@strClientIP,GETDATE())
	
		--�����Ϣ
	DECLARE @Score BIGINT
	DECLARE @Diamond BIGINT
	DECLARE @AwardTicket INT
	
	--��ѯ�Ƹ�
	SELECT @Score=Score FROM WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	SELECT @Diamond=Diamond, @AwardTicket=AwardTicket FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
	
	IF @Score IS NULL SET @Score=0
	IF @Diamond IS NULL SET @Diamond=0
	IF @AwardTicket IS NULL SET @AwardTicket=0
	
	SELECT @Score AS Score,@Diamond AS Diamond,@AwardTicket AS AwardTicket
END

RETURN 0

GO