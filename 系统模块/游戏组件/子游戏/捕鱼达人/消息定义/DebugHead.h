#pragma once

// ��ҵ�������
enum EnumPalyBullionsType
{
	PalyBullionsType_Hit,
	PalyBullionsType_Miss,
};
// Ԫ������
typedef CMap< unsigned long, unsigned long, EnumPalyBullionsType, EnumPalyBullionsType > CMapPalyBullions;

// ��Ҳ���ըȨ��
enum EnumExplosionAdmin
{
	ExplosionType_NO,				//�޷�����
	ExplosionType_Allow,			//������
	ExplosionType_Invalid=255,		//��ը��Ч
};

//�������ͣ����ȼ�������>����>ϵͳ���ԣ�
typedef enum
{
	USER_CTRL,																//�˸�����
	ROOM_CTRL,																//�������
	SYS_CTRL																//ϵͳ����
}EM_CTRL_TYPE;

typedef enum
{
	RUNNNING_STATUS = 0,													//ִ�е���
	WIAT_STATUS,															//�ȴ�����	
	CANCEL_STATUS,															//ȡ������
	FINISH_STATUS,															//��ɵ���
	DELETE_STATUS,															//ɾ������
}EM_DEBUG_STATUS;

struct tagRoomUserInfo
{
	TCHAR				szPlayName[32];
	DWORD				wPlayID;
	WORD				wChairID;
	WORD				wTableID;
	LONGLONG			lPlayScore;
	LONGLONG			lUserScore;
	LONGLONG			lEnterScore;

	//���˵��Բ���
	bool				bDebugIndividual;						//���Ը��˱�ʶ
	DWORD				dwDebugIndex;							//��������
	CTime				tDebugTime;								//����ʱ��
	CTime				tUpdateTime;							//����ʱ��
	bool				bSystemStorageEnd;						//ϵͳ����ֹ	
	double				dInitSystemStorage;						//��ʼϵͳ���
	double				dSystemStorage;							//ϵͳ���
	double				dInitUserStorage;						//��ʼ��ҿ��
	double				dUserStorage;							//��ҿ��
	int					nParameterK;							//��ǰ����K(�ٷֱ�)

	bool				bAtRoom;
	EnumExplosionAdmin  enExpAdmin;
};
typedef CMap< DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo > CMapRoomUserInfo;

//�����û���Ϣ
struct tagHistoryRoomUserInfo
{
	DWORD							dwDebugIndex;							//��������
	DWORD							dwGameID;								//GAMEID
	LONGLONG						lDebugTime;								//����ʱ��
	LONGLONG						lUpdateTime;							//����ʱ��
	bool							bSystemStorageEnd;						//ϵͳ����ֹ	
	double							dInitSystemStorage;						//��ʼϵͳ���
	double							dSystemStorage;							//ϵͳ���
	double							dInitUserStorage;						//��ʼ��ҿ��
	double							dUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	int								nDebugStatus;							//����״̬
};
typedef CList< tagHistoryRoomUserInfo, tagHistoryRoomUserInfo > CListHistoryRoomUserInfo;

struct tagRoomDebugInfo
{
	DWORD							dwDebugIndex;							//��������
	CTime							tDebugTime;								//����ʱ��
	CTime							tUpdateTime;							//����ʱ��
	bool							bSystemStorageEnd;						//ϵͳ����ֹ
	double							dInitSystemStorage;						//��ʼϵͳ���
	double							dSystemStorage;							//ϵͳ���
	double							dInitUserStorage;						//��ʼ��ҿ��
	double							dUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	int								nDebugStatus;							//����״̬
};
typedef CList<tagRoomDebugInfo, tagRoomDebugInfo > CListRoomDebugInfo;

//������ԣ�Ψһ��DWORD��Ӧһ�����ԣ�
struct tagHistoryRoomDebugInfo
{
	DWORD							dwDebugIndex;							//��������
	LONGLONG						lDebugTime;								//����ʱ��
	LONGLONG						lUpdateTime;							//����ʱ��
	bool							bSystemStorageEnd;						//ϵͳ����ֹ
	double							dInitSystemStorage;						//��ʼϵͳ���
	double							dSystemStorage;							//ϵͳ���
	double							dInitUserStorage;						//��ʼ��ҿ��
	double							dUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	int								nDebugStatus;							//����״̬

};
typedef CList<tagHistoryRoomDebugInfo, tagHistoryRoomDebugInfo > CListHistoryRoomDebugInfo;


// �ͻ��˳�����Ϣ����ṹ
enum
{
	SUBC_C_REFRESH_STORAGE = 1,												//ˢ�¿��
	SUBC_C_SET_CUR_STORAGE,													//���õ�ǰ���
	SUBC_C_SET_ROOM_STORAGE,												//���÷�����
	SUBC_C_CANCEL_ROOM_STORAGE,												//ȡ��������
	SUBC_C_SET_USER_STORAGE,												//������ҿ��
	SUBC_C_CANCEL_USER_STORAGE,												//ȡ����ҿ��
	SUBC_C_RULE_SCENE,														//���³�������
	SUBC_C_RULE_DIAN_MAN,													//����ȫ�ֱ�ը����
};

//�����Ϣ
struct CMDC_C_SetRoomStorage
{
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	bool							bSystemStorageEnd;						//����ֹ
};

//�����Ϣ
struct CMDC_C_CancelRoomStorage
{
	DWORD							dwDebugIndex;							//��������
};

//��ҿ��
struct CMDC_C_SetUserStorage
{
	DWORD							dwGameID;								//��ϷID
	LONGLONG						lSystemStorage;							//ϵͳ���
	LONGLONG						lUserStorage;							//��ҿ��
	int								nParameterK;							//��ǰ����K(�ٷֱ�)
	bool							bSystemStorageEnd;						//����ֹ
};

//�����Ϣ
struct CMDC_C_CancelUserStorage
{
	DWORD							dwDebugIndex;							//��������
	DWORD							dwGameID;								//��ϷID
};

// ��������
struct CMDC_C_RuleScene
{
	// ��������
	int						nCreateCount;							// ��������
	int						nSceneTime;								// ����ά��ʱ��
	int						nMaxBigFishCount;						// ���������޶�

	// �ӵ�����
	int						nBulletVelocity;						// �ӵ��ٶ�
	int						nBulletCoolingTime;						// �ӵ���ȴ

};

//ȫ�ֱ�ը
struct CMDC_C_RuleExplosion
{
	int								nExplosionProportion;					// ��ը����
	int								nExplosionStart[2];						// ��ը����
	longlong						lExplosionCondition;					// ��ը����
	longlong						lExplosionObtainMax24;					// 24Сʱ����ȡ��
	int								nExplosionConditionType;				// ��������
};

// ������������Ϣ����ṹ
enum
{
	SUBC_S_REFRESH_STORAGE = 1,												//ˢ�¿����Ϣ
	SUBC_S_REFRESH_OTHER,													//ˢ�¿����Ϣ
	SUBC_S_SET_ROOM_STORAGE,												//���ӷ��������
	SUBC_S_DEL_ROOM_STORAGE,												//ɾ�����������
	SUBC_S_SET_USER_STORAGE,												//������ҿ�����
	SUBC_S_DEL_USER_STORAGE,												//ɾ����ҿ�����
	SUBC_S_DEBUG_TEXT,														//������ʾ��Ϣ
	SUBC_S_REFRESH_ROOM_DEBUG,												//���·�������б�
	SUBC_S_REFRESH_HISTORY_ROOM_DEBUG,										//������ʷ��������б�
	SUBC_S_REFRESH_USER_DEBUG,												//�����û������б�
	SUBC_S_REFRESH_HISTORY_USER_DEBUG,										//������ʷ�û������б�
	SUBC_S_REFRESH_SYS_DEBUG,												//����ϵͳ�����б�
	SUBC_S_REFRESH_HISTORY_SYS_DEBUG,										//������ʷϵͳ�����б�
	SUBC_S_RULE_SCENE,														//���³�������
	SUBC_S_RULE_DIAN_MAN,													//����ȫ�ֱ�ը����
	SUBC_S_REFRESH_PLAYER_WATCH,											//������Ҽ����Ϣ
};

//������ʾ
struct CMDC_S_DebugText
{
	TCHAR							szMessageText[256];						//�����ı�
};

//�����Ϣ����
struct CMDC_S_BaseStorageInfo
{
	LONGLONG						lStartDebugTime;						//����������ʱ��
	LONGLONG						lResetSystemStorage;					//���ÿ��
	DWORD							dwSysDebugIndex;						//ϵͳ��������
	bool							bSystemStorageEnd;						//ϵͳ����ֹ	
	double							dInitSystemStorage;						//��ʼϵͳ���
	double							dLastInitSystemStorage;					//��ʼϵͳ���
	double							dSystemStorage;							//ϵͳ���
	double							dInitUserStorage;						//��ʼ��ҿ��
	double							dLastInitUserStorage;					//��ʼ��ҿ��
	double							dUserStorage;							//��ҿ��
	int								nInitParameterK;						//��ʼ����K(�ٷֱ�)
};

//�����Ϣ����
struct CMDC_S_UpdateStorageInfo
{
	DWORD							dwDebugIndex;							//��������
	double							dSystemStorage;							//ϵͳ���
	double							dUserStorage;							//��ҿ��
	int								nDebugStatus;							//����״̬
	LONGLONG						lUpdateTime;							//����ʱ��

	double							dStorageCurrent;						//ʵʱ��Ӯ
};

//�����Ϣ����
struct CMDC_S_UpdateOtherInfo
{
	double							dDartStock;								// ���ڿ��
	double							dWaitDartStock;							// ���ڿ��
	longlong						lRoomRevenue;							// ����ʵ��˰��
};

//�����Ϣ����
struct CMDC_S_SetRoomStorageInfo
{
	DWORD							dwDebugIndex;							//��������
	double							dSystemStorage;							//ϵͳ���
	double							dUserStorage;							//��ҿ��
	int								nParameterK;							//����K(�ٷֱ�)
	bool							bSystemStorageEnd;						//ϵͳ����ֹ	
	LONGLONG						lUpdateTime;							//����ʱ��
	int								nDebugStatus;							//����״̬
};

//�����Ϣ����
struct CMDC_S_SetUserStorageInfo
{
	DWORD							dwDebugIndex;							//��������
	DWORD							dwGameID;								//�������ID
	double							dSystemStorage;							//ϵͳ���
	double							dUserStorage;							//��ҿ��
	int								nParameterK;							//����K(�ٷֱ�)
	bool							bSystemStorageEnd;						//ϵͳ����ֹ	
	LONGLONG						lUpdateTime;							//����ʱ��
};

//�����Ϣɾ��
struct CMDC_S_DeleteStorageInfo
{
	DWORD							dwDebugIndex;							//��������
};

// ��������
struct CMDC_S_RuleScene
{
	// ��������
	int						nCreateCount;							// ��������
	int						nSceneTime;								// ����ά��ʱ��
	int						nMaxBigFishCount;						// ���������޶�

	// �ӵ�����
	int						nBulletVelocity;						// �ӵ��ٶ�
	int						nBulletCoolingTime;						// �ӵ���ȴ
};

//ȫ�ֱ�ը���
struct CMDC_S_RuleExplosion
{
	int								nExplosionProportion;					// ��ը����
	int								nExplosionStart[2];						// ��ը����
	longlong						lExplosionCondition;					// ��ը����
	longlong						lExplosionObtainMax24;					// 24Сʱ����ȡ��
	int								nExplosionConditionType;				// ��������
};

//�����Ϣ����
struct CMDC_S_UserWatch
{
	DWORD							dwGameID[4];							// �������ID
	tagRoomUserInfo					userInfo[4];							// �û���Ϣ
};