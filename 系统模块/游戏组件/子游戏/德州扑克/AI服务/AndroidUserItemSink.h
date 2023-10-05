#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\服务器组件\GameLogic.h"
#pragma warning(disable : 4244)
//时间定义
#define TIMER_START_GAME			10									//准备时间
//辅助时间
#define TIME_LESS					2									//最少时间
#define IDI_START_GAME				1									//开始定时器
#define IDI_ADD_SCORE				2									//下注
#define IDI_SET_CHIP				3									//携带筹码

//////////////////////////////////////////////////////////////////////////

//AI类
class CAndroidUserItemSink : public IAndroidUserItemSink
{

	//时间定义
protected:
	BYTE						m_cbTimeStartGame;						//开始时间
	BYTE						m_cbTimeBet;							//叫庄时间
	LONGLONG					m_lMinBetScore;							//最小带入
	LONGLONG					m_lMaxBetScore;							//最大带入
	LONGLONG					m_lAnteScore;							//固定底注
	LONGLONG					m_lCellScore;							//大盲
	BYTE						m_cbUserCount;							//玩家人数
	//游戏变量
protected:
	BYTE						m_cbPlayStatus[GAME_PLAYER];			//玩家状态
	LONGLONG					m_lUserMaxScore[GAME_PLAYER];			//携带分数
	LONGLONG					m_lUserTableScore[GAME_PLAYER];			//当前下注分数
	LONGLONG					m_lUserTotalScore[GAME_PLAYER];			//总下注分数
	BYTE						m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//手上扑克
	BYTE						m_cbCenterCardCount;					//中心扑克数量
	BYTE						m_cbCenterCardData[MAX_CENTER_COUNT];	//中心扑克
	bool						m_bAndroidUser[GAME_PLAYER];			//AI玩家
	LONGLONG					m_lTurnLessScore;						//最小下注
	LONGLONG					m_lTurnMaxScore;						//最大下注
	LONGLONG					m_lAddLessScore;						//最小加注
	BYTE						m_cbFinalCardType[GAME_PLAYER];			//最后牌型
	UserWinList					m_UserWinList;							//最大牌玩家
	BYTE						m_cbCurType[GAME_PLAYER];				//当前牌型
	BYTE						m_cbCurCardWeight;						//当前牌权重
	bool						m_bMustWin;								//机器必须赢
	bool						m_bAddedScoreThisTurn;					//本轮已主动加注
	//控件变量
protected:
	CGameLogic					m_GameLogic;							//游戏逻辑
	IAndroidUserItem *			m_pIAndroidUserItem;					//用户接口
	//AI存取款
	LONGLONG					m_lRobotScoreRange[2];					//最大范围
	LONGLONG					m_lRobotBankGetScore;					//提款数额
	LONGLONG					m_lRobotBankGetScoreBanker;				//提款数额 (庄家)
	int							m_nRobotBankStorageMul;					//存款倍数
	WORD						m_wServerID;							//房间标识

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
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//调试接口
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
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual VOID OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//消息处理
protected:
	//发送扑克
	bool OnSubAndroidCard(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//玩家下注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//玩家弃牌
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//发牌
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize) ;

private:
	//获取自己
	IServerUserItem* GetMeItem(){ return m_pIAndroidUserItem->GetMeUserItem(); }
	//获取自己座位号
	WORD GetMeChairID();
	//银行操作
	void BankOperate();
	//写日志
	void WriteDebugInfo(LPCTSTR pszString);
	//自己是否最大赢家
	bool IsMeWinner();
	//加注
	bool AddScore(LONGLONG lscore);
	//弃牌
	bool GiveUp();
	//获取加注值
	LONGLONG GetAddScore();
	//是否需要弃牌
	bool NeedGiveUp();
};

//////////////////////////////////////////////////////////////////////////

#endif