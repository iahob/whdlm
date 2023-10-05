//����ӣ���ݡ��Ŀ�,Ⱥ:16886186����ϵQQ:412069412
#ifndef NETWORK_INFO_HEAD_FILE
#define NETWORK_INFO_HEAD_FILE

#pragma once

#include "WHSocketHead.h"

//////////////////////////////////////////////////////////////////////////

//�������
class CWHNetwork : public IWHNetwork
{
	//��������
public:
	//���캯��
	CWHNetwork();
	//��������
	virtual ~CWHNetwork();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ܺ���
public:
	//��ַת��
	virtual DWORD ConversionAddr(LPCTSTR pszAddress);
	//��ȡ��ַ
	virtual DWORD GetLocalNetworkIP(DWORD dwAddrList[], DWORD dwMaxCount);

	//����ӿ�
public:
	//ö�ٴ���
	virtual bool EnumProxyServerType(WORD wIndex, tagProxyDescribe & ProxyDescribe);
	//�������
	virtual BYTE ProxyServerTesting(BYTE cbProxyType, const tagProxyServer & ProxyServer);
};

//////////////////////////////////////////////////////////////////////////

//�������
extern CWHNetwork g_WHNetwork;

//////////////////////////////////////////////////////////////////////////

#endif