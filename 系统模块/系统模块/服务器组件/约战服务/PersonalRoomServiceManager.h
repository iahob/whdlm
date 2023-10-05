#ifndef GAME_MATCH_SERVICE_MANAGER_HEAD_FILE
#define GAME_MATCH_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\..\���������\��Ϸ������\PersonalRoomServiceHead.h"

////////////////////////////////////////////////////////////////////////

//Լս���������
class PERSONAL_ROOM_SERVICE_CLASS CPersonalRoomServiceManager : public IPersonalRoomServiceManager
{
	//״̬����
protected:
	bool								m_bIsService;					//�����ʶ	
	
	//�ӿڱ���
protected:	
	IPersonalRoomItem *					m_pIPersonalRoomItem;				//Լս����

	//����ӿ�
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//���ܽӿ�

	//��������
public:
	//���캯��
	CPersonalRoomServiceManager(void);
	//��������
	virtual ~CPersonalRoomServiceManager(void);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);
	
	//���ƽӿ�
public:
	//ֹͣ����
	virtual bool StopService();
	//��������
	virtual bool StartService();
	
	//����ӿ�
public:
	//����Լս����
	virtual bool CreatePersonalRoom(BYTE cbPersonalRoomType);
	//������
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID);
	//��ʼ���ӿ�
	virtual bool InitPersonalRooomInterface(tagPersonalRoomManagerParameter & PersonalManagerParameter);	

	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID);

	//�����¼�
public:
	//Լս�¼�
	virtual bool OnEventSocketPersonalRoom(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	
	//Լս�������¼�
	virtual bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//��¼���
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem);

	//�ӿ���Ϣ
public:
	//�û��ӿ�
	virtual IUnknownEx * GetServerUserItemSink();
};

//////////////////////////////////////////////////////////////////////////
#endif