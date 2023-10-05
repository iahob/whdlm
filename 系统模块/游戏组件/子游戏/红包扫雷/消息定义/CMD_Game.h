#ifndef CMD_GAME_HEAD_FILE_HBSL
#define CMD_GAME_HEAD_FILE_HBSL

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						148									//��Ϸ I D
#define GAME_NAME					TEXT("���ɨ��")					//��Ϸ����

#define GAME_PLAYER					MAX_CHAIR							//��Ϸ����

//�������
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//����״̬
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					//��Ϸ״̬

#define HISTORY_DEBUG_COUNT			10000000							//��������
#define MAX_BAG_COUNT				10									//����������
#define GAME_TIME					100									//��Ϸ��������10������۷�10*100
#define MIN_POINT_ONE				0.1									//��һλС������Сֵ
#define MIN_POINT_TWO				0.01								//�ڶ�λС������Сֵ

#define MAKELONGLONG(a, b)			((LONGLONG)(((DWORD)((a) & 0xffffffff)) | ((LONGLONG)((DWORD)((b) & 0xffffffff))) << 32))
#define D_EPUALS(a, b)				(fabs((a) - (b)) < 0.000001)
#define D_COMPARE(a, b)				((a) - 0.000001 > (b))

struct stRedBagInfo
{
	WORD	wDispatchUser;
	SCORE	lBagScore;
	BYTE	cbThunderPoint;
	SCORE	lBagKey;
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//������Ϣ
	int					nScoreRatio;										//��������
	BYTE				cbPlayTime;											//����ʱ��
	BYTE				cbBagCount;											//�������
	LONGLONG			lScoreRangeMin;										//�����Сֵ
	LONGLONG			lScoreRangeMax;										//������ֵ
	LONGLONG			lDispatchCondition;									//����Ҫ��
	DOUBLE				dBagTimes;											//����
	BYTE				cbThunderPositon;									//�׺�λ�ã�0С�����һλ��1С����ڶ�λ

	//��Ϸ��Ϣ
	WORD				wBankerUser;										//��ǰ�������
	LONGLONG			lScore;												//��ǰ�������
	BYTE				cbThunderPoint;										//�׺�
	BYTE				cbLeftCount;										//ʣ���������
	BYTE				cbLeftTime;											//ʣ������ʱ��
	WORD				wGetUser[MAX_BAG_COUNT];							//���������
	LONGLONG			lBagKey;											//��ǰ�����ʶ
	DOUBLE				dBagDetail;											//������
	DOUBLE				dUserScore;											//��ҵ÷�
	bool				bThunderPoint;										//�Ƿ�����
};

//��Ϸ״̬
struct CMD_S_StatusPlayAI
{
	CMD_S_StatusPlay	StatusPlay;											//��Ϸ����

	//AI����
	BYTE				cbMinBagListCount;									//�����б������������ֵʱ��ʼ���ף�
	BYTE				cbMaxRealBagListCount;								//�����б����������������������ֵʱ�����ף�
	BYTE				cbSetBagCount[2];									//���׸���
	BYTE				cbSetBagTime[2];									//���׼��
	LONGLONG			lSetBagScore[2];									//���׺����С
	BYTE				cbStartGetBagTime;									//�������ʼʱ��
	BYTE				cbGetBagTime[2];									//��������
	BYTE				cbMaxGetBagCount;									//������������
	BYTE				cbLastNoGetBagCount;								//������������
};

//����������ṹ
#define SUB_S_START							101								//��Ϸ��ʼ
#define SUB_S_DISPATCH_BAG					102								//�ɷ����
#define SUB_S_CANCEL_BAG					103								//ȡ�����
#define SUB_S_GET_BAG						104								//�����
#define SUB_S_GET_BAG_FAIL					105								//����ʧ��
#define SUB_S_END							106								//��������

#define SUB_S_REFRESH_STORAGE				1								//ˢ�¿����Ϣ
#define SUB_S_SET_ROOM_STORAGE				2								//���ӷ��������
#define SUB_S_DEL_ROOM_STORAGE				3								//ɾ�����������
#define SUB_S_SET_USER_STORAGE				4								//������ҿ�����
#define SUB_S_DEL_USER_STORAGE				5								//ɾ����ҿ�����
#define SUB_S_DEBUG_TEXT					6								//������ʾ��Ϣ
#define SUB_S_REFRESH_ROOM_DEBUG			7								//���·�������б�
#define SUB_S_REFRESH_HISTORY_ROOM_DEBUG	8								//������ʷ��������б�
#define SUB_S_REFRESH_USER_DEBUG			9								//�����û������б�
#define SUB_S_REFRESH_HISTORY_USER_DEBUG	10								//������ʷ�û������б�
#define SUB_S_REFRESH_SYS_DEBUG				11								//����ϵͳ�����б�
#define SUB_S_REFRESH_HISTORY_SYS_DEBUG		12								//������ʷϵͳ�����б�


//��Ϸ��ʼ
struct CMD_S_GameStart
{
	WORD				wBankerUser;										//ׯ��
	LONGLONG			lScore;												//������
	BYTE				cbThunderPoint;										//�׺�
	LONGLONG			lBagKey;											//��ʶ
};

//�����
struct CMD_S_DispatchBag
{
	WORD			wDispatchUser;											//�������
	LONGLONG		lScore;													//������
	LONGLONG		lBagKey;												//�����ʶ
};

//ȡ�����
struct CMD_S_CancelBag
{
	LONGLONG		lBagKey;												//ȡ�����
};

//�����
struct CMD_S_GetBag
{
	WORD			wGetUser;												//�������
	DOUBLE			dBagScore;												//�������
	DOUBLE			dUserScore;												//��ҵ÷�
	bool			bThunderPoint;											//����
	BYTE			cbLeftCount;											//ʣ�����
};

//����ʧ��
struct CMD_S_GetBag_Fail
{
	WORD			wGetUser;												//�������
	BYTE			cbReason;												//ʧ��ԭ��
};

//��������
struct CMD_S_GameEnd
{
	DOUBLE			dBankerScore;											//ׯ�ҵ÷�
	BYTE			cbGetCount;												//��������
	DOUBLE			dBagScore[MAX_BAG_COUNT];								//��������
	bool			bThunderPoint[MAX_BAG_COUNT];							//�Ƿ�����
	DOUBLE			dUserScore[MAX_BAG_COUNT];								//��ҵ÷�
};

//�ͻ�������ṹ
#define SUB_C_DISPATCH_BAG			1										//�ɷ����
#define SUB_C_GET_BAG				2										//�����

#define SUB_C_REFRESH_STORAGE		1										//ˢ�¿��
#define SUB_C_SET_CUR_STORAGE		2										//���õ�ǰ���
#define SUB_C_SET_INIT_STORAGE		3										//���ó�ʼ���
#define SUB_C_REFRESH_ROOM_STORAGE	4										//ˢ�·�������Ϣ
#define SUB_C_SET_ROOM_STORAGE		5										//���÷�����
#define SUB_C_CANCEL_ROOM_STORAGE	6										//ȡ��������
#define SUB_C_REFRESH_USER_STORAGE	7										//ˢ����ҿ����Ϣ
#define SUB_C_SET_USER_STORAGE		8										//������ҿ��
#define SUB_C_CANCEL_USER_STORAGE	9										//ȡ����ҿ��
#define SUB_C_REFRESH_HISTORY_RECORD 11										//ˢ����ʷ��־

//�����
struct CMD_C_DispatchBag
{
	LONGLONG		lScore;													//������
	LONGLONG		lThunderPoint;											//�׺�
};

//�����
struct CMD_C_GetBag
{
	LONGLONG   		lBagKey;												//�����ʶ
};


//�����Ϣ
struct CMD_C_SetCurStorage
{
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	bool							bSystemStorageEnd;						//����ֹ
};

//�����Ϣ
struct CMD_C_SetRoomStorage
{
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	bool							bSystemStorageEnd;						//����ֹ
};

//�����Ϣ
struct CMD_C_CancelRoomStorage
{
	DWORD							dwDebugIndex;							//��������
};

//��ҿ��
struct CMD_C_SetUserStorage
{
	DWORD							dwGameID;								//��ϷID
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	bool							bSystemStorageEnd;						//����ֹ
};

//�����Ϣ
struct CMD_C_CancelUserStorage
{
	DWORD							dwDebugIndex;							//��������
	DWORD							dwGameID;								//��ϷID
};
//////////////////////////////////////////////////////////////////////////////////
//�û���Ϊ
typedef enum { USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT } EM_USER_ACTION;
//�������ͣ����ȼ�������>����>ϵͳ���ԣ�
typedef enum
{
	RAND_DEBUG,																//�������
	USER_DEBUG,																//�˸�����
	ROOM_DEBUG,																//�������
	SYS_DEBUG																//ϵͳ����
}EM_CTRL_DEBUG;

typedef enum
{
	RUNNNING_STATUS = 0,													//ִ�е���
	WIAT_STATUS,															//�ȴ�����	
	CANCEL_STATUS,															//ȡ������
	FINISH_STATUS,															//��ɵ���
	DELETE_STATUS,															//ɾ������
}EM_DEBUG_STATUS;

//������ʾ
struct CMD_S_DebugText
{
	TCHAR							szMessageText[256];						//�����ı�
};

//�����Ϣ����
struct CMD_S_BaseStorageInfo
{
	LONGLONG						lStartDebugTime;						//����������ʱ��
	LONGLONG						lResetSystemStorage;					//���ÿ��
	DWORD							dwSysDebugIndex;						//ϵͳ��������
	bool							bSystemStorageEnd;						//ϵͳ����ֹ	
	LONGLONG						lInitSystemStorage;						//��ʼϵͳ���
	LONGLONG						lLastInitSystemStorage;					//�Ͼ�ϵͳ���
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lInitUserStorage;						//��ʼ��ҿ��
	LONGLONG						lLastInitUserStorage;					//�Ͼֳ�ʼ��ҿ��
	LONGLONG						lUserStorage;							//��ҿ��
	int								nInitParameterK;						//��ʼ����K(�ٷֱ�)
};

//�����Ϣ����
struct CMD_S_UpdateStorageInfo
{
	DWORD							dwDebugIndex;							//��������
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lUserStorage;							//��ҿ��
	int								nDebugStatus;							//����״̬
	LONGLONG						lUpdateTime;							//����ʱ��
};

//�����Ϣ����
struct CMD_S_SetRoomStorageInfo
{
	DWORD							dwDebugIndex;							//��������
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//����K(�ٷֱ�)
	bool							bSystemStorageEnd;						//ϵͳ����ֹ	
	LONGLONG						lUpdateTime;							//����ʱ��
	int								nDebugStatus;							//����״̬
};

//�����Ϣ����
struct CMD_S_SetUserStorageInfo
{
	DWORD							dwDebugIndex;							//��������
	DWORD							dwGameID;								//�������ID
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//����K(�ٷֱ�)
	bool							bSystemStorageEnd;						//ϵͳ����ֹ	
	LONGLONG						lUpdateTime;							//����ʱ��
};

//�����Ϣɾ��
struct CMD_S_DeleteStorageInfo
{
	DWORD							dwDebugIndex;							//��������
};

//�����û���Ϣ
typedef struct
{
	WORD							wTableID;								//����ID
	DWORD							dwGameID;								//GAMEID
	bool							bAndroid;								//AI��ʶ
	TCHAR							szNickName[LEN_NICKNAME];				//�û��ǳ�
	BYTE							cbUserStatus;							//�û�״̬
	BYTE							cbGameStatus;							//��Ϸ״̬
	LONGLONG						lGameScore;								//��Ϸ��Ӯ��

	//���˵��Բ���
	DWORD							dwDebugIndex;							//��������
	CTime							tDebugTime;								//����ʱ��
	CTime							tUpdateTime;							//����ʱ��
	bool							bSystemStorageEnd;						//ϵͳ����ֹ
	bool							bDebugIndividual;						//���Ը��˱�ʶ	
	LONGLONG						lInitSystemStorage;						//��ʼϵͳ���
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lInitUserStorage;						//��ʼ��ҿ��
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)

} tagRoomUserInfo;

//�����û���Ϣ
typedef struct
{
	DWORD							dwDebugIndex;							//��������
	DWORD							dwGameID;								//GAMEID
	LONGLONG						lDebugTime;								//����ʱ��
	LONGLONG						lUpdateTime;							//����ʱ��
	bool							bSystemStorageEnd;						//ϵͳ����ֹ	
	LONGLONG						lInitSystemStorage;						//��ʼϵͳ���
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lInitUserStorage;						//��ʼ��ҿ��
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	int								nDebugStatus;							//����״̬
} tagHistoryRoomUserInfo;

//�������
typedef struct
{
	DWORD							dwDebugIndex;							//��������
	CTime							tDebugTime;								//����ʱ��
	CTime							tUpdateTime;							//����ʱ��
	bool							bSystemStorageEnd;						//ϵͳ����ֹ
	LONGLONG						lInitSystemStorage;						//��ʼϵͳ���
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lInitUserStorage;						//��ʼ��ҿ��
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	int								nDebugStatus;							//����״̬

}tagRoomDebugInfo;


//������ԣ�Ψһ��DWORD��Ӧһ�����ԣ�
typedef struct
{
	DWORD							dwDebugIndex;							//��������
	LONGLONG						lDebugTime;								//����ʱ��
	LONGLONG						lUpdateTime;							//����ʱ��
	bool							bSystemStorageEnd;						//ϵͳ����ֹ
	LONGLONG						lInitSystemStorage;						//��ʼϵͳ���
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lInitUserStorage;						//��ʼ��ҿ��
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	int								nDebugStatus;							//����״̬

}tagHistoryRoomDebugInfo;

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
