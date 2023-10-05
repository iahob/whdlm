#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
#include "../服务器组件/dump.h"
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//时间定义
	m_cbTimeStartGame = 0;
	m_cbTimeBet = 0;

	//扑克变量
	m_cbCenterCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbCenterCardData, sizeof(m_cbCenterCardData));
	ZeroMemory(m_bAndroidUser, sizeof(m_bAndroidUser));
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_lUserMaxScore, sizeof(m_lUserMaxScore));
	ZeroMemory(m_lUserTableScore, sizeof(m_lUserTableScore));
	ZeroMemory(m_lUserTotalScore, sizeof(m_lUserTotalScore));
	ZeroMemory(m_cbCurType, sizeof(m_cbCurType));
	ZeroMemory(m_cbFinalCardType, sizeof(m_cbFinalCardType));
	m_lMinBetScore = 0;
	m_lMaxBetScore = 0;
	//接口变量
	m_pIAndroidUserItem=NULL;
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)WHCrashHandler);
	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
VOID * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	//扑克变量
	m_cbCenterCardCount = 0;
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbCenterCardData, sizeof(m_cbCenterCardData));
	ZeroMemory(m_bAndroidUser, sizeof(m_bAndroidUser));
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_lUserTableScore, sizeof(m_lUserTableScore));
	ZeroMemory(m_lUserTotalScore, sizeof(m_lUserTotalScore));
	ZeroMemory(m_cbCurType, sizeof(m_cbCurType));
	ZeroMemory(m_cbFinalCardType, sizeof(m_cbFinalCardType));
	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//开始游戏
	{
		CString cc;
		SCORE lScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
		if (m_lUserMaxScore[GetMeChairID()] < m_lMinBetScore && lScore-m_lUserMaxScore[GetMeChairID()]>=m_lMinBetScore)
		{
			CMD_C_SetChip SetChip;
			SetChip.bAutoAddChip = false;
			SetChip.lChipScore = min(m_lMaxBetScore-m_lUserMaxScore[GetMeChairID()], lScore-m_lUserMaxScore[GetMeChairID()]);
			m_pIAndroidUserItem->SendSocketData(SUB_C_SET_CHIP, &SetChip, sizeof(SetChip));
			cc.Format(_T("补充筹码 chipscore=%I64d,takescore=%I64d,lscore=%I64d,minscore=%I64d,maxscore=%I64d,name=%s"),SetChip.lChipScore, m_lUserMaxScore[GetMeChairID()], lScore, m_lMinBetScore, m_lMaxBetScore, m_pIAndroidUserItem->GetMeUserItem()->GetNickName());
			WriteDebugInfo(cc);
		}
		else if (m_pIAndroidUserItem->GetGameStatus() == GS_FREE && m_pIAndroidUserItem->GetGameStatus() != US_READY && m_lUserMaxScore[GetMeChairID()] >= m_lMinBetScore)
		{
			cc.Format(_T("AI发送准备，name=%s"), m_pIAndroidUserItem->GetMeUserItem()->GetNickName());
			WriteDebugInfo(cc);
			m_pIAndroidUserItem->SendUserReady(NULL, 0);
		}
		else
		{
			cc.Format(_T("Error,takescore=%I64d,userscore=%I64d,minbetscore=%I64d,name=%s"), m_lUserMaxScore[GetMeChairID()], lScore, m_lMinBetScore, m_pIAndroidUserItem->GetMeUserItem()->GetNickName());
			WriteDebugInfo(cc);
		}
		return true;
	}
	case IDI_ADD_SCORE:
	{
		WORD wMeChair = GetMeChairID();
		BYTE cbMyCardType = m_cbCurType[wMeChair];
		bool bCanPass = m_lTurnLessScore == 0;
		double dValue1 = m_lUserTotalScore[wMeChair] / (double)m_lUserMaxScore[wMeChair];
		if (m_bMustWin && IsMeWinner())
		{
			if (bCanPass && m_cbCurCardWeight < 30)
			{
				return AddScore(m_lTurnLessScore);
			}

			return AddScore(GetAddScore());
		}
		else//正常模式
		{
			if (bCanPass)
			{
				if (m_bAddedScoreThisTurn)
				{
					return AddScore(0);
				}
				else
				{
					if (m_cbCurCardWeight > 30 && rand() % 100 < m_cbCurCardWeight)
					{
						return AddScore(GetAddScore());
					}

				}

				return AddScore(0);
			}
			else
			{
				if (NeedGiveUp())
					return GiveUp();

				return AddScore(GetAddScore());
			}
		}
		return true;
	}
	}
	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_ANDROID_CARD:			//发送扑克
		{
			return OnSubAndroidCard(pData,wDataSize);
		}
	case SUB_S_GAME_START:			//游戏开始
		{
			return OnSubGameStart(pData , wDataSize) ;
		}
	case SUB_S_ADD_SCORE:				//下注
		{
			return OnSubAddScore(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:				//弃牌
	{
		return OnSubGiveUp(pData, wDataSize);
	}
	case SUB_S_SEND_CARD:
	{
		return OnSubSendCard(pData, wDataSize);
	}
	case SUB_S_GAME_CONCLUDE://游戏结束
	{
		return OnSubGameEnd(pData, wDataSize);
	}
	case SUB_S_SET_CHIP:
	{
		CMD_S_SetChip *pChip = (CMD_S_SetChip*)pData;
		if (pChip->wChipUser == GetMeChairID())
		{

			m_lUserMaxScore[GetMeChairID()] += pChip->lChipScore;
			UINT nElapse = rand() % 5 + TIME_LESS;
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);
			CString cc;
			cc.Format(_T("补充筹码返回,chipscore=%I64d,takescore=%I64d,name=%s"), pChip->lChipScore, m_lUserMaxScore[GetMeChairID()], m_pIAndroidUserItem->GetMeUserItem()->GetNickName());
			WriteDebugInfo(cc);
		}
		return true;
	}
	case SUB_S_ANDROID_INFO:
	{
		//效验数据
		if (wDataSize != sizeof(tagCustomAndroid)) return false;

		//消息处理
		tagCustomAndroid * pAndroidInfo = (tagCustomAndroid *)pData;

		m_lRobotScoreRange[0] = pAndroidInfo->lRobotScoreMin;
		m_lRobotScoreRange[1] = pAndroidInfo->lRobotScoreMax;
		m_lRobotBankGetScore = pAndroidInfo->lRobotBankGet;
		m_lRobotBankGetScoreBanker = pAndroidInfo->lRobotBankGetBanker;
		m_nRobotBankStorageMul = pAndroidInfo->lRobotBankStoMul;
		CString cc;
		cc.Format(_T("收到配置信息 range0=%I64d,range1=%I64d,getscore=%I64d,scoreBanker=%I64d,mul=%d,name=%s"),
			m_lRobotScoreRange[0], m_lRobotScoreRange[1], m_lRobotBankGetScore, m_lRobotBankGetScoreBanker, m_nRobotBankStorageMul,GetMeItem()->GetNickName());
		WriteDebugInfo(cc);
		return true;
	}
	case SUB_S_GAME_OVER:
	{
		//游戏结束
		WriteDebugInfo(_T("Game Over"));
		m_pIAndroidUserItem->SetGameStatus(GS_FREE);
		RepositionSink();

		UINT nElapse = 2+(rand() % 5);
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

		BankOperate();
	}
	default:
		{
			return true;
		}
	}

	return true;

}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_FREE:	//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

			//时间定义
			m_cbTimeStartGame = pStatusFree->cbStartTime;
			m_cbTimeBet = pStatusFree->cbBetTime;
			m_lMinBetScore = pStatusFree->lMinBetScore;
			m_lMaxBetScore = pStatusFree->lMaxBetScore;
			m_lAnteScore = pStatusFree->lAnteScore;
			m_lCellScore = pStatusFree->lCellScore;
			m_lUserMaxScore[GetMeChairID()] = 0;
			CopyMemory(m_lUserMaxScore, pStatusFree->lUserMaxScore, sizeof(m_lUserMaxScore));

			//玩家设置
			if (pIServerUserItem->GetUserStatus()!=US_READY)
			{
				UINT nElapse=rand()%TIMER_START_GAME+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
			}

			CString cc;
			cc.Format(_T("收到空闲场景,takescore=%I64d,minbetscore=%I64d,maxbetscore=%I64d,name=%s"),m_lUserMaxScore[GetMeChairID()], m_lMinBetScore, m_lMaxBetScore,GetMeItem()->GetNickName());
			WriteDebugInfo(cc);
			return true;
		}
	case GS_PLAY: //游戏状态
		{
			//验证参数
			ASSERT( sizeof( CMD_S_StatusPlay ) == wDataSize ) ;
			if ( sizeof( CMD_S_StatusPlay ) != wDataSize ) return false ;

			CMD_S_StatusPlay *pStatusPlay = (CMD_S_StatusPlay *)pData;
			m_lCellScore = pStatusPlay->lCellScore;
			m_lAnteScore = pStatusPlay->lAnteScore;
			m_lMinBetScore = pStatusPlay->lMinBetScore;
			m_lMaxBetScore = pStatusPlay->lMaxBetScore;
			m_cbTimeBet = pStatusPlay->cbBetTime;
			m_cbTimeStartGame = pStatusPlay->cbStartTime;
			CopyMemory(m_lUserMaxScore, pStatusPlay->lUserMaxScore, sizeof(m_lUserMaxScore));

			CString cc;
			cc.Format(_T("收到游戏场景,minbetscore=%I64d,maxbetscore=%I64d,lusermaxscore=%I64d,name=%s"), m_lMinBetScore, m_lMaxBetScore,m_lUserMaxScore[GetMeChairID()], GetMeItem()->GetNickName());
			WriteDebugInfo(cc);

			return true;
		}
	}
	ASSERT(false);
	return true;
}

//用户进入
VOID CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	WORD a = pIAndroidUserItem->GetChairID();
	return;
}

//用户离开
VOID CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
VOID CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
VOID CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
VOID CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//发送扑克
bool CAndroidUserItemSink::OnSubAndroidCard( const void * pBuffer, WORD wDataSize )
{
	m_bAddedScoreThisTurn = false;
	m_bMustWin = false;
	m_cbCurCardWeight = 0;
	if (m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus() != US_PLAYING )
	{
		return true;
	}

	ASSERT(wDataSize == sizeof(CMD_S_AndroidCard));
	if (wDataSize != sizeof(CMD_S_AndroidCard)) return false;

	//变量定义
	CMD_S_AndroidCard * pSendCard = (CMD_S_AndroidCard *)pBuffer;
	IServerUserItem * pIServerUserItem = m_pIAndroidUserItem->GetMeUserItem();
	CopyMemory(m_bAndroidUser, pSendCard->bAndroidUser, sizeof(m_bAndroidUser));
	CopyMemory(m_cbCenterCardData, pSendCard->cbCenterCardData, sizeof(m_cbCenterCardData));
	CopyMemory(m_cbHandCardData, pSendCard->cbAllHandCardData, sizeof(m_cbHandCardData));
	CopyMemory(m_lUserTotalScore, pSendCard->lTotalScore, sizeof(m_lUserTotalScore));
	CopyMemory(m_lUserTableScore, pSendCard->lTotalScore, sizeof(m_lUserTableScore));
	m_bMustWin = pSendCard->bMustWin;
	BYTE cbEndCardData[GAME_PLAYER][MAX_CENTER_COUNT];
	ZeroMemory(cbEndCardData, sizeof(cbEndCardData));

	m_cbUserCount = 0;
	//获取最大牌型
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_cbHandCardData[i][0] == 0) continue;
		m_cbFinalCardType[i] = m_GameLogic.FiveFromSeven(m_cbHandCardData[i], MAX_COUNT, m_cbCenterCardData, MAX_CENTER_COUNT, cbEndCardData[i], MAX_CENTER_COUNT);
		if (m_cbHandCardData[i][0] == m_cbHandCardData[i][1])
			m_cbCurType[i] = CT_ONE_LONG;
		else
			m_cbCurType[i] = CT_SINGLE;

		m_cbUserCount++;
	}
	
	m_cbCurCardWeight = m_GameLogic.GetCurCardWeight(m_cbHandCardData[GetMeChairID()], m_cbCenterCardData, m_cbCenterCardCount);
	m_GameLogic.SelectMaxUser(cbEndCardData, m_UserWinList, NULL);

	return true;
}

bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_S_GameStart));
	if (wDataSize != sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart = (CMD_S_GameStart *)pBuffer;

	CopyMemory(m_lUserMaxScore, pGameStart->lUserMaxScore, sizeof(m_lUserMaxScore));
	CopyMemory(m_cbPlayStatus, pGameStart->cbPlayStatus, sizeof(m_cbPlayStatus));
	m_lTurnLessScore = pGameStart->lTurnLessScore;
	m_lTurnMaxScore = pGameStart->lTurnMaxScore;
	m_lAddLessScore = pGameStart->lAddLessScore;

	if (pGameStart->wCurrentUser == GetMeChairID())
	{
		UINT nElapse = rand() % (m_cbTimeBet / 4) + TIME_LESS+m_cbUserCount/2+2;
		m_pIAndroidUserItem->SetGameTimer(IDI_ADD_SCORE, nElapse);
	}

	return true;
}

bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore * pAddScore = (CMD_S_AddScore *)pBuffer;

	//变量定义
	WORD wAddScoreUser = pAddScore->wAddScoreUser;
	if (pAddScore->lAddScoreCount > 0)
	{
		m_lUserTableScore[wAddScoreUser] += pAddScore->lAddScoreCount;
		m_lUserTotalScore[wAddScoreUser] += pAddScore->lAddScoreCount;
	}
	if (pAddScore->wCurrentUser == INVALID_CHAIR)
	{
		ZeroMemory(m_lUserTableScore, sizeof(m_lUserTableScore));
	}
	else
	{
		m_lTurnLessScore = pAddScore->lTurnLessScore;
		m_lTurnMaxScore = pAddScore->lTurnMaxScore;
		m_lAddLessScore = pAddScore->lAddLessScore;
	}

	if (pAddScore->wCurrentUser == GetMeChairID())
	{
		UINT nElapse = rand() % (m_cbTimeBet / 4) + TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_ADD_SCORE, nElapse);
	}

	return true;
}

bool CAndroidUserItemSink::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp = (CMD_S_GiveUp *)pBuffer;
	m_cbPlayStatus[pGiveUp->wGiveUpUser] = FALSE;

	BYTE cbEndCardData[GAME_PLAYER][MAX_CENTER_COUNT];
	ZeroMemory(cbEndCardData, sizeof(cbEndCardData));

	//获取最大牌型
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i] == FALSE) continue;
		m_cbFinalCardType[i] = m_GameLogic.FiveFromSeven(m_cbHandCardData[i], MAX_COUNT, m_cbCenterCardData, MAX_CENTER_COUNT, cbEndCardData[i], MAX_CENTER_COUNT);
	}

	m_GameLogic.SelectMaxUser(cbEndCardData, m_UserWinList, NULL);

	return true;
}

bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//校验数据
	if (wDataSize != sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * pSendCard = (CMD_S_SendCard *)pBuffer;
	m_cbCenterCardCount = pSendCard->cbSendCardCount;
	m_lTurnLessScore = pSendCard->lTurnLessScore;
	m_lTurnMaxScore = pSendCard->lTurnMaxScore;
	m_lAddLessScore = pSendCard->lAddLessScore;
	m_bAddedScoreThisTurn = false;
	if (pSendCard->wCurrentUser == GetMeChairID())
	{
		UINT nElapse = 0;
		if (m_cbCenterCardCount==3)
			nElapse = rand() % (m_cbTimeBet / 4) + TIME_LESS + 3;
		else
			nElapse = rand() % (m_cbTimeBet / 4) + TIME_LESS + TIME_LESS;

		m_pIAndroidUserItem->SetGameTimer(IDI_ADD_SCORE, nElapse);
	}

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i]==0 || m_cbHandCardData[i][0]==0) continue;
		BYTE cbLastCardData[MAX_CENTER_COUNT] = { 0 };
		m_cbCurType[i] = m_GameLogic.FiveFromSeven(m_cbHandCardData[i], MAX_COUNT, m_cbCenterCardData, m_cbCenterCardCount, cbLastCardData, MAX_CENTER_COUNT);
	}

	if (m_cbPlayStatus[GetMeChairID()])
		m_cbCurCardWeight = m_GameLogic.GetCurCardWeight(m_cbHandCardData[GetMeChairID()], m_cbCenterCardData, m_cbCenterCardCount);
	return true;
}

bool CAndroidUserItemSink::OnSubGameEnd( const void * pBuffer, WORD wDataSize )
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_GameConclude)) return false;
	CMD_S_GameConclude * pGameConclude = (CMD_S_GameConclude *)pBuffer;
	CopyMemory(m_lUserMaxScore, pGameConclude->lUserMaxScore, sizeof(m_lUserMaxScore));
	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_ADD_SCORE);
	return true;
}

WORD CAndroidUserItemSink::GetMeChairID()
{
	return m_pIAndroidUserItem->GetMeUserItem()->GetChairID();
}

//银行操作
void CAndroidUserItemSink::BankOperate()
{
	IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	//变量定义
	LONGLONG lRobotScore = pUserItem->GetUserScore();

	//判断存取
	if (lRobotScore > m_lRobotScoreRange[1])
	{			
		LONGLONG lSaveScore = 0L;

		LONGLONG lCanSaveScore = max(0, lRobotScore - m_lUserMaxScore[GetMeChairID()]);
		if (lCanSaveScore > 0)
		{
			if (m_lUserMaxScore[GetMeChairID()] >= min(m_lRobotScoreRange[1],m_lMaxBetScore))
			{
				lSaveScore = lCanSaveScore;
			}
			else if (m_lUserMaxScore[GetMeChairID()]>=m_lMinBetScore)
			{
				lSaveScore = max(0, lCanSaveScore - m_lMinBetScore);
			}
			else
			{
				lSaveScore = lCanSaveScore - m_lMinBetScore;
			}

			if (lSaveScore > 0)
			{
				CString cc;
				cc.Format(_T("AI分数%I64d高于%I64d,执行存款%I64d,takescore=%I64d,name=%s"), lRobotScore, m_lRobotScoreRange[1], lSaveScore,m_lUserMaxScore[GetMeChairID()], pUserItem->GetNickName());
				WriteDebugInfo(cc);
				m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
			}
		}
		
	}
	else if ( (m_lUserMaxScore[GetMeChairID()]<m_lMinBetScore && lRobotScore-m_lUserMaxScore[GetMeChairID()]< m_lMinBetScore) || lRobotScore<m_lRobotScoreRange[0])
	{
		SCORE lScore = max(m_lRobotBankGetScore, rand() % m_lRobotBankGetScoreBanker);
		if ( (lScore + lRobotScore- m_lUserMaxScore[GetMeChairID()]) < m_lMinBetScore)
			lScore = m_lMinBetScore;

		if (lScore > 0)
		{
			CString cc;
			cc.Format(_T("AI分数%I64d低于%I64d,执行取款%I64d,takescore=%I64d,name=%s"), lRobotScore, m_lRobotScoreRange[0], lScore, m_lUserMaxScore[GetMeChairID()], pUserItem->GetNickName());
			WriteDebugInfo(cc);
			m_pIAndroidUserItem->PerformTakeScore(lScore);

		}
	}
	return;
}


void CAndroidUserItemSink::WriteDebugInfo(LPCTSTR pszString)
{
	//设置语言区域
	//char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	//setlocale(LC_CTYPE, "chs");

	DWORD dwAttrib = GetFileAttributes(_T(".//德州扑克日志目录"));
	if (INVALID_FILE_ATTRIBUTES == dwAttrib)
	{
		CreateDirectory(_T(".//德州扑克日志目录"), NULL);
	}

	CTime time = CTime::GetCurrentTime();
	static CString strFile;
	//if (strFile.IsEmpty())
	//	strFile.Format(_T(".//德州扑克日志目录//AI批次%d日志%d-%d-%d-%02d%02d%02d.log"),m_pIAndroidUserItem->GetAndroidParameter()->dwBatchID, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	CStdioFile myFile;
	BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		if (myFile.GetLength() > 50 * 1024 * 1024)//50M分文件 
		{
			myFile.Close();
			strFile.Format(_T(".//德州扑克日志目录//AI日志%d-%d-%d-%02d%02d%02d.log"),time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
			if (bOpen == false)
				return;
		}

		CString strtip;
		strtip.Format(TEXT("房间号%d,%d/%d/%d_%d:%d:%d--%s\n"), m_pIAndroidUserItem->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pszString);

		myFile.SeekToEnd();
		myFile.WriteString(strtip);
		myFile.Flush();
		myFile.Close();
	}

	//还原区域设定
	//setlocale(LC_CTYPE, old_locale);
	//free(old_locale);


}

bool CAndroidUserItemSink::IsMeWinner()
{
	if (m_UserWinList.bSameCount == 0)
		return false;

	for (BYTE i = 0; i < m_UserWinList.bSameCount; i++)
	{
		if (m_UserWinList.wWinerList[i] == GetMeChairID())
			return true;
	}

	return false;
}

bool CAndroidUserItemSink::AddScore(LONGLONG lscore)
{
	CMD_C_AddScore pack;
	pack.lScore = lscore;
	m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &pack, sizeof(pack));

	if (lscore > m_lTurnLessScore)
		m_bAddedScoreThisTurn = true;

	return true;
}

bool CAndroidUserItemSink::GiveUp()
{
	m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP);
	return true;
}

LONGLONG CAndroidUserItemSink::GetAddScore()
{
	WORD wChair = GetMeChairID();
	LONGLONG lScore = 0;
	if (m_cbCurCardWeight >= 60 && rand() % 100 < 30)
	{
		return m_lTurnMaxScore;//一定概率allin
	}

	if (m_lAddLessScore > m_lTurnMaxScore || m_bAddedScoreThisTurn /*|| m_cbCurCardWeight<10*/)//只能跟注或梭哈
	{
		lScore = m_lTurnLessScore;
	}
	else
	{
		LONGLONG lMaxScore = m_lTurnMaxScore*m_cbCurCardWeight / 100;
		if (lMaxScore > m_lTurnMaxScore)
			lMaxScore = m_lTurnMaxScore;
		if (lMaxScore < m_lTurnLessScore)
			lMaxScore = m_lTurnLessScore;

		lScore = m_lAddLessScore;

		if (lMaxScore > m_lAddLessScore)
			lScore += rand() % (lMaxScore - m_lAddLessScore) + m_lAddLessScore;
	}

	LONGLONG lLeftScore = m_lUserMaxScore[wChair] - lScore - m_lUserTotalScore[wChair];
	double dRate1 = lLeftScore / (double)m_lTurnMaxScore;
	double dRate2 = lLeftScore / (double)lScore;
	if (dRate1 <= 0.1 || dRate2 <= 0.2)
		lScore += lLeftScore;

	if (lScore < m_lTurnLessScore)
	{
		lScore = m_lTurnLessScore;
	}
	return lScore;
}

bool CAndroidUserItemSink::NeedGiveUp()
{
	if (m_cbCenterCardCount == 0)
	{
		if (m_cbCurCardWeight<5 && m_lUserTotalScore[GetMeChairID()]==0 && rand()%100 <40)
			return true;
		if (m_cbCurCardWeight < 10 && m_lUserTotalScore[GetMeChairID()]>0 && m_lUserTotalScore[GetMeChairID()] < m_lAnteScore + m_lCellScore && rand() % 100 < 20)
			return true;
		if (m_cbCurCardWeight<10 && m_lTurnLessScore>m_lCellScore + m_lAnteScore)
			return true;
	}
	else
	{
		if (m_cbCurCardWeight < 10)
			return true;
		if (m_cbCurCardWeight < 20 && m_lTurnLessScore + m_lUserTableScore[GetMeChairID()] >= m_lUserMaxScore[GetMeChairID()] / 5 && rand()%100<60)
			return true;
		if (m_cbCurCardWeight < 40 && m_bAddedScoreThisTurn && m_lTurnLessScore + m_lUserTableScore[GetMeChairID()] >= m_lUserMaxScore[GetMeChairID()] / 2 && rand()%100<40)
			return true;
	}


	return false;
}

//////////////////////////////////////////////////////////////////////////
DECLARE_CREATE_MODULE(AndroidUserItemSink);