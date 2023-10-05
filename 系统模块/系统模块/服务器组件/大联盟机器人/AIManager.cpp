#include "StdAfx.h"
#include "AIManager.h"
#include "DataBasePacket.h"


//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAIManager::CAIManager()
{
	m_mapGroupAIParameter.InitHashTable(PRIME_SERVER);
	m_mapGroupAIUser.InitHashTable(PRIME_PLATFORM_USER);
	WHLogControl::SetLogStatus(1);
}

//析构函数
CAIManager::~CAIManager()
{
	m_mapGroupAIParameter.RemoveAll();
}

//插入俱乐部参数
byte CAIManager::InsertGroupParameter(tagGroupAIParameter GroupAIParameter)
{
	//添加桌子
	CKindAIParameter* pKindAIParameter = NULL;

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai **** ***************************************************GroupID = %d  wKindID = %d"), GroupAIParameter.dwGroupID, GroupAIParameter.wKindID);
	OutputDebugString(szInfo);

	//添加桌子
	if (m_mapGroupAIParameter.Lookup(GroupAIParameter.dwGroupID, pKindAIParameter) == FALSE)
	{
		//申请内存
		pKindAIParameter = new CKindAIParameter();
		pKindAIParameter->InitHashTable(PRIME_SERVER);

		//俱乐部所有游戏类型参数的集合
		m_mapGroupAIParameter[GroupAIParameter.dwGroupID] = pKindAIParameter;

		//俱乐部单个游戏的参数
		tagGroupAIParameter * pGroupAIParameter = new tagGroupAIParameter();
		memcpy_s(pGroupAIParameter, sizeof(tagGroupAIParameter), &GroupAIParameter, sizeof(tagGroupAIParameter));
		(*pKindAIParameter)[GroupAIParameter.wKindID] =  pGroupAIParameter;
	}
	else
	{
		//俱乐部单个游戏的参数
		tagGroupAIParameter * pGroupAIParameter = new tagGroupAIParameter();
		memcpy_s(pGroupAIParameter, sizeof(tagGroupAIParameter), &GroupAIParameter, sizeof(tagGroupAIParameter));
		(*pKindAIParameter)[GroupAIParameter.wKindID] = pGroupAIParameter;
		return false;
	}

	return true;
}

//获取AI参数
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


//设置数据库引擎
void CAIManager::SetDBCorrespondManager(IDBCorrespondManager *	pIDBCorrespondManager, ITCPNetworkEngine *	pITCPNetworkEngine)
{
	m_pIDBCorrespondManager = pIDBCorrespondManager;
	m_pITCPNetworkEngine = pITCPNetworkEngine;
}

//发送加载AI
void CAIManager::SendLoadOneKindAIAction(DWORD dwGroupID, WORD wKindID)
{
	//添加桌子
	CKindAIParameter* pKindAIParameter = NULL;

	//判断俱乐部是否存在
	if (m_mapGroupAIParameter.Lookup(dwGroupID, pKindAIParameter) == FALSE)
	{
		return;
	}

	//判断游戏AI是否存在
	tagGroupAIParameter * pGroupAIParameter = NULL;
	if ((*pKindAIParameter).Lookup(wKindID, pGroupAIParameter) == FALSE)
	{
		return;
	}

	//获取当前游戏AI参数
	tagGroupAIParameter GroupAIParameter;

	memcpy_s(&GroupAIParameter, sizeof(tagGroupAIParameter), (*pKindAIParameter)[wKindID], sizeof(tagGroupAIParameter));

	//发送加载机器操作
	m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_LOAD_AI_USER, DBR_GR_LOAD_AI_USER, 0, &GroupAIParameter, sizeof(GroupAIParameter));


}

//发送加载所有AI
void CAIManager::SendLoadAllAIAction()
{
	//遍历所有俱乐部AI参数
	DWORD dwAIID = 0;
	CKindAIParameter * pKindAIParameter = NULL;
	POSITION Position = m_mapGroupAIParameter.GetStartPosition();

	//遍历所有俱乐部
	while (Position != NULL)
	{
		m_mapGroupAIParameter.GetNextAssoc(Position, dwAIID, pKindAIParameter);
		if (pKindAIParameter != NULL)
		{
			//遍历俱乐部下所有游戏
			WORD wAIKindID = 0;
			
			tagGroupAIParameter * pGroupAIParameter = NULL;
			POSITION PositionKind = pKindAIParameter->GetStartPosition();

			while (PositionKind != NULL)
			{
				pKindAIParameter->GetNextAssoc(PositionKind, wAIKindID, pGroupAIParameter);

				//获取当前游戏AI参数
				tagGroupAIParameter GroupAIParameter;

				ZeroMemory(&GroupAIParameter, sizeof(GroupAIParameter));

				memcpy_s(&GroupAIParameter, sizeof(tagGroupAIParameter), pGroupAIParameter, sizeof(tagGroupAIParameter));

				//发送加载机器操作
				m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_LOAD_AI_USER, DBR_GR_LOAD_AI_USER, 0, &GroupAIParameter, sizeof(GroupAIParameter));
			}
		}
	}
}

//发送加载所有AI
void CAIManager::InsertAIUser(DWORD dwGroupID, WORD wKindID, tagAIUserInfo * pAIUserInfo)
{
	TCHAR szInfo[260] = {0};
	//参数初始化
	CGroupKindAI* pGroupKindAI = NULL;

	//添加俱乐部中某类型的玩家
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		//申请内存
		pGroupKindAI = new CGroupKindAI();
		pGroupKindAI->InitHashTable(PRIME_PLATFORM_USER);

		//俱乐部所有游戏类型参数的集合
		m_mapGroupAIUser[dwGroupID] = pGroupKindAI;

		CKindAIUser * pKindAIUser = new CKindAIUser();
		(*pGroupKindAI)[wKindID] = pKindAIUser;


		//俱乐部单个游戏的AI玩家
		tagAIUserInfo * pAIUserInfoNew = new tagAIUserInfo();

		memcpy_s(pAIUserInfoNew, sizeof(tagAIUserInfo), pAIUserInfo, sizeof(tagAIUserInfo));
		(*pKindAIUser)[pAIUserInfo->dwUserID] = pAIUserInfoNew;

		wsprintf(szInfo, TEXT("ptdtai **** InsertAIUser dwGroupID = %d wKindID = %d userid = %d cbStatus = %d cbIsLeave = %d"), dwGroupID, wKindID, pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbIsLeave);
		
		OutputDebugString(szInfo);
	}
	else
	{
		CKindAIUser * pKindAIUser = NULL;

		//添加某类型的玩家
		if (pGroupKindAI->Lookup(wKindID, pKindAIUser) == FALSE)
		{
			pKindAIUser = new CKindAIUser();
			(*pGroupKindAI)[wKindID] = pKindAIUser;


			//俱乐部单个游戏的AI玩家
			tagAIUserInfo * pAIUserInfoNew = new tagAIUserInfo();

			memcpy_s(pAIUserInfoNew, sizeof(tagAIUserInfo), pAIUserInfo, sizeof(tagAIUserInfo));
			(*pKindAIUser)[pAIUserInfo->dwUserID] = pAIUserInfoNew;

			wsprintf(szInfo, TEXT("ptdtai **** InsertAIUser dwGroupID = %d wKindID = %d userid = %d cbStatus = %d cbIsLeave = %d"), dwGroupID, wKindID, pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbIsLeave);
			OutputDebugString(szInfo);
		}
		else
		{
			//俱乐部单个游戏的AI玩家
			tagAIUserInfo * pAIUserInfoNew = new tagAIUserInfo();
			memcpy_s(pAIUserInfoNew, sizeof(tagAIUserInfo), pAIUserInfo, sizeof(tagAIUserInfo));
			(*pKindAIUser)[pAIUserInfo->dwUserID] = pAIUserInfoNew;

			wsprintf(szInfo, TEXT("ptdtai **** InsertAIUser dwGroupID = %d wKindID = %d userid = %d cbStatus = %d cbIsLeave = %d"), dwGroupID, wKindID, pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbIsLeave);
			OutputDebugString(szInfo);
		}

	}
}

//向游戏服发送AI 使其登录
void CAIManager::SendAIToGroupServer(DWORD dwGroupID, WORD wKindID, DWORD dwSocketID)
{
	//遍历所有俱乐部AI参数
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	POSITION Position = m_mapGroupAIUser.GetStartPosition();

	TCHAR szInfo[260] = { 0 };
	if (Position == NULL)
	{
		wsprintf(szInfo, TEXT("ptdtai *** SendAIToGroupServer 俱乐部不存在"));
		OutputDebugString(szInfo);
	}
	

	//遍历所有俱乐部
	while (Position != NULL)
	{
		m_mapGroupAIUser.GetNextAssoc(Position, dwAIID, pGroupKindAI);
		if (pGroupKindAI != NULL)
		{
			//遍历俱乐部下所有游戏
			WORD wAIKindID = 0;
			CKindAIUser * pKindAIUser = NULL;
			POSITION PositionKind = pGroupKindAI->GetStartPosition();

			while (PositionKind != NULL)
			{
				pGroupKindAI->GetNextAssoc(PositionKind, wAIKindID, pKindAIUser);
				if (pKindAIUser != NULL)
				{

					//遍历俱乐部下某种游戏里面的AI
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
							wsprintf(szInfo, TEXT("ptdtai *** SendAIToGroupServer 玩家不存在"));
							OutputDebugString(szInfo);
						}
						
					}
				}
				else
				{
					wsprintf(szInfo, TEXT("ptdtai *** SendAIToGroupServer 游戏类型不存在"));
					OutputDebugString(szInfo);
				}
		

			}
		}
	}
}

//插入房间
bool CAIManager::InsertServerItem(DWORD dwSocketID,  tagGameServer & GameServer)
{
	//寻找房间
	tagServerItem * pServerItem = NULL;
	if (m_mapServerID.Lookup(GameServer.wServerID, pServerItem) == TRUE)
	{
		////ASSERT(FALSE);
		return false;
	}

	//创建房间
	pServerItem = new tagServerItem();
	if (pServerItem == NULL)
	{
		////ASSERT(FALSE);
		return false;
	}

	//设置房间
	pServerItem->m_dwSocketID = dwSocketID;
	pServerItem->m_wUserTable = 0;
	pServerItem->m_GameServer = GameServer;

	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtai **** InsertServerItem dwSocketID = %d"), dwSocketID);
	OutputDebugString(szInfo);

	//设置索引
	m_mapServerID[GameServer.wServerID] = pServerItem;

	return true;
}



