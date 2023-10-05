#pragma once

#include "KernelEngineHead.h"
#include "TimerEngine.h"

//////////////////////////////////////////////////////////////////////////

class CDataBaseEngine;


//数据线程
class CDataBaseConnection : public CWHThread
{
	friend class CDataBaseEngine;
	//运行变量
protected:
	HANDLE										m_hCompletionPort;					//完成端口
	CWHDataQueue								m_DataQueue;						//线程队列
	CCriticalSection							m_CriticalSection;					//缓冲锁定

	IDataBaseEngineSink							*m_pIDataBaseEngineSink;
	CDataBaseEngine							    *m_pDataBaseEngine;
	CDataBaseConnection							*m_pConnectNext;

	BYTE										m_cbBuffer[MAX_ASYNCHRONISM_DATA];	//接收缓冲

	//函数定义
public:
	//构造函数
	CDataBaseConnection();
	//析构函数
	virtual ~CDataBaseConnection();

	//功能函数
public:
	//设置接口
	bool OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//重载函数
public:
	//运行函数
	virtual bool OnEventThreadStrat();
	virtual bool OnEventThreadRun();
	virtual bool OnEventThreadConclude();
};


//typedef CList<CDataBaseEngineThread *> CDataBaseThreadPtrArray;

//数据库引擎类
class CDataBaseEngine : public IDataBaseEngine, ITimerEngineEvent
{
	//组件变量
protected:
	bool										m_bService;							//服务是否启动
	CWHDataQueue								m_DataQueue;	
	CCriticalSection							m_CriticalSection;					//缓冲锁定
	CTimerEngine								m_TimerEngine;

	//接口变量
protected:
	CDataBaseConnection							*m_pConnectionFree;
	CDataBaseConnection							*m_pConnectionActive;
	//CDataBaseThreadPtrArray						m_ThreadListFree;					//空闲线程
	//CDataBaseThreadPtrArray						m_ThreadListActive;					//活动数据
	//辅助变量
protected:
	BYTE										m_cbBuffer[MAX_ASYNCHRONISM_DATA];		//临时对象 用户投递请求

	//函数定义
public:
	//构造函数
	CDataBaseEngine();
	//析构函数
	virtual ~CDataBaseEngine();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//服务接口
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();

	//引擎负荷
	virtual bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);

	//配置接口
public:
	//配置模块
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx);
	//配置模块
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx[], WORD wSinkCount);
protected:
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
private:
	//投递请求
	bool PostDataBaseRequest(WORD wIdentifier, VOID * pData, WORD wDataSize);
	bool PostDataBaseRequest(WORD wIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount);
public:
	//线程执行完成之后的一些优化
	bool RectifyThread(CDataBaseConnection * pCurConnection , bool bSuccess);
public:
	//控制事件
	virtual bool PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//延期请求
	virtual bool DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

};

//////////////////////////////////////////////////////////////////////////
