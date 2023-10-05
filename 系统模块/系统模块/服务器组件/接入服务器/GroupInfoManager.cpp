#include "StdAfx.h"
#include "GroupInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CGroupItem::CGroupItem()
{
	//设置变量	
	m_GroupMemberArray.RemoveAll();
	ZeroMemory(&m_GroupItem,sizeof(m_GroupItem));
	m_Notice = L"";
}

//析构函数
CGroupItem::~CGroupItem()
{
	//设置变量	
	ResetGroupItem();
}

//移除成员
bool CGroupItem::RemoveMember(DWORD dwMemberID)
{
	//查找并移除
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

//添加成员
bool CGroupItem::AddMember(tagIMGroupMember & GroupMember)
{
	//查找成员
	tagIMGroupMember *pGroupMember=SearchMember(GroupMember.dwMemberID);
	if (pGroupMember!=NULL)
	{
		//更新信息
		CopyMemory(pGroupMember,&GroupMember,sizeof(GroupMember));
	}
	else
	{
		m_GroupMemberArray.Add(GroupMember);		
	}

	return true;
}

//枚举成员
tagIMGroupMember * CGroupItem::EnumMember(INT nIndex)
{
	if (nIndex>=m_GroupMemberArray.GetCount()) return NULL;
	return &m_GroupMemberArray[nIndex];
}

//查找成员
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
	//查找成员
	tagClubRollMsg *pGroupMember = SearchRollMsg(RollMsg.dwUserID);
	if (pGroupMember != NULL)
	{
		//更新信息
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

//查找成员
bool CGroupItem::AddPersonalTable(DWORD dwPersonalRoomID, tagClubTableInfo TableInfo)
{
	m_MapClubTableInfo.SetAt(dwPersonalRoomID, TableInfo);

	return true;
}

//移除成员
bool CGroupItem::RemovePersonalTable(DWORD dwPersonalRoomID)
{
	//查找并移除
	m_MapClubTableInfo.RemoveKey(dwPersonalRoomID);

	return true;
}

//移除成员
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

//移除成员
bool CGroupItem::GetTableInfo(DWORD dwPersonalRoomID, tagClubTableInfo &TableInfo)
{
	return m_MapClubTableInfo.Lookup(dwPersonalRoomID, TableInfo);
}

//移除成员
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

//移除成员
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

//移除成员
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

//枚举成员
tagGroupConfigInfo * CGroupItem::EnumConfig(INT nIndex)
{
	if (nIndex >= m_GroupConfigArray.GetCount()) return NULL;
	return &m_GroupConfigArray[nIndex];
}

//查找成员
bool CGroupItem::AddConfig(tagGroupConfigInfo & GroupConfig)
{
	//查找成员
	tagGroupConfigInfo *pGroupConfig = SearchConfig(GroupConfig.dwConfigID);
	if (pGroupConfig != NULL)
	{
		//更新信息
		CopyMemory(pGroupConfig, &GroupConfig, sizeof(GroupConfig));
	}
	else
	{
		m_GroupConfigArray.Add(GroupConfig);
	}

	return true;
}

//移除成员
bool CGroupItem::ModifyConfig(tagGroupConfigInfo & GroupConfig)
{
	return AddConfig(GroupConfig);
}

//移除成员
bool CGroupItem::RemoveConfig(DWORD dwConfigID)
{
	//查找并移除
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

//移除成员
bool CGroupItem::RemoveConfig(INT nIndex)
{
	if (nIndex<m_GroupConfigArray.GetCount())
	{
		m_GroupConfigArray.RemoveAt(nIndex);
		return true;
	}

	return false;
}

//查找成员
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

//枚举成员
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

//枚举成员
tagClubTableInfo CGroupItem::EnumTable(POSITION &pos)
{
	//获取对象
	//zhaozw TODO
	tagClubTableInfo TableInfo;
	DWORD dwGroupID = 0;
	if (pos == NULL) pos = m_MapClubTableInfo.GetStartPosition();
	if (pos != NULL)	m_MapClubTableInfo.GetNextAssoc(pos, dwGroupID, TableInfo);
	//m_MapClubTableInfo..Lookup(dwPersonalRoomID, TableInfo)
	//ZeroMemory(&ClubTableInfo, sizeof(ClubTableInfo));
	return TableInfo;
}

//重置对象
void CGroupItem::ResetGroupItem()
{
	//设置变量	
	m_GroupMemberArray.RemoveAll();
	ZeroMemory(&m_GroupItem,sizeof(m_GroupItem));
}

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CGroupInfoManager::CGroupInfoManager()
{
	//设置质数
	m_GroupIDMap.InitHashTable(10003L);
}

//析构函数
CGroupInfoManager::~CGroupInfoManager()
{
	//释放用户
	for (INT_PTR i=0;i<m_GroupItemStore.GetCount();i++)
	{
		CGroupItem * pGroupItem = m_GroupItemStore[i];
		SafeDelete(pGroupItem);
	}

	//删除接入
	DWORD dwGroupID=0;
	CGroupItem * pGroupItem=NULL;
	POSITION Position=m_GroupIDMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GroupIDMap.GetNextAssoc(Position,dwGroupID,pGroupItem);
		SafeDelete(pGroupItem);
	}

	//删除数据
	m_GroupIDMap.RemoveAll();
	m_GroupItemStore.RemoveAll();	

	return;
}

//枚举群组
CGroupItem * CGroupInfoManager::EnumGroupItem(POSITION & Position)
{
	//变量定义
	DWORD dwGroupID=0L;
	CGroupItem * pGroupItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_GroupIDMap.GetStartPosition();
	if (Position!=NULL)	m_GroupIDMap.GetNextAssoc(Position,dwGroupID,pGroupItem);

	return pGroupItem;
}

//查找群组
CGroupItem * CGroupInfoManager::SearchGroupItem(DWORD dwGroupID)
{
	//查找对象
	CGroupItem * pGroupItem=NULL;
	m_GroupIDMap.Lookup(dwGroupID,pGroupItem);

	return pGroupItem;
}

//移除群组
bool CGroupInfoManager::RemoveGroupItem()
{
	//删除接入
	DWORD dwGroupID=0;
	CGroupItem * pGroupItem=NULL;
	POSITION Position=m_GroupIDMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GroupIDMap.GetNextAssoc(Position,dwGroupID,pGroupItem);
		m_GroupItemStore.Add(pGroupItem);
	}

	//移除对象
	m_GroupIDMap.RemoveAll();	

	return true;
}

//移除群组
bool CGroupInfoManager::RemoveGroupItem(DWORD dwGroupID)
{
	//查找对象
	CGroupItem * pGroupItem=NULL;
	if (m_GroupIDMap.Lookup(dwGroupID,pGroupItem)==TRUE)
	{
		m_GroupItemStore.Add(pGroupItem);
		m_GroupIDMap.RemoveKey(dwGroupID);

		return true;
	}

	return false;
}

//插入群组
bool CGroupInfoManager::ActiveGroupItem(tagIMGroupItem & GroupItem)
{
	//变量定义
	CGroupItem * pGroupItem=NULL;

	//获取指针
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
			if (pGroupItem==NULL) throw TEXT("内存不足,创建对象失败！");
		}
		catch (...)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//属性变量
	CopyMemory(&pGroupItem->m_GroupItem,&GroupItem,sizeof(GroupItem));

	//插入群组
	m_GroupIDMap[GroupItem.GroupProperty.dwGroupID]=pGroupItem;

	return true;
}
//获取推荐
WORD CGroupInfoManager::GetRecommendGroup(DWORD dwUserID, WORD wRecomCount, tagIMGroupProperty *pGroupProperty)
{
	if (wRecomCount == 0) return 0;

	//变量定义
	DWORD dwGroupID = 0L;
	CGroupItem * pGroupItem = NULL;
	tagIMGroupProperty *pTempGroup = new tagIMGroupProperty[m_GroupIDMap.GetCount()];
	WORD wGroupIndex = 0;

	//获取对象
	POSITION Position = m_GroupIDMap.GetStartPosition();
	while (Position != NULL)
	{
		m_GroupIDMap.GetNextAssoc(Position, dwGroupID, pGroupItem);

		//排除已加入的群组
		if (pGroupItem->SearchMember(dwUserID) != NULL) continue;

		CopyMemory(&pTempGroup[wGroupIndex], pGroupItem->GetGroupProperty(), sizeof(tagIMGroupProperty));
		wGroupIndex++;
	}

	ASSERT(wGroupIndex <= m_GroupIDMap.GetCount());

	if (wGroupIndex == 0) return 0;

	//输出排序
	wRecomCount = min(wRecomCount, wGroupIndex);
	CopyMemory(pGroupProperty, pTempGroup, sizeof(tagIMGroupProperty)*wRecomCount);


	return wRecomCount;
}
//获取推荐
int CGroupInfoManager::CheckValid(){
	return 1;
}
//////////////////////////////////////////////////////////////////////////////////
