#include "StdAfx.h"
#include "ShareStorageItem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

CShareStorageItemArray CShareStorageItem::m_ShareStorageItemActive;
CShareStorageItemArray CShareStorageItem::m_ShareStorageItemBuffer;
CShareStorageItemMap CShareStorageItem::m_ShareStorageItemMap;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CShareStorageItem::CShareStorageItem()
{
	//��ʼ��
	m_ShareStorageItemActive.RemoveAll();
	m_ShareStorageItemBuffer.RemoveAll();
	m_ShareStorageItemMap.InitHashTable(1000);
	m_ShareStorageItemMap.RemoveAll();
}

//��������
CShareStorageItem::~CShareStorageItem(void)
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

//�ӿڲ�ѯ
VOID* CShareStorageItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IShareStorageItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IShareStorageItem,Guid,dwQueryVer);
	return NULL;
}

//�������
bool CShareStorageItem::AddShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem)
{
	//��������
	tagShareStorageItem * pShareStorageItem = NULL;

	//����У��
	ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE);
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

	ASSERT(pShareStorageItem != NULL);
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

//�Ƴ�����
bool CShareStorageItem::RemoveShareStorageItem(ULONGLONG ulItemID)
{
	//��������
	tagShareStorageItem * pShareStorageItem = NULL;

	//����У��
	ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == TRUE);
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
	ASSERT(wIndex != INVALID_WORD);
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

//�޸�����
bool CShareStorageItem::ModifyShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem)
{
	//��������
	tagShareStorageItem * pShareStorageItem = NULL;

	//����У��
	ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == TRUE);
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
	ASSERT(wIndex != INVALID_WORD);
	if (wIndex == INVALID_WORD)
	{
		return false;
	}

	//����
	CopyMemory(pShareStorageItem, &ShareStorageItem, sizeof(tagShareStorageItem));

	return true;
}

//��ȡ����
tagShareStorageItem* CShareStorageItem::GetShareStorageItem(ULONGLONG ulItemID)
{
	//��������
	tagShareStorageItem * pShareStorageItem = NULL;

	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE)
	{
		return NULL;
	}

	return pShareStorageItem;
}

//��ȡ��������
WORD CShareStorageItem::GetShareStorageItemArrayIndex(ULONGLONG ulItemID)
{
	//��������
	tagShareStorageItem * pShareStorageItem = NULL;
	WORD wIndex = INVALID_WORD;

	//����У��
	ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == TRUE);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////
