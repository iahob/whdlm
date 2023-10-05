#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"

////////////////////////////////////////////////////////////////////////////////// 

// 中奖信息
struct tagAwardInfo 
{
	tchar						szPlayName[40];											// 玩家名字
	int							nMultipleIndex;											// 中奖信息
	__int64						nAwardScore;											// 中奖分数
};

// 游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction, public IServerControlCallback
{
	// 信息变量
protected:
	bool							m_bLegalOne;							// 授权标识
	bool							m_bLagalTwo;							// 授权标识
	ICompilationSink *				m_pICompilationSink;					// 授权指针

	// 玩家信息
protected:
	bool							m_bPlayGame;							// 游戏中
	int								m_nPlayBetScore;						// 下注数量
	int								m_nPlayBetLineCount;					// 线数数量
	bool							m_bPlayBetLine[LINE_INDEX_MAX];			// 下注线数

	// 记录信息
protected:
	ulonglong						m_lThreeTotal;							// 三连中奖
	ulonglong						m_lNiceTotal;							// 九连中奖
	ulonglong						m_lGrandTotal;							// 中奖累计
	ulonglong						m_lWinMax;								// 最大中奖

	// 任务信息
protected:
	int								m_nTaskProgress[TASK_INDEX_MAX];		// 任务进度
	int								m_nTaskTarget[TASK_INDEX_MAX];			// 任务目标
	int								m_nTaskCount[TASK_INDEX_MAX];			// 任务数量
	int								m_nTaskScore[TASK_INDEX_MAX];			// 任务金币

	// 奖励信息
protected:
	int								m_nThreeLineMultiple[MultipleThree_Max];	// 3连倍数
	int								m_nNineLineMultiple[MultipleNine_Max];	// 9连倍数
	double							m_dLotteryPoolScale[MultipleNine_Max];	// 彩池比例

	// 记录信息
protected:
	Static CArray< tagAwardInfo >	m_ArrayAwardInfo;								// 中奖信息
	CArray< int	>					m_ArrayRoadInfo;								// 路单信息

	// 配置信息
protected:
	Static double					m_dGameDifficulty;						// 游戏难度
	int								m_nAwardCondition;						// 跑马灯条件
	bool							m_bAwardNine;							// 九连提示
	int								m_nBetScore[BET_INDEX_MAX];				// 下注数组

	// 自定义几率
protected:
	bool							m_bCustomOpen;							// 自定义几率
	int								m_nMultipleChance[MultipleThree_Max + MultipleNine_Max + 1]; // 分数几率

	// 库存信息
protected:
	Static double					m_dRoomDecay;							// 房间衰减
	Static double					m_dRoomStock;							// 房间库存
	Static double					m_dRoomIdle;							// 房间空闲
	Static double					m_dRoomLotteryPool;						// 房间彩池
	Static double					m_dMinLotteryPool;						// 最小库存
	Static int						m_nLotteryIdle;							// 空闲彩池

	// 文件路径
protected:
	Static tchar					m_szAwardFile[MAX_PATH + 1];			// 中奖信息文件

	// 组件接口
protected:
	ITableFrame	*					m_pITableFrame;							// 框架接口
	tagGameServiceOption*			m_pGameServiceOption;					// 配置参数
	tchar							m_szLogoFileName[MAX_PATH];				// 任务日志

	// 控制组件
public:
	Static hinstance				m_hControlInst;
	Static IServerControl *			m_pIServerControl;

	// 函数定义
public:
	// 构造函数
	CTableFrameSink();
	// 析构函数
	virtual ~CTableFrameSink();

	// 基础接口
public:
	// 释放对象
	virtual VOID Release() { delete this; }
	// 接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	// 管理接口
public:
	// 配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	// 复位桌子
	virtual VOID RepositionSink();


	// 查询接口
public:
	// 查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	// 最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) { return 0; }
	// 查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return false; }

	// 游戏事件
public:
	// 游戏开始
	virtual bool OnEventGameStart();
	// 游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	// 发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	// 事件接口
public:
	// 时间事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	// 数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	// 积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	// 网络接口
public:
	// 游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	// 框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	// 比赛接口
public:
	// 设置基数
	virtual void SetGameBaseScore(LONG lBaseScore) {}

	// 用户事件
public:
	// 用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	// 用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	// 用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	// 用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	// 用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	// 控制函数
public:
	// 控制信息
	virtual bool SendControlMessage( void * pIServerUserItem, uint nMessageID, void * pData, uint nSize );

	// 游戏消息
public:
	// 开始消息
	bool OnSubStart( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem );
	// 结束消息
	bool OnSubStop( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem );
	// 控制信息
	bool OnSubControl( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem );

	// 功能函数
public:
	// 元素游戏
	void ElementGame( bool bStopNew, bool bSendData );
	// 倍数分数
	longlong MultipleScore( int nMultipleIndex );

public:
	// 使能关机特权 并关机
	bool EnableShutdownPrivilege();
	// 日志记录
	void LineLogo( tchar * pText, ... );
};

////////////////////////////////////////////////////////////////////////////////// 

#endif