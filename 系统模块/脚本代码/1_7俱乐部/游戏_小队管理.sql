
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
--统计俱乐部队长日记录
CREATE  PROCEDURE dbo.[GSP_IM_QueryCaptainList]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@dwCaptainID	BIGINT,			--队长ID
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


	-- 查询权限
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
			SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
			RETURN 6	
		END

		IF (@UserRight&2) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x100)<>0 
				SET @HaveRight=1
		END
		ELSE IF (@UserRight&4)<>0--队长
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
		SET @strErrorDescribe=N'抱歉，您没有权限进行此操作！'
		RETURN 6		
	END

	--此语句只能查出有记录的队长
	--SELECT A.CaptainID,A.DrawCount,A.BigWinnerCount,A.RevenueCount,A.ContributionValue,A.Flag,C.GameID,C.NickName FROM RecordTeamDayInfo A,IMGroupMember B ,WHQJAccountsDB.dbo.AccountsInfo C
	--WHERE A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.CaptainID=B.UserID AND B.CaptainID=@dwCaptainID AND B.UserID=C.UserID

			--创建临时表
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
    for (select [UserID] from IMGroupMember WHERE GroupID=@dwGroupID  AND CaptainID=@dwCaptainID AND (MemberRight&4)<>0)--筛选该队长的下属队长

    open order_cursor
    fetch next from order_cursor into @temp

    while @@FETCH_STATUS = 0 
    begin 

		insert into #temptable SELECT @temp,C.GameID,C.NickName,0,0,0,0,0 FROM WHQJAccountsDB.dbo.AccountsInfo C
		WHERE C.UserID=@temp

		--update #temptable SET DrawCount=A.DrawCount,BigWinnerCount=A.BigWinnerCount,RevenueCount=A.RevenueCount,ContributionValue=A.ContributionValue,Flag=A.Flag FROM RecordTeamDayInfo A
		--WHERE A.DateID=@dwDateID AND #temptable.UserID=A.CaptainID
		--小队没有独立的点赞，与队长的日记录关联
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
--调配队员
CREATE  PROCEDURE dbo.[GSP_IM_Add_Teamer]
	@dwGroupID BIGINT,							-- 群组标识
	@dwOperatorID INT,							-- 操作者ID		
	@dwCaptainID  INT,							-- 队长标识
	--@dwMemberID   INT,							-- 成员标识
	@cbMode   TINYINT,							--模式：0移除，1增加
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID	INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT	
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwOperatorID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END	

	-- 帐号关闭
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须到重新开通后才能继续使用！'
		RETURN 3
	END

	-- 查询盟主
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty WHERE GroupID=@dwGroupID
	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息不存在，请查证后再次尝试！'
		RETURN 4		
	END

	DECLARE @UserRight BIGINT
	SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperatorID
	IF @UserRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
		RETURN 5	
	END

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF (@UserRight&1) <> 0--盟主
		SET @HaveRight=1
	ELSE
	BEGIN

		IF (@UserRight&2) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperatorID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x1000)<>0 
				SET @HaveRight=1
		END
		ELSE IF (@UserRight&4)<>0--队长
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
		SET @strErrorDescribe=N'抱歉，您没有权限进行此操作！'
		RETURN 6		
	END

	IF NOT EXISTS(SELECT 1 FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID AND (MemberRight&0x04)<>0)
	BEGIN
		SET @strErrorDescribe=N'抱歉，队长信息不存在！'
		RETURN 7	
	END

	--IF @cbMode=0--移除
	--BEGIN
	--	IF NOT EXISTS(SELECT 1 FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=@dwCaptainID)
	--	BEGIN
	--		SET @strErrorDescribe=N'移除失败,该玩家不是队员！'
	--		RETURN 8	
	--	END

	--	UPDATE IMGroupMember SET CaptainID=0,MemberRight=0 WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=@dwCaptainID
	--END
	--ELSE IF @cbMode=1--添加
	--BEGIN
	--	DECLARE @dwMemberRight INT
	--	SELECT @dwMemberRight=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=0
	--	IF @dwMemberRight IS NULL
	--	BEGIN
	--		SET @strErrorDescribe=N'操作失败,该玩家已有队长！'
	--		RETURN 9	
	--	END

	--	IF @dwMemberRight<>0
	--	BEGIN
	--		SET @strErrorDescribe=N'操作失败,只能添加大联盟普通成员为队员！'
	--		RETURN 9	
	--	END

	--	UPDATE IMGroupMember SET CaptainID=@dwCaptainID,MemberRight=8 WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	--END

END

RETURN 0

GO
---------------------------------------------------------------------------------------------------


--调配队员
CREATE  PROCEDURE dbo.[GSP_IM_Add_TeamerEx]
	@dwGroupID BIGINT,							-- 群组标识	
	@dwCaptainID  INT,							-- 队长标识
	@dwMemberID   INT,							-- 成员标识
	@cbMode   TINYINT							--模式：0移除，1增加
	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	IF @cbMode=0--移除
	BEGIN
		IF NOT EXISTS(SELECT 1 FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=@dwCaptainID)
		BEGIN
			RETURN 1	
		END

		UPDATE IMGroupMember SET CaptainID=0,MemberRight=0 WHERE GroupID=@dwGroupID AND UserID=@dwMemberID AND CaptainID=@dwCaptainID
	END
	ELSE IF @cbMode=1--添加
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

