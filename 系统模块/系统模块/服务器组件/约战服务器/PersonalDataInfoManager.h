
#ifndef PERSONAL_DATA_MANAGER_HEAD_FILE
#define PERSONAL_DATA_MANAGER_HEADFILE


#include "PersonalDataInfo.h"
//#include "../Լս������/GlobalInfoManager.h"
//AI��Ϣ
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
	virtual VOID Release(){ delete this;};
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer) ;
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

#endif