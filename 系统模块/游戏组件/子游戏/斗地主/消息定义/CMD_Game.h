#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						200									//��Ϸ I D
#define GAME_NAME					TEXT("������")						//��Ϸ����

//�������
#define GAME_PLAYER					3									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////

//��Ŀ����
#define MAX_COUNT					20									//�����Ŀ
#define FULL_COUNT					54									//ȫ����Ŀ

//�߼���Ŀ
#define NORMAL_COUNT				17									//������Ŀ
#define DISPATCH_COUNT				51									//�ɷ���Ŀ
#define GOOD_CARD_COUTN				38									//������Ŀ

//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

#define MAX_RECORD_COUNT			30									//��������ͳ��������

//�߼�����
#define CT_ERROR					0									//��������
#define CT_SINGLE					1									//��������
#define CT_DOUBLE					2									//��������
#define CT_THREE					3									//��������
#define CT_SINGLE_LINE				4									//��������
#define CT_DOUBLE_LINE				5									//��������
#define CT_THREE_LINE				6									//��������
#define CT_THREE_TAKE_ONE			7									//����һ��
#define CT_THREE_TAKE_TWO			8									//����һ��
#define CT_FOUR_TAKE_ONE			9									//�Ĵ�����
#define CT_FOUR_TAKE_TWO			10									//�Ĵ�����
#define CT_BOMB_CARD				11									//ը������
#define CT_MISSILE_CARD				12									//�������

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_CALL				GAME_STATUS_PLAY					//�з�״̬
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY+1					//��Ϸ����

typedef struct
{
	//�±�Ϊ0��ʼ����Ϊ ������С����2 A K Q J ~~3,ʣ������0Ϊ��ʣ��
	BYTE cbRemainCount[15];
}REMAINCARDCOUNT;

typedef struct
{
	WORD							wFaceID;							
	DWORD							dwCustomID;							
	DWORD							dwGameID;
	TCHAR							szNickName[LEN_NICKNAME];
	BYTE                            cbBombCount;
	BYTE                            cbPlaneCount;
	BYTE                            cbMissileCount;
	BYTE                            cbChunTianCount;
	BYTE                            cbFanChunTianCount;
}USERACHIEVEMENT;

//����״̬
struct CMD_S_StatusFree
{
	//��Ϸ����
	LONG							lCellScore;							//��������

	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeCallScore;					//�з�ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��

	//��ʷ����
	SCORE							lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];			//������Ϣ

	//���� 0 ���ֳ�	1 ��ҳ�  
	BYTE							cbPlayMode;							//Լս��Ϸģʽ
	REMAINCARDCOUNT					remainCardCount;					//ʣ���˿�����
};

//�з�״̬
struct CMD_S_StatusCall
{
	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeCallScore;					//�з�ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��

	//��Ϸ��Ϣ
	LONG							lCellScore;							//��Ԫ����
	WORD							wCurrentUser;						//��ǰ���
	BYTE							cbBankerScore;						//ׯ�ҽз�
	BYTE							cbScoreInfo[GAME_PLAYER];			//�з���Ϣ
	BYTE							cbHandCardData[NORMAL_COUNT];		//�����˿�
	BYTE							cbUserTrustee[GAME_PLAYER];			//�йܱ�־
	//��ʷ����
	SCORE							lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];			//������Ϣ

	LONG							lScoreTimes;						//����

	//���� 0 ���ֳ�	1 ��ҳ�
	BYTE							cbPlayMode;							//Լս��Ϸģʽ
	REMAINCARDCOUNT					remainCardCount;					//ʣ���˿�����
	BYTE							cbTimeRemain;						//����ʣ������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeCallScore;					//�з�ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��

	//��Ϸ����
	LONG							lCellScore;							//��Ԫ����
	BYTE							cbBombCount;						//ը������
	WORD							wBankerUser;						//ׯ���û�
	WORD							wCurrentUser;						//��ǰ���
	BYTE							cbBankerScore;						//ׯ�ҽз�

	//������Ϣ
	WORD							wTurnWiner;							//ʤ�����
	BYTE							cbTurnCardCount;					//������Ŀ
	BYTE							cbTurnCardData[MAX_COUNT];			//��������

	//�˿���Ϣ
	BYTE							cbBankerCard[3];					//��Ϸ����
	BYTE							cbHandCardData[MAX_COUNT];			//�����˿�
	BYTE							cbHandCardCount[GAME_PLAYER];		//�˿���Ŀ

	//��ʷ����
	SCORE							lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];			//������Ϣ
	BYTE							cbUserTrustee[GAME_PLAYER];			//�йܱ�־

	LONG							lScoreTimes;						//����

	//���� 0 ���ֳ�	1 ��ҳ�
	BYTE							cbPlayMode;							//Լս��Ϸģʽ
	REMAINCARDCOUNT					remainCardCount;					//ʣ���˿�����
	BYTE							cbTimeRemain;						//����ʣ������
};

