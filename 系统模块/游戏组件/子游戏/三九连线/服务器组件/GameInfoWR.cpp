#include "StdAfx.h"
#include ".\gameinfowr.h"

#define  KEY_INFO_MAX			512

CGameInfoWR			CGameInfoWR::m_Object;

// 保存结构
struct KeyInfo
{
	WORD	wServerID;
	DWORD	dwUserID;
	BYTE	pData[KEY_INFO_MAX];

	KeyInfo()
	{
		wServerID = USHRT_MAX;
		dwUserID = ULONG_MAX;
		memset(pData, 0xff, sizeof(pData));
	}
};

CGameInfoWR::CGameInfoWR(void)
{

}

CGameInfoWR::~CGameInfoWR(void)
{

}

// 写信息
bool CGameInfoWR::WriteInfo( WORD wServerID, DWORD dwUserID, VOID * pData, WORD wDataSize )
{
	// 检测大小
	ASSERT( wDataSize <= KEY_INFO_MAX );
	if( wDataSize > KEY_INFO_MAX )
		return false;

	// 结构定义
	KeyInfo WriteKey;
	WriteKey.wServerID = wServerID;
	WriteKey.dwUserID = dwUserID;
	CopyMemory( WriteKey.pData, pData, wDataSize );

	// 打开文件
	BOOL bOpen = m_FileInfo.Open(TEXT("DuoBao.info"), CFile::modeNoTruncate|CFile::modeReadWrite);
	if ( !bOpen )
	{
		bOpen = m_FileInfo.Open(TEXT("DuoBao.info"), CFile::modeCreate|CFile::modeReadWrite);

		if ( !bOpen )
		{
			ASSERT(FALSE);
			return false;
		}
	}

	// 定义第一
	m_FileInfo.SeekToBegin();
	UINT nReadCount = 0;
	UINT lOff = 0;
	bool bFind = false;

	// 遍历查找
	do
	{
		KeyInfo ReadKey;
		nReadCount = m_FileInfo.Read( &ReadKey, sizeof(ReadKey) );
		
		if ( ReadKey.dwUserID == dwUserID && ReadKey.wServerID == wServerID )
		{
			bFind = true;
			m_FileInfo.Seek( lOff, CFile::begin );
			m_FileInfo.Write( &WriteKey, sizeof(WriteKey) );
		}

		lOff += nReadCount;
	}while( !bFind && nReadCount != 0 );

	if ( !bFind )
	{
		m_FileInfo.Write( &WriteKey, sizeof(WriteKey) );
	}

	// 关闭文件
	m_FileInfo.Flush();
	m_FileInfo.Close();

	return true;
}

// 读取信息
bool CGameInfoWR::ReadInfo( WORD wServerID, DWORD dwUserID, VOID * pData, WORD wDataSize )
{
	// 检测大小
	ASSERT( wDataSize <= KEY_INFO_MAX );
	if( wDataSize > KEY_INFO_MAX )
		return false;
	// 打开文件
	BOOL bOpen = m_FileInfo.Open(TEXT("DuoBao.info"), CFile::modeNoTruncate|CFile::modeReadWrite);
	if ( !bOpen )
	{
		bOpen = m_FileInfo.Open(TEXT("DuoBao.info"), CFile::modeCreate|CFile::modeReadWrite);

		if ( !bOpen )
		{
			ASSERT(FALSE);
			return false;
		}
	}

	// 定义第一
	m_FileInfo.SeekToBegin();
	UINT nReadCount = 0;
	UINT lOff = 0;
	bool bFind = false;

	// 遍历查找
	do
	{
		KeyInfo ReadKey;
		nReadCount = m_FileInfo.Read( &ReadKey, sizeof(ReadKey) );

		if ( ReadKey.dwUserID == dwUserID && ReadKey.wServerID == wServerID )
		{
			bFind = true;
			CopyMemory(pData, ReadKey.pData, wDataSize);

			// 关闭文件
			m_FileInfo.Flush();
			m_FileInfo.Close();

			return true;
		}

		lOff += nReadCount;
	}while( !bFind && nReadCount != 0 );

	// 关闭文件
	m_FileInfo.Flush();
	m_FileInfo.Close();

	return false;
}
