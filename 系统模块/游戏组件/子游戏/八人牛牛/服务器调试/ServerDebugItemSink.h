#pragma once
#include "../��Ϸ������/ServerDebug.h"
#include "GameLogic.h"

class CServerDebugItemSink : public IServerDebug
{

	CGameLogic						m_GameLogic;							//��Ϸ�߼�

public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//���ص�������
	virtual bool __cdecl DebugResult(BYTE cbDebugCardData[GAME_PLAYER][MAX_CARDCOUNT], WORD wCtrlChairID, bool bWin, SENDCARDTYPE_CONFIG stConfig, CARDTYPE_CONFIG ctConfig, KING_CONFIG gtConfig, WORD	wPlayerCount);

};
