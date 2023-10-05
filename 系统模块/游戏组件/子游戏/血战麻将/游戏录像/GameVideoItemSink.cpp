#include "StdAfx.h"
#include "GameVideoItemSink.h"
#include <locale>

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

bool __cdecl CGameVideoItemSink::StopAndSaveVideo(WORD wServerID,WORD wTableID)
{
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

	WriteInfo(szFilePath);

	ResetVideoItem();

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,Video_GameStart *pVideoGameStart,bool bFirst)
{
	//一定要按顺序写
	if (bFirst)
	{
		WriteUint16(wMsgKind);
	}
	
	WriteUint8(pVideoGameStart->bHuanSanZhang);
	WriteUint8(pVideoGameStart->bHuJiaoZhuanYi);
	WriteUint8(pVideoGameStart->bZiMoAddTimes);
	WriteUint8(pVideoGameStart->bTianDiHu);
	WriteUint8(pVideoGameStart->bHaiDiLaoYue);
	WriteUint8(pVideoGameStart->bMenQing);

	WriteUint32(pVideoGameStart->lSiceCount);
	WriteUint16(pVideoGameStart->wBankerUser);
	WriteUint16(pVideoGameStart->wCurrentUser);
	WriteUint8(pVideoGameStart->cbUserAction);
	Write(pVideoGameStart->cbCardData, sizeof(BYTE) * MAX_COUNT * GAME_PLAYER);

	WriteUint8(pVideoGameStart->cbLeftCardCount);
	WriteUint8(pVideoGameStart->cbSendCardData);

	Write(pVideoGameStart->szNickName,sizeof(TCHAR)*LEN_NICKNAME);
	WriteUint16(pVideoGameStart->wChairID);
	WriteUint64(pVideoGameStart->lScore);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_CallCard *pVideoCallCard)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);
	WriteUint16(pVideoCallCard->wCallUser);
	WriteUint8(pVideoCallCard->cbCallCard);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, CMD_S_BankerInfo *pVideoBankerInfo, bool first)
{
	//一定要按顺序写
	if (first)
	{
		WriteUint16(wMsgKind);
	}

	WriteUint16(pVideoBankerInfo->wBankerUser);
	WriteUint16(pVideoBankerInfo->wCurrentUser);
	Write(pVideoBankerInfo->cbCallCard, sizeof(BYTE)* GAME_PLAYER);
	WriteUint8(pVideoBankerInfo->cbUserAction);

	return TRUE;

}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_OutCard *pVideoOutCard)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);
	WriteUint16(pVideoOutCard->wOutCardUser);
	WriteUint8(pVideoOutCard->cbOutCardData);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind,CMD_S_SendCard *pVideoSendCard)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);
	WriteUint8(pVideoSendCard->cbCardData);
	WriteUint8(pVideoSendCard->cbActionMask);
	WriteUint16(pVideoSendCard->wCurrentUser);
	WriteUint8(pVideoSendCard->bTail);

	return TRUE;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, Video_OperateResult *pVideoOperateResult)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);

	WriteUint16(pVideoOperateResult->wOperateUser);
	WriteUint16(pVideoOperateResult->wProvideUser);
	WriteUint8(pVideoOperateResult->cbOperateCode);
	WriteUint8(pVideoOperateResult->cbOperateCard);

	return true;
}

bool __cdecl  CGameVideoItemSink::AddVideoData(WORD wMsgKind, Video_GameEnd *pVideoGameEnd)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);

	Write(pVideoGameEnd->cbCardCount, sizeof(BYTE)* GAME_PLAYER);
	Write(pVideoGameEnd->cbCardData, sizeof(BYTE)* GAME_PLAYER * MAX_COUNT);

	Write(pVideoGameEnd->cbWeaveCount, sizeof(BYTE)* GAME_PLAYER);
	Write(pVideoGameEnd->WeaveItemArray, sizeof(tagWeaveItem)* GAME_PLAYER * MAX_WEAVE);
	Write(pVideoGameEnd->cbChiHuCardArray, sizeof(BYTE)* GAME_PLAYER);

	Write(pVideoGameEnd->wProvideUser, sizeof(WORD)* GAME_PLAYER);
	Write(pVideoGameEnd->dwChiHuRight, sizeof(DWORD)* GAME_PLAYER);
	Write(pVideoGameEnd->lGameScore, sizeof(LONGLONG)* GAME_PLAYER);
	Write(pVideoGameEnd->lGameTax, sizeof(LONG)* GAME_PLAYER);

	Write(pVideoGameEnd->tagHistoryChart, sizeof(tagChart)* GAME_PLAYER * MAX_CHART_COUNT);

	return true;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, Video_ChiHu *pVideoChiHu)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);

	WriteUint16(pVideoChiHu->wChiHuUser);
	WriteUint16(pVideoChiHu->wProviderUser);
	WriteUint8(pVideoChiHu->cbChiHuCard);
	WriteUint8(pVideoChiHu->cbCardCount);
	WriteUint64(pVideoChiHu->lGameScore);
	WriteUint8(pVideoChiHu->cbWinOrder);

	return true;
}

bool __cdecl CGameVideoItemSink::AddVideoData(WORD wMsgKind, Video_ChangeCardResult *pVideoChangeCardResult)
{
	//一定要按顺序写
	WriteUint16(wMsgKind);

	WriteUint8(pVideoChangeCardResult->cbChangeType);
	Write(pVideoChangeCardResult->cbChangeCardDataBefore, sizeof(BYTE)* GAME_PLAYER * MAX_CHANGE_CARDCOUNT);
	Write(pVideoChangeCardResult->cbChangeCardDataAfter, sizeof(BYTE)* GAME_PLAYER * MAX_CHANGE_CARDCOUNT);
	WriteUint8(pVideoChangeCardResult->cbSendCardData);

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

//写日志文件
void CGameVideoItemSink::WriteInfo(LPCTSTR pszString)
{
	//设置语言区域
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName = TEXT("SparrowXZVideo.txt");
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		myFile.SeekToEnd();
		myFile.WriteString(pszString);
		myFile.Flush();
		myFile.Close();
	}

	//还原区域设定
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}