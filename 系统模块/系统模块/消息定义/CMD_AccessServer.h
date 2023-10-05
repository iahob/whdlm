#ifndef CMD_ACCESS_HEAD_FILE
#define CMD_ACCESS_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//��Ա��ݶ���
#define GP_MEMBER					0x00								//��ͨ��Ա
#define GP_CREATOR					0x01								//Ȧ��
#define GP_MANAGER					0x02								//����Ա
#define GP_CAPTAIN					0x04								//�ӳ�
#define GP_TEAMER					0x08								//��Ա

//����ԱȨ�޶���
#define MR_1						0x01									//��������Ȧ
#define MR_2						0x02									//��������
#define MR_3						0x04									//���ù���
#define MR_4						0x08									//���������
#define MR_5						0x10									//�û�����
#define MR_6						0x20									//ѫ��ģʽ����
#define MR_7						0x40									//�淨����
#define MR_8						0x80									//���ޱ��
#define MR_9						0x100									//�ӳ�Ȩ��
#define MR_10						0x200									//��Ӷӳ�
#define MR_11						0x400									//����ӳ�
#define MR_12						0x800									//�����Ա
#define MR_13						0x1000									//�����Ա
#define MR_14						0x2000									//��Ӯ������
//�ӳ�Ȩ�޶���
#define CR_1						0x01									//ѫ�¿���
#define CR_2						0x02									//ѫ����ʾ
#define CR_3						0x04									//ѫ���޸�
#define CR_4						0x08									//���ޱ��
#define CR_5						0x10									//�����Ա
#define CR_6						0x20									//��Ӷ�Ա
#define CR_7						0x40									//�����Ա
#define CR_8						0x80									//������Ϣ

//Լս����
#define MDM_GA_BATTLE_SERVICE		1									//Լս����

//���Ӳ���
#define SUB_GA_QUERY_TABLE			110									//��ѯ����
#define SUB_GA_CREATE_TABLE			111									//��������
#define SUB_GA_DISMISS_TABLE		112									//��ɢ����
#define SUB_GA_GET_TABLE_USERLIST	113									//��ȡ�б�
#define SUB_GA_SEARCH_SERVER_TABLE	114									//���Ų�ѯ
#define SUB_GA_QUERY_GAME_SERVER	115									//��ȡ�����б�
//�������
#define SUB_GA_TABLE_ITEM			200									//������Ϣ
#define SUB_GA_TABLE_LIST			201									//�����б�
#define SUB_GA_OPERATE_FAILED		202									//����ʧ��
#define SUB_GA_BATTLE_RECORD		203									//Լս��¼
#define SUB_GA_TABLE_PARAMLIST		204									//���Ӳ���
#define SUB_GA_TABLE_USERLIST		205									//�û��б�
#define SUB_GA_DISMISS_RESULT		206									//��ɢ���
#define SUB_GA_TABLE_LIST_FINISH	207									//�б����

//��Ϣ����
#define SUB_GA_USER_TABLE_STATUS	300									//�û�����
#define SUB_GA_TABLE_START			301									//���ӿ�ʼ
#define SUB_GA_TABLE_PARAM			302									//���Ӳ���
#define SUB_GA_TABLE_CREATE			303									//���Ӵ���
#define SUB_GA_TABLE_DISMISS		304									//���ӽ�ɢ
#define SUB_GA_TABLE_CHANGEBANKER	305									//�л�ׯ��

//////////////////////////////////////////////////////////////////////////////////
//��ѯ����
struct CMD_GA_QueryGameServer
{
	DWORD							dwGroupID;							//����Ȧ��ʶ
	WORD							wQueryBeginIndex;					//��ѯ��ʼ
	WORD							wQueryCount;						//
};
//��ѯ����
struct CMD_GA_QueryTable
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwGroupID;							//����Ȧ��ʶ
};

//��������
struct CMD_GA_CreateTable
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwStationID;						//վ���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwGroupCreaterID;					//Ⱥ����ʶ

	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ	
	WORD							wPayType;							//֧������
	WORD							wPlayerCount;						//��Ϸ����
	BYTE							cbSettleKind;						//���㷽ʽ
	BYTE							cbVideoMode;						//��Ƶģʽ
	BYTE							cbCurrencyKind;						//��������
	BYTE							cbForbidSameIP;						//��ֹͬIP	
	WORD							wPlayCount;							//��Ϸ����
	DWORD							dwPlayTime;							//��Ϸʱ��
	LONGLONG						lBaseScore;							//��Ϸ�׷�

	//�ͻ���Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ɢ����
struct CMD_GA_DismissTable
{
	DWORD							dwGroupID;							//Ⱥ���ʶ

	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwPersonalRoomID;					//���ӱ��
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����	
};

//��ȡ�б�
struct CMD_GA_GetTableUserList
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwPersonalRoomID;					//�����
};

//�����б�
struct CMD_GA_TableList
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	WORD							wTableCount;						//������Ŀ
};

//����ʧ��
struct CMD_GA_OperateFailed
{
	WORD							wErrorCode;							//ʧ�ܱ���
	TCHAR							szErrorMessage[128];				//������Ϣ
};

//�û�����
struct CMD_GA_UserTableStatus
{
	DWORD                           dwKindID;							//����ID
	DWORD							dwServerID;							//����I D
	WORD							wTableID;							//����I D
	DWORD							dwPersonalRoomID;					//�����
	DWORD							dwGroupID;							//����ȦID
	DWORD							dwUserID;							//�û���ʶ
	WORD							wChairID;							//�û�����
	bool							bSit;								//��������
};

//���Ӳ���
struct CMD_GA_TableParam
{
	DWORD							dwMappedNum;						//ӳ����
	DWORD							dwStartTime;						//��ʼʱ��
	WORD							wFinishCount;						//��ɾ���
};

//���ӽ�ɢ
struct CMD_GA_TableDismiss
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwMappedNum;						//ӳ����
};

//��ɢ���
struct CMD_GA_DismissResult
{
	BYTE							cbResultCode;						//�������
	TCHAR							szDescribeString[256];				//������Ϣ
};

//�û��б�
struct CMD_GA_TableUserList
{
	WORD							wUserCount;							//�û�����
	DWORD							dwUserID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];	//�û�I D
};

//���ӿ�ʼ
struct CMD_GA_TableStart
{
	DWORD                           dwKindID;							//����ID
	DWORD							dwServerID;							//����I D
	WORD							wTableID;							//����I D
	DWORD							dwGroupID;							//����ȦID
	DWORD							dwPersonalRoomID;					//�����
	bool							bStart;								//�Ƿ�ʼ
};

//�л�ׯ��
struct CMD_GA_TableChangeBanker
{
	DWORD                           dwKindID;							//����ID
	DWORD							dwServerID;							//����I D
	WORD							wTableID;							//����I D
	DWORD							dwGroupID;							//����ȦID
	DWORD							dwPersonalRoomID;					//�����
	DWORD							dwBankerUserID;						//������Ϸׯ��USERID(��ЧΪ0)
};

//////////////////////////////////////////////////////////////////////////////////

//�߼�����
#define MDM_GA_LOGIC_SERVICE		2									//�߼�����

//��������
#define SUB_GA_LOGON_SERVER			1									//��¼����
#define SUB_GA_SEARCH_GROUP			2									//����Ⱥ��
#define SUB_GA_CREATE_GROUP			3									//����Ⱥ��
#define SUB_GA_UPDATE_GROUP			4									//����Ⱥ��
#define SUB_GA_DELETE_GROUP			5									//ɾ��Ⱥ��
#define SUB_GA_DELETE_MEMBER		7									//ɾ����Ա		
#define SUB_GA_APPLY_REQUEST		8									//��������
#define SUB_GA_APPLY_RESPOND		9									//������Ӧ
#define SUB_GA_APPLY_DELETE			10									//����ɾ��
#define SUB_GA_APPLY_REPEAL			11									//���볷��
#define SUB_GA_SETTLE_BATTLE		12									//Լս����
#define SUB_GA_QUERY_BATTLE			13									//��ѯԼս
#define SUB_GA_QUERY_TOTAL_BATTLE	14									//Լս����	

#define SUB_GA_PRESENT_SCORE		15									//����ѫ��
#define SUB_GA_QUERY_PRESENT		16									//��ѯ����
#define SUB_GA_QUERY_REVENUE		17									//��ѯ�۷�
#define SUB_GA_APPEND_CONFIG		18									//����淨
#define SUB_GA_MODIFY_CONFIG		19									//�޸��淨
#define SUB_GA_DELETE_CONFIG		20									//ɾ���淨
#define SUB_GA_MODIFY_RIGHT			21									//�޸�Ȩ��
#define SUB_GA_QUERY_CREATE			22									//��ѯ����
#define SUB_GA_QUERY_MEDALMODE		23									//��ѯѫ��ģʽ
#define SUB_GA_QUERY_DAYRECORD		24									//��ѯ��ͳ��
#define SUB_GA_SET_ROLLMSG			25									//���������
#define SUB_GA_EDIT_NOTICE			26									//�༭����
#define SUB_GA_SET_GROUP_STATUS		27									//��������Ȧ״̬
#define SUB_GA_SET_INOUT			28									//��������
#define SUB_GA_QUERY_RIGHT			29									//��ѯȨ��
#define SUB_GA_RIGHT_MANAGEMENT		30									//Ȩ�޹���
#define SUB_GA_SET_MEDAL			31									//ѫ��ģʽ����
#define SUB_GA_SET_FREE_TABLE		32									//��������
#define SUB_GA_SET_BIGWINNER		33									//��Ӯ����ʾ����
#define SUB_GA_QUERY_USER_DETAIL	34									//��ѯ���һ������
#define SUB_GA_QUERY_GROUP_DETAIL	35									//��ѯ����Ȧһ������
#define SUB_GA_SET_FLAG				36									//���ý�����
#define SUB_GA_DISMISS_CLUB_TABLE	37									//��ɢ����Ȧ����
#define SUB_GA_SET_FAVORITE_CONFIG	38									//�����ղ��淨
#define SUB_GA_MODIFY_GROUP_NAME	39									//�޸�����Ȧ����
#define SUB_GA_QUERY_ROOM_DETAIL	40									//��ѯ����Ծ�����
#define SUB_GA_QUERY_CAPTAIN_LIST	41									//��ѯ�ӳ��б�
#define SUB_GA_ADD_CAPTAIN_TEAMER	42									//�����Ա
#define SUB_GA_QUERY_TEAM_RECORD	43									//��ѯС����ͳ��
#define SUB_GA_QUERY_TEAMER_RECORD	44									//��ѯС�ӳ�Ա��ͳ��
#define SUB_GA_SET_PRIVACY			45									//��˽����
#define SUB_GA_SET_CAPTAIN_POINT	46									//���öӳ���λ
#define SUB_GA_OPEN_BOX				47									//�򿪻��ֱ�����
#define SUB_GA_GET_AWARDSCORE		48									//��ȡ����
#define SUB_GA_QUERY_INVITATION_CODE 49									//��ѯ������
#define SUB_GA_BLIND_INVITATION_CODE 50									//���ƹ���
#define SUB_GA_QUERY_SCORE			 51									//Ȧ��/����Ա/�ӳ���ѯ���÷���
#define SUB_GA_QUERY_ROOM_BATTLE_INFO	52								//��ѯ���ӶԾ���Ϣ
#define SUB_GA_MODIFY_INFO			53									//�޸���Ϣ
//Ӧ�����
#define SUB_GA_GROUP_OPTION			100									//Ⱥ������
#define SUB_GA_APPLY_MESSAGE		101									//������Ϣ
#define SUB_GA_LOGON_FAILURE		102									//��¼ʧ��
#define SUB_GA_SEARCH_RESULT		103									//�������
#define SUB_GA_WEALTH_UPDATE		104									//�Ƹ�����
#define SUB_GA_APPLY_DEL_RESULT		105									//ɾ�����
#define SUB_GA_QUERY_BATTLE_RESULT	106									//Լս���
#define SUB_GA_SETTLE_RESULT		107									//������
#define SUB_GA_TOTAL_RESULT			108									//������

#define SUB_GR_PRESENT_SUCCESS		109									//���ͳɹ�
#define SUB_GA_QUERY_PRESENT_LIST	110									//�����б�
#define SUB_GA_QUERY_REVENUE_LIST	111									//�۷��б�
#define SUB_GA_QUERY_CREATE_RESULT	112									//��ѯ�������
#define SUB_GA_QUERY_DAYRECORD_LIST	113									//�ռ�¼�б�
#define SUB_GA_USER_DAY_DETAIL_LIST	116									//����������б�
#define SUB_GA_GROUP_DAY_DETAIL_LIST 117								//����Ȧ�������б�
#define SUB_GA_FAVORITE_CONFIG		118									//�ղ��淨
#define SUB_GA_ROOM_DETAIL_INFO		119									//����Ȧ��������
#define SUB_GA_CAPTAIN_LIST			120									//�ӳ��б�
#define SUB_GA_TEAM_DAY_RECORD		121									//С����ͳ��
#define SUB_GA_TEAMER_DAY_RECORD	122									//С�ӳ�Ա��ͳ��
#define SUB_GA_CAPTAIN_POINT		123									//�ӳ���λ
#define SUB_GA_BOX_INFO				124									//��������Ϣ
#define SUB_GA_GETBOXSCORE_RESULT	125									//��ȡ���
#define SUB_GA_INVITATION_CODE		126									//��������Ϣ
#define SUB_GA_BLIND_CODE_RES		127									//�󶨳ɹ�
#define SUB_GA_QUERY_SCORE_RES		128									//��ѯ�������
#define SUB_GA_QUERY_TABLE_RES		129									//��ѯ�Ծ���Ϣ���
//�������
#define SUB_GA_OPERATE_SUCCESS		200									//�����ɹ�
#define SUB_GA_OPERATE_FAILURE		201									//����ʧ��	

