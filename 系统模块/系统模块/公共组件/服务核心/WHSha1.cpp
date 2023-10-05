//藏宝库IT社区（28xin.com）
#include "StdAfx.h"
#include "WHSha1.h"

/////////////////////////////////////////////////////////////////////////////

//类型定义
typedef unsigned __int64 uint64;

//常量定义
const UINT K[4] = {0x5A827999,0x6ED9EBA1,0x8F1BBCDC,0xCA62C1D6};


/////////////////////////////////////////////////////////////////////////////

//编码助手
class CWHShaAide
{
	//变量定义
protected:
	UINT			H[5];
	BYTE			m_dataBlock[64];
	int				m_dataIndex;	
	uint64			m_dataSizeInBits;

	//函数定义
public:
	//构造函数
	CWHShaAide();
	//析构函数
	virtual ~CWHShaAide();	

	//功能函数
public:
	//哈希数据
	int CWHShaAide::HashData(const void * pData, UINT nDataSize, UINT uMsgDigest[5]);

	//辅助函数
protected:
	//重置数据
	void ResetData();
	//数据处理
	void PadData();
	//处理数据
	void ProcessDataBlock();
	//函数定义
	UINT CircularLeftShift32(int bits, UINT data);
};

/////////////////////////////////////////////////////////////////////////////

//构造函数
CWHShaAide::CWHShaAide()
{
	ResetData();
}
CWHShaAide::~CWHShaAide()
{
}

//重置数据
void CWHShaAide::ResetData()
{
	//设置变量
	H[0] = 0x67452301;
	H[1] = 0xEFCDAB89;
	H[2] = 0x98BADCFE;
	H[3] = 0x10325476;
	H[4] = 0xC3D2E1F0;

	//设置变量
	m_dataIndex = 0;
	m_dataSizeInBits = 0;
}

//数据处理
void CWHShaAide::PadData()
{
	m_dataBlock[m_dataIndex++] = 0x80;
	if (m_dataIndex > 56)
	{
		while(m_dataIndex < 64)
		{
			m_dataBlock[m_dataIndex++] = 0;
		}

		ProcessDataBlock();
	}

	while(m_dataIndex < 56)
	{
		m_dataBlock[m_dataIndex++] = 0;
	}

	for (int i = 56; m_dataIndex < 64; i -= 8)
	{
		m_dataBlock[m_dataIndex++] = (m_dataSizeInBits >> i) & 0xFF;
	}

	ProcessDataBlock();
}

//处理数据
void CWHShaAide::ProcessDataBlock()
{
	UINT W[80];

	int t;
	UINT A, B, C, D, E, temp;

	for (t = 0; t < 16; t++)
	{
		W[t] = (m_dataBlock[t * 4] << 24) | (m_dataBlock[t * 4 + 1] << 16) 
			| (m_dataBlock[t * 4 + 2] << 8) | (m_dataBlock[t * 4 + 3]);
	}

	for ( ; t < 80; t++)
	{
		W[t] = CircularLeftShift32(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
	}

	A = H[0];
	B = H[1];
	C = H[2];
	D = H[3];
	E = H[4];

	for (t = 0; t < 20; t++)
	{
		temp = CircularLeftShift32(5, A) + ((B & C)|((~B) & D)) + E + W[t] + K[0]; 
		E = D;
		D = C; 
		C = CircularLeftShift32(30, B);
		B = A; 
		A = temp; 
	}

	for (; t < 40; t++)
	{
		temp = CircularLeftShift32(5, A) + (B ^ C ^ D) + E + W[t] + K[1]; 
		E = D;
		D = C; 
		C = CircularLeftShift32(30, B);
		B = A; 
		A = temp; 
	}

	for (; t < 60; t++)
	{
		temp = CircularLeftShift32(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2]; 
		E = D;
		D = C; 
		C = CircularLeftShift32(30, B);
		B = A; 
		A = temp; 
	}

	for (; t < 80; t++)
	{
		temp = CircularLeftShift32(5, A) + (B ^ C ^ D) + E + W[t] + K[3]; 
		E = D;
		D = C; 
		C = CircularLeftShift32(30, B);
		B = A; 
		A = temp; 
	}

	H[0] += A;
	H[1] += B;
	H[2] += C;
	H[3] += D;
	H[4] += E;

	m_dataIndex = 0;
}

//函数定义
UINT CWHShaAide::CircularLeftShift32(int bits, UINT data)
{
	return (data << bits) | (data >> (32 - bits));
}

//哈希数据
int CWHShaAide::HashData(const void * pData, UINT nDataSize, UINT uMsgDigest[5])
{
	//重置数据
	ResetData();

	if (uint64(-1) - m_dataSizeInBits < (uint64)nDataSize)
	{
		return SHA1_DATA_TOO_LONG;
	}

	BYTE* dataByte = (BYTE*)pData;

	while (nDataSize--)
	{
		m_dataBlock[m_dataIndex++] = *dataByte++;
		m_dataSizeInBits += 8;
		if (m_dataIndex == 64)
		{
			ProcessDataBlock();
		}
	}

	PadData();
	
	for (int i = 0; i < CountArray(H); i++)
	{
		uMsgDigest[i] = H[i];
	}

	return SHA1_DATA_PUSH_SUCCEED;
}

/////////////////////////////////////////////////////////////////////////////

//构造函数
CWHSha1::CWHSha1()
{
}

//析构函数
CWHSha1::~CWHSha1()
{
}

//哈希数据
int CWHSha1::HashData(const void * pData, UINT nDataSize, UINT uMsgDigest[5])
{
	//重置数据
	CWHShaAide ShaAide;
	return ShaAide.HashData(pData,nDataSize,uMsgDigest);	
}
/////////////////////////////////////////////////////////////////////////////