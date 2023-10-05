
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
-- 约战查询
CREATE  PROCEDURE dbo.GSP_IM_QueryBattle	
	@dwGroupID	BIGINT,			-- 群组标识	
	@dwCreaterID INT,			-- 用户标识
	@strPassword NCHAR(32),		-- 用户密码

	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- 输出信息
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
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwCreaterID

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
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END

	-- 查询大联盟
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 大联盟不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要查询的大联盟信息不存在，请查证后再次尝试！'		
		RETURN 5
	END

	-- 不是群主
	IF @CreaterID<>@dwCreaterID
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的盟主，没有权限进行此操作！'
		RETURN 6
	END

	--获取俱乐部约战消耗信息
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
-- 约战信息查询
CREATE  PROCEDURE dbo.GSP_IM_QueryBattleInfo	
	@dwGroupID	BIGINT			-- 群组标识	
WITH ENCRYPTION AS

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
		RETURN 1
	END

	SELECT * FROM StreamGroupBalance WHERE GroupID = @dwGroupID
	
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- 约战就算
CREATE  PROCEDURE dbo.GSP_IM_SettleBattle	
	@dwGroupID	BIGINT,			-- 群组标识	
	@dwMemberID INT,			-- 成员标识
	@wSettleCount SMALLINT,		-- 结算次数
	@dwCreaterID INT,			-- 用户标识
	@strPassword NCHAR(32),	-- 用户密码

	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- 输出信息
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
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwCreaterID

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
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END

	-- 查询大联盟
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 大联盟不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要操作的大联盟信息不存在信息不存在，请查证后再次尝试！'		
		RETURN 5
	END

	-- 不是群主
	IF @CreaterID<>@dwCreaterID
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的盟主，没有权限进行此操作！'
		RETURN 6
	END

	-- 更新数据
	DECLARE @BattleCount INT
	DECLARE @BalanceCount INT
	SELECT @BattleCount=BattleCount,@BalanceCount=BalanceCount FROM StreamGroupBalance(NOLOCK) WHERE UserID=@dwMemberID AND GroupID=@dwGroupID

	-- 存在校验
	IF @BattleCount IS NULL OR @BalanceCount IS NULL
	BEGIN
		SET @strErrorDescribe=N'抱歉，系统未查询到该成员的约战统计信息，结算失败！'
		RETURN 7				
	END

	-- 次数校验
	IF @wSettleCount>@BattleCount
	BEGIN
		SET @strErrorDescribe=N'抱歉，结算次数不能超过约战次数，结算失败！'
		RETURN 8
	END
	
	IF @BalanceCount>@wSettleCount
	BEGIN
		SET @strErrorDescribe=N'抱歉，结算次数不能小于历史次数，结算失败！'
		RETURN 8
	END

	-- 更新数据
	UPDATE StreamGroupBalance SET BalanceCount=@wSettleCount WHERE GroupID=@dwGroupID AND UserID=@dwMemberID	
	
	SET @strErrorDescribe=N'结算成功！'

	SELECT UserID,BattleCount,BalanceCount FROM StreamGroupBalance WHERE GroupID=@dwGroupID AND UserID=@dwMemberID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 赠送勋章
CREATE  PROCEDURE dbo.GSP_IM_PresentGroupScore
	@dwGroupID	BIGINT,			-- 群组标识
	@dwPresentUserID INT,		-- 赠送玩家ID
	@lScore BIGINT,				-- 赠送分
	@cbType TINYINT,			-- 消耗方式 0发送 1消耗
	@nCount INT,				-- 
	@strPassword NCHAR(32),		-- 用户密码
	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- 输出信息
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
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwPresentUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 1
	END	

	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 2
	END

	-- 查询大联盟
	DECLARE @CreaterID  INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 大联盟不存在
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息不存在，请查证后再次尝试！'	
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
			SET @strErrorDescribe=N'操作失败，管理员权限不够！'	
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
			SET @strErrorDescribe=N'操作失败，队长权限不够！'	
			RETURN 5
		END

		SET @cbHaveRight=4
	END
	
	IF @cbHaveRight=0
	BEGIN
		SET @strErrorDescribe=N'操作失败，权限不够！'	
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
				SET @strErrorDescribe=N'操作失败，俱乐部勋章不够发放！'
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
				SET @strErrorDescribe=N'操作失败，队长勋章不够发放！'
				RETURN 7
			END
		END
	END

	SELECT @cbHaveRight AS UserRight,@lAllScore AS BeforeScore
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- 赠送勋章
CREATE  PROCEDURE dbo.GSP_IM_PresentGroupScoreEx
	@dwGroupID	BIGINT,			-- 群组标识
	@dwPresentUserID INT,		-- 赠送玩家ID
	@dwReceiveUserID INT,		-- 接收玩家ID	
	@lScore BIGINT,				-- 赠送分
	@cbType TINYINT				-- 消耗方式 0发送 1消耗
--WITH ENCRYPTION AS
AS
-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
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

	IF @cbType=0--发放
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
	ELSE--扣除
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
-- 赠送记录查询
CREATE  PROCEDURE dbo.GSP_IM_QueryPresent
	@dwGroupID	INT,			-- 群组标识	
	@dwQueryID	INT,			-- 查询标识
	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- 输出信息
WITH ENCRYPTION AS

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
		RETURN 1
	END

	-- 检查查询玩家
	DECLARE @QueryUserID INT
	DECLARE @MemberRight INT
	SELECT @QueryUserID=UserID,@MemberRight=MemberRight FROM IMGroupMember WHERE GroupID = @dwGroupID AND UserID=@dwQueryID
	-- 查询玩家不属于该大联盟
	IF @QueryUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'查询玩家不属于该大联盟！'
		RETURN 2
	END
	
	--盟主查询
	IF @dwQueryID=@CreaterID
	BEGIN
		SELECT top 30 * FROM RecordGroupScorePresent WHERE GroupID = @dwGroupID  Order By CollectDate Desc
	END
	--操作玩家是管理员
	ELSE IF (@MemberRight&2) <> 0
	BEGIN
		SELECT top 30 * FROM RecordGroupScorePresent WHERE GroupID = @dwGroupID Order By CollectDate Desc
	END
	--操作玩家是队长
	ELSE IF (@MemberRight&4) <> 0
	BEGIN
		SELECT top 30 * FROM RecordGroupScorePresent WHERE GroupID = @dwGroupID AND ( ReceiveUserID in (select UserID from IMGroupMember where GroupID=@dwGroupID AND (CaptainID=@QueryUserID OR UserID=@dwQueryID)) OR PresentUserID=@dwQueryID )Order By CollectDate Desc
	END
	ELSE--成员查询
	BEGIN
		SELECT top 30 * FROM RecordGroupScorePresent WHERE GroupID = @dwGroupID AND  ReceiveUserID=@dwQueryID Order By CollectDate Desc
	END

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------


-- 扣费信息查询
CREATE  PROCEDURE dbo.GSP_IM_QueryRevenue
	@dwGroupID	INT,			-- 群组标识	
	@dwQueryID	INT,			-- 查询标识
	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- 输出信息
WITH ENCRYPTION AS

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
		SET @strErrorDescribe=N'该大联盟信息不存在！'
		RETURN 1
	END
	
	DECLARE @nRight  INT
	SELECT @nRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID
		
	IF @CreaterID = @dwQueryID OR (@nRight&2) <> 0
	BEGIN
		SELECT * FROM RecordGroupScoreRevenue WHERE GroupID = @dwGroupID AND  CollectDate>=DateAdd(d,-3,getdate()) Order By CollectDate Desc
	END
	ELSE--成员查询
	BEGIN
		SET @strErrorDescribe=N'只有盟主和管理员才能查询！'
		RETURN 4
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 扣费信息查询
CREATE  PROCEDURE dbo.GSP_IM_SetFlag
	@dwGroupID		INT,			-- 群组标识	
	@dwOperateID	INT,			-- 操作玩家标识
	@dwTargetID		INT,			-- 目标玩家标识
	@dwDateID		INT,			-- 日期ID
	@cbFlag			TINYINT,		-- 标记
	-- 输出参数	
	@strErrorDescribe NVARCHAR(127) OUTPUT -- 输出信息
WITH ENCRYPTION AS

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
		RETURN 1
	END

	IF NOT EXISTS(Select 1 FROM RecordUserDayInfo WHERE GroupID=@dwGroupID AND UserID=@dwTargetID AND DateID=@dwDateID)
	BEGIN
		SET @strErrorDescribe=N'您要操作的记录信息不存在，请查证后再次尝试！'		
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
			SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
			RETURN 2	
		END

		IF (@UserRight&2) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperateID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x80)<>0 
				SET @HaveRight=1
		END

		IF (@UserRight&4) <> 0--队长
		BEGIN
			IF @dwOperateID=@dwTargetID
			BEGIN
				SET @strErrorDescribe=N'不能给自己点赞！'
				RETURN 3	
			END

			IF @dwOperateID<>(Select CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwTargetID)
			BEGIN
				SET @strErrorDescribe=N'该玩家不是您的下属队员！'
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
		SET @strErrorDescribe=N'操作失败，无权设置！'
		RETURN 3		
	END

	UPDATE RecordUserDayInfo SET Flag=@cbFlag WHERE GroupID=@dwGroupID AND UserID=@dwTargetID AND DateID=@dwDateID

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------


-- 勋章清零
CREATE  PROCEDURE dbo.GSP_IM_ClearMedal
	@dwGroupID		INT,			-- 群组标识	
	@dwDateID		INT
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	

	declare @inserttime datetime
	set @inserttime=GETDATE()

	--declare @dateID int
	--Set @dateID=CONVERT(int,CONVERT(nvarchar(8),@inserttime,112))--格式20190715   

    declare @temp bigint
    --申明游标
    declare order_cursor cursor 
    for (select [UserID] from IMGroupMember WHERE GroupID=@dwGroupID)
    --打开游标--
    open order_cursor
    --开始循环游标变量--
    fetch next from order_cursor into @temp

    while @@FETCH_STATUS = 0    --返回被 FETCH语句执行的最后游标的状态--
    begin 
		-- 更新日记录
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
        fetch next from order_cursor into @temp   --转到下一个游标，没有会死循环
    end    

    close order_cursor  --关闭游标
    deallocate order_cursor   --释放游标

END

RETURN 0

GO