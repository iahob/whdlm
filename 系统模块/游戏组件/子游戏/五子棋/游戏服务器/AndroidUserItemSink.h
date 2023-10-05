#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//游戏变量
protected:
	WORD							m_wBlackUser;						//黑棋玩家
	WORD							m_wCurrentUser;						//当前用户
	BYTE							XPos;								//当前X位置
	BYTE							YPos;								//当前Y位置

//	BYTE							chess[15][15];						//棋盘

	int								win[2][572];						//该组合有几个棋子
	int								board[15][15];						//棋盘
	bool							ptab[15][15][572];					//玩家胜利组合
	bool							ctab[15][15][572];					//机器人胜利组合
	//规则变量
protected:
	bool							m_bRestrict;						//是否禁手
	bool							m_bTradeUser;						//是否对换
	bool							m_bDoubleChess;						//是否双打
	
	//控件变量
public:
	CGameLogic						m_GameLogic;						//游戏逻辑
	IAndroidUserItem *				m_pIAndroidUserItem;				//用户接口
	ITableFrame						* m_pITableFrame;					//框架接口
	
	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release() { }
	//是否有效
	virtual bool  IsValid() { return AfxIsValidAddress(this,sizeof(CAndroidUserItemSink))?true:false; }
	//接口查询
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool  RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool  OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool  OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//游戏消息
	virtual bool  OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//场景消息
	virtual bool  OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void  OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual void  OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual void  OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual void  OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual void  OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户放棋
	bool OnSubPlaceChess(const void * pBuffer, WORD wDataSize);
	//悔棋请求
	bool OnSubRegretReq(const void * pBuffer, WORD wDataSize);
	//悔棋失败
	bool OnSubRegretFaile(const void * pBuffer, WORD wDataSize);
	//悔棋结果
	bool OnSubRegretResult(const void * pBuffer, WORD wDataSize);
	//和棋请求
	bool OnSubPeaceReq(const void * pBuffer, WORD wDataSize);
	//和棋应答
	bool OnSubPeaceAnser(const void * pBuffer, WORD wDataSize);
	//交换对家
	bool OnSubBlackTrade(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//游戏棋谱
	bool OnSubChessManual(const void * pBuffer, WORD wDataSize);
	//放置棋子
	void SetChess(void);
	//设置放棋计时器
	void SetPlaceTimer(void);
	//初始数据
	void initGame();
	//机器人下棋
	void ComTurn();
	//棋盘上有多少棋子
	int  GetCount();
};

//////////////////////////////////////////////////////////////////////////

#endif
