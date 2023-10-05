
#ifndef CLUB_SERVER_DATA_FILE
#define CLUB_SERVER_DATA_FILE

#pragma once

#include "Stdafx.h"
//
//��������
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

//��˵��
class CLUB_SERVER_DATA_CLASS CGroupItem;
class CLUB_SERVER_DATA_CLASS CGroupInfoManager;

//���鶨��
typedef CWHArray<CGroupItem *> CGroupItemArray;
typedef CWHArray<tagIMGroupMember> CGroupMemberArray;
typedef CWHArray<tagGroupConfigInfo> CGroupConfigArray;
typedef CWHArray<tagClubRollMsg> CGroupRollMsgArray;
//��������
typedef CMap<DWORD, DWORD, CGroupItem *, CGroupItem *> CMapGroupID;

//���Ͷ���
struct tagTableUserInfo
{
	DWORD           dwKindID;							//����ID
	DWORD			dwServerID;							//����I D
	WORD			wTableID;							//����I D
	DWORD			dwPersonalRoomID;					//Լս����ID
	DWORD			dwGroupID;							//������ID
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
//Ⱥ������
class CLUB_SERVER_DATA_CLASS ICGroupItemManager :public IUnknownEx
{
	

	//Ⱥ������
public:
	//Ⱥ���ʶ
	virtual DWORD GetGroupID() = 0;
	//Ⱥ������
	virtual tagIMGroupItem * GetGroupItem() = 0;
	//Ⱥ��Ƹ�
	virtual tagIMGroupWealth * GetGroupWealth() = 0;
	//Ⱥ������
	virtual tagIMGroupProperty * GetGroupProperty() = 0;
	//Ⱥ�����
	virtual tagIMGroupBattleRule * GetGroupBattleRule() = 0;
	//��������
	virtual void AddRollMsg(tagClubRollMsg &RollMsg) = 0;
	//���������
	virtual tagClubRollMsg * SearchRollMsg(DWORD dwUserID) = 0;
	//���ù���
	virtual void SetNotice(TCHAR* pNotice) = 0;
	//��ȡ����
	virtual TCHAR *GetNotice() = 0;
	//��Ա����
public:
	//�Ƴ���Ա
	virtual bool RemoveMember(DWORD dwMemberID) = 0;
	//��ӳ�Ա
	virtual bool AddMember(tagIMGroupMember & GroupMember) = 0;
	//ö�ٳ�Ա
	virtual tagIMGroupMember * EnumMember(INT nIndex) = 0;
	//���ҳ�Ա
	virtual tagIMGroupMember * SearchMember(DWORD dwMemberID) = 0;
	//�������
	virtual bool AddPersonalTable(DWORD dwPersonalRoomID, tagClubTableInfo TableInfo) = 0;
	//ɾ������
	virtual bool RemovePersonalTable(DWORD dwPersonalRoomID) = 0;
	//ɾ������
	virtual bool RemovePersonalTable(WORD wServerID) = 0;
	//��ȡ������Ϣ
	virtual bool GetTableInfo(DWORD dwPersonalRoomID, tagClubTableInfo &TableInfo) = 0;
	//�������״̬
	virtual bool UpdateTableUserInfo(CMD_CS_S_UpdateTable *pUpdateTable) = 0;
	//��ȡ��Ա��Ϸ״̬
	virtual bool IsUserInGame(DWORD dwUserID, tagClubTableInfo &TableInfo, bool bPlaying = false) = 0;
	//��ȡȺ����ʶ
	virtual DWORD GetGroupCreaterID() = 0;
	//��ȡ�淨ID
	virtual DWORD GetConfigID(DWORD dwPersonalRoomID) = 0;
	//��ȡ�淨��Ŀ
	virtual DWORD GetConfigCount() = 0;
	//��ȡ������Ϣ
	virtual CMapClubTableInfo* GetCMapClubTableInfo() = 0;

	//�淨����
public:
	//����淨
	virtual bool AddConfig(tagGroupConfigInfo & GroupConfig) = 0;
	//�޸��淨
	virtual bool ModifyConfig(tagGroupConfigInfo & GroupConfig) = 0;
	//ɾ���淨
	virtual bool RemoveConfig(DWORD dwConfigID) = 0;
	//ɾ���淨
	virtual bool RemoveConfig(INT nIndex) = 0;
	//ö���淨
	virtual tagGroupConfigInfo * EnumConfig(INT nIndex) = 0;
	//�����淨
	virtual tagGroupConfigInfo * SearchConfig(DWORD dwConfigID) = 0;
	//��ѯ��������
	virtual INT GetNullTableCount(DWORD dwConfigID) = 0;
	//ö������
	virtual tagClubTableInfo EnumTable(POSITION &pos) = 0;
	//��������
protected:
	//���ö���
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

//Ⱥ��������
typedef CWHArray<CGroupItem *> CGroupItemArray;
typedef CMap<DWORD, DWORD, CGroupItem *, CGroupItem *> CGroupItemMap;

//Ⱥ�������
class CLUB_SERVER_DATA_CLASS ICGroupInfoManagerManager : public IUnknownEx
{
	
public:
	//����Ⱥ��
	virtual CGroupItem * SearchGroupItem(DWORD dwGroupID) = 0;
	//ö��Ⱥ��
	virtual CGroupItem * EnumGroupItem(POSITION & Position) = 0;

	//ͳ�ƺ���
public:
	//Ⱥ������
	virtual DWORD GetGroupItemCount() = 0;

	//������
public:
	//�Ƴ�Ⱥ��
	virtual bool RemoveGroupItem() = 0;
	//�Ƴ�Ⱥ��
	virtual bool RemoveGroupItem(DWORD dwGroupID) = 0;
	//����Ⱥ��
	virtual bool ActiveGroupItem(tagIMGroupItem & GroupItem) = 0;
	//��ȡ�Ƽ�
	virtual WORD GetRecommendGroup(DWORD dwUserID, WORD wRecomCount, tagIMGroupProperty *pGroupProperty) = 0;

public:
	//У��
	virtual int CheckValid() = 0;
};

//�����ļ�
#ifndef SHARE_STORAGE_SERVICE_DLL
#include "ClubServerDataInfoManager.h"
#endif
//////////////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////////////

