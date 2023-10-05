#ifndef SHARE_STORAGE_SERVICE_MANAGER_HEAD_FILE
#define SHARE_STORAGE_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ShareStorageServiceHead.h"

////////////////////////////////////////////////////////////////////////

typedef CWHArray<tagShareStorageItem *> CShareStorageItemArray;
typedef CMap<ULONGLONG, ULONGLONG, tagShareStorageItem *, tagShareStorageItem *> CShareStorageItemMap;

//约战服务管理类
class SHARE_STORAGE_SERVICE_CLASS CShareStorageServiceManager : public IShareStorageServiceManager
{	
	//控制变量
protected:
	CShareStorageItemArray				m_ShareStorageItemActive;				//服务激活对象
	CShareStorageItemArray				m_ShareStorageItemBuffer;				//服务存储对象
	CShareStorageItemMap				m_ShareStorageItemMap;					//服务索引

	//函数定义
public:
	//构造函数
	CShareStorageServiceManager();
	//析构函数
	virtual ~CShareStorageServiceManager();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	
	//控制接口
public:
	//停止服务
	virtual bool StopService(WORD wServerID, WORD wKindID, BYTE cbPlayMode);
	//启动服务
	virtual bool StartService(WORD wServerID, WORD wKindID, BYTE cbPlayMode);
	//获取状态
	virtual ENUM_SHARESTORAGE_STATUS GetShareConfigStatus();
	//设置状态
	virtual void SetShareConfigStatus(ENUM_SHARESTORAGE_STATUS emShareConfigStatus);

	//管理接口
public:
	//同步子项
	virtual bool SyncShareStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagShareStorageItem & ShareStorageItem);
	//添加子项
	virtual bool AddSyncShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//修改子项
	virtual bool ModifySyncShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//移除子项
	virtual bool RemoveSyncShareStorageItem(ULONGLONG ulItemID);
	//本地子项
	virtual bool LocalShareStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagShareStorageItem & ShareStorageItem);
	//添加子项
	virtual bool AddShareStorageArrayItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//修改子项
	virtual bool ModifyShareStorageArrayItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//移除子项
	virtual bool RemoveShareStorageArrayItem(ULONGLONG ulItemID);
	//获取子项
	virtual tagShareStorageItem* GetShareStorageItem(ULONGLONG ulItemID);
	//校验同步
	virtual bool EfficacySyncShareStorageItem(WORD wServerID, WORD wKindID, BYTE cbPlayMode);

	//辅助函数
protected:
	//获取数组索引
	WORD GetShareStorageItemArrayIndex(ULONGLONG ulItemID);
	//获取同步索引
	WORD GetSyncShareStorageItemIndex(ULONGLONG ulItemID);
	//校验ServerID
	bool EfficacySyncServerID(tagShareConfig* pShareConfig, WORD wServerID, WORD wKindID, BYTE cbPlayMode);
	//添加ServerID
	bool AddSyncServerID(tagShareConfig* pShareConfig, WORD wServerID, WORD wKindID, BYTE cbPlayMode);
	//删除ServerID
	bool RemoveSyncServerID(tagShareConfig* pShareConfig, WORD wServerID, WORD wKindID, BYTE cbPlayMode);
	//重置存储
	void ResetShareStorage();
};

//////////////////////////////////////////////////////////////////////////
#endif