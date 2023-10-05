
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_InsertGameRoom]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_InsertGameRoom]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ModifyGameRoom]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_ModifyGameRoom]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_DeleteGameRoom]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_DeleteGameRoom]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadGameRoomItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadGameRoomItem]
GO

--关闭房间后解锁房间里面的玩家
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_UnLock_Server]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_UnLock_Server]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ModifyConfigSubStorageItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_ModifyConfigSubStorageItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ModifyCurSubStorageItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_ModifyCurSubStorageItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadShareStorageItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadShareStorageItem]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 插入房间
CREATE  PROCEDURE dbo.GSP_GS_InsertGameRoom

	-- 索引变量
	@wGameID INT,								-- 模块标识
	@wKindID INT,								-- 类型标识
	@wNodeID INT,								-- 挂接标识
	@wSortID INT,								-- 排序标识

	-- 积分配置
	@lCellScore INT,							-- 单元积分
	@wRevenueRatio INT,							-- 服务比例
	@wServiceRatio INT,							-- 入场比例
	@lServiceScore BIGINT,						-- 入场费

	-- 限制配置
	@lRestrictScore	BIGINT,						-- 限制积分
	@lMinTableScore	BIGINT,						-- 最低积分
	@lMaxTableScore	BIGINT,						-- 最高积分
	@lMinEnterScore	BIGINT,						-- 最低积分
	@lMaxEnterScore	BIGINT,						-- 最高积分

	-- 会员限制
	@cbMinEnterMember INT,						-- 最低会员
	@cbMaxEnterMember INT,						-- 最高会员

	-- 房间配置
	@dwServerRule INT,							-- 房间规则
	@dwAttachUserRight INT,						-- 附加权限

	-- 房间属性
	@wMaxPlayer INT,							-- 游戏人数
	@wTableCount INT,							-- 桌子数目
	@wServerPort INT,							-- 服务端口
	@wServerKind INT,							-- 房间类型
	@wServerType INT,							-- 服务类型
	@wServerLevel INT,							-- 房间等级
	@strServerName NVARCHAR(32),				-- 房间名字
	@strServerPasswd NVARCHAR(32),				-- 房间密码

	-- 分组属性
	@cbDistributeRule INT,						-- 分组规则
	@wMinDistributeUser INT,					-- 最少人数	
	@wDistributeTimeSpace INT,					-- 分组间隔
	@wDistributeDrawCount INT,					-- 分组局数
	@wMinPartakeGameUser INT,					-- 最少人数
	@wMaxPartakeGameUser INT,					-- 最多人数	

	-- 连接信息
	@strDataBaseName NVARCHAR(32),				-- 数据库名
	@strDataBaseAddr NVARCHAR(15),				-- 连接地址

	-- 数据设置
	@strCustomRule NVARCHAR(2048),				-- 定制规则
	@strServiceMachine NVARCHAR(32),			-- 服务机器
	@strPersonalRule NVARCHAR(2048),			-- 约战定制规则	
	@strShareStorageRule NVARCHAR(1024),		-- 共享库存规则	
	
	-- 输出信息
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息

WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查找模块
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM GameGameItem(NOLOCK) WHERE GameID=@wGameID

	-- 存在判断
	IF @GameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'相应的游戏服务组件信息不存在，房间创建失败！'
		RETURN 1
	END

	-- 获取端口
	IF @wServerPort<>0
	BEGIN
		
		-- 获取端口
		DECLARE @ServerPort INT
		SELECT @ServerPort=ServerPort FROM GameRoomInfo(NOLOCK) WHERE ServerPort=@wServerPort AND ServiceMachine=@strServiceMachine

		-- 端口判断
		IF @ServerPort=@wServerPort
		BEGIN
			SET @strErrorDescribe=N'存在服务端口相同的游戏房间，房间创建失败！'
			RETURN 1
		END

	END

	-- 创建房间
	INSERT INTO GameRoomInfo (ServerName, KindID, NodeID, SortID, GameID, TableCount, ServerPort, ServerKind, ServerType, ServerLevel, CellScore,
		RevenueRatio, ServiceRatio, ServiceScore, RestrictScore, MinTableScore, MaxTableScore, MinEnterScore, MaxEnterScore, MinEnterMember, MaxEnterMember,
		MaxPlayer, ServerRule, DistributeRule, MinDistributeUser, DistributeTimeSpace, DistributeDrawCount,MinPartakeGameUser,
		MaxPartakeGameUser, AttachUserRight, ServerPasswd, DataBaseName, DataBaseAddr, ServiceMachine, CustomRule,PersonalRule,ShareStorageRule)
	VALUES (@strServerName, @wKindID, @wNodeID, @wSortID, @wGameID, @wTableCount, @wServerPort, @wServerKind, @wServerType, @wServerLevel, @lCellScore,
		@wRevenueRatio, @wServiceRatio, @lServiceScore, @lRestrictScore, @lMinTableScore, @lMaxTableScore, @lMinEnterScore, @lMaxEnterScore, @cbMinEnterMember,
		@cbMaxEnterMember, @wMaxPlayer, @dwServerRule, @cbDistributeRule, @wMinDistributeUser,@wDistributeTimeSpace,
		@wDistributeDrawCount, @wMinPartakeGameUser,@wMaxPartakeGameUser, @dwAttachUserRight, @strServerPasswd, @strDataBaseName, @strDataBaseAddr, @strServiceMachine, @strCustomRule,@strPersonalRule,@strShareStorageRule)

	-- 加载房间
	SELECT GameRoomInfo.*, GameGameItem.GameName, GameGameItem.ServerVersion, GameGameItem.ClientVersion, GameGameItem.ServerDLLName,
		GameGameItem.ClientExeName FROM GameRoomInfo(NOLOCK), GameGameItem(NOLOCK)
	WHERE GameRoomInfo.GameID=GameGameItem.GameID AND ServerID=SCOPE_IDENTITY()

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改房间
CREATE  PROCEDURE dbo.GSP_GS_ModifyGameRoom

	-- 索引变量
	@wServerID INT,								-- 房间标识

	-- 挂接属性
	@wKindID INT,								-- 类型标识
	@wNodeID INT,								-- 挂接标识
	@wSortID INT,								-- 排序标识

	-- 服务比例配置
	@lCellScore INT,							-- 单元积分
	@wRevenueRatio INT,							-- 服务比例
	@wServiceRatio INT,							-- 入场比例
	@lServiceScore BIGINT,						-- 入场费

	-- 限制配置
	@lRestrictScore	BIGINT,						-- 限制积分
	@lMinTableScore	BIGINT,						-- 最低积分
	@lMaxTableScore	BIGINT,						-- 最高积分
	@lMinEnterScore	BIGINT,						-- 最低积分
	@lMaxEnterScore	BIGINT,						-- 最高积分

	-- 会员限制
	@cbMinEnterMember INT,						-- 最低会员
	@cbMaxEnterMember INT,						-- 最高会员

	-- 房间配置
	@dwServerRule INT,							-- 房间规则
	@dwAttachUserRight INT,						-- 附加权限

	-- 房间属性
	@wMaxPlayer INT,							-- 游戏人数
	@wTableCount INT,							-- 桌子数目
	@wServerPort INT,							-- 服务端口
	@wServerKind INT,							-- 房间类型
	@wServerType INT,							-- 服务类型
	@wServerLevel INT,							-- 房间等级
	@strServerName NVARCHAR(32),				-- 房间名字
	@strServerPasswd NVARCHAR(32),				-- 房间密码

	-- 分组属性
	@cbDistributeRule INT,						-- 分组规则
	@wMinDistributeUser INT,					-- 最少人数	
	@wDistributeTimeSpace INT,					-- 分组间隔
	@wDistributeDrawCount INT,					-- 分组局数
	@wMinPartakeGameUser INT,					-- 最少人数
	@wMaxPartakeGameUser INT,					-- 最多人数	

	-- 连接信息
	@strDataBaseName NVARCHAR(32),				-- 数据库名
	@strDataBaseAddr NVARCHAR(15),				-- 连接地址

	-- 数据设置
	@strCustomRule NVARCHAR(2048),				-- 定制规则
	@strServiceMachine NVARCHAR(32),			-- 服务机器
	@strPersonalRule NVARCHAR(2048),			-- 约战定制规则
	@strShareStorageRule NVARCHAR(1024),		-- 共享库存规则	
	
	-- 输出信息
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
		RETURN 2
	END

	-- 获取端口
	IF @wServerPort<>0
	BEGIN
		
		-- 获取端口
		DECLARE @ServerPort INT
		SELECT @ServerPort=ServerPort FROM GameRoomInfo(NOLOCK) WHERE ServerPort=@wServerPort AND ServiceMachine=@strServiceMachine AND ServerID<>@wServerID

		-- 端口判断
		IF @ServerPort=@wServerPort
		BEGIN
			SET @strErrorDescribe=N'存在服务端口相同的游戏房间，房间修改失败！'
			RETURN 1
		END

	END

	-- 更新房间
	UPDATE GameRoomInfo SET ServerName=@strServerName, KindID=@wKindID, NodeID=@wNodeID, SortID=@wSortID, TableCount=@wTableCount,
		ServerPort=@wServerPort, ServerKind=@wServerKind, ServerType=@wServerType, ServerLevel=@wServerLevel, CellScore=@lCellScore, RevenueRatio=@wRevenueRatio, ServiceRatio=@wServiceRatio,
		ServiceScore=@lServiceScore,RestrictScore=@lRestrictScore, MinTableScore=@lMinTableScore, MaxTableScore=@lMaxTableScore, MinEnterScore=@lMinEnterScore, 
		MaxEnterScore=@lMaxEnterScore,MinEnterMember=@cbMinEnterMember, MaxEnterMember=@cbMaxEnterMember, MaxPlayer=@wMaxPlayer, 
		ServerRule=@dwServerRule,DistributeRule=@cbDistributeRule, MinDistributeUser=@wMinDistributeUser,DistributeTimeSpace=@wDistributeTimeSpace, 
		DistributeDrawCount=@wDistributeDrawCount, MinPartakeGameUser=@wMinPartakeGameUser,MaxPartakeGameUser=@wMaxPartakeGameUser,
		AttachUserRight=@dwAttachUserRight, ServerPasswd=@strServerPasswd, DataBaseName=@strDataBaseName, DataBaseAddr=@strDataBaseAddr, 
		ServiceMachine=@strServiceMachine,CustomRule=@strCustomRule,PersonalRule = @strPersonalRule,ShareStorageRule=@strShareStorageRule
	WHERE ServerID=@ServerID
	
	-- 加载房间
	SELECT GameRoomInfo.*, GameGameItem.GameName, GameGameItem.ServerVersion, GameGameItem.ClientVersion, GameGameItem.ServerDLLName,
		GameGameItem.ClientExeName FROM GameRoomInfo(NOLOCK), GameGameItem(NOLOCK)
	WHERE GameRoomInfo.GameID=GameGameItem.GameID AND ServerID=@wServerID

END

RETURN 0

GO

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

	--删除房间
	DELETE GameRoomInfo WHERE ServerID=@wServerID

	--删除AI批次
	DELETE WHQJAccountsDB..AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载房间
CREATE  PROCEDURE dbo.GSP_GS_LoadGameRoomItem
	@wServerID INT,								-- 房间标识
	@strMachineID NCHAR(32),					-- 服务机器
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 加载房间
	SELECT GameRoomInfo.*, GameGameItem.GameName, GameGameItem.ServerVersion, GameGameItem.ClientVersion,
		GameGameItem.ServerDLLName, GameGameItem.ClientExeName FROM GameRoomInfo(NOLOCK), GameGameItem(NOLOCK)
	WHERE GameRoomInfo.GameID=GameGameItem.GameID AND ((@wServerID=ServerID) OR (@wServerID=0))
		AND ((ServiceMachine=@strMachineID) OR (@strMachineID=''))
	ORDER BY GameName,ServerName

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- 关闭房间后解锁房间里面的玩家
CREATE PROC GSP_GS_UnLock_Server
	@dwServerID INT,							-- 用户 I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	--删除此房间的所有锁表
	DELETE FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE ServerID = @dwServerID;
	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改配置库存子项
CREATE  PROCEDURE dbo.GSP_GS_ModifyConfigSubStorageItem
	@dwGroupID INT,
	@wKindID INT,								-- 房间标识
	@cbPlayMode TINYINT,
	@strConfigSubStorageItem NVARCHAR(1024),
	@strCurSubStorageItem NVARCHAR(1024),
	@lConfigSysStorage BIGINT,
	@lConfigPlayerStorage BIGINT,
	@lConfigParameterK BIGINT,
	@lCurResetCount BIGINT

WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	UPDATE PersonalRoomInfo SET ShareStorageRule=@strConfigSubStorageItem WHERE KindID=@wKindID AND PlayMode=@cbPlayMode
	
	-- 查找房间
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM PersonalRoomInfo(NOLOCK) WHERE KindID=@wKindID AND PlayMode=@cbPlayMode
	
	UPDATE GameRoomInfo SET ShareStorageRule=@strConfigSubStorageItem WHERE ServerID=@ServerID

	UPDATE WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty SET ConfigSubStorageItem=@strConfigSubStorageItem,CurSubStorageItem=@strCurSubStorageItem,ConfigSysStorage=@lConfigSysStorage,ConfigPlayerStorage=@lConfigPlayerStorage,ConfigParameterK=@lConfigParameterK,CurResetCount=@lCurResetCount WHERE KindID=@wKindID AND GroupID=@dwGroupID AND PlayMode=@cbPlayMode
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改当前库存子项
CREATE  PROCEDURE dbo.GSP_GS_ModifyCurSubStorageItem
	@dwGroupID INT,
	@wKindID INT,								-- 房间标识
	@cbPlayMode TINYINT,
	@strCurSubStorageItem NVARCHAR(1024),
	@lCurResetCount BIGINT	
	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	UPDATE WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty SET CurSubStorageItem=@strCurSubStorageItem,CurResetCount=@lCurResetCount WHERE KindID=@wKindID AND GroupID=@dwGroupID AND PlayMode=@cbPlayMode
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载共享库存
CREATE  PROCEDURE dbo.GSP_GS_LoadShareStorageItem
	@wKindID INT,								
	@cbPlayMode TINYINT,
	@dwGroupID INT
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	IF @dwGroupID <> 0
	BEGIN
		SELECT GroupID,KindID,PlayMode,ConfigSubStorageItem,CurSubStorageItem FROM WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty WHERE KindID=@wKindID AND PlayMode=@cbPlayMode AND GroupID=@dwGroupID
	END 
	ELSE
	BEGIN
		SELECT GroupID,KindID,PlayMode,ConfigSubStorageItem,CurSubStorageItem FROM WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty WHERE KindID=@wKindID AND PlayMode=@cbPlayMode
	END
END

RETURN 0

GO