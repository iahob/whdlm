#include "Stdafx.h"
#include "GameControl.h"

//////////////////////////////////////////////////////////////////////////
//�������
//����һ������
void CDebugCrontol::Add(BaseCtrlInfo* pInfo)
{
	BaseCrontol::Add(pInfo);
	pInfo->nId = 0;
	if (!m_ctrllist.empty()) pInfo->nId = m_ctrllist.back().nId + 1;

	bool bUpdate = false;
	GameDebugInfo* pDebug = static_cast<GameDebugInfo*>(pInfo);
	
	//��ͬGameID��
	for (auto& it : m_ctrllist)
	{
		if (it.nGameID == pDebug->nGameID)
		{
#if 1//����
			it = *pDebug;
			it.lConfigParameterK = pDebug->lConfigParameterK;
			it.lConfigPlayerStorage = pDebug->lConfigPlayerStorage;
			it.lConfigSysStorage = pDebug->lConfigSysStorage;
			it.lConfigResetSection = pDebug->lConfigResetSection;

			it.curStatus = WaitForCtrl;
			pDebug = &it;
			bUpdate = true;
			break;
#else //ȡ��
			CancelControl(it.nId);
			break;
#endif
		}
	}

	bool bActive = true;
	for (auto& it : m_ctrllist)
	{
		if (it.curStatus == AlreadyInCtrl)
		{
			bActive = false;
			break;
		}
	}
	if (bActive)
	{
		pDebug->curStatus = AlreadyInCtrl;
	}
	pDebug->lWinRatio = CGameControl::CalcWinRatio(pDebug->lCurSysStorage, pDebug->lCurPlayerStorage, pDebug->lCurParameterK);
	//����֪ͨ
	m_pGameControl->SendUpdateEvent(EventUserCtrl, (void*)pDebug);

	if (!bUpdate)
		m_ctrllist.emplace_back(*pDebug);
};
//���һ����Ч����
BaseCtrlInfo* CDebugCrontol::GetValid()
{
	for (auto& it : m_ctrllist)
	{
		if (it.curStatus == AlreadyInCtrl || it.curStatus == WaitForCtrl)
			return static_cast<BaseCtrlInfo*>(&it);
	}

	return nullptr;
};
//������һ������
BaseCtrlInfo* CDebugCrontol::GetLast()
{
	if (m_ctrllist.empty())
		return nullptr;

	return (BaseCtrlInfo*)&m_ctrllist.back();
};
//��������״̬
void CDebugCrontol::CancelControl(int64_t nChooseIndx, bool bRemove)
{
	auto it = m_ctrllist.begin();
	for (; it != m_ctrllist.end(); ++it)
	{
		if ( (nChooseIndx < 0 && (it->curStatus == AlreadyInCtrl || it->curStatus == WaitForCtrl)) ||
			//(it->nId == nChooseIndx || it->nGameID == nChooseIndx)
			it->nGameID == nChooseIndx
			)
		{
			break;
		}
	}
	if (it == m_ctrllist.end())
		return;

	it->curStatus = CancelCtrl;
	//δ��Ч����Ĭ��ɾ��
	if (bRemove || (it->lSysCtrlSysWin == 0 && it->lSysCtrlPlayerWin == 0))
		it->curStatus = RemoveCtrl;
	//����֪ͨ
	m_pGameControl->SendUpdateEvent(EventUserCtrl, (void*)&(*it));

	if (RemoveCtrl == it->curStatus)
	{
		m_ctrllist.erase(it);
	}
};
//���¿��
void CDebugCrontol::UpdateStorage(SCOREEX lChangeValue)
{
	GameDebugInfo* pCtrl = static_cast<GameDebugInfo*>(GetValid());
	if (pCtrl == nullptr)
		return;

	//>0 ������ҿ�棬<0 ����ϵͳ���
	if (lChangeValue > 0)
	{
		pCtrl->lCurPlayerStorage -= lChangeValue;
		pCtrl->lSysCtrlPlayerWin += lChangeValue;
		m_lSysCtrlPlayerWin += lChangeValue;
	}
	else if (lChangeValue < 0)
	{
		pCtrl->lCurSysStorage += lChangeValue;
		pCtrl->lSysCtrlSysWin -= lChangeValue;
		m_lSysCtrlSysWin -= lChangeValue;
	}
	pCtrl->lWinRatio = CGameControl::CalcWinRatio(pCtrl->lCurSysStorage, pCtrl->lCurPlayerStorage, pCtrl->lCurParameterK);

	++pCtrl->nCurWorkCount;
	if( (pCtrl->lCurSysStorage <=0 && pCtrl->lCurPlayerStorage <= 0) || 
		pCtrl->lCurSysStorage <= -pCtrl->lConfigSysStorage*pCtrl->lCurResetSection / 100)
		Done();
	else
	{
		//����֪ͨ
		m_pGameControl->SendUpdateEvent(EventUserCtrl, (void*)pCtrl);
	}
};
//������һ������
void CDebugCrontol::ActiveNext()
{
	GameDebugInfo* pCtrl = static_cast<GameDebugInfo*>(GetValid());
	if (pCtrl != nullptr)
	{
		pCtrl->curStatus = AlreadyInCtrl;
		//����֪ͨ
		m_pGameControl->SendUpdateEvent(EventUserCtrl, (void*)pCtrl);
	}
	//֪ͨ������յ�ǰ����
	else
	{
		GameDebugInfo tmp;
		ZeroMemory(&tmp, sizeof(tmp));
		//����֪ͨ
		m_pGameControl->SendUpdateEvent(EventUserCtrl, (void*)&tmp);
	}
};
//ִ�����
void CDebugCrontol::Done()
{
	GameDebugInfo* pCtrl = static_cast<GameDebugInfo*>(GetValid());
	if (pCtrl == nullptr)
		return;

	pCtrl->curStatus = DoneCtrl;
	//����֪ͨ
	m_pGameControl->SendUpdateEvent(EventUserCtrl, (void*)pCtrl);

	//��һ�������Զ���ʼ
	ActiveNext();
};

