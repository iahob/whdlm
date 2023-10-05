#pragma once

#include "GameControlDefine.h"

#include <list>
#include <map>
#include <chrono>
#include <cassert>
#include <functional>
#include <queue>

interface ITableFrame;
interface IServerUserItem;
/////////////////////////////////////////////////////////////////////////////////////////
//�ӵ��Ի���

class CGameControl;
//���Ϳ���
class BaseCrontol
{
public:
	CGameControl*					m_pGameControl;										//�ϲ�ӿ�
	ControlType						m_ctrlType;											//��������
	SCOREEX							m_lSysCtrlSysWin;									//ϵͳ��Ӯ����
	SCOREEX							m_lSysCtrlPlayerWin;								//�����Ӯ����
	int64_t							m_lStorageResetCount;								//������ô���

public:
	BaseCrontol(ControlType type)
	{
		m_ctrlType = type;
		m_lSysCtrlSysWin = 0;
		m_lSysCtrlPlayerWin = 0;
		m_lStorageResetCount = 0;
	}
	virtual ~BaseCrontol(){};

public:
	//����һ������
	virtual void Add(BaseCtrlInfo* pInfo)
	{
		pInfo->curStatus = WaitForCtrl;
		pInfo->lUpdateTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	}
	//���һ����Ч����
	virtual BaseCtrlInfo* GetValid() = 0;
	//������һ������
	virtual BaseCtrlInfo* GetLast() = 0;
	//��������״̬
	virtual void CancelControl(int64_t nChooseIndx, bool bRemove = false) = 0;
	//���¿��
	virtual void UpdateStorage(SCOREEX lChangeValue) = 0;
	//������һ������
	virtual void ActiveNext() = 0;
	//ִ�����
	virtual void Done() = 0;
	//���͸����¼�
	virtual void SendUpdateEvent(void* pInfo) = 0;
	//ɾ�����������
	virtual bool RemoveDone() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////
//��ҵ���

class CDebugCrontol :public BaseCrontol
{
public:
	std::list<GameDebugInfo>		m_ctrllist;

public:
	CDebugCrontol(ControlType type) :BaseCrontol(type){}
	~CDebugCrontol(){}

public:
	//����һ������
	virtual void Add(BaseCtrlInfo* pInfo);
	//���һ����Ч����
	virtual BaseCtrlInfo* GetValid();
	//������һ������
	virtual BaseCtrlInfo* GetLast();
	//��������״̬
	virtual void CancelControl(int64_t nChooseIndx, bool bRemove = false);
	//���¿��
	virtual void UpdateStorage(SCOREEX lChangeValue);
	//������һ������
	virtual void ActiveNext();
	//ִ�����
	virtual void Done();
	//���͸����¼�
	virtual void SendUpdateEvent(void* pInfo);
	//ɾ�����������
	virtual bool RemoveDone();

public:
	//
	BaseCtrlInfo* GetValid(int32_t nGameID);
	//
	void UpdateStorage(int32_t nGameID, SCOREEX lChangeValue);
};

/////////////////////////////////////////////////////////////////////////////////////////
//ϵͳ���ԡ��������

class CStorageCrontol :public BaseCrontol
{
public:
	std::list<StorageInfo>		m_ctrllist;

public:
	CStorageCrontol(ControlType type) :BaseCrontol(type){}
	~CStorageCrontol(){}

public:
	//����һ������
	virtual void Add(BaseCtrlInfo* pInfo);
	//���һ����Ч����
	virtual BaseCtrlInfo* GetValid();
	//������һ������
	virtual BaseCtrlInfo* GetLast();
	//��������״̬
	virtual void CancelControl(int64_t nChooseIndx, bool bRemove = false);
	//���¿��
	virtual void UpdateStorage(SCOREEX lChangeValue);
	//������һ������
	virtual void ActiveNext();
	//ִ�����
	virtual void Done();
	//���͸����¼�
	virtual void SendUpdateEvent(void* pInfo);
	//ɾ�����������
	virtual bool RemoveDone();
};

/////////////////////////////////////////////////////////////////////////////////////////
//Ȩ������

class CGameWeightControl
{
	std::vector<WeightConfig>				m_weightConfig;
	int64_t									m_lTotalWieght;

public:
	CGameControl*							m_pGameControl;										//�ϲ�ӿ�

public:
	CGameWeightControl(){};
	virtual ~CGameWeightControl(){};

public:
	int32_t Size(){ return m_weightConfig.size(); }
	WeightConfig At(uint32_t idx)
	{
		if (idx >= m_weightConfig.size())
			return WeightConfig();

		return m_weightConfig[idx];
	}
	//�����ڵĻ��Զ�����
	void Add(int64_t minTimes, int64_t maxTimes = 0, int64_t weight = 1)
	{
		for (auto& it : m_weightConfig)
		{
			if (it.lMinTimes == minTimes && it.lMaxTimes == maxTimes)
			{
				m_lTotalWieght += weight - it.lWeight;
				it.lWeight = weight;
				return;
			}
		}

		m_weightConfig.emplace_back(WeightConfig(minTimes, maxTimes, weight, m_weightConfig.size()));
		m_lTotalWieght += weight;
	}
	//
	void Update(uint32_t nPos, int64_t weight)
	{
		if (nPos >= m_weightConfig.size() || weight < 1)
		{
			assert(false);
			return;
		}

		m_lTotalWieght += weight - m_weightConfig[nPos].lWeight;
		m_weightConfig[nPos].lWeight = weight;
	}

public:
	int CalcRatio(int64_t lWeight)
	{
		return (int)((SCOREEX)lWeight / m_lTotalWieght * 1000);
	}
	int GetTargetTimeRatio(int64_t lMinTimes, int64_t lMaxTimes = 0)
	{
		for (auto& it : m_weightConfig)
		{
			if (it.lMinTimes <= lMinTimes && (it.lMaxTimes >= lMaxTimes || it.lMaxTimes < 0))
			{
				return CalcRatio(it.lWeight);
			}
		}

		return 0;
	}

	WeightConfig GetRandConfig()
	{
		if (m_lTotalWieght == 0) m_lTotalWieght = 1000;
		int nRandValue = rand() % m_lTotalWieght;
		int64_t nStart = 0;
		for (auto& it : m_weightConfig)
		{
			if (nRandValue >= nStart && nRandValue < nStart + it.lWeight)
			{
				return it;
			}

			nStart += it.lWeight;
		}

		assert(false);
		return WeightConfig();
	}
	void GetNextTimes(int64_t nStartTimes,int64_t& lCurMinTimes, int64_t& lCurMaxTimes)
	{
		for (auto& it : m_weightConfig)
		{
			if (it.lMinTimes > nStartTimes)
			{
				lCurMinTimes = it.lMinTimes;
				lCurMaxTimes = it.lMaxTimes;
				if (lCurMaxTimes < 0) lCurMaxTimes = MAX_LONGLONG;
				else if (lCurMaxTimes == 0) lCurMaxTimes = lCurMinTimes;
				if (lCurMinTimes == lCurMaxTimes)
				{
					if (it.lIndex == 0) lCurMinTimes = 0;
					else
					{
						lCurMinTimes = At(static_cast<int32_t>(it.lIndex - 1)).lMaxTimes;
						if (lCurMinTimes == 0)
							lCurMinTimes = At(static_cast<int32_t>(it.lIndex - 1)).lMinTimes;
					}
				}
				return;
			}
		}
		assert(false);
		return;
	}
	void GetRandTimes(int64_t& lCurMinTimes, int64_t& lCurMaxTimes)
	{
		WeightConfig randWeight = GetRandConfig();
		if (randWeight.lWeight > 0)
		{
			lCurMinTimes = randWeight.lMinTimes;
			lCurMaxTimes = randWeight.lMaxTimes;
			if (lCurMaxTimes < 0) lCurMaxTimes = MAX_LONGLONG;
			else if (lCurMaxTimes == 0) lCurMaxTimes = lCurMinTimes;
			if (lCurMinTimes == lCurMaxTimes)
			{
				if (randWeight.lIndex == 0) lCurMinTimes = 0;
				else
				{
					lCurMinTimes = At(static_cast<int32_t>(randWeight.lIndex - 1)).lMaxTimes;
					if (lCurMinTimes == 0)
						lCurMinTimes = At(static_cast<int32_t>(randWeight.lIndex - 1)).lMinTimes;
				}
			}
		}
		else
		{
			lCurMinTimes = 0;
			lCurMaxTimes = MAX_LONGLONG;
		}
	}

	void SendUpdateEvent();
};

/////////////////////////////////////////////////////////////////////////////////////////
//��Ϸ���ƹ�����

//����ʵ��
class CGameControl
{
	friend class CStorageCrontol;
	friend class CDebugCrontol;
	friend class CGameWeightControl;

	std::map<ControlType, BaseCrontol*>					m_ctrlTypes;					//���п�������
	std::queue<ControlUpdateEvent>						m_eventUpdate;					//�¼�����
	char												m_eventSendBuffer[MAX_EVENT_SEND_BUFFER];//�¼����ͻ�����

public:
	GameTaxInfo						m_gameTax;											//��Ϸ��ˮ
	GameRewardInfo					m_gameReward;										//�ʽ�
	GameExtraInfo					m_gameExtraInfo;									//��Ϸ����
	//		
	CGameWeightControl				m_gameWieght;										//����Ȩ��
	//
	StatisticsInfo					m_gameStatisticsInfo;								//ͳ������

public:
	CGameControl();
	virtual ~CGameControl();

public:
	//��ȡ��ǰ��Ч����
	BaseCrontol* GetCurValidControl(int32_t nGameID = -1);
	//��ȡϵͳ��ǰĳ���Ϳ�����Ϣ
	BaseCrontol* GetCurTypeCtrl(ControlType type);
	//��ȡ�����Ӯ����
	int32_t GetUserWinLoseRatio(int32_t nGameID = -1);
	//��õ�ǰ��ҿ��
	SCOREEX GetCurPlayerStorage(int32_t nGameID = -1);
	//��õ�ǰ���
	void GetCurStorage(SCOREEX& dSysStorage,SCOREEX& dUserStorage,int32_t nGameID = -1);

public:
	//����һ����������
	void AddControl(ControlType type, GameDebugInfo& info);
	void AddControl(ControlType type, StorageInfo& info);
	void AddControl(ControlType type, GameTaxInfo& info);
	void AddControl(ControlType type, GameRewardInfo& info);
	void AddControl(ControlType type, GameExtraInfo& info);
	void AddControl(ControlType type, WeightConfig* pInfo, int nSize);
	//ȡ��ĳһ����������(<0��ʾȡ������ͷ������),�Ƿ��Ƴ�����
	void CancelControl(ControlType type, int32_t index,bool bRemove = false);
	//���������Ӯ�����¿��
	void UpdateControlStorage(SCOREEX lUserWinLose, int32_t nGameID = -1);
	//��������ֵ
	void AddStorage(ControlEventType type,SCOREEX dValue);

	//
public:
	//������Ϸ������
	void LineGameUpdateStorage(SCOREEX dUserBet, SCOREEX dUserWinScore, int32_t nGameID);

protected:
	//���͸����¼�
	void SendUpdateEvent(ControlEventType type, void* pInfo);

public:
	//�¼�
	void SendEvent(ControlUpdateEvent& event);
	//�����¼���֪ͨ���Զ�
	void RefreshEvent(ITableFrame* pTable, IServerUserItem *pIServerUserItem);
	//
	void RefreshAll(ITableFrame* pTable, IServerUserItem *pIServerUserItem);

	//���߽ӿ�
public:
	//����ʤ��
	static int32_t CalcWinRatio(SCOREEX lCurSysStorage, SCOREEX lCurPlayerStorage, int64_t lCurParameteK);
	//����˰��(ǧ�ֱ�)
	static SCOREEX CalcTax(int32_t nTaxRatio, SCOREEX dScore)
	{
		if (dScore <= 0 || nTaxRatio <= 0)
			return 0;
		//2λ����
		int64_t tax = static_cast<int64_t>(dScore*nTaxRatio / 1000*100);
		return static_cast<SCOREEX>(tax/100);
	};
	//��ȡlist�ƶ�λ��Ԫ�أ�δ�ҵ����ؿ�
	template<typename T = GameDebugInfo>
	static T* ListGetParam(std::list<T>& input,int32_t pos)
	{
		if (input.size() <= static_cast<size_t>(pos))
		{
			return nullptr;
		}

		for (auto& it : input)
		{
			if (0 == pos)
			{
				return (T*)&it;
			}
			--pos;
		}

		return nullptr;
	};
};

