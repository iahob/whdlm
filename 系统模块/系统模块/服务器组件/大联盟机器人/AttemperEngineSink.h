#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "ServerListManager.h"
#include "DataBasePacket.h"
#include "AIManager.h"

//////////////////////////////////////////////////////////////////////////////////


//绑定参数
struct tagBindParameter
{
	DWORD					dwSocketID;									//网络标识
	WORD					wServiceID;									//服务标识
	WORD					wServiceType;								//服务类型
};


typedef CWHArray<DWORD> CArrayNewGroup;
//////////////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bCollectUser;						//汇总标志
	bool							m_bNeekCorrespond;					//协调标志
	bool							m_bNeekPrsnlRmCorrespond;		    //约战标志
	bool							m_bNeekLogServer;					//日志标志

	//控制变量
protected:
	DWORD							m_dwIntervalTime;					//间隔时间
	DWORD							m_dwLastDisposeTime;				//处理时间
	TimeChange						m_TimeChange;						//时间改变
	DWORD							m_dwGroupSocketID;					//大联盟SocketID
	CArrayNewGroup					m_arrayNewGroup;					//新增俱乐部

	//绑定信息
protected:
	tagBindParameter *				m_pNormalParameter;					//绑定信息


	//配置信息
protected:
	CInitParameter *				m_pInitParameter;					//配置参数

	//AI管理
protected:
	CAIManager						m_AIManager;						//AI管理


	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IAttemperEngine *				m_pIAttemperEngine;					//调度引擎
	ITCPSocketService *				m_pITCPSocketService;				//网络服务
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IGameServiceCustomTime *		m_pIGameServiceSustomTime;			//时间配置
	ITCPSocketService *				m_pPrsnlRmITCPSocketService;		//约战房网络服务器
	ITCPSocketService *				m_pLogServerTCPSocketService;		//日志服务引擎

	//比赛服务
public:	
	IPersonalRoomServiceManager		* m_pIPersonalRoomServiceManager;	//约战服务管理
	//数据引擎
public:
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//数据引擎
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//数据引擎
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //数据协调

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

	//消息接口

	//网络接口
public:
	//发送数据
	virtual bool SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送日志
	virtual void SendLogData(IServerUserItem * pIServerUserItem, const TCHAR * pszFormat, ...);
	//发送日志数据
	virtual void SendLogData(tagLogUserInfo LogUserInfo);
	//发送日志数据
	virtual void  SendLogData(const TCHAR * pszFormat, ...);
	//发送日志数据
	virtual void SendLogData(IServerUserItem * pIServerUserItem, const TCHAR szDescName[LEN_SERVER], const TCHAR * pszFormat);

	//功能接口
public:
	//断开协调
	virtual bool DisconnectCorrespond();	
	//插入用户
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem);



	//数据事件
protected:

	//AI参数
	bool OnDBGroupAIParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//AI参数结束
	bool OnDBGroupAIParameterEnd(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//加载AI玩家
	bool OnDBLoadAIUser(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//加载AI玩家结束
	bool OnDBLoadAIUserEnd(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//加载AI时间变化
	bool OnDBLoadAITimeChange(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//AI进入房间权重
	bool OnDBLoadAITimeWeightIn(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//AI离开俱乐部权重
	bool OnDBLoadAITimeWeightOut(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//AI操作结果
	bool OnDBAIOperateResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//清理AI结果
	bool OnDBClearAIResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	
	//网络事件
protected:
	//用户处理
	bool OnTCPNetworkService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务器注册
	bool OnTCPNetworkSubRegister(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//俱乐部服务
	bool OnTCPNetworkSubGroupGameRule(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//网站事件
	bool OnTCPSocketMainWebMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//俱乐部服务注册
	bool OnTCPNetworkSubGroupRegister(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//更新俱乐部玩法
	bool OnTCPNetworkSubUpdateGroupGameRule(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//更新俱乐部玩家分数
	bool OnTCPNetworkSubUpdateGroupUserScore(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//AI网络事件
protected:
	//AI动作
	bool OnTCPNetworkAIAction(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//AI 参数
	bool OnTCPNetworkSubAIParameter(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//房间创建成功
	bool OnTCPNetworkSubAIPrsnlRoomCreateSuccess(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//AI 进入房间结果
	bool OnTCPNetworkSubAILogonResult(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//更新 AI 状态
	bool OnTCPNetworkSubAIUpdateStatus(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//更新 AI 桌子数量
	bool OnTCPNetworkSubAITableChange(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//AI坐下失败
	bool OnTCPNetworkSubAISitFailure(VOID * pData, WORD wDataSize, DWORD dwSocketID);


	//执行功能
protected:
	//分配用户
	bool PerformDistribute();	

	//发送函数
protected:
	//用户信息
	bool SendUserInfoPacket(IServerUserItem * pIServerUserItem,DWORD dwSocketID,bool bSendAndroidFalg=false);


	//辅助函数
public:
	//绑定参数
	virtual tagBindParameter * GetBindParameter(WORD wBindIndex);

	//辅助函数
protected:
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif