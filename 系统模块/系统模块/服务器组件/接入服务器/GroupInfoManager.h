#ifndef GROUP_INFO_MANAGER_HEAD_FILE
#define GROUP_INFO_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////////////

//类说明
class CGroupItem;
class CGroupInfoManager;

//数组定义
typedef CWHArray<CGroupItem *> CGroupItemArray;
typedef CWHArray<tagIMGroupMember> CGroupMemberArray;
typedef CWHArray<tagGroupConfigInfo> CGroupConfigArray;
typedef CWHArray<tagClubRollMsg> CGroupRollMsgArray;

//索引定义
typedef CMap<DWORD, DWORD, CGroupItem *, CGroupItem *> CMapGroupID;
typedef CMap<DWORD, DWORD, tagClubTableInfo, tagClubTableInfo&> CMapClubTableInfo;

//////////////////////////////////////////////////////////////////////////////////
//群组子项
class CGroupItem
{
	//友元定义
	friend class CGroupInfoManager;

	//群组属性
protected:
	tagIMGroupItem						m_GroupItem;						//群组子项

	//群组成员
protected:
	CGroupMemberArray					m_GroupMemberArray;					//成员数组
	CGroupConfigArray					m_GroupConfigArray;					//成员数组
	CGroupRollMsgArray					m_GroupRollMsgArray;					//成员数组
	CMapClubTableInfo					m_MapClubTableInfo;
	TCHAR* m_Notice;

	//函数定义
protected:
	//构造函数
	CGroupItem();
	//析构函数
	virtual ~CGroupItem();

	//群组属性
public:
	//群组标识
	virtual DWORD GetGroupID() { return m_GroupItem.GroupProperty.dwGroupID; }
	//群组子项
	virtual tagIMGroupItem * GetGroupItem() { return &m_GroupItem; }
	//群组财富
	virtual tagIMGroupWealth * GetGroupWealth() { return &m_GroupItem.GroupWealth; }
	//群组属性
	virtual tagIMGroupProperty * GetGroupProperty() { return &m_GroupItem.GroupProperty; }
	//群组规则
	virtual tagIMGroupBattleRule * GetGroupBattleRule() { return &m_GroupItem.GroupBattleRule; }
	//添加跑马灯
	virtual void AddRollMsg(tagClubRollMsg &RollMsg);
	//查找跑马灯
	virtual tagClubRollMsg * SearchRollMsg(DWORD dwUserID);
	//设置公告
	virtual void SetNotice(TCHAR* pNotice){ m_Notice = pNotice; }
	//获取公告
	virtual TCHAR *GetNotice(){ return m_Notice; }
	//成员操作
public:
	//移除成员
	virtual bool RemoveMember(DWORD dwMemberID);
	//添加成员
	virtual bool AddMember(tagIMGroupMember & GroupMember);
	//枚举成员
	virtual tagIMGroupMember * EnumMember(INT nIndex);
	//查找成员
	virtual tagIMGroupMember * SearchMember(DWORD dwMemberID);
	//添加桌子
	virtual bool AddPersonalTable(DWORD dwPersonalRoomID, tagClubTableInfo TableInfo);
	//删除桌子
	virtual bool RemovePersonalTable(DWORD dwPersonalRoomID);
	//删除桌子
	virtual bool RemovePersonalTable(WORD wServerID);
	//获取桌子信息
	virtual bool GetTableInfo(DWORD dwPersonalRoomID, tagClubTableInfo &TableInfo);
	//更新玩家状态
	virtual bool UpdateTableUserInfo(CMD_CS_S_UpdateTable *pUpdateTable);
	//获取成员游戏状态
	virtual bool IsUserInGame(DWORD dwUserID, tagClubTableInfo &TableInfo, bool bPlaying = false);
	//获取群主标识
	virtual DWORD GetGroupCreaterID() { return m_GroupItem.GroupProperty.dwCreaterID; }
	//获取玩法ID
	virtual DWORD GetConfigID(DWORD dwPersonalRoomID);
	//获取玩法数目
	virtual DWORD GetConfigCount() { return m_GroupConfigArray.GetCount(); }
	//获取桌子信息
	virtual CMapClubTableInfo* GetCMapClubTableInfo() { return &m_MapClubTableInfo; }

	//玩法操作
public:
	//添加玩法
	virtual bool AddConfig(tagGroupConfigInfo & GroupConfig);
	//修改玩法
	virtual bool ModifyConfig(tagGroupConfigInfo & GroupConfig);
	//删除玩法
	virtual bool RemoveConfig(DWORD dwConfigID);
	//删除玩法
	virtual bool RemoveConfig(INT nIndex);
	//枚举玩法
	virtual tagGroupConfigInfo * EnumConfig(INT nIndex);
	//查找玩法
	virtual tagGroupConfigInfo * SearchConfig(DWORD dwConfigID);
	//查询空桌数量
	virtual INT GetNullTableCount(DWORD dwConfigID);
	//枚举桌子
	virtual tagClubTableInfo EnumTable(POSITION &pos);
	//辅助函数
protected:
	//重置对象
	virtual void ResetGroupItem();
};

//////////////////////////////////////////////////////////////////////////////////

//群组索引类
typedef CWHArray<CGroupItem *> CGroupItemArray;
typedef CMap<DWORD,DWORD,CGroupItem *,CGroupItem *> CGroupItemMap;

//群组管理类
class CGroupInfoManager
{
	//用户变量
protected:
	CGroupItemMap					m_GroupIDMap;						//群组索引	
	CGroupItemArray					m_GroupItemStore;					//存储群组

	//函数定义
public:
	//构造函数
	CGroupInfoManager();
	//析构函数
	virtual ~CGroupInfoManager();

	//查找函数
public:	
	//查找群组
	virtual CGroupItem * SearchGroupItem(DWORD dwGroupID);
	//枚举群组
	virtual CGroupItem * EnumGroupItem(POSITION & Position);

	//统计函数
public:
	//群组总数
	virtual DWORD GetGroupItemCount() { return (DWORD)m_GroupIDMap.GetCount(); }

	//管理函数
public:
	//移除群组
	virtual bool RemoveGroupItem();
	//移除群组
	virtual bool RemoveGroupItem(DWORD dwGroupID);
	//插入群组
	virtual bool ActiveGroupItem(tagIMGroupItem & GroupItem);
	//获取推荐
	virtual WORD GetRecommendGroup(DWORD dwUserID, WORD wRecomCount, tagIMGroupProperty *pGroupProperty);

public:
	//校验
	virtual int CheckValid();
};

//////////////////////////////////////////////////////////////////////////////////
#endif