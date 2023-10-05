//分享樱花草、的酷,群:16886186，联系QQ:412069412
#include "Stdafx.h"
#include "SocketData.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPSocketData::CTCPSocketData()
{
	//设置变量
	m_wBufferSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	return;
}

//析构函数
CTCPSocketData::~CTCPSocketData()
{
}

//设置数据
bool CTCPSocketData::SetBufferData(LPBYTE pcbBuffer, WORD wBufferSize, tagTCPData & TCPData)
{
	//数据校验
	ASSERT(wBufferSize>sizeof(TCP_Info)&&(wBufferSize<=SOCKET_TCP_BUFFER));
	if(wBufferSize<=sizeof(TCP_Info)||(wBufferSize>SOCKET_TCP_BUFFER)) return false;

	//设置数据
	m_wBufferSize=wBufferSize;
	CopyMemory(m_cbDataBuffer,pcbBuffer,wBufferSize);

	//校验数据
	ASSERT(((TCP_Info *)m_cbDataBuffer)->wPacketSize==wBufferSize);
	if(((TCP_Info *)m_cbDataBuffer)->wPacketSize!=wBufferSize) return false;

	//解压数据
	//if (UnCompressBuffer()==false)
	//{
	//	ASSERT(FALSE);
	//	return false;
	//}

	//解密数据
	//if (CrevasseBuffer()==false)
	//{
	//	ASSERT(FALSE);
	//	return false;
	//}

	//映射数据
	if (UnMappedBuffer()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//变量定义
	TCP_Head * pHead=(TCP_Head *)m_cbDataBuffer;

	//设置数据
	TCPData.Command=pHead->CommandInfo;
	TCPData.wDataSize=m_wBufferSize-sizeof(TCP_Head);
	TCPData.pDataBuffer=m_cbDataBuffer+sizeof(TCP_Head);

	return true;
}

//设置数据
bool CTCPSocketData::InitSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//校验大小
	ASSERT(wDataSize<=SOCKET_TCP_BUFFER);
	if(wDataSize>SOCKET_TCP_BUFFER) return false;

	//变量定义
	TCP_Head * pHead=(TCP_Head *)m_cbDataBuffer;

	//设置命令
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;

	//设置包头
	pHead->TCPInfo.cbDataKind=0;
	pHead->TCPInfo.cbCheckCode=0;
	pHead->TCPInfo.wPacketSize=sizeof(TCP_Head)+wDataSize;

	//设置数据
	if (wDataSize>0)
	{
		m_wBufferSize=sizeof(TCP_Head)+wDataSize;
		CopyMemory(m_cbDataBuffer+sizeof(TCP_Head),pData,wDataSize);
	}
	else
	{
		m_wBufferSize=sizeof(TCP_Head);
	}

	//映射数据
	if (MappedBuffer()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//加密数据
	//if (EncryptBuffer()==false)
	//{
	//	ASSERT(FALSE);
	//	return false;
	//}

	//压缩数据
	//if (CompressBuffer()==false)
	//{
	//	ASSERT(FALSE);
	//	return false;
	//}

	return true;
}

//映射数据
bool CTCPSocketData::MappedBuffer()
{
	//变量定义
	BYTE cbCheckCode=0;
	TCP_Info * pInfo=(TCP_Info *)m_cbDataBuffer;

	//字节映射
	for (WORD i=sizeof(TCP_Info);i<m_wBufferSize;i++)
	{
		cbCheckCode+=m_cbDataBuffer[i];
		m_cbDataBuffer[i]=g_SendByteMap[m_cbDataBuffer[i]];
	}

	//设置包头
	pInfo->cbDataKind|=DK_MAPPED;
	pInfo->cbCheckCode=~cbCheckCode+1;

	return true;
}

//映射数据
bool CTCPSocketData::UnMappedBuffer()
{
	//校验参数
	ASSERT(m_wBufferSize>=sizeof(TCP_Head));
	ASSERT(((TCP_Head *)m_cbDataBuffer)->TCPInfo.wPacketSize==m_wBufferSize);

	//校验数据
	if (m_wBufferSize<sizeof(TCP_Head)) return false;
	if(((TCP_Head *)m_cbDataBuffer)->TCPInfo.wPacketSize!=m_wBufferSize) return false;

	//变量定义
	TCP_Info * pInfo=(TCP_Info *)m_cbDataBuffer;

	//映射数据
	if ((pInfo->cbDataKind&DK_MAPPED)!=0)
	{
		//变量定义
		BYTE cbCheckCode=pInfo->cbCheckCode;

		//校验字节
		for(INT i=sizeof(TCP_Info);i<m_wBufferSize;i++)
		{
			cbCheckCode+=g_RecvByteMap[m_cbDataBuffer[i]];
			m_cbDataBuffer[i]=g_RecvByteMap[m_cbDataBuffer[i]];
		}

		//结果判断
		ASSERT(cbCheckCode==0);
		if(cbCheckCode!=0) return false;
	}

	return true;
}

//加密数据
bool CTCPSocketData::EncryptBuffer()
{
	return true;
}

//解密数据
bool CTCPSocketData::CrevasseBuffer()
{
	return true;
}

//压缩数据
bool CTCPSocketData::CompressBuffer()
{
	//校验参数
	ASSERT(m_wBufferSize>=sizeof(TCP_Head));
	if(m_wBufferSize<sizeof(TCP_Head)) return false;

	//起始数据
	WORD wSourceSize=m_wBufferSize-sizeof(TCP_Info);
	LPBYTE pcbSourceData=m_cbDataBuffer+sizeof(TCP_Info);

	//压缩数据
	BYTE cbResultData[SOCKET_TCP_BUFFER];
	ULONG lResultSize=CWHService::CompressData(pcbSourceData,wSourceSize,cbResultData,sizeof(cbResultData));

	//结果处理
	if ((lResultSize>0L)&&(lResultSize<wSourceSize))
	{
		//变量定义
		TCP_Info * pInfo=(TCP_Info *)m_cbDataBuffer;

		//设置包头
		pInfo->cbDataKind|=DK_COMPRESS;
		pInfo->wPacketSize=(WORD)(lResultSize)+sizeof(TCP_Info);

		//设置数据
		m_wBufferSize=pInfo->wPacketSize;
		CopyMemory(m_cbDataBuffer+sizeof(TCP_Info),cbResultData,lResultSize);
	}

	return true;
}

//解压数据
bool CTCPSocketData::UnCompressBuffer()
{
	//校验参数
	ASSERT(m_wBufferSize>=sizeof(TCP_Info));
	ASSERT(((TCP_Info *)m_cbDataBuffer)->wPacketSize==m_wBufferSize);

	//变量定义
	TCP_Info * pInfo=(TCP_Info*)m_cbDataBuffer;

	//解压判断
	if ((pInfo->cbDataKind&DK_COMPRESS)!=0)
	{
		//起始数据
		WORD wSourceSize=m_wBufferSize-sizeof(TCP_Info);
		LPBYTE pcbSourceData=m_cbDataBuffer+sizeof(TCP_Info);

		//解压数据
		BYTE cbResultData[SOCKET_TCP_BUFFER];
		ULONG lResultSize=CWHService::UnCompressData(pcbSourceData,wSourceSize,cbResultData,sizeof(cbResultData));

		//校验结果
		ASSERT((lResultSize>wSourceSize)&&((lResultSize+sizeof(TCP_Info))<=SOCKET_TCP_BUFFER));
		if((lResultSize<=wSourceSize)||((lResultSize+sizeof(TCP_Info))>SOCKET_TCP_BUFFER)) return false;

		//设置包头
		pInfo->cbDataKind&=~DK_COMPRESS;
		pInfo->wPacketSize=(WORD)(lResultSize)+sizeof(TCP_Info);

		//设置数据
		m_wBufferSize=pInfo->wPacketSize;
		CopyMemory(m_cbDataBuffer+sizeof(TCP_Info),cbResultData,lResultSize);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CUDPSocketData::CUDPSocketData()
{
	//设置变量
	m_wBufferSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	return;
}

//析构函数
CUDPSocketData::~CUDPSocketData()
{
}

//设置数据
bool CUDPSocketData::SetBufferData(LPBYTE pcbBuffer, WORD wBufferSize, tagUDPData & UDPData)
{
	//数据校验
	ASSERT(wBufferSize>sizeof(UDP_Info)&&(wBufferSize<=SOCKET_UDP_BUFFER));
	if(wBufferSize<=sizeof(UDP_Info)||(wBufferSize>SOCKET_UDP_BUFFER)) return false;

	//设置数据
	m_wBufferSize=wBufferSize;
	CopyMemory(m_cbDataBuffer,pcbBuffer,wBufferSize);

	//校验数据
	ASSERT(((UDP_Info *)m_cbDataBuffer)->wPacketSize==wBufferSize);
	if(((UDP_Info *)m_cbDataBuffer)->wPacketSize!=wBufferSize) return false;

	//解压数据
	if ((UnCompressBuffer()==false)||(CrevasseBuffer()==false))	return false;

	//变量定义
	UDP_Head * pHead=(UDP_Head *)m_cbDataBuffer;

	//设置数据
	UDPData.Command=pHead->CommandInfo;
	UDPData.wDataSize=m_wBufferSize-sizeof(UDP_Head);
	UDPData.pDataBuffer=m_cbDataBuffer+sizeof(UDP_Head);

	return true;
}

//设置数据
bool CUDPSocketData::InitSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//校验大小
	ASSERT(wDataSize<=SOCKET_UDP_BUFFER);
	if(wDataSize>SOCKET_UDP_BUFFER) return false;

	//变量定义
	UDP_Head * pHead=(UDP_Head *)m_cbDataBuffer;

	//设置命令
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;

	//设置包头
	pHead->UDPInfo.cbDataKind=0;
	pHead->UDPInfo.cbCheckCode=0;
	pHead->UDPInfo.wPacketSize=sizeof(UDP_Head)+wDataSize;

	//设置数据
	if (wDataSize>0)
	{
		m_wBufferSize=sizeof(UDP_Head)+wDataSize;
		CopyMemory(m_cbDataBuffer+sizeof(UDP_Head),pData,wDataSize);
	}
	else
	{
		m_wBufferSize=sizeof(UDP_Head);
	}

	//加密压缩
	if ((EncryptBuffer()==false)||(CompressBuffer()==false)) return false;

	return true;
}

//加密数据
bool CUDPSocketData::EncryptBuffer()
{
	//变量定义
	BYTE cbCheckCode=0;
	UDP_Info * pInfo=(UDP_Info *)m_cbDataBuffer;

	//字节映射
	for(WORD i=sizeof(UDP_Info); i<m_wBufferSize; i++)
	{
		cbCheckCode+=m_cbDataBuffer[i];
		m_cbDataBuffer[i]=g_SendByteMap[m_cbDataBuffer[i]];
	}

	//设置包头
	pInfo->cbDataKind|=DK_ENCRYPT;
	pInfo->cbCheckCode=~cbCheckCode+1;

	return true;
}

//解密数据
bool CUDPSocketData::CrevasseBuffer()
{
	//校验参数
	ASSERT(m_wBufferSize>=sizeof(UDP_Head));
	ASSERT(((UDP_Head *)m_cbDataBuffer)->UDPInfo.wPacketSize==m_wBufferSize);

	//校验参数
	if(m_wBufferSize<sizeof(UDP_Head)) return false;
	if(((UDP_Head *)m_cbDataBuffer)->UDPInfo.wPacketSize!=m_wBufferSize) return false;

	//变量定义
	UDP_Info * pInfo=(UDP_Info *)m_cbDataBuffer;
	BYTE cbCheckCode=pInfo->cbCheckCode;

	//校验字节
	for (INT i=sizeof(UDP_Info); i<m_wBufferSize; i++)
	{
		cbCheckCode+=g_RecvByteMap[m_cbDataBuffer[i]];
		m_cbDataBuffer[i]=g_RecvByteMap[m_cbDataBuffer[i]];
	}

	//结果判断
	ASSERT(cbCheckCode==0);
	if(cbCheckCode!=0) return false;

	return true;
}

//压缩数据
bool CUDPSocketData::CompressBuffer()
{
	//校验参数
	ASSERT(m_wBufferSize>=sizeof(UDP_Head));
	if(m_wBufferSize<sizeof(UDP_Head)) return false;

	//起始数据
	WORD wSourceSize=m_wBufferSize-sizeof(UDP_Info);
	LPBYTE pcbSourceData=m_cbDataBuffer+sizeof(UDP_Info);

	//压缩数据
	BYTE cbResultData[SOCKET_UDP_BUFFER];
	ULONG lResultSize=CWHService::CompressData(pcbSourceData,wSourceSize,cbResultData,sizeof(cbResultData));

	//结果处理
	if ((lResultSize>0L)&&(lResultSize<wSourceSize))
	{
		//变量定义
		UDP_Info * pInfo=(UDP_Info *)m_cbDataBuffer;

		//设置包头
		pInfo->cbDataKind|=DK_COMPRESS;
		pInfo->wPacketSize=(WORD)(lResultSize)+sizeof(UDP_Info);

		//设置数据
		m_wBufferSize=pInfo->wPacketSize;
		CopyMemory(m_cbDataBuffer+sizeof(UDP_Info),cbResultData,lResultSize);
	}

	return true;
}

//解压数据
bool CUDPSocketData::UnCompressBuffer()
{
	//校验参数
	ASSERT(m_wBufferSize>=sizeof(UDP_Info));
	ASSERT(((UDP_Info *)m_cbDataBuffer)->wPacketSize==m_wBufferSize);

	//变量定义
	UDP_Info * pInfo=(UDP_Info*)m_cbDataBuffer;

	//解压判断
	if ((pInfo->cbDataKind&DK_COMPRESS)!=0)
	{
		//起始数据
		WORD wSourceSize=m_wBufferSize-sizeof(UDP_Info);
		LPBYTE pcbSourceData=m_cbDataBuffer+sizeof(UDP_Info);

		//解压数据
		BYTE cbResultData[SOCKET_UDP_BUFFER];
		ULONG lResultSize=CWHService::UnCompressData(pcbSourceData,wSourceSize,cbResultData,sizeof(cbResultData));

		//校验结果
		ASSERT((lResultSize>wSourceSize)&&((lResultSize+sizeof(UDP_Info))<=SOCKET_UDP_BUFFER));
		if((lResultSize<=wSourceSize)||((lResultSize+sizeof(UDP_Info))>SOCKET_UDP_BUFFER)) return false;

		//设置包头
		pInfo->cbDataKind&=~DK_COMPRESS;
		pInfo->wPacketSize=(WORD)(lResultSize)+sizeof(UDP_Info);

		//设置数据
		m_wBufferSize=pInfo->wPacketSize;
		CopyMemory(m_cbDataBuffer+sizeof(TCP_Info),cbResultData,lResultSize);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////