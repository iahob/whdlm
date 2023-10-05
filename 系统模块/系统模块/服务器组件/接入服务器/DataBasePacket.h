#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//�������ݰ�

//ϵͳ����
#define DBR_GR_LOAD_GROUP_OPTION	1									//��������
#define DBR_GR_LOAD_FORBID_WORDS	2									//��ֹ�ʻ�

//Ⱥ������
#define DBR_GR_LOGON_SERVER			10									//�û���¼
#define DBR_GR_CREATE_GROUP			11									//����Ⱥ��
#define DBR_GR_UPDATE_GROUP			12									//����Ⱥ��
#define DBR_GR_DELETE_GROUP			13									//ɾ��Ⱥ��
#define DBR_GR_INSERT_MEMBER		14									//��ӳ�Ա
#define DBR_GR_DELETE_MEMBER		15									//ɾ����Ա
#define DBR_GR_SETTLE_BATTLE		16									//Լս����
#define DBR_GR_QUERY_BATTLE			17									//��ѯԼս
#define DBR_GR_QUERY_TOTAL_BATTLE	18									//Լս����
#define DBR_GR_APPEND_CONFIG		19									//�����淨
#define DBR_GR_MODIFY_CONFIG		20									//�޸��淨
#define DBR_GR_DELETE_CONFIG		21									//ɾ���淨
#define DBR_GR_MODIFY_RIGHT			22									//�޸�Ȩ��
#define DBR_GR_PRESENT_SCORE		23									//����ѫ��
#define DBR_GR_QUERY_PRESENT		24									//���ͼ�¼
#define DBR_GR_QUERY_REVENUE		25									//�۷Ѽ�¼
#define DBR_GR_QUERY_DAYRECORD		26									//�ռ�¼
#define DBR_GR_QUERY_CREATE			27									//��ѯ��������
#define DBR_GR_QUERY_MEDAL_MODE		28									//��ѯѫ��ģʽ
#define DBR_GR_SET_GROUP_STATUS		29									//���ô�����״̬
//��������
#define DBR_GR_APPLY_REQUEST		30									//��������
#define DBR_GR_APPLY_RESPOND		31									//������Ӧ
#define DBR_GR_APPLY_DELETE			32									//����ɾ��

#define DBR_GR_SET_ROLLMSG			33									//���������
#define DBR_GR_SET_NOTICE			34									//���ù���
#define DBR_GR_SET_INOUT			35									//��������
#define DBR_GR_RIGHT_MANAGEMENT		36									//Ȩ�޹���
#define DBR_GR_RIGHT_QUERY			37									//Ȩ�޲�ѯ
#define DBR_GR_SET_MEDAL			38									//ѫ������
#define DBR_GR_SET_FREETABLE_LIMIT	39									//��������
#define DBR_GR_SET_BIGWINNER_SHOW	40									//��Ӯ����ʾ
#define DBR_GR_QUERY_USERDAY_DETAIL	41									//������������
#define DBR_GR_QUERY_GROUPDAY_DETAIL 42									//��Ҵ�����������
#define DBR_GR_SET_FLAG				43									//���õ��ޱ�־
#define DBR_GR_MEDAL_CLEAR			44									//ѫ������
#define DBR_GR_SET_FAVORITE_CONFIG	45									//�����ղ��淨
#define DBR_GR_MODIFY_GROUP_NAME	46									//�޸Ĵ���������
#define DBR_GR_QUERY_ROOM_DETAIL	47									//��ѯ����Ծ�����

/////////////////С�����
#define DBR_GR_QUERY_CAPTAIN_LIST	48									//��ѯ�ӳ�
#define DBR_GR_ADD_CAPTAIN_TEAMER	49									//�����Ա
#define DBR_GR_QUERY_TEAM_RECORD	50									//С����ͳ��
#define DBR_GR_QUERY_TEAMER_RECORD	51									//С�ӳ�Ա��ͳ��
#define DBR_GR_SET_PRIVACY			52									//��˽����
#define DBR_GR_SET_CAPTAIN_POINT	53									//�ӳ���λ����
#define DBR_GR_OPEN_BOX				54									//�򿪻��ֱ�����
#define DBR_GR_GET_AWARD_SCORE		55									//��ȡ����
#define DBR_GR_QUERY_CODE			56									//��ѯ������
#define DBR_GR_BLIND_CODE			57									//��������
#define DBR_GR_CHECK_NEW_USER		58									//��������
#define DBR_GR_QUERY_SCORE			59									//��ѯ���÷�

////////////////////////////
#define DBR_GR_UPDATE_ONLINE_STATUS			60							//���¾��ֲ�����״̬

#define DBR_GR_MODIFY_INFO			61									//�޸�����

//////////////////////////////////////////////////////////////////////////

//�û���¼
struct DBR_GR_LogonServer
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//����Ⱥ��
struct DBR_GR_CreateGroup
{
	//������Ϣ
	BYTE							cbPayType;							//֧������	
	BYTE							cbCurrencyKind;						//��������	

	//�û���Ϣ
	DWORD							dwCreateID;							//Ⱥ����ʶ	
	DWORD							dwStationID;						//վ���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//Ⱥ����Ϣ
	TCHAR							szGroupName[LEN_GROUP_NAME];		//Ⱥ������
	TCHAR							szGroupIntroduce[LEN_GROUP_INTRODUCE];//Ⱥ�����	
};

//����Ⱥ��
struct DBR_GR_UpdateGroup
{
	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwStationID;						//վ���ʶ
	BYTE							cbUpdateKind;						//��������

	//�û���Ϣ
	DWORD							dwCreateID;							//Ⱥ����ʶ	
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//ɾ��Ⱥ��
struct DBR_GR_DeleteGroup
{
	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwStationID;						//վ���ʶ

	//�û���Ϣ
	DWORD							dwCreateID;							//Ⱥ����ʶ	
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//����Ⱥ��״̬
struct DBR_GR_SetGroupStatus
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwCreateID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	BYTE							cbStatus;							//״ֵ̬1��Ч��2ɾ����3����
};

//��ӳ�Ա
struct DBR_GR_InsertMember
{
	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ	

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//ɾ����Ա
struct DBR_GR_DeleteMember
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	INT								nCount;
};

//��������
struct DBR_GR_ApplyRequest
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwApplyerID;						//�û���ʶ		
	BYTE							cbApplyType;						//��������
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	LONGLONG						lInvitationCode;					//�����룬��������Ϊ0ʱ��Ч
};

//������Ӧ
struct DBR_GR_ApplyRespond
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
struct DBR_GR_ApplyDelete
{
	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageType;						//��Ϣ����

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//Լս����
struct DBR_GR_SettleBattle
{		
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	DWORD							dwSettleCount;						//�������

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//��ѯԼս
struct DBR_GR_QueryBattle
{
	DWORD							dwGroupID;							//Ⱥ���ʶ

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//��ѯԼս
struct DBR_GR_QueryBattleResult
{
	DWORD							dwCreateCount;						//������Ŀ
	LONGLONG						lConsumeIngot;						//������Ŀ
	DWORD							dwRecordCount;						//��¼��Ŀ
};


//�����淨
struct DBR_GR_AppendConfig
{
	DWORD							dwCreateID;								//������ID
	DWORD							dwGroupID;								//������id
	tagGroupConfigInfo				configInfo;								//�淨��Ϣ
};
//�޸��淨
struct DBR_GR_ModifyConfig
{
	DWORD							dwUserID;								//�û���ʶ
	DWORD							dwGroupID;								//Ⱥ��ID
	tagGroupConfigInfo				configInfo;								//�淨��Ϣ

};
//ɾ���淨
struct DBR_GR_DeleteConfig
{
	DWORD							dwUserID;								//���ID
	DWORD							dwGroupID;								//Ⱥ��ID
	DWORD							dwConfigID;								//�淨ID
	bool							bGroupPersonalMaxChair;					//�����˰�����Ϸ
	bool							bAttemperEngineConclude;				//
};

//�޸�Ȩ��
struct DBR_GR_ModifyRight
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

//����ѫ��
struct DBR_GR_PresentScore
{
	DWORD							dwGroupID;							//������ID
	DWORD							dwPresentUserID;					//�������ID
	LONGLONG						lPresentScore;						//������Ŀ
	BYTE							cbType;								//0���ͣ�1����
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	INT								nCount;

};

//��ѯ���ͼ�¼
struct DBR_GR_QueryPresent
{
	DWORD							dwGroupID;							//������ID
	DWORD							dwQueryUserID;						//��ѯ���ID
};

//��ѯ�۷�
struct DBR_GR_QueryRevenue
{
	DWORD							dwGroupID;							//������ID
	DWORD							dwQueryUserID;						//��ѯ���ID
};

//��ѯ�ռ�¼
struct DBR_GR_QueryDayRecord
{
	DWORD							dwGroupID;							//������ID
	DWORD							dwQueryUserID;						//��ѯ���ID
	DWORD							dwDateID;							//����ID
};

//��ѯ���������
struct DBR_GR_QueryUserDayDetail
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwQueryUserID;						//��ѯ���
	DWORD							dwTargetUserID;						//Ŀ�����
	DWORD							dwDateID;							//����ID --20190715
};

//��ѯ������������
struct DBR_GR_QueryGroupDayDetail
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwQueryUserID;						//��ѯ���
	DWORD							dwDateID;							//����ID --20190715
};

//��ѯ��������
struct DBR_GR_QueryCreate
{
	DWORD							dwGroupID;							//������ID
	DWORD							dwConfigID;							//�淨ID
	DWORD							dwUserID;							//���ID
};

//��ѯѫ��ģʽ
struct DBR_GR_QueryMedalMode
{
	DWORD							dwGroupID;							//������ID
	DWORD							dwUserID;							//���ID
};

//�������Ϣ
struct DBR_GR_SetRollMsg
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbFlag;								//0�رգ�1����
	TCHAR							szRollMsg[33];						//��Ϣ�������32����
};

//������Ϣ
struct DBR_GR_SetNotice
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNotice[109];						//��Ϣ�������108����
};

//��������
struct DBR_GR_SetInOut
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbInMode;							//��������0������ˣ�1�����
	BYTE							cbInType;							//���뷽ʽ0��������루����ID����1������ID����
	BYTE							cbOutMode;							//�˳�����0������ˣ�1�����
};

//Ȩ�޹���
struct DBR_GR_RightManagement
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwTargetID;							//Ŀ���û�ID
	LONGLONG						lManagerRight;						//����ԱȨ��
	LONGLONG						lCaptainRight;						//�ӳ�Ȩ��
};

//Ȩ�޲�ѯ
struct DBR_GR_QueryRight
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//ѫ������
struct DBR_GR_SetMedal
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
struct DBR_GR_SetFreeTableLimit
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbCount;							//����
};

//��Ӯ������
struct DBR_GR_SetBigWinner
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbShow;								//0���пɼ���1����Ա�ɼ�
};

//���õ��ޱ��
struct DBR_GR_SetFlag
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwOperateUserID;					//�������
	DWORD							dwTargetUserID;						//Ŀ�����
	DWORD							dwDateID;							//����ID --20190715
	BYTE							cbFlag;								//��ǣ�0ȡ����1��Ч
};

//�����ղ��淨
struct DBR_GR_SetFavoriteConfig
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//�������
	DWORD							dwConfigID;							//�ղ��淨
};

//�޸Ĵ���������
struct DBR_GR_ModifyGroupName
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�������
	TCHAR							szGroupName[LEN_GROUP_NAME];		//Ⱥ������
};

//ѫ������
struct DBR_GR_ClearMedal
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwDateID;
};

//��ѯ��������
struct DBR_GR_QueryRoomDetail
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwRoomID;							//����ID
	DWORD							dwDateID;							//���ڱ�ʶ
	DWORD							dwUserID;							//��ѯ���
	SYSTEMTIME						EndTime;							//�������ʱ��
};

//��ѯ�ӳ��б�
struct DBR_GR_QueryCaptain
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//��ѯ���
	DWORD							dwCaptainID;						//�ӳ�ID,Ϊ0ʱ����ѯ������ֱ���ӳ�����Ϊ0�����ѯ�öӳ��������ӳ�
	DWORD							dwDateID;
};
//��Ӷ�Ա
struct DBR_GR_AddTeamer
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//����ID
	DWORD							dwCaptainID;						//�ӳ�ID
	//DWORD							dwMemberID;							//��ԱID
	BYTE							cbMode;								//0�Ƴ���1���
	INT								nCount;								//��������ӳ�ԱID
};

//��ѯС���ռ�¼
struct DBR_GR_QueryTeamDayRecord
{
	DWORD							dwGroupID;							//������ID
	DWORD							dwQueryUserID;						//��ѯ���ID
	DWORD							dwDateID;							//����ID
};

//��ѯС�ӳ�Ա�ռ�¼
struct DBR_GR_QueryTeamerDayRecord
{
	DWORD							dwGroupID;							//������ID
	DWORD							dwQueryUserID;						//��ѯ���ID
	DWORD							dwCaptainID;						//�ӳ�ID
	DWORD							dwDateID;							//����ID
};

//��˽����
struct DBR_GR_SetPrivacy
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbShowGroupID;						//0����ID��1��ʾID
};

//���öӳ���λ
struct DBR_GR_SetCaptainPoint
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwCaptainID;						//�ӳ�ID
	WORD							wPoint;								//��λ
};

//�򿪻��ֱ�����
struct DBR_GR_OpenBox
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//��ȡ����
struct DBR_GR_GetAwardScore
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lScore;								//��ȡ����
};

//��ѯ������
struct DBR_GR_QueryCode
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//��������
struct DBR_GR_BlindCode
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	LONGLONG						lCode;								//�ƹ���
};

//��ѯ���÷�
struct DBR_GR_QueryScore
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//��������
struct DBR_GR_CheckNewUser
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szClientIP[15];						//���ip
};

//��������״̬
struct DBR_GR_UpdateOnLineStatus
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	byte							cbStatus;							//0 �뿪 1 ����
};

//�޸���Ϣ
struct DBR_GR_Modify_Info
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbFlag[3];							//�޸ı�ǩ��1��Ч
	TCHAR							szQQ[LEN_NICKNAME];					//�û�QQ
	TCHAR							szWX[LEN_NICKNAME];					//�û�WX
	TCHAR							szPhone[12];						//�û��ֻ���
};
//////////////////////////////////////////////////////////////////////////

//�����Ϣ

//ϵͳ����
#define DBO_GR_GROUP_OPTION			100									//Ⱥ������
#define DBO_GR_FIRBID_WORDS			101									//��ֹ�ʻ�

//Ⱥ������
#define DBO_GR_LOGON_SUCCESS		110									//��¼�ɹ�
#define DBO_GR_LOGON_FAILURE		111									//��¼ʧ��
#define DBO_GR_CREATE_GROUP			112									//����Ⱥ��
#define DBO_GR_UPDATE_GROUP			113									//����Ⱥ��
#define DBO_GR_DELETE_GROUP			114									//ɾ��Ⱥ��
#define DBO_GR_INSERT_MEMBER		115									//��ӳ�Ա
#define DBO_GR_DELETE_MEMBER		116									//ɾ����Ա
#define DBO_GR_BATTLE_RESULT		117									//��ѯԼս
#define DBO_GR_SETTLE_RESULT		118									//������
#define DBO_GR_TOTAL_RESULT			119									//�������
//��������
#define DBO_GR_WEALTH_UPDATE		120									//�Ƹ�����
#define DBO_GR_APPLY_MESSAGE		121									//������Ϣ
#define DBO_GR_APPLY_DEL_RESULT		122									//ɾ�����
#define DBO_GR_APPLY_MESSAGE_EX		123									//������Ϣ

#define DBO_GR_MODIFY_RIGHT			130									//�޸�Ȩ��	
#define DBO_GR_PRESENT_SUCCESS		131									//���ͽ��
#define DBO_GR_QUERY_PRESENT_LIST	132									//���ͼ�¼�б�
#define DBO_GR_QUERY_REVENUE_LIST	133									//�۷Ѽ�¼�б�
#define DBO_GR_APPEND_CONFIG		134									//�����淨
#define DBO_GR_MODIFY_CONFIG		135									//�޸��淨
#define DBO_GR_DELETE_CONFIG		136									//ɾ���淨
#define DBO_GR_DAY_RECORD_LIST		137									//�ռ�¼
#define DBO_GR_QUERY_CREATE_RESULT	138									//������ѯ
#define DBO_GR_MEDAL_MODE			139									//ѫ��ģʽ
#define DBO_GR_GROUP_STATUS			140									//Ⱥ��״̬
#define DBO_GR_INOUT_MODE			141									//��������
#define DBO_GR_MANAGER_RIGHT		142									//����ԱȨ��
#define DBO_GR_FREE_TABLE_LIMIT		143									//��������
#define DBO_GR_BIGWINNER_SHOW		144									//��Ӯ����ʾ����
#define DBO_GR_SET_MANAGER_RIGHT	145									//���ù���ԱȨ��
#define DBO_GR_USER_DAY_DETAIL_LIST	146									//���������
#define DBO_GR_GROUP_DAY_DETAIL_LIST 147								//������������
#define DBO_GR_SET_FLAG				148									//��������
#define DBO_GR_CLEAR_MEDAL			149									//����ѫ��
#define DBO_GR_FAVORITE_CONFIG		150									//�ղ��淨
#define DBO_GR_ROLL_MSG				151									//�����
#define DBO_GR_GROUP_NAME			152									//����������
#define DBO_GR_ROOM_DETAIL			153									//��������
#define DBO_GR_CAPTAIN_LIST			154									//�ӳ��б�
#define DBO_GR_ADD_TEAMER			155									//�����Ա���
#define DBO_GR_TEAM_DAY_RECORD_LIST	156									//С���ռ�¼
#define DBO_GR_TEAMER_DAY_LIST		157									//С�ӳ�Ա�ռ�¼
#define DBO_GR_GROUP_PRIVACY		158									//��˽����
#define DBO_GR_CAPTAIN_POINT		159									//��λ���óɹ�
#define DBO_GR_BOX_INFO				160									//��������Ϣ
#define DBO_GR_GET_AWARDSCORE_RES	161									//��������ȡ���
#define DBO_GR_INVITATION_CODE		162									//��������Ϣ
#define DBO_GR_BLIND_CODE_RES		163									//�󶨽��
#define DBO_GR_QUERY_SCORE_RES		164									//��ѯ�������
#define DBO_GR_MODIFY_INFO			165									//�޸���Ϣ
//����ʧ��
#define DBO_GR_OPERATE_SUCCESS		200									//�����ɹ�
#define DBO_GR_OPERATE_FAILURE		201									//����ʧ��	
//////////////////////////////////////////////////////////////////////////////////
//��¼�ɹ�
struct DBO_GR_LogonSuccess
{
	//��������	
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D	
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwStationID;						//վ���ʶ
	TCHAR							szNickName[LEN_ACCOUNTS];			//����ǳ�
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//�Զ�ͷ��
	DWORD							dwCustomID;							//�Զ���ʶ

	//������Ϣ
	tagIMSocketInfo					SocketInfo;							//������Ϣ

	//Ⱥ���б�
	WORD							wGroupCount;						//Ⱥ������	
};

//��¼ʧ��
struct DBO_GR_LogonFailure
{
	//�û���Ϣ	
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//����Ⱥ��
struct DBO_GR_CreateGroup
{
	tagIMGroupItem					GroupItem;							//Ⱥ������
	tagIMGroupMember				GroupMember;						//Ⱥ���Ա
};

//����Ⱥ��
struct DBO_GR_UpdateGroup
{
	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ	
	BYTE							cbUpdateKind;						//��������
};

//ɾ��Ⱥ��
struct DBO_GR_DeleteGroup
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//Ⱥ��״̬
struct DBO_GR_GroupStatus
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	BYTE							cbStatus;							//Ⱥ��״̬
};

//��������
struct DBO_GR_InOutMode
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	BYTE							cbInMode;							//��������0������ˣ�1�����
	BYTE							cbInType;							//���뷽ʽ0��������루����ID����1������ID����
	BYTE							cbOutMode;							//�˳�����0������ˣ�1�����
};

//���������
struct DBO_GR_RollMsg
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbFlag;								//0�رգ�1����
	TCHAR							szRollMsg[33];						//��Ϣ�������32����
	INT								nTime;								//�������
	BYTE							cbType;								//1����Ȧ��Ϣ��2�ӳ���Ϣ
};

//����������
struct DBO_GR_GroupName
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	TCHAR							szGroupName[GROUP_LEN_NAME];		//����
};

//ѫ�����ý��
struct DBO_GR_SetMedalResult
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	BYTE							cbMedalMode;						//0�رգ�1����
	BYTE							cbMedalShow;						//0����ʾ��1��ʾ
	BYTE							cbCurMedalShow;						//0�����˿ɼ���1����Ա�ɼ�
	BYTE							cbMedalRevenueShow;					//0�����˿ɼ���1����Ա�ɼ�
	BYTE							cbMedalClear;						//0�����㣬1�������
};

//�������ý��
struct DBO_GR_FreeTableLimit
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	BYTE							cbCount;							//����
};


//��Ӯ�����ý��
struct DBO_GR_BigWinnerShow
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	BYTE							cbShowMode;							//0���пɼ���1����Ա�ɼ�
};

//��ӳ�Ա
struct DBO_GR_InsertMember
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	tagIMGroupMemberUser			MemberUser;							//��Ա�û�
};

//ɾ����Ա
struct DBO_GR_DeleteMember
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�������
	BYTE							cbType;								//0�����˳���1��ɾ��
	INT								nCount;								//���������tagIMDeleteMember
};

//�Ƹ�����
struct DBO_GR_WealthUpdate
{
	DWORD							dwUserID;							//�û���ʶ
	LONGLONG						lUserIngot;							//�û���ʯ
	LONGLONG						lUserRoomCard;						//�û�����
	LONGLONG						lUserMedal;							//�û�ѫ��
};

//������Ϣ
struct DBO_GR_ApplyMessage
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbMessageType;						//��Ϣ����
	WORD							wMessageCount;						//��Ϣ����	
};

//ɾ�����
struct DBO_GR_ApplyDelResult
{
	DWORD							dwCreaterID;						//Ⱥ��I D
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageType;						//��Ϣ����
	bool							bDelResult;							//ɾ�����
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//�����ɹ�
struct DBO_GR_OperateSuccess
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	LONG							lOperateCode;						//��������
	TCHAR							szOperateNotice[128];				//������ʾ
};

//����ʧ��
struct DBO_GR_OperateFailure
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//����޸�Ȩ��
struct DBO_GR_ModifyRight
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	INT								nRight;								//�޸�Ȩ��
	BYTE							cbMode;								//0����Ȩ�ޣ�1ɾ��Ȩ��

	DWORD							dwMemberID;							//��Ա��ʶ
	INT								nMemberRight;						//�޸ĺ�Ȩ��


};

//�Ƴ��ӳ�
struct DBO_GR_CancelCaptain
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	INT								nMemberRight;						//�޸ĺ�Ȩ��
};

//����ԱȨ��
struct DBO_GR_ManagerRight
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwTargetID;							//Ŀ���û�ID
	LONGLONG						lManagerRight;
	LONGLONG						lCaptainRight;
};

//���·ֽ��
struct DBO_GR_PresentSuccess
{
	//�û���Ϣ
	DWORD							dwGroupID;							//������ID
	DWORD							dwPresentID;						//�������ID
	BYTE							cbType;								//����0���ţ�1����
	INT								nCount;								//�ɹ�����
};

//���ͼ�¼�б�
struct DBO_GR_Present_List
{
	DWORD							dwGroupID;							//������ID
	INT								nCount;								//��¼����
	bool							bListEnd;							//����
};

//�۷Ѽ�¼�б�
struct DBO_GR_QueryRevenue_List
{
	DWORD							dwGroupID;							//������ID
	INT								nCount;								//��¼����
	bool							bListEnd;							//����
};

//�ռ�¼�б�
struct DBO_GR_QueryDayRecord_List
{
	DWORD							dwGroupID;							//������ID
	INT								nCount;								//��¼����
	bool							bListEnd;							//����
};

//����������б�
struct DBO_GR_QueryUserDayDetail_List
{
	DWORD							dwGroupID;							//������ID
	DWORD							dwUserID;							//���ID
	DWORD							dwGameID;							//���GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			//����ǳ�
	INT								nPlayCount;							//�Ծִ���
	INT								nBigWinnerCount;					//��Ӯ�Ҵ���
	SCORE							lCurScore;							//��ǰ����
	SCORE							lGameScore;							//��Ӯ����
	INT								nCount;								//�����������
	bool							bListEnd;							//����
};

//�������������б�
struct DBO_GR_QueryGroupDayDetail_List
{
	DWORD							dwGroupID;							//������ID
	INT								nCount;								//��¼����
	INT								nDiamond;							//��������ʯ
	LONGLONG						lAllRoomRevenue;					//�ܹ���
	bool							bListEnd;							//����
};

//�����淨
struct DBO_GR_CreateConfig
{
	DWORD							dwGroupID;
	SCORE							lDiamond;
	tagGroupConfigInfo				ConfigInfo;
};

//�޸��淨
struct DBO_GR_ModifyConfig
{
	DWORD							dwGroupID;
	tagGroupConfigInfo				ConfigInfo;
};

//ɾ���淨
struct DBO_GR_DeleteConfig
{
	DWORD							dwGroupID;
	DWORD							dwConfigID;
};

//��ѯ��������
struct DBO_GR_QueryCreateResult
{
	DWORD							dwGroupID;
	DWORD							dwConfigID;
	DWORD							dwUserID;
	INT								nMaxNullTableCount;
};

//��ѯѫ��ģʽ
struct DBO_GR_MedalMode
{
	DWORD							dwGroupID;
	BYTE							cbMedalMode;
};

//��������
struct DBO_GR_QueryRoomDetail_List
{
	DWORD							dwRoomID;							//����ID
	TCHAR							szKindName[LEN_ACCOUNTS];			//��Ϸ����
	SCORE							lMinTableScore;						//����볡��
	INT								nDrawCount;							//�ܾ���
	BYTE							cbPlayerCount;						//�������
	TCHAR							szNickName[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT][LEN_NICKNAME];		//����ǳ�
	SCORE							lAllScore[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];						//�����б��ܷ�
	INT								nListCount;
	bool							bListEnd;							//����
};

//�ӳ��б�
struct DBO_GR_Captain_List
{
	DWORD							dwGroupID;							//������ID
	INT								nCaptainCount;						//�����ӳ�����
	INT								nPlayCount;							//�ܾ���
	INT								nBigWinnerCount;					//��Ӯ������
	SCORE							lRevenueCount;						//����˰���ۼ�
	SCORE							lContributionCount;					//�����ۼ�
	INT								nCount;								//����
	bool							bListEnd;							//����
};

//�����Ա
struct DBO_GR_Add_Teamer
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//����ID
	DWORD							dwCaptainID;						//�ӳ�ID
	//DWORD							dwMemberID;							//��ԱID
	BYTE							cbMode;								//0�Ƴ���1���
	INT								nCount;								//���������tagIMAddTeamer
};

//������
struct tagIMAddTeamer
{
	DWORD							dwUserID;							//�û�ID
	BYTE							cbSuccess;							//0ʧ�ܣ�1�ɹ�
};

//С����ͳ���б�
struct DBO_GR_TeamDayRecord_List
{
	DWORD							dwGroupID;							//������ID
	INT								nCount;								//��¼����
	bool							bListEnd;							//����
};

//С�ӳ�Ա��ͳ���б�
struct DBO_GR_TeamerDayRecord_List
{
	DWORD							dwGroupID;							//������ID
	INT								nCount;								//��¼����
	bool							bListEnd;							//����
};

//��˽���ý��
struct DBO_GR_Privacy
{
	DWORD							dwGroupID;							//������ID
	BYTE							cbShowGroupID;						//0����ID,1��ʾID
};

//��λ���ý��
struct DBO_GR_CaptainPoint
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwCaptainID;						//�ӳ�ID
	WORD							wPoint;								//��λ
};

//�򿪱��ս��
struct DBO_GR_BoxInfo
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//���ID
	WORD							wPoint;								//��λ
	SCORE							lAwardScore;						//������
	SCORE							lTodayGetScore;						//��������ȡ��
	INT								nCount;								//��ȡ��¼����
	bool							bListEnd;
};

//��ȡ���ֽ��
struct DBO_GR_GetAwardScoreResult
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//���ID
	WORD							wAwardPoint;						//��λ
	SCORE							lCurAwardScore;						//��ǰ������
	SCORE							lGetScore;							//��ȡ��
};

//��������Ϣ
struct DBO_GR_InvitationCode
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//���ID
	LONGLONG						lInvitationCode;					//������
};

//�󶨳ɹ�
struct DBO_GR_BlindCodeRes
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//���ID
	DWORD							dwCaptainID;						//�ӳ�ID
};

//��ѯ�������
struct DBO_GR_QueryScoreResult
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//���ID
	LONGLONG						lScore;								//���÷�
};

//�޸����Ͻ��
struct DBO_GR_ModifyInfo
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	INT								nMemberRight;						//���Ȩ��

	BYTE							cbFlag[3];							//�޸ı�ǩ��1��Ч
	TCHAR							szQQ[LEN_NICKNAME];					//�û�QQ
	TCHAR							szWX[LEN_NICKNAME];					//�û�WX
	TCHAR							szPhone[12];						//�û��ֻ���

};
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//�������ݰ�

//ϵͳ����
#define DBR_GR_LOAD_GROUP_LIST		301									//����Ⱥ��
#define DBR_GR_LOAD_GROUP_ITEM		302									//����Ⱥ��
#define DBR_GR_LOAD_GROUP_MEMBER	303									//Ⱥ���Ա
#define DBR_GR_LOAD_GROUP_CONFIG	304									//Ⱥ���淨
#define DBR_GR_LOAD_SINGLE_GROUP	305									//���ص������ֲ�����

//��Ա����
#define DBR_GR_UPDATE_MEMBER		310									//���³�Ա

//////////////////////////////////////////////////////////////////////////
//�����Ϣ

//Ⱥ���б�
#define DBO_GR_GROUP_LIST			400									//Ⱥ������
#define DBO_GR_GROUP_LIST_FINISH	401									//�б����

//Ⱥ������
#define DBO_GR_GROUP_ITEM			410									//Ⱥ������
#define DBO_GR_GROUP_ITEM_FINISH	411									//�������	

//Ⱥ���Ա
#define DBO_GR_GROUP_MEMBER			420									//Ⱥ���Ա
#define DBO_GR_UPDATE_MEMBER		421									//���³�Ա

//Ⱥ���淨
#define DBO_GR_GROUP_CONFIG			422									//Ⱥ���淨
//Ⱥ�鹫������
#define DBO_GR_COMMON_INFO			423									//��������
//////////////////////////////////////////////////////////////////////////////////
//Ⱥ����Ϣ

//����Ⱥ���б�
struct DBR_GR_LoadGroupList
{
	DWORD							dwGroupStartID;						//Ⱥ���ʶ
	DWORD							dwGroupEndID;						//Ⱥ���ʶ
};

//����Ⱥ����Ϣ
struct DBR_GR_LoadGroupItem
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//Ⱥ���Ա
struct DBR_GR_LoadGroupMember
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//Ⱥ���Ա
struct DBR_GR_LoadSingleGroup
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};



//Ⱥ���淨
struct DBR_GR_LoadGroupConfig
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//���³�Ա
struct DBR_GR_UpdateMember
{
	//��Ա��Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ

	//��������							
	BYTE							cbUpdateKind;						//��������
	WORD							wIncBattleCount;					//��������
};

//////////////////////////////////////////////////////////////////////////////////
//Ⱥ���Ա
struct DBO_GR_GroupMember
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//���³�Ա
struct DBO_GR_UpdateMember
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	tagIMGroupMember				GroupMember;						//Ⱥ���Ա
};

//Ⱥ���淨
struct DBO_GR_GroupConfig
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//���ͳɹ�
struct SUB_GR_PresentSuccess
{
	//�û���Ϣ
	DWORD							dwGroupID;							//������ID
	DWORD							dwPresentID;						//�������ID
	DWORD							dwReceiveID;						//�û���ʶ
	SCORE							lScore;								//ѫ��
	BYTE							cbType;								//����0���ţ�1����
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()
#endif