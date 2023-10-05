
USE WHQJGroupDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryCaptainList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryCaptainList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_Add_Teamer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_Add_Teamer]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_Add_TeamerEx]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_Add_TeamerEx]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetCaptainPoint]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_IM_SetCaptainPoint
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryTeamerRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_IM_QueryTeamerRecord
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryTeamDayRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_IM_QueryTeamDayRecord
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_OpenBox]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_IM_OpenBox
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_LoadBoxRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_IM_LoadBoxRecord
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_InvitationCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_IM_InvitationCode
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_BlindInvitationCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_IM_BlindInvitationCode
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_GetAwardScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_IM_GetAwardScore
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_CheckNewUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_IM_CheckNewUser
GO
----------------------------------------------------------------------------------------------------
--ͳ�ƾ��ֲ��ӳ��ռ�¼
CREATE  PROCEDURE dbo.[GSP_IM_QueryCaptainList]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwUserID		BIGINT,			--��ұ�ʶ	
	@dwCaptainID	BIGINT,			--�ӳ�ID
	@dwDateID		BIGINT,			--����ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
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
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- ��ѯ������
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- �����˲�����
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫ�����Ĵ�������Ϣ�����ڣ����֤���ٴγ��ԣ�'		
		RETURN 5
	END


	-- ��ѯȨ��
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

			IF (@ManagerRight&0x100)<>0 
				SET @HaveRight=1
		END
		ELSE IF (@UserRight&4)<>0--�ӳ�
		BEGIN
			IF @dwCaptainID=@dwUserID
				SET @HaveRight=1
			ELSE
			BEGIN
				DECLARE @dwLeaderID INT
				SELECT @dwLeaderID=CaptainID FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID
				WHILE @dwLeaderID IS NOT NULL
				BEGIN

					IF @dwLeaderID=@dwUserID
					BEGIN
						SET @HaveRight=1
						BREAK
					END

					SELECT @dwLeaderID=CaptainID FROM   WHQJGroupDB.dbo.IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwLeaderID
					IF @dwLeaderID=0
						BREAK
				END
			END
		END
			
	END


	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ�޽��д˲�����'
		RETURN 6		
	END

	--�����ֻ�ܲ���м�¼�Ķӳ�
	--SELECT A.CaptainID,A.DrawCount,A.BigWinnerCount,A.RevenueCount,A.ContributionValue,A.Flag,C.GameID,C.NickName FROM RecordTeamDayInfo A,IMGroupMember B ,WHQJAccountsDB.dbo.AccountsInfo C
	--WHERE A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.CaptainID=B.UserID AND B.CaptainID=@dwCaptainID AND B.UserID=C.UserID

			--������ʱ��
	CREATE TABLE #temptable
	(
		[UserID] [bigint],
		[GameID] [bigint],
		[NickName] nvarchar(32),
		[DrawCount] [int] ,
		[BigWinnerCount] [int] ,
		[RevenueCount] bigint ,
		[ContributionValue] bigint ,
		[Flag] tinyint
	)

	declare @temp bigint

    declare order_cursor cursor 
    for (select [UserID] from IMGroupMember WHERE GroupID=@dwGroupID  AND CaptainID=@dwCaptainID AND (MemberRight&4)<>0)--ɸѡ�öӳ��������ӳ�

    open order_cursor
    fetch next from order_cursor into @temp

    while @@FETCH_STATUS = 0 
    begin 

		insert into #temptable SELECT @temp,C.GameID,C.NickName,0,0,0,0,0 FROM WHQJAccountsDB.dbo.AccountsInfo C
		WHERE C.UserID=@temp

		--update #temptable SET DrawCount=A.DrawCount,BigWinnerCount=A.BigWinnerCount,RevenueCount=A.RevenueCount,ContributionValue=A.ContributionValue,Flag=A.Flag FROM RecordTeamDayInfo A
		--WHERE A.DateID=@dwDateID AND #temptable.UserID=A.CaptainID
		--С��û�ж����ĵ��ޣ���ӳ����ռ�¼����
		update #temptable SET DrawCount=A.DrawCount,BigWinnerCount=A.BigWinnerCount,RevenueCount=A.RevenueCount,ContributionValue=A.ContributionValue,Flag=B.Flag FROM RecordTeamDayInfo A,RecordUserDayInfo B
		WHERE A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND #temptable.UserID=A.CaptainID AND B.UserID=A.CaptainID AND B.DateID=@dwDateID AND B.GroupID=@dwGroupID

        fetch next from order_cursor into @temp 
    end    

	close order_cursor
    deallocate order_cursor

	select * from #temptable

	drop table #temptable

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
--�����Ա
CREATE  PROCEDURE dbo.[GSP_IM_Add_Teamer]
	@dwGroupID BIGINT,							-- Ⱥ���ʶ
	@dwOperatorID INT,							-- ������ID		
	@dwCaptainID  INT,							-- �ӳ���ʶ
	--@dwMemberID   INT,							-- ��Ա��ʶ
	@cbMode   TINYINT,							--ģʽ��0�Ƴ���1����
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
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
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwOperatorID

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

	-- ��ѯ����
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty WHERE GroupID=@dwGroupID
	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ�����ڣ����֤���ٴγ��ԣ�'
		RETURN 4		
	END

	DECLARE @UserRight BIGINT
	SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperatorID
	IF @UserRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ���ã�'
		RETURN 5	
	END

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF (@UserRight&1) <> 0--����
		SET @HaveRight=1
	ELSE
	BEGIN

		IF (@UserRight&2) <> 0--����Ա
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperatorID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x1000)<>0 
				SET @HaveRight=1
		END
		ELSE IF (@UserRight&4)<>0--�ӳ�
		BEGIN
			DECLARE @CaptainRight BIGINT
			SELECT @CaptainRight = CaptainRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
			IF (@CaptainRight&0x10)<>0 AND @cbMode=0
				SET @HaveRight=1
			ELSE IF (@CaptainRight&0x20)<>0 AND @cbMode=1
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ�޽��д˲�����'
		RETURN 6		
	END

	IF NOT EXISTS(SELECT 1 FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID AND (MemberRight&0x04)<>0)
	BEGIN
		SET @strErrorDescribe=N'��Ǹ���ӳ���Ϣ�����ڣ�'
		RETURN 7	
	END

	--IF @cbMode=0--�Ƴ�
	--BEGIN
	--	IF NOT EXISTS(SELECT 1 FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=@dwCaptainID)
	--	BEGIN
	--		SET @strErrorDescribe=N'�Ƴ�ʧ��,����Ҳ��Ƕ�Ա��'
	--		RETURN 8	
	--	END

	--	UPDATE IMGroupMember SET CaptainID=0,MemberRight=0 WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=@dwCaptainID
	--END
	--ELSE IF @cbMode=1--���
	--BEGIN
	--	DECLARE @dwMemberRight INT
	--	SELECT @dwMemberRight=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=0
	--	IF @dwMemberRight IS NULL
	--	BEGIN
	--		SET @strErrorDescribe=N'����ʧ��,��������жӳ���'
	--		RETURN 9	
	--	END

	--	IF @dwMemberRight<>0
	--	BEGIN
	--		SET @strErrorDescribe=N'����ʧ��,ֻ����Ӵ�������ͨ��ԱΪ��Ա��'
	--		RETURN 9	
	--	END

	--	UPDATE IMGroupMember SET CaptainID=@dwCaptainID,MemberRight=8 WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	--END

END

RETURN 0

GO
---------------------------------------------------------------------------------------------------


--�����Ա
CREATE  PROCEDURE dbo.[GSP_IM_Add_TeamerEx]
	@dwGroupID BIGINT,							-- Ⱥ���ʶ	
	@dwCaptainID  INT,							-- �ӳ���ʶ
	@dwMemberID   INT,							-- ��Ա��ʶ
	@cbMode   TINYINT							--ģʽ��0�Ƴ���1����
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	IF @cbMode=0--�Ƴ�
	BEGIN
		IF NOT EXISTS(SELECT 1 FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=@dwCaptainID)
		BEGIN
			RETURN 1	
		END

		UPDATE IMGroupMember SET CaptainID=0,MemberRight=0 WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=@dwCaptainID
	END
	ELSE IF @cbMode=1--���
	BEGIN
		DECLARE @dwMemberRight INT
		SELECT @dwMemberRight=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=0
		IF @dwMemberRight IS NULL
		BEGIN
			RETURN 2	
		END

		IF @dwMemberRight<>0
		BEGIN
			RETURN 3	
		END

		UPDATE IMGroupMember SET CaptainID=@dwCaptainID,MemberRight=8 WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	END

END

RETURN 0

GO
---------------------------------------------------------------------------------------------------

--ͳ�ƾ��ֲ���Ա�ռ�¼
CREATE  PROCEDURE dbo.[GSP_IM_QueryTeamerRecord]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwUserID		BIGINT,			--��ұ�ʶ	
	@dwCaptainID	BIGINT,			--�ӳ�ID
	@dwDateID		BIGINT,			--����ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
 AS

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

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
		RETURN 5
	END

	-- ��ѯȨ��
	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF @CreaterID = @dwUserID OR @dwUserID=@dwCaptainID
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

			IF (@ManagerRight&0x100)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ�޽��д˲�����'
		RETURN 6		
	END

	--�����ֻ�ܲ���м�¼�Ķ�Ա
	--SELECT A.UserID,A.DrawCount,A.BigWinnerCount,A.RevenueCount,A.ContributionValue, A.Flag,C.GameID,C.NickName FROM RecordUserDayInfo A,IMGroupMember B  ,WHQJAccountsDB.dbo.AccountsInfo C
	--WHERE A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=B.UserID AND ( (B.CaptainID=@dwCaptainID AND (MemberRight&4)=0 ) OR  B.UserID=@dwCaptainID) AND A.UserID=C.UserID

	--������ʱ��
	CREATE TABLE #temptable
	(
		[UserID] [bigint],
		[GameID] [bigint],
		[NickName] nvarchar(32),
		[DrawCount] [int] ,
		[BigWinnerCount] [int] ,
		[RevenueCount] bigint ,
		[ContributionValue] bigint ,
		[Flag] tinyint
	)

	declare @temp bigint

    declare order_cursor cursor 
    for (select [UserID] from IMGroupMember WHERE GroupID=@dwGroupID  AND ( (CaptainID=@dwCaptainID AND (MemberRight&4)=0 ) OR UserID=@dwCaptainID))--ɸѡ�öӳ���������Ա�������öӳ�

    open order_cursor
    fetch next from order_cursor into @temp

    while @@FETCH_STATUS = 0 
    begin 

		insert into #temptable SELECT @temp,C.GameID,C.NickName,0,0,0,0,0 FROM WHQJAccountsDB.dbo.AccountsInfo C
		WHERE C.UserID=@temp

		update #temptable SET DrawCount=A.DrawCount,BigWinnerCount=A.BigWinnerCount,RevenueCount=A.RevenueCount,ContributionValue=A.ContributionValue,Flag=A.Flag FROM RecordUserDayInfo A
		WHERE A.DateID=@dwDateID AND #temptable.UserID=A.UserID AND A.GroupID=@dwGroupID

        fetch next from order_cursor into @temp 
    end    

	close order_cursor
    deallocate order_cursor

	select * from #temptable

	drop table #temptable
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
--С����ͳ��
CREATE  PROCEDURE dbo.[GSP_IM_QueryTeamDayRecord]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwUserID		BIGINT,			--��ұ�ʶ	
	@dwDateID		BIGINT,			--����ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
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
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- ��ѯ������
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- �����˲�����
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫ�����Ĵ�������Ϣ�����ڣ����֤���ٴγ��ԣ�'		
		RETURN 5
	END

	-- ��ѯȨ��
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

			IF (@ManagerRight&0x100)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ�޽��д˲�����'
		RETURN 6		
	END

	--������ʱ��
	CREATE TABLE #temptable
	(
		[CaptainID] [bigint],
		[DrawCount] [int] ,
		[BigWinnerCount] [int] ,
		[CurScore] bigint ,
		[ScoreCount] bigint ,
		[RevenueCount] bigint
	)
	/*
	--��һ�ַ���
	insert #temptable(CaptainID)  select A.UserID from IMGroupMember A WHERE GroupID=@dwGroupID AND (A.MemberRight&0x04)<>0

	update #temptable set DrawCount= (select sum(A.DrawCount) From RecordUserDayInfo A,IMGroupMember B
	where A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=B.UserID AND (B.CaptainID=#temptable.CaptainID OR ((B.MemberRight&0x04)<>0 AND B.UserID=#temptable.CaptainID)))

	update #temptable set BigWinnerCount= (select sum(A.BigWinnerCount) From RecordUserDayInfo A,IMGroupMember B
	where A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=B.UserID AND (B.CaptainID=#temptable.CaptainID OR ((B.MemberRight&0x04)<>0 AND B.UserID=#temptable.CaptainID)))

	update #temptable set CurScore= (select sum(A.CurScore) From RecordUserDayInfo A,IMGroupMember B
	where A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=B.UserID AND (B.CaptainID=#temptable.CaptainID OR ((B.MemberRight&0x04)<>0 AND B.UserID=#temptable.CaptainID)))

	update #temptable set GameScoreCount= (select sum(A.ScoreCount) From RecordUserDayInfo A,IMGroupMember B
	where A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=B.UserID AND (B.CaptainID=#temptable.CaptainID OR ((B.MemberRight&0x04)<>0 AND B.UserID=#temptable.CaptainID)))

	update #temptable set RevenueCount= (select sum(A.RevenueCount) From RecordUserDayInfo A,IMGroupMember B
	where A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=B.UserID AND (B.CaptainID=#temptable.CaptainID OR ((B.MemberRight&0x04)<>0 AND B.UserID=#temptable.CaptainID)))
	*/

	--�ڶ��ַ���
	declare @temp bigint
    --�����α�
    declare order_cursor cursor 
    for (select [UserID] from IMGroupMember WHERE GroupID=@dwGroupID  AND (MemberRight&0x04)<>0)
    --���α�--
    open order_cursor
    --��ʼѭ���α����--
    fetch next from order_cursor into @temp

    while @@FETCH_STATUS = 0    --���ر� FETCH���ִ�е�����α��״̬--
    begin 

		insert into   #temptable select @temp, sum(A.DrawCount),sum(A.BigWinnerCount),sum(A.CurScore),sum(A.ScoreCount),sum(A.RevenueCount) 
		From RecordUserDayInfo A ,IMGroupMember B
		where A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=B.UserID AND (B.CaptainID=@temp OR ((B.MemberRight&0x04)<>0 AND B.UserID=@temp))

        fetch next from order_cursor into @temp   --ת����һ���α꣬û�л���ѭ��
    end    

	close order_cursor  --�ر��α�
    deallocate order_cursor   --�ͷ��α�

	select * from #temptable

	drop table #temptable
END

RETURN 0

GO

----------------------------------------------------------------------------------

-- ���öӳ���λ
CREATE  PROCEDURE dbo.GSP_IM_SetCaptainPoint
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@dwCaptainID	INT,			-- �ӳ�ID
	@wAwardPoint	INT,			-- ��λ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
--WITH ENCRYPTION AS
AS
-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	DECLARE @UserRight BIGINT 
	DECLARE @MaxPoint INT
	DECLARE @OldPoint INT
	SELECT @UserRight=MemberRight,@OldPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID
	IF @UserRight IS NULL OR (@UserRight&0x04)=0
	BEGIN
		SET @strErrorDescribe=N'�ӳ���Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
		RETURN 1	
	END

	SELECT @UserRight=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @UserRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ���ã�'
		RETURN 2	
	END

	IF (@UserRight&1)<>0 OR (@UserRight&2)<>0--���������Ա
	BEGIN
		DECLARE @CreatorPoint SMALLINT
		SELECT @CreatorPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND MemberRight=1 
		IF @CreatorPoint IS NULL OR @CreatorPoint<0 OR @CreatorPoint>100
		BEGIN
			SET @strErrorDescribe=N'����ϵ��Ӫ����Ȧ����λ��'
			RETURN 3
		END

		
		IF @OldPoint> @wAwardPoint
		BEGIN
			SET @strErrorDescribe=N'������Ч����λֻ�����Ӳ��ܼ��٣�'
			RETURN 4	
		END

		IF @wAwardPoint> @CreatorPoint
		BEGIN
			SET @strErrorDescribe=N'һ���ӳ���λ����С��������λ�����������ã�'
			RETURN 5	
		END
	END
	ELSE
	BEGIN
		IF @dwUserID<>(SELECT CaptainID FROM IMGroupMember WHERE UserID=@dwCaptainID AND GroupID=@dwGroupID)
		BEGIN
			SET @strErrorDescribe=N'�����Ǹöӳ����ϼ�����Ȩ���ã�'
			RETURN 6	
		END

		SELECT @MaxPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		IF @wAwardPoint> @MaxPoint
		BEGIN
			SET @strErrorDescribe=N'�¼��ӳ���λ����С���ϼ���λ�����������ã�'
			RETURN 7	
		END

		IF @OldPoint> @wAwardPoint
		BEGIN
			SET @strErrorDescribe=N'������Ч����λֻ�����Ӳ��ܼ��٣�'
			RETURN 8	
		END

	END


	UPDATE IMGroupMember SET AwardPoint=@wAwardPoint WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID

END

RETURN 0

GO

--------------------------------------------------------------

--��������Ϣ
CREATE  PROCEDURE dbo.[GSP_IM_OpenBox]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwUserID		BIGINT,			--��ұ�ʶ	
	@szPwd			nvarchar(32),	--����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
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

	-- �����ж�
	IF @LogonPass<>@szPwd
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 4
	END

	DECLARE @lAwardScore BIGINT
	DECLARE @nAwardPoint INT
	DECLARE @UserRight BIGINT
	SELECT @UserRight=MemberRight, @lAwardScore = AwardScore,@nAwardPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @UserRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ�������֤���ٴγ��ԣ�'
		RETURN 5
	END

	SELECT @nAwardPoint AS AwardPoint,@lAwardScore AS AwardScore

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------


--------------------------------------------------------------

--��������ȡ��¼
CREATE  PROCEDURE dbo.[GSP_IM_LoadBoxRecord]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwUserID		BIGINT			--��ұ�ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	SELECT BeforeScore,Score,AfterScore,CollectTime FROM RecordBoxScore WHERE GroupID=@dwGroupID AND UserID=@dwUserID ORDER BY CollectTime DESC
END

RETURN 0

GO

-----------------------------------------------------
--��ѯ������
CREATE  PROCEDURE dbo.[GSP_IM_InvitationCode]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwUserID		BIGINT,			--��ұ�ʶ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	DECLARE @lInvitationCode BIGINT	
	SELECT @lInvitationCode=InvitationCode FROM IMGroupInvitationCode WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	IF @lInvitationCode IS NULL
	BEGIN
		SET @strErrorDescribe=N'��ѯʧ�ܣ�����ϵ�ͷ���'
		RETURN 2
	END

	SELECT @lInvitationCode AS InvitationCode

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

--��������
CREATE  PROCEDURE dbo.[GSP_IM_BlindInvitationCode]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwUserID		BIGINT,			--��ұ�ʶ	
	@lCode			BIGINT,			--������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	DECLARE @GroupID BIGINT
	DECLARE @SpreadID BIGINT
	DECLARE @UserRight INT

	SELECT @GroupID=GroupID,@SpreadID=UserID FROM IMGroupInvitationCode WHERE InvitationCode=@lCode
	IF @GroupID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����벻���ڣ����֤���ٴγ��ԣ�'
		RETURN 1
	END

	IF @GroupID<>@dwGroupID
	BEGIN
		SET @strErrorDescribe=N'�������벻���ڵ�ǰ�����ˣ����֤���ٴγ��ԣ�'
		RETURN 2
	END

	--��ѯ�����Ϣ
	SELECT @UserRight=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @UserRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ�������֤���ٴγ��ԣ�'
		RETURN 3
	END

	IF @UserRight<>0
	BEGIN
		SET @strErrorDescribe=N'ֻ����ͨ��Ա���ܰ������룡'
		RETURN 4
	END

	--��ѯ�ƹ�����Ϣ
	SELECT @UserRight=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@SpreadID
	IF @UserRight IS NULL OR (@UserRight&4)=0
	BEGIN
		SET @strErrorDescribe=N'ֻ�ܰ󶨶ӳ��������룡'
		RETURN 5
	END

	UPDATE IMGroupMember SET MemberRight=8,CaptainID=@SpreadID WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	SELECT @SpreadID AS CaptainID


END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

--��ȡ����
CREATE  PROCEDURE dbo.[GSP_IM_GetAwardScore]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwUserID		BIGINT,			--��ұ�ʶ	
	@lScore			BIGINT,			--����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	IF @lScore<=0
	BEGIN
		SET @strErrorDescribe=N'��ȡ���ֱ������0��'
		RETURN 1
	END

	DECLARE @AwardScore BIGINT
	SELECT @AwardScore=AwardScore FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @AwardScore IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����Ϣ�����ڣ����֤���ٴγ��ԣ�'
		RETURN 2
	END

	IF @lScore>@AwardScore
	BEGIN
		SET @strErrorDescribe=N'���ֲ��㣡'
		RETURN 3
	END

	UPDATE IMGroupMember SET AwardScore=AwardScore-@lScore WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	INSERT INTO RecordBoxScore(GroupID,UserID,BeforeScore,Score,AfterScore,CollectTime) VALUES(@dwGroupID,@dwUserID,@AwardScore,@lScore,@AwardScore-@lScore,GETDATE())
	
	DECLARE @CurScore BIGINT
	SELECT @CurScore=Score FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	UPDATE IMGroupMember SET Score=Score+@lScore WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	INSERT INTO RecordUserScoreChange(GroupID,UserID,BeforeScore,ChangeType,ChangeScore,AfterScore,CollectDate) 
	VALUES(@dwGroupID,@dwUserID,@CurScore,5,@lScore,@CurScore+@lScore,GETDATE())

	--SELECT AwardPoint,AwardScore FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	DECLARE @lAwardScore BIGINT
	DECLARE @nAwardPoint INT
	DECLARE @UserRight BIGINT
	SELECT @UserRight=MemberRight, @lAwardScore = AwardScore,@nAwardPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	SELECT @nAwardPoint AS AwardPoint,@lAwardScore AS AwardScore
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

--��ȡ����
CREATE  PROCEDURE dbo.[GSP_IM_CheckNewUser]
	@dwUserID		BIGINT,			--��ұ�ʶ	
	@szClientIP		NVARCHAR(15)	--IP
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	SELECT GroupID,InvitationCode FROM IMGroupBindIP WHERE UserIp=@szClientIP
	delete  FROM IMGroupBindIP WHERE UserIp=@szClientIP
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------