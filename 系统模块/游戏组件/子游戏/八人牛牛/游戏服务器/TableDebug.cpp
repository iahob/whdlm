#include "stdafx.h"
#include "TableDebug.h"
#include "CLog.h"

CTableDebug::CTableDebug()
{
	memset(m_bDebugDoneChair, 0, sizeof(m_bDebugDoneChair));
}


CTableDebug::~CTableDebug()
{
}

void CTableDebug::RefreshCardPool(BYTE* pPool, BYTE cbPoolSize)
{
	GetCardHelper().RefreshPattern(pPool, cbPoolSize);
};
void CTableDebug::AddConfig(stConfigDebug& stConfig)
{
	RemoveConfig(&stConfig);

	m_Configs.emplace_back(stServerConfig(stConfig));
};
void CTableDebug::RemoveConfig(stConfigDebug* stConfig)
{
	if (m_Configs.empty())
		return;

	if (stConfig == nullptr)
		stConfig = &(m_Configs.back().stConfig);

	for (auto it = m_Configs.begin(); it != m_Configs.end(); ++it)
	{
		if (it->stConfig.wDebugUserChair == stConfig->wDebugUserChair)
		{
			m_Configs.erase(it);
			break;
		}
	}
};
stServerConfig* CTableDebug::GetUserConfig(WORD wDebugChair)
{
	for (auto& it : m_Configs)
	{
		if (it.stConfig.wDebugUserChair == wDebugChair)
			return &it;
	}

	return nullptr;
}; 
void CTableDebug::GetCurValidDebugType(bool bDebugCardType[MAX_CARD_TYPE])
{
	for (int i = EM_DEBUG_CARD_OX_0; i < EM_DEBUG_CARD_MAX; ++i)
		bDebugCardType[i] = GetCardHelper().HaveType((emUserDebugType)i);
};

bool CTableDebug::FindMatchResult(CheckCondition pCheck)
{
	if (pCheck == nullptr)
	{
		ASSERT(false);
		return true;
	}

	BYTE* pUserCards[GAME_PLAYER] = { 0 };

	int nRandChair[GAME_PLAYER] = { 0 };
	int nSrcChair[GAME_PLAYER] = { 0 };
	int nRandCount = 0;
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		if (m_UserDebugs[i].Valid())
			nSrcChair[nRandCount++] = i;
	
		//
		pUserCards[i] = m_UserDebugs[i].GetCards();
	}

	if (nRandCount == 0)
		return true;


	memcpy(nRandChair, nSrcChair, sizeof(nSrcChair));
	//
	do{
		//判断是否满足结果
		if (!pCheck(pUserCards))
			continue;

		//换牌
		for (int i = 0; i < nRandCount; ++i)
		{
			if (nRandChair[i] <= i)
				continue;

			int nLeftChair = nSrcChair[i];
			int nRightChair = nRandChair[i];
			if (!(m_UserDebugs[nLeftChair].CanSwitchCard() && m_UserDebugs[nRightChair].CanSwitchCard()))
				continue;

			m_UserDebugs[nLeftChair].ExchangeCard(&m_UserDebugs[nRightChair]);
		}

		return true;

	} while (next_permutation(nRandChair, nRandChair + nRandCount));

	return false;
}
void CTableDebug::OnEnd()
{
	
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		m_UserDebugs[i].Reset();
	}

	if (m_Configs.empty())
		return;

	if (!m_bDebugDoneChair[m_Configs.back().stConfig.wDebugUserChair])
		return;

	while (true)
	{
		bool bContinue = false;
		for (auto& it : m_Configs)
		{
			if (it.stConfig.cbType == 2)
			{
				LOG_DEBUG(L"自动取消调试，配置玩家椅子:", m_Configs.back().stConfig.wConfigUserChair);
				RemoveConfig(nullptr);
				bContinue = true;
				break;
			}
			if (m_Configs.back().stConfig.cbGameCount > 0)
			{
				++m_Configs.back().nCurDebugCount;
			}
			if (m_Configs.back().nCurDebugCount < m_Configs.back().stConfig.cbGameCount)
				continue;

			LOG_DEBUG(L"自动取消调试，配置玩家椅子:", m_Configs.back().stConfig.wConfigUserChair);
			RemoveConfig(nullptr);
			bContinue = true;
			break;
		}
		if (!bContinue)
			break;
	}
}