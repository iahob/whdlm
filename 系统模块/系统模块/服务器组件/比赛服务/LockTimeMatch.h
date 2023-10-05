//藏宝库IT社区（28xin.com）
#ifndef LOCKTIME_MATCH_HEAD_FILE
#define LOCKTIME_MATCH_HEAD_FILE

#pragma once

//引入文件
#include "TableFrameHook.h"
#include "MatchServiceHead.h"
#include "DistributeManager.h"

//移除理由
#define REMOVE_BY_ENDMATCH			250												//比赛结束

//时钟定义
#define IDI_LOCK_CONTINUE_GAME			1											//继续游戏
#define IDI_LOCK_LEAVE_TABLE			2											//离开桌子
#define IDI_LOCK_CHECK_TABLE_START		3											//检查分配桌子是否开始(解决有可能准备后不开始情况)
#define IDI_LOCK_ANDROID_JOIN			4											//AI加入方式
///////////////////////////////////////////////////////////////////////////////////////////
//结构定义

//比赛定时器
struct tagLockMatchTimer
{
	DWORD					dwTimerID;						//定时器ID
	int						iElapse;						//执行间隔秒	
	WPARAM					wParam;							//附加参数
	LPARAM					lParam;							//附加参数
};

//比赛桌详情
struct tagLockTimeTableInfo
{
	ITableFrame		*		pTableFrame;					//桌子接口
	WORD					wTableID;						//桌子编号

	BYTE					cbTotalRounds;					//总轮数
	BYTE					cbCurRounds;					//当前轮数
	BYTE					cbCurPlayCount;					//当前局数
	tagMatchRounds			hMatchRounds[MAX_MATCH_ROUNDS];	//比赛轮数
	LONGLONG				lBaseScore;						//基础分数	
	bool					bMatchTaskFinish;				//完成标识
	bool					bSwtichTaskFinish;				//换桌标识
	bool					bWaitForNextInnings;			//等待下局
};
//////////////////////////////////////////////////////////////////////////////////////////
//数组定义
typedef CWHArray<tagLockMatchTimer *>		CLockUserMatchTimerArray;
typedef CWHArray<tagLockTimeTableInfo *>	CLockTimeTableMgrArray;
typedef CWHArray<IServerUserItem *>			CLockMatchUserItemArray;
typedef CWHArray<DWORD>						CLockMatchAndroidIDArray;

//字典定义
typedef CMap<DWORD, DWORD, IServerUserItem *, IServerUserItem *> CLockUserItemMap;
//定时赛
class CLockTimeMatch : public IGameMatchItem,public IMatchEventSink,public IServerUserItemSink
{
	//状态变量
protected:
	BYTE								m_MatchStatus;					//比赛状态	
	CTime								m_TimeLastMatch;				//比赛时间
	
	BYTE								m_cbMatchStatus;				//比赛阶段
	SYSTEMTIME							m_MatchStartTime;				//比赛开始
	BYTE								m_cbCurMatchRounds;				//当前轮数
	BYTE								m_cbTotalPlayerCount;			//参赛人数
	
	//变量定义
protected:
	CDistributeManager					m_DistributeManage;             //分组管理

	//比赛配置
protected:
	tagGameMatchOption *				m_pMatchOption;					//比赛配置	
	tagLockTimeMatch *					m_pLockTimeMatch;				//比赛规则
	tagGameServiceOption *				m_pGameServiceOption;			//服务配置
	tagGameServiceAttrib *				m_pGameServiceAttrib;			//服务属性

	//内核接口
protected:
	ITableFrame	**						m_ppITableFrame;				//框架接口
	ITimerEngine *						m_pITimerEngine;				//时间引擎
	IDBCorrespondManager *				m_pIDataBaseEngine;				//数据引擎
	ITCPNetworkEngineEvent *			m_pITCPNetworkEngineEvent;		//网络引擎

	//服务接口
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//功能接口
	IServerUserManager *				m_pIServerUserManager;			//用户管理
	IAndroidUserManager	*				m_pAndroidUserManager;			//机器管理
	IServerUserItemSink *				m_pIServerUserItemSink;			//用户回调
	//存储变量
protected:
	CLockUserItemMap					m_LockOnMatchUserMap;			//参赛用户
	CLockMatchUserItemArray				m_LockFreeUserArray;			//空闲用户
	CLockMatchAndroidIDArray			m_LockAndroidArray;				//AI
	CLockTimeTableMgrArray				m_LockTimeTableArray;			//比赛桌子
	CLockUserMatchTimerArray			m_LockMatchTimerArray;			//比赛专用定时器	
	//函数定义
public:
	//构造函数
	CLockTimeMatch();
	//析构函数
	virtual ~CLockTimeMatch(void);

	//基础接口
public:
 	//释放对象
 	virtual VOID Release(){ delete this; }
 	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口
public:
	//启动通知
	virtual void OnStartService();

	//管理接口
public:
	//绑定桌子
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID);
	//初始化接口
	virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter);	

	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID);

	//网络事件
public:
	//比赛事件
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	

	//用户接口
public:
	//用户积分
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//用户数据
	virtual bool OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason);
	//用户状态
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID);
	//用户权限
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind);
	//AI挂机事件
	virtual bool OnEventAndroidUserHangingNotify(IServerUserItem * pIServerUserItem, bool bAndroidUserHanging) { return true; }
	//AI等级事件
	virtual bool OnEventAndroidUserGradeNotify(IServerUserItem * pIServerUserItem, WORD wAndroidGrade) { return true; }

	//事件接口
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//登录完成
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem);
	//进入事件
	virtual bool OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile);	
	//用户参赛
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID);
	//用户退赛
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);

	 //功能函数
public:
	 //游戏开始
	 virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	 //游戏结束
	 virtual bool OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	 //消息函数
protected:
	//提示消息
	VOID SendWaitMessage(ITableFrame *pTableFrame, WORD wChairID = INVALID_CHAIR);
	 //用户事件
public:
	 //玩家返赛
	 virtual bool OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem);

	 //用户事件
public:
	 //用户坐下
	 virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //用户起来
	 virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //用户同意
	 virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//辅助函数
protected:
	//清除玩家
	void ClearSameTableUser(DWORD dwUserID);		
	//插入用户
	void InsertSameTableUser(DWORD dwUserID,DWORD dwTableUserID);		
	//移除分组
	bool RemoveDistribute(IServerUserItem * pIServerUserItem);
	//插入用户
	bool InsertDistribute(IServerUserItem * pIServerUserItem);
	//发送分数
	bool SendMatchUserScore(IServerUserItem * pIServerUserItem);
	//发送分数
	bool SendMatchUserInitScore(IServerUserItem * pIServerUserItem);

	//提示函数
protected:
	//保险提示
	bool SendSafeCardNotify(IServerUserItem * pIServerUserItem);
	//扣费提示
	bool SendDeductFeeNotify(IServerUserItem * pIServerUserItem);	
	//淘汰提示
	bool SendEliminateNotify(IServerUserItem * pIServerUserItem);
	//报名成功
	bool SendSignupSuccessNotify(IServerUserItem * pIServerUserItem);	
	//复活提示
	bool SendReviveNotify(IServerUserItem * pIServerUserItem, BYTE cbReviveTimesed, bool bUseSafeCard);

	//校验函数
private:
	//参赛校验
	bool VerifyUserEnterMatch(IServerUserItem * pIServerUserItem);
	//时间校验
	bool VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType);		

	//比赛事件
private:
	//比赛开始
	VOID EfficacyStartMatch();
	//发送数据
    bool SendMatchInfo(IServerUserItem * pIServerUserItem = NULL, DWORD Seconds = 0);
	//每轮结束
	bool MatchRoundsOver(int wPromotion);
	//比赛结束
	VOID MatchOver();
	//报名人满，开始比赛
	bool OnEventMatchStart();
	
	//发送函数
private:
	//发送用户分数
	bool SendGroupUserScore(IServerUserItem * pIServerUserItem);
	//为首轮插入空桌子
	void InsertNullTable();
	//发送数据到一组用户
	bool SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);
	//发送一组信息
	VOID SendGroupMessage(LPCTSTR pStrMessage);
	//发送比赛信息
	VOID SendTableMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	
	//检测函数
protected:
	//检测人数
	bool CheckMatchUser();
	//检测结束
	bool CheckMatchTaskFinish();
	//检测标记
	bool CheckSwitchTaskFinish();
	//获取名次
	WORD GetUserRank(IServerUserItem * pUserServerItem, ITableFrame *pITableFrame=NULL);
	
	//辅助函数
protected:	
	//插入分组
	VOID InsertFreeGroup(IServerUserItem * pUserServerItem);
	//继续游戏
	VOID ContinueGame(ITableFrame * pITableFrame);
	//检查桌子
	VOID CheckTableStart(ITableFrame *pITableFrame);
	//AI调试
	VOID MatchAndroidDebug(WORD wPromotion);
	//排名晋级
	VOID RankingPromotion(VOID * pData);
	//分配桌子
	VOID PerformDistributeTable();
	//用户起立
	VOID PerformAllUserStandUp();
	//用户起立
	VOID PerformAllUserStandUp(ITableFrame *pITableFrame);
	//计算场次
	LONGLONG CalcMatchNo();
	//分配用户
	bool PerformDistribute();
	//更新状态
	VOID UpdateMatchStatus();
	//切换时钟
	VOID SwitchMatchTimer();
	//截止报名
	bool IsMatchSignupEnd();
	//开始报名
	bool IsMatchSignupStart();	
	//开始间隔
	DWORD GetMatchStartInterval();	
	//執行起立
	//VOID PerformAllUserStandUp(ITableFrame *pITableFrame);
	//发送比赛信息
	void SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	//写入奖励
	bool WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo);
	//获取接口
	tagLockTimeTableInfo * GetTableInterface(ITableFrame * pITableFrame);

	//定时器函数
protected:
	//捕获定时器
	VOID CaptureMatchTimer();
	//杀死定时器
	VOID AllKillMatchTimer();
	//杀死定时器
	bool KillMatchTimer(INT_PTR dwIndexID);
	//杀死定时器
	VOID KillMatchGameTimer(DWORD dwTimerID);
	//杀死定时器
	bool KillMatchTimer(DWORD dwTimerID, WPARAM wParam);
	//投递定时器
	VOID PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam = 0, LPARAM lParam = 0);
	//设定定时器
	VOID SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
};

#endif