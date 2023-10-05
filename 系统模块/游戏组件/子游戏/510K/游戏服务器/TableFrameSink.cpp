#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////
#define IDI_CHECK_TABLE				1					//检查桌子
#define IDI_USER_ACTION			2					//玩家操作
//构造函数
CTableFrameSink::CTableFrameSink()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameCustomRule=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));


	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//扑克信息
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	ZeroMemory(m_bAddTimes,sizeof(m_bAddTimes));
	ZeroMemory(m_bWinFlag,sizeof(m_bWinFlag));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	m_b2BiggestMode=false;
	m_bCurrenUserDone=false;
	m_cbFriendFlag=0;
	m_bShuangKou=false;
	m_bGameFinish=false;
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{

}

//接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//扑克信息
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
	m_bGameFinish=false;
	return;
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

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//游戏配置
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//自定规则
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();
	m_b2BiggestMode = m_pGameCustomRule->cb2BiggestMode;
	m_GameLogic.Set2Biggest(m_b2BiggestMode);

	return true;
}

//消费能力
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	return 0L;
}

//最少积分
SCORE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//金币游戏
	if (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{		
		return 48 * m_pITableFrame->GetCellScore();
	}
	return 0L;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wXuanZhanUser=INVALID_CHAIR;
	m_wAskUser=INVALID_CHAIR;
	m_cbFinishUserCount=0;
	m_cbBaseTimes=1;
	m_cbCurTurnScore=0;
	m_bCurrenUserDone=false;
	m_cbFriendFlag=0;
	ZeroMemory(m_wFinishUser,sizeof(m_wFinishUser));
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_bAddTimes,sizeof(m_bAddTimes));
	ZeroMemory(m_cbAskStatus,sizeof(m_cbAskStatus));
	ZeroMemory(m_bAddTimes,sizeof(m_bAddTimes));
	ZeroMemory(m_cbGameScore,sizeof(m_cbGameScore));
	ZeroMemory(m_bWinFlag,sizeof(m_bWinFlag));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	memset(m_wFriend,INVALID_CHAIR,sizeof(m_wFriend));
	m_bShuangKou=false;
	//设置状态
	m_pITableFrame->SetGameStatus(GS_XUAN_ZHAN);

	//混乱扑克
	BYTE cbRandCard[FULL_COUNT];
	m_GameLogic.RandCardList(cbRandCard,CountArray(cbRandCard));
	
	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	CString strSaveFileName;
	strSaveFileName.Format(TEXT("%s\\configcard\\510K.card"), szPath);

	CFile file;
	if (file.Open(strSaveFileName, CFile::modeRead))
	{
		file.Read(cbRandCard,sizeof(cbRandCard));		
		file.Close();
	}		

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHandCardCount[i]=NORMAL_CARD_COUNT;
		CopyMemory(&m_cbHandCardData[i],&cbRandCard[i*NORMAL_CARD_COUNT],sizeof(m_cbHandCardData[i]));

		if(m_wBankerUser == INVALID_CHAIR)
		{
			BYTE card = (m_b2BiggestMode==true?0x23:0x22);
			for(int j=0;j<NORMAL_CARD_COUNT;j++)
			{
				if(m_cbHandCardData[i][j] == card)//找出庄家玩家
				{
					m_wBankerUser = i;
					break;
				}
			}
		}

	}

	//设置用户
	m_wCurrentUser=(m_wBankerUser+1)%GAME_PLAYER;

	//构造变量
	CMD_S_GameStart GameStart;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.wBanker=m_wBankerUser;
	GameStart.b2Biggest = m_b2BiggestMode;
	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		CopyMemory(GameStart.cbCardData,m_cbHandCardData[i],sizeof(GameStart.cbCardData));

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	//排列扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i],ST_ORDER);
	}

	//判断金币
	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		m_pITableFrame->SetGameTimer(IDI_CHECK_TABLE,30000,-1,NULL);
		m_cbWaitTime=0;
	}
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		m_pITableFrame->KillGameTimer(IDI_CHECK_TABLE);

	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			m_pITableFrame->KillGameTimer(IDI_USER_ACTION);
			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude, sizeof(GameConclude));
			GameConclude.bShuangKou=m_bShuangKou;
			CopyMemory(GameConclude.cbScore,m_cbGameScore,sizeof(GameConclude.cbScore));

			//设置变量
			GameConclude.cbBaseTimes = m_cbBaseTimes;
			GameConclude.lCellScore = m_pITableFrame->GetCellScore();

			//用户扑克
			BYTE cbCardIndex = 0;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//拷贝扑克
				GameConclude.cbCardCount[i] = m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex], m_cbHandCardData[i], m_cbHandCardCount[i]*sizeof(BYTE));

				GameConclude.cbUserTimes[i] = m_cbBaseTimes*(m_bAddTimes[i]?2:1);
				//设置索引
				cbCardIndex += m_cbHandCardCount[i];
			}

			BYTE cbWinUserCount=0;
			BYTE cbLoseUserCount=0;
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bWinFlag[i])
					cbWinUserCount++;
				else
					cbLoseUserCount++;
			}
			//积分变量
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

			//统计积分

			SCORE lCellScore = m_pITableFrame->GetCellScore();

			SCORE lUserScore[GAME_PLAYER]={0};
			for(WORD i=0;i<GAME_PLAYER;i++)
				lUserScore[i] = m_pITableFrame->GetTableUserItem(i)->GetUserScore();

			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_bWinFlag[i] == TRUE)
				{

					for(WORD j=0;j<GAME_PLAYER;j++)
					{
						SCORE LoseScore = lCellScore*m_cbBaseTimes;
						if(m_bAddTimes[i])
							LoseScore *= 2;
						
						if(m_bShuangKou)
							LoseScore *= 2;

						if(m_bWinFlag[j] == FALSE)
						{
							if(m_bAddTimes[j])
								LoseScore *= 2;

							if(LoseScore > lUserScore[j])
							{
								LoseScore = lUserScore[j];
							}

							lUserScore[j] -= LoseScore;
							ScoreInfoArray[j].lScore -= LoseScore;
							ScoreInfoArray[j].cbType = SCORE_TYPE_LOSE;

							ScoreInfoArray[i].lScore += LoseScore;
						}
					}
					
					ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
					ScoreInfoArray[i].lRevenue = m_pITableFrame->CalculateRevenue(i,ScoreInfoArray[i].lScore);
					if(ScoreInfoArray[i].lRevenue > 0L)
						ScoreInfoArray[i].lScore -= ScoreInfoArray[i].lRevenue;
				}

			}

			for(int i=0;i<GAME_PLAYER;i++)
			{
				GameConclude.lGameScore[i] = ScoreInfoArray[i].lScore;
				m_HistoryScore.OnEventUserScore(i,GameConclude.lGameScore[i]);
			}
			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_GAME_FREE);

			return true;
		}
	case GER_USER_LEAVE:	//用户强退
		{
			if((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0)//房卡模式不处理
				return true;

			if(m_pITableFrame->GetGameStatus() == GS_PLAY)//普通出牌阶段托管
				return OnUserTrustee(wChairID,true);

			return OnUserFlee(wChairID);//叫牌阶段逃跑
		}
	case GER_NETWORK_ERROR:	//网络中断
		{
			if((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0)//房卡模式不处理
				return true;

			if(m_pITableFrame->GetGameStatus() == GS_PLAY)//普通出牌阶段托管
				return OnUserTrustee(wChairID,true);

			return OnUserFlee(wChairID);//叫牌阶段逃跑
		}
	case GER_DISMISS:		//游戏解散
		{
			m_pITableFrame->KillGameTimer(IDI_USER_ACTION);
			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude, sizeof(GameConclude));

			//用户扑克
			BYTE cbCardIndex=0;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//拷贝扑克
				GameConclude.cbCardCount[i] = m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex], m_cbHandCardData[i], m_cbHandCardCount[i]*sizeof(BYTE));

				//设置索引
				cbCardIndex += m_cbHandCardCount[i];
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_GAME_FREE);

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_GAME_FREE:	//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore = m_pITableFrame->GetCellScore();
			StatusFree.b2Biggest = m_b2BiggestMode;
			//自定规则
			StatusFree.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusFree.cbTimeStart = m_pGameCustomRule->cbTimeStartGame;
			StatusFree.cbTimeXuanZhan=m_pGameCustomRule->cbTimeXuanZhan;
			StatusFree.cbTimeFindFriend=m_pGameCustomRule->cbTimeFindFriend;
			StatusFree.cbTimeAskFriend=m_pGameCustomRule->cbTimeAskFriend;
			StatusFree.cbTimeAddTimes=m_pGameCustomRule->cbTimeAddTimes;
			//历史积分
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//变量定义
				tagHistoryScore* pHistoryScore = m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusFree.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
		}
	case GS_XUAN_ZHAN:
	case GS_FIND_FRIEND:
	case GS_ASK_FRIEND:
	case GS_ADD_TIMES:
	case GS_PLAY:	//游戏状态
		{
			OnUserTrustee(wChairID,false);//取消托管
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));

			//单元积分
			StatusPlay.lCellScore = m_pITableFrame->GetCellScore();
			StatusPlay.b2Biggest = m_b2BiggestMode;
			StatusPlay.cbBaseTimes=m_cbBaseTimes;
			//自定规则
			StatusPlay.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusPlay.cbTimeStart = m_pGameCustomRule->cbTimeStartGame;
			StatusPlay.cbTimeXuanZhan=m_pGameCustomRule->cbTimeXuanZhan;
			StatusPlay.cbTimeFindFriend=m_pGameCustomRule->cbTimeFindFriend;
			StatusPlay.cbTimeAskFriend=m_pGameCustomRule->cbTimeAskFriend;
			StatusPlay.cbTimeAddTimes=m_pGameCustomRule->cbTimeAddTimes;

			//游戏变量
			StatusPlay.wBankerUser = m_wBankerUser;
			StatusPlay.cbSelectedCardData = m_cbSelectedCardData;
			StatusPlay.wAskUser = m_wAskUser;
			StatusPlay.wCurrentUser = m_wCurrentUser;
			StatusPlay.cbGameStatus = m_pITableFrame->GetGameStatus();
			StatusPlay.cbFriendFlag=m_cbFriendFlag;
			StatusPlay.wXuanZhanUser=m_wXuanZhanUser;
			CopyMemory(StatusPlay.bAddTimes,m_bAddTimes,sizeof(StatusPlay.bAddTimes));
			CopyMemory(StatusPlay.wFriend,m_wFriend,sizeof(StatusPlay.wFriend));

			if(StatusPlay.cbGameStatus == GS_ASK_FRIEND)
			{
				StatusPlay.bEnabledAskFriend=true;
				if(m_wXuanZhanUser ==m_wBankerUser && m_wCurrentUser == m_wBankerUser)
					StatusPlay.bEnabledAskFriend=false;
			}
			else if(StatusPlay.cbGameStatus == GS_ADD_TIMES)
			{
				StatusPlay.bEnabledAddTimes=(m_cbAskStatus[wChairID] != 0xFF);
				
				if(m_cbFriendFlag == 6 && (wChairID == m_wAskUser || wChairID == m_wFriend[m_wAskUser]) )
				{
					bool bFlag=false;
					for(int i=0;i<GAME_PLAYER;i++)
					{
						if(i != m_wAskUser && i != m_wFriend[m_wAskUser] && m_cbAskStatus[i] == 0xFF && m_bAddTimes[i])
							bFlag=true;
					}

					if(!bFlag)
						StatusPlay.bEnabledAddTimes=false;
				}
				else if(m_cbFriendFlag == 1 && m_wXuanZhanUser != INVALID_CHAIR)
				{
					bool bFlag=false;
					for(int i=0;i<GAME_PLAYER;i++)
					{
						if(i != m_wXuanZhanUser && m_cbAskStatus[i] == 0xFF && m_bAddTimes[i])
							bFlag=true;
					}
					if(!bFlag)
						StatusPlay.bEnabledAddTimes=false;
				}
			}
			else if(StatusPlay.cbGameStatus == GS_PLAY)
			{
				//出牌信息
				StatusPlay.wTurnWiner = m_wTurnWiner;
				StatusPlay.cbTurnCardCount = m_cbTurnCardCount;
				StatusPlay.cbTunrScore = m_cbCurTurnScore;
				
				CopyMemory(StatusPlay.cbGameScore,m_cbGameScore,sizeof(StatusPlay.cbGameScore));
				CopyMemory(StatusPlay.cbTurnCardData, m_cbTurnCardData, m_cbTurnCardCount*sizeof(BYTE));
				CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(StatusPlay.bTrustee));
			}
			
			//扑克信息
			CopyMemory(StatusPlay.cbHandCardCount, m_cbHandCardCount, sizeof(m_cbHandCardCount));
			CopyMemory(StatusPlay.cbHandCardData, m_cbHandCardData[wChairID], sizeof(BYTE)*m_cbHandCardCount[wChairID]);

			//历史积分
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//变量定义
				tagHistoryScore* pHistoryScore = m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusPlay.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//时间事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		if(wTimerID==IDI_CHECK_TABLE)
		{
			m_cbWaitTime++;
			if(m_cbWaitTime>=3)
			{
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS);
			}
			else if(m_cbWaitTime>=2)
			{
				for(int i=0;i<GAME_PLAYER;i++)
				{
					IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(i);
					if(pServerUserItem!=NULL && pServerUserItem->GetUserStatus()==US_OFFLINE)return true;				
				}
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS);
			}
			return true;
		}
	}
	else if(wTimerID == IDI_USER_ACTION)
	{
		m_pITableFrame->KillGameTimer(IDI_USER_ACTION);
		AutomatismOutCard();
		return true;
	}
	return false;
}

//数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return false;
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	m_cbWaitTime=0;
	switch (wSubCmdID)
	{
	case SUB_C_XUAN_ZHAN:	//用户宣战
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_XuanZhan));
			if (wDataSize!=sizeof(CMD_C_XuanZhan)) return false;

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_XUAN_ZHAN);
			if (m_pITableFrame->GetGameStatus()!=GS_XUAN_ZHAN) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//变量定义
			CMD_C_XuanZhan * pXuanZhan=(CMD_C_XuanZhan *)pData;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserXuanZhan(wChairID,pXuanZhan->bXuanZhan);
		}
	case SUB_C_FIND_FRIEND:	//用户找同盟
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_FindFriend));
			if (wDataSize!=sizeof(CMD_C_FindFriend)) return false;

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_FIND_FRIEND);
			if (m_pITableFrame->GetGameStatus()!=GS_FIND_FRIEND) return true;
			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;
			//变量定义
			CMD_C_FindFriend * pFindFriend=(CMD_C_FindFriend *)pData;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserFindFriend(wChairID,pFindFriend->cbCardData);
		}
	case SUB_C_ASK_FRIEND:	//用户问同盟
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AskFriend));
			if (wDataSize!=sizeof(CMD_C_AskFriend)) return false;

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_ASK_FRIEND);
			if (m_pITableFrame->GetGameStatus()!=GS_ASK_FRIEND) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//变量定义
			CMD_C_AskFriend * pAsk=(CMD_C_AskFriend *)pData;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserAskFriend(wChairID,pAsk->bAsk);
		}
	case SUB_C_ADD_TIMES:	//用户加倍
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddTimes));
			if (wDataSize!=sizeof(CMD_C_AddTimes)) return false;

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_ADD_TIMES);
			if (m_pITableFrame->GetGameStatus()!=GS_ADD_TIMES) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//变量定义
			CMD_C_AddTimes * pAddTimes=(CMD_C_AddTimes *)pData;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserAddTimes(wChairID,pAddTimes->bAddTimes);
		}
	case SUB_C_OUT_CARD:	//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//效验数据
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE))));
			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE)))) return false;

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GS_PLAY) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserOutCard(wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
	case SUB_C_PASS_CARD:	//用户放弃
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GS_PLAY) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			return OnUserPassCard(pIServerUserItem->GetChairID());
		}
	case SUB_C_TRUSTEE://托管
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_Trustee));
			if (wDataSize!=sizeof(CMD_C_Trustee)) return false;

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GS_PLAY) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			CMD_C_Trustee *pack = (CMD_C_Trustee*)pData;
			return OnUserTrustee(pIServerUserItem->GetChairID(),pack->bTrustee);
		}
	}

	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//获取规则设置
	if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)
	{
		BYTE* pSetInfo = m_pITableFrame->GetGameRule();
		if(pSetInfo[0] == 1)
		{
			m_b2BiggestMode = (pSetInfo[3]==1);
			m_GameLogic.Set2Biggest(m_b2BiggestMode);
		}
	}



	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserEnter(wChairID);
	}

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserLeave(wChairID);
	}

	return true;
}

bool CTableFrameSink::OnUserXuanZhan(WORD wChairID, bool bXuanZhan)
{
	ASSERT(m_wXuanZhanUser==INVALID_CHAIR);
	if(m_wXuanZhanUser != INVALID_CHAIR) return false;

	CMD_S_XuanZhan pack;
	ZeroMemory(&pack,sizeof(pack));
	pack.wXuanZhanUser=wChairID;
	pack.bXuanZhan = bXuanZhan;
	
	if(bXuanZhan)
	{
		m_cbFriendFlag=1;
		m_cbBaseTimes *=4;
		m_wXuanZhanUser = wChairID;
		pack.wCurrentUser = m_wBankerUser;
		m_wAskUser = m_wXuanZhanUser;
		m_pITableFrame->SetGameStatus(GS_ADD_TIMES);
	}
	else
	{
		WORD wUser = (wChairID+1)%GAME_PLAYER;
		if(wUser == m_wBankerUser)//三家都没有宣战
		{
			m_pITableFrame->SetGameStatus(GS_FIND_FRIEND);
		}

		m_wCurrentUser=wUser;
		pack.wCurrentUser = wUser;
	}
	pack.cbFriendFlag=m_cbFriendFlag;
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_XUAN_ZHAN,&pack,sizeof(pack));
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_XUAN_ZHAN,&pack,sizeof(pack));
	
	return true;
}

bool CTableFrameSink::OnUserFindFriend(WORD wChairID, BYTE cbCardData)
{
	m_cbSelectedCardData = cbCardData;

	CMD_S_FindFriend pack;
	ZeroMemory(&pack,sizeof(pack));
	pack.wFindUser = m_wBankerUser;
	pack.wCurrentUser = m_wBankerUser;
	pack.cbSelectedCardData = cbCardData;
	WORD wFriend=INVALID_CHAIR;
	for(int i=0;i<GAME_PLAYER;i++)
	{
		for(int j=0;j<NORMAL_CARD_COUNT;j++)
		{
			if(m_cbHandCardData[i][j] == cbCardData)
			{
				wFriend = i;
				break;
			}
		}
	}
	ASSERT(wFriend != INVALID_CHAIR);
	pack.wSelectedUser=wFriend;
	if(wFriend == m_wBankerUser)//选择了自己
	{
		m_cbFriendFlag=2;
		m_wXuanZhanUser = m_wBankerUser;
		m_cbBaseTimes *= 2;//加一倍
		pack.bEnabled = FALSE;//默认自己不问
		m_cbAskStatus[m_wBankerUser]=0xFF;
	}
	else
	{
		m_cbFriendFlag=5;
		//标记队友
		m_wFriend[wFriend]=m_wBankerUser;
		m_wFriend[m_wBankerUser]=wFriend;
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(i != wFriend && i != m_wBankerUser)
			{
				for(int j=i+1;j<GAME_PLAYER;j++)
				{
					if(j != wFriend && j != m_wBankerUser)
					{
						m_wFriend[i]=j;
						m_wFriend[j]=i;
						break;
					}
				}
				break;
			}
		}
		CopyMemory(pack.wFriend,m_wFriend,sizeof(pack.wFriend));
		pack.bEnabled=TRUE;
	}

	m_pITableFrame->SetGameStatus(GS_ASK_FRIEND);

	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FIND_FRIEND,&pack,sizeof(pack));
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FIND_FRIEND,&pack,sizeof(pack));
	return true;
}

bool CTableFrameSink::OnUserAskFriend(WORD wChairID,bool bAsk)
{
	CMD_S_ASKFriend pack;
	ZeroMemory(&pack,sizeof(pack));

	pack.bAsk = bAsk;
	pack.wAskUser=wChairID;
	
	if(bAsk)
	{
		m_cbAskStatus[wChairID]=1;
		m_cbBaseTimes *= 2;

		//有人问了
		m_wAskUser = wChairID;
		if(m_cbFriendFlag==5)//正常
			m_cbFriendFlag=6;
		else if(m_cbFriendFlag==2)//暗独变明独
		{
			memset(m_cbAskStatus,0xFF,sizeof(m_cbAskStatus));
			m_cbFriendFlag=4;
		}


		m_pITableFrame->SetGameStatus(GS_ADD_TIMES);
		m_wCurrentUser = m_wBankerUser;
		pack.wXuanZhanUser = m_wXuanZhanUser;
		CopyMemory(pack.wFriend,m_wFriend,sizeof(pack.wFriend));
		pack.cbFriendFlag=m_cbFriendFlag;

		///////////////////////////////////////////////////////////

		//检查是否还有敌对能加倍，如果不能，直接开始游戏
		bool bFlag=false;
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(i != m_wAskUser && i != m_wFriend[m_wAskUser])
			{
				if(m_cbAskStatus[i] != 0xFF)
					bFlag=true;
			}
		}

		if(bFlag == false)
		{
			m_pITableFrame->SetGameStatus(GS_PLAY);
			m_wCurrentUser = m_wBankerUser;

			pack.bStart=true;
			pack.wCurrentUser=m_wCurrentUser;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ASK_FRIEND,&pack,sizeof(pack));
		}
		else
		{
			//1选择了问的玩家以及没来得及问的玩家可以加倍,2询问后，要等敌对加倍后才能选择加倍
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbAskStatus[i] == 0xFF || i == m_wAskUser || i == m_wFriend[m_wAskUser])
					pack.wCurrentUser = INVALID_CHAIR;
				else
					pack.wCurrentUser=i;

				m_pITableFrame->SendTableData(i,SUB_S_ASK_FRIEND,&pack,sizeof(pack));
			}
		}
	}
	else
	{
		m_cbAskStatus[wChairID]=0xFF;
		WORD wUser = (wChairID+1)%GAME_PLAYER;
		if(wUser == m_wBankerUser)//都不问，直接出牌
		{
			m_pITableFrame->SetGameStatus(GS_PLAY);
		}
		m_wCurrentUser = wUser;
		pack.wCurrentUser = wUser;
		pack.cbFriendFlag=m_cbFriendFlag;
		CopyMemory(pack.wFriend,m_wFriend,sizeof(pack.wFriend));
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ASK_FRIEND,&pack,sizeof(pack));
	}

	
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ASK_FRIEND,&pack,sizeof(pack));
	

	return true;
}
//用户加倍
bool CTableFrameSink::OnUserAddTimes(WORD wChairID, bool bAddTimes)
{
	//效验参数
	ASSERT(m_bAddTimes[wChairID]==FALSE);
	if(m_bAddTimes[wChairID]==TRUE) return false;

	m_bAddTimes[wChairID]=bAddTimes;
	m_cbAskStatus[wChairID]=0xFF;

	if(bAddTimes == false)//敌对方都不加倍，询问方不能加倍
	{
		BYTE cbCount=0;
		for(WORD i=0;i<GAME_PLAYER;i++)
		{
			if(i != m_wAskUser && i != m_wFriend[m_wAskUser])
			{
				if(m_cbAskStatus[i] == 0xFF && m_bAddTimes[i]==false)
					cbCount++;
			}
		}

		if(cbCount >= 2)
		{
			if(m_cbFriendFlag == 6)
			{
				m_cbAskStatus[m_wAskUser]=0xFF;
				m_bAddTimes[m_wAskUser]=false;
				m_cbAskStatus[m_wFriend[m_wAskUser]]=0xFF;
				m_bAddTimes[m_wFriend[m_wAskUser]]=false;
			}
			else if(m_cbFriendFlag == 1 && cbCount == 3)
			{
				m_cbAskStatus[m_wXuanZhanUser]=0xFF;
				m_bAddTimes[m_wXuanZhanUser]=false;
			}
		}

	}


	CMD_S_AddTimes pack;
	ZeroMemory(&pack,sizeof(pack));
	pack.bAddTimes=bAddTimes;
	pack.wAddTimesUser=wChairID;
	pack.wCurrentUser=m_wBankerUser;
	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbAskStatus[i] != 0xFF)
		{
			//还有人没有回答
			pack.wCurrentUser=INVALID_CHAIR;
			break;
		}
	}
	if(pack.wCurrentUser != INVALID_CHAIR)
	{
		m_pITableFrame->SetGameStatus(GS_PLAY);
		m_wCurrentUser=m_wBankerUser;
	}
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_TIMES,&pack,sizeof(pack));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_TIMES,&pack,sizeof(pack));

	if(pack.wCurrentUser == INVALID_CHAIR && bAddTimes)//询问方的敌对加倍后，通知询问方能加倍的玩家操作
	{
		CMD_S_AddTimes packNotify;
		ZeroMemory(&packNotify,sizeof(packNotify));
		packNotify.wAddTimesUser=INVALID_CHAIR;
		packNotify.wCurrentUser=INVALID_CHAIR;

		if(m_cbFriendFlag == 1 && m_cbAskStatus[m_wXuanZhanUser] != 0xFF)
		{
			m_pITableFrame->SendTableData(m_wXuanZhanUser,SUB_S_ADD_TIMES,&packNotify,sizeof(packNotify));
		}
		else if(m_cbFriendFlag == 6)
		{
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(i != wChairID && i != m_wFriend[wChairID] && m_cbAskStatus[i] != 0xFF)
				{
					m_pITableFrame->SendTableData(i,SUB_S_ADD_TIMES,&packNotify,sizeof(packNotify));
				}
			}
		}

	}

	return true;
}


//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount,bool bTrusteeOut/*=false*/)
{
	//效验状态
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return false;

	//获取类型
	BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);

	//类型判断
	if (cbCardType==CT_ERROR) 
	{
		ASSERT(FALSE);
		return false;
	}

	//出牌判断
	if (m_cbTurnCardCount!=0)
	{
		//对比扑克
		if (m_GameLogic.CompareCard(m_cbTurnCardData,cbCardData,m_cbTurnCardCount,cbCardCount)==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//删除扑克
	if (m_GameLogic.RemoveCardList(cbCardData,cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//出牌变量
	m_cbOutCardCount[wChairID]++;

	//设置变量
	m_cbTurnCardCount=cbCardCount;
	m_cbHandCardCount[wChairID]-=cbCardCount;
	CopyMemory(m_cbTurnCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//统计这一手牌分数
	for(int i=0;i<m_cbTurnCardCount;i++)
	{
		BYTE value = m_GameLogic.GetCardLogicValue(m_cbTurnCardData[i]);
		if(value==5 || value==10 || value==13)
		{
			m_cbCurTurnScore += (value==5?5:10);
		}
	}
	
	if(m_bGameFinish)//游戏已经结束
	{
		m_cbTurnCardCount=0;
		m_cbGameScore[wChairID] += m_cbCurTurnScore;
		m_cbCurTurnScore=0;

		return OnEventGameConclude(wChairID,NULL,GER_NORMAL);
	}
	//切换用户
	m_wTurnWiner=wChairID;

	if(m_cbFinishUserCount<GAME_PLAYER)
	{
		do
		{
			m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
		}while(m_cbHandCardCount[m_wCurrentUser]==0);
	}

	if (m_cbHandCardCount[wChairID] == 0)//玩家出完牌了，判断是否结束
	{
		m_wFinishUser[m_cbFinishUserCount++]=wChairID;

		if(m_wXuanZhanUser != INVALID_CHAIR)//独或宣，直接结束
		{
			//m_wCurrentUser=INVALID_CHAIR;
			m_bGameFinish=true;
			if(wChairID == m_wXuanZhanUser)
				m_bWinFlag[wChairID]=TRUE;
			else
			{
				for(int i=0;i<GAME_PLAYER;i++)
				{
					if(i != m_wXuanZhanUser)
					{
						m_bWinFlag[i]=TRUE;
					}
				}
			}
		}
		else
		{
			if(m_cbFinishUserCount==1)
			{
// 				WORD wFirst = m_wFinishUser[0];
// 				if(m_cbGameScore[wFirst] + m_cbGameScore[m_wFriend[wFirst]]>=50 )//先走的一方分数够50,已经赢了
// 				{
// 					m_wCurrentUser=INVALID_CHAIR;
// 					m_bWinFlag[wFirst]=TRUE;
// 					m_bWinFlag[m_wFriend[wFirst]]=TRUE;
// 				}
			}
			else if(m_cbFinishUserCount == 2)
			{
				WORD wFirst = m_wFinishUser[0];
				if( (m_wFriend[wFirst] == m_wFinishUser[1]) || (m_cbGameScore[wFirst] + m_cbGameScore[m_wFriend[wFirst]]>=50) )//一方都出或先走的一方分数够50
				{
					//m_wCurrentUser=INVALID_CHAIR;
					m_bGameFinish=true;
					m_bWinFlag[wFirst]=TRUE;
					m_bWinFlag[m_wFriend[wFirst]]=TRUE;
					if(m_wFriend[wFirst] == m_wFinishUser[1])//一方都走，双扣
						m_bShuangKou=true;
				}
			}
			else if(m_cbFinishUserCount == 3)
			{
				//m_wCurrentUser=INVALID_CHAIR;
				m_bGameFinish=true;
				WORD wFirst = m_wFinishUser[0];
				WORD wSecond = m_wFinishUser[1];
				BYTE cbFirstScore = m_cbGameScore[wFirst]+m_cbGameScore[m_wFriend[wFirst]];
				BYTE cbSecondScore = m_cbGameScore[wSecond]+m_cbGameScore[m_wFriend[wSecond]];
				if(m_wFriend[wFirst] == wChairID)
				{
					cbFirstScore += 30;
					cbSecondScore -= 30;

					m_cbGameScore[wFirst]+=30;
					m_cbGameScore[m_wFriend[wSecond]]-=30;
					
				}

				if(cbFirstScore >= cbSecondScore)
				{
					m_bWinFlag[wFirst]=TRUE;
					m_bWinFlag[m_wFriend[wFirst]]=TRUE;
				}
				else
				{
					m_bWinFlag[wSecond]=TRUE;
					m_bWinFlag[m_wFriend[wSecond]]=TRUE;
				}
			}

		}

	}
	//构造数据
	CMD_S_OutCard OutCard;
	ZeroMemory(&OutCard,sizeof(OutCard));
	OutCard.wOutCardUser=wChairID;
	OutCard.cbCardCount=cbCardCount;
	OutCard.wCurrentUser=m_wCurrentUser;
	OutCard.cbCurTurnScore=m_cbCurTurnScore;
	OutCard.bTrusteeOut = bTrusteeOut;
	CopyMemory(OutCard.cbCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	if(m_cbFriendFlag==5 || m_cbFriendFlag==2)//同盟未知情况下，选择的牌打出了选择的牌，显示关系
	{
		for(int i=0;i<m_cbTurnCardCount;i++)
		{
			if(m_cbSelectedCardData == m_cbTurnCardData[i])
			{
				if(m_cbFriendFlag==5)
					m_cbFriendFlag=6;//队友明示
				else
					m_cbFriendFlag=3;//暗独明示

				CMD_S_Friend pack;
				pack.cbFriendFlag=m_cbFriendFlag;
				pack.wXuanZhanUser=m_wXuanZhanUser;
				CopyMemory(pack.wFriend,m_wFriend,sizeof(pack.wFriend));
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FRIEND,&pack,sizeof(pack));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FRIEND,&pack,sizeof(pack));
				break;
			}
		}
	}


	m_bCurrenUserDone=false;

	//结束判断,双扣、明独、暗独、宣战的情况下，不再判断最后分数归属
	if (m_bGameFinish && (m_cbCurTurnScore==0 || m_wXuanZhanUser != INVALID_CHAIR || (m_cbFinishUserCount==2 && m_wFinishUser[0]==m_wFinishUser[1]) ))
	{
		OnEventGameConclude(wChairID,NULL,GER_NORMAL);
	}
	else if(m_bTrustee[m_wCurrentUser] == true)
	{
		m_bCurrenUserDone=true;
		m_pITableFrame->SetGameTimer(IDI_USER_ACTION,1000,-1,NULL);
	}
	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID,bool bTrusteePass/*=false*/)
{
	//效验状态
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbTurnCardCount!=0));
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return false;

	do
	{
		m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
		if (m_wCurrentUser==m_wTurnWiner)
		{
			m_cbTurnCardCount=0;
			m_cbGameScore[m_wTurnWiner] += m_cbCurTurnScore;
			m_cbCurTurnScore=0;

			if(m_bGameFinish)
			{
				return OnEventGameConclude(wChairID,NULL,GER_NORMAL);
			}
			if(m_cbFriendFlag == 6 && m_cbHandCardCount[m_wCurrentUser]==0)//出完牌，在同盟确定情况下，由同盟接风
			{
				ASSERT(m_cbHandCardCount[m_wFriend[m_wCurrentUser]] > 0);
				m_wCurrentUser = m_wFriend[m_wCurrentUser];
			}
		}

	}while(m_cbHandCardCount[m_wCurrentUser]==0 );

	if(m_cbFinishUserCount==2)
	{
		WORD wFirstFinishUser = m_wFinishUser[0];
		BYTE cbScore = m_cbGameScore[wFirstFinishUser] + m_cbGameScore[m_wFriend[wFirstFinishUser]];
		if(cbScore>=50)
			m_wCurrentUser=INVALID_CHAIR;
	}
	//构造消息
	CMD_S_PassCard PassCard;
	PassCard.wPassCardUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.cbTurnOver=(m_cbTurnCardCount==0)?TRUE:FALSE;
	PassCard.cbTurnWinnerScore = m_cbGameScore[m_wTurnWiner];
	PassCard.wTurnWinner=m_wTurnWiner;
	PassCard.bTrusteePass=bTrusteePass;
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	m_bCurrenUserDone=false;
	if(m_wCurrentUser != INVALID_CHAIR && m_bTrustee[m_wCurrentUser])
	{
		m_bCurrenUserDone=true;
		m_pITableFrame->SetGameTimer(IDI_USER_ACTION,1000,-1,NULL);
	}
	
	if(m_wCurrentUser == INVALID_CHAIR)
	{
		WORD wFirst=m_wFinishUser[0];
		m_bWinFlag[wFirst]=TRUE;
		m_bWinFlag[m_wFriend[wFirst]]=TRUE;
		OnEventGameConclude(wChairID,NULL,GER_NORMAL);
	}
	return true;
}

bool CTableFrameSink::OnUserTrustee(WORD wChairID,bool bTrustee)
{
	if(m_bTrustee[wChairID] != bTrustee)
	{
		CMD_S_Trustee pack;
		pack.wChair = wChairID;
		pack.bTrustee = bTrustee;

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&pack,sizeof(pack));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&pack,sizeof(pack));
	}

	m_bTrustee[wChairID]=bTrustee;

	if(bTrustee && m_wCurrentUser == wChairID && !m_bCurrenUserDone)
	{
		AutomatismOutCard();
	}

	return true;
}


//自动出牌
bool CTableFrameSink::AutomatismOutCard()
{
	//出牌处理
	m_GameLogic.SearchOutCard(m_cbHandCardData[m_wCurrentUser],m_cbHandCardCount[m_wCurrentUser],m_cbTurnCardData,m_cbTurnCardCount,&m_SearchCardResult);
	//设置界面
	if (m_SearchCardResult.cbSearchCount>0)
	{
		OnUserOutCard(m_wCurrentUser,m_SearchCardResult.cbResultCard[0],m_SearchCardResult.cbCardCount[0],true);
		return true;
	}
	else if(m_cbTurnCardCount != 0)
	{
		OnUserPassCard(m_wCurrentUser,true);
		return true;
	}

	return true;
}


//设置基数
void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

bool CTableFrameSink::OnUserFlee(WORD wChairID)
{
	m_pITableFrame->KillGameTimer(IDI_USER_ACTION);
	//变量定义
	CMD_S_GameConclude GameConclude;
	ZeroMemory(&GameConclude, sizeof(GameConclude));

	//设置变量
	GameConclude.cbBaseTimes = m_cbBaseTimes;
	GameConclude.lCellScore = m_pITableFrame->GetCellScore();
	CopyMemory(GameConclude.cbScore,m_cbGameScore,sizeof(GameConclude.cbScore));
	//用户扑克
	BYTE cbCardIndex = 0;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//拷贝扑克
		GameConclude.cbCardCount[i] = m_cbHandCardCount[i];
		CopyMemory(&GameConclude.cbHandCardData[cbCardIndex], m_cbHandCardData[i], m_cbHandCardCount[i]*sizeof(BYTE));

		//设置索引
		cbCardIndex += m_cbHandCardCount[i];
	}

	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		m_bWinFlag[i] = (i!=wChairID);

		GameConclude.cbUserTimes[i]=m_cbBaseTimes*(m_bAddTimes[i]?2:1);
	}
	BYTE cbWinUserCount=3;
	BYTE cbLoseUserCount=1;

	//积分变量
	tagScoreInfo ScoreInfoArray[GAME_PLAYER];
	ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

	//统计积分

	SCORE lCellScore = m_pITableFrame->GetCellScore();

	SCORE lUserScore[GAME_PLAYER]={0};
	for(WORD i=0;i<GAME_PLAYER;i++)
		lUserScore[i] = m_pITableFrame->GetTableUserItem(i)->GetUserScore();

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_bWinFlag[i] == TRUE)
		{

			for(WORD j=0;j<GAME_PLAYER;j++)
			{
				SCORE LoseScore = lCellScore*m_cbBaseTimes;
				if(m_bAddTimes[i])
					LoseScore *= 2;

				if(m_bWinFlag[j] == FALSE)
				{
					if(m_bAddTimes[j])
						LoseScore *= 2;

					if(LoseScore > lUserScore[j])
					{
						LoseScore = lUserScore[j];
					}

					lUserScore[j] -= LoseScore;
					ScoreInfoArray[j].lScore -= LoseScore;
					ScoreInfoArray[j].cbType = SCORE_TYPE_FLEE;

					ScoreInfoArray[i].lScore += LoseScore;
				}
			}

			ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
			ScoreInfoArray[i].lRevenue = m_pITableFrame->CalculateRevenue(i,ScoreInfoArray[i].lScore);
			if(ScoreInfoArray[i].lRevenue > 0L)
				ScoreInfoArray[i].lScore -= ScoreInfoArray[i].lRevenue;
		}

	}

	for(int i=0;i<GAME_PLAYER;i++)
	{
		GameConclude.lGameScore[i] = ScoreInfoArray[i].lScore;
		m_HistoryScore.OnEventUserScore(i,GameConclude.lGameScore[i]);
	}
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

	//写入积分
	m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));

	//结束游戏
	m_pITableFrame->ConcludeGame(GS_GAME_FREE);

	return true;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
