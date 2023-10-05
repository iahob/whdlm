USE WHQJTreasureDB
GO



IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_InsertCreateRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_InsertCreateRecord]



SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

-----------------------------------------------------------------------
-- ���������¼
CREATE PROC GSP_GR_InsertCreateRecord
	@dwUserID INT,								-- �û� I D
	@dwServerID INT,							-- ���� ��ʶ
	@dwPersonalRoomID INT,						-- Լս�����ʶ
	@lCellScore INT,							-- ���� �׷�
	@dwDrawCountLimit INT,						-- ��������
	@dwDrawTimeLimit INT,						-- ʱ������
	@szPassWord NVARCHAR(15),					-- ���ӵ�ַ
	@wJoinGamePeopleCount TINYINT,				-- ��������
	@dwRoomTax BIGINT,							-- ˽�˷���˰��
	@strClientAddr NVARCHAR(15),
	@cbPayMode TINYINT,							-- ֧��ģʽ
	@lNeedRoomCard TINYINT,						-- ������Ҫ����ʯ
	@cbGameMode TINYINT,						-- ��Ϸģʽ
	@cbPlayMode		TINYINT,					-- ��Ϸģʽ 0 ���ֳ�  1 ��ҳ�
	@lGroupID		BIGINT,						-- ����ȦID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	DECLARE @Fee INT
	DECLARE @Nicname NVARCHAR(31)
	DECLARE @lPersonalRoomTax BIGINT

	-- ��ѯ���� �� �����Ƿ������Ϸ
	DECLARE @wKindID INT
	SELECT  @wKindID = KindID FROM WHQJPlatformDBLink.WHQJPlatformDB.dbo.GameRoomInfo  WHERE ServerID = @dwServerID 
	-- ��ѯ����
	SELECT @Fee=TableFee FROM WHQJPlatformDBLink.WHQJPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID AND  PlayMode= @cbPlayMode
	IF @cbPlayMode = 0
	BEGIN
		IF @Fee IS NULL OR @Fee=0
		BEGIN
			SET @strErrorDescribe=N'���ݿ��ѯ����ʧ�ܣ������³��ԣ�'
			RETURN 3
		END
	END
	ELSE
	BEGIN
		SET @Fee = 0
	END

	-- ��ȡ����������ҵ��ǳ�
	SELECT @Nicname =NickName FROM WHQJAccountsDBLink.WHQJAccountsDB.dbo.AccountsInfo WHERE UserID = @dwUserID
	IF @Nicname IS NULL
	SET @Nicname =''
		
	SELECT  @lPersonalRoomTax = PersonalRoomTax FROM WHQJPlatformDBLink.WHQJPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 
	
	--�����������ʯ����ѯ����˰��
	DECLARE @lTaxAgency BIGINT
	SELECT  @lTaxAgency = AgentScale FROM WHQJAccountsDBLink.WHQJAccountsDB.dbo.AccountsAgent WHERE UserID = @dwUserID 
	IF @lTaxAgency IS NOT NULL
	BEGIN
		SET @lPersonalRoomTax = @lTaxAgency
	END

	-- д�����ķſ���¼
	INSERT INTO WHQJPlatformDB..StreamCreateTableFeeInfo(UserID,NickName, ServerID, RoomID, CellScore, CountLimit,TimeLimit,CreateTableFee,CreateDate, TaxAgency, JoinGamePeopleCount,PayMode, RoomStatus, NeedRoomCard,GameMode,PlayMode,GroupID)
												VALUES(@dwUserID,@Nicname, @dwServerID, @dwPersonalRoomID, @lCellScore,@dwDrawCountLimit, @dwDrawTimeLimit, 0,GETDATE(), @dwRoomTax, @wJoinGamePeopleCount, @cbPayMode, 0, @lNeedRoomCard,@cbGameMode,@cbPlayMode,@lGroupID)	


END

RETURN 0
GO


