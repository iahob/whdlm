#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

#define UR_GAME_DEBUG					0x20000000L				//��Ϸ�������
//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						104									//��Ϸ I D
#define GAME_NAME					TEXT("����ţţ")					//��Ϸ����

//�������
#define GAME_PLAYER					MAX_CHAIR							//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_PLACE_JETTON		GAME_STATUS_PLAY					//��ע״̬
#define GAME_SCENE_GAME_END			GAME_STATUS_PLAY+1					//����״̬


//��������
#define ID_TIAN_MEN					1									//��
#define ID_DI_MEN					2									//��
#define ID_XUAN_MEN					3									//��
#define ID_HUANG_MEN				4									//��

//�������
#define BANKER_INDEX				0									//ׯ������
#define SHUN_MEN_INDEX				1									//˳������
#define DUI_MEN_INDEX				2									//��������
#define DAO_MEN_INDEX				3									//��������
#define HUAN_MEN_INDEX				4									//��������

#define AREA_MAX					4									//������Ŀ
#define DEBUG_AREA					4

//ϵͳ��Ӯ
#define AREA_SYSLOST				5									//�������ϵͳ��
#define AREA_SYSWIN					6									//�������ϵͳӮ

//���Է�ʽ
#define	CS_SYS_LOSE					1
#define	CS_SYS_WIN					2
#define	CS_BET_AREA					3

//���ʶ���
#define RATE_TWO_PAIR				12									//��������
#define SERVER_LEN					32									//���䳤��

//ռ��
#define SEAT_LEFT1_INDEX			0									//��һ
#define SEAT_LEFT2_INDEX			1									//���
#define SEAT_LEFT3_INDEX			2									//����
#define SEAT_RIGHT1_INDEX			3									//��һ
#define SEAT_RIGHT2_INDEX			4									//�Ҷ�
#define SEAT_RIGHT3_INDEX			5									//����
#define MAX_OCCUPY_SEAT_COUNT		6									//���ռλ����
#define SEAT_INVALID_INDEX			7									//��Ч����
#define	MAX_CHIP_COUNT				7									//ǰ�˳������
#define	RECENT_RECORD_COUNT			20									//ͳ�����20��
#define WEEK_LEN					7 * 24 * 3600						//һ��ʱ�䳤��

////////////////////////////////////////////////////////////////////////////////////
//�궨��
#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#endif

//�������ͣ����ȼ���ϵͳ��Ӯ=����>����>ϵͳ��
typedef enum
{
	SYSWINLOST_CTRL,
	AREA_CTRL,
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

typedef enum
{
	VALID = 0,
	INVALID,
}EM_AREACTRL_STATUS;

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

//�������
typedef struct
{
	BYTE							cbDebugStyle;						//���Է�ʽ
	bool							bWinArea[AREA_MAX + 1];				//Ӯ������
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	BYTE							cbConfigCtrlTimes;					//���õ��Ծ���
	BYTE							cbCurCtrlTimes;						//��ǰ���Ծ���
	SCORE							lAreaCtrlSysWin;					//ϵͳ��Ӯ����
	SCORE							lAreaCtrlPlayerWin;					//�����Ӯ����
	CTime							tmConfigTime;						//����ʱ��
	EM_AREACTRL_STATUS				areaCtrlStatus;						//��ǰ״̬
}AREACTRL;

//���ýṹ
struct tagCustomRule
{
	//���
	SCORE							lConfigSysStorage;					//ϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE							lConfigPlayerStorage;				//��ҿ��(���ҪӮ��Ǯ)
	SCORE							lConfigParameterK;					//����ϵ��(�ٷֱ�):
};

//ͳ�Ƽ�¼
struct tagStatisticsRecord
{
	SCORE							lBankerWinScore;					//�þ�ׯ��Ӯ��
	SCORE							lPlayerWinScore;					//�þ��м�Ӯ��
	SCORE							lAllJettonScore[AREA_MAX + 1];		//�þ�ȫ��������ע
	SCORE							lAreaWinScore[AREA_MAX + 1];		//�þָ�������÷�
	BYTE							cbAreaCardType[AREA_MAX + 1];		//�þָ�����������
	bool							bWinFlag[AREA_MAX + 1];				//��Ӯ��־
	bool							bValid;								//�þ��Ƿ���Ч(��������ׯ�Ҳ��ۼ�����20��ͳ�Ƽ�¼)
	CTime							tmRecordTime;						//��¼ʱ��
};

//������ׯ
///////////////////////////////

//������ׯ����
typedef enum
{
    SUPERBANKER_VIPTYPE,
    SUPERBANKER_CONSUMETYPE
} SUPERBANKERTYPE;

//ռλ����
typedef enum
{
    OCCUPYSEAT_VIPTYPE,
    OCCUPYSEAT_CONSUMETYPE,
    OCCUPYSEAT_FREETYPE
} OCCUPYSEATTYPE;

typedef enum
{
    VIP1_INDEX = 1,  //cbMemberOrder = 1 �����Ա
    VIP2_INDEX,		 //�����Ա
    VIP3_INDEX,		 //�����Ա
    VIP4_INDEX,		 //�����Ա
    VIP5_INDEX,		 //VIP��Ա
    VIP_INVALID		 //��Ч
} VIPINDEX;

typedef struct
{
    SUPERBANKERTYPE		superbankerType;		//��ׯ����
    VIPINDEX			enVipIndex;				//VIP����
    SCORE				lSuperBankerConsume;	//��ׯ����
} SUPERBANKERCONFIG;

typedef struct
{
    OCCUPYSEATTYPE		occupyseatType;			//ռλ����
    VIPINDEX			enVipIndex;				//VIP����
    SCORE				lOccupySeatConsume;	    //ռλ����
    SCORE				lOccupySeatFree;	    //���ռλ�������
    SCORE				lForceStandUpCondition;	//ǿ��վ������
} OCCUPYSEATCONFIG;

typedef enum
{
    ORDINARY_BANKER,		//��ͨׯ��
    SUPERROB_BANKER,		//������ׯׯ��
    INVALID_SYSBANKER		//��Ч����(ϵͳׯ��)
} CURRENT_BANKER_TYPE;

/////////////////////////////////////////

struct tagCustomAndroid
{
    //��ׯ
	BOOL							nEnableRobotBanker;				//�Ƿ���ׯ
    LONGLONG						lRobotBankerCountMin;			//��ׯ����
    LONGLONG						lRobotBankerCountMax;			//��ׯ����
    LONGLONG						lRobotListMinCount;				//�б�����
    LONGLONG						lRobotListMaxCount;				//�б�����
    LONGLONG						lRobotApplyBanker;				//����������
    LONGLONG						lRobotWaitBanker;				//��������

    //��ע
    LONGLONG						lRobotMinBetTime;				//��ע�������
    LONGLONG						lRobotMaxBetTime;				//��ע�������
    LONGLONG						lRobotMinJetton;				//��ע������
    LONGLONG						lRobotMaxJetton;				//��ע������
    LONGLONG						lRobotBetMinCount;				//��ע��ϷAI��
    LONGLONG						lRobotBetMaxCount;				//��ע��ϷAI��
    LONGLONG						lRobotAreaLimit;				//��������

    //��ȡ��
    LONGLONG						lRobotScoreMin;					//�������
    LONGLONG						lRobotScoreMax;					//�������
    LONGLONG						lRobotBankGetMin;				//ȡ����Сֵ(��ׯ)
    LONGLONG						lRobotBankGetMax;				//ȡ�����ֵ(��ׯ)
    LONGLONG						lRobotBankGetBankerMin;			//ȡ����Сֵ(��ׯ)
    LONGLONG						lRobotBankGetBankerMax;			//ȡ�����ֵ(��ׯ)
    LONGLONG						lRobotBankStoMul;				//���ٷֱ�

    //���캯��
    tagCustomAndroid()
    {
        DefaultCustomRule();
    }

    void DefaultCustomRule()
    {
        nEnableRobotBanker = TRUE;
        lRobotBankerCountMin = 5;
        lRobotBankerCountMax = 10;
        lRobotListMinCount = 2;
        lRobotListMaxCount = 5;
        lRobotApplyBanker = 5;
        lRobotWaitBanker = 3;

        lRobotMinBetTime = 6;
        lRobotMaxBetTime = 8;
        lRobotMinJetton = 100;
		lRobotMaxJetton = 5000000;
        lRobotBetMinCount = 4;
        lRobotBetMaxCount = 8;
        lRobotAreaLimit = 10000000;

        lRobotScoreMin = 1000000;
        lRobotScoreMax = 100000000;
        lRobotBankGetMin = 100;
        lRobotBankGetMax = 30000000;
        lRobotBankGetBankerMin = 10000000;
        lRobotBankGetBankerMax = 50000000;
        lRobotBankStoMul = 50;
    }
};

//��ϷAI��Ϣ
struct tagRobotInfo
{
	LONGLONG nChip[MAX_CHIP_COUNT];											//���붨��
    int nAreaChance[AREA_MAX];										//������
    TCHAR szCfgFileName[MAX_PATH];										//�����ļ�
    int	nMaxTime;														//�������

    tagRobotInfo()
    {
		LONGLONG nTmpChip[MAX_CHIP_COUNT] = { 500, 1000, 5000, 10000, 20000, 50000, 100000 };
        int nTmpAreaChance[AREA_MAX] = {1, 1, 1, 1};

        nMaxTime = 10;
        memcpy(nChip, nTmpChip, sizeof(nChip));
        memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
    }
};

//��¼��Ϣ
struct tagServerGameRecord
{
    bool							bWinShunMen;						//˳��ʤ��
    bool							bWinDuiMen;							//����ʤ��
    bool							bWinDaoMen;							//����ʤ��
    bool							bWinHuang;							//����ʤ��
	bool							bMeJettonShunMen;					//������ע˳��
	bool							bMeJettonDuiMen;					//������ע����
	bool							bMeJettonDaoMen;					//������ע����
	bool							bMeJettonHuang;						//������ע����
};

//��ע��Ϣ
struct tagUserBet
{
    TCHAR							szNickName[32];						//�û��ǳ�
    DWORD							dwUserGameID;						//�û�ID
    LONGLONG						lUserStartScore;					//�û����
    LONGLONG						lUserWinLost;						//�û����
    LONGLONG						lUserBet[AREA_MAX + 1];			//�û���ע
};

//��ע��Ϣ����
typedef CWHArray<tagUserBet, tagUserBet &> CUserBetArray;

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_FREE				99									//��Ϸ����
#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_PLACE_JETTON			101									//�û���ע
#define SUB_S_GAME_END				102									//��Ϸ����
#define SUB_S_APPLY_BANKER			103									//����ׯ��
#define SUB_S_CHANGE_BANKER			104									//�л�ׯ��
#define SUB_S_CHANGE_USER_SCORE		105									//���»���
#define SUB_S_SEND_RECORD			106									//��Ϸ��¼
#define SUB_S_PLACE_JETTON_FAIL		107									//��עʧ��
#define SUB_S_CANCEL_BANKER			108									//ȡ������
#define SUB_S_CONTINUE_JETTON		109									//������ע
#define SUB_S_CONTINUE_JETTON_FAI	110									//������עʧ��
#define SUB_S_QURARY_ROOM_PLAYERS_INFO 119								//�������������

//���Զ����
#define SUB_S_AMDIN_COMMAND			10									//ϵͳ����
#define SUB_S_SEND_USER_BET_INFO	11									//������ע
#define SUB_S_REFRESH_RULE_RESULT	21									//ˢ�����ý��
#define SUB_S_SET_RULE_RESULT		22									//�������ý��
#define SUB_S_REFRESH_SYSCTRL		23									//����ϵͳ�����б�
#define SUB_S_REFRESH_ROOMCTRL		24									//���·�������б�
#define SUB_S_REFRESH_AREACTRL		25									//������������б�
#define SUB_S_LIST_WINLOST_INFO		26									//�б���Ӯ��Ϣ
#define SUB_S_DEBUG_TEXT			28									//������ʾ��Ϣ
#define SUB_S_CUR_ROOMCTRL_INFO		30									//��ǰ���������Ϣ
#define SUB_S_CUR_AREACTRL_INFO		31									//��ǰ���������Ϣ

#define SUB_S_ADVANCE_OPENCARD		113									//��ǰ����
#define SUB_S_SUPERROB_BANKER		114									//������ׯ
#define SUB_S_CURSUPERROB_LEAVE		115									//������ׯ����뿪
#define SUB_S_OCCUPYSEAT			116									//ռλ
#define SUB_S_OCCUPYSEAT_FAIL		117									//ռλʧ��
#define SUB_S_UPDATE_OCCUPYSEAT		118									//����ռλ
#define SUB_S_RECENTTWENTY_RECORD	119									//
#define SUB_S_RECENTWEEK_RECORD		120									//

//������ׯ
struct CMD_S_SuperRobBanker
{
    bool					bSucceed;
    WORD					wApplySuperRobUser;
    WORD					wCurSuperRobBankerUser;
};

//������ׯ����뿪
struct CMD_S_CurSuperRobLeave
{
    WORD					wCurSuperRobBankerUser;
};

//������ע
struct CMD_S_SendUserBetInfo
{
	SCORE						    lUserStartScore[GAME_PLAYER];				//��ʼ����
	SCORE							lUserJettonScore[GAME_PLAYER][AREA_MAX + 1];	//������ע
};

//ʧ�ܽṹ
struct CMD_S_PlaceJettonFail
{
    WORD							wPlaceUser;							//��ע���
    BYTE							lJettonArea;						//��ע����
    LONGLONG						lPlaceScore;						//��ǰ��ע
};

//���»���
struct CMD_S_ChangeUserScore
{
    WORD							wChairID;							//���Ӻ���
    DOUBLE							lScore;								//��һ���

    //ׯ����Ϣ
    WORD							wCurrentBankerChairID;				//��ǰׯ��
    BYTE							cbBankerTime;						//ׯ�Ҿ���
    DOUBLE							lCurrentBankerScore;				//ׯ�ҷ���
};

//����ׯ��
struct CMD_S_ApplyBanker
{
    WORD							wApplyUser;							//�������
};

//ȡ������
struct CMD_S_CancelBanker
{
    WORD							wCancelUser;					//ȡ�����
};

//�л�ׯ��
struct CMD_S_ChangeBanker
{
    WORD							wBankerUser;						//��ׯ���
    LONGLONG						lBankerScore;						//ׯ�ҽ��
    CURRENT_BANKER_TYPE				typeCurrentBanker;					//��ǰׯ������
};

//��Ϸ״̬
struct CMD_S_StatusFree
{
    //ȫ����Ϣ
    BYTE							cbTimeLeave;						//ʣ��ʱ��

    //�����Ϣ
    LONGLONG						lUserMaxScore;						//��ҽ��

    //ׯ����Ϣ
    WORD							wBankerUser;						//��ǰׯ��
    WORD							cbBankerTime;						//ׯ�Ҿ���
    LONGLONG						lBankerWinScore;					//ׯ�ҳɼ�
    LONGLONG						lBankerScore;						//ׯ�ҷ���
    INT								nEndGameMul;						//��ǰ���ưٷֱ�
    bool							bEnableSysBanker;					//ϵͳ��ׯ

    //������Ϣ
    LONGLONG						lApplyBankerCondition;				//��������
    LONGLONG						lAreaLimitScore;					//��������

    //������Ϣ
    TCHAR							szGameRoomName[SERVER_LEN];			//��������
    bool							bGenreEducate;						//��ϰģʽ

    SUPERBANKERCONFIG				superbankerConfig;					//��ׯ����
    WORD							wCurSuperRobBankerUser;
    CURRENT_BANKER_TYPE				typeCurrentBanker;					//��ǰׯ������

    OCCUPYSEATCONFIG				occupyseatConfig;							//ռλ����
    WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID

	LONGLONG						lChipArray[MAX_CHIP_COUNT];			//��������
    tagCustomAndroid				CustomAndroid;						//��ϷAI����
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
    //ȫ����ע
    LONGLONG						lAllJettonScore[AREA_MAX + 1];		//ȫ����ע

    //�����ע
    LONGLONG						lUserJettonScore[AREA_MAX + 1];		//������ע

    //��һ���
    LONGLONG						lUserMaxScore;						//�����ע

    //������Ϣ
    LONGLONG						lApplyBankerCondition;				//��������
    LONGLONG						lAreaLimitScore;					//��������

    //�˿���Ϣ
    BYTE							cbTableCardArray[5][5];				//�����˿�

    //ׯ����Ϣ
    WORD							wBankerUser;						//��ǰׯ��
    WORD							cbBankerTime;						//ׯ�Ҿ���
    LONGLONG						lBankerWinScore;					//ׯ��Ӯ��
    LONGLONG						lBankerScore;						//ׯ�ҷ���
    INT								nEndGameMul;						//��ǰ���ưٷֱ�
    bool							bEnableSysBanker;					//ϵͳ��ׯ

    //������Ϣ
    LONGLONG						lEndBankerScore;					//ׯ�ҳɼ�
    LONGLONG						lEndUserScore;						//��ҳɼ�
    LONGLONG						lEndUserReturnScore;				//���ػ���
    LONGLONG						lEndRevenue;						//��Ϸ˰��

    //ȫ����Ϣ
    BYTE							cbTimeLeave;						//ʣ��ʱ��
    BYTE							cbGameStatus;						//��Ϸ״̬

    //������Ϣ
    TCHAR							szGameRoomName[SERVER_LEN];			//��������
    bool							bGenreEducate;						//��ϰģʽ

    SUPERBANKERCONFIG				superbankerConfig;					//��ׯ����
    WORD							wCurSuperRobBankerUser;
    CURRENT_BANKER_TYPE				typeCurrentBanker;					//��ǰׯ������

    OCCUPYSEATCONFIG				occupyseatConfig;					//ռλ����
    WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID

    LONGLONG						lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];	//ռλ��ҳɼ�

	LONGLONG						lChipArray[MAX_CHIP_COUNT];			//��������
	LONGLONG						lLastUserJettonScore[AREA_MAX + 1];//�Ͼ���ע
    tagCustomAndroid				CustomAndroid;						//��ϷAI����
	BYTE							cbCardType[5];						//����
	bool							bWinFlag[5];						//��Ӯ��Ϣ
};

//��Ϸ����
struct CMD_S_GameFree
{
    BYTE							cbTimeLeave;						//ʣ��ʱ��
    INT64                           nListUserCount;						//�б�����
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
    WORD							wBankerUser;						//ׯ��λ��
    LONGLONG						lBankerScore;						//ׯ�ҽ��
    LONGLONG						lUserMaxScore;						//�ҵĽ��
    BYTE							cbTimeLeave;						//ʣ��ʱ��
    bool							bContiueCard;						//��������
    int								nChipRobotCount;					//�������� (��ע��ϷAI)
    int								nAndriodApplyCount;					//��ϷAI�б�����

    TCHAR							szServerName[LEN_SERVER];			//��������
};

//�û���ע
struct CMD_S_PlaceJetton
{
    WORD							wChairID;							//�û�λ��
    BYTE							cbJettonArea;						//��������
    LONGLONG						lJettonScore;						//��ע��Ŀ
    bool							bIsAndroid;							//�Ƿ���ϷAI
    bool							bAndroid;							//��ϷAI��ʶ
};

//������ע
struct CMD_S_ContiueJetton
{
	WORD							wChairID;							//�û�λ��
	LONGLONG						lJettonScore[AREA_MAX + 1];		//���˼�ע
};

//������עʧ��
struct CMD_S_ContiueJettonFail
{
	WORD							wChairID;							//�û�λ��
};

//��Ϸ����
struct CMD_S_GameEnd
{
    //�¾���Ϣ
    BYTE							cbTimeLeave;						//ʣ��ʱ��

    //�˿���Ϣ
    BYTE							cbTableCardArray[5][5];				//�����˿�
    BYTE							cbLeftCardCount;					//�˿���Ŀ

    BYTE							bcFirstCard;

    //ׯ����Ϣ
    LONGLONG						lBankerScore;						//ׯ�ҳɼ�
    LONGLONG						lBankerTotallScore;					//ׯ�ҳɼ�
    INT								nBankerTime;						//��ׯ����

    //��ҳɼ�
    LONGLONG						lUserScore;							//��ҳɼ�
    LONGLONG						lUserReturnScore;					//���ػ���

    //ȫ����Ϣ
    LONGLONG						lRevenue;							//��Ϸ˰��

    LONGLONG						lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];	//ռλ��ҳɼ�

	BYTE							cbCardType[5];						//����
	bool							bWinFlag[5];						//��Ӯ   
	LONGLONG							lEndOtherUserScore;					//�����ɼ�
};

//ռλ
struct CMD_S_OccupySeat
{
    WORD							wOccupySeatChairID;							//����ռλ���ID
    BYTE							cbOccupySeatIndex;							//ռλ����
    WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID
};

//ռλʧ��
struct CMD_S_OccupySeat_Fail
{
    WORD							wAlreadyOccupySeatChairID;					//������ռλ���ID
    BYTE							cbAlreadyOccupySeatIndex;					//��ռλ����
    WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID
};

//����ռλ
struct CMD_S_UpdateOccupySeat
{
    WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID
    WORD							wQuitOccupySeatChairID;						//�����˳�ռλ���
};

//ϵͳ��ʾ
struct CMD_S_SysTip
{
	TCHAR							szTip[512];							//��ʾ��Ϣ
};

//ͳ�Ƽ�¼���20��
struct CMD_S_StatisticsRecord
{
	WORD							wChairID;							//����ID
	SCORE							lRecordInAllScore;					//���20������ע
	SCORE							lWinCount;							//���20��ʤ��(0�ֲ��ۼ�ʤ��)
};

//����ظ�
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//�ظ�����
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_DEBUG 3
	BYTE cbResult;
#define CR_ACCEPT  2			//����
#define CR_REFUSAL 3			//�ܾ�
	BYTE cbExtendData[20];			//��������
};

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

//������Խ��
struct CMD_S_AreaCtrlResult
{
	BYTE							cbDebugStyle;						//���Է�ʽ
	bool							bWinArea[AREA_MAX + 1];				//Ӯ������
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	BYTE							cbCurCtrlTimes;						//��ǰ���Ծ���
	SCORE							lAreaCtrlSysWin;					//ϵͳ��Ӯ����
	SCORE							lAreaCtrlPlayerWin;					//�����Ӯ����
	CTime							tmConfigTime;						//����ʱ��
	EM_AREACTRL_STATUS				areaCtrlStatus;						//��ǰ״̬
};

//�б���Ӯ��Ϣ
struct CMD_S_ListWinLostInfo
{
	SCORE							lSysCtrlSysWin;						//ϵͳ��Ӯ����
	SCORE							lSysCtrlPlayerWin;					//�����Ӯ����
	SCORE							lRoomCtrlSysWin;					//ϵͳ��Ӯ����
	SCORE							lRoomCtrlPlayerWin;					//�����Ӯ����
	SCORE							lAreaCtrlSysWin;					//ϵͳ��Ӯ����
	SCORE							lAreaCtrlPlayerWin;					//�����Ӯ����
	CTime							tmStartRecord;						//��ʼ��¼ʱ��
};

//������ʾ
struct CMD_S_DebugText
{
	TCHAR							szMessageText[256];					//�����ı�
};

//ͳ�Ƽ�¼���20��
struct CMD_S_RecentTwentyRecord
{
	BYTE							cbPlayCount;												//����
	BYTE							cbBankerWinPercent;
	BYTE							cbPlayserWinPercent;
	BYTE							cbAreaCardType[RECENT_RECORD_COUNT][AREA_MAX + 1];		//�þָ�����������
	bool							bWinFlag[RECENT_RECORD_COUNT][AREA_MAX + 1];				//�þָ���������Ӯ
};

//ͳ�Ƽ�¼���һ��
struct CMD_S_RecentWeekRecord
{
	LONG							lGamePlayCount;
	LONG							lAreaCardWinCount[AREA_MAX + 1];		//��������Ӯ����
	LONG							lAreaCardLostCount[AREA_MAX + 1];		//�������������
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
};

//��ǰ���������Ϣ
struct CMD_S_CurAreaCtrlInfo
{
	BYTE							cbDebugStyle;						//���Է�ʽ
	bool							bWinArea[AREA_MAX + 1];				//Ӯ������
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	BYTE							cbConfigCtrlTimes;					//���õ��Ծ���
	BYTE							cbCurCtrlTimes;						//��ǰ���Ծ���
	SCORE							lAreaCtrlSysWin;					//ϵͳ��Ӯ����
	SCORE							lAreaCtrlPlayerWin;					//�����Ӯ����
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_PLACE_JETTON			1									//�û���ע
#define SUB_C_APPLY_BANKER			2									//����ׯ��
#define SUB_C_CANCEL_BANKER			3									//ȡ������
#define SUB_C_CONTINUE_CARD			4									//��������
#define SUB_C_SUPERROB_BANKER		7									//������ׯ
#define SUB_C_OCCUPYSEAT			8									//ռλ
#define SUB_C_QUIT_OCCUPYSEAT		9									//�˳�ռλ
#define SUB_C_CONTINUE_JETTON		10									//������ע
#define SUB_C_REQUEST_STATISTICS_RECORD		20							//����ͳ�Ƽ�¼���20��

//���Զ����
#define SUB_C_REFRESH_RULE					11							//ˢ������
#define SUB_C_SET_RULE						12							//��������
#define SUB_C_ROOM_CTRL						13							//�������
#define SUB_C_REFRESH_CUR_ROOMCTRL_INFO		14							//ˢ�·��������Ϣ
#define SUB_C_AMDIN_COMMAND					15							//�������
#define SUB_C_REFRESH_CUR_AREACTRL_INFO		16							//ˢ�����������Ϣ
#define SUB_C_ADVANCED_REFRESH_ALLCTRLLIST	17							//ˢ�����п��Ƽ�¼

//�ͻ�����Ϣ
#define IDM_ADMIN_COMMDN			WM_USER+1000
#define IDM_UPDATE_STORAGE			WM_USER+1001

//�û���ע
struct CMD_C_PlaceJetton
{
    BYTE							cbJettonArea;						//��������
    LONGLONG						lJettonScore;						//��ע��Ŀ
};

//ռλ
struct CMD_C_OccupySeat
{
    WORD							wOccupySeatChairID;				//ռλ���
    BYTE							cbOccupySeatIndex;				//ռλ����
};

struct tagAdminReq
{
	BYTE							cbExcuteTimes;					//ִ�д���
	BYTE							cbDebugStyle;					//���Է�ʽ
	bool							bWinArea[AREA_MAX + 1];			//Ӯ������
};

struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA	1
#define RQ_RESET_DEBUG	2
	BYTE cbExtendData[20];			//��������
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

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif