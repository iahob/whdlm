//�ر���IT������28xin.com��
//����ӣ���ݡ��Ŀ�,Ⱥ:16886186����ϵQQ:412069412
#include "Stdafx.h"
#include "UDPSocket.h"
#include "WHNetwork.h"

//////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define	IDI_DETEST_SOCKET			100									//������

//��Ϣ����
#define WM_SOCKET_NOTIFY			(WM_USER+100)						//������Ϣ

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CUDPSocket, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(WM_SOCKET_NOTIFY,OnSocketNotify)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CUDPSocket::CUDPSocket()
{
	//��������
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;

	//��������
	m_wSocketID=0;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//�ں˱���
	m_pIUDPSocketSink=NULL;
	m_hSocket=INVALID_SOCKET;

	return;
}

//��������
CUDPSocket::~CUDPSocket()
{
}

//�ӿڲ�ѯ
VOID * CUDPSocket::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IUDPSocket,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IUDPSocket,Guid,dwQueryVer);
	return NULL;
}

//���ñ�ʶ
VOID CUDPSocket::SetSocketID(WORD wSocketID)
{
	//���ñ���
	m_wSocketID=wSocketID;

	return;
}

//���ýӿ�
bool CUDPSocket::SetUDPSocketSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if(pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUDPSocketSink)!=NULL);
		m_pIUDPSocketSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUDPSocketSink);

		//�ɹ��ж�
		if(m_pIUDPSocketSink==NULL) return false;
	}
	else m_pIUDPSocketSink=NULL;

	return true;
}

//�ر�����
VOID CUDPSocket::CloseSocket()
{
	//�ر�����
	CloseSocket(SHUT_REASON_NORMAL);

	return;
}

//���Ӳ���
BYTE CUDPSocket::Connect(DWORD dwServerIP, WORD wPort)
{
	return 0;
}

//���Ӳ���
BYTE CUDPSocket::Connect(LPCTSTR pszServerIP, WORD wPort)
{
	//У������
	ASSERT((wPort!=0)&&(pszServerIP!=NULL));
	if((pszServerIP==NULL)||(wPort==0)) return false;

	//���Ӳ���
	return Connect(g_WHNetwork.ConversionAddr(pszServerIP),wPort);
}

//���ͺ���
WORD CUDPSocket::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	return 0;
}

//���ͺ���
WORD CUDPSocket::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return 0;
}

//�����ȡ
LRESULT CUDPSocket::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	return 0L;
}

//���緢��
LRESULT CUDPSocket::OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//����ر�
LRESULT CUDPSocket::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//��������
LRESULT CUDPSocket::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//�ر�����
VOID CUDPSocket::CloseSocket(BYTE cbShutReason)
{
	//���ܱ���
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//�ر��ж�
	if(m_hSocket!=INVALID_SOCKET)
	{
		//ɾ��ʱ��
		KillTimer(IDI_DETEST_SOCKET);

		//ע���¼�
		WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY,0);

		//�ر�����
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;

		//�¼�֪ͨ
		if(cbShutReason!=SHUT_REASON_INSIDE)
		{
			ASSERT(m_pIUDPSocketSink!=NULL);
			if(m_pIUDPSocketSink!=NULL) m_pIUDPSocketSink->OnEventUDPSocketShut(m_wSocketID,cbShutReason);
		}
	}

	return;
}

//ʱ����Ϣ
VOID CUDPSocket::OnTimer(UINT_PTR nIDEvent)
{
	//������
	if(nIDEvent==IDI_DETEST_SOCKET)
	{
		//��ֹ�ж�
		if(m_cbSocketStatus!=SOCKET_STATUS_CONNECT)
		{
			ASSERT(FALSE);
			KillTimer(IDI_DETEST_SOCKET);

			return;
		}

		//����ж�
		DWORD dwCompareTickCount=GetTickCount()/1000L-30L;
		bool bIntermit=((m_dwSendTickCount<dwCompareTickCount)&&(m_dwRecvTickCount<dwCompareTickCount));

		//��������
		if(bIntermit==true) CloseSocket(SHUT_REASON_TIME_OUT);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//������Ϣ
LRESULT CUDPSocket::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:		//���ݶ�ȡ
		{
			return OnSocketNotifyRead(wParam,lParam);
		}
	case FD_WRITE:		//���緢��
		{
			return OnSocketNotifyWrite(wParam,lParam);
		}
	case FD_CLOSE:		//����ر�
		{
			return OnSocketNotifyClose(wParam,lParam);
		}
	case FD_CONNECT:	//��������
		{
			return OnSocketNotifyConnect(wParam,lParam);
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(UDPSocket);

//////////////////////////////////////////////////////////////////////////