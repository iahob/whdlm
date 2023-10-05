#pragma once
#include "../服务器组件/ServerDebug.h"
#include "../服务器组件/GameLogic.h"
//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码


class CServerDebugItemSink : public IServerDebug
{
	CGameLogic m_GameLogic;
public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//服务器调试
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame);
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem);
	virtual bool __cdecl DebugResult(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], BYTE cbCenterCard[MAX_CENTER_COUNT], ROOMUSERDEBUG Keyroomuserdebug);
};
