#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	bool								m_b2BiggestMode;								//是否2最大
	BYTE								m_cbFriendFlag;									//同盟标记：0未知，1宣战，2暗独，3暗独明示，4明独，5选队友，6选队友明示
	BYTE								m_cbSelectedCardData;						//红桃玩家选择的牌
	WORD							m_wXuanZhanUser;							//宣战玩家
	WORD							m_wAskUser;										//询问玩家
	bool								m_bAddTimes[GAME_PLAYER];			//是否加倍
	bool								m_bAskFriend[GAME_PLAYER];			//是否询问同盟
	int									m_cbGameScore[GAME_PLAYER];		//游戏得分
	BYTE								m_cbBaseTimes;								//基础倍数
	WORD							m_wFriend[GAME_PLAYER];				//队友
	BYTE								m_cbAskStatus[GAME_PLAYER];
	BYTE								m_cbCurTurnScore;							//当前轮分数
	BYTE								m_cbFinishUserCount;
	WORD							m_wFinishUser[GAME_PLAYER];
	bool								m_bWinFlag[GAME_PLAYER];				//输赢标识
	bool								m_bShuangKou;									//双扣加倍
	bool								m_bTrustee[GAME_PLAYER];				//托管

	bool								m_bGameFinish;								//结束标志
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前玩家
	bool								m_bCurrenUserDone;					//当前玩家是否动作
	BYTE								m_cbOutCardCount[GAME_PLAYER];		//出牌次数
	BYTE								m_cbWaitTime;						//等待时间

	tagSearchCardResult				m_SearchCardResult;					//搜索结果
	
	//出牌信息
protected:
	WORD						m_wTurnWiner;						//胜利玩家
	BYTE							m_cbTurnCardCount;					//出牌数目
	BYTE							m_cbTurnCardData[MAX_CARD_COUNT];	//出牌数据

	//扑克信息
protected:
	BYTE							m_cbHandCardCount[GAME_PLAYER];		//扑克数目
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_CARD_COUNT];//手上扑克

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CHistoryScore					m_HistoryScore;						//历史成绩

	//组件接口
protected:
	ITableFrame	*					m_pITableFrame;						//框架接口
	tagCustomRule *					m_pGameCustomRule;					//自定规则
	tagGameServiceOption *			m_pGameServiceOption;				//游戏配置
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//游戏属性

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//复位桌子
	virtual VOID RepositionSink();
	//配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//时间事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//网络接口
public:
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore);
	//游戏事件
protected:
	//用户宣战
	bool OnUserXuanZhan(WORD wChairID, bool bXuanZhan);
	//用户找同盟
	bool OnUserFindFriend(WORD wChairID, BYTE cbCardData);
	//用户问同盟
	bool OnUserAskFriend(WORD wChairID,bool bAsk);
	//用户加倍
	bool OnUserAddTimes(WORD wChairID, bool bAddTimes);
	//用户放弃
	bool OnUserPassCard(WORD wChairID,bool bTrusteePass=false);
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount,bool bTrusteeOut=false);
	//用户托管
	bool	OnUserTrustee(WORD wChairID,bool bTrustee);
	//用户逃跑
	bool OnUserFlee(WORD wChairID);
	//自动出牌
	bool AutomatismOutCard();
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif