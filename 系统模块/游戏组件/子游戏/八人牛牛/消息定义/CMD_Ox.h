#ifndef CMD_OXSIXX_HEAD_FILE
#define CMD_OXSIXX_HEAD_FILE

#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��
#define KIND_ID							57									//��Ϸ I D
#define GAME_PLAYER						8									//��Ϸ����
#define GAME_NAME						TEXT("����ţţ")					//��Ϸ����
#define MAX_CARDCOUNT					5									//�����Ŀ
#define MAX_CONFIG						5									//������ø���
#define MAX_RECORD_COUNT				30									//��������ͳ��������
#define MAX_CARD_TYPE					19									//�������
#define MAX_SPECIAL_CARD_TYPE			9									//����������� �������廨ţ��˳��ͬ����«ը��ͬ��˳��Сţ ���޴�С��
#define MAX_BANKERMODE					6									//���ׯ��ģʽ ���ΰ���ׯ��������ׯ��ţţ��ׯ����ţ��ׯ��������ׯ��ͨ���淨
#define MAX_GAMEMODE					2									//�����Ϸģʽ ���ξ���ģʽ�����ģʽ
#define MAX_BEBANKERCON					4									//�����ׯ����	Ĭ���ޣ�100��150��200
#define MAX_USERBETTIMES				4									//����м���ע����	Ĭ���ޣ�5��10��20
#define MAX_ADVANCECONFIG				5									//���߼�ѡ��  ���η��ĵ��壬 ��Ҫ���룬 ��ʼ��ֹ���룬 ��ֹ���ƣ��������ߴ���
#define MAX_POOL_SIZE					54									//�Ƴ���󳤶�
#define MAX_TABLECTRL_TYPE				4									//���������������

#define VERSION_SERVER					PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT					PROCESS_VERSION(7,0,1)				//����汾

//��Ϸ״̬
#define GS_TK_FREE						GAME_STATUS_FREE					//�ȴ���ʼ
#define GS_TK_CALL						GAME_STATUS_PLAY					//��ׯ״̬
#define GS_TK_SCORE						GAME_STATUS_PLAY+1					//��ע״̬
#define GS_TK_PLAYING					GAME_STATUS_PLAY+2					//��Ϸ����
#define GS_TK_WAITEND					GAME_STATUS_PLAY+3					//����״̬

//������Ϣ
#define IDM_ADMIN_UPDATE_STORAGE		WM_USER+1001
#define IDM_ADMIN_MODIFY_STORAGE		WM_USER+1011
#define IDM_REQUEST_QUERY_USER			WM_USER+1012
#define IDM_USER_DEBUG				WM_USER+1013
#define IDM_REQUEST_UPDATE_ROOMINFO		WM_USER+1014
#define IDM_CLEAR_CURRENT_QUERYUSER		WM_USER+1015

//������¼
#define MAX_OPERATION_RECORD			20									//������¼����
#define RECORD_LENGTH					128									//ÿ����¼�ֳ�

#define INVALID_LONGLONG				((LONGLONG)(0xFFFFFFFF))			//��Ч��ֵ

//��Ϸ��¼
struct CMD_S_RECORD
{
	int									nCount;
	LONGLONG							lUserWinCount[GAME_PLAYER];						//���ʤ������
	LONGLONG							lUserLostCount[GAME_PLAYER];					//���ʧ�ܴ���
};

//-------------------------------------------
//��Ϸ����
typedef enum
{
	CT_CLASSIC_ = 22,						//����ģʽ
	CT_ADDTIMES_ = 23,						//���ӱ�
	CT_INVALID_ = 255,						//��Ч
}CARDTYPE_CONFIG;

//����ģʽ
typedef enum
{
	ST_SENDFOUR_ = 32,						//���ĵ���
	ST_BETFIRST_ = 33,						//��ע����
	ST_INVALID_ = 255,						//��Ч
}SENDCARDTYPE_CONFIG;

//�˿��淨
typedef enum
{
	GT_HAVEKING_ = 42,						//�д�С��
	GT_NOKING_ = 43,						//�޴�С��
	GT_INVALID_ = 255,						//��Ч
}KING_CONFIG;

//ׯ���淨
typedef enum
{
	BGT_DESPOT_ = 52,						//����ׯ
	BGT_ROB_ = 53,							//������ׯ
	BGT_NIUNIU_ = 54,						//ţţ��ׯ
	BGT_NONIUNIU_ = 55,						//��ţ��ׯ

	BGT_FREEBANKER_ = 56,					//������ׯ
	BGT_TONGBI_ = 57,						//ͨ���淨

	BGT_INVALID_ = 255,						//��Ч
}BANERGAMETYPE_CONFIG;

//��ע����
typedef enum
{
	BT_FREE_ = 62,							//�������ö��
	BT_PENCENT_ = 63,						//�ٷֱ����ö��
	BT_INVALID_ = 255,						//��Ч
}BETTYPE_CONFIG;

//��ע����(ֻ�ڻ��ַ�����Ч)
typedef enum
{
	BT_TUI_NONE_ = 72,						//����ע
	BT_TUI_DOUBLE_ = 73,					//������ע(�Ͼ�Ӯ���ܶ�ķ���Ϊ������ע�Ķ��)
	BT_TUI_INVALID_ = 255,					//��Ч
}TUITYPE_CONFIG;

//-------------------------------------------

typedef enum
{
	PROGRESSINGEX = 0,
	CANCELEX,				//δִ�й��ڶ�����ȡ��
	EXECUTE_CANCELEX,		//ִ�й�ȡ��
}EM_SYSCTRL_STATUS;

typedef enum
{
	QUEUE = 0,
	PROGRESSING,
	FINISH,
	CANCEL,				//δִ�й��ڶ�����ȡ��
	EXECUTE_CANCEL,		//ִ�й�ȡ��
}EM_ROOMCTRL_STATUS;

typedef enum
{
	TABLECTRL_PROGRESSING = 0,
	TABLECTRL_FINISH,
	TABLECTRL_CANCEL,				//δִ�й��ڶ�����ȡ��
	TABLECTRL_EXECUTE_CANCEL,		//ִ�й�ȡ��
}EM_TABLECTRL_STATUS;

typedef enum
{
	CLIENT_CARDCONFIG_CTRL,
	TABLE_CTRL,
	ROOM_CTRL,
	SYS_CTRL
}EM_CTRL_TYPE;

//������������
//��ǰ���Ӯ�䣬ͬ��ׯ��Ӯ��
typedef enum{ CURPLAYER_WIN = 0, CURPLAYER_LOST = 1, BANKER_WIN = 2, BANKER_LOST = 3, TABLECTRL_INVALID = INVALID_DWORD }TABLECTRL_TYPE;

//�û���Ϊ
typedef enum{ USER_SITDOWN = 11, USER_STANDUP, USER_OFFLINE, USER_RECONNECT, USER_NULL }USERACTION;

//�����û���Ϣ
typedef struct
{
	WORD								wChairID;							//����ID
	WORD								wTableID;							//����ID
	DWORD								dwGameID;							//GAMEID
	DWORD								dwUserID;							//USERID
	WORD								wBankerChairID;						//ׯ������ID(ͨ���淨��ЧΪINVALID_CHAIR)
	WORD								wCurTablePlayerCount;				//��������(����AI)
	bool								bAndroid;							//AI��ʶ
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE								cbUserStatus;						//�û�״̬
	BYTE								cbGameStatus;						//��Ϸ״̬;						
	TABLECTRL_TYPE						tablectrl_type;						//��������(��ӦcbCtrlPosIndex,��Χ0~3)
	BYTE								cbConfigCtrlTimes;					//���õ��Ծ���
	BYTE								cbCurCtrlTimes;						//��ǰ���Ծ���
}ROOMUSERINFO;

//ϵͳ����
typedef struct
{
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	SCORE							lSysCtrlConfigSysStorage;			//����ϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE							lSysCtrlConfigPlayerStorage;		//������ҿ��(���ҪӮ��Ǯ)
	SCORE							lSysCtrlCurSysStorage;				//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE							lSysCtrlCurPlayerStorage;			//��ǰ��ҿ��(���ҪӮ��Ǯ)
	SCORE							lSysCtrlParameterK;					//��ǰ����ϵ��(�ٷֱ�):
	SCORE							lSysCtrlSysStorResetRate;			//ϵͳ����ϵͳ������ñ���
	SCORE							lSysCtrlResetCount;					//������ô���
	SCORE							lSysCtrlSysWin;						//ͨ�ÿ���ϵͳ����Ӯ
	CTime							tmResetTime;						//����ʱ��
	EM_SYSCTRL_STATUS				sysCtrlStatus;						//��ǰ״̬
}SYSCTRL;

//�������
typedef struct
{
	//������Բ���
	DWORD							dwCtrlIndex;						//��������(��1��ʼ����)
	SCORE						    lRoomCtrlConfigSysStorage;			//���÷������ϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE						    lRoomCtrlConfigPlayerStorage;		//���÷��������ҿ��(ϵͳҪӮ��Ǯ)
	SCORE						    lRoomCtrlCurSysStorage;				//��ǰ�������ϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE						    lRoomCtrlCurPlayerStorage;			//��ǰ���������ҿ��(ϵͳҪӮ��Ǯ)
	SCORE							lRoomCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
	SCORE							lRoomCtrlSysStorResetRate;			//�������ϵͳ������ñ���
	SCORE							lRoomCtrlSysWin;					//�������ϵͳ����Ӯ
	CTime							tmConfigTime;						//����ʱ��
	EM_ROOMCTRL_STATUS				roomCtrlStatus;						//��ǰ״̬
	DWORD							dwDebugUserGameID;					//�������GameID
}ROOMCTRL;

//��������
typedef struct
{
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	ROOMUSERINFO					roomUserInfo;						//������Ϣ
	SCORE							lTableCtrlSysWin;					//��������ϵͳ����Ӯ
	CTime							tmConfigTime;						//����ʱ��
	EM_TABLECTRL_STATUS				tableCtrlStatus;					//��ǰ״̬
	TABLECTRL_TYPE					tablectrl_type;						//��������
	DWORD							dwDebugUserGameID;					//�������GameID
}TABLECTRL;

//////////////////////////////////////////////////////////////////////////
//����������ṹ
#define SUB_S_GAME_START				100									//��Ϸ��ʼ
#define SUB_S_ADD_SCORE					101									//��ע���
#define SUB_S_PLAYER_EXIT				102									//�û�ǿ��
#define SUB_S_SEND_CARD					103									//������Ϣ
#define SUB_S_GAME_END					104									//��Ϸ����
#define SUB_S_OPEN_CARD					105									//�û�̯��
#define SUB_S_CALL_BANKER				106									//�û���ׯ
#define SUB_S_CALL_BANKERINFO			107									//�û���ׯ��Ϣ
#define SUB_S_ADMIN_STORAGE_INFO		112									//ˢ�µ��Է����
#define SUB_S_RECORD					113									//��Ϸ��¼
#define SUB_S_ROOMCARD_RECORD			114									//������¼

#define SUB_S_SEND_FOUR_CARD			120
#define SUB_S_ANDROID_BANKOPER			122									//AI�洢�����
#define SUB_S_ANDROID_READY				124									//AI׼��
#define SUB_S_GAMEFREE_NOTIFY			129									//����֪ͨ
#define SUB_S_ANDROID_USEROVERTIME		130									//������ʱ
#define SUB_S_ANDROID_NOTIFYWIN			131									//����ʤ������
#define SUB_S_VIDEO_START				132									//¼��ʼ
//�����������
#define SUB_S_CONFIG_DEBUG				125									//�������
#define SUB_S_GET_POOL_CARD				126									//�Ƴ�ʣ������
#define SUB_S_REQUEST_CARDTYPE			127									//��������
#define SUB_S_ERROR_NOTIFY				128									//������ʾ


#define SUB_S_GAME_STAUTS				1000								//��Ϸ״̬��Ϣ

//���ƶ����
#define SUB_S_STATISTIC_SERVICE				13									//ͳ�Ƽ������������
#define SUB_S_ANCHOU						14									//����

#define SUB_S_CUR_SYSCTRL_INFO				21									//ˢ�����ý��
#define SUB_S_CUR_ROOMCTRL_INFO				22									//��ǰ���������Ϣ
#define SUB_S_LIST_CUR_ROOMCTRL_INFO		23									//��ǰ�����б���Ϣ
#define SUB_S_CUR_TABLECTRL_INFO			24									//��ǰ����������Ϣ
#define SUB_S_LIST_CUR_TABLECTRL_INFO		25									//��ǰ�����б���Ϣ
#define SUB_S_QUERY_GAMEID					26									//��ѯGAMEID
#define SUB_S_REFRESH_SYSCTRL_TRACELOG		31									//����ϵͳ�����б�
#define SUB_S_REFRESH_ROOMCTRL_TRACELOG		32									//���·�������б�
#define SUB_S_REFRESH_TABLECTRL_TRACELOG	33									//�������������б�
#define SUB_S_STARTRECORD_TRACELOG			34									//��ʼ��¼ʱ��
#define SUB_S_DEBUGTEXT_GENERAL				35									//������ʾ��Ϣ(GENERAL)
#define SUB_S_DEBUGTEXT_ADVANCED			36									//������ʾ��Ϣ(ADVANCED)

#define GAME_START			 1
#define GAME_NOTE			 2
#define GAME_SEND_LAST_CARD  3
#define GAME_END			 4

struct tagGameStaus
{
	byte cbStatus; //1 ��ʼ 2 ��ע 3 �����һ���� 4 ����
};

//////////////////////////////////////////////////////////////////////////////////////
//
struct stConfigDebug
{
	WORD			wDebugUserChair;					//�������
	BYTE			cbType;								//��������	1-���Խ�� 2-��������
	BYTE			cbGameWin;							//���Խ��	1-���� 2-��Ӯ
	BYTE			cbGameCount;						//���Ծ���	//...	
	BYTE			cbGameCards[MAX_CARDCOUNT];			//��������	//...
	//
	WORD			wConfigUserChair;					//�������
};
struct CMD_S_ConfigDebug
{
	//
	stConfigDebug	stConfig;							//��������
	//
	BYTE			cbCurDebugCount;					//��ǰ���Ծ���
	BYTE			cCards[MAX_CARDCOUNT];				//����
	bool			bValidDebugType[MAX_CARD_TYPE];		//�ɵ�������,0-��ţ 1~10 ţ1~ţţ  11-�Ļ� 12-�廨 13-˳�� 14-ͬ�� 15-��« 16-ը�� 17-ͬ��˳ 18-��Сţ
	BYTE			cbCurCardType;						//��ǰ����
};
struct CMD_S_PoolCard
{
	bool			bCards[MAX_POOL_SIZE];				//0~53��
};
struct CMD_S_RequestCardType
{
	BYTE			cbGameCards[MAX_CARDCOUNT];			//��������	//...
};
struct CMD_S_ErrorNotify
{
	TCHAR			tErrorInfo[100];						//������ʾ
};
//////////////////////////////////////////////////////////////////////////////////////
//Ԥ��AI���ȡ��
struct tagCustomAndroid
{
	SCORE								lRobotScoreMin;	
	SCORE								lRobotScoreMax;
	SCORE	                            lRobotBankGet; 
	SCORE								lRobotBankGetBanker; 
	SCORE								lRobotBankStoMul; 
};

//////////////////////////////////////////////////////////////////////////////////////

//��Ϸ״̬
struct CMD_S_StatusFree
{
	LONGLONG							lCellScore;							//��������
	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//AI����

	bool								bIsAllowAvertDebug;					//�����Ա�־
	
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	BYTE								cbAdmitRevCard;						//������� TRUEΪ����

	BYTE								cbPlayMode;							//Լս��Ϸģʽ
	WORD								wCardTypeTimes[MAX_CARD_TYPE];		//���ͱ���

	int									nTimeStart;							//��ʼʱ��
	int									nTimeCallBanker;					//��ׯʱ��
	int									nTimeAddScore;						//��עʱ��
	int									nTimeOpenCard;						//̯��ʱ��
};

//��ׯ״̬
struct CMD_S_StatusCall
{
	LONGLONG							lCellScore;							//��������
	BYTE                                cbDynamicJoin;                      //��̬���� 
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//AI����

	bool								bIsAllowAvertDebug;					//�����Ա�־
	
