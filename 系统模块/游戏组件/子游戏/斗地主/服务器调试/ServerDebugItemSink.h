#pragma once
#include "../服务器组件/ServerDebug.h"

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//信息调试
struct tagDebugInfo
{
	DWORD							dwDebugID;							//调试ID
	WORD							wTableID;							//桌子号
	int								nCardLevel;							//好牌等级
	BYTE							cbCardChance;						//出牌概率
	int								nStartTime;							//开始时间
	int								nEndTime;							//结束时间
	int								nRunCount;							//执行次数
	SCORE							lSystemScore;						//调试得分
	bool							bWaitScore;							//等待分数
	bool							bWaitDelete;						//等待删除
	CTime							tStartTime;							//开始时间
	CTime							tEndTime;							//结束时间
};

//信息调试
struct tagHistoryDebugInfo
{
	DWORD							dwDebugID;						//调试ID
	WORD							wTableID;							//桌子号
	int								nCardLevel;							//好牌等级
	BYTE							cbCardChance;						//出牌概率
	int								nDebugTime[3];					//调试日期
	int								nStartTime;							//开始时间
	int								nEndTime;							//结束时间
	SCORE							lSystemScore;						//调试得分
};

//信息调试
struct tagDebugCardInfo
{
	BYTE							cbCardData[4];						//调试扑克
	BYTE							cbCardCount;						//调试数量
	BYTE							cbDebugCount;						//调试数量
	int								nCardLevel;							//好牌等级
	tagDebugCardInfo()
	{
		ZeroMemory(this, sizeof(tagDebugCardInfo));
	}
	tagDebugCardInfo(const tagDebugCardInfo &TRight)
	{
		CopyMemory(this, &TRight, sizeof(tagDebugCardInfo));
	}
	void operator= (const tagDebugCardInfo &TRight)
	{
		CopyMemory(this, &TRight, sizeof(tagDebugCardInfo));
	}
};


typedef CWHArray< tagDebugInfo >				CDebugInfo;
typedef CWHArray< tagHistoryDebugInfo >		CHistoryDebugInfo;
typedef CWHArray< tagDebugCardInfo >			CDebugCard;

class CServerDebugItemSink : public IServerDebug
{
	DWORD				m_dwDebugID;
	CDebugInfo			m_ArrayDebug;
	CHistoryDebugInfo	m_ArrayHistoryDebug;
	CDebugCard			m_ArrayDebugCard;

public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//服务器调试
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame);
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem);

	//好牌调试
	virtual bool __cdecl GetDebugInfo(ITableFrame * pITableFrame, BYTE cbBankerCard[3], BYTE cbHandCardCount[GAME_PLAYER], BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT]);

	//更新分数
	virtual void __cdecl UpdateDebugInfo(ITableFrame * pITableFrame);
	virtual bool __cdecl UpdateDebugInfo(ITableFrame * pITableFrame, tagScoreInfo ScoreInfoArray[GAME_PLAYER]);

	//消息函数
private:
	//增加调试
	bool AddDebug(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, CMD_C_AddDebug *pAddDebug);
	//修改调试
	bool ModifyDebug(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, CMD_C_ModifyDebug *pModifyDebug);
	//删除调试
	bool DeleteDebug(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, DWORD dwDebugID);

	//内部函数
private:
	//获得索引
	int GetDebugInfoIndex(DWORD dwDebugID);
	//获取调试
	int GetDebugIndex(ITableFrame * pITableFrame, bool bUpdateScore);
	//获得索引
	int GetInsertIndex(tagDebugInfo &DebugInfo);
	//校验调试信息
	bool CheckDebugInfo(tagDebugInfo &DebugInfo, int nExcludeIndex = -1);
	//发送通知
	bool SendMesasage(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, TCHAR * pText, ...);
	// 发送信息
	bool SendDebugInfo(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, tagDebugInfo &DebugInfo);
	// 发送信息
	bool SendHistoryDebugInfo(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, tagDebugInfo &DebugInfo);
	//增加好牌
	int AddGoodCard(tagDebugCardInfo &DebugCardInfo);
	//最小组合数
	int GetMinRandCount(int nCardLevel);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//混乱扑克
	bool RandCardList(BYTE cbSrcCardData[], BYTE cbSrcCardCount, BYTE cbNewCardData[], BYTE cbNewCardCount);
	//随机剩下的牌
	bool RandDebugCardList(BYTE cbSrcCardData[], BYTE cbSrcCardCount, BYTE cbNewCardData[], BYTE cbNewCardCount, int nRandCount);
};
