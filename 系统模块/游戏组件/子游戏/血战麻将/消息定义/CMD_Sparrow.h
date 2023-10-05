#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

#pragma pack(1)

#define		CARD_CONFIG

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						302									//��Ϸ I D
#define GAME_NAME					TEXT("Ѫս�齫")					//��Ϸ����

//�������
#define GAME_PLAYER					4									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//��Ϸ״̬
#define GS_MJ_FREE					GAME_STATUS_FREE					//����״̬
#define GS_MJ_CHANGECARD			GAME_STATUS_PLAY					//������״̬������˿����ã��������ã��򲻽�ȥ��״̬��
#define GS_MJ_CALL					GAME_STATUS_PLAY+1					//ѡȱ״̬
#define GS_MJ_PLAY					GAME_STATUS_PLAY+2					//��Ϸ״̬

//��������
#define MAX_WEAVE					4									//������
#define MAX_INDEX					27									//���������û�����ƺͷ��ƣ�
#define MAX_COUNT					14									//�����Ŀ
#define MAX_REPERTORY				108									//�����
#define MAX_HUA_INDEX				0

//�˿˶���
#define HEAP_FULL_COUNT				26									//����ȫ��
#define MAX_RIGHT_COUNT				1									//���ȨλDWORD����			

#define MAX_RECORD_COUNT			32									//��������ͳ��������

//������
#define CLOCKWISE_CHANGE			0									//˳ʱ�뻻��
#define ANTI_CLOCKWISE_CHANGE		1									//��ʱ�뻻��
#define OPPOSITE_CHANGE				2									//�Լһ���
#define MAX_CHANGE_TYPE				3									//�����ŷ�ʽ����
#define MAX_CHANGE_CARDCOUNT		3									//������������

//�Ծ���ˮ�嵥
#define MAX_CHART_COUNT				20									//���Ծ���ˮ����
#define LEN_SERVER					32
//////////////////////////////////////////////////////////////////////////

//�������
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbPublicCard;						//������־
	WORD							wProvideUser;						//��Ӧ�û�
};

struct CMD_S_RECORD
{
	//���ܺͲ���Ϊ���η硱������Ϊ������
	int								nCount;
	BYTE							cbGuaFeng[GAME_PLAYER];							//�η����
	BYTE							cbXiaYu[GAME_PLAYER];							//�������

	BYTE							cbFangPao[GAME_PLAYER];							//���ڴ���
	BYTE							cbZiMo[GAME_PLAYER];							//��������

	LONGLONG						lAllScore[GAME_PLAYER];							//�ܽ����
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//���ֽ����
};

//�嵥����
typedef enum
{
	INVALID_CHARTTYPE = 0,
	GUAFENG_TYPE = 22,
	BEIGUAFENG_TYPE = 23,
	XIAYU_TYPE = 24,
	BEIXIAYU_TYPE = 25,
	DIANPAO_TYPE = 26,
	BEIDIANPAO_TYPE = 27,
	ZIMO_TYPE = 28,
	BEIZIMO_TYPE = 29,
	HUJIAOZHUANYI_TYPE = 30,
	TUISHUI_TYPE = 31,
	CHAHUAZHU_TYPE = 32,
	CHADAJIAO_TYPE = 33,
}CHARTTYPE;

//�Ծ���ˮ�嵥
struct tagChart
{
	CHARTTYPE		charttype;											//�嵥����
	LONGLONG		lTimes;												//����
	LONGLONG		lScore;												//�÷�
	bool			bEffectChairID[GAME_PLAYER];						//���õ�����ID��falseΪ������
};

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_CALL_CARD				101									//ȱ������
#define SUB_S_BANKER_INFO			102									//ׯ����Ϣ
#define SUB_S_OUT_CARD				103									//��������
#define SUB_S_SEND_CARD				104									//�����˿�
#define SUB_S_OPERATE_NOTIFY		105									//������ʾ
#define SUB_S_OPERATE_RESULT		106									//��������
#define SUB_S_GAME_END				107									//��Ϸ����
#define SUB_S_TRUSTEE				108									//�û��й�
#define SUB_S_CHI_HU				109									//�û��Ժ�
#define SUB_S_GANG_SCORE			110									//���Ƶ÷�
#define SUB_S_WAIT_OPERATE			111									//�ȴ�����
#define SUB_S_RECORD				112									//��Ϸ��¼
#define SUB_S_CHANGE_CARD_RESULT	113									//�����Ž��

#define SUB_S_ASK_HANDCARD			114									//�����Ž��
#define SUB_S_VIEW_CHART			115									//��ˮ���
#define SUB_S_ROOMCARD_RECORD		116									//������¼

//����״̬
struct CMD_S_StatusFree
{
	LONGLONG						lCellScore;							//�������
	WORD							wBankerUser;						//ׯ���û�
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�
	bool							bCheatMode;							//�����ױ�ʶ

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ

	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeOperateCard;					//����ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeWaitEnd;						//����ȴ�

	bool							bHuanSanZhang;						//trueΪ��������
	bool							bHuJiaoZhuanYi;						//true�������ת��
	bool							bZiMoAddTimes;						//true���������ӱ�
	bool							bTianDiHu;							//trueΪ������غ��ӱ�
	bool							bHaiDiLaoYue;						//true���������¼ӱ�
	bool							bMenQing;							//true��������ӱ�
	BYTE							cbPlayMode;							//0���ֳ���1��ҳ�
	WORD							wServerLevel;						//����ȼ�
};

//������״̬
struct CMD_S_StatusChangeCard
{
	//��Ϸ����
	LONGLONG						lCellScore;							//�������
	WORD							wBankerUser;						//ׯ���û�
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�

	//�˿�����
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbCardData[MAX_COUNT];				//�˿��б�
	BYTE							cbSendCardData;						//�����˿�

	//������Ϣ
	WORD							wHeapHand;							//����ͷ��
	WORD							wHeapTail;							//����β��
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//������Ϣ

	bool							bChangeCard[GAME_PLAYER];			//�����ű�ʶ
	BYTE							cbChangeCardDataBefore[GAME_PLAYER][MAX_CHANGE_CARDCOUNT];	//��bChangeCardΪtrue����cbChangeCardDataBeforeΪѡ�����ŵ���
	BYTE							cbPriorChangeCardData[MAX_CHANGE_CARDCOUNT]; //ǰ���������������˿�(����֧�ֻ�����Ϊ0)

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ

	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeOperateCard;					//����ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeWaitEnd;						//����ȴ�

	bool							bHuanSanZhang;						//trueΪ��������
	bool							bHuJiaoZhuanYi;						//true�������ת��
	bool							bZiMoAddTimes;						//true���������ӱ�
	bool							bTianDiHu;							//trueΪ������غ��ӱ�
	bool							bHaiDiLaoYue;						//true���������¼ӱ�
	bool							bMenQing;							//true��������ӱ�
	BYTE							cbPlayMode;							//0���ֳ���1��ҳ�
	WORD							wServerLevel;						//����ȼ�
	BYTE							cbTimeRemain;						//����ʣ������
};

//ѡȱ״̬
struct CMD_S_StatusCall
{
	//��Ϸ����
	LONGLONG						lCellScore;							//�������
	WORD							wBankerUser;						//ׯ���û�
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�

	//ȱ����Ϣ
	bool							bCallCard[GAME_PLAYER];				//ȱ��״̬
	BYTE							cbCallCard[GAME_PLAYER];			//ȱ������

	//�˿�����
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbCardData[MAX_COUNT];				//�˿��б�
	BYTE							cbSendCardData;						//�����˿�

	//������Ϣ
	WORD							wHeapHand;							//����ͷ��
	WORD							wHeapTail;							//����β��
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//������Ϣ

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ

	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeOperateCard;					//����ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeWaitEnd;						//����ȴ�

	bool							bHuanSanZhang;						//trueΪ��������
	bool							bHuJiaoZhuanYi;						//true�������ת��
	bool							bZiMoAddTimes;						//true���������ӱ�
	bool							bTianDiHu;							//trueΪ������غ��ӱ�
	bool							bHaiDiLaoYue;						//true���������¼ӱ�
	bool							bMenQing;							//true��������ӱ�
	BYTE							cbPlayMode;						//0���ֳ���1��ҳ�
	WORD							wServerLevel;						//����ȼ�
	BYTE							cbTimeRemain;						//����ʣ������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��Ϸ����
	LONGLONG						lCellScore;							//��Ԫ����
	WORD							wBankerUser;						//ׯ���û�
	WORD							wCurrentUser;						//��ǰ�û�

	//״̬����
	BYTE							cbActionCard;						//�����˿�
	BYTE							cbActionMask;						//��������
	BYTE							cbLeftCardCount;					//ʣ����Ŀ
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�
	WORD							wWinOrder[GAME_PLAYER];				//����˳��
	BYTE							cbCallCard[GAME_PLAYER];			//ȱ����Ϣ

	//������Ϣ
	WORD							wOutCardUser;						//�����û�
	BYTE							cbOutCardData;						//�����˿�
	BYTE							cbDiscardCount[GAME_PLAYER];		//������Ŀ
	BYTE							cbDiscardCard[GAME_PLAYER][60];		//������¼

	//�˿�����
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbCardData[MAX_COUNT];				//�˿��б�
	BYTE							cbChiHuCardArray[GAME_PLAYER];		//�������飬��0��־δ����
	BYTE							cbSendCardData;						//�����˿�

	//����˿�
	BYTE							cbWeaveCount[GAME_PLAYER];			//�����Ŀ
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//����˿�

	//������Ϣ
	WORD							wHeapHand;							//����ͷ��
	WORD							wHeapTail;							//����β��
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//������Ϣ

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ

	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeOperateCard;					//����ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeWaitEnd;						//����ȴ�

	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//�Ծ���ˮ�嵥

	bool							bHuanSanZhang;						//trueΪ��������
	bool							bHuJiaoZhuanYi;						//true�������ת��
	bool							bZiMoAddTimes;						//true���������ӱ�
	bool							bTianDiHu;							//trueΪ������غ��ӱ�
	bool							bHaiDiLaoYue;						//true���������¼ӱ�
	bool							bMenQing;							//true��������ӱ�
	BYTE							cbPlayMode;						//0���ֳ���1��ҳ�
	WORD							wServerLevel;						//����ȼ�
	BYTE							cbTimeRemain;						//����ʣ������
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	LONG							lSiceCount;							//���ӵ���
	WORD							wBankerUser;						//ׯ���û�
	WORD							wCurrentUser;						//��ǰ�û�
	BYTE							cbUserAction;						//�û�����
	BYTE							cbCardData[MAX_COUNT*GAME_PLAYER];	//�˿��б�
	WORD							wHeapHand;							//������ͷ
	WORD							wHeapTail;							//������β
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//������Ϣ
	BYTE							cbLeftCardCount;					//ʣ���˿�
	BYTE							cbSendCardData;						//�����˿� (����ׯ�Ҹ�ֵΪINVALID_BYTE)
	bool							bHuanSanZhang;						//�����ű�־

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ
	BYTE							cbPriorChangeCardData[MAX_CHANGE_CARDCOUNT]; //ǰ���������������˿�(����֧�ֻ�����Ϊ0)
};

//ȱ������
struct CMD_S_CallCard
{
	WORD							wCallUser;							//�����û�
	BYTE							cbCallCard;							//ȱ������
};

//ׯ����Ϣ
struct CMD_S_BankerInfo
{
	WORD				 			wBankerUser;						//ׯ�����
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbCallCard[GAME_PLAYER];			//ȱ������
	BYTE							cbUserAction;						//�û�����
};

//��������
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//�����û�
	BYTE							cbOutCardData;						//�����˿�
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

//������ʾ
struct CMD_S_WaitOperate
{
	WORD							wProvideUser;						//��Ӧ�û�
};

//��������
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//�����û�
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbOperateCode;						//��������
	BYTE							cbOperateCard;						//�����˿�

	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//�Ծ���ˮ�嵥
};

//��Ϸ����
struct CMD_S_GameEnd
{
	//�˿˱���
	BYTE							cbCardCount[GAME_PLAYER];			//�˿���Ŀ
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//�˿�����

	//������Ϣ
	WORD							wProvideUser[GAME_PLAYER];			//��Ӧ�û�
	DWORD							dwChiHuRight[GAME_PLAYER];			//��������

	//������Ϣ
	LONGLONG						lGameScore[GAME_PLAYER];			//��Ϸ����
	LONG							lGameTax[GAME_PLAYER];				//��Ϸ˰��

	WORD							wWinOrder[GAME_PLAYER];				//��������

	LONGLONG						lGangScore[GAME_PLAYER];			//��ϸ�÷�
	LONGLONG						lHuaZhuScore[GAME_PLAYER][GAME_PLAYER];//����÷�
	LONGLONG						lChaJiaoScore[GAME_PLAYER][GAME_PLAYER];//���յ÷�
	LONGLONG						lLostHuaZhuScore[GAME_PLAYER][GAME_PLAYER];//����÷�
	LONGLONG						lLostChaJiaoScore[GAME_PLAYER][GAME_PLAYER];//���յ÷�
	WORD							wLeftUser;							//�����û�

	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//�Ծ���ˮ�嵥
};

//�û��й�
struct CMD_S_Trustee
{
	bool							bTrustee;							//�Ƿ��й�
	WORD							wChairID;							//�й��û�
};

//�Ժ���Ϣ
struct CMD_S_ChiHu
{
	WORD							wChiHuUser;							//�Ժ��û�
	WORD							wProviderUser;						//�ṩ�û�
	BYTE							cbChiHuCard;						//��������
	DWORD							dwChiHuRight;						//����Ȩλ
	BYTE							cbCardCount;						//�˿���Ŀ
	LONGLONG						lGameScore;							//��Ϸ����
	BYTE							cbWinOrder;							//����˳��

	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//�Ծ���ˮ�嵥
};

//���Ʒ���
struct CMD_S_GangScore
{
	WORD							wChairId;							//�����û�
	BYTE							cbXiaYu;							//�η�����
	LONGLONG						lGangScore[GAME_PLAYER];			//���Ʒ���
};

//�����Ž��
struct CMD_S_ChangeCardResult
{
	BYTE							cbChangeType;						//0˳ʱ�� 1��ʱ�� 2�Լ�
	BYTE							cbChangeCardResult[MAX_CHANGE_CARDCOUNT];	//�������˿�
	BYTE							cbSendCardData;						//�����˿�
};

struct CMD_S_Askhandcard
{
	BYTE							cbCardData[MAX_COUNT];
};

//��ˮ��Ϣ
struct CMD_S_ViewChart
{
	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//�Ծ���ˮ�嵥
};

//������¼
struct CMD_S_RoomCardRecord
{
	WORD							nCount;											//����
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//���ֽ����
};

///////////////////////////////////////////////////////////

//¼������
struct Video_GameStart
{
	bool							bHuanSanZhang;						//trueΪ��������
	bool							bHuJiaoZhuanYi;						//true�������ת��
	bool							bZiMoAddTimes;						//true���������ӱ�
	bool							bTianDiHu;							//trueΪ������غ��ӱ�
	bool							bHaiDiLaoYue;						//true���������¼ӱ�
	bool							bMenQing;							//true��������ӱ�

