#include "StdAfx.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CServerUserItem::CServerUserItem(enUserItemKind enUserKind)
{
	//���ñ���	
	m_enUserKind=enUserKind;
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	m_dwSocketID = 0;
	m_dwGroupID = 0;

}

//��������
CServerUserItem::~CServerUserItem()
{
}

//��������
VOID CServerUserItem::ResetUserItem()
{
	//���ñ���	
	m_enUserKind=enNoneKind;
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CLocalUserItem::CLocalUserItem():CServerUserItem(enLocalKind)
{
	//��������
	m_GroupIDArray.RemoveAll();

	//��������
	m_dwLogonTime=0;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));	

	return;
}

//��������
CLocalUserItem::~CLocalUserItem()
{
}

//���Ⱥ��
bool CLocalUserItem::InsertGroupID(DWORD dwGroupID)
{
	//����Ⱥ��
	for (INT_PTR i=0; i<m_GroupIDArray.GetCount(); i++)
	{
		if (m_GroupIDArray[i]==dwGroupID)
		{
			return false;
		}
	}

	//���Ⱥ��
	m_GroupIDArray.Add(dwGroupID);

	return true;
}

//�Ƴ�Ⱥ��
bool CLocalUserItem::RemoveGroupID(DWORD dwGroupID)
{
	//����Ⱥ��
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

//�Ա��ʺ�
bool CLocalUserItem::ContrastNickName(LPCWSTR pszNickName)
{
	//Ч�����
	//ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//���ȶԱ�
	INT nContrastLen=lstrlenW(pszNickName);
	INT nSourceLen=lstrlenW(m_UserInfo.szNickName);

	//�ַ��Ա�
	if (nContrastLen!=nSourceLen) return false;
	if (CompareStringW(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszNickName,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//�Ա�����
bool CLocalUserItem::ContrastLogonPass(LPCWSTR pszPassword)
{
	//Ч�����
	//ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//���ȶԱ�
	INT nTargetLen=lstrlenW(pszPassword);
	INT nSourceLen=lstrlenW(m_szLogonPass);

	//����Ա�
	if (nTargetLen!=nSourceLen) return false;
	if (CompareStringW(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszPassword,nTargetLen,m_szLogonPass,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//��������
VOID CLocalUserItem::ResetUserItem()
{
	//��������
	__super::ResetUserItem();

	//��������
	m_GroupIDArray.RemoveAll();

	//��������
	m_dwLogonTime=0;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));	

	return;
}

//�޸�����
VOID CLocalUserItem::ModifyLogonPassword(LPCWSTR pszPassword)
{
	//�����ַ�
	lstrcpynW(m_szLogonPass,pszPassword,CountArray(m_szLogonPass));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CRemoteUserItem::CRemoteUserItem(): CServerUserItem(enRemoteKind)
{
}

//��������
CRemoteUserItem::~CRemoteUserItem()
{
}

//��������
VOID CRemoteUserItem::ResetUserItem()
{
	//��������
	__super::ResetUserItem();
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerUserManager::CServerUserManager()
{
	//��������
	m_UserIDMap.InitHashTable(PRIME_PLATFORM_USER);

	return;
}

//��������
CServerUserManager::~CServerUserManager()
{
	//�ͷ��û�
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++)
	{
		CServerUserItem * pServerUserItem = m_UserItemStore[i];
		SafeDelete(pServerUserItem);
	}

	//�ͷ��û�
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		CServerUserItem * pServerUserItem = m_UserItemArray[i];
		SafeDelete(pServerUserItem);
	}

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//ö���û�
CServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//�����û�
CServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	//���Ҷ���
	CServerUserItem * pServerUserItem=NULL;
	m_UserIDMap.Lookup(dwUserID,pServerUserItem);

	return pServerUserItem;
}

//ɾ���û�
bool CServerUserManager::DeleteUserItem()
{
	//�洢����
	m_UserItemStore.Append(m_UserItemArray);

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();

	return true;
}

//ɾ���û�
bool CServerUserManager::DeleteUserItem(DWORD dwUserID)
{
	//��������
	CServerUserItem * pTempUserItem=NULL;

	//Ѱ�Ҷ���
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//��ȡ�û�
		pTempUserItem=m_UserItemArray[i];
		if (pTempUserItem->GetUserID()!=dwUserID) continue;

		//���ö���
		pTempUserItem->ResetUserItem();

		//ɾ������
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//�������
	//ASSERT(FALSE);

	return false;
}

//�����û�
CLocalUserItem * CServerUserManager::InsertLocalUserItem(tagIMUserInfo & UserInfo, LPCWSTR pszPassword)
{
	//��������
	CLocalUserItem * pServerUserItem=NULL;

	//��ȡָ��
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
	
	//��������
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

	//���Ա���
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));

	//��������
	lstrcpynW(pServerUserItem->m_szLogonPass,pszPassword,CountArray(pServerUserItem->m_szLogonPass));

	//�����û�
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	return pServerUserItem;
}

//�����û�
CRemoteUserItem * CServerUserManager::InsertRemoteUserItem(tagIMUserInfo & UserInfo)
{
	//��������
	CRemoteUserItem * pServerUserItem=NULL;

	//��ȡָ��
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
	
	//��������
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

	//���Ա���
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));

	//�����û�
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	return pServerUserItem;
}

//////////////////////////////////////////////////////////////////////////////////
