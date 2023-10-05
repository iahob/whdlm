
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

--增加AI参数
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AddAIParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AddAIParameter]
GO

--修改AI参数
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyAIParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyAIParameter]
GO

--删除AI参数
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_DeleteAIParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_DeleteAIParameter]
GO


--加载AI参数
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAIParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAIParameter]
GO

--加载AI玩家
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAIUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAIUser]
GO


--解锁AI玩家
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UNLOCK_AI]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UNLOCK_AI]
GO

--AI退出俱乐部
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AI_QUIT_GROUP]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AI_QUIT_GROUP]
GO

--加载单个AI
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadSingleAI]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadSingleAI]
GO

--清除AI
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ClearAI]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ClearAI]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 增加AI参数
CREATE PROC GSP_GP_AddAIParameter
	@GroupID					INT,							
	@lDistirbuteTotalScore		BIGINT,
	@dwAICount					BIGINT,
	@wKindID					SMALLINT,
	@dwPlayingAIRateMin			INT,
	@dwPlayingAIRateMax			INT,
	@lDistirbuteSingleScore		BIGINT,
	@lDistirbuteScoreMinRatio	BIGINT,
	@lDistirbuteScoreMaxRatio	BIGINT,
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息			
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--判断盟主剩余积分是否足够分配
	DECLARE @GroupTotalScore BIGINT
	SELECT @GroupTotalScore = Score FROM WHQJGroupDB.dbo.IMGroupWealth WHERE GroupID = @GroupID

	IF @lDistirbuteTotalScore > @GroupTotalScore
	BEGIN
		SET @strErrorDescribe = N'要求分配的勋章总数 ' + CONVERT(NVARCHAR(20),@lDistirbuteTotalScore) + N' 大于联盟未分配的勋章 ' + CONVERT(NVARCHAR(20),@GroupTotalScore) + '， 请重新配置'
		RETURN 1
	END



	INSERT INTO GroupAIParameter(GroupID, KindID, AICount, DistributeTotalScore,PlayingAIRateMin,PlayingAIRateMax,
	DistirbuteSingleScore,DistirbuteScoreMinRatio,DistirbuteScoreMaxRatio) 
	VALUES (@GroupID, @wKindID, @dwAICount,@lDistirbuteTotalScore, @dwPlayingAIRateMin, @dwPlayingAIRateMax,
	@lDistirbuteSingleScore, @lDistirbuteScoreMinRatio, @lDistirbuteScoreMaxRatio)

	--大联盟盟主
	DECLARE @GroupMaster INT
	SELECT @GroupMaster = CreaterID FROM WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID = @GroupID

	--选择机器人，并将其置入大联盟AI列表
	DECLARE @CountBegin INT
	SET @CountBegin = 0

	DECLARE @UserID INT

	WHILE(@CountBegin < @dwAICount)
	BEGIN
		SELECT @UserID = UserID  FROM AccountsInfo  WHERE IsAndroid = 1 AND IsGroupMember = 1 AND UserID NOT IN (SELECT UserID FROM GroupAIMember) AND UserID NOT IN (SELECT UserID FROM AndroidLockInfo)

		IF @UserID IS NULL OR @UserID = 0
		BEGIN
			DECLARE @InsertAICount INT
			SELECT @InsertAICount = COUNT(*) FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID
			SET @strErrorDescribe = N'已增加机器人总数' + CONVERT(NVARCHAR(20),@InsertAICount) + N'不满足指定AI人数 ' + CONVERT(NVARCHAR(20),@dwAICount) + '， AI账号数量不足，请配置'
			RETURN 2
		END

		INSERT INTO GroupAIMember( GroupID, UserID, KindID, PlayStatus) VALUES(@GroupID, @UserID, @wKindID, 0 )

		UPDATE AccountsInfo SET IsGroupMember = 1 WHERE UserID = @UserID

		--设置AI为大联盟成员
		DECLARE @Result INT 
		EXEC @Result = WHQJGroupDB.dbo.GSP_IM_AddGroupMember  @GroupID, @UserID, 0

		
		--select @GroupID as GroupID , @UserID as UserID , @Result as Result
		IF @Result = 0
		BEGIN
			--随机一个勋章值
			DECLARE @RandValue BIGINT
			SET @RandValue = @lDistirbuteScoreMinRatio + rand()*( @lDistirbuteScoreMaxRatio - @lDistirbuteScoreMinRatio)
			
			--随机值结果
			SET @RandValue = @lDistirbuteSingleScore + @lDistirbuteSingleScore * @RandValue/100

			select @RandValue as RandValue1

			--获取已经分配的勋章
			DECLARE @DistributedScore BIGINT
			SELECT @DistributedScore = SUM(Score) FROM WHQJGroupDB.dbo.IMGroupMember WHERE UserID IN (SELECT UserID FROM GroupAIMember WHERE GroupID = @GroupID)
			IF @DistributedScore + @RandValue> @lDistirbuteTotalScore
			BEGIN

			--select @DistributedScore as DistributedScore, @lDistirbuteTotalScore as DistirbuteTotalScore
			--	SET @RandValue = 0
				--SET @strErrorDescribe = N'当前AI分配总积分 ' + CONVERT(NVARCHAR(20),@DistributedScore) + N' 大于设定AI总积分 ' + CONVERT(NVARCHAR(20),@lDistirbuteTotalScore) + '， ，请配置'
				RETURN 0
			END
			select @RandValue as RandValue2
			--给AI分配勋章
			EXEC @Result = WHQJGroupDB.dbo.GSP_IM_PresentGroupScoreEx @GroupID, @GroupMaster, @UserID, @RandValue, 0
		END

		SET @CountBegin = @CountBegin + 1
	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 增加AI参数
CREATE PROC GSP_GP_ModifyAIParameter
	@GroupID					INT,							
	@lDistirbuteTotalScore		BIGINT,
	@dwAICount					BIGINT,
	@wKindID					SMALLINT,
	@dwPlayingAIRateMin			INT,
	@dwPlayingAIRateMax			INT,
	@lDistirbuteSingleScore		BIGINT,
	@lDistirbuteScoreMinRatio	BIGINT,
	@lDistirbuteScoreMaxRatio	BIGINT,
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息			
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--判断盟主剩余积分是否足够分配
	DECLARE @GroupTotalScore BIGINT
	SELECT @GroupTotalScore = Score FROM WHQJGroupDB.dbo.IMGroupWealth WHERE GroupID = @GroupID

	IF @lDistirbuteTotalScore > @GroupTotalScore
	BEGIN
		SET @strErrorDescribe = N'要求分配的勋章总数 ' + CONVERT(NVARCHAR(20),@lDistirbuteTotalScore) + N' 大于盟主身上的勋章 ' + CONVERT(NVARCHAR(20),@GroupTotalScore) + '， 请重新配置'
		RETURN 1
	END

	--判断当前配置是否存在
	DECLARE @preDistirbuteSingleScore BIGINT
	DECLARE @preDistirbuteTotalScore BIGINT
	DECLARE @preAICout INT
	SELECT @preAICout = AICount , @preDistirbuteTotalScore = DistributeTotalScore, @preDistirbuteSingleScore = DistirbuteSingleScore FROM GroupAIParameter WHERE GroupID = @GroupID AND KindID = @wKindID 
	IF @preDistirbuteTotalScore IS NOT NULL AND @preDistirbuteSingleScore IS NOT NULL
	BEGIN
		--判断当前批次AI 是否正在加载中
		DECLARE @AICount INT 
		SELECT @AICount = COUNT(*) FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 1
		IF @AICount <> 0 AND (@preDistirbuteSingleScore <> @lDistirbuteTotalScore OR @preDistirbuteTotalScore <> @lDistirbuteSingleScore)
		BEGIN
			SET @strErrorDescribe = N'AI正在使用中不允许修改该俱乐部AI携带总分数以及单个AI携带分数！'
			RETURN 2
		END
		ELSE
		BEGIN
			UPDATE GroupAIParameter SET AICount = @dwAICount, PlayingAIRateMin = @dwPlayingAIRateMin, PlayingAIRateMax = @dwPlayingAIRateMax, 
			DistirbuteScoreMinRatio = @lDistirbuteScoreMinRatio,DistirbuteScoreMaxRatio = @lDistirbuteScoreMaxRatio
		END
	END
	ELSE
	BEGIN
		SET @strErrorDescribe = N'改类型AI参数不存在请先添加！'
		RETURN 3
	END



	--大联盟盟主
	DECLARE @GroupMaster INT
	SELECT @GroupMaster = CreaterID FROM WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID = @GroupID

	--选择机器人，并将其置入大联盟AI列表
	DECLARE @CountBegin INT
	SET @CountBegin = 0

	DECLARE @UserID INT

	IF @dwAICount > @preAICout
	BEGIN
		WHILE(@CountBegin <= @dwAICount - @preAICout)
		BEGIN
			SELECT @UserID = UserID  FROM AccountsInfo  WHERE UserID NOT IN (SELECT UserID FROM GroupAIMember)

			IF @UserID IS NULL OR @UserID = 0
			BEGIN
				DECLARE @InsertAICount INT
				SELECT @InsertAICount = COUNT(*) FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID
				SET @strErrorDescribe = N'已增加机器人总数' + CONVERT(NVARCHAR(20),@InsertAICount) + N'不满足指定AI人数 ' + CONVERT(NVARCHAR(20),@dwAICount) + '， AI账号数量不足，请配置'
				RETURN 2
			END

			INSERT INTO GroupAIMember( GroupID, UserID, KindID, PlayStatus) VALUES(@GroupID, @UserID, @wKindID, 0 )

			--设置AI为大联盟成员
			DECLARE @Result INT 
			EXEC @Result = WHQJGroupDB.dbo.GSP_IM_AddGroupMember  @GroupID, @UserID, 0
			IF @Result = 0
			BEGIN


				--随机一个勋章值
				DECLARE @RandValue BIGINT
				SET @RandValue = @lDistirbuteScoreMinRatio + rand()*( @lDistirbuteScoreMaxRatio - @lDistirbuteScoreMinRatio)
			
				--随机值结果
				SET @RandValue = @lDistirbuteSingleScore + @lDistirbuteSingleScore * @RandValue/100

				--获取已经分配的勋章
				DECLARE @DistributedScore BIGINT
				SELECT @DistributedScore = SUM(Score) FROM WHQJGroupDB.dbo.IMGroupMember WHERE UserID IN (SELECT UserID FROM GroupAIMember WHERE GroupID = @GroupID)
				IF @DistributedScore + @RandValue> @lDistirbuteTotalScore
				BEGIN
					SET @RandValue = 0
				END

				--给AI分配勋章
				EXEC @Result = WHQJGroupDB.dbo.GSP_IM_PresentGroupScoreEx @GroupID, @GroupMaster, @UserID, @RandValue, 0
			END

			SET @CountBegin = @CountBegin + 1
		END
		
	END

	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 增加AI参数