BaseCtrlInfo* CDebugCrontol::GetValid(int32_t nGameID)
{
	for (auto& it : m_ctrllist)
	{
		if (it.nGameID == nGameID && (it.curStatus == AlreadyInCtrl || it.curStatus == WaitForCtrl))
			return static_cast<BaseCtrlInfo*>(&it);
	}
	return nullptr;
};
//
void CDebugCrontol::UpdateStorage(int32_t nGameID, SCOREEX lChangeValue)
{
	GameDebugInfo* pCtrl = static_cast<GameDebugInfo*>(GetValid(nGameID));
	ASSERT(pCtrl != nullptr);

	//>0 ������ҿ�棬<0 ����ϵͳ���
	if (lChangeValue > 0)
	{
		pCtrl->lCurPlayerStorage -= lChangeValue;
		pCtrl->lSysCtrlPlayerWin += lChangeValue;
		m_lSysCtrlPlayerWin += lChangeValue;
		m_pGameControl->m_gameStatisticsInfo.stArea.lSysCtrlPlayerWin = m_lSysCtrlPlayerWin;
	}
	else if (lChangeValue < 0)
	{
		pCtrl->lCurSysStorage += lChangeValue;
		pCtrl->lSysCtrlSysWin -= lChangeValue;
		m_lSysCtrlSysWin -= lChangeValue;
		m_pGameControl->m_gameStatisticsInfo.stArea.lSysCtrlSysWin = m_lSysCtrlSysWin;
	}

	++pCtrl->nCurWorkCount;
	pCtrl->lWinRatio = CGameControl::CalcWinRatio(pCtrl->lCurSysStorage, pCtrl->lCurPlayerStorage, pCtrl->lCurParameterK);

	if ((pCtrl->lCurSysStorage <= 0 && pCtrl->lCurPlayerStorage <= 0) ||
		pCtrl->lCurSysStorage <= -pCtrl->lConfigSysStorage*pCtrl->lCurResetSection/100)
	{
		pCtrl->curStatus = DoneCtrl;
		//����֪ͨ
		m_pGameControl->SendUpdateEvent(EventUserCtrl, (void*)pCtrl);

		//��һ�������Զ���ʼ
		ActiveNext();
	}
	else
	{
		//����֪ͨ
		m_pGameControl->SendUpdateEvent(EventUserCtrl, (void*)pCtrl);
	}
};

//���͸����¼�
void CDebugCrontol::SendUpdateEvent(void* pInfo)
{
	ControlUpdateEvent stEvent;
	stEvent.emEventType = EventUserCtrl;
	stEvent.nDataLen = sizeof(GameDebugInfo);
	stEvent.pData = (void*)(new GameDebugInfo());
	if (stEvent.pData == nullptr)
		return;
	memcpy(stEvent.pData, pInfo,static_cast<size_t>(stEvent.nDataLen));
	m_pGameControl->SendEvent(stEvent);
};
//ɾ�����������
bool CDebugCrontol::RemoveDone()
{
	for (auto& it : m_ctrllist)
	{
		if (it.curStatus == CancelCtrl || it.curStatus == DoneCtrl)
		{
			CancelControl(it.nGameID, true);
			return true;
		}
	}

	return false;
};
/////////////////////////////////////////////////////////////////////////
//������
//����һ������
void CStorageCrontol::Add(BaseCtrlInfo* pInfo)
{
	BaseCrontol::Add(pInfo);
	pInfo->nId = 0;
	if (!m_ctrllist.empty()) pInfo->nId = m_ctrllist.back().nId + 1;

	StorageInfo* pDebug = static_cast<StorageInfo*>(pInfo);

	bool bActive = true;
	for (auto& it : m_ctrllist)
	{
		if (it.curStatus == AlreadyInCtrl)
		{
			bActive = false;
			break;
		}
	}
	if (bActive)
	{
		pDebug->curStatus = AlreadyInCtrl;
	}

	++m_lStorageResetCount;
	//pDebug->lResetTimes = m_lStorageResetCount;
	pDebug->lWinRatio = CGameControl::CalcWinRatio(pDebug->lCurSysStorage, pDebug->lCurPlayerStorage, pDebug->lCurParameterK);

	//����֪ͨ
	m_pGameControl->SendUpdateEvent(this->m_ctrlType == SysCtrl ? EventSysCtrl : EventRoomCtrl, (void*)pDebug);

	m_ctrllist.emplace_back(*pDebug);
};
//���һ����Ч����
BaseCtrlInfo* CStorageCrontol::GetValid()
{
	for (auto& it : m_ctrllist)
	{
		if (it.curStatus == AlreadyInCtrl || it.curStatus == WaitForCtrl)
			return static_cast<BaseCtrlInfo*>(&it);
	}

	return nullptr;
};
//������һ������
BaseCtrlInfo* CStorageCrontol::GetLast()
{
	if (m_ctrllist.empty())
		return nullptr;

	return (BaseCtrlInfo*)&m_ctrllist.back();
};
//��������״̬
void CStorageCrontol::CancelControl(int64_t nChooseIndx, bool bRemove)
{
	auto it = m_ctrllist.begin();
	for (; it != m_ctrllist.end(); ++it)
	{
		if ((nChooseIndx < 0 && it->curStatus == AlreadyInCtrl) ||
			it->nId == nChooseIndx
			)
		{
			break;
		}
	}
	if (it == m_ctrllist.end())
		return;

	it->curStatus = CancelCtrl;
	if (bRemove || (it->lSysCtrlSysWin == 0 && it->lSysCtrlPlayerWin == 0))
		it->curStatus = RemoveCtrl;

	//����֪ͨ
	m_pGameControl->SendUpdateEvent(this->m_ctrlType == SysCtrl ? EventSysCtrl : EventRoomCtrl, (void*)&(*it));

	//δ��Ч����Ĭ��ɾ��
	if (it->curStatus == RemoveCtrl)
	{
		m_ctrllist.erase(it);
	}
};
//���¿��
void CStorageCrontol::UpdateStorage(SCOREEX lChangeValue)
{
	StorageInfo* pCtrl = static_cast<StorageInfo*>(GetValid());
	if (pCtrl == nullptr)
		return;

	//>0 ������ҿ�棬<0 ����ϵͳ���
	if (lChangeValue > 0)
	{
		pCtrl->lCurPlayerStorage -= lChangeValue;
		pCtrl->lSysCtrlPlayerWin += lChangeValue;
		m_lSysCtrlPlayerWin += lChangeValue;
		if (m_ctrlType == SysCtrl)	m_pGameControl->m_gameStatisticsInfo.stSystem.lSysCtrlPlayerWin = m_lSysCtrlPlayerWin;
		else m_pGameControl->m_gameStatisticsInfo.stRoom.lSysCtrlPlayerWin = m_lSysCtrlPlayerWin;
	}
	else if (lChangeValue < 0)
	{
		pCtrl->lCurSysStorage += lChangeValue;
		pCtrl->lSysCtrlSysWin -= lChangeValue;
		m_lSysCtrlSysWin -= lChangeValue;
		if (m_ctrlType == SysCtrl)	m_pGameControl->m_gameStatisticsInfo.stSystem.lSysCtrlSysWin = m_lSysCtrlSysWin;
		else m_pGameControl->m_gameStatisticsInfo.stRoom.lSysCtrlSysWin = m_lSysCtrlSysWin;
	}
	pCtrl->lWinRatio = CGameControl::CalcWinRatio(pCtrl->lCurSysStorage, pCtrl->lCurPlayerStorage, pCtrl->lCurParameterK);
	//ϵͳ����ҿ��Ϊ0ʱ��ʾ���̴������
	if ((pCtrl->lCurSysStorage <= 0 && pCtrl->lCurPlayerStorage <= 0) || 
		pCtrl->lCurSysStorage <= -pCtrl->lConfigSysStorage*pCtrl->lCurResetSection / 100)
	{
		Done();
	}
	else
	{
		//����֪ͨ
		m_pGameControl->SendUpdateEvent(this->m_ctrlType == SysCtrl ? EventSysCtrl : EventRoomCtrl, (void*)pCtrl);
	}
};
//������һ������
void CStorageCrontol::ActiveNext()
{
	StorageInfo* pCtrl = static_cast<StorageInfo*>(GetValid());
	if (pCtrl != nullptr)
	{
		pCtrl->curStatus = AlreadyInCtrl;
		//����֪ͨ
		m_pGameControl->SendUpdateEvent(this->m_ctrlType == SysCtrl ? EventSysCtrl : EventRoomCtrl, (void*)pCtrl);
	}
	else if (m_ctrlType == SysCtrl)
	{
		StorageInfo* plast = (StorageInfo*)GetLast();
		if (plast != nullptr)
		{
			++m_lStorageResetCount;
			StorageInfo& newInfo = *plast;
			//����
			newInfo.lCurResetSection = newInfo.lConfigResetSection;
			newInfo.lCurParameterK = newInfo.lConfigParameterK;
			newInfo.lCurSysStorage += newInfo.lConfigSysStorage;
			newInfo.lCurPlayerStorage += newInfo.lConfigPlayerStorage;
			newInfo.lUpdateTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			newInfo.lWinRatio = CGameControl::CalcWinRatio(newInfo.lCurSysStorage, newInfo.lCurPlayerStorage, newInfo.lCurParameterK);
			newInfo.lResetTimes += 1;
			newInfo.curStatus = AlreadyInCtrl;
			//m_ctrllist.emplace_back(newInfo);

			//����֪ͨ
			m_pGameControl->SendUpdateEvent(this->m_ctrlType == SysCtrl ? EventSysCtrl : EventRoomCtrl, (void*)&newInfo);
		}
	}
	//֪ͨ������յ�ǰ����
	else
	{
		StorageInfo tmp;
		ZeroMemory(&tmp, sizeof(tmp));
		//����֪ͨ
		m_pGameControl->SendUpdateEvent(this->m_ctrlType == SysCtrl ? EventSysCtrl : EventRoomCtrl, (void*)&tmp);
	}
};
//ִ�����
void CStorageCrontol::Done()
{
	StorageInfo* pCtrl = static_cast<StorageInfo*>(GetValid());
	if (pCtrl == nullptr)
		return;

	pCtrl->curStatus = DoneCtrl;

	//����֪ͨ
	m_pGameControl->SendUpdateEvent(this->m_ctrlType == SysCtrl ? EventSysCtrl : EventRoomCtrl, (void*)pCtrl);

	//��һ�������Զ���ʼ
	ActiveNext();
};

//���͸����¼�
void CStorageCrontol::SendUpdateEvent(void* pInfo)
{
	ControlUpdateEvent stEvent;
	stEvent.emEventType = this->m_ctrlType == SysCtrl ? EventSysCtrl : EventRoomCtrl;
	stEvent.nDataLen = sizeof(StorageInfo);
	stEvent.pData = (void*)(new StorageInfo());
	if (stEvent.pData == nullptr)
		return;
	memcpy(stEvent.pData, pInfo, static_cast<size_t>(stEvent.nDataLen));
	m_pGameControl->SendEvent(stEvent);
};
//ɾ�����������
bool CStorageCrontol::RemoveDone()
{
	for (auto& it : m_ctrllist)
	{
		if (it.curStatus == CancelCtrl || it.curStatus == DoneCtrl)
		{
			CancelControl(it.nId, true);
			return true;
		}
	}

	return false;
};
//////////////////////////////////////////////////////////////////////////
//Ȩ������
void CGameWeightControl::SendUpdateEvent()
{
	WeightConfig weightConfig[WEIGHT_CONFIG_MAX_SZIE];
	ZeroMemory(weightConfig, sizeof(weightConfig));

	int nCurIndex = 0;
	for (auto& it : m_weightConfig)
	{
		weightConfig[nCurIndex] = it;
		weightConfig[nCurIndex].lRatio = CalcRatio(it.lWeight);
		if (++nCurIndex == WEIGHT_CONFIG_MAX_SZIE)
			break;
	}
	assert(nCurIndex == WEIGHT_CONFIG_MAX_SZIE);
	m_pGameControl->SendUpdateEvent(EventGameWeightConfig, (void*)weightConfig);
}

