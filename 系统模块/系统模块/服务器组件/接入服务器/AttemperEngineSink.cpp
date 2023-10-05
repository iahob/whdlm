//�ر���IT������28xin.com��
#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"
#include <locale.h>
#include "Dump.h"
//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_CONNECT_CORRESPOND		1									    //������ʶ

//ʱ���ʶ
#define IDI_CONNECT_LOGIC_START		(IDI_CONNECT_CORRESPOND+1)			    //������ʶ
#define IDI_CONNECT_LOGIC_END		(IDI_CONNECT_LOGIC_START+LOGIC_COUNT)	//������ʶ

//ʱ���ʶ
#define IDI_CONNECT_GROUP_START		(IDI_CONNECT_LOGIC_END+1)				//������ʶ
#define IDI_CONNECT_GROUP_END		(IDI_CONNECT_GROUP_START+GROUP_COUNT)   //������ʶ

//ʱ���ʶ
#define IDI_CONNECT_BATTLE_START	(IDI_CONNECT_GROUP_END+1)				//������ʶ
#define IDI_CONNECT_BATTLE_END		(IDI_CONNECT_BATTLE_START+BATTLE_COUNT) //������ʶ

//ʱ���ʶ
#define IDI_CONNECT_LOG_SERVER		(IDI_CONNECT_BATTLE_END + 1)			//��־������ʶ

//����������
#define IDI_CLEAR_MEDAL				(IDI_CONNECT_LOG_SERVER+1)

//AI������ʶ
#define IDI_CONNECT_AI_SERVER		(IDI_CLEAR_MEDAL + 1)

//��������
#define LOGON_INVALID_TIME			10*1000L							//��¼ʧЧʱ��							

#define IDI_LOAD_FORBID_WORDS		100									//���ؽ���
#define IDI_TEST_LOG				101									//������־

LONGLONG g_lManagerRight[30] = { MR_1, MR_2, MR_3, MR_4, MR_5, MR_6, MR_7, MR_8, MR_9, MR_10, MR_11, MR_12, MR_13, MR_14 };
LONGLONG g_lCaptainRight[30] = { CR_1, CR_2, CR_3, CR_4, CR_5, CR_6, CR_7,CR_8 };
//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bNeedBattle=true;

	//״̬����
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_TCPSocketCorrespond=NULL;
	m_pAITCPSocketService = NULL;
	ZeroMemory(&m_BattleItem, sizeof(m_BattleItem));
	m_bNeedConnectBattle = true;
	m_pITCPSocketBattle = NULL;
	m_pLogServerTCPSocketService = NULL;

	m_ServerUserManager.DeleteUserItem();
	m_GroupInfoManager.RemoveGroupItem();
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)WHCrashHandler);

	ZeroMemory(m_PrsAdd, sizeof(m_PrsAdd));
	m_wPrsPort = 0;
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
	m_pBindParameter = new tagBindParameter[m_pInitParameter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter, sizeof(tagBindParameter)*m_pInitParameter->m_wMaxConnect);
	
	//���ؽ���
	m_pITimerEngine->SetTimer(IDI_LOAD_FORBID_WORDS, 1000L, 1, 0);

	//������־
	m_pITimerEngine->SetTimer(IDI_TEST_LOG, 5*1000L, -1, 0);

	tagIMGroupItem IMGroupItem;
	m_GroupInfoManager.ActiveGroupItem(IMGroupItem);
	ZeroMemory(&IMGroupItem, sizeof(IMGroupItem));
	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_bNeedBattle=true;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_TCPSocketCorrespond=NULL;
	m_pAITCPSocketService = NULL;
	m_pLogServerTCPSocketService = NULL;

	//ɾ�����������淨
	POSITION grouppos = NULL;
	do
	{
		//ö�ٴ�����
		CGroupItem *pGroupItem = m_GroupInfoManager.EnumGroupItem(grouppos);
		if (pGroupItem == NULL)
		{
			break;
		}

		for (int nIndex = 0; nIndex < pGroupItem->GetConfigCount(); nIndex++)
		{
			tagGroupConfigInfo *pGroupConfigInfo = pGroupItem->EnumConfig(nIndex);
			if (!pGroupConfigInfo)
			{
				break;
			}

			if (pGroupConfigInfo->cbGroupPersonalMaxChair == TRUE)
			{
				//����ṹ
				DBR_GR_DeleteConfig deleteConfig;
				ZeroMemory(&deleteConfig, sizeof(deleteConfig));

				deleteConfig.dwConfigID = pGroupConfigInfo->dwConfigID;
				deleteConfig.dwGroupID = pGroupConfigInfo->dwGroupID;
				deleteConfig.dwUserID = pGroupItem->GetGroupCreaterID();
				deleteConfig.bGroupPersonalMaxChair = true;
				deleteConfig.bAttemperEngineConclude = true;

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_CONFIG, INVALID_DWORD, &deleteConfig, sizeof(deleteConfig));
			}
		}
	} while (grouppos != NULL);

	//ɾ������
	SafeDeleteArray(m_pBindParameter);	
	m_bNeedConnectBattle = true;
	m_ServerUserManager.DeleteUserItem();
	m_GroupInfoManager.RemoveGroupItem();
	m_WordFilter.ResetFilter();

	//�б����
	m_ServerListManager.ResetServerList();

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_CORRESPOND:		//����Э��
		{
			//��ȡ��ַ					
			tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CorrespondAddress;

			//��������
			m_TCPSocketCorrespond->Connect(pAddressInfo->szAddress, m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ��Խ���Э��������..."),TraceLevel_Normal);

			return true;
		}
	case CT_CONNECT_BATTLE:			//����Լս
		{
			//������֤		
			if (m_bNeedConnectBattle == true)
			{
				//��ȡ��ַ					
				tagAddressInfo * pAddressInfo = &m_pInitParameter->m_PrsnlRmCorrespondAddress;

				m_pITCPSocketBattle->Connect(pAddressInfo->szAddress, m_pInitParameter->m_wPrsnlRmCorrespondPort);

				//��ʾ��Ϣ
				TCHAR szConnectInfo[MAX_PATH] = { 0 };
				_stprintf_s(szConnectInfo, TEXT("��������Լս������[%s:%d]"), pAddressInfo->szAddress, m_pInitParameter->m_wPrsnlRmCorrespondPort);
				CTraceService::TraceString(szConnectInfo, TraceLevel_Normal);
			}			
			return true;
		}
	case CT_LOAD_GROUP_LIST:		//����Ⱥ��
		{
			m_GroupInfoManager.RemoveGroupItem();
			//��������
			DBR_GR_LoadGroupList LoadGroupList;
			ZeroMemory(&LoadGroupList, sizeof(LoadGroupList));

			//����ṹ
			LoadGroupList.dwGroupEndID = GS_GROUP_COUNT;
			LoadGroupList.dwGroupStartID = 1;

			//����Ⱥ��
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_GROUP_LIST, 0, &LoadGroupList, sizeof(LoadGroupList));

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ��Լ���Ⱥ����Ϣ..."), TraceLevel_Normal);
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
			_sntprintf_s(szString, CountArray(szString), TEXT("����������־������ [ %s:%d ]"), pLogServerAddress->szAddress, m_pInitParameter->m_wLogServerPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString, TraceLevel_Normal);
			return true;
		}
	case CT_CONNECT_AI_SERVER:
		{
			//��ȡ��ַ					
			tagAddressInfo * pAddressInfo = &m_pInitParameter->m_AIAddress;

			//��������
			m_pAITCPSocketService->Connect(pAddressInfo->szAddress, m_pInitParameter->m_wAIPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ�������AI������..."), TraceLevel_Normal);
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
	//����Э��
	if (dwTimerID==IDI_CONNECT_CORRESPOND)
	{
		//��ȡ��ַ					
		tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CorrespondAddress;

		//��������
		m_TCPSocketCorrespond->Connect(pAddressInfo->szAddress, m_pInitParameter->m_wCorrespondPort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ��Խ���Э��������..."),TraceLevel_Normal);

		return true;
	}

	//����AI
	if (dwTimerID == IDI_CONNECT_AI_SERVER)
	{
		//��ȡ��ַ					
		tagAddressInfo * pAddressInfo = &m_pInitParameter->m_AIAddress;

		//��������
		m_pAITCPSocketService->Connect(pAddressInfo->szAddress, m_pInitParameter->m_wAIPort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ�������AI������..."), TraceLevel_Normal);

		return true;
	}

	
	
	//����Լս
	if (dwTimerID==IDI_CONNECT_BATTLE_START)
	{
		//������֤		
		if (m_bNeedConnectBattle == true)
		{
			//��ȡ��ַ					
			tagAddressInfo * pAddressInfo = &m_pInitParameter->m_PrsnlRmCorrespondAddress;

			m_pITCPSocketBattle->Connect(pAddressInfo->szAddress, m_pInitParameter->m_wPrsnlRmCorrespondPort);

			//��ʾ��Ϣ
			TCHAR szConnectInfo[MAX_PATH] = { 0 };
			_stprintf_s(szConnectInfo, TEXT("��������Լս������[%s:%d]"), pAddressInfo->szAddress, m_pInitParameter->m_wPrsnlRmCorrespondPort);
			CTraceService::TraceString(szConnectInfo, TraceLevel_Normal);

			CopyMemory(m_PrsAdd, pAddressInfo->szAddress, sizeof(m_PrsAdd));
			m_wPrsPort = m_pInitParameter->m_wPrsnlRmCorrespondPort;
		}

		return true;
	}

	if (IDI_LOAD_FORBID_WORDS==dwTimerID)		//���ؽ���
	{
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_FORBID_WORDS, 0, NULL, 0);

		return true;
	}


	if (IDI_TEST_LOG == dwTimerID)
	{
		//SendLogData(TEXT("�������������"));
	}

	if (IDI_CONNECT_LOG_SERVER == dwTimerID)
	{
		//��������
		tagAddressInfo * pLogServerAddress = &m_pInitParameter->m_LogServerAddress;

		//��������
		m_pLogServerTCPSocketService->Connect(pLogServerAddress->szAddress, m_pInitParameter->m_wLogServerPort);

		return true;
	}

	if (IDI_CLEAR_MEDAL == dwTimerID)
	{
		time_t curTime = time(NULL);
		LONGLONG lOffset = 86400 - difftime(curTime, m_tMedalClearStart);//ʱ�������
		m_pITimerEngine->SetTimer(IDI_CLEAR_MEDAL, (86400 + lOffset) * 1000, 1, 0);

		//��ǰ����ID
		struct tm today = *localtime(&m_tMedalClearStart);
		CString cc;
		cc.Format(_T("%04d%02d%02d"), 1900 + today.tm_year, 1 + today.tm_mon, today.tm_mday);
		DWORD dwDateID = _wtoi(cc.GetString());

		//������ʼʱ��
		m_tMedalClearStart += 86400;

		//ѫ������
		POSITION grouppos = NULL;
		do
		{
			//ö�ٴ�����
			CGroupItem *pGroupItem = m_GroupInfoManager.EnumGroupItem(grouppos);
			if (pGroupItem == NULL) break;

			if (pGroupItem->GetGroupBattleRule()->cbMedalClear == 1)
			{
				tagClubTableInfo TableInfo;
				POSITION tablepos = NULL;
				do
				{
					//����������ѫ������
					TableInfo=pGroupItem->EnumTable(tablepos);
					if (TableInfo.dwServerID != 0 && TableInfo.cbPlayMode == 1)
					{
						//֪ͨԼս��ɢ����
						CMD_CS_C_DismissTable pack;
						ZeroMemory(&pack, sizeof(pack));
						pack.dwServerID = TableInfo.dwServerID;
						pack.dwTableID = TableInfo.wTableID;
						m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &pack, sizeof(CMD_CS_C_DismissTable));
					}

				} while (tablepos != NULL);

				//���ݿ�����ѫ��
				DBR_GR_ClearMedal ClearMedal;
				ClearMedal.dwGroupID = pGroupItem->GetGroupID();
				ClearMedal.dwDateID = dwDateID;
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MEDAL_CLEAR, 0, &ClearMedal, sizeof(DBR_GR_ClearMedal));
			}


		} while (grouppos != NULL);

		
	}

	return false;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//��ȡ����
	//ASSERT(LOWORD(dwSocketID)<m_pInitParameter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParameter->m_wMaxConnect) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���	
	if (pBindParameter != NULL)
	{
		pBindParameter->dwSocketID = dwSocketID;
		pBindParameter->dwClientIP = dwClientIP;
		pBindParameter->dwActiveTime = (DWORD)time(NULL);

		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, _T("ptdt OnEventTCPNetworkBind dwSocketID=%d,wBindIndex=%d"), dwSocketID, wBindIndex);
		//OutputDebugString(szInfo);
		SendLogData(szInfo);
		return true;
	}

	return false;

}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	
	if (wcslen(m_PrsAdd) > 0)
	{
		bool bchange = false;
		if (wcscmp(m_PrsAdd, m_pInitParameter->m_PrsnlRmCorrespondAddress.szAddress) != 0)
			bchange = true;
		if (m_pInitParameter->m_wPrsnlRmCorrespondPort != m_wPrsPort)
			bchange = true;
		if (bchange)
		{
			CTraceService::TraceString(_T("Լս��ַ�쳣�ı�"), TraceLevel_Exception);
			SendLogData(_T("Լս��ַ�쳣�ı�"));
		}
	}


	CString cc;
	cc.Format(_T("�������յ�����main=%d,sub=%d,socket=%d,"),Command.wMainCmdID,Command.wSubCmdID,dwSocketID);
	SendLogData(cc);
	switch (Command.wMainCmdID)
	{
	case MDM_GA_LOGIC_SERVICE:	 //�߼�����
		{
			return OnTCPNetworkMainLogicService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GA_GROUP_SERVICE:	 //Ⱥ�����
		{
			return OnTCPNetworkMainGroupService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GA_BATTLE_SERVICE:	 //Լս����
		{
			return OnTCPNetworkMainBattleService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}		
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	TCHAR szInfo[260] = { 0 };

	//��ȡ����
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//�ж��û�
	if (pBindParameter->dwUserID!=0)
	{
		//�޸�״̬
		CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pBindParameter->dwUserID);
		if (pServerUserItem != NULL)
		{
			SUB_GA_MemberStatus Status;
			Status.dwUserID = pServerUserItem->GetUserInfo()->dwUserID;
			Status.cbStatus = US_GP_ONLINE;

			//�����������״̬
			DBR_GR_UpdateOnLineStatus UpdateOnLineStatus;
			ZeroMemory(&UpdateOnLineStatus, sizeof(UpdateOnLineStatus));

			//���ñ���
			UpdateOnLineStatus.dwGroupID = pServerUserItem->GetActiveGroupID();
			UpdateOnLineStatus.dwUserID = pServerUserItem->GetUserID();
			UpdateOnLineStatus.cbStatus = US_GP_OFFLINE;


			POSITION pos=NULL;
			do
			{
				//ö�ٳ�Ա
				CGroupItem *pGroupItem = m_GroupInfoManager.EnumGroupItem(pos);
				if (pGroupItem == NULL) break;

				if (pGroupItem->SearchMember(pServerUserItem->GetUserID())==NULL) continue;
				pGroupItem->SearchMember(pServerUserItem->GetUserID())->cbStatus = US_GP_OFFLINE;
				SendDataToGroupMember(pGroupItem->GetGroupID(), MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_STATUS, &Status, sizeof(Status));

			} while (pos != NULL);

			
			//����״̬
			pServerUserItem->SetUserStatus(US_GP_OFFLINE);
			//����Ⱥ��
			pServerUserItem->SetActiveGroupID(0);
			//����Socket
			pServerUserItem->SetSocketID(0);

			wsprintf(szInfo, _T("ptdtai OnEventTCPNetworkShut dwSocketID=%d,wBindIndex=%d dwGroupID = %d dwUserID = %d  cbStatus = %d"), dwSocketID, wBindIndex,
				UpdateOnLineStatus.dwGroupID, UpdateOnLineStatus.dwUserID, UpdateOnLineStatus.cbStatus);
			//OutputDebugString(szInfo);

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_ONLINE_STATUS, dwSocketID, &UpdateOnLineStatus, sizeof(UpdateOnLineStatus));
		}
	}
	
	wsprintf(szInfo, _T("ptdtai OnEventTCPNetworkShut dwSocketID=%d,wBindIndex=%d"), dwSocketID, wBindIndex);
	//OutputDebugString(szInfo);
	SendLogData(szInfo);
	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	if (wcslen(m_PrsAdd) > 0)
	{
		bool bchange = false;
		if (wcscmp(m_PrsAdd, m_pInitParameter->m_PrsnlRmCorrespondAddress.szAddress) != 0)
			bchange = true;
		if (m_pInitParameter->m_wPrsnlRmCorrespondPort != m_wPrsPort)
			bchange = true;
		if (bchange)
		{
			CTraceService::TraceString(_T("Լս��ַ�쳣�ı�"), TraceLevel_Exception);
			SendLogData(_T("Լս��ַ�쳣�ı�"));
		}
	}

	CString cc;
	cc.Format(_T("OnEventDataBase wRequestID=%d,dwcontextid=%d"), wRequestID,dwContextID);
	SendLogData(cc);
	try
	{
		switch (wRequestID)
		{
			case DBO_GR_LOGON_SUCCESS:		//��¼�ɹ�
			{
				return OnDBLogonSuccess(dwContextID, pData, wDataSize);
			}
			case DBO_GR_LOGON_FAILURE:		//��¼ʧ��
			{
				return OnDBLogonFailure(dwContextID, pData, wDataSize);
			}
			case DBO_GR_CREATE_GROUP:		//����Ⱥ��
			{
				return OnDBCreateGroup(dwContextID, pData, wDataSize);
			}
			case DBO_GR_UPDATE_GROUP:		//����Ⱥ��
			{
				return OnDBUpdateGroup(dwContextID, pData, wDataSize);
			}
			case DBO_GR_DELETE_GROUP:		//ɾ��Ⱥ��
			{
				return OnDBDeleteGroup(dwContextID, pData, wDataSize);
			}
			case DBO_GR_INSERT_MEMBER:		//��ӳ�Ա
			{
				return OnDBInsertMember(dwContextID, pData, wDataSize);
			}
			case DBO_GR_DELETE_MEMBER:		//ɾ����Ա
			{
				return OnDBDeleteMember(dwContextID, pData, wDataSize);
			}
			case DBO_GR_APPLY_MESSAGE:		//������Ϣ
			{
				return OnDBApplyMessage(dwContextID, pData, wDataSize);
			}
			case DBO_GR_APPLY_MESSAGE_EX:
			{

				CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(dwContextID);
				if (pGroupItem != NULL )
				{
					//��ȡ����
					DBO_GR_ApplyMessage * pDBApplyMessage = (DBO_GR_ApplyMessage *)pData;

					//��ȡ����
					SUB_GA_ApplyMessage * pApplyMessage = (SUB_GA_ApplyMessage *)m_cbBuffer;
					ZeroMemory(pApplyMessage, sizeof(m_cbBuffer));

					//���ñ���
					pApplyMessage->cbMessageType = pDBApplyMessage->cbMessageType;
					pApplyMessage->wMessageCount = pDBApplyMessage->wMessageCount;

					//���ñ���	
					CopyMemory(pApplyMessage + 1, pDBApplyMessage + 1, wDataSize - sizeof(DBO_GR_ApplyMessage));

					//��������
					WORD wPacketSize = sizeof(SUB_GA_ApplyMessage) + wDataSize - sizeof(DBO_GR_ApplyMessage);
					SendDataToGroupMember(pGroupItem->GetGroupID(), MDM_GA_LOGIC_SERVICE, SUB_GA_APPLY_MESSAGE, pApplyMessage, wPacketSize,false);
				}

				return true;
			}
			case DBO_GR_GROUP_OPTION:		//Ⱥ������
			{
				return OnDBGroupOption(dwContextID, pData, wDataSize);
			}
			case DBO_GR_FIRBID_WORDS:		//��ֹ�ʻ�
			{
				return OnDBForbidWords(dwContextID, pData, wDataSize);
			}
			case DBO_GR_WEALTH_UPDATE:		//�Ƹ�����
			{
				return OnDBWealthUpdate(dwContextID, pData, wDataSize);
			}
			case DBO_GR_APPLY_DEL_RESULT:	//ɾ�����
			{
				return OnDBApplyDelResult(dwContextID, pData, wDataSize);
			}
			case DBO_GR_OPERATE_SUCCESS:	//�����ɹ�
			{
				return OnDBOperateSuccess(dwContextID, pData, wDataSize);
			}
			case DBO_GR_OPERATE_FAILURE:	//����ʧ��
			{
				return OnDBOperateFailure(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GROUP_LIST:			//Ⱥ���б�
			{
				return OnDBGroupList(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GROUP_LIST_FINISH:	//�б����
			{
				return OnDBGroupListFinish(dwContextID, pData, wDataSize);
			}
			case DBO_GR_COMMON_INFO:
			{
				return OnDBGroupCommonInfo(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GROUP_ITEM:			//Ⱥ������
			{
				return OnDBGroupItem(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GROUP_ITEM_FINISH:	//�������
			{
				return OnDBGroupItemFinish(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GROUP_MEMBER:		//Ⱥ���Ա
			{
				return OnDBGroupMember(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GROUP_CONFIG:		//Ⱥ���淨
			{
				return OnDBGroupConfig(dwContextID, pData, wDataSize);
			}
			case DBO_GR_UPDATE_MEMBER:		//���³�Ա
			{
				return OnDBUpdateMember(dwContextID, pData, wDataSize);
			}
			case DBO_GR_BATTLE_RESULT:		//��ѯԼս
			{
				return OnDBBattleResult(dwContextID, pData, wDataSize);
			}
			case DBO_GR_SETTLE_RESULT:		//������
			{
				//��������
				WORD wBindIndex = LOWORD(dwContextID);
				tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);


				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_SETTLE_RESULT, pData, wDataSize);

				return true;
			}
			case DBO_GR_TOTAL_RESULT:		//�������
			{
				//��������
				WORD wBindIndex = LOWORD(dwContextID);
				tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_TOTAL_RESULT, pData, wDataSize);
				return true;
			}
			case DBO_GR_MODIFY_RIGHT:		//�޸�Ȩ��
			{
				return OnDBModifyRight(dwContextID, pData, wDataSize);
			}
			case DBO_GR_PRESENT_SUCCESS://���ͳɹ�
			{
				return OnDBPresentSuccess(dwContextID, pData, wDataSize);
			}
			case  DBO_GR_QUERY_PRESENT_LIST://�����б�
			{
				return OnDBPresentList(dwContextID, pData, wDataSize);
			}
			case  DBO_GR_QUERY_REVENUE_LIST://�۷��б�
			{
				return OnDBRevenueList(dwContextID, pData, wDataSize);
			}
			case DBO_GR_APPEND_CONFIG:
			{
				return OnDBAppendConfig(dwContextID, pData, wDataSize);
			}
			case DBO_GR_MODIFY_CONFIG:
			{
				return OnDBModifyConfig(dwContextID, pData, wDataSize);
			}
			case DBO_GR_DELETE_CONFIG:
			{
				return OnDBDeleteConfig(dwContextID, pData, wDataSize);
			}
			case DBO_GR_DAY_RECORD_LIST:
			{
				return OnDBDayRecordList(dwContextID, pData, wDataSize);
			}
			case DBO_GR_USER_DAY_DETAIL_LIST:
			{
				return OnDBUserDayDetailList(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GROUP_DAY_DETAIL_LIST:
			{
				return OnDBGroupDayDetailList(dwContextID, pData, wDataSize);
			}
			case DBO_GR_QUERY_CREATE_RESULT:
			{
				return OnDBQueryCreateResult(dwContextID, pData, wDataSize);
			}
			case DBO_GR_MEDAL_MODE:
			{
				return OnDBMedalMode(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GROUP_STATUS:
			{
				return OnDBGroupStatus(dwContextID, pData, wDataSize);
			}
			case DBO_GR_INOUT_MODE:
			{
				return OnDBInOutMode(dwContextID, pData, wDataSize);
			}
			case DBO_GR_MANAGER_RIGHT:
			{
				return OnDBQueryManagerRight(dwContextID, pData, wDataSize);
			}
			case DBO_GR_SET_MANAGER_RIGHT:
			{
				return OnDBSetManagerRight(dwContextID, pData, wDataSize);
			}
			case DBO_GR_FREE_TABLE_LIMIT:
			{
				return OnDBFreeTableLImit(dwContextID, pData, wDataSize);
			}
			case DBO_GR_BIGWINNER_SHOW:
			{
				return OnDBBigWinnerShow(dwContextID, pData, wDataSize);
			}
			case DBO_GR_SET_FLAG:
			{
				return OnDBSetFlag(dwContextID, pData, wDataSize);
			}
			case DBO_GR_CLEAR_MEDAL:
			{
				return OnDBClearMedal(dwContextID, pData, wDataSize);
			}
			case DBO_GR_FAVORITE_CONFIG:
			{
				return OnDBFavoriteConfig(dwContextID, pData, wDataSize);
			}
			case DBO_GR_ROLL_MSG:
			{
				return OnDBRollMsg(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GROUP_NAME:
			{
				return OnDBGroupName(dwContextID, pData, wDataSize);
			}
			case DBO_GR_ROOM_DETAIL:
			{
				return OnDBRoomDetailInfo(dwContextID, pData, wDataSize);
			}
			case DBO_GR_CAPTAIN_LIST:
			{
				return OnDBCaptainList(dwContextID, pData, wDataSize);
			}
			case DBO_GR_ADD_TEAMER:
			{
				return OnDBAddTeamer(dwContextID, pData, wDataSize);
			}
			case DBO_GR_TEAM_DAY_RECORD_LIST:
			{
				return OnDBTeamDayRecord(dwContextID, pData, wDataSize);
			}
			case DBO_GR_TEAMER_DAY_LIST:
			{
				return OnDBTeamerDayRecord(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GROUP_PRIVACY:
			{
				return OnDBGroupPrivacy(dwContextID, pData, wDataSize);
			}
			case DBO_GR_CAPTAIN_POINT:
			{
				return OnDBCaptainPoint(dwContextID, pData, wDataSize);
			}
			case DBO_GR_BOX_INFO:
			{
				return OnDBBoxInfo(dwContextID, pData, wDataSize);
			}
			case DBO_GR_GET_AWARDSCORE_RES:
			{
				return OnDBGetAwardScore(dwContextID, pData, wDataSize);
			}
			case DBO_GR_INVITATION_CODE:
			{
				return OnDBInvitationCode(dwContextID, pData, wDataSize);
			}
			case DBO_GR_BLIND_CODE_RES:
			{
				return OnDBBlindCode(dwContextID, pData, wDataSize);
			}
			case DBO_GR_QUERY_SCORE_RES:
			{
				return OnDBQueryScore(dwContextID, pData, wDataSize);
			}
			case DBO_GR_MODIFY_INFO:
			{
				return OnDBModifyInfo(dwContextID, pData, wDataSize);
			}
		}
	}
	catch (...)
	{
		//������ʾ
		TCHAR szDescribe[128] = TEXT("");

		_snwprintf_s(szDescribe, CountArray(szDescribe), TEXT("OnEventDataBase(SubCmdID=%d)"), wRequestID);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe, TraceLevel_Warning);
	}
	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)		
	{
		//������ʾ
		TCHAR szDescribe[128]=TEXT("");

		_snwprintf_s(szDescribe, CountArray(szDescribe), TEXT("��Э�������������ӹر��ˣ�%ld �����������"), m_pInitParameter->m_wConnectTime);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		//����ʱ��
		//ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);

		return true;
	}
	//Լս����
	if (wServiceID == NETWORK_BATTLE)
	{

		//������ʾ
		TCHAR szDescribe[128]=TEXT("");

		_snwprintf_s(szDescribe, CountArray(szDescribe), TEXT("��Լս�����������ӹر��ˣ�%ld �����������"), m_pInitParameter->m_wConnectTime);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		//����״̬
		m_bNeedConnectBattle = true;

		//����ʱ��
		//ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_CONNECT_BATTLE_START, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

		return true;
	}

	//��־����
	if (wServiceID == NETWORK_LOG_SERVER)
	{
		//����ʱ��
		//ASSERT(m_pITimerEngine != NULL);
		m_pITimerEngine->SetTimer(IDI_CONNECT_LOG_SERVER, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

		return true;
	}

	//AI����
	if (wServiceID == NETWORK_AI_SERVER)
	{
		//������ʾ
		TCHAR szDescribe[128] = TEXT("");

		_snwprintf_s(szDescribe, CountArray(szDescribe), TEXT("��AI�����������ӹر��ˣ�%ld �����������"), m_pInitParameter->m_wConnectTime);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe, TraceLevel_Warning);

		//����ʱ��
		//ASSERT(m_pITimerEngine != NULL);
		m_pITimerEngine->SetTimer(IDI_CONNECT_AI_SERVER, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

		return true;
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

			_snwprintf_s(szDescribe, CountArray(szDescribe), TEXT("Э������������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			//ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڻ�ȡ������Ϣ..."),TraceLevel_Normal);

		//��������
		CMD_CS_C_RegisterGroup RegisterGroup;
		ZeroMemory(&RegisterGroup, sizeof(RegisterGroup));

		//���ñ���	
		RegisterGroup.wServicePort = m_pInitParameter->m_wServicePort;
		lstrcpyn(RegisterGroup.szServerAddr, m_pInitParameter->m_ServiceAddress.szAddress, CountArray(RegisterGroup.szServerAddr));

		//��������
		m_TCPSocketCorrespond->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_GROUP, &RegisterGroup, sizeof(RegisterGroup));

		return true;
	}

	//����Լս
	if (wServiceID == NETWORK_BATTLE)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");

			_snwprintf_s(szDescribe, CountArray(szDescribe), TEXT("��Լս����������ʧ�� [ %ld ]��%ld �����������"),
			nErrorCode,m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			//ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_BATTLE_START, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return false;
		}

		if (m_pITCPSocketBattle != NULL)
		{

			//��������
			CMD_CS_C_RegisterGroup RegisterGroup;
			ZeroMemory(&RegisterGroup, sizeof(RegisterGroup));

			//���ñ���	
			RegisterGroup.wServicePort = m_pInitParameter->m_wServicePort;
			lstrcpyn(RegisterGroup.szServerAddr, m_pInitParameter->m_ServiceAddress.szAddress, CountArray(RegisterGroup.szServerAddr));

			//��������
			m_pITCPSocketBattle->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_GROUP, &RegisterGroup, sizeof(RegisterGroup));

			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess = ER_SUCCESS;
			SendUIControlPacket(UI_BATTLE_RESULT, &ControlResult, sizeof(ControlResult));
		}

		return true;
	}


	if (wServiceID == NETWORK_LOG_SERVER)
	{
		//�����ж�
		if (nErrorCode != 0)
		{
			//����ʱ��
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_LOG_SERVER, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return true;
		}
	}


	//AI������
	if (wServiceID == NETWORK_AI_SERVER)
	{
		//�����ж�
		if (nErrorCode != 0)
		{
			//������ʾ
			TCHAR szDescribe[128] = TEXT("");

			_snwprintf_s(szDescribe, CountArray(szDescribe), TEXT("AI����������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode, m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe, TraceLevel_Warning);

			//����ʱ��
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_AI_SERVER, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڻ�ȡ������Ϣ..."), TraceLevel_Normal);


		//��������
		m_pAITCPSocketService->SendData(MDM_SS_S_SERVICE, SUB_SS_S_AI_SERVER, NULL, 0);

		return true;
	}

	

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	if (wcslen(m_PrsAdd) > 0)
	{
		bool bchange = false;
		if (wcscmp(m_PrsAdd, m_pInitParameter->m_PrsnlRmCorrespondAddress.szAddress) != 0)
			bchange = true;
		if (m_pInitParameter->m_wPrsnlRmCorrespondPort != m_wPrsPort)
			bchange = true;
		if (bchange)
		{
			CTraceService::TraceString(_T("Լս��ַ�쳣�ı�"), TraceLevel_Exception);
			SendLogData(_T("Լս��ַ�쳣�ı�"));
		}
	}
	CString cc;
	cc.Format(_T("OnEventTCPSocketRead wServiceID=%d,mainid=%d,subid=%d"), wServiceID,Command.wMainCmdID,Command.wSubCmdID);
	SendLogData(cc);
	try
	{

		//Э������
		if (wServiceID==NETWORK_CORRESPOND)
		{
			switch (Command.wMainCmdID)
			{
			case MDM_CS_REGISTER:		//ע�����
				{
					return OnTCPSocketMainRegister(Command.wSubCmdID, pData, wDataSize);
				}
			case MDM_CS_SERVICE_INFO:	//������Ϣ
				{
					return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
				}	
			case MDM_CS_CLUB_SERVICE_INFO://Э��ת��
				{
					return OnTCPSocketClubInfo(Command.wSubCmdID, pData, wDataSize);
				}
			case MDM_CS_WEB_SERVICE://��վ��Ϣ
				{
					return OnTCPSocketWebInfo(Command.wSubCmdID, pData, wDataSize);
				}
			}


		}

		if (wServiceID == NETWORK_BATTLE)
		{
			if (Command.wMainCmdID == MDM_CS_REGISTER)
			{
				return OnTCPSocketBattleMainRegister(Command.wSubCmdID, pData, wDataSize);
			}
			else if (Command.wMainCmdID == MDM_CS_CLUB_SERVICE_INFO)
			{
				return OnTCPSocketBattleMainBattleService(Command.wSubCmdID, pData, wDataSize);
			}

			return true;
		}

		if (wServiceID == NETWORK_AI_SERVER)
		{
			if (Command.wMainCmdID == MDM_CS_S_AI_ACTION)
			{
				return OnTCPSocketAIMainRegister(Command.wSubCmdID, pData, wDataSize);
			}

			return true;
		}


	} 
	catch (...)
	{
		CString strMessage;
		strMessage.Format(TEXT("wServiceID=%d,wMainCmdID=%d,wSubCmdID=%d"),wServiceID,Command.wMainCmdID,Command.wSubCmdID);
		CTraceService::TraceString(strMessage,TraceLevel_Exception);
	}

	//�������
	////ASSERT(FALSE);

	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
		{
			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT ,&ControlResult,sizeof(ControlResult));			

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//��������
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//Ч�����
			//ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_TCPSocketCorrespond->CloseSocket();

			//��ʾ��Ϣ
			LPCTSTR pszDescribeString = pRegisterFailure->szDescribeString;
			if (lstrlen(pszDescribeString) > 0)
			{
				CTraceService::TraceString(pszDescribeString, TraceLevel_Exception);
			}

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess = ER_FAILURE;
			SendUIControlPacket(UI_CORRESPOND_RESULT, &ControlResult, sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//�߼�����
bool CAttemperEngineSink::OnTCPNetworkMainLogicService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ����
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	switch (wSubCmdID)
	{
	case SUB_GA_LOGON_SERVER:		//��¼����
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, _T("ptdt SUB_GA_LOGON_SERVER1 dwSocketID=%d"), dwSocketID);

			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_LogonServer));
			if (wDataSize != sizeof(SUB_GA_LogonServer)) return false;

			//��ֵ�ж�
			if ((pBindParameter == NULL))
			{
				//ASSERT(FALSE);
				return false;
			}
			wsprintf(szInfo, _T("ptdt SUB_GA_LOGON_SERVER2 dwSocketID=%d"), dwSocketID);
			//��ȡ����
			SUB_GA_LogonServer * pLogonServer = (SUB_GA_LogonServer *)pData;
			pLogonServer->szLogonPass[CountArray(pLogonServer->szLogonPass) - 1] = 0;
			CString cc;
			cc.Format(_T("��������½�����ˣ�userid=%d,scoketid=%d"), pLogonServer->dwUserID,dwSocketID);
			SendLogData(cc);
			//��ѯ�û�
			CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pLogonServer->dwUserID);

			//�Ͽ�ǰ������ ����������
			if (pServerUserItem != NULL && pServerUserItem->GetSocketID() != 0)
			{
				//������Ϣ
				WORD wLastBindIndex = LOWORD(pServerUserItem->GetSocketID());
				tagBindParameter * pLastBindParameter = GetBindParameter(wLastBindIndex);
				//�Ͽ�����
				m_pITCPNetworkEngine->ShutDownSocket(pServerUserItem->GetSocketID());
				//�����Ϣ
				ZeroMemory(pLastBindParameter, sizeof(tagBindParameter));
			}
			//���ñ���
			pBindParameter->dwUserID = pLogonServer->dwUserID;

			//��¼ʱ��			
			DWORD dwLogonTime = (DWORD)time(NULL);

			wsprintf(szInfo, _T("ptdt SUB_GA_LOGON_SERVER3 dwSocketID=%d"), dwSocketID);
			//OutputDebugString(szInfo);
			if (pServerUserItem != NULL && pServerUserItem->GetUserItemKind() == enLocalKind)
			{
				//��������
				bool bEffectUser = false;
				CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;

				//У������
				if (pLocalUserItem->ContrastLogonPass(pLogonServer->szLogonPass) == false)
				{
					bEffectUser = true;
				}

				//У��ʱ��
				if (dwLogonTime - pLocalUserItem->GetLogonTime() > LOGON_INVALID_TIME)
				{
					//����ʱ��					
					bEffectUser = true;
				}

				//������֤
				if (bEffectUser == false)
				{
					//�û��ǳ�
					if (pServerUserItem->GetUserStatus() == US_GP_ONLINE)
					{
						//������Ϣ
						CMD_CM_SystemMessage SystemMessage;
						ZeroMemory(&SystemMessage, sizeof(SystemMessage));

						//��������						
						SystemMessage.wType = SMT_EJECT | SMT_CLOSE_LINK;
						lstrcpynW(SystemMessage.szString, L"�����˺��������ط���¼�����������ߣ�", CountArray(SystemMessage.szString));
						SystemMessage.wLength = CountStringBufferW(SystemMessage.szString);
						//������Ϣ
						WORD wPacketSize = sizeof(SystemMessage)-sizeof(SystemMessage.szString) + CountStringBufferW(SystemMessage.szString);
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_SYSTEM_MESSAGE, &SystemMessage, wPacketSize);
					}

					//���õ�¼ʱ��
					pLocalUserItem->SetLogonTime(dwLogonTime);
					return OnEventUserLogon(pLocalUserItem);
				}
			}

			//����ṹ
			DBR_GR_LogonServer LogonServer;
			ZeroMemory(&LogonServer, sizeof(LogonServer));

			//���ñ���
			LogonServer.dwUserID = pLogonServer->dwUserID;
			lstrcpyn(LogonServer.szLogonPass, pLogonServer->szLogonPass, CountArray(LogonServer.szLogonPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_SERVER, dwSocketID, &LogonServer, sizeof(LogonServer));
			cc.Format(_T("��������½������1��userid=%d"), pLogonServer->dwUserID);
			SendLogData(cc);
			return true;
		}
	case SUB_GA_SEARCH_GROUP:		//����Ⱥ��
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_SearchGroup));
			if (wDataSize != sizeof(SUB_GA_SearchGroup)) return false;

			SUB_GA_SearchGroup * pSearchGroup = (SUB_GA_SearchGroup *)pData;
			//�����û�
			CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pSearchGroup->dwUserID);
			if (pServerUserItem == NULL || pServerUserItem->GetUserStatus() != US_GP_ONLINE) return false;

			//����ṹ
			SUB_GA_S_SearchResult * pSearchResult = (SUB_GA_S_SearchResult *)m_cbBuffer;
			ZeroMemory(pSearchResult, sizeof(m_cbBuffer));

			//�Ƽ�Ⱥ��
			if (pSearchGroup->dwGroupID == INVALID_DWORD)
			{
				tagIMGroupProperty GroupProperty[10];
				ZeroMemory(GroupProperty, sizeof(GroupProperty));
				pSearchResult->wGroupCount = m_GroupInfoManager.GetRecommendGroup(pSearchGroup->dwUserID, 10, GroupProperty);
				CopyMemory(pSearchResult + 1, GroupProperty, sizeof(tagIMGroupProperty)*pSearchResult->wGroupCount);
			}
			else
			{
				//����Ⱥ��
				CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pSearchGroup->dwGroupID);
				if (pGroupItem != NULL)
				{
					pSearchResult->wGroupCount++;
					CopyMemory(pSearchResult + 1, pGroupItem->GetGroupProperty(), sizeof(tagIMGroupProperty));
				}
			}

			//�����ж�			
			WORD wPacketSize = sizeof(SUB_GA_S_SearchResult)+pSearchResult->wGroupCount*sizeof(tagIMGroupProperty);
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_SEARCH_RESULT, m_cbBuffer, wPacketSize);

			return true;
		}
	case SUB_GA_CREATE_GROUP:		//����Ⱥ��
		{
			//����У��
			//ASSERT(wDataSize <= sizeof(SUB_GA_CreateGroup));
			if (wDataSize > sizeof(SUB_GA_CreateGroup)) return false;

			SUB_GA_CreateGroup *pCreateGroup = (SUB_GA_CreateGroup *)pData;
			//��������
			pCreateGroup->szGroupName[CountArray(pCreateGroup->szGroupName) - 1] = 0;
			pCreateGroup->szGroupIntroduce[CountArray(pCreateGroup->szGroupIntroduce) - 1] = 0;

			//��ѯ�û�
			CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pCreateGroup->dwCreateID);
			if (pServerUserItem == NULL || pServerUserItem->GetUserItemKind() != enLocalKind) return false;

			//��������У��
			int nCheckResult = CheckClubName(pCreateGroup->szGroupName);
			if (nCheckResult!=0)
			{
				//��ȡ����
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));

				//���ñ���
				if (nCheckResult==-1) lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,���������Ʋ���Ϊ��,�����˴���ʧ�ܣ�", CountArray(OperateFailure.szErrorDescribe));
				if (nCheckResult == 1) lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,���������Ƴ��ȷ�ΧΪ2-6���ַ�,�����˴���ʧ�ܣ�", CountArray(OperateFailure.szErrorDescribe));
				if (nCheckResult == 2) lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,���������Ʋ��ɰ����������,�����˴���ʧ�ܣ�", CountArray(OperateFailure.szErrorDescribe));

				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));

				return true;
			}
			//������������
			if (m_WordFilter.FilterWords(pCreateGroup->szGroupName) == true)
			{
				//��ȡ����
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));

				//���ñ���		
				lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,���������а������дʻ�,�����˴���ʧ�ܣ�", CountArray(OperateFailure.szErrorDescribe));

				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));

				return true;
			}

			//�����˽��ܹ���				
			if (m_WordFilter.FilterWords(pCreateGroup->szGroupIntroduce) == true)
			{
				//��ȡ����
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));

				//���ñ���		
				lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,�����˽����а������дʻ�,�����˴���ʧ�ܣ�", CountArray(OperateFailure.szErrorDescribe));

				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));

				return true;
			}

			//����ṹ
			DBR_GR_CreateGroup CreateGroup;
			ZeroMemory(&CreateGroup, sizeof(CreateGroup));

			//������Ϣ
			CreateGroup.cbPayType = pCreateGroup->cbPayType;
			CreateGroup.cbCurrencyKind = pCreateGroup->cbCurrencyKind;

			//�û���Ϣ
			CreateGroup.dwCreateID = pCreateGroup->dwCreateID;
			lstrcpynW(CreateGroup.szLogonPass, pCreateGroup->szLogonPass, CountArray(CreateGroup.szLogonPass));

			//Ⱥ����Ϣ
			lstrcpynW(CreateGroup.szGroupName, pCreateGroup->szGroupName, CountArray(CreateGroup.szGroupName));
			lstrcpynW(CreateGroup.szGroupIntroduce, pCreateGroup->szGroupIntroduce, CountArray(CreateGroup.szGroupIntroduce));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CREATE_GROUP, dwSocketID, &CreateGroup, sizeof(CreateGroup));

			return true;
		}
	case SUB_GA_UPDATE_GROUP:		//����Ⱥ��
		{
			//����У��
			//ASSERT(wDataSize >= sizeof(SUB_GA_UpdateGroup));
			if (wDataSize < sizeof(SUB_GA_UpdateGroup)) return false;

			SUB_GA_UpdateGroup *pUpdateGroup = (SUB_GA_UpdateGroup *)pData;

			//��ѯ�û�
			CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUpdateGroup->dwCreateID);
			if (pServerUserItem == NULL || pServerUserItem->GetUserItemKind() != enLocalKind) return false;

			//��������
			LPBYTE pcbBuffer = (LPBYTE)pData;
			WORD wOffsetPos = sizeof(SUB_GA_UpdateGroup);

			//���²Ƹ�
			if ((pUpdateGroup->cbUpdateKind&UPGROUP_KIND_WEALTH) != 0)
			{
				//����ƫ��
				wOffsetPos += sizeof(tagIMGroupWealth);
			}

			//��������
			if ((pUpdateGroup->cbUpdateKind&UPGROUP_KIND_PROPERTY) != 0)
			{
				//��ȡ����
				tagIMGroupProperty * pGroupProperty = (tagIMGroupProperty *)(pcbBuffer + wOffsetPos);
				pGroupProperty->szGroupIntroduce[CountArray(pGroupProperty->szGroupIntroduce) - 1] = 0;

				//�����˽��ܹ���
				if (m_WordFilter.FilterWords(pGroupProperty->szGroupIntroduce) == true)
				{

					//��ȡ����
					SUB_GA_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//���ñ���		
					lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,�����˽����а������дʻ�,��������Ϣ�޸�ʧ�ܣ�", CountArray(OperateFailure.szErrorDescribe));

					//��������
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
					return true;
				}
			}

			//����ṹ
			DBR_GR_UpdateGroup * pDBUpdateGroup = (DBR_GR_UpdateGroup *)m_cbBuffer;
			ZeroMemory(pDBUpdateGroup, sizeof(m_cbBuffer));

			//Ⱥ����Ϣ
			pDBUpdateGroup->dwGroupID = pUpdateGroup->dwGroupID;
			pDBUpdateGroup->cbUpdateKind = pUpdateGroup->cbUpdateKind;

			//�û���Ϣ
			pDBUpdateGroup->dwCreateID = pUpdateGroup->dwCreateID;
			lstrcpynW(pDBUpdateGroup->szLogonPass, pUpdateGroup->szLogonPass, CountArray(pDBUpdateGroup->szLogonPass));

			//��������
			CopyMemory(pDBUpdateGroup + 1, pUpdateGroup + 1, wDataSize - sizeof(SUB_GA_UpdateGroup));

			//Ͷ������
			WORD wPacketSize = sizeof(DBR_GR_UpdateGroup)+wDataSize - sizeof(SUB_GA_UpdateGroup);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_GROUP, dwSocketID, m_cbBuffer, wPacketSize);

			return true;
		}
	case SUB_GA_DELETE_GROUP:		//ɾ��Ⱥ��
		{
			//����У��
			//ASSERT(wDataSize <= sizeof(SUB_GA_DeleteGroup));
			if (wDataSize > sizeof(SUB_GA_DeleteGroup)) return false;

			//��ȡ����
			SUB_GA_DeleteGroup * pDeleteGroup = (SUB_GA_DeleteGroup *)pData;

			//����ṹ
			DBR_GR_DeleteGroup DeleteGroup;
			ZeroMemory(&DeleteGroup, sizeof(DeleteGroup));

			//Ⱥ���ʶ
			DeleteGroup.dwGroupID = pDeleteGroup->dwGroupID;

			//�û���Ϣ
			DeleteGroup.dwCreateID = pDeleteGroup->dwCreateID;
			lstrcpynW(DeleteGroup.szLogonPass, pDeleteGroup->szLogonPass, CountArray(DeleteGroup.szLogonPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_GROUP, dwSocketID, &DeleteGroup, sizeof(DeleteGroup));

			return true;
		}
	case SUB_GA_DELETE_MEMBER:		//ɾ����Ա
		{
			//����У��
			//ASSERT(wDataSize > sizeof(SUB_GA_DeleteMember));
			if (wDataSize <= sizeof(SUB_GA_DeleteMember)) return false;

			SUB_GA_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ����
			SUB_GA_DeleteMember * pDeleteMember = (SUB_GA_DeleteMember *)pData;

			//���Ҵ�����
			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pDeleteMember->dwGroupID);
			if (pGroupItem == NULL)
			{
				lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,��������Ϣ��ѯʧ�ܣ����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;
			}

			if (pGroupItem->GetGroupProperty()->cbGroupStatus != 1)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"����ʧ�ܣ��ô������Ѷ��ᣡ", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;

			}

			BYTE *pp = (BYTE*)pData;
			DWORD *pUserID = (DWORD*)(pp + sizeof(SUB_GA_DeleteMember));
			tagClubTableInfo tableinfo;
			for (int i = 0; i < pDeleteMember->nCount; i++)
			{
				DWORD dwReceiveUserID = *(pUserID++);
				if (pGroupItem->IsUserInGame(dwReceiveUserID, tableinfo, false))
				{
					//֪ͨԼս��ɢ����
					CMD_CS_C_DismissTable pack;
					ZeroMemory(&pack, sizeof(pack));
					pack.dwServerID = tableinfo.dwServerID;
					pack.dwTableID = tableinfo.wTableID;
					m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &pack, sizeof(CMD_CS_C_DismissTable));

				}
			}

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_MEMBER, dwSocketID, pData, wDataSize);

			return true;
		}
	case SUB_GA_APPLY_REQUEST:	//��������
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_ApplyRequest));
			if (wDataSize != sizeof(SUB_GA_ApplyRequest)) return false;

			//��ȡ����
			SUB_GA_ApplyRequest * pApplyRequest = (SUB_GA_ApplyRequest *)pData;

			//����IDУ��
			if (pBindParameter->dwUserID != pApplyRequest->dwApplyerID) return false;

			//����ṹ
			DBR_GR_ApplyRequest ApplyRequest;
			ZeroMemory(&ApplyRequest, sizeof(ApplyRequest));

			//Ⱥ����Ϣ			
			ApplyRequest.dwGroupID = pApplyRequest->dwGroupID;
			ApplyRequest.dwApplyerID = pApplyRequest->dwApplyerID;
			ApplyRequest.cbApplyType = pApplyRequest->cbApplyType;
			ApplyRequest.lInvitationCode = pApplyRequest->lInvitationCode;
			lstrcpynW(ApplyRequest.szLogonPass, pApplyRequest->szLogonPass, CountArray(ApplyRequest.szLogonPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_REQUEST, dwSocketID, &ApplyRequest, sizeof(ApplyRequest));

			return true;
		}
	case SUB_GA_APPLY_RESPOND:	//������Ӧ
	{
			//����У��
			//ASSERT(wDataSize <= sizeof(SUB_GA_ApplyRespond));
			if (wDataSize > sizeof(SUB_GA_ApplyRespond)) return false;
			//��ȡ����
			SUB_GA_ApplyRespond * pApplyRespond = (SUB_GA_ApplyRespond *)pData;

			//����ṹ
			DBR_GR_ApplyRespond ApplyRespond;
			ZeroMemory(&ApplyRespond, sizeof(ApplyRespond));

			//���ñ���
			ApplyRespond.dwMessageID = pApplyRespond->dwMessageID;

			//Ⱥ����Ϣ
			ApplyRespond.dwGroupID = pApplyRespond->dwGroupID;
			ApplyRespond.dwApplyerID = pApplyRespond->dwApplyerID;
			ApplyRespond.cbApplyStatus = pApplyRespond->cbApplyStatus;

			//�û���Ϣ
			ApplyRespond.dwCreaterID = pApplyRespond->dwCreaterID;
			lstrcpynW(ApplyRespond.szLogonPass, pApplyRespond->szLogonPass, CountArray(ApplyRespond.szLogonPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_RESPOND, dwSocketID, &ApplyRespond, sizeof(ApplyRespond));


			return true;
	}
	case SUB_GA_APPLY_DELETE:	//����ɾ��
		{
			//����У��
			//ASSERT(wDataSize <= sizeof(SUB_GA_ApplyDelete));
			if (wDataSize > sizeof(SUB_GA_ApplyDelete)) return false;

			//��ȡ����
			SUB_GA_ApplyDelete * pApplyDelete = (SUB_GA_ApplyDelete *)pData;

			//����ṹ
			DBR_GR_ApplyDelete ApplyDelete;
			ZeroMemory(&ApplyDelete, sizeof(ApplyDelete));

			//���ñ���
			ApplyDelete.dwMessageID = pApplyDelete->dwMessageID;
			ApplyDelete.cbMessageType = pApplyDelete->cbMessageType;

			//���ñ���
			ApplyDelete.dwUserID = pApplyDelete->dwUserID;
			lstrcpynW(ApplyDelete.szLogonPass, pApplyDelete->szLogonPass, CountArray(ApplyDelete.szLogonPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_DELETE, dwSocketID, &ApplyDelete, sizeof(ApplyDelete));
			return true;
		}
	case SUB_GA_SETTLE_BATTLE:	//Լս����
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_SettleBattle));
			if (wDataSize != sizeof(SUB_GA_SettleBattle)) return false;

			//��ȡ����
			SUB_GA_SettleBattle * pSettleBattle = (SUB_GA_SettleBattle *)pData;

			//����ṹ
			DBR_GR_SettleBattle SettleBattle;
			ZeroMemory(&SettleBattle, sizeof(SettleBattle));

			//���ñ���
			SettleBattle.dwGroupID = pSettleBattle->dwGroupID;
			SettleBattle.dwMemberID = pSettleBattle->dwMemberID;
			SettleBattle.dwSettleCount = pSettleBattle->dwSettleCount;

			//�û���Ϣ
			SettleBattle.dwCreaterID = pSettleBattle->dwCreaterID;
			lstrcpynW(SettleBattle.szLogonPass, pSettleBattle->szLogonPass, CountArray(SettleBattle.szLogonPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SETTLE_BATTLE, dwSocketID, &SettleBattle, sizeof(SettleBattle));
			return true;
		}
	case SUB_GA_QUERY_BATTLE:	//��ѯԼս
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_QueryBattle));
			if (wDataSize != sizeof(SUB_GA_QueryBattle)) return false;

			//��ȡ����
			SUB_GA_QueryBattle * pQueryBattle = (SUB_GA_QueryBattle *)pData;

			//����ṹ
			DBR_GR_QueryBattle QueryBattle;
			ZeroMemory(&QueryBattle, sizeof(QueryBattle));

			//���ñ���
			QueryBattle.dwGroupID = pQueryBattle->dwGroupID;

			//�û���Ϣ
			QueryBattle.dwCreaterID = pQueryBattle->dwCreaterID;
			lstrcpynW(QueryBattle.szLogonPass, pQueryBattle->szLogonPass, CountArray(QueryBattle.szLogonPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_BATTLE, dwSocketID, &QueryBattle, sizeof(QueryBattle));
			return true;
		}
	case SUB_GA_QUERY_TOTAL_BATTLE:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_QueryBattle));
			if (wDataSize != sizeof(SUB_GA_QueryBattle)) return false;

			//��ȡ����
			SUB_GA_QueryBattle * pQueryBattle = (SUB_GA_QueryBattle *)pData;

			//����ṹ
			DBR_GR_QueryBattle QueryBattle;
			ZeroMemory(&QueryBattle, sizeof(QueryBattle));

			//���ñ���
			QueryBattle.dwGroupID = pQueryBattle->dwGroupID;

			//�û���Ϣ
			QueryBattle.dwCreaterID = pQueryBattle->dwCreaterID;
			lstrcpynW(QueryBattle.szLogonPass, pQueryBattle->szLogonPass, CountArray(QueryBattle.szLogonPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_TOTAL_BATTLE, dwSocketID, &QueryBattle, sizeof(QueryBattle));
			return true;
		}
	case SUB_GA_APPEND_CONFIG:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_AppendConfig));
			if (wDataSize != sizeof(SUB_GA_AppendConfig)) return false;

			//��ȡ����
			SUB_GA_AppendConfig * pConfig = (SUB_GA_AppendConfig *)pData;
			pConfig->szConfigName[CountArray(pConfig->szConfigName) - 1] = 0;

			//����Ⱥ��
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pConfig->dwGroupID);

			if (!pGroupItem)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"����ʧ�ܣ��ô����˲����ڣ�", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;

			}

			if (pGroupItem != NULL && pGroupItem->GetGroupProperty()->cbGroupStatus != 1)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"����ʧ�ܣ��ô������Ѷ��ᣡ", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;

			}

			//У������˰�����Ϸ(����У��)
			if (m_ServerListManager.IsGroupPersonalMaxChairGame(pConfig->wKindID) && pGroupItem != NULL && pGroupItem->GetGroupCreaterID() != pConfig->dwCreateID)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"����ʧ�ܣ������˰�����Ϸֻ��������������", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;
			}

			//����ṹ
			DBR_GR_AppendConfig appendConfig;
			ZeroMemory(&appendConfig, sizeof(appendConfig));
			//���ñ���
			appendConfig.dwCreateID = pConfig->dwCreateID;
			appendConfig.dwGroupID = pConfig->dwGroupID;

			tagGroupConfigInfo *pConfigInfo = &appendConfig.configInfo;
			pConfigInfo->dwGroupID = pConfig->dwGroupID;
			pConfigInfo->wKindID = pConfig->wKindID;
			CopyMemory(pConfigInfo->szConfigName, pConfig->szConfigName, sizeof(pConfigInfo->szConfigName));
			pConfigInfo->lCellScore = pConfig->lCellScore;
			pConfigInfo->cbPlayMode = pConfig->cbPlayMode;
			pConfigInfo->dwDrawCountLimit = pConfig->dwDrawCountLimit;
			pConfigInfo->dwDrawTimeLimit = pConfig->dwDrawTimeLimit;
			pConfigInfo->wJoinGamePeopleCount = pConfig->wJoinGamePeopleCount;
			pConfigInfo->dwRoomTax = pConfig->dwRoomTax;
			pConfigInfo->cbPassword = pConfig->cbPassword;
			pConfigInfo->cbPayMode = pConfig->cbPayMode;
			CopyMemory(pConfigInfo->cbGameRule, pConfig->cbGameRule, sizeof(pConfigInfo->cbGameRule));
			pConfigInfo->cbGameMode = pConfig->cbGameMode;
			pConfigInfo->lEnterScore = pConfig->lEnterScore;
			pConfigInfo->lLeaveScore = pConfig->lLeaveScore;
			pConfigInfo->cbJoinMode = pConfig->cbJoinMode;
			pConfigInfo->cbTaxMode = pConfig->cbTaxMode;
			pConfigInfo->dwTaxCount = pConfig->dwTaxCount;
			pConfigInfo->cbLessScoreDismiss = pConfig->cbLessScoreDismiss;
			pConfigInfo->lTimeSecond = time(NULL);
			pConfigInfo->cbGroupPersonalMaxChair = m_ServerListManager.IsGroupPersonalMaxChairGame(pConfig->wKindID) ? TRUE : FALSE;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPEND_CONFIG, dwSocketID, &appendConfig, sizeof(appendConfig));
			return true;
		}
	case SUB_GA_MODIFY_CONFIG:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_ModifyConfig));
			if (wDataSize != sizeof(SUB_GA_ModifyConfig)) return false;

			//��ȡ����
			SUB_GA_ModifyConfig * pModifyConfig = (SUB_GA_ModifyConfig *)pData;

			//����Ⱥ��
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pModifyConfig->dwGroupID);
			if (pGroupItem != NULL && pGroupItem->GetGroupProperty()->cbGroupStatus != 1)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"����ʧ�ܣ��ô������Ѷ��ᣡ", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;

			}

			//����ṹ
			DBR_GR_ModifyConfig modifyConfig;
			ZeroMemory(&modifyConfig, sizeof(modifyConfig));

			tagGroupConfigInfo *pConfigInfo = &modifyConfig.configInfo;
			//���ñ���
			modifyConfig.dwUserID = pModifyConfig->dwUserID;
			modifyConfig.dwGroupID = pModifyConfig->dwGroupID;
			pConfigInfo->dwConfigID = pModifyConfig->dwConfigID;
			pConfigInfo->dwGroupID = pModifyConfig->dwGroupID;
			CopyMemory(pConfigInfo->szConfigName, pModifyConfig->szConfigName, sizeof(pConfigInfo->szConfigName));
			pConfigInfo->lCellScore = pModifyConfig->lCellScore;
			pConfigInfo->cbPlayMode = pModifyConfig->cbPlayMode;
			pConfigInfo->dwDrawCountLimit = pModifyConfig->dwDrawCountLimit;
			pConfigInfo->dwDrawTimeLimit = pModifyConfig->dwDrawTimeLimit;
			pConfigInfo->wJoinGamePeopleCount = pModifyConfig->wJoinGamePeopleCount;
			pConfigInfo->cbPassword = pModifyConfig->cbPassword;
			pConfigInfo->cbPayMode = pModifyConfig->cbPayMode;
			CopyMemory(pConfigInfo->cbGameRule, pModifyConfig->cbGameRule, sizeof(pConfigInfo->cbGameRule));
			pConfigInfo->cbGameMode = pModifyConfig->cbGameMode;
			pConfigInfo->lEnterScore = pModifyConfig->lEnterScore;
			pConfigInfo->lLeaveScore = pModifyConfig->lLeaveScore;
			pConfigInfo->cbTaxMode = pModifyConfig->cbTaxMode;
			pConfigInfo->dwTaxCount = pModifyConfig->dwTaxCount;
			pConfigInfo->cbLessScoreDismiss = pModifyConfig->cbLessScoreDismiss;
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_CONFIG, dwSocketID, &modifyConfig, sizeof(modifyConfig));
			return true;
		}
	case SUB_GA_DELETE_CONFIG:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_DeleteConfig));
			if (wDataSize != sizeof(SUB_GA_DeleteConfig)) return false;

			//��ȡ����
			SUB_GA_DeleteConfig * pDeleteConfig = (SUB_GA_DeleteConfig *)pData;
			//����Ⱥ��
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pDeleteConfig->dwGroupID);
			if (pGroupItem != NULL && pGroupItem->GetGroupProperty()->cbGroupStatus != 1)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"����ʧ�ܣ��ô������Ѷ��ᣡ", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;

			}

			//�����淨
			tagGroupConfigInfo * pGroupConfigInfo = pGroupItem->SearchConfig(pDeleteConfig->dwConfigID);
			if (!pGroupConfigInfo)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"����ʧ�ܣ����淨�����ڣ�", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;
			}

			//����ṹ
			DBR_GR_DeleteConfig deleteConfig;
			ZeroMemory(&deleteConfig, sizeof(deleteConfig));

			deleteConfig.dwConfigID = pDeleteConfig->dwConfigID;
			deleteConfig.dwGroupID = pDeleteConfig->dwGroupID;
			deleteConfig.dwUserID = pDeleteConfig->dwUserID;
			deleteConfig.bGroupPersonalMaxChair = (pGroupConfigInfo->cbGroupPersonalMaxChair == TRUE ? true : false);

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_CONFIG, dwSocketID, &deleteConfig, sizeof(deleteConfig));
			return true;
		}
	case SUB_GA_MODIFY_RIGHT:
		{
			//����У��
			//ASSERT(wDataSize <= sizeof(SUB_GA_ModifyRight));
			if (wDataSize > sizeof(SUB_GA_ModifyRight)) return false;

			//��ȡ����
			SUB_GA_ModifyRight * pModifyRight = (SUB_GA_ModifyRight *)pData;

			//����Ⱥ��
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pModifyRight->dwGroupID);
			if (pGroupItem != NULL && pGroupItem->GetGroupProperty()->cbGroupStatus != 1)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"����ʧ�ܣ��ô������Ѷ��ᣡ", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;

			}

			//����ṹ
			DBR_GR_ModifyRight ModifyRight;
			ZeroMemory(&ModifyRight, sizeof(ModifyRight));

			//Ⱥ����Ϣ
			ModifyRight.dwGroupID = pModifyRight->dwGroupID;
			ModifyRight.dwMemberID = pModifyRight->dwMemberID;

			//Ȩ����Ϣ
			ModifyRight.nRight = pModifyRight->nRight;
			ModifyRight.cbMode = pModifyRight->cbMode;

			//�û���Ϣ
			ModifyRight.dwCreateID = pModifyRight->dwCreateID;
			lstrcpynW(ModifyRight.szLogonPass, pModifyRight->szLogonPass, CountArray(ModifyRight.szLogonPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_RIGHT, dwSocketID, &ModifyRight, sizeof(ModifyRight));

			return true;
		}
	case SUB_GA_PRESENT_SCORE:
		{
			//����У��
			//ASSERT(wDataSize > sizeof(SUB_GA_PresentScore));
			if (wDataSize <= sizeof(SUB_GA_PresentScore)) return false;

			//��ȡ����
			SUB_GA_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ����
			SUB_GA_PresentScore * pPresentScore = (SUB_GA_PresentScore *)pData;

			//���Ҵ�����
			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pPresentScore->dwGroupID);
			if (pGroupItem == NULL)
			{
				lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,��������Ϣ��ѯʧ�ܣ����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;
			}

			if (pGroupItem->GetGroupProperty()->cbGroupStatus != 1)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"����ʧ�ܣ��ô������Ѷ��ᣡ", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;

			}

			if (pPresentScore->cbType>1)
			{
				lstrcpynW(OperateFailure.szErrorDescribe, L"�������Ͳ���ȷ��", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;
			}

			BYTE *pp = (BYTE*)pData;

			DWORD *pUserID = (DWORD*)(pp + sizeof(SUB_GA_PresentScore));
			tagClubTableInfo tableinfo;
			for (int i = 0; i < pPresentScore->nCount; i++)
			{
				DWORD dwReceiveUserID = *(pUserID++);
				if (pGroupItem->IsUserInGame(dwReceiveUserID, tableinfo, true))
				{
					lstrcpynW(OperateFailure.szErrorDescribe, L"�в�������������Ϸ��,���Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
					return true;
				}

				if (pPresentScore->cbType == 1)
				{
					if (pGroupItem->SearchMember(dwReceiveUserID)->lScore<pPresentScore->lPresentScore)
					{
						lstrcpynW(OperateFailure.szErrorDescribe, L"�в�������ѫ�²����۳������飡", CountArray(OperateFailure.szErrorDescribe));
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
						return true;
					}
				}
			}
			/*
			DBR_GR_PresentScore PresentScore;
			ZeroMemory(&PresentScore, sizeof(PresentScore));

			//���ñ���
			PresentScore.dwGroupID = pPresentScore->dwGroupID;
			PresentScore.dwPresentUserID = pPresentScore->dwPresentUserID;
			PresentScore.cbType = pPresentScore->cbType;
			PresentScore.lPresentScore = pPresentScore->lPresentScore;
			lstrcpynW(PresentScore.szLogonPass, pPresentScore->szLogonPass, CountArray(PresentScore.szLogonPass));
			*/
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_PRESENT_SCORE, dwSocketID, pData, wDataSize);
			return true;
		}
	case SUB_GA_QUERY_PRESENT:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_QueryPresent));
			if (wDataSize != sizeof(SUB_GA_QueryPresent)) return false;

			//��ȡ����
			SUB_GA_QueryPresent * pQuery = (SUB_GA_QueryPresent *)pData;

			//����ṹ
			DBR_GR_QueryPresent QueryPresent;
			ZeroMemory(&QueryPresent, sizeof(QueryPresent));

			//���ñ���
			QueryPresent.dwGroupID = pQuery->dwGroupID;
			QueryPresent.dwQueryUserID = pQuery->dwQueryUserID;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_PRESENT, dwSocketID, &QueryPresent, sizeof(QueryPresent));

			/*
			//�����Ȧ��������Ͷ�ݿ۷���Ϣ
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pQuery->dwGroupID);

			//���ҳ�Ա
			tagIMGroupMember *pGroupMember = pGroupItem->SearchMember(pQuery->dwQueryUserID);
			if ((pGroupItem != NULL && pGroupItem->GetGroupItem()->GroupProperty.dwCreaterID == pQuery->dwQueryUserID)
				|| (pGroupItem != NULL && pGroupMember != NULL && (pGroupMember->nMemberRight&2)!=0))
			{
				//����ṹ
				DBR_GR_QueryRevenue QueryRevenue;
				ZeroMemory(&QueryRevenue, sizeof(QueryRevenue));

				//���ñ���
				QueryRevenue.dwGroupID = pQuery->dwGroupID;
				QueryRevenue.dwQueryUserID = pQuery->dwQueryUserID;

				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_REVENUE, dwSocketID, &QueryRevenue, sizeof(QueryRevenue));
			}*/
			return true;

		}
	case SUB_GA_QUERY_DAYRECORD:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_QueryDayRecord));
			if (wDataSize != sizeof(SUB_GA_QueryDayRecord)) return false;

			//��ȡ����
			SUB_GA_QueryDayRecord * pQuery = (SUB_GA_QueryDayRecord *)pData;

			//����ṹ
			DBR_GR_QueryDayRecord QueryRecord;
			ZeroMemory(&QueryRecord, sizeof(QueryRecord));

			//���ñ���
			QueryRecord.dwGroupID = pQuery->dwGroupID;
			QueryRecord.dwQueryUserID = pQuery->dwQueryUserID;
			QueryRecord.dwDateID = pQuery->dwDateID;
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_DAYRECORD, dwSocketID, &QueryRecord, sizeof(QueryRecord));

			return true;

		}
	case SUB_GA_QUERY_USER_DETAIL:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_QueryUserDayDetail));
			if (wDataSize != sizeof(SUB_GA_QueryUserDayDetail)) return false;

			//��ȡ����
			SUB_GA_QueryUserDayDetail * pQuery = (SUB_GA_QueryUserDayDetail *)pData;

			//����ṹ
			DBR_GR_QueryUserDayDetail QueryRecord;
			ZeroMemory(&QueryRecord, sizeof(QueryRecord));

			//���ñ���
			QueryRecord.dwGroupID = pQuery->dwGroupID;
			QueryRecord.dwQueryUserID = pQuery->dwQueryUserID;
			QueryRecord.dwTargetUserID = pQuery->dwTargetUserID;
			QueryRecord.dwDateID = pQuery->dwDateID;
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_USERDAY_DETAIL, dwSocketID, &QueryRecord, sizeof(QueryRecord));

			return true;

		}
	case SUB_GA_QUERY_GROUP_DETAIL:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_QueryGroupDetail));
		if (wDataSize != sizeof(SUB_GA_QueryGroupDetail)) return false;

		//��ȡ����
		SUB_GA_QueryGroupDetail * pQuery = (SUB_GA_QueryGroupDetail *)pData;

		//����ṹ
		DBR_GR_QueryGroupDayDetail QueryRecord;
		ZeroMemory(&QueryRecord, sizeof(QueryRecord));

		//���ñ���
		QueryRecord.dwGroupID = pQuery->dwGroupID;
		QueryRecord.dwQueryUserID = pQuery->dwQueryUserID;
		QueryRecord.dwDateID = pQuery->dwDateID;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_GROUPDAY_DETAIL, dwSocketID, &QueryRecord, sizeof(QueryRecord));

		return true;

	}
	case SUB_GA_QUERY_CREATE:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_QueryCreate));
			if (wDataSize != sizeof(SUB_GA_QueryCreate)) return false;

			//��ȡ����
			SUB_GA_QueryCreate * pCreate = (SUB_GA_QueryCreate *)pData;
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pCreate->dwGroupID);
			if (pGroupItem == NULL)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,������ID�����ڣ����飡", CountArray(OperateFailure.szErrorDescribe));
				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));

				return true;
			}
			if (pGroupItem->GetGroupProperty()->cbGroupStatus != 1)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,�������Ѷ��ᣡ", CountArray(OperateFailure.szErrorDescribe));
				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));

				return true;
			}
			tagGroupConfigInfo *pConfig = NULL;
			pConfig = pGroupItem->SearchConfig(pCreate->dwConfigID);
			if (pConfig == NULL)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,�淨�����ڣ����飡", CountArray(OperateFailure.szErrorDescribe));
				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));

				return true;
			}

			if (pConfig->cbPlayMode == 1 && pGroupItem->GetGroupBattleRule()->cbMedalMode != 1)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"��Ǹ,�޷�ʹ��ѫ���淨�����飡", CountArray(OperateFailure.szErrorDescribe));
				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));

				return true;
			}
			SUB_GA_QueryCreateResult CreateResult;
			ZeroMemory(&CreateResult, sizeof(CreateResult));
			CreateResult.dwGroupID = pCreate->dwGroupID;
			CreateResult.dwConfigID = pCreate->dwConfigID;
			CreateResult.nMaxNullTableCount = pGroupItem->GetGroupBattleRule()->cbFreeTableLimit;

			if (pGroupItem->GetNullTableCount(pCreate->dwConfigID) < CreateResult.nMaxNullTableCount)
			{
				CreateResult.bCanCreate = true;
			}
			else//����һ�����乩�Զ�����
			{
				tagClubTableInfo TableInfo;
				POSITION tablepos = NULL;
				do
				{
					//�����������п�λ����
					TableInfo = pGroupItem->EnumTable(tablepos);
					if (TableInfo.dwServerID != 0 && TableInfo.dwConfigID==pCreate->dwConfigID && TableInfo.bGameStart==false)
					{
						for (int i = 0; i < TableInfo.wJoinGamePeopleCount; i++)
						{
							if (TableInfo.dwPlayerID[i] == 0)
							{
								CreateResult.dwRoomID = TableInfo.dwPersonalRoomID;
								m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_QUERY_CREATE_RESULT, &CreateResult, sizeof(CreateResult));

								return true;
							}
						}
					}

				} while (tablepos != NULL);
			}

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_QUERY_CREATE_RESULT, &CreateResult, sizeof(CreateResult));

			return true;
		}
	case SUB_GA_QUERY_MEDALMODE:
	{
		//����У��
		/*
		//ASSERT(wDataSize == sizeof(SUB_GA_QueryMedalMode));
		if (wDataSize != sizeof(SUB_GA_QueryMedalMode)) return false;

		//��ȡ����
		SUB_GA_QueryMedalMode * pPresentScore = (SUB_GA_QueryMedalMode *)pData;

		DBR_GR_QueryMedalMode QueryMode;
		QueryMode.dwGroupID = pPresentScore->dwGroupID;
		QueryMode.dwUserID = pPresentScore->dwUserID;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_MEDAL_MODE, dwSocketID, &QueryMode, sizeof(QueryMode));
		*/
		return true;
	}
	case SUB_GA_SET_ROLLMSG:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_SetRollMsg));
			if (wDataSize != sizeof(SUB_GA_SetRollMsg)) return false;

			//��ȡ����
			SUB_GA_SetRollMsg * pMsg = (SUB_GA_SetRollMsg *)pData;

			
			DBR_GR_SetRollMsg RollMsg;
			RollMsg.dwGroupID = pMsg->dwGroupID;
			RollMsg.dwUserID = pMsg->dwUserID;
			RollMsg.cbFlag = pMsg->cbFlag;
			CopyMemory(RollMsg.szRollMsg, pMsg->szRollMsg, sizeof(RollMsg.szRollMsg));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_ROLLMSG, dwSocketID, &RollMsg, sizeof(RollMsg));
			
			return true;
		}
	case SUB_GA_EDIT_NOTICE:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_SetNotice));
			if (wDataSize != sizeof(SUB_GA_SetNotice)) return false;

			//��ȡ����
			SUB_GA_SetNotice * pNotice = (SUB_GA_SetNotice *)pData;

			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pNotice->dwGroupID);
			if (pGroupItem == NULL)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"��������ʧ�ܣ���������Ϣ�����ڣ�", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;
			}
			
			tagIMGroupMember * pMember = pGroupItem->SearchMember(pNotice->dwUserID);
			if (pMember == NULL)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"��������ʧ�ܣ�Ȩ�޲��㣡", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;
			}

			bool bHaveRight = false;
			if (pMember->nMemberRight & GP_CREATOR)
				bHaveRight = true;
			if ((pMember->nMemberRight & GP_MANAGER) && pGroupItem->GetGroupBattleRule()->lManagerRight&MR_3)
				bHaveRight = true;
			if (bHaveRight == false)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"��������ʧ�ܣ�Ȩ�޲��㣡", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;
			}

			pGroupItem->SetNotice(pNotice->szNotice);

			//��������
			SUB_GA_GroupNotice Notice;
			ZeroMemory(&Notice, sizeof(Notice));
			Notice.dwGroupID = pNotice->dwGroupID;
			CopyMemory(Notice.szNotice, pNotice->szNotice, sizeof(Notice.szNotice));
			SendDataToGroupMember(pNotice->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_NOTICE, &Notice, sizeof(Notice), true);

			return true;
		}
	case SUB_GA_SET_GROUP_STATUS:		//����Ⱥ��״̬
		{
			//����У��
			//ASSERT(wDataSize <= sizeof(SUB_GA_SetGroupStatus));
			if (wDataSize > sizeof(SUB_GA_SetGroupStatus)) return false;

			//��ȡ����
			SUB_GA_SetGroupStatus * pStatus = (SUB_GA_SetGroupStatus *)pData;

			//����ṹ
			DBR_GR_SetGroupStatus GroupStatus;
			ZeroMemory(&GroupStatus, sizeof(GroupStatus));

			GroupStatus.dwGroupID = pStatus->dwGroupID;
			GroupStatus.dwCreateID = pStatus->dwCreateID;
			lstrcpynW(GroupStatus.szLogonPass, pStatus->szLogonPass, CountArray(GroupStatus.szLogonPass));
			GroupStatus.cbStatus = pStatus->cbStatus;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_GROUP_STATUS, dwSocketID, &GroupStatus, sizeof(GroupStatus));

			return true;
		}
	case SUB_GA_SET_INOUT:		//��������
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_SetInOut));
			if (wDataSize != sizeof(SUB_GA_SetInOut)) return false;

			//��ȡ����
			SUB_GA_SetInOut * pSet = (SUB_GA_SetInOut *)pData;
			//����Ⱥ��
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pSet->dwGroupID);
			if (pGroupItem != NULL && pGroupItem->GetGroupProperty()->cbGroupStatus != 1)
			{
				SUB_GA_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));
				lstrcpynW(OperateFailure.szErrorDescribe, L"����ʧ�ܣ��ô������Ѷ��ᣡ", CountArray(OperateFailure.szErrorDescribe));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
				return true;
			}

			DBR_GR_SetInOut SetInOut;
			SetInOut.dwGroupID = pSet->dwGroupID;
			SetInOut.dwUserID = pSet->dwUserID;
			SetInOut.cbInMode = pSet->cbInMode;
			SetInOut.cbInType = pSet->cbInType;
			SetInOut.cbOutMode = pSet->cbOutMode;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_INOUT, dwSocketID, &SetInOut, sizeof(SetInOut));

			return true;
		}
	case SUB_GA_QUERY_RIGHT:		//��ѯȨ��
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_QueryRight));
			if (wDataSize != sizeof(SUB_GA_QueryRight)) return false;

			//��ȡ����
			SUB_GA_QueryRight * pQuery = (SUB_GA_QueryRight *)pData;

			//����ṹ
			DBR_GR_QueryRight QueryRight;
			ZeroMemory(&QueryRight, sizeof(QueryRight));

			QueryRight.dwGroupID = pQuery->dwGroupID;
			QueryRight.dwUserID = pQuery->dwUserID;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_RIGHT_QUERY, dwSocketID, &QueryRight, sizeof(QueryRight));

			return true;
		}
	case SUB_GA_RIGHT_MANAGEMENT:		//Ȩ�޹���
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_RightManagement));
			if (wDataSize != sizeof(SUB_GA_RightManagement)) return false;

			//��ȡ����
			SUB_GA_RightManagement * pManagement = (SUB_GA_RightManagement *)pData;
			
			//����ṹ
			DBR_GR_RightManagement Management;
			ZeroMemory(&Management, sizeof(Management));

			Management.dwGroupID = pManagement->dwGroupID;
			Management.dwUserID = pManagement->dwUserID;
			Management.dwTargetID = pManagement->dwTargetID;
			for (int i = 0; i < 30; i++)
			{
				if (g_lManagerRight[i] >0 && pManagement->cbManagerRight[i])
					Management.lManagerRight |= g_lManagerRight[i];

				if (g_lCaptainRight[i] > 0 && pManagement->cbCaptainRight[i])
					Management.lCaptainRight |= g_lCaptainRight[i];
			}

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_RIGHT_MANAGEMENT, dwSocketID, &Management, sizeof(Management));

			return true;
		}
	case SUB_GA_SET_MEDAL:		//ѫ������
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_SetMedal));
			if (wDataSize != sizeof(SUB_GA_SetMedal)) return false;

			//��ȡ����
			SUB_GA_SetMedal * pSetMedal = (SUB_GA_SetMedal *)pData;

			//����ṹ
			DBR_GR_SetMedal SetMedal;
			ZeroMemory(&SetMedal, sizeof(SetMedal));

			SetMedal.dwGroupID = pSetMedal->dwGroupID;
			SetMedal.dwUserID = pSetMedal->dwUserID;
			SetMedal.cbMedalMode = pSetMedal->cbMedalMode;
			SetMedal.cbMedalShow = pSetMedal->cbMedalShow;
			SetMedal.cbCurMedalShow = pSetMedal->cbCurMedalShow;
			SetMedal.cbMedalRevenueShow = pSetMedal->cbMedalRevenueShow;
			SetMedal.cbMedalClear = pSetMedal->cbMedalClear;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_MEDAL, dwSocketID, &SetMedal, sizeof(SetMedal));

			return true;
		}
	case SUB_GA_SET_FREE_TABLE:		//��������
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_SetFreeTable));
			if (wDataSize != sizeof(SUB_GA_SetFreeTable)) return false;

			//��ȡ����
			SUB_GA_SetFreeTable * pLimit = (SUB_GA_SetFreeTable *)pData;

			//����ṹ
			DBR_GR_SetFreeTableLimit Limit;
			ZeroMemory(&Limit, sizeof(Limit));
			Limit.dwGroupID = pLimit->dwGroupID;
			Limit.dwUserID = pLimit->dwUserID;
			Limit.cbCount = pLimit->cbFreeTableLimit;
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_FREETABLE_LIMIT, dwSocketID, &Limit, sizeof(Limit));

			return true;
		}
	case SUB_GA_SET_BIGWINNER:		//��Ӯ������
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_SetBigWinner));
			if (wDataSize != sizeof(SUB_GA_SetBigWinner)) return false;

			//��ȡ����
			SUB_GA_SetBigWinner * pSetBigWinner = (SUB_GA_SetBigWinner *)pData;

			//����ṹ
			DBR_GR_SetBigWinner SetBig;
			ZeroMemory(&SetBig, sizeof(SetBig));
			SetBig.dwGroupID = pSetBigWinner->dwGroupID;
			SetBig.dwUserID = pSetBigWinner->dwUserID;
			SetBig.cbShow = pSetBigWinner->cbShow;
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_BIGWINNER_SHOW, dwSocketID, &SetBig, sizeof(SetBig));

			return true;
		}
	case SUB_GA_SET_FLAG:		//���õ��ޱ��
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_SetFlag));
			if (wDataSize != sizeof(SUB_GA_SetFlag)) return false;

			//��ȡ����
			SUB_GA_SetFlag * pSet = (SUB_GA_SetFlag *)pData;

			//����ṹ
			DBR_GR_SetFlag SetFlag;
			ZeroMemory(&SetFlag, sizeof(SetFlag));
			SetFlag.dwGroupID = pSet->dwGroupID;
			SetFlag.dwOperateUserID = pSet->dwOperateUserID;
			SetFlag.dwTargetUserID = pSet->dwTargetUserID;
			SetFlag.dwDateID = pSet->dwDateID;
			SetFlag.cbFlag = pSet->cbFlag;
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_FLAG, dwSocketID, &SetFlag, sizeof(SetFlag));

			return true;
		}
	case SUB_GA_DISMISS_CLUB_TABLE:
		{
			//����У��
			//ASSERT(wDataSize == sizeof(SUB_GA_ApplyDismissClubTable));
			if (wDataSize != sizeof(SUB_GA_ApplyDismissClubTable)) return false;

			//��ȡ����
			SUB_GA_ApplyDismissClubTable * pDismissTable = (SUB_GA_ApplyDismissClubTable *)pData;

			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pDismissTable->dwGroupID);
			if (pGroupItem)
			{
				tagIMGroupMember * pMember = pGroupItem->SearchMember(pDismissTable->dwUserID);
				if (pMember)
				{
					if ( (pMember->nMemberRight&GP_CREATOR)
						|| ((pMember->nMemberRight & GP_MANAGER) != 0 && (pGroupItem->GetGroupBattleRule()->lManagerRight&MR_7) != 0))
					{
						tagClubTableInfo TableInfo;
						POSITION tablepos = NULL;
						do
						{
							//��������������
							TableInfo = pGroupItem->EnumTable(tablepos);
							if (TableInfo.dwServerID != 0 && TableInfo.dwPersonalRoomID == pDismissTable->dwRoomID)
							{
								//֪ͨԼս��ɢ����
								CMD_CS_C_DismissTable pack;
								ZeroMemory(&pack, sizeof(pack));
								pack.dwSocketID = dwSocketID;
								pack.dwServerID = TableInfo.dwServerID;
								pack.dwTableID = TableInfo.wTableID;
								m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &pack, sizeof(CMD_CS_C_DismissTable));

								return true;
							}

						} while (tablepos != NULL);

						SUB_GA_OperateFailure OperateFailure;
						ZeroMemory(&OperateFailure, sizeof(OperateFailure));
						lstrcpynW(OperateFailure.szErrorDescribe, _T("��ɢʧ�ܣ������Ӳ����ڣ�"), CountArray(OperateFailure.szErrorDescribe));
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
					}
					else
					{
						SUB_GA_OperateFailure OperateFailure;
						ZeroMemory(&OperateFailure, sizeof(OperateFailure));
						lstrcpynW(OperateFailure.szErrorDescribe, _T("��ɢʧ�ܣ�Ȩ�޲�����"), CountArray(OperateFailure.szErrorDescribe));
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
					}

				} 

			}

			SUB_GA_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));
			lstrcpynW(OperateFailure.szErrorDescribe, _T("��ɢʧ�ܣ�������Ϣ��"), CountArray(OperateFailure.szErrorDescribe));
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));


			return true;
		}
	case SUB_GA_SET_FAVORITE_CONFIG:		//�����ղ��淨
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_SetFavoriteConfig));
		if (wDataSize != sizeof(SUB_GA_SetFavoriteConfig)) return false;

		//��ȡ����
		SUB_GA_SetFavoriteConfig * pSet = (SUB_GA_SetFavoriteConfig *)pData;

		//����ṹ
		DBR_GR_SetFavoriteConfig SetFlag;
		ZeroMemory(&SetFlag, sizeof(SetFlag));
		SetFlag.dwGroupID = pSet->dwGroupID;
		SetFlag.dwMemberID = pSet->dwMemberID;
		SetFlag.dwConfigID = pSet->dwConfigID;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_FAVORITE_CONFIG, dwSocketID, &SetFlag, sizeof(SetFlag));

		return true;
	}
	case SUB_GA_MODIFY_GROUP_NAME:		//�޸Ĵ���������
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_ModifyGroupName));
		if (wDataSize != sizeof(SUB_GA_ModifyGroupName)) return false;

		//��ȡ����
		SUB_GA_ModifyGroupName * pSet = (SUB_GA_ModifyGroupName *)pData;

		//����ṹ
		DBR_GR_ModifyGroupName ModifyName;
		ZeroMemory(&ModifyName, sizeof(ModifyName));
		ModifyName.dwGroupID = pSet->dwGroupID;
		ModifyName.dwUserID = pSet->dwUserID;
		CopyMemory(ModifyName.szGroupName, pSet->szGroupName, sizeof(ModifyName.szGroupName));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_GROUP_NAME, dwSocketID, &ModifyName, sizeof(ModifyName));

		return true;
	}
	case SUB_GA_QUERY_ROOM_DETAIL:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_QueryRoomDetail));
		if (wDataSize != sizeof(SUB_GA_QueryRoomDetail)) return false;

		//��ȡ����
		SUB_GA_QueryRoomDetail * pQuery = (SUB_GA_QueryRoomDetail *)pData;

		//����ṹ
		DBR_GR_QueryRoomDetail QueryRoom;
		ZeroMemory(&QueryRoom, sizeof(QueryRoom));
		QueryRoom.dwGroupID = pQuery->dwGroupID;
		QueryRoom.dwRoomID = pQuery->dwRoomID;
		QueryRoom.dwDateID = pQuery->dwDateID;
		QueryRoom.dwUserID = pQuery->dwUserID;
		QueryRoom.EndTime = pQuery->EndTime;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_ROOM_DETAIL, dwSocketID, &QueryRoom, sizeof(QueryRoom));

		return true;
	}
	case SUB_GA_QUERY_CAPTAIN_LIST:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_QueryCaptain));
		if (wDataSize != sizeof(SUB_GA_QueryCaptain)) return false;

		//��ȡ����
		SUB_GA_QueryCaptain * pQuery = (SUB_GA_QueryCaptain *)pData;

		//����ṹ
		DBR_GR_QueryCaptain QueryCaptain;
		ZeroMemory(&QueryCaptain, sizeof(QueryCaptain));
		QueryCaptain.dwGroupID = pQuery->dwGroupID;
		QueryCaptain.dwUserID = pQuery->dwUserID;
		QueryCaptain.dwCaptainID = pQuery->dwCaptainID;
		QueryCaptain.dwDateID = pQuery->dwDateID;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_CAPTAIN_LIST, dwSocketID, &QueryCaptain, sizeof(QueryCaptain));
		return true;
	}
	case SUB_GA_ADD_CAPTAIN_TEAMER:
	{
		//����У��
		//ASSERT(wDataSize > sizeof(SUB_GA_AddTeamer));
		if (wDataSize <= sizeof(SUB_GA_AddTeamer)) return false;

		//��ȡ����
		SUB_GA_AddTeamer * pAddTeamer = (SUB_GA_AddTeamer *)pData;

		////����ṹ
		//DBR_GR_AddTeamer AddTeamer;
		//ZeroMemory(&AddTeamer, sizeof(AddTeamer));
		//AddTeamer.dwGroupID = pAddTeamer->dwGroupID;
		//AddTeamer.dwUserID = pAddTeamer->dwUserID;
		//AddTeamer.dwCaptainID = pAddTeamer->dwCaptainID;
		//AddTeamer.cbMode = pAddTeamer->cbMode;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_ADD_CAPTAIN_TEAMER, dwSocketID, pData, wDataSize);
		return true;
	}
	case SUB_GA_QUERY_TEAM_RECORD:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_QueryTeamDayRecord));
		if (wDataSize != sizeof(SUB_GA_QueryTeamDayRecord)) return false;

		//��ȡ����
		SUB_GA_QueryTeamDayRecord * pQuery = (SUB_GA_QueryTeamDayRecord *)pData;

		//����ṹ
		DBR_GR_QueryTeamDayRecord QueryRecord;
		ZeroMemory(&QueryRecord, sizeof(QueryRecord));

		//���ñ���
		QueryRecord.dwGroupID = pQuery->dwGroupID;
		QueryRecord.dwQueryUserID = pQuery->dwQueryUserID;
		QueryRecord.dwDateID = pQuery->dwDateID;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_TEAM_RECORD, dwSocketID, &QueryRecord, sizeof(QueryRecord));

		return true;

	}
	case SUB_GA_QUERY_TEAMER_RECORD:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_QueryTeamerDayRecord));
		if (wDataSize != sizeof(SUB_GA_QueryTeamerDayRecord)) return false;

		//��ȡ����
		SUB_GA_QueryTeamerDayRecord * pQuery = (SUB_GA_QueryTeamerDayRecord *)pData;

		//����ṹ
		DBR_GR_QueryTeamerDayRecord QueryRecord;
		ZeroMemory(&QueryRecord, sizeof(QueryRecord));

		//���ñ���
		QueryRecord.dwGroupID = pQuery->dwGroupID;
		QueryRecord.dwCaptainID = pQuery->dwCaptainID;
		QueryRecord.dwQueryUserID = pQuery->dwQueryUserID;
		QueryRecord.dwDateID = pQuery->dwDateID;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_TEAMER_RECORD, dwSocketID, &QueryRecord, sizeof(QueryRecord));

		return true;

	}
	case SUB_GA_SET_PRIVACY:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_SetPrivacy));
		if (wDataSize != sizeof(SUB_GA_SetPrivacy)) return false;

		//��ȡ����
		SUB_GA_SetPrivacy * pPrivacy = (SUB_GA_SetPrivacy *)pData;

		//����ṹ
		DBR_GR_SetPrivacy Privacy;
		ZeroMemory(&Privacy, sizeof(Privacy));

		//���ñ���
		Privacy.dwGroupID = pPrivacy->dwGroupID;
		Privacy.dwUserID = pPrivacy->dwUserID;
		Privacy.cbShowGroupID = pPrivacy->cbShowGroupID;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_PRIVACY, dwSocketID, &pPrivacy, sizeof(pPrivacy));

		return true;
	}
	case SUB_GA_SET_CAPTAIN_POINT:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_SetCaptainPoint));
		if (wDataSize != sizeof(SUB_GA_SetCaptainPoint)) return false;

		//��ȡ����
		SUB_GA_SetCaptainPoint * pCaptainPoint = (SUB_GA_SetCaptainPoint *)pData;

		//����ṹ
		DBR_GR_SetCaptainPoint CaptainPoint;
		ZeroMemory(&CaptainPoint, sizeof(CaptainPoint));

		//���ñ���
		CaptainPoint.dwGroupID = pCaptainPoint->dwGroupID;
		CaptainPoint.dwUserID = pCaptainPoint->dwUserID;
		CaptainPoint.dwCaptainID = pCaptainPoint->dwCaptainID;
		CaptainPoint.wPoint = pCaptainPoint->wPoint;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_CAPTAIN_POINT, dwSocketID, &CaptainPoint, sizeof(CaptainPoint));

		return true;
	}
	case SUB_GA_OPEN_BOX:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_OpenBox));
		if (wDataSize != sizeof(SUB_GA_OpenBox)) return false;

		//��ȡ����
		SUB_GA_OpenBox * pOpenBox = (SUB_GA_OpenBox *)pData;

		//����ṹ
		DBR_GR_OpenBox OpenBox;
		ZeroMemory(&OpenBox, sizeof(OpenBox));

		//���ñ���
		OpenBox.dwGroupID = pOpenBox->dwGroupID;
		OpenBox.dwUserID = pOpenBox->dwUserID;
		CopyMemory(OpenBox.szPassword, pOpenBox->szPassword, sizeof(OpenBox.szPassword));
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_OPEN_BOX, dwSocketID, &OpenBox, sizeof(OpenBox));

		return true;
	}
	case SUB_GA_GET_AWARDSCORE:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_GetAwardScore));
		if (wDataSize != sizeof(SUB_GA_GetAwardScore)) return false;

		//��ȡ����
		SUB_GA_GetAwardScore * pGetScore = (SUB_GA_GetAwardScore *)pData;

		//����ṹ
		DBR_GR_GetAwardScore GetScore;
		ZeroMemory(&GetScore, sizeof(GetScore));

		//���ñ���
		GetScore.dwGroupID = pGetScore->dwGroupID;
		GetScore.dwUserID = pGetScore->dwUserID;
		GetScore.lScore = pGetScore->lScore;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_GET_AWARD_SCORE, dwSocketID, &GetScore, sizeof(GetScore));

		return true;
	}
	case SUB_GA_QUERY_INVITATION_CODE:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_QueryInvatationCode));
		if (wDataSize != sizeof(SUB_GA_QueryInvatationCode)) return false;

		//��ȡ����
		SUB_GA_QueryInvatationCode * pQuery = (SUB_GA_QueryInvatationCode *)pData;

		//����ṹ
		DBR_GR_QueryCode QueryCode;
		ZeroMemory(&QueryCode, sizeof(QueryCode));

		//���ñ���
		QueryCode.dwGroupID = pQuery->dwGroupID;
		QueryCode.dwUserID = pQuery->dwUserID;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_CODE, dwSocketID, &QueryCode, sizeof(QueryCode));

		return true;
	}
	case SUB_GA_BLIND_INVITATION_CODE:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_BlindInvatationCode));
		if (wDataSize != sizeof(SUB_GA_BlindInvatationCode)) return false;

		//��ȡ����
		SUB_GA_BlindInvatationCode * pBlind = (SUB_GA_BlindInvatationCode *)pData;

		//����ṹ
		DBR_GR_BlindCode BlindCode;
		ZeroMemory(&BlindCode, sizeof(BlindCode));

		//���ñ���
		BlindCode.dwGroupID = pBlind->dwGroupID;
		BlindCode.dwUserID = pBlind->dwUserID;
		BlindCode.lCode = pBlind->lInvitationCode;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_BLIND_CODE, dwSocketID, &BlindCode, sizeof(BlindCode));

		return true;
	}
	case SUB_GA_QUERY_SCORE:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_QueryScore));
		if (wDataSize != sizeof(SUB_GA_QueryScore)) return false;

		//��ȡ����
		SUB_GA_QueryScore * pQuery = (SUB_GA_QueryScore *)pData;

		//����ṹ
		DBR_GR_QueryScore QueryScore;
		ZeroMemory(&QueryScore, sizeof(QueryScore));

		//���ñ���
		QueryScore.dwGroupID = pQuery->dwGroupID;
		QueryScore.dwUserID = pQuery->dwUserID;
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_SCORE, dwSocketID, &QueryScore, sizeof(QueryScore));

		return true;
	}
	case SUB_GA_QUERY_ROOM_BATTLE_INFO:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_QueryTableBattleInfo));
		if (wDataSize != sizeof(SUB_GA_QueryTableBattleInfo)) return false;

		//��ȡ����
		SUB_GA_QueryTableBattleInfo * pQuery = (SUB_GA_QueryTableBattleInfo *)pData;

		if (CheckClubUser(pQuery->dwGroupID, pBindParameter->dwUserID) == false) return true;

		//��Լս����������Ϣ
		if (m_pITCPSocketBattle)
		{
			CMD_CS_C_QueryTableInfo QueryInfo;
			ZeroMemory(&QueryInfo, sizeof(QueryInfo));
			QueryInfo.dwSocketID = dwSocketID;
			QueryInfo.dwGroupID = pQuery->dwGroupID;
			QueryInfo.dwServerID = pQuery->dwServerID;
			QueryInfo.dwTableID = pQuery->dwTableID;
			m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_ROOM_BATTLE_INFO, &QueryInfo, sizeof(QueryInfo));
		}

		return true;
	}
	case SUB_GA_MODIFY_INFO:
	{
		//����У��
		//ASSERT(wDataSize == sizeof(SUB_GA_Modify_Info));
		if (wDataSize != sizeof(SUB_GA_Modify_Info)) return false;

		//��ȡ����
		SUB_GA_Modify_Info * pModifyInfo = (SUB_GA_Modify_Info *)pData;

		//����ṹ
		DBR_GR_Modify_Info ModifyInfo;
		ZeroMemory(&ModifyInfo, sizeof(ModifyInfo));

		ModifyInfo.dwGroupID = pModifyInfo->dwGroupID;
		ModifyInfo.dwUserID = pModifyInfo->dwUserID;
		CopyMemory(&ModifyInfo.cbFlag, pModifyInfo->cbFlag, sizeof(ModifyInfo.cbFlag));
		CopyMemory(&ModifyInfo.szQQ, pModifyInfo->szQQ, sizeof(ModifyInfo.szQQ));
		CopyMemory(&ModifyInfo.szWX, pModifyInfo->szWX, sizeof(ModifyInfo.szWX));
		CopyMemory(&ModifyInfo.szPhone, pModifyInfo->szPhone, sizeof(ModifyInfo.szPhone));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_INFO, dwSocketID, &ModifyInfo, sizeof(ModifyInfo));

		return true;
	}
	}

	return true;
}

//Ⱥ�����
bool CAttemperEngineSink::OnTCPNetworkMainGroupService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ����
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	switch (wSubCmdID)
	{
	case SUB_GA_ENTER_GROUP:		//����Ⱥ��
		{
			//����У��
			//ASSERT(wDataSize <= sizeof(SUB_GA_EnterGroup));
			if (wDataSize>sizeof(SUB_GA_EnterGroup)) return false;

			//��ȡ����
			SUB_GA_EnterGroup * pEnterGroup = (SUB_GA_EnterGroup *)pData;

			//����Ⱥ��
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pEnterGroup->dwGroupID);
			if (pGroupItem == NULL)
			{
				//����ṹ
				SUB_GA_EnterFailure EnterFailure;
				ZeroMemory(&EnterFailure, sizeof(EnterFailure));

				//���ñ���
				EnterFailure.lErrorCode = LOGON_ECODE_FAILED;
				lstrcpynW(EnterFailure.szErrorDescribe, L"ϵͳδ��ѯ���ô����˵���Ϣ������ʧ�ܣ�", CountArray(EnterFailure.szErrorDescribe));

				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_ENTER_FAILURE, &EnterFailure, sizeof(EnterFailure));

				return true;
			}
			/*
			if (pGroupItem->GetGroupProperty()->cbGroupStatus !=1)
			{
				//����ṹ
				SUB_GA_EnterFailure EnterFailure;
				ZeroMemory(&EnterFailure, sizeof(EnterFailure));

				//���ñ���
				EnterFailure.lErrorCode = LOGON_ECODE_FAILED;
				lstrcpynW(EnterFailure.szErrorDescribe, L"�ô����˴��ڶ���״̬������ʧ�ܣ�", CountArray(EnterFailure.szErrorDescribe));

				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_ENTER_FAILURE, &EnterFailure, sizeof(EnterFailure));

				return true;
			}
			*/
			//���ҳ�Ա
			tagIMGroupMember *pMember = pGroupItem->SearchMember(pEnterGroup->dwUserID);
			if (pMember == NULL)
			{
				//����ṹ
				SUB_GA_EnterFailure EnterFailure;
				ZeroMemory(&EnterFailure, sizeof(EnterFailure));

				//���ñ���
				EnterFailure.lErrorCode = LOGON_ECODE_FAILED;
				lstrcpynW(EnterFailure.szErrorDescribe, L"��Ǹ���㲻�Ǹô����˵ĳ�Ա������ʧ�ܣ�", CountArray(EnterFailure.szErrorDescribe));

				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_ENTER_FAILURE, &EnterFailure, sizeof(EnterFailure));

				return true;
			}

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai **** �ٴ����� dwGroupID = %d ���ҵ� userid = %d �����"), pEnterGroup->dwGroupID, pEnterGroup->dwUserID);
			//OutputDebugString(szInfo);

			//�����û�
			CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pEnterGroup->dwUserID);
			if (pServerUserItem != NULL)
			{
				//����Ⱥ��
				pServerUserItem->SetActiveGroupID(pEnterGroup->dwGroupID);

				//���ñ���
				SUB_GA_GroupItem GroupItem;
				ZeroMemory(&GroupItem, sizeof(SUB_GA_GroupItem));
				
				CopyMemory(&GroupItem, pGroupItem->GetGroupItem(), sizeof(tagIMGroupItem));

				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_ITEM, &GroupItem, sizeof(tagIMGroupItem));

				//���ͳ�Ա
				SendGroupMemberToUser(pEnterGroup->dwGroupID, dwSocketID);

				//�����淨
				SendGroupConfigToUser(pEnterGroup->dwGroupID, dwSocketID);

				////���͹���
				SUB_GA_GroupNotice Notice;
				ZeroMemory(&Notice, sizeof(Notice));
				Notice.dwGroupID = pGroupItem->GetGroupID();
				CopyMemory(Notice.szNotice, pGroupItem->GetNotice(), sizeof(Notice.szNotice));
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_NOTICE, &Notice, sizeof(Notice));

				//���������
				SUB_GA_RollMsg RollMsg;
				ZeroMemory(&RollMsg, sizeof(RollMsg));
				RollMsg.dwGroupID = pGroupItem->GetGroupID();

				//Ȧ�������
				tagClubRollMsg *pRollMsg = pGroupItem->SearchRollMsg(pGroupItem->GetGroupProperty()->dwCreaterID);
				if (pRollMsg != NULL)
				{
					RollMsg.cbMsgType = 1;
					RollMsg.nTime = pRollMsg->nRollMsgTime;
					RollMsg.cbFlag = pRollMsg->cbRollMsgFlag;
					CopyMemory(RollMsg.szRollMsg, pRollMsg->szRollMsg, sizeof(RollMsg.szRollMsg));
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_ROLL_MSG, &RollMsg, sizeof(RollMsg));
				}

				//�ӳ������
				if (pMember->dwCaptainID != 0)
				{
					pRollMsg = pGroupItem->SearchRollMsg(pMember->dwCaptainID);
					if (pRollMsg != NULL)
					{
						RollMsg.cbMsgType = 2;
						RollMsg.nTime = pRollMsg->nRollMsgTime;
						RollMsg.cbFlag = pRollMsg->cbRollMsgFlag;
						CopyMemory(RollMsg.szRollMsg, pRollMsg->szRollMsg, sizeof(RollMsg.szRollMsg));
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_ROLL_MSG, &RollMsg, sizeof(RollMsg));
					}
				}

				//�Լ������
				if (pMember->nMemberRight == GP_CAPTAIN)
				{
					pRollMsg = pGroupItem->SearchRollMsg(pMember->dwMemberID);
					if (pRollMsg != NULL)
					{
						RollMsg.cbMsgType = 3;
						RollMsg.nTime = pRollMsg->nRollMsgTime;
						RollMsg.cbFlag = pRollMsg->cbRollMsgFlag;
						CopyMemory(RollMsg.szRollMsg, pRollMsg->szRollMsg, sizeof(RollMsg.szRollMsg));
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_ROLL_MSG, &RollMsg, sizeof(RollMsg));
					}
				}

				//���͹���ԱȨ��
				SUB_GA_ManagerRight ManagerRight;
				ZeroMemory(&ManagerRight, sizeof(ManagerRight));
				ManagerRight.dwGroupID = pGroupItem->GetGroupID();
				ManagerRight.lManagerRight = pGroupItem->GetGroupBattleRule()->lManagerRight;
				ManagerRight.lCaptainRight = pGroupItem->GetGroupBattleRule()->lCaptainRight;

				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_MANAGER_RIGHT, &ManagerRight, sizeof(ManagerRight));
			}

			//��������
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_ENTER_SUCCESS);

			//�����������״̬
			DBR_GR_UpdateOnLineStatus UpdateOnLineStatus;
			ZeroMemory(&UpdateOnLineStatus, sizeof(UpdateOnLineStatus));

			//���ñ���
			UpdateOnLineStatus.dwGroupID = pEnterGroup->dwGroupID;
			UpdateOnLineStatus.dwUserID = pEnterGroup->dwUserID;
			UpdateOnLineStatus.cbStatus = 1;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_ONLINE_STATUS, dwSocketID, &UpdateOnLineStatus, sizeof(UpdateOnLineStatus));

			return true;
		}
	case SUB_GA_LEAVE_GROUP:		//�뿪Ⱥ��
		{
			//����У��
			//ASSERT(wDataSize <= sizeof(SUB_GA_LeaveGroup));
			if (wDataSize>sizeof(SUB_GA_LeaveGroup)) return false;

			//��ȡ����
			SUB_GA_LeaveGroup * pLeaveGroup = (SUB_GA_LeaveGroup *)pData;

			//�����û�
			CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pLeaveGroup->dwUserID);
			if (pServerUserItem != NULL && pServerUserItem->GetActiveGroupID()==pLeaveGroup->dwGroupID)
			{
				//����Ⱥ��
				pServerUserItem->SetActiveGroupID(0);
			}

			//���ñ�ʶ
			pBindParameter->wGroupID = 0;

			//�����������״̬
			DBR_GR_UpdateOnLineStatus UpdateOnLineStatus;
			ZeroMemory(&UpdateOnLineStatus, sizeof(UpdateOnLineStatus));

			//���ñ���
			UpdateOnLineStatus.dwGroupID = pLeaveGroup->dwGroupID;
			UpdateOnLineStatus.dwUserID = pLeaveGroup->dwUserID;
			UpdateOnLineStatus.cbStatus = 0;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_ONLINE_STATUS, dwSocketID, &UpdateOnLineStatus, sizeof(UpdateOnLineStatus));

			return true;
		}
	}

	return true;
}

//Լս����
bool CAttemperEngineSink::OnTCPNetworkMainBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
 {
	//��Լս��������������Ϣ
	if (m_pITCPSocketBattle==NULL)
	{
		//����ṹ
		CMD_GA_OperateFailed OperateFailed;
		ZeroMemory(&OperateFailed, sizeof(OperateFailed));

		//���ñ���
		OperateFailed.wErrorCode = 0;
		lstrcpynW(OperateFailed.szErrorMessage, L"��Ǹ��Լս������ʱ�ر�,�޷���ȡ��Ϣ��", CountArray(OperateFailed.szErrorMessage));

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_ENTER_FAILURE, &OperateFailed, sizeof(OperateFailed));

	}

	//��ȡ����
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

	//��ѯ�û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pBindParameter->dwUserID);
	if (pServerUserItem == NULL)  return false;

	//��Ϣ����
	switch (wSubCmdID)
	{
	case SUB_GA_QUERY_TABLE:				//��ȡ����
		{
			//����У��
			 //ASSERT(wDataSize == sizeof(CMD_GA_QueryTable));
			 if (wDataSize != sizeof(CMD_GA_QueryTable)) return false;

			//��ȡ����
			 CMD_GA_QueryTable * pTableList = (CMD_GA_QueryTable *)pData;

			//У���û�
			if (CheckClubUser(pTableList->dwGroupID, pBindParameter->dwUserID) == false) return true;

			//��Լս��������������Ϣ

			if (m_pITCPSocketBattle)
			{
				CMD_CS_C_QueryClubRoom QueryClubRoom;
				ZeroMemory(&QueryClubRoom, sizeof(CMD_CS_C_QueryClubRoom));
				QueryClubRoom.dwSocketID = pServerUserItem->GetSocketID();
				QueryClubRoom.dwGroupID = pTableList->dwGroupID;

				m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_CLUB_ROOM_LIST, &QueryClubRoom, sizeof(CMD_CS_C_QueryClubRoom));
			}

			return true;
		}
	case SUB_GA_GET_TABLE_USERLIST:			//�û��б�
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_GA_GetTableUserList));
			if (wDataSize != sizeof(CMD_GA_GetTableUserList)) return false;

			//��ȡ����
			CMD_GA_GetTableUserList * pTableUserList = (CMD_GA_GetTableUserList *)pData;

			//У���û�
			if (CheckClubUser(pTableUserList->dwGroupID, pBindParameter->dwUserID) == false) return true;
			//���Ҵ�����
			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pTableUserList->dwGroupID);
			if (pGroupItem == NULL) return true;
			//�����û���Ϣ
			tagClubTableInfo TableInfo;
			if (pGroupItem->GetTableInfo(pTableUserList->dwPersonalRoomID, TableInfo))
			{
				//����ṹ
				CMD_GA_TableUserList TableUserList;
				ZeroMemory(&TableUserList, sizeof(CMD_GA_TableUserList));

				//��������

				for (WORD i = 0; i < GROUP_MAX_CHAIR_GAME_CHAIR_COUNT; i++)
				{
					if (TableInfo.dwPlayerID[i] != INVALID_DWORD) TableUserList.wUserCount++;
				}
				CopyMemory(TableUserList.dwUserID, TableInfo.dwPlayerID, sizeof(TableUserList.dwUserID));

				//��������
				m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_BATTLE_SERVICE, SUB_GA_TABLE_USERLIST, &TableUserList, sizeof(TableUserList));

			}
			else
			{
				//����ṹ
				CMD_GA_OperateFailed OperateFailed;
				ZeroMemory(&OperateFailed, sizeof(OperateFailed));

				//���ñ���
				OperateFailed.wErrorCode = 0;
				lstrcpynW(OperateFailed.szErrorMessage, L"ϵͳδ��ѯ���÷������Ϣ����ȡʧ�ܣ�", CountArray(OperateFailed.szErrorMessage));

				//��������
				m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_BATTLE_SERVICE, SUB_GA_OPERATE_FAILED, &OperateFailed, sizeof(OperateFailed));
			}
			return true;
		}
	case SUB_GA_QUERY_GAME_SERVER:			//��ȡ�����б�
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_GA_QueryGameServer));
			if (wDataSize != sizeof(CMD_GA_QueryGameServer)) return false;

			//��ȡ����
			CMD_GA_QueryGameServer * pQueryGameServer = (CMD_GA_QueryGameServer *)pData;

			//У���û�
			if (CheckClubUser(pQueryGameServer->dwGroupID, pBindParameter->dwUserID) == false) return true;

			return true;
		}
	case SUB_GA_DISMISS_TABLE:				//��ɢ����
		{
				//����У��
				//ASSERT(wDataSize == sizeof(CMD_GA_DismissTable));
				if (wDataSize != sizeof(CMD_GA_DismissTable)) return false;

				//��ȡ����
				CMD_GA_DismissTable * pDismissTable = (CMD_GA_DismissTable *)pData;

				//��У��
				if (pBindParameter->dwUserID != pDismissTable->dwUserID) return false;

				pDismissTable->szPassword[CountArray(pDismissTable->szPassword) - 1] = 0; 

				//У������
				CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
				if (pLocalUserItem->ContrastLogonPass(pDismissTable->szPassword) == false)
				{
					return false;
				}

				//����Ⱥ��
				CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pDismissTable->dwGroupID);
				if (pGroupItem == NULL)
				{
					//����ṹ
					CMD_GA_OperateFailed OperateFailed;
					ZeroMemory(&OperateFailed, sizeof(OperateFailed));

					//���ñ���
					OperateFailed.wErrorCode = 0;
					lstrcpynW(OperateFailed.szErrorMessage, L"ϵͳδ��ѯ���ô����˵���Ϣ����ɢʧ�ܣ�", CountArray(OperateFailed.szErrorMessage));

					//��������
					m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_BATTLE_SERVICE, SUB_GA_OPERATE_FAILED, &OperateFailed, sizeof(OperateFailed));

					return true;
				}

				//Ⱥ��У��
				if (pGroupItem->GetGroupProperty()->dwCreaterID != pDismissTable->dwUserID)
				{
					//����ṹ
					CMD_GA_OperateFailed OperateFailed;
					ZeroMemory(&OperateFailed, sizeof(OperateFailed));

					//���ñ���
					OperateFailed.wErrorCode = 0;
					lstrcpynW(OperateFailed.szErrorMessage, L"�㲻�Ǹô�����Ⱥ������Ȩ��ɢ���䣡", CountArray(OperateFailed.szErrorMessage));

					//��������
					m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_BATTLE_SERVICE, SUB_GA_OPERATE_FAILED, &OperateFailed, sizeof(OperateFailed));

					return false;
				}

				//����У��

				//��Լս���ͽ�ɢ����
				if (m_pITCPSocketBattle)
				{
					//��������
					CMD_CS_C_SearchTable SearchTable;
					ZeroMemory(&SearchTable, sizeof(CMD_CS_C_SearchTable));

					SearchTable.dwSocketID = dwSocketID;

					//	m_pITCPSocketBattle->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISSUME_SEARCH_TABLE, &SearchTable, sizeof(CMD_CS_C_SearchTable));
				}
				return true;
		}
	}
	return false;
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
		case SUB_CS_S_SERVER_INFO:		//������Ϣ
		{
			//�����б�
			m_ServerListManager.DisuseServerItem();

			return true;
		}
		case SUB_CS_S_SERVER_INSERT:	//�������
		{
			//Ч�����
			//ASSERT(wDataSize%sizeof(tagGameServer) == 0);
			if (wDataSize%sizeof(tagGameServer) != 0) return false;

			//��������
			WORD wItemCount = wDataSize / sizeof(tagGameServer);
			tagGameServer * pGameServer = (tagGameServer *)pData;

			//��������
			for (WORD i = 0; i<wItemCount; i++)
			{
				m_ServerListManager.InsertGameServer(pGameServer++);
			}

			return true;
		}
	case SUB_CS_S_SERVER_MODIFY:	//�����޸�
		{
			//Ч�����
			//ASSERT(wDataSize == sizeof(CMD_CS_S_ServerModify));
			if (wDataSize != sizeof(CMD_CS_S_ServerModify)) return false;

			//��������
			CMD_CS_S_ServerModify * pServerModify = (CMD_CS_S_ServerModify *)pData;

			//���ҷ���
			//ASSERT(m_ServerListManager.SearchGameServer(pServerModify->wServerID));
			CGameServerItem * pGameServerItem = m_ServerListManager.SearchGameServer(pServerModify->wServerID);

			//���÷���
			if (pGameServerItem != NULL)
			{
				//��������
				DWORD dwOldOnlineCount = 0, dwOldAndroidCount = 0, dwOldFullCount = 0;
				dwOldAndroidCount = pGameServerItem->m_GameServer.dwAndroidCount;
				dwOldOnlineCount = pGameServerItem->m_GameServer.dwOnLineCount;
				dwOldFullCount = pGameServerItem->m_GameServer.dwFullCount;

				//�޸ķ�����Ϣ
				pGameServerItem->m_GameServer.wKindID = pServerModify->wKindID;
				pGameServerItem->m_GameServer.wNodeID = pServerModify->wNodeID;
				pGameServerItem->m_GameServer.wSortID = pServerModify->wSortID;
				pGameServerItem->m_GameServer.wServerPort = pServerModify->wServerPort;
				pGameServerItem->m_GameServer.dwOnLineCount = pServerModify->dwOnLineCount;
				pGameServerItem->m_GameServer.dwFullCount = pServerModify->dwFullCount;
				pGameServerItem->m_GameServer.dwAndroidCount = pServerModify->dwAndroidCount;
				pGameServerItem->m_GameServer.dwSetPlayerCount = pServerModify->dwSetPlayerCount;
				lstrcpyn(pGameServerItem->m_GameServer.szServerName, pServerModify->szServerName, CountArray(pGameServerItem->m_GameServer.szServerName));
				lstrcpyn(pGameServerItem->m_GameServer.szServerAddr, pServerModify->szServerAddr, CountArray(pGameServerItem->m_GameServer.szServerAddr));
				lstrcpyn(pGameServerItem->m_GameServer.szGameInfomation, pServerModify->szGameInfomation, CountArray(pGameServerItem->m_GameServer.szGameInfomation));
			}

			return true;
		}
	case SUB_CS_S_SERVER_REMOVE:	//����ɾ��
		{
			//Ч�����
			//ASSERT(wDataSize == sizeof(CMD_CS_S_ServerRemove));
			if (wDataSize != sizeof(CMD_CS_S_ServerRemove)) return false;

			//��������
			CMD_CS_S_ServerRemove * pServerRemove = (CMD_CS_S_ServerRemove *)pData;

			//��������
			m_ServerListManager.DeleteGameServer(pServerRemove->wServerID);

			return true;
		}
	case SUB_CS_S_SERVER_FINISH:	//�������
		{
			//�����б�
			m_ServerListManager.CleanServerItem();

			return true;
		}
	default:
		break;
	}


/*	switch (wSubCmdID)
	{		
	case SUB_CS_S_BATTLE_INSERT:	//Լս����
		{
			//Ч�����
			//ASSERT(wDataSize%sizeof(tagServiceItem)==0);
			if (wDataSize%sizeof(tagServiceItem) != 0) return false;

			//��������
			tagServiceItem * pBattleItem = (tagServiceItem *)pData;

			//��ȡ����
			CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;

			//��������
			CopyMemory(&m_BattleItem, pBattleItem, sizeof(m_BattleItem));

			//����ʱ��
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_BATTLE_START, 1000L, 1, 0);

			return true;
		}
	case SUB_CS_S_BATTLE_REMOVE:	//Լս�Ƴ�
		{
			//Ч�����
			//ASSERT(wDataSize==sizeof(CMD_CS_S_BattleRemove));
			if (wDataSize!=sizeof(CMD_CS_S_BattleRemove)) return false;

			//��ȡ����
            CMD_CS_S_BattleRemove * pBattleRemove=(CMD_CS_S_BattleRemove *)pData;	

			//ɾ����ʱ��
			m_pITimerEngine->KillTimer(IDI_CONNECT_BATTLE_START);

			//��������
			if (pStatusRemove->wStatusID == m_BattleItem.wStatusID)
			{
				ZeroMemory(&m_StatusItem, sizeof(m_StatusItem));
			}

			//�ر�ʱ��			
			m_pITimerEngine->KillTimer(IDI_CONNECT_BATTLE_START);

			return true;
		}
	}*/

	return true;
}


bool CAttemperEngineSink::OnTCPSocketClubInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
		case SUB_CS_C_CHECK_NEW_USER:		//��������
		{
			CMD_CS_S_CheckNewUser * pCheckUser = (CMD_CS_S_CheckNewUser *)pData;
			DBR_GR_CheckNewUser CheckUser;
			CheckUser.dwUserID = pCheckUser->dwUserID;
			CopyMemory(CheckUser.szClientIP, pCheckUser->szClientIP, sizeof(CheckUser.szClientIP));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CHECK_NEW_USER, 0, &CheckUser, sizeof(CheckUser));
			return true;
		}
	}
	return true;
}

bool CAttemperEngineSink::OnTCPSocketWebInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_GROUP_MAX_MEMBER:		//Ⱥ���������
	{
		CMD_CS_C_GroupMaxMember * pMaxMember = (CMD_CS_C_GroupMaxMember *)pData;
		if (pMaxMember->dwGroupID == 0)
		{
			POSITION grouppos = NULL;
			do
			{
				//ö�ٴ�����
				CGroupItem *pGroupItem = m_GroupInfoManager.EnumGroupItem(grouppos);
				if (pGroupItem == NULL) break;

				pGroupItem->GetGroupProperty()->wMaxMemberCount = pMaxMember->wMaxCount;

			} while (grouppos != NULL);
		}
		else
		{
			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pMaxMember->dwGroupID);
			if (pGroupItem)
			{
				pGroupItem->GetGroupProperty()->wMaxMemberCount = pMaxMember->wMaxCount;
			}
		}

		return true;
	}
	}
	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPSocketBattleMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
		{
			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess = ER_SUCCESS;
			SendUIControlPacket(UI_BATTLE_RESULT, &ControlResult, sizeof(ControlResult));

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//��������
			CMD_CS_S_RegisterFailure * pRegisterFailure = (CMD_CS_S_RegisterFailure *)pData;

			//Ч�����
			//ASSERT(wDataSize >= (sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���		
			m_pITCPSocketBattle->CloseSocket();

			//��ʾ��Ϣ
			LPCTSTR pszDescribeString = pRegisterFailure->szDescribeString;
			if (lstrlen(pszDescribeString) > 0)
			{
				CTraceService::TraceString(pszDescribeString, TraceLevel_Exception);
			}

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess = ER_FAILURE;
			SendUIControlPacket(UI_BATTLE_RESULT, &ControlResult, sizeof(ControlResult));
			return true;
		}
	}

	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPSocketAIMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_SG_G_UPDATE_GROUP_STATUS:		//���¾��ֲ�״̬
		{
			CMD_SS_G_Update_Group_Status * pUpdateGroupStatus = (CMD_SS_G_Update_Group_Status *)pData;


			TCHAR szInfo[260] = {0};

			//wsprintf(szInfo, TEXT("ptdtai *** OnTCPSocketAIMainRegister ���¾��ֲ�״̬S userid = %d"), pUpdateGroupStatus->dwGroupID);
			////OutputDebugString(szInfo);

			if (pUpdateGroupStatus)
			{
				SUB_GA_MemberStatus Status;
				Status.dwUserID = pUpdateGroupStatus->dwUserID;
				Status.cbStatus = pUpdateGroupStatus->cbStatus;

		
				CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pUpdateGroupStatus->dwGroupID);
				if (pGroupItem == NULL)
				{
					wsprintf(szInfo, TEXT("ptdtai *** OnTCPSocketAIMainRegister ���¾��ֲ�״̬S userid = %d Ⱥ��Τ��"), pUpdateGroupStatus->dwGroupID);
					//OutputDebugString(szInfo);
					return true;
				}

				if (pGroupItem->SearchMember(Status.dwUserID) == NULL)
				{
					//wsprintf(szInfo, TEXT("ptdtai *** �ı��û�״̬ SUB_SG_G_UPDATE_GROUP_STATUS userid = %d δ�ҵ�"), Status.dwUserID);
					////OutputDebugString(szInfo);
					return true;
				}
				//////////////////////////////////////////////////////////////

				//����ṹ
				tagIMUserInfo UserInfo;
				ZeroMemory(&UserInfo, sizeof(UserInfo));

				//���ñ���
				UserInfo.cbUserStatus = Status.cbStatus;
				UserInfo.dwUserID = Status.dwUserID;
				UserInfo.dwGameID = 0;
				UserInfo.cbGender = 0;
				UserInfo.dwCustomID = 0;
				lstrcpynW(UserInfo.szNickName, TEXT("111"), CountArray(UserInfo.szNickName));

				//��������
				CLocalUserItem * pLocalUserItem = NULL;
				CServerUserItem * pServerUserItem = NULL;

				//�����û�	
				pServerUserItem = m_ServerUserManager.SearchUserItem(UserInfo.dwUserID);
				if (pServerUserItem != NULL)
				{
					//wsprintf(szInfo, TEXT("ptdtai *** �ı��û�״̬  dwGroupID = %d userid = %d cbStatus = %d  ��Ҵ���"), pUpdateGroupStatus->dwGroupID, Status.dwUserID, pUpdateGroupStatus->cbStatus);
					////OutputDebugString(szInfo);

					tagIMUserInfo * pUserInfo = pServerUserItem->GetUserInfo();
					pUserInfo->cbUserStatus = Status.cbStatus;

				}
				else
				{
					//wsprintf(szInfo, TEXT("ptdtai *** �ı��û�״̬  dwGroupID = %d userid = %d cbStatus = %d  ��Ҳ�����"), pUpdateGroupStatus->dwGroupID, Status.dwUserID, pUpdateGroupStatus->cbStatus);
					////OutputDebugString(szInfo);

					//�����û�
					pLocalUserItem = m_ServerUserManager.InsertLocalUserItem(UserInfo,  TEXT(""));
					pLocalUserItem->SetSocketID(0);
				}

				///////////////////////////////////////////////////////////////
				//wsprintf(szInfo, TEXT("ptdtai *** +++++++++++ �ı��û�״̬ SUB_SG_G_UPDATE_GROUP_STATUS dwGroupID = %d userid = %d cbStatus = %d"), pUpdateGroupStatus->dwGroupID, Status.dwUserID, pUpdateGroupStatus->cbStatus);
				////OutputDebugString(szInfo);

				pGroupItem->SearchMember(Status.dwUserID)->cbStatus = pUpdateGroupStatus->cbStatus;
				SendDataToGroupMember(pGroupItem->GetGroupID(), MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_STATUS, &Status, sizeof(Status));


				//����ṹ
				SUB_GA_GroupUpdate * pGroupUpdate = (SUB_GA_GroupUpdate *)m_cbBuffer;
				ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
				//���ñ���
				pGroupUpdate->dwGroupID = pGroupItem->GetGroupID();
				pGroupUpdate->cbUpdateKind = UPGROUP_KIND_PROPERTY;

				//��������
				CopyMemory(pGroupUpdate + 1, pGroupItem->GetGroupProperty(), sizeof(tagIMGroupProperty));

				//��������
				SendDataToGroupMember(pGroupUpdate->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_UPDATE, pGroupUpdate, sizeof(SUB_GA_GroupUpdate)+sizeof(tagIMGroupProperty));
			}

			
			return true;
		}
	case SUB_SG_G_RELOAD_GROUP_MEMBER:
		{
			CMD_SS_G_ReloadGroupMember * pReloadGroupMember = (CMD_SS_G_ReloadGroupMember *)pData;
			if (!pReloadGroupMember)
			{
				return true;
			}

			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtai *** ���ֲ� dwGroupID = %d ���¼��س�Ա"), pReloadGroupMember->dwGroupID);
			//OutputDebugString(szInfo);

			//����ṹ
			DBR_GR_LoadSingleGroup LoadSingleGroup;
			LoadSingleGroup.dwGroupID = pReloadGroupMember->dwGroupID;


			//DBR_GR_LOAD_SINGLE_GROUP
			//���س�Ա
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SINGLE_GROUP, 0, &LoadSingleGroup, sizeof(LoadSingleGroup));
			return true;
		}
	}

	return true;
}


//Լս����
bool CAttemperEngineSink::OnTCPSocketBattleMainBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_QUERY_CLUB_ROOM_LIST_RESULT:		//��ѯ����
		{
			//��ȡ����
			CMD_CS_S_QueryResult *pQueryClubRoom = (CMD_CS_S_QueryResult *)pData;

			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_S_QueryResult)+pQueryClubRoom->wTableCount*sizeof(tagClubTableInfo));
			if (wDataSize != sizeof(CMD_CS_S_QueryResult)+pQueryClubRoom->wTableCount*sizeof(tagClubTableInfo)) return false;

			//����ṹ
			CMD_GA_TableList *pTableList = (CMD_GA_TableList*)m_cbBuffer;
			ZeroMemory(pTableList, sizeof(m_cbBuffer));

			//���ñ���
			pTableList->dwGroupID = pQueryClubRoom->dwGroupID;
			pTableList->wTableCount = pQueryClubRoom->wTableCount;

			//��������
			CopyMemory(pTableList + 1, pQueryClubRoom + 1, sizeof(tagClubTableInfo)*pQueryClubRoom->wTableCount);

			//��������
			WORD wPacketSize = sizeof(CMD_GA_TableList)+pQueryClubRoom->wTableCount*sizeof(tagClubTableInfo);

			m_pITCPNetworkEngine->SendData(pQueryClubRoom->dwSocketID, MDM_GA_BATTLE_SERVICE, SUB_GA_TABLE_LIST, m_cbBuffer, wPacketSize);

			return true;
		}
	case SUB_CS_C_QUERY_CLUB_ROOM_LIST_FINISH:		//��ѯ���
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_S_QueryFinish));
			if (wDataSize != sizeof(CMD_CS_S_QueryFinish)) return false;

			//��ȡ����
			CMD_CS_S_QueryFinish *pQueryFinish = (CMD_CS_S_QueryFinish *)pData;

			//��������
			m_pITCPNetworkEngine->SendData(pQueryFinish->dwSocketID, MDM_GA_BATTLE_SERVICE, SUB_GA_TABLE_LIST_FINISH, 0, 0);

			return true;
		}
	case SUB_CS_C_CLUB_USER_STATUS:					//״̬�仯
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_S_UpdateTable));
			if (wDataSize != sizeof(CMD_CS_S_UpdateTable)) return false;

			//��ȡ����
			CMD_CS_S_UpdateTable *pUpdateTable = (CMD_CS_S_UpdateTable *)pData;

			//����У��
			if (pUpdateTable->wChairID >= GROUP_MAX_CHAIR_GAME_CHAIR_COUNT) return true;

			//���Ҵ�����
			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pUpdateTable->dwGroupID);
			if (pGroupItem == NULL) return true;

			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtai **** SUB_CS_C_CLUB_USER_STATUS bStart = %d"), pUpdateTable->bGameStart?1:0);
			//OutputDebugString(szInfo);

			

			pGroupItem->UpdateTableUserInfo(pUpdateTable);
			//��Ϸ��ʼ
			if (pUpdateTable->dwUserID == 0)
			{
				//��������
				CMD_GA_TableStart TableStart;
				ZeroMemory(&TableStart, sizeof(CMD_GA_TableStart));
				TableStart.dwGroupID = pUpdateTable->dwGroupID;
				TableStart.dwPersonalRoomID = pUpdateTable->dwPersonalRoomID;
				TableStart.dwKindID = pUpdateTable->dwKindID;
				TableStart.dwServerID = pUpdateTable->dwServerID;
				TableStart.wTableID = (WORD)pUpdateTable->wTableID;
				TableStart.bStart = pUpdateTable->bGameStart;

				//OutputDebugString(TEXT("ptdtai *** SUB_CS_C_CLUB_USER_STATUS ����ҷ��� ��������״̬ 1 "));
				SendDataToGroupMember(pUpdateTable->dwGroupID, MDM_GA_BATTLE_SERVICE, SUB_GA_TABLE_START, &TableStart, sizeof(TableStart));
			}
			else  //�û�״̬�仯
			{
				//��������
				CMD_GA_UserTableStatus UserTableStatus;
				ZeroMemory(&UserTableStatus, sizeof(UserTableStatus));

				UserTableStatus.dwGroupID = pUpdateTable->dwGroupID;
				UserTableStatus.dwPersonalRoomID = pUpdateTable->dwPersonalRoomID;
				UserTableStatus.dwKindID = pUpdateTable->dwKindID;
				UserTableStatus.dwServerID = pUpdateTable->dwServerID;
				UserTableStatus.wTableID = pUpdateTable->wTableID;
				UserTableStatus.bSit = pUpdateTable->bSit;
				UserTableStatus.dwUserID = pUpdateTable->dwUserID;

				UserTableStatus.wChairID = (WORD)pUpdateTable->wChairID;

				//OutputDebugString(TEXT("ptdtai *** SUB_CS_C_CLUB_USER_STATUS ����ҷ��� ��������״̬ 2"));
				SendDataToGroupMember(pUpdateTable->dwGroupID, MDM_GA_BATTLE_SERVICE, SUB_GA_USER_TABLE_STATUS, &UserTableStatus, sizeof(UserTableStatus));

				if (!pUpdateTable->bSit)
				{
					tagClubTableInfo TableInfo;
					pGroupItem->GetTableInfo(pUpdateTable->dwPersonalRoomID, TableInfo);
					bool bHaveUser = false;

					for (int i = 0; i < GROUP_MAX_CHAIR_GAME_CHAIR_COUNT; i++)
					{
						if (TableInfo.dwPlayerID[i] > 0 && TableInfo.dwPlayerID[i]!=INVALID_DWORD)
						{
							bHaveUser = true;
							break;
						}
					}
					if (!bHaveUser && pGroupItem->SearchConfig(TableInfo.dwConfigID) == NULL)//�����������淨��ɾ������ɢ����
					{
						//֪ͨԼս��ɢ����
						CMD_CS_C_DismissTable pack;
						ZeroMemory(&pack, sizeof(pack));
						pack.dwServerID = TableInfo.dwServerID;
						pack.dwTableID = TableInfo.wTableID;
						m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &pack, sizeof(CMD_CS_C_DismissTable));
					}

					tagIMGroupMember * pIMGroupMember = pGroupItem->SearchMember(pUpdateTable->dwUserID);
					if (pIMGroupMember)
					{
						pIMGroupMember->cbStatus = 1;

						SUB_GA_MemberStatus Status;
						Status.dwUserID = pUpdateTable->dwUserID;
						Status.cbStatus = 1;

						SendDataToGroupMember(pGroupItem->GetGroupID(), MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_STATUS, &Status, sizeof(Status));
					}
					

				}
				else
				{
					tagIMGroupMember * pIMGroupMember = pGroupItem->SearchMember(pUpdateTable->dwUserID);
					if (pIMGroupMember)
					{
						pIMGroupMember->cbStatus = 2;

						SUB_GA_MemberStatus Status;
						Status.dwUserID = pUpdateTable->dwUserID;
						Status.cbStatus = 2;

						SendDataToGroupMember(pGroupItem->GetGroupID(), MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_STATUS, &Status, sizeof(Status));
					}
				}
			}

			return true;
		}
	case SUB_CS_C_CLUB_TABLE_CHANGEBANKER:					//���ֲ�����ׯ��
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_S_TableChangeBanker));
			if (wDataSize != sizeof(CMD_CS_S_TableChangeBanker)) return false;

			//��ȡ����
			CMD_CS_S_TableChangeBanker *pTableChangeBanker = (CMD_CS_S_TableChangeBanker *)pData;

			//���Ҵ�����
			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pTableChangeBanker->dwGroupID);
			if (pGroupItem == NULL) return true;

			CMapClubTableInfo *pMapClubTableInfo = pGroupItem->GetCMapClubTableInfo();

			if (pMapClubTableInfo)
			{
				tagClubTableInfo TableInfo;
				if (pMapClubTableInfo->Lookup(pTableChangeBanker->dwPersonalRoomID, TableInfo))
				{
					//��ֵ������Ϸׯ��USERID
					TableInfo.dwBankerUserID = pTableChangeBanker->dwBankerUserID;

					//��������
					CMD_GA_TableChangeBanker TableChangeBanker;
					ZeroMemory(&TableChangeBanker, sizeof(TableChangeBanker));

					TableChangeBanker.dwGroupID = pTableChangeBanker->dwGroupID;
					TableChangeBanker.dwPersonalRoomID = pTableChangeBanker->dwPersonalRoomID;
					TableChangeBanker.dwKindID = pTableChangeBanker->dwKindID;
					TableChangeBanker.dwServerID = pTableChangeBanker->dwServerID;
					TableChangeBanker.wTableID = pTableChangeBanker->wTableID;
					TableChangeBanker.dwBankerUserID = pTableChangeBanker->dwBankerUserID;

					SendDataToGroupMember(pTableChangeBanker->dwGroupID, MDM_GA_BATTLE_SERVICE, SUB_GA_TABLE_CHANGEBANKER, &TableChangeBanker, sizeof(TableChangeBanker));
				}
			}

			return true;
		}
	case SUB_CS_C_CLUB_CREATE_TABLE:			//�����˷��䴴��
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai ****** �����˷����� SUB_CS_C_CLUB_CREATE_TABLE  "));
			//OutputDebugString(szInfo);

			//����У��
			//ASSERT(wDataSize == sizeof(tagClubTableInfo));
			if (wDataSize != sizeof(tagClubTableInfo)) return false;

			//��ȡ����
			tagClubTableInfo *pClubTableInfo = (tagClubTableInfo *)pData;

			wsprintf(szInfo, TEXT("ptdtai ****** �����˷����� SUB_CS_C_CLUB_CREATE_TABLE  dwGroupID = %d  ��ʼ����"), pClubTableInfo->dwGroupID);
			//OutputDebugString(szInfo);

			//���Ҵ�����
			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pClubTableInfo->dwGroupID);
			if (pGroupItem == NULL)
			{
				
				wsprintf(szInfo, TEXT("ptdtai ****** �����˷����� SUB_CS_C_CLUB_CREATE_TABLE  dwGroupID = %d  ������"), pClubTableInfo->dwGroupID);
				//OutputDebugString(szInfo);
				return true;
			}

			//У������˰�����Ϸ(����У��)
			if (m_ServerListManager.IsGroupPersonalMaxChairGame(pClubTableInfo->dwKindID) && pGroupItem != NULL && pGroupItem->GetGroupCreaterID() != pClubTableInfo->dwUserID)
			{
				wsprintf(szInfo, TEXT("ptdtai ****** �����˷����� SUB_CS_C_CLUB_CREATE_TABLE  dwGroupID = %d  ������Ϸ"), pClubTableInfo->dwGroupID);
				//OutputDebugString(szInfo);
				//��ʾ��Ϣ
				CString strTrace;
				strTrace.Format(TEXT("ServerID=%dΪ�����˰�����Ϸ,ֻ����������!"), pClubTableInfo->dwServerID);
				CTraceService::TraceString(strTrace, TraceLevel_Normal);

				return true;
			}

			//���ӷ���
			pGroupItem->AddPersonalTable(pClubTableInfo->dwPersonalRoomID,*pClubTableInfo);

			wsprintf(szInfo, TEXT("ptdtai ****** �����˷����� SUB_CS_C_CLUB_CREATE_TABLE  dwGroupID = %d  ���͸������˳�Ա"), pClubTableInfo->dwGroupID);
			//OutputDebugString(szInfo);

			//���͸������˳�Ա
			SendDataToGroupMember(pClubTableInfo->dwGroupID, MDM_GA_BATTLE_SERVICE, SUB_GA_TABLE_CREATE, pData, sizeof(tagClubTableInfo));

			return true;
		}
	case SUB_CS_C_CLUB_DISMISS_TABLE:			//�����˷����ɢ
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_S_DismissClubTable));
			if (wDataSize != sizeof(CMD_CS_S_DismissClubTable)) return false;

			//��ȡ����
			CMD_CS_S_DismissClubTable *pDismissClubTable = (CMD_CS_S_DismissClubTable *)pData;

			//���Ҵ�����
			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pDismissClubTable->dwGroupID);
			if (pGroupItem == NULL) return true;

			//��ȡ�淨ID
			DWORD dwConfigID = pGroupItem->GetConfigID(pDismissClubTable->dwPersonalRoomID);

			//ɾ������
			pGroupItem->RemovePersonalTable(pDismissClubTable->dwPersonalRoomID);

			//���͸������˳�Ա
			SendDataToGroupMember(pDismissClubTable->dwGroupID, MDM_GA_BATTLE_SERVICE, SUB_GA_TABLE_DISMISS, pData, sizeof(CMD_CS_S_DismissClubTable));

			//У������˰�����Ϸ
			if (m_ServerListManager.IsGroupPersonalMaxChairGame(pDismissClubTable->dwKindID) && dwConfigID != INVALID_DWORD)
			{
				//ɾ���淨
				//����ṹ
				DBR_GR_DeleteConfig deleteConfig;
				ZeroMemory(&deleteConfig, sizeof(deleteConfig));

				deleteConfig.dwConfigID = dwConfigID;
				deleteConfig.dwGroupID = pDismissClubTable->dwGroupID;
				deleteConfig.dwUserID = pGroupItem->GetGroupCreaterID();
				deleteConfig.bGroupPersonalMaxChair = true;

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_CONFIG, INVALID_DWORD, &deleteConfig, sizeof(deleteConfig));
			}

			return true;
		}
	case SUB_CS_C_CLUB_CURRENCE_FEE:	//��������
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_GR_CurrenceGroupFee));
			if (wDataSize != sizeof(CMD_GR_CurrenceGroupFee)) return false;

			//��ȡ����
			CMD_GR_CurrenceGroupFee *pCurrenceGroupFee = (CMD_GR_CurrenceGroupFee *)pData;
		
			//���Ҵ�����
			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pCurrenceGroupFee->dwGroupID);
			if (pGroupItem == NULL) return true;

			//���´����˲Ƹ�
			tagIMGroupWealth *pGroupWealth = pGroupItem->GetGroupWealth();	
			pGroupWealth->lIngot = pCurrenceGroupFee->lDiamond;

			//��ѯ�û�
			CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pGroupItem->GetGroupProperty()->dwCreaterID);

			if (pServerUserItem != NULL && pServerUserItem->GetUserItemKind() == enLocalKind)
			{
				//������
				SUB_GA_WealthUpdate WealthUpdate;
				ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

				//���ñ���		
				WealthUpdate.lUserIngot = pCurrenceGroupFee->lDiamond;
				//������Ϣ
				m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_LOGIC_SERVICE, SUB_GA_WEALTH_UPDATE, &WealthUpdate, sizeof(WealthUpdate));
			}

			return true;

		}
	case SUB_CS_C_UPDATE_GROUP_SCORE:
		{
			CString cc;

			//ASSERT(wDataSize == sizeof(CMD_CS_S_UpdateClubScore));
			if (wDataSize != sizeof(CMD_CS_S_UpdateClubScore)) return false;

			CMD_CS_S_UpdateClubScore* pUpdateTable = (CMD_CS_S_UpdateClubScore*)pData;
			cc.Format(_T("������%d�յ���Ϸ��������Ϣ���������%d����%I64d,"), pUpdateTable->dwGroupID, pUpdateTable->dwUserID, pUpdateTable->lScore);

			//����Ⱥ��
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pUpdateTable->dwGroupID);
			if (pGroupItem != NULL)
			{
				tagIMGroupMember *pMember = pGroupItem->SearchMember(pUpdateTable->dwUserID);
				if (pMember)
				{
					pMember->lScore += pUpdateTable->lScore;
					cc.AppendFormat(_T("���³ɹ���%d����%I64d"), pUpdateTable->dwUserID, pMember->lScore);
					SendLogData(cc);

					//�㲥�����ı�
					//��������			
					BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
					ZeroMemory(cbBuffer, sizeof(cbBuffer));

					//��������
					DBO_GR_PresentSuccess * pResult = (DBO_GR_PresentSuccess *)cbBuffer;
					pResult->dwGroupID = pGroupItem->GetGroupID();
					pResult->dwPresentID = pGroupItem->GetGroupProperty()->dwCreaterID;
					pResult->cbType = 0;

					WORD wPacketSize = sizeof(DBO_GR_PresentSuccess);

					//��������
					tagIMUserScore * pUser = NULL;
					pUser = (tagIMUserScore *)(cbBuffer + wPacketSize);
					pUser->dwUserID = pUpdateTable->dwUserID;
					pUser->lCurScore = pMember->lScore;
					pUser->cbSuccess = 1;
					pResult->nCount++;
					wPacketSize += sizeof(tagIMUserScore);
					SendDataToGroupMember(pGroupItem->GetGroupID(), MDM_GA_LOGIC_SERVICE, SUB_GR_PRESENT_SUCCESS, cbBuffer, wPacketSize);

				}
			}
			return true;
		}
	case SUB_CS_C_CLUB_CREATE_RULE:			//�������淨����
		{
			//����У��
			//ASSERT(wDataSize == sizeof(tagClubTableInfo));
			if (wDataSize != sizeof(tagClubTableInfo)) return false;

			return true;
		}
	case SUB_CS_C_CLUB_DISMISS_RULE:		//�������淨��ɢ
		{
			//����У��
			//ASSERT(wDataSize == sizeof(CMD_CS_S_DismissClubTable));
			if (wDataSize != sizeof(CMD_CS_S_DismissClubTable)) return false;

			//��ȡ����
			CMD_CS_S_DismissClubTable *pDismissClubTable = (CMD_CS_S_DismissClubTable *)pData;

			//���Ҵ�����
			CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pDismissClubTable->dwGroupID);
			if (pGroupItem == NULL) return true;

			//���͸������˳�Ա
			SendDataToGroupMember(pDismissClubTable->dwGroupID, MDM_GA_BATTLE_SERVICE, SUB_GA_TABLE_DISMISS, pData, sizeof(CMD_CS_S_DismissClubTable));

			return true;
		}
	case SUB_CS_C_QUERY_ROOM_BATTLE_INFO_RES:
		{
			//ASSERT(wDataSize == sizeof(CMD_CS_S_QueryTableInfoResult));
			if (wDataSize != sizeof(CMD_CS_S_QueryTableInfoResult)) return false;

			CMD_CS_S_QueryTableInfoResult* pResult = (CMD_CS_S_QueryTableInfoResult*)pData;

			//����Ⱥ��
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
			if (pGroupItem != NULL)
			{
				SUB_GA_QueryTableBattleInfoResult pack;
				ZeroMemory(&pack, sizeof(pack));
				pack.dwGroupID = pResult->dwGroupID;
				pack.dwRoomID = pResult->dwRoomID;
				pack.dwAllCount = pResult->dwAllCount;
				pack.dwPlayCount = pResult->dwPlayCount;
				CopyMemory(pack.dwUserID, pResult->dwUserID, sizeof(pack.dwUserID));
				CopyMemory(pack.lGameScore, pResult->lGameScore, sizeof(pack.lGameScore));
				m_pITCPNetworkEngine->SendData(pResult->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_QUERY_TABLE_RES, &pack, sizeof(pack));
			}
			return true;
		}
	case SUB_CS_S_CLUB_SERVER_CLOSE:
		{
			CMD_CS_S_ServerRemove *pServerRemove = (CMD_CS_S_ServerRemove*)pData;
			if (pServerRemove)
			{
				//����÷���������
				POSITION grouppos = NULL;
				do
				{
					//ö�ٴ�����
					CGroupItem *pGroupItem = m_GroupInfoManager.EnumGroupItem(grouppos);
					if (pGroupItem == NULL) break;

					CMapClubTableInfo *pMapClubTableInfo = pGroupItem->GetCMapClubTableInfo();
					tagClubTableInfo TableInfo;
					DWORD dwRoomID;
					POSITION tablepos = pMapClubTableInfo->GetStartPosition();;
					while (tablepos != NULL)
					{
						pMapClubTableInfo->GetNextAssoc(tablepos, dwRoomID, TableInfo);

						//�����淨
						if (TableInfo.dwServerID == pServerRemove->wServerID && pGroupItem->SearchConfig(TableInfo.dwConfigID))
						{
							tagGroupConfigInfo *pGroupConfigInfo = pGroupItem->SearchConfig(TableInfo.dwConfigID);
							if (pGroupConfigInfo->cbGroupPersonalMaxChair == TRUE)
							{
								//����ṹ
								DBR_GR_DeleteConfig deleteConfig;
								ZeroMemory(&deleteConfig, sizeof(deleteConfig));

								deleteConfig.dwConfigID = pGroupConfigInfo->dwConfigID;
								deleteConfig.dwGroupID = pGroupConfigInfo->dwGroupID;
								deleteConfig.dwUserID = pGroupItem->GetGroupCreaterID();
								deleteConfig.bGroupPersonalMaxChair = true;

								//Ͷ������
								m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_CONFIG, INVALID_DWORD, &deleteConfig, sizeof(deleteConfig));
							}
						}
					}

					pGroupItem->RemovePersonalTable(pServerRemove->wServerID);

					CMD_CS_S_DismissClubTable DismissClubTable;
					ZeroMemory(&DismissClubTable, sizeof(DismissClubTable));
					DismissClubTable.dwGroupID = pGroupItem->GetGroupID();
					DismissClubTable.dwServerID = pServerRemove->wServerID;
					DismissClubTable.bDeleteServer = true;

					SendDataToGroupMember(pGroupItem->GetGroupID(), MDM_GA_BATTLE_SERVICE, SUB_GA_TABLE_DISMISS, &DismissClubTable, sizeof(CMD_CS_S_DismissClubTable));
				} while (grouppos != NULL);
			}

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

			//����Ⱥ��
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pClubDeleteConfig->dwGroupID);
			if (pGroupItem != NULL && pGroupItem->GetGroupProperty()->cbGroupStatus != 1)
			{
				return true;
			}

			//�����淨
			tagGroupConfigInfo * pGroupConfigInfo = pGroupItem->SearchConfig(pClubDeleteConfig->dwConfigID);
			if (!pGroupConfigInfo)
			{
				return true;
			}

			//����ṹ
			DBR_GR_DeleteConfig deleteConfig;
			ZeroMemory(&deleteConfig, sizeof(deleteConfig));

			deleteConfig.dwConfigID = pClubDeleteConfig->dwConfigID;
			deleteConfig.dwGroupID = pClubDeleteConfig->dwGroupID;
			deleteConfig.dwUserID = pClubDeleteConfig->dwUserID;
			deleteConfig.bGroupPersonalMaxChair = true;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_CONFIG, INVALID_DWORD, &deleteConfig, sizeof(deleteConfig));

			return true;
		}
	}
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


//Լս����
bool CAttemperEngineSink::SendDataToBattle(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//���񲻿���
	if (m_pITCPSocketBattle == NULL)
	{
		//����ṹ
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������						
		SystemMessage.wType=SMT_EJECT|SMT_CLOSE_LINK;		
		lstrcpyn(SystemMessage.szString, _T("Լս���񲻿��á����Ժ��ٴ����ԣ�"), CountArray(SystemMessage.szString));

		//������Ϣ
		WORD wPacketSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString)+CountStringBuffer(SystemMessage.szString);		
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_SYSTEM_MESSAGE, &SystemMessage, wPacketSize);

		return false;
	}

	//��������
	if (m_pITCPSocketBattle != NULL)
	{
		m_pITCPSocketBattle->SendData(wMainCmdID, wSubCmdID, pData, wDataSize);
	}

	return true;
}
//Ⱥ�鷢�� 
bool CAttemperEngineSink::SendDataToGroupMember(DWORD dwGroupID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, bool bVerifyGroupID)
{
	TCHAR szInfo[260] = { 0 };
	//���ҽ���
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(dwGroupID);
	if (pGroupItem == NULL)
	{
		//wsprintf(szInfo, TEXT("ptdtai **** SendDataToGroupMember ��ȡ�淨�ɹ� ��ÿ����Ա�����淨 δ�ҵ��þ��ֲ� dwGroupID = %d"),  dwGroupID);
		////OutputDebugString(szInfo);
		return false;
	}

	//ö�ٳ�Ա
	INT_PTR nIndex = 0;
	tagIMGroupMember *pGroupMember = NULL;
	CServerUserItem *pServerUserItem = NULL;


	do
	{	//ö�ٳ�Ա
		pGroupMember = pGroupItem->EnumMember(nIndex++);
		if (pGroupMember == NULL)
		{
			//wsprintf(szInfo, TEXT("ptdtai **** SendDataToGroupMember ��ȡ�淨�ɹ� ��ÿ����Ա�����淨 �þ��ֲ��������"));
			////OutputDebugString(szInfo);
			break;
		}

		//�����û�
		pServerUserItem = m_ServerUserManager.SearchUserItem(pGroupMember->dwMemberID);
		if (pServerUserItem == NULL)
		{
			//wsprintf(szInfo, TEXT("ptdtai **** SendDataToGroupMember ��ȡ�淨�ɹ� ��ÿ����Ա�����淨 dwMemberID = %d ���Ϊ��"), pGroupMember->dwMemberID);
			////OutputDebugString(szInfo);
			continue;
		}
		if (pServerUserItem->GetUserStatus() != US_GP_ONLINE)
		{
			//wsprintf(szInfo, TEXT("ptdtai **** SendDataToGroupMember ��ȡ�淨�ɹ� ��ÿ����Ա�����淨 dwMemberID = %d ��Ҳ�����"), pGroupMember->dwMemberID);
			////OutputDebugString(szInfo);
			continue;
		}
		if (pServerUserItem->GetActiveGroupID() != dwGroupID && bVerifyGroupID)
		{
			//wsprintf(szInfo, TEXT("ptdtai **** SendDataToGroupMember ��ȡ�淨�ɹ� ��ÿ����Ա�����淨 GetActiveGroupID = %d bVerifyGroupID = %d ��Ҳ�����"), pServerUserItem->GetActiveGroupID(), bVerifyGroupID?1:0);
			////OutputDebugString(szInfo);
			continue;
		}
		if (pServerUserItem->GetActiveGroupID() == dwGroupID && wSubCmdID == SUB_GA_GROUP_DELETE) pServerUserItem->SetActiveGroupID(0);

		if (307 == pServerUserItem->GetUserID())
		{
			wsprintf(szInfo, TEXT("ptdtai **** ++++++++++++++ SendDataToGroupMember ���� userid = %d dwGroupID = %d socketid = %d wSubCmdID = %d"), pServerUserItem->GetUserID(), dwGroupID, pServerUserItem->GetSocketID(), wSubCmdID);
			//OutputDebugString(szInfo);
		}


		//��������
		m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), wMainCmdID, wSubCmdID, pData, wDataSize);

	} while (true);

	return true;
}

bool CAttemperEngineSink::SendDataToGroupManager(DWORD dwGroupID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//���ҽ���
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(dwGroupID);
	if (pGroupItem == NULL) return false;

	//ö�ٳ�Ա
	INT_PTR nIndex = 0;
	tagIMGroupMember *pGroupMember = NULL;
	CServerUserItem *pServerUserItem = NULL;

	do
	{	//ö�ٳ�Ա
		pGroupMember = pGroupItem->EnumMember(nIndex++);
		if (pGroupMember == NULL) break;

		if ((pGroupMember->nMemberRight&(GP_MANAGER|GP_CREATOR))==0) continue;
		//�����û�
		pServerUserItem = m_ServerUserManager.SearchUserItem(pGroupMember->dwMemberID);
		if (pServerUserItem == NULL) continue;
		if (pServerUserItem->GetUserStatus() != US_GP_ONLINE) continue;
		if (pServerUserItem->GetActiveGroupID() != dwGroupID) continue;
		//��������
		m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), wMainCmdID, wSubCmdID, pData, wDataSize);

	} while (true);

	return true;
}

bool CAttemperEngineSink::SendDataToGroupTeamer(DWORD dwGroupID, DWORD dwCaptainID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//���ҽ���
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(dwGroupID);
	if (pGroupItem == NULL) return false;

	//ö�ٳ�Ա
	INT_PTR nIndex = 0;
	tagIMGroupMember *pGroupMember = NULL;
	CServerUserItem *pServerUserItem = NULL;

	do
	{	//ö�ٳ�Ա
		pGroupMember = pGroupItem->EnumMember(nIndex++);
		if (pGroupMember == NULL) break;

		if (pGroupMember->dwCaptainID != dwCaptainID) continue;
		//�����û�
		pServerUserItem = m_ServerUserManager.SearchUserItem(pGroupMember->dwMemberID);
		if (pServerUserItem == NULL) continue;
		if (pServerUserItem->GetUserStatus() != US_GP_ONLINE) continue;
		if (pServerUserItem->GetActiveGroupID() != dwGroupID) continue;
		//��������
		m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), wMainCmdID, wSubCmdID, pData, wDataSize);

	} while (true);

	return true;
}

//���ͳ�Ա
bool CAttemperEngineSink::SendGroupMemberToUser(DWORD dwGroupID, DWORD dwSocketID)
{
	//���ҽ���
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(dwGroupID);
	if (pGroupItem == NULL) return false;

	//��������
	INT nIndex = 0;
	WORD wPacketSize = 0L;
	BYTE cbBuffer[GP_SOCKET_PACKET];

	//��������
	tagIMGroupMember * pGroupMember = NULL;
	CServerUserItem * pServerUserItem = NULL;
	tagIMGroupMemberUser * pGroupMemberUser = NULL;

	//��������
	SUB_GA_GroupMember * pIMGroupMember = (SUB_GA_GroupMember *)cbBuffer;

	//���ñ�ʶ
	pIMGroupMember->dwGroupID = dwGroupID;
	wPacketSize = sizeof(SUB_GA_GroupMember);

	TCHAR szInfo[260] = {0};

	do
	{
		//ö�ٳ�Ա
		pGroupMember = pGroupItem->EnumMember(nIndex++);
		if (pGroupMember == NULL) break;

		//�����û�
		pServerUserItem = m_ServerUserManager.SearchUserItem(pGroupMember->dwMemberID);
		if (pServerUserItem == NULL) continue;

		//�����ж�
		if (wPacketSize + sizeof(tagIMGroupMemberUser)>sizeof(cbBuffer))
		{
			//��������
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_MEMBER, cbBuffer, wPacketSize);
			wPacketSize = sizeof(SUB_GA_GroupMember);
		}

		//��ȡ����
		pGroupMemberUser = (tagIMGroupMemberUser *)&cbBuffer[wPacketSize];

		//��������		
		CopyMemory(&pGroupMemberUser->MemberInfo, pGroupMember, sizeof(pGroupMemberUser->MemberInfo));
		CopyMemory(&pGroupMemberUser->UserInfo, pServerUserItem->GetUserInfo(), sizeof(pGroupMemberUser->UserInfo));

		//wsprintf(szInfo, TEXT("ptdtai ***--------------------------  SendGroupMemberToUser userid = %d cbStatus = %d"), pGroupMemberUser->MemberInfo.dwMemberID, pGroupMemberUser->MemberInfo.cbStatus);
		////OutputDebugString(szInfo);

		//���ñ���
		wPacketSize += sizeof(tagIMGroupMemberUser);

	} while (true);

	//��������
	if (wPacketSize>0)
	{
		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_MEMBER, cbBuffer, wPacketSize);
		wPacketSize = sizeof(SUB_GA_GroupMember);
	}

	return true;
}

bool CAttemperEngineSink::SendGroupConfigToUser(DWORD dwGroupID, DWORD dwSocketID)
{
	//���ҽ���
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(dwGroupID);
	if (pGroupItem == NULL) return false;

	//��������
	INT nIndex = 0;
	WORD wPacketSize = 0L;
	BYTE cbBuffer[GP_SOCKET_PACKET];

	//��������
	tagGroupConfigInfo * pConfigInfo = NULL;

	//��������
	SUB_GA_GroupConfig * pIMGroupConfig = (SUB_GA_GroupConfig *)cbBuffer;

	//���ñ�ʶ
	pIMGroupConfig->dwGroupID = dwGroupID;
	pIMGroupConfig->nCount = 0;
	wPacketSize = sizeof(SUB_GA_GroupConfig);

	do
	{
		//ö���淨
		tagGroupConfigInfo *pConfig = pGroupItem->EnumConfig(nIndex++);
		if (pConfig == NULL) break;

		//�����ж�
		if (wPacketSize + sizeof(tagGroupConfigInfo) > sizeof(cbBuffer))
		{
			//��������
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_CONFIG, cbBuffer, wPacketSize);
			wPacketSize = sizeof(SUB_GA_GroupConfig);
			pIMGroupConfig->nCount = 0;
		}

		//��ȡ����
		pConfigInfo = (tagGroupConfigInfo *)&cbBuffer[wPacketSize];

		//��������		
		CopyMemory(pConfigInfo, pConfig, sizeof(tagGroupConfigInfo));

		//���ñ���
		wPacketSize += sizeof(tagGroupConfigInfo);
		pIMGroupConfig->nCount++;
	} while (true);

	//��������
	if (wPacketSize > 0)
	{
		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_CONFIG, cbBuffer, wPacketSize);
		wPacketSize = sizeof(SUB_GA_GroupConfig);
	}

	return true;
}

bool CAttemperEngineSink::UpdateGroupExtraInfo(DWORD dwGroupID, DWORD dwMemberID)
{
	CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(dwGroupID);
	if (pGroupItem == NULL)
		return true;
	tagIMGroupMember *pUserItem = pGroupItem->SearchMember(dwMemberID);
	if (pUserItem == NULL)
		return true;
	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(dwMemberID);
	if (pServerUserItem != NULL && pServerUserItem->GetUserStatus() == US_GP_ONLINE)
	{

		//��ȡ����
		tagIMGroupExtraInfo ExtraInfo;
		ZeroMemory(&ExtraInfo, sizeof(ExtraInfo));

		//���ñ���
		ExtraInfo.dwGroupID = dwGroupID;
		ExtraInfo.dwUserID = dwMemberID;
		ExtraInfo.cbGroupIDShow = pGroupItem->GetGroupBattleRule()->cbGroupIDShow;
		ExtraInfo.nMemberRight = pUserItem->nMemberRight;
		if (pUserItem->dwCaptainID != 0)
		{
			tagIMGroupMember *pCaptain = pGroupItem->SearchMember(pUserItem->dwCaptainID);
			if (pCaptain)
			{
				ExtraInfo.lInvitationCode = pCaptain->lInvitationCode;
				CopyMemory(&ExtraInfo.szQQ, pCaptain->szQQ, sizeof(ExtraInfo.szQQ));
				CopyMemory(&ExtraInfo.szWX, pCaptain->szWX, sizeof(ExtraInfo.szWX));
				CopyMemory(&ExtraInfo.szPhone, pCaptain->szPhone, sizeof(ExtraInfo.szPhone));
			}
		}

		else
		{
			tagIMGroupMember *pCreator = pGroupItem->SearchMember(pGroupItem->GetGroupProperty()->dwCreaterID);
			if (pCreator)
			{
				ExtraInfo.lInvitationCode = pCreator->lInvitationCode;
				CopyMemory(&ExtraInfo.szQQ, pCreator->szQQ, sizeof(ExtraInfo.szQQ));
				CopyMemory(&ExtraInfo.szWX, pCreator->szWX, sizeof(ExtraInfo.szWX));
				CopyMemory(&ExtraInfo.szPhone, pCreator->szPhone, sizeof(ExtraInfo.szPhone));
			}
		}
		//��������

		m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_GROUP_SERVICE, SUB_AG_GROUP_EXTRA_INFO, &ExtraInfo, sizeof(ExtraInfo));
	}
	return true;
}

//��ȡ����
DWORD CAttemperEngineSink::BuildTokenID()
{
	return 0;
}

//�󶨲���
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//��Ч����
	if (wBindIndex==INVALID_WORD) return NULL;

	//��������
	if (wBindIndex<m_pInitParameter->m_wMaxConnect)
	{
		return m_pBindParameter+wBindIndex;
	}

	//�������
	//ASSERT(FALSE);

	return NULL;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//У�����
	//ASSERT(wDataSize >= sizeof(DBO_GR_LogonSuccess));
	if (wDataSize<sizeof(DBO_GR_LogonSuccess)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_LogonSuccess * pLogonSuccess = (DBO_GR_LogonSuccess *)pData;

	//����ṹ
	tagIMUserInfo UserInfo;
	ZeroMemory(&UserInfo, sizeof(UserInfo));

	//���ñ���
	UserInfo.cbUserStatus = US_GP_ONLINE;
	UserInfo.dwUserID = pLogonSuccess->dwUserID;
	UserInfo.dwGameID = pLogonSuccess->dwGameID;
	UserInfo.cbGender = pLogonSuccess->cbGender;
	UserInfo.dwCustomID = pLogonSuccess->dwCustomID;
	lstrcpynW(UserInfo.szNickName, pLogonSuccess->szNickName, CountArray(UserInfo.szNickName));

	//��������
	CLocalUserItem * pLocalUserItem = NULL;
	CServerUserItem * pServerUserItem = NULL;

	//�����û�	
	pServerUserItem = m_ServerUserManager.SearchUserItem(UserInfo.dwUserID);
	if (pServerUserItem != NULL)
	{
		//Զ���û�
		if (pServerUserItem->GetUserItemKind() == enRemoteKind)
		{
			//ɾ���û�
			m_ServerUserManager.DeleteUserItem(UserInfo.dwUserID);

			//�����û�
			pLocalUserItem = m_ServerUserManager.InsertLocalUserItem(UserInfo, pLogonSuccess->szLogonPass);
			pLocalUserItem->SetSocketID(pBindParameter->dwSocketID);
		}

		//�����û�
		if (pServerUserItem->GetUserItemKind() == enLocalKind)
		{
			//�ǳ�����
			if (pServerUserItem->GetUserStatus() == US_GP_ONLINE)
			{
				//������Ϣ
				CMD_CM_SystemMessage SystemMessage;
				ZeroMemory(&SystemMessage, sizeof(SystemMessage));

				//��������						
				SystemMessage.wType = SMT_EJECT | SMT_CLOSE_LINK;
				lstrcpynW(SystemMessage.szString, L"�����˺��������ط���¼�����������ߣ�", CountArray(SystemMessage.szString));
				SystemMessage.wLength = CountStringBufferW(SystemMessage.szString);
				//������Ϣ
				WORD wPacketSize = sizeof(SystemMessage)-sizeof(SystemMessage.szString) + CountStringBufferW(SystemMessage.szString);
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_SYSTEM_MESSAGE, &SystemMessage, wPacketSize);
			}

			//���ñ���
			pLocalUserItem = (CLocalUserItem *)pServerUserItem;

			//��������
			pLocalUserItem->ModifyLogonPassword(pLogonSuccess->szLogonPass);

			//������Ϣ
			CopyMemory(pLocalUserItem->GetUserInfo(), &UserInfo, sizeof(tagIMUserInfo));
			//���������־
			pLocalUserItem->SetSocketID(pBindParameter->dwSocketID);

		}

	}
	else
	{
		//�����û�
		pLocalUserItem = m_ServerUserManager.InsertLocalUserItem(UserInfo,pLogonSuccess->szLogonPass);
		pLocalUserItem->SetSocketID(pBindParameter->dwSocketID);
	}

	//�ж�ָ��
	if (pLocalUserItem != NULL)
	{
		//��¼ʱ��			
		DWORD dwLogonTime = (DWORD)time(NULL);
		pLocalUserItem->SetLogonTime(dwLogonTime);

		//�Ƴ�Ⱥ��
		pLocalUserItem->RemoveGroupID();

		//���Ⱥ��
		if (pLogonSuccess->wGroupCount>0)
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdt InsertGroupID wGroupCount=%d"), pLogonSuccess->wGroupCount);
			//OutputDebugString(szInfo);

			//��ȡ����
			DWORD * pdwGroup = (DWORD *)(pLogonSuccess + 1);

			//����Ⱥ��
			for (WORD i = 0; i<pLogonSuccess->wGroupCount; i++)
			{
				pLocalUserItem->InsertGroupID(*pdwGroup++);
			}
		}
	}



	return OnEventUserLogon(pLocalUserItem);
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize <= sizeof(DBO_GR_LogonFailure));
	if (wDataSize>sizeof(DBO_GR_LogonFailure)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_LogonFailure * pLogonFailure = (DBO_GR_LogonFailure *)pData;

	//��ȡ����
	SUB_GA_S_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure, sizeof(LogonFailure));

	//���ñ���			
	LogonFailure.lErrorCode = pLogonFailure->lErrorCode;
	lstrcpynW(LogonFailure.szErrorDescribe, pLogonFailure->szErrorDescribe, CountArray(LogonFailure.szErrorDescribe));

	//��������
	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_LOGON_FAILURE, &LogonFailure, sizeof(LogonFailure));

	return true;
}

//����Ⱥ��
bool CAttemperEngineSink::OnDBCreateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize == sizeof(DBO_GR_CreateGroup));
	if (wDataSize != sizeof(DBO_GR_CreateGroup)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_CreateGroup * pCreateGroup = (DBO_GR_CreateGroup *)pData;

	//��ѯȺ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pCreateGroup->GroupItem.GroupProperty.dwGroupID);
	if (pGroupItem != NULL)
	{
		//��������
		CopyMemory(pGroupItem->GetGroupItem(), &pCreateGroup->GroupItem, sizeof(tagIMGroupItem));
	}
	else
	{
		if (m_GroupInfoManager.ActiveGroupItem(pCreateGroup->GroupItem))
		{
			pGroupItem = m_GroupInfoManager.SearchGroupItem(pCreateGroup->GroupItem.GroupProperty.dwGroupID);
		}
	}

	//��ѯ�û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pCreateGroup->GroupItem.GroupProperty.dwCreaterID);
	if (pServerUserItem != NULL && pServerUserItem->GetUserItemKind() == enLocalKind && pGroupItem!=NULL)
	{
		//��ӳ�Ա
		pGroupItem->GetGroupProperty()->wMemberCount++;
		pGroupItem->AddMember(pCreateGroup->GroupMember);
		((CLocalUserItem *)pServerUserItem)->InsertGroupID(pCreateGroup->GroupItem.GroupProperty.dwGroupID);

	}
	SUB_GA_GroupProperty * pGroupProperty = (SUB_GA_GroupProperty *)m_cbBuffer;
	ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

	//��������
	CopyMemory(&pGroupProperty->GroupProperty, pGroupItem->GetGroupProperty(), sizeof(tagIMGroupProperty));
	tagIMGroupExtraInfo * pGroupExtraInfo = (tagIMGroupExtraInfo *)(pGroupProperty + 1);
	pGroupExtraInfo->dwGroupID = pGroupItem->GetGroupID();
	pGroupExtraInfo->dwUserID = pGroupItem->GetGroupProperty()->dwCreaterID;
	pGroupExtraInfo->cbGroupIDShow = pGroupItem->GetGroupBattleRule()->cbGroupIDShow;
	pGroupExtraInfo->nMemberRight = GP_CREATOR;
	pGroupExtraInfo->lInvitationCode = pCreateGroup->GroupMember.lInvitationCode;
	//������Ϣ
	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_PROPERTY, pGroupProperty, sizeof(SUB_GA_GroupProperty) + sizeof(tagIMGroupExtraInfo));
	
	//֪ͨԼս
	if (m_pITCPSocketBattle)
	{
		CMD_CS_C_CreateGroupSuccess CreateGroupSuccess;
		ZeroMemory(&CreateGroupSuccess, sizeof(CreateGroupSuccess));

		CreateGroupSuccess.dwGroupID = pGroupItem->GetGroupID();

		m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CREATE_GROUP_SUCCESS, &CreateGroupSuccess, sizeof(CreateGroupSuccess));
	}

	////���Դ���
	//tagGroupAIParameter GroupAIParameter;

	//GroupAIParameter.lDistirbuteTotalScore = 1000000;
	//GroupAIParameter.dwGroupID = pGroupExtraInfo->dwGroupID;
	//GroupAIParameter.wKindID = 57;
	//GroupAIParameter.dwAICount = 50;
	//GroupAIParameter.dwPlayingAIRateMin = 60;
	//GroupAIParameter.dwPlayingAIRateMax = 80;
	//GroupAIParameter.lDistirbuteSingleScore = 2000;
	//GroupAIParameter.lDistirbuteScoreMinRatio = 50;
	//GroupAIParameter.lDistirbuteScoreMaxRatio = 90;
	//m_pAITCPSocketService->SendData(MDM_CS_S_AI_ACTION, SUB_CS_S_AI_PARAMETER, &GroupAIParameter, sizeof(GroupAIParameter));
	
	return true;
}

//����Ⱥ��
bool CAttemperEngineSink::OnDBUpdateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize >= sizeof(DBO_GR_UpdateGroup));
	if (wDataSize<sizeof(DBO_GR_UpdateGroup)) return false;
	
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_UpdateGroup * pDBUpdateGroup = (DBO_GR_UpdateGroup *)pData;

	//��ѯȺ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pDBUpdateGroup->dwGroupID);
	if (pGroupItem != NULL)
	{
		//����ṹ
		SUB_GA_GroupUpdate * pGroupUpdate = (SUB_GA_GroupUpdate *)m_cbBuffer;
		ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
		//���ñ���
		pGroupUpdate->dwGroupID = pDBUpdateGroup->dwGroupID;
		pGroupUpdate->cbUpdateKind = pDBUpdateGroup->cbUpdateKind;

		//���²Ƹ�
		if (pDBUpdateGroup->cbUpdateKind == UPGROUP_KIND_WEALTH)
		{
			//��������
			CopyMemory(pGroupUpdate + 1, pDBUpdateGroup + 1, sizeof(tagIMGroupWealth));

			//��������
			CopyMemory(pGroupItem->GetGroupWealth(), pDBUpdateGroup + 1, sizeof(tagIMGroupWealth));

			//����Ⱥ��
			CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pGroupItem->GetGroupProperty()->dwCreaterID);
			if (pServerUserItem != NULL)
			{
				//��������
				WORD wPacketSize = sizeof(SUB_GA_GroupUpdate)+sizeof(tagIMGroupWealth);
				//������Ϣ
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_UPDATE, pGroupUpdate, wPacketSize);

			}
		}

		//��������
		if (pDBUpdateGroup->cbUpdateKind == UPGROUP_KIND_PROPERTY)
		{
			//��������
			CopyMemory(pGroupUpdate + 1, pDBUpdateGroup + 1, sizeof(tagIMGroupProperty));

			//��������
			CopyMemory(pGroupItem->GetGroupProperty(), pDBUpdateGroup + 1, sizeof(tagIMGroupProperty));

			//��������
			WORD wPacketSize = sizeof(SUB_GA_GroupUpdate)+sizeof(tagIMGroupProperty);
			SendDataToGroupMember(pGroupUpdate->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_UPDATE, pGroupUpdate, wPacketSize);
		}

		//���¹���
		if (pDBUpdateGroup->cbUpdateKind == UPGROUP_KIND_BATTLERULE)
		{
			//��������
			CopyMemory(pGroupUpdate + 1, pDBUpdateGroup + 1, sizeof(tagIMGroupBattleRule));

			//��������
			CopyMemory(pGroupItem->GetGroupBattleRule(), pDBUpdateGroup + 1, sizeof(tagIMGroupBattleRule));

			//��������
			WORD wPacketSize = sizeof(SUB_GA_GroupUpdate)+sizeof(tagIMGroupBattleRule);
			SendDataToGroupMember(pGroupUpdate->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_UPDATE, pGroupUpdate, wPacketSize);
		}
	}

	return true;
}

//ɾ��Ⱥ��
bool CAttemperEngineSink::OnDBDeleteGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize == sizeof(DBO_GR_DeleteGroup));
	if (wDataSize != sizeof(DBO_GR_DeleteGroup)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_DeleteGroup * pDBDeleteGroup = (DBO_GR_DeleteGroup *)pData;


	//ɾ��Ⱥ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pDBDeleteGroup->dwGroupID);
	if (pGroupItem != NULL)
	{
		//ɾ���˴����˵�����
		tagClubTableInfo TableInfo;
		POSITION tablepos = NULL;
		do
		{
			//��������������
			TableInfo = pGroupItem->EnumTable(tablepos);
			if (TableInfo.dwServerID != 0 && TableInfo.dwGroupID == pDBDeleteGroup->dwGroupID)
			{
				//֪ͨԼս��ɢ����
				CMD_CS_C_DismissTable pack;
				ZeroMemory(&pack, sizeof(pack));
				pack.dwServerID = TableInfo.dwServerID;
				pack.dwTableID = TableInfo.wTableID;
				m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &pack, sizeof(CMD_CS_C_DismissTable));
			}

		} while (tablepos != NULL);

		//����ṹ
		SUB_GA_GroupDelete DeleteGroup;
		ZeroMemory(&DeleteGroup, sizeof(SUB_GA_GroupDelete));

		//��������
		DeleteGroup.dwGroupID = pDBDeleteGroup->dwGroupID;

		//��������
		SendDataToGroupMember(DeleteGroup.dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_DELETE, &DeleteGroup, sizeof(DeleteGroup), false);

		//ɾ��Ⱥ��
		m_GroupInfoManager.RemoveGroupItem(pDBDeleteGroup->dwGroupID);
	}


	return true;
}

//��ӳ�Ա
bool CAttemperEngineSink::OnDBInsertMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize == sizeof(DBO_GR_InsertMember));
	if (wDataSize != sizeof(DBO_GR_InsertMember)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_InsertMember * pInsertMember = (DBO_GR_InsertMember *)pData;
	
	//��ȡ����
	SUB_GA_MemberInsert IMInsertMember;
	ZeroMemory(&IMInsertMember, sizeof(SUB_GA_MemberInsert));

	//��ѯ�û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pInsertMember->MemberUser.UserInfo.dwUserID);
	if (pServerUserItem != NULL && pServerUserItem->GetUserStatus() == US_GP_ONLINE)
	{
		pInsertMember->MemberUser.MemberInfo.cbStatus = US_GP_ONLINE;
	}

	//���ñ���
	IMInsertMember.dwGroupID = pInsertMember->dwGroupID;
	CopyMemory(&IMInsertMember.MemberUser, &pInsertMember->MemberUser, sizeof(IMInsertMember.MemberUser));



	//��ѯȺ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pInsertMember->dwGroupID);
	if (pGroupItem != NULL)
	{
		//��������
		SendDataToGroupMember(IMInsertMember.dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_INSERT, &IMInsertMember, sizeof(IMInsertMember), false);

		//��ӳ�Ա
		pGroupItem->GetGroupProperty()->wMemberCount++;
		pGroupItem->AddMember(pInsertMember->MemberUser.MemberInfo);

		//����û�
		CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pInsertMember->MemberUser.UserInfo.dwUserID);
		if (pServerUserItem != NULL)
		{
			//����״̬
			BYTE cbUserStatus = pServerUserItem->GetUserStatus();

			//��������
			CopyMemory(pServerUserItem->GetUserInfo(), &pInsertMember->MemberUser.UserInfo, sizeof(tagIMUserInfo));

			//����״̬
			pServerUserItem->SetUserStatus(cbUserStatus);
		}
		else
		{
			m_ServerUserManager.InsertRemoteUserItem(pInsertMember->MemberUser.UserInfo);
		}

		if (pInsertMember->MemberUser.MemberInfo.cbStatus == US_GP_ONLINE)
		{
			//����Ⱥ��
			if (pServerUserItem->GetUserItemKind() == enLocalKind)
			{
				((CLocalUserItem *)pServerUserItem)->InsertGroupID(pInsertMember->dwGroupID);
			}

			SUB_GA_GroupProperty * pGroupProperty = (SUB_GA_GroupProperty *)m_cbBuffer;
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
			CopyMemory(&pGroupProperty->GroupProperty, pGroupItem->GetGroupProperty(), sizeof(tagIMGroupProperty));

			tagIMGroupExtraInfo * pGroupExtraInfo = (tagIMGroupExtraInfo *)(pGroupProperty + 1);
			pGroupExtraInfo->dwGroupID = pGroupItem->GetGroupID();
			pGroupExtraInfo->dwUserID = pGroupItem->GetGroupProperty()->dwCreaterID;
			pGroupExtraInfo->cbGroupIDShow = pGroupItem->GetGroupBattleRule()->cbGroupIDShow;
			pGroupExtraInfo->nMemberRight = pInsertMember->MemberUser.MemberInfo.nMemberRight;

			if (pInsertMember->MemberUser.MemberInfo.dwCaptainID != 0)
			{
				tagIMGroupMember *pCaptain = pGroupItem->SearchMember(pInsertMember->MemberUser.MemberInfo.dwCaptainID);
				if (pCaptain)
					pGroupExtraInfo->lInvitationCode = pCaptain->lInvitationCode;
				else
					SendLogData(_T("Captatin null"));
			}
			else
			{
				tagIMGroupMember *pCreator = pGroupItem->SearchMember(pGroupItem->GetGroupProperty()->dwCreaterID);
				if (pCreator)
					pGroupExtraInfo->lInvitationCode = pCreator->lInvitationCode;
			}

			//������Ϣ
			m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_PROPERTY, pGroupProperty, sizeof(SUB_GA_GroupProperty) + sizeof(tagIMGroupExtraInfo));

			//��ȡ����
			SUB_GA_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���		
			//lstrcpynW(OperateSuccess.szOperateNotice, _T("ͨ����ˣ��ɹ���������ˣ�"), CountArray(OperateSuccess.szOperateNotice));
			_stprintf(OperateSuccess.szOperateNotice, _T("ͨ����ˣ��ɹ����������'%s'��"), pGroupProperty->GroupProperty.szGroupName);
			//��������
			//������Ϣ
			m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_SUCCESS, &OperateSuccess, sizeof(OperateSuccess));
		}
	}


	return true;
}

//ɾ����Ա
bool CAttemperEngineSink::OnDBDeleteMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize > sizeof(DBO_GR_DeleteMember));
	if (wDataSize <= sizeof(DBO_GR_DeleteMember)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_DeleteMember * pDeleteMember = (DBO_GR_DeleteMember *)pData;


	//��ѯȺ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pDeleteMember->dwGroupID);
	if (pGroupItem != NULL)
	{
		//��������
		SendDataToGroupMember(pDeleteMember->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_DELETE, pData, wDataSize, false);

		BYTE *p = (BYTE*)pData;
		tagIMDeleteMember*pUserItem = (tagIMDeleteMember*)(p + sizeof(DBO_GR_DeleteMember));
		//ɾ������Ƕӳ���Ǩ����Ϣ
		for (int i = 0; i < pDeleteMember->nCount; i++)
		{
			if (pUserItem->cbSuccess == 1)
			{
				tagIMGroupMember*pMember=pGroupItem->SearchMember(pUserItem->dwMemberID);
				//�Ƴ��ӳ�
				if (pMember->nMemberRight&GP_CAPTAIN)
				{
					//�ҵ����������
					INT_PTR nIndex = 0;
					tagIMGroupMember *pUser = NULL;
					do
					{	//ö�ٳ�Ա
						pUser = pGroupItem->EnumMember(nIndex++);
						if (pUser == NULL) break;

						if (pUser->dwCaptainID == pMember->dwMemberID)
						{
							pUser->dwCaptainID = pMember->dwCaptainID;
							if (pUser->dwCaptainID == 0 && pUser->nMemberRight == GP_TEAMER)
								pUser->nMemberRight = GP_MEMBER;

							SUB_GA_MemberUpdate MemberUpdate;
							ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));
							MemberUpdate.dwGroupID = pDeleteMember->dwGroupID;
							CopyMemory(&MemberUpdate.GroupMember, pUser, sizeof(tagIMGroupMember));
							SendDataToGroupMember(pDeleteMember->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate));

							UpdateGroupExtraInfo(pDeleteMember->dwGroupID, pUser->dwMemberID);
						}

					} while (true);
				}

			}
			pUserItem++;

		}

		//ɾ����Ա
		pUserItem = (tagIMDeleteMember*)(p + sizeof(DBO_GR_DeleteMember));
		for (int i = 0; i < pDeleteMember->nCount; i++)
		{
			if (pUserItem->cbSuccess == 1)
			{
				tagIMGroupMember*pMember = pGroupItem->SearchMember(pUserItem->dwMemberID);
				if (pMember)
				{
					//ɾ����Ա
					pGroupItem->GetGroupProperty()->wMemberCount--;
					pGroupItem->RemoveMember(pUserItem->dwMemberID);

					//�Ƴ�Ⱥ��
					CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserItem->dwMemberID);
					if (pServerUserItem != NULL && pServerUserItem->GetUserItemKind() == enLocalKind)
					{
						((CLocalUserItem *)pServerUserItem)->RemoveGroupID(pDeleteMember->dwGroupID);
					}
				}
			}
			pUserItem++;

		}
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBApplyMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize >= sizeof(DBO_GR_ApplyMessage));
	if (wDataSize<sizeof(DBO_GR_ApplyMessage)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_ApplyMessage * pDBApplyMessage = (DBO_GR_ApplyMessage *)pData;

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDBApplyMessage->dwUserID);
	if (pServerUserItem != NULL && pServerUserItem->GetUserStatus() == US_GP_ONLINE)
	{

		//��ȡ����
		SUB_GA_ApplyMessage * pApplyMessage = (SUB_GA_ApplyMessage *)m_cbBuffer;
		ZeroMemory(pApplyMessage, sizeof(m_cbBuffer));

		//���ñ���
		pApplyMessage->cbMessageType = pDBApplyMessage->cbMessageType;
		pApplyMessage->wMessageCount = pDBApplyMessage->wMessageCount;

		//���ñ���	
		CopyMemory(pApplyMessage + 1, pDBApplyMessage + 1, wDataSize - sizeof(DBO_GR_ApplyMessage));

		//��������
		WORD wPacketSize = sizeof(SUB_GA_ApplyMessage) + wDataSize - sizeof(DBO_GR_ApplyMessage);
		m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_LOGIC_SERVICE, SUB_GA_APPLY_MESSAGE, pApplyMessage, wPacketSize);
	}

	return true;
}

//Ⱥ������
bool CAttemperEngineSink::OnDBGroupOption(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize%sizeof(tagIMGroupOption) == 0);
	if (wDataSize%sizeof(tagIMGroupOption) != 0) return false;

	//��������
	WORD wOptionCount = wDataSize / sizeof(tagIMGroupOption);
	tagIMGroupOption * pGroupOption = (tagIMGroupOption *)pData;

	//�Ƴ�����
	m_GroupOptionArray.RemoveAll();

	//�������
	for (WORD i = 0; i<wOptionCount; i++)
	{
		m_GroupOptionArray.Add(*pGroupOption++);
	}

	//�¼�֪ͨ
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess = wOptionCount>0 ? ER_SUCCESS : ER_FAILURE;
	SendUIControlPacket(UI_LOAD_GROUP_OPTION, &ControlResult, sizeof(ControlResult));

	return true;
}

//��ֹ�ʻ�
bool CAttemperEngineSink::OnDBForbidWords(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�������
	/*if (dwContextID==0xFFFF)
	{
	m_WordFilter.AddFinish();
	return true;
	}*/

	//��ʼ����
	if (dwContextID == 0xFFFE)
	{
		//���ö���
		m_WordFilter.ResetFilter();

		//�رն�ʱ��
		m_pITimerEngine->KillTimer(IDI_LOAD_FORBID_WORDS);

		return true;
	}

	//��������
	WORD wBufferPos = 0;
	BYTE cbWordsCount = 0;
	BYTE * pBuffer = (LPBYTE)pData;
	TCHAR szForbidWords[32] = TEXT("");

	//��ȡ����
	while (wBufferPos<wDataSize)
	{
		cbWordsCount = *(BYTE *)&pBuffer[wBufferPos++];
		CopyMemory(szForbidWords, &pBuffer[wBufferPos], cbWordsCount);
		wBufferPos += cbWordsCount;

		//��Ӵʻ�
		m_WordFilter.InsertKeyword(szForbidWords);
	}

	return true;
}

//�Ƹ�����
bool CAttemperEngineSink::OnDBWealthUpdate(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize == sizeof(DBO_GR_WealthUpdate));
	if (wDataSize != sizeof(DBO_GR_WealthUpdate)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_WealthUpdate * pWealthUpdate = (DBO_GR_WealthUpdate *)pData;

	//��ѯ�û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pWealthUpdate->dwUserID);
	if (pServerUserItem != NULL && pServerUserItem->GetUserItemKind() == enLocalKind)
	{
		//������
		SUB_GA_WealthUpdate WealthUpdate;
		ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

		//���ñ���		
		WealthUpdate.lUserIngot = pWealthUpdate->lUserIngot;

		//������Ϣ
		m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_WEALTH_UPDATE, &WealthUpdate, sizeof(WealthUpdate));
	}

	return true;
}

//ɾ�����
bool CAttemperEngineSink::OnDBApplyDelResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize <= sizeof(DBO_GR_ApplyDelResult));
	if (wDataSize>sizeof(DBO_GR_ApplyDelResult)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_ApplyDelResult * pApplyDelResult = (DBO_GR_ApplyDelResult *)pData;

	//��ѯ�û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pApplyDelResult->dwUserID);
	if (pServerUserItem != NULL && pServerUserItem->GetUserItemKind() == enLocalKind)
	{
		//������
		SUB_GA_ApplyDelResult ApplyDelResult;
		ZeroMemory(&ApplyDelResult, sizeof(ApplyDelResult));

		//���ñ���		
		ApplyDelResult.bDelResult = pApplyDelResult->bDelResult;
		ApplyDelResult.dwMessageID = pApplyDelResult->dwMessageID;
		ApplyDelResult.cbMessageType = pApplyDelResult->cbMessageType;
		lstrcpynW(ApplyDelResult.szErrorDescribe, pApplyDelResult->szErrorDescribe, CountArray(ApplyDelResult.szErrorDescribe));

		//������Ϣ
		m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_APPLY_DEL_RESULT, &ApplyDelResult, sizeof(ApplyDelResult));
	}

	//֪ͨȺ��
	if (pApplyDelResult->bDelResult)
	{
		CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pApplyDelResult->dwCreaterID);
		if (pServerUserItem != NULL && pServerUserItem->GetUserItemKind() == enLocalKind)
		{
			//������
			SUB_GA_ApplyDelResult ApplyDelResult;
			ZeroMemory(&ApplyDelResult, sizeof(ApplyDelResult));

			//���ñ���		
			ApplyDelResult.bDelResult = pApplyDelResult->bDelResult;
			ApplyDelResult.dwMessageID = pApplyDelResult->dwMessageID;
			ApplyDelResult.cbMessageType = 3;
			lstrcpynW(ApplyDelResult.szErrorDescribe, pApplyDelResult->szErrorDescribe, CountArray(ApplyDelResult.szErrorDescribe));

			//������Ϣ
			m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_LOGIC_SERVICE, SUB_GA_APPLY_DEL_RESULT, &ApplyDelResult, sizeof(ApplyDelResult));
		}

	}

	return true;
}

//�����ɹ�
bool CAttemperEngineSink::OnDBOperateSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize <= sizeof(DBO_GR_OperateSuccess));
	if (wDataSize>sizeof(DBO_GR_OperateSuccess)) return false;
	
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_OperateSuccess * pOperateSuccess = (DBO_GR_OperateSuccess *)pData;

	//��ѯ�û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pOperateSuccess->dwUserID);
	if (pServerUserItem != NULL && pServerUserItem->GetUserItemKind() == enLocalKind)
	{
		//��ȡ����
		SUB_GA_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

		//���ñ���		
		OperateSuccess.lOperateCode = pOperateSuccess->lOperateCode;
		lstrcpynW(OperateSuccess.szOperateNotice, pOperateSuccess->szOperateNotice, CountArray(OperateSuccess.szOperateNotice));

		//��������
		//������Ϣ
		m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_SUCCESS, &OperateSuccess, sizeof(OperateSuccess));
	}

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBOperateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize <= sizeof(DBO_GR_OperateFailure));
	if (wDataSize>sizeof(DBO_GR_OperateFailure)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);
	if (pBindParameter == NULL)return false;
	//��ȡ����
	DBO_GR_OperateFailure * pOperateFailure = (DBO_GR_OperateFailure *)pData;

	//��ѯ�û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pOperateFailure->dwUserID);
	if (pServerUserItem != NULL && pServerUserItem->GetUserItemKind() == enLocalKind)
	{
		//��ȡ����
		SUB_GA_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���		
		OperateFailure.lErrorCode = pOperateFailure->lErrorCode;
		lstrcpynW(OperateFailure.szErrorDescribe, pOperateFailure->szErrorDescribe, CountArray(OperateFailure.szErrorDescribe));

		//������Ϣ
		m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_FAILURE, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}
//��������
bool CAttemperEngineSink::OnDBLoadGroupOption(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return true;
}
//Ⱥ���Ա
bool CAttemperEngineSink::OnDBGroupMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT((wDataSize - sizeof(DBO_GR_GroupMember)) % sizeof(tagIMGroupMemberUser) == 0);
	if ((wDataSize - sizeof(DBO_GR_GroupMember)) % sizeof(tagIMGroupMemberUser) != 0) return false;

	//��ȡ����
	DBO_GR_GroupMember *pGroupMember = (DBO_GR_GroupMember *)pData;

	//��������
	WORD wMemberCount = (wDataSize - sizeof(DBO_GR_GroupMember)) / sizeof(tagIMGroupMemberUser);
	tagIMGroupMemberUser * pGroupMemberUser = (tagIMGroupMemberUser *)(pGroupMember + 1);

	//��ѯȺ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pGroupMember->dwGroupID);
	TCHAR szInfo[260] = { 0 };
	//��������
	for (int i = 0; i<wMemberCount; i++)
	{
		//��ӳ�Ա
		pGroupItem->AddMember(pGroupMemberUser->MemberInfo);
		CString cc;
		cc.Format(_T("������%d,���س�Ա%d������%I64d"), pGroupItem->GetGroupID(), pGroupMemberUser->MemberInfo.dwMemberID, pGroupMemberUser->MemberInfo.lScore);
		SendLogData(cc);
		//����û�
		CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pGroupMemberUser->UserInfo.dwUserID);
		if (pServerUserItem != NULL)
		{
			DWORD dwSocketID = pServerUserItem->GetSocketID();
			CopyMemory(pServerUserItem->GetUserInfo(), &pGroupMemberUser->UserInfo, sizeof(pGroupMemberUser->UserInfo));
			pServerUserItem->SetSocketID(dwSocketID);
			pServerUserItem->SetUserStatus(US_GP_ONLINE);
		}
		else
		{
			m_ServerUserManager.InsertRemoteUserItem(pGroupMemberUser->UserInfo);
		}

		
		wsprintf(szInfo, TEXT("ptdtai *** ���¼��س�Ա dwGroupID = %d userid = %d"), pGroupItem->GetGroupID(), pGroupMemberUser->UserInfo.dwUserID);
		//OutputDebugString(szInfo);
		//ָ�����
		pGroupMemberUser++;
	}

	if (pGroupItem)
	{
		////���ͳ�Ա
		//SendGroupMemberToUser(pGroupItem->GetGroupID(), 0);

		//����ṹ
		SUB_GA_GroupUpdate * pGroupUpdate = (SUB_GA_GroupUpdate *)m_cbBuffer;
		ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
		//���ñ���
		pGroupUpdate->dwGroupID = pGroupItem->GetGroupID();
		pGroupUpdate->cbUpdateKind = UPGROUP_KIND_PROPERTY;

		//��������
		CopyMemory(pGroupUpdate + 1, pGroupItem->GetGroupProperty(), sizeof(tagIMGroupProperty));

		//��������
		SendDataToGroupMember(pGroupUpdate->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_UPDATE, pGroupUpdate, sizeof(SUB_GA_GroupUpdate)+sizeof(tagIMGroupProperty));
	}


	return true;
}

//���³�Ա
bool CAttemperEngineSink::OnDBUpdateMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize == sizeof(DBO_GR_UpdateMember));
	if (wDataSize != sizeof(DBO_GR_UpdateMember)) return false;

	//��ȡ����
	DBO_GR_UpdateMember * pUpdateMember = (DBO_GR_UpdateMember *)pData;

	//����Ⱥ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pUpdateMember->dwGroupID);
	if (pGroupItem != NULL)
	{
		//�����˳�Ա
		tagIMGroupMember * pGroupMember = pGroupItem->SearchMember(pUpdateMember->GroupMember.dwMemberID);
		if (pGroupMember != NULL)
		{
			//������Ϣ
			CopyMemory(pGroupMember, &pUpdateMember->GroupMember, sizeof(pUpdateMember->GroupMember));

			//����ṹ
			//SUB_IM_S_MemberUpdate MemberUpdate;
			//ZeroMemory(&MemberUpdate,sizeof(MemberUpdate));

			////���ñ���				
			//MemberUpdate.dwGroupID=pUpdateMember->dwGroupID;
			//CopyMemory(&MemberUpdate.GroupMember,&pUpdateMember->GroupMember,sizeof(MemberUpdate.GroupMember));

			////��������
			//SendDataToGroupMember(MemberUpdate.dwGroupID,MDM_IM_GROUP_SERVICE,SUB_IM_S_MEMBER_UPDATE,&MemberUpdate,sizeof(MemberUpdate));			
		}
	}

	return true;
}

bool CAttemperEngineSink::OnDBGroupConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT((wDataSize - sizeof(DBO_GR_GroupConfig)) % sizeof(tagGroupConfigInfo) == 0);
	if ((wDataSize - sizeof(DBO_GR_GroupConfig)) % sizeof(tagGroupConfigInfo) != 0) return false;

	//��ȡ����
	DBO_GR_GroupConfig *pGroupConfig = (DBO_GR_GroupConfig *)pData;

	//��������
	WORD wConfigCount = (wDataSize - sizeof(DBO_GR_GroupConfig)) / sizeof(tagGroupConfigInfo);
	tagGroupConfigInfo * pGroupConfigInfo = (tagGroupConfigInfo *)(pGroupConfig + 1);

	//��ѯȺ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pGroupConfig->dwGroupID);

	CMD_SS_G_UpdateGroupGameRule UpdateGroupGameRule;
	UpdateGroupGameRule.cbStatus = 1;

	//��������
	for (int i = 0; i < wConfigCount; i++)
	{
		//����淨
		pGroupItem->AddConfig(*pGroupConfigInfo);

		memcpy_s(&UpdateGroupGameRule.GroupConfigInfo, sizeof(tagGroupConfigInfo), pGroupConfigInfo, sizeof(tagGroupConfigInfo));

		//���淨����AI������
		m_pAITCPSocketService->SendData(MDM_SS_S_SERVICE, SUB_SS_S_AI_UPDATE_BATTLE_RULE, &UpdateGroupGameRule, sizeof(UpdateGroupGameRule));
		
		//ָ�����
		pGroupConfigInfo++;
	}

	return true;
}

//Ⱥ������
bool CAttemperEngineSink::OnDBGroupList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize%sizeof(tagIMGroupItem) == 0);
	if (wDataSize%sizeof(tagIMGroupItem) != 0) return false;

	//��������
	WORD wGroupCount = wDataSize / sizeof(tagIMGroupItem);
	tagIMGroupItem * pIMGroupItem = (tagIMGroupItem *)pData;
	
	//��������
	for (int i = 0; i<wGroupCount; i++)
	{
		CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pIMGroupItem->GroupProperty.dwGroupID);
		if (pGroupItem != NULL)
		{
			CopyMemory(pGroupItem->GetGroupItem(), pIMGroupItem, sizeof(tagIMGroupItem));
		}
		else
		{
			m_GroupInfoManager.ActiveGroupItem(*pIMGroupItem);
		}

		//����ṹ
		DBR_GR_LoadGroupMember LoadGroupMember;
		LoadGroupMember.dwGroupID = pIMGroupItem->GroupProperty.dwGroupID;

		//���س�Ա
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_GROUP_MEMBER, 0, &LoadGroupMember, sizeof(LoadGroupMember));

		//�����淨
		DBR_GR_LoadGroupConfig LoadGroupConfig;
		LoadGroupConfig.dwGroupID = pIMGroupItem->GroupProperty.dwGroupID;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_GROUP_CONFIG, 0, &LoadGroupConfig, sizeof(LoadGroupConfig));

		//ָ�����
		pIMGroupItem++;
	}

	return true;
}

//�б����
bool CAttemperEngineSink::OnDBGroupListFinish(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//������
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess = ER_SUCCESS;
	SendUIControlPacket(UI_LOAD_GROUP_OPTION, &ControlResult, sizeof(ControlResult));

	//�������������
	m_tMedalClearStart = time(NULL);
	struct tm today = *localtime(&m_tMedalClearStart);
	today.tm_hour = 0;
	today.tm_min = 0;
	today.tm_sec = 0;
	LONGLONG lSeconds = difftime(m_tMedalClearStart, mktime(&today));//�����ѹ�ʱ����
	m_tMedalClearStart -= lSeconds;

	m_pITimerEngine->SetTimer(IDI_CLEAR_MEDAL, (86400-lSeconds) * 1000, 1, 0);

	return true;
}

bool CAttemperEngineSink::OnDBGroupCommonInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//ASSERT(wDataSize%sizeof(tagIMCommonItem)==0);
	if (wDataSize%sizeof(tagIMCommonItem) != 0) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);
	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_COMMON_INFO, pData, wDataSize);

	return true;
}

//Ⱥ������
bool CAttemperEngineSink::OnDBGroupItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(tagIMGroupItem));
	if (wDataSize != sizeof(tagIMGroupItem)) return false;

	//��������
	tagIMGroupItem * pIMGroupItem = (tagIMGroupItem *)pData;

	//��������
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pIMGroupItem->GroupProperty.dwGroupID);
	if (pGroupItem != NULL)
	{
		CopyMemory(pGroupItem->GetGroupItem(), pIMGroupItem, sizeof(tagIMGroupItem));
	}
	else
	{
		m_GroupInfoManager.ActiveGroupItem(*pIMGroupItem);
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnDBGroupItemFinish(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return true;
}

//Լս���
bool CAttemperEngineSink::OnDBBattleResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��������
	DBR_GR_QueryBattleResult *pBattleResult = (DBR_GR_QueryBattleResult*)pData;

	//�������
	SUB_GA_BattleResult *pSubBattleResult = (SUB_GA_BattleResult *)m_cbBuffer;
	ZeroMemory(pSubBattleResult, sizeof(m_cbBuffer));

	//��ȡ����
	pSubBattleResult->dwCreateCount = pBattleResult->dwCreateCount;
	pSubBattleResult->lConsumeIngot = pBattleResult->lConsumeIngot;
	pSubBattleResult->dwRecordCount = pBattleResult->dwRecordCount;
	CopyMemory(pSubBattleResult + 1, pBattleResult + 1, wDataSize - sizeof(DBR_GR_QueryBattleResult));

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_QUERY_BATTLE_RESULT, m_cbBuffer, wDataSize);

	return true;
}


bool CAttemperEngineSink::OnDBModifyRight(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize == sizeof(DBO_GR_ModifyRight));
	if (wDataSize != sizeof(DBO_GR_ModifyRight)) return false;

	//��ȡ����
	DBO_GR_ModifyRight * pModifyRight = (DBO_GR_ModifyRight *)pData;

	SUB_GA_RightChange RightChange;
	ZeroMemory(&RightChange, sizeof(RightChange));

	RightChange.dwGroupID = pModifyRight->dwGroupID;
	RightChange.dwMemberID = pModifyRight->dwMemberID;
	RightChange.nMemberRight = pModifyRight->nMemberRight;

	SendDataToGroupMember(pModifyRight->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_RIGHT_CHANGE, &RightChange, sizeof(RightChange));

	//����Ⱥ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pModifyRight->dwGroupID);
	if (pGroupItem != NULL)
	{
		//�����˳�Ա
		tagIMGroupMember * pGroupMember = pGroupItem->SearchMember(pModifyRight->dwMemberID);
		if (pGroupMember != NULL)
		{
			pGroupMember->nMemberRight = pModifyRight->nMemberRight;
			if ((pGroupMember->nMemberRight&GP_MANAGER) == 0)
			{
				pGroupMember->cbManagerRightFlag = 0;
				pGroupMember->lMemberManagerRight = 0;
			}

			if (pGroupMember->nMemberRight == GP_MEMBER)
				pGroupMember->dwCaptainID = 0;

			UpdateGroupExtraInfo(pModifyRight->dwGroupID, pGroupMember->dwMemberID);

			//�Ƴ��ӳ�
			if (pModifyRight->cbMode == 1 && pModifyRight->nRight == GP_CAPTAIN)
			{
				//�ҵ����������
				INT_PTR nIndex = 0;
				tagIMGroupMember *pUser = NULL;
				do
				{	//ö�ٳ�Ա
					pUser = pGroupItem->EnumMember(nIndex++);
					if (pUser == NULL) break;

					if (pUser->dwCaptainID == pModifyRight->dwMemberID)
					{
						pUser->dwCaptainID = pGroupMember->dwCaptainID;
						if (pUser->dwCaptainID == 0 && pUser->nMemberRight == GP_TEAMER)
							pUser->nMemberRight = GP_MEMBER;

						SUB_GA_MemberUpdate MemberUpdate;
						ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));
						MemberUpdate.dwGroupID = pModifyRight->dwGroupID;
						CopyMemory(&MemberUpdate.GroupMember, pUser, sizeof(tagIMGroupMember));
						SendDataToGroupMember(pModifyRight->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate));

						UpdateGroupExtraInfo(pModifyRight->dwGroupID, pUser->dwMemberID);
					}

				} while (true);
			}
		}
	}



	return true;
}

bool CAttemperEngineSink::OnDBPresentSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize > sizeof(DBO_GR_PresentSuccess));
	if (wDataSize <= sizeof(DBO_GR_PresentSuccess)) return false;

	//��ȡ����
	DBO_GR_PresentSuccess * pPresentSuccess = (DBO_GR_PresentSuccess *)pData;

	//����Ⱥ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pPresentSuccess->dwGroupID);
	if (pGroupItem != NULL)
	{
		SendDataToGroupMember(pGroupItem->GetGroupID(), MDM_GA_LOGIC_SERVICE, SUB_GR_PRESENT_SUCCESS, pData, wDataSize);
		BYTE *p = (BYTE*)pData;
		tagIMUserScore*pUserScore = (tagIMUserScore*)(p + sizeof(DBO_GR_PresentSuccess));
		for (int i = 0; i < pPresentSuccess->nCount; i++)
		{
			if (pUserScore->cbSuccess == 1)
			{
				pGroupItem->SearchMember(pUserScore->dwUserID)->lScore = pUserScore->lCurScore;
				CString cc;
				cc.Format(_T("������%d,���%d�������ͺ����%I64d"),pGroupItem->GetGroupID(), pUserScore->dwUserID, pUserScore->lCurScore);
				SendLogData(cc);
								
				//��������ڴ����������ϣ�֪ͨ��Ϸ��
				tagClubTableInfo tableinfo;
				if (pGroupItem->IsUserInGame(pUserScore->dwUserID, tableinfo))
				{
					CMD_CS_S_UpdateClubScore pack;
					pack.dwServerID = tableinfo.dwServerID;
					pack.wTableID = tableinfo.wTableID;
					pack.dwUserID = pUserScore->dwUserID;
					pack.dwGroupID = pPresentSuccess->dwGroupID;
					pack.lScore = pUserScore->lCurScore;
					m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_UPDATE_GROUP_SCORE, &pack, sizeof(pack));

				}

				//֪ͨ���ֲ�������
				CMD_SS_S_UpdateClubUserScore UpdateClubUserScore;
				UpdateClubUserScore.dwServerID = tableinfo.dwServerID;
				UpdateClubUserScore.wTableID = tableinfo.wTableID;
				UpdateClubUserScore.dwUserID = pUserScore->dwUserID;
				UpdateClubUserScore.dwGroupID = pPresentSuccess->dwGroupID;
				UpdateClubUserScore.lScore = pUserScore->lCurScore;
				UpdateClubUserScore.wKindID = tableinfo.dwKindID;
				m_pAITCPSocketService->SendData(MDM_SS_S_SERVICE, SUB_SS_S_AI_UPDATE_USER_SCORE, &UpdateClubUserScore, sizeof(UpdateClubUserScore));
			}

			pUserScore++;
		}
	}

	return true;
}

bool CAttemperEngineSink::OnDBPresentList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize >= sizeof(DBO_GR_Present_List));
	if (wDataSize < sizeof(DBO_GR_Present_List)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_QUERY_PRESENT_LIST, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBRevenueList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize >= sizeof(DBO_GR_QueryRevenue_List));
	if (wDataSize < sizeof(DBO_GR_QueryRevenue_List)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_QUERY_REVENUE_LIST, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBAppendConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize == sizeof(DBO_GR_CreateConfig));
	if (wDataSize != sizeof(DBO_GR_CreateConfig)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_CreateConfig * pCreateConfig = (DBO_GR_CreateConfig *)pData;

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai **** OnDBAppendConfig ��ȡ�淨�ɹ� 1 configID = %d dwGroupID = %d"), pCreateConfig->ConfigInfo.dwConfigID, pCreateConfig->dwGroupID);
	//OutputDebugString(szInfo);


	//����Ⱥ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pCreateConfig->ConfigInfo.dwGroupID);
	if (pGroupItem != NULL)
	{
		pGroupItem->AddConfig(pCreateConfig->ConfigInfo);

		//��������
		SUB_GA_ConfigAppend ConfigAppend;
		ZeroMemory(&ConfigAppend, sizeof(ConfigAppend));
		ConfigAppend.dwGroupID = pCreateConfig->ConfigInfo.dwGroupID;
		ConfigAppend.ConfigInfo = pCreateConfig->ConfigInfo;

		wsprintf(szInfo, TEXT("ptdtai **** OnDBAppendConfig ��ȡ�淨�ɹ� 2 ��ÿ����Ա�����淨 configID = %d dwGroupID = %d"), pCreateConfig->ConfigInfo.dwConfigID, ConfigAppend.dwGroupID);
		//OutputDebugString(szInfo);


		CMD_SS_G_UpdateGroupGameRule UpdateGroupGameRule;
		UpdateGroupGameRule.cbStatus = 1;

		SendDataToGroupMember(ConfigAppend.dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_CONFIG_APPEND, &ConfigAppend, sizeof(ConfigAppend), false);

		memcpy_s(&UpdateGroupGameRule.GroupConfigInfo, sizeof(tagGroupConfigInfo), &(pCreateConfig->ConfigInfo), sizeof(tagGroupConfigInfo));

		//���淨����AI������
		m_pAITCPSocketService->SendData(MDM_SS_S_SERVICE, SUB_SS_S_AI_UPDATE_BATTLE_RULE, &UpdateGroupGameRule, sizeof(UpdateGroupGameRule));

		//��ѯ�û�
		CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pGroupItem->GetGroupProperty()->dwCreaterID);

		if (pServerUserItem != NULL && pServerUserItem->GetUserItemKind() == enLocalKind)
		{
			//������
			SUB_GA_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//���ñ���		
			WealthUpdate.lUserIngot = pCreateConfig->lDiamond;
			//������Ϣ
			m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_LOGIC_SERVICE, SUB_GA_WEALTH_UPDATE, &WealthUpdate, sizeof(WealthUpdate));
		}

		m_pAITCPSocketService->SendData(MDM_SS_S_SERVICE, SUB_GA_CONFIG_APPEND, &ConfigAppend, sizeof(ConfigAppend));
		return true;

	}

	return false;
}

bool CAttemperEngineSink::OnDBModifyConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize == sizeof(DBO_GR_ModifyConfig));
	if (wDataSize != sizeof(DBO_GR_ModifyConfig)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_ModifyConfig * pModifyConfig = (DBO_GR_ModifyConfig *)pData;

	CMD_SS_G_UpdateGroupGameRule UpdateGroupGameRule;
	UpdateGroupGameRule.cbStatus = 2;

	//����Ⱥ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pModifyConfig->dwGroupID);
	if (pGroupItem != NULL)
	{
		pGroupItem->ModifyConfig(pModifyConfig->ConfigInfo);

		//��������
		SUB_GA_ConfigModify ConfigModify;
		ZeroMemory(&ConfigModify, sizeof(ConfigModify));
		ConfigModify.dwGroupID = pModifyConfig->dwGroupID;
		ConfigModify.ConfigInfo = pModifyConfig->ConfigInfo;
		SendDataToGroupMember(ConfigModify.dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_CONFIG_MODIFY, &ConfigModify, sizeof(ConfigModify), false);

		//
		memcpy_s(&UpdateGroupGameRule.GroupConfigInfo, sizeof(tagGroupConfigInfo), &(pModifyConfig->ConfigInfo), sizeof(tagGroupConfigInfo));

		//���淨����AI������
		m_pAITCPSocketService->SendData(MDM_SS_S_SERVICE, SUB_SS_S_AI_UPDATE_BATTLE_RULE, &UpdateGroupGameRule, sizeof(UpdateGroupGameRule));

		//��մ��淨���ղ�
		INT_PTR nIndex = 0;
		tagIMGroupMember *pGroupMember = NULL;
		do
		{	//ö�ٳ�Ա
			pGroupMember = pGroupItem->EnumMember(nIndex++);
			if (pGroupMember == NULL) break;

			pGroupMember->dwFavoriteConfig = 0;

		} while (true);
		return true;

	}

	return false;
}

bool CAttemperEngineSink::OnDBDeleteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize == sizeof(DBO_GR_DeleteConfig));
	if (wDataSize != sizeof(DBO_GR_DeleteConfig)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_DeleteConfig * pDeleteConfig = (DBO_GR_DeleteConfig *)pData;

	//����Ⱥ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pDeleteConfig->dwGroupID);
	if (pGroupItem != NULL)
	{
		pGroupItem->RemoveConfig(pDeleteConfig->dwConfigID);

		CMD_SS_G_UpdateGroupGameRule UpdateGroupGameRule;
		UpdateGroupGameRule.cbStatus = 0;

		//��������
		SUB_GA_ConfigDelete ConfigDelete;
		ZeroMemory(&ConfigDelete, sizeof(ConfigDelete));
		ConfigDelete.dwGroupID = pDeleteConfig->dwGroupID;
		ConfigDelete.dwConfigID = pDeleteConfig->dwConfigID;
		SendDataToGroupMember(ConfigDelete.dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_CONFIG_DELETE, &ConfigDelete, sizeof(ConfigDelete), false);

		//
		UpdateGroupGameRule.GroupConfigInfo.dwConfigID = pDeleteConfig->dwConfigID;
		UpdateGroupGameRule.GroupConfigInfo.dwGroupID = pDeleteConfig->dwGroupID;

		//���淨����AI������
		m_pAITCPSocketService->SendData(MDM_SS_S_SERVICE, SUB_SS_S_AI_UPDATE_BATTLE_RULE, &UpdateGroupGameRule, sizeof(UpdateGroupGameRule));

		//ɾ�����淨������
		tagClubTableInfo TableInfo;
		POSITION tablepos = NULL;
		do
		{
			//��������������
			TableInfo = pGroupItem->EnumTable(tablepos);
			if (TableInfo.dwServerID != 0 && TableInfo.dwConfigID == pDeleteConfig->dwConfigID)
			{
				bool bHaveUser = false;

				for (int i = 0; i < GROUP_MAX_CHAIR_GAME_CHAIR_COUNT; i++)
				{
					if (TableInfo.dwPlayerID[i] > 0 && TableInfo.dwPlayerID[i] != INVALID_DWORD)
					{
						bHaveUser = true;
						break;
					}
				}
				if (bHaveUser)
					continue;

				//֪ͨԼս��ɢ����
				CMD_CS_C_DismissTable pack;
				ZeroMemory(&pack, sizeof(pack));
				pack.dwServerID = TableInfo.dwServerID;
				pack.dwTableID = TableInfo.wTableID;
				m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &pack, sizeof(CMD_CS_C_DismissTable));
			}

		} while (tablepos != NULL);

		//��մ��淨���ղ�
		INT_PTR nIndex = 0;
		tagIMGroupMember *pGroupMember = NULL;
		do
		{	//ö�ٳ�Ա
			pGroupMember = pGroupItem->EnumMember(nIndex++);
			if (pGroupMember == NULL) break;
			if (pGroupMember->dwFavoriteConfig == pDeleteConfig->dwConfigID)
				pGroupMember->dwFavoriteConfig = 0;

		} while (true);

		return true;
	}

	return false;
}

bool CAttemperEngineSink::OnDBDayRecordList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize >= sizeof(DBO_GR_QueryDayRecord_List));
	if (wDataSize < sizeof(DBO_GR_QueryDayRecord_List)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_QUERY_DAYRECORD_LIST, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBUserDayDetailList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize >= sizeof(DBO_GR_QueryUserDayDetail_List));
	if (wDataSize < sizeof(DBO_GR_QueryUserDayDetail_List)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_USER_DAY_DETAIL_LIST, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBGroupDayDetailList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize >= sizeof(DBO_GR_QueryGroupDayDetail_List));
	if (wDataSize < sizeof(DBO_GR_QueryGroupDayDetail_List)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_GROUP_DAY_DETAIL_LIST, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBQueryCreateResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_QueryCreateResult));
	if (wDataSize != sizeof(DBO_GR_QueryCreateResult)) return false;

	//��ȡ����
	DBO_GR_QueryCreateResult * pResult = (DBO_GR_QueryCreateResult *)pData;

	SUB_GA_QueryCreateResult CreateResult;
	ZeroMemory(&CreateResult, sizeof(CreateResult));
	CreateResult.dwGroupID = pResult->dwGroupID;
	CreateResult.dwConfigID = pResult->dwConfigID;
	CreateResult.nMaxNullTableCount = pResult->nMaxNullTableCount;

	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
	if (pGroupItem && pGroupItem->GetNullTableCount(pResult->dwConfigID)<pResult->nMaxNullTableCount)
	{
		CreateResult.bCanCreate = true;
	}
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_QUERY_CREATE_RESULT, &CreateResult, sizeof(CreateResult));

	return true;
}

bool CAttemperEngineSink::OnDBMedalMode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_SetMedalResult));
	if (wDataSize != sizeof(DBO_GR_SetMedalResult)) return false;

	DBO_GR_SetMedalResult* pResult = (DBO_GR_SetMedalResult*)pData;
	CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
	if (pGroupItem)
	{
		tagIMGroupBattleRule *pBattleRule = pGroupItem->GetGroupBattleRule();
		pBattleRule->cbMedalMode = pResult->cbMedalMode;
		pBattleRule->cbMedalShow = pResult->cbMedalShow;
		pBattleRule->cbCurMedalShow = pResult->cbCurMedalShow;
		pBattleRule->cbMedalClear = pResult->cbMedalClear;
		pBattleRule->cbMedalRevenueShow = pResult->cbMedalRevenueShow;

		//ѫ�¹ر�
		if (pResult->cbMedalMode == 0)
		{
			//��ɢѫ������
			tagClubTableInfo TableInfo;
			POSITION tablepos = NULL;
			do
			{
				//����������ѫ������
				TableInfo = pGroupItem->EnumTable(tablepos);
				if (TableInfo.dwServerID != 0 && TableInfo.cbPlayMode == 1)
				{
					//֪ͨԼս��ɢ����
					CMD_CS_C_DismissTable pack;
					ZeroMemory(&pack, sizeof(pack));
					pack.dwServerID = TableInfo.dwServerID;
					pack.dwTableID = TableInfo.wTableID;
					m_pITCPSocketBattle->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &pack, sizeof(CMD_CS_C_DismissTable));
				}

			} while (tablepos != NULL);

			//ɾ��ѫ���淨
			INT nIndex = 0;
			do
			{
				//ö���淨
				tagGroupConfigInfo *pConfig = pGroupItem->EnumConfig(nIndex++);
				if (pConfig == NULL) break;

				if (pConfig->cbPlayMode == 1)
				{
					SUB_GA_ConfigDelete ConfigDelete;
					ZeroMemory(&ConfigDelete, sizeof(ConfigDelete));
					ConfigDelete.dwGroupID = pResult->dwGroupID;
					ConfigDelete.dwConfigID = pConfig->dwConfigID;
					SendDataToGroupMember(ConfigDelete.dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_CONFIG_DELETE, &ConfigDelete, sizeof(ConfigDelete), false);
					if(pGroupItem->RemoveConfig(nIndex-1))
						nIndex--;
				}

			} while (true);
		}
	}

	SendDataToGroupMember(pResult->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEDAL_MODE, pData, wDataSize, false);
	return true;
}

bool CAttemperEngineSink::OnDBGroupStatus(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_GroupStatus));
	if (wDataSize != sizeof(DBO_GR_GroupStatus)) return false;


	DBO_GR_GroupStatus *pStatus = (DBO_GR_GroupStatus*)pData;

	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pStatus->dwGroupID);
	pGroupItem->GetGroupProperty()->cbGroupStatus = pStatus->cbStatus;

	SendDataToGroupMember(pStatus->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_STATUS, pData, wDataSize, false);


	return true;
}

bool CAttemperEngineSink::OnDBInOutMode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_InOutMode));
	if (wDataSize != sizeof(DBO_GR_InOutMode)) return false;


	DBO_GR_InOutMode *pInOutMode = (DBO_GR_InOutMode*)pData;

	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pInOutMode->dwGroupID);
	pGroupItem->GetGroupBattleRule()->cbInMode = pInOutMode->cbInMode;
	pGroupItem->GetGroupBattleRule()->cbOutMode = pInOutMode->cbOutMode;
	pGroupItem->GetGroupBattleRule()->cbGroupIDShow = pInOutMode->cbInType;
	SendDataToGroupMember(pInOutMode->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_INOUT_MODE, pData, wDataSize, false);


	return true;
}

bool CAttemperEngineSink::OnDBQueryManagerRight(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_ManagerRight));
	if (wDataSize != sizeof(DBO_GR_ManagerRight)) return false;
	
	DBO_GR_ManagerRight* pRight = (DBO_GR_ManagerRight*)pData;

	SUB_GA_ManagerRight ManagerRight;
	ZeroMemory(&ManagerRight, sizeof(ManagerRight));
	ManagerRight.dwGroupID = pRight->dwGroupID;
	ManagerRight.lManagerRight = pRight->lManagerRight;
	ManagerRight.lCaptainRight = pRight->lCaptainRight;
	/*
	for (int i = 0; i < 30; i++)
	{
		if (g_lManagerRight[i] > 0 && (pRight->lManagerRight&g_lManagerRight[i]) != 0)
			ManagerRight.cbManagerRight[i] = 1;

		if (g_lCaptainRight[i] > 0 && (pRight->lCaptainRight&g_lCaptainRight[i]) != 0)
			ManagerRight.cbCaptainRight[i] = 1;
	}
	*/
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);
	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_MANAGER_RIGHT, &ManagerRight, sizeof(ManagerRight));

	return true;
}

bool CAttemperEngineSink::OnDBFreeTableLImit(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_FreeTableLimit));
	if (wDataSize != sizeof(DBO_GR_FreeTableLimit)) return false;


	DBO_GR_FreeTableLimit *pSet = (DBO_GR_FreeTableLimit*)pData;

	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pSet->dwGroupID);
	pGroupItem->GetGroupBattleRule()->cbFreeTableLimit = pSet->cbCount;

	SendDataToGroupMember(pSet->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_FREE_TABLE_LIMIT, pData, wDataSize, false);
	return true;
}

bool CAttemperEngineSink::OnDBBigWinnerShow(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_BigWinnerShow));
	if (wDataSize != sizeof(DBO_GR_BigWinnerShow)) return false;


	DBO_GR_BigWinnerShow *pSet = (DBO_GR_BigWinnerShow*)pData;

	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pSet->dwGroupID);
	pGroupItem->GetGroupBattleRule()->cbBigWinnerShow = pSet->cbShowMode;

	SendDataToGroupMember(pSet->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_BIGWINNER_SHOW_MODE, pData, wDataSize, false);
	return true;
}

bool CAttemperEngineSink::OnDBSetManagerRight(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_ManagerRight));
	if (wDataSize != sizeof(DBO_GR_ManagerRight)) return false;

	DBO_GR_ManagerRight * pResult = (DBO_GR_ManagerRight*)pData;
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
	if (pGroupItem)
	{
		if (pResult->dwTargetID == 0)
		{
			pGroupItem->GetGroupBattleRule()->lManagerRight = pResult->lManagerRight;
			pGroupItem->GetGroupBattleRule()->lCaptainRight = pResult->lCaptainRight;
		}
		else
		{
			tagIMGroupMember *pUserItem = pGroupItem->SearchMember(pResult->dwTargetID);
			if (pUserItem)
			{
				pUserItem->cbManagerRightFlag = TRUE;
				pUserItem->lMemberManagerRight = pResult->lManagerRight;
			}
		}

	}
	
	SUB_GA_ManagerRight ManagerRight;
	ZeroMemory(&ManagerRight, sizeof(ManagerRight));
	ManagerRight.dwGroupID = pResult->dwGroupID;
	ManagerRight.dwTargetID = pResult->dwTargetID;
	ManagerRight.lManagerRight = pResult->lManagerRight;
	ManagerRight.lCaptainRight = pResult->lCaptainRight;
	/*
	for (int i = 0; i < 30; i++)
	{
		if (g_lManagerRight[i] > 0 && (pResult->lManagerRight&g_lManagerRight[i]) != 0)
			ManagerRight.cbManagerRight[i] = 1;

		if (g_lCaptainRight[i] > 0 && (pResult->lCaptainRight&g_lCaptainRight[i]) != 0)
			ManagerRight.cbCaptainRight[i] = 1;
	}
	*/
	SendDataToGroupMember(pResult->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MANAGER_RIGHT, &ManagerRight, sizeof(SUB_GA_ManagerRight), false);

	return true;
}

bool CAttemperEngineSink::OnDBSetFlag(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBR_GR_SetFlag));
	if (wDataSize != sizeof(DBR_GR_SetFlag)) return false;

	DBR_GR_SetFlag * pResult = (DBR_GR_SetFlag*)pData;
	SendDataToGroupManager(pResult->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_SET_FLAG_RESULT, pData, wDataSize);

	CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
	if (pGroupItem)
	{
		tagIMGroupMember *pMember1= pGroupItem->SearchMember(pResult->dwOperateUserID);
		if (pMember1 && (pMember1->nMemberRight&GP_CAPTAIN))
		{
			m_pITCPNetworkEngine->SendData(dwContextID, MDM_GA_GROUP_SERVICE, SUB_GA_SET_FLAG_RESULT, pData, wDataSize);
		}

		tagIMGroupMember *pMember = pGroupItem->SearchMember(pResult->dwTargetUserID);
		if (pMember && (pMember->nMemberRight&GP_CAPTAIN))
		{
			CServerUserItem *pServerUserItem = m_ServerUserManager.SearchUserItem(pResult->dwTargetUserID);
			if (pServerUserItem != NULL && pServerUserItem->GetUserStatus() == US_GP_ONLINE && pServerUserItem->GetActiveGroupID() == pResult->dwGroupID)
				m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_GROUP_SERVICE, SUB_GA_SET_FLAG_RESULT, pData, wDataSize);
		}
	}
	return true;
}

bool CAttemperEngineSink::OnDBClearMedal(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBR_GR_ClearMedal));
	if (wDataSize != sizeof(DBR_GR_ClearMedal)) return false;
	DBR_GR_ClearMedal * pResult = (DBR_GR_ClearMedal*)pData;
	CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
	if (pGroupItem)
	{
		tagIMGroupMember *pGroupMember = NULL;
		INT_PTR nIndex = 0;
		do
		{	//ö�ٳ�Ա
			pGroupMember = pGroupItem->EnumMember(nIndex++);
			if (pGroupMember == NULL) break;

			pGroupMember->lScore = 0;

		} while (true);
	}
	SendDataToGroupMember(pResult->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_SET_CLEAR_MEDAL, pData, wDataSize, false);
	return true;
}

bool CAttemperEngineSink::OnDBFavoriteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBR_GR_SetFavoriteConfig));
	if (wDataSize != sizeof(DBR_GR_SetFavoriteConfig)) return false;
	DBR_GR_SetFavoriteConfig * pResult = (DBR_GR_SetFavoriteConfig*)pData;

	CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
	if (pGroupItem)
	{
		if (pGroupItem->SearchMember(pResult->dwMemberID))
			pGroupItem->SearchMember(pResult->dwMemberID)->dwFavoriteConfig = pResult->dwConfigID;
	}

	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);
	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_FAVORITE_CONFIG, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBRollMsg(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_RollMsg));
	if (wDataSize != sizeof(DBO_GR_RollMsg)) return false;
	DBO_GR_RollMsg * pResult = (DBO_GR_RollMsg*)pData;

	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
	if (pGroupItem != NULL )
	{
		tagClubRollMsg RollMsg;
		RollMsg.cbRollMsgFlag = pResult->cbFlag;
		RollMsg.nRollMsgTime = pResult->nTime;
		CopyMemory(RollMsg.szRollMsg, pResult->szRollMsg, sizeof(RollMsg.szRollMsg));
		if (pResult->cbType == 1)
			RollMsg.dwUserID = pGroupItem->GetGroupProperty()->dwCreaterID;
		else
			RollMsg.dwUserID = pResult->dwUserID;
		
		pGroupItem->AddRollMsg(RollMsg);
	}


	//��������
	SUB_GA_RollMsg RollMsg;
	ZeroMemory(&RollMsg, sizeof(RollMsg));
	RollMsg.dwGroupID = pResult->dwGroupID;
	RollMsg.cbFlag = pResult->cbFlag;
	RollMsg.nTime = pResult->nTime;
	CopyMemory(RollMsg.szRollMsg, pResult->szRollMsg, sizeof(RollMsg.szRollMsg));
	RollMsg.cbMsgType = pResult->cbType;
	if (pResult->cbType==1)
		SendDataToGroupMember(pResult->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_ROLL_MSG, &RollMsg, sizeof(RollMsg), true);
	else if (pResult->cbType == 2)
	{
		SendDataToGroupTeamer(pResult->dwGroupID, pResult->dwUserID, MDM_GA_GROUP_SERVICE, SUB_GA_ROLL_MSG, &RollMsg, sizeof(RollMsg));

		RollMsg.cbMsgType = 3;
		WORD wBindIndex = LOWORD(dwContextID);
		tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);
		m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_GROUP_SERVICE, SUB_GA_ROLL_MSG, &RollMsg, sizeof(RollMsg));
	}
	return true;
}

bool CAttemperEngineSink::OnDBGroupName(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_GroupName));
	if (wDataSize != sizeof(DBO_GR_GroupName)) return false;
	DBO_GR_GroupName * pResult = (DBO_GR_GroupName*)pData;

	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
	if (pGroupItem != NULL)
	{
		CopyMemory(pGroupItem->GetGroupProperty()->szGroupName,pResult->szGroupName, sizeof(TCHAR)*GROUP_LEN_NAME);
	}

	SendDataToGroupMember(pResult->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_NAME, pData, wDataSize, false);

	return true;
}

bool CAttemperEngineSink::OnDBRoomDetailInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize >= sizeof(DBO_GR_QueryRoomDetail_List));
	if (wDataSize < sizeof(DBO_GR_QueryRoomDetail_List)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_ROOM_DETAIL_INFO, pData, wDataSize);
	return true;
}

bool CAttemperEngineSink::OnDBCaptainList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize >= sizeof(DBO_GR_Captain_List));
	if (wDataSize < sizeof(DBO_GR_Captain_List)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_CAPTAIN_LIST, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBAddTeamer(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize > sizeof(DBO_GR_Add_Teamer));
	if (wDataSize <= sizeof(DBO_GR_Add_Teamer)) return false;

	DBO_GR_Add_Teamer * pResult = (DBO_GR_Add_Teamer*)pData;
	SendDataToGroupMember(pResult->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_ADD_TEAMER, pData, wDataSize, false);

	
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
	if (pGroupItem != NULL)
	{
		BYTE *p = (BYTE*)pData;
		tagIMAddTeamer*pTeamerItem = (tagIMAddTeamer*)(p + sizeof(DBO_GR_Add_Teamer));
		for (int i = 0; i < pResult->nCount; i++)
		{
			if (pTeamerItem->cbSuccess == 1)
			{
				tagIMGroupMember *pMember = pGroupItem->SearchMember(pTeamerItem->dwUserID);
				if (pResult->cbMode == 0)//�Ƴ����
				{
					//������Ƕӳ����ҵ����������
					if (pMember->nMemberRight&GP_CAPTAIN)
					{
						INT_PTR nIndex = 0;
						tagIMGroupMember *pUser = NULL;
						do
						{	//ö�ٳ�Ա
							pUser = pGroupItem->EnumMember(nIndex++);
							if (pUser == NULL) break;

							if (pUser->dwCaptainID == pMember->dwMemberID)
							{
								pUser->dwCaptainID = pMember->dwCaptainID;
								if (pUser->dwCaptainID == 0 && pUser->nMemberRight == GP_TEAMER)
									pUser->nMemberRight = GP_MEMBER;

								SUB_GA_MemberUpdate MemberUpdate;
								ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));
								MemberUpdate.dwGroupID = pResult->dwGroupID;
								CopyMemory(&MemberUpdate.GroupMember, pUser, sizeof(tagIMGroupMember));
								SendDataToGroupMember(pResult->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate));
								
								UpdateGroupExtraInfo(pResult->dwGroupID, pUser->dwMemberID);
							}

						} while (true);
					}

					pMember->dwCaptainID = 0;
					pMember->nMemberRight = GP_MEMBER;
				}
				else
				{
					pMember->dwCaptainID = pResult->dwCaptainID;
					pMember->nMemberRight = GP_TEAMER;
				}

				SUB_GA_MemberUpdate MemberUpdate;
				ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));
				MemberUpdate.dwGroupID = pResult->dwGroupID;
				CopyMemory(&MemberUpdate.GroupMember, pMember, sizeof(tagIMGroupMember));
				SendDataToGroupMember(pResult->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate));

				UpdateGroupExtraInfo(pResult->dwGroupID, pMember->dwMemberID);
			}

			pTeamerItem++;
		}


	}

	
	return true;
}

bool CAttemperEngineSink::OnDBTeamDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize >= sizeof(DBO_GR_TeamDayRecord_List));
	if (wDataSize < sizeof(DBO_GR_TeamDayRecord_List)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_TEAM_DAY_RECORD, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBTeamerDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize >= sizeof(DBO_GR_TeamerDayRecord_List));
	if (wDataSize < sizeof(DBO_GR_TeamerDayRecord_List)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_TEAMER_DAY_RECORD, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBGroupPrivacy(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_Privacy));
	if (wDataSize != sizeof(DBO_GR_Privacy)) return false;

	DBO_GR_Privacy * pPrivacy = (DBO_GR_Privacy*)pData;

	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pPrivacy->dwGroupID);
	if (pGroupItem != NULL)
	{
		pGroupItem->GetGroupBattleRule()->cbGroupIDShow = pPrivacy->cbShowGroupID;
	}

	SendDataToGroupMember(pPrivacy->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_GROUP_PRIVACY, pData, wDataSize, false);
	return true;
}

bool CAttemperEngineSink::OnDBCaptainPoint(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_CaptainPoint));
	if (wDataSize != sizeof(DBO_GR_CaptainPoint)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);
	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_CAPTAIN_POINT, pData, wDataSize);

	DBO_GR_CaptainPoint * pSetPoint = (DBO_GR_CaptainPoint*)pData;
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pSetPoint->dwGroupID);
	if (pGroupItem != NULL)
	{
		tagIMGroupMember *pMember = pGroupItem->SearchMember(pSetPoint->dwCaptainID);
		if (pMember)
			pMember->nAwardPoint = pSetPoint->wPoint;

		SUB_GA_MemberUpdate MemberUpdate;
		ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));
		MemberUpdate.dwGroupID = pSetPoint->dwGroupID;
		CopyMemory(&MemberUpdate.GroupMember, pMember, sizeof(tagIMGroupMember));
		SendDataToGroupMember(pSetPoint->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate));
	}

	return true;
}

bool CAttemperEngineSink::OnDBBoxInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize >= sizeof(DBO_GR_BoxInfo));
	if (wDataSize < sizeof(DBO_GR_BoxInfo)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_BOX_INFO, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBGetAwardScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_GetAwardScoreResult));
	if (wDataSize != sizeof(DBO_GR_GetAwardScoreResult)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_GETBOXSCORE_RESULT, pData, wDataSize);

	DBO_GR_GetAwardScoreResult *pResult = (DBO_GR_GetAwardScoreResult*)pData;
	CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pResult->dwGroupID);
	if (pGroupItem)
	{
		tagIMGroupMember *pMemberItem = pGroupItem->SearchMember(pResult->dwUserID);
		if (pMemberItem)
		{
			pMemberItem->lScore += pResult->lGetScore;
			pMemberItem->nAwardPoint = pResult->wAwardPoint;

			SUB_GA_MemberUpdate MemberUpdate;
			ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));
			MemberUpdate.dwGroupID = pResult->dwGroupID;
			CopyMemory(&MemberUpdate.GroupMember, pMemberItem, sizeof(tagIMGroupMember));
			SendDataToGroupMember(pResult->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate));
		}
	}
	return true;
}

bool CAttemperEngineSink::OnDBInvitationCode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_InvitationCode));
	if (wDataSize != sizeof(DBO_GR_InvitationCode)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_INVITATION_CODE, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBBlindCode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_BlindCodeRes));
	if (wDataSize != sizeof(DBO_GR_BlindCodeRes)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	DBO_GR_BlindCodeRes *pBlind = (DBO_GR_BlindCodeRes*)pData;
	CGroupItem *pGroupItem = m_GroupInfoManager.SearchGroupItem(pBlind->dwGroupID);
	if (pGroupItem)
	{
		tagIMGroupMember *pMemberInfo = pGroupItem->SearchMember(pBlind->dwUserID);
		if (pMemberInfo)
		{
			pMemberInfo->nMemberRight = GP_TEAMER;
			pMemberInfo->dwCaptainID = pBlind->dwCaptainID;

			UpdateGroupExtraInfo(pBlind->dwGroupID, pBlind->dwUserID);

			SUB_GA_MemberUpdate MemberUpdate;
			ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));
			MemberUpdate.dwGroupID = pBlind->dwGroupID;
			CopyMemory(&MemberUpdate.GroupMember, pMemberInfo, sizeof(tagIMGroupMember));
			SendDataToGroupMember(pBlind->dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate));
		}
	}

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_BLIND_CODE_RES, pData, wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBQueryScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У������
	//ASSERT(wDataSize == sizeof(DBO_GR_QueryScoreResult));
	if (wDataSize != sizeof(DBO_GR_QueryScoreResult)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GA_LOGIC_SERVICE, SUB_GA_QUERY_SCORE_RES, pData, wDataSize);
	return true;
}

bool CAttemperEngineSink::OnDBModifyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wDataSize == sizeof(DBO_GR_ModifyInfo));
	if (wDataSize != sizeof(DBO_GR_ModifyInfo)) return false;

	//��ȡ����
	DBO_GR_ModifyInfo * pModifyInfo = (DBO_GR_ModifyInfo *)pData;

	//����Ⱥ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(pModifyInfo->dwGroupID);
	if (pGroupItem != NULL)
	{
		//�����˳�Ա
		tagIMGroupMember * pGroupMember = pGroupItem->SearchMember(pModifyInfo->dwUserID);
		if (pGroupMember != NULL)
		{
			if (pModifyInfo->cbFlag[0])
			{
				CopyMemory(pGroupMember->szQQ, pModifyInfo->szQQ, sizeof(pGroupMember->szQQ));
			}
			if (pModifyInfo->cbFlag[1])
			{
				CopyMemory(pGroupMember->szWX, pModifyInfo->szWX, sizeof(pGroupMember->szWX));
			}
			if (pModifyInfo->cbFlag[2])
			{
				CopyMemory(pGroupMember->szPhone, pModifyInfo->szPhone, sizeof(pGroupMember->szPhone));
			}
			
			SUB_GA_MemberUpdate MemberUpdate;
			ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));
			MemberUpdate.dwGroupID = pModifyInfo->dwGroupID;
			CopyMemory(&MemberUpdate.GroupMember, pGroupMember, sizeof(tagIMGroupMember));
			m_pITCPNetworkEngine->SendData(dwContextID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate));

			//��ȡ����
			SUB_GA_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));
			_stprintf(OperateSuccess.szOperateNotice, _T("�����޸ĳɹ���"));

			m_pITCPNetworkEngine->SendData(dwContextID, MDM_GA_LOGIC_SERVICE, SUB_GA_OPERATE_SUCCESS, &OperateSuccess, sizeof(OperateSuccess));

			//�ӳ�
			if (pModifyInfo->nMemberRight == GP_CAPTAIN)
			{
				//�ҵ����������
				INT_PTR nIndex = 0;
				tagIMGroupMember *pUser = NULL;
				do
				{	//ö�ٳ�Ա
					pUser = pGroupItem->EnumMember(nIndex++);
					if (pUser == NULL) break;

					if (pUser->dwCaptainID == pModifyInfo->dwUserID)
					{
						UpdateGroupExtraInfo(pModifyInfo->dwGroupID, pUser->dwMemberID);
					}

				} while (true);
			}
			else if (pModifyInfo->nMemberRight==GP_CREATOR)
			{
				//�ҵ����������
				INT_PTR nIndex = 0;
				tagIMGroupMember *pUser = NULL;
				do
				{	//ö�ٳ�Ա
					pUser = pGroupItem->EnumMember(nIndex++);
					if (pUser == NULL) break;

					if (pUser->dwCaptainID == 0)
					{
						UpdateGroupExtraInfo(pModifyInfo->dwGroupID, pUser->dwMemberID);
					}

				} while (true);
			}
		}
	}



	return true;
}

//�û���¼
bool CAttemperEngineSink::OnEventUserLogon(CLocalUserItem * pServerUserItem)
{
	//����У��
	//ASSERT(pServerUserItem != NULL);
	if (pServerUserItem == NULL) return true;

	//��ѯȺ��
	if (pServerUserItem->GetGroupCount()>0)
	{
		//����ṹ
		SUB_GA_QueryGroup * pQueryGroup = (SUB_GA_QueryGroup *)(m_cbBuffer);
		ZeroMemory(pQueryGroup, sizeof(SUB_GA_QueryGroup));

		//���ñ���
		pQueryGroup->dwUserID = pServerUserItem->GetUserInfo()->dwUserID;
		WORD wGroupCount = pServerUserItem->GetGroupCount();
		DWORD *pGroupData = new DWORD[wGroupCount];
		ZeroMemory(pGroupData, sizeof(DWORD)*wGroupCount);
		//����Ⱥ��
		CopyMemory(pGroupData, pServerUserItem->GetGroupData(), wGroupCount*sizeof(DWORD));
		
		//��������
		WORD wPacketSize = sizeof(SUB_GA_QueryGroup);
		tagIMGroupProperty * pGroupProperty = NULL;
		tagIMGroupExtraInfo * pGroupExtraInfo = NULL;
		DWORD dwGroupID = 0;
		DWORD * pdwGroupID = pGroupData;

		SUB_GA_MemberStatus Status;
		Status.dwUserID = pServerUserItem->GetUserInfo()->dwUserID;
		Status.cbStatus = US_GP_ONLINE;
		CString cc;
		cc.Format(_T("���userid=%d��½�����ֲ�����=%d"), pServerUserItem->GetUserID(), wGroupCount);
		SendLogData(cc);
		//��ѯȺ��
		for (int i = 0; i<wGroupCount; i++)
		{
			dwGroupID = *(pdwGroupID + i);
			CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(dwGroupID);
			if (pGroupItem == NULL) continue;
			tagIMGroupMember *pMember = pGroupItem->SearchMember(pQueryGroup->dwUserID);
			if (pMember == NULL)continue;
			
			pMember->cbStatus = US_GP_ONLINE;
			//֪ͨ�������
			SendDataToGroupMember(dwGroupID, MDM_GA_GROUP_SERVICE, SUB_GA_MEMBER_STATUS, &Status, sizeof(Status));

			pGroupProperty = (tagIMGroupProperty *)(m_cbBuffer + wPacketSize);
			pGroupExtraInfo = (tagIMGroupExtraInfo *)(pGroupProperty + 1);

			//�����ж�
			if (wPacketSize + sizeof(tagIMGroupProperty)+sizeof(tagIMGroupExtraInfo)>sizeof(m_cbBuffer))
			{
				//��������
				m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_GROUP_SERVICE, SUB_GA_QUERY_PROPERTY, m_cbBuffer, wPacketSize);
				wPacketSize = sizeof(SUB_GA_QueryGroup);
				pQueryGroup->wGroupCount=0;

			}

			//��������
			CopyMemory(pGroupProperty, pGroupItem->GetGroupProperty(), sizeof(tagIMGroupProperty));
			pGroupExtraInfo->dwGroupID = dwGroupID;
			pGroupExtraInfo->dwUserID = pQueryGroup->dwUserID;
			tagIMGroupMember *pUser = pGroupItem->SearchMember(pQueryGroup->dwUserID);
			if (pUser)
			{
				cc.Format(_T("���%d�ھ��ֲ�%d=%d�����=%d,�ӳ�=%d"),pServerUserItem->GetUserID(),i, dwGroupID, pUser->nMemberRight,pUser->dwCaptainID);
				SendLogData(cc);
				pGroupExtraInfo->cbGroupIDShow = pGroupItem->GetGroupBattleRule()->cbGroupIDShow;
				pGroupExtraInfo->nMemberRight = pUser->nMemberRight;

				if (pUser->dwCaptainID != 0)
				{
					tagIMGroupMember *pCaptain = pGroupItem->SearchMember(pUser->dwCaptainID);
					if (pCaptain)
					{
						pGroupExtraInfo->lInvitationCode = pCaptain->lInvitationCode;
						CopyMemory(pGroupExtraInfo->szQQ, pCaptain->szQQ, sizeof(pGroupExtraInfo->szQQ));
						CopyMemory(pGroupExtraInfo->szWX, pCaptain->szWX, sizeof(pGroupExtraInfo->szWX));
						CopyMemory(pGroupExtraInfo->szPhone, pCaptain->szPhone, sizeof(pGroupExtraInfo->szPhone));
					}
					else
						SendLogData(_T("Captatin null"));
				}
				else
				{
					tagIMGroupMember *pCreator = pGroupItem->SearchMember(pGroupItem->GetGroupProperty()->dwCreaterID);
					if (pCreator)
					{
						pGroupExtraInfo->lInvitationCode = pCreator->lInvitationCode;
						CopyMemory(pGroupExtraInfo->szQQ, pCreator->szQQ, sizeof(pGroupExtraInfo->szQQ));
						CopyMemory(pGroupExtraInfo->szWX, pCreator->szWX, sizeof(pGroupExtraInfo->szWX));
						CopyMemory(pGroupExtraInfo->szPhone, pCreator->szPhone, sizeof(pGroupExtraInfo->szPhone));
					}
				}
			
			}

			pQueryGroup->wGroupCount++;
			wPacketSize += sizeof(tagIMGroupProperty) + sizeof(tagIMGroupExtraInfo);

		}

		//�����ж�
		if (wPacketSize>0)
		{
			//��������
			m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_GROUP_SERVICE, SUB_GA_QUERY_PROPERTY, m_cbBuffer, wPacketSize);

			m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_GROUP_SERVICE, SUB_GA_QUERY_PROPERTY_END);
		}

		delete[] pGroupData;

	}
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai **** ��������� ��¼�ɹ� userid = %d socketid = %d"), pServerUserItem->GetUserID(), pServerUserItem->GetSocketID());
	//OutputDebugString(szInfo);
	//��������
	m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_GROUP_SERVICE, SUB_GA_LOGON_SUCCESS, 0, 0);

	return true;
}

//�����˳�ԱУ��
bool CAttemperEngineSink::CheckClubUser(DWORD dwGroupID, DWORD dwUserID)
{
	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(dwUserID);
	if (pServerUserItem == NULL) return false;

	//����Ⱥ��
	CGroupItem * pGroupItem = m_GroupInfoManager.SearchGroupItem(dwGroupID);
	if (pGroupItem == NULL)
	{
		//����ṹ
		CMD_GA_OperateFailed OperateFailed;
		ZeroMemory(&OperateFailed, sizeof(OperateFailed));

		//���ñ���
		OperateFailed.wErrorCode = 0;
		lstrcpynW(OperateFailed.szErrorMessage, L"ϵͳδ��ѯ���ô����˵���Ϣ����ȡʧ�ܣ�", CountArray(OperateFailed.szErrorMessage));

		//��������
		m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_BATTLE_SERVICE, SUB_GA_OPERATE_FAILED, &OperateFailed, sizeof(OperateFailed));

		return false;
	}

	//���ҳ�Ա
	if (pGroupItem->SearchMember(dwUserID) == NULL)
	{
		//����ṹ
		CMD_GA_OperateFailed OperateFailed;
		ZeroMemory(&OperateFailed, sizeof(OperateFailed));

		//���ñ���
		OperateFailed.wErrorCode = 0;
		lstrcpynW(OperateFailed.szErrorMessage, L"��Ǹ���㲻�Ǹô����˵ĳ�Ա���޷���ȡ��", CountArray(OperateFailed.szErrorMessage));

		//��������
		m_pITCPNetworkEngine->SendData(pServerUserItem->GetSocketID(), MDM_GA_GROUP_SERVICE, SUB_GA_ENTER_FAILURE, &OperateFailed, sizeof(OperateFailed));

		return false;
	}

	return true;
}
//�ַ��ж�
int CAttemperEngineSink::CheckClubName(LPCWSTR pszGroupName)
{
	//����У��
	if (pszGroupName == NULL)  return -1;

	//�ַ�����У��
	int nLen = lstrlenW(pszGroupName);
	if (nLen<2 || nLen>6) return 1;

	//�ַ�У��
	for (int i = 0; i < nLen; i++)
	{
		//СдӢ���ַ�
		if (pszGroupName[i] >= 0x61 && pszGroupName[i] <= 0x7a) continue;
		//��дӢ���ַ�
		if (pszGroupName[i] >= 0x41 && pszGroupName[i] <= 0x5a) continue;
		//���ְ���
		if (pszGroupName[i] >= 0x30 && pszGroupName[i] <= 0x39) continue;
		//�����ַ�
		if (pszGroupName[i] >= 0x4e00 && pszGroupName[i] <= 0x9fcb) continue;
		//��չA-D
		if (pszGroupName[i] >= 0x3400 && pszGroupName[i] <= 0x4db5) continue;
		if (pszGroupName[i] >= 0x20000 && pszGroupName[i] <= 0x2a6d6) continue;
		if (pszGroupName[i] >= 0x2a700 && pszGroupName[i] <= 0x2b734) continue;
		if (pszGroupName[i] >= 0x2b740 && pszGroupName[i] <= 0x2b81d) continue;

		return 2;
	}
	return 0;
}

//������־
void CAttemperEngineSink::SendLogData(const TCHAR * pszFormat, ...)
{
	//������������
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	//����Ŀ¼
	TCHAR szFilePack[MAX_PATH];
	GetModuleFileName(NULL, szFilePack, MAX_PATH);
	(wcsrchr(szFilePack, '\\'))[0] = 0;
	_stprintf(szFilePack, _T("%s\\��������־Ŀ¼"), szFilePack);
	DWORD dwAttrib = GetFileAttributes(szFilePack);
	if (INVALID_FILE_ATTRIBUTES == dwAttrib)
	{
		CreateDirectory(szFilePack, NULL);
	}

	//�����ļ�
	CTime time = CTime::GetCurrentTime();
	static CString strFile;
	if (strFile.IsEmpty())
		strFile.Format(_T("%s//[%s]��־%d-%d-%d-%02d%02d%02d.log"), szFilePack,_T("������"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	//д�ļ�
	CStdioFile myFile;
	BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		if (myFile.GetLength() > 50 * 1024 * 1024)//50M���ļ� 
		{
			myFile.Close();
			strFile.Format(_T("%s//[%s]��־%d-%d-%d-%02d%02d%02d.log"), szFilePack, _T("������"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
			if (bOpen == false)
				return;
		}

		CString strtip;
		strtip.Format(TEXT("%d/%d/%d_%d:%d:%d--%s\n"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pszFormat);

		myFile.SeekToEnd();
		myFile.WriteString(strtip);
		myFile.Flush();
		myFile.Close();
	}

	//��ԭ�����趨
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}

/*
//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkSubMBQueryGameServer(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//У������
	//ASSERT(wDataSize == sizeof(CMD_MB_QueryGameServer));
	if (wDataSize != sizeof(CMD_MB_QueryGameServer))
	{
		return false;
	}

	CMD_MB_QueryGameServer *pGameServer = (CMD_MB_QueryGameServer *)pData;

	//��������
	CMD_CS_C_QueryGameServer QueryGameServer;
	ZeroMemory(&QueryGameServer, sizeof(CMD_CS_C_QueryGameServer));
	QueryGameServer.dwUserID = pGameServer->dwUserID;
	QueryGameServer.dwKindID = pGameServer->dwKindID;
	QueryGameServer.cbIsJoinGame = pGameServer->cbIsJoinGame;
	QueryGameServer.dwSocketID = dwSocketID;
	QueryGameServer.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;

	//��������
	if (m_pPrsnlRmITCPSocketService)
	{
		m_pPrsnlRmITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_GAME_SERVER, &QueryGameServer, sizeof(CMD_CS_C_QueryGameServer));
	}


	return true;
}

//������������
bool CAttemperEngineSink::OnTCPNetworkSubMBSearchServerTable(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//У������
	//ASSERT(wDataSize == sizeof(CMD_MB_SearchServerTable));
	if (wDataSize != sizeof(CMD_MB_SearchServerTable))
	{
		return false;
	}

	CMD_MB_SearchServerTable *pSearchServerTable = (CMD_MB_SearchServerTable *)pData;

	CMD_CS_C_SearchTable SearchTable;
	ZeroMemory(&SearchTable, sizeof(CMD_CS_C_SearchTable));

	SearchTable.dwSocketID = dwSocketID;
	SearchTable.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	SearchTable.dwPersonalRoomID = pSearchServerTable->dwPersonalRoomID;

	if (m_pPrsnlRmITCPSocketService)
	{
		m_pPrsnlRmITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_SEARCH_TABLE, &SearchTable, sizeof(CMD_CS_C_SearchTable));
	}
	return true;
}

//��ɢ������������
bool CAttemperEngineSink::OnTCPNetworkSubMBDissumeSearchServerTable(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//У������
	//ASSERT(wDataSize == sizeof(CMD_MB_SearchServerTable));
	if (wDataSize != sizeof(CMD_MB_SearchServerTable))
	{
		return false;
	}

	CMD_MB_SearchServerTable *pSearchServerTable = (CMD_MB_SearchServerTable *)pData;

	CMD_CS_C_SearchTable SearchTable;
	ZeroMemory(&SearchTable, sizeof(CMD_CS_C_SearchTable));

	SearchTable.dwSocketID = dwSocketID;
	SearchTable.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	SearchTable.dwPersonalRoomID = pSearchServerTable->dwPersonalRoomID;

	if (m_pPrsnlRmITCPSocketService)
	{
		m_pPrsnlRmITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISSUME_SEARCH_TABLE, &SearchTable, sizeof(CMD_CS_C_SearchTable));
	}


	return true;
}

//Լս��������
bool CAttemperEngineSink::OnTCPNetworkSubMBQueryRoomPassword(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//У������
	//ASSERT(wDataSize == sizeof(CMD_MB_SearchServerTable));
	if (wDataSize != sizeof(CMD_MB_SearchServerTable))
	{
		return false;
	}

	CMD_MB_SearchServerTable *pSearchServerTable = (CMD_MB_SearchServerTable *)pData;

	CMD_CS_C_SearchTable SearchTable;
	ZeroMemory(&SearchTable, sizeof(CMD_CS_C_SearchTable));

	SearchTable.dwSocketID = dwSocketID;
	SearchTable.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	SearchTable.dwPersonalRoomID = pSearchServerTable->dwPersonalRoomID;

	if (m_pPrsnlRmITCPSocketService)
	{
		m_pPrsnlRmITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_S_QUERY_ROOM_PASSWORD, &SearchTable, sizeof(CMD_CS_C_SearchTable));
	}
	return true;
}

//��ѯԼս�����б�
bool CAttemperEngineSink::OnTCPNetworkSubMBQueryPersonalRoomList(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//У������
	//ASSERT(wDataSize == sizeof(CMD_MB_QeuryPersonalRoomList));
	if (wDataSize != sizeof(CMD_MB_QeuryPersonalRoomList))
	{
		return false;
	}

	CMD_MB_QeuryPersonalRoomList *pQueryPersonalRoomList = (CMD_MB_QeuryPersonalRoomList *)pData;

	CMD_MB_SC_QeuryPersonalRoomList  SC_QeuryPersonalRoomList;
	SC_QeuryPersonalRoomList.dwUserID = pQueryPersonalRoomList->dwUserID;
	SC_QeuryPersonalRoomList.dwSocketID = dwSocketID;

	//��������
	if (m_pPrsnlRmITCPSocketService)
	{
		m_pPrsnlRmITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_S_QUERY_PERSONAL_ROOM_LIST, &SC_QeuryPersonalRoomList, sizeof(CMD_MB_SC_QeuryPersonalRoomList));
	}



	return true;
}
*/
//////////////////////////////////////////////////////////////////////////

