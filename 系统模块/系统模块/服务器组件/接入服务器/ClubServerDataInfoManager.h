
#ifndef CLUB_SERVER_DATA_MANAGER_FILE
#define CLUB_SERVER_DATA_MANAGER_FILE

#pragma once

#include "Stdafx.h"
#include "ClubServerDataInfo.h"



//////////////////////////////////////////////////////////////////////////////////
//Ⱥ������
class CLUB_SERVER_DATA_CLASS CGroupItem : public ICGroupItemManager
{
	//��Ԫ����
	friend class CGroupInfoManager;

	//Ⱥ������
protected:
	tagIMGroupItem						m_GroupItem;						//Ⱥ������

	//Ⱥ���Ա
protected:
	CGroupMemberArray					m_GroupMemberArray;					//��Ա����
	CGroupConfigArray					m_GroupConfigArray;					//�淨����
	CMapClubTableInfo					m_MapClubTableInfo;					//������Ϣ
	CGroupRollMsgArray					m_GroupRollMsgArray;				//�������Ϣ
protected:
	TCHAR								m_szNotice[109];					//��������
	//��������
protected:
	//���캯��
	CGroupItem();
	//��������
	virtual ~CGroupItem();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	//Ⱥ������
public:
	//Ⱥ���ʶ
	virtual DWORD GetGroupID() { return m_GroupItem.GroupProperty.dwGroupID; }
	//Ⱥ������
	virtual tagIMGroupItem * GetGroupItem() { return &m_GroupItem; }
	//Ⱥ��Ƹ�
	virtual tagIMGroupWealth * GetGroupWealth() { return &m_GroupItem.GroupWealth; }
	//Ⱥ������
	virtual tagIMGroupProperty * GetGroupProperty() { return &m_GroupItem.GroupProperty; }
	//Ⱥ�����
	virtual tagIMGroupBattleRule * GetGroupBattleRule() { return &m_GroupItem.GroupBattleRule; }
	//��������
	virtual void AddRollMsg(tagClubRollMsg &RollMsg);
	//���������
	virtual tagClubRollMsg * SearchRollMsg(DWORD dwUserID);
	//���ù���
	virtual void SetNotice(TCHAR* pNotice);
	//��ȡ����
	virtual TCHAR *GetNotice();
	//��Ա����
public:
	//�Ƴ���Ա
	virtual bool RemoveMember(DWORD dwMemberID);
	//��ӳ�Ա
	virtual bool AddMember(tagIMGroupMember & GroupMember);
	//ö�ٳ�Ա
	virtual tagIMGroupMember * EnumMember(INT nIndex);
	//���ҳ�Ա
	virtual tagIMGroupMember * SearchMember(DWORD dwMemberID);
	//�������
	virtual bool AddPersonalTable(DWORD dwPersonalRoomID, tagClubTableInfo TableInfo);
	//ɾ������
	virtual bool RemovePersonalTable(DWORD dwPersonalRoomID);
	//ɾ������
	virtual bool RemovePersonalTable(WORD wServerID);
	//��ȡ������Ϣ
	virtual bool GetTableInfo(DWORD dwPersonalRoomID, tagClubTableInfo &TableInfo);
	//�������״̬
	virtual bool UpdateTableUserInfo(CMD_CS_S_UpdateTable *pUpdateTable);
	//��ȡ��Ա��Ϸ״̬
	virtual bool IsUserInGame(DWORD dwUserID, tagClubTableInfo &TableInfo, bool bPlaying = false);
	//��ȡȺ����ʶ
	virtual DWORD GetGroupCreaterID() { return m_GroupItem.GroupProperty.dwCreaterID; }
	//��ȡ�淨ID
	virtual DWORD GetConfigID(DWORD dwPersonalRoomID);
	//��ȡ�淨��Ŀ
	virtual DWORD GetConfigCount() { return m_GroupConfigArray.GetCount(); }
	//��ȡ������Ϣ
	virtual CMapClubTableInfo* GetCMapClubTableInfo() { return &m_MapClubTableInfo; }

	//�淨����
public:
	//����淨
	virtual bool AddConfig(tagGroupConfigInfo & GroupConfig);
	//�޸��淨
	virtual bool ModifyConfig(tagGroupConfigInfo & GroupConfig);
	//ɾ���淨
	virtual bool RemoveConfig(DWORD dwConfigID);
	//ɾ���淨
	virtual bool RemoveConfig(INT nIndex);
	//ö���淨
	virtual tagGroupConfigInfo * EnumConfig(INT nIndex);
	//�����淨
	virtual tagGroupConfigInfo * SearchConfig(DWORD dwConfigID);
	//��ѯ��������
	virtual INT GetNullTableCount(DWORD dwConfigID);
	//ö������
	virtual tagClubTableInfo EnumTable(POSITION &pos);
	//��������
protected:
	//���ö���
	virtual void ResetGroupItem();
};

//////////////////////////////////////////////////////////////////////////////////



//Ⱥ�������
class CLUB_SERVER_DATA_CLASS CGroupInfoManager :public ICGroupInfoManagerManager
{
	//�û�����
protected:
	CGroupItemMap					m_GroupIDMap;						//Ⱥ������	
	CGroupItemArray					m_GroupItemStore;					//�洢Ⱥ��

	//��������
public:
	//���캯��
	CGroupInfoManager();
	//��������
	virtual ~CGroupInfoManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	//���Һ���
public:
	//����Ⱥ��
	virtual CGroupItem * SearchGroupItem(DWORD dwGroupID);
	//ö��Ⱥ��
	virtual CGroupItem * EnumGroupItem(POSITION & Position);

	//ͳ�ƺ���
public:
	//Ⱥ������
	virtual DWORD GetGroupItemCount() { return (DWORD)m_GroupIDMap.GetCount(); }

	//������
public:
	//�Ƴ�Ⱥ��
	virtual bool RemoveGroupItem();
	//�Ƴ�Ⱥ��
	virtual bool RemoveGroupItem(DWORD dwGroupID);
	//����Ⱥ��
	virtual bool ActiveGroupItem(tagIMGroupItem & GroupItem);
	//��ȡ�Ƽ�
	virtual WORD GetRecommendGroup(DWORD dwUserID, WORD wRecomCount, tagIMGroupProperty *pGroupProperty);

public:
	//У��
	virtual int CheckValid();
};
//////////////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////////////

