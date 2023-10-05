#pragma once
#include "../服务器组件/ServerDebug.h"
#include "../服务器组件/GameLogic.h"

#define INDEX_BANKER				1
#define DEBUG_AREA					4
#define MAX_CARDGROUP				5

class CServerDebugItemSink : public IServerDebug
{
	//玩家调试						
protected:
	BYTE							m_cbExcuteTimes;								//执行次数
	BYTE							m_cbDebugStyle;									//调试方式
	bool							m_bWinArea[DEBUG_AREA];							//赢家区域
	BYTE							m_cbWinAreaCount;
	int								m_nCompareCard[MAX_CARDGROUP][MAX_CARDGROUP];	//对比矩阵

	//辅助信息
protected:
	BYTE							m_cbTableCardArray[MAX_CARDGROUP][MAX_CARD];	//牌组信息
	BYTE							m_cbTableCard[CARD_COUNT];						//桌面扑克
	LONGLONG						m_lUserJettonScore[AREA_MAX + 1][GAME_PLAYER];	//全体总注
	WORD							m_wCurrentBanker;
	ITableFrame						*m_pITableFrame;
	CGameLogic						m_GameLogic;									//游戏逻辑

public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//服务器调试
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize,
		IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	//需要调试
	virtual bool  NeedDebug();

	//开始调试
	virtual void  GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], WORD wCurrentBanker);

	//输赢调试
protected:

	//区域牌型
	bool AreaWinCard(BYTE cbStack[]);

	//计算分配
	bool GetSuitCardCombine(BYTE cbStack[]);

	//系统输赢
	LONGLONG GetSysWinScore(int nWinMultiple[]);

	//系统赢牌
	void SysWinCard(bool bIsWin,BYTE cbStack[]);

	//调试信息
	VOID DebugInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	// 记录函数
	VOID WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString );
};
