USE WHQJGameMatchDB
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchQueryQualify]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchQueryQualify]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchBuySafeCard]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchBuySafeCard]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserMatchFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserMatchFee]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserMatchQuit]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserMatchQuit]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchQueryReviveInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchQueryReviveInfo]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchUserRevive]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchUserRevive]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ѯ�ʸ�
CREATE PROC GSP_GR_MatchQueryQualify
	@dwUserID INT,								-- �û� I D
	@dwMatchID	INT,							-- ���� I D
	@lMatchNo	BIGINT							-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	
	-- ��ѯ���
	IF NOT EXISTS(SELECT * FROM MatchPromoteInfo(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID)
	BEGIN
		RETURN 1	
	END	
	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- ������
CREATE PROC GSP_GR_MatchBuySafeCard
	@dwUserID INT,								-- �û� I D
	@dwMatchID	INT,							-- ���� I D
	@lMatchNo	BIGINT,							-- ��������
	@lSafeCardFee INT,							-- ���շ���		
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	
	-- ��ѯ���
	DECLARE @HoldSafeCard TINYINT
	SELECT @HoldSafeCard=HoldSafeCard FROM MatchReviveInfo(NOLOCK) WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- ��������
	IF @HoldSafeCard IS NULL 
	BEGIN
		SET @HoldSafeCard=0 
		
		-- �����¼
		INSERT INTO MatchReviveInfo(UserID, MatchID, MatchNo, HoldSafeCard, ReviveFee, ReviveTimes, CollectDate) 
		VALUES (@dwUserID, @dwMatchID, @lMatchNo, @HoldSafeCard, 0, 0, GETDATE())
	END
	
	-- �Ѿ�����
	IF @HoldSafeCard=1
	BEGIN
		SET @strErrorDescribe=N'���α������ѹ����տ�������Ҫ�ظ�����'
		RETURN 1
	END	
	
	-- ��ѯ���
	DECLARE @Score BIGINT
	SELECT @Score=Score FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID
	
	-- �ж����
	IF @Score<@lSafeCardFee
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ͻ�����㣬����ʧ�ܣ�'
		RETURN 2	
	END
	
	-- �۳����
	UPDATE WHQJTreasureDB..GameScoreInfo SET Score=Score-@lSafeCardFee WHERE UserID=@dwUserID
	
	-- ����״̬
	UPDATE MatchReviveInfo SET HoldSafeCard=1 WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- ��ѯ���
	DECLARE @CurrScore INT
	SELECT @CurrScore=Score FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID
	
	-- �׳�����
	SELECT @CurrScore AS Score
END

RETURN 0
GO


----------------------------------------------------------------------------------------------------

-- ��Ҹ���
CREATE PROC GSP_GR_MatchUserRevive
	@dwUserID INT,								-- �û� I D
	@dwMatchID	INT,							-- ���� I D	
	@lMatchNo	BIGINT,							-- ��������
	@wServerID	INT,							-- ���� I D
	@InitScore  BIGINT,							-- ��ʼ����	
	@CullScore  BIGINT,							-- ��̭����	
	@lReviveFee INT,							-- �������			
	@cbReviveTimes TINYINT,						-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	
	-- ��ѯ����
	DECLARE @MatchScore BIGINT
	SELECT @MatchScore=Score FROM MatchScoreInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo		
	
	-- �жϷ���
	IF @MatchScore > @CullScore
	BEGIN
		SET @strErrorDescribe=N'���ķ���δ�ﵽ��̭�����ߣ�����Ҫ���'	
		RETURN 1			
	END
	
	-- ��ѯ���
	DECLARE @HoldSafeCard TINYINT
	DECLARE @UseSafeCard TINYINT
	DECLARE @ReviveTimes TINYINT
	SELECT @HoldSafeCard=HoldSafeCard, @UseSafeCard=UseSafeCard, @ReviveTimes=ReviveTimes FROM MatchReviveInfo(NOLOCK) 
	WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- ��������
	IF @HoldSafeCard IS NULL 
	BEGIN
		-- ���ñ���				
		SET @ReviveTimes=0
		SET @UseSafeCard=0		
		SET @HoldSafeCard=0
		
		-- �����¼
		INSERT INTO MatchReviveInfo(UserID, MatchID, MatchNo, HoldSafeCard, UseSafeCard, ReviveFee, ReviveTimes, CollectDate) 
		VALUES (@dwUserID, @dwMatchID, @lMatchNo, 0, 0, 0, 0, GETDATE())
	END
	
	-- ӵ�б��տ���δʹ��
	IF @HoldSafeCard=1 AND @UseSafeCard=0
	BEGIN
		-- ����״̬
		UPDATE MatchReviveInfo SET UseSafeCard=1 WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo					
	END ELSE
	BEGIN
		-- �жϴ���
		IF @ReviveTimes>=@cbReviveTimes
		BEGIN
			SET @strErrorDescribe=N'��Ǹ������������������� ' +CONVERT(nvarchar(20),@cbReviveTimes)+N' �Σ�����ʧ�ܣ�'	
			RETURN 2					 
		END 
		
		-- ��ѯ���
		DECLARE @Score BIGINT
		SELECT @Score=Score FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID
		
		-- �жϽ��
		IF @Score<@lReviveFee
		BEGIN
			SET @strErrorDescribe=N'��Ǹ�������Ͻ�����㣬����ʧ�ܣ�'
			RETURN 5
		END
	
		-- �۳����
		UPDATE WHQJTreasureDB..GameScoreInfo SET Score=Score-@lReviveFee WHERE UserID=@dwUserID
		
		-- ����״̬
		UPDATE MatchReviveInfo SET ReviveFee=ReviveFee+@lReviveFee,ReviveTimes=ReviveTimes+1 
		WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo		
	END
	
	-- ���·���
	UPDATE MatchScoreInfo SET Score=@InitScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,OnlineTime=0,MatchRight=0x10000000,SignupTime=GetDate()
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo	
	
	-- ��ѯ���
	DECLARE @CurrScore INT
	SELECT @CurrScore=Score FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID
	
	-- �׳�����
	SELECT @CurrScore AS Score
END

RETURN 0
GO


----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GR_MatchQueryReviveInfo
	@dwUserID INT,								-- �û� I D
	@dwMatchID	INT,							-- ���� I D
	@lMatchNo	BIGINT							-- ��������	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	
	-- ��ѯ���
	DECLARE @UseSafeCard TINYINT
	DECLARE @HoldSafeCard TINYINT	
	DECLARE @ReviveTimes TINYINT
	SELECT @HoldSafeCard=HoldSafeCard, @UseSafeCard=UseSafeCard, @ReviveTimes=ReviveTimes FROM MatchReviveInfo(NOLOCK) 
	WHERE UserID=@dwUserID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	
	-- ��������
	IF @HoldSafeCard IS NULL 
	BEGIN
		-- ���ñ���		
		SET @ReviveTimes=0
		SET @UseSafeCard=0
		SET @HoldSafeCard=0
	END
	
	-- ��������
	DECLARE @SafeCardRevive TINYINT
	SET @SafeCardRevive=1
	
	-- ��������
	IF @HoldSafeCard=0 SET @SafeCardRevive=0
	IF @HoldSafeCard=1 AND @UseSafeCard=1 SET @SafeCardRevive=0
	
	-- �׳�����
	SELECT @SafeCardRevive AS SafeCardRevive, @ReviveTimes AS ReviveTimes
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- �۳�����
CREATE PROC GSP_GR_UserMatchFee
	@dwUserID INT,								-- �û� I D	
	@InitScore BIGINT,							-- ��ʼ����
	@cbSignupMode TINYINT,						-- ������ʽ	
	@wMaxSignupUser SMALLINT,					-- �������
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@dwMatchID	INT,							-- ���� I D
	@lMatchNo	BIGINT,							-- ��������
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	-- ������Ϣ
	DECLARE @FeeType TINYINT
	DECLARE @SignupFee INT
	DECLARE @Experience INT	
	DECLARE @SignupMode TINYINT	
	DECLARE @MatchType TINYINT
	DECLARE @MemberOrder TINYINT
	DECLARE @FromMatchID INT		
	DECLARE @JoinCondition TINYINT
	
	-- ��ѯ����	
	SELECT @FeeType=FeeType, @SignupFee=SignupFee, @MatchType=MatchType, @Experience=Experience, @JoinCondition=JoinCondition, 
	@SignupMode=SignupMode, @FromMatchID=FromMatchID, @MemberOrder=MemberOrder FROM MatchPublic(NOLOCK) WHERE MatchID=@dwMatchID

	-- �����ж�
	IF @MatchType IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,�������ı��������ڣ�'
		return 1		
	END
	
	-- ��ѯ�û�
	DECLARE @lExperience INT
	DECLARE @cbMemberOrder TINYINT
--	SELECT @lExperience=Experience, @cbMemberOrder=MemberOrder FROM WHQJAccountsDB..AccountsInfo WHERE UserID=@dwUserID
	
	-- �����ж�
	--IF @lExperience IS NULL
	--BEGIN
	--	SET @strErrorDescribe = N'��Ǹ,�����û���Ϣ�����ڣ�����ϵ�ͷ����Ĺ�����Ա�˽������'
	--	return 1	 
	--END	
	
	-- ��Ա�ȼ�
	--IF (@JoinCondition&0x01)<>0 AND @cbMemberOrder<@MemberOrder
	--BEGIN
	--	SET @strErrorDescribe = N'��Ǹ�����Ļ�Ա�ȼ��ﲻ����������������ʧ�ܣ�'
	--	return 2				
	--END
	
	-- �û�����
	--IF (@JoinCondition&0x02)<>0 AND @lExperience<@Experience
	--BEGIN
	--	SET @strErrorDescribe = N'��Ǹ�����ľ���ֵ�ﲻ����������������ʧ�ܣ�'
	--	return 2				
	--END
	
	-- ��������
	DECLARE @MaxSignupUser SMALLINT
	SELECT @MaxSignupUser=COUNT(*) FROM MatchScoreInfo WHERE MatchID=@dwMatchID AND MatchNo=@lMatchNo AND ServerID=@wServerID 
	
	-- �ж�����
	IF @wMaxSignupUser>0 AND @MaxSignupUser>=@wMaxSignupUser
	BEGIN
		SET @strErrorDescribe = N'��Ǹ�����������Ѵﵽ���������������ʧ�ܣ�'
		return 3	
	END
	
	-- �û���Ϣ
	DECLARE @IsAndroidUser TINYINT
	SET @IsAndroidUser=0
	
	-- �����ж�
	IF @strClientIP=N'0.0.0.0' SET @IsAndroidUser=1

	-- �۳�������
	IF (@SignupMode&@cbSignupMode)<>0 AND @cbSignupMode=1		
	BEGIN				
		-- �ǻ������	
		IF @IsAndroidUser = 0	
		BEGIN	
			-- �۳�����
			IF @SignupFee > 0
			BEGIN				
				-- �۳����
				IF @FeeType = 0
				BEGIN
					-- ��ѯ���
					DECLARE @Score BIGINT
					SELECT @Score=Score FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID
					
					-- �������
					IF @Score IS NULL SET @Score=0

					-- ��Ҳ���
					IF @Score < @SignupFee		
					BEGIN
						SET @strErrorDescribe = N'��Ǹ,�����ϵ���Ϸ�Ҳ���,ϵͳ�޷�Ϊ���ɹ�������'
						return 4				
					END

					-- ���½��
					UPDATE WHQJTreasureDB..GameScoreInfo SET Score=@Score-@SignupFee WHERE UserID=@dwUserID					
				END
				
				-- �۳���ʯ
				IF @FeeType = 1
				BEGIN
					-- ��ѯ��ʯ
					DECLARE @UserDimond BIGINT
					SELECT @UserDimond=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID
					
					-- ��������
					IF @UserDimond IS NULL SET @UserDimond=0

					-- ��ʯ����
					IF @UserDimond < @SignupFee		
					BEGIN
						SET @strErrorDescribe = N'��Ǹ,�����ϵ���ʯ����,ϵͳ�޷�Ϊ��������'
						return 4				
					END

					-- ������ʯ
					UPDATE WHQJTreasureDB..UserCurrency SET Diamond=Diamond-@SignupFee WHERE UserID=@dwUserID					
				END							
			END	

			-- �����¼	
			INSERT StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchType,SignupMode,MatchFee,CollectDate) 
			VALUES(@dwUserID,@wServerID,@dwMatchID,@lMatchNo,1,@cbSignupMode,@SignupFee,GETDATE())
		END		
		
	END
	
	-- �������
	/* IF (@SignupMode&@cbSignupMode)<>0 AND @cbSignupMode=2
	BEGIN
		-- �����������
		IF @MatchType=0 AND @FromMatchID<>0
		BEGIN
			-- ��ѯ����
			DECLARE @szMatchName NVARCHAR(64)
			SELECT @szMatchName=MatchName FROM MatchPublic WHERE MatchID=@FromMatchID

			-- У�鱨������
			IF @szMatchName IS NULL  RETURN 4
			
			-- ��ѯ�ʸ�					
			IF NOT Exists(SELECT * FROM MatchPromoteInfo WHERE UserID=@dwUserID AND MatchID=@FromMatchID AND PromoteMatchID=@dwMatchID)
			BEGIN
			
				IF @SignupMode=@cbSignupMode
				BEGIN
					SET @strErrorDescribe = N'��Ǹ,������������[ '+@szMatchName+N' ]�н�������ұ����μӣ������߱�������������'
				END									
				return 4				
			END					
			
			-- �ǻ������	
			IF @IsAndroidUser=0		
			BEGIN			
				-- �����¼	
				INSERT StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchType,SignupMode,MatchFee,CollectDate) 
				VALUES(@dwUserID,@wServerID,@dwMatchID,@lMatchNo,2,@cbSignupMode,0,GETDATE())
			END
														   												
		END ELSE
		BEGIN
			SET @strErrorDescribe = N'��Ǹ,����������֧�ֱ��������Ϊ������ʽ��'			
			return 4						
		END		
	END	 */	
	
	-- ���·���
	UPDATE MatchScoreInfo SET Score=@InitScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,OnlineTime=0,MatchRight=0x10000000,SignupTime=GetDate()
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo				
	
	-- �������
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO MatchScoreInfo (UserID,ServerID,MatchID,MatchNo,Score,MatchRight) 
		VALUES (@dwUserID,@wServerID,@dwMatchID,@lMatchNo,@InitScore,0x10000000)				
	END			

	-- ��ѯ���
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID	
	
	-- ��ѯ��ʯ
	DECLARE @CurrUserIngot BIGINT
	SELECT @CurrUserIngot=Diamond FROM  WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID
	
	-- ��������
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrUserIngot IS NULL SET @CurrUserIngot=0

	-- �׳�����
	SELECT @cbSignupMode AS SignupMode,@CurrScore AS Score, @CurrUserIngot AS UserIngot
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- �˻�����
CREATE PROC GSP_GR_UserMatchQuit
	@dwUserID INT,								-- �û� I D
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@dwMatchID	INT,							-- ���� I D
	@lMatchNo	BIGINT,							-- ��������
	@strMachineID NVARCHAR(32)	,				-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ������Ϣ
	IF Exists(SELECT * FROM WHQJAccountsDB..AccountsInfo WHERE UserID=@dwUserID AND IsAndroid = 1)
	BEGIN
		-- ɾ��������
		DELETE MatchScoreInfo WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	END
	
	-- ������Ϣ
	DECLARe @FeeType TINYINT
	DECLARE @SignupFee BIGINT		
	SELECT @FeeType=FeeType, @SignupFee=SignupFee FROM MatchPublic WHERE MatchID=@dwMatchID
	
	-- ��ѯģʽ
	DECLARE @SignupMode TINYINT	
	SELECT @SignupMode=SignupMode FROM StreamMatchFeeInfo WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- �����ж�	
	IF @SignupMode IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,�������ı��������ڣ�'
		return 1		
	END

	-- �����ж�
	IF NOT Exists(SELECT * FROM WHQJAccountsDB..AccountsInfo WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,�����û���Ϣ�����ڣ�'
		return 2		
	END

	-- ������ʶ
	DECLARE @IsAndroidUser TINYINT	

	-- �����ж�
	SET @IsAndroidUser=0
	IF @strClientIP=N'0.0.0.0' SET @IsAndroidUser=1	

	-- ��������
	IF @IsAndroidUser=0
	BEGIN		
		-- �۷�ģʽ �����
		IF @SignupMode=1 AND @SignupFee>0
		BEGIN		
			-- ���֧��
			IF @FeeType = 0
			BEGIN
				-- ��ѯ���
				DECLARE @Score BIGINT
				SELECT @Score=Score FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID
				IF @Score IS NULL
				BEGIN
					SET @strErrorDescribe = N'û���ҵ����Ľ����Ϣ,���������ǵĿͷ���Ա��ϵ��'
					return 3						
				END

				-- ���½��
				UPDATE WHQJTreasureDB..GameScoreInfo SET Score=@Score+@SignupFee WHERE UserID=@dwUserID				
			END
			
			-- ����֧��
			IF @FeeType = 1
			BEGIN
				-- ��ѯ��ʯ
				DECLARE @UserDiamond BIGINT
				SELECT @UserDiamond=Diamond FROM  WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID
				
				-- ������ʯ
				IF @UserDiamond IS NULL
				BEGIN
					SET @strErrorDescribe = N'û���ҵ�������ʯ��Ϣ,���������ǵĿͷ���Ա��ϵ��'
					return 3				
				END

				-- ������ʯ
				UPDATE  WHQJTreasureDB..UserCurrency SET Diamond=Diamond+@SignupFee WHERE UserID=@dwUserID		
			END	
		END					
	END

	-- ɾ����¼
	IF @IsAndroidUser=0 
	BEGIN
		DELETE StreamMatchFeeInfo WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo
	END

	-- ɾ��������
	DELETE MatchScoreInfo WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@lMatchNo

	-- ��ѯ���
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM WHQJTreasureDB..GameScoreInfo WHERE UserID=@dwUserID	
	
	-- ��ѯ����
	DECLARE @CurrUserIngot BIGINT
	SELECT @CurrUserIngot=Diamond FROM WHQJTreasureDB..UserCurrency WHERE UserID=@dwUserID
	
	-- ��������
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrUserIngot IS NULL SET @CurrUserIngot=0

	-- �׳�����
	SELECT @SignupMode AS SignupMode, @CurrScore AS Score, 0 AS UserIngot
END

RETURN 0
GO