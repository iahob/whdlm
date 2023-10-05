//����ӣ���ݡ��Ŀ�,Ⱥ:16886186����ϵQQ:412069412
#ifndef SOCKET_DATA_HEAD_FILE
#define SOCKET_DATA_HEAD_FILE

#pragma once

#include "WHSocketHead.h"

//////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagTCPData 
{
	//������Ϣ
	TCP_Command					Command;							//��������

	//������Ϣ
	WORD						wDataSize;							//���ݴ�С
	VOID *						pDataBuffer;						//����ָ��
};

//������Ϣ
struct tagUDPData 
{
	//������Ϣ
	UDP_Command					Command;							//��������

	//������Ϣ
	WORD						wDataSize;							//���ݴ�С
	VOID *						pDataBuffer;						//����ָ��
};

//////////////////////////////////////////////////////////////////////////

//���ݴ���
class CTCPSocketData
{
	//��������
public:
	WORD						m_wBufferSize;						//���ݳ���
	BYTE						m_cbDataBuffer[SOCKET_TCP_BUFFER];	//���ݻ���

	//��������
public:
	//���캯��
	CTCPSocketData();
	//��������
	virtual ~CTCPSocketData();

	//��������
public:
	//��������
	bool SetBufferData(LPBYTE pcbBuffer, WORD wBufferSize, tagTCPData & TCPData);
	//��������
	bool InitSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//ӳ������
private:
	//ӳ������
	bool MappedBuffer();
	//ӳ������
	bool UnMappedBuffer();

	//���ܺ���
private:
	//��������
	bool EncryptBuffer();
	//��������
	bool CrevasseBuffer();

	//ѹ������
private:
	//ѹ������
	bool CompressBuffer();
	//��ѹ����
	bool UnCompressBuffer();
};

//////////////////////////////////////////////////////////////////////////

//���ݴ���
class CUDPSocketData
{
	//��������
public:
	WORD						m_wBufferSize;						//���ݳ���
	BYTE						m_cbDataBuffer[SOCKET_UDP_BUFFER];	//���ݻ���

	//��������
public:
	//���캯��
	CUDPSocketData();
	//��������
	virtual ~CUDPSocketData();

	//��������
public:
	//��������
	bool SetBufferData(LPBYTE pcbBuffer, WORD wBufferSize, tagUDPData & UDPData);
	//��������
	bool InitSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//���ܺ���
private:
	//��������
	bool EncryptBuffer();
	//��������
	bool CrevasseBuffer();

	//ѹ������
private:
	//ѹ������
	bool CompressBuffer();
	//��ѹ����
	bool UnCompressBuffer();	
};

//////////////////////////////////////////////////////////////////////////

#endif