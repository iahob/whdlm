#pragma once

#define MAX_COUNT					11									//�����Ŀ

//��Ϸ���Ի���
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//����������
	virtual bool __cdecl ServerDebug(BYTE cbHandCardData[GAME_PLAYER*2][11], ITableFrame * pITableFrame) = NULL;
	virtual bool __cdecl ServerDebug(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame) = NULL;

	//���ص�������
	virtual bool __cdecl DebugResult(BYTE cbDebugCardData[GAME_PLAYER * 2][MAX_COUNT], BYTE	cbCardCount[GAME_PLAYER*2], ROOMUSERDEBUG Keyroomusercontrol, ITableFrame * pITableFrame, WORD wBankerUser, BYTE cbPlayStatus[GAME_PLAYER]) = NULL;
};
