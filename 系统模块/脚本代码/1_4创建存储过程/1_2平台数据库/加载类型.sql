
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

--��Ϸ����
CREATE  PROCEDURE dbo.GSP_GP_LoadGameKindItem WITH ENCRYPTION AS

--��������
SET NOCOUNT ON

--��Ϸ����
SELECT * FROM GameKindItem(NOLOCK) WHERE Nullity=0 ORDER BY SortID

RETURN 0

GO

----------------------------------------------------------------------------------------------------

--��Ϸ����
CREATE  PROCEDURE dbo.GSP_GP_LoadLuaGameKindItem
	@dwUserID INT								-- �û���ʶ 
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON


DECLARE @AgentID INT
DECLARE @UpAgentID INT
-- ִ���߼�
BEGIN 
	-- �ж��û��Ƿ񵥶�������Ϸ
	IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1)
	BEGIN
		--��Ϸ����
		SELECT * FROM MobileKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1 ) ORDER BY SortID		
	END
	ELSE
	BEGIN
		-- ����û��Ǵ����򷵻ش���������Ϸ
		SELECT @AgentID=AgentID FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
		IF @AgentID>0
		BEGIN
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2)
			BEGIN
				--�������򷵻�������Ϸ
				SELECT * FROM MobileKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2 ) ORDER BY SortID	
			END
			ELSE
			BEGIN
				--�������򷵻�������Ϸ
				SELECT * FROM MobileKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
		-- ������Ǵ��������ϼ�����������Ϸ
		ELSE
		BEGIN
			SELECT @UpAgentID=AgentID FROM WHQJAgentDB.dbo.AgentBelowInfo(NOLOCK) WHERE UserID=@dwUserID
			IF @UpAgentID IS NULL SET @UpAgentID=-100
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2)
			BEGIN
				-- ���ϼ����������򷵻�������Ϸ
				SELECT * FROM MobileKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2 ) ORDER BY SortID
			END
			ELSE
			BEGIN
				-- ���ϼ����������򷵻�ͨ����Ϸ
				SELECT * FROM MobileKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
	END		
END


RETURN 0

GO

----------------------------------------------------------------------------------------------------

--��Ϸ����
CREATE  PROCEDURE dbo.GSP_GP_LoadH5GameKindItem 
	@dwUserID INT								-- �û���ʶ
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON

DECLARE @AgentID INT
DECLARE @UpAgentID INT
-- ִ���߼�
BEGIN 
	-- �ж��û��Ƿ񵥶�������Ϸ
	IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1)
	BEGIN
		--��Ϸ����
		SELECT * FROM H5GameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1 ) ORDER BY SortID		
	END
	ELSE
	BEGIN
		-- ����û��Ǵ����򷵻ش���������Ϸ
		SELECT @AgentID=AgentID FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
		IF @AgentID>0
		BEGIN
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2)
			BEGIN
				--�������򷵻�������Ϸ
				SELECT * FROM H5GameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2 ) ORDER BY SortID	
			END
			ELSE
			BEGIN
				--�������򷵻�������Ϸ
				SELECT * FROM H5GameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
		-- ������Ǵ��������ϼ�����������Ϸ
		ELSE
		BEGIN
			SELECT @UpAgentID=AgentID FROM WHQJAgentDB.dbo.AgentBelowInfo(NOLOCK) WHERE UserID=@dwUserID
			IF @UpAgentID IS NULL SET @UpAgentID=-100
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2)
			BEGIN
				-- ���ϼ����������򷵻�������Ϸ
				SELECT * FROM H5GameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2 ) ORDER BY SortID
			END
			ELSE
			BEGIN
				-- ���ϼ����������򷵻�ͨ����Ϸ
				SELECT * FROM H5GameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
	END		
END


RETURN 0

GO

----------------------------------------------------------------------------------------------------

--��Ϸ����
CREATE  PROCEDURE dbo.GSP_GP_LoadU3DGameKindItem 
	@dwUserID INT								-- �û���ʶ
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON


DECLARE @AgentID INT
DECLARE @UpAgentID INT
-- ִ���߼�
BEGIN 
	-- �ж��û��Ƿ񵥶�������Ϸ
	IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1)
	BEGIN
		--��Ϸ����
		SELECT * FROM U3DGameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1 ) ORDER BY SortID		
	END
	ELSE
	BEGIN
		-- ����û��Ǵ����򷵻ش���������Ϸ
		SELECT @AgentID=AgentID FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
		IF @AgentID>0
		BEGIN
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2)
			BEGIN
				--�������򷵻�������Ϸ
				SELECT * FROM U3DGameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2 ) ORDER BY SortID	
			END
			ELSE
			BEGIN
				--�������򷵻�������Ϸ
				SELECT * FROM U3DGameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
		-- ������Ǵ��������ϼ�����������Ϸ
		ELSE
		BEGIN
			SELECT @UpAgentID=AgentID FROM WHQJAgentDB.dbo.AgentBelowInfo(NOLOCK) WHERE UserID=@dwUserID
			IF @UpAgentID IS NULL SET @UpAgentID=-100
			IF EXISTS(SELECT ID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2)
			BEGIN
				-- ���ϼ����������򷵻�������Ϸ
				SELECT * FROM U3DGameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2 ) ORDER BY SortID
			END
			ELSE
			BEGIN
				-- ���ϼ����������򷵻�ͨ����Ϸ
				SELECT * FROM U3DGameKindItem(NOLOCK) WHERE Nullity=0 AND KindID IN (SELECT KindID FROM WHQJAccountsDB.dbo.AccountsGame(NOLOCK) WHERE TypeID=0 ) ORDER BY SortID
			END
		END
	END		
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
