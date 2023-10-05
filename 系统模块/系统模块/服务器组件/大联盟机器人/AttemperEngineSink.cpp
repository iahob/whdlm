#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



////////////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

#define IDI_LOAD_AI_PARAMETER		1									//����AI����

#define IDI_AI_IN_OUT				2									//AI������뿪���

#define IDI_AI_IN_OUT_STEP			3									//AI���������뿪

#define IDI_CONNECT_CORRESPOND		4									//Э��������ʱ��

#define IDI_SEND_GROUP_ONLINE		5									//���;��ֲ����������Ϣ

#define IDI_CHECK_TABLE_IS_NULL		6									//�����ֲ������Ƿ�Ϊ��

#define IDI_CHECK_AI_LEAVE			7									//AI���߻��˳����
//////////////////////////////////////////////////////////////////////////////////
//ʱ�䶨�����

#define TIME_LOAD_AI_PARAMETER			5*1000L							//����AI����

#define TIME_AI_IN_OUT					5*60*1000L						//ÿһ��Сʱ���һ�Σ����ô���������

#define TIME_AI_SEND_GROUP_ONLINE		5*60*1000L						//����AI����

#define TIME_AI_CHECK_TABLE_IS_NULL		3*60*1000L						//ÿ�����Ӽ��һ�ο���

#define TIME_AI_CHECK_AI_LEAVE			2*60*1000L						//ÿ�����Ӽ��һ�ο���




//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_bNeekPrsnlRmCorrespond = true;
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
	m_pPrsnlRmITCPSocketService=NULL;
	m_pLogServerTCPSocketService = NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;
	
	//��������
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;
	m_pIDBCorrespondManager=NULL;

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{

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
	m_pNormalParameter = new tagBindParameter[m_pInitParameter->m_wMaxConnect];

	m_pITimerEngine->SetTimer(IDI_CHECK_TABLE_IS_NULL, TIME_AI_CHECK_TABLE_IS_NULL, -1, 0);

	m_pITimerEngine->SetTimer(IDI_CHECK_AI_LEAVE, TIME_AI_CHECK_AI_LEAVE, -1, 0);

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_bNeekPrsnlRmCorrespond=true;
	m_bNeekLogServer = true;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pPrsnlRmITCPSocketService=NULL;
	m_pLogServerTCPSocketService = NULL;
	m_pITCPNetworkEngine=NULL;
	
	//��������
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;



	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_LOAD_SERVICE_CONFIG:	//��������
		{
			OutputDebugString(TEXT("ptdtai **** OnEventControl"));
			m_AIManager.SetDBCorrespondManager(m_pIDBCorrespondManager,m_pITCPNetworkEngine);

			m_pITimerEngine->SetTimer(IDI_LOAD_AI_PARAMETER, TIME_LOAD_AI_PARAMETER, 1, 0);

			m_pITimerEngine->SetTimer(IDI_AI_IN_OUT, TIME_AI_IN_OUT, -1, 0);

			return true;
		}
	case CT_CONNECT_CORRESPOND:		//����Э��
		{
			//��������
			tagAddressInfo * pCorrespondAddress = &m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

			//������ʾ
			TCHAR szString[512] = TEXT("");
			_sntprintf_s(szString, CountArray(szString), TEXT("��������Э�������� [ %s:%d ]"), pCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString, TraceLevel_Normal);

			return true;
		}
	case UI_CLEAR_AI:
		{
						m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_CLEAR_AI, DBR_GR_CLEAR_AI, 0L, NULL, 0L);
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
	try
	{
		switch (dwTimerID)
		{
		case IDI_LOAD_AI_PARAMETER:		
			{				//��������
									  m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_LOAD_AI_PARAMETER, DBR_GR_LOAD_AI_PARAMETER, 0L, NULL, 0L);


				return true;
			}
		case IDI_AI_IN_OUT:
			{
				OutputDebugString(TEXT("ptdtai *** IDI_AI_IN_OUT �������"));
				// һ������Ӻ�ʼ�˳�
				DWORD dwRand = rand() % 2 + 1;
				m_pITimerEngine->SetTimer(IDI_AI_IN_OUT_STEP, dwRand * 60 * 1000, 1, 0);
	
				return true;
			}
		case IDI_AI_IN_OUT_STEP:
			{
				OutputDebugString(TEXT("ptdtai *** IDI_AI_IN_OUT ������� ������뿪"));
				m_AIManager.AIInOrOut(m_dwGroupSocketID);
				return true;
			}
		case IDI_CONNECT_CORRESPOND:	//����Э��
			{
				//��������
				tagAddressInfo * pCorrespondAddress = &m_pInitParameter->m_CorrespondAddress;
				m_pITCPSocketService->Connect(pCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

				//������ʾ
				TCHAR szString[512] = TEXT("");
				_sntprintf_s(szString, CountArray(szString), TEXT("��������Э�������� [ %s:%d ]"), pCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

				//��ʾ��Ϣ
				CTraceService::TraceString(szString, TraceLevel_Normal);

				return true;
			}
		case IDI_SEND_GROUP_ONLINE:
			{
				OutputDebugString(TEXT("ptdtai *** IDI_SEND_GROUP_ONLINE"));
				m_AIManager.SendAIToGroupServer(0, 0, m_dwGroupSocketID);
				return true;
			}
		case IDI_CHECK_TABLE_IS_NULL:
			{
				OutputDebugString(TEXT("ptdtai *** IDI_CHECK_TABLE_IS_NULL"));
				m_AIManager.CheckGroupRoomIsNull();
				return true;
			}
		case IDI_CHECK_AI_LEAVE:
			{
				OutputDebugString(TEXT("ptdtai *** IDI_CHECK_AI_LEAVE"));
				m_AIManager.LetUserLeave();
				return true;
			}
		}
		

	}catch(...)
	{
		TCHAR szMessage[128];
		_sntprintf_s(szMessage,CountArray(szMessage),TEXT("��ʱ���쳣��TimerID=%d"),dwTimerID);
		CTraceService::TraceString(szMessage,TraceLevel_Warning);
	}

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	SendLogData(TEXT("OnEventDataBase wRequestID = %d start"), wRequestID);

	bool bReturnVal = false;
	switch (wRequestID)
	{
	case DBO_GR_LOAD_AI_PARAMETER_RESULT:	//��������
		{
			bReturnVal = OnDBGroupAIParameter(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_LOAD_AI_PARAMETER_END:
		{
			bReturnVal = OnDBGroupAIParameterEnd(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_LOAD_AI_USER_RESULT:
		{
			bReturnVal = OnDBLoadAIUser(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_LOAD_AI_USER_END:
		{
			bReturnVal = OnDBLoadAIUserEnd(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_AI_TIME_CHANGE:
		{
			bReturnVal = OnDBLoadAITimeChange(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_AI_WEIGHT_IN:
		{
			bReturnVal = OnDBLoadAITimeWeightIn(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_AI_WEIGHT_OUT:
		{
			bReturnVal = OnDBLoadAITimeWeightOut(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_AI_OPERATE_RESULT:
		{
			bReturnVal = OnDBAIOperateResult(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_CLEAR_AI_RESULT:
		{
			bReturnVal = OnDBClearAIResult(dwContextID, pData, wDataSize);
			break;
		}
	
	}
	
	
	if (!bReturnVal)
	{
		SendLogData(TEXT("OnEventDataBase wRequestID = %d end false"), wRequestID);
	}
	SendLogData(TEXT("OnEventDataBase wRequestID = %d end"), wRequestID);
	return bReturnVal;
}


//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//������ʾ
	TCHAR szDescribe[128] = TEXT("");
	_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("Э������������ʧ�� [ %ld ]��%ld �����������"),
		cbShutReason, m_pInitParameter->m_wConnectTime);

	//��ʾ��Ϣ
	CTraceService::TraceString(szDescribe, TraceLevel_Warning);

	//����ʱ��
	//ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

	return false;

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//Э������
	if (wServiceID == NETWORK_CORRESPOND)
	{
		//�����ж�
		if (nErrorCode != 0)
		{
			//������ʾ
			TCHAR szDescribe[128] = TEXT("");
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("Э������������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode, m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe, TraceLevel_Warning);

			//����ʱ��
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڷ�����Ϸ����ע����Ϣ..."), TraceLevel_Normal);

		//��������
		CMD_CS_C_RegisterPlaza RegisterPlaza;
		ZeroMemory(&RegisterPlaza, sizeof(RegisterPlaza));

		//���ñ���
		lstrcpyn(RegisterPlaza.szServerName, m_pInitParameter->m_szServerName, CountArray(RegisterPlaza.szServerName));
		lstrcpyn(RegisterPlaza.szServerAddr, m_pInitParameter->m_ServiceAddress.szAddress, CountArray(RegisterPlaza.szServerAddr));

		//��������
		m_pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_AI, &RegisterPlaza, sizeof(RegisterPlaza));

		return true;
	}
	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Э������
	if (wServiceID == NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
			case MDM_CS_REGISTER:		//ע�����
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID, pData, wDataSize);
			}
			case MDM_CS_WEB_SERVICE:
			{
				return OnTCPSocketMainWebMessage(Command.wSubCmdID, pData, wDataSize);
			}
		}
	}
	return true;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnEventTCPNetworkBind AI����������յ���Ϣ wBindIndex = %d "), wBindIndex);
	OutputDebugString(szInfo);

	//���ñ���
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;


		wsprintf(szInfo, TEXT("ptdtai *** OnEventTCPNetworkBind AI����������յ���Ϣ wBindIndex = %d �󶨳ɹ�"), wBindIndex);
		OutputDebugString(szInfo);

		return true;
	}

	//�������
	////ASSERT(FALSE);

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	if(pBindParameter==NULL) return false;


	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnEventTCPNetworkShut ���������� dwSocketID  = %d "), dwSocketID);
	OutputDebugString(szInfo);

	m_AIManager.DeleteServer(dwSocketID);


	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnEventTCPNetworkRead ai�������յ���Ϣ wMainCmdID = %d wSubCmdID = %d "), Command.wMainCmdID, Command.wSubCmdID);
	OutputDebugString(szInfo);

	bool bReturnVal = false;
	switch (Command.wMainCmdID)
	{
	case MDM_SS_S_SERVICE:		//�û�����
		{
			bReturnVal = OnTCPNetworkService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			break;
		}
	case MDM_CS_S_AI_ACTION://AI ����
		{
			bReturnVal = OnTCPNetworkAIAction(Command.wSubCmdID, pData, wDataSize, dwSocketID);					
			break;
		}
	}

	//if (!bReturnVal)
	//{
	//	TCHAR szInfo[260] = { 0 };
	//	wsprintf(szInfo, TEXT("������� false   wMainCmdID = %d  wSubCmdID = %d "), Command.wMainCmdID, Command.wSubCmdID);
	//	CTraceService::TraceString(szInfo, TraceLevel_Normal);
	//}

	return true;
}


//��������
bool CAttemperEngineSink::SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//�û�����
	if (cbSendMask==BG_ALL_CLIENT)
	{
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,BG_MOBILE);
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,BG_COMPUTER);
	}
	else
	{
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,cbSendMask);
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	if (LOWORD(dwSocketID)!=INVALID_WORD)
	{
		//�����û�
		m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	return false;
}


//������AI����
bool CAttemperEngineSink::OnDBGroupAIParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	int nCount = wDataSize / sizeof(DBR_GR_GroupAIParameter);

	//�����Ϣ
	TCHAR szInfo[260] = {0};

	//��Ӳ���
	int nOffset = 0;
	for (int i = 0; i < nCount; i++)
	{
		DBR_GR_GroupAIParameter GroupAIParameter;
		memcpy_s(&GroupAIParameter, sizeof(GroupAIParameter), ((DBR_GR_GroupAIParameter*)pData + nOffset), sizeof(GroupAIParameter));
		tagGroupAIParameter  GroupAIParameterResult;
		GroupAIParameterResult.dwGroupID = GroupAIParameter.dwGroupID;
		GroupAIParameterResult.lDistirbuteTotalScore = GroupAIParameter.lDistirbuteTotalScore;
		GroupAIParameterResult.dwAICount = GroupAIParameter.dwAICount;
		GroupAIParameterResult.wKindID = GroupAIParameter.wKindID;
		GroupAIParameterResult.dwPlayingAIRateMin = GroupAIParameter.dwPlayingAIRateMin;
		GroupAIParameterResult.dwPlayingAIRateMax = GroupAIParameter.dwPlayingAIRateMax;
		GroupAIParameterResult.lDistirbuteSingleScore = GroupAIParameter.lDistirbuteSingleScore;
		GroupAIParameterResult.lDistirbuteScoreMinRatio = GroupAIParameter.lDistirbuteScoreMinRatio;
		GroupAIParameterResult.lDistirbuteScoreMaxRatio = GroupAIParameter.lDistirbuteScoreMaxRatio;
		m_AIManager.InsertGroupParameter(GroupAIParameterResult);

		wsprintf(szInfo, TEXT("ptdtaiwz *** dwAICount = %d dwGroupID = %d dwPlayingAIRateMax = %d dwPlayingAIRateMin = %d lDistirbuteScoreMaxRatio = %d"),
			GroupAIParameter.dwAICount, GroupAIParameter.dwGroupID, GroupAIParameter.dwPlayingAIRateMax, GroupAIParameter.dwPlayingAIRateMin, GroupAIParameter.lDistirbuteScoreMaxRatio);
		OutputDebugString(szInfo);

		wsprintf(szInfo, TEXT("ptdtaiwz *** lDistirbuteScoreMinRatio = %d lDistirbuteSingleScore = %d lDistirbuteTotalScore = %d wKindID = %d "),
			GroupAIParameter.lDistirbuteScoreMinRatio, GroupAIParameter.lDistirbuteSingleScore, GroupAIParameter.lDistirbuteTotalScore, GroupAIParameter.wKindID);
		OutputDebugString(szInfo);

		nOffset++;
	}

	return true;
}

//������AI����
bool CAttemperEngineSink::OnDBGroupAIParameterEnd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�������д�����AI
	m_AIManager.SendLoadAllAIAction();

	return true;
}


//����AI���
bool CAttemperEngineSink::OnDBLoadAIUser(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	int nCount = wDataSize / sizeof(DBR_GR_AIUserInfo);

	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAIUser ���ػ����� nCount = %d"), nCount);
	OutputDebugString(szInfo);

	//��Ӳ���
	int nOffset = 0;
	for (int i = 0; i < nCount; i++)
	{
		DBR_GR_AIUserInfo AIUserInfoResult;
		ZeroMemory(&AIUserInfoResult, sizeof(DBR_GR_AIUserInfo));
		memcpy_s(&AIUserInfoResult, sizeof(AIUserInfoResult), ((DBR_GR_AIUserInfo*)pData + nOffset), sizeof(AIUserInfoResult));
		tagAIUserInfo  AIUserInfo;
		ZeroMemory(&AIUserInfo, sizeof(AIUserInfo));
		AIUserInfo.dwUserID = AIUserInfoResult.dwUserID;
		AIUserInfo.lScore = AIUserInfoResult.lScore;
		AIUserInfo.cbIsLeave = 0;
		AIUserInfo.cbStatus = 0;
		time(&AIUserInfo.tEnterTime);
		time(&AIUserInfo.tTime);

		if (AIUserInfoResult.dwGroupID != 0 && AIUserInfoResult.wKindID != 0)
		{
			wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAIUser ���ػ����� nCount = %d  AIUserInfo.cbIsLeave = %d ���������"), nCount, AIUserInfo.cbIsLeave);
			OutputDebugString(szInfo);

			m_AIManager.InsertAIUser(AIUserInfoResult.dwGroupID, AIUserInfoResult.wKindID, &AIUserInfo);

			CMD_SS_G_Update_Group_Status UpdateGroupStatus;
			UpdateGroupStatus.cbStatus = US_GP_ONLINE;
			UpdateGroupStatus.dwGroupID = AIUserInfoResult.dwGroupID;
			UpdateGroupStatus.dwUserID = AIUserInfoResult.dwUserID;

			m_pITCPNetworkEngine->SendData(m_dwGroupSocketID, MDM_CS_S_AI_ACTION, SUB_SG_G_UPDATE_GROUP_STATUS, &UpdateGroupStatus, sizeof(UpdateGroupStatus));
		}

		nOffset += 1;
	}

	return true;
}

//����AI��ҽ���
bool CAttemperEngineSink::OnDBLoadAIUserEnd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	TCHAR szInfo[260] = { 0 };


	DBR_GR_LoadAIEnd * pLoadAIEnd = (DBR_GR_LoadAIEnd *)pData;

	CMD_SS_G_ReloadGroupMember ReloadGroupMember;
	ReloadGroupMember.dwGroupID = pLoadAIEnd->dwGroupID;

	for (int i = 0; i < m_arrayNewGroup.GetCount(); i++)
	{
		if (m_arrayNewGroup[i] == pLoadAIEnd->dwGroupID)
		{
			wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAIUserEnd ֪ͨ���ֲ� GroupID = %d ���¼��س�Ա m_dwGroupSocketID = %d"), pLoadAIEnd->dwGroupID, m_dwGroupSocketID);
			OutputDebugString(szInfo);
			m_pITCPNetworkEngine->SendData(m_dwGroupSocketID, MDM_CS_S_AI_ACTION, SUB_SG_G_RELOAD_GROUP_MEMBER, &ReloadGroupMember, sizeof(ReloadGroupMember));
			m_arrayNewGroup.RemoveAt(i);
		}
	}

	wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAIUserEnd ����AI������Ϣ�����ֲ� m_dwGroupSocketID = %d"), m_dwGroupSocketID);
	OutputDebugString(szInfo);
	m_AIManager.SendAIToGroupServer(0, 0, m_dwGroupSocketID);
	return true;
}

//����AI�����仯����
bool CAttemperEngineSink::OnDBLoadAITimeChange(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//ʱ��任
	if (sizeof(TimeChange) != wDataSize) return false;
	TimeChange * pTimeChange = (TimeChange *)pData;
	if (!pTimeChange)
	{
		return false;
	}

	TCHAR szInfo[260] = {0};

	for (int i = 0; i < 24; i++)
	{
		wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAITimeChange i = %d AICount = %d "), i, pTimeChange->dwTimeChange[i]);
		OutputDebugString(szInfo);
	}
	//����AI������ʱ��仯ֵ
	m_AIManager.SetAITimeChange(pTimeChange);
	return true;
}


//AI���뷿��Ȩ��
bool CAttemperEngineSink::OnDBLoadAITimeWeightIn(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�Ϸ��ж�
	if (sizeof(WeightInOption) != wDataSize) return false;
	WeightInOption * pWeightInOption = (WeightInOption *)pData;
	if (!pWeightInOption)
	{
		return false;
	}

	TCHAR szInfo[260] = { 0 };
	for (int i = 0; i < 10; i++)
	{
		wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAITimeWeightIn i = %d dwTime = %d dwInRatio = %d "), i, pWeightInOption->WeightInRatio[i].dwTime, pWeightInOption->WeightInRatio[i].dwInRatio);
		OutputDebugString(szInfo);
	}

	//����AI������ʱ��仯ֵ
	m_AIManager.SetWeightInOption(*pWeightInOption);
	return true;
}

//AI�뿪���ֲ�Ȩ��
bool CAttemperEngineSink::OnDBLoadAITimeWeightOut(DWORD dwContextID, VOID * pData, WORD wDataSize)
{	
	//�Ϸ��ж�
	if (sizeof(WeightOutOption) != wDataSize) return false;
	WeightOutOption * pWeightOutOption = (WeightOutOption *)pData;
	if (!pWeightOutOption)
	{
		return false;
	}

	TCHAR szInfo[260] = { 0 };
	for (int i = 0; i < 10; i++)
	{
		wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAITimeWeightOut i = %d dwTime = %d dwOutRatio = %d "), i, pWeightOutOption->WeightOutRatio[i].dwTime, pWeightOutOption->WeightOutRatio[i].dwOutRatio);
		OutputDebugString(szInfo);
	}

	//����AI������ʱ��仯ֵ
	m_AIManager.SetWeightOutOption(*pWeightOutOption);
	return true;
}



//�û�����
bool CAttemperEngineSink::OnTCPNetworkService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//AI����
	switch (wSubCmdID)
	{
	case SUB_SS_S_REGISTER_SERVER:		//��Ϸ������ע��
		{
			return OnTCPNetworkSubRegister(pData, wDataSize, dwSocketID);
		}
	case SUB_SS_S_AI_SERVER:		//���������ע��
		{
			OnTCPNetworkSubGroupRegister(pData, wDataSize, dwSocketID);
			return true;
		}
	case SUB_GA_CONFIG_APPEND://���ֲ�
		{
			return OnTCPNetworkSubGroupGameRule(pData, wDataSize, dwSocketID);
		}
	case SUB_SS_S_AI_UPDATE_BATTLE_RULE://���´������淨
		{
			return OnTCPNetworkSubUpdateGroupGameRule(pData, wDataSize, dwSocketID);
		}
	case SUB_SS_S_AI_UPDATE_USER_SCORE://���´�������ҷ���
		{
			return OnTCPNetworkSubUpdateGroupUserScore(pData, wDataSize, dwSocketID);
		}
		
	}

	return true;


}


//AI����
bool CAttemperEngineSink::OnTCPNetworkAIAction(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{


	//AI����
	switch (wSubCmdID)
	{
		case SUB_CS_S_AI_PARAMETER:						//����AI����
		{
			return OnTCPNetworkSubAIParameter(pData, wDataSize, dwSocketID);
		}
		case SUB_CS_S_PSRNL_ROOM_CREATE_SUCCESS:		//Լս���䴴���ɹ�
		{
			return OnTCPNetworkSubAIPrsnlRoomCreateSuccess(pData, wDataSize, dwSocketID);
		}
		case SUB_CS_C_LOGON_AI_RESULT:					//AI���뷿����
		{
			return OnTCPNetworkSubAILogonResult(pData, wDataSize, dwSocketID);
		}
		case SUB_CS_C_UPDATE_AI_STATUS:					//AI���뷿����
		{
			return OnTCPNetworkSubAIUpdateStatus(pData, wDataSize, dwSocketID);
		}
		case SUB_CS_C_AI_UPDATE_TABLE_COUNT:					//AI���뷿����
		{
			return OnTCPNetworkSubAITableChange(pData, wDataSize, dwSocketID);
		}
		case SUB_CS_C_AI_FAILURE:
		{
			return OnTCPNetworkSubAISitFailure(pData, wDataSize, dwSocketID);
		}
	}

	return true;
}


//����AI����
bool CAttemperEngineSink::OnTCPNetworkSubAIParameter(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize >= sizeof(tagGroupAIParameter));
	if (wDataSize<sizeof(tagGroupAIParameter)) return false;


	//������Ϣ
	tagGroupAIParameter * pGroupAIParamete = (tagGroupAIParameter *)pData;


	//��������
	DBR_GR_GroupAIParameter GroupAIParameter;
	ZeroMemory(&GroupAIParameter, sizeof(GroupAIParameter));
	GroupAIParameter.dwSocketID = 0;
	GroupAIParameter.cbStatus = 1;
	GroupAIParameter.lDistirbuteTotalScore = pGroupAIParamete->lDistirbuteTotalScore;
	GroupAIParameter.dwGroupID = pGroupAIParamete->dwGroupID;
	GroupAIParameter.wKindID = pGroupAIParamete->wKindID;
	GroupAIParameter.dwAICount = pGroupAIParamete->dwAICount;
	GroupAIParameter.dwPlayingAIRateMin = pGroupAIParamete->dwPlayingAIRateMin;
	GroupAIParameter.dwPlayingAIRateMax = pGroupAIParamete->dwPlayingAIRateMax;
	GroupAIParameter.lDistirbuteSingleScore = pGroupAIParamete->lDistirbuteSingleScore;
	GroupAIParameter.lDistirbuteScoreMinRatio = pGroupAIParamete->lDistirbuteScoreMinRatio;
	GroupAIParameter.lDistirbuteScoreMaxRatio = pGroupAIParamete->lDistirbuteScoreMaxRatio;

	m_arrayNewGroup.Add(pGroupAIParamete->dwGroupID);

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_AI_PARAMETER, DBR_GR_AI_PARAMETER, dwSocketID, &GroupAIParameter, sizeof(GroupAIParameter));

	
	return true;
}


//Լս���䴴���ɹ�
bool CAttemperEngineSink::OnTCPNetworkSubAIPrsnlRoomCreateSuccess(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize >= sizeof(CMD_SS_G_PersonalRoomCreateSuccess));
	if (wDataSize < sizeof(CMD_SS_G_PersonalRoomCreateSuccess)) return true;

	//������Ϣ
	CMD_SS_G_PersonalRoomCreateSuccess * pPersonalRoomCreateSuccess = (CMD_SS_G_PersonalRoomCreateSuccess *)pData;

	if (pPersonalRoomCreateSuccess->wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAIPrsnlRoomCreateSuccess AI  ��Ϊ57"));
		return true;
	}
	m_AIManager.SendAIToGameServer(pPersonalRoomCreateSuccess, dwSocketID);
	return true;
}

//AI ���뷿����
bool CAttemperEngineSink::OnTCPNetworkSubAILogonResult(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize >= sizeof(CMD_GR_LogonAI_Result));
	if (wDataSize < sizeof(CMD_GR_LogonAI_Result))
	{
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtai *** ������� false   wDataSize = %d  size = %d "), wDataSize, sizeof(CMD_GR_LogonAI_Result));
		OutputDebugString(szInfo);
		return true;
	}

	CMD_GR_LogonAI_Result * pLogonAIResult = (CMD_GR_LogonAI_Result *)pData;
	if (!pLogonAIResult)
	{
		return true;
	}

	//��ֵ
	tagAIStatusInfo AIStatusInfo;

	if (pLogonAIResult->cbCode == 0)//ʧ��
	{
		AIStatusInfo.cbStatus = 0;
	}
	else//�ɹ�
	{
		AIStatusInfo.cbStatus = 1;
	}

	AIStatusInfo.dwGroupID = pLogonAIResult->dwGroupID;
	AIStatusInfo.dwUserID = pLogonAIResult->dwUserID;
	AIStatusInfo.wKindID = pLogonAIResult->wKindID;
	AIStatusInfo.lScore = pLogonAIResult->lScore;

	//����AI״̬
	m_AIManager.UpdateAIStatusInfo(&AIStatusInfo);

	return true;

}


//����AI״̬
bool CAttemperEngineSink::OnTCPNetworkSubAIUpdateStatus(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize >= sizeof(tagAIStatusInfo));
	if (wDataSize < sizeof(tagAIStatusInfo)) return true;

	tagAIStatusInfo * pAIStatusInfo = (tagAIStatusInfo *)pData;
	if (!pAIStatusInfo)
	{
		return true;
	}

	if (pAIStatusInfo->wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAIUpdateStatus AI  ��Ϊ57"));
		return true;
	}

	//����AI״̬
	m_AIManager.UpdateAIStatusInfo(pAIStatusInfo);

	return true;
}

//����AI״̬
bool CAttemperEngineSink::OnTCPNetworkSubAITableChange(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	
	//Ч�����
	//ASSERT(wDataSize >= sizeof(CMD_SS_G_AI_Update_Table_Count));
	if (wDataSize < sizeof(CMD_SS_G_AI_Update_Table_Count)) return true;

	CMD_SS_G_AI_Update_Table_Count * pAIUpdateTableCount = (CMD_SS_G_AI_Update_Table_Count *)pData;

	if (!pAIUpdateTableCount)
	{
		return true;
	}

	if (pAIUpdateTableCount->wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAITableChange   ��Ϊ57"));
		return true;
	}
	OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAITableChange �յ����Ӹ���"));
	//����AI״̬
	m_AIManager.UpdateAITableChange(pAIUpdateTableCount);

	return true;

}

// ����AI״̬
bool CAttemperEngineSink::OnTCPNetworkSubAISitFailure(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{

	//Ч�����
	//ASSERT(wDataSize >= sizeof(CMD_SS_G_AI_Update_Table_Count));
	if (wDataSize < sizeof(tagAISitFailure)) return true;

	tagAISitFailure * pAISitFailure = (tagAISitFailure *)pData;

	if (!pAISitFailure)
	{
		return true;
	}

	if (pAISitFailure->wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAISitFailure   ��Ϊ57"));
		return true;
	}

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubAISitFailure �յ�����ʧ�ܴ��� lEnterLimiteScore = %d"), pAISitFailure->lEnterLimiteScore);
	OutputDebugString(szInfo);

	WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("OnTCPNetworkSubAISitFailure �յ�����ʧ�ܴ���"));

	//����AI״̬
	m_AIManager.SendOneAIToGameServer(pAISitFailure, dwSocketID);

	return true;

}


//������ע��
bool CAttemperEngineSink::OnTCPNetworkSubRegister(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	//ASSERT(wDataSize >= sizeof(CMD_CS_C_RegisterServer));
	if (wDataSize<sizeof(CMD_CS_C_RegisterServer)) return false;


	//������Ϣ
	CMD_CS_C_RegisterServer * pRegisterServer = (CMD_CS_C_RegisterServer *)pData;

	//���ð�
	WORD wBindIndex = LOWORD(dwSocketID);
	(m_pNormalParameter + wBindIndex)->wServiceID = pRegisterServer->wServerID;

	//��������
	tagGameServer GameServer;
	ZeroMemory(&GameServer, sizeof(GameServer));

	//��������
	GameServer.wKindID = pRegisterServer->wKindID;
	GameServer.wNodeID = pRegisterServer->wNodeID;
	GameServer.wSortID = pRegisterServer->wSortID;
	GameServer.wServerID = pRegisterServer->wServerID;
	GameServer.wServerKind = pRegisterServer->wServerKind;
	GameServer.wServerType = pRegisterServer->wServerType;
	GameServer.wServerLevel = pRegisterServer->wServerLevel;
	GameServer.wServerPort = pRegisterServer->wServerPort;
	GameServer.lCellScore = pRegisterServer->lCellScore;
	GameServer.cbEnterMember = pRegisterServer->cbEnterMember;
	GameServer.lEnterScore = pRegisterServer->lEnterScore;
	GameServer.dwServerRule = pRegisterServer->dwServerRule;
	GameServer.dwOnLineCount = pRegisterServer->dwOnLineCount;
	GameServer.dwFullCount = pRegisterServer->dwFullCount;
	GameServer.wTableCount = pRegisterServer->wTableCount;
	GameServer.dwSetPlayerCount = pRegisterServer->dwSetPlayerCount;

	lstrcpyn(GameServer.szServerName, pRegisterServer->szServerName, CountArray(GameServer.szServerName));
	lstrcpyn(GameServer.szServerAddr, pRegisterServer->szServerAddr, CountArray(GameServer.szServerAddr));
	lstrcpyn(GameServer.szGameInfomation, pRegisterServer->szGameInfomation, CountArray(GameServer.szGameInfomation));
	GameServer.dwSurportType = pRegisterServer->cbPlayMode;

	//ע�᷿��
	m_AIManager.InsertServerItem(dwSocketID, GameServer);

	return true;
}

//���ֲ�����
bool CAttemperEngineSink::OnTCPNetworkSubGroupGameRule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (wDataSize != sizeof(SUB_GA_ConfigAppend))
	{
		return true;
	}

	//��������
	SUB_GA_ConfigAppend * pConfigAppend = (SUB_GA_ConfigAppend *)pData;
	if (!pConfigAppend)
	{
		return true;
	}
	
	if (pConfigAppend->ConfigInfo.wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubGroupGameRule AI �յ��淨������Ϣ ��Ϊ57"));
		return true;
	}
	WORD wKindID = pConfigAppend->ConfigInfo.wKindID;
	byte cbPlayMode = pConfigAppend->ConfigInfo.cbPlayMode;

	//��������
	CMD_GR_AICreateTable CreateTable;

	CreateTable.lCellScore = pConfigAppend->ConfigInfo.lCellScore;							    //�׷�����
	CreateTable.dwDrawCountLimit = pConfigAppend->ConfigInfo.dwDrawCountLimit;					//��������
	CreateTable.dwDrawTimeLimit = pConfigAppend->ConfigInfo.dwDrawTimeLimit;					//ʱ������
	CreateTable.wJoinGamePeopleCount = pConfigAppend->ConfigInfo.wJoinGamePeopleCount;			//������Ϸ������
	CreateTable.dwRoomTax = pConfigAppend->ConfigInfo.dwRoomTax;								//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	CreateTable.cbPassword = pConfigAppend->ConfigInfo.cbPassword;								//�������� 0:������ 1:��Ҫ����
	CreateTable.cbPayMode = pConfigAppend->ConfigInfo.cbPayMode;								//֧����ʽ									
	memcpy_s(CreateTable.cbGameRule, RULE_LEN, pConfigAppend->ConfigInfo.cbGameRule, RULE_LEN);
	CreateTable.cbGameMode = pConfigAppend->ConfigInfo.cbGameMode;								//��Ϸģʽ
	CreateTable.lEnterScore = pConfigAppend->ConfigInfo.lEnterScore;							//���뷿��ķ�������
	CreateTable.lLeaveScore = pConfigAppend->ConfigInfo.lLeaveScore;							//�뿪�����������
	CreateTable.cbJoinMode = pConfigAppend->ConfigInfo.cbJoinMode;								//��Ҳ��뷽ʽ	0 ������İ���˲���	1 ����İ���˲���	
	CreateTable.dwGroupID = pConfigAppend->ConfigInfo.dwGroupID;								//���ֲ�id�� �Ǿ��ֲ�ģʽ �� 0
	CreateTable.dwConfigID = pConfigAppend->ConfigInfo.dwConfigID;								//�淨ID

	OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubGroupGameRule AI �յ��淨������Ϣ"));

	if (cbPlayMode)
	{
		//��������
		m_AIManager.CreateRoom(wKindID, cbPlayMode, CreateTable);
	}



	//���Դ���
	tagGroupAIParameter GroupAIParameter;

	GroupAIParameter.lDistirbuteTotalScore = 1000000;
	GroupAIParameter.dwGroupID = CreateTable.dwGroupID;
	GroupAIParameter.wKindID = 57;
	GroupAIParameter.dwAICount = 5;
	GroupAIParameter.dwPlayingAIRateMin = 60;
	GroupAIParameter.dwPlayingAIRateMax = 80;
	GroupAIParameter.lDistirbuteSingleScore = 2000;
	GroupAIParameter.lDistirbuteScoreMinRatio = 50;
	GroupAIParameter.lDistirbuteScoreMaxRatio = 90;

	//OnTCPNetworkSubAIParameter(&GroupAIParameter, sizeof(GroupAIParameter), 0);
	
	return true;
}


//���¾��ֲ��淨
bool CAttemperEngineSink::OnTCPNetworkSubUpdateGroupGameRule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule"));

	if (wDataSize != sizeof(CMD_SS_G_UpdateGroupGameRule))
	{
		return true;
	}

	//��������
	CMD_SS_G_UpdateGroupGameRule * pUpdateGroupGameRule = (CMD_SS_G_UpdateGroupGameRule *)pData;
	if (!pUpdateGroupGameRule)
	{
		return true;
	}

	if (pUpdateGroupGameRule->GroupConfigInfo.wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule   ��Ϊ57"));
		return true;
	}

	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule cbStatus = %d configid = %d"), pUpdateGroupGameRule->cbStatus, pUpdateGroupGameRule->GroupConfigInfo.dwConfigID);
	OutputDebugString(szInfo);
	if (pUpdateGroupGameRule->cbStatus == 0)
	{
		m_AIManager.DeleteGroupGameRule(pUpdateGroupGameRule->GroupConfigInfo);
	}
	else if (pUpdateGroupGameRule->cbStatus == 1)
	{
		m_AIManager.InsertGroupGameRule(pUpdateGroupGameRule->GroupConfigInfo);
	}
	else if (pUpdateGroupGameRule->cbStatus == 2)
	{
		m_AIManager.ModifyGroupGameRule(pUpdateGroupGameRule->GroupConfigInfo);
	}

	return true;
}


//���¾��ֲ���ҷ���
bool CAttemperEngineSink::OnTCPNetworkSubUpdateGroupUserScore(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupUserScore"));

	WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupUserScore"));

	if (wDataSize != sizeof(CMD_SS_S_UpdateClubUserScore))
	{
		return true;
	}

	//��������
	CMD_SS_S_UpdateClubUserScore * pUpdateGroupUserScore = (CMD_SS_S_UpdateClubUserScore *)pData;
	if (!pUpdateGroupUserScore)
	{
		return true;
	}
	tagAIStatusInfo AIStatusInfo;
	AIStatusInfo.dwGroupID = pUpdateGroupUserScore->dwGroupID;
	AIStatusInfo.wKindID = pUpdateGroupUserScore->wKindID;
	AIStatusInfo.lScore = pUpdateGroupUserScore->lScore;
	AIStatusInfo.dwUserID = pUpdateGroupUserScore->dwUserID;
	m_AIManager.UpdateAIScore(&AIStatusInfo);
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupUserScore groupid = %d kindid = %d serverid = %d userid = %d"), 
		pUpdateGroupUserScore->dwGroupID, pUpdateGroupUserScore->wKindID, pUpdateGroupUserScore->dwServerID, pUpdateGroupUserScore->dwUserID);
	OutputDebugString(szInfo);


	return true;
}


//���ֲ�����
bool CAttemperEngineSink::OnTCPNetworkSubGroupRegister(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	m_dwGroupSocketID = dwSocketID;
	m_pITimerEngine->SetTimer(IDI_SEND_GROUP_ONLINE, TIME_AI_SEND_GROUP_ONLINE, 1, 0);
	//m_AIManager.SendAIToGroupServer(0, 0, dwSocketID);

	return true;
}



//�󶨲���
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//��Ч����
	if (wBindIndex == INVALID_WORD) return NULL;

	//��������
	if (wBindIndex < m_pInitParameter->m_wMaxConnect)
	{
		return m_pNormalParameter + wBindIndex;
	}


	return NULL;
}



//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//�Ͽ�Э��
bool CAttemperEngineSink::DisconnectCorrespond()
{
	//���ñ���
	m_bNeekCorrespond=false;

	//�ر�����
	if (m_pITCPSocketService!=NULL) m_pITCPSocketService->CloseSocket();

	//�ر�Լս��������
	if (m_pPrsnlRmITCPSocketService!=NULL) m_pPrsnlRmITCPSocketService->CloseSocket();

	return true;
}

//�������
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{


	return true;
}

//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
		{
			CTraceService::TraceString(TEXT("Э��������ע��ɹ�"), TraceLevel_Normal);
			return true;
		}
	}

	return true;
}

//��վ�¼�
bool CAttemperEngineSink::OnTCPSocketMainWebMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
		case SUB_CS_C_UPDATE_AI_PARAMTER:
			{
				OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_PARAMTER"));
				if (wDataSize != sizeof(CMD_CS_C_UpdateAIParameterToAI))
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_PARAMTER ���ݴ�С����ȷ"));
				}

				CMD_CS_C_UpdateAIParameterToAI * pUpdateAIParameter = (CMD_CS_C_UpdateAIParameterToAI *)pData;

				if (!pUpdateAIParameter)
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_PARAMTER ����Ϊ��"));
				}

				tagGroupAIParameter GroupAIParameter;
				GroupAIParameter.dwAICount = pUpdateAIParameter->dwAICount;
				GroupAIParameter.dwGroupID = pUpdateAIParameter->dwGroupID;
				GroupAIParameter.dwPlayingAIRateMax = pUpdateAIParameter->dwPlayingAIRateMax;
				GroupAIParameter.dwPlayingAIRateMin = pUpdateAIParameter->dwPlayingAIRateMin;
				GroupAIParameter.lDistirbuteScoreMaxRatio = pUpdateAIParameter->lDistirbuteScoreMaxRatio;
				GroupAIParameter.lDistirbuteScoreMinRatio = pUpdateAIParameter->lDistirbuteScoreMinRatio;
				GroupAIParameter.lDistirbuteSingleScore = pUpdateAIParameter->lDistirbuteSingleScore;
				GroupAIParameter.lDistirbuteTotalScore = pUpdateAIParameter->lDistirbuteTotalScore;
				GroupAIParameter.wKindID = pUpdateAIParameter->wKindID;


				TCHAR szInfo[260] = {0};
				wsprintf(szInfo, TEXT("ptdtaiwz *** cbStatus = %d dwGroupID = %d lDistirbuteTotalScore = %I64d dwAICount = %d wKindID = %d  dwSocketID = %d"), pUpdateAIParameter->dwSocketID,
					pUpdateAIParameter->cbStatus, pUpdateAIParameter->dwGroupID, pUpdateAIParameter->lDistirbuteTotalScore, pUpdateAIParameter->dwAICount, pUpdateAIParameter->wKindID);
				OutputDebugString(szInfo);

				wsprintf(szInfo, TEXT("ptdtaiwz *** dwPlayingAIRateMin = %d dwPlayingAIRateMax = %d lDistirbuteSingleScore = %I64d lDistirbuteScoreMinRatio = %I64d lDistirbuteScoreMaxRatio = %I64d"), 
					pUpdateAIParameter->dwPlayingAIRateMin, pUpdateAIParameter->dwPlayingAIRateMax, pUpdateAIParameter->lDistirbuteSingleScore, pUpdateAIParameter->lDistirbuteScoreMinRatio, pUpdateAIParameter->lDistirbuteScoreMaxRatio);
				OutputDebugString(szInfo);

				//��������
				DBR_GR_GroupAIParameter dbGroupAIParameter;
				ZeroMemory(&GroupAIParameter, sizeof(GroupAIParameter));
				dbGroupAIParameter.dwSocketID = pUpdateAIParameter->dwSocketID;
				dbGroupAIParameter.cbStatus = pUpdateAIParameter->cbStatus;
				dbGroupAIParameter.lDistirbuteTotalScore = pUpdateAIParameter->lDistirbuteTotalScore;
				dbGroupAIParameter.dwGroupID = pUpdateAIParameter->dwGroupID;
				dbGroupAIParameter.wKindID = pUpdateAIParameter->wKindID;
				dbGroupAIParameter.dwAICount = pUpdateAIParameter->dwAICount;
				dbGroupAIParameter.dwPlayingAIRateMin = pUpdateAIParameter->dwPlayingAIRateMin;
				dbGroupAIParameter.dwPlayingAIRateMax = pUpdateAIParameter->dwPlayingAIRateMax;
				dbGroupAIParameter.lDistirbuteSingleScore = pUpdateAIParameter->lDistirbuteSingleScore;
				dbGroupAIParameter.lDistirbuteScoreMinRatio = pUpdateAIParameter->lDistirbuteScoreMinRatio;
				dbGroupAIParameter.lDistirbuteScoreMaxRatio = pUpdateAIParameter->lDistirbuteScoreMaxRatio;

				m_arrayNewGroup.Add(pUpdateAIParameter->dwGroupID);

				//Ͷ������
				m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_AI_PARAMETER, DBR_GR_AI_PARAMETER, 0, &dbGroupAIParameter, sizeof(dbGroupAIParameter));

				//m_AIManager.InsertGroupParameter(GroupAIParameter);

				return true;
			}
		case SUB_CS_C_UPDATE_AI_WEIGHT_OUT:
			{
				OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_OUT"));

				if (wDataSize != sizeof(CMD_CS_C_UpdateAIWeightOutToAI))
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_OUT ���ݴ�С����ȷ"));
				}

				CMD_CS_C_UpdateAIWeightOutToAI * pUpdateAIWeightOut = (CMD_CS_C_UpdateAIWeightOutToAI *)pData;

				if (!pUpdateAIWeightOut)
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_OUT ����Ϊ��"));
				}

				WeightOutOption wghtOutOption;

				TCHAR szInfo[260] = { 0 };

				wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwOutRatio = %d dwTime = %d"), pUpdateAIWeightOut->UpdateAIWeightOut.nIndex, pUpdateAIWeightOut->UpdateAIWeightOut.WeightOutRatio.dwOutRatio, pUpdateAIWeightOut->UpdateAIWeightOut.WeightOutRatio.dwTime);
				OutputDebugString(szInfo);
			

				m_AIManager.SetWeightOutOption(pUpdateAIWeightOut->UpdateAIWeightOut.nIndex, pUpdateAIWeightOut->UpdateAIWeightOut.WeightOutRatio);

				DBR_GR_AIOperateResult AIOperateResult;
				AIOperateResult.dwSocketID = pUpdateAIWeightOut->dwSocketID;
				AIOperateResult.cbResult = 0;

				OnDBAIOperateResult(AIOperateResult.dwSocketID, &AIOperateResult, sizeof(AIOperateResult));


				return true;
			}
		case SUB_CS_C_UPDATE_AI_WEIGHT_IN:
			{
				OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_IN"));

				if (wDataSize != sizeof(CMD_CS_C_UpdateAIWeightInToAI))
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_IN ���ݴ�С����ȷ"));
				}

				CMD_CS_C_UpdateAIWeightInToAI * pUpdateAIWeightIn = (CMD_CS_C_UpdateAIWeightInToAI *)pData;

				if (!pUpdateAIWeightIn)
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_IN ����Ϊ��"));
				}

				WeightInOption wghtInRatio;

				TCHAR szInfo[260] = { 0 };
		
				wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwOutRatio = %d dwTime = %d"), pUpdateAIWeightIn->UpdateAIWeightIn.nIndex, pUpdateAIWeightIn->UpdateAIWeightIn.WeightInRatio.dwInRatio, pUpdateAIWeightIn->UpdateAIWeightIn.WeightInRatio.dwTime);
					OutputDebugString(szInfo);
			

				m_AIManager.SetWeightInOption(pUpdateAIWeightIn->UpdateAIWeightIn.nIndex, pUpdateAIWeightIn->UpdateAIWeightIn.WeightInRatio);

				DBR_GR_AIOperateResult AIOperateResult;
				AIOperateResult.dwSocketID = pUpdateAIWeightIn->dwSocketID;
				AIOperateResult.cbResult = 0;

				OnDBAIOperateResult(AIOperateResult.dwSocketID, &AIOperateResult, sizeof(AIOperateResult));
												
				return true;
			}
		case SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL:
			{
				OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL"));

				if (wDataSize != sizeof(CMD_CS_C_UpdateAITimeTotalToAI))
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL ���ݴ�С����ȷ"));
				}

				CMD_CS_C_UpdateAITimeTotalToAI * pUpdateAITimeTotal = (CMD_CS_C_UpdateAITimeTotalToAI *)pData;

				if (!pUpdateAITimeTotal)
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL ����Ϊ��"));
				}

				TimeChange tmChange;

				TCHAR szInfo[260] = { 0 };


				m_AIManager.SetAITimeChange(pUpdateAITimeTotal->UpdateAITimeTotal.nIndex, pUpdateAITimeTotal->UpdateAITimeTotal.dwTimeChange);

	
				wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwTime = %d dwTimeChange = %d"), pUpdateAITimeTotal->UpdateAITimeTotal.nIndex, pUpdateAITimeTotal->UpdateAITimeTotal.dwTime, pUpdateAITimeTotal->UpdateAITimeTotal.dwTimeChange);
					OutputDebugString(szInfo);
			

				DBR_GR_AIOperateResult AIOperateResult;
				AIOperateResult.dwSocketID = pUpdateAITimeTotal->dwSocketID;
				AIOperateResult.cbResult = 0;

				OnDBAIOperateResult(AIOperateResult.dwSocketID, &AIOperateResult, sizeof(AIOperateResult));
													
				return true;
			}
		case SUB_CS_C_DELETE_AI:
			{
				if (wDataSize != sizeof(CMD_CS_C_DeleteAIParameterToAI))
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_DELETE_AI ���ݴ�С����ȷ"));
				}

				CMD_CS_C_DeleteAIParameterToAI * pDeleteAIParameterToAI = (CMD_CS_C_DeleteAIParameterToAI *)pData;

				if (!pDeleteAIParameterToAI)
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI������ �յ���վ����Ϣ SUB_CS_C_DELETE_AI ����Ϊ��"));
				}

				TCHAR szInfo[260] = { 0 };

				for (int i = 0; i < 20; i++)
				{
					wsprintf(szInfo, TEXT("ptdtaiwz *** GroupID = %d operateID = %d kindid = %d userid = %d isAI = %d"), pDeleteAIParameterToAI->DeleteAI.dwGroupID, pDeleteAIParameterToAI->DeleteAI.cbOperate,
						pDeleteAIParameterToAI->DeleteAI.DeleteAI[i].wKindID, pDeleteAIParameterToAI->DeleteAI.DeleteAI[i].dwUserID, pDeleteAIParameterToAI->DeleteAI.DeleteAI[i].cbIsAI);
					OutputDebugString(szInfo);
				}



				m_AIManager.SetUserLeave(pDeleteAIParameterToAI->DeleteAI);

				DBR_GR_AIOperateResult AIOperateResult;
				AIOperateResult.dwSocketID = pDeleteAIParameterToAI->dwSocketID;
				AIOperateResult.cbResult = 0;

				OnDBAIOperateResult(AIOperateResult.dwSocketID, &AIOperateResult, sizeof(AIOperateResult));
								  
				return true;
			}
	}

	return true;
}

//������־
void CAttemperEngineSink::SendLogData(const TCHAR * pszFormat, ...)
{
	TCHAR buf[1024];
	ZeroMemory(buf, 1024 * sizeof(TCHAR));

	va_list arglist;
	va_start(arglist, pszFormat);
	vswprintf_s(buf, 1024 - 1, pszFormat, arglist);
	va_end(arglist);

	tagLogUserInfo LogUserInfo;
	ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	wsprintf(LogUserInfo.szLogContent, TEXT("%s"), buf);


	CString strDefaultDescName = TEXT("ͨ����־");
	wsprintf(LogUserInfo.szDescName, TEXT("%s"), strDefaultDescName);



	LogUserInfo.cbServerSign = 3;
	GetLocalTime(&LogUserInfo.sysTime);
	m_pLogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &LogUserInfo, sizeof(LogUserInfo));
}


//������־
void CAttemperEngineSink::SendLogData(IServerUserItem * pIServerUserItem, const TCHAR * pszFormat, ...)
{
	TCHAR buf[1024];
	ZeroMemory(buf, 1024 * sizeof(TCHAR));

	va_list arglist;
	va_start(arglist, pszFormat);
	vswprintf_s(buf, 1024 - 1, pszFormat, arglist);
	va_end(arglist);

	tagLogUserInfo LogUserInfo;
	ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	wsprintf(LogUserInfo.szLogContent, TEXT("%s"), buf);


	CString strDefaultDescName = TEXT("ͨ����־");
	wsprintf(LogUserInfo.szDescName, TEXT("%s"), strDefaultDescName);


	LogUserInfo.cbServerSign = 3;
	GetLocalTime(&LogUserInfo.sysTime);

	if (pIServerUserItem != NULL)
	{
		LogUserInfo.dwUserID = pIServerUserItem->GetUserID();
		LogUserInfo.wTableID = pIServerUserItem->GetTableID();
		LogUserInfo.wChairID = pIServerUserItem->GetChairID();
		CopyMemory(LogUserInfo.szNickName, pIServerUserItem->GetNickName(), sizeof(LogUserInfo.szNickName));
	}

	m_pLogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &LogUserInfo, sizeof(LogUserInfo));
}


//������־����
void CAttemperEngineSink::SendLogData(tagLogUserInfo LogUserInfo)
{
	tagLogUserInfo sendLogUserInfo;
	ZeroMemory(&sendLogUserInfo, sizeof(sendLogUserInfo));

	//����ĩβ����
	LogUserInfo.szLogContent[CountArray(LogUserInfo.szLogContent) - 1] = 0;
	LogUserInfo.szDescName[CountArray(LogUserInfo.szDescName) - 1] = 0;
	LogUserInfo.szGameName[CountArray(LogUserInfo.szGameName) - 1] = 0;
	LogUserInfo.szNickName[CountArray(LogUserInfo.szNickName) - 1] = 0;
	LogUserInfo.szServerName[CountArray(LogUserInfo.szServerName) - 1] = 0;

	//��ֵ
	wsprintf(sendLogUserInfo.szLogContent, TEXT("%s"), LogUserInfo.szLogContent);
	
	if (wcslen(LogUserInfo.szDescName) <= 1)
	{
		CString strDefaultDescName = TEXT("ͨ����־");
		wsprintf(LogUserInfo.szDescName, TEXT("%s"), strDefaultDescName);
	}
	else
	{
		CopyMemory(sendLogUserInfo.szDescName, LogUserInfo.szDescName, sizeof(LogUserInfo.szDescName));
	}


	sendLogUserInfo.cbServerSign = 3;
	GetLocalTime(&sendLogUserInfo.sysTime);
	
	sendLogUserInfo.dwUserID = LogUserInfo.dwUserID;
	sendLogUserInfo.wTableID = LogUserInfo.wTableID;
	sendLogUserInfo.wChairID = LogUserInfo.wChairID;
	CopyMemory(sendLogUserInfo.szNickName, LogUserInfo.szNickName, sizeof(LogUserInfo.szNickName));

	m_pLogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &sendLogUserInfo, sizeof(sendLogUserInfo));
}

//������־����
void CAttemperEngineSink::SendLogData(IServerUserItem * pIServerUserItem, const TCHAR szDescName[LEN_SERVER], const TCHAR * pszFormat)
{
	tagLogUserInfo sendLogUserInfo;
	ZeroMemory(&sendLogUserInfo, sizeof(sendLogUserInfo));

	wsprintf(sendLogUserInfo.szLogContent, TEXT("%s"), pszFormat);
	if (szDescName == NULL || wcslen(szDescName) <= 1)
	{
		CString strDefaultDescName = TEXT("ͨ����־");
		wsprintf(sendLogUserInfo.szDescName, TEXT("%s"), strDefaultDescName);
	}
	else
	{
		CopyMemory(sendLogUserInfo.szDescName, szDescName, sizeof(sendLogUserInfo.szDescName));
	}


	sendLogUserInfo.cbServerSign = 3;
	GetLocalTime(&sendLogUserInfo.sysTime);
	
	if (pIServerUserItem != NULL)
	{
		sendLogUserInfo.dwUserID = pIServerUserItem->GetUserID();
		sendLogUserInfo.wTableID = pIServerUserItem->GetTableID();
		sendLogUserInfo.wChairID = pIServerUserItem->GetChairID();
		CopyMemory(sendLogUserInfo.szNickName, pIServerUserItem->GetNickName(), sizeof(sendLogUserInfo.szNickName));
	}

	m_pLogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &sendLogUserInfo, sizeof(sendLogUserInfo));
}


//AI�������
bool CAttemperEngineSink::OnDBAIOperateResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//��������
	DBR_GR_AIOperateResult * pOperateResult = (DBR_GR_AIOperateResult *)pData;

	CMD_WS_S_AIOperateResult AIOperateResult;
	AIOperateResult.dwSocketID = pOperateResult->dwSocketID;
	AIOperateResult.wOperateID = pOperateResult->wOperateID;
	AIOperateResult.cbResult = pOperateResult->cbResult;
	lstrcpyn(AIOperateResult.szDescripe, pOperateResult->szDescripe, CountArray(AIOperateResult.szDescripe));

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtaiwz *** OnAIOperateResult AI��������Э�������� ���ز������ wOperateCode = %d cbResult = %d dwSocketID = %d"), AIOperateResult.wOperateID, AIOperateResult.cbResult, pOperateResult->dwSocketID);
	OutputDebugString(szInfo);

	m_pITCPSocketService->SendData(MDM_CS_WEB_SERVICE, SUB_SW_S_AI_OPERATE_RESULT, &AIOperateResult, sizeof(AIOperateResult));

	return true;
}



//AI�������
bool CAttemperEngineSink::OnDBClearAIResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{



	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBClearAIResult AI����ɹ�"));
	OutputDebugString(szInfo);
	//�¼�֪ͨ
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess = ER_SUCCESS;
	SendUIControlPacket(UI_CLEAR_RESULT, &ControlResult, sizeof(ControlResult));
	return true;
}




//////////////////////////////////////////////////////////////////////////////////
