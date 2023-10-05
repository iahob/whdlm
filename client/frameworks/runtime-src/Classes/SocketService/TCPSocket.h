#pragma once

#include "Define.h"

#ifndef ZeroMemory
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#endif

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <net/if.h>
typedef int				SOCKET;

//#pragma region define win32 const variable in linux
#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
//#pragma endregion
#else
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int 			socklen_t;
#endif

class CTCPSocket
{
protected:
	bool m_bDetach;
	SOCKET m_sock;
	fd_set m_fdRead;
	bool m_bBlockMode;
	char *m_szIPV6Url;
	int m_lRevTimeOut;
	int m_lSendTimeOut;
	int m_lConnectTimeSec;
	int m_lConnectTimeUsec;
public:
	CTCPSocket(SOCKET sock = INVALID_SOCKET);
	virtual ~CTCPSocket();
public:
	//设置IPV6
	void SetIPV6TestUrl(const char *szUrl);
	//设置模式
	void SetBlockMode(bool bBlockMode);
	//设置接收超时值
	void SetRevTimeOut(int timeout);
	//设置发送超时值
	void SetSendTimeOut(int timeout);
	//设置连接超时值
	void SetConnectTimeVal(long lSec, long lUsec);
	//创建
	bool OnEventSocketCreate(int af = AF_INET, int type = SOCK_STREAM, int protocol=0);
	//连接
	bool OnEventSocketConnet(const char *ip, WORD port);
	//连接IP
	bool OnEventSocketConnetByIP(const char *ip, WORD port);
	//连接域名
	bool OnEventSocketConnetByDomain(const char *domain, WORD port);
	//连接检查
	bool DetachTest();
	//事件选择
	int OnEventSocketSelect();
	//发送数据
	bool OnEventSocketSend(const char *buf, int len, int flags);
	//接收数据
	int OnEventSocketRecv(char *buf, int len, int flags);
	//关闭发送
	int OnOnShutDownSend();
	//关闭读
	int OnShutDownRead();
	//关闭读写
	int OnShutDown();
	//关闭
	int OnEventSocketClose();
};

