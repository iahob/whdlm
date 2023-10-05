USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CreateTableFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CreateTableFee]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_AICreateTableFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_AICreateTableFee]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CreateTableQuit]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CreateTableQuit]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_InsertCreateRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_InsertCreateRecord]


---����˽�˷�����Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_QueryPersonalRoomInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_QueryPersonalRoomInfo]
GO

---֧����ʯ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_PayRoomCard]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_PayRoomCard]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �����ж�
CREATE PROC GSP_GR_CreateTableFee
	@dwUserID INT,								-- �û� I D
	@dwServerID INT,							-- ���� I D
	@dwDrawCountLimit INT,						-- ʱ������
	@dwDrawTimeLimit INT,						-- ��������
	@cbPayMode		TINYINT,					-- ������ʽ	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@cbPlayMode		TINYINT,					-- ��Ϸģʽ 0 ���ֳ�  1 ��ҳ�
	@dwGroupID		INT,						-- ������ID
	@dwConfigID		INT,						-- �淨ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	DECLARE @ReturnValue INT
	DECLARE @dUserBeans DECIMAL(18,2)
	DECLARE @dCurBeans DECIMAL(18,2)
	DECLARE @Fee INT
	DECLARE @AAFee INT
	DECLARE @lDiamond bigint
	DECLARE @cbIsJoinGame TINYINT
	DECLARE @MemberOrder TINYINT
	DECLARE @CreateRight TINYINT
	DECLARE @iCanCreateCount INT
	DECLARE @szConfigName nvarchar(31)
	DECLARE @cbConfigTaxMode TINYINT
	DECLARE @dwConfigTaxCount INT
	DECLARE @wKindID INT
	DECLARE @cbLessScoreDismiss TINYINT
	SELECT  @wKindID = KindID FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID = @dwServerID 
	-- ��ѯ���� �� �����Ƿ������Ϸ
	SELECT  @CreateRight = CreateRight FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 

	--�û���ʯ	
	SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID

		-- ��ѯ����
	DECLARE @LockServerID INT
	SELECT @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

		-- �����ж�
	IF  @LockServerID IS NOT NULL and @LockServerID<>@dwServerID
	BEGIN
		-- ��ѯ��Ϣ
		DECLARE @ServerName NVARCHAR(31)
		SELECT @ServerName=ServerName FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

		-- ������Ϣ
		IF @ServerName IS NULL SET @ServerName=N'δ֪����'
		SET @strErrorDescribe=N'������ [ '+@ServerName+N' ] ��Ϸ�����У���������Ϸ��ͬʱ�������䣡'
		RETURN 2
	END

	--���뷽ʽ
	SELECT  @cbIsJoinGame = IsJoinGame,@iCanCreateCount=CanCreateCount FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 
		
	-- ��ѯ����
	SELECT @Fee=TableFee,@AAFee = AAPayFee FROM  WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID AND PlayMode = @cbPlayMode
	
	IF @cbPlayMode = 0 OR @dwGroupID <>0
	BEGIN
		IF @Fee IS NULL OR @Fee=0
		BEGIN
			SET @strErrorDescribe=N'���ݿ��ѯ����ʧ�ܣ������³��ԣ�'
			RETURN 3
		END
	END

	--������֧����ʽ
	DECLARE @cbPayType TINYINT
	SET @cbPayType=0
	--��ѯ������
	IF @dwGroupID<>0
	BEGIN
	
		DECLARE @TmpGroupID INT 
		SELECT @TmpGroupID=GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID=@dwGroupID
		IF @TmpGroupID IS NULL
		BEGIN
			SET @strErrorDescribe=N'��ѯ������ʧ�ܣ��޷����������˷��䣬�����³��ԣ�'
			RETURN 4
		END
		
		--��ѯ��Ա
		SELECT @TmpGroupID=GroupID FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID = @dwGroupID
		IF @TmpGroupID IS NULL
		BEGIN
			SET @strErrorDescribe=N'�㲻�Ǹô����˳�Ա���޷����������˷��䣡'
			RETURN 5
		END
		
		--��֤֧��
		SELECT @cbPayType=PayType FROM   WHQJGroupDB.dbo.IMGroupBattleRule WHERE GroupID=@dwGroupID
		IF @cbPayType IS NULL SET @cbPayType=1
		
		
		IF @cbPayMode=1 AND @cbPayType=1
		BEGIN
			SET @strErrorDescribe=N'�����˲�֧�����֧����������ѡ��'
			RETURN 6
		END
				
		SELECT @szConfigName=ConfigName,@cbConfigTaxMode=TaxMode,@dwConfigTaxCount=TaxCount,@cbLessScoreDismiss=LessScoreDismiss FROM   WHQJGroupDB.dbo.IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID = @dwConfigID
		IF @cbConfigTaxMode IS NULL
		BEGIN
			SET @strErrorDescribe=N'�ô����˲����ڴ��淨��������ѡ��'
			RETURN 7
		END
	END
	
	IF @cbConfigTaxMode IS NULL
	BEGIN
		SET @cbConfigTaxMode=0
		SET @dwConfigTaxCount=0
		SET @szConfigName=N''
		SET @cbLessScoreDismiss=0
	END

	--��ѯ���봴�����з�����Ҫ����ʯ
	DECLARE @MaxUserPay INT
	DECLARE @MaxGroupPay INT	

	IF @dwGroupID=0
	BEGIN
		SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID

		SELECT @MaxUserPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @dwUserID AND GroupID=0 AND RoomStatus <> 2
		SELECT @MaxGroupPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE RoomStatus <> 2 
		AND GroupID in(Select GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty where CreaterID=@dwUserID) 
	END
	ELSE
	BEGIN
		DECLARE @GroupCreaterID INT
		Select @GroupCreaterID=CreaterID FROM   WHQJGroupDB.dbo.IMGroupProperty where GroupID=@dwGroupID
		IF @GroupCreaterID IS NULL
		BEGIN
			SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����飡'
			RETURN 1
		END

		SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID = @GroupCreaterID

		SELECT @MaxUserPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @GroupCreaterID AND GroupID=0 AND RoomStatus <> 2

		SELECT @MaxGroupPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE RoomStatus <> 2 
		AND GroupID in(Select GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty where CreaterID=@GroupCreaterID) 
	END
	
	IF @lDiamond IS NULL SET @lDiamond=0
	IF @MaxUserPay IS NULL SET @MaxUserPay=0
	IF @MaxGroupPay IS NULL	SET @MaxGroupPay=0

	-- ��ͨ���Լսģʽ����Ҫ������ʯ
	IF @cbPlayMode = 1 AND @dwGroupID=0
	BEGIN
		IF @cbIsJoinGame IS NULL  SET @cbIsJoinGame = 0
		IF @AAFee IS NULL  SET @AAFee = 0
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@AAFee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
		RETURN 0
	END
	
	--�Ƿ�AA��
	IF @cbPayMode = 1
	BEGIN
		IF @lDiamond  < @MaxUserPay+@MaxGroupPay + @AAFee
		BEGIN
			SET @strErrorDescribe=N'�û���ʯ���㣬�빺��'
			RETURN 1
		END
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@AAFee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
	END
	ELSE 
	BEGIN
		IF @lDiamond  < @MaxUserPay+@MaxGroupPay + @Fee
		BEGIN
			IF @cbPayType=1
				SET @strErrorDescribe=N'������ʯ���㣬����ϵ������ֵ��'
			ELSE
			 SET @strErrorDescribe=N'�û���ʯ���㣬�빺��'
			
			RETURN 1
		END
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@Fee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
	END
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- �����ж�
CREATE PROC GSP_GR_AICreateTableFee
	@dwUserID INT,								-- �û� I D
	@dwServerID INT,							-- ���� I D
	@dwDrawCountLimit INT,						-- ʱ������
	@dwDrawTimeLimit INT,						-- ��������
	@cbPayMode		TINYINT,					-- ������ʽ	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@cbPlayMode		TINYINT,					-- ��Ϸģʽ 0 ���ֳ�  1 ��ҳ�
	@dwGroupID		INT,						-- ������ID
	@dwConfigID		INT,						-- �淨ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	DECLARE @ReturnValue INT
	DECLARE @dUserBeans DECIMAL(18,2)
	DECLARE @dCurBeans DECIMAL(18,2)
	DECLARE @Fee INT
	DECLARE @AAFee INT
	DECLARE @lDiamond bigint
	DECLARE @cbIsJoinGame TINYINT
	DECLARE @MemberOrder TINYINT
	DECLARE @CreateRight TINYINT
	DECLARE @iCanCreateCount INT
	DECLARE @szConfigName nvarchar(31)
	DECLARE @cbConfigTaxMode TINYINT
	DECLARE @dwConfigTaxCount INT
	DECLARE @wKindID INT
	DECLARE @cbLessScoreDismiss TINYINT
	SELECT  @wKindID = KindID FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID = @dwServerID 
	-- ��ѯ���� �� �����Ƿ������Ϸ
	SELECT  @CreateRight = CreateRight FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 

	--�û���ʯ	
	SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID



	--���뷽ʽ
	SELECT  @cbIsJoinGame = IsJoinGame,@iCanCreateCount=CanCreateCount FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 
		
	-- ��ѯ����
	SELECT @Fee=TableFee,@AAFee = AAPayFee FROM  WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID AND PlayMode = @cbPlayMode
	
	IF @cbPlayMode = 0 OR @dwGroupID <>0
	BEGIN
		IF @Fee IS NULL OR @Fee=0
		BEGIN
			SET @strErrorDescribe=N'���ݿ��ѯ����ʧ�ܣ������³��ԣ�'
			RETURN 3
		END
	END

	--������֧����ʽ
	DECLARE @cbPayType TINYINT
	SET @cbPayType=0
	--��ѯ������
	IF @dwGroupID<>0
	BEGIN
	
		DECLARE @TmpGroupID INT 
		SELECT @TmpGroupID=GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID=@dwGroupID
		IF @TmpGroupID IS NULL
		BEGIN
			SET @strErrorDescribe=N'��ѯ������ʧ�ܣ��޷����������˷��䣬�����³��ԣ�'
			RETURN 4
		END
		
		--��ѯ��Ա
		SELECT @TmpGroupID=GroupID FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID = @dwGroupID
		IF @TmpGroupID IS NULL
		BEGIN
			SET @strErrorDescribe=N'�㲻�Ǹô����˳�Ա���޷����������˷��䣡'
			RETURN 5
		END
		
		--��֤֧��
		SELECT @cbPayType=PayType FROM   WHQJGroupDB.dbo.IMGroupBattleRule WHERE GroupID=@dwGroupID
		IF @cbPayType IS NULL SET @cbPayType=1
		
		
				
		SELECT @szConfigName=ConfigName,@cbConfigTaxMode=TaxMode,@dwConfigTaxCount=TaxCount,@cbLessScoreDismiss=LessScoreDismiss FROM   WHQJGroupDB.dbo.IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID = @dwConfigID
		IF @cbConfigTaxMode IS NULL
		BEGIN
			SET @strErrorDescribe=N'�ô����˲����ڴ��淨��������ѡ��'
			RETURN 7
		END
	END
	
	IF @cbConfigTaxMode IS NULL
	BEGIN
		SET @cbConfigTaxMode=0
		SET @dwConfigTaxCount=0
		SET @szConfigName=N''
		SET @cbLessScoreDismiss=0
	END
	--��ѯ���봴�����з�����Ҫ����ʯ
	DECLARE @MaxPay INT	
	IF @dwGroupID=0
		SELECT @MaxPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @dwUserID AND RoomStatus <> 2
	ELSE
	BEGIN
		SELECT @MaxPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE GroupID = @dwGroupID AND RoomStatus <> 2
	END
	IF @MaxPay IS NULL
	SET @MaxPay=0
	
	-- ��ȡ��ʯ	
	SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID = (Select CreaterID FROM   WHQJGroupDB.dbo.IMGroupProperty where GroupID=@dwGroupID)

	
	IF @lDiamond IS NULL SET @lDiamond=0

	-- ��ͨ���Լսģʽ����Ҫ������ʯ
	IF @cbPlayMode = 1 AND @dwGroupID=0
	BEGIN
		IF @cbIsJoinGame IS NULL  SET @cbIsJoinGame = 0
		IF @AAFee IS NULL  SET @AAFee = 0
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@AAFee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
		RETURN 0
	END
	
	
	--�Ƿ�AA��
	IF @cbPayMode = 1
	BEGIN
		IF @lDiamond  < @MaxPay + @AAFee
		BEGIN
			SET @strErrorDescribe=N'�û���ʯ���㣬�빺��'
			RETURN 1
		END
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@AAFee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
	END
	ELSE 
	BEGIN
		IF @lDiamond  < @MaxPay + @Fee
		BEGIN
			IF @cbPayType=1
				SET @strErrorDescribe=N'������ʯ���㣬����ϵ������ֵ��'
			ELSE
			 SET @strErrorDescribe=N'�û���ʯ���㣬�빺��'
			
			RETURN 1
		END
		SELECT @lDiamond AS Diamond, @cbIsJoinGame AS IsJoinGame,@Fee AS RoomCardFee,@szConfigName AS ConfigName,@cbConfigTaxMode AS TaxMode,@dwConfigTaxCount AS TaxCount,@cbLessScoreDismiss AS LessScoreDismiss
	END
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------

