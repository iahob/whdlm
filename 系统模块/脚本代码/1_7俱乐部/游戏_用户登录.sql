
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_EfficacyUserID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_EfficacyUserID]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 校验标识
CREATE  PROCEDURE dbo.GSP_IM_EfficacyUserID
	@dwUserID INT,							-- 用户标识
	@strPassword NCHAR(32),					-- 用户密码
	@strErrorDescribe NVARCHAR(127) OUTPUT  -- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID			INT
DECLARE @Gender			TINYINT
DECLARE @GameID			INT
DECLARE @CustomID		INT
DECLARE @NickName		NVARCHAR(31)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END	

	-- 帐号关闭
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须到重新开通后才能继续使用！'
		RETURN 3
	END

	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 4
	END

	-- 查询群组
	DECLARE @strGroupIDArray NVARCHAR(512)
	SELECT @strGroupIDArray=GroupIDArray FROM IMUserGroupInfo(NOLOCK) WHERE UserID=@dwUserID

	--调整变量
	IF @strGroupIDArray IS NULL SET @strGroupIDArray=N''

	-- 抛出结果
	SELECT @UserID AS UserID, @GameID AS GameID, @LogonPass AS LogonPass, @NickName AS NickName, @Gender AS Gender, @CustomID AS CustomID, @strGroupIDArray AS GroupIDArray	      
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------