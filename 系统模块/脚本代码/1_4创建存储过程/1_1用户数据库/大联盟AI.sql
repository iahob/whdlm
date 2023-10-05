
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

--����AI����
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AddAIParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AddAIParameter]
GO

--�޸�AI����
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyAIParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyAIParameter]
GO

--ɾ��AI����
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_DeleteAIParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_DeleteAIParameter]
GO


--����AI����
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAIParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAIParameter]
GO

--����AI���
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAIUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAIUser]
GO


--����AI���
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UNLOCK_AI]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UNLOCK_AI]
GO

--AI�˳����ֲ�
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AI_QUIT_GROUP]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AI_QUIT_GROUP]
GO

--���ص���AI
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadSingleAI]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadSingleAI]
GO

--���AI
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ClearAI]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ClearAI]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ����AI����
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
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ			
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	--�ж�����ʣ������Ƿ��㹻����
	DECLARE @GroupTotalScore BIGINT
	SELECT @GroupTotalScore = Score FROM WHQJGroupDB.dbo.IMGroupWealth WHERE GroupID = @GroupID

	IF @lDistirbuteTotalScore > @GroupTotalScore
	BEGIN
		SET @strErrorDescribe = N'Ҫ������ѫ������ ' + CONVERT(NVARCHAR(20),@lDistirbuteTotalScore) + N' ��������δ�����ѫ�� ' + CONVERT(NVARCHAR(20),@GroupTotalScore) + '�� ����������'
		RETURN 1
	END



	INSERT INTO GroupAIParameter(GroupID, KindID, AICount, DistributeTotalScore,PlayingAIRateMin,PlayingAIRateMax,
	DistirbuteSingleScore,DistirbuteScoreMinRatio,DistirbuteScoreMaxRatio) 
	VALUES (@GroupID, @wKindID, @dwAICount,@lDistirbuteTotalScore, @dwPlayingAIRateMin, @dwPlayingAIRateMax,
	@lDistirbuteSingleScore, @lDistirbuteScoreMinRatio, @lDistirbuteScoreMaxRatio)

	--����������
	DECLARE @GroupMaster INT
	SELECT @GroupMaster = CreaterID FROM WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID = @GroupID

	--ѡ������ˣ����������������AI�б�
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
			SET @strErrorDescribe = N'�����ӻ���������' + CONVERT(NVARCHAR(20),@InsertAICount) + N'������ָ��AI���� ' + CONVERT(NVARCHAR(20),@dwAICount) + '�� AI�˺��������㣬������'
			RETURN 2
		END

		INSERT INTO GroupAIMember( GroupID, UserID, KindID, PlayStatus) VALUES(@GroupID, @UserID, @wKindID, 0 )

		UPDATE AccountsInfo SET IsGroupMember = 1 WHERE UserID = @UserID

		--����AIΪ�����˳�Ա
		DECLARE @Result INT 
		EXEC @Result = WHQJGroupDB.dbo.GSP_IM_AddGroupMember  @GroupID, @UserID, 0

		
		--select @GroupID as GroupID , @UserID as UserID , @Result as Result
		IF @Result = 0
		BEGIN
			--���һ��ѫ��ֵ
			DECLARE @RandValue BIGINT
			SET @RandValue = @lDistirbuteScoreMinRatio + rand()*( @lDistirbuteScoreMaxRatio - @lDistirbuteScoreMinRatio)
			
			--���ֵ���
			SET @RandValue = @lDistirbuteSingleScore + @lDistirbuteSingleScore * @RandValue/100

			select @RandValue as RandValue1

			--��ȡ�Ѿ������ѫ��
			DECLARE @DistributedScore BIGINT
			SELECT @DistributedScore = SUM(Score) FROM WHQJGroupDB.dbo.IMGroupMember WHERE UserID IN (SELECT UserID FROM GroupAIMember WHERE GroupID = @GroupID)
			IF @DistributedScore + @RandValue> @lDistirbuteTotalScore
			BEGIN

			--select @DistributedScore as DistributedScore, @lDistirbuteTotalScore as DistirbuteTotalScore
			--	SET @RandValue = 0
				--SET @strErrorDescribe = N'��ǰAI�����ܻ��� ' + CONVERT(NVARCHAR(20),@DistributedScore) + N' �����趨AI�ܻ��� ' + CONVERT(NVARCHAR(20),@lDistirbuteTotalScore) + '�� ��������'
				RETURN 0
			END
			select @RandValue as RandValue2
			--��AI����ѫ��
			EXEC @Result = WHQJGroupDB.dbo.GSP_IM_PresentGroupScoreEx @GroupID, @GroupMaster, @UserID, @RandValue, 0
		END

		SET @CountBegin = @CountBegin + 1
	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ����AI����
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
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ			
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	--�ж�����ʣ������Ƿ��㹻����
	DECLARE @GroupTotalScore BIGINT
	SELECT @GroupTotalScore = Score FROM WHQJGroupDB.dbo.IMGroupWealth WHERE GroupID = @GroupID

	IF @lDistirbuteTotalScore > @GroupTotalScore
	BEGIN
		SET @strErrorDescribe = N'Ҫ������ѫ������ ' + CONVERT(NVARCHAR(20),@lDistirbuteTotalScore) + N' �����������ϵ�ѫ�� ' + CONVERT(NVARCHAR(20),@GroupTotalScore) + '�� ����������'
		RETURN 1
	END

	--�жϵ�ǰ�����Ƿ����
	DECLARE @preDistirbuteSingleScore BIGINT
	DECLARE @preDistirbuteTotalScore BIGINT
	DECLARE @preAICout INT
	SELECT @preAICout = AICount , @preDistirbuteTotalScore = DistributeTotalScore, @preDistirbuteSingleScore = DistirbuteSingleScore FROM GroupAIParameter WHERE GroupID = @GroupID AND KindID = @wKindID 
	IF @preDistirbuteTotalScore IS NOT NULL AND @preDistirbuteSingleScore IS NOT NULL
	BEGIN
		--�жϵ�ǰ����AI �Ƿ����ڼ�����
		DECLARE @AICount INT 
		SELECT @AICount = COUNT(*) FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 1
		IF @AICount <> 0 AND (@preDistirbuteSingleScore <> @lDistirbuteTotalScore OR @preDistirbuteTotalScore <> @lDistirbuteSingleScore)
		BEGIN
			SET @strErrorDescribe = N'AI����ʹ���в������޸ĸþ��ֲ�AIЯ���ܷ����Լ�����AIЯ��������'
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
		SET @strErrorDescribe = N'������AI����������������ӣ�'
		RETURN 3
	END



	--����������
	DECLARE @GroupMaster INT
	SELECT @GroupMaster = CreaterID FROM WHQJGroupDB.dbo.IMGroupProperty WHERE GroupID = @GroupID

	--ѡ������ˣ����������������AI�б�
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
				SET @strErrorDescribe = N'�����ӻ���������' + CONVERT(NVARCHAR(20),@InsertAICount) + N'������ָ��AI���� ' + CONVERT(NVARCHAR(20),@dwAICount) + '�� AI�˺��������㣬������'
				RETURN 2
			END

			INSERT INTO GroupAIMember( GroupID, UserID, KindID, PlayStatus) VALUES(@GroupID, @UserID, @wKindID, 0 )

			--����AIΪ�����˳�Ա
			DECLARE @Result INT 
			EXEC @Result = WHQJGroupDB.dbo.GSP_IM_AddGroupMember  @GroupID, @UserID, 0
			IF @Result = 0
			BEGIN


				--���һ��ѫ��ֵ
				DECLARE @RandValue BIGINT
				SET @RandValue = @lDistirbuteScoreMinRatio + rand()*( @lDistirbuteScoreMaxRatio - @lDistirbuteScoreMinRatio)
			
				--���ֵ���
				SET @RandValue = @lDistirbuteSingleScore + @lDistirbuteSingleScore * @RandValue/100

				--��ȡ�Ѿ������ѫ��
				DECLARE @DistributedScore BIGINT
				SELECT @DistributedScore = SUM(Score) FROM WHQJGroupDB.dbo.IMGroupMember WHERE UserID IN (SELECT UserID FROM GroupAIMember WHERE GroupID = @GroupID)
				IF @DistributedScore + @RandValue> @lDistirbuteTotalScore
				BEGIN
					SET @RandValue = 0
				END

				--��AI����ѫ��
				EXEC @Result = WHQJGroupDB.dbo.GSP_IM_PresentGroupScoreEx @GroupID, @GroupMaster, @UserID, @RandValue, 0
			END

			SET @CountBegin = @CountBegin + 1
		END
		
	END

	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ����AI����
CREATE PROC GSP_GP_DeleteAIParameter
	@GroupID					INT,							
	@wKindID					SMALLINT,
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ			
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	--�жϵ�ǰ�����Ƿ����
	DECLARE @preDistirbuteSingleScore BIGINT
	DECLARE @preDistirbuteTotalScore BIGINT
	DECLARE @preAICout INT
	SELECT @preAICout = AICount , @preDistirbuteTotalScore = DistributeTotalScore, @preDistirbuteSingleScore = DistirbuteSingleScore FROM GroupAIParameter WHERE GroupID = @GroupID AND KindID = @wKindID 
	IF @preDistirbuteTotalScore IS NOT NULL AND @preDistirbuteSingleScore IS NOT NULL
	BEGIN
		--�жϵ�ǰ����AI �Ƿ����ڼ�����
		DECLARE @AICount INT 
		SELECT @AICount = COUNT(*) FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 1
		IF @AICount <> 0
		BEGIN
			SET @strErrorDescribe = N'AI����ʹ���в�����ɾ����'
			RETURN 2
		END
	END

	--��ȡAI�ܻ���
	DECLARE @GroupAITotalScore BIGINT
	SELECT @GroupAITotalScore = SUM(Score) FROM WHQJGroupDB.dbo.IMGroupMember WHERE UserID IN (SELECT UserID FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID )

	--��AI���ֻ�������
	UPDATE WHQJGroupDB.dbo.IMGroupWealth  SET Score = Score + @GroupAITotalScore  WHERE GroupID = @GroupID

	--��AI�˺���Ϊ δ����װ��
	UPDATE AccountsInfo SET IsGroupMember = 0 WHERE UserID IN (SELECT UserID FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID )

	--ɾ��AI�˺�
	DELETE  FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID 

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
-- ��ȡ����
CREATE PROC GSP_GP_LoadAIParameter

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	--ѡ�����״̬���ֲ���AI����
	SELECT * FROM GroupAIParameter WHERE GroupID IN (SELECT GroupID FROM WHQJGroupDB.dbo.IMGroupProperty WHERE GroupStatus = 1 )

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------
------------����AI���
-- ��ȡ����
CREATE PROC GSP_GP_LoadAIUser
	@GroupID					INT,							
	@wKindID					SMALLINT,
	@dwAICount					INT,
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	DECLARE @UserID INT

	--���������
	DECLARE @temp TABLE
	(
	UserID INT
	);

	-- ��Դ���е����ݲ��뵽�������
	INSERT INTO @temp(UserID)
	SELECT TOP(@dwAICount) UserID FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 0 

	--ѡ�����״̬���ֲ���AI����
	--SELECT TOP(@dwAICount)* FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 0 
	SELECT UserID, Score  FROM WHQJGroupDB.dbo.IMGroupMember WHERE GroupID = @GroupID AND UserID IN (SELECT UserID  FROM @temp)

	-- ��ѡ�е�AI��Ϊ �Ѿ�ʹ��״̬
	UPDATE GroupAIMember SET PlayStatus = 1 WHERE  UserID IN (SELECT UserID  FROM @temp)

	-- ��ѡ�е�AI��Ϊ �Ѿ����������״̬
	UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = 1 WHERE GroupID = @GroupID AND UserID IN (SELECT UserID  FROM @temp)
