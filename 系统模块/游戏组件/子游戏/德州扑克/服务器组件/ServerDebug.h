#pragma once

//��Ϸ���Ի���
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//����������
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame) = NULL;
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem) = NULL;
	virtual bool __cdecl DebugResult(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT],BYTE cbCenterCard[MAX_CENTER_COUNT], ROOMUSERDEBUG Keyroomuserdebug) = NULL;
};
