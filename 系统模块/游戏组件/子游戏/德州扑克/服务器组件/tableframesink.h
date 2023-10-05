#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerDebug.h"
#include "DlgCustomRule.h"
//////////////////////////////////////////////////////////////////////////
//类型定义

typedef CWHArray<WORD> CUserDynamicJoin;									//坐下记录

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//玩家变量
protected:
	WORD							m_wDUser;								//D玩家
	WORD							m_wMinChipInUser;						//小盲注
	WORD							m_wMaxChipInUser;						//在盲注
	WORD							m_wCurrentUser;							//当前玩家
	WORD							m_wPlayerCount;							//游戏人数	
	//玩家状态
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE							m_cbPlayStatusRecord[GAME_PLAYER];		//游戏状态
	BYTE							m_cbPlayDynamicJoin[GAME_PLAYER];		//游戏状态
	bool							m_bStandUp[GAME_PLAYER];				//玩家站起
	bool							m_bCanStandUp[GAME_PLAYER];				//玩家站起
	CUserDynamicJoin				m_UserDynamicJoin;						//坐下顺序
	bool							m_bRealPlayer[GAME_PLAYER];
	bool							m_bUpdateStorage;						//更新库存
	//结束信息
	bool							m_bGameEnd[GAME_PLAYER];				//游戏结束
	CMD_S_GameConclude				m_GameEnd;								//结束信息
	tagScoreInfo					m_ScoreInfo[GAME_PLAYER];				//得分信息

	CMD_S_Record					m_stRecord;								//约战记录
	DWORD							m_dwUserID[GAME_PLAYER];				//玩家ID
	CMD_S_LastRecord				m_stLastRecord;							//上局信息
	CMD_S_LastRecord				m_stCurRecord;							//当前记录
	bool							m_bUpdatePool;							//是否更新池子
	BYTE							m_cbPoolCount;							//奖池数量
	SCORE							m_lPoolMinScore[GAME_PLAYER];			//入池最低分
	SCORE							m_lPoolScore[GAME_PLAYER+1];			//奖池分数
	bool							m_bPoolPlayer[GAME_PLAYER][GAME_PLAYER];//池子玩家
	bool							m_bAndroidMustWin;
	DWORD							m_dwStartTime;							//切换玩家操作计时
	//加注信息
protected:
	SCORE							m_lCellScore;							//单元下注
	SCORE							m_lMinBetScore;							//最低携带金币
	SCORE							m_lMaxBetScore;							//最高携带金币
	SCORE							m_lAnteScore;							//固定底注
	BYTE							m_cbTime;								//开房时间
	int								m_nLeftSecond;							//私人房剩余时间
	time_t							m_tStartTime;
	SCORE							m_lTurnLessScore;						//最小下注
	SCORE							m_lAddLessScore;						//加最小注
	SCORE							m_lTurnMaxScore;						//最大下注
	SCORE							m_lBalanceScore;						//平衡下注
	WORD							m_wOperaCount;							//操作次数
	BYTE							m_cbBalanceCount;						//平衡次数
	SCORE							m_lTableScore[GAME_PLAYER];				//桌面下注
	SCORE							m_lTotalScore[GAME_PLAYER];				//累计下注
	bool							m_bAutoChip[GAME_PLAYER];				//自动兑换
	SCORE							m_lUserMaxScore[GAME_PLAYER];			//最大下注
	SCORE							m_lUserTakeScore[GAME_PLAYER];			//游戏中补充分数
	BYTE							m_cbShowHand[GAME_PLAYER];				//梭哈用户
	bool							m_bTrustee[GAME_PLAYER];				//是否托管
	bool							m_bGiveUp[GAME_PLAYER];					//是否弃牌
	//扑克信息
protected:
	BYTE							m_cbStartCount;							//开始人数
	BYTE							m_cbSendCardCount;						//发牌数目
	BYTE							m_cbRepertoryData[FULL_COUNT];			//库存数据
	BYTE							m_cbCenterCardData[MAX_CENTER_COUNT];	//中心扑克
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//手上扑克

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数
	tagGameServiceAttrib			* m_pGameServiceAttrib;					//游戏属性
	tagCustomRule					* m_pGameCustomRule;					//自定义规则
	//属性变量
public:
	static DWORD					m_dwRandSend;							//随机种子

	//服务调试
protected:
	HINSTANCE						m_hDebugInst;
	IServerDebug*					m_pServerDebug;

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

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
	//定时器事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//数据事件
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize);

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){}

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	
	//动作事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);


	//消息事件
protected:
	//用户站起
	bool OnUserStandUp(IServerUserItem * pIServerUserItem, WORD wChairID);
	//筹码事件
	bool OnUserSetChipScore(IServerUserItem * pIServerUserItem, WORD wChairID, bool bAutoChip, SCORE lChipScore);
	//放弃事件
	bool OnUserGiveUp(WORD wChairID,bool bLeft=false);
	//换牌事件
	bool OnUserSwitchCard(IServerUserItem * pIServerUserItem);
	//亮牌事件
	bool OnUserShowCard(WORD wChairID);
	//加注事件
	bool OnUserAddScore(WORD wChairID, SCORE lScore, bool bGiveUp);	
		
	//辅助函数
protected:
	//游戏结束
	bool OnGameFree();
	//牌转文字
	CString TransformCardInfo( BYTE cbCardData );
	//混乱数据
	void RandCardList(BYTE pBufferData[], WORD wBufferSize);
	//写日志
	void WriteDebugInfo(LPCTSTR pszString);
	//特殊处理
	bool OnDebugCard(IServerUserItem * pIServerUserItem,bool bFresh=false);
	//是否私人房
	bool IsPersonalRoom();

	//调试函数
protected:
	//除重用户调试
	void TravelDebugList(ROOMUSERDEBUG keyroomuserdebug);
	//是否满足调试条件
	void IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnableDebug);
	//获取调试类型
	void GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr);
	//更新房间用户信息
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
	//分析房间用户调试
	bool AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList);
	//扑克分析
	void AnalyseCard();
};

//////////////////////////////////////////////////////////////////////////

#endif
