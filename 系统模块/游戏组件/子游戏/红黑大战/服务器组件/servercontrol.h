#pragma once

//游戏控制基类
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	virtual bool __cdecl ControlResult(BYTE	cbTableCardArray[2][MAX_CARD]) = NULL;
	//服务器控制
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption) = NULL;
	//需要控制
	virtual bool  __cdecl NeedControl() = NULL;
	//完成控制
	virtual bool  __cdecl CompleteControl() = NULL;

};
