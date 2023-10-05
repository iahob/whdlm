
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[IMF_SplitGroupID]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[IMF_SplitGroupID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryGroupList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryGroupList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryUserGroupList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryUserGroupList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QuerySingleGroup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QuerySingleGroup]
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryCommonInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryCommonInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------
-- 拆分群组标识
CREATE FUNCTION [dbo].[IMF_SplitGroupID]
(
	@strSource NVARCHAR(1024),		--源字符串
	@strSeparator CHAR(1)			--分隔符号
)
RETURNS @tbResult TABLE(ID INT IDENTITY(1,1), GroupID BIGINT)
WITH ENCRYPTION AS  
BEGIN  
   -- 变量定义	
   DECLARE @dwIndexEnd INT;
   DECLARE @dwIndexStart INT;   
   DECLARE @strGroupID NVARCHAR(18);

   -- 提取数据
   SET @strSource=RTRIM(LTRIM(@strSource));    
   SET @dwIndexStart=CHARINDEX(@strSeparator,@strSource)+1;
   SET @dwIndexEnd=CHARINDEX(@strSeparator,@strSource,@dwIndexStart);

   WHILE @dwIndexEnd>0
   BEGIN
	  SET @strGroupID=SUBSTRING(@strSource,@dwIndexStart,@dwIndexEnd-@dwIndexStart);      
	  INSERT INTO @tbResult VALUES (CONVERT(BIGINT,@strGroupID))

	  SET @dwIndexStart=@dwIndexEnd+1;     
      SET @dwIndexEnd=CHARINDEX(@strSeparator,@strSource,@dwIndexStart);
   END 

   RETURN
END

GO


----------------------------------------------------------------------------------------------------

-- 加载群组列表
CREATE  PROCEDURE dbo.GSP_IM_QuerySingleGroup
	@dwGroupID	INT				-- 群组标识	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询群组信息	
	SELECT A.GroupID, A.CreaterID, A.CreaterGameID, A.CreaterNickName, A.GroupName, A.GroupLevel, A.GroupIntroduce, A.MemberCount, A.MaxMemberCount,  A.CreateDateTime,A.GroupStatus,
	B.PayType, B.CurrencyKind,B.ManagerRight,B.CaptainRight,B.InMode,B.OutMode,B.FreeTableLimit,B.BigWinnerShow,B.MedalMode,B.MedalShow,B.CurMedalShow,B.MedalRevenueShow,B.MedalClear,B.GroupIDShow,
	C.Score,D.Diamond AS Ingot
	FROM IMGroupProperty AS A, IMGroupBattleRule AS B, IMGroupWealth AS C,WHQJTreasureDB..UserCurrency AS D 
	WHERE   a.GroupID = @dwGroupID AND b.GroupID = @dwGroupID AND c.GroupID = @dwGroupID AND a.GroupStatus<>0 AND A.CreaterID=D.UserID AND A.GroupStatus<>0
	--WHERE   a.GroupID=b.GroupID AND a.GroupID=c.GroupID AND a.GroupStatus<>0 AND A.CreaterID=D.UserID AND A.GroupStatus<>0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载群组列表
CREATE  PROCEDURE dbo.GSP_IM_QueryGroupList
	@dwGroupStartID INT,			-- 群组标识	
	@dwGroupEndID	INT				-- 群组标识	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询群组信息	
	SELECT A.GroupID, A.CreaterID, A.CreaterGameID, A.CreaterNickName, A.GroupName, A.GroupLevel, A.GroupIntroduce, A.MemberCount, A.MaxMemberCount,  A.CreateDateTime,A.GroupStatus,
	B.PayType, B.CurrencyKind,B.ManagerRight,B.CaptainRight,B.InMode,B.OutMode,B.FreeTableLimit,B.BigWinnerShow,B.MedalMode,B.MedalShow,B.CurMedalShow,B.MedalRevenueShow,B.MedalClear,B.GroupIDShow,
	C.Score,D.Diamond AS Ingot
	FROM IMGroupProperty AS A, IMGroupBattleRule AS B, IMGroupWealth AS C,WHQJTreasureDB..UserCurrency AS D 
	WHERE   a.GroupID=b.GroupID AND a.GroupID=c.GroupID AND a.GroupStatus<>0 AND A.CreaterID=D.UserID AND A.GroupStatus<>0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询群组列表
CREATE  PROCEDURE dbo.GSP_IM_QueryUserGroupList
	@dwUserID INT			-- 用户标识	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 读取群组信息
	DECLARE @strGroupIDArray NVARCHAR(1024)
	SELECT @strGroupIDArray=GroupIDArray FROM IMUserGroupInfo WHERE UserID=@dwUserID

	-- 拆分群组标识
	SELECT * INTO #tmpTableGroupID FROM dbo.IMF_SplitGroupID(@strGroupIDArray,';')

	-- 查询群组信息
	SELECT * FROM IMGroupProperty WHERE GroupStatus<>0 AND GroupID IN (SELECT GroupID FROM #tmpTableGroupID);

	-- 删临时表
	DROP TABLE #tmpTableGroupID;

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询公共配置
CREATE  PROCEDURE dbo.GSP_IM_QueryCommonInfo

WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	SELECT OptionName,OptionValue FROM IMGroupOption
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
