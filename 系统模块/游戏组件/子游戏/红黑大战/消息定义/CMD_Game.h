#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

//1�ֽڶ���
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						144									//��Ϸ I D
#define GAME_NAME					TEXT("��ڴ�ս")						//��Ϸ����

#define GAME_PLAYER					MAX_CHAIR							//��Ϸ����

//�������
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//״̬����
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_PLACE_JETTON		GAME_STATUS_PLAY					//��ע״̬
#define	GAME_SCENE_GAME_END			GAME_STATUS_PLAY+1					//����״̬

#define MAX_CARD					3
//�������
#define ID_RED_AR					0									//��
#define ID_BLACK_AR					1									//��
#define ID_POKER_TYPE_AR			2									//��������
#define ID_INVALID					3									//��Ч����

#define RED_INDEX					1									//��
#define BLACK_INDEX					2									//�� 
#define POKER_TYPE_INDEX			3									//����
#define MAX_INDEX					3									//�������

#define AREA_COUNT					3
#define CONTROL_AREA				3
//������
#define RATE_PAIR					1									//����
#define RATE_SHUN					2									//˳��
#define RATE_JING					3									//��
#define RATE_SHUN_JING				5									//˳��
#define RATE_BAO_ZI					10									//����
#define MAX_RATE					10									//�����


//ռ��
#define SEAT_LEFT1_INDEX			1									//��һ
#define SEAT_LEFT2_INDEX			2									//���
#define SEAT_LEFT3_INDEX			3									//����
#define SEAT_RIGHT1_INDEX			4									//��һ
#define SEAT_RIGHT2_INDEX			5									//�Ҷ�
#define SEAT_RIGHT3_INDEX			6									//����
#define MAX_OCCUPY_SEAT_COUNT		6									//���ռλ����
#define SEAT_INVALID_INDEX			7									//��Ч����

#define SERVER_LEN					32									//���䳤��

#define IDM_UPDATE_STORAGE			WM_USER+1001

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

//������ׯ
///////////////////////////////

typedef struct
{
	SCORE				lOccupySeatFree;	    //���ռλ�������
	SCORE				lForceStandUpCondition;	//ǿ��վ������
}OCCUPYSEATCONFIG;

typedef enum
{
	ORDINARY_BANKER,		//��ͨׯ��
	INVALID_SYSBANKER		//��Ч����(ϵͳׯ��)
}CURRENT_BANKER_TYPE;
/////////////////////////////////////////

//��¼��Ϣ
struct tagServerGameRecord
{
	bool							bWinRed;							//�췽ʤ��
	bool							bWinBlack;							//�ڷ�ʤ��
	bool							bWinPoker;							//������ʤ��
	BYTE							cbPokerType;						//1����  2����(2��8) 3����(9-a) 4˳�� 5�� 6˳�� 7���� 
};

//ͳ�Ƽ�¼
struct tagStatisticsRecord
{
	SCORE							lPlayerBetScore;					//�����ע����
	SCORE							lPlayerWinScore;					//���
};

typedef CList<tagStatisticsRecord, tagStatisticsRecord&> tagStatisticsRecordList;

struct tagRoomUserInfo
{
	WORD							wChairID;
	SCORE							lBetRank20Score;
	SCORE							lBetRank20Jetton;
	int								nWinCount;
};
typedef vector<tagRoomUserInfo> vecTagRoomUserInfo;

//ͳ�Ƽ�¼
struct CMD_S_StatisticsRecord
{
	WORD							wChairID;					// �û�����ID
	SCORE							lRecordInAllScore;			// �����ʮ������ע
	SCORE							lWinCount;					// �����ʮ��ʤ����
};

struct CMD_S_SendRoomUserInfo
{
	int								nPlayerCount;					//��������Ҹ���
	WORD							wChairID[20];					//
	SCORE							lBetRank20Score[20];			//�ĺ�ǰ��ʮ�˵���Ҽ���
	SCORE							lBetRank20Jetton[20];			//�ĺ�ǰ��ʮ�˽���ʮ����ע�ܽ��
	int								nWinCount[20];					//�ĺ�ǰ��ʮ�˵Ľ���ʮ�ֵ�ʤ������
};

struct tagCustomAndroid
{
	//��ׯ
	bool							nEnableRobotBanker;				//�Ƿ���ׯ
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
	LONGLONG						lRobotBetMinCount;				//��ע��������
	LONGLONG						lRobotBetMaxCount;				//��ע��������
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
		nEnableRobotBanker = true;
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

//��������Ϣ
struct tagRobotInfo
{
	LONGLONG nChip[5];														//���붨��
	int nAreaChance[AREA_COUNT];										//������

	tagRobotInfo()
	{
		LONGLONG nTmpChip[5] = {500, 1000, 5000, 10000, 50000};
		int nTmpAreaChance[AREA_COUNT] = { 1, 1, 1 };
		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
	}
};

//��ע��Ϣ
struct tagUserBet
{
	TCHAR							szNickName[32];						//�û��ǳ�
	DWORD							dwUserGameID;						//�û�ID
	LONGLONG						lUserStartScore;					//�û����
	LONGLONG						lUserWinLost;						//�û����
	LONGLONG						lUserBet[AREA_COUNT];				//�û���ע
};

//��ע��Ϣ����
typedef CWHArray<tagUserBet,tagUserBet&> CUserBetArray;

//������
#define RQ_REFRESH_STORAGE		1
#define RQ_SET_STORAGE			2
//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_FREE				99									//��Ϸ����
#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_PLACE_JETTON			101									//�û���ע
#define SUB_S_GAME_END				102									//��Ϸ����
#define SUB_S_UP_DESK				103									//����
#define SUB_S_HISTORY_RECORD		104									//��Ϸ·����¼
#define SUB_S_QURARY_LOTTORY_INFO	105									//��ѯ������Ϣ û�õ�
#define SUB_S_SEND_RECORD			106									//��Ϸ��¼ û�õ�
#define SUB_S_PLACE_JETTON_FAIL		107									//��עʧ��
#define SUB_S_DOWN_DESK				108									//����
#define SUB_S_SEND_REJETTON			109									//��������Ƿ������ظ�Ͷ�Լ�����Ͷע���
#define SUB_S_QURARY_ROOM_PLAYERS_INFO			119						//��ѯ��������ҵ���Ϣ
#define SUB_S_CANCEL_BANKER			111									//ȡ������
#define SUB_S_APPLY_BANKER			112									//����ׯ��
#define SUB_S_CHANGE_BANKER			113									//�л�ׯ��

#define SUB_S_ADMIN_STORAGE_INFO	4	
#define SUB_S_UPDATE_STORAGE        5									//���¿�� 
#define SUB_S_AMDIN_COMMAND			8									//����Ա����
#define SUB_S_SEND_USER_BET_INFO    9									//������ע


//������ע
struct CMD_S_SendBetInfo
{
	int								iResult;						// 0 �ɹ� 1 û���������� 2 ��Ҳ���
	LONGLONG						lUserJettonScore[AREA_COUNT];	//������ע
};

//ʧ�ܽṹ
struct CMD_S_PlaceJettonFail
{
	WORD							wPlaceUser;							//��ע���
	BYTE							lJettonArea;						//��ע����
	LONGLONG						lPlaceScore;						//��ǰ��ע
	BYTE							cbReason;							//��עʧ��ԭ��
};

//����ׯ��
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;							//�������
};

//ȡ������
struct CMD_S_CancelBanker
{
	bool							bAuto;								//�Ƿ��Զ�ȡ������
	WORD							wCancelUser;						//ȡ�����
	bool							bIsAllowApplyBanker;				//�Ƿ���������ׯ
};

//�л�ׯ��
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//��ׯ���
	LONGLONG						lBankerScore;						//ׯ�ҷ���
};

struct CMD_S_GameDown
{
	WORD							wChairID;							//���������
	WORD							wDeskChairID;						//���������Ӻ�
};
//��Ϸ״̬
struct CMD_S_StatusFree
{
	bool							bIsAllowApplyBanker;					//����Ƿ���������ׯ
	//ȫ����Ϣ
	BYTE							cbTimeLeave;							//ʣ��ʱ��
	LONGLONG						lCurrentPoolScore;						//���ط���
	BYTE							cbDeskChairID[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID
	LONGLONG						lBet[5];								//����

	//ׯ����Ϣ
	WORD							wBankerUser;						//��ǰׯ��
	WORD							wBankerTime;						//ׯ�Ҿ���
	LONGLONG						lBankerWinScore;					//ׯ��Ӯ��
	LONGLONG						lBankerScore;						//ׯ�ҷ���

	//������Ϣ
	LONGLONG						lApplyBankerCondition;				//��������
	LONGLONG						lLimitScore;						//�������Ʒ���
	LONGLONG						lUserMaxScore;						//��ҵ��������ע����
	LONGLONG						lAreaLimitScore[2];					// 0 �����������  1 ������������
	//������Ϣ
	TCHAR							szGameRoomName[SERVER_LEN];			//��������
	tagCustomAndroid				CustomAndroid;						//����������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	bool							bIsAllowApplyBanker;				//����Ƿ���������ׯ
	bool							bIsJettonStatus;					//��ǰ��Ϸ�Ƿ���Ѻע״̬
	LONGLONG						lBet[5];							//����

	//��ע��
	LONGLONG						lAllJettonScore[AREA_COUNT];		//����ע
	LONGLONG						lUserJettonScore[AREA_COUNT];		//������ע
	LONGLONG						lUserMaxScore;						//�û������ע��
	LONGLONG						lApplyBankerCondition;				//��������
	LONGLONG						lLimitScore;						//�������Ʒ���
	LONGLONG						lAreaLimitScore[2];					// 0 �����������  1 ������������
	//�˿���Ϣ
	BYTE							cbTableCardArray[2][3];				//�����˿�
	LONGLONG						lStorageStart;
	//������Ϣ
	LONGLONG						lCurrentPoolScore;					//���ط���
	LONGLONG						lEndUserScore;						//��ҳɼ�
	LONGLONG						lEndUserReturnScore;				//���ػ���
	LONGLONG						lEndRevenue;						//��Ϸ˰��
	BYTE							cbDeskChairID[MAX_OCCUPY_SEAT_COUNT];	//������ڰ�ID
	//ׯ����Ϣ
	WORD							wBankerUser;						//��ǰׯ��
	WORD							wBankerTime;						//ׯ�Ҿ���
	LONGLONG						lBankerWinScore;					//ׯ��Ӯ��
	LONGLONG						lBankerScore;						//ׯ�ҷ���
	//������Ϣ
	LONGLONG						lEndBankerScore;					//ׯ�ҳɼ�
	//ȫ����Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	BYTE							cbGameStatus;						//��Ϸ״̬
	//������Ϣ
	TCHAR							szGameRoomName[SERVER_LEN];			//��������
	tagCustomAndroid				CustomAndroid;						//����������
};

//��Ϸ����
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	LONGLONG						lStorageStart;						//�����ֵ
	LONGLONG						lPoolScore;							//�ʳ���ֵ
	WORD							wCurrentBanker;						//��ǰׯ��
	int								nBankerTime;						//��ׯ����
	int								nListUserCount;						//����б�������û�õ�
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//��ǰׯ��
	int								nBankerTime;						//ׯ�Ҿ���
	LONGLONG						lBankerScore;						//ׯ�ҷ���
	LONGLONG						lStorageStart;						//�����ֵ
	LONGLONG						lCurrentPoolScore;					//���ط���
	LONGLONG						lUserMaxScore;						//��������ע
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	int								nChipRobotCount;					//�������� (��ע������)
	int								nAndriodApplyCount;						//�������б�����
};

//��Ϸ·�������ݼ�¼
struct CMD_S_GameHistoryRecord
{
	BYTE							cbRBHistory[20][6];					//20����·���Ƽ�¼
	BYTE							cbRB_WL_History[20];				//��20�ֺ��ʤ�����Ƽ�¼
	BYTE							cbPokerType[20];					//��20�ֵ��������Ƽ�¼
};

//�û���ע
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//�û�λ��
	BYTE							cbJettonArea;						//��������
	LONGLONG						lJettonScore;						//��ע��Ŀ
	BYTE							bIsAndroid;							//������ʶ
};

struct CMD_S_UpDesk
{
	WORD							wChairID;							//�û�λ��ID
	bool							bUpDeskOk;							//�����Ƿ�ɹ�
	WORD							wDeskChairID;						//�������Ӻ�
	BYTE							cbFailReason;						//����ʧ��ԭ�� 1 û�п���λ�� 2 �Ƹ���������������
};

