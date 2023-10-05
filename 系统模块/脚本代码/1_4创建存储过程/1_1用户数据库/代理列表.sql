
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

-- �����б�
CREATE PROC GSP_GP_LoadAgentGameKindItem
	@dwUserID INT,								-- �û���ʶ
	@dwDeviceID INT,							-- �豸��ʶ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

DECLARE @AgentID INT
DECLARE @UpAgentID INT
-- ִ���߼�
BEGIN 
	-- �ж��û��Ƿ񵥶�������Ϸ
	IF EXISTS(SELECT ID FROM AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1)
	BEGIN
		SELECT KindID,SortID FROM AccountsGame(NOLOCK) WHERE UniteID=@dwUserID AND TypeID=1
	END
	ELSE
	BEGIN
		-- ����û��Ǵ����򷵻ش���������Ϸ
		SELECT @AgentID=AgentID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
		IF @AgentID>0
		BEGIN
			IF EXISTS(SELECT ID FROM AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2)
			BEGIN
				-- �������򷵻�������Ϸ
				SELECT KindID,SortID FROM AccountsGame(NOLOCK) WHERE UniteID=@AgentID AND TypeID=2
			END
			ELSE
			BEGIN
				-- �������򷵻�ͨ����Ϸ
				SELECT KindID,SortID FROM AccountsGame(NOLOCK) WHERE TypeID=0
			END
		END
		-- ������Ǵ��������ϼ�����������Ϸ
		ELSE
		BEGIN
			SELECT @UpAgentID=AgentID FROM WHQJAgentDB.dbo.AgentBelowInfo(NOLOCK) WHERE UserID=@dwUserID
			IF @UpAgentID IS NULL SET @UpAgentID=-100
			IF EXISTS(SELECT ID FROM AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2)
			BEGIN
				-- ���ϼ����������򷵻�������Ϸ
				SELECT KindID,SortID FROM AccountsGame(NOLOCK) WHERE UniteID=@UpAgentID AND TypeID=2
			END
			ELSE
			BEGIN
				-- ���ϼ����������򷵻�ͨ����Ϸ
				SELECT KindID,SortID FROM AccountsGame(NOLOCK) WHERE TypeID=0
			END
		END
	END		
END

GO


