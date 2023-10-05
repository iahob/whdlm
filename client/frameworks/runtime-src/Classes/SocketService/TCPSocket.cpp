#include "TCPSocket.h"
#include "MCKernel.h"

CTCPSocket::CTCPSocket(SOCKET sock)
{
	m_sock = sock;
	m_bDetach = true;
	m_bBlockMode = true;
	m_szIPV6Url = NULL;
	m_lConnectTimeSec = 3;
	m_lConnectTimeUsec = 0;
	m_lRevTimeOut = 20000;
	m_lSendTimeOut = 3000;
}


CTCPSocket::~CTCPSocket()
{
	if (m_szIPV6Url)
	{
		delete m_szIPV6Url;
		m_szIPV6Url = NULL;
	}
}

void CTCPSocket::SetIPV6TestUrl(const char *szUrl)
{
	if (szUrl != NULL)
	{
		if (m_szIPV6Url)
		{
			delete m_szIPV6Url;
			m_szIPV6Url = NULL;
		}

		int nLen = strlen(szUrl);
		m_szIPV6Url = new char[nLen+1];
		strcpy(m_szIPV6Url, szUrl);
		m_szIPV6Url[nLen + 1] = '\0';
	}
}

void CTCPSocket::SetBlockMode(bool bBlockMode)
{
	m_bBlockMode = bBlockMode;
}

void CTCPSocket::SetRevTimeOut(int timeout)
{
	m_lRevTimeOut = timeout;
}
//设置发送超时值
void CTCPSocket::SetSendTimeOut(int timeout)
{
	m_lSendTimeOut = timeout;
}
//设置连接超时值
void CTCPSocket::SetConnectTimeVal(long lSec, long lUsec)
{
	m_lConnectTimeSec = lSec;
	m_lConnectTimeUsec = lUsec;
}

bool CTCPSocket::OnEventSocketCreate(int af, int type, int protocol)
{
	
	this->m_sock = socket(af, type, protocol);
	
	if (this->m_sock != INVALID_SOCKET)
	{
		
		int nResult = SOCKET_ERROR;

#ifndef _WIN32
		int fhFlags = fcntl(this->m_sock, F_GETFL);
		nResult = fcntl(this->m_sock, F_SETFL, fhFlags | O_NONBLOCK);
#else
		unsigned long off = 1;
		nResult = ioctlsocket(m_sock, FIONBIO, &off);
#endif
		if (nResult != SOCKET_ERROR)
		{
			return true;
		}

		OnEventSocketClose();
	}

	return false;
}

bool CTCPSocket::OnEventSocketConnet(const char * ip, WORD port)
{
	bool bResult = false;
	//win32 
	//WSAData wsaData;
	//WSAStartup(MAKEWORD(2, 2), &wsaData);


	int cbIpByte[4];
	memset(cbIpByte, 0, sizeof(cbIpByte));
	int nRet = sscanf(ip, "%d.%d.%d.%d", &cbIpByte[0], &cbIpByte[1], &cbIpByte[2], &cbIpByte[3]);
	if ((cbIpByte[3] | cbIpByte[2] | cbIpByte[1] | (unsigned int)cbIpByte[0]) > 0xFF || nRet != 4)
	{
		bResult = OnEventSocketConnetByDomain(ip, port);
	}
	else
	{
		bResult = OnEventSocketConnetByIP(ip, port);
	}
	

	//连接成功了
	if (bResult)
	{
		int nResult = SOCKET_ERROR;

#ifndef _WIN32
		int flags = fcntl(m_sock, F_GETFL);
		nResult = fcntl(m_sock, F_SETFL, flags & (~(O_NONBLOCK)));
#else
		unsigned long off = 0;
		nResult = ioctlsocket(this->m_sock, FIONBIO, &off);
#endif
		if (nResult != SOCKET_ERROR)
		{
			//设置接收超时
#ifndef _WIN32
			timeval  timeout;
			timeout.tv_sec = m_lRevTimeOut / 1000;
			timeout.tv_usec = 0;
			if(setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)))
#else
			if (setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&m_lRevTimeOut, sizeof(int)))
#endif
			{
				printf("设置接收超时值失败\r\n");
			}
			else
			{
#ifndef _WIN32
				timeval  timeout;
				timeout.tv_sec = m_lSendTimeOut / 1000;
				timeout.tv_usec = 0;
				nRet = setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
#else
				//设置发送超时
				int nRet = setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&m_lSendTimeOut, sizeof(int));
#endif
				//设置成功  返回结果
				if (!nRet) return true;

				printf("设置接收超时值失败\r\n");
				
			}
		}
		else
		{
			OnEventSocketClose();
		}
	}
	
	return false;
}


bool CTCPSocket::OnEventSocketConnetByIP(const char *ip, WORD port)
{
	addrinfo *pResultAddr;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int nRet;
	if (m_szIPV6Url != NULL)
	{
		nRet = getaddrinfo(m_szIPV6Url, 0, &hints, &pResultAddr);
	}
	else
	{
		nRet = getaddrinfo(ip, 0, &hints, &pResultAddr);
	}

	if (nRet)
	{
		if (!m_szIPV6Url)
		{
			printf("OnEventSocketConnetByIP getaddrinfo fail %s", ip);
		}
		return false;
	}

	
	if (!OnEventSocketCreate(pResultAddr->ai_family, SOCK_STREAM, 0))
	{
		printf("OnEventSocketConnetByIP OnEventSocketCreate fail");
		return false;
	}

	int nResult = SOCKET_ERROR;

	if (pResultAddr->ai_family == AF_INET6)
	{
		sockaddr_in6 sin6;
		int cbIpByte[4];
		char szIpV6[128];
		ZeroMemory(szIpV6, sizeof(szIpV6));
		ZeroMemory(cbIpByte, sizeof(cbIpByte));
		sscanf(ip, "%d.%d.%d.%d", cbIpByte, &cbIpByte[1], &cbIpByte[2], &cbIpByte[3]);
		sprintf(szIpV6,"0064:ff9b:0000:0000:0000:0000:%02x%02x:%02x%02x",cbIpByte[0], &cbIpByte[1], &cbIpByte[2], &cbIpByte[3]);

		ZeroMemory(&sin6, sizeof(sin6));
		sin6.sin6_family = AF_INET6;
		sin6.sin6_port = htons(port);
		if (inet_pton(AF_INET6, szIpV6, &sin6.sin6_addr) < 0)
		{
			return false;
		}

		nResult = connect(m_sock, (const struct sockaddr *)&sin6, sizeof(sin6));

	}
	else
	{
		if (pResultAddr->ai_family != AF_INET)
			return false;

		sockaddr_in sin;
		ZeroMemory(&sin, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);

#ifndef _WIN32
		if (!inet_aton(ip, &sin.sin_addr))
#else
		if (!inet_pton(AF_INET, ip, &sin.sin_addr))
#endif
		{
			return false;
		}

		nResult = connect(m_sock, (const struct sockaddr *)&sin, sizeof(sin));
	}
	

	return nResult >= 0 || CTCPSocket::DetachTest();
}


bool CTCPSocket::OnEventSocketConnetByDomain(const char *domain, WORD port)
{

	struct addrinfo hints, *ppResult, *cur;
	ZeroMemory(&hints, sizeof(addrinfo));
	
	bool bRet = false;

	char szPort[16];
	ZeroMemory(szPort, sizeof(szPort));
	sprintf(szPort, "%u", port);

	if (!getaddrinfo(domain, szPort, &hints, &ppResult))
	{
		int ai_family;
		for (cur = ppResult; cur != NULL; cur = cur->ai_next)
		{
			if (cur->ai_family == AF_INET)
			{
				ai_family = AF_INET;
			}
			else if (cur->ai_family == AF_INET6)
			{
				ai_family = AF_INET6;
			}

			if (OnEventSocketCreate(ai_family, SOCK_STREAM, 0))
			{
				break;
			}
		}

		int nReuslt = connect(m_sock, cur->ai_addr, cur->ai_addrlen);

		if (nReuslt >= 0 || CTCPSocket::DetachTest())
		{
			bRet = true;
		}
	}
	
	if (ppResult)
	{
		freeaddrinfo(ppResult);
	}

	return bRet;
}

bool CTCPSocket::DetachTest()
{
#ifndef _WIN32
	if (errno != EINPROGRESS)
#else
	if (WSAGetLastError() != WSAEWOULDBLOCK)
#endif

	{
		return false;
	}

	fd_set fdWrite;
	FD_ZERO(&fdWrite);
	FD_SET(m_sock,&fdWrite);

	struct timeval timeout;
	timeout.tv_sec = m_lSendTimeOut;
	timeout.tv_usec = 0;

	if (select(m_sock+1, NULL, &fdWrite, NULL, &timeout) <= 0)
	{
		return false;
	}

	int error = SOCKET_ERROR;
	socklen_t len = sizeof(error);

	getsockopt(m_sock, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

	return error == 0;
}

int CTCPSocket::OnEventSocketSelect()
{
	
	FD_ZERO(&m_fdRead);
	FD_SET(m_sock, &m_fdRead);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int nRet = select(m_sock+1, &m_fdRead, NULL, NULL, &timeout);
	if (nRet != SOCKET_ERROR)
	{
		if (nRet)
		{
			if (m_sock != INVALID_SOCKET)
				nRet = -3 - ((FD_ISSET(m_sock, &m_fdRead) < 1) - 1);//-3是返回值偏移基值
		}
	}
	return nRet;
}

bool CTCPSocket::OnEventSocketSend(const char *buf, int len, int flags)
{
	int nAllSentDataSize = 0;
	//判断网络接口
	if (m_sock != INVALID_SOCKET)
	{
		//需要发送的长度
		if (len > 0)
		{
			do {
				//发送数据
				int nCurSentDataSize = send(m_sock, &buf[nAllSentDataSize], len - nAllSentDataSize, flags);
				//判断是否发送成功
				if (nCurSentDataSize+1 < 2)
					return false;
				//累计发送字节
				nAllSentDataSize += nCurSentDataSize;
			} while (nAllSentDataSize < len);
		}

		//已经发送字节 和 需要发送的字节
		return nAllSentDataSize == len;
	}
	
	return false;
}

int CTCPSocket::OnEventSocketRecv(char *buf, int len, int flags)
{
	printf("OnEventSocketRecv len %d,flags %d\n",len,flags);
	if (m_sock != INVALID_SOCKET)
		return recv(m_sock, buf, len, flags);
	return SOCKET_ERROR;
}

int CTCPSocket::OnOnShutDownSend()
{
	if (m_sock != INVALID_SOCKET)
	{
		return shutdown(m_sock, 1);
	}

	return INVALID_SOCKET;
}
//关闭读
int CTCPSocket::OnShutDownRead()
{
	if (m_sock != INVALID_SOCKET)
	{
		return shutdown(m_sock, 0);
	}

	return INVALID_SOCKET;
}
//关闭读写
int CTCPSocket::OnShutDown()
{
	//SHUT_RD SHUT_WR SHUT_RDWR
	if (m_sock != INVALID_SOCKET)
	{
		//SD_BOTH
		return shutdown(m_sock, 2);
	}

	return INVALID_SOCKET;
}

int CTCPSocket::OnEventSocketClose()
{
	int nResult = 0;

	if (m_sock != INVALID_SOCKET)
	{

	#ifndef _WIN32
		nResult = close(m_sock);
	#else
		nResult = closesocket(m_sock);
		//WSACleanup();
	#endif
		if (nResult)
		{
			char szMessage[128];
			sprintf(szMessage, "OnEventSocketClose: error:%d result:%d", errno, nResult);
			//输出日志
			CMCKernel::GetInstance()->LogOut((const char*)szMessage);
		}
	}
	return nResult;
}