#ifndef CMD_CORRESPOND_HEAD_FILE
#define CMD_CORRESPOND_HEAD_FILE

#pragma pack(1)

//ƽ̨����
#include "Platform.h"

//////////////////////////////////////////////////////////////////////////////////
//ע������

#define MDM_CS_REGISTER				1									//����ע��

//����ע��
#define SUB_CS_C_REGISTER_PLAZA		100									//ע��㳡
#define SUB_CS_C_REGISTER_SERVER	101									//ע�᷿��
#define SUB_CS_C_REGISTER_MATCH		102									//ע�����
#define SUB_CS_C_REGISTER_CHAT		103									//ע������	
#define SUB_CS_C_REGISTER_PERSONAL		104								//ע��Լս��������	
#define SUB_CS_C_REGISTER_GROUP		105									//ע����ֲ�
#define SUB_CS_C_REGISTER_AI		106									//AIע��

//ע����
#define SUB_CS_S_REGISTER_FAILURE	200									//ע��ʧ��
#define SUB_CS_S_REGISTER_SUCCESS	201									//ע��ɹ�
//////////////////////////////////////////////////////////////////////////////////

//ע��㳡
struct CMD_CS_C_RegisterPlaza
{
	TCHAR							szServerAddr[32];					//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//ע����Ϸ
struct CMD_CS_C_RegisterServer
{
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wServerID;							//��������
	WORD                            wServerKind;                        //�������
	WORD                            wServerType;                        //��������		
	WORD							wServerLevel;						//����ȼ�
	WORD							wServerPort;						//����˿�
	SCORE							lCellScore;							//��Ԫ����
	SCORE							lEnterScore;						//�������
	BYTE							cbEnterMember;						//�����Ա
	DWORD							dwOnLineCount;						//��������
	DWORD							dwFullCount;						//��Ա����
	WORD							wTableCount;						//������Ŀ
	DWORD							dwServerRule;						//�������
	TCHAR							szServerAddr[32];					//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
	DWORD							dwSetPlayerCount;					//��������
	TCHAR							szGameInfomation[LEN_SERVER];		//��Ϸ˵��
	BYTE							cbPlayMode;							//0 ����ģʽ	1	���ģʽ
	bool							bGroupPersonalMaxChair;				//�����˰�����Ϸ
};

//ע��ʧ��
struct CMD_CS_S_RegisterFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//ע�����
struct CMD_CS_C_RegisterMatch
{
	//������Ϣ
	WORD							wServerID;							//�����ʶ
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ	
	LONGLONG						lMatchNo;							//��������
	BYTE							cbMatchStatus;						//����״̬
	BYTE							cbMatchType;						//��������
	WCHAR							szMatchName[32];					//��������

	//������Ϣ
	BYTE							cbFeeType;							//��������
	BYTE							cbDeductArea;						//�ɷ�����
	LONGLONG						lSignupFee;							//��������	
	BYTE							cbSignupMode;						//������ʽ
	BYTE							cbJoinCondition;					//��������
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	DWORD							dwExperience;						//��Ҿ���
	DWORD							dwFromMatchID;						//������ʶ		
	BYTE							cbFilterType;						//ɸѡ��ʽ
	WORD							wMaxRankID;							//�������
	SYSTEMTIME						MatchEndDate;						//��������
	SYSTEMTIME						MatchStartDate;						//��ʼ����	

	//������ʽ
	BYTE							cbRankingMode;						//������ʽ	
	WORD							wCountInnings;						//ͳ�ƾ���
	BYTE							cbFilterGradesMode;					//ɸѡ��ʽ

	//��������
	BYTE							cbDistributeRule;					//�������
	WORD							wMinDistributeUser;					//��������
	WORD							wDistributeTimeSpace;				//������	
	WORD							wMinPartakeGameUser;				//��������
	WORD							wMaxPartakeGameUser;				//�������

	//��������
	BYTE							cbMatchRule[512];					//��������

	//������Ϣ
	WORD							wRewardCount;						//��������
	tagMatchRewardInfo				MatchRewardInfo[3];					//��������
};

//Լս��������������
struct CMD_CS_S_RegisterPersonal
{
	DWORD dwKindID;
	BYTE  cbPlayMode;
	DWORD dwMaxCreate;
	bool  bSupportGroup;
};


//ע����ֲ�
struct CMD_CS_C_RegisterGroup
{
	TCHAR							szServerAddr[32];					//�����ַ	
	WORD							wServicePort;						//�˿ڵ�ַ
};
//////////////////////////////////////////////////////////////////////////////////
//������Ϣ

#define MDM_CS_SERVICE_INFO			2									//������Ϣ

//������Ϣ
#define SUB_CS_C_SERVER_ONLINE		1									//��������
#define SUB_CS_C_SERVER_MODIFY		2									//�����޸�
#define SUB_CS_C_SHARE_STORAGE		3									//������
#define SUB_CS_C_GAMESERVER_WEB_SHARE_STORAGE	4						//������

//�㳡����
#define SUB_CS_S_PLAZA_INFO			100									//�㳡��Ϣ
#define SUB_CS_S_PLAZA_INSERT		101									//�㳡�б�
#define SUB_CS_S_PLAZA_REMOVE		103									//�㳡ɾ��
#define SUB_CS_S_PLAZA_FINISH		104									//�㳡���

//��������
#define SUB_CS_S_SERVER_INFO		110									//������Ϣ
#define SUB_CS_S_SERVER_ONLINE		111									//��������
#define SUB_CS_S_SERVER_INSERT		112									//�����б�
#define SUB_CS_S_SERVER_MODIFY		113									//�����޸�
#define SUB_CS_S_SERVER_REMOVE		114									//����ɾ��
#define SUB_CS_S_SERVER_FINISH		115									//�������
#define SUB_CS_S_SHARE_STORAGE		116									//������
#define SUB_CS_S_WEB_SHARE_STORAGE	117									//������

//��������
#define SUB_CS_S_MATCH_INSERT		120									//�����б�
#define SUB_CS_S_MATCH_OPTION       121									//��������
#define SUB_CS_S_SIGNUP_COUNT		122									//��������

//��������
#define SUB_CS_S_CHAT_INSERT		130									//�������
#define SUB_SS_S_ONLINE				140									//�û�����
#define SUB_SS_S_OFFLINE			141									//�û�����
#define SUB_SS_S_STATUS				142									//�û�״̬
#define SUB_SS_S_GAMESTATUS			143									//��Ϸ״̬
#define SUB_SS_S_SYNCHRFRIEND		144									//ͬ������	
#define SUB_SS_S_SYNCHARFINISH		145									//ͬ�����
#define SUB_SS_S_MODIFYFRIEND		146									//�޸ĺ���
#define SUB_SS_S_REMOVEGROUP		147									//�Ƴ�����

//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_CS_C_ServerModify
{
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wServerPort;						//����˿�
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//AI��
	DWORD							dwFullCount;						//��Ա����
	DWORD							dwSetCount;							//��������
	TCHAR							szServerAddr[32];					//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR							szGameInfomation[LEN_SERVER];		//��Ϸ˵��
};

//��������
struct CMD_CS_C_ServerOnLine
{
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//AI��
	DWORD							dwSetCount;							//��������
};

//������
struct CMD_CS_C_ShareStorage
{
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;
	tagShareStorageItem				ShareStorageItem;
};

//��������
struct CMD_CS_C_MatchSignUpCount
{
	LONGLONG						lMatchNo;							//�������				
	DWORD							dwMatchCount;						//��������
	WORD							wServerID;							//�����ʶ
};
//////////////////////////////////////////////////////////////////////////////////

//�㳡ɾ��
struct CMD_CS_S_PlazaRemove
{
	DWORD							dwPlazaID;							//�㳡��ʶ
};

//��������
struct CMD_CS_S_ServerOnLine
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//AI��
	DWORD							dwSetCount;							//��������
};

//�����޸�
struct CMD_CS_S_ServerModify
{
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wServerID;							//�����ʶ
	WORD							wServerPort;						//����˿�
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//AI��
	DWORD							dwFullCount;						//��Ա����
	TCHAR							szServerAddr[32];					//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
	DWORD							dwSetPlayerCount;					//��������
	TCHAR							szGameInfomation[LEN_SERVER];		//��Ϸ˵��

};

//������
struct CMD_CS_S_ShareStorage
{
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;
	tagShareStorageItem				ShareStorageItem;
};

//������
struct CMD_CS_S_WebShareStorage
{
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;
	tagShareStorageItem				ShareStorageItem;
};

//����ɾ��
struct CMD_CS_S_ServerRemove
{
	WORD							wServerID;							//�����ʶ
};

//////////////////////////////////////////////////////////////////////////////////
//�û�����

#define MDM_CS_USER_COLLECT			3									//�û�����

//�û�״̬
#define SUB_CS_C_USER_ENTER			1									//�û�����
#define SUB_CS_C_USER_LEAVE			2									//�û��뿪
#define SUB_CS_C_USER_FINISH		3									//�û����
#define SUB_CS_C_USER_STATUS		4									//�û�״̬

//�û�״̬
#define SUB_CS_S_COLLECT_REQUEST	100									//��������

#define SUB_CS_S_USER_GAMESTATE		101									//�û�״̬

#define SUB_CS_S_PERSONAL_INFO			102									//Լս��Ϣ
#define SUB_CS_S_PERSONAL_INFO_UPDATE	103									//����
#define SUB_CS_S_DELETE_PERSONAL		104									//ɾ��Լս����
//////////////////////////////////////////////////////////////////////////////////

//�û�����
struct CMD_CS_C_UserEnter
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�

	//������Ϣ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	//��ϸ��Ϣ
	tagUserInfo						userInfo;							//�û���Ϣ
};

//�û��뿪
struct CMD_CS_C_UserLeave
{
	DWORD							dwUserID;							//�û���ʶ
};

//�û�״̬
struct CMD_CS_C_UserStatus
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbUserStatus;						//�û�״̬
	WORD							wKindID;							//��Ϸ��ʶ
	WORD							wServerID;							//�����ʶ
	WORD							wTableID;							//��������
	WORD							wChairID;							//����λ��
};

//�û�״̬
struct CMD_CS_S_UserGameStatus
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbGameStatus;						//�û�״̬
	WORD							wKindID;							//��Ϸ��ʶ
	WORD							wServerID;							//�����ʶ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wChairID;							//����λ��
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//////////////////////////////////////////////////////////////////////////////////
//Զ�̷���

#define MDM_CS_REMOTE_SERVICE		4									//Զ�̷���

//���ҷ���
#define SUB_CS_C_SEARCH_DATABASE	1									//���ݲ���
#define SUB_CS_C_SEARCH_CORRESPOND	2									//Э������
#define SUB_CS_C_SEARCH_ALLCORRESPOND	3								//Э������

//���ҽ��
#define SUB_CS_S_SEARCH_DATABASE	100									//���ݲ���
#define SUB_CS_S_SEARCH_CORRESPOND	101									//Э������
#define SUB_CS_S_SEARCH_ALLCORRESPOND	102								//Э������

//////////////////////////////////////////////////////////////////////////////////

//Э������
struct CMD_CS_C_SearchCorrespond
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//��������
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
};

//Э������
struct CMD_CS_S_SearchCorrespond
{
	//������Ϣ
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//������Ϣ
	WORD							wUserCount;							//�û���Ŀ
	tagUserRemoteInfo				UserRemoteInfo[16];					//�û���Ϣ
};


//Э������
struct CMD_CS_C_AllSearchCorrespond
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	DWORD							dwCount;
	DWORD							dwGameID[1];						//��Ϸ��ʶ
};

//Э������
struct CMD_CS_S_SearchAllCorrespond
{
	//������Ϣ
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//������Ϣ
	DWORD							dwCount;
	tagUserRemoteInfo				UserRemoteInfo[1];
};


//////////////////////////////////////////////////////////////////////////////////
//�������

#define MDM_CS_MANAGER_SERVICE		5									//�������

#define SUB_CS_C_SYSTEM_MESSAGE 	1									//ϵͳ��Ϣ
#define SUB_CS_C_PROPERTY_TRUMPET 	2									//������Ϣ

#define SUB_CS_S_SYSTEM_MESSAGE 	100									//ϵͳ��Ϣ
#define SUB_CS_S_PROPERTY_TRUMPET  	200									//������Ϣ
#define SUB_CS_S_PLATFORM_PARAMETER 300									//ƽ̨����	


//��������
struct CMD_CS_S_SendTrumpet
{
	WORD                           wPropertyIndex;                      //�������� 
	DWORD                          dwSendUserID;                         //�û� I D
	DWORD                          TrumpetColor;                        //������ɫ
	TCHAR                          szSendNickName[32];				    //����ǳ� 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //��������
};
//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_CS_ANDROID_SERVICE	    6									//��������

#define SUB_CS_C_ADDPARAMETER		100									//��Ӳ���
#define SUB_CS_C_MODIFYPARAMETER    101									//�޸Ĳ���
#define SUB_CS_C_DELETEPARAMETER	102									//ɾ������	

#define SUB_CS_S_ADDPARAMETER		200									//��Ӳ���
#define SUB_CS_S_MODIFYPARAMETER    201									//�޸Ĳ���
#define SUB_CS_S_DELETEPARAMETER	202									//ɾ������	

//////////////////////////////////////////////////////////////////////////////////
//��Ӳ���
struct CMD_CS_C_AddParameter
{
	WORD							wServerID;							//�����ʶ
	tagAndroidParameter				AndroidParameter;					//��������
};

