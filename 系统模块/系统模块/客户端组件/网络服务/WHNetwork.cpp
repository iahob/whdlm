//����ӣ���ݡ��Ŀ�,Ⱥ:16886186����ϵQQ:412069412
#include "stdafx.h"
#include "TCPSocket.h"
#include "WHNetwork.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CWHNetwork::CWHNetwork()
{
}

//��������
CWHNetwork::~CWHNetwork()
{
}

//�ӿڲ�ѯ
VOID * CWHNetwork::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IWHNetwork,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IWHNetwork,Guid,dwQueryVer);
	return NULL;
}

//��ַת��
DWORD CWHNetwork::ConversionAddr(LPCTSTR pszAddress)
{
	//ת����ַ
	CT2CA ServerAddr(pszAddress);
	DWORD dwServerIP=inet_addr(ServerAddr);

	//��������
	if(dwServerIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(ServerAddr);
		if(lpHost!=NULL) dwServerIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}

	return dwServerIP;
}

//��ȡ��ַ
DWORD CWHNetwork::GetLocalNetworkIP(DWORD dwAddrList[], DWORD dwMaxCount)
{
	//��ȡ����
	CHAR szHostName[64];
	gethostname(szHostName,sizeof(szHostName));
	HOSTENT * pHostent=gethostbyname(szHostName);

	//��������
	DWORD dwIPCount=0;
	LPIN_ADDR * lpAddrList=(LPIN_ADDR *)pHostent->h_addr_list;

	//���õ�ַ
	while(*lpAddrList!=NULL)
	{
		//����ж�
		if(dwIPCount>=dwMaxCount)
		{
			ASSERT(FALSE);
			break;
		}

		//��ַת��
		dwAddrList[dwIPCount++]=(*lpAddrList++)->S_un.S_addr;
	}

	return dwIPCount;
}

//ö�ٴ���
bool CWHNetwork::EnumProxyServerType(WORD wIndex, tagProxyDescribe & ProxyDescribe)
{
	//��Ϣ���
	switch(wIndex)
	{
	case PROXY_NONE:			//��ʹ�ô���
		{
			ProxyDescribe.cbProxyType=PROXY_NONE;
			lstrcpyn(ProxyDescribe.szDescribe,TEXT("��ʹ�ô���"),CountArray(ProxyDescribe.szDescribe));
			return true;
		}
	case PROXY_HTTP:			//HTTP ����
		{
			ProxyDescribe.cbProxyType=PROXY_HTTP;
			lstrcpyn(ProxyDescribe.szDescribe,TEXT("HTTP ����"),CountArray(ProxyDescribe.szDescribe));
			return true;
		}
	case PROXY_SOCKS4:			//SOCKS4 ����
		{
			ProxyDescribe.cbProxyType=PROXY_SOCKS4;
			lstrcpyn(ProxyDescribe.szDescribe,TEXT("SOCKS4 ����"),CountArray(ProxyDescribe.szDescribe));
			return true;
		}
	case PROXY_SOCKS5:			//SOCKS5 ����
		{
			ProxyDescribe.cbProxyType=PROXY_SOCKS5;
			lstrcpyn(ProxyDescribe.szDescribe,TEXT("SOCKS5 ����"),CountArray(ProxyDescribe.szDescribe));
			return true;
		}
	}

	return false;
}

//�������
BYTE CWHNetwork::ProxyServerTesting(BYTE cbProxyType, const tagProxyServer & ProxyServer)
{
	//У�����
	ASSERT(cbProxyType!=PROXY_NONE);
	if(cbProxyType==PROXY_NONE) return CONNECT_EXCEPTION;

	//��������
	CTCPSocket TCPSocket;
	if(TCPSocket.SetProxyServerInfo(cbProxyType,ProxyServer)==false) return CONNECT_EXCEPTION;

	//�������
	return TCPSocket.ProxyServerTesting();
}

//////////////////////////////////////////////////////////////////////////

//�������
CWHNetwork g_WHNetwork;

//�����������
extern "C" __declspec(dllexport) VOID * CreateWHNetwork(const GUID & Guid, DWORD dwInterfaceVer)
{
	return g_WHNetwork.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////