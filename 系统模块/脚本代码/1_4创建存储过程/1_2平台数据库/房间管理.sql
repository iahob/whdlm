
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

--�رշ�������������������
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

-- ���뷿��
CREATE  PROCEDURE dbo.GSP_GS_InsertGameRoom

	-- ��������
	@wGameID INT,								-- ģ���ʶ
	@wKindID INT,								-- ���ͱ�ʶ
	@wNodeID INT,								-- �ҽӱ�ʶ
	@wSortID INT,								-- �����ʶ

	-- ��������
	@lCellScore INT,							-- ��Ԫ����
	@wRevenueRatio INT,							-- �������
	@wServiceRatio INT,							-- �볡����
	@lServiceScore BIGINT,						-- �볡��

	-- ��������
	@lRestrictScore	BIGINT,						-- ���ƻ���
	@lMinTableScore	BIGINT,						-- ��ͻ���
	@lMaxTableScore	BIGINT,						-- ��߻���
	@lMinEnterScore	BIGINT,						-- ��ͻ���
	@lMaxEnterScore	BIGINT,						-- ��߻���

	-- ��Ա����
	@cbMinEnterMember INT,						-- ��ͻ�Ա
	@cbMaxEnterMember INT,						-- ��߻�Ա

	-- ��������
	@dwServerRule INT,							-- �������
	@dwAttachUserRight INT,						-- ����Ȩ��

	-- ��������
	@wMaxPlayer INT,							-- ��Ϸ����
	@wTableCount INT,							-- ������Ŀ
	@wServerPort INT,							-- ����˿�
	@wServerKind INT,							-- ��������
	@wServerType INT,							-- ��������
	@wServerLevel INT,							-- ����ȼ�
	@strServerName NVARCHAR(32),				-- ��������
	@strServerPasswd NVARCHAR(32),				-- ��������

	-- ��������
	@cbDistributeRule INT,						-- �������
	@wMinDistributeUser INT,					-- ��������	
	@wDistributeTimeSpace INT,					-- ������
	@wDistributeDrawCount INT,					-- �������
	@wMinPartakeGameUser INT,					-- ��������
	@wMaxPartakeGameUser INT,					-- �������	

	-- ������Ϣ
	@strDataBaseName NVARCHAR(32),				-- ���ݿ���
	@strDataBaseAddr NVARCHAR(15),				-- ���ӵ�ַ

	-- ��������
	@strCustomRule NVARCHAR(2048),				-- ���ƹ���
	@strServiceMachine NVARCHAR(32),			-- �������
	@strPersonalRule NVARCHAR(2048),			-- Լս���ƹ���	
	@strShareStorageRule NVARCHAR(1024),		-- ���������	
	
	-- �����Ϣ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ����ģ��
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM GameGameItem(NOLOCK) WHERE GameID=@wGameID

	-- �����ж�
	IF @GameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ӧ����Ϸ���������Ϣ�����ڣ����䴴��ʧ�ܣ�'
		RETURN 1
	END

	-- ��ȡ�˿�
	IF @wServerPort<>0
	BEGIN
		
		-- ��ȡ�˿�
		DECLARE @ServerPort INT
		SELECT @ServerPort=ServerPort FROM GameRoomInfo(NOLOCK) WHERE ServerPort=@wServerPort AND ServiceMachine=@strServiceMachine

		-- �˿��ж�
		IF @ServerPort=@wServerPort
		BEGIN
			SET @strErrorDescribe=N'���ڷ���˿���ͬ����Ϸ���䣬���䴴��ʧ�ܣ�'
			RETURN 1
		END

	END

	-- ��������
	INSERT INTO GameRoomInfo (ServerName, KindID, NodeID, SortID, GameID, TableCount, ServerPort, ServerKind, ServerType, ServerLevel, CellScore,
		RevenueRatio, ServiceRatio, ServiceScore, RestrictScore, MinTableScore, MaxTableScore, MinEnterScore, MaxEnterScore, MinEnterMember, MaxEnterMember,
		MaxPlayer, ServerRule, DistributeRule, MinDistributeUser, DistributeTimeSpace, DistributeDrawCount,MinPartakeGameUser,
		MaxPartakeGameUser, AttachUserRight, ServerPasswd, DataBaseName, DataBaseAddr, ServiceMachine, CustomRule,PersonalRule,ShareStorageRule)
	VALUES (@strServerName, @wKindID, @wNodeID, @wSortID, @wGameID, @wTableCount, @wServerPort, @wServerKind, @wServerType, @wServerLevel, @lCellScore,
		@wRevenueRatio, @wServiceRatio, @lServiceScore, @lRestrictScore, @lMinTableScore, @lMaxTableScore, @lMinEnterScore, @lMaxEnterScore, @cbMinEnterMember,
		@cbMaxEnterMember, @wMaxPlayer, @dwServerRule, @cbDistributeRule, @wMinDistributeUser,@wDistributeTimeSpace,
		@wDistributeDrawCount, @wMinPartakeGameUser,@wMaxPartakeGameUser, @dwAttachUserRight, @strServerPasswd, @strDataBaseName, @strDataBaseAddr, @strServiceMachine, @strCustomRule,@strPersonalRule,@strShareStorageRule)

	-- ���ط���
	SELECT GameRoomInfo.*, GameGameItem.GameName, GameGameItem.ServerVersion, GameGameItem.ClientVersion, GameGameItem.ServerDLLName,
		GameGameItem.ClientExeName FROM GameRoomInfo(NOLOCK), GameGameItem(NOLOCK)
	WHERE GameRoomInfo.GameID=GameGameItem.GameID AND ServerID=SCOPE_IDENTITY()

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸ķ���
CREATE  PROCEDURE dbo.GSP_GS_ModifyGameRoom

	-- ��������
	@wServerID INT,								-- �����ʶ

	-- �ҽ�����
	@wKindID INT,								-- ���ͱ�ʶ
	@wNodeID INT,								-- �ҽӱ�ʶ
	@wSortID INT,								-- �����ʶ

	-- �����������
	@lCellScore INT,							-- ��Ԫ����
	@wRevenueRatio INT,							-- �������
	@wServiceRatio INT,							-- �볡����
	@lServiceScore BIGINT,						-- �볡��

	-- ��������
	@lRestrictScore	BIGINT,						-- ���ƻ���
	@lMinTableScore	BIGINT,						-- ��ͻ���
	@lMaxTableScore	BIGINT,						-- ��߻���
	@lMinEnterScore	BIGINT,						-- ��ͻ���
	@lMaxEnterScore	BIGINT,						-- ��߻���

	-- ��Ա����
	@cbMinEnterMember INT,						-- ��ͻ�Ա
	@cbMaxEnterMember INT,						-- ��߻�Ա

	-- ��������
	@dwServerRule INT,							-- �������
	@dwAttachUserRight INT,						-- ����Ȩ��

	-- ��������
	@wMaxPlayer INT,							-- ��Ϸ����
	@wTableCount INT,							-- ������Ŀ
	@wServerPort INT,							-- ����˿�
	@wServerKind INT,							-- ��������
	@wServerType INT,							-- ��������
	@wServerLevel INT,							-- ����ȼ�
	@strServerName NVARCHAR(32),				-- ��������
	@strServerPasswd NVARCHAR(32),				-- ��������

	-- ��������
	@cbDistributeRule INT,						-- �������
	@wMinDistributeUser INT,					-- ��������	
	@wDistributeTimeSpace INT,					-- ������
	@wDistributeDrawCount INT,					-- �������
	@wMinPartakeGameUser INT,					-- ��������
	@wMaxPartakeGameUser INT,					-- �������	

	-- ������Ϣ
	@strDataBaseName NVARCHAR(32),				-- ���ݿ���
	@strDataBaseAddr NVARCHAR(15),				-- ���ӵ�ַ

	-- ��������
	@strCustomRule NVARCHAR(2048),				-- ���ƹ���
	@strServiceMachine NVARCHAR(32),			-- �������
	@strPersonalRule NVARCHAR(2048),			-- Լս���ƹ���
	@strShareStorageRule NVARCHAR(1024),		-- ���������	
	
	-- �����Ϣ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ҷ���
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM GameRoomInfo(NOLOCK) WHERE ServerID=@wServerID

	-- ����ж�
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ϸ���䲻���ڻ����Ѿ���ɾ���ˣ������޸�ʧ�ܣ�'
		RETURN 2
	END

	-- ��ȡ�˿�
	IF @wServerPort<>0
	BEGIN
		
		-- ��ȡ�˿�
		DECLARE @ServerPort INT
		SELECT @ServerPort=ServerPort FROM GameRoomInfo(NOLOCK) WHERE ServerPort=@wServerPort AND ServiceMachine=@strServiceMachine AND ServerID<>@wServerID

		-- �˿��ж�
		IF @ServerPort=@wServerPort
		BEGIN
			SET @strErrorDescribe=N'���ڷ���˿���ͬ����Ϸ���䣬�����޸�ʧ�ܣ�'
			RETURN 1
		END

	END

	-- ���·���
	UPDATE GameRoomInfo SET ServerName=@strServerName, KindID=@wKindID, NodeID=@wNodeID, SortID=@wSortID, TableCount=@wTableCount,
		ServerPort=@wServerPort, ServerKind=@wServerKind, ServerType=@wServerType, ServerLevel=@wServerLevel, CellScore=@lCellScore, RevenueRatio=@wRevenueRatio, ServiceRatio=@wServiceRatio,
		ServiceScore=@lServiceScore,RestrictScore=@lRestrictScore, MinTableScore=@lMinTableScore, MaxTableScore=@lMaxTableScore, MinEnterScore=@lMinEnterScore, 
		MaxEnterScore=@lMaxEnterScore,MinEnterMember=@cbMinEnterMember, MaxEnterMember=@cbMaxEnterMember, MaxPlayer=@wMaxPlayer, 
		ServerRule=@dwServerRule,DistributeRule=@cbDistributeRule, MinDistributeUser=@wMinDistributeUser,DistributeTimeSpace=@wDistributeTimeSpace, 
		DistributeDrawCount=@wDistributeDrawCount, MinPartakeGameUser=@wMinPartakeGameUser,MaxPartakeGameUser=@wMaxPartakeGameUser,
		AttachUserRight=@dwAttachUserRight, ServerPasswd=@strServerPasswd, DataBaseName=@strDataBaseName, DataBaseAddr=@strDataBaseAddr, 
		ServiceMachine=@strServiceMachine,CustomRule=@strCustomRule,PersonalRule = @strPersonalRule,ShareStorageRule=@strShareStorageRule
	WHERE ServerID=@ServerID
	
	-- ���ط���
	SELECT GameRoomInfo.*, GameGameItem.GameName, GameGameItem.ServerVersion, GameGameItem.ClientVersion, GameGameItem.ServerDLLName,
		GameGameItem.ClientExeName FROM GameRoomInfo(NOLOCK), GameGameItem(NOLOCK)
	WHERE GameRoomInfo.GameID=GameGameItem.GameID AND ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ������
CREATE  PROCEDURE dbo.GSP_GS_DeleteGameRoom
	@wServerID INT,								-- �����ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ҷ���
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM GameRoomInfo(NOLOCK) WHERE ServerID=@wServerID

	-- ����ж�
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ϸ���䲻���ڻ����Ѿ���ɾ���ˣ������޸�ʧ�ܣ�'
		RETURN 1
	END

	--ɾ������
	DELETE GameRoomInfo WHERE ServerID=@wServerID

	--ɾ��AI����
	DELETE WHQJAccountsDB..AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ط���
CREATE  PROCEDURE dbo.GSP_GS_LoadGameRoomItem
	@wServerID INT,								-- �����ʶ
	@strMachineID NCHAR(32),					-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ط���
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

-- �رշ�������������������
CREATE PROC GSP_GS_UnLock_Server
	@dwServerID INT,							-- �û� I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	--ɾ���˷������������
	DELETE FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE ServerID = @dwServerID;
	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸����ÿ������
CREATE  PROCEDURE dbo.GSP_GS_ModifyConfigSubStorageItem
	@dwGroupID INT,
	@wKindID INT,								-- �����ʶ
	@cbPlayMode TINYINT,
	@strConfigSubStorageItem NVARCHAR(1024),
	@strCurSubStorageItem NVARCHAR(1024),
	@lConfigSysStorage BIGINT,
	@lConfigPlayerStorage BIGINT,
	@lConfigParameterK BIGINT,
	@lCurResetCount BIGINT

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	UPDATE PersonalRoomInfo SET ShareStorageRule=@strConfigSubStorageItem WHERE KindID=@wKindID AND PlayMode=@cbPlayMode
	
	-- ���ҷ���
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM PersonalRoomInfo(NOLOCK) WHERE KindID=@wKindID AND PlayMode=@cbPlayMode
	
	UPDATE GameRoomInfo SET ShareStorageRule=@strConfigSubStorageItem WHERE ServerID=@ServerID

	UPDATE WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty SET ConfigSubStorageItem=@strConfigSubStorageItem,CurSubStorageItem=@strCurSubStorageItem,ConfigSysStorage=@lConfigSysStorage,ConfigPlayerStorage=@lConfigPlayerStorage,ConfigParameterK=@lConfigParameterK,CurResetCount=@lCurResetCount WHERE KindID=@wKindID AND GroupID=@dwGroupID AND PlayMode=@cbPlayMode
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸ĵ�ǰ�������
CREATE  PROCEDURE dbo.GSP_GS_ModifyCurSubStorageItem
	@dwGroupID INT,
	@wKindID INT,								-- �����ʶ
	@cbPlayMode TINYINT,
	@strCurSubStorageItem NVARCHAR(1024),
	@lCurResetCount BIGINT	
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	UPDATE WHQJGroupDBLink.WHQJGroupDB.dbo.IMGroupShareStorageProperty SET CurSubStorageItem=@strCurSubStorageItem,CurResetCount=@lCurResetCount WHERE KindID=@wKindID AND GroupID=@dwGroupID AND PlayMode=@cbPlayMode
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ع�����
CREATE  PROCEDURE dbo.GSP_GS_LoadShareStorageItem
	@wKindID INT,								
	@cbPlayMode TINYINT,
	@dwGroupID INT
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
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