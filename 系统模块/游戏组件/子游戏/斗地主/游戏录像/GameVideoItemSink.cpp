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
	//�����ʽ���ַ���+Ψһ��ʶID (ID��������������ʱ����+����ID+����ID) 	
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
	//һ��Ҫ��˳��д
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
	//һ��Ҫ��˳��д
	WriteUint16(wMsgKind);
	WriteUint16(pVideoCall->wCurrentUser);
	WriteUint16(pVideoCall->wCallScoreUser);
	WriteUint8(pVideoCall->cbCurrentScore);
	WriteUint8(pVideoCall->cbUserCallScore);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_BankerInfo *pVideoCall)
{
	//һ��Ҫ��˳��д
	WriteUint16(wMsgKind);
	WriteUint16(pVideoCall->wBankerUser);
	WriteUint16(pVideoCall->wCurrentUser);
	WriteUint8(pVideoCall->cbBankerScore);
	Write(pVideoCall->cbBankerCard,sizeof(BYTE)*3);

	//���� 
	WriteUint32(pVideoCall->lScoreTimes);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_StatusPlay *pVideoPlay)
{
	//һ��Ҫ��˳��д
	WriteUint16(wMsgKind);
	//ʱ����Ϣ
	WriteUint8(pVideoPlay->cbTimeOutCard);
	WriteUint8(pVideoPlay->cbTimeCallScore);
	WriteUint8(pVideoPlay->cbTimeStartGame);
	WriteUint8(pVideoPlay->cbTimeHeadOutCard);

	WriteInt32(pVideoPlay->lCellScore);
	WriteUint8(pVideoPlay->cbBombCount);
	WriteUint16(pVideoPlay->wBankerUser);
	WriteUint16(pVideoPlay->wCurrentUser);
	WriteUint8(pVideoPlay->cbBankerScore);
	
	//������Ϣ
	WriteUint16(pVideoPlay->wTurnWiner);
	WriteUint8(pVideoPlay->cbTurnCardCount);
	Write(pVideoPlay->cbTurnCardData,sizeof(BYTE)*MAX_COUNT);
	
	//�˿���Ϣ
	Write(pVideoPlay->cbBankerCard,sizeof(BYTE)*3);
	Write(pVideoPlay->cbHandCardData,sizeof(BYTE)*MAX_COUNT);
	Write(pVideoPlay->cbHandCardCount,sizeof(BYTE)*GAME_PLAYER);

	//��ʷ����
	Write(pVideoPlay->lTurnScore,sizeof(SCORE)*GAME_PLAYER);
	Write(pVideoPlay->lCollectScore,sizeof(SCORE)*GAME_PLAYER);
	Write(pVideoPlay->cbUserTrustee,sizeof(BYTE)*GAME_PLAYER);
	
	return true;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_OutCard *pVideoOutCard)
{
	//һ��Ҫ��˳��д
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
	//һ��Ҫ��˳��д
	WriteUint16(wMsgKind);

	WriteUint8(pVideoPassCard->cbTurnOver);
	WriteUint16(pVideoPassCard->wCurrentUser);
	WriteUint16(pVideoPassCard->wPassCardUser);

	return true;
}
bool __cdecl  CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_GameConclude *pVideoConclude)
{
	//һ��Ҫ��˳��д
	WriteUint16(wMsgKind);
	
	//���ֱ���
	WriteUint32(pVideoConclude->lCellScore);
	Write(pVideoConclude->lGameScore,sizeof(SCORE)*GAME_PLAYER);
	//�����־
	WriteUint8(pVideoConclude->bChunTian);
	WriteUint8(pVideoConclude->bFanChunTian);
	
	//ը����Ϣ
	WriteUint8(pVideoConclude->cbBombCount);
	Write(pVideoConclude->cbEachBombCount,sizeof(BYTE)*GAME_PLAYER);

	//��Ϸ��Ϣ
	WriteUint8(pVideoConclude->cbBankerScore);
	Write(pVideoConclude->cbCardCount,sizeof(BYTE)*GAME_PLAYER);
	Write(pVideoConclude->cbHandCardData,sizeof(BYTE)*FULL_COUNT);
	//���� 
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
	_snprintf_s(szVideoNumber,wLen,wLen,"%04d%02d%02d%02d%02d%02d%03d%03d",SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,
		SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond,wServerID,wTableID);
}