#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//游戏桌子类
#ifndef __SPECIAL___
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
#else
class CTableFrameSink : public ITableFrameSink, public ITableUserAction,public ITableUserActionEX
#endif
{
	//用户信息
protected:
	LONGLONG						m_lUserStartScore[GAME_PLAYER];		//起始分数
	//五星宏辉下注
public:
	LONGLONG						m_lAreaScore[enAreaCount];				//所有区域下注
	LONGLONG						m_lAreaUserScore[GAME_PLAYER][enAreaCount];	//每个人的下注

	BYTE							m_cbCardValue;							//当前局扑克值
	BYTE							m_cbHistoryRcord[MAX_GAMECOUNT];			//历史记录
	BYTE							m_cbBigCount;							//第几轮
	BYTE							m_cbSmallCount;							//第几局
	BYTE							m_cbLuDanSmallCount;                    //小路单局数
	BYTE                            m_cbLuDanSmallData[LU_DAN_COUNT_SMALL];//小路单扑克
	int                             m_wCardTypeCount[5];                    //统计

	BYTE                            m_SrcHeiTao;//
	BYTE                            m_SrcHongTao;//
	BYTE                            m_SrcCaoHua;
	BYTE                            m_SrcFanPian;
	BYTE                            m_SrcKing;
	
	//库存控制
	LONGLONG						m_lStorageStart;						//库存数值
	LONGLONG						m_lStorageCurrent;						//库存数值
	int								m_nStorageDeduct;						//扣取比例
	LONGLONG						m_lStorageMax1;							//库存封顶1
	LONGLONG						m_lStorageMul1;							//系统输钱概率
	LONGLONG						m_lStorageMax2;							//库存封顶2
	LONGLONG						m_lStorageMul2;							//系统输钱概率
	//玩家成绩
protected:
	LONGLONG						m_lUserWinScore[GAME_PLAYER];			//玩家成绩
	LONGLONG						m_lUserReturnScore[GAME_PLAYER];		//返回下注
	LONGLONG						m_lUserRevenue[GAME_PLAYER];			//玩家税收
	LONGLONG						m_lUserEndPoint[GAME_PLAYER];			//玩家结束分数
	//状态变量
protected:
	DWORD							m_dwJettonTime;							//开始时间
	LONGLONG						m_lAreaLimitScore;						//区域限制
	LONGLONG						m_lUserLimitScore;						//个人限制
	INT								m_nEndGameMul;							//提前开牌百分比
	//庄家信息
protected:
	//坐庄控制
	LONGLONG						m_lApplyBankerCondition;				//申请条件
	int								m_nBankerTimeLimit;						//次数限制
	LONGLONG						m_lPlayerBankerMAX;						//玩家最大庄家数
	int								m_nBankerTimeAdd;						//增加次数 (金币大于其他玩家时)
	LONGLONG						m_lExtraBankerScore;					//额外条件 (大于此值时可以无视条件额外坐庄)
	int								m_nExtraBankerTime;						//额外次数

	CWHArray<WORD>					m_ApplyUserArray;						//申请玩家
	WORD							m_wCurrentBanker;						//当前庄家
	WORD							m_wOfflineBanker;						//离线庄家
	WORD							m_wBankerTime;							//做庄次数
	LONGLONG						m_lBankerScore;							//
	LONGLONG						m_lBankerWinScore;						//累计成绩
	LONGLONG						m_lBankerCurGameScore;					//当前成绩
	bool							m_bEnableSysBanker;						//系统做庄
	//超级抢庄
	SUPERBANKERCONFIG				m_superbankerConfig;					//抢庄配置
	CURRENT_BANKER_TYPE				m_typeCurrentBanker;					//当前庄家类型
	WORD							m_wCurSuperRobBankerUser;				//当前超级抢庄玩家
	//占位
	OCCUPYSEATCONFIG				m_occupyseatConfig;									//占位配置
	WORD							m_wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子I
	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	//ITableFrameControl			* m_pITableFrameControl;				//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数
	//机器人控制
	int								m_nMaxChipRobot;						//最大数目 (下注机器人)
	int								m_nChipRobotCount;						//人数统计 (下注机器人)
	LONGLONG						m_lRobotAreaLimit;						//区域统计 (机器人)
	LONGLONG						m_lRobotAreaScore[enAreaCount];		    //区域统计 (机器人)
	int								m_nRobotListMaxCount;					//最多人数
public:
	//static const enStartMode		m_GameStartMode;						//开始模式
	//控制变量
protected:
	TCHAR							m_szGameRoomName[SERVER_LEN];			//房间名称
	//long double					m_StorageStart;							//房间启动每桌子的库存数值，读取失败按 0 设置
	//long double					m_StorageDeduct;						//每局游戏结束后扣除的库存比例，读取失败按 1.00 设置
	BYTE							m_cbExcuteTimes;				        //执行次数
	BYTE							m_cbControlStyle;				        //控制方式
	BYTE							m_cbWinAreaCount;                       //赢区计数 
	bool							m_bWinArea[enAreaCount];		        //赢家区域
	//机器人限制
	LONGLONG                        m_nRobotBankerCount;                    //机器人总下注限制
	DWORD                           m_dwStartIn[5][20];                     //记录20局黑桃红桃草花方片王牌点数
	LONGLONG                        m_nRobotRegionLimit[enAreaCount];       //机器人区域限制
	DWORD							m_dwCount;						        //记录数目20
	//时间设置
protected:
	BYTE							m_cbFreeTime;							//空闲时间
	BYTE							m_cbBetTime;							//下注时间
	BYTE							m_cbEndTime;							//结束时间
	DWORD                           m_TodayTickCount;                       //= GetTodayTickCount(SystemTime);//判断当前时间
protected:
	BYTE                           m_CardHeiTao;                            //黑桃
	BYTE                           m_CardHongTao;                           //红桃
	BYTE                           m_CardCaoHua;                            //草花
	BYTE                           m_CardFanPian;                           //方片
	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() {}
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual void RepositionSink();
    virtual bool IsUserPlaying(WORD wChairID);

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//数据事件
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize){return true;}

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//查询服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){return;}

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

public:

	void WriteDataToInI(int m_SortID);

    BYTE ReadDataFromInI(int m_SortID);

#ifdef __SPECIAL___
	//银行操作
public:
	virtual bool __cdecl OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem);////
#endif

	//游戏事件
protected:
	//五星宏辉 加注
	bool OnUserPoint(WORD wChairID, const VOID * pData, WORD wDataSize);
	//控制信息
	VOID ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption);
	// 记录函数
	VOID WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);
	//辅助函数
private:
	//发送消息
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
	//读取配置
	void ReadConfigInformation(int a);
	//是否衰减
	bool NeedDeductStorage();
	//发送下注信息
	void SendUserBetInfo(IServerUserItem *pIServerUserItem);
	//发送消息
	void SendPlaceBetFail(WORD wChairID, BYTE cbBetArea, LONGLONG lBetScore, bool Again);
	//机器人智能下注
	void AiAndroidRandom();
	//转换时间
	DWORD GetTodayTickCount(SYSTEMTIME & SystemTime);
};
//////////////////////////////////////////////////////////////////////////

#endif
