#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////////////
//�û�����
enum enUserItemKind
{
	enNoneKind=0,
	enLocalKind,
	enRemoteKind,
};

//////////////////////////////////////////////////////////////////////////////////
//�û���Ϣ
class CServerUserItem
{
	//��Ԫ����
	friend class CServerUserManager;

	//��������
protected:	
	enUserItemKind					m_enUserKind;						//�û�����	
	DWORD							m_dwGroupID;						//Ⱥ���ʶ

	//�û���Ϣ
protected:
	tagIMUserInfo					m_UserInfo;							//�û���Ϣ
	DWORD							m_dwSocketID;						//�����־

	//��������
protected:
	//���캯��
	CServerUserItem(enUserItemKind enUserKind);
	//��������
	virtual ~CServerUserItem();

	//�û�״̬
public:	
	//��ȡ״̬
	virtual BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }
	//�޸�״̬
	virtual VOID SetUserStatus(BYTE cbUserStatus) { m_UserInfo.cbUserStatus=cbUserStatus; }
	//Ⱥ����Ϣ
public:
	//��ȡȺ��
	virtual DWORD GetActiveGroupID() { return m_dwGroupID; }
	//����Ⱥ��
	virtual VOID SetActiveGroupID(DWORD dwGroupID) { m_dwGroupID = dwGroupID; }
	//������Ϣ
public:
	//�û��Ա�
	virtual BYTE GetGender() { return m_UserInfo.cbGender; }
	//�û���ʶ
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//��Ϸ��ʶ
	virtual DWORD GetGameID() { return m_UserInfo.dwGameID; }
	//�û��ǳ�
	virtual LPCWSTR GetNickName() { return m_UserInfo.szNickName; }
	//�û���Ϣ
	virtual tagIMUserInfo * GetUserInfo() { return &m_UserInfo; }
	//�����־
	virtual DWORD  GetSocketID() { return m_dwSocketID; }
	//���������־
	virtual VOID SetSocketID(DWORD dwSocketID) { m_dwSocketID = dwSocketID; }
	//��������
protected:
	//��������
	virtual VOID ResetUserItem();

	//��������
public:
	//��ȡ����
	enUserItemKind GetUserItemKind() { return m_enUserKind; }
};

//////////////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class CLocalUserItem : public CServerUserItem
{
	//��Ԫ����
	friend class CServerUserManager;

	//���Ա���
protected:	
	CDWordArray						m_GroupIDArray;						//Ⱥ������
	
	//��������
protected:
	DWORD							m_dwLogonTime;						//��¼ʱ��
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//�û�����	

	//��������
protected:
	//���캯��
	CLocalUserItem();
	//��������
	virtual ~CLocalUserItem();

	//Ⱥ�����
public:
	//���Ⱥ��
	virtual bool InsertGroupID(DWORD dwGroupID);
	//�Ƴ�Ⱥ��
	virtual bool RemoveGroupID(DWORD dwGroupID);
	//�Ƴ�Ⱥ��
	virtual VOID RemoveGroupID() { m_GroupIDArray.RemoveAll(); }
	//Ⱥ������
	virtual DWORD * GetGroupData() { return m_GroupIDArray.GetData(); };
	//Ⱥ������
	virtual WORD GetGroupCount() { return (WORD)m_GroupIDArray.GetCount(); }	

	//��¼��Ϣ
public:
	//��¼ʱ��
	virtual DWORD GetLogonTime() { return m_dwLogonTime; }
	//����ʱ��
	virtual VOID SetLogonTime(DWORD dwLogonTime) { m_dwLogonTime=dwLogonTime; }

	//Ч��ӿ�
public:
	//�Ա��ʺ�
	virtual bool ContrastNickName(LPCWSTR pszNickName);
	//�Ա�����
	virtual bool ContrastLogonPass(LPCWSTR pszPassword);

	//��������
public:
	//�޸�����
	VOID ModifyLogonPassword(LPCWSTR pszPassword);	
	
	//��������
protected:	
	//��������
	virtual VOID ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class CRemoteUserItem: public CServerUserItem
{
	//��Ԫ����
	friend class CServerUserManager;

	//��������
protected:
	//���캯��
	CRemoteUserItem();
	//��������
	virtual ~CRemoteUserItem();

	//��������
protected:
	//��������
	virtual VOID ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////////////

//�û�������
typedef CWHArray<CServerUserItem *> CServerUserItemArray;
typedef CMap<DWORD,DWORD,CServerUserItem *,CServerUserItem *> CServerUserItemMap;

//�û�������
class CServerUserManager
{
	//�û�����
protected:
	CServerUserItemMap				m_UserIDMap;						//�û�����
	CServerUserItemArray			m_UserItemArray;					//�û�����
	CServerUserItemArray			m_UserItemStore;					//�洢�û�

	//��������
public:
	//���캯��
	CServerUserManager();
	//��������
	virtual ~CServerUserManager();

	//���Һ���
public:
	//ö���û�
	virtual CServerUserItem * EnumUserItem(WORD wEnumIndex);
	//�����û�
	virtual CServerUserItem * SearchUserItem(DWORD dwUserID);

	//ͳ�ƺ���
public:
	//��������
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }

	//������
public:
	//ɾ���û�
	virtual bool DeleteUserItem();
	//ɾ���û�
	virtual bool DeleteUserItem(DWORD dwUserID);
	//�����û�
	virtual CRemoteUserItem * InsertRemoteUserItem(tagIMUserInfo & UserInfo);
	//�����û�
	virtual CLocalUserItem * InsertLocalUserItem(tagIMUserInfo & UserInfo,LPCWSTR pszPassword);	
};

//////////////////////////////////////////////////////////////////////////////////
#endif