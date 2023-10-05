
----------------------------------------------------------------------------------------------------

USE WHQJAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidGetParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidGetParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidAddParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidAddParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidModifyParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidModifyParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidDeleteParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidDeleteParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 读取参数
CREATE PROC GSP_GP_AndroidGetParameter
	@wServerID INT								-- 房间标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询参数
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 添加参数
CREATE PROC GSP_GP_AndroidAddParameter
	@wServerID INT,								-- 房间标识
	@dwServiceMode INT,							-- 服务模式
	@dwAndroidCount INT,						-- 机器数目
	@dwSwitchMinInnings INT,					-- 换桌局数
	@dwSwitchMaxInnings INT,					-- 换桌局数
	@dwAIHangingMinRate INT,					-- 挂机比例
	@dwAIHangingMaxRate INT,					-- 挂机比例
	
	@dwAICarryScoreMinMultiple0 INT,			--最小筹码倍数
	@dwAICarryScoreMaxMultiple0 INT,			--最小筹码倍数
	@dwAIScoreLevelRate0 INT,					--人数比例
	@dwAIScoreFloatMinRate0 INT,				--浮动范围
	@dwAIScoreFloatMaxRate0 INT,				--浮动范围
	
	@dwAICarryScoreMinMultiple1 INT,			--最小筹码倍数
	@dwAICarryScoreMaxMultiple1 INT,			--最小筹码倍数
	@dwAIScoreLevelRate1 INT,					--人数比例
	@dwAIScoreFloatMinRate1 INT,				--浮动范围
	@dwAIScoreFloatMaxRate1 INT,				--浮动范围
	
	@dwAICarryScoreMinMultiple2 INT,			--最小筹码倍数
	@dwAICarryScoreMaxMultiple2 INT,			--最小筹码倍数
	@dwAIScoreLevelRate2 INT,					--人数比例
	@dwAIScoreFloatMinRate2 INT,				--浮动范围
	@dwAIScoreFloatMaxRate2 INT,				--浮动范围
	
	@dwAICarryScoreMinMultiple3 INT,			--最小筹码倍数
	@dwAICarryScoreMaxMultiple3 INT,			--最小筹码倍数
	@dwAIScoreLevelRate3 INT,					--人数比例
	@dwAIScoreFloatMinRate3 INT,				--浮动范围
	@dwAIScoreFloatMaxRate3 INT					--浮动范围
	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	IF EXISTS (SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID)
	BEGIN
		-- 更新参数
		UPDATE AndroidConfigure SET ServiceMode=@dwServiceMode,AndroidCount=@dwAndroidCount,SwitchMinInnings=@dwSwitchMinInnings,SwitchMaxInnings=@dwSwitchMaxInnings,AIHangingMinRate=@dwAIHangingMinRate,AIHangingMaxRate=@dwAIHangingMaxRate,
		AICarryScoreMinMultiple0=@dwAICarryScoreMinMultiple0,AICarryScoreMaxMultiple0=@dwAICarryScoreMaxMultiple0,AIScoreLevelRate0=@dwAIScoreLevelRate0,dwAIScoreFloatMinRate0=@dwAIScoreFloatMinRate0,dwAIScoreFloatMaxRate0=@dwAIScoreFloatMaxRate0,
		AICarryScoreMinMultiple1=@dwAICarryScoreMinMultiple1,AICarryScoreMaxMultiple1=@dwAICarryScoreMaxMultiple1,AIScoreLevelRate1=@dwAIScoreLevelRate1,dwAIScoreFloatMinRate1=@dwAIScoreFloatMinRate1,dwAIScoreFloatMaxRate1=@dwAIScoreFloatMaxRate1,
		AICarryScoreMinMultiple2=@dwAICarryScoreMinMultiple2,AICarryScoreMaxMultiple2=@dwAICarryScoreMaxMultiple2,AIScoreLevelRate2=@dwAIScoreLevelRate2,dwAIScoreFloatMinRate2=@dwAIScoreFloatMinRate2,dwAIScoreFloatMaxRate2=@dwAIScoreFloatMaxRate2,
		AICarryScoreMinMultiple3=@dwAICarryScoreMinMultiple3,AICarryScoreMaxMultiple3=@dwAICarryScoreMaxMultiple3,AIScoreLevelRate3=@dwAIScoreLevelRate3,dwAIScoreFloatMinRate3=@dwAIScoreFloatMinRate3,dwAIScoreFloatMaxRate3=@dwAIScoreFloatMaxRate3
		WHERE ServerID=@wServerID
	END
	ELSE
	BEGIN
		-- 插入数据
		INSERT AndroidConfigure(ServerID,ServiceMode,AndroidCount,SwitchMinInnings,SwitchMaxInnings,AIHangingMinRate,AIHangingMaxRate,
		AICarryScoreMinMultiple0,AICarryScoreMaxMultiple0,AIScoreLevelRate0,dwAIScoreFloatMinRate0,dwAIScoreFloatMaxRate0,
		AICarryScoreMinMultiple1,AICarryScoreMaxMultiple1,AIScoreLevelRate1,dwAIScoreFloatMinRate1,dwAIScoreFloatMaxRate1,
		AICarryScoreMinMultiple2,AICarryScoreMaxMultiple2,AIScoreLevelRate2,dwAIScoreFloatMinRate2,dwAIScoreFloatMaxRate2,
		AICarryScoreMinMultiple3,AICarryScoreMaxMultiple3,AIScoreLevelRate3,dwAIScoreFloatMinRate3,dwAIScoreFloatMaxRate3)
		VALUES(@wServerID,@dwServiceMode,@dwAndroidCount,@dwSwitchMinInnings,@dwSwitchMaxInnings,@dwAIHangingMinRate,@dwAIHangingMaxRate,
		@dwAICarryScoreMinMultiple0,@dwAICarryScoreMaxMultiple0,@dwAIScoreLevelRate0,@dwAIScoreFloatMinRate0,@dwAIScoreFloatMaxRate0,
		@dwAICarryScoreMinMultiple1,@dwAICarryScoreMaxMultiple1,@dwAIScoreLevelRate1,@dwAIScoreFloatMinRate1,@dwAIScoreFloatMaxRate1,
		@dwAICarryScoreMinMultiple2,@dwAICarryScoreMaxMultiple2,@dwAIScoreLevelRate2,@dwAIScoreFloatMinRate2,@dwAIScoreFloatMaxRate2,
		@dwAICarryScoreMinMultiple3,@dwAICarryScoreMaxMultiple3,@dwAIScoreLevelRate3,@dwAIScoreFloatMinRate3,@dwAIScoreFloatMaxRate3)
	END
	
	-- 查询数据
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 修改参数
CREATE PROC GSP_GP_AndroidModifyParameter
	@wServerID INT,								-- 批次标识
	@dwServiceMode INT,							-- 服务模式
	@dwAndroidCount INT,						-- 机器数目
	@dwSwitchMinInnings INT,					-- 换桌局数
	@dwSwitchMaxInnings INT,					-- 换桌局数	
	@dwAIHangingMinRate INT,					-- 挂机比例
	@dwAIHangingMaxRate INT,					-- 挂机比例
	
	@dwAICarryScoreMinMultiple0 INT,			--最小筹码倍数
	@dwAICarryScoreMaxMultiple0 INT,			--最小筹码倍数
	@dwAIScoreLevelRate0 INT,					--人数比例
	@dwAIScoreFloatMinRate0 INT,				--浮动范围
	@dwAIScoreFloatMaxRate0 INT,				--浮动范围
	
	@dwAICarryScoreMinMultiple1 INT,			--最小筹码倍数
	@dwAICarryScoreMaxMultiple1 INT,			--最小筹码倍数
	@dwAIScoreLevelRate1 INT,					--人数比例
	@dwAIScoreFloatMinRate1 INT,				--浮动范围
	@dwAIScoreFloatMaxRate1 INT,				--浮动范围
	
	@dwAICarryScoreMinMultiple2 INT,			--最小筹码倍数
	@dwAICarryScoreMaxMultiple2 INT,			--最小筹码倍数
	@dwAIScoreLevelRate2 INT,					--人数比例
	@dwAIScoreFloatMinRate2 INT,				--浮动范围
	@dwAIScoreFloatMaxRate2 INT,				--浮动范围
	
	@dwAICarryScoreMinMultiple3 INT,			--最小筹码倍数
	@dwAICarryScoreMaxMultiple3 INT,			--最小筹码倍数
	@dwAIScoreLevelRate3 INT,					--人数比例
	@dwAIScoreFloatMinRate3 INT,				--浮动范围
	@dwAIScoreFloatMaxRate3 INT					--浮动范围
	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 更新参数
	UPDATE AndroidConfigure SET ServiceMode=@dwServiceMode,AndroidCount=@dwAndroidCount,SwitchMinInnings=@dwSwitchMinInnings,SwitchMaxInnings=@dwSwitchMaxInnings,AIHangingMinRate=@dwAIHangingMinRate,AIHangingMaxRate=@dwAIHangingMaxRate,
	AICarryScoreMinMultiple0=@dwAICarryScoreMinMultiple0,AICarryScoreMaxMultiple0=@dwAICarryScoreMaxMultiple0,AIScoreLevelRate0=@dwAIScoreLevelRate0,dwAIScoreFloatMinRate0=@dwAIScoreFloatMinRate0,dwAIScoreFloatMaxRate0=@dwAIScoreFloatMaxRate0,
	AICarryScoreMinMultiple1=@dwAICarryScoreMinMultiple1,AICarryScoreMaxMultiple1=@dwAICarryScoreMaxMultiple1,AIScoreLevelRate1=@dwAIScoreLevelRate1,dwAIScoreFloatMinRate1=@dwAIScoreFloatMinRate1,dwAIScoreFloatMaxRate1=@dwAIScoreFloatMaxRate1,
	AICarryScoreMinMultiple2=@dwAICarryScoreMinMultiple2,AICarryScoreMaxMultiple2=@dwAICarryScoreMaxMultiple2,AIScoreLevelRate2=@dwAIScoreLevelRate2,dwAIScoreFloatMinRate2=@dwAIScoreFloatMinRate2,dwAIScoreFloatMaxRate2=@dwAIScoreFloatMaxRate2,
	AICarryScoreMinMultiple3=@dwAICarryScoreMinMultiple3,AICarryScoreMaxMultiple3=@dwAICarryScoreMaxMultiple3,AIScoreLevelRate3=@dwAIScoreLevelRate3,dwAIScoreFloatMinRate3=@dwAIScoreFloatMinRate3,dwAIScoreFloatMaxRate3=@dwAIScoreFloatMaxRate3
	WHERE ServerID=@wServerID
	
	-- 查询数据
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除参数
CREATE PROC GSP_GP_AndroidDeleteParameter
	@wServerID INT								-- 批次标识	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询参数
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

	-- 删除参数
	DELETE AndroidConfigure WHERE ServerID=@wServerID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------