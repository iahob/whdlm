
----------------------------------------------------------------------------------------------------

USE WHQJGroupDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[IMF_GetTableID]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
DROP FUNCTION dbo.[IMF_GetTableID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_AddGroupMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_AddGroupMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_DeleteMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_DeleteMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_DeleteGroupMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_DeleteGroupMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_QueryGroupMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_QueryGroupMember]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_UpdateMemberBattleCount]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_UpdateMemberBattleCount]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ModifyRight]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ModifyRight]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_IM_ModifyInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_IM_ModifyInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

CREATE FUNCTION [dbo].[IMF_GetTableID]
(
   @dwGroupID BIGINT		--Ⱥ���ʶ    
)
RETURNS INT
AS
BEGIN
    RETURN (@dwGroupID-1)/20000
END

GO

----------------------------------------------------------------------------------------------------

-- ��ӳ�Ա
CREATE  PROCEDURE dbo.GSP_IM_AddGroupMember
	@dwGroupID BIGINT,			--Ⱥ���ʶ
	@dwMemberID INT,			--��Ա��ʶ	
	@lInvitationCode BIGINT		--������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- ִ���߼�
BEGIN
	
	-- ��ȡ����
	DECLARE @wMemberCount SMALLINT
	DECLARE @wMaxMemberCount SMALLINT
	DECLARE @dwCreaterID INT
	SELECT @wMemberCount=MemberCount, @wMaxMemberCount=MaxMemberCount, @dwCreaterID=CreaterID FROM IMGroupProperty WHERE GroupID=@dwGroupID

	-- ����У��
	IF @wMemberCount IS NULL OR @wMemberCount=@wMaxMemberCount
	BEGIN
		RETURN 1;
	END	

	-- ��ȡȺ���б�
	DECLARE @strGroupIDArray NVARCHAR(1024)
	SELECT @strGroupIDArray=GroupIDArray FROM IMUserGroupInfo WHERE UserID=@dwMemberID
	IF @strGroupIDArray IS NULL 
	BEGIN
		SET @strGroupIDArray=';'
		INSERT INTO IMUserGroupInfo VALUES (@dwMemberID,@strGroupIDArray)
	END

	-- ���ұ�ʶ
	DECLARE @strGroupID NVARCHAR(20)
	SET @strGroupID=';'+CONVERT(NVARCHAR(18),@dwGroupID)+';';	
	IF CHARINDEX(@strGroupID,@strGroupIDArray)>0
	BEGIN		
		RETURN 2
	END	

	DECLARE @nMemberRight INT
	SET @nMemberRight=0
	DECLARE @nAwardPoint INT
	SET @nAwardPoint=0
	if(@dwMemberID=@dwCreaterID)
	BEGIN
		SET @nMemberRight=1
		SELECT @nAwardPoint=OptionValue FROM IMGroupOption WHERE OptionName=N'CreatorPoint'
		IF @nAwardPoint IS NULL
			SET @nAwardPoint=90
	END
	INSERT INTO IMGroupMember(GroupID, UserID,MemberRight,CaptainID,AwardPoint) VALUES( CONVERT(NVARCHAR(18), @dwGroupID), CONVERT(NVARCHAR(12), @dwMemberID),@nMemberRight,0,@nAwardPoint)

	-- ����ɹ�
	IF @@ERROR=0
	BEGIN
		-- ���Ⱥ���ʶ		
		SET @strGroupIDArray=@strGroupIDArray+CONVERT(NVARCHAR(18),@dwGroupID)+';'
		UPDATE IMUserGroupInfo SET GroupIDArray=@strGroupIDArray WHERE UserID=@dwMemberID

		-- ��������
		UPDATE IMGroupProperty SET MemberCount=MemberCount+1 WHERE GroupID=@dwGroupID
		IF NOT EXISTS(SELECT 1 FROM IMGroupInvitationCode WHERE GroupID=@dwGroupID AND UserID=@dwMemberID)
			INSERT INTO IMGroupInvitationCode(GroupID,UserID) VALUES(@dwGroupID,@dwMemberID)

		DECLARE @SpreadGroupID BIGINT
		DECLARE @SpreadUserID BIGINT
		IF @lInvitationCode>0 AND @nMemberRight=0
		BEGIN
			SELECT @SpreadGroupID=GroupID,@SpreadUserID=UserID FROM IMGroupInvitationCode WHERE InvitationCode=@lInvitationCode
			IF @SpreadGroupID IS NOT NULL AND @SpreadGroupID=@dwGroupID
			BEGIN
				DECLARE @SpreadUserRight INT
				SELECT @SpreadUserRight=MemberRight FROM IMGroupMember WHERE GroupID=@SpreadGroupID AND UserID=@SpreadUserID
				IF (@SpreadUserRight&4)<>0
				BEGIN
					UPDATE IMGroupMember SET MemberRight=8,CaptainID=@SpreadUserID WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
					SET @nMemberRight=8
				END
			END
		END

		IF @SpreadUserID IS NULL
			SET @SpreadUserID=0
	END
	
	DECLARE @lMemberInvitationCode BIGINT
	SELECT @lMemberInvitationCode=InvitationCode FROM IMGroupInvitationCode WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	IF @lMemberInvitationCode IS NULL
		SET @lMemberInvitationCode=0

	DECLARE @AwardPoint INT
	SELECT @AwardPoint=AwardPoint FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	IF @AwardPoint IS NULL
		SET @AwardPoint=0
	-- ��ѯ��¼
	SELECT @dwMemberID AS MemberID,@lMemberInvitationCode AS InvitationCode,@AwardPoint AS AwardPoint, 0 AS BattleCount,@nMemberRight AS MemberRight,@SpreadUserID AS CaptainID, GetDate() AS JoinDateTime, UserID, GameID, Gender, NickName, CustomID FROM  WHQJAccountsDB.dbo.AccountsInfo WHERE UserID=@dwMemberID 

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ����Ա
CREATE  PROCEDURE dbo.GSP_IM_DeleteGroupMember
	@dwGroupID BIGINT,		--Ⱥ���ʶ
	@dwUserID	INT,		--�������
	@dwMemberID INT			--��Ա��ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- ִ���߼�
BEGIN

	DECLARE @UserRight INT
	SELECT @UserRight=MemberRight FROM IMGroupMember WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
	IF (@UserRight&4)<>0
	BEGIN
		--�öӳ�������Ǩ�Ƶ��ϼ�
		DECLARE @dwOldCaptain int
		SELECT @dwOldCaptain=CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID

		IF @dwOldCaptain=0
		BEGIN
			UPDATE IMGroupMember SET MemberRight=0 WHERE CaptainID=@dwMemberID AND GroupID=@dwGroupID AND MemberRight=8
		END

		UPDATE IMGroupMember SET CaptainID=@dwOldCaptain WHERE CaptainID=@dwMemberID AND GroupID=@dwGroupID
	END

	DELETE IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	--DELETE IMGroupInvitationCode WHERE GroupID=@dwGroupID AND Userid=@dwMemberID
	-- ɾ���ɹ�
	IF @@ROWCOUNT=1
	BEGIN
		-- ��ȡȺ����Ϣ
		DECLARE @strGroupIDArray NVARCHAR(1024)
		SELECT @strGroupIDArray=GroupIDArray FROM IMUserGroupInfo WHERE UserID=@dwMemberID

		-- �޸�Ⱥ����Ϣ
		DECLARE @strGroupID NVARCHAR(20)		
		SET @strGroupID=';'+CONVERT(NVARCHAR(18),@dwGroupID)+';';
		SET @strGroupIDArray=REPLACE(@strGroupIDArray,@strGroupID,';');

		-- ����Ⱥ��
		UPDATE IMUserGroupInfo SET GroupIDArray=@strGroupIDArray WHERE UserID=@dwMemberID		

		-- ��������
		UPDATE IMGroupProperty SET MemberCount=MemberCount-1 WHERE GroupID=@dwGroupID

		--����ɾ����¼
		IF @dwUserID <> 0
		BEGIN
			DECLARE @UserID		INT
			DECLARE @GameID		INT
			DECLARE @CustomID	INT
			DECLARE @NickName	NVARCHAR(32)
			SELECT @UserID=UserID, @GameID=GameID, @CustomID=CustomID, @NickName=NickName	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwMemberID

			DECLARE @CreaterID  INT
			DECLARE @GroupName  NVARCHAR(32)
			SELECT @CreaterID=CreaterID,@GroupName=GroupName FROM IMGroupProperty(NOLOCK) WHERE GroupID=@dwGroupID

			DECLARE @OperatorGameID		INT
			DECLARE @OperatorNickName	NVARCHAR(32)
			SELECT  @OperatorGameID=GameID,@OperatorNickName=NickName	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

			INSERT INTO IMGroupApplyRecord(GroupID, ApplyerID, ApplyerGameID, ApplyerNickName, ApplyerCustomID, ApplyStatus, CreaterID, GroupName,ApplyType,OperatorID,OperatorName, CollectDateTime)
			VALUES (@dwGroupID, @dwMemberID, @GameID, @NickName, @CustomID, 3, @CreaterID, @GroupName,1,@OperatorGameID,@OperatorNickName, GetDate())	
			DECLARE @RecordID INT
			SELECT 	@RecordID=SCOPE_IDENTITY()
			SELECT * FROM IMGroupApplyRecord WHERE RecordID=@RecordID
		END
	END	
	ELSE
	BEGIN
		return 1
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ����Ա
CREATE  PROCEDURE dbo.GSP_IM_DeleteMember
	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ	
	@strPassword NCHAR(32),						-- �û�����

	--Ⱥ����Ϣ
	@dwGroupID BIGINT,							-- Ⱥ���ʶ
	
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @UserID	INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT	
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

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
	SELECT @CreaterID=CreaterID FROM IMGroupProperty WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫ�Ĵ����˲����ڣ����֤���ٴγ��ԣ�'
		RETURN 5		
	END

	DECLARE @MemberRight  INT
	SELECT @MemberRight=MemberRight FROM IMGroupMember(NOLOCK) WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @MemberRight IS NULL
		SET @MemberRight=0

	IF @CreaterID<> @dwUserID
	BEGIN
		IF (@MemberRight&0x02)<>0 
		BEGIN
			DECLARE @ManagerRight INT
			DECLARE @Flag TINYINT
			SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
			IF @Flag<>1
				SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

			IF @ManagerRight IS NULL
				SET @ManagerRight=0
			IF (@ManagerRight&0x10)=0 -- ����û�����Ȩ��
			BEGIN
				SET @strErrorDescribe=N'����ʧ�ܣ�Ȩ�޲�����'	
				RETURN 4
			END
		END
		ELSE
		BEGIN
			SET @strErrorDescribe=N'����ʧ�ܣ�Ȩ�޲�����'	
			RETURN 4
		END
	END

	/*
	-- ��������	
	DECLARE	@Return_Value INT
	
	-- ��ӳ�Ա
	EXEC @Return_Value = GSP_IM_DeleteGroupMember
		 @dwGroupID = @dwGroupID,
		 @dwMemberID = @dwMemberID
	RETURN @Return_Value
	*/
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯȺ���Ա
CREATE  PROCEDURE dbo.GSP_IM_QueryGroupMember
	@dwGroupID BIGINT		--Ⱥ���ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ��������
DECLARE	@TableID	INT;																			
DECLARE	@TableName	NVARCHAR(128);								
DECLARE @Sqlstr		NVARCHAR(512);								

-- ִ���߼�
BEGIN
	
	-- ��ѯ���ݱ�
	SELECT a.UserID, a.GameID, a.NickName, a.Gender, a.CustomID,
	b.BattleCount, b.JoinDateTime, b.Score, b.MemberRight,b.CaptainID,b.FavoriteConfig,b.MemberManagerRight,b.ManagerRightFlag,b.AwardPoint,
	c.InvitationCode,b.UserStatus,b.QQ,b.WX,b.Phone
	FROM WHQJAccountsDB.dbo.AccountsInfo a,IMGroupMember b,IMGroupInvitationCode c
	WHERE a.UserID=b.UserID AND  b.GroupID=@dwGroupID AND b.GroupID=c.GroupID AND b.UserID=c.UserID


END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����Լս����
CREATE  PROCEDURE dbo.GSP_IM_UpdateMemberBattleCount
	@dwGroupID BIGINT,		--Ⱥ���ʶ	
	@dwMemberID BIGINT,		--��Ա��ʶ	
	@wIncBattleCount SMALLINT --��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ���½ű�		
	UPDATE IMGroupMember SET BattleCount=BattleCount+@wIncBattleCount WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
	-- ��ѯ�ű�		
	SELECT * FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID	
END

RETURN 0

GO


----------------------------------------------------------------------------------------------------

-- �޸�Ȩ��
CREATE  PROCEDURE dbo.GSP_IM_ModifyRight
	-- �û���Ϣ
	@dwOperatorID INT,							-- �û���ʶ	
	@strPassword NCHAR(32),						-- �û�����

	--Ⱥ����Ϣ
	@dwGroupID BIGINT,							-- Ⱥ���ʶ
	@dwMemberID BIGINT,							-- ��Ա��ʶ
	
	--������Ϣ
	@nRight INT,								--Ȩ��
	@cbMode TINYINT,							--ģʽ��0���ӣ�1�Ƴ�
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @UserID	INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT	
	DECLARE @LogonPass AS NCHAR(32)			

	-- ��ѯ�û�
	SELECT @UserID=UserID, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown 		
	FROM WHQJAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwOperatorID

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

	-- ��ѯ����
	DECLARE @CreaterID INT
	DECLARE @ManagerCount SMALLINT
	DECLARE @strManageUserIDArray nvarchar(512)
	
	SELECT @CreaterID=CreaterID FROM IMGroupProperty WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ҫ�Ĵ����˲����ڣ����֤���ٴγ��ԣ�'
		RETURN 5		
	END
	
	IF @nRight=2--�޸Ĺ���Ա
	BEGIN
		IF @dwOperatorID <> @CreaterID
		BEGIN
			SET @strErrorDescribe=N'�����Ǹô������������޷��޸�Ȩ�ޣ�'
			RETURN 6	
		END
	
		IF @dwOperatorID = @dwMemberID
		BEGIN
			SET @strErrorDescribe=N'���Ǹô����������������޸��Լ�Ȩ�ޣ�'
			RETURN 7	
		END
	END
	ELSE IF @nRight=4--�޸Ķӳ�
	BEGIN
		DECLARE @HaveRight INT 
		SET @HaveRight=0

		IF @CreaterID = @dwOperatorID
			SET @HaveRight=1
		ELSE
		BEGIN
			DECLARE @UserRight BIGINT
			SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperatorID
			IF @UserRight IS NULL
			BEGIN
				SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ���ã�'
				RETURN 6	
			END

			IF (@UserRight&2) <> 0--����Ա
			BEGIN
				DECLARE @ManagerRight BIGINT
				DECLARE @Flag TINYINT
				SELECT @Flag=ManagerRightFlag,@ManagerRight=MemberManagerRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwOperatorID
				IF @Flag<>1
					SELECT @ManagerRight = ManagerRight FROM IMGroupBattleRule WHERE GroupID=@dwGroupID

				IF (@ManagerRight&0x200)<>0 AND @cbMode=0
					SET @HaveRight=1
				ELSE IF (@ManagerRight&0x400)<>0 AND @cbMode=1
					SET @HaveRight=1
			END
			ELSE IF (@UserRight&0x04)<>0 AND @dwOperatorID=(SELECT CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID)
				SET @HaveRight=2
			
		END

		IF @HaveRight=0 
		BEGIN
			SET @strErrorDescribe=N'��Ǹ����û��Ȩ�޽��д˲�����'
			RETURN 6		
		END
	END

	
	--�ж����
	DECLARE @nCurRight INT
	SELECT @nCurRight= MemberRight FROM IMGroupMember WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
	IF  @nCurRight IS NULL 
	BEGIN
		SET @strErrorDescribe=N'����Ҳ��Ǵ����˳�Ա�����֤���ٴγ��ԣ�'
		RETURN 8	
	END
	
	if(@cbMode=0)--���Ȩ��
	BEGIN
		--����Ƿ��Ѿ��߱�Ȩ��
		IF (@nCurRight&@nRight)<>0
		BEGIN
			SET @strErrorDescribe=N'������Ѿ��߱���Ȩ�ޣ������ظ���ӣ�'
			RETURN 9	
		END
	
		IF @nRight=2
		BEGIN
			IF (@nCurRight)<>0--����Ƿ��Ѿ��߱�����Ȩ��
			BEGIN
				--SET @strErrorDescribe=N'USERID=['+RTRIM(LTRIM(STR(@dwMemberID)))+N' ]�Ѿ��߱�����Ȩ�ޣ��޷��޸ģ�'
				SET @strErrorDescribe=N'������Ѿ��߱�����Ȩ�ޣ��޷��޸ģ�'
				RETURN 9	
			END

			-- ������Ȩ��	
			UPDATE IMGroupMember SET MemberRight=@nRight WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
		END
		ELSE IF(@nRight=4)--�ӳ�Ȩ��
		BEGIN
			IF(@nCurRight>0 AND @nCurRight<>8)
			BEGIN
				SET @strErrorDescribe=N'������Ѿ��߱�����Ȩ�ޣ��޷��޸ģ�'
				RETURN 9	
			END

			UPDATE IMGroupMember SET MemberRight=4 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID

			IF @HaveRight=1
			BEGIN
				UPDATE IMGroupMember SET CaptainID=0,AwardPoint=0 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
			END
			ELSE IF @HaveRight=2
				UPDATE IMGroupMember SET CaptainID=@dwOperatorID,AwardPoint=0 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
		END




	END
	ELSE--�Ƴ�Ȩ��
	BEGIN
		--����Ƿ��Ѿ��߱�Ȩ��
		IF (@nCurRight&@nRight)=0
		BEGIN
			SET @strErrorDescribe=N'USERID=['+RTRIM(LTRIM(STR(@dwMemberID)))+N','+RTRIM(LTRIM(STR(@nCurRight)))+N' ]���߱���Ȩ��'+RTRIM(LTRIM(STR(@nRight)))+N'���޷��Ƴ���'
			RETURN 10	
		END
	
		-- �Ƴ����Ȩ��	
		UPDATE IMGroupMember SET MemberRight=(@nCurRight&~@nRight) WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
		IF @nRight=2
			UPDATE IMGroupMember SET ManagerRightFlag=0,MemberManagerRight=0 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
		ELSE IF(@nRight=4)--�Ƴ��ӳ�Ȩ��
		BEGIN
			UPDATE IMGroupMember SET AwardPoint=0 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
				
			--�öӳ�������Ǩ�Ƶ��ϼ�
			DECLARE @dwOldCaptain int
			SELECT @dwOldCaptain=CaptainID FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwMemberID

			IF @dwOldCaptain<>0
			BEGIN
				UPDATE IMGroupMember SET MemberRight=8 WHERE GroupID=@dwGroupID AND UserID=@dwMemberID
			END
			ELSE
			BEGIN
				UPDATE IMGroupMember SET MemberRight=0 WHERE CaptainID=@dwMemberID AND GroupID=@dwGroupID AND MemberRight=8
			END

			UPDATE IMGroupMember SET CaptainID=@dwOldCaptain WHERE CaptainID=@dwMemberID AND GroupID=@dwGroupID
		END
		ELSE IF (@nRight=8)--�Ƴ���Ա
		BEGIN
			UPDATE IMGroupMember SET CaptainID=0 WHERE UserID=@dwMemberID AND GroupID=@dwGroupID
		END
	END

	DECLARE @nMemberRight INT
	SELECT @nMemberRight=MemberRight FROM IMGroupMember WHERE UserID=@dwMemberID AND GroupID=@dwGroupID

	SELECT @nMemberRight AS MemberRight

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- �޸�Ȩ��
CREATE  PROCEDURE dbo.GSP_IM_ModifyInfo
	@dwGroupID BIGINT,							-- Ⱥ���ʶ
	@dwUserID BIGINT,							-- ��Ա��ʶ
	@cbFlag0 TINYINT,
	@cbFlag1 TINYINT,
	@cbFlag2 TINYINT,
	@szQQ nvarchar(32),							-- �û�QQ
	@szWX nvarchar(32),							-- �û�WX
	@szPhone nvarchar(11),						-- �û�phone
	-- �������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ		
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯ����
	DECLARE @CreaterID INT
	SELECT @CreaterID=CreaterID FROM IMGroupProperty WHERE GroupID=@dwGroupID

	-- ����ж�
	IF @CreaterID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����˲����ڣ����֤���ٴγ��ԣ�'
		RETURN 1		
	END

	DECLARE @UserRight BIGINT
	SELECT @UserRight = MemberRight FROM IMGroupMember WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @UserRight IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����Ǹô����˳�Ա����Ȩ���ã�'
		RETURN 2	
	END

	IF @cbFlag0=1
		UPDATE IMGroupMember set QQ=@szQQ WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @cbFlag1=1
		UPDATE IMGroupMember set WX=@szWX WHERE GroupID=@dwGroupID AND UserID=@dwUserID
	IF @cbFlag2=1
		UPDATE IMGroupMember set Phone=@szPhone WHERE GroupID=@dwGroupID AND UserID=@dwUserID

	SELECT @UserRight AS MemberRight
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

