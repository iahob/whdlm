#ifndef CMD_OX_HEAD_FILE
#define CMD_OX_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID							33									//��Ϸ I D
#define GAME_PLAYER						6									//��Ϸ����
#define GAME_NAME						TEXT("��������")						//��Ϸ����
#define MAX_COUNT						3									//�����Ŀ
#define MAX_JETTON_AREA					4									//��ע����
#define MAX_TIMES						5									//�������
#define MAX_CARD						52									//����˿���
#define MAX_RECORD_COUNT				32									//��������ͳ��������

#define VERSION_SERVER					PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT					PROCESS_VERSION(7,0,1)				//����汾

//����ԭ��
#define GER_NO_PLAYER					0x10								//û�����

//��Ϸ״̬
#define GS_TK_FREE						GAME_STATUS_FREE					//�ȴ���ʼ
#define GS_TK_CALL						GAME_STATUS_PLAY					//��ׯ״̬
#define GS_TK_SCORE						GAME_STATUS_PLAY+1					//��ע״̬
#define GS_TK_PLAYING					GAME_STATUS_PLAY+2					//��Ϸ����

#define	IDC_BT_AGREED					200								//ͬ�ⰴť
#define IDC_BT_DISAGREED				201								//ȡ����ť
#define IDC_BT_OK						202								//ȷ����ť
#define IDC_BT_OKADD					203								//ȷ������
#define IDC_BT_CLOSE					204								//�رհ�ť
#define IDC_CALL_SCORE_ONE				205								//1 �Ű�ť
#define IDC_CALL_SCORE_TWO				206								//2 �Ű�ť
#define IDC_CALL_SCORE_THREE			207								//3 �Ű�ť


#define SERVER_LEN						32

//////////////////////////////////////////////////////////////////////////
//����������ṹ
#define SUB_S_READY_BEGIN				99									//׼����ʼ	
#define SUB_S_CALL_BEGIN				100									//��ׯ��ʼ
#define SUB_S_CALL_BANKER				101									//�û���ׯ
#define SUB_S_ADD_BEGIN					102									//��ע��ʼ
#define SUB_S_ADD_SCORE					103									//��ע���
#define SUB_S_SEND_CARD					104									//������Ϣ
#define SUB_S_OPEN_CARD					105									//�û�̯��
#define SUB_S_GAME_END					106									//��Ϸ����
//#define SUB_S_ALL_CARD					107									//������Ϣ
#define SUB_S_TRUSTEE                   107									//�й�
#define SUB_S_AMDIN_COMMAND				108									//ϵͳ����
#define SUB_S_BANKER_OPERATE			109									//��ȡ��
#define SUB_S_SET_ROOM					110									//�������õ�ע
#define SUB_S_USE_EXIT					111									//��ׯ����뿪
#define SUB_S_PLAYER_EXIT				112									//�û�ǿ��
#define SUB_S_RECORD					113									//��Ϸ��¼
#define SUB_S_VEDIO_BEGIN				114									//¼��ʼ
#define SUB_S_ANDROID_BANKOPERATOR		115									//���������в���

#define SUB_S_GAME_START                117                                //��Ϸ��ʼ
#define SUB_S_CHIPMODE                  118                                //�й�ģʽ

#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#define myatoi      _wtoi
#define myatoi64	_wtoi64
#endif


#pragma pack(push)
#pragma pack(1)

struct CMD_S_RECORD
{
    int			nCount[GAME_PLAYER];
    LONGLONG	lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//���ֽ����
	LONGLONG	lAllScore[GAME_PLAYER];	//�ܽ����
};

//��������
typedef enum{ CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL }CONTROL_TYPE;

//���ƽ��      ���Ƴɹ� ������ʧ�� ������ȡ���ɹ� ������ȡ����Ч
typedef enum{ CONTROL_SUCCEED, CONTROL_FAIL, CONTROL_CANCEL_SUCCEED, CONTROL_CANCEL_INVALID }CONTROL_RESULT;

//�û���Ϊ
typedef enum{ USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT }USERACTION;

//������Ϣ
typedef struct
{
	CONTROL_TYPE						control_type;					  //��������
	BYTE								cbControlCount;					  //���ƾ���
	bool							    bCancelControl;					  //ȡ����ʶ
}USERCONTROL;

//�����û���Ϣ
typedef struct
{
	WORD								wChairID;							//����ID
	WORD								wTableID;							//����ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//�����˱�ʶ
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE								cbUserStatus;						//�û�״̬
	BYTE								cbGameStatus;						//��Ϸ״̬
}ROOMUSERINFO;

//�����û�����
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//�����û���Ϣ
	USERCONTROL							userControl;						//�û�����
}ROOMUSERCONTROL;

