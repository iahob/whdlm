
----------------------------------------------------------------------------------------------------

USE WHQJGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_LoadParameter
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT							-- ���� I D
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ƻ���
	DECLARE @MedalRate AS INT
	SELECT @MedalRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MedalRate'

	-- ����˰��
	DECLARE @RevenueRate AS INT
	SELECT @RevenueRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRate'

	-- �һ�����
	DECLARE @ExchangeRate AS INT
	SELECT @ExchangeRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MedalExchangeRate'

	-- Ӯ�־���
	DECLARE @WinExperience AS INT
	SELECT @WinExperience=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'WinExperience'	

	-- ��������
	IF @MedalRate IS NULL SET @MedalRate=1
	IF @RevenueRate IS NULL SET @RevenueRate=1

	-- ����汾
	DECLARE @ClientVersion AS INT
	DECLARE @ServerVersion AS INT

	SELECT @ClientVersion=TableGame.ClientVersion, @ServerVersion=TableGame.ServerVersion
	FROM  WHQJPlatformDB.dbo.GameGameItem TableGame, WHQJPlatformDB.dbo.GameKindItem TableKind
	WHERE TableGame.GameID=TableKind.GameID	AND TableKind.KindID=@wKindID

	--ÿ������
	DECLARE @DayWinLostScoreLimit AS BIGINT
	DECLARE @DayPlayTimeLimit AS INT
	SELECT @DayWinLostScoreLimit=StatusValue
	FROM WHQJAccountsDB.dbo.SystemStatusInfo
	WHERE StatusName=N'DayWinLostScoreLimit'
	
	SELECT @DayPlayTimeLimit=StatusValue
	FROM WHQJAccountsDB.dbo.SystemStatusInfo
	WHERE StatusName=N'DayPlayTimeLimit'

	-- �һ�����
	DECLARE @PresentExchangeRate AS INT
	SELECT @PresentExchangeRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'PresentExchangeRate'

	-- �һ�����
	DECLARE @RateGold AS INT
	SELECT @RateGold=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RateGold'

	-- ��������
	IF @PresentExchangeRate IS NULL SET @PresentExchangeRate=0
	IF @RateGold IS NULL SET @RateGold=0

	-- ������
	SELECT @MedalRate AS MedalRate, @RevenueRate AS RevenueRate,@ExchangeRate AS ExchangeRate,@WinExperience AS WinExperience, 
		@ClientVersion AS ClientVersion, @ServerVersion AS ServerVersion, @PresentExchangeRate AS PresentExchangeRate, @RateGold AS RateGold,
		@DayWinLostScoreLimit AS DayWinLostScoreLimit, @DayPlayTimeLimit AS DayPlayTimeLimit

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------