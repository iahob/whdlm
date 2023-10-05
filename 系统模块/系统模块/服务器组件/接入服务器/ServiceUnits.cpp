//�ر���IT������28xin.com��
#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CServiceUnits *	CServiceUnits::g_pServiceUnits=NULL;			//����ָ��

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CServiceUnits::CServiceUnits()
{
	//���ñ���
	m_ServiceStatus=ServiceStatus_Stop;

	//���ýӿ�
	m_pIServiceUnitsSink=NULL;

	//���ö���
	//ASSERT(g_pServiceUnits==NULL);
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	return;
}

//��������
CServiceUnits::~CServiceUnits()
{
	//ֹͣ����
	ConcludeService();


}

//��������
bool __cdecl CServiceUnits::StartService()
{
	//Ч��״̬
	//ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//����״̬
	SetServiceStatus(ServiceStatus_Config);

	//��������
	if (m_hWnd==NULL)
	{
		//��������
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);
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

	//�������ݿ�
	if (StartDataBaseService() == false)
	{
		ConcludeService();
		return 0;
	}
	//��Ȩ��֤
	//SendControlPacket(CT_CONNECT_CORRESPOND,NULL,0);
//	SendControlPacket(CT_LOAD_GROUP_OPTION, NULL, 0);
	//����Ⱥ��
	SendControlPacket(CT_LOAD_GROUP_LIST, NULL, 0);

	return true;
}

//ֹͣ����
bool __cdecl CServiceUnits::ConcludeService()
{
	//���ñ���
	SetServiceStatus(ServiceStatus_Stop);

	//ֹͣ����
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();
	if (m_TCPSocketCorrespond.GetInterface()!=NULL) m_TCPSocketCorrespond->ConcludeService();
	if (m_TCPSocketBattle.GetInterface() != NULL) m_TCPSocketBattle->ConcludeService();
	if (m_DataBaseEngine.GetInterface() != NULL) m_DataBaseEngine->ConcludeService();
	if (m_LogServerTCPSocketService.GetInterface() != NULL) m_LogServerTCPSocketService->ConcludeService();
	if (m_pAITCPSocketService.GetInterface() != NULL) m_pAITCPSocketService->ConcludeService();

	return true;
}

//���ýӿ�
bool __cdecl CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//���ñ���
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//Ͷ������
bool __cdecl CServiceUnits::PostControlRequest(WORD wIdentifier, VOID *pData, WORD wDataSize)
{
	//״̬�ж�
	//ASSERT(IsWindow(m_hWnd));
	if (IsWindow(m_hWnd) == FALSE)
	{
		return false;
	}

	//�������
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.InsertData(wIdentifier, pData, wDataSize) == false)
	{
		return false;
	}

	//������Ϣ
	PostMessage(WM_UICONTROL_REQUEST, wIdentifier, wDataSize);

	return true;
}

//�������
bool CServiceUnits::InitializeService()
{
	//���ز���
	m_InitParameter.LoadInitParameter();

	//�������
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false)) return false;
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return false;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return false;
	if ((m_TCPSocketCorrespond.GetInterface()==NULL)&&(m_TCPSocketCorrespond.CreateInstance()==false)) return false;
	if ((m_TCPSocketBattle.GetInterface() == NULL) && (m_TCPSocketBattle.CreateInstance() == false)) return false;
	if ((m_DataBaseEngine.GetInterface() == NULL) && (m_DataBaseEngine.CreateInstance() == false)) return false;
	if ((m_LogServerTCPSocketService.GetInterface() == NULL) && (m_LogServerTCPSocketService.CreateInstance() == false)) return false;
	if ((m_pAITCPSocketService.GetInterface() == NULL) && (m_pAITCPSocketService.CreateInstance() == false)) return false;

	//����ӿ�
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//���ݽӿ�
	IUnknownEx * pDataBaseEngineSink[CountArray(m_DataBaseEngineSink)];
	for (WORD i = 0; i<CountArray(pDataBaseEngineSink); i++)
	{
		pDataBaseEngineSink[i] = QUERY_OBJECT_INTERFACE(m_DataBaseEngineSink[i], IUnknownEx);
	}

	//�󶨽ӿ�
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink) == false) return false;
	if (m_DataBaseEngine->SetDataBaseEngineSink(pDataBaseEngineSink, CountArray(pDataBaseEngineSink)) == false) return false;
	//�ں����
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//�������
	m_TCPSocketCorrespond->SetServiceID(NETWORK_CORRESPOND);
	if (m_TCPSocketCorrespond->SetTCPSocketEvent(pIAttemperEngine)==false) return false;

	//�������
	m_TCPSocketBattle->SetServiceID(NETWORK_BATTLE);
	if (m_TCPSocketBattle->SetTCPSocketEvent(pIAttemperEngine) == false) return false;

	//��־������
	if (m_LogServerTCPSocketService->SetServiceID(NETWORK_LOG_SERVER) == false) return false;
	if (m_LogServerTCPSocketService->SetTCPSocketEvent(pIAttemperEngine) == false) return false;

	//AI�������
	m_pAITCPSocketService->SetServiceID(NETWORK_AI_SERVER);
	if (m_pAITCPSocketService->SetTCPSocketEvent(pIAttemperEngine) == false) return false;

	//���Ȼص�
	m_AttemperEngineSink.m_pInitParameter=&m_InitParameter;
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_TCPSocketCorrespond=m_TCPSocketCorrespond.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketBattle = m_TCPSocketBattle.GetInterface();
	m_AttemperEngineSink.m_pIDataBaseEngine = m_DataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pLogServerTCPSocketService = m_LogServerTCPSocketService.GetInterface();
	m_AttemperEngineSink.m_pAITCPSocketService = m_pAITCPSocketService.GetInterface();

	//���ݿ�ص�
	for (INT i = 0; i < CountArray(m_DataBaseEngineSink); i++)
	{
		m_DataBaseEngineSink[i].m_pInitParameter = &m_InitParameter;
		m_DataBaseEngineSink[i].m_pIDataBaseEngineEvent = QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine, IDataBaseEngineEvent);
	}

	//��������
	WORD wMaxConnect = m_InitParameter.m_wMaxConnect;
	WORD wServicePort = m_InitParameter.m_wServicePort;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort, wMaxConnect,szCompilation) == false) return false;

	return true;
}

//�����ں�
bool CServiceUnits::StartKernelService()
{
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

	//��������
	if (m_TCPSocketCorrespond->StartService()==false)
	{
		//ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_TCPSocketBattle->StartService() == false)
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

	//AI��������
	if (m_pAITCPSocketService->StartService() == false)
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

//�������ݿ�
bool CServiceUnits::StartDataBaseService()
{
	//�������ݿ�
	if (m_DataBaseEngine->StartService() == false)
	{
		//ASSERT(FALSE);
		return false;
	}

	return true;
}

//����״̬
bool CServiceUnits::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//״̬�ж�
	if (m_ServiceStatus!=ServiceStatus)
	{
		//����֪ͨ
		if ((m_ServiceStatus!=ServiceStatus_Service)&&(ServiceStatus==ServiceStatus_Stop))
		{
			LPCTSTR pszString=TEXT("��������ʧ��");
			CTraceService::TraceString(pszString,TraceLevel_Exception);
		}

		//���ñ���
		m_ServiceStatus=ServiceStatus;

		//״̬֪ͨ
		//ASSERT(m_pIServiceUnitsSink!=NULL);
		if (m_pIServiceUnitsSink!=NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(m_ServiceStatus);
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
	case UI_CORRESPOND_RESULT :		//Э���ɹ�
		{
			//Ч����Ϣ
			//ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//��������
			CP_ControlResult * pControlResult = (CP_ControlResult *)cbBuffer;

			//ʧ�ܴ���
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_FAILURE))
			{
				ConcludeService();
				return 0;
			}

			//�ɹ�����
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_SUCCESS))
			{
				//����״̬
				SendControlPacket(CT_CONNECT_BATTLE, NULL, 0);
			}
			return 0;
		}
	case UI_BATTLE_RESULT:			//Լս�ɹ�
		{
			//Ч����Ϣ
			//ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//��������
			CP_ControlResult * pControlResult = (CP_ControlResult *)cbBuffer;

			//ʧ�ܴ���
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_FAILURE))
			{
				ConcludeService();
				return 0;
			}

			//�ɹ�����
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_SUCCESS))
			{

				//��������
				if (StartNetworkService()==false)
				{
					ConcludeService();
					return 0;
				}

				//����״̬
				SetServiceStatus(ServiceStatus_Service);
			}						
			return 0;
		}
	case UI_LOAD_GROUP_OPTION:		//��������
		{
			//Ч����Ϣ
			//ASSERT(DataHead.wDataSize == sizeof(CP_ControlResult));
			if (DataHead.wDataSize != sizeof(CP_ControlResult)) return 0;

			//��������
			CP_ControlResult * pControlResult = (CP_ControlResult *)cbBuffer;

			//ʧ�ܴ���
			if ((m_ServiceStatus != ServiceStatus_Service) && (pControlResult->cbSuccess == ER_FAILURE))
			{
				ConcludeService();
				return 0;
			}

			//�ɹ�����
			if ((m_ServiceStatus != ServiceStatus_Service) && (pControlResult->cbSuccess == ER_SUCCESS))
			{
				//����״̬
				SendControlPacket(CT_CONNECT_CORRESPOND, NULL, 0);

				//������־������
				SendControlPacket(CT_CONNECT_LOG_SERVER, NULL, 0);

				//����AI������
				SendControlPacket(CT_CONNECT_AI_SERVER, NULL, 0);
				
			}

			return 0;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
