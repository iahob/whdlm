#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"
#include "..\消息定义\CMD_Thirteen.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "ServerDebug.h"
#include "..\..\..\..\系统模块\全局定义\Log.h"
#include "..\游戏录像\GameVideo.h"
//////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4244)

#define CARD_DISPATCHER_CONTROL

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//组件变量
protected:
	CGameLogic							m_GameLogic;										//游戏逻辑
	ITableFrame							*m_pITableFrame;									//框架接口
	const tagGameServiceOption			*m_pGameServiceOption;								//配置参数
	tagCustomRule*						m_pGameCustomRule;									//自定规则
	CHistoryScore						m_HistoryScore;										//历史成绩
	DWORD								m_dwShowTime;										//开始时间
	CMD_S_Record						m_stRecord;											//游戏记录
	std::vector<LONGLONG>				m_vecRecord[GAME_PLAYER];

	//时间信息
protected:
	BYTE								m_cbTimeStartGame;									//开始时间
	BYTE								m_cbTimeRangeCard;									//理牌时间
	//游戏变量
protected:
	SCORE								m_lUserChip[GAME_PLAYER];							//用户下注
	SCORE								m_lScoreTimes[GAME_PLAYER][3];						//输赢注数
	SCORE								m_lScoreTimesEx[GAME_PLAYER][GAME_PLAYER];			//输赢注数 与其他玩家的结算
	SCORE								m_lScoreTimesExEx[GAME_PLAYER][GAME_PLAYER][3];		//输赢注数 与其他玩家每一道的结算
	SCORE								m_lscoreTimesPlus[GAME_PLAYER][3+1];				//每一道输赢+总分
	INT									m_bAllWin[GAME_PLAYER][GAME_PLAYER];				//全赢
	BYTE								m_bSpecialType[GAME_PLAYER];						//特殊牌型
	BYTE								m_bUserCardData[GAME_PLAYER][HAND_CARD_COUNT];		//玩家扑克
	BYTE								m_bSegmentCard[GAME_PLAYER][3][5];					//分段扑克
	BYTE								m_cbShowSpecialType[GAME_PLAYER];					//特殊牌型	
	WORD								m_wBanker;											//庄家玩家
	WORD								m_wShowCardCount;									//摊牌计数
	bool								m_bStartCompare;									//开始比牌
	WORD								m_wCompleteCompareCount;							//比较计数		
	bool								m_bCompleteCompare[GAME_PLAYER];					//完成比较
	bool								m_bFinishSegment[GAME_PLAYER];						//完成分段
	bool								m_bGameStatus[GAME_PLAYER];							//游戏状态
	WORD								m_wUserEnter[GAME_PLAYER];							//玩家进场
	CMD_S_MobilePutCard					m_MobilePutCard[GAME_PLAYER];						//推荐牌型
	WORD								m_wAllKillUser;										//全垒打玩家
	//调试变量
protected:
	bool								m_bHaveBanker;										//庄家模式
	INT 								m_nAllWinTimes;										//打枪额外番数
	BYTE								m_cbPlayerCount;									//游戏人数

	BYTE								m_cbMaCard;											//码牌
	WORD								m_wMaCardUser;										//码牌玩家
	BYTE								m_cbTongHuaMode;									//同花比较0先比点数，1先比花色
	BYTE								m_cbQuickMode;										//快速比牌0普通，1快速
	bool								m_bTest;
	SCORE								m_lTestScore[GAME_PLAYER];							//测试得分
	CMD_S_GameEnd						m_stGameEnd;										//游戏结果
	//游戏视频
protected:
	HINSTANCE							m_hVideoInst;
	IGameVideo*							m_pGameVideo;

	//调试服务
protected:
	HINSTANCE							m_hDebugInst;
	IServerDebug*						m_pServerDebug;

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
	//是否有效
	virtual bool  IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
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

	//通用函数
public:
	void MatchCard();
	//统计人数
	DWORD GetPlayerCount();
	//获取倍数
	void CompareCardGetTimes();
	//开始比牌
	VOID StartCompareCard();
	//获取倍数
	int GetCardTimes(BYTE bCard[13]);
	//获取下注
	void GetChips( LONGLONG lChipArray[GAME_PLAYER][SET_CHIP_COUNT]);
	//获取下注
	void GetChips(WORD wChairID, LONGLONG lChipArray[SET_CHIP_COUNT]);

	//消息处理
protected:
	//玩家摊牌
	bool OnUserShowCard(const WORD wChairID , const void * pDataBuffer, WORD wDataSize) ;

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

	//////////////////////////////////////////////////////////////////////////
	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};    
	//将牌型转化为牌型得分
	int   CardTypeToScore(BYTE bCardType,BYTE bSpecies);

public:
	//扑克分析
	void AnalyseCard();
	//除重用户调试
	void TravelDebugList(ROOMUSERDEBUG keyroomuserdebug);
	//是否满足调试条件
	void IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnabledebug);
	//获取调试类型
	void GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr);
	//读取配置
	void ReadConfigInformation();
	//更新房间用户信息
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
	//分析房间用户控制
	bool AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList);
	void WriteDebugInfo(LPCTSTR pszString);
	//分析调试结果
	SCORE JudgeStock();
	//计算得分
	void CalculateScore();
};

//////////////////////////////////////////////////////////////////////////

#endif
