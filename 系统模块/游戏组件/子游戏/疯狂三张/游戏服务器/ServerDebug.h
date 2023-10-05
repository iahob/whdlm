#pragma once

//游戏调试基类
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//服务器调试
	virtual bool __cdecl ServerDebug(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], ITableFrame * pITableFrame, WORD wPlayerCount) = NULL;

	//返回调试区域
	virtual bool __cdecl DebugResult(BYTE cbDebugCardData[GAME_PLAYER][MAX_COUNT], ROOMUSERDEBUG Keyroomuserdebug) = NULL;
};
