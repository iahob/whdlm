#include "StdAfx.h"
#include "ServerListManager.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameServerItem::CGameServerItem()
{
	//设置变量
	ZeroMemory(&m_GameServer,sizeof(m_GameServer));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerListManager::CServerListManager()
{
	//设置质数
	m_ServerItemMap.InitHashTable(PRIME_SERVER);

	return;
}

//析构函数
CServerListManager::~CServerListManager()
{
	//变量定义
	WORD wKey=0;

	//删除房间
	CGameServerItem * pGameServerItem=NULL;
	POSITION Position=m_ServerItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);
		SafeDelete(pGameServerItem);
	}

	for (INT_PTR i=0;i<m_ServerItemBuffer.GetCount();i++)
	{
		pGameServerItem=m_ServerItemBuffer[i];
		SafeDelete(pGameServerItem);
	}

	m_ServerItemMap.RemoveAll();
	m_ServerItemBuffer.RemoveAll();

	return;
}

//重置列表
VOID CServerListManager::ResetServerList()
{
	//废弃列表
	DisuseServerItem();

	//清理列表
	CleanServerItem();

	return;
}

//清理房间
VOID CServerListManager::CleanServerItem()
{
	//变量定义
	POSITION PositionServer=m_ServerItemMap.GetStartPosition();

	//删除房间
	while (PositionServer!=NULL)
	{
		//索引房间
		WORD wServerID=0;
		CGameServerItem * pGameServerItem=NULL;
		m_ServerItemMap.GetNextAssoc(PositionServer,wServerID,pGameServerItem);

		//删除判断
		if (pGameServerItem->m_bDisuse==true)
		{
			//删除数据
			m_ServerItemMap.RemoveKey(wServerID);
			m_ServerItemBuffer.Add(pGameServerItem);
		}
	}

	return;
}

//废弃房间
VOID CServerListManager::DisuseServerItem()
{
	//变量定义
	WORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;

	//废弃房间
	POSITION Position=m_ServerItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);
		pGameServerItem->m_bDisuse=true;
	}

	return;
}

//插入房间
bool CServerListManager::InsertGameServer(tagGameServer * pGameServer)
{
	//效验参数
	//ASSERT(pGameServer!=NULL);
	if (pGameServer==NULL) return false;

	//过滤非约战
	if (pGameServer->wServerType != GAME_GENRE_PERSONAL)
	{
		return true;
	}

	//查找房间
	WORD wKindID=0;
	DWORD dwOnLineCount=0L;
	DWORD dwAndroidCount=0L;
	DWORD dwMaxPlayer=0L;
	CGameServerItem * pGameServerItem=NULL;

	//获取子项
	if (m_ServerItemMap.Lookup(pGameServer->wServerID,pGameServerItem)==FALSE)
	{
		//创建对象
		try
		{
			INT_PTR nStroeCount=m_ServerItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameServerItem=m_ServerItemBuffer[nStroeCount-1];
				m_ServerItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameServerItem=new CGameServerItem;
				if (pGameServerItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//设置变量
		ZeroMemory(pGameServerItem,sizeof(CGameServerItem));
	}

	//设置数据
	pGameServerItem->m_bDisuse=false;
	CopyMemory(&pGameServerItem->m_GameServer,pGameServer,sizeof(tagGameServer));

	//设置索引
	m_ServerItemMap[pGameServer->wServerID]=pGameServerItem;

	return true;
}

//删除房间
bool CServerListManager::DeleteGameServer(WORD wServerID)
{
	//查找房间
	CGameServerItem * pGameServerItem=NULL;
	if (m_ServerItemMap.Lookup(wServerID,pGameServerItem)==FALSE) return false;

	//删除数据
	m_ServerItemMap.RemoveKey(wServerID);
	m_ServerItemBuffer.Add(pGameServerItem);

	return false;
}

//枚举房间
CGameServerItem * CServerListManager::EmunGameServerItem(POSITION & Position)
{
	//变量定义
	WORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_ServerItemMap.GetStartPosition();
	if (Position!=NULL) m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);

	return pGameServerItem;
}

//查找房间
CGameServerItem * CServerListManager::SearchGameServer(WORD wServerID)
{
	CGameServerItem * pGameServerItem=NULL;
	m_ServerItemMap.Lookup(wServerID,pGameServerItem);
	return pGameServerItem;
}

//校验大联盟百人游戏
bool CServerListManager::IsGroupPersonalMaxChairGame(WORD wKindID)
{
	//变量定义
	POSITION Position = NULL;
	CGameServerItem * pGameServerItem = NULL;

	//枚举数据
	for (DWORD i = 0; i < GetGameServerCount(); i++)
	{
		//获取数据
		pGameServerItem = EmunGameServerItem(Position);
		if (pGameServerItem == NULL)
		{
			break;
		}

		if (pGameServerItem->m_GameServer.wKindID == wKindID)
		{
			return pGameServerItem->m_GameServer.bGroupPersonalMaxChair;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
