#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�������ݰ�
#define  DATA_BASE_REQUEST_ID 10000000
//�û��¼�
#define	DBR_GR_LOGON_USERID			100									//I D ��¼
#define	DBR_GR_LOGON_MOBILE			101									//�ֻ���¼
#define	DBR_GR_LOGON_ACCOUNTS		102									//�ʺŵ�¼
#define DBR_GR_LOGON_MATCH			103									//������¼
#define	DBR_GR_LOGON_AI				104									//I D ��¼

//ϵͳ�¼�
#define DBR_GR_WRITE_GAME_SCORE		200									//��Ϸ����
#define DBR_GR_LEAVE_GAME_SERVER	201									//�뿪����
#define DBR_GR_GAME_SCORE_RECORD	202									//���ּ�¼
#define DBR_GR_MANAGE_USER_RIGHT	203									//Ȩ�޹���
#define DBR_GR_LOAD_SYSTEM_MESSAGE	204									//ϵͳ��Ϣ
#define DBR_GR_LOAD_SENSITIVE_WORDS	205									//���д�	
#define DBR_GR_UNLOCK_ANDROID_USER	206									//����AI
#define DBR_GR_GROUP_SCORE_RECORD	207									//�����˼�¼

//�����¼�
#define DBR_GR_LOAD_PARAMETER		300									//���ز���
#define DBR_GR_LOAD_GAME_COLUMN		301									//�����б�
#define DBR_GR_LOAD_ANDROID_PARAMETER 302								//���ز���
#define DBR_GR_LOAD_ANDROID_USER	303									//���ػ���
#define DBR_GR_LOAD_GAME_CHAT		304									//��������

#define DBR_GR_LOAD_GROWLEVEL_CONFIG 305								//�ȼ�����
#define DBR_GR_LOAD_GAME_BUFF		306									//����Buff ���磺1Сʱ˫�����֣�
#define DBR_GR_LOAD_CHECKIN_REWARD	307 								//����ǩ��
#define DBR_GR_LOAD_BASEENSURE		308 								//���صͱ� 
#define DBR_GR_LOAD_TRUMPET			309									//��������
#define DBR_GR_LOAD_MEMBER_PARAMETER 310								//��Ա����

//�����¼�
#define DBR_GR_USER_ENABLE_INSURE	400									//��ͨ����
#define DBR_GR_USER_SAVE_SCORE		401									//������Ϸ��
#define DBR_GR_USER_TAKE_SCORE		402									//��ȡ��Ϸ��
#define DBR_GR_USER_TRANSFER_SCORE	403									//ת����Ϸ��
#define DBR_GR_QUERY_INSURE_INFO	404									//��ѯ����
#define DBR_GR_QUERY_TRANSFER_USER_INFO	    405							//��ѯ�û�

//��Ϸ�¼�
#define DBR_GR_GAME_FRAME_REQUEST	502									//��Ϸ����
#define DBR_GR_WRITE_USER_GAME_DATA	503									//�û���Ϸ����

//�����¼�
#define DBR_GR_MATCH_SIGNUP			600									//��������
#define DBR_GR_MATCH_UNSIGNUP		601									//�˳�����
#define DBR_GR_MATCH_SIGNUP_START	602									//��ʼ����
#define DBR_GR_MATCH_QUERY_QUALIFY	603									//��ѯ�ʸ�
#define DBR_GR_MATCH_BUY_SAFECARD	604									//�����տ�	
#define DBR_GR_MATCH_START			605									//������ʼ
#define DBR_GR_MATCH_OVER			606									//��������
#define DBR_GR_MATCH_CANCEL			607									//����ȡ��
#define DBR_GR_MATCH_REWARD			608									//��������
#define DBR_GR_MATCH_ELIMINATE      609									//������̭
#define DBR_GR_MATCH_QUERY_REVIVE	610									//��ѯ����
#define DBR_GR_MATCH_USER_REVIVE	611									//�û�����
#define DBR_GR_MATCH_RECORD_GRADES	612									//��¼�ɼ�
#define DBR_GR_MATCH_ROUNDS_OVER	613									//ÿ�ֽ���
#define DBR_GR_MATCH_ANDROID_DEBUG	614									//AI����

//�����¼�
#define DBR_GR_TASK_TAKE			700									//��ȡ����
#define DBR_GR_TASK_REWARD			701									//��ȡ����
#define DBR_GR_TASK_LOAD_LIST		702									//��������
#define DBR_GR_TASK_QUERY_INFO		703									//��ѯ����
#define DBR_GR_TASK_GIVEUP			704									//��������
#define DBR_GP_TASK_PERFORM			705									//�ƽ�����

//�һ��¼�
#define DBR_GR_PURCHASE_MEMBER		801									//�����Ա
#define DBR_GR_EXCHANGE_SCORE_INGOT	802									//�һ���Ϸ��
#define DBR_GR_EXCHANGE_SCORE_BEANS	803									//�һ���Ϸ��

//�ȼ�����
#define DBR_GR_GROWLEVEL_QUERY_IFNO	 820								//��ѯ�ȼ�

//�����¼�
#define DBR_GR_LOAD_PROPERTY		900									//���ص���
#define DBR_GR_GAME_PROPERTY_BUY	901									//�������
#define DBR_GR_QUERY_BACKPACK		902									//��������
#define DBR_GR_PROPERTY_USE			903									//ʹ�õ���
#define DBR_GR_QUERY_SEND_PRESENT	904									//��ѯ����
#define DBR_GR_PROPERTY_PRESENT		905									//���͵���
#define DBR_GR_GET_SEND_PRESENT		906									//��ȡ����
#define DBR_GR_SEND_TRUMPET			907									//ʹ������

//ǩ���¼�
#define DBR_GR_CHECKIN_DONE			1000								//ִ��ǩ��
#define DBR_GR_CHECKIN_QUERY_INFO	1001								//��ѯ��Ϣ

//�ͱ��¼�
#define DBR_GR_BASEENSURE_TAKE		1100								//��ȡ�ͱ�

//��Ա�¼�
#define DBR_GR_MEMBER_QUERY_INFO	1200								//��Ա��ѯ
#define DBR_GR_MEMBER_DAY_PRESENT	1201								//��Ա�ͽ�
#define DBR_GR_MEMBER_DAY_GIFT		1202								//��Ա���

//˽�˷���
#define DBR_GR_WRITE_PERSONAL_GAME_SCORE	1300						//д˽�˷���Ϸ����
//˽�˷���
#define DBR_GR_CREATE_TABLE					1301						//��������
#define DBR_GR_CANCEL_CREATE_TABLE			1302						//ȡ������
#define DBR_GR_LOAD_PERSONAL_PARAMETER		1303						//˽������

#define DBR_GR_INSERT_CREATE_RECORD			1304						//���봴����¼
#define DBR_GR_DISSUME_ROOM					1305						//��ɢ����
#define DBR_GR_QUERY_USER_ROOM_SCORE		1306						//������󷿼�ɼ�
#define DBO_GR_LOAD_PERSONAL_ROOM_OPTION	1307						//˽������
#define DBR_GR_HOST_CANCEL_CREATE_TABLE		1308						//ȡ������

#define DBR_GR_WRITE_JOIN_INFO				1309						//ȡ������
#define DBR_GR_WRITE_GROUP_TABLE_INFO		1310						//������һ����Ϣ
#define DBR_GR_AI_CREATE_TABLE				1311						//AI��������
#define DBR_GR_PAY_ROOM_CARD				1400

#define DBR_GR_WRITE_VIDEO_PLAYER_INFO		1600
#define DBR_GR_WRITE_VIDEO_DATA_INFO		1601
#define DBR_GR_WRITE_ROOM_TASK_PROGRESS		1700						//�Ƽ�Լս����
	
#define DBR_GR_MODIFY_CONFIG_SUBSTORAGE_ITEM 1701						//�޸����ÿ������
#define DBR_GR_MODIFY_CUR_SUBSTORAGE_ITEM	 1702						//�޸ĵ�ǰ�������
#define DBR_GR_LOAD_SHARESTORAGE_ITEM		 1703						//���ع�����
#define DBR_GR_LOAD_SINGLE_GROUPSHARESTORAGE_ITEM	 1704				//���ص���Ⱥ�鹲����

//////////////////////////////////////////////////////////////////////////////////
//�����Ϣ

//�߼��¼�
#define DBO_GR_LOGON_SUCCESS		100									//��¼�ɹ�
#define DBO_GR_LOGON_FAILURE		101									//��¼ʧ��
#define DBO_GR_AI_LOGON_SUCCESS		102									//��¼�ɹ�

//�����¼�
#define DBO_GR_GAME_PARAMETER		200									//������Ϣ
#define DBO_GR_GAME_COLUMN_INFO		201									//�б���Ϣ
#define DBO_GR_GAME_ANDROID_PARAMETER 202								//������Ϣ
#define DBO_GR_GAME_ANDROID_INFO	203									//������Ϣ
#define DBO_GR_GAME_UNLOCK_ANDROID	204									//��������
#define DBO_GR_GAME_CHAT_INFO		205									//������Ϣ

#define DBO_GR_GAME_MEMBER_PAREMETER 206								//��Ա����
#define DBO_GR_GAME_PROPERTY_BUFF	207									//����BUFF
#define DBO_GR_CHECKIN_REWARD		208									//ǩ������
#define DBO_GR_BASEENSURE_PARAMETER	209									//�ͱ���Ϣ
#define DBO_GR_LOAD_TRUMPET			210									//��������
#define DBO_GR_SEND_TRUMPET			211									//��������
#define DBO_GR_GAME_PROPERTY_ITEM	220									//������Ϣ

//��������
#define DBO_GR_USER_INSURE_INFO		300									//��������
#define DBO_GR_USER_INSURE_SUCCESS	301									//���гɹ�
#define DBO_GR_USER_INSURE_FAILURE	302									//����ʧ��
#define DBO_GR_USER_INSURE_USER_INFO   303								//�û�����
#define DBO_GR_USER_INSURE_ENABLE_RESULT 304							//��ͨ���

//��Ϸ�¼�
#define DBO_GR_PROPERTY_SUCCESS		400									//���߳ɹ�
#define DBO_GR_PROPERTY_FAILURE		401									//����ʧ��
#define DBO_GR_GAME_FRAME_RESULT	402									//��Ϸ���

//�����¼�
#define DBO_GR_MATCH_EVENT_START	500									//������ʶ
#define DBO_GR_MATCH_SIGNUP_RESULT	501									//�������
#define DBO_GR_MATCH_UNSIGNUP_RESULT 502								//�������
#define DBO_GR_MATCH_QUERY_RESULT	503									//��ѯ���
#define DBO_GR_MATCH_BUY_RESULT		504									//������
#define DBO_GR_MATCH_REVIVE_INFO	505									//������Ϣ
#define DBO_GR_MATCH_REVIVE_RESULT	506									//������		
#define DBO_GR_MATCH_RANK_LIST		507									//��������
#define DBO_GR_MATCH_REWARD_RESULT  508									//�������
#define DBO_GR_MATCH_CANCEL_RESULT	509									//����ȡ��
#define DBO_GR_MATCH_EVENT_END		599									//������ʶ
#define DBO_GR_MATCH_ROUNDS_RANK_LIST	510								//��������
#define DBO_GR_MATCH_ANDROID_DEBUG	511									//AI����
//ϵͳ�¼�
#define DBO_GR_SYSTEM_MESSAGE_RESULT	600								//��Ϣ���
#define DBO_GR_SYSTEM_MESSAGE_FINISH	601								//��Ϣ���	
#define DBO_GR_SENSITIVE_WORDS		    602								//���д�

//�����¼�
#define DBO_GR_TASK_LIST			700									//�����б�
#define DBO_GR_TASK_LIST_END		701									//�������
#define DBO_GR_TASK_INFO			702									//������Ϣ
#define DBO_GR_TASK_RESULT			703									//������

//�һ��¼�
#define DBO_GR_MEMBER_PARAMETER     800									//��Ա����
#define DBO_GR_PURCHASE_RESULT		801									//������
#define DBO_GR_EXCHANGE_RESULT		802									//�һ����

//�ȼ�����
#define DBO_GR_GROWLEVEL_CONFIG		820									//�ȼ�����
#define DBO_GR_GROWLEVEL_PARAMETER	821									//�ȼ�����
#define DBO_GR_GROWLEVEL_UPGRADE	822									//�ȼ�����


//�����¼�
//��������
#define DBO_GP_GAME_PROPERTY_TYPE_ITEM		901							//���ͽڵ�
#define DBO_GP_GAME_PROPERTY_RELAT_ITEM		902							//���߹�ϵ
#define DBO_GP_GAME_PROPERTY_ITEM			903							//���߽ڵ�
#define DBO_GP_GAME_PROPERTY_SUB_ITEM		904							//���߽ڵ�
#define DBO_GP_GAME_PROPERTY_LIST_RESULT	905							//���߽��

#define DBO_GR_LOAD_GAME_PROPERTY	910									//���ص���
#define DBO_GR_GAME_PROPERTY_BUY	911									//�������
#define DBO_GR_QUERY_BACKPACK		912									//��ѯ����
#define DBO_GR_PROPERTY_USE			913									//����ʹ��
#define DBO_GR_QUERY_SEND_PRESENT	914									//��ѯ����
#define DBO_GR_PROPERTY_PRESENT		915									//���͵���
#define DBO_GR_GET_SEND_PRESENT		916									//��ȡ����
#define DBO_GR_GAME_PROPERTY_FAILURE 920								//����ʧ��
//ǩ���¼�
#define DBO_GR_CHECKIN_INFO			1000								//ǩ����Ϣ
#define DBO_GR_CHECKIN_RESULT		1001								//ǩ�����

#define DBO_GR_BASEENSURE_RESULT	1100								//��ȡ���

//��Ա�¼�
#define DBO_GR_MEMBER_QUERY_INFO_RESULT				1201					//��ѯ���
#define DBO_GR_MEMBER_DAY_PRESENT_RESULT			1202					//�ͽ���
#define DBO_GR_MEMBER_DAY_GIFT_RESULT				1203					//������

//˽�˷���
#define DBO_GR_CREATE_SUCCESS						1300					//�����ɹ�
#define DBO_GR_CREATE_FAILURE						1301					//����ʧ��
#define DBO_GR_CANCEL_CREATE_RESULT					1302					//ȡ������
#define DBO_GR_LOAD_PERSONAL_PARAMETER				1303					//˽������
#define DBO_GR_DISSUME_TABLE_RESULTE				1304					//��ɢ������
#define DBO_GR_CURRENCE_ROOMCARD_AND_BEAN			1305					//��ɢ������
#define DBO_GR_GROUP_CURRENCE_FEE					1306					//֧�����
#define DBO_GR_AI_CREATE_SUCCESS					1308					//AI�����ɹ�

//�������¼�
#define DBO_GR_LOAD_SHARESTORAGE_ITEM_RESULT		1309					//���ع�������


//////////////////////////////////////////////////////////////////////////////////

//ID ��¼
struct DBR_GR_LogonUserID
{		
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//�������
	BYTE							cbMatchType;						//��������
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//ID ��¼
struct DBR_GR_LogonAIUser
{
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//�������
	BYTE							cbMatchType;						//��������
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	DWORD							dwGroupID;							//���ֲ���ʶ
	WORD							wTableID;							//���ӱ�ʶ
};

//ID ��¼
struct DBR_GR_LogonMobile
{		
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	BYTE                            cbDeviceType;                       //�豸����
	WORD                            wBehaviorFlags;                     //��Ϊ��ʶ
	WORD                            wPageTableCount;                    //��ҳ����

	//������Ϣ
	BYTE							cbPersonalServer;					//˽�˷���
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//�������
	BYTE							cbMatchType;						//��������
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	DWORD							dwGroupID;							//������ID
};

//�ʺŵ�¼
struct DBR_GR_LogonAccounts
{
	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��Ϸ����
struct DBR_GR_WriteGameScore
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//�û���Ϣ
	DWORD							dwDBQuestID;						//�����ʶ
	DWORD							dwInoutIndex;						//��������

	//������Ϣ
	bool							bTaskForward;						//�������

	//�ɼ�����
	tagVariationInfo				VariationInfo;						//��ȡ��Ϣ

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
};

//�뿪����
struct DBR_GR_LeaveGameServer
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwInoutIndex;						//��¼����
	DWORD							dwLeaveReason;						//�뿪ԭ��
	DWORD							dwOnLineTimeCount;					//����ʱ��
	BYTE							cbIsPersonalRoom;					//�Ƿ�Ϊ˽�˷���
	DWORD							dwPlayTime;							//��Ϸʱ��

	//�ɼ�����
	tagVariationInfo				RecordInfo;							//��¼��Ϣ
	tagVariationInfo				VariationInfo;						//��ȡ��Ϣ

	//ϵͳ��Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��Ϸ��¼
struct DBR_GR_GameScoreRecord
{
	//������Ϣ
	WORD							wTableID;							//���Ӻ���
	WORD							wUserCount;							//�û���Ŀ
	WORD							wAndroidCount;						//������Ŀ

	//��ķ������
	SCORE							lWasteCount;						//�����Ŀ
	SCORE							lRevenueCount;						//���������Ŀ

	//ͳ����Ϣ
	DWORD							dwUserMemal;						//������Ŀ
	DWORD							dwPlayTimeCount;					//��Ϸʱ��

	//ʱ����Ϣ
	SYSTEMTIME						SystemTimeStart;					//��ʼʱ��
	SYSTEMTIME						SystemTimeConclude;					//����ʱ��

	//���ּ�¼
	WORD							wRecordCount;						//��¼��Ŀ
	tagGameScoreRecord				GameScoreRecord[MAX_CHAIR];			//��Ϸ��¼
};

//��Ϸ��¼
struct DBR_GR_GroupScoreRecord
{
	//������Ϣ
	DWORD							dwGroupID;
	DWORD							dwRoomID;
	DWORD							dwTurnIndex;
	//ʱ����Ϣ
	SYSTEMTIME						SystemTimeStart;					//��ʼʱ��
	SYSTEMTIME						SystemTimeConclude;					//����ʱ��
	//���ּ�¼
	WORD							wRecordCount;						//��¼��Ŀ
	tagGroupScoreRecord				GroupScoreRecord[MAX_CHAIR];			//��Ϸ��¼
};

//��ͨ����
struct DBR_GR_UserEnableInsure
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D	
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������Ϸ��
struct DBR_GR_UserSaveScore
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D
	SCORE							lSaveScore;							//������Ϸ��
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ȡ����Ϸ��
struct DBR_GR_UserTakeScore
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D
	SCORE							lTakeScore;							//��ȡ��Ϸ��
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ת����Ϸ��
struct DBR_GR_UserTransferScore
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ	
	SCORE							lTransferScore;						//ת����Ϸ��
	DWORD							dwGameID;							//Ŀ��GameID
	//TCHAR							szAccounts[LEN_NICKNAME];			//Ŀ���û�
	TCHAR							szPassword[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szTransRemark[LEN_TRANS_REMARK];	//ת�ʱ�ע
};

//���ػ���
struct DBR_GR_LoadAndroidUser
{
	DWORD							dwAndroidCount;						//������Ŀ
};

//��ѯ����
struct DBR_GR_QueryInsureInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//��ѯ�û�
struct DBR_GR_QueryTransferUserInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE                            cbByNickName;                       //�ǳ�����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
	DWORD							dwUserID;							//�û� I D
};


//���߽ڵ�
struct DBO_GP_GamePropertyTypeItem
{
	DWORD							dwTypeID;							//���ͱ�ʶ
	DWORD							dwSortID;							//�����ʶ
	TCHAR							szTypeName[LEN_TYPE];				//��������
};

//���߽ڵ�
struct DBO_GP_GamePropertyRelatItem
{
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwTypeID;							//���ͱ�ʶ
};

//���߽ڵ�
struct DBO_GP_GamePropertyItem
{
	//������Ϣ
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwPropertyKind;						//��������

	BYTE							cbUseArea;							//ʹ�÷�Χ
	BYTE							cbServiceArea;						//����Χ
	BYTE							cbRecommend;						//�Ƽ���ʶ

	BYTE							cbExchangeType;						//�һ�����
	DWORD							dwExchangeRatio;					//�һ�����

	//��òƸ�
	SCORE							lSendLoveLiness;					//������������ͷ���
	SCORE							lRecvLoveLiness;					//������������շ���
	SCORE							lUseResultsGold;					//��ý��

	//����Ч��
	DWORD							dwUseResultsValidTime;				//��Чʱ��
	DWORD							dwUseResultsValidTimeScoreMultiple;	//��Ч����

	//������
	DWORD							dwUseResultsGiftPackage;			//������

	DWORD							dwSortID;						//�����ʶ
	TCHAR							szName[PROPERTY_LEN_NAME];			 //��������
	TCHAR							szRegulationsInfo[PROPERTY_LEN_INFO];//ʹ����Ϣ
	BYTE							cbPlatformKind;						 //ƽ̨���0:ȫ�� 1:LUA,2:H5,3:U3D
};

//�ӵ��߽ڵ�
struct DBO_GP_GamePropertySubItem
{
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwOwnerPropertyID;					//���߱�ʶ
	DWORD                           dwPropertyCount;                    //������Ŀ
	DWORD							dwSortID;							//�����ʶ
};

//���ؽ��
struct DBO_GP_GamePropertyListResult
{
	BYTE							cbSuccess;							//�ɹ���־
};


//���߹���
struct DBR_GR_PropertyBuy
{
	DWORD							dwUserID;							//�������
	DWORD							dwPropertyID;						//���߱�ʶ
	BYTE							cbUseKind;							//��������(0��ʯ1���)
	DWORD							dwPropCount;						//������Ŀ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};


//��������
struct DBR_GR_PropertyRequest
{
	//������Ϣ
	WORD							wItemCount;							//������Ŀ
	WORD							wPropertyIndex;						//��������
	DWORD							dwSourceUserID;						//�������
	DWORD							dwTargetUserID;						//ʹ�ö���

	//����ģʽ
	BYTE                            cbRequestArea;			            //ʹ�÷�Χ 
	BYTE							cbConsumeScore;						//��������
	SCORE							lFrozenedScore;						//�������

	//�û�Ȩ��
	DWORD                           dwUserRight;						//��ԱȨ��

	//ϵͳ��Ϣ
	WORD							wTableID;							//���Ӻ���
	DWORD							dwInoutIndex;						//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ����
struct DBR_GR_QueryBackpack
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwKindID;							//��������

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//����ʹ��
struct DBR_GR_PropertyUse
{
	//ʹ����Ϣ
	DWORD							dwUserID;							//ʹ����
	DWORD							dwRecvUserID;						//���߶�˭ʹ��
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//��ѯ�ʸ�
struct DBR_GR_QueryMatchQualify
{	
	DWORD							dwUserID;							//�û�ID
	DWORD							dwMatchID;							//����ID
	SCORE							lMatchNo;							//��������
};

//������
struct DBR_GR_MatchBuySafeCard
{
	DWORD							dwUserID;							//�û�ID
	DWORD							dwMatchID;							//����ID
	SCORE							lMatchNo;							//��������	
	SCORE							lSafeCardFee;						//���շ���		
};

//��������
struct DBR_GR_MatchSignup
{
	//��������
	DWORD							dwUserID;							//�û�ID	
	SCORE							lInitScore;							//��ʼ����
	BYTE							cbSignupMode;						//������ʽ
	WORD							wMaxSignupUser;						//�������

	//ϵͳ��Ϣ
	DWORD							dwInoutIndex;						//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwMatchID;							//����ID
	SCORE							lMatchNo;							//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ȡ������
struct DBR_GR_MatchUnSignup
{
	//��������
	DWORD							dwReason;							//ȡ��ԭ��
	DWORD							dwUserID;							//�û�ID

	//ϵͳ��Ϣ
	DWORD							dwInoutIndex;						//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwMatchID;							//����ID
	SCORE							lMatchNo;							//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������ʼ
struct DBR_GR_MatchStart
{
	WORD							wServerID;							//�����ʶ
	BYTE							cbMatchType;						//��������
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������	
};

//������ʼ
struct DBR_GR_SignupStart
{	
	WORD							wServerID;							//�����ʶ
	BYTE							cbMatchType;						//��������
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������	
	BYTE							cbSignupMode;						//������ʽ
};

//AI����
struct DBR_GR_MatchAndroidDebug
{
	WORD							wServerID;							//�����ʶ
	BYTE							cbMatchType;						//��������	
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	WORD							wPromotion;							//��������
	DWORD							dwAndroidID1;						//AIID
	DWORD							dwAndroidID2;						//AIID
	DWORD							dwAndroidID3;						//AIID
	BYTE							cbNo1Ratio1;						//�߳���һ���ٷֱ�
	BYTE							cbNo1Ratio2;						//�߳���һ���ٷֱ�
	BYTE							cbNo1Ratio3;						//�߳���һ���ٷֱ�
		
};

//ÿ�ֽ���
struct DBR_GR_MatchRoundsOver
{
	WORD							wServerID;							//�����ʶ
	BYTE							cbMatchType;						//��������
	BOOL							bMatchFinish;						//�������
	WORD							wPlayCount;							//��Ϸ����
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	BYTE							cbMatchRounds;						//��������
	WORD							wPromotion;							//��������
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchEndTime;						//����ʱ��
};

//��������
struct DBR_GR_MatchOver
{	
	WORD							wServerID;							//�����ʶ
	BYTE							cbMatchType;						//��������
	BOOL							bMatchFinish;						//�������
	WORD							wPlayCount;							//��Ϸ����
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	BYTE							cbMatchRounds;						//��������
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchEndTime;						//����ʱ��
};

//����ȡ��
struct DBR_GR_MatchCancel
{
	BOOL							bMatchFinish;						//�������
	WORD							wServerID;							//�����ʶ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	SCORE							lSafeCardFee;						//���շ���
};

//������̭
struct DBR_GR_MatchEliminate
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wServerID;							//�����ʶ
	BYTE							cbMatchType;						//��������
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������	
};

//��ѯ����
struct DBR_GR_MatchQueryRevive
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������		
};

//�û�����
struct DBR_GR_MatchUserRevive
{
	//������Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������

	//��������
	SCORE							lInitScore;							//��ʼ����
	SCORE							lCullScore;							//��̭����			

	//��������
	BYTE							cbReviveTimes;						//�������
	SCORE							lReviveFee;							//�������	
};

//��������
struct DBR_GR_MatchReward
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lRewardGold;						//�������
	SCORE							lRewardIngot;						//����Ԫ��
	DWORD							dwRewardTicket;						//������ȯ
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//��¼�ɼ�
struct DBR_GR_MatchRecordGrades
{
	//������Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwMatchID;							//������ʶ
	WORD							wServerID;							//�����ʶ
	SCORE							lMatchNo;							//��������
	SCORE							lInitScore;							//��ʼ����
};

//���ȷ���
struct DBR_GR_Send_Trumpet
{
	//ʹ����Ϣ
	DWORD							dwUserID;							//ʹ����
	DWORD							dwRecvUserID;						//���߶�˭ʹ��
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD                           TrumpetColor;                       //������ɫ
	TCHAR							szSendNickName[16];					//����ǳ�
	TCHAR                           szTrumpetContent[TRUMPET_MAX_CHAR]; //��������
};


//��ѯ����
struct DBR_GR_QuerySendPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwClientAddr;						//�ͻ��˵�ַ
};

//��ȡ����
struct DBR_GR_GetSendPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwClientAddr;						//�ͻ��˵�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//���͵���
struct DBR_GR_PropertyPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwRecvGameID;						//���߸�˭(GameID)
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	WORD							wType;								//0 ��������ǳ�  1 �������GameID
	DWORD							dwClientAddr;						//�ͻ��˵�ַ
	TCHAR							szRecvNickName[16];					//���߶�˭ʹ�ã��ǳƣ�
};

//��ѯǩ��
struct DBR_GR_CheckInQueryInfo
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//ִ��ǩ��
struct DBR_GR_CheckInDone
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ�ͱ�
struct DBR_GR_TakeBaseEnsure
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ����
struct DBR_GR_TaskQueryInfo
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//��������
struct DBR_GR_TaskGiveUp
{
	//�û���Ϣ
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};


//��ȡ����
struct DBR_GR_TaskTake
{
	//�û���Ϣ
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ����
struct DBR_GR_TaskReward
{
	//�û���Ϣ
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ�ȼ�
struct DBR_GR_GrowLevelQueryInfo
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�����Ա
struct DBR_GR_PurchaseMember
{
	DWORD							dwUserID;							//�û� I D
	BYTE							cbMemberOrder;						//��Ա��ʶ
	WORD							wPurchaseTime;						//����ʱ��

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�һ���Ϸ��
struct DBR_GR_ExchangeScoreByIngot
{
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lExchangeIngot;						//�һ�Ԫ��

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�һ���Ϸ��
struct DBR_GR_ExchangeScoreByBeans
{
	DWORD							dwUserID;							//�û���ʶ
	double							dExchangeBeans;						//�һ���Ϸ��

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��Ա��ѯ
struct DBR_GR_MemberQueryInfo
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��Ա����
struct DBR_GR_MemberDayPresent
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��Ա���
struct DBR_GR_MemberDayGift
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�û���Ϸ����
struct DBR_GR_WriteUserGameData
{
	//�û���Ϣ
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szUserGameData[LEN_GAME_DATA];		//��Ϸ����
};

//////////////////////////////////////////////////////////////////////////////////

//��¼�ɹ�
struct DBO_GR_LogonSuccess
{
	//��������
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwGroupID;							//���� I D
	DWORD							dwCustomID;							//�Զ�ͷ��
	TCHAR							szNickName[LEN_NICKNAME];			//�ʺ��ǳ�
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������

	//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//��������
	TCHAR							szQQ[LEN_QQ];						//�û�QQ
	TCHAR							szEMail[LEN_EMAIL];					//�û�Eamil
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����	
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//��ϵ��ַ
	TCHAR							szPostalCode[LEN_POSTALCODE];		//��������
	DWORD							dwDayPlayTime;						//ÿ��ʱ��
	SCORE							lDayWinLostScore;					//ÿ�ճɼ�

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lIngot;								//�û�Ԫ��
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lInsure;							//�û�����	
	DOUBLE							dBeans;								//�û���Ϸ��
	SCORE							lDimand;							//��ʯ
	SCORE							lHistoryKindIDWin;					//����Ϸ��ʷ����Ӯ

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����
	SCORE							lIntegralCount;						//��������(��ǰ����)

	//������Ϣ
	DWORD							dwAgentID;							//���� I D

	//�û�Ȩ��
	DWORD							dwUserRight;						//�û�Ȩ��
	DWORD							dwMasterRight;						//����Ȩ��

	//��������
	DWORD							dwInoutIndex;						//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ

	//�������
	WORD							wTaskID;							//�����ʶ
	BYTE							cbTaskStatus;						//����״̬
	DWORD							dwTaskProgress;						//�������	
	SYSTEMTIME						TaskTakeTime;						//��ȡʱ��

	//�ֻ�����
	BYTE                            cbDeviceType;                       //�豸����
	WORD                            wBehaviorFlags;                     //��Ϊ��ʶ
	WORD                            wPageTableCount;                    //��ҳ����

	//������Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szDescribeString[128];				//������Ϣ
	TCHAR							szUserGameData[LEN_GAME_DATA];		//��Ϸ����
	//Ⱥ����Ϣ
	TCHAR							szGroupID[GROUP_ID_ARRAY_LEN];		//���ֲ�ID����

	//���ֲ�AI
	WORD							wTableID;							//����ID

	//������Ϣ
	WORD							wTaskCount;							//������Ŀ
	tagUserTaskInfo					UserTaskInfo[TASK_MAX_COUNT];		//������Ϣ


};

//��¼ʧ��
struct DBO_GR_LogonFailure
{
	LONG							lResultCode;						//������� 
	WORD							wKindID;							//��Ϸ���� ������Ϊ100ʱʹ��
	WORD							wServerID;							//������ID ������Ϊ100ʱʹ��
	TCHAR							szDescribeString[128];				//������Ϣ
};

//������Ϣ
struct DBO_GR_GameParameter
{
	//�汾��Ϣ
	DWORD							dwClientVersion;					//�ͻ��汾
	DWORD							dwServerVersion;					//����汾
	SCORE							lDayWinLostScoreLimit;				//ÿ������
	DWORD							dwDayTimeLimit;						//ÿ������
};

//�б���Ϣ
struct DBO_GR_GameColumnInfo
{
	LONG							lResultCode;						//�������
	BYTE							cbColumnCount;						//�б���Ŀ
	tagColumnItem					ColumnItemInfo[MAX_COLUMN];			//�б���Ϣ
};

//������Ϣ
struct DBO_GR_GameAndroidParameter
{
	LONG							lResultCode;						//�������
	tagAndroidParameter				AndroidParameter;					//������Ϣ
};

//������Ϣ
struct DBO_GR_GameAndroidInfo
{
	LONG							lResultCode;						//�������
	WORD							wAndroidCount;						//�û���Ŀ
	tagAndroidAccountsInfo			AndroidAccountsInfo[MAX_ANDROID];	//�����ʺ�
};

//������Ϣ
struct DBO_GR_GameChatInfo
{
	LONG							lResultCode;						//�������
	BYTE							cbGameChatCount;					//������Ŀ
	tagGameChatItem					GameChatItemInfo[MAX_GAMECHAT];		//������Ϣ
};

//////////////////////////////////////////////////////////////////////////
//���߽ڵ�
struct DBO_GR_GamePropertyItem
{
	//������Ϣ
	WORD							wIndex;								//���߱�ʶ
	WORD							wKind;								//��������
	WORD							wUseArea;							//������Χ
	WORD							wServiceArea;						//����Χ
	WORD							wRecommend;							//�Ƽ���ʶ
	WORD							cbSuportMobile;						//֧���ֻ�

	//���ۼ۸�
	SCORE							lPropertyGold;						//���߽��
	DOUBLE							dPropertyCash;						//���߼۸�
	SCORE							lPropertyUserMedal;					//���߽��
	SCORE							lPropertyLoveLiness;				//���߽��

	//���ͲƸ�
	SCORE							lSendLoveLiness;					//��������
	SCORE							lRecvLoveLiness;					//��������
	SCORE							lUseResultsGold;					//��ý��
	DWORD							wUseResultsValidTime;				//��Чʱ��
	WORD							wUseResultsValidTimeScoreMultiple;	//��Ч����
	DWORD							wUseResultsGiftPackage;				//������


	TCHAR							szName[PROPERTY_LEN_NAME];			//��������
	TCHAR							szRegulationsInfo[PROPERTY_LEN_INFO];//ʹ����Ϣ
};

//���ؽ��
struct DBO_GR_GamePropertyListResult
{
	BYTE							cbSuccess;							//�ɹ���־
};

//������Ϣ
struct DBO_GR_GamePropertyInfo
{
	LONG							lResultCode;						//�������
	BYTE							cbPropertyCount;					//������Ŀ
	tagPropertyInfo					PropertyInfo[MAX_PROPERTY];			//������Ϣ
};

//������
struct DBO_GR_PropertyBuyResult
{
	//������Ϣ
	DWORD							dwUserID;							//�û� I D
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwItemCount;						//������Ŀ
	LONGLONG						lDiamond;
	LONGLONG						lInsureScore;						//���д��

	TCHAR							szNotifyContent[128];				//��ʾ����
};

//����ʧ��
struct DBO_GR_PropertyFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};


struct DBO_GR_PropertyUse
{
	DWORD							dwUserID;							//ʹ����
	DWORD							dwRecvUserID;						//���߶�˭ʹ��
	WORD							wUseArea;							//������Χ
	WORD							wServiceArea;						//����Χ
	DWORD							dwPropID;							//����ID
	DWORD							wPropCount;							//ʹ����Ŀ
	DWORD							dwRemainderPropCount;				//ʣ������
	SCORE							Score;								//��Ϸ���

	SCORE							lSendLoveLiness;					//��������
	SCORE							lRecvLoveLiness;					//��������
	SCORE							lUseResultsGold;					//��ý��					
	DWORD							dwPropKind;							//��������
	time_t							tUseTime;							//ʹ�õ�ʱ��
	DWORD							UseResultsValidTime;				//��Чʱ��(��)
	DWORD							dwHandleCode;						//������
	TCHAR							szName[16];							//��������
	BYTE							cbMemberOrder;						//��Ա��ʶ
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//��ҵ���Buff
struct DBO_GR_UserGamePropertyBuff
{
	DWORD							dwUserID;
	BYTE							cbBuffCount;						//Buff��Ŀ
	tagPropertyBuff					PropertyBuff[MAX_BUFF];			
};

//�������
struct DBO_GR_UserGameTrumpet
{
	DWORD							dwTrumpetCount;						//С������
	DWORD							dwTyphonCount;						//��������		
};

//��ҷ�������
struct DBO_GR_Send_Trumpet
{
	//ʹ����Ϣ
	DWORD							dwUserID;							//ʹ����
	DWORD							dwRecvUserID;						//���߶�˭ʹ��
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	DWORD                           TrumpetColor;                       //������ɫ
	BOOL							bSuccful;							//�ɹ�����
	TCHAR							szSendNickName[16];					//����ǳ�
	TCHAR                           szTrumpetContent[TRUMPET_MAX_CHAR]; //��������
};

//��ѯ����
struct DBO_GR_QuerySendPresent
{
	WORD							wPresentCount;						//���ʹ���
	SendPresent						Present[MAX_PROPERTY];		
};

//��ȡ����
struct DBO_GR_GetSendPresent
{
	WORD							wPresentCount;						//���ʹ���
	SendPresent						Present[MAX_PROPERTY];											
};

//���͵���
struct DBO_GR_PropertyPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwRecvUserID;						//���߸�˭(UserID)
	DWORD							dwRecvGameID;						//���߸�˭(GameID)
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	WORD							wType;								//0 ��������ǳ�  1 �������GameID
	TCHAR							szRecvNickName[16];					//���߶�˭ʹ�ã��ǳƣ�
	int								nHandleCode;						//������
	TCHAR							szNotifyContent[64];				//��ʾ����
};

//ǩ������
struct DBO_GR_CheckInReward
{
	SCORE							lRewardGold[LEN_WEEK];				//�������
};

//ǩ����Ϣ
struct DBO_GR_CheckInInfo
{
	WORD							wSeriesDate;						//��������
	bool							bTodayChecked;						//ǩ����ʶ
};

//ǩ�����
struct DBO_GR_CheckInResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lScore;								//��ǰ����
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�ͱ�����
struct DBO_GR_BaseEnsureParameter
{
	SCORE							lScoreCondition;					//��Ϸ������
	SCORE							lScoreAmount;						//��Ϸ������
	BYTE							cbTakeTimes;						//��ȡ����	
};

//�ͱ����
struct DBO_GR_BaseEnsureResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lGameScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//��������
struct DBO_GR_UserInsureInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE							cbEnjoinTransfer;					//ת�ʿ���
	WORD							wRevenueTake;						//�������
	WORD							wRevenueTransfer;					//�������
	WORD							wRevenueTransferMember;				//�������
	WORD							wServerID;							//�����ʶ
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserInsure;						//������Ϸ��
	SCORE							lTransferPrerequisite;				//ת������
	DWORD							dwUserRight;						//�û�Ȩ��
	SCORE							lBankOPButtonArray[LEN_BANKEROP_BT];//���д�ȡ������ť����
};

//���гɹ�
struct DBO_GR_UserInsureSuccess
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D
	SCORE							lSourceScore;						//ԭ����Ϸ��
	SCORE							lSourceInsure;						//ԭ����Ϸ��
	SCORE							lInsureRevenue;						//���з������
	SCORE							lFrozenedScore;						//�������
	SCORE							lVariationScore;					//��Ϸ�ұ仯
	SCORE							lVariationInsure;					//���б仯
	TCHAR							szDescribeString[128];				//������Ϣ
};

//����ʧ��
struct DBO_GR_UserInsureFailure
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	LONG							lResultCode;						//��������
	SCORE							lFrozenedScore;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�û�����
struct DBO_GR_UserTransferUserInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwGameID;							//�û� I D
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�ʺ��ǳ�
};

//��ͨ���
struct DBO_GR_UserInsureEnableResult
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE							cbInsureEnabled;					//ʹ�ܱ�ʶ
	TCHAR							szDescribeString[128];				//������Ϣ
};

//���߳ɹ�
struct DBO_GR_S_PropertySuccess
{
	//������Ϣ
	WORD							wItemCount;							//������Ŀ
	WORD							wPropertyIndex;						//��������
	WORD							wServiceArea;
	WORD							wPropKind;							//��������
	DWORD							dwSourceUserID;						//�������
	DWORD							dwTargetUserID;						//ʹ�ö���

	//����ģʽ
	BYTE                            cbRequestArea;						//����Χ
	BYTE							cbConsumeScore;						//��������
	SCORE							lFrozenedScore;						//�������

	//��ԱȨ��
	DWORD                           dwUserRight;						//��ԱȨ��

	//�����Ϣ
	SCORE							lConsumeGold;						//������Ϸ��
	SCORE							lSendLoveLiness;					//��������
	SCORE							lRecvLoveLiness;					//��������

	//��Ա��Ϣ
	BYTE							cbMemberOrder;						//��Ա�ȼ�
};

//�����б�
struct DBO_GR_TaskListInfo
{
	WORD							wTaskCount;							//������Ŀ
};

//������Ϣ
struct DBO_GR_TaskInfo
{
	WORD							wTaskCount;							//��������
	tagTaskStatus					TaskStatus[TASK_MAX_COUNT];			//����״̬
};

//�����ƽ�
struct DBO_GR_TaskPerform
{
	DWORD							dwUserID;							//�û�I D
	WORD							wTaskID;							//����I D
	int								nWinCount;							//ʤ����Ŀ
	int								nLostCount;							//�����Ŀ
	int								nDrawCount;							//�;���Ŀ
};
//ǩ�����
struct DBO_GR_TaskResult
{
	//�����Ϣ
	bool							bSuccessed;							//�ɹ���ʶ
	WORD							wCommandID;							//�����ʶ
	WORD							wCurrTaskID;						//��ǰ����

	//�Ƹ���Ϣ
	SCORE							lCurrScore;							//��ǰ���
	SCORE							lCurrIngot;							//��ǰԪ��
	
