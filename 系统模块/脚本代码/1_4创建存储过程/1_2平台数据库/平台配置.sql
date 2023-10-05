
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadPlatformParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadPlatformParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_LoadPlatformParameter
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- �һ�����
	DECLARE @ExchangeRate AS INT
	DECLARE @PresentExchangeRate AS INT
	DECLARE @RateGold AS INT
	SELECT @ExchangeRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MedalExchangeRate'
	SELECT @PresentExchangeRate=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'PresentExchangeRate'
--	SELECT @RateGold=StatusValue FROM  WHQJAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RateGold'

	-- ������
	SELECT @ExchangeRate AS ExchangeRate, @PresentExchangeRate AS PresentExchangeRate
	--, @RateGold AS RateGold
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------