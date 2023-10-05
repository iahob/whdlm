
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ApplyRequest]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ApplyRequest]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ApplyRespond]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ApplyRespond]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ApplyDelete]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ApplyDelete]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_LoadApplyMessage]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_LoadApplyMessage]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- ��������
CREATE  PROCEDURE dbo.GSP_IM_ApplyRequest
	-- �û���Ϣ	
	@dwGroupID BIGINT,							-- Ⱥ���ʶ	
	@dwApplyerID INT,							-- �û���ʶ
	@cbApplyType TINYINT,						-- ��������
	@strPassword NCHAR(32),						-- �û�����
	@lInvitationCode BIGINT,					-- ������
	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @CustomID	INT
DECLARE @NickName	NVARCHAR(32)

-- Ⱥ����Ϣ
DECLARE @CreaterID  INT
DECLARE @GroupName  NVARCHAR(32)
DECLARE @GroupStatus TINYINT
DECLARE @MemberCount SMALLINT
DECLARE @MaxMemberCount SMALLINT
DECLARE @GroupIDShow TINYINT
-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @CustomID=CustomID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwApplyerID

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

	---- �����ж�
	--IF @LogonPass<>@strPassword
	--BEGIN
	--	SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
	--	RETURN 4
	--END

	-- ��ѯȺ��	
	IF @dwGroupID=0 AND @lInvitationCode>0
	BEGIN
		DECLARE @TempGroupID BIGINT
		DECLARE @SpreadUserID BIGINT
		SELECT @TempGroupID=GroupID,@SpreadUserID=UserID FROM IMGroupInvitationCode WHERE InvitationCode=@lInvitationCode
		IF @TempGroupID IS NULL
		BEGIN
			SET @strErrorDescribe=N'��������Ϣ�����ڣ����֤���ٴγ��ԣ�'		
			RETURN 5
		END
		
		DECLARE @SpreadRight INT
		SELECT @SpreadRight=MemberRight FROM IMGroupMember WHERE GroupID=@TempGroupID AND UserID=@SpreadUserID
		IF @SpreadRight IS NULL OR (@SpreadRight<>0x01 AND @SpreadRight<>4)
		BEGIN
			SET @strErrorDescribe=N'��Ч�������룬���֤���ٴγ��ԣ�'		
			RETURN 6
		END

		SELECT @GroupIDShow=GroupIDShow FROM IMGroupBattleRule(NOLOCK) WHERE GroupID=@TempGroupID
		SELECT @CreaterID=CreaterID,@GroupName=GroupName,@GroupStatus=GroupStatus,@MemberCount=MemberCount,@MaxMemberCount=MaxMemberCount FROM IMGroupProperty(NOLOCK) WHERE GroupID=@TempGroupID

		--IF @GroupIDShow<>0
		--BEGIN
		--	SET @strErrorDescribe=N'�ô�����ֻ����ͨ��ID���룡'		
		--	RETURN 6
		--END

		SET @dwGroupID=@TempGroupID
	END
	ELSE IF @dwGroupID>0
	BEGIN
		SELECT @CreaterID=CreaterID,@GroupName=GroupName,@GroupStatus=GroupStatus,@MemberCount=MemberCount,@MaxMemberCount=MaxMemberCount FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID
		SELECT @GroupIDShow=GroupIDShow FROM IMGroupBattleRule(NOLOCK) WHERE GroupID=@dwGroupID
		
		--IF @GroupIDShow<>1 AND @cbApplyType=0
		--BEGIN
		--	SET @strErrorDescribe=N'�ô�����ֻ����ͨ����������룡'		
		--	RETURN 6
		--END
	END
	-- �����˲�����
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'������Ĵ�������Ϣ�����ڣ����֤���ٴγ��ԣ�'		
		RETURN 5
	END

	-- ��������ͣ��
	IF @GroupStatus<>1
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��������Ĵ������ѱ�����ֹͣʹ�ã�'				
		RETURN 7
	END

	-- ��ȡȺ���б�
	DECLARE @strGroupIDArray NVARCHAR(1024)
	SELECT @strGroupIDArray=GroupIDArray FROM IMUserGroupInfo(NOLOCK) WHERE UserID=@dwApplyerID
	IF @strGroupIDArray IS NULL 
	BEGIN
		SET @strGroupIDArray=';'
		INSERT INTO IMUserGroupInfo VALUES (@dwApplyerID,@strGroupIDArray)
	END

	-- ���ұ�ʶ
	DECLARE @strGroupID NVARCHAR(20)
	SET @strGroupID=';'+CONVERT(NVARCHAR(18),@dwGroupID)+';';

	IF @cbApplyType=0
	BEGIN
		-- �����ж�
		IF EXISTS(SELECT * FROM IMGroupApplyRecord(NOLOCK) WHERE GroupID=@dwGroupID AND ApplyerID=@dwApplyerID AND ApplyStatus=0 AND ApplyType=0)
		BEGIN
			SET @strErrorDescribe=N'���Ѿ��������ô����ˣ�����Ҫ�ظ����룬�����ĵȴ�������ˣ�'		
			RETURN 5					
		END	

		-- ��������Ա
		IF @MemberCount=@MaxMemberCount
		BEGIN
			SET @strErrorDescribe=N'��Ǹ���ô���������Ա��������������룡'				
			RETURN 8		
		END	

		-- ��������
		DECLARE @wCreateGroupCount SMALLINT
		SELECT @wCreateGroupCount=COUNT(GroupID) FROM IMGroupProperty(NOLOCK) WHERE CreaterID=@dwApplyerID AND GroupStatus=1

	
		IF CHARINDEX(@strGroupID,@strGroupIDArray)>0
		BEGIN		
			SET @strErrorDescribe=N'��Ǹ�����Ѿ��Ǹô����˵ĳ�Ա��'
			RETURN 9
		END	

		-- ��ѯ����
		DECLARE @MaxJoinGroupCount SMALLINT		
		SELECT @MaxJoinGroupCount=OptionValue FROM IMGroupOption(NOLOCK) WHERE OptionName=N'MaxJoinGroupCount'
		IF @MaxJoinGroupCount IS NULL SET @MaxJoinGroupCount=5
	
		-- Ⱥ����Ŀ
		DECLARE @dwGroupCount SMALLINT	
		SELECT @dwGroupCount=(LEN(@strGroupIDArray)-LEN(REPLACE(@strGroupIDArray, ';', '')))

		-- ��������
		IF @dwGroupCount>0 SET @dwGroupCount=@dwGroupCount-1
	
		-- ͳ�ƴ���	
		DECLARE @ApplyCount SMALLINT
		SELECT @ApplyCount=COUNT(GroupID) FROM IMGroupApplyRecord(NOLOCK) WHERE ApplyerID=@dwApplyerID AND ApplyStatus=0

		-- ����У��
		IF (@dwGroupCount-@wCreateGroupCount)+@ApplyCount>=@MaxJoinGroupCount
		BEGIN
			SET @strErrorDescribe=N'��Ǹ�����������Ĵ����˸����Ѵﵽϵͳ���õ������Ŀ��'		
			RETURN 10				
		END

		---------------------------------------------------------------------------------------------------------------------------

		-- �����¼
		INSERT INTO IMGroupApplyRecord(GroupID, ApplyerID, ApplyerGameID, ApplyerNickName, ApplyerCustomID, ApplyStatus, CreaterID, GroupName,ApplyType, CollectDateTime,InvitationCode)
		VALUES (@dwGroupID, @dwApplyerID, @GameID, @NickName, @CustomID, 0, @CreaterID, @GroupName,@cbApplyType, GetDate(),@lInvitationCode)	

		-- ��¼��ʶ
		DECLARE @RecordID INT
		SELECT 	@RecordID=SCOPE_IDENTITY()

		DECLARE @cbInMode TINYINT
		SELECT @cbInMode=InMode FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
		IF @cbInMode=0
		BEGIN
			-- ����ɹ�
			UPDATE IMGroupApplyRecord SET ApplyStatus=1,DeleteMask=2 ,OperatorName=N'�Զ�����' WHERE RecordID=@RecordID
			SET @strErrorDescribe=N'����ɹ���'	
		END
		ELSE
		BEGIN
			-- ����ɹ�
			SET @strErrorDescribe=N'�������ɹ��������ĵȴ�������ˣ�'	
		END
	END
	ELSE IF @cbApplyType=1
	BEGIN
		-- �����ж�
		IF EXISTS(SELECT * FROM IMGroupApplyRecord(NOLOCK) WHERE GroupID=@dwGroupID AND ApplyerID=@dwApplyerID AND ApplyStatus=0 AND ApplyType=1)
		BEGIN
			SET @strErrorDescribe=N'���Ѿ������˳�������Ҫ�ظ����룬�����ĵȴ�������ˣ�'		
			RETURN 5					
		END	

		IF CHARINDEX(@strGroupID,@strGroupIDArray)=0
		BEGIN		
			SET @strErrorDescribe=N'�����˳�ʧ�ܣ��㲻�Ǹô����˵ĳ�Ա��'
			RETURN 9
		END	

		-- �˳���¼
		INSERT INTO IMGroupApplyRecord(GroupID, ApplyerID, ApplyerGameID, ApplyerNickName, ApplyerCustomID, ApplyStatus, CreaterID, GroupName,ApplyType, CollectDateTime)
		VALUES (@dwGroupID, @dwApplyerID, @GameID, @NickName, @CustomID, 0, @CreaterID, @GroupName,@cbApplyType, GetDate())	

		-- ��¼��ʶ
		SELECT 	@RecordID=SCOPE_IDENTITY()

		DECLARE @cbOutMode TINYINT
		SELECT @cbOutMode=OutMode FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
		IF @cbOutMode=0
		BEGIN
			-- �˳��ɹ�
			UPDATE IMGroupApplyRecord SET ApplyStatus=1,DeleteMask=2,OperatorName=N'�Զ��˳�' WHERE RecordID=@RecordID
			SET @strErrorDescribe=N'���ѳɹ��˳��ô����ˣ�'	
		END
		ELSE
		BEGIN
			-- ����ɹ�
			SET @strErrorDescribe=N'�����˳��ɹ��������ĵȴ�������ˣ�'	
		END
	END
	ELSE
	BEGIN
		SET @strErrorDescribe=N'�������ʹ��������ԣ�'		
		RETURN 5	
	END


	-- ��ѯ��Ϣ
	SELECT * FROM IMGroupApplyRecord WHERE RecordID=@RecordID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ������Ӧ
CREATE  PROCEDURE dbo.GSP_IM_ApplyRespond
	-- �û���Ϣ
	@dwRecordID INT,							-- ��¼��ʶ 
	@dwUserID	 INT,							-- �û���ʶ
	@dwApplyerID INT,							-- �û���ʶ	
	@dwGroupID INT,								-- վ���ʶ		
	@strPassword NCHAR(32),						-- �û�����
	@cbApplyStatus TINYINT,						-- ����״̬
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
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

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

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 4
	END

	-- ��ѯȺ��
	DECLARE @CreaterID INT
	DECLARE @MemberCount SMALLINT
	DECLARE @MaxMemberCount SMALLINT
	SELECT @CreaterID=CreaterID, @MemberCount=MemberCount, @MaxMemberCount=MaxMemberCount FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF @CreaterID = @dwUserID
		SET @HaveRight=1
	ELSE
	BEGIN
		DECLARE @UserRight BIGINT
		SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		IF @UserRight IS NULL
		BEGIN
			SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ���ã�'
			RETURN 6	
		END

		IF (@UserRight&2) <> 0--����Ա
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x02)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ�޽��д˲�����'
		RETURN 7		
	END

	-- ��ѯ��¼
	DECLARE @GroupID INT
	DECLARE @ApplyerID INT
	DECLARE @ApplyStatus TINYINT	
	DECLARE @ApplyType TINYINT	
	SELECT @GroupID=GroupID,@ApplyerID=ApplyerID,@ApplyStatus=ApplyStatus ,@ApplyType=ApplyType FROM IMGroupApplyRecord(NOLOCK) WHERE RecordID=@dwRecordID

	IF @ApplyerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'ϵͳδ��ѯ����ص������¼������ϵ�ͻ����������˽���ϸ�����'
		RETURN 6		
	END 

	IF @dwApplyerID<>@ApplyerID OR @dwGroupID<>@GroupID
	BEGIN
		SET @strErrorDescribe=N'ϵͳ��⵽�����¼�쳣������ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END	

	-- ��������Ա
	IF @MemberCount=@MaxMemberCount AND @cbApplyStatus=1 AND @ApplyType=0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�����Ĵ������Ѿ���Ա����Ա���ʧ�ܣ�'
		RETURN 5		
	END



	-- Ⱥ������
	UPDATE IMGroupApplyRecord SET ApplyStatus=@cbApplyStatus,DeleteMask=2,OperatorID=@GameID,OperatorName=@NickName WHERE RecordID=@dwRecordID		

	-- ��ѯ��¼
	SELECT * FROM IMGroupApplyRecord(NOLOCK) WHERE RecordID=@dwRecordID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ����ɾ��
CREATE  PROCEDURE dbo.GSP_IM_ApplyDelete
	-- �û���Ϣ
	@dwMessageID INT,							-- ��Ϣ��ʶ 
	@cbMessageType INT,							-- ��Ϣ���� 
	@dwUserID INT,								-- �û���ʶ	
	@strPassword NCHAR(32),						-- �û�����	
	
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
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

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

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 4
	END

	-- ��ѯ��Ϣ
	DECLARE @CreaterID INT
	DECLARE @ApplyerID INT
	DECLARE @ApplyStatus TINYINT
	DECLARE @DeleteMask TINYINT
	SELECT @CreaterID=CreaterID,@ApplyerID=ApplyerID,@ApplyStatus=ApplyStatus,@DeleteMask=DeleteMask FROM IMGroupApplyRecord(NOLOCK) WHERE RecordID=@dwMessageID

	-- ��Ϣ������
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ǹ������δ��ѯ����Ҫɾ������Ϣ��'
		RETURN 5		
	END

	-- ������ɾ��
	IF @cbMessageType<>1 OR @dwUserID<>@ApplyerID
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��������������,�޷�ɾ����Ϣ��'
		RETURN 6		
	END	

	-- ɾ����Ϣ
	DELETE IMGroupApplyRecord WHERE RecordID=@dwMessageID

	SELECT @CreaterID AS CreaterID
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ������Ӧ
CREATE  PROCEDURE dbo.GSP_IM_LoadApplyMessage
	-- �û���Ϣ	
	@dwUserID INT,								-- �û���ʶ
	@cbMessageType INT							-- ��Ϣ����		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ��Ϣ
	--SELECT * FROM IMGroupApplyRecord(NOLOCK) WHERE CreaterID=@dwUserID 
	--�����Լ������������Լ��ǹ���Ա���Ҿ������Ȩ�޵���Ϣ
	SELECT * FROM IMGroupApplyRecord(NOLOCK) WHERE GroupID IN 
	(SELECT A.GroupID FROM IMGroupProperty A,IMGroupBattleRule B,IMGroupMember C WHERE A.GroupID=B.GroupID AND 
	A.GroupID=C.GroupID AND (A.CreaterID=@dwUserID OR (B.ManagerRight&0x02)<>0 /*AND (C.MemberRight&0x02)<>0 AND C.UserID=@dwUserID*/)
	) 
	ORDER BY CollectDateTime 

END

RETURN 0

GO
