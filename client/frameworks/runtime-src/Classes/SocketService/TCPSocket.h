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
	//����IPV6
	void SetIPV6TestUrl(const char *szUrl);
	//����ģʽ
	void SetBlockMode(bool bBlockMode);
	//���ý��ճ�ʱֵ
	void SetRevTimeOut(int timeout);
	//���÷��ͳ�ʱֵ
	void SetSendTimeOut(int timeout);
	//�������ӳ�ʱֵ
	void SetConnectTimeVal(long lSec, long lUsec);
	//����
	bool OnEventSocketCreate(int af = AF_INET, int type = SOCK_STREAM, int protocol=0);
	//����
	bool OnEventSocketConnet(const char *ip, WORD port);
	//����IP
	bool OnEventSocketConnetByIP(const char *ip, WORD port);
	//��������
	bool OnEventSocketConnetByDomain(const char *domain, WORD port);
	//���Ӽ��
	bool DetachTest();
	//�¼�ѡ��
	int OnEventSocketSelect();
	//��������
	bool OnEventSocketSend(const char *buf, int len, int flags);
	//��������
	int OnEventSocketRecv(char *buf, int len, int flags);
	//�رշ���
	int OnOnShutDownSend();
	//�رն�
	int OnShutDownRead();
	//�رն�д
	int OnShutDown();
	//�ر�
	int OnEventSocketClose();
};

