#pragma once
#include "../���������/ServerDebug.h"
#include "../���������/GameLogic.h"
//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����


class CServerDebugItemSink : public IServerDebug
{
	CGameLogic m_GameLogic;
public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//����������
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame);
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem);
	virtual bool __cdecl DebugResult(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], BYTE cbCenterCard[MAX_CENTER_COUNT], ROOMUSERDEBUG Keyroomuserdebug);
};
