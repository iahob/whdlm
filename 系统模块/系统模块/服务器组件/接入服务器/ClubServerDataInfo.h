
#ifndef CLUB_SERVER_DATA_FILE
#define CLUB_SERVER_DATA_FILE

#pragma once

#include "Stdafx.h"
//
//导出定义
#ifndef CLUB_SERVER_DATA_CLASS
#ifdef  CLUB_SERVER_DATA_DLL
#define CLUB_SERVER_DATA_CLASS _declspec(dllexport)
#else
#define CLUB_SERVER_DATA_CLASS _declspec(dllimport)
#endif
#endif



#ifdef _UNICODE
#define VER_ICGroupItemManager	INTERFACE_VERSION(1,1)
static const GUID IID_ICGroupItemManager = { 0x4a686dd7, 0x1cd8, 0x423f, 0xa7, 0xa6, 0xd6, 0xd3, 0xb7, 0x5a, 0x4c, 0xc7 };
#else
#define VER_ICGroupItemManager	INTERFACE_VERSION(1,1)
static const GUID IID_ICGroupItemManager = { 0x4a686dd7, 0x1cd8, 0x423f, 0xa7, 0xa6, 0xd6, 0xd3, 0xb7, 0x5a, 0x4c, 0xc7 };
#endif
//////////////////////////////////////////////////////////////////////////////////

//类说明
class CLUB_SERVER_DATA_CLASS CGroupItem;
class CLUB_SERVER_DATA_CLASS CGroupInfoManager;

//数组定义
typedef CWHArray<CGroupItem *> CGroupItemArray;
typedef CWHArray<tagIMGroupMember> CGroupMemberArray;
typedef CWHArray<tagGroupConfigInfo> CGroupConfigArray;
typedef CWHArray<tagClubRollMsg> CGroupRollMsgArray;
//索引定义
typedef CMap<DWORD, DWORD, CGroupItem *, CGroupItem *> CMapGroupID;

//类型定义
struct tagTableUserInfo
{
	DWORD           dwKindID;							//类型ID
	DWORD			dwServerID;							//房间I D
	WORD			wTableID;							//桌子I D
	DWORD			dwPersonalRoomID;					//约战房间ID
	DWORD			dwGroupID;							//大联盟ID
	bool			bStart;
	DWORD			dwUserID[GROUP_MAX_CHAIR_GAME_CHAIR_COUNT];
	tagTableUserInfo()
	{
		bStart = false;
		memset(dwUserID, INVALID_DWORD, sizeof(dwUserID));
	}
};
typedef CMap<DWORD, DWORD, tagClubTableInfo, tagClubTableInfo&> CMapClubTableInfo;
//////////////////////////////////////////////////////////////////////////////////
//群组子项
class CLUB_SERVER_DATA_CLASS ICGroupItemManager :public IUnknownEx
{
	

	//群组属性
public:
	//群组标识
	virtual DWORD GetGroupID() = 0;
	//群组子项
	virtual tagIMGroupItem * GetGroupItem() = 0;
	//群组财富
	virtual tagIMGroupWealth * GetGroupWealth() = 0;
	//群组属性
	virtual tagIMGroupProperty * GetGroupProperty() = 0;
	//群组规则
	virtual tagIMGroupBattleRule * GetGroupBattleRule() = 0;
	//添加跑马灯
	virtual void AddRollMsg(tagClubRollMsg &RollMsg) = 0;
	//查找跑马灯
	virtual tagClubRollMsg * SearchRollMsg(DWORD dwUserID) = 0;
	//设置公告
	virtual void SetNotice(TCHAR* pNotice) = 0;
	//获取公告
	virtual TCHAR *GetNotice() = 0;
	//成员操作
public:
	//移除成员
	virtual bool RemoveMember(DWORD dwMemberID) = 0;
	//添加成员
	virtual bool AddMember(tagIMGroupMember & GroupMember) = 0;
	//枚举成员
	virtual tagIMGroupMember * EnumMember(INT nIndex) = 0;
	//查找成员
	virtual tagIMGroupMember * SearchMember(DWORD dwMemberID) = 0;
	//添加桌子
	virtual bool AddPersonalTable(DWORD dwPersonalRoomID, tagClubTableInfo TableInfo) = 0;
	//删除桌子
	virtual bool RemovePersonalTable(DWORD dwPersonalRoomID) = 0;
	//删除桌子
	virtual bool RemovePersonalTable(WORD wServerID) = 0;
	//获取桌子信息
	virtual bool GetTableInfo(DWORD dwPersonalRoomID, tagClubTableInfo &TableInfo) = 0;
	//更新玩家状态
	virtual bool UpdateTableUserInfo(CMD_CS_S_UpdateTable *pUpdateTable) = 0;
	//获取成员游戏状态
	virtual bool IsUserInGame(DWORD dwUserID, tagClubTableInfo &TableInfo, bool bPlaying = false) = 0;
	//获取群主标识
	virtual DWORD GetGroupCreaterID() = 0;
	//获取玩法ID
	virtual DWORD GetConfigID(DWORD dwPersonalRoomID) = 0;
	//获取玩法数目
	virtual DWORD GetConfigCount() = 0;
	//获取桌子信息
	virtual CMapClubTableInfo* GetCMapClubTableInfo() = 0;

	//玩法操作
public:
	//添加玩法
	virtual bool AddConfig(tagGroupConfigInfo & GroupConfig) = 0;
	//修改玩法
	virtual bool ModifyConfig(tagGroupConfigInfo & GroupConfig) = 0;
	//删除玩法
	virtual bool RemoveConfig(DWORD dwConfigID) = 0;
	//删除玩法
	virtual bool RemoveConfig(INT nIndex) = 0;
	//枚举玩法
	virtual tagGroupConfigInfo * EnumConfig(INT nIndex) = 0;
	//查找玩法
	virtual tagGroupConfigInfo * SearchConfig(DWORD dwConfigID) = 0;
	//查询空桌数量
	virtual INT GetNullTableCount(DWORD dwConfigID) = 0;
	//枚举桌子
	virtual tagClubTableInfo EnumTable(POSITION &pos) = 0;
	//辅助函数
protected:
	//重置对象
	virtual void ResetGroupItem() = 0;
};

//////////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_ICGroupInfoManagerManager	INTERFACE_VERSION(1,1)
static const GUID IID_ICGroupInfoManagerManager = { 0xa11ce943, 0x5e1d, 0x4e62, 0xb6, 0xac, 0x18, 0x87, 0x4c, 0x45, 0x2c, 0x30 };
#else
#define VER_ICGroupItemManager	INTERFACE_VERSION(1,1)
static const GUID IID_ICGroupInfoManagerManager = { 0xa11ce943, 0x5e1d, 0x4e62, 0xb6, 0xac, 0x18, 0x87, 0x4c, 0x45, 0x2c, 0x30 };
#endif

//群组索引类
typedef CWHArray<CGroupItem *> CGroupItemArray;
typedef CMap<DWORD, DWORD, CGroupItem *, CGroupItem *> CGroupItemMap;

//群组管理类
class CLUB_SERVER_DATA_CLASS ICGroupInfoManagerManager : public IUnknownEx
{
	
public:
	//查找群组
	virtual CGroupItem * SearchGroupItem(DWORD dwGroupID) = 0;
	//枚举群组
	virtual CGroupItem * EnumGroupItem(POSITION & Position) = 0;

	//统计函数
public:
	//群组总数
	virtual DWORD GetGroupItemCount() = 0;

	//管理函数
public:
	//移除群组
	virtual bool RemoveGroupItem() = 0;
	//移除群组
	virtual bool RemoveGroupItem(DWORD dwGroupID) = 0;
	//插入群组
	virtual bool ActiveGroupItem(tagIMGroupItem & GroupItem) = 0;
	//获取推荐
	virtual WORD GetRecommendGroup(DWORD dwUserID, WORD wRecomCount, tagIMGroupProperty *pGroupProperty) = 0;

public:
	//校验
	virtual int CheckValid() = 0;
};

//包含文件
#ifndef SHARE_STORAGE_SERVICE_DLL
#include "ClubServerDataInfoManager.h"
#endif
//////////////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////////////

