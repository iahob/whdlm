#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////
//��ʱ������

#define IDI_LOAD_PLATFORM_PARAMETER		1							//���ز���
#define TIME_LOAD_PLATFORM_PARAMETER	600000						//ʱ����

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_wCollectItem=INVALID_WORD;
	m_wWebBindIndex = INVALID_WORD;

	//���ñ���
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

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
	switch (Command.wMainCmdID)
	{
	case MDM_CS_REGISTER:		//����ע��
		{
			return OnTCPNetworkMainRegister(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_SERVICE_INFO:	//������Ϣ
		{
			return OnTCPNetworkMainServiceInfo(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_USER_COLLECT:	//�û�����
		{
			return OnTCPNetworkMainUserCollect(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_REMOTE_SERVICE:	//Զ�̷���
		{
			return OnTCPNetworkMainRemoteService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_MANAGER_SERVICE: //�������
		{
			return OnTCPNetworkMainManagerService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_ANDROID_SERVICE: //��������
		{
			return OnTCPNetworkMainAndroidService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_CLUB_SERVICE_INFO://�����˷�����Ϣ
		{
			return OnTCPNetworkMainClubService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case MDM_CS_WEB_SERVICE://��վ��Ϣ
		{
			return OnTCPNetworkMainWebService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	CString strWebBindIndex;
	strWebBindIndex.Format(TEXT("OnTCPNetworkMainWebService��վ�������󱣴�����,m_wWebBindIndex=%d"), m_wWebBindIndex);
	CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);

	if (m_wWebBindIndex == LOWORD(dwSocketID))
	{
		CString strWebBindIndex;
		strWebBindIndex.Format(TEXT("OnEventTCPNetworkShut��վ�����Ͽ����ӣ���ǰ����,m_wWebBindIndex=%d"), m_wWebBindIndex);
		CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);
	}

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

	if (pBindParameter->ServiceKind == ServiceKind_Club)
		m_ClubServer.dwSocketID = 0;
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
	case SUB_CS_C_REGISTER_PLAZA:	//ע��㳡
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterPlaza));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterPlaza)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterPlaza * pRegisterPlaza=(CMD_CS_C_RegisterPlaza *)pData;

			//��Ч�ж�
			if ((pRegisterPlaza->szServerName[0]==0)||(pRegisterPlaza->szServerAddr[0]==0))
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("������ע��ʧ�ܣ��������ַ���롰�������������Ϸ���"),CountArray(RegisterFailure.szDescribeString));

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
			(m_pBindParameter+wBindIndex)->wServiceID=wBindIndex;
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Plaza;

			//��������
			tagGamePlaza GamePlaza;
			ZeroMemory(&GamePlaza,sizeof(GamePlaza));

			//��������
			GamePlaza.wPlazaID=wBindIndex;
			lstrcpyn(GamePlaza.szServerName,pRegisterPlaza->szServerName,CountArray(GamePlaza.szServerName));
			lstrcpyn(GamePlaza.szServerAddr,pRegisterPlaza->szServerAddr,CountArray(GamePlaza.szServerAddr));

			//ע�᷿��
			m_GlobalInfoManager.ActivePlazaItem(wBindIndex,GamePlaza);

			//�����б�
			SendServerListItem(dwSocketID);

			SendMatchListItem(dwSocketID);

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			return true;
		}
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
			GameServer.dwSurportType = pRegisterServer->cbPlayMode;
			GameServer.bGroupPersonalMaxChair = pRegisterServer->bGroupPersonalMaxChair;

			//ע�᷿��
			m_GlobalInfoManager.ActiveServerItem(wBindIndex,GameServer);

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
	case SUB_CS_C_REGISTER_MATCH:	//ע�����
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterMatch));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterMatch)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterMatch * pRegisterMatch=(CMD_CS_C_RegisterMatch *)pData;

			//���ҷ���
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.SearchServerItem(pRegisterMatch->wServerID);
			if(pGlobalServerItem==NULL) return true;

			//��������
			tagGameMatch GameMatch;
			ZeroMemory(&GameMatch,sizeof(GameMatch));

			//��������
			GameMatch.wServerID = pRegisterMatch->wServerID;
			GameMatch.dwMatchID = pRegisterMatch->dwMatchID;
			GameMatch.cbMatchType = pRegisterMatch->cbMatchType;
			lstrcpyn(GameMatch.szMatchName,pRegisterMatch->szMatchName,CountArray(GameMatch.szMatchName));

			GameMatch.cbFeeType = pRegisterMatch->cbFeeType;
			GameMatch.cbDeductArea = pRegisterMatch->cbDeductArea;
			GameMatch.lSignupFee = pRegisterMatch->lSignupFee;
			GameMatch.cbSignupMode = pRegisterMatch->cbSignupMode;
			GameMatch.cbJoinCondition = pRegisterMatch->cbJoinCondition;
			GameMatch.cbMemberOrder = pRegisterMatch->cbMemberOrder;
			GameMatch.lExperience = pRegisterMatch->dwExperience;

			GameMatch.cbRankingMode = pRegisterMatch->cbRankingMode;
			GameMatch.wCountInnings = pRegisterMatch->wCountInnings;
			GameMatch.cbFilterGradesMode = pRegisterMatch->cbFilterGradesMode;

			CopyMemory(&GameMatch.cbMatchRule,&pRegisterMatch->cbMatchRule,sizeof(GameMatch.cbMatchRule));

			GameMatch.wRewardCount = pRegisterMatch->wRewardCount;
			CopyMemory(&GameMatch.MatchRewardInfo,pRegisterMatch->MatchRewardInfo,sizeof(GameMatch.MatchRewardInfo));

			//��������
			CopyMemory(&pGlobalServerItem->m_GameMatch,&GameMatch,sizeof(pGlobalServerItem->m_GameMatch));

			//Ⱥ������
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,&GameMatch,sizeof(GameMatch),0L);

			return true;
		}
	case SUB_CS_C_REGISTER_CHAT:	//ע������
		{		
			//���ð�
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceID=wBindIndex;
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Chat;

			tagChatServer ChatServer ;
			ChatServer.wChatID = wBindIndex;
			ChatServer.dwClientAddr = (m_pBindParameter+wBindIndex)->dwClientAddr;
			ChatServer.dwSocketID = dwSocketID;

			m_GlobalInfoManager.ActiveChatItem(wBindIndex,ChatServer);

			//Ⱥ������
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_CHAT_INSERT,&ChatServer,sizeof(ChatServer),0L);

			//������Ϣ
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_CHAT_INSERT);

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			return true;
		}
	case SUB_CS_C_REGISTER_GROUP:	//ע�������
		{
			//���ð�
			WORD wBindIndex = LOWORD(dwSocketID);
			(m_pBindParameter + wBindIndex)->wServiceID = wBindIndex;
			(m_pBindParameter + wBindIndex)->ServiceKind = ServiceKind_Club;

			m_ClubServer.wClubID = wBindIndex;
			m_ClubServer.dwClientAddr = (m_pBindParameter + wBindIndex)->dwClientAddr;
			m_ClubServer.dwSocketID = dwSocketID;

			//������Ϣ
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS);

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID, true, 0L);

			//�����б�
			SendServerListItem(dwSocketID);

			return true;
		}
	case SUB_CS_C_REGISTER_AI:	//ע�������
		{
			m_BindAI.dwSocketID = dwSocketID;

			//������Ϣ
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS);

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
	case SUB_CS_S_SIGNUP_COUNT:
	case SUB_CS_S_MATCH_OPTION:		//��������
	{
		//��������
		POSITION posChat = NULL;

		//���ҷ���
		do
		{
			//���ҷ���
			CGlobalChatItem *pGlobalChatItem = m_GlobalInfoManager.EnumChatItem(posChat);

			//��ֹ�ж�
			if (pGlobalChatItem == NULL)
			{
				break;
			}

			//����״̬
			m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID, MDM_CS_SERVICE_INFO, wSubCmdID, pData, wDataSize);

		} while (posChat != NULL);


		return true;
	}
	case SUB_CS_C_SERVER_ONLINE:	//��������
		{
			//Ч������
			//ASSERT(wDataSize==sizeof(CMD_CS_C_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_C_ServerOnLine)) return false;

			//��Ϣ����
			CMD_CS_C_ServerOnLine * pServerOnLine=(CMD_CS_C_ServerOnLine *)pData;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			//ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//���ҷ���
			WORD wServerID=pBindParameter->wServiceID;
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);

			//��������
			if (pGlobalServerItem!=NULL)
			{
				//��������
				CMD_CS_S_ServerOnLine ServerOnLine;
				ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

				//���ñ���
				pGlobalServerItem->m_GameServer.dwAndroidCount=pServerOnLine->dwAndroidCount;
				pGlobalServerItem->m_GameServer.dwOnLineCount=pServerOnLine->dwOnLineCount;
				pGlobalServerItem->m_GameServer.dwSetPlayerCount = pServerOnLine->dwSetCount;
				//���ñ���
				ServerOnLine.wServerID=wServerID;
				ServerOnLine.dwOnLineCount=pServerOnLine->dwOnLineCount;
				ServerOnLine.dwAndroidCount=pServerOnLine->dwAndroidCount;
				ServerOnLine.dwSetCount = pServerOnLine->dwSetCount;
				//����֪ͨ
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_ONLINE,&ServerOnLine,sizeof(ServerOnLine),0L);
			}

			return true;
		}
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
	case SUB_CS_C_SHARE_STORAGE:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_C_ShareStorage));
			if (wDataSize != sizeof(CMD_CS_C_ShareStorage))
			{
				return false;
			}

			//��������
			CMD_CS_C_ShareStorage *pShareStorage = (CMD_CS_C_ShareStorage*)pData;

			if (pShareStorage->emShareConfigStatus == _ADD_SUBITEM_ || pShareStorage->emShareConfigStatus == _MODIFY_SUBITEM_
				|| pShareStorage->emShareConfigStatus == _REMOVE_SUBITEM_)
			{
				CMD_CS_S_ShareStorage ShareStorage;
				ZeroMemory(&ShareStorage, sizeof(ShareStorage));

				ShareStorage.emShareConfigStatus = pShareStorage->emShareConfigStatus;
				CopyMemory(&(ShareStorage.ShareStorageItem), &(pShareStorage->ShareStorageItem), sizeof(ShareStorage.ShareStorageItem));

				//Ⱥ������
				SendDataToGame(INVALID_SERVER, MDM_CS_SERVICE_INFO, SUB_CS_S_SHARE_STORAGE, &ShareStorage, sizeof(ShareStorage));

				//֪ͨ��վˢ��
				if (m_wWebBindIndex != INVALID_WORD)
				{
					CMD_CS_S_WebShareStorageResult WebShareStorageResult;
					ZeroMemory(&WebShareStorageResult, sizeof(WebShareStorageResult));

					WebShareStorageResult.bGameServerRegister = true;
					WebShareStorageResult.emShareConfigStatus = _SERVERCUR_SUBITEM_;
					CopyMemory(&(WebShareStorageResult.ShareStorageItem), &(pShareStorage->ShareStorageItem), sizeof(WebShareStorageResult.ShareStorageItem));

					tagBindParameter BindParameter = m_pBindParameter[m_wWebBindIndex];
					m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_WEB_SHARE_STORAGE_RESULT, &WebShareStorageResult, sizeof(WebShareStorageResult));

					CString strWebBindIndex;
					strWebBindIndex.Format(TEXT("SUB_CS_S_WEB_SHARE_STORAGE_RESULTC ������վ�������ǰ��վ����,m_wWebBindIndex=%d"), m_wWebBindIndex);
					CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);
				}
			}

			return true;
		}
	case SUB_CS_C_GAMESERVER_WEB_SHARE_STORAGE:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_C_GameServerWebShareStorage));
			if (wDataSize != sizeof(CMD_CS_C_GameServerWebShareStorage))
			{
				return false;
			}

			//��������
			CMD_CS_C_GameServerWebShareStorage *pGameServerWebShareStorage = (CMD_CS_C_GameServerWebShareStorage*)pData;

			CString strDebug;
			strDebug.Format(TEXT("SUB_CS_S_WEB_SHARE_STORAGE_RESULT_C_%d"), m_wWebBindIndex);
			//CTraceService::TraceString(strDebug, TraceLevel_Debug);

			{
				CString strWebBindIndex;
				strWebBindIndex.Format(TEXT("SUB_CS_C_GAMESERVER_WEB_SHARE_STORAGE ��Ϸ����������Э������ǰ��վ����,m_wWebBindIndex=%d"), m_wWebBindIndex);
				CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);
			}

			if (m_wWebBindIndex != INVALID_WORD)
			{
				CMD_CS_S_WebShareStorageResult WebShareStorageResult;
				ZeroMemory(&WebShareStorageResult, sizeof(WebShareStorageResult));

				WebShareStorageResult.bGameServerRegister = true;
				WebShareStorageResult.emShareConfigStatus = pGameServerWebShareStorage->emShareConfigStatus;
				CopyMemory(&(WebShareStorageResult.ShareStorageItem), &(pGameServerWebShareStorage->ShareStorageItem), sizeof(WebShareStorageResult.ShareStorageItem));

				//������ʾ
				if (pGameServerWebShareStorage->emShareConfigStatus == _CLIENTQUERY_SUBITEM_RES_SUCCEED_)
				{
					TCHAR szQuerySubItem[32] = { TEXT("��ѯ����ɹ�") };
					CopyMemory(WebShareStorageResult.szResultDescripe, szQuerySubItem, sizeof(szQuerySubItem));
				}
				else if (pGameServerWebShareStorage->emShareConfigStatus == _MODIFY_SUBITEM_RES_SUCCEED_)
				{
					TCHAR szModifySubItem[32] = { TEXT("�޸�����ɹ�") };
					CopyMemory(WebShareStorageResult.szResultDescripe, szModifySubItem, sizeof(szModifySubItem));
				}
				else if (pGameServerWebShareStorage->emShareConfigStatus == _MODIFY_SUBITEM_RES_FAILED_)
				{
					CopyMemory(WebShareStorageResult.szResultDescripe, pGameServerWebShareStorage->szResultDescripe, sizeof(WebShareStorageResult.szResultDescripe));
				}

				tagBindParameter BindParameter = m_pBindParameter[m_wWebBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_WEB_SHARE_STORAGE_RESULT, &WebShareStorageResult, sizeof(WebShareStorageResult));

				CString strWebBindIndex;
				strWebBindIndex.Format(TEXT("SUB_CS_S_WEB_SHARE_STORAGE_RESULT ������վ�������ǰ��վ����,m_wWebBindIndex=%d"), m_wWebBindIndex);
				CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);
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
				pGlobalUserItem->UpdateStatus(pUserStatus->wTableID, pUserStatus->wChairID, pUserStatus->cbUserStatus);

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
			//Ч�����
			//ASSERT(wDataSize==sizeof(CMD_CS_C_SearchCorrespond));
			if (wDataSize!=sizeof(CMD_CS_C_SearchCorrespond)) return false;

			//������Ϣ
			CMD_CS_C_SearchCorrespond * pSearchCorrespond=(CMD_CS_C_SearchCorrespond *)pData;
			pSearchCorrespond->szNickName[CountArray(pSearchCorrespond->szNickName)-1]=0;

			//��������
			CMD_CS_S_SearchCorrespond SearchCorrespond;
			ZeroMemory(&SearchCorrespond,sizeof(SearchCorrespond));

			//����û�
			CGlobalUserItem * ResultUserItem[2];
			ZeroMemory(ResultUserItem,sizeof(ResultUserItem));

			//���ñ���
			SearchCorrespond.dwSocketID=pSearchCorrespond->dwSocketID;
			SearchCorrespond.dwClientAddr=pSearchCorrespond->dwClientAddr;

			//�����û�
			if (pSearchCorrespond->dwGameID!=0L)
			{
				ResultUserItem[0]=m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID);
			}

			//�����û�
			if (pSearchCorrespond->szNickName[0]!=0)
			{
				_wcslwr_s(pSearchCorrespond->szNickName);
				ResultUserItem[1]=m_GlobalInfoManager.SearchUserItemByNickName(pSearchCorrespond->szNickName);
			}

			//���ý��
			for (BYTE i=0;i<CountArray(ResultUserItem);i++)
			{
				if (ResultUserItem[i]!=NULL)
				{
					//��������
					WORD wServerIndex=0;

					//���ҷ���
					do
					{
						//���ҷ���
						CGlobalServerItem * pGlobalServerItem=ResultUserItem[i]->EnumServerItem(wServerIndex++);

						//��ֹ�ж�
						if (pGlobalServerItem==NULL) break;
						if (SearchCorrespond.wUserCount>=CountArray(SearchCorrespond.UserRemoteInfo)) break;

						//��������
						WORD wIndex=SearchCorrespond.wUserCount++;

						//������Ϣ
						SearchCorrespond.UserRemoteInfo[wIndex].cbGender=ResultUserItem[i]->GetGender();
						SearchCorrespond.UserRemoteInfo[wIndex].cbMemberOrder=ResultUserItem[i]->GetMemberOrder();
						SearchCorrespond.UserRemoteInfo[wIndex].cbMasterOrder=ResultUserItem[i]->GetMasterOrder();

						//�û���Ϣ
						SearchCorrespond.UserRemoteInfo[wIndex].dwUserID=ResultUserItem[i]->GetUserID();
						SearchCorrespond.UserRemoteInfo[wIndex].dwGameID=ResultUserItem[i]->GetGameID();
						lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szNickName,ResultUserItem[i]->GetNickName(),LEN_NICKNAME);

						//������Ϣ
						SearchCorrespond.UserRemoteInfo[wIndex].wKindID=pGlobalServerItem->GetKindID();
						SearchCorrespond.UserRemoteInfo[wIndex].wServerID=pGlobalServerItem->GetServerID();
						lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szGameServer,pGlobalServerItem->m_GameServer.szServerName,LEN_SERVER);

						tagUserInfo* pUserInfo = ResultUserItem[i]->GetUserInfo();
						SearchCorrespond.UserRemoteInfo[wIndex].wFaceID = pUserInfo->wFaceID;
						SearchCorrespond.UserRemoteInfo[wIndex].wChairID = pUserInfo->wChairID;
						SearchCorrespond.UserRemoteInfo[wIndex].cbUserStatus = pUserInfo->cbUserStatus;
						SearchCorrespond.UserRemoteInfo[wIndex].wTableID = pUserInfo->wTableID;
						SearchCorrespond.UserRemoteInfo[wIndex].wLastTableID = pUserInfo->wLastTableID;
						

					} while (true);
				}
			}

			//��������
			WORD wHeadSize=sizeof(SearchCorrespond)-sizeof(SearchCorrespond.UserRemoteInfo);
			WORD wItemSize=sizeof(SearchCorrespond.UserRemoteInfo[0])*SearchCorrespond.wUserCount;
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_CORRESPOND,&SearchCorrespond,wHeadSize+wItemSize);

			return true;
		}
	case SUB_CS_C_SEARCH_ALLCORRESPOND:	//Э������
		{
			CMD_CS_C_AllSearchCorrespond * pSearchCorrespond=(CMD_CS_C_AllSearchCorrespond *)pData;
			if(pSearchCorrespond->dwCount > 512) return false;

			BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
			CMD_CS_S_SearchAllCorrespond* pAllSearchCorrespond = (CMD_CS_S_SearchAllCorrespond*)cbDataBuffer;
			//���ñ���
			pAllSearchCorrespond->dwSocketID=pSearchCorrespond->dwSocketID;
			pAllSearchCorrespond->dwClientAddr=pSearchCorrespond->dwClientAddr;

			int nCount = 0;
			const int cbPacketHeadSize = sizeof(CMD_CS_S_SearchAllCorrespond) - sizeof(tagUserRemoteInfo);
			int cbPacketSize = cbPacketHeadSize;
			//�����û�
			int nSearchCount = (int)pSearchCorrespond->dwCount;
			for(int i = 0; i < nSearchCount; i++)
			{
				CGlobalUserItem* pGlobalUserItem =  m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID[i]);
				if(pGlobalUserItem != NULL)
				{
					tagUserInfo* pUserInfo = pGlobalUserItem->GetUserInfo();
					//���ҷ���
					WORD wServerIndex=0;
					CGlobalServerItem * pGlobalServerItem = pGlobalUserItem->EnumServerItem(wServerIndex++);
					
					pAllSearchCorrespond->UserRemoteInfo[nCount].dwUserID = pUserInfo->dwUserID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].dwGameID = pUserInfo->dwGameID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbMasterOrder = pUserInfo->cbMasterOrder;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbMemberOrder = pUserInfo->cbMemberOrder;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbGender = pUserInfo->cbGender;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbUserStatus = pUserInfo->cbUserStatus;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wFaceID = pUserInfo->wFaceID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wChairID = pUserInfo->wChairID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wTableID = pUserInfo->wTableID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wLastTableID = pUserInfo->wLastTableID;
					lstrcpyn(pAllSearchCorrespond->UserRemoteInfo[nCount].szNickName,pUserInfo->szNickName, LEN_NICKNAME);
					if(pGlobalServerItem != NULL)
					{
						pAllSearchCorrespond->UserRemoteInfo[nCount].wServerID = pGlobalServerItem->GetServerID();
						pAllSearchCorrespond->UserRemoteInfo[nCount].wKindID = pGlobalServerItem->GetKindID();
						lstrcpyn(pAllSearchCorrespond->UserRemoteInfo[nCount].szGameServer, pGlobalServerItem->m_GameServer.szServerName,LEN_SERVER);
					}
					nCount+=1;
					cbPacketSize += sizeof(tagUserRemoteInfo);
				}
				if( cbPacketSize > (SOCKET_TCP_PACKET-sizeof(CMD_CS_S_SearchAllCorrespond)) )
				{
					pAllSearchCorrespond->dwCount = nCount;
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_ALLCORRESPOND, pAllSearchCorrespond, cbPacketSize);
					cbPacketSize = cbPacketHeadSize;
					nCount = 0;
				}
			}
			
			pAllSearchCorrespond->dwCount = nCount;
			if(nCount == 0) cbPacketSize = sizeof(CMD_CS_S_SearchAllCorrespond);
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_ALLCORRESPOND, pAllSearchCorrespond, cbPacketSize);
			
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
	switch (wSubCmdID)
	{
	case SUB_CS_C_ADDPARAMETER:		//��Ӳ���
		{
			//����У��
			//ASSERT(sizeof(CMD_CS_C_AddParameter)==wDataSize);
			if(sizeof(CMD_CS_C_AddParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_C_AddParameter * pAddParameter = (CMD_CS_C_AddParameter *)pData;

			//����ṹ
			CMD_CS_S_AddParameter AddParameter;
			CopyMemory(&AddParameter.AndroidParameter,&pAddParameter->AndroidParameter,sizeof(tagAndroidParameter));

			//������Ϣ
			SendDataToGame(pAddParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_ADDPARAMETER,&AddParameter,sizeof(AddParameter));

			return true;
		}
	case SUB_CS_C_MODIFYPARAMETER:  //�޸Ĳ���
		{
			//����У��
			//ASSERT(sizeof(CMD_CS_C_ModifyParameter)==wDataSize);
			if(sizeof(CMD_CS_C_ModifyParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_C_ModifyParameter * pModifyParameter = (CMD_CS_C_ModifyParameter *)pData;

			//����ṹ
			CMD_CS_S_ModifyParameter ModifyParameter;
			CopyMemory(&ModifyParameter.AndroidParameter,&pModifyParameter->AndroidParameter,sizeof(tagAndroidParameter));

			//����֪ͨ
			SendDataToGame(pModifyParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_MODIFYPARAMETER,&ModifyParameter,sizeof(ModifyParameter));

			return true;
		}
	case SUB_CS_C_DELETEPARAMETER:  //ɾ������
		{
			//����У��
			//ASSERT(sizeof(CMD_CS_C_DeleteParameter)==wDataSize);
			if(sizeof(CMD_CS_C_DeleteParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_C_DeleteParameter * pDeleteParameter = (CMD_CS_C_DeleteParameter *)pData;

			//����ṹ
			CMD_CS_S_DeleteParameter DeleteParameter;
			DeleteParameter.wServerID = pDeleteParameter->wServerID;

			//����֪ͨ
			SendDataToGame(pDeleteParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_DELETEPARAMETER,&DeleteParameter,sizeof(DeleteParameter));

			return true;
		}
	}

	return false;
}



//��������
bool CAttemperEngineSink::OnTCPNetworkMainClubService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_CHECK_NEW_USER:		//��������
	{
		//����У��
		//ASSERT(sizeof(CMD_CS_S_CheckNewUser) == wDataSize);
		if (sizeof(CMD_CS_S_CheckNewUser) != wDataSize) return false;

		//ת����������
		if (m_ClubServer.dwSocketID>0)
			m_pITCPNetworkEngine->SendData(m_ClubServer.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CHECK_NEW_USER,pData,wDataSize);
		return true;
	}

	}

	return false;
}

//�����б�
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	//��������
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INFO);

	//�ռ�����
	do
	{
		//��������
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ����
		tagGameServer * pGameServer=(tagGameServer *)(cbBuffer+wPacketSize);
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

		//��������
		if (pGlobalServerItem!=NULL)
		{
			wPacketSize+=sizeof(tagGameServer);
			CopyMemory(pGameServer,&pGlobalServerItem->m_GameServer,sizeof(tagGameServer));
		}

	} while (Position!=NULL);

	//��������
	if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);

	//�������
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_FINISH);

	return true;
}
//�����б�
bool CAttemperEngineSink::SendMatchListItem(DWORD dwSocketID)
{
	//��������
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//�ռ�����
	do
	{
		//��������
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ����
		tagGameMatch * pGameMatch=(tagGameMatch *)(cbBuffer+wPacketSize);
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

		//��������
		if (pGlobalServerItem!=NULL && pGlobalServerItem->IsMatchServer())
		{
			wPacketSize+=sizeof(tagGameMatch);
			CopyMemory(pGameMatch,&pGlobalServerItem->m_GameMatch,sizeof(tagGameMatch));
		}

	} while (Position!=NULL);

	//��������
	if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);

	return true;
}

//���䷢��
bool CAttemperEngineSink::SendDataToGame(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ָ������
	if (wServerID != INVALID_SERVER)
	{
		//���ҷ���
		CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.SearchServerItem(wServerID);
		if (pGlobalServerItem == NULL) return false;

		//��ȡ����
		WORD wBindIndex = pGlobalServerItem->GetIndex();
		tagBindParameter * pBindParameter = (m_pBindParameter + wBindIndex);

		//��������
		DWORD dwSocketID = pBindParameter->dwSocketID;
		m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);
	}
	//����Ⱥ��
	else
	{
		POSITION Position = NULL;

		do
		{
			//��ȡ����
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.EnumServerItem(Position);

			//Ⱥ��Լս����
			if (pGlobalServerItem != NULL && pGlobalServerItem->m_GameServer.wServerType == GAME_GENRE_PERSONAL && pGlobalServerItem->m_GameServer.cbPlayMode == 1)
			{
				//��ȡ����
				WORD wBindIndex = pGlobalServerItem->GetIndex();
				tagBindParameter * pBindParameter = (m_pBindParameter + wBindIndex);

				//��������
				DWORD dwSocketID = pBindParameter->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);
			}
		} while (Position != NULL);
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::SendDataToUser(WORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}


//��վ����
bool CAttemperEngineSink::OnTCPNetworkMainWebService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	TCHAR szInfo11[260] = { 0 };
	wsprintf(szInfo11, TEXT("ptdtaiwz *** Э�������� �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_PARAMTER wSubCmdID = %d"), wSubCmdID);
	//OutputDebugString(szInfo11);

	//������վ��������
	m_wWebBindIndex = LOWORD(dwSocketID);

	CString strWebBindIndex;
	strWebBindIndex.Format(TEXT("OnTCPNetworkMainWebService��վ�������󱣴�����,m_wWebBindIndex=%d"), m_wWebBindIndex);
	CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);

	switch (wSubCmdID)
	{
		case SUB_CS_C_UPDATE_USER_SCORE:
		{
			CTraceService::TraceString(_T("SUB_CS_C_UPDATE_USER_SCORE 1"), TraceLevel_Debug);
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_C_UpdateUserScore));
			if (wDataSize != sizeof(CMD_CS_C_UpdateUserScore))
			{
				//������Ϣ
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_FAILURE);
				return false;
			}
			CTraceService::TraceString(_T("SUB_CS_C_UPDATE_USER_SCORE 1"), TraceLevel_Debug);
			//��ȡ����
			CMD_CS_C_UpdateUserScore *pUpdateUserScore = (CMD_CS_C_UpdateUserScore*)pData;

			//�����û�
			CGlobalUserItem *pGlobalUserItem = m_GlobalInfoManager.SearchUserItemByUserID(pUpdateUserScore->dwUserID);
			CString str;
			str.Format(_T("SUB_CS_C_UPDATE_USER_SCORE UserID=%d"), pUpdateUserScore->dwUserID);

			CTraceService::TraceString(str.GetBuffer(), TraceLevel_Debug);

			if (pGlobalUserItem)
			{
				CTraceService::TraceString(_T("SUB_CS_C_UPDATE_USER_SCORE 2"), TraceLevel_Debug);
				//ת�����񣨲��� ���͵�һ�����䣩
				CGlobalServerItem* pGlobalServerItem = pGlobalUserItem->EnumServerItem(0);
				if (pGlobalServerItem)
				{
					CTraceService::TraceString(_T("SUB_CS_C_UPDATE_USER_SCORE 3"), TraceLevel_Debug);
					//��ȡSOCKET
					DWORD dwGameScocketID = (m_pBindParameter + pGlobalServerItem->GetIndex())->dwSocketID;
					//������Ϣ
					m_pITCPNetworkEngine->SendData(dwGameScocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_USER_SCORE, pData, wDataSize);

					//������Ϣ
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_SUCCESS);

					return true;
				}
			}
			CTraceService::TraceString(_T("SUB_CS_C_UPDATE_USER_SCORE 4"), TraceLevel_Debug);
			//������Ϣ
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_FAILURE);
			return true;

		}
		case SUB_CS_S_GROUP_MAX_MEMBER:
		{
			//ASSERT(wDataSize == sizeof(CMD_CS_C_GroupMaxMember));
			if (wDataSize != sizeof(CMD_CS_C_GroupMaxMember))
			{
				//������Ϣ
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_FAILURE);
				return false;
			}

			//ת����������
			if (m_ClubServer.dwSocketID > 0)
			{
				m_pITCPNetworkEngine->SendData(m_ClubServer.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_GROUP_MAX_MEMBER, pData, wDataSize);
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_SUCCESS);
				return true;
			}

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_FAILURE);
			return true;
		}
		case SUB_CS_C_WEB_SHARE_STORAGE:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_C_WebShareStorage));
			if (wDataSize != sizeof(CMD_CS_C_WebShareStorage))
			{
				return false;
			}

			//��������
			CMD_CS_C_WebShareStorage *pWebShareStorage = (CMD_CS_C_WebShareStorage*)pData;

			//ע���ʶ
			DWORD dwRegisterSocketID = INVALID_DWORD;
			POSITION Position = NULL;

			do
			{
				//��ȡ����
				CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.EnumServerItem(Position);

				//��������
				if (pGlobalServerItem != NULL && pGlobalServerItem->m_GameServer.wKindID == pWebShareStorage->ShareStorageItem.wKindID
					&& pGlobalServerItem->m_GameServer.cbPlayMode == pWebShareStorage->ShareStorageItem.cbPlayMode
					&& pGlobalServerItem->m_GameServer.wServerType == GAME_GENRE_PERSONAL)
				{
					//��ȡ����
					WORD wBindIndex = pGlobalServerItem->GetIndex();
					tagBindParameter * pBindParameter = (m_pBindParameter + wBindIndex);

					//��������
					dwRegisterSocketID = pBindParameter->dwSocketID;

					break;
				}
			} while (Position != NULL);

			//CString strDebug;
			//strDebug.Format(TEXT("dwRegisterSocketID=%d,wKindID=%d,cbPlayMode=%d,dwGroupID=%d"), dwRegisterSocketID, pWebShareStorage->ShareStorageItem.wKindID, pWebShareStorage->ShareStorageItem.cbPlayMode, pWebShareStorage->ShareStorageItem.dwGroupID);
			//CTraceService::TraceString(strDebug, TraceLevel_Debug);

			if (dwRegisterSocketID == INVALID_DWORD)
			{
				CMD_CS_S_WebShareStorageResult WebShareStorageResult;
				ZeroMemory(&WebShareStorageResult, sizeof(WebShareStorageResult));

				WebShareStorageResult.bGameServerRegister = false;
				WebShareStorageResult.emShareConfigStatus = (pWebShareStorage->emShareConfigStatus == _CLIENTQUERY_SUBITEM_ ? _CLIENTQUERY_SUBITEM_RES_FAILED_ : _MODIFY_SUBITEM_RES_FAILED_);
	
				//������ʾ
				_sntprintf_s(WebShareStorageResult.szResultDescripe, CountArray(WebShareStorageResult.szResultDescripe), TEXT("wKindID=%d,cbPlayMode=%d��Ӧ�ķ���δע��!"),
					pWebShareStorage->ShareStorageItem.wKindID, pWebShareStorage->ShareStorageItem.cbPlayMode);

				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_WEB_SHARE_STORAGE_RESULT, &WebShareStorageResult, sizeof(WebShareStorageResult));

				CTraceService::TraceString(TEXT("SUB_CS_S_WEB_SHARE_STORAGE_RESULT_A"), TraceLevel_Debug);
			}
			else 
			{
				//��������
				CMD_CS_S_WebShareStorage WebShareStorage;
				ZeroMemory(&WebShareStorage, sizeof(WebShareStorage));

				WebShareStorage.emShareConfigStatus = pWebShareStorage->emShareConfigStatus;
				CopyMemory(&(WebShareStorage.ShareStorageItem), &(pWebShareStorage->ShareStorageItem), sizeof(WebShareStorage.ShareStorageItem));

				m_pITCPNetworkEngine->SendData(dwRegisterSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_WEB_SHARE_STORAGE, pData, wDataSize);

				CTraceService::TraceString(TEXT("SUB_CS_S_WEB_SHARE_STORAGE_RESULT_B"), TraceLevel_Debug);
			}

			return true;
		}
		case SUB_CS_C_UPDATE_AI_PARAMTER:
		{
	
			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtaiwz *** Э�������� �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_PARAMTER dwSocketID = %d"),dwSocketID);
			//OutputDebugString(szInfo);

			CMD_CS_C_UpdateAIParameterToAI UpdateAIParameter;

			UpdateAIParameter.dwSocketID = dwSocketID;
			memcpy_s((byte*)(&UpdateAIParameter) + 4, sizeof(UpdateAIParameter)-4, pData, wDataSize);
			m_pITCPNetworkEngine->SendData(m_BindAI.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_C_UPDATE_AI_PARAMTER, &UpdateAIParameter, sizeof(UpdateAIParameter));
			return true;
		}
		case SUB_CS_C_UPDATE_AI_WEIGHT_OUT:
		{
			//OutputDebugString(TEXT("ptdtaiwz *** Э�������� �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_OUT"));
			CMD_CS_C_UpdateAIWeightOut * UpdateAIWeightOut = (CMD_CS_C_UpdateAIWeightOut *)pData;
			if (sizeof(CMD_CS_C_UpdateAIWeightOut) != wDataSize)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtaiwz *** Э�������� �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_OUT wDataSize = %d sizeof(CMD_CS_C_UpdateAIWeightOut) = %d  ���ݽṹ����ȷ"), wDataSize, sizeof(CMD_CS_C_UpdateAIWeightOut));
				//OutputDebugString(szInfo);
				return false;
			}

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwTime = %d dwInRatio = %d"), UpdateAIWeightOut->nIndex, UpdateAIWeightOut->WeightOutRatio.dwTime, UpdateAIWeightOut->WeightOutRatio.dwOutRatio);
			//OutputDebugString(szInfo);

			CMD_CS_C_UpdateAIWeightOutToAI UpdateAIWeightOutToAI;
			UpdateAIWeightOutToAI.dwSocketID = dwSocketID;
			memcpy_s((byte*)(&UpdateAIWeightOutToAI) + 4, sizeof(UpdateAIWeightOutToAI)-4, pData, wDataSize);
			m_pITCPNetworkEngine->SendData(m_BindAI.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_C_UPDATE_AI_WEIGHT_OUT, &UpdateAIWeightOutToAI, sizeof(UpdateAIWeightOutToAI));
			return true;
		}
		case SUB_CS_C_UPDATE_AI_WEIGHT_IN:
		{
			//OutputDebugString(TEXT("ptdtaiwz *** Э�������� �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_IN"));

			CMD_CS_C_UpdateAIWeightIn * UpdateAIWeightOut = (CMD_CS_C_UpdateAIWeightIn *)pData;

			if (sizeof(CMD_CS_C_UpdateAIWeightIn) != wDataSize)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtaiwz *** Э�������� �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_IN dwSocketID = %d  wDataSize = %d sizeof(CMD_CS_C_UpdateAIWeightIn) = %d ���ݽṹ����ȷ"), wDataSize, sizeof(CMD_CS_C_UpdateAIWeightIn));
				//OutputDebugString(szInfo);
				return false;
			}


			
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwTime = %d dwInRatio = %d"), UpdateAIWeightOut->nIndex, UpdateAIWeightOut->WeightInRatio.dwTime, UpdateAIWeightOut->WeightInRatio.dwInRatio);
			//OutputDebugString(szInfo);

			CMD_CS_C_UpdateAIWeightInToAI UpdateAIWeightInToAI;
			UpdateAIWeightInToAI.dwSocketID = dwSocketID;
			memcpy_s((byte*)(&UpdateAIWeightInToAI) + 4, sizeof(UpdateAIWeightInToAI)-4, pData, wDataSize);
			m_pITCPNetworkEngine->SendData(m_BindAI.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_C_UPDATE_AI_WEIGHT_IN, &UpdateAIWeightInToAI, sizeof(UpdateAIWeightInToAI));
			return true;
		}
		case SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL:
		{
			//OutputDebugString(TEXT("ptdtaiwz *** Э�������� �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL"));

			CMD_CS_C_UpdateAITimeTotal * UpdateAIWeightOut = (CMD_CS_C_UpdateAITimeTotal *)pData;

			if (sizeof(CMD_CS_C_UpdateAITimeTotal) != wDataSize)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtaiwz *** Э�������� �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL  wDataSize = %d  sizeof(CMD_CS_C_UpdateAITimeTotal) = %d���ݽṹ����ȷ"), wDataSize, sizeof(CMD_CS_C_UpdateAITimeTotal));
				//OutputDebugString(szInfo);
				return false;
			}


			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwTime = %d dwTimeChange = %d"), UpdateAIWeightOut->nIndex, UpdateAIWeightOut->dwTime, UpdateAIWeightOut->dwTimeChange);
			//OutputDebugString(szInfo);

			CMD_CS_C_UpdateAITimeTotalToAI UpdateAITimeTotalToAI;
			UpdateAITimeTotalToAI.dwSocketID = dwSocketID;
			memcpy_s((byte*)(&UpdateAITimeTotalToAI) + 4, sizeof(UpdateAITimeTotalToAI) - 4, pData, wDataSize);
			m_pITCPNetworkEngine->SendData(m_BindAI.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL, &UpdateAITimeTotalToAI, sizeof(UpdateAITimeTotalToAI));
			return true;
		}
		case SUB_CS_C_DELETE_AI:
		{
			//OutputDebugString(TEXT("ptdtaiwz *** Э�������� �յ���վ����Ϣ SUB_CS_C_DELETE_AI"));

			CMD_CS_C_DeleteAI * pDeleteAI = (CMD_CS_C_DeleteAI *)pData;
			if (pDeleteAI == NULL)
			{
				return true;
			}

			if (sizeof(CMD_CS_C_DeleteAI) != wDataSize)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtaiwz *** Э�������� �յ���վ����Ϣ SUB_CS_C_DELETE_AI  wDataSize = %d  sizeof(CMD_CS_C_UpdateAITimeTotal) = %d���ݽṹ����ȷ"), wDataSize, sizeof(CMD_CS_C_DeleteAI));
				//OutputDebugString(szInfo);
				return false;
			}



			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaiwz *** dwGroupID = %d"), pDeleteAI->dwGroupID);
			//OutputDebugString(szInfo);

			CMD_CS_C_DeleteAIParameterToAI DeleteAIParameterToAI;
			DeleteAIParameterToAI.dwSocketID = dwSocketID;
			memcpy_s((byte*)(&DeleteAIParameterToAI) + 4, sizeof(DeleteAIParameterToAI)-4, pData, wDataSize);
			m_pITCPNetworkEngine->SendData(m_BindAI.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_C_DELETE_AI, &DeleteAIParameterToAI, sizeof(DeleteAIParameterToAI));
			return true;
		}
		case SUB_SW_S_AI_OPERATE_RESULT:
		{
			CMD_WS_S_AIOperateResult * pAIOperateResult = (CMD_WS_S_AIOperateResult *)pData;

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaiwz *** Э�������� �� ��վ���ؽ�� SUB_SW_S_AI_OPERATE_RESULT dwSocketID = %d"), pAIOperateResult->dwSocketID);
			//OutputDebugString(szInfo);

			m_pITCPNetworkEngine->SendData(pAIOperateResult->dwSocketID, MDM_CS_WEB_SERVICE, SUB_SW_S_AI_OPERATE_RESULT, pData, wDataSize);
			return true;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////
