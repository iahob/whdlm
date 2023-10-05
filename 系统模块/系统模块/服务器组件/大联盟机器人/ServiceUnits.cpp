//�ر���IT������28xin.com��
#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

//////////////////////////////////////////////////////////////////////////////////

//��̬����
CServiceUnits *			CServiceUnits::g_pServiceUnits=NULL;			//����ָ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServiceUnits::CServiceUnits()
{
	//�������
	m_ServiceStatus=ServiceStatus_Stop;

	//���ýӿ�
	m_pIServiceUnitsSink=NULL;

	//���ö���
	//ASSERT(g_pServiceUnits==NULL);
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	//�������
	ZeroMemory(&m_GameParameter,sizeof(m_GameParameter));
	ZeroMemory(&m_DataBaseParameter,sizeof(m_DataBaseParameter));
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));
	ZeroMemory(&m_GameMatchOption,sizeof(m_GameMatchOption));

	return;
}

//��������
CServiceUnits::~CServiceUnits()
{
}

//���ýӿ�
bool CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//���ñ���
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//Ͷ������
bool CServiceUnits::PostControlRequest(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//״̬�ж�
	//ASSERT(IsWindow(m_hWnd));
	if (IsWindow(m_hWnd)==FALSE) return false;

	//�������
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.InsertData(wIdentifier,pData,wDataSize)==false) return false;

	//������Ϣ
	PostMessage(WM_UICONTROL_REQUEST,wIdentifier,wDataSize);

	return true;
}

//��������
bool CServiceUnits::StartService()
{
	//Ч��״̬
	//ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//��������
	if (m_hWnd==NULL)
	{
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);
	}

	//����ģ��
	if (CreateServiceDLL()==false)
	{
		ConcludeService();
		return false;
	}

	//��������
	if (RectifyServiceParameter()==false)
	{
		ConcludeService();
		return false;
	}

	//���÷���
	if (InitializeService()==false)
	{
		ConcludeService();
		return false;
	}

	//�����ں�
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}	

	//��������
	if (StartNetworkService() == false)
	{
		ConcludeService();
		return 0;
	}

	SendControlPacket(CT_CONNECT_CORRESPOND, NULL, 0);
	//����״̬
	SetServiceStatus(ServiceStatus_Service);

	return true;
}

//ֹͣ����
bool CServiceUnits::ConcludeService()
{
	//���ñ���
	SetServiceStatus(ServiceStatus_Stop);

	//�ں����
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketService.GetInterface()!=NULL) m_TCPSocketService->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();
	if (m_PrsnlRmTCPSocketService.GetInterface()!=NULL) m_PrsnlRmTCPSocketService->ConcludeService();
	if (m_LogServerTCPSocketService.GetInterface() != NULL) m_LogServerTCPSocketService->ConcludeService();

	//��������
	if (m_RecordDataBaseEngine.GetInterface()!=NULL) m_RecordDataBaseEngine->ConcludeService();
	if (m_KernelDataBaseEngine.GetInterface()!=NULL) m_KernelDataBaseEngine->ConcludeService();
	m_DBCorrespondManager.ConcludeService();

	//ע�����
	if (m_GameServiceManager.GetInterface()!=NULL) m_GameServiceManager.CloseInstance();
	if (m_MatchServiceManager.GetInterface()!=NULL)m_MatchServiceManager.CloseInstance();
	if (m_PersonalRoomServiceManager.GetInterface()!=NULL) m_PersonalRoomServiceManager.CloseInstance();
	return true;
}

//��Ϸ����
bool CServiceUnits::CollocateService(LPCTSTR pszGameModule, tagGameServiceOption & GameServiceOption)
{
	//Ч��״̬
	//ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//����ģ��
	m_GameServiceOption=GameServiceOption;
	m_GameServiceManager.SetModuleCreateInfo(pszGameModule,GAME_SERVICE_CREATE_NAME);

	return true;
}

//��Ϸ����
bool CServiceUnits::CollocateService(tagGameMatchOption & GameMatchOption)
{
	//Ч��״̬
	//ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//���ñ���
	m_GameMatchOption = GameMatchOption;

	return true;
}

//����ģ��
bool CServiceUnits::CreateServiceDLL()
{
	//ʱ������
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TimerEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//��������
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_AttemperEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//�������
	if ((m_TCPSocketService.GetInterface()==NULL)&&(m_TCPSocketService.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPSocketService.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}


	//�������
	if ((m_PrsnlRmTCPSocketService.GetInterface()==NULL)&&(m_PrsnlRmTCPSocketService.CreateInstance()==false))
	{
		CTraceService::TraceString(m_PrsnlRmTCPSocketService.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//��������
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPNetworkEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}
	//�������
	if ((m_KernelDataBaseEngine.GetInterface()==NULL)&&(m_KernelDataBaseEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_KernelDataBaseEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//�������
	if ((m_RecordDataBaseEngine.GetInterface()==NULL)&&(m_RecordDataBaseEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_RecordDataBaseEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	

	//��־ģ��
	if ((m_LogServerTCPSocketService.GetInterface() == NULL) && (m_LogServerTCPSocketService.CreateInstance() == false))
	{
		CTraceService::TraceString(TEXT("��־�ӿڴ���ʧ��"), TraceLevel_Exception);
		return false;
	}

	return true;
}

//�������
bool CServiceUnits::InitializeService()
{
	//���ò���
	m_InitParameter.SetServerParameter(&m_GameServiceAttrib,&m_GameServiceOption);

	//���ز���
	m_InitParameter.LoadInitParameter();

	//���ò���
	m_GameParameter.dwMedalRate=1L;
	m_GameParameter.dwRevenueRate=1L;
	m_GameParameter.dwExchangeRate=100L;
	m_GameParameter.lDayWinLostScoreLimit = 0;
	m_GameParameter.dwDayTimeLimit = 0;
	//������Ϣ
	LPCTSTR pszDataBaseAddr=m_GameServiceOption.szDataBaseAddr;
	LPCTSTR pszDataBaseName=m_GameServiceOption.szDataBaseName;
	if (LoadDataBaseParameter(pszDataBaseAddr,pszDataBaseName,m_DataBaseParameter)==false) return false;

	//����ӿ�
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//��������
	IUnknownEx * pIDataBaseEngineRecordSink[CountArray(m_RecordDataBaseSink)];
	IUnknownEx * pIDataBaseEngineKernelSink[CountArray(m_KernelDataBaseSink)];
	for (WORD i=0;i<CountArray(pIDataBaseEngineRecordSink);i++) pIDataBaseEngineRecordSink[i]=QUERY_OBJECT_INTERFACE(m_RecordDataBaseSink[i],IUnknownEx);
	for (WORD i=0;i<CountArray(pIDataBaseEngineKernelSink);i++) pIDataBaseEngineKernelSink[i]=QUERY_OBJECT_INTERFACE(m_KernelDataBaseSink[i],IUnknownEx);

	//�󶨽ӿ�
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;
	if (m_RecordDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineRecordSink,CountArray(pIDataBaseEngineRecordSink))==false) return false;
	if (m_KernelDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineKernelSink,CountArray(pIDataBaseEngineKernelSink))==false) return false;

	//��ѯ�ӿ�
	IGameServiceCustomTime * pIGameServiceSustomTime=QUERY_OBJECT_PTR_INTERFACE(m_GameServiceManager.GetInterface(),IGameServiceCustomTime);

	//ʱ������
	if (pIGameServiceSustomTime!=NULL)
	{
		//��Ԫʱ��
		DWORD dwTimeCell=dwTimeCell=pIGameServiceSustomTime->GetTimerEngineTimeCell();
		if (dwTimeCell>TIME_CELL) dwTimeCell=TIME_CELL;

		//����ʱ��
		m_TimerEngine->SetTimeCell(dwTimeCell);
	}

	//�ں����
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;
	
	//Э������
	if (m_TCPSocketService->SetServiceID(NETWORK_CORRESPOND)==false) return false;
	if (m_TCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return false;


	//Լս������
	if (m_PrsnlRmTCPSocketService->SetServiceID(NETWORK_PERSONAL_ROOM_CORRESPOND)==false) return false;
	if (m_PrsnlRmTCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return false;

	//��־������
	if (m_LogServerTCPSocketService->SetServiceID(NETWORK_LOG_SERVER) == false) return false;
	if (m_LogServerTCPSocketService->SetTCPSocketEvent(pIAttemperEngine) == false) return false;

	//����Э��
	m_DBCorrespondManager.InitDBCorrespondManager(m_KernelDataBaseEngine.GetInterface());

	//��������
	if(m_MatchServiceManager.GetInterface()!=NULL)
	{
		if(m_MatchServiceManager->CreateGameMatch(m_GameMatchOption.cbMatchType)==false) return false;
	}

	//����˽�˷�
	if(m_PersonalRoomServiceManager.GetInterface()!=NULL)
	{
		if(m_PersonalRoomServiceManager->CreatePersonalRoom(0)==false) return false;
	}



	//���Ȼص�
	m_AttemperEngineSink.m_pInitParameter=&m_InitParameter;


	//���Ȼص�
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

	//���ݻص�
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

	//���ݻص�
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

	//��������
	WORD wMaxConnect = m_InitParameter.m_wMaxConnect;
	WORD wServicePort = m_InitParameter.m_wAndroidDistributePort;

	m_TCPNetworkEngine->SetServiceParameter(wServicePort, wMaxConnect, szCompilation);
	
	return true;
}

//�����ں�
bool CServiceUnits::StartKernelService()
{
	LPCTSTR pszString = TEXT("AI�������ʼ����");
	CTraceService::TraceString(pszString, TraceLevel_Exception);

	//ʱ������
	if (m_TimerEngine->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//Э������
	if (m_TCPSocketService->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//Լս����
	if (m_PrsnlRmTCPSocketService->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//��־����
	if (m_LogServerTCPSocketService->StartService() == false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_RecordDataBaseEngine->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_KernelDataBaseEngine->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//����Э��
	if (m_DBCorrespondManager.StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}


	return true;
}

//��������
bool CServiceUnits::StartNetworkService()
{
	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}
	
	return true;
}

//��������
bool CServiceUnits::RectifyServiceParameter()
{
	return true;
}

//����״̬
bool CServiceUnits::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//״̬�ж�
	if (ServiceStatus == ServiceStatus_Service)
	{
		LPCTSTR pszString = TEXT("���������ɹ�");
		CTraceService::TraceString(pszString, TraceLevel_Exception);

		SendControlPacket(CT_LOAD_SERVICE_CONFIG, NULL, 0);

	}

	//״̬�ж�
	if (ServiceStatus == ServiceStatus_Stop)
	{
		LPCTSTR pszString = TEXT("����ֹͣ");
		CTraceService::TraceString(pszString, TraceLevel_Exception);
	}

	
	return true;
}

//���Ϳ���
bool CServiceUnits::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	//ASSERT(m_AttemperEngine.GetInterface()!=NULL);
	if (m_AttemperEngine.GetInterface()==NULL) return false;

	//���Ϳ���
	m_AttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//������Ϣ
bool CServiceUnits::LoadDataBaseParameter(LPCTSTR pszDataBaseAddr, LPCTSTR pszDataBaseName, tagDataBaseParameter & DataBaseParameter)
{
	return true;
}

//������Ϣ
LRESULT CServiceUnits::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//��������
	tagDataHead DataHead;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];

	//��ȡ����
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.DistillData(DataHead, cbBuffer, sizeof(cbBuffer)) == false)
	{
		//ASSERT(FALSE);
		return NULL;
	}

	//���ݴ���
	switch (DataHead.wIdentifier)
	{
		case UI_CLEAR_RESULT:		//Э���ɹ�
		{
			//Ч����Ϣ
			//ASSERT(DataHead.wDataSize == sizeof(CP_ControlResult));
			if (DataHead.wDataSize != sizeof(CP_ControlResult)) return 0;

			ConcludeService();

			if (m_pIServiceUnitsSink != NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(ServiceStatus_Stop);
			return 0;
		}
	}
	return 0;
}

//��¼����������������
void CServiceUnits::WriteSeverCount(bool bStartOrClose)
{

}

//д˽�˷����ɢ��Ϣ
bool	CServiceUnits::WritePersonalDissumeInfo()
{
	return false;
}

bool CServiceUnits::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{

	return true;
}


//����رպ����������������
bool CServiceUnits::UnLockServerUser()
{
	return false;
}

//////////////////////////////////////////////////////////////////////////////////
