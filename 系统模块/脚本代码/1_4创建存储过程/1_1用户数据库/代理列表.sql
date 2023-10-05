
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAgentGameKindItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAgentGameKindItem]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO



----------------------------------------------------------------------------------------------------

-- 代理列表
CREATE PROC GSP_GP_LoadAgentGameKindItem
	@dwUserID INT,								-- 用户标识
	@dwDeviceID INT,							-- 设备标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @AgentID INT
DECLARE @UpAgentID INT
-- 执行逻辑
BEGIN 
	-- 判断用户是否单独配置游戏
	IF EXISTS(SELECT ID FROM AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1)
	BEGIN
		SELECT KindID,SortID FROM AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1
	END
	ELSE
	BEGIN
		-- 如果用户是代理则返回代理配置游戏
		SELECT @AgentID=AgentID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
		IF @AgentID>0
		BEGIN
			IF EXISTS(SELECT ID FROM AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2)
			BEGIN
				-- 有配置则返回配置游戏
				SELECT KindID,SortID FROM AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2
			END
			ELSE
			BEGIN
				-- 无配置则返回通用游戏
				SELECT KindID,SortID FROM AccountsGame(NOLOCK) WHERE TypeID=0
			END
		END
		-- 如果不是代理则找上级代理配置游戏
		ELSE
		BEGIN
			SELECT @UpAgentID=AgentID FROM WHQJAgentDB.dbo.AgentBelowInfo(NOLOCK) WHERE UserID=@dwUserID
			IF @UpAgentID IS NULL SET @UpAgentID=-100
			IF EXISTS(SELECT ID FROM AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2)
			BEGIN
				-- 有上级代理配置则返回配置游戏
				SELECT KindID,SortID FROM AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2
			END
			ELSE
			BEGIN
				-- 无上级代理配置则返回通用游戏
				SELECT KindID,SortID FROM AccountsGame(NOLOCK) WHERE TypeID=0
			END
		END
	END		
END

GO