CREATE PROC GSP_GP_DeleteAIParameter
	@GroupID					INT,							
	@wKindID					SMALLINT,
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息			
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	--判断当前配置是否存在
	DECLARE @preDistirbuteSingleScore BIGINT
	DECLARE @preDistirbuteTotalScore BIGINT
	DECLARE @preAICout INT
	SELECT @preAICout = AICount , @preDistirbuteTotalScore = DistributeTotalScore, @preDistirbuteSingleScore = DistirbuteSingleScore FROM GroupAIParameter WHERE GroupID = @GroupID AND KindID = @wKindID 
	IF @preDistirbuteTotalScore IS NOT NULL AND @preDistirbuteSingleScore IS NOT NULL
	BEGIN
		--判断当前批次AI 是否正在加载中
		DECLARE @AICount INT 
		SELECT @AICount = COUNT(*) FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 1
		IF @AICount <> 0
		BEGIN
			SET @strErrorDescribe = N'AI正在使用中不允许删除！'
			RETURN 2
		END
	END

	--获取AI总积分
	DECLARE @GroupAITotalScore BIGINT
	SELECT @GroupAITotalScore = SUM(Score) FROM WHQJGroupDB.dbo.IMGroupMember WHERE UserID IN (SELECT UserID FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID )

	--将AI积分还给盟主
	UPDATE WHQJGroupDB.dbo.IMGroupWealth  SET Score = Score + @GroupAITotalScore  WHERE GroupID = @GroupID

	--将AI账号置为 未分配装填
	UPDATE AccountsInfo SET IsGroupMember = 0 WHERE UserID IN (SELECT UserID FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID )

	--删除AI账号
	DELETE  FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID 

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
-- 读取参数
CREATE PROC GSP_GP_LoadAIParameter

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--选择可用状态俱乐部的AI配置
	SELECT * FROM GroupAIParameter WHERE GroupID IN (SELECT GroupID FROM WHQJGroupDB.dbo.IMGroupProperty WHERE GroupStatus = 1 )

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
------------加载AI玩家
-- 读取参数
CREATE PROC GSP_GP_LoadAIUser
	@GroupID					INT,							
	@wKindID					SMALLINT,
	@dwAICount					INT,
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	DECLARE @UserID INT

	--声明表变量
	DECLARE @temp TABLE
	(
	UserID INT
	);

	-- 将源表中的数据插入到表变量中
	INSERT INTO @temp(UserID)
	SELECT TOP(@dwAICount) UserID FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 0 

	--选择可用状态俱乐部的AI配置
	--SELECT TOP(@dwAICount)* FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 0 
	SELECT UserID, Score  FROM WHQJGroupDB.dbo.IMGroupMember WHERE GroupID = @GroupID AND UserID IN (SELECT UserID  FROM @temp)

	-- 将选中的AI设为 已经使用状态
	UPDATE GroupAIMember SET PlayStatus = 1 WHERE  UserID IN (SELECT UserID  FROM @temp)

	-- 将选中的AI设为 已经进入大联盟状态
	UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = 1 WHERE GroupID = @GroupID AND UserID IN (SELECT UserID  FROM @temp)
END

RETURN 0

GO

GO
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
------------加载AI玩家
CREATE PROC GSP_GP_UNLOCK_AI
	@GroupID					INT,							
	@wKindID					SMALLINT,
	@dwUserID					INT
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 将选中的AI设为 未使用状态
	UPDATE GroupAIMember SET PlayStatus = 0 WHERE GroupID = @GroupID AND KindID = @wKindID AND UserID = @dwUserID

	-- 改变俱乐部成员状态
	UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = 0 WHERE GroupID = @GroupID AND UserID  = @dwUserID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
------------加载AI玩家
CREATE PROC GSP_GP_AI_QUIT_GROUP
	@GroupID					INT,							
	@wKindID					SMALLINT,
	@dwUserID					INT
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	DECLARE @UserScore BIGINT	--勋章
	SELECT @UserScore = Score FROM WHQJGroupDB.dbo.IMGroupMember WHERE GroupID = @GroupID AND UserID  = @dwUserID

	-- 将选中的AI设为 未使用状态
	DELETE FROM GroupAIMember  WHERE GroupID = @GroupID AND KindID = @wKindID AND UserID = @dwUserID

	-- 改变俱乐部成员状态
	DELETE FROM WHQJGroupDB.dbo.IMGroupMember WHERE GroupID = @GroupID AND UserID  = @dwUserID

	-- 读取群组信息
	DECLARE @strGroupIDArray NVARCHAR(1024)
	SELECT @strGroupIDArray=GroupIDArray FROM WHQJGroupDB.dbo.IMUserGroupInfo WHERE UserID=@dwUserID

	-- 修改群组信息
	DECLARE @strGroupID NVARCHAR(20)		
	SET @strGroupID=';'+CONVERT(NVARCHAR(18),@GroupID)+';';
	SET @strGroupIDArray=REPLACE(@strGroupIDArray,@strGroupID,';');

	-- 更新群组
	UPDATE WHQJGroupDB.dbo.IMUserGroupInfo SET GroupIDArray=@strGroupIDArray WHERE UserID=@dwUserID		

	-- 更新人数
	UPDATE WHQJGroupDB.dbo.IMGroupProperty SET MemberCount=MemberCount-1 WHERE GroupID=@GroupID

	--插入删除记录
	IF @dwUserID <> 0
	BEGIN
		DECLARE @UserID		INT
		DECLARE @GameID		INT
		DECLARE @CustomID	INT
		DECLARE @NickName	NVARCHAR(32)
		SELECT @UserID=UserID, @GameID=GameID, @CustomID=CustomID, @NickName=NickName	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

		DECLARE @CreaterID  INT
		DECLARE @GroupName  NVARCHAR(32)
		SELECT @CreaterID=CreaterID,@GroupName=GroupName FROM WHQJGroupDB.dbo.IMGroupProperty(NOLOCK) WHERE GroupID=@GroupID

		DECLARE @OperatorGameID		INT
		DECLARE @OperatorNickName	NVARCHAR(32)
		SELECT  @OperatorGameID=GameID,@OperatorNickName=NickName	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

		INSERT INTO WHQJGroupDB.dbo.IMGroupApplyRecord(GroupID, ApplyerID, ApplyerGameID, ApplyerNickName, ApplyerCustomID, ApplyStatus, CreaterID, GroupName,ApplyType,OperatorID,OperatorName, CollectDateTime)
		VALUES (@GroupID, @dwUserID, @GameID, @NickName, @CustomID, 3, @CreaterID, @GroupName,1,@OperatorGameID,@OperatorNickName, GetDate())	
		DECLARE @RecordID INT
		SELECT 	@RecordID=SCOPE_IDENTITY()
		SELECT * FROM WHQJGroupDB.dbo.IMGroupApplyRecord WHERE RecordID=@RecordID
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 读取参数
CREATE PROC GSP_GP_LoadSingleAI
	@GroupID					INT,							
	@wKindID					SMALLINT,
	@dwAICount					INT,
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	DECLARE @UserID INT

	--声明表变量
	DECLARE @temp TABLE
	(
	UserID INT
	);

	-- 将源表中的数据插入到表变量中
	INSERT INTO @temp(UserID)
	SELECT TOP(@dwAICount) UserID FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 0 

	--选择可用状态俱乐部的AI配置
	SELECT TOP(@dwAICount)* FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 0 

	-- 将选中的AI设为 已经使用状态
	UPDATE GroupAIMember SET PlayStatus = 1 WHERE  UserID IN (SELECT UserID  FROM @temp)

	-- 将选中的AI设为 已经进入大联盟状态
	UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = 1 WHERE GroupID = @GroupID AND UserID IN (SELECT UserID  FROM @temp)

END

RETURN 0

GO
-----------------------------------------------------------------------------------------------------
------------加载AI玩家
CREATE PROC GSP_GP_ClearAI

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 将选中的AI设为 未使用状态
	UPDATE GroupAIMember SET PlayStatus = 0 

	-- 改变俱乐部成员状态
	UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = 0 WHERE  UserID IN (SELECT UserID  FROM GroupAIMember)
END

RETURN 0

GO

-----------------------------------------------------------------------------------------------------

