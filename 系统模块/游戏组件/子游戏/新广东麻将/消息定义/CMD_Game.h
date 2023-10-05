#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

//����������
//#ifdef _DEBUG
#define CARD_DISPATCHER_CONTROL
//#endif

///////////////////////���ܿ���
#define G_SHOW_TING_DATA		TRUE			//���Ƹ�����Ϣ

#define G_SHOW_HU_DATA			FALSE			//������ʾ

#define G_CAN_CHI_CARD 			FALSE			//����

#define G_CAN_TING_CARD			FALSE			//����

#define G_QIANG_GANG_HU			TRUE			//���ܺ�

#define G_GANG_SHANG_PAO		FALSE			//������

#define G_CHI_HU				FALSE			//�Ժ�

#define G_ZI_SHUN				FALSE			//���Ƴ�˳ ���ƣ�������������������˳ ���ƣ��з��׳�˳

#define G_GEN_ZHUANG			FALSE			//��ׯ

#define G_ONLY_ONE_HU			TRUE			//ֻ��һ���˺�

///////////////////////////////////////////////////////////////

#define MAX_MAGIC_COUNT			2				//��������

#define MAX_RECORD_COUNT		32				//��������ͳ��������

#define TIME_DISPATCH_CARD		16				//��������ƶ���ʱ��
//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						391									//��Ϸ I D
#define GAME_NAME					TEXT("�¹㶫�齫")					//��Ϸ����

//�������
#define GAME_PLAYER					4									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////

//��������
#define MAX_WEAVE					4									//������
#define MAX_INDEX					34									//�������
#define MAX_COUNT					14									//�����Ŀ
#define MAX_REPERTORY				136									//�����
#define MAX_HUA_INDEX				0									//��������
#define MAX_HUA_COUNT				8									//���Ƹ���
#define MAX_MA_COUNT				8									//���ץ����
//�˿˶���
#define HEAP_FULL_COUNT				34									//����ȫ��
#define MAX_RIGHT_COUNT				1									//���ȨλDWORD����	

//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					//��Ϸ����

struct CMD_S_RECORD
{
	int								nCount;
	BYTE							cbHuCount[GAME_PLAYER];					//���ƴ���
	BYTE							cbMaCount[GAME_PLAYER];					//�������
	BYTE							cbAnGang[GAME_PLAYER];					//���ܴ���
	BYTE							cbMingGang[GAME_PLAYER];				//���ܴ���
	SCORE							lAllScore[GAME_PLAYER];					//�ܽ����
	SCORE							lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];//���ֽ����
};
//�������
struct tagWeaveItem
{
	BYTE							cbWeaveKind;							//�������
	BYTE							cbCenterCard;							//�����˿�
	BYTE							cbParam;								//���ͱ�־
	WORD							wProvideUser;							//��Ӧ�û�
	BYTE							cbCardData[4];							//�齫����
};

//����״̬
struct CMD_S_StatusFree
{
	BYTE							cbHeapCount[GAME_PLAYER];				//������
	//��Ϸ����
	LONG							lCellScore;								//��������
	//ʱ����Ϣ
	BYTE							cbTimeOutCard;							//����ʱ��
	BYTE							cbTimeOperateCard;						//����ʱ��
	BYTE							cbTimeStartGame;						//��ʼʱ��
	BYTE							cbTimeWaitEnd;							//����ȴ�

	//��ʷ����
	SCORE							lTurnScore[GAME_PLAYER];				//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];				//������Ϣ

	//����					
	BYTE							cbPlayerCount;							//�������
	BYTE							cbMaCount;								//����
	BYTE							cbMagicMode;							//����ģʽ
	bool							bQiangGangHu;							//���ܺ�
	bool							bHuQiDui;								//���߶�
	bool							bHaveZiCard;							//������
	bool							bNoMagicDouble;							//�޹�ӱ�
	BYTE							cbAllCardCount;							//������
	BYTE							cbPlayMode;								//0���ֳ� 1��ҳ�
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	BYTE							cbHeapCount[GAME_PLAYER];				//������
	//ʱ����Ϣ
	BYTE							cbTimeOutCard;							//����ʱ��
	BYTE							cbTimeOperateCard;						//����ʱ��
	BYTE							cbTimeStartGame;						//��ʼʱ��
	BYTE							cbTimeWaitEnd;							//����ȴ�
	//��Ϸ����
	LONGLONG						lCellScore;								//��Ԫ����
	WORD							wBankerUser;							//ׯ���û�
	WORD							wCurrentUser;							//��ǰ�û�
	BYTE							cbMagicIndex[MAX_MAGIC_COUNT];			//��������

	//����					
	BYTE							cbPlayerCount;							//�������
	BYTE							cbMaCount;								//����
	BYTE							cbMagicMode;							//����ģʽ
	bool							bQiangGangHu;							//���ܺ�
	bool							bHuQiDui;								//���߶�
	bool							bHaveZiCard;							//������
	bool							bNoMagicDouble;							//�޹�ӱ�

	//״̬����
	BYTE							cbActionCard;							//�����˿�
	BYTE							cbActionMask;							//��������
	WORD							wProvideUser;							//�������
	BYTE							cbAllCardCount;							//������
	BYTE							cbLeftCardCount;						//ʣ����Ŀ
	bool							bTrustee[GAME_PLAYER];					//�Ƿ��й�

	//������Ϣ
	WORD							wOutCardUser;							//�����û�
	BYTE							cbOutCardData;							//�����˿�
	BYTE							cbDiscardCount[GAME_PLAYER];			//������Ŀ
	BYTE							cbDiscardCard[GAME_PLAYER][60];			//������¼

	//�˿�����
	BYTE							cbCardCount[GAME_PLAYER];				//����Ŀ
	BYTE							cbCardData[MAX_COUNT];					//������
	BYTE							cbSendCardData;							//����

	//����˿�
	BYTE							cbWeaveItemCount[GAME_PLAYER];			//�����Ŀ
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//�������

	//������Ϣ
	WORD							wHeapHead;								//����ͷ��
	WORD							wHeapTail;								//����β��
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];			//������Ϣ

	//��ʷ����
	SCORE							lTurnScore[GAME_PLAYER];				//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];				//������Ϣ

	BYTE							cbPlayMode;								//0���ֳ� 1��ҳ�
	BYTE							cbRemainingTime;						//ʣ�����ʱ��
};

//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_S_GAME_START			100										//��Ϸ��ʼ
#define SUB_S_OUT_CARD				101										//�û�����
#define SUB_S_SEND_CARD				102										//�����˿�
#define SUB_S_OPERATE_NOTIFY		103										//������ʾ
#define SUB_S_OPERATE_RESULT		104										//�������
#define SUB_S_LISTEN_CARD			105										//�û�����
#define SUB_S_TRUSTEE				106										//�û��й�
#define SUB_S_REPLACE_CARD			107										//�û�����
#define SUB_S_GAME_CONCLUDE			108										//��Ϸ����
#define SUB_S_SET_BASESCORE			109										//���û���
#define SUB_S_TING_DATA				110										//��������
#define SUB_S_HU_DATA				111										//��������
#define SUB_S_RECORD				112										//��Ϸ��¼

//¼������
struct Video_GameStart
{	
	TCHAR							szNickName[LEN_NICKNAME];				//�û��ǳ�	
	SCORE							lScore;									//����
	LONG							lCellScore;								//�׷�
	WORD							wChairID;								//����ID
	WORD				 			wBankerUser;							//ׯ��
	WORD							wSiceCount;								//���ӵ���
	BYTE							cbCardData[MAX_COUNT];					//�˿��б�
	BYTE							cbMagicIndex[MAX_MAGIC_COUNT];			//��������
	BYTE							cbUserAction;							//�û�����
	BYTE							cbFanCardData;							//������

	BYTE							cbAllCardCount;							//������
	BYTE							cbPlayerCount;							//�������
	BYTE							cbMaCount;								//����
	BYTE							cbMagicMode;							//����ģʽ
	bool							bQiangGangHu;							//���ܺ�
	bool							bHuQiDui;								//���߶�
	bool							bHaveZiCard;							//������
	bool							bNoMagicDouble;							//�޹�ӱ�
	
};
//�����˿�
struct CMD_S_GameStart
{
	WORD				 			wBankerUser;							//��ǰׯ��
	WORD							wSiceCount;								//���ӵ���
	WORD							wHeapHead;								//�ƶ�ͷ��
	WORD							wHeapTail;								//�ƶ�β��
	BYTE							cbMagicIndex[MAX_MAGIC_COUNT];			//��������
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];			//������Ϣ
	BYTE							cbUserAction;							//�û�����
	BYTE							cbCardData[MAX_COUNT];					//������
	BYTE							cbFanCardData;							//������
};

//�û�����
struct CMD_S_OutCard
{
	WORD							wOutCardUser;							//�����û�
	BYTE							cbOutCardData;							//�����˿�
	bool							bSysOut;								//�йܳ���
};

//�����˿�
struct CMD_S_SendCard
{
	BYTE							cbCardData;								//�˿�����
	BYTE							cbActionMask;							//��������
	WORD							wCurrentUser;							//��ǰ�û�
	WORD							wSendCardUser;							//�����û�
	WORD							wReplaceUser;							//�����û�
	bool							bTail;									//ĩβ����
};

//������ʾ
struct CMD_S_OperateNotify
{
	BYTE							cbActionMask;							//��������
	BYTE							cbActionCard;							//�����˿�
	WORD							wProvideUser;							//�ṩ��
};

//��������
struct CMD_S_OperateResult
{
	WORD							wOperateUser;							//�����û�
	BYTE							cbActionMask;							//��������
	WORD							wProvideUser;							//��Ӧ�û�
	BYTE							cbOperateCode;							//��������
	BYTE							cbOperateCard[3];						//�����˿�
};

//���Ƹ�����Ϣ
struct CMD_S_TING_DATA
{
	//���ļ�������
	BYTE							cbOutCardCount;
	BYTE							cbOutCardData[MAX_COUNT];
	//�����ܺ��ļ�����
	BYTE							cbHuCardCount[MAX_COUNT];
	BYTE							cbHuCardData[MAX_COUNT][HEAP_FULL_COUNT];
	//����ʣ����
	BYTE							cbHuCardRemainingCount[MAX_COUNT][HEAP_FULL_COUNT];
	//���Ʒ���
	BYTE							cbHuFan[MAX_COUNT][HEAP_FULL_COUNT];
};

struct CMD_S_HU_DATA
{
	WORD							wUser;									//�û�
	BYTE							cbHuCardCount;							//��������
	BYTE							cbHuCardData[MAX_INDEX];				//��������
};

//��������
struct CMD_S_ListenCard
{
	WORD							wListenUser;							//�����û�
	bool							bListen;								//�Ƿ�����
};

//��Ϸ����
struct CMD_S_GameConclude
{
	//���ֱ���
	LONG							lCellScore;								//������
	SCORE							lGameScore[GAME_PLAYER];				//����Ӯ
	SCORE							lRevenue[GAME_PLAYER];					//˰�ջ���
	SCORE							lGangScore[GAME_PLAYER];				//����Ӯ
	SCORE							lHuScore[GAME_PLAYER];					//������Ӯ
	SCORE							lMaScore[GAME_PLAYER];					//����Ӯ
	//������Ϣ
	WORD							wFleeUser;								//�������
	WORD							wProvideUser;							//��Ӧ�û�
	BYTE							cbProvideCard;							//��Ӧ�˿�
	BYTE							cbSendCardData;							//�����
	DWORD							dwChiHuKind[GAME_PLAYER];				//��������
	DWORD							dwChiHuRight[GAME_PLAYER][MAX_RIGHT_COUNT];	//��������	
	
	//��Ϸ��Ϣ
	BYTE							cbCardCount[GAME_PLAYER];				//�˿���Ŀ
	BYTE							cbHandCardData[GAME_PLAYER][MAX_COUNT];	//�˿��б�

	BYTE							cbMaCount;								//����
	BYTE							cbMaData[MAX_MA_COUNT];					//������
	BYTE							cbZhongMaCount;							//�������
	BYTE							cbZhongMaData[MAX_MA_COUNT];			//��������
	BOOL							bNoMagicCard;							//�Ƿ��޹�ӱ�
	BYTE							cbMagicIndex[MAX_MAGIC_COUNT];			//��������
	BYTE							cbWeaveItemCount[GAME_PLAYER];			//�����Ŀ
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//����˿�
};

//�û��й�
struct CMD_S_Trustee
{
	bool							bTrustee;								//�Ƿ��й�
	WORD							wChairID;								//�й��û�
};

//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_C_OUT_CARD				1										//��������
#define SUB_C_OPERATE_CARD			2										//�����˿�
#define SUB_C_LISTEN_CARD			3										//�û�����
#define SUB_C_TRUSTEE				4										//�û��й�
#define SUB_C_REPLACE_CARD			5										//�û�����
#define SUB_C_SEND_CARD				6                                 		//�����˿� 
#define SUB_C_REQUEST_RCRecord		7										//���󷿿���¼
//��������
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//�˿�����
};

//��������
struct CMD_C_OperateCard
{
	BYTE							cbOperateCode;						//��������
	BYTE							cbOperateCard[3];					//�����˿�
};

//�û�����
struct CMD_C_ListenCard
{
	bool							bListenCard;						//�Ƿ�����	
};

//�û��й�
struct CMD_C_Trustee
{
	bool							bTrustee;							//�Ƿ��й�	
};

//////////////////////////////////////////////////////////////////////////////////

#endif