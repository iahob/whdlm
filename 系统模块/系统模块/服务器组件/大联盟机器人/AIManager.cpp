#include "StdAfx.h"
#include "AIManager.h"
#include "DataBasePacket.h"


//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAIManager::CAIManager()
{
	m_mapGroupAIParameter.InitHashTable(PRIME_SERVER);
	m_mapGroupAIUser.InitHashTable(PRIME_PLATFORM_USER);
	WHLogControl::SetLogStatus(1);
}

//��������
CAIManager::~CAIManager()
{
	m_mapGroupAIParameter.RemoveAll();
}

//������ֲ�����
byte CAIManager::InsertGroupParameter(tagGroupAIParameter GroupAIParameter)
{
	//�������
	CKindAIParameter* pKindAIParameter = NULL;

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai **** ***************************************************GroupID = %d  wKindID = %d"), GroupAIParameter.dwGroupID, GroupAIParameter.wKindID);
	OutputDebugString(szInfo);

	//�������
	if (m_mapGroupAIParameter.Lookup(GroupAIParameter.dwGroupID, pKindAIParameter) == FALSE)
	{
		//�����ڴ�
		pKindAIParameter = new CKindAIParameter();
		pKindAIParameter->InitHashTable(PRIME_SERVER);

		//���ֲ�������Ϸ���Ͳ����ļ���
		m_mapGroupAIParameter[GroupAIParameter.dwGroupID] = pKindAIParameter;

		//���ֲ�������Ϸ�Ĳ���
		tagGroupAIParameter * pGroupAIParameter = new tagGroupAIParameter();
		memcpy_s(pGroupAIParameter, sizeof(tagGroupAIParameter), &GroupAIParameter, sizeof(tagGroupAIParameter));
		(*pKindAIParameter)[GroupAIParameter.wKindID] =  pGroupAIParameter;
	}
	else
	{
		//���ֲ�������Ϸ�Ĳ���
		tagGroupAIParameter * pGroupAIParameter = new tagGroupAIParameter();
		memcpy_s(pGroupAIParameter, sizeof(tagGroupAIParameter), &GroupAIParameter, sizeof(tagGroupAIParameter));
		(*pKindAIParameter)[GroupAIParameter.wKindID] = pGroupAIParameter;
		return false;
	}

	return true;
}

//��ȡAI����
tagGroupAIParameter * CAIManager::GetGroupParameter(DWORD dwGroupID, WORD wKindID)
{
	CKindAIParameter* pKindAIParameter = NULL;
	if (m_mapGroupAIParameter.Lookup(dwGroupID, pKindAIParameter) == FALSE)
	{
		return NULL;
	}

	tagGroupAIParameter * pGroupAIParameter = NULL;
	if ((*pKindAIParameter).Lookup(wKindID, pGroupAIParameter) == FALSE)
	{
		return NULL;
	}

	return pGroupAIParameter;
}


//�������ݿ�����
void CAIManager::SetDBCorrespondManager(IDBCorrespondManager *	pIDBCorrespondManager, ITCPNetworkEngine *	pITCPNetworkEngine)
{
	m_pIDBCorrespondManager = pIDBCorrespondManager;
	m_pITCPNetworkEngine = pITCPNetworkEngine;
}

//���ͼ���AI
void CAIManager::SendLoadOneKindAIAction(DWORD dwGroupID, WORD wKindID)
{
	//�������
	CKindAIParameter* pKindAIParameter = NULL;

	//�жϾ��ֲ��Ƿ����
	if (m_mapGroupAIParameter.Lookup(dwGroupID, pKindAIParameter) == FALSE)
	{
		return;
	}

	//�ж���ϷAI�Ƿ����
	tagGroupAIParameter * pGroupAIParameter = NULL;
	if ((*pKindAIParameter).Lookup(wKindID, pGroupAIParameter) == FALSE)
	{
		return;
	}

	//��ȡ��ǰ��ϷAI����
	tagGroupAIParameter GroupAIParameter;

	memcpy_s(&GroupAIParameter, sizeof(tagGroupAIParameter), (*pKindAIParameter)[wKindID], sizeof(tagGroupAIParameter));

	//���ͼ��ػ�������
	m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_LOAD_AI_USER, DBR_GR_LOAD_AI_USER, 0, &GroupAIParameter, sizeof(GroupAIParameter));


}

//���ͼ�������AI
void CAIManager::SendLoadAllAIAction()
{
	//�������о��ֲ�AI����
	DWORD dwAIID = 0;
	CKindAIParameter * pKindAIParameter = NULL;
	POSITION Position = m_mapGroupAIParameter.GetStartPosition();

	//�������о��ֲ�
	while (Position != NULL)
	{
		m_mapGroupAIParameter.GetNextAssoc(Position, dwAIID, pKindAIParameter);
		if (pKindAIParameter != NULL)
		{
			//�������ֲ���������Ϸ
			WORD wAIKindID = 0;
			
			tagGroupAIParameter * pGroupAIParameter = NULL;
			POSITION PositionKind = pKindAIParameter->GetStartPosition();

			while (PositionKind != NULL)
			{
				pKindAIParameter->GetNextAssoc(PositionKind, wAIKindID, pGroupAIParameter);

				//��ȡ��ǰ��ϷAI����
				tagGroupAIParameter GroupAIParameter;

				ZeroMemory(&GroupAIParameter, sizeof(GroupAIParameter));

				memcpy_s(&GroupAIParameter, sizeof(tagGroupAIParameter), pGroupAIParameter, sizeof(tagGroupAIParameter));

				//���ͼ��ػ�������
				m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_LOAD_AI_USER, DBR_GR_LOAD_AI_USER, 0, &GroupAIParameter, sizeof(GroupAIParameter));
			}
		}
	}
}

//���ͼ�������AI
void CAIManager::InsertAIUser(DWORD dwGroupID, WORD wKindID, tagAIUserInfo * pAIUserInfo)
{
	TCHAR szInfo[260] = {0};
	//������ʼ��
	CGroupKindAI* pGroupKindAI = NULL;

	//��Ӿ��ֲ���ĳ���͵����
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		//�����ڴ�
		pGroupKindAI = new CGroupKindAI();
		pGroupKindAI->InitHashTable(PRIME_PLATFORM_USER);

		//���ֲ�������Ϸ���Ͳ����ļ���
		m_mapGroupAIUser[dwGroupID] = pGroupKindAI;

		CKindAIUser * pKindAIUser = new CKindAIUser();
		(*pGroupKindAI)[wKindID] = pKindAIUser;


		//���ֲ�������Ϸ��AI���
		tagAIUserInfo * pAIUserInfoNew = new tagAIUserInfo();

		memcpy_s(pAIUserInfoNew, sizeof(tagAIUserInfo), pAIUserInfo, sizeof(tagAIUserInfo));
		(*pKindAIUser)[pAIUserInfo->dwUserID] = pAIUserInfoNew;

		wsprintf(szInfo, TEXT("ptdtai **** InsertAIUser dwGroupID = %d wKindID = %d userid = %d cbStatus = %d cbIsLeave = %d"), dwGroupID, wKindID, pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbIsLeave);
		
		OutputDebugString(szInfo);
	}
	else
	{
		CKindAIUser * pKindAIUser = NULL;

		//���ĳ���͵����
		if (pGroupKindAI->Lookup(wKindID, pKindAIUser) == FALSE)
		{
			pKindAIUser = new CKindAIUser();
			(*pGroupKindAI)[wKindID] = pKindAIUser;


			//���ֲ�������Ϸ��AI���
			tagAIUserInfo * pAIUserInfoNew = new tagAIUserInfo();

			memcpy_s(pAIUserInfoNew, sizeof(tagAIUserInfo), pAIUserInfo, sizeof(tagAIUserInfo));
			(*pKindAIUser)[pAIUserInfo->dwUserID] = pAIUserInfoNew;

			wsprintf(szInfo, TEXT("ptdtai **** InsertAIUser dwGroupID = %d wKindID = %d userid = %d cbStatus = %d cbIsLeave = %d"), dwGroupID, wKindID, pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbIsLeave);
			OutputDebugString(szInfo);
		}
		else
		{
			//���ֲ�������Ϸ��AI���
			tagAIUserInfo * pAIUserInfoNew = new tagAIUserInfo();
			memcpy_s(pAIUserInfoNew, sizeof(tagAIUserInfo), pAIUserInfo, sizeof(tagAIUserInfo));
			(*pKindAIUser)[pAIUserInfo->dwUserID] = pAIUserInfoNew;

			wsprintf(szInfo, TEXT("ptdtai **** InsertAIUser dwGroupID = %d wKindID = %d userid = %d cbStatus = %d cbIsLeave = %d"), dwGroupID, wKindID, pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbIsLeave);
			OutputDebugString(szInfo);
		}

	}
}

//����Ϸ������AI ʹ���¼
void CAIManager::SendAIToGroupServer(DWORD dwGroupID, WORD wKindID, DWORD dwSocketID)
{
	//�������о��ֲ�AI����
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	POSITION Position = m_mapGroupAIUser.GetStartPosition();

	TCHAR szInfo[260] = { 0 };
	if (Position == NULL)
	{
		wsprintf(szInfo, TEXT("ptdtai *** SendAIToGroupServer ���ֲ�������"));
		OutputDebugString(szInfo);
	}
	

	//�������о��ֲ�
	while (Position != NULL)
	{
		m_mapGroupAIUser.GetNextAssoc(Position, dwAIID, pGroupKindAI);
		if (pGroupKindAI != NULL)
		{
			//�������ֲ���������Ϸ
			WORD wAIKindID = 0;
			CKindAIUser * pKindAIUser = NULL;
			POSITION PositionKind = pGroupKindAI->GetStartPosition();

			while (PositionKind != NULL)
			{
				pGroupKindAI->GetNextAssoc(PositionKind, wAIKindID, pKindAIUser);
				if (pKindAIUser != NULL)
				{

					//�������ֲ���ĳ����Ϸ�����AI
					DWORD wAIUser = 0;
					tagAIUserInfo * pAIUserInfo = NULL;
					POSITION PositionAIUser = pGroupKindAI->GetStartPosition();

					while (PositionAIUser != NULL)
					{
						pKindAIUser->GetNextAssoc(PositionAIUser, wAIUser, pAIUserInfo);

						if (pAIUserInfo)
						{
							CMD_SS_G_Update_Group_Status UpdateGroupStatus;
							UpdateGroupStatus.cbStatus = US_GP_ONLINE;
							UpdateGroupStatus.dwGroupID = dwAIID;
							UpdateGroupStatus.dwUserID = pAIUserInfo->dwUserID;

							wsprintf(szInfo, TEXT("ptdtai *** SendAIToGroupServer dwUserID = %d"), pAIUserInfo->dwUserID);
							OutputDebugString(szInfo);

							m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_S_AI_ACTION, SUB_SG_G_UPDATE_GROUP_STATUS, &UpdateGroupStatus, sizeof(UpdateGroupStatus));
						}
						else
						{
							wsprintf(szInfo, TEXT("ptdtai *** SendAIToGroupServer ��Ҳ�����"));
							OutputDebugString(szInfo);
						}
						
					}
				}
				else
				{
					wsprintf(szInfo, TEXT("ptdtai *** SendAIToGroupServer ��Ϸ���Ͳ�����"));
					OutputDebugString(szInfo);
				}
		

			}
		}
	}
}

//���뷿��
bool CAIManager::InsertServerItem(DWORD dwSocketID,  tagGameServer & GameServer)
{
	//Ѱ�ҷ���
	tagServerItem * pServerItem = NULL;
	if (m_mapServerID.Lookup(GameServer.wServerID, pServerItem) == TRUE)
	{
		////ASSERT(FALSE);
		return false;
	}

	//��������
	pServerItem = new tagServerItem();
	if (pServerItem == NULL)
	{
		////ASSERT(FALSE);
		return false;
	}

	//���÷���
	pServerItem->m_dwSocketID = dwSocketID;
	pServerItem->m_wUserTable = 0;
	pServerItem->m_GameServer = GameServer;

	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtai **** InsertServerItem dwSocketID = %d"), dwSocketID);
	OutputDebugString(szInfo);

	//��������
	m_mapServerID[GameServer.wServerID] = pServerItem;

	return true;
}



//��AI����
void CAIManager::SendAIToGameServer(CMD_SS_G_PersonalRoomCreateSuccess * pPersonalRoomCreateSuccess, DWORD dwSocketID)
{
	DWORD	dwGroupID = pPersonalRoomCreateSuccess->dwGroupID;					//���ֲ�ID
	WORD	wKindID = pPersonalRoomCreateSuccess->wKindID;						//��Ϸ��ʶ
	WORD	wServerID = pPersonalRoomCreateSuccess->wServerID;					//�����ʶ
	WORD	wTableID = pPersonalRoomCreateSuccess->wTableID;					//���ӱ�ʶ
	WORD	wChairCount = pPersonalRoomCreateSuccess->wChairCount;				//��������
	TCHAR szInfo[260] = { 0 };

	wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer dwGroupID = %d  wKindID = %d wServerID = %d wTableID = %d wChairCount = %d"), dwGroupID, wKindID, wServerID, wTableID, wChairCount);
	OutputDebugString(szInfo);



	//Ѱ�Ҿ��ֲ�AI����
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer dwGroupID = %d �Ҳ������ֲ�����false"), dwGroupID);
		OutputDebugString(szInfo);
		return;
	}

	//Ѱ����ϷAI����
	WORD wAIKindID = 0;
	CKindAIUser * pKindAIUser = NULL;
	if (pGroupKindAI->Lookup(wKindID, pKindAIUser) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer dwGroupID = %d �Ҳ���Ϸ����false"), dwGroupID);
		OutputDebugString(szInfo);
		return;
	}


	//�������ֲ���ĳ����Ϸ�����AI
	DWORD wAIUser = 0;
	tagAIUserInfo * pAIUserInfo = NULL;
	POSITION PositionAIUser = pGroupKindAI->GetStartPosition();

	//�������ӻ����˵���Ŀ
	WORD wPlayingAICount = 2;
	if (wChairCount > 2)
	{
		wPlayingAICount = rand() % (wChairCount - 2) + 2;

	}

	wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer Ԥ������Ļ���������  wPlayingAICount = %d"), wPlayingAICount);
	OutputDebugString(szInfo);
	
	int nStatic = 0;
	while (PositionAIUser != NULL)
	{
		pKindAIUser->GetNextAssoc(PositionAIUser, wAIUser, pAIUserInfo);

		if (pAIUserInfo->cbStatus != 0 || pAIUserInfo->cbIsLeave != 0)
		{
			wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer Ԥ������Ļ���������  dwUserID = %d ʹ���� cbStatus = %d  cbIsLeave = %d"), pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbIsLeave);
			OutputDebugString(szInfo);
			continue;
		}

		//����Ȩ���жϵ�ǰAI�Ƿ���Խ��뷿��
		DWORD dwTimeCount = GetAIWaitTimeCount(pAIUserInfo->tTime)/60;
		DWORD dwWeight = GetAIWeightIn(dwTimeCount);
		if (rand() % 100 > dwWeight)
		{
			wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer Ԥ������Ļ���������  dwWeight = %d dwTimeCount = %d"), dwWeight, dwTimeCount);
			OutputDebugString(szInfo);

			continue;
		}

		pAIUserInfo->cbStatus = 1;
		pAIUserInfo->wServerID = wServerID;

		CMD_GR_LogonAI LogonAI;
		LogonAI.dwGroupID = dwGroupID;
		LogonAI.wKindID = wKindID;
		LogonAI.dwUserID = pAIUserInfo->dwUserID;//AIUserInfo.dwUserID;
		LogonAI.wTableID = wTableID;
		
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_S_AI_ACTION, SUB_CS_C_LOGON_AIID, &LogonAI, sizeof(LogonAI));

		nStatic++;

		wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer nStatic = %d  wPlayingAICount = %d dwGroupID = %d wKindID = %d userid = %d wTableID = %d"), nStatic, wPlayingAICount, dwGroupID, wKindID, pAIUserInfo->dwUserID, wTableID);
		OutputDebugString(szInfo);
		if (nStatic >= wPlayingAICount)
		{
			return;
		}
	}
}

//��һ��AI����
void CAIManager::SendOneAIToGameServer(tagAISitFailure * pAISitFailure, DWORD dwSocketID)
{
	DWORD	dwGroupID = pAISitFailure->dwGroupID;					//���ֲ�ID
	WORD	wKindID = pAISitFailure->wKindID;						//��Ϸ��ʶ
	WORD	wServerID = pAISitFailure->wServerID;					//�����ʶ
	WORD	wTableID = pAISitFailure->wTableID;					//���ӱ�ʶ

	TCHAR szInfo[260] = { 0 };

	wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer dwGroupID = %d  wKindID = %d wServerID = %d wTableID = %d "), dwGroupID, wKindID, wServerID, wTableID);
	OutputDebugString(szInfo);

	WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);



	//Ѱ�Ҿ��ֲ�AI����
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer dwGroupID = %d �Ҳ������ֲ�����false"), dwGroupID);
		OutputDebugString(szInfo);
		WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);
		return;
	}

	//Ѱ����ϷAI����
	WORD wAIKindID = 0;
	CKindAIUser * pKindAIUser = NULL;
	if (pGroupKindAI->Lookup(wKindID, pKindAIUser) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer wKindID = %d �Ҳ���Ϸ����false"), dwGroupID);
		OutputDebugString(szInfo);
		WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);
		return;
	}


	//�������ֲ���ĳ����Ϸ�����AI
	DWORD wAIUser = 0;
	tagAIUserInfo * pAIUserInfo = NULL;
	POSITION PositionAIUser = pGroupKindAI->GetStartPosition();


	int nStatic = 0;
	while (PositionAIUser != NULL)
	{
		pKindAIUser->GetNextAssoc(PositionAIUser, wAIUser, pAIUserInfo);

		wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer   dwUserID = %d  cbStatus = %d  cbIsLeave = %d"), pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbStatus);
		OutputDebugString(szInfo);

		//WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);

		if (pAIUserInfo->cbStatus != 0 || pAIUserInfo->cbIsLeave != 0)
		{
			wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer   dwUserID = %d ʹ���� cbStatus = %d  cbIsLeave = %d"), pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbStatus);
			OutputDebugString(szInfo);
			//WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);
			continue;
		}

		//���������ϵ�ǮС�ڽ�������
		if (pAIUserInfo->lScore < pAISitFailure->lEnterLimiteScore)
		{
			wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer   dwUserID = %d  score = %I64d  lEnterLimiteScore = %I64d"), pAIUserInfo->dwUserID, pAIUserInfo->lScore, pAISitFailure->lEnterLimiteScore);
			OutputDebugString(szInfo);
			//WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);

			continue;
		}

		//����Ȩ���жϵ�ǰAI�Ƿ���Խ��뷿��
		DWORD dwTimeCount = GetAIWaitTimeCount(pAIUserInfo->tTime) / 60;
		DWORD dwWeight = GetAIWeightIn(dwTimeCount);
		if (rand() % 100 > dwWeight)
		{
			continue;
		}

		pAIUserInfo->cbStatus = 1;
		pAIUserInfo->wServerID = wServerID;

		CMD_GR_LogonAI LogonAI;
		LogonAI.dwGroupID = dwGroupID;
		LogonAI.wKindID = wKindID;
		LogonAI.dwUserID = pAIUserInfo->dwUserID;//AIUserInfo.dwUserID;
		LogonAI.wTableID = wTableID;

		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_S_AI_ACTION, SUB_CS_C_LOGON_AIID, &LogonAI, sizeof(LogonAI));

		wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer  dwGroupID = %d wKindID = %d userid = %d wTableID = %d ���͵�¼"), dwGroupID, wKindID, pAIUserInfo->dwUserID, wTableID);
		OutputDebugString(szInfo);

		WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);
		break;
	}
}


//����AI״̬
bool CAIManager::UpdateAIStatusInfo(tagAIStatusInfo * pAIStatusInfo)
{
	DWORD	dwGroupID = pAIStatusInfo->dwGroupID;			//���ֲ�ID
	WORD	wKindID = pAIStatusInfo->wKindID;				//��Ϸ��ʶ
	DWORD	dwUserID = pAIStatusInfo->dwUserID;				//���ID

	//Ѱ�Ҿ��ֲ�AI����
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		return false;
	}

	//Ѱ����ϷAI����
	WORD wAIKindID = 0;
	CKindAIUser * pKindAIUser = NULL;
	if (pGroupKindAI->Lookup(wKindID, pKindAIUser) == FALSE)
	{
		return false;
	}


	//Ѱ��AI
	DWORD wAIUser = 0;
	tagAIUserInfo * pAIUserInfo = NULL;
	if (pKindAIUser->Lookup(pAIStatusInfo->dwUserID, pAIUserInfo) == FALSE)
	{
		return false;
	}

	//����״̬��ѫ��
	pAIUserInfo->cbStatus = pAIStatusInfo->cbStatus;
	pAIUserInfo->lScore = pAIStatusInfo->lScore;

	//���AI�˳�����
	if (pAIStatusInfo->cbStatus == 0)
	{
		time(&pAIUserInfo->tTime);
		pAIUserInfo->cbStatus = 0;

		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("ptdtai *** UpdateAIStatusInfo  dwUserID = %d cbStatus = %d  cbIsLeave = %d"), pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbStatus);
		OutputDebugString(szInfo);
	}

	return true;

}

//������������
bool CAIManager::UpdateAITableChange(CMD_SS_G_AI_Update_Table_Count * pTableChange)
{
	DWORD	dwGroupID = pTableChange->dwGroupID;			//���ֲ�ID
	WORD	wKindID = pTableChange->wKindID;				//��Ϸ��ʶ
	DWORD	wServerID = pTableChange->wServerID;				//���ID

	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange dwGroupID = %d wKindID = %d wServerID = %d nTableChange = %d"), dwGroupID, wKindID, wServerID, pTableChange->nTableChange);
	OutputDebugString(szInfo);
	WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);

	//Ѱ�ҷ���
	tagServerItem * pServerItem = NULL;
	if (m_mapServerID.Lookup(wServerID, pServerItem))
	{

		pServerItem->m_wUserTable += pTableChange->nTableChange;

		int nGroupTableCount = 0;
		if (!pServerItem->m_GroupTable.Lookup(dwGroupID, nGroupTableCount))
		{
			pServerItem->m_GroupTable[dwGroupID] = 0;

			wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange pServerItem->m_GroupTable[dwGroupID] 000 ����������  dwGroupID = %d wKindID = %d wServerID = %d count = %d  pServerItem->m_GroupTable[dwGroupID] = %d"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID], pServerItem->m_GroupTable[dwGroupID]);
			OutputDebugString(szInfo);
			WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);

			
		}
		pServerItem->m_GroupTable[dwGroupID] += pTableChange->nTableChange;;

		if (pServerItem->m_wUserTable == INVALID_TABLE || pServerItem->m_wUserTable >= pServerItem->m_GameServer.wTableCount)
		{
			pServerItem->m_wUserTable = 0;
		}

		if (pServerItem->m_GroupTable[dwGroupID] < 0)
		{
			pServerItem->m_GroupTable[dwGroupID] = 0;

			wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange pServerItem->m_GroupTable[dwGroupID] ����������  dwGroupID = %d wKindID = %d wServerID = %d count = %d  pServerItem->m_GroupTable[dwGroupID] = %d"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID], pServerItem->m_GroupTable[dwGroupID]);
			OutputDebugString(szInfo);
			WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);
		}

		int nCount = 0;
		if (!m_mapGroupTable.Lookup(dwGroupID, nCount))
		{
			m_mapGroupTable[dwGroupID] = 0;

			wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange m_mapGroupTable[dwGroupID] ����������  dwGroupID = %d wKindID = %d wServerID = %d count = %d  pServerItem->m_GroupTable[dwGroupID] = %d"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID], pServerItem->m_GroupTable[dwGroupID]);
			OutputDebugString(szInfo);
			WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);
		}
		m_mapGroupTable[dwGroupID] += pTableChange->nTableChange;

		wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange dwGroupID = %d wKindID = %d wServerID = %d count = %d  pServerItem->m_GroupTable[dwGroupID] = %d"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID], pServerItem->m_GroupTable[dwGroupID]);
		OutputDebugString(szInfo);
		WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);


		if (m_mapGroupTable[dwGroupID] < 0)
		{
			m_mapGroupTable[dwGroupID] = 0;
		}

		//�������û�����Զ�����
		if (m_mapGroupTable[dwGroupID] == 0)
		{
			wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange dwGroupID = %d wKindID = %d wServerID = %d count = %d ���Ϊ 0 �Զ���������"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID]);
			OutputDebugString(szInfo);
			WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);

			CreateRoom(wServerID, pServerItem->m_GameServer.dwSurportType, pTableChange->CreateTable);
		}




		return true;
	}

	return false;
}


//�����뿪������Ȩ������
void CAIManager::SetWeightOutOption(WeightOutOption wghtOutOption)
{
	memcpy_s(&m_WeightOutOption, sizeof(m_WeightOutOption), &wghtOutOption, sizeof(wghtOutOption));
}

//���ý�����ϷȨ������
void CAIManager::SetWeightInOption(WeightInOption wghtInOption)
{
	memcpy_s(&m_WeightInOption, sizeof(m_WeightInOption), &wghtInOption, sizeof(wghtInOption));
}

//����AI����ʱ��任
bool CAIManager::SetAITimeChange(TimeChange * pTimeChange)
{
	memcpy_s(&m_TimeChange, sizeof(m_TimeChange), pTimeChange, sizeof(m_TimeChange));
	return true;
}


//����AI����ʱ��任
bool CAIManager::SetAITimeChange(int nIndex, int nWeight)
{
	if (nIndex < 0 || nIndex > 24)
	{
		return false;
	}
	m_TimeChange.dwTimeChange[nIndex] = nWeight;
	return true;
}



//��ȡ����Ȩ��
DWORD CAIManager::GetAIWeightIn(DWORD dwTimeCount)
{
	//Ȩ��λ��
	if (dwTimeCount < m_WeightInOption.WeightInRatio[0].dwTime)
	{
		return m_WeightInOption.WeightInRatio[0].dwInRatio;
	}

	int wPosition = -1;
	for (int i = 1; i < 10; i++)
	{
		if (m_WeightInOption.WeightInRatio[i].dwTime > dwTimeCount)
		{
			wPosition = i - 1;
			return m_WeightInOption.WeightInRatio[i - 1].dwInRatio;
		}
	}

	if (wPosition == -1)
	{
		return m_WeightInOption.WeightInRatio[9].dwInRatio;
	}

	return 0;
}

//��ȡ�뿪Ȩ��
DWORD CAIManager::GetAIWeightOut(DWORD dwTimeCount)
{
	//Ȩ��λ��
	if (dwTimeCount < m_WeightOutOption.WeightOutRatio[0].dwTime)
	{
		return m_WeightOutOption.WeightOutRatio[0].dwOutRatio;
	}

	int wPosition = -1;
	for (int i = 1; i < 10; i++)
	{
		if (m_WeightOutOption.WeightOutRatio[i].dwTime > dwTimeCount)
		{
			wPosition = i - 1;
			return m_WeightOutOption.WeightOutRatio[i - 1].dwOutRatio;
		}
	}

	if (wPosition == -1)
	{
		return m_WeightOutOption.WeightOutRatio[9].dwOutRatio;
	}

	return 0;
}

//��ȡ��ǰʱ���AI��������
DWORD CAIManager::GetTimeAICount(DWORD dwTimeCount)
{
	if (dwTimeCount >= 0 && dwTimeCount < 24)
	{
		return  m_TimeChange.dwTimeChange[dwTimeCount];
	}

	return 0;
}


//ʱ�����
DWORD CAIManager::GetAIWaitTimeCount(time_t tTime)
{
	//��ȡ��ǰʱ��
	time_t curTime;
	time(&curTime);

	//��ȡʱ���
	DWORD dwTime = (DWORD)difftime(curTime, tTime);

	return dwTime;
}


//AI ������뿪
DWORD CAIManager::AIInOrOut(DWORD dwGroupSocketID)
{
	TCHAR szInfo[260] = {0};
	//��ȡ��ǰʱ�����Ҫ��������˵�AI����
	SYSTEMTIME	sysTime;
	GetLocalTime(&sysTime);
	DWORD dwNeedAICount = GetTimeAICount(sysTime.wHour);

	wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut dwNeedAICount = %d ��Ҫ�뿪������"), dwNeedAICount);
	OutputDebugString(szInfo);

	//�������о��ֲ�AI����
	DWORD dwGroupID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	POSITION Position = m_mapGroupAIUser.GetStartPosition();

	//�������о��ֲ�
	while (Position != NULL)
	{
		m_mapGroupAIUser.GetNextAssoc(Position, dwGroupID, pGroupKindAI);
		if (pGroupKindAI != NULL)
		{
			//�������ֲ���������Ϸ
			WORD wAIKindID = 0;
			CKindAIUser * pKindAIUser = NULL;
			POSITION PositionKind = pGroupKindAI->GetStartPosition();

			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut dwNeedAICount = %d  dwGroupID = %d ��Ҫ�뿪������"), dwNeedAICount, dwGroupID);
			OutputDebugString(szInfo);

			while (PositionKind != NULL)
			{
				pGroupKindAI->GetNextAssoc(PositionKind, wAIKindID, pKindAIUser);
				if (pKindAIUser != NULL)
				{	
					wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut dwNeedAICount = %d  dwGroupID = %d kind = %d GetCount = %d"), dwNeedAICount, dwGroupID, wAIKindID, pKindAIUser->GetCount());
					OutputDebugString(szInfo);

					if (pKindAIUser->GetCount() > dwNeedAICount)
					{
						//����� 1 �� 5 ������뿪
						DWORD dwRandLeave = rand() % (pKindAIUser->GetCount() - dwNeedAICount) + 1;

						wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut dwRandLeave = %d ����뿪������"), dwRandLeave);
						OutputDebugString(szInfo);


						//�������ֲ���ĳ����Ϸ�����AI
						DWORD dwAIUser = 0;
						tagAIUserInfo * pAIUserInfo = NULL;
						POSITION PositionAIUser = pGroupKindAI->GetStartPosition();

						DBR_GR_UnLockAI UnLockAI;
						UnLockAI.dwAICount = dwRandLeave;
						UnLockAI.dwGroupID = dwGroupID;
						UnLockAI.wKindID = wAIKindID;
						
						//ɸѡ���뿪�����
						int nStatic = 0;
						while (PositionAIUser != NULL)
						{
							pKindAIUser->GetNextAssoc(PositionAIUser, dwAIUser, pAIUserInfo);

							//ɾ��������Ϸ�е����
							if (pAIUserInfo->cbStatus != 0)
							{
								continue;
							}

							//�жϵ�ǰ��ҵ��뿪Ȩ��
							DWORD dwOnlineCount = GetAIWaitTimeCount(pAIUserInfo->tEnterTime)%60;
							DWORD dwWeight = GetAIWeightOut(dwOnlineCount);

							if (rand() % 100 >= dwWeight)
							{
								continue;
							}

							wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut UserID = %d �뿪����"), pAIUserInfo->dwUserID);
							OutputDebugString(szInfo);

							UnLockAI.dwUserID[nStatic] = pAIUserInfo->dwUserID;

							//ɾ�����
							pKindAIUser->RemoveKey(dwAIUser);
							nStatic++;
							if (nStatic >= dwRandLeave)
							{
								break;
							}

							CMD_SS_G_Update_Group_Status UpdateGroupStatus;
							UpdateGroupStatus.cbStatus = US_GP_OFFLINE;
							UpdateGroupStatus.dwGroupID = dwGroupID;
							UpdateGroupStatus.dwUserID = pAIUserInfo->dwUserID;

							m_pITCPNetworkEngine->SendData(dwGroupSocketID, MDM_CS_S_AI_ACTION, SUB_SG_G_UPDATE_GROUP_STATUS, &UpdateGroupStatus, sizeof(UpdateGroupStatus));
						}	

						if (nStatic > 0)
						{
							//�ı����ݿ��е�״̬
							m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_UNLOCK_AI_USER, DBR_GR_UNLOCK_AI_USER, 0, &UnLockAI, sizeof(UnLockAI));


						}
					}
					else
					{
						//����� 1 �� 5 ������뿪
						if (dwNeedAICount > pKindAIUser->GetCount())
						{
							DWORD dwRandLoad = rand() % (dwNeedAICount - pKindAIUser->GetCount()) + 1;

							//����AI
							DBR_GR_LoadSingleAI LoadSingleAI;
							LoadSingleAI.dwGroupID = dwGroupID;
							LoadSingleAI.wKindID = wAIKindID;
							LoadSingleAI.dwAICount = dwRandLoad;

							wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut �������������� dwRandLoad = %d "), dwRandLoad);
							OutputDebugString(szInfo);
							//�ı����ݿ��е�״̬
							m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_LOAD_SINGLE_AI, DBR_GR_LOAD_SINGLE_AI, 0, &LoadSingleAI, sizeof(DBR_GR_LoadSingleAI));
						}
					}
					
				}
			}
		}
	}

	return 0;
}

//ͳ�ƴ����˵�������
DWORD CAIManager::GetAICount()
{
	//AI����
	DWORD dwAICount = 0;

	//�������о��ֲ�AI����
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	POSITION Position = m_mapGroupAIUser.GetStartPosition();

	//�������о��ֲ�
	while (Position != NULL)
	{
		m_mapGroupAIUser.GetNextAssoc(Position, dwAIID, pGroupKindAI);
		if (pGroupKindAI != NULL)
		{
			//�������ֲ���������Ϸ
			WORD wAIKindID = 0;
			CKindAIUser * pKindAIUser = NULL;
			POSITION PositionKind = pGroupKindAI->GetStartPosition();

			while (PositionKind != NULL)
			{
				pGroupKindAI->GetNextAssoc(PositionKind, wAIKindID, pKindAIUser);
				if (pKindAIUser != NULL)
				{
					dwAICount += pKindAIUser->GetCount();
				}
			}
		}
	}

	return dwAICount;
}

//��������
DWORD CAIManager::CreateRoom(WORD wServerID, WORD wPlayMode, CMD_GR_AICreateTable CreateTable)
{
	OutputDebugString(TEXT("ptdtai *** CreateRoom AI ��������"));
	//�������ֲ���ĳ����Ϸ�����AI
	WORD wAIServerID = 0;
	tagServerItem * pServerItem = NULL;
	POSITION PositionAIUser = m_mapServerID.GetStartPosition();
	TCHAR szInfo[260] = { 0 };
	int nStatic = 0;
	while (PositionAIUser != NULL)
	{
		m_mapServerID.GetNextAssoc(PositionAIUser, wAIServerID, pServerItem);

		if (!pServerItem)
		{
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom  Ѱ�ҷ��� dwAIServerID = %d"), wAIServerID);
			OutputDebugString(szInfo);
			continue;
		}
		tagAIUserInfo * pAIUserInfo = GetSingleAICount(CreateTable.dwGroupID, pServerItem->m_GameServer.wKindID);
		if (!pAIUserInfo)
		{
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom �Ҳ������ dwGroupID = %d wKindID = %d"), CreateTable.dwGroupID, pServerItem->m_GameServer.wKindID);
			OutputDebugString(szInfo);
			continue;
		}

		if (pServerItem->m_dwSocketID == 0)
		{
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** ���䲻����"));
			OutputDebugString(szInfo);
			continue;
		}

		if (pServerItem->m_GameServer.dwSurportType == 0 || wPlayMode == 0)
		{
			continue;
		}

		CreateTable.dwUserID = pAIUserInfo->dwUserID;

		if (pServerItem && ((pServerItem->m_wUserTable < pServerItem->m_GameServer.wTableCount) && wPlayMode == pServerItem->m_GameServer.dwSurportType))
		{
			
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom AI �������� ������Ϣ dwSocketID = %d"), pServerItem->m_dwSocketID);
			OutputDebugString(szInfo);
		
			m_pITCPNetworkEngine->SendData(pServerItem->m_dwSocketID, MDM_CS_S_AI_ACTION, SUB_GR_AI_CREATE_TABLE, &CreateTable, sizeof(CreateTable));
			break;
		}

	}


	return 0;
}


//��AI����
tagAIUserInfo * CAIManager::GetSingleAICount(DWORD dwGroupID, WORD wKindID)
{
	TCHAR szInfo[260] = { 0 };

	//Ѱ�Ҿ��ֲ�AI����
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom �Ҳ����þ��ֲ��µ���Ϸ dwGroupID = %d wKindID = %d"), dwGroupID, wKindID);
		OutputDebugString(szInfo);
		return NULL;
	}

	//Ѱ����ϷAI����
	WORD wAIKindID = 0;
	CKindAIUser * pKindAIUser = NULL;
	if (pGroupKindAI->Lookup(wKindID, pKindAIUser) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom �Ҳ����þ��ֲ��µ���Ϸ�µ���� dwGroupID = %d wKindID = %d"), dwGroupID, wKindID);
		OutputDebugString(szInfo);

		return NULL;
	}


	//�������ֲ���ĳ����Ϸ�����AI
	DWORD wAIUser = 0;
	tagAIUserInfo * pAIUserInfo = NULL;
	POSITION PositionAIUser = pGroupKindAI->GetStartPosition();


	int nStatic = 0;
	while (PositionAIUser != NULL)
	{
		pKindAIUser->GetNextAssoc(PositionAIUser, wAIUser, pAIUserInfo);

		if (pAIUserInfo->cbStatus != 0)
		{
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom �Ҳ����þ��ֲ��µ���Ϸ�µ���� dwGroupID = %d wKindID = %d userid = %d �Ѿ���ʹ��"), dwGroupID, wKindID, pAIUserInfo->dwUserID);
			OutputDebugString(szInfo);
			continue;
		}

		return pAIUserInfo;
	}

	return NULL;
}

//��ȡ����AI
bool CAIManager::DeleteServer(DWORD dwSocketID)
{
	//�������ֲ���ĳ����Ϸ�����AI
	WORD wAIServerID = 0;
	tagServerItem * pServerItem = NULL;
	POSITION PositionAIUser = m_mapServerID.GetStartPosition();

	int nStatic = 0;
	while (PositionAIUser != NULL)
	{
		m_mapServerID.GetNextAssoc(PositionAIUser, wAIServerID, pServerItem);

		if (pServerItem && pServerItem->m_dwSocketID == dwSocketID)
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** ɾ������ dwSocketID = %d  wAIServerID = %d GetCount = %d"), pServerItem->m_dwSocketID, wAIServerID, m_mapGroupTable.GetCount());
			OutputDebugString(szInfo);
			pServerItem->m_dwSocketID = 0;
			m_mapServerID.RemoveKey(wAIServerID);

			DWORD dwGroupID = 0;
			int nTalbeCount = 0;

			POSITION PositionGroupTable = m_mapGroupTable.GetStartPosition();
			while (PositionGroupTable != NULL)
			{
				m_mapGroupTable.GetNextAssoc(PositionGroupTable, dwGroupID, nTalbeCount);

				wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** ɾ������ dwGroupID = %d nTalbeCount = %d"), dwGroupID, nTalbeCount);
				OutputDebugString(szInfo);

				if (dwGroupID > 0)
				{
					m_mapGroupTable[dwGroupID] -= pServerItem->m_GroupTable[dwGroupID];

					wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** ɾ������ dwGroupID = %d m_mapGroupTable[dwGroupID] = %d   pServerItem->m_GroupTable[dwGroupID]"), dwGroupID, m_mapGroupTable[dwGroupID], pServerItem->m_GroupTable[dwGroupID]);
					OutputDebugString(szInfo);

					if (m_mapGroupTable[dwGroupID] <= 0)
					{
						wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** ɾ������ dwSocketID = %d  wAIServerID = %d dwGroupID = %d ����Ϊ�� �ȴ�����"), pServerItem->m_dwSocketID, wAIServerID, dwGroupID);
						OutputDebugString(szInfo);
						m_mapGroupTable[dwGroupID] = 0;
					}
				}
			}
			
			//�ָ����������״̬
			ResetUserStatus(pServerItem->m_GameServer.wServerID);
	
			break;
		}

	}
	return true;
}

//ͳ�Ƶ�ǰ���ֲ�ʹ�����ӵ�����
bool CAIManager::GroupCreateRoomCount(DWORD dwGroupID, WORD wKindID)
{
	return true;
}

//�����뿪������Ȩ������
void CAIManager::SetWeightOutOption(int nIndex, tagWeightOutRatio WeightOutRatio)
{
	if (nIndex < 1 || nIndex > 10)
	{
		return;
	}
	m_WeightOutOption.WeightOutRatio[nIndex] = WeightOutRatio;
}

//���ý�����ϷȨ������
void CAIManager::SetWeightInOption(int nIndex, tagWeightInRatio WeightInRatio)
{
	if (nIndex < 1 || nIndex > 10)
	{
		return;
	}
	m_WeightInOption.WeightInRatio[nIndex - 1] = WeightInRatio;
}

// �����淨
bool CAIManager::InsertGroupGameRule(tagGroupConfigInfo GroupConfigInfo)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule �����淨 count = %d configid = %d"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
	OutputDebugString(szInfo);

	//�ж��淨�Ƿ����
	bool bIsGameRuleExist = false;
	for (int i = 0; i < m_ArrayGroupGameRuleTable.GetCount(); i++)
	{
		if (m_ArrayGroupGameRuleTable[i].dwConfigID == GroupConfigInfo.dwConfigID)
		{
			bIsGameRuleExist = true;
			break;
		}
	}

	if (!bIsGameRuleExist)
	{
		wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule �����淨 count = %d configid = %d �ɹ�"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
		OutputDebugString(szInfo);

		m_ArrayGroupGameRuleTable.Add(GroupConfigInfo);
	}
	else
	{
		wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule �����淨 count = %d configid = %d �淨�Ѿ�����"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
		OutputDebugString(szInfo);
	}

	return true;
}


// ɾ���淨
bool CAIManager::DeleteGroupGameRule(tagGroupConfigInfo GroupConfigInfo)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule ɾ���淨 count = %d configid = %d"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
	OutputDebugString(szInfo);
	for (int i = 0; i < m_ArrayGroupGameRuleTable.GetCount(); i++)
	{
		if (m_ArrayGroupGameRuleTable[i].dwConfigID == GroupConfigInfo.dwConfigID)
		{
			wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule ɾ���淨 count = %d configid = %d �ɹ�"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
			OutputDebugString(szInfo);

			m_ArrayGroupGameRuleTable.RemoveAt(i);
			break;
		}
	}

	return true;
}

// �޸��淨
bool CAIManager::ModifyGroupGameRule(tagGroupConfigInfo GroupConfigInfo)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule �޸��淨 count = %d configid = %d"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
	OutputDebugString(szInfo);
	for (int i = 0; i < m_ArrayGroupGameRuleTable.GetCount(); i++)
	{
		if (m_ArrayGroupGameRuleTable[i].dwConfigID == GroupConfigInfo.dwConfigID)
		{
			wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule �޸��淨 count = %d configid = %d �ɹ�"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
			OutputDebugString(szInfo);
			memcpy_s(&m_ArrayGroupGameRuleTable[i], sizeof(m_ArrayGroupGameRuleTable[i]), &GroupConfigInfo, sizeof(GroupConfigInfo));
			break;
		}
	}

	return true;
}

//��������Ƿ�Ϊ��
bool CAIManager::CheckGroupRoomIsNull()
{
	//��־�����Ϣ
	TCHAR szInfo[260] = { 0 };

	WORD	wKindID = 0;				//��Ϸ��ʶ
	DWORD	wServerID = 0;				//���ID
		
	//�������о��ֲ�AI����
	DWORD dwGroupID = 0;
	CKindAIParameter * pKindAIParameter = NULL;
	POSITION Position = m_mapGroupAIParameter.GetStartPosition();

	struct tagAreadyCheckKind
	{
		DWORD dwGroupID;
		WORD  wKindID;
	};

	CWHArray<tagAreadyCheckKind> arrayKindID;

	//�������о��ֲ�
	while (Position != NULL)
	{
		m_mapGroupAIParameter.GetNextAssoc(Position, dwGroupID, pKindAIParameter);
		if (pKindAIParameter != NULL)
		{
			wsprintf(szInfo, TEXT("ptdtai *** CheckGroupRoomIsNull dwGroupID = %d "), dwGroupID);
			OutputDebugString(szInfo);

			int nCount = 0;
			if (!m_mapGroupTable.Lookup(dwGroupID, nCount) || m_mapGroupTable[dwGroupID] == 0)
			{
				m_mapGroupTable[dwGroupID] = 0;
				wsprintf(szInfo, TEXT("ptdtai *** CheckGroupRoomIsNull dwGroupID = %d tableCount = %d"), dwGroupID, m_mapGroupTable[dwGroupID]);
				OutputDebugString(szInfo);

				//������ֲ��µ���������Ϊ0
				if (m_mapGroupTable[dwGroupID] == 0)
				{
					//�����þ��ֲ��µ������淨
					for (int i = 0; i < m_ArrayGroupGameRuleTable.GetCount(); i++)
					{
						wsprintf(szInfo, TEXT("ptdtai *** CheckGroupRoomIsNull dwGroupID = %d tableCount = %d kindid = %d configid = %d"), dwGroupID, m_mapGroupTable[dwGroupID], 
							m_ArrayGroupGameRuleTable[i].wKindID, m_ArrayGroupGameRuleTable[i].dwConfigID);
						OutputDebugString(szInfo);

						//�����ǰ�������Ѿ�������ģ�������
						//bool bIsCheck = false;
						//for (int k = 0; k < arrayKindID.GetCount(); k++)
						//{
						//	//WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("Ԫ�� dwGroupID = %d  wKindID = %d"), arrayKindID[k].dwGroupID, arrayKindID[k].wKindID);
						//	if (arrayKindID[k].wKindID == m_ArrayGroupGameRuleTable[i].wKindID &&  arrayKindID[k].dwGroupID == m_ArrayGroupGameRuleTable[i].dwGroupID)
						//	{
						//		bIsCheck = true;

						//		//WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("��ͬ���� dwGroupID = %d  wKindID = %d"), arrayKindID[k].dwGroupID, arrayKindID[k].wKindID);
						//		break;
						//	}
						//}

						//if (bIsCheck)
						//{
						//	continue;
						//}

						//�������з�������ѡ���ʺ���Ϸ���͵ķ�����
						WORD wServerID = 0;
						tagServerItem * pServerItem = NULL;
						POSITION PositionServer = m_mapServerID.GetStartPosition();

						//�������з�����
						while (PositionServer != NULL)
						{
							m_mapServerID.GetNextAssoc(PositionServer, wServerID, pServerItem);

							//����ǻ���ģʽ ������
							if (pServerItem && pServerItem->m_GameServer.dwSurportType == 0)
							{
								continue;
							}

							wsprintf(szInfo, TEXT("ptdtai *** CheckGroupRoomIsNull dwGroupID = %d tableCount = %d kindid = %d configid = %d serverid = %d serverkind = %d"), dwGroupID, m_mapGroupTable[dwGroupID],
								m_ArrayGroupGameRuleTable[i].wKindID, m_ArrayGroupGameRuleTable[i].dwConfigID, wServerID, pServerItem->m_GameServer.wKindID);
							OutputDebugString(szInfo);

							//WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);

							//ѡ���ʺ���Ϸ���͵ķ�����
							if (pServerItem && pServerItem->m_GameServer.wKindID == m_ArrayGroupGameRuleTable[i].wKindID && m_ArrayGroupGameRuleTable[i].dwGroupID == dwGroupID)
							{
								tagAreadyCheckKind AreadyCheckKind;
								AreadyCheckKind.dwGroupID = m_ArrayGroupGameRuleTable[i].dwGroupID;
								AreadyCheckKind.wKindID = m_ArrayGroupGameRuleTable[i].wKindID;
								arrayKindID.Add(AreadyCheckKind);

								//��������
								CMD_GR_AICreateTable CreateTable;

								CreateTable.lCellScore = m_ArrayGroupGameRuleTable[i].lCellScore;							    //�׷�����
								CreateTable.dwDrawCountLimit = m_ArrayGroupGameRuleTable[i].dwDrawCountLimit;					//��������
								CreateTable.dwDrawTimeLimit = m_ArrayGroupGameRuleTable[i].dwDrawTimeLimit;					//ʱ������
								CreateTable.wJoinGamePeopleCount = m_ArrayGroupGameRuleTable[i].wJoinGamePeopleCount;			//������Ϸ������
								CreateTable.dwRoomTax = m_ArrayGroupGameRuleTable[i].dwRoomTax;								//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
								CreateTable.cbPassword = m_ArrayGroupGameRuleTable[i].cbPassword;								//�������� 0:������ 1:��Ҫ����
								CreateTable.cbPayMode = m_ArrayGroupGameRuleTable[i].cbPayMode;								//֧����ʽ									
								memcpy_s(CreateTable.cbGameRule, RULE_LEN, m_ArrayGroupGameRuleTable[i].cbGameRule, RULE_LEN);
								CreateTable.cbGameMode = m_ArrayGroupGameRuleTable[i].cbGameMode;								//��Ϸģʽ
								CreateTable.lEnterScore = m_ArrayGroupGameRuleTable[i].lEnterScore;							//���뷿��ķ�������
								CreateTable.lLeaveScore = m_ArrayGroupGameRuleTable[i].lLeaveScore;							//�뿪�����������
								CreateTable.cbJoinMode = m_ArrayGroupGameRuleTable[i].cbJoinMode;								//��Ҳ��뷽ʽ	0 ������İ���˲���	1 ����İ���˲���	
								CreateTable.dwGroupID = m_ArrayGroupGameRuleTable[i].dwGroupID;								//���ֲ�id�� �Ǿ��ֲ�ģʽ �� 0
								CreateTable.dwConfigID = m_ArrayGroupGameRuleTable[i].dwConfigID;								//�淨ID

								wsprintf(szInfo, TEXT("ptdtai *** CheckGroupRoomIsNull dwGroupID = %d wKindID = %d wServerID = %d count = %d ���Ϊ 0 �Զ���������"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID]);
								OutputDebugString(szInfo);
								WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);
								CreateRoom(wServerID, pServerItem->m_GameServer.dwSurportType, CreateTable);
								break;
							}
						}
					}
				}
			}
		}

	}

	arrayKindID.RemoveAll();
	arrayKindID.FreeMemory();
	return true;
}


//������뿪
bool CAIManager::SetUserLeave(CMD_CS_C_DeleteAI DeleteAI)
{
	TCHAR szInfo[260] = { 0 };

	//Ѱ�Ҿ��ֲ�AI����
	DWORD dwGroupID = DeleteAI.dwGroupID;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai ****  SetUserLeave ���ֲ������� dwGroupID = %d "), dwGroupID);
		OutputDebugString(szInfo);
		return false;
	}


	//
	for (int i = 0; i < 20; i++)
	{
		//Ѱ����ϷAI����
		WORD wAIKindID = 0;
		CKindAIUser * pKindAIUser = NULL;
		if (DeleteAI.DeleteAI[i].wKindID == 0)
		{
			//��ʵ���
		}
		else
		{
			if (pGroupKindAI->Lookup(DeleteAI.DeleteAI[i].wKindID, pKindAIUser) == FALSE)
			{
				wsprintf(szInfo, TEXT("ptdtai **** SetUserLeave �Ҳ����þ��ֲ��µ���Ϸ�µ���� dwGroupID = %d wKindID = %d"), dwGroupID, DeleteAI.DeleteAI[i].wKindID);
				OutputDebugString(szInfo);
				continue;
			}

			if (DeleteAI.DeleteAI[i].dwUserID != 0)
			{
				tagAIUserInfo * pAIUserInfo = NULL;
				pKindAIUser->Lookup(DeleteAI.DeleteAI[i].dwUserID, pAIUserInfo);

				if (pAIUserInfo)
				{
					if (DeleteAI.cbOperate == 0)//����
					{
						pAIUserInfo->cbIsLeave = 1;

						tagDeleteGroupUser DeleteGroupUser;
						DeleteGroupUser.dwGroupID = dwGroupID;
						DeleteGroupUser.dwUserID = pAIUserInfo->dwUserID;
						DeleteGroupUser.wKindID = DeleteAI.DeleteAI[i].wKindID;
						DeleteGroupUser.cbOperateID = 1;

						wsprintf(szInfo, TEXT("ptdtai **** SetUserLeave ���� dwGroupID = %d wKindID = %d userid = %d ����������б�"), dwGroupID, DeleteAI.DeleteAI[i].wKindID, DeleteAI.DeleteAI[i].dwUserID);
						OutputDebugString(szInfo);

						m_ArrayDeleteGroupUser.Add(DeleteGroupUser);
					
					}
					else if (DeleteAI.cbOperate == 1)//�˳�������
					{
						pAIUserInfo->cbIsLeave = 2;

						tagDeleteGroupUser DeleteGroupUser;
						DeleteGroupUser.dwGroupID = dwGroupID;
						DeleteGroupUser.dwUserID = pAIUserInfo->dwUserID;
						DeleteGroupUser.wKindID = DeleteAI.DeleteAI[i].wKindID;
						DeleteGroupUser.cbOperateID = 2;


						m_ArrayDeleteGroupUser.Add(DeleteGroupUser);

						wsprintf(szInfo, TEXT("ptdtai **** SetUserLeave �˳������� dwGroupID = %d wKindID = %d userid = %d ����������б�"), dwGroupID, DeleteAI.DeleteAI[i].wKindID, DeleteAI.DeleteAI[i].dwUserID);
						OutputDebugString(szInfo);

					}

				}
			}
		}
		
	}

	return true;
}


//������뿪
bool CAIManager::LetUserLeave()
{
	DBR_GR_UnLockAI UnLockAI;
	ZeroMemory(&UnLockAI, sizeof(UnLockAI));

	DBR_GR_AIQuitGroup AIQuitGroup;
	ZeroMemory(&AIQuitGroup, sizeof(AIQuitGroup));

	TCHAR szInfo[260] = {0};
	for (int i = m_ArrayDeleteGroupUser.GetCount() - 1; i >= 0; i--)
	{
		if (m_ArrayDeleteGroupUser[i].cbOperateID == 1)
		{
			UnLockAI.dwAICount++;
			UnLockAI.dwGroupID = m_ArrayDeleteGroupUser[i].dwGroupID;
			UnLockAI.wKindID = m_ArrayDeleteGroupUser[i].wKindID;
			UnLockAI.dwUserID[UnLockAI.dwAICount - 1] = m_ArrayDeleteGroupUser[i].dwUserID;

			wsprintf(szInfo, TEXT("ptdtai **** LetUserLeave ���� dwGroupID = %d wKindID = %d userid = %d dwAICount = %d ִ�в���"), m_ArrayDeleteGroupUser[i].dwGroupID, m_ArrayDeleteGroupUser[i].wKindID, m_ArrayDeleteGroupUser[i].dwUserID, UnLockAI.dwAICount);
			OutputDebugString(szInfo);

			m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_UNLOCK_AI_USER, DBR_GR_UNLOCK_AI_USER, 0, &UnLockAI, sizeof(UnLockAI));

			if (UnLockAI.dwAICount >= 10)
			{
				ZeroMemory(&UnLockAI, sizeof(UnLockAI));
			}

			m_ArrayDeleteGroupUser.RemoveAt(i);
		}
		else if (m_ArrayDeleteGroupUser[i].cbOperateID == 2)
		{
			AIQuitGroup.dwAICount++;
			AIQuitGroup.dwGroupID = m_ArrayDeleteGroupUser[i].dwGroupID;
			AIQuitGroup.wKindID = m_ArrayDeleteGroupUser[i].wKindID;
			AIQuitGroup.dwUserID[AIQuitGroup.dwAICount - 1] = m_ArrayDeleteGroupUser[i].dwUserID;


			wsprintf(szInfo, TEXT("ptdtai **** LetUserLeave �˳������� dwGroupID = %d wKindID = %d userid = %d dwAICount = %d ִ�в���"), m_ArrayDeleteGroupUser[i].dwGroupID, m_ArrayDeleteGroupUser[i].wKindID, m_ArrayDeleteGroupUser[i].dwUserID, AIQuitGroup.dwAICount);
			OutputDebugString(szInfo);

			m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_AI_QUIT_GROUP, DBR_GR_AI_QUIT_GROUP, 0, &AIQuitGroup, sizeof(AIQuitGroup));

			if (AIQuitGroup.dwAICount >= 10)
			{
				ZeroMemory(&AIQuitGroup, sizeof(AIQuitGroup));
			}
			m_ArrayDeleteGroupUser.RemoveAt(i);
		}
	}

	return true;
}


//�������״̬
bool CAIManager::ResetUserStatus(WORD wServerID)
{
	TCHAR szInfo[260] = { 0 };
	//��ȡ��ǰʱ�����Ҫ��������˵�AI����


	//�������о��ֲ�AI����
	DWORD dwGroupID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	POSITION Position = m_mapGroupAIUser.GetStartPosition();

	//�������о��ֲ�
	while (Position != NULL)
	{
		m_mapGroupAIUser.GetNextAssoc(Position, dwGroupID, pGroupKindAI);
		if (pGroupKindAI != NULL)
		{
			//�������ֲ���������Ϸ
			WORD wAIKindID = 0;
			CKindAIUser * pKindAIUser = NULL;
			POSITION PositionKind = pGroupKindAI->GetStartPosition();

			while (PositionKind != NULL)
			{
				pGroupKindAI->GetNextAssoc(PositionKind, wAIKindID, pKindAIUser);
				if (pKindAIUser != NULL)
				{

					//�������ֲ���ĳ����Ϸ�����AI
					DWORD dwAIUser = 0;
					tagAIUserInfo * pAIUserInfo = NULL;
					POSITION PositionAIUser = pGroupKindAI->GetStartPosition();

					//ɸѡ���뿪�����
					int nStatic = 0;
					while (PositionAIUser != NULL)
					{
						pKindAIUser->GetNextAssoc(PositionAIUser, dwAIUser, pAIUserInfo);

						//ɾ��������Ϸ�е����
						if (pAIUserInfo->wServerID == wServerID)
						{
							pAIUserInfo->cbStatus = 0;

							wsprintf(szInfo, TEXT("ptdtai **** ResetUserStatus  wServerID = %d wKindID = %d userid = %d �ָ�״̬"), wServerID, wAIKindID, pAIUserInfo->dwUserID);
							OutputDebugString(szInfo);
						}
					}
				}
			}
		}
	}

	return true;
}


//����AI����
bool CAIManager::UpdateAIScore(tagAIStatusInfo * pAIStatusInfo)
{
	DWORD	dwGroupID = pAIStatusInfo->dwGroupID;			//���ֲ�ID
	WORD	wKindID = pAIStatusInfo->wKindID;				//��Ϸ��ʶ
	DWORD	dwUserID = pAIStatusInfo->dwUserID;				//���ID

	//Ѱ�Ҿ��ֲ�AI����
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		return false;
	}

	//�������ֲ���������Ϸ
	WORD wAIKindID = 0;
	CKindAIUser * pKindAIUser = NULL;
	POSITION PositionKind = pGroupKindAI->GetStartPosition();

	while (PositionKind != NULL)
	{
		pGroupKindAI->GetNextAssoc(PositionKind, wAIKindID, pKindAIUser);
		if (pKindAIUser != NULL)
		{

			//�������ֲ���ĳ����Ϸ�����AI
			DWORD dwAIUser = 0;
			tagAIUserInfo * pAIUserInfo = NULL;
			POSITION PositionAIUser = pGroupKindAI->GetStartPosition();

			//ɸѡ���뿪�����
			int nStatic = 0;
			while (PositionAIUser != NULL)
			{
				pKindAIUser->GetNextAssoc(PositionAIUser, dwAIUser, pAIUserInfo);

				//ɾ��������Ϸ�е����
				if (pAIUserInfo->dwUserID == pAIStatusInfo->dwUserID)
				{
					//������ҷ���
					pAIUserInfo->lScore = pAIStatusInfo->lScore;

					TCHAR szInfo[260] = { 0 };
					wsprintf(szInfo, TEXT("ptdtai *** UpdateAIStatusInfo  groupid = %d dwUserID = %d lscore = %d"), dwGroupID, pAIUserInfo->dwUserID, pAIUserInfo->lScore);
					OutputDebugString(szInfo);
					WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);
				}
			}
		}
	}

	return true;
}



//////////////////////////////////////////////////////////////////////////////////
