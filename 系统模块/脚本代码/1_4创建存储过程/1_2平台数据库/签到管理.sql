
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

-- 加载奖励
CREATE PROC GSP_GP_LoadSignInReward
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询奖励
	SELECT A.PackageID, A.Name,A.PlatformKind,B.TypeID,B.NeedDay,B.Probability,B.SortID FROM GamePackage AS A,GameSignIn AS B  
	WHERE B.TypeID<=1 AND B.Nullity=0 AND A.PackageID=B.PackageID ORDER BY B.SortID ASC  

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- 礼包详情
CREATE PROC GSP_GP_LoadPackageInfo
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	--定义变量
	DECLARE @dwPackageID INT
	DECLARE @dwGoodsCount INT
	DECLARE @strbuf NVARCHAR(100)
	DECLARE @ResourceURL NVARCHAR(100)
	CREATE TABLE #TmpTable (PackageID INT , PackageInfo NVARCHAR(100) , GoodsCount INT , ResourceURL NVARCHAR(100))
	--申明游标
	DECLARE MyCursor CURSOR    
		FOR SELECT  PackageID FROM GamePackage WHERE Nullity=0
	 
	--打开游标    
	OPEN MyCursor
	 
	--循环游标
	FETCH NEXT FROM  MyCursor INTO  @dwPackageID
	WHILE @@FETCH_STATUS =0
		BEGIN	
			--获取货币信息	
			SELECT @strbuf=STUFF(   (SELECT '，'+(case TypeID when 0 then '金币*' when 1 then '钻石*' when 3 then '奖劵*'end)				 
											  +convert(nvarchar,GoodsNum)
									 FROM GamePackageGoods  WHERE PackageID=@dwPackageID FOR XML PATH('')),
								 1,1,'') 
							
			--获取道具信息							 
			IF EXISTS (SELECT 1  FROM GameProperty AS A,GamePackageGoods AS B WHERE B.PackageID=@dwPackageID AND B.TypeID=2 AND B.PropertyID=A.ID)
			BEGIN
				IF @strbuf IS NULL SET @strbuf=N''
				ELSE SET @strbuf=@strbuf+'，'
				
				SELECT @strbuf=@strbuf+STUFF( (SELECT TOP 1 '，'+A.Name+'*'+convert(nvarchar,B.GoodsNum) 
													FROM GameProperty AS A,GamePackageGoods AS B 
													 WHERE B.PackageID=@dwPackageID AND B.TypeID=2 AND B.PropertyID=A.ID FOR XML PATH('')),
												  1,1,'') 
			END
			
	
			
			-- 变量赋值
			IF @strbuf IS NULL	SET @strbuf='无'
			ELSE 
			BEGIN
				--获取道具数目
				SELECT @dwGoodsCount=COUNT(PackageID) FROM GamePackageGoods WHERE PackageID=@dwPackageID
				IF @dwGoodsCount=1 
				BEGIN
					SELECT TOP 1 @ResourceURL=ResourceURL FROM GamePackageGoods WHERE PackageID=@dwPackageID
				END
				ELSE SET @ResourceURL=''
			END
			--插入记录
			INSERT INTO #TmpTable(PackageID,PackageInfo,GoodsCount,ResourceURL)
			VALUES (@dwPackageID,@strbuf,@dwGoodsCount,@ResourceURL)
							
			--下一条记录
			FETCH NEXT FROM  MyCursor INTO  @dwPackageID
		END    
	 
	--关闭游标
	CLOSE MyCursor
	--释放游标
	DEALLOCATE MyCursor
	
	SELECT * FROM #TmpTable
	
	--删除临时表
	IF EXISTS (select * from tempdb.dbo.sysobjects where id = object_id(N'tempdb..#TmpTable') and type='U')
	DROP TABLE #TmpTable

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 查询签到
CREATE PROC GSP_GR_SignInQueryInfo
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	IF NOT EXISTS (SELECT UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND DynamicPass=@strPassword)
	BEGIN
		SET @strErrorDescribe = N'抱歉，你的用户信息不存在或者密码不正确！'
		return 1
	END

	-- 签到记录
	DECLARE @wSeriesDate INT
	DECLARE @wSignInCycDate INT 
	DECLARE @StartDateTime DateTime
	DECLARE @LastDateTime DateTime
	DECLARE @GotPackage NVARCHAR(32)
	--查询记录
	SELECT @StartDateTime=StartDateTime,@LastDateTime=LastDateTime,@wSeriesDate=SeriesDate,@GotPackage=GotPackageInfo FROM  WHQJAccountsDB.dbo.AccountsSignin 	
	WHERE UserID=@dwUserID
	
	--初始数据
	IF @StartDateTime IS NULL OR @LastDateTime IS NULL OR @wSeriesDate IS NULL
	BEGIN
		SELECT @StartDateTime=GetDate(),@LastDateTime=GetDate(),@wSeriesDate=0,@GotPackage= N''
		INSERT INTO  WHQJAccountsDB.dbo.AccountsSignin VALUES(@dwUserID,@StartDateTime,@LastDateTime,0,@GotPackage)		
	END

	--获取周期
	SELECT TOP 1 @wSignInCycDate=NeedDay FROM GameSignIn WHERE TypeID=1 AND Nullity=0 ORDER BY NeedDay DESC  

	--重置签到
	IF @wSeriesDate = @wSignInCycDate  AND  DateDiff(dd,@LastDateTime,GetDate())>0 OR @wSeriesDate>@wSignInCycDate
	BEGIN			
		SET @wSeriesDate = 0
		SET @GotPackage=N''
		UPDATE  WHQJAccountsDB.dbo.AccountsSignin SET StartDateTime=GetDate(),LastDateTime=GetDate(),SeriesDate=0 ,GotPackageInfo=N'' WHERE UserID=@dwUserID									
	END

	-- 日期判断
	DECLARE @TodaySignIned TINYINT
	SET @TodaySignIned = 0
	
	--已签到
	IF DateDiff(dd,@LastDateTime,GetDate()) = 0 	
	BEGIN
		IF @wSeriesDate > 0 SET @TodaySignIned = 1
	END ELSE
	BEGIN		
		--连续签到断结 重置签到
		IF DateDiff(dd,@StartDateTime,GetDate()) <> @wSeriesDate OR @wSeriesDate >= @wSignInCycDate 
		BEGIN
			SET @wSeriesDate = 0
			SET @GotPackage=N''
			UPDATE  WHQJAccountsDB.dbo.AccountsSignin SET StartDateTime=GetDate(),LastDateTime=GetDate(),SeriesDate=0 ,GotPackageInfo=N'' WHERE UserID=@dwUserID									
		END
	END

	-- 抛出数据
	SELECT @wSeriesDate AS SeriesDate,@TodaySignIned AS TodaySignIned,@GotPackage AS GotPackage
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 签到执行
CREATE PROC GSP_GR_SignInDone
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	IF NOT EXISTS(SELECT UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND DynamicPass=@strPassword)
	BEGIN
		SET @strErrorDescribe = N'用户信息不存在或者密码不正确！'
		RETURN 1
	END
	
	-- 签到记录
	DECLARE @wSeriesDate INT
	DECLARE @wSignInCycDate INT 	
	DECLARE @StartDateTime DateTime
	DECLARE @LastDateTime DateTime
	DECLARE @GotPackage NVARCHAR(32)
	--查询记录
	SELECT @StartDateTime=StartDateTime,@LastDateTime=LastDateTime,@wSeriesDate=SeriesDate,@GotPackage=GotPackageInfo FROM  WHQJAccountsDB.dbo.AccountsSignin 	
	WHERE UserID=@dwUserID
	
	--初始数据
	IF @StartDateTime IS NULL OR @LastDateTime IS NULL OR @wSeriesDate IS NULL
	BEGIN
		SELECT @StartDateTime=GetDate(),@LastDateTime=GetDate(),@wSeriesDate=0,@GotPackage= N''
		INSERT INTO  WHQJAccountsDB.dbo.AccountsSignin VALUES(@dwUserID,@StartDateTime,@LastDateTime,0,@GotPackage)		
	END

	--签到判断
	IF DateDiff(dd,@LastDateTime,GetDate()) = 0 AND @wSeriesDate > 0
	BEGIN
		SET @strErrorDescribe = N'抱歉，您今天已经签到了！'
		return 2		
	END

	--获取周期
	SELECT TOP 1 @wSignInCycDate=NeedDay FROM GameSignIn WHERE TypeID=1 AND Nullity=0 ORDER BY NeedDay DESC  

	-- 日期越界
	IF @wSeriesDate > @wSignInCycDate
	BEGIN
		SET @strErrorDescribe = N'您的签到信息出现异常，请与我们的客服人员联系！'
		return 3				
	END
	
	SELECT @wSeriesDate AS wSeriesDate
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 签到奖励
CREATE PROC GSP_GR_SignInReword
	@dwUserID INT,								-- 用户 I D
	@dwPackageID INT,							-- 礼包索引
	@dwProbability INT,							-- 礼包概率
	@dwSeriesDate INT,							-- 连续天数
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询用户
	IF NOT EXISTS(SELECT UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe = N'用户信息不存在或者密码不正确！'
		RETURN 1
	END
	
	-- 查询礼包
	DECLARE @PackageName NVARCHAR(30)
	SELECT @PackageName=Name FROM GamePackage WHERE PackageID=@dwPackageID AND Nullity=0
	
	IF @PackageName IS NULL
	BEGIN
		SET @strErrorDescribe = N'礼包正在打包中，请稍后再试试！'
		RETURN 1
	END
	
	-- 更新签到记录
	SET @dwSeriesDate = @dwSeriesDate+1
	UPDATE  WHQJAccountsDB.dbo.AccountsSignin SET LastDateTime = GetDate(),SeriesDate = @dwSeriesDate WHERE UserID = @dwUserID

	--领取礼包物品
	DECLARE @wTypeID TINYINT
	DECLARE @dwPropertyID INT
	DECLARE @dwGoodsNum INT
	
	--礼包记录
	DECLARE @PackageInfo NVARCHAR(128)
	SET @PackageInfo=''
	
	--流水号
	DECLARE @SerialNumber NVARCHAR(20)

	--申明游标
	DECLARE MyCursor CURSOR    
		FOR SELECT TypeID, PropertyID,GoodsNum FROM GamePackageGoods WHERE PackageID=@dwPackageID
	 
	--打开游标    
	OPEN MyCursor
	 
	--循环游标
	FETCH NEXT FROM  MyCursor INTO @wTypeID, @dwPropertyID,@dwGoodsNum
	WHILE @@FETCH_STATUS =0
		BEGIN
			--金币
			IF @wTypeID=0
			BEGIN

				DECLARE @CurScore AS BIGINT
				DECLARE @CurInsureScore AS BIGINT
				--读取银行
				SELECT @CUrScore=Score,@CurInsureScore=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

			
				-- 更新数据
				UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score = @dwGoodsNum + Score WHERE UserID=@dwUserID
		
				--插入日志
				SELECT @SerialNumber=dbo.WF_GetSerialNumber()
				

				
				--金币流水
				IF @CurScore IS NOT NULL AND @CurInsureScore IS NOT NULL
				BEGIN
					INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
					VALUES(@SerialNumber,0,@dwUserID,14,@CurScore,@CurInsureScore,@dwGoodsNum,@strClientIP,GETDATE())	
				END
				
				--礼包物品记录
				SET @PackageInfo=@PackageInfo+'0:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--钻石
			ELSE IF @wTypeID=1
			BEGIN	
			
				DECLARE @TotalDiamond BIGINT
				-- 读取钻石
				SELECT @TotalDiamond=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
						
				-- 更新数据
				UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@dwGoodsNum WHERE UserID=@dwUserID
				
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket)	VALUES(@dwUserID, @dwGoodsNum,0)
				END
							

				--插入日志
				SELECT @SerialNumber=dbo.WF_GetSerialNumber()
				--钻石流水
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,16,@TotalDiamond,@dwGoodsNum,@strClientIP,GETDATE())
				
				--礼包物品记录
				SET @PackageInfo=@PackageInfo+'1:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--道具
			ELSE IF @wTypeID=2
			BEGIN
				--加入背包
				UPDATE  WHQJAccountsDB.dbo.AccountsPackage SET GoodShowID=0, GoodsCount=GoodsCount+ @dwGoodsNum, PushTime=GETDATE() WHERE UserID=@dwUserID AND GoodsID=@dwPropertyID
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsCount,PushTime)	VALUES(@dwUserID, @dwPropertyID,0,@dwGoodsNum,GETDATE())
				END
				
				--礼包物品记录
				SET @PackageInfo=@PackageInfo+'2:'+CONVERT(NVARCHAR,@dwPropertyID)+':'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--奖卷
			ELSE IF @wTypeID=3
			BEGIN

				--奖劵记录
				DECLARE @TotalTicket BIGINT
				-- 读取奖劵
				SELECT @TotalTicket=AwardTicket FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID

				-- 更新数据
				UPDATE  WHQJTreasureDB.dbo.UserCurrency SET AwardTicket=AwardTicket+@dwGoodsNum WHERE UserID=@dwUserID
				
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket)	VALUES(@dwUserID, 0,@dwGoodsNum)
				END
				
				-- 奖劵记录
				INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
				VALUES (dbo.WF_GetSerialNumber(),0,@dwUserID,1,@TotalTicket-@dwGoodsNum,@dwGoodsNum,@strClientIP,GETDATE())
				
				--礼包物品记录
				SET @PackageInfo=@PackageInfo+'3:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'			
			END
			
			FETCH NEXT FROM  MyCursor INTO @wTypeID, @dwPropertyID,@dwGoodsNum
		END    
	 
	--关闭游标
	CLOSE MyCursor
	--释放游标
	DEALLOCATE MyCursor
	
	--插入日志
	INSERT INTO WHQJRecordDB.dbo.RecordGameSignIn(UserID,SignType,PackageName,PackageGoods,Probability,NeedDay,TotalDay,ClinetIP,CollectDate)
	VALUES (@dwUserID,0,@PackageName,@PackageInfo,@dwProbability,1,@dwSeriesDate,@strClientIP,GETDATE())
	
	--输出信息
	DECLARE @Score BIGINT
	DECLARE @Diamond BIGINT
	DECLARE @AwardTicket INT
	
	--查询财富
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

-- 连续签到奖励
CREATE PROC GSP_GR_SignInCycReword
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@dwPackageID INT,							-- 礼包索引
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询用户
	IF NOT EXISTS(SELECT UserID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND DynamicPass=@strPassword)
	BEGIN
		SET @strErrorDescribe = N'用户信息不存在或者密码不正确！'
		RETURN 1
	END
	
	-- 查询礼包
	DECLARE @PackageName NVARCHAR(30)
	SELECT @PackageName=Name FROM GamePackage WHERE PackageID=@dwPackageID AND Nullity=0
	
	IF @PackageName IS NULL
	BEGIN
		SET @strErrorDescribe = N'礼包正在打包中，请稍后再试试！'
		RETURN 1
	END
	
	--查询玩家连续签到时间
	DECLARE @wSeriesDate INT
	DECLARE @GotPackageInfo NVARCHAR(32)
	SELECT @wSeriesDate=SeriesDate ,@GotPackageInfo=GotPackageInfo FROM  WHQJAccountsDB.dbo.AccountsSignin WHERE UserID=@dwUserID	
	
	--初始数据
	IF @wSeriesDate IS NULL
	BEGIN
		INSERT INTO  WHQJAccountsDB.dbo.AccountsSignin 
		VALUES(@dwUserID,GETDATE(),GETDATE(),0,N'')	
		
		SET @wSeriesDate=0
		SET @GotPackageInfo=N''
	END
	
	--获取礼包需要的时间
	DECLARE @wNeedDate INT
	SELECT TOP 1 @wNeedDate=NeedDay FROM GameSignIn WHERE PackageID=@dwPackageID AND TypeID=1 AND Nullity=0 ORDER BY NeedDay DESC  

	IF @wSeriesDate<@wNeedDate
	BEGIN
		SET @strErrorDescribe = N'抱歉，你连续签到时间不够，不能领取该礼包'
		RETURN 3
	END

	--查询是否已领
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
		SET @strErrorDescribe = N'抱歉，你已领取该礼包，不能重复领取该礼包'
		RETURN 4
	END
	
	--领取礼包物品	
	DECLARE @wTypeID TINYINT
	DECLARE @dwPropertyID INT
	DECLARE @dwGoodsNum INT

	--礼包记录
	DECLARE @PackageInfo NVARCHAR(128)
	SET @PackageInfo=''
	
	--流水号
	DECLARE @SerialNumber NVARCHAR(20)

	--申明游标
	DECLARE MyCursor CURSOR    
		FOR SELECT TypeID, PropertyID,GoodsNum FROM GamePackageGoods WHERE PackageID=@dwPackageID
	 
	--打开游标    
	OPEN MyCursor
	 
	--循环游标
	FETCH NEXT FROM  MyCursor INTO @wTypeID, @dwPropertyID,@dwGoodsNum
	WHILE @@FETCH_STATUS =0
		BEGIN
			--金币
			IF @wTypeID=0
			BEGIN
			
				DECLARE @CurScore AS BIGINT
				DECLARE @CurInsureScore AS BIGINT
				--读取银行
				SELECT @CUrScore=Score,@CurInsureScore=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

				-- 更新数据
				UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score = @dwGoodsNum + Score WHERE UserID=@dwUserID
		
				--插入日志
				SELECT @SerialNumber=dbo.WF_GetSerialNumber()
			
				
				--金币流水
				IF @CurScore IS NOT NULL AND @CurInsureScore IS NOT NULL
				BEGIN
					INSERT INTO WHQJRecordDB.dbo.RecordTreasureSerial(SerialNumber,MasterID,UserID,TypeID,CurScore,CurInsureScore,ChangeScore,ClientIP,CollectDate) 
					VALUES(@SerialNumber,0,@dwUserID,14,@CurScore,@CurInsureScore,@dwGoodsNum,@strClientIP,GETDATE())	
				END
				
				--礼包物品记录
				SET @PackageInfo=@PackageInfo+'0:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--钻石
			ELSE IF @wTypeID=1
			BEGIN	

				DECLARE @TotalDiamond BIGINT
				-- 读取钻石
				SELECT @TotalDiamond=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
						
				-- 更新数据
				UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@dwGoodsNum WHERE UserID=@dwUserID
				
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket)	VALUES(@dwUserID, @dwGoodsNum,0)
				END
				

				--插入日志
				SELECT @SerialNumber=dbo.WF_GetSerialNumber()
				--钻石流水
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,16,@TotalDiamond,@dwGoodsNum,@strClientIP,GETDATE())
				
				--礼包物品记录
				SET @PackageInfo=@PackageInfo+'1:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--道具
			ELSE IF @wTypeID=2
			BEGIN
				--加入背包
				UPDATE  WHQJAccountsDB.dbo.AccountsPackage SET GoodShowID=0, GoodsCount=GoodsCount+ @dwGoodsNum, PushTime=GETDATE() WHERE UserID=@dwUserID AND GoodsID=@dwPropertyID
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsCount,PushTime)	VALUES(@dwUserID, @dwPropertyID,0,@dwGoodsNum,GETDATE())
				END
				
				--礼包物品记录
				SET @PackageInfo=@PackageInfo+'2:'+CONVERT(NVARCHAR,@dwPropertyID)+':'+CONVERT(NVARCHAR,@dwGoodsNum)+';'
			END
			--奖卷
			ELSE IF @wTypeID=3
			BEGIN

				--奖劵记录
				DECLARE @TotalTicket BIGINT
				-- 读取奖劵
				SELECT @TotalTicket=AwardTicket FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID

				-- 更新数据
				UPDATE  WHQJTreasureDB.dbo.UserCurrency SET AwardTicket=AwardTicket+@dwGoodsNum WHERE UserID=@dwUserID
				
				IF @@ROWCOUNT = 0
				BEGIN
					INSERT  WHQJTreasureDB.dbo.UserCurrency(UserID,Diamond,AwardTicket)	VALUES(@dwUserID, 0,@dwGoodsNum)
				END
				

				-- 奖劵记录
				INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
				VALUES (dbo.WF_GetSerialNumber(),0,@dwUserID,1,@TotalTicket-@dwGoodsNum,@dwGoodsNum,@strClientIP,GETDATE())
				
				--礼包物品记录
				SET @PackageInfo=@PackageInfo+'3:0:'+CONVERT(NVARCHAR,@dwGoodsNum)+';'	
			END
			
			FETCH NEXT FROM  MyCursor INTO @wTypeID, @dwPropertyID,@dwGoodsNum
		END    
	 
	--关闭游标
	CLOSE MyCursor
	--释放游标
	DEALLOCATE MyCursor
	
	--领取记录
	SET @GotPackageInfo=@GotPackageInfo+CONVERT(NVARCHAR,@wNeedDate)+N';'
	UPDATE WHQJAccountsDB.dbo.AccountsSignin SET GotPackageInfo=@GotPackageInfo WHERE UserID=@dwUserID
	--插入日志
	INSERT INTO WHQJRecordDB.dbo.RecordGameSignIn(UserID,SignType,PackageName,PackageGoods,Probability,NeedDay,TotalDay,ClinetIP,CollectDate)
	VALUES (@dwUserID,1,@PackageName,@PackageInfo,100,1,@wSeriesDate,@strClientIP,GETDATE())
	
		--输出信息
	DECLARE @Score BIGINT
	DECLARE @Diamond BIGINT
	DECLARE @AwardTicket INT
	
	--查询财富
	SELECT @Score=Score FROM WHQJTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	SELECT @Diamond=Diamond, @AwardTicket=AwardTicket FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
	
	IF @Score IS NULL SET @Score=0
	IF @Diamond IS NULL SET @Diamond=0
	IF @AwardTicket IS NULL SET @AwardTicket=0
	
	SELECT @Score AS Score,@Diamond AS Diamond,@AwardTicket AS AwardTicket
END

RETURN 0

GO