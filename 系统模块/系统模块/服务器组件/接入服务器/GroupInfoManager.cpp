#include "StdAfx.h"
#include "GroupInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CGroupItem::CGroupItem()
{
	//���ñ���	
	m_GroupMemberArray.RemoveAll();
	ZeroMemory(&m_GroupItem,sizeof(m_GroupItem));
	m_Notice = L"";
}

//��������
CGroupItem::~CGroupItem()
{
	//���ñ���	
	ResetGroupItem();
}

//�Ƴ���Ա
bool CGroupItem::RemoveMember(DWORD dwMemberID)
{
	//���Ҳ��Ƴ�
	for (INT_PTR i=0; i<m_GroupMemberArray.GetCount(); i++)
	{
		if (m_GroupMemberArray[i].dwMemberID==dwMemberID)
		{
			m_GroupMemberArray.RemoveAt(i);			
			return true;
		}
	}

	return false;
}

//��ӳ�Ա
bool CGroupItem::AddMember(tagIMGroupMember & GroupMember)
{
	//���ҳ�Ա
	tagIMGroupMember *pGroupMember=SearchMember(GroupMember.dwMemberID);
	if (pGroupMember!=NULL)
	{
		//������Ϣ
		CopyMemory(pGroupMember,&GroupMember,sizeof(GroupMember));
	}
	else
	{
		m_GroupMemberArray.Add(GroupMember);		
	}

	return true;
}

//ö�ٳ�Ա
tagIMGroupMember * CGroupItem::EnumMember(INT nIndex)
{
	if (nIndex>=m_GroupMemberArray.GetCount()) return NULL;
	return &m_GroupMemberArray[nIndex];
}

//���ҳ�Ա
tagIMGroupMember * CGroupItem::SearchMember(DWORD dwMemberID)
{
	for (INT_PTR i=0; i<m_GroupMemberArray.GetCount(); i++)
	{
		if (m_GroupMemberArray[i].dwMemberID==dwMemberID)
		{
			return &m_GroupMemberArray[i];
		}
	}

	return NULL;
}
void CGroupItem::AddRollMsg(tagClubRollMsg &RollMsg)
{
	//���ҳ�Ա
	tagClubRollMsg *pGroupMember = SearchRollMsg(RollMsg.dwUserID);
	if (pGroupMember != NULL)
	{
		//������Ϣ
		CopyMemory(pGroupMember, &RollMsg, sizeof(tagClubRollMsg));
	}
	else
	{
		m_GroupRollMsgArray.Add(RollMsg);
	}

}
tagClubRollMsg * CGroupItem::SearchRollMsg(DWORD dwUserID)
{
	for (INT_PTR i = 0; i<m_GroupRollMsgArray.GetCount(); i++)
	{
		if (m_GroupRollMsgArray[i].dwUserID == dwUserID)
		{
			return &m_GroupRollMsgArray[i];
		}
	}

	return NULL;
}

//���ҳ�Ա
bool CGroupItem::AddPersonalTable(DWORD dwPersonalRoomID, tagClubTableInfo TableInfo)
{
	m_MapClubTableInfo.SetAt(dwPersonalRoomID, TableInfo);

	return true;
}

//�Ƴ���Ա
bool CGroupItem::RemovePersonalTable(DWORD dwPersonalRoomID)
{
	//���Ҳ��Ƴ�
	m_MapClubTableInfo.RemoveKey(dwPersonalRoomID);

	return true;
}

//�Ƴ���Ա
bool CGroupItem::RemovePersonalTable(WORD wServerID)
{
	CMapClubTableInfo::CPair *  pValue = m_MapClubTableInfo.PGetFirstAssoc();

	while (pValue != NULL)

	{
		bool b = false;
		DWORD key = 0;
		if (pValue->value.dwServerID == wServerID)
		{
			key = pValue->key;
			b = true;
		}
		pValue = m_MapClubTableInfo.PGetNextAssoc(pValue);
		if (b)
		{
			m_MapClubTableInfo.RemoveKey(key);
		}
	}

	return false;
}

//�Ƴ���Ա
bool CGroupItem::GetTableInfo(DWORD dwPersonalRoomID, tagClubTableInfo &TableInfo)
{
	return m_MapClubTableInfo.Lookup(dwPersonalRoomID, TableInfo);
}

//�Ƴ���Ա
bool CGroupItem::UpdateTableUserInfo(CMD_CS_S_UpdateTable *pUpdateTable)
{
	tagClubTableInfo TableInfo;
	bool result = m_MapClubTableInfo.Lookup(pUpdateTable->dwPersonalRoomID, TableInfo);
	if (result)
	{
		/*
		TableInfo.dwKindID = pUpdateTable->dwKindID;
		TableInfo.dwServerID = pUpdateTable->dwServerID;
		TableInfo.wTableID = pUpdateTable->wTableID;
		TableInfo.dwGroupID = pUpdateTable->dwGroupID;
		TableInfo.dwUserID = pUpdateTable->dwUserID;
		TableInfo.wChairID = pUpdateTable->wChairID;
		TableInfo.bSit = pUpdateTable->bSit;
		TableInfo.bGameStart = pUpdateTable->bGameStart;*/
		if (pUpdateTable->bSit)
		{
			TableInfo.dwPlayerID[pUpdateTable->wChairID] = pUpdateTable->dwUserID;
			TableInfo.wCurSitPlayer++;
		}
		else{
			TableInfo.dwPlayerID[pUpdateTable->wChairID] = 0;
			TableInfo.wCurSitPlayer--;
		}
	}
	return result;
}

//�Ƴ���Ա
bool CGroupItem::IsUserInGame(DWORD dwUserID, tagClubTableInfo &TableInfo, bool bPlaying)
{
	if (!TableInfo.bGameStart)
	{
		return false;
	}
	for (BYTE i = 0; i < GROUP_MAX_CHAIR_GAME_CHAIR_COUNT; i++)
	{
		if (TableInfo.dwPlayerID[i] == dwUserID&&TableInfo.bGameStart)
		{
			return true;
		}
	}
	return false;
}

//�Ƴ���Ա
DWORD CGroupItem::GetConfigID(DWORD dwPersonalRoomID)
{
	tagClubTableInfo TableInfo;
	bool result = m_MapClubTableInfo.Lookup(dwPersonalRoomID, TableInfo);
	if (result)
	{
		return TableInfo.dwConfigID;
	}
	return 0;
}

//ö�ٳ�Ա
tagGroupConfigInfo * CGroupItem::EnumConfig(INT nIndex)
{
	if (nIndex >= m_GroupConfigArray.GetCount()) return NULL;
	return &m_GroupConfigArray[nIndex];
}

//���ҳ�Ա
bool CGroupItem::AddConfig(tagGroupConfigInfo & GroupConfig)
{
	//���ҳ�Ա
	tagGroupConfigInfo *pGroupConfig = SearchConfig(GroupConfig.dwConfigID);
	if (pGroupConfig != NULL)
	{
		//������Ϣ
		CopyMemory(pGroupConfig, &GroupConfig, sizeof(GroupConfig));
	}
	else
	{
		m_GroupConfigArray.Add(GroupConfig);
	}

	return true;
}

//�Ƴ���Ա
bool CGroupItem::ModifyConfig(tagGroupConfigInfo & GroupConfig)
{
	return AddConfig(GroupConfig);
}

//�Ƴ���Ա
bool CGroupItem::RemoveConfig(DWORD dwConfigID)
{
	//���Ҳ��Ƴ�
	for (INT_PTR i = 0; i<m_GroupConfigArray.GetCount(); i++)
	{
		if (m_GroupConfigArray[i].dwConfigID == dwConfigID)
		{
			m_GroupConfigArray.RemoveAt(i);
			return true;
		}
	}

	return false;
}

//�Ƴ���Ա
bool CGroupItem::RemoveConfig(INT nIndex)
{
	if (nIndex<m_GroupConfigArray.GetCount())
	{
		m_GroupConfigArray.RemoveAt(nIndex);
		return true;
	}

	return false;
}

//���ҳ�Ա
tagGroupConfigInfo * CGroupItem::SearchConfig(DWORD dwConfigID)
{
	for (INT_PTR i = 0; i<m_GroupConfigArray.GetCount(); i++)
	{
		if (m_GroupConfigArray[i].dwConfigID == dwConfigID)
		{
			return &m_GroupConfigArray[i];
		}
	}

	return NULL;
}

//ö�ٳ�Ա
INT CGroupItem::GetNullTableCount(DWORD dwConfigID)
{
	int result = 0;
	CMapClubTableInfo::CPair *  pValue = m_MapClubTableInfo.PGetFirstAssoc();

	while (pValue != NULL)

	{
		DWORD key = 0;
		if (pValue->value.wCurSitPlayer == 0)
		{
			result++;
		}
		pValue = m_MapClubTableInfo.PGetNextAssoc(pValue);
	}

	return result;
}

//ö�ٳ�Ա
tagClubTableInfo CGroupItem::EnumTable(POSITION &pos)
{
	//��ȡ����
	//zhaozw TODO
	tagClubTableInfo TableInfo;
	DWORD dwGroupID = 0;
	if (pos == NULL) pos = m_MapClubTableInfo.GetStartPosition();
	if (pos != NULL)	m_MapClubTableInfo.GetNextAssoc(pos, dwGroupID, TableInfo);
	//m_MapClubTableInfo..Lookup(dwPersonalRoomID, TableInfo)
	//ZeroMemory(&ClubTableInfo, sizeof(ClubTableInfo));
	return TableInfo;
}

//���ö���
void CGroupItem::ResetGroupItem()
{
	//���ñ���	
	m_GroupMemberArray.RemoveAll();
	ZeroMemory(&m_GroupItem,sizeof(m_GroupItem));
}

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CGroupInfoManager::CGroupInfoManager()
{
	//��������
	m_GroupIDMap.InitHashTable(10003L);
}

//��������
CGroupInfoManager::~CGroupInfoManager()
{
	//�ͷ��û�
	for (INT_PTR i=0;i<m_GroupItemStore.GetCount();i++)
	{
		CGroupItem * pGroupItem = m_GroupItemStore[i];
		SafeDelete(pGroupItem);
	}

	//ɾ������
	DWORD dwGroupID=0;
	CGroupItem * pGroupItem=NULL;
	POSITION Position=m_GroupIDMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GroupIDMap.GetNextAssoc(Position,dwGroupID,pGroupItem);
		SafeDelete(pGroupItem);
	}

	//ɾ������
	m_GroupIDMap.RemoveAll();
	m_GroupItemStore.RemoveAll();	

	return;
}

//ö��Ⱥ��
CGroupItem * CGroupInfoManager::EnumGroupItem(POSITION & Position)
{
	//��������
	DWORD dwGroupID=0L;
	CGroupItem * pGroupItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_GroupIDMap.GetStartPosition();
	if (Position!=NULL)	m_GroupIDMap.GetNextAssoc(Position,dwGroupID,pGroupItem);

	return pGroupItem;
}

//����Ⱥ��
CGroupItem * CGroupInfoManager::SearchGroupItem(DWORD dwGroupID)
{
	//���Ҷ���
	CGroupItem * pGroupItem=NULL;
	m_GroupIDMap.Lookup(dwGroupID,pGroupItem);

	return pGroupItem;
}

//�Ƴ�Ⱥ��
bool CGroupInfoManager::RemoveGroupItem()
{
	//ɾ������
	DWORD dwGroupID=0;
	CGroupItem * pGroupItem=NULL;
	POSITION Position=m_GroupIDMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GroupIDMap.GetNextAssoc(Position,dwGroupID,pGroupItem);
		m_GroupItemStore.Add(pGroupItem);
	}

	//�Ƴ�����
	m_GroupIDMap.RemoveAll();	

	return true;
}

//�Ƴ�Ⱥ��
bool CGroupInfoManager::RemoveGroupItem(DWORD dwGroupID)
{
	//���Ҷ���
	CGroupItem * pGroupItem=NULL;
	if (m_GroupIDMap.Lookup(dwGroupID,pGroupItem)==TRUE)
	{
		m_GroupItemStore.Add(pGroupItem);
		m_GroupIDMap.RemoveKey(dwGroupID);

		return true;
	}

	return false;
}

//����Ⱥ��
bool CGroupInfoManager::ActiveGroupItem(tagIMGroupItem & GroupItem)
{
	//��������
	CGroupItem * pGroupItem=NULL;

	//��ȡָ��
	if (m_GroupItemStore.GetCount()>0)
	{
		INT_PTR nIndex=m_GroupItemStore.GetCount()-1;
		pGroupItem=m_GroupItemStore[nIndex];		
		m_GroupItemStore.RemoveAt(nIndex);
		pGroupItem->ResetGroupItem();
	}
	else
	{
		try
		{
			pGroupItem=new CGroupItem;
			if (pGroupItem==NULL) throw TEXT("�ڴ治��,��������ʧ�ܣ�");
		}
		catch (...)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//���Ա���
	CopyMemory(&pGroupItem->m_GroupItem,&GroupItem,sizeof(GroupItem));

	//����Ⱥ��
	m_GroupIDMap[GroupItem.GroupProperty.dwGroupID]=pGroupItem;

	return true;
}
//��ȡ�Ƽ�
WORD CGroupInfoManager::GetRecommendGroup(DWORD dwUserID, WORD wRecomCount, tagIMGroupProperty *pGroupProperty)
{
	if (wRecomCount == 0) return 0;

	//��������
	DWORD dwGroupID = 0L;
	CGroupItem * pGroupItem = NULL;
	tagIMGroupProperty *pTempGroup = new tagIMGroupProperty[m_GroupIDMap.GetCount()];
	WORD wGroupIndex = 0;

	//��ȡ����
	POSITION Position = m_GroupIDMap.GetStartPosition();
	while (Position != NULL)
	{
		m_GroupIDMap.GetNextAssoc(Position, dwGroupID, pGroupItem);

		//�ų��Ѽ����Ⱥ��
		if (pGroupItem->SearchMember(dwUserID) != NULL) continue;

		CopyMemory(&pTempGroup[wGroupIndex], pGroupItem->GetGroupProperty(), sizeof(tagIMGroupProperty));
		wGroupIndex++;
	}

	ASSERT(wGroupIndex <= m_GroupIDMap.GetCount());

	if (wGroupIndex == 0) return 0;

	//�������
	wRecomCount = min(wRecomCount, wGroupIndex);
	CopyMemory(pGroupProperty, pTempGroup, sizeof(tagIMGroupProperty)*wRecomCount);


	return wRecomCount;
}
//��ȡ�Ƽ�
int CGroupInfoManager::CheckValid(){
	return 1;
}
//////////////////////////////////////////////////////////////////////////////////
