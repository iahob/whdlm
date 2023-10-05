#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

//平台定义
#include "..\..\全局定义\Platform.h"
#include "..\..\全局定义\RightDefine.h"


//消息定义
#include "..\..\消息定义\CMD_Commom.h"

//平台文件
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

#include "InitParameter.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"
//////////////////////////////////////////////////////////////////////////
//常量定义

#define LOGIC_COUNT					500										//逻辑数量
#define GROUP_COUNT					500										//群组数量
#define BATTLE_COUNT				500										//约战数量

//////////////////////////////////////////////////////////////////////////
//服务状态
enum enServiceStatus
{
	ServiceStatus_Stop,				//停止状态
	ServiceStatus_Config,			//配置状态
	ServiceStatus_Service,			//服务状态
};
//////////////////////////////////////////////////////////////////////////
//网络标识
#define NETWORK_CORRESPOND			1										//协调连接
#define NETWORK_BATTLE				2										//约战服务
#define NETWORK_LOG_SERVER			3										//日志服务器连接
#define NETWORK_AI_SERVER			6										//AI服务器连接

//网络标识
#define NETWORK_LOGIC_START			(NETWORK_CORRESPOND+1)					//逻辑连接
#define NETWORK_LOGIC_END			(NETWORK_LOGIC_START+LOGIC_COUNT)		//逻辑连接

//网络标识
#define NETWORK_GROUP_START			(NETWORK_LOGIC_END+1)					//群组连接
#define NETWORK_GROUP_END			(NETWORK_GROUP_START+GROUP_COUNT)		//群组连接

//网络标识
#define NETWORK_BATTLE_START		(NETWORK_GROUP_END+1)					//约战连接
#define NETWORK_BATTLE_END			(NETWORK_BATTLE_START+BATTLE_COUNT)		//约战连接
//////////////////////////////////////////////////////////////////////////////////
//消息定义
#define WM_UICONTROL_REQUEST		(WM_USER+100)						//控制请求
//////////////////////////////////////////////////////////////////////////////////
//状态接口
interface IServiceUnitsSink
{
	//接口定义
public:
	//服务状态
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus) = NULL;
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef CWHArray<DWORD>	CRoomIDArray;
typedef CMap<DWORD, DWORD, CRoomIDArray *, CRoomIDArray *> CMapGroupRoom;

//服务单元
class CServiceUnits : public CWnd
{
	//友元定义
	friend class CAttemperEngineSink;
	friend class CDataBaseEngineSink;

	//状态变量
protected:
	//tagAccessOption					m_AccessOption;						//接入配置
	CInitParameter					m_InitParameter;					//配置参数
	enServiceStatus					m_ServiceStatus;					//运行状态

	//服务组件
protected:
	CAttemperEngineSink				m_AttemperEngineSink;				//调度钩子
	CDataBaseEngineSink				m_DataBaseEngineSink[4];			//数据钩子

	//组件变量
private:
	CWHDataQueue					m_DataQueue;						//数据队列
	CCriticalSection				m_CriticalSection;					//同步对象

	//内核组件
protected:
	CTimerEngineHelper				m_TimerEngine;						//时间引擎
	CAttemperEngineHelper			m_AttemperEngine;					//调度引擎
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//网络引擎
	CTCPSocketServiceHelper			m_TCPSocketCorrespond;				//协调服务
	CTCPSocketServiceHelper			m_TCPSocketBattle;					//状态服务	
	CDataBaseEngineHelper			m_DataBaseEngine;					//数据引擎
	CTCPSocketServiceHelper			m_LogServerTCPSocketService;		//日志服务
	CTCPSocketServiceHelper			m_pAITCPSocketService;				//AI服务引擎
	CMapGroupRoom					m_Map;
	//接口变量
protected:
	IServiceUnitsSink *				m_pIServiceUnitsSink;				//状态接口

	//静态变量
public:
	static CServiceUnits *			g_pServiceUnits;					//对象指针

	//函数定义
public:
	//构造函数
	CServiceUnits();
	//析构函数
	virtual ~CServiceUnits();

	//信息函数
public:
	//获取状态
	virtual enServiceStatus __cdecl GetServiceStatus() { return m_ServiceStatus; }

	//服务控制
public:
	//启动服务
	virtual bool __cdecl StartService();
	//停止服务
	virtual bool __cdecl ConcludeService();
	//设置接口
	virtual bool __cdecl SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink);
	//投递请求
	virtual bool __cdecl PostControlRequest(WORD wIdentifier, VOID *pData, WORD wDataSize);

	//辅助函数
protected:
	//配置组件
	bool InitializeService();
	//启动内核
	bool StartKernelService();
	//启动网络
	bool StartNetworkService();
	//启动数据库
	bool StartDataBaseService();
	//内部函数
private:
	//设置状态
	bool SetServiceStatus(enServiceStatus ServiceStatus);
	//发送控制
	bool SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize);

	//功能函数
public:
	//配置参数
	const CInitParameter * GetInitParameter() const { return &m_InitParameter; }

	//消息映射
protected:
	//控制消息
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif