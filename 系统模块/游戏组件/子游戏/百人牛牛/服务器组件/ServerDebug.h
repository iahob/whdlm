#pragma once

#define MAX_CARD					5

//游戏控制基类
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//服务器控制
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption) = NULL;

	//需要控制
	virtual bool  NeedDebug() = NULL;

	//开始控制
	virtual void  GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], WORD wCurrentBanker) = NULL;
};