//��Ϸ״̬
struct CMD_S_StatusFree
{
    //����
    BYTE								cbTimeStartGame;					//��ʼʱ��
    BYTE								cbTimeCallBanker;					//��ׯʱ��
    BYTE								cbTimeAddScore;						//��עʱ��
    BYTE								cbTimeOpenCard;						//����ʱ��

	BYTE								cbPlayMode;                         //Լս����Ϸģʽ
	BYTE								cbTrusteeMode;                      //�й������С���
  
	LONGLONG							cbBaseScore;						//�����׷�
    bool								bCallBanker;						//�Ƿ���ׯ

    //��ʷ����
    LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
    LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
    TCHAR								szGameRoomName[SERVER_LEN];			//��������
};

//��ׯ״̬
struct CMD_S_StatusCall
{
    //����
    BYTE								cbTimeStartGame;					//��ʼʱ��
    BYTE								cbTimeCallBanker;					//��ׯʱ��
    BYTE								cbTimeAddScore;						//��עʱ��
    BYTE								cbTimeOpenCard;						//����ʱ��

	BYTE								cbPlayMode;                         //Լս����Ϸģʽ
	BYTE								cbTrusteeMode;                      //�й������С���
	LONGLONG							cbBaseScore;						//�׷�
    bool								bCallBanker;						//�Ƿ���ׯ

    INT									nCallStatus[GAME_PLAYER];			//��ׯ״̬
    BYTE                                cbDynamicJoin;                      //��̬����
    BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬

    //��ʷ����
    LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
    LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
    TCHAR								szGameRoomName[SERVER_LEN];			//��������
};

//��ע״̬
struct CMD_S_StatusScore
{
    //����
    BYTE								cbTimeStartGame;					//��ʼʱ��
    BYTE								cbTimeCallBanker;					//��ׯʱ��
    BYTE								cbTimeAddScore;						//��עʱ��
    BYTE								cbTimeOpenCard;						//����ʱ��

	BYTE								cbPlayMode;                         //Լս����Ϸģʽ
	BYTE								cbTrusteeMode;                      //�й������С���

	LONGLONG							cbBaseScore;						//�׷�
    bool								bCallBanker;						//�Ƿ���ׯ

    //��ע��Ϣ
    BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬
    BYTE                                cbDynamicJoin;                      //��̬����
    LONGLONG							lTableScore[GAME_PLAYER];			//��ע��Ŀ
    WORD								wBankerUser;						//ׯ���û�
    TCHAR								szGameRoomName[SERVER_LEN];			//��������

    //��ʷ����
    LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
    LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
    //����
    BYTE								cbTimeStartGame;					//��ʼʱ��
    BYTE								cbTimeCallBanker;					//��ׯʱ��
    BYTE								cbTimeAddScore;						//��עʱ��
    BYTE								cbTimeOpenCard;						//����ʱ��

	BYTE								cbPlayMode;                         //Լս����Ϸģʽ
	BYTE								cbTrusteeMode;                      //�й������С���
  
	LONGLONG							cbBaseScore;						//�׷�
    bool								bCallBanker;						//�Ƿ���ׯ

    //״̬��Ϣ
    BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬
    BYTE                                cbDynamicJoin;                      //��̬����
    LONGLONG							lTableScore[GAME_PLAYER];			//��ע��Ŀ
    WORD								wBankerUser;						//ׯ���û�
    BYTE								cbType[GAME_PLAYER];				//̯������
    bool								bOperation[GAME_PLAYER];			//�û�����

    //�˿���Ϣ
    BYTE								cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�

    //��ʷ����
    LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
    LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
    TCHAR								szGameRoomName[SERVER_LEN];			//��������
};

//������׼����ʼ
struct CMD_S_ReadyBein
{
	bool								bReadyBein;							//׼����ʼ
};

//¼������
struct Video_Bein
{
    WORD								wPlayerCount;						//��ʵ��������
    WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
    WORD								wChairID;							//����ID
    WORD								wRoomID;							//����ID
    bool								bCallBanker;						//�Ƿ���ׯ
    LONGLONG							lScore;								//����
    LONGLONG							lCellScore;							//�׷�
};

//��ׯ��ʼ
struct CMD_S_CallBegin
{
    bool								bCallBanker;						//��ʼ��ׯ

    //ʱ�䶨��
    BYTE								cbTimeStartGame;					//��ʼʱ��
    BYTE								cbTimeCallBanker;					//��ׯʱ��
    BYTE								cbTimeAddScore;						//��עʱ��
    BYTE								cbTimeOpenCard;						//����ʱ��
};

