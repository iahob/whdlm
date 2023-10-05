
----------------------------------------------------------------------------------------------------

USE WHQJTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteAwardTicket]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteGameScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 游戏写分
CREATE PROC GSP_GR_WriteAwardTicket

	-- 用户信息
	@dwUserID INT,						-- 用户 I D
	@nAwardTicket INT					-- 游戏时间

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 更新奖券
	UPDATE UserCurrency SET AwardTicket=AwardTicket+@nAwardTicket WHERE UserID=@dwUserID

	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------