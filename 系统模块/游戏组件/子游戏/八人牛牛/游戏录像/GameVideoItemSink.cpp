#include "StdAfx.h"
#include "GameVideoItemSink.h"

#define  ADD_VIDEO_BUF  4096

CGameVideoItemSink::CGameVideoItemSink(void)
{
	//���ñ���
	m_iCurPos = 0;
	m_iBufferSize = 0;
	m_pVideoDataBuffer = NULL;
	m_pITableFrame = NULL;

	m_cbPlayerCount = 0;
}

CGameVideoItemSink::~CGameVideoItemSink(void)
{

}

bool __cdecl CGameVideoItemSink::StartVideo(ITableFrame	*pTableFrame, BYTE cbPlayerCount)
{
	try
	{
		ResetVideoItem();

		m_pITableFrame = pTableFrame;
		m_cbPlayerCount = cbPlayerCount;

		m_iCurPos = 0;
		m_iBufferSize = ADD_VIDEO_BUF;

		//�����ڴ�
		m_pVideoDataBuffer = new BYTE[m_iBufferSize];
		if (m_pVideoDataBuffer == NULL) return false;
		memset(m_pVideoDataBuffer, 0, m_iBufferSize);
	}
	catch (...)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

bool __cdecl CGameVideoItemSink::StopAndSaveVideo(WORD wServerID, WORD wTableID)
{
	//���浽���ݿ�
	if (m_pITableFrame == NULL) return false;
	//�����ʽ���ַ���+Ψһ��ʶID (ID��������������ʱ����+����ID+����ID) 	
	CHAR   szVideoNumber[22];
	szVideoNumber[0] = 0;
	BuildVideoNumber(szVideoNumber, 22, wServerID, wTableID);

	bool bAllAndroid = true;
	for (WORD i = 0; i<m_cbPlayerCount; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem && pServerUserItem->IsAndroidUser() == false)
		{
			bAllAndroid = false;
		}
	}
	if (bAllAndroid) return TRUE;

	for (WORD i = 0; i<m_cbPlayerCount; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem)
		{
			m_pITableFrame->WriteTableVideoPlayer(pServerUserItem->GetUserID(), szVideoNumber);
		}
	}
	m_pITableFrame->WriteTableVideoData(szVideoNumber, wServerID, wTableID, m_pVideoDataBuffer, (WORD)m_iCurPos);

	int iLength;
	TCHAR   szVideoID[50];
	iLength = MultiByteToWideChar(CP_ACP, 0, szVideoNumber, strlen(szVideoNumber) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, szVideoNumber, strlen(szVideoNumber) + 1, szVideoID, iLength);

	TCHAR szFilePath[MAX_PATH];
	_sntprintf_s(szFilePath, CountArray(szFilePath), TEXT("%s.Video"), szVideoID);

	ResetVideoItem();

	return TRUE;
}
bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, Viedeo_GameConfig *pVideoGameStart)
{
	WriteUint16(wMsgKind);
	Write(pVideoGameStart->wCardTypeTimes, sizeof(WORD)* MAX_CARD_TYPE);
	return TRUE;
};
bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, Video_GameStart *pVideoGameStart,bool bFirst)
{
	if (bFirst) WriteUint16(wMsgKind);
	WriteUint64(pVideoGameStart->lCellScore);
	WriteUint16(pVideoGameStart->wPlayerCount);
	WriteUint16(pVideoGameStart->wGamePlayerCountRule);
	WriteUint16(pVideoGameStart->wBankerUser);
	Write(pVideoGameStart->cbPlayerStatus, sizeof(BYTE)* GAME_PLAYER);
	WriteUint64(pVideoGameStart->lTurnMaxScore);
	Write(pVideoGameStart->cbCardData, sizeof(BYTE)* GAME_PLAYER * MAX_CARDCOUNT);

	WriteUint32(pVideoGameStart->ctConfig);
	WriteUint32(pVideoGameStart->stConfig);
	WriteUint32(pVideoGameStart->bgtConfig);
	WriteUint32(pVideoGameStart->btConfig);
	WriteUint32(pVideoGameStart->gtConfig);

	Write(pVideoGameStart->lFreeConfig, sizeof(DWORD)* MAX_CONFIG);
	Write(pVideoGameStart->lPercentConfig, sizeof(DWORD)* MAX_CONFIG);

	Write(pVideoGameStart->szNickName, sizeof(TCHAR)*LEN_NICKNAME);
	WriteUint16(pVideoGameStart->wChairID);
	WriteUint64(pVideoGameStart->lScore);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_CallBanker *pVideoCallBanker)
{
	//һ��Ҫ��˳��д

	WriteUint16(wMsgKind);
	WriteUint16(pVideoCallBanker->wGamePlayerCountRule);
	WriteUint32(pVideoCallBanker->ctConfig);
	WriteUint32(pVideoCallBanker->stConfig);
	WriteUint32(pVideoCallBanker->bgtConfig);
	WriteUint8(pVideoCallBanker->cbMaxCallBankerTimes);
	WriteUint16(pVideoCallBanker->wBgtRobNewTurnChairID);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_SendFourCard *pSendFourCard)
{
	//һ��Ҫ��˳��д
	WriteUint16(wMsgKind);
	Write(pSendFourCard->cbCardData, sizeof(BYTE)* GAME_PLAYER * MAX_CARDCOUNT);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_CallBankerInfo *pVideoCallBankerInfo)
{
	//һ��Ҫ��˳��д

	WriteUint16(wMsgKind);
	Write(pVideoCallBankerInfo->cbCallBankerStatus, sizeof(BYTE)* GAME_PLAYER);
	Write(pVideoCallBankerInfo->cbCallBankerTimes, sizeof(BYTE)* GAME_PLAYER);
	WriteUint16(pVideoCallBankerInfo->wChairID);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_AddScore *pVideoAddScore)
{
	//һ��Ҫ��˳��д

	WriteUint16(wMsgKind);
	WriteUint16(pVideoAddScore->wAddScoreUser);
	WriteUint64(pVideoAddScore->lAddScoreCount);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_SendCard *pVideoSendCard)
{
	//һ��Ҫ��˳��д

	WriteUint16(wMsgKind);
	Write(pVideoSendCard->cbCardData, sizeof(BYTE)* GAME_PLAYER * MAX_CARDCOUNT);
	Write(pVideoSendCard->bSpecialCard, sizeof(bool)* GAME_PLAYER);
	Write(pVideoSendCard->cbOriginalCardType, sizeof(BYTE)* GAME_PLAYER);

	return true;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_Open_Card *pVideoOpen_Card)
{
	//һ��Ҫ��˳��д

	WriteUint16(wMsgKind);
	WriteUint16(pVideoOpen_Card->wOpenChairID);
	WriteUint8(pVideoOpen_Card->bOpenCard);

	return true;
}

bool __cdecl  CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_GameEnd *pVideoGameEnd)
{
	//һ��Ҫ��˳��д

	WriteUint16(wMsgKind);
	Write(pVideoGameEnd->lGameTax, sizeof(LONGLONG)* GAME_PLAYER);
	Write(pVideoGameEnd->lGameScore, sizeof(LONGLONG)* GAME_PLAYER);
	Write(pVideoGameEnd->cbHandCardData, sizeof(BYTE)* GAME_PLAYER * MAX_CARDCOUNT);
	Write(pVideoGameEnd->cbCardType, sizeof(BYTE)* GAME_PLAYER);
	Write(pVideoGameEnd->wCardTypeTimes, sizeof(WORD)* GAME_PLAYER);
	WriteUint8(pVideoGameEnd->cbDelayOverGame);

	return true;
}

size_t	CGameVideoItemSink::Write(const void* data, size_t size)
{
	if (size + m_iCurPos > m_iBufferSize)
	{
		if (RectifyBuffer(ADD_VIDEO_BUF / 2) != TRUE) return 0;
	}

	CopyMemory(m_pVideoDataBuffer + m_iCurPos, data, size);
	m_iCurPos += size;

	return size;
}

//��������
bool CGameVideoItemSink::RectifyBuffer(size_t iSize)
{
	try
	{
		size_t iNewbufSize = iSize + m_iBufferSize;
		//�����ڴ�
		BYTE * pNewVideoBuffer = new BYTE[iNewbufSize];
		if (pNewVideoBuffer == NULL) return false;
		memset(pNewVideoBuffer, 0, iNewbufSize);

		//��������
		if (m_pVideoDataBuffer != NULL)
		{
			CopyMemory(pNewVideoBuffer, m_pVideoDataBuffer, m_iCurPos);
		}

		//���ñ���			
		m_iBufferSize = iNewbufSize;
		SafeDeleteArray(m_pVideoDataBuffer);
		m_pVideoDataBuffer = pNewVideoBuffer;
	}
	catch (...)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//��������
void CGameVideoItemSink::ResetVideoItem()
{
	//���ñ���
	m_iCurPos = 0;
	m_iBufferSize = 0;
	SafeDeleteArray(m_pVideoDataBuffer);
}

//¼����
VOID CGameVideoItemSink::BuildVideoNumber(CHAR szVideoNumber[], WORD wLen, WORD wServerID, WORD wTableID)
{
	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	//��ʽ�����
	_snprintf_s(szVideoNumber, wLen, wLen, "%04d%02d%02d%02d%02d%02d%03d%03d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
		SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, wServerID, wTableID);
}