#pragma once

//��Ϸ���ƻ���
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//���ؿ�������
	virtual bool __cdecl ControlResult(BYTE cbControlCardData[GAME_PLAYER][HAND_CARD_COUNT], ROOMUSERCONTROL Keyroomusercontrol, BYTE cbMaCard, WORD wBanker) = NULL;
};
