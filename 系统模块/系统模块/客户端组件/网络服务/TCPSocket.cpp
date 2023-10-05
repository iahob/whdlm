//分享樱花草、的酷,群:16886186，联系QQ:412069412
#include "Stdafx.h"
#include "TCPSocket.h"
#include "WHNetwork.h"

//////////////////////////////////////////////////////////////////////////

//时间定义
#define	IDI_DETEST_SOCKET			100									//网络检测

//消息定义
#define WM_SOCKET_NOTIFY			(WM_USER+100)						//网络消息

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTCPSocket, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(WM_SOCKET_NOTIFY,OnSocketNotify)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPSocket::CTCPSocket()
{
	//辅助变量
	m_wSocketID=0;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//内核变量
	m_pITCPSocketSink=NULL;
	m_hSocket=INVALID_SOCKET;

	//代理信息
	m_cbProxyType=PROXY_NONE;
	ZeroMemory(&m_ProxyServer,sizeof(m_ProxyServer));

	//接受变量
	m_wBufferSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//缓冲变量
	m_dwBufferData=0L;
	m_dwBufferSize=0L;
	m_bNeedBuffer=false;
	m_pcbDataBuffer=NULL;

	//计数变量
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;

	return;
}

//析构函数
CTCPSocket::~CTCPSocket()
{
	//关闭连接
	if (m_hSocket!=INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;
	}

	//删除缓冲
	SafeDeleteArray(m_pcbDataBuffer);
	
	return;
}

//接口查询
VOID * CTCPSocket::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocket,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocket,Guid,dwQueryVer);
	return NULL;
}

//设置标识
VOID CTCPSocket::SetSocketID(WORD wSocketID)
{
	//设置变量
	m_wSocketID=wSocketID;

	return;
}

//设置接口
bool CTCPSocket::SetTCPSocketSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if(pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPSocketSink)!=NULL);
		m_pITCPSocketSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPSocketSink);

		//成功判断
		if(m_pITCPSocketSink==NULL) return false;
	}
	else m_pITCPSocketSink=NULL;

	return true;
}

//代理测试
BYTE CTCPSocket::ProxyServerTesting()
{
	//校验参数
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_cbProxyType!=PROXY_NONE);
	ASSERT(m_cbSocketStatus==SOCKET_STATUS_IDLE);

	//校验状态
	if(m_hSocket!=INVALID_SOCKET) return CONNECT_EXCEPTION;
	if(m_cbProxyType==PROXY_NONE) return CONNECT_EXCEPTION;
	if(m_cbSocketStatus!=SOCKET_STATUS_IDLE) return CONNECT_EXCEPTION;

	//建立 SOCKET
	m_hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(m_hSocket==INVALID_SOCKET) return CONNECT_EXCEPTION;

	//连接代理
	BYTE cbResult=ConnectProxyServer();

	//关闭连接
	closesocket(m_hSocket);
	m_hSocket=INVALID_SOCKET;

	return cbResult;
}

//设置代理
bool CTCPSocket::SetProxyServerInfo(BYTE cbProxyType, const tagProxyServer & ProxyServer)
{
	switch(cbProxyType)
	{
	case PROXY_NONE:		//没有代理
		{
			//设置变量
			m_cbProxyType=cbProxyType;
			ZeroMemory(&m_ProxyServer,sizeof(m_ProxyServer));

			return true;
		}
	case PROXY_HTTP:		//HTTP 代理
	case PROXY_SOCKS4:		//SOCKET4 代理
	case PROXY_SOCKS5:		//SOCKET5 代理
		{
			//设置变量
			m_cbProxyType=cbProxyType;
			m_ProxyServer=ProxyServer;

			return true;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return false;
}

//连接操作
BYTE CTCPSocket::Connect(DWORD dwServerIP, WORD wPort)
{
	//AfxMessageBox(L"正在连接...");
	//校验参数
	ASSERT(dwServerIP!=INADDR_NONE);
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_cbSocketStatus==SOCKET_STATUS_IDLE);

	//设置参数
	m_wBufferSize=0;
	m_dwSendTickCount=GetTickCount()/1000L;
	m_dwRecvTickCount=GetTickCount()/1000L;

	try
	{
		//建立 SOCKET
		m_hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(m_hSocket==INVALID_SOCKET) throw CONNECT_EXCEPTION;

		//创建窗口
		if(m_hWnd==NULL)
		{
			CRect rcSocketWnd(0,0,0,0);
			Create(NULL,NULL,WS_CHILD,rcSocketWnd,GetDesktopWindow(),100);
		}

		//变量定义
		SOCKADDR_IN SocketAddr;
		ZeroMemory(&SocketAddr,sizeof(SocketAddr));

		//设置变量
		SocketAddr.sin_family=AF_INET;
		SocketAddr.sin_port=htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr=dwServerIP;
		
		//代理连接
		if(m_cbProxyType!=PROXY_NONE)
		{
			//AfxMessageBox(L"代理");
			//连接代理
			DWORD dwResult=ConnectProxyServer();
			if(dwResult!=CONNECT_SUCCESS) throw dwResult;

			//请求连接
			switch(m_cbProxyType)
			{
			case PROXY_HTTP:		//HTTP 代理
				{
					//构造请求
					char cbDataBuffer[512];
					BYTE * pcbServerIP=(BYTE *)&dwServerIP;
					_snprintf(cbDataBuffer,CountArray(cbDataBuffer),"CONNECT %d.%d.%d.%d:%d HTTP/1.0\r\nUser-Agent: WHPlatform/0.1\r\n\r\n",
						pcbServerIP[0],pcbServerIP[1],pcbServerIP[2],pcbServerIP[3],wPort);

					//连接请求
					INT nRequestLen=(INT)strlen(cbDataBuffer);
					if(send(m_hSocket,cbDataBuffer,nRequestLen,0)!=nRequestLen) throw CONNECT_PROXY_FAILURE;

					//接受响应
					INT nRecvCount=recv(m_hSocket,cbDataBuffer,sizeof(cbDataBuffer),0);
					if(nRecvCount>0) cbDataBuffer[nRecvCount]=0;

					//结果判断
					bool bSuccess=false;
					for (INT i=0; i<nRecvCount; i++)
					{
						if((cbDataBuffer[i]==' ')&&(cbDataBuffer[i+1]=='2')&&(cbDataBuffer[i+2]=='0')&&(cbDataBuffer[i+3]=='0'))
						{
							bSuccess=true;
							break;
						}
					}

					//结果判断
					if(bSuccess==false) throw CONNECT_PROXY_FAILURE;

					break;
				}
			case PROXY_SOCKS5:		//Socket5 代理
				{
					//构造请求
					INT nOff=4;
					char cbDataBuffer[256]={5,1,0,1};

					//连接地址
					CopyMemory(cbDataBuffer+nOff,&SocketAddr.sin_addr,sizeof(SocketAddr.sin_addr));
					nOff+=sizeof(SocketAddr.sin_addr);

					//连接端口
					*(WORD*)(cbDataBuffer+nOff)=SocketAddr.sin_port;
					nOff+=sizeof(WORD);

					//连接请求
					if(send(m_hSocket,cbDataBuffer,nOff,0)!=nOff) throw CONNECT_PROXY_FAILURE;

					//接受响应
					INT nRecvCount=recv(m_hSocket,cbDataBuffer,4,0);
					if(nRecvCount!=4) throw CONNECT_PROXY_FAILURE;
					if(cbDataBuffer[0]!=5) throw CONNECT_PROXY_FAILURE;

					//失败判断
					if(cbDataBuffer[1]==0) throw CONNECT_PROXY_FAILURE;
					
					//接受数据
					INT nLeftDataLen=(cbDataBuffer[3]==1)?6:cbDataBuffer[4]+2;
					if(recv(m_hSocket,cbDataBuffer,nLeftDataLen,0)!=nLeftDataLen) throw CONNECT_PROXY_FAILURE;
                    
					break;
				}
			case PROXY_SOCKS4:		//Socket4 代理
				{
					//构造请求
					INT nOff=2;
					char cbDataBuffer[256]={4,1};

					//连接端口
					*(WORD*)(cbDataBuffer+nOff)=SocketAddr.sin_port;
					nOff+=sizeof(WORD);

					//连接地址
					CopyMemory(cbDataBuffer+nOff,&SocketAddr.sin_addr,sizeof(SocketAddr.sin_addr));
					nOff+=sizeof(SocketAddr.sin_addr);

					//保留字段
					cbDataBuffer[nOff++]=0;

					//连接请求
					INT nResult=send(m_hSocket,cbDataBuffer,nOff,0);
					if(nResult!=nOff) throw CONNECT_PROXY_FAILURE;

					//接受响应
					nResult=recv(m_hSocket,cbDataBuffer,8,0);
					if(nResult!=8) throw CONNECT_PROXY_FAILURE;
					if(cbDataBuffer[0]!=0) throw CONNECT_PROXY_FAILURE;

					//失败判断
					if(cbDataBuffer[1]!=90) throw CONNECT_PROXY_FAILURE;

					break;
				}
			default:		//默认处理
				{
					ASSERT(FALSE);
					throw CONNECT_PROXY_FAILURE;
				}
			}

			//连接成功
			WSASetLastError(0);

			//设置变量
			m_cbSocketStatus=SOCKET_STATUS_CONNECT;

			//发送消息
			PostMessage(WM_SOCKET_NOTIFY,0,MAKELONG(FD_CONNECT,0));

			//绑定窗口
			WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY,FD_READ|FD_CONNECT|FD_CLOSE|FD_WRITE);
		}
		else
		{
			//AfxMessageBox(L"正常连接");
			//绑定窗口
			WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY,FD_READ|FD_CONNECT|FD_CLOSE|FD_WRITE);
			
			//发起连接
			INT nErrorCode=connect(m_hSocket,(SOCKADDR *)&SocketAddr,sizeof(SocketAddr));
			if((nErrorCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSAEWOULDBLOCK)) throw CONNECT_EXCEPTION;

			//设置变量
			m_cbSocketStatus=SOCKET_STATUS_WAIT;
		}
	}
	catch (BYTE cbErrorCode)
	{
		//关闭连接
		CloseSocket(SHUT_REASON_INSIDE);

		return cbErrorCode;
	}

	return CONNECT_SUCCESS;
}

//连接操作
BYTE CTCPSocket::Connect(LPCTSTR pszServerIP, WORD wPort)
{
	//AfxMessageBox(pszServerIP);
	//校验数据
	ASSERT((wPort!=0)&&(pszServerIP!=NULL));
	if((pszServerIP==NULL)||(wPort==0)) return false;

	//连接操作
	return Connect(g_WHNetwork.ConversionAddr(pszServerIP),wPort);
}

//发送函数
WORD CTCPSocket::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//校验状态
	if(m_hSocket==INVALID_SOCKET) return 0L;
	if(m_cbSocketStatus!=SOCKET_STATUS_CONNECT) return 0L;

	//设置数据
	if(m_TCPSocketData.InitSocketData(wMainCmdID,wSubCmdID,NULL,0)==false)
	{
		return false;
	}

	//发送数据
	if(SendDataBuffer(m_TCPSocketData.m_cbDataBuffer,m_TCPSocketData.m_wBufferSize)==false)
	{
		return false;
	}

	return true;
}

//发送函数
WORD CTCPSocket::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//校验大小
	ASSERT(wDataSize<=SOCKET_TCP_PACKET);
	if(wDataSize>SOCKET_TCP_PACKET) return false;

	//校验状态
	if(m_hSocket==INVALID_SOCKET) return false;
	if(m_cbSocketStatus!=SOCKET_STATUS_CONNECT) return false;

	//设置数据
	if(m_TCPSocketData.InitSocketData(wMainCmdID,wSubCmdID,pData,wDataSize)==false)
	{
		return false;
	}

	//发送数据
	if(SendDataBuffer(m_TCPSocketData.m_cbDataBuffer,m_TCPSocketData.m_wBufferSize)==false)
	{
		return false;
	}

	return true;
}

//关闭连接
VOID CTCPSocket::CloseSocket()
{
	//关闭连接
	CloseSocket(SHUT_REASON_NORMAL);
	
	return;
}

//关闭连接
VOID CTCPSocket::CloseSocket(BYTE cbShutReason)
{
	//接受变量
	m_wBufferSize=0;
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//缓冲变量
	m_dwBufferData=0L;
	m_dwBufferSize=0L;
	m_bNeedBuffer=false;
	SafeDeleteArray(m_pcbDataBuffer);

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
			ASSERT(m_pITCPSocketSink!=NULL);
			if(m_pITCPSocketSink!=NULL) m_pITCPSocketSink->OnEventTCPSocketShut(m_wSocketID,cbShutReason);
		}
	}

	return;
}

//缓冲数据
VOID CTCPSocket::AmortizeBuffer(VOID * pData, WORD wDataSize)
{
	//申请缓冲
	if((m_dwBufferData+wDataSize)>m_dwBufferSize)
	{
		//变量定义
		LPBYTE pcbDataBuffer=NULL;
		LPBYTE pcbDeleteBuffer=m_pcbDataBuffer;

		//计算大小
		DWORD dwNeedSize=m_dwBufferData+wDataSize;
		DWORD dwApplySize=__max(dwNeedSize,m_dwBufferSize*2L);

		//申请缓冲
		try
		{
			pcbDataBuffer=new BYTE[dwApplySize];
		}
		catch(...) { }

		//失败判断
		if(pcbDataBuffer==NULL)
		{
			CloseSocket(SHUT_REASON_EXCEPTION);
			return;
		}

		//设置变量
		m_dwBufferSize=dwApplySize;
		m_pcbDataBuffer=pcbDataBuffer;
		CopyMemory(m_pcbDataBuffer,pcbDeleteBuffer,m_dwBufferData);

		//删除缓冲
		SafeDeleteArray(pcbDeleteBuffer);
	}

	//设置变量
	m_bNeedBuffer=true;
	m_dwBufferData+=wDataSize;
	CopyMemory(m_pcbDataBuffer+m_dwBufferData-wDataSize,pData,wDataSize);

	return;
}

//发送数据
WORD CTCPSocket::SendDataBuffer(VOID * pBuffer, WORD wSendSize)
{
	//变量定义
	WORD wTotalCount=0;

	//设置变量
	m_dwSendTickCount=GetTickCount()/1000L;

	//发送数据
	while((m_bNeedBuffer==false)&&(wTotalCount<wSendSize))
	{
		//发送数据
		INT nSendCount=send(m_hSocket,(char *)pBuffer+wTotalCount,wSendSize-wTotalCount,0);

		//错误判断
		if(nSendCount==SOCKET_ERROR)
		{
			//缓冲判断
			if(WSAGetLastError()==WSAEWOULDBLOCK)
			{
				AmortizeBuffer((LPBYTE)pBuffer+wTotalCount,wSendSize-wTotalCount);
				return wSendSize;
			}

			//关闭连接
			CloseSocket(SHUT_REASON_EXCEPTION);

			return 0L;
		}
		else
		{
			//设置变量
			wTotalCount+=nSendCount;
		}
	}

	//缓冲数据
	if(wTotalCount>wSendSize)
	{
		AmortizeBuffer((LPBYTE)pBuffer+wTotalCount,wSendSize-wTotalCount);
	}

	return wSendSize;
}

//连接代理
BYTE CTCPSocket::ConnectProxyServer()
{
	//校验状态
	ASSERT(m_cbProxyType!=PROXY_NONE);
	if(m_cbProxyType==PROXY_NONE) return CONNECT_EXCEPTION;

	//变量定义
	SOCKADDR_IN SocketAddrProxy;
	ZeroMemory(&SocketAddrProxy,sizeof(SocketAddrProxy));

	//设置变量
	SocketAddrProxy.sin_family=AF_INET;
	SocketAddrProxy.sin_port=htons(m_ProxyServer.wProxyPort);
	SocketAddrProxy.sin_addr.S_un.S_addr=g_WHNetwork.ConversionAddr(m_ProxyServer.szProxyServer);

	//连接代理
	INT nErrorCode=connect(m_hSocket,(sockaddr *)&SocketAddrProxy,sizeof(SocketAddrProxy));
	if(nErrorCode==SOCKET_ERROR) return CONNECT_PROXY_FAILURE;

	//发送数据
	if(m_cbProxyType==PROXY_SOCKS5)
	{
		//变量定义
		INT nOff=2;
		char cbDataBuffer[256]={5,1};

		//发送数据
		cbDataBuffer[nOff++]=(m_ProxyServer.szUserName[0]==0)?0:2;
		if(send(m_hSocket,cbDataBuffer,nOff,0)!=nOff) return CONNECT_PROXY_FAILURE;

		//接受数据
		if(recv(m_hSocket,cbDataBuffer,2,0)!=2) return CONNECT_PROXY_FAILURE;

		//数据分析
		if(cbDataBuffer[0]!=5) return CONNECT_PROXY_FAILURE;
		if((cbDataBuffer[1]!=0)&&(cbDataBuffer[1]!=2)) return CONNECT_PROXY_FAILURE;

		//身份验证
		if(cbDataBuffer[1]==2)
		{
			//变量定义
			INT nStringLen=0;
			CT2CA strUserName(m_ProxyServer.szUserName);
			CT2CA strPassword(m_ProxyServer.szPassword);

			//登录帐号
			nOff=0;
			cbDataBuffer[nOff++]=1;
			nStringLen=lstrlenA(strUserName);
			cbDataBuffer[nOff++]=(char)nStringLen;
			CopyMemory(cbDataBuffer+nOff,(LPCSTR)strUserName,nStringLen);
			nOff+=nStringLen;

			//登录密码
			nStringLen=lstrlenA(strPassword);
			cbDataBuffer[nOff++]=(char)nStringLen;
			CopyMemory(cbDataBuffer+nOff,(LPCSTR)strPassword,nStringLen);
			nOff+=nStringLen;

			//发送数据
			if(send(m_hSocket,cbDataBuffer,nOff,0)!=nOff) return CONNECT_PROXY_FAILURE;

			//接受数据
			if(recv(m_hSocket,cbDataBuffer,2,0)!=2) return CONNECT_PROXY_FAILURE;

			//数据分析
			if((cbDataBuffer[0]!=1)||(cbDataBuffer[1]!=0)) return CONNECT_PROXY_USER_INVALID;
		}
	}

	return CONNECT_SUCCESS;
}

//网络读取
LRESULT CTCPSocket::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	//读取数据
	INT nResultCode=recv(m_hSocket,(char *)m_cbDataBuffer+m_wBufferSize,sizeof(m_cbDataBuffer)-m_wBufferSize,0);
	
	//关闭判断
	if(nResultCode==SOCKET_ERROR)
	{
		CloseSocket(SHUT_REASON_EXCEPTION);
		return 1L;
	}

	//设置变量
	m_wBufferSize+=nResultCode;
	m_dwRecvTickCount=GetTickCount()/1000L;

	//数据处理
	while((m_wBufferSize>=sizeof(TCP_Head))&&(m_hSocket!=INVALID_SOCKET))
	{
		//变量定义
		TCP_Info * pInfo=(TCP_Info *)m_cbDataBuffer;

		//长度校验
		if((pInfo->wPacketSize<sizeof(TCP_Info))||(pInfo->wPacketSize>SOCKET_TCP_BUFFER))
		{
			CloseSocket(SHUT_REASON_EXCEPTION);
			return 1L;
		}

		//完整判断
		if(m_wBufferSize<pInfo->wPacketSize) return 1L;

		//变量定义
		tagTCPData TCPData;
		WORD wBufferDataSize=pInfo->wPacketSize;

		//设置数据
		if(m_TCPSocketData.SetBufferData(m_cbDataBuffer,wBufferDataSize,TCPData)==false)
		{
			CloseSocket(SHUT_REASON_EXCEPTION);
			return 1L;
		}
		
		//移动数据
		m_wBufferSize-=wBufferDataSize;
		MoveMemory(m_cbDataBuffer,m_cbDataBuffer+wBufferDataSize,m_wBufferSize);

		//网络检测
		if((TCPData.Command.wMainCmdID==MDM_KN_COMMAND)&&(TCPData.Command.wSubCmdID==SUB_KN_DETECT_SOCKET))
		{
			SendData(MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET);
			continue;
		}

		//常规判断
		if((TCPData.Command.wMainCmdID!=MDM_KN_COMMAND)&&(m_pITCPSocketSink!=NULL))
		{
			try
			{
				//处理数据
				bool bSuccess=m_pITCPSocketSink->OnEventTCPSocketRead(m_wSocketID,TCPData.Command,TCPData.pDataBuffer,TCPData.wDataSize);

				//关闭判断
				if(bSuccess==false)
				{
					CloseSocket(SHUT_REASON_EXCEPTION);
					return 1L;
				}
			}
			catch (...)
			{
				//关闭连接
				CloseSocket(SHUT_REASON_EXCEPTION);
				return 1L;				
			}
		}
	};

	return 1L;
}

//网络发送
LRESULT CTCPSocket::OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam)
{
	//缓冲判断
	if((m_bNeedBuffer==true)&&(m_dwBufferData>0L))
	{
		//变量定义
		DWORD dwTotalCount=0;
		DWORD dwPacketSize=4096;

		//设置变量
		m_dwSendTickCount=GetTickCount()/1000L;

		//发送数据
		while(dwTotalCount<m_dwBufferData)
		{
			//发送数据
			WORD wSendSize=(WORD)__min(dwPacketSize,m_dwBufferData-dwTotalCount);
			INT nSendCount=send(m_hSocket,(char*)m_pcbDataBuffer+dwTotalCount,wSendSize,0);

			//错误判断
			if(nSendCount==SOCKET_ERROR)
			{
				//缓冲判断
				if(WSAGetLastError()==WSAEWOULDBLOCK)
				{
					//设置变量
					m_bNeedBuffer=false;
					m_dwBufferData-=dwTotalCount;

					//移动内存
					if(m_dwBufferData>0L)
					{
						m_bNeedBuffer=true;
						MoveMemory(m_pcbDataBuffer,m_pcbDataBuffer+dwTotalCount,m_dwBufferData);
					}

					return 1L;
				}

				//关闭连接
				CloseSocket(SHUT_REASON_EXCEPTION);

				return 1L;
			}

			//设置变量
			dwTotalCount+=nSendCount;
		}

		//设置变量
		m_dwBufferData=0L;
		m_bNeedBuffer=false;
	}

	return 1L;
}

//网络关闭
LRESULT CTCPSocket::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	//关闭连接
	CloseSocket(SHUT_REASON_REMOTE);

	return 1L;
}

//网络连接
LRESULT CTCPSocket::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	
	//获取错误
	INT nErrorCode=WSAGETSELECTERROR(lParam);

	//判断状态
	if(nErrorCode==0)
	{
		//AfxMessageBox(L"连接成功");
#ifndef _DEBUG
		
		//设置时间
		SetTimer(IDI_DETEST_SOCKET,10000L,NULL);

#endif

		//设置状态
		m_cbSocketStatus=SOCKET_STATUS_CONNECT;

		//发送连接授权信息
		TCHAR szValidate[128] = TEXT("");
		lstrcpy(szValidate, szCompilation);

		SendData(MDM_KN_COMMAND, SUB_KN_VALIDATE_SOCKET, szValidate, CountArray(szValidate));
	}
	else
	{
		//关闭连接
		CloseSocket(SHUT_REASON_INSIDE);
	}

	//发送通知
	ASSERT(m_pITCPSocketSink!=NULL);
	if(m_pITCPSocketSink!=NULL) m_pITCPSocketSink->OnEventTCPSocketLink(m_wSocketID,nErrorCode);

	return 1L;
}

//时间消息
VOID CTCPSocket::OnTimer(UINT_PTR nIDEvent)
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
LRESULT CTCPSocket::OnSocketNotify(WPARAM wParam, LPARAM lParam)
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
DECLARE_CREATE_MODULE(TCPSocket);

//////////////////////////////////////////////////////////////////////////