#ifndef CMD_HK_FIVE_CARD_HEAD_FILE
#define CMD_HK_FIVE_CARD_HEAD_FILE


#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID							56									//��Ϸ I D
#define GAME_PLAYER						5									//��Ϸ����
#define GAME_NAME						TEXT("�������")						//��Ϸ����
#define MAX_COUNT						3									//�˿���Ŀ
#define VERSION_SERVER			    	PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				    PROCESS_VERSION(7,0,1)				//����汾

//����ԭ��
#define GER_NO_PLAYER					0x10								//û�����
#define GER_COMPARECARD					0x20								//���ƽ���

#define LEN_NICKNAME					32									//�ǳƳ���
#define TIME_DISPATCH_CARD				5									//��������ƶ���ʱ��


//������¼
#define MAX_OPERATION_RECORD					20							//������¼����
#define RECORD_LENGTH							128							//ÿ����¼�ֳ�

#define MAX_RECORD_COUNT						32							//��������ͳ��������

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START				        100							//��Ϸ��ʼ
#define SUB_S_ADD_SCORE					        101							//��ע���
#define SUB_S_GIVE_UP					        102							//������ע
#define SUB_S_GAME_END					        104							//��Ϸ����
#define SUB_S_COMPARE_CARD				        105							//���Ƹ�ע
#define SUB_S_LOOK_CARD					        106							//���Ƹ�ע
#define SUB_S_WAIT_COMPARE				        109							//�ȴ�����
#define SUB_S_ANDROID_CARD				        110							//������Ϣ
#define SUB_S_CHEAT_CARD				        111							//������Ϣ

#define SUB_S_ADMIN_STORAGE_INFO		        112							//ˢ�µ��Է����
#define SUB_S_REQUEST_QUERY_RESULT		        113							//��ѯ�û����
#define SUB_S_USER_DEBUG				        114							//�û�����
#define SUB_S_USER_DEBUG_COMPLETE		        115							//�û��������
#define SUB_S_OPERATION_RECORD			        116							//������¼
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT	117

#define SUB_S_ROOMCARD_RECORD					123							//������¼
#define SUB_S_TURN_COUNT						124							//��ǰ����

//�����˴��ȡ��
struct tagCustomAndroid
{
	LONGLONG									lRobotScoreMin;	
	LONGLONG									lRobotScoreMax;
	LONGLONG	                                lRobotBankGet; 
	LONGLONG									lRobotBankGetBanker; 
	LONGLONG									lRobotBankStoMul; 
};

//��������
typedef enum{CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL}DEBUG_TYPE;

//���Խ��      ���Գɹ� ������ʧ�� ������ȡ���ɹ� ������ȡ����Ч
typedef enum{DEBUG_SUCCEED, DEBUG_FAIL, DEBUG_CANCEL_SUCCEED, DEBUG_CANCEL_INVALID}DEBUG_RESULT;

//�û���Ϊ
typedef enum{USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT}USERACTION;

//������Ϣ
typedef struct
{
	DEBUG_TYPE							debug_type;							//��������
	BYTE								cbDebugCount;						//���Ծ���
	bool							    bCancelDebug;						//ȡ����ʶ
}USERDEBUG;

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
	USERDEBUG							userDebug;							//�û�����
}ROOMUSERDEBUG;

//��Ϸ״̬
struct CMD_S_StatusFree
{
	LONGLONG							lCellScore;								//��������
	LONGLONG							lRoomStorageStart;						//������ʼ���
	LONGLONG							lRoomStorageCurrent;					//���䵱ǰ���
	tagCustomAndroid					CustomAndroid;							//����������
	TCHAR								szServerName[32];						//��������

	WORD								wGamePlayerCountRule;					//2-5��Ϊ0����������������ֵΪ����
	LONGLONG							lInitScore;								//��ʼ��
	BYTE								cbMinLookCardTurnCount;					//��С��������
	BYTE								cbMinCompareCardTurnCount;				//��С��������
	int									nButtonValue[4];						//��ע��ť����ֵ
	BYTE								cbPlayMode;								//0���ֳ� 1��ҳ�
	BYTE								cbMaxTurnCount;							//��ע�������
	BYTE								cbMinAllinTurnCount;					//��Сȫѹ����
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��ע��Ϣ
	LONGLONG							lMaxCellScore;						//��Ԫ����
	LONGLONG							lCellScore;							//��Ԫ��ע
	LONGLONG							lCurrentTimes;						//��ǰ��ע
	LONGLONG							lUserMaxScore;						//�û���������

	//״̬��Ϣ
	WORD								wBankerUser;						//ׯ���û�
	WORD				 				wCurrentUser;						//��ǰ���
	BYTE								cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	bool								bMingZhu[GAME_PLAYER];				//����״̬
	LONGLONG							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���
	tagCustomAndroid					CustomAndroid;						//����������

	//�˿���Ϣ
	BYTE								cbHandCardData[MAX_COUNT];			//�˿�����

	//״̬��Ϣ
	bool								bCompareState;						//����״̬		
	TCHAR								szServerName[32];					//��������

	WORD								wGamePlayerCountRule;				//2-5��Ϊ0����������������ֵΪ����
	LONGLONG							lInitScore;							//��ʼ��
	BYTE								cbMinLookCardTurnCount;				//��С��������
	BYTE								cbMinCompareCardTurnCount;			//��С��������
	BYTE								cbCurTurnCount;						//��ǰ����
	bool								bShowHand;							//���
	int									nButtonValue[4];					//��ע��ť����ֵ
	BYTE								cbPlayMode;							//0���ֳ� 1��ҳ�
	BYTE								cbMaxTurnCount;						//��ע�������
	BYTE								cbMinAllinTurnCount;				//��Сȫѹ����
	BYTE								cbRemainingTime;					//ʣ��ʱ��
	BYTE								cbCardType;							//�ѿ����������
};

//AI�˿�
struct CMD_S_AndroidCard
{
	BYTE								cbRealPlayer[GAME_PLAYER];					//�������
	BYTE								cbAndroidStatus[GAME_PLAYER];				//������Ŀ
	BYTE								cbAllHandCardData[GAME_PLAYER][MAX_COUNT];	//�����˿�
	LONGLONG							lStockScore;								//��ǰ���
};


//AI�˿�
struct CMD_S_DebugCardInfo
{
	BYTE								cbAllHandCardData[GAME_PLAYER][MAX_COUNT];	//�����˿�
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	//��ע��Ϣ
	LONGLONG							lMaxScore;							//�����ע
	LONGLONG							lCellScore;							//��Ԫ��ע
	LONGLONG							lCurrentTimes;						//��ǰ����
	LONGLONG							lUserMaxScore;						//��������

	//�û���Ϣ
	WORD								wBankerUser;						//ׯ���û�
	WORD				 				wCurrentUser;						//��ǰ���
	BYTE								cbPlayStatus[GAME_PLAYER];			//�û�״̬
};

//�û���ע
struct CMD_S_AddScore
{
	WORD								wCurrentUser;						//��ǰ�û�
	WORD								wAddScoreUser;						//��ע�û�
	BYTE								wCompareState;						//����״̬
	LONGLONG							lAddScoreCount;						//��ע��Ŀ
	LONGLONG							lCurrentTimes;						//��ǰ����
	bool								bShowHand;							//���
};

//�û�����
struct CMD_S_GiveUp
{
	WORD								wGiveUpUser;						//�����û�
};

//�������ݰ�
struct CMD_S_CompareCard
{
	WORD								wCurrentUser;						//��ǰ�û�
	WORD								wCompareUser[2];					//�����û�
	WORD								wLostUser;							//�����û�
	bool								bShowHand;							//���
};
struct CMD_S_TurnCount
{
	BYTE								cbTurnCount;//��ǰ����
};
//�������ݰ�
struct CMD_S_LookCard
{
	WORD								wLookCardUser;						//�����û�
	BYTE								cbCardData[MAX_COUNT];				//�û��˿�
	BYTE								cbCardType;							//����
};

//��Ϸ����
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax;							//��Ϸ˰��
	LONGLONG							lGameScore[GAME_PLAYER];			//��Ϸ�÷�
	BYTE								cbCardData[GAME_PLAYER][3];			//�û��˿�
	WORD								wCompareUser[GAME_PLAYER][4];		//�����û�
	WORD								wEndState;							//����״̬
	bool								bDelayOverGame;						//�ӳٿ�ʼ
	WORD								wServerType;						//��������
	BYTE								cbCardType[GAME_PLAYER];			//�������
};

//�ȴ�����
struct CMD_S_WaitCompare
{
	WORD								wCompareUser;						//�����û�
};

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
	DEBUG_TYPE							debugType;							//��������
	BYTE								cbDebugCount;						//���Ծ���
};

//�û�����
struct CMD_S_UserDebugComplete
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	DEBUG_TYPE							debugType;							//��������
	BYTE								cbRemainDebugCount;					//ʣ����Ծ���
};

//���Է���˿����Ϣ
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG	                        lRoomStorageStart;											
	LONGLONG	                        lRoomStorageCurrent;
	LONGLONG	                        lRoomStorageDeduct;
	LONGLONG	                        lMaxRoomStorage[2];
	WORD		                        wRoomStorageMul[2];
};

//������¼
struct CMD_S_Operation_Record
{
	TCHAR		szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];	                //��¼���²�����20����¼
};

//������½��
struct CMD_S_RequestUpdateRoomInfo_Result
{
	LONGLONG							lRoomStorageCurrent;				    //���䵱ǰ���
	ROOMUSERINFO						currentqueryuserinfo;				    //��ǰ��ѯ�û���Ϣ
	bool								bExistDebug;						    //��ѯ�û����ڵ��Ա�ʶ
	USERDEBUG							currentuserdebug;
};


//¼������
struct Video_GameStart
{	
	WORD								wPlayerCount;							//��ʵ��������
	WORD								wGamePlayerCountRule;					//2-5��Ϊ0����������������ֵΪ����
	BYTE								cbHandCardData[GAME_PLAYER][MAX_COUNT];	//����˿�����
	LONGLONG							lMaxScore;								//�����ע
	LONGLONG							lCellScore;								//��Ԫ��ע
	LONGLONG							lCurrentTimes;							//��ǰ����
	LONGLONG							lUserMaxScore;							//��������
	WORD								wBankerUser;							//ׯ���û�
	WORD				 				wCurrentUser;							//��ǰ���
	BYTE								cbPlayStatus[GAME_PLAYER];				//�û�״̬
	TCHAR								szNickName[LEN_NICKNAME];				//�û��ǳ�		
	WORD								wChairID;								//����ID
	LONGLONG							lScore;									//����
};

//������¼
struct CMD_S_RoomCardRecord
{
	WORD							nCount;										//����
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];//���ֽ����
	BYTE							cbCardTypeCount[GAME_PLAYER][7];			//ÿ����������
	LONGLONG						lAllScore[GAME_PLAYER];
};
//////////////////////////////////////////////////////////////////////////

//�ͻ�������ṹ
#define SUB_C_ADD_SCORE					1									//�û���ע
#define SUB_C_GIVE_UP					2									//������Ϣ
#define SUB_C_COMPARE_CARD				3									//������Ϣ
#define SUB_C_LOOK_CARD					4									//������Ϣ
#define SUB_C_WAIT_COMPARE				6									//�ȴ�����
#define SUB_C_FINISH_FLASH				7									//��ɶ���

#define SUB_C_STORAGE					10									//���¿��
#define	SUB_C_STORAGEMAXMUL				11									//��������
#define SUB_C_REQUEST_QUERY_USER		12									//�����ѯ�û�
#define SUB_C_USER_DEBUG				13									//�û�����
#define SUB_C_REQUEST_UDPATE_ROOMINFO	14									//������·�����Ϣ
#define SUB_C_CLEAR_CURRENT_QUERYUSER	15

#define SUB_C_REQUEST_RCRecord			16									//��ѯ������¼

//�û���ע
struct CMD_C_AddScore
{
	LONGLONG							lScore;								//��ע��Ŀ
	WORD								wState;								//��ǰ״̬
};

//�������ݰ�
struct CMD_C_CompareCard
{	
	WORD								wCompareUser;						//�����û�
};

struct CMD_C_UpdateStorage
{
	LONGLONG						lRoomStorageCurrent;					//�����ֵ
	LONGLONG						lRoomStorageDeduct;						//�����ֵ
};

struct CMD_C_ModifyStorage
{
	LONGLONG						lMaxRoomStorage[2];						//�������
	WORD							wRoomStorageMul[2];						//Ӯ�ָ���
};

struct CMD_C_RequestQuery_User
{
	DWORD							dwGameID;								//��ѯ�û�GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			    //��ѯ�û��ǳ�
};

//�û�����
struct CMD_C_UserDebug
{
	DWORD							dwGameID;								//GAMEID
	TCHAR							szNickName[LEN_NICKNAME];				//�û��ǳ�
	USERDEBUG						userDebugInfo;						
};

//////////////////////////////////////////////////////////////////////////


#pragma pack(pop)

#endif