//�޸Ĳ���
struct CMD_CS_C_ModifyParameter
{
	WORD							wServerID;							//�����ʶ
	tagAndroidParameter				AndroidParameter;					//��������
};

//ɾ������
struct CMD_CS_C_DeleteParameter
{
	WORD							wServerID;							//�����ʶ
};


//��Ӳ���
struct CMD_CS_S_AddParameter
{	
	tagAndroidParameter				AndroidParameter;					//��������
};

//�޸Ĳ���
struct CMD_CS_S_ModifyParameter
{
	tagAndroidParameter				AndroidParameter;					//��������
};

//ɾ������
struct CMD_CS_S_DeleteParameter
{
	WORD							wServerID;							//�����ʶ
};

//++++++++++++++++++++++++++++++Լս����
//˽�˷�������
#define SUB_CS_C_SEARCH_TABLE					131						//��������
#define SUB_CS_S_SEARCH_TABLE_RESULT			132						//���ҽ��
#define SUB_CS_C_QUERY_GAME_SERVER				133						//���ҷ���
#define SUB_CS_S_QUERY_GAME_SERVER_RESULT		134						//���ҽ��
#define SUB_CS_C_CREATE_TABLE					135						//��������
#define SUB_CS_S_CREATE_TABLE_RESULT			136						//�������
#define SUB_CS_C_DISMISS_TABLE					137						//��ɢ����
#define SUB_CS_C_DISMISS_TABLE_RESULT			138						//��ɢ����
#define SUB_CS_S_INSERT_CREATE_RECORD			139						//���봴����¼
#define SUB_CS_S_QUERY_PERSONAL_ROOM_LIST		140						//����˽�˷����б�
#define SUB_CS_C_QUERY_PERSONAL_ROOM_LIST_RESULT	141					//����˽�˷����� 
#define SUB_CS_C_DISSUME_SEARCH_TABLE			142						//�����ɢ˽�˷��� 
#define SUB_CS_S_DISSUME_SEARCH_TABLE_RESULT	143						//���ҽ��
#define SUB_CS_S_QUERY_ROOM_PASSWORD			144						//���ҷ�������
#define SUB_CS_S_QUERY_ROOM_PASSWORD_RESULT		145						//���ҷ���������
#define SUB_CS_C_UPDATE_TABLE_COUNT				146						//��������
#define SUB_CS_C_QUERY_GOLD_ROOM_LIST			147						//���Լս�����б�
#define SUB_CS_C_QUERY_GOLD_ROOM_LIST_RESULT	148						//���Լս�����б���
#define SUB_CS_C_QUERY_GOLD_ROOM_LIST_FINISH	149						//���Լս�����б���
#define SUB_CS_C_CURRENCE_GROUP_FEE				150						//����Ȧ��������
#define SUB_CS_C_UPDATE_BANKER_USERID			151						//���´����˰�����Ϸׯ��USERID

//���ֲ����
#define MDM_CS_CLUB_SERVICE_INFO				8						//���ֲ�������Ϣ

#define SUB_CS_C_QUERY_CLUB_ROOM_LIST			200						//���ֲ�����
#define SUB_CS_C_QUERY_CLUB_ROOM_LIST_RESULT	201						//���ֲ�������
#define SUB_CS_C_QUERY_CLUB_ROOM_LIST_FINISH	202						//���ֲ��������
#define SUB_CS_C_CLUB_USER_STATUS				203						//���ֲ��û�״̬�仯
#define SUB_CS_C_CLUB_CREATE_TABLE				204						//���ֲ����䴴��
#define SUB_CS_C_CLUB_DISMISS_TABLE				205						//���ֲ������ɢ
#define SUB_CS_C_CLUB_CURRENCE_FEE				206						//��������
#define SUB_CS_C_CLUB_CREATE_RULE				207						//���ֲ��淨����
#define SUB_CS_C_CLUB_DISMISS_RULE				208						//���ֲ��淨ɾ��
#define SUB_CS_C_UPDATE_GROUP_SCORE				209						//��������Ȧ��
#define SUB_CS_C_CHECK_NEW_USER					210						//��������
#define SUB_CS_C_QUERY_ROOM_BATTLE_INFO			211						//��ѯ���ӶԾ���Ϣ
#define SUB_CS_C_QUERY_ROOM_BATTLE_INFO_RES		212						//��ѯ���ӶԾ���Ϣ���
#define SUB_CS_S_CLUB_SERVER_CLOSE				213						//�������ر�
#define SUB_CS_C_CLUB_DELETE_CONFIG				214						//���ֲ��淨ɾ
#define SUB_CS_C_CLUB_TABLE_CHANGEBANKER		215						//���ֲ�����ׯ��
#define SUB_CS_C_CREATE_GROUP_SUCCESS			216						//���ֲ������ɹ�

//��ȡ˽�˷����б�
struct CMD_MB_QeuryPersonalRoomList
{
	DWORD							dwUserID;							//�û�ID
};

struct CMD_MB_SC_QeuryPersonalRoomList
{
	DWORD							dwUserID;							//�û�ID
	DWORD							dwSocketID;						//����ID
};


//������󷿼�ɼ�
struct CMD_GR_QUERY_USER_ROOM_SCORE
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwKindID;								//��������
};

//++++++++++++++++++++++++++++++Լս����
//��������
struct CMD_CS_C_SearchTable
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwKindID;							//��Ϸ���ͱ�ʶ
	DWORD							dwPersonalRoomID;					//Լս����ID	
};

//���ҽ��
struct CMD_CS_S_SearchTableResult
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	DWORD							dwServerID;							//����I D
	DWORD							dwTableID;							//����I D
	DWORD							dwKindID;							//��������
	DWORD							dwGroupID;							//���ֲ�ID
};

//���ҷ���������
struct CMD_CS_S_QueryRoomPasswordResult
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	DWORD							dwRoomPassword;						//��������	
};

//��ѯ����
struct CMD_CS_C_QueryGameServer
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwKindID;							//��Ϸ��ʶ
	BYTE							cbIsJoinGame;						//�Ƿ������Ϸ
	BYTE							cbPlayMode;							//0 ����ģʽ 1 ���ģʽ
	DWORD							dwGroupID;							//����ȦID
	DWORD							dwConfigID;							//�淨ID

	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//��ѯ���
struct CMD_CS_S_QueryGameServerResult
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//������Ϣ
	DWORD							dwServerID;							//��ѯ���
	bool							bCanCreateRoom;						//�Ƿ���Դ�������

	//��������
	TCHAR							szErrDescrybe[MAX_PATH];			//��������
};

//��������
struct CMD_CS_C_CreateTable
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szClientAddr[16];					//���������IP��ַ
	BYTE							cbPayMode;							//֧��ģʽ
	BYTE							cbGameMode;							//��Ϸģʽ
	LONG							lNeedRoomCard;						//������֧����������	
	//������Ϣ
	tagPersonalTableInfo			PersonalTable;						//������Ϣ
};

//�������
struct CMD_CS_S_CreateTableResult
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szClientAddr[16];					//���������IP��ַ
	BYTE							cbPayMode;							//֧��ģʽ
	BYTE							cbGameMode;							//��Ϸģʽ
	LONG							lNeedRoomCard;						//������֧����������
	//������Ϣ
	tagPersonalTableInfo			PersonalTable;						//������Ϣ
	////������Ϣ
	//DWORD							dwTableID;							//����I D						
	//TCHAR							szServerID[7];						//������
};
//��������
struct CMD_CS_C_UpdateTablePlayerCount
{
	DWORD							dwSocketID;							//����ID
	WORD							wServerID;							//����I D
	WORD							wTableID;							//����I D
	WORD							wSitCount;							//��������
	DWORD							dwUserID;							//�����û�
	DWORD							wChairID;							//����λ��
	bool							bSit;								//��������
	bool							bGameStart;							//��Ϸ��ʼ

	CMD_CS_C_UpdateTablePlayerCount()
	{
		dwSocketID	= -1;
		wServerID	= 0;
		wTableID	= 0;
		wSitCount	= 0;
		dwUserID = INVALID_DWORD;
		wChairID = INVALID_CHAIR;
		bSit = false;
		bGameStart = false;
	}
};

//�������˰�����Ϸׯ��USERID
struct CMD_CS_C_UpdateGroupMaxChairBankerUserID
{
	WORD							wServerID;							//����I D
	WORD							wTableID;							//����I D
	WORD							wBankerChairID;						//ׯ��λ��
	DWORD							dwBankerUserID;						//ׯ��USERID

	CMD_CS_C_UpdateGroupMaxChairBankerUserID()
	{
		wServerID = 0;
		wTableID = 0;
		wBankerChairID = INVALID_CHAIR;
		dwBankerUserID = 0;
	}
};

//��ɢ����
struct CMD_CS_C_DismissTable
{
	DWORD							dwSocketID;						//����ID
	DWORD							dwServerID;							//����I D
	DWORD							dwTableID;							//����I D
	CMD_CS_C_DismissTable()
	{
		dwSocketID = -1;
	}
};

//����ǿ�ƽ�ɢ��Ϸ
struct CMD_GR_HostDissumeGame
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwTableID;							//���� I D
};

struct CMD_CS_C_DismissTableResult
{
	DWORD								dwSocketID;									//����ID
	tagPersonalTableInfo				PersonalTableInfo;							
	tagPersonalUserScoreInfo			PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];
};

//˽�˷����б�
struct CMD_CS_C_HostCreatRoomInfo
{
	DWORD								wSocketID;							//�����ʶ
	int									nCreatRoomCount;					//������Ŀ
	tagHostCreatRoomInfo				HostCreatRoomInfo;					//������Ϣ
};

struct CMD_CS_C_GoldPersonalRoomInfo
{
	DWORD							dwRoomID;							//����I D
	DWORD							dwKindID;							//��������
	DWORD							dwServerID;							//����I D
	DWORD							dwTableID;							//����I D
	SCORE							lCellScore;							//����׷�
	DWORD							dwRoomTax;							//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	WORD							wPlayerCount;						//������Ϸ�������
	WORD							wPlayingUser;						//������Ϸ������
	WORD							wBeginFreeTime;						//���ʱ�ο�ʼ
	WORD							wEndFreeTime;						//���ʱ�ν���
	SCORE							lEnterScoreLimit;					//���뷿������
	SCORE							lLeaveScoreLimit;					//�뿪��������
	BYTE							cbGameRule[RULE_LEN];				//��Ϸ���� �� 0 λ��ʶ �Ƿ����ù��� 0 �������� 1 ����δ����
	TCHAR							szCreateTime[32];					//����ʱ��
};

//����Ȧ��������
struct CMD_GR_CurrenceGroupFee
{
	DWORD					dwGroupID;							//����ȦID
	SCORE					lFeeDiamond;						//������ʯ
	SCORE					lDiamond;							//ʣ����ʯ
};

//���Ӵ�����¼
struct DBR_GR_InsertCreateRecord
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwServerID;							//�����ʶ
	LONGLONG						lCellScore;							//���õ׷�
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	DWORD							dwPersonalRoomID;					//Լս����ID
	TCHAR							szPassword[LEN_PASSWORD];			//��������
	DWORD							dwRoomTax;							//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	WORD							wJoinGamePeopleCount;				//������Ϸ������
	TCHAR							szClientAddr[16];					//���������IP��ַ
	BYTE							cbPayMode;							//֧��ģʽ
	LONG							lNeedRoomCard;						//��������Ҫ���ĵķ���
	BYTE							cbGameMode;							//��Ϸģʽ
	BYTE							cbPlayMode;							//0 ���ֳ� 1 ��ҳ�
	LONGLONG						lGroupID;							//���ֲ�ID
	LONGLONG						lMinEnterScore;
};

//�����ҷ����б�
struct CMD_CS_C_QueryGoldPersonalRoomList
{
	DWORD							dwSocketID;						//�����ʶ
	WORD							wKindID;						//��Ϸ����					
	BYTE							cbPlayMode;						//0 ���ֳ� 1 ��ҳ�
	DWORD							dwGroupID;						//���ֲ�id�� �Ǿ��ֲ�ģʽ �� 0
};

//�����ҷ����б�
struct CMD_CS_C_QueryGoldPersonalRoomListResult
{
	DWORD							dwSocketID;						//�����ʶ	
};
//////////////////////////////////////////////////////////////////////////////////
//���ֲ����

//��ѯ����
struct CMD_CS_C_QueryClubRoom
{
	DWORD							dwSocketID;						//�����ʶ	
	DWORD							dwGroupID;						//���ֲ�ID
};

//��ѯ���
struct CMD_CS_S_QueryResult
{
	DWORD							dwSocketID;						//�����ʶ
	DWORD							dwGroupID;						//���ֲ�ID
	WORD							wTableCount;					//��ѯ��Ŀ
};
//��ѯ���
struct CMD_CS_S_QueryFinish
{
	DWORD							dwSocketID;						//�����ʶ	
};

//����״̬�仯
struct CMD_CS_S_UpdateTable
{
	DWORD                           dwKindID;							//����ID
	DWORD							dwServerID;							//����I D
	WORD							wTableID;							//����I D
	DWORD							dwPersonalRoomID;					//Լս����ID
	DWORD							dwGroupID;							//���ֲ�ID
	DWORD							dwUserID;							//�����û�
	DWORD							wChairID;							//����λ��
	bool							bSit;								//��������
	bool							bGameStart;							//��Ϸ��ʼ
};

//����ׯ�ұ仯
struct CMD_CS_S_TableChangeBanker
{
	DWORD                           dwKindID;							//����ID
	DWORD							dwServerID;							//����I D
	WORD							wTableID;							//����I D
	DWORD							dwPersonalRoomID;					//Լս����ID
	DWORD							dwGroupID;							//���ֲ�ID
	DWORD							wBankerChairID;						//ׯ��λ��
	DWORD							dwBankerUserID;						//������Ϸׯ��USERID(��ЧΪ0)
};

//��������
struct CMD_CS_S_DismissClubTable
{
	DWORD                           dwKindID;							//����ID
	DWORD							dwServerID;							//����I D
	WORD							wTableID;							//����I D
	DWORD							dwPersonalRoomID;					//Լս����ID
	DWORD							dwGroupID;							//���ֲ�ID
	bool							bDeleteServer;						//trueɾ������server����

};

//Ȧ�ѷ����仯
struct CMD_CS_S_UpdateClubScore
{
	DWORD							dwServerID;							//����I D
	WORD							wTableID;							//����I D
	DWORD							dwPersonalRoomID;					//Լս����ID
	DWORD							dwGroupID;							//���ֲ�ID
	DWORD							dwUserID;							//�����û�
	DWORD							wChairID;							//����λ��
	SCORE							lScore;								//����
};

//��������
struct CMD_CS_S_CheckNewUser
{
	DWORD							dwUserID;							//���I D
	TCHAR							szClientIP[15];						//IP
};

//��ѯ���Ӷ�ս��Ϣ
struct CMD_CS_C_QueryTableInfo
{
	DWORD							dwSocketID;							//�����ʶ	
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwServerID;							//������ID
	DWORD							dwTableID;							//����ID
};

//��ѯ���Ӷ�ս��Ϣ���
struct CMD_CS_S_QueryTableInfoResult
{
	DWORD							dwSocketID;							//�����ʶ	
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwRoomID;							//������ID
	DWORD							dwAllCount;							//�ܾ���
	DWORD							dwPlayCount;						//��ǰ����
	DWORD							dwUserID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//���ID
	SCORE							lGameScore[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];//��Ϸ�÷�

};

//���ֲ��淨ɾ��
struct CMD_CS_C_ClubDeleteConfig
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGroupID;							//Ⱥ��ID
	DWORD							dwConfigID;							//�淨ID
};

//���ֲ������ɹ�
struct CMD_CS_C_CreateGroupSuccess
{
	DWORD							dwGroupID;							//���ֲ�ID
};

//////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++++++��վ����
#define MDM_CS_WEB_SERVICE						9							//��վ����

//��������
#define SUB_CS_C_UPDATE_PARAMTER				101						//��������
#define SUB_CS_C_UPDATE_USER_SCORE				102						//���²Ƹ�
#define SUB_CS_C_UPDATE_MAIL					103						//�����ʼ�
#define SUB_CS_C_WEB_SHARE_STORAGE				106						//������

//��վ���� AI ���
#define SUB_CS_C_UPDATE_AI_PARAMTER				201						//����AI��������
#define SUB_CS_C_UPDATE_AI_WEIGHT_OUT			202						//����AI�뿪Ȩ������
#define SUB_CS_C_UPDATE_AI_WEIGHT_IN			203						//����AI���뷿������
#define SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL			204						//����AI����Ȩ��
#define SUB_CS_C_DELETE_AI						205						//ɾ��AI

//������վ
#define SUB_CS_S_UPDATE_SUCCESS					111						//���³ɹ�
#define SUB_CS_S_UPDATE_FAILURE					112						//����ʧ��

//ת������
#define SUB_CS_S_UPDATE_PARAMTER				101						//��������
#define SUB_CS_S_UPDATE_USER_SCORE				102						//���²Ƹ�
#define SUB_CS_S_UPDATE_MAIL					103						//�����ʼ�
#define SUB_CS_S_GROUP_MAX_MEMBER				104						//Ⱥ���������

//AI�������
#define SUB_SW_S_AI_OPERATE_RESULT				105						//AI �������

//
#define SUB_CS_S_WEB_SHARE_STORAGE_RESULT		113						//��������

//��������
struct CMD_CS_C_UpdateParamter
{
	WORD							wUpdateType;						//��������
};

//���·���
struct CMD_CS_C_UpdateUserScore
{
	DWORD							dwUserID;							//�û� I D
};

//�����ʼ�
struct CMD_CS_C_UpdateMail
{
	WORD							wUserCount;							//Ŀ������
};

//����Ⱥ���������
struct CMD_CS_C_GroupMaxMember
{
	DWORD							dwGroupID;							//Ⱥ��ID��0�Ƕ�����Ⱥ
	WORD							wMaxCount;
};

//������
struct CMD_CS_C_WebShareStorage
{
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;				//������״̬
	tagShareStorageItem				ShareStorageItem;					//����������
};

//������
struct CMD_CS_C_GameServerWebShareStorage
{
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;				//������״̬
	tagShareStorageItem				ShareStorageItem;					//����������
	TCHAR							szResultDescripe[512];				//�������
};

//AI ��������
struct CMD_CS_C_UpdateAIParameter
{
	BYTE							cbStatus;					//0 ɾ�� 1 ���� 2 �޸�
	DWORD							dwGroupID;					//���ֲ�ID
	SCORE							lDistirbuteTotalScore;		//�����ܷ���
	DWORD							dwAICount;					//AI����
	WORD							wKindID;					//��Ϸ��ʶ
	DWORD							dwPlayingAIRateMin;			//��Ϸ�е�AI
	DWORD							dwPlayingAIRateMax;			//�ȴ��е�AI
	SCORE							lDistirbuteSingleScore;		//����AI�������
	SCORE							lDistirbuteScoreMinRatio;	//����AI���������������
	SCORE							lDistirbuteScoreMaxRatio;	//����AI���������������
};


//AI ��������
struct CMD_CS_C_UpdateAIParameterToAI
{
	DWORD dwSocketID;											//�����ʶ
	BYTE							cbStatus;					//0 ɾ�� 1 ���� 2 �޸�
	DWORD							dwGroupID;					//���ֲ�ID
	SCORE							lDistirbuteTotalScore;		//�����ܷ���
	DWORD							dwAICount;					//AI����
	WORD							wKindID;					//��Ϸ��ʶ
	DWORD							dwPlayingAIRateMin;			//��Ϸ�е�AI
	DWORD							dwPlayingAIRateMax;			//�ȴ��е�AI
	SCORE							lDistirbuteSingleScore;		//����AI�������
	SCORE							lDistirbuteScoreMinRatio;	//����AI���������������
	SCORE							lDistirbuteScoreMaxRatio;	//����AI���������������
};



//AI �뿪Ȩ�޸���
struct CMD_CS_C_UpdateAIWeightOut
{
	int nIndex;
	tagWeightOutRatio WeightOutRatio;
};

//AI �뿪Ȩ�޸���
struct CMD_CS_C_UpdateAIWeightOutToAI
{
	DWORD dwSocketID;						//�����ʶ
	CMD_CS_C_UpdateAIWeightOut UpdateAIWeightOut;
};


//AI ���뷿��Ȩ�޸���
struct CMD_CS_C_UpdateAIWeightIn
{
	int nIndex;
	tagWeightInRatio WeightInRatio;
};

//AI ���뷿��Ȩ�޸���
struct CMD_CS_C_UpdateAIWeightInToAI
{
	
	DWORD dwSocketID;						//�����ʶ
	CMD_CS_C_UpdateAIWeightIn UpdateAIWeightIn;
};

//��ͬʱ��� AI ����
struct CMD_CS_C_UpdateAITimeTotal
{
	int nIndex;
	DWORD dwTime;
	DWORD dwTimeChange;
};

//��ͬʱ��� AI ����
struct CMD_CS_C_UpdateAITimeTotalToAI
{
	DWORD dwSocketID;						//�����ʶ
	CMD_CS_C_UpdateAITimeTotal UpdateAITimeTotal;
};


//�������
struct CMD_WS_S_AIOperateResult
{
	DWORD							dwSocketID;						//�����ʶ
	WORD							wOperateID;							//������ʶ
	BYTE							cbResult;							//�������
	TCHAR							szDescripe[260];					//��¼����
};

//��������
struct CMD_CS_S_WebShareStorageResult
{
	bool							bGameServerRegister;				//ע���ʶ
	ENUM_SHARESTORAGE_STATUS		emShareConfigStatus;				//������״̬
	tagShareStorageItem				ShareStorageItem;					//����������
	TCHAR							szResultDescripe[512];				//�������
};

struct tagDeleteAI
{
	WORD  wKindID;
	DWORD dwUserID; //һ�����ɾ��20��AI
	byte  cbIsAI;	//0 ��ʵ��� 1 ������
};

//ɾ�����ֲ��е����
struct CMD_CS_C_DeleteAI
{
	byte  cbOperate;	//0 ���� 1 �˳����ֲ�
	DWORD dwGroupID;
	tagDeleteAI DeleteAI[20];
};

//��ͬʱ��� AI ����
struct CMD_CS_C_DeleteAIParameterToAI
{
	DWORD dwSocketID;						//�����ʶ
	CMD_CS_C_DeleteAI DeleteAI;				//ɾ��AI
};

#pragma pack()

#endif