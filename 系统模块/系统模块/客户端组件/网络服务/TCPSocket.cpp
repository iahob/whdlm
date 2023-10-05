//����ӣ���ݡ��Ŀ�,Ⱥ:16886186����ϵQQ:412069412
#include "Stdafx.h"
#include "TCPSocket.h"
#include "WHNetwork.h"

//////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define	IDI_DETEST_SOCKET			100									//������

//��Ϣ����
#define WM_SOCKET_NOTIFY			(WM_USER+100)						//������Ϣ

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTCPSocket, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(WM_SOCKET_NOTIFY,OnSocketNotify)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CTCPSocket::CTCPSocket()
{
	//��������
	m_wSocketID=0;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//�ں˱���
	m_pITCPSocketSink=NULL;
	m_hSocket=INVALID_SOCKET;

	//������Ϣ
	m_cbProxyType=PROXY_NONE;
	ZeroMemory(&m_ProxyServer,sizeof(m_ProxyServer));

	//���ܱ���
	m_wBufferSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//�������
	m_dwBufferData=0L;
	m_dwBufferSize=0L;
	m_bNeedBuffer=false;
	m_pcbDataBuffer=NULL;

	//��������
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;

	return;
}

//��������
CTCPSocket::~CTCPSocket()
{
	//�ر�����
	if (m_hSocket!=INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;
	}

	//ɾ������
	SafeDeleteArray(m_pcbDataBuffer);
	
	return;
}

//�ӿڲ�ѯ
VOID * CTCPSocket::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocket,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocket,Guid,dwQueryVer);
	return NULL;
}

//���ñ�ʶ
VOID CTCPSocket::SetSocketID(WORD wSocketID)
{
	//���ñ���
	m_wSocketID=wSocketID;

	return;
}

//���ýӿ�
bool CTCPSocket::SetTCPSocketSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if(pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPSocketSink)!=NULL);
		m_pITCPSocketSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPSocketSink);

		//�ɹ��ж�
		if(m_pITCPSocketSink==NULL) return false;
	}
	else m_pITCPSocketSink=NULL;

	return true;
}

//�������
BYTE CTCPSocket::ProxyServerTesting()
{
	//У�����
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_cbProxyType!=PROXY_NONE);
	ASSERT(m_cbSocketStatus==SOCKET_STATUS_IDLE);

	//У��״̬
	if(m_hSocket!=INVALID_SOCKET) return CONNECT_EXCEPTION;
	if(m_cbProxyType==PROXY_NONE) return CONNECT_EXCEPTION;
	if(m_cbSocketStatus!=SOCKET_STATUS_IDLE) return CONNECT_EXCEPTION;

	//���� SOCKET
	m_hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(m_hSocket==INVALID_SOCKET) return CONNECT_EXCEPTION;

	//���Ӵ���
	BYTE cbResult=ConnectProxyServer();

	//�ر�����
	closesocket(m_hSocket);
	m_hSocket=INVALID_SOCKET;

	return cbResult;
}

//���ô���
bool CTCPSocket::SetProxyServerInfo(BYTE cbProxyType, const tagProxyServer & ProxyServer)
{
	switch(cbProxyType)
	{
	case PROXY_NONE:		//û�д���
		{
			//���ñ���
			m_cbProxyType=cbProxyType;
			ZeroMemory(&m_ProxyServer,sizeof(m_ProxyServer));

			return true;
		}
	case PROXY_HTTP:		//HTTP ����
	case PROXY_SOCKS4:		//SOCKET4 ����
	case PROXY_SOCKS5:		//SOCKET5 ����
		{
			//���ñ���
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

//���Ӳ���
BYTE CTCPSocket::Connect(DWORD dwServerIP, WORD wPort)
{
	//AfxMessageBox(L"��������...");
	//У�����
	ASSERT(dwServerIP!=INADDR_NONE);
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_cbSocketStatus==SOCKET_STATUS_IDLE);

	//���ò���
	m_wBufferSize=0;
	m_dwSendTickCount=GetTickCount()/1000L;
	m_dwRecvTickCount=GetTickCount()/1000L;

	try
	{
		//���� SOCKET
		m_hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(m_hSocket==INVALID_SOCKET) throw CONNECT_EXCEPTION;

		//��������
		if(m_hWnd==NULL)
		{
			CRect rcSocketWnd(0,0,0,0);
			Create(NULL,NULL,WS_CHILD,rcSocketWnd,GetDesktopWindow(),100);
		}

		//��������
		SOCKADDR_IN SocketAddr;
		ZeroMemory(&SocketAddr,sizeof(SocketAddr));

		//���ñ���
		SocketAddr.sin_family=AF_INET;
		SocketAddr.sin_port=htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr=dwServerIP;
		
		//��������
		if(m_cbProxyType!=PROXY_NONE)
		{
			//AfxMessageBox(L"����");
			//���Ӵ���
			DWORD dwResult=ConnectProxyServer();
			if(dwResult!=CONNECT_SUCCESS) throw dwResult;

			//��������
			switch(m_cbProxyType)
			{
			case PROXY_HTTP:		//HTTP ����
				{
					//��������
					char cbDataBuffer[512];
					BYTE * pcbServerIP=(BYTE *)&dwServerIP;
					_snprintf(cbDataBuffer,CountArray(cbDataBuffer),"CONNECT %d.%d.%d.%d:%d HTTP/1.0\r\nUser-Agent: WHPlatform/0.1\r\n\r\n",
						pcbServerIP[0],pcbServerIP[1],pcbServerIP[2],pcbServerIP[3],wPort);

					//��������
					INT nRequestLen=(INT)strlen(cbDataBuffer);
					if(send(m_hSocket,cbDataBuffer,nRequestLen,0)!=nRequestLen) throw CONNECT_PROXY_FAILURE;

					//������Ӧ
					INT nRecvCount=recv(m_hSocket,cbDataBuffer,sizeof(cbDataBuffer),0);
					if(nRecvCount>0) cbDataBuffer[nRecvCount]=0;

					//����ж�
					bool bSuccess=false;
					for (INT i=0; i<nRecvCount; i++)
					{
						if((cbDataBuffer[i]==' ')&&(cbDataBuffer[i+1]=='2')&&(cbDataBuffer[i+2]=='0')&&(cbDataBuffer[i+3]=='0'))
						{
							bSuccess=true;
							break;
						}
					}

					//����ж�
					if(bSuccess==false) throw CONNECT_PROXY_FAILURE;

					break;
				}
			case PROXY_SOCKS5:		//Socket5 ����
				{
					//��������
					INT nOff=4;
					char cbDataBuffer[256]={5,1,0,1};

					//���ӵ�ַ
					CopyMemory(cbDataBuffer+nOff,&SocketAddr.sin_addr,sizeof(SocketAddr.sin_addr));
					nOff+=sizeof(SocketAddr.sin_addr);

					//���Ӷ˿�
					*(WORD*)(cbDataBuffer+nOff)=SocketAddr.sin_port;
					nOff+=sizeof(WORD);

					//��������
					if(send(m_hSocket,cbDataBuffer,nOff,0)!=nOff) throw CONNECT_PROXY_FAILURE;

					//������Ӧ
					INT nRecvCount=recv(m_hSocket,cbDataBuffer,4,0);
					if(nRecvCount!=4) throw CONNECT_PROXY_FAILURE;
					if(cbDataBuffer[0]!=5) throw CONNECT_PROXY_FAILURE;

					//ʧ���ж�
					if(cbDataBuffer[1]==0) throw CONNECT_PROXY_FAILURE;
					
					//��������
					INT nLeftDataLen=(cbDataBuffer[3]==1)?6:cbDataBuffer[4]+2;
					if(recv(m_hSocket,cbDataBuffer,nLeftDataLen,0)!=nLeftDataLen) throw CONNECT_PROXY_FAILURE;
                    
					break;
				}
			case PROXY_SOCKS4:		//Socket4 ����
				{
					//��������
					INT nOff=2;
					char cbDataBuffer[256]={4,1};

					//���Ӷ˿�
					*(WORD*)(cbDataBuffer+nOff)=SocketAddr.sin_port;
					nOff+=sizeof(WORD);

					//���ӵ�ַ
					CopyMemory(cbDataBuffer+nOff,&SocketAddr.sin_addr,sizeof(SocketAddr.sin_addr));
					nOff+=sizeof(SocketAddr.sin_addr);

					//�����ֶ�
					cbDataBuffer[nOff++]=0;

					//��������
					INT nResult=send(m_hSocket,cbDataBuffer,nOff,0);
					if(nResult!=nOff) throw CONNECT_PROXY_FAILURE;

					//������Ӧ
					nResult=recv(m_hSocket,cbDataBuffer,8,0);
					if(nResult!=8) throw CONNECT_PROXY_FAILURE;
					if(cbDataBuffer[0]!=0) throw CONNECT_PROXY_FAILURE;

					//ʧ���ж�
					if(cbDataBuffer[1]!=90) throw CONNECT_PROXY_FAILURE;

					break;
				}
			default:		//Ĭ�ϴ���
				{
					ASSERT(FALSE);
					throw CONNECT_PROXY_FAILURE;
				}
			}

			//���ӳɹ�
			WSASetLastError(0);

			//���ñ���
			m_cbSocketStatus=SOCKET_STATUS_CONNECT;

			//������Ϣ
			PostMessage(WM_SOCKET_NOTIFY,0,MAKELONG(FD_CONNECT,0));

			//�󶨴���
			WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY,FD_READ|FD_CONNECT|FD_CLOSE|FD_WRITE);
		}
		else
		{
			//AfxMessageBox(L"��������");
			//�󶨴���
			WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY,FD_READ|FD_CONNECT|FD_CLOSE|FD_WRITE);
			
			//��������
			INT nErrorCode=connect(m_hSocket,(SOCKADDR *)&SocketAddr,sizeof(SocketAddr));
			if((nErrorCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSAEWOULDBLOCK)) throw CONNECT_EXCEPTION;

			//���ñ���
			m_cbSocketStatus=SOCKET_STATUS_WAIT;
		}
	}
	catch (BYTE cbErrorCode)
	{
		//�ر�����
		CloseSocket(SHUT_REASON_INSIDE);

		return cbErrorCode;
	}

	return CONNECT_SUCCESS;
}

