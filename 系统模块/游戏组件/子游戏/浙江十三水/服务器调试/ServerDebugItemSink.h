#pragma once
#include "../��Ϸ������/ServerDebug.h"
#include "../��Ϸ������/GameLogic.h"

#pragma warning(disable : 4244)

class CServerDebugItemSink : public IServerDebug
{

	CGameLogic						m_GameLogic;							//��Ϸ�߼�

public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//���ؿ�������
	virtual bool __cdecl DebugResult(BYTE cbDebugCardData[GAME_PLAYER][HAND_CARD_COUNT], ROOMUSERDEBUG Keyroomuserdebug,BYTE cbMaCard,WORD wBanker);

};
