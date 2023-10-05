//�ر���IT������28xin.com��
#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once
#include <math.h>
#include "AfxTempl.h"

#define PI                      3.1415926		//�г���
#define EARTH_RADIUS            6378.137        //����뾶
//����γ��
class Cltladistance
{
public:
	Cltladistance();
	virtual ~Cltladistance();

public:
	//��ȡ����
    inline double GetRadian(double d);
	//��������
	inline DWORD GetRound(double number);
	//��ȡ����
	inline double GetDistance(double lat1, double lng1, double lat2, double lng2);
};
//////////////////////////////////////////////////////////////////////////////////
//�û�����
enum enUserItemKind
{
	enNoneKind=0,
	enLocalKind,
	enRemoteKind,
};

//�����û�
struct tagInsertLocalUserInfo
{
	DWORD								dwSocketID;						//���ӱ�ʶ
	DWORD								dwLogonTime;					//��¼ʱ��
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

	//��������
protected:
	//���캯��
	CServerUserItem(enUserItemKind enUserKind);
	//��������
	virtual ~CServerUserItem();

	//�û�����
public:
	//�û���ʶ
	virtual DWORD GetUserID()=NULL;

	//������Ϣ
public:
	//�޸�״̬
	virtual VOID SetMainStatus(BYTE cbMainStatus)=NULL;
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID)=NULL;
	//�û�״̬
	virtual BYTE GetMainStatus()=NULL;
	//�û�״̬
	virtual BYTE GetGameStatus()=NULL;
	//�����ʶ
	virtual WORD GetServerID()=NULL;
	//���ӱ�ʶ
	virtual WORD GetTableID()=NULL;
	//���ӵ�ַ
	virtual DWORD GetClientAddr()=NULL;

	//��������
private:
	//��������
	virtual VOID ResetUserItem()=NULL;

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
	tagFriendUserInfo				m_UserInfo;							//�û���Ϣ
	DWORD							m_dwSocketID;						//���ӱ�ʶ
	LONGLONG						m_lMatchNO;							//�������

	//��Ϸ��Ϣ
protected:
	WORD							m_wServerID;						//�����ʶ
	WORD							m_wTableID;							//���ӱ�ʶ

	//��������
protected:
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//�û�����
	DWORD							m_dwLogonTime;						//��¼ʱ��
	TCHAR							m_szFaceUrl[LEN_FACE_URL];			//ͷ���ַ						

	//��������
protected:
	//���캯��
	CLocalUserItem();
	//��������
	virtual ~CLocalUserItem();

	//�û���Ϣ
public:
	//�û���Ϣ
	virtual tagFriendUserInfo * GetUserInfo() { return &m_UserInfo; }

	//������Ϣ
public:
	//�û��Ա�
	virtual BYTE GetGender() { return m_UserInfo.cbGender; }
	//�û���ʶ
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//�û���ʶ
	virtual DWORD GetGameID() { return m_UserInfo.dwGameID; }
	//�û��ǳ�
	virtual LPCTSTR GetNickName() { return m_UserInfo.szNickName; }

	//������Ϣ
public:
	//�޸�״̬
	virtual VOID SetMainStatus(BYTE cbMainStatus) { m_UserInfo.cbMainStatus=cbMainStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID);
	//���ӱ�ʶ
	virtual DWORD GetSocketID() { return m_dwSocketID; }
	//�����ʶ
	virtual WORD GetServerID() { return m_wServerID; }
	//���ӱ�ʶ
	virtual WORD GetTableID() { return m_wTableID; }	
	//�û�״̬
	virtual BYTE GetMainStatus() { return m_UserInfo.cbMainStatus; }
	//�û�״̬
	virtual BYTE GetGameStatus() { return m_UserInfo.cbGameStatus; }	
	//���ӵ�ַ
	virtual DWORD GetClientAddr() { return m_UserInfo.dwClientAddr; }
	//�������
	virtual LONGLONG GetMatchNo() {	return m_lMatchNO; }

	//Ч��ӿ�
public:
	//�Ա��ʺ�
	virtual bool ContrastNickName(LPCTSTR pszNickName);
	//�Ա�����
	virtual bool ContrastLogonPass(LPCTSTR pszPassword);

	//��������
public:
	//�޸�����
	VOID ModifyLogonPassword(LPCTSTR pszPassword);
	//���ñ�ʶ
	virtual VOID SetSocketID(DWORD dwSocketID) { m_dwSocketID=dwSocketID; }
	
	//��¼��Ϣ
public:
	//��¼ʱ��
	virtual DWORD GetLogonTime() { return m_dwLogonTime; }
	//����ʱ��
	virtual VOID SetLogonTime(DWORD dwLogonTime) { m_dwLogonTime=dwLogonTime; }
	//���ñ��
	virtual VOID SetMatchNo(LONGLONG lMatchNo) { m_lMatchNO = lMatchNo;}

	//��������
private:	
	//��������
	virtual VOID ResetUserItem();

	//ͷ���ַ
public:
	//����ͷ���ַ
	virtual void SetFaceUrl(LPCTSTR  pFaceUrl) { lstrcpyn(m_szFaceUrl, pFaceUrl, CountArray(m_szFaceUrl)); }
	//��ȡͷ���ַ
	virtual LPCTSTR GetFaceUrl() { return m_szFaceUrl; }
};

//////////////////////////////////////////////////////////////////////////////////

//�û�������
typedef CWHArray<CServerUserItem *> CServerUserItemArray;
typedef CMap<DWORD,DWORD,CServerUserItem *,CServerUserItem *> CServerUserItemMap;

typedef CWHArray<tagNearUserInfo *> CNearUserInfoItemArray;
typedef CMap<DWORD,DWORD,tagNearUserInfo *,tagNearUserInfo *> CNearUserInfoItemMap;

//�����û���
class CNearUserManager
{
	//�û�����
protected:
	CNearUserInfoItemArray				m_UserItemArray;					//�û�����
	//��������
public:
	//���캯��
	CNearUserManager();
	//��������
	virtual ~CNearUserManager();
public:
	//ö���û�
	virtual tagNearUserInfo * EnumUserItem(DWORD dwEnumIndex);
	//�����û�
	virtual bool InsertNearUserItem(tagNearUserInfo & NearUserItem);
	//��������
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }
};

//�û�������
class CServerUserManager
{
	//�û�����
protected:
	CServerUserItemMap				m_UserIDMap;						//�û�����
	CServerUserItemArray			m_UserItemArray;					//�û�����
	CServerUserItemArray			m_UserItemStore;					//�洢�û�
	Cltladistance					m_ltladistance;						//��ȡ����

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
	virtual bool InsertLocalUserItem(tagInsertLocalUserInfo & InsertLocalUserInfo,tagFriendUserInfo & UserInfo,LPCTSTR szPassword);

	//��������
public:
	//��������
	virtual DWORD GetNearUserItem(DWORD dwUserID,CNearUserManager & NearUserManager);
	//��ѯ����
	virtual DWORD QueryNearUserItem(DWORD dwUserID,DWORD dwNearUserID,CNearUserManager & NearUserManager);
};

//////////////////////////////////////////////////////////////////////////////////

//���ѷ���
class CUserFriendGroup
{
	//��Ԫ����
	friend class CFriendGroupManager;

	//��������
protected:
	DWORD										m_dwOwnerUserID;				//�û���ʶ
	WORD										m_wGroupContent;				//��������
	WORD										m_wFriendCount;					//������Ŀ

protected:
	tagServerFriendInfo *						m_pFriendInfo;					//�û���Ϣ	

	//��������
public:
	//���캯��
	CUserFriendGroup();
	//��������
	virtual ~CUserFriendGroup();

	//��������
public:
	//���ñ�ʶ
	VOID SetOwnerUserID(DWORD dwUserID) { m_dwOwnerUserID=dwUserID; }
	//��ȡ��ʶ
	DWORD GetOwnerUserID() { return m_dwOwnerUserID; }
	//��ȡ����
	WORD GetGroupContent() { return m_wGroupContent; }
	//��ȡ��Ŀ
	WORD GetFriendCount() { return m_wFriendCount; }

	//���ܺ���
public:
	//��ʼ������
	bool InitFriendGroup(WORD wGroupContent);
	//��Ӻ���
	bool AppendFriendInfo(tagServerFriendInfo & FriendInfo);
	//�Ƴ�����
	bool RemoveFriendInfo(DWORD dwUserID);
	//���Һ���
	tagServerFriendInfo * SearchFriendItem(DWORD dwUserID);
	//ö�ٺ���
	tagServerFriendInfo * EnumFriendItem(WORD wEnumIndex);
	//���÷���
	VOID ResetFriendGroup();


	//��������
private:
	//��������
	tagServerFriendInfo * AllocateContent(WORD wGroupContent);
	//��չ����
	bool ExtendGroupContent(WORD wExtendCount);	
};

//////////////////////////////////////////////////////////////////////////////////

//�û�������
typedef CWHArray<CUserFriendGroup *> CUserFriendGroupArray;
typedef CMap<DWORD,DWORD,CUserFriendGroup *,CUserFriendGroup *> CUserFriendGroupMap;

//���ѹ�����
class CFriendGroupManager
{
	//�û�����
protected:
	CUserFriendGroupMap				m_UserIDMap;						//�û�����
	CUserFriendGroupArray			m_UserFriendGroupArray;				//�û�����
	CUserFriendGroupArray			m_UserFriendGroupStore;				//�洢�û�

	//��������
public:
	//���캯��
	CFriendGroupManager();
	//��������
	virtual ~CFriendGroupManager();

	//���Һ���
public:
	//ö���û�
	virtual CUserFriendGroup * EnumGroupItem(WORD wEnumIndex);
	//�����û�
	virtual CUserFriendGroup * SearchGroupItem(DWORD dwUserID);

	//������
public:
	//ɾ���û�
	virtual bool DeleteFriendGroup();
	//ɾ���û�
	virtual bool DeleteFriendGroup(DWORD dwUserID);
	//�������
	virtual bool InsertFriendGroup(DWORD dwUserID,CUserFriendGroup * pUserFriendGroup);

	//��������
public:
	//��ȡ����
	CUserFriendGroup * ActiveFriendGroup(WORD wFriendCount);
};

//////////////////////////////////////////////////////////////////////////////////
//����

class CGameMatchItem
{
	//��������
public:
	bool							m_bDisuse;							//������־

	//��������
protected:
	//���캯��
	CGameMatchItem()
	{
		m_bDisuse = false;
	}
};

//��������
class CMatchOptionItem : public CGameMatchItem
{
	//��������
public:
	tagMatchSignUpCount				m_GameMatchOption;

	//���ܺ���
public:
	//��������
public:
	//���캯��
	CMatchOptionItem();
};

//��������
class CMatchServerItem : public CGameMatchItem
{
	//��������
public:
	tagGameMatch					m_GameMatch;						//������Ϣ

	//���ܺ���
public:
	//��������
public:
	//���캯��
	CMatchServerItem();
};

//����˵��
typedef CWHArray<CMatchOptionItem *>			CMatchOptionItemArray;
typedef CWHArray<CMatchServerItem *>			CMatchServerItemArray;
//����˵��
typedef CMap<WORD, WORD, CMatchOptionItem *, CMatchOptionItem *&>			CMatchOptionItemMap;
typedef CMap<WORD, WORD, CMatchServerItem *, CMatchServerItem *&>			CMatchServerItemMap;

//��������
class CMatchServerManager
{
	//��������
protected:
	CMatchOptionItemMap				m_MatchOptionItemMap;					//��������
	CMatchServerItemMap				m_MatchServerItemMap;					//��������

	//�洢����
protected:
	CMatchOptionItemArray			m_MatchOptionItemBuffer;				//��������
	CMatchServerItemArray			m_MatchServerItemBuffer;				//��������

	//��������
public:
	//���캯��
	CMatchServerManager();
	//��������
	virtual ~CMatchServerManager();

	//����ӿ�
public:
	//�����б�
	VOID ResetMatchData();
	//�����ں�
	VOID CleanKernelItem();
	//������
	VOID CleanServerItem();
	//�����ں�
	VOID DisuseKernelItem();
	//��������
	VOID DisuseServerItem();

	//	//ͳ�ƽӿ�
	//public:
	//	//ͳ������
	//	DWORD CollectOnlineInfo(bool bAndroid);
	//	//��������
	//	DWORD CollectOnlineInfo(WORD wKindID, bool bAndroid);
	//	//ͳ����������
	//	DWORD CollectSetPlayer(WORD wKindID);
	//����ӿ�
public:
	//��������
	bool InsertMatchOption(tagMatchSignUpCount *pMatchOption);
	//���뷿��
	bool InsertMatchServer(tagGameMatch *pGameServer);

	//ɾ���ӿ�
public:
	//ɾ������
	bool DeleteMatchOption(WORD wPageID);
	//ɾ������
	bool DeleteMatchServer(WORD wServerID);

	//ö�ٽӿ�
public:
	//ö������
	CMatchOptionItem *EmunMatchOptionItem(POSITION &Position);
	//ö�ٷ���
	CMatchServerItem *EmunMatchServerItem(POSITION &Position);

	//���ҽӿ�
public:
	//��������
	CMatchOptionItem *SearchMatchOption(WORD wServerID);
	//���ұ���
	CMatchServerItem *SearchMatchServer(WORD wServerID);

	//��Ŀ�ӿ�
public:
	//������Ŀ
	DWORD GetMatchOptionCount()
	{
		return (DWORD)m_MatchOptionItemMap.GetCount();
	}
	//������Ŀ
	DWORD GetMatchServerCount()
	{
		return (DWORD)m_MatchServerItemMap.GetCount();
	}
};

//////////////////////////////////////////////////////////////////////////////////

//�û�������
typedef CWHArray<tagPersonalTableInfoEx *> CPersonalTableInfoEx;
typedef CMap<DWORD, DWORD, CPersonalTableInfoEx *, CPersonalTableInfoEx *> CServerPersonalTableMap;
//�û�������
class CPersonalRoomManager
{
	//�û�����
protected:
	CServerPersonalTableMap			m_PersonalTableMap;						//�û�����
	CPersonalTableInfoEx			m_PersonalTableArray;				//�û�����

	//��������
public:
	//���캯��
	CPersonalRoomManager();
	//��������
	virtual ~CPersonalRoomManager();

	//���Һ���
public:
	//ö���û�
	virtual tagPersonalTableInfoEx * EnumPersonalTableItem(DWORD wKindID, BYTE cbPlayMode, int nPos);

	//��ȡ�����ͷ�������
	virtual int GetPersonalTableCount(DWORD wKindID);

	//������
public:
	//ɾ���û�
	virtual bool DeletePersonalTableItem(DWORD wKindID, DWORD wServerID, DWORD dwRoomID);

	//�����û�
	virtual bool InsertPersonalTableItem(tagPersonalTableInfoEx * pPersonalTableInfoEx);
};


#endif