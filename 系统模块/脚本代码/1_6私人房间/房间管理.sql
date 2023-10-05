
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_DeleteGameRoom]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_DeleteGameRoom]
GO



---插入私人房间参数
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_InsertPersonalRoomParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_InsertPersonalRoomParameter]
GO

--修改私人房间参数
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ModifyPersonalRoomParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_ModifyPersonalRoomParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DissumePersonalRoom]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DissumePersonalRoom]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------------------

-- 删除房间
CREATE  PROCEDURE dbo.GSP_GS_DeleteGameRoom
	@wServerID INT,								-- 房间标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查找房间
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM GameRoomInfo(NOLOCK) WHERE ServerID=@wServerID

	-- 结果判断
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'游戏房间不存在或者已经被删除了，房间修改失败！'
		RETURN 1
	END

	-- 删除房间
	DECLARE @nKindID INT
	SELECT @nKindID=KindID from GameRoomInfo where ServerID=@wServerID
	DECLARE @nRoomInfoCount int
	SET @nRoomInfoCount=(Select COUNT(*) from GameRoomInfo where KindID in (select KindID From GameRoomInfo where ServerID=@wServerID))
	DELETE GameRoomInfo WHERE ServerID=@wServerID

	-- 删除私人房间参数
	IF @nRoomInfoCount=1
	BEGIN
		DELETE PersonalRoomInfo WHERE KindID=@nKindID
	END
	ELSE
	BEGIN
		DECLARE @PersonalInfoCount int
		SET @PersonalInfoCount=(Select COUNT(*) from PersonalRoomInfo where KindID in (select KindID from PersonalRoomInfo where ServerID=@wServerID) and PlayMode=(select PlayMode From PersonalRoomInfo where ServerID=@wServerID))
		IF @PersonalInfoCount>1
			DELETE PersonalRoomInfo WHERE ServerID=@wServerID
	END
	SET @strErrorDescribe=N'删除成功！'
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 插入房间
CREATE  PROCEDURE dbo.GSP_GS_InsertPersonalRoomParameter

	-- 索引变量
	@cbPlayMode TINYINT,						-- 游戏模式
	@wServerID INT,								-- 房间标识
	@wKindID INT,								-- 房间ID	
	@cbIsJoinGame TINYINT,						-- 是否参与游戏
	@cbMinPeople TINYINT,						-- 参与游戏的最小人数
	@cbMaxPeople TINYINT,						-- 参与游戏的最大人数

	-- 积分配置
	@lPersonalRoomTax  BIGINT,					-- 私人房间游戏服务比例
	@lMinCellScore BIGINT,						-- 最小底分
	@lMaxCellScore BIGINT,						-- 最大底分
	@lMinEnterScore BIGINT,						-- 最小进入分
	@lMaxEnterScore BIGINT,						-- 最大进入分
	@wCanCreateCount SMALLINT,					-- 可以创建的最大房间数目
	@wPlayTurnCount INT,						-- 房间能够进行游戏的最大局数

	-- 限制配置
	@wPlayTimeLimit	INT,						-- 房间能够进行游戏的最大时间
	@wTimeAfterBeginCount	INT,				-- 一局游戏开始后多长时间后解散桌子
	@wTimeOffLineCount	INT,					-- 玩家掉线多长时间后解散桌子
	@wTimeNotBeginGame	INT,					-- 玩家坐下后多长时间后还未开始游戏解散桌子
	@wTimeNotBeginAfterCreateRoom	INT,		-- 房间创建后多长时间后还未开始游戏解散桌子
	@cbCreateRight	TINYINT,					-- 创建私人房间权限
	@wBeginFreeTime   SMALLINT,					-- 免费时段-开始
	@wEndFreeTime	 SMALLINT,					-- 免费时段-结束
	@wTimeRespondDismiss INT,					-- 响应解散请求时间
	@strShareStorageRule NVARCHAR(1024),		-- 共享库存规则	
	@strCurSubStorageItem NVARCHAR(1024),		-- 当前共享库存
	@lConfigSysStorage BIGINT,
	@lConfigPlayerStorage BIGINT,
	@lConfigParameterK BIGINT,
	@lCurResetCount BIGINT,
	
	-- 输出信息
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	DECLARE @dwServerID INT
	SELECT  @dwServerID = ServerID FROM PersonalRoomInfo WHERE KindID = @wKindID  AND PlayMode = @cbPlayMode
	IF @dwServerID IS NULL
	BEGIN
		-- 插入私人房间参数																					 
		INSERT INTO PersonalRoomInfo (PlayMode, ServerID,  KindID,  IsJoinGame, MinPeople, MaxPeople, PersonalRoomTax, MinCellScore, MaxCellScore, MinEnterScore, MaxEnterScore, CanCreateCount, 
					PlayTurnCount, PlayTimeLimit, TimeAfterBeginCount, TimeOffLineCount,TimeNotBeginGame, TimeNotBeginAfterCreateRoom, CreateRight,BeginFreeTime,EndFreeTime,TimeRespondDismiss,ShareStorageRule)
		VALUES (@cbPlayMode, @wServerID,@wKindID, @cbIsJoinGame, @cbMinPeople, @cbMaxPeople, @lPersonalRoomTax, @lMinCellScore, @lMaxCellScore, @lMinEnterScore, @lMaxEnterScore, @wCanCreateCount, @wPlayTurnCount, 
		@wPlayTimeLimit, @wTimeAfterBeginCount, @wTimeOffLineCount, @wTimeNotBeginGame, @wTimeNotBeginAfterCreateRoom, @cbCreateRight,@wBeginFreeTime,@wEndFreeTime,@wTimeRespondDismiss,@strShareStorageRule)
	END
	ELSE
	BEGIN
		--插入私人房间参数
		UPDATE PersonalRoomInfo SET  ServerID = @dwServerID, IsJoinGame = @cbIsJoinGame, MinPeople = @cbMinPeople, MaxPeople = @cbMaxPeople,PersonalRoomTax = @lPersonalRoomTax, MinCellScore = @lMinCellScore,
									 PlayTurnCount = @wPlayTurnCount, PlayTimeLimit = @wPlayTimeLimit, TimeAfterBeginCount = @wTimeAfterBeginCount, CanCreateCount = @wCanCreateCount,
									TimeOffLineCount = @wTimeOffLineCount,TimeNotBeginGame = @wTimeNotBeginGame, TimeNotBeginAfterCreateRoom = @wTimeNotBeginAfterCreateRoom, CreateRight = @cbCreateRight,
									BeginFreeTime=@wBeginFreeTime,EndFreeTime=@wEndFreeTime, PlayMode = @cbPlayMode, MaxCellScore = @lMaxCellScore, MinEnterScore = @lMinEnterScore, MaxEnterScore = @lMaxEnterScore, 
									TimeRespondDismiss=@wTimeRespondDismiss,ShareStorageRule=@strShareStorageRule
		WHERE KindID = @wKindID AND PlayMode = @cbPlayMode
	END
	
	IF @cbPlayMode = 1 
	BEGIN
		--插入默认共享库存规则
		DECLARE @tableGroupID TABLE
		(
			GroupID BIGINT
		);
		
		-- 插入数据
		INSERT INTO @tableGroupID(GroupID) SELECT GroupID FROM WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupProperty 
		
		WHILE EXISTS(SELECT GroupID FROM @tableGroupID)
		BEGIN
			SET ROWCOUNT 1		
			
			DECLARE @GroupID INT
			SELECT @GroupID=GroupID  FROM @tableGroupID 
			
			IF NOT EXISTS(SELECT * FROM WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty WHERE KindID=@wKindID AND GroupID=@GroupID AND PlayMode=@cbPlayMode)
			BEGIN
				INSERT INTO WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty(GroupID,KindID,PlayMode,ConfigSubStorageItem,CurSubStorageItem,ConfigSysStorage,ConfigPlayerStorage,ConfigParameterK,CurResetCount) VALUES(@GroupID,@wKindID,@cbPlayMode,@strShareStorageRule,@strCurSubStorageItem,@lConfigSysStorage,@lConfigPlayerStorage,@lConfigParameterK,@lCurResetCount)
			END
			ELSE
			BEGIN
				UPDATE WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty SET ConfigSubStorageItem=@strShareStorageRule,CurSubStorageItem=@strCurSubStorageItem,ConfigSysStorage=@lConfigSysStorage,ConfigPlayerStorage=@lConfigPlayerStorage,ConfigParameterK=@lConfigParameterK,CurResetCount=@lCurResetCount WHERE KindID=@wKindID AND GroupID=@GroupID AND PlayMode=@cbPlayMode
			END
		
			SET ROWCOUNT 0 
			DELETE FROM @tableGroupID WHERE GroupID=@GroupID
		END
		
		
		IF OBJECT_ID('tempdb..#tableGroupID') IS NOT NULL
		BEGIN
			DROP TABLE tableGroupID
		End
	END
	
	SET @strErrorDescribe=N'创建成功！'
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
-- 修改私人房间参数
CREATE  PROCEDURE dbo.GSP_GS_ModifyPersonalRoomParameter

	-- 索引变量
	@cbPlayMode TINYINT,						-- 游戏模式
	@wServerID INT,								-- 房间标识
	@wKindID INT,								-- 房间标识	
	@cbIsJoinGame TINYINT,						-- 是否参与游戏
	@cbMinPeople TINYINT,						-- 参与游戏的最小人数
	@cbMaxPeople TINYINT,						-- 参与游戏的最大人数

	-- 积分配置
	@lPersonalRoomTax  BIGINT,					-- 初始游分数
	@lMinCellScore BIGINT,						-- 最小底分
	@lMaxCellScore BIGINT,						-- 最大底分
	@lMinEnterScore BIGINT,						-- 最小进入分
	@lMaxEnterScore BIGINT,						-- 最大进入分
	@wCanCreateCount int,						-- 可以创建的最大房间数目
	@wPlayTurnCount INT,						-- 房间能够进行游戏的最大局数

	-- 限制配置
	@wPlayTimeLimit	INT,						-- 房间能够进行游戏的最大时间
	@wTimeAfterBeginCount	INT,				-- 一局游戏开始后多长时间后解散桌子
	@wTimeOffLineCount	INT,					-- 玩家掉线多长时间后解散桌子
	@wTimeNotBeginGame	INT,					-- 房间创建多长时间后还未开始游戏解散桌子
	@wTimeNotBeginAfterCreateRoom	INT,		-- 房间创建后多长时间后还未开始游戏解散桌子
	@cbCreateRight	TINYINT,					-- 创建私人房间权限

	@wBeginFreeTime   SMALLINT,					-- 免费时段-开始
	@wEndFreeTime		SMALLINT,				-- 免费时段-结束
	@wTimeRespondDismiss INT,					-- 响应解散请求时间
	@strShareStorageRule NVARCHAR(1024),		-- 共享库存规则	
	@strCurSubStorageItem NVARCHAR(1024),		-- 当前共享库存
	@lConfigSysStorage BIGINT,
	@lConfigPlayerStorage BIGINT,
	@lConfigParameterK BIGINT,
	@lCurResetCount BIGINT,
	
	-- 输出信息
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	DECLARE @dwServerID INT
	SELECT  @dwServerID = ServerID FROM PersonalRoomInfo WHERE KindID = @wKindID  AND PlayMode = @cbPlayMode
	IF @dwServerID IS NULL
	BEGIN
			SELECT  @dwServerID = ServerID FROM PersonalRoomInfo WHERE ServerID=@wServerID
			IF @dwServerID IS NULL
			BEGIN
					-- 插入私人房间参数																					 
					INSERT INTO PersonalRoomInfo (PlayMode, ServerID,  KindID,  IsJoinGame, MinPeople, MaxPeople, PersonalRoomTax, MinCellScore, MaxCellScore, MinEnterScore, MaxEnterScore, CanCreateCount, 
								PlayTurnCount, PlayTimeLimit, TimeAfterBeginCount, TimeOffLineCount,TimeNotBeginGame, TimeNotBeginAfterCreateRoom, CreateRight,BeginFreeTime,EndFreeTime,TimeRespondDismiss,ShareStorageRule)
					VALUES (@cbPlayMode, @wServerID,@wKindID, @cbIsJoinGame, @cbMinPeople, @cbMaxPeople, @lPersonalRoomTax, @lMinCellScore, @lMaxCellScore, @lMinEnterScore, @lMaxEnterScore, @wCanCreateCount, @wPlayTurnCount, 
							@wPlayTimeLimit, @wTimeAfterBeginCount, @wTimeOffLineCount, @wTimeNotBeginGame, @wTimeNotBeginAfterCreateRoom, @cbCreateRight,@wBeginFreeTime,@wEndFreeTime,@wTimeRespondDismiss,@strShareStorageRule)
			END
			ELSE
			BEGIN
					--更新私人房间参数
					UPDATE PersonalRoomInfo SET  ServerID = @dwServerID, IsJoinGame = @cbIsJoinGame, MinPeople = @cbMinPeople, MaxPeople = @cbMaxPeople,PersonalRoomTax = @lPersonalRoomTax, MinCellScore = @lMinCellScore,
									 PlayTurnCount = @wPlayTurnCount, PlayTimeLimit = @wPlayTimeLimit, TimeAfterBeginCount = @wTimeAfterBeginCount, CanCreateCount = @wCanCreateCount,
									TimeOffLineCount = @wTimeOffLineCount,TimeNotBeginGame = @wTimeNotBeginGame, TimeNotBeginAfterCreateRoom = @wTimeNotBeginAfterCreateRoom, CreateRight = @cbCreateRight,
									BeginFreeTime=@wBeginFreeTime,EndFreeTime=@wEndFreeTime, PlayMode = @cbPlayMode, MaxCellScore = @lMaxCellScore, MinEnterScore = @lMinEnterScore, MaxEnterScore = @lMaxEnterScore,
									TimeRespondDismiss=@wTimeRespondDismiss,ShareStorageRule=@strShareStorageRule
					WHERE KindID = @wKindID  AND PlayMode = @cbPlayMode
			END

	END
	ELSE
	BEGIN
		--更新私人房间参数
		UPDATE PersonalRoomInfo SET  ServerID = @dwServerID, IsJoinGame = @cbIsJoinGame, MinPeople = @cbMinPeople, MaxPeople = @cbMaxPeople,PersonalRoomTax = @lPersonalRoomTax, MinCellScore = @lMinCellScore,
									 PlayTurnCount = @wPlayTurnCount, PlayTimeLimit = @wPlayTimeLimit, TimeAfterBeginCount = @wTimeAfterBeginCount, CanCreateCount = @wCanCreateCount,
									TimeOffLineCount = @wTimeOffLineCount,TimeNotBeginGame = @wTimeNotBeginGame, TimeNotBeginAfterCreateRoom = @wTimeNotBeginAfterCreateRoom, CreateRight = @cbCreateRight,
									BeginFreeTime=@wBeginFreeTime,EndFreeTime=@wEndFreeTime, PlayMode = @cbPlayMode, MaxCellScore = @lMaxCellScore, MinEnterScore = @lMinEnterScore, MaxEnterScore = @lMaxEnterScore,
									TimeRespondDismiss=@wTimeRespondDismiss,ShareStorageRule=@strShareStorageRule
		WHERE KindID = @wKindID  AND PlayMode = @cbPlayMode
	END
	
	IF @cbPlayMode = 1 
	BEGIN
		--插入默认共享库存规则
		DECLARE @tableGroupID TABLE
		(
			GroupID BIGINT
		);
		
		-- 插入数据
		INSERT INTO @tableGroupID(GroupID) SELECT GroupID FROM WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupProperty 
		
		WHILE EXISTS(SELECT GroupID FROM @tableGroupID)
		BEGIN
			SET ROWCOUNT 1		
			
			DECLARE @GroupID INT
			SELECT @GroupID=GroupID  FROM @tableGroupID 
			
			IF EXISTS(SELECT * FROM WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty WHERE KindID=@wKindID AND GroupID=@GroupID AND PlayMode=@cbPlayMode)
			BEGIN
				UPDATE WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty SET ConfigSubStorageItem=@strShareStorageRule,CurSubStorageItem=@strCurSubStorageItem,ConfigSysStorage=@lConfigSysStorage,ConfigPlayerStorage=@lConfigPlayerStorage,ConfigParameterK=@lConfigParameterK,CurResetCount=@lCurResetCount WHERE KindID=@wKindID AND GroupID=@GroupID AND PlayMode=@cbPlayMode 
			END
		
			SET ROWCOUNT 0 
			DELETE FROM @tableGroupID WHERE GroupID=@GroupID
		END
		
		
		IF OBJECT_ID('tempdb..#tableGroupID') IS NOT NULL
		BEGIN
			DROP TABLE tableGroupID
		End
	END
	
	SET @strErrorDescribe=N'修改成功！'
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 解散私人房
CREATE PROC GSP_GR_DissumePersonalRoom
	@dwPersonalRoomID INT,						-- 约战房间ID
	@lRoomHostID INT,							-- 房主 ID
	@lTaxCount bigint,
	@cbPersonalRoomInfo varbinary(MAX),
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	DECLARE @tDissumDate datetime
	DECLARE @TaxRatio INT
	SET @TaxRatio = 0
	DECLARE @RoomTaxRatio INT
	DECLARE @ServerID INT
	DECLARE @TaxAgency	INT 
	SET @RoomTaxRatio = 0
	SET @ServerID = 0

	-- 获得时间,房间标识
	select @tDissumDate = DissumeDate, @ServerID = ServerID, @TaxAgency = TaxAgency from WHQJPlatformDB..StreamCreateTableFeeInfo where RoomID = @dwPersonalRoomID AND UserID = @lRoomHostID

	-- 获取对应房间的服务比例
	select @RoomTaxRatio = RevenueRatio from WHQJPlatformDB..GameRoomInfo where ServerID = @ServerID

	if @TaxAgency is null
	begin
		set @TaxAgency = 0
	end

	if @RoomTaxRatio is null or @RoomTaxRatio = 0
	begin
		set @TaxAgency = 1
	end

	declare @TaxRevenue int
	set @TaxRevenue = 0
	if @RoomTaxRatio is null or @RoomTaxRatio = 0
	begin
		set @RoomTaxRatio = @lTaxCount
	end
	else
	begin
		set @RoomTaxRatio =  @lTaxCount * @TaxAgency/@RoomTaxRatio
	end


	-- 写入创建记录
	UPDATE  WHQJPlatformDB..StreamCreateTableFeeInfo   SET  DissumeDate = GETDATE(),TaxCount = @lTaxCount, RoomScoreInfo = @cbPersonalRoomInfo , RoomStatus = 2  WHERE RoomID = @dwPersonalRoomID AND UserID = @lRoomHostID

	if @tDissumDate is null
	SET  @tDissumDate = GETDATE()

	--解锁
	DECLARE @temp TABLE
	(
	UserID INT
	);

	-- 将源表中的数据插入到表变量中
	INSERT INTO @temp(UserID)
	SELECT UserID FROM PersonalRoomScoreInfo WHERE RoomID = @dwPersonalRoomID
	ORDER BY UserID;

	-- 声明变量
	DECLARE
	@UserID AS INT,
	@firstname AS NVARCHAR(10),
	@lastname AS NVARCHAR(20);
     
	WHILE EXISTS(SELECT UserID FROM @temp)
	BEGIN
	-- 也可以使用top 1
	SET ROWCOUNT 1
	SELECT @UserID= UserID FROM @temp;
	DELETE FROM  WHQJTreasureDB.dbo.GameScoreLocker where UserID = @UserID;
	SET ROWCOUNT 0 
	DELETE FROM @temp WHERE UserID=@UserID;
	END

	select @tDissumDate AS DissumeDate
												
END

RETURN 0
GO

-----------------------------------------------


