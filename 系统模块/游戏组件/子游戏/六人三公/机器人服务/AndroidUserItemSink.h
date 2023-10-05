#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "../游戏服务器/GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//宏定义
#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#endif

//最大下注次数
#define MAX_CHIP_TIME								50
//////////////////////////////////////////////////////////////////////////

//机器人类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
protected:
	SCORE							m_lCellScore;
	WORD							m_wBankerUser;
	BYTE							m_cbPlayStatus[GAME_PLAYER];

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	IAndroidUserItem *				m_pIAndroidUserItem;				//用户接口
	//配置变量  (机器人配置)
protected:
	//tagRobotInfo					m_RobotInfo;						//全局配置
	TCHAR							m_szRoomName[32];					//配置房间
	LONGLONG						m_lCurGameCount;					//游戏次数
	//机器人存取款
	LONGLONG						m_lRobotScoreRange[2];				//最大范围
	LONGLONG						m_lRobotBankGetScore;				//提款数额
	LONGLONG						m_lRobotBankGetScoreBanker;			//提款数额 (庄家)
	int								m_nRobotBankStorageMul;				//存款倍数
	LONGLONG						m_lRobotLeaveGameMin;
	LONGLONG						m_lRobotLeaveGameMax;
	LONGLONG						m_lRobotCurLeaveMax;				//此时离开局数
	LONGLONG						m_lRobotGameEndLeaveMin;
	LONGLONG						m_lRobotGameEndLeaveMax;	
	LONGLONG						m_lRobotLeaveTableMin;				//机器人准备后离开桌子时间
	LONGLONG						m_lRobotLeaveTableMax;				//机器人准备后离开桌子时间
	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { }
	//是否有效
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CAndroidUserItemSink))?true:false; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	// 消息处理
protected:
	// 游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	// 游戏投注
	bool OnSubGameCoin(const void * pBuffer, WORD wDataSize);
	// 游戏发牌
	bool OnSubSendCard(const void* pBuffer, WORD wDataSize);
	// 看牌
	bool OnSubLookCard(const void* pBuffer, WORD wDataSize);
	// 游戏结束
	bool OnSubGameEnd(const void* pBuffer, WORD wDataSize);

	//开始叫庄
	bool OnSubCallBegin(const void * pBuffer, WORD wDataSize);
	//开始下注
	bool OnSubOnAddScore(const void * pBuffer, WORD wDataSize);


	//读取配置
	VOID ReadConfigInformation(TCHAR szFileName[], TCHAR szRoomName[], bool bReadFresh);
	LONGLONG GetPrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lDefault, LPCTSTR lpFileName);
};

//////////////////////////////////////////////////////////////////////////

#endif
