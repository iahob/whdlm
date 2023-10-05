
----------------------------------------------------------------------------------------------------

USE WHQJGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WF_SplitStr]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[WF_SplitStr]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadGameMatchItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadGameMatchItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_DeleteGameMatchItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_DeleteGameMatchItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_InsertGameMatchItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_InsertGameMatchItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ModifyGameMatchItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_ModifyGameMatchItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadMatchReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadMatchReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ����ַ���
CREATE FUNCTION [dbo].[WF_SplitStr]
(
	@strSource NVARCHAR(1024),		--Դ�ַ���
	@strSeparator CHAR(1)			--�ָ�����
)
RETURNS @tbResult TABLE(id INT IDENTITY(1,1),rs NVARCHAR(1000))
WITH ENCRYPTION AS  
BEGIN 
   DECLARE @dwIndex INT ,@strResult NVARCHAR(1000);
   SET @strSource = RTRIM(LTRIM(@strSource)); -- ���ո�
   SET @dwIndex = CHARINDEX(@strSeparator,@strSource);	-- ȡ�õ�һ���ָ�����λ��

   WHILE @dwIndex>0
   BEGIN 
      SET @strResult = LTRIM(RTRIM(LEFT(@strSource,@dwIndex-1)));
--      IF @strResult IS NULL OR @strResult = '' OR @strResult = @strSeparator
--      BEGIN 
--        SET @strSeparator = SUBSTRING(@strSource,@dwIndex+1,LEN(@strSource)-@dwIndex); --��Ҫ�������ַ���ȥ�����зֲ���
--        SET @dwIndex = CHARINDEX(@strSeparator,@strSource); --ѭ��������
--		CONTINUE;
--      END 

      INSERT @tbResult VALUES(@strResult);
      SET @strSource = SUBSTRING(@strSource,@dwIndex+1,LEN(@strSource)-@dwIndex); --��Ҫ�������ַ���ȥ�����зֲ���
      SET @dwIndex=CHARINDEX(@strSeparator,@strSource); --ѭ��������
   END 

   --�������һ��
   IF  @strSource IS NOT NULL AND LTRIM(RTRIM(@strSource)) <> '' AND @strSource <> @strSeparator 
   BEGIN 
      INSERT @tbResult VALUES(@strSource)
   END 

   RETURN
END

GO
----------------------------------------------------------------------------------------------------

-- ���ر���
CREATE  PROCEDURE dbo.GSP_GS_LoadGameMatchItem
	@wKindID   INT,								-- ���ͱ�ʶ
	@dwMatchID INT,								-- ������ʶ	
	@strServiceMachine NCHAR(32),				-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ���ر���
	SELECT * FROM MatchPublic WHERE ((KindID=@wKindID AND MatchID=@dwMatchID) OR (@dwMatchID=0 AND KindID=@wKindID AND ServiceMachine=@strServiceMachine)) AND Nullity=0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ������
CREATE  PROCEDURE dbo.GSP_GS_DeleteGameMatchItem	
	@dwMatchID INT								-- ������ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	DECLARE @cbMatchType SMALLINT
	SELECT @cbMatchType=MatchType FROM MatchPublic WHERE MatchID=@dwMatchID

	-- �����ж�
	IF @cbMatchType IS NULL RETURN 1
	
	-- ɾ������
	DELETE MatchPublic WHERE MatchID=@dwMatchID

	-- ɾ������	
	DELETE MatchReward WHERE MatchID=@dwMatchID				
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �������
CREATE  PROCEDURE dbo.GSP_GS_InsertGameMatchItem	
	-- ������Ϣ
	@wKindID INT,								-- ���ͱ�ʶ		
	@strMatchName NVARCHAR(32),					-- ��������
	@strMatchDate NVARCHAR(64),					-- ����ʱ��	
	@cbMatchType TINYINT,						-- ��������		
	
	-- ������Ϣ
	@cbFeeType TINYINT,							-- ��������
	@cbDeductArea TINYINT,						-- �۷�����
	@lSignupFee BIGINT,							-- ��������
	@cbSignupMode TINYINT,						-- ������ʽ
	@cbJoinCondition TINYINT,					-- ��������
	@cbMemberOrder TINYINT,						-- ��Ա�ȼ�	
	@lExperience INT,							-- ����ȼ�	
	@dwFromMatchID INT,							-- ������ʶ	
	@cbFilterType TINYINT,						-- ɸѡ��ʽ
	@wMaxRankID SMALLINT,						-- �������
	@MatchEndDate DATETIME,						-- ��ʼ����
	@MatchStartDate DATETIME,					-- ��ʼ����
	
	-- ������Ϣ
	@cbRankingMode TINYINT,						-- ������ʽ
	@wCountInnings SMALLINT,					-- ͳ�ƾ���
	@cbFilterGradesMode TINYINT,				-- ɸѡ��ʽ

	-- ������Ϣ
	@cbDistributeRule TINYINT,					-- �������
	@wMinDistributeUser SMALLINT,				-- ��������
	@wDistributeTimeSpace SMALLINT,				-- ������
	@wMinPartakeGameUser SMALLINT,				-- ��С����
	@wMaxPartakeGameUser SMALLINT,				-- �������

	-- ��������
	@strRewardGold NVARCHAR(512),				-- ��ҽ���
	@strRewardIngot NVARCHAR(512),				-- Ԫ������
	@strRewardExperience NVARCHAR(512),			-- ���齱��

	-- ��չ����
	@strMatchRule NVARCHAR(1024),				-- ��������	
	@strServiceMachine NCHAR(32),				-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	INSERT INTO MatchPublic(KindID,MatchName,MatchType,SignupMode,FeeType,SignupFee,DeductArea,JoinCondition,MemberOrder,
		Experience,FromMatchID,FilterType,MaxRankID,MatchEndDate,MatchStartDate,RankingMode,CountInnings,FilterGradesMode,DistributeRule,MinDistributeUser,
		DistributeTimeSpace,MinPartakeGameUser,MaxPartakeGameUser,MatchRule,ServiceMachine,CollectDate)		
	VALUES(@wKindID,@strMatchName,@cbMatchType,@cbSignupMode,@cbFeeType,@lSignupFee,@cbDeductArea,@cbJoinCondition,@cbMemberOrder,
		@lExperience,@dwFromMatchID,@cbFilterType,@wMaxRankID,@MatchEndDate,@MatchStartDate,@cbRankingMode,@wCountInnings,@cbFilterGradesMode,@cbDistributeRule,@wMinDistributeUser,
		@wDistributeTimeSpace,@wMinPartakeGameUser,@wMaxPartakeGameUser,@strMatchRule,@strServiceMachine,GETDATE())

	-- ������ʶ
	DECLARE @dwMatchID INT
	SET @dwMatchID=SCOPE_IDENTITY()	
	
	-- �����ж�
	IF NOT EXISTS(SELECT * FROM MatchPublic WHERE MatchID=@dwMatchID) 
	BEGIN
		SET @strErrorDescribe=N'�������ݿ��쳣����������ʧ�ܣ����Ժ����ԣ�'
		RETURN 1
	END
	
	-- ��ʱ��״̬
	IF @cbMatchType=1
	BEGIN
		-- �޸�״̬
		UPDATE MatchPublic SET MatchStatus=0x02 WHERE MatchID=@dwMatchID		
	END
	
	-- ������Ϣ
	INSERT INTO dbo.MatchInfo(MatchID,MatchName,MatchDate) VALUES (@dwMatchID,@strMatchName,@strMatchDate)

	-- ��ֽ���
	SELECT * INTO #tmpGold FROM dbo.WF_SplitStr(@strRewardGold,'|')	
	SELECT * INTO #tmpIngot FROM dbo.WF_SplitStr(@strRewardIngot,'|')	
	SELECT * INTO #tmpExperience FROM dbo.WF_SplitStr(@strRewardExperience,'|')		

	-- ��������
	DECLARE @Index INT
	DECLARE @RewardGold BIGINT
	DECLARE @RewardIngot BIGINT
	DECLARE @RewardExperience BIGINT

	-- ���뽱��
	SELECT @Index=COUNT(id) FROM #tmpGold
	WHILE @Index > 0
	BEGIN
		-- ��ѯ����
		SELECT @RewardGold=rs FROM #tmpGold WHERE id=@Index		
		SELECT @RewardIngot=rs FROM #tmpIngot WHERE id=@Index
		SELECT @RewardExperience=rs FROM #tmpExperience WHERE id=@Index					

		-- ��������
		INSERT INTO MatchReward(MatchID,MatchRank,RewardGold,RewardIngot,RewardExperience,RewardDescibe)
		VALUES(@dwMatchID,@Index,@RewardGold,@RewardIngot,@RewardExperience,'')

		SET @Index=@Index-1
	END

	-- ɾ��ʱ��
	DROP TABLE #tmpGold,#tmpIngot,#tmpExperience

	-- ��ѯ����
	EXEC GSP_GS_LoadGameMatchItem @wKindID,@dwMatchID,@strServiceMachine,N''				
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸ı���
CREATE  PROCEDURE dbo.GSP_GS_ModifyGameMatchItem
	-- ������Ϣ
	@wKindID INT,								-- ���ͱ�ʶ
	@dwMatchID INT,								-- ������ʶ
	@strMatchName NVARCHAR(32),					-- ��������
	@strMatchDate NVARCHAR(64),					-- ����ʱ��
	@cbMatchType TINYINT,						-- ��������
	
	-- ������Ϣ
	@cbFeeType TINYINT,							-- ��������
	@cbDeductArea TINYINT,						-- �۷�����
	@lSignupFee BIGINT,							-- ��������
	@cbSignupMode TINYINT,						-- ������ʽ
	@cbJoinCondition TINYINT,					-- ��������
	@cbMemberOrder TINYINT,						-- ��Ա�ȼ�	
	@lExperience INT,							-- ����ȼ�	
	@dwFromMatchID INT,							-- ������ʶ	
	@cbFilterType TINYINT,						-- ɸѡ��ʽ
	@wMaxRankID SMALLINT,						-- �������
	@MatchEndDate DATETIME,						-- ��������
	@MatchStartDate DATETIME,					-- ��ʼ����
	
	-- ������Ϣ
	@cbRankingMode TINYINT,						-- ������ʽ
	@wCountInnings SMALLINT,					-- ͳ�ƾ���
	@cbFilterGradesMode TINYINT,				-- ɸѡ��ʽ

	-- ������Ϣ
	@cbDistributeRule TINYINT,					-- �������
	@wMinDistributeUser SMALLINT,				-- ��������
	@wDistributeTimeSpace SMALLINT,				-- ������
	@wMinPartakeGameUser SMALLINT,				-- ��С����
	@wMaxPartakeGameUser SMALLINT,				-- �������

	-- ��������
	@strRewardGold NVARCHAR(1024),				-- ��ҽ���
	@strRewardIngot NVARCHAR(1024),				-- Ԫ������
	@strRewardExperience NVARCHAR(512),			-- ���齱��

	-- ��������
	@strMatchRule NVARCHAR(1024),				-- ��������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	UPDATE MatchPublic 	SET KindID=@wKindID,MatchName=@strMatchName,MatchType=@cbMatchType,SignupFee=@lSignupFee,
	SignupMode=@cbSignupMode,FeeType=@cbFeeType,DeductArea=@cbDeductArea,JoinCondition=@cbJoinCondition,MemberOrder=@cbMemberOrder,Experience=@lExperience,FromMatchID=@dwFromMatchID,
	FilterType=@cbFilterType,MaxRankID=@wMaxRankID,MatchEndDate=@MatchEndDate,MatchStartDate=@MatchStartDate,RankingMode=@cbRankingMode,CountInnings=@wCountInnings,FilterGradesMode=@cbFilterGradesMode,
	DistributeRule=@cbDistributeRule,MinDistributeUser=@wMinDistributeUser,DistributeTimeSpace=@wDistributeTimeSpace,MinPartakeGameUser=@wMinPartakeGameUser,MaxPartakeGameUser=@wMaxPartakeGameUser,MatchRule=@strMatchRule
	WHERE MatchID=@dwMatchID	
	
	-- ��������
	UPDATE MatchInfo SET MatchName=@strMatchName,MatchDate=@strMatchDate WHERE MatchID=@dwMatchID
	IF @@ROWCOUNT=0
	BEGIN
		-- �������	
		INSERT INTO dbo.MatchInfo(MatchID,MatchName,MatchDate) VALUES (@dwMatchID,@strMatchName,@strMatchDate)
	END

	-- ɾ������
	DELETE MatchReward WHERE MatchID=@dwMatchID	

	-- ��ֽ���
	SELECT * INTO #tmpGold FROM dbo.WF_SplitStr(@strRewardGold,'|')	
	SELECT * INTO #tmpIngot FROM dbo.WF_SplitStr(@strRewardIngot,'|')	
	SELECT * INTO #tmpExperience FROM dbo.WF_SplitStr(@strRewardExperience,'|')		

	-- ��������
	DECLARE @Index INT
	DECLARE @RewardGold BIGINT
	DECLARE @RewardIngot BIGINT
	DECLARE @RewardExperience BIGINT

	-- ���뽱��
	SELECT @Index=COUNT(id) FROM #tmpGold
	WHILE @Index > 0
	BEGIN
		-- ��ѯ����
		SELECT @RewardGold=rs FROM #tmpGold WHERE id=@Index		
		SELECT @RewardIngot=rs FROM #tmpIngot WHERE id=@Index
		SELECT @RewardExperience=rs FROM #tmpExperience WHERE id=@Index					

		-- ��������
		INSERT INTO MatchReward(MatchID,MatchRank,RewardGold,RewardIngot,RewardExperience,RewardDescibe)
		VALUES(@dwMatchID,@Index,@RewardGold,@RewardIngot,@RewardExperience,'')

		SET @Index=@Index-1
	END

	-- ɾ��ʱ��
	DROP TABLE #tmpGold,#tmpIngot,#tmpExperience	

	-- ��ѯ����
	EXEC GSP_GS_LoadGameMatchItem @wKindID,@dwMatchID,N'',N''
			
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ؽ���
CREATE  PROCEDURE dbo.GSP_GS_LoadMatchReward
	@dwMatchID INT								-- ������ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ؽ���
	SELECT * FROM MatchReward WHERE MatchID=@dwMatchID 

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------