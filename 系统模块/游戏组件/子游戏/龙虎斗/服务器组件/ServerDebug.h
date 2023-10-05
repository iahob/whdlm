#pragma once

//游戏控制基类
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//服务器控制
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption) = NULL;

	//需要控制
	virtual bool __cdecl NeedDebug(BYTE &cbWinSide) = NULL;

	//返回控制区域
	virtual bool __cdecl DebugResult(BYTE	cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker) = NULL;

    //桌子接口回调
    virtual void SetTableFrameSink(ITableFrame * pITableFrame) = NULL;
};

