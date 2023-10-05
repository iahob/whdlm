
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_QueryVideoInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_QueryVideoInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_QueryVideoDetails]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_QueryVideoDetails]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_QueryVideoDetailsByRoomID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_QueryVideoDetailsByRoomID]

GO
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_QueryPlayBackCodeYZ]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_QueryPlayBackCodeYZ]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ѯ��Ƶ�б�
CREATE PROC GSP_MB_QueryVideoInfo
	@iQueryType INT,								-- 1:��ͨ 2:Լս����  3:Լս���			
	@dwUserID	INT,								-- �û� I D
	@dwPlayBack INT,								-- �ط����0
	@dwPersonalRoomID	INT,						-- Լս����ID
	@szPersonalGUID NVARCHAR(31),					-- ����Ψһ��ʶ
	@cbQueryMode tinyint,							-- ��������
	@wIndexBegin INT,								-- ������ʼ
	@wIndexEnd INT,									-- ��������
	@strErrorDescribe NVARCHAR(127) OUTPUT			-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	DECLARE @cbQueryTypeTemp TINYINT
	DECLARE @cbPlayMode TINYINT
	IF @iQueryType = 2 OR @iQueryType = 3
	BEGIN
		SET @cbQueryTypeTemp = 0
	END
			
	IF @dwPlayBack=0
	BEGIN
		-- ��ѯ�û�
		IF not exists(SELECT * FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID)
		BEGIN
			SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڣ�'
			return 1
		END


		--2Լս����
		IF @iQueryType = 2 
		BEGIN
			SET @cbPlayMode = 0
		END

		--3 Լս���
		IF @iQueryType = 3
		BEGIN
			SET @cbPlayMode = 1
		END
	
		IF @cbQueryTypeTemp=0
		BEGIN
			IF @dwPersonalRoomID <> 0 and @cbQueryMode = 0
			BEGIN
		
				SELECT a.UserID as UserID,a.CellScore as CellScore, c.NickName as NickName,c.GameID as GameID,c.FaceID as FaceID,c.CustomID as CustomID,a.PersonalRoomGUID as PersonalGUID,a.RoomID as RoomID,a.Score as TotalScore,a.WriteTime as CreateTime,
				a.ChairID as ChairID,a.KindID as KindID,c.Gender as Gender,@iQueryType as QueryType,
				(case when (EXISTS(select * from [WHQJPlatformDB].[dbo].[StreamCreateTableFeeInfo] where UserID=a.UserID and RoomID=a.RoomID )) then 1 else 0 end) AS CreateRoom 
				FROM [WHQJPlatformDB].[dbo].[PersonalRoomScoreInfo]  a ,
				(select UserID,PersonalRoomGUID from [WHQJPlatformDB].[dbo].[PersonalRoomScoreInfo] where RoomID=@dwPersonalRoomID ) b ,
				[WHQJAccountsDB].[dbo].[AccountsInfo] c			
				where a.PersonalRoomGUID = b.PersonalRoomGUID and a.GroupID=0 and c.UserID = a.UserID and a.PlayMode = @cbPlayMode and DateDiff(d,a.WriteTime,GetDate()) < 1 order by a.WriteTime desc
			END
			else
			BEGIN
				
				IF @iQueryType = 2
				BEGIN
					--����������ս������
					DECLARE @tempRoomRecord TABLE
					(
						UserID INT, PersonalRoomGUID nvarchar(31), RoomID INT, Score BIGINT, PlayBackCode INT, ChairID INT, KindID INT, GroupID INT, PlayMode TINYINT, CellScore BIGINT, StartTime DATETIME, RoomHostID INT
					);

					--��ȡ��Ҳ�����ķ���
					DECLARE @tempJoinRoomRecord TABLE
					(
						PersonalRoomGUID nvarchar(31)
					);
					--��ȡ���һ��Ĳ���Լս��¼�����20��Լս����	
					INSERT INTO @tempJoinRoomRecord(PersonalRoomGUID )
					SELECT TOP (20)             PersonalRoomGUID FROM [WHQJPlatformDB].[dbo].PersonalRoomScoreInfo A
					WHERE UserID = @dwUserID AND DateDiff(HH,WriteTime,GetDate()) <= 24 AND ((SELECT COUNT(PersonalRoomGUID)  FROM [WHQJPlatformDB].[dbo].RecordBackInfo WHERE  PersonalRoomGUID = A.PersonalRoomGUID ) > 0)  order by WriteTime desc	
					


					--��ȡ���һ��Ĳ���Լս��¼�����20��Լս����	
					INSERT INTO @tempRoomRecord(UserID, PersonalRoomGUID, RoomID, Score, PlayBackCode, ChairID, KindID, GroupID, PlayMode, CellScore,StartTime,RoomHostID )
					SELECT   a.UserID, a.PersonalRoomGUID, a.RoomID, a.Score, a.PlayBackCode, a.ChairID, a.KindID, a.GroupID, a.PlayMode, a.CellScore, a.StartTime,a.RoomHostID FROM [WHQJPlatformDB].[dbo].PersonalRoomScoreInfo a, @tempJoinRoomRecord b 
					WHERE a.PersonalRoomGUID = b.PersonalRoomGUID and a.GroupID=0	

					--��ȡ�����
					SELECT a.UserID as UserID, a.CellScore as CellScore, b.NickName as NickName,b.GameID as GameID, b.FaceID as FaceID, b.CustomID as CustomID,a.PersonalRoomGUID as PersonalGUID,a.RoomID as RoomID,
					a.Score as TotalScore, a.StartTime as CreateTime,a.ChairID as ChairID,a.KindID as KindID,b.Gender as Gender,@iQueryType as QueryType,
					(case when (EXISTS(select * from [WHQJPlatformDB].[dbo].[StreamCreateTableFeeInfo] where UserID=a.UserID and RoomID=a.RoomID )) then 1 else 0 end) AS CreateRoom --�˴���ʶ�Ƿ�Ϊ����
					FROM @tempRoomRecord a, [WHQJAccountsDB].[dbo].[AccountsInfo] b WHERE a.UserID = b.UserID
				END
				ELSE
				BEGIN
						DECLARE @temp TABLE
				(
					UserID INT,
					PersonalRoomGUID nvarchar(31),
					GameNum INT
				);
				
				INSERT INTO @temp(UserID, PersonalRoomGUID, GameNum)
				select UserID, PersonalRoomGUID,  GamesNum    from [WHQJPlatformDB].[dbo].[RecordBackInfo] where UserID=@dwUserID and PersonalRoomGUID = @szPersonalGUID
					
				
				

				--��ȡ�÷�����������	
				DECLARE @tempAllUser TABLE
				(
					UserID INT,
					PersonalRoomGUID nvarchar(31),
					GameNum INT,
					RoomID  INT
				);
				
				DECLARE @temUserID INT
				DECLARE @tempPersonalRoomGUID nvarchar(31)
				DECLARE @tempGamesNum INT
				DECLARE @tempRoomID INT
				
	
				INSERT INTO @tempAllUser SELECT a.UserID ,a.PersonalRoomGUID , a.GamesNum, a.RoomID
				FROM [WHQJPlatformDB].[dbo].[RecordBackInfo]  a ,
				@temp c		
				where a.PersonalRoomGUID = c.PersonalRoomGUID and a.GamesNum = c.GameNum
				
				
				--��������
				DECLARE @result TABLE
				(
					UserID INT,
					CellScore int,
					NickName nvarchar(31),
					GameID int,
					FaceID int,
					CustomID int,
					PersonalGUID nvarchar(31),
					RoomID int,
					TotalScore bigint,
					CreateTime datetime,
					ChairID smallint,
					KindID smallint,
					Gender tinyint,
					QueryType tinyint,
					GamesNum int,
					CreateRoom tinyint
				);
				
					DECLARE @UserID INT
					DECLARE @CellScore int
					DECLARE @NickName nvarchar(31)
					DECLARE @GameID int
					DECLARE @FaceID int
					DECLARE @CustomID int
					DECLARE @PersonalGUID nvarchar(31)
					DECLARE @RoomID nvarchar(6)
					DECLARE @TotalScore bigint
					DECLARE @CreateTime datetime
					DECLARE @ChairID smallint
					DECLARE @KindID smallint
					DECLARE @Gender tinyint
					DECLARE @QueryType tinyint
					DECLARE @GamesNum int
					DECLARE @CreateRoom tinyint
						
					set @cbPlayMode = 1
					set @CreateRoom = 1
					
				
				
					 --��������
					WHILE EXISTS(SELECT UserID FROM @tempAllUser)
					BEGIN
					-- Ҳ����ʹ��top 1
					SET ROWCOUNT 1
					
					SELECT @temUserID= UserID, @tempPersonalRoomGUID = PersonalRoomGUID,  @tempGamesNum = GameNum, @tempRoomID = RoomID FROM @tempAllUser;
					
					SELECT @UserID = a.UserID , @CellScore = a.CellScore, @NickName = c.NickName, @GameID = c.GameID, @FaceID = c.FaceID, @CustomID = c.CustomID,
					@PersonalGUID = a.PersonalRoomGUID, @RoomID = a.RoomID, @TotalScore = a.Score, @CreateTime = a.WriteTime,
					@ChairID = a.ChairID, @KindID = a.KindID, @Gender = c.Gender, @QueryType = @iQueryType
					
					FROM [WHQJPlatformDB].[dbo].[PersonalRoomScoreInfo]  a ,
					[WHQJAccountsDB].[dbo].[AccountsInfo] c		
					where a.PersonalRoomGUID = @tempPersonalRoomGUID and a.UserID = @temUserID and c.UserID = a.UserID and a.PlayMode = @cbPlayMode and DateDiff(d,a.WriteTime,GetDate()) < 2 order by a.WriteTime desc
					
	
					if @UserID is not null
					begin
					set @CreateRoom = 1
					if EXISTS(select * from [WHQJPlatformDB].[dbo].[StreamCreateTableFeeInfo] where UserID=@temUserID and RoomID=@tempRoomID)
					begin
						set @CreateRoom = 1
					end
												
					insert into @result values(@UserID, @CellScore, @NickName,@GameID, @FaceID , @CustomID ,@PersonalGUID, @RoomID, @TotalScore, @CreateTime,
					@ChairID , @KindID , @Gender , @iQueryType, @tempGamesNum, @CreateRoom)
					end
					SET ROWCOUNT 0 
					DELETE FROM @tempAllUser WHERE PersonalRoomGUID=@tempPersonalRoomGUID and GameNum = @tempGamesNum and UserID = @temUserID;
					END
					
					if NOT EXISTS (select *  from @result)
					begin
						insert into @result values(0, 0, 0,0, 0 , 0 ,'', 0, 0, 0,
						0 , 0 , 0 , 3, 0, 0)
					end
					select * from @result
				END
			END
		END
		ELSE
		BEGIN
			SELECT a.UserID as UserID,c.NickName as NickName,c.GameID as GameID,c.FaceID as FaceID,c.CustomID as CustomID,a.VideoNumber as VideoNumber,a.Score as TotalScore,a.BuildVideoTime as CreateTime,			 
			a.ChairID as ChairID,a.KindID as KindID,c.Gender as Gender,1 as QueryType
			FROM [WHQJTreasureDB].[dbo].[RecordVideoPlayerInfo]  a ,					
			(select top (@wIndexEnd) VideoNumber from [WHQJTreasureDB].[dbo].[RecordVideoPlayerInfo] where UserID=@dwUserID and (VideoNumber NOT IN (select top (@wIndexBegin) VideoNumber from [WHQJTreasureDB].[dbo].[RecordVideoPlayerInfo]  where UserID=@dwUserID))) b ,		
			[WHQJAccountsDB].[dbo].[AccountsInfo] c			
			where c.UserID = a.UserID and b.VideoNumber=a.VideoNumber and DateDiff(d,a.BuildVideoTime,GetDate()) < 1			
		END	
	END	
	ELSE
	BEGIN	
		IF EXISTS (select *  from [WHQJPlatformDB].[dbo].[PersonalRoomScoreInfo] where PlayBackCode=@dwPlayBack)	
		BEGIN
			
			SELECT a.UserID as UserID, a.CellScore as CellScore,c.NickName as NickName,c.GameID as GameID,c.FaceID as FaceID,c.CustomID as CustomID,a.PersonalRoomGUID as PersonalGUID,a.RoomID as RoomID,a.Score as TotalScore,a.WriteTime as CreateTime,
			a.ChairID as ChairID,a.KindID as KindID,c.Gender as Gender,b.UserID as PlayUserID,@iQueryType as QueryType,
			(case when (EXISTS(select * from [WHQJPlatformDB].[dbo].[StreamCreateTableFeeInfo] where UserID=a.UserID and RoomID=a.RoomID )) then 1 else 0 end) AS CreateRoom 
			FROM [WHQJPlatformDB].[dbo].[PersonalRoomScoreInfo]  a ,
			(select UserID,PersonalRoomGUID from [WHQJPlatformDB].[dbo].[PersonalRoomScoreInfo] where PlayBackCode=@dwPlayBack) b ,
			[WHQJAccountsDB].[dbo].[AccountsInfo] c			
			where a.PersonalRoomGUID = b.PersonalRoomGUID and c.UserID = a.UserID and DateDiff(d,a.WriteTime,GetDate()) < 1
		END
		ELSE		
		BEGIN
			SELECT a.UserID as UserID, c.NickName as NickName,c.GameID as GameID,c.FaceID as FaceID,c.CustomID as CustomID,a.VideoNumber as VideoNumber,a.Score as TotalScore,a.BuildVideoTime as CreateTime,			 
			a.ChairID as ChairID,a.KindID as KindID,c.Gender as Gender,b.UserID as PlayUserID,1 as QueryType
			FROM [WHQJTreasureDB].[dbo].[RecordVideoPlayerInfo]  a ,	
			(select UserID,VideoNumber from [WHQJTreasureDB].[dbo].[RecordVideoPlayerInfo]  where PlayBackCode=@dwPlayBack) b ,		
			[WHQJAccountsDB].[dbo].[AccountsInfo] c			
			where c.UserID = a.UserID and b.VideoNumber=a.VideoNumber and DateDiff(d,a.BuildVideoTime,GetDate()) < 1
		END			
	END
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- ��ѯ��Ƶ����
CREATE PROC GSP_MB_QueryVideoDetails
	@szPersonalGUID	NVARCHAR(31),					-- Լս��Ψһ I D
	@strErrorDescribe NVARCHAR(127) OUTPUT			-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	SELECT a.ID as VideoNumber,a.UserID as UserID,b.NickName as NickName,a.Score as Score,
		a.GamesNum as GameNum,@szPersonalGUID as PersonalGUID,a.GameMode as GameMode, a.LoopCount as LoopCount
	FROM [WHQJPlatformDB].[dbo].[RecordBackInfo] a,[WHQJAccountsDB].[dbo].[AccountsInfo] b
	where PersonalRoomGUID=@szPersonalGUID and a.UserID = b.UserID
