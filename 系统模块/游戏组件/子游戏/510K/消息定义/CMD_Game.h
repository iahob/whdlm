#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						238									//��Ϸ I D
#define GAME_NAME					TEXT("510K�����ư棩")						//��Ϸ����

//�������
#define GAME_PLAYER					4									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////

//��Ŀ����
#define FULL_COUNT					52									//ȫ����Ŀ
#define DISPATCH_COUNT				52									//������Ŀ
#define MAX_CARD_COUNT				13									//��������
#define NORMAL_CARD_COUNT			13									//��������
#define PUBLIC_CARD_COUNT			0									//����

//�߼�����
#define CT_ERROR					0									//��������
#define CT_SINGLE					1									//��������
#define CT_DOUBLE					2									//��������
#define CT_THREE						3									//��������
#define CT_SINGLE_LINE				4									//��������
#define CT_DOUBLE_LINE				5									//��������
#define CT_THREE_LINE				6									//��������
#define CT_THREE_TAKE_ONE			7									//����һ
#define CT_THREE_TAKE_TWO			8									//������
#define CT_THREE_LINE_TAKE_ONE		9									//�ɻ������1
#define CT_THREE_LINE_TAKE_TWO		10									//�ɻ������2
#define CT_510K_FALSE					11							
#define CT_510K_TRUE					12							
#define CT_BOMB_CARD				13									//ը������

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define GS_GAME_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GS_XUAN_ZHAN			GAME_STATUS_PLAY					//��ս
#define GS_FIND_FRIEND			GAME_STATUS_PLAY+1			//��ͬ��
#define GS_ASK_FRIEND			GAME_STATUS_PLAY+2			//ѯ��ͬ��
#define GS_ADD_TIMES				GAME_STATUS_PLAY+3			//�ӱ�
#define GS_PLAY						GAME_STATUS_PLAY+4			//��Ϸ����

//����״̬
struct CMD_S_StatusFree
{
	//ʱ����Ϣ
	BYTE							cbTimeStart;							//��ʼʱ��
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeXuanZhan;					//��սʱ��
	BYTE							cbTimeFindFriend;					//Ѱ��ʱ��
	BYTE							cbTimeAskFriend;					//ѯ��ʱ��
	BYTE							cbTimeAddTimes;					//�ӱ�ʱ��

	bool							b2Biggest;								//����2���
	//��ʷ����
	SCORE							lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];			//������Ϣ	
	
	//��Ϸ����
	LONG							lCellScore;							//��������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//ʱ����Ϣ
	BYTE							cbTimeStart;							//��ʼʱ��
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeXuanZhan;					//��սʱ��
	BYTE							cbTimeFindFriend;					//Ѱ��ʱ��
	BYTE							cbTimeAskFriend;					//ѯ��ʱ��
	BYTE							cbTimeAddTimes;					//�ӱ�ʱ��
	
	bool							b2Biggest;								//����2���
	BYTE							cbGameStatus;					//��Ϸ״̬
	bool							bEnabledAskFriend;			
	bool							bEnabledAddTimes;
	//��ʷ����
	bool								bTrustee[GAME_PLAYER];				//�й���Ϣ
	SCORE							lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];			//������Ϣ
	BYTE								cbTunrScore;									//��ǰ�Ʒ�
	int									cbGameScore[GAME_PLAYER];		//��Ϸ�÷�
	bool								bAddTimes[GAME_PLAYER];			//�Ƿ�ӱ�

	WORD							wFriend[GAME_PLAYER];				//����
	WORD							wXuanZhanUser;							//��ս���
	WORD							wAskUser;										//ѯ�ʵ����
	BYTE								cbFriendFlag;									//���ѱ��,1��ս��3������4����������δ֪
	BYTE								cbSelectedCardData;								//�������ѡ�����
	//��Ϸ����
	LONG							lCellScore;							//��Ԫ����
	WORD							wBankerUser;						//ׯ���û�
	WORD							wCurrentUser;						//��ǰ���
	BYTE								cbBaseTimes;						//��������
	//������Ϣ
	WORD							wTurnWiner;							//ʤ�����
	BYTE							cbTurnCardCount;					//������Ŀ
	BYTE							cbTurnCardData[MAX_CARD_COUNT];		//��������

	//�˿���Ϣ
	BYTE							cbHandCardData[MAX_CARD_COUNT];		//�����˿�
	BYTE							cbHandCardCount[GAME_PLAYER];		//�˿���Ŀ
};

//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_XUAN_ZHAN			101									//�û���ս
#define SUB_S_FIND_FRIEND			102									//�û���ͬ��
#define SUB_S_ASK_FRIEND			103									//�û���ͬ��
#define SUB_S_ADD_TIMES			104									//�û��ӱ�
#define SUB_S_OUT_CARD				105									//�û�����
#define SUB_S_PASS_CARD				106									//�û�����
#define SUB_S_GAME_CONCLUDE			107									//��Ϸ����
#define SUB_S_TRUSTEE					108										//�û��й�
#define SUB_S_SET_BASESCORE			109									//���û���
#define SUB_S_FRIEND							110									//����ͬ��
//�����˿�
struct CMD_S_GameStart
{
	bool							b2Biggest;								//����2���
	WORD						wBanker;
	WORD				 		wCurrentUser;						//��ǰ���
	BYTE							cbCardData[NORMAL_CARD_COUNT];		//�˿��б�
};

//�û���ս
struct CMD_S_XuanZhan
{
	WORD						wXuanZhanUser;						//��ս���
	bool							bXuanZhan;
	WORD						wCurrentUser;
	BYTE							cbFriendFlag;
};
//�û���ͬ��
struct CMD_S_FindFriend
{
	WORD						wFindUser;									//�������
	WORD						wCurrentUser;								//��ǰ���
	bool							bEnabled;										//ׯ���ܲ�����
	BYTE							cbSelectedCardData;						//ѡ�����
	WORD						wSelectedUser;								//ѡ������
	WORD						wFriend[GAME_PLAYER];				//������Ϣ
};
//�û���ͬ��
struct CMD_S_ASKFriend
{
	WORD						wAskUser;									//ѯ�����
	bool							bAsk;
	WORD						wCurrentUser;
	WORD						wFriend[GAME_PLAYER];			//����
	WORD						wXuanZhanUser;
	BYTE							cbFriendFlag;
	bool							bStart;										//ֱ�ӿ�ʼ��Ϸ
};
//�û��ӱ�
struct CMD_S_AddTimes
{
	WORD						wAddTimesUser;						//�ӱ����
	bool							bAddTimes;
	WORD						wCurrentUser;
};


//�û�����
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//������Ŀ
	WORD				 		wCurrentUser;						//��ǰ���
	WORD						wOutCardUser;						//�������
	BYTE							cbCurTurnScore;
	BYTE							cbCardData[MAX_CARD_COUNT];			//�˿��б�
	bool							bTrusteeOut;								//�Ƿ�ϵͳ�йܳ���
};

//��������
struct CMD_S_PassCard
{
	BYTE							cbTurnOver;							//һ�ֽ���
	WORD				 		wCurrentUser;						//��ǰ���
	WORD				 		wPassCardUser;						//�������
	BYTE							cbTurnWinnerScore;
	WORD						wTurnWinner;
	bool							bTrusteePass;
};
//�û��й�
struct CMD_S_Trustee
{
	WORD						wChair;
	bool							bTrustee;												
};
//����ͬ�˹�ϵ
struct CMD_S_Friend
{
	BYTE							cbFriendFlag;
	WORD						wXuanZhanUser;
	WORD						wFriend[GAME_PLAYER];			//����

};
//��Ϸ����
struct CMD_S_GameConclude
{
	//���ֱ���
	LONG							lCellScore;									//��Ԫ����
	SCORE							lGameScore[GAME_PLAYER];		//��Ϸ��Ӯ��
	BYTE								cbBaseTimes;								//����
	int									cbScore[GAME_PLAYER];			//�÷�
	BYTE								cbUserTimes[GAME_PLAYER];		//��ұ���
	//��Ϸ��Ϣ
	BYTE							cbCardCount[GAME_PLAYER];			//�˿���Ŀ
	BYTE							cbHandCardData[FULL_COUNT];			//�˿��б�
	bool							bShuangKou;										//�Ƿ�˫��
};

//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_C_XUAN_ZHAN			1									//�û���ս
#define SUB_C_FIND_FRIEND			2									//�û�ѡͬ��
#define SUB_C_ASK_FRIEND			3									//�û���ͬ��
#define SUB_C_ADD_TIMES			4									//�û��ӱ�
#define SUB_C_OUT_CARD			5									//�û�����
#define SUB_C_PASS_CARD			6									//�û�����
#define SUB_C_TRUSTEE				7									//�û��й�
//�û���ս
struct CMD_C_XuanZhan
{
	bool							bXuanZhan;						//�Ƿ���ս
};
//�û���ͬ��
struct CMD_C_FindFriend
{
	BYTE							cbCardData;						//ѡ���� 5,10,K
};
//�û���ͬ��
struct CMD_C_AskFriend
{
	bool							bAsk;								//�Ƿ���
};
//�û��ӱ�
struct CMD_C_AddTimes
{
	bool							bAddTimes;						//�Ƿ�ӱ�
};
//�û�����
struct CMD_C_OutCard
{
	BYTE							cbCardCount;											//������Ŀ
	BYTE							cbCardData[MAX_CARD_COUNT];			//�˿�����
};
//�û��й�
struct CMD_C_Trustee
{
	bool							bTrustee;												
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif