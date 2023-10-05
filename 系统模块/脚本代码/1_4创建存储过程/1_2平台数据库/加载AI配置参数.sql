
----------------------------------------------------------------------------------------------------

USE WHQJPlatformDB
GO

-----��ͬʱ�����AI�����仯
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAITimeChange]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAITimeChange]
GO


-----���뷿��Ȩ��
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAIWeightIn]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAIWeightIn]
GO


-----���뷿��Ȩ��
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAIWeightOut]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAIWeightOut]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
------------��ͬʱ�����AI�����仯
CREATE PROC GSP_GP_LoadAITimeChange
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	--ѡ�����״̬���ֲ���AI����
	SELECT * FROM AITimeChange 

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

------------��ͬʱ�����AI�����仯
CREATE PROC GSP_GP_LoadAIWeightIn
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	--ѡ�����״̬���ֲ���AI����
	SELECT * FROM AIWeightInRatio 

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

------------��ͬʱ�����AI�����仯
CREATE PROC GSP_GP_LoadAIWeightOut
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	--ѡ�����״̬���ֲ���AI����
	SELECT * FROM AIWeightOutRatio 

END

RETURN 0

GO
