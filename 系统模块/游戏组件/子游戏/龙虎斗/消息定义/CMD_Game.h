#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						145									//��Ϸ I D
#define GAME_NAME					TEXT("������")					//��Ϸ����

//�������
#define GAME_PLAYER					MAX_CHAIR							//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//��������
#define	MAX_CHIP_COUNT				6									//ǰ�˳������
#define	MAX_SIMPLE_CHART_COUNT		20									//���״̬�����20��ʤ�����   
#define	MAX_ADVANCED_CHART_COUNT	100									//����100�����

#define	MAX_FREEBET_OCCUPY_SEAT		3									//��������û��Ѻע�߳�ռλ
#define	STATISTICS_RECORD_COUNT		20									//ͳ�����20��
#define	MAX_BANKERCOUNT_PERSONALROOM	5								//Լս�����ׯ��������

//////////////////////////////////////////////////////////////////////////////////

//״̬����
#define	GAME_SCENE_FREE				GAME_STATUS_FREE
#define GAME_SCENE_PLACE_JETTON		GAME_STATUS_PLAY					//�ȴ���ʼ
#define GAME_SCENE_GAME_END			GAME_STATUS_PLAY+1					//��Ϸ����
//////////////////////////////////////////////////////////////////////////

//������multiple
#define MULTIPLE_LONG				1									//�� ����
#define MULTIPLE_PING				8									//ƽ ����
#define MULTIPLE_HU					1									//�� ����

//��������area
#define AREA_LONG					0									//�� ����
#define AREA_PING					1									//ƽ ����
#define AREA_HU						2									//�� ����
#define AREA_MAX					3									//�ϼ�����

//ϵͳ��Ӯ
#define AREA_SYSLOST				3									//�������ϵͳ��
#define AREA_SYSWIN					4									//�������ϵͳӮ

//������
#define RESULT_WIN					1									//Ӯ
#define RESULT_LOSE					-1									//��

//ռλ
#define SEAT_LEFT1_INDEX			0											//��һ
#define SEAT_LEFT2_INDEX			1											//���
#define SEAT_LEFT3_INDEX			2											//����
#define SEAT_RIGHT1_INDEX			3											//��һ
#define SEAT_RIGHT2_INDEX			4											//�Ҷ�
#define SEAT_RIGHT3_INDEX			5											//����
#define MAX_OCCUPY_SEAT_COUNT		6											//���ռλ����

//������
#define MAX_AREA_TIMES				8											//���������
#define RANDVAL(a, b)				(a >= b ? b : ((rand() % (b - a + 1) + a)))

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
	AREACTRL_PROGRESSING = 0,
	AREACTRL_FINISH,
	AREACTRL_CANCEL,				//δִ�й��ڶ�����ȡ��
	AREACTRL_EXECUTE_CANCEL,		//ִ�й�ȡ��
}EM_AREACTRL_STATUS;

typedef enum
{
	SYSWINLOST_CTRL = 0,
	AREA_CTRL,
	ROOM_CTRL,
	SYS_CTRL
}EM_CTRL_TYPE;

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
	SCORE							lSysCtrlChouMosgoldStorage;			//��ȡ�󽱿��
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

//�������
typedef struct
{
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	BYTE							cbCtrlPosIndex;						//����λ��(0~6)
	BYTE							cbConfigCtrlTimes;					//���õ��Ծ���
	BYTE							cbCurCtrlTimes;						//��ǰ���Ծ���
	SCORE							lAreaCtrlSysWin;					//�������ϵͳ����Ӯ
	CTime							tmConfigTime;						//����ʱ��
	EM_AREACTRL_STATUS				areaCtrlStatus;						//��ǰ״̬
	DWORD							dwDebugUserGameID;					//�������GameID
}AREACTRL;

//��¼��Ϣ
struct tagServerGameRecord
{
	BYTE							cbLong;								//����
	BYTE							cbHu;								//����
};

//��ע��Ϣ
struct tagUserBet
{
	TCHAR							szNickName[32];						//�û��ǳ�
	DWORD							dwUserGameID;						//�û�ID
    SCORE						    lUserStartScore;					//�û���ʼ���
	SCORE							lUserScore;							//�û���ǰ���
	SCORE						    lUserInsure;						//�û�����
    SCORE						    lUserWinLost;						//�û���������Ӯ
	SCORE						    lUserHistoryWinLost;				//�û���ʷ����Ӯ
    SCORE						    lUserBet[AREA_MAX];					//�û���ע
	bool							bFocusUser;							//��ע��ʶ
};

//��ע��Ϣ����
typedef CWHArray<tagUserBet, tagUserBet &> CUserBetArray;

//��ע�������
typedef CWHArray<DWORD, DWORD &> CUserFocusArray;

//��������Ϊ�߼��Ż�(ͨ��)
///////////////////////////////////////////////////////////////////////////////

#define AI_OCCUPYSEAT_CONFIGCOUNT				3
#define AI_REQUESTBANKER_CONFIGCOUNT			3
#define AI_BANKOPERATE_CONFIGCOUNT				8
#define AI_PLACEJETTON_CONFIGCOUNT				5

//������ռλ
struct tagAIOccupySeatConfig
{
    //��Χ����
    //��ά�±�0Ϊ��Сֵ��������ΪINVALID_WORD
    DWORD			wTableRealPlayerRange[AI_OCCUPYSEAT_CONFIGCOUNT][2];			//��ǰ������ʵ��ҷ�Χ
    DWORD			wAIOccupySeatCountRange[AI_OCCUPYSEAT_CONFIGCOUNT][2];			//������ռλ������Χ
    DWORD			wAIOccupySeatChange[AI_OCCUPYSEAT_CONFIGCOUNT];					//������ռλ����

    //ͨ������(����)
    DWORD			wAIOccupySeatTurnCountMin;										//ռλ������С(�ﵽ�˳�)
    DWORD			wAIOccupySeatTurnCountMax;										//ռλ�������(�ﵽ�˳�)

    //Ĭ��ֵ
    tagAIOccupySeatConfig()
    {
        DefaultConfig();
    }

    void DefaultConfig()
    {
        wTableRealPlayerRange[0][0] = 100;
        wTableRealPlayerRange[0][1] = INVALID_WORD;
        wTableRealPlayerRange[1][0] = 10;
        wTableRealPlayerRange[1][1] = 100;
        wTableRealPlayerRange[2][0] = 0;
        wTableRealPlayerRange[2][1] = 10;

        wAIOccupySeatCountRange[0][0] = 1;
        wAIOccupySeatCountRange[0][1] = 1;
        wAIOccupySeatCountRange[1][0] = 1;
        wAIOccupySeatCountRange[1][1] = 1;
        wAIOccupySeatCountRange[2][0] = 1;
        wAIOccupySeatCountRange[2][1] = 2;

        wAIOccupySeatChange[0] = 50;
        wAIOccupySeatChange[1] = 50;
        wAIOccupySeatChange[2] = 50;

        wAIOccupySeatTurnCountMin = 10;
        wAIOccupySeatTurnCountMax = 15;
    }
};

//��������ׯ
struct tagAIRequestBankerConfig
{
    //��Χ����
    //��ά�±�0Ϊ��Сֵ��������ΪINVALID_WORD
    DWORD			wTableRealPlayerRange[AI_REQUESTBANKER_CONFIGCOUNT][2];			//��ǰ������ʵ��ҷ�Χ
    DWORD			wAIRequestBankerCountRange[AI_REQUESTBANKER_CONFIGCOUNT][2];	//��������ׯ������Χ
    DWORD			wAIRequestBankerChange[AI_REQUESTBANKER_CONFIGCOUNT];			//��������ׯ����

    //ͨ������(ȡ������)
    DWORD			wAICRChange;													//����ׯ�б��еĻ������и���ȡ������
    DWORD			wAICRChangeContainRealPlayer;									//(������ʵ�������ׯ�б�)�������и���ȡ������
    DWORD			wAICRChangeFreeMin;												//ȡ��������ׯ�Ļ�������N�����ڲ����ٴ�����

    //ͨ������(������ׯ)
    DWORD			wAIQBChange;													//��ׯ����
    DWORD			wAIQBChangeFreeMin;												//������ׯ�Ļ�������N�����ڲ����ٴ�����

    //ͨ������(ǿ����ׯ)
    LONGLONG		lAILostScoreLimitQB;											//����ʱǿ����ׯ
    DWORD			wMaxSerialTurn;													//������ׯN��ʱǿ����ׯ

    tagAIRequestBankerConfig()
    {
        DefaultConfig();
    }

    void DefaultConfig()
    {
        wTableRealPlayerRange[0][0] = 100;
        wTableRealPlayerRange[0][1] = INVALID_WORD;
        wTableRealPlayerRange[1][0] = 10;
        wTableRealPlayerRange[1][1] = 100;
        wTableRealPlayerRange[2][0] = 0;
        wTableRealPlayerRange[2][1] = 10;

        wAIRequestBankerCountRange[0][0] = 1;
        wAIRequestBankerCountRange[0][1] = 2;
        wAIRequestBankerCountRange[1][0] = 1;
        wAIRequestBankerCountRange[1][1] = 4;
        wAIRequestBankerCountRange[2][0] = 1;
        wAIRequestBankerCountRange[2][1] = 6;

        wAIRequestBankerChange[0] = 50;
        wAIRequestBankerChange[1] = 50;
        wAIRequestBankerChange[2] = 50;

        wAICRChange = 8;
        wAICRChangeContainRealPlayer = 25;
        wAICRChangeFreeMin = 2;

        wAIQBChange = 5;
        wAIQBChangeFreeMin = 2;

        lAILostScoreLimitQB = 100000;
        wMaxSerialTurn = 10;
    }
};

//�����˴�ȡ
struct tagAIBankOperateConfig
{
    //���жϴ����ж�ȡ
    DWORD			wVerifyTurn;													//������ÿ��N��ɨ��
    LONGLONG		lAICarryScoreRange[AI_BANKOPERATE_CONFIGCOUNT][2];				//������Я����Χ
    BYTE			cbSave[AI_BANKOPERATE_CONFIGCOUNT];								//TRUE��,FALSEȡ
    DWORD			wAIBankOperateChange[AI_BANKOPERATE_CONFIGCOUNT];				//�����˲�������
    LONGLONG		lAIBankOperateScoreRange[AI_BANKOPERATE_CONFIGCOUNT][2];		//�����˲�����ҷ�Χ

    tagAIBankOperateConfig()
    {
        DefaultConfig();
    }

    void DefaultConfig()
    {
        wVerifyTurn = 5;

        lAICarryScoreRange[0][0] = 1;
        lAICarryScoreRange[0][1] = 100;
        lAICarryScoreRange[1][0] = 100;
        lAICarryScoreRange[1][1] = 500;
        lAICarryScoreRange[2][0] = 500;
        lAICarryScoreRange[2][1] = 1000;
        lAICarryScoreRange[3][0] = 1000;
        lAICarryScoreRange[3][1] = INVALID_DWORD;

        lAICarryScoreRange[4][0] = 1;
        lAICarryScoreRange[4][1] = 100;
        lAICarryScoreRange[5][0] = 100;
        lAICarryScoreRange[5][1] = 500;
        lAICarryScoreRange[6][0] = 500;
        lAICarryScoreRange[6][1] = 1000;
        lAICarryScoreRange[7][0] = 1000;
        lAICarryScoreRange[7][1] = INVALID_DWORD;

        cbSave[0] = FALSE;
        cbSave[1] = FALSE;
        cbSave[2] = FALSE;
        cbSave[3] = FALSE;
        cbSave[4] = TRUE;
        cbSave[5] = TRUE;
        cbSave[6] = TRUE;
        cbSave[7] = TRUE;

        wAIBankOperateChange[0] = 100;
        wAIBankOperateChange[1] = 100;
        wAIBankOperateChange[2] = 100;
        wAIBankOperateChange[3] = 100;
        wAIBankOperateChange[4] = 100;
        wAIBankOperateChange[5] = 100;
        wAIBankOperateChange[6] = 100;
        wAIBankOperateChange[7] = 100;

        lAIBankOperateScoreRange[0][0] = 500;
        lAIBankOperateScoreRange[0][1] = 900;
        lAIBankOperateScoreRange[1][0] = 100;
        lAIBankOperateScoreRange[1][1] = 500;
        lAIBankOperateScoreRange[2][0] = 1;
        lAIBankOperateScoreRange[2][1] = 100;
        lAIBankOperateScoreRange[3][0] = 0;
        lAIBankOperateScoreRange[3][1] = 50;

        lAIBankOperateScoreRange[4][0] = 0;
        lAIBankOperateScoreRange[4][1] = 50;
        lAIBankOperateScoreRange[5][0] = 1;
        lAIBankOperateScoreRange[5][1] = 100;
        lAIBankOperateScoreRange[6][0] = 100;
        lAIBankOperateScoreRange[6][1] = 500;
        lAIBankOperateScoreRange[7][0] = 500;
        lAIBankOperateScoreRange[7][1] = 900;
    }
};

//������Ѻע
struct tagAIPlaceJettonConfig
{
    DWORD			wCountDownPerRange[AI_PLACEJETTON_CONFIGCOUNT][2];				//����ʱ�ٷֱ�
    DWORD			wPlaceJettonLimitRange[AI_PLACEJETTON_CONFIGCOUNT][2];			//��ע���ֵ
    DWORD			wPlaceJettonRate[MAX_CHIP_COUNT];								//����Ȩ��

    LONGLONG		lAIWinLostLimitTimes;											//AI��Ӯ������Χ(������С����N��)

    tagAIPlaceJettonConfig()
    {
        DefaultConfig();
    }

    void DefaultConfig()
    {
        wCountDownPerRange[0][0] = 1;
        wCountDownPerRange[0][1] = 20;
        wCountDownPerRange[1][0] = 20;
        wCountDownPerRange[1][1] = 40;
        wCountDownPerRange[2][0] = 40;
        wCountDownPerRange[2][1] = 60;
        wCountDownPerRange[3][0] = 60;
        wCountDownPerRange[3][1] = 80;
        wCountDownPerRange[4][0] = 80;
        wCountDownPerRange[4][1] = 100;

        wPlaceJettonLimitRange[0][0] = 70;
        wPlaceJettonLimitRange[0][1] = 90;
        wPlaceJettonLimitRange[1][0] = 70;
        wPlaceJettonLimitRange[1][1] = 90;
        wPlaceJettonLimitRange[2][0] = 70;
        wPlaceJettonLimitRange[2][1] = 90;
        wPlaceJettonLimitRange[3][0] = 80;
        wPlaceJettonLimitRange[3][1] = 99;
        wPlaceJettonLimitRange[4][0] = 80;
        wPlaceJettonLimitRange[4][1] = 99;

        wPlaceJettonRate[0] = 40;
        wPlaceJettonRate[1] = 35;
        wPlaceJettonRate[2] = 25;
        wPlaceJettonRate[3] = 10;
        wPlaceJettonRate[4] = 10;
        wPlaceJettonRate[5] = 5;

        lAIWinLostLimitTimes = 600;
    }
};

struct tagAIConfig
{
    tagAIOccupySeatConfig			AIOccupySeatConfig;
    tagAIRequestBankerConfig		AIRequestBankerConfig;
    tagAIBankOperateConfig			AIBankOperateConfig;
    tagAIPlaceJettonConfig			AIPlaceJettonConfig;

    tagAIConfig()
    {
        DefaultAIConfig();
    }

    void DefaultAIConfig()
    {
        AIOccupySeatConfig.DefaultConfig();
        AIRequestBankerConfig.DefaultConfig();
        AIBankOperateConfig.DefaultConfig();
        AIPlaceJettonConfig.DefaultConfig();
    }
};

///////////////////////////////////////////////////////////////////////////////
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
    SCORE							lRobotMinJetton;				//��ע������
    SCORE							lRobotMaxJetton;				//��ע������
    LONGLONG						lRobotBetMinCount;				//��ע��������
    LONGLONG						lRobotBetMaxCount;				//��ע��������
    LONGLONG						lRobotAreaLimit;				//��������

    //��ȡ��
    LONGLONG						lRobotScoreMin;					//�������
    LONGLONG						lRobotScoreMax;					//�������
    LONGLONG						lRobotBankGetMin;				//ȡ����Сֵ
    LONGLONG						lRobotBankGetMax;				//ȡ�����ֵ
    LONGLONG						lRobotBankStoMul;				//���ٷֱ�

    //ռλ����
    BOOL							nEnableRobotOccupySeat;			//�Ƿ�ռλ
    LONGLONG						lRobotOccupySeatTableMin;		//ռλ��������
    LONGLONG						lRobotOccupySeatTableMax;		//ռλ�������
    LONGLONG						lRobotQuitSeatTurnMin;			//�˳�ռλ��������
    LONGLONG						lRobotQuitSeatTurnMax;			//�˳�ռλ�������

    tagAIConfig						CustomAIConfig;					//Ĭ��AICONFIG
    SCORE							dMinTableScore;					//��ͻ���(�����˴�ȡ�����)

    //���캯��
    tagCustomAndroid()
    {
        PrimaryDefaultCustomRule();
        dMinTableScore = 1000;
    }

    void PrimaryDefaultCustomRule()
    {
        nEnableRobotBanker = TRUE;
        lRobotBankerCountMin = 5;
        lRobotBankerCountMax = 10;
        lRobotListMinCount = 2;
        lRobotListMaxCount = 5;
        lRobotApplyBanker = 5;
        lRobotWaitBanker = 3;

        lRobotMinBetTime = 50;
        lRobotMaxBetTime = 80;
        lRobotMinJetton = 1;
        lRobotMaxJetton = 5000;
        lRobotBetMinCount = 80;
        lRobotBetMaxCount = MAX_CHAIR;
        lRobotAreaLimit = 10000;

        lRobotScoreMin = 100;
        lRobotScoreMax = 100000;
        lRobotBankGetMin = 1000;
        lRobotBankGetMax = 30000;
        lRobotBankStoMul = 50;

        nEnableRobotOccupySeat = TRUE;
        lRobotOccupySeatTableMin = 2;
        lRobotOccupySeatTableMax = 4;
        lRobotQuitSeatTurnMin = 3;
        lRobotQuitSeatTurnMax = 10;
    }

    void MidDefaultCustomRule()
    {
        nEnableRobotBanker = TRUE;
        lRobotBankerCountMin = 5;
        lRobotBankerCountMax = 10;
        lRobotListMinCount = 2;
        lRobotListMaxCount = 5;
        lRobotApplyBanker = 5;
        lRobotWaitBanker = 3;

        lRobotMinBetTime = 50;
        lRobotMaxBetTime = 80;
        lRobotMinJetton = 1;
        lRobotMaxJetton = 5000;
        lRobotBetMinCount = 80;
        lRobotBetMaxCount = MAX_CHAIR;
        lRobotAreaLimit = 10000;

        lRobotScoreMin = 100;
        lRobotScoreMax = 100000;
        lRobotBankGetMin = 1000;
        lRobotBankGetMax = 30000;
        lRobotBankStoMul = 50;

        nEnableRobotOccupySeat = TRUE;
        lRobotOccupySeatTableMin = 2;
        lRobotOccupySeatTableMax = 4;
        lRobotQuitSeatTurnMin = 3;
        lRobotQuitSeatTurnMax = 10;
    }

    void SeniorDefaultCustomRule()
    {
        nEnableRobotBanker = TRUE;
        lRobotBankerCountMin = 5;
        lRobotBankerCountMax = 10;
        lRobotListMinCount = 2;
        lRobotListMaxCount = 5;
        lRobotApplyBanker = 5;
        lRobotWaitBanker = 3;

        lRobotMinBetTime = 50;
        lRobotMaxBetTime = 80;
        lRobotMinJetton = 1;
        lRobotMaxJetton = 5000;
        lRobotBetMinCount = 80;
        lRobotBetMaxCount = MAX_CHAIR;
        lRobotAreaLimit = 10000;

        lRobotScoreMin = 100;
        lRobotScoreMax = 100000;
        lRobotBankGetMin = 1000;
        lRobotBankGetMax = 30000;
        lRobotBankStoMul = 50;

        nEnableRobotOccupySeat = TRUE;
        lRobotOccupySeatTableMin = 2;
        lRobotOccupySeatTableMax = 4;
        lRobotQuitSeatTurnMin = 3;
        lRobotQuitSeatTurnMax = 10;
    }
};



//ͳ�Ƽ�¼���20��
struct tagStatisticsRecord
{
	bool							bBeBanker;							//�þ���ׯ��ʶ
	SCORE							lInAllScore;						//�þ�����ע
	SCORE							lWinScore;							//�þ��ܵ÷�(0�ֲ��ۼ�ʤ��)
};

//����״̬
struct CMD_S_StatusFree
{
	//��Ϸ����
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	SCORE							lUserMaxScore;						//��������ע���
	LONGLONG						lAreaLimitScore;					//��������
	SCORE							lChipArray[MAX_CHIP_COUNT];			//��������

	//��ׯ��ׯ��
	bool							bEnableBanker;						//������ׯ���ܱ�־(��Ϊfalse���йر������Ժ���)
	WORD							wBankerUser;						//��ǰׯ��
	WORD							wBankerTime;						//ׯ�Ҿ���
	SCORE							lBankerWinScore;					//ׯ��Ӯ��
	SCORE							lBankerScore;						//ׯ��Я�����
	bool							bEnableSysBanker;					//����ϵͳ��ׯ��־
	LONGLONG						lApplyBankerCondition;				//��������

	//ռλ
	LONGLONG						lOccupySeatFree;					//���ռλ�������
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID

	//����������
	tagCustomAndroid				CustomAndroid;						//����������
	bool							bIsGroupPersonalRoom;				//�����˽��Լս��ʶ
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��Ϸ����
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	BYTE							cbGameStatus;						//��Ϸ״̬
	SCORE							lUserMaxScore;						//��������ע���		
	LONGLONG						lAreaLimitScore;					//��������
	SCORE							lChipArray[MAX_CHIP_COUNT];			//��������

	//�˿���Ϣ
	BYTE							cbTableCardArray[2];				//�����˿�

	//��ע
	SCORE							lAreaInAllScore[AREA_MAX];			//ÿ���������ע				
	SCORE							lUserInAllScore[AREA_MAX];			//��ǰ���ÿ���������ע
					
	//��ׯ��ׯ��
	bool							bEnableBanker;						//������ׯ���ܱ�־(��Ϊfalse���йر������Ժ���)
	WORD							wBankerUser;						//��ǰׯ��
	WORD							wBankerTime;						//ׯ�Ҿ���
	SCORE							lBankerScore;						//ׯ��Я�����
	bool							bEnableSysBanker;					//����ϵͳ��ׯ��־
	LONGLONG						lApplyBankerCondition;				//��������

	//ռλ
	LONGLONG						lOccupySeatFree;					//���ռλ�������
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID

	//������Ϣ
	SCORE							lEndUserScore;								//��ҳɼ�
	SCORE							lBankerWinScore;							//ׯ�ҳɼ�
	SCORE							lUserAllScore[GAME_PLAYER][AREA_MAX];		//ÿ�����ÿ���������ע
	SCORE							lUserWinScore[GAME_PLAYER];					//��ҳɼ�
	SCORE							lUserAreaWinScore[GAME_PLAYER][AREA_MAX];	//ÿ�����ÿ������ĵ÷�
	SCORE							lOtherUserInAllScore[AREA_MAX];				//��ȥׯ�Һ�ռ����Һ��Լ�ÿ���������ע
	SCORE							lOtherUserAreaWinScore[AREA_MAX];			//��ȥׯ�Һ�ռ����Һ��Լ�ÿ������ĵ÷�(����˰)
	SCORE							lUserRevenue[GAME_PLAYER];					//���˰��
	//����������
	tagCustomAndroid				CustomAndroid;								//����������
	bool							bIsGroupPersonalRoom;						//�����˽��Լս��ʶ
};

//////////////////////////////////////////////////////////////////////////////////
//�����-������
#define SUB_S_GAME_FREE				99									//��Ϸ����
#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_PLACE_JETTON			101									//�û���ע
#define SUB_S_PLACE_JETTON_FAIL		102									//��עʧ��
#define SUB_S_CONTINU_JETTON		103									//��Ͷ
#define SUB_S_CONTINUE_JETTON_FAIL	104									//��Ͷʧ��
#define SUB_S_GAME_END				105									//��Ϸ����
#define SUB_S_SEND_SIMPLE_CHART		106									//���·��
#define SUB_S_SEND_ADVANCED_CHART	107									//�߼�·��	
#define SUB_S_OCCUPYSEAT			108									//ռλ���
#define SUB_S_OCCUPYSEAT_FAIL		109									//ռλʧ��
#define SUB_S_UPDATE_OCCUPYSEAT		110									//����ռλ
#define SUB_S_OCCUPYSEAT_STANDUP	111									//ռλ����(��������û��Ѻע)

#define SUB_S_APPLY_BANKER			112									//����ׯ��
#define SUB_S_CANCEL_BANKER			113									//ȡ������
#define SUB_S_CHANGE_BANKER			114									//�л�ׯ��
#define SUB_S_BANKER_QUIT_RESULT	115									//ׯ������ע���߽�����������ׯ
#define SUB_S_SYSTIP				116									//ϵͳ��ʾ
#define SUB_S_STATISTICS_RECORD		117									//ͳ�Ƽ�¼���20��
#define SUB_S_FREE_AICONFIG			118									//��Ϸ����

//���ƶ����
#define SUB_S_SEND_ROOM_USER_INFO			10									//�����û���Ϣ
#define SUB_S_SEND_USER_BET_INFO			11									//������ע
#define SUB_S_CLEAR_USER_BET_INFO			12									//�����ע(����Ϣ��)
#define SUB_S_STATISTIC_SERVICE				13									//ͳ�Ƽ������������
#define SUB_S_ANCHOU						14									//����
#define SUB_S_MOSGOLD						15									//��

#define SUB_S_CUR_SYSCTRL_INFO				21									//ˢ�����ý��
#define SUB_S_CUR_ROOMCTRL_INFO				22									//��ǰ���������Ϣ
#define SUB_S_LIST_CUR_ROOMCTRL_INFO		23									//��ǰ�����б���Ϣ
#define SUB_S_CUR_AREACTRL_INFO				24									//��ǰ���������Ϣ
#define SUB_S_REFRESH_SYSCTRL_TRACELOG		31									//����ϵͳ�����б�
#define SUB_S_REFRESH_ROOMCTRL_TRACELOG		32									//���·�������б�
#define SUB_S_REFRESH_AREACTRL_TRACELOG		33									//������������б�
#define SUB_S_STARTRECORD_TRACELOG			34									//��ʼ��¼ʱ��
#define SUB_S_DEBUGTEXT_GENERAL				35									//������ʾ��Ϣ(GENERAL)
#define SUB_S_DEBUGTEXT_ADVANCED			36									//������ʾ��Ϣ(ADVANCED)

//AI���Ե��Զ����
#define SUB_S_QUERY_AICONFIG_DEBUG_RESULT	40									//AI���Խ��

//��Ϸ����
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;							//ʣ��ʱ��
	int                             nListUserCount;							//�б�����
};

//����״̬
struct CMD_S_GameFreeAIConfig
{
	//����������
	tagAIConfig						CustomAIConfig;
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	WORD							wBankerUser;							//ׯ��λ��
	SCORE							lBankerScore;							//ׯ��Я�����
	SCORE							lUserMaxScore;							//��������ע���	
	BYTE							cbTimeLeave;							//ʣ��ʱ��
	int								nChipRobotCount;						//�����ע��������Ŀ
};

//�û���ע
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//�û�λ��
	BYTE							cbJettonArea;						//��������
	SCORE							lJettonScore;						//��ע��Ŀ
	BYTE							cbAndroid;							//������
};

//ʧ�ܽṹ
struct CMD_S_PlaceJettonFail
{
	WORD							wPlaceUser;							//��ע���
	BYTE							lJettonArea;						//��ע����
	SCORE							lPlaceScore;						//��ǰ��ע
};

//��Ͷ
struct CMD_S_ContiueJetton
{
	SCORE							lAreaInAllScore[AREA_MAX];			//ÿ�����������ע			
	SCORE							lCurUserInAllScore[AREA_MAX];		//��ǰ���ÿ���������ע
	WORD							wPlaceUser;							//��Ͷ���
};

//��Ϸ����
struct CMD_S_GameEnd
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	//�˿���Ϣ
	BYTE							cbTableCardArray[2];				//�����˿�

	//ׯ����Ϣ
	WORD							wBankerTime;						//ׯ�Ҿ���

	//��ҳɼ�
	SCORE							lUserScore;									//��ҳɼ�
	SCORE							lBankerWinScore;							//ׯ�ҳɼ�
	SCORE							lUserInAllScore[GAME_PLAYER][AREA_MAX];		//ÿ�����ÿ���������ע
	SCORE							lUserWinScore[GAME_PLAYER];					//��ҳɼ�
	SCORE							lUserAreaWinScore[GAME_PLAYER][AREA_MAX];	//ÿ�����ÿ������ĵ÷�

	SCORE							lOtherUserInAllScore[AREA_MAX];				//��ȥׯ�Һ�ռ����Һ��Լ�ÿ���������ע
	SCORE							lOtherUserAreaWinScore[AREA_MAX];			//��ȥׯ�Һ�ռ����Һ��Լ�ÿ������ĵ÷�(����˰)
	SCORE							lUserRevenue[GAME_PLAYER];					//���˰��
};

//���·��
struct CMD_S_SimpleChart
{
	tagServerGameRecord				ServerGameRecordArray[MAX_SIMPLE_CHART_COUNT];
	BYTE							cbGameStatus;
};

//�߼�·��
struct CMD_S_AdvancedChart
{
	tagServerGameRecord				ServerGameRecordArray[MAX_ADVANCED_CHART_COUNT];
	BYTE							cbGameStatus;
};

//ռλ���
struct CMD_S_OccupySeat
{
	WORD							wOccupySeatChairID;							//����ռλ���ID
	BYTE							cbOccupySeatIndex;							//ռλ����
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID
};

//ռλʧ��
struct CMD_S_OccupySeat_Fail
{
	bool							bSeatBusy;									//��λ���˱�ʶ
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

//ռλ����
struct CMD_S_OccupySeatStandUp
{
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID
	WORD							wStandUpOccupySeatChairID;						//�˳�ռλ���
};

//����ׯ��
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;								//�������
};

//ȡ������
struct CMD_S_CancelBanker
{
	WORD							wCancelUser;							//ȡ�����
};

//�л�ׯ��
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;							//��ǰׯ��
	SCORE							lBankerScore;							//ׯ��Я�����
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
	LONGLONG						lWinCount;							//���20��ʤ��(0�ֲ��ۼ�ʤ��)
};

//���ƶ����
//////////////////////////////////////////////////////////////////////////////////

//������ע
struct CMD_S_SendUserBetInfo
{
	DWORD							dwCurAllScoreGameID;					//��ǰ��ע���GAMEID(INVALID_DWORDΪ����)
	SCORE							lUserStartScore[GAME_PLAYER];			//��ʼ����
	SCORE							lUserJettonScore[GAME_PLAYER][AREA_MAX];//������ע
	SCORE							lAreaInAllScore[AREA_MAX];				//ÿ�����������ע(������)	
	SCORE							lAreaRealPlayerInAllScore[AREA_MAX];	//ÿ�����������ע(����)	
	WORD							wOnLineAICount;							//���߻�����
	WORD							wOnLineRealPlayerCount;					//��������
	WORD							wInScoreAICount;						//��������ע����
	WORD							wInScoreRealPlayerCount;				//������ע����
};

//ͳ�Ƽ������������
struct CMD_S_StatisticService
{
	SCORE							lStatisticSysWin;						//ͳ��(����ͨ�ÿ��,����,���������ϵͳ��Ӯ�Ͱ����)
	SCORE							lStatisticService;						//�����������
	SCORE							lAreaLimitScore;						//��������
	WORD							wChairCount;							//������Ŀ
};

//����
struct CMD_S_AnChou
{
	SCORE							lCurAnChouRate;							//�������
	SCORE							lCurAnChouTotal;						//��������
};

//��
struct CMD_S_Mosgold
{
	SCORE							lCurMosgoldStorage;						//�󽱿��
	SCORE							lCurMosgoldRate;						//�󽱳�ȡ����
	SCORE							lCurMosgoldDispatchCon;					//�ɽ����
	SCORE							lCurMosgoldDispatchRate;				//�ɽ�����
	SCORE							lCurMosgoldStorageRate;					//�󽱿�����
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

//��ǰ���������Ϣ
struct CMD_S_CurAreaCtrlInfo
{
	BYTE							cbCtrlPosIndex;						//����λ��(0~6)
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	BYTE							cbConfigCtrlTimes;					//���õ��Ծ���
	BYTE							cbCurCtrlTimes;						//��ǰ���Ծ���
	SCORE							lAreaCtrlSysWin;					//ϵͳ����Ӯ
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

//�������TraceLog
struct CMD_S_AreaCtrlTraceLog
{
	AREACTRL						AreaCtrlTraceLog;
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

//�����û���Ϣ
struct CMD_S_RoomUserInfo
{
	DWORD							dwGameID[GAME_PLAYER];					//��ЧΪINVALID_DWORD
	TCHAR							szNickName[GAME_PLAYER][LEN_NICKNAME];	//��ЧΪ0
};

//AI���Ե��Զ����
//////////////////////////////////////////////////////////////////////////////////

//����AI����
struct CMD_S_AIConfigDebugResult
{
	BYTE							cbType;									//TRUEΪ��ѯ��FALSEΪ����
	WORD							wKindID;
	WORD							wServerID;
	tagAIConfig						AIConfigDebugResult;
};

//////////////////////////////////////////////////////////////////////////////////
//�����-�ͻ���
#define SUB_C_PLACE_JETTON			1									//�û���ע
#define SUB_C_CONTINUE_JETTON		2									//��Ͷ
#define SUB_C_OCCUPYSEAT			3									//ռλ
#define SUB_C_QUIT_OCCUPYSEAT		4									//�˳�ռλ
#define SUB_C_APPLY_BANKER			5									//����ׯ��
#define SUB_C_CANCEL_BANKER			6									//ȡ������
#define SUB_C_REQUEST_STATISTICS_RECORD		7							//����ͳ�Ƽ�¼���20��

//���ƶ����
#define SUB_C_ANCHOU							11							//���ð���
#define SUB_C_MOSGOLD							12							//���ô�

#define SUB_C_SYS_CTRL							13							//����ϵͳ����
#define SUB_C_REFRESH_CUR_SYSCTRL_INFO			14							//ˢ��ϵͳ������Ϣ
#define SUB_C_ROOM_CTRL							15							//���÷������
#define SUB_C_REFRESH_CUR_ROOMCTRL_INFO			16							//ˢ�·��������Ϣ
#define SUB_C_REFRESH_LIST_CUR_ROOMCTRL_INFO	17							//ˢ�µ�ǰ�����б���Ϣ
#define SUB_C_AREA_CTRL							18							//�����������
#define SUB_C_REFRESH_CUR_AREACTRL_INFO			19							//ˢ�����������Ϣ

#define SUB_C_REFRESH_STATISTIC_SERVICE			20							//ˢ��ͳ��
#define SUB_C_REFRESH_ANCHOU_MOSGOLD			21							//ˢ�°����
#define SUB_C_REFRESH_TRACELOG					22							//ˢ����־��¼

//AI���Ե��Զ����
#define SUB_C_QUERY_AICONFIG_DEBUG				27							//��ѯAI����(�����ݰ�)
#define SUB_C_SAVE_AICONFIG_DEBUG				28							//����AI����

//�û���ע
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;							//��������
	SCORE							lJettonScore;							//��ע��Ŀ
};

//ռλ
struct CMD_C_OccupySeat
{
	WORD							wOccupySeatChairID;						//ռλ���
	BYTE							cbOccupySeatIndex;						//ռλ����
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

//�������
struct CMD_C_AreaCtrl
{
	BYTE							dwCtrlIndex;							//��������
	BYTE							cbCtrlPosIndex;							//����λ��(0~6)
	BYTE							cbConfigCtrlTimes;						//���õ��Ծ���
	bool							bCancelAreaCtrl;						//ȡ�����Ա�ʶ(trueΪȡ����falseΪ���õ���)
};

//���ð���
struct CMD_C_AnChou
{
	SCORE							lConfigAnChouRate;						//�������
};

//���ô�
struct CMD_C_Mosgold
{
	SCORE							lConfigMosgoldRate;						//�������
	SCORE							lConfigMosgoldDispatchCon;				//�ɽ����(Ĭ��Ϊ0)
	SCORE							lConfigMosgoldDispatchRate;				//�ɽ�����
	SCORE							lConfigMosgoldStorageRate;				//�󽱿�����
};

//AI���Ե��Զ����
//////////////////////////////////////////////////////////////////////////////////

//����AI����
struct CMD_C_SaveAIConfigDebug
{
	//����������
	tagAIConfig						SaveAIConfigDebug;
};

//////////////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif