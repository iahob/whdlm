#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//出牌结果
struct tagOutCardResult
{
	BYTE							cbOperateCode;							//出牌代码
	BYTE							cbOperateCard;							//出牌值
};

//听牌分析结果
struct tagTingCardResult
{
	BYTE cbAbandonCount;													//可丢弃牌数
	BYTE cbAbandonCard[MAX_COUNT];											//可丢弃牌
	BYTE cbTingCardCount[MAX_COUNT];										//听牌数目
	BYTE cbTingCard[MAX_COUNT][MAX_COUNT-1];								//听牌
	BYTE cbRemainCount[MAX_COUNT];											//听的牌的剩余数
};

//////////////////////////////////////////////////////////////////////////

//AI类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//游戏变量
protected:
	bool							m_bCheatMode;						//防作弊标识
	bool							m_bHuanSanZhang;					//换三张标识
	bool							m_bStartOutCard;					//开始出牌

	//用户变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户
	BYTE							m_cbActionMask;						//玩家动作
	BYTE							m_cbActionCard;						//动作扑克
	LONG							m_lSiceCount;						//骰子点数
	TCHAR							m_szAccounts[GAME_PLAYER][32];		//用户名字
	IClientUserItem					*m_pIClientUserItemArray[GAME_PLAYER];

	//缺门信息
protected:
	bool							m_bCallCard[GAME_PLAYER];			//用户状态
	BYTE							m_cbCallCard[GAME_PLAYER];			//用户缺门

	//扑克变量
protected:
	BYTE							m_cbLeftCardCount;					//剩余数目
	BYTE							m_cbCardIndex[MAX_INDEX];			//手中扑克

	//组合扑克
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];		//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][4];	//组合扑克

	//出牌信息
protected:
	BYTE							m_cbLastcbOutCard;					//自己最后出牌
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbOutCardData;					//出牌扑克
	BYTE							m_cbEnjoinOutCardCount;				//禁止出牌数
	BYTE							m_cbEnjoinOutCard[MAX_COUNT];		//禁止出牌数据
	BYTE							m_cbDiscardCount[GAME_PLAYER];		//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];	//丢弃记录
	BYTE							m_cbFirstSendCardData;				//第一次发牌扑克，庄家有效

	//时间信息
	BYTE							m_cbTimeOutCard;					//出牌时间
	BYTE							m_cbTimeOperateCard;				//操作时间
	BYTE							m_cbTimeStartGame;					//开始时间

	//组件变量
public:
	CGameLogic						m_GameLogic;						//游戏逻辑
	IAndroidUserItem *				m_pIAndroidUserItem;				//用户接口

	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pData, WORD wDataSize);
	//用户选缺
	bool OnSubCallCard(const void * pData, WORD wDataSize);
	//庄家信息
	bool OnSubBankerInfo(const void * pData, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pData, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pData, WORD wDataSize);
	//操作提示
	bool OnSubOperateNotify(const void * pData, WORD wDataSize);
	//操作结果
	bool OnSubOperateResult(const void * pData, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pData, WORD wDataSize);
	//用户托管
	bool OnSubTrustee(const void * pData,WORD wDataSize);
	//用户吃胡
	bool OnSubUserChiHu( const void *pData,WORD wDataSize );
	//用户杠牌
	bool OnSubGangScore( const void *pData, WORD wDataSize );
	//用户换三张结果
	bool OnSubChangeCardResult(const void * pData, WORD wDataSize);
	//流水结果
	bool OnSubViewChart(const void * pData, WORD wDataSize);

	//处理函数
protected:
	//打缺
	void CallCard();
	//换牌
	void ChangeCard();	
	//出牌
	void OnOutCard( BYTE cbOutCard, bool bSend );
	//操作牌
	void OnOperateCard( BYTE cbOperateCode,BYTE cbOperateCard );

	//辅助函数
protected:
	//返回废弃的牌
	bool SearchTingCard( tagTingCardResult &TingCardResult );
	//搜索出牌
	bool SearchOutCard( tagOutCardResult &OutCardResult );
	//自动出牌
	bool AutomatismOutCard();
};

//////////////////////////////////////////////////////////////////////////

#endif