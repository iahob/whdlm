#pragma once
#include "../游戏服务器/ServerDebug.h"
#include "GameLogic.h"

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码


class CServerDebugItemSink : public IServerDebug
{
public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

	CGameLogic						m_GameLogic;							//游戏逻辑

public:
	//服务器调试
	virtual bool __cdecl ServerDebug(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], ITableFrame * pITableFrame, WORD wPlayerCount);

	//获取牌信息
	CString GetCradInfo( BYTE cbCardData );

	//返回调试区域
	virtual bool __cdecl DebugResult(BYTE cbDebugCardData[GAME_PLAYER][MAX_COUNT], ROOMUSERDEBUG Keyroomuserdebug);
};