//////////////////////////////////////////////////////////////////////////////////
//�����
#define SUB_S_REQUEST_CUR_DEBUG		1									//��ѯ��ǰ����
#define SUB_S_REQUEST_HISTORY_DEBUG	2									//��ѯ��ʷ����
#define SUB_S_REQUEST_DELETE_DEBUG	3									//ɾ������
#define SUB_S_UPDATE_CUR_DEBUG		4									//���µ�ǰ����
#define SUB_S_DEBUG_TEXT			5									//������ʾ

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_CALL_SCORE			101									//�û��з�
#define SUB_S_BANKER_INFO			102									//ׯ����Ϣ
#define SUB_S_OUT_CARD				103									//�û�����
#define SUB_S_PASS_CARD				104									//�û�����
#define SUB_S_GAME_CONCLUDE			105									//��Ϸ����
#define SUB_S_SET_BASESCORE			106									//���û���
#define SUB_S_DEBUG_CARD			107									//�����˿�
#define SUB_S_TRUSTEE				108									//�й�
#define SUB_S_ROOMCARD_RECORD		109									//������¼

//�����˿�
struct CMD_S_GameStart
{
	LONG							lCellScore;							//�׷�
	WORD							wStartUser;							//��ʼ���
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbValidCardData;					//�����˿�
	BYTE							cbValidCardIndex;					//����λ��
	BYTE							cbCardData[NORMAL_COUNT];			//�˿��б�
};

//AI�˿�
struct CMD_S_AndroidCard
{
	BYTE							cbHandCard[GAME_PLAYER][NORMAL_COUNT];//�����˿�
	BYTE							cbBankerCard[3];
	WORD							wCurrentUser ;						//��ǰ���
};

//�����˿�
struct CMD_S_DebugCard
{
	WORD							wCardUser[GAME_PLAYER ];				//�������
	BYTE							cbUserCount;							//��������
	BYTE							cbCardData[GAME_PLAYER ][MAX_COUNT];	//�˿��б�
	BYTE							cbCardCount[GAME_PLAYER ];				//�˿�����

};
//�û��з�
struct CMD_S_CallScore
{
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wCallScoreUser;						//�з����
	BYTE							cbCurrentScore;						//��ǰ�з�
	BYTE							cbUserCallScore;					//�ϴνз�
};

//ׯ����Ϣ
struct CMD_S_BankerInfo
{
	WORD				 			wBankerUser;						//ׯ�����
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbBankerScore;						//ׯ�ҽз�
	BYTE							cbBankerCard[3];					//ׯ���˿�

	LONG							lScoreTimes;						//����
};

//�û�����
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//������Ŀ
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wOutCardUser;						//�������
	LONG							lScoreTimes;						//����
	REMAINCARDCOUNT					remainCardCount;					//ʣ���˿�����
	BYTE							cbCardData[MAX_COUNT];				//�˿��б�
};

//��������
struct CMD_S_PassCard
{
	BYTE							cbTurnOver;							//һ�ֽ���
	WORD				 			wCurrentUser;						//��ǰ���
	WORD				 			wPassCardUser;						//�������
};

//��Ϸ����
struct CMD_S_GameConclude
{
	//���ֱ���
	LONG							lCellScore;							//��Ԫ����
	SCORE							lGameScore[GAME_PLAYER];			//��Ϸ����

	//�����־
	BYTE							bChunTian;							//�����־
	BYTE							bFanChunTian;						//�����־

	//ը����Ϣ
	BYTE							cbBombCount;						//ը������
	BYTE							cbEachBombCount[GAME_PLAYER];		//ը������

	//��Ϸ��Ϣ
	BYTE							cbBankerScore;						//�з���Ŀ
	BYTE							cbCardCount[GAME_PLAYER];			//�˿���Ŀ
	BYTE							cbHandCardData[FULL_COUNT];			//�˿��б�

	LONG							lScoreTimes;						//����
	REMAINCARDCOUNT					remainCardCount;					//ʣ���˿�����
};

//�й�
struct CMD_S_TRUSTEE
{
	WORD							wTrusteeUser;						//�й����
	BYTE							bTrustee;							//�йܱ�־
};

//������¼
struct CMD_S_RoomCardRecord
{
	WORD							nCount;											//����
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//���ֽ����
};

//////////////////////////////////////////////////////////////////////////
//¼������
struct Video_GameStart
{	
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�		
	WORD							wTableID;							//����ID
	WORD							wStartUser;							//��ʼ���
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbValidCardData;					//�����˿�
	BYTE							cbValidCardIndex;					//����λ��
	BYTE							cbCardData[NORMAL_COUNT];			//�˿��б�
	SCORE							lScore;								//����
	LONG							lCellScore;							//�׷�
};

//������Ϣ
struct CMD_S_HistoryDebugInfo
{
	DWORD							dwDebugID;							//����ID
	WORD							wTableID;							//���Ӻ�
	int								nCardLevel;							//���Ƶȼ�
	BYTE							cbCardChance;						//���Ƹ���
	int								nDebugTime[3];						//��������
	int								nStartTime;							//��ʼʱ��
	int								nEndTime;							//����ʱ��
	SCORE							lSystemScore;						//���Ե÷�
};

//������Ϣ
struct CMD_S_CurDebugInfo
{
	DWORD							dwDebugID;						//����ID
	WORD							wTableID;							//���Ӻ�
	int								nCardLevel;							//���Ƶȼ�
	BYTE							cbCardChance;						//���Ƹ���
	int								nStartTime;							//��ʼʱ��
	int								nEndTime;							//����ʱ��
	SCORE							lSystemScore;						//���Ե÷�
};

//ɾ������
struct CMD_S_DeleteDebug
{
	DWORD							dwDebugID;						//����ID
};

//���µ���
struct CMD_S_UpdateDebug
{
	DWORD							dwDebugID;						//����ID
	SCORE							lSystemScore;						//���Ե÷�
};

//������ʾ
struct CMD_S_DebugText
{
	TCHAR							szMessageText[128];					//�����ı�
};

//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_C_CALL_SCORE				1									//�û��з�
#define SUB_C_OUT_CARD					2									//�û�����
#define SUB_C_PASS_CARD					3									//�û�����
#define SUB_C_TRUSTEE					4									//�û��й�
#define SUB_C_REQUEST_RCRecord			5									//��ѯ������¼

#define SUB_C_REQUEST_CUR_DEBUG			6									//��ѯ��ǰ����
#define SUB_C_REQUEST_HISTORY_DEBUG		7									//��ѯ��ʷ����
#define SUB_C_REQUEST_ADD_DEBUG			8									//���ӵ���
#define SUB_C_REQUEST_MODIFY_DEBUG		9									//�޸ĵ���
#define SUB_C_REQUEST_DELETE_DEBUG		10									//ɾ������

//�û��з�
struct CMD_C_CallScore
{
	BYTE							cbCallScore;						//�з���Ŀ
};

//�û�����
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//������Ŀ
	BYTE							cbCardData[MAX_COUNT];				//�˿�����
};

//�й�
struct CMD_C_TRUSTEE
{
	BYTE							bTrustee;							//�йܱ�־
};

//���ӵ���
struct CMD_C_AddDebug
{
	WORD							wTableID;							//���Ӻ�
	int								nCardLevel;							//���Ƶȼ�
	BYTE							cbCardChance;						//���Ƹ���
	int								nStartTime;							//��ʼʱ��
	int								nEndTime;							//����ʱ��
};

//�޸ĵ���
struct CMD_C_ModifyDebug
{
	DWORD							dwDebugID;							//����ID
	WORD							wTableID;							//���Ӻ�
	int								nCardLevel;							//���Ƶȼ�
	BYTE							cbCardChance;						//���Ƹ���
	int								nStartTime;							//��ʼʱ��
	int								nEndTime;							//����ʱ��
};

//ɾ������
struct CMD_C_DeleteDebug
{
	DWORD							dwDebugID;							//����ID
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif