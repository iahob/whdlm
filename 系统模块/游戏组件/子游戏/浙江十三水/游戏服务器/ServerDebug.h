#pragma once

//��Ϸ���ƻ���
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//���ؿ�������
	virtual bool __cdecl DebugResult(BYTE cbControlCardData[GAME_PLAYER][HAND_CARD_COUNT], ROOMUSERDEBUG Keyroomuserdebug, BYTE cbMaCard, WORD wBanker) = NULL;
};
