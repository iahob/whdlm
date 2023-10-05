//藏宝库IT社区（28xin.com）
//分享樱花草、的酷,群:16886186，联系QQ:412069412
#include "Stdafx.h"
#include "UDPSocket.h"
#include "WHNetwork.h"

//////////////////////////////////////////////////////////////////////////

//时间定义
#define	IDI_DETEST_SOCKET			100									//网络检测

//消息定义
#define WM_SOCKET_NOTIFY			(WM_USER+100)						//网络消息

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CUDPSocket, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(WM_SOCKET_NOTIFY,OnSocketNotify)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CUDPSocket::CUDPSocket()
{
	//计数变量
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;

	//辅助变量
	m_wSocketID=0;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//内核变量
	m_pIUDPSocketSink=NULL;
	m_hSocket=INVALID_SOCKET;

	return;
}

//析构函数
CUDPSocket::~CUDPSocket()
{
}

//接口查询
VOID * CUDPSocket::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IUDPSocket,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IUDPSocket,Guid,dwQueryVer);
	return NULL;
}

//设置标识
VOID CUDPSocket::SetSocketID(WORD wSocketID)
{
	//设置变量
	m_wSocketID=wSocketID;

	return;
}

//设置接口
bool CUDPSocket::SetUDPSocketSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if(pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUDPSocketSink)!=NULL);
		m_pIUDPSocketSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUDPSocketSink);

		//成功判断
		if(m_pIUDPSocketSink==NULL) return false;
	}
	else m_pIUDPSocketSink=NULL;

	return true;
}

//关闭连接
VOID CUDPSocket::CloseSocket()
{
	//关闭连接
	CloseSocket(SHUT_REASON_NORMAL);

	return;
}

//连接操作
BYTE CUDPSocket::Connect(DWORD dwServerIP, WORD wPort)
{
	return 0;
}

//连接操作
BYTE CUDPSocket::Connect(LPCTSTR pszServerIP, WORD wPort)
{
	//校验数据
	ASSERT((wPort!=0)&&(pszServerIP!=NULL));
	if((pszServerIP==NULL)||(wPort==0)) return false;

	//连接操作
	return Connect(g_WHNetwork.ConversionAddr(pszServerIP),wPort);
}

//发送函数
WORD CUDPSocket::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	return 0;
}

//发送函数
WORD CUDPSocket::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return 0;
}

//网络读取
LRESULT CUDPSocket::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	return 0L;
}

//网络发送
LRESULT CUDPSocket::OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//网络关闭
LRESULT CUDPSocket::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//网络连接
LRESULT CUDPSocket::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//关闭连接
VOID CUDPSocket::CloseSocket(BYTE cbShutReason)
{
	//接受变量
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//关闭判断
	if(m_hSocket!=INVALID_SOCKET)
	{
		//删除时间
		KillTimer(IDI_DETEST_SOCKET);

		//注销事件
		WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY,0);

		//关闭连接
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;

		//事件通知
		if(cbShutReason!=SHUT_REASON_INSIDE)
		{
			ASSERT(m_pIUDPSocketSink!=NULL);
			if(m_pIUDPSocketSink!=NULL) m_pIUDPSocketSink->OnEventUDPSocketShut(m_wSocketID,cbShutReason);
		}
	}

	return;
}

//时间消息
VOID CUDPSocket::OnTimer(UINT_PTR nIDEvent)
{
	//网络检测
	if(nIDEvent==IDI_DETEST_SOCKET)
	{
		//终止判断
		if(m_cbSocketStatus!=SOCKET_STATUS_CONNECT)
		{
			ASSERT(FALSE);
			KillTimer(IDI_DETEST_SOCKET);

			return;
		}

		//检测判断
		DWORD dwCompareTickCount=GetTickCount()/1000L-30L;
		bool bIntermit=((m_dwSendTickCount<dwCompareTickCount)&&(m_dwRecvTickCount<dwCompareTickCount));

		//发送命令
		if(bIntermit==true) CloseSocket(SHUT_REASON_TIME_OUT);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//网络消息
LRESULT CUDPSocket::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:		//数据读取
		{
			return OnSocketNotifyRead(wParam,lParam);
		}
	case FD_WRITE:		//网络发送
		{
			return OnSocketNotifyWrite(wParam,lParam);
		}
	case FD_CLOSE:		//网络关闭
		{
			return OnSocketNotifyClose(wParam,lParam);
		}
	case FD_CONNECT:	//网络连接
		{
			return OnSocketNotifyConnect(wParam,lParam);
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(UDPSocket);

//////////////////////////////////////////////////////////////////////////