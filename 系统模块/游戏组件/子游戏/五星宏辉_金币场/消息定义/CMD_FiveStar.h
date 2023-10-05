#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��FiveStarServer.dll   FiveStar.exe
#define								SCORE								LONGLONG
#define KIND_ID						147									//��Ϸ I D
#define GAME_PLAYER					200									//��Ϸ����
#define GAME_NAME					TEXT("���Ǻ��")				    //��Ϸ����

//�������
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//״̬����
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define	GS_PLACE_JETTON				GAME_STATUS_PLAY					//��ע״̬
#define	GAME_SCENE_END				GAME_STATUS_PLAY+1					//����״̬

#define MAX_GAMECOUNT				100									//������
#define LU_DAN_COUNT_SMALL          48                                  //С·������
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

// ���ǻ�ɫ��������
enum enArea
{
	enAreaBlack, //����0
	enAreaRed,   //����1
	enAreaFlower,//�ݻ�2
	enAreaSquare,//��Ƭ3
	enAreaKing,  //С��4
	enAreaCount
};

const long double fBeiPoint[enAreaCount] = {3.8,3.8,4.0,4.0,20.0};
//������ׯ
///////////////////////////////

//������ׯ����
typedef enum
{
	SUPERBANKER_VIPTYPE,
	SUPERBANKER_CONSUMETYPE
}SUPERBANKERTYPE;

//ռλ����
typedef enum
{
	OCCUPYSEAT_VIPTYPE,
	OCCUPYSEAT_CONSUMETYPE,
	OCCUPYSEAT_FREETYPE
}OCCUPYSEATTYPE;

typedef enum
{
	VIP1_INDEX = 1,  //cbMemberOrder = 1 �����Ա
	VIP2_INDEX,		 //�����Ա
	VIP3_INDEX,		 //�����Ա
	VIP4_INDEX,		 //�����Ա
	VIP5_INDEX,		 //VIP��Ա
	VIP_INVALID		 //��Ч
}VIPINDEX;

typedef struct
{
	SUPERBANKERTYPE		superbankerType;		//��ׯ����
	VIPINDEX			enVipIndex;				//VIP����
	SCORE				lSuperBankerConsume;	//��ׯ����
}SUPERBANKERCONFIG;

typedef struct
{
	OCCUPYSEATTYPE		occupyseatType;			//ռλ����
	VIPINDEX			enVipIndex;				//VIP����
	SCORE				lOccupySeatConsume;	    //ռλ����
	SCORE				lOccupySeatFree;	    //���ռλ�������
	SCORE				lForceStandUpCondition;	//ǿ��վ������
}OCCUPYSEATCONFIG;

typedef enum
{
	ORDINARY_BANKER,		//��ͨׯ��
	SUPERROB_BANKER,		//������ׯׯ��
	INVALID_SYSBANKER		//��Ч����(ϵͳׯ��)
}CURRENT_BANKER_TYPE;

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
	LONGLONG						lRobotMinTotalJetton;			//��С����ע������
	LONGLONG						lRobotMaxTotalJetton;			//�������ע������
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
		lRobotMinTotalJetton = 100000;			    //��С����ע������
		lRobotMaxTotalJetton = 2147483647;			//�������ע������
		lRobotBankGetMin = 100;
		lRobotBankGetMax = 30000000;
		lRobotBankGetBankerMin = 10000000;
		lRobotBankGetBankerMax = 50000000;
		lRobotBankStoMul = 50;
	}
};

//��ע��Ϣ
struct tagUserBet
{
	TCHAR							szNickName[32];						//�û��ǳ�
	DWORD							dwUserGameID;						//�û�ID
	LONGLONG						lUserStartScore;					//�û����
	LONGLONG						lUserWinLost;						//�û����
	LONGLONG						lUserBet[enAreaCount];				//�û���ע
};
//��ע��Ϣ����
typedef CWHArray<tagUserBet, tagUserBet&> CUserBetArray;
//������
#define RQ_REFRESH_STORAGE		1
#define RQ_SET_STORAGE			2
//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_FREE				100									//��Ϸ����
#define SUB_S_GAME_START			101									//��Ϸ��ʼ
#define SUB_S_USERPOINT				102									//�û���ע
#define SUB_S_GAME_END				103									//��Ϸ����
#define SUB_S_APPLY_BANKER          104                                 //����ׯ��
#define SUB_S_CHANGE_BANKER         105                                 //�л�ׯ��
#define SUB_S_CANCEL_BANKER         106                                 //ȡ������
#define SUB_S_PLACE_JETTON_FAIL     107                                 //��עʧ��
#define SUB_S_HORN_INFO             108                                 //�󽱹㲥
#define SUB_S_CANCEL_JETTON         109                                 //ȡ����ע
#define SUB_S_AMDIN_COMMAND			110									//����Ա����
#define SUB_S_UPDATE_STORAGE        111									//���¿��
#define SUB_S_SEND_USER_BET_INFO    112									//������ע��Ϣ

//�û���ע
struct  CMD_S_PlaceBet 
{
	WORD  wChairID;														//��ע�û�
	BYTE  cbArea;														//��ע����
	SCORE lBetScore;													//��ע���
};
//����״̬
struct CMD_S_StatusFree
{
	//ȫ����Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	//ׯ����Ϣ
	WORD                            wBankerUser;						//��ǰׯ��
	SCORE                           lBankerScore;						//ׯ�ҷ���
	SCORE                           lBankerWinScore;					//ׯ�ҳɼ�
	WORD                            wBankerTime;						//ׯ�Ҿ���
	SCORE                           lApplyBankerCondition;				//����ׯ������

	//������Ϣ
	SCORE                           lAreaLimitScore;					//��������
	SCORE                           lUserLimitScore;					//��������
	SCORE                           lChipCount[5];						//ע��

	//��·����Ϣ100
	BYTE							cbBitCount;							//����
	BYTE							cbHistroyRecord[MAX_GAMECOUNT];		//��ʷ��¼

	//С·����Ϣ48
	BYTE                            cbLuDanSmallCount;					//С·������
	BYTE                            cbLuDanSmallData[LU_DAN_COUNT_SMALL];//С·���˿�

	//ͳ����Ϣ
	BYTE							cbSmallCount;						//����
	int                             nOpenColorCount[enAreaCount];       //��ɫͳ��
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��Ϸ��Ϣ
	BYTE							cbTimeLeave;                        //ʣ��ʱ��  
	BYTE                            cbGameStatus;                       //��Ϸ״̬
	//ׯ����Ϣ
	WORD                            wBankerUser;						//��ǰׯ��
	SCORE                           lBankerScore;						//ׯ�ҷ���
	SCORE                           lBankerWinScore;					//ׯ�ҳɼ�
	WORD                            wBankerTime;						//ׯ�Ҿ���
	SCORE                           lApplyBankerCondition;				//����ׯ������
	//������Ϣ
	SCORE                           lAreaLimitScore;					//��������
	SCORE                           lUserLimitScore;					//��������
	SCORE                           lChipCount[5];						//ע��
	//��·����Ϣ100
	BYTE							cbBitCount;							//����
	BYTE							cbHistroyRecord[MAX_GAMECOUNT];		//��ʷ��¼
	//С·����Ϣ48
	BYTE                            cbLuDanSmallCount;					//С·������
	BYTE                            cbLuDanSmallData[LU_DAN_COUNT_SMALL];//С·���˿�
	//ͳ����Ϣ
	BYTE							cbSmallCount;						//��Ϸ����
	int                             nOpenColorCount[enAreaCount];       //��ɫͳ��
	//�˿���Ϣ
	BYTE							cbCardValue;						//�����˿�
	//��ע��
	SCORE      						lAreaScore[enAreaCount];			//���������µ�ע
	SCORE      						lMeScore[enAreaCount];				//�Լ������µ�ע

	//�û���Ϣ
	SCORE    						lEndUserScore;						//����Ӯ 
	//��ҳɼ� 
	SCORE    						lEndUserReturnScore;				//��ҳɼ� 
	SCORE      						lEndRevenue;						//��Ϸ˰��
};

//��Ϸ����
struct CMD_S_GameFree
{
	//ȫ����Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	//ͳ����Ϣ
	BYTE							cbSmallCount;						//��Ϸ����
	int                             nOpenColorCount[enAreaCount];       //��ɫͳ��
	//��·����Ϣ100
	BYTE							cbBitCount;							//����
	BYTE							cbHistroyRecord[MAX_GAMECOUNT];		//��ʷ��¼
	//С·����Ϣ48
	BYTE                            cbLuDanSmallCount;					//С·������
	BYTE                            cbLuDanSmallData[LU_DAN_COUNT_SMALL];//С·���˿�

    //�û���Ϣ
	LONGLONG                        lAllTurnScore;						//�ֵܾ÷�
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	WORD                            wBankerUser;						//ׯ��λ��
	SCORE                           lBankerScore;						//ׯ�ҽ��
	//AIʹ��
	int  							cbBigCount;							//��ǰ�ڼ���
	int  							cbSmallCount;						//��ǰ�ڼ���
	int                             cbCardHeiTao;                       //���ҵĳ��ֻ���     
};