-- �˻�����
CREATE PROC GSP_GR_CreateTableQuit
	@dwUserID INT,								-- �û� I D
	@dwPersonalRoomID INT,						-- Լս����ID
	@dwServerID INT,							-- �����ʶ
	@dwDrawCountLimit INT,						-- ʱ������
	@dwDrawTimeLimit INT,						-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	DECLARE @ReturnValue INT
	SET @ReturnValue=0

	DECLARE @lDiamond bigint
	DECLARE @cbJoin TINYINT

	DECLARE @wKindID INT
	SELECT  @wKindID = KindID FROM  WHQJPlatformDB.dbo.GameRoomInfo WHERE ServerID = @dwServerID 
	-- ��ѯ���� �� �����Ƿ������Ϸ
	SELECT @cbJoin = IsJoinGame FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 

	-- ���¼�¼
	UPDATE  WHQJPlatformDB..StreamCreateTableFeeInfo   SET  DissumeDate = GETDATE(), RoomStatus = 2  WHERE RoomID = @dwPersonalRoomID AND UserID = @dwUserID

	--���������Ϸ��������
	IF @cbJoin = 1
	BEGIN
		DELETE FROM GameScoreLocker WHERE UserID = @dwUserID
	END

	--�û���ʯ	
	SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID
	IF @lDiamond IS NULL SET @lDiamond=0

	SELECT @lDiamond AS Diamond

