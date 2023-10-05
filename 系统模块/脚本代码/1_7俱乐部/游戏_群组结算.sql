
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryBattle]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryBattle]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryBattleInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryBattleInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SettleBattle]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SettleBattle]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_PresentGroupScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_PresentGroupScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_PresentGroupScoreEx]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_PresentGroupScoreEx]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryPresent]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryPresent]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryRevenue]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryRevenue]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetFlag]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SetFlag]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ClearMedal]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ClearMedal]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- Լս��ѯ
CREATE  PROCEDURE dbo.GSP_IM_QueryBattle	
	@dwGroupID	BIGINT,			-- Ⱥ���ʶ	
	@dwCreaterID INT,			-- �û���ʶ
	@strPassword NCHAR(32),		-- �û�����

	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- �����Ϣ
WITH ENCRYPTION AS

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwCreaterID

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

	-- ��ѯ������
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- �����˲�����
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫ��ѯ�Ĵ�������Ϣ�����ڣ����֤���ٴγ��ԣ�'		
		RETURN 5
	END

	-- ����Ⱥ��
	IF @CreaterID<>@dwCreaterID
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵�������û��Ȩ�޽��д˲�����'
		RETURN 6
	END

	--��ȡ���ֲ�Լս������Ϣ
	DECLARE @ConsumeIngot	BIGINT
	DECLARE @BattleCreate	INT
	SELECT @ConsumeIngot=ConsumeIngot,@BattleCreate=BattleCreate FROM IMGroupWealth WHERE GroupID=@dwGroupID
	
	IF @ConsumeIngot IS NULL OR @BattleCreate IS NULL
	BEGIN
		SET @ConsumeIngot=0
		SET @BattleCreate=0
	END
	
	SELECT @ConsumeIngot AS ConsumeIngot,@BattleCreate AS BattleCreate
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- Լս��Ϣ��ѯ
CREATE  PROCEDURE dbo.GSP_IM_QueryBattleInfo	
	@dwGroupID	BIGINT			-- Ⱥ���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	-- ��ѯ������
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- �����˲�����
	IF @CreaterID IS NULL
	BEGIN
		RETURN 1
	END

	SELECT * FROM StreamGroupBalance WHERE GroupID = @dwGroupID
	
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- Լս����
CREATE  PROCEDURE dbo.GSP_IM_SettleBattle	
	@dwGroupID	BIGINT,			-- Ⱥ���ʶ	
	@dwMemberID INT,			-- ��Ա��ʶ
	@wSettleCount SMALLINT,		-- �������
	@dwCreaterID INT,			-- �û���ʶ
	@strPassword NCHAR(32),	-- �û�����

	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- �����Ϣ
WITH ENCRYPTION AS

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwCreaterID

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

	-- ��ѯ������
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- �����˲�����
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫ�����Ĵ�������Ϣ��������Ϣ�����ڣ����֤���ٴγ��ԣ�'		
		RETURN 5
	END

	-- ����Ⱥ��
	IF @CreaterID<>@dwCreaterID
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵�������û��Ȩ�޽��д˲�����'
		RETURN 6
	END

	-- ��������
	DECLARE @BattleCount INT
	DECLARE @BalanceCount INT
	SELECT @BattleCount=BattleCount,@BalanceCount=BalanceCount FROM StreamGroupBalance(NOLOCK) WHERE UserID=@dwMemberID AND GroupID=@dwGroupID

	-- ����У��
	IF @BattleCount IS NULL OR @BalanceCount IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��ϵͳδ��ѯ���ó�Ա��Լսͳ����Ϣ������ʧ�ܣ�'
		RETURN 7				
	END

	-- ����У��
	IF @wSettleCount>@BattleCount
	BEGIN
		SET @strErrorDescribe=N'��Ǹ������������ܳ���Լս����������ʧ�ܣ�'
		RETURN 8
	END
	
	IF @BalanceCount>@wSettleCount
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������������С����ʷ����������ʧ�ܣ�'
		RETURN 8
	END

	-- ��������
	UPDATE StreamGroupBalance SET BalanceCount=@wSettleCount WHERE GroupID=@dwGroupID AND UserID=@dwMemberID	
	
	SET @strErrorDescribe=N'����ɹ���'

	SELECT UserID,BattleCount,BalanceCount FROM StreamGroupBalance WHERE GroupID=@dwGroupID AND UserID=@dwMemberID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����ѫ��
CREATE  PROCEDURE dbo.GSP_IM_PresentGroupScore
	@dwGroupID	BIGINT,			-- Ⱥ���ʶ
	@dwPresentUserID INT,		-- �������ID
	@lScore BIGINT,				-- ���ͷ�
	@cbType TINYINT,			-- ���ķ�ʽ 0���� 1����
	@nCount INT,				-- 
	@strPassword NCHAR(32),		-- �û�����
	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	-- ��ѯ�û�
	DECLARE @UserID	INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT	
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwPresentUserID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 2
	END

	-- ��ѯ������
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- �����˲�����
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ�����ڣ����֤���ٴγ��ԣ�'	
		RETURN 3
	END

	DECLARE @MemberRight  INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwPresentUserID
	IF @MemberRight IS NULL
		SET @MemberRight=0

	DECLARE @cbHaveRight TINYINT
	SET @cbHaveRight=0

	IF @CreaterID=@dwPresentUserID
		SET @cbHaveRight=1
	ELSE IF (@MemberRight&2)<>0 
	BEGIN
		DECLARE @ManagerRight INT
		DECLARE @Flag TINYINT
		SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwPresentUserID
		IF @Flag<>1
			SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

		IF @ManagerRight IS NULL
			SET @ManagerRight=0
		IF (@ManagerRight&0x10)=0
		BEGIN
			SET @strErrorDescribe=N'����ʧ�ܣ�����ԱȨ�޲�����'	
			RETURN 4
		END

		SET @cbHaveRight=2
	END
	ELSE IF (@MemberRight&4)<>0
	BEGIN
		DECLARE @CaptainRight INT
		SELECT @CaptainRight=CaptainRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

		IF @CaptainRight IS NULL
			SET @CaptainRight=0
		IF (@CaptainRight&0x04)=0
		BEGIN
			SET @strErrorDescribe=N'����ʧ�ܣ��ӳ�Ȩ�޲�����'	
			RETURN 5
		END

		SET @cbHaveRight=4
	END
	
	IF @cbHaveRight=0
	BEGIN
		SET @strErrorDescribe=N'����ʧ�ܣ�Ȩ�޲�����'	
		RETURN 6
	END

	DECLARE @lAllScore BIGINT
	SET @lAllScore=0
	
	IF @cbHaveRight<4
	BEGIN
		SELECT @lAllScore=Score FROM IMGroupWealth WHERE GroupID = @dwGroupID
		IF @cbType=0
		BEGIN
			IF @lAllScore IS NULL OR @lAllScore<@lScore*@nCount
			BEGIN
				SET @strErrorDescribe=N'����ʧ�ܣ����ֲ�ѫ�²������ţ�'
				RETURN 6
			END
		END
	END
	ELSE
	BEGIN
		SELECT @lAllScore=Score FROM IMGroupMember WHERE GroupID = @dwGroupID AND UserID=@dwPresentUserID

		IF @cbType=0
		BEGIN
			IF @lAllScore IS NULL OR @lAllScore<@lScore*@nCount
			BEGIN
				SET @strErrorDescribe=N'����ʧ�ܣ��ӳ�ѫ�²������ţ�'
				RETURN 7
			END
		END
	END

	SELECT @cbHaveRight AS UserRight,@lAllScore AS BeforeScore
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- ����ѫ��
CREATE  PROCEDURE dbo.GSP_IM_PresentGroupScoreEx
	@dwGroupID	BIGINT,			-- Ⱥ���ʶ
	@dwPresentUserID INT,		-- �������ID
	@dwReceiveUserID INT,		-- �������ID	
	@lScore BIGINT,				-- ���ͷ�
	@cbType TINYINT				-- ���ķ�ʽ 0���� 1����
--WITH ENCRYPTION AS
AS
-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	DECLARE @right TINYINT
	SELECT @right=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwPresentUserID

	DECLARE @CurUserScore BIGINT
	select @CurUserScore=Score from IMGroupMember where UserID=@dwReceiveUserID AND GroupID=@dwGroupID
	IF @CurUserScore IS NULL
	BEGIN
		SELECT 0 AS CurScore
		RETURN 1
	END

	DECLARE @CurGroupScore BIGINT
	IF @right<4
		select @CurGroupScore=Score from IMGroupWealth where GroupID=@dwGroupID
	ELSE
	BEGIN
		SELECT @CurGroupScore=Score FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwPresentUserID
		IF @dwPresentUserID <> (SELECT CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwReceiveUserID)
		BEGIN
			SELECT 0 AS CurScore
			RETURN 1
		END
	END

	IF @CurGroupScore IS NULL 
		SET @CurGroupScore=0

	IF @cbType=0--����
	BEGIN

		IF @CurGroupScore<@lScore
		BEGIN
			SELECT @CurUserScore AS CurScore
			RETURN 1
		END
		IF @right<4
		BEGIN
			update IMGroupWealth SET Score=Score-@lScore WHERE GroupID=@dwGroupID
			INSERT INTO RecordGroupScoreChange(GroupID,BeforeScore,ChangeType,ChangeScore,AfterScore,CollectDate) 
				VALUES(@dwGroupID,@CurGroupScore,@cbType,-@lScore,@CurGroupScore-@lScore,GETDATE())
		END
		ELSE
		BEGIN
			update IMGroupMember SET Score=Score-@lScore WHERE UserID=@dwPresentUserID AND GroupID=@dwGroupID
			INSERT INTO RecordUserScoreChange(GroupID,UserID,BeforeScore,ChangeType,ChangeScore,AfterScore,CollectDate) 
				VALUES(@dwGroupID,@dwPresentUserID,@CurGroupScore,@cbType,-@lScore,@CurGroupScore-@lScore,GETDATE())
		END

		update IMGroupMember SET Score=Score+@lScore WHERE UserID=@dwReceiveUserID AND GroupID=@dwGroupID
		INSERT INTO RecordUserScoreChange(GroupID,UserID,BeforeScore,ChangeType,ChangeScore,AfterScore,CollectDate) 
			VALUES(@dwGroupID,@dwReceiveUserID,@CurUserScore,@cbType,@lScore,@CurUserScore+@lScore,GETDATE())
	END
	ELSE--�۳�
	BEGIN

		IF @CurUserScore<@lScore
		BEGIN
			SELECT @CurUserScore AS CurScore
			RETURN 2
		END

		IF @right<4
		BEGIN
			update IMGroupWealth SET Score=Score+@lScore WHERE GroupID=@dwGroupID
			INSERT INTO RecordGroupScoreChange(GroupID,BeforeScore,ChangeType,ChangeScore,AfterScore,CollectDate) 
				VALUES(@dwGroupID,@CurGroupScore,@cbType,@lScore,@CurGroupScore+@lScore,GETDATE())
		END
		ELSE
		BEGIN
			update IMGroupMember SET Score=Score+@lScore WHERE UserID=@dwPresentUserID AND GroupID=@dwGroupID
			INSERT INTO RecordUserScoreChange(GroupID,UserID,BeforeScore,ChangeType,ChangeScore,AfterScore,CollectDate) 
				VALUES(@dwGroupID,@dwPresentUserID,@CurGroupScore,@cbType,@lScore,@CurGroupScore+@lScore,GETDATE())
		END

		update IMGroupMember SET Score=Score-@lScore WHERE UserID=@dwReceiveUserID AND GroupID=@dwGroupID
		INSERT INTO RecordUserScoreChange(GroupID,UserID,BeforeScore,ChangeType,ChangeScore,AfterScore,CollectDate) 
			VALUES(@dwGroupID,@dwReceiveUserID,@CurUserScore,@cbType,-@lScore,@CurUserScore-@lScore,GETDATE())
	END

	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID
	DECLARE @PresentNickName NVARCHAR(31)
	DECLARE @PresentGameID INT
	DECLARE @ReceiveNickName NVARCHAR(31)
	DECLARE @ReceiveGameID INT
	select @PresentNickName=NickName,@PresentGameID=GameID From WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) where UserID=@dwPresentUserID
	select @ReceiveNickName=NickName,@ReceiveGameID=GameID From WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) where UserID=@dwReceiveUserID



	IF @cbType=1
		SET @lScore=-1*@lScore

	SET @CurUserScore+=@lScore

	INSERT INTO RecordGroupScorePresent(GroupID,PresentUserID,PresentGameID,PresentNickName,PresentUserRight,ReceiveUserID,ReceiveGameID,ReceiveNickName,ChangeType,ChangeScore,CurScore,CollectDate) 
	VALUES(@dwGroupID,@dwPresentUserID,@PresentGameID,@PresentNickName,@right,@dwReceiveUserID,@ReceiveGameID,@ReceiveNickName,@cbType,@lScore,@CurUserScore,GETDATE())

	--SELECT @CurUserScore=Score from IMGroupMember where UserID=@dwReceiveUserID AND GroupID=@dwGroupID
	DECLARE @DateID INT
	SET @DateID=CONVERT(int,CONVERT(nvarchar(8),GETDATE(),112))
	IF NOT EXISTS(SELECT 1 FROM RecordUserDayInfo WHERE GroupID=@dwGroupID AND UserID=@dwReceiveUserID AND DateID=@DateID)
	BEGIN
		DECLARE @szUserName nvarchar(31)
		SELECT @szUserName=NickName FROM WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwReceiveUserID

		insert   WHQJGroupDB.dbo.RecordUserDayInfo (DateID,GroupID,UserID,UserName,CurScore,CreateCount,DrawCount,BigWinnerCount,ScoreCount,RevenueCount,ContributionValue,Flag,InsertTime)
			 values(@dateID,@dwGroupID,@dwReceiveUserID,@szUserName,@CurUserScore,0,0,0,0,0,0,0,GETDATE())
	END
	ELSE
	BEGIN
		update   WHQJGroupDB.dbo.RecordUserDayInfo SET CurScore=@CurUserScore where DateID=@dateID AND UserID=@dwReceiveUserID AND GroupID=@dwGroupID
	END

	SELECT @CurUserScore AS CurScore
END

RETURN 0

GO
----------------------------------------------------------------------------------
-- ���ͼ�¼��ѯ
CREATE  PROCEDURE dbo.GSP_IM_QueryPresent
	@dwGroupID	INT,			-- Ⱥ���ʶ	
	@dwQueryID	INT,			-- ��ѯ��ʶ
	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	-- ��ѯ������
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- �����˲�����
	IF @CreaterID IS NULL
	BEGIN
		RETURN 1
	END

	-- ����ѯ���
	DECLARE @QueryUserID INT
	DECLARE @MemberRight INT
	SELECT @QueryUserID=UserID,@MemberRight=MemberRight FROM IMGroupMember WHERE GroupID = @dwGroupID AND UserID=@dwQueryID
	-- ��ѯ��Ҳ����ڸô�����
	IF @QueryUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��ѯ��Ҳ����ڸô����ˣ�'
		RETURN 2
	END
	
	--������ѯ
	IF @dwQueryID=@CreaterID
	BEGIN
		SELECT top 30 * FROM RecordGroupScorePresent WHERE GroupID = @dwGroupID  Order By CollectDate Desc
	END
	--��������ǹ���Ա
	ELSE IF (@MemberRight&2) <> 0
	BEGIN
		SELECT top 30 * FROM RecordGroupScorePresent WHERE GroupID = @dwGroupID Order By CollectDate Desc
	END
	--��������Ƕӳ�
	ELSE IF (@MemberRight&4) <> 0
	BEGIN
		SELECT top 30 * FROM RecordGroupScorePresent WHERE GroupID = @dwGroupID AND ( ReceiveUserID in (select UserID from IMGroupMember where GroupID=@dwGroupID AND (CaptainID=@QueryUserID OR UserID=@dwQueryID)) OR PresentUserID=@dwQueryID )Order By CollectDate Desc
	END
	ELSE--��Ա��ѯ
	BEGIN
		SELECT top 30 * FROM RecordGroupScorePresent WHERE GroupID = @dwGroupID AND  ReceiveUserID=@dwQueryID Order By CollectDate Desc
	END

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------


-- �۷���Ϣ��ѯ
CREATE  PROCEDURE dbo.GSP_IM_QueryRevenue
	@dwGroupID	INT,			-- Ⱥ���ʶ	
	@dwQueryID	INT,			-- ��ѯ��ʶ
	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	-- ��ѯ������
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- �����˲�����
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�ô�������Ϣ�����ڣ�'
		RETURN 1
	END
	
	DECLARE @nRight  INT
	SELECT @nRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID
		
	IF @CreaterID = @dwQueryID OR (@nRight&2) <> 0
	BEGIN
		SELECT * FROM RecordGroupScoreRevenue WHERE GroupID = @dwGroupID AND  CollectDate>=DateAdd(d,-3,getdate()) Order By CollectDate Desc
	END
	ELSE--��Ա��ѯ
	BEGIN
		SET @strErrorDescribe=N'ֻ�������͹���Ա���ܲ�ѯ��'
		RETURN 4
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- �۷���Ϣ��ѯ
CREATE  PROCEDURE dbo.GSP_IM_SetFlag
	@dwGroupID		INT,			-- Ⱥ���ʶ	
	@dwOperateID	INT,			-- ������ұ�ʶ
	@dwTargetID		INT,			-- Ŀ����ұ�ʶ
	@dwDateID		INT,			-- ����ID
	@cbFlag			TINYINT,		-- ���
	-- �������	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	-- ��ѯ������
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- �����˲�����
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫ�����Ĵ�������Ϣ�����ڣ����֤���ٴγ��ԣ�'		
		RETURN 1
	END

	IF NOT EXISTS(Select 1 FROM RecordUserDayInfo WHERE GroupID=@dwGroupID AND UserID=@dwTargetID AND DateID=@dwDateID)
	BEGIN
		SET @strErrorDescribe=N'��Ҫ�����ļ�¼��Ϣ�����ڣ����֤���ٴγ��ԣ�'		
		RETURN 2
	END

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF @CreaterID = @dwOperateID
		SET @HaveRight=1
	ELSE
	BEGIN
		DECLARE @UserRight BIGINT
		SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperateID
		IF @UserRight IS NULL
		BEGIN
			SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ���ã�'
			RETURN 2	
		END

		IF (@UserRight&2) <> 0--����Ա
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperateID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x80)<>0 
				SET @HaveRight=1
		END

		IF (@UserRight&4) <> 0--�ӳ�
		BEGIN
			IF @dwOperateID=@dwTargetID
			BEGIN
				SET @strErrorDescribe=N'���ܸ��Լ����ޣ�'
				RETURN 3	
			END

			IF @dwOperateID<>(Select CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwTargetID)
			BEGIN
				SET @strErrorDescribe=N'����Ҳ�������������Ա��'
				RETURN 3	
			END

			DECLARE @CaptainRight BIGINT
			SELECT @CaptainRight = CaptainRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@CaptainRight&0x08)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight=0
	BEGIN
		SET @strErrorDescribe=N'����ʧ�ܣ���Ȩ���ã�'
		RETURN 3		
	END

	UPDATE RecordUserDayInfo SET Flag=@cbFlag WHERE GroupID=@dwGroupID AND UserID=@dwTargetID AND DateID=@dwDateID

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------


-- ѫ������
CREATE  PROCEDURE dbo.GSP_IM_ClearMedal
	@dwGroupID		INT,			-- Ⱥ���ʶ	
	@dwDateID		INT
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	declare @inserttime datetime
	set @inserttime=GETDATE()

	--declare @dateID int
	--Set @dateID=CONVERT(int,CONVERT(nvarchar(8),@inserttime,112))--��ʽ20190715   

    declare @temp bigint
    --�����α�
    declare order_cursor cursor 
    for (select [UserID] from IMGroupMember WHERE GroupID=@dwGroupID)
    --���α�--
    open order_cursor
    --��ʼѭ���α����--
    fetch next from order_cursor into @temp

    while @@FETCH_STATUS = 0    --���ر� FETCH���ִ�е�����α��״̬--
    begin 
		-- �����ռ�¼
	  	declare @CurScore bigint
		select @CurScore=Score from IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@temp

		IF NOT Exists(select 1 from   WHQJGroupDB.dbo.RecordUserDayInfo where DateID=@dwDateID AND UserID=@temp AND GroupID=@dwGroupID)
		BEGIN
			declare @szUserName nvarchar(31)
			select @szUserName=NickName from WHQJAccountsDB.dbo.AccountsInfo where UserID=@temp



			insert   WHQJGroupDB.dbo.RecordUserDayInfo (DateID,GroupID,UserID,UserName,CurScore,CreateCount,DrawCount,BigWinnerCount,ScoreCount,RevenueCount,ContributionValue,Flag,InsertTime)
			 values(@dwDateID,@dwGroupID,@temp,@szUserName,@curScore,0,0,0,0,0,0,0,@inserttime)
		END
		ELSE
		BEGIN
			update   WHQJGroupDB.dbo.RecordUserDayInfo SET CurScore=@CurScore where DateID=@dwDateID AND UserID=@temp AND GroupID=@dwGroupID
		END

        update IMGroupMember set Score=0 where UserID=@temp AND GroupID=@dwGroupID
		update IMGroupWealth set Score=Score+@CurScore where GroupID=@dwGroupID
        fetch next from order_cursor into @temp   --ת����һ���α꣬û�л���ѭ��
    end    

    close order_cursor  --�ر��α�
    deallocate order_cursor   --�ͷ��α�

END

RETURN 0

GO