END 

RETURN 0

GO
----------------------------------------------------------------------------------------------------
-- ��ѯ��Ƶ����
CREATE PROC GSP_MB_QueryVideoDetailsByRoomID
	@dwPersonalRoomID	INT,							-- Լս��I D
	@strErrorDescribe	NVARCHAR(127) OUTPUT			-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	SELECT a.ID as VideoNumber,a.UserID as UserID,b.NickName as NickName,a.Score as Score,
		a.GamesNum as GameNum,a.PersonalRoomGUID as PersonalGUID,a.GameMode as GameMode, a.LoopCount as LoopCount
	FROM [WHQJPlatformDB].[dbo].[RecordBackInfo] a,[WHQJAccountsDB].[dbo].[AccountsInfo] b
	where RoomID=@dwPersonalRoomID and a.UserID = b.UserID
END 

RETURN 0

GO
----------------------------------------------------------------------------------------------------
-- ��ѯ�ط���
CREATE PROC GSP_MB_QueryPlayBackCodeYZ
	@dwUserID		INT,							-- �û� I D
	@szPersonalGUID	NVARCHAR(31),					-- Լս��Ψһ I D
	@strErrorDescribe NVARCHAR(127) OUTPUT			-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	declare @bFind	INT
	declare @Code	INT
	SET @Code=0      
	select @Code=PlayBackCode from  [WHQJPlatformDB].[dbo].[PersonalRoomScoreInfo] where PersonalRoomGUID=@szPersonalGUID and UserID=@dwUserID and DateDiff(d,WriteTime,GetDate()) <= 10
	
	IF @Code=0
	BEGIN
		SET @bFind=0       
		WHILE @bFind=0    
		BEGIN
			SET @Code = cast(ceiling(rand() * 899999) as int)+100000
			IF NOT EXISTS (select * from  [WHQJPlatformDB].[dbo].[PersonalRoomScoreInfo] where PlayBackCode=@Code and DateDiff(d,WriteTime,GetDate()) <= 10)
			BEGIN
				SET @bFind = 1  
				UPDATE [WHQJPlatformDB].[dbo].[PersonalRoomScoreInfo] SET PlayBackCode =@Code WHERE UserID=@dwUserID and PersonalRoomGUID=@szPersonalGUID   
			END
		END 
	END	
	
	SELECT @Code as Code
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------