	LONG							lSiceCount;							//���ӵ���
	WORD							wBankerUser;						//ׯ���û�
	WORD							wCurrentUser;						//��ǰ�û�
	BYTE							cbUserAction;						//�û�����
	BYTE							cbCardData[MAX_COUNT*GAME_PLAYER];	//�˿��б�
	BYTE							cbLeftCardCount;					//ʣ���˿�
	BYTE							cbSendCardData;						//�����˿� (����ׯ�Ҹ�ֵΪINVALID_BYTE)

	TCHAR							szNickName[32];						//�û��ǳ�		
	WORD							wChairID;							//����ID
	LONGLONG						lScore;								//����
};

//��������
struct Video_OperateResult
{
	WORD							wOperateUser;						//�����û�
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbOperateCode;						//��������
	BYTE							cbOperateCard;						//�����˿�
};

//�Ժ���Ϣ
struct Video_ChiHu
{
	WORD							wChiHuUser;							//�Ժ��û�
	WORD							wProviderUser;						//�ṩ�û�
	BYTE							cbChiHuCard;						//��������
	BYTE							cbCardCount;						//�˿���Ŀ
	LONGLONG						lGameScore;							//��Ϸ����
	BYTE							cbWinOrder;							//����˳��
};

//��Ϸ����
struct Video_GameEnd
{
	//�˿˱���
	BYTE							cbCardCount[GAME_PLAYER];			//�˿���Ŀ
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//�˿�����

	//����˿�
	BYTE							cbWeaveCount[GAME_PLAYER];			//�����Ŀ
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//����˿�

	BYTE							cbChiHuCardArray[GAME_PLAYER];		//�Ժ��˿�����

	//������Ϣ
	WORD							wProvideUser[GAME_PLAYER];			//��Ӧ�û�
	DWORD							dwChiHuRight[GAME_PLAYER];			//��������

	//������Ϣ
	LONGLONG						lGameScore[GAME_PLAYER];			//��Ϸ����
	LONG							lGameTax[GAME_PLAYER];				//��Ϸ˰��

	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//�Ծ���ˮ�嵥
};

//�����Ž��
struct Video_ChangeCardResult
{
	BYTE							cbChangeType;						//0˳ʱ�� 1��ʱ�� 2�Լ�
	BYTE							cbChangeCardDataBefore[GAME_PLAYER][MAX_CHANGE_CARDCOUNT];	//�������˿�(��֮ǰ)
	BYTE							cbChangeCardDataAfter[GAME_PLAYER][MAX_CHANGE_CARDCOUNT];		//�������˿�(��֮��)
	BYTE							cbSendCardData;						//�����˿�
};

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_CALL_CARD				1									//�û�ȱ��
#define SUB_C_OUT_CARD				2									//��������
#define SUB_C_OPERATE_CARD			3									//�����˿�
#define SUB_C_TRUSTEE				4									//�û��й�
#define SUB_C_CHANGE_CARD			5									//������
#define SUB_C_ASK_HANDCARD			11

#define SUB_C_REQUEST_VIEWCHART		15									//������ˮ��¼

#ifdef CARD_CONFIG
#define SUB_C_CARD_CONFIG			12									//����
#endif

#define SUB_C_REQUEST_RCRecord		16									//��ѯ������¼


//�û�ȱ��
struct CMD_C_CallCard
{
	BYTE							cbCallCard;							//ȱ������
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
	BYTE							cbOperateCard;						//�����˿�
};

//�û��й�
struct CMD_C_Trustee
{
	bool							bTrustee;							//�Ƿ��й�	
};

//������
struct CMD_C_ChangeCard
{
	BYTE							cbChangeCardData[MAX_CHANGE_CARDCOUNT];	//�������˿�
};

#ifdef CARD_CONFIG
struct CMD_C_CardConfig
{
	BYTE							cbConfigCard[MAX_REPERTORY];		//�����˿�
};
#endif
//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
