
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryGroupConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryGroupConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_AppendConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_AppendConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ModifyConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ModifyConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_DeleteConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_DeleteConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryCreate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryCreate]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetFavoriteConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SetFavoriteConfig]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ѯȺ���淨
CREATE  PROCEDURE dbo.GSP_IM_QueryGroupConfig
	@dwGroupID BIGINT		--Ⱥ���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ���ݱ�
	SELECT * FROM IMGroupConfig WHERE GroupID=@dwGroupID

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- �����淨
CREATE  PROCEDURE dbo.GSP_IM_AppendConfig
	-- ��������
	@dwGroupID   INT,							-- ���ֲ�ID
	@dwCreaterID INT,							-- �������ID
	@strConfigName NVARCHAR(12),				-- �淨����
	@wKindID     INT,							-- ���ͱ�ʶ
	@cbPlayMode	 TINYINT,						-- ����ģʽ
	@lCellScore BIGINT,							-- ��Ԫ����
	@dwDrawCountLimit BIGINT,					-- ��������
	@dwDrawTimeLimit BIGINT,					-- ʱ������
	@wJoinCount INT,							-- ��������
	@dwRoomTax	INT,							-- ˽�˷��������
	@cbPassword	TINYINT,						-- ����ģʽ
	@cbPayMode	TINYINT,						-- ֧��ģʽ
	@cbGameMode	TINYINT,						-- ��Ϸģʽ
	@lEnterScore BIGINT,						-- �����
	@lLeaveScore BIGINT,						-- �뿪��	
	@cbJoinMode	TINYINT,						-- ����ģʽ
	@cbTaxMode	TINYINT,						-- �������ģʽ
	@lTaxCount  BIGINT,							-- ���������ֵ
	@cbLessScoreDismiss  TINYINT,				-- ����������ɢ
	@lTimeSecond BIGINT,						-- ʱ���
	@cbGroupPersonalMaxChair TINYINT,			-- �����˰�����Ϸ��ʶ
	@strGameRule NVARCHAR(200),					-- ���ƹ���

	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ����뵽���¿�ͨ����ܼ���ʹ�ã�'
		RETURN 3
	END

	DECLARE @GroupCreaterID INT
	Select @GroupCreaterID=CreaterID FROM   WHQJGroupDB.dbo.IMGroupProperty where GroupID=@dwGroupID
	IF @GroupCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����飡'
		RETURN 1
	END

	-- ���Ȩ��
	DECLARE @MemberRight INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwCreaterID

	IF @MemberRight IS NULL
		SET @MemberRight=0

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF (@MemberRight&0x01) <> 0
		SET @HaveRight=1
	ELSE
	BEGIN

		IF (@MemberRight&0x02) <> 0--����Ա
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwCreaterID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
			IF (@ManagerRight&0x40)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ�޽��д˲�����'
		RETURN 4		
	END

	IF @cbPlayMode=1 AND NOT EXISTS(SELECT 1 FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND CanUseMedal=1 )
	BEGIN
		SET @strErrorDescribe=N'��Ǹ���ô�������Ȩ����ѫ���淨��'
		RETURN 5	
	END

	-- ��������
	DECLARE @CreateConfigDeductIngot BIGINT
	SELECT @CreateConfigDeductIngot=OptionValue FROM IMGroupOption WHERE OptionName=N'CreateConfigDeductIngot'					
	IF @CreateConfigDeductIngot IS NULL SET @CreateConfigDeductIngot=0
	
	-- ��ѯ������ʯ
	DECLARE @UserIngot BIGINT
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency(NOLOCK) WHERE UserID=@GroupCreaterID
	IF @UserIngot IS NULL SET @UserIngot=0

	DECLARE @MaxUserPay INT
	DECLARE @MaxGroupPay INT	

	SELECT @MaxUserPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @GroupCreaterID AND GroupID=0 AND RoomStatus <> 2
	SELECT @MaxGroupPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE RoomStatus <> 2 
	AND GroupID in(Select GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty where CreaterID=@GroupCreaterID) 
	IF @MaxUserPay IS NULL SET @MaxUserPay=0
	IF @MaxGroupPay IS NULL	SET @MaxGroupPay=0

	IF @UserIngot<@CreateConfigDeductIngot+@MaxUserPay+@MaxGroupPay
	BEGIN
		SET @strErrorDescribe=N'����ʧ�ܣ�������ʯ����'+CONVERT(NVARCHAR(16),@CreateConfigDeductIngot+@MaxUserPay+@MaxGroupPay)+',�޷������淨��'
		RETURN 6		
	END

	DECLARE @valueMin INT
	DECLARE @valueMax INT
	IF @cbTaxMode < 3
	BEGIN

		SELECT @valueMin= OptionValue FROM IMGroupOption WHERE OptionName='FixedMin'
		IF @valueMin IS NULL
			SET @valueMin=0
		
		SELECT @valueMax= OptionValue FROM IMGroupOption WHERE OptionName='FixedMax'
		IF @valueMax IS NULL
			SET @valueMax=100
	END
	ELSE
	BEGIN
		SELECT @valueMin= OptionValue FROM IMGroupOption WHERE OptionName='PercentMin'
		IF @valueMin IS NULL
			SET @valueMin=0
		
		SELECT @valueMax= OptionValue FROM IMGroupOption WHERE OptionName='PercentMax'
		IF @valueMax IS NULL
			SET @valueMax=100	
	END

	IF @lTaxCount<@valueMin OR @lTaxCount>@valueMax
	BEGIN
		SET @strErrorDescribe=N'����ʧ�ܣ� ������ֵ��Χ'+CONVERT(NVARCHAR(16),@valueMin) +'-'+CONVERT(NVARCHAR(16),@valueMax)+'��'
		RETURN 7		
	END

	-- �����淨
	DECLARE @CreateDateTime DATETIME
	SET @CreateDateTime=GetDate()

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond-@CreateConfigDeductIngot WHERE UserID=@dwCreaterID
	-- Ⱥ������
	INSERT INTO IMGroupConfig(CreaterID, GroupID,ConfigName, KindID,PlayMode, CellScore, DrawCountLimit, DrawTimeLimit,JoinCount,RoomTax, PasswordMode,PayMode,GameMode,JoinMode,EnterScore,LeaveScore,GameRule,TaxMode,TaxCount,LessScoreDismiss,TimeSecond, GroupPersonalMaxChair)
	VALUES (@GameID, @dwGroupID,@strConfigName, @wKindID,@cbPlayMode, @lCellScore, @dwDrawCountLimit,@dwDrawTimeLimit,@wJoinCount,@dwRoomTax, @cbPassword,@cbPayMode,@cbGameMode,@cbJoinMode,@lEnterScore,@lLeaveScore,@strGameRule,@cbTaxMode,@lTaxCount,@cbLessScoreDismiss,@lTimeSecond, @cbGroupPersonalMaxChair)
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- ������Ϣ
		SET @strErrorDescribe=N'����ϵͳԭ��,�淨����ʧ��[0]������ϵ�ͻ����������˽���ϸ�����'
		RETURN 6
	END
	

	-- ��ѯȺ��
	DECLARE @dwConfigID BIGINT
	SELECT 	@dwConfigID=SCOPE_IDENTITY()

	SELECT @dwConfigID AS ConfigID,@UserIngot-@CreateConfigDeductIngot AS Diamond

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED		
	
END

RETURN 0

GO



----------------------------------------------------------------------------------------------------

-- �޸��淨
CREATE  PROCEDURE dbo.GSP_IM_ModifyConfig
	-- ��������
	@dwGroupID   INT,							-- ���ֲ�ID
	@dwUserID INT,								-- �޸����ID
	@dwConfigID	 INT,							-- �淨ID
	@strConfigName NVARCHAR(12),				-- �淨����
	@wKindID     INT,							-- ���ͱ�ʶ
	@cbPlayMode	 TINYINT,						-- ����ģʽ
	@lCellScore BIGINT,							-- ��Ԫ����
	@dwDrawCountLimit BIGINT,					-- ��������
	@dwDrawTimeLimit BIGINT,					-- ʱ������
	@wJoinGamePeopleCount INT,					-- ��������
	@cbPassword	TINYINT,						-- ����ģʽ
	@cbPayMode	TINYINT,						-- ֧��ģʽ
	@cbGameMode	TINYINT,						-- ��Ϸģʽ
	@cbJoinMode	TINYINT,						-- ����ģʽ
	@lEnterScore BIGINT,						-- �����
	@lLeaveScore BIGINT,						-- �뿪��	
	@cbTaxMode	TINYINT,						-- �������ģʽ
	@lTaxCount  BIGINT,							-- ���������ֵ
	@cbLessScoreDismiss  TINYINT,				-- ����������ɢ
	@strGameRule NVARCHAR(200),					-- ���ƹ���

	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ����뵽���¿�ͨ����ܼ���ʹ�ã�'
		RETURN 3
	END

	-- ���Ȩ��
	DECLARE @MemberRight INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF (@MemberRight&0x01) <> 0
		SET @HaveRight=1
	ELSE
	BEGIN

		IF (@MemberRight&0x02) <> 0--����Ա
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
			IF (@ManagerRight&0x40)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ���޸��淨��'
		RETURN 4		
	END

	DECLARE @ConfigID INT
	SELECT @ConfigID=ConfigID FROM IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID
	IF @ConfigID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�淨�����ڣ��������ã�'
		RETURN 5	
	END

	IF @cbPlayMode=1 AND NOT EXISTS(SELECT 1 FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND CanUseMedal=1 )
	BEGIN
		SET @strErrorDescribe=N'��Ǹ���ô�������Ȩ����ѫ���淨��'
		RETURN 6	
	END

	DECLARE @valueMin INT
	DECLARE @valueMax INT
	IF @cbTaxMode < 3
	BEGIN

		SELECT @valueMin= OptionValue FROM IMGroupOption WHERE OptionName='FixedMin'
		IF @valueMin IS NULL
			SET @valueMin=0
		
		SELECT @valueMax= OptionValue FROM IMGroupOption WHERE OptionName='FixedMax'
		IF @valueMax IS NULL
			SET @valueMax=100
	END
	ELSE
	BEGIN
		SELECT @valueMin= OptionValue FROM IMGroupOption WHERE OptionName='PercentMin'
		IF @valueMin IS NULL
			SET @valueMin=0
		
		SELECT @valueMax= OptionValue FROM IMGroupOption WHERE OptionName='PercentMax'
		IF @valueMax IS NULL
			SET @valueMax=100	
	END

	IF @lTaxCount<@valueMin OR @lTaxCount>@valueMax
	BEGIN
		SET @strErrorDescribe=N'����ʧ�ܣ� ������ֵ��Χ'+CONVERT(NVARCHAR(16),@valueMin) +'-'+CONVERT(NVARCHAR(16),@valueMax)+'��'
		RETURN 7		
	END

	-- �޸��淨
	UPDATE IMGroupConfig SET ConfigName=@strConfigName,PlayMode=@cbPlayMode, CellScore=@lCellScore, DrawCountLimit=@dwDrawCountLimit, DrawTimeLimit=@dwDrawTimeLimit, PasswordMode=@cbPassword,
	PayMode=@cbPayMode,GameMode=@cbGameMode,JoinMode=@cbJoinMode,EnterScore=@lEnterScore,LeaveScore=@lLeaveScore,GameRule=@strGameRule,TaxMode=@cbTaxMode,TaxCount=@lTaxCount,LessScoreDismiss=@cbLessScoreDismiss
	 WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID

	Select KindID,TimeSecond From IMGroupConfig  WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ���淨
CREATE  PROCEDURE dbo.GSP_IM_DeleteConfig
	@dwGroupID INT,								-- Ⱥ���ʶ
	@dwUserID INT,								-- �û���ʶ
	@dwConfigID INT,							-- �淨��ʶ
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ����뵽���¿�ͨ����ܼ���ʹ�ã�'
		RETURN 3
	END

	-- ���Ȩ��
	DECLARE @MemberRight INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF (@MemberRight&0x01) <> 0
		SET @HaveRight=1
	ELSE
	BEGIN

		IF (@MemberRight&0x02) <> 0--����Ա
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
			IF (@ManagerRight&0x40)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ���޸��淨��'
		RETURN 4		
	END

	DECLARE @ConfigID INT
	SELECT @ConfigID=ConfigID FROM IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID
	IF @ConfigID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�淨�����ڣ����飡'
		RETURN 5	
	END

	DELETE FROM IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID
	--������淨���ղ�
	UPDATE IMGroupMember SET FavoriteConfig=0 WHERE GroupID=@dwGroupID AND FavoriteConfig=@dwConfigID
END

RETURN 0

GO



----------------------------------------------------------------------------------------------------


-- ɾ���淨
CREATE  PROCEDURE dbo.GSP_IM_QueryCreate
	@dwGroupID INT,								-- Ⱥ���ʶ
	@dwConfigID INT,							-- �淨��ʶ
	@dwUserID INT,								-- �û���ʶ
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
		-- ��ѯ����
	DECLARE @MaxNullTableCount SMALLINT		
	SELECT @MaxNullTableCount=OptionValue FROM IMGroupOption(NOLOCK) WHERE OptionName=N'MaxNullTableCount'
	IF @MaxNullTableCount IS NULL SET @MaxNullTableCount=5

	Select @MaxNullTableCount AS MaxNullTableCount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- �ղ��淨
CREATE  PROCEDURE dbo.GSP_IM_SetFavoriteConfig
	@dwGroupID INT,								-- Ⱥ���ʶ
	@dwUserID INT,								-- �û���ʶ
	@dwConfigID INT,							-- �淨��ʶ
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	IF NOT EXISTS(SELECT 1 FROM IMGroupProperty WHERE GroupID=@dwGroupID)
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ�������飡'
		RETURN 1	
	END

	IF NOT EXISTS(SELECT 1 FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe=N'�����Ǹô����˳�Ա������ʧ�ܣ�'
		RETURN 2	
	END

	IF NOT EXISTS(SELECT 1 FROM IMGroupConfig WHERE GroupID=@dwGroupID AND ConfigID=@dwConfigID)
	BEGIN
		SET @strErrorDescribe=N'�淨�����ڣ����飡'
		RETURN 3	
	END
	
	UPDATE IMGroupMember SET FavoriteConfig=@dwConfigID WHERE GroupID=@dwGroupID AND UserID=@dwUserID

END

RETURN 0

GO