//���Ӳ���
BYTE CTCPSocket::Connect(LPCTSTR pszServerIP, WORD wPort)
{
	//AfxMessageBox(pszServerIP);
	//У������
	ASSERT((wPort!=0)&&(pszServerIP!=NULL));
	if((pszServerIP==NULL)||(wPort==0)) return false;

	//���Ӳ���
	return Connect(g_WHNetwork.ConversionAddr(pszServerIP),wPort);
}

//���ͺ���
WORD CTCPSocket::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//У��״̬
	if(m_hSocket==INVALID_SOCKET) return 0L;
	if(m_cbSocketStatus!=SOCKET_STATUS_CONNECT) return 0L;

	//��������
	if(m_TCPSocketData.InitSocketData(wMainCmdID,wSubCmdID,NULL,0)==false)
	{
		return false;
	}

	//��������
	if(SendDataBuffer(m_TCPSocketData.m_cbDataBuffer,m_TCPSocketData.m_wBufferSize)==false)
	{
		return false;
	}

	return true;
}

//���ͺ���
WORD CTCPSocket::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//У���С
	ASSERT(wDataSize<=SOCKET_TCP_PACKET);
	if(wDataSize>SOCKET_TCP_PACKET) return false;

	//У��״̬
	if(m_hSocket==INVALID_SOCKET) return false;
	if(m_cbSocketStatus!=SOCKET_STATUS_CONNECT) return false;

	//��������
	if(m_TCPSocketData.InitSocketData(wMainCmdID,wSubCmdID,pData,wDataSize)==false)
	{
		return false;
	}

	//��������
	if(SendDataBuffer(m_TCPSocketData.m_cbDataBuffer,m_TCPSocketData.m_wBufferSize)==false)
	{
		return false;
	}

	return true;
}

//�ر�����
VOID CTCPSocket::CloseSocket()
{
	//�ر�����
	CloseSocket(SHUT_REASON_NORMAL);
	
	return;
}

//�ر�����
VOID CTCPSocket::CloseSocket(BYTE cbShutReason)
{
	//���ܱ���
	m_wBufferSize=0;
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;

	//�������
	m_dwBufferData=0L;
	m_dwBufferSize=0L;
	m_bNeedBuffer=false;
	SafeDeleteArray(m_pcbDataBuffer);

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
			ASSERT(m_pITCPSocketSink!=NULL);
			if(m_pITCPSocketSink!=NULL) m_pITCPSocketSink->OnEventTCPSocketShut(m_wSocketID,cbShutReason);
		}
	}

	return;
}

//��������
VOID CTCPSocket::AmortizeBuffer(VOID * pData, WORD wDataSize)
{
	//���뻺��
	if((m_dwBufferData+wDataSize)>m_dwBufferSize)
	{
		//��������
		LPBYTE pcbDataBuffer=NULL;
		LPBYTE pcbDeleteBuffer=m_pcbDataBuffer;

		//�����С
		DWORD dwNeedSize=m_dwBufferData+wDataSize;
		DWORD dwApplySize=__max(dwNeedSize,m_dwBufferSize*2L);

		//���뻺��
		try
		{
			pcbDataBuffer=new BYTE[dwApplySize];
		}
		catch(...) { }

		//ʧ���ж�
		if(pcbDataBuffer==NULL)
		{
			CloseSocket(SHUT_REASON_EXCEPTION);
			return;
		}

		//���ñ���
		m_dwBufferSize=dwApplySize;
		m_pcbDataBuffer=pcbDataBuffer;
		CopyMemory(m_pcbDataBuffer,pcbDeleteBuffer,m_dwBufferData);

		//ɾ������
		SafeDeleteArray(pcbDeleteBuffer);
	}

	//���ñ���
	m_bNeedBuffer=true;
	m_dwBufferData+=wDataSize;
	CopyMemory(m_pcbDataBuffer+m_dwBufferData-wDataSize,pData,wDataSize);

	return;
}

//��������
WORD CTCPSocket::SendDataBuffer(VOID * pBuffer, WORD wSendSize)
{
	//��������
	WORD wTotalCount=0;

	//���ñ���
	m_dwSendTickCount=GetTickCount()/1000L;

	//��������
	while((m_bNeedBuffer==false)&&(wTotalCount<wSendSize))
	{
		//��������
		INT nSendCount=send(m_hSocket,(char *)pBuffer+wTotalCount,wSendSize-wTotalCount,0);

		//�����ж�
		if(nSendCount==SOCKET_ERROR)
		{
			//�����ж�
			if(WSAGetLastError()==WSAEWOULDBLOCK)
			{
				AmortizeBuffer((LPBYTE)pBuffer+wTotalCount,wSendSize-wTotalCount);
				return wSendSize;
			}

			//�ر�����
			CloseSocket(SHUT_REASON_EXCEPTION);

			return 0L;
		}
		else
		{
			//���ñ���
			wTotalCount+=nSendCount;
		}
	}

	//��������
	if(wTotalCount>wSendSize)
	{
		AmortizeBuffer((LPBYTE)pBuffer+wTotalCount,wSendSize-wTotalCount);
	}

	return wSendSize;
}

//���Ӵ���
BYTE CTCPSocket::ConnectProxyServer()
{
	//У��״̬
	ASSERT(m_cbProxyType!=PROXY_NONE);
	if(m_cbProxyType==PROXY_NONE) return CONNECT_EXCEPTION;

	//��������
	SOCKADDR_IN SocketAddrProxy;
	ZeroMemory(&SocketAddrProxy,sizeof(SocketAddrProxy));

	//���ñ���
	SocketAddrProxy.sin_family=AF_INET;
	SocketAddrProxy.sin_port=htons(m_ProxyServer.wProxyPort);
	SocketAddrProxy.sin_addr.S_un.S_addr=g_WHNetwork.ConversionAddr(m_ProxyServer.szProxyServer);

	//���Ӵ���
	INT nErrorCode=connect(m_hSocket,(sockaddr *)&SocketAddrProxy,sizeof(SocketAddrProxy));
	if(nErrorCode==SOCKET_ERROR) return CONNECT_PROXY_FAILURE;

	//��������
	if(m_cbProxyType==PROXY_SOCKS5)
	{
		//��������
		INT nOff=2;
		char cbDataBuffer[256]={5,1};

		//��������
		cbDataBuffer[nOff++]=(m_ProxyServer.szUserName[0]==0)?0:2;
		if(send(m_hSocket,cbDataBuffer,nOff,0)!=nOff) return CONNECT_PROXY_FAILURE;

		//��������
		if(recv(m_hSocket,cbDataBuffer,2,0)!=2) return CONNECT_PROXY_FAILURE;

		//���ݷ���
		if(cbDataBuffer[0]!=5) return CONNECT_PROXY_FAILURE;
		if((cbDataBuffer[1]!=0)&&(cbDataBuffer[1]!=2)) return CONNECT_PROXY_FAILURE;

		//�����֤
		if(cbDataBuffer[1]==2)
		{
			//��������
			INT nStringLen=0;
			CT2CA strUserName(m_ProxyServer.szUserName);
			CT2CA strPassword(m_ProxyServer.szPassword);

			//��¼�ʺ�
			nOff=0;
			cbDataBuffer[nOff++]=1;
			nStringLen=lstrlenA(strUserName);
			cbDataBuffer[nOff++]=(char)nStringLen;
			CopyMemory(cbDataBuffer+nOff,(LPCSTR)strUserName,nStringLen);
			nOff+=nStringLen;

			//��¼����
			nStringLen=lstrlenA(strPassword);
			cbDataBuffer[nOff++]=(char)nStringLen;
			CopyMemory(cbDataBuffer+nOff,(LPCSTR)strPassword,nStringLen);
			nOff+=nStringLen;

			//��������
			if(send(m_hSocket,cbDataBuffer,nOff,0)!=nOff) return CONNECT_PROXY_FAILURE;

			//��������
			if(recv(m_hSocket,cbDataBuffer,2,0)!=2) return CONNECT_PROXY_FAILURE;

			//���ݷ���
			if((cbDataBuffer[0]!=1)||(cbDataBuffer[1]!=0)) return CONNECT_PROXY_USER_INVALID;
		}
	}

	return CONNECT_SUCCESS;
}

//�����ȡ
LRESULT CTCPSocket::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	INT nResultCode=recv(m_hSocket,(char *)m_cbDataBuffer+m_wBufferSize,sizeof(m_cbDataBuffer)-m_wBufferSize,0);
	
	//�ر��ж�
	if(nResultCode==SOCKET_ERROR)
	{
		CloseSocket(SHUT_REASON_EXCEPTION);
		return 1L;
	}

	//���ñ���
	m_wBufferSize+=nResultCode;
	m_dwRecvTickCount=GetTickCount()/1000L;

	//���ݴ���
	while((m_wBufferSize>=sizeof(TCP_Head))&&(m_hSocket!=INVALID_SOCKET))
	{
		//��������
		TCP_Info * pInfo=(TCP_Info *)m_cbDataBuffer;

		//����У��
		if((pInfo->wPacketSize<sizeof(TCP_Info))||(pInfo->wPacketSize>SOCKET_TCP_BUFFER))
		{
			CloseSocket(SHUT_REASON_EXCEPTION);
			return 1L;
		}

		//�����ж�
		if(m_wBufferSize<pInfo->wPacketSize) return 1L;

		//��������
		tagTCPData TCPData;
		WORD wBufferDataSize=pInfo->wPacketSize;

		//��������
		if(m_TCPSocketData.SetBufferData(m_cbDataBuffer,wBufferDataSize,TCPData)==false)
		{
			CloseSocket(SHUT_REASON_EXCEPTION);
			return 1L;
		}
		
		//�ƶ�����
		m_wBufferSize-=wBufferDataSize;
		MoveMemory(m_cbDataBuffer,m_cbDataBuffer+wBufferDataSize,m_wBufferSize);

		//������
		if((TCPData.Command.wMainCmdID==MDM_KN_COMMAND)&&(TCPData.Command.wSubCmdID==SUB_KN_DETECT_SOCKET))
		{
			SendData(MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET);
			continue;
		}

		//�����ж�
		if((TCPData.Command.wMainCmdID!=MDM_KN_COMMAND)&&(m_pITCPSocketSink!=NULL))
		{
			try
			{
				//��������
				bool bSuccess=m_pITCPSocketSink->OnEventTCPSocketRead(m_wSocketID,TCPData.Command,TCPData.pDataBuffer,TCPData.wDataSize);

				//�ر��ж�
				if(bSuccess==false)
				{
					CloseSocket(SHUT_REASON_EXCEPTION);
					return 1L;
				}
			}
			catch (...)
			{
				//�ر�����
				CloseSocket(SHUT_REASON_EXCEPTION);
				return 1L;				
			}
		}
	};

	return 1L;
}

//���緢��
LRESULT CTCPSocket::OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam)
{
	//�����ж�
	if((m_bNeedBuffer==true)&&(m_dwBufferData>0L))
	{
		//��������
		DWORD dwTotalCount=0;
		DWORD dwPacketSize=4096;

		//���ñ���
		m_dwSendTickCount=GetTickCount()/1000L;

		//��������
		while(dwTotalCount<m_dwBufferData)
		{
			//��������
			WORD wSendSize=(WORD)__min(dwPacketSize,m_dwBufferData-dwTotalCount);
			INT nSendCount=send(m_hSocket,(char*)m_pcbDataBuffer+dwTotalCount,wSendSize,0);

			//�����ж�
			if(nSendCount==SOCKET_ERROR)
			{
				//�����ж�
				if(WSAGetLastError()==WSAEWOULDBLOCK)
				{
					//���ñ���
					m_bNeedBuffer=false;
					m_dwBufferData-=dwTotalCount;

					//�ƶ��ڴ�
					if(m_dwBufferData>0L)
					{
						m_bNeedBuffer=true;
						MoveMemory(m_pcbDataBuffer,m_pcbDataBuffer+dwTotalCount,m_dwBufferData);
					}

					return 1L;
				}

				//�ر�����
				CloseSocket(SHUT_REASON_EXCEPTION);

				return 1L;
			}

			//���ñ���
			dwTotalCount+=nSendCount;
		}

		//���ñ���
		m_dwBufferData=0L;
		m_bNeedBuffer=false;
	}

	return 1L;
}

//����ر�
LRESULT CTCPSocket::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	//�ر�����
	CloseSocket(SHUT_REASON_REMOTE);

	return 1L;
}

//��������
LRESULT CTCPSocket::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	
	//��ȡ����
	INT nErrorCode=WSAGETSELECTERROR(lParam);

	//�ж�״̬
	if(nErrorCode==0)
	{
		//AfxMessageBox(L"���ӳɹ�");
#ifndef _DEBUG
		
		//����ʱ��
		SetTimer(IDI_DETEST_SOCKET,10000L,NULL);

#endif

		//����״̬
		m_cbSocketStatus=SOCKET_STATUS_CONNECT;

		//����������Ȩ��Ϣ
		TCHAR szValidate[128] = TEXT("");
		lstrcpy(szValidate, szCompilation);

		SendData(MDM_KN_COMMAND, SUB_KN_VALIDATE_SOCKET, szValidate, CountArray(szValidate));
	}
	else
	{
		//�ر�����
		CloseSocket(SHUT_REASON_INSIDE);
	}

	//����֪ͨ
	ASSERT(m_pITCPSocketSink!=NULL);
	if(m_pITCPSocketSink!=NULL) m_pITCPSocketSink->OnEventTCPSocketLink(m_wSocketID,nErrorCode);

	return 1L;
}

//ʱ����Ϣ
VOID CTCPSocket::OnTimer(UINT_PTR nIDEvent)
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
LRESULT CTCPSocket::OnSocketNotify(WPARAM wParam, LPARAM lParam)
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
DECLARE_CREATE_MODULE(TCPSocket);

//////////////////////////////////////////////////////////////////////////