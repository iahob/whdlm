#include "StdAfx.h"
#include "ServerListManager.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerItem::CGameServerItem()
{
	//���ñ���
	ZeroMemory(&m_GameServer,sizeof(m_GameServer));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerListManager::CServerListManager()
{
	//��������
	m_ServerItemMap.InitHashTable(PRIME_SERVER);

	return;
}

//��������
CServerListManager::~CServerListManager()
{
	//��������
	WORD wKey=0;

	//ɾ������
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

//�����б�
VOID CServerListManager::ResetServerList()
{
	//�����б�
	DisuseServerItem();

	//�����б�
	CleanServerItem();

	return;
}

//������
VOID CServerListManager::CleanServerItem()
{
	//��������
	POSITION PositionServer=m_ServerItemMap.GetStartPosition();

	//ɾ������
	while (PositionServer!=NULL)
	{
		//��������
		WORD wServerID=0;
		CGameServerItem * pGameServerItem=NULL;
		m_ServerItemMap.GetNextAssoc(PositionServer,wServerID,pGameServerItem);

		//ɾ���ж�
		if (pGameServerItem->m_bDisuse==true)
		{
			//ɾ������
			m_ServerItemMap.RemoveKey(wServerID);
			m_ServerItemBuffer.Add(pGameServerItem);
		}
	}

	return;
}

//��������
VOID CServerListManager::DisuseServerItem()
{
	//��������
	WORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;

	//��������
	POSITION Position=m_ServerItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);
		pGameServerItem->m_bDisuse=true;
	}

	return;
}

//���뷿��
bool CServerListManager::InsertGameServer(tagGameServer * pGameServer)
{
	//Ч�����
	//ASSERT(pGameServer!=NULL);
	if (pGameServer==NULL) return false;

	//���˷�Լս
	if (pGameServer->wServerType != GAME_GENRE_PERSONAL)
	{
		return true;
	}

	//���ҷ���
	WORD wKindID=0;
	DWORD dwOnLineCount=0L;
	DWORD dwAndroidCount=0L;
	DWORD dwMaxPlayer=0L;
	CGameServerItem * pGameServerItem=NULL;

	//��ȡ����
	if (m_ServerItemMap.Lookup(pGameServer->wServerID,pGameServerItem)==FALSE)
	{
		//��������
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

		//���ñ���
		ZeroMemory(pGameServerItem,sizeof(CGameServerItem));
	}

	//��������
	pGameServerItem->m_bDisuse=false;
	CopyMemory(&pGameServerItem->m_GameServer,pGameServer,sizeof(tagGameServer));

	//��������
	m_ServerItemMap[pGameServer->wServerID]=pGameServerItem;

	return true;
}

//ɾ������
bool CServerListManager::DeleteGameServer(WORD wServerID)
{
	//���ҷ���
	CGameServerItem * pGameServerItem=NULL;
	if (m_ServerItemMap.Lookup(wServerID,pGameServerItem)==FALSE) return false;

	//ɾ������
	m_ServerItemMap.RemoveKey(wServerID);
	m_ServerItemBuffer.Add(pGameServerItem);

	return false;
}

//ö�ٷ���
CGameServerItem * CServerListManager::EmunGameServerItem(POSITION & Position)
{
	//��������
	WORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_ServerItemMap.GetStartPosition();
	if (Position!=NULL) m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);

	return pGameServerItem;
}

//���ҷ���
CGameServerItem * CServerListManager::SearchGameServer(WORD wServerID)
{
	CGameServerItem * pGameServerItem=NULL;
	m_ServerItemMap.Lookup(wServerID,pGameServerItem);
	return pGameServerItem;
}

//У������˰�����Ϸ
bool CServerListManager::IsGroupPersonalMaxChairGame(WORD wKindID)
{
	//��������
	POSITION Position = NULL;
	CGameServerItem * pGameServerItem = NULL;

	//ö������
	for (DWORD i = 0; i < GetGameServerCount(); i++)
	{
		//��ȡ����
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
