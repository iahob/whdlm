#pragma once

//游戏控制基类
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//服务器控制
	virtual bool  ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame) = NULL;

	//需要控制
	virtual bool  NeedControl() = NULL;

	//完成控制
	virtual bool  CompleteControl() = NULL;

	//返回控制区域
	virtual bool  ReturnControlArea(tagControlInfo& ControlInfo,LONGLONG lAllJettonScore[]) = NULL;

	//返回系统控制区域
	virtual bool  ReturnSysCtrlArea(tagControlInfo& ControlInfo, LONGLONG lUserJettonScore[AREA_COUNT + 1][GAME_PLAYER], ITableFrame *pITableFrame) = NULL;

	//获取当前控制类型
	virtual EM_CTRL_TYPE GetCurCtrlType() = NULL;
};
