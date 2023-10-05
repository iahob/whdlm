#include "StdAfx.h"
#include "TableFrameSink.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////////////
#define IDI_CHECK_TABLE				1					//检查桌子
#define IDI_USER_OUT_TIME			6					//玩家超时
#define IDI_GOOD_CARD_TIME			2					//检查好牌

HINSTANCE				CTableFrameSink::m_hDebugInst = NULL;
IServerDebug*			CTableFrameSink::m_pServerDebug = NULL;

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameCustomRule=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;
	m_bOffLineTrustee = false;

	m_cbTrusteeDelayTime = 5;

	//炸弹变量
	m_wFirstUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));
	ZeroMemory(m_cbUserTrustee,sizeof(m_cbUserTrustee));
	m_lScoreTimes = 0L;

	//游戏变量
	m_wTimerDebug=0;
	m_cbBombCount=0;
	ZeroMemory(m_cbEachBombCount,sizeof(m_cbEachBombCount));

	//叫分信息
	m_cbBankerScore=0;
	ZeroMemory(m_cbScoreInfo,sizeof(m_cbScoreInfo));
	m_cbCallScoreCount=0;

	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//扑克信息
	ZeroMemory(m_cbBankerCard,sizeof(m_cbBankerCard));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//服务调试
	if (m_hDebugInst == NULL)
	{
		m_hDebugInst = LoadLibrary(TEXT("LandServerDebug.dll"));
		if (m_hDebugInst)
		{
			typedef void * (*CREATE)();
			CREATE ServerDebug = (CREATE)GetProcAddress(m_hDebugInst, "CreateServerDebug");
			if (ServerDebug)
			{
				m_pServerDebug = static_cast<IServerDebug*>(ServerDebug());
			}
		}
	}
	//游戏视频
	m_hVideoInst = NULL;
	m_pGameVideo = NULL;
	m_hVideoInst = LoadLibrary(TEXT("LandGameVideo.dll"));
	if ( m_hVideoInst )
	{
		typedef void * (*CREATE)(); 
		CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst,"CreateGameVideo"); 
		if ( GameVideo )
		{
			m_pGameVideo = static_cast<IGameVideo*>(GameVideo());
		}
	}	
	
	ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		m_listWinScoreRecord[i].RemoveAll();
	}

	ZeroMemory(m_userachievement, sizeof(m_userachievement));
	InitializeListRemainCard();

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
	if( m_pServerDebug != NULL)
	{
		delete m_pServerDebug;
		m_pServerDebug = NULL;
	}

	if( m_hDebugInst )
	{
		FreeLibrary(m_hDebugInst);
		m_hDebugInst = NULL;
	}
	
	if(m_pGameVideo)
	{
		delete m_pGameVideo;
		m_pGameVideo = NULL;
	}
	
	if(m_hVideoInst)
	{
		FreeLibrary(m_hVideoInst);
		m_hVideoInst = NULL;
	}
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
	m_cbBombCount=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));
	ZeroMemory(m_cbEachBombCount,sizeof(m_cbEachBombCount));
	ZeroMemory(m_cbUserTrustee,sizeof(m_cbUserTrustee));
	m_lScoreTimes = 0L;

	//叫分信息
	m_cbBankerScore=0;
	ZeroMemory(m_cbScoreInfo,sizeof(m_cbScoreInfo));
	m_cbCallScoreCount=0;

	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//扑克信息
	ZeroMemory(m_cbBankerCard,sizeof(m_cbBankerCard));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

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

	m_bOffLineTrustee = CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule);
	m_cbTrusteeDelayTime = m_pGameCustomRule->cbTrusteeDelayTime;
		
	//启动好牌更新定时器
	if (m_pServerDebug != NULL && m_pITableFrame->GetTableID() == 0)
	{
		m_pITableFrame->SetGameTimer(IDI_GOOD_CARD_TIME, 1000, -1, NULL);
	}

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
	return 0L;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	////////////////////////////////////////////////////////////
	
	InitializeListRemainCard();

	//积分房卡个人成就
	if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
	{
		//新的私人房开局标志
		bool bNewAround = false;
		for (WORD i = 0; i<GAME_PLAYER; i++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (!pIServerUserItem)
			{
				continue;
			}

			if (m_userachievement[i].dwGameID != 0 && m_userachievement[i].dwGameID != pIServerUserItem->GetGameID())
			{
				bNewAround = true;
				break;
			}
		}

		//新开局重置
		if (bNewAround)
		{
			ZeroMemory(m_userachievement, sizeof(m_userachievement));
		}

		for (WORD i = 0; i<GAME_PLAYER; i++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (!pIServerUserItem)
			{
				continue;
			}

			tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
			m_userachievement[i].wFaceID = pUserInfo->wFaceID;
			m_userachievement[i].dwCustomID = pUserInfo->dwCustomID;
			m_userachievement[i].dwGameID = pUserInfo->dwGameID;
			CopyMemory(m_userachievement[i].szNickName, pUserInfo->szNickName, sizeof(m_userachievement[i].szNickName));
		}
	}

	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_CALL);
	m_ctChangeTime = CTime::GetCurrentTime();

	//混乱扑克
	BYTE cbRandCard[FULL_COUNT];
	m_GameLogic.RandCardList(cbRandCard,CountArray(cbRandCard));

// 
 #ifdef _DEBUG

	//0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D, 方块
	//0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D, 梅花
	//0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D, 红桃
	//0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D, 黑桃

 	//BYTE cbTempRandCard[FULL_COUNT]=
 	//{
 	//	0x11,0x11,0x1D,0x0C,0x0B,0x0A,0x0A,0x09,0x07,0x07,0x07,0x07,0x06,0x06,0x06,0x05,0x05,
		//0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
		//0x29,0x2A,0x2B,0x2C,0x2D,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,
		//0x05,0x05,0x04,
 	//};
 
 	//CopyMemory(cbRandCard, cbTempRandCard, sizeof(cbRandCard));
 #endif
	
	//比赛房间随机第一个随机叫分
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0 )
	{
		m_wFirstUser = INVALID_CHAIR;
	}

	//抽取明牌
	BYTE cbValidCardData=0;
	BYTE cbValidCardIndex=0;
	WORD wStartUser=m_wFirstUser;
	WORD wCurrentUser=m_wFirstUser;

	//抽取玩家
	if (wStartUser==INVALID_CHAIR)
	{
		//抽取扑克
		cbValidCardIndex=rand()%DISPATCH_COUNT;
		cbValidCardData=cbRandCard[cbValidCardIndex];

		//设置用户
		wStartUser=m_GameLogic.GetCardValue(cbValidCardData)%GAME_PLAYER;
		wCurrentUser=(wStartUser+cbValidCardIndex/NORMAL_COUNT)%GAME_PLAYER;
	}

	
	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wUserIndex=(wStartUser+i)%GAME_PLAYER;
		m_cbHandCardCount[wUserIndex]=NORMAL_COUNT;
		CopyMemory(&m_cbHandCardData[wUserIndex],&cbRandCard[i*m_cbHandCardCount[wUserIndex]],sizeof(BYTE)*m_cbHandCardCount[wUserIndex]);
	}

	//设置底牌
	CopyMemory(m_cbBankerCard,&cbRandCard[DISPATCH_COUNT],sizeof(m_cbBankerCard));

	//调试好牌
	if (m_pServerDebug != NULL)
	{
		bool bControlGoodCard = m_pServerDebug->GetDebugInfo(m_pITableFrame, m_cbBankerCard, m_cbHandCardCount, m_cbHandCardData);
	}

	//AI数据
	CMD_S_AndroidCard AndroidCard ;
	ZeroMemory(&AndroidCard, sizeof(AndroidCard)) ;

	//用户扑克
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		CopyMemory(AndroidCard.cbHandCard[i], m_cbHandCardData[i], sizeof(BYTE)*m_cbHandCardCount[i]);
	}

	//设置用户
	m_wFirstUser=wCurrentUser;
	m_wCurrentUser=wCurrentUser;

	//构造变量
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));
	GameStart.lCellScore = m_pITableFrame->GetCellScore();
	GameStart.wStartUser=wStartUser;
	GameStart.wCurrentUser=wCurrentUser;
	GameStart.cbValidCardData=cbValidCardData;
	GameStart.cbValidCardIndex=cbValidCardIndex;

	AndroidCard.wCurrentUser = m_wCurrentUser;
	CopyMemory(AndroidCard.cbBankerCard,m_cbBankerCard,3*sizeof(BYTE));

	if(m_pGameVideo)
	{
		m_pGameVideo->StartVideo(m_pITableFrame);
	}
	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//构造扑克
		ASSERT(CountArray(GameStart.cbCardData)>=m_cbHandCardCount[i]);
		CopyMemory(GameStart.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);

		//发送数据
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i) ;
		if(!pServerUserItem->IsAndroidUser())
		{
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		else 
		{
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&AndroidCard,sizeof(AndroidCard));
		}
	
		CString strConclude;
		CTime time = CTime::GetCurrentTime();
		CString strSingleUser;
		if (i == 2)
			strSingleUser.Format(TEXT("游戏开始，桌子ID=%d,椅子ID=%d,玩家【%s】,AI标志：%d \n"), m_pITableFrame->GetTableID(), i, pServerUserItem->GetNickName(), pServerUserItem->IsAndroidUser());
		else
			strSingleUser.Format(TEXT("游戏开始，桌子ID=%d,椅子ID=%d,玩家【%s】,AI标志：%d"), m_pITableFrame->GetTableID(), i,pServerUserItem->GetNickName(), pServerUserItem->IsAndroidUser());
		strConclude += strSingleUser;
		CString strdebugB;
        strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
        m_pITableFrame->SendGameLogData(strdebugB);

		if (m_pGameVideo)
		{
			Video_GameStart video;
			lstrcpyn(video.szNickName,pServerUserItem->GetNickName(),CountArray(video.szNickName));
			video.wTableID			= i;
			video.wStartUser		= wStartUser;
			video.wCurrentUser		= wCurrentUser;
			video.cbValidCardData	= cbValidCardData;
			video.cbValidCardIndex	= cbValidCardIndex;
			CopyMemory(video.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
			video.lScore			= pServerUserItem->GetUserScore();
			video.lCellScore		= m_pITableFrame->GetCellScore();;
			m_pGameVideo->AddVideoData(SUB_S_GAME_START,&video,i==0?true:false);
		}
	}

	//WB改
	SendDebugCard();

	//排列扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i],ST_ORDER);
	}

	//启动离线托管
	IsOfflineTrustee();
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("游戏结束，桌子ID=%d,椅子ID=%d,玩家【%s】,AI标志：%d "), m_pITableFrame->GetTableID(), wChairID, pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	else 
		strSingleUser.Format(TEXT("游戏结束，桌子ID=%d,椅子ID=%d,cbReason：%d "), m_pITableFrame->GetTableID(), wChairID, cbReason);
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//删除超时定时器
			m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				IServerUserItem * pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (!pIKeyServerUserItem)
				{
					continue;
				}

				//非比赛情况下游戏结束取消托管
				if (m_pGameServiceOption->wServerType != GAME_GENRE_MATCH)
				{
					pIKeyServerUserItem->SetTrusteeUser(false);
				}
				
			}

			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude,sizeof(GameConclude));

			//设置变量
			GameConclude.cbBankerScore=m_cbBankerScore;
			GameConclude.lCellScore=m_pITableFrame->GetCellScore();

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(GameConclude.remainCardCount), &remaincard, sizeof(remaincard));

			//炸弹信息
			GameConclude.cbBombCount=m_cbBombCount;
			CopyMemory(GameConclude.cbEachBombCount,m_cbEachBombCount,sizeof(GameConclude.cbEachBombCount));

			//用户扑克
			BYTE cbCardIndex=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//拷贝扑克
				GameConclude.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));

				//设置索引
				cbCardIndex+=m_cbHandCardCount[i];
			}

			//炸弹统计
			LONG lScoreTimes=1L * m_cbBankerScore;
			for (BYTE i=0;i<m_cbBombCount;i++) lScoreTimes*=2L;

			//春天判断
			if (wChairID==m_wBankerUser)
			{
				//用户定义
				WORD wUser1=(m_wBankerUser+1)%GAME_PLAYER;
				WORD wUser2=(m_wBankerUser+2)%GAME_PLAYER;

				//用户判断
				if ((m_cbOutCardCount[wUser1]==0)&&(m_cbOutCardCount[wUser2]==0)) 
				{
					lScoreTimes*=2L;
					GameConclude.bChunTian=TRUE;

					//积分房卡个人成就
					if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
					  && m_userachievement[wChairID].dwGameID == m_pITableFrame->GetTableUserItem(wChairID)->GetGameID())
					{
						m_userachievement[wChairID].cbChunTianCount++;
					}
				}
			}

			//春天判断
			if (wChairID!=m_wBankerUser)
			{
				if (m_cbOutCardCount[m_wBankerUser]==1)
				{
					lScoreTimes*=2L;
					GameConclude.bFanChunTian=TRUE;

					//积分房卡个人成就
					if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
						&& m_userachievement[m_wBankerUser].dwGameID == m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetGameID())
					{
						m_userachievement[m_wBankerUser].cbFanChunTianCount++;
					}
				}
			}

			//调整倍数
			lScoreTimes=__min(m_pGameCustomRule->wMaxScoreTimes,lScoreTimes);

			m_lScoreTimes = lScoreTimes;

			//农民托管数量
			WORD wTrusteeCount=0;
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
			{
				for(WORD i = 0; i < GAME_PLAYER; i++)
				{
					if (i!=m_wBankerUser)
					{
						if (m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser()==true) wTrusteeCount++;
					}
				}
			}

			//CString strCellScore;
			//strCellScore.Format(TEXT("GameConcludeCellScore = %d"), m_pITableFrame->GetCellScore());
			//CTraceService::TraceString(strCellScore, TraceLevel_Exception);

			//积分变量
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			//农民得分
			LONGLONG lPlayerWinScore = 0;

			//统计积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				SCORE lUserScore=0L;
				SCORE lCellScore=m_pITableFrame->GetCellScore();
				bool bTrusteeUser=m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser();
				
				//积分基数
				if (i==m_wBankerUser)
				{
					lUserScore=(m_cbHandCardCount[m_wBankerUser]==0)?2L:-2L;
				}
				else
				{
					if (m_cbHandCardCount[m_wBankerUser]==0)
					{
						if (wTrusteeCount>0) lUserScore=(bTrusteeUser==true)?(-2L/wTrusteeCount):0L;
						else lUserScore=-1L;
					}
					else
					{
						if (wTrusteeCount>0) lUserScore=(bTrusteeUser==true)?0L:(2L/wTrusteeCount);
						else lUserScore=1L;
					}
				}

				//设置积分
				GameConclude.lGameScore[i] = lUserScore*lCellScore*lScoreTimes;

				if (i != m_wBankerUser && lPlayerWinScore == 0)
				{
					lPlayerWinScore = GameConclude.lGameScore[i];
				}
			}

			//调整得分
			//地主赢
			if (m_cbHandCardCount[m_wBankerUser] == 0)
			{
				LONGLONG lBankerCarryScore = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore();

				//校验农民能否赔付
				LONGLONG lBankerWinScore = 0;
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					//
					if (i == m_wBankerUser)
					{
						continue;
					}

					IServerUserItem *pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (!pIKeyServerUserItem)
					{
						continue;
					}

					ASSERT(GameConclude.lGameScore[i] < 0);
					if (-GameConclude.lGameScore[i] > pIKeyServerUserItem->GetUserScore())
					{
						GameConclude.lGameScore[i] = -pIKeyServerUserItem->GetUserScore();
					}

					GameConclude.lGameScore[i] = -(min(-GameConclude.lGameScore[i], lBankerCarryScore / 2));

					lBankerWinScore += (-GameConclude.lGameScore[i]);
				}

				GameConclude.lGameScore[m_wBankerUser] = lBankerWinScore;
			}
			//农民赢
			else
			{
				ASSERT(lPlayerWinScore > 0);

				//判断地主携带金币是否够赔
				if (m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore() >= 2 * lPlayerWinScore)
				{
					//校验农民赢的钱是否超过身上携带
					LONGLONG lBankerWinScore = 0;
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{
						//
						if (i == m_wBankerUser)
						{
							continue;
						}

						IServerUserItem *pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (!pIKeyServerUserItem)
						{
							continue;
						}

						ASSERT(GameConclude.lGameScore[i] > 0);
						if (GameConclude.lGameScore[i] > pIKeyServerUserItem->GetUserScore())
						{
							GameConclude.lGameScore[i] = pIKeyServerUserItem->GetUserScore();
						}

						lBankerWinScore += (-GameConclude.lGameScore[i]);
					}

					GameConclude.lGameScore[m_wBankerUser] = lBankerWinScore;
				}
				//地主不够赔
				else
				{
					//携带分平均分
					LONGLONG lBankerCarryScore = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore();
					LONGLONG lBankerWinScore = 0;
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{
						//
						if (i == m_wBankerUser)
						{
							continue;
						}

						IServerUserItem *pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (!pIKeyServerUserItem)
						{
							continue;
						}

						ASSERT(GameConclude.lGameScore[i] > 0);
						if (lBankerCarryScore / 2 > pIKeyServerUserItem->GetUserScore())
						{
							GameConclude.lGameScore[i] = pIKeyServerUserItem->GetUserScore();
						}
						else
						{
							GameConclude.lGameScore[i] = lBankerCarryScore / 2;
						}

						lBankerWinScore += (-GameConclude.lGameScore[i]);
					}

					GameConclude.lGameScore[m_wBankerUser] = lBankerWinScore;
				}
			}

			//计算税收
			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				ScoreInfoArray[i].lScore=GameConclude.lGameScore[i];
				ScoreInfoArray[i].cbType=(ScoreInfoArray[i].lScore>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

				if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
					((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
				{
					//计算税收
					ScoreInfoArray[i].lRevenue = m_pITableFrame->CalculateRevenue(i, ScoreInfoArray[i].lScore);
					if (ScoreInfoArray[i].lRevenue>0L) ScoreInfoArray[i].lScore -= ScoreInfoArray[i].lRevenue;
				}

				//设置积分
				GameConclude.lGameScore[i]=ScoreInfoArray[i].lScore;

				//历史积分
				m_HistoryScore.OnEventUserScore(i,GameConclude.lGameScore[i]);

				//房卡模式
				//积分房卡
				if ((m_pITableFrame->GetDataBaseMode() == 0) &&((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && !m_pITableFrame->IsPersonalRoomDisumme())
				{
					if (m_RoomCardRecord.nCount < MAX_RECORD_COUNT)
					{
						m_RoomCardRecord.lDetailScore[i][m_RoomCardRecord.nCount] = ScoreInfoArray[i].lScore;
					}
				}
				//金币房卡
				else if ((m_pITableFrame->GetDataBaseMode() == 1) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
				{
					m_listWinScoreRecord[i].AddHead(ScoreInfoArray[i].lScore);
				}
			}

			//房卡模式
			if ((m_pITableFrame->GetDataBaseMode() == 0) && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				m_RoomCardRecord.nCount++;
			}

			//积分房卡个人成就
			if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				m_pITableFrame->GetSpeicalInfo((byte*)(&m_userachievement[0]), sizeof(m_userachievement));
			}

			GameConclude.lScoreTimes = m_lScoreTimes;

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));

			if(m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_CONCLUDE,&GameConclude);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
			}
			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//切换用户
			m_wFirstUser=wChairID;

			//更新调试得分
			if (m_pServerDebug != NULL)
			{
				m_pServerDebug->UpdateDebugInfo(m_pITableFrame, ScoreInfoArray);
			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
				
			////////////////////////////////////////////////////////////
			CString strConclude;
			CTime time = CTime::GetCurrentTime();
			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				IServerUserItem *pIConcludeUserItem = m_pITableFrame->GetTableUserItem(i);
				if (pIConcludeUserItem)
				{
					CString strSingleUser;
					strSingleUser.Format(TEXT("游戏结束，桌子ID=%d,昵称【%s】GAMEID = %d 得分 = %d \n"), m_pITableFrame->GetTableID(), pIConcludeUserItem->GetNickName(), pIConcludeUserItem->GetGameID(), GameConclude.lGameScore[i]);
					strConclude += strSingleUser;
				}
				else
				{
					CString strSingleUser;
					strSingleUser.Format(TEXT("游戏结束，桌子ID=%d,chairID = %d 得分 = %d \n"), m_pITableFrame->GetTableID(), i, GameConclude.lGameScore[i]);
					strConclude += strSingleUser;
				}
			}

			CString strdebugB;
            strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s, 重置空闲 剩余的牌张数 [%d][%d][%d]\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude, m_cbHandCardCount[0], m_cbHandCardCount[1], m_cbHandCardCount[2]);
            m_pITableFrame->SendGameLogData(strdebugB);
			////////////////////////////////////////////////////////////

			//房卡模式
			if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_pITableFrame->IsPersonalRoomDisumme())
			{
				//约战解散
				ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
				ZeroMemory(m_userachievement, sizeof(m_userachievement));
			}

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//删除超时定时器
			m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				IServerUserItem * pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (!pIKeyServerUserItem)
				{
					continue;
				}

				//非比赛情况下游戏结束取消托管
				if (m_pGameServiceOption->wServerType != GAME_GENRE_MATCH)
				{
					pIKeyServerUserItem->SetTrusteeUser(false);
				}
			}

			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude,sizeof(GameConclude));

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(GameConclude.remainCardCount), &remaincard, sizeof(remaincard));

			//用户扑克
			BYTE cbCardIndex=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//拷贝扑克
				GameConclude.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));

				//设置索引
				cbCardIndex+=m_cbHandCardCount[i];
			}

			//积分房卡个人成就
			if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				m_pITableFrame->GetSpeicalInfo((byte*)(&m_userachievement[0]), sizeof(m_userachievement));
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			
			//更新调试得分
			if (m_pServerDebug != NULL)
			{
				//积分变量
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
				m_pServerDebug->UpdateDebugInfo(m_pITableFrame, ScoreInfoArray);
			}

			/*if(m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_CONCLUDE,&GameConclude);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
			}*/
			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			return true;
		}
	case GER_USER_LEAVE:	//用户强退
	case GER_NETWORK_ERROR:	//网络中断
		{
			//删除超时定时器
			m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude,sizeof(GameConclude));

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(GameConclude.remainCardCount), &remaincard, sizeof(remaincard));

			//设置变量
			GameConclude.cbBankerScore=m_cbBankerScore;
			GameConclude.lCellScore=m_pITableFrame->GetCellScore();

			//炸弹信息
			GameConclude.cbBombCount=m_cbBombCount;
			CopyMemory(GameConclude.cbEachBombCount,m_cbEachBombCount,sizeof(GameConclude.cbEachBombCount));

			//用户扑克
			BYTE cbCardIndex=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//拷贝扑克
				GameConclude.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));

				//设置索引
				cbCardIndex+=m_cbHandCardCount[i];
			}

			//炸弹统计
			WORD lScoreTimes=1;
			for (WORD i=0;i<m_cbBombCount;i++) lScoreTimes*=2L;

			//调整倍数
			lScoreTimes=__min(m_pGameCustomRule->wMaxScoreTimes,lScoreTimes);

			//积分变量
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			//变量定义
			SCORE lCellScore=m_pITableFrame->GetCellScore();
			SCORE lUserScore=lCellScore*__max(lScoreTimes,m_pGameCustomRule->wFleeScoreTimes);

			//金币平衡
			if ((m_pGameServiceOption->wServerType&SCORE_GENRE_POSITIVE)!=0 || (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_EDUCATE))!=0)
			{
				lUserScore=__min(pIServerUserItem->GetUserScore(),lUserScore);
			}

			//逃跑罚分
			ScoreInfoArray[wChairID].lScore=-lUserScore;
			ScoreInfoArray[wChairID].cbType=SCORE_TYPE_FLEE;

			//分享罚分
			if (m_pGameCustomRule->cbFleeScorePatch==TRUE)
			{
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					//过滤
					if ( i == wChairID )
						continue;

					//设置积分
					ScoreInfoArray[i].lScore=lUserScore/2L;
					ScoreInfoArray[i].cbType=SCORE_TYPE_WIN;

					if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
						((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
					{
						//计算税收
						ScoreInfoArray[i].lRevenue = m_pITableFrame->CalculateRevenue(i, ScoreInfoArray[i].lScore);
						if (ScoreInfoArray[i].lRevenue>0L) ScoreInfoArray[i].lScore -= ScoreInfoArray[i].lRevenue;
					}
				}
			}

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//设置成绩
				GameConclude.lGameScore[i]=ScoreInfoArray[i].lScore;

				//历史成绩
				m_HistoryScore.OnEventUserScore(i,GameConclude.lGameScore[i]);
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			/*
			if(m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_CONCLUDE,&GameConclude);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
			}*/
			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//更新调试得分
			if (m_pServerDebug != NULL)
			{
				m_pServerDebug->UpdateDebugInfo(m_pITableFrame, ScoreInfoArray);
			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

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
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("发送场景，桌子ID=%d,椅子ID=%d,玩家【%s】,AI标志：%d，游戏状态：%d"), m_pITableFrame->GetTableID(), wChairID, pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser(), cbGameStatus);
	else 
		strSingleUser.Format(TEXT("发送场景，桌子ID=%d,椅子ID=%d,游戏状态：%d"), m_pITableFrame->GetTableID(), wChairID,cbGameStatus);
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	//CString strCellScore;
	//strCellScore.Format(TEXT("GameSceneCellScore = %d"), m_pITableFrame->GetCellScore());
	//CTraceService::TraceString(strCellScore, TraceLevel_Exception);

	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=m_pITableFrame->GetCellScore();

			//自定规则
			StatusFree.cbTimeOutCard=m_pGameCustomRule->cbTimeOutCard;		
			StatusFree.cbTimeCallScore=m_pGameCustomRule->cbTimeCallScore;
			StatusFree.cbTimeStartGame=m_pGameCustomRule->cbTimeStartGame;
			StatusFree.cbTimeHeadOutCard=m_pGameCustomRule->cbTimeHeadOutCard;

			InitializeListRemainCard();
			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(StatusFree.remainCardCount), &remaincard, sizeof(remaincard));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusFree.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//0 积分场	1 金币场
			StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SCENE_CALL:	//叫分状态
		{
			//构造数据
			CMD_S_StatusCall StatusCall;
			ZeroMemory(&StatusCall,sizeof(StatusCall));

			//单元积分
			StatusCall.lCellScore=m_pITableFrame->GetCellScore();

			//自定规则
			StatusCall.cbTimeOutCard=m_pGameCustomRule->cbTimeOutCard;		
			StatusCall.cbTimeCallScore=m_pGameCustomRule->cbTimeCallScore;
			StatusCall.cbTimeStartGame=m_pGameCustomRule->cbTimeStartGame;
			StatusCall.cbTimeHeadOutCard=m_pGameCustomRule->cbTimeHeadOutCard;

			//游戏信息
			StatusCall.wCurrentUser=m_wCurrentUser;
			StatusCall.cbBankerScore=m_cbBankerScore;
			CopyMemory(StatusCall.cbScoreInfo,m_cbScoreInfo,sizeof(m_cbScoreInfo));
			CopyMemory(StatusCall.cbHandCardData,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID]*sizeof(BYTE));
			CopyMemory(StatusCall.cbUserTrustee,m_cbUserTrustee,sizeof(m_cbUserTrustee));

			StatusCall.lScoreTimes = m_lScoreTimes;

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(StatusCall.remainCardCount), &remaincard, sizeof(remaincard));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusCall.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusCall.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//0 积分场	1 金币场
			StatusCall.cbPlayMode = m_pITableFrame->GetDataBaseMode();

			CTime ctCurTime = CTime::GetCurrentTime();
			CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;
			StatusCall.cbTimeRemain = StatusCall.cbTimeCallScore - TimeSpan.GetSeconds();

			//发送场景
			bool bSendResult = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusCall,sizeof(StatusCall));

			if(pIServerUserItem)
			{
				//调试用户
				if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
				{
					OnDebugCard(pIServerUserItem);
				}
			}

			if (m_bOffLineTrustee && wChairID == m_wCurrentUser)
			{
				pIServerUserItem->SetTrusteeUser(false);
				IsOfflineTrustee();
			}
			else if (m_bOffLineTrustee && wChairID != m_wCurrentUser)
			{
				pIServerUserItem->SetTrusteeUser(false);
			}

			return bSendResult;
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//单元积分
			StatusPlay.lCellScore=m_pITableFrame->GetCellScore();

			//游戏变量
			StatusPlay.cbBombCount=m_cbBombCount;
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.cbBankerScore=m_cbBankerScore;

			//自定规则
			StatusPlay.cbTimeOutCard=m_pGameCustomRule->cbTimeOutCard;		
			StatusPlay.cbTimeCallScore=m_pGameCustomRule->cbTimeCallScore;
			StatusPlay.cbTimeStartGame=m_pGameCustomRule->cbTimeStartGame;
			StatusPlay.cbTimeHeadOutCard=m_pGameCustomRule->cbTimeHeadOutCard;

			//出牌信息
			StatusPlay.wTurnWiner=m_wTurnWiner;
			StatusPlay.cbTurnCardCount=m_cbTurnCardCount;
			CopyMemory(StatusPlay.cbTurnCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

			//扑克信息
			CopyMemory(StatusPlay.cbBankerCard,m_cbBankerCard,sizeof(m_cbBankerCard));
			CopyMemory(StatusPlay.cbHandCardCount,m_cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(StatusPlay.cbHandCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*m_cbHandCardCount[wChairID]);
			CopyMemory(StatusPlay.cbUserTrustee,m_cbUserTrustee,sizeof(m_cbUserTrustee));
			StatusPlay.lScoreTimes = m_lScoreTimes;

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(StatusPlay.remainCardCount), &remaincard, sizeof(remaincard));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusPlay.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//0 积分场	1 金币场
			StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();

			CTime ctCurTime = CTime::GetCurrentTime();
			CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;

			//判断是否首出
			BYTE cbOutCardTime = (wChairID == m_wBankerUser && m_cbOutCardCount[wChairID] == 0) ? StatusPlay.cbTimeHeadOutCard : StatusPlay.cbTimeOutCard;
			StatusPlay.cbTimeRemain = cbOutCardTime - TimeSpan.GetSeconds();

			//发送场景
			bool bSendResult = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			if(pIServerUserItem)
			{
				//调试用户
				if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
				{
					OnDebugCard(pIServerUserItem);
				}
			}

			if (m_bOffLineTrustee && wChairID == m_wCurrentUser)
			{
				pIServerUserItem->SetTrusteeUser(false);
				IsOfflineTrustee();
			}
			else if (m_bOffLineTrustee && wChairID != m_wCurrentUser)
			{
				pIServerUserItem->SetTrusteeUser(false);
			}

			return bSendResult;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//时间事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	//if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		if(wTimerID==IDI_USER_OUT_TIME)
		{
			OutputDebugString(TEXT("ptdt *** 断线托管代打 出牌"));
			m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

			if (m_wCurrentUser==INVALID_CHAIR) return true;

			IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
			ASSERT(pServerUserItem!=NULL);
			if (pServerUserItem==NULL) return false;

			bool bTrusteeUser=pServerUserItem->IsTrusteeUser();
			BYTE cbGameStatus = m_pITableFrame->GetGameStatus();

			CString strConclude;
			CTime time = CTime::GetCurrentTime();
			CString strSingleUser;
			strSingleUser.Format(TEXT("用户代打，桌子ID=%d,椅子ID=%d,玩家【%s】,AI标志：%d,托管状态%d,游戏场景%d"), m_pITableFrame->GetTableID(), pServerUserItem->GetChairID(), pServerUserItem->GetNickName(), pServerUserItem->IsAndroidUser(), bTrusteeUser, cbGameStatus);
			strConclude += strSingleUser;
			CString strdebugB;
            strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
            m_pITableFrame->SendGameLogData(strdebugB);

			switch(cbGameStatus)
			{
			case GAME_SCENE_CALL:
				{
					WORD wTempCurrentUser = m_wCurrentUser;
					if (m_cbBankerScore == 0)
						OnUserCallScore(m_wCurrentUser, 0x01);
					else
						OnUserCallScore(m_wCurrentUser, 0xFF);

					//超时自动托管
					OnUserTrustee(wTempCurrentUser, true);
					break;
				}
			case GAME_SCENE_PLAY:
				{
					if(m_cbTurnCardCount == 0)
					{
						tagSearchCardResult SearchCardResult;
						m_GameLogic.SearchOutCard( m_cbHandCardData[m_wCurrentUser],m_cbHandCardCount[m_wCurrentUser],NULL,0,&SearchCardResult);
						if (SearchCardResult.cbSearchCount > 0)
						{
							WORD wTempCurrentUser = m_wCurrentUser;
							OnUserOutCard(m_wCurrentUser, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0]);

							//超时自动托管
							OnUserTrustee(wTempCurrentUser, true);
						}
						else
						{
							ASSERT(FALSE);
						}
					}
					else
					{
						tagSearchCardResult SearchCardResult;
						ZeroMemory(&SearchCardResult, sizeof(SearchCardResult));
						WORD wTempCurrentUser = m_wCurrentUser;

						m_GameLogic.SearchOutCard(m_cbHandCardData[m_wCurrentUser], m_cbHandCardCount[m_wCurrentUser], m_cbTurnCardData, m_cbTurnCardCount, &SearchCardResult);
						if (SearchCardResult.cbSearchCount > 0)
						{
							OnUserOutCard(m_wCurrentUser, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0]);

							//超时自动托管
							OnUserTrustee(wTempCurrentUser, true);
						}
						else
						{
							OnUserPassCard(m_wCurrentUser);

							//超时自动托管
							OnUserTrustee(wTempCurrentUser, true);
						}
					}					
					break;
				}
			}
			return true;
		}
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
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("游戏消息，桌子ID=%d,椅子ID=%d,玩家【%s】,AI标志：%d，游戏指令：%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser(), wSubCmdID);
	else 
		strSingleUser.Format(TEXT("游戏消息，桌子ID=%d,椅子ID=%d，游戏指令：%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(),wSubCmdID);
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(),time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	m_cbWaitTime=0;
	switch (wSubCmdID)
	{
	case SUB_C_CALL_SCORE:	//用户叫分
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CallScore));
			if (wDataSize!=sizeof(CMD_C_CallScore)) return false;

			//状态效验
			//ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_CALL);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_CALL) return true;

			//用户效验
			//ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//变量定义
			CMD_C_CallScore * pCallScore=(CMD_C_CallScore *)pData;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserCallScore(wChairID,pCallScore->cbCallScore);
		}
	case SUB_C_OUT_CARD:	//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//效验数据
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE))));
			if ((wDataSize < wHeadSize) || (wDataSize != (wHeadSize + pOutCard->cbCardCount*sizeof(BYTE))))
			{
				return false;
			}

			//状态效验
			//ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

			//用户效验
			//ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserOutCard(wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
	case SUB_C_PASS_CARD:	//用户放弃
		{
			//状态效验
			//ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

			//用户效验
			//ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			return OnUserPassCard(pIServerUserItem->GetChairID());
		}
	case SUB_C_TRUSTEE:
		{
			//用户效验
// 			tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
// 			if (pUserData == NULL || pUserData->cbUserStatus != US_PLAYING)
// 			{
// 				return true;
// 			}
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_TRUSTEE));
			if (wDataSize!=sizeof(CMD_C_TRUSTEE)) return false;

			CMD_C_TRUSTEE* pCTrustee = (CMD_C_TRUSTEE*)pData;
			pIServerUserItem->SetTrusteeUser(pCTrustee->bTrustee == TRUE ? true : false);

			//托管后立即操作
			if (TRUE == pCTrustee->bTrustee && m_wCurrentUser != INVALID_CHAIR && pIServerUserItem->GetChairID() == m_wCurrentUser)
			{
				BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
				switch (cbGameStatus)
				{
					case GAME_SCENE_CALL:
					{
						if (m_cbBankerScore == 0)
							OnUserCallScore(m_wCurrentUser, 0x01);
						else
							OnUserCallScore(m_wCurrentUser, 0xFF);
						break;
					}
				case GAME_SCENE_PLAY:
					{
						if (m_cbTurnCardCount == 0)
						{
							tagSearchCardResult SearchCardResult;
							m_GameLogic.SearchOutCard(m_cbHandCardData[m_wCurrentUser], m_cbHandCardCount[m_wCurrentUser], NULL, 0, &SearchCardResult);
							if (SearchCardResult.cbSearchCount > 0)
							{
								OnUserOutCard(m_wCurrentUser, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0]);
							}
							else
							{
								ASSERT(FALSE);
							}
						}
						else
						{
							tagSearchCardResult SearchCardResult;
							ZeroMemory(&SearchCardResult, sizeof(SearchCardResult));

							m_GameLogic.SearchOutCard(m_cbHandCardData[m_wCurrentUser], m_cbHandCardCount[m_wCurrentUser], m_cbTurnCardData, m_cbTurnCardCount, &SearchCardResult);
							if (SearchCardResult.cbSearchCount > 0)
							{
								OnUserOutCard(m_wCurrentUser, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0]);
							}
							else
							{
								OnUserPassCard(m_wCurrentUser);
							}
						}
						break;
					}
				default:
					break;
				}
			}
			else if (FALSE == pCTrustee->bTrustee && m_wCurrentUser != INVALID_CHAIR && pIServerUserItem->GetChairID() == m_wCurrentUser)
			{
				//删除超时定时器
				m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

				BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
				switch (cbGameStatus)
				{
					case GAME_SCENE_CALL:
					{
						CTime ctCurTime = CTime::GetCurrentTime();
						CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;
						BYTE cbTimeRemain = m_pGameCustomRule->cbTimeCallScore - TimeSpan.GetSeconds();
						m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME, cbTimeRemain * 1000, -1, NULL);

						break;
					}
					case GAME_SCENE_PLAY:
					{
						CTime ctCurTime = CTime::GetCurrentTime();
						CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;

						//判断是否首出
						BYTE cbOutCardTime = (pIServerUserItem->GetChairID() == m_wBankerUser && m_cbOutCardCount[pIServerUserItem->GetChairID()] == 0) ? m_pGameCustomRule->cbTimeHeadOutCard : m_pGameCustomRule->cbTimeOutCard;
						BYTE cbTimeRemain = cbOutCardTime - TimeSpan.GetSeconds();
						m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME, cbTimeRemain * 1000, -1, NULL);

						break;
					}
					default:
						break;
				}
			}

			CMD_S_TRUSTEE pSTrustee;
			pSTrustee.wTrusteeUser = pIServerUserItem->GetChairID();
			pSTrustee.bTrustee = pCTrustee->bTrustee;

			m_cbUserTrustee[pIServerUserItem->GetChairID()] = pCTrustee->bTrustee;

			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&pSTrustee,sizeof(CMD_S_TRUSTEE));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&pSTrustee,sizeof(CMD_S_TRUSTEE));
			return true;
		}
	case SUB_C_REQUEST_RCRecord:
		{
			ASSERT (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) !=0)/* && (m_pITableFrame->IsPersonalRoomDisumme())*/);
			if (!(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) !=0)/* && (m_pITableFrame->IsPersonalRoomDisumme())*/))
			{
				return false;
			}

			ASSERT (pIServerUserItem->IsMobileUser());
			if (!pIServerUserItem->IsMobileUser())
			{
				return false;
			}

			CMD_S_RoomCardRecord RoomCardRecord;
			ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

			//积分房卡
			if ((m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0))
			{
				CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
			}
			else
			{
				WORD wChairID = pIServerUserItem->GetChairID();
				POSITION pos = m_listWinScoreRecord[wChairID].GetHeadPosition();
				CString strUSER;
                strUSER.Format(TEXT("table id is %d, 请求玩家昵称 %s chairID %d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetNickName(), wChairID);
				CString strSingle;
				while (pos)
				{
					if (RoomCardRecord.nCount >= MAX_RECORD_COUNT)
					{
						break;
					}

					RoomCardRecord.lDetailScore[wChairID][RoomCardRecord.nCount++] = m_listWinScoreRecord[wChairID].GetNext(pos);
					strSingle.Format(TEXT("局数%d,得分 %I64d\n"), RoomCardRecord.nCount, RoomCardRecord.lDetailScore[wChairID][RoomCardRecord.nCount - 1]);
					strUSER += strSingle;
				}
                m_pITableFrame->SendGameLogData(strUSER);
			}
			
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));

			return true;
		}
	}

	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (m_pServerDebug != NULL)
	{
		// 消息处理
		if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
		{
			// 判断消息
			return m_pServerDebug->ServerDebug(wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN, pData, wDataSize, pIServerUserItem, m_pITableFrame);
		}
	}

	return false;
}

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("用户断线，桌子ID=%d,椅子ID=%d,玩家【%s】,AI标志：%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	else
		strSingleUser.Format(TEXT("用户断线，桌子ID=%d,椅子ID=%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID());
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(),time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	if(m_bOffLineTrustee)
	{
		//启动离线托管
		pIServerUserItem->SetTrusteeUser(true);

 		//if (wChairID == m_wCurrentUser)
 		//{
 		//	IsOfflineTrustee(1);
 		//}
	}
	return true;
}

//用户重入
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("用户重入，桌子ID=%d,椅子ID=%d,玩家【%s】,AI标志：%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	else
		strSingleUser.Format(TEXT("用户重入，桌子ID=%d,椅子ID=%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID());
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(),time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	OutputDebugString(TEXT("ptdt *** 用户重入"));
	if(m_bOffLineTrustee)
	{
		//if (((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)&&(pIServerUserItem->GetChairID()==m_wCurrentUser))
		{
			//OutputDebugString(TEXT("ptdt *** 用户重入 杀掉自动出牌定时器"));
			//m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);
		}
	}
	return true; 
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("用户坐下，桌子ID=%d,椅子ID=%d,玩家【%s】,AI标志：%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	else
		strSingleUser.Format(TEXT("用户坐下，桌子ID=%d,椅子ID=%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID());
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID() ,time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

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
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("用户起立，桌子ID=%d,椅子ID=%d,玩家【%s】,AI标志：%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	else
		strSingleUser.Format(TEXT("用户起立，桌子ID=%d,椅子ID=%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID());
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserLeave(wChairID);
	}
	m_cbUserTrustee[wChairID] = 0;

	//金币房卡
	if ((m_pITableFrame->GetDataBaseMode() == 1) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
	{
		m_listWinScoreRecord[wChairID].RemoveAll();
	}
	//积分房卡
	else if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_pITableFrame->IsPersonalRoomDisumme())
	{
		//约战解散
		ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
		ZeroMemory(m_userachievement, sizeof(m_userachievement));
	}

	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbTurnCardCount!=0));
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return true;

	//设置变量
	m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
	if (m_wCurrentUser == m_wTurnWiner)
	{
		m_cbTurnCardCount = 0;
		ZeroMemory(m_cbTurnCardData, sizeof(m_cbTurnCardData));
	}

	m_ctChangeTime = CTime::GetCurrentTime();

	//构造消息
	CMD_S_PassCard PassCard;
	PassCard.wPassCardUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.cbTurnOver=(m_cbTurnCardCount==0)?TRUE:FALSE;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	if (m_pGameVideo)
	{		
		m_pGameVideo->AddVideoData(SUB_S_PASS_CARD,&PassCard);
	}

	////////////////////////////////////////////////////////////
	IServerUserItem *pIOutCardServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	CString strdebug;
	CTime time = CTime::GetCurrentTime();

	if (pIOutCardServerUserItem)
	{
        strdebug.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d 出牌昵称【%s】GAMEID = %d, chairID = %d , 弃牌 剩余的牌张数 [%d] android = %d\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIOutCardServerUserItem->GetNickName(), pIOutCardServerUserItem->GetGameID(), wChairID, m_cbHandCardCount[wChairID], pIOutCardServerUserItem->IsAndroidUser());
        m_pITableFrame->SendGameLogData(strdebug);
	}
	////////////////////////////////////////////////////////////

	//if(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
		IsOfflineTrustee();

	return true;
}

//用户叫分
bool CTableFrameSink::OnUserCallScore(WORD wChairID, BYTE cbCallScore)
{
	//效验状态
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return true;

	//效验参数
	//ASSERT(((cbCallScore>=1)&&(cbCallScore<=3)&&(cbCallScore>m_cbBankerScore))||(cbCallScore==255));
	if (((cbCallScore<1)||(cbCallScore>3)||(cbCallScore<=m_cbBankerScore))&&(cbCallScore!=255))
		cbCallScore = 255;

	//设置状态
	if (cbCallScore!=0xFF)
	{
		m_cbBankerScore=cbCallScore;
		m_wBankerUser=m_wCurrentUser;
	}

	//设置叫分
	m_cbScoreInfo[wChairID]=cbCallScore;

	//设置用户
	if ((m_cbBankerScore==3)||(m_wFirstUser==(wChairID+1)%GAME_PLAYER))
	{
		m_wCurrentUser=INVALID_CHAIR;
	}
	else
	{
		m_wCurrentUser=(wChairID+1)%GAME_PLAYER;
	}

	m_ctChangeTime = CTime::GetCurrentTime();

	//构造变量
	CMD_S_CallScore CallScore;
	CallScore.wCallScoreUser=wChairID;
	CallScore.wCurrentUser=m_wCurrentUser;
	CallScore.cbUserCallScore=cbCallScore;
	CallScore.cbCurrentScore=m_cbBankerScore;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CALL_SCORE,&CallScore,sizeof(CallScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_SCORE,&CallScore,sizeof(CallScore));

	if (m_pGameVideo)
	{		
		m_pGameVideo->AddVideoData(SUB_S_CALL_SCORE,&CallScore);
	}

	//开始判断
	if ((m_cbBankerScore==3)||(m_wFirstUser==(wChairID+1)%GAME_PLAYER))
	{
		//无人叫分
		if ( m_cbBankerScore == 0 )
		{
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				if(m_cbCallScoreCount<2)
				{
					m_cbCallScoreCount++;
					return OnEventGameStart();
				}
			}
			m_wBankerUser=m_wFirstUser;
			m_cbBankerScore=1;
		}

		//设置状态
		m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

		//设置变量
		if (m_cbBankerScore==0) m_cbBankerScore=1;
		if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wFirstUser;
		m_cbCallScoreCount=0;

		//发送底牌
		m_cbHandCardCount[m_wBankerUser]+=CountArray(m_cbBankerCard);
		CopyMemory(&m_cbHandCardData[m_wBankerUser][NORMAL_COUNT],m_cbBankerCard,sizeof(m_cbBankerCard));

		//排列扑克
		m_GameLogic.SortCardList(m_cbHandCardData[m_wBankerUser],m_cbHandCardCount[m_wBankerUser],ST_ORDER);


		//设置用户
		m_wTurnWiner=m_wBankerUser;
		m_wCurrentUser=m_wBankerUser;

		//发送消息
		CMD_S_BankerInfo BankerInfo;
		BankerInfo.wBankerUser=m_wBankerUser;
		BankerInfo.wCurrentUser=m_wCurrentUser;
		BankerInfo.cbBankerScore=m_cbBankerScore;
		CopyMemory(BankerInfo.cbBankerCard,m_cbBankerCard,sizeof(m_cbBankerCard));
		
		//游戏倍数
		m_lScoreTimes = m_cbBankerScore;
		BankerInfo.lScoreTimes = m_lScoreTimes;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BANKER_INFO,&BankerInfo,sizeof(BankerInfo));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BANKER_INFO,&BankerInfo,sizeof(BankerInfo));

		if (m_pGameVideo)
		{		
			m_pGameVideo->AddVideoData(SUB_S_BANKER_INFO,&BankerInfo);
		}

		//启动离线托管
		IsOfflineTrustee();

		return true;

	}

	//启动离线托管
	IsOfflineTrustee();

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//效验状态
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return true;

	ASSERT(cbCardCount <= m_cbHandCardCount[wChairID] && cbCardCount > 0);
	if (cbCardCount > m_cbHandCardCount[wChairID] || cbCardCount < 1)
	{
		return false;
	}

	//判断牌有效
	for (WORD i = 0; i < cbCardCount; i++)
	{
		if (!m_GameLogic.IsValidCard(cbCardData[i]))
		{
			return false;
		}
	}

	//删除扑克
	//先删除避免前端发过来10张1等错误数据导致先调用GetCardType函数崩溃
	if (m_GameLogic.RemoveCardList(cbCardData, cbCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]) == false)
	{
		ASSERT(FALSE);

		CString strActualCardData;
		for (WORD i = 0; i < cbCardCount; i++)
		{
			CString str;
			str.Format(TEXT("[%d]"), cbCardData[i]);
			strActualCardData += str;
		}

		CString strHandCardData;
		for (WORD i = 0; i < m_cbHandCardCount[wChairID]; i++)
		{
			CString str;
			str.Format(TEXT("[%d]"), m_cbHandCardData[wChairID][i]);
			strHandCardData += str;
		}

		CString strTrace;
		strTrace.Format(TEXT("GAMEID = %d, RemoveCardInvalid， android = %d"), m_pITableFrame->GetTableUserItem(wChairID)->GetGameID(), m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser());

		CTraceService::TraceString(strTrace, TraceLevel_Exception);
		CTraceService::TraceString(strActualCardData, TraceLevel_Exception);
		CTraceService::TraceString(strHandCardData, TraceLevel_Exception);

		return false;
	}

	//获取类型
	BYTE cbTempCardData[MAX_COUNT];
	ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
	CopyMemory(cbTempCardData, cbCardData, sizeof(BYTE)* cbCardCount);

	//先排序（单连顺子必须从大到小才算）
	m_GameLogic.SortCardList(cbTempCardData, cbCardCount, ST_ORDER);
	BYTE cbCardType = m_GameLogic.GetCardType(cbTempCardData, cbCardCount);

	//类型判断
	if (cbCardType==CT_ERROR) 
	{
		ASSERT(FALSE);

		CString strCardData;
		for (WORD i = 0; i < cbCardCount; i++)
		{
			CString str;
			str.Format(TEXT("[%d]"), cbCardData[i]);
			strCardData += str;
		}

		CString strTrace;
		strTrace.Format(TEXT("GAMEID = %d, CardType == CT_ERROR， android = %d"), m_pITableFrame->GetTableUserItem(wChairID)->GetGameID(), m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser());

		CTraceService::TraceString(strTrace, TraceLevel_Exception);
		CTraceService::TraceString(strCardData, TraceLevel_Exception);

		return false;
	}

	//出牌判断
	if (m_cbTurnCardCount!=0)
	{
		//对比扑克
		if (m_GameLogic.CompareCard(m_cbTurnCardData,cbCardData,m_cbTurnCardCount,cbCardCount)==false)
		{
			ASSERT(FALSE);

			CString strTurnCardData;
			for (WORD i = 0; i < m_cbTurnCardCount; i++)
			{
				CString str;
				str.Format(TEXT("[%d]"), m_cbTurnCardData[i]);
				strTurnCardData += str;
			}

			CString strCardData;
			for (WORD i = 0; i < cbCardCount; i++)
			{
				CString str;
				str.Format(TEXT("[%d]"), cbCardData[i]);
				strCardData += str;
			}

			CString strTrace;
			strTrace.Format(TEXT("GAMEID = %d, CompareInvalid， android = %d"), m_pITableFrame->GetTableUserItem(wChairID)->GetGameID(), m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser());

			CTraceService::TraceString(strTrace, TraceLevel_Exception);
			CTraceService::TraceString(strTurnCardData, TraceLevel_Exception);
			CTraceService::TraceString(strCardData, TraceLevel_Exception);

			return false;
		}
	}

	//出牌变量
	m_cbOutCardCount[wChairID]++;

	//设置变量
	m_cbTurnCardCount=cbCardCount;
	m_cbHandCardCount[wChairID]-=cbCardCount;
	CopyMemory(m_cbTurnCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//炸弹判断
	if ((cbCardType==CT_BOMB_CARD)||(cbCardType==CT_MISSILE_CARD)) 
	{
		m_cbBombCount++;
		m_cbEachBombCount[wChairID]++;

		m_lScoreTimes *= 2;
	}
	
	//切换用户
	m_wTurnWiner=wChairID;
	if (m_cbHandCardCount[wChairID]!=0)
	{
		if (cbCardType!=CT_MISSILE_CARD)
		{
			m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
		}
	}
	else m_wCurrentUser=INVALID_CHAIR;

	m_ctChangeTime = CTime::GetCurrentTime();

	for (WORD i = 0; i<cbCardCount; i++)
	{
		POSITION ptkey = m_llistRemainCard.Find(cbCardData[i]);
		if (ptkey)
		{
			m_llistRemainCard.RemoveAt(ptkey);
		}
	}

	//积分房卡个人成就
	if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	  && m_userachievement[wChairID].dwGameID == m_pITableFrame->GetTableUserItem(wChairID)->GetGameID())
	{
		if (cbCardType == CT_BOMB_CARD)
		{
			m_userachievement[wChairID].cbBombCount++;
		}
		else if (cbCardType == CT_MISSILE_CARD)
		{
			m_userachievement[wChairID].cbMissileCount++;
		}
		else if ((cbCardType == CT_THREE_LINE)
			|| (cbCardType == CT_THREE_TAKE_ONE && cbCardCount > 4)
			|| (cbCardType == CT_THREE_TAKE_TWO && cbCardCount > 5))
		{
			m_userachievement[wChairID].cbPlaneCount++;
		}
	}

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbCardCount=cbCardCount;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.cbCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));
	OutCard.lScoreTimes = m_lScoreTimes;

	REMAINCARDCOUNT remaincard;
	ZeroMemory(&remaincard, sizeof(remaincard));
	GetRemainCard(remaincard);

	CopyMemory(&(OutCard.remainCardCount), &remaincard, sizeof(remaincard));

	//发送数据
	WORD wHeadSize=sizeof(OutCard)-sizeof(OutCard.cbCardData);
	WORD wSendSize=wHeadSize+OutCard.cbCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_OUT_CARD,&OutCard);
	}

	////////////////////////////////////////////////////////////
	CString strCardData;
	for (WORD i = 0; i < cbCardCount; i++)
	{
		CString str;
		str.Format(TEXT("[%d]"), cbCardData[i]);
		strCardData += str;
	}

	IServerUserItem *pIOutCardServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (pIOutCardServerUserItem)
	{
		CString strdebug;
		CTime time = CTime::GetCurrentTime();
        strdebug.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d 出牌昵称【%s】GAMEID = %d, chairID = %d, %s 剩余的牌张数 [%d] android = %d\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIOutCardServerUserItem->GetNickName(), pIOutCardServerUserItem->GetGameID(), wChairID, strCardData, m_cbHandCardCount[wChairID], pIOutCardServerUserItem->IsAndroidUser());
        m_pITableFrame->SendGameLogData(strdebug);
	}
	////////////////////////////////////////////////////////////

	//出牌最大
	if (cbCardType==CT_MISSILE_CARD) m_cbTurnCardCount=0;

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameConclude(wChairID,NULL,GER_NORMAL);
	else
	{
		OutputDebugString(TEXT("ptdt *** 用户出牌"));
		//启动离线托管
		IsOfflineTrustee();			
	}

	return true;
}

//托管事件
bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	//状态效验
	ASSERT(wChairID < GAME_PLAYER);
	if (wChairID >= GAME_PLAYER)
	{
		return true;
	}
	CMD_S_TRUSTEE pSTrustee;
	pSTrustee.wTrusteeUser = wChairID;
	pSTrustee.bTrustee = bTrustee;

	//获取用户
	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (pIServerUserItem != NULL) pIServerUserItem->SetTrusteeUser(bTrustee);

	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TRUSTEE, &pSTrustee, sizeof(CMD_S_TRUSTEE));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TRUSTEE, &pSTrustee, sizeof(CMD_S_TRUSTEE));
	return true;
}

//调试用户
bool CTableFrameSink::OnDebugCard(IServerUserItem * pIServerUserItem)
{
	if (m_pServerDebug != NULL)
	{
		CMD_S_DebugCard DebugCard ;
		ZeroMemory(&DebugCard, sizeof(DebugCard));

		for(WORD i = 0; i < GAME_PLAYER; i++)
		{
			DebugCard.wCardUser[DebugCard.cbUserCount] = i;
			DebugCard.cbCardCount[DebugCard.cbUserCount] = m_cbHandCardCount[i];
			CopyMemory(DebugCard.cbCardData[DebugCard.cbUserCount++],m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
		}
		//发送调试;
		m_pServerDebug->ServerDebug(&DebugCard, m_pITableFrame, pIServerUserItem);
		
	}

	return true;
}

//发送调试信息
void CTableFrameSink::SendDebugCard()
{
	if (m_pServerDebug != NULL)
	{
		CMD_S_DebugCard DebugCard ;
		ZeroMemory(&DebugCard, sizeof(DebugCard));

		for(WORD i = 0; i < GAME_PLAYER; i++)
		{
			DebugCard.wCardUser[DebugCard.cbUserCount] = i;
			DebugCard.cbCardCount[DebugCard.cbUserCount] = m_cbHandCardCount[i];
			CopyMemory(DebugCard.cbCardData[DebugCard.cbUserCount++],m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
		}
		m_pServerDebug->ServerDebug(&DebugCard, m_pITableFrame);
	}

	return;
}
//设置基数
void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

//是否托管
bool CTableFrameSink::IsOfflineTrustee(BYTE cbTrusteeTime)
{
	//非比赛模式
	//if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;
	//非常规座位
	if(m_wCurrentUser==INVALID_CHAIR) return false;

	//金币场和金币房卡可以断线代打，积分房卡不断线代打
	//允许代打
	if (m_bOffLineTrustee && (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
	|| (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0))
	{
		OutputDebugString(TEXT("ptdt *** 断线托管代打"));
		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
		ASSERT(pServerUserItem!=NULL);
		if (pServerUserItem==NULL) return false;

		//删除超时定时器
		m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

		//变量定义
		bool bTrusteeUser=pServerUserItem->IsTrusteeUser();
		DWORD dwTimerTime=3;
	
		BYTE cbGameStatus=m_pITableFrame->GetGameStatus();
		if (cbGameStatus==GAME_SCENE_CALL) 
		{
			dwTimerTime=m_pGameCustomRule->cbTimeCallScore;
		}
		if (cbGameStatus==GAME_SCENE_PLAY)
		{
			if (m_cbTurnCardCount==0)
				dwTimerTime=m_pGameCustomRule->cbTimeHeadOutCard;
			else
				dwTimerTime=m_pGameCustomRule->cbTimeOutCard;
		}

		dwTimerTime += 1;
		
		BYTE cbDelayTrusteeTime = ((cbTrusteeTime != INVALID_BYTE) ? cbTrusteeTime : (bTrusteeUser ? m_cbTrusteeDelayTime : dwTimerTime));

		m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME, cbDelayTrusteeTime * 1000, -1, NULL);
		return true;
	}

	return false;
}

//写日志文件
void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{	
	//设置语言区域
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("LandServerDEBUG%d.txt"), m_pITableFrame->GetTableID());
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

void CTableFrameSink::InitializeListRemainCard()
{
	m_llistRemainCard.RemoveAll();
	BYTE cbRandCard[FULL_COUNT];
	ZeroMemory(cbRandCard, sizeof(cbRandCard));
	m_GameLogic.RandCardList(cbRandCard, CountArray(cbRandCard));
	for (WORD i = 0; i<FULL_COUNT; i++)
	{
		m_llistRemainCard.AddTail(cbRandCard[i]);
	}
}

//获取剩余张数
void CTableFrameSink::GetRemainCard(REMAINCARDCOUNT &remaincard)
{
	ZeroMemory(&remaincard, sizeof(remaincard));

	POSITION ptHead = m_llistRemainCard.GetHeadPosition();
	while (ptHead)
	{
		BYTE &cbCardData = m_llistRemainCard.GetNext(ptHead);
		BYTE cbLogicValue = m_GameLogic.GetCardLogicValue(cbCardData);

		switch (cbLogicValue)
		{
		case 17://大王
		{
					remaincard.cbRemainCount[0]++;
					break;
		}
		case 16://小王
		{
					remaincard.cbRemainCount[1]++;
					break;
		}
		case 15://2
		{
					remaincard.cbRemainCount[2]++;
					break;
		}
		case 14://A
		{
					remaincard.cbRemainCount[3]++;
					break;
		}

		case 13://K
		{
					remaincard.cbRemainCount[4]++;
					break;
		}
		case 12://Q
		{
					remaincard.cbRemainCount[5]++;
					break;
		}
		case 11://J
		{
					remaincard.cbRemainCount[6]++;
					break;
		}
		case 10://10
		{
					remaincard.cbRemainCount[7]++;
					break;
		}

		case 9://9
		{
				   remaincard.cbRemainCount[8]++;
				   break;
		}
		case 8://8
		{
				   remaincard.cbRemainCount[9]++;
				   break;
		}
		case 7://7
		{
				   remaincard.cbRemainCount[10]++;
				   break;
		}
		case 6://6
		{
				   remaincard.cbRemainCount[11]++;
				   break;
		}

		case 5://5
		{
				   remaincard.cbRemainCount[12]++;
				   break;
		}
		case 4://4
		{
				   remaincard.cbRemainCount[13]++;
				   break;
		}
		case 3://3
		{
				   remaincard.cbRemainCount[14]++;
				   break;
		}
		default:
			ASSERT(false);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
