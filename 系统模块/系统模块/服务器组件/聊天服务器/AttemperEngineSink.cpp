#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////
//��������
#define TEMP_MESSAGE_ID				INVALID_DWORD			

//ʱ���ʶ
#define IDI_CONNECT_CORRESPOND		1									//����ʱ��
#define IDI_LOAD_SYSTEM_MESSAGE		2									//ϵͳ��Ϣ
#define IDI_SEND_SYSTEM_MESSAGE		3									//ϵͳ��Ϣ
#define IDI_TEST_LOG_MESSAGE		4									//ϵͳ��Ϣ
#define IDI_CONNECT_LOG_SERVER		5								    //ϵͳ��Ϣʱ��
#define IDI_PERSONAL_ROOM			6									//��ʱ����Լս������

//���ʱ��
#define MIN_INTERVAL_TIME			10									//���ʱ��
#define MAX_INTERVAL_TIME			1*60								//��ʱ��ʶ
#define TIME_LOAD_SYSTEM_MESSAGE	600L								//ϵͳ��Ϣʱ��
#define TIME_SEND_SYSTEM_MESSAGE	10L								    //ϵͳ��Ϣʱ��
#define TIME_TEST_LOG_MESSAGE		11L								    //ϵͳ��Ϣʱ��


//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_bNeekLogServer = true;

	//���Ʊ���
	m_dwIntervalTime=0;
	m_dwLastDisposeTime=0;

	//������
	m_pNormalParameter=NULL;

	//״̬����
	m_pInitParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pLogServerTCPSocketService = NULL;
	m_pPrsnlRmITCPSocketService = NULL;

	//�ж��Ƿ���Լս���
	HINSTANCE hInstLibrary = NULL;
#ifdef _DEBUG
	hInstLibrary = LoadLibrary(TEXT("PersonalRoomServiceD.dll"));
#else
	hInstLibrary = LoadLibrary(TEXT("PersonalRoomService.dll"));
#endif

	if (hInstLibrary == NULL)
	{
		FreeLibrary(hInstLibrary);
	}
	else
	{
		m_bHasPrsnlRM = true;
		FreeLibrary(hInstLibrary);
	}
	
	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
	//ɾ������
	SafeDeleteArray(m_pNormalParameter);

	//ɾ����Ϣ
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	for (INT_PTR i = 0; i<m_SystemMessageBuffer.GetCount(); i++)
	{
		SafeDelete(m_SystemMessageBuffer[i]);
	}

	//�������
	m_SystemMessageActive.RemoveAll();
	m_SystemMessageBuffer.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//����Ϣ
	m_pNormalParameter=new tagBindParameter[m_pInitParameter->m_wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxPlayer);

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//������Ϣ
	m_pInitParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pPrsnlRmITCPSocketService = NULL;
	m_pLogServerTCPSocketService = NULL;
	//�Ƴ���Ϣ
	RemoveSystemMessage();

	//������
	SafeDeleteArray(m_pNormalParameter);

	//ɾ���û�
	m_FriendGroupManager.DeleteFriendGroup();
	m_ServerUserManager.DeleteUserItem();
	m_MatchServerManager.ResetMatchData();

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_CORRESPOND:		//����Э��
		{
			//��������
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf(szString,CountArray(szString),TEXT("��������Э�������� [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
	case CT_LOAD_SERVICE_CONFIG:	//��������
		{
			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case CT_CONNECT_PERSONAL_ROOM_CORRESPOND:		//����Լս
		{
			//�Ƿ���Լս������
			if (!m_bHasPrsnlRM)
			{
				return true;
			}

			//��������
			tagAddressInfo * pPrsnlRmCorrespondAddress = &m_pInitParameter->m_PrsnlRmCorrespondAddress;
			//��������
			if (m_pPrsnlRmITCPSocketService)
			{
				m_pPrsnlRmITCPSocketService->Connect(pPrsnlRmCorrespondAddress->szAddress, m_pInitParameter->m_wPrsnlRmCorrespondPort);
			}

			//������ʾ
			TCHAR szString[512] = TEXT("");
			_sntprintf(szString, CountArray(szString), TEXT("��������Լս������ [ %s:%d ]"), pPrsnlRmCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString, TraceLevel_Normal);

			return true;
		}
	case CT_CONNECT_LOG_SERVER:
		{
			//��������
			tagAddressInfo * pLogServerAddress = &m_pInitParameter->m_LogServerAddress;

			//��������
			m_pLogServerTCPSocketService->Connect(pLogServerAddress->szAddress, m_pInitParameter->m_wLogServerPort);


			//������ʾ
			TCHAR szString[512] = TEXT("");
			_sntprintf(szString, CountArray(szString), TEXT("����������־������ [ %s:%d ]"), pLogServerAddress->szAddress, m_pInitParameter->m_wLogServerPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString, TraceLevel_Normal);
			return true;
		}
	}	
	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
		case IDI_CONNECT_CORRESPOND:
		{
			//��������
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf(szString,CountArray(szString),TEXT("��������Э�������� [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
		case IDI_PERSONAL_ROOM:
		{
			//�Ƿ���Լս
			if (!m_bHasPrsnlRM)
			{
				return true;
			}

			//��������
			tagAddressInfo * pPrsnlRmCorrespondAddress = &m_pInitParameter->m_PrsnlRmCorrespondAddress;
			if (m_pPrsnlRmITCPSocketService)
			{
				m_pPrsnlRmITCPSocketService->Connect(pPrsnlRmCorrespondAddress->szAddress, m_pInitParameter->m_wPrsnlRmCorrespondPort);
			}

			//������ʾ
			TCHAR szString[512] = TEXT("");
			_sntprintf(szString, CountArray(szString), TEXT("��������Լս������ [ %s:%d ]"), pPrsnlRmCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString, TraceLevel_Normal);
			return true;
		}
		case IDI_LOAD_SYSTEM_MESSAGE:
		{
			//�����Ϣ����
			RemoveSystemMessage();

			//������Ϣ
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SYSTEM_MESSAGE, 0L, NULL, 0L);

			return true;
		}
		case IDI_SEND_SYSTEM_MESSAGE:
		{
			//�����ж�
			if (m_SystemMessageActive.GetCount() == 0) return true;

			//ʱЧ�ж�
			DWORD dwCurrTime = (DWORD)time(NULL);
			for (INT_PTR nIndex = m_SystemMessageActive.GetCount() - 1; nIndex >= 0; nIndex--)
			{
				tagSystemMessage *pTagSystemMessage = m_SystemMessageActive[nIndex];

				//ʱЧ�ж�
				if (pTagSystemMessage->SystemMessage.tConcludeTime < dwCurrTime)
				{
					m_SystemMessageActive.RemoveAt(nIndex);
					SafeDelete(pTagSystemMessage);

					continue;
				}

				//����ж�
				if (pTagSystemMessage->dwLastTime + pTagSystemMessage->SystemMessage.dwTimeRate < dwCurrTime)
				{
					//��������
					pTagSystemMessage->dwLastTime = dwCurrTime;

					//������Ϣ
					CMD_GR_SendMessage SendMessage = {};
					SendMessage.cbAllRoom = (pTagSystemMessage->SystemMessage.dwMessageID == TEMP_MESSAGE_ID) ? TRUE : FALSE;
					SendMessage.cbGame = (pTagSystemMessage->SystemMessage.cbMessageType == 1) ? TRUE : FALSE;
					SendMessage.cbRoom = (pTagSystemMessage->SystemMessage.cbMessageType == 2) ? TRUE : FALSE;
					if (pTagSystemMessage->SystemMessage.cbMessageType == 3)
					{
						SendMessage.cbGame = TRUE;
						SendMessage.cbRoom = TRUE;
					}
					lstrcpyn(SendMessage.szSystemMessage, pTagSystemMessage->SystemMessage.szSystemMessage, CountArray(SendMessage.szSystemMessage));
					SendMessage.wChatLength = lstrlen(SendMessage.szSystemMessage) + 1;

					//������Ϣ
					WORD wSendSize = sizeof(SendMessage) - sizeof(SendMessage.szSystemMessage) + CountStringBuffer(SendMessage.szSystemMessage);
					//SendSystemMessage(&SendMessage, wSendSize);
					SendSystemMessage(SendMessage.szSystemMessage,0);
				}
			}
			return true;
		}
		case IDI_CONNECT_LOG_SERVER://��־����������
		{
			//��������
			tagAddressInfo * pLogServerAddress = &m_pInitParameter->m_LogServerAddress;

			//��������
			m_pLogServerTCPSocketService->Connect(pLogServerAddress->szAddress, m_pInitParameter->m_wLogServerPort);

			return true;
		}
		case IDI_TEST_LOG_MESSAGE:
		{
			SendLogData(TEXT("�����������־����"));
			return true;
		}
	}
	
	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("Э������������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			//ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}
	
#ifdef _DEBUG
			m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE, 15 * 1000L, TIMES_INFINITY, NULL);
			m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE, 5 * 1000L, TIMES_INFINITY, NULL);
#else
			m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE, TIME_LOAD_SYSTEM_MESSAGE * 1000L, TIMES_INFINITY, NULL);
			m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE, TIME_SEND_SYSTEM_MESSAGE * 1000L, TIMES_INFINITY, NULL);
#endif	
		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("����ע����Ϸ���������..."),TraceLevel_Normal);

		//ע������
		m_pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_CHAT);

		return true;
	}
	else if (wServiceID == NETWORK_PERSONAL_ROOM_CORRESPOND)
	{
		//�����ж�
		if (nErrorCode != 0)
		{
			//������ʾ
			TCHAR szDescribe[128] = TEXT("");
			_sntprintf(szDescribe, CountArray(szDescribe), TEXT("Լս����������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode, m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe, TraceLevel_Warning);

			//����ʱ��
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_PERSONAL_ROOM, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("������Լս������ע����Ϸ���������..."), TraceLevel_Normal);

		//ע������
		m_pPrsnlRmITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_CHAT);
		return true;
	}
	else if (wServiceID == NETWORK_LOG_SERVER)
	{
		//�����ж�
		if (nErrorCode != 0)
		{
			//����ʱ��
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_LOG_SERVER, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return false;
		}
		
			//������־���Զ�ʱ��
		m_pITimerEngine->SetTimer(IDI_TEST_LOG_MESSAGE, 5 * 1000L, TIMES_INFINITY, NULL);

	}
	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//���ñ���
		m_bCollectUser=false;

		//ɾ��ʱ��
		m_pITimerEngine->KillTimer(IDI_CONNECT_CORRESPOND);

		//�����ж�
		if (m_bNeekCorrespond==true)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("��Э�������������ӹر��ˣ�%ld �����������"),m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			//ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);
		}

		return true;
	}
	else if (wServiceID == NETWORK_PERSONAL_ROOM_CORRESPOND)
	{
		//������ʾ
		TCHAR szDescribe[128] = TEXT("");
		_sntprintf(szDescribe, CountArray(szDescribe), TEXT("��Լս�����������ӹر��ˣ�%ld �����������"), m_pInitParameter->m_wConnectTime);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe, TraceLevel_Warning);

		//����ʱ��
		//ASSERT(m_pITimerEngine != NULL);
		m_pITimerEngine->SetTimer(IDI_PERSONAL_ROOM, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

		return true;
	}
	//��־����
	else if (wServiceID == NETWORK_LOG_SERVER)
	{
		//�����ж�
		if (m_bNeekLogServer)
		{
			//����ʱ��
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_LOG_SERVER, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return true;
		}
	}


	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_SERVICE_INFO:	//������Ϣ
			{
				return OnEventTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_USER_COLLECT:	//�û���Ϣ
			{
				return OnEventTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE: //�������
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}
	else if (wServiceID == NETWORK_PERSONAL_ROOM_CORRESPOND)
	{
		if (Command.wMainCmdID == MDM_CS_USER_COLLECT)
		{
			return OnEventTCPSocketPersonalRoomInfoCollect(Command.wSubCmdID, pData, wDataSize);
		}
		else if (Command.wMainCmdID == MDM_CS_SERVICE_INFO)
		{
			return OnEventTCPSocketMainServicePersonalRoomInfo(Command.wSubCmdID, pData, wDataSize);
		}

		return true;
	}

	return true;
}

bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SYSTEM_MESSAGE:
		{
			//Ч������
			CMD_GR_SendMessage* pSendMessage = (CMD_GR_SendMessage*)pData;

			//Ч������
			//ASSERT(wDataSize == sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage) + sizeof(TCHAR)*pSendMessage->wChatLength);
			if (wDataSize != sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage) + sizeof(TCHAR)*pSendMessage->wChatLength)
			{
				return false;
			}

			SendSystemMessage(pSendMessage->szSystemMessage, 0);

			return true;
		}
	case SUB_CS_S_PROPERTY_TRUMPET:  //������Ϣ
		{
			//�㲥����
			m_pITCPNetworkEngine->SendDataBatch(MDM_GC_USER,SUB_GC_TRUMPET_NOTIFY,pData,wDataSize,0xFF);
			return true;
		}

	}	

	return true;
}

bool CAttemperEngineSink::OnEventTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_CHAT_INSERT:	//�������
	{
		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;
		SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

		return true;
	}
	case SUB_CS_S_SIGNUP_COUNT:		//��������
	{
		//Ч������
		//ASSERT(wDataSize == sizeof(CMD_CS_C_MatchSignUpCount));
		if (wDataSize != sizeof(CMD_CS_C_MatchSignUpCount))
		{
			return false;
		}

		//��Ϣ����
		CMD_CS_C_MatchSignUpCount *pMatchSignUp = (CMD_CS_C_MatchSignUpCount *)pData;

		//���ҷ���		
		CMatchOptionItem *pGlobalServerItem = m_MatchServerManager.SearchMatchOption(pMatchSignUp->wServerID);

		//�����޸�
		if (pGlobalServerItem != NULL)
		{
			//���ñ���
			pGlobalServerItem->m_GameMatchOption.dwMatchCount = pMatchSignUp->dwMatchCount;
		}
		else
		{
			//��������
			tagMatchSignUpCount MatchSignUpCount;

			MatchSignUpCount.lMatchNo = pMatchSignUp->lMatchNo;
			MatchSignUpCount.dwMatchCount = pMatchSignUp->dwMatchCount;
			MatchSignUpCount.wServerID = pMatchSignUp->wServerID;

			m_MatchServerManager.InsertMatchOption(&MatchSignUpCount);
		}

		//��������
		CMD_GS_S_MatchSignUpCount MatchSignUp;

		//��������
		MatchSignUp.lMatchNo = pMatchSignUp->lMatchNo;
		MatchSignUp.dwMatchCount = pMatchSignUp->dwMatchCount;
		MatchSignUp.wServerID = pMatchSignUp->wServerID;

		CServerUserItem *pServerUserItem = NULL;
		//ö������
		for (INT i = 0; i < (int)m_ServerUserManager.GetUserItemCount(); i++)
		{
			//��ȡ����
			pServerUserItem = m_ServerUserManager.EnumUserItem(i);
			if (pServerUserItem == NULL)
			{
				break;
			}

			//��ȡ����
			CLocalUserItem *pLocalUserItem = (CLocalUserItem *)pServerUserItem;

			//��������
			m_pITCPNetworkEngine->SendData(pLocalUserItem->GetSocketID(), MDM_GC_MATCH, SUB_GC_MY_MATCH_PLAYER_COUNT, &MatchSignUp, sizeof(CMD_GS_S_MatchSignUpCount));
		}

		break;
	}
	case SUB_CS_S_MATCH_OPTION:		//��������	
		return true;
	}

	return true;
}


