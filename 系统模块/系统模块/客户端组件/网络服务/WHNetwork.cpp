//分享樱花草、的酷,群:16886186，联系QQ:412069412
#include "stdafx.h"
#include "TCPSocket.h"
#include "WHNetwork.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CWHNetwork::CWHNetwork()
{
}

//析构函数
CWHNetwork::~CWHNetwork()
{
}

//接口查询
VOID * CWHNetwork::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IWHNetwork,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IWHNetwork,Guid,dwQueryVer);
	return NULL;
}

//地址转换
DWORD CWHNetwork::ConversionAddr(LPCTSTR pszAddress)
{
	//转换地址
	CT2CA ServerAddr(pszAddress);
	DWORD dwServerIP=inet_addr(ServerAddr);

	//域名解释
	if(dwServerIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(ServerAddr);
		if(lpHost!=NULL) dwServerIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}

	return dwServerIP;
}

//获取地址
DWORD CWHNetwork::GetLocalNetworkIP(DWORD dwAddrList[], DWORD dwMaxCount)
{
	//获取名字
	CHAR szHostName[64];
	gethostname(szHostName,sizeof(szHostName));
	HOSTENT * pHostent=gethostbyname(szHostName);

	//变量定义
	DWORD dwIPCount=0;
	LPIN_ADDR * lpAddrList=(LPIN_ADDR *)pHostent->h_addr_list;

	//设置地址
	while(*lpAddrList!=NULL)
	{
		//溢出判断
		if(dwIPCount>=dwMaxCount)
		{
			ASSERT(FALSE);
			break;
		}

		//地址转换
		dwAddrList[dwIPCount++]=(*lpAddrList++)->S_un.S_addr;
	}

	return dwIPCount;
}

//枚举代理
bool CWHNetwork::EnumProxyServerType(WORD wIndex, tagProxyDescribe & ProxyDescribe)
{
	//信息填充
	switch(wIndex)
	{
	case PROXY_NONE:			//不使用代理
		{
			ProxyDescribe.cbProxyType=PROXY_NONE;
			lstrcpyn(ProxyDescribe.szDescribe,TEXT("不使用代理"),CountArray(ProxyDescribe.szDescribe));
			return true;
		}
	case PROXY_HTTP:			//HTTP 代理
		{
			ProxyDescribe.cbProxyType=PROXY_HTTP;
			lstrcpyn(ProxyDescribe.szDescribe,TEXT("HTTP 代理"),CountArray(ProxyDescribe.szDescribe));
			return true;
		}
	case PROXY_SOCKS4:			//SOCKS4 代理
		{
			ProxyDescribe.cbProxyType=PROXY_SOCKS4;
			lstrcpyn(ProxyDescribe.szDescribe,TEXT("SOCKS4 代理"),CountArray(ProxyDescribe.szDescribe));
			return true;
		}
	case PROXY_SOCKS5:			//SOCKS5 代理
		{
			ProxyDescribe.cbProxyType=PROXY_SOCKS5;
			lstrcpyn(ProxyDescribe.szDescribe,TEXT("SOCKS5 代理"),CountArray(ProxyDescribe.szDescribe));
			return true;
		}
	}

	return false;
}

//代理测试
BYTE CWHNetwork::ProxyServerTesting(BYTE cbProxyType, const tagProxyServer & ProxyServer)
{
	//校验参数
	ASSERT(cbProxyType!=PROXY_NONE);
	if(cbProxyType==PROXY_NONE) return CONNECT_EXCEPTION;

	//代理设置
	CTCPSocket TCPSocket;
	if(TCPSocket.SetProxyServerInfo(cbProxyType,ProxyServer)==false) return CONNECT_EXCEPTION;

	//代理测试
	return TCPSocket.ProxyServerTesting();
}

//////////////////////////////////////////////////////////////////////////

//输出管理
CWHNetwork g_WHNetwork;

//组件创建函数
extern "C" __declspec(dllexport) VOID * CreateWHNetwork(const GUID & Guid, DWORD dwInterfaceVer)
{
	return g_WHNetwork.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////