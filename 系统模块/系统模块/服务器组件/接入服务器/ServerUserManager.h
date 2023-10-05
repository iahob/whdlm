#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////////////
//用户类型
enum enUserItemKind
{
	enNoneKind=0,
	enLocalKind,
	enRemoteKind,
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
	DWORD							m_dwGroupID;						//群组标识

	//用户信息
protected:
	tagIMUserInfo					m_UserInfo;							//用户信息
	DWORD							m_dwSocketID;						//网络标志

	//函数定义
protected:
	//构造函数
	CServerUserItem(enUserItemKind enUserKind);
	//析构函数
	virtual ~CServerUserItem();

	//用户状态
public:	
	//获取状态
	virtual BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }
	//修改状态
	virtual VOID SetUserStatus(BYTE cbUserStatus) { m_UserInfo.cbUserStatus=cbUserStatus; }
	//群组信息
public:
	//获取群组
	virtual DWORD GetActiveGroupID() { return m_dwGroupID; }
	//设置群组
	virtual VOID SetActiveGroupID(DWORD dwGroupID) { m_dwGroupID = dwGroupID; }
	//属性信息
public:
	//用户性别
	virtual BYTE GetGender() { return m_UserInfo.cbGender; }
	//用户标识
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//游戏标识
	virtual DWORD GetGameID() { return m_UserInfo.dwGameID; }
	//用户昵称
	virtual LPCWSTR GetNickName() { return m_UserInfo.szNickName; }
	//用户信息
	virtual tagIMUserInfo * GetUserInfo() { return &m_UserInfo; }
	//网络标志
	virtual DWORD  GetSocketID() { return m_dwSocketID; }
	//设置网络标志
	virtual VOID SetSocketID(DWORD dwSocketID) { m_dwSocketID = dwSocketID; }
	//辅助变量
protected:
	//重置数据
	virtual VOID ResetUserItem();

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
	CDWordArray						m_GroupIDArray;						//群组数组
	
	//辅助变量
protected:
	DWORD							m_dwLogonTime;						//登录时间
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//用户密码	

	//函数定义
protected:
	//构造函数
	CLocalUserItem();
	//析构函数
	virtual ~CLocalUserItem();

	//群组操作
public:
	//添加群组
	virtual bool InsertGroupID(DWORD dwGroupID);
	//移除群组
	virtual bool RemoveGroupID(DWORD dwGroupID);
	//移除群组
	virtual VOID RemoveGroupID() { m_GroupIDArray.RemoveAll(); }
	//群组数据
	virtual DWORD * GetGroupData() { return m_GroupIDArray.GetData(); };
	//群组数量
	virtual WORD GetGroupCount() { return (WORD)m_GroupIDArray.GetCount(); }	

	//登录信息
public:
	//登录时间
	virtual DWORD GetLogonTime() { return m_dwLogonTime; }
	//设置时间
	virtual VOID SetLogonTime(DWORD dwLogonTime) { m_dwLogonTime=dwLogonTime; }

	//效验接口
public:
	//对比帐号
	virtual bool ContrastNickName(LPCWSTR pszNickName);
	//对比密码
	virtual bool ContrastLogonPass(LPCWSTR pszPassword);

	//辅助函数
public:
	//修改密码
	VOID ModifyLogonPassword(LPCWSTR pszPassword);	
	
	//辅助函数
protected:	
	//重置数据
	virtual VOID ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////////////

//用户信息
class CRemoteUserItem: public CServerUserItem
{
	//友元定义
	friend class CServerUserManager;

	//函数定义
protected:
	//构造函数
	CRemoteUserItem();
	//析构函数
	virtual ~CRemoteUserItem();

	//辅助函数
protected:
	//重置数据
	virtual VOID ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////////////

//用户索引类
typedef CWHArray<CServerUserItem *> CServerUserItemArray;
typedef CMap<DWORD,DWORD,CServerUserItem *,CServerUserItem *> CServerUserItemMap;

//用户管理类
class CServerUserManager
{
	//用户变量
protected:
	CServerUserItemMap				m_UserIDMap;						//用户索引
	CServerUserItemArray			m_UserItemArray;					//用户数组
	CServerUserItemArray			m_UserItemStore;					//存储用户

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
	virtual CRemoteUserItem * InsertRemoteUserItem(tagIMUserInfo & UserInfo);
	//插入用户
	virtual CLocalUserItem * InsertLocalUserItem(tagIMUserInfo & UserInfo,LPCWSTR pszPassword);	
};

//////////////////////////////////////////////////////////////////////////////////
#endif