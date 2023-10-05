#ifndef GAME_SERVER_MANAGER_HEAD_FILE
#define GAME_SERVER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

//游戏管理
#ifdef VER_IGameServiceCustomTime
class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomRule, public IGameServiceCustomTime
#else
class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomRule
#endif 
{
	//控件变量
protected:
	CDlgCustomRule						m_DlgCustomRule;				//自定规则
	virtual bool DefaultServerLevel(LPBYTE pcbCustomRule, WORD wCustonSize, tagGameServerLevelInfo & GameServerLevelInfo, WORD wCurSelServerLevel) { return true; };

	//变量定义
protected:
	tagGameServiceAttrib				m_GameServiceAttrib;			//服务属性	
	HINSTANCE							m_hDllInstance;					//DLL 句柄

	//函数定义
public:
	//构造函数
	CGameServiceManager();
	//析构函数
	virtual ~CGameServiceManager();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//创建接口
public:
	//创建桌子
	virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer);
	//创建游戏AI
	virtual VOID * CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer);
	//创建数据
	virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer);
	//参数接口
public:
	//组件属性
	virtual bool GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
	//调整参数
	virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption);

	//配置接口
public:
	//获取配置
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//默认配置
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//创建窗口
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize);

#ifdef VER_IGameServiceCustomTime
	//参数接口
public:
	//机器脉冲
	virtual DWORD GetAndroidTimerPulse() { return 200; }
	//时间范围
	virtual DWORD GetTableSinkTimeRange() { return 200; }
	//时间单元
	virtual DWORD GetTimerEngineTimeCell() { return 200; }
#endif 
};

//////////////////////////////////////////////////////////////////////////////////

#endif