#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//#define CARD_DISPATCHER_DEBUG
//������

//��Ϸ����
#define KIND_ID						241										//��Ϸ I D
#define GAME_NAME					TEXT("21 ��")							//��Ϸ����

//�������
#define GAME_PLAYER					6										//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)					//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)					//����汾

//////////////////////////////////////////////////////////////////////////////////

//����ԭ��
#define GER_NO_PLAYER				0x10									//û�����
#define LEN_NICKNAME				32										//�ǳƳ���

//״̬����
#define GAME_SCENE_FREE				GAME_STATUS_FREE						//�ȴ���ʼ
#define GAME_SCENE_ADD_SCORE		GAME_STATUS_PLAY						//�м���ע
#define GAME_SCENE_GET_CARD			(GAME_STATUS_PLAY+1)					//ׯ�Ҳ���

//������¼
#define MAX_OPERATION_RECORD			20									//������¼����
#define RECORD_LENGTH					128									//ÿ����¼�ֳ�

#define MAX_RECORD_COUNT				32									//��������ͳ��������

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START				100									//��Ϸ��ʼ
#define SUB_S_GAME_END					101									//��Ϸ����
#define SUB_S_SEND_CARD					102									//����
#define SUB_S_SPLIT_CARD				103									//����
#define SUB_S_STOP_CARD					104									//ͣ��
#define SUB_S_DOUBLE_SCORE				105									//�ӱ�
#define SUB_S_INSURE					106									//����
#define SUB_S_ADD_SCORE					107									//��ע
#define SUB_S_GET_CARD					108									//Ҫ��
#define SUB_S_CHEAT_CARD				109									//��������
#define SUB_S_RECORD					118									//��Ϸ��¼

struct CMD_S_RECORD
{
	int						nCount[GAME_PLAYER];
	SCORE					lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//���ֽ����
	SCORE					lAllScore[GAME_PLAYER];							//�ܽ����
};

//��ϷAI���ȡ��
struct tagCustomAndroid
{
	LONGLONG							lRobotScoreMin;	
	LONGLONG							lRobotScoreMax;
	LONGLONG	                        lRobotBankGet; 
	LONGLONG							lRobotBankGetBanker; 
	LONGLONG							lRobotBankStoMul; 
};

//��������
typedef enum{CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL}DEBUG_TYPE;

//���Խ��      ���Գɹ� ������ʧ�� ������ȡ���ɹ� ������ȡ����Ч
typedef enum{DEBUG_SUCCEED, DEBUG_FAIL, DEBUG_CANCEL_SUCCEED, DEBUG_CANCEL_INVALID}DEBUG_RESULT;

//�û���Ϊ
typedef enum{USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT}USERACTION;



struct CMD_S_StatusFree
{
	//ʱ�䶨��	
	BYTE								cbInitTimeAddScore;					//��עʱ��
	BYTE								cbInitTimeGetCard;					//����ʱ��

	BYTE								cbBankerMode;						//ׯ��ģʽ
	SCORE								lBaseJeton;							//��Ԫ����
	WORD								wPlayerCount;						//��Ϸ����

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���
	tagCustomAndroid					CustomAndroid;						//��ϷAI����
	BYTE								cbPlayMode;							//Լս��Ϸģʽ
};

//��Ϸ״̬
struct CMD_S_StatusAddScore
{
	//ʱ�䶨��	
	BYTE								cbInitTimeAddScore;					//��עʱ��
	BYTE								cbInitTimeGetCard;					//����ʱ��
	BYTE								cbTimeRemain;						//����ʱ��

	BYTE								cbBankerMode;						//ׯ��ģʽ
	SCORE								lBaseJeton;							//��Ԫ����
	WORD								wPlayerCount;						//��Ϸ����

	BYTE								cbPlayStatus[GAME_PLAYER];			//���״̬

	LONGLONG							lMaxScore;							//���ע��

	WORD								wBankerUser;						//ׯ��

	LONGLONG							lTableScore[GAME_PLAYER];			//������ע
	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���
	tagCustomAndroid					CustomAndroid;						//��ϷAI����
	BYTE								cbPlayMode;							//Լս��Ϸģʽ

};

//��Ϸ״̬
struct CMD_S_StatusGetCard
{
	//ʱ�䶨��	
	BYTE								cbInitTimeAddScore;					//��עʱ��
	BYTE								cbInitTimeGetCard;					//����ʱ��
	BYTE								cbTimeRemain;						//����ʱ��

	BYTE								cbBankerMode;						//ׯ��ģʽ
	SCORE								lBaseJeton;							//��Ԫ����
	WORD								wPlayerCount;						//��Ϸ����

	BYTE								cbPlayStatus[GAME_PLAYER];			//���״̬

	WORD								wBankerUser;						//ׯ��
	WORD								wCurrentUser;						//��ǰ���

	SCORE								lTableScore[GAME_PLAYER];			//������ע

	//�˿���Ϣ
	BYTE								cbCardCount[GAME_PLAYER*2];			//�˿���Ŀ
	BYTE								cbHandCardData[GAME_PLAYER*2][11];	//�����˿�

	//
	bool								bStopCard[GAME_PLAYER*2];			//���ͣ��
	bool								bDoubleCard[GAME_PLAYER*2];			//��Ҽӱ�
	bool								bInsureCard[GAME_PLAYER*2];			//����±���
	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���
	tagCustomAndroid					CustomAndroid;						//��ϷAI����
	BYTE								cbPlayMode;							//Լս��Ϸģʽ

};

//¼������
struct Video_GameStart
{
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�	
	SCORE								lUserScore;							//�û�����
	LONG								lGameCellScore;						//��Ϸ�׷�
	WORD								wChairID;							//����ID

	SCORE								lMaxScore;							//�����ע
	WORD				 				wBankerUser;						//��ǰׯ��

	//ʱ�䶨��	
	BYTE								cbTimeAddScore;						//��עʱ��
	BYTE								cbTimeGetCard;						//����ʱ��

	BYTE								cbBankerMode;						//ׯ��ģʽ
	SCORE								lBaseJeton;							//��Ԫ����
	WORD								wPlayerCount;						//�������
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	BYTE								cbPlayStatus[GAME_PLAYER];			//���״̬
	//��ע��Ϣ
	SCORE								lCellScore;							//��Ԫ��ע
	SCORE								lMaxScore;							//�����ע
	//�û���Ϣ
	WORD				 				wBankerUser;						//��ǰׯ��
};

//��ע
struct CMD_S_AddScore
{
	WORD								wAddScoreUser;						//��ע���

	LONGLONG							lAddScore;							//��ע��
};

//Ҫ��
struct CMD_S_GetCard
{
	WORD								wGetCardUser;						//Ҫ�����
	BYTE								cbCardData;							//������
	bool								bSysGet;							//�Ƿ�ϵͳҪ��
};

//����
struct CMD_S_SendCard
{
	BYTE								cbHandCardData[GAME_PLAYER][2];		//��������
	bool								bWin;								//�Ƿ�Ӯ��
	WORD								wCurrentUser;						//��ǰ���
};

//����
struct CMD_S_SplitCard
{
	WORD								wSplitUser;							//�������
	BYTE								bInsured;							//�Ƿ�֮ǰ���˱���

	LONGLONG							lAddScore;							//��ע��
	BYTE								cbCardData[2];						//������
};

//ͣ��
struct CMD_S_StopCard					
{
	WORD								wStopCardUser;						//ͣ�����
	WORD								wCurrentUser;						//��ǰ���
};

//�ӱ�
struct CMD_S_DoubleScore
{
	WORD								wDoubleScoreUser;					//�ӱ����
	BYTE								cbCardData;							//������
	LONGLONG							lAddScore;							//��ע��
};

//����
struct CMD_S_Insure
{
	WORD								wInsureUser;						//�������
	double								dInsureScore;						//���ս�
};

//��Ϸ����
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax[GAME_PLAYER];				//��Ϸ˰��
	LONGLONG							lGameScore[GAME_PLAYER];			//��Ϸ�÷�
	BYTE								cbCardData[GAME_PLAYER * 2][11];	//�û��˿�
};



//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_ADD_SCORE					1									//�û���ע
#define SUB_C_GET_CARD					2									//Ҫ��
#define SUB_C_DOUBLE_SCORE				3									//�ӱ�
#define SUB_C_INSURE					4									//����
#define SUB_C_SPLIT_CARD				5									//����
#define SUB_C_STOP_CARD					6									//������ע




//�û���ע
struct CMD_C_AddScore
{
	LONGLONG							lScore;								//��ע��Ŀ
};

struct CMD_C_RequestQuery_User
{
    DWORD								dwGameID;							//��ѯ�û�GAMEID
};


//������ؽṹ

#define MAX_DEFAULT_CUSTOM_RULE		3											//���Ĭ������

typedef enum
{
    PRIMARY = 0,				//��������
    MIDDLE = 1,					//�м�����
    SENIOR = 2,					//�߼�����
    INVALID = INVALID_BYTE,
}EM_DEFAULT_CUSTOM_RULE;

typedef enum
{
    USER_CTRL,
    ROOM_CTRL,
    SYS_CTRL
}EM_CTRL_TYPE;

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

//ͨ�ÿ�����
typedef struct
{
    DWORD							dwCtrlIndex;						//����(��1��ʼ����)
    DWORD							dwCtrlCount;						//ִ�д���
    SCORE							lSysCtrlSysStorage;					//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lSysCtrlPlayerStorage;				//��ǰ��ҿ��(���ҪӮ��Ǯ)
    SCORE							lSysCtrlParameterK;					//��ǰ����ϵ��(�ٷֱ�)
    SCORE							lSysResetParameterK;				//��������(�ٷֱ�)
    SCORE							lSysCtrlSysWin;						//ϵͳ��Ӯ����
    SCORE							lSysCtrlPlayerWin;					//�����Ӯ����
    CTime							tmConfigTime;						//����ʱ��
    EM_SYSCTRL_STATUS				sysCtrlStatus;						//��ǰ״̬
}SYSCTRL;

//�������
typedef struct
{
    //������Բ���
    DWORD							dwCtrlIndex;						//��������(��1��ʼ����)
    SCORE						    lRoomCtrlInitialSysStorage;			//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lRoomCtrlInitialPlayerStorage;		//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE						    lRoomCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lRoomCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lRoomCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    SCORE							lRoomResetParameterK;				//��������(�ٷֱ�)
    SCORE							lRoomCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lRoomCtrlPlayerWin;					//�����Ӯ����
    CTime							tmConfigTime;						//����ʱ��
    EM_ROOMCTRL_STATUS				roomCtrlStatus;						//��ǰ״̬
}ROOMCTRL;

//�������ѡ��
typedef struct
{
    //������Բ���
    DWORD							dwCtrlIndex;						//��������(��1��ʼ����)
    SCORE						    lRoomCtrlInitialSysStorage;			//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lRoomCtrlInitialPlayerStorage;		//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    LONGLONG						lRoomCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�)
    LONGLONG						lRoomResetParameterK;				//��������(�ٷֱ�)
    SCORE							lUserCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lUserCtrlPlayerWin;					//�����Ӯ����

}ROOMCTRL_ITEM;


//�û�����
typedef struct
{
    //�û����Բ���
    DWORD							dwCtrlIndex;						//��������(��1��ʼ����)
    DWORD							dwGameID;						    //���id
    SCORE						    lUserCtrlInitialSysStorage;			//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lUserCtrlInitialPlayerStorage;		//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE						    lUserCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lUserCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lUserCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    SCORE							lUserResetParameterK;				//��������(�ٷֱ�)
    SCORE							lUserCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lUserCtrlPlayerWin;					//�����Ӯ����
    CTime							tmConfigTime;						//����ʱ��
    EM_ROOMCTRL_STATUS				roomCtrlStatus;						//��ǰ״̬
}USERCTRL;

//�û�����
typedef struct
{
    //�û����Բ���
    DWORD							dwGameID;						    //���id
    SCORE						    lUserCtrlInitialSysStorage;			//���˵���ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lUserCtrlInitialPlayerStorage;		//���˵�����ҿ��(ϵͳҪӮ��Ǯ)
    LONGLONG 						lUserCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    LONGLONG 						lUserResetParameterK;				//��������(�ٷֱ�):
    SCORE							lUserCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lUserCtrlPlayerWin;					//�����Ӯ����
    EM_ROOMCTRL_STATUS				UserCtrlStatus;						//��ǰ״̬
}USERCTRL_ITEM;

//������Ϣ
typedef struct
{
    DEBUG_TYPE							debug_type;						  //��������
    BYTE								cbDebugCount;					  //���Ծ���
    BYTE								cbInitialCount;					  //���Ծ���
    bool							    bCancelDebug;					  //ȡ����ʶ
}ZUANGDEBUG;

//������Ϣ
typedef struct
{
    DWORD								dwGameID;							//���ID
    WORD								wTableID;							//����ID
    DWORD                               dwBankerGameID;                     //ׯ��gameid
    WORD                                wBankerChairID;                     //ׯ��chairID
    bool								bAndroid;							//��ϷAI��ʶ
    TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
    int                                 nPlayerCount;                       //��ǰ��������

}ROOMUSERINFO;

//������Ϣ
typedef struct
{
    WORD								wTableID;							//����ID
    DWORD                               dwBankerGameID;                     //ׯ��gameid
    WORD                                wBankerChairID;                     //ׯ��chairID
    int                                 nPlayerCount;                       //��ǰ��������

}DESKINFO;


//���Զ����
//���Զ����
#define SUB_S_AMDIN_COMMAND			    10									//ϵͳ����
#define SUB_S_SEND_USER_BET_INFO	    11									//������ע
#define SUB_S_REFRESH_RULE_RESULT	    21									//ˢ�����ý��
#define SUB_S_SET_RULE_RESULT		    22									//�������ý��
#define SUB_S_REFRESH_SYSCTRL		    23									//����ϵͳ�����б�
#define SUB_S_REFRESH_ROOMCTRL		    24									//���·�������б�
#define SUB_S_ALL_WINLOST_INFO		    26									//��Ӯ��Ϣͳ��
#define SUB_S_DEBUG_TEXT			    28									//������ʾ��Ϣ
#define SUB_S_CUR_ROOMCTRL_INFO		    30									//��ǰ���������Ϣ
#define SUB_S_REQUEST_QUERY_RESULT		31								    //��ѯ�û����
#define SUB_S_OPERATION_RECORD		    32								    //������¼
#define SUB_S_ZUANG_DEBUG				33								    //�û�����
#define SUB_S_USER_DEBUG_COMPLETE		34								    //�û��������
#define SUB_S_REFRESH_USERCTRL		    35									//�����û������б�
#define SUB_S_CUR_USERCTRL_INFO		    36									//��ǰ�û�������Ϣ
#define SUB_REFRESH_ROOM_CTRL           37                                  //ˢ�µ�ǰ���������
#define SUB_S_DELETE_USER_ITEM_INFO	    38									//ɾ����ҵ���ѡ��
#define SUB_S_ADD_USER_ITEM_INFO	    39									//�����ҵ���ѡ��
#define SUB_S_REFRESH_USER_ITEM_INFO	40									//ˢ����ҵ���ѡ��
#define SUB_S_ADD_ROOM_ITEM_INFO        41                                  //��ӷ������ѡ��
#define SUB_S_DELETE_ROOM_ITEM_INFO     42                                  //ɾ���������ѡ��
#define SUB_S_CLEAR_ROOM_CTRL           43                                  //�������ɷ����������
#define SUB_S_CLEAR_INFO                44                                  //�����ǰ�������ҵ�����Ϣ��ʾ��ɾ����Ч���ݽ������ã�
#define SUB_S_REFRESH_DESKCTRL		    45									//����ZHUOZI�����б�


#define SUB_C_REFRESH_RULE					11							//ˢ������
#define SUB_C_SET_RULE						12							//��������
#define SUB_C_ROOM_CTRL						13							//�������
#define SUB_C_REFRESH_CUR_ROOMCTRL_INFO		14							//ˢ�·��������Ϣ
#define SUB_C_REQUEST_QUERY_USER            15                          //��ѯ�û���Ϣ
#define SUB_C_USER_DEBUG                    16                          //�û�����
#define SUB_C_ADVANCED_REFRESH_ALLCTRLLIST	17							//ˢ�����е��Լ�¼
#define SUB_C_CLEAR_CURRENT_QUERYUSER	    18                          //�����ǰ�û���Ϣ
#define SUB_C_DELETE_USER_CTRL				19							//ɾ���û�����
#define SUB_C_REFRESH_CUR_USERCTRL_INFO		20							//ˢ���û�������Ϣ
#define SUB_C_SET_CHOU_PERCENT      		21							//���ó�ˮ����
#define SUB_C_CANCEL_ROOM_CTRL      		22							//ɾ����ǰ�������
#define SUB_C_DELETE_ROOM_CTRL			    23					        //ɾ��ָ���������
#define SUB_C_DELETE_ALL_USER_CTRL			24					        //ɾ�������û�����
#define SUB_C_DELETE_ALL_ROOM_CTRL			25					        //ɾ�����з������
#define SUB_C_DELETE_USER_CTRL_LOG			26					        //ɾ��ָ���û�����LOG
#define SUB_C_DELETE_ROOM_CTRL_LOG			27					        //ɾ��ָ���������LOG
#define SUB_C_DIAN_USER_DEBUG			    28					        //��ҵ�debug
#define SUB_C_CLRARE_DESK       			29					        //���ָ������log
#define SUB_C_CLEARE_DESK_ALL			    30					        //����������ӵ���log

struct CMD_S_RequestQueryResult
{
    ROOMUSERINFO						userinfo;							//�û���Ϣ
    bool								bFind;								//�ҵ���ʶ
};


//������½��
struct CMD_S_RequestUpdateRoomInfo_Result
{
    ROOMUSERINFO						currentQueryUserInfo;				//��ǰ��ѯ�û���Ϣ
    bool								bExistDebug;						//��ѯ�û����ڵ��Ա�ʶ
    ZUANGDEBUG							currentUserDebug;
};


//ˢ�����ý��
struct CMD_S_RefreshRuleResult
{
    SCORE							lCurSysStorage;					//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lCurPlayerStorage;				//��ǰ��ҿ��(���ҪӮ��Ǯ)
    SCORE							lCurParameterK;					//��ǰ����ϵ��(�ٷֱ�):
    SCORE							lResetParameterK;				//ǿ����������(�ٷֱ�):
    SCORE							lStorageResetCount;				//���ô���
    SCORE							lSysCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lSysCtrlPlayerWin;				//�����Ӯ����
};

//�������ý��
struct CMD_S_SetRuleResult
{
    SCORE							lConfigSysStorage;				//����ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lConfigPlayerStorage;			//������ҿ��(���ҪӮ��Ǯ)
    SCORE							lConfigParameterK;				//���õ���ϵ��(�ٷֱ�):
    SCORE							lResetParameterK;				//ǿ����������(�ٷֱ�):
};

//������Խ��
struct CMD_S_RoomCtrlResult
{
    DWORD							dwCtrlIndex;						//��������
    SCORE						    lRoomCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lRoomCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lRoomCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    SCORE							lRoomResetParameterK;				//ǿ����������(�ٷֱ�):
    SCORE							lRoomCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lRoomCtrlPlayerWin;					//�����Ӯ����
    CTime							tmConfigTime;						//����ʱ��
    EM_ROOMCTRL_STATUS				roomCtrlStatus;						//��ǰ״̬
};

//�û����Խ��
struct CMD_S_UserCtrlResult
{
    DWORD							dwCtrlIndex;						//��������
    DWORD							dwGameID;						    //���ID
    SCORE						    lUserCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lUserCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lUserCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    SCORE							lUserResetParameterK;				//��������(�ٷֱ�):
    SCORE							lUserCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lUserCtrlPlayerWin;					//�����Ӯ����
    CTime							tmConfigTime;						//����ʱ��
    EM_ROOMCTRL_STATUS				roomCtrlStatus;						//��ǰ״̬
};


//ϵͳ���Խ��
struct CMD_S_SysCtrlResult
{
    DWORD							dwCtrlIndex;						//����(��1��ʼ����)
    DWORD							dwResetCount;						//���ô���
    SCORE							lSysCtrlSysStorage;					//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lSysCtrlPlayerStorage;				//��ǰ��ҿ��(���ҪӮ��Ǯ)
    SCORE							lSysCtrlParameterK;					//��ǰ����ϵ��(�ٷֱ�):
    SCORE							lResetParameterK;					//��ǰ����ϵ��(�ٷֱ�):
    SCORE							lSysCtrlSysWin;						//ϵͳ��Ӯ����
    SCORE							lSysCtrlPlayerWin;					//�����Ӯ����
    CTime							tmResetTime;						//����ʱ��
    EM_SYSCTRL_STATUS				sysCtrlStatus;						//��ǰ״̬
};

//��Ӯ��Ϣͳ��
struct CMD_S_RefreshWinTotal
{
    SCORE							lSysWin;						    //��Ӯ����
    SCORE							lTotalServiceWin;					//�����������
    SCORE							lTotalChouShui;						//��ˮ����
    SCORE                           lTotalRoomCtrlWin;                  //�����������Ӯ
    SCORE							lChouPercent;					    //��ˮ����
    bool                            bIsCleanRoomCtrl;

};

//ȡ����ǰ��ҵ���
struct CMD_S_DELETE_USER
{
    DWORD                               dUserDebugID;                       //����ID
};

//ȡ����ǰ�������
struct CMD_S_DELETE_ROOM
{
    DWORD                               dRoomDebugID;                       //����ID
};

//������ʾ
struct CMD_S_DebugText
{
    TCHAR							szMessageText[256];					//�����ı�
};


//������¼
struct CMD_S_Operation_Record
{
    TCHAR		szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];			//��¼���²�����20����¼
};

//��ǰ���������Ϣ
struct CMD_S_CurRoomCtrlInfo
{
    DWORD							dwCtrlIndex;						//��������
    SCORE						    lRoomCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lRoomCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lRoomCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    SCORE							lRoomResetParameterK;				//ǿ����������(�ٷֱ�):
    SCORE							lRoomCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lRoomCtrlPlayerWin;					//�����Ӯ����
    BYTE                            cbCtrlType;                         //��ǰ��������
};

//��ǰ�û�������Ϣ
struct CMD_S_CurUserCtrlInfo
{
    DWORD							dwCtrlIndex;						//��������
    DWORD							dwGameID;						    //�û�id
    SCORE						    lUserCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lUserCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lUserCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    SCORE							lResetParameterK;					//ǿ����������(�ٷֱ�)
    SCORE							lUserCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lUserCtrlPlayerWin;					//�����Ӯ����
};


//��������
struct CMD_C_SetRule
{
    SCORE							lConfigSysStorage;							//����ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lConfigPlayerStorage;						//������ҿ��(���ҪӮ��Ǯ)
    SCORE							lConfigParameterK;							//���õ���ϵ��(�ٷֱ�)
    SCORE							lResetParameterK;							//ǿ����������(�ٷֱ�)

};

//�������
struct CMD_C_RoomCtrl
{
    SCORE						    lRoomCtrlSysStorage;					    //���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lRoomCtrlPlayerStorage;					    //���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lRoomCtrlParameterK;					    //���˷�����Ե���ϵ��(�ٷֱ�):
    SCORE							lResetParameterK;							//ǿ����������(�ٷֱ�)

};

//�û�����
struct CMD_C_UserCtrl
{
    DWORD							dwUserID;							    //ѡ�����id
    SCORE						    lUserCtrlSysStorage;					//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE						    lUserCtrlPlayerStorage;					//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lUserCtrlParameterK;					//���˷�����Ե���ϵ��(�ٷֱ�):
    SCORE							lUserResetParameterK;					//ǿ����������(�ٷֱ�):
};



//ׯ�ҵ���
struct CMD_C_ZuangDebug
{
    DWORD								dwGameID;							//GAMEID
    WORD                                wTableID;
    ZUANGDEBUG							zuangDebugInfo;
};

//���ó�ˮ����
struct CMD_C_ChouPercent
{
    int                                 nChouShuiPercent;                   //��ˮ����
};

//ɾ����ǰָ�����ӵ��Ե�log
struct CMD_C_Delete_Desk_log
{
    DWORD                               dDeskDebugID;                       //����ID
};

//ȡ����ǰ�������
struct CMD_C_Cancel_RoomCtrl
{
    DWORD                               dRoomDebugID;                       //����ID
};

//ȡ����ǰ��ҵ���
struct CMD_C_DELETE_USER
{
    DWORD                               dUserDebugID;                       //����ID
};

//ׯ�ҵ���
struct CMD_S_ZuangDebug
{
    bool                                bOk;
    BYTE								cbDebugCount;						//���Ծ���
};

//ZUANG����
struct CMD_S_ZuangDebugComplete
{
    DWORD							    dwBankerID;							//��������
    DEBUG_TYPE							debugType;							//��������
    BYTE								cbRemainDebugCount;					//ʣ����Ծ���
    BYTE								cbPlayers;					        //����
    SCORE                               lTotalScore;                        //���ֵ�����Ӯͳ��
};

//����ׯ�ҵ���
typedef struct
{
    DWORD							    dwCtrlIndex;						//��������
    DWORD                               dwGameID;                           //�������
    DESKINFO						    deskInfo;						    //������Ϣ
    ZUANGDEBUG							zuangDebug;						    //ׯ�ҵ���
    EM_ROOMCTRL_STATUS				    CtrlStatus;						    //��ǰ״̬
    CTime							    tmResetTime;						//����ʱ��

}ROOMDESKDEBUG;


//���ӵ��Խ��
struct CMD_S_DeskCtrlResult
{
    ROOMDESKDEBUG                       deskCtrl;
};


//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)
#endif