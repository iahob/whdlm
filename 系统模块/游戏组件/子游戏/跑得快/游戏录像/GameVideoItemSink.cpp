#include "StdAfx.h"
#include "GameVideoItemSink.h"

#define  ADD_VIDEO_BUF  4096

CGameVideoItemSink::CGameVideoItemSink(void)
{
	//设置变量
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

		//申请内存
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
	//保存到数据库
	if(m_pITableFrame == NULL) return false;
	//保存格式：字符串+唯一标识ID (ID命名规则：年月日时分秒+房间ID+桌子ID) 	
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
	//一定要按顺序写
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
	//一定要按顺序写
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
	//一定要按顺序写
	WriteUint16(wMsgKind);
	WriteUint8(pVideoPassCard->bNewTurn);
	WriteUint16(pVideoPassCard->wPassUser);
	WriteUint16(pVideoPassCard->wCurrentUser);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_GameEnd *pVideoGameEnd)
{
	//一定要按顺序写
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

//调整缓冲
bool CGameVideoItemSink::RectifyBuffer(size_t iSize)
{
	try
	{		
		size_t iNewbufSize =  iSize+m_iBufferSize;
		//申请内存
		BYTE * pNewVideoBuffer=new BYTE [iNewbufSize];
		if (pNewVideoBuffer==NULL) return false;
		memset(pNewVideoBuffer,0,iNewbufSize);

		//拷贝数据
		if (m_pVideoDataBuffer!=NULL) 
		{
			CopyMemory(pNewVideoBuffer,m_pVideoDataBuffer,m_iCurPos);				
		}

		//设置变量			
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

//重置数据
void CGameVideoItemSink::ResetVideoItem()
{
	//设置变量
	m_iCurPos	= 0;	
	m_iBufferSize  = 0;
	SafeDeleteArray(m_pVideoDataBuffer);
}

//录像编号
VOID CGameVideoItemSink::BuildVideoNumber(CHAR szVideoNumber[], WORD wLen,WORD wServerID,WORD wTableID)
{
	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//格式化编号
	_snprintf_s(szVideoNumber, wLen, wLen,"%04d%02d%02d%02d%02d%02d%03d%03d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
		SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond,wServerID,wTableID);
}