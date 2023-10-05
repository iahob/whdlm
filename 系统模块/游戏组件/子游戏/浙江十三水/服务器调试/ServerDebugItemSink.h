#pragma once
#include "../游戏服务器/ServerDebug.h"
#include "../游戏服务器/GameLogic.h"

#pragma warning(disable : 4244)

class CServerDebugItemSink : public IServerDebug
{

	CGameLogic						m_GameLogic;							//游戏逻辑

public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//返回控制区域
	virtual bool __cdecl DebugResult(BYTE cbDebugCardData[GAME_PLAYER][HAND_CARD_COUNT], ROOMUSERDEBUG Keyroomuserdebug,BYTE cbMaCard,WORD wBanker);

};
