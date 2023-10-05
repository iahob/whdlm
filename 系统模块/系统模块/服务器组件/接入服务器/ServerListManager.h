//�ر���IT������28xin.com��
#ifndef SERVER_LIST_MANAGER_HEAD_FILE
#define SERVER_LIST_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////////////

//�б����
namespace NServerListManager
{

	//////////////////////////////////////////////////////////////////////////////////

	//��Ϸ����
	class CGameListItem
	{
		//��������
	public:
		bool							m_bDisuse;							//������־

		//��������
	protected:
		//���캯��
		CGameListItem() { m_bDisuse=false; }
	};

	//////////////////////////////////////////////////////////////////////////////////

	//����ṹ
	class CGameServerItem : public CGameListItem
	{
		//��������
	public:
		tagGameServer					m_GameServer;						//������Ϣ

		//��������
	public:
		//���캯��
		CGameServerItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//����˵��
	typedef CWHArray<CGameServerItem *>	CGameServerItemArray;
	typedef CMap<WORD,WORD,CGameServerItem *,CGameServerItem * &> CServerItemMap;

	//////////////////////////////////////////////////////////////////////////////////

	//�б����
	class CServerListManager
	{
		//��������
	protected:
		CServerItemMap					m_ServerItemMap;					//��������

		//�洢����
	protected:
		CGameServerItemArray			m_ServerItemBuffer;					//��������

		//��������
	public:
		//���캯��
		CServerListManager();
		//��������
		virtual ~CServerListManager();

		//����ӿ�
	public:
		//�����б�
		VOID ResetServerList();
		//������
		VOID CleanServerItem();
		//��������
		VOID DisuseServerItem();

		//����ӿ�
	public:
		//���뷿��
		bool InsertGameServer(tagGameServer * pGameServer);

		//ɾ���ӿ�
	public:
		//ɾ������
		bool DeleteGameServer(WORD wServerID);

		//ö�ٽӿ�
	public:
		//ö�ٷ���
		CGameServerItem * EmunGameServerItem(POSITION & Position);

		//���ҽӿ�
	public:
		//���ҷ���
		CGameServerItem * SearchGameServer(WORD wServerID);

		//��Ŀ�ӿ�
	public:
		//������Ŀ
		DWORD GetGameServerCount() { return (DWORD)m_ServerItemMap.GetCount(); }

		//У��ӿ�
	public:
		//У������˰�����Ϸ
		bool IsGroupPersonalMaxChairGame(WORD wKindID);				
	};

	//////////////////////////////////////////////////////////////////////////////////

};

using namespace NServerListManager;

#endif