--统计俱乐部队员日记录
CREATE  PROCEDURE dbo.[GSP_IM_QueryTeamerRecord]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@dwCaptainID	BIGINT,			--队长ID
	@dwDateID		BIGINT,			--日期ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
 AS

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询大联盟
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 大联盟不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要操作的大联盟信息不存在，请查证后再次尝试！'		
		RETURN 5
	END

	-- 查询权限
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
			SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
			RETURN 6	
		END

		IF (@UserRight&2) <> 0--管理员
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
		SET @strErrorDescribe=N'抱歉，您没有权限进行此操作！'
		RETURN 6		
	END

	--此语句只能查出有记录的队员
	--SELECT A.UserID,A.DrawCount,A.BigWinnerCount,A.RevenueCount,A.ContributionValue, A.Flag,C.GameID,C.NickName FROM RecordUserDayInfo A,IMGroupMember B  ,WHQJAccountsDB.dbo.AccountsInfo C
	--WHERE A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=B.UserID AND ( (B.CaptainID=@dwCaptainID AND (MemberRight&4)=0 ) OR  B.UserID=@dwCaptainID) AND A.UserID=C.UserID

	--创建临时表
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
    for (select [UserID] from IMGroupMember WHERE GroupID=@dwGroupID  AND ( (CaptainID=@dwCaptainID AND (MemberRight&4)=0 ) OR UserID=@dwCaptainID))--筛选该队长的下属队员，包含该队长

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
--小队日统计
CREATE  PROCEDURE dbo.[GSP_IM_QueryTeamDayRecord]
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

	-- 查询权限
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
			SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
			RETURN 6	
		END

		IF (@UserRight&2) <> 0--管理员
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
		SET @strErrorDescribe=N'抱歉，您没有权限进行此操作！'
		RETURN 6		
	END

	--创建临时表
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
	--第一种方法
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

	--第二种方法
	declare @temp bigint
    --申明游标
    declare order_cursor cursor 
    for (select [UserID] from IMGroupMember WHERE GroupID=@dwGroupID  AND (MemberRight&0x04)<>0)
    --打开游标--
    open order_cursor
    --开始循环游标变量--
    fetch next from order_cursor into @temp

    while @@FETCH_STATUS = 0    --返回被 FETCH语句执行的最后游标的状态--
    begin 

		insert into   #temptable select @temp, sum(A.DrawCount),sum(A.BigWinnerCount),sum(A.CurScore),sum(A.ScoreCount),sum(A.RevenueCount) 
		From RecordUserDayInfo A ,IMGroupMember B
		where A.DateID=@dwDateID AND A.GroupID=@dwGroupID AND A.UserID=B.UserID AND (B.CaptainID=@temp OR ((B.MemberRight&0x04)<>0 AND B.UserID=@temp))

        fetch next from order_cursor into @temp   --转到下一个游标，没有会死循环
    end    

	close order_cursor  --关闭游标
    deallocate order_cursor   --释放游标

	select * from #temptable

	drop table #temptable
END

RETURN 0

GO

----------------------------------------------------------------------------------

-- 设置队长点位
CREATE  PROCEDURE dbo.GSP_IM_SetCaptainPoint
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@dwCaptainID	INT,			-- 队长ID
	@wAwardPoint	INT,			-- 点位
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
--WITH ENCRYPTION AS
AS
-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	DECLARE @UserRight BIGINT 
	DECLARE @MaxPoint INT
	DECLARE @OldPoint INT
	SELECT @UserRight=MemberRight,@OldPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID
	IF @UserRight IS NULL OR (@UserRight&0x04)=0
	BEGIN
		SET @strErrorDescribe=N'队长信息查询失败，请查证后再次尝试！'
		RETURN 1	
	END

	SELECT @UserRight=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @UserRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
		RETURN 2	
	END

	IF (@UserRight&1)<>0 OR (@UserRight&2)<>0--盟主或管理员
	BEGIN
		DECLARE @CreatorPoint SMALLINT
		SELECT @CreatorPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND MemberRight=1 
		IF @CreatorPoint IS NULL OR @CreatorPoint<0 OR @CreatorPoint>100
		BEGIN
			SET @strErrorDescribe=N'请联系运营设置圈主点位！'
			RETURN 3
		END

		
		IF @OldPoint> @wAwardPoint
		BEGIN
			SET @strErrorDescribe=N'设置无效，点位只能增加不能减少！'
			RETURN 4	
		END

		IF @wAwardPoint> @CreatorPoint
		BEGIN
			SET @strErrorDescribe=N'一级队长点位必须小于盟主点位，请重新设置！'
			RETURN 5	
		END
	END
	ELSE
	BEGIN
		IF @dwUserID<>(SELECT CaptainID FROM IMGroupMember WHERE UserID=@dwCaptainID AND GroupID=@dwGroupID)
		BEGIN
			SET @strErrorDescribe=N'您不是该队长的上级，无权设置！'
			RETURN 6	
		END

		SELECT @MaxPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		IF @wAwardPoint> @MaxPoint
		BEGIN
			SET @strErrorDescribe=N'下级队长点位必须小于上级点位，请重新设置！'
			RETURN 7	
		END

		IF @OldPoint> @wAwardPoint
		BEGIN
			SET @strErrorDescribe=N'设置无效，点位只能增加不能减少！'
			RETURN 8	
		END

	END


	UPDATE IMGroupMember SET AwardPoint=@wAwardPoint WHERE GroupID=@dwGroupID AND UserID=@dwCaptainID

END

RETURN 0

GO

--------------------------------------------------------------

--请求宝箱信息
CREATE  PROCEDURE dbo.[GSP_IM_OpenBox]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@szPwd			nvarchar(32),	--密码
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
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END	

	-- 帐号关闭
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须到重新开通后才能继续使用！'
		RETURN 3
	END

	-- 密码判断
	IF @LogonPass<>@szPwd
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END

	DECLARE @lAwardScore BIGINT
	DECLARE @nAwardPoint INT
	DECLARE @UserRight BIGINT
	SELECT @UserRight=MemberRight, @lAwardScore = AwardScore,@nAwardPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @UserRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息有误，请查证后再次尝试！'
		RETURN 5
	END

	SELECT @nAwardPoint AS AwardPoint,@lAwardScore AS AwardScore

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------


--------------------------------------------------------------

--保险箱提取记录
CREATE  PROCEDURE dbo.[GSP_IM_LoadBoxRecord]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT			--玩家标识	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	SELECT BeforeScore,Score,AfterScore,CollectTime FROM RecordBoxScore WHERE GroupID=@dwGroupID AND UserID=@dwUserID ORDER BY CollectTime DESC
END

RETURN 0

GO

-----------------------------------------------------
--查询邀请码
CREATE  PROCEDURE dbo.[GSP_IM_InvitationCode]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	DECLARE @lInvitationCode BIGINT	
	SELECT @lInvitationCode=InvitationCode FROM IMGroupInvitationCode WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	IF @lInvitationCode IS NULL
	BEGIN
		SET @strErrorDescribe=N'查询失败，请联系客服！'
		RETURN 2
	END

	SELECT @lInvitationCode AS InvitationCode

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

--绑定邀请码
CREATE  PROCEDURE dbo.[GSP_IM_BlindInvitationCode]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@lCode			BIGINT,			--邀请码
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	DECLARE @GroupID BIGINT
	DECLARE @SpreadID BIGINT
	DECLARE @UserRight INT

	SELECT @GroupID=GroupID,@SpreadID=UserID FROM IMGroupInvitationCode WHERE InvitationCode=@lCode
	IF @GroupID IS NULL
	BEGIN
		SET @strErrorDescribe=N'邀请码不存在，请查证后再次尝试！'
		RETURN 1
	END

	IF @GroupID<>@dwGroupID
	BEGIN
		SET @strErrorDescribe=N'该邀请码不属于当前大联盟，请查证后再次尝试！'
		RETURN 2
	END

	--查询玩家信息
	SELECT @UserRight=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @UserRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息错误，请查证后再次尝试！'
		RETURN 3
	END

	IF @UserRight<>0
	BEGIN
		SET @strErrorDescribe=N'只有普通成员才能绑定邀请码！'
		RETURN 4
	END

	--查询推广人信息
	SELECT @UserRight=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@SpreadID
	IF @UserRight IS NULL OR (@UserRight&4)=0
	BEGIN
		SET @strErrorDescribe=N'只能绑定队长的邀请码！'
		RETURN 5
	END

	UPDATE IMGroupMember SET MemberRight=8,CaptainID=@SpreadID WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	SELECT @SpreadID AS CaptainID


END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

--提取积分
CREATE  PROCEDURE dbo.[GSP_IM_GetAwardScore]
	@dwGroupID		BIGINT,			--群组标识	
	@dwUserID		BIGINT,			--玩家标识	
	@lScore			BIGINT,			--积分
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	IF @lScore<=0
	BEGIN
		SET @strErrorDescribe=N'提取积分必须大于0！'
		RETURN 1
	END

	DECLARE @AwardScore BIGINT
	SELECT @AwardScore=AwardScore FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @AwardScore IS NULL
	BEGIN
		SET @strErrorDescribe=N'玩家信息不存在，请查证后再次尝试！'
		RETURN 2
	END

	IF @lScore>@AwardScore
	BEGIN
		SET @strErrorDescribe=N'积分不足！'
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

--提取积分
CREATE  PROCEDURE dbo.[GSP_IM_CheckNewUser]
	@dwUserID		BIGINT,			--玩家标识	
	@szClientIP		NVARCHAR(15)	--IP
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	SELECT GroupID,InvitationCode FROM IMGroupBindIP WHERE UserIp=@szClientIP
	delete  FROM IMGroupBindIP WHERE UserIp=@szClientIP
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------