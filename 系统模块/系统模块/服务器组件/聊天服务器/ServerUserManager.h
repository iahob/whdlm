//藏宝库IT社区（28xin.com）
#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once
#include <math.h>
#include "AfxTempl.h"

#define PI                      3.1415926		//π常量
#define EARTH_RADIUS            6378.137        //地球半径
//经度纬度
class Cltladistance
{
public:
	Cltladistance();
	virtual ~Cltladistance();

public:
	//获取弧度
    inline double GetRadian(double d);
	//四舍五入
	inline DWORD GetRound(double number);
	//获取距离
	inline double GetDistance(double lat1, double lng1, double lat2, double lng2);
};
//////////////////////////////////////////////////////////////////////////////////
//用户类型
enum enUserItemKind
{
	enNoneKind=0,
	enLocalKind,
	enRemoteKind,
};

//本地用户
struct tagInsertLocalUserInfo
{
	DWORD								dwSocketID;						//连接标识
	DWORD								dwLogonTime;					//登录时间
};

//////////////////////////////////////////////////////////////////////////////////
//用户信息
class CServerUserItem
{
	//友元定义
	friend class CServerUserManager;

	//变量定义
protected:
	enUserItemKind					m_enUserKind;						//用户类型

	//函数定义
protected:
	//构造函数
	CServerUserItem(enUserItemKind enUserKind);
	//析构函数
	virtual ~CServerUserItem();

	//用户属性
public:
	//用户标识
	virtual DWORD GetUserID()=NULL;

	//在线信息
public:
	//修改状态
	virtual VOID SetMainStatus(BYTE cbMainStatus)=NULL;
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID)=NULL;
	//用户状态
	virtual BYTE GetMainStatus()=NULL;
	//用户状态
	virtual BYTE GetGameStatus()=NULL;
	//房间标识
	virtual WORD GetServerID()=NULL;
	//桌子标识
	virtual WORD GetTableID()=NULL;
	//连接地址
	virtual DWORD GetClientAddr()=NULL;

	//辅助变量
private:
	//重置数据
	virtual VOID ResetUserItem()=NULL;

	//辅助变量
public:
	//获取类型
	enUserItemKind GetUserItemKind() { return m_enUserKind; }
};

//////////////////////////////////////////////////////////////////////////////////

//用户信息
class CLocalUserItem : public CServerUserItem
{
	//友元定义
	friend class CServerUserManager;

	//属性变量
protected:
	tagFriendUserInfo				m_UserInfo;							//用户信息
	DWORD							m_dwSocketID;						//连接标识
	LONGLONG						m_lMatchNO;							//比赛编号

	//游戏信息
protected:
	WORD							m_wServerID;						//房间标识
	WORD							m_wTableID;							//桌子标识

	//辅助变量
protected:
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//用户密码
	DWORD							m_dwLogonTime;						//登录时间
	TCHAR							m_szFaceUrl[LEN_FACE_URL];			//头像地址						

	//函数定义
protected:
	//构造函数
	CLocalUserItem();
	//析构函数
	virtual ~CLocalUserItem();

	//用户信息
public:
	//用户信息
	virtual tagFriendUserInfo * GetUserInfo() { return &m_UserInfo; }

	//属性信息
public:
	//用户性别
	virtual BYTE GetGender() { return m_UserInfo.cbGender; }
	//用户标识
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//用户标识
	virtual DWORD GetGameID() { return m_UserInfo.dwGameID; }
	//用户昵称
	virtual LPCTSTR GetNickName() { return m_UserInfo.szNickName; }

	//在线信息
public:
	//修改状态
	virtual VOID SetMainStatus(BYTE cbMainStatus) { m_UserInfo.cbMainStatus=cbMainStatus; }
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID);
	//连接标识
	virtual DWORD GetSocketID() { return m_dwSocketID; }
	//房间标识
	virtual WORD GetServerID() { return m_wServerID; }
	//桌子标识
	virtual WORD GetTableID() { return m_wTableID; }	
	//用户状态
	virtual BYTE GetMainStatus() { return m_UserInfo.cbMainStatus; }
	//用户状态
	virtual BYTE GetGameStatus() { return m_UserInfo.cbGameStatus; }	
	//连接地址
	virtual DWORD GetClientAddr() { return m_UserInfo.dwClientAddr; }
	//比赛编号
	virtual LONGLONG GetMatchNo() {	return m_lMatchNO; }

	//效验接口
public:
	//对比帐号
	virtual bool ContrastNickName(LPCTSTR pszNickName);
	//对比密码
	virtual bool ContrastLogonPass(LPCTSTR pszPassword);

	//辅助函数
public:
	//修改密码
	VOID ModifyLogonPassword(LPCTSTR pszPassword);
	//设置标识
	virtual VOID SetSocketID(DWORD dwSocketID) { m_dwSocketID=dwSocketID; }
	
	//登录信息
public:
	//登录时间
	virtual DWORD GetLogonTime() { return m_dwLogonTime; }
	//设置时间
	virtual VOID SetLogonTime(DWORD dwLogonTime) { m_dwLogonTime=dwLogonTime; }
	//设置编号
	virtual VOID SetMatchNo(LONGLONG lMatchNo) { m_lMatchNO = lMatchNo;}

	//辅助函数
private:	
	//重置数据
	virtual VOID ResetUserItem();

	//头像地址
public:
	//设置头像地址
	virtual void SetFaceUrl(LPCTSTR  pFaceUrl) { lstrcpyn(m_szFaceUrl, pFaceUrl, CountArray(m_szFaceUrl)); }
	//获取头像地址
	virtual LPCTSTR GetFaceUrl() { return m_szFaceUrl; }
};

//////////////////////////////////////////////////////////////////////////////////

//用户索引类
typedef CWHArray<CServerUserItem *> CServerUserItemArray;
typedef CMap<DWORD,DWORD,CServerUserItem *,CServerUserItem *> CServerUserItemMap;

typedef CWHArray<tagNearUserInfo *> CNearUserInfoItemArray;
typedef CMap<DWORD,DWORD,tagNearUserInfo *,tagNearUserInfo *> CNearUserInfoItemMap;

//附近用户类
class CNearUserManager
{
	//用户变量
protected:
	CNearUserInfoItemArray				m_UserItemArray;					//用户数组
	//函数定义
public:
	//构造函数
	CNearUserManager();
	//析构函数
	virtual ~CNearUserManager();
public:
	//枚举用户
	virtual tagNearUserInfo * EnumUserItem(DWORD dwEnumIndex);
	//插入用户
	virtual bool InsertNearUserItem(tagNearUserInfo & NearUserItem);
	//在线人数
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }
};

//用户管理类
class CServerUserManager
{
	//用户变量
protected:
	CServerUserItemMap				m_UserIDMap;						//用户索引
	CServerUserItemArray			m_UserItemArray;					//用户数组
	CServerUserItemArray			m_UserItemStore;					//存储用户
	Cltladistance					m_ltladistance;						//获取距离

	//函数定义
public:
	//构造函数
	CServerUserManager();
	//析构函数
	virtual ~CServerUserManager();

	//查找函数
public:
	//枚举用户
	virtual CServerUserItem * EnumUserItem(WORD wEnumIndex);
	//查找用户
	virtual CServerUserItem * SearchUserItem(DWORD dwUserID);

	//统计函数
public:
	//在线人数
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }

	//管理函数
public:
	//删除用户
	virtual bool DeleteUserItem();
	//删除用户
	virtual bool DeleteUserItem(DWORD dwUserID);
	//插入用户
	virtual bool InsertLocalUserItem(tagInsertLocalUserInfo & InsertLocalUserInfo,tagFriendUserInfo & UserInfo,LPCTSTR szPassword);

	//附近的人
public:
	//附近的人
	virtual DWORD GetNearUserItem(DWORD dwUserID,CNearUserManager & NearUserManager);
	//查询附近
	virtual DWORD QueryNearUserItem(DWORD dwUserID,DWORD dwNearUserID,CNearUserManager & NearUserManager);
};

//////////////////////////////////////////////////////////////////////////////////

//好友分组
class CUserFriendGroup
{
	//友元定义
	friend class CFriendGroupManager;

	//变量定义
protected:
	DWORD										m_dwOwnerUserID;				//用户标识
	WORD										m_wGroupContent;				//分组容量
	WORD										m_wFriendCount;					//好友数目

protected:
	tagServerFriendInfo *						m_pFriendInfo;					//用户信息	

	//函数定义
public:
	//构造函数
	CUserFriendGroup();
	//析构函数
	virtual ~CUserFriendGroup();

	//辅助函数
public:
	//设置标识
	VOID SetOwnerUserID(DWORD dwUserID) { m_dwOwnerUserID=dwUserID; }
	//获取标识
	DWORD GetOwnerUserID() { return m_dwOwnerUserID; }
	//获取容量
	WORD GetGroupContent() { return m_wGroupContent; }
	//获取数目
	WORD GetFriendCount() { return m_wFriendCount; }

	//功能函数
public:
	//初始化分组
	bool InitFriendGroup(WORD wGroupContent);
	//添加好友
	bool AppendFriendInfo(tagServerFriendInfo & FriendInfo);
	//移除好友
	bool RemoveFriendInfo(DWORD dwUserID);
	//查找好友
	tagServerFriendInfo * SearchFriendItem(DWORD dwUserID);
	//枚举好友
	tagServerFriendInfo * EnumFriendItem(WORD wEnumIndex);
	//重置分组
	VOID ResetFriendGroup();


	//辅助变量
private:
	//分配容量
	tagServerFriendInfo * AllocateContent(WORD wGroupContent);
	//扩展容量
	bool ExtendGroupContent(WORD wExtendCount);	
};

//////////////////////////////////////////////////////////////////////////////////

//用户索引类
typedef CWHArray<CUserFriendGroup *> CUserFriendGroupArray;
typedef CMap<DWORD,DWORD,CUserFriendGroup *,CUserFriendGroup *> CUserFriendGroupMap;

//好友管理类
class CFriendGroupManager
{
	//用户变量
protected:
	CUserFriendGroupMap				m_UserIDMap;						//用户索引
	CUserFriendGroupArray			m_UserFriendGroupArray;				//用户数组
	CUserFriendGroupArray			m_UserFriendGroupStore;				//存储用户

	//函数定义
public:
	//构造函数
	CFriendGroupManager();
	//析构函数
	virtual ~CFriendGroupManager();

	//查找函数
public:
	//枚举用户
	virtual CUserFriendGroup * EnumGroupItem(WORD wEnumIndex);
	//查找用户
	virtual CUserFriendGroup * SearchGroupItem(DWORD dwUserID);

	//管理函数
public:
	//删除用户
	virtual bool DeleteFriendGroup();
	//删除用户
	virtual bool DeleteFriendGroup(DWORD dwUserID);
	//插入好友
	virtual bool InsertFriendGroup(DWORD dwUserID,CUserFriendGroup * pUserFriendGroup);

	//辅助函数
public:
	//获取分组
	CUserFriendGroup * ActiveFriendGroup(WORD wFriendCount);
};

//////////////////////////////////////////////////////////////////////////////////
//比赛

class CGameMatchItem
{
	//变量定义
public:
	bool							m_bDisuse;							//废弃标志

	//函数定义
protected:
	//构造函数
	CGameMatchItem()
	{
		m_bDisuse = false;
	}
};

//比赛配置
class CMatchOptionItem : public CGameMatchItem
{
	//变量定义
public:
	tagMatchSignUpCount				m_GameMatchOption;

	//功能函数
public:
	//函数定义
public:
	//构造函数
	CMatchOptionItem();
};

//比赛房间
class CMatchServerItem : public CGameMatchItem
{
	//变量定义
public:
	tagGameMatch					m_GameMatch;						//比赛信息

	//功能函数
public:
	//函数定义
public:
	//构造函数
	CMatchServerItem();
};

//数组说明
typedef CWHArray<CMatchOptionItem *>			CMatchOptionItemArray;
typedef CWHArray<CMatchServerItem *>			CMatchServerItemArray;
//索引说明
typedef CMap<WORD, WORD, CMatchOptionItem *, CMatchOptionItem *&>			CMatchOptionItemMap;
typedef CMap<WORD, WORD, CMatchServerItem *, CMatchServerItem *&>			CMatchServerItemMap;

//比赛服务
class CMatchServerManager
{
	//索引变量
protected:
	CMatchOptionItemMap				m_MatchOptionItemMap;					//配置索引
	CMatchServerItemMap				m_MatchServerItemMap;					//比赛索引

	//存储变量
protected:
	CMatchOptionItemArray			m_MatchOptionItemBuffer;				//配置数组
	CMatchServerItemArray			m_MatchServerItemBuffer;				//比赛数组

	//函数定义
public:
	//构造函数
	CMatchServerManager();
	//析构函数
	virtual ~CMatchServerManager();

	//管理接口
public:
	//重置列表
	VOID ResetMatchData();
	//清理内核
	VOID CleanKernelItem();
	//清理房间
	VOID CleanServerItem();
	//废弃内核
	VOID DisuseKernelItem();
	//废弃房间
	VOID DisuseServerItem();

	//	//统计接口
	//public:
	//	//统计人数
	//	DWORD CollectOnlineInfo(bool bAndroid);
	//	//类型在线
	//	DWORD CollectOnlineInfo(WORD wKindID, bool bAndroid);
	//	//统计设置人数
	//	DWORD CollectSetPlayer(WORD wKindID);
	//插入接口
public:
	//插入配置
	bool InsertMatchOption(tagMatchSignUpCount *pMatchOption);
	//插入房间
	bool InsertMatchServer(tagGameMatch *pGameServer);

	//删除接口
public:
	//删除配置
	bool DeleteMatchOption(WORD wPageID);
	//删除房间
	bool DeleteMatchServer(WORD wServerID);

	//枚举接口
public:
	//枚举配置
	CMatchOptionItem *EmunMatchOptionItem(POSITION &Position);
	//枚举房间
	CMatchServerItem *EmunMatchServerItem(POSITION &Position);

	//查找接口
public:
	//查找配置
	CMatchOptionItem *SearchMatchOption(WORD wServerID);
	//查找比赛
	CMatchServerItem *SearchMatchServer(WORD wServerID);

	//数目接口
public:
	//配置数目
	DWORD GetMatchOptionCount()
	{
		return (DWORD)m_MatchOptionItemMap.GetCount();
	}
	//房间数目
	DWORD GetMatchServerCount()
	{
		return (DWORD)m_MatchServerItemMap.GetCount();
	}
};

//////////////////////////////////////////////////////////////////////////////////

//用户索引类
typedef CWHArray<tagPersonalTableInfoEx *> CPersonalTableInfoEx;
typedef CMap<DWORD, DWORD, CPersonalTableInfoEx *, CPersonalTableInfoEx *> CServerPersonalTableMap;
//用户管理类
class CPersonalRoomManager
{
	//用户变量
protected:
	CServerPersonalTableMap			m_PersonalTableMap;						//用户索引
	CPersonalTableInfoEx			m_PersonalTableArray;				//用户数组

	//函数定义
public:
	//构造函数
	CPersonalRoomManager();
	//析构函数
	virtual ~CPersonalRoomManager();

	//查找函数
public:
	//枚举用户
	virtual tagPersonalTableInfoEx * EnumPersonalTableItem(DWORD wKindID, BYTE cbPlayMode, int nPos);

	//获取该类型房间数量
	virtual int GetPersonalTableCount(DWORD wKindID);

	//管理函数
public:
	//删除用户
	virtual bool DeletePersonalTableItem(DWORD wKindID, DWORD wServerID, DWORD dwRoomID);

	//插入用户
	virtual bool InsertPersonalTableItem(tagPersonalTableInfoEx * pPersonalTableInfoEx);
};


#endif