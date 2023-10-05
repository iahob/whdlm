#include "StdAfx.h"
#include "AfxTempl.h"
#include "PersonalRoomGame.h"
#include "PersonalRoomServiceManager.h"
	
////////////////////////////////////////////////////////////////////////

//���캯��
CPersonalRoomServiceManager::CPersonalRoomServiceManager(void)
{
	//״̬����
	m_bIsService=false;

	//���ñ���
	m_pIPersonalRoomItem=NULL;

	return;
}

//��������
CPersonalRoomServiceManager::~CPersonalRoomServiceManager(void)
{	
	//�ͷ�ָ��
	if(m_pIPersonalRoomItem!=NULL) SafeDelete(m_pIPersonalRoomItem);
}

//ֹͣ����
bool CPersonalRoomServiceManager::StopService()
{
	////״̬�ж�
	////ASSERT(m_bIsService==true);

	////����״̬
	//m_bIsService=false;

	////�ͷ�ָ��
	//if(m_pIPersonalRoomItem!=NULL) SafeRelease(m_pIPersonalRoomItem);

	return true;
}

//��������
bool CPersonalRoomServiceManager::StartService()
{
	//״̬�ж�
	//ASSERT(m_bIsService==false);
	if(m_bIsService==true) return false;

	//����״̬
	m_bIsService=true;

	//����֪ͨ
	if(m_pIPersonalRoomItem!=NULL) m_pIPersonalRoomItem->OnStartService();

	return true;
}
//�ӿڲ�ѯ
void *  CPersonalRoomServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
								 
	QUERYINTERFACE(IPersonalRoomServiceManager,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IPersonalRoomServiceManager,Guid,dwQueryVer);
	return NULL;
}

//����Լս��
bool CPersonalRoomServiceManager::CreatePersonalRoom(BYTE cbPersonalRoomType)
{
	//�ӿ��ж�
	//ASSERT(m_pIPersonalRoomItem==NULL);
	if(m_pIPersonalRoomItem!=NULL) return false;

	try
	{
		m_pIPersonalRoomItem = new CPersonalRoomGame();
		if(m_pIPersonalRoomItem==NULL) throw TEXT("��������ʧ�ܣ�");
	}
	catch(...)
	{
		//ASSERT(FALSE);
		return false;
	}

	return m_pIPersonalRoomItem!=NULL;
}

//��ʼ�����ӿ��
bool CPersonalRoomServiceManager::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	//ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return m_pIPersonalRoomItem->BindTableFrame(pTableFrame,wTableID);	
	}

	return true;
}


//��ʼ���ӿ�
bool CPersonalRoomServiceManager::InitPersonalRooomInterface(tagPersonalRoomManagerParameter & PersonalManagerParameter)
{
	//ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return m_pIPersonalRoomItem->InitPersonalRooomInterface(PersonalManagerParameter);
	}

	return true;
}

//ʱ���¼�
bool CPersonalRoomServiceManager::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return m_pIPersonalRoomItem->OnEventTimer(dwTimerID,dwBindParameter);	
	}

	return true;
}

//���ݿ��¼�
bool CPersonalRoomServiceManager::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)
{
	//ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return m_pIPersonalRoomItem->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize,dwContextID);	
	}

	return true;
}


//������Ϣ
bool CPersonalRoomServiceManager::OnEventSocketPersonalRoom(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return m_pIPersonalRoomItem->OnEventSocketPersonalRoom(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);	
	}

	return true;
}

//Լս�������¼�
bool CPersonalRoomServiceManager::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return m_pIPersonalRoomItem->OnTCPSocketMainServiceInfo(wSubCmdID,pData,wDataSize);	
	}
	return true;
}

//�û���¼
bool CPersonalRoomServiceManager::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	//ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return m_pIPersonalRoomItem->OnEventUserLogon(pIServerUserItem);	
	}

	return true;
}

//�û��ǳ�
bool CPersonalRoomServiceManager::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	//ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return m_pIPersonalRoomItem->OnEventUserLogout(pIServerUserItem);	
	}

	return true;
}

//��¼���
bool CPersonalRoomServiceManager::OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)
{
	//ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return m_pIPersonalRoomItem->OnEventUserLogonFinish(pIServerUserItem);	
	}

	return true;
}


//�û��ӿ�
IUnknownEx * CPersonalRoomServiceManager::GetServerUserItemSink()
{
	//ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return QUERY_OBJECT_PTR_INTERFACE(m_pIPersonalRoomItem,IUnknownEx);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(PersonalRoomServiceManager);
											 

////////////////////////////////////////////////////////////////////////
