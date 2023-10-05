#include "StdAfx.h"
#include "ShareStorageItem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

CShareStorageItemArray CShareStorageItem::m_ShareStorageItemActive;
CShareStorageItemArray CShareStorageItem::m_ShareStorageItemBuffer;
CShareStorageItemMap CShareStorageItem::m_ShareStorageItemMap;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CShareStorageItem::CShareStorageItem()
{
	//初始化
	m_ShareStorageItemActive.RemoveAll();
	m_ShareStorageItemBuffer.RemoveAll();
	m_ShareStorageItemMap.InitHashTable(1000);
	m_ShareStorageItemMap.RemoveAll();
}

//析构函数
CShareStorageItem::~CShareStorageItem(void)
{
	//删除存储
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

//接口查询
VOID* CShareStorageItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IShareStorageItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IShareStorageItem,Guid,dwQueryVer);
	return NULL;
}

//添加子项
bool CShareStorageItem::AddShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem)
{
	//变量定义
	tagShareStorageItem * pShareStorageItem = NULL;

	//数据校验
	ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE);
	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem))
	{
		return false;
	}

	//空闲数目
	INT_PTR nFreeItemCount = m_ShareStorageItemBuffer.GetCount();

	//获取对象
	if (nFreeItemCount>0)
	{
		//获取对象
		INT_PTR nItemPostion = nFreeItemCount - 1;
		pShareStorageItem = m_ShareStorageItemBuffer[nItemPostion];

		//删除数组
		m_ShareStorageItemBuffer.RemoveAt(nItemPostion);
	}

	//创建对象
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

	//拷贝数据
	CopyMemory(pShareStorageItem, &ShareStorageItem, sizeof(tagShareStorageItem));

	//设置索引
	m_ShareStorageItemActive.Add(pShareStorageItem);
	m_ShareStorageItemMap[ulItemID] = pShareStorageItem;

	return true;
}

//移除子项
bool CShareStorageItem::RemoveShareStorageItem(ULONGLONG ulItemID)
{
	//变量定义
	tagShareStorageItem * pShareStorageItem = NULL;

	//数据校验
	ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == TRUE);
	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE)
	{
		return false;
	}

	if (pShareStorageItem == NULL)
	{
		return false;
	}

	//获取数组索引
	WORD wIndex = GetShareStorageItemArrayIndex(ulItemID);
	ASSERT(wIndex != INVALID_WORD);
	if (wIndex == INVALID_WORD)
	{
		return false;
	}

	//重置
	ZeroMemory(pShareStorageItem, sizeof(tagShareStorageItem));

	//设置索引
	m_ShareStorageItemMap.RemoveKey(ulItemID);
	m_ShareStorageItemActive.RemoveAt(wIndex);
	m_ShareStorageItemBuffer.Add(pShareStorageItem);

	return true;
}

//修改子项
bool CShareStorageItem::ModifyShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem)
{
	//变量定义
	tagShareStorageItem * pShareStorageItem = NULL;

	//数据校验
	ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == TRUE);
	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE)
	{
		return false;
	}

	if (pShareStorageItem == NULL)
	{
		return false;
	}

	//获取数组索引
	WORD wIndex = GetShareStorageItemArrayIndex(ulItemID);
	ASSERT(wIndex != INVALID_WORD);
	if (wIndex == INVALID_WORD)
	{
		return false;
	}

	//重置
	CopyMemory(pShareStorageItem, &ShareStorageItem, sizeof(tagShareStorageItem));

	return true;
}

//获取子项
tagShareStorageItem* CShareStorageItem::GetShareStorageItem(ULONGLONG ulItemID)
{
	//变量定义
	tagShareStorageItem * pShareStorageItem = NULL;

	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE)
	{
		return NULL;
	}

	return pShareStorageItem;
}

//获取数组索引
WORD CShareStorageItem::GetShareStorageItemArrayIndex(ULONGLONG ulItemID)
{
	//变量定义
	tagShareStorageItem * pShareStorageItem = NULL;
	WORD wIndex = INVALID_WORD;

	//数据校验
	ASSERT(m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == TRUE);
	if (m_ShareStorageItemMap.Lookup(ulItemID, pShareStorageItem) == FALSE)
	{
		return wIndex;
	}

	for (INT_PTR i = 0; i<m_ShareStorageItemActive.GetCount(); i++)
	{
		//获取对象
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
