#include "StdAfx.h"
#include "Math.h"
#include "DataBaseEngine.h"
#include "TraceServiceManager.h"
#include "TraceService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
/////////////////////////////////////////////////////////////////////////////

CDataBaseConnection::CDataBaseConnection()
{
	m_hCompletionPort = NULL;
	m_pIDataBaseEngineSink = NULL;
	m_pDataBaseEngine = NULL;
	m_pConnectNext = NULL;
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));
}

CDataBaseConnection::~CDataBaseConnection()
{
}

//数据处理函数
bool CDataBaseConnection::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{

	if(wIdentifier == EVENT_TIMER)		//时间事件
	{
		if (wDataSize != sizeof(NTY_TimerEvent)) return false;

		//变量定义
		NTY_TimerEvent * pTimerEvent = (NTY_TimerEvent *)pData;
		//处理
		return m_pIDataBaseEngineSink->OnDataBaseEngineTimer(pTimerEvent->dwTimerID, pTimerEvent->dwBindParameter);
	}
	else if (wIdentifier == EVENT_CONTROL)		//控制事件
	{
		if (wDataSize < sizeof(NTY_ControlEvent)) return false;

		//变量定义
		NTY_ControlEvent * pControlEvent = (NTY_ControlEvent *)pData;

		return m_pIDataBaseEngineSink->OnDataBaseEngineControl(pControlEvent->wControlID, ((BYTE*)pData)+sizeof(NTY_ControlEvent), wDataSize - sizeof(NTY_ControlEvent));

	}
	else if(wIdentifier == EVENT_DATABASE)	//数据事件
	{
		if (wDataSize < sizeof(NTY_DataBaseEvent)) return false;
		//变量定义
		NTY_DataBaseEvent * pDataBaseEvent = (NTY_DataBaseEvent *)pData;

		return m_pIDataBaseEngineSink->OnDataBaseEngineRequest(pDataBaseEvent->wRequestID, 
			pDataBaseEvent->dwContextID,
			((BYTE*)pData) + sizeof(NTY_DataBaseEvent),
			wDataSize - sizeof(NTY_DataBaseEvent));
	}

	return false;
}

//线程执行函数 用来从队列中获取数据并处理
bool CDataBaseConnection::OnEventThreadRun()
{

	//变量定义
	DWORD dwThancferred=0;
	OVERLAPPED * pOverLapped=NULL;
	CDataBaseEngine * pDataBaseEngine=NULL;

	//完成端口
	if (GetQueuedCompletionStatus(m_hCompletionPort,&dwThancferred,(PULONG_PTR)&pDataBaseEngine,&pOverLapped,INFINITE) 
		&& pDataBaseEngine != NULL)
	{
		//队列锁定
		CWHDataLocker ThreadLock(m_CriticalSection);

		//提取数据
		tagDataHead DataHead;
		m_DataQueue.DistillData(DataHead,m_cbBuffer,sizeof(m_cbBuffer));
		//队列解锁
		ThreadLock.UnLock();
		//处理
		bool bSuccess = OnAsynchronismEngineData(DataHead.wIdentifier, m_cbBuffer, DataHead.wDataSize);

		//调整线程
		pDataBaseEngine->RectifyThread(this, bSuccess);

		//处理结果
		return true;
	}
	return false;
}

//线程函数运行前回调  创建IO完成端口  并调用钩子回调函数
bool CDataBaseConnection::OnEventThreadStrat()
{
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

	if (m_hCompletionPort == NULL)
	{
		return false;
	}

	if (m_pDataBaseEngine)
	{
		//查询接口
		IUnknownEx * pIUnknownEx = QUERY_OBJECT_PTR_INTERFACE(m_pDataBaseEngine, IUnknownEx);
		//调用事件
		return m_pIDataBaseEngineSink->OnDataBaseEngineStart(pIUnknownEx);
	}
	else
	{
		return m_pIDataBaseEngineSink->OnDataBaseEngineStart(NULL);
	}
}

//线程关闭回调  清理完成端口 及 调用钩子回调
bool CDataBaseConnection::OnEventThreadConclude()
{
	//关闭完成端口
	if (m_hCompletionPort)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	if (m_pDataBaseEngine)
	{
		//查询接口
		IUnknownEx * pIUnknownEx = QUERY_OBJECT_PTR_INTERFACE(m_pDataBaseEngine, IUnknownEx);
		//调用事件
		return m_pIDataBaseEngineSink->OnDataBaseEngineConclude(pIUnknownEx);
	}
	else
	{
		return m_pIDataBaseEngineSink->OnDataBaseEngineConclude(NULL);
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngine::CDataBaseEngine()
{
	//设置变量
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));
	m_pConnectionFree = NULL;
	m_pConnectionActive = NULL;
	m_TimerEngine.SetTimerEngineEvent(QUERY_ME_INTERFACE(IUnknownEx));
}

//析构函数
CDataBaseEngine::~CDataBaseEngine()
{
}

//接口查询
VOID * CDataBaseEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(IDataBaseEngine,Guid,dwQueryVer);
	QUERYINTERFACE(ITimerEngineEvent, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngine,Guid,dwQueryVer);
	return NULL;
}

//启动服务
bool CDataBaseEngine::StartService()
{
	//开启定时器服务

	//遍历开启线程服务
	if (m_pConnectionFree != NULL && m_pConnectionActive == NULL)
	{
		m_bService = true;

		CDataBaseConnection *pConnection = m_pConnectionFree;

		while ( pConnection != NULL)
		{
			if (!pConnection->StartThread()) return false;
			pConnection = pConnection->m_pConnectNext;
		}

		//开启定时器
		if (!m_TimerEngine.StartService()) return false;

		//设置定时器
		m_TimerEngine.SetTimer(EVENT_TIMER, 60000, TIMES_INFINITY, 0);

		return true;
	}
	return false;
}

//停止服务
bool CDataBaseEngine::ConcludeService()
{
	m_bService = false;

	//停止定时器服务
	m_TimerEngine.ConcludeService();

	while (m_pConnectionFree)
	{
		CDataBaseConnection * pConnection = m_pConnectionFree;
		m_pConnectionFree = pConnection->m_pConnectNext;

		PostQueuedCompletionStatus(pConnection->m_hCompletionPort, 0, NULL, NULL);
		pConnection->ConcludeThread(INFINITE);

		SafeDelete(pConnection);

	}

	m_pConnectionActive = NULL;
	m_pConnectionFree = NULL;
	return true;
}

//引擎负荷
bool CDataBaseEngine::GetBurthenInfo(tagBurthenInfo & BurthenInfo)
{
	CWHDataLocker ThreadLock(m_CriticalSection);
	m_DataQueue.GetBurthenInfo(BurthenInfo);
	return true;
}

//配置模块
bool CDataBaseEngine::SetDataBaseEngineSink(IUnknownEx * pIUnknownEx)
{

	IDataBaseEngineSink *pIDataBaseEngineSink = NULL;

	if (m_pConnectionFree || m_pConnectionActive) return false;
	
	if(!pIUnknownEx || (pIDataBaseEngineSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IDataBaseEngineSink)) == NULL) return false;

	CDataBaseConnection* pConnection = new CDataBaseConnection();
	//当前对象指针
	pConnection->m_pDataBaseEngine = this;
	//钩子对象
	pConnection->m_pIDataBaseEngineSink = pIDataBaseEngineSink;

	pConnection->m_pConnectNext = m_pConnectionFree;
	m_pConnectionFree = pConnection;

	return false;
}

//配置模块
bool CDataBaseEngine::SetDataBaseEngineSink(IUnknownEx * pIUnknownEx[], WORD wSinkCount)
{
	if (m_pConnectionFree || m_pConnectionActive) return false;
	
	for (INT_PTR i = 0; i < wSinkCount; i++)
	{
		IDataBaseEngineSink *pIDataBaseEngineSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx[i], IDataBaseEngineSink);
		if (pIDataBaseEngineSink == NULL) return false;

		CDataBaseConnection* pConnection = new CDataBaseConnection();
		//当前对象指针
		pConnection->m_pDataBaseEngine = this;
		//钩子对象
		pConnection->m_pIDataBaseEngineSink = pIDataBaseEngineSink;

		pConnection->m_pConnectNext = m_pConnectionFree;
		m_pConnectionFree = pConnection;
	}

	return true;
}

bool CDataBaseEngine::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//这里有问题
	if (!m_bService) return false;

	NTY_TimerEvent pTimerEvent;
	pTimerEvent.dwTimerID = dwTimerID;
	pTimerEvent.dwBindParameter = dwBindParameter;

	PostDataBaseRequest(EVENT_TIMER, &pTimerEvent, sizeof(NTY_TimerEvent));

	return true;
}

bool CDataBaseEngine::PostDataBaseRequest(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	CWHDataLocker ThreadLock(m_CriticalSection);

	//这里选择合适的线程进行投递
	
	if (m_pConnectionFree)
	{
		CDataBaseConnection * pConnectionFree = m_pConnectionFree;
		CDataBaseConnection * pConnectionActive = m_pConnectionActive;

		m_pConnectionActive = pConnectionFree;
		pConnectionFree->m_pConnectNext = pConnectionActive;

		m_pConnectionFree = m_pConnectionFree->m_pConnectNext;
		//插入数据
		CWHDataLocker ThreadLock(pConnectionFree->m_CriticalSection);
		pConnectionFree->m_DataQueue.InsertData(wIdentifier, pData, wDataSize);
		PostQueuedCompletionStatus(pConnectionFree->m_hCompletionPort, 0, (ULONG_PTR)this, NULL);
	}
	else //如果没有合适的线程进行投递 那么这里就保存到当前的队列中 当有合适的线程后 再进行投递
	{
		m_DataQueue.InsertData(wIdentifier, pData, wDataSize);
	}
	return true;
}

bool CDataBaseEngine::PostDataBaseRequest(WORD wIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount)
{

	CWHDataLocker ThreadLock(m_CriticalSection);

	//这里选择合适的线程进行投递
	
	if (m_pConnectionFree)
	{
		CDataBaseConnection * pConnectionFree = m_pConnectionFree;
		CDataBaseConnection * pConnectionActive = m_pConnectionActive;

		m_pConnectionActive = pConnectionFree;
		pConnectionFree->m_pConnectNext = pConnectionActive;

		m_pConnectionFree = m_pConnectionFree->m_pConnectNext;

		CWHDataLocker ThreadLock(pConnectionFree->m_CriticalSection);
		pConnectionFree->m_DataQueue.InsertData(wIdentifier, DataBuffer, wDataCount);
		PostQueuedCompletionStatus(pConnectionFree->m_hCompletionPort, 0, (ULONG_PTR)this, NULL);
	}
	else //如果没有合适的线程进行投递 那么这里就保存到当前的队列中 当有合适的线程后 再进行投递
	{
		m_DataQueue.InsertData(wIdentifier, DataBuffer, wDataCount);
	}
	return true;
}

bool CDataBaseEngine::DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return true;
}

bool CDataBaseEngine::RectifyThread(CDataBaseConnection * pCurConnection, bool bSuccess)
{
	//锁定当前线程
	CWHDataLocker ThreadLock(m_CriticalSection);

	//线程函数数据处理完成后
	if (m_DataQueue.GetDataPacketCount())
	{
		//当线程处理处理完成 检查当前队列中是否有数据需要处理 如果有 则插入到线程队列
		//从引擎提取数据
		tagDataHead DataHead;
		m_DataQueue.DistillData(DataHead, m_cbBuffer, sizeof(m_cbBuffer));
		//锁定数据处理线程
		CWHDataLocker ThreadLock(pCurConnection->m_CriticalSection);
		//插入数据至线程队列
		pCurConnection->m_DataQueue.InsertData(DataHead.wIdentifier, m_cbBuffer, DataHead.wDataSize);
		//通知线程处理
		PostQueuedCompletionStatus(pCurConnection->m_hCompletionPort, 0, (ULONG_PTR)this, NULL);
	}
	else
	{
		CDataBaseConnection * pConnectionPrev = NULL;
		CDataBaseConnection * pConnectionFree = m_pConnectionFree;
		CDataBaseConnection * pConnection = NULL;
		for (pConnection = m_pConnectionActive; pConnection; pConnection = pConnection->m_pConnectNext)
		{
			if (pConnection == pCurConnection)
			{
				if (pConnectionPrev)
					pConnectionPrev->m_pConnectNext = pConnection->m_pConnectNext;
				else
					m_pConnectionActive = pConnection->m_pConnectNext;

				m_pConnectionFree = pConnection;
				pConnection->m_pConnectNext = pConnectionFree;

				break;
			}
			pConnectionPrev = pConnection;
		}
		
	}
	return true;
}


//控制事件
bool CDataBaseEngine::PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	if (!m_bService) return false;
	//效验参数

	//投递消息
	NTY_ControlEvent pControlEvent;
	pControlEvent.wControlID = wControlID;
	return PostDataBaseRequest(EVENT_CONTROL, &pControlEvent, sizeof(NTY_ControlEvent));


}

//处理事件
bool CDataBaseEngine::PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	if (!m_bService) return false;

	ASSERT((wDataSize + sizeof(NTY_DataBaseEvent)) <= MAX_ASYNCHRONISM_DATA);
	if ((wDataSize + sizeof(NTY_DataBaseEvent)) > MAX_ASYNCHRONISM_DATA) return false;

	NTY_DataBaseEvent pDataBaseEvent;
	pDataBaseEvent.wRequestID = wRequestID;
	pDataBaseEvent.dwContextID = dwContextID;
	
	if (wDataSize > 0)
	{
		tagDataBuffer pDataBuffer[2];

		pDataBuffer[0].pDataBuffer = &pDataBaseEvent;
		pDataBuffer[0].wDataSize = sizeof(NTY_DataBaseEvent);

		pDataBuffer[1].pDataBuffer = pData;
		pDataBuffer[1].wDataSize = wDataSize;

		PostDataBaseRequest(EVENT_DATABASE, pDataBuffer, 2);

	}
	else
	{
		PostDataBaseRequest(EVENT_DATABASE, &pDataBaseEvent, sizeof(NTY_DataBaseEvent));
	}

	return true;

}



//////////////////////////////////////////////////////////////////////////


DECLARE_CREATE_MODULE(DataBaseEngine);

//////////////////////////////////////////////////////////////////////////
