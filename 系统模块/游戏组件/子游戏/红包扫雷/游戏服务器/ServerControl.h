#pragma once

#define MAX_CARD_COUNT				3									//最大数目
#define AREA_MAX					5									//最大区域

//游戏控制基类
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//服务器控制
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption) = NULL;

	//需要控制
	virtual bool NeedControl() = NULL;

	//完成控制
	virtual bool CompleteControl() = NULL;

	//开始控制
	virtual void GetSuitResult(BYTE cbTableCardArray[AREA_MAX][MAX_CARD_COUNT], LONGLONG lAllAreaBet[AREA_MAX - 1]) = NULL;
};
