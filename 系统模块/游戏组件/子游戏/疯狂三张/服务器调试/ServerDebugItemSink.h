#pragma once
#include "../��Ϸ������/ServerDebug.h"
#include "GameLogic.h"

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����


class CServerDebugItemSink : public IServerDebug
{
public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

	CGameLogic						m_GameLogic;							//��Ϸ�߼�

public:
	//����������
	virtual bool __cdecl ServerDebug(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], ITableFrame * pITableFrame, WORD wPlayerCount);

	//��ȡ����Ϣ
	CString GetCradInfo( BYTE cbCardData );

	//���ص�������
	virtual bool __cdecl DebugResult(BYTE cbDebugCardData[GAME_PLAYER][MAX_COUNT], ROOMUSERDEBUG Keyroomuserdebug);
};
