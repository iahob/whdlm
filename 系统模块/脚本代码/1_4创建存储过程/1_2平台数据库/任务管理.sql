
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

-- ��ѯ����
CREATE PROC GSP_GR_QueryTaskInfo
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ�û�
	IF NOT EXISTS(SELECT UserID FROM AccountsInfo WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END
	
	--ɾ������
	DELETE FROM AccountsTask WHERE  UserID=@dwUserID AND TaskID NOT IN(Select TaskID From  WHQJPlatformDB.dbo.TaskInfo WHERE Nullity=0)
	
	--����Ŀ��ı�
	UPDATE AccountsTask SET Progress=0,TaskStatus=0,CollectDate=GETDATE(),KindID=A.KindID
	FROM  WHQJPlatformDB.dbo.TaskInfo AS A,AccountsTask AS B
	WHERE  UserID=@dwUserID  AND A.TaskID=B.TaskID AND EXISTS 
	(SELECT UserID FROM  WHQJPlatformDB.dbo.TaskInfo AS A,AccountsTask AS B
	 WHERE UserID=@dwUserID AND B.TaskID=A.TaskID AND A.KindID<>B.KindID)
	
	
	--��ʱ�����������
	UPDATE AccountsTask SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE  UserID=@dwUserID  AND( (TimeType=0x02 AND DATEDIFF(DAY,CollectDate,GETDATE())>0 )
																										OR(TimeType=0x04 AND DATEDIFF(WEEK,CollectDate,GETDATE())>0)
																										OR(TimeType=0x08 AND DATEDIFF(MONTH,CollectDate,GETDATE())>0 ))
	
	
	--��������	
	INSERT INTO AccountsTask(UserID,TaskID,KindID,ServerID,TaskType,TaskObject,Progress,TimeType,TaskStatus)
	SELECT @dwUserID AS UserID,TaskID,KindID,ServerID,TaskType,Innings,0,TimeType,0
	FROM  WHQJPlatformDB.dbo.TaskInfo AS A
	WHERE A.Nullity=0 AND NOT EXISTS (SELECT UserID,TaskID FROM AccountsTask AS B WITH(INDEX(IX_AccountsTask_UserTask)) WHERE UserID=@dwUserID AND B.TaskID=A.TaskID)

	--��ѯ����
	SELECT C.TaskID,C.KindID,C.ServerID,C.TaskType,C.TaskObject,C.Progress,C.TimeType,C.TaskStatus,D.TaskAward,D.TaskName,D.TaskDescription 
	FROM AccountsTask AS C 
	LEFT JOIN  WHQJPlatformDB.dbo.TaskInfo AS D
	ON C.TaskID=D.TaskID
	WHERE UserID=@dwUserID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �����ƽ�
CREATE PROC GSP_GR_TaskForward
	@dwUserID INT,								-- �û� I D
	@wKindID INT,								-- ��Ϸ��ʶ
	@wServerID INT,								-- �����ʶ
	@lWinCount INT,								-- Ӯ�־���
	@lLostCount INT,							-- ��־���
	@lDrawCount	INT								-- �;־���
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	--��ѯ����
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

	-- ��������
	UPDATE #TempTaskInfo SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE  TimeType=0x02 AND DATEDIFF(DAY,CollectDate,GETDATE())>0 
	UPDATE #TempTaskInfo SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE  TimeType=0x04 AND DATEDIFF(WEEK,CollectDate,GETDATE())>0 
	UPDATE #TempTaskInfo SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE  TimeType=0x08 AND DATEDIFF(MONTH,CollectDate,GETDATE())>0 																		

	-- �ƽ�����
	UPDATE #TempTaskInfo SET Progress=Progress+@lWinCount+@lLostCount+@lDrawCount WHERE TaskType=0x01	--�ܾ�����
	UPDATE #TempTaskInfo SET Progress=Progress+@lWinCount WHERE TaskType=0x02							--ʤ������

	IF @lWinCount<>0 --��ʤ��ʤ����
	BEGIN
		UPDATE #TempTaskInfo SET Progress=@lWinCount WHERE TaskType=0x04
		UPDATE #TempTaskInfo SET Progress=Progress+@lWinCount WHERE TaskType=0x08
	END
	ELSE	--��ʤ�ж�
	BEGIN
		UPDATE #TempTaskInfo SET Progress=0 WHERE TaskType=0x08
	END

	-- ����״̬
	UPDATE #TempTaskInfo SET TaskStatus = 1 WHERE Progress>=TaskObject

	--���������
	UPDATE AccountsTask SET AccountsTask.Progress=#TempTaskInfo.Progress,AccountsTask.TaskStatus=#TempTaskInfo.TaskStatus,AccountsTask.CollectDate=#TempTaskInfo.CollectDate
	FROM AccountsTask WITH(INDEX(IX_AccountsTask_UserTask)),#TempTaskInfo WHERE AccountsTask.ID=#TempTaskInfo.ID AND AccountsTask.UserID=#TempTaskInfo.UserID
		
	--ɾ����ʱ��
	DROP TABLE #TempTaskInfo

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��ȡ����
CREATE PROC GSP_GR_TaskReward
	@dwUserID INT,								-- �û� I D
	@wTaskID  INT,								-- ���� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @MemberOrder INT
	SELECT @MemberOrder=MemberOrder FROM WHQJAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID AND DynamicPass=@strPassword
	IF @MemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END
	
	-- ��ѯ����
	DECLARE @dwID INT
	DECLARE @TaskInputDate DATETIME
	DECLARE @TimeType TINYINT
	SELECT @dwID=ID,@TaskInputDate=CollectDate,@TimeType=TimeType FROM AccountsTask WITH(INDEX(IX_AccountsTask_UserTask))
	WHERE UserID=@dwUserID AND TaskID=@wTaskID

	IF @dwID IS NULL
	BEGIN
		SET @strErrorDescribe = N'ϵͳ���Ҳ�����������˶�֮���ٲ�����'
		return 2		
	END	
	
	--��ѯ�����
	DECLARE @wTmpTaskID INT
	SELECT @wTmpTaskID = TaskID FROM  WHQJPlatformDB.dbo.TaskInfo WHERE TaskID = @wTaskID AND Nullity=0
	
	IF @wTmpTaskID IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,�������Ѿ�ʧЧ��'
		return 3	
	END
	
	--��������
	DECLARE @dwDateDiff INT
	SET @dwDateDiff = (CASE WHEN @TimeType=0x02 THEN DATEDIFF(DAY,@TaskInputDate,GETDATE())
							WHEN @TimeType=0x04 THEN DATEDIFF(WEEK,@TaskInputDate,GETDATE())
							WHEN @TimeType=0x08 THEN DATEDIFF(MONTH,@TaskInputDate,GETDATE())
						END)
	
	IF @dwDateDiff>0 UPDATE AccountsTask SET Progress=0,TaskStatus=0,CollectDate=GETDATE() WHERE ID=@dwID
	
	--��ȡ״̬
	DECLARE @dwTaskStatus INT
	SELECT @dwTaskStatus=TaskStatus FROM AccountsTask WHERE ID=@dwID
	
	IF @dwTaskStatus =0
	BEGIN
		SET @strErrorDescribe = N'����ɵ�ǰ������������ȡ������'
		return 4		
	END	

	IF @dwTaskStatus =2
	BEGIN
		SET @strErrorDescribe = N'���Ѿ���ȡ�˸ý���,�벻Ҫ�ظ���ȡ��'
		return 4		
	END		

	-- ��ѯ����
	DECLARE @AwardType INT
	DECLARE @strAwardValue NVARCHAR(100)
	SELECT @strAwardValue=TaskAward
	FROM  WHQJPlatformDB.dbo.TaskInfo WHERE TaskID=@wTaskID
	
	IF LEN(@strAwardValue)=0
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,������û�����ý���,����ϵ������Ա�������ã�'
		return 5	
	END
	
  /*  -- ��Ա�ȼ�
    DECLARE @CurrMemberOrder SMALLINT
    SET   @CurrMemberOrder = 0
    SELECT @CurrMemberOrder=MemberOrder FROM AccountsInfo WHERE UserID=@dwUserID
    
    --��Ա����
    DECLARE @MemberRate INT
    SET @MemberRate = 0

    SELECT @MemberRate=TaskRate FROM MemberProperty where @CurrMemberOrder =MemberOrder
    SET @AwardGold = (@AwardGold * (100+ @MemberRate)/100)
    SET @AwardIngot = (@AwardIngot * (100+ @MemberRate)/100)*/
	
	-- ��ѯ���
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	DECLARE @CurrMedal INT
	SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM  WHQJTreasureDB.dbo.GameScoreInfo  WHERE UserID=@dwUserID
	SELECT @CurrMedal=Diamond FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID
	
	--��ѯ����
	DECLARE @AwardIngot BIGINT
	DECLARE @AwardGot BIGINT
	DECLARE @AwardTicket INT
	DECLARE @StrFindBegin INT
	DECLARE @StrFindEnd INT
	DECLARE @StrLen INT
	
	--���ý�������
	SET @AwardType=0;
	IF CHARINDEX('1:',@strAwardValue)>0 SET @AwardType= @AwardType|0x01
	IF CHARINDEX('2:',@strAwardValue)>0 SET @AwardType= @AwardType|0x02
	IF CHARINDEX('3:',@strAwardValue)>0 SET @AwardType= @AwardType|0x04
	
	-- ����Ԫ��
	IF @AwardType&0x02>0
	BEGIN
		--��ȡ����ֵ
		SET @StrFindBegin = CHARINDEX('2:',@strAwardValue)
		SET @StrFindEnd = CHARINDEX(';',@strAwardValue,@StrFindBegin)
		--����У��
		IF @StrFindBegin=0
		BEGIN
			SET @strErrorDescribe = N'��Ǹ,������û��������ʯ����,����ϵ������Ա�������ã�'
			return 5
		END
		--�ַ�����
		IF @StrFindEnd=0 SET @StrLen=LEN(@strAwardValue)-@StrFindBegin+1
		ELSE SET @StrLen=@StrFindEnd-@StrFindBegin
		
		--ת������ֵ
		SET @AwardIngot = CAST(SUBSTRING(@strAwardValue,@StrFindBegin+2,@StrLen-2) AS BIGINT);
		
		UPDATE  WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@AwardIngot 
		WHERE UserID=@dwUserID	
	END

		
	-- ���½��
	IF @AwardType&0x01>0
	BEGIN
		--��ȡ����ֵ	
		SET @StrFindBegin = CHARINDEX('1:',@strAwardValue)
		SET @StrFindEnd = CHARINDEX(';',@strAwardValue,@StrFindBegin)
		--����У��
		IF @StrFindBegin=0
		BEGIN
			SET @strErrorDescribe = N'��Ǹ,������û�����ý�ҽ���,����ϵ������Ա�������ã�'
			return 5
		END
		
		--�ַ�����
		IF @StrFindEnd=0 SET @StrLen=LEN(@strAwardValue)-@StrFindBegin+1
		ELSE SET @StrLen=@StrFindEnd-@StrFindBegin
		
		--ת������ֵ
		SET @AwardGot = CAST(SUBSTRING(@strAwardValue,@StrFindBegin+2,@StrLen-2) AS BIGINT);
		
		UPDATE  WHQJTreasureDB.dbo.GameScoreInfo SET Score=Score+@AwardGot 
		WHERE UserID=@dwUserID
	END
	--���½���
	IF @AwardType&0x04>0
	BEGIN
		--��ȡ����ֵ	
		SET @StrFindBegin = CHARINDEX('3:',@strAwardValue)
		--����У��
		IF @StrFindBegin=0
		BEGIN
			SET @strErrorDescribe = N'��Ǹ,������û�����ý�������,����ϵ������Ա�������ã�'
			return 5
		END
		
		--�ַ�����
		SET @StrLen=LEN(@strAwardValue)-@StrFindBegin+1
		
		--ת������ֵ
		SET @AwardTicket = CAST(SUBSTRING(@strAwardValue,@StrFindBegin+2,@StrLen-2) AS INT);
		
		UPDATE  WHQJTreasureDB.dbo.UserCurrency SET AwardTicket=AwardTicket+@AwardTicket 
		WHERE UserID=@dwUserID
	END
	--��������
	IF @AwardIngot IS NULL SET @AwardIngot=0
	IF @AwardGot IS NULL SET @AwardGot=0
	IF @AwardTicket IS NULL SET @AwardTicket=0

	-- ��ѯ����
	DECLARE @UserScore BIGINT
	DECLARE @UserIngot BIGINT
	DECLARE @UserTicket BIGINT

	-- ��ѯ���
	SELECT @UserScore=Score FROM   WHQJTreasureDB.dbo.GameScoreInfo 
	WHERE UserID=@dwUserID				

	-- ��ѯԪ��
	SELECT @UserIngot=Diamond,@UserTicket=AwardTicket FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwUserID

	
	-- ��������
	IF @UserScore IS NULL SET @UserScore=0		
	IF @UserIngot IS NULL SET @UserIngot=0
	IF @UserTicket IS NULL SET @UserTicket=0
	-- ��������״̬	
	UPDATE AccountsTask SET TaskStatus=2 WHERE ID=@dwID
	
	-- �����¼
	INSERT WHQJRecordDB.dbo.RecordTask(DateID,UserID,TaskID,TimeType,TaskAward,CollectDate)
	VALUES (CAST(CAST(@TaskInputDate AS FLOAT) AS INT),@dwUserID,@wTaskID,@TimeType,@strAwardValue,GetDate())

	-- ����ʱ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
	IF @AwardType&0x02>0
	BEGIN
		-- Ԫ����¼
		INSERT INTO RecordMedalChange(UserID,SrcMedal,TradeMedal,TypeID,ClientIP,CollectDate)	
		VALUES (@dwUserID,@CurrMedal,@AwardIngot,1,@strClientIP,GETDATE())
	END
	
	IF @AwardType&0x04>0
	BEGIN
		-- ������¼
		INSERT INTO WHQJRecordDB.dbo.RecordAwardTicketSerial(SerialNumber,MasterID,UserID,TypeID,CurAwardTicket,ChangeAwardTicket,ClientIP,CollectDate)	
		VALUES (dbo.WF_GetSerialNumber(),0,@dwUserID,2,@UserTicket-@AwardTicket,@AwardTicket,@strClientIP,GETDATE())
	END
	-- ��ˮ��
	IF @AwardType&0x01>0
	BEGIN
		INSERT INTO  WHQJTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
		VALUES (@dwUserID,@CurrScore,@CurrInsure,@AwardGot,7,@strClientIP,GETDATE())
	
		-- ��ͳ��
		UPDATE  WHQJTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@AwardGot WHERE UserID=@dwUserID AND TypeID=7
		IF @@ROWCOUNT=0
		BEGIN
			INSERT  WHQJTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,7,1,@AwardGot)
		END	
	END
		
	-- �ɹ���ʾ
	SET @strErrorDescribe = N'��ϲ����������ȡ�ɹ���'

	-- �׳�����
	SELECT @UserScore AS Score,@UserIngot AS Ingot,@UserTicket AS AwardTicket
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