//�û���ׯ
struct CMD_S_CallBanker
{
    WORD								wCallBankerUser;					//��ׯ�û�
    bool								bCallBanker;						//�Ƿ��ׯ
};

//��ע��ʼ
struct CMD_S_AddBegin
{
    //��ע��Ϣ
    WORD								wBankerUser;						//ׯ���û�
    LONGLONG							lCellScore;							//��������

    //ʱ�䶨��
    BYTE								cbTimeStartGame;					//��ʼʱ��
    BYTE								cbTimeCallBanker;					//��ׯʱ��
    BYTE								cbTimeAddScore;						//��עʱ��
    BYTE								cbTimeOpenCard;						//����ʱ��

    bool								bCallBanker;						//�Ƿ���ׯ
};

//�û���ע
struct CMD_S_AddScore
{
    WORD								wAddScoreUser;						//��ע�û�
    LONGLONG							lAddScoreCount;						//��ע��Ŀ
};

//�������ݰ�
struct CMD_S_SendCard
{
    BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//�û��˿�
    BYTE								cbUserCount;						//�û�����
};

//�û�̯��
struct CMD_S_Open_Card
{
    WORD								wPlayerID;							//̯���û�
    BYTE								cbType[GAME_PLAYER];				//̯������
    bool								bOperation;							//�û�����
};

//��Ϸ����
struct CMD_S_GameEnd
{
    LONGLONG							lGameTax[GAME_PLAYER];				//��Ϸ˰��
    LONGLONG							lGameScore[GAME_PLAYER];			//��Ϸ�÷�
    BYTE								cbType[GAME_PLAYER];				//̯������
	BYTE								cbDelayOverGame;
};

//�û��˳�
struct CMD_S_PlayerExit
{
    WORD								wPlayerID;							//�˳��û�
};

//��ׯ����뿪
struct CMD_S_UserExit
{
    WORD							wChairID;								//��ׯ�û�
};

//���õ�ע
struct CMD_S_SendFinish
{
    bool							bFinish;								//���ƽ���
};

//�й�
struct CMD_S_Trustee
{
	WORD							wTrusteeUser;							//��ׯ�û�
	bool							bTrustee;								//
};

//�й�ģʽ
struct CMD_S_ChipMode
{
	BYTE							cbTrusteeMode;								//���ƽ���
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_CALL_BANKER				1									//�û���ׯ
#define SUB_C_ADD_SCORE					2									//�û���ע
#define SUB_C_OPEN_CARD					3									//�û�̯��
#define SUB_C_GAME_BEGIN				4                                   //��Ϸ��ʼ
#define SUB_C_AMDIN_COMMAND				5									//ϵͳ����
#define SUB_C_TRUSTEE				    6                                   //�й�
#define SUB_C_CHIPMODE		    		7								    //�����С�й���ע

//�û���ׯ
struct CMD_C_CallBanker
{
    bool								bBanker;							//��ׯ��־
};

//�û���ע
struct CMD_C_AddScore
{
    LONGLONG							lScore;								//��ע��Ŀ
};

//�û��й�
struct CMD_C_Trustee
{
	bool							bTrustee;								//�Ƿ��й�
};

//�û��й�
struct CMD_C_ChipMode
{
	BYTE							cbTrusteeMode;								//�й�ģʽ 0��ע��С��ȣ�1��ע�����
};

//////////////////////////////////////////////////////////////////////////
struct CMD_C_AdminReq
{
    BYTE cbReqType;
#define RQ_SET_WIN_AREA		1
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
#define CR_ACCEPT  2				//����
#define CR_REFUSAL 3				//�ܾ�
    BYTE cbExtendData[20];			//��������
};


//����������Ϣ
struct tagControlInfo
{
    INT  nAreaWin;		//��������
};

//���������Ʒ���
#define	 S_CR_FAILURE				0		//ʧ��
#define  S_CR_UPDATE_SUCCES			1		//���³ɹ�
#define	 S_CR_SET_SUCCESS			2		//���óɹ�
#define  S_CR_CANCEL_SUCCESS		3		//ȡ���ɹ�
struct CMD_S_ControlReturns
{
    BYTE cbReturnsType;				//�ظ�����
    BYTE cbControlArea;				//��������
    BYTE cbControlTimes;			//���ƴ���
};


//�ͻ��˿�������
#define  C_CA_UPDATE				1		//����
#define	 C_CA_SET					2		//����
#define  C_CA_CANCELS				3		//ȡ��
struct CMD_C_ControlApplication
{
    BYTE cbControlAppType;			//��������
    BYTE cbControlArea;				//��������
    BYTE cbControlTimes;			//���ƴ���
};


#pragma pack(pop)

#endif
