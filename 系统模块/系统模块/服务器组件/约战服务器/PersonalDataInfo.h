
#ifndef PERSONAL_DATA_FILE
#define PERSONAL_DATA_FILE

#include "..\..\ȫ�ֶ���\Platform.h"

//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_Correspond.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"

//ƽ̨�ļ�
#include "..\..\�������\�������\ServiceCoreHead.h"
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"
#include "../Լս������/GlobalInfoManager.h"

//��������
/*
#ifndef PERSONAL_DATA_CLASS
	#ifdef  PERSONAL_DATA_DLL
		#define PERSONAL_DATA_CLASS _declspec(dllexport)
	#else
		#define PERSONAL_DATA_CLASS _declspec(dllimport)
	#endif
#endif*/
#pragma once


//ģ�鶨��
/*
#ifndef _DEBUG
#define PERSONAL_DATA_DLL_NAME		TEXT("PersonalData.dll")			//�������
#else
#define PERSONAL_DATA_DLL_NAME		TEXT("PersonalDataD.dll")		//�������
#endif*/

#ifdef _UNICODE
#define VER_IPersonalDataManager	INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalDataManager = { 0xaddb9939, 0xd474, 0x4fc9, 0x88, 0x2c, 0x7a, 0x4, 0x3a, 0xdf, 0x8b, 0x25 };
#else
#define VER_IPersonalDataManager	INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalDataManager = { 0xaddb9939, 0xd474, 0x4fc9, 0x88, 0x2c, 0x7a, 0x4, 0x3a, 0xdf, 0x8b, 0x25 };
#endif


//AI��Ϣ
interface /*PERSONAL_DATA_CLASS*/ IPersonalDataManager : public IUnknownEx
{


	//������
public:
	//��������
	virtual VOID ResetData() = 0;
	//�û���Ŀ
	virtual DWORD GetUserItemCount() = 0;
	//������Ŀ
	virtual DWORD GetPlazaItemCount() = 0;
	//������Ŀ
	virtual DWORD GetServerItemCount() = 0;
	//������Ŀ
	virtual DWORD GetChatItemCount() = 0;

	//˽�˷���
public:
	//��������
	virtual tagPersonalTableInfo* SearchTableByID(DWORD dwRoomID) = 0;
	//��������
	virtual tagPersonalTableInfo* SearchTableByTableIDAndServerID(DWORD dwServerID, DWORD dwTableID) = 0;
	//�������
	virtual bool AddFreeServerTable(DWORD dwServerID) = 0;
	//�������
	virtual bool AddServerTable(DWORD dwRoomID, tagPersonalTableInfo PersonalTable) = 0;
	//�Ƴ�����
	virtual bool RemoveFreeServerTable(DWORD dwServerID) = 0;
	//�Ƴ�����
	virtual bool RemoveServerTable(DWORD dwServerID, DWORD dwTableID) = 0;
	//�Ƴ�����
	virtual bool RemoveServerTable(DWORD dwServerID) = 0;
	//��ȡ����
	virtual DWORD GetFreeServer(DWORD dwUserID, DWORD dwKindID, BYTE cbIsJoinGame, BYTE cbPlayMode, bool bSupportGroup) = 0;
	//��ȡ������Ŀ
	virtual DWORD GetFreeServerCount(DWORD dwKindID, BYTE cbPlayMode, bool bSupportGroup) = 0;
	//���ɷ���ID
	virtual DWORD RandPersonalRoomID() = 0;

	//��ȡ�������������з���
	virtual int GetHostCreatePersonalRoom(tagHostCreatRoomInfo & HostCreatRoomInfo) = 0;

	//��ȡ���������ķ��������
	virtual INT GetHostCreatePersonalRoomCount(DWORD dwUserID, DWORD dwKindID, BYTE cbPlayMode, DWORD dwGroupID) = 0;
	//�������
	virtual bool AddPersonalMaxCreate(CMD_CS_S_RegisterPersonal RegisterPersonal) = 0;
	//�Ƿ�����ٴ�������
	virtual bool CanCreatePersonalRoom(DWORD dwKindID, DWORD dwUserID, BYTE cbPlayMode, DWORD dwGroupID) = 0;

	//�û�����
public:
	//ɾ���û�
	virtual bool DeleteUserItem(DWORD dwUserID, WORD wServerID) = 0;
	//�����û�
	virtual bool ActiveUserItem(tagGlobalUserInfo &GlobalUserInfo, WORD wServerID) = 0;

	//�㳡����
public:
	//ɾ���㳡
	virtual bool DeletePlazaItem(WORD wPlazaID) = 0;
	//����㳡
	virtual bool ActivePlazaItem(WORD wBindIndex, tagGamePlaza & GamePlaza) = 0;

	//�������
public:
	//ɾ������
	virtual bool DeleteServerItem(WORD wServerID) = 0;
	//�����
	virtual bool ActiveServerItem(WORD wBindIndex, BYTE	cbPlayMode, tagGameServer &GameServer) = 0;

	//������ֲ�����
	virtual bool ActiveGroupRoom(DWORD dwGroupID, DWORD dwRoomID) = 0;

	//��ȡ���ֲ�����
	virtual CRoomIDArray * GetGroupRoom(DWORD dwGroupID) = 0;

	//ɾ�����ֲ�����
	virtual BOOL DeleteGroupRoom(DWORD dwGroupID, DWORD dwRoomID) = 0;

	//�������
public:
	//ɾ������
	virtual bool DeleteChatItem(WORD wChatID) = 0;
	//��������
	virtual bool ActiveChatItem(WORD wBindIndex, tagChatServer & ChatServer) = 0;

	//�������
public:
	//Ѱ�ҹ㳡
	virtual CGlobalPlazaItem * SearchPlazaItem(WORD wPlazaID) = 0;
	//Ѱ�ҷ���
	virtual CGlobalServerItem * SearchServerItem(WORD wServerID) = 0;
	//Ѱ������
	virtual CGlobalChatItem * SearchChatItem(WORD wChatID) = 0;

	//�û�����
public:
	//Ѱ���û�
	virtual CGlobalUserItem * SearchUserItemByUserID(DWORD dwUserID) = 0;
	//Ѱ���û�
	virtual CGlobalUserItem * SearchUserItemByGameID(DWORD dwGameID) = 0;
	//Ѱ���û�
	virtual CGlobalUserItem * SearchUserItemByNickName(LPCTSTR pszNickName) = 0;
	//����һ��������Ϸ������
	virtual tagPersonalTableInfo * SearchTableByPosition(WORD wKindID, BYTE cbPlayMode, DWORD dwGroupID, POSITION & Position, DWORD & dwRoomID) = 0;

	//ö�ٺ���
public:
	//ö���û�
	virtual CGlobalUserItem * EnumUserItem(POSITION & Position )= 0;
	//ö�ٹ㳡
	virtual CGlobalPlazaItem * EnumPlazaItem(POSITION & Position) = 0;
	//ö�ٷ���
	virtual CGlobalServerItem * EnumServerItem(POSITION & Position) = 0;
	//ö������
	virtual CGlobalChatItem * EnumChatItem(POSITION & Position) = 0;

	//��������
private:
	//�����û�
	virtual CGlobalUserItem * CreateGlobalUserItem() = 0;
	//�����㳡
	virtual CGlobalPlazaItem * CreateGlobalPlazaItem() = 0;
	//��������
	virtual CGlobalServerItem * CreateGlobalServerItem() = 0;
	//��������
	virtual CGlobalChatItem * CreateGlobalChatItem() = 0;

	//�ͷź���
private:
	//�ͷ��û�
	virtual bool FreeGlobalUserItem(CGlobalUserItem * pGlobalUserItem) = 0;
	//�ͷŹ㳡
	virtual bool FreeGlobalPlazaItem(CGlobalPlazaItem * pGlobalPlazaItem) = 0;
	//�ͷŷ���
	virtual bool FreeGlobalServerItem(CGlobalServerItem * pGlobalServerItem) = 0;
	//�ͷ�����
	virtual bool FreeGlobalChatItem(CGlobalChatItem * pGlobalChatItem) = 0;

public:
	//У��
	virtual int CheckValid() = 0;
};
//////////////////////////////////////////////////////////////////////////////////
//�����ļ�
//#ifndef PERSONAL_DATA_DLL
class /*PERSONAL_DATA_CLASS*/ CGlobalInfoManager : public IPersonalDataManager
{
	//��������
protected:
	CMapUserID						m_MapUserID;						//�û���ʶ
	CMapPlazaID						m_MapPlazaID;						//�㳡��ʶ
	CMapServerID					m_MapServerID;						//�����ʶ
	CMapChatID						m_MapChatID;						//�����ʶ
	CMapGroupRoom							m_MapGroupRoom;							//���ֲ�����ӳ��

	//++++++++++++++++++++++++++++++Լս����
public:
	CServerTableCountArray					m_ServerTableCountArray;				//���������������
	CMapPersonalTableInfo					m_MapPersonalTableInfo;					//˽�˷�����Ϣ
	std::vector<tagPersonalTableInfo>		m_VecDissumePersonalTableInfo;			//����ɢ��˽�˷�����Ϣ
	std::vector<CMD_CS_S_RegisterPersonal>	m_vecPersonalRoomMaxCreate;				//˽�˷����Դ�������󷿼���
	//��������
protected:
	CMapGameID						m_MapGameID;						//��ʶ����
	CMapStringToPtr					m_MapNickName;						//�ǳ�����

	//�洢����
protected:
	CGlobalUserItem *				m_pGlobalUserItem;					//�û��洢
	CGlobalPlazaItem *				m_pGlobalPlazaItem;					//�㳡�洢
	CGlobalServerItem *				m_pGlobalServerItem;				//����洢
	CGlobalChatItem *				m_pGlobalChatItem;					//����洢

	//��������
public:
	//���캯��
	CGlobalInfoManager();
	//��������
	virtual ~CGlobalInfoManager();
	//��������
public:
	//�ͷŶ���
	virtual VOID Release(){ delete this; };
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	//������
public:
	//��������
	virtual VOID ResetData();
	//�û���Ŀ
	virtual DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }
	//������Ŀ
	virtual DWORD GetPlazaItemCount() { return (DWORD)m_MapPlazaID.GetCount(); }
	//������Ŀ
	virtual DWORD GetServerItemCount() { return (DWORD)m_MapServerID.GetCount(); }
	//������Ŀ
	virtual DWORD GetChatItemCount() { return (DWORD)m_MapChatID.GetCount(); }

	//˽�˷���
public:
	//��������
	virtual tagPersonalTableInfo* SearchTableByID(DWORD dwRoomID);
	//��������
	virtual tagPersonalTableInfo* SearchTableByTableIDAndServerID(DWORD dwServerID, DWORD dwTableID);
	//�������
	virtual bool AddFreeServerTable(DWORD dwServerID);
	//�������
	virtual bool AddServerTable(DWORD dwRoomID, tagPersonalTableInfo PersonalTable);
	//�Ƴ�����
	virtual bool RemoveFreeServerTable(DWORD dwServerID);
	//�Ƴ�����
	virtual bool RemoveServerTable(DWORD dwServerID, DWORD dwTableID);
	//�Ƴ�����
	virtual bool RemoveServerTable(DWORD dwServerID);
	//��ȡ����
	virtual DWORD GetFreeServer(DWORD dwUserID, DWORD dwKindID, BYTE cbIsJoinGame, BYTE cbPlayMode, bool bSupportGroup);
	//��ȡ������Ŀ
	virtual DWORD GetFreeServerCount(DWORD dwKindID, BYTE cbPlayMode, bool bSupportGroup);
	//���ɷ���ID
	virtual DWORD RandPersonalRoomID();

	//��ȡ�������������з���
	virtual int GetHostCreatePersonalRoom(tagHostCreatRoomInfo & HostCreatRoomInfo);

	//��ȡ���������ķ��������
	virtual INT GetHostCreatePersonalRoomCount(DWORD dwUserID, DWORD dwKindID, BYTE cbPlayMode, DWORD dwGroupID);
	//�������
	virtual bool AddPersonalMaxCreate(CMD_CS_S_RegisterPersonal RegisterPersonal);
	//�Ƿ�����ٴ�������
	virtual bool CanCreatePersonalRoom(DWORD dwKindID, DWORD dwUserID, BYTE cbPlayMode, DWORD dwGroupID);

	//�û�����
public:
	//ɾ���û�
	virtual bool DeleteUserItem(DWORD dwUserID, WORD wServerID);
	//�����û�
	virtual bool ActiveUserItem(tagGlobalUserInfo &GlobalUserInfo, WORD wServerID);

	//�㳡����
public:
	//ɾ���㳡
	virtual bool DeletePlazaItem(WORD wPlazaID);
	//����㳡
	virtual bool ActivePlazaItem(WORD wBindIndex, tagGamePlaza & GamePlaza);

	//�������
public:
	//ɾ������
	virtual bool DeleteServerItem(WORD wServerID);
	//�����
	virtual bool ActiveServerItem(WORD wBindIndex, BYTE	cbPlayMode, tagGameServer &GameServer);

	//������ֲ�����
	virtual bool ActiveGroupRoom(DWORD dwGroupID, DWORD dwRoomID);

	//��ȡ���ֲ�����
	virtual CRoomIDArray * GetGroupRoom(DWORD dwGroupID);

	//ɾ�����ֲ�����
	virtual BOOL DeleteGroupRoom(DWORD dwGroupID, DWORD dwRoomID);

	//�������
public:
	//ɾ������
	virtual bool DeleteChatItem(WORD wChatID);
	//��������
	virtual bool ActiveChatItem(WORD wBindIndex, tagChatServer & ChatServer);

	//�������
public:
	//Ѱ�ҹ㳡
	virtual CGlobalPlazaItem * SearchPlazaItem(WORD wPlazaID);
	//Ѱ�ҷ���
	virtual CGlobalServerItem * SearchServerItem(WORD wServerID);
	//Ѱ������
	virtual CGlobalChatItem * SearchChatItem(WORD wChatID);

	//�û�����
public:
	//Ѱ���û�
	virtual CGlobalUserItem * SearchUserItemByUserID(DWORD dwUserID);
	//Ѱ���û�
	virtual CGlobalUserItem * SearchUserItemByGameID(DWORD dwGameID);
	//Ѱ���û�
	virtual CGlobalUserItem * SearchUserItemByNickName(LPCTSTR pszNickName);
	//����һ��������Ϸ������
	virtual tagPersonalTableInfo * SearchTableByPosition(WORD wKindID, BYTE cbPlayMode, DWORD dwGroupID, POSITION & Position, DWORD & dwRoomID);

	//ö�ٺ���
public:
	//ö���û�
	virtual CGlobalUserItem * EnumUserItem(POSITION & Position);
	//ö�ٹ㳡
	virtual CGlobalPlazaItem * EnumPlazaItem(POSITION & Position);
	//ö�ٷ���
	virtual CGlobalServerItem * EnumServerItem(POSITION & Position);
	//ö������
	virtual CGlobalChatItem * EnumChatItem(POSITION & Position);

	//��������
private:
	//�����û�
	virtual CGlobalUserItem * CreateGlobalUserItem();
	//�����㳡
	virtual CGlobalPlazaItem * CreateGlobalPlazaItem();
	//��������
	virtual CGlobalServerItem * CreateGlobalServerItem();
	//��������
	virtual CGlobalChatItem * CreateGlobalChatItem();

	//�ͷź���
private:
	//�ͷ��û�
	virtual bool FreeGlobalUserItem(CGlobalUserItem * pGlobalUserItem);
	//�ͷŹ㳡
	virtual bool FreeGlobalPlazaItem(CGlobalPlazaItem * pGlobalPlazaItem);
	//�ͷŷ���
	virtual bool FreeGlobalServerItem(CGlobalServerItem * pGlobalServerItem);
	//�ͷ�����
	virtual bool FreeGlobalChatItem(CGlobalChatItem * pGlobalChatItem);

public:
	//У��
	virtual int CheckValid();
};

//////////////////////////////////////////////////////////////////////////////////

//#endif
//////////////////////////////////////////////////////////////////////////////////

#endif