#pragma once

//游戏调试基类
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//服务器控制
	virtual bool ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption) = NULL;

	//需要控制
	virtual bool NeedDebug() = NULL;

	//返回控制区域
	virtual bool DebugResult(BYTE cbTableCardArray[2], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], ITableFrame *pITableFrame) = NULL;
};
