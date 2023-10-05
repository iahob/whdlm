
USE WHQJGroupDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryDayRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryDayRecord]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryUserDayRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryUserDayRecord]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryUserDayDetail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryUserDayDetail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryGroupDayDetail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryGroupDayDetail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryRoomDetail]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryRoomDetail]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryRoomDetailEx]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryRoomDetailEx]
GO
----------------------------------------------------------------------------------------------------
--ͳ�ƾ��ֲ�����ռ�¼
CREATE  PROCEDURE dbo.[GSP_IM_QueryDayRecord]
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

	-- ��ѯ������
	DECLARE @MemberRight  INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	IF @MemberRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵ĳ�Ա��û��Ȩ�޽��д˲�����'
		RETURN 6
	END


	--SELECT UserID,DrawCount,BigWinnerCount,ScoreCount,RevenueCount,Flag,CurScore FROM RecordUserDayInfo WHERE DateID=@dwDateID AND GroupID=@dwGroupID

				--������ʱ��
	CREATE TABLE #temptable
	(
		[UserID] [bigint],
		[GameID] [bigint],
		[NickName] nvarchar(32),
		[DrawCount] [int] ,
		[BigWinnerCount] [int] ,
		[ScoreCount] bigint,
		[RevenueCount] bigint ,
		[CurScore] bigint ,
		[Flag] tinyint
	)

	declare @temp bigint

    declare order_cursor cursor 
    for (select [UserID] from IMGroupMember WHERE GroupID=@dwGroupID)

    open order_cursor
    fetch next from order_cursor into @temp

    while @@FETCH_STATUS = 0 
    begin 

		insert into #temptable SELECT C.UserID,C.GameID,C.NickName,0,0,0,0,0,0 FROM WHQJAccountsDB.dbo.AccountsInfo C	WHERE C.UserID=@temp

		update #temptable set DrawCount=A.DrawCount,BigWinnerCount=A.BigWinnerCount,ScoreCount=A.ScoreCount,RevenueCount=A.RevenueCount,Flag=A.Flag,CurScore=A.CurScore
		FROM RecordUserDayInfo A WHERE A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=@temp AND #temptable.UserID=A.UserID


        fetch next from order_cursor into @temp 
    end    

	close order_cursor
    deallocate order_cursor

	select * from #temptable

	drop table #temptable
END

RETURN 0

GO
---------------------------------------------------------------------------------------------------
--��ѯ���ֲ���������ռ�¼
CREATE  PROCEDURE dbo.[GSP_IM_QueryUserDayRecord]
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

	-- ��ѯ������
	DECLARE @MemberRight  INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	IF @MemberRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵ĳ�Ա��û��Ȩ�޽��д˲�����'
		RETURN 6
	END

	--������ʱ��
	CREATE TABLE #temptable
	(
		[UserID] [bigint],
		[GameID] [bigint],
		[NickName] nvarchar(32),
		[DrawCount] [int] ,
		[BigWinnerCount] [int] ,
		[ScoreCount] bigint ,
		[RevenueCount] bigint ,
		[CurScore] bigint,
		[Flag] tinyint
	)

	insert into #temptable SELECT C.UserID,C.GameID,C.NickName,0,0,0,0,0,0 FROM WHQJAccountsDB.dbo.AccountsInfo C	WHERE C.UserID=@dwUserID

	update #temptable set DrawCount=A.DrawCount,BigWinnerCount=A.BigWinnerCount,ScoreCount=A.ScoreCount,RevenueCount=A.RevenueCount,Flag=A.Flag,CurScore=A.CurScore
	FROM RecordUserDayInfo A WHERE A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=@dwUserID

	select * from #temptable

	drop table #temptable
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
--ͳ�ƾ־��ֲ�һ�췿����Ϣ
CREATE  PROCEDURE dbo.[GSP_IM_QueryGroupDayDetail]
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

	-- ��ѯ������
	DECLARE @MemberID  INT
	DECLARE @UserRight INT
	SELECT @UserRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	-- ���ǳ�Ա
	IF @UserRight IS NULL 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵ĳ�Ա��û��Ȩ�޽��д˲�����'
		RETURN 6
	END

	IF (@UserRight&3)=0--���������Ա���ܲ�ѯ
		BEGIN
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ�޽��д˲�����'
		RETURN 7
	END

	--������ʱ��
	CREATE TABLE #temptable
	(
		[RoomID] [bigint] NOT NULL,
		[PlayMode] [tinyint] NOT NULL,
		[KindName] [nvarchar](31) NOT NULL,
		[DrawCount] [int] NOT NULL,
		[BigWinnerID] [int] NOT NULL,
		[BigWinnerName] [nvarchar](31) NOT NULL,
		[Diamond] bigint default 0,
		[RoomRevenue] bigint default 0,
		[EndTime] [datetime] NOT NULL,
	)
	insert #temptable(RoomID,PlayMode,KindName,DrawCount,BigWinnerID,BigWinnerName,RoomRevenue,EndTime,Diamond)  select A.RoomID,A.PlayMode,A.KindName,A.DrawCount,A.BigWinnerID,A.BigWinnerName ,A.RoomRevenue, A.EndTime,B.NeedRoomCard
	from RecordGroupRoomInfo A,WHQJPlatformDB.dbo.StreamCreateTableFeeInfo B
	where CONVERT(int,convert(nvarchar(8),A.EndTime,112))=@dwDateID AND A.GroupID=@dwGroupID AND A.RoomID=B.RoomID AND B.GroupID=A.GroupID AND B.CreateDate>A.StartTime AND B.CreateDate < A.EndTime

	select * from #temptable ORDER BY EndTime DESC

	drop table #temptable
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
--ͳ�����һ��Ծ�����
CREATE  PROCEDURE dbo.[GSP_IM_QueryUserDayDetail]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwUserID		BIGINT,			--��ұ�ʶ	
	@dwTargetUserID BIGINT,			--Ŀ�����
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

	-- ��ѯ������
	DECLARE @MemberID  INT
	DECLARE @UserRight INT
	SELECT @UserRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	-- ���ǳ�Ա
	IF @UserRight IS NULL 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵ĳ�Ա��û��Ȩ�޽��д˲�����'
		RETURN 6
	END
	
	DECLARE @HaveRight INT
	SET @HaveRight=0
	IF @dwUserID=@dwTargetUserID OR (@UserRight&3)<>0
		SET @HaveRight=1
	ELSE IF @dwUserID=(SELECT CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwTargetUserID)
		SET @HaveRight=1
	IF @HaveRight=0--���������Ա/�ӳ����ܲ�ѯ������Ա
		BEGIN
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ�޽��д˲�����'
		RETURN 7
	END


	--������ʱ��
	CREATE TABLE #temptable
	(
		[RoomID] [bigint] NOT NULL,
		[PlayMode] [tinyint] NOT NULL,
		[KindName] [nvarchar](31) NOT NULL,
		[DrawCount] [int] NOT NULL,
		[BigWinnerID] [int] NOT NULL,
		[BigWinnerName] [nvarchar](31) NOT NULL,
		[Score] bigint default 0,
		[StartTime] [datetime] NOT NULL,
		[EndTime] [datetime] NOT NULL,
	)
	insert #temptable(RoomID,PlayMode,KindName,DrawCount,BigWinnerID,BigWinnerName,StartTime,EndTime)  select A.RoomID,A.PlayMode,A.KindName,A.DrawCount,A.BigWinnerID,A.BigWinnerName,A.StartTime,A.EndTime from RecordGroupRoomInfo A
	where CONVERT(int,convert(nvarchar(8),A.EndTime,112))=@dwDateID AND A.GroupID=@dwGroupID AND A.RoomID in ( select distinct RoomID from RecordGroupDrawScore where UserID=@dwTargetUserID ) 

	update #temptable set Score=( select sum(A.Score) From [RecordGroupDrawScore] A,[RecordGroupDrawInfo] B where A.UserID=@dwTargetUserID and A.RoomID=#temptable.RoomID and A.RoomID=B.RoomID AND B.GroupID=@dwGroupID AND A.DrawID=B.DrawID and B.StartTime>=#temptable.StartTime and B.ConcludeTime<=#temptable.EndTime)
	select * from #temptable where Score is not null ORDER BY EndTime desc

	drop table #temptable
END

RETURN 0

GO

------------------------------------------------------------------------------------------
--��ѯĳ������Ծ�����
CREATE  PROCEDURE dbo.[GSP_IM_QueryRoomDetail]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwUserID		BIGINT,			--��ұ�ʶ	
	@dwDateID		BIGINT,			--����ID
	@dwRoomID		BIGINT,			--����ID
	@EndTime		DateTime,		--����ʱ��
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

	-- ��ѯ������
	DECLARE @MemberID  INT
	DECLARE @UserRight INT
	SELECT @UserRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	-- ���ǳ�Ա
	IF @UserRight IS NULL 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵ĳ�Ա��û��Ȩ�޽��д˲�����'
		RETURN 6
	END

	SELECT A.KindName,A.RoomID,A.DrawCount,A.MinTableScore,A.StartTime FROM [RecordGroupRoomInfo] A where A.RoomID=@dwRoomID AND A.GroupID=@dwGroupID AND ( A.EndTime=@EndTime OR A.KindID=145)
END

RETURN 0

GO


------------------------------------------------------------------------------------------
--��ѯĳ������Ծ�����
CREATE  PROCEDURE dbo.[GSP_IM_QueryRoomDetailEx]
	@dwGroupID		BIGINT,			--Ⱥ���ʶ	
	@dwRoomID		BIGINT,			--����ID
	@dwDateID		BIGINT,			--����ID
	@BeginTime		DateTime,		--����ʱ��
	@EndTime		DateTime		--����ʱ��
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	SELECT A.[DrawID] ,A.[RoomID] ,A.[TurnIndex] ,A.[StartTime]	,B.[UserID],B.UserName,b.ChairID,b.Score
	  FROM [RecordGroupDrawInfo] A,[RecordGroupDrawScore] B where A.GroupID=@dwGroupID AND A.RoomID=@dwRoomID  AND A.DrawID=B.DrawID AND A.StartTime>=@BeginTime AND A.ConcludeTime<=@EndTime order by a.DrawID,B.ChairID
END

RETURN 0

GO