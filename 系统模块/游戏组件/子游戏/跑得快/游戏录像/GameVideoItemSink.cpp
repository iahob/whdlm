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

	m_cbPlayerCount = 0;
}

CGameVideoItemSink::~CGameVideoItemSink( void )
{

}

bool __cdecl CGameVideoItemSink::StartVideo(ITableFrame	*pTableFrame, BYTE cbPlayerCount)
{
	try
	{
		ResetVideoItem();

		m_pITableFrame = pTableFrame;
		m_cbPlayerCount = cbPlayerCount;

		m_iCurPos	= 0;
		m_iBufferSize  = ADD_VIDEO_BUF;

		//�����ڴ�
		m_pVideoDataBuffer =new BYTE [m_iBufferSize];
		if (m_pVideoDataBuffer==NULL) return false;	
		memset(m_pVideoDataBuffer,0,m_iBufferSize);
	}
	catch (...) 
	{
		ASSERT (FALSE);
		return false; 
	}
	
	return true;
}

bool __cdecl CGameVideoItemSink::StopAndSaveVideo(WORD wServerID,WORD wTableID){
	//���浽���ݿ�
	if(m_pITableFrame == NULL) return false;
	//�����ʽ���ַ���+Ψһ��ʶID (ID��������������ʱ����+����ID+����ID) 	
	CHAR   szVideoNumber[22];
	szVideoNumber[0] = 0;
	BuildVideoNumber(szVideoNumber,22,wServerID,wTableID);
	
	bool bAllAndroid = true;
	for (WORD i = 0; i<m_cbPlayerCount; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem && pServerUserItem->IsAndroidUser() == false)
		{
			bAllAndroid = false;
		}
	}
	if(bAllAndroid) return TRUE;

	for (WORD i = 0; i<m_cbPlayerCount; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem)
		{
			m_pITableFrame->WriteTableVideoPlayer(pServerUserItem->GetUserID(),szVideoNumber);
		}
	}
	m_pITableFrame->WriteTableVideoData(szVideoNumber,wServerID,wTableID,m_pVideoDataBuffer,(WORD)m_iCurPos);
	
	int iLength ;  
	TCHAR   szVideoID[50];
	iLength = MultiByteToWideChar (CP_ACP, 0, szVideoNumber, strlen (szVideoNumber) + 1, NULL, 0) ;  
	MultiByteToWideChar (CP_ACP, 0, szVideoNumber, strlen (szVideoNumber) + 1, szVideoID, iLength) ;  

	TCHAR szFilePath[MAX_PATH];
	_sntprintf_s(szFilePath,CountArray(szFilePath),TEXT("%s.Video"),szVideoID);

	ResetVideoItem();

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, Video_GameStart *pVideoGameStart, bool bFirst)
{
	//һ��Ҫ��˳��д
	if(bFirst) WriteUint16(wMsgKind);

	WriteUint64(pVideoGameStart->lBaseScore);
	WriteUint8(pVideoGameStart->cbPlayMode);
	WriteUint16(pVideoGameStart->wPlayerCount);
	WriteUint16(pVideoGameStart->wGamePlayerCountRule);

	WriteUint16(pVideoGameStart->wBankerUser);
	WriteUint16(pVideoGameStart->wBeforWiner);
	WriteUint16(pVideoGameStart->wCurrentUser);
	
	Write(pVideoGameStart->cbCardData, sizeof(BYTE)*NORMAL_CARD_COUNT);
	WriteUint8(pVideoGameStart->bSixTheenCard);
	WriteUint8(pVideoGameStart->bNoForceClose);
	WriteUint8(pVideoGameStart->bNoRemoveBomb);
	WriteUint8(pVideoGameStart->bSpadeBanker);

	WriteUint8(pVideoGameStart->cbSpadeBankerFirstOutCardData);
	WriteUint8(pVideoGameStart->cbWinBankerFirstOutCardData);

	WriteUint8(pVideoGameStart->bFirstOutSpade);

	WriteUint8(pVideoGameStart->bForceOutCard);
	WriteUint8(pVideoGameStart->bEchelonConclude);

	Write(pVideoGameStart->szNickName, sizeof(TCHAR)*LEN_NICKNAME);
	WriteUint16(pVideoGameStart->wChairID);
	WriteUint64(pVideoGameStart->lScore);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_OutCard *pVideoOutCard)
{
	//һ��Ҫ��˳��д
	WriteUint16(wMsgKind);
	WriteUint8(pVideoOutCard->bCardCount); 
	WriteUint16(pVideoOutCard->wCurrentUser);
	WriteUint16(pVideoOutCard->wOutCardUser);
	Write((void*)(&pVideoOutCard->remainCardCount), sizeof(REMAINCARDCOUNT));
	Write(pVideoOutCard->bCardData, sizeof(BYTE)*NORMAL_CARD_COUNT);
	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_PassCard *pVideoPassCard)
{
	//һ��Ҫ��˳��д
	WriteUint16(wMsgKind);
	WriteUint8(pVideoPassCard->bNewTurn);
	WriteUint16(pVideoPassCard->wPassUser);
	WriteUint16(pVideoPassCard->wCurrentUser);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_GameEnd *pVideoGameEnd)
{
	//һ��Ҫ��˳��д
	WriteUint16(wMsgKind);
	WriteUint64(pVideoGameEnd->lGameTax);
	Write(pVideoGameEnd->lGameScore, sizeof(SCORE)*GAME_PLAYER);
	Write(pVideoGameEnd->bCardCount, sizeof(BYTE)*GAME_PLAYER);
	Write(pVideoGameEnd->bCardData, sizeof(BYTE)*GAME_PLAYER*NORMAL_CARD_COUNT);
	Write(pVideoGameEnd->bBombCount, sizeof(BYTE)*GAME_PLAYER);
	Write(pVideoGameEnd->lBombScore, sizeof(LONGLONG)*GAME_PLAYER);
	Write((void*)(&pVideoGameEnd->remainCardCount), sizeof(REMAINCARDCOUNT));

	return true;
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
}

//¼����
VOID CGameVideoItemSink::BuildVideoNumber(CHAR szVideoNumber[], WORD wLen,WORD wServerID,WORD wTableID)
{
	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//��ʽ�����
	_snprintf_s(szVideoNumber, wLen, wLen,"%04d%02d%02d%02d%02d%02d%03d%03d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
		SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond,wServerID,wTableID);
}