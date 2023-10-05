
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_CreateGroup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_CreateGroup]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_DeleteGroup]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_DeleteGroup]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetGroupStatus]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SetGroupStatus]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_UpdateGroupWealth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_UpdateGroupWealth]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_UpdateGroupProperty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_UpdateGroupProperty]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_UpdateGroupBattleRule]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_UpdateGroupBattleRule]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryMedalMode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryMedalMode]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetInOut]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SetInOut]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_RightManagement]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_RightManagement]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryRight]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryRight]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetMedal]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SetMedal]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetFreeTableLimit]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SetFreeTableLimit]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetBigWinner]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SetBigWinner]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetRollMsg]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SetRollMsg]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ModifyGroupName]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ModifyGroupName]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_SetPrivacy]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_SetPrivacy]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryScore]
GO

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[GetCurSubStorageItem]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION [dbo].[GetCurSubStorageItem]
GO

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[HexToDec]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION [dbo].[HexToDec]
GO

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[BinaryToHex]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION [dbo].[BinaryToHex]
GO

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[DecToBinary]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION [dbo].[DecToBinary]
GO

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[GetScoreVal]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION [dbo].[GetScoreVal]
GO

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[FormatString]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION [dbo].[FormatString]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- 创建群组
CREATE  PROCEDURE dbo.GSP_IM_CreateGroup
	-- 用户信息
	@dwCreaterID INT,							-- 用户标识
	@strPassword NCHAR(32),						-- 用户密码

	-- 群组信息
	@strGroupName NVARCHAR(32),					-- 群组名称
	@strGroupIntroduce NVARCHAR(128),			-- 群组介绍

	-- 群组规则
	@cbPayType TINYINT,							-- 支付类型	
	@cbCurrencyKind TINYINT,					-- 货币类型	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			
	DECLARE @AgentID INT
	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown ,@AgentID=AgentID		 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
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
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END
	-- 支付类型
	IF @cbPayType=0 OR @cbPayType>3
	BEGIN
		SET @strErrorDescribe=N'抱歉，支付类型值为无效值，请重新设置！'
		RETURN 5		
	END
	-- 变量定义
	DECLARE @MaxMemberCount SMALLINT
	DECLARE @MaxCreateGroupCount SMALLINT
	DECLARE @CreateGroupTakeIngot BIGINT
	DECLARE @CreateGroupDeductIngot BIGINT
	DECLARE @GroupPayType SMALLINT
	DECLARE @CreateSwitch SMALLINT
	DECLARE @CreateGroup SMALLINT
	DECLARE @OpenMedalMode SMALLINT
	DECLARE @CreatePresentMedal BIGINT
	-- 查询配置
	SELECT @MaxMemberCount=OptionValue FROM IMGroupOption WHERE OptionName=N'MaxMemberCount' 
	SELECT @MaxCreateGroupCount=OptionValue FROM IMGroupOption WHERE OptionName=N'MaxCreateGroupCount' 	
	SELECT @CreateGroupTakeIngot=OptionValue FROM IMGroupOption WHERE OptionName=N'CreateGroupTakeIngot' 	
	SELECT @CreateGroupDeductIngot=OptionValue FROM IMGroupOption WHERE OptionName=N'CreateGroupDeductIngot'		
	SELECT @GroupPayType=OptionValue FROM IMGroupOption WHERE OptionName=N'GroupPayType' 	
	SELECT @CreateSwitch=OptionValue FROM IMGroupOption WHERE OptionName=N'CreateSwitch' 
	SELECT @CreateGroup=OptionValue FROM IMGroupOption WHERE OptionName=N'CreateGroup' 		
	SELECT @OpenMedalMode=OptionValue FROM IMGroupOption WHERE OptionName=N'OpenMedalMode' 	
	SELECT @CreatePresentMedal=OptionValue FROM IMGroupOption WHERE OptionName=N'CreatePresentMedal' 					
	-- 调整配置
	IF @MaxMemberCount IS NULL SET @MaxMemberCount=500
	IF @MaxCreateGroupCount IS NULL SET @MaxCreateGroupCount=5
	IF @CreateGroupTakeIngot IS NULL SET @CreateGroupTakeIngot=0
	IF @CreateGroupDeductIngot IS NULL SET @CreateGroupDeductIngot=0
	IF @GroupPayType IS NULL SET @GroupPayType=1
	IF @CreateSwitch IS NULL SET @CreateSwitch=0
	IF @CreateGroup IS NULL SET @CreateGroup=3
	IF @OpenMedalMode IS NULL SET @OpenMedalMode=3
	IF @CreatePresentMedal IS NULL SET @CreatePresentMedal=0

	--1.普通用户 2.代理用户 3.全部
	IF @CreateGroup<>3
	BEGIN
		IF @CreateGroup=1 AND @AgentID>0
		BEGIN
			SET @strErrorDescribe=N'创建失败，没有创建资格！'
			RETURN 1
		END
		ELSE IF @CreateGroup=2 AND @AgentID=0
		BEGIN
			SET @strErrorDescribe=N'创建失败，没有创建资格！'
			RETURN 1
		END
		ELSE IF NOT EXISTS(SELECT 1 FROM IMGroupWhiteList WHERE UserID=@dwCreaterID)
		BEGIN
			SET @strErrorDescribe=N'创建失败，没有创建资格！'
			RETURN 1
		END
	END

	--判断规则
	IF @GroupPayType<>3
	BEGIN
		IF @GroupPayType=1 AND @cbPayType=2
		BEGIN
			SET @strErrorDescribe=N'创建失败，系统不支持玩家支付！'
			RETURN 6
		END
		
		IF @GroupPayType=2 AND @cbPayType=1
		BEGIN
			SET @strErrorDescribe=N'创建失败，系统不支持群主支付！'
			RETURN 7
		END
	END
	
	-- 查询财富
	DECLARE @UserIngot BIGINT
	DECLARE @UserScore BIGINT
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency(NOLOCK) WHERE UserID=@dwCreaterID
	SELECT @UserScore=Score FROM WHQJTreasureDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwCreaterID
	-- 调整财富
	IF @UserIngot IS NULL SET @UserIngot=0
	IF @UserScore IS NULL SET @UserScore=0
	
	IF @CreateGroupTakeIngot=1 OR @CreateGroupTakeIngot=3
	BEGIN
		-- 判断金币
		IF @UserScore<@CreateGroupDeductIngot
		BEGIN
			SET @strErrorDescribe=N'创建失败，当前帐号身上金币少于'+CONVERT(NVARCHAR(16),@CreateGroupDeductIngot)+',无法创建大联盟！'
			RETURN 110
		END
	END
	
	IF @CreateGroupTakeIngot=2 OR @CreateGroupTakeIngot=4
	BEGIN
		-- 判断钻石
		DECLARE @MaxUserPay INT
		DECLARE @MaxGroupPay INT	

		SELECT @MaxUserPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @dwCreaterID AND GroupID=0 AND RoomStatus <> 2
		SELECT @MaxGroupPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE RoomStatus <> 2 
		AND GroupID in(Select GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty where CreaterID=@dwCreaterID) 
		IF @MaxUserPay IS NULL SET @MaxUserPay=0
		IF @MaxGroupPay IS NULL	SET @MaxGroupPay=0

		IF @UserIngot<@CreateGroupDeductIngot+@MaxUserPay+@MaxGroupPay
		BEGIN
			SET @strErrorDescribe=N'创建失败，当前帐号身上钻石少于'+CONVERT(NVARCHAR(16),@CreateGroupDeductIngot+@MaxUserPay+@MaxGroupPay)+',无法创建大联盟！'
			RETURN 111		
		END
	END

	-- 创建数量
	DECLARE @wCreateGroupCount SMALLINT
	SELECT @wCreateGroupCount=COUNT(GroupID) FROM IMGroupProperty(NOLOCK) WHERE CreaterID=@dwCreaterID AND GroupStatus<>0

	-- 判断数量
	IF @wCreateGroupCount>=@MaxCreateGroupCount
	BEGIN
		SET @strErrorDescribe=N'抱歉，您创建的大联盟个数已达到系统设置的上限'+CONVERT(NVARCHAR(16),@MaxCreateGroupCount)+'个,大联盟创建失败！'
		RETURN 10		
	END

	-- 校验大联盟名
	--IF EXISTS(SELECT GroupID FROM IMGroupProperty(NOLOCK) WHERE GroupName=@strGroupName)
	--BEGIN
	--	SET @strErrorDescribe=N'系统中已经存在相同名字的大联盟，创建失败！'
	--	RETURN 11		
	--END

	-- 插入群组
	DECLARE @CreateDateTime DATETIME
	SET @CreateDateTime=GetDate()

	DECLARE @CanUseMedal INT
	SET @CanUseMedal=0

	IF @OpenMedalMode=3
		SET @CanUseMedal=1
	ELSE IF @OpenMedalMode=2 AND @AgentID>0
		SET @CanUseMedal=1
	ELSE IF @OpenMedalMode=1 AND @AgentID=0
		SET @CanUseMedal=1
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- 群组属性
	INSERT INTO IMGroupProperty(CreaterID, CreaterGameID, CreaterNickName, GroupName, GroupIntroduce, MemberCount, MaxMemberCount,CanUseMedal, CreateDateTime)
	VALUES (@dwCreaterID, @GameID, @NickName, @strGroupName, @strGroupIntroduce, 0, @MaxMemberCount,@CanUseMedal, @CreateDateTime)
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- 错误信息
		SET @strErrorDescribe=N'由于系统原因,大联盟创建失败[0]，请联系客户服务中心了解详细情况！'
		RETURN 12
	END

	-- 查询群组
	DECLARE @dwGroupID BIGINT
	SELECT 	@dwGroupID=SCOPE_IDENTITY()

	-- 群组规则
	INSERT INTO IMGroupBattleRule(GroupID, PayType, CurrencyKind,MedalShow,CurMedalShow,MedalRevenueShow,ManagerRight,MedalMode) VALUES (@dwGroupID, @cbPayType, @cbCurrencyKind,1,1,1,10,@CanUseMedal)	
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- 错误信息
		SET @strErrorDescribe=N'由于系统原因,大联盟创建失败[1]，请联系客户服务中心了解详细情况！'
		RETURN 13
	END

	-- 群组财富
	INSERT INTO IMGroupWealth(GroupID, Ingot,Score) VALUES(@dwGroupID, 0,@CreatePresentMedal)
	IF @@ERROR<>0
	BEGIN
		-- 事务回滚
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- 错误信息
		SET @strErrorDescribe=N'由于系统原因,大联盟创建失败[2]，请联系客户服务中心了解详细情况！'
		RETURN 14
	END 
	--初始赠送记录	
	INSERT INTO RecordGroupScoreChange(GroupID,BeforeScore,ChangeType,ChangeScore,AfterScore,CollectDate) VALUES(@dwGroupID,0,2,@CreatePresentMedal,@CreatePresentMedal,@CreateDateTime)

	-- 扣除金币
	IF @CreateGroupDeductIngot>0 AND @CreateGroupTakeIngot=1
	BEGIN
		--更新金币
		UPDATE WHQJTreasureDB.dbo.GameScoreInfo SET Score=Score-@CreateGroupDeductIngot WHERE UserID=@dwCreaterID
	END
	
	-- 扣除钻石
	IF @CreateGroupDeductIngot>0 AND @CreateGroupTakeIngot=2
	BEGIN
		-- 变更记录
		INSERT INTO WHQJTreasureDB.dbo.RecordCurrencyChange (UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
		VALUES (@dwCreaterID,-@CreateGroupDeductIngot,14,@UserIngot,@UserIngot-@CreateGroupDeductIngot,N'-------------',GETDATE(),N'创建大联盟')	

		-- 更新钻石
		UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond-@CreateGroupDeductIngot WHERE UserID=@dwCreaterID
	END	


	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED		

	-- 查询财富	
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency(NOLOCK) WHERE UserID=@dwCreaterID

	-- 调整财富
	IF @UserIngot IS NULL SET @UserIngot=0
	
	--插入默认共享库存规则
	DECLARE @tablePersonalRoomInfo TABLE
	(
		KindID INT,
		PlayMode TINYINT,
		ShareStorageRule nvarchar(1024)
	);
	
	-- 插入数据
	INSERT INTO @tablePersonalRoomInfo(KindID,PlayMode,ShareStorageRule) SELECT KindID,PlayMode,ShareStorageRule FROM WHQJPlatformDBLink.WHQJPlatformDB.dbo.PersonalRoomInfo WHERE PlayMode=1
	
	WHILE EXISTS(SELECT KindID FROM @tablePersonalRoomInfo)
	BEGIN
		SET ROWCOUNT 1		
		
		DECLARE @KindID INT
		DECLARE @PlayMode TINYINT
		DECLARE @ShareStorageRule nvarchar(1024)
		SELECT @KindID=KindID,@PlayMode=PlayMode,@ShareStorageRule=ShareStorageRule FROM @tablePersonalRoomInfo 
		
		IF NOT EXISTS(SELECT * FROM IMGroupShareStorageProperty WHERE KindID=@KindID AND PlayMode=@PlayMode AND GroupID=@dwGroupID)
		BEGIN
		
			DECLARE @ConfigSysStorage BIGINT
			DECLARE @ConfigPlayerStorage BIGINT
			DECLARE @ConfigParameterK BIGINT
			DECLARE @CurResetCount BIGINT
			DECLARE @CurSubStorageItem nvarchar(1024)

			IF NOT EXISTS(SELECT * FROM IMGroupShareStorageProperty WHERE KindID=@KindID AND PlayMode=@PlayMode)
			BEGIN
				SET @ConfigSysStorage=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*0,16))
				SET @ConfigPlayerStorage=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*1,16))
				SET @ConfigParameterK=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*2,16))
	
				INSERT INTO IMGroupShareStorageProperty(GroupID,KindID,PlayMode,ConfigSubStorageItem,CurSubStorageItem,ConfigSysStorage,ConfigPlayerStorage,ConfigParameterK,CurResetCount) 
				VALUES(@dwGroupID,@KindID,@PlayMode,@ShareStorageRule,dbo.GetCurSubStorageItem(@ShareStorageRule),@ConfigSysStorage,@ConfigPlayerStorage,@ConfigParameterK,0)		
			END
			ELSE
			BEGIN
				SELECT @CurSubStorageItem=CurSubStorageItem,@ConfigSysStorage=ConfigSysStorage,@ConfigPlayerStorage=ConfigPlayerStorage,@ConfigParameterK=ConfigParameterK,@CurResetCount=CurResetCount FROM IMGroupShareStorageProperty WHERE KindID=@KindID AND PlayMode=@PlayMode
				INSERT INTO IMGroupShareStorageProperty(GroupID,KindID,PlayMode,ConfigSubStorageItem,CurSubStorageItem,ConfigSysStorage,ConfigPlayerStorage,ConfigParameterK,CurResetCount) 
				VALUES(@dwGroupID,@KindID,@PlayMode,@ShareStorageRule,@CurSubStorageItem,@ConfigSysStorage,@ConfigPlayerStorage,@ConfigParameterK,@CurResetCount)		
			END
		END
	
		SET ROWCOUNT 0 
		DELETE FROM @tablePersonalRoomInfo WHERE KindID=@KindID AND PlayMode=@PlayMode
	END
	
	
	IF OBJECT_ID('tempdb..#tablePersonalRoomInfo') IS NOT NULL
	BEGIN
		DROP TABLE tablePersonalRoomInfo
	End

	-- 查询信息
	SELECT A.GroupID, A.CreaterID, A.CreaterGameID, A.CreaterNickName, A.GroupName, A.GroupLevel,A.GroupStatus, A.GroupIntroduce, A.MemberCount, A.MaxMemberCount, A.CreateDateTime,
	 B.PayType, B.CurrencyKind,B.ManagerRight,B.CaptainRight,B.InMode,B.OutMode,B.FreeTableLimit,B.BigWinnerShow,B.MedalMode,B.MedalShow,B.CurMedalShow,B.MedalRevenueShow,B.MedalClear,B.GroupIDShow,
	  C.Ingot, @UserIngot AS UserIngot
	FROM IMGroupProperty AS A, IMGroupBattleRule AS B, IMGroupWealth AS C 
	WHERE A.GroupID=B.GroupID AND A.GroupID=C.GroupID AND A.GroupID=@dwGroupID	
	
END

RETURN 0

GO



----------------------------------------------------------------------------------------------------

-- 删除群组
CREATE  PROCEDURE dbo.GSP_IM_DeleteGroup
	-- 用户信息
	@dwCreaterID INT,							-- 用户标识
	@strPassword NCHAR(32),						-- 用户密码
	@dwGroupID BIGINT,							-- 群组标识
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);	

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
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
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END

	-- 查询群主
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要删除的大联盟不存在，请查证后再次尝试！'
		RETURN 5		
	END

	-- 不是群主
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的盟主，没有权限进行此操作！'
		RETURN 5		
	END
	
	-- 变量定义
	DECLARE @UserIngot BIGINT

	-- 查询财富
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwCreaterID	

	-- 调整财富
	IF @UserIngot IS NULL SET @UserIngot=0


	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 变量定义
	DECLARE @GroupIngot BIGINT	
	

	-- 查询财富	
	SELECT @GroupIngot=Ingot FROM IMGroupWealth WITH (UPDLOCK) WHERE GroupID=@dwGroupID

	-- 调整财富	
	IF @GroupIngot IS NULL SET @GroupIngot=0

	-- 取出钻石
	IF @GroupIngot>0
	BEGIN
		-- 变更记录
		INSERT INTO WHQJTreasureDB.dbo.RecordCurrencyChange (UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
		VALUES (@dwCreaterID,@GroupIngot,12,@UserIngot,@UserIngot+@GroupIngot,N'-------------',GETDATE(),N'删除大联盟')	
		--插入记录
		INSERT INTO RecordGroupIngotChange(GroupID,BeforeIngot,TradeType,TradeIngot,TradeRoomGUID,CollectDate) 
		VALUES(@dwGroupID,@GroupIngot,4,-@GroupIngot,'0',GETDATE())
		-- 更新钻石
		UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@GroupIngot WHERE UserID=@dwCreaterID

		-- 更新财富信息
		UPDATE IMGroupWealth SET Ingot=Ingot-@GroupIngot WHERE GroupID=@dwGroupID
	END

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
	-- 修改状态
	--DELETE IMGroupProperty WHERE  GroupID=@dwGroupID
	UPDATE IMGroupProperty SET GroupStatus=0 WHERE GroupID=@dwGroupID


	-- 构造标识
	DECLARE @strGroupID NVARCHAR(20)
	SET @strGroupID=';'+CONVERT(NVARCHAR(18),@dwGroupID)+';';
	
	UPDATE IMUserGroupInfo SET GroupIDArray=REPLACE(GroupIDArray,@strGroupID,';') WHERE UserID IN (SELECT UserID FROM IMGroupMember WHERE GroupID=@dwGroupID)
	-- 查询财富
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwCreaterID	

	-- 调整财富
	IF @UserIngot IS NULL SET @UserIngot=0
	
	--删除默认共享库存规则
	DELETE FROM IMGroupShareStorageProperty WHERE GroupID=@dwGroupID
	
	SELECT 	@UserIngot AS UserIngot					  
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 设置大联盟状态
CREATE  PROCEDURE dbo.GSP_IM_SetGroupStatus
	-- 用户信息
	@dwUserID	 INT,							-- 用户标识
	@strPassword NCHAR(32),						-- 用户密码
	@dwGroupID BIGINT,							-- 群组标识
	@cbStatus TINYINT,							--状态1生效，2解散，3冻结
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 变量定义
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);	

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
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
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END

	-- 查询群主
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND GroupStatus<>0

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要删除的大联盟不存在，请查证后再次尝试！'
		RETURN 5		
	END

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF @CreaterID = @dwUserID
		SET @HaveRight=1
	ELSE
	BEGIN
		DECLARE @UserRight BIGINT
		SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		IF @UserRight IS NULL
		BEGIN
			SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
			RETURN 6	
		END

		IF (@UserRight&2) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x01)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight <> 1 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您没有权限进行此操作！'
		RETURN 6		
	END
	
	DECLARE @CurStatus TINYINT
	SELECT @CurStatus=GroupStatus From IMGroupProperty WHERE GroupID=@dwGroupID
	IF @cbStatus<>1 AND @cbStatus<>2
	BEGIN
		SET @strErrorDescribe=N'抱歉，设置状态有误，请检查！'
		RETURN 7	
	END

	-- 修改状态
	UPDATE IMGroupProperty SET GroupStatus=@cbStatus WHERE GroupID=@dwGroupID
		  
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- 更新财富
CREATE  PROCEDURE dbo.GSP_IM_UpdateGroupWealth
	-- 用户信息
	@dwCreaterID INT,							-- 用户标识
	@strPassword NCHAR(32),						-- 用户密码
	@dwGroupID BIGINT,							-- 群组标识

	-- 群组财富	
	@lIngot BIGINT,								-- 钻石数量
	
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
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
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END

	-- 查询群主
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND GroupStatus<>0

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要删除的大联盟不存在，请查证后再次尝试！'
		RETURN 5		
	END

	-- 不是群主
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的盟主，没有权限进行此操作！'
		RETURN 5		
	END

	-- 变量定义
	DECLARE @UserIngot BIGINT
	
	-- 查询财富
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwCreaterID	

	-- 调整财富
	IF @UserIngot IS NULL SET @UserIngot=0

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 变量定义
	DECLARE @GroupIngot BIGINT	

	-- 查询财富	
	SELECT @GroupIngot=Ingot FROM IMGroupWealth WITH (UPDLOCK) WHERE GroupID=@dwGroupID

	-- 调整财富	
	IF @GroupIngot IS NULL SET @GroupIngot=0

	-- 存入钻石
	IF @lIngot>0
	BEGIN		
		IF @UserIngot<@lIngot
		BEGIN
			-- 事务回滚
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			-- 错误信息
			SET @strErrorDescribe=N'钻石余额不足,存储失败！'
			RETURN 4		
		END

		-- 需要消耗的钻石数量
		DECLARE @UserNeedPayIngot BIGINT

		-- 理论上需要消耗的钻石
		SELECT @UserNeedPayIngot = SUM(NeedRoomCard) FROM WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @dwCreaterID AND GroupID<>@dwGroupID AND RoomStatus = 0

		-- 剩下的钻石是否够支付 
		IF @UserNeedPayIngot IS NOT NULL AND @UserIngot - @UserNeedPayIngot < @lIngot
		BEGIN
			-- 事务回滚
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			-- 错误信息
			SET @strErrorDescribe=N'您有房间未支付完成，不能存入大联盟钻石！'
			RETURN 4
		END
		
		----查找未支付的房间
		--IF EXISTS (SELECT * FROM WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @dwCreaterID AND GroupID<>@dwGroupID AND RoomStatus = 0)
		--BEGIN
		--	-- 事务回滚
		--	ROLLBACK TRAN
		--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		--	-- 错误信息
		--	SET @strErrorDescribe=N'您有房间未支付完成，不能存入大联盟钻石！'
		--	RETURN 4
		--END
		
		-- 变更记录
		INSERT INTO WHQJTreasureDB.dbo.RecordCurrencyChange (UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
		VALUES (@dwCreaterID,-@lIngot,11,@UserIngot,@UserIngot-@lIngot,N'-------------',GETDATE(),N'存入钻石')	

		--插入记录
		INSERT INTO RecordGroupIngotChange(GroupID,BeforeIngot,TradeType,TradeIngot,TradeRoomGUID,CollectDate) 
		VALUES(@dwGroupID,@GroupIngot,1,@lIngot,'0',GETDATE())
		-- 更新钻石
		UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond-@lIngot WHERE UserID=@dwCreaterID

		-- 更新财富信息
		UPDATE IMGroupWealth SET Ingot=Ingot+@lIngot WHERE GroupID=@dwGroupID
	END

	-- 取出钻石
	IF @lIngot<0
	BEGIN
		IF @GroupIngot+@lIngot<0
		BEGIN
			-- 事务回滚
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			-- 错误信息
			SET @strErrorDescribe=N'大联盟的钻石余额不足,钻石提取失败！'
			RETURN 4		
		END
		--查找未支付的房间
		IF EXISTS (SELECT * FROM WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE GroupID=@dwGroupID AND RoomStatus = 0)
		BEGIN
			-- 事务回滚
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			-- 错误信息
			SET @strErrorDescribe=N'大联盟有房间未支付完成，不能提取大联盟钻石！'
			RETURN 4
		END
		
		
		-- 变更记录
		INSERT INTO WHQJTreasureDB.dbo.RecordCurrencyChange (UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
		VALUES (@dwCreaterID,-@lIngot,12,@UserIngot,@UserIngot-@lIngot,N'-------------',GETDATE(),N'取出钻石')	
		--插入记录
		INSERT INTO RecordGroupIngotChange(GroupID,BeforeIngot,TradeType,TradeIngot,TradeRoomGUID,CollectDate) 
		VALUES(@dwGroupID,@GroupIngot,2,@lIngot,'0',GETDATE())
		-- 更新钻石
		UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond-@lIngot WHERE UserID=@dwCreaterID

		-- 更新财富信息
		UPDATE IMGroupWealth SET Ingot=Ingot+@lIngot WHERE GroupID=@dwGroupID
	END

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 查询财富
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwCreaterID	

	-- 调整财富
	IF @UserIngot IS NULL SET @UserIngot=0
	
	-- 查询信息
	SELECT GroupID, Ingot, @UserIngot AS UserIngot FROM IMGroupWealth(NOLOCK) WHERE GroupID=@dwGroupID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新属性
CREATE  PROCEDURE dbo.GSP_IM_UpdateGroupProperty
	-- 用户信息
	@dwCreaterID INT,							-- 用户标识
	@strPassword NCHAR(32),						-- 用户密码
	@dwGroupID BIGINT,							-- 群组标识

	-- 群组规则
	@strGroupIntroduce NVARCHAR(128),			-- 群组介绍	
	
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
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
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END

	-- 查询群主
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND GroupStatus<>0

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要删除的大联盟不存在，请查证后再次尝试！'
		RETURN 5		
	END

	-- 不是群主
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的盟主，没有权限进行此操作！'
		RETURN 5		
	END

	-- 更新规则
	UPDATE IMGroupProperty SET GroupIntroduce=@strGroupIntroduce WHERE GroupID=@dwGroupID

	-- 查询信息
	SELECT * FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 更新规则
CREATE  PROCEDURE dbo.GSP_IM_UpdateGroupBattleRule
	-- 用户信息
	@dwCreaterID INT,							-- 用户标识
	@strPassword NCHAR(32),						-- 用户密码
	@dwGroupID BIGINT,							-- 群组标识

	-- 群组规则
	@cbPayType TINYINT,							-- 支付方式
	@cbCurrencyKind TINYINT,					-- 货币类型
	
	-- 输出参数
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 用户信息
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- 用户不存在
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
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
		SET @strErrorDescribe=N'您的帐号不存在或者密码有误，请查证后再次尝试！'
		RETURN 4
	END

	-- 查询群主
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND GroupStatus<>0

	-- 结果判断
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您要更新的大联盟不存在，请查证后再次尝试！'
		RETURN 5		
	END

	-- 不是群主
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'抱歉，您不是该大联盟的盟主，没有权限进行此操作！'
		RETURN 5		
	END

	-- 支付类型
	IF @cbPayType=0 OR @cbPayType>3
	BEGIN
		SET @strErrorDescribe=N'抱歉，支付类型值为无效值，请重新设置！'
		RETURN 6		
	END
	-- 查询配置
	DECLARE @GroupPayTypeChange SMALLINT
	DECLARE @GroupPayType SMALLINT
	SELECT @GroupPayTypeChange=OptionValue FROM IMGroupOption WHERE OptionName=N'GroupPayTypeChange'
	SELECT @GroupPayType=OptionValue FROM IMGroupOption WHERE OptionName=N'GroupPayType' 
	
	IF @GroupPayTypeChange IS NULL  SET @GroupPayTypeChange=0
	IF @GroupPayType IS NULL  SET @GroupPayType=1
	
	IF @GroupPayTypeChange=0
		BEGIN
		SET @strErrorDescribe=N'系统禁止修改支付规则！'
		RETURN 5				
	END
	
	-- 校验支付方式
	IF @GroupPayType=1 AND @cbPayType=2
	BEGIN
		SET @strErrorDescribe=N'你设置的支付类型与系统配置不符，请重新设置'
		RETURN 5
	END
		
	IF @GroupPayType=2 AND @cbPayType=1
	BEGIN
		SET @strErrorDescribe=N'你设置的支付类型与系统配置不符，请重新设置！'
		RETURN 5
	END

	-- 更新规则
	UPDATE IMGroupBattleRule SET PayType=@cbPayType WHERE GroupID=@dwGroupID

	-- 查询信息
	SELECT * FROM IMGroupBattleRule(NOLOCK) WHERE GroupID=@dwGroupID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询勋章模式
CREATE  PROCEDURE dbo.GSP_IM_QueryMedalMode
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @cbMedalMode TINYINT
	SELECT @cbMedalMode=MedalMode FROM IMGroupBattleRule(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @cbMedalMode IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END

	IF NOT EXISTS(SELECT 1 FROM IMGroupMember WHERE UserID=@dwUserID AND GroupID=@dwGroupID)
	BEGIN
		SET @strErrorDescribe=N'您不是该大联盟成员，无权查询！'
		RETURN 2	
	END
	
	SELECT @cbMedalMode AS MedalMode
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 进出设置
CREATE  PROCEDURE dbo.GSP_IM_SetInOut
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@cbInMode		TINYINT,		-- 进入设置
	@cbInType		TINYINT,		-- 进入方式
	@cbOutMode		TINYINT,		-- 退出设置
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF @dwCreaterID = @dwUserID
		SET @HaveRight=1
	ELSE
	BEGIN
		DECLARE @UserRight BIGINT
		SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		IF @UserRight IS NULL
		BEGIN
			SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
			RETURN 2	
		END

		IF (@UserRight&2) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x02)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight=0
	BEGIN
		SET @strErrorDescribe=N'操作失败，无权设置！'
		RETURN 3		
	END

	UPDATE IMGroupBattleRule SET InMode=@cbInMode,OutMode=@cbOutMode,GroupIDShow=@cbInType WHERE GroupID=@dwGroupID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 权限管理
CREATE  PROCEDURE dbo.GSP_IM_RightManagement
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 玩家标识	
	@dwTargetID		INT,			-- 目标玩家标识	
	@dwManagerRight	BIGINT,			-- 管理员权限
	@dwCaptainRight	BIGINT,			-- 队长权限
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END

	IF @dwCreaterID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'您不是该大联盟盟主，无权设置！'
		RETURN 2		
	END

	IF @dwTargetID=0
		UPDATE IMGroupBattleRule SET ManagerRight=@dwManagerRight,CaptainRight=@dwCaptainRight WHERE GroupID=@dwGroupID
	ELSE
	BEGIN
		DECLARE @MemberRight INT
		SELECT @MemberRight=MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwTargetID
		IF @MemberRight IS NULL
		BEGIN
			SET @strErrorDescribe=N'目标用户不是该大联盟成员，请检查！'
			RETURN 3
		END

		IF (@MemberRight&0x02)=0
		BEGIN
			SET @strErrorDescribe=N'目标用户不是管理员！'
			RETURN 4
		END

		UPDATE IMGroupMember SET MemberManagerRight=@dwManagerRight,ManagerRightFlag=1 WHERE GroupID=@dwGroupID AND UserID=@dwTargetID
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 权限管理
CREATE  PROCEDURE dbo.GSP_IM_QueryRight
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END

	IF @dwCreaterID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'您不是该大联盟盟主，无权设置！'
		RETURN 2		
	END

	SELECT ManagerRight,CaptainRight  FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 勋章设置
CREATE  PROCEDURE dbo.GSP_IM_SetMedal
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@cbMedalMode	TINYINT,		-- 勋章开关
	@cbMedalShow	TINYINT,		-- 勋章显示
	@cbCurMedalShow	TINYINT,		-- 当前勋章显示
	@cbMedalRevenueShow	TINYINT,	-- 勋章损耗显示
	@cbMedalClear	TINYINT,		-- 勋章清零
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF @dwCreaterID = @dwUserID
		SET @HaveRight=1
	ELSE
	BEGIN
		DECLARE @UserRight BIGINT
		SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		IF @UserRight IS NULL
		BEGIN
			SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
			RETURN 2	
		END

		IF (@UserRight&2) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x20)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight=0
	BEGIN
		SET @strErrorDescribe=N'操作失败，无权设置！'
		RETURN 3		
	END

	IF @cbMedalMode=1
	BEGIN
		DECLARE @CanUseMedal TINYINT
		SELECT @CanUseMedal=CanUseMedal FROM IMGroupProperty WHERE GroupID=@dwGroupID
		IF @CanUseMedal=0
		BEGIN
			SET @strErrorDescribe=N'该大联盟无法开启勋章，请联系运营商！'
			RETURN 4
		END
	END

	UPDATE IMGroupBattleRule SET MedalMode=@cbMedalMode,MedalShow=@cbMedalShow,CurMedalShow=@cbCurMedalShow,MedalRevenueShow=@cbMedalRevenueShow,MedalClear=@cbMedalClear
		WHERE GroupID=@dwGroupID

	--关闭勋章模式，则删除勋章玩法
	IF @cbMedalMode=0
	BEGIN
		DELETE FROM IMGroupConfig WHERE PlayMode=1 AND GroupID=@dwGroupID
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 大赢家设置
CREATE  PROCEDURE dbo.GSP_IM_SetBigWinner
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@cbShowMode		TINYINT,		-- 模式
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF @dwCreaterID = @dwUserID
		SET @HaveRight=1
	ELSE
	BEGIN
		DECLARE @UserRight BIGINT
		SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		IF @UserRight IS NULL
		BEGIN
			SET @strErrorDescribe=N'您不是该大联盟成员，无权设置！'
			RETURN 2	
		END

		IF (@UserRight&2) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x2000)<>0 
				SET @HaveRight=1
		END
			
	END

	IF @HaveRight=0
	BEGIN
		SET @strErrorDescribe=N'操作失败，无权设置！'
		RETURN 3		
	END

	UPDATE IMGroupBattleRule SET BigWinnerShow=@cbShowMode WHERE GroupID=@dwGroupID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 空桌限制
