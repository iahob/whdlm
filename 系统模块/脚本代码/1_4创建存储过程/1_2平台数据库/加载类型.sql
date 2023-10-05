
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGameKindItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGameKindItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadLuaGameKindItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadLuaGameKindItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadH5GameKindItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadH5GameKindItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadU3DGameKindItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadU3DGameKindItem]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

------------------------------------------------------------------------------------------------------

--游戏类型
CREATE  PROCEDURE dbo.GSP_GP_LoadGameKindItem WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

--游戏类型
SELECT * FROM GameKindItem(NOLOCK) WHERE Nullity=0 ORDER BY SortID

RETURN 0

GO

----------------------------------------------------------------------------------------------------

--游戏类型
CREATE  PROCEDURE dbo.GSP_GP_LoadLuaGameKindItem
	@dwUserID INT								-- 用户标识 
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON


DECLARE @AgentID INT
DECLARE @UpAgentID INT
-- 执行逻辑
BEGIN 
	-- 判断用户是否单独配置游戏
	IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1)
	BEGIN
		--游戏类型
		SELECT * FROM MobileKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1 ) ORDER BY SortID		
	END
	ELSE
	BEGIN
		-- 如果用户是代理则返回代理配置游戏
		SELECT @AgentID=AgentID FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
		IF @AgentID>0
		BEGIN
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2)
			BEGIN
				--有配置则返回配置游戏
				SELECT * FROM MobileKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2 ) ORDER BY SortID	
			END
			ELSE
			BEGIN
				--有配置则返回配置游戏
				SELECT * FROM MobileKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
		-- 如果不是代理则找上级代理配置游戏
		ELSE
		BEGIN
			SELECT @UpAgentID=AgentID FROM WHQJAgentDB.dbo.AgentBelowInfo(NOLOCK) WHERE UserID=@dwUserID
			IF @UpAgentID IS NULL SET @UpAgentID=-100
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2)
			BEGIN
				-- 有上级代理配置则返回配置游戏
				SELECT * FROM MobileKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2 ) ORDER BY SortID
			END
			ELSE
			BEGIN
				-- 无上级代理配置则返回通用游戏
				SELECT * FROM MobileKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
	END		
END


RETURN 0

GO

----------------------------------------------------------------------------------------------------

--游戏类型
CREATE  PROCEDURE dbo.GSP_GP_LoadH5GameKindItem 
	@dwUserID INT								-- 用户标识
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

DECLARE @AgentID INT
DECLARE @UpAgentID INT
-- 执行逻辑
BEGIN 
	-- 判断用户是否单独配置游戏
	IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1)
	BEGIN
		--游戏类型
		SELECT * FROM H5GameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1 ) ORDER BY SortID		
	END
	ELSE
	BEGIN
		-- 如果用户是代理则返回代理配置游戏
		SELECT @AgentID=AgentID FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
		IF @AgentID>0
		BEGIN
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2)
			BEGIN
				--有配置则返回配置游戏
				SELECT * FROM H5GameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2 ) ORDER BY SortID	
			END
			ELSE
			BEGIN
				--有配置则返回配置游戏
				SELECT * FROM H5GameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
		-- 如果不是代理则找上级代理配置游戏
		ELSE
		BEGIN
			SELECT @UpAgentID=AgentID FROM WHQJAgentDB.dbo.AgentBelowInfo(NOLOCK) WHERE UserID=@dwUserID
			IF @UpAgentID IS NULL SET @UpAgentID=-100
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2)
			BEGIN
				-- 有上级代理配置则返回配置游戏
				SELECT * FROM H5GameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2 ) ORDER BY SortID
			END
			ELSE
			BEGIN
				-- 无上级代理配置则返回通用游戏
				SELECT * FROM H5GameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
	END		
END


RETURN 0

GO

----------------------------------------------------------------------------------------------------

--游戏类型
CREATE  PROCEDURE dbo.GSP_GP_LoadU3DGameKindItem 
	@dwUserID INT								-- 用户标识
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON


DECLARE @AgentID INT
DECLARE @UpAgentID INT
-- 执行逻辑
BEGIN 
	-- 判断用户是否单独配置游戏
	IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1)
	BEGIN
		--游戏类型
		SELECT * FROM U3DGameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1 ) ORDER BY SortID		
	END
	ELSE
	BEGIN
		-- 如果用户是代理则返回代理配置游戏
		SELECT @AgentID=AgentID FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
		IF @AgentID>0
		BEGIN
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2)
			BEGIN
				--有配置则返回配置游戏
				SELECT * FROM U3DGameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2 ) ORDER BY SortID	
			END
			ELSE
			BEGIN
				--有配置则返回配置游戏
				SELECT * FROM U3DGameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
		-- 如果不是代理则找上级代理配置游戏
		ELSE
		BEGIN
			SELECT @UpAgentID=AgentID FROM WHQJAgentDB.dbo.AgentBelowInfo(NOLOCK) WHERE UserID=@dwUserID
			IF @UpAgentID IS NULL SET @UpAgentID=-100
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2)
			BEGIN
				-- 有上级代理配置则返回配置游戏
				SELECT * FROM U3DGameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2 ) ORDER BY SortID
			END
			ELSE
			BEGIN
				-- 无上级代理配置则返回通用游戏
				SELECT * FROM U3DGameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
	END		
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
