#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "GameFrameViewDexter.h"
#include "UserActionItem.h"


//////////////////////////////////////////////////////////////////////////

typedef CWHArray<tagUserAction *> CUserActionArray;				//操作项目

//////////////////////////////////////////////////////////////////////////


// 游戏引擎
class CGameClientEngine : public CGameFrameEngine
{
	// 信息变量
protected:
	bool							m_bLegalOne;						// 合法标识
	bool							m_bLagalTwo;						// 合法标识

public:
	WORD							m_wClockID;							//时间标识
	WORD							m_wClockCount;						//剩下时间


	//游戏变量
protected:
	bool							m_bShuoMing;								//显示说明
	bool							m_bAutoStart;								//自动游戏
	SCORE							m_lSaveScore;								//保存分数
	BYTE							m_cbGameMode;								//游戏模式
	SCORE							m_lCellScore;								//单元底分
	SCORE							m_lBet;										//下注分数
	BYTE							m_cbBetCount;								//下注数量
	SCORE							m_lBetScore[8];								//下注大小
	SCORE							m_lLine;									//押线数量
	SCORE							m_lOneGameScore;							//游戏得分
	SCORE							m_lTwoGameScore;							//游戏得分
	SCORE							m_lThreeGameScore;							//游戏得分
	BYTE							m_cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//开奖信息
	CPoint							m_ptZhongJiang[ITEM_COUNT][ITEM_X_COUNT];	//中奖位置
	bool							m_bWaitOpenOne;								//等待开奖

	//BYTE							m_cbThreeItemInfo[4];						//开奖信息
	bool							m_bWaitOpenTwo;								//等待下注
	bool							m_bWaitBetTwo;									//等待下注
	bool							m_bOpenTwo;									//开始开奖
	BYTE							m_cbOpenTwoMode;							//比倍模式

	bool							m_bWaitOpenThree;								//等待下注
	bool							m_bWaitBetThree;									//等待下注
	bool							m_bOpenThree;									//开始开奖

	//动作变量
protected:
	CUserActionArray				m_UserActionArray;					//操作项目
	CUserActionArray				m_UserActionStorage;				//操作项目

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑

	// 组件变量
protected:
	CGameClientView	&				m_GameClientView;					// 游戏窗口
	CGameFrameViewDexter			m_GameFrameViewDexter;				// 窗口框架

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
	// 声音控制
	virtual bool AllowBackGroundSound(bool bAllowSound);

	// 游戏事件
public:
	// 旁观消息
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	// 游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	// 场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	// 用户事件
public:
	// 用户进入
	virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser);
	// 用户离开
	virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser);
	
	// 系统消息
public:
	// 定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//删除时钟
	VOID KillGameClockEx(WORD wClockID);
	//设置时钟
	VOID SetGameClockEx(WORD wChairID, WORD wClockID, UINT nElapse);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubTwoStart(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubThreeStart(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubThreeKaiJian(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubThreeEnd(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubGameConclude(VOID * pData, WORD wDataSize);

	//消息映射
protected:
	//游戏关闭
	LRESULT OnMessageGameClose(WPARAM wParam, LPARAM lParam);
	//游戏设置
	LRESULT OnMessageGameOption(WPARAM wParam, LPARAM lParam);
	//游戏操作
	LRESULT OnMessageGameOperate(WPARAM wParam, LPARAM lParam);
	//加减下注
	LRESULT OnMessageGameBet(WPARAM wParam, LPARAM lParam);
	//控制
	LRESULT OnAdminControl(WPARAM wParam, LPARAM lParam);
	//聊天
	LRESULT OnChatMessage( WPARAM wParam, LPARAM lParam );
	//游戏声音
	LRESULT OnMessageGameSound(WPARAM wParam, LPARAM lParam);	
	//操作完成
	LRESULT OnUserActionFinish(WPARAM wParam, LPARAM lParam);
	//操作完成
	LRESULT OnUserActionFinishSteps(WPARAM wParam, LPARAM lParam);

public:
	//进入说明
	VOID EnterShuoMing();
	//进入押线
	VOID EnterOne();
	//进入比倍
	VOID EnterTwo();
	//进入比倍
	VOID EnterThree();
	//播放声音
	VOID PlaySoundEx(CString strSound);

	//动作操作函数
protected:
	//开始玩家动作
	bool BeginUserAction();
	//停止玩家动作
	bool StopUserAction();
	//移除当前动作
	bool RemoveCurrentAction();
	//激活动作
	tagUserAction *ActiveUserAction(enmActionKind enActionKind);
	//押线开牌
	bool BeginActionOneGunDong();
	//押线
	bool BeginActionOneYaXian();
	//开奖
	bool BeginActionOneKaiJian();
	//等待开奖
	bool BeginActionTwoWaitBet();
	//开奖
	bool BeginActionTwoKaiJian();
	//开始
	bool BeginActionThreeGunDong();
	//开奖完成
	bool BeginActionThreeKaiJian();
	//开奖完成
	bool BeginActionThreeKaiJianEnd();
	//开奖完成
	bool BeginActionThreeEnd();
	//游戏结束
	bool BeginActionGameConclude();
	//游戏声音
	bool BeginActionEnter();
	//押线开牌完成
	bool OnActionOneGunDong();
	//押线完成
	bool OnActionOneYaXian();
	//开奖完成
	bool OnActionOneKaiJian();
	//等待开奖完成
	bool OnActionTwoWaitBet();
	//开奖完成
	bool OnActionTwoKaiJian();
	//开奖
	bool OnActionThreeGunDong();
	//开奖完成
	bool OnActionThreeKaiJian();
	//开奖完成
	bool OnActionThreeKaiJianEnd();
	//开奖完成
	bool OnActionThreeEnd();
	//游戏结束
	bool OnActionGameConclude();
	//游戏声音
	bool OnActionEnter();


	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////// 

#endif