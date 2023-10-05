
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
--统计俱乐部玩家日记录
CREATE  PROCEDURE dbo.[GSP_IM_QueryDayRecord]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@dwDateID		BIGINT,			--日期ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 查询大联盟
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 大联盟不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要操作的大联盟信息不存在，请查证后再次尝试！'		
		RETURN 5
	END

	-- 查询大联盟
	DECLARE @MemberRight  INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	IF @MemberRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的成员，没有权限进行此操作！'
		RETURN 6
	END


	--SELECT UserID,DrawCount,BigWinnerCount,ScoreCount,RevenueCount,Flag,CurScore FROM RecordUserDayInfo WHERE DateID=@dwDateID AND GroupID=@dwGroupID

				--创建临时表
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
--查询俱乐部单个玩家日记录
CREATE  PROCEDURE dbo.[GSP_IM_QueryUserDayRecord]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@dwDateID		BIGINT,			--日期ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 查询大联盟
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 大联盟不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要操作的大联盟信息不存在，请查证后再次尝试！'		
		RETURN 5
	END

	-- 查询大联盟
	DECLARE @MemberRight  INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	IF @MemberRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的成员，没有权限进行此操作！'
		RETURN 6
	END

	--创建临时表
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
--统计局俱乐部一天房间信息
CREATE  PROCEDURE dbo.[GSP_IM_QueryGroupDayDetail]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@dwDateID		BIGINT,			--日期ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 查询大联盟
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 大联盟不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要操作的大联盟信息不存在，请查证后再次尝试！'		
		RETURN 5
	END

	-- 查询大联盟
	DECLARE @MemberID  INT
	DECLARE @UserRight INT
	SELECT @UserRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	-- 不是成员
	IF @UserRight IS NULL 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的成员，没有权限进行此操作！'
		RETURN 6
	END

	IF (@UserRight&3)=0--盟主或管理员才能查询
		BEGIN
		SET @strErrorDescribe=N'抱歉，您没有权限进行此操作！'
		RETURN 7
	END

	--创建临时表
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
--统计玩家一天对局详情
CREATE  PROCEDURE dbo.[GSP_IM_QueryUserDayDetail]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@dwTargetUserID BIGINT,			--目标玩家
	@dwDateID		BIGINT,			--日期ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 查询大联盟
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 大联盟不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要操作的大联盟信息不存在，请查证后再次尝试！'		
		RETURN 5
	END

	-- 查询大联盟
	DECLARE @MemberID  INT
	DECLARE @UserRight INT
	SELECT @UserRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	-- 不是成员
	IF @UserRight IS NULL 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的成员，没有权限进行此操作！'
		RETURN 6
	END
	
	DECLARE @HaveRight INT
	SET @HaveRight=0
	IF @dwUserID=@dwTargetUserID OR (@UserRight&3)<>0
		SET @HaveRight=1
	ELSE IF @dwUserID=(SELECT CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwTargetUserID)
		SET @HaveRight=1
	IF @HaveRight=0--盟主或管理员/队长才能查询其他成员
		BEGIN
		SET @strErrorDescribe=N'抱歉，您没有权限进行此操作！'
		RETURN 7
	END


	--创建临时表
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
--查询某个房间对局详情
CREATE  PROCEDURE dbo.[GSP_IM_QueryRoomDetail]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@dwDateID		BIGINT,			--日期ID
	@dwRoomID		BIGINT,			--房间ID
	@EndTime		DateTime,		--结束时间
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 查询大联盟
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 大联盟不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要操作的大联盟信息不存在，请查证后再次尝试！'		
		RETURN 5
	END

	-- 查询大联盟
	DECLARE @MemberID  INT
	DECLARE @UserRight INT
	SELECT @UserRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	-- 不是成员
	IF @UserRight IS NULL 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的成员，没有权限进行此操作！'
		RETURN 6
	END

	SELECT A.KindName,A.RoomID,A.DrawCount,A.MinTableScore,A.StartTime FROM [RecordGroupRoomInfo] A where A.RoomID=@dwRoomID AND A.GroupID=@dwGroupID AND ( A.EndTime=@EndTime OR A.KindID=145)
END

RETURN 0

GO


------------------------------------------------------------------------------------------
--查询某个房间对局详情
CREATE  PROCEDURE dbo.[GSP_IM_QueryRoomDetailEx]
	@dwGroupID		BIGINT,			--群组标识	
	@dwRoomID		BIGINT,			--房间ID
	@dwDateID		BIGINT,			--日期ID
	@BeginTime		DateTime,		--结束时间
	@EndTime		DateTime		--结束时间
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	SELECT A.[DrawID] ,A.[RoomID] ,A.[TurnIndex] ,A.[StartTime]	,B.[UserID],B.UserName,b.ChairID,b.Score
	  FROM [RecordGroupDrawInfo] A,[RecordGroupDrawScore] B where A.GroupID=@dwGroupID AND A.RoomID=@dwRoomID  AND A.DrawID=B.DrawID AND A.StartTime>=@BeginTime AND A.ConcludeTime<=@EndTime order by a.DrawID,B.ChairID
END

RETURN 0

GO