	BYTE								cbCallBankerStatus[GAME_PLAYER];	//��ׯ״̬
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//��ׯ����
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨 (������ׯĬ�϶���1��)
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	BYTE								cbAdmitRevCard;						//������� TRUEΪ����
	BYTE								cbMaxCallBankerTimes;				//�����ׯ����
	WORD								wBgtRobNewTurnChairID;				//������ׯ�¿�һ����ׯ����ң���ЧΪINVALID_CHAIR�� ����Чʱ��ֻ��������ѡ���������Ҹ������ׯ�ң�

	BYTE								cbPlayMode;							//Լս��Ϸģʽ
	BYTE								cbTimeRemain;						//����ʣ������
	WORD								wCardTypeTimes[MAX_CARD_TYPE];		//���ͱ���

	int									nTimeStart;							//��ʼʱ��
	int									nTimeCallBanker;					//��ׯʱ��
	int									nTimeAddScore;						//��עʱ��
	int									nTimeOpenCard;						//̯��ʱ��
};

//��ע״̬
struct CMD_S_StatusScore
{
	LONGLONG							lCellScore;							//��������
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬
	BYTE                                cbDynamicJoin;                      //��̬����
	LONGLONG							lTurnMaxScore;						//�����ע
	LONGLONG							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	WORD								wBankerUser;						//ׯ���û�

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//AI����

	bool								bIsAllowAvertDebug;					//�����Ա�־

	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//��ׯ����
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	LONG								lFreeConfig[MAX_CONFIG];			//�������ö��(��Чֵ0)
	LONG								lPercentConfig[MAX_CONFIG];			//�ٷֱ����ö��(��Чֵ0)
	LONG								lPlayerBetBtEx[GAME_PLAYER];		//�мҶ������ע����

	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	BYTE								cbAdmitRevCard;						//������� TRUEΪ����

	BYTE								cbPlayMode;							//Լս��Ϸģʽ
	BYTE								cbTimeRemain;						//����ʣ������
	WORD								wCardTypeTimes[MAX_CARD_TYPE];		//���ͱ���

	int									nTimeStart;							//��ʼʱ��
	int									nTimeCallBanker;					//��ׯʱ��
	int									nTimeAddScore;						//��עʱ��
	int									nTimeOpenCard;						//̯��ʱ��
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	LONGLONG							lCellScore;							//��������
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬
	BYTE                                cbDynamicJoin;                      //��̬����
	LONGLONG							lTurnMaxScore;						//�����ע
	LONGLONG							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	WORD								wBankerUser;						//ׯ���û�

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//�˿���Ϣ
	BYTE								cbHandCardData[GAME_PLAYER][MAX_CARDCOUNT];//�����˿�
	bool								bOpenCard[GAME_PLAYER];				//���Ʊ�ʶ
	bool								bSpecialCard[GAME_PLAYER];			//�������ͱ�־ ���Ļ�ţ���廨ţ��˳�ӣ�ͬ������«��ը����ͬ��˳����Сţ��
	BYTE								cbOriginalCardType[GAME_PLAYER];	//���ԭʼ���ͣ�û�о��������ϵ����ͣ�
	BYTE								cbCombineCardType[GAME_PLAYER];     //���������ͣ����������ϵ����ͣ�

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//AI����
	bool								bIsAllowAvertDebug;					//�����Ա�־
	
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//��ׯ����
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	LONG								lFreeConfig[MAX_CONFIG];			//�������ö��(��Чֵ0)
	LONG								lPercentConfig[MAX_CONFIG];			//�ٷֱ����ö��(��Чֵ0)
	LONG								lPlayerBetBtEx[GAME_PLAYER];		//�мҶ������ע����

	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	BYTE								cbAdmitRevCard;						//������� TRUEΪ����

	BYTE								cbPlayMode;							//Լս��Ϸģʽ
	BYTE								cbTimeRemain;						//����ʣ������

	bool								bDelayFreeDynamicJoin;
	WORD								wCardTypeTimes[MAX_CARD_TYPE];		//���ͱ���

	int									nTimeStart;							//��ʼʱ��
	int									nTimeCallBanker;					//��ׯʱ��
	int									nTimeAddScore;						//��עʱ��
	int									nTimeOpenCard;						//̯��ʱ��
};

//�û���ׯ��Ϣ
struct CMD_S_CallBankerInfo
{
	BYTE								cbCallBankerStatus[GAME_PLAYER];	//��ׯ״̬
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//��ׯ����(���û�����ׯ����ֵ0)������ׯĬ�϶���1��
	WORD								wChairID;							//��ׯ���Ӻ�
};

//��4����
struct CMD_S_SendFourCard
{
	//��ǰ��4��
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	WORD								wBankerUser;						//ׯ���û�
	BYTE								cbPlayerStatus[GAME_PLAYER];		//���״̬
	LONGLONG							lTurnMaxScore;						//�����ע

	//(����ģʽ���Ϊ���ĵ��壬�������ƣ� ����ȫΪ0)
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	LONG								lFreeConfig[MAX_CONFIG];			//�������ö��(��Чֵ0)
	LONG								lPercentConfig[MAX_CONFIG];			//�ٷֱ����ö��(��Чֵ0)
	LONG								lPlayerBetBtEx[GAME_PLAYER];		//�мҶ������ע����
};

//�û���ע
struct CMD_S_AddScore
{
	WORD								wAddScoreUser;						//��ע�û�
	LONGLONG							lAddScoreCount;						//��ע��Ŀ
};

//��Ϸ����
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax[GAME_PLAYER];				//��Ϸ˰��
	LONGLONG							lGameScore[GAME_PLAYER];			//��Ϸ�÷�
	BYTE								cbHandCardData[GAME_PLAYER][MAX_CARDCOUNT];//�����˿�
	BYTE								cbCardType[GAME_PLAYER];			//�������
	WORD								wCardTypeTimes[GAME_PLAYER];		//���ͱ���
	BYTE								cbDelayOverGame;
	BYTE								cbLastSingleCardData[GAME_PLAYER];	//ԭʼ���һ����
};

//�������ݰ�
struct CMD_S_SendCard
{
	//(��ȫ��5���ƣ��������ģʽ�Ƿ��ĵ��壬��ǰ�����ź�CMD_S_GameStart��Ϣһ��) ͨ���淨�յ�����ϢҪĬ���µ�ע
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�
	bool								bSpecialCard[GAME_PLAYER];				//�������ͱ�־ ���Ļ�ţ���廨ţ��˳�ӣ�ͬ������«��ը����ͬ��˳����Сţ��
	BYTE								cbOriginalCardType[GAME_PLAYER];		//���ԭʼ���ͣ�û�о��������ϵ����ͣ�
};

//�û��˳�
struct CMD_S_PlayerExit
{
	WORD								wPlayerID;							//�˳��û�
};

//�û�̯��
struct CMD_S_Open_Card
{
	WORD								wOpenChairID;						//̯���û�
	BYTE								bOpenCard;							//̯�Ʊ�־
};

//¼������
struct Viedeo_GameConfig
{
	WORD								wCardTypeTimes[MAX_CARD_TYPE];		//���ͱ���
};
struct Video_GameStart
{
	LONGLONG							lCellScore;							//��������
	WORD								wPlayerCount;						//��ʵ��������
	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	WORD								wBankerUser;						//ׯ���û�
	BYTE								cbPlayerStatus[GAME_PLAYER];		//���״̬
	LONGLONG							lTurnMaxScore;						//�����ע

	//(����ģʽ���Ϊ���ĵ��壬�������ƣ� ����ȫΪ0)
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	DWORD								lFreeConfig[MAX_CONFIG];			//�������ö��(��Чֵ0)
	DWORD								lPercentConfig[MAX_CONFIG];			//�ٷֱ����ö��(��Чֵ0)
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�		
	WORD								wChairID;							//����ID
	LONGLONG							lScore;								//����
};

//������¼
struct CMD_S_RoomCardRecord
{
	WORD							nCount;											//����
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//���ֽ����
	BYTE                            cbPlayStatus[GAME_PLAYER];						//�û�״̬
};

////////���ڻط�
//�û���ׯ
struct CMD_S_CallBanker
{
	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BYTE								cbMaxCallBankerTimes;				//�����ׯ����������ׯĬ�϶���1��
	WORD								wBgtRobNewTurnChairID;				//������ׯ�¿�һ����ׯ����ң���ЧΪINVALID_CHAIR�� ����Чʱ��ֻ��������ѡ���������Ҹ������ׯ�ң�
};

//���ƶ����
//////////////////////////////////////////////////////////////////////////////////

//ͳ�Ƽ������������
struct CMD_S_StatisticService
{
	SCORE							lStatisticSysWin;						//ͳ��(����ͨ�ÿ��,����,����������ϵͳ��Ӯ�Ͱ����)
	SCORE							lStatisticService;						//�����������
};

//����
struct CMD_S_AnChou
{
	SCORE							lCurAnChouRate;							//�������
	SCORE							lCurAnChouTotal;						//��������
};

//��ǰϵͳ������Ϣ
struct CMD_S_CurSysCtrlInfo
{
	SCORE							lSysCtrlCurSysStorage;					//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE							lSysCtrlCurPlayerStorage;				//��ǰ��ҿ��(���ҪӮ��Ǯ)
	SCORE							lSysCtrlParameterK;						//��ǰ����ϵ��(�ٷֱ�):
	SCORE							lSysCtrlSysStorResetRate;				//ϵͳ������ñ���
	SCORE							lSysCtrlResetCount;						//���ô���
	SCORE							lSysCtrlSysWin;							//ϵͳ��Ӯ����
};

//��ǰ���������Ϣ
struct CMD_S_CurRoomCtrlInfo
{
	DWORD							dwCtrlIndex;						//��������
	SCORE					        lRoomCtrlCurSysStorage;				//�������ϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE						    lRoomCtrlCurPlayerStorage;			//���������ҿ��(ϵͳҪӮ��Ǯ)
	SCORE							lRoomCtrlParameterK;				//������Ե���ϵ��(�ٷֱ�):
	SCORE							lRoomCtrlSysStorResetRate;			//�������ϵͳ������ñ���
	SCORE							lRoomCtrlSysWin;					//ϵͳ����Ӯ
};

//��ǰ�����б���Ϣ
struct CMD_S_ListCurRoomCtrlInfo
{
	ROOMCTRL						curRoomCtrlListInfo;
};

//��ǰ����������Ϣ
struct CMD_S_CurTableCtrlInfo
{
	BYTE							cbCtrlPosIndex;						//����λ��(0~3)
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	WORD							wTableID;							//����
	WORD							wBankerChairID;						//ׯ�����Ӻ�
	WORD							wCurTablePlayerCount;				//��������
	BYTE							cbConfigCtrlTimes;					//���õ��Ծ���
	BYTE							cbCurCtrlTimes;						//��ǰ���Ծ���
	SCORE							lTableCtrlSysWin;					//ϵͳ����Ӯ
};

//��ǰ�����б���Ϣ
struct CMD_S_ListCurTableCtrlInfo
{
	TABLECTRL						curTableCtrlListInfo;
};

//��ѯGAMEID
struct CMD_S_QueryGameID
{
	ROOMUSERINFO					QueryRoomUserInfo;					//��ǰ��ѯ�����Ϣ
	BANERGAMETYPE_CONFIG			bgtConfig;
};

//ϵͳ����TraceLog
struct CMD_S_SysCtrlTraceLog
{
	SYSCTRL							SysCtrlTraceLog;
};

//�������TraceLog
struct CMD_S_RoomCtrlTraceLog
{
	ROOMCTRL						RoomCtrlTraceLog;
};

//��������TraceLog
struct CMD_S_TableCtrlTraceLog
{
	TABLECTRL						TableCtrlTraceLog;
};

//��ʼ��¼ʱ��
struct CMD_S_StartRecordTraceLog
{
	CTime							tmStartRecord;						//��ʼ��¼ʱ��
};

//������ʾ
struct CMD_S_DebugText
{
	TCHAR							szMessageText[256];					//�����ı�
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_CALL_BANKER				1									//�û���ׯ
#define SUB_C_ADD_SCORE					2									//�û���ע
#define SUB_C_OPEN_CARD					3									//�û�̯��
#define SUB_C_REQUEST_RCRecord			12									//��ѯ������¼

//�����������
#define SUB_C_CONFIG_DEBUG				17									//�������
#define SUB_C_GET_POOL_CARD				18									//��ȡʣ��������
#define SUB_C_CONFIG_CANCEL				19									//ȡ������
#define SUB_C_REQUEST_CARDTYPE			20									//��������
#define SUB_C_REQUEST_CURCONFIG			21									//����ǰ����

//���ƶ����
#define SUB_C_ANCHOU							11							//���ð���
#define SUB_C_SYS_CTRL							12							//����ϵͳ����
#define SUB_C_REFRESH_CUR_SYSCTRL_INFO			13							//ˢ��ϵͳ������Ϣ
#define SUB_C_ROOM_CTRL							14							//���÷������
#define SUB_C_REFRESH_CUR_ROOMCTRL_INFO			15							//ˢ�·��������Ϣ
#define SUB_C_REFRESH_LIST_CUR_ROOMCTRL_INFO	16							//ˢ�µ�ǰ�����б���Ϣ
#define SUB_C_QUERY_GAMEID						17							//��ѯGAMEID
#define SUB_C_TABLE_CTRL						18							//������������
#define SUB_C_REFRESH_CUR_TABLECTRL_INFO		19							//ˢ������������Ϣ
#define SUB_C_REFRESH_LIST_CUR_TABLECTRL_INFO	20							//ˢ�µ�ǰ���������б���Ϣ
#define SUB_C_REFRESH_STATISTIC_SERVICE			21							//ˢ��ͳ��
#define SUB_C_REFRESH_ANCHOU					22							//ˢ�°���
#define SUB_C_REFRESH_TRACELOG					23							//ˢ����־��¼

struct CMD_C_ConfigDebug
{
	stConfigDebug		stConfig;		//����
};
struct CMD_C_RequestCardType
{
	WORD		wDebugUserChair;
	BYTE		cbCardType;
};

//�û���ׯ
struct CMD_C_CallBanker
{
	bool								bBanker;							//��ׯ��־
	BYTE								cbBankerTimes;						//��ׯ����(���û�����ׯ����ֵ0)
};

//�û���ע
struct CMD_C_AddScore
{
	LONGLONG							lScore;								//��ע��Ŀ
};

//�û�̯��
struct CMD_C_OpenCard
{	
	//������ϵ��ƣ���ǰ�����ܷ�ճ�10���
	BYTE								cbCombineCardData[MAX_CARDCOUNT];	//�������˿�
};

//���ð���
struct CMD_C_AnChou
{
	SCORE							lConfigAnChouRate;						//�������
};

//ϵͳ����
struct CMD_C_SysCtrl
{
	SCORE							lSysCtrlSysStorage;							//����ϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE							lSysCtrlPlayerStorage;						//������ҿ��(���ҪӮ��Ǯ)
	SCORE							lSysCtrlParameterK;							//���õ���ϵ��(�ٷֱ�)
	SCORE							lSysCtrlSysStorResetRate;					//ϵͳ������ñ���
};

//�������
struct CMD_C_RoomCtrl
{
	DWORD							dwSelCtrlIndex;							//ѡ���������(����ʱΪINVALID_DWORD)
	SCORE						    lRoomCtrlCurSysStorage;					//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE						    lRoomCtrlCurPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
	SCORE							lRoomCtrlParameterK;					//���˷�����Ե���ϵ��(�ٷֱ�)
	SCORE							lRoomCtrlSysStorResetRate;				//ϵͳ������ñ���
	bool							bCancelRoomCtrl;						//ȡ�����Ա�ʶ(trueΪȡ����falseΪ���õ���)
};

//��ѯGAMEID
struct CMD_C_QueryGameID
{
	DWORD							dwQueryGameID;							//��ѯ���GAMEID
	DWORD							dwQueryUserID;							//��ѯ���USERID
};

//��������
struct CMD_C_TableCtrl
{
	DWORD							dwCtrlGameID;							//�������GAMEID
	DWORD							dwCtrlUserID;							//�������USERID
	BYTE							dwCtrlIndex;							//��������
	BYTE							cbCtrlPosIndex;							//����λ��(0~3)
	BYTE							cbConfigCtrlTimes;						//���õ��Ծ���
	bool							bCancelTableCtrl;						//ȡ�����Ա�ʶ(trueΪȡ����falseΪ���õ���)
};


#pragma pack(pop)

#endif
