#ifndef SHARE_STORAGE_ITEM_HEAD_FILE
#define SHARE_STORAGE_ITEM_HEAD_FILE
#pragma once

//引入文件
#include "ShareStorageServiceHead.h"


///////////////////////////////////////////////////////////////////////////////////////////

#define MAKEULONGLONG(a, b)  ((ULONGLONG)(((DWORD)(((ULONGLONG)(a)) & 0xffffffff)) | ((ULONGLONG)((DWORD)(((ULONGLONG)(b)) & 0xffffffff))) << 32))
typedef CWHArray<tagShareStorageItem *> CShareStorageItemArray;
typedef CMap<ULONGLONG, ULONGLONG, tagShareStorageItem *, tagShareStorageItem *> CShareStorageItemMap;
 
//服务子项
class CShareStorageItem : public IShareStorageItem
{
	//共享变量
protected:
	static CShareStorageItemArray			m_ShareStorageItemActive;				//服务激活对象
	static CShareStorageItemArray			m_ShareStorageItemBuffer;				//服务存储对象
	static CShareStorageItemMap				m_ShareStorageItemMap;					//服务索引

	//函数定义
public:
	//构造函数
	CShareStorageItem();
	//析构函数
	virtual ~CShareStorageItem();

	//基础接口
public:
 	//释放对象
 	virtual VOID Release(){ delete this; }
 	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	//添加子项
	virtual bool AddShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//移除子项
	virtual bool RemoveShareStorageItem(ULONGLONG ulItemID);
	//修改子项
	virtual bool ModifyShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//获取子项
	virtual tagShareStorageItem* GetShareStorageItem(ULONGLONG ulItemID);

	//辅助函数
protected:
	//获取数组索引
	WORD GetShareStorageItemArrayIndex(ULONGLONG ulItemID);
};	

#endif