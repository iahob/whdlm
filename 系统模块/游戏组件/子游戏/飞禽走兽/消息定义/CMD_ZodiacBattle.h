#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE
// 1�ֽڶ���
#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						123									//��Ϸ I D
#define GAME_PLAYER					100									//��Ϸ����
#define GAME_NAME					TEXT("��������")						//��Ϸ����

//״̬����
#define	GS_PLACE_JETTON				GAME_STATUS_PLAY					//��ע״̬
#define	GS_GAME_END					GAME_STATUS_PLAY+1					//����״̬

#define VERSION_SERVER			    PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//����X6	����X8	��ȸX8	��ӥX12	ʨ��X12	��èX8	����X8	����X6 ����24 ����2 ����2

//��������
#define ID_TU_ZI					1									//����
#define ID_YAN_ZI					2									//����
#define ID_GE_ZI					3									//����
#define ID_KONG_QUE					4									//��ȸ
#define ID_LAO_YING					5									//��ӥ
#define ID_SHI_ZI					6									//ʨ��
#define ID_XIONG_MAO				7									//��è
#define ID_HOU_ZI					8									//����
#define ID_SHA_YU					9									//����
#define ID_FEI_QIN					10									//����		//ͨ��
#define ID_ZOU_SHOU					11									//����		//ͨɱ
#define ID_GLOD_SHA					12									//����		//����

#define AREA_MAX					12									//������Ŀ

//ϵͳ��Ӯ
#define AREA_SYSLOST				13									//�������ϵͳ��
#define AREA_SYSWIN					14									//�������ϵͳӮ

#define ANIMAL_COUNT                28                                  //�������

#define MAX_SCORE_HISTORY			65									//��ʷ����
#define	MAX_CHIP_COUNT				5									//ǰ�˳������

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
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	BYTE							cbCtrlPosIndex;						//����λ��(0~6)
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

//AI��Ϣ
struct tagRobotInfo
{
	LONGLONG nChip[MAX_CHIP_COUNT];														//���붨��
	int nAreaChance[AREA_MAX];										//������
	int	nMaxTime;														//�������

	tagRobotInfo()
	{
		LONGLONG nTmpChip[MAX_CHIP_COUNT] = { 100, 1000, 10000, 100000, 1000000 };
		int nTmpAreaChance[AREA_MAX] = {2, 2, 2, 2, 2, 2, 2, 2,2,2,2};

		nMaxTime = 10;
		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
	}
};

//��¼��Ϣ
struct tagServerGameRecord
{
	BYTE							bWinMen[AREA_MAX];						//˳��ʤ��
};

//��ע��Ϣ
struct tagUserBet
{
	TCHAR							szNickName[32];						//�û��ǳ�
	DWORD							dwUserGameID;						//�û�ID
	LONGLONG						lUserStartScore;					//�û����
	LONGLONG						lUserWinLost;						//�û����
	LONGLONG						lUserBet[AREA_MAX + 1];				//�û���ע
};

//��ע��Ϣ����
typedef CWHArray<tagUserBet, tagUserBet &> CUserBetArray;

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_FREE				299									//��Ϸ����
#define SUB_S_GAME_START			300									//��Ϸ��ʼ
#define SUB_S_PLACE_JETTON			301									//�û���ע
#define SUB_S_GAME_END				302									//��Ϸ����
#define SUB_S_APPLY_BANKER			303									//����ׯ��
#define SUB_S_CHANGE_BANKER			304									//�л�ׯ��
#define SUB_S_SEND_RECORD			306									//��Ϸ��¼
#define SUB_S_PLACE_JETTON_FAIL		307									//��עʧ��
#define SUB_S_CANCEL_BANKER			308									//ȡ������
#define	SUB_S_CLEAN_JETTON			309									//�����ע
#define SUB_S_CONTINU_JETTON		310									//������ע
#define SUB_S_SYSTIP				311									//ϵͳ��ʾ

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
	LONGLONG						lRobotBetMinCount;				//��עAI��
	LONGLONG						lRobotBetMaxCount;				//��עAI��
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
		lRobotMaxJetton = 1000000;
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

//�����ע
struct CMD_S_CleanJetton
{
	LONGLONG						lAllPlaceScore[AREA_MAX+1];		//��ǰ��ע
	WORD							wChairID;							//�û�λ��
};

struct CMD_S_ContiueJetton
{
	LONGLONG						lAllJettonScore[AREA_MAX+1];		//ȫ����ע
	LONGLONG						lUserJettonScore[AREA_MAX+1];		//������ע
	LONGLONG						lUserStartScore[GAME_PLAYER];		//��ʼ����

	WORD							wChairID;							//�û�λ��
	BYTE							cbAndroid;							//AI
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
	WORD							wCanceUser;							//ȡ�����
};

//�л�ׯ��
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//��ׯ���
	LONGLONG						lBankerScore;						//ׯ�ҽ��
};

//��Ϸ״̬
struct CMD_S_StatusFree
{
	//ȫ����Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	//�����Ϣ
	LONGLONG						lUserMaxScore;						//��ҽ��
	INT								nAnimalPercent[AREA_MAX];			//���б���
	//ׯ����Ϣ
	WORD							wBankerUser;						//��ǰׯ��
	WORD							cbBankerTime;						//ׯ�Ҿ���
	LONGLONG						lBankerWinScore;					//ׯ�ҳɼ�
	LONGLONG						lBankerScore;						//ׯ�ҷ���
	bool							bEnableSysBanker;					//ϵͳ��ׯ

	//������Ϣ
	LONGLONG						lApplyBankerCondition;				//��������
	LONGLONG						lAreaLimitScore;					//��������
	
	TCHAR							szGameRoomName[32];					//�������� 
	
	bool							bShowTrend;
	LONGLONG						lChipArray[MAX_CHIP_COUNT];			//��������
	tagCustomAndroid				CustomAndroid;						//AI����
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//ȫ����ע
	LONGLONG						lAllJettonScore[AREA_MAX+1];		//ȫ����ע

	//�����ע
	LONGLONG						lUserJettonScore[AREA_MAX+1];		//������ע

	//��һ���
	LONGLONG						lUserMaxScore;						//�����ע							
	INT								nAnimalPercent[AREA_MAX];			//���б���
	//������Ϣ
	LONGLONG						lApplyBankerCondition;				//��������
	LONGLONG						lAreaLimitScore;					//��������

	//�˿���Ϣ
	BYTE							cbTableCardArray[2];				//�����˿�

	//ׯ����Ϣ
	WORD							wBankerUser;						//��ǰׯ��
	WORD							cbBankerTime;						//ׯ�Ҿ���
	LONGLONG						lBankerWinScore;					//ׯ��Ӯ��
	LONGLONG						lBankerScore;						//ׯ�ҷ���
	bool							bEnableSysBanker;					//ϵͳ��ׯ

	//������Ϣ
	LONGLONG						lEndBankerScore;					//ׯ�ҳɼ�
	LONGLONG						lEndUserScore;						//��ҳɼ�
	LONGLONG						lEndOtherUserScore;					//������ҳɼ�(������ϵͳ���Լ�)
	LONGLONG						lEndUserReturnScore;				//���ػ���
	LONGLONG						lEndRevenue;						//��Ϸ˰��
	
	//ȫ����Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	BYTE							cbGameStatus;						//��Ϸ״̬
	TCHAR							szGameRoomName[32];					//�������� 

	bool							bShowTrend;
	LONGLONG						lChipArray[MAX_CHIP_COUNT];			//��������

	//LONGLONG						lEndOtherUserGameScore;				//������ҳɼ�(������ϵͳ���Լ�,����˰)

	tagCustomAndroid				CustomAndroid;						//AI����

};

//��Ϸ����
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//ׯ��λ��
	LONGLONG						lBankerScore;						//ׯ�ҽ��
	LONGLONG						lUserMaxScore;						//�ҵĽ��
	BYTE							cbTimeLeave;						//ʣ��ʱ��	
	bool							bContiueCard;						//��������
	int								nChipRobotCount;					//�������� (��עAI)
};

//�û���ע
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//�û�λ��
	BYTE							cbJettonArea;						//��������
	LONGLONG						lJettonScore;						//��ע��Ŀ
	BYTE							cbAndroid;							//AI
	LONGLONG						lUserStartScore[GAME_PLAYER];				//��ʼ����
	//�����ע
	LONGLONG						lUserJettonScore[AREA_MAX+1];		//������ע
};

//������ע
struct CMD_S_SendUserBetInfo
{
	LONGLONG						lUserStartScore[GAME_PLAYER];				//��ʼ����
	LONG							lUserJettonScore[GAME_PLAYER][AREA_MAX + 1];	//������ע
};

//��Ϸ����
struct CMD_S_GameEnd
{
	//�¾���Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	//�˿���Ϣ
	BYTE							cbTableCardArray[2];				//�����˿�
	BYTE                            cbShaYuAddMulti;					//���ӱ���
	//ׯ����Ϣ
	LONGLONG						lBankerScore;						//ׯ�ҳɼ�
	LONGLONG						lBankerTotallScore;					//ׯ�ҳɼ�
	INT								nBankerTime;						//��ׯ����

	//��ҳɼ�
	LONGLONG						lUserScore;							//��ҳɼ�
	LONGLONG						lOtherUserScore;					//������ҳɼ�(������ϵͳ���Լ�)
	LONGLONG						lUserReturnScore;					//���ػ���

	//ȫ����Ϣ
	LONGLONG						lRevenue;							//��Ϸ˰��
	INT								nAnimalPercent[AREA_MAX];			//���б���

	LONGLONG						lLastJetton[AREA_MAX+1];			//�Ͼ���ע

	LONGLONG						lOtherUserGameScore;				//������ҳɼ�(������ϵͳ���Լ�,����˰)
};

//���Ƽ�¼
struct CMD_S_SendRecord
{
	BYTE							GameRecordArrary[MAX_SCORE_HISTORY];
};

//ϵͳ��ʾ
struct CMD_S_SysTip
{
	TCHAR							szTip[512];							//��ʾ��Ϣ
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
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	BYTE							cbCtrlPosIndex;						//����λ��(0~6)
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
	BYTE							cbCtrlPosIndex;						//����λ��(0~6)
	DWORD							dwCtrlIndex;						//����(��1��ʼ����)
	BYTE							cbConfigCtrlTimes;					//���õ��Ծ���
	BYTE							cbCurCtrlTimes;						//��ǰ���Ծ���
	SCORE							lAreaCtrlSysWin;					//ϵͳ��Ӯ����
	SCORE							lAreaCtrlPlayerWin;					//�����Ӯ����
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_PLACE_JETTON			201									//�û���ע
#define SUB_C_APPLY_BANKER			202									//����ׯ��
#define SUB_C_CANCEL_BANKER			203									//ȡ������
#define SUB_C_CLEAN_JETTON			205									//��������
#define SUB_C_CONTINUE_JETTON		206									//������ע

//���Զ����
#define SUB_C_REFRESH_RULE					11							//ˢ������
#define SUB_C_SET_RULE						12							//��������
#define SUB_C_ROOM_CTRL						13							//�������
#define SUB_C_REFRESH_CUR_ROOMCTRL_INFO		14							//ˢ�·��������Ϣ
#define SUB_C_AMDIN_COMMAND					15							//�������
#define SUB_C_REFRESH_CUR_AREACTRL_INFO		16							//ˢ�����������Ϣ
#define SUB_C_ADVANCED_REFRESH_ALLCTRLLIST	17							//ˢ�����п��Ƽ�¼

//�û���ע
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;						//��������
	LONGLONG						lJettonScore;						//��ע��Ŀ
};

struct CMD_C_CleanMeJetton
{
	WORD							wChairID;							//�û�λ��
};

struct CMD_C_ContinueJetton
{
	WORD					wChairID;							//�û�λ��
	LONGLONG				lLastAllJettonPlace[AREA_MAX +1];	//ȫ������
	LONGLONG				lLastAllJettonArea[AREA_MAX +1];	//�������

	BYTE					cbJettonArea;						//��������
	LONGLONG				lJettonScore;						//��ע��Ŀ
	BYTE					cbAndroid;							//AI
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
// ��ԭ������
#pragma pack()
#endif
