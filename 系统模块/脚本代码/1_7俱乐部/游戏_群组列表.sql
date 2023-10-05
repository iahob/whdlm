
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
-- ���Ⱥ���ʶ
CREATE FUNCTION [dbo].[IMF_SplitGroupID]
(
	@strSource NVARCHAR(1024),		--Դ�ַ���
	@strSeparator CHAR(1)			--�ָ�����
)
RETURNS @tbResult TABLE(ID INT IDENTITY(1,1), GroupID BIGINT)
WITH ENCRYPTION AS  
BEGIN  
   -- ��������	
   DECLARE @dwIndexEnd INT;
   DECLARE @dwIndexStart INT;   
   DECLARE @strGroupID NVARCHAR(18);

   -- ��ȡ����
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

-- ����Ⱥ���б�
CREATE  PROCEDURE dbo.GSP_IM_QuerySingleGroup
	@dwGroupID	INT				-- Ⱥ���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯȺ����Ϣ	
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

-- ����Ⱥ���б�
CREATE  PROCEDURE dbo.GSP_IM_QueryGroupList
	@dwGroupStartID INT,			-- Ⱥ���ʶ	
	@dwGroupEndID	INT				-- Ⱥ���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯȺ����Ϣ	
	SELECT A.GroupID, A.CreaterID, A.CreaterGameID, A.CreaterNickName, A.GroupName, A.GroupLevel, A.GroupIntroduce, A.MemberCount, A.MaxMemberCount,  A.CreateDateTime,A.GroupStatus,
	B.PayType, B.CurrencyKind,B.ManagerRight,B.CaptainRight,B.InMode,B.OutMode,B.FreeTableLimit,B.BigWinnerShow,B.MedalMode,B.MedalShow,B.CurMedalShow,B.MedalRevenueShow,B.MedalClear,B.GroupIDShow,
	C.Score,D.Diamond AS Ingot
	FROM IMGroupProperty AS A, IMGroupBattleRule AS B, IMGroupWealth AS C,WHQJTreasureDB..UserCurrency AS D 
	WHERE   a.GroupID=b.GroupID AND a.GroupID=c.GroupID AND a.GroupStatus<>0 AND A.CreaterID=D.UserID AND A.GroupStatus<>0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯȺ���б�
CREATE  PROCEDURE dbo.GSP_IM_QueryUserGroupList
	@dwUserID INT			-- �û���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ȡȺ����Ϣ
	DECLARE @strGroupIDArray NVARCHAR(1024)
	SELECT @strGroupIDArray=GroupIDArray FROM IMUserGroupInfo WHERE UserID=@dwUserID

	-- ���Ⱥ���ʶ
	SELECT * INTO #tmpTableGroupID FROM dbo.IMF_SplitGroupID(@strGroupIDArray,';')

	-- ��ѯȺ����Ϣ
	SELECT * FROM IMGroupProperty WHERE GroupStatus<>0 AND GroupID IN (SELECT GroupID FROM #tmpTableGroupID);

	-- ɾ��ʱ��
	DROP TABLE #tmpTableGroupID;

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ��������
CREATE  PROCEDURE dbo.GSP_IM_QueryCommonInfo

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	SELECT OptionName,OptionValue FROM IMGroupOption
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
