
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryTaskInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryTaskInfo]
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TaskForward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TaskForward]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TaskForwardRoom]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TaskForwardRoom]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TaskReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TaskReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 查询任务
CREATE PROC GSP_GR_QueryTaskInfo
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询用户
	IF NOT EXISTS(SELECT UserID FROM AccountsInfo WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe = N'抱歉，你的用户信息不存在或者密码不正确！'
		return 1
	END
	
	--删除任务
	DELETE FROM AccountsTask WHERE  UserID=@dwUserID AND TaskID NOT IN(Select TaskID From  WHQJPlatformDB.dbo.TaskInfo WHERE Nullity=0)
	
	--任务目标改变
	UPDATE AccountsTask SET Progress=0,TaskStatus=0,CollectDate=GETDATE(),KindID=A.KindID
	FROM  WHQJPlatformDB.dbo.TaskInfo AS A,AccountsTask AS B
	WHERE  UserID=@dwUserID  AND A.TaskID=B.TaskID AND EXISTS 
	(SELECT UserID FROM  WHQJPlatformDB.dbo.TaskInfo AS A,AccountsTask AS B
	 WHERE UserID=@dwUserID AND B.TaskID=A.TaskID AND A.KindID<>B.KindID)
	
	
	--超时更新任务进度
	UPDATE AccountsTask SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE  UserID=@dwUserID  AND( (TimeType=0x02 AND DATEDIFF(DAY,CollectDate,GETDATE())>0 )
																										OR(TimeType=0x04 AND DATEDIFF(WEEK,CollectDate,GETDATE())>0)
																										OR(TimeType=0x08 AND DATEDIFF(MONTH,CollectDate,GETDATE())>0 ))
	
	
	--插入任务	
	INSERT INTO AccountsTask(UserID,TaskID,KindID,ServerID,TaskType,TaskObject,Progress,TimeType,TaskStatus)
	SELECT @dwUserID AS UserID,TaskID,KindID,ServerID,TaskType,Innings,0,TimeType,0
	FROM  WHQJPlatformDB.dbo.TaskInfo AS A
	WHERE A.Nullity=0 AND NOT EXISTS (SELECT UserID,TaskID FROM AccountsTask AS B WITH(INDEX(IX_AccountsTask_UserTask)) WHERE UserID=@dwUserID AND B.TaskID=A.TaskID)

	--查询任务
	SELECT C.TaskID,C.KindID,C.ServerID,C.TaskType,C.TaskObject,C.Progress,C.TimeType,C.TaskStatus,D.TaskAward,D.TaskName,D.TaskDescription 
	FROM AccountsTask AS C 
	LEFT JOIN  WHQJPlatformDB.dbo.TaskInfo AS D
	ON C.TaskID=D.TaskID
	WHERE UserID=@dwUserID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 任务推进
CREATE PROC GSP_GR_TaskForward
	@dwUserID INT,								-- 用户 I D
	@wKindID INT,								-- 游戏标识
	@wServerID INT,								-- 房间标识
	@lWinCount INT,								-- 赢局局数
	@lLostCount INT,							-- 输局局数
	@lDrawCount	INT								-- 和局局数
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--查询任务
	CREATE TABLE #TempTaskInfo(	
		[ID] [int] primary key NOT NULL,
		[UserID] [int] NOT NULL,
		[TaskID] [int] NOT NULL,
		[KindID] [int] NOT NULL,
		[ServerID] [int] NOT NULL,
		[TaskType] [int] NOT NULL,
		[TaskObject] [int] NOT NULL,
		[Progress] [int] NOT NULL,
		[TimeType] [tinyint] NOT NULL,
		[TaskStatus] [tinyint] NOT NULL,
		[CollectDate] [datetime] NOT NULL)

	INSERT INTO #TempTaskInfo 
	SELECT * FROM AccountsTask WITH(INDEX(IX_AccountsTask_UserKind)) WHERE UserID=@dwUserID AND KindID=@wKindID AND ServerID=@wServerID AND TaskStatus=0

	INSERT INTO #TempTaskInfo 
	SELECT * FROM AccountsTask WITH(INDEX(IX_AccountsTask_UserKind)) WHERE UserID=@dwUserID AND KindID=@wKindID AND ServerID=0 AND TaskStatus=0

	-- 更新任务
	UPDATE #TempTaskInfo SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE  TimeType=0x02 AND DATEDIFF(DAY,CollectDate,GETDATE())>0 
	UPDATE #TempTaskInfo SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE  TimeType=0x04 AND DATEDIFF(WEEK,CollectDate,GETDATE())>0 
	UPDATE #TempTaskInfo SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE  TimeType=0x08 AND DATEDIFF(MONTH,CollectDate,GETDATE())>0 																		

	-- 推进任务
	UPDATE #TempTaskInfo SET Progress=Progress+@lWinCount+@lLostCount+@lDrawCount WHERE TaskType=0x01	--总局任务
	UPDATE #TempTaskInfo SET Progress=Progress+@lWinCount WHERE TaskType=0x02							--胜局任务

	IF @lWinCount<>0 --首胜连胜任务
	BEGIN
		UPDATE #TempTaskInfo SET Progress=@lWinCount WHERE TaskType=0x04
		UPDATE #TempTaskInfo SET Progress=Progress+@lWinCount WHERE TaskType=0x08
	END
	ELSE	--连胜中断
	BEGIN
		UPDATE #TempTaskInfo SET Progress=0 WHERE TaskType=0x08
	END

	-- 更新状态
	UPDATE #TempTaskInfo SET TaskStatus = 1 WHERE Progress>=TaskObject

	--更新物理表
	UPDATE AccountsTask SET AccountsTask.Progress=#TempTaskInfo.Progress,AccountsTask.TaskStatus=#TempTaskInfo.TaskStatus,AccountsTask.CollectDate=#TempTaskInfo.CollectDate
	FROM AccountsTask WITH(INDEX(IX_AccountsTask_UserTask)),#TempTaskInfo WHERE AccountsTask.ID=#TempTaskInfo.ID AND AccountsTask.UserID=#TempTaskInfo.UserID
		
	--删除临时表
	DROP TABLE #TempTaskInfo

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 领取奖励
CREATE PROC GSP_GR_TaskReward
	@dwUserID INT,								-- 用户 I D
	@wTaskID  INT,								-- 任务 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @MemberOrder INT
	SELECT @MemberOrder=MemberOrder FROM WHQJAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID AND DynamicPass=@strPassword
	IF @MemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe = N'抱歉，你的用户信息不存在或者密码不正确！'
		return 1
	END
	
	-- 查询任务
	DECLARE @dwID INT
	DECLARE @TaskInputDate DATETIME
	DECLARE @TimeType TINYINT
	SELECT @dwID=ID,@TaskInputDate=CollectDate,@TimeType=TimeType FROM AccountsTask WITH(INDEX(IX_AccountsTask_UserTask))
	WHERE UserID=@dwUserID AND TaskID=@wTaskID

	IF @dwID IS NULL
	BEGIN
		SET @strErrorDescribe = N'系统查找不到该任务，请核对之后再操作！'
		return 2		
	END	
	
	--查询任务表
	DECLARE @wTmpTaskID INT
	SELECT @wTmpTaskID = TaskID FROM  WHQJPlatformDB.dbo.TaskInfo WHERE TaskID = @wTaskID AND Nullity=0
	
	IF @wTmpTaskID IS NULL
	BEGIN
		SET @strErrorDescribe = N'抱歉,该任务已经失效！'
		return 3	
	END
	
	--更新任务
	DECLARE @dwDateDiff INT
	SET @dwDateDiff = (CASE WHEN @TimeType=0x02 THEN DATEDIFF(DAY,@TaskInputDate,GETDATE())
							WHEN @TimeType=0x04 THEN DATEDIFF(WEEK,@TaskInputDate,GETDATE())
							WHEN @TimeType=0x08 THEN DATEDIFF(MONTH,@TaskInputDate,GETDATE())
						END)
	
	IF @dwDateDiff>0 UPDATE AccountsTask SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE ID=@dwID
	
	--获取状态
	DECLARE @dwTaskStatus INT
	SELECT @dwTaskStatus=TaskStatus FROM AccountsTask WHERE ID=@dwID
	
	IF @dwTaskStatus =0
	BEGIN
		SET @strErrorDescribe = N'请完成当前的任务再来领取奖励！'
		return 4		
	END	

	IF @dwTaskStatus =2
	BEGIN
		SET @strErrorDescribe = N'您已经领取了该奖励,请不要重复领取！'
		return 4		
	END		

	-- 查询奖励
	DECLARE @AwardType INT
	DECLARE @strAwardValue NVARCHAR(100)
	SELECT @strAwardValue=TaskAward
	FROM  WHQJPlatformDB.dbo.TaskInfo WHERE TaskID=@wTaskID
	
	IF LEN(@strAwardValue)=0
	BEGIN
		SET @strErrorDescribe = N'抱歉,该任务没有配置奖励,请联系管理人员进行配置！'
		return 5	
	END
	
  /*  -- 会员等级
    DECLARE @CurrMemberOrder SMALLINT
    SET   @CurrMemberOrder = 0
    SELECT @CurrMemberOrder=MemberOrder FROM AccountsInfo WHERE UserID=@dwUserID
    
    --会员奖励
    DECLARE @MemberRate INT
    SET @MemberRate = 0

    SELECT @MemberRate=TaskRate FROM MemberProperty where @CurrMemberOrder =MemberOrder
    SET @AwardGold = (@AwardGold * (100+ @MemberRate)/100)
    SET @AwardIngot = (@AwardIngot * (100+ @MemberRate)/100)*/
	
	-- 查询金币
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	DECLARE @CurrMedal INT
	SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo  WHERE UserID=@dwUserID
	SELECT @CurrMedal=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
	
	--查询奖励
	DECLARE @AwardIngot BIGINT
	DECLARE @AwardGot BIGINT
	DECLARE @AwardTicket INT
	DECLARE @StrFindBegin INT
	DECLARE @StrFindEnd INT
	DECLARE @StrLen INT
	
	--配置奖励类型
	SET @AwardType=0;
	IF CHARINDEX('1:',@strAwardValue)>0 SET @AwardType= @AwardType|0x01
	IF CHARINDEX('2:',@strAwardValue)>0 SET @AwardType= @AwardType|0x02
	IF CHARINDEX('3:',@strAwardValue)>0 SET @AwardType= @AwardType|0x04
	
	-- 更新元宝
	IF @AwardType&0x02>0
	BEGIN
		--获取奖励值
		SET @StrFindBegin = CHARINDEX('2:',@strAwardValue)
		SET @StrFindEnd = CHARINDEX(';',@strAwardValue,@StrFindBegin)
		--奖励校验
		IF @StrFindBegin=0
		BEGIN
			SET @strErrorDescribe = N'抱歉,该任务没有配置钻石奖励,请联系管理人员进行配置！'
			return 5
		END
		--字符长度
		IF @StrFindEnd=0 SET @StrLen=LEN(@strAwardValue)-@StrFindBegin+1
		ELSE SET @StrLen=@StrFindEnd-@StrFindBegin
		
		--转换奖励值
		SET @AwardIngot = CAST(SUBSTRING(@strAwardValue,@StrFindBegin+2,@StrLen-2) AS BIGINT);
		
		UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@AwardIngot 
		WHERE UserID=@dwUserID	
	END

		
	-- 更新金币
	IF @AwardType&0x01>0
	BEGIN
		--获取奖励值	
		SET @StrFindBegin = CHARINDEX('1:',@strAwardValue)
		SET @StrFindEnd = CHARINDEX(';',@strAwardValue,@StrFindBegin)
		--奖励校验
		IF @StrFindBegin=0
		BEGIN
			SET @strErrorDescribe = N'抱歉,该任务没有配置金币奖励,请联系管理人员进行配置！'
			return 5
		END
		
		--字符长度
		IF @StrFindEnd=0 SET @StrLen=LEN(@strAwardValue)-@StrFindBegin+1
		ELSE SET @StrLen=@StrFindEnd-@StrFindBegin
		
		--转换奖励值
		SET @AwardGot = CAST(SUBSTRING(@strAwardValue,@StrFindBegin+2,@StrLen-2) AS BIGINT);
		
		UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score=Score+@AwardGot 
		WHERE UserID=@dwUserID
	END
	--更新奖劵
	IF @AwardType&0x04>0
	BEGIN
		--获取奖励值	
		SET @StrFindBegin = CHARINDEX('3:',@strAwardValue)
		--奖励校验
		IF @StrFindBegin=0
		BEGIN
			SET @strErrorDescribe = N'抱歉,该任务没有配置奖劵奖励,请联系管理人员进行配置！'
			return 5
		END
		
		--字符长度
		SET @StrLen=LEN(@strAwardValue)-@StrFindBegin+1
		
		--转换奖励值
		SET @AwardTicket = CAST(SUBSTRING(@strAwardValue,@StrFindBegin+2,@StrLen-2) AS INT);
		
		UPDATE  WHQJTreasureDB.dbo.UserCurrency SET AwardTicket=AwardTicket+@AwardTicket 
		WHERE UserID=@dwUserID
	END
	--更正分数
	IF @AwardIngot IS NULL SET @AwardIngot=0
	IF @AwardGot IS NULL SET @AwardGot=0
	IF @AwardTicket IS NULL SET @AwardTicket=0

	-- 查询分数
	DECLARE @UserScore BIGINT
	DECLARE @UserIngot BIGINT
	DECLARE @UserTicket BIGINT

	-- 查询金币
	SELECT @UserScore=Score FROM   WHQJTreasureDB.dbo.GameScoreInfo 
	WHERE UserID=@dwUserID				

	-- 查询元宝
	SELECT @UserIngot=Diamond,@UserTicket=AwardTicket FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID

	
	-- 调整分数
	IF @UserScore IS NULL SET @UserScore=0		
	IF @UserIngot IS NULL SET @UserIngot=0
	IF @UserTicket IS NULL SET @UserTicket=0
	-- 更新任务状态	
	UPDATE AccountsTask SET TaskStatus=2 WHERE ID=@dwID
	
	-- 任务记录
	INSERT WHQJRecordDB.dbo.RecordTask(DateID,UserID,TaskID,TimeType,TaskAward,CollectDate)
	VALUES (CAST(CAST(@TaskInputDate AS FLOAT) AS INT),@dwUserID,@wTaskID,@TimeType,@strAwardValue,GetDate())

	-- 计算时间
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
	IF @AwardType&0x02>0
	BEGIN
		-- 元宝记录
		INSERT INTO RecordMedalChange(UserID,SrcMedal,TradeMedal,TypeID,ClientIP,CollectDate)	
		VALUES (@dwUserID,@CurrMedal,@AwardIngot,1,@strClientIP,GETDATE())
	END
	
	IF @AwardType&0x04>0
	BEGIN
		-- 奖劵记录
		INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
		VALUES (dbo.WF_GetSerialNumber(),0,@dwUserID,2,@UserTicket-@AwardTicket,@AwardTicket,@strClientIP,GETDATE())
	END
	-- 流水账
	IF @AwardType&0x01>0
	BEGIN
		INSERT INTO  WHQJTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
		VALUES (@dwUserID,@CurrScore,@CurrInsure,@AwardGot,7,@strClientIP,GETDATE())
	
		-- 日统计
		UPDATE  WHQJTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@AwardGot WHERE UserID=@dwUserID AND TypeID=7
		IF @@ROWCOUNT=0
		BEGIN
			INSERT  WHQJTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,7,1,@AwardGot)
		END	
	END
		
	-- 成功提示
	SET @strErrorDescribe = N'恭喜您，奖励领取成功！'

	-- 抛出数据
	SELECT @UserScore AS Score,@UserIngot AS Ingot,@UserTicket AS AwardTicket
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