//让AI坐桌
void CAIManager::SendAIToGameServer(CMD_SS_G_PersonalRoomCreateSuccess * pPersonalRoomCreateSuccess, DWORD dwSocketID)
{
	DWORD	dwGroupID = pPersonalRoomCreateSuccess->dwGroupID;					//俱乐部ID
	WORD	wKindID = pPersonalRoomCreateSuccess->wKindID;						//游戏标识
	WORD	wServerID = pPersonalRoomCreateSuccess->wServerID;					//房间标识
	WORD	wTableID = pPersonalRoomCreateSuccess->wTableID;					//桌子标识
	WORD	wChairCount = pPersonalRoomCreateSuccess->wChairCount;				//椅子总数
	TCHAR szInfo[260] = { 0 };

	wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer dwGroupID = %d  wKindID = %d wServerID = %d wTableID = %d wChairCount = %d"), dwGroupID, wKindID, wServerID, wTableID, wChairCount);
	OutputDebugString(szInfo);



	//寻找俱乐部AI参数
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer dwGroupID = %d 找不到俱乐部返回false"), dwGroupID);
		OutputDebugString(szInfo);
		return;
	}

	//寻找游戏AI参数
	WORD wAIKindID = 0;
	CKindAIUser * pKindAIUser = NULL;
	if (pGroupKindAI->Lookup(wKindID, pKindAIUser) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer dwGroupID = %d 找不游戏返回false"), dwGroupID);
		OutputDebugString(szInfo);
		return;
	}


	//遍历俱乐部下某种游戏里面的AI
	DWORD wAIUser = 0;
	tagAIUserInfo * pAIUserInfo = NULL;
	POSITION PositionAIUser = pGroupKindAI->GetStartPosition();

	//进入桌子机器人的数目
	WORD wPlayingAICount = 2;
	if (wChairCount > 2)
	{
		wPlayingAICount = rand() % (wChairCount - 2) + 2;

	}

	wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer 预备进入的机器人数量  wPlayingAICount = %d"), wPlayingAICount);
	OutputDebugString(szInfo);
	
	int nStatic = 0;
	while (PositionAIUser != NULL)
	{
		pKindAIUser->GetNextAssoc(PositionAIUser, wAIUser, pAIUserInfo);

		if (pAIUserInfo->cbStatus != 0 || pAIUserInfo->cbIsLeave != 0)
		{
			wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer 预备进入的机器人数量  dwUserID = %d 使用中 cbStatus = %d  cbIsLeave = %d"), pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbIsLeave);
			OutputDebugString(szInfo);
			continue;
		}

		//根据权重判断当前AI是否可以进入房间
		DWORD dwTimeCount = GetAIWaitTimeCount(pAIUserInfo->tTime)/60;
		DWORD dwWeight = GetAIWeightIn(dwTimeCount);
		if (rand() % 100 > dwWeight)
		{
			wsprintf(szInfo, TEXT("ptdtai *** SendAIToGameServer 预备进入的机器人数量  dwWeight = %d dwTimeCount = %d"), dwWeight, dwTimeCount);
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

//让一个AI坐桌
void CAIManager::SendOneAIToGameServer(tagAISitFailure * pAISitFailure, DWORD dwSocketID)
{
	DWORD	dwGroupID = pAISitFailure->dwGroupID;					//俱乐部ID
	WORD	wKindID = pAISitFailure->wKindID;						//游戏标识
	WORD	wServerID = pAISitFailure->wServerID;					//房间标识
	WORD	wTableID = pAISitFailure->wTableID;					//桌子标识

	TCHAR szInfo[260] = { 0 };

	wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer dwGroupID = %d  wKindID = %d wServerID = %d wTableID = %d "), dwGroupID, wKindID, wServerID, wTableID);
	OutputDebugString(szInfo);

	WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);



	//寻找俱乐部AI参数
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer dwGroupID = %d 找不到俱乐部返回false"), dwGroupID);
		OutputDebugString(szInfo);
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
		return;
	}

	//寻找游戏AI参数
	WORD wAIKindID = 0;
	CKindAIUser * pKindAIUser = NULL;
	if (pGroupKindAI->Lookup(wKindID, pKindAIUser) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer wKindID = %d 找不游戏返回false"), dwGroupID);
		OutputDebugString(szInfo);
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
		return;
	}


	//遍历俱乐部下某种游戏里面的AI
	DWORD wAIUser = 0;
	tagAIUserInfo * pAIUserInfo = NULL;
	POSITION PositionAIUser = pGroupKindAI->GetStartPosition();


	int nStatic = 0;
	while (PositionAIUser != NULL)
	{
		pKindAIUser->GetNextAssoc(PositionAIUser, wAIUser, pAIUserInfo);

		wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer   dwUserID = %d  cbStatus = %d  cbIsLeave = %d"), pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbStatus);
		OutputDebugString(szInfo);

		//WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

		if (pAIUserInfo->cbStatus != 0 || pAIUserInfo->cbIsLeave != 0)
		{
			wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer   dwUserID = %d 使用中 cbStatus = %d  cbIsLeave = %d"), pAIUserInfo->dwUserID, pAIUserInfo->cbStatus, pAIUserInfo->cbStatus);
			OutputDebugString(szInfo);
			//WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
			continue;
		}

		//如果玩家身上的钱小于进入限制
		if (pAIUserInfo->lScore < pAISitFailure->lEnterLimiteScore)
		{
			wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer   dwUserID = %d  score = %I64d  lEnterLimiteScore = %I64d"), pAIUserInfo->dwUserID, pAIUserInfo->lScore, pAISitFailure->lEnterLimiteScore);
			OutputDebugString(szInfo);
			//WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

			continue;
		}

		//根据权重判断当前AI是否可以进入房间
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

		wsprintf(szInfo, TEXT("ptdtai *** SendOneAIToGameServer  dwGroupID = %d wKindID = %d userid = %d wTableID = %d 发送登录"), dwGroupID, wKindID, pAIUserInfo->dwUserID, wTableID);
		OutputDebugString(szInfo);

		WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
		break;
	}
}


//更新AI状态
bool CAIManager::UpdateAIStatusInfo(tagAIStatusInfo * pAIStatusInfo)
{
	DWORD	dwGroupID = pAIStatusInfo->dwGroupID;			//俱乐部ID
	WORD	wKindID = pAIStatusInfo->wKindID;				//游戏标识
	DWORD	dwUserID = pAIStatusInfo->dwUserID;				//玩家ID

	//寻找俱乐部AI参数
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		return false;
	}

	//寻找游戏AI参数
	WORD wAIKindID = 0;
	CKindAIUser * pKindAIUser = NULL;
	if (pGroupKindAI->Lookup(wKindID, pKindAIUser) == FALSE)
	{
		return false;
	}


	//寻找AI
	DWORD wAIUser = 0;
	tagAIUserInfo * pAIUserInfo = NULL;
	if (pKindAIUser->Lookup(pAIStatusInfo->dwUserID, pAIUserInfo) == FALSE)
	{
		return false;
	}

	//更新状态和勋章
	pAIUserInfo->cbStatus = pAIStatusInfo->cbStatus;
	pAIUserInfo->lScore = pAIStatusInfo->lScore;

	//如果AI退出房间
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

//更新桌子数量
bool CAIManager::UpdateAITableChange(CMD_SS_G_AI_Update_Table_Count * pTableChange)
{
	DWORD	dwGroupID = pTableChange->dwGroupID;			//俱乐部ID
	WORD	wKindID = pTableChange->wKindID;				//游戏标识
	DWORD	wServerID = pTableChange->wServerID;				//玩家ID

	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange dwGroupID = %d wKindID = %d wServerID = %d nTableChange = %d"), dwGroupID, wKindID, wServerID, pTableChange->nTableChange);
	OutputDebugString(szInfo);
	WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

	//寻找房间
	tagServerItem * pServerItem = NULL;
	if (m_mapServerID.Lookup(wServerID, pServerItem))
	{

		pServerItem->m_wUserTable += pTableChange->nTableChange;

		int nGroupTableCount = 0;
		if (!pServerItem->m_GroupTable.Lookup(dwGroupID, nGroupTableCount))
		{
			pServerItem->m_GroupTable[dwGroupID] = 0;

			wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange pServerItem->m_GroupTable[dwGroupID] 000 不存在置零  dwGroupID = %d wKindID = %d wServerID = %d count = %d  pServerItem->m_GroupTable[dwGroupID] = %d"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID], pServerItem->m_GroupTable[dwGroupID]);
			OutputDebugString(szInfo);
			WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

			
		}
		pServerItem->m_GroupTable[dwGroupID] += pTableChange->nTableChange;;

		if (pServerItem->m_wUserTable == INVALID_TABLE || pServerItem->m_wUserTable >= pServerItem->m_GameServer.wTableCount)
		{
			pServerItem->m_wUserTable = 0;
		}

		if (pServerItem->m_GroupTable[dwGroupID] < 0)
		{
			pServerItem->m_GroupTable[dwGroupID] = 0;

			wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange pServerItem->m_GroupTable[dwGroupID] 不存在置零  dwGroupID = %d wKindID = %d wServerID = %d count = %d  pServerItem->m_GroupTable[dwGroupID] = %d"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID], pServerItem->m_GroupTable[dwGroupID]);
			OutputDebugString(szInfo);
			WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
		}

		int nCount = 0;
		if (!m_mapGroupTable.Lookup(dwGroupID, nCount))
		{
			m_mapGroupTable[dwGroupID] = 0;

			wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange m_mapGroupTable[dwGroupID] 不存在置零  dwGroupID = %d wKindID = %d wServerID = %d count = %d  pServerItem->m_GroupTable[dwGroupID] = %d"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID], pServerItem->m_GroupTable[dwGroupID]);
			OutputDebugString(szInfo);
			WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
		}
		m_mapGroupTable[dwGroupID] += pTableChange->nTableChange;

		wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange dwGroupID = %d wKindID = %d wServerID = %d count = %d  pServerItem->m_GroupTable[dwGroupID] = %d"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID], pServerItem->m_GroupTable[dwGroupID]);
		OutputDebugString(szInfo);
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);


		if (m_mapGroupTable[dwGroupID] < 0)
		{
			m_mapGroupTable[dwGroupID] = 0;
		}

		//如果桌子没有了自动创建
		if (m_mapGroupTable[dwGroupID] == 0)
		{
			wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange dwGroupID = %d wKindID = %d wServerID = %d count = %d 检测为 0 自动创建房间"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID]);
			OutputDebugString(szInfo);
			WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

			CreateRoom(wServerID, pServerItem->m_GameServer.dwSurportType, pTableChange->CreateTable);
		}




		return true;
	}

	return false;
}


//设置离开大联盟权重配置
void CAIManager::SetWeightOutOption(WeightOutOption wghtOutOption)
{
	memcpy_s(&m_WeightOutOption, sizeof(m_WeightOutOption), &wghtOutOption, sizeof(wghtOutOption));
}

//设置进入游戏权重配置
void CAIManager::SetWeightInOption(WeightInOption wghtInOption)
{
	memcpy_s(&m_WeightInOption, sizeof(m_WeightInOption), &wghtInOption, sizeof(wghtInOption));
}

//设置AI总数时间变换
bool CAIManager::SetAITimeChange(TimeChange * pTimeChange)
{
	memcpy_s(&m_TimeChange, sizeof(m_TimeChange), pTimeChange, sizeof(m_TimeChange));
	return true;
}


//设置AI总数时间变换
bool CAIManager::SetAITimeChange(int nIndex, int nWeight)
{
	if (nIndex < 0 || nIndex > 24)
	{
		return false;
	}
	m_TimeChange.dwTimeChange[nIndex] = nWeight;
	return true;
}



//获取进入权重
DWORD CAIManager::GetAIWeightIn(DWORD dwTimeCount)
{
	//权重位置
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

//获取离开权重
DWORD CAIManager::GetAIWeightOut(DWORD dwTimeCount)
{
	//权重位置
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

//获取当前时间段AI的总数量
DWORD CAIManager::GetTimeAICount(DWORD dwTimeCount)
{
	if (dwTimeCount >= 0 && dwTimeCount < 24)
	{
		return  m_TimeChange.dwTimeChange[dwTimeCount];
	}

	return 0;
}


//时间计算
DWORD CAIManager::GetAIWaitTimeCount(time_t tTime)
{
	//获取当前时间
	time_t curTime;
	time(&curTime);

	//获取时间差
	DWORD dwTime = (DWORD)difftime(curTime, tTime);

	return dwTime;
}


//AI 进入或离开
DWORD CAIManager::AIInOrOut(DWORD dwGroupSocketID)
{
	TCHAR szInfo[260] = {0};
	//获取当前时间段需要进入大联盟的AI总数
	SYSTEMTIME	sysTime;
	GetLocalTime(&sysTime);
	DWORD dwNeedAICount = GetTimeAICount(sysTime.wHour);

	wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut dwNeedAICount = %d 需要离开的人数"), dwNeedAICount);
	OutputDebugString(szInfo);

	//遍历所有俱乐部AI参数
	DWORD dwGroupID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	POSITION Position = m_mapGroupAIUser.GetStartPosition();

	//遍历所有俱乐部
	while (Position != NULL)
	{
		m_mapGroupAIUser.GetNextAssoc(Position, dwGroupID, pGroupKindAI);
		if (pGroupKindAI != NULL)
		{
			//遍历俱乐部下所有游戏
			WORD wAIKindID = 0;
			CKindAIUser * pKindAIUser = NULL;
			POSITION PositionKind = pGroupKindAI->GetStartPosition();

			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut dwNeedAICount = %d  dwGroupID = %d 需要离开的人数"), dwNeedAICount, dwGroupID);
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
						//随机让 1 到 5 个玩家离开
						DWORD dwRandLeave = rand() % (pKindAIUser->GetCount() - dwNeedAICount) + 1;

						wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut dwRandLeave = %d 随机离开的人数"), dwRandLeave);
						OutputDebugString(szInfo);


						//遍历俱乐部下某种游戏里面的AI
						DWORD dwAIUser = 0;
						tagAIUserInfo * pAIUserInfo = NULL;
						POSITION PositionAIUser = pGroupKindAI->GetStartPosition();

						DBR_GR_UnLockAI UnLockAI;
						UnLockAI.dwAICount = dwRandLeave;
						UnLockAI.dwGroupID = dwGroupID;
						UnLockAI.wKindID = wAIKindID;
						
						//筛选出离开的玩家
						int nStatic = 0;
						while (PositionAIUser != NULL)
						{
							pKindAIUser->GetNextAssoc(PositionAIUser, dwAIUser, pAIUserInfo);

							//删除不在游戏中的玩家
							if (pAIUserInfo->cbStatus != 0)
							{
								continue;
							}

							//判断当前玩家的离开权重
							DWORD dwOnlineCount = GetAIWaitTimeCount(pAIUserInfo->tEnterTime)%60;
							DWORD dwWeight = GetAIWeightOut(dwOnlineCount);

							if (rand() % 100 >= dwWeight)
							{
								continue;
							}

							wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut UserID = %d 离开房间"), pAIUserInfo->dwUserID);
							OutputDebugString(szInfo);

							UnLockAI.dwUserID[nStatic] = pAIUserInfo->dwUserID;

							//删除玩家
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
							//改变数据库中的状态
							m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_UNLOCK_AI_USER, DBR_GR_UNLOCK_AI_USER, 0, &UnLockAI, sizeof(UnLockAI));


						}
					}
					else
					{
						//随机让 1 到 5 个玩家离开
						if (dwNeedAICount > pKindAIUser->GetCount())
						{
							DWORD dwRandLoad = rand() % (dwNeedAICount - pKindAIUser->GetCount()) + 1;

							//锁定AI
							DBR_GR_LoadSingleAI LoadSingleAI;
							LoadSingleAI.dwGroupID = dwGroupID;
							LoadSingleAI.wKindID = wAIKindID;
							LoadSingleAI.dwAICount = dwRandLoad;

							wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** AIInOrOut 新增机器人数量 dwRandLoad = %d "), dwRandLoad);
							OutputDebugString(szInfo);
							//改变数据库中的状态
							m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_LOAD_SINGLE_AI, DBR_GR_LOAD_SINGLE_AI, 0, &LoadSingleAI, sizeof(DBR_GR_LoadSingleAI));
						}
					}
					
				}
			}
		}
	}

	return 0;
}

//统计大联盟的总人数
DWORD CAIManager::GetAICount()
{
	//AI总数
	DWORD dwAICount = 0;

	//遍历所有俱乐部AI参数
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	POSITION Position = m_mapGroupAIUser.GetStartPosition();

	//遍历所有俱乐部
	while (Position != NULL)
	{
		m_mapGroupAIUser.GetNextAssoc(Position, dwAIID, pGroupKindAI);
		if (pGroupKindAI != NULL)
		{
			//遍历俱乐部下所有游戏
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

//创建房间
DWORD CAIManager::CreateRoom(WORD wServerID, WORD wPlayMode, CMD_GR_AICreateTable CreateTable)
{
	OutputDebugString(TEXT("ptdtai *** CreateRoom AI 创建房间"));
	//遍历俱乐部下某种游戏里面的AI
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
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom  寻找房间 dwAIServerID = %d"), wAIServerID);
			OutputDebugString(szInfo);
			continue;
		}
		tagAIUserInfo * pAIUserInfo = GetSingleAICount(CreateTable.dwGroupID, pServerItem->m_GameServer.wKindID);
		if (!pAIUserInfo)
		{
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom 找不到玩家 dwGroupID = %d wKindID = %d"), CreateTable.dwGroupID, pServerItem->m_GameServer.wKindID);
			OutputDebugString(szInfo);
			continue;
		}

		if (pServerItem->m_dwSocketID == 0)
		{
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** 房间不存在"));
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
			
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom AI 创建房间 发送消息 dwSocketID = %d"), pServerItem->m_dwSocketID);
			OutputDebugString(szInfo);
		
			m_pITCPNetworkEngine->SendData(pServerItem->m_dwSocketID, MDM_CS_S_AI_ACTION, SUB_GR_AI_CREATE_TABLE, &CreateTable, sizeof(CreateTable));
			break;
		}

	}


	return 0;
}


//让AI坐桌
tagAIUserInfo * CAIManager::GetSingleAICount(DWORD dwGroupID, WORD wKindID)
{
	TCHAR szInfo[260] = { 0 };

	//寻找俱乐部AI参数
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom 找不到该俱乐部下的游戏 dwGroupID = %d wKindID = %d"), dwGroupID, wKindID);
		OutputDebugString(szInfo);
		return NULL;
	}

	//寻找游戏AI参数
	WORD wAIKindID = 0;
	CKindAIUser * pKindAIUser = NULL;
	if (pGroupKindAI->Lookup(wKindID, pKindAIUser) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom 找不到该俱乐部下的游戏下的玩家 dwGroupID = %d wKindID = %d"), dwGroupID, wKindID);
		OutputDebugString(szInfo);

		return NULL;
	}


	//遍历俱乐部下某种游戏里面的AI
	DWORD wAIUser = 0;
	tagAIUserInfo * pAIUserInfo = NULL;
	POSITION PositionAIUser = pGroupKindAI->GetStartPosition();


	int nStatic = 0;
	while (PositionAIUser != NULL)
	{
		pKindAIUser->GetNextAssoc(PositionAIUser, wAIUser, pAIUserInfo);

		if (pAIUserInfo->cbStatus != 0)
		{
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** CreateRoom 找不到该俱乐部下的游戏下的玩家 dwGroupID = %d wKindID = %d userid = %d 已经被使用"), dwGroupID, wKindID, pAIUserInfo->dwUserID);
			OutputDebugString(szInfo);
			continue;
		}

		return pAIUserInfo;
	}

	return NULL;
}

//获取单个AI
bool CAIManager::DeleteServer(DWORD dwSocketID)
{
	//遍历俱乐部下某种游戏里面的AI
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
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** 删除房间 dwSocketID = %d  wAIServerID = %d GetCount = %d"), pServerItem->m_dwSocketID, wAIServerID, m_mapGroupTable.GetCount());
			OutputDebugString(szInfo);
			pServerItem->m_dwSocketID = 0;
			m_mapServerID.RemoveKey(wAIServerID);

			DWORD dwGroupID = 0;
			int nTalbeCount = 0;

			POSITION PositionGroupTable = m_mapGroupTable.GetStartPosition();
			while (PositionGroupTable != NULL)
			{
				m_mapGroupTable.GetNextAssoc(PositionGroupTable, dwGroupID, nTalbeCount);

				wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** 删除房间 dwGroupID = %d nTalbeCount = %d"), dwGroupID, nTalbeCount);
				OutputDebugString(szInfo);

				if (dwGroupID > 0)
				{
					m_mapGroupTable[dwGroupID] -= pServerItem->m_GroupTable[dwGroupID];

					wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** 删除房间 dwGroupID = %d m_mapGroupTable[dwGroupID] = %d   pServerItem->m_GroupTable[dwGroupID]"), dwGroupID, m_mapGroupTable[dwGroupID], pServerItem->m_GroupTable[dwGroupID]);
					OutputDebugString(szInfo);

					if (m_mapGroupTable[dwGroupID] <= 0)
					{
						wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** 删除房间 dwSocketID = %d  wAIServerID = %d dwGroupID = %d 桌子为空 等待创建"), pServerItem->m_dwSocketID, wAIServerID, dwGroupID);
						OutputDebugString(szInfo);
						m_mapGroupTable[dwGroupID] = 0;
					}
				}
			}
			
			//恢复服务器玩家状态
			ResetUserStatus(pServerItem->m_GameServer.wServerID);
	
			break;
		}

	}
	return true;
}

//统计当前俱乐部使用桌子的数量
bool CAIManager::GroupCreateRoomCount(DWORD dwGroupID, WORD wKindID)
{
	return true;
}

//设置离开大联盟权重配置
void CAIManager::SetWeightOutOption(int nIndex, tagWeightOutRatio WeightOutRatio)
{
	if (nIndex < 1 || nIndex > 10)
	{
		return;
	}
	m_WeightOutOption.WeightOutRatio[nIndex] = WeightOutRatio;
}

//设置进入游戏权重配置
void CAIManager::SetWeightInOption(int nIndex, tagWeightInRatio WeightInRatio)
{
	if (nIndex < 1 || nIndex > 10)
	{
		return;
	}
	m_WeightInOption.WeightInRatio[nIndex - 1] = WeightInRatio;
}

// 插入玩法
bool CAIManager::InsertGroupGameRule(tagGroupConfigInfo GroupConfigInfo)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule 增加玩法 count = %d configid = %d"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
	OutputDebugString(szInfo);

	//判断玩法是否存在
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
		wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule 增加玩法 count = %d configid = %d 成功"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
		OutputDebugString(szInfo);

		m_ArrayGroupGameRuleTable.Add(GroupConfigInfo);
	}
	else
	{
		wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule 增加玩法 count = %d configid = %d 玩法已经存在"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
		OutputDebugString(szInfo);
	}

	return true;
}


// 删除玩法
bool CAIManager::DeleteGroupGameRule(tagGroupConfigInfo GroupConfigInfo)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule 删除玩法 count = %d configid = %d"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
	OutputDebugString(szInfo);
	for (int i = 0; i < m_ArrayGroupGameRuleTable.GetCount(); i++)
	{
		if (m_ArrayGroupGameRuleTable[i].dwConfigID == GroupConfigInfo.dwConfigID)
		{
			wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule 删除玩法 count = %d configid = %d 成功"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
			OutputDebugString(szInfo);

			m_ArrayGroupGameRuleTable.RemoveAt(i);
			break;
		}
	}

	return true;
}

// 修改玩法
bool CAIManager::ModifyGroupGameRule(tagGroupConfigInfo GroupConfigInfo)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule 修改玩法 count = %d configid = %d"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
	OutputDebugString(szInfo);
	for (int i = 0; i < m_ArrayGroupGameRuleTable.GetCount(); i++)
	{
		if (m_ArrayGroupGameRuleTable[i].dwConfigID == GroupConfigInfo.dwConfigID)
		{
			wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule 修改玩法 count = %d configid = %d 成功"), m_ArrayGroupGameRuleTable.GetCount(), GroupConfigInfo.dwConfigID);
			OutputDebugString(szInfo);
			memcpy_s(&m_ArrayGroupGameRuleTable[i], sizeof(m_ArrayGroupGameRuleTable[i]), &GroupConfigInfo, sizeof(GroupConfigInfo));
			break;
		}
	}

	return true;
}

