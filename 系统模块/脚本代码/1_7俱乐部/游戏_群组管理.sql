
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

-- ����Ⱥ��
CREATE  PROCEDURE dbo.GSP_IM_CreateGroup
	-- �û���Ϣ
	@dwCreaterID INT,							-- �û���ʶ
	@strPassword NCHAR(32),						-- �û�����

	-- Ⱥ����Ϣ
	@strGroupName NVARCHAR(32),					-- Ⱥ������
	@strGroupIntroduce NVARCHAR(128),			-- Ⱥ�����

	-- Ⱥ�����
	@cbPayType TINYINT,							-- ֧������	
	@cbCurrencyKind TINYINT,					-- ��������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			
	DECLARE @AgentID INT
	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown ,@AgentID=AgentID		 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ����뵽���¿�ͨ����ܼ���ʹ�ã�'
		RETURN 3
	END

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 4
	END
	-- ֧������
	IF @cbPayType=0 OR @cbPayType>3
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֧������ֵΪ��Чֵ�����������ã�'
		RETURN 5		
	END
	-- ��������
	DECLARE @MaxMemberCount SMALLINT
	DECLARE @MaxCreateGroupCount SMALLINT
	DECLARE @CreateGroupTakeIngot BIGINT
	DECLARE @CreateGroupDeductIngot BIGINT
	DECLARE @GroupPayType SMALLINT
	DECLARE @CreateSwitch SMALLINT
	DECLARE @CreateGroup SMALLINT
	DECLARE @OpenMedalMode SMALLINT
	DECLARE @CreatePresentMedal BIGINT
	-- ��ѯ����
	SELECT @MaxMemberCount=OptionValue FROM IMGroupOption WHERE OptionName=N'MaxMemberCount' 
	SELECT @MaxCreateGroupCount=OptionValue FROM IMGroupOption WHERE OptionName=N'MaxCreateGroupCount' 	
	SELECT @CreateGroupTakeIngot=OptionValue FROM IMGroupOption WHERE OptionName=N'CreateGroupTakeIngot' 	
	SELECT @CreateGroupDeductIngot=OptionValue FROM IMGroupOption WHERE OptionName=N'CreateGroupDeductIngot'		
	SELECT @GroupPayType=OptionValue FROM IMGroupOption WHERE OptionName=N'GroupPayType' 	
	SELECT @CreateSwitch=OptionValue FROM IMGroupOption WHERE OptionName=N'CreateSwitch' 
	SELECT @CreateGroup=OptionValue FROM IMGroupOption WHERE OptionName=N'CreateGroup' 		
	SELECT @OpenMedalMode=OptionValue FROM IMGroupOption WHERE OptionName=N'OpenMedalMode' 	
	SELECT @CreatePresentMedal=OptionValue FROM IMGroupOption WHERE OptionName=N'CreatePresentMedal' 					
	-- ��������
	IF @MaxMemberCount IS NULL SET @MaxMemberCount=500
	IF @MaxCreateGroupCount IS NULL SET @MaxCreateGroupCount=5
	IF @CreateGroupTakeIngot IS NULL SET @CreateGroupTakeIngot=0
	IF @CreateGroupDeductIngot IS NULL SET @CreateGroupDeductIngot=0
	IF @GroupPayType IS NULL SET @GroupPayType=1
	IF @CreateSwitch IS NULL SET @CreateSwitch=0
	IF @CreateGroup IS NULL SET @CreateGroup=3
	IF @OpenMedalMode IS NULL SET @OpenMedalMode=3
	IF @CreatePresentMedal IS NULL SET @CreatePresentMedal=0

	--1.��ͨ�û� 2.�����û� 3.ȫ��
	IF @CreateGroup<>3
	BEGIN
		IF @CreateGroup=1 AND @AgentID>0
		BEGIN
			SET @strErrorDescribe=N'����ʧ�ܣ�û�д����ʸ�'
			RETURN 1
		END
		ELSE IF @CreateGroup=2 AND @AgentID=0
		BEGIN
			SET @strErrorDescribe=N'����ʧ�ܣ�û�д����ʸ�'
			RETURN 1
		END
		ELSE IF NOT EXISTS(SELECT 1 FROM IMGroupWhiteList WHERE UserID=@dwCreaterID)
		BEGIN
			SET @strErrorDescribe=N'����ʧ�ܣ�û�д����ʸ�'
			RETURN 1
		END
	END

	--�жϹ���
	IF @GroupPayType<>3
	BEGIN
		IF @GroupPayType=1 AND @cbPayType=2
		BEGIN
			SET @strErrorDescribe=N'����ʧ�ܣ�ϵͳ��֧�����֧����'
			RETURN 6
		END
		
		IF @GroupPayType=2 AND @cbPayType=1
		BEGIN
			SET @strErrorDescribe=N'����ʧ�ܣ�ϵͳ��֧��Ⱥ��֧����'
			RETURN 7
		END
	END
	
	-- ��ѯ�Ƹ�
	DECLARE @UserIngot BIGINT
	DECLARE @UserScore BIGINT
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency(NOLOCK) WHERE UserID=@dwCreaterID
	SELECT @UserScore=Score FROM WHQJTreasureDB.dbo.GameScoreInfo(NOLOCK) WHERE UserID=@dwCreaterID
	-- �����Ƹ�
	IF @UserIngot IS NULL SET @UserIngot=0
	IF @UserScore IS NULL SET @UserScore=0
	
	IF @CreateGroupTakeIngot=1 OR @CreateGroupTakeIngot=3
	BEGIN
		-- �жϽ��
		IF @UserScore<@CreateGroupDeductIngot
		BEGIN
			SET @strErrorDescribe=N'����ʧ�ܣ���ǰ�ʺ����Ͻ������'+CONVERT(NVARCHAR(16),@CreateGroupDeductIngot)+',�޷����������ˣ�'
			RETURN 110
		END
	END
	
	IF @CreateGroupTakeIngot=2 OR @CreateGroupTakeIngot=4
	BEGIN
		-- �ж���ʯ
		DECLARE @MaxUserPay INT
		DECLARE @MaxGroupPay INT	

		SELECT @MaxUserPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @dwCreaterID AND GroupID=0 AND RoomStatus <> 2
		SELECT @MaxGroupPay = SUM(NeedRoomCard) FROM  WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE RoomStatus <> 2 
		AND GroupID in(Select GroupID FROM   WHQJGroupDB.dbo.IMGroupProperty where CreaterID=@dwCreaterID) 
		IF @MaxUserPay IS NULL SET @MaxUserPay=0
		IF @MaxGroupPay IS NULL	SET @MaxGroupPay=0

		IF @UserIngot<@CreateGroupDeductIngot+@MaxUserPay+@MaxGroupPay
		BEGIN
			SET @strErrorDescribe=N'����ʧ�ܣ���ǰ�ʺ�������ʯ����'+CONVERT(NVARCHAR(16),@CreateGroupDeductIngot+@MaxUserPay+@MaxGroupPay)+',�޷����������ˣ�'
			RETURN 111		
		END
	END

	-- ��������
	DECLARE @wCreateGroupCount SMALLINT
	SELECT @wCreateGroupCount=COUNT(GroupID) FROM IMGroupProperty(NOLOCK) WHERE CreaterID=@dwCreaterID AND GroupStatus<>0

	-- �ж�����
	IF @wCreateGroupCount>=@MaxCreateGroupCount
	BEGIN
		SET @strErrorDescribe=N'��Ǹ���������Ĵ����˸����Ѵﵽϵͳ���õ�����'+CONVERT(NVARCHAR(16),@MaxCreateGroupCount)+'��,�����˴���ʧ�ܣ�'
		RETURN 10		
	END

	-- У���������
	--IF EXISTS(SELECT GroupID FROM IMGroupProperty(NOLOCK) WHERE GroupName=@strGroupName)
	--BEGIN
	--	SET @strErrorDescribe=N'ϵͳ���Ѿ�������ͬ���ֵĴ����ˣ�����ʧ�ܣ�'
	--	RETURN 11		
	--END

	-- ����Ⱥ��
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
	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN	

	-- Ⱥ������
	INSERT INTO IMGroupProperty(CreaterID, CreaterGameID, CreaterNickName, GroupName, GroupIntroduce, MemberCount, MaxMemberCount,CanUseMedal, CreateDateTime)
	VALUES (@dwCreaterID, @GameID, @NickName, @strGroupName, @strGroupIntroduce, 0, @MaxMemberCount,@CanUseMedal, @CreateDateTime)
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- ������Ϣ
		SET @strErrorDescribe=N'����ϵͳԭ��,�����˴���ʧ��[0]������ϵ�ͻ����������˽���ϸ�����'
		RETURN 12
	END

	-- ��ѯȺ��
	DECLARE @dwGroupID BIGINT
	SELECT 	@dwGroupID=SCOPE_IDENTITY()

	-- Ⱥ�����
	INSERT INTO IMGroupBattleRule(GroupID, PayType, CurrencyKind,MedalShow,CurMedalShow,MedalRevenueShow,ManagerRight,MedalMode) VALUES (@dwGroupID, @cbPayType, @cbCurrencyKind,1,1,1,10,@CanUseMedal)	
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- ������Ϣ
		SET @strErrorDescribe=N'����ϵͳԭ��,�����˴���ʧ��[1]������ϵ�ͻ����������˽���ϸ�����'
		RETURN 13
	END

	-- Ⱥ��Ƹ�
	INSERT INTO IMGroupWealth(GroupID, Ingot,Score) VALUES(@dwGroupID, 0,@CreatePresentMedal)
	IF @@ERROR<>0
	BEGIN
		-- ����ع�
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		-- ������Ϣ
		SET @strErrorDescribe=N'����ϵͳԭ��,�����˴���ʧ��[2]������ϵ�ͻ����������˽���ϸ�����'
		RETURN 14
	END 
	--��ʼ���ͼ�¼	
	INSERT INTO RecordGroupScoreChange(GroupID,BeforeScore,ChangeType,ChangeScore,AfterScore,CollectDate) VALUES(@dwGroupID,0,2,@CreatePresentMedal,@CreatePresentMedal,@CreateDateTime)

	-- �۳����
	IF @CreateGroupDeductIngot>0 AND @CreateGroupTakeIngot=1
	BEGIN
		--���½��
		UPDATE WHQJTreasureDB.dbo.GameScoreInfo SET Score=Score-@CreateGroupDeductIngot WHERE UserID=@dwCreaterID
	END
	
	-- �۳���ʯ
	IF @CreateGroupDeductIngot>0 AND @CreateGroupTakeIngot=2
	BEGIN
		-- �����¼
		INSERT INTO WHQJTreasureDB.dbo.RecordCurrencyChange (UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
		VALUES (@dwCreaterID,-@CreateGroupDeductIngot,14,@UserIngot,@UserIngot-@CreateGroupDeductIngot,N'-------------',GETDATE(),N'����������')	

		-- ������ʯ
		UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond-@CreateGroupDeductIngot WHERE UserID=@dwCreaterID
	END	


	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED		

	-- ��ѯ�Ƹ�	
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency(NOLOCK) WHERE UserID=@dwCreaterID

	-- �����Ƹ�
	IF @UserIngot IS NULL SET @UserIngot=0
	
	--����Ĭ�Ϲ��������
	DECLARE @tablePersonalRoomInfo TABLE
	(
		KindID INT,
		PlayMode TINYINT,
		ShareStorageRule nvarchar(1024)
	);
	
	-- ��������
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

	-- ��ѯ��Ϣ
	SELECT A.GroupID, A.CreaterID, A.CreaterGameID, A.CreaterNickName, A.GroupName, A.GroupLevel,A.GroupStatus, A.GroupIntroduce, A.MemberCount, A.MaxMemberCount, A.CreateDateTime,
	 B.PayType, B.CurrencyKind,B.ManagerRight,B.CaptainRight,B.InMode,B.OutMode,B.FreeTableLimit,B.BigWinnerShow,B.MedalMode,B.MedalShow,B.CurMedalShow,B.MedalRevenueShow,B.MedalClear,B.GroupIDShow,
	  C.Ingot, @UserIngot AS UserIngot
	FROM IMGroupProperty AS A, IMGroupBattleRule AS B, IMGroupWealth AS C 
	WHERE A.GroupID=B.GroupID AND A.GroupID=C.GroupID AND A.GroupID=@dwGroupID	
	
END

RETURN 0

GO



----------------------------------------------------------------------------------------------------

-- ɾ��Ⱥ��
CREATE  PROCEDURE dbo.GSP_IM_DeleteGroup
	-- �û���Ϣ
	@dwCreaterID INT,							-- �û���ʶ
	@strPassword NCHAR(32),						-- �û�����
	@dwGroupID BIGINT,							-- Ⱥ���ʶ
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ��������
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);	

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ����뵽���¿�ͨ����ܼ���ʹ�ã�'
		RETURN 3
	END

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 4
	END

	-- ��ѯȺ��
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫɾ���Ĵ����˲����ڣ����֤���ٴγ��ԣ�'
		RETURN 5		
	END

	-- ����Ⱥ��
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵�������û��Ȩ�޽��д˲�����'
		RETURN 5		
	END
	
	-- ��������
	DECLARE @UserIngot BIGINT

	-- ��ѯ�Ƹ�
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwCreaterID	

	-- �����Ƹ�
	IF @UserIngot IS NULL SET @UserIngot=0


	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��������
	DECLARE @GroupIngot BIGINT	
	

	-- ��ѯ�Ƹ�	
	SELECT @GroupIngot=Ingot FROM IMGroupWealth WITH (UPDLOCK) WHERE GroupID=@dwGroupID

	-- �����Ƹ�	
	IF @GroupIngot IS NULL SET @GroupIngot=0

	-- ȡ����ʯ
	IF @GroupIngot>0
	BEGIN
		-- �����¼
		INSERT INTO WHQJTreasureDB.dbo.RecordCurrencyChange (UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
		VALUES (@dwCreaterID,@GroupIngot,12,@UserIngot,@UserIngot+@GroupIngot,N'-------------',GETDATE(),N'ɾ��������')	
		--�����¼
		INSERT INTO RecordGroupIngotChange(GroupID,BeforeIngot,TradeType,TradeIngot,TradeRoomGUID,CollectDate) 
		VALUES(@dwGroupID,@GroupIngot,4,-@GroupIngot,'0',GETDATE())
		-- ������ʯ
		UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond+@GroupIngot WHERE UserID=@dwCreaterID

		-- ���²Ƹ���Ϣ
		UPDATE IMGroupWealth SET Ingot=Ingot-@GroupIngot WHERE GroupID=@dwGroupID
	END

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
	-- �޸�״̬
	--DELETE IMGroupProperty WHERE  GroupID=@dwGroupID
	UPDATE IMGroupProperty SET GroupStatus=0 WHERE GroupID=@dwGroupID


	-- �����ʶ
	DECLARE @strGroupID NVARCHAR(20)
	SET @strGroupID=';'+CONVERT(NVARCHAR(18),@dwGroupID)+';';
	
	UPDATE IMUserGroupInfo SET GroupIDArray=REPLACE(GroupIDArray,@strGroupID,';') WHERE UserID IN (SELECT UserID FROM IMGroupMember WHERE GroupID=@dwGroupID)
	-- ��ѯ�Ƹ�
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwCreaterID	

	-- �����Ƹ�
	IF @UserIngot IS NULL SET @UserIngot=0
	
	--ɾ��Ĭ�Ϲ��������
	DELETE FROM IMGroupShareStorageProperty WHERE GroupID=@dwGroupID
	
	SELECT 	@UserIngot AS UserIngot					  
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ô�����״̬
CREATE  PROCEDURE dbo.GSP_IM_SetGroupStatus
	-- �û���Ϣ
	@dwUserID	 INT,							-- �û���ʶ
	@strPassword NCHAR(32),						-- �û�����
	@dwGroupID BIGINT,							-- Ⱥ���ʶ
	@cbStatus TINYINT,							--״̬1��Ч��2��ɢ��3����
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ��������
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);	

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ����뵽���¿�ͨ����ܼ���ʹ�ã�'
		RETURN 3
	END

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 4
	END

	-- ��ѯȺ��
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND GroupStatus<>0

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫɾ���Ĵ����˲����ڣ����֤���ٴγ��ԣ�'
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
			SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ���ã�'
			RETURN 6	
		END

		IF (@UserRight&2) <> 0--����Ա
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
		SET @strErrorDescribe=N'��Ǹ����û��Ȩ�޽��д˲�����'
		RETURN 6		
	END
	
	DECLARE @CurStatus TINYINT
	SELECT @CurStatus=GroupStatus From IMGroupProperty WHERE GroupID=@dwGroupID
	IF @cbStatus<>1 AND @cbStatus<>2
	BEGIN
		SET @strErrorDescribe=N'��Ǹ������״̬�������飡'
		RETURN 7	
	END

	-- �޸�״̬
	UPDATE IMGroupProperty SET GroupStatus=@cbStatus WHERE GroupID=@dwGroupID
		  
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- ���²Ƹ�
CREATE  PROCEDURE dbo.GSP_IM_UpdateGroupWealth
	-- �û���Ϣ
	@dwCreaterID INT,							-- �û���ʶ
	@strPassword NCHAR(32),						-- �û�����
	@dwGroupID BIGINT,							-- Ⱥ���ʶ

	-- Ⱥ��Ƹ�	
	@lIngot BIGINT,								-- ��ʯ����
	
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ����뵽���¿�ͨ����ܼ���ʹ�ã�'
		RETURN 3
	END

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 4
	END

	-- ��ѯȺ��
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND GroupStatus<>0

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫɾ���Ĵ����˲����ڣ����֤���ٴγ��ԣ�'
		RETURN 5		
	END

	-- ����Ⱥ��
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵�������û��Ȩ�޽��д˲�����'
		RETURN 5		
	END

	-- ��������
	DECLARE @UserIngot BIGINT
	
	-- ��ѯ�Ƹ�
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwCreaterID	

	-- �����Ƹ�
	IF @UserIngot IS NULL SET @UserIngot=0

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��������
	DECLARE @GroupIngot BIGINT	

	-- ��ѯ�Ƹ�	
	SELECT @GroupIngot=Ingot FROM IMGroupWealth WITH (UPDLOCK) WHERE GroupID=@dwGroupID

	-- �����Ƹ�	
	IF @GroupIngot IS NULL SET @GroupIngot=0

	-- ������ʯ
	IF @lIngot>0
	BEGIN		
		IF @UserIngot<@lIngot
		BEGIN
			-- ����ع�
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			-- ������Ϣ
			SET @strErrorDescribe=N'��ʯ����,�洢ʧ�ܣ�'
			RETURN 4		
		END

		-- ��Ҫ���ĵ���ʯ����
		DECLARE @UserNeedPayIngot BIGINT

		-- ��������Ҫ���ĵ���ʯ
		SELECT @UserNeedPayIngot = SUM(NeedRoomCard) FROM WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @dwCreaterID AND GroupID<>@dwGroupID AND RoomStatus = 0

		-- ʣ�µ���ʯ�Ƿ�֧�� 
		IF @UserNeedPayIngot IS NOT NULL AND @UserIngot - @UserNeedPayIngot < @lIngot
		BEGIN
			-- ����ع�
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			-- ������Ϣ
			SET @strErrorDescribe=N'���з���δ֧����ɣ����ܴ����������ʯ��'
			RETURN 4
		END
		
		----����δ֧���ķ���
		--IF EXISTS (SELECT * FROM WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE UserID = @dwCreaterID AND GroupID<>@dwGroupID AND RoomStatus = 0)
		--BEGIN
		--	-- ����ع�
		--	ROLLBACK TRAN
		--	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
		--	-- ������Ϣ
		--	SET @strErrorDescribe=N'���з���δ֧����ɣ����ܴ����������ʯ��'
		--	RETURN 4
		--END
		
		-- �����¼
		INSERT INTO WHQJTreasureDB.dbo.RecordCurrencyChange (UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
		VALUES (@dwCreaterID,-@lIngot,11,@UserIngot,@UserIngot-@lIngot,N'-------------',GETDATE(),N'������ʯ')	

		--�����¼
		INSERT INTO RecordGroupIngotChange(GroupID,BeforeIngot,TradeType,TradeIngot,TradeRoomGUID,CollectDate) 
		VALUES(@dwGroupID,@GroupIngot,1,@lIngot,'0',GETDATE())
		-- ������ʯ
		UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond-@lIngot WHERE UserID=@dwCreaterID

		-- ���²Ƹ���Ϣ
		UPDATE IMGroupWealth SET Ingot=Ingot+@lIngot WHERE GroupID=@dwGroupID
	END

	-- ȡ����ʯ
	IF @lIngot<0
	BEGIN
		IF @GroupIngot+@lIngot<0
		BEGIN
			-- ����ع�
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			-- ������Ϣ
			SET @strErrorDescribe=N'�����˵���ʯ����,��ʯ��ȡʧ�ܣ�'
			RETURN 4		
		END
		--����δ֧���ķ���
		IF EXISTS (SELECT * FROM WHQJPlatformDB.dbo.StreamCreateTableFeeInfo WHERE GroupID=@dwGroupID AND RoomStatus = 0)
		BEGIN
			-- ����ع�
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED
			
			-- ������Ϣ
			SET @strErrorDescribe=N'�������з���δ֧����ɣ�������ȡ��������ʯ��'
			RETURN 4
		END
		
		
		-- �����¼
		INSERT INTO WHQJTreasureDB.dbo.RecordCurrencyChange (UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
		VALUES (@dwCreaterID,-@lIngot,12,@UserIngot,@UserIngot-@lIngot,N'-------------',GETDATE(),N'ȡ����ʯ')	
		--�����¼
		INSERT INTO RecordGroupIngotChange(GroupID,BeforeIngot,TradeType,TradeIngot,TradeRoomGUID,CollectDate) 
		VALUES(@dwGroupID,@GroupIngot,2,@lIngot,'0',GETDATE())
		-- ������ʯ
		UPDATE WHQJTreasureDB.dbo.UserCurrency SET Diamond=Diamond-@lIngot WHERE UserID=@dwCreaterID

		-- ���²Ƹ���Ϣ
		UPDATE IMGroupWealth SET Ingot=Ingot+@lIngot WHERE GroupID=@dwGroupID
	END

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ��ѯ�Ƹ�
	SELECT @UserIngot=Diamond FROM WHQJTreasureDB.dbo.UserCurrency WHERE UserID=@dwCreaterID	

	-- �����Ƹ�
	IF @UserIngot IS NULL SET @UserIngot=0
	
	-- ��ѯ��Ϣ
	SELECT GroupID, Ingot, @UserIngot AS UserIngot FROM IMGroupWealth(NOLOCK) WHERE GroupID=@dwGroupID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE  PROCEDURE dbo.GSP_IM_UpdateGroupProperty
	-- �û���Ϣ
	@dwCreaterID INT,							-- �û���ʶ
	@strPassword NCHAR(32),						-- �û�����
	@dwGroupID BIGINT,							-- Ⱥ���ʶ

	-- Ⱥ�����
	@strGroupIntroduce NVARCHAR(128),			-- Ⱥ�����	
	
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ����뵽���¿�ͨ����ܼ���ʹ�ã�'
		RETURN 3
	END

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 4
	END

	-- ��ѯȺ��
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND GroupStatus<>0

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫɾ���Ĵ����˲����ڣ����֤���ٴγ��ԣ�'
		RETURN 5		
	END

	-- ����Ⱥ��
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵�������û��Ȩ�޽��д˲�����'
		RETURN 5		
	END

	-- ���¹���
	UPDATE IMGroupProperty SET GroupIntroduce=@strGroupIntroduce WHERE GroupID=@dwGroupID

	-- ��ѯ��Ϣ
	SELECT * FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���¹���
CREATE  PROCEDURE dbo.GSP_IM_UpdateGroupBattleRule
	-- �û���Ϣ
	@dwCreaterID INT,							-- �û���ʶ
	@strPassword NCHAR(32),						-- �û�����
	@dwGroupID BIGINT,							-- Ⱥ���ʶ

	-- Ⱥ�����
	@cbPayType TINYINT,							-- ֧����ʽ
	@cbCurrencyKind TINYINT,					-- ��������
	
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �û���Ϣ
DECLARE @UserID		INT
DECLARE @GameID		INT
DECLARE @NickName	NVARCHAR(32)

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwCreaterID

	-- �û�������
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ����뵽���¿�ͨ����ܼ���ʹ�ã�'
		RETURN 3
	END

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ��������������֤���ٴγ��ԣ�'
		RETURN 4
	END

	-- ��ѯȺ��
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID AND GroupStatus<>0

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫ���µĴ����˲����ڣ����֤���ٴγ��ԣ�'
		RETURN 5		
	END

	-- ����Ⱥ��
	IF @CreaterID <> @dwCreaterID 
	BEGIN
		SET @strErrorDescribe=N'��Ǹ�������Ǹô����˵�������û��Ȩ�޽��д˲�����'
		RETURN 5		
	END

	-- ֧������
	IF @cbPayType=0 OR @cbPayType>3
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֧������ֵΪ��Чֵ�����������ã�'
		RETURN 6		
	END
	-- ��ѯ����
	DECLARE @GroupPayTypeChange SMALLINT
	DECLARE @GroupPayType SMALLINT
	SELECT @GroupPayTypeChange=OptionValue FROM IMGroupOption WHERE OptionName=N'GroupPayTypeChange'
	SELECT @GroupPayType=OptionValue FROM IMGroupOption WHERE OptionName=N'GroupPayType' 
	
	IF @GroupPayTypeChange IS NULL  SET @GroupPayTypeChange=0
	IF @GroupPayType IS NULL  SET @GroupPayType=1
	
	IF @GroupPayTypeChange=0
		BEGIN
		SET @strErrorDescribe=N'ϵͳ��ֹ�޸�֧������'
		RETURN 5				
	END
	
	-- У��֧����ʽ
	IF @GroupPayType=1 AND @cbPayType=2
	BEGIN
		SET @strErrorDescribe=N'�����õ�֧��������ϵͳ���ò���������������'
		RETURN 5
	END
		
	IF @GroupPayType=2 AND @cbPayType=1
	BEGIN
		SET @strErrorDescribe=N'�����õ�֧��������ϵͳ���ò��������������ã�'
		RETURN 5
	END

	-- ���¹���
	UPDATE IMGroupBattleRule SET PayType=@cbPayType WHERE GroupID=@dwGroupID

	-- ��ѯ��Ϣ
	SELECT * FROM IMGroupBattleRule(NOLOCK) WHERE GroupID=@dwGroupID	
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯѫ��ģʽ
CREATE  PROCEDURE dbo.GSP_IM_QueryMedalMode
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @cbMedalMode TINYINT
	SELECT @cbMedalMode=MedalMode FROM IMGroupBattleRule(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @cbMedalMode IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
		RETURN 1		
	END

	IF NOT EXISTS(SELECT 1 FROM IMGroupMember WHERE UserID=@dwUserID AND GroupID=@dwGroupID)
	BEGIN
		SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ��ѯ��'
		RETURN 2	
	END
	
	SELECT @cbMedalMode AS MedalMode
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE  PROCEDURE dbo.GSP_IM_SetInOut
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@cbInMode		TINYINT,		-- ��������
	@cbInType		TINYINT,		-- ���뷽ʽ
	@cbOutMode		TINYINT,		-- �˳�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
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
			SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ���ã�'
			RETURN 2	
		END

		IF (@UserRight&2) <> 0--����Ա
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
		SET @strErrorDescribe=N'����ʧ�ܣ���Ȩ���ã�'
		RETURN 3		
	END

	UPDATE IMGroupBattleRule SET InMode=@cbInMode,OutMode=@cbOutMode,GroupIDShow=@cbInType WHERE GroupID=@dwGroupID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- Ȩ�޹���
CREATE  PROCEDURE dbo.GSP_IM_RightManagement
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- ��ұ�ʶ	
	@dwTargetID		INT,			-- Ŀ����ұ�ʶ	
	@dwManagerRight	BIGINT,			-- ����ԱȨ��
	@dwCaptainRight	BIGINT,			-- �ӳ�Ȩ��
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
		RETURN 1		
	END

	IF @dwCreaterID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'�����Ǹô�������������Ȩ���ã�'
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
			SET @strErrorDescribe=N'Ŀ���û����Ǹô����˳�Ա�����飡'
			RETURN 3
		END

		IF (@MemberRight&0x02)=0
		BEGIN
			SET @strErrorDescribe=N'Ŀ���û����ǹ���Ա��'
			RETURN 4
		END

		UPDATE IMGroupMember SET MemberManagerRight=@dwManagerRight,ManagerRightFlag=1 WHERE GroupID=@dwGroupID AND UserID=@dwTargetID
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- Ȩ�޹���
CREATE  PROCEDURE dbo.GSP_IM_QueryRight
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
		RETURN 1		
	END

	IF @dwCreaterID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'�����Ǹô�������������Ȩ���ã�'
		RETURN 2		
	END

	SELECT ManagerRight,CaptainRight  FROM IMGroupBattleRule WHERE GroupID=@dwGroupID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ѫ������
CREATE  PROCEDURE dbo.GSP_IM_SetMedal
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@cbMedalMode	TINYINT,		-- ѫ�¿���
	@cbMedalShow	TINYINT,		-- ѫ����ʾ
	@cbCurMedalShow	TINYINT,		-- ��ǰѫ����ʾ
	@cbMedalRevenueShow	TINYINT,	-- ѫ�������ʾ
	@cbMedalClear	TINYINT,		-- ѫ������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
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
			SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ���ã�'
			RETURN 2	
		END

		IF (@UserRight&2) <> 0--����Ա
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
		SET @strErrorDescribe=N'����ʧ�ܣ���Ȩ���ã�'
		RETURN 3		
	END

	IF @cbMedalMode=1
	BEGIN
		DECLARE @CanUseMedal TINYINT
		SELECT @CanUseMedal=CanUseMedal FROM IMGroupProperty WHERE GroupID=@dwGroupID
		IF @CanUseMedal=0
		BEGIN
			SET @strErrorDescribe=N'�ô������޷�����ѫ�£�����ϵ��Ӫ�̣�'
			RETURN 4
		END
	END

	UPDATE IMGroupBattleRule SET MedalMode=@cbMedalMode,MedalShow=@cbMedalShow,CurMedalShow=@cbCurMedalShow,MedalRevenueShow=@cbMedalRevenueShow,MedalClear=@cbMedalClear
		WHERE GroupID=@dwGroupID

	--�ر�ѫ��ģʽ����ɾ��ѫ���淨
	IF @cbMedalMode=0
	BEGIN
		DELETE FROM IMGroupConfig WHERE PlayMode=1 AND GroupID=@dwGroupID
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- ��Ӯ������
CREATE  PROCEDURE dbo.GSP_IM_SetBigWinner
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@cbShowMode		TINYINT,		-- ģʽ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
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
			SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ���ã�'
			RETURN 2	
		END

		IF (@UserRight&2) <> 0--����Ա
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
		SET @strErrorDescribe=N'����ʧ�ܣ���Ȩ���ã�'
		RETURN 3		
	END

	UPDATE IMGroupBattleRule SET BigWinnerShow=@cbShowMode WHERE GroupID=@dwGroupID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- ��������
CREATE  PROCEDURE dbo.GSP_IM_SetFreeTableLimit
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@cbCount		TINYINT,		-- ��������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
		RETURN 1		
	END

	DECLARE @HaveRight INT 
	SET @HaveRight=0

	IF @dwCreaterID = @dwUserID
		SET @HaveRight=1

	IF @HaveRight=0
	BEGIN
		SET @strErrorDescribe=N'����ʧ�ܣ���Ȩ���ã�'
		RETURN 3		
	END


	UPDATE IMGroupBattleRule SET FreeTableLimit=@cbCount WHERE GroupID=@dwGroupID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �����
CREATE  PROCEDURE dbo.GSP_IM_SetRollMsg
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
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
			SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ��������ƣ�'
			RETURN 2	
		END

		IF (@UserRight&2) <> 0--����Ա
		BEGIN
			DECLARE @ManagerRight BIGINT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@ManagerRight&0x08)<>0 
				SET @HaveRight=1
		END
		ELSE IF (@UserRight&4)<>0--�ӳ�
		BEGIN
			DECLARE @CaptainRight BIGINT
			SELECT @CaptainRight = CaptainRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF (@CaptainRight&0x80)<>0 
				SET @HaveRight=2
		END
			
	END

	IF @HaveRight=0
	BEGIN
		SET @strErrorDescribe=N'���������ʧ�ܣ�Ȩ�޲��㣡'
		RETURN 3		
	END

	DECLARE @nValue INT
	-- ��ѯ����
	SELECT @nValue=OptionValue FROM IMGroupOption WHERE OptionName=N'RollMsgTime' 
	
	SELECT @nValue AS RollTime,@HaveRight AS MsgType
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �޸Ĵ���������
CREATE  PROCEDURE dbo.GSP_IM_ModifyGroupName
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@szGroupName	NVARCHAR(32),	-- ����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
		RETURN 1		
	END


	IF @dwCreaterID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'�����Ǹô�������������Ȩ�޸����ƣ�'
		RETURN 2	
	END

	UPDATE IMGroupProperty SET GroupName=@szGroupName WHERE GroupID=@dwGroupID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ô�������˽
CREATE  PROCEDURE dbo.GSP_IM_SetPrivacy
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@cbIDShow		TINYINT,		-- ID��ʾ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @dwCreaterID INT
	SELECT @dwCreaterID=CreaterID FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @dwCreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��������Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
		RETURN 1		
	END


	IF @dwCreaterID <> @dwUserID
	BEGIN
		SET @strErrorDescribe=N'�����Ǹô�������������Ȩ�޸���˽���ã�'
		RETURN 2	
	END

	UPDATE IMGroupBattleRule SET GroupIDShow=@cbIDShow WHERE GroupID=@dwGroupID

END

RETURN 0

GO
----------------------------------------------------------------------------------------------------


-- ��ѯ����
CREATE  PROCEDURE dbo.GSP_IM_QueryScore
	@dwGroupID		INT,			-- Ⱥ���ʶ
	@dwUserID		INT,			-- Ⱥ���ʶ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯȺ��
	DECLARE @nRight INT
	SELECT @nRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	-- ����ж�
	IF @nRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����Ϣ��ѯʧ�ܣ����֤���ٴγ��ԣ�'
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
		SET @strErrorDescribe=N'��û��Ȩ���޸����ѫ�£�'
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
	
	--��ȡԭ��
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
	
	--����ת��
	IF @Negative = 1
	BEGIN

		DECLARE @TruncateStr NVARCHAR(64)	
		DECLARE @NegativeStr NVARCHAR(64)	
		DECLARE @TempChar NVARCHAR(1)
		DECLARE @OffsetIndex TINYINT    
		
		SET @NegativeStr='1'
		SET @OffsetIndex=1		
		SET @TruncateStr=SUBSTRING(@BinStr, 2, LEN(@BinStr)-1)
		
		--��������
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
		
		--��������(����+1)
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
				
				--�滻ָ��λ��
				SET @BinStr='1'+SUBSTRING(@TruncateStr,1,LEN(@TruncateStr)-@ReverseIndex)+@TempChar+SUBSTRING(@TruncateStr,@OffsetIndex+1,LEN(@TruncateStr)-@OffsetIndex)
				SET @TruncateStr=SUBSTRING(@TruncateStr,1,LEN(@TruncateStr)-@ReverseIndex)+@TempChar+SUBSTRING(@TruncateStr,@OffsetIndex+1,LEN(@TruncateStr)-@OffsetIndex)
				
				BREAK
			END
			ELSE IF CAST(@TempChar AS TINYINT) + 1 > 1
			BEGIN
				SET @TempChar=0
				
				--�滻ָ��λ��
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
