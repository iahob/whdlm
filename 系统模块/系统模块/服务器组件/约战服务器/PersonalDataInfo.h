
#ifndef PERSONAL_DATA_FILE
#define PERSONAL_DATA_FILE

#include "..\..\全局定义\Platform.h"

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_Correspond.h"
#include "..\..\消息定义\CMD_GameServer.h"

//平台文件
#include "..\..\公共组件\服务核心\ServiceCoreHead.h"
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"
#include "../约战服务器/GlobalInfoManager.h"

//导出定义
/*
#ifndef PERSONAL_DATA_CLASS
	#ifdef  PERSONAL_DATA_DLL
		#define PERSONAL_DATA_CLASS _declspec(dllexport)
	#else
		#define PERSONAL_DATA_CLASS _declspec(dllimport)
	#endif
#endif*/
#pragma once


//模块定义
/*
#ifndef _DEBUG
#define PERSONAL_DATA_DLL_NAME		TEXT("PersonalData.dll")			//组件名字
#else
#define PERSONAL_DATA_DLL_NAME		TEXT("PersonalDataD.dll")		//组件名字
#endif*/

#ifdef _UNICODE
#define VER_IPersonalDataManager	INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalDataManager = { 0xaddb9939, 0xd474, 0x4fc9, 0x88, 0x2c, 0x7a, 0x4, 0x3a, 0xdf, 0x8b, 0x25 };
#else
#define VER_IPersonalDataManager	INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalDataManager = { 0xaddb9939, 0xd474, 0x4fc9, 0x88, 0x2c, 0x7a, 0x4, 0x3a, 0xdf, 0x8b, 0x25 };
#endif


//AI信息
interface /*PERSONAL_DATA_CLASS*/ IPersonalDataManager : public IUnknownEx
{


	//管理函数
public:
	//重置数据
	virtual VOID ResetData() = 0;
	//用户数目
	virtual DWORD GetUserItemCount() = 0;
	//大厅数目
	virtual DWORD GetPlazaItemCount() = 0;
	//房间数目
	virtual DWORD GetServerItemCount() = 0;
	//聊天数目
	virtual DWORD GetChatItemCount() = 0;

	//私人房间
public:
	//查找桌子
	virtual tagPersonalTableInfo* SearchTableByID(DWORD dwRoomID) = 0;
	//查找桌子
	virtual tagPersonalTableInfo* SearchTableByTableIDAndServerID(DWORD dwServerID, DWORD dwTableID) = 0;
	//添加桌子
	virtual bool AddFreeServerTable(DWORD dwServerID) = 0;
	//添加桌子
	virtual bool AddServerTable(DWORD dwRoomID, tagPersonalTableInfo PersonalTable) = 0;
	//移除桌子
	virtual bool RemoveFreeServerTable(DWORD dwServerID) = 0;
	//移除桌子
	virtual bool RemoveServerTable(DWORD dwServerID, DWORD dwTableID) = 0;
	//移除桌子
	virtual bool RemoveServerTable(DWORD dwServerID) = 0;
	//获取房间
	virtual DWORD GetFreeServer(DWORD dwUserID, DWORD dwKindID, BYTE cbIsJoinGame, BYTE cbPlayMode, bool bSupportGroup) = 0;
	//获取房间数目
	virtual DWORD GetFreeServerCount(DWORD dwKindID, BYTE cbPlayMode, bool bSupportGroup) = 0;
	//生成房间ID
	virtual DWORD RandPersonalRoomID() = 0;

	//获取房主创建的所有房间
	virtual int GetHostCreatePersonalRoom(tagHostCreatRoomInfo & HostCreatRoomInfo) = 0;

	//获取房主创建的房间的数量
	virtual INT GetHostCreatePersonalRoomCount(DWORD dwUserID, DWORD dwKindID, BYTE cbPlayMode, DWORD dwGroupID) = 0;
	//添加桌子
	virtual bool AddPersonalMaxCreate(CMD_CS_S_RegisterPersonal RegisterPersonal) = 0;
	//是否可以再创建房间
	virtual bool CanCreatePersonalRoom(DWORD dwKindID, DWORD dwUserID, BYTE cbPlayMode, DWORD dwGroupID) = 0;

	//用户管理
public:
	//删除用户
	virtual bool DeleteUserItem(DWORD dwUserID, WORD wServerID) = 0;
	//激活用户
	virtual bool ActiveUserItem(tagGlobalUserInfo &GlobalUserInfo, WORD wServerID) = 0;

	//广场管理
public:
	//删除广场
	virtual bool DeletePlazaItem(WORD wPlazaID) = 0;
	//激活广场
	virtual bool ActivePlazaItem(WORD wBindIndex, tagGamePlaza & GamePlaza) = 0;

	//房间管理
public:
	//删除房间
	virtual bool DeleteServerItem(WORD wServerID) = 0;
	//激活房间
	virtual bool ActiveServerItem(WORD wBindIndex, BYTE	cbPlayMode, tagGameServer &GameServer) = 0;

	//激活俱乐部房间
	virtual bool ActiveGroupRoom(DWORD dwGroupID, DWORD dwRoomID) = 0;

	//获取俱乐部房间
	virtual CRoomIDArray * GetGroupRoom(DWORD dwGroupID) = 0;

	//删除俱乐部房间
	virtual BOOL DeleteGroupRoom(DWORD dwGroupID, DWORD dwRoomID) = 0;

	//聊天管理
public:
	//删除聊天
	virtual bool DeleteChatItem(WORD wChatID) = 0;
	//激活聊天
	virtual bool ActiveChatItem(WORD wBindIndex, tagChatServer & ChatServer) = 0;

	//服务查找
public:
	//寻找广场
	virtual CGlobalPlazaItem * SearchPlazaItem(WORD wPlazaID) = 0;
	//寻找房间
	virtual CGlobalServerItem * SearchServerItem(WORD wServerID) = 0;
	//寻找聊天
	virtual CGlobalChatItem * SearchChatItem(WORD wChatID) = 0;

	//用户查找
public:
	//寻找用户
	virtual CGlobalUserItem * SearchUserItemByUserID(DWORD dwUserID) = 0;
	//寻找用户
	virtual CGlobalUserItem * SearchUserItemByGameID(DWORD dwGameID) = 0;
	//寻找用户
	virtual CGlobalUserItem * SearchUserItemByNickName(LPCTSTR pszNickName) = 0;
	//遍历一个类型游戏的桌子
	virtual tagPersonalTableInfo * SearchTableByPosition(WORD wKindID, BYTE cbPlayMode, DWORD dwGroupID, POSITION & Position, DWORD & dwRoomID) = 0;

	//枚举函数
public:
	//枚举用户
	virtual CGlobalUserItem * EnumUserItem(POSITION & Position )= 0;
	//枚举广场
	virtual CGlobalPlazaItem * EnumPlazaItem(POSITION & Position) = 0;
	//枚举房间
	virtual CGlobalServerItem * EnumServerItem(POSITION & Position) = 0;
	//枚举聊天
	virtual CGlobalChatItem * EnumChatItem(POSITION & Position) = 0;

	//创建函数
private:
	//创建用户
	virtual CGlobalUserItem * CreateGlobalUserItem() = 0;
	//创建广场
	virtual CGlobalPlazaItem * CreateGlobalPlazaItem() = 0;
	//创建房间
	virtual CGlobalServerItem * CreateGlobalServerItem() = 0;
	//创建聊天
	virtual CGlobalChatItem * CreateGlobalChatItem() = 0;

	//释放函数
private:
	//释放用户
	virtual bool FreeGlobalUserItem(CGlobalUserItem * pGlobalUserItem) = 0;
	//释放广场
	virtual bool FreeGlobalPlazaItem(CGlobalPlazaItem * pGlobalPlazaItem) = 0;
	//释放房间
	virtual bool FreeGlobalServerItem(CGlobalServerItem * pGlobalServerItem) = 0;
	//释放聊天
	virtual bool FreeGlobalChatItem(CGlobalChatItem * pGlobalChatItem) = 0;

public:
	//校验
	virtual int CheckValid() = 0;
};
//////////////////////////////////////////////////////////////////////////////////
//包含文件
//#ifndef PERSONAL_DATA_DLL
class /*PERSONAL_DATA_CLASS*/ CGlobalInfoManager : public IPersonalDataManager
{
	//索引变量
protected:
	CMapUserID						m_MapUserID;						//用户标识
	CMapPlazaID						m_MapPlazaID;						//广场标识
	CMapServerID					m_MapServerID;						//房间标识
	CMapChatID						m_MapChatID;						//聊天标识
	CMapGroupRoom							m_MapGroupRoom;							//俱乐部房间映射

	//++++++++++++++++++++++++++++++约战房间
public:
	CServerTableCountArray					m_ServerTableCountArray;				//房间空余桌子数量
	CMapPersonalTableInfo					m_MapPersonalTableInfo;					//私人房间信息
	std::vector<tagPersonalTableInfo>		m_VecDissumePersonalTableInfo;			//被解散的私人房间信息
	std::vector<CMD_CS_S_RegisterPersonal>	m_vecPersonalRoomMaxCreate;				//私人房可以创建的最大房间数
	//辅助索引
protected:
	CMapGameID						m_MapGameID;						//标识索引
	CMapStringToPtr					m_MapNickName;						//昵称索引

	//存储变量
protected:
	CGlobalUserItem *				m_pGlobalUserItem;					//用户存储
	CGlobalPlazaItem *				m_pGlobalPlazaItem;					//广场存储
	CGlobalServerItem *				m_pGlobalServerItem;				//房间存储
	CGlobalChatItem *				m_pGlobalChatItem;					//聊天存储

	//函数定义
public:
	//构造函数
	CGlobalInfoManager();
	//析构函数
	virtual ~CGlobalInfoManager();
	//函数定义
public:
	//释放对象
	virtual VOID Release(){ delete this; };
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	//管理函数
public:
	//重置数据
	virtual VOID ResetData();
	//用户数目
	virtual DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }
	//大厅数目
	virtual DWORD GetPlazaItemCount() { return (DWORD)m_MapPlazaID.GetCount(); }
	//房间数目
	virtual DWORD GetServerItemCount() { return (DWORD)m_MapServerID.GetCount(); }
	//聊天数目
	virtual DWORD GetChatItemCount() { return (DWORD)m_MapChatID.GetCount(); }

	//私人房间
public:
	//查找桌子
	virtual tagPersonalTableInfo* SearchTableByID(DWORD dwRoomID);
	//查找桌子
	virtual tagPersonalTableInfo* SearchTableByTableIDAndServerID(DWORD dwServerID, DWORD dwTableID);
	//添加桌子
	virtual bool AddFreeServerTable(DWORD dwServerID);
	//添加桌子
	virtual bool AddServerTable(DWORD dwRoomID, tagPersonalTableInfo PersonalTable);
	//移除桌子
	virtual bool RemoveFreeServerTable(DWORD dwServerID);
	//移除桌子
	virtual bool RemoveServerTable(DWORD dwServerID, DWORD dwTableID);
	//移除桌子
	virtual bool RemoveServerTable(DWORD dwServerID);
	//获取房间
	virtual DWORD GetFreeServer(DWORD dwUserID, DWORD dwKindID, BYTE cbIsJoinGame, BYTE cbPlayMode, bool bSupportGroup);
	//获取房间数目
	virtual DWORD GetFreeServerCount(DWORD dwKindID, BYTE cbPlayMode, bool bSupportGroup);
	//生成房间ID
	virtual DWORD RandPersonalRoomID();

	//获取房主创建的所有房间
	virtual int GetHostCreatePersonalRoom(tagHostCreatRoomInfo & HostCreatRoomInfo);

	//获取房主创建的房间的数量
	virtual INT GetHostCreatePersonalRoomCount(DWORD dwUserID, DWORD dwKindID, BYTE cbPlayMode, DWORD dwGroupID);
	//添加桌子
	virtual bool AddPersonalMaxCreate(CMD_CS_S_RegisterPersonal RegisterPersonal);
	//是否可以再创建房间
	virtual bool CanCreatePersonalRoom(DWORD dwKindID, DWORD dwUserID, BYTE cbPlayMode, DWORD dwGroupID);

	//用户管理
public:
	//删除用户
	virtual bool DeleteUserItem(DWORD dwUserID, WORD wServerID);
	//激活用户
	virtual bool ActiveUserItem(tagGlobalUserInfo &GlobalUserInfo, WORD wServerID);

	//广场管理
public:
	//删除广场
	virtual bool DeletePlazaItem(WORD wPlazaID);
	//激活广场
	virtual bool ActivePlazaItem(WORD wBindIndex, tagGamePlaza & GamePlaza);

	//房间管理
public:
	//删除房间
	virtual bool DeleteServerItem(WORD wServerID);
	//激活房间
	virtual bool ActiveServerItem(WORD wBindIndex, BYTE	cbPlayMode, tagGameServer &GameServer);

	//激活俱乐部房间
	virtual bool ActiveGroupRoom(DWORD dwGroupID, DWORD dwRoomID);

	//获取俱乐部房间
	virtual CRoomIDArray * GetGroupRoom(DWORD dwGroupID);

	//删除俱乐部房间
	virtual BOOL DeleteGroupRoom(DWORD dwGroupID, DWORD dwRoomID);

	//聊天管理
public:
	//删除聊天
	virtual bool DeleteChatItem(WORD wChatID);
	//激活聊天
	virtual bool ActiveChatItem(WORD wBindIndex, tagChatServer & ChatServer);

	//服务查找
public:
	//寻找广场
	virtual CGlobalPlazaItem * SearchPlazaItem(WORD wPlazaID);
	//寻找房间
	virtual CGlobalServerItem * SearchServerItem(WORD wServerID);
	//寻找聊天
	virtual CGlobalChatItem * SearchChatItem(WORD wChatID);

	//用户查找
public:
	//寻找用户
	virtual CGlobalUserItem * SearchUserItemByUserID(DWORD dwUserID);
	//寻找用户
	virtual CGlobalUserItem * SearchUserItemByGameID(DWORD dwGameID);
	//寻找用户
	virtual CGlobalUserItem * SearchUserItemByNickName(LPCTSTR pszNickName);
	//遍历一个类型游戏的桌子
	virtual tagPersonalTableInfo * SearchTableByPosition(WORD wKindID, BYTE cbPlayMode, DWORD dwGroupID, POSITION & Position, DWORD & dwRoomID);

	//枚举函数
public:
	//枚举用户
	virtual CGlobalUserItem * EnumUserItem(POSITION & Position);
	//枚举广场
	virtual CGlobalPlazaItem * EnumPlazaItem(POSITION & Position);
	//枚举房间
	virtual CGlobalServerItem * EnumServerItem(POSITION & Position);
	//枚举聊天
	virtual CGlobalChatItem * EnumChatItem(POSITION & Position);

	//创建函数
private:
	//创建用户
	virtual CGlobalUserItem * CreateGlobalUserItem();
	//创建广场
	virtual CGlobalPlazaItem * CreateGlobalPlazaItem();
	//创建房间
	virtual CGlobalServerItem * CreateGlobalServerItem();
	//创建聊天
	virtual CGlobalChatItem * CreateGlobalChatItem();

	//释放函数
private:
	//释放用户
	virtual bool FreeGlobalUserItem(CGlobalUserItem * pGlobalUserItem);
	//释放广场
	virtual bool FreeGlobalPlazaItem(CGlobalPlazaItem * pGlobalPlazaItem);
	//释放房间
	virtual bool FreeGlobalServerItem(CGlobalServerItem * pGlobalServerItem);
	//释放聊天
	virtual bool FreeGlobalChatItem(CGlobalChatItem * pGlobalChatItem);

public:
	//校验
	virtual int CheckValid();
};

//////////////////////////////////////////////////////////////////////////////////

//#endif
//////////////////////////////////////////////////////////////////////////////////

#endif