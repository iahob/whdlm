#ifndef CMD_KWX_HEAD_FILE 
#define CMD_KWX_HEAD_FILE
#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//�����궨��
#define KIND_ID							707									//��Ϸ I D
#define GAME_NAME						TEXT("����������")					    //��Ϸ����
#define GAME_PLAYER						3									//��Ϸ����

#define VERSION_SERVER			    	PROCESS_VERSION(6,0,3)				//����汾
#define VERSION_CLIENT				    PROCESS_VERSION(6,0,3)				//����汾

//��Ϸ״̬
#define GS_MJ_FREE				    	GAME_STATUS_FREE					//����״̬
#define GS_MJ_PIAO						GAME_STATUS_PLAY+1					//��Ư״̬
#define GS_MJ_PLAY						GAME_STATUS_PLAY+2					//��Ϸ״̬

//��������
#define MAX_WEAVE					4									//������
#define MAX_SELECT					12									//���ѡ��
#define MAX_INDEX					34									//�������
#define MAX_COUNT					14									//�����Ŀ
#define MAX_REPERTORY				84									//�����

//�˿˶���
#define HEAP_FULL_COUNT				28									//����ȫ��

#define MAX_RIGHT_COUNT				1									//���ȨλDWORD����			

//////////////////////////////////////////////////////////////////////////

//�������
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbPublicCard;						//������־
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbCardData[4];						//�������
};

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_OUT_CARD				101									//��������
#define SUB_S_SEND_CARD				102									//�����˿�
#define SUB_S_OPERATE_NOTIFY		104									//������ʾ
#define SUB_S_OPERATE_RESULT		105									//��������
#define SUB_S_GAME_END				106									//��Ϸ����
#define SUB_S_TRUSTEE				107									//�û��й�
#define SUB_S_LISTEN				103									//�û�����
#define SUB_S_GANG					109									//�û�����
#define SUB_S_CALLPIAO				111									//��ʼ��Ư
#define SUB_S_HU_CARD				114									//��������
#define SUB_S_PIAO					115									//��Ư
#define SUB_S_RECORD				116									//��Ϸ��¼
#define SUB_S_TING_DATA				117									//���Ƹ���
//������ʾ
struct CMD_S_HuCard
{
	WORD							wUser;								//�������
	BYTE							cbHuCount;							//���Ƹ���
	BYTE							cbHuCardData[MAX_COUNT];			//��������
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
	//�ܸǵ���
	BYTE							cbSelectCount;
	BYTE							cbSelectCardData[4];
};

//��ʼ��Ư
struct CMD_S_StartCallPiao
{
	bool bPlayStatus[GAME_PLAYER];
};

//��Ư
struct CMD_S_CallPiao
{
	WORD		wUser;
	BYTE		cbPiao;
};

struct CMD_S_RecordInfo
{
	int nCount;
	BYTE cbZiMo[GAME_PLAYER];
	BYTE cbJiePao[GAME_PLAYER];
	BYTE cbDianPao[GAME_PLAYER];
	BYTE cbAnGang[GAME_PLAYER];
	BYTE cbMingGang[GAME_PLAYER];
	SCORE lAllScore[GAME_PLAYER];
	SCORE lDetailScore[GAME_PLAYER][32];
};

//����״̬
struct CMD_S_StatusFree
{
	//ʱ����Ϣ
	BYTE							cbTimeStart;							//��ʼʱ��
	BYTE							cbTimeCallPiao;							//��Ưʱ��
	BYTE							cbTimeOutCard;							//����ʱ��
	BYTE							cbTimeOperate;							//����ʱ��

	SCORE							lCellScore;								//�������
	BYTE							cbPlayerCount;							//�������
	BYTE							cbPiaoMode;								//Ư 0��Ư��1�״�Ư��2ÿ��Ư
	BYTE							cbMaMode;								//0������1��������2������������
	BYTE							cbMaCount;								//0�ޣ�1��1��2��6��3��1��1
	BYTE							cbRule;									//0ȫƵ����1��Ƶ��
	DWORD							wTableOwnerID;
	BYTE							cbPlayMode;
};

//��Ư״̬
struct CMD_S_StatusPiao
{
	//ʱ����Ϣ
	BYTE							cbTimeStart;							//��ʼʱ��
	BYTE							cbTimeCallPiao;							//��Ưʱ��
	BYTE							cbTimeOutCard;							//����ʱ��
	BYTE							cbTimeOperate;							//����ʱ��

	SCORE							lCellScore;								//��Ԫ����
	BYTE							cbPlayerCount;							//�������
	BYTE							cbPiaoMode;								//Ư 0��Ư��1�״�Ư��2ÿ��Ư
	BYTE							cbMaMode;								//0������1��������2������������
	BYTE							cbMaCount;								//0�ޣ�1��1��2��6��3��1��1
	BYTE							cbRule;									//0ȫƵ����1��Ƶ��

	bool							bPlayStatus[GAME_PLAYER];				//����״̬
	bool							bTrustee[GAME_PLAYER];					//�Ƿ��й�

	//��Ư���
	bool							bCallPiao[GAME_PLAYER];					//�Ƿ��Ư
	BYTE							cbUserPiao[GAME_PLAYER];				//��Ư����
	//����
	DWORD							wTableOwnerID;
	BYTE							cbPlayMode;
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//ʱ����Ϣ
	BYTE							cbTimeStart;							//��ʼʱ��
	BYTE							cbTimeCallPiao;							//��Ưʱ��
	BYTE							cbTimeOutCard;							//����ʱ��
	BYTE							cbTimeOperate;							//����ʱ��

	SCORE							lCellScore;								//��Ԫ����
	BYTE							cbPlayerCount;							//�������
	BYTE							cbPiaoMode;								//Ư 0��Ư��1�״�Ư��2ÿ��Ư
	BYTE							cbMaMode;								//0������1��������2������������
	BYTE							cbMaCount;								//0�ޣ�1��1��2��6��3��1��1
	BYTE							cbRule;									//0ȫƵ����1��Ƶ��

	//��Ϸ����
	WORD							wBankerUser;							//ׯ���û�
	WORD							wCurrentUser;							//��ǰ�û�

	//״̬����
	BYTE							cbActionCard;								//�����˿�
	BYTE							cbActionMask;								//��������
	BYTE							cbHearStatus[GAME_PLAYER];					//����״̬
	BYTE							cbLeftCardCount;							//ʣ����Ŀ
	bool							bTrustee[GAME_PLAYER];						//�Ƿ��й�

	//������Ϣ
	WORD							wOutCardUser;								//�����û�
	BYTE							cbOutCardData;								//�����˿�
	BYTE							cbDiscardCount[GAME_PLAYER];				//������Ŀ
	BYTE							cbDiscardCard[GAME_PLAYER][60];				//������¼

	//�˿�����
	BYTE							cbCardCount[GAME_PLAYER];					//�˿���Ŀ
	BYTE							cbCardData[MAX_COUNT];						//�˿��б�
	BYTE							cbSendCardData;								//�����˿�
	BYTE							cbCardIndex[GAME_PLAYER][MAX_INDEX];		//��������
	BYTE							cbLinstenCardData[GAME_PLAYER][MAX_COUNT];	//��������	

	//����˿�
	BYTE							cbWeaveCount[GAME_PLAYER];					//�����Ŀ
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//����˿�

	//������Ϣ
	WORD							wHeapHead;									//����ͷ��
	WORD							wHeapTail;									//����β��
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];				//������Ϣ

	//������Ϣ
	BYTE							cbHuCardCount[GAME_PLAYER];
	BYTE							cbHuCardData[GAME_PLAYER][MAX_COUNT];

	//��Ư���
	BYTE							cbUserPiao[GAME_PLAYER];
	//����
	DWORD							wTableOwnerID;

	bool							bPlayStatus[GAME_PLAYER];					//����״̬
	BYTE							cbPlayMode;
};

//¼������
struct Video_GameStart
{
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�	
	SCORE							lScore;								//����
	LONG							lCellScore;							//�׷�
	WORD							wChairID;							//����ID
	WORD				 			wBankerUser;						//ׯ��
	LONG							lSiceCount;							//��������
	BYTE							cbUserAction;						//�û�����
	BYTE							cbCardData[MAX_COUNT];				//�˿��б�
	WORD							wHeapHead;							//������ͷ
	WORD							wHeapTail;							//������β
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//������Ϣ

	//����					
	BYTE							cbPlayerCount;						//�������
	BYTE							cbPiaoMode;							//Ưģʽ 0��Ư��1Ưһ�Σ�2ÿ��Ư
	BYTE							cbMaMode;							//0������1��������2������������
	BYTE							cbMaCount;							//0�ޣ�1��1��2��6��3��1��1
	BYTE							cbRule;								//0ȫƵ����1��Ƶ��

	BYTE							cbUserPiao;							//�����Ư�ı���

};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	LONG							lSiceCount;										//���ӵ���
	WORD							wBankerUser;									//ׯ���û�
	WORD							wCurrentUser;									//��ǰ�û�
	BYTE							cbUserAction;									//�û�����
	BYTE							cbCardData[MAX_COUNT];							//�˿��б�
	WORD							wHeapHead;										//������ͷ
	WORD							wHeapTail;										//������β
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];					//������Ϣ
	bool							bPlayStatus[GAME_PLAYER];						//����״̬
};

//��������
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//�����û�
	BYTE							cbOutCardData;						//�����˿�
	bool							bSysOut;							//�йܳ���
};

//�����˿�
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//�˿�����
	BYTE							cbActionMask;						//��������
	WORD							wCurrentUser;						//��ǰ�û�
	bool							bTail;								//ĩβ����	
};

//������ʾ
struct CMD_S_OperateNotify
{
	WORD							wResumeUser;						//��ԭ�û�
	BYTE							cbActionMask;						//��������
	BYTE							cbActionCard;						//�����˿�
};

//��������
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//�����û�
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbOperateCode;						//��������
	BYTE							cbOperateCard[3];					//�����˿�	
	BYTE							cbActionMask;						//��������
};

//��Ϸ����
struct CMD_S_GameEnd
{
	//������Ϣ
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbProvideCard;						//��Ӧ�˿�
	DWORD							dwChiHuKind[GAME_PLAYER];			//��������
	DWORD							dwChiHuRight[GAME_PLAYER];			//��������
	BYTE							cbHuaCardCount;						//���Ƹ���
	BYTE							cbFanCount[GAME_PLAYER];			//�ܷ���

	//������Ϣ
	SCORE							lGameScore[GAME_PLAYER];			//��Ϸ����

	//�˿���Ϣ
	BYTE							cbCardCount[GAME_PLAYER];			//�˿���Ŀ
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//�˿�����

	BYTE							cbWeaveItemCount[GAME_PLAYER];			//�����Ŀ
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//����˿�
	SCORE							lPiaoScore[GAME_PLAYER];				//Ư��Ӯ
	SCORE							lGangScore[GAME_PLAYER];				//����Ӯ
	SCORE							lMaScore[GAME_PLAYER];					//����Ӯ
	BYTE							cbMaFan;								//���뱶��
	BYTE							cbMaCount;								//������
	BYTE							cbMaData[6];							//������
};

//�û��й�
struct CMD_S_Trustee
{
	bool							bTrustee;							//�Ƿ��й�
	WORD							wChairID;							//�й��û�
};

//�û�����
struct CMD_S_Listen
{
	WORD							wChairId;							//�����û�
	BYTE							cbCardIndex[MAX_INDEX];				//�˿�����	
	BYTE							cbSelectCard[MAX_COUNT];			//����ס����
	BYTE							cbSelectCount;						//��ס�Ƶ�����
};

//��������
struct CMD_S_GangCard
{	
	WORD							wChairId;							//�����û�
	SCORE							cbGangScore[GAME_PLAYER];			//���Ʒ���
	BYTE							cbCardIndex[MAX_INDEX];				//����˿�����
	BYTE							cbSelectCount;						//��������
	BYTE							cbSelectCard[MAX_COUNT];			//��ס��������
	
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_OUT_CARD				1									//��������
#define SUB_C_OPERATE_CARD			2									//�����˿�
#define	SUB_C_CALLPIAO				3									//��ҽ�Ư
#define SUB_C_LISTEN				4									//�û�����
#define SUB_C_TRUSTEE				5									//�û��й�
#define SUB_C_RECORD				6									//������Ϸ��¼
//��Ư����
struct CMD_C_CallPiao
{
	BYTE	 cbPiao;
};
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
struct CMD_C_Listen
{
	BYTE							cbListen;							//�����û�
	BYTE							cbSelectCard[MAX_COUNT];			//����ס����
	BYTE							cbSelectCount;						//��ס�Ƶ�����
};

//�û��й�
struct CMD_C_Trustee
{
	bool							bTrustee;							//�Ƿ��й�	
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
