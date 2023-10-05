
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_DeleteGameRoom]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_DeleteGameRoom]
GO



---����˽�˷������
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_InsertPersonalRoomParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_InsertPersonalRoomParameter]
GO

--�޸�˽�˷������
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

	-- ɾ������
	DECLARE @nKindID INT
	SELECT @nKindID=KindID from GameRoomInfo where ServerID=@wServerID
	DECLARE @nRoomInfoCount int
	SET @nRoomInfoCount=(Select COUNT(*) from GameRoomInfo where KindID in (select KindID From GameRoomInfo where ServerID=@wServerID))
	DELETE GameRoomInfo WHERE ServerID=@wServerID

	-- ɾ��˽�˷������
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
	SET @strErrorDescribe=N'ɾ���ɹ���'
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ���뷿��
CREATE  PROCEDURE dbo.GSP_GS_InsertPersonalRoomParameter

	-- ��������
	@cbPlayMode TINYINT,						-- ��Ϸģʽ
	@wServerID INT,								-- �����ʶ
	@wKindID INT,								-- ����ID	
	@cbIsJoinGame TINYINT,						-- �Ƿ������Ϸ
	@cbMinPeople TINYINT,						-- ������Ϸ����С����
	@cbMaxPeople TINYINT,						-- ������Ϸ���������

	-- ��������
	@lPersonalRoomTax  BIGINT,					-- ˽�˷�����Ϸ�������
	@lMinCellScore BIGINT,						-- ��С�׷�
	@lMaxCellScore BIGINT,						-- ���׷�
	@lMinEnterScore BIGINT,						-- ��С�����
	@lMaxEnterScore BIGINT,						-- �������
	@wCanCreateCount SMALLINT,					-- ���Դ�������󷿼���Ŀ
	@wPlayTurnCount INT,						-- �����ܹ�������Ϸ��������

	-- ��������
	@wPlayTimeLimit	INT,						-- �����ܹ�������Ϸ�����ʱ��
	@wTimeAfterBeginCount	INT,				-- һ����Ϸ��ʼ��೤ʱ����ɢ����
	@wTimeOffLineCount	INT,					-- ��ҵ��߶೤ʱ����ɢ����
	@wTimeNotBeginGame	INT,					-- ������º�೤ʱ���δ��ʼ��Ϸ��ɢ����
	@wTimeNotBeginAfterCreateRoom	INT,		-- ���䴴����೤ʱ���δ��ʼ��Ϸ��ɢ����
	@cbCreateRight	TINYINT,					-- ����˽�˷���Ȩ��
	@wBeginFreeTime   SMALLINT,					-- ���ʱ��-��ʼ
	@wEndFreeTime	 SMALLINT,					-- ���ʱ��-����
	@wTimeRespondDismiss INT,					-- ��Ӧ��ɢ����ʱ��
	@strShareStorageRule NVARCHAR(1024),		-- ���������	
	@strCurSubStorageItem NVARCHAR(1024),		-- ��ǰ������
	@lConfigSysStorage BIGINT,
	@lConfigPlayerStorage BIGINT,
	@lConfigParameterK BIGINT,
	@lCurResetCount BIGINT,
	
	-- �����Ϣ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	DECLARE @dwServerID INT
	SELECT  @dwServerID = ServerID FROM PersonalRoomInfo WHERE KindID = @wKindID  AND PlayMode = @cbPlayMode
	IF @dwServerID IS NULL
	BEGIN
		-- ����˽�˷������																					 
		INSERT INTO PersonalRoomInfo (PlayMode, ServerID,  KindID,  IsJoinGame, MinPeople, MaxPeople, PersonalRoomTax, MinCellScore, MaxCellScore, MinEnterScore, MaxEnterScore, CanCreateCount, 
					PlayTurnCount, PlayTimeLimit, TimeAfterBeginCount, TimeOffLineCount,TimeNotBeginGame, TimeNotBeginAfterCreateRoom, CreateRight,BeginFreeTime,EndFreeTime,TimeRespondDismiss,ShareStorageRule)
		VALUES (@cbPlayMode, @wServerID,@wKindID, @cbIsJoinGame, @cbMinPeople, @cbMaxPeople, @lPersonalRoomTax, @lMinCellScore, @lMaxCellScore, @lMinEnterScore, @lMaxEnterScore, @wCanCreateCount, @wPlayTurnCount, 
		@wPlayTimeLimit, @wTimeAfterBeginCount, @wTimeOffLineCount, @wTimeNotBeginGame, @wTimeNotBeginAfterCreateRoom, @cbCreateRight,@wBeginFreeTime,@wEndFreeTime,@wTimeRespondDismiss,@strShareStorageRule)
	END
	ELSE
	BEGIN
		--����˽�˷������
		UPDATE PersonalRoomInfo SET  ServerID = @dwServerID, IsJoinGame = @cbIsJoinGame, MinPeople = @cbMinPeople, MaxPeople = @cbMaxPeople,PersonalRoomTax = @lPersonalRoomTax, MinCellScore = @lMinCellScore,
									 PlayTurnCount = @wPlayTurnCount, PlayTimeLimit = @wPlayTimeLimit, TimeAfterBeginCount = @wTimeAfterBeginCount, CanCreateCount = @wCanCreateCount,
									TimeOffLineCount = @wTimeOffLineCount,TimeNotBeginGame = @wTimeNotBeginGame, TimeNotBeginAfterCreateRoom = @wTimeNotBeginAfterCreateRoom, CreateRight = @cbCreateRight,
									BeginFreeTime=@wBeginFreeTime,EndFreeTime=@wEndFreeTime, PlayMode = @cbPlayMode, MaxCellScore = @lMaxCellScore, MinEnterScore = @lMinEnterScore, MaxEnterScore = @lMaxEnterScore, 
									TimeRespondDismiss=@wTimeRespondDismiss,ShareStorageRule=@strShareStorageRule
		WHERE KindID = @wKindID AND PlayMode = @cbPlayMode
	END
	
	IF @cbPlayMode = 1 
	BEGIN
		--����Ĭ�Ϲ��������
		DECLARE @tableGroupID TABLE
		(
			GroupID BIGINT
		);
		
		-- ��������
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
	
	SET @strErrorDescribe=N'�����ɹ���'
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
-- �޸�˽�˷������
CREATE  PROCEDURE dbo.GSP_GS_ModifyPersonalRoomParameter

	-- ��������
	@cbPlayMode TINYINT,						-- ��Ϸģʽ
	@wServerID INT,								-- �����ʶ
	@wKindID INT,								-- �����ʶ	
	@cbIsJoinGame TINYINT,						-- �Ƿ������Ϸ
	@cbMinPeople TINYINT,						-- ������Ϸ����С����
	@cbMaxPeople TINYINT,						-- ������Ϸ���������

	-- ��������
	@lPersonalRoomTax  BIGINT,					-- ��ʼ�η���
	@lMinCellScore BIGINT,						-- ��С�׷�
	@lMaxCellScore BIGINT,						-- ���׷�
	@lMinEnterScore BIGINT,						-- ��С�����
	@lMaxEnterScore BIGINT,						-- �������
	@wCanCreateCount int,						-- ���Դ�������󷿼���Ŀ
	@wPlayTurnCount INT,						-- �����ܹ�������Ϸ��������

	-- ��������
	@wPlayTimeLimit	INT,						-- �����ܹ�������Ϸ�����ʱ��
	@wTimeAfterBeginCount	INT,				-- һ����Ϸ��ʼ��೤ʱ����ɢ����
	@wTimeOffLineCount	INT,					-- ��ҵ��߶೤ʱ����ɢ����
	@wTimeNotBeginGame	INT,					-- ���䴴���೤ʱ���δ��ʼ��Ϸ��ɢ����
	@wTimeNotBeginAfterCreateRoom	INT,		-- ���䴴����೤ʱ���δ��ʼ��Ϸ��ɢ����
	@cbCreateRight	TINYINT,					-- ����˽�˷���Ȩ��

	@wBeginFreeTime   SMALLINT,					-- ���ʱ��-��ʼ
	@wEndFreeTime		SMALLINT,				-- ���ʱ��-����
	@wTimeRespondDismiss INT,					-- ��Ӧ��ɢ����ʱ��
	@strShareStorageRule NVARCHAR(1024),		-- ���������	
	@strCurSubStorageItem NVARCHAR(1024),		-- ��ǰ������
	@lConfigSysStorage BIGINT,
	@lConfigPlayerStorage BIGINT,
	@lConfigParameterK BIGINT,
	@lCurResetCount BIGINT,
	
	-- �����Ϣ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	DECLARE @dwServerID INT
	SELECT  @dwServerID = ServerID FROM PersonalRoomInfo WHERE KindID = @wKindID  AND PlayMode = @cbPlayMode
	IF @dwServerID IS NULL
	BEGIN
			SELECT  @dwServerID = ServerID FROM PersonalRoomInfo WHERE ServerID=@wServerID
			IF @dwServerID IS NULL
			BEGIN
					-- ����˽�˷������																					 
					INSERT INTO PersonalRoomInfo (PlayMode, ServerID,  KindID,  IsJoinGame, MinPeople, MaxPeople, PersonalRoomTax, MinCellScore, MaxCellScore, MinEnterScore, MaxEnterScore, CanCreateCount, 
								PlayTurnCount, PlayTimeLimit, TimeAfterBeginCount, TimeOffLineCount,TimeNotBeginGame, TimeNotBeginAfterCreateRoom, CreateRight,BeginFreeTime,EndFreeTime,TimeRespondDismiss,ShareStorageRule)
					VALUES (@cbPlayMode, @wServerID,@wKindID, @cbIsJoinGame, @cbMinPeople, @cbMaxPeople, @lPersonalRoomTax, @lMinCellScore, @lMaxCellScore, @lMinEnterScore, @lMaxEnterScore, @wCanCreateCount, @wPlayTurnCount, 
							@wPlayTimeLimit, @wTimeAfterBeginCount, @wTimeOffLineCount, @wTimeNotBeginGame, @wTimeNotBeginAfterCreateRoom, @cbCreateRight,@wBeginFreeTime,@wEndFreeTime,@wTimeRespondDismiss,@strShareStorageRule)
			END
			ELSE
			BEGIN
					--����˽�˷������
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
		--����˽�˷������
		UPDATE PersonalRoomInfo SET  ServerID = @dwServerID, IsJoinGame = @cbIsJoinGame, MinPeople = @cbMinPeople, MaxPeople = @cbMaxPeople,PersonalRoomTax = @lPersonalRoomTax, MinCellScore = @lMinCellScore,
									 PlayTurnCount = @wPlayTurnCount, PlayTimeLimit = @wPlayTimeLimit, TimeAfterBeginCount = @wTimeAfterBeginCount, CanCreateCount = @wCanCreateCount,
									TimeOffLineCount = @wTimeOffLineCount,TimeNotBeginGame = @wTimeNotBeginGame, TimeNotBeginAfterCreateRoom = @wTimeNotBeginAfterCreateRoom, CreateRight = @cbCreateRight,
									BeginFreeTime=@wBeginFreeTime,EndFreeTime=@wEndFreeTime, PlayMode = @cbPlayMode, MaxCellScore = @lMaxCellScore, MinEnterScore = @lMinEnterScore, MaxEnterScore = @lMaxEnterScore,
									TimeRespondDismiss=@wTimeRespondDismiss,ShareStorageRule=@strShareStorageRule
		WHERE KindID = @wKindID  AND PlayMode = @cbPlayMode
	END
	
	IF @cbPlayMode = 1 
	BEGIN
		--����Ĭ�Ϲ��������
		DECLARE @tableGroupID TABLE
		(
			GroupID BIGINT
		);
		
		-- ��������
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
	
	SET @strErrorDescribe=N'�޸ĳɹ���'
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- ��ɢ˽�˷�
CREATE PROC GSP_GR_DissumePersonalRoom
	@dwPersonalRoomID INT,						-- Լս����ID
	@lRoomHostID INT,							-- ���� ID
	@lTaxCount bigint,
	@cbPersonalRoomInfo varbinary(MAX),
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	DECLARE @tDissumDate datetime
	DECLARE @TaxRatio INT
	SET @TaxRatio = 0
	DECLARE @RoomTaxRatio INT
	DECLARE @ServerID INT
	DECLARE @TaxAgency	INT 
	SET @RoomTaxRatio = 0
	SET @ServerID = 0

	-- ���ʱ��,�����ʶ
	select @tDissumDate = DissumeDate, @ServerID = ServerID, @TaxAgency = TaxAgency from WHQJPlatformDB..StreamCreateTableFeeInfo where RoomID = @dwPersonalRoomID AND UserID = @lRoomHostID

	-- ��ȡ��Ӧ����ķ������
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


	-- д�봴����¼
	UPDATE  WHQJPlatformDB..StreamCreateTableFeeInfo   SET  DissumeDate = GETDATE(),TaxCount = @lTaxCount, RoomScoreInfo = @cbPersonalRoomInfo , RoomStatus = 2  WHERE RoomID = @dwPersonalRoomID AND UserID = @lRoomHostID

	if @tDissumDate is null
	SET  @tDissumDate = GETDATE()

	--����
	DECLARE @temp TABLE
	(
	UserID INT
	);

	-- ��Դ���е����ݲ��뵽�������
	INSERT INTO @temp(UserID)
	SELECT UserID FROM PersonalRoomScoreInfo WHERE RoomID = @dwPersonalRoomID
	ORDER BY UserID;

	-- ��������
	DECLARE
	@UserID AS INT,
	@firstname AS NVARCHAR(10),
	@lastname AS NVARCHAR(20);
     
	WHILE EXISTS(SELECT UserID FROM @temp)
	BEGIN
	-- Ҳ����ʹ��top 1
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