END

RETURN @ReturnValue
GO


-----------------------------------------------------------------------
-- ���������¼
CREATE PROC GSP_GR_InsertCreateRecord
	@dwUserID INT,								-- �û� I D
	@dwServerID INT,							-- ���� ��ʶ
	@dwPersonalRoomID INT,						-- Լս�����ʶ
	@lCellScore INT,							-- ���� �׷�
	@dwDrawCountLimit INT,						-- ��������
	@dwDrawTimeLimit INT,						-- ʱ������
	@szPassWord NVARCHAR(15),					-- ���ӵ�ַ
	@wJoinGamePeopleCount TINYINT,				-- ��������
	@dwRoomTax BIGINT,							-- ˽�˷���������
	@strClientAddr NVARCHAR(15),
	@cbPayMode TINYINT,							-- ֧��ģʽ
	@lNeedRoomCard TINYINT,						-- ������Ҫ����ʯ
	@cbGameMode TINYINT,						-- ��Ϸģʽ
	@cbPlayMode		TINYINT,					-- ��Ϸģʽ 0 ���ֳ�  1 ��ҳ�
	@lGroupID		BIGINT,						-- ������ID
	@lMinEnterScore BIGINT,						-- ����볡��
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	DECLARE @Fee INT
	DECLARE @Nicname NVARCHAR(31)
	DECLARE @lPersonalRoomTax BIGINT

	-- ��ѯ���� �� �����Ƿ������Ϸ
	DECLARE @wKindID INT
	SELECT  @wKindID = KindID FROM  WHQJPlatformDB.dbo.GameRoomInfo  WHERE ServerID = @dwServerID 
	-- ��ѯ����
	SELECT @Fee=TableFee FROM  WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID AND  PlayMode= @cbPlayMode
	IF @cbPlayMode = 0 OR @lGroupID<>0
	BEGIN
		IF @Fee IS NULL OR @Fee=0
		BEGIN
			SET @strErrorDescribe=N'���ݿ��ѯ����ʧ�ܣ������³��ԣ�'
			RETURN 3
		END
	END
	ELSE
	BEGIN
		SET @Fee = 0
	END

	-- ��ȡ����������ҵ��ǳ�
	SELECT @Nicname =NickName FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID = @dwUserID
	IF @Nicname IS NULL
	SET @Nicname =''
		
	SELECT  @lPersonalRoomTax = PersonalRoomTax FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 
	
	--�����������ʯ����ѯ����������
	DECLARE @lTaxAgency BIGINT
	SELECT  @lTaxAgency = AgentScale FROM  WHQJAccountsDB.dbo.AccountsAgent WHERE UserID = @dwUserID 
	IF @lTaxAgency IS NOT NULL
	BEGIN
		SET @lPersonalRoomTax = @lTaxAgency
	END

	-- д�����ļ�¼
	INSERT INTO WHQJPlatformDB..StreamCreateTableFeeInfo(UserID,NickName, ServerID, RoomID, CellScore, CountLimit,TimeLimit,CreateTableFee,CreateDate, TaxAgency, JoinGamePeopleCount,PayMode, RoomStatus, NeedRoomCard,GameMode,PlayMode,GroupID)
												VALUES(@dwUserID,@Nicname, @dwServerID, @dwPersonalRoomID, @lCellScore,@dwDrawCountLimit, @dwDrawTimeLimit, 0,GETDATE(), @dwRoomTax, @wJoinGamePeopleCount, @cbPayMode, 0, @lNeedRoomCard,@cbGameMode,@cbPlayMode,@lGroupID)	
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------------
-- ����Լս������Ϣ
CREATE  PROCEDURE dbo.GSP_GS_QueryPersonalRoomInfo
	@dwPersonalRoomID INT,							-- Լս�����ʶ
	@dwRoomHostID INT,									-- ����ID
	@strErrorDescribe NVARCHAR(127) OUTPUT				-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	DECLARE @dwUserID			INT
	DECLARE @dwServerID			INT
	DECLARE @dwKindID			INT
	DECLARE @Nicname 			NVARCHAR(31)
	DECLARE @dwPlayTurnCount 	INT
	DECLARE @dwPlayTimeLimit 	INT
	DECLARE @cbIsDisssumRoom 	TINYINT
	DECLARE @sysCreateTime 		DATETIME
	DECLARE @sysDissumeTime 	DATETIME
	DECLARE @lTaxCount 			BIGINT
	DECLARE @lCreateFee 		TINYINT
	DECLARE @bnryRoomScoreInfo 	varbinary(MAX)
	DECLARE @cbPayMode 			TINYINT
	DECLARE @lNeedRoomCard 		TINYINT
	DECLARE @lJoinPeopleCount 	TINYINT
	DECLARE @cbRoomStatus		TINYINT
	DECLARE @cbGameMode 			TINYINT
	-- ���ط���
	SELECT @dwUserID = UserID, @dwPlayTurnCount=CountLimit, @dwPlayTimeLimit = TimeLimit, @sysCreateTime = CreateDate, @sysDissumeTime = DissumeDate, @lTaxCount = TaxRevenue, @lCreateFee = CreateTableFee,
	  @bnryRoomScoreInfo = RoomScoreInfo, @cbPayMode = PayMode, @lNeedRoomCard = NeedRoomCard, @lJoinPeopleCount = JoinGamePeopleCount, @cbRoomStatus = RoomStatus,@dwServerID=ServerID
	  ,@cbGameMode = GameMode
	FROM WHQJPlatformDB..StreamCreateTableFeeInfo WHERE RoomID = @dwPersonalRoomID AND UserID = @dwRoomHostID
	IF @sysDissumeTime IS NULL
	BEGIN
		SET @cbIsDisssumRoom = 0
		SET @sysDissumeTime = @sysCreateTime
	END
	ELSE
	BEGIN
		SET @cbIsDisssumRoom = 1
	END
	--��ȡ��ϷID
	IF @dwServerID IS NOT NULL	SELECT @dwKindID=KindID FROM WHQJPlatformDB..GameRoomInfo WHERE ServerID=@dwServerID
	IF @dwKindID IS NULL SET @dwKindID=0
	
	declare @strRoomScoreInfo varchar(8000),@i int
	select @strRoomScoreInfo='',@i=datalength(@bnryRoomScoreInfo)
	while @i>0
		select @strRoomScoreInfo=substring('0123456789ABCDEF',substring(@bnryRoomScoreInfo,@i,1)/16+1,1)
				+substring('0123456789ABCDEF',substring(@bnryRoomScoreInfo,@i,1)%16+1,1)
				+@strRoomScoreInfo
			,@i=@i-1

	-- ��ȡ����ǳ�
	SELECT @Nicname =NickName FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID = @dwUserID
	IF @Nicname IS NULL
	SET @Nicname =''


	if @dwPlayTurnCount is null
	set @dwPlayTurnCount=0

	if @dwPlayTimeLimit is null
	set @dwPlayTimeLimit=0

	if @cbIsDisssumRoom is null
	set @cbIsDisssumRoom=0

	if @sysDissumeTime is null
	set @sysDissumeTime=GETDATE()

	if @sysCreateTime is null
	set @sysCreateTime=GETDATE()

	if @lTaxCount is null
	set @lTaxCount=0	

	if @lCreateFee is null
	set @lCreateFee=0

	SELECT @Nicname AS UserNicname, @dwPlayTurnCount AS dwPlayTurnCount, @dwPlayTimeLimit AS dwPlayTimeLimit, @cbIsDisssumRoom AS cbIsDisssumRoom, @sysCreateTime AS sysCreateTime, 
	@sysDissumeTime AS sysDissumeTime, @lTaxCount AS lTaxCount, @lCreateFee AS CreateTableFee,@bnryRoomScoreInfo AS RoomScoreInfo, @strRoomScoreInfo AS strRoomScoreInfo,
	@cbPayMode AS PayMode, @lNeedRoomCard AS NeedRoomCard, @lJoinPeopleCount AS JoinGamePeopleCount, @cbRoomStatus AS RoomStatus,@dwKindID AS wKindID,@cbGameMode AS GameMode

END

RETURN 0

GO

-------------------------------------
-----------------------------------------------------------------------
-- ���������¼
CREATE PROC GSP_GS_PayRoomCard
	@cbPayMode	INT,							-- ֧����ʽ
	@dwUserID INT,								-- �û� I D
	@dwKindID INT,								-- ���� ��ʶ
	@dwPersonalRoomID INT,						-- Լս����ID
	@dwDrawCountLimit INT,						-- ��������
	@dwDrawTimeLimit INT,						-- ʱ������
	@strClientAddr NVARCHAR(15),				-- �ͻ��˵�ַ
	@dwGroupID	INT,							-- ������ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN		
	DECLARE @Fee INT
	DECLARE @Nicname NVARCHAR(31)
	DECLARE @lPersonalRoomTax BIGINT
	DECLARE @ReturnValue INT

	-- ��ѯ���� �� �����Ƿ������Ϸ	
	DECLARE @wKindID INT

	-- ��ѯ����
	IF @cbPayMode=1
	BEGIN
		SELECT @Fee=AAPayFee FROM WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @dwKindID 
	END
	ELSE
	BEGIN
		SELECT @Fee=TableFee FROM WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @dwKindID 
	END
	
	IF @Fee IS NULL OR @Fee=0
	BEGIN
		SET @strErrorDescribe=N'���ݿ��ѯ����ʧ�ܣ������³��ԣ�'
		RETURN 3
	END

	--	��ѯ���ķ�ʽ
	SELECT @lPersonalRoomTax = PersonalRoomTax FROM  WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 	

	--��ѯ���������ѷ�ʽ
	DECLARE @cbPayType TINYINT
	SET @cbPayType=0
	IF @dwGroupID<>0
	BEGIN
		SELECT @cbPayType=PayType FROM   WHQJGroupDB.dbo.IMGroupBattleRule WHERE GroupID=@dwGroupID
		IF @cbPayType IS NULL SET @cbPayType=1
	END
	
	--��ѯ��ʯ
	DECLARE @lDiamond BIGINT
	IF @cbPayType<>1
	BEGIN 
		SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID
		IF @lDiamond IS NULL
		BEGIN
			SET @strErrorDescribe=N'�û���ʯ���㣬���ֵ��'
			RETURN 1
		END
	END
	ELSE
	BEGIN
		SELECT @lDiamond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=(SELECT CreaterID FROM WHQJGroupDB..IMGroupProperty WHERE GroupID=@dwGroupID)
		IF @lDiamond IS NULL
		BEGIN
			SET @strErrorDescribe=N'������ʯ���㣬����ϵ������ֵ��'
			RETURN 1
		END
		
	END


	-- д�����
	IF @lDiamond < @Fee
	BEGIN
		SET @strErrorDescribe=N'������ʯ���㣬���ȳ�ֵ��'
		RETURN 4
	END	
	ELSE
	BEGIN
		IF @cbPayType<>1
		BEGIN
			UPDATE WHQJTreasureDB..UserCurrency SET Diamond=@lDiamond-@Fee WHERE UserID=@dwUserID					
		
			--������־
			DECLARE @DateTime DATETIME
			DECLARE @SerialNumber NVARCHAR(20)
			SET @DateTime = GETDATE()
			SELECT @SerialNumber=dbo.WF_GetSerialNumber()
		
			--һ�˸�����
			IF @cbPayMode = 0
			BEGIN
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,10,@lDiamond,-@Fee,@strClientAddr,@DateTime)
			END
			ELSE
			BEGIN
				INSERT INTO WHQJRecordDB.dbo.RecordDiamondSerial(SerialNumber,MasterID,UserID,TypeID,CurDiamond,ChangeDiamond,ClientIP,CollectDate) 
				VALUES(@SerialNumber,0,@dwUserID,11,@lDiamond,-@Fee,@strClientAddr,@DateTime)
			END
		END
		ELSE
		BEGIN
			-- ���²Ƹ���Ϣ
			UPDATE WHQJTreasureDB..UserCurrency SET Diamond=@lDiamond-@Fee WHERE UserID=(SELECT CreaterID FROM WHQJGroupDB..IMGroupProperty WHERE GroupID=@dwGroupID)
			
			--�����¼
			INSERT INTO   WHQJGroupDB.dbo.RecordGroupIngotChange(GroupID,BeforeIngot,TradeType,TradeIngot,TradeRoomGUID,CollectDate) 
			VALUES(@dwGroupID,@lDiamond,3,-@Fee,@dwPersonalRoomID,GETDATE())
			
			
			
			--�����û�Լս��������������ͳ���ڣ�
			IF  EXISTS ( SELECT 1 FROM   WHQJGroupDB.dbo.StreamGroupBalance WHERE GroupID=@dwGroupID AND UserID=@dwUserID)
			BEGIN
				UPDATE   WHQJGroupDB.dbo.StreamGroupBalance SET BattleCount=BattleCount+1 WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			END
			ELSE
			BEGIN			
				INSERT INTO   WHQJGroupDB.dbo.StreamGroupBalance(GroupID,UserID,BattleCount,BalanceCount,CollectDate) 
				VALUES(@dwGroupID,@dwUserID,1,0,GETDATE())
			END
			
			--���³�ԱԼս����
			--UPDATE   WHQJGroupDB.dbo.IMGroupMember SET BattleCount=BattleCount+1 WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		END

	END			
	
	--һ�˸�����
	IF @cbPayMode = 0
	BEGIN
		UPDATE WHQJPlatformDB..StreamCreateTableFeeInfo SET CreateTableFee = @Fee,NeedRoomCard=0 WHERE RoomID = @dwPersonalRoomID AND UserID = @dwUserID
	END

	
	SELECT @lDiamond-@Fee AS Diamond,@Fee AS Fee 
END

RETURN 0
GO
