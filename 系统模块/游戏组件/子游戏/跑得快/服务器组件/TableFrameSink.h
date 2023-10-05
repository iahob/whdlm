#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE
#define	CARD_DISPATCHER_CONTROL

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "GameVideo.h"

#pragma warning(disable : 4244)//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前玩家
	WORD							m_wBeforWiner;						//上局赢家
	BYTE							m_bBombCount[3];					//炸弹数目
	LONGLONG						m_lBombScore[GAME_PLAYER];			//炸弹分
	LONGLONG						m_lGameScore[3];					//游戏得分
	LONGLONG						m_lCellScore;
	bool							m_bAutoPlayer[GAME_PLAYER];			//托管玩家
	BYTE							m_cbPlayStatus[GAME_PLAYER];		//游戏状态
	CList<BYTE, BYTE&>				m_llistRemainCard;					//剩余扑克列表
	BYTE							m_cbTrusteeDelayTime;				//托管延迟时间
	CTime							m_ctStatusStartTime;				//状态开始时间
	BYTE							m_cbStatusConfigTime;				//状态配置时间

	//特殊玩法
	bool							m_bSixTheenCard;					//16张牌
	bool							m_bNoForceClose;					//不可强关
	bool							m_bNoRemoveBomb;					//炸弹不可拆
	bool							m_bSpadeBanker;						//黑桃3庄
	bool							m_bFirstOutSpade;					//首出必带黑桃3(废弃变量)
	WORD							m_wPlayerCount;						//游戏人数

	bool							m_bForceOutCard;					//有大必出
	bool							m_bEchelonConclude;					//梯度结算
	BYTE							m_cbSpadeBankerFirstOutCardData;
	BYTE							m_cbWinBankerFirstOutCardData;
	//
	CMD_S_GameEnd					m_stGameEnd;						//结算数据缓存

	//游戏视频
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;

	CMD_S_RoomCardRecord			m_RoomCardRecord;					//房卡游戏记录
	CList<SCORE, SCORE&>			m_listWinScoreRecord[GAME_PLAYER];

	//历史积分
protected:
	LONGLONG                         m_lAllTurnScore[3];				//总局得分
	LONGLONG                         m_lLastTurnScore[3];				//上局得分

	//运行信息
protected:
	WORD							m_wBaoPeiUser;						//包赔玩家
	WORD							m_wTurnWiner;						//胜利玩家
	BYTE							m_bTurnCardType;					//出牌类型
	BYTE							m_bTurnCardCount;					//出牌数目
	BYTE							m_bTurnCardData[16];				//出牌列表

	//扑克信息
protected:
	BYTE							m_bCardCount[3];					//扑克数目
	BYTE							m_bHandCardData[3][16];				//手上扑克

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	ITableFrame						* m_pITableFrame;					//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;				//配置参数
	CHistoryScore					m_HistoryScore;						//历史成绩
	tagCustomRule*					m_pGameCustomRule;					//自定规则

#ifdef  CARD_DISPATCHER_CONTROL
	bool							m_bConfigCard;
	BYTE							m_cbConfigItemInfo[48];
#endif

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
	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//管理接口
public:
	//初始化
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID RepositionSink();

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
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//游戏事件
protected:
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount, bool bClient = false);
	//用户放弃
	bool OnUserPassCard(WORD wChairID);;
	//初始化剩余列表
	void InitializeListRemainCard();
	//获取剩余张数
	void GetRemainCard(REMAINCARDCOUNT &remaincard);
	//判断房卡房间
	bool IsRoomCardType();

	//
public:
	void StartTimer(DWORD dwTimerID, WPARAM wBindParam = 0);
	void StopTimer(DWORD dwTimerID);
	void KillAllTimer();
	//获取定时器配置时间
	int GetTimerConfigTime(DWORD dwTimerID);
	//获取状态剩余时间
	BYTE GetStatusLeftTime();
};

//////////////////////////////////////////////////////////////////////////////////

#endif