CREATE  PROCEDURE dbo.GSP_IM_SetFreeTableLimit
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@cbCount		TINYINT,		-- 限制数量
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF @dwCreaterID = @dwUserID
		SET @HaveRight=1

	IF @HaveRight=0
	BEGIN
		SET @strErrorDescribe=N'操作失败，无权设置！'
		RETURN 3		
	END


	UPDATE IMGroupBattleRule SET FreeTableLimit=@cbCount WHERE GroupID=@dwGroupID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 跑马灯
CREATE  PROCEDURE dbo.GSP_IM_SetRollMsg
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END

	DECLARE @HaveRight TINYINT 
	SET @HaveRight=0

	IF @dwCreaterID = @dwUserID
		SET @HaveRight=1
	ELSE
	BEGIN
		DECLARE @UserRight BIGINT
		SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
		IF @UserRight IS NULL
		BEGIN
			SET @strErrorDescribe=N'您不是该大联盟成员，无权发送跑马灯！'
			RETURN 2	
		END

		IF (@UserRight&2) <> 0--管理员
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x08)<>0 
				SET @HaveRight=1
		END
		ELSE IF (@UserRight&4)<>0--队长
		BEGIN
			DECLARE @CaptainRight BIGINT
			SELECT @CaptainRight = CaptainRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@CaptainRight&0x80)<>0 
				SET @HaveRight=2
		END
			
	END

	IF @HaveRight=0
	BEGIN
		SET @strErrorDescribe=N'发送跑马灯失败，权限不足！'
		RETURN 3		
	END

	DECLARE @nValue INT
	-- 查询配置
	SELECT @nValue=OptionValue FROM IMGroupOption WHERE OptionName=N'RollMsgTime' 
	
	SELECT @nValue AS RollTime,@HaveRight AS MsgType
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改大联盟名称
CREATE  PROCEDURE dbo.GSP_IM_ModifyGroupName
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@szGroupName	NVARCHAR(32),	-- 名称
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END


	IF @dwCreaterID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'您不是该大联盟盟主，无权修改名称！'
		RETURN 2	
	END

	UPDATE IMGroupProperty SET GroupName=@szGroupName WHERE GroupID=@dwGroupID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 设置大联盟隐私
CREATE  PROCEDURE dbo.GSP_IM_SetPrivacy
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@cbIDShow		TINYINT,		-- ID显示
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- 结果判断
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'大联盟信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END


	IF @dwCreaterID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'您不是该大联盟盟主，无权修改隐私设置！'
		RETURN 2	
	END

	UPDATE IMGroupBattleRule SET GroupIDShow=@cbIDShow WHERE GroupID=@dwGroupID

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------


-- 查询分数
CREATE  PROCEDURE dbo.GSP_IM_QueryScore
	@dwGroupID		INT,			-- 群组标识
	@dwUserID		INT,			-- 群组标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息	
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查询群主
	DECLARE @nRight INT
	SELECT @nRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	-- 结果判断
	IF @nRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'玩家信息查询失败，请查证后再次尝试！'
		RETURN 1		
	END

	DECLARE @lScore BIGINT
	IF @nRight=1 OR @nRight=2
	BEGIN
		SELECT @lScore=Score FROM IMGroupWealth WHERE GroupID=@dwGroupID
	END
	ELSE IF @nRight=4
	BEGIN
		SELECT @lScore=Score FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	END
	ELSE
	BEGIN
		SET @strErrorDescribe=N'您没有权限修改玩家勋章！'
		RETURN 2	
	END

	IF @lScore IS NULL
		SET @lScore=0

	SELECT @lScore AS SCORE

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

CREATE FUNCTION GetCurSubStorageItem(@ShareStorageRule NVARCHAR(1024))
RETURNS NVARCHAR(1024)
BEGIN
	DECLARE @CurSubStorageItem NVARCHAR(1024)
	
	DECLARE @lConfigSysStorage BIGINT
	DECLARE @lConfigPlayerStorage BIGINT
	DECLARE @lConfigParameterK BIGINT
	DECLARE @lConfigSysStorResetRate BIGINT
	DECLARE @lConfigAnChouRate BIGINT
	DECLARE @lConfigMosgoldRate BIGINT
	DECLARE @lConfigMosgoldDispatchRate BIGINT
	DECLARE @lConfigMosgoldStorageRate BIGINT
	
	SET @CurSubStorageItem=''
	SET @lConfigSysStorage=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*0,16))
	SET @lConfigPlayerStorage=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*1,16))
	SET @lConfigParameterK=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*2,16))
	SET @lConfigSysStorResetRate=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*3,16))
	SET @lConfigAnChouRate=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*4,16))
	SET @lConfigMosgoldRate=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*5,16))
	SET @lConfigMosgoldDispatchRate=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*6,16))
	SET @lConfigMosgoldStorageRate=dbo.GetScoreVal(SUBSTRING(@ShareStorageRule,1+16*7,16))
	
	SET @CurSubStorageItem=@CurSubStorageItem+dbo.FormatString(0)+dbo.FormatString(0)+dbo.FormatString(@lConfigSysStorage)+dbo.FormatString(@lConfigPlayerStorage)+dbo.FormatString(@lConfigParameterK)
	+dbo.FormatString(@lConfigSysStorResetRate)+dbo.FormatString(0)+dbo.FormatString(0)+dbo.FormatString(0)+dbo.FormatString(@lConfigAnChouRate)+dbo.FormatString(0)+dbo.FormatString(@lConfigMosgoldRate)
	+dbo.FormatString(@lConfigMosgoldDispatchRate)+dbo.FormatString(@lConfigMosgoldStorageRate)+dbo.FormatString(0)+dbo.FormatString(0)+dbo.FormatString(0)+dbo.FormatString(0)+dbo.FormatString(0)
	
	RETURN @CurSubStorageItem
END

GO

----------------------------------------------------------------------------------------------------

CREATE  FUNCTION HexToDec (@hex NVARCHAR(100))
RETURNS INT
AS
 BEGIN
     DECLARE @result INT,@iPos INT,@iTmp INT,@iLoop INT,@tmp NVARCHAR(16)
     SET @tmp = '0123456789ABCDEF'
     SELECT @result = 0,@iPos = 0
     WHILE @iPos <  LEN(@hex)
     BEGIN
           SET  @iTmp = 
                CHARINDEX(SUBSTRING(@hex,LEN(@hex) - @iPos,1),@tmp)-1
           SET @iLoop = 1
           WHILE @iLoop <= @iPos and @iTmp > 0
           BEGIN
               SET @iTmp = @iTmp * 16
               SET @iLoop = @iLoop + 1
           END
           SET @result = @result + @iTmp
           SET @iLoop = @iLoop + 1     
           SET @iPos = @iPos + 1
     END

    return @result
 END

GO

----------------------------------------------------------------------------------------------------

CREATE  FUNCTION BinaryToHex (@Binary NVARCHAR(4))
RETURNS NVARCHAR(1)
AS
BEGIN
     DECLARE @tmp NVARCHAR(16)
     DECLARE @Dec TINYINT
     DECLARE @Index TINYINT    
     SET @tmp='0123456789ABCDEF'
     SET @Dec=0
     SET @Index=1
     
     WHILE @Index <=4
     BEGIN
		DECLARE @CHAR NVARCHAR(1)   
		SET @CHAR=SUBSTRING(@Binary,@Index,1)
		SET @Dec=@Dec+POWER(2,4-@Index) * CAST(@CHAR AS TINYINT)
		SET @Index=@Index+1
     END
     
    RETURN SUBSTRING(@tmp,@Dec+1,1)
 END

GO

----------------------------------------------------------------------------------------------------

CREATE FUNCTION [dbo].[DecToBinary](@Dec BIGINT, @StrLen INT) 
RETURNS NVARCHAR(64)
AS 
BEGIN 
    DECLARE @BinStr AS NVARCHAR(64)     
    DECLARE @Div2 AS INT       
    DECLARE @Mod2 AS INT  
	DECLARE @Negative AS TINYINT  
	SET @Negative=0
	
	IF @Dec < 0
	BEGIN
		SET @Dec=-@Dec
		SET @Negative=1
	END
	
	--获取原码
	SET @Div2 = @Dec / 2  
	SET @Mod2 = @Dec % 2  
	SET @BinStr = '' 
	WHILE @Div2 <> 0 
	BEGIN 
		SET @BinStr = CAST(@Mod2 AS CHAR(1)) + @BinStr 
		SET @Dec = @Dec / 2 
		SET @Div2 = @Dec / 2 
		SET @Mod2 = @Dec % 2 
	END

	SET @BinStr = CAST(@Mod2 AS CHAR(1)) + @BinStr 
	IF @StrLen > LEN(@BinStr) 
	BEGIN 
		IF @StrLen > 64
		BEGIN
			SET @StrLen = 64 
		END
            
		DECLARE @ZeroStr NVARCHAR(64)   
		DECLARE @OffsetLen TINYINT    
		SET @ZeroStr = '' 
		SET @OffsetLen = @StrLen - LEN(@BinStr)  
		WHILE  @OffsetLen > 0 
		BEGIN 
			SET @ZeroStr = @ZeroStr + '0' 
			SET @OffsetLen = @OffsetLen - 1 
		END 
		SET @BinStr = @ZeroStr + @BinStr 
	END 
	
	--负数转换
	IF @Negative = 1
	BEGIN

		DECLARE @TruncateStr NVARCHAR(64)	
		DECLARE @NegativeStr NVARCHAR(64)	
		DECLARE @TempChar NVARCHAR(1)
		DECLARE @OffsetIndex TINYINT    
		
		SET @NegativeStr='1'
		SET @OffsetIndex=1		
		SET @TruncateStr=SUBSTRING(@BinStr, 2, LEN(@BinStr)-1)
		
		--负数反码
		WHILE  @OffsetIndex <= LEN(@BinStr)-1 
		BEGIN 
			SET @TempChar = SUBSTRING(@TruncateStr, @OffsetIndex,1)
			
			IF @TempChar = 0
			BEGIN
				SET @TempChar=1
			END
			ELSE IF @TempChar=1
			BEGIN
				SET @TempChar=0
			END
			
			SET @NegativeStr=@NegativeStr+@TempChar
			SET @OffsetIndex=@OffsetIndex+1 
		END 
		
		SET @BinStr=@NegativeStr 
		
		--负数补码(反码+1)
		SET @TruncateStr=SUBSTRING(@BinStr, 2, LEN(@BinStr)-1)
		SET @OffsetIndex=LEN(@TruncateStr)		
		DECLARE @ReverseIndex TINYINT  
		SET @ReverseIndex=1
		
		WHILE  @OffsetIndex >= 1
		BEGIN 	
			SET @TempChar = SUBSTRING(@TruncateStr, @OffsetIndex,1)
			
			IF CAST(@TempChar AS TINYINT) + 1 <= 1
			BEGIN
				SET @TempChar=1
				
				--替换指定位置
				SET @BinStr='1'+SUBSTRING(@TruncateStr,1,LEN(@TruncateStr)-@ReverseIndex)+@TempChar+SUBSTRING(@TruncateStr,@OffsetIndex+1,LEN(@TruncateStr)-@OffsetIndex)
				SET @TruncateStr=SUBSTRING(@TruncateStr,1,LEN(@TruncateStr)-@ReverseIndex)+@TempChar+SUBSTRING(@TruncateStr,@OffsetIndex+1,LEN(@TruncateStr)-@OffsetIndex)
				
				BREAK
			END
			ELSE IF CAST(@TempChar AS TINYINT) + 1 > 1
			BEGIN
				SET @TempChar=0
				
				--替换指定位置
				SET @BinStr='1'+SUBSTRING(@TruncateStr,1,LEN(@TruncateStr)-@ReverseIndex)+@TempChar+SUBSTRING(@TruncateStr,@OffsetIndex+1,LEN(@TruncateStr)-@OffsetIndex)
				SET @TruncateStr=SUBSTRING(@TruncateStr,1,LEN(@TruncateStr)-@ReverseIndex)+@TempChar+SUBSTRING(@TruncateStr,@OffsetIndex+1,LEN(@TruncateStr)-@OffsetIndex)
				
				SET @OffsetIndex=@OffsetIndex-1 
				SET @ReverseIndex=@ReverseIndex+1
				
				CONTINUE
			END
			
			SET @OffsetIndex=@OffsetIndex-1 
			SET @ReverseIndex=@ReverseIndex+1
		END 				
	END
	
    RETURN @BinStr 
END

GO

----------------------------------------------------------------------------------------------------

CREATE FUNCTION GetScoreVal(@ScoreString NVARCHAR(16))
RETURNS BIGINT
AS
BEGIN
    DECLARE @ScoreVal BIGINT
	SET @ScoreVal = 0
	DECLARE @ScoreIndex TINYINT
	SET @ScoreIndex = 0
	
	WHILE @ScoreIndex < 8
	BEGIN
		DECLARE @CHAR1 NVARCHAR(1)
		DECLARE @CHAR2 NVARCHAR(1)
		SET @CHAR1=SUBSTRING(@ScoreString, 1 + @ScoreIndex * 2, 1)
		SET @CHAR2=SUBSTRING(@ScoreString, 2 + @ScoreIndex * 2, 1)
		
		DECLARE @CircleVal INT
		SET @CircleVal=0
		IF @CHAR2 >= '0' AND @CHAR2 <= '9'
		BEGIN
			SET @CircleVal = @CircleVal + dbo.HexToDec(@CHAR2) - dbo.HexToDec('0')
		END
		ELSE IF @CHAR2 >= 'A' AND @CHAR2 <= 'F'
		BEGIN
			SET @CircleVal = @CircleVal + dbo.HexToDec(@CHAR2) - dbo.HexToDec('A') + dbo.HexToDec('A')
		END 
		
		IF @CHAR1 >= '0' AND @CHAR1 <= '9'
		BEGIN
			SET @CircleVal = @CircleVal + (dbo.HexToDec(@CHAR1) - dbo.HexToDec('0')) * 16
		END
		ELSE IF @CHAR1 >= 'A' AND @CHAR1 <= 'F'
		BEGIN
			SET @CircleVal = @CircleVal + (dbo.HexToDec(@CHAR1) - dbo.HexToDec('A') + dbo.HexToDec('A')) * 16
		END 

		IF @CircleVal > 0
		BEGIN
			SET @ScoreVal = @ScoreVal + @CircleVal * POWER(256, @ScoreIndex)
		END
		
		SET @ScoreIndex=@ScoreIndex + 1
	END

    return @ScoreVal
END

GO

----------------------------------------------------------------------------------------------------

CREATE FUNCTION FormatString(@ConfigScoreVal BIGINT)
RETURNS NVARCHAR(16)
AS
BEGIN
	DECLARE @BinaryString NVARCHAR(64)
	DECLARE @ConfigString NVARCHAR(16)
	SET @BinaryString=dbo.DecToBinary(@ConfigScoreVal, 64)
	SET @ConfigString=''
	
	DECLARE @CHAR1 NVARCHAR(4)
	DECLARE @CHAR2 NVARCHAR(4)
	DECLARE @ScoreIndex INT
	SET @ScoreIndex = LEN(@BinaryString) - 8 + 1
	
	WHILE @ScoreIndex >= 1
	BEGIN
		SET @CHAR1=SUBSTRING(@BinaryString, @ScoreIndex, 4)
		SET @CHAR2=SUBSTRING(@BinaryString, @ScoreIndex+4, 4)
		SET @ConfigString=@ConfigString+dbo.BinaryToHex(@CHAR1)+dbo.BinaryToHex(@CHAR2)
		SET @ScoreIndex = @ScoreIndex-8
	END
	
	RETURN @ConfigString
END

GO
