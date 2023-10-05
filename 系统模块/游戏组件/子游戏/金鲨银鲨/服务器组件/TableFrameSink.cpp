#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////

//空闲时间
#define IDI_FREE					1									//空闲时间
#define TIME_FREE					10									//空闲时间

//下注时间
#define IDI_BET						2									//下注时间
#ifdef _DEBUG
	#define TIME_BET				20									//下注时间
#else
	#define TIME_BET				30									//下注时间
#endif 


//结束时间
#define IDI_END						3									//结束时间
#define TIME_END					17									//结束时间


LONGLONG		CTableFrameSink::m_lStorageStart = 0L;
LONGLONG		CTableFrameSink::m_lStorageDeduct = 1L;

LONGLONG         g_lChipArray[6] = { 1, 2, 5, 10, 20, 50 };

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//下注数
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory(m_HPlayBet,sizeof(m_HPlayBet));
	ZeroMemory(m_FPlayBet,sizeof(m_FPlayBet));
	ZeroMemory(m_APlayBet,sizeof(m_APlayBet));
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

	//玩家分数
	ZeroMemory(m_dwDrawStartTime, sizeof(m_dwDrawStartTime));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	ZeroMemory(m_lPlayOriginalScore, sizeof(m_lPlayOriginalScore));

	//筹码
	ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
	ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
	ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
	ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

	//转盘路径
	m_bTurnTwoTime = FALSE;
	ZeroMemory(m_nTurnTableTarget, sizeof(m_nTurnTableTarget));

	//转盘倍数
	m_nLastMultipleIndex = 0;
	ZeroMemory(m_nAnimalMultiple, sizeof(m_nAnimalMultiple));
	
	//生成几率
	ZeroMemory(m_nGenerationChance, sizeof(m_nGenerationChance));

	//状态变量
	m_dwBetTime=0L;

	//时间设置
	m_cbFreeTime = TIME_FREE;
	m_cbBetTime = TIME_BET;
	m_cbEndTime = TIME_END;

	//切换设置
	m_lAddScore[0]=1000;
	m_lAddScore[1]=10000;
	m_lAddScore[2]=100000;
	m_lAddScore[3]=500000;
	//设置变量
	m_lAreaLimitScore = 0;
	m_lPlayLimitScore = 0;

	//服务控制
	m_hInst = NULL;
	m_pServerContro = NULL;
	m_bControl = FALSE;
	m_hInst = LoadLibrary(TEXT("SharkBattleServerDebug.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}

	m_bIsCheat = false;
	m_lRobotloseMax = 0;
	m_lRobotLoseMaxChange = 0;
	m_btRobotLoseChangePercent = 0;
	m_lRealBankerRobotLoseMax = 0;
	m_lRobotProbabilityTanking = 0;
	m_lRobotDefineTanking = 0;
	m_btRobotTankingPercent = 0;
	m_lRealBankerRobotLoseMaxChange = 0;
	m_btRealBankerRobotLoseChangePercent = 0;
	m_btUsuallyChangePercent = 0;
	m_btRobotWinScorePercent = 0;
	m_lRobotWinChiefScore = 0;
	m_lRobotResultScore = 0;
	m_lRobotAchievement = 0;

	m_IsCheatUser = FALSE;
	m_dwUserID = 0;
	m_dwUserLoseWinPercent = 0;
	m_bLoseWin = FALSE;
	m_lWinMaxGold = 0;
	m_lLoseMaxGold = 0;
	m_lAllUserGold = 0;
	m_lUserScore = 0;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
}
//释放对象
VOID  CTableFrameSink::Release()
{
	if( m_pServerContro )
	{
		delete m_pServerContro;
		m_pServerContro = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}

	delete this; 

}
//接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//配置桌子
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	//错误判断
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"),TraceLevel_Exception);
		return false;
	}

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	//设置文件名
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\SharkBattleControl\\SharkBattle.ini"),szPath);
	_sntprintf(m_szConfigFilePath,sizeof(m_szConfigFilePath),TEXT("%s\\SharkBattleControl\\User.ini"),szPath);
	_sntprintf(m_szGameRoomName, sizeof(m_szGameRoomName), TEXT("%d"),m_pGameServiceOption->wServerID);

	ReadConfigInformation();

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	WriteGameINI();
	m_lRobotAchievement = 0;
	m_lUserScore = 0;

	//下注数
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory(m_HPlayBet,sizeof(m_HPlayBet));
	ZeroMemory(m_FPlayBet,sizeof(m_FPlayBet));
	ZeroMemory(m_APlayBet,sizeof(m_APlayBet));
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

	//筹码
	ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
	ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
	//ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
	ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

	//转盘路径
	m_bTurnTwoTime = FALSE;
	ZeroMemory(m_nTurnTableTarget, sizeof(m_nTurnTableTarget));

	//转盘倍数
	//m_nLastMultipleIndex = 0;
	ZeroMemory(m_nAnimalMultiple, sizeof(m_nAnimalMultiple));

	m_bControl = FALSE;

	return;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL://正常结束
		{
			//结束游戏
			int nPrizesMultiple = GameOver();

			//发送结束消息
			CMD_S_GameEnd CMDGameEnd;
			ZeroMemory(&CMDGameEnd, sizeof(CMDGameEnd));
			CMDGameEnd.cbTimeLeave = m_cbEndTime;
			CMDGameEnd.bTurnTwoTime = m_bTurnTwoTime;
			CMDGameEnd.nPrizesMultiple = nPrizesMultiple;
			CMDGameEnd.lPlayShowPrizes = rand()*85 + 15;

			//波波
			CMDGameEnd.lServiceScore = 0;

			CopyMemory(CMDGameEnd.nTurnTableTarget, m_nTurnTableTarget, sizeof(CMDGameEnd.nTurnTableTarget));
			for (int i = 0; i < GAME_PLAYER; ++i )
			{
				IServerUserItem* pServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if ( pServerUserItem == NULL || !pServerUserItem->IsClientReady() )
					continue;

				CMDGameEnd.lPlayWin[0] = m_lPlayWinScore[i][0];
				CMDGameEnd.lPlayWin[1] = m_lPlayWinScore[i][1];
				CMDGameEnd.lPlayPrizes = m_lPlayWinPrizes[i];
				//波波
				CMDGameEnd.lGameScore = pServerUserItem->GetUserScore();
				m_pITableFrame->SendUserItemData(pServerUserItem, SUB_S_GAME_END, &CMDGameEnd, sizeof(CMDGameEnd));
			}

			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END, &CMDGameEnd, sizeof(CMDGameEnd));

			return true;
		}
	case GER_NETWORK_ERROR:		//网络中断
	case GER_USER_LEAVE:		//用户离开
		{
			//非结算状态， 返还下注金币
			if (m_pITableFrame->GetGameStatus() != GAME_SCENE_END)
			{
				//定义消息
				CMD_S_BetClear CMDBetClear;
				CMDBetClear.wChairID = wChairID;
				//_tcscpy(CMDBetClear.szPlayerName, pIServerUserItem->GetUserInfo()->szNickName);
				CopyMemory(CMDBetClear.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDBetClear.lPlayBet));

				//清除下注
				BOOL bSendMessage = FALSE;
				for ( int i = 0; i < ANIMAL_MAX; ++i )
				{
					if ( m_lPlayBet[wChairID][i] > 0 )
					{
						bSendMessage = TRUE;
						m_lAllBet[i] -= m_lPlayBet[wChairID][i];
						m_lPlayBet[wChairID][i] = 0;
					}
				}

				//发送消息
				if ( bSendMessage )
				{
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
				}
				return true;
			}

			//扣除筹码
			m_lPlayScore[wChairID] += m_lPlayWinScore[wChairID][0]+m_lPlayWinScore[wChairID][1];

			if ( m_lPlayScore[wChairID] < 0 )
			{
				ASSERT(FALSE);
				m_lPlayScore[wChairID] = 0;
			}

			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if(pIServerUserItem != NULL)
				UpdataScore(wChairID, pIServerUserItem);
	
			return true;
		}
	}
	return false;
}

void CTableFrameSink::UpdataScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{

	if(pIServerUserItem == NULL) return;
	//写分
	LONGLONG lScore = m_lPlayScore[wChairID] - m_lPlayOriginalScore[wChairID];

	m_lPlayOriginalScore[wChairID] = m_lPlayScore[wChairID];

	if ( lScore != 0 )
	{
		//防止负分
		if ( lScore < 0 && (-lScore) > pIServerUserItem->GetUserScore() )
		{
			lScore = -pIServerUserItem->GetUserScore();
		}

		//计算税收
		LONGLONG lRevenue = 0;
		if ( lScore > 0 )
		{
			lRevenue = m_pITableFrame->CalculateRevenue(wChairID, lScore);
			lScore -= lRevenue;
		}

		//写分
		tagScoreInfo ScoreInfo;
		ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
		ScoreInfo.lScore = lScore;
		ScoreInfo.lRevenue = lRevenue;
		if ( lScore > 0 )
			ScoreInfo.cbType = SCORE_TYPE_WIN;
		else if ( lScore < 0 )
			ScoreInfo.cbType = SCORE_TYPE_LOSE;
		else
			ScoreInfo.cbType = SCORE_TYPE_DRAW;

		m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);
	}
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch( cbGameStatus )
	{
	case GAME_SCENE_FREE:
		{
			//变量定义
			CMD_S_StatusFree CMDStatusFree;
			ZeroMemory(&CMDStatusFree,sizeof(CMDStatusFree));

			//时间信息
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			CMDStatusFree.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));

			//变量信息
			//CMDStatusFree.lPlayScore = pIServerUserItem->GetUserScore();
			CMDStatusFree.lStorageStart = m_lStorageStart;

			CMDStatusFree.lAreaLimitScore = m_lAreaLimitScore;
			CMDStatusFree.lPlayLimitScore = m_lPlayLimitScore;
			//CMDStatusFree.szGameRoomID=m_pGameServiceOption->wServerID;

			//游戏记录
			for ( int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				CMDStatusFree.nTurnTableRecord[i] = INT_MAX;
				if( i < m_ArrayTurnTableRecord.GetCount() )
					CMDStatusFree.nTurnTableRecord[i] = m_ArrayTurnTableRecord[i];
			}

			CMDStatusFree.lStartStorage = m_lStorageStart;
			CMDStatusFree.lCellScore = 1;
			CopyMemory(CMDStatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(CMDStatusFree.szGameRoomName));
			CMDStatusFree.bGenreEducate = false;
			CopyMemory(CMDStatusFree.dChipArray, g_lChipArray, sizeof(g_lChipArray));

			//发送切换数量
			SendSwitchNum(pIServerUserItem);
			//发送场景
			bool bReturn = m_pITableFrame->SendGameScene(pIServerUserItem,&CMDStatusFree,sizeof(CMDStatusFree));
			return bReturn;
		}
		break;
	case GAME_SCENE_BET:
		{
			//变量定义
			CMD_S_StatusPlay CMDStatusPlay;
			ZeroMemory(&CMDStatusPlay,sizeof(CMDStatusPlay));

			//时间信息
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			CMDStatusPlay.cbTimeLeave = (BYTE)(m_cbBetTime - __min(dwPassTime, m_cbBetTime));

			//变量信息
			CMDStatusPlay.lStorageStart = m_lStorageStart;

			CMDStatusPlay.lAreaLimitScore = m_lAreaLimitScore;
			CMDStatusPlay.lPlayLimitScore = m_lPlayLimitScore;

			CopyMemory(CMDStatusPlay.nAnimalMultiple, m_nAnimalMultiple, sizeof(CMDStatusPlay.nAnimalMultiple));
			CopyMemory(CMDStatusPlay.lAllBet, m_lAllBet, sizeof(CMDStatusPlay.lAllBet));
			CopyMemory(CMDStatusPlay.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDStatusPlay.lPlayBet));

			//游戏记录
			for (int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				CMDStatusPlay.nTurnTableRecord[i] = INT_MAX;
				if(i < m_ArrayTurnTableRecord.GetCount() )
					CMDStatusPlay.nTurnTableRecord[i] = m_ArrayTurnTableRecord[i];
			}

			CMDStatusPlay.lStartStorage = m_lStorageStart;
			CMDStatusPlay.lCellScore = 1;
			CopyMemory(CMDStatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(CMDStatusPlay.szGameRoomName));
			CMDStatusPlay.bGenreEducate = false;
			CopyMemory(CMDStatusPlay.dChipArray, g_lChipArray, sizeof(g_lChipArray));

			//发送切换数量
			SendSwitchNum(pIServerUserItem);
			//发送场景
			bool bReturn = m_pITableFrame->SendGameScene(pIServerUserItem,&CMDStatusPlay,sizeof(CMDStatusPlay));
			return bReturn;
		}
		break;
	case GAME_SCENE_END:
		{
			//变量定义
			CMD_S_StatusEnd CMDStatusEnd;
			ZeroMemory(&CMDStatusEnd,sizeof(CMDStatusEnd));

			//时间信息
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			CMDStatusEnd.cbTimeLeave = (BYTE)(m_cbEndTime - __min(dwPassTime, m_cbEndTime));

			//变量信息
			CMDStatusEnd.lStorageStart = m_lStorageStart;

			CMDStatusEnd.lAreaLimitScore = m_lAreaLimitScore;
			CMDStatusEnd.lPlayLimitScore = m_lPlayLimitScore;

			CopyMemory( CMDStatusEnd.nAnimalMultiple, m_nAnimalMultiple, sizeof(CMDStatusEnd.nAnimalMultiple));
			CopyMemory( CMDStatusEnd.lAllBet, m_lAllBet, sizeof(CMDStatusEnd.lAllBet));
			CopyMemory( CMDStatusEnd.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDStatusEnd.lPlayBet));

			//游戏记录
			for ( int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				CMDStatusEnd.nTurnTableRecord[i] = INT_MAX;
				if( i < m_ArrayTurnTableRecord.GetCount() )
					CMDStatusEnd.nTurnTableRecord[i] = m_ArrayTurnTableRecord[i];
			}

			CMDStatusEnd.lStartStorage = m_lStorageStart;
			CMDStatusEnd.lCellScore = 1;
			CopyMemory(CMDStatusEnd.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(CMDStatusEnd.szGameRoomName));
			CMDStatusEnd.bGenreEducate = false;
			CopyMemory(CMDStatusEnd.dChipArray, g_lChipArray, sizeof(g_lChipArray));

			//发送切换数量
			SendSwitchNum(pIServerUserItem);
			//发送场景
			bool bReturn = m_pITableFrame->SendGameScene(pIServerUserItem,&CMDStatusEnd,sizeof(CMDStatusEnd));
			return bReturn;
		}
		break;
	}
	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE://开始游戏
		{
			m_pITableFrame->StartGame();
			//-------------------------------------------------------
			//设置时间
			m_dwBetTime = (DWORD)time(NULL);

			//显示库存
			//static int	nCurrentHour = 0;							//显示库存
			//CTime time = CTime::GetCurrentTime();					//获取当前时间. 
			//if ( time.GetHour() != nCurrentHour )
			//{
			//	TCHAR szServiceInfo[128] = {0};
			//	_sntprintf(szServiceInfo,sizeof(szServiceInfo), TEXT("当前库存量：%I64d"), m_lStorageStart);
			//	CTraceService::TraceString(szServiceInfo,TraceLevel_Info);
			//	nCurrentHour = time.GetHour();
			//}

			//消息定义
			CMD_S_GameStart CMDGameStart;
			ZeroMemory(&CMDGameStart,sizeof(CMDGameStart));

			//设置变量
			CMDGameStart.cbTimeLeave = m_cbBetTime;
			CMDGameStart.lStorageStart = m_lStorageStart;

			//生产倍数
			int nAnimalMultiple[2][ANIMAL_MAX] = 
			{
				{ 12,  8,  8,  6, 12,  8,  8,  6, 24, 24,  2,  2},
				{ 24, 12,  8,  4, 24, 12,  8,  4, 24, 24,  2,  2},
			};

			int nLastMultipleIndex = m_nLastMultipleIndex;
			do 
			{
				m_nLastMultipleIndex = rand()%2;
			} while ( nLastMultipleIndex == m_nLastMultipleIndex );
			CopyMemory(m_nAnimalMultiple, nAnimalMultiple[m_nLastMultipleIndex], sizeof(m_nAnimalMultiple));
			CopyMemory(CMDGameStart.nAnimalMultiple, m_nAnimalMultiple, sizeof(CMDGameStart.nAnimalMultiple));
			
			
			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, &CMDGameStart, sizeof(CMDGameStart));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START, &CMDGameStart, sizeof(CMDGameStart));

			//开启定时器
			m_pITableFrame->SetGameTimer(IDI_BET, m_cbBetTime*1000, 1, NULL);

			//设置状态
			m_pITableFrame->SetGameStatus(GAME_SCENE_BET);

			return true;
		}
	case IDI_BET:		//下注时间
		{
			//状态判断(防止强退重复设置)
			if ( m_pITableFrame->GetGameStatus() != GAME_SCENE_END )
			{
				//设置状态
				m_pITableFrame->SetGameStatus(GAME_SCENE_END);

				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
				//设置时间
				m_dwBetTime = (DWORD)time(NULL);
				if ( CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]) == ANIMAL_GOLD_SHARK )
				{
					m_pITableFrame->SetGameTimer(IDI_END, (m_cbEndTime + 30)*1000, 1, NULL);	
				}
				else if( CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]) == ANIMAL_SLIVER_SHARK )
				{
					m_pITableFrame->SetGameTimer(IDI_END, (m_cbEndTime + 17)*1000, 1, NULL);	
				}
				else
				{
					m_pITableFrame->SetGameTimer(IDI_END, m_cbEndTime*1000, 1, NULL);	
				}	
			}
			return true;
		}
	case IDI_END:				//结束游戏
		{
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			//写入积分
			/*
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) continue;

				ScoreInfo[wUserChairID].cbType=(m_lPlayWinChip[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
				ScoreInfo[wUserChairID].lRevenue=m_pITableFrame->CalculateRevenue(wUserChairID, m_lPlayWinChip[wUserChairID]);
				ScoreInfo[wUserChairID].lScore=m_lPlayWinChip[wUserChairID];

				}
			}
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));*/
			//结束游戏
			m_dwBetTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
			m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,0L);
			GameOverWriteScore();

			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree,sizeof(GameFree));
			GameFree.cbTimeLeave = m_cbFreeTime;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));

			m_pITableFrame->ConcludeGame(GER_NORMAL);	
			m_bRefreshCfg = GetPrivateProfileInt(m_szGameRoomName, TEXT("Refresh"), 0, m_szConfigFileName);
			//读取配置
			if (m_bRefreshCfg)
				ReadConfigInformation();
			return true;
		}
	}

	return false;
}

//数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return false;
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch(wSubCmdID)
	{
	case SUB_C_SWITCT_NUM:{
		SendSwitchNum(pIServerUserItem);
		return true;}
	case SUB_C_PLAY_BET:			//用户下注
		return OnSubPlayBet( pData, wDataSize, pIServerUserItem );
	case SUB_C_BET_CLEAR:			//清除下注
		return OnSubBetClear( pData, wDataSize, pIServerUserItem );
	//case SUB_C_AMDIN_COMMAND:
	//	{
	//		
	//		ASSERT(wDataSize==sizeof(CMD_C_AdminReq));
	//		if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

	//		//权限判断
	//		if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())==false)
	//			return false;

	//		
	//		if ( m_pServerContro == NULL)
	//			return true;

	//		const CMD_C_AdminReq* AdminReq=static_cast<const CMD_C_AdminReq*>(pData);
	//		if(AdminReq->cbReqType == RQ_SET_WIN_AREA)
	//			m_bControl = TRUE;
	//		
	//		return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame);
	//	}
	//case SUB_C_UPDATE_STORAGE:
	//	{
	//		ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
	//		if(wDataSize!=sizeof(CMD_C_UpdateStorage)) return false;

	//		//权限判断
	//		if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
	//			return false;

	//		//消息处理
	//		CMD_C_UpdateStorage * pUpdateStorage=(CMD_C_UpdateStorage *)pData;
	//		if (pUpdateStorage->cbReqType==RQ_SET_STORAGE)
	//		{
	//			OutputDebugString(TEXT("RQ_SET_STORAGE"));
	//			m_lStorageStart=pUpdateStorage->lStorage;
	//			m_lStorageDeduct=pUpdateStorage->lStorageDeduct;
	//		}

	//		CMD_S_UpdateStorage updateStorage;
	//		updateStorage.lStorage=m_lStorageStart;
	//		updateStorage.lStorageDeduct=m_lStorageDeduct;

	//		m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
	//		return true;
	//	}
	}
	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (wSubCmdID < SUB_GF_FRAME_MESSAG_GAME_MIN || wSubCmdID > SUB_GF_FRAME_MESSAG_GAME_MAX)
		return false;

	switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
	{
			case SUB_C_AMDIN_COMMAND:
		{
			
			ASSERT(wDataSize==sizeof(CMD_C_AdminReq));
			if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

			//权限判断
			if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())==false)
				return false;

			
			if ( m_pServerContro == NULL)
				return true;

			const CMD_C_AdminReq* AdminReq=static_cast<const CMD_C_AdminReq*>(pData);
			if(AdminReq->cbReqType == RQ_SET_WIN_AREA)
				m_bControl = TRUE;
			
			return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame);
		}
	case SUB_C_UPDATE_STORAGE:
		{
			ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
			if(wDataSize!=sizeof(CMD_C_UpdateStorage)) return false;

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			//消息处理
			CMD_C_UpdateStorage * pUpdateStorage=(CMD_C_UpdateStorage *)pData;
			if (pUpdateStorage->cbReqType==RQ_SET_STORAGE)
			{
				OutputDebugString(TEXT("RQ_SET_STORAGE"));
				m_lStorageStart=pUpdateStorage->lStorage;
				m_lStorageDeduct=pUpdateStorage->lStorageDeduct;
			}

			CMD_S_UpdateStorage updateStorage;
			updateStorage.lStorage=m_lStorageStart;
			updateStorage.lStorageDeduct=m_lStorageDeduct;

			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_UPDATE_STORAGE, &updateStorage, sizeof(updateStorage));
			return true;
	    }
	}
	return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if ( bLookonUser )
		return true;

	if(m_dwBetTime == 0)
	{
		m_dwBetTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
	}

	//设置变量
	m_dwDrawStartTime[wChairID] = (DWORD)time(NULL);
	m_lPlayOriginalScore[wChairID] = pIServerUserItem->GetUserScore();
	m_lPlayScore[wChairID] = pIServerUserItem->GetUserScore();
	m_lPlayWinChip[wChairID] = 0;
	m_lPlayWinPrizes[wChairID] = 0;
	ZeroMemory(m_lPlayWinScore[wChairID], sizeof(LONGLONG)*2);
	ZeroMemory(m_lPlayWinAnimal[wChairID], sizeof(LONGLONG)*ANIMAL_MAX);

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if ( bLookonUser )
		return true;

	//设置变量
	m_dwDrawStartTime[wChairID] = 0;
	m_lPlayOriginalScore[wChairID] = 0;
	m_lPlayScore[wChairID] = 0;
	m_lPlayWinChip[wChairID] = 0;
	m_lPlayWinPrizes[wChairID] = 0;
	ZeroMemory(m_lPlayWinScore[wChairID], sizeof(LONGLONG)*2);
	ZeroMemory(m_lPlayWinAnimal[wChairID], sizeof(LONGLONG)*ANIMAL_MAX);

	////检测是否有人
	//for ( WORD i = 0; i < GAME_PLAYER; ++i )
	//{
	//	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
	//	if (pIServerUserItem == NULL) continue;

	//	return true;
	//}

	//结束游戏
	//m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

	return true;
}

//用户下注消息
bool CTableFrameSink::OnSubPlayBet(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验数据
	ASSERT( wDataSize == sizeof(CMD_C_PlayBet) );
	if ( wDataSize != sizeof(CMD_C_PlayBet) )
	{
		OutputDebugString(TEXT(">>>>>>>>>>>>>>>>>0"));
		return false;
	}
	//消息处理
	CMD_C_PlayBet * pPlayBet = (CMD_C_PlayBet *)pData;
	WORD wChairID = pIServerUserItem->GetChairID();

	//下注异常
	if ( pPlayBet->nAnimalIndex < 0 || pPlayBet->nAnimalIndex >= ANIMAL_MAX || pPlayBet->lBetChip > m_lPlayScore[wChairID] || pPlayBet->lBetChip < 0 )
	{
		OutputDebugString(TEXT(">>>>>>>>>>>>>>>>>1"));
		ASSERT(FALSE);
		return false;
	}

	//下注为0
	if( pPlayBet->lBetChip == 0 ) 
	{
		OutputDebugString(TEXT(">>>>>>>>>>>>>>>>>2"));

		ASSERT(FALSE);
		return true;
	}
	//判断超额
	if ( (m_lAreaLimitScore != 0 && m_lAllBet[pPlayBet->nAnimalIndex] + pPlayBet->lBetChip > m_lAreaLimitScore)
		|| (m_lPlayLimitScore != 0 && m_lPlayBet[wChairID][pPlayBet->nAnimalIndex] + pPlayBet->lBetChip > m_lPlayLimitScore)
		|| ( m_pITableFrame->GetGameStatus() != GAME_SCENE_BET ) )
	{
		OutputDebugString(TEXT(">>>>>>>>>>>>>>>>>3"));

		//下注失败
		CMD_S_PlayBetFail CMDPlayBetFail;
		CMDPlayBetFail.wChairID = wChairID;
		CMDPlayBetFail.nAnimalIndex = pPlayBet->nAnimalIndex;
		CMDPlayBetFail.lBetChip = pPlayBet->lBetChip;
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_PLAY_BET_FAIL, &CMDPlayBetFail, sizeof(CMDPlayBetFail));
		return true;
	}
	//下注成功
	m_lPlayScore[wChairID] -= pPlayBet->lBetChip;
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
	//if (pIServerUserItem && pIServerUserItem->GetUserInfo()->cbMemberLevel>0)
		m_HPlayBet[pPlayBet->nAnimalIndex]+=pPlayBet->lBetChip;
	if (pIServerUserItem && pIServerUserItem->IsAndroidUser())
		m_APlayBet[pPlayBet->nAnimalIndex]+=pPlayBet->lBetChip;
	//if (pIServerUserItem && pIServerUserItem->GetUserInfo()->cbMemberLevel==0)
		m_FPlayBet[pPlayBet->nAnimalIndex]+=pPlayBet->lBetChip;
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
	m_lPlayBet[wChairID][pPlayBet->nAnimalIndex] += pPlayBet->lBetChip;
	m_lAllBet[pPlayBet->nAnimalIndex] += pPlayBet->lBetChip;

	//发送消息
	CMD_S_PlayBet CMDPlayBet;
	CMDPlayBet.wChairID = wChairID;
	CMDPlayBet.nAnimalIndex = pPlayBet->nAnimalIndex;
	CMDPlayBet.lBetChip = pPlayBet->lBetChip;
	CMDPlayBet.bAndroid = false;
	if (pIServerUserItem)
	{
		/*memcpy(CMDPlayBet.szPlayerName, pIServerUserItem->GetUserInfo()->szNickName, LEN_NICKNAME);
		if(!pIServerUserItem->IsAndroidUser())
			CMDPlayBet.mHBet=pPlayBet->lBetChip;*/

		CMDPlayBet.bAndroid = pIServerUserItem->IsAndroidUser();
	}
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLAY_BET, &CMDPlayBet, sizeof(CMDPlayBet));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLAY_BET, &CMDPlayBet, sizeof(CMDPlayBet));

	//发送玩家下注信息
	if (!pIServerUserItem->IsAndroidUser())
	{
		//服务器处理下注数据  直接发送给作弊用户
		BYTE cbBuffer[SOCKET_TCP_BUFFER];
		ZeroMemory(cbBuffer, sizeof(cbBuffer));
		DWORD dwPaketSize = 0;

		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pServerUserItem == NULL) continue;
			if (pServerUserItem->IsAndroidUser()) continue;

			LONGLONG lUserAllBet = 0;
			for (BYTE j = 0; j < ANIMAL_MAX; j++)
			{
				lUserAllBet += m_lPlayBet[j][i];
				TCHAR szDes[200];
				_sntprintf(szDes, 200, TEXT("下注[%d]"), m_lPlayBet[j][i]);
				CTraceService::TraceString(szDes, TraceLevel_Debug);
			}
			if (0 == lUserAllBet) continue;

			//构造数据
			tagUserBet *UserBet = (tagUserBet*)(cbBuffer + dwPaketSize);
			CopyMemory(UserBet->szNickName, pServerUserItem->GetNickName(), sizeof(UserBet->szNickName));
			UserBet->dwUserGameID = pServerUserItem->GetGameID();
			UserBet->lUserStartScore = m_lPlayOriginalScore[i];
			UserBet->lUserWinLost = pServerUserItem->GetUserScore() - UserBet->lUserStartScore;
			for (BYTE k = 0; k < ANIMAL_MAX; k++)
			{
				UserBet->lUserBet[k] = m_lPlayBet[k][i];
				TCHAR szDes[200];
				_sntprintf(szDes, 200, TEXT("下注1[%d]"), m_lPlayBet[k][i]);
				CTraceService::TraceString(szDes, TraceLevel_Debug);
			}

			dwPaketSize += sizeof(tagUserBet);
		}
		CTraceService::TraceString(TEXT("发送下注"), TraceLevel_Debug);
		m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, cbBuffer, dwPaketSize);

	}

	return true;
}

//清除下注消息
bool CTableFrameSink::OnSubBetClear(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验数据
	ASSERT( wDataSize == NULL );
	if ( wDataSize != NULL ) return false;

	//消息处理
	WORD wChairID = pIServerUserItem->GetChairID();

	//定义消息
	CMD_S_BetClear CMDBetClear;
	CMDBetClear.wChairID = wChairID;
	//_tcscpy(CMDBetClear.szPlayerName, pIServerUserItem->GetUserInfo()->szNickName);
	CopyMemory(CMDBetClear.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDBetClear.lPlayBet));

	//清除下注
	BOOL bSendMessage = FALSE;
	for ( int i = 0; i < ANIMAL_MAX; ++i )
	{
		if ( m_lPlayBet[wChairID][i] > 0 )
		{
			bSendMessage = TRUE;
			m_lPlayScore[wChairID] += m_lPlayBet[wChairID][i];
			m_lAllBet[i] -= m_lPlayBet[wChairID][i];
			m_lPlayBet[wChairID][i] = 0;
		}
	}
	
	//发送消息
	if ( bSendMessage )
	{
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
	}
	
	return true;
}

//生成转盘
bool CTableFrameSink::CreateTableTable()
 {
	//设置变量
	m_bTurnTwoTime = FALSE;

	//生产第一张图
	int nWinTable = CGameLogic::GetInstance()->RandomArea(TURAN_TABLE_MAX, 
	m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3,
	m_nGenerationChance[ANIMAL_GOLD_SHARK]/4, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3,
	m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_GOLD_SHARK]/4,
	m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_RABBIT]/3,
	m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_GOLD_SHARK]/4, m_nGenerationChance[ANIMAL_SWALLOW]/3,
	m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_PIGEON]/2,
	m_nGenerationChance[ANIMAL_PIGEON]/2, m_nGenerationChance[ANIMAL_GOLD_SHARK]/4, m_nGenerationChance[ANIMAL_PEACOCK]/2, m_nGenerationChance[ANIMAL_PEACOCK]/2);

	
	////int nTemp[] = { 0, 4 };
	//static nWin = 0;
	//nWin = (nWin + 1)%CountArray(nTemp);
	//nWinTable = nTemp[nWin];
	//nWinTable = 4;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//m_nTurnTableTarget[0]//控制第一次现得动物
	m_nTurnTableTarget[0] = nWinTable;
	BYTE ProbilityRand=GetProbility();
	bool bControl = false;
	if( m_pServerContro != NULL && m_pServerContro->NeedControl() )//在此处加入控制
	{
		if(m_pServerContro->ControlResult(&m_nTurnTableTarget[0]))
		{
			bControl=true;
			m_pServerContro->CompleteControl();
			nWinTable=m_nTurnTableTarget[0];
			if ( CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_GOLD_SHARK || CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_SLIVER_SHARK ) 
			m_bTurnTwoTime = TRUE;
		}			
	}
	else if(rand()%100<ProbilityRand)
	{
		int minArea1=0;
		int minArea2=0;
		int minArea3=0;
		int minArea4=0;
		LONGLONG AreaBet[8];
		AreaBet[0]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_LION]*m_nAnimalMultiple[ANIMAL_LION];
		AreaBet[1]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_PANDA]*m_nAnimalMultiple[ANIMAL_PANDA];
		AreaBet[2]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_MONKEY]*m_nAnimalMultiple[ANIMAL_MONKEY];
		AreaBet[3]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_RABBIT]*m_nAnimalMultiple[ANIMAL_RABBIT];
		AreaBet[4]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_EAGLE]*m_nAnimalMultiple[ANIMAL_EAGLE];
		AreaBet[5]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PEACOCK]*m_nAnimalMultiple[ANIMAL_PEACOCK];
		AreaBet[6]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PIGEON]*m_nAnimalMultiple[ANIMAL_PIGEON];
		AreaBet[7]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_SWALLOW]*m_nAnimalMultiple[ANIMAL_SWALLOW];
		LONGLONG maxAreaBet=0;
		int maxArea=0;
		for	 (int i=0;i<8;i++)
		{
			if (maxAreaBet<=AreaBet[i])
			{
				maxAreaBet=AreaBet[i];
				maxArea=i;
			}
		}
		if (maxAreaBet!=0)
		{
			LONGLONG AreaBet1=AreaBet[maxArea];
			LONGLONG AreaBet2=AreaBet[maxArea];
			LONGLONG AreaBet3=AreaBet[maxArea];
			LONGLONG AreaBet4=AreaBet[maxArea];
			for	 (int i=0;i<8;i++)
			{
				if (AreaBet1>=AreaBet[i])
				{
					AreaBet1=AreaBet[i];
					minArea1=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet2>=AreaBet[i]&&i!=minArea1)
				{
					AreaBet2=AreaBet[i];
					minArea2=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet3>=AreaBet[i]&&i!=minArea1&&i!=minArea2)
				{
					AreaBet3=AreaBet[i];
					minArea3=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet4>=AreaBet[i]&&i!=minArea1&&i!=minArea2&&i!=minArea3)
				{
					AreaBet4=AreaBet[i];
					minArea4=i;
				}
			}
		/*	switch(rand()%4)
			{
			case 0:
				nWinTable=minArea1;
				break;
			case 1:
				nWinTable=minArea2;
				break;
			case 2:
				nWinTable=minArea3;
				break;
			case 3:
				nWinTable=minArea4;
				break;
			}*/
			nWinTable=minArea1;
			switch(nWinTable)
			{
			case 0:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=5;
						break;
					case 1:
						nWinTable=6;
						break;
					case 2:
						nWinTable=7;
						break;
					}
				}
				break;
			case 1:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=9;
						break;
					case 1:
						nWinTable=10;
						break;
					}
				}
				break;
			case 2:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=12;
						break;
					case 1:
						nWinTable=13;
						break;
					}
				}
				break;
			case 3:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=15;
						break;
					case 1:
						nWinTable=16;
						break;
					case 2:
						nWinTable=17;
						break;
					}
				}
				break;
			case 4:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=1;
						break;
					case 1:
						nWinTable=2;
						break;
					case 2:
						nWinTable=3;
						break;
					}
				}
				break;
			case 5:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=26;
						break;
					case 1:
						nWinTable=27;
						break;
					}
				}
				break;
			case 6:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=23;
						break;
					case 1:
						nWinTable=24;
						break;
					}
				}
				break;
			case 7:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=19;
						break;
					case 1:
						nWinTable=20;
						break;
					case 2:
						nWinTable=21;
						break;
					}
				}
				break;
			}
		}
		m_nTurnTableTarget[0] = nWinTable;
		//如果是鲨鱼，生产下一张图
		if ( CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_GOLD_SHARK || CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_SLIVER_SHARK )
		{
			m_bTurnTwoTime = TRUE;


			//生产第二张图
			nWinTable = CGameLogic::GetInstance()->RandomArea(TURAN_TABLE_MAX, 
				0, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3,
				0, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3,
				0, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_PANDA]/2, 0,
				m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_MONKEY]/2, 0, m_nGenerationChance[ANIMAL_RABBIT]/3,
				m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_RABBIT]/3, 0, m_nGenerationChance[ANIMAL_SWALLOW]/3,
				m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SWALLOW]/3, 0, m_nGenerationChance[ANIMAL_PIGEON]/2,
				m_nGenerationChance[ANIMAL_PIGEON]/2, 0, m_nGenerationChance[ANIMAL_PEACOCK]/2, m_nGenerationChance[ANIMAL_PEACOCK]/2);

			////nWinTable=3;
			//m_nTurnTableTarget[1] //控制当出现金鲨银鲨之后，第二次出现的动物
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			m_nTurnTableTarget[1] = nWinTable;
			int minArea1=0;
			int minArea2=0;
			int minArea3=0;
			int minArea4=0;
			LONGLONG AreaBet[8];
			AreaBet[0]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_LION]*m_nAnimalMultiple[ANIMAL_LION];
			AreaBet[1]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_PANDA]*m_nAnimalMultiple[ANIMAL_PANDA];
			AreaBet[2]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_MONKEY]*m_nAnimalMultiple[ANIMAL_MONKEY];
			AreaBet[3]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_RABBIT]*m_nAnimalMultiple[ANIMAL_RABBIT];
			AreaBet[4]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_EAGLE]*m_nAnimalMultiple[ANIMAL_EAGLE];
			AreaBet[5]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PEACOCK]*m_nAnimalMultiple[ANIMAL_PEACOCK];
			AreaBet[6]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PIGEON]*m_nAnimalMultiple[ANIMAL_PIGEON];
			AreaBet[7]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_SWALLOW]*m_nAnimalMultiple[ANIMAL_SWALLOW];
			LONGLONG maxAreaBet=0;
			int maxArea=0;
			for	 (int i=0;i<8;i++)
			{
				if (maxAreaBet<=AreaBet[i])
				{
					maxAreaBet=AreaBet[i];
					maxArea=i;
				}
			}
			if (maxAreaBet!=0)
			{
				LONGLONG AreaBet1=AreaBet[maxArea];
				LONGLONG AreaBet2=AreaBet[maxArea];
				LONGLONG AreaBet3=AreaBet[maxArea];
				LONGLONG AreaBet4=AreaBet[maxArea];
			for	 (int i=0;i<8;i++)
			{
				if (AreaBet1>=AreaBet[i])
				{
					AreaBet1=AreaBet[i];
					minArea1=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet2>=AreaBet[i]&&i!=minArea1)
				{
					AreaBet2=AreaBet[i];
					minArea2=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet3>=AreaBet[i]&&i!=minArea1&&i!=minArea2)
				{
					AreaBet3=AreaBet[i];
					minArea3=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet4>=AreaBet[i]&&i!=minArea1&&i!=minArea2&&i!=minArea3)
				{
					AreaBet4=AreaBet[i];
					minArea4=i;
				}
			}
			/*switch(rand()%4)
			{
			case 0:
				nWinTable=minArea1;
				break;
			case 1:
				nWinTable=minArea2;
				break;
			case 2:
				nWinTable=minArea3;
				break;
			case 3:
				nWinTable=minArea4;
				break;
			}*/
			nWinTable=minArea1;
			switch(nWinTable)
			{
			case 0:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=5;
						break;
					case 1:
						nWinTable=6;
						break;
					case 2:
						nWinTable=7;
						break;
					}
				}
				break;
			case 1:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=9;
						break;
					case 1:
						nWinTable=10;
						break;
					}
				}
				break;
			case 2:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=12;
						break;
					case 1:
						nWinTable=13;
						break;
					}
				}
				break;
			case 3:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=15;
						break;
					case 1:
						nWinTable=16;
						break;
					case 2:
						nWinTable=17;
						break;
					}
				}
				break;
			case 4:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=1;
						break;
					case 1:
						nWinTable=2;
						break;
					case 2:
						nWinTable=3;
						break;
					}
				}
				break;
			case 5:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=26;
						break;
					case 1:
						nWinTable=27;
						break;
					}
				}
				break;
			case 6:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=23;
						break;
					case 1:
						nWinTable=24;
						break;
					}
				}
				break;
			case 7:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=19;
						break;
					case 1:
						nWinTable=20;
						break;
					case 2:
						nWinTable=21;
						break;
					}
				}
				break;
			}
			m_nTurnTableTarget[1] = nWinTable;
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
	else
	{
		//生产第二张图
		nWinTable = CGameLogic::GetInstance()->RandomArea(TURAN_TABLE_MAX, 
			0, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3,
			0, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3,
			0, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_PANDA]/2, 0,
			m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_MONKEY]/2, 0, m_nGenerationChance[ANIMAL_RABBIT]/3,
			m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_RABBIT]/3, 0, m_nGenerationChance[ANIMAL_SWALLOW]/3,
			m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SWALLOW]/3, 0, m_nGenerationChance[ANIMAL_PIGEON]/2,
			m_nGenerationChance[ANIMAL_PIGEON]/2, 0, m_nGenerationChance[ANIMAL_PEACOCK]/2, m_nGenerationChance[ANIMAL_PEACOCK]/2);

		////nWinTable=3;
		//m_nTurnTableTarget[1] //控制当出现金鲨银鲨之后，第二次出现的动物
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		m_nTurnTableTarget[1] = nWinTable;
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

//游戏结束计算
int CTableFrameSink::GameOver()
{
	//定义变量
	int nPrizesMultiple = 0;
	bool bSuccess = false;
	//系统输赢
	LONGLONG lSystemScore = 0l;
	//该局控制的玩家输赢
	LONGLONG lPlayerScore = 0l;

	//LONGLONG lTempCalculateScore = 0l;
	//LONGLONG lMaxSystemScore = LLONG_MIN;
	//int nMaxTurnTableTarget[2] = {0};
	//int nWhileCount = 0;

	//计算分数
	//do 
	//{
		//输赢分数
		ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
		ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
		ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
		ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

		//读取控制
		LoadGameINI();

		if( m_pServerContro != NULL && m_pServerContro->NeedControl() )//在此处加入控制
		{
			if(m_pServerContro->ControlResult(&m_nTurnTableTarget[0]))
			{
				m_pServerContro->CompleteControl();
				int nWinTable=m_nTurnTableTarget[0];
				if ( CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_GOLD_SHARK || CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_SLIVER_SHARK )
					m_bTurnTwoTime = TRUE;
			}	//试探性判断
			CalculateScore( true, CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]), nPrizesMultiple );
			if ( m_bTurnTwoTime)
				CalculateScore( false, CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[1]), nPrizesMultiple );		
		}
		else
		{
			while (true)
			{
				//输赢分数
				ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
				ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
				ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
				ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));
				lSystemScore = 0l;
				lPlayerScore = 0l;

				//派发扑克
				CreateTableTable();

				int nWinTable=m_nTurnTableTarget[0];
				if ( CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_GOLD_SHARK || CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_SLIVER_SHARK )
					m_bTurnTwoTime = TRUE;
				lSystemScore += CalculateScore( true, CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]), nPrizesMultiple );
				if ( m_bTurnTwoTime)
					lSystemScore += CalculateScore( false, CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[1]), nPrizesMultiple );

				for (WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
				{
					//获取用户
					IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem==NULL) 
						continue;
					if(pIServerUserItem->GetGameID() == m_dwUserID)
					{
						LONGLONG lWinScore = 0;
						int nWinAnimal = CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]);
						if(nWinAnimal == ANIMAL_GOLD_SHARK)
							lWinScore = (m_lPlayBet[wChairID][nWinAnimal] * nPrizesMultiple);
						lPlayerScore += (m_lPlayWinScore[wChairID][0]/*+lWinScore*/);
						if ( m_bTurnTwoTime)
							lPlayerScore += m_lPlayWinScore[wChairID][1];
						break;
					}
				}

				//试探性判断
				if (WhetherControl(lSystemScore, lPlayerScore))
				{
					break;
				}
			}
		}

	//	//系统分值计算
	//	if(m_bControl==TRUE)
	//	{
	//		break;
	//	}
	//	else if ((lSystemScore + m_lStorageStart) < 0l)
	//	{
	//		bSuccess =  false;

	//		nWhileCount++;
	//		if( lSystemScore > lMaxSystemScore )
	//		{
	//			lMaxSystemScore = lSystemScore;
	//			CopyMemory(nMaxTurnTableTarget, m_nTurnTableTarget, sizeof(nMaxTurnTableTarget));
	//		}

	//		if( nWhileCount >= 100000 && lMaxSystemScore != LLONG_MIN )
	//		{
	//			//强制结束
	//			m_lStorageStart += lMaxSystemScore;
	//			CopyMemory(m_nTurnTableTarget, nMaxTurnTableTarget, sizeof(m_nTurnTableTarget));
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		m_lStorageStart += lSystemScore;
	//		
	//		//有人并且有人下注才衰减库存
	//		for( int i = 0; i < GAME_PLAYER; ++i )
	//		{
	//			if( m_pITableFrame->GetTableUserItem(i) != NULL && lSystemScore != 0 )
	//			{
	//				m_lStorageStart = m_lStorageStart - (m_lStorageStart * m_lStorageDeduct / 1000);
	//				break;
	//			}
	//		}

	//		bSuccess = true;
	//	}

	//} while (!bSuccess);

	//添加记录
	m_ArrayTurnTableRecord.Add( m_nTurnTableTarget[0] );
	if( m_ArrayTurnTableRecord.GetCount() > RECORD_COUNT_MAX )
	{
		m_ArrayTurnTableRecord.RemoveAt(0);
	}

	return nPrizesMultiple;
}

//计算得分
LONGLONG CTableFrameSink::CalculateScore( bool bFirst, int nWinAnimal, int& nPrizesMultiple )
{
	//系统输赢
	LONGLONG lSystemScore = 0l;
	
	//彩金
	if ( bFirst && nWinAnimal == ANIMAL_GOLD_SHARK )
	{
		nPrizesMultiple = (rand()%6) + 6;
	}

	//计算积分
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		//是否机器人
		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		for ( int nAnimalIndex = 0; nAnimalIndex < ANIMAL_MAX; ++nAnimalIndex )
		{
			if ( nWinAnimal == nAnimalIndex || 
				(nAnimalIndex == ANIMAL_BIRD && CGameLogic::GetInstance()->AnimalType(nWinAnimal) == ANIMAL_TYPE_BIRD ) ||
				(nAnimalIndex == ANIMAL_BEAST && CGameLogic::GetInstance()->AnimalType(nWinAnimal) == ANIMAL_TYPE_BEAST ) )
			{
				//玩家赢分
				LONGLONG lWinScore = m_lPlayBet[wChairID][nAnimalIndex] * m_nAnimalMultiple[nAnimalIndex];
				m_lPlayWinChip[wChairID] += lWinScore;
				m_lPlayWinAnimal[wChairID][nAnimalIndex] += lWinScore;

				if ( bFirst )
					m_lPlayWinScore[wChairID][0] += lWinScore;
				else
					m_lPlayWinScore[wChairID][1] += lWinScore;

				//系统输分
				if ( !bIsAndroidUser )
					lSystemScore -= (lWinScore - m_lPlayBet[wChairID][nAnimalIndex]);
			}
			else if( bFirst )
			{
				//玩家输分
				LONGLONG lLoseScore = m_lPlayBet[wChairID][nAnimalIndex];

				//系统赢分
				if ( !bIsAndroidUser )
					lSystemScore += lLoseScore;
			}
		}

		//计算彩金
		if ( nPrizesMultiple != 0 && bFirst )
		{
			//玩家赢分
			LONGLONG lWinScore = (m_lPlayBet[wChairID][nWinAnimal] * nPrizesMultiple);
			m_lPlayWinChip[wChairID] += lWinScore;
			m_lPlayWinAnimal[wChairID][nWinAnimal] += lWinScore;
			m_lPlayWinPrizes[wChairID] += lWinScore;
			m_lPlayWinScore[wChairID][0] += lWinScore;

			//系统输分
			if ( !bIsAndroidUser )
				lSystemScore -= lWinScore;
		}

	}

	return lSystemScore;
}

//结束写分
void CTableFrameSink::GameOverWriteScore()
{
	//写入积分
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if(pIServerUserItem == NULL) continue;

		//扣除筹码
		m_lPlayScore[wChairID] += m_lPlayWinScore[wChairID][0]+m_lPlayWinScore[wChairID][1];

		if ( m_lPlayScore[wChairID] < 0 )
		{
			ASSERT(FALSE);
			m_lPlayScore[wChairID] = 0;
		}

		UpdataScore(wChairID, pIServerUserItem);
	}

	//清空下注
	ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
	ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
	ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
	ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

	//下注数
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory(m_HPlayBet,sizeof(m_HPlayBet));
	ZeroMemory(m_FPlayBet,sizeof(m_FPlayBet));
	ZeroMemory(m_APlayBet,sizeof(m_APlayBet));
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
}

//读取配置文件
BYTE CTableFrameSink::GetProbility()
{
	return ::GetPrivateProfileInt(m_pGameServiceOption->szServerName, _T("Problility"), 30, m_szConfigFileName);
}

void CTableFrameSink::LoadGameINI()
{
	//是否作弊换牌
	m_bIsCheat = GetPrivateProfileInt(m_szGameRoomName,TEXT("Ischeat"),0,m_szConfigFileName);
	//机器人坐庄，机器人输的最大分数有概率换牌
	m_lRobotloseMax = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotLoseMax"),-2000000,m_szConfigFileName);
	//机器人坐庄，机器人输的最大分数百分百换牌
	m_lRobotLoseMaxChange = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotLoseMaxChange"),-5000000,m_szConfigFileName);
	//机器人坐庄，机器人换牌的百分比
	m_btRobotLoseChangePercent = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotLoseChangePercent"),50,m_szConfigFileName);

	//有概率换牌的起始输分分数
	m_lRealBankerRobotLoseMax = GetPrivateProfileInt(m_szGameRoomName,TEXT("RealBankerRobotLoseMax"),10000000,m_szConfigFileName);
	//百分百换牌，机器人输的起始分数
	m_lRealBankerRobotLoseMaxChange = GetPrivateProfileInt(m_szGameRoomName,TEXT("RealBankerRobotLoseMaxChange"),30000000,m_szConfigFileName);
	//换牌百分比
	m_btRealBankerRobotLoseChangePercent = GetPrivateProfileInt(m_szGameRoomName,TEXT("RealBankerRobotLoseChangePercent"),30,m_szConfigFileName);

	//赢的金额数，机器人有概率放水
	m_lRobotProbabilityTanking = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotProbabilityTanking"),80000000,m_szConfigFileName);
	//赢的金额数，机器人百分百放水
	m_lRobotDefineTanking = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotDefineTanking"),100000000,m_szConfigFileName);
	//机器人放水的百分比
	m_btRobotTankingPercent = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotTankingPercent"),20,m_szConfigFileName);

	//通常情况下，换牌百分比
	m_btUsuallyChangePercent = GetPrivateProfileInt(m_szGameRoomName,TEXT("UsuallyChangePercent"),5,m_szConfigFileName);
	//扣除机器人赢的分数的百分比
	m_btRobotWinScorePercent = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotWinScorePercent"),5,m_szConfigFileName);
	//机器人累计的输赢分数
	m_lRobotResultScore = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotResultScore"),100,m_szConfigFileName);
	//扣除的机器人赢的分数总和
	if(m_lRobotWinChiefScore == 0)
		m_lRobotWinChiefScore = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotWinChiefScore"),0,m_szConfigFileName);

	//是否控制玩家
	m_IsCheatUser = GetPrivateProfileInt(TEXT("1"),TEXT("IsCheatUser"),0,m_szConfigFilePath);
	//控制输赢的玩家ID (0表示不控制)
	m_dwUserID = GetPrivateProfileInt(TEXT("1"),TEXT("UserID"),0,m_szConfigFilePath);
	//控制玩家输赢的总局数（每控制一局减一）
	m_dwUserLoseWinPercent = GetPrivateProfileInt(TEXT("1"),TEXT("UserLoseWinPercent"),0,m_szConfigFilePath);
	//控制玩家是赢还是输（1表示赢 0表示输）
	m_bLoseWin = GetPrivateProfileInt(TEXT("1"),TEXT("LoseWin"),0,m_szConfigFilePath);
	//控制玩家赢的金额最大上限
	m_lWinMaxGold = GetPrivateProfileInt(TEXT("1"),TEXT("WinMaxGold"),0,m_szConfigFilePath);
	//控制玩家输的金额的最大下限
	m_lLoseMaxGold = GetPrivateProfileInt(TEXT("1"),TEXT("LoseMaxGold"),0,m_szConfigFilePath);
	//控制玩家的输赢情况
	m_lAllUserGold = GetPrivateProfileInt(TEXT("1"),TEXT("UserGold"),0,m_szConfigFilePath);
}

void CTableFrameSink::WriteGameINI()
{
	//写入配置
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szFileName[MAX_PATH];

	if (GetCurrentDirectory(sizeof(szPath), szPath))
	{
		TCHAR szRobotResultScore[MAX_PATH]=TEXT("");
		//玩家控制
		if(m_IsCheatUser && m_lUserScore!=0)
		{
			_sntprintf(szRobotResultScore,sizeof(szRobotResultScore),TEXT("%d"),m_lAllUserGold);
			WritePrivateProfileString(TEXT("1"),TEXT("UserGold"),szRobotResultScore,m_szConfigFilePath);
		}
		if(((m_bLoseWin && m_lAllUserGold>m_lWinMaxGold) || (!m_bLoseWin && m_lAllUserGold<m_lLoseMaxGold)) && m_IsCheatUser)
			WritePrivateProfileString(TEXT("1"),TEXT("IsCheatUser"),TEXT("0"),m_szConfigFilePath);

		//机器人控制
		if(m_lRobotAchievement != 0)
		{
			m_lRobotResultScore+=m_lRobotAchievement;
			if(m_lRobotResultScore>=m_lRobotDefineTanking)
			{
				long lMoney = m_lRobotResultScore*( m_btRobotWinScorePercent*0.01);
				m_lRobotWinChiefScore += lMoney;
				m_lRobotResultScore -= lMoney;
				//扣除的机器人赢的分数总和
				_sntprintf(szRobotResultScore,sizeof(szRobotResultScore),TEXT("%d"),m_lRobotWinChiefScore);
				WritePrivateProfileString(m_szGameRoomName,TEXT("RobotWinChiefScore"),szRobotResultScore,m_szConfigFileName);
			}
			//机器人累计的输赢分数
			_sntprintf(szRobotResultScore,sizeof(szRobotResultScore),TEXT("%d"),m_lRobotResultScore);
			WritePrivateProfileString(m_szGameRoomName,TEXT("RobotResultScore"),szRobotResultScore,m_szConfigFileName);
		}

		static bool bControl = true;
		TCHAR szBunko[MAX_PATH]=TEXT("");
		CString str;
		CTime tm;
		tm = CTime::GetCurrentTime();
		if(tm.GetHour() == 11)
		{
			bControl = true;
		}
		if(tm.GetHour() == 14)
		{
			if(bControl)
			{
				bControl = false;
				_sntprintf(szFileName,sizeof(szFileName),TEXT("%s\\GameLog\\AnimalBattle.log"),szPath);
				str = tm.Format("%Y-%m-%d %X");
				str+= "\t胜负";
				_sntprintf(szBunko,sizeof(szBunko),TEXT("%d"),m_lRobotResultScore+m_lRobotWinChiefScore);
				WritePrivateProfileString(m_szGameRoomName,str,szBunko,szFileName);
				//bControl = false;
				m_lRobotWinChiefScore = 0;
				m_lRobotResultScore = 0;
				//扣除的机器人赢的分数总和
				_sntprintf(szRobotResultScore,sizeof(szRobotResultScore),TEXT("%d"),m_lRobotWinChiefScore);
				WritePrivateProfileString(m_szGameRoomName,TEXT("RobotWinChiefScore"),szRobotResultScore,m_szConfigFileName);
				//机器人累计的输赢分数
				_sntprintf(szRobotResultScore,sizeof(szRobotResultScore),TEXT("%d"),m_lRobotResultScore);
				WritePrivateProfileString(m_szGameRoomName,TEXT("RobotResultScore"),szRobotResultScore,m_szConfigFileName);
			}
		}
	}
}

//读取配置
void CTableFrameSink::ReadConfigInformation()
{
	//变量定义
	TCHAR OutBuf[255] = {};

	//单位积分
	/*m_lCellScore = GetPrivateProfileInt(m_szGameRoomName, TEXT("CellScore"), 1, m_szConfigFileName);
	if ( m_lCellScore < 1 )
		m_lCellScore = 1;*/

	//区域限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szGameRoomName, TEXT("AreaLimitScore"), _T("0"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lAreaLimitScore);

	//玩家区域限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szGameRoomName, TEXT("PlayLimitScore"), _T("0"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lPlayLimitScore);

	//库存
	//ZeroMemory(OutBuf, sizeof(OutBuf));
	//GetPrivateProfileString(m_szGameRoomName, TEXT("StorageStart"), _T("0"), OutBuf, 255, m_szConfigFileName);
	//_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lStorageStart);

	//衰减值
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szGameRoomName, TEXT("StorageDeduct"), _T("1"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lStorageDeduct);

	//下注机器人数目
	m_nMaxChipRobot = GetPrivateProfileInt(m_szGameRoomName, TEXT("RobotBetCount"), 10, m_szConfigFileName);
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 10;

	//最多个数
	m_nRobotListMaxCount = GetPrivateProfileInt(m_szGameRoomName, _T("RobotListMaxCount"), 5, m_szConfigFileName);	

	//机器人下注限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szGameRoomName, TEXT("RobotAreaLimit"), TEXT("50000000"), OutBuf, 255*sizeof(TCHAR), m_szConfigFileName);
	_snwscanf(OutBuf, wcslen(OutBuf), TEXT("%I64d"), &m_lRobotAreaLimit);

	//区域概率
	m_nGenerationChance[0] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat1"), 8, m_szConfigFileName);
	m_nGenerationChance[1] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat2"), 8, m_szConfigFileName);
	m_nGenerationChance[2] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat3"), 12, m_szConfigFileName);
	m_nGenerationChance[3] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat4"), 6, m_szConfigFileName);
	m_nGenerationChance[4] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat5"), 12, m_szConfigFileName);
	m_nGenerationChance[5] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat6"), 6, m_szConfigFileName);
	m_nGenerationChance[6] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat7"), 6, m_szConfigFileName);
	m_nGenerationChance[7] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat8"), 6, m_szConfigFileName);
	m_nGenerationChance[8] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat9"), 1, m_szConfigFileName);
	m_nGenerationChance[9] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat10"), 1, m_szConfigFileName);

	//时间控制
	m_cbFreeTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("FreeTime"), TIME_FREE, m_szConfigFileName);

	m_cbBetTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("BetTime"), TIME_BET, m_szConfigFileName);

	m_cbEndTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("EndTime"), TIME_END, m_szConfigFileName);

	//切换数量
	m_lAddScore[0]=GetPrivateProfileInt(m_szGameRoomName, TEXT("AddScore1"), 1000, m_szConfigFileName);
	m_lAddScore[1]=GetPrivateProfileInt(m_szGameRoomName, TEXT("AddScore2"), 10000, m_szConfigFileName);
	m_lAddScore[2]=GetPrivateProfileInt(m_szGameRoomName, TEXT("AddScore3"), 100000, m_szConfigFileName);
	m_lAddScore[3]=GetPrivateProfileInt(m_szGameRoomName, TEXT("AddScore4"), 500000, m_szConfigFileName);

	if (m_cbFreeTime <= 0 || m_cbFreeTime > 99)			
		m_cbFreeTime = TIME_FREE;
	if (m_cbBetTime <= 0 || m_cbBetTime > 99)		
		m_cbBetTime = TIME_BET;
	if (m_cbEndTime <= 0 || m_cbEndTime > 99)			
		m_cbEndTime = TIME_END;


	if( m_lStorageDeduct < 1 )
	{
		m_lStorageDeduct = 1;
	}	
}


//试探性判断
bool CTableFrameSink::WhetherControl(LONGLONG lSystemScore, LONGLONG lUserScore)
{
	//控制
	if(m_IsCheatUser && lUserScore!=0)
	{
		if(m_bLoseWin)
		{
			if((rand()%100<m_dwUserLoseWinPercent && lUserScore<0) /*|| ((m_lUserScore+m_lAllUserGold) > (m_lWinMaxGold+m_lWinMaxGold/10))*/)
				return false;
		}
		else
		{
			if((rand()%100<m_dwUserLoseWinPercent && lUserScore>0) /*|| ((m_lUserScore+m_lAllUserGold) < (m_lLoseMaxGold+m_lLoseMaxGold/10))*/)
				return false;
		}
	}
	else
	{
		if(m_bIsCheat)
		{	
			//是否瞒足换牌条件(放水)
			if((m_lRobotResultScore>=m_lRobotDefineTanking) || (m_lRobotResultScore>=m_lRobotProbabilityTanking && rand()%100 < m_btRobotTankingPercent))
			{
				if(lSystemScore>0)
				{
					printf("\n >>>>>>>>>>>机器人进入放水状态！");
					return false;
				}
			}
			//是否瞒足换牌条件
			else if((m_lRobotResultScore<=m_lRobotLoseMaxChange) || (m_lRobotResultScore<=m_lRobotloseMax && rand()%100 < m_btRobotLoseChangePercent) ||
				(rand()%100 < m_btUsuallyChangePercent) || ((lSystemScore*-1)>=m_lRealBankerRobotLoseMaxChange) || 
				((lSystemScore*-1)>=m_lRealBankerRobotLoseMax && rand()%100 < m_btRealBankerRobotLoseChangePercent))
			{
				if(lSystemScore < 0)
				{
					printf("\n >>>>>>>>>>>机器人进入换牌状态！");
					return false;
				}
			}
		}
	}
	m_lUserScore = lUserScore;
	m_lAllUserGold += lUserScore;
	m_lRobotAchievement = lSystemScore;

	return true;
}
void CTableFrameSink::SendSwitchNum(IServerUserItem * pIServerUserItem)
{
	//定义消息
	CMD_S_SwitchNum SwitchNum;
	SwitchNum.lAddScore1 = m_lAddScore[0];
	SwitchNum.lAddScore2 = m_lAddScore[1];
	SwitchNum.lAddScore3 = m_lAddScore[2];
	SwitchNum.lAddScore4 = m_lAddScore[3];
	
	m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_SWITCT_NUM, &SwitchNum, sizeof(SwitchNum));
	m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(),SUB_S_SWITCT_NUM, &SwitchNum, sizeof(SwitchNum));
	
	return;
}
//////////////////////////////////////////////////////////////////////////////////
