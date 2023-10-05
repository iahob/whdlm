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

bool __cdecl CGameVideoItemSink::StopAndSaveVideo(WORD wServerID,WORD wTableID)
{
	//保存到数据库
	if(m_pITableFrame == NULL) return false;
	//保存格式：字符串+唯一标识ID (ID命名规则：年月日时分秒+房间ID+桌子ID) 	
	CHAR   szVideoNumber[22];
	szVideoNumber[0] = 0;
	BuildVideoNumber(szVideoNumber,22,wServerID,wTableID);
	
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

//	CFile VideoFile(szFilePath,CFile::modeReadWrite|CFile::modeCreate|CFile::typeBinary);
//	VideoFile.Write(m_pVideoDataBuffer,m_iCurPos);

	ResetVideoItem();

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,Video_GameStart *pVideoGameStart,bool bFirst)
{
	//一定要按顺序写
	if(bFirst) WriteUint16(wMsgKind);

	Write(pVideoGameStart->szNickName,sizeof(TCHAR)*LEN_NICKNAME);
	WriteUint16(pVideoGameStart->wTableID);
	WriteUint16(pVideoGameStart->wStartUser);
	WriteUint16(pVideoGameStart->wCurrentUser);
	WriteUint8(pVideoGameStart->cbValidCardData);
	WriteUint8(pVideoGameStart->cbValidCardIndex);
	Write(pVideoGameStart->cbCardData,sizeof(BYTE)*NORMAL_COUNT);
	WriteUint64(pVideoGameStart->lScore);
	WriteUint32(pVideoGameStart->lCellScore);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_CallScore *pVideoCall)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);
	WriteUint16(pVideoCall->wCurrentUser);
	WriteUint16(pVideoCall->wCallScoreUser);
	WriteUint8(pVideoCall->cbCurrentScore);
	WriteUint8(pVideoCall->cbUserCallScore);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_BankerInfo *pVideoCall)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);
	WriteUint16(pVideoCall->wBankerUser);
	WriteUint16(pVideoCall->wCurrentUser);
	WriteUint8(pVideoCall->cbBankerScore);
	Write(pVideoCall->cbBankerCard,sizeof(BYTE)*3);

	//倍数 
	WriteUint32(pVideoCall->lScoreTimes);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_StatusPlay *pVideoPlay)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);
	//时间信息
	WriteUint8(pVideoPlay->cbTimeOutCard);
	WriteUint8(pVideoPlay->cbTimeCallScore);
	WriteUint8(pVideoPlay->cbTimeStartGame);
	WriteUint8(pVideoPlay->cbTimeHeadOutCard);

	WriteInt32(pVideoPlay->lCellScore);
	WriteUint8(pVideoPlay->cbBombCount);
	WriteUint16(pVideoPlay->wBankerUser);
	WriteUint16(pVideoPlay->wCurrentUser);
	WriteUint8(pVideoPlay->cbBankerScore);
	
	//出牌信息
	WriteUint16(pVideoPlay->wTurnWiner);
	WriteUint8(pVideoPlay->cbTurnCardCount);
	Write(pVideoPlay->cbTurnCardData,sizeof(BYTE)*MAX_COUNT);
	
	//扑克信息
	Write(pVideoPlay->cbBankerCard,sizeof(BYTE)*3);
	Write(pVideoPlay->cbHandCardData,sizeof(BYTE)*MAX_COUNT);
	Write(pVideoPlay->cbHandCardCount,sizeof(BYTE)*GAME_PLAYER);

	//历史积分
	Write(pVideoPlay->lTurnScore,sizeof(SCORE)*GAME_PLAYER);
	Write(pVideoPlay->lCollectScore,sizeof(SCORE)*GAME_PLAYER);
	Write(pVideoPlay->cbUserTrustee,sizeof(BYTE)*GAME_PLAYER);
	
	return true;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_OutCard *pVideoOutCard)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);

	WriteUint8(pVideoOutCard->cbCardCount);
	WriteUint16(pVideoOutCard->wCurrentUser);
	WriteUint16(pVideoOutCard->wOutCardUser);
	WriteUint32(pVideoOutCard->lScoreTimes);
	Write((void*)(&(pVideoOutCard->remainCardCount)), sizeof(REMAINCARDCOUNT));
	Write(pVideoOutCard->cbCardData,sizeof(BYTE)*MAX_COUNT);

	return true;
}

bool __cdecl  CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_PassCard *pVideoPassCard)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);

	WriteUint8(pVideoPassCard->cbTurnOver);
	WriteUint16(pVideoPassCard->wCurrentUser);
	WriteUint16(pVideoPassCard->wPassCardUser);

	return true;
}
bool __cdecl  CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_GameConclude *pVideoConclude)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);
	
	//积分变量
	WriteUint32(pVideoConclude->lCellScore);
	Write(pVideoConclude->lGameScore,sizeof(SCORE)*GAME_PLAYER);
	//春天标志
	WriteUint8(pVideoConclude->bChunTian);
	WriteUint8(pVideoConclude->bFanChunTian);
	
	//炸弹信息
	WriteUint8(pVideoConclude->cbBombCount);
	Write(pVideoConclude->cbEachBombCount,sizeof(BYTE)*GAME_PLAYER);

	//游戏信息
	WriteUint8(pVideoConclude->cbBankerScore);
	Write(pVideoConclude->cbCardCount,sizeof(BYTE)*GAME_PLAYER);
	Write(pVideoConclude->cbHandCardData,sizeof(BYTE)*FULL_COUNT);
	//倍数 
	WriteUint32(pVideoConclude->lScoreTimes);
	Write((void*)(&(pVideoConclude->remainCardCount)), sizeof(REMAINCARDCOUNT));

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
	_snprintf_s(szVideoNumber,wLen,wLen,"%04d%02d%02d%02d%02d%02d%03d%03d",SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,
		SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond,wServerID,wTableID);
}