//ϵͳ��Ϣ
#define SUB_GA_SYSTEM_MESSAGE		300									//ϵͳ��Ϣ

//////////////////////////////////////////////////////////////////////////////////
//��¼����
struct SUB_GA_LogonServer
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//����Ⱥ��
struct SUB_GA_SearchGroup
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//����Ⱥ��
struct SUB_GA_CreateGroup
{
	//������Ϣ
	BYTE							cbPayType;							//֧������	
	BYTE							cbCurrencyKind;						//��������	
	BYTE							cbBattleMask;						//Լս����

	//�û���Ϣ
	DWORD							dwCreateID;							//Ⱥ����ʶ
	DWORD							dwStationID;						//վ���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//Ⱥ����Ϣ
	TCHAR							szGroupName[LEN_GROUP_NAME];		//Ⱥ������
	TCHAR							szGroupIntroduce[LEN_GROUP_INTRODUCE];//Ⱥ�����
};

//����Ⱥ��
struct SUB_GA_UpdateGroup
{
	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwStationID;						//վ���ʶ
	BYTE							cbUpdateKind;						//��������

	//�û���Ϣ
	DWORD							dwCreateID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//ɾ��Ⱥ��
struct SUB_GA_DeleteGroup
{
	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwStationID;						//վ���ʶ

	//�û���Ϣ
	DWORD							dwCreateID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//����Ⱥ��״̬
struct SUB_GA_SetGroupStatus
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwCreateID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	BYTE							cbStatus;							//״ֵ̬1��Ч��2ɾ����3����
};

//ɾ����Ա
struct SUB_GA_DeleteMember
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����	
	INT								nCount;								//ɾ���������DWORD
};

//��������
struct SUB_GA_ApplyRequest
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwApplyerID;						//�û���ʶ		
	BYTE							cbApplyType;						//��������0���룬1�˳�
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	LONGLONG						lInvitationCode;					//�����룬��������Ϊ0ʱ��Ч
};

//������Ӧ
struct SUB_GA_ApplyRespond
{
	DWORD							dwMessageID;						//��Ϣ��ʶ

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwApplyerID;						//�û���ʶ	
	BYTE							cbApplyStatus;						//����״̬
};

//����ɾ��
struct SUB_GA_ApplyDelete
{
	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageType;						//��Ϣ����

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//���볷��
struct SUB_GA_ApplyRepeal
{
	DWORD							dwMessageID;						//��Ϣ��ʶ	

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//Լս����
struct SUB_GA_SettleBattle
{	
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	DWORD							dwSettleCount;						//�������

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//��ѯԼս
struct SUB_GA_QueryBattle
{
	DWORD							dwGroupID;							//Ⱥ���ʶ

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//����淨
struct tagConfig
{
	DWORD							dwConfigID;								//�淨ID
	DWORD							dwCreateID;								//������ID
	DWORD							dwGroupID;								//����Ȧid
	WORD							wKindID;								//����ID
	LONGLONG						lCellScore;							    //�׷�����
	DWORD							dwDrawCountLimit;						//��������
	DWORD							dwDrawTimeLimit;						//ʱ������
	WORD							wJoinGamePeopleCount;					//������Ϸ������
	DWORD							dwRoomTax;								//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	BYTE							cbPassword;								//�������� 0:������ 1:��Ҫ����
	BYTE							cbPayMode;								//֧����ʽ									
	BYTE							cbGameRule[RULE_LEN];					//��Ϸ���� ��0λ��ʶ �Ƿ����ù��� 0 ����δ���� 1 ��������
	BYTE							cbGameMode;								//��Ϸģʽ
	LONGLONG						lEnterScore;							//���뷿��ķ�������
	LONGLONG						lLeaveScore;							//�뿪�����������
	BYTE							cbJoinMode;								//��Ҳ��뷽ʽ	0 ������İ���˲���	1 ����İ���˲���	
};

//����淨
struct SUB_GA_AppendConfig
{
	DWORD							dwCreateID;								//������ID
	DWORD							dwGroupID;								//����Ȧid
	TCHAR							szConfigName[13];						//�淨����
	WORD							wKindID;								//����ID
	BYTE							cbPlayMode;								//����ģʽ0���֣�1ѫ��
	LONGLONG						lCellScore;							    //�׷�����
	DWORD							dwDrawCountLimit;						//��������
	DWORD							dwDrawTimeLimit;						//ʱ������
	WORD							wJoinGamePeopleCount;					//������Ϸ������
	DWORD							dwRoomTax;
	BYTE							cbPassword;								//�������� 0:������ 1:��Ҫ����
	BYTE							cbPayMode;								//֧����ʽ									
	BYTE							cbGameRule[RULE_LEN];					//��Ϸ���� ��0λ��ʶ �Ƿ����ù��� 0 ����δ���� 1 ��������
	BYTE							cbGameMode;								//��Ϸģʽ
	LONGLONG						lEnterScore;							//���뷿��ķ�������
	LONGLONG						lLeaveScore;							//�뿪�����������
	BYTE							cbJoinMode;
	BYTE							cbTaxMode;								//˰��ģʽ��1�̶�ģʽ������ң�2�̶�ģʽ��Ӯ�ң�3����ģʽ������ң�4����ģʽ��Ӯ��
	DWORD							dwTaxCount;								//˰����ֵ
	BYTE							cbLessScoreDismiss;						//����������ɢ
};

//�޸��淨
struct SUB_GA_ModifyConfig
{
	DWORD							dwUserID;								//�û���ʶ
	DWORD							dwGroupID;								//Ⱥ��ID
	DWORD							dwConfigID;								//�淨ID
	TCHAR							szConfigName[13];						//�淨����
	BYTE							cbPlayMode;								//����ģʽ0���֣�1ѫ��
	LONGLONG						lCellScore;							    //�׷�����
	DWORD							dwDrawCountLimit;						//��������
	DWORD							dwDrawTimeLimit;						//ʱ������
	WORD							wJoinGamePeopleCount;					//������Ϸ������
	BYTE							cbPassword;								//�������� 0:������ 1:��Ҫ����
	BYTE							cbPayMode;								//֧����ʽ									
	BYTE							cbGameRule[RULE_LEN];					//��Ϸ���� ��0λ��ʶ �Ƿ����ù��� 0 ����δ���� 1 ��������
	BYTE							cbGameMode;								//��Ϸģʽ
	LONGLONG						lEnterScore;							//���뷿��ķ�������
	LONGLONG						lLeaveScore;							//�뿪�����������
	BYTE							cbTaxMode;								//����ģʽ��1�̶�ģʽ������ң�2�̶�ģʽ��Ӯ�ң�3����ģʽ������ң�4����ģʽ��Ӯ��
	DWORD							dwTaxCount;								//���������ֵ
	BYTE							cbLessScoreDismiss;						//����������ɢ
};

//ɾ���淨
struct SUB_GA_DeleteConfig
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGroupID;							//Ⱥ��ID
	DWORD							dwConfigID;							//�淨ID
};

//�޸�Ȩ��
struct SUB_GA_ModifyRight
{
	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ

	//Ȩ����Ϣ
	INT								nRight;								//�޸�Ȩ��
	BYTE							cbMode;								//0����Ȩ�ޣ�1ɾ��Ȩ��
	//�û���Ϣ
	DWORD							dwCreateID;							//�����û���ʶ(ֻ��������)
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����	
};

//������Ϣ
struct SUB_GA_ApplyMessage
{
	BYTE							cbMessageType;						//��Ϣ����
	WORD							wMessageCount;						//��Ϣ����
};

//��¼ʧ��
struct SUB_GA_S_LogonFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//�������
struct SUB_GA_S_SearchResult
{
	WORD							wGroupCount;						//Ⱥ������
};

//�û��Ƹ�
struct SUB_GA_WealthUpdate
{
	LONGLONG						lUserIngot;							//�û���ʯ
	LONGLONG						lUserRoomCard;						//�û�����
};

//ɾ�����
struct SUB_GA_ApplyDelResult
{
	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageType;						//��Ϣ����
	bool							bDelResult;							//ɾ�����
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//����֪ͨ
struct SUB_GA_ApplyRepResult
{
	DWORD							dwMessageID;						//��Ϣ��ʶ	
	bool							bRepResult;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//�����ɹ�
struct SUB_GA_OperateSuccess
{
	LONG							lOperateCode;						//��������
	TCHAR							szOperateNotice[128];				//������ʾ
};

//����ʧ��
struct SUB_GA_OperateFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//ϵͳ��Ϣ
struct SUB_GA_SystemMessage
{
	WORD							wType;								//��Ϣ����		
	TCHAR							szString[128];						//��Ϣ����
};

//Լս���
struct SUB_GA_BattleResult
{
	DWORD							dwCreateCount;						//������Ŀ
	LONGLONG						lConsumeIngot;						//������Ŀ
	DWORD							dwRecordCount;						//��¼��Ŀ
};
//����ѫ��
struct SUB_GA_PresentScore
{
	DWORD							dwGroupID;							//����ȦID
	DWORD							dwPresentUserID;					//�������ID
	LONGLONG						lPresentScore;						//������Ŀ
	BYTE							cbType;								//0���ͣ�1����
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	INT								nCount;								//��������
};

//��ѯ���ͼ�¼
struct SUB_GA_QueryPresent
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwQueryUserID;						//�û���ʶ
};

//���󴴽�
struct SUB_GA_QueryCreate
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwConfigID;							//�淨��ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//��ѯ����ͳ��
struct SUB_GA_QueryDayRecord
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwQueryUserID;						//�û���ʶ
	DWORD							dwDateID;							//����ID --20190715
};

//��ѯ����Ȧ������
struct SUB_GA_QueryGroupDetail
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwQueryUserID;						//��ѯ���
	DWORD							dwDateID;							//����ID --20190715
};

//��ѯ���������
struct SUB_GA_QueryUserDayDetail
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwQueryUserID;						//��ѯ���
	DWORD							dwTargetUserID;						//Ŀ�����
	DWORD							dwDateID;							//����ID --20190715
};

//��ѯѫ��ģʽ
struct SUB_GA_QueryMedalMode
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//���������
struct SUB_GA_SetRollMsg
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbFlag;								//0�رգ�1����
	TCHAR							szRollMsg[33];						//��Ϣ�������32����
};

//����Ȧ����
struct SUB_GA_SetNotice
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNotice[109];						//��Ϣ�������108����
};

//��������
struct SUB_GA_SetInOut
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbInMode;							//��������0������ˣ�1�����
	BYTE							cbInType;							//���뷽ʽ0��������루����ID����1����ȦID����
	BYTE							cbOutMode;							//�˳�����0������ˣ�1�����
};

//Ȩ�޹���
struct SUB_GA_RightManagement
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwTargetID;							//Ŀ���û�ID
	BYTE							cbManagerRight[30];					//����ԱȨ��
	BYTE							cbCaptainRight[30];					//�ӳ�Ȩ��
};

//Ȩ�޲�ѯ
struct SUB_GA_QueryRight
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//ѫ������
struct SUB_GA_SetMedal
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbMedalMode;						//0�رգ�1����
	BYTE							cbMedalShow;						//0����ʾ��1��ʾ
	BYTE							cbCurMedalShow;						//0�����˿ɼ���1����Ա�ɼ�
	BYTE							cbMedalRevenueShow;					//0�����˿ɼ���1����Ա�ɼ�
	BYTE							cbMedalClear;						//0�����㣬1�������
};

//��������
struct SUB_GA_SetFreeTable
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbFreeTableLimit;					//�����������
};

//��Ӯ������
struct SUB_GA_SetBigWinner
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbShow;								//0����ʾ��1��ʾ
};

//���õ��ޱ��
struct SUB_GA_SetFlag
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwOperateUserID;					//�������
	DWORD							dwTargetUserID;						//Ŀ�����
	DWORD							dwDateID;							//����ID --20190715
	BYTE							cbFlag;								//��ǣ�0ȡ����1��Ч
};

//�����ɢ����
struct SUB_GA_ApplyDismissClubTable
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�������
	DWORD							dwRoomID;							//����ID
};

//�����ղ��淨
struct SUB_GA_SetFavoriteConfig
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//�������
	DWORD							dwConfigID;							//�ղ��淨
};

//�޸�����
struct SUB_GA_ModifyGroupName
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�������
	TCHAR							szGroupName[LEN_GROUP_NAME];		//Ⱥ������
};

//��ѯ����Ծ�����
struct SUB_GA_QueryRoomDetail
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwRoomID;							//����ID
	DWORD							dwDateID;							//���ڱ�ʶ
	DWORD							dwUserID;							//��ѯ���
	SYSTEMTIME						EndTime;							//�������ʱ��
};

//��ѯ�ӳ��б�
struct SUB_GA_QueryCaptain
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//��ѯ���
	DWORD							dwCaptainID;						//�ӳ�ID,Ϊ0ʱ����ѯ����Ȧֱ���ӳ�����Ϊ0�����ѯ�öӳ��������ӳ�
	DWORD							dwDateID;							//����ID
};

//��Ӷ�Ա
struct SUB_GA_AddTeamer
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//����ID
	DWORD							dwCaptainID;						//�ӳ�ID
	//DWORD							dwMemberID;							//��ԱID
	BYTE							cbMode;								//0�Ƴ���1���
	INT								nCount;								//��������ӳ�ԱID
};

//��ѯС�ӵ���ͳ��
struct SUB_GA_QueryTeamDayRecord
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwQueryUserID;						//�û���ʶ
	DWORD							dwDateID;							//����ID --20190715
};

//��ѯС�ӳ�Ա����ͳ��
struct SUB_GA_QueryTeamerDayRecord
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwQueryUserID;						//�û���ʶ
	DWORD							dwCaptainID;						//�ӳ�ID
	DWORD							dwDateID;							//����ID --20190715
};

//��˽����
struct SUB_GA_SetPrivacy
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbShowGroupID;						//0����ID��1��ʾID
};

//���öӳ���λ
struct SUB_GA_SetCaptainPoint
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwCaptainID;						//�ӳ�ID
	WORD							wPoint;								//��λ
};

//�򿪻��ֱ�����
struct SUB_GA_OpenBox
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//��ȡ����
struct SUB_GA_GetAwardScore
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lScore;								//��ȡ����
};

//��ѯ������
struct SUB_GA_QueryInvatationCode
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//��������
struct SUB_GA_BlindInvatationCode
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	LONGLONG						lInvitationCode;					//�ƹ���
};

//��ѯ���÷�
struct SUB_GA_QueryScore
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//��ѯ������Ϣ
struct SUB_GA_QueryTableBattleInfo
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwServerID;							//������ID
	DWORD							dwTableID;							//����ID
};

//��ѯ������Ϣ���
struct SUB_GA_QueryTableBattleInfoResult
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwRoomID;							//����ID
	DWORD							dwAllCount;							//�ܾ���
	DWORD							dwPlayCount;						//��ǰ����
	DWORD							dwUserID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//���ID
	SCORE							lGameScore[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//��Ϸ�÷�
};

//�޸���Ϣ
struct SUB_GA_Modify_Info
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbFlag[3];							//�޸ı�ǩ��1��Ч
	TCHAR							szQQ[LEN_NICKNAME];					//�û�QQ
	TCHAR							szWX[LEN_NICKNAME];					//�û�WX
	TCHAR							szPhone[12];						//�û��ֻ���
};
//////////////////////////////////////////////////////////////////////////////////

//Ⱥ�����
#define MDM_GA_GROUP_SERVICE		3									//Ⱥ�����

//��������
#define SUB_GA_ENTER_GROUP			1									//����Ⱥ��
#define SUB_GA_LEAVE_GROUP			2									//�뿪Ⱥ��

//Ⱥ������
#define SUB_GA_GROUP_ITEM			100									//Ⱥ����Ϣ
#define SUB_GA_GROUP_PROPERTY		101									//Ⱥ������
#define SUB_GA_GROUP_MEMBER			102									//Ⱥ���Ա
#define SUB_GA_GROUP_UPDATE			103									//Ⱥ�����
#define SUB_GA_GROUP_DELETE			104									//Ⱥ���Ƴ�
#define SUB_GA_MEMBER_INSERT		105									//��ӳ�Ա
#define SUB_GA_MEMBER_DELETE		106									//ɾ����Ա
#define SUB_GA_MEMBER_UPDATE		107									//���³�Ա
#define SUB_GA_LOGON_PROPERTY		108									//Ⱥ������
#define SUB_GA_LOGON_SUCCESS		109									//��¼�ɹ�
#define SUB_GA_QUERY_PROPERTY		110									//Ⱥ���ѯ
#define SUB_GA_QUERY_PROPERTY_END	111									//Ⱥ���ѯ����
#define SUB_AG_GROUP_EXTRA_INFO		112									//Ⱥ�鸽����Ϣ
//�淨
#define SUB_GA_GROUP_CONFIG			120									//Ⱥ���淨
#define SUB_GA_CONFIG_APPEND		121									//�淨���
#define SUB_GA_CONFIG_MODIFY		122									//�淨�޸�
#define SUB_GA_CONFIG_DELETE		123									//�淨ɾ��
#define SUB_GA_RIGHT_CHANGE			124									//Ȩ�޸ı�
#define SUB_GA_ROLL_MSG				125									//�����
#define SUB_GA_GROUP_NOTICE			126									//����
#define SUB_GA_GROUP_STATUS			127									//Ⱥ��״̬
#define SUB_GA_INOUT_MODE			128									//��������
#define SUB_GA_FREE_TABLE_LIMIT		129									//��������
#define SUB_GA_BIGWINNER_SHOW_MODE	130									//��Ӯ������
#define SUB_GA_MANAGER_RIGHT		131									//����ԱȨ������
#define SUB_GA_MEDAL_MODE			132									//ѫ��ģʽ
#define SUB_GA_MEMBER_STATUS		133									//��Ա״̬
#define SUB_GA_SET_FLAG_RESULT		134									//���õ��ޱ��
#define SUB_GA_SET_CLEAR_MEDAL		135									//ѫ������
#define SUB_GA_GROUP_NAME			136									//Ⱥ������
#define SUB_GA_ADD_TEAMER			137									//�����Ա
#define SUB_GA_GROUP_PRIVACY		138									//���ֲ���˽
#define SUB_GA_GROUP_COMMON_INFO	139									//��������
//������
#define SUB_GA_ENTER_SUCCESS		200									//����ɹ�
#define SUB_GA_ENTER_FAILURE		201									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////

//����Ⱥ��
struct SUB_GA_EnterGroup
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//�뿪Ⱥ��
struct SUB_GA_LeaveGroup
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGroupID;							//Ⱥ���ʶ
};
//��ѯȺ��
struct SUB_GA_QueryGroup
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wGroupCount;						//Ⱥ������
};
//Ⱥ������
struct SUB_GA_GroupItem
{
	tagIMGroupItem					GroupItem;							//Ⱥ������
};
//Ⱥ������
struct SUB_GA_GroupProperty
{
	tagIMGroupProperty				GroupProperty;						//Ⱥ������
};

//Ⱥ���Ա
struct SUB_GA_GroupMember
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//Ⱥ�����
struct SUB_GA_GroupUpdate
{	
	DWORD							dwGroupID;							//Ⱥ���ʶ
	BYTE							cbUpdateKind;						//��������
};

//Ⱥ��ɾ��
struct SUB_GA_GroupDelete
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//��ӳ�Ա
struct SUB_GA_MemberInsert
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	WORD							wMemberCount;						//��Ա����
	tagIMGroupMemberUser			MemberUser;							//��Ա�û�	
};

//ɾ����Ա
struct SUB_GA_MemberDelete
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	WORD							wMemberCount;						//��Ա����
};

//���³�Ա
struct SUB_GA_MemberUpdate
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	tagIMGroupMember				GroupMember;						//��Ա��Ϣ
};

//Ⱥ���淨
struct SUB_GA_GroupConfig
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	INT								nCount;								//�淨����
};

//�淨���
struct SUB_GA_ConfigAppend
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	tagGroupConfigInfo				ConfigInfo;							//�淨��Ϣ
};

//�淨�޸�
struct SUB_GA_ConfigModify
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	tagGroupConfigInfo				ConfigInfo;							//�淨��Ϣ
};

//�淨ɾ��
struct SUB_GA_ConfigDelete
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwConfigID;							//�淨��ʶ
};

//Ȩ�޸ı�
struct SUB_GA_RightChange
{
	DWORD							dwGroupID;							//���ͱ�ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	INT								nMemberRight;						//��ԱȨ��
};

//����ʧ��
struct SUB_GA_EnterFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//��ѯ�������
struct SUB_GA_QueryCreateResult
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwConfigID;							//�淨��ʶ
	INT								nMaxNullTableCount;					//����������
	bool							bCanCreate;							//���
	DWORD							dwRoomID;							//������ܴ������򷵻�һ��RoomID
};

//�����
struct SUB_GA_RollMsg
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	BYTE							cbFlag;								//0������1�ر�
	INT								nTime;								//���ż����λ��
	TCHAR							szRollMsg[33];						//����
	BYTE							cbMsgType;							//1Ȧ����Ϣ��2�ӳ���Ϣ,3�Լ�Ϊ�ӳ�ʱ����Ϣ
};

//����
struct SUB_GA_GroupNotice
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	TCHAR							szNotice[109];						//����
};

//����ԱȨ��
struct SUB_GA_ManagerRight
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwTargetID;							//Ŀ�����ID
	LONGLONG						lManagerRight;						//����ԱȨ��
	LONGLONG						lCaptainRight;						//�ӳ�Ȩ��
	//BYTE							cbManagerRight[30];					//����ԱȨ��
	//BYTE							cbCaptainRight[30];					//�ӳ�Ȩ��
};

//��Ա״̬
struct SUB_GA_MemberStatus
{
	DWORD							dwUserID;							//��ұ�ʶ
	BYTE							cbStatus;							//0���ߣ�1����

};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif