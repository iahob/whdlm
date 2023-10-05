//藏宝库IT社区（28xin.com）
#ifndef SHARE_STORAGE_SERVICE_HEAD_FILE
#define SHARE_STORAGE_SERVICE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//平台定义
#include "..\..\全局定义\Platform.h"

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_Correspond.h"
#include "..\..\消息定义\CMD_GameServer.h"

//平台文件
#include "..\..\公共组件\服务核心\ServiceCoreHead.h"
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

#ifdef _DEBUG		
#define		MAX_SHARED_BUFFER				10240000
#else
#define		MAX_SHARED_BUFFER				102400000
#endif

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef SHARE_STORAGE_SERVICE_CLASS
	#ifdef  SHARE_STORAGE_SERVICE_DLL
		#define SHARE_STORAGE_SERVICE_CLASS _declspec(dllexport)
	#else
		#define SHARE_STORAGE_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define SHARE_STORAGE_SERVICE_DLL_NAME		TEXT("ShareStorageService.dll")			//组件名字
#else
	#define SHARE_STORAGE_SERVICE_DLL_NAME		TEXT("ShareStorageServiceD.dll")		//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//约战房参数
struct tagShareStorageManagerParameter
{
	tagGameServiceOption *			pGameServiceOption;					//服务配置
	tagGameServiceAttrib *			pGameServiceAttrib;					//服务属性
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IShareStorageServiceManager	INTERFACE_VERSION(1,1)
static const GUID IID_IShareStorageServiceManager = { 0x55f92262, 0x191e, 0x461e, 0x96, 0x2a, 0xe7, 0x14, 0xaf, 0x1e, 0x85, 0x92 };
#else
#define VER_IShareStorageServiceManager	INTERFACE_VERSION(1,1)
static const GUID IID_IShareStorageServiceManager={0x35053879, 0x45ed, 0x4c8c, 0xbe, 0x1, 0x1f, 0xa6, 0x29, 0x17, 0x25, 0x5c};
#endif

//共享库存服务接口
interface IShareStorageServiceManager : public IUnknownEx
{
	//控制接口
public:
	//停止服务
	virtual bool StopService(WORD wServerID, WORD wKindID, BYTE cbPlayMode) = NULL;
	//启动服务
	virtual bool StartService(WORD wServerID, WORD wKindID, BYTE cbPlayMode) = NULL;
	//获取状态
	virtual ENUM_SHARESTORAGE_STATUS GetShareConfigStatus() = NULL;
	//设置状态
	virtual void SetShareConfigStatus(ENUM_SHARESTORAGE_STATUS emShareConfigStatus) = NULL;

	//管理接口
public:
	//同步子项
	virtual bool SyncShareStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagShareStorageItem & ShareStorageItem) = NULL;
	//添加子项
	virtual bool AddSyncShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem) = NULL;
	//修改子项
	virtual bool ModifySyncShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem) = NULL;
	//移除子项
	virtual bool RemoveSyncShareStorageItem(ULONGLONG ulItemID) = NULL;
	//本地子项
	virtual bool LocalShareStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagShareStorageItem & ShareStorageItem) = NULL;
	//添加子项
	virtual bool AddShareStorageArrayItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem) = NULL;
	//修改子项
	virtual bool ModifyShareStorageArrayItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem) = NULL;
	//移除子项
	virtual bool RemoveShareStorageArrayItem(ULONGLONG ulItemID) = NULL;
	//获取子项
	virtual tagShareStorageItem* GetShareStorageItem(ULONGLONG ulItemID) = NULL;
	//校验同步
	virtual bool EfficacySyncShareStorageItem(WORD wServerID, WORD wKindID, BYTE cbPlayMode) = NULL;
};

///////////////////////////////////////////////////////////////////////////

//包含文件
#ifndef SHARE_STORAGE_SERVICE_DLL
	#include "ShareStorageServiceManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif