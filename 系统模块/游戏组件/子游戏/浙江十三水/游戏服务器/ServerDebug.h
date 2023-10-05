#pragma once

//游戏控制基类
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//返回控制区域
	virtual bool __cdecl DebugResult(BYTE cbControlCardData[GAME_PLAYER][HAND_CARD_COUNT], ROOMUSERDEBUG Keyroomuserdebug, BYTE cbMaCard, WORD wBanker) = NULL;
};
