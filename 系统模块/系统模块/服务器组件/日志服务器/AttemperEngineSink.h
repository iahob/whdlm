#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "GlobalInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////
//数据定义

//服务类型
enum enServiceKind
{
	ServiceKind_None,				//无效服务
	ServiceKind_Game,				//游戏服务
	ServiceKind_Plaza,				//广场服务
	ServiceKind_Chat,				//好友服务
	ServiceKind_Club,				//群组服务
};

//绑定参数
struct tagBindParameter
{
	//网络数据
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwActiveTime;						//激活时间

	//附加数据
	WORD							wServiceID;							//服务标识
	enServiceKind					ServiceKind;						//服务类型
};

//数组定义
typedef CWHArray<WORD> CWordArrayTemplate;

//////////////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	WORD							m_wCollectItem;						//汇总连接
	CWordArrayTemplate				m_WaitCollectItemArray;				//汇总等待

	//变量定义
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//辅助数组

	//组件变量
protected:
	CGlobalInfoManager				m_GlobalInfoManager;				//全局管理

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎

	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//异步接口
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//事件接口
public:
	//控制事件
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//自定事件
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//内核事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//连接事件
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//网络事件
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//网络事件
protected:
	//注册服务
	bool OnTCPNetworkMainLog(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);


	//日志控制事件
public:
	//关闭登录服务器日志
	VOID CloseLogonServerLog(byte cbClose);
	//关闭游戏服务器日志
	VOID CloseGamedServerLog(byte cbClose);
	//关闭聊天服务器日志
	VOID CloseChatServerLog(byte cbClose);
	//关闭俱乐部日志
	VOID CloseGroupServerLog(byte cbClose);
};

//////////////////////////////////////////////////////////////////////////////////

#endif