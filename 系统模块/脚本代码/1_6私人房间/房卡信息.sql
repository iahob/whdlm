
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_QueryPersonalRoomUserInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_QueryPersonalRoomUserInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 请求房间信息
CREATE  PROCEDURE dbo.GSP_GS_QueryPersonalRoomUserInfo
	@dwUserID INT,								-- 房间标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--钻石
	DECLARE @lDiamond BIGINT	
	SELECT @lDiamond = Diamond  FROM  WHQJTreasureDB.dbo.UserCurrency WHERE UserID = @dwUserID
	
	select @lDiamond AS Diamond
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------------------



-----------------------------------------------


