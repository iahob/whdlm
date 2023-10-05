
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO



IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadForbidWords]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadForbidWords]

GO
SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- º”‘ÿ≈‰÷√
CREATE  PROCEDURE dbo.GSP_GR_LoadForbidWords	
WITH ENCRYPTION AS

-- …Ë÷√ Ù–‘
SET NOCOUNT ON

-- ÷¥––¬ﬂº≠
BEGIN	

	-- ≤È—Ø≈‰÷√
	SELECT ForbidWords FROM WHQJPlatformDB.dbo.SensitiveWords(NOLOCK)

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------