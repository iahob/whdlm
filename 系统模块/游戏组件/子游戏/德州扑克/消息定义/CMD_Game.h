#ifndef CMD_DZSHOWHAND_HEAD_FILE
#define CMD_DZSHOWHAND_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

//����������
#define CARD_DISPATCHER_DEBUG

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						3									//��Ϸ I D
#define GAME_PLAYER					9									//��Ϸ����
#define GAME_NAME					TEXT("�����˿�")						//��Ϸ����

//�汾��Ϣ
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//��Ϸ״̬
#define GS_FREE						GAME_STATUS_FREE					//����״̬
#define GS_PLAY						GAME_STATUS_PLAY					//��Ϸ״̬

//����ԭ��
#define GER_NO_PLAYER				0x10								//û�����

//��Ŀ����
#define FULL_COUNT					52									//ȫ����Ŀ
#define MAX_COUNT					2									//�����Ŀ
#define MAX_CENTER_COUNT			5									//�����Ŀ
#define SCORE_ZERO					0
#define JETTON_COUNT				10

//��ֵ����
#define	CARD_MASK_COLOR				0xF0								//��ɫ����
#define	CARD_MASK_VALUE				0x0F								//��ֵ����


//������ʶ
#define AC_NO_ADD					1									//���Ӷ���
#define AC_FOLLOW					2									//��ע����
#define AC_GIVE_UP					3									//��������
#define AC_ADD_SCORE				4									//��ע����
#define AC_SHOW_HAND				5									//�������


//X ���з�ʽ
enum enXCollocateMode 
{
	enXLeft,						//�����
	enXCenter,						//�ж���
	enXRight,						//�Ҷ���
};

//Y ���з�ʽ
enum enYCollocateMode 
{ 
	enYTop,							//�϶���
	enYCenter,						//�ж���
	enYBottom,						//�¶���
};

//�˿˽ṹ
struct tagCardItem
{
	bool							bEffect;							//��Ч��־
	bool							bMy;								//�Լ���־
	bool							bShoot;								//�����־
	BYTE							cbCardData;							//�˿�����
};

//���Զ���
#define MAX_CARD_COUNT				5									//�˿���Ŀ
#define SPACE_CARD_DATA				255									//����˿�


//��ඨ��
#define DEF_X_DISTANCE				19									//Ĭ�ϼ��
#define DEF_Y_DISTANCE				17									//Ĭ�ϼ��
#define DEF_SHOOT_DISTANCE			20									//Ĭ�ϼ��


//////////////////////////////////////////////////////////////////////////
//����������ṹ
#define SUB_S_GAME_START			100								//��Ϸ��ʼ
#define SUB_S_ADD_SCORE				101								//��ע
#define SUB_S_GIVE_UP				102								//����	
#define SUB_S_SEND_CARD				103								//������Ϣ
#define SUB_S_GAME_CONCLUDE			104								//��Ϸ����
#define SUB_S_SIT_DOWN				105								//�û�����
#define SUB_S_GAME_FREE				106								//��Ϸ����
#define SUB_S_SET_CHIP				107								//���ó���
#define SUB_S_SHOW_CARD				108								//�û�����
#define SUB_S_STAND_UP				109								//�û�վ��

#define SUB_S_CHEAT_CARD			110								//��������
#define SUB_S_ANDROID_CARD			111								//������Ϣ
#define SUB_S_ANDROID_INFO			112								//��������

#define SUB_S_EX_USER_RECORD		135								//�û���¼
#define SUB_S_RECORD				136								//˽�˷���¼
#define SUB_S_POOL_INFO				137								//������Ϣ
#define SUB_S_REST_SCORE			138								//�ɲ���ķ�
#define SUB_S_LAST_RECORD			139								//�Ͼ���Ϣ
#define SUB_S_GAME_OVER				140								//��Ϸ����
//////////////////////////////////////////////////////////////////////////
//��������
struct CMD_S_DebugCard
{
	bool							bFresh;
	BYTE							cbCenterCardData[MAX_CENTER_COUNT];		//�����˿�
	BYTE							cbHandCardData[GAME_PLAYER][MAX_COUNT]; //�����˿�
};

//������Ϣ
struct CMD_S_PoolInfo
{
	BYTE							cbPoolCount;
	SCORE							lPoolScore[GAME_PLAYER];
};
//��һ�ֻع�
struct CMD_S_LastRecord
{
	TCHAR							szUserName[GAME_PLAYER][32];
	SCORE							lScore[GAME_PLAYER];
	BYTE							cbPlayStatus[GAME_PLAYER];				//0��ʾ����
	BYTE							cbHandCard[GAME_PLAYER][MAX_COUNT];
	BYTE							cbCenterCard[MAX_CENTER_COUNT];
	BYTE							cbCardType[GAME_PLAYER];				//����
	BYTE							cbMaxCard[GAME_PLAYER][MAX_CENTER_COUNT];//����5����
	WORD							wMinChipInUser;							//Сä
	WORD							wMaxChipInUser;							//��ä
};

//��Ϸ״̬
struct CMD_S_StatusFree
{
	//ʱ����Ϣ
	BYTE							cbStartTime;						//��ʱʱ��
	BYTE							cbBetTime;							//��עʱ��
	BYTE							cbEndTime;							//����ʱ��

	SCORE							lCellScore;							//äע����
	SCORE							lAnteScore;							//�̶���ע
	SCORE							lMinBetScore;						//���Я�����
	SCORE							lMaxBetScore;						//���Я�����
	SCORE							lAllBetScore;						//�ۼƴ�������
	SCORE							lUserMaxScore[GAME_PLAYER];			//��ҷ���
	BYTE							cbTime;								//����ʱ������
	int								nLeftSecond;						//˽�˷�ʣ��ʱ��
	BYTE							cbMaxPlayerCount;					//�������
	BYTE							cbPlayMode;							//Լս��Ϸģʽ0 ���ֳ�	1 ��ҳ�  
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//ʱ����Ϣ
	BYTE							cbStartTime;						//��ʱʱ��
	BYTE							cbBetTime;							//��עʱ��
	BYTE							cbEndTime;							//����ʱ��

	//��ע��Ϣ
	SCORE							lCellScore;							//äע����
	SCORE							lAnteScore;							//�̶���ע
	SCORE							lMinBetScore;						//���Я�����
	SCORE							lMaxBetScore;						//���Я�����
	SCORE							lAllBetScore;						//�ۼƴ�������

	SCORE							lTurnMaxScore;						//�����ע
	SCORE							lTurnLessScore;						//��С��ע
	SCORE							lCellMaxScore;						//�����ע
	SCORE							lAddLessScore;						//����Сע
	SCORE							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	SCORE							lTotalScore[GAME_PLAYER];			//�ۼ���ע
	SCORE							lUserMaxScore[GAME_PLAYER];			//��ҷ���
	SCORE							lCenterScore;						//���ĳ���
	BYTE							cbTime;								//����ʱ��0.5Сʱ�ı���
	int								nLeftSecond;						//˽�˷�ʣ��ʱ��
	//״̬��Ϣ
	bool							bAutoChip;							//�Զ��һ�
	WORD							wDUser;								//D���
	WORD							wMaxChipInUser;						//��äע���	
	WORD							wMinChipInUser;						//Сäע���	
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE							cbPlayStatusRecord[GAME_PLAYER];	//��Ϸ״̬
	BYTE							cbPlayDynamicJoin[GAME_PLAYER];		//��Ϸ״̬
	bool							bStandUp[GAME_PLAYER];				//���վ��
	BYTE							cbBalanceCount;						//ƽ�����

	//�˿���Ϣ
	BYTE							cbCenterCardData[MAX_CENTER_COUNT];	//�˿���Ŀ
	BYTE							cbHandCardData[MAX_COUNT];			//�����˿�
	BYTE							cbMaxPlayerCount;					//�������
	BYTE							cbCardType;							//����
	BYTE							cbPoolCount;						//���ظ���
	SCORE							lPoolScore[GAME_PLAYER];			//���ط���
	BYTE							cbPlayMode;							//Լս��Ϸģʽ0 ���ֳ�	1 ��ҳ� 
	BYTE							cbTimeLeave;						//ʣ�����ʱ��
	bool							bGiveUp[GAME_PLAYER];				//����״̬
};

//���ó���
struct CMD_S_SetChip
{
	WORD							wChipUser;							//��ǰ�û�
	SCORE							lChipScore;							//������Ŀ
};

//Լռ��¼
struct CMD_S_Record
{
	int								nCount;								//�ܾ���
	WORD							wMaxLoser;							//������
	WORD							wMaxTaker;							//���Я����
	WORD							wMvp;								//���Ӯ��
	SCORE							lAllScore[GAME_PLAYER];				//����Ӯ��
	SCORE							lTakeScore[GAME_PLAYER];			//�ܴ����
};

//AI�˿�
struct CMD_S_AndroidCard
{
	BYTE							cbAllHandCardData[GAME_PLAYER][MAX_COUNT];	//�����˿�
	BYTE							cbCenterCardData[MAX_CENTER_COUNT];			//�����˿�
	bool							bAndroidUser[GAME_PLAYER];					//AI
	SCORE							lTotalScore[GAME_PLAYER];					//������ע
	bool							bMustWin;									//����Ӯ
};

//�����˿�
struct CMD_S_GameStart
{
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wDUser;								//D���
	WORD							wMaxChipInUser;						//��äע���	
	BYTE							cbDynamicJoinCount;					//��̬�����������
	WORD							wDynamicJoinUser[GAME_PLAYER];		//��̬���뷣ä���
	WORD							wMinChipInUser;						//Сäע���	
	SCORE							lTurnMaxScore;						//�����ע
	SCORE							lTurnLessScore;						//��С��ע
	SCORE							lAddLessScore;						//����Сע
	SCORE							lUserMaxScore[GAME_PLAYER];			//��ҷ���
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//�û��˿�
	BYTE							cbPlayStatus[GAME_PLAYER];			//�û�״̬
	bool							bStandUp[GAME_PLAYER];				//���վ��
	SCORE							lUserTableScore[GAME_PLAYER];		//��ǰ����ע��
};


//�û���ע
struct CMD_S_AddScore
{
	WORD							wCurrentUser;					//��ǰ�û�
	WORD							wAddScoreUser;					//��ע�û�
	SCORE							lAddScoreCount;					//��ע��Ŀ
	SCORE							lTurnLessScore;					//���ټ�ע
	SCORE							lTurnMaxScore;					//�����ע
	SCORE							lAddLessScore;					//����Сע
};

//�û�����
struct CMD_S_GiveUp
{
	WORD							wGiveUpUser;					//�����û�
	SCORE							lLostScore;						//������	
	SCORE							lTableScore;
};

//�������ݰ�
struct CMD_S_SendCard
{
	BYTE							cbPublic;						//�Ƿ���
	WORD							wCurrentUser;					//��ǰ�û�
	SCORE							lTurnLessScore;					//���ټ�ע
	SCORE							lTurnMaxScore;					//�����ע
	SCORE							lAddLessScore;					//����Сע
	BYTE							cbSendCardCount;				//������Ŀ
	BYTE							cbCenterCardData[MAX_CENTER_COUNT];	//�����˿�	
	BYTE							cbCardType;						//���ƺ������
};

//��Ϸ����
struct CMD_S_GameConclude
{
	BYTE							cbTotalEnd;											//ǿ�˱�־
	BYTE							cbDelayTime;										//��ʱʱ��
	SCORE							lGameTax[GAME_PLAYER];								//��Ϸ˰��
	SCORE							lGameScore[GAME_PLAYER];							//��Ϸ�÷�
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];					//�û��˿�
	BYTE							cbLastCenterCardData[GAME_PLAYER][MAX_CENTER_COUNT];//����˿�
	BYTE							cbLastCardType[GAME_PLAYER];						//�������
	BYTE							cbPoolCount;										//���ظ���
	SCORE							lPoolScore[GAME_PLAYER];							//���ط���
	BYTE							cbPoolWinnerCount[GAME_PLAYER];						//����Ӯ�Ҹ���
	WORD							wPoolWinner[GAME_PLAYER][GAME_PLAYER];				//����Ӯ����λ��
	SCORE							lUserMaxScore[GAME_PLAYER];							//���ֽ�������Я����
};

//��Ϸ�ձ�
struct CMD_S_GameFree
{
	bool							bAutoStart;							//�Զ���ʼ
	SCORE							lUserMaxScore[GAME_PLAYER];			//��ҷ���
};

//��Ϸ״̬
struct CMD_S_StatusEnd
{
	//ʱ����Ϣ
	BYTE							cbStartTime;						//��ʱʱ��
	BYTE							cbBetTime;							//��עʱ��
	BYTE							cbEndTime;							//����ʱ��

	//��ע��Ϣ
	SCORE							lCellScore;							//äע����
	SCORE							lMinBetScore;						//���Я�����
	SCORE							lMaxBetScore;						//���Я�����

	BYTE							cbTime;								//����ʱ��0.5Сʱ�ı���
	int								nLeftSecond;						//˽�˷�ʣ��ʱ��

	SCORE							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	SCORE							lTotalScore[GAME_PLAYER];			//�ۼ���ע
	SCORE							lUserMaxScore[GAME_PLAYER];			//��ҷ���
	SCORE							lCenterScore;						//���ĳ���

	//״̬��Ϣ
	bool							bAutoChip;							//�Զ��һ�
	WORD							wDUser;								//D���
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE							cbPlayStatusRecord[GAME_PLAYER];	//��Ϸ״̬
	BYTE							cbPlayDynamicJoin[GAME_PLAYER];		//��Ϸ״̬
	bool							bStandUp[GAME_PLAYER];				//���վ��
	BYTE							cbBalanceCount;						//ƽ�����

	//�˿���Ϣ
	BYTE							cbCenterCardData[MAX_CENTER_COUNT];	//�˿���Ŀ

	CMD_S_GameConclude				GameConclude;						//������Ϣ
};

//�û�����
struct CMD_S_ShowCard
{
	WORD							wShowUser;							//�����û�
	BYTE							cbCardData[MAX_COUNT];				//�û��˿�
};

//�û�վ��
struct CMD_S_StandUp
{
	WORD							wStandUpUser;						//վ���û�
};
//����ɲ���ķ�
struct CMD_S_REST_SCORE
{
	SCORE							lRestScore;
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_ADD_SCORE					1									//�û���ע
#define SUB_C_GIVE_UP					2									//������Ϣ
#define SUB_C_GAME_END					3									//��Ϸ����
#define SUB_C_SET_CHIP					4									//���ó���
#define SUB_C_SHOW_CARD					5									//�û�����.
#define SUB_C_STAND_UP					6									//�û�վ��
#define SUB_C_GAME_RECORD				7									//������Ϸ��¼
#define SUB_C_GET_REST_SCORE			8									//����ɲ����
#define SUB_C_LAST_RECORD				9									//�Ͼ���Ϣ

//�û���ע
struct CMD_C_AddScore
{
	SCORE							lScore;								//��ע��Ŀ
};

//���ó���
struct CMD_C_SetChip
{
	bool							bAutoAddChip;						//�Զ��һ�
	SCORE							lChipScore;							//������Ŀ
};

//////////////////////////////////////////////////////////////////////////
//�ؼ���Ϣ

//��Ϣ����
#define IDM_START					WM_USER+100							//��ʼ��Ϣ
#define IDM_EXIT					WM_USER+101							//�뿪��Ϣ
#define IDM_GIVEUP					WM_USER+102							//������Ϣ
#define IDM_ADD_SCORE				WM_USER+103							//��ע��Ϣ	
#define IDM_PASS_CARD				WM_USER+104							//������Ϣ	
#define IDM_FOLLOW					WM_USER+105							//��ע��Ϣ
#define IDM_AUTO_PASS_GIVEUP		WM_USER+106							//�Զ�����
#define IDM_AUTO_ADD_SCORE			WM_USER+107							//������ע	
#define IDM_AUTO_PASS_CARD			WM_USER+108							//�Զ�����	
#define IDM_AUTO_FOLLOW				WM_USER+109							//�Զ���ע
#define IDM_GAME_OVER				WM_USER+111							//������Ϣ
#define IDM_SEND_FINISH				WM_USER+112							//���ƽ���
#define IDM_BACK_PLAZA				WM_USER+122							//������ť
#define IDM_CHANGE_TABLES			WM_USER+123							//������ť
#define IDM_SHOW_CHIP				WM_USER+124 						//���ó���
#define IDM_SET_CHIP				WM_USER+125 						//���ó���
#define IDM_CLOSE_CHIP				WM_USER+126 						//���ó���
#define IDM_SWITCH_BUTTON			WM_USER+127 						//�л���ť
#define IDM_SHOW_CARD				WM_USER+128 						//���ư�ť
#define IDM_STAND_UP				WM_USER+129							//վ��ť


//////////////////////////////////////////////////////////////////////////
struct tagCustomAndroid
{
	SCORE								lRobotScoreMin;
	SCORE								lRobotScoreMax;
	SCORE	                            lRobotBankGet;
	SCORE								lRobotBankGetBanker;
	SCORE								lRobotBankStoMul;
};

//��������
typedef enum{ CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL }DEBUG_TYPE;

//���Խ��      ���Գɹ� ������ʧ�� ������ȡ���ɹ� ������ȡ����Ч
typedef enum{ DEBUG_SUCCEED, DEBUG_FAIL, DEBUG_CANCEL_SUCCEED, DEBUG_CANCEL_INVALID }DEBUG_RESULT;

//�û���Ϊ
typedef enum{ USER_SITDOWN = 11, USER_STANDUP, USER_OFFLINE, USER_RECONNECT }USERACTION;

#define MAX_OPERATION_RECORD			20									//������¼����
#define RECORD_LENGTH					128									//ÿ����¼�ֳ�

#define SUB_S_ADMIN_STORAGE_INFO		112									//ˢ�µ��Է����
#define SUB_S_ROOMCARD_RECORD			114									//������¼

#define SUB_S_REQUEST_QUERY_RESULT		115									//��ѯ�û����
#define SUB_S_USER_DEBUG				116									//�û�����
#define SUB_S_USER_DEBUG_COMPLETE		117									//�û��������
#define SUB_S_OPERATION_RECORD		    118									//������¼
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT 119


typedef struct
{
	DEBUG_TYPE							debug_type;					//��������
	BYTE								cbDebugCount;					//���Ծ���
	bool							    bCancelDebug;					//ȡ����ʶ
}USERDEBUG;

//�����û���Ϣ
typedef struct
{
	WORD								wChairID;							//����ID
	WORD								wTableID;							//����ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//AI��ʶ
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE								cbUserStatus;						//�û�״̬
	BYTE								cbGameStatus;						//��Ϸ״̬
}ROOMUSERINFO;

//�����û�����
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//�����û���Ϣ
	USERDEBUG							userDebug;						//�û�����
}ROOMUSERDEBUG;

struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO						userinfo;							//�û���Ϣ
	bool								bFind;								//�ҵ���ʶ
};

//�û�����
struct CMD_S_UserDebug
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	DEBUG_RESULT						debugResult;						//���Խ��
	DEBUG_TYPE						debugType;						//��������
	BYTE								cbDebugCount;						//���Ծ���
};

//�û�����
struct CMD_S_UserDebugComplete
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	DEBUG_TYPE						debugType;						//��������
	BYTE								cbRemainDebugCount;				//ʣ����Ծ���
};
//���Է���˿����Ϣ
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;
	LONGLONG							lRoomStorageDeduct;
	LONGLONG							lMaxRoomStorage[2];
	WORD								wRoomStorageMul[2];
};


//������¼
struct CMD_S_Operation_Record
{
	TCHAR								szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];	//��¼���²�����20����¼
};

//������½��
struct CMD_S_RequestUpdateRoomInfo_Result
{
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���
	ROOMUSERINFO						currentqueryuserinfo;				//��ǰ��ѯ�û���Ϣ
	bool								bExistDebug;						//��ѯ�û����ڵ��Ա�ʶ
	USERDEBUG							currentuserdebug;
};
#define SUB_C_STORAGE					6									//���¿��
#define	SUB_C_STORAGEMAXMUL				7									//��������
#define SUB_C_REQUEST_RCRecord			12									//��ѯ������¼

#define SUB_C_REQUEST_QUERY_USER		13									//�����ѯ�û�
#define SUB_C_USER_DEBUG				14									//�û�����

//�����������
#define SUB_C_REQUEST_UDPATE_ROOMINFO	15									//������·�����Ϣ
#define SUB_C_CLEAR_CURRENT_QUERYUSER	16


struct CMD_C_UpdateStorage
{
	LONGLONG							lRoomStorageCurrent;				//�����ֵ
	LONGLONG							lRoomStorageDeduct;					//�����ֵ
};

struct CMD_C_ModifyStorage
{
	LONGLONG							lMaxRoomStorage[2];					//�������
	WORD								wRoomStorageMul[2];					//Ӯ�ָ���
};


struct CMD_C_RequestQuery_User
{
	DWORD								dwGameID;							//��ѯ�û�GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//��ѯ�û��ǳ�
};

//�û�����
struct CMD_C_UserDebug
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	USERDEBUG							userDebugInfo;					//
};
#pragma pack()

#endif
