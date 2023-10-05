#ifndef __H_TABLE_DEBUG_H__
#define __H_TABLE_DEBUG_H__
#include "Stdafx.h"
#include<algorithm>
#include <functional>
#include "CardHelper.h"
#include <list>

class CTableDebug;
class CUserDebug
{
	friend CTableDebug;

protected:
	BYTE						m_cbCards[MAX_CARDCOUNT];
	BYTE						m_cbCurCount;
	bool						m_bLock;
	BYTE						m_cbExchangeCount;

public:
	CUserDebug(){Reset();};
	~CUserDebug(){};

public:
	void Lock(){ m_bLock = true; }
	void Reset()
	{
		memset(m_cbCards, 0, sizeof(m_cbCurCount));
		m_cbCurCount = 0;
		m_bLock = false;
		m_cbExchangeCount = 0;
	}
	bool CanSwitchCard()
	{
		if (m_bLock) return false;
		if (!Valid()) return false;
		return true;
	}
	bool Valid()
	{ 
		if (m_cbCurCount == 0) return false;
		return true; 
	}
	BYTE* GetCards(){return m_cbCards;}
	BYTE GetCount(){ return m_cbCurCount; }

public:
	//InitCards  初始手牌(重置锁定状态)
	void Init(BYTE * pCards, BYTE cbCount)
	{
		Reset();
		SetCards(pCards,cbCount);
		m_cbExchangeCount = cbCount == MAX_CARDCOUNT ? MAX_CARDCOUNT : 1;
	}
	void AddCard(BYTE cbCard)
	{
		if (m_cbCurCount >= MAX_CARDCOUNT)
		{
			//ASSERT(false);
			return;
		}
		m_cbCards[m_cbCurCount++] = cbCard;
	};
	//配置手牌
	void SetCards(BYTE * pCards, BYTE cbCount)
	{
		if (cbCount > MAX_CARDCOUNT)
		{
			ASSERT(false);
			return;
		}
		m_cbCurCount = cbCount;
		memcpy(m_cbCards, pCards, m_cbCurCount*sizeof(m_cbCards[0]));
	};
	void SetCurCount(BYTE cbCurCount)
	{
		if (cbCurCount > MAX_CARDCOUNT)
		{
			ASSERT(false);
			return;
		}
		m_cbCurCount = cbCurCount;
	};
	//ChangeCard 变更某张手牌
	BYTE ChangeCard(BYTE cbCard, BYTE cbOldCard)
	{
		for (int i = 0; i < m_cbCurCount; ++i)
		{
			if (m_cbCards[i] == cbOldCard)
			{
				return ChangeCardByIndex(cbCard, i);
				
			}
		}
		return INVALID_BYTE;
	}
	//ChangeCard 变更某张手牌
	BYTE ChangeCardByIndex(BYTE cbCard, BYTE cbIndex)
	{
		if (cbIndex >= MAX_CARDCOUNT)
		{
			ASSERT(false);
			return INVALID_BYTE;
		}

		m_bLock = true;
		BYTE cbOldCard = m_cbCards[cbIndex];
		m_cbCards[cbIndex] = cbCard;
		return cbOldCard;
	}
	//Exchange     交换手牌(不修改、不判断锁定状态)
	void ExchangeCard(CUserDebug* pRight)
	{
		ASSERT(m_cbCurCount == pRight->m_cbCurCount);
		if (m_cbCurCount != pRight->m_cbCurCount)
			return;

		BYTE cbTmpCards[MAX_CARDCOUNT];
		if (m_cbExchangeCount == 1)
		{
			cbTmpCards[0] = m_cbCards[MAX_CARDCOUNT - 1];
			m_cbCards[MAX_CARDCOUNT - 1] = pRight->m_cbCards[MAX_CARDCOUNT - 1];
			pRight->m_cbCards[MAX_CARDCOUNT - 1] = cbTmpCards[0];
		}
		else
		{
			memcpy(cbTmpCards, m_cbCards, sizeof(cbTmpCards));
			memcpy(m_cbCards, pRight->m_cbCards, sizeof(m_cbCards));
			memcpy(pRight->m_cbCards, cbTmpCards, sizeof(cbTmpCards));
		}
	}
};

struct stServerConfig
{
	stConfigDebug	stConfig;
	int				nCurDebugCount;
	int				nCurConfigCardCount;
	stServerConfig()
	{
		ZeroMemory(&stConfig, sizeof(stConfig));
		nCurDebugCount = 0;
		nCurConfigCardCount = 0;
	}
	stServerConfig(stConfigDebug& config)
	{
		memcpy(&stConfig, &config, sizeof(stConfig));
		nCurDebugCount = 0;
		nCurConfigCardCount = 0;
		for (int i = 0; i < MAX_CONFIG_CARD_COUNT; ++i)
		{
			if (COLOR(config.cbGameCards[i]) < 0 || COLOR(config.cbGameCards[i]) > 0x4)
				continue;
			if (VALUE(config.cbGameCards[i]) < 1 || VALUE(config.cbGameCards[i]) > 0xF)
				continue;
			if (config.cbGameCards[i] != 0)
				stConfig.cbGameCards[nCurConfigCardCount++] = config.cbGameCards[i];
		}
	}
};
class CTableDebug
{
	CCardHelper						m_CardHelper;
	CUserDebug						m_UserDebugs[GAME_PLAYER];
	std::list<stServerConfig>		m_Configs;

public:
	CTableDebug();
	virtual ~CTableDebug();

public:
	void RefreshCardPool(BYTE* pPool, BYTE cbPoolSize);
	CCardHelper& GetCardHelper(){ 
		return m_CardHelper; 
	}
	bool							m_bDebugDoneChair[GAME_PLAYER];

public:


public:
	//GetUserDebug		获取玩家调试对象
	CUserDebug* GetDebugUser(WORD wChair)
	{
		if (wChair < 0 || wChair >= GAME_PLAYER)
		{
			//ASSERT(false);
			return nullptr;
		}

		return &m_UserDebugs[wChair];
	}
	void AddConfig(stConfigDebug& stConfig);
	void RemoveConfig(stConfigDebug* stConfig = nullptr);
	stServerConfig* GetUserConfig(WORD wDebugChair);
	void GetCurValidDebugType(bool bDebugCardType[MAX_CARD_TYPE]);
	void OnEnd();
	
public:
	//FindMatchResult   
	typedef std::function<bool(BYTE* pUserCards[GAME_PLAYER])>	CheckCondition;
	bool FindMatchResult(CheckCondition pCheck);

};

#endif