#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//������
#define RQ_REFRESH_STORAGE		1
#define RQ_SET_STORAGE			2


//���¿��
struct CMD_C_UpdateStorage
{
	BYTE                            cbReqType;						//��������
	LONGLONG						lStorage;						//�¿��ֵ
	LONGLONG						lStorageDeduct;					//���˥��
};
//���¿��
struct CMD_S_UpdateStorage
{
	LONGLONG						lStorage;							//�¿��ֵ
	LONGLONG						lStorageDeduct;						//���˥��
};

//��Ϸ����
#define KIND_ID						127									//��Ϸ I D
#define GAME_NAME					TEXT("��������")					//��Ϸ����

//�������
#define GAME_PLAYER					MAX_CHAIR							//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//״̬����

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_BET				GAME_STATUS_PLAY					//��ע״̬
#define GAME_SCENE_END				(GAME_STATUS_PLAY+1)				//����״̬

//////////////////////////////////////////////////////////////////////////////////
//�����
#define GEM_FIRST							4							//��һ����ֵ
#define GEM_SECOND							5							//�ڶ�����ֵ
#define GEM_THIRD							6							//��������ֵ
#define GEM_MAX								7							//�����ֵ

//////////////////////////////////////////////////////////////////////////////////
//ת������
#define TURAN_TABLE_MAX						28							//ת������
#define TURAN_TABLE_JUMP					30							//ת����ת

//��������
#define ANIMAL_LION							0							//ʨ��
#define ANIMAL_PANDA						1							//��è
#define ANIMAL_MONKEY						2							//����
#define ANIMAL_RABBIT						3							//����
#define ANIMAL_EAGLE						4							//��ӥ
#define ANIMAL_PEACOCK						5							//��ȸ
#define ANIMAL_PIGEON						6							//����
#define ANIMAL_SWALLOW						7							//����
#define ANIMAL_SLIVER_SHARK					8							//����
#define ANIMAL_GOLD_SHARK					9							//����
#define ANIMAL_BIRD							10							//����
#define ANIMAL_BEAST						11							//����
#define ANIMAL_MAX							12

//������Ϣ
#define ANIMAL_TYPE_NULL					0							//��
#define ANIMAL_TYPE_BEAST					1							//����
#define ANIMAL_TYPE_BIRD					2							//����
#define ANIMAL_TYPE_GOLD					3							//�ƽ�
#define ANIMAL_TYPE_SLIVER					4							//����

//��Ϸ��¼
#define RECORD_COUNT_MAX					20							//�������

//////////////////////////////////////////////////////////////////////////////////
//��ע��ť
#define IDM_PLAY_BET						(WM_USER+201)				//ʨ��

//���ư�ť
#define IDM_OPERATION_RENEWAL				(WM_USER+213)				//��ѹ
#define IDM_OPERATION_CANCEL				(WM_USER+214)				//ȡ��
#define IDM_OPERATION_SWITCH				(WM_USER+215)				//�л�

#define IDM_ADMIN_COMMDN			WM_USER+1000
#define IDM_UPDATE_STORAGE			WM_USER+1001

//////////////////////////////////////////////////////////////////////////////////
//����
#define	MAX_VOLUME (3000)
#define SUB_C_AMDIN_COMMAND			4									//ϵͳ����
#define SUB_C_UPDATE_STORAGE		5									//���¿��
//////////////////////////////////////////////////////////////////////////////////
//�����-������
#define SUB_S_AMDIN_COMMAND			109									//ϵͳ����
#define SUB_S_UPDATE_STORAGE        110									//���¿��
#define SUB_S_SEND_USER_BET_INFO    111                                 //������ע
//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA  	1
#define RQ_RESET_CONTROL	2
#define RQ_PRINT_SYN		3
	BYTE cbExtendData[20];			//��������
};

//����ظ�
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//�ظ�����
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3
	BYTE cbResult;
#define CR_ACCEPT  2			//����
#define CR_REFUSAL 3			//�ܾ�
	BYTE cbExtendData[20];			//��������
};
//����������Ϣ
struct tagControlInfo
{
	INT  nAreaWin[ANIMAL_MAX];		//��������
};

////��ע��Ϣ
struct tagUserBet
{
	TCHAR							szNickName[32];						//�û��ǳ�
	DWORD							dwUserGameID;						//�û�ID
	LONGLONG						lUserStartScore;					//�û����
	LONGLONG						lUserWinLost;						//�û����
	LONGLONG						lUserBet[ANIMAL_MAX];				//�û���ע 
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��Ϸ״̬
struct CMD_S_StatusFree
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	LONGLONG						lStartStorage;						//��ʼ���
	int								lCellScore;							//�׷�
	LONGLONG						lStorageStart;						//��棨�ʳأ�

	LONGLONG						lAreaLimitScore;					//��������
	LONGLONG						lPlayLimitScore;					//�������

	int								nTurnTableRecord[RECORD_COUNT_MAX];	//��Ϸ��¼
	int								nTurnTableRecordExt[RECORD_COUNT_MAX];//��Ϸ��¼
	//WORD							szGameRoomID;						//����ID
	TCHAR							szGameRoomName[32];					//��������
	bool							bGenreEducate;						//��ϰģʽ

	LONGLONG						dChipArray[6];						//��������
	//LONGLONG						lPlayScore;							//��ҷ���
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	LONGLONG						lStartStorage;						//��ʼ���
	int								lCellScore;							//�׷�
	//LONGLONG						lPlayScore;							//��ҷ���
	LONGLONG						lStorageStart;						//��棨�ʳأ�		

	LONGLONG						lAreaLimitScore;					//��������
	LONGLONG						lPlayLimitScore;					//�������

	int								nAnimalMultiple[ANIMAL_MAX];		//���ﱶ��
	LONGLONG						lAllBet[ANIMAL_MAX];				//����ע
	LONGLONG						lPlayBet[ANIMAL_MAX];				//�����ע

	int								nTurnTableRecord[RECORD_COUNT_MAX];	//��Ϸ��¼
	int								nTurnTableRecordExt[RECORD_COUNT_MAX];//��Ϸ��¼
	//WORD							szGameRoomID;						//����ID
	TCHAR							szGameRoomName[32];					//��������
	bool							bGenreEducate;						//��ϰģʽ
	LONGLONG						dChipArray[6];						//��������
};

//��Ϸ״̬
struct CMD_S_StatusEnd
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	LONGLONG						lStartStorage;						//��ʼ���
	int								lCellScore;							//�׷�
	//LONGLONG						lPlayScore;							//��ҷ���
	LONGLONG						lStorageStart;						//��棨�ʳأ�

	LONGLONG						lAreaLimitScore;					//��������
	LONGLONG						lPlayLimitScore;					//�������

	int								nAnimalMultiple[ANIMAL_MAX];		//���ﱶ��
	LONGLONG						lAllBet[ANIMAL_MAX];				//����ע
	LONGLONG						lPlayBet[ANIMAL_MAX];				//�����ע

	int								nTurnTableRecord[RECORD_COUNT_MAX];	//��Ϸ��¼
	int								nTurnTableRecordExt[RECORD_COUNT_MAX];//��Ϸ��¼
	TCHAR							szGameRoomName[32];					//��������
	bool							bGenreEducate;						//��ϰģʽ
	LONGLONG						dChipArray[6];						//��������
};


//////////////////////////////////////////////////////////////////////////////////
//����������ṹ
enum
{
	SUB_S_GAME_FREE		= 101,						//��Ϸ����
	SUB_S_GAME_START	= 102,						//��Ϸ��ʼ
	SUB_S_GAME_END		= 103,						//��Ϸ����
	SUB_S_PLAY_BET		= 104,						//�û���ע
	SUB_S_PLAY_BET_FAIL	= 105,						//�û���עʧ��
	SUB_S_BET_CLEAR		= 106,						//�����ע
	SUB_S_SWITCT_NUM	= 199,						//�л�����

};

//�л�����
struct CMD_S_SwitchNum
{
	int						lAddScore1;
	int						lAddScore2;
	int						lAddScore3;
	int						lAddScore4;
};

//��Ϸ����
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	int								nAnimalMultiple[ANIMAL_MAX];		//���ﱶ��
	LONGLONG						lStorageStart;						//��棨�ʳأ�
};

//��Ϸ����
struct CMD_S_GameEnd
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	BOOL							bTurnTwoTime;						//ת2��
	int								nTurnTableTarget[2];				//ת��Ŀ��
	int								nPrizesMultiple;					//�ʽ�	
	LONGLONG						lPlayWin[2];						//�����Ӯ
	LONGLONG						lPlayPrizes;						//��Ҳʽ�

	LONGLONG						lPlayShowPrizes;					//��ʾ�ʽ�
	LONGLONG						lGameScore;							//����ܷ�
	LONGLONG						lServiceScore;						//�����
};

//�û���ע
struct CMD_S_PlayBet
{
	WORD							wChairID;							//���λ��
	//TCHAR							szPlayerName[32];					
	//LONGLONG                        mHBet;								//��Ա�����ע
	int								nAnimalIndex;						//��ע����
	LONGLONG						lBetChip;							//��������
	bool							bAndroid;							//������־
};

//�û���עʧ��
struct CMD_S_PlayBetFail
{
	WORD							wChairID;							//���λ��
	int								nAnimalIndex;						//��ע����
	LONGLONG						lBetChip;							//��������
};

//�����ע
struct CMD_S_BetClear
{
	WORD							wChairID;							//���λ��
	//TCHAR							szPlayerName[32];					
	LONGLONG						lPlayBet[ANIMAL_MAX];				//����������
};
//////////////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
enum
{
	SUB_C_SWITCT_NUM	= 101,									//�һ�����
	SUB_C_PLAY_BET		= 102,									//��ע��Ϣ
	SUB_C_BET_CLEAR		= 103,									//�����ע
	//SUB_C_AMDIN_COMMAND = 104,									//����Ա����
	//SUB_C_AMDIN_STORANGINFO = 105,								//�����Ϣ
};
///////////////////////////////////////////////////////////////////////////////////////////////
//���������Ʒ���
#define	 S_CR_FAILURE				0		//ʧ��
#define  S_CR_UPDATE_SUCCES			1		//���³ɹ�
#define	 S_CR_SET_SUCCESS			2		//���óɹ�
#define  S_CR_CANCEL_SUCCESS		3		//ȡ���ɹ�
#define  S_CR_ACK_PRINT_STORAGE		4		//���ÿ��
struct CMD_S_ControlReturns
{
	BYTE cbReturnsType;				//�ظ�����
	BYTE cbControlStyle;			//���Ʒ�ʽ
	BYTE cbControlArea;				//��������
	BYTE cbControlTimes;			//���ƴ���
	LONGLONG lCurStorage;			//��ǰ���
};


//��������
#define  C_CA_UPDATE				1		//����
#define	 C_CA_SET					2		//����
#define  C_CA_CANCELS				3		//ȡ��
#define  C_CA_SET_STORAGE			4		//���
#define  C_CA_PRINT_STORAGE			5		//���

//���Ʒ�ʽ
#define		CS_BANKER_LOSE    1
#define		CS_BANKER_WIN	  2
#define		CS_BET_AREA		  3

struct CMD_C_ControlApplication
{
	BYTE cbControlAppType;			//��������
	BYTE cbControlStyle;			//���Ʒ�ʽ
	BYTE cbControlArea;				//��������
	BYTE cbControlTimes;			//���ƴ���
	LONGLONG lSetStorage;			//���ÿ��
	LONGLONG lSetDeduct;			//���ÿ��˥��ֵ
};

//////////////////////////////////////////////////////////////////////////////////////////////////

//�һ�����
struct CMD_C_Chip
{
	LONGLONG						lChip;								//��������
};

//��ע��Ϣ
struct CMD_C_PlayBet
{
	int								nAnimalIndex;						//��ע����
	LONGLONG						lBetChip;							//��������
};

//�����ע
struct CMD_C_BetClear
{
};
//��������Ϣ
struct tagRobotInfo
{
	int nChip[6];														//���붨��
	TCHAR szCfgFileName[MAX_PATH];										//�����ļ�
	int	nMaxTime;														//�������

	tagRobotInfo()
	{
		int nTmpChip[6] = {1, 2, 5, 10, 20, 50};
		TCHAR szPath[MAX_PATH] = TEXT("");
		GetCurrentDirectory(sizeof(szPath), szPath);
		_sntprintf(szCfgFileName,sizeof(szCfgFileName),TEXT("%s\\SharkBattleControl\\SharkBattle.ini"),szPath);

		nMaxTime = 1;
		memcpy(nChip, nTmpChip, sizeof(nChip));
	}
};

#pragma pack()
#endif