END

RETURN 0

GO

GO
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
------------����AI���
CREATE PROC GSP_GP_UNLOCK_AI
	@GroupID					INT,							
	@wKindID					SMALLINT,
	@dwUserID					INT
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѡ�е�AI��Ϊ δʹ��״̬
	UPDATE GroupAIMember SET PlayStatus = 0 WHERE GroupID = @GroupID AND KindID = @wKindID AND UserID = @dwUserID

	-- �ı���ֲ���Ա״̬
	UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = 0 WHERE GroupID = @GroupID AND UserID  = @dwUserID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
------------����AI���
CREATE PROC GSP_GP_AI_QUIT_GROUP
	@GroupID					INT,							
	@wKindID					SMALLINT,
	@dwUserID					INT
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	DECLARE @UserScore BIGINT	--ѫ��
	SELECT @UserScore = Score FROM WHQJGroupDB.dbo.IMGroupMember WHERE GroupID = @GroupID AND UserID  = @dwUserID

	-- ��ѡ�е�AI��Ϊ δʹ��״̬
	DELETE FROM GroupAIMember  WHERE GroupID = @GroupID AND KindID = @wKindID AND UserID = @dwUserID

	-- �ı���ֲ���Ա״̬
	DELETE FROM WHQJGroupDB.dbo.IMGroupMember WHERE GroupID = @GroupID AND UserID  = @dwUserID

	-- ��ȡȺ����Ϣ
	DECLARE @strGroupIDArray NVARCHAR(1024)
	SELECT @strGroupIDArray=GroupIDArray FROM WHQJGroupDB.dbo.IMUserGroupInfo WHERE UserID=@dwUserID

	-- �޸�Ⱥ����Ϣ
	DECLARE @strGroupID NVARCHAR(20)		
	SET @strGroupID=';'+CONVERT(NVARCHAR(18),@GroupID)+';';
	SET @strGroupIDArray=REPLACE(@strGroupIDArray,@strGroupID,';');

	-- ����Ⱥ��
	UPDATE WHQJGroupDB.dbo.IMUserGroupInfo SET GroupIDArray=@strGroupIDArray WHERE UserID=@dwUserID		

	-- ��������
	UPDATE WHQJGroupDB.dbo.IMGroupProperty SET MemberCount=MemberCount-1 WHERE GroupID=@GroupID

	--����ɾ����¼
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
-- ��ȡ����
CREATE PROC GSP_GP_LoadSingleAI
	@GroupID					INT,							
	@wKindID					SMALLINT,
	@dwAICount					INT,
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	DECLARE @UserID INT

	--���������
	DECLARE @temp TABLE
	(
	UserID INT
	);

	-- ��Դ���е����ݲ��뵽�������
	INSERT INTO @temp(UserID)
	SELECT TOP(@dwAICount) UserID FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 0 

	--ѡ�����״̬���ֲ���AI����
	SELECT TOP(@dwAICount)* FROM GroupAIMember WHERE GroupID = @GroupID AND KindID = @wKindID AND PlayStatus = 0 

	-- ��ѡ�е�AI��Ϊ �Ѿ�ʹ��״̬
	UPDATE GroupAIMember SET PlayStatus = 1 WHERE  UserID IN (SELECT UserID  FROM @temp)

	-- ��ѡ�е�AI��Ϊ �Ѿ����������״̬
	UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = 1 WHERE GroupID = @GroupID AND UserID IN (SELECT UserID  FROM @temp)

END

RETURN 0

GO
-----------------------------------------------------------------------------------------------------
------------����AI���
CREATE PROC GSP_GP_ClearAI

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѡ�е�AI��Ϊ δʹ��״̬
	UPDATE GroupAIMember SET PlayStatus = 0 

	-- �ı���ֲ���Ա״̬
	UPDATE WHQJGroupDB.dbo.IMGroupMember SET UserStatus = 0 WHERE  UserID IN (SELECT UserID  FROM GroupAIMember)
END

RETURN 0

GO

-----------------------------------------------------------------------------------------------------

