#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

#pragma  pack(1)

#include "windows.h"

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						614								//��Ϸ I D
#define GAME_PLAYER					100								//��Ϸ����
#define GAME_NAME					TEXT("3Dɭ������")				//��Ϸ����
#define CLIENT_MODULE_NAME			TEXT("3DSLWG.exe")				//�ͻ���ģ������
#define	SERVER_MODULE_NAME			TEXT("3DSLWGServer.DLL")		//�����ģ������

//״̬����
#define	GS_PLACE_JETTON				GAME_STATUS_PLAY				//��ע״̬
#define	GS_GAME_END					GAME_STATUS_PLAY - 1			//����״̬

#define VERSION_SERVER			    PROCESS_VERSION(7,0,1)			//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)			//����汾

//////////////////////////////////////////////////////////////////////////
// ��Ϸ����
enum eGambleType
{
	eGambleType_Invalid = -1,
	eGambleType_AnimalGame = 0,	//3d����
	eGambleType_EnjoyGame,		//С��Ϸ��ׯ�к�
	eGambleType_Max,
};

//�������Ͷ���
enum eAnimalType
{
	eAnimalType_Invalid	= -1,
	eAnimalType_Lion	= 0,
	eAnimalType_Panda,
	eAnimalType_Monkey,
	eAnimalType_Rabbit,

	eAnimalType_Max, //���ֵ
};

//��ɫ���Ͷ���
enum eColorType
{
	eColorType_Invalid		= -1,
	eColorType_Red		= 0,
	eColorType_Green,
	eColorType_Yellow,

	eColorType_Max, //���ֵ
};

//����ģʽ�Ŀ���ģʽ
enum eAnimalPrizeMode
{
	eAnimalPrizeMode_Invalid = -1,
	eAnimalPrizeMode_SingleAnimalSingleColor = 0,
	eAnimalPrizeMode_AllAnimalSingleColr,
	eAnimalPrizeMode_AllColorSingleAnimal,
	eAnimalPrizeMode_SysPrize,
	eAnimalPrizeMode_RepeatTime,
	eAnimalPrizeMode_Flash,

	eAnimalPrizeMode_Max,
};

//ׯ�к���Ϸ����
enum eEnjoyGameType
{
	eEnjoyGameType_Invalid = -1,
	eEnjoyGameType_Zhuang = 0,
	eEnjoyGameType_Xian,
	eEnjoyGameType_He,

	eEnjoyGameType_Max, //���ֵ
};

// ��������
typedef struct tagSTAnimalInfo
{
	eAnimalType				eAnimal;					//����
	eColorType				eColor;						//��ɫ

}STAnimalInfo;

//��������
typedef struct tagSTAnimalAtt
{
	STAnimalInfo stAnimal;//��������
	UINT32  dwMul;//���￪������
	UINT64  qwJettonLimit;//������ע�������
}STAnimalAtt;

// ׯ�к�����
typedef struct tagSTEnjoyGameInfo
{
	eEnjoyGameType eEnjoyGame;//ׯ�к�����
	UINT32 dwMul;//����
	//UINT64  qwJetton;
	UINT64  qwJettonLimit;//��ע�������
}STEnjoyGameAtt;

//������Ϣ
typedef struct tagSTPrizeInfo
{
	eGambleType eGamble;//��������
	STAnimalInfo stWinAnimal;//��������
	STEnjoyGameAtt stEnjoyGame;//����ׯ�к�
}STPrizeInfo;

//���￪����Ϣ
typedef struct tagSTAnimalPrize
{
	STAnimalInfo stAnimalInfo;
	eAnimalPrizeMode ePrizeMode;

	/*
	��prizemode=eAnimalPrizeMode_SysPrizeʱ��qwFlag��ʾ��������ϵͳ�ʽ�
	��prizemode=eAnimalPrizeMode_RepeatTimeʱ��qwFlag��ʾ�ظ�����
	��prizemode=eAnimalPrizeMode_Flashʱ��qwFlag��ʾϵͳ����
	*/
	UINT64 qwFlag;

	//��repeat�£������ٿ��Ķ����б�,���20��
	STAnimalInfo arrstRepeatModePrize[20];
}STAnimalPrize;

//ׯ�кͿ�����Ϣ
typedef struct tagSTEnjoyGamePrizeInfo
{
	eEnjoyGameType ePrizeGameType;
}STEnjoyGamePrizeInfo;

//////////////////////////////////////////////////////////////////////////
//�������
#define ID_BIG_TIGER			0									//������
#define ID_SMALL_TIGER			1									//С������
#define ID_BIG_DOG				2									//������
#define ID_SMALL_DOG			3									//С������
#define ID_BIG_HORSE			4									//��������
#define ID_SMALL_HORSE			5									//С������
#define ID_BIG_SNAKE			6									//��������
#define ID_SMALL_SNAKE			7									//С������


#define	PATH_STEP_NUMBER		32									//ת�������
#define JETTON_AREA_COUNT		8									//��ע��������

//��¼��Ϣ
struct tagServerGameRecord
{
	__int64 	cbGameTimes;										//�ڼ���
	//UINT		cbRecord;											//��Χ��ID_BIG_TIGER��ID_SMALL_SNAKE
	STAnimalPrize stAnimalPrize;
	STEnjoyGamePrizeInfo stEnjoyGamePrizeInfo;
};

struct CMD_S_Status_Base
{
	//��ʶ����
	bool	bAllowApplyBanker;									//������ׯ

	//ʱ�����	
	BYTE		cbFreeTimeCount;									//����ʱ��
	BYTE		cbBetTimeCount;										//��עʱ��
	BYTE		cbDrawTimeCount;									//����ʱ��
	BYTE		cbPayOutTimeCount;									//�ɲ�ʱ��

	//�Ƕȱ���
	int		nAnimalRotateAngle;									//���̽Ƕ�
	int		nPointerRatateAngle;								//ָ��Ƕ�

	//·����Ϣ
	BYTE	cbRouteListCount;									//·����Ŀ
	int		dwRouteListData[8];									//·������

	//�ʵ���Ϣ
	BYTE	cbColorLightIndexList[24];							//�ʵ�����

	//������Ϣ
	BYTE	cbBetItemRatioList[12];								//�����б�

	//�ⶥ����
	LONGLONG	lItemBetTopLimit[12];								//��ע�ⶥ
	LONGLONG	lUserItemBetTopLimit;								//Ͷע�ⶥ
	LONGLONG	lUserTotalBetTopLimit;								//Ͷע�ⶥ

	//��ׯ��Ϣ
	BYTE	cbBankerListMaxItem;								//��ׯ����
	BYTE	cbApplyBankerCount;									//��������
	BYTE	cbBankerKeepCount;									//��ׯ����
	BYTE	cbCurrBankerKeepCount;								//��ׯ����
	WORD	wBankerChairID;										//ׯ�ҷ�λ
	WORD	wApplyBankerList[GAME_PLAYER];						//�����б�
	LONGLONG	lApplyBankerScore;									//��ׯ����
	LONGLONG	lBankerGrade;										//ׯ�ҳɼ�

	//��ע����
	LONGLONG	lBetTotalCount[12];										//��ע���
};


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

#define SUB_S_CLEAR_JETTON			109									//��עʧ��
#define SUB_S_SEND_INFO				110
#define SUB_S_ACK_UPDATE_STORAGE	111
#define SUB_S_ACK_SET_PRIZE			112
#define SUB_S_GAME_OPTION			113									//��Ϸ����

#define SUB_S_GAME_START_CLIENT		150									//��Ϸ��ʼ
#define SUB_S_SEND_RECORD_CLIENT	156									//��Ϸ��¼


//added by sam ����������
struct CMD_S_Control
{
	LONGLONG lStorageScore;
	DWORD	 nStorageDeduct;
	UINT64 animalJettonScore[eAnimalType_Max][eColorType_Max];
	UINT64 enjoyGameJettonScore[eEnjoyGameType_Max];
	TCHAR userNickName[GAME_PLAYER][32];
};

struct CMD_S_ACK_Control
{
	TCHAR msg[1024];
};

struct CMD_S_Game_Option
{
	LONGLONG							lMinTableScore;						//��ͻ���
	LONGLONG							lMinEnterScore;						//��ͻ���
	LONGLONG							lMaxEnterScore;						//��߻���
};

struct CMD_S_ClearJetton
{
	// 0: �ɹ���1��ʧ��
	UINT32			dwErrorCode;
};

//ʧ�ܽṹ  ��עʧ�� �� CMD_S_PlaceJettonһ����
//dwErrorCode:˵����
//1�����ֲ���
//2: �ﵽ������ע����
//3: ������עʱ��
struct CMD_S_PlaceJettonFail
{
	eGambleType eGamble;//����
	STAnimalInfo stAnimalInfo;//�����ע��Ϣ
	eEnjoyGameType eEnjoyGameInfo;//ׯ�кͼ�ע��Ϣ
	UINT64			iPlaceJettonScore;				    //��ǰ��ע
	UINT32			dwErrorCode;
	//BYTE							lJettonArea;						//��ע����
	//__int64							iPlaceScore;				    //��ǰ��ע
};

struct CMD_BANKER_INFO
{
	TCHAR							szBankerName[32];					//ׯ���
	TCHAR							szBankerAccounts[32];
	DWORD							dwUserID;							//�û�ID
	__int64							iBankerScore;						//ׯ���
	UINT32							wBankCount;							//��ׯ����
};

////���»��֣���������
//struct CMD_S_ChangeUserScore
//{
//	WORD							wChairID;							//���Ӻ���
//	__int64							iScore;								//��һ���
//
//	//ׯ����Ϣ
//	WORD							wCurrentBankerChairID;				//��ǰׯ��
//	BYTE							cbBankerTime;						//ׯ�Ҿ���
//	__int64							iCurrentBankerScore;				//ׯ�ҷ���
//};

//����ׯ��
struct CMD_S_ApplyBanker
{
	DWORD							wApplyUser;							//�������
};

//ȡ������
struct CMD_S_CancelBanker
{
	TCHAR							szCancelUser[32];					//ȡ�����
	DWORD							dwUserID;					        //ȡ�����ID
};

//�л�ׯ��
struct CMD_S_ChangeBanker
{
	DWORD							wBankerUser;						//��ׯ���
	__int64							iBankerScore;						//ׯ�ҽ��
};

//��Ϸ״̬
//struct CMD_S_StatusFree
//{
//	//ȫ����Ϣ
//	BYTE							cbTimeLeave;						//ʣ��ʱ��
//
//	//�����Ϣ
//	__int64							iUserMaxScore;						//��ҽ��
//
//	//ׯ����Ϣ
//	WORD							wBankerUser;						//��ǰׯ��
//	WORD							cbBankerTime;						//ׯ�Ҿ���
//	__int64							iBankerWinScore;					//ׯ�ҳɼ�
//	__int64							iBankerScore;						//ׯ�ҷ���
//	bool							bEnableSysBanker;					//ϵͳ��ׯ
//
//	//������Ϣ
//	__int64							iApplyBankerCondition;				//��������
//	__int64							iAreaLimitScore[JETTON_AREA_COUNT];	//��������
//};

struct CMD_S_StatusFree
{
	__int64							iUserScore;						//�ҵĽ��

	//ȫ����Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	// ��ע������Ϣ
	STAnimalAtt					    arrSTAnimalJettonLimit[eAnimalType_Max][eColorType_Max]; //��������
	UINT32							arrColorRate[eColorType_Max];//��ɫ�ֲ�����
	STEnjoyGameAtt					arrSTEnjoyGameJettonLimit[eEnjoyGameType_Max];//ׯ�к�����

	CMD_BANKER_INFO					stBankerInfo;						//ׯ����Ϣ
};

//��Ϸ״̬
//struct CMD_S_StatusPlay
//{
//	//ȫ����ע
//	__int64								iTotalAreaScore[JETTON_AREA_COUNT];
//
//	//�����ע
//	__int64								iUserAreaScore[JETTON_AREA_COUNT];
//
//	//��������ע
//	__int64								iAreaScoreLimit[JETTON_AREA_COUNT];		
//
//	//��һ���
//	__int64							iUserMaxScore;					//�����ע							
//
//	//������Ϣ
//	__int64							iApplyBankerCondition;			//��������
//
//	//�˿���Ϣ
// 	BYTE							cbCardCount[2];					//�˿���Ŀ
//	BYTE							cbTableCardArray[2][3];			//�����˿�
//
//	//ׯ����Ϣ
//	WORD							wBankerUser;					//��ǰׯ��
//	WORD							cbBankerTime;					//ׯ�Ҿ���
//	__int64							iBankerWinScore;				//ׯ��Ӯ��
//	__int64							iBankerScore;					//ׯ�ҷ���
//	bool							bEnableSysBanker;				//ϵͳ��ׯ
//
//	//������Ϣ
//	__int64							iEndBankerScore;				//ׯ�ҳɼ�
//	__int64							iEndUserScore;					//��ҳɼ�
//	__int64							iEndUserReturnScore;			//���ػ���
//	__int64							iEndRevenue;					//��Ϸ˰��
//
//	//ȫ����Ϣ
//	BYTE							cbTimeLeave;					//ʣ��ʱ��
//	BYTE							cbGameStatus;					//��Ϸ״̬
//};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	__int64							iUserScore;						//�ҵĽ��

	BYTE							cbTimeLeave;						//ʣ��ʱ��
	BYTE							cbGameStatus;					//��Ϸ״̬


	// ������Ϣ
	STAnimalAtt					    arrSTAnimalAtt[eAnimalType_Max][eColorType_Max]; //��������
	UINT32							arrColorRate[eColorType_Max];			//��ɫ�ֲ�����
	STEnjoyGameAtt					arrSTEnjoyGameAtt[eEnjoyGameType_Max];	//ׯ�к�����

	//������Ϣ
	STAnimalPrize				    stWinAnimal;						//��������
	STEnjoyGamePrizeInfo			stWinEnjoyGameType;					//����ׯ�к�

	CMD_BANKER_INFO					stBankerInfo;						//ׯ����Ϣ
};

//��Ϸ����
struct CMD_S_GameFree
{
	__int64							iUserScore;						//�ҵĽ��
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	//BYTE							cbGameRecord;						//���ο����Ľ��
	//STAnimalPrize stAnimalPrize;
	//STEnjoyGamePrizeInfo stEnjoyGamePrizeInfo;
	UINT64							qwGameTimes;						   //��ǰ����Ϸ���������ĵڼ���

	CMD_BANKER_INFO					stBankerInfo;						//ׯ����Ϣ
	BYTE							cbCanCancelBank;					//�Ƿ����������ׯ��0�� ������ׯ��1������ׯ��

};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	__int64							iUserScore;						//�ҵĽ��
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	STAnimalAtt					    arrSTAnimalAtt[eAnimalType_Max][eColorType_Max]; //��������
	UINT32							arrColorRate[eColorType_Max];//��ɫ�ֲ�����
	STEnjoyGameAtt					arrSTEnjoyGameAtt[eEnjoyGameType_Max];//ׯ�к�����

	CMD_BANKER_INFO					stBankerInfo;						//ׯ����Ϣ
    BYTE							cbBankerFlag;					      //ׯ�ұ�ʾ 0�� ��ׯ�ң�1�� ׯ��
};

//�û���ע
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//�û�λ��
	eGambleType eGamble;
	STAnimalInfo stAnimalInfo;
	eEnjoyGameType eEnjoyGameInfo;
	UINT64			iPlaceJettonScore;				    //��ǰ��ע
	BYTE			cbBanker;							//�Ƿ���ׯ�ң�0�� ��ׯ�ң�1��ׯ��
	UINT64			iTotalPlayerJetton;				    //ׯ��ʱ����ʾ���������ע�ܺ�
};

//��Ϸ����
struct CMD_S_GameEnd
{
	//��һ����Ϣ
	UINT32							dwTimeLeave;						//ʣ��ʱ��
	//UINT32							dwEnjoyGameTimeLeave;			//ʣ��ʱ��
	STAnimalPrize				    stWinAnimal;						//��������
	STEnjoyGamePrizeInfo			stWinEnjoyGameType;					//����ׯ�к�
	//��ҳɼ�
	__int64							iUserScore;							//��ҳɼ�
	//ȫ����Ϣ
	__int64							iRevenue;							//��Ϸ˰��

	CMD_BANKER_INFO					stBankerInfo;						//ׯ����Ϣ
	__int64							iBankerScore;						//ׯ���ε÷�
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_PLACE_JETTON			1									//�û���ע
#define SUB_C_CLEAR_JETTON			2									//�����ע

#define SUB_C_APPLY_BANKER			3									//����ׯ��
#define SUB_C_CANCEL_BANKER			4									//ȡ������

#define SUB_C_CONTROL_GET_INFO			5								//����Ա��ȡϵͳ��Ϣ
#define SUB_C_CONTROL_SET_PRIZE			6								//����Ա���ÿ������
#define SUB_C_CONTROL_UPDATE_STORAGE	7								//���¿��

//�û���ע
struct CMD_C_PlaceJetton
{
	eGambleType eGamble;
	STAnimalInfo stAnimalInfo;
	eEnjoyGameType  eEnjoyGameInfo;
	UINT64		 iPlaceJettonScore;										//��ǰ��ע
};

//�ͻ����û���ע
struct CMD_C_PlaceJetton_Client
{
	int		 eGamble;
	int		 eAnimal;														//����
	int		 eColor;														//��ɫ
	int		 eEnjoyGameInfo;
	LONGLONG	 iPlaceJettonScore;												//��ǰ��ע
	bool	 isClient;														//�Ƿ�ͻ���
};


//�û�����ָ��
struct CMD_C_Control
{
	BYTE cbCmdType;
	WORD wChairID;
	eAnimalPrizeMode eAnimalPrize;
	eAnimalType eAnimal;
	eColorType eColor;
	eEnjoyGameType eEnjoyGame;
	LONGLONG lStorageScore;
	DWORD	nStorageDecute;
};

//�������
//enum enOperateResult
//{
//	enOperateResult_NULL,
//	enOperateResult_Win,
//	enOperateResult_Lost
//};

//��¼��Ϣ
struct tagClientGameRecord
{
	//enOperateResult					enOperateFlags;						//������ʶ
	//BYTE							cbPlayerCount;						//�мҵ���
	//BYTE							cbBankerCount;						//ׯ�ҵ���
	//BYTE							cbKingWinner;						//����Ӯ��
	//bool							bPlayerTwoPair;						//���ӱ�ʶ
	//bool							bBankerTwoPair;						//���ӱ�ʶ

	STPrizeInfo stAnimalWinRecord;
	STEnjoyGamePrizeInfo stEnjoyGameRecord;
};

//��ʷ��¼
#define MAX_SCORE_HISTORY			5								    //��ʷ����
#define MAX_SHOW_HISTORY			8									//һ�������ʾ����ʷ��¼����

//ͬʱ��ʾ������ׯ�������
#define MAX_SCORE_BANKER			7
//////////////////////////////////////////////////////////////////////////
inline void DebugString(LPCTSTR lpszFormat,...) 
{ 
	va_list args; 
	TCHAR szText[1024];

	va_start(args, lpszFormat); 
	wvsprintf(szText, lpszFormat, args); 
	OutputDebugString(szText); 
	va_end(args); 
}
#endif
