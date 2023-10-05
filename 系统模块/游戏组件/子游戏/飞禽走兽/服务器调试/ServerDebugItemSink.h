#pragma once
#include "../游戏服务器/ServerDebug.h"
#include "../游戏服务器/GameLogic.h"

class CServerDebugItemSink : public IServerDebug
{
	//控制变量
protected:
	BYTE                            m_cbWinSideDebug;
	BYTE							m_cbExcuteTimes;					//执行次数

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	ITableFrame*					m_pITableFrame;						//桌子接口

public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//服务器调试
	virtual bool  ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption);

	//需要调试
	virtual bool  NeedDebug();

	//返回控制区域
	virtual bool DebugResult(BYTE cbTableCardArray[2], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], ITableFrame *pITableFrame);

	// 判断输赢
	void JudgeSystemScore(BYTE cbTableCardArray[2], LONGLONG& lSystemScore, LONGLONG& lAllScore, LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], ITableFrame *pITableFrame);

	////实际区域
	BYTE GetRealArea(BYTE cbNeedDebugArea);

	//奖励区域
	BYTE GetAnotherArea();

	//更新调试
	void  DebugInfo(const void * pBuffer, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption);

	// 记录函数
	VOID WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);

};