//��Ϸ����
struct CMD_S_GameEnd
{
	//�����Ƿ񿪳����ش�
	bool							bIsOpenReward;
	//�¾���Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	//�˿���Ϣ
	BYTE							cbTableCardArray[2][3];				//�����˿�
	LONGLONG						lCurrentPoolScore;					//���ط���

	LONGLONG						lUserScore;
	LONGLONG						lUserReturnScore;
	LONGLONG						lUserScoreT;						//��ҳɼ� û�г�ȥ��ˮ��˰��ֵ
	bool							bWinRed;							//�췽ʤ��
	bool							bWinBlack;							//�ڷ�ʤ��
	bool							bWinPoker;							//������ʤ��
	BYTE							cbPokerType[2];						//����

	LONGLONG						lDeskScore[MAX_OCCUPY_SEAT_COUNT];//��������ҳɼ�
	LONGLONG						lDeskScoreT[MAX_OCCUPY_SEAT_COUNT];//��������ҳɼ� û�г�ȥ��ˮ��˰��
	//ׯ����Ϣ
	WORD							wCurrentBanker;						//��ǰׯ��
	LONGLONG						lBankerScore;						//ׯ�ҳɼ�
	LONGLONG						lBankerTotallScore;					//ׯ�ҳɼ�
	INT								nBankerTime;						//��ׯ����
};

//����ظ�
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//�ظ�����
#define ANSWER_SET_WIN_AREA  111
#define ANSWER_CANCEL_CONTROL 112
	BYTE cbResult;
#define CR_ACCEPT  2				//����
#define CR_REFUSAL 3				//�ܾ�
	BYTE cbExtendData[20];			//��������
};

struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG							lRoomStorageStart;						//������ʼ���
	LONGLONG							lRoomStorageCurrent;
	LONGLONG							lRoomStorageDeduct;
	LONGLONG							lMaxRoomStorage[2];
	WORD								wRoomStorageMul[2];
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_PLACE_JETTON			1									//�û���ע
#define SUB_C_APPLY_DESK			2									//��������
#define SUB_C_QURARY_LOTTORY_INFO	3									//��ѯ������Ϣ  û�õ�
#define SUB_C_CLICK_REJETTON		4									//��ѡ�ظ���Ͷ
#define SUB_C_QURARY_RECORD			5									//�û������¼��Ϣ
#define SUB_C_QURARY_ROOM_USERS_INFO	6								//��ѯ�����е��û���Ϣ
#define SUB_C_APPLY_BANKER			7									//����ׯ��
#define SUB_C_CANCEL_BANKER			8									//ȡ������

#define SUB_C_UPDATE_STORAGE		9
#define SUB_C_STORAGEMAXMUL			10
#define SUB_C_REFRESH_ROOM_STORAGESTATUS			11
#define SUB_C_AMDIN_COMMAND			12									//���¿��


struct CMD_C_UpdateStorage
{
	LONGLONG							lRoomStorageCurrent;				//�����ֵ
	LONGLONG							lRoomStorageDeduct;					//�����ֵ
};

struct CMD_C_ModifyStorage
{
	LONGLONG							lMaxRoomStorage[2];					//�������
	WORD								wRoomStorageMul[2];					//Ӯ�ָ���
};

struct tagAdminReq
{
	BYTE							m_cbExcuteTimes;					//ִ�д���
	bool							m_bWinArea[3];						//Ӯ������
};

struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA		11
#define RQ_RESET_CONTROL	21
	BYTE cbExtendData[20];			//��������
};


//�û���ע
struct CMD_C_PlaceJetton
{
	WORD							wUserChairID;						//�û�λ��ID
	BYTE							cbJettonArea;						//��������
	LONGLONG						lJettonScore;						//��ע��Ŀ
};

struct CMD_C_ApplyDesk
{
	WORD							wUserChairID;	
	bool							bApply;
	WORD							wDeskChairID;
};

struct CMD_C_UserReJetton
{
	WORD							wUserChairID;
};

//��ԭ������
#pragma pack()
//////////////////////////////////////////////////////////////////////////

#endif
