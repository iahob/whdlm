#pragma once

//��Ϸ���Ի���
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//���ص�������
	virtual bool __cdecl DebugResult(BYTE cbFirstCardData[MAX_CARD_COUNT], BYTE cbSecondCardData[MAX_CARD_COUNT], S_ROOMUSERDEBUG keyRoomUserDebug) = NULL;
};
