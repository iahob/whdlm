#include "StdAfx.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CServerUserItem::CServerUserItem(enUserItemKind enUserKind)
{
	//设置变量	
	m_enUserKind=enUserKind;
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	m_dwSocketID = 0;
	m_dwGroupID = 0;

}

//析构函数
CServerUserItem::~CServerUserItem()
{
}

//重置数据
VOID CServerUserItem::ResetUserItem()
{
	//设置变量	
	m_enUserKind=enNoneKind;
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CLocalUserItem::CLocalUserItem():CServerUserItem(enLocalKind)
{
	//清理数组
	m_GroupIDArray.RemoveAll();

	//辅助变量
	m_dwLogonTime=0;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));	

	return;
}

//析构函数
CLocalUserItem::~CLocalUserItem()
{
}

//添加群组
bool CLocalUserItem::InsertGroupID(DWORD dwGroupID)
{
	//查找群组
	for (INT_PTR i=0; i<m_GroupIDArray.GetCount(); i++)
	{
		if (m_GroupIDArray[i]==dwGroupID)
		{
			return false;
		}
	}

	//添加群组
	m_GroupIDArray.Add(dwGroupID);

	return true;
}

//移除群组
bool CLocalUserItem::RemoveGroupID(DWORD dwGroupID)
{
	//查找群组
	for (INT_PTR i=0; i<m_GroupIDArray.GetCount(); i++)
	{
		if (m_GroupIDArray[i]==dwGroupID)
		{
			m_GroupIDArray.RemoveAt(i);
			return true;
		}
	}

	return false;
}

//对比帐号
bool CLocalUserItem::ContrastNickName(LPCWSTR pszNickName)
{
	//效验参数
	//ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//长度对比
	INT nContrastLen=lstrlenW(pszNickName);
	INT nSourceLen=lstrlenW(m_UserInfo.szNickName);

	//字符对比
	if (nContrastLen!=nSourceLen) return false;
	if (CompareStringW(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszNickName,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//对比密码
bool CLocalUserItem::ContrastLogonPass(LPCWSTR pszPassword)
{
	//效验参数
	//ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//长度对比
	INT nTargetLen=lstrlenW(pszPassword);
	INT nSourceLen=lstrlenW(m_szLogonPass);

	//密码对比
	if (nTargetLen!=nSourceLen) return false;
	if (CompareStringW(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszPassword,nTargetLen,m_szLogonPass,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//重置数据
VOID CLocalUserItem::ResetUserItem()
{
	//重置数据
	__super::ResetUserItem();

	//清理数组
	m_GroupIDArray.RemoveAll();

	//辅助变量
	m_dwLogonTime=0;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));	

	return;
}

//修改密码
VOID CLocalUserItem::ModifyLogonPassword(LPCWSTR pszPassword)
{
	//拷贝字符
	lstrcpynW(m_szLogonPass,pszPassword,CountArray(m_szLogonPass));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CRemoteUserItem::CRemoteUserItem(): CServerUserItem(enRemoteKind)
{
}

//析构函数
CRemoteUserItem::~CRemoteUserItem()
{
}

//重置数据
VOID CRemoteUserItem::ResetUserItem()
{
	//重置数据
	__super::ResetUserItem();
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserManager::CServerUserManager()
{
	//设置质数
	m_UserIDMap.InitHashTable(PRIME_PLATFORM_USER);

	return;
}

//析构函数
CServerUserManager::~CServerUserManager()
{
	//释放用户
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++)
	{
		CServerUserItem * pServerUserItem = m_UserItemStore[i];
		SafeDelete(pServerUserItem);
	}

	//释放用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		CServerUserItem * pServerUserItem = m_UserItemArray[i];
		SafeDelete(pServerUserItem);
	}

	//删除数据
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//枚举用户
CServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//查找用户
CServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	//查找对象
	CServerUserItem * pServerUserItem=NULL;
	m_UserIDMap.Lookup(dwUserID,pServerUserItem);

	return pServerUserItem;
}

//删除用户
bool CServerUserManager::DeleteUserItem()
{
	//存储对象
	m_UserItemStore.Append(m_UserItemArray);

	//删除对象
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();

	return true;
}

//删除用户
bool CServerUserManager::DeleteUserItem(DWORD dwUserID)
{
	//变量定义
	CServerUserItem * pTempUserItem=NULL;

	//寻找对象
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//获取用户
		pTempUserItem=m_UserItemArray[i];
		if (pTempUserItem->GetUserID()!=dwUserID) continue;

		//重置对象
		pTempUserItem->ResetUserItem();

		//删除对象
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//错误断言
	//ASSERT(FALSE);

	return false;
}

//插入用户
CLocalUserItem * CServerUserManager::InsertLocalUserItem(tagIMUserInfo & UserInfo, LPCWSTR pszPassword)
{
	//变量定义
	CLocalUserItem * pServerUserItem=NULL;

	//获取指针
	if (m_UserItemStore.GetCount()>0)
	{
		for (INT_PTR i=0; i<m_UserItemStore.GetCount(); i++)
		{
			if (m_UserItemStore[i]->GetUserItemKind()==enLocalKind)
			{
				pServerUserItem=(CLocalUserItem *)m_UserItemStore[i];
				m_UserItemStore.RemoveAt(i);
				break;
			}
		}
	}
	
	//创建对象
	if (pServerUserItem==NULL)
	{
		try
		{
			pServerUserItem=new CLocalUserItem;
		}
		catch (...)
		{
			//ASSERT(FALSE);
			return NULL;
		}
	}

	//属性变量
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));

	//辅助变量
	lstrcpynW(pServerUserItem->m_szLogonPass,pszPassword,CountArray(pServerUserItem->m_szLogonPass));

	//插入用户
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	return pServerUserItem;
}

//插入用户
CRemoteUserItem * CServerUserManager::InsertRemoteUserItem(tagIMUserInfo & UserInfo)
{
	//变量定义
	CRemoteUserItem * pServerUserItem=NULL;

	//获取指针
	if (m_UserItemStore.GetCount()>0)
	{
		for (INT_PTR i=0; i<m_UserItemStore.GetCount(); i++)
		{
			if (m_UserItemStore[i]->GetUserItemKind()==enRemoteKind)
			{
				pServerUserItem=(CRemoteUserItem *)m_UserItemStore[i];
				m_UserItemStore.RemoveAt(i);
				break;
			}
		}
	}
	
	//创建对象
	if (pServerUserItem==NULL)
	{
		try
		{
			pServerUserItem=new CRemoteUserItem;
		}
		catch (...)
		{
			//ASSERT(FALSE);
			return NULL;
		}
	}

	//属性变量
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));

	//插入用户
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	return pServerUserItem;
}

//////////////////////////////////////////////////////////////////////////////////
