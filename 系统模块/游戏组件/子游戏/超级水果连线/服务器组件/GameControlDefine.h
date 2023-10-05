#ifndef __H_GAME_CONTROL_DEFINE_H__
#define __H_GAME_CONTROL_DEFINE_H__
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////////
//���Կ��ƶ�(��������ͻظ�)
#define SUB_C_EVENT_UPDATE					11									//�¼�����
#define SUB_S_EVENT_UPDATE					12									//�¼�����
//////////////////////////////////////////////////////////////////////////////////

#ifndef SCOREEX
#define SCOREEX int64_t
#endif

////////////////////////////////////////////
//���Ͷ���
#define MAX_EVENT_SEND_BUFFER		8192

//�����¼�����
enum ControlEventType
{
	EventMin = 0,
	EventSysCtrl,			//ϵͳ���Ը���
	EventRoomCtrl,			//������Ը���
	EventUserCtrl,			//��ҵ��Ը���
	EventGameTax,			//��Ϸ��ˮ����
	EventGameReward,		//��Ϸ�ʽ����
	EventGameExtra,			//��Ϸ�������ݸ���
	EventGameStatistics,	//��Ϸ�ۺ����ݸ���
	EventGameWeightConfig,	//Ȩ������
	EventMax,
};

//�����¼�����֪ͨ(��ͷ)
struct ControlUpdateEvent
{
	ControlEventType		emEventType;
	int64_t					nDataLen;
	void*					pData;
};

//�����ȼ���ϵͳ=����>����>�շű�
enum ControlType
{
	MinCtrl = 0,
	SysCtrl,							//ϵͳ������
	RoomCtrl,							//���������
	UserCtrl,							//��ҵ���
	//
	GameTax,							//˰��
	GameReward,							//�ʽ�
	GameExtra,							//����
	GameWeight,							//Ȩ��
	MaxCtrl,
};
enum ControlStatus
{
	NullStatus = 0,
	CancelCtrl,
	WaitForCtrl,
	AlreadyInCtrl,
	DoneCtrl,
	RemoveCtrl,
};
struct BaseCtrlInfo
{
	int64_t							nId = 0;											//������
	int64_t							lUpdateTime = 0;									//����ʱ��
	ControlStatus					curStatus = WaitForCtrl;							//��ǰ״̬
};

//ϵͳ���ԡ������������
struct StorageInfo :public BaseCtrlInfo
{
	SCOREEX							lConfigSysStorage = 188000;							//����ϵͳ���(ϵͳҪӮ��Ǯ)
	SCOREEX							lConfigPlayerStorage = 168000;						//������ҿ��(���ҪӮ��Ǯ)
	int64_t							lConfigParameterK = 10;								//���õ���ϵ��(�ٷֱ�)
	int64_t							lConfigResetSection = 5;							//ǿ����������(�ٷֱ�)

	SCOREEX							lCurSysStorage = 188000;							//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
	SCOREEX							lCurPlayerStorage = 168000;							//��ǰ��ҿ��(���ҪӮ��Ǯ)
	int64_t							lCurParameterK = 10;								//��ǰ����ϵ��(�ٷֱ�):
	int64_t							lCurResetSection = 5;								//ǿ����������(�ٷֱ�)

	SCOREEX							lSysCtrlSysWin = 0;									//ϵͳ��Ӯ����
	SCOREEX							lSysCtrlPlayerWin = 0;								//�����Ӯ����
	int64_t							lWinRatio = 0;										//��ǰ��Ӯ����
	int64_t							lResetTimes = 0;									//���ô���
};

//��ҵ�������
struct GameDebugInfo :public BaseCtrlInfo
{
	int32_t							nType = 0;												//��������
	int32_t							nMaxWorkCount = 0;										//���ִ�д���
	int32_t							nCurWorkCount = 0;										//��ǰִ�д���
	int32_t							nGameID = 0;											//�������GameID

	SCOREEX							lConfigSysStorage = 188000;							//����ϵͳ���(ϵͳҪӮ��Ǯ)
	SCOREEX							lConfigPlayerStorage = 168000;						//������ҿ��(���ҪӮ��Ǯ)
	int64_t							lConfigParameterK = 10;								//���õ���ϵ��(�ٷֱ�):
	int64_t							lConfigResetSection = 5;							//ǿ����������(�ٷֱ�)

	SCOREEX							lCurSysStorage = 188000;							//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
	SCOREEX							lCurPlayerStorage = 168000;							//��ǰ��ҿ��(���ҪӮ��Ǯ)
	int64_t							lCurParameterK = 10;								//��ǰ����ϵ��(�ٷֱ�)
	int64_t							lCurResetSection = 5;								//ǿ����������(�ٷֱ�):

	SCOREEX							lSysCtrlSysWin = 0;									//ϵͳ��Ӯ����
	SCOREEX							lSysCtrlPlayerWin = 0;								//�����Ӯ����
	int64_t							lWinRatio = 0;										//��ǰ��Ӯ����
	int64_t							lResetTimes = 0;									//���ô���
};

//Ȩ������
struct WeightConfig
{
#define WEIGHT_CONFIG_MAX_SZIE	14
	int64_t		lIndex;					//����
	int64_t		lMinTimes;				//��С����
	int64_t		lMaxTimes;				//�����
	int64_t		lWeight;				//����Ȩ��(1~1000)
	int64_t		lRatio;					//��������
	WeightConfig(int64_t minTimes = 0, int64_t maxTimes = 0, int64_t weight = 1, int64_t index = 0)
	{
		lMinTimes = minTimes;
		lMaxTimes = maxTimes;
		lWeight = weight;
		lIndex = index;
		lRatio = 0;
	}
};

//��ˮ����
struct GameTaxInfo
{
	SCOREEX		lCurStorage;				//��ǰ��ˮ����
	int64_t		lTaxRatio;					//��ˮ����
	GameTaxInfo()
	{
		lCurStorage = 0;
		lTaxRatio = 2;
	}
};
//�ʽ�����
struct GameRewardInfo
{
	SCOREEX		lConfigStorage;				//���òʽ���
	SCOREEX		lCurStorage;				//��ǰ�ʽ���
	SCOREEX		lDispatchStorage;			//�ɲ����
	SCOREEX		lVirtualStorage;			//����ʽ���(�ͻ�����ʾʹ��)
	SCOREEX		lUserBetDispatch;			//������ע�ɲ�����
	int64_t		lDispatchRatio;				//�ɲʸ���
	int64_t		lTaxRatio;					//��ˮ����
	GameRewardInfo()
	{
		lConfigStorage = 0;
		lCurStorage = 0;
		lDispatchStorage = 0;
		lDispatchRatio = 2;
		lTaxRatio = 3;
		lVirtualStorage = 0;
		lUserBetDispatch = 0;
	}
};
//��������
struct GameExtraInfo
{
	int64_t		lFreeGameRatio;				//�����Ϸ����
	int64_t		lExtraGameRatio;			//С��Ϸ����
	GameExtraInfo()
	{
		lFreeGameRatio = 2;
		lExtraGameRatio = 3;
	}
};

//�ӿ�ͳ������
struct SubControlStatisticsInfo
{
	SCOREEX							lSysCtrlSysWin;									//ϵͳ��Ӯ����
	SCOREEX							lSysCtrlPlayerWin;								//�����Ӯ����
	int64_t							lStorageResetCount;								//������ô���
};
//��������
struct GameConfig
{
	SCOREEX							lStorageMin;									//�����С����
	SCOREEX							lStorageMax;									//����������
};
//��Ϸͳ������
struct StatisticsInfo
{
	SCOREEX							lSysCtrlSysWin;									//ϵͳ��Ӯ����
	SCOREEX							lSysCtrlPlayerWin;								//�����Ӯ����
	SCOREEX							lTotalServiceTax;								//��˰��
	int64_t							lStartTime;										//��ʼʱ��

	SubControlStatisticsInfo		stSystem;										//ϵͳ����
	SubControlStatisticsInfo		stRoom;											//�������
	SubControlStatisticsInfo		stArea;											//��ҵ���

	GameConfig						stGameConfig;									//��Ϸ����
};
#endif