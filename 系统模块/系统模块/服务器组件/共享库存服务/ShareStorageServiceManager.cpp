//�ر���IT������28xin.com��
#include "StdAfx.h"
#include "ShareStorageServiceManager.h"
	
////////////////////////////////////////////////////////////////////////

//������ڴ滺��
#pragma data_seg("SharedBuffer")  
BYTE g_cbSharedBuffer[MAX_SHARED_BUFFER] = { 0 };
CSemaphore g_Semaphore(1, 1);
#pragma data_seg()  
#pragma comment(linker, "/section:SharedBuffer,rws") 

//���캯��
CShareStorageServiceManager::CShareStorageServiceManager()
{
	//��ʼ��
	m_ShareStorageItemActive.RemoveAll();
	m_ShareStorageItemBuffer.RemoveAll();
	m_ShareStorageItemMap.InitHashTable(1000);
	m_ShareStorageItemMap.RemoveAll();
}

//��������
CShareStorageServiceManager::~CShareStorageServiceManager()
{	
	//���ô洢
	ResetShareStorage();
}

//�ӿڲ�ѯ
void * CShareStorageServiceManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IShareStorageServiceManager, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IShareStorageServiceManager, Guid, dwQueryVer);
	return NULL;
}

//ֹͣ����
bool CShareStorageServiceManager::StopService(WORD wServerID, WORD wKindID, BYTE cbPlayMode)
{
	g_Semaphore.Lock();

	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;
	bool bRemoveSync = RemoveSyncServerID(pShareConfig, wServerID, wKindID, cbPlayMode);
	if (bRemoveSync)
	{
		ULONGLONG *pUlItemIDArray = new ULONGLONG[m_ShareStorageItemActive.GetCount()];
		ZeroMemory(pUlItemIDArray, sizeof(ULONGLONG) * m_ShareStorageItemActive.GetCount());
		WORD wUlItemIDCount = 0;

		for (INT_PTR i = 0; i<m_ShareStorageItemActive.GetCount(); i++)
		{
			//��ȡ����
			tagShareStorageItem * pShareStorageItem = m_ShareStorageItemActive[i];

			if (pShareStorageItem == NULL)
			{
				continue;
			}

			if (pShareStorageItem->wKindID == wKindID && pShareStorageItem->cbPlayMode == cbPlayMode)
			{
				pUlItemIDArray[wUlItemIDCount++] = pShareStorageItem->ulItemID;
			}
		}

		//���ô洢
		ResetShareStorage();

		tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;
		bool bEfficacySyncShareStorageItem = false;

		if (!pShareConfig || pShareConfig->wLoadServerCount == 0)
		{
			bEfficacySyncShareStorageItem = false;
		}
		else
		{
			for (WORD i = 0; i < pShareConfig->wLoadServerCount; i++)
			{
				if (pShareConfig->tagServerInfoArray[i].wKindID == wKindID && pShareConfig->tagServerInfoArray[i].cbPlayMode == cbPlayMode
					&& pShareConfig->tagServerInfoArray[i].wServerID != wServerID)
				{
					bEfficacySyncShareStorageItem = true;
					break;
				}
			}
		}

		//
		if (wUlItemIDCount > 0 && !bEfficacySyncShareStorageItem)
		{
			for (INT_PTR i = 0; i < wUlItemIDCount; i++)
			{
				pShareConfig->emShareConfigStatus = _REMOVE_SUBITEM_;
				RemoveSyncShareStorageItem(pUlItemIDArray[i]);
			}
		}

		delete[] pUlItemIDArray;
	}

	g_Semaphore.Unlock();

	return bRemoveSync;
}

//��������
bool CShareStorageServiceManager::StartService(WORD wServerID, WORD wKindID, BYTE cbPlayMode)
{
	g_Semaphore.Lock();

	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;
	bool bAddSync = AddSyncServerID(pShareConfig, wServerID, wKindID, cbPlayMode);

	g_Semaphore.Unlock();

	return bAddSync;
}

//��ȡ״̬
ENUM_SHARESTORAGE_STATUS CShareStorageServiceManager::GetShareConfigStatus()
{
	//��������
	ENUM_SHARESTORAGE_STATUS emShareConfigStatus = _NULL_STATUS_;

	g_Semaphore.Lock();
	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;
	emShareConfigStatus = pShareConfig->emShareConfigStatus;
	g_Semaphore.Unlock();

	return emShareConfigStatus;
}

//����״̬
void CShareStorageServiceManager::SetShareConfigStatus(ENUM_SHARESTORAGE_STATUS emShareConfigStatus)
{
	g_Semaphore.Lock();
	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;
	if (pShareConfig->emShareConfigStatus != emShareConfigStatus)
	{
		pShareConfig->emShareConfigStatus = emShareConfigStatus;
	}
	g_Semaphore.Unlock();
}

//ͬ������
bool CShareStorageServiceManager::SyncShareStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagShareStorageItem & ShareStorageItem)
{
	g_Semaphore.Lock();
	bool bSyncResult = false;
	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;

	if (pShareConfig->emShareConfigStatus != emShareConfigStatus)
	{
		pShareConfig->emShareConfigStatus = emShareConfigStatus;

		switch (emShareConfigStatus)
		{
		case _ADD_SUBITEM_:
		{
			AddSyncShareStorageItem(ShareStorageItem.ulItemID, ShareStorageItem);
			bSyncResult = true;
			break;
		}
		case _MODIFY_SUBITEM_:
		{
			ModifySyncShareStorageItem(ShareStorageItem.ulItemID, ShareStorageItem);
			bSyncResult = true;
			break;
		}
		case _REMOVE_SUBITEM_:
		{
			RemoveSyncShareStorageItem(ShareStorageItem.ulItemID);
			bSyncResult = true;
			break;
		}
		default:
			bSyncResult = false;
			break;
		}
	}

	g_Semaphore.Unlock();

	return bSyncResult;
}

//�������
bool CShareStorageServiceManager::AddSyncShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem)
{
	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;
	if (pShareConfig && pShareConfig->emShareConfigStatus == _ADD_SUBITEM_)
	{
		tagShareStorageItem *pShareStorageItem = (tagShareStorageItem *)(pShareConfig + 1);
		CopyMemory((pShareStorageItem + pShareConfig->wShareStorageItemCount), &ShareStorageItem, sizeof(tagShareStorageItem));
		pShareConfig->wShareStorageItemCount++;
		pShareConfig->emShareConfigStatus = _NULL_STATUS_;
	}

	return true;
}

//�޸�����
bool CShareStorageServiceManager::ModifySyncShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem)
{
	//��ȡͬ������
	WORD wIndex = GetSyncShareStorageItemIndex(ulItemID);
	//ASSERT(wIndex != INVALID_WORD);
	if (wIndex == INVALID_WORD)
	{
		return false;
	}

	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;
	if (pShareConfig && pShareConfig->emShareConfigStatus == _MODIFY_SUBITEM_)
	{
		tagShareStorageItem *pShareStorageItem = (tagShareStorageItem *)(pShareConfig + 1);
		CopyMemory((pShareStorageItem + wIndex), &ShareStorageItem, sizeof(tagShareStorageItem));
		pShareConfig->emShareConfigStatus = _NULL_STATUS_;
	}

	return true;
}

//�Ƴ�����
bool CShareStorageServiceManager::RemoveSyncShareStorageItem(ULONGLONG ulItemID)
{
	//��ȡͬ������
	WORD wIndex = GetSyncShareStorageItemIndex(ulItemID);
	//ASSERT(wIndex != INVALID_WORD);
	if (wIndex == INVALID_WORD)
	{
		return false;
	}

	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;
	if (pShareConfig && pShareConfig->emShareConfigStatus == _REMOVE_SUBITEM_)
	{
		tagShareStorageItem *pShareStorageItem = (tagShareStorageItem *)(pShareConfig + 1);
		WORD wMoveItemCount = pShareConfig->wShareStorageItemCount - wIndex - 1;
		memmove(pShareStorageItem + wIndex, pShareStorageItem + wIndex + 1, sizeof(tagShareStorageItem)* wMoveItemCount);
		pShareConfig->wShareStorageItemCount--;
		pShareConfig->emShareConfigStatus = _NULL_STATUS_;
	}

	return true;
}

//��������
bool CShareStorageServiceManager::LocalShareStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagShareStorageItem & ShareStorageItem)
{
	switch (emShareConfigStatus)
	{
	case _ADD_SUBITEM_:
	{
		AddShareStorageArrayItem(ShareStorageItem.ulItemID, ShareStorageItem);
		return true;
	}
	case _MODIFY_SUBITEM_:
	{
		ModifyShareStorageArrayItem(ShareStorageItem.ulItemID, ShareStorageItem);
		return true;
	}
	case _REMOVE_SUBITEM_:
	{
		RemoveShareStorageArrayItem(ShareStorageItem.ulItemID);
		return true;
	}
	default:
		return false;
	}

	return false;
}

//�������
bool CShareStorageServiceManager::AddShareStorageArrayItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem)
{
	//��������
	tagShareStorageItem * pShareStorageItem = NULL;

	//����У��
	//ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE);
	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem))
	{
		return false;
	}

	//������Ŀ
	INT_PTR nFreeItemCount = m_ShareStorageItemBuffer.GetCount();

	//��ȡ����
	if (nFreeItemCount>0)
	{
		//��ȡ����
		INT_PTR nItemPostion = nFreeItemCount - 1;
		pShareStorageItem = m_ShareStorageItemBuffer[nItemPostion];

		//ɾ������
		m_ShareStorageItemBuffer.RemoveAt(nItemPostion);
	}

	//��������
	if (pShareStorageItem == NULL)
	{
		pShareStorageItem = new tagShareStorageItem;
		ZeroMemory(pShareStorageItem, sizeof(tagShareStorageItem));
	}

	//ASSERT(pShareStorageItem != NULL);
	if (pShareStorageItem == NULL)
	{
		return false;
	}

	//��������
	CopyMemory(pShareStorageItem, &ShareStorageItem, sizeof(tagShareStorageItem));

	//��������
	m_ShareStorageItemActive.Add(pShareStorageItem);
	m_ShareStorageItemMap[ulItemID] = pShareStorageItem;

	return true;
}

//�޸�����
bool CShareStorageServiceManager::ModifyShareStorageArrayItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem)
{
	//��������
	tagShareStorageItem * pShareStorageItem = NULL;

	//����У��
	//ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == TRUE);
	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE)
	{
		return false;
	}

	if (pShareStorageItem == NULL)
	{
		return false;
	}

	//����
	CopyMemory(pShareStorageItem, &ShareStorageItem, sizeof(tagShareStorageItem));

	return true;
}

//�Ƴ�����
bool CShareStorageServiceManager::RemoveShareStorageArrayItem(ULONGLONG ulItemID)
{
	//��������
	tagShareStorageItem * pShareStorageItem = NULL;

	//����У��
	//ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == TRUE);
	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE)
	{
		return false;
	}

	if (pShareStorageItem == NULL)
	{
		return false;
	}

	//��ȡ��������
	WORD wIndex = GetShareStorageItemArrayIndex(ulItemID);
	//ASSERT(wIndex != INVALID_WORD);
	if (wIndex == INVALID_WORD)
	{
		return false;
	}

	//����
	ZeroMemory(pShareStorageItem, sizeof(tagShareStorageItem));

	//��������
	m_ShareStorageItemMap.RemoveKey(ulItemID);
	m_ShareStorageItemActive.RemoveAt(wIndex);
	m_ShareStorageItemBuffer.Add(pShareStorageItem);

	return true;
}

//��ȡ����
tagShareStorageItem* CShareStorageServiceManager::GetShareStorageItem(ULONGLONG ulItemID)
{
	//��������
	tagShareStorageItem * pShareStorageItem = NULL;

	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE)
	{
		return NULL;
	}

	//����ͬ��
	g_Semaphore.Lock();

	//��ȡͬ������
	WORD wIndex = GetSyncShareStorageItemIndex(ulItemID);
	//ASSERT(wIndex != INVALID_WORD);
	if (wIndex == INVALID_WORD)
	{
		g_Semaphore.Unlock();

		return NULL;
	}

	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;
	tagShareStorageItem *pSyncShareStorageItem = (tagShareStorageItem *)(pShareConfig + 1);
	pSyncShareStorageItem += wIndex;

	if (pShareConfig && pSyncShareStorageItem)
	{
		CopyMemory(pShareStorageItem, pSyncShareStorageItem, sizeof(tagShareStorageItem));
	}

	g_Semaphore.Unlock();

	return pShareStorageItem;
}

//У��ͬ��
bool CShareStorageServiceManager::EfficacySyncShareStorageItem(WORD wServerID, WORD wKindID, BYTE cbPlayMode)
{
	g_Semaphore.Lock();
	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;

	if (!pShareConfig || pShareConfig->wLoadServerCount == 0)
	{
		g_Semaphore.Unlock();

		return false;
	}

	for (WORD i = 0; i < pShareConfig->wLoadServerCount; i++)
	{
		if (pShareConfig->tagServerInfoArray[i].wKindID == wKindID && pShareConfig->tagServerInfoArray[i].cbPlayMode == cbPlayMode 
		 && pShareConfig->tagServerInfoArray[i].wServerID != wServerID)
		{
			g_Semaphore.Unlock();

			return true;
		}
	}

	g_Semaphore.Unlock();

	return false;
}

//��ȡ��������
WORD CShareStorageServiceManager::GetShareStorageItemArrayIndex(ULONGLONG ulItemID)
{
	//��������
	tagShareStorageItem * pShareStorageItem = NULL;
	WORD wIndex = INVALID_WORD;

	//����У��
	//ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == TRUE);
	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE)
	{
		return wIndex;
	}

	for (INT_PTR i = 0; i<m_ShareStorageItemActive.GetCount(); i++)
	{
		//��ȡ����
		pShareStorageItem = m_ShareStorageItemActive[i];

		if (pShareStorageItem == NULL)
		{
			continue;
		}

		if (pShareStorageItem->ulItemID == ulItemID)
		{
			wIndex = i;
			break;
		}
	}

	return wIndex;
}

//��ȡͬ������
WORD CShareStorageServiceManager::GetSyncShareStorageItemIndex(ULONGLONG ulItemID)
{
	tagShareConfig* pShareConfig = (tagShareConfig*)g_cbSharedBuffer;
	tagShareStorageItem * pShareStorageItem = (tagShareStorageItem *)(pShareConfig + 1);
	
	WORD wIndex = INVALID_WORD;
	for (INT_PTR i = 0; i<pShareConfig->wShareStorageItemCount; i++, pShareStorageItem++)
	{
		//��ȡ����
		if (pShareStorageItem->ulItemID == ulItemID)
		{
			wIndex = i;
			break;
		}
	}

	return wIndex;
}

//У��ServerID
bool CShareStorageServiceManager::EfficacySyncServerID(tagShareConfig* pShareConfig, WORD wServerID, WORD wKindID, BYTE cbPlayMode)
{
	if (!pShareConfig || pShareConfig->wLoadServerCount == 0)
	{
		return false;
	}

	for (WORD i = 0; i < pShareConfig->wLoadServerCount; i++)
	{
		if (pShareConfig->tagServerInfoArray[i].wServerID == wServerID && pShareConfig->tagServerInfoArray[i].wKindID == wKindID
		 && pShareConfig->tagServerInfoArray[i].cbPlayMode == cbPlayMode)
		{
			return true;
		}
	}

	return false;
}

//���ServerID
bool CShareStorageServiceManager::AddSyncServerID(tagShareConfig* pShareConfig, WORD wServerID, WORD wKindID, BYTE cbPlayMode)
{
	if (EfficacySyncServerID(pShareConfig, wServerID, wKindID, cbPlayMode))
	{
		return false;
	}

	WORD wLoadServerCount = pShareConfig->wLoadServerCount;
	pShareConfig->tagServerInfoArray[wLoadServerCount].wServerID = wServerID;
	pShareConfig->tagServerInfoArray[wLoadServerCount].wKindID = wKindID;
	pShareConfig->tagServerInfoArray[wLoadServerCount].cbPlayMode = cbPlayMode;

	pShareConfig->wLoadServerCount++;

	return true;
}

//ɾ��ServerID
bool CShareStorageServiceManager::RemoveSyncServerID(tagShareConfig* pShareConfig, WORD wServerID, WORD wKindID, BYTE cbPlayMode)
{
	if (!EfficacySyncServerID(pShareConfig, wServerID, wKindID, cbPlayMode))
	{
		return false;
	}

	WORD wMoveIndex = INVALID_WORD;
	WORD wMoveCount = 0;
	for (WORD i = 0; i < pShareConfig->wLoadServerCount; i++)
	{
		if (pShareConfig->tagServerInfoArray[i].wServerID == wServerID && pShareConfig->tagServerInfoArray[i].wKindID == wKindID
			&& pShareConfig->tagServerInfoArray[i].cbPlayMode == cbPlayMode)
		{
			wMoveIndex = i;
			break;
		}
	}

	//ASSERT(wMoveIndex != INVALID_WORD);
	wMoveCount = pShareConfig->wLoadServerCount - wMoveIndex - 1;

	if (wMoveCount > 0)
	{
		memmove(pShareConfig->tagServerInfoArray + wMoveIndex, pShareConfig->tagServerInfoArray + wMoveIndex + 1, sizeof(WORD)* wMoveCount);
	}

	WORD wLastServerIndex = --(pShareConfig->wLoadServerCount);
	ZeroMemory(&(pShareConfig->tagServerInfoArray[wLastServerIndex]), sizeof(tagShareServerInfo));

	return true;
}

//���ô洢
void CShareStorageServiceManager::ResetShareStorage()
{
	//ɾ���洢
	for (INT_PTR i = 0; i<m_ShareStorageItemActive.GetCount(); i++)
	{
		SafeDelete(m_ShareStorageItemActive[i]);
	}

	for (INT_PTR i = 0; i<m_ShareStorageItemBuffer.GetCount(); i++)
	{
		SafeDelete(m_ShareStorageItemBuffer[i]);
	}

	m_ShareStorageItemActive.RemoveAll();
	m_ShareStorageItemBuffer.RemoveAll();
	m_ShareStorageItemMap.RemoveAll();
}

////////////////////////////////////////////////////////////////////////