//��Ϸ����
struct CMD_S_GameEnd 
{
	//ȫ����Ϣ
	BYTE                             cbTimeLeave;                            //ʣ��ʱ��
	//�˿���Ϣ
	BYTE                             cbCurrentOpenCard;                      //�����˿�
	//ׯ����Ϣ
	SCORE                            lBankerScore;                           //ׯ�ұ��ֳɼ�
	SCORE                            lBankerWinScore;                        //ׯ���ܳɼ�
	int                              nBankerTime;                            //��ׯ����
	//��ҳɼ�
	SCORE                            lGameScore[GAME_PLAYER];                 //������ҳɼ�
	SCORE                            lRevenue;                                //��Ϸ˰��
};
//��עʧ��
struct CMD_S_PlaceBetFail
{
	WORD                             wChairID;                            //��ע���
	BYTE                             cbBetArea;                           //��ע����
	SCORE                            lPlaceScore;                         //��ǰ��ע
	BYTE                             cbReason;                            //ʧ��ԭ��
};
//���¿��
struct CMD_S_UpdateStorage
{
	BYTE                            cbReqType;						//��������
	LONGLONG						lStorageStart;				    //��ʼ���
	LONGLONG						lStorageDeduct;				    //���˥��
	LONGLONG						lStorageCurrent;			    //��ǰ���
	LONGLONG						lStorageMax1;					//�������1
	LONGLONG						lStorageMul1;					//ϵͳ��ָ���1
	LONGLONG						lStorageMax2;					//�������2
	LONGLONG						lStorageMul2;					//ϵͳ��ָ���2
};
//������ע
struct CMD_S_SendUserBetInfo
{
	LONGLONG						lUserStartScore[GAME_PLAYER];	//��ʼ����
	LONGLONG						lUserJettonScore[enAreaCount][GAME_PLAYER];//������ע
};

//ȡ����ע
struct CMD_S_ClearBet
{
	WORD        wChairID;                     //˭ȡ����
	LONGLONG	lUserJettonScore[enAreaCount];//5������ֱ�ȡ������
};

//����ظ�
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//�ظ�����
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3
	BYTE cbResult;
#define CR_ACCEPT  2			    //����
#define CR_REFUSAL 3			    //�ܾ�
	BYTE cbExtendData[20];			//��������
};


//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_SET_POINT				1									//�û���ע
#define SUB_C_APPLY_BANKER          2                                   //����ׯ��
#define SUB_C_CANCEL_BANKER         3                                   //ȡ������
#define SUB_C_CLEAR_JETTON          4                                   //ȡ����ע
#define SUB_C_AMDIN_COMMAND			5									//����Ա����
#define SUB_C_LEAVE					6									//�û��뿪
#define SUB_C_UPDATE_STORAGE        7									//���¿��

struct CMD_C_PlaceBet
{
	BYTE                            cbBetArea;							//��������
	SCORE                           lBetScore;							//��ע��Ŀ
};
//ȡ����ע
struct CMD_C_ClearBet
{
       WORD cbBetArea; //��������
};

//�ͻ�����Ϣ
#define IDM_ADMIN_COMMDN			WM_USER+1000
#define IDM_UPDATE_STORAGE			WM_USER+1001

//����������Ϣ
struct tagControlInfo
{
	BYTE cbControlArea[enAreaCount];			//��������
};

struct tagAdminReq
{
	BYTE							m_cbExcuteTimes;					//ִ�д���
	BYTE							m_cbControlStyle;					//���Ʒ�ʽ
#define		CS_BANKER_LOSE    1
#define		CS_BANKER_WIN	  2
#define		CS_BET_AREA		  3
	bool							m_bWinArea[enAreaCount];			//Ӯ������
};


struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA		1
#define RQ_RESET_CONTROL	2
#define RQ_PRINT_SYN		3
	BYTE cbExtendData[20];			//��������
};
//�û���ע
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;						//��������
	LONGLONG						lJettonScore;						//��ע��Ŀ
};
//���¿��
struct CMD_C_UpdateStorage
{
	BYTE                            cbReqType;						//��������
	LONGLONG						lStorageDeduct;					//���˥��
	LONGLONG						lStorageCurrent;				//��ǰ���
	LONGLONG						lStorageMax1;					//�������1
	LONGLONG						lStorageMul1;					//ϵͳ��ָ���1
	LONGLONG						lStorageMax2;					//�������2
	LONGLONG						lStorageMul2;					//ϵͳ��ָ���2
};

#pragma pack()

//////////////////////////////////////////////////////////////////////////

#endif
