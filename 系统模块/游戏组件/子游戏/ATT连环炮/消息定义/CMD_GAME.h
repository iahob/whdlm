#ifndef CMD_ATT_HEAD_FILE
#define CMD_ATT_HEAD_FILE

#pragma pack(push)  
#pragma pack(1)
#include "../��Ϸ������/GameControlDefine.h"
//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						532									//��Ϸ I D
#define GAME_PLAYER					1									//��Ϸ����
#define GAME_NAME					TEXT("ATT������")					//��Ϸ����
#define	MAX_CARD_COUNT				5									//����˿���Ŀ
#define FULL_COUNT					54									//ȫ����Ŀ

//�汾��Ϣ
#define VERSION_SERVER			    PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//״̬����
#define	GS_GAME_FREE				GAME_STATUS_FREE
#define GS_GAME_PLAY				GAME_STATUS_FREE+1
#define	GS_GAME_LUCKYTIME			GAME_STATUS_FREE+2									

//���Ͷ���
#define	CT_5K						0                                    //����
#define	CT_RS						1									 //ͬ����˳
#define	CT_SF						2									 //ͬ��˳
#define	CT_4K						3									 //����
#define	CT_FH						4									 //��«
#define	CT_FL						5									 //ͬ��
#define	CT_ST						6									 //˳��
#define	CT_3K						7									 //����
#define	CT_2P						8									 //����
#define	CT_1P						9									 //һ��
#define	CT_INVALID					11									 //��Ч����

#define MAX_CARD_CT					10									 //�����������
#define MAX_GUESS_COUNT				6									 //���´�С����
#define LUCKYTIME_CARDDATA_COUNT	13									 //ȫ���˿���Ŀ

//������¼
#define MAX_OPERATION_RECORD		20									 //������¼����
#define RECORD_LENGTH				200									 //ÿ����¼�ֳ�

//������Ϣ
#define IDM_ADMIN_UPDATE_STORAGE_ROOM		WM_USER+1001
#define IDM_ADMIN_MODIFY_ROOMCONFIG			WM_USER+1011
#define IDM_REQUEST_QUERY_USER				WM_USER+1012
#define ROOMUSERDEBUG					    WM_USER+1013
#define IDM_REQUEST_UPDATE_ROOMUSERLIST		WM_USER+1015
#define IDM_REQUEST_UPDATE_ROOMSTORAGE		WM_USER+1016

#ifndef SCOREEX
#define SCOREEX SCORE
#endif
#ifndef SCOREEX_STRING
#define SCOREEX_STRING TEXT("%I64d")
#endif
//����
inline bool D_GreaterThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 > dVal2;
}
//С��
inline bool D_LessThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 < dVal2;
}
//С�ڻ����
inline bool D_LessThenEquals(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 <= dVal2;
}
inline bool D_UnGreaterThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return D_LessThenEquals(dVal1, dVal2);
}
//���ڻ����
inline bool D_GreaterThenEquals(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 >= dVal2;
}
inline bool D_UnLessThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return D_GreaterThenEquals(dVal1, dVal2);
}
//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START			    100									//��Ϸ��ʼ
#define SUB_S_GAME_CONCLUDE			    101									//��Ϸ����
#define SUB_S_CURRENT_BET			    102									//ѹע
#define	SUB_S_GUESS					    103									//�´�С
#define SUB_S_SWITCH_CARD			    104									//ת���˿�
#define	SUB_S_LUCKYTIME				    105									//luckytime
#define	SUB_S_CONTINUE_GUESS		    106									//��Ѻ
#define SUB_S_GAME_RECORD			    107									//�鿴��¼
#define SUB_S_GAME_AUTO				    108									//�Զ���Ϸ
#define SUB_S_GAME_GUSS_INDEX			109									//�²��С��index

////////////////////////////////////////////////////////////////////////////////////////////////////////

//�������ͣ����ȼ�������>���䣩
typedef enum
{
    USER_CTRL,
    ROOM_CTRL,
    STORAGE_CTRL

}EM_CTRL_TYPE;

//���ýṹ
struct tagCustomRule
{
    int									    lExchangeRadio;				//�һ�����
    int                                     nDebugPercent;              //���ڱ���
    SCORE									lSysStorage;
    SCORE									lUserStorage;
    SCORE                                   lBet[10];
   
};


//BONUS�ṹ
typedef struct
{
	LONG							lBonus5K;
	LONG							lBonusRS;
	LONG							lBonusSF;
	LONG							lBonus4K;
}BONUS;

typedef struct
{
	bool							bSwitchFlag[MAX_CARD_COUNT];				//ת����ʶ
	BYTE							cbSwitchCount;								//ת������
}SWITCH_CARD;

//ѹע����
typedef struct
{
	LONG							lHighRadio;
	LONG							lMidRadio;
	LONG							lLowRadio;
}BET_RADIO;

//ѹ��С
typedef struct
{
	WORD							wLuckyCardIndex;					
	WORD							wGuessCount;						//�´�С����
	bool							bBig;								//��С��ʶ
}GUESSINFO;

//ѹ��С��¼
typedef struct
{
	bool							bIsAlreadyGuess;					//�Ѿ���
	bool							bGuessRight;						//�¶�
	bool							bValid;								//�Ƿ���Ч
}GUESSRECORD;

//��¼
typedef struct
{
	SCORE							l5KBet;
	SCORE							lRSBet;
	SCORE							lSFBet;
	SCORE							l4KBet;
	SCORE							lFHBet;
	SCORE							lFLBet;
	SCORE							lSTBet;
	SCORE							l3KBet;
	SCORE							l2PBet;
	SCORE							l1PBet;
	SCORE							lInvalidBet;
	SCORE							lPlayTotal;
}RECORD_INFO;

//�Զ���ʶ
typedef struct
{
	bool							bFirstReversalFinish;					//��һ�η�����ɱ�ʶ
	bool							bAlreadySwitchCard;						//�Ѽ���ת����
}AUTO_FLAG;


//�û���Ϊ
typedef enum { USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT } EM_USER_ACTION;


//�����û���Ϣ
typedef struct
{
	WORD							wChairID;							//����ID
	WORD							wTableID;							//����ID
	DWORD							dwGameID;							//GAMEID
	bool							bAndroid;							//�����˱�ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbUserStatus;						//�û�״̬
	BYTE							cbGameStatus;						//��Ϸ״̬
	LONGLONG						lGameScore;							//��Ϸ��Ӯ��
}ROOMUSERINFO;


//����״̬
struct CMD_S_StatusGameFree
{
	BONUS							bonus;								//����
	LONG							lExchangeRadio;						//�һ�����
	LONG							lbureauCount;						//��Ϸ����
	RECORD_INFO						recordInfo;							//��¼��Ϣ
	WORD							wTableID;							//����ID
	TCHAR							szServerName[LEN_SERVER];			//��������
    SCORE                           lBetItem[10]; 
    LONG							lBet;								//ѹע
};

//��Ϸ״̬
struct CMD_S_StatusGamePlay
{
	BONUS							bonus;								//����
	LONG							lbureauCount;						//��Ϸ����
	LONG							lExchangeRadio;						//�һ�����
	LONG							lBet;								//ѹע
	bool							balreadySwitch;						//�Ƿ�ת��
	bool							bSwitchFlag[MAX_CARD_COUNT];		//ת����ʶ
	bool							bAuto;
	BYTE							cbFirstCardData[MAX_CARD_COUNT];	//��ʼ�˿��б�
	BYTE							cbSecondCardData[MAX_CARD_COUNT];	//�ڶ����˿��б�
	BYTE							cbSwitchCardData[MAX_CARD_COUNT];	//�����л����˿��б�
	RECORD_INFO						recordInfo;							//��¼��Ϣ
	WORD							wTableID;							//����ID
	TCHAR							szServerName[LEN_SERVER];			//��������
    SCORE                           lBetItem[10];
};

//LuckyTime״̬
struct CMD_S_StatusLuckyTime
{
	BONUS							bonus;										//����
	LONG							lbureauCount;								//��Ϸ����
	LONG							lExchangeRadio;								//�һ�����
	LONG							lBet;										//ѹע
	bool							bLuckyTimePause;							//��ͣ��ʶ
	GUESSRECORD						guessRecord[MAX_GUESS_COUNT];				//�´�С��¼
	BYTE							cbGuessCardResultRecord[MAX_GUESS_COUNT];	//�´�С�Ƽ�¼
	WORD							wCurrentGuessCount;							//�´�С����
	BYTE							cbLuckyCardData[LUCKYTIME_CARDDATA_COUNT];  //LuckyCard
	BYTE							cbHandCardData[MAX_CARD_COUNT];				//�����˿�
	RECORD_INFO						recordInfo;									//��¼��Ϣ
	WORD							wTableID;									//����ID
	TCHAR							szServerName[LEN_SERVER];					//��������
    SCORE                           lBetItem[10];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	BYTE							cbFirstCardArray[MAX_CARD_COUNT];	    //��ʼ�˿��б�
	BYTE							cbSecondCardArray[MAX_CARD_COUNT];	    //�ڶ����˿��б�
};

//��Ϸ����
struct CMD_S_GameConclude
{
	//��ҳɼ�
	SCORE							lUserScore;							//��ҳɼ�
	LONG							lbureauCount;						//��Ϸ����
	BONUS							Bonus;								//����
	bool							bGuess;								//�Ƿ�´�С
	RECORD_INFO						recordInfo;							//��¼��Ϣ
	LONG							lExchangeRadio;						//�һ�����
	LONG							llastBet;							//�ϴ���ע�ĳ���
	
};

//ѹ��С
struct CMD_S_Guess
{	
	GUESSRECORD						guessRecord[MAX_GUESS_COUNT];				//�´�С��¼
	BYTE							cbGuessCardResultRecord[MAX_GUESS_COUNT];	//�´�С�Ƽ�¼
	WORD							wCurrentGuessCount;							//��ǰ�´�С����
};

//ѹע
struct CMD_S_CurrentBet
{
	LONG							lBet;				
};

//ת��
struct CMD_S_SwitchCard
{
	SWITCH_CARD						switchCard;
    bool                            bGuss;
};

//luckytime
struct CMD_S_LuckyTime
{
	BYTE							cbLuckyCardData[LUCKYTIME_CARDDATA_COUNT];
    BYTE							cbGuessCardResultRecord[MAX_GUESS_COUNT];	//�´�С�Ƽ�¼
};

//��¼
struct CMD_S_GameRecord
{
	bool							bShowRecord;								
};

//�Զ���Ϸ
struct CMD_S_GameAuto
{
	bool							bAuto;		

};

//��ѯ�û����
struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO					userinfo;							//�û���Ϣ
	bool							bFind;								//�ҵ���ʶ
};



//��������б�
struct CMD_S_UpdateRoomUserList
{
	DWORD							dwUserID;							//�û�ID
	ROOMUSERINFO					roomUserInfo;
};


//�ͻ�������ṹ��20 �� 27��
#define	SUB_C_LUCKYTIME				    20									//luckytime
#define SUB_C_CURRENT_BET			    21									//ѹע
#define SUB_C_SWITCH_CARD			    22									//ת���˿�
#define SUB_C_GAME_END			        23									//��Ϸ����
#define SUB_C_GUESS					    24									//��Ϸ����
#define SUB_C_CONTINUE_GUESS		    25									//��Ѻ
#define SUB_C_GAME_RECORD			    26									//�鿴��¼
#define SUB_C_GAME_AUTO				    27									//�Զ���Ϸ
#define SUB_C_GAME_CLICK_GUSS			28									//����´�С


//�´�С
struct CMD_S_CLICK_Guess
{
    BYTE							cbIndex;						    //Ŀ���˿�
};



//�´�С
struct CMD_C_Guess
{
	GUESSINFO						guessInfo;							//�²���Ϣ
	BYTE							cbKeyCardData;						//Ŀ���˿�
};

//ѹע
struct CMD_C_CurrentBet
{
	LONG							lBet;								
};

//��¼
struct CMD_C_GameRecord
{
	bool							bShowRecord;								
};

//�Զ���Ϸ
struct CMD_C_GameAuto
{
	bool							bAuto;			
};

//����
struct CMD_C_GameEnd
{
	WORD							wChairID;
	LONG							lBet;	
	BYTE							cbHandCardData[MAX_CARD_COUNT];
};

//ת��
struct CMD_C_SwitchCard
{
	SWITCH_CARD						switchCard;	
	bool							bSwitchCard;
	BYTE							cbSwitchCardData[MAX_CARD_COUNT];		//�����л����˿��б�
};

struct CMD_C_RequestQuery_User
{
	DWORD							dwGameID;								//��ѯ�û�GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			    //��ѯ�û��ǳ�
};


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
    SCORE							lSysCtrlSysStorage;					//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lSysCtrlPlayerStorage;				//��ǰ��ҿ��(���ҪӮ��Ǯ)
    SCORE							lSysCtrlParameterK;					//��ǰ����ϵ��(�ٷֱ�):
    SCORE							lSysCtrlDeadLineLimit;				//��ֹ���
    SCORE							lSysCtrlSysWin;						//ϵͳ��Ӯ����
    SCORE							lSysCtrlPlayerWin;					//�����Ӯ����
    CTime							tmResetTime;						//����ʱ��
    EM_SYSCTRL_STATUS				sysCtrlStatus;						//��ǰ״̬
}SYSCTRL;

//�������
typedef struct
{
    //������Բ���
    DWORD							dwCtrlIndex;						//��������(��1��ʼ����)
    LONGLONG						lRoomCtrlInitialSysStorage;			//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    LONGLONG						lRoomCtrlInitialPlayerStorage;		//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    LONGLONG						lRoomCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    LONGLONG						lRoomCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lRoomCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    BYTE							nRoomCtrlStorageDeadLine;			//����ֹ
    SCORE							lRoomCtrlDeadLineLimit;				//��ֹ���
    SCORE							lRoomCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lRoomCtrlPlayerWin;					//�����Ӯ����
    CTime							tmConfigTime;						//����ʱ��
    EM_ROOMCTRL_STATUS				roomCtrlStatus;						//��ǰ״̬
}ROOMCTRL;

//�û�����
typedef struct
{
    //�û����Բ���
    DWORD							dwCtrlIndex;						//��������(��1��ʼ����)
    DWORD							dwGameID;						    //���id
    LONGLONG						lUserCtrlInitialSysStorage;			//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    LONGLONG						lUserCtrlInitialPlayerStorage;		//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    LONGLONG						lUserCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    LONGLONG						lUserCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lUserCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    BYTE							nUserCtrlStorageDeadLine;			//����ֹ
    SCORE							lUserCtrlDeadLineLimit;				//��ֹ���
    SCORE							lUserCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lUserCtrlPlayerWin;					//�����Ӯ����
    CTime							tmConfigTime;						//����ʱ��
    EM_ROOMCTRL_STATUS				roomCtrlStatus;						//��ǰ״̬
}USERCTRL;

//���ƶ����
#define SUB_S_AMDIN_COMMAND			    10									//ϵͳ����
#define SUB_S_SEND_USER_BET_INFO	    11									//������ע
#define SUB_S_REFRESH_RULE_RESULT	    21									//ˢ�����ý��
#define SUB_S_SET_RULE_RESULT		    22									//�������ý��
#define SUB_S_REFRESH_SYSCTRL		    23									//����ϵͳ�����б�
#define SUB_S_REFRESH_ROOMCTRL		    24									//���·�������б�
#define SUB_S_LIST_WINLOST_INFO		    26									//�б���Ӯ��Ϣ
#define SUB_S_DEBUG_TEXT			    28									//������ʾ��Ϣ
#define SUB_S_CUR_ROOMCTRL_INFO		    30									//��ǰ���������Ϣ
#define SUB_S_REQUEST_QUERY_RESULT		31								    //��ѯ�û����
#define SUB_S_OPERATION_RECORD		    32								    //������¼
#define SUB_S_USER_DEBUG				33								    //�û�����
#define SUB_S_USER_DEBUG_COMPLETE		34								    //�û��������
#define SUB_S_REFRESH_USERCTRL		    35									//�����û������б�
#define SUB_S_CUR_USERCTRL_INFO		    36									//��ǰ�û�������Ϣ

//���Զ����
#define SUB_C_REFRESH_RULE					11							//ˢ������
#define SUB_C_SET_RULE						12							//��������
#define SUB_C_ROOM_CTRL						13							//�������
#define SUB_C_REFRESH_CUR_ROOMCTRL_INFO		14							//ˢ�·��������Ϣ
#define SUB_C_REQUEST_QUERY_USER            15                          //��ѯ�û���Ϣ
#define SUB_C_USER_DEBUG                    16                          //�û�����
#define SUB_C_ADVANCED_REFRESH_ALLCTRLLIST	17							//ˢ�����е��Լ�¼
#define SUB_C_CLEAR_CURRENT_QUERYUSER	    18                          //�����ǰ�û���Ϣ
#define SUB_C_USER_CTRL						19							//�û�����
#define SUB_C_REFRESH_CUR_USERCTRL_INFO		20							//ˢ���û�������Ϣ

//ˢ�����ý��
struct CMD_S_RefreshRuleResult
{
    SCORE							lCurSysStorage;					//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lCurPlayerStorage;				//��ǰ��ҿ��(���ҪӮ��Ǯ)
    SCORE							lCurParameterK;					//��ǰ����ϵ��(�ٷֱ�):
    SCORE							lConfigSysStorage;				//����ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lConfigPlayerStorage;			//������ҿ��(���ҪӮ��Ǯ)
    SCORE							lConfigParameterK;				//���õ���ϵ��(�ٷֱ�):
    BOOL							nSysCtrlStorageDeadLine;		//����ֹ
    SCORE							lStorageResetCount;				//���ô���
    SCORE							lSysCtrlDeadLineLimit;			//��ֹ���
    SCORE							lSysCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lSysCtrlPlayerWin;				//�����Ӯ����
    int                             nCtrlType;                      //2ϵͳ���ƣ�1������ƣ�0��ҿ��ƣ�����ˢ�¿��ƽ���
};

//�������ý��
struct CMD_S_SetRuleResult
{
    SCORE							lConfigSysStorage;				//����ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lConfigPlayerStorage;			//������ҿ��(���ҪӮ��Ǯ)
    SCORE							lConfigParameterK;				//���õ���ϵ��(�ٷֱ�):
    BOOL							nSysCtrlStorageDeadLine;		//����ֹ
};

//������Խ��
struct CMD_S_RoomCtrlResult
{
    DWORD							dwCtrlIndex;						//��������
    LONGLONG						lRoomCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    LONGLONG						lRoomCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lRoomCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    BOOL							nRoomCtrlStorageDeadLine;			//����ֹ
    SCORE							lRoomCtrlDeadLineLimit;				//��ֹ���
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
    LONGLONG						lUserCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    LONGLONG						lUserCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lUserCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    BOOL							nUserCtrlStorageDeadLine;			//����ֹ
    SCORE							lUserCtrlDeadLineLimit;				//��ֹ���
    SCORE							lUserCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lUserCtrlPlayerWin;					//�����Ӯ����
    CTime							tmConfigTime;						//����ʱ��
    EM_ROOMCTRL_STATUS				roomCtrlStatus;						//��ǰ״̬
};


//ϵͳ���Խ��
struct CMD_S_SysCtrlResult
{
    DWORD							dwCtrlIndex;						//����(��1��ʼ����)
    SCORE							lSysCtrlSysStorage;					//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lSysCtrlPlayerStorage;				//��ǰ��ҿ��(���ҪӮ��Ǯ)
    SCORE							lSysCtrlParameterK;					//��ǰ����ϵ��(�ٷֱ�):
    SCORE							lSysCtrlDeadLineLimit;				//��ֹ���
    SCORE							lSysCtrlSysWin;						//ϵͳ��Ӯ����
    SCORE							lSysCtrlPlayerWin;					//�����Ӯ����
    CTime							tmResetTime;						//����ʱ��
    EM_SYSCTRL_STATUS				sysCtrlStatus;						//��ǰ״̬
};

//�б���Ӯ��Ϣ
struct CMD_S_ListWinLostInfo
{
    SCORE							lSysCtrlSysWin;						//ϵͳ��Ӯ����
    SCORE							lSysCtrlPlayerWin;					//�����Ӯ����
    SCORE							lRoomCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lRoomCtrlPlayerWin;					//�����Ӯ����
    SCORE							lUserCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lUserCtrlPlayerWin;					//�����Ӯ����
    SCORE                           lRevenueWin;                        //ϵͳ˰��
    CTime							tmStartRecord;						//��ʼ��¼ʱ��
};


//������ʾ
struct CMD_S_DebugText
{
    TCHAR							szMessageText[256];					//�����ı�
};


//������¼
struct CMD_S_Operation_Record
{
    TCHAR		szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];				//��¼���²�����20����¼
};

//��ǰ���������Ϣ
struct CMD_S_CurRoomCtrlInfo
{
    DWORD							dwCtrlIndex;						//��������
    LONGLONG						lRoomCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    LONGLONG						lRoomCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lRoomCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    SCORE							lRoomCtrlDeadLineLimit;				//��ֹ���
    SCORE							lRoomCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lRoomCtrlPlayerWin;					//�����Ӯ����
    BYTE                            cbCtrlType;                         //��ǰ��������
};

//��ǰ�û�������Ϣ
struct CMD_S_CurUserCtrlInfo
{
    DWORD							dwCtrlIndex;						//��������
    DWORD							dwGameID;						    //�û�id
    LONGLONG						lUserCtrlSysStorage;				//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    LONGLONG						lUserCtrlPlayerStorage;				//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lUserCtrlParameterK;				//���˷�����Ե���ϵ��(�ٷֱ�):
    SCORE							lUserCtrlDeadLineLimit;				//��ֹ���
    SCORE							lUserCtrlSysWin;					//ϵͳ��Ӯ����
    SCORE							lUserCtrlPlayerWin;					//�����Ӯ����
};


//��������
struct CMD_C_SetRule
{
    SCORE							lConfigSysStorage;							//����ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							lConfigPlayerStorage;						//������ҿ��(���ҪӮ��Ǯ)
    SCORE							lConfigParameterK;							//���õ���ϵ��(�ٷֱ�)
    BOOL							nSysCtrlStorageDeadLine;					//����ֹ
};

//�������
struct CMD_C_RoomCtrl
{
    DWORD							dwSelCtrlIndex;							//ѡ���������
    LONGLONG						lRoomCtrlSysStorage;					//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    LONGLONG						lRoomCtrlPlayerStorage;					//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lRoomCtrlParameterK;					//���˷�����Ե���ϵ��(�ٷֱ�):
    bool							bCancelRoomCtrl;						//ȡ�����Ա�ʶ(trueΪȡ����falseΪ���õ���)
    BOOL							nRoomCtrlStorageDeadLine;				//����ֹ
};

//�û�����
struct CMD_C_UserCtrl
{
    DWORD							dwSelCtrlIndex;							//ѡ���������
    DWORD							dwUserID;							    //ѡ�����id
    LONGLONG						lUserCtrlSysStorage;					//���˷������ϵͳ���(ϵͳҪӮ��Ǯ)
    LONGLONG						lUserCtrlPlayerStorage;					//���˷��������ҿ��(ϵͳҪӮ��Ǯ)
    SCORE							lUserCtrlParameterK;					//���˷�����Ե���ϵ��(�ٷֱ�):
    bool							bCancelUserCtrl;						//ȡ�����Ա�ʶ(trueΪȡ����falseΪ���õ���)
    BOOL							nUserCtrlStorageDeadLine;				//����ֹ
};


/////////////////////////////////////////////////////////////////////////

#pragma pack(pop)
#endif
