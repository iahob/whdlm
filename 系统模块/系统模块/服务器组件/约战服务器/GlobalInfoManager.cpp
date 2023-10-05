//�ر���IT������28xin.com��
#include "StdAfx.h"
#include "GlobalInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalUserItem::CGlobalUserItem()
{
	//�û�����
	m_dwUserID=0L;
	m_dwGameID=0L;
	m_szNickName[0]=0;

	//�ȼ���Ϣ
	m_cbMemberOrder=0;
	m_cbMasterOrder=0;

	//��������
	m_pNextUserItemPtr=NULL;

	return;
}

//��������
CGlobalUserItem::~CGlobalUserItem()
{
}

void CGlobalUserItem::UpdateStatus(const WORD wTableID, const WORD wChairID, const BYTE cbUserStatus)
{
	m_UserInfo.wTableID = wTableID;
	m_UserInfo.wChairID = wChairID;
	m_UserInfo.cbUserStatus = cbUserStatus;
}

//ö�ٷ���
CGlobalServerItem * CGlobalUserItem::EnumServerItem(WORD wIndex)
{
	if (wIndex>=m_GlobalServerItemArray.GetCount()) return NULL;
	return m_GlobalServerItemArray[wIndex];
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalPlazaItem::CGlobalPlazaItem()
{
	//���ñ���
	m_wIndex=0;
	ZeroMemory(&m_GamePlaza,sizeof(m_GamePlaza));

	//��������
	m_pNextPlazaItemPtr=NULL;

	return;
}

//��������
CGlobalPlazaItem::~CGlobalPlazaItem()
{
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalChatItem::CGlobalChatItem()
{
	//���ñ���
	m_wIndex=0;
	ZeroMemory(&m_ChatServer,sizeof(m_ChatServer));

	//��������
	m_pNextChatServerPtr=NULL;

	return;
}

//��������
CGlobalChatItem::~CGlobalChatItem()
{
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalServerItem::CGlobalServerItem()
{
	//���ñ���
	m_wIndex=0;
	ZeroMemory(&m_GameServer,sizeof(m_GameServer));

	//��������
	m_pNextServerItemPtr=NULL;

	//��������
	m_MapUserID.InitHashTable(PRIME_SERVER_USER);

	return;
}

//��������
CGlobalServerItem::~CGlobalServerItem()
{
}

//Ѱ���û�
CGlobalUserItem * CGlobalServerItem::SearchUserItem(DWORD dwUserID)
{
	//�����û�
	CGlobalUserItem * pGlobalUserItem=NULL;
	m_MapUserID.Lookup(dwUserID,pGlobalUserItem);

	return pGlobalUserItem;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalInfoManager::CGlobalInfoManager()
{
	//�洢����
	m_pGlobalUserItem=NULL;
	m_pGlobalPlazaItem=NULL;
	m_pGlobalServerItem=NULL;
	m_pGlobalChatItem = NULL;

	//��������
	m_MapPlazaID.InitHashTable(PRIME_SERVER);
	m_MapServerID.InitHashTable(PRIME_SERVER);
	m_MapUserID.InitHashTable(PRIME_PLATFORM_USER);
	m_MapGameID.InitHashTable(PRIME_PLATFORM_USER);
	m_MapNickName.InitHashTable(PRIME_PLATFORM_USER);
	m_MapChatID.InitHashTable(PRIME_SERVER);
	//Լս��
	m_MapPersonalTableInfo.InitHashTable(MAX_SERVER);
	m_ServerTableCountArray.RemoveAll();
	m_MapGroupRoom.InitHashTable(PRIME_PLATFORM_USER);

	return;
}

//��������
CGlobalInfoManager::~CGlobalInfoManager()
{
	//ɾ���û�
	DWORD dwUserID=0L;
	CGlobalUserItem * pGlobalUserItem=NULL;
	POSITION Position=m_MapUserID.GetStartPosition();
	while (Position!=NULL)
	{
		m_MapUserID.GetNextAssoc(Position,dwUserID,pGlobalUserItem);
		SafeDelete(pGlobalUserItem);
	}
	m_MapUserID.RemoveAll();

	//ɾ���㳡
	WORD wPlazaID=0;
	CGlobalPlazaItem * pGlobalPlazaItem=NULL;
	Position=m_MapPlazaID.GetStartPosition();
	while (Position!=NULL)
	{
		m_MapPlazaID.GetNextAssoc(Position,wPlazaID,pGlobalPlazaItem);
		SafeDelete(pGlobalPlazaItem);
	}
	m_MapPlazaID.RemoveAll();

	//ɾ������
	WORD wServerID=0;
	CGlobalServerItem * pGlobalServerItem=NULL;
	Position=m_MapServerID.GetStartPosition();
	while (Position!=NULL)
	{
		m_MapServerID.GetNextAssoc(Position,wServerID,pGlobalServerItem);
		SafeDelete(pGlobalServerItem);
	}
	m_MapServerID.RemoveAll();

	//ɾ������
	WORD wChatID=0;
	CGlobalChatItem * pGlobalChatItem=NULL;
	Position=m_MapChatID.GetStartPosition();
	while (Position!=NULL)
	{
		m_MapChatID.GetNextAssoc(Position,wChatID,pGlobalChatItem);
		SafeDelete(pGlobalChatItem);
	}
	m_MapChatID.RemoveAll();

	//ɾ���û�
	while (m_pGlobalUserItem!=NULL)
	{
		pGlobalUserItem=m_pGlobalUserItem;
		m_pGlobalUserItem=m_pGlobalUserItem->m_pNextUserItemPtr;
		SafeDelete(pGlobalUserItem);
	}

	//ɾ���㳡
	while (m_pGlobalPlazaItem!=NULL)
	{
		pGlobalPlazaItem=m_pGlobalPlazaItem;
		m_pGlobalPlazaItem=m_pGlobalPlazaItem->m_pNextPlazaItemPtr;
		SafeDelete(pGlobalPlazaItem);
	}

	//ɾ������
	while (m_pGlobalServerItem!=NULL)
	{
		pGlobalServerItem=m_pGlobalServerItem;
		m_pGlobalServerItem=m_pGlobalServerItem->m_pNextServerItemPtr;
		SafeDelete(pGlobalServerItem);
	}

	//ɾ������
	while (m_pGlobalChatItem!=NULL)
	{
		pGlobalChatItem=m_pGlobalChatItem;
		m_pGlobalChatItem=m_pGlobalChatItem->m_pNextChatServerPtr;
		SafeDelete(pGlobalChatItem);
	}

	// ɾ�����ֲ�����
	CRoomIDArray * pRoomIDArray = NULL;
	Position = m_MapGroupRoom.GetStartPosition();
	DWORD dwGroupID = 0;
	while (Position != NULL)
	{
		m_MapGroupRoom.GetNextAssoc(Position, dwGroupID, pRoomIDArray);
		pRoomIDArray->RemoveAll();
		pRoomIDArray->FreeMemory();
	}
	m_MapGroupRoom.RemoveAll();
	return;
}

//��������
VOID CGlobalInfoManager::ResetData()
{
	//ɾ���û�
	DWORD dwUserID=0L;
	CGlobalUserItem * pGlobalUserItem=NULL;
	POSITION Position=m_MapUserID.GetStartPosition();

	//ɾ���û�
	while (Position!=NULL)
	{
		m_MapUserID.GetNextAssoc(Position,dwUserID,pGlobalUserItem);
		if (pGlobalUserItem!=NULL) FreeGlobalUserItem(pGlobalUserItem);
	}

	//ɾ���㳡
	WORD wPlazaID=0;
	CGlobalPlazaItem * pGlobalPlazaItem=NULL;
	Position=m_MapPlazaID.GetStartPosition();

	//ɾ���㳡
	while (Position!=NULL)
	{
		m_MapPlazaID.GetNextAssoc(Position,wPlazaID,pGlobalPlazaItem);
		if (pGlobalPlazaItem!=NULL) FreeGlobalPlazaItem(pGlobalPlazaItem);
	}

	//ɾ������
	WORD wServerID=0;
	CGlobalServerItem * pGlobalServerItem=NULL;
	Position=m_MapServerID.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_MapServerID.GetNextAssoc(Position,wServerID,pGlobalServerItem);
		if (pGlobalServerItem!=NULL) FreeGlobalServerItem(pGlobalServerItem);
	}

	//ɾ������
	WORD wChatID=0;
	CGlobalChatItem * pGlobalChatItem=NULL;
	Position=m_MapChatID.GetStartPosition();

	//ɾ���㳡
	while (Position!=NULL)
	{
		m_MapChatID.GetNextAssoc(Position,wPlazaID,pGlobalChatItem);
		if (pGlobalChatItem!=NULL) FreeGlobalChatItem(pGlobalChatItem);
	}

	//ɾ������
	m_MapUserID.RemoveAll();
	m_MapGameID.RemoveAll();
	m_MapPlazaID.RemoveAll();
	m_MapServerID.RemoveAll();
	m_MapNickName.RemoveAll();
	m_MapChatID.RemoveAll();

	// ɾ�����ֲ�����
	CRoomIDArray * pRoomIDArray = NULL;
	Position = m_MapGroupRoom.GetStartPosition();
	DWORD dwGroupID = 0;
	while (Position != NULL)
	{
		m_MapGroupRoom.GetNextAssoc(Position, dwGroupID, pRoomIDArray);
		pRoomIDArray->RemoveAll();
		pRoomIDArray->FreeMemory();
	}
	m_MapGroupRoom.RemoveAll();
	return;
}

//��������
tagPersonalTableInfo* CGlobalInfoManager::SearchTableByID(DWORD dwRoomID)
{	
	//�������
	tagPersonalTableInfo* pPersonalTable = NULL;
	if(m_MapPersonalTableInfo.Lookup(dwRoomID, pPersonalTable) == TRUE)
		return pPersonalTable;
	return NULL;
}

tagPersonalTableInfo* CGlobalInfoManager::SearchTableByTableIDAndServerID(DWORD dwServerID, DWORD dwTableID)
{
	//�Ƴ�����
	POSITION Position = NULL;
	tagPersonalTableInfo* pPersonalTable = NULL;
	DWORD dwRoomID;
	int iHostCreateRoomCount = 0;
	Position = m_MapPersonalTableInfo.GetStartPosition();
	while(Position != NULL)
	{
		m_MapPersonalTableInfo.GetNextAssoc(Position,dwRoomID,pPersonalTable);
		if(pPersonalTable != NULL && pPersonalTable->dwServerID == dwServerID &&  pPersonalTable->dwTableID == dwTableID)
		{
			return pPersonalTable;
		}
	}

	return NULL;
}

tagPersonalTableInfo *CGlobalInfoManager::SearchTableByPosition(WORD wKindID, BYTE cbPlayMode, DWORD dwGroupID, POSITION & Position, DWORD & dwRoomID)
{
	//�Ƴ�����
	if (Position == NULL)
	{
		Position = m_MapPersonalTableInfo.GetStartPosition();
	}

	tagPersonalTableInfo *pPersonalTable = NULL;

	while (Position != NULL)
	{
		m_MapPersonalTableInfo.GetNextAssoc(Position, dwRoomID, pPersonalTable);
		if (pPersonalTable != NULL && pPersonalTable->dwKindID == wKindID &&  pPersonalTable->dwGroupID == dwGroupID   && pPersonalTable->cbPlayMode == cbPlayMode)
		{
			if (cbPlayMode == 1 && pPersonalTable->cbJoinMode == 0)
			{
				continue;
			}
			return pPersonalTable;
		}
	}

	return NULL;
}

//�������
bool CGlobalInfoManager::AddFreeServerTable(DWORD dwServerID)
{
	//���ҷ���
	INT_PTR nSize = m_ServerTableCountArray.GetCount();
	CGlobalServerItem* pServerItem = SearchServerItem(static_cast<WORD>(dwServerID));
	if(pServerItem == NULL) return true;
	for(INT_PTR i = 0; i < nSize; ++i)
	{
		tagServerTableCount* pServerTableCount = m_ServerTableCountArray.GetAt(i);
		if(pServerTableCount->dwServerID == dwServerID)
		{
			if(pServerItem->m_GameServer.wTableCount < pServerTableCount->dwTableCount +1) return true;
			pServerTableCount->dwTableCount += 1;
			break;
		}
	}
	return true;
}

//�������
bool CGlobalInfoManager::AddServerTable(DWORD dwRoomID, tagPersonalTableInfo PersonalTable)
{
	//�������
	tagPersonalTableInfo* pPersonalTable = NULL;

	//��鷿������Լս�������Ŀ�������������Ŀ���������Ŀ��������
	if (GetHostCreatePersonalRoomCount(PersonalTable.dwUserID, PersonalTable.dwKindID, PersonalTable.cbPlayMode) > MAX_CREATE_PERSONAL_ROOM)
	{
		return false;
	}

	//�������
	pPersonalTable = NULL;
	if (m_MapPersonalTableInfo.Lookup(dwRoomID, pPersonalTable) == FALSE)
	{
		pPersonalTable = new tagPersonalTableInfo;

		m_MapPersonalTableInfo[dwRoomID] = pPersonalTable;
		CopyMemory(pPersonalTable, &PersonalTable, sizeof(tagPersonalTableInfo));
	}
	else
	{
		return false;
	}

	return true;
}

//�Ƴ���������
bool CGlobalInfoManager::RemoveFreeServerTable(DWORD dwServerID)
{
	//���ҷ���
	INT_PTR nSize = m_ServerTableCountArray.GetCount();
	CGlobalServerItem* pServerItem = SearchServerItem(static_cast<WORD>(dwServerID));
	if(pServerItem == NULL) return true;
	for(INT_PTR i = 0; i < nSize; ++i)
	{
		tagServerTableCount* pServerTableCount = m_ServerTableCountArray.GetAt(i);
		if(pServerTableCount->dwServerID == dwServerID)
		{
			if(pServerTableCount->dwTableCount < 1) return true;
			pServerTableCount->dwTableCount -= 1;
			break;
		}
	}
	return true;
}

//�Ƴ�����
bool CGlobalInfoManager::RemoveServerTable(DWORD dwServerID, DWORD dwTableID)
{
	//�Ƴ�����
	POSITION Position = NULL;
	tagPersonalTableInfo* pPersonalTable = NULL;
	DWORD dwRoomID;

	Position = m_MapPersonalTableInfo.GetStartPosition();
	while(Position != NULL)
	{
		m_MapPersonalTableInfo.GetNextAssoc(Position, dwRoomID, pPersonalTable);
		if(pPersonalTable != NULL && pPersonalTable->dwServerID == dwServerID && pPersonalTable->dwTableID == dwTableID)
		{
			//������ɢ�����������ɢ���Ӽ��� Լս��
			//ͳ��ͬһ����������ɢ�ķ�������
			int nOneHostDissumeCount = 0;
			for (int i = 0; i < (int)m_VecDissumePersonalTableInfo.size(); i++)
			{
				if (m_VecDissumePersonalTableInfo[i].dwUserID == pPersonalTable->dwUserID)
				{
					nOneHostDissumeCount++;
				}
			}

			//ɾ�����ȱ���ɢ��Լս��
			if (nOneHostDissumeCount > MAX_CREATE_PERSONAL_ROOM)
			{
				int nOneHostDissumeCount = 0;
				for (int i = 0; i < (int)m_VecDissumePersonalTableInfo.size(); i++)
				{
					if (m_VecDissumePersonalTableInfo[i].dwUserID == pPersonalTable->dwUserID)
					{
						nOneHostDissumeCount = i;
						m_VecDissumePersonalTableInfo.erase(m_VecDissumePersonalTableInfo.begin() + i);
						break;
					}
				}

			}
			m_VecDissumePersonalTableInfo.push_back(*pPersonalTable);

			SafeDelete(pPersonalTable);
			m_MapPersonalTableInfo.RemoveKey(dwRoomID);
			return true;
		}
	}
	return false;
}


//�Ƴ�����
bool CGlobalInfoManager::RemoveServerTable(DWORD dwServerID)
{
	//�Ƴ�����
	POSITION Position = NULL;
	tagPersonalTableInfo* pPersonalTable = NULL;
	DWORD dwRoomID;

	Position = m_MapPersonalTableInfo.GetStartPosition();
	while(Position != NULL)
	{
		m_MapPersonalTableInfo.GetNextAssoc(Position, dwRoomID, pPersonalTable);
		if (Position == NULL)
		{
			break;
		}
		if(pPersonalTable != NULL && pPersonalTable->dwServerID == dwServerID)
		{

			SafeDelete(pPersonalTable);
			m_MapPersonalTableInfo.RemoveKey(dwRoomID);

		}
	}
	return true;
}


//��ȡ�������������з���
int CGlobalInfoManager::GetHostCreatePersonalRoom(tagHostCreatRoomInfo & HostCreatRoomInfo)
{
	TCHAR szInfo[260] = {0};
	//�Ƴ�����
	POSITION Position = NULL;
	tagPersonalTableInfo* pPersonalTable = NULL;
	DWORD dwRoomID;
	int iHostCreateRoomCount = 0;
	Position = m_MapPersonalTableInfo.GetStartPosition();
	while(Position != NULL)
	{
		m_MapPersonalTableInfo.GetNextAssoc(Position, dwRoomID, pPersonalTable);
		if (pPersonalTable != NULL && pPersonalTable->dwUserID == HostCreatRoomInfo.dwUserID && pPersonalTable->cbPlayMode == 0 && pPersonalTable->dwGroupID==0)
		{
			HostCreatRoomInfo.hRoomCreateInfo[iHostCreateRoomCount].dwPersonalRoomID =  pPersonalTable->dwPersonalRoomID;
			HostCreatRoomInfo.hRoomCreateInfo[iHostCreateRoomCount].wBeginFreeTime = pPersonalTable->wBeginFreeTime;
			HostCreatRoomInfo.hRoomCreateInfo[iHostCreateRoomCount].wEndFreeTime = pPersonalTable->wEndFreeTime;
			HostCreatRoomInfo.hRoomCreateInfo[iHostCreateRoomCount].wCurSitCount = pPersonalTable->wCurSitPlayer;

			iHostCreateRoomCount++;
			//������󷿼���Ŀ����
			if(iHostCreateRoomCount >= MAX_CREATE_PERSONAL_ROOM)
			{
				return iHostCreateRoomCount;
			}
		}

	}

	//�����Ѿ���ɢ������
	for (int i = m_VecDissumePersonalTableInfo.size() - 1; i >= 0 ; i--)
	{
		if (pPersonalTable != NULL && m_VecDissumePersonalTableInfo[i].dwUserID == HostCreatRoomInfo.dwUserID && m_VecDissumePersonalTableInfo[i].cbPlayMode == 0 && pPersonalTable->dwGroupID == 0)
		{
			HostCreatRoomInfo.hRoomCreateInfo[iHostCreateRoomCount].dwPersonalRoomID = m_VecDissumePersonalTableInfo[i].dwPersonalRoomID;		
			iHostCreateRoomCount++;
			if(iHostCreateRoomCount >= MAX_CREATE_PERSONAL_ROOM)
			{
				break;
			}
		}
	}

	return iHostCreateRoomCount;
}


//��ȡ���������ķ��������
INT CGlobalInfoManager::GetHostCreatePersonalRoomCount(DWORD dwUserID, DWORD dwKindID, BYTE cbPlayMode)
{
	tagPersonalTableInfo* pPersonalTable = NULL;
	POSITION Position = NULL;
	int iHostCreateRoomCount = 0;
	DWORD dwRoomID;
	Position = m_MapPersonalTableInfo.GetStartPosition();
	while(Position != NULL)
	{
		m_MapPersonalTableInfo.GetNextAssoc(Position, dwRoomID, pPersonalTable);
		if (pPersonalTable != NULL && pPersonalTable->dwUserID == dwUserID &&  pPersonalTable->dwKindID == dwKindID && pPersonalTable->cbPlayMode == cbPlayMode)
		{
			iHostCreateRoomCount++;
		}
	}
	return  iHostCreateRoomCount;
}

//��ȡ����
DWORD CGlobalInfoManager::GetFreeServer(DWORD dwUserID, DWORD dwKindID, BYTE cbIsJoinGame, BYTE cbPlayMode, DWORD dwGroupID)
{

	//��������
	POSITION Position;
	bool bExit = false;
	DWORD dwRoomID;

	//�������������Ϸ
	if (cbIsJoinGame)
	{
		tagPersonalTableInfo* pPersonalTableInfo;
		//�����û�
		Position=m_MapPersonalTableInfo.GetStartPosition();
		while(Position != NULL)
		{
			m_MapPersonalTableInfo.GetNextAssoc(Position, dwRoomID, pPersonalTableInfo);
			if(pPersonalTableInfo != NULL)
			{
				if (pPersonalTableInfo->dwUserID == dwUserID && pPersonalTableInfo->cbPlayMode == cbPlayMode)
				{
					bExit = true;
					return 0;
				}
			}
		}
	}


	//���ҷ���
	INT_PTR nSize = m_ServerTableCountArray.GetCount();
	for(INT_PTR i = 0; i < nSize; ++i)
	{
		tagServerTableCount* pServerTableCount = m_ServerTableCountArray.GetAt(i);
		if (pServerTableCount->dwKindID == dwKindID && pServerTableCount->cbPlayMode == cbPlayMode && pServerTableCount->dwTableCount > 0)
		{
			return pServerTableCount->dwServerID;
		}
	}
	return 0;
}

//��ȡ������Ŀ
DWORD CGlobalInfoManager::GetFreeServerCount(DWORD dwKindID, BYTE cbPlayMode)
{
	int iServerCount = 0;
	//���ҷ���
	INT_PTR nSize = m_ServerTableCountArray.GetCount();
	for(INT_PTR i = 0; i < nSize; ++i)
	{
		tagServerTableCount* pServerTableCount = m_ServerTableCountArray.GetAt(i);
		if (pServerTableCount->dwKindID == dwKindID && cbPlayMode == pServerTableCount->cbPlayMode)
		{
			iServerCount++;
		}
	}

	return iServerCount;
}

//��ȡ������Ŀ
DWORD CGlobalInfoManager::GetFreeServerCount(DWORD dwKindID, BYTE cbPlayMode, bool bSupportGroup)
{
	int iServerCount = 0;
	//���ҷ���
	INT_PTR nSize = m_ServerTableCountArray.GetCount();
	for (INT_PTR i = 0; i < nSize; ++i)
	{
		tagServerTableCount* pServerTableCount = m_ServerTableCountArray.GetAt(i);
		if (pServerTableCount->dwKindID == dwKindID && cbPlayMode == pServerTableCount->cbPlayMode && pServerTableCount->bSupportGroup == bSupportGroup)
		{
			iServerCount++;
		}
	}

	return iServerCount;
}

//���ɷ���ID
DWORD CGlobalInfoManager::RandPersonalRoomID()
{
	//�������
	DWORD dwRoomID = 0;
	bool bExit = true;
	srand(rand());
	while(bExit)
	{		
		dwRoomID = rand() % 9999 + (rand() % 88 + 10) * 10000;
		
		if (dwRoomID > 999999 || dwRoomID < 100000) continue;
		//����ID
		tagPersonalTableInfo* pPersonalTable = NULL;
		if (m_MapPersonalTableInfo.Lookup(dwRoomID, pPersonalTable) == TRUE)
			bExit = true;
		else
			bExit = false;
	}
	return dwRoomID;
}

//�����û�
bool CGlobalInfoManager::ActiveUserItem(tagGlobalUserInfo &GlobalUserInfo, WORD wServerID)
{
	//Ѱ�ҷ���
	CGlobalServerItem *pGlobalServerItem = NULL;
	if (m_MapServerID.Lookup(wServerID, pGlobalServerItem) == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//Ѱ���û�
	CGlobalUserItem *pGlobalUserItem = NULL;
	if (m_MapUserID.Lookup(GlobalUserInfo.dwUserID, pGlobalUserItem) == FALSE)
	{
		//�����û�
		pGlobalUserItem = CreateGlobalUserItem();
		if (pGlobalUserItem == NULL)
		{
			ASSERT(FALSE);
			return false;
		}

		//�����ǳ�
		TCHAR szNickName[LEN_NICKNAME] = TEXT("");
		lstrcpyn(szNickName, GlobalUserInfo.szNickName, CountArray(szNickName));

		//�����û�
		pGlobalUserItem->m_dwUserID = GlobalUserInfo.dwUserID;
		pGlobalUserItem->m_dwGameID = GlobalUserInfo.dwGameID;
		lstrcpyn(pGlobalUserItem->m_szNickName, GlobalUserInfo.szNickName, CountArray(pGlobalUserItem->m_szNickName));

		//������Ϣ
		pGlobalUserItem->m_cbGender = GlobalUserInfo.cbGender;
		pGlobalUserItem->m_cbMemberOrder = GlobalUserInfo.cbMemberOrder;
		pGlobalUserItem->m_cbMasterOrder = GlobalUserInfo.cbMasterOrder;
		memcpy(&pGlobalUserItem->m_UserInfo, &GlobalUserInfo.userInfo, sizeof(tagUserInfo));

		//�ǳ�����
		m_MapNickName[szNickName] = pGlobalUserItem;

		//��������
		m_MapUserID[GlobalUserInfo.dwUserID] = pGlobalUserItem;
		m_MapGameID[GlobalUserInfo.dwGameID] = pGlobalUserItem;
	}
	else
	{
		//�ظ��ж�
		for (INT_PTR i = 0; i < pGlobalUserItem->m_GlobalServerItemArray.GetCount(); i++)
		{
			if (pGlobalUserItem->m_GlobalServerItemArray[i]->GetServerID() == wServerID)
			{
				ASSERT(FALSE);
				return false;
			}
		}
	}

	//���ù���
	pGlobalUserItem->m_GlobalServerItemArray.Add(pGlobalServerItem);
	pGlobalServerItem->m_MapUserID[GlobalUserInfo.dwUserID] = pGlobalUserItem;

	return true;
}

//ɾ���û�
bool CGlobalInfoManager::DeleteUserItem(DWORD dwUserID, WORD wServerID)
{
	//Ѱ���û�
	CGlobalUserItem * pGlobalUserItem=NULL;
	if (m_MapUserID.Lookup(dwUserID,pGlobalUserItem)==FALSE) 
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	CGlobalServerItem * pGlobalServerItem=NULL;
	INT_PTR nServerCount=pGlobalUserItem->m_GlobalServerItemArray.GetCount();

	//�˳�����
	for (INT_PTR i=0;i<nServerCount;i++)
	{
		//��ȡ����
		pGlobalServerItem=pGlobalUserItem->m_GlobalServerItemArray[i];

		//�����ж�
		if (pGlobalServerItem->GetServerID()==wServerID)
		{
			//ɾ������
			pGlobalServerItem->m_MapUserID.RemoveKey(dwUserID);
			pGlobalUserItem->m_GlobalServerItemArray.RemoveAt(i);

			//�ͷ��û�
			if (pGlobalUserItem->m_GlobalServerItemArray.GetCount()==0L) 
			{
				//�ͷ�����
				m_MapUserID.RemoveKey(dwUserID);
				m_MapGameID.RemoveKey(pGlobalUserItem->GetGameID());
				m_MapNickName.RemoveKey(pGlobalUserItem->GetNickName());

				//�ͷŶ���
				FreeGlobalUserItem(pGlobalUserItem);
			}

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}


//ɾ���㳡
bool CGlobalInfoManager::DeletePlazaItem(WORD wPlazaID)
{
	//Ѱ�ҹ㳡
	CGlobalPlazaItem * pGlobalPlazaItem=NULL;
	if (m_MapPlazaID.Lookup(wPlazaID,pGlobalPlazaItem)==FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//�ͷŹ㳡
	m_MapPlazaID.RemoveKey(wPlazaID);
	FreeGlobalPlazaItem(pGlobalPlazaItem);

	return true;
}

//����㳡
bool CGlobalInfoManager::ActivePlazaItem(WORD wBindIndex, tagGamePlaza & GamePlaza)
{
	//Ѱ�ҹ㳡
	CGlobalPlazaItem * pGlobalPlazaItem=NULL;
	if (m_MapPlazaID.Lookup(GamePlaza.wPlazaID,pGlobalPlazaItem)==TRUE)
	{
		ASSERT(FALSE);
		return false;
	}

	//�����㳡
	pGlobalPlazaItem=CreateGlobalPlazaItem();
	if (pGlobalPlazaItem==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//���ù㳡
	pGlobalPlazaItem->m_wIndex=wBindIndex;
	pGlobalPlazaItem->m_GamePlaza=GamePlaza;

	//��������
	m_MapPlazaID[GamePlaza.wPlazaID]=pGlobalPlazaItem;

	return true;
}

//ɾ������
bool CGlobalInfoManager::DeleteServerItem(WORD wServerID)
{
	//Ѱ�ҷ���
	CGlobalServerItem * pGlobalServerItem=NULL;
	if (m_MapServerID.Lookup(wServerID,pGlobalServerItem)==FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	DWORD dwUserKey=0L;
	CGlobalUserItem * pGlobalUserItem=NULL;
	POSITION Position=pGlobalServerItem->m_MapUserID.GetStartPosition();

	//ɾ���û�
	while (Position!=NULL)
	{
		//��ȡ����
		pGlobalServerItem->m_MapUserID.GetNextAssoc(Position,dwUserKey,pGlobalUserItem);

		//�������
		for (INT_PTR i=0;i<pGlobalUserItem->m_GlobalServerItemArray.GetCount();i++)
		{
			//��ȡ����
			CGlobalServerItem * pTempServerItem=pGlobalUserItem->m_GlobalServerItemArray[i];

			//�����ж�
			if (pTempServerItem->GetServerID()==wServerID)
			{
				pGlobalUserItem->m_GlobalServerItemArray.RemoveAt(i);
				break;
			}
		}

		//�ͷ��û�
		if (pGlobalUserItem->m_GlobalServerItemArray.GetCount()==0)
		{
			m_MapUserID.RemoveKey(dwUserKey);
			FreeGlobalUserItem(pGlobalUserItem);
		}
	}

	//�ͷŷ���
	m_MapServerID.RemoveKey(wServerID);
	FreeGlobalServerItem(pGlobalServerItem);

	//���ҷ���
	INT_PTR nSize = m_ServerTableCountArray.GetCount();
	for(INT_PTR i = 0; i < nSize; ++i)
	{
		tagServerTableCount* pServerTableCount = m_ServerTableCountArray.GetAt(i);
		if(pServerTableCount->dwServerID == wServerID)
		{
			SafeDelete(pServerTableCount);
			m_ServerTableCountArray.RemoveAt(i);
			break;
		}
	}

	//ɾ���û���Ϣ
	DWORD dwRoomID;
	tagPersonalTableInfo* pPersonalTableInfo = NULL;
	Position = m_MapPersonalTableInfo.GetStartPosition();
	while(Position != NULL)
	{
		m_MapPersonalTableInfo.GetNextAssoc(Position, dwRoomID, pPersonalTableInfo);

		if(pPersonalTableInfo != NULL && pPersonalTableInfo->dwServerID == wServerID)
		{

			SafeDelete(pPersonalTableInfo);
			m_MapPersonalTableInfo.RemoveKey(dwRoomID);
		}
		pPersonalTableInfo = NULL;
	}

	//ɾ���û���Ϣ
	RemoveServerTable(wServerID);
	return true;
}

//�����
bool CGlobalInfoManager::ActiveServerItem(WORD wBindIndex, BYTE	cbPlayMode, tagGameServer &GameServer)
{
	//Ѱ�ҷ���
	CGlobalServerItem * pGlobalServerItem=NULL;
	if (m_MapServerID.Lookup(GameServer.wServerID,pGlobalServerItem)==TRUE)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	pGlobalServerItem=CreateGlobalServerItem();
	if (pGlobalServerItem==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//���÷���
	pGlobalServerItem->m_wIndex=wBindIndex;
	pGlobalServerItem->m_GameServer=GameServer;

	//��������
	m_MapServerID[GameServer.wServerID]=pGlobalServerItem;

	//����Լս����
	DWORD dwServerID = pGlobalServerItem->GetServerID();
	DWORD dwTableCount = pGlobalServerItem->GetTabelCount();

	//���ҷ���
	INT_PTR nSize = m_ServerTableCountArray.GetCount();
	bool bExit = false;
	for(INT_PTR i = 0; i < nSize; ++i)
	{
		tagServerTableCount* pServerTableCount = m_ServerTableCountArray.GetAt(i);
		if(pServerTableCount != NULL && pServerTableCount->dwServerID == dwServerID)
		{
			bExit = true;
			break;
		}
	}

	if(bExit == false && pGlobalServerItem->m_GameServer.wServerType == GAME_GENRE_PERSONAL)
	{
		tagServerTableCount* pServerTableCount = new tagServerTableCount;
		pServerTableCount->dwKindID = pGlobalServerItem->GetKindID();
		pServerTableCount->dwServerID = dwServerID;
		pServerTableCount->dwTableCount = dwTableCount;
		pServerTableCount->cbPlayMode = cbPlayMode;
		pServerTableCount->bSupportGroup = CServerRule::IsSuportGroup(GameServer.dwServerRule);

		m_ServerTableCountArray.Add(pServerTableCount);
	}

	return true;
}

//ɾ������
bool CGlobalInfoManager::DeleteChatItem(WORD wChatID)
{
	//Ѱ�ҹ㳡
	CGlobalChatItem * pGlobalChatItem=NULL;
	if (m_MapChatID.Lookup(wChatID,pGlobalChatItem)==FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//�ͷŹ㳡
	m_MapChatID.RemoveKey(wChatID);
	FreeGlobalChatItem(pGlobalChatItem);

	return true;
}

//��������
bool CGlobalInfoManager::ActiveChatItem(WORD wBindIndex, tagChatServer & ChatServer)
{
	//Ѱ�ҷ���
	CGlobalChatItem * pGlobalChatItem=NULL;
	if (m_MapChatID.Lookup(ChatServer.wChatID,pGlobalChatItem)==TRUE)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	pGlobalChatItem=CreateGlobalChatItem();
	if (pGlobalChatItem==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//���÷���
	pGlobalChatItem->m_wIndex=wBindIndex;
	pGlobalChatItem->m_ChatServer=ChatServer;

	//��������
	m_MapChatID[ChatServer.wChatID]=pGlobalChatItem;

	return true;
}
//������ֲ�����
bool CGlobalInfoManager::ActiveGroupRoom(DWORD dwGroupID, DWORD dwRoomID)
{
	if (dwGroupID == 0) return true;

	CRoomIDArray * pRoomIDArray = NULL;
	BOOL bResult = m_MapGroupRoom.Lookup(dwGroupID, pRoomIDArray);

	if (bResult == 0)
	{
		pRoomIDArray = new CRoomIDArray;
		pRoomIDArray->Add(dwRoomID);
		m_MapGroupRoom[dwGroupID] = pRoomIDArray;
	}
	else
	{
		if (pRoomIDArray)
		{
			pRoomIDArray->Add(dwRoomID);
		}

	}

	return false;
}

//��ȡ���ֲ�����
CRoomIDArray * CGlobalInfoManager::GetGroupRoom(DWORD dwGroupID)
{
	CRoomIDArray * pRoomIDArray = NULL;
	m_MapGroupRoom.Lookup(dwGroupID, pRoomIDArray);
	return pRoomIDArray;
}

//ɾ�����ֲ�����
BOOL CGlobalInfoManager::DeleteGroupRoom(DWORD dwGroupID, DWORD dwRoomID)
{
	if (dwGroupID == 0) return TRUE;

	CRoomIDArray * pRoomIDArray = NULL;
	BOOL bResult = m_MapGroupRoom.Lookup(dwGroupID, pRoomIDArray);

	if (bResult == 0)
	{
		return FALSE;
	}
	else
	{
		for (int i = 0; i < pRoomIDArray->GetCount(); i++)
		{
			if (dwRoomID == pRoomIDArray->GetAt(i))
			{
				pRoomIDArray->RemoveAt(i);
				break;
			}
		}
	}

	return TRUE;
}

//Ѱ�ҹ㳡
CGlobalPlazaItem * CGlobalInfoManager::SearchPlazaItem(WORD wPlazaID)
{
	//Ѱ�ҷ���
	CGlobalPlazaItem * pGlobalPlazaItem=NULL;
	m_MapPlazaID.Lookup(wPlazaID,pGlobalPlazaItem);

	return pGlobalPlazaItem;
}

//Ѱ�ҷ���
CGlobalServerItem * CGlobalInfoManager::SearchServerItem(WORD wServerID)
{
	//Ѱ�ҷ���
	CGlobalServerItem * pGlobalServerItem=NULL;
	m_MapServerID.Lookup(wServerID,pGlobalServerItem);

	return pGlobalServerItem;
}

//Ѱ������
CGlobalChatItem * CGlobalInfoManager::SearchChatItem(WORD wChatID)
{
	//Ѱ�ҷ���
	CGlobalChatItem * pGlobalChatItem=NULL;
	m_MapChatID.Lookup(wChatID,pGlobalChatItem);

	return pGlobalChatItem;
}

//Ѱ���û�
CGlobalUserItem * CGlobalInfoManager::SearchUserItemByUserID(DWORD dwUserID)
{
	//Ѱ���û�
	CGlobalUserItem * pGlobalUserItem=NULL;
	m_MapUserID.Lookup(dwUserID,pGlobalUserItem);

	return pGlobalUserItem;
}

//Ѱ���û�
CGlobalUserItem * CGlobalInfoManager::SearchUserItemByGameID(DWORD dwGameID)
{
	//Ѱ���û�
	CGlobalUserItem * pGlobalUserItem=NULL;
	m_MapGameID.Lookup(dwGameID,pGlobalUserItem);

	return pGlobalUserItem;
}

//Ѱ���û�
CGlobalUserItem * CGlobalInfoManager::SearchUserItemByNickName(LPCTSTR pszNickName)
{
	//Ѱ���û�
	VOID * pGlobalUserItem=NULL;
	m_MapNickName.Lookup(pszNickName,pGlobalUserItem);

	return (CGlobalUserItem *)pGlobalUserItem;
}

//ö���û�
CGlobalUserItem * CGlobalInfoManager::EnumUserItem(POSITION & Position)
{
	//��������
	DWORD dwUserID=0L;
	CGlobalUserItem * pGlobalUserItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_MapUserID.GetStartPosition();
	if (Position!=NULL)	m_MapUserID.GetNextAssoc(Position,dwUserID,pGlobalUserItem);

	return pGlobalUserItem;
}

//ö�ٹ㳡
CGlobalPlazaItem * CGlobalInfoManager::EnumPlazaItem(POSITION & Position)
{
	//��������
	WORD wPlazaID=0L;
	CGlobalPlazaItem * pGlobalPlazaItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_MapPlazaID.GetStartPosition();
	if (Position!=NULL)	m_MapPlazaID.GetNextAssoc(Position,wPlazaID,pGlobalPlazaItem);

	return pGlobalPlazaItem;
}

//ö�ٷ���
CGlobalServerItem * CGlobalInfoManager::EnumServerItem(POSITION & Position)
{
	//��������
	WORD wServerID=0L;
	CGlobalServerItem * pGlobalServerItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_MapServerID.GetStartPosition();
	if (Position!=NULL)	m_MapServerID.GetNextAssoc(Position,wServerID,pGlobalServerItem);

	return pGlobalServerItem;
}

//ö������
CGlobalChatItem * CGlobalInfoManager::EnumChatItem(POSITION & Position)
{
	//��������
	WORD wChatID=0L;
	CGlobalChatItem * pGlobalChatItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_MapChatID.GetStartPosition();
	if (Position!=NULL)	m_MapChatID.GetNextAssoc(Position,wChatID,pGlobalChatItem);

	return pGlobalChatItem;
}

//�����û�
CGlobalUserItem * CGlobalInfoManager::CreateGlobalUserItem()
{
	//ʹ�ô洢
	if (m_pGlobalUserItem!=NULL)
	{
		CGlobalUserItem * pGlobalUserItem=m_pGlobalUserItem;
		m_pGlobalUserItem=m_pGlobalUserItem->m_pNextUserItemPtr;
		pGlobalUserItem->m_pNextUserItemPtr=NULL;
		return pGlobalUserItem;
	}

	//��������
	try 
	{ 
		CGlobalUserItem * pGlobalUserItem=new CGlobalUserItem;
		return pGlobalUserItem;
	}
	catch (...) {}

	return NULL;
}

//�����㳡
CGlobalPlazaItem * CGlobalInfoManager::CreateGlobalPlazaItem()
{
	//ʹ�ô洢
	if (m_pGlobalPlazaItem!=NULL)
	{
		CGlobalPlazaItem * pGlobalPlazaItem=m_pGlobalPlazaItem;
		m_pGlobalPlazaItem=m_pGlobalPlazaItem->m_pNextPlazaItemPtr;
		pGlobalPlazaItem->m_pNextPlazaItemPtr=NULL;
		return pGlobalPlazaItem;
	}

	//��������
	try 
	{ 
		CGlobalPlazaItem * pGlobalPlazaItem=new CGlobalPlazaItem;
		return pGlobalPlazaItem;
	}
	catch (...) {}

	return NULL;
}

//��������
CGlobalServerItem * CGlobalInfoManager::CreateGlobalServerItem()
{
	//ʹ�ô洢
	if (m_pGlobalServerItem!=NULL)
	{
		CGlobalServerItem * pGlobalServerItem=m_pGlobalServerItem;
		m_pGlobalServerItem=m_pGlobalServerItem->m_pNextServerItemPtr;
		pGlobalServerItem->m_pNextServerItemPtr=NULL;
		return pGlobalServerItem;
	}

	//��������
	try 
	{ 
		CGlobalServerItem * pGlobalServerItem=new CGlobalServerItem;
		return pGlobalServerItem;
	}
	catch (...) {}

	return NULL;
}

//��������
CGlobalChatItem * CGlobalInfoManager::CreateGlobalChatItem()
{
	//ʹ�ô洢
	if (m_pGlobalChatItem!=NULL)
	{
		CGlobalChatItem * pGlobalChatItem=m_pGlobalChatItem;
		m_pGlobalChatItem=m_pGlobalChatItem->m_pNextChatServerPtr;
		pGlobalChatItem->m_pNextChatServerPtr=NULL;
		return pGlobalChatItem;
	}

	//��������
	try 
	{ 
		CGlobalChatItem * pGlobalChatItem=new CGlobalChatItem;
		return pGlobalChatItem;
	}
	catch (...) {}

	return NULL;
}

//�ͷ��û�
bool CGlobalInfoManager::FreeGlobalUserItem(CGlobalUserItem * pGlobalUserItem)
{
	//Ч�����
	ASSERT(pGlobalUserItem!=NULL);
	if (pGlobalUserItem==NULL) return false;

	//���ñ���
	pGlobalUserItem->m_dwUserID=0L;
	pGlobalUserItem->m_dwGameID=0L;
	pGlobalUserItem->m_szNickName[0]=0;
	ZeroMemory(&pGlobalUserItem->m_UserInfo, sizeof(tagUserInfo));
	pGlobalUserItem->m_GlobalServerItemArray.RemoveAll();

	//����洢
	pGlobalUserItem->m_pNextUserItemPtr=m_pGlobalUserItem;
	m_pGlobalUserItem=pGlobalUserItem;

	return true;
}

//�ͷŹ㳡
bool CGlobalInfoManager::FreeGlobalPlazaItem(CGlobalPlazaItem * pGlobalPlazaItem)
{
	//Ч�����
	ASSERT(pGlobalPlazaItem!=NULL);
	if (pGlobalPlazaItem==NULL) return false;

	//���ñ���
	pGlobalPlazaItem->m_wIndex=0;
	ZeroMemory(&pGlobalPlazaItem->m_GamePlaza,sizeof(pGlobalPlazaItem->m_GamePlaza));

	//����洢
	pGlobalPlazaItem->m_pNextPlazaItemPtr=m_pGlobalPlazaItem;
	m_pGlobalPlazaItem=pGlobalPlazaItem;

	return true;
}

//�ͷŷ���
bool CGlobalInfoManager::FreeGlobalServerItem(CGlobalServerItem * pGlobalServerItem)
{
	//Ч�����
	ASSERT(pGlobalServerItem!=NULL);
	if (pGlobalServerItem==NULL) return false;

	//��������
	pGlobalServerItem->m_MapUserID.RemoveAll();

	//���ñ���
	pGlobalServerItem->m_wIndex=0;
	ZeroMemory(&pGlobalServerItem->m_GameServer,sizeof(pGlobalServerItem->m_GameServer));

	//����洢
	pGlobalServerItem->m_pNextServerItemPtr=m_pGlobalServerItem;
	m_pGlobalServerItem=pGlobalServerItem;

	return true;
}

//�ͷ�����
bool CGlobalInfoManager::FreeGlobalChatItem(CGlobalChatItem * pGlobalChatItem)
{
	//Ч�����
	ASSERT(pGlobalChatItem!=NULL);
	if (pGlobalChatItem==NULL) return false;

	//���ñ���
	pGlobalChatItem->m_wIndex=0;
	ZeroMemory(&pGlobalChatItem->m_ChatServer,sizeof(pGlobalChatItem->m_ChatServer));

	//����洢
	pGlobalChatItem->m_pNextChatServerPtr=m_pGlobalChatItem;
	m_pGlobalChatItem=pGlobalChatItem;

	return true;
}

//���һ����Ϸ��ഴ��Լս�������Ŀ
bool CGlobalInfoManager::AddPersonalMaxCreate(CMD_CS_S_RegisterPersonal RegisterPersonal)
{
	int iCount = m_vecPersonalRoomMaxCreate.size();

	//�������������������룬 ����������޸�
	bool bIsExist = false;
	for (int i = 0; i < iCount; i++)
	{
		if (m_vecPersonalRoomMaxCreate[i].dwKindID == RegisterPersonal.dwKindID)
		{
			m_vecPersonalRoomMaxCreate[i].dwMaxCreate = RegisterPersonal.dwMaxCreate;
			bIsExist = true;
			break;
		}
	}

	if (!bIsExist)
	{
		m_vecPersonalRoomMaxCreate.push_back(RegisterPersonal);
	}

	return true;
}

//�Ƿ�����ٴ�������
bool CGlobalInfoManager::CanCreatePersonalRoom(DWORD dwKindID, DWORD dwUserID, BYTE cbPlayMode, DWORD dwGroupID)
{
	//��ȡ���Դ�������������Ŀ
	int iCount = m_vecPersonalRoomMaxCreate.size();
	int iMaxCreateCount = 0;
	for (int i = 0; i < iCount; i++)
	{
		if (m_vecPersonalRoomMaxCreate[i].dwKindID == dwKindID && m_vecPersonalRoomMaxCreate[i].cbPlayMode == cbPlayMode)
		{
			iMaxCreateCount = m_vecPersonalRoomMaxCreate[i].dwMaxCreate;
			break;
		}
	}

	if (iMaxCreateCount == 0)
	{
		iMaxCreateCount = MAX_CREATE_COUNT;
	}

	//���Լս������Ŀ
	int iHaveCreate = GetHostCreatePersonalRoomCount(dwUserID, dwKindID, cbPlayMode);
	if (iHaveCreate < iMaxCreateCount)
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
