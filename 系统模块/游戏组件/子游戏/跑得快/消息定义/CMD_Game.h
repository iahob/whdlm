#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						240									//��Ϸ I D
#define GAME_NAME					TEXT("�ܵÿ�")						//��Ϸ����

//�������
#define GAME_PLAYER					3									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////
//״̬����
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					//��Ϸ����
#define GAME_SCENE_WAITEND			GAME_STATUS_PLAY+1					//����ȴ�

#define MAX_RECORD_COUNT			30									//��������ͳ��������
#define NORMAL_CARD_COUNT			16									//��������
#define LEN_NICKNAME				32
//////////////////////////////////////////////////////////////////////////

//����������ṹ
#define SUB_S_GAME_START			101									//��Ϸ��ʼ
#define SUB_S_OUT_CARD				102									//�û�����
#define SUB_S_PASS_CARD				103									//��������
#define SUB_S_GAME_END				104									//��Ϸ����
#define SUB_S_AUTOMATISM			105									//�й���Ϣ
#define SUB_S_ROOMCARD_RECORD		116									//������¼

//////////////////////////////////////////////////////////////////////////

typedef struct
{
	//�±�Ϊ0��ʼ����Ϊ2 ~3,ʣ������INVALID_BYTEΪ��ʣ��
	BYTE cbRemainCount[13];
}REMAINCARDCOUNT;

//��Ϸ����
struct CMD_S_GameEnd
{
	LONGLONG						lGameTax;									//��Ϸ˰��
	LONGLONG						lGameScore[GAME_PLAYER];					//��Ϸ����
	BYTE							bCardCount[GAME_PLAYER];					//�˿���Ŀ
	BYTE							bCardData[NORMAL_CARD_COUNT * GAME_PLAYER];	//�˿��б� 
	BYTE							bBombCount[GAME_PLAYER];					//ը����Ŀ
	LONGLONG						lBombScore[GAME_PLAYER];					//ը����
	REMAINCARDCOUNT					remainCardCount;							//ʣ���˿�����
};

//��Ϸ״̬
struct CMD_S_StatusFree
{
	LONGLONG						lBaseScore;							//��������

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ
	bool							bAutoStatus[GAME_PLAYER];			//�й�״̬

	//�Զ������
	WORD							wPlayerCount;						//��������
	bool							bSixTheenCard;						//��Ϸģʽ
	bool							bNoForceClose;						//����ǿ��
	bool							bNoRemoveBomb;						//ը�����ɲ�
	bool							bSpadeBanker;						//����3ׯ
	bool							bFirstOutSpade;						//�׳��ش�����3

	bool							bForceOutCard;						//�д�س�
	bool							bEchelonConclude;					//�ݶȽ���

	BYTE							cbPlayMode;							//Լս��Ϸģʽ
	WORD							wServerLevel;						//����ȼ�
	REMAINCARDCOUNT					remainCardCount;					//ʣ���˿�����
	//ʱ������
	BYTE							cbTimeoutOutCard;				//���Ƴ�ʱ
	BYTE							cbTimeoutGameEnd;				//���㳬ʱ
	BYTE							cbTimeoutReady;					//׼����ʱ
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	LONGLONG						lBaseScore;							//��������
	WORD							wBankerUser;						//ׯ���û�
	WORD							wBeforWiner;						//�Ͼ�Ӯ��
	WORD							wLastOutUser;						//���Ƶ���
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							bCardData[NORMAL_CARD_COUNT];		//�����˿�
	BYTE							bCardCount[GAME_PLAYER];			//�˿���Ŀ
	BYTE							bBombCount[GAME_PLAYER];			//ը����Ŀ
	BYTE							bTurnCardCount;						//��������
	BYTE							bTurnCardData[NORMAL_CARD_COUNT];	//�����б�
	LONGLONG                        lAllTurnScore[GAME_PLAYER];			//�ֵܾ÷�
	LONGLONG                        lLastTurnScore[GAME_PLAYER];		//�Ͼֵ÷�
	bool							bAutoStatus[GAME_PLAYER];			//�й�״̬
	WORD							wRoomType;							//��������

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ

	//�Զ������
	WORD							wPlayerCount;						//��������
	bool							bSixTheenCard;						//��Ϸģʽ
	bool							bNoForceClose;						//����ǿ��
	bool							bNoRemoveBomb;						//ը�����ɲ�
	bool							bSpadeBanker;						//����3ׯ
	BYTE							cbSpadeBankerFirstOutCardData;
	BYTE							cbWinBankerFirstOutCardData;
	bool							bFirstOutSpade;						//�׳��ش�����3

	bool							bForceOutCard;						//�д�س�
	bool							bEchelonConclude;					//�ݶȽ���

	BYTE							cbPlayMode;							//Լս��Ϸģʽ
	WORD							wServerLevel;						//����ȼ�
	REMAINCARDCOUNT					remainCardCount;					//ʣ���˿�����
	BYTE							cbTimeRemain;						//����ʣ������
	//ʱ������
	BYTE							cbTimeoutOutCard;				//���Ƴ�ʱ
	BYTE							cbTimeoutGameEnd;				//���㳬ʱ
	BYTE							cbTimeoutReady;					//׼����ʱ
};
//����״̬
struct CMD_S_StatusGameEnd
{
	LONGLONG						lBaseScore;							//��������

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ
	bool							bAutoStatus[GAME_PLAYER];			//�й�״̬

	//�Զ������
	WORD							wPlayerCount;						//��������
	bool							bSixTheenCard;						//��Ϸģʽ
	bool							bNoForceClose;						//����ǿ��
	bool							bNoRemoveBomb;						//ը�����ɲ�
	bool							bSpadeBanker;						//����3ׯ
	bool							bFirstOutSpade;						//�׳��ش�����3

	bool							bForceOutCard;						//�д�س�
	bool							bEchelonConclude;					//�ݶȽ���

	BYTE							cbPlayMode;							//Լս��Ϸģʽ
	WORD							wServerLevel;						//����ȼ�
	REMAINCARDCOUNT					remainCardCount;					//ʣ���˿�����
	//ʱ������
	BYTE							cbTimeoutOutCard;					//���Ƴ�ʱ
	BYTE							cbTimeoutGameEnd;					//���㳬ʱ
	BYTE							cbTimeoutReady;						//׼����ʱ

	//
	CMD_S_GameEnd					stGameEnd;							//��������
};
//�����˿�
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//ׯ���û�
	WORD							wBeforWiner;						//�Ͼ�Ӯ��
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbCardData[NORMAL_CARD_COUNT];		//�˿��б�
	bool							bSixTheenCard;						//��Ϸģʽ
	bool							bNoForceClose;						//����ǿ��
	bool							bNoRemoveBomb;						//ը�����ɲ�
	bool							bSpadeBanker;						//����3ׯ
	BYTE							cbSpadeBankerFirstOutCardData;		//����ѡ����3ׯ���׳���Ȼ��������3,���������˵������
																		//cbSpadeBankerFirstOutCardData����Ϊ0x33 ~0x31
																		//����ѡӮ���׳�,cbSpadeBankerFirstOutCardDataΪINVALID_BYTE
		
	BYTE							cbWinBankerFirstOutCardData;		//INVALID_BYTE��ʾ��ѡӮ���׳��ҳ��������ƣ�  ���߹�ѡ����3ׯ
																		//��ΪINVALID_BYTE���ǵ�Ӯ���׳���һ�ѳ������ƣ�����Ϊ0x33 ~0x31

	bool							bFirstOutSpade;						//�׳��ش�����3

	bool							bForceOutCard;						//�д�س�
	bool							bEchelonConclude;					//�ݶȽ���
	REMAINCARDCOUNT					remainCardCount;					//ʣ���˿�����
};

//¼������
struct Video_GameStart
{
	LONGLONG						lBaseScore;							//��������
	BYTE							cbPlayMode;							//Լս��Ϸģʽ
	WORD							wPlayerCount;						//��ʵ��������
	WORD							wGamePlayerCountRule;				//2-5��Ϊ0����������������ֵΪ����
	
	WORD							wBankerUser;						//ׯ���û�
	WORD							wBeforWiner;						//�Ͼ�Ӯ��
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbCardData[NORMAL_CARD_COUNT];		//�˿��б�
	bool							bSixTheenCard;						//��Ϸģʽ
	bool							bNoForceClose;						//����ǿ��
	bool							bNoRemoveBomb;						//ը�����ɲ�
	bool							bSpadeBanker;						//����3ׯ
	BYTE							cbSpadeBankerFirstOutCardData;
	BYTE							cbWinBankerFirstOutCardData;
	bool							bFirstOutSpade;						//�׳��ش�����3

	bool							bForceOutCard;						//�д�س�
	bool							bEchelonConclude;					//�ݶȽ���

	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�		
	WORD							wChairID;							//����ID
	LONGLONG						lScore;								//����
};

//�û�����
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//�˿���Ŀ
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wOutCardUser;						//�������
	REMAINCARDCOUNT					remainCardCount;					//ʣ���˿�����
	BYTE							bCardData[NORMAL_CARD_COUNT];		//�˿��б�
};

//��������
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//һ�ֿ�ʼ
	WORD				 			wPassUser;							//�������
	WORD				 			wCurrentUser;						//��ǰ���
};


//����й��¼�
struct CMD_S_UserAutomatism
{
	WORD							wChairID;
	bool							bTrusee;
};

//������¼
struct CMD_S_RoomCardRecord
{
	WORD							nCount;											//����
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//���ֽ����
	BYTE                            cbPlayStatus[GAME_PLAYER];						//�û�״̬
	DWORD							dwBombCount[GAME_PLAYER];						//�û��ۼ�ը����
};


//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_OUT_CART				2									//�û�����
#define SUB_C_PASS_CARD				3									//��������
#define SUB_C_AUTOMATISM			4									//�й���Ϣ
#define SUB_C_REQUEST_RCRecord		5									//��ѯ������¼

//�û��й�
struct CMD_C_Automatism
{
	bool							bAutomatism;
};

//�������ݰ�
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//������Ŀ
	BYTE							bCardData[MAX_RECORD_COUNT];		//�˿��б�
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif