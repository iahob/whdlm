#pragma once
#include "../服务器组件/ServerControl.h"
#include "../服务器组件/GameLogic.h"

class CServerControlItemSink : public IServerControl
{
	//控制变量
protected:
	BYTE							m_cbExcuteTimes;					//执行次数
	bool							m_bWinArea[CONTROL_AREA];			//控制区域
	BYTE							m_cbWinAreaCount;
	int								m_nSendCardCount;					//发送次数
	CGameLogic						m_GameLogic;

public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	//服务器控制
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize,
		IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	//需要控制
	virtual bool __cdecl NeedControl();
	//返回控制区域
	virtual bool __cdecl ControlResult(BYTE	cbTableCardArray[2][MAX_CARD]);

	virtual bool __cdecl CompleteControl();
	//控制信息
	VOID ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	// 记录函数
	VOID WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString );

};
