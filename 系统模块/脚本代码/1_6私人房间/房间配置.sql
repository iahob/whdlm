USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_WritePersonalFeeParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_WritePersonalFeeParameter]
--删除配置
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_DeletePersonalFeeParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_DeletePersonalFeeParameter]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetPersonalParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_GetPersonalParameter]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetPersonalFeeParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_GetPersonalFeeParameter]


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetPersonalFeeParameterEx]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_GetPersonalFeeParameterEx]

---加载私人房间参数
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadPersonalRoomParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadPersonalRoomParameter]

---加载私人房间参数
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadPersonalRoomParameterByKindID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_GS_LoadPersonalRoomParameterByKindID
GO

----获取单个玩家进入过的房间
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_GetPersonalRoomUserScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_GetPersonalRoomUserScore]
GO


----写入结束时间
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_CloseRoomWriteDissumeTime]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_CloseRoomWriteDissumeTime]
GO

----获取私人房定制配置
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetPersonalRule]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_GetPersonalRule]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

------------------------------------------------------
---费用配置
CREATE PROC GSP_GS_WritePersonalFeeParameter
	@KindID	INT,							-- 房间类型
	@cbPlayMode TINYINT,					-- 游戏玩法
	@DrawCountLimit	INT,					-- 局数
	@DrawTimeLimit	INT,					-- 玩家掉线多长时间后解散桌子
	@TableFee	INT,						-- 房间创建多长时间后还未开始游戏解散桌子
	@AAPayFee   INT,						-- AA制费用
	@IniScore	INT,						-- 房间创建后多长时间后还未开始游戏解散桌子
	@cbGameMode TINYINT,					-- 游戏模式
	@strErrorDescribe NVARCHAR(127) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

BEGIN
	INSERT INTO PersonalTableFee (KindID,PlayMode, DrawCountLimit,DrawTimeLimit, TableFee,AAPayFee,IniScore,GameMode) 
	VALUES (@KindID, @cbPlayMode, @DrawCountLimit,@DrawTimeLimit, @TableFee,@AAPayFee, @IniScore,@cbGameMode) 
END

return 0
GO

--删除配置
CREATE PROC GSP_GS_DeletePersonalFeeParameter
	@KindID	INT,						-- 游戏类型
	@cbPlayMode TINYINT,				--	0 积分场  1 金币场
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

BEGIN
	
	DELETE  FROM PersonalTableFee WHERE  KindID = @KindID AND PlayMode = @cbPlayMode
	
END

return 0
GO


------------------------------------------------------
---费用配置
CREATE PROC GSP_MB_GetPersonalFeeParameterEx
	@dwKindID INT,								-- 游戏 I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

BEGIN

	SELECT PlayMode, DrawCountLimit, DrawTimeLimit, TableFee,AAPayFee,IniScore,GameMode FROM PersonalTableFee WHERE KindID=@dwKindID
END

return 0
GO

------------------------------------------------------
---费用配置
CREATE PROC GSP_MB_GetPersonalFeeParameter
	@dwKindID INT,								-- 游戏 I D
	@cbPlayMode TINYINT,						-- 游戏模式
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

BEGIN

	SELECT PlayMode, DrawCountLimit, DrawTimeLimit, TableFee,AAPayFee,IniScore,GameMode FROM PersonalTableFee WHERE KindID=@dwKindID	AND PlayMode = @cbPlayMode
END

return 0
GO

---------------------------------------------------------------
-- 加载配置
CREATE PROC GSP_GS_LoadPersonalRoomParameterByKindID
	@dwKindID INT,								-- 房间标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON	
	DECLARE @cbIsJoinGame TINYINT						-- 是否参与游戏
	DECLARE @cbMinPeople TINYINT						-- 参与游戏的最小人数
	DECLARE @cbMaxPeople TINYINT						-- 参与游戏的最大人数

	-- 积分配置
	DECLARE @lPersonalRoomTax  BIGINT					-- 服务比例
	DECLARE @lMaxCellScore BIGINT						-- 最大底分
	DECLARE @wCanCreateCount int						-- 可以创建的最大房间数目
	DECLARE @wPlayTurnCount INT							-- 房间能够进行游戏的最大局数
	DECLARE @cbCreateRight TINYINT						-- 创建私人房间权限

	-- 限制配置
	DECLARE @wPlayTimeLimit	INT						-- 房间能够进行游戏的最大时间
	DECLARE @wTimeAfterBeginCount	INT				-- 一局游戏开始后多长时间后解散桌子
	DECLARE @wTimeOffLineCount	INT					-- 玩家掉线多长时间后解散桌子
	DECLARE @wTimeNotBeginGame	INT					-- 房间创建多长时间后还未开始游戏解散桌子
	DECLARE @wTimeNotBeginAfterCreateRoom	INT		-- 房间创建后多长时间后还未开始游戏解散桌子

	DECLARE @wBeginFreeTime		INT					-- 免费时段-开始
	DECLARE @wEndFreeTime		INT					-- 免费时段-结束
	DECLARE	@lMinCellScore	BIGINT					-- 最小底分
	DECLARE @lMinEnterScore BIGINT					-- 最小进入分
	DECLARE @lMaxEnterScore BIGINT					-- 最大进入分
	DECLARE @lPlayMode		BIGINT					-- 最大进入分

-- 执行逻辑
BEGIN		
	SET @cbIsJoinGame = 0
	SET @cbMinPeople = 0
	SET @cbMaxPeople = 0	
	SET @lMaxCellScore = 0
	SET @lPersonalRoomTax = 0
	SET @lMaxCellScore = 0
	SET @wCanCreateCount = 0
	SET @wPlayTurnCount = 0
	SET @wPlayTimeLimit = 0
	SET @wTimeAfterBeginCount = 0
	SET @wTimeOffLineCount = 0
	SET @wTimeNotBeginGame = 0
	SET @wTimeNotBeginAfterCreateRoom = 0
	SET @cbCreateRight = 0
	SET @wBeginFreeTime = 0
	SET @wEndFreeTime = 0
	SET @lMinCellScore = 0
	SET @lMinEnterScore = 0
	SET @lMaxEnterScore = 0
	SET @lPlayMode = 0

	SELECT IsJoinGame, MinPeople, MaxPeople, PersonalRoomTax,MaxCellScore,CanCreateCount, PlayTurnCount, PlayTimeLimit,  TimeAfterBeginCount, 
	TimeOffLineCount, TimeNotBeginGame, TimeNotBeginAfterCreateRoom, CreateRight,
	BeginFreeTime,EndFreeTime, PlayMode, MinCellScore, MinEnterScore, MaxEnterScore
	FROM PersonalRoomInfo WHERE KindID = @dwKindID 

END

RETURN 0

GO

---------------------------------------------------------------
-- 加载配置
CREATE PROC GSP_GS_LoadPersonalRoomParameter
	@dwKindID INT,								-- 房间标识
	@cbPlayMode TINYINT,							-- 游戏模式
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON	
	DECLARE @cbIsJoinGame TINYINT						-- 是否参与游戏
	DECLARE @cbMinPeople TINYINT						-- 参与游戏的最小人数
	DECLARE @cbMaxPeople TINYINT						-- 参与游戏的最大人数

	-- 积分配置
	DECLARE @lPersonalRoomTax  BIGINT					-- 服务比例
	DECLARE @lMaxCellScore BIGINT						-- 最大底分
	DECLARE @wCanCreateCount int						-- 可以创建的最大房间数目
	DECLARE @wPlayTurnCount INT							-- 房间能够进行游戏的最大局数
	DECLARE @cbCreateRight TINYINT						-- 创建私人房间权限

	-- 限制配置
	DECLARE @wPlayTimeLimit	INT						-- 房间能够进行游戏的最大时间
	DECLARE @wTimeAfterBeginCount	INT				-- 一局游戏开始后多长时间后解散桌子
	DECLARE @wTimeOffLineCount	INT					-- 玩家掉线多长时间后解散桌子
	DECLARE @wTimeNotBeginGame	INT					-- 房间创建多长时间后还未开始游戏解散桌子
	DECLARE @wTimeNotBeginAfterCreateRoom	INT		-- 房间创建后多长时间后还未开始游戏解散桌子

	DECLARE @wBeginFreeTime		INT					-- 免费时段-开始
	DECLARE @wEndFreeTime		INT					-- 免费时段-结束
	DECLARE	@lMinCellScore	BIGINT					-- 最小底分
	DECLARE @lMinEnterScore BIGINT					-- 最小进入分
	DECLARE @lMaxEnterScore BIGINT					-- 最大进入分
	DECLARE @lPlayMode		BIGINT					-- 最大进入分

-- 执行逻辑
BEGIN		
	SET @cbIsJoinGame = 0
	SET @cbMinPeople = 0
	SET @cbMaxPeople = 0	
	SET @lMaxCellScore = 0
	SET @lPersonalRoomTax = 0
	SET @lMaxCellScore = 0
	SET @wCanCreateCount = 0
	SET @wPlayTurnCount = 0
	SET @wPlayTimeLimit = 0
	SET @wTimeAfterBeginCount = 0
	SET @wTimeOffLineCount = 0
	SET @wTimeNotBeginGame = 0
	SET @wTimeNotBeginAfterCreateRoom = 0
	SET @cbCreateRight = 0
	SET @wBeginFreeTime = 0
	SET @wEndFreeTime = 0
	SET @lMinCellScore = 0
	SET @lMinEnterScore = 0
	SET @lMaxEnterScore = 0
	SET @lPlayMode = 0

	SELECT IsJoinGame, MinPeople, MaxPeople, PersonalRoomTax,MaxCellScore,CanCreateCount, PlayTurnCount, PlayTimeLimit,  TimeAfterBeginCount, 
	TimeOffLineCount, TimeNotBeginGame, TimeNotBeginAfterCreateRoom, CreateRight,
	BeginFreeTime,EndFreeTime, PlayMode, MinCellScore, MinEnterScore, MaxEnterScore,TimeRespondDismiss
	FROM PersonalRoomInfo WHERE KindID = @dwKindID AND PlayMode = @cbPlayMode

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 请求私人房间信息
CREATE PROC GSP_GS_GetPersonalRoomUserScore
	@dwUserID INT,								-- 用户 I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 加载房间
	SELECT * FROM PersonalRoomScoreInfo WHERE UserID = @dwUserID ORDER BY WriteTime DESC

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- 请求私人房间信息
CREATE PROC GSP_GS_CloseRoomWriteDissumeTime
	@dwServerID INT,								-- 用户 I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	--删除此房间的所有锁表
	DELETE FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE ServerID = @dwServerID;
	-- 加载房间
	UPDATE StreamCreateTableFeeInfo SET DissumeDate = GetDATE(), RoomStatus = 2 WHERE ServerID = @dwServerID AND DissumeDate IS NULL

END

RETURN 0

GO

------------------------------------------------------
---费用配置
CREATE PROC GSP_MB_GetPersonalRule
	@dwKindID INT,								-- 游戏 I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

BEGIN
	SELECT PlayMode, PersonalRule FROM PersonalRoomInfo a, GameRoomInfo b WHERE a.ServerID = b.ServerID AND @dwKindID = a.KindID
	
	IF @@ROWCOUNT = 0
	BEGIN		
		SET @strErrorDescribe=N'找不到该游戏！'
		RETURN 1
	END
	
	return 0		
END

return 0
GO

---------------------------------------------------------------