#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "GameFrameViewDexter.h"

//////////////////////////////////////////////////////////////////////////////////

// 中奖信息
struct tagAwardInfo 
{
	tchar						szPlayName[40];											// 玩家名字
	int							nMultipleIndex;											// 中奖信息
	__int64						nAwardScore;											// 中奖分数
};

// 游戏引擎
class CGameClientEngine : public CGameFrameEngine, public IClientControlCallback
{
	// 信息变量
protected:
	bool							m_bLegalOne;						// 标识
	bool							m_bLagalTwo;						// 标识

	// 操作信息
protected:
	bool							m_bPlayGame;						// 游戏中
	bool							m_bPlayHosted;						// 玩家托管

	// 下注信息
protected:
	int								m_nBetIndex;						// 下注索引
	int								m_nBetScore[BET_INDEX_MAX];			// 下注数组
	bool							m_bBetLine[LINE_INDEX_MAX];			// 下注线数

	// 赢信息
protected:
	longlong						m_lPlayWinScore;						// 玩家赢分数
	int								m_nPlayTaskScore;						// 任务分数
	int								m_nPlayMultipleIndex;					// 玩家中奖索引
	bool							m_bMagnification;						// 大倍分数
	bool							m_bTaskComplete[TASK_INDEX_MAX];		// 任务完成
	bool							m_bPlayWinLine[LINE_INDEX_MAX];			// 玩家赢线数
	int								m_nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];// 元素信息

	// 记录信息
protected:
	CWHArray< tagAwardInfo >		m_ArrayAwardInfo;					// 中奖信息

	// 组件变量
protected:
	CGameClientView	&				m_GameClientView;					// 游戏窗口
	CGameFrameViewDexter			m_GameFrameViewDexter;				// 窗口框架

	// 控制组件
public:
	hinstance						m_hControlInst;
	IClientControl *				m_pIClientControl;

	// 函数定义
public:
	// 构造函数
	CGameClientEngine();
	// 析构函数
	virtual ~CGameClientEngine();

	// 系统事件
public:
	// 创建函数
	virtual bool OnInitGameEngine();
	// 重置函数
	virtual bool OnResetGameEngine();

	// 时钟事件
public:
	// 时钟删除
	virtual bool OnEventGameClockKill(WORD wChairID);
	// 时钟信息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);

	// 游戏事件
public:
	// 旁观消息
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	// 游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	// 场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	// 控制函数
public:
	// 控制信息
	virtual bool SendControlMessage( uint nMessageID, void * pData, uint nSize );

	// 系统消息
public:
	// 定时器消息
	afx_msg void OnTimer(UINT nIDEvent);

	// 服务器消息
public:
	// 元素消息
	bool OnSubElementStop( VOID * pData, WORD wDataSize );
	// 彩池消息
	bool OnSubLotteryPool( VOID * pData, WORD wDataSize );
	// 任务信息
	bool OnSubTaskInfo( VOID * pData, WORD wDataSize );
	// 中奖信息
	bool OnSubAwardInfo( VOID * pData, WORD wDataSize );
	//控制消息
	bool OnSubControl( VOID * pData, WORD wDataSize );

	// 消息函数
public:
	// 旋转结束
	LRESULT OnMessageTurnOver( WPARAM wParam, LPARAM lParam );
	// 开始消息
	LRESULT OnMessageStartGame( WPARAM wParam, LPARAM lParam );
	// 停止消息
	LRESULT OnMessageStopGame( WPARAM wParam, LPARAM lParam );
	// 开始托管
	LRESULT OnMessageStartHosted( WPARAM wParam, LPARAM lParam );
	// 停止托管
	LRESULT OnMessageStopHosted( WPARAM wParam, LPARAM lParam );
	// 分数减少
	LRESULT OnMessageScoreLess( WPARAM wParam, LPARAM lParam );
	// 分数增加
	LRESULT OnMessageScoreAdd( WPARAM wParam, LPARAM lParam );
	// 分数最大
	LRESULT OnMessageScoreMax( WPARAM wParam, LPARAM lParam );
	// 线数选择
	LRESULT OnMessageLineCheck( WPARAM wParam, LPARAM lParam );
	// 打开控制
	LRESULT OnMessageOpenControl( WPARAM wParam, LPARAM lParam );

	// 声明消息
public:
	// 声明消息
	DECLARE_MESSAGE_MAP()

	
};

//////////////////////////////////////////////////////////////////////////////////

#endif