//////////////////////////////////////////////////////////////////////////
//
CGameControl::CGameControl()
{
	m_ctrlTypes.clear();
	srand(static_cast<unsigned int>(time(0)));

	memset((void*)&m_gameStatisticsInfo, 0, sizeof(m_gameStatisticsInfo));
	m_gameStatisticsInfo.lStartTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	m_gameWieght.m_pGameControl = this;
}


CGameControl::~CGameControl()
{
	for (auto& it : m_ctrlTypes)
	{
		if (it.second != nullptr)
		{
			delete it.second;
			it.second = nullptr;
		}
	}

	m_ctrlTypes.clear();
}

//��ȡ��ǰ��Ч����
BaseCrontol* CGameControl::GetCurValidControl(int32_t nGameID /*= -1*/)
{
	//���ȼ��� ���� >  ���� > ϵͳ
	auto it = m_ctrlTypes.find(UserCtrl);
	if (nGameID > 0 && it != m_ctrlTypes.end())
	{
		CDebugCrontol* pUserCtrl = (CDebugCrontol*)it->second;
		if (pUserCtrl->GetValid(nGameID) != nullptr)
			return dynamic_cast<BaseCrontol*>(it->second);
	}

	it = m_ctrlTypes.find(RoomCtrl);
	if (it != m_ctrlTypes.end())
	{
		if (it->second->GetValid() != nullptr)
			return dynamic_cast<BaseCrontol*>(it->second);
	}

	it = m_ctrlTypes.find(SysCtrl);
	if (it != m_ctrlTypes.end())
	{
		if (it->second->GetValid() != nullptr)
			return dynamic_cast<BaseCrontol*>(it->second);
	}

	return nullptr;
}
//��ȡϵͳ��ǰϵͳ���ƿ����Ϣ
BaseCrontol* CGameControl::GetCurTypeCtrl(ControlType type)
{
	auto it = m_ctrlTypes.find(type);
	if (it == m_ctrlTypes.end())
		return nullptr;
	
	return it->second;
};

//����һ����������
void CGameControl::AddControl(ControlType type, GameDebugInfo& info)
{
	auto it = m_ctrlTypes.find(type);
	if (it == m_ctrlTypes.end())
	{
		m_ctrlTypes[type] = nullptr;
		m_ctrlTypes[type] = new CDebugCrontol(type);
		m_ctrlTypes[type]->m_pGameControl = this;
	}
	if (info.curStatus == CancelCtrl || info.curStatus == RemoveCtrl)
	{
		CancelControl(type, info.nGameID, info.curStatus == RemoveCtrl ? true : false);
		return;
	}
	assert(m_ctrlTypes[type] != nullptr);
	m_ctrlTypes[type]->Add((BaseCtrlInfo*)&info);
}
void CGameControl::AddControl(ControlType type, StorageInfo& info)
{
	auto it = m_ctrlTypes.find(type);
	if (it == m_ctrlTypes.end())
	{
		m_ctrlTypes[type] = nullptr;
		m_ctrlTypes[type] = new CStorageCrontol(type);
		m_ctrlTypes[type]->m_pGameControl = this;
	}
	if (info.curStatus == CancelCtrl)
	{
		CancelControl(type, static_cast<int32_t>(info.nId));
		return;
	}
	//��ҿ�桢ϵͳ��棬��ͬʱֻ��һ����Ч
	else
	{
		m_ctrlTypes[type]->CancelControl(-1);
	}

	assert(m_ctrlTypes[type] != nullptr);
	m_ctrlTypes[type]->Add((BaseCtrlInfo*)&info);
}
void CGameControl::AddControl(ControlType type, GameTaxInfo& info)
{
	m_gameTax.lTaxRatio = info.lTaxRatio;
	SendUpdateEvent(EventGameTax, &m_gameTax);
};
void CGameControl::AddControl(ControlType type, GameRewardInfo& info)
{
	//m_gameReward.lCurStorage = m_gameReward.lConfigStorage = info.lConfigStorage;
	m_gameReward.lDispatchStorage = info.lDispatchStorage;
	m_gameReward.lDispatchRatio = info.lDispatchRatio;
	m_gameReward.lTaxRatio = info.lTaxRatio;
	m_gameReward.lVirtualStorage = info.lVirtualStorage;
	SendUpdateEvent(EventGameReward, &m_gameReward);
};
void CGameControl::AddControl(ControlType type, GameExtraInfo& info)
{
	m_gameExtraInfo.lFreeGameRatio = info.lFreeGameRatio;
	m_gameExtraInfo.lExtraGameRatio = info.lExtraGameRatio;
	SendUpdateEvent(EventGameExtra, &m_gameExtraInfo);
};
void CGameControl::AddControl(ControlType type, WeightConfig* pInfo, int nSize)
{
	for (int i = 0; i < nSize; ++i)
	{
		m_gameWieght.Update(static_cast<uint32_t>((pInfo + i)->lIndex), (pInfo + i)->lWeight);
	}
	m_gameWieght.SendUpdateEvent();
};
//ȡ��ĳһ����������
void CGameControl::CancelControl(ControlType type, int32_t index, bool bRemove)
{
	auto it = m_ctrlTypes.find(type);
	if (it == m_ctrlTypes.end())
		return ;

	//��ʾɾ�����з�ִ��̬
	if (index < 0 && bRemove)
	{
		while (it->second->RemoveDone());

		return;
	}
	else 
		it->second->CancelControl(index, bRemove);

	it->second->ActiveNext();
};

//���������Ӯ�����¿��
void CGameControl::UpdateControlStorage(SCOREEX lUserWinLose, int32_t nGameID /*= -1*/)
{
	//�����ӿ��
	BaseCrontol* pCtrl = GetCurValidControl(nGameID);
	if (pCtrl != nullptr)
	{
		if (pCtrl->m_ctrlType != UserCtrl)
			pCtrl->UpdateStorage(lUserWinLose);
		else
		{
			CDebugCrontol* pDebug = (CDebugCrontol*)pCtrl;
			pDebug->UpdateStorage(nGameID,lUserWinLose);
		}
	}

	//��������Ӯ ϵͳ����Ӯ = ������ϵͳ����Ӯ + ��ע��ˮ��� + �ʽ��� 
	m_gameStatisticsInfo.lSysCtrlPlayerWin += lUserWinLose;
	m_gameStatisticsInfo.lSysCtrlSysWin -= lUserWinLose;

	//����֪ͨ
	StatisticsInfo tmp = m_gameStatisticsInfo;
	tmp.lSysCtrlSysWin += m_gameTax.lCurStorage + m_gameReward.lCurStorage;
	SendUpdateEvent(EventGameStatistics, (void*)&tmp);
	SendUpdateEvent(EventGameTax, (void*)&m_gameTax);
	SendUpdateEvent(EventGameReward, (void*)&m_gameReward);
	SendUpdateEvent(EventGameExtra, (void*)&m_gameExtraInfo);
};
//��������ֵ
void CGameControl::AddStorage(ControlEventType type, SCOREEX dValue)
{
	switch (type)
	{
	case EventGameTax:
		m_gameTax.lCurStorage += dValue;
		SendUpdateEvent(type, (void*)&m_gameTax);
		break;
	case EventGameReward:
		m_gameReward.lCurStorage += dValue;
		SendUpdateEvent(type, (void*)&m_gameReward);
		break;
	case EventSysCtrl:
	case EventRoomCtrl:
	case EventUserCtrl:
	case EventGameExtra:
	case EventGameStatistics:
	case EventGameWeightConfig:
	default:
		return;
	}
};
//���͸����¼�
void CGameControl::SendUpdateEvent(ControlEventType type, void* pInfo)
{
	ControlUpdateEvent stEvent;
	stEvent.emEventType = type;
	switch (type)
	{
	case EventGameTax:
		stEvent.nDataLen = sizeof(GameTaxInfo);
		break;
	case EventGameReward:
		stEvent.nDataLen = sizeof(GameRewardInfo);
		break;
	case EventGameExtra:
		stEvent.nDataLen = sizeof(GameExtraInfo);
		break;
	case EventGameStatistics:
		stEvent.nDataLen = sizeof(StatisticsInfo);
		break;
	case EventGameWeightConfig:
		stEvent.nDataLen = sizeof(WeightConfig)*WEIGHT_CONFIG_MAX_SZIE;
		break;
	case EventSysCtrl:
	case EventRoomCtrl:
		stEvent.nDataLen = sizeof(StorageInfo);
		break;
	case EventUserCtrl:
		stEvent.nDataLen = sizeof(GameDebugInfo);
		break;
	default:
		assert(false);
		return;
	}

	stEvent.pData = (void*)(new char[static_cast<uint32_t>(stEvent.nDataLen)]);
	if (stEvent.pData == nullptr)
	{
		assert(false);
		return;
	}

	memcpy(stEvent.pData, pInfo, static_cast<size_t>(stEvent.nDataLen));
	SendEvent(stEvent);
};

//�¼�
void CGameControl::SendEvent(ControlUpdateEvent& event)
{
	m_eventUpdate.push(event);
};
//�����¼���֪ͨ���Զ�
void CGameControl::RefreshEvent(ITableFrame* pTable, IServerUserItem *pIServerUserItem)
{
	assert(pTable != nullptr);
	if (pTable == nullptr)
		return;

	while (!m_eventUpdate.empty())
	{
		ControlUpdateEvent& stEvent = m_eventUpdate.front();
		//�������ݳ���
		if (sizeof(ControlUpdateEvent)+stEvent.nDataLen > MAX_EVENT_SEND_BUFFER)
		{
			assert(false);
			m_eventUpdate.pop();
			continue;
		}
		//����ͷ
		memcpy(m_eventSendBuffer, (void*)&stEvent, sizeof(ControlUpdateEvent));
		//������ݲ���
		memcpy(m_eventSendBuffer + sizeof(ControlUpdateEvent), stEvent.pData, static_cast<uint32_t>(stEvent.nDataLen));
		//����
		pTable->SendRoomData(pIServerUserItem, SUB_S_EVENT_UPDATE, (void*)m_eventSendBuffer, static_cast<WORD>(sizeof(ControlUpdateEvent)+stEvent.nDataLen));

		//�ͷ��ڴ�
		delete stEvent.pData;
		m_eventUpdate.pop();
	}
};

//
void CGameControl::RefreshAll(ITableFrame* pTable, IServerUserItem *pIServerUserItem)
{
	SendUpdateEvent(EventGameTax,(void*)&m_gameTax);
	SendUpdateEvent(EventGameReward, (void*)&m_gameReward);
	SendUpdateEvent(EventGameExtra, (void*)&m_gameExtraInfo);
	//
	StatisticsInfo tmp = m_gameStatisticsInfo;
	tmp.lSysCtrlSysWin += m_gameTax.lCurStorage + m_gameReward.lCurStorage;
	SendUpdateEvent(EventGameStatistics, (void*)&tmp);
	//�б�����
	for (auto& it : m_ctrlTypes)
	{
		switch (it.second->m_ctrlType)
		{
		case SysCtrl:
		case RoomCtrl:
		{
						 CStorageCrontol* pControl = (CStorageCrontol*)it.second;
						 for (auto& pItem : pControl->m_ctrllist)
						 {
							 SendUpdateEvent(pControl->m_ctrlType == SysCtrl ? EventSysCtrl : EventRoomCtrl, &pItem);
						 }
		}break;
		case UserCtrl:
		{
						 CStorageCrontol* pControl = (CStorageCrontol*)it.second;
						 for (auto& pItem : pControl->m_ctrllist)
						 {
							 SendUpdateEvent(EventUserCtrl, &pItem);
						 }
		}break;
		default:
			break;
		}
	}
	//Ȩ������
	m_gameWieght.SendUpdateEvent();

	//����
	RefreshEvent(pTable, pIServerUserItem);
};

//��ȡ�����Ӯ����
int32_t CGameControl::GetUserWinLoseRatio(int32_t nGameID /*= -1*/)
{
	int32_t nWinRatio = 0;

	BaseCrontol* pCurCtrl = GetCurValidControl(nGameID );
	if (pCurCtrl == nullptr)
	{
		//�����
		nWinRatio = rand()%100;
		return nWinRatio;
	}
	if (pCurCtrl->m_ctrlType == UserCtrl)
	{
		CDebugCrontol* pCtrl = (CDebugCrontol*)pCurCtrl;
		GameDebugInfo* pDebug = (GameDebugInfo*)pCtrl->GetValid(nGameID);
		//���ݿ�����
		return CalcWinRatio(pDebug->lCurSysStorage, pDebug->lCurPlayerStorage, pDebug->lCurParameterK);
	}
	else
	{
		StorageInfo* pDebug = (StorageInfo*)pCurCtrl->GetValid();
		//���ݿ�����
		return CalcWinRatio(pDebug->lCurSysStorage, pDebug->lCurPlayerStorage, pDebug->lCurParameterK);
	}
};

//����ʤ��
int32_t CGameControl::CalcWinRatio(SCOREEX lCurSysStorage, SCOREEX lCurPlayerStorage, int64_t lCurParameteK)
{
	int32_t nWinRatio = 0;
	SCOREEX lDval = abs(lCurSysStorage - lCurPlayerStorage);
	SCOREEX lMax = max(lCurPlayerStorage, lCurSysStorage);
	int64_t lParameterK = lCurParameteK;

	bool bSysWin = lCurSysStorage > lCurPlayerStorage;
	if (lDval == lMax)
	{
		nWinRatio = lParameterK > 0 && lCurPlayerStorage == 0 ? 0 : 70;
	}
	else if (lDval <= lMax * lCurParameteK / 100)
	{
		nWinRatio = bSysWin ? 50 : 20;
	}
	else if (lDval > lMax * lParameterK / 100 && lDval <= 1.5 * lMax * lParameterK / 100)//1.5
	{
		nWinRatio = bSysWin ? 40 : 30;
	}
	else if (lDval > lMax * lParameterK / 100 && lDval <= 2 * lMax * lParameterK / 100)//2
	{
		nWinRatio = bSysWin ? 30 : 40;
	}
	else if (lDval > lMax * lParameterK / 100 && lDval <= 3 * lMax * lParameterK / 100)//3
	{
		nWinRatio = bSysWin ? 20 : 50;
	}
	else
	{
		nWinRatio = bSysWin ? 0 : 70;
	}

	return nWinRatio;
};

//��õ�ǰ��ҿ��
SCOREEX CGameControl::GetCurPlayerStorage(int32_t nGameID)
{
	BaseCrontol* pCurCtrl = GetCurValidControl(nGameID);
	if (pCurCtrl == nullptr)
	{
		ASSERT(false);
		return 0;
	}

	if (pCurCtrl->m_ctrlType == UserCtrl)
	{
		GameDebugInfo* pInfo = (GameDebugInfo*)pCurCtrl->GetValid();
		return pInfo->lCurPlayerStorage;
	}

	StorageInfo* pInfo = (StorageInfo*)pCurCtrl->GetValid();
	return pInfo->lCurPlayerStorage;
};

//��õ�ǰ���
void CGameControl::GetCurStorage(SCOREEX& dSysStorage, SCOREEX& dUserStorage, int32_t nGameID )
{
	BaseCrontol* pCurCtrl = GetCurValidControl(nGameID);
	if (pCurCtrl == nullptr)
	{
		ASSERT(false);
		return ;
	}

	if (pCurCtrl->m_ctrlType == UserCtrl)
	{
		GameDebugInfo* pInfo = (GameDebugInfo*)pCurCtrl->GetValid();
		dSysStorage = pInfo->lCurSysStorage;
		dUserStorage = pInfo->lCurPlayerStorage;
		return ;
	}

	StorageInfo* pInfo = (StorageInfo*)pCurCtrl->GetValid();
	dSysStorage = pInfo->lCurSysStorage;
	dUserStorage = pInfo->lCurPlayerStorage;
};

//������Ϸ������
void CGameControl::LineGameUpdateStorage(SCOREEX dUserBet, SCOREEX dUserWinScore,int32_t nGameID)
{
	//��ע��ˮ
	SCOREEX dBetTxChange = (dUserBet*m_gameTax.lTaxRatio) / 100;
	m_gameTax.lCurStorage += dBetTxChange;
	//�ʽ��ˮ������ע��
	SCOREEX dRewardChange = (dUserBet*m_gameReward.lTaxRatio) / 100;
	m_gameReward.lCurStorage += dRewardChange;

	SCOREEX dStorageChange = 0;
	//����䣬��ϵͳ���
	if (dUserBet > dUserWinScore)
	{
		// = ϵͳӮǮ - ��ˮ - �ʽ��ˮ
		dStorageChange = -(dUserWinScore - dUserBet) - dBetTxChange - dRewardChange;
		dStorageChange > 0 ? dStorageChange = -dStorageChange : 0;
	}
	//���Ӯ������ҿ��
	else if (dUserBet < dUserWinScore)
	{
		// = ���ӮǮ + ��ˮ + �ʽ��ˮ
		dStorageChange = (dUserWinScore - dUserBet) + dBetTxChange + dRewardChange;
		dStorageChange > 0 ? 0 : dStorageChange = -dStorageChange;
	}
	//��Ҳ��䲻Ӯ����ϵͳ���
	else
	{
		dStorageChange = dBetTxChange + dRewardChange;
		dStorageChange > 0 ? dStorageChange = -dStorageChange : 0;
	}

	//dStorageChange > 0��ʾ����ҿ�棬<0��ʾ��ϵͳ���
	UpdateControlStorage(dStorageChange, nGameID);
};