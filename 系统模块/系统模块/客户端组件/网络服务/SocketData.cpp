//����ӣ���ݡ��Ŀ�,Ⱥ:16886186����ϵQQ:412069412
#include "Stdafx.h"
#include "SocketData.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CTCPSocketData::CTCPSocketData()
{
	//���ñ���
	m_wBufferSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	return;
}

//��������
CTCPSocketData::~CTCPSocketData()
{
}

//��������
bool CTCPSocketData::SetBufferData(LPBYTE pcbBuffer, WORD wBufferSize, tagTCPData & TCPData)
{
	//����У��
	ASSERT(wBufferSize>sizeof(TCP_Info)&&(wBufferSize<=SOCKET_TCP_BUFFER));
	if(wBufferSize<=sizeof(TCP_Info)||(wBufferSize>SOCKET_TCP_BUFFER)) return false;

	//��������
	m_wBufferSize=wBufferSize;
	CopyMemory(m_cbDataBuffer,pcbBuffer,wBufferSize);

	//У������
	ASSERT(((TCP_Info *)m_cbDataBuffer)->wPacketSize==wBufferSize);
	if(((TCP_Info *)m_cbDataBuffer)->wPacketSize!=wBufferSize) return false;

	//��ѹ����
	//if (UnCompressBuffer()==false)
	//{
	//	ASSERT(FALSE);
	//	return false;
	//}

	//��������
	//if (CrevasseBuffer()==false)
	//{
	//	ASSERT(FALSE);
	//	return false;
	//}

	//ӳ������
	if (UnMappedBuffer()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	TCP_Head * pHead=(TCP_Head *)m_cbDataBuffer;

	//��������
	TCPData.Command=pHead->CommandInfo;
	TCPData.wDataSize=m_wBufferSize-sizeof(TCP_Head);
	TCPData.pDataBuffer=m_cbDataBuffer+sizeof(TCP_Head);

	return true;
}

//��������
bool CTCPSocketData::InitSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//У���С
	ASSERT(wDataSize<=SOCKET_TCP_BUFFER);
	if(wDataSize>SOCKET_TCP_BUFFER) return false;

	//��������
	TCP_Head * pHead=(TCP_Head *)m_cbDataBuffer;

	//��������
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;

	//���ð�ͷ
	pHead->TCPInfo.cbDataKind=0;
	pHead->TCPInfo.cbCheckCode=0;
	pHead->TCPInfo.wPacketSize=sizeof(TCP_Head)+wDataSize;

	//��������
	if (wDataSize>0)
	{
		m_wBufferSize=sizeof(TCP_Head)+wDataSize;
		CopyMemory(m_cbDataBuffer+sizeof(TCP_Head),pData,wDataSize);
	}
	else
	{
		m_wBufferSize=sizeof(TCP_Head);
	}

	//ӳ������
	if (MappedBuffer()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	//if (EncryptBuffer()==false)
	//{
	//	ASSERT(FALSE);
	//	return false;
	//}

	//ѹ������
	//if (CompressBuffer()==false)
	//{
	//	ASSERT(FALSE);
	//	return false;
	//}

	return true;
}

//ӳ������
bool CTCPSocketData::MappedBuffer()
{
	//��������
	BYTE cbCheckCode=0;
	TCP_Info * pInfo=(TCP_Info *)m_cbDataBuffer;

	//�ֽ�ӳ��
	for (WORD i=sizeof(TCP_Info);i<m_wBufferSize;i++)
	{
		cbCheckCode+=m_cbDataBuffer[i];
		m_cbDataBuffer[i]=g_SendByteMap[m_cbDataBuffer[i]];
	}

	//���ð�ͷ
	pInfo->cbDataKind|=DK_MAPPED;
	pInfo->cbCheckCode=~cbCheckCode+1;

	return true;
}

//ӳ������
bool CTCPSocketData::UnMappedBuffer()
{
	//У�����
	ASSERT(m_wBufferSize>=sizeof(TCP_Head));
	ASSERT(((TCP_Head *)m_cbDataBuffer)->TCPInfo.wPacketSize==m_wBufferSize);

	//У������
	if (m_wBufferSize<sizeof(TCP_Head)) return false;
	if(((TCP_Head *)m_cbDataBuffer)->TCPInfo.wPacketSize!=m_wBufferSize) return false;

	//��������
	TCP_Info * pInfo=(TCP_Info *)m_cbDataBuffer;

	//ӳ������
	if ((pInfo->cbDataKind&DK_MAPPED)!=0)
	{
		//��������
		BYTE cbCheckCode=pInfo->cbCheckCode;

		//У���ֽ�
		for(INT i=sizeof(TCP_Info);i<m_wBufferSize;i++)
		{
			cbCheckCode+=g_RecvByteMap[m_cbDataBuffer[i]];
			m_cbDataBuffer[i]=g_RecvByteMap[m_cbDataBuffer[i]];
		}

		//����ж�
		ASSERT(cbCheckCode==0);
		if(cbCheckCode!=0) return false;
	}

	return true;
}

//��������
bool CTCPSocketData::EncryptBuffer()
{
	return true;
}

//��������
bool CTCPSocketData::CrevasseBuffer()
{
	return true;
}

//ѹ������
bool CTCPSocketData::CompressBuffer()
{
	//У�����
	ASSERT(m_wBufferSize>=sizeof(TCP_Head));
	if(m_wBufferSize<sizeof(TCP_Head)) return false;

	//��ʼ����
	WORD wSourceSize=m_wBufferSize-sizeof(TCP_Info);
	LPBYTE pcbSourceData=m_cbDataBuffer+sizeof(TCP_Info);

	//ѹ������
	BYTE cbResultData[SOCKET_TCP_BUFFER];
	ULONG lResultSize=CWHService::CompressData(pcbSourceData,wSourceSize,cbResultData,sizeof(cbResultData));

	//�������
	if ((lResultSize>0L)&&(lResultSize<wSourceSize))
	{
		//��������
		TCP_Info * pInfo=(TCP_Info *)m_cbDataBuffer;

		//���ð�ͷ
		pInfo->cbDataKind|=DK_COMPRESS;
		pInfo->wPacketSize=(WORD)(lResultSize)+sizeof(TCP_Info);

		//��������
		m_wBufferSize=pInfo->wPacketSize;
		CopyMemory(m_cbDataBuffer+sizeof(TCP_Info),cbResultData,lResultSize);
	}

	return true;
}

//��ѹ����
bool CTCPSocketData::UnCompressBuffer()
{
	//У�����
	ASSERT(m_wBufferSize>=sizeof(TCP_Info));
	ASSERT(((TCP_Info *)m_cbDataBuffer)->wPacketSize==m_wBufferSize);

	//��������
	TCP_Info * pInfo=(TCP_Info*)m_cbDataBuffer;

	//��ѹ�ж�
	if ((pInfo->cbDataKind&DK_COMPRESS)!=0)
	{
		//��ʼ����
		WORD wSourceSize=m_wBufferSize-sizeof(TCP_Info);
		LPBYTE pcbSourceData=m_cbDataBuffer+sizeof(TCP_Info);

		//��ѹ����
		BYTE cbResultData[SOCKET_TCP_BUFFER];
		ULONG lResultSize=CWHService::UnCompressData(pcbSourceData,wSourceSize,cbResultData,sizeof(cbResultData));

		//У����
		ASSERT((lResultSize>wSourceSize)&&((lResultSize+sizeof(TCP_Info))<=SOCKET_TCP_BUFFER));
		if((lResultSize<=wSourceSize)||((lResultSize+sizeof(TCP_Info))>SOCKET_TCP_BUFFER)) return false;

		//���ð�ͷ
		pInfo->cbDataKind&=~DK_COMPRESS;
		pInfo->wPacketSize=(WORD)(lResultSize)+sizeof(TCP_Info);

		//��������
		m_wBufferSize=pInfo->wPacketSize;
		CopyMemory(m_cbDataBuffer+sizeof(TCP_Info),cbResultData,lResultSize);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CUDPSocketData::CUDPSocketData()
{
	//���ñ���
	m_wBufferSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	return;
}

//��������
CUDPSocketData::~CUDPSocketData()
{
}

//��������
bool CUDPSocketData::SetBufferData(LPBYTE pcbBuffer, WORD wBufferSize, tagUDPData & UDPData)
{
	//����У��
	ASSERT(wBufferSize>sizeof(UDP_Info)&&(wBufferSize<=SOCKET_UDP_BUFFER));
	if(wBufferSize<=sizeof(UDP_Info)||(wBufferSize>SOCKET_UDP_BUFFER)) return false;

	//��������
	m_wBufferSize=wBufferSize;
	CopyMemory(m_cbDataBuffer,pcbBuffer,wBufferSize);

	//У������
	ASSERT(((UDP_Info *)m_cbDataBuffer)->wPacketSize==wBufferSize);
	if(((UDP_Info *)m_cbDataBuffer)->wPacketSize!=wBufferSize) return false;

	//��ѹ����
	if ((UnCompressBuffer()==false)||(CrevasseBuffer()==false))	return false;

	//��������
	UDP_Head * pHead=(UDP_Head *)m_cbDataBuffer;

	//��������
	UDPData.Command=pHead->CommandInfo;
	UDPData.wDataSize=m_wBufferSize-sizeof(UDP_Head);
	UDPData.pDataBuffer=m_cbDataBuffer+sizeof(UDP_Head);

	return true;
}

//��������
bool CUDPSocketData::InitSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//У���С
	ASSERT(wDataSize<=SOCKET_UDP_BUFFER);
	if(wDataSize>SOCKET_UDP_BUFFER) return false;

	//��������
	UDP_Head * pHead=(UDP_Head *)m_cbDataBuffer;

	//��������
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;

	//���ð�ͷ
	pHead->UDPInfo.cbDataKind=0;
	pHead->UDPInfo.cbCheckCode=0;
	pHead->UDPInfo.wPacketSize=sizeof(UDP_Head)+wDataSize;

	//��������
	if (wDataSize>0)
	{
		m_wBufferSize=sizeof(UDP_Head)+wDataSize;
		CopyMemory(m_cbDataBuffer+sizeof(UDP_Head),pData,wDataSize);
	}
	else
	{
		m_wBufferSize=sizeof(UDP_Head);
	}

	//����ѹ��
	if ((EncryptBuffer()==false)||(CompressBuffer()==false)) return false;

	return true;
}

//��������
bool CUDPSocketData::EncryptBuffer()
{
	//��������
	BYTE cbCheckCode=0;
	UDP_Info * pInfo=(UDP_Info *)m_cbDataBuffer;

	//�ֽ�ӳ��
	for(WORD i=sizeof(UDP_Info); i<m_wBufferSize; i++)
	{
		cbCheckCode+=m_cbDataBuffer[i];
		m_cbDataBuffer[i]=g_SendByteMap[m_cbDataBuffer[i]];
	}

	//���ð�ͷ
	pInfo->cbDataKind|=DK_ENCRYPT;
	pInfo->cbCheckCode=~cbCheckCode+1;

	return true;
}

//��������
bool CUDPSocketData::CrevasseBuffer()
{
	//У�����
	ASSERT(m_wBufferSize>=sizeof(UDP_Head));
	ASSERT(((UDP_Head *)m_cbDataBuffer)->UDPInfo.wPacketSize==m_wBufferSize);

	//У�����
	if(m_wBufferSize<sizeof(UDP_Head)) return false;
	if(((UDP_Head *)m_cbDataBuffer)->UDPInfo.wPacketSize!=m_wBufferSize) return false;

	//��������
	UDP_Info * pInfo=(UDP_Info *)m_cbDataBuffer;
	BYTE cbCheckCode=pInfo->cbCheckCode;

	//У���ֽ�
	for (INT i=sizeof(UDP_Info); i<m_wBufferSize; i++)
	{
		cbCheckCode+=g_RecvByteMap[m_cbDataBuffer[i]];
		m_cbDataBuffer[i]=g_RecvByteMap[m_cbDataBuffer[i]];
	}

	//����ж�
	ASSERT(cbCheckCode==0);
	if(cbCheckCode!=0) return false;

	return true;
}

//ѹ������
bool CUDPSocketData::CompressBuffer()
{
	//У�����
	ASSERT(m_wBufferSize>=sizeof(UDP_Head));
	if(m_wBufferSize<sizeof(UDP_Head)) return false;

	//��ʼ����
	WORD wSourceSize=m_wBufferSize-sizeof(UDP_Info);
	LPBYTE pcbSourceData=m_cbDataBuffer+sizeof(UDP_Info);

	//ѹ������
	BYTE cbResultData[SOCKET_UDP_BUFFER];
	ULONG lResultSize=CWHService::CompressData(pcbSourceData,wSourceSize,cbResultData,sizeof(cbResultData));

	//�������
	if ((lResultSize>0L)&&(lResultSize<wSourceSize))
	{
		//��������
		UDP_Info * pInfo=(UDP_Info *)m_cbDataBuffer;

		//���ð�ͷ
		pInfo->cbDataKind|=DK_COMPRESS;
		pInfo->wPacketSize=(WORD)(lResultSize)+sizeof(UDP_Info);

		//��������
		m_wBufferSize=pInfo->wPacketSize;
		CopyMemory(m_cbDataBuffer+sizeof(UDP_Info),cbResultData,lResultSize);
	}

	return true;
}

//��ѹ����
bool CUDPSocketData::UnCompressBuffer()
{
	//У�����
	ASSERT(m_wBufferSize>=sizeof(UDP_Info));
	ASSERT(((UDP_Info *)m_cbDataBuffer)->wPacketSize==m_wBufferSize);

	//��������
	UDP_Info * pInfo=(UDP_Info*)m_cbDataBuffer;

	//��ѹ�ж�
	if ((pInfo->cbDataKind&DK_COMPRESS)!=0)
	{
		//��ʼ����
		WORD wSourceSize=m_wBufferSize-sizeof(UDP_Info);
		LPBYTE pcbSourceData=m_cbDataBuffer+sizeof(UDP_Info);

		//��ѹ����
		BYTE cbResultData[SOCKET_UDP_BUFFER];
		ULONG lResultSize=CWHService::UnCompressData(pcbSourceData,wSourceSize,cbResultData,sizeof(cbResultData));

		//У����
		ASSERT((lResultSize>wSourceSize)&&((lResultSize+sizeof(UDP_Info))<=SOCKET_UDP_BUFFER));
		if((lResultSize<=wSourceSize)||((lResultSize+sizeof(UDP_Info))>SOCKET_UDP_BUFFER)) return false;

		//���ð�ͷ
		pInfo->cbDataKind&=~DK_COMPRESS;
		pInfo->wPacketSize=(WORD)(lResultSize)+sizeof(UDP_Info);

		//��������
		m_wBufferSize=pInfo->wPacketSize;
		CopyMemory(m_cbDataBuffer+sizeof(TCP_Info),cbResultData,lResultSize);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////