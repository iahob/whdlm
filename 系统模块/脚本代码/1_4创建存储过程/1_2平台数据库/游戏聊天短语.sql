
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_GameChatPhrase]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_GameChatPhrase]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--º”‘ÿ∂Ã”Ô
CREATE  PROCEDURE dbo.GSP_GR_GameChatPhrase 
WITH ENCRYPTION AS

--…Ë÷√ Ù–‘
SET NOCOUNT ON

--≤È—Ø√Ë ˆ
SELECT * FROM GameChatPhrase ORDER BY SortID

RETURN 0

GO

----------------------------------------------------------------------------------------------------