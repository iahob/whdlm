USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_WritePersonalFeeParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_WritePersonalFeeParameter]
--ɾ������
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_DeletePersonalFeeParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_DeletePersonalFeeParameter]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetPersonalParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_GetPersonalParameter]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetPersonalFeeParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_GetPersonalFeeParameter]


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetPersonalFeeParameterEx]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_GetPersonalFeeParameterEx]

---����˽�˷������
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadPersonalRoomParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadPersonalRoomParameter]

---����˽�˷������
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadPersonalRoomParameterByKindID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].GSP_GS_LoadPersonalRoomParameterByKindID
GO

----��ȡ������ҽ�����ķ���
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_GetPersonalRoomUserScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_GetPersonalRoomUserScore]
GO


----д�����ʱ��
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_CloseRoomWriteDissumeTime]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_CloseRoomWriteDissumeTime]
GO

----��ȡ˽�˷���������
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetPersonalRule]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_GetPersonalRule]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

------------------------------------------------------
---��������
CREATE PROC GSP_GS_WritePersonalFeeParameter
	@KindID	INT,							-- ��������
	@cbPlayMode TINYINT,					-- ��Ϸ�淨
	@DrawCountLimit	INT,					-- ����
	@DrawTimeLimit	INT,					-- ��ҵ��߶೤ʱ����ɢ����
	@TableFee	INT,						-- ���䴴���೤ʱ���δ��ʼ��Ϸ��ɢ����
	@AAPayFee   INT,						-- AA�Ʒ���
	@IniScore	INT,						-- ���䴴����೤ʱ���δ��ʼ��Ϸ��ɢ����
	@cbGameMode TINYINT,					-- ��Ϸģʽ
	@strErrorDescribe NVARCHAR(127) OUTPUT	-- �����Ϣ
WITH ENCRYPTION AS

BEGIN
	INSERT INTO PersonalTableFee (KindID,PlayMode, DrawCountLimit,DrawTimeLimit, TableFee,AAPayFee,IniScore,GameMode) 
	VALUES (@KindID, @cbPlayMode, @DrawCountLimit,@DrawTimeLimit, @TableFee,@AAPayFee, @IniScore,@cbGameMode) 
END

return 0
GO

--ɾ������
CREATE PROC GSP_GS_DeletePersonalFeeParameter
	@KindID	INT,						-- ��Ϸ����
	@cbPlayMode TINYINT,				--	0 ���ֳ�  1 ��ҳ�
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

BEGIN
	
	DELETE  FROM PersonalTableFee WHERE  KindID = @KindID AND PlayMode = @cbPlayMode
	
END

return 0
GO


------------------------------------------------------
---��������
CREATE PROC GSP_MB_GetPersonalFeeParameterEx
	@dwKindID INT,								-- ��Ϸ I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

BEGIN

	SELECT PlayMode, DrawCountLimit, DrawTimeLimit, TableFee,AAPayFee,IniScore,GameMode FROM PersonalTableFee WHERE KindID=@dwKindID
END

return 0
GO

------------------------------------------------------
---��������
CREATE PROC GSP_MB_GetPersonalFeeParameter
	@dwKindID INT,								-- ��Ϸ I D
	@cbPlayMode TINYINT,						-- ��Ϸģʽ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

BEGIN

	SELECT PlayMode, DrawCountLimit, DrawTimeLimit, TableFee,AAPayFee,IniScore,GameMode FROM PersonalTableFee WHERE KindID=@dwKindID	AND PlayMode = @cbPlayMode
END

return 0
GO

---------------------------------------------------------------
-- ��������
CREATE PROC GSP_GS_LoadPersonalRoomParameterByKindID
	@dwKindID INT,								-- �����ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON	
	DECLARE @cbIsJoinGame TINYINT						-- �Ƿ������Ϸ
	DECLARE @cbMinPeople TINYINT						-- ������Ϸ����С����
	DECLARE @cbMaxPeople TINYINT						-- ������Ϸ���������

	-- ��������
	DECLARE @lPersonalRoomTax  BIGINT					-- �������
	DECLARE @lMaxCellScore BIGINT						-- ���׷�
	DECLARE @wCanCreateCount int						-- ���Դ�������󷿼���Ŀ
	DECLARE @wPlayTurnCount INT							-- �����ܹ�������Ϸ��������
	DECLARE @cbCreateRight TINYINT						-- ����˽�˷���Ȩ��

	-- ��������
	DECLARE @wPlayTimeLimit	INT						-- �����ܹ�������Ϸ�����ʱ��
	DECLARE @wTimeAfterBeginCount	INT				-- һ����Ϸ��ʼ��೤ʱ����ɢ����
	DECLARE @wTimeOffLineCount	INT					-- ��ҵ��߶೤ʱ����ɢ����
	DECLARE @wTimeNotBeginGame	INT					-- ���䴴���೤ʱ���δ��ʼ��Ϸ��ɢ����
	DECLARE @wTimeNotBeginAfterCreateRoom	INT		-- ���䴴����೤ʱ���δ��ʼ��Ϸ��ɢ����

	DECLARE @wBeginFreeTime		INT					-- ���ʱ��-��ʼ
	DECLARE @wEndFreeTime		INT					-- ���ʱ��-����
	DECLARE	@lMinCellScore	BIGINT					-- ��С�׷�
	DECLARE @lMinEnterScore BIGINT					-- ��С�����
	DECLARE @lMaxEnterScore BIGINT					-- �������
	DECLARE @lPlayMode		BIGINT					-- �������

-- ִ���߼�
BEGIN		
	SET @cbIsJoinGame = 0
	SET @cbMinPeople = 0
	SET @cbMaxPeople = 0	
	SET @lMaxCellScore = 0
	SET @lPersonalRoomTax = 0
	SET @lMaxCellScore = 0
	SET @wCanCreateCount = 0
	SET @wPlayTurnCount = 0
	SET @wPlayTimeLimit = 0
	SET @wTimeAfterBeginCount = 0
	SET @wTimeOffLineCount = 0
	SET @wTimeNotBeginGame = 0
	SET @wTimeNotBeginAfterCreateRoom = 0
	SET @cbCreateRight = 0
	SET @wBeginFreeTime = 0
	SET @wEndFreeTime = 0
	SET @lMinCellScore = 0
	SET @lMinEnterScore = 0
	SET @lMaxEnterScore = 0
	SET @lPlayMode = 0

	SELECT IsJoinGame, MinPeople, MaxPeople, PersonalRoomTax,MaxCellScore,CanCreateCount, PlayTurnCount, PlayTimeLimit,  TimeAfterBeginCount, 
	TimeOffLineCount, TimeNotBeginGame, TimeNotBeginAfterCreateRoom, CreateRight,
	BeginFreeTime,EndFreeTime, PlayMode, MinCellScore, MinEnterScore, MaxEnterScore
	FROM PersonalRoomInfo WHERE KindID = @dwKindID 

END

RETURN 0

GO

---------------------------------------------------------------
-- ��������
CREATE PROC GSP_GS_LoadPersonalRoomParameter
	@dwKindID INT,								-- �����ʶ
	@cbPlayMode TINYINT,							-- ��Ϸģʽ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON	
	DECLARE @cbIsJoinGame TINYINT						-- �Ƿ������Ϸ
	DECLARE @cbMinPeople TINYINT						-- ������Ϸ����С����
	DECLARE @cbMaxPeople TINYINT						-- ������Ϸ���������

	-- ��������
	DECLARE @lPersonalRoomTax  BIGINT					-- �������
	DECLARE @lMaxCellScore BIGINT						-- ���׷�
	DECLARE @wCanCreateCount int						-- ���Դ�������󷿼���Ŀ
	DECLARE @wPlayTurnCount INT							-- �����ܹ�������Ϸ��������
	DECLARE @cbCreateRight TINYINT						-- ����˽�˷���Ȩ��

	-- ��������
	DECLARE @wPlayTimeLimit	INT						-- �����ܹ�������Ϸ�����ʱ��
	DECLARE @wTimeAfterBeginCount	INT				-- һ����Ϸ��ʼ��೤ʱ����ɢ����
	DECLARE @wTimeOffLineCount	INT					-- ��ҵ��߶೤ʱ����ɢ����
	DECLARE @wTimeNotBeginGame	INT					-- ���䴴���೤ʱ���δ��ʼ��Ϸ��ɢ����
	DECLARE @wTimeNotBeginAfterCreateRoom	INT		-- ���䴴����೤ʱ���δ��ʼ��Ϸ��ɢ����

	DECLARE @wBeginFreeTime		INT					-- ���ʱ��-��ʼ
	DECLARE @wEndFreeTime		INT					-- ���ʱ��-����
	DECLARE	@lMinCellScore	BIGINT					-- ��С�׷�
	DECLARE @lMinEnterScore BIGINT					-- ��С�����
	DECLARE @lMaxEnterScore BIGINT					-- �������
	DECLARE @lPlayMode		BIGINT					-- �������

-- ִ���߼�
BEGIN		
	SET @cbIsJoinGame = 0
	SET @cbMinPeople = 0
	SET @cbMaxPeople = 0	
	SET @lMaxCellScore = 0
	SET @lPersonalRoomTax = 0
	SET @lMaxCellScore = 0
	SET @wCanCreateCount = 0
	SET @wPlayTurnCount = 0
	SET @wPlayTimeLimit = 0
	SET @wTimeAfterBeginCount = 0
	SET @wTimeOffLineCount = 0
	SET @wTimeNotBeginGame = 0
	SET @wTimeNotBeginAfterCreateRoom = 0
	SET @cbCreateRight = 0
	SET @wBeginFreeTime = 0
	SET @wEndFreeTime = 0
	SET @lMinCellScore = 0
	SET @lMinEnterScore = 0
	SET @lMaxEnterScore = 0
	SET @lPlayMode = 0

	SELECT IsJoinGame, MinPeople, MaxPeople, PersonalRoomTax,MaxCellScore,CanCreateCount, PlayTurnCount, PlayTimeLimit,  TimeAfterBeginCount, 
	TimeOffLineCount, TimeNotBeginGame, TimeNotBeginAfterCreateRoom, CreateRight,
	BeginFreeTime,EndFreeTime, PlayMode, MinCellScore, MinEnterScore, MaxEnterScore,TimeRespondDismiss
	FROM PersonalRoomInfo WHERE KindID = @dwKindID AND PlayMode = @cbPlayMode

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����˽�˷�����Ϣ
CREATE PROC GSP_GS_GetPersonalRoomUserScore
	@dwUserID INT,								-- �û� I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ط���
	SELECT * FROM PersonalRoomScoreInfo WHERE UserID = @dwUserID ORDER BY WriteTime DESC

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- ����˽�˷�����Ϣ
CREATE PROC GSP_GS_CloseRoomWriteDissumeTime
	@dwServerID INT,								-- �û� I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	--ɾ���˷������������
	DELETE FROM  WHQJTreasureDB.dbo.GameScoreLocker WHERE ServerID = @dwServerID;
	-- ���ط���
	UPDATE StreamCreateTableFeeInfo SET DissumeDate = GetDATE(), RoomStatus = 2 WHERE ServerID = @dwServerID AND DissumeDate IS NULL

END

RETURN 0

GO

------------------------------------------------------
---��������
CREATE PROC GSP_MB_GetPersonalRule
	@dwKindID INT,								-- ��Ϸ I D
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

BEGIN
	SELECT PlayMode, PersonalRule FROM PersonalRoomInfo a, GameRoomInfo b WHERE a.ServerID = b.ServerID AND @dwKindID = a.KindID
	
	IF @@ROWCOUNT = 0
	BEGIN		
		SET @strErrorDescribe=N'�Ҳ�������Ϸ��'
		RETURN 1
	END
	
	return 0		
END

return 0
GO

---------------------------------------------------------------