	//��ʾ��Ϣ
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�ȼ�����
struct DBO_GR_GrowLevelConfig
{
	WORD							wLevelCount;						//�ȼ���Ŀ
	tagGrowLevelConfig				GrowLevelConfig[60];				//�ȼ�����
};

//�ȼ�����
struct DBO_GR_GrowLevelParameter
{
	WORD							wCurrLevelID;						//��ǰ�ȼ�	
	DWORD							dwExperience;						//��ǰ����
	DWORD							dwUpgradeExperience;				//�¼�����
	SCORE							lUpgradeRewardGold;					//��������
	SCORE							lUpgradeRewardIngot;				//��������
};

//�ȼ�����
struct DBO_GR_GrowLevelUpgrade
{
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrIngot;							//��ǰԪ��
	TCHAR							szNotifyContent[128];				//������ʾ
};

//��Ա����
struct DBO_GR_MemberParameter
{
	WORD							wMemberCount;						//��Ա��Ŀ
	tagMemberParameterNew			MemberParameter[64];				//��Ա����
};

//��Ա��ѯ
struct DBO_GR_MemberQueryInfoResult
{
	bool							bPresent;							//��Ա�ͽ�
	bool							bGift;								//��Ա���
	DWORD							GiftSubCount;						//��������
	tagGiftPropertyInfo				GiftSub[50];						//��������
};

//��Ա����
struct DBO_GR_MemberDayPresentResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lGameScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//��Ա���
struct DBO_GR_MemberDayGiftResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//������
struct DBO_GR_PurchaseResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	BYTE							cbMemberOrder;						//��Աϵ��
	DWORD							dwUserRight;						//�û�Ȩ��
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	DOUBLE							dCurrBeans;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};
 
//�һ����
struct DBO_GR_ExchangeResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrInsure;						//��ǰ����
	SCORE							lCurrIngot;							//��ǰԪ��
	double							dCurrBeans;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�������
struct DBO_GR_MatchSingupResult
{
	bool							bResultCode;						//�������
	BYTE							cbSignupMode;						//������ʽ
	DWORD							dwReason;							//ȡ��ԭ��
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lMatchNo;							//��������
	SCORE							lCurrGold;							//��ǰ���
	SCORE							lCurrIngot;							//��ǰԪ��
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ѯ���
struct DBO_GR_MatchQueryResult
{
	bool							bHoldQualify;						//ӵ���ʸ�
};

//������
struct DBO_GR_MatchBuyResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrGold;							//��ǰ���
	TCHAR							szDescribeString[128];				//������Ϣ
};

//������Ϣ
struct DBO_GR_MatchReviveInfo
{	
	bool							bSuccessed;							//�ɹ���ʶ
	bool							bSafeCardRevive;					//���տ�����
	BYTE							cbReviveTimesed;					//�������
};

//������
struct DBO_GR_MatchReviveResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrGold;							//��ǰ���	
	TCHAR							szDescribeString[128];				//������Ϣ
};

//AI����
struct DBO_GR_MatchAndroidDebugResult
{
	SCORE							lMatchNo;							//��������
	WORD							wPromotion;							//��������
};

//��������
struct DBO_GR_MatchRankList
{
	BOOL							bMatchFinish;						//�������
	WORD							wUserCount;							//�û���Ŀ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	SYSTEMTIME						MatchEndTime;						//����ʱ��
	tagMatchRankInfo				MatchRankInfo[128];					//������Ϣ
};

//�������
struct DBO_GR_MatchRewardResult
{
	bool							bResultCode;						//�������
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lCurrGold;							//��ǰ���
};

//ȡ�����
struct DBO_GR_MatchCancelResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	BOOL							bMatchFinish;						//�������
};

//Ȩ�޹���
struct DBR_GR_ManageUserRight
{
	DWORD							dwUserID;							//Ŀ���û�
	DWORD							dwAddRight;							//���Ȩ��
	DWORD							dwRemoveRight;						//ɾ��Ȩ��
	bool							bGameRight;							//��ϷȨ��
};

//Ȩ�޹���
struct DBR_GR_ManageMatchRight
{	
	DWORD							dwUserID;							//Ŀ���û�
	DWORD							dwAddRight;							//���Ȩ��
	DWORD							dwRemoveRight;						//ɾ��Ȩ��	
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������
};
//ϵͳ��Ϣ
struct DBR_GR_SystemMessage
{
	DWORD                           dwMessageID;                        //��ϢID
	BYTE							cbMessageType;						//��Ϣ����
	BYTE                            cbAllRoom;                          //ȫ�巿��
	DWORD							dwTimeRate;						    //ʱ��Ƶ��
	__time64_t						tStartTime;							//��ʼʱ��
	__time64_t                      tConcludeTime;                      //����ʱ��
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//ϵͳ��Ϣ
};
//����AI
struct DBR_GR_UnlockAndroidUser
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wServerID;							//����ID
	BYTE							cbAllAndroid;						//���б�ʶ
};

//��Ϸ����
struct DBR_GR_WritePersonalGameScore
{
	//�û���Ϣ
	DWORD							dwUserID;								//�û���ʶ
	DWORD							dwClientAddr;							//���ӵ�ַ

	//�û���Ϣ
	DWORD							dwDBQuestID;							//�����ʶ
	DWORD							dwInoutIndex;							//��������

	//������Ϣ
	bool							bTaskForward;							//�������

	//�ɼ�����
	tagVariationInfo				VariationInfo;							//��ȡ��Ϣ
	//������Ϣ
	DWORD							dwMatchID;								//������ʶ
	DWORD							dwMatchNO;								//��������
	
	//����ID	
	DWORD							dwRoomHostID;							//����ID
	DWORD							dwPersonalRoomTax;						//˽�˷��������
	DWORD							dwPersonalRoomID;						//Լս����ID
	DWORD							dwGroupID;								//���ֲ�ID
	TCHAR							szPersonalRoomGUID[PERSONAL_ROOM_GUID];	//����ΨһID
	char							szVideoNumber[LEN_VIDEO_NUMBER];		//��ƵID	
	BYTE							cbLoopCount;							//Ȧ��
	int								nGamesNum;								//��Ϸ����
	BYTE							cbGameMode;								//��Ϸģʽ
	BYTE							cbPlayMode;								//0 ���ֳ� 1 ��ҳ�
	BYTE							cbGameoverData[LEN_GAMEOVER_DATA];		//��Ϸ����ʱ����
	WORD							wGameoverDataSize;						//ʵ�ʳ���

};

//��������
struct DBR_GR_CreateTable
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientAddr;						//�û���ַ
	LONG							lCellScore;							//���õ׷�
	DWORD							dwServerID;							//�����ʶ
	DWORD							dwTableID;							//Ԥ��������
	WORD							wJoinGamePeopleCount;				//������Ϸ������
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	DWORD							dwRoomTax;							//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	BYTE							cbPassword;							//�����־
	BYTE							cbGameRule[RULE_LEN];				//��Ϸ���� �� 0 λ��ʶ �Ƿ����ù��� 0 �������� 1 ����δ����
	//����λ��Ϸ�Զ������ ����Э������Ϸ����˺Ϳͻ����̶�
	BYTE							cbPayMode;							//֧����ʽ
	BYTE							cbGameMode;							//��Ϸģʽ
	BYTE							cbPlayMode;							// 0 ����ģʽ  1 ���ģʽ
	LONGLONG						lLeaveScore;						//�뿪�����������
	LONGLONG						lEnterScore;						//���뷿���������
	BYTE							cbJoinMode;							//�Ƿ�����İ���˼���	0 ������	1 ����
	DWORD							dwGroupID;							//���ֲ�id�� �Ǿ��ֲ����� �� 0
	DWORD							dwConfigID;							//�淨ID
	bool							bGroupPersonalMaxChair;				//�����˰�����Ϸ��ʶ
};

//AI��������
struct DBR_GR_AICreateTable
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientAddr;						//�û���ַ
	LONG							lCellScore;							//���õ׷�
	DWORD							dwServerID;							//�����ʶ
	DWORD							dwTableID;							//Ԥ��������
	WORD							wJoinGamePeopleCount;				//������Ϸ������
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	DWORD							dwRoomTax;							//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	BYTE							cbPassword;							//�����־
	BYTE							cbGameRule[RULE_LEN];				//��Ϸ���� �� 0 λ��ʶ �Ƿ����ù��� 0 �������� 1 ����δ����
	//����λ��Ϸ�Զ������ ����Э������Ϸ����˺Ϳͻ����̶�
	BYTE							cbPayMode;							//֧����ʽ
	BYTE							cbGameMode;							//��Ϸģʽ
	BYTE							cbPlayMode;							// 0 ����ģʽ  1 ���ģʽ
	LONGLONG						lLeaveScore;						//�뿪�����������
	LONGLONG						lEnterScore;						//���뷿���������
	BYTE							cbJoinMode;							//�Ƿ�����İ���˼���	0 ������	1 ����
	DWORD							dwGroupID;							//���ֲ�id�� �Ǿ��ֲ����� �� 0
	DWORD							dwConfigID;							//�淨ID
	bool							bGroupPersonalMaxChair;				//�����˰�����Ϸ��ʶ
};

//
struct DBR_GR_CancelCreateTable
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwServerID;							//�û���ʶ
	DWORD							dwClientAddr;						//�û���ַ
	DWORD							dwTableID;							//���� I D
	DWORD							dwReason;							//ȡ��ԭ��
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	DWORD							dwPersonalRoomID;					//Լս����ID
};

//�ƽ�����
struct DBR_GR_PerformRoomTaskProgress
{
	DWORD							dwUserID;							//�û���ʶ	
};


//�����ɹ�
struct DBO_GR_CreateSuccess
{
	DWORD							dwUserID;							//�û���ʶ
	LONG							lCellScore;							//���õ׷�
	LONGLONG						lDiamond;							//��ǰ��ʯ
	DWORD							dwTableID;							//Ԥ��������
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	WORD							wJoinGamePeopleCount;				//������Ϸ������
	DWORD							dwRoomTax;							//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	BYTE							cbIsJoinGame;						//����Ƿ������Ϸ
	BYTE							cbPassword;							//�Ƿ���������
	TCHAR							szClientAddr[16];					//���������IP��ַ
	BYTE							cbGameRule[RULE_LEN];				//��Ϸ���� �� 0 λ��ʶ �Ƿ����ù��� 0 �������� 1 ����δ����
	//����λ��Ϸ�Զ������ ����Э������Ϸ����˺Ϳͻ����̶�
	INT								iRoomCardFee;						//��������	
	BYTE							cbPayMode;							//֧����ʽ 0 ����֧���� 1 AA��֧��
	BYTE							cbGameMode;							//��Ϸģʽ
	BYTE							cbPlayMode;							//0 ����ģʽ  1 ���ģʽ
	LONGLONG						lEnterScore;						//���뷿���������
	LONGLONG						lLeaveScore;						//�뿪�����������
	BYTE							cbJoinMode;							//�Ƿ�����İ���˲���	0 ����	1 ������
	DWORD							dwGroupID;							//������id�� �Ǵ�����ģʽ �� 0
	BYTE							cbGroupTaxMode;						//����ģʽ
	DWORD							dwGroupTaxCount;					//���������ֵ
	DWORD							dwConfigID;							//�淨ID
	TCHAR							szConfigName[13];					//�淨����
	BYTE							cbLessScoreDismiss;					//����������ɢ
};


//����ʧ��
struct DBO_GR_CreateFailure
{
	DWORD							dwTableID;							//Ԥ��������
	LONG							lErrorCode;							//�������
	bool							bGroupPersonalMaxChair;				//�����˰�����Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGroupID;							//Ⱥ��ID
	DWORD							dwConfigID;							//�淨ID
	TCHAR							szDescribeString[128];				//������Ϣ
};

//ȡ������
struct DBO_GR_CancelCreateResult
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwTableID;							//���� I D
	DWORD							dwReason;							//ȡ��ԭ��

	LONG							lErrorCode;							//�������	
	TCHAR							szDescribeString[128];				//������Ϣ							
};


//��ɢ���
struct DBO_GR_DissumeResult
{
	DWORD					dwSocketID;							//�����ʶ
	DWORD					dwUserID;							//���ID
	BYTE					cbIsDissumSuccess;					//�Ƿ��ɢ�ɹ�
	DWORD					dwPersonalRoomID;					//Լս����ID			
	SYSTEMTIME				sysDissumeTime;						//��ɢʱ��	
	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];
};

//��ɢ���
struct DBO_GR_CurrenceRoomCardAndBeans
{
//	DOUBLE					dbBeans;							//�����ʶ
	SCORE					lDiamond;							//��ʯ
	DWORD					dwTableID;
};

//֧�����
struct DBO_GR_CurrenceGroupFee
{
	DWORD					dwGroupID;							//����ȦID
	SCORE					lFeeDiamond;						//������ʯ
	SCORE					lDiamond;							//ʣ����ʯ
};

//˽����ҷ�������Ϣ
struct DBR_GR_WriteJoinInfo
{
	DWORD							dwUserID;											//�û���ʶ
	WORD							wTableID;											//���� I D
	DWORD							dwRoomHostID;										//����ID
	WORD							wChairID;											//����ID
	WORD							wKindID;											//����ID
	DWORD							dwPersonalRoomID;									//Լս����ID
	TCHAR							szPeronalRoomGUID[PERSONAL_ROOM_GUID];				//����Ψһ��ʶ
};

//֧������
struct DBR_GR_PersonalPayRoomCard
{
	byte							cbPayMode;							//֧����ʽ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwPlayTurnCount; 					//˽�˷Ž�����Ϸ��������
	DWORD							dwPlayTimeLimit;					 //˽�˷�������Ϸ�����ʱ�� ��λ��
	DWORD							dwKindID;							//��Ϸ��������ʶ
	DWORD							dwAddr;								//�ͻ��˵�ַ
	DWORD							dwPersonalRoomID;					//Լս����ID
	DWORD							dwGroupID;							//���ֲ�ID
};

//¼�������Ϣ
struct DBR_GR_WriteVideoPlayer
{
	DWORD							dwUserID;							//�û���ʶ
	CHAR							szVideoID[22];						//¼���ʶ
	SCORE							lScore;								//��Ӯ����
	WORD							wChairID;							//����ID
	WORD							wKindID;							//����ID
};

//¼����Ϣ
struct DBR_GR_WriteVideoData
{
	CHAR							szVideoID[22];						//¼���ʶ	
	WORD							wServerID;
	WORD							wTableID;	
	BYTE							cbVideoData[10000];					//¼������	
	WORD							wVideoSize;							//ʵ�ʳ���
};

//���������ӽ���
struct DBR_GR_GroupTableEnd
{
	DWORD							dwGroupID;
	DWORD							dwRoomID;
	BYTE							cbPlayMode;
	//TCHAR							szGameName[32];
	WORD							wKindID;
	TCHAR							szServerName[32];
	LONGLONG						lMinEnterScore;
	WORD							wDrawCount;
	DWORD							dwTableOwnerID;
	DWORD							dwBigwinnerID;
	DWORD							dwUserID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];
	SCORE							lScore[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];
	SCORE							lRevenue[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];
	
	SYSTEMTIME						sysStartTime;						//��ʼʱ��
	SYSTEMTIME						sysEndTime;							//����ʱ��
};

//���ع�������
struct DBO_GR_LoadShareStorageItemResult
{
	LONG							lResultCode;								//�������
	WORD							wShareStorageItemCount;						//������������Ŀ
	tagShareStorageItem				ShareStorageItem[MAX_SHARESTORAGEITEM];		//����������
};

//���ص���Ⱥ�鹲����
struct DBR_GR_LoadSingleGroupShareStorageItem
{
	WORD							wKindID;									//��Ϸ��ʶ
	DWORD							dwGroupID;									//���ֲ�ID
};

//////////////////////////////////////////////////////////////////////////////////

#endif