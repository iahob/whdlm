#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//时间定义
	m_cbTimeStartGame = 0;
	m_cbTimeCallBanker = 0;
	m_cbTimeSetChip = 0;
	m_cbTimeRangeCard = 0;
	m_cbTimeShowCard = 0;

	//扑克变量
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbSegmentCard , sizeof(m_cbSegmentCard)) ;
	m_tagSegInfo.Init();
	m_bFinishSegment = false;

	//接口变量
	m_pIAndroidUserItem=NULL;

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
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbSegmentCard , sizeof(m_cbSegmentCard)) ;
	m_tagSegInfo.Init();
	m_bFinishSegment = false;

	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//开始游戏
		{
			//开始判断
			m_pIAndroidUserItem->SendUserReady(NULL,0);
			return true;
		}
	case IDI_DISPATCHCARDFINISH:  //发牌完毕
		{
			BYTE cbType = m_GameLogic.GetSpecialType(m_cbHandCardData, 13);
			if (cbType > CT_EX_INVALID)
			{
				BYTE cbSegCardData[3][5] = { 0 };
				if (0 != m_GameLogic.GetSpecialCardData(m_cbHandCardData, m_cbHandCardCount, cbSegCardData))
				{
					CMD_C_ShowCard ShowCard;
					ZeroMemory(&ShowCard, sizeof(ShowCard));
					ShowCard.cbSpecialCardType = cbType;

					CopyMemory(ShowCard.bFrontCard, cbSegCardData[0], sizeof(BYTE) * 3);
					m_GameLogic.SortCardList(ShowCard.bFrontCard, 3);

					CopyMemory(ShowCard.bMidCard, cbSegCardData[1], sizeof(BYTE) * 5);
					m_GameLogic.SortCardList(ShowCard.bMidCard, 5);

					CopyMemory(ShowCard.bBackCard, cbSegCardData[2], sizeof(BYTE) * 5);
					m_GameLogic.SortCardList(ShowCard.bBackCard, 5);

					m_pIAndroidUserItem->SendSocketData(SUB_C_SHOWARD, &ShowCard, sizeof(ShowCard));
					return true;
				}
			}

			//开始理牌
			ASSERT(m_cbTimeRangeCard != 0);
			UINT nElapse = rand() % (m_cbTimeRangeCard / 4) + TIME_LESS;

			m_pIAndroidUserItem->SetGameTimer(IDI_RANGE_CARD,nElapse);

			return true;
		}
	case IDI_RANGE_CARD:  //自动出牌
		{
			AutomaticOutCard();
			return true;
		}
	case IDI_COMPARECARD:
		{
			m_pIAndroidUserItem->SendSocketData(SUB_C_COMPLETE_COMPARE, NULL, 0);
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
	case SUB_S_SEND_CARD_EX:			//发送扑克
		{
			return OnSubSendCardEx(pData,wDataSize);
		}
	case SUB_S_COMPARE_CARD:			//比较扑克
		{
			return OnSubCompareCard(pData , wDataSize) ;
		}
	case SUB_S_GAME_END:				//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_ANDROID_INFO:
	{
		//效验数据
		if (wDataSize != sizeof(tagCustomAndroid)) return false;

		//消息处理
		tagCustomAndroid * pAndroidInfo = (tagCustomAndroid *)pData;

		m_lRobotScoreRange[0] = pAndroidInfo->lRobotScoreMin;
		m_lRobotScoreRange[1] = pAndroidInfo->lRobotScoreMax;

		if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])
			m_lRobotScoreRange[1] = m_lRobotScoreRange[0];

		m_lRobotBankGetScore = pAndroidInfo->lRobotBankGet;
		m_lRobotBankGetScoreBanker = pAndroidInfo->lRobotBankGetBanker;
		m_nRobotBankStorageMul = pAndroidInfo->lRobotBankStoMul;

		if (m_nRobotBankStorageMul < 0 || m_nRobotBankStorageMul>100)
			m_nRobotBankStorageMul = 20;

		//银行操作
		BankOperate(2);

		return true;
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
	case GS_WK_FREE:	//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

			//时间定义
			m_cbTimeStartGame = pStatusFree->cbTimeStartGame;
			m_cbTimeCallBanker = pStatusFree->cbTimeCallBanker;
			m_cbTimeSetChip = pStatusFree->cbTimeSetChip;
			m_cbTimeRangeCard = pStatusFree->cbTimeRangeCard;
			m_cbTimeShowCard = pStatusFree->cbTimeShowCard;
			m_GameLogic.SetTongHuaMode(pStatusFree->cbTongHuaMode);
			//房间标识
			m_wServerID = pStatusFree->wServerID;
			//玩家设置
			if (pIServerUserItem->GetUserStatus() != US_READY)
			{

				UINT nElapse = rand() % TIMER_START_GAME + TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);
			}

			return true;
		}
	case GS_WK_PLAYING: //游戏状态
		{
			//验证参数
			ASSERT( sizeof( CMD_S_StatusPlay ) == wDataSize ) ;
			if ( sizeof( CMD_S_StatusPlay ) != wDataSize ) return false ;

			CMD_S_StatusPlay *pStatusPlay = (CMD_S_StatusPlay *)pData;

			//时间定义
			m_cbTimeStartGame = pStatusPlay->cbTimeStartGame;
			m_cbTimeCallBanker = pStatusPlay->cbTimeCallBanker;
			m_cbTimeSetChip = pStatusPlay->cbTimeSetChip;
			m_cbTimeRangeCard = pStatusPlay->cbTimeRangeCard;
			m_cbTimeShowCard = pStatusPlay->cbTimeShowCard;
			m_GameLogic.SetTongHuaMode(pStatusPlay->cbTongHuaMode);
			//房间标识
			m_wServerID = pStatusPlay->wServerID;
			m_cbUserCount=pStatusPlay->cbPlayCount;

			CopyMemory(m_cbHandCardData , pStatusPlay->bHandCardData , sizeof(m_cbHandCardData));
			m_cbHandCardCount = HAND_CARD_COUNT;
			CopyMemory(m_cbSegmentCard , pStatusPlay->bSegmentCard , sizeof(m_cbSegmentCard));
			//玩家设置
			if (m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus() == US_PLAYING )
			{
				WORD wMeChairID = m_pIAndroidUserItem->GetMeUserItem()->GetChairID();
				if (pStatusPlay->bFinishSegment[wMeChairID] == false)
				{
					UINT nElapse = 5;// rand() % (m_cbTimeRangeCard / 4 + TIME_LESS;
					m_pIAndroidUserItem->SetGameTimer(IDI_RANGE_CARD, nElapse);
				}				
			}
			return true;
		}
	}
	ASSERT(false);
	return true;
}

//用户进入
VOID CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
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
bool CAndroidUserItemSink::OnSubSendCardEx( const void * pBuffer, WORD wDataSize )
{
	if (m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus() != US_PLAYING )
	{
		return true;
	}

	ASSERT(wDataSize == sizeof(CMD_S_SendCard));
	if (wDataSize != sizeof(CMD_S_SendCard)) return false;

	//变量定义
	CMD_S_SendCard * pSendCard = (CMD_S_SendCard *)pBuffer;
	IServerUserItem * pIServerUserItem = m_pIAndroidUserItem->GetMeUserItem();

	m_cbUserCount=pSendCard->cbPlayCount;

	//设置扑克
	if (pIServerUserItem->GetChairID() == pSendCard->wCurrentUser)
	{
		m_cbHandCardCount = HAND_CARD_COUNT;
		CopyMemory(m_cbHandCardData, pSendCard->bCardData, sizeof(BYTE)*m_cbHandCardCount);
		m_GameLogic.SortCardList(m_cbHandCardData, m_cbHandCardCount);

		//洗牌时间
		UINT nElapse = 6+(m_cbUserCount-2);
		m_pIAndroidUserItem->SetGameTimer(IDI_DISPATCHCARDFINISH, max(nElapse,TIME_DISPATCH_CARD));
	}

	return true;
}

//自动代打
void CAndroidUserItemSink::AutomaticOutCard()
{
	BYTE cbHandCard[MAX_COUNT] = { 0 };
	CopyMemory(cbHandCard, m_cbHandCardData, m_cbHandCardCount);
	m_GameLogic.SortCardList(cbHandCard, m_cbHandCardCount, enDescend);

	m_tagSegInfo.Init();


	//自动摆牌
	BYTE bAutoPutCard[13];
	ZeroMemory(bAutoPutCard, sizeof(bAutoPutCard));

	m_GameLogic.AutoPutCard(cbHandCard, bAutoPutCard, false, false);

	CMD_C_ShowCard ShowCard;
	ZeroMemory(&ShowCard, sizeof(ShowCard));
	CopyMemory(ShowCard.bFrontCard, &bAutoPutCard[0], 3);
	CopyMemory(ShowCard.bMidCard, &bAutoPutCard[3], 5);
	CopyMemory(ShowCard.bBackCard, &bAutoPutCard[8], 5);

	m_pIAndroidUserItem->SendSocketData(SUB_C_SHOWARD, &ShowCard, sizeof(ShowCard)) ;

}


void CAndroidUserItemSink::GetCardIndex( BYTE cbSrcIdx[], BYTE cbDstCard[], BYTE cbDstLen)
{
	if (m_cbHandCardCount < cbDstLen)
	{
		return;
	}
	for (int i = 0 ; i < cbDstLen; i++)
	{
		cbDstCard[i] = m_cbHandCardData[cbSrcIdx[i]];
	}

}

bool CAndroidUserItemSink::OnSubGameEnd( const void * pBuffer, WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	

	//游戏状态
	m_pIAndroidUserItem->SetGameStatus(GS_WK_FREE);

	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
	m_pIAndroidUserItem->KillGameTimer(IDI_COMPARECARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_RANGE_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_DISPATCHCARDFINISH);

	//扑克变量
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbSegmentCard , sizeof(m_cbSegmentCard)) ;
	m_tagSegInfo.Init();
	m_bFinishSegment = false;


	UINT nElapse=rand()%TIMER_START_GAME+TIME_LESS+m_cbUserCount*4;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	return true;
}


bool CAndroidUserItemSink::OnSubCompareCard( const void * pBuffer , WORD wDataSize )
{
	if (m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus() != US_PLAYING )
	{
		return true;
	}

	//数据验证
	ASSERT(sizeof(CMD_S_CompareCard)==wDataSize) ;
	if(sizeof(CMD_S_CompareCard)!=wDataSize) return false ;

	CMD_S_CompareCard* pCMD_S_CompareCard = (CMD_S_CompareCard*)pBuffer;

	UINT nElapse = TIME_LESS + pCMD_S_CompareCard->cbPlayCount * 3;
	m_pIAndroidUserItem->SetGameTimer(IDI_COMPARECARD, nElapse);
	return true;
}

//读取配置
void CAndroidUserItemSink::ReadConfigInformation()
{
	return;

	//设置文件名
	TCHAR szPath[MAX_PATH] = TEXT("");
	TCHAR szConfigFileName[MAX_PATH] = TEXT("");
	TCHAR OutBuf[255] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	myprintf(szConfigFileName, sizeof(szConfigFileName), _T("%s\\Thirteen.ini"), szPath);
	CString strServerID;
	strServerID.Format(TEXT("%d"),m_wServerID);
	
	//分数限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(strServerID, TEXT("RobotScoreMin"), _T("100000"), OutBuf, 255, szConfigFileName);
	_snwscanf_s(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lRobotScoreRange[0]);

	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(strServerID, TEXT("RobotScoreMax"), _T("50000000"), OutBuf, 255, szConfigFileName);
    _snwscanf_s(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lRobotScoreRange[1]);
	if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])	m_lRobotScoreRange[1] = m_lRobotScoreRange[0];

	//提款数额
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(strServerID, TEXT("RobotBankGet"), _T("1000000"), OutBuf, 255, szConfigFileName);
    _snwscanf_s(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lRobotBankGetScore);

	//提款数额 (庄家)
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(strServerID, TEXT("RobotBankGetBanker"), _T("10000000"), OutBuf, 255, szConfigFileName);
    _snwscanf_s(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lRobotBankGetScoreBanker);

	//存款倍数
	m_nRobotBankStorageMul = GetPrivateProfileInt(strServerID, _T("RobotBankStoMul"), 20, szConfigFileName);

	if (m_nRobotBankStorageMul<0||m_nRobotBankStorageMul>100) m_nRobotBankStorageMul =20;

	return;
}

//银行操作
void CAndroidUserItemSink::BankOperate(BYTE cbType)
{
	IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	//变量定义
	LONGLONG lRobotScore = pUserItem->GetUserScore();
	
	//判断存取
	if (lRobotScore > m_lRobotScoreRange[1])
	{			
		LONGLONG lSaveScore=0L;
		lSaveScore = LONGLONG(lRobotScore*m_nRobotBankStorageMul/100);
		if (lSaveScore > lRobotScore) lSaveScore = lRobotScore;
		if (lSaveScore > 0) m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
	}
	else if (lRobotScore < m_lRobotScoreRange[0])
	{
		SCORE lScore=rand()%m_lRobotBankGetScoreBanker+m_lRobotBankGetScore;
		if (lScore > 0)
			m_pIAndroidUserItem->PerformTakeScore(lScore);
	}
	return;
}

void CAndroidUserItemSink::WriteDebugInfo(LPCTSTR pszString)
{
	//设置语言区域
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CString strtip;
	CTime time = CTime::GetCurrentTime();
	strtip.Format(TEXT("%d/%d/%d_%d:%d:%d--%s\n"),time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pszString);

	CString strFile;
	strFile.Format(_T("浙江十三水机器日志.log"));
	CStdioFile myFile;
	BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		myFile.SeekToEnd();
		myFile.WriteString(strtip);
		myFile.Flush();
		myFile.Close();
	}

	//还原区域设定
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}
//////////////////////////////////////////////////////////////////////////
DECLARE_CREATE_MODULE(AndroidUserItemSink);