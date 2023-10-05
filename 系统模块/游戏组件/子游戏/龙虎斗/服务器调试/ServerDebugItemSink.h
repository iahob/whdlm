#pragma once
#include "../服务器组件/ServerDebug.h"
#pragma warning(disable :4244)
//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//所有可能的牌
typedef struct
{
    BYTE							cbCardArray[2];
}CARDARRAY;

class CServerDebugItemSink : public IServerDebug
{
	//控制变量
protected:
	BYTE                            m_cbWinSideDebug;
	BYTE							m_cbExcuteTimes;					//执行次数

    CList<CARDARRAY, CARDARRAY>		m_cardArrayList;
    static const BYTE				m_cbCardData[4][13];					//扑克定义

    //组件变量
protected:
    ITableFrame						*m_pITableFrame;

public:
    CServerDebugItemSink(void);
    virtual ~CServerDebugItemSink(void);

public:
    //服务器控制
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption);

    //需要控制
	virtual bool __cdecl NeedDebug(BYTE &cbWinSide);

	//更新调试
	void  DebugInfo(bool bCancelAreaCtrl, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption);

	// 记录函数
	VOID WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);

	//返回控制区域
    virtual bool __cdecl DebugResult(BYTE	cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker);

    //桌子接口回调
    void SetTableFrameSink(ITableFrame * pITableFrame)  { if(m_pITableFrame != pITableFrame) { m_pITableFrame = pITableFrame; } }

    //获取数值
    BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }

    //初始化扑克所有种可能
    void InitCardArray();

    //计算分数
    SCORE CalculateScore(BYTE cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker);
};
