#include "StdAfx.h"
#include "GameVideoItemSink.h"

#define  ADD_VIDEO_BUF  4096

CGameVideoItemSink::CGameVideoItemSink(void)
{
	//���ñ���
	m_iCurPos	= 0;	
	m_iBufferSize	= 0;
	m_pVideoDataBuffer = NULL;
	m_pITableFrame = NULL;
}

CGameVideoItemSink::~CGameVideoItemSink( void )
{

}

bool __cdecl CGameVideoItemSink::StartVideo(ITableFrame	*pTableFrame)
{
	try
	{
		ResetVideoItem();

		m_pITableFrame = pTableFrame;

		m_iCurPos	= 0;
		m_iBufferSize  = ADD_VIDEO_BUF;

		//�����ڴ�
		m_pVideoDataBuffer =new BYTE [m_iBufferSize];
		if (m_pVideoDataBuffer==NULL) return false;	
		memset(m_pVideoDataBuffer,0,m_iBufferSize);

		BuildVideoNumber(pTableFrame->GetGameServiceOption()->wServerID, pTableFrame->GetTableID());
	}
	catch (...) 
	{
		ASSERT (FALSE);
		return false; 
	}
	
	return true;
}

bool __cdecl CGameVideoItemSink::StopAndSaveVideo(WORD wServerID,WORD wTableID)
{
	//���浽���ݿ�
	if(m_pITableFrame == NULL) return false;
	
	bool bAllAndroid = true;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem && pServerUserItem->IsAndroidUser() == false)
		{
			bAllAndroid = false;
		}
	}
	if(bAllAndroid) return TRUE;

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem)
		{
			m_pITableFrame->WriteTableVideoPlayer(pServerUserItem->GetUserID(), m_szVideoNumber);
		}
	}
	m_pITableFrame->WriteTableVideoData(m_szVideoNumber, wServerID, wTableID, m_pVideoDataBuffer, (WORD)m_iCurPos);
	

	ResetVideoItem();

	return TRUE;
}


bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,void *pPack,int nSize,bool bFirst)
{
	//һ��Ҫ��˳��д
	if (bFirst)
		WriteUint16(wMsgKind);

	Write(pPack,nSize);

	return TRUE;
}

CHAR* __cdecl CGameVideoItemSink::GetVideoNumber()
{
	return m_szVideoNumber;
}

size_t	CGameVideoItemSink::Write(const void* data, size_t size)
{   
	if(size + m_iCurPos > m_iBufferSize)
	{
		if(RectifyBuffer(ADD_VIDEO_BUF/2)!=TRUE) return 0;
	}
	
	CopyMemory(m_pVideoDataBuffer+m_iCurPos,data,size);				
	m_iCurPos += size;

	return size;
}

//��������
bool CGameVideoItemSink::RectifyBuffer(size_t iSize)
{
	try
	{		
		size_t iNewbufSize =  iSize+m_iBufferSize;
		//�����ڴ�
		BYTE * pNewVideoBuffer=new BYTE [iNewbufSize];
		if (pNewVideoBuffer==NULL) return false;
		memset(pNewVideoBuffer,0,iNewbufSize);

		//��������
		if (m_pVideoDataBuffer!=NULL) 
		{
			CopyMemory(pNewVideoBuffer,m_pVideoDataBuffer,m_iCurPos);				
		}

		//���ñ���			
		m_iBufferSize=iNewbufSize;
		SafeDeleteArray(m_pVideoDataBuffer);
		m_pVideoDataBuffer=pNewVideoBuffer;
	}
	catch (...) 
	{
		ASSERT (FALSE);
		return false; 
	}

	return true;
}

//��������
void CGameVideoItemSink::ResetVideoItem()
{
	//���ñ���
	m_iCurPos	= 0;	
	m_iBufferSize  = 0;
	SafeDeleteArray(m_pVideoDataBuffer);
	ZeroMemory(m_szVideoNumber, sizeof(m_szVideoNumber));
}

//¼����
VOID CGameVideoItemSink::BuildVideoNumber(WORD wServerID,WORD wTableID)
{
	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//��ʽ�����
	_snprintf_s(m_szVideoNumber, sizeof(m_szVideoNumber), sizeof(m_szVideoNumber), "%04d%02d%02d%02d%02d%02d%03d%03d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
		SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond,wServerID,wTableID);
}