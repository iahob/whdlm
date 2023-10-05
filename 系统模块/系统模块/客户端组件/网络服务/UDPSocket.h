//����ӣ���ݡ��Ŀ�,Ⱥ:16886186����ϵQQ:412069412
#ifndef UDP_SOCKET_HEAD_FILE
#define UDP_SOCKET_HEAD_FILE

#pragma once

#include "SocketData.h"
#include "WHSocketHead.h"

//////////////////////////////////////////////////////////////////////////

//UDP ����
class CUDPSocket : public CWnd, public IUDPSocket
{
	//��������
protected:
	WORD							m_wSocketID;						//�����ʶ
	BYTE							m_cbSocketStatus;					//����״̬

	//�ں˱���
protected:
	SOCKET							m_hSocket;							//���Ӿ��
	CUDPSocketData					m_UDPSocketData;					//��������
	IUDPSocketSink	*				m_pIUDPSocketSink;					//�ص�����

	//��������
protected:
	DWORD							m_dwSendTickCount;					//����ʱ��
	DWORD							m_dwRecvTickCount;					//����ʱ��

	//��������
public:
	//���캯��
	CUDPSocket();
	//��������
	virtual ~CUDPSocket();

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
	virtual bool SetUDPSocketSink(IUnknownEx * pIUnknownEx);

	//��Ϣ�ӿ�
public:
	//��ȡ״̬
	virtual	BYTE GetSocketStatus() { return m_cbSocketStatus; }
	//���ͼ��
	virtual DWORD GetLastSendTick() { return m_dwSendTickCount; }
	//���ܼ��
	virtual DWORD GetLastRecvTick() { return m_dwRecvTickCount; }

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
	//�ر�����
	VOID CloseSocket(BYTE cbShutReason);

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