bool CAttemperEngineSink::OnEventTCPSocketMainServicePersonalRoomInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
		case SUB_CS_S_CHAT_INSERT:	//�������
		{
			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess = ER_SUCCESS;
			SendUIControlPacket(UI_PERSONAL_ROOM_RESULT, &ControlResult, sizeof(ControlResult));


			//OutputDebugString(TEXT("ptdt *** ��������� SUB_CS_S_CHAT_INSERT"));
			return true;
		}

	}

	return true;
}

bool CAttemperEngineSink::OnEventTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_USER_GAMESTATE:	//�������
		{
			return OnEventTCPSocketSubUserStatus(pData,wDataSize);
		}
	}	

	return true;
}


bool CAttemperEngineSink::OnEventTCPSocketPersonalRoomInfoCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_PERSONAL_INFO:	//Լս����Ϣ
		{
			return OnEventTCPSocketSubPersonalInfo(pData, wDataSize);
		}
	case SUB_CS_S_PERSONAL_INFO_UPDATE:	//����Լս����Ϣ
		{
			return OnEventTCPSocketSubPersonalInfoUpdate(pData, wDataSize);
		}
	case SUB_CS_S_DELETE_PERSONAL:	//����Լս����Ϣ
		{
			return OnEventTCPSocketSubDeletePersonalInfo(pData, wDataSize);
		}
	}

	return true;
}



bool CAttemperEngineSink::OnEventTCPSocketSubUserStatus(VOID * pData, WORD wDataSize)
{
	//������֤
	//ASSERT(wDataSize==sizeof(CMD_CS_S_UserGameStatus));
	if(wDataSize!=sizeof(CMD_CS_S_UserGameStatus)) return false;

	//��ȡ����
	CMD_CS_S_UserGameStatus * pUserGameStatus = (CMD_CS_S_UserGameStatus *)pData;
	//ASSERT(pUserGameStatus!=NULL);
	if(pUserGameStatus==NULL) return false;

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserGameStatus->dwUserID);
	if(pServerUserItem==NULL ) return true;

	//��ȡ����
	CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;

	//�ذ��ж�
	if((pUserGameStatus->cbGameStatus==pLocalUserItem->GetGameStatus()) &&
		(pUserGameStatus->wServerID==pLocalUserItem->GetServerID()) && 
		(pUserGameStatus->wTableID==pLocalUserItem->GetTableID()) )
	{
		return true;
	}

	//����״̬
	tagFriendUserInfo* pUserInfo=pLocalUserItem->GetUserInfo();
	pUserInfo->cbGameStatus=pUserGameStatus->cbGameStatus;
	pUserInfo->wKindID = pUserGameStatus->wKindID;
	pUserInfo->wServerID=pUserGameStatus->wServerID;
	pUserInfo->wTableID=pUserGameStatus->wTableID;
	pUserInfo->wChairID=pUserGameStatus->wChairID;
	lstrcpyn(pUserInfo->szServerName,pUserGameStatus->szServerName,CountArray(pUserInfo->szServerName));

	//�㲥״̬
	CMD_GC_UserGameStatusNotify UserGameStatusNotify;
	UserGameStatusNotify.dwUserID=pUserGameStatus->dwUserID;
	UserGameStatusNotify.cbGameStatus=pUserGameStatus->cbGameStatus;
	UserGameStatusNotify.wKindID = pUserGameStatus->wKindID;
	UserGameStatusNotify.wServerID=pUserGameStatus->wServerID;
	UserGameStatusNotify.wTableID=pUserGameStatus->wTableID;
	lstrcpyn(UserGameStatusNotify.szServerName,pUserGameStatus->szServerName,CountArray(UserGameStatusNotify.szServerName));

	//�㲥������
	SendDataToUserFriend(pUserGameStatus->dwUserID,MDM_GC_USER,SUB_GC_GAME_STATUS_NOTIFY,&UserGameStatusNotify,sizeof(UserGameStatusNotify));


	return true;
}

//Լս������Ϣ
bool CAttemperEngineSink::OnEventTCPSocketSubPersonalInfo(VOID * pData, WORD wDataSize)
{
	//������֤
	//ASSERT(wDataSize == sizeof(tagPersonalTableInfoEx));
	if (wDataSize != sizeof(tagPersonalTableInfoEx)) return false;

	//��ȡ����
	tagPersonalTableInfoEx * pPersonalTableInfoEx = (tagPersonalTableInfoEx *)pData;
	//ASSERT(pPersonalTableInfoEx != NULL);
	if (pPersonalTableInfoEx == NULL) return false;

	m_PersonalRoomManager.InsertPersonalTableItem(pPersonalTableInfoEx);


	//��������
	CMD_SC_C_PersonalGoldRoomListResult PersonalGoldRoomListResult;

	PersonalGoldRoomListResult.dwServerID = pPersonalTableInfoEx->dwServerID;
	PersonalGoldRoomListResult.dwKindID = pPersonalTableInfoEx->dwKindID;
	PersonalGoldRoomListResult.dwTableID = pPersonalTableInfoEx->dwTableID;
	PersonalGoldRoomListResult.dwUserID = pPersonalTableInfoEx->dwUserID;
	PersonalGoldRoomListResult.lCellScore = pPersonalTableInfoEx->lCellScore;
	PersonalGoldRoomListResult.dwPersonalRoomID = pPersonalTableInfoEx->dwPersonalRoomID;
	PersonalGoldRoomListResult.dwRoomTax = pPersonalTableInfoEx->dwRoomTax;
	PersonalGoldRoomListResult.cbPlayMode = pPersonalTableInfoEx->cbPlayMode;
	PersonalGoldRoomListResult.wJoinGamePeopleCount = pPersonalTableInfoEx->wJoinGamePeopleCount;
	PersonalGoldRoomListResult.lEnterScoreLimit = pPersonalTableInfoEx->lEnterScoreLimit;
	PersonalGoldRoomListResult.lLeaveScoreLimit = pPersonalTableInfoEx->lLeaveScoreLimit;
	PersonalGoldRoomListResult.bGameStart = pPersonalTableInfoEx->bGameStart;
	memcpy_s(PersonalGoldRoomListResult.cbGameRule, sizeof(PersonalGoldRoomListResult.cbGameRule), pPersonalTableInfoEx->cbGameRule, sizeof(pPersonalTableInfoEx->cbGameRule));
	memcpy_s(PersonalGoldRoomListResult.szNickName, sizeof(PersonalGoldRoomListResult.szNickName), pPersonalTableInfoEx->szNickName, sizeof(pPersonalTableInfoEx->szNickName));
	memcpy_s(PersonalGoldRoomListResult.dwJoinUserID, sizeof(PersonalGoldRoomListResult.dwJoinUserID), pPersonalTableInfoEx->dwJoinUserID, sizeof(pPersonalTableInfoEx->dwJoinUserID));
	memcpy_s(PersonalGoldRoomListResult.dwFaceID, sizeof(PersonalGoldRoomListResult.dwFaceID), pPersonalTableInfoEx->dwFaceID, sizeof(pPersonalTableInfoEx->dwFaceID));
	memcpy_s(PersonalGoldRoomListResult.dwCustomID, sizeof(PersonalGoldRoomListResult.dwCustomID), pPersonalTableInfoEx->dwCustomID, sizeof(pPersonalTableInfoEx->dwCustomID));
	memcpy_s(PersonalGoldRoomListResult.szJoinUserNickName, sizeof(PersonalGoldRoomListResult.szJoinUserNickName), pPersonalTableInfoEx->szJoinUserNickName, sizeof(pPersonalTableInfoEx->szJoinUserNickName));


	//��������	
	m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_INSERT_TABLE_INFO, &PersonalGoldRoomListResult, sizeof(PersonalGoldRoomListResult), 0xFF);

	//OutputDebugString(TEXT("ptdt *** ��������� �յ���Ϣ"));



	return true;
}

//����˽�˷���Ϣ 
bool CAttemperEngineSink::OnEventTCPSocketSubPersonalInfoUpdate(VOID * pData, WORD wDataSize)
{
	//������֤
	//ASSERT(wDataSize == sizeof(CMD_CS_S_UpdateTable));
	if (wDataSize != sizeof(CMD_CS_S_UpdateTable)) return false;

	//��ȡ����
	CMD_CS_S_UpdateTable * pUpdateTable = (CMD_CS_S_UpdateTable *)pData;
	//ASSERT(pUpdateTable != NULL);
	if (pUpdateTable == NULL) return false;

	//��ȡͬ��������Ϸ��������
	int nCount = m_PersonalRoomManager.GetPersonalTableCount(pUpdateTable->dwKindID);
	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdt *** OnEventTCPSocketSubPersonalInfoUpdate �������� kindid = %d  pUpdateTable->dwUserID = %d"), pUpdateTable->dwKindID, pUpdateTable->dwUserID);
	//OutputDebugString(szInfo);
	//�������з���
	for (int i = 0; i < nCount; i++)
	{
		tagPersonalTableInfoEx * pPersonalTableInfoEx = m_PersonalRoomManager.EnumPersonalTableItem(pUpdateTable->dwKindID, 1, i);
		if (pPersonalTableInfoEx->dwPersonalRoomID == pUpdateTable->dwPersonalRoomID)
		{
			pPersonalTableInfoEx->bGameStart = pUpdateTable->bGameStart;
	
			//����������Ϣ
			CMD_SC_C_PersonalGoldRoomUpdate	PersonalGoldRoomUpdate;
			ZeroMemory(&PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate));

			PersonalGoldRoomUpdate.bGameStart = pUpdateTable->bGameStart;


			PersonalGoldRoomUpdate.dwServerID = pUpdateTable->dwServerID;
			PersonalGoldRoomUpdate.dwKindID = pUpdateTable->dwKindID;
			PersonalGoldRoomUpdate.dwTableID = pUpdateTable->wTableID;
			PersonalGoldRoomUpdate.dwPersonalRoomID = pUpdateTable->dwPersonalRoomID;

			if (PersonalGoldRoomUpdate.bGameStart)
			{
				PersonalGoldRoomUpdate.cbStatus = 4;
				wsprintf(szInfo, TEXT("ptdt *** OnEventTCPSocketSubPersonalInfoUpdate �������� kindid = %d  pUpdateTable->dwUserID = %d  cbStatus = %d"), pUpdateTable->dwKindID, pUpdateTable->dwUserID, PersonalGoldRoomUpdate.cbStatus);
				//OutputDebugString(szInfo);


				//��������	
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_UPDATE_TABLE_INFO, &PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate), 0xFF);
				return true;
			}
			else
			{
				if (!PersonalGoldRoomUpdate.bGameStart && pUpdateTable->dwUserID == 0)
				{
					PersonalGoldRoomUpdate.cbStatus = 5;
					wsprintf(szInfo, TEXT("ptdt *** OnEventTCPSocketSubPersonalInfoUpdate �������� kindid = %d  pUpdateTable->dwUserID = %d  cbStatus = %d"), pUpdateTable->dwKindID, pUpdateTable->dwUserID, PersonalGoldRoomUpdate.cbStatus);
					//OutputDebugString(szInfo);

					//��������	
					m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_UPDATE_TABLE_INFO, &PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate), 0xFF);
				}
			}

			//�����û�
			CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUpdateTable->dwUserID);
			if (pServerUserItem == NULL) return true;

			//��ȡ����
			CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;

			tagFriendUserInfo * pUserInfo = pLocalUserItem->GetUserInfo();

			pPersonalTableInfoEx->dwJoinUserID[pUpdateTable->wChairID] = pUpdateTable->dwUserID;
			
			if (pUpdateTable->bSit)
			{
				PersonalGoldRoomUpdate.cbStatus = 0;

				pPersonalTableInfoEx->dwCustomID[pUpdateTable->wChairID] = pUserInfo->dwCustomID;
				pPersonalTableInfoEx->dwFaceID[pUpdateTable->wChairID] = pUserInfo->dwFaceID;

				//��ȡ����ǳ�
				lstrcpyn(pPersonalTableInfoEx->szJoinUserNickName[pUpdateTable->wChairID], pLocalUserItem->GetNickName(), LEN_NICKNAME * 2);

				lstrcpyn(PersonalGoldRoomUpdate.szNickName, pLocalUserItem->GetNickName(), LEN_NICKNAME * 2);
			}
			else
			{
				PersonalGoldRoomUpdate.cbStatus = 1;

				pPersonalTableInfoEx->dwCustomID[pUpdateTable->wChairID] = -1;
				pPersonalTableInfoEx->dwFaceID[pUpdateTable->wChairID] = -1;
				pPersonalTableInfoEx->dwJoinUserID[pUpdateTable->wChairID] = 0;

				//��ȡ����ǳ�
				ZeroMemory(pPersonalTableInfoEx->szJoinUserNickName[pUpdateTable->wChairID], LEN_NICKNAME * 2);

			}



			wsprintf(szInfo, TEXT("ptdt *** OnEventTCPSocketSubPersonalInfoUpdate �������� kindid = %d  pUpdateTable->dwUserID = %d  cbStatus = %d"), pUpdateTable->dwKindID, pUpdateTable->dwUserID, PersonalGoldRoomUpdate.cbStatus);
			//OutputDebugString(szInfo);


			PersonalGoldRoomUpdate.dwUserID = pUpdateTable->dwUserID;
			PersonalGoldRoomUpdate.wChairID = pUpdateTable->wChairID;
			PersonalGoldRoomUpdate.dwCustomID = pUserInfo->dwCustomID;
			PersonalGoldRoomUpdate.dwFaceID = pUserInfo->dwFaceID;

			//��������	
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_UPDATE_TABLE_INFO, &PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate), 0xFF);
		}
	}

	return true;
}

//Լս������Ϣ
bool CAttemperEngineSink::OnEventTCPSocketSubDeletePersonalInfo(VOID * pData, WORD wDataSize)
{
	//������֤
	//ASSERT(wDataSize == sizeof(tagPersonalTableInfoEx));
	if (wDataSize != sizeof(tagPersonalTableInfoEx)) return false;

	//��ȡ����
	tagPersonalTableInfoEx * pPersonalTableInfoEx = (tagPersonalTableInfoEx *)pData;
	//ASSERT(pPersonalTableInfoEx != NULL);
	if (pPersonalTableInfoEx == NULL) return false;

	//ɾ��Լս
	m_PersonalRoomManager.DeletePersonalTableItem(pPersonalTableInfoEx->dwKindID, pPersonalTableInfoEx->dwServerID, pPersonalTableInfoEx->dwPersonalRoomID);




	CMD_SC_C_PersonalGoldRoomUpdate	PersonalGoldRoomUpdate;
	ZeroMemory(&PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate));


	PersonalGoldRoomUpdate.cbStatus = 2;
	PersonalGoldRoomUpdate.dwServerID = pPersonalTableInfoEx->dwServerID;
	PersonalGoldRoomUpdate.dwKindID = pPersonalTableInfoEx->dwKindID;
	PersonalGoldRoomUpdate.dwTableID = pPersonalTableInfoEx->dwTableID;
	PersonalGoldRoomUpdate.dwPersonalRoomID = pPersonalTableInfoEx->dwPersonalRoomID;

	//��������	
	m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_UPDATE_TABLE_INFO, &PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate), 0xFF);

	return true;
}


//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//���ñ���
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;
		pBindParameter->dwClientAddr=dwClientAddr;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);

		return true;
	}

	//�������
	//ASSERT(FALSE);

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	if(pBindParameter==NULL) return false;

	//��ȡ�û�
	CServerUserItem * pServerUserItem =pBindParameter->pServerUserItem;

	try
	{
		//�û�����
		if (pServerUserItem!=NULL)
		{
			//״̬�ж�
			if(pServerUserItem->GetMainStatus()==FRIEND_US_OFFLINE) return true;

			//����״̬
			CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
			tagFriendUserInfo * pUserInfo = pLocalUserItem->GetUserInfo();
			pUserInfo->cbMainStatus=FRIEND_US_OFFLINE;
			pUserInfo->cbGameStatus=US_NULL;
			pUserInfo->wKindID=INVALID_KIND;
			pUserInfo->wServerID=INVALID_SERVER;
			pUserInfo->wTableID=INVALID_TABLE;
			pUserInfo->wChairID=INVALID_CHAIR;
			ZeroMemory(pUserInfo->szServerName,CountArray(pUserInfo->szServerName));

			//TCHAR szMessage[256]=TEXT("");
			//_sntprintf(szMessage,CountArray(szMessage),TEXT("%d-----����֪ͨ"),pServerUserItem->GetUserID());
			//CTraceService::TraceString(szMessage,TraceLevel_Normal);

			//�û�����
			CMD_GC_UserOnlineStatusNotify UserOnLine;
			UserOnLine.dwUserID=pServerUserItem->GetUserID();
			UserOnLine.cbMainStatus=pServerUserItem->GetMainStatus();	
			SendDataToUserFriend(UserOnLine.dwUserID,MDM_GC_USER,SUB_GC_USER_STATUS_NOTIFY,&UserOnLine,sizeof(UserOnLine));
		}
	}
	catch(...)
	{
		TCHAR szMessage[128]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("�ر������쳣: dwSocketID=%d"),dwSocketID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}
	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GC_LOGON:		//��¼����
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GC_USER:		//�û�����
		{
			return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GC_MATCH:
		{
			return OnTCPNetworkMainMatch(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case MDM_CS_PERSONAL:
		{
			return OnTCPNetworkMainPersonal(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	}

	return true;
}


//��������
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������MAX_CONTENT
	m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);

	return true;
}


//��������
bool CAttemperEngineSink::SendChatServerConfig(DWORD dwUserID)
{
	CMD_GC_ConfigServer ConfigServer;
	ZeroMemory(&ConfigServer,sizeof(ConfigServer));
	ConfigServer.wMaxPlayer=m_pInitParameter->m_wMaxPlayer;
	ConfigServer.dwServiceRule=m_pInitParameter->m_dwServiceRule;
	ConfigServer.cbMinOrder=m_pInitParameter->m_cbMinOrder;
	//SendData(pIServerUserItem,MDM_GC_CONFIG,SUB_GC_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));
	return true;
}

//��������
bool CAttemperEngineSink::SendSystemMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_GC_S_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_pITCPNetworkEngine->SendDataBatch(MDM_GC_USER,SUB_GC_SYSTEM_MESSAGE,&SystemMessage,wSendSize,0xFF);

	return true;
}

//��������
bool CAttemperEngineSink::SendSystemMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_GC_S_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GC_USER,SUB_GC_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//���ʹ���
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	return true;
}


//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GC_PC_LOGON_USERID:		//I D ��¼
	case SUB_GC_MB_LOGON_USERID:		//I D ��¼
		{
			return OnTCPNetworkSubMBLogonByUserID(pData,wDataSize,dwSocketID);
		}
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GC_APPLYFOR_FRIEND:		//��������
		{
			return OnTCPNetworkSubApplyForFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_RESPOND_FRIEND:			//���ѻ�Ӧ
		{
			return OnTCPNetworkSubRespondFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_INVITE_GAME:			//������Ϸ
		{
			return OnTCPNetworkSubRoomInvite(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_INVITE_PERSONAL:
		{
			return OnTCPNetworkSubInvitePersonal(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_USER_CHAT:				//�û�����
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_SEARCH_USER:			//�����û�
		{
			return OnTCPNetworkSubSearchUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_TRUMPET:				//������Ϣ
		{
			return OnTCPNetworkSubTrumpet(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_DELETE_FRIEND:			//ɾ������
		{
			return OnTCPNetworkDeleteFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_UPDATE_COORDINATE:
		{
			return OnTCPNetworkUpdateCoordinate(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_GET_NEARUSER:
		{
			return OnTCPNetworkGetNearuser(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_QUERY_NEARUSER:
		{
			return OnTCPNetworkQueryNearuser(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_USER_SHARE:
		{
			return OnTCPNetworkSubUserShare(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GC_SIGN_UP:			//���뱨��
		return OnTCPNetworkSubSignUp(pData, wDataSize, dwSocketID);
	case SUB_GC_UN_SIGNUP:			//ȡ������
		return  OnTCPNetworkSubUnSignUp(pData, wDataSize, dwSocketID);
	case SUB_GC_MY_MATCH_HISTORY:	//�ҵı���
		return  OnTCPNetworkMyMatchHistory(pData, wDataSize, dwSocketID);
	case SUB_GC_MY_MATCH_PLAYER_COUNT: //��������
		return  OnTCPNetworkMatchPlayerCount(pData, wDataSize, dwSocketID);
	}
	return true;
}

//Լս����
bool CAttemperEngineSink::OnTCPNetworkMainPersonal(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CMD_CS_C_GET_GOLD_LIST:			//����Լս��ҳ������б���Ϣ
		return OnTCPNetworkSubPersonalGoldRoomList(pData, wDataSize, dwSocketID);
	}
	return true;
}


//I D ��¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonByUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_MB_LogonByUserID));
	if (wDataSize>sizeof(CMD_GC_MB_LogonByUserID)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//������Ϣ
	CMD_GC_MB_LogonByUserID * pLogonUserID=(CMD_GC_MB_LogonByUserID *)pData;
	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;

	//��������
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//��������
	LogonUserID.dwUserID=pLogonUserID->dwUserID;
	LogonUserID.dwClientAddr=pBindParameter->dwClientAddr;
	LogonUserID.wLogonComand=LOGON_COMMAND_USERINFO|LOGON_COMMAND_GROUPINFO|LOGON_COMMAND_FRIENDS;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szPhoneMode,pLogonUserID->szPhoneMode,CountArray(LogonUserID.szPhoneMode));

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);
	if(pServerUserItem!=NULL)
	{
		//�����û�
		if(pServerUserItem->GetUserItemKind()==enLocalKind)
		{
			//��������
			CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;			

			//У������
			if(pLocalUserItem->ContrastLogonPass(pLogonUserID->szPassword)==false)
			{
				SendLogonFailure(TEXT("��������˺Ż����벻��ȷ,��ȷ�Ϻ����µ�¼��"),0,dwSocketID);

				return true;
			}

			//�ذ��ж�
			if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)
			{
				if(GetTickCount()-pLocalUserItem->GetLogonTime()< LOGON_TIME_SPACE)
				{
					return true;
				}
			}

			//����������Ϣ
			//if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)//&& (pServerUserItem->GetClientAddr()!=pBindParameter->dwClientAddr ))
			{
				if(pLocalUserItem->GetSocketID()!=dwSocketID)
				{
					//TCHAR szMessage[256]=TEXT("");
					//_sntprintf(szMessage,CountArray(szMessage),TEXT("dwSocketID0=%d,Address0=%d,dwSocketID1=%d,Address1=%d"),
					//	pLocalUserItem->GetSocketID(),pLocalUserItem->GetClientAddr(),dwSocketID,pBindParameter->dwClientAddr);

					////������Ϣ
					//CTraceService::TraceString(szMessage,TraceLevel_Normal);	

					//����ṹ
					CMD_GC_UserLogonAfresh UserLogonAfresh;
					_sntprintf(UserLogonAfresh.wNotifyMessage,CountArray(UserLogonAfresh.wNotifyMessage),TEXT("�����˺��ڱ𴦵�¼,���������ߣ�"));

					//��������
					WORD wSize = sizeof(UserLogonAfresh)-sizeof(UserLogonAfresh.wNotifyMessage)+CountStringBuffer(UserLogonAfresh.wNotifyMessage);
					SendData(pLocalUserItem->GetSocketID(),MDM_GC_LOGON,SUB_S_LOGON_AFRESH,&UserLogonAfresh,wSize);

				}
			}			

			//�޸��û�״̬
			pLocalUserItem->SetSocketID(dwSocketID);
			pLocalUserItem->SetLogonTime(GetTickCount());
			pLocalUserItem->GetUserInfo()->cbMainStatus=FRIEND_US_ONLINE;
			pLocalUserItem->GetUserInfo()->dwClientAddr=pBindParameter->dwClientAddr;
		}
	}	

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));


	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubApplyForFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize==sizeof(CMD_GC_ApplyForFriend));
	if (wDataSize!=sizeof(CMD_GC_ApplyForFriend)) return false;

	//������Ϣ
	CMD_GC_ApplyForFriend * pApplyForFriend=(CMD_GC_ApplyForFriend *)pData;
	//ASSERT(pApplyForFriend!=NULL);

	//���Ҷ���
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pApplyForFriend->dwUserID);
	if(pUserFriendGroup!=NULL) 
	{
		tagServerFriendInfo * pServerFriendInfo = pUserFriendGroup->SearchFriendItem(pApplyForFriend->dwFriendID);
		if(pServerFriendInfo!=NULL && pServerFriendInfo->cbGroupID!=0) 
		{
			//���뷴��
			CMD_GC_ApplyForFriendEcho ApplyForFriend;
			ZeroMemory(&ApplyForFriend,sizeof(ApplyForFriend));
			ApplyForFriend.lErrorCode=CHAT_MSG_ERR;
			lstrcpyn(ApplyForFriend.szDescribeString,_T("�Ѿ��Ǻ��ѣ���������"),CountArray(ApplyForFriend.szDescribeString));
			WORD wDataSize2=CountStringBuffer(ApplyForFriend.szDescribeString);
			WORD wHeadSize=sizeof(ApplyForFriend)-sizeof(ApplyForFriend.szDescribeString);
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_APPLYFOR_FRIEND_ECHO,&ApplyForFriend,wHeadSize+wDataSize2);

			return true;
		}
	}

	//�����û�
	CServerUserItem * pFriendUserItem = m_ServerUserManager.SearchUserItem(pApplyForFriend->dwFriendID);

	//��������
	bool bBufferMessage=false;
	if(pFriendUserItem==NULL) bBufferMessage=true;
	if(pFriendUserItem && pFriendUserItem->GetMainStatus()==FRIEND_US_OFFLINE) bBufferMessage=true; 

	if(pFriendUserItem!=NULL)
	{
		//TCHAR szMessage[256]=TEXT("");
		//_sntprintf(szMessage,CountArray(szMessage),TEXT(".........%d�����%dΪ����,%d��ǰ״̬Ϊ%d"),pApplyForFriend->dwUserID,pApplyForFriend->dwFriendID,pApplyForFriend->dwFriendID,pFriendUserItem->GetMainStatus());

		////������Ϣ
		//CTraceService::TraceString(szMessage,TraceLevel_Normal);	
	}

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pApplyForFriend->dwUserID);
	if(pServerUserItem==NULL || pServerUserItem->GetUserItemKind()!=enLocalKind) return false;

	//����ṹ
	CMD_GC_ApplyForNotify ApplyForNotify;
	ApplyForNotify.dwRequestID = pApplyForFriend->dwUserID;
	ApplyForNotify.cbGroupID=pApplyForFriend->cbGroupID;
	lstrcpyn(ApplyForNotify.szNickName,((CLocalUserItem *)pServerUserItem)->GetNickName(),CountArray(ApplyForNotify.szNickName));

	//�����ж�
	if(bBufferMessage==true)
	{
		SaveOfflineMessage(pApplyForFriend->dwFriendID,SUB_GC_APPLYFOR_NOTIFY,&ApplyForNotify,sizeof(ApplyForNotify),dwSocketID);
	}
	else
	{
		if(pFriendUserItem->GetUserItemKind()==enLocalKind)
		{
			SendData(((CLocalUserItem *)pFriendUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_APPLYFOR_NOTIFY,&ApplyForNotify,sizeof(ApplyForNotify));
		}
	}

	//���뷴��
	CMD_GC_ApplyForFriendEcho ApplyForFriend;
	ZeroMemory(&ApplyForFriend,sizeof(ApplyForFriend));
	ApplyForFriend.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(ApplyForFriend.szDescribeString,_T(""),CountArray(ApplyForFriend.szDescribeString));
	WORD wDataSize2=CountStringBuffer(ApplyForFriend.szDescribeString);
	WORD wHeadSize=sizeof(ApplyForFriend)-sizeof(ApplyForFriend.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_APPLYFOR_FRIEND_ECHO,&ApplyForFriend,wHeadSize+wDataSize2);


	return true;
}

//���ѻ�Ӧ
bool CAttemperEngineSink::OnTCPNetworkSubRespondFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize==sizeof(CMD_GC_RespondFriend));
	if (wDataSize!=sizeof(CMD_GC_RespondFriend)) return false;

	//������Ϣ
	CMD_GC_RespondFriend * pRespondFriend=(CMD_GC_RespondFriend *)pData;
	//ASSERT(pRespondFriend!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pRespondFriend->dwUserID);
	CServerUserItem * pRequestUserItem = m_ServerUserManager.SearchUserItem(pRespondFriend->dwRequestID);
	if(pServerUserItem==NULL ) 
	{
		//��Ӧ����
		CMD_GC_RespondFriendEcho RespondFriendEcho;
		ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
		RespondFriendEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(RespondFriendEcho.szDescribeString,_T("δ�ҵ����û�"),CountArray(RespondFriendEcho.szDescribeString));
		WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
		WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);
		return true;
	}

	//���Ҷ���
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pRespondFriend->dwRequestID);
	if(pUserFriendGroup!=NULL) 
	{
		if(pUserFriendGroup->SearchFriendItem(pRespondFriend->dwUserID)!=NULL) 
		{
			//��Ӧ����
			CMD_GC_RespondFriendEcho RespondFriendEcho;
			ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
			RespondFriendEcho.lErrorCode=CHAT_MSG_ERR;
			lstrcpyn(RespondFriendEcho.szDescribeString,_T("�Ѿ��Ǻ���"),CountArray(RespondFriendEcho.szDescribeString));
			WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
			WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);
			return true;
		}
	}

	//��������
	if(pRespondFriend->bAccepted==true)
	{
		//����ṹ
		DBR_GR_AddFriend AddFriend;
		AddFriend.bLoadUserInfo = false;
		AddFriend.dwRequestUserID = pRespondFriend->dwRequestID;
		AddFriend.dwUserID = pRespondFriend->dwUserID;
		AddFriend.cbRequestGroupID = pRespondFriend->cbRequestGroupID;
		AddFriend.cbGroupID = pRespondFriend->cbGroupID;		
		if(pRequestUserItem==NULL )
		{
			AddFriend.bLoadUserInfo = true;
		}		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_ADD_FRIEND,dwSocketID,&AddFriend,sizeof(AddFriend));
	}
	else
	{
		//��������
		bool bBufferMessage=false;
		if(pRequestUserItem==NULL) bBufferMessage=true;
		if(pRequestUserItem && pRequestUserItem->GetMainStatus()==FRIEND_US_OFFLINE) bBufferMessage=true;

		//����ṹ
		CMD_GC_RespondNotify RespondNotify;
		_sntprintf(RespondNotify.szNotifyContent,CountArray(RespondNotify.szNotifyContent),TEXT("%s �ܾ������ĺ������룡"),((CLocalUserItem *)pServerUserItem)->GetNickName());

		//������Ϣ
		if(bBufferMessage==true)
		{
			WORD wSize = sizeof(RespondNotify)-sizeof(RespondNotify.szNotifyContent)+CountStringBuffer(RespondNotify.szNotifyContent);
			SaveOfflineMessage(pRespondFriend->dwRequestID,SUB_GC_RESPOND_NOTIFY,&RespondNotify,wSize,dwSocketID);

		}
		else
		{
			if(pRequestUserItem->GetUserItemKind()==enLocalKind)
			{
				SendData(((CLocalUserItem*)pRequestUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify));
			}
		}
	}

	//��Ӧ����
	CMD_GC_RespondFriendEcho RespondFriendEcho;
	ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
	RespondFriendEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(RespondFriendEcho.szDescribeString,_T(""),CountArray(RespondFriendEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
	WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);

	return true;
}

//������Ϸ
bool CAttemperEngineSink::OnTCPNetworkSubRoomInvite(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_InviteGame));
	if (wDataSize>sizeof(CMD_GC_InviteGame)) return false;

	//������Ϣ
	CMD_GC_InviteGame * pVipRoomInvite=(CMD_GC_InviteGame *)pData;
	//ASSERT(pVipRoomInvite!=NULL);
	pVipRoomInvite->szInviteMsg[128]='\0';

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwUserID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//�����û�
	CServerUserItem * pInvitedUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwInvitedUserID);
	if(pInvitedUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("δ�ҵ����û�"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//���߼��
	if(pInvitedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//���뷴��
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("�û��Ѿ�����"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//��������
	CMD_GC_InviteGameNotify VipRoomInviteNotify;
	VipRoomInviteNotify.dwInviteUserID=pVipRoomInvite->dwUserID;
	VipRoomInviteNotify.wKindID = pVipRoomInvite->wKindID;
	VipRoomInviteNotify.wServerID=pVipRoomInvite->wServerID;
	VipRoomInviteNotify.wTableID=pVipRoomInvite->wTableID;
	lstrcpyn(VipRoomInviteNotify.szInviteMsg,pVipRoomInvite->szInviteMsg,CountArray(VipRoomInviteNotify.szInviteMsg));

	//��������
	WORD wSize = sizeof(VipRoomInviteNotify);
	SendData(((CLocalUserItem *)pInvitedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_INVITE_GAME_NOTIFY,&VipRoomInviteNotify,wSize);

	//��Ӧ����
	CMD_GC_InviteGameEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}

//������Ϸ
bool CAttemperEngineSink::OnTCPNetworkSubInvitePersonal(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_InvitePersonalGame));
	if (wDataSize>sizeof(CMD_GC_InvitePersonalGame)) return false;

	//������Ϣ
	CMD_GC_InvitePersonalGame * pVipRoomInvite=(CMD_GC_InvitePersonalGame *)pData;
	//ASSERT(pVipRoomInvite!=NULL);
	pVipRoomInvite->szInviteMsg[128]='\0';

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwUserID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//�����û�
	CServerUserItem * pInvitedUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwInvitedUserID);
	if(pInvitedUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("δ�ҵ����û�"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//���߼��
	if(pInvitedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//���뷴��
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("�û��Ѿ�����"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//��������
	CMD_GC_InvitePersonalGameNotify VipRoomInviteNotify;
	VipRoomInviteNotify.dwInviteUserID=pVipRoomInvite->dwUserID;
	VipRoomInviteNotify.wKindID = pVipRoomInvite->wKindID;
	VipRoomInviteNotify.dwServerNumber=pVipRoomInvite->dwServerNumber;
	VipRoomInviteNotify.wTableID=pVipRoomInvite->wTableID;
	lstrcpyn(VipRoomInviteNotify.szInviteMsg,pVipRoomInvite->szInviteMsg,CountArray(VipRoomInviteNotify.szInviteMsg));

	//��������
	WORD wSize = sizeof(VipRoomInviteNotify);
	SendData(((CLocalUserItem *)pInvitedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_INVITE_PERSONAL_NOTIFY,&VipRoomInviteNotify,wSize);

	//��Ӧ����
	CMD_GC_InvitePersonalGameEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}


//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_UserChat));
	if (wDataSize>sizeof(CMD_GC_UserChat)) return false;

	//������Ϣ
	CMD_GC_UserChat * pUserChat=(CMD_GC_UserChat *)pData;
	//ASSERT(pUserChat!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwSenderID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//���췴��
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(UserChatEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);
		return false;
	}


	//�����û�
	CServerUserItem * pTargetUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
	if(pTargetUserItem!=NULL)
	{
		CLocalUserItem * pLocalUerItem = (CLocalUserItem *)pTargetUserItem;
		SendData(pLocalUerItem->GetSocketID(),MDM_GC_USER,SUB_GC_USER_CHAT_NOTIFY,pData,wDataSize);

		//���췴��
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(UserChatEcho.szDescribeString,_T(""),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);

	}
	else if(pTargetUserItem==NULL)
	{
		//���췴��
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(UserChatEcho.szDescribeString,_T("����ʧ�ܣ��Է��Ѿ�����"),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);

		////������Ϣ
		//SaveOfflineMessage(pUserChat->dwTargetUserID,SUB_GC_USER_CHAT_NOTIFY,pData,wDataSize,dwSocketID);
	}

	return true;
}

//�����û�
bool CAttemperEngineSink::OnTCPNetworkSubSearchUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_SearchByGameID));
	if (wDataSize>sizeof(CMD_GC_SearchByGameID)) return false;

	//������Ϣ
	CMD_GC_SearchByGameID * pSearchUser=(CMD_GC_SearchByGameID *)pData;
	//ASSERT(pSearchUser!=NULL);

	//��������
	DBR_GR_SearchUser SearchUser;
	SearchUser.dwSearchByGameID = pSearchUser->dwSearchByGameID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SEARCH_USER,dwSocketID,&SearchUser,sizeof(SearchUser));

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_Trumpet));
	if (wDataSize>sizeof(CMD_GC_Trumpet)) return false;

	//������Ϣ
	CMD_GC_Trumpet * pUserChat=(CMD_GC_Trumpet *)pData;
	//ASSERT(pUserChat!=NULL);

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwSendUserID);
	if(pServerUserItem==NULL ) 
	{
		//���ȷ���
		CMD_GC_TrumpetEcho TrumpetEcho;
		ZeroMemory(&TrumpetEcho,sizeof(TrumpetEcho));
		TrumpetEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(TrumpetEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(TrumpetEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(TrumpetEcho.szDescribeString);
		WORD wHeadSize=sizeof(TrumpetEcho)-sizeof(TrumpetEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_TRUMPET_ECHO,&TrumpetEcho,wHeadSize+wDataSize);
		return false;
	}


	DBR_GR_Trumpet Trumpet;
	Trumpet.wPropertyID = pUserChat->wPropertyID;
	Trumpet.dwSendUserID = pUserChat->dwSendUserID;
	Trumpet.TrumpetColor = pUserChat->TrumpetColor;
	Trumpet.dwClientAddr =pBindParameter->dwClientAddr;
	lstrcpyn(Trumpet.szSendNickName, pUserChat->szSendNickName, CountArray(Trumpet.szSendNickName));
	lstrcpyn(Trumpet.szTrumpetContent, pUserChat->szTrumpetContent, CountArray(Trumpet.szTrumpetContent));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_TRUMPET, dwSocketID, &Trumpet, sizeof(Trumpet));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkDeleteFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_DeleteFriend));
	if (wDataSize>sizeof(CMD_GC_DeleteFriend)) return false;

	//������Ϣ
	CMD_GC_DeleteFriend * pDeleteFriend=(CMD_GC_DeleteFriend *)pData;
	//ASSERT(pDeleteFriend!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		//ɾ������
		CMD_GC_DeleteFriendEcho DeleteEcho;
		ZeroMemory(&DeleteEcho,sizeof(DeleteEcho));
		DeleteEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(DeleteEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(DeleteEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(DeleteEcho.szDescribeString);
		WORD wHeadSize=sizeof(DeleteEcho)-sizeof(DeleteEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_DELETE_FRIEND_ECHO,&DeleteEcho,wHeadSize+wDataSize);
		return false;
	}

	//����ṹ
	DBR_GR_DeleteFriend DeleteFriend;
	DeleteFriend.dwUserID=pDeleteFriend->dwUserID;
	DeleteFriend.dwFriendUserID=pDeleteFriend->dwFriendUserID;
	DeleteFriend.cbGroupID=pDeleteFriend->cbGroupID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_FRIEND,dwSocketID,&DeleteFriend,sizeof(DeleteFriend));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkUpdateCoordinate(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_Update_Coordinate));
	if (wDataSize>sizeof(CMD_GC_Update_Coordinate)) return false;

	//������Ϣ
	CMD_GC_Update_Coordinate * pUpdateCoordinate=(CMD_GC_Update_Coordinate *)pData;
	//ASSERT(pUpdateCoordinate!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUpdateCoordinate->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		//���·���
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("��Ϣ��ʱ"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}

	if (pUpdateCoordinate->dLatitude>180 || pUpdateCoordinate->dLongitude>180)
	{
		//���·���
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("�������"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
		return true;
	}

	//���¶�λ
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	tagFriendUserInfo * pUserInfo = ((CLocalUserItem *)pServerUserItem)->GetUserInfo();
	pUserInfo->dwClientAddr = pBindParameter->dwClientAddr;
	pUserInfo->dLongitude = pUpdateCoordinate->dLongitude;
	pUserInfo->dLatitude = pUpdateCoordinate->dLatitude;
	lstrcpyn(pUserInfo->szPlaceName, pUpdateCoordinate->szPlaceName, CountArray(pUpdateCoordinate->szPlaceName));
	pUserInfo->cbCoordinate = 1;

	//CMD_GC_Update_CoordinateNotify Notify;
	//Notify.dLatitude = pUserInfo->dLatitude;
	//Notify.dLongitude = pUserInfo->dLongitude;
	//Notify.cbCoordinate = 1;
	//Notify.dwClientAddr = pUserInfo->dwClientAddr;
	//SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_NOTIFY,&Notify,sizeof(Notify));

	{
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(Echo.szDescribeString,_T(""),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
	}
	
	//����ṹ
	DBR_GR_UpdatePlaceName placeName;
	placeName.dwUserID = pUpdateCoordinate->dwUserID;
	lstrcpyn(placeName.szPlaceName, pUpdateCoordinate->szPlaceName, CountArray(placeName.szPlaceName));
	
	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_PLACENAME, dwSocketID, &placeName, sizeof(placeName));
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkGetNearuser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_Get_Nearuser));
	if (wDataSize>sizeof(CMD_GC_Get_Nearuser)) return false;

	//������Ϣ
	CMD_GC_Get_Nearuser * pGetNearuser=(CMD_GC_Get_Nearuser *)pData;
	//ASSERT(pGetNearuser!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pGetNearuser->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		CMD_GC_Get_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("��Ϣ��ʱ"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}

	//��ȡ����
	CNearUserManager  NearUserManager;
	m_ServerUserManager.GetNearUserItem(pGetNearuser->dwUserID,NearUserManager);

	CMD_GC_Get_NearuserResult Nearuser;
	BYTE cbTotalUserCount = (BYTE)(NearUserManager.GetUserItemCount());
	Nearuser.cbUserCount = cbTotalUserCount;
	ZeroMemory(&Nearuser.NearUserInfo,sizeof(Nearuser.NearUserInfo));
	if (cbTotalUserCount !=0)
	{
		int nIndex = 0;
		for (INT i = 0;i<cbTotalUserCount;i++)
		{
			tagNearUserInfo * pUserInfo = NearUserManager.EnumUserItem(i);
			//���볬������
			if((m_pInitParameter->m_dwDistance!=0)&&(pUserInfo->dwDistance>m_pInitParameter->m_dwDistance)) continue;

			CopyMemory(&(Nearuser.NearUserInfo[nIndex++]),pUserInfo,sizeof(tagNearUserInfo));
			if (nIndex == MAX_FRIEND_TRANSFER)
			{
				Nearuser.cbUserCount = nIndex;
                SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));
				nIndex = 0;
			}
		}
		if (nIndex !=0)
		{
			Nearuser.cbUserCount = nIndex;
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));
			nIndex = 0;
		}

	}    
	else
	{
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));
	}

	{
		CMD_GC_Get_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(Echo.szDescribeString,_T(""),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
	}


	return true;
}

bool CAttemperEngineSink::OnTCPNetworkQueryNearuser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_Query_Nearuser));
	if (wDataSize>sizeof(CMD_GC_Query_Nearuser)) return false;

	//������Ϣ
	CMD_GC_Query_Nearuser * pQueryNearuser=(CMD_GC_Query_Nearuser *)pData;
	//ASSERT(pQueryNearuser!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pQueryNearuser->dwUserID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		CMD_GC_Query_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("��Ϣ��ʱ"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}
	CServerUserItem * pNearuserUserItem = m_ServerUserManager.SearchUserItem(pQueryNearuser->dwNearuserUserID);
	if(pNearuserUserItem==NULL ) 
	{
		CMD_GC_Query_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("����ʧ�ܣ��Է��Ѿ�����"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return true;
	}
	//��ȡ����
	CNearUserManager  NearUserManager;
	m_ServerUserManager.QueryNearUserItem(pQueryNearuser->dwUserID,pQueryNearuser->dwNearuserUserID,NearUserManager);
	CMD_GC_Query_NearuserResult Nearuser;
	Nearuser.cbUserCount = (BYTE)(NearUserManager.GetUserItemCount());
	ZeroMemory(&Nearuser.NearUserInfo,sizeof(Nearuser.NearUserInfo));
	if (Nearuser.cbUserCount !=0)
	{
		tagNearUserInfo * pUserInfo = NearUserManager.EnumUserItem(0);
		CopyMemory(&Nearuser.NearUserInfo,pUserInfo,sizeof(tagNearUserInfo));
	}

	SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));

	{
		CMD_GC_Query_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(Echo.szDescribeString,_T(""),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
	}


	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubUserShare(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize<=sizeof(CMD_GC_UserShare));
	if (wDataSize>sizeof(CMD_GC_UserShare)) return false;

	//������Ϣ
	CMD_GC_UserShare * pUserShare=(CMD_GC_UserShare *)pData;
	//ASSERT(pUserShare!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserShare->dwUserID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//�����û�
	CServerUserItem * pSharedUserItem = m_ServerUserManager.SearchUserItem(pUserShare->dwSharedUserID);
	if(pSharedUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("δ�ҵ����û�"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//���߼��
	if(pSharedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//���뷴��
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("�û��Ѿ�����"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//��������
	CMD_GC_UserShareNotify UserShareNotify;
	UserShareNotify.dwShareUserID=pUserShare->dwUserID;
	lstrcpyn(UserShareNotify.szShareImageAddr,pUserShare->szShareImageAddr,CountArray(UserShareNotify.szShareImageAddr));
	lstrcpyn(UserShareNotify.szMessageContent,pUserShare->szMessageContent,CountArray(UserShareNotify.szMessageContent));

	//��������
	SendData(((CLocalUserItem *)pSharedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_USER_SHARE_NOTIFY,&UserShareNotify,sizeof(UserShareNotify));

	//��Ӧ����
	CMD_GC_UserShareEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}


//��������
bool CAttemperEngineSink::OnTCPNetworkSubSignUp(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize == sizeof(CMD_GC_MatchSignup));
	if (wDataSize != sizeof(CMD_GC_MatchSignup)) return false;

	//������Ϣ
	CMD_GC_MatchSignup * pMatchSignup = (CMD_GC_MatchSignup *)pData;
	pMatchSignup->szPassword[CountArray(pMatchSignup->szPassword) - 1] = 0;
	pMatchSignup->szMachineID[CountArray(pMatchSignup->szMachineID) - 1] = 0;

	//���ҷ���
	CMatchServerItem * pGameServerItem = m_MatchServerManager.SearchMatchServer(pMatchSignup->wServerID);
	if (pGameServerItem == NULL)
	{
		//����ʧ��
		SendOperateFailure(TEXT("��Ǹ���������ı��������ڻ����Ѿ�������"), 0, 0, dwSocketID);
		return true;
	}

	//����ṹ	
	DBR_GR_MatchSignup MatchSignup;

	//������Ϣ
	MatchSignup.wServerID = pMatchSignup->wServerID;
	MatchSignup.dwMatchID = pMatchSignup->dwMatchID;
	MatchSignup.lMatchNo = pMatchSignup->dwMatchNO;

	//�û���Ϣ
	MatchSignup.dwUserID = pMatchSignup->dwUserID;
	//	lstrcpyn(MatchSignup.szPassword, pMatchSignup->szPassword, CountArray(MatchSignup.szPassword));

	//������Ϣ
	//MatchSignup.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;

	lstrcpyn(MatchSignup.szMachineID, pMatchSignup->szMachineID, CountArray(MatchSignup.szMachineID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MATCH_SIGNUP, dwSocketID, &MatchSignup, sizeof(MatchSignup));

	return true;
}
//ȡ������
bool CAttemperEngineSink::OnTCPNetworkSubUnSignUp(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize == sizeof(CMD_GC_MatchUnSignup));
	if (wDataSize != sizeof(CMD_GC_MatchUnSignup)) return false;

	//������Ϣ
	CMD_GC_MatchUnSignup * pMatchUnSignup = (CMD_GC_MatchUnSignup *)pData;
	pMatchUnSignup->szPassword[CountArray(pMatchUnSignup->szPassword) - 1] = 0;
	pMatchUnSignup->szMachineID[CountArray(pMatchUnSignup->szMachineID) - 1] = 0;

	//����ṹ
	DBR_GR_MatchUnSignup MatchUnSignup;

	//������Ϣ
	MatchUnSignup.wServerID = pMatchUnSignup->wServerID;
	MatchUnSignup.dwMatchID = pMatchUnSignup->dwMatchID;
	MatchUnSignup.lMatchNo = pMatchUnSignup->dwMatchNO;

	//�û���Ϣ
	MatchUnSignup.dwUserID = pMatchUnSignup->dwUserID;
	//	lstrcpyn(MatchUnSignup.szPassword, pMatchUnSignup->szPassword, CountArray(MatchUnSignup.szPassword));

	//������Ϣ
	//MatchUnSignup.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(MatchUnSignup.szMachineID, pMatchUnSignup->szMachineID, CountArray(MatchUnSignup.szMachineID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MATCH_UNSIGNUP, dwSocketID, &MatchUnSignup, sizeof(MatchUnSignup));

	return true;
}

//�ҵı���
bool CAttemperEngineSink::OnTCPNetworkMyMatchHistory(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize == sizeof(CMD_GC_MyMatchHistory));
	if (wDataSize != sizeof(CMD_GC_MyMatchHistory)) return false;

	//������Ϣ
	CMD_GC_MyMatchHistory * pMyMatch = (CMD_GC_MyMatchHistory *)pData;
	pMyMatch->szPassword[CountArray(pMyMatch->szPassword) - 1] = 0;
	
	//����ṹ	
	DBO_GR_MyMatchHistory MyMatch;

	//�û���Ϣ
	MyMatch.dwUserID = pMyMatch->dwUserID;
	lstrcpyn(MyMatch.szPassword, pMyMatch->szPassword, CountArray(MyMatch.szPassword));
	
	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBO_GR_MY_MATCH_HISTORY, dwSocketID, &MyMatch, sizeof(MyMatch));

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkMatchPlayerCount(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{	
	//��������
	CMD_GS_S_MatchSignUpCount MatchSignUp;
	CMatchOptionItem *pGlobalServerItem = NULL;

	//ö������
	POSITION Position = NULL;
	do 
	{
		pGlobalServerItem = m_MatchServerManager.EmunMatchOptionItem(Position);
		if (pGlobalServerItem == NULL)
		{
			break;
		}
		//��������
		MatchSignUp.lMatchNo = pGlobalServerItem->m_GameMatchOption.lMatchNo;
		MatchSignUp.dwMatchCount = pGlobalServerItem->m_GameMatchOption.dwMatchCount;
		MatchSignUp.wServerID = pGlobalServerItem->m_GameMatchOption.wServerID;

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GC_MATCH, SUB_GC_MY_MATCH_PLAYER_COUNT, &MatchSignUp, sizeof(CMD_GS_S_MatchSignUpCount));
	} while(Position != NULL);
	
	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID)
{
	//��������
	CMD_GC_LogonEcho LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//��������
	LogonFailure.lErrorCode=lErrorCode;
	lstrcpyn(LogonFailure.szDescribeString,pszString,CountArray(LogonFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);

	//��������
	SendData(dwSocketID,MDM_GC_LOGON,SUB_GC_LOGON_FAILURE,&LogonFailure,wHeadSize+wDataSize);

	return true;
}

//�������
bool CAttemperEngineSink::SendOperateFailure(LPCTSTR pszString, LONG lErrorCode,LONG lOperateCode,DWORD dwSocketID)
{
	//��������
	CMD_GC_OperateFailure OperateFailure;
	OperateFailure.lErrorCode=lErrorCode;
	OperateFailure.lResultCode=lOperateCode;
	lstrcpyn(OperateFailure.szDescribeString,pszString,CountArray(OperateFailure.szDescribeString));

	//��������
	SendData(dwSocketID,MDM_GC_USER,SUB_GP_OPERATE_FAILURE,&OperateFailure,sizeof(OperateFailure));

	return true;
}

//�������
bool CAttemperEngineSink::SendOperateSuccess(LPCTSTR pszString,LONG lOperateCode,DWORD dwSocketID)
{
	//����ṹ
	CMD_GC_OperateSuccess OperateSuccess;
	OperateSuccess.lResultCode=lOperateCode;
	lstrcpyn(OperateSuccess.szDescribeString,pszString,CountArray(OperateSuccess.szDescribeString));

	//��������
	WORD wSize = sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+CountStringBuffer(OperateSuccess.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_OPERATE_SUCCESS,&OperateSuccess,wSize);

	return true;
}


//��������
bool CAttemperEngineSink::SendDataToUserFriend(DWORD dwUserID,WORD wMainCmdID, WORD wSubCmdID,VOID * pData,WORD wDataSize)
{
	//�Ƴ��û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(dwUserID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem!=NULL)
	{
		//��ȡ����
		CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(dwUserID);
		if(pUserFriendGroup!=NULL)
		{
			//ö���û�
			WORD wEnumIndex=0;
			tagServerFriendInfo * pServerFriendInfo = pUserFriendGroup->EnumFriendItem(wEnumIndex);
			while(pServerFriendInfo!=NULL)
			{
				//���ұ����û�
				CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pServerFriendInfo->dwUserID);
				if(pServerUserItem!=NULL )
				{
					CLocalUserItem * pLocalUerItem = (CLocalUserItem *)pServerUserItem;					
					if(pLocalUerItem->GetMainStatus()!=FRIEND_US_OFFLINE)
					{
						SendData(pLocalUerItem->GetSocketID(),wMainCmdID,wSubCmdID,pData,wDataSize);
					}
				}

				//ö�ٺ���
				pServerFriendInfo = pUserFriendGroup->EnumFriendItem(++wEnumIndex);
			}			
		}
	}

	return true;
}

//�洢������Ϣ
bool CAttemperEngineSink::SaveOfflineMessage(DWORD dwUserID,WORD wMessageType,VOID * pData,WORD wDataSize,DWORD dwSocketID)
{
	//��������
	DBR_GR_SaveOfflineMessage OfflineMessage;
	OfflineMessage.dwUserID = dwUserID;
	OfflineMessage.wMessageType = wMessageType;
	OfflineMessage.wDataSize = wDataSize;
	OfflineMessage.szOfflineData[0]=0;

	//��������
	TCHAR szOffLineData[CountArray(OfflineMessage.szOfflineData)+1]=TEXT("");
	BYTE cbOffLineData[CountArray(OfflineMessage.szOfflineData)/2];
	ZeroMemory(cbOffLineData,sizeof(cbOffLineData));
	CopyMemory(cbOffLineData,pData,wDataSize);		

	//������Ϣ
	for (INT i=0;i<CountArray(cbOffLineData);i++) 
	{
		_stprintf(&szOffLineData[i * 2], TEXT("%02X"), cbOffLineData[i]);
	}

	//���ñ���
	lstrcpyn(OfflineMessage.szOfflineData,szOffLineData,CountArray(OfflineMessage.szOfflineData));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SAVE_OFFLINEMESSAGE,dwSocketID,&OfflineMessage,sizeof(OfflineMessage));

	return true;
}

//���û�
CServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//��ȡ����
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��ȡ�û�
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pServerUserItem;
	}

	//�������
	//ASSERT(FALSE);

	return NULL;
}

//�󶨲���
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//��Ч����
	if (wBindIndex==INVALID_WORD) return NULL;

	//��������
	if (wBindIndex<m_pInitParameter->m_wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//�������
	//ASSERT(FALSE);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		switch (wRequestID)
		{
		case DBO_GR_LOGON_SUCCESS:			//��¼�ɹ�
			{
				return OnDBLogonSuccess(dwContextID,pData,wDataSize);
			}
		case DBO_GR_LOGON_FAILURE:			//��¼ʧ��
			{
				return OnDBLogonFailure(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_GROUPINFO:			//���ط���
			{
				return OnDBLoadUserGroup(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_FRIENDINFO:		//���غ���
			{
				return OnDBLoadUserFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_SINDIVIDUAL:		//��������
			{
				return OnDBLoadUserSimpleIndividual(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_REMARKS:			//�û���ע
			{
				return OnDBLoadUserRemarksInfo(dwContextID,pData,wDataSize);
			}
		case DBO_GR_LOAD_OFFLINEMESSAGE:	//������Ϣ
			{
				return OnDBLoadUserOfflineMessage(dwContextID,pData,wDataSize);
			}
		case DBO_GR_SEARCH_USER_RESULT:		//���ҽ��
			{
				return OnDBSearchUserResult(dwContextID,pData,wDataSize);
			}
		case DBO_GR_ADD_FRIEND:				//��Ӻ���
			{
				return OnDBUserAddFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_DELETE_FRIEND:			//ɾ������
			{
				return OnDBLoadDeleteFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_OPERATE_RESULT:			//�������
			{
				return OnDBUserOperateResult(dwContextID,pData,wDataSize);
			}
		case DBO_GR_TRUMPET_RESULT:
			{
				return OnDBTrumpetResult(dwContextID,pData,wDataSize);
			}
		case DBO_GR_SYSTEM_MESSAGE_RESULT:  //ϵͳ��Ϣ
			{
				return OnDBSystemMessage(dwContextID, pData, wDataSize);
			}
		case DBO_GR_SYSTEM_MESSAGE_FINISH:	//�������
			{
				return OnDBSystemMessageFinish(dwContextID, pData, wDataSize);
			}
		case DBO_GR_MATCH_SIGNUP_RESULT:	//�������
			{
				return OnDBPCUserMatchSignupResult(dwContextID, pData, wDataSize);
			}
		case DBO_GR_MATCH_HISTORY_RESULT_T:
			{
				return OnDBUserMatchHistoryResult_T(dwContextID, pData, wDataSize);
			}
		case DBO_GR_MATCH_HISTORY_RESULT_F:
			{
				return OnDBUserMyMatchHistoryResult(dwContextID, pData, wDataSize);
			}
		}
	}catch(...)
	{
		TCHAR szMessage[256]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("OnEventDataBase-wRequestID=%d"),wRequestID);

		//������Ϣ
		CTraceService::TraceString(szMessage,TraceLevel_Exception);
	}

	return false;
}

bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize<=sizeof(DBO_GR_LogonSuccess));
	if(wDataSize>sizeof(DBO_GR_LogonSuccess)) return false;

	//��������
	DBO_GR_LogonSuccess * pDBOLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	//�ظ���¼
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	if(pServerUserItem!=NULL)
	{
		m_ServerUserManager.DeleteUserItem(pDBOLogonSuccess->dwUserID);
	}


	//�û�����
	tagFriendUserInfo UserInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));

	//��������
	UserInfo.dwUserID=pDBOLogonSuccess->dwUserID;
	UserInfo.dwGameID = pDBOLogonSuccess->dwGameID;
	lstrcpyn(UserInfo.szNickName,pDBOLogonSuccess->szNickName,CountArray(UserInfo.szNickName));
	lstrcpyn(UserInfo.szPassword,pDBOLogonSuccess->szPassword,CountArray(UserInfo.szPassword));
	UserInfo.cbGender=pDBOLogonSuccess->cbGender;
	UserInfo.dwFaceID =pDBOLogonSuccess->dwFaceID;
	UserInfo.dwCustomID =pDBOLogonSuccess->dwCustomID;
	UserInfo.wMemberOrder = pDBOLogonSuccess->wMemberOrder;
	UserInfo.wGrowLevel =pDBOLogonSuccess->wGrowLevel;
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogonSuccess->szUnderWrite,CountArray(UserInfo.szUnderWrite));
	lstrcpyn(UserInfo.szCompellation,pDBOLogonSuccess->szCompellation,CountArray(UserInfo.szCompellation));
	
	//�û�״̬
	UserInfo.cbMainStatus=FRIEND_US_ONLINE;
	UserInfo.cbGameStatus=US_NULL;
	UserInfo.wKindID=INVALID_KIND;
	UserInfo.wServerID=INVALID_SERVER;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;
	ZeroMemory(UserInfo.szServerName,CountArray(UserInfo.szServerName));
	UserInfo.dwClientAddr=pDBOLogonSuccess->dwClientAddr;
	lstrcpyn(UserInfo.szPhoneMode,pDBOLogonSuccess->szPhoneMode,CountArray(UserInfo.szPhoneMode));

	//�û�����	
	lstrcpyn(UserInfo.szQQ,pDBOLogonSuccess->szQQ,CountArray(UserInfo.szQQ));
	lstrcpyn(UserInfo.szEMail,pDBOLogonSuccess->szEMail,CountArray(UserInfo.szEMail));
	lstrcpyn(UserInfo.szSeatPhone,pDBOLogonSuccess->szSeatPhone,CountArray(UserInfo.szSeatPhone));
	lstrcpyn(UserInfo.szMobilePhone,pDBOLogonSuccess->szMobilePhone,CountArray(UserInfo.szMobilePhone));
	lstrcpyn(UserInfo.szDwellingPlace,pDBOLogonSuccess->szDwellingPlace,CountArray(UserInfo.szDwellingPlace));
	lstrcpyn(UserInfo.szPostalCode,pDBOLogonSuccess->szPostalCode,CountArray(UserInfo.szPostalCode));

	//��������
	tagInsertLocalUserInfo InsertLocalUserInfo;
	InsertLocalUserInfo.dwSocketID=dwContextID;
	InsertLocalUserInfo.dwLogonTime=GetTickCount();

	//�����û�
	m_ServerUserManager.InsertLocalUserItem(InsertLocalUserInfo,UserInfo,pDBOLogonSuccess->szPassword);

	//CLocalUserItem *
	//��������
	CMD_GC_LogonEcho LogonSuccess;
	ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));
	WORD wLogonDataSize = sizeof(LogonSuccess);
	SendData(dwContextID,MDM_GC_LOGON,SUB_GC_LOGON_SUCCESS,&LogonSuccess,wLogonDataSize);

	//�����û�
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	pServerUserItem = m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	pBindParameter->pServerUserItem=pServerUserItem;

	//����ͷ���ַ
	CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
	pLocalUserItem->SetFaceUrl(pDBOLogonSuccess->szFaceUrl);


	//�û�����
	CMD_GC_UserOnlineStatusNotify UserOnLine;
	UserOnLine.dwUserID=pServerUserItem->GetUserID();
	UserOnLine.cbMainStatus=pServerUserItem->GetMainStatus();	
	SendDataToUserFriend(UserOnLine.dwUserID,MDM_GC_USER,SUB_GC_USER_STATUS_NOTIFY,&UserOnLine,sizeof(UserOnLine));

	//TCHAR szMessage[256]=TEXT("");
	//_sntprintf(szMessage,CountArray(szMessage),TEXT("%d-----����֪ͨ"),UserOnLine.dwUserID);
	//CTraceService::TraceString(szMessage,TraceLevel_Normal);

	//����Ⱥ��
	m_pITCPNetworkEngine->AllowBatchSend(dwContextID,true,0xFF);

	return true;
}



//���ط���
bool CAttemperEngineSink::OnDBLoadUserGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize<=sizeof(DBO_GR_UserGroupInfo));
	if(wDataSize>sizeof(DBO_GR_UserGroupInfo)) return false;

	//��ȡ����
	DBO_GR_UserGroupInfo * pDBOUserGroupInfo = (DBO_GR_UserGroupInfo *)pData;
	//ASSERT(pDBOUserGroupInfo);

	//��������
	tagClientGroupInfo * pTempGroupInfo=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[sizeof(tagClientGroupInfo)];
	WORD wDataIndex=0,wPacketSize=0;

	//ѭ������
	for(int nIndex=0;nIndex<pDBOUserGroupInfo->wGroupCount;++nIndex)
	{
		//���ñ���
		pTempGroupInfo = &pDBOUserGroupInfo->GroupInfo[nIndex];

		//��������		
		tagClientGroupInfo * pSendGroupInfo=(tagClientGroupInfo *)&cbDataBuffer;

		//���ñ���
		pSendGroupInfo->cbGroupIndex=pTempGroupInfo->cbGroupIndex;
		lstrcpyn(pSendGroupInfo->szGroupName,pTempGroupInfo->szGroupName,CountArray(pSendGroupInfo->szGroupName));

		//���ñ���
		wPacketSize = sizeof(tagClientGroupInfo)-sizeof(pSendGroupInfo->szGroupName)+CountStringBuffer(pSendGroupInfo->szGroupName);

		//�����ж�
		if(wDataIndex+wPacketSize+sizeof(BYTE)>CountArray(cbSendBuffer))
		{			
			//������Ϣ
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_GROUP,cbSendBuffer,wDataIndex);

			//���ñ���
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//��������
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(BYTE));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(BYTE)],cbDataBuffer,wPacketSize);

		//���ñ���
		wDataIndex += sizeof(BYTE)+wPacketSize;
	}

	//ʣ�෢��
	if(wDataIndex>0)
	{
		//������Ϣ
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_GROUP,cbSendBuffer,wDataIndex);			
	}


	return true;
}

//���غ���
bool CAttemperEngineSink::OnDBLoadUserFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize<=sizeof(DBO_GR_UserFriendInfo));
	if(wDataSize>sizeof(DBO_GR_UserFriendInfo)) return false;

	//��ȡ����
	DBO_GR_UserFriendInfo * pUserFriendInfo = (DBO_GR_UserFriendInfo *)pData;
	//ASSERT(pUserFriendInfo);

	//������Ϣ
	for(WORD wIndex=0;wIndex<pUserFriendInfo->wFriendCount;++wIndex)
	{
		//�����û�
		CServerUserItem * pServerUserItem=m_ServerUserManager.SearchUserItem(pUserFriendInfo->FriendInfo[wIndex].dwUserID);
		if(pServerUserItem==NULL)
		{
			//����״̬
			pUserFriendInfo->FriendInfo[wIndex].cbMainStatus=FRIEND_US_OFFLINE;
			pUserFriendInfo->FriendInfo[wIndex].cbGameStatus=US_NULL;
			pUserFriendInfo->FriendInfo[wIndex].wKindID=INVALID_KIND;
			pUserFriendInfo->FriendInfo[wIndex].wServerID=INVALID_SERVER;
			pUserFriendInfo->FriendInfo[wIndex].wTableID=INVALID_TABLE;
			pUserFriendInfo->FriendInfo[wIndex].wChairID=INVALID_CHAIR;
			ZeroMemory(pUserFriendInfo->FriendInfo[wIndex].szServerName,CountArray(pUserFriendInfo->FriendInfo[wIndex].szServerName));
			ZeroMemory(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode,CountArray(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode));
			continue;
		}

		CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
		tagFriendUserInfo * pUserInfo = pLocalUserItem->GetUserInfo();

		//���ñ���
		pUserFriendInfo->FriendInfo[wIndex].cbMainStatus=pServerUserItem->GetMainStatus();
		pUserFriendInfo->FriendInfo[wIndex].cbGameStatus=pServerUserItem->GetGameStatus();
		pUserFriendInfo->FriendInfo[wIndex].wKindID=pUserInfo->wKindID;
		pUserFriendInfo->FriendInfo[wIndex].wServerID=pUserInfo->wServerID;
		pUserFriendInfo->FriendInfo[wIndex].wTableID=pUserInfo->wTableID;
		pUserFriendInfo->FriendInfo[wIndex].wChairID=pUserInfo->wChairID;
		lstrcpyn(pUserFriendInfo->FriendInfo[wIndex].szServerName,pUserInfo->szServerName,CountArray(pUserFriendInfo->FriendInfo[wIndex].szServerName));
		lstrcpyn(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode,pUserInfo->szPhoneMode,CountArray(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode));
	}

	//��������
	WORD wFriendCount=pUserFriendInfo->wFriendCount;
	CMD_GC_UserFriendInfo IPCUserFriendInfo;


	//ѭ������
	while(wFriendCount>CountArray(IPCUserFriendInfo.FriendInfo))
	{
		//��������
		IPCUserFriendInfo.wFriendCount=CountArray(IPCUserFriendInfo.FriendInfo);
		CopyMemory(IPCUserFriendInfo.FriendInfo,&pUserFriendInfo->FriendInfo[pUserFriendInfo->wFriendCount-wFriendCount],sizeof(tagClientFriendInfo)*IPCUserFriendInfo.wFriendCount);

		//��������
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_FRIEND,&IPCUserFriendInfo,sizeof(IPCUserFriendInfo));

		//���ñ���
		wFriendCount -= CountArray(IPCUserFriendInfo.FriendInfo);
	}

	//ʣ�෢��
	if(wFriendCount>0)
	{
		//��������
		IPCUserFriendInfo.wFriendCount=wFriendCount;
		CopyMemory(IPCUserFriendInfo.FriendInfo,&pUserFriendInfo->FriendInfo[pUserFriendInfo->wFriendCount-wFriendCount],sizeof(tagClientFriendInfo)*wFriendCount);

		//��������
		WORD wPacketDataSize = sizeof(IPCUserFriendInfo)-sizeof(IPCUserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*wFriendCount;

		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_FRIEND,&IPCUserFriendInfo,wPacketDataSize);

	}

	if(pUserFriendInfo->bLogonFlag==true)
	{
		//��¼���
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_LOGON_FINISH);

		////��ʾ��Ϣ
		//CTraceService::TraceString(TEXT("��������ͬ����Ϣ"),TraceLevel_Normal);
	}

	//��������
	tagServerFriendInfo ServerFriendInfo;

	//�������
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pUserFriendInfo->dwUserID);
	if(pUserFriendGroup==NULL)
	{
		//�������
		pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(pUserFriendInfo->wFriendCount);
		m_FriendGroupManager.InsertFriendGroup(pUserFriendInfo->dwUserID,pUserFriendGroup);
	}

	if(pUserFriendGroup!=NULL)
	{
		//���÷���
		if(pUserFriendInfo->bLogonFlag==true)
		{
			pUserFriendGroup->ResetFriendGroup();
			pUserFriendGroup->SetOwnerUserID(pUserFriendInfo->dwUserID);
		}

		for(WORD wIndex=0;wIndex<pUserFriendInfo->wFriendCount;++wIndex)
		{
			ServerFriendInfo.dwUserID=pUserFriendInfo->FriendInfo[wIndex].dwUserID;
			ServerFriendInfo.cbMainStatus=pUserFriendInfo->FriendInfo[wIndex].cbMainStatus;
			ServerFriendInfo.cbGameStatus=pUserFriendInfo->FriendInfo[wIndex].cbGameStatus;
			ServerFriendInfo.cbGroupID=pUserFriendInfo->FriendInfo[wIndex].cbGroupID;
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);
		}		
	}

	return true;
}

//���ѱ�ע
bool CAttemperEngineSink::OnDBLoadUserRemarksInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize<=sizeof(DBO_GR_UserRemarksInfo));
	if(wDataSize>sizeof(DBO_GR_UserRemarksInfo)) return false;

	//��ȡ����
	DBO_GR_UserRemarksInfo * pDBOUserRemarksInfo = (DBO_GR_UserRemarksInfo *)pData;
	//ASSERT(pDBOUserRemarksInfo);

	//��������
	tagUserRemarksInfo * pUserRemarksInfo=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[sizeof(tagUserRemarksInfo)];
	WORD wDataIndex=0,wPacketSize=0;

	//ѭ������
	for(int nIndex=0;nIndex<pDBOUserRemarksInfo->wFriendCount;++nIndex)
	{
		//���ñ���
		pUserRemarksInfo = &pDBOUserRemarksInfo->RemarksInfo[nIndex];

		//��������		
		tagUserRemarksInfo * pSendUserRemarksInfo=(tagUserRemarksInfo *)&cbDataBuffer;

		//���ñ���
		pSendUserRemarksInfo->dwFriendUserID=pUserRemarksInfo->dwFriendUserID;
		lstrcpyn(pSendUserRemarksInfo->szRemarksInfo,pUserRemarksInfo->szRemarksInfo,CountArray(pSendUserRemarksInfo->szRemarksInfo));

		//���ñ���
		wPacketSize = sizeof(tagUserRemarksInfo)-sizeof(pSendUserRemarksInfo->szRemarksInfo)+CountStringBuffer(pSendUserRemarksInfo->szRemarksInfo);

		//�����ж�
		if(wDataIndex+wPacketSize+sizeof(BYTE)>CountArray(cbSendBuffer))
		{			
			//������Ϣ
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_REMARKS,cbSendBuffer,wDataIndex);

			//���ñ���
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//��������
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(BYTE));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(BYTE)],cbDataBuffer,wPacketSize);

		//���ñ���
		wDataIndex += sizeof(BYTE)+wPacketSize;
	}

	//ʣ�෢��
	if(wDataIndex>0)
	{
		//������Ϣ
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_REMARKS,cbSendBuffer,wDataIndex);			
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnDBLoadUserSimpleIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize<=sizeof(DBO_GR_UserIndividual));
	if(wDataSize>sizeof(DBO_GR_UserIndividual)) return false;

	//��ȡ����
	DBO_GR_UserIndividual * pDBOUserSimpleIndividual = (DBO_GR_UserIndividual *)pData;
	//ASSERT(pDBOUserSimpleIndividual);

	//��������
	tagUserIndividual * pUserSimpleIndividual=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	WORD wDataIndex=0,wPacketSize=0;

	//ѭ������
	for(int nIndex=0;nIndex<pDBOUserSimpleIndividual->wUserCount;++nIndex)
	{
		//���ñ���
		pUserSimpleIndividual = &pDBOUserSimpleIndividual->UserIndividual[nIndex];

		//��������		
		CMD_GC_UserIndividual * pUserIndividual=(CMD_GC_UserIndividual *)&cbDataBuffer;
		CSendPacketHelper SendPacket(pUserIndividual+1,sizeof(cbDataBuffer)-sizeof(CMD_GC_UserIndividual));

		//����ṹ
		pUserIndividual->dwUserID = pUserSimpleIndividual->dwUserID;

		//���ñ���
		wPacketSize = sizeof(CMD_GC_UserIndividual);

		//QQ����
		if (pUserSimpleIndividual->szQQ[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szQQ);
			SendPacket.AddPacket(pUserSimpleIndividual->szQQ,wBufferSize,DTP_GP_UI_QQ);
		}

		//Email
		if (pUserSimpleIndividual->szEMail[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szEMail);
			SendPacket.AddPacket(pUserSimpleIndividual->szEMail,wBufferSize,DTP_GP_UI_EMAIL);
		}

		//�̶��绰
		if (pUserSimpleIndividual->szSeatPhone[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szSeatPhone);
			SendPacket.AddPacket(pUserSimpleIndividual->szSeatPhone,wBufferSize,DTP_GP_UI_SEATPHOHE);
		}


		//�ֻ�����
		if (pUserSimpleIndividual->szMobilePhone[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szMobilePhone);
			SendPacket.AddPacket(pUserSimpleIndividual->szMobilePhone,wBufferSize,DTP_GP_UI_MOBILEPHONE);			
		}

		//��ϵ��ַ
		if (pUserSimpleIndividual->szDwellingPlace[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szDwellingPlace);
			SendPacket.AddPacket(pUserSimpleIndividual->szDwellingPlace,wBufferSize,DTP_GP_UI_DWELLINGPLACE);
		}

		//��������
		if (pUserSimpleIndividual->szPostalCode[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szPostalCode);
			SendPacket.AddPacket(pUserSimpleIndividual->szPostalCode,wBufferSize,DTP_GP_UI_POSTALCODE);
		}

		//���ñ���
		wPacketSize += SendPacket.GetDataSize();

		//�����ж�
		if(wDataIndex+wPacketSize+sizeof(WORD)>CountArray(cbSendBuffer))
		{			
			//������Ϣ
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_SINDIVIDUAL,cbSendBuffer,wDataIndex);

			//���ñ���
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//��������
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(WORD));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(WORD)],cbDataBuffer,wPacketSize);

		//���ñ���
		wDataIndex += sizeof(WORD)+wPacketSize;
	}

	//ʣ�෢��
	if(wDataIndex>0)
	{
		//������Ϣ
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_SINDIVIDUAL,cbSendBuffer,wDataIndex);			
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBLoadUserOfflineMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize<=sizeof(DBO_GR_UserOfflineMessage));
	if(wDataSize>sizeof(DBO_GR_UserOfflineMessage)) return false;

	//��ȡ����
	DBO_GR_UserOfflineMessage * pUserUserOfflineMessage = (DBO_GR_UserOfflineMessage *)pData;
	//ASSERT(pUserUserOfflineMessage);

	//��������
	BYTE cbOfflineData[CountArray(pUserUserOfflineMessage->szOfflineData)];
	ZeroMemory(cbOfflineData,sizeof(cbOfflineData));

	//��������	
	if (pUserUserOfflineMessage->szOfflineData[0]!=0)
	{
		//��ȡ����
		INT nCustomRuleSize=lstrlen(pUserUserOfflineMessage->szOfflineData)/2;

		//ת���ַ�
		for (INT i=0;i<nCustomRuleSize;i++)
		{
			//��ȡ�ַ�
			TCHAR cbChar1=pUserUserOfflineMessage->szOfflineData[i*2];
			TCHAR cbChar2=pUserUserOfflineMessage->szOfflineData[i*2+1];

			//Ч���ַ�
			//ASSERT((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))||(cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F')));
			//ASSERT((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))||(cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F')));

			//���ɽ��
			if ((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))) cbOfflineData[i]+=(cbChar2-TEXT('0'));
			if ((cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F'))) cbOfflineData[i]+=(cbChar2-TEXT('A')+0x0A);

			//���ɽ��
			if ((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))) cbOfflineData[i]+=(cbChar1-TEXT('0'))*0x10;
			if ((cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F'))) cbOfflineData[i]+=(cbChar1-TEXT('A')+0x0A)*0x10;
		}
	}

	//������Ϣ
	SendData(dwContextID,MDM_GC_USER,pUserUserOfflineMessage->wMessageType,&cbOfflineData,pUserUserOfflineMessage->wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBLoadDeleteFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize<=sizeof(DBO_GR_DeleteFriend));
	if(wDataSize>sizeof(DBO_GR_DeleteFriend)) return false;

	//��ȡ����
	DBO_GR_DeleteFriend * pDeleteFriend = (DBO_GR_DeleteFriend *)pData;
	//ASSERT(pDeleteFriend);

	//��ȡ�û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwUserID);
	if(pServerUserItem!=NULL)
	{
		CUserFriendGroup * pFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwUserID);
		if(pFriendGroup!=NULL) 
		{
			//�Ƴ�����
			pFriendGroup->RemoveFriendInfo(pDeleteFriend->dwFriendUserID);
			pFriendGroup->RemoveFriendInfo(pDeleteFriend->dwUserID);

		}

		//��ȡ����
		CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwUserID);
		if(pUserFriendGroup==NULL) 
		{
			pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
		}

		if(pUserFriendGroup!=NULL)
		{
			//��ȡ���ѷ���
			CUserFriendGroup * pDestFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwFriendUserID);
			if(pDestFriendGroup!=NULL) 
			{
				//�Ƴ�����
				pUserFriendGroup->RemoveFriendInfo(pDeleteFriend->dwFriendUserID);
				pDestFriendGroup->RemoveFriendInfo(pDeleteFriend->dwUserID);

			}



			CServerUserItem * pFriendUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwFriendUserID);
			if(pFriendUserItem!=NULL&&pFriendUserItem->GetMainStatus()==FRIEND_US_ONLINE)
			{
				//����ṹ
				CMD_GC_DeleteFriendNotify DeleteFriendNotify;
				DeleteFriendNotify.dwFriendUserID=pDeleteFriend->dwUserID;

				//��������
				SendData(((CLocalUserItem *)pFriendUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_DELETE_FRIEND_NOTIFY,&DeleteFriendNotify,sizeof(DeleteFriendNotify));
			}
					
		}
	}

		//����ṹ
		CMD_GC_DeleteFriendNotify DeleteFriendNotify;
		DeleteFriendNotify.dwFriendUserID=pDeleteFriend->dwFriendUserID;

		//��������
		SendData(dwContextID,MDM_GC_USER,SUB_GC_DELETE_FRIEND_NOTIFY,&DeleteFriendNotify,sizeof(DeleteFriendNotify));

		return true;

}

//�����û�
bool CAttemperEngineSink::OnDBSearchUserResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{	
	//����У��
	//ASSERT(wDataSize<=sizeof(DBO_GR_SearchUserResult));
	if(wDataSize>sizeof(DBO_GR_SearchUserResult)) return false;

	//��ȡ����
	DBO_GR_SearchUserResult * pSearchUserResult = (DBO_GR_SearchUserResult *)pData;
	//ASSERT(pSearchUserResult);

	//�������
	CServerUserItem * pServerUserItem=m_ServerUserManager.SearchUserItem(pSearchUserResult->FriendInfo.dwUserID);
	if(pServerUserItem==NULL)
	{
		pSearchUserResult->FriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
		pSearchUserResult->FriendInfo.cbGameStatus=US_NULL;
		pSearchUserResult->FriendInfo.wKindID=INVALID_KIND;
		pSearchUserResult->FriendInfo.wServerID=INVALID_SERVER;
		pSearchUserResult->FriendInfo.wTableID=INVALID_TABLE;
		pSearchUserResult->FriendInfo.wChairID=INVALID_CHAIR;
		ZeroMemory(pSearchUserResult->FriendInfo.szServerName,CountArray(pSearchUserResult->FriendInfo.szServerName));
		ZeroMemory(pSearchUserResult->FriendInfo.szPhoneMode,CountArray(pSearchUserResult->FriendInfo.szPhoneMode));
	}
	else
	{
		CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
		tagFriendUserInfo * pUserInfo = pLocalUserItem->GetUserInfo();
		pSearchUserResult->FriendInfo.cbMainStatus=pServerUserItem->GetMainStatus();
		pSearchUserResult->FriendInfo.cbGameStatus=pServerUserItem->GetGameStatus();
		pSearchUserResult->FriendInfo.wKindID=pUserInfo->wKindID;
		pSearchUserResult->FriendInfo.wServerID=pUserInfo->wServerID;
		pSearchUserResult->FriendInfo.wTableID=pUserInfo->wTableID;
		pSearchUserResult->FriendInfo.wChairID=pUserInfo->wChairID;
		lstrcpyn(pSearchUserResult->FriendInfo.szServerName,pUserInfo->szServerName,CountArray(pSearchUserResult->FriendInfo.szServerName));
		lstrcpyn(pSearchUserResult->FriendInfo.szPhoneMode,pUserInfo->szPhoneMode,CountArray(pSearchUserResult->FriendInfo.szPhoneMode));
	}

	//��������	
	CMD_GC_SearchByGameIDResult SearchUserResult;
	ZeroMemory(&SearchUserResult,sizeof(SearchUserResult));
	SearchUserResult.cbUserCount =pSearchUserResult->cbUserCount;
	BYTE cbUserCount=pSearchUserResult->cbUserCount;
	if(cbUserCount>0)
	{
		CopyMemory(&(SearchUserResult.FriendInfo),&(pSearchUserResult->FriendInfo),sizeof(tagClientFriendInfo)*(pSearchUserResult->cbUserCount));

		SendData(dwContextID,MDM_GC_USER,SUB_GC_SEARCH_USER_RESULT,&SearchUserResult,sizeof(SearchUserResult));
	}
	else
	{
		//��Ӧ����
		CMD_GC_SearchByGameIDEcho SearchUserEcho;
		ZeroMemory(&SearchUserEcho,sizeof(SearchUserEcho));
		SearchUserEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(SearchUserEcho.szDescribeString,_T("��ѯ�����û�"),CountArray(SearchUserEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(SearchUserEcho.szDescribeString);
		WORD wHeadSize=sizeof(SearchUserEcho)-sizeof(SearchUserEcho.szDescribeString);
		SendData(dwContextID,MDM_GC_USER,SUB_GC_SEARCH_USER_ECHO,&SearchUserEcho,wHeadSize+wDataSize);
	}
	return  true;
}

//��Ӻ���
bool CAttemperEngineSink::OnDBUserAddFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize==sizeof(DBO_GR_AddFriend));
	if(wDataSize!=sizeof(DBO_GR_AddFriend)) return false;

	//��ȡ����
	DBO_GR_AddFriend * pAddFriend = (DBO_GR_AddFriend *)pData;
	//ASSERT(pAddFriend);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pAddFriend->dwUserID);
	CServerUserItem * pRequestUserItem = m_ServerUserManager.SearchUserItem(pAddFriend->dwRequestUserID);
	if(pServerUserItem==NULL || pServerUserItem->GetUserItemKind()!=enLocalKind) return false;

	//�����û�
	if(pServerUserItem!=NULL)
	{
		if(pAddFriend->bLoadUserInfo==false && pRequestUserItem && pRequestUserItem->GetUserItemKind()==enLocalKind)
		{
			//��������
			CLocalUserItem * pRequestLocalInfo = (CLocalUserItem *)pRequestUserItem;

			//��ȡ����
			CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pAddFriend->dwUserID);
			if(pUserFriendGroup==NULL) 
			{
				pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
			}

			//��������
			tagServerFriendInfo ServerFriendInfo; 
			ZeroMemory(&ServerFriendInfo,sizeof(ServerFriendInfo));

			//���ñ���
			ServerFriendInfo.dwUserID=pAddFriend->dwRequestUserID;
			ServerFriendInfo.cbGroupID=pAddFriend->cbGroupID;
			ServerFriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
			ServerFriendInfo.cbGameStatus=US_OFFLINE;

			//��ȡ�û�
			if(pRequestUserItem!=NULL)
			{
				ServerFriendInfo.cbMainStatus=pRequestUserItem->GetMainStatus();
				ServerFriendInfo.cbGameStatus=pRequestUserItem->GetGameStatus();
			}		

			//��Ӻ���
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);

			//��������
			CMD_GC_UserFriendInfo UserFriendInfo;

			ZeroMemory(&UserFriendInfo.FriendInfo,sizeof(tagClientFriendInfo)*MAX_FRIEND_TRANSFER);

			//����ṹ
			UserFriendInfo.wFriendCount = 1;

			//������Ϣ
			UserFriendInfo.FriendInfo[0].dwUserID=pRequestLocalInfo->GetUserID();
			UserFriendInfo.FriendInfo[0].dwGameID= pRequestLocalInfo->GetGameID();
			UserFriendInfo.FriendInfo[0].cbGroupID =pAddFriend->cbGroupID;
			UserFriendInfo.FriendInfo[0].dwFaceID =pRequestLocalInfo->GetUserInfo()->dwFaceID;
			UserFriendInfo.FriendInfo[0].cbGender =pRequestLocalInfo->GetUserInfo()->cbGender;
			UserFriendInfo.FriendInfo[0].dwCustomID =pRequestLocalInfo->GetUserInfo()->dwCustomID;
			UserFriendInfo.FriendInfo[0].wMemberOrder =pRequestLocalInfo->GetUserInfo()->wMemberOrder;
			UserFriendInfo.FriendInfo[0].wGrowLevel =pRequestLocalInfo->GetUserInfo()->wGrowLevel;
			lstrcpyn(UserFriendInfo.FriendInfo[0].szNickName,pRequestLocalInfo->GetNickName(),CountArray(UserFriendInfo.FriendInfo[0].szNickName));
			lstrcpyn(UserFriendInfo.FriendInfo[0].szUnderWrite,pRequestLocalInfo->GetUserInfo()->szUnderWrite,CountArray(UserFriendInfo.FriendInfo[0].szUnderWrite));
			lstrcpyn(UserFriendInfo.FriendInfo[0].szCompellation,pRequestLocalInfo->GetUserInfo()->szCompellation,CountArray(UserFriendInfo.FriendInfo[0].szCompellation));
			lstrcpyn(UserFriendInfo.FriendInfo[0].szPhoneMode,pRequestLocalInfo->GetUserInfo()->szPhoneMode,CountArray(UserFriendInfo.FriendInfo[0].szPhoneMode));

			//������Ϣ
			UserFriendInfo.FriendInfo[0].cbMainStatus=pRequestLocalInfo->GetMainStatus();
			UserFriendInfo.FriendInfo[0].cbGameStatus=pRequestLocalInfo->GetGameStatus();
			UserFriendInfo.FriendInfo[0].wServerID=pRequestLocalInfo->GetServerID();
			UserFriendInfo.FriendInfo[0].wTableID = pRequestLocalInfo->GetTableID();


			//��������
			WORD wSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
			SendData(((CLocalUserItem *)pServerUserItem)->GetSocketID(),MDM_GC_LOGON,SUB_S_USER_FRIEND,&UserFriendInfo,wSize);

		}		
	}

	//�����û�
	if(pRequestUserItem!=NULL)
	{
		if(pRequestUserItem->GetUserItemKind()==enLocalKind)
		{
			//��������
			CLocalUserItem * pServerLocalInfo = (CLocalUserItem *)pServerUserItem;

			//��ȡ����
			CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pAddFriend->dwRequestUserID);
			if(pUserFriendGroup==NULL) 
			{
				pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
			}

			//��������
			tagServerFriendInfo ServerFriendInfo; 
			ZeroMemory(&ServerFriendInfo,sizeof(ServerFriendInfo));

			//���ñ���
			ServerFriendInfo.dwUserID=pAddFriend->dwUserID;
			ServerFriendInfo.cbGroupID=pAddFriend->cbRequestGroupID;
			ServerFriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
			ServerFriendInfo.cbGameStatus=US_OFFLINE;

			//��ȡ�û�
			if(pServerUserItem!=NULL)
			{
				ServerFriendInfo.cbMainStatus=pServerUserItem->GetMainStatus();
				ServerFriendInfo.cbGameStatus=pServerUserItem->GetGameStatus();
			}		

			//��Ӻ���
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);


			if(pRequestUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)
			{
				//��������
				CMD_GC_UserFriendInfo UserFriendInfo;

				ZeroMemory(&UserFriendInfo.FriendInfo,sizeof(tagClientFriendInfo)*MAX_FRIEND_TRANSFER);

				//����ṹ
				UserFriendInfo.wFriendCount = 1;

				//������Ϣ
				UserFriendInfo.FriendInfo[0].dwUserID=pServerLocalInfo->GetUserID();
				UserFriendInfo.FriendInfo[0].dwGameID= pServerLocalInfo->GetGameID();
				UserFriendInfo.FriendInfo[0].cbGroupID =pAddFriend->cbRequestGroupID;
				UserFriendInfo.FriendInfo[0].dwFaceID =pServerLocalInfo->GetUserInfo()->dwFaceID;
				UserFriendInfo.FriendInfo[0].dwCustomID =pServerLocalInfo->GetUserInfo()->dwCustomID;
				UserFriendInfo.FriendInfo[0].cbGender =pServerLocalInfo->GetUserInfo()->cbGender;
				UserFriendInfo.FriendInfo[0].wMemberOrder =pServerLocalInfo->GetUserInfo()->wMemberOrder;
				UserFriendInfo.FriendInfo[0].wGrowLevel =pServerLocalInfo->GetUserInfo()->wGrowLevel;
				lstrcpyn(UserFriendInfo.FriendInfo[0].szNickName,pServerLocalInfo->GetNickName(),CountArray(UserFriendInfo.FriendInfo[0].szNickName));
				lstrcpyn(UserFriendInfo.FriendInfo[0].szUnderWrite,pServerLocalInfo->GetUserInfo()->szUnderWrite,CountArray(UserFriendInfo.FriendInfo[0].szUnderWrite));
				lstrcpyn(UserFriendInfo.FriendInfo[0].szCompellation,pServerLocalInfo->GetUserInfo()->szCompellation,CountArray(UserFriendInfo.FriendInfo[0].szCompellation));
				lstrcpyn(UserFriendInfo.FriendInfo[0].szPhoneMode,pServerLocalInfo->GetUserInfo()->szPhoneMode,CountArray(UserFriendInfo.FriendInfo[0].szPhoneMode));

				//������Ϣ
				UserFriendInfo.FriendInfo[0].cbMainStatus=pServerLocalInfo->GetMainStatus();
				UserFriendInfo.FriendInfo[0].cbGameStatus=pServerLocalInfo->GetGameStatus();
				UserFriendInfo.FriendInfo[0].wServerID=pServerLocalInfo->GetServerID();
				UserFriendInfo.FriendInfo[0].wTableID = pServerLocalInfo->GetTableID();

				//��������
				WORD wSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
				SendData(((CLocalUserItem *)pRequestUserItem)->GetSocketID(),MDM_GC_LOGON,SUB_S_USER_FRIEND,&UserFriendInfo,wSize);

			}	

			//��Ϣ����
			CMD_GC_RespondNotify RespondNotify;
			_sntprintf(RespondNotify.szNotifyContent,CountArray(RespondNotify.szNotifyContent),TEXT("%s ͬ�������ĺ������룡"),pServerLocalInfo->GetNickName());

			if(pRequestUserItem->GetMainStatus()==FRIEND_US_OFFLINE)
			{
				SaveOfflineMessage(pRequestUserItem->GetUserID(),SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify),dwContextID);
			}
			else
			{
				//��������
				SendData(((CLocalUserItem *)pRequestUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify));
			}
		}		
	}

	return true;
}

bool CAttemperEngineSink::OnDBTrumpetResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize==sizeof(DBO_GR_TrumpetResult));
	if(wDataSize!=sizeof(DBO_GR_TrumpetResult)) return false;

	//��ȡ����
	DBO_GR_TrumpetResult * pTrumpetResult = (DBO_GR_TrumpetResult *)pData;
	//ASSERT(pTrumpetResult);



	if (pTrumpetResult->lResult == 0)
	{
		//���ȷ���
		CMD_GC_TrumpetEcho TrumpetEcho;
		ZeroMemory(&TrumpetEcho,sizeof(TrumpetEcho));
		TrumpetEcho.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(TrumpetEcho.szDescribeString,pTrumpetResult->szNotifyContent,CountArray(TrumpetEcho.szDescribeString));
		WORD wDataSize2=CountStringBuffer(TrumpetEcho.szDescribeString);
		WORD wHeadSize=sizeof(TrumpetEcho)-sizeof(TrumpetEcho.szDescribeString);
		SendData(dwContextID,MDM_GC_USER,SUB_GC_TRUMPET_ECHO,&TrumpetEcho,wHeadSize+wDataSize2);

		CMD_GC_Trumpet Trumpet;
		ZeroMemory(&Trumpet, sizeof(Trumpet));
		Trumpet.wPropertyID = pTrumpetResult->wPropertyID;
		Trumpet.dwSendUserID = pTrumpetResult->dwSendUserID;
		Trumpet.TrumpetColor = pTrumpetResult->TrumpetColor;
		lstrcpyn(Trumpet.szSendNickName, pTrumpetResult->szSendNickName, CountArray(Trumpet.szSendNickName));
		lstrcpyn(Trumpet.szTrumpetContent, pTrumpetResult->szTrumpetContent, CountArray(Trumpet.szTrumpetContent));

		m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_PROPERTY_TRUMPET,&Trumpet,sizeof(Trumpet));
		
	}
	else
	{
		//���ȷ���
		CMD_GC_TrumpetEcho TrumpetEcho;
		ZeroMemory(&TrumpetEcho,sizeof(TrumpetEcho));
		TrumpetEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(TrumpetEcho.szDescribeString,pTrumpetResult->szNotifyContent,CountArray(TrumpetEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(TrumpetEcho.szDescribeString);
		WORD wHeadSize=sizeof(TrumpetEcho)-sizeof(TrumpetEcho.szDescribeString);
		SendData(dwContextID,MDM_GC_USER,SUB_GC_TRUMPET_ECHO,&TrumpetEcho,wHeadSize+wDataSize);
	}



	return true;
}

//�������
bool CAttemperEngineSink::OnDBUserOperateResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize<=sizeof(DBO_GR_OperateResult));
	if(wDataSize>sizeof(DBO_GR_OperateResult)) return false;

	//��ȡ����
	DBO_GR_OperateResult * pUserOperateResult = (DBO_GR_OperateResult *)pData;
	//ASSERT(pUserOperateResult);

	//��ȡ�û�
	if(pUserOperateResult->bModifySucesss==true)
	{
		//���ͳɹ�
		SendOperateSuccess(pUserOperateResult->szDescribeString,pUserOperateResult->wOperateCode,dwContextID);
	}
	else
	{
		//����ʧ��
		SendOperateFailure(pUserOperateResult->szDescribeString,pUserOperateResult->dwErrorCode,pUserOperateResult->wOperateCode,dwContextID);
	}

	return true;
}

//�Ƴ���Ϣ
void CAttemperEngineSink::RemoveSystemMessage()
{
	//������Ϣ
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	m_SystemMessageActive.RemoveAll();
}
//ϵͳ��Ϣ
bool CAttemperEngineSink::OnDBSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(tag_GR_SystemMessage));
	if (wDataSize != sizeof(tag_GR_SystemMessage)) return false;

	//��ȡ����
	tag_GR_SystemMessage * pSystemMessage = (tag_GR_SystemMessage *)pData;
	if (pSystemMessage == NULL) return false;

	for (INT_PTR nIndex = m_SystemMessageBuffer.GetCount() - 1; nIndex >= 0; nIndex--)
	{
		tagSystemMessage *pTagSystemMessage = m_SystemMessageBuffer[nIndex];
		if (pTagSystemMessage && pTagSystemMessage->SystemMessage.dwMessageID == pSystemMessage->dwMessageID)
		{
			CopyMemory(&pTagSystemMessage->SystemMessage, pSystemMessage, sizeof(tag_GR_SystemMessage));
			m_SystemMessageActive.Add(pTagSystemMessage);
			m_SystemMessageBuffer.RemoveAt(nIndex);

			return true;
		}
	}

	//�������
	tagSystemMessage  *pSendMessage = new tagSystemMessage;
	ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

	//���ñ���
	pSendMessage->dwLastTime = (DWORD)pSystemMessage->tStartTime;
	CopyMemory(&pSendMessage->SystemMessage, pSystemMessage, sizeof(tag_GR_SystemMessage));

	//��¼��Ϣ
	m_SystemMessageActive.Add(pSendMessage);

	return true;
}

//�������
bool CAttemperEngineSink::OnDBSystemMessageFinish(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//������ʱ��Ϣ
	if (m_SystemMessageBuffer.GetCount()>0)
	{
		//��������
		tagSystemMessage *pTagSystemMessage = NULL;

		for (INT_PTR nIndex = m_SystemMessageBuffer.GetCount() - 1; nIndex >= 0; nIndex--)
		{
			pTagSystemMessage = m_SystemMessageBuffer[nIndex];
			if (pTagSystemMessage && pTagSystemMessage->SystemMessage.dwMessageID == TEMP_MESSAGE_ID)
			{
				m_SystemMessageActive.Add(pTagSystemMessage);
				m_SystemMessageBuffer.RemoveAt(nIndex);
			}
		}
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnDBPCUserMatchSignupResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	//	//ASSERT(LOWORD(dwContextID) < m_pInitParameter->m_wMaxConnect);
	//	if ((m_pBindParameter + LOWORD(dwContextID))->dwSocketID != dwContextID) return true;

	//��������
	DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult *)pData;

	//��������
	CMD_GC_MatchSignupResult MatchSignupResult;
	ZeroMemory(&MatchSignupResult, sizeof(MatchSignupResult));

	//����ṹ	
	MatchSignupResult.bSuccessed = pMatchSignupResult->bSuccessed;	
	MatchSignupResult.wServerID = pMatchSignupResult->wServerID;
	MatchSignupResult.lCurrScore = pMatchSignupResult->lCurrScore;
	lstrcpyn(MatchSignupResult.szDescribeString, pMatchSignupResult->szDescribeString, CountArray(MatchSignupResult.szDescribeString));

	//��������
	WORD wSendSize = sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString) + CountStringBuffer(MatchSignupResult.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GC_MATCH, SUB_GC_MATCH_SIGNUP_RESULT, &MatchSignupResult, sizeof(MatchSignupResult));

	return true;
}

//�ҵı������
bool CAttemperEngineSink::OnDBUserMatchHistoryResult_T(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������	
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GC_MATCH, SUB_GC_MY_MATCH_HISTORY_RESULT_T, pData, wDataSize);

	return true;
}

//�ҵı������
bool CAttemperEngineSink::OnDBUserMyMatchHistoryResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	//	//ASSERT(LOWORD(dwContextID) < m_pInitParameter->m_wMaxConnect);
	//	if ((m_pBindParameter + LOWORD(dwContextID))->dwSocketID != dwContextID) return true;

	//��������
	DBO_GR_MyMatchHistoryResult * pMyMatchResult = (DBO_GR_MyMatchHistoryResult *)pData;

	//��������
	CMD_GC_MyMatchHistoryResult MyMatchResult;
	ZeroMemory(&MyMatchResult, sizeof(MyMatchResult));

	//����ṹ
	MyMatchResult.bResultCode = pMyMatchResult->bResultCode;
	lstrcpyn(MyMatchResult.szMatchName, pMyMatchResult->szMatchName, CountArray(MyMatchResult.szMatchName));
	MyMatchResult.wKindID = pMyMatchResult->wKindID;
	MyMatchResult.wRandID = pMyMatchResult->wRandID;
	lstrcpyn(MyMatchResult.szDescribeString, pMyMatchResult->szDescribeString, CountArray(MyMatchResult.szDescribeString));

	//��������	
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GC_MATCH, SUB_GC_MY_MATCH_HISTORY_RESULT_F, &MyMatchResult, sizeof(MyMatchResult));

	return true;
}




//����Լս�����б�
bool CAttemperEngineSink::OnTCPNetworkSubPersonalGoldRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{

	//Ч�����
	//ASSERT(wDataSize == sizeof(CMD_SC_C_PersonalGoldRoomList));
	if (wDataSize != sizeof(CMD_SC_C_PersonalGoldRoomList)) return false;

	CMD_SC_C_PersonalGoldRoomList * pPersonalGoldRoomList = (CMD_SC_C_PersonalGoldRoomList *)pData;
	if (pPersonalGoldRoomList == NULL)
	{
		return false;
	}

	//��������
	CMD_SC_C_PersonalGoldRoomListResult PersonalGoldRoomListResult;
	
	BYTE cbDataBuffer[SOCKET_TCP_PACKET] = { 0 };
	ZeroMemory(cbDataBuffer, sizeof(cbDataBuffer));
	CMD_SC_C_PersonalGoldRoomListResult * pPersonalGoldRoomListResult = (CMD_SC_C_PersonalGoldRoomListResult*)cbDataBuffer;

	int cbPacketSize = 0;

	//��ȡͬ��������Ϸ��������
	int nCount = m_PersonalRoomManager.GetPersonalTableCount(pPersonalGoldRoomList->dwKindID);

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdt *** OnTCPNetworkSubPersonalGoldRoomList ��������nCount = %d kindid = %d  cbPacketSize = %d"), nCount, pPersonalGoldRoomList->dwKindID, cbPacketSize );
	//OutputDebugString(szInfo);

	//�������з���
	for (int i = 0; i < nCount; i++)
	{
		//�����ж�				
		if (cbPacketSize + sizeof(CMD_SC_C_PersonalGoldRoomListResult) > sizeof(cbDataBuffer))
		{
	
			//��������	
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_GET_GOLD_LIST_RESLUT, pPersonalGoldRoomListResult, cbPacketSize, 0xFF);
			//���ñ���										
			cbPacketSize = 0;
			ZeroMemory(cbDataBuffer, sizeof(cbDataBuffer));
			pPersonalGoldRoomListResult = (CMD_SC_C_PersonalGoldRoomListResult*)cbDataBuffer;

		}

		tagPersonalTableInfoEx * pPersonalTableInfoEx = m_PersonalRoomManager.EnumPersonalTableItem(pPersonalGoldRoomList->dwKindID, 1, i);
		if (!pPersonalTableInfoEx)
		{
			continue;
		}

		PersonalGoldRoomListResult.dwServerID = pPersonalTableInfoEx->dwServerID;						
		PersonalGoldRoomListResult.dwKindID = pPersonalTableInfoEx->dwKindID;							
		PersonalGoldRoomListResult.dwTableID = pPersonalTableInfoEx->dwTableID;							
		PersonalGoldRoomListResult.dwUserID = pPersonalTableInfoEx->dwUserID;							
		PersonalGoldRoomListResult.lCellScore = pPersonalTableInfoEx->lCellScore;							
		PersonalGoldRoomListResult.dwPersonalRoomID = pPersonalTableInfoEx->dwPersonalRoomID;					
		PersonalGoldRoomListResult.dwRoomTax = pPersonalTableInfoEx->dwRoomTax;							
		PersonalGoldRoomListResult.cbPlayMode = pPersonalTableInfoEx->cbPlayMode;	
		PersonalGoldRoomListResult.wJoinGamePeopleCount = pPersonalTableInfoEx->wJoinGamePeopleCount;
		PersonalGoldRoomListResult.lEnterScoreLimit = pPersonalTableInfoEx->lEnterScoreLimit;					
		PersonalGoldRoomListResult.lLeaveScoreLimit = pPersonalTableInfoEx->lLeaveScoreLimit;					
		PersonalGoldRoomListResult.bGameStart = pPersonalTableInfoEx->bGameStart;							
		memcpy_s(PersonalGoldRoomListResult.cbGameRule, sizeof(PersonalGoldRoomListResult.cbGameRule), pPersonalTableInfoEx->cbGameRule, sizeof(pPersonalTableInfoEx->cbGameRule));
		memcpy_s(PersonalGoldRoomListResult.szNickName, sizeof(PersonalGoldRoomListResult.szNickName), pPersonalTableInfoEx->szNickName, sizeof(pPersonalTableInfoEx->szNickName));
		memcpy_s(PersonalGoldRoomListResult.dwJoinUserID, sizeof(PersonalGoldRoomListResult.dwJoinUserID), pPersonalTableInfoEx->dwJoinUserID, sizeof(pPersonalTableInfoEx->dwJoinUserID));
		memcpy_s(PersonalGoldRoomListResult.dwFaceID, sizeof(PersonalGoldRoomListResult.dwFaceID), pPersonalTableInfoEx->dwFaceID, sizeof(pPersonalTableInfoEx->dwFaceID));
		memcpy_s(PersonalGoldRoomListResult.dwCustomID, sizeof(PersonalGoldRoomListResult.dwCustomID), pPersonalTableInfoEx->dwCustomID, sizeof(pPersonalTableInfoEx->dwCustomID));
		memcpy_s(PersonalGoldRoomListResult.szJoinUserNickName, sizeof(PersonalGoldRoomListResult.szJoinUserNickName), pPersonalTableInfoEx->szJoinUserNickName, sizeof(pPersonalTableInfoEx->szJoinUserNickName));

		memcpy(&cbDataBuffer[cbPacketSize], &PersonalGoldRoomListResult, sizeof(CMD_SC_C_PersonalGoldRoomListResult));

		cbPacketSize += sizeof(CMD_SC_C_PersonalGoldRoomListResult);
	
	}


	if (cbPacketSize > 0)
	{
		wsprintf(szInfo, TEXT("ptdt *** OnTCPNetworkSubPersonalGoldRoomList ��������nCount = %d kindid = %d  cbPacketSize = %d"), nCount, pPersonalGoldRoomList->dwKindID, cbPacketSize);
		//OutputDebugString(szInfo);

		//��������	
		if (!m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_PERSONAL, CMD_CS_C_GET_GOLD_LIST_RESLUT, pPersonalGoldRoomListResult, cbPacketSize))
		{
			wsprintf(szInfo, TEXT("ptdt *** OnTCPNetworkSubPersonalGoldRoomList ��������nCount = %d kindid = %d  cbPacketSize = %d  ��������ʧ��"), nCount, pPersonalGoldRoomList->dwKindID, cbPacketSize);
			//OutputDebugString(szInfo);
		}
	}

	wsprintf(szInfo, TEXT("ptdt *** OnTCPNetworkSubPersonalGoldRoomList ��������nCount = %d kindid = %d  cbPacketSize = %d  �����б������Ϣ"), nCount, pPersonalGoldRoomList->dwKindID, cbPacketSize);
	//OutputDebugString(szInfo);
	//��������	
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_PERSONAL, CMD_CS_C_GET_GOLD_LIST_RESLUT_END, NULL, 0);
	

	return true;
}

//������־
void CAttemperEngineSink::SendLogData(TCHAR * szLogContent)
{
	tagLogUserInfo LogUserInfo;
	ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	wsprintf(LogUserInfo.szLogContent, TEXT("%s"), szLogContent);
	wsprintf(LogUserInfo.szServerName, TEXT("%s"), TEXT("���������"));
	LogUserInfo.wKindID = 0;
	LogUserInfo.wServerID = 0;
	LogUserInfo.cbServerSign = 4;
	GetLocalTime(&LogUserInfo.sysTime);
	m_pLogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &LogUserInfo, sizeof(LogUserInfo));
}
//////////////////////////////////////////////////////////////////////////////////
