#pragma once

//��Ϸ���ƻ���
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//���ؿ�������
	virtual bool __cdecl DebugResult(BYTE cbDebugCardData[GAME_PLAYER][MAX_CARDCOUNT], WORD wCtrlChairID, bool bWin, SENDCARDTYPE_CONFIG stConfig, CARDTYPE_CONFIG ctConfig, KING_CONFIG gtConfig, WORD	wPlayerCount) = NULL;
};
