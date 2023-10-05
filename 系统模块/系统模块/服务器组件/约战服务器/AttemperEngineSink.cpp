#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"


//////////////////////////////////////////////////////////////////////////////////
//��ʱ������

#define IDI_LOAD_PLATFORM_PARAMETER		1							//���ز���
#define TIME_LOAD_PLATFORM_PARAMETER	600000						//ʱ����

/*
#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../���ӿ�/Unicode/PersonalData.lib")
#else
#pragma comment (lib,"../../���ӿ�/Unicode/PersonalData.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../���ӿ�/Unicode/PersonalDataD.lib")
#else
#pragma comment (lib,"../../���ӿ�/Unicode/PersonalDataD.lib")

#endif
#endif
*/
//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_wCollectItem=INVALID_WORD;

	//���ñ���
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	m_wClubBindIndex = INVALID_WORD;
	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
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
	//�󶨲���
	m_pBindParameter=new tagBindParameter[m_pInitParameter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxConnect);

	//���ö�ʱ��
#ifndef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_PLATFORM_PARAMETER,TIME_LOAD_PLATFORM_PARAMETER,TIMES_INFINITY,0);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_PLATFORM_PARAMETER,60000,TIMES_INFINITY,0);
#endif



	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_wCollectItem=INVALID_WORD;
	m_WaitCollectItemArray.RemoveAll();

	//���ñ���
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//�������
	m_GlobalInfoManager.ResetData();

	m_wClubBindIndex = INVALID_WORD;
	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
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
	//���ز���
	if(IDI_LOAD_PLATFORM_PARAMETER==dwTimerID)
	{
		//����֪ͨ
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_PLATFORM_PARAMETER,0,0,0L);

		return true;
	}

	return false;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��ȡ����
	//ASSERT(LOWORD(dwSocketID)<m_pInitParameter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParameter->m_wMaxConnect) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	TCHAR szInfo[260] = { 0 };

	wsprintf(szInfo, TEXT("ptdtai +++++++++++++++++ Լս�������յ���Ϣ wMainCmdID = %d wSubCmdID = %d"), Command.wMainCmdID, Command.wSubCmdID);
	//OutputDebugString(szInfo);

	bool bReturnValue = false;
	switch (Command.wMainCmdID)
	{
	case MDM_CS_REGISTER:		//����ע��
		{
			bReturnValue = OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			break;
		}
	case MDM_CS_SERVICE_INFO:	//������Ϣ
		{
			bReturnValue = OnTCPNetworkMainServiceInfo(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			break;
		}
	case MDM_CS_CLUB_SERVICE_INFO:	//�����˷�����Ϣ
		{
			bReturnValue = OnTCPNetworkMainClubService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			break;
		}
	}

	if (!bReturnValue)
	{
		wsprintf(szInfo, TEXT("ptdtai +++++++++++++++++ wMainCmdID = %d wSubCmdID = %d ����false"), Command.wMainCmdID, Command.wSubCmdID);
		//OutputDebugString(szInfo);
	}

	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//��Ϸ����
	if (pBindParameter->ServiceKind==ServiceKind_Game)
	{
		//��������
		WORD wBindIndex=LOWORD(dwSocketID);

		//���ܴ���
		if (wBindIndex==m_wCollectItem)
		{
			//���ñ���
			m_wCollectItem=INVALID_WORD;

			//�����л�
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//��ȡ����
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//ɾ������
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//������Ϣ
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
		}
		else
		{
			//ɾ���ȴ�
			for (INT_PTR i=0;i<m_WaitCollectItemArray.GetCount();i++)
			{
				if (wBindIndex==m_WaitCollectItemArray[i])
				{
					m_WaitCollectItemArray.RemoveAt(i);
					break;
				}
			}
		}

		//��������
		CMD_CS_S_ServerRemove ServerRemove;
		ZeroMemory(&ServerRemove,sizeof(ServerRemove));

		//ɾ��֪ͨ
		ServerRemove.wServerID=pBindParameter->wServiceID;
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_REMOVE,&ServerRemove,sizeof(ServerRemove),0L);

		if (m_wClubBindIndex != INVALID_WORD)
		{
			tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
			m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_S_CLUB_SERVER_CLOSE, &ServerRemove, sizeof(ServerRemove));
		}

		tagPersonalTableInfoEx PersonalTableInfoEx;
		ZeroMemory(&PersonalTableInfoEx, sizeof(PersonalTableInfoEx));

		PersonalTableInfoEx.dwServerID = pBindParameter->wServiceID;
		PersonalTableInfoEx.dwPersonalRoomID = 0;


		//��ȡ�ص�����������Ϸ����
		POSITION Position = NULL;

		//�ռ�����
		do
		{
			//��ȡ����
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.EnumServerItem(Position);

			//��������
			if (pGlobalServerItem != NULL)
			{
				if (pGlobalServerItem->GetServerID() == pBindParameter->wServiceID)
				{
					PersonalTableInfoEx.dwKindID = pGlobalServerItem->GetKindID();
					break;
				}
			}

		} while (Position != NULL);


		//�����������ת��
		//��������
		POSITION posChat = NULL;

		//���ҷ���
		do
		{
			//���ҷ���
			CGlobalChatItem * pGlobalChatItem = m_GlobalInfoManager.EnumChatItem(posChat);

			//��ֹ�ж�
			if (pGlobalChatItem == NULL) break;

			//�����������������Ϣ
			m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_DELETE_PERSONAL, &PersonalTableInfoEx, sizeof(tagPersonalTableInfoEx));

		} while (posChat != NULL);


		//ע������
		m_GlobalInfoManager.DeleteServerItem(pBindParameter->wServiceID);
	}

	//�㳡����
	if (pBindParameter->ServiceKind==ServiceKind_Plaza)
	{
		//��������
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//ע������
		m_GlobalInfoManager.DeletePlazaItem(pBindParameter->wServiceID);
	}

	//�������
	if (pBindParameter->ServiceKind==ServiceKind_Chat)
	{
		//��������
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//ע������
		m_GlobalInfoManager.DeleteChatItem(pBindParameter->wServiceID);
	}

	//���ֲ�����
	if (pBindParameter->ServiceKind == ServiceKind_Club)
	{
		m_wClubBindIndex = INVALID_WORD;
	}

	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_REGISTER_SERVER:	//ע�᷿��
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterServer));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterServer)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterServer * pRegisterServer=(CMD_CS_C_RegisterServer *)pData;

			//���ҷ���
			if (m_GlobalInfoManager.SearchServerItem(pRegisterServer->wServerID)!=NULL)
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("�Ѿ�������ͬ��ʶ����Ϸ������񣬷������ע��ʧ��"),CountArray(RegisterFailure.szDescribeString));

				//������Ϣ
				WORD wStringSize=CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize);

				//�ж�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//���ð�
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Game;
			(m_pBindParameter+wBindIndex)->wServiceID=pRegisterServer->wServerID;

			//��������
			tagGameServer GameServer;
			ZeroMemory(&GameServer,sizeof(GameServer));

			//��������
			GameServer.wKindID=pRegisterServer->wKindID;
			GameServer.wNodeID=pRegisterServer->wNodeID;
			GameServer.wSortID=pRegisterServer->wSortID;
			GameServer.wServerID=pRegisterServer->wServerID;
			GameServer.wServerKind=pRegisterServer->wServerKind;
			GameServer.wServerType=pRegisterServer->wServerType;
			GameServer.wServerLevel=pRegisterServer->wServerLevel;
			GameServer.wServerPort=pRegisterServer->wServerPort;
			GameServer.cbPlayMode = pRegisterServer->cbPlayMode;
			GameServer.lCellScore=pRegisterServer->lCellScore;
			GameServer.cbEnterMember=pRegisterServer->cbEnterMember;
			GameServer.lEnterScore=pRegisterServer->lEnterScore;
			GameServer.dwServerRule=pRegisterServer->dwServerRule;
			GameServer.dwOnLineCount=pRegisterServer->dwOnLineCount;
			GameServer.dwFullCount=pRegisterServer->dwFullCount;
			GameServer.wTableCount = pRegisterServer->wTableCount;
			GameServer.dwSetPlayerCount = pRegisterServer->dwSetPlayerCount;
			lstrcpyn(GameServer.szServerName,pRegisterServer->szServerName,CountArray(GameServer.szServerName));
			lstrcpyn(GameServer.szServerAddr,pRegisterServer->szServerAddr,CountArray(GameServer.szServerAddr));
			lstrcpyn(GameServer.szGameInfomation, pRegisterServer->szGameInfomation, CountArray(GameServer.szGameInfomation));
			GameServer.bGroupPersonalMaxChair = pRegisterServer->bGroupPersonalMaxChair;

			//ע�᷿��
			m_GlobalInfoManager.ActiveServerItem(wBindIndex, pRegisterServer->cbPlayMode, GameServer);

		

			//Ⱥ������
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,&GameServer,sizeof(GameServer),0L);

			//�����б�
			SendServerListItem(dwSocketID);

			SendMatchListItem(dwSocketID);

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			//����֪ͨ
			if (m_wCollectItem==INVALID_WORD)
			{
				m_wCollectItem=wBindIndex;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
			else m_WaitCollectItemArray.Add(wBindIndex);

			return true;
		}	
	case SUB_CS_C_REGISTER_PERSONAL:
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_S_RegisterPersonal));
			if (wDataSize!=sizeof(CMD_CS_S_RegisterPersonal)) return false;
			CMD_CS_S_RegisterPersonal * RegisterPersonal = (CMD_CS_S_RegisterPersonal *)pData;
			m_GlobalInfoManager.AddPersonalMaxCreate(* RegisterPersonal);
			return true;
		}
	case SUB_CS_C_REGISTER_GROUP:
		{
			//ASSERT(wDataSize == sizeof(CMD_CS_C_RegisterGroup));
			if (wDataSize != sizeof(CMD_CS_C_RegisterGroup)) return false;
			CMD_CS_C_RegisterGroup * RegisterPersonal = (CMD_CS_C_RegisterGroup *)pData;

			WORD wBindIndex = LOWORD(dwSocketID);
			(m_pBindParameter + wBindIndex)->ServiceKind = ServiceKind_Club;

			m_wClubBindIndex = wBindIndex;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS);
			return true;
		}
	case SUB_CS_C_REGISTER_CHAT:	//ע������
		{
			//���ð�
			WORD wBindIndex = LOWORD(dwSocketID);
			(m_pBindParameter + wBindIndex)->wServiceID = wBindIndex;
			(m_pBindParameter + wBindIndex)->ServiceKind = ServiceKind_Chat;

			tagChatServer ChatServer;
			ChatServer.wChatID = wBindIndex;
			ChatServer.dwClientAddr = (m_pBindParameter + wBindIndex)->dwClientAddr;
			ChatServer.dwSocketID = dwSocketID;

			m_GlobalInfoManager.ActiveChatItem(wBindIndex, ChatServer);

			//Ⱥ������
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO, SUB_CS_S_CHAT_INSERT, &ChatServer, sizeof(ChatServer), 0L);

			//������Ϣ
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_CHAT_INSERT);

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID, true, 0L);

			return true;
		}
	}

	return false;
}

//����״̬
bool CAttemperEngineSink::OnTCPNetworkMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SERVER_MODIFY:	//�����޸�
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_C_ServerModify)) return false;

			//��Ϣ����
			CMD_CS_C_ServerModify * pServerModify=(CMD_CS_C_ServerModify *)pData;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			//ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//���ҷ���
			//ASSERT(m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID)!=NULL);
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//�����޸�
			if (pGlobalServerItem!=NULL)
			{
				//���ñ���
				pGlobalServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGlobalServerItem->m_GameServer.wKindID=pServerModify->wKindID;
				pGlobalServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGlobalServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGlobalServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGlobalServerItem->m_GameServer.dwAndroidCount = pServerModify->dwAndroidCount;
				pGlobalServerItem->m_GameServer.dwFullCount=pServerModify->dwFullCount;		
				pGlobalServerItem->m_GameServer.dwSetPlayerCount = pServerModify->dwSetCount;
				lstrcpyn(pGlobalServerItem->m_GameServer.szServerName,pServerModify->szServerName,CountArray(pGlobalServerItem->m_GameServer.szServerName));
				lstrcpyn(pGlobalServerItem->m_GameServer.szServerAddr,pServerModify->szServerAddr,CountArray(pGlobalServerItem->m_GameServer.szServerAddr));
				lstrcpyn(pGlobalServerItem->m_GameServer.szGameInfomation, pServerModify->szGameInfomation, CountArray(pGlobalServerItem->m_GameServer.szGameInfomation));

				//��������
				CMD_CS_S_ServerModify ServerModify;
				ZeroMemory(&ServerModify,sizeof(ServerModify));

				//���ñ���
				ServerModify.wKindID=pServerModify->wKindID;
				ServerModify.wSortID=pServerModify->wSortID;
				ServerModify.wNodeID=pServerModify->wNodeID;
				ServerModify.wServerID=pBindParameter->wServiceID;
				ServerModify.wServerPort=pServerModify->wServerPort;
				ServerModify.dwOnLineCount=pServerModify->dwOnLineCount;
				ServerModify.dwFullCount=pServerModify->dwFullCount;
				ServerModify.dwAndroidCount= pServerModify->dwAndroidCount;
				lstrcpyn(ServerModify.szServerAddr,pServerModify->szServerAddr,CountArray(ServerModify.szServerAddr));
				lstrcpyn(ServerModify.szServerName,pServerModify->szServerName,CountArray(ServerModify.szServerName));

				//����֪ͨ
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_MODIFY,&ServerModify,sizeof(ServerModify),0L);
			}

			return true;
		}
	case SUB_CS_C_SEARCH_TABLE:			//��������
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_SearchTable));
			if (wDataSize!=sizeof(CMD_CS_C_SearchTable)) return false;

			CMD_CS_C_SearchTable *pSearchTable = (CMD_CS_C_SearchTable*)pData;

			//��������
			tagPersonalTableInfo* pPersonalTable = m_GlobalInfoManager.SearchTableByID(pSearchTable->dwPersonalRoomID);

			//��������
			CMD_CS_S_SearchTableResult SearchTableResult;
			ZeroMemory(&SearchTableResult, sizeof(CMD_CS_S_SearchTableResult));

			SearchTableResult.dwSocketID = pSearchTable->dwSocketID;
			SearchTableResult.dwClientAddr = pSearchTable->dwClientAddr;

			if(pPersonalTable == NULL)
			{
				SearchTableResult.dwServerID = 0;
				SearchTableResult.dwTableID = 0;
			}
			else
			{
				SearchTableResult.dwServerID = pPersonalTable->dwServerID;
				SearchTableResult.dwTableID = pPersonalTable->dwTableID;
				SearchTableResult.dwKindID = pPersonalTable->dwKindID;
				SearchTableResult.dwGroupID = pPersonalTable->dwGroupID;
			}
			
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_SEARCH_TABLE_RESULT, &SearchTableResult, sizeof(SearchTableResult));

			return true;
		}
	case SUB_CS_C_DISSUME_SEARCH_TABLE:			//��������
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_SearchTable));
			if (wDataSize!=sizeof(CMD_CS_C_SearchTable)) return false;

			CMD_CS_C_SearchTable *pSearchTable = (CMD_CS_C_SearchTable*)pData;

			//��������
			tagPersonalTableInfo* pPersonalTable = m_GlobalInfoManager.SearchTableByID(pSearchTable->dwPersonalRoomID);

			//��������
			CMD_CS_S_SearchTableResult SearchTableResult;
			ZeroMemory(&SearchTableResult, sizeof(CMD_CS_S_SearchTableResult));

			SearchTableResult.dwSocketID = pSearchTable->dwSocketID;
			SearchTableResult.dwClientAddr = pSearchTable->dwClientAddr;

			if(pPersonalTable == NULL)
			{
				SearchTableResult.dwServerID = 0;
				SearchTableResult.dwTableID = 0;
			}
			else
			{
				SearchTableResult.dwServerID = pPersonalTable->dwServerID;
				SearchTableResult.dwTableID = pPersonalTable->dwTableID;
			}
			
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_DISSUME_SEARCH_TABLE_RESULT, &SearchTableResult, sizeof(SearchTableResult));

			return true;
		}
	case SUB_CS_S_QUERY_ROOM_PASSWORD:
		{
			//Ч������
			//ASSERT(wDataSize == sizeof(CMD_CS_C_SearchTable));
			if (wDataSize != sizeof(CMD_CS_C_SearchTable)) return false;

			CMD_CS_C_SearchTable *pSearchTable = (CMD_CS_C_SearchTable*)pData;

			//��������
			tagPersonalTableInfo* pPersonalTable = m_GlobalInfoManager.SearchTableByID(pSearchTable->dwPersonalRoomID);

			//��������
			CMD_CS_S_QueryRoomPasswordResult SearchResult;
			ZeroMemory(&SearchResult, sizeof(CMD_CS_S_QueryRoomPasswordResult));

			SearchResult.dwSocketID = pSearchTable->dwSocketID;
			SearchResult.dwClientAddr = pSearchTable->dwClientAddr;

			if (pPersonalTable == NULL)
			{
				SearchResult.dwRoomPassword = 0;
			}
			else
			{
				bool bRefresh = true;
				for (int i = 0; i < MAX_PASSWORD_COUNT; i++)
				{
					if (pPersonalTable->hRoomPassword[i].enPS == enPasswordState::enUnShare)
					{
						bRefresh = false;
						break;
					}
				}
				if (bRefresh == true)
				{
					for (int i = 4; i < MAX_PASSWORD_COUNT; i++)
					{
						pPersonalTable->hRoomPassword[i].enPS = enPasswordState::enUnShare;
					}
				}
				for (int i = 0; i < MAX_PASSWORD_COUNT; i++)
				{
					if (pPersonalTable->hRoomPassword[i].enPS == enPasswordState::enUnShare)
					{
						pPersonalTable->hRoomPassword[i].enPS = enPasswordState::enShare;
						SearchResult.dwRoomPassword = pPersonalTable->hRoomPassword[i].dwRoomPass;
						break;
					}
				}				
			}
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_QUERY_ROOM_PASSWORD_RESULT, &SearchResult, sizeof(SearchResult));
			return true;
		}
	case SUB_CS_C_QUERY_GAME_SERVER:		//��ѯ���÷���
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_QueryGameServer));
			if (wDataSize!=sizeof(CMD_CS_C_QueryGameServer)) return false;

			CMD_CS_C_QueryGameServer *pQueryGameServer = (CMD_CS_C_QueryGameServer *)pData;

			//�ж��Ƿ���Դ���Լս����
			DWORD dwServerID = 0;
			bool bCanCreateRoom = false;

			CMD_CS_S_QueryGameServerResult QueryGameServerResult;
			ZeroMemory(&QueryGameServerResult, sizeof(CMD_CS_S_QueryGameServerResult));
			
			if (m_GlobalInfoManager.GetFreeServerCount(pQueryGameServer->dwKindID, pQueryGameServer->cbPlayMode,pQueryGameServer->dwGroupID>0) > 0)
			{
				if (m_GlobalInfoManager.CanCreatePersonalRoom(pQueryGameServer->dwKindID, pQueryGameServer->dwUserID, pQueryGameServer->cbPlayMode, pQueryGameServer->dwGroupID))
				{
					//��������
					dwServerID = m_GlobalInfoManager.GetFreeServer(pQueryGameServer->dwUserID, pQueryGameServer->dwKindID, pQueryGameServer->cbIsJoinGame, pQueryGameServer->cbPlayMode,pQueryGameServer->dwGroupID>0);
					bCanCreateRoom = true;

					if (dwServerID == 0)
					{
						lstrcpyn(QueryGameServerResult.szErrDescrybe, TEXT("�������������Ϸģʽ��ֻ�ܴ���һ�����䣬����Ϸ����������Լս����������"),  sizeof(QueryGameServerResult.szErrDescrybe));
					}
				}
				else
				{
					lstrcpyn(QueryGameServerResult.szErrDescrybe, TEXT("�ѵ��ﴴ���������ޣ���������ʧ�ܣ�"),  sizeof(QueryGameServerResult.szErrDescrybe));
				}
			}
			else
			{
				lstrcpyn(QueryGameServerResult.szErrDescrybe, TEXT("��ʱ���ɴ�����ǰ��Ϸ����ѡ��������Ϸ��"),  sizeof(QueryGameServerResult.szErrDescrybe));
			}


			//��������
			QueryGameServerResult.dwSocketID = pQueryGameServer->dwSocketID;
			QueryGameServerResult.dwClientAddr = pQueryGameServer->dwClientAddr;
			QueryGameServerResult.dwServerID = dwServerID;
			QueryGameServerResult.bCanCreateRoom= bCanCreateRoom;


			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_QUERY_GAME_SERVER_RESULT, &QueryGameServerResult, sizeof(CMD_CS_S_QueryGameServerResult));

			//���ҷ���
			DWORD dwKeyServerID = INVALID_DWORD;
			INT_PTR nSize = m_GlobalInfoManager.m_ServerTableCountArray.GetCount();
			for (INT_PTR i = 0; i < nSize; ++i)
			{
				tagServerTableCount* pServerTableCount = m_GlobalInfoManager.m_ServerTableCountArray.GetAt(i);
				if (pServerTableCount->dwKindID == pQueryGameServer->dwKindID && pServerTableCount->cbPlayMode == pQueryGameServer->cbPlayMode)
				{
					dwKeyServerID = pServerTableCount->dwServerID;
					break;
				}
			}

			//
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.SearchServerItem(dwKeyServerID);
			if (!bCanCreateRoom && pGlobalServerItem && (pGlobalServerItem->m_GameServer.bGroupPersonalMaxChair))
			{
				//ת������
				if (m_wClubBindIndex != INVALID_WORD)
				{
					//��������
					CMD_CS_C_ClubDeleteConfig ClubDeleteConfig;
					ZeroMemory(&ClubDeleteConfig, sizeof(ClubDeleteConfig));

					ClubDeleteConfig.dwUserID = pQueryGameServer->dwUserID;
					ClubDeleteConfig.dwGroupID = pQueryGameServer->dwGroupID;
					ClubDeleteConfig.dwConfigID = pQueryGameServer->dwConfigID;

					tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
					m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DELETE_CONFIG, &ClubDeleteConfig, sizeof(ClubDeleteConfig));
				}
			}

			return true;
		}
	case SUB_CS_C_CREATE_TABLE:		//��������
		{
			//У������
			//ASSERT(wDataSize == sizeof(CMD_CS_C_CreateTable));
			if(wDataSize != sizeof(CMD_CS_C_CreateTable)) return false;

			CMD_CS_C_CreateTable* pCreateTable = (CMD_CS_C_CreateTable*)pData;

			//����ID
			DWORD dwRoomID;
			dwRoomID = m_GlobalInfoManager.RandPersonalRoomID();

			pCreateTable->PersonalTable.dwPersonalRoomID  = dwRoomID;
			//���ܴ�����
			m_GlobalInfoManager.ActiveGroupRoom(pCreateTable->PersonalTable.dwGroupID, dwRoomID);

			//��������
			m_GlobalInfoManager.AddServerTable(dwRoomID, pCreateTable->PersonalTable);
			m_GlobalInfoManager.RemoveFreeServerTable(pCreateTable->PersonalTable.dwServerID);

			//��������
			CMD_CS_S_CreateTableResult CreateTableResult;
			ZeroMemory(&CreateTableResult, sizeof(CMD_CS_S_CreateTableResult));

			CreateTableResult.dwSocketID = pCreateTable->dwSocketID;
			CreateTableResult.PersonalTable.dwTableID = pCreateTable->PersonalTable.dwTableID;
			CreateTableResult.PersonalTable.dwServerID = pCreateTable->PersonalTable.dwServerID;
			CreateTableResult.PersonalTable.dwKindID = pCreateTable->PersonalTable.dwKindID;
			CreateTableResult.PersonalTable.dwUserID = pCreateTable->PersonalTable.dwUserID;
			CreateTableResult.PersonalTable.lCellScore = pCreateTable->PersonalTable.lCellScore;
			CreateTableResult.PersonalTable.dwDrawTimeLimit = pCreateTable->PersonalTable.dwDrawTimeLimit;
			CreateTableResult.PersonalTable.dwDrawCountLimit = pCreateTable->PersonalTable.dwDrawCountLimit;
			CreateTableResult.PersonalTable.dwPersonalRoomID = dwRoomID;
			CreateTableResult.PersonalTable.lEnterScoreLimit = pCreateTable->PersonalTable.lEnterScoreLimit;
			CreateTableResult.PersonalTable.lLeaveScoreLimit = pCreateTable->PersonalTable.lLeaveScoreLimit;
			lstrcpyn(CreateTableResult.szClientAddr, pCreateTable->szClientAddr, CountArray(CreateTableResult.szClientAddr));
			CreateTableResult.PersonalTable.wJoinGamePeopleCount = pCreateTable->PersonalTable.wJoinGamePeopleCount;
			CreateTableResult.PersonalTable.dwRoomTax = pCreateTable->PersonalTable.dwRoomTax;
			CreateTableResult.cbPayMode = pCreateTable->cbPayMode;
			CreateTableResult.lNeedRoomCard = pCreateTable->lNeedRoomCard;
			CreateTableResult.PersonalTable.wCurSitPlayer = pCreateTable->PersonalTable.wCurSitPlayer;
			CreateTableResult.cbGameMode = pCreateTable->cbGameMode;
			CreateTableResult.PersonalTable.cbPlayMode = pCreateTable->PersonalTable.cbPlayMode;
			CreateTableResult.PersonalTable.dwGroupID = pCreateTable->PersonalTable.dwGroupID;
			memcpy(CreateTableResult.PersonalTable.cbGameRule, pCreateTable->PersonalTable.cbGameRule, sizeof(CreateTableResult.PersonalTable.cbGameRule));
			memcpy(CreateTableResult.PersonalTable.szConfigName, pCreateTable->PersonalTable.szConfigName, sizeof(CreateTableResult.PersonalTable.szConfigName));
			CreateTableResult.PersonalTable.dwConfigID = pCreateTable->PersonalTable.dwConfigID;
			CreateTableResult.PersonalTable.lMaxChairGameMinChip = pCreateTable->PersonalTable.lMaxChairGameMinChip;
			CreateTableResult.dwClientAddr = pCreateTable->dwClientAddr;
			//OutputDebugString(TEXT("ptdtai *** Լս�������յ������ɹ���Ϣ ����Ϸ������������Ϣ"));
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_CREATE_TABLE_RESULT, &CreateTableResult, sizeof(CMD_CS_S_CreateTableResult));

			if (pCreateTable->PersonalTable.cbJoinMode == 1)
			{
				//���������������������Ϣ
				tagPersonalTableInfoEx PersonalTableInfoEx;
				ZeroMemory(&PersonalTableInfoEx, sizeof(PersonalTableInfoEx));

				//PersonalTableInfoEx �������һ���ֶΣ��������ֶ���PersonalTable ��ͬ
				memcpy_s(&PersonalTableInfoEx, sizeof(PersonalTableInfoEx), &CreateTableResult.PersonalTable, sizeof(CreateTableResult.PersonalTable));

				//��������
				POSITION posChat = NULL;

				//���ҷ���
				do
				{
					//���ҷ���
					CGlobalChatItem * pGlobalChatItem = m_GlobalInfoManager.EnumChatItem(posChat);

					//��ֹ�ж�
					if (pGlobalChatItem == NULL) break;

					//�����������������Ϣ
					m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_PERSONAL_INFO, &PersonalTableInfoEx, sizeof(tagPersonalTableInfoEx));

				} while (posChat != NULL);
			}
			
			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtai ****** Լս�������յ������ɹ���Ϣ �� ������ ������Ϣ dwGroupID = %d m_wClubBindIndex = %d"), pCreateTable->PersonalTable.dwGroupID, m_wClubBindIndex);
			//OutputDebugString(szInfo);

			//ת��������
			if (pCreateTable->PersonalTable.dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//������Ϣ
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				tagClubTableInfo TableInfo;
				ZeroMemory(&TableInfo, sizeof(TableInfo));
				TableInfo.dwServerID = pCreateTable->PersonalTable.dwServerID;
				TableInfo.dwKindID = pCreateTable->PersonalTable.dwKindID;
				TableInfo.wTableID = (WORD)pCreateTable->PersonalTable.dwTableID;
				TableInfo.dwUserID = pCreateTable->PersonalTable.dwUserID;
				TableInfo.dwDrawCountLimit = pCreateTable->PersonalTable.dwDrawCountLimit;
				TableInfo.dwDrawTimeLimit = pCreateTable->PersonalTable.dwDrawTimeLimit;
				TableInfo.lCellScore = pCreateTable->PersonalTable.lCellScore;
				TableInfo.dwUserID = pCreateTable->PersonalTable.dwUserID;
				TableInfo.dwPersonalRoomID = pCreateTable->PersonalTable.dwPersonalRoomID;
				TableInfo.wJoinGamePeopleCount = pCreateTable->PersonalTable.wJoinGamePeopleCount;
				TableInfo.wCurSitPlayer = pCreateTable->PersonalTable.wCurSitPlayer;
				TableInfo.cbPlayMode = pCreateTable->PersonalTable.cbPlayMode;
				TableInfo.lEnterScoreLimit = pCreateTable->PersonalTable.lEnterScoreLimit;
				TableInfo.lLeaveScoreLimit = pCreateTable->PersonalTable.lLeaveScoreLimit;
				TableInfo.cbJoinMode = pCreateTable->PersonalTable.cbJoinMode;
				TableInfo.dwGroupID = pCreateTable->PersonalTable.dwGroupID;
				TableInfo.bGameStart = pCreateTable->PersonalTable.bGameStart;
				TableInfo.cbPayMode = pCreateTable->PersonalTable.cbPayMode;
				CopyMemory(TableInfo.szNickName, pCreateTable->PersonalTable.szNickName, sizeof(TableInfo.szNickName));
				CopyMemory(TableInfo.szCreateTime, pCreateTable->PersonalTable.szCreateTime, sizeof(TableInfo.szCreateTime));
				CopyMemory(TableInfo.cbGameRule, pCreateTable->PersonalTable.cbGameRule, sizeof(TableInfo.cbGameRule));
				TableInfo.dwConfigID = pCreateTable->PersonalTable.dwConfigID;
				CopyMemory(TableInfo.szConfigName, pCreateTable->PersonalTable.szConfigName, sizeof(TableInfo.szConfigName));
				CopyMemory(TableInfo.dwPlayerID, pCreateTable->PersonalTable.dwPlayerUserID, sizeof(TableInfo.dwPlayerID));
				TableInfo.lMaxChairGameMinChip = pCreateTable->PersonalTable.lMaxChairGameMinChip;

				wsprintf(szInfo, TEXT("ptdtai ****** Լս�������յ������ɹ���Ϣ �� ������ ������Ϣ dwGroupID = %d m_wClubBindIndex = %d ����"), pCreateTable->PersonalTable.dwGroupID, m_wClubBindIndex);
				//OutputDebugString(szInfo);

				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_CREATE_TABLE, &TableInfo, sizeof(tagClubTableInfo));
			}

			return true;
		}
	case SUB_CS_C_DISMISS_TABLE:		//��ɢ����
		{
			//У������
			//ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTable));
			if(wDataSize != sizeof(CMD_CS_C_DismissTable)) return false;

			CMD_CS_C_DismissTable* pDismissTable = (CMD_CS_C_DismissTable*)pData;

			tagPersonalTableInfo * pPersonalTableInfo = m_GlobalInfoManager.SearchTableByTableIDAndServerID(pDismissTable->dwServerID, pDismissTable->dwTableID);
			if (pPersonalTableInfo == NULL)
			{
				return true;
			}

			//ת��������
			if (pPersonalTableInfo->dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//��������
				CMD_CS_S_DismissClubTable DismissClubTable;
				DismissClubTable.dwGroupID = pPersonalTableInfo->dwGroupID;
				DismissClubTable.dwKindID = pPersonalTableInfo->dwKindID;
				DismissClubTable.dwPersonalRoomID = pPersonalTableInfo->dwPersonalRoomID;
				DismissClubTable.dwServerID = pPersonalTableInfo->dwServerID;
				DismissClubTable.wTableID = (WORD)pPersonalTableInfo->dwTableID;
				DismissClubTable.bDeleteServer = false;
				//������Ϣ
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &DismissClubTable, sizeof(DismissClubTable));
			}

			tagPersonalTableInfoEx PersonalTableInfoEx;
			ZeroMemory(&PersonalTableInfoEx, sizeof(PersonalTableInfoEx));
			memcpy_s(&PersonalTableInfoEx, sizeof(PersonalTableInfoEx), pPersonalTableInfo, sizeof(tagPersonalTableInfo));

			//�����������ת��
			//��������
			POSITION posChat = NULL;

			//���ҷ���
			do
			{
				//���ҷ���
				CGlobalChatItem * pGlobalChatItem = m_GlobalInfoManager.EnumChatItem(posChat);

				//��ֹ�ж�
				if (pGlobalChatItem == NULL) break;

				//�����������������Ϣ
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_DELETE_PERSONAL, &PersonalTableInfoEx, sizeof(tagPersonalTableInfoEx));

			} while (posChat != NULL);



			CMD_CS_C_DismissTableResult  DismissTableResult;
			DismissTableResult.dwSocketID = pDismissTable->dwSocketID;
			memcpy(&DismissTableResult.PersonalTableInfo, pPersonalTableInfo, sizeof(tagPersonalTableInfo));

			//��������
			m_GlobalInfoManager.RemoveServerTable(pDismissTable->dwServerID, pDismissTable->dwTableID);
			m_GlobalInfoManager.AddFreeServerTable(pDismissTable->dwServerID);
			//���ܴ�����
			m_GlobalInfoManager.DeleteGroupRoom(pPersonalTableInfo->dwGroupID, pPersonalTableInfo->dwPersonalRoomID);

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE_RESULT, &DismissTableResult, sizeof(CMD_CS_C_DismissTableResult));

			return true;
		}
	case SUB_CS_C_UPDATE_TABLE_COUNT:
		{
			//У������
			//ASSERT(wDataSize == sizeof(CMD_CS_C_UpdateTablePlayerCount));
			if (wDataSize != sizeof(CMD_CS_C_UpdateTablePlayerCount)) return false;

			CMD_CS_C_UpdateTablePlayerCount* pUpdateTable = (CMD_CS_C_UpdateTablePlayerCount*)pData;

			tagPersonalTableInfo * pPersonalTableInfo = m_GlobalInfoManager.SearchTableByTableIDAndServerID(pUpdateTable->wServerID, pUpdateTable->wTableID);
			if (pPersonalTableInfo == NULL)
			{
				return true;
			}

			pPersonalTableInfo->wCurSitPlayer = pUpdateTable->wSitCount;

			if (pUpdateTable->dwUserID == 0)
				pPersonalTableInfo->bGameStart = pUpdateTable->bGameStart;

			if (pUpdateTable->bGameStart == false && pUpdateTable->wChairID >= 0 && pUpdateTable->wChairID < GROUP_MAX_CHAIR_GAME_CHAIR_COUNT)
			{
				if (pUpdateTable->bSit)
				{
					pPersonalTableInfo->dwPlayerUserID[pUpdateTable->wChairID] = pUpdateTable->dwUserID;
				}
				else if (pUpdateTable->dwUserID != 0)
					pPersonalTableInfo->dwPlayerUserID[pUpdateTable->wChairID] = 0;
			}

			//�������
			CMD_CS_S_UpdateTable UpdateTable;
			ZeroMemory(&UpdateTable, sizeof(CMD_CS_S_UpdateTable));

			UpdateTable.dwKindID = pPersonalTableInfo->dwKindID;
			UpdateTable.dwServerID = pPersonalTableInfo->dwServerID;
			UpdateTable.wTableID = (WORD)pPersonalTableInfo->dwTableID;
			UpdateTable.dwPersonalRoomID = pPersonalTableInfo->dwPersonalRoomID;
			UpdateTable.dwGroupID = pPersonalTableInfo->dwGroupID;
			UpdateTable.bGameStart = pUpdateTable->bGameStart;
			UpdateTable.bSit = pUpdateTable->bSit;
			UpdateTable.dwUserID = pUpdateTable->dwUserID;
			UpdateTable.wChairID = pUpdateTable->wChairID;

			//�����������ת��
			//��������
			POSITION posChat = NULL;

			//���ҷ���
			do
			{
				//���ҷ���
				CGlobalChatItem * pGlobalChatItem = m_GlobalInfoManager.EnumChatItem(posChat);

				//��ֹ�ж�
				if (pGlobalChatItem == NULL) break;

				//�����������������Ϣ
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_PERSONAL_INFO_UPDATE, &UpdateTable, sizeof(UpdateTable));

			} while (posChat != NULL);

			//ת��������
			if (pPersonalTableInfo->dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//������Ϣ
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_USER_STATUS, &UpdateTable, sizeof(CMD_CS_S_UpdateTable));
			}
			return true;
		}
	case SUB_CS_C_UPDATE_BANKER_USERID:
		{
			//У������
			//ASSERT(wDataSize == sizeof(CMD_CS_C_UpdateGroupMaxChairBankerUserID));
			if (wDataSize != sizeof(CMD_CS_C_UpdateGroupMaxChairBankerUserID)) return false;

			CMD_CS_C_UpdateGroupMaxChairBankerUserID* pUpdateGroupMaxChairBankerUserID = (CMD_CS_C_UpdateGroupMaxChairBankerUserID*)pData;

			tagPersonalTableInfo * pPersonalTableInfo = m_GlobalInfoManager.SearchTableByTableIDAndServerID(pUpdateGroupMaxChairBankerUserID->wServerID, pUpdateGroupMaxChairBankerUserID->wTableID);
			if (pPersonalTableInfo == NULL)
			{
				return true;
			}

			pPersonalTableInfo->dwBankerUserID = pUpdateGroupMaxChairBankerUserID->dwBankerUserID;

			//�������
			CMD_CS_S_TableChangeBanker TableChangeBanker;
			ZeroMemory(&TableChangeBanker, sizeof(CMD_CS_S_TableChangeBanker));

			TableChangeBanker.dwKindID = pPersonalTableInfo->dwKindID;
			TableChangeBanker.dwServerID = pPersonalTableInfo->dwServerID;
			TableChangeBanker.wTableID = (WORD)pPersonalTableInfo->dwTableID;
			TableChangeBanker.dwPersonalRoomID = pPersonalTableInfo->dwPersonalRoomID;
			TableChangeBanker.dwGroupID = pPersonalTableInfo->dwGroupID;
			TableChangeBanker.wBankerChairID = pUpdateGroupMaxChairBankerUserID->wBankerChairID;
			TableChangeBanker.dwBankerUserID = pPersonalTableInfo->dwBankerUserID;

			//ת��������
			if (pPersonalTableInfo->dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//������Ϣ
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_TABLE_CHANGEBANKER, &TableChangeBanker, sizeof(CMD_CS_S_TableChangeBanker));
			}

			return true;
		}
	case SUB_CS_S_QUERY_PERSONAL_ROOM_LIST:		//���󷿼��б�
		{
			//У������
			//ASSERT(wDataSize == sizeof(CMD_MB_SC_QeuryPersonalRoomList));
			if(wDataSize != sizeof(CMD_MB_SC_QeuryPersonalRoomList)) return false;

			CMD_MB_SC_QeuryPersonalRoomList * QeuryPersonalRoomList = (CMD_MB_SC_QeuryPersonalRoomList * ) pData;

			CMD_CS_C_HostCreatRoomInfo  cmdHostCreatRoomInfo;
			ZeroMemory(&cmdHostCreatRoomInfo,  sizeof(CMD_CS_C_HostCreatRoomInfo));

			cmdHostCreatRoomInfo.wSocketID = QeuryPersonalRoomList->dwSocketID;
			cmdHostCreatRoomInfo.HostCreatRoomInfo.dwUserID = QeuryPersonalRoomList->dwUserID;			
			cmdHostCreatRoomInfo.nCreatRoomCount = m_GlobalInfoManager.GetHostCreatePersonalRoom(cmdHostCreatRoomInfo.HostCreatRoomInfo);

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_PERSONAL_ROOM_LIST_RESULT, &cmdHostCreatRoomInfo, sizeof(CMD_CS_C_HostCreatRoomInfo));

			return true;
		}
	case SUB_CS_C_QUERY_GOLD_ROOM_LIST://������Լս���б�
		{
			CMD_CS_C_QueryGoldPersonalRoomList *pGoldPersonalRoomList = (CMD_CS_C_QueryGoldPersonalRoomList *)pData;
			if (pGoldPersonalRoomList == NULL)
			{
				return false;
			}

			WORD wKindID = pGoldPersonalRoomList->wKindID;
			BYTE cbPlayMode = pGoldPersonalRoomList->cbPlayMode;
			DWORD dwGroupID = pGoldPersonalRoomList->dwGroupID;

			CMD_CS_C_QueryGoldPersonalRoomListResult QueryGoldPersonalRoomListResult;
			QueryGoldPersonalRoomListResult.dwSocketID = pGoldPersonalRoomList->dwSocketID;

			//��������
			WORD wPacketSize = sizeof(CMD_CS_C_QueryGoldPersonalRoomListResult);
			POSITION Position = NULL;
			BYTE cbBuffer[SOCKET_TCP_PACKET];
			memcpy(cbBuffer, &QueryGoldPersonalRoomListResult, sizeof(QueryGoldPersonalRoomListResult));
			int nSize = sizeof(CMD_CS_C_GoldPersonalRoomInfo);

			//�ռ�����
			do
			{
				//��������
				if ((wPacketSize + sizeof(CMD_CS_C_GoldPersonalRoomInfo)) > sizeof(cbBuffer))
				{
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_GOLD_ROOM_LIST_RESULT, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}


				//��ȡ����
				DWORD dwRoomID = 0;
				CMD_CS_C_GoldPersonalRoomInfo * GoldPersonalRoomInfo = (CMD_CS_C_GoldPersonalRoomInfo *)(cbBuffer + wPacketSize);
				tagPersonalTableInfo * pPersonalTableTemp = m_GlobalInfoManager.SearchTableByPosition(wKindID, cbPlayMode, dwGroupID, Position, dwRoomID);


				//��������
				if (pPersonalTableTemp != NULL)
				{
					wPacketSize += sizeof(CMD_CS_C_GoldPersonalRoomInfo);

					GoldPersonalRoomInfo->dwKindID = pPersonalTableTemp->dwKindID;
					GoldPersonalRoomInfo->dwRoomID = pPersonalTableTemp->dwPersonalRoomID;
					GoldPersonalRoomInfo->dwServerID = pPersonalTableTemp->dwServerID;
					GoldPersonalRoomInfo->dwTableID = pPersonalTableTemp->dwTableID;
					GoldPersonalRoomInfo->lCellScore = pPersonalTableTemp->lCellScore;
					GoldPersonalRoomInfo->dwRoomTax = pPersonalTableTemp->dwRoomTax;
					GoldPersonalRoomInfo->wBeginFreeTime = pPersonalTableTemp->wBeginFreeTime;
					GoldPersonalRoomInfo->wEndFreeTime = pPersonalTableTemp->wEndFreeTime;
					GoldPersonalRoomInfo->lEnterScoreLimit = pPersonalTableTemp->lEnterScoreLimit;
					GoldPersonalRoomInfo->lLeaveScoreLimit = pPersonalTableTemp->lLeaveScoreLimit;
					GoldPersonalRoomInfo->wPlayingUser = pPersonalTableTemp->wCurSitPlayer;
					GoldPersonalRoomInfo->wPlayerCount = pPersonalTableTemp->wJoinGamePeopleCount;
					memcpy(GoldPersonalRoomInfo->cbGameRule, pPersonalTableTemp->cbGameRule, sizeof(GoldPersonalRoomInfo->cbGameRule));
					memcpy(GoldPersonalRoomInfo->szCreateTime, pPersonalTableTemp->szCreateTime, sizeof(GoldPersonalRoomInfo->szCreateTime));
				}

			} while (Position != NULL);

			//��������
			if (wPacketSize > 0)
			{
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_GOLD_ROOM_LIST_RESULT, cbBuffer, wPacketSize);
			}

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_GOLD_ROOM_LIST_FINISH, &QueryGoldPersonalRoomListResult, sizeof(CMD_CS_C_QueryGoldPersonalRoomListResult));

			return true;
		}
	case SUB_CS_C_CURRENCE_GROUP_FEE:		//�����˷�������
		{
			//Ч������
			//ASSERT(wDataSize == sizeof(CMD_GR_CurrenceGroupFee));
			if (wDataSize != sizeof(CMD_GR_CurrenceGroupFee)) return false;

			CMD_GR_CurrenceGroupFee *pCurrenceGroupFee = (CMD_GR_CurrenceGroupFee *)pData;

			//ת��������		
			if (pCurrenceGroupFee->dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//������Ϣ
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_CURRENCE_FEE, pCurrenceGroupFee, sizeof(CMD_GR_CurrenceGroupFee));
			}

			return true;
		}
	case SUB_CS_C_UPDATE_GROUP_SCORE:
		{
			//У������
			//ASSERT(wDataSize == sizeof(CMD_CS_S_UpdateClubScore));
			if (wDataSize != sizeof(CMD_CS_S_UpdateClubScore)) return false;

			CMD_CS_S_UpdateClubScore* pUpdateTable = (CMD_CS_S_UpdateClubScore*)pData;
			if (pUpdateTable->dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//������Ϣ
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_UPDATE_GROUP_SCORE, pData, wDataSize);

			}
			return true;
		}
	case SUB_CS_C_QUERY_ROOM_BATTLE_INFO_RES:
		{
			//У������
			//ASSERT(wDataSize == sizeof(CMD_CS_S_QueryTableInfoResult));
			if (wDataSize != sizeof(CMD_CS_S_QueryTableInfoResult)) return false;

			//������Ϣ
			if (m_wClubBindIndex != INVALID_WORD)
			{
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_ROOM_BATTLE_INFO_RES, pData, wDataSize);
			}
			return true;
		}
	}

	return false;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_USER_ENTER:		//�û�����
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_UserEnter));
			if (wDataSize!=sizeof(CMD_CS_C_UserEnter)) return false;

			//��Ϣ����
			CMD_CS_C_UserEnter * pUserEnter=(CMD_CS_C_UserEnter *)pData;
			pUserEnter->szNickName[CountArray(pUserEnter->szNickName)-1]=0;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			//ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//��������
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

			//���ñ���
			GlobalUserInfo.dwUserID=pUserEnter->dwUserID;
			GlobalUserInfo.dwGameID=pUserEnter->dwGameID;
			lstrcpyn(GlobalUserInfo.szNickName,pUserEnter->szNickName,CountArray(GlobalUserInfo.szNickName));

			//������Ϣ
			GlobalUserInfo.cbGender=pUserEnter->cbGender;
			GlobalUserInfo.cbMemberOrder=pUserEnter->cbMemberOrder;
			GlobalUserInfo.cbMasterOrder=pUserEnter->cbMasterOrder;
			
			//������ϸ��Ϣ
			memcpy(&GlobalUserInfo.userInfo, &pUserEnter->userInfo, sizeof(tagUserInfo));

			//�����û�
			m_GlobalInfoManager.ActiveUserItem(GlobalUserInfo,pBindParameter->wServiceID);


			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//ͬ��״̬
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserEnter->dwUserID;
			UserGameStatus.cbGameStatus=pUserEnter->userInfo.cbUserStatus;
			UserGameStatus.wKindID   = pBindParameter->ServiceKind;
			UserGameStatus.wServerID = pBindParameter->wServiceID;
			UserGameStatus.wTableID  = INVALID_TABLE;
			UserGameStatus.wChairID  = INVALID_CHAIR;
			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));
			

			//��������
			POSITION posChat=NULL;

			//���ҷ���
			do
			{
				//���ҷ���
				CGlobalChatItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//��ֹ�ж�
				if (pGlobalChatItem==NULL) break;

				//����״̬
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	case SUB_CS_C_USER_LEAVE:		//�û��뿪
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_UserLeave));
			if (wDataSize!=sizeof(CMD_CS_C_UserLeave)) return false;

			//��Ϣ����
			CMD_CS_C_UserLeave * pUserLeave=(CMD_CS_C_UserLeave *)pData;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			//ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//ɾ���û�
			m_GlobalInfoManager.DeleteUserItem(pUserLeave->dwUserID,pBindParameter->wServiceID);

			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//ͬ��״̬
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserLeave->dwUserID;
			UserGameStatus.cbGameStatus=US_NULL;
			UserGameStatus.wKindID   = pBindParameter->ServiceKind;
			UserGameStatus.wServerID = pBindParameter->wServiceID;
			UserGameStatus.wTableID  = INVALID_TABLE;
			UserGameStatus.wChairID  = INVALID_CHAIR;

			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));

			POSITION posChat=NULL;

			//���ҷ���
			do
			{
				//���ҷ���
				CGlobalChatItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//��ֹ�ж�
				if (pGlobalChatItem==NULL) break;

				//����״̬
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	case SUB_CS_C_USER_FINISH:		//�û����
		{
			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			//ASSERT((m_wCollectItem==wBindIndex)&&(pBindParameter->ServiceKind==ServiceKind_Game));
			if ((m_wCollectItem!=wBindIndex)||(pBindParameter->ServiceKind!=ServiceKind_Game)) return false;

			//���ñ���
			m_wCollectItem=INVALID_WORD;

			//�����л�
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//�л�����
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//ɾ������
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//������Ϣ
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}

			return true;
		}
	case SUB_CS_C_USER_STATUS:		//�û�״̬
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_UserStatus));
			if (wDataSize!=sizeof(CMD_CS_C_UserStatus)) return false;

			//��Ϣ����
			CMD_CS_C_UserStatus * pUserStatus=(CMD_CS_C_UserStatus *)pData;

			CGlobalUserItem* pGlobalUserItem = m_GlobalInfoManager.SearchUserItemByUserID(pUserStatus->dwUserID);
			if(pGlobalUserItem != NULL)
			{
				//pGlobalUserItem->UpdateStatus(pUserStatus->wTableID, pUserStatus->wChairID, pUserStatus->cbUserStatus);

			}

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//ͬ��״̬
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserStatus->dwUserID;
			UserGameStatus.cbGameStatus=pUserStatus->cbUserStatus;
			UserGameStatus.wKindID =pUserStatus->wKindID;
			UserGameStatus.wServerID=pUserStatus->wServerID;
			UserGameStatus.wTableID=pUserStatus->wTableID;
			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));

			//��������
			POSITION posChat=NULL;

			//���ҷ���
			do
			{
				//���ҷ���
				CGlobalChatItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//��ֹ�ж�
				if (pGlobalChatItem==NULL) break;

				//����״̬
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	}

	return false;
}

//Զ�̷���
bool CAttemperEngineSink::OnTCPNetworkMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SEARCH_CORRESPOND:	//Э������
		{
			////Ч�����
			////ASSERT(wDataSize==sizeof(CMD_CS_C_SearchCorrespond));
			//if (wDataSize!=sizeof(CMD_CS_C_SearchCorrespond)) return false;

			////������Ϣ
			//CMD_CS_C_SearchCorrespond * pSearchCorrespond=(CMD_CS_C_SearchCorrespond *)pData;
			//pSearchCorrespond->szNickName[CountArray(pSearchCorrespond->szNickName)-1]=0;

			////��������
			//CMD_CS_S_SearchCorrespond SearchCorrespond;
			//ZeroMemory(&SearchCorrespond,sizeof(SearchCorrespond));

			////����û�
			//CGlobalUserItem * ResultUserItem[2];
			//ZeroMemory(ResultUserItem,sizeof(ResultUserItem));

			////���ñ���
			//SearchCorrespond.dwSocketID=pSearchCorrespond->dwSocketID;
			//SearchCorrespond.dwClientAddr=pSearchCorrespond->dwClientAddr;

			////�����û�
			//if (pSearchCorrespond->dwGameID!=0L)
			//{
			//	ResultUserItem[0]=m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID);
			//}

			////�����û�
			//if (pSearchCorrespond->szNickName[0]!=0)
			//{
			//	_wcslwr_s(pSearchCorrespond->szNickName);
			//	ResultUserItem[1]=m_GlobalInfoManager.SearchUserItemByNickName(pSearchCorrespond->szNickName);
			//}

			////���ý��
			//for (BYTE i=0;i<CountArray(ResultUserItem);i++)
			//{
			//	if (ResultUserItem[i]!=NULL)
			//	{
			//		//��������
			//		WORD wServerIndex=0;

			//		//���ҷ���
			//		do
			//		{
			//			//���ҷ���
			//			CGlobalServerItem * pGlobalServerItem=ResultUserItem[i]->EnumServerItem(wServerIndex++);

			//			//��ֹ�ж�
			//			if (pGlobalServerItem==NULL) break;
			//			if (SearchCorrespond.wUserCount>=CountArray(SearchCorrespond.UserRemoteInfo)) break;

			//			//��������
			//			WORD wIndex=SearchCorrespond.wUserCount++;

			//			//������Ϣ
			//			SearchCorrespond.UserRemoteInfo[wIndex].cbGender=ResultUserItem[i]->GetGender();
			//			SearchCorrespond.UserRemoteInfo[wIndex].cbMemberOrder=ResultUserItem[i]->GetMemberOrder();
			//			SearchCorrespond.UserRemoteInfo[wIndex].cbMasterOrder=ResultUserItem[i]->GetMasterOrder();

			//			//�û���Ϣ
			//			SearchCorrespond.UserRemoteInfo[wIndex].dwUserID=ResultUserItem[i]->GetUserID();
			//			SearchCorrespond.UserRemoteInfo[wIndex].dwGameID=ResultUserItem[i]->GetGameID();
			//			lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szNickName,ResultUserItem[i]->GetNickName(),LEN_NICKNAME);

			//			//������Ϣ
			//			SearchCorrespond.UserRemoteInfo[wIndex].wKindID=pGlobalServerItem->GetKindID();
			//			SearchCorrespond.UserRemoteInfo[wIndex].wServerID=pGlobalServerItem->GetServerID();
			//			lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szGameServer,pGlobalServerItem->m_GameServer.szServerName,LEN_SERVER);

			//			tagUserInfo* pUserInfo = ResultUserItem[i]->GetUserInfo();
			//			SearchCorrespond.UserRemoteInfo[wIndex].wFaceID = pUserInfo->wFaceID;
			//			SearchCorrespond.UserRemoteInfo[wIndex].wChairID = pUserInfo->wChairID;
			//			SearchCorrespond.UserRemoteInfo[wIndex].cbUserStatus = pUserInfo->cbUserStatus;
			//			SearchCorrespond.UserRemoteInfo[wIndex].wTableID = pUserInfo->wTableID;
			//			SearchCorrespond.UserRemoteInfo[wIndex].wLastTableID = pUserInfo->wLastTableID;
			//			

			//		} while (true);
			//	}
			//}

			////��������
			//WORD wHeadSize=sizeof(SearchCorrespond)-sizeof(SearchCorrespond.UserRemoteInfo);
			//WORD wItemSize=sizeof(SearchCorrespond.UserRemoteInfo[0])*SearchCorrespond.wUserCount;
			//m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_CORRESPOND,&SearchCorrespond,wHeadSize+wItemSize);

			return true;
		}
	case SUB_CS_C_SEARCH_ALLCORRESPOND:	//Э������
		{
			//CMD_CS_C_AllSearchCorrespond * pSearchCorrespond=(CMD_CS_C_AllSearchCorrespond *)pData;
			//if(pSearchCorrespond->dwCount > 512) return false;

			//BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
			//CMD_CS_S_SearchAllCorrespond* pAllSearchCorrespond = (CMD_CS_S_SearchAllCorrespond*)cbDataBuffer;
			////���ñ���
			//pAllSearchCorrespond->dwSocketID=pSearchCorrespond->dwSocketID;
			//pAllSearchCorrespond->dwClientAddr=pSearchCorrespond->dwClientAddr;

			//int nCount = 0;
			//const int cbPacketHeadSize = sizeof(CMD_CS_S_SearchAllCorrespond) - sizeof(tagUserRemoteInfo);
			//int cbPacketSize = cbPacketHeadSize;
			////�����û�
			//int nSearchCount = (int)pSearchCorrespond->dwCount;
			//for(int i = 0; i < nSearchCount; i++)
			//{
			//	CGlobalUserItem* pGlobalUserItem =  m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID[i]);
			//	if(pGlobalUserItem != NULL)
			//	{
			//		tagUserInfo* pUserInfo = pGlobalUserItem->GetUserInfo();
			//		//���ҷ���
			//		WORD wServerIndex=0;
			//		CGlobalServerItem * pGlobalServerItem = pGlobalUserItem->EnumServerItem(wServerIndex++);
			//		
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].dwUserID = pUserInfo->dwUserID;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].dwGameID = pUserInfo->dwGameID;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].cbMasterOrder = pUserInfo->cbMasterOrder;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].cbMemberOrder = pUserInfo->cbMemberOrder;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].cbGender = pUserInfo->cbGender;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].cbUserStatus = pUserInfo->cbUserStatus;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].wFaceID = pUserInfo->wFaceID;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].wChairID = pUserInfo->wChairID;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].wTableID = pUserInfo->wTableID;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].wLastTableID = pUserInfo->wLastTableID;
			//		lstrcpyn(pAllSearchCorrespond->UserRemoteInfo[nCount].szNickName,pUserInfo->szNickName, LEN_NICKNAME);
			//		if(pGlobalServerItem != NULL)
			//		{
			//			pAllSearchCorrespond->UserRemoteInfo[nCount].wServerID = pGlobalServerItem->GetServerID();
			//			pAllSearchCorrespond->UserRemoteInfo[nCount].wKindID = pGlobalServerItem->GetKindID();
			//			lstrcpyn(pAllSearchCorrespond->UserRemoteInfo[nCount].szGameServer, pGlobalServerItem->m_GameServer.szServerName,LEN_SERVER);
			//		}
			//		nCount+=1;
			//		cbPacketSize += sizeof(tagUserRemoteInfo);
			//	}
			//	if( cbPacketSize > (SOCKET_TCP_PACKET-sizeof(CMD_CS_S_SearchAllCorrespond)) )
			//	{
			//		pAllSearchCorrespond->dwCount = nCount;
			//		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_ALLCORRESPOND, pAllSearchCorrespond, cbPacketSize);
			//		cbPacketSize = cbPacketHeadSize;
			//		nCount = 0;
			//	}
			//}
			//
			//pAllSearchCorrespond->dwCount = nCount;
			//if(nCount == 0) cbPacketSize = sizeof(CMD_CS_S_SearchAllCorrespond);
			//m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_ALLCORRESPOND, pAllSearchCorrespond, cbPacketSize);
			//
			return true;
		}
	}

	return false;
}

//�������
bool CAttemperEngineSink::OnTCPNetworkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SYSTEM_MESSAGE:	//ϵͳ��Ϣ
		{
			//����֪ͨ
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_SYSTEM_MESSAGE,pData,wDataSize,0L);
			return true;
		}
	case SUB_CS_C_PROPERTY_TRUMPET:  //������Ϣ
		{
			//����֪ͨ
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_PROPERTY_TRUMPET,pData,wDataSize,0L);

			return true;
		}
	}

	return false;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//switch (wSubCmdID)
	//{
	//case SUB_CS_C_ADDPARAMETER:		//��Ӳ���
	//	{
	//		//����У��
	//		//ASSERT(sizeof(CMD_CS_C_AddParameter)==wDataSize);
	//		if(sizeof(CMD_CS_C_AddParameter)!=wDataSize) return false;

	//		//��ȡ����
	//		CMD_CS_C_AddParameter * pAddParameter = (CMD_CS_C_AddParameter *)pData;

	//		//����ṹ
	//		CMD_CS_S_AddParameter AddParameter;
	//		CopyMemory(&AddParameter.AndroidParameter,&pAddParameter->AndroidParameter,sizeof(tagAndroidParameter));

	//		//������Ϣ
	//		SendDataToGame(pAddParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_ADDPARAMETER,&AddParameter,sizeof(AddParameter));

	//		return true;
	//	}
	//case SUB_CS_C_MODIFYPARAMETER:  //�޸Ĳ���
	//	{
	//		//����У��
	//		//ASSERT(sizeof(CMD_CS_C_ModifyParameter)==wDataSize);
	//		if(sizeof(CMD_CS_C_ModifyParameter)!=wDataSize) return false;

	//		//��ȡ����
	//		CMD_CS_C_ModifyParameter * pModifyParameter = (CMD_CS_C_ModifyParameter *)pData;

	//		//����ṹ
	//		CMD_CS_S_ModifyParameter ModifyParameter;
	//		CopyMemory(&ModifyParameter.AndroidParameter,&pModifyParameter->AndroidParameter,sizeof(tagAndroidParameter));

	//		//����֪ͨ
	//		SendDataToGame(pModifyParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_MODIFYPARAMETER,&ModifyParameter,sizeof(ModifyParameter));

	//		return true;
	//	}
	//case SUB_CS_C_DELETEPARAMETER:  //ɾ������
	//	{
	//		//����У��
	//		//ASSERT(sizeof(CMD_CS_C_DeleteParameter)==wDataSize);
	//		if(sizeof(CMD_CS_C_DeleteParameter)!=wDataSize) return false;

	//		//��ȡ����
	//		CMD_CS_C_DeleteParameter * pDeleteParameter = (CMD_CS_C_DeleteParameter *)pData;

	//		//����ṹ
	//		CMD_CS_S_DeleteParameter DeleteParameter;
	//		DeleteParameter.dwBatchID=pDeleteParameter->dwBatchID;

	//		//����֪ͨ
	//		SendDataToGame(pDeleteParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_DELETEPARAMETER,&DeleteParameter,sizeof(DeleteParameter));

	//		return true;
	//	}
	//}

	return false;
}


//�����б�
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	////��������
	//WORD wPacketSize=0L;
	//POSITION Position=NULL;
	//BYTE cbBuffer[SOCKET_TCP_PACKET];

	////������Ϣ
	//m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INFO);

	////�ռ�����
	//do
	//{
	//	//��������
	//	if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
	//	{
	//		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);
	//		wPacketSize=0;
	//	}

	//	//��ȡ����
	//	tagGameServer * pGameServer=(tagGameServer *)(cbBuffer+wPacketSize);
	//	CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

	//	//��������
	//	if (pGlobalServerItem!=NULL)
	//	{
	//		wPacketSize+=sizeof(tagGameServer);
	//		CopyMemory(pGameServer,&pGlobalServerItem->m_GameServer,sizeof(tagGameServer));
	//	}

	//} while (Position!=NULL);

	////��������
	//if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);

	////�������
	//m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_FINISH);

	return true;
}
//�����б�
bool CAttemperEngineSink::SendMatchListItem(DWORD dwSocketID)
{
	////��������
	//WORD wPacketSize=0L;
	//POSITION Position=NULL;
	//BYTE cbBuffer[SOCKET_TCP_PACKET];

	////�ռ�����
	//do
	//{
	//	//��������
	//	if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
	//	{
	//		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);
	//		wPacketSize=0;
	//	}

	//	//��ȡ����
	//	tagGameMatch * pGameMatch=(tagGameMatch *)(cbBuffer+wPacketSize);
	//	CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

	//	//��������
	//	if (pGlobalServerItem!=NULL && pGlobalServerItem->IsMatchServer())
	//	{
	//		wPacketSize+=sizeof(tagGameMatch);
	//		CopyMemory(pGameMatch,&pGlobalServerItem->m_GameMatch,sizeof(tagGameMatch));
	//	}

	//} while (Position!=NULL);

	////��������
	//if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);

	return true;
}

//���䷢��
bool CAttemperEngineSink::SendDataToGame(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//���ҷ���
	CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);
	if (pGlobalServerItem==NULL) return false;

	//��ȡ����
	WORD wBindIndex=pGlobalServerItem->GetIndex();
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//��������
	DWORD dwSocketID=pBindParameter->dwSocketID;
	m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//�û�����
bool CAttemperEngineSink::SendDataToUser(WORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//�����˷���
bool CAttemperEngineSink::OnTCPNetworkMainClubService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_QUERY_CLUB_ROOM_LIST:			//��ѯ�����˷���
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_C_QueryClubRoom));
			if (wDataSize != sizeof(CMD_CS_C_QueryClubRoom))  return false;

			//��ȡ����
			CMD_CS_C_QueryClubRoom *pQueryClubRoom = (CMD_CS_C_QueryClubRoom *)pData;

			//��ѯ����
			CRoomIDArray * pRoomIDArray = m_GlobalInfoManager.GetGroupRoom(pQueryClubRoom->dwGroupID);
			if (pRoomIDArray != NULL)
			{
				//��������
				BYTE *pSendData = new BYTE[GP_SOCKET_PACKET];
				ZeroMemory(pSendData, sizeof(BYTE)*GP_SOCKET_PACKET);

				//����ṹ
				CMD_CS_S_QueryResult *pQueryResult = (CMD_CS_S_QueryResult*)pSendData;

				//���ñ���
				pQueryResult->dwSocketID = pQueryClubRoom->dwSocketID;
				pQueryResult->dwGroupID = pQueryClubRoom->dwGroupID;
				WORD wPacketSize = sizeof(CMD_CS_S_QueryResult);

				tagClubTableInfo *pPersonalTableInfo = (tagClubTableInfo *)(pQueryResult + 1);
				//��������
				for (WORD i = 0; i < pRoomIDArray->GetCount(); i++)
				{
					//�����ж�
					if (wPacketSize + sizeof(tagClubTableInfo)>GP_SOCKET_PACKET)
					{
						//��������
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_CLUB_ROOM_LIST_RESULT, pSendData, wPacketSize);
						wPacketSize = sizeof(CMD_CS_S_QueryResult);
						pQueryResult->wTableCount = 0;

					}

					//��������
					tagPersonalTableInfo *pTableInfo = m_GlobalInfoManager.SearchTableByID(pRoomIDArray->GetAt(i));
					if (pTableInfo == NULL) continue;

					//���˷�ͬ��̨��
					if (pTableInfo->dwGroupID != pQueryClubRoom->dwGroupID)
					{
						continue;
					}

					pPersonalTableInfo->dwServerID = pTableInfo->dwServerID;
					pPersonalTableInfo->dwKindID = pTableInfo->dwKindID;
					pPersonalTableInfo->wTableID = (WORD)pTableInfo->dwTableID;
					pPersonalTableInfo->dwUserID = pTableInfo->dwUserID;
					pPersonalTableInfo->dwDrawCountLimit = pTableInfo->dwDrawCountLimit;
					pPersonalTableInfo->dwDrawTimeLimit = pTableInfo->dwDrawTimeLimit;
					pPersonalTableInfo->lCellScore = pTableInfo->lCellScore;
					pPersonalTableInfo->dwUserID = pTableInfo->dwUserID;
					pPersonalTableInfo->dwPersonalRoomID = pTableInfo->dwPersonalRoomID;
					pPersonalTableInfo->wJoinGamePeopleCount = pTableInfo->wJoinGamePeopleCount;
					pPersonalTableInfo->wCurSitPlayer = pTableInfo->wCurSitPlayer;
					pPersonalTableInfo->cbPlayMode = pTableInfo->cbPlayMode;
					pPersonalTableInfo->lEnterScoreLimit = pTableInfo->lEnterScoreLimit;
					pPersonalTableInfo->lLeaveScoreLimit = pTableInfo->lLeaveScoreLimit;
					pPersonalTableInfo->cbJoinMode = pTableInfo->cbJoinMode;
					pPersonalTableInfo->dwGroupID = pTableInfo->dwGroupID;
					pPersonalTableInfo->bGameStart = pTableInfo->bGameStart;
					pPersonalTableInfo->cbPayMode = pTableInfo->cbPayMode;
					pPersonalTableInfo->dwConfigID = pTableInfo->dwConfigID;
					CopyMemory(pPersonalTableInfo->szNickName, pTableInfo->szNickName, sizeof(pPersonalTableInfo->szNickName));
					CopyMemory(pPersonalTableInfo->szCreateTime, pTableInfo->szCreateTime, sizeof(pPersonalTableInfo->szCreateTime));
					CopyMemory(pPersonalTableInfo->cbGameRule, pTableInfo->cbGameRule, sizeof(pPersonalTableInfo->cbGameRule));
					CopyMemory(pPersonalTableInfo->dwPlayerID, pTableInfo->dwPlayerUserID, sizeof(pPersonalTableInfo->dwPlayerID));
					CopyMemory(pPersonalTableInfo->szConfigName, pTableInfo->szConfigName, sizeof(pPersonalTableInfo->szConfigName));
					pPersonalTableInfo->lMaxChairGameMinChip = pTableInfo->lMaxChairGameMinChip;
					pPersonalTableInfo->dwBankerUserID = pTableInfo->dwBankerUserID;
					wPacketSize += sizeof(tagClubTableInfo);
					pQueryResult->wTableCount++;
					pPersonalTableInfo++;
				}

				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_CLUB_ROOM_LIST_RESULT, pSendData, wPacketSize);

				//�ͷ�����
				delete[] pSendData;
			}

			//��ѯ���
			CMD_CS_S_QueryFinish QueryFinish;
			QueryFinish.dwSocketID = pQueryClubRoom->dwSocketID;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_CLUB_ROOM_LIST_FINISH, &QueryFinish, sizeof(CMD_CS_S_QueryFinish));
			return true;
		}
	case SUB_CS_C_UPDATE_GROUP_SCORE:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_S_UpdateClubScore));
			if (wDataSize != sizeof(CMD_CS_S_UpdateClubScore))  return false;

			//��ȡ����
			CMD_CS_S_UpdateClubScore *pQueryClubRoom = (CMD_CS_S_UpdateClubScore *)pData;

			CGlobalServerItem* p = m_GlobalInfoManager.SearchServerItem(pQueryClubRoom->dwServerID);
			if (p)
				m_pITCPNetworkEngine->SendData(m_pBindParameter[p->m_wIndex].dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_UPDATE_GROUP_SCORE, pData, wDataSize);
			return true;
		}
	case SUB_CS_C_CLUB_DISMISS_TABLE:
	{
		//У������
		//ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTable));
		if (wDataSize != sizeof(CMD_CS_C_DismissTable)) return false;

		CMD_CS_C_DismissTable* pDismissTable = (CMD_CS_C_DismissTable*)pData;

		CGlobalServerItem* p = m_GlobalInfoManager.SearchServerItem(pDismissTable->dwServerID);
		if (p)
		{
			CMD_GR_HostDissumeGame pack;
			pack.dwTableID = pDismissTable->dwTableID;
			pack.dwUserID = 0;
			m_pITCPNetworkEngine->SendData(m_pBindParameter[p->m_wIndex].dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &pack, sizeof(pack));
		}

		return true;
	}
	case SUB_CS_C_QUERY_ROOM_BATTLE_INFO:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(CMD_CS_C_QueryTableInfo));
		if (wDataSize != sizeof(CMD_CS_C_QueryTableInfo))  return false;

		//��ȡ����
		CMD_CS_C_QueryTableInfo *pQueryClubRoom = (CMD_CS_C_QueryTableInfo *)pData;

		tagPersonalTableInfo *pTableInfo = m_GlobalInfoManager.SearchTableByTableIDAndServerID(pQueryClubRoom->dwServerID,pQueryClubRoom->dwTableID);
		if (pTableInfo == NULL) return false;

		CGlobalServerItem* p = m_GlobalInfoManager.SearchServerItem(pTableInfo->dwServerID);
		if (p)
			m_pITCPNetworkEngine->SendData(m_pBindParameter[p->m_wIndex].dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_ROOM_BATTLE_INFO, pData, wDataSize);
		return true;
	}
	case SUB_CS_C_CLUB_DELETE_CONFIG:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(CMD_CS_C_ClubDeleteConfig));
		if (wDataSize != sizeof(CMD_CS_C_ClubDeleteConfig))
		{
			return false;
		}

		//��ȡ����
		CMD_CS_C_ClubDeleteConfig *pClubDeleteConfig = (CMD_CS_C_ClubDeleteConfig *)pData;

		//ת������
		if (m_wClubBindIndex != INVALID_WORD)
		{
			tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
			m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DELETE_CONFIG, (void*)pClubDeleteConfig, wDataSize);
		}

		return true;
	}
	case SUB_CS_C_CREATE_GROUP_SUCCESS:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(CMD_CS_C_CreateGroupSuccess));
		if (wDataSize != sizeof(CMD_CS_C_CreateGroupSuccess))
		{
			return false;
		}

		//��ȡ����
		CMD_CS_C_CreateGroupSuccess *pCreateGroupSuccess = (CMD_CS_C_CreateGroupSuccess *)pData;

		POSITION Position = NULL;

		do
		{
			//��ȡ����
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.EnumServerItem(Position);

			//��������
			if (pGlobalServerItem != NULL && pGlobalServerItem->m_GameServer.cbPlayMode == 1
			&& pGlobalServerItem->m_GameServer.wServerType == GAME_GENRE_PERSONAL)
			{
				//��ȡ����
				WORD wBindIndex = pGlobalServerItem->GetIndex();
				tagBindParameter * pBindParameter = (m_pBindParameter + wBindIndex);

				//��������
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CREATE_GROUP_SUCCESS, pData, wDataSize);
			}
		} while (Position != NULL);

		return true;
	}

	}
	return true;
}
	//////////////////////////////////////////////////////////////////////////////////