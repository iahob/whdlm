//����ӣ���ݡ��Ŀ�,Ⱥ:16886186����ϵQQ:412069412
#ifndef TCP_SOCKET_HEAD_FILE
#define TCP_SOCKET_HEAD_FILE

#pragma once

#include "SocketData.h"
#include "WHSocketHead.h"

//////////////////////////////////////////////////////////////////////////

//TCP ����
class CTCPSocket : public CWnd, public ITCPSocket
{
	//��������
protected:
	WORD							m_wSocketID;						//�����ʶ
	BYTE							m_cbSocketStatus;					//����״̬

	//�ں˱���
protected:
	SOCKET							m_hSocket;							//���Ӿ��
	CTCPSocketData					m_TCPSocketData;					//��������
	ITCPSocketSink	*				m_pITCPSocketSink;					//�ص�����

	//������Ϣ
protected:
	BYTE							m_cbProxyType;						//��������
	tagProxyServer					m_ProxyServer;						//������Ϣ

	//���ܱ���
protected:
	WORD							m_wBufferSize;						//���ܳ���
	BYTE							m_cbDataBuffer[SOCKET_TCP_BUFFER*8];//���ܻ���

	//�������
protected:
	bool							m_bNeedBuffer;						//����״̬
	DWORD							m_dwBufferData;						//��������
	DWORD							m_dwBufferSize;						//�����С
	LPBYTE							m_pcbDataBuffer;					//��������

	//��������
protected:
	DWORD							m_dwSendTickCount;					//����ʱ��
	DWORD							m_dwRecvTickCount;					//����ʱ��

	//��������
public:
	//���캯��
	CTCPSocket();
	//��������
	virtual ~CTCPSocket();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//���ñ�ʶ
	virtual VOID SetSocketID(WORD wSocketID);
	//���ýӿ�
	virtual bool SetTCPSocketSink(IUnknownEx * pIUnknownEx);

	//��Ϣ�ӿ�
public:
	//��ȡ״̬
	virtual	BYTE GetSocketStatus() { return m_cbSocketStatus; }
	//���ͼ��
	virtual DWORD GetLastSendTick() { return m_dwSendTickCount; }
	//���ܼ��
	virtual DWORD GetLastRecvTick() { return m_dwRecvTickCount; }

	//����ӿ�
public:
	//�������
	virtual BYTE ProxyServerTesting();
	//���ô���
	virtual bool SetProxyServerInfo(BYTE cbProxyType, const tagProxyServer & ProxyServer);

	//�����ӿ�
public:
	//�ر�����
	virtual VOID CloseSocket();
	//���Ӳ���
	virtual BYTE Connect(DWORD dwServerIP, WORD wPort);
	//���Ӳ���
	virtual BYTE Connect(LPCTSTR szServerIP, WORD wPort);
	//���ͺ���
	virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//��������
protected:
	//���Ӵ���
	BYTE ConnectProxyServer();
	
	//��������
protected:
	//�ر�����
	VOID CloseSocket(BYTE cbShutReason);
	//��������
	VOID AmortizeBuffer(VOID * pData, WORD wDataSize);
	//��������
	WORD SendDataBuffer(VOID * pBuffer, WORD wSendSize);

	//��Ϣӳ��
protected:
	//ʱ����Ϣ
	VOID OnTimer(UINT_PTR nIDEvent);
	//������Ϣ
	LRESULT OnSocketNotify(WPARAM wParam, LPARAM lParam);

	//��Ϣ����
protected:
	//�����ȡ
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
	//���緢��
	LRESULT OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam);
	//����ر�
	LRESULT OnSocketNotifyClose(WPARAM wParam, LPARAM lParam);
	//��������
	LRESULT OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////


#endif