#pragma once

//��Ϸ���Ի���
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//����������
	virtual bool __cdecl ServerDebug(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], ITableFrame * pITableFrame, WORD wPlayerCount) = NULL;

	//���ص�������
	virtual bool __cdecl DebugResult(BYTE cbDebugCardData[GAME_PLAYER][MAX_COUNT], ROOMUSERDEBUG Keyroomuserdebug) = NULL;
};