//检测桌子是否为空
bool CAIManager::CheckGroupRoomIsNull()
{
	//日志输出信息
	TCHAR szInfo[260] = { 0 };

	WORD	wKindID = 0;				//游戏标识
	DWORD	wServerID = 0;				//玩家ID
		
	//遍历所有俱乐部AI参数
	DWORD dwGroupID = 0;
	CKindAIParameter * pKindAIParameter = NULL;
	POSITION Position = m_mapGroupAIParameter.GetStartPosition();

	struct tagAreadyCheckKind
	{
		DWORD dwGroupID;
		WORD  wKindID;
	};

	CWHArray<tagAreadyCheckKind> arrayKindID;

	//遍历所有俱乐部
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

				//如果俱乐部下的桌子总数为0
				if (m_mapGroupTable[dwGroupID] == 0)
				{
					//遍历该俱乐部下的所有玩法
					for (int i = 0; i < m_ArrayGroupGameRuleTable.GetCount(); i++)
					{
						wsprintf(szInfo, TEXT("ptdtai *** CheckGroupRoomIsNull dwGroupID = %d tableCount = %d kindid = %d configid = %d"), dwGroupID, m_mapGroupTable[dwGroupID], 
							m_ArrayGroupGameRuleTable[i].wKindID, m_ArrayGroupGameRuleTable[i].dwConfigID);
						OutputDebugString(szInfo);

						//如果当前类型是已经分配过的，则跳过
						//bool bIsCheck = false;
						//for (int k = 0; k < arrayKindID.GetCount(); k++)
						//{
						//	//WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("元素 dwGroupID = %d  wKindID = %d"), arrayKindID[k].dwGroupID, arrayKindID[k].wKindID);
						//	if (arrayKindID[k].wKindID == m_ArrayGroupGameRuleTable[i].wKindID &&  arrayKindID[k].dwGroupID == m_ArrayGroupGameRuleTable[i].dwGroupID)
						//	{
						//		bIsCheck = true;

						//		//WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("相同类型 dwGroupID = %d  wKindID = %d"), arrayKindID[k].dwGroupID, arrayKindID[k].wKindID);
						//		break;
						//	}
						//}

						//if (bIsCheck)
						//{
						//	continue;
						//}

						//遍历所有服务器，选择适合游戏类型的服务器
						WORD wServerID = 0;
						tagServerItem * pServerItem = NULL;
						POSITION PositionServer = m_mapServerID.GetStartPosition();

						//遍历所有服务器
						while (PositionServer != NULL)
						{
							m_mapServerID.GetNextAssoc(PositionServer, wServerID, pServerItem);

							//如果是积分模式 不创建
							if (pServerItem && pServerItem->m_GameServer.dwSurportType == 0)
							{
								continue;
							}

							wsprintf(szInfo, TEXT("ptdtai *** CheckGroupRoomIsNull dwGroupID = %d tableCount = %d kindid = %d configid = %d serverid = %d serverkind = %d"), dwGroupID, m_mapGroupTable[dwGroupID],
								m_ArrayGroupGameRuleTable[i].wKindID, m_ArrayGroupGameRuleTable[i].dwConfigID, wServerID, pServerItem->m_GameServer.wKindID);
							OutputDebugString(szInfo);

							//WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

							//选择适合游戏类型的服务器
							if (pServerItem && pServerItem->m_GameServer.wKindID == m_ArrayGroupGameRuleTable[i].wKindID && m_ArrayGroupGameRuleTable[i].dwGroupID == dwGroupID)
							{
								tagAreadyCheckKind AreadyCheckKind;
								AreadyCheckKind.dwGroupID = m_ArrayGroupGameRuleTable[i].dwGroupID;
								AreadyCheckKind.wKindID = m_ArrayGroupGameRuleTable[i].wKindID;
								arrayKindID.Add(AreadyCheckKind);

								//创建桌子
								CMD_GR_AICreateTable CreateTable;

								CreateTable.lCellScore = m_ArrayGroupGameRuleTable[i].lCellScore;							    //底分设置
								CreateTable.dwDrawCountLimit = m_ArrayGroupGameRuleTable[i].dwDrawCountLimit;					//局数限制
								CreateTable.dwDrawTimeLimit = m_ArrayGroupGameRuleTable[i].dwDrawTimeLimit;					//时间限制
								CreateTable.wJoinGamePeopleCount = m_ArrayGroupGameRuleTable[i].wJoinGamePeopleCount;			//参与游戏的人数
								CreateTable.dwRoomTax = m_ArrayGroupGameRuleTable[i].dwRoomTax;								//单独一个私人房间的税率，千分比
								CreateTable.cbPassword = m_ArrayGroupGameRuleTable[i].cbPassword;								//密码设置 0:无密码 1:需要密码
								CreateTable.cbPayMode = m_ArrayGroupGameRuleTable[i].cbPayMode;								//支付方式									
								memcpy_s(CreateTable.cbGameRule, RULE_LEN, m_ArrayGroupGameRuleTable[i].cbGameRule, RULE_LEN);
								CreateTable.cbGameMode = m_ArrayGroupGameRuleTable[i].cbGameMode;								//游戏模式
								CreateTable.lEnterScore = m_ArrayGroupGameRuleTable[i].lEnterScore;							//进入房间的分数下限
								CreateTable.lLeaveScore = m_ArrayGroupGameRuleTable[i].lLeaveScore;							//离开房间分数下限
								CreateTable.cbJoinMode = m_ArrayGroupGameRuleTable[i].cbJoinMode;								//玩家参与方式	0 不允许陌生人参与	1 允许陌生人参与	
								CreateTable.dwGroupID = m_ArrayGroupGameRuleTable[i].dwGroupID;								//俱乐部id， 非俱乐部模式 填 0
								CreateTable.dwConfigID = m_ArrayGroupGameRuleTable[i].dwConfigID;								//玩法ID

								wsprintf(szInfo, TEXT("ptdtai *** CheckGroupRoomIsNull dwGroupID = %d wKindID = %d wServerID = %d count = %d 检测为 0 自动创建房间"), dwGroupID, wKindID, wServerID, m_mapGroupTable[dwGroupID]);
								OutputDebugString(szInfo);
								WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
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


//让玩家离开
bool CAIManager::SetUserLeave(CMD_CS_C_DeleteAI DeleteAI)
{
	TCHAR szInfo[260] = { 0 };

	//寻找俱乐部AI参数
	DWORD dwGroupID = DeleteAI.dwGroupID;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		wsprintf(szInfo, TEXT("ptdtai ****  SetUserLeave 俱乐部不存在 dwGroupID = %d "), dwGroupID);
		OutputDebugString(szInfo);
		return false;
	}


	//
	for (int i = 0; i < 20; i++)
	{
		//寻找游戏AI参数
		WORD wAIKindID = 0;
		CKindAIUser * pKindAIUser = NULL;
		if (DeleteAI.DeleteAI[i].wKindID == 0)
		{
			//真实玩家
		}
		else
		{
			if (pGroupKindAI->Lookup(DeleteAI.DeleteAI[i].wKindID, pKindAIUser) == FALSE)
			{
				wsprintf(szInfo, TEXT("ptdtai **** SetUserLeave 找不到该俱乐部下的游戏下的玩家 dwGroupID = %d wKindID = %d"), dwGroupID, DeleteAI.DeleteAI[i].wKindID);
				OutputDebugString(szInfo);
				continue;
			}

			if (DeleteAI.DeleteAI[i].dwUserID != 0)
			{
				tagAIUserInfo * pAIUserInfo = NULL;
				pKindAIUser->Lookup(DeleteAI.DeleteAI[i].dwUserID, pAIUserInfo);

				if (pAIUserInfo)
				{
					if (DeleteAI.cbOperate == 0)//离线
					{
						pAIUserInfo->cbIsLeave = 1;

						tagDeleteGroupUser DeleteGroupUser;
						DeleteGroupUser.dwGroupID = dwGroupID;
						DeleteGroupUser.dwUserID = pAIUserInfo->dwUserID;
						DeleteGroupUser.wKindID = DeleteAI.DeleteAI[i].wKindID;
						DeleteGroupUser.cbOperateID = 1;

						wsprintf(szInfo, TEXT("ptdtai **** SetUserLeave 离线 dwGroupID = %d wKindID = %d userid = %d 加入待操作列表"), dwGroupID, DeleteAI.DeleteAI[i].wKindID, DeleteAI.DeleteAI[i].dwUserID);
						OutputDebugString(szInfo);

						m_ArrayDeleteGroupUser.Add(DeleteGroupUser);
					
					}
					else if (DeleteAI.cbOperate == 1)//退出大联盟
					{
						pAIUserInfo->cbIsLeave = 2;

						tagDeleteGroupUser DeleteGroupUser;
						DeleteGroupUser.dwGroupID = dwGroupID;
						DeleteGroupUser.dwUserID = pAIUserInfo->dwUserID;
						DeleteGroupUser.wKindID = DeleteAI.DeleteAI[i].wKindID;
						DeleteGroupUser.cbOperateID = 2;


						m_ArrayDeleteGroupUser.Add(DeleteGroupUser);

						wsprintf(szInfo, TEXT("ptdtai **** SetUserLeave 退出大联盟 dwGroupID = %d wKindID = %d userid = %d 加入待操作列表"), dwGroupID, DeleteAI.DeleteAI[i].wKindID, DeleteAI.DeleteAI[i].dwUserID);
						OutputDebugString(szInfo);

					}

				}
			}
		}
		
	}

	return true;
}


//让玩家离开
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

			wsprintf(szInfo, TEXT("ptdtai **** LetUserLeave 离线 dwGroupID = %d wKindID = %d userid = %d dwAICount = %d 执行操作"), m_ArrayDeleteGroupUser[i].dwGroupID, m_ArrayDeleteGroupUser[i].wKindID, m_ArrayDeleteGroupUser[i].dwUserID, UnLockAI.dwAICount);
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


			wsprintf(szInfo, TEXT("ptdtai **** LetUserLeave 退出大联盟 dwGroupID = %d wKindID = %d userid = %d dwAICount = %d 执行操作"), m_ArrayDeleteGroupUser[i].dwGroupID, m_ArrayDeleteGroupUser[i].wKindID, m_ArrayDeleteGroupUser[i].dwUserID, AIQuitGroup.dwAICount);
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


//重置玩家状态
bool CAIManager::ResetUserStatus(WORD wServerID)
{
	TCHAR szInfo[260] = { 0 };
	//获取当前时间段需要进入大联盟的AI总数


	//遍历所有俱乐部AI参数
	DWORD dwGroupID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	POSITION Position = m_mapGroupAIUser.GetStartPosition();

	//遍历所有俱乐部
	while (Position != NULL)
	{
		m_mapGroupAIUser.GetNextAssoc(Position, dwGroupID, pGroupKindAI);
		if (pGroupKindAI != NULL)
		{
			//遍历俱乐部下所有游戏
			WORD wAIKindID = 0;
			CKindAIUser * pKindAIUser = NULL;
			POSITION PositionKind = pGroupKindAI->GetStartPosition();

			while (PositionKind != NULL)
			{
				pGroupKindAI->GetNextAssoc(PositionKind, wAIKindID, pKindAIUser);
				if (pKindAIUser != NULL)
				{

					//遍历俱乐部下某种游戏里面的AI
					DWORD dwAIUser = 0;
					tagAIUserInfo * pAIUserInfo = NULL;
					POSITION PositionAIUser = pGroupKindAI->GetStartPosition();

					//筛选出离开的玩家
					int nStatic = 0;
					while (PositionAIUser != NULL)
					{
						pKindAIUser->GetNextAssoc(PositionAIUser, dwAIUser, pAIUserInfo);

						//删除不在游戏中的玩家
						if (pAIUserInfo->wServerID == wServerID)
						{
							pAIUserInfo->cbStatus = 0;

							wsprintf(szInfo, TEXT("ptdtai **** ResetUserStatus  wServerID = %d wKindID = %d userid = %d 恢复状态"), wServerID, wAIKindID, pAIUserInfo->dwUserID);
							OutputDebugString(szInfo);
						}
					}
				}
			}
		}
	}

	return true;
}


//更新AI分数
bool CAIManager::UpdateAIScore(tagAIStatusInfo * pAIStatusInfo)
{
	DWORD	dwGroupID = pAIStatusInfo->dwGroupID;			//俱乐部ID
	WORD	wKindID = pAIStatusInfo->wKindID;				//游戏标识
	DWORD	dwUserID = pAIStatusInfo->dwUserID;				//玩家ID

	//寻找俱乐部AI参数
	DWORD dwAIID = 0;
	CGroupKindAI * pGroupKindAI = NULL;
	if (m_mapGroupAIUser.Lookup(dwGroupID, pGroupKindAI) == FALSE)
	{
		return false;
	}

	//遍历俱乐部下所有游戏
	WORD wAIKindID = 0;
	CKindAIUser * pKindAIUser = NULL;
	POSITION PositionKind = pGroupKindAI->GetStartPosition();

	while (PositionKind != NULL)
	{
		pGroupKindAI->GetNextAssoc(PositionKind, wAIKindID, pKindAIUser);
		if (pKindAIUser != NULL)
		{

			//遍历俱乐部下某种游戏里面的AI
			DWORD dwAIUser = 0;
			tagAIUserInfo * pAIUserInfo = NULL;
			POSITION PositionAIUser = pGroupKindAI->GetStartPosition();

			//筛选出离开的玩家
			int nStatic = 0;
			while (PositionAIUser != NULL)
			{
				pKindAIUser->GetNextAssoc(PositionAIUser, dwAIUser, pAIUserInfo);

				//删除不在游戏中的玩家
				if (pAIUserInfo->dwUserID == pAIStatusInfo->dwUserID)
				{
					//更新玩家分数
					pAIUserInfo->lScore = pAIStatusInfo->lScore;

					TCHAR szInfo[260] = { 0 };
					wsprintf(szInfo, TEXT("ptdtai *** UpdateAIStatusInfo  groupid = %d dwUserID = %d lscore = %d"), dwGroupID, pAIUserInfo->dwUserID, pAIUserInfo->lScore);
					OutputDebugString(szInfo);
					WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
				}
			}
		}
	}

	return true;
}



//////////////////////////////////////////////////////////////////////////////////
