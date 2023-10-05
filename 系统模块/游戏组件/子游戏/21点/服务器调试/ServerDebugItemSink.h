#pragma once
#include "../���������/ServerDebug.h"
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
	virtual bool __cdecl ServerDebug(BYTE cbHandCardData[GAME_PLAYER*2][11], ITableFrame * pITableFrame);
	
	//��ȡ����Ϣ
	CString GetCradInfo( BYTE cbCardData );

	//���ص�������
    virtual bool __cdecl DebugResult(BYTE cbDebugCardData[GAME_PLAYER * 2][MAX_COUNT], BYTE	cbCardCount[GAME_PLAYER * 2], ROOMDESKDEBUG Keyroomuserdebug, ITableFrame * pITableFrame, WORD wBankerUser, BYTE cbPlayStatus[GAME_PLAYER]);
};
