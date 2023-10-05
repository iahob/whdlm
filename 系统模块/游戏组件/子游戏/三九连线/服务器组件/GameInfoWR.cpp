#include "StdAfx.h"
#include ".\gameinfowr.h"

#define  KEY_INFO_MAX			512

CGameInfoWR			CGameInfoWR::m_Object;

// ����ṹ
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

// д��Ϣ
bool CGameInfoWR::WriteInfo( WORD wServerID, DWORD dwUserID, VOID * pData, WORD wDataSize )
{
	// ����С
	ASSERT( wDataSize <= KEY_INFO_MAX );
	if( wDataSize > KEY_INFO_MAX )
		return false;

	// �ṹ����
	KeyInfo WriteKey;
	WriteKey.wServerID = wServerID;
	WriteKey.dwUserID = dwUserID;
	CopyMemory( WriteKey.pData, pData, wDataSize );

	// ���ļ�
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

	// �����һ
	m_FileInfo.SeekToBegin();
	UINT nReadCount = 0;
	UINT lOff = 0;
	bool bFind = false;

	// ��������
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

	// �ر��ļ�
	m_FileInfo.Flush();
	m_FileInfo.Close();

	return true;
}

// ��ȡ��Ϣ
bool CGameInfoWR::ReadInfo( WORD wServerID, DWORD dwUserID, VOID * pData, WORD wDataSize )
{
	// ����С
	ASSERT( wDataSize <= KEY_INFO_MAX );
	if( wDataSize > KEY_INFO_MAX )
		return false;
	// ���ļ�
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

	// �����һ
	m_FileInfo.SeekToBegin();
	UINT nReadCount = 0;
	UINT lOff = 0;
	bool bFind = false;

	// ��������
	do
	{
		KeyInfo ReadKey;
		nReadCount = m_FileInfo.Read( &ReadKey, sizeof(ReadKey) );

		if ( ReadKey.dwUserID == dwUserID && ReadKey.wServerID == wServerID )
		{
			bFind = true;
			CopyMemory(pData, ReadKey.pData, wDataSize);

			// �ر��ļ�
			m_FileInfo.Flush();
			m_FileInfo.Close();

			return true;
		}

		lOff += nReadCount;
	}while( !bFind && nReadCount != 0 );

	// �ر��ļ�
	m_FileInfo.Flush();
	m_FileInfo.Close();

	return false;
}
