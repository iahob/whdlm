#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "ServerDebug.h"

struct tagAreaInfo
{
	SCORE					lAreaInScore;		//区域下注
	SCORE					lAreaAnChou;		//区域暗抽
	SCORE					lAreaMosgold;		//区域大奖
	SCORE					lAreaWinScore;		//区域得分
	SCORE					lAreaRevenue;		//区域税收
};

//////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//下注变量
protected:
	SCORE								m_lAreaInAllScore[AREA_MAX];				//每个区域的总下注			
	SCORE								m_lUserInAllScore[GAME_PLAYER][AREA_MAX];	//每个玩家每个区域的下注
	SCORE								m_lUserAreaWinScore[GAME_PLAYER][AREA_MAX];	//每个玩家每个区域的得分
	SCORE								m_lUserStartScore[GAME_PLAYER];				//起始分数
	SCORE								m_lAreaLimitScore;							//区域限制
	SCORE								m_lUserLimitScore;							//个人限制
	tagAreaInfo							m_tagUserAreaInfo[GAME_PLAYER][AREA_MAX];	//每个玩家区域信息(统计真人)

	//上局信息
public:
	SCORE								m_lLastAllJettonPlace[GAME_PLAYER][AREA_MAX]; //上局总下注
	SCORE								m_lLastAllJettonArea[GAME_PLAYER][AREA_MAX];  //本局最后一次操作

	//成绩
protected:
	SCORE								m_lUserWinScore[GAME_PLAYER];				//玩家成绩
	SCORE								m_lUserRevenue[GAME_PLAYER];				//玩家税收
	SCORE								m_lBankerWinScore;							//庄家成绩
	SCORE								m_lCurTurnAnChouWin;						//当局暗抽总数

	//扑克信息
protected:
	BYTE								m_cbTableCardArray[2];						//桌面扑克
	
	//庄家信息
protected:
	bool								m_bEnableBanker;							//上庄功能
	LONGLONG							m_lApplyBankerCondition;					//申请条件
	CWHArray<WORD>						m_ApplyUserArray;							//申请玩家
	WORD								m_wCurrentBanker;							//当前庄家
	WORD								m_wBankerTime;								//做庄次数
	SCORE								m_lBankerScore;								//庄家金币
	bool								m_bEnableSysBanker;							//系统做庄
	bool								m_bWaitGiveUpBanker;						//等待主动下庄

	//庄家设置
protected:
	//加庄局数设置：当庄家坐满设定的局数之后(m_lBankerMAX)，
	//所带金币值还超过下面申请庄家列表里面所有玩家金币时，
	//可以再加坐庄m_lBankerAdd局，加庄局数可设置。
	LONGLONG							m_lBankerMAX;								//最大庄家数
	LONGLONG							m_lBankerAdd;								//庄家增加数

	//金币超过m_lBankerScoreMAX之后，就算是下面玩家的金币值大于他的金币值，他也可以再加庄m_lBankerScoreAdd局。
	SCORE								m_lBankerScoreMAX;							//庄家钱
	LONGLONG							m_lBankerScoreAdd;							//庄家钱大时,坐庄增加数

	//最大庄家数
	LONGLONG							m_lPlayerBankerMAX;							//玩家最大庄家数

	//换庄
	bool								m_bExchangeBanker;							//交换庄家

	//占位
    
    //机器人占位
    bool								m_bEnableRobotOccupySeat;				    //机器人占位标识
    int									m_nRobotOccupySeatTable;				    //占位桌数
    int									m_nRobotQuitSeatTurn;					    //退出占位局数	
    LONGLONG							m_lRobotOccupySeatPlayCount[GAME_PLAYER];   //机器人占位后玩的局数(只保存机器人)
	WORD								m_wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID
	BYTE								m_bContinueNoBetCount[MAX_OCCUPY_SEAT_COUNT];		//占位玩家连续不下注次数

	//机器人上庄
	int									m_nRobotRequestBankerCount;				    //上庄数目
	LONGLONG							m_lRobotLostScoreLimitQB;					//超过时强制下庄
	DWORD								m_wRobotMaxSerialTurn;						//连续上庄N局时强制下庄
	SCORE								m_lRobotBankerWinScore;						//机器人庄家坐庄累加成绩

	//机器人变量
protected:
	int									m_nMaxChipRobot;						//最大下注机器人数目
	int									m_nChipRobotCount;						//已下注机器人统计
	SCORE								m_lRobotAreaLimit;						//机器人区域限制
	SCORE								m_lRobotAreaScore[AREA_MAX];			//机器人区域下注
	int									m_nRobotListMaxCount;					//列表最多人数(机器人上庄)

    SCORE	                            m_lRealAllJettonScore[10];                  //所有真人玩家近十轮下注额，用来判断虚拟机器人押注
    SCORE                            	m_lLimitRobotJettonScore;                   //每局机器人下注总额限制
    SCORE	                            m_lCurrentRobotJettonScore;                 //当局机器人玩家下注总额

	CTime								m_tmStartPlaceJetton;					//开始下注时间

	//时间设置
protected:				
	DWORD								m_dwJettonTime;							//开始时间
	BYTE								m_cbFreeTime;							//空闲时间
	BYTE								m_cbBetTime;							//下注时间
	BYTE								m_cbEndTime;							//结束时间

	//记录变量
protected:
	LONGLONG							m_lCurGameCount;
	tagServerGameRecord					m_SimpleChartArray[MAX_SIMPLE_CHART_COUNT];			//简洁路单记录(最近的20局胜数结果，不清空)
	tagServerGameRecord					m_AdvancedChartArray[MAX_ADVANCED_CHART_COUNT];		//高级路单记录(显示最近 78局，可以左右拖动   满足100局清空)
	CList<tagStatisticsRecord, tagStatisticsRecord&>	m_listStatisticsRecord[GAME_PLAYER];	//统计记录最近20局

	//组件变量
protected:
	HINSTANCE							m_hInst;
	IServerDebug*						m_pServerDebug;
	CGameLogic							m_GameLogic;							//游戏逻辑
	ITableFrame							* m_pITableFrame;						//框架接口
	tagGameServiceOption				* m_pGameServiceOption;					//配置参数
	tagGameServiceAttrib *				m_pGameServiceAttrib;					//游戏属性
	tagAIConfig							m_CustomAIConfig;						//默认AICONFIG

public:
	static CList<SYSCTRL, SYSCTRL&> m_listSysCtrl;							//通用库存调试列表
	static CList<ROOMCTRL, ROOMCTRL&> m_listRoomCtrl;						//房间调试列表
	static CList<AREACTRL, AREACTRL&> m_listAreaCtrl;						//区域调试列表

	CTime							m_tmStartRecord;						//开始记录时间
	WORD							m_wChairCount;							//椅子数目

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release();
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//复位接口
	virtual VOID RepositionSink();
	//配置接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

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
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) { return true; }
	//扣除服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//游戏事件
protected:
	//加注事件
	bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, SCORE lJettonScore);
	//用户占位
	bool OnUserOccupySeat(WORD wOccupyChairID, BYTE cbOccupySeatIndex);
	//申请庄家
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//取消申请
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);

	//辅助函数
private:
	//发送扑克
	bool DispatchTableCard();
	//发送庄家
	void SendApplyUser(IServerUserItem *pServerUserItem);
	//更换庄家
	bool ChangeBanker(bool bCancelCurrentBanker);
	//发送记录
	void SendGameRecord(IServerUserItem *pIServerUserItem, BYTE cbGameStatus);
	//是否衰减
	bool NeedDeductStorage();
	// 记录函数
	void WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString );
	//发送下注信息
	void SendUserBetInfo(DWORD dwCurAllScoreGameID);
	//发送房间用户信息
	void SendRoomUserInfo();

	//下注计算
private:
	//最大下注
	SCORE GetMaxPlayerScore( BYTE cbJettonArea, WORD wChairID );

	//游戏统计
private:
    //获取数值
    BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//游戏结束计算
	SCORE GameOver();
    //计算分数
	SCORE Calculate(BYTE cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker, BYTE cbWinAre[3]);
	//计算得分
    SCORE CalculateScore(OUT SCORE& lBankerWinScore);
	//推断赢家
	void DeduceWinner(OUT INT nAreaWin[AREA_MAX], OUT BYTE& cbResult);
	//读取配置
	void ReadConfigInformation(bool bReadFresh);

	//辅助函数
public:
	//获取玩家控制类型
	EM_CTRL_TYPE GetUserCtrlType();
	//更新控制结果
	void UpdateCtrlRes(EM_CTRL_TYPE emCtrlType);
	//初始化全局配置
	void InitGlobalConfig();
	//发送消息
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
    void SaveData();
    //重设机器人占位参数
    void RectifyRobotOccupySeatPara();
    //除去庄家和占座玩家和自己的有效玩家
    bool FilterUser(WORD wMeChairID, WORD wChairID);

	//查找房间控制
	POSITION FindRoomCtrl(DWORD dwSelCtrlIndex);

	//读取AIConfig
	void ReadAIConfig();
	//工作目录
	bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);
	//读取数据
	SCORE ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault);
	//获取真人玩家个数
	LONG GetRealPlayerCount();
	//获取机器人占位人数
	LONG GetRandRobotOccupySeatCount();
	//获取机器人上庄人数
	LONG GetRandRobotRequestBankerCount();
	//获取两个数之间的随机数
	DWORD GetRandVal(DWORD dwVal1, DWORD dwVal2);
	//重设机器人上庄参数
	void RectifyRobotRequestBankerPara();
	//获取申请列表机器人数目
	LONG GetApplyUserArrayRobotCount();
	//校验机器人下注是否有效
	bool IsRobotPlaceJettonValid(BYTE cbJettonArea, SCORE lJettonScore, WORD wChairID);
	//获取下注额比值
	LONG GetPlaceJettonLitPercent(LONGLONG lElaseTm);
	//房间同时存在AI和真人玩家
	bool IsContainAIAndRealPlayer(bool bCurPlaceAndroidUser);
	//区域控制有效
	bool IsAreaCtrlSysValid();
	//统计房间控制列表
	SCORE CalListSysCtrlSysWin();
	//统计房间控制列表
	SCORE CalListRoomCtrlSysWin();
	//统计区域控制列表
	SCORE CalListAreaCtrlSysWin();

	//大联盟支持
public:
	//是否大联盟金币约战房间
	bool IsGroupPersonalRoom();
	//获取大联盟房间规则
	void GetGroupPersonalRoomRule();
};

//////////////////////////////////////////////////////////////////////////////////

#endif