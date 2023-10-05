//藏宝库IT社区（28xin.com）
#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

//////////////////////////////////////////////////////////////////////////////////

//静态变量
CServiceUnits *			CServiceUnits::g_pServiceUnits=NULL;			//对象指针

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServiceUnits::CServiceUnits()
{
	//服务参数
	m_ServiceStatus=ServiceStatus_Stop;

	//设置接口
	m_pIServiceUnitsSink=NULL;

	//设置对象
	//ASSERT(g_pServiceUnits==NULL);
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	//组件配置
	ZeroMemory(&m_GameParameter,sizeof(m_GameParameter));
	ZeroMemory(&m_DataBaseParameter,sizeof(m_DataBaseParameter));
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));
	ZeroMemory(&m_GameMatchOption,sizeof(m_GameMatchOption));

	return;
}

//析构函数
CServiceUnits::~CServiceUnits()
{
}

//设置接口
bool CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//设置变量
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//投递请求
bool CServiceUnits::PostControlRequest(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//状态判断
	//ASSERT(IsWindow(m_hWnd));
	if (IsWindow(m_hWnd)==FALSE) return false;

	//插入队列
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.InsertData(wIdentifier,pData,wDataSize)==false) return false;

	//发送消息
	PostMessage(WM_UICONTROL_REQUEST,wIdentifier,wDataSize);

	return true;
}

//启动服务
bool CServiceUnits::StartService()
{
	//效验状态
	//ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//创建窗口
	if (m_hWnd==NULL)
	{
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);
	}

	//创建模块
	if (CreateServiceDLL()==false)
	{
		ConcludeService();
		return false;
	}

	//调整参数
	if (RectifyServiceParameter()==false)
	{
		ConcludeService();
		return false;
	}

	//配置服务
	if (InitializeService()==false)
	{
		ConcludeService();
		return false;
	}

	//启动内核
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}	

	//启动网络
	if (StartNetworkService() == false)
	{
		ConcludeService();
		return 0;
	}

	SendControlPacket(CT_CONNECT_CORRESPOND, NULL, 0);
	//设置状态
	SetServiceStatus(ServiceStatus_Service);

	return true;
}

//停止服务
bool CServiceUnits::ConcludeService()
{
	//设置变量
	SetServiceStatus(ServiceStatus_Stop);

	//内核组件
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketService.GetInterface()!=NULL) m_TCPSocketService->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();
	if (m_PrsnlRmTCPSocketService.GetInterface()!=NULL) m_PrsnlRmTCPSocketService->ConcludeService();
	if (m_LogServerTCPSocketService.GetInterface() != NULL) m_LogServerTCPSocketService->ConcludeService();

	//数据引擎
	if (m_RecordDataBaseEngine.GetInterface()!=NULL) m_RecordDataBaseEngine->ConcludeService();
	if (m_KernelDataBaseEngine.GetInterface()!=NULL) m_KernelDataBaseEngine->ConcludeService();
	m_DBCorrespondManager.ConcludeService();

	//注销组件
	if (m_GameServiceManager.GetInterface()!=NULL) m_GameServiceManager.CloseInstance();
	if (m_MatchServiceManager.GetInterface()!=NULL)m_MatchServiceManager.CloseInstance();
	if (m_PersonalRoomServiceManager.GetInterface()!=NULL) m_PersonalRoomServiceManager.CloseInstance();
	return true;
}

//游戏配置
bool CServiceUnits::CollocateService(LPCTSTR pszGameModule, tagGameServiceOption & GameServiceOption)
{
	//效验状态
	//ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//配置模块
	m_GameServiceOption=GameServiceOption;
	m_GameServiceManager.SetModuleCreateInfo(pszGameModule,GAME_SERVICE_CREATE_NAME);

	return true;
}

//游戏配置
bool CServiceUnits::CollocateService(tagGameMatchOption & GameMatchOption)
{
	//效验状态
	//ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//设置变量
	m_GameMatchOption = GameMatchOption;

	return true;
}

//创建模块
bool CServiceUnits::CreateServiceDLL()
{
	//时间引擎
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TimerEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//调度引擎
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_AttemperEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//网络组件
	if ((m_TCPSocketService.GetInterface()==NULL)&&(m_TCPSocketService.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPSocketService.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}


	//网络组件
	if ((m_PrsnlRmTCPSocketService.GetInterface()==NULL)&&(m_PrsnlRmTCPSocketService.CreateInstance()==false))
	{
		CTraceService::TraceString(m_PrsnlRmTCPSocketService.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//网络引擎
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPNetworkEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}
	//数据组件
	if ((m_KernelDataBaseEngine.GetInterface()==NULL)&&(m_KernelDataBaseEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_KernelDataBaseEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//数据组件
	if ((m_RecordDataBaseEngine.GetInterface()==NULL)&&(m_RecordDataBaseEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_RecordDataBaseEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	

	//日志模块
	if ((m_LogServerTCPSocketService.GetInterface() == NULL) && (m_LogServerTCPSocketService.CreateInstance() == false))
	{
		CTraceService::TraceString(TEXT("日志接口创建失败"), TraceLevel_Exception);
		return false;
	}

	return true;
}

//配置组件
bool CServiceUnits::InitializeService()
{
	//设置参数
	m_InitParameter.SetServerParameter(&m_GameServiceAttrib,&m_GameServiceOption);

	//加载参数
	m_InitParameter.LoadInitParameter();

	//配置参数
	m_GameParameter.dwMedalRate=1L;
	m_GameParameter.dwRevenueRate=1L;
	m_GameParameter.dwExchangeRate=100L;
	m_GameParameter.lDayWinLostScoreLimit = 0;
	m_GameParameter.dwDayTimeLimit = 0;
	//连接信息
	LPCTSTR pszDataBaseAddr=m_GameServiceOption.szDataBaseAddr;
	LPCTSTR pszDataBaseName=m_GameServiceOption.szDataBaseName;
	if (LoadDataBaseParameter(pszDataBaseAddr,pszDataBaseName,m_DataBaseParameter)==false) return false;

	//组件接口
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//数据引擎
	IUnknownEx * pIDataBaseEngineRecordSink[CountArray(m_RecordDataBaseSink)];
	IUnknownEx * pIDataBaseEngineKernelSink[CountArray(m_KernelDataBaseSink)];
	for (WORD i=0;i<CountArray(pIDataBaseEngineRecordSink);i++) pIDataBaseEngineRecordSink[i]=QUERY_OBJECT_INTERFACE(m_RecordDataBaseSink[i],IUnknownEx);
	for (WORD i=0;i<CountArray(pIDataBaseEngineKernelSink);i++) pIDataBaseEngineKernelSink[i]=QUERY_OBJECT_INTERFACE(m_KernelDataBaseSink[i],IUnknownEx);

	//绑定接口
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;
	if (m_RecordDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineRecordSink,CountArray(pIDataBaseEngineRecordSink))==false) return false;
	if (m_KernelDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineKernelSink,CountArray(pIDataBaseEngineKernelSink))==false) return false;

	//查询接口
	IGameServiceCustomTime * pIGameServiceSustomTime=QUERY_OBJECT_PTR_INTERFACE(m_GameServiceManager.GetInterface(),IGameServiceCustomTime);

	//时间引擎
	if (pIGameServiceSustomTime!=NULL)
	{
		//单元时间
		DWORD dwTimeCell=dwTimeCell=pIGameServiceSustomTime->GetTimerEngineTimeCell();
		if (dwTimeCell>TIME_CELL) dwTimeCell=TIME_CELL;

		//设置时间
		m_TimerEngine->SetTimeCell(dwTimeCell);
	}

	//内核组件
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;
	
	//协调服务
	if (m_TCPSocketService->SetServiceID(NETWORK_CORRESPOND)==false) return false;
	if (m_TCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return false;


	//约战服务器
	if (m_PrsnlRmTCPSocketService->SetServiceID(NETWORK_PERSONAL_ROOM_CORRESPOND)==false) return false;
	if (m_PrsnlRmTCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return false;

	//日志服务器
	if (m_LogServerTCPSocketService->SetServiceID(NETWORK_LOG_SERVER) == false) return false;
	if (m_LogServerTCPSocketService->SetTCPSocketEvent(pIAttemperEngine) == false) return false;

	//数据协调
	m_DBCorrespondManager.InitDBCorrespondManager(m_KernelDataBaseEngine.GetInterface());

	//创建比赛
	if(m_MatchServiceManager.GetInterface()!=NULL)
	{
		if(m_MatchServiceManager->CreateGameMatch(m_GameMatchOption.cbMatchType)==false) return false;
	}

	//创建私人房
	if(m_PersonalRoomServiceManager.GetInterface()!=NULL)
	{
		if(m_PersonalRoomServiceManager->CreatePersonalRoom(0)==false) return false;
	}



	//调度回调
	m_AttemperEngineSink.m_pInitParameter=&m_InitParameter;


	//调度回调
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIAttemperEngine=m_AttemperEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketService=m_TCPSocketService.GetInterface();
	m_AttemperEngineSink.m_pPrsnlRmITCPSocketService=m_PrsnlRmTCPSocketService.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pIGameServiceSustomTime=pIGameServiceSustomTime;
	m_AttemperEngineSink.m_pIGameServiceManager=m_GameServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIRecordDataBaseEngine=m_RecordDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIKernelDataBaseEngine=m_KernelDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIPersonalRoomServiceManager=m_PersonalRoomServiceManager.GetInterface();
	m_AttemperEngineSink.m_pLogServerTCPSocketService = m_LogServerTCPSocketService.GetInterface();
	
	m_AttemperEngineSink.m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);

	//数据回调
	for (INT i=0;i<CountArray(m_RecordDataBaseSink);i++)
	{
		m_RecordDataBaseSink[i].m_pInitParameter=&m_InitParameter;
		m_RecordDataBaseSink[i].m_pGameParameter=&m_GameParameter;
		m_RecordDataBaseSink[i].m_pDataBaseParameter=&m_DataBaseParameter;
		m_RecordDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_RecordDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_RecordDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_RecordDataBaseSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
	}

	//数据回调
	for (INT i=0;i<CountArray(m_KernelDataBaseSink);i++)
	{
		m_KernelDataBaseSink[i].m_pInitParameter=&m_InitParameter;
		m_KernelDataBaseSink[i].m_pGameParameter=&m_GameParameter;
		m_KernelDataBaseSink[i].m_pDataBaseParameter=&m_DataBaseParameter;
		m_KernelDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_KernelDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_KernelDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_KernelDataBaseSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
		m_KernelDataBaseSink[i].m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);
	}

	//配置网络
	WORD wMaxConnect = m_InitParameter.m_wMaxConnect;
	WORD wServicePort = m_InitParameter.m_wAndroidDistributePort;

	m_TCPNetworkEngine->SetServiceParameter(wServicePort, wMaxConnect, szCompilation);
	
	return true;
}

//启动内核
bool CServiceUnits::StartKernelService()
{
	LPCTSTR pszString = TEXT("AI分配服务开始启动");
	CTraceService::TraceString(pszString, TraceLevel_Exception);

	//时间引擎
	if (m_TimerEngine->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//调度引擎
	if (m_AttemperEngine->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//协调引擎
	if (m_TCPSocketService->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//约战引擎
	if (m_PrsnlRmTCPSocketService->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//日志引擎
	if (m_LogServerTCPSocketService->StartService() == false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//数据引擎
	if (m_RecordDataBaseEngine->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//数据引擎
	if (m_KernelDataBaseEngine->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//数据协调
	if (m_DBCorrespondManager.StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}


	return true;
}

//启动网络
bool CServiceUnits::StartNetworkService()
{
	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}
	
	return true;
}

//调整参数
bool CServiceUnits::RectifyServiceParameter()
{
	return true;
}

//设置状态
bool CServiceUnits::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//状态判断
	if (ServiceStatus == ServiceStatus_Service)
	{
		LPCTSTR pszString = TEXT("服务启动成功");
		CTraceService::TraceString(pszString, TraceLevel_Exception);

		SendControlPacket(CT_LOAD_SERVICE_CONFIG, NULL, 0);

	}

	//状态判断
	if (ServiceStatus == ServiceStatus_Stop)
	{
		LPCTSTR pszString = TEXT("服务停止");
		CTraceService::TraceString(pszString, TraceLevel_Exception);
	}

	
	return true;
}

//发送控制
bool CServiceUnits::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//状态效验
	//ASSERT(m_AttemperEngine.GetInterface()!=NULL);
	if (m_AttemperEngine.GetInterface()==NULL) return false;

	//发送控制
	m_AttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//连接信息
bool CServiceUnits::LoadDataBaseParameter(LPCTSTR pszDataBaseAddr, LPCTSTR pszDataBaseName, tagDataBaseParameter & DataBaseParameter)
{
	return true;
}

//控制消息
LRESULT CServiceUnits::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	tagDataHead DataHead;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];

	//提取数据
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.DistillData(DataHead, cbBuffer, sizeof(cbBuffer)) == false)
	{
		//ASSERT(FALSE);
		return NULL;
	}

	//数据处理
	switch (DataHead.wIdentifier)
	{
		case UI_CLEAR_RESULT:		//协调成功
		{
			//效验消息
			//ASSERT(DataHead.wDataSize == sizeof(CP_ControlResult));
			if (DataHead.wDataSize != sizeof(CP_ControlResult)) return 0;

			ConcludeService();

			if (m_pIServiceUnitsSink != NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(ServiceStatus_Stop);
			return 0;
		}
	}
	return 0;
}

//记录启动服务器的数量
void CServiceUnits::WriteSeverCount(bool bStartOrClose)
{

}

//写私人房间解散信息
bool	CServiceUnits::WritePersonalDissumeInfo()
{
	return false;
}

bool CServiceUnits::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{

	return true;
}


//房间关闭后解锁房间里面的玩家
bool CServiceUnits::UnLockServerUser()
{
	return false;
}

//////////////////////////////////////////////////////////////////////////////////
