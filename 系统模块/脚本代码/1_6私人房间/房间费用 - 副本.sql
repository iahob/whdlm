USE WHQJTreasureDB
GO



IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_InsertCreateRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_InsertCreateRecord]



SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

-----------------------------------------------------------------------
-- 创建房间记录
CREATE PROC GSP_GR_InsertCreateRecord
	@dwUserID INT,								-- 用户 I D
	@dwServerID INT,							-- 房间 标识
	@dwPersonalRoomID INT,						-- 约战房间标识
	@lCellScore INT,							-- 房间 底分
	@dwDrawCountLimit INT,						-- 局数限制
	@dwDrawTimeLimit INT,						-- 时间限制
	@szPassWord NVARCHAR(15),					-- 连接地址
	@wJoinGamePeopleCount TINYINT,				-- 人数限制
	@dwRoomTax BIGINT,							-- 私人房间税收
	@strClientAddr NVARCHAR(15),
	@cbPayMode TINYINT,							-- 支付模式
	@lNeedRoomCard TINYINT,						-- 理论需要的钻石
	@cbGameMode TINYINT,						-- 游戏模式
	@cbPlayMode		TINYINT,					-- 游戏模式 0 积分场  1 金币场
	@lGroupID		BIGINT,						-- 亲友圈ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	DECLARE @Fee INT
	DECLARE @Nicname NVARCHAR(31)
	DECLARE @lPersonalRoomTax BIGINT

	-- 查询费用 及 房主是否参与游戏
	DECLARE @wKindID INT
	SELECT  @wKindID = KindID FROM WHQJPlatformDBLink.WHQJPlatformDB.dbo.GameRoomInfo  WHERE ServerID = @dwServerID 
	-- 查询费用
	SELECT @Fee=TableFee FROM WHQJPlatformDBLink.WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID AND  PlayMode= @cbPlayMode
	IF @cbPlayMode = 0
	BEGIN
		IF @Fee IS NULL OR @Fee=0
		BEGIN
			SET @strErrorDescribe=N'数据库查询费用失败，请重新尝试！'
			RETURN 3
		END
	END
	ELSE
	BEGIN
		SET @Fee = 0
	END

	-- 获取创建房间玩家的昵称
	SELECT @Nicname =NickName FROM WHQJAccountsDBLink.WHQJAccountsDB.dbo.AccountsInfo WHERE UserID = @dwUserID
	IF @Nicname IS NULL
	SET @Nicname =''
		
	SELECT  @lPersonalRoomTax = PersonalRoomTax FROM WHQJPlatformDBLink.WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 
	
	--如果是消耗钻石，查询代理税收
	DECLARE @lTaxAgency BIGINT
	SELECT  @lTaxAgency = AgentScale FROM WHQJAccountsDBLink.WHQJAccountsDB.dbo.AccountsAgent WHERE UserID = @dwUserID 
	IF @lTaxAgency IS NOT NULL
	BEGIN
		SET @lPersonalRoomTax = @lTaxAgency
	END

	-- 写入消耗放开记录
	INSERT INTO WHQJPlatformDB..StreamCreateTableFeeInfo(UserID,NickName, ServerID, RoomID, CellScore, CountLimit,TimeLimit,CreateTableFee,CreateDate, TaxAgency, JoinGamePeopleCount,PayMode, RoomStatus, NeedRoomCard,GameMode,PlayMode,GroupID)
												VALUES(@dwUserID,@Nicname, @dwServerID, @dwPersonalRoomID, @lCellScore,@dwDrawCountLimit, @dwDrawTimeLimit, 0,GETDATE(), @dwRoomTax, @wJoinGamePeopleCount, @cbPayMode, 0, @lNeedRoomCard,@cbGameMode,@cbPlayMode,@lGroupID)	


END

RETURN 0
GO


