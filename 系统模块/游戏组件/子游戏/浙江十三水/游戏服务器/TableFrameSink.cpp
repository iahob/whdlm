#include "StdAfx.h"
#include "TableFrameSink.h"


//房间玩家信息
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo;	//玩家USERID映射玩家信息
//房间用户调试
CList<ROOMUSERDEBUG, ROOMUSERDEBUG&> g_ListRoomUserDebug;		//房间用户调试链表
//操作调试记录
CList<CString, CString&> g_ListOperationRecord;						//操作调试记录

ROOMUSERINFO	g_CurrentQueryUserInfo;								//当前查询用户信息

//全局变量
LONGLONG						g_lRoomStorageStart = 0LL;								//房间起始库存
LONGLONG						g_lRoomStorageCurrent = 0LL;							//总输赢分
LONGLONG						g_lStorageDeductRoom = 0LL;								//回扣变量
LONGLONG						g_lStorageMax1Room = 0LL;								//库存封顶
LONGLONG						g_lStorageMul1Room = 0LL;								//系统输钱比例
LONGLONG						g_lStorageMax2Room = 0LL;								//库存封顶
LONGLONG						g_lStorageMul2Room = 0LL;								//系统输钱比例


#define IDI_SHOW_CARD				1					//理牌事件
#define IDI_WAIT_COMAPRE			2					//等待比牌结束
///////////////////////////////////////////////////////////////////////////

//视频组件
#define VIDEO_SERVICE_DLL_NAME	TEXT("ThirteenZJGameVideo.dll")	//组件名字
//////////////////////////////////////////////////////////////////////////
//构造函数
CTableFrameSink::CTableFrameSink()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pGameCustomRule=NULL;

	//游戏变量
	ZeroMemory(m_lUserChip, sizeof(m_lUserChip));
	ZeroMemory(m_lScoreTimes, sizeof(m_lScoreTimes));
	ZeroMemory(m_lscoreTimesPlus, sizeof(m_lscoreTimesPlus));
	ZeroMemory(m_lScoreTimesEx, sizeof(m_lScoreTimesEx));
	ZeroMemory(m_lScoreTimesExEx, sizeof(m_lScoreTimesExEx));
	ZeroMemory(m_bSpecialType, sizeof(m_bSpecialType));
	ZeroMemory(m_bUserCardData , sizeof(m_bUserCardData)) ;
	ZeroMemory(m_bSegmentCard , sizeof(m_bSegmentCard)) ;
	ZeroMemory(m_cbShowSpecialType,sizeof(m_cbShowSpecialType));
	ZeroMemory(m_MobilePutCard, sizeof(m_MobilePutCard));
	m_cbTimeStartGame = 20;					//开始时间
	m_cbTimeRangeCard = 20;					//理牌时间
	
	//用户变量
	m_wBanker = INVALID_CHAIR ;
	m_cbPlayerCount=GAME_PLAYER;
	//计数变量
	m_wShowCardCount = 0; 
	m_wCompleteCompareCount = 0;
	//状态变量
	ZeroMemory(m_bGameStatus, sizeof(m_bGameStatus));
	ZeroMemory(m_bFinishSegment , sizeof(m_bFinishSegment)) ;
	ZeroMemory(m_bAllWin,sizeof(m_bAllWin));
	ZeroMemory(m_bCompleteCompare, sizeof(m_bCompleteCompare));

	memset(m_wUserEnter,INVALID_CHAIR,sizeof(m_wUserEnter));
	m_bHaveBanker=true;

	m_cbMaCard=0;
	m_cbTongHuaMode = 0;
	m_cbQuickMode = 0;
	m_wMaCardUser=INVALID_CHAIR;
	m_bTest = false;
	m_bStartCompare = false;
	m_wAllKillUser = INVALID_CHAIR;
	ZeroMemory(m_lTestScore, sizeof(m_lTestScore));
	//服务调试
	m_hDebugInst = NULL;
	m_pServerDebug = NULL;
	m_hDebugInst = LoadLibrary(TEXT("ThirteenZJServerDebug.dll"));
	if (m_hDebugInst)
	{
		typedef void * (*CREATE)();
		CREATE ServerDebug = (CREATE)GetProcAddress(m_hDebugInst, "CreateServerDebug");
		if (ServerDebug)
		{
			m_pServerDebug = static_cast<IServerDebug*>(ServerDebug());
		}
	}

	//游戏视频
	m_hVideoInst = NULL;
	m_pGameVideo = NULL;


	ZeroMemory(&m_stRecord, sizeof(m_stRecord));
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
	if (m_pGameVideo)
	{
		delete m_pGameVideo;
		m_pGameVideo = NULL;
	}

	if (m_hVideoInst)
	{
		FreeLibrary(m_hVideoInst);
		m_hVideoInst = NULL;
	}

	if (m_pServerDebug)
	{
		delete m_pServerDebug;
		m_pServerDebug = NULL;
	}

	if (m_hDebugInst)
	{
		FreeLibrary(m_hDebugInst);
		m_hDebugInst = NULL;
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

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//游戏配置
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;

	//读取配置
	ReadConfigInformation();

	ZeroMemory(&m_stRecord, sizeof(m_stRecord));

	if (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)
	{
		m_hVideoInst = LoadLibrary(VIDEO_SERVICE_DLL_NAME);
		if (m_hVideoInst)
		{
			typedef void * (*CREATE)();
			CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
			if (GameVideo)
			{
				m_pGameVideo = static_cast<IGameVideo*>(GameVideo());
			}
		}
	}
	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//游戏变量
	ZeroMemory(m_lUserChip, sizeof(m_lUserChip));
	ZeroMemory(m_lScoreTimes, sizeof(m_lScoreTimes));
	ZeroMemory(m_lscoreTimesPlus, sizeof(m_lscoreTimesPlus));
	ZeroMemory(m_lScoreTimesEx, sizeof(m_lScoreTimesEx));
	ZeroMemory(m_lScoreTimesExEx, sizeof(m_lScoreTimesExEx));
	ZeroMemory(m_bSpecialType, sizeof(m_bSpecialType));
	ZeroMemory(m_bUserCardData, sizeof(m_bUserCardData));
	ZeroMemory(m_bSegmentCard, sizeof(m_bSegmentCard));
	ZeroMemory(m_bFinishSegment, sizeof(m_bFinishSegment));
	ZeroMemory(m_bGameStatus, sizeof(m_bGameStatus));
	ZeroMemory(m_cbShowSpecialType, sizeof(m_cbShowSpecialType));
	ZeroMemory(m_bCompleteCompare, sizeof(m_bCompleteCompare));
	memset(m_wUserEnter, INVALID_CHAIR, sizeof(m_wUserEnter));
	ZeroMemory(m_MobilePutCard, sizeof(m_MobilePutCard));
	m_wCompleteCompareCount = 0;
	m_wShowCardCount = 0;
	m_bTest = false;
	m_bStartCompare = false;
	m_wAllKillUser = INVALID_CHAIR;
	return;
}

//消费能力
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	//效验状态
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetTableID()==m_pITableFrame->GetTableID()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetTableID()!=m_pITableFrame->GetTableID())) return 0;

	//用户判断
	switch (pIServerUserItem->GetUserStatus())
	{
	case US_SIT:		//坐下状态
	case US_READY:		//准备状态
		{
			return pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore;
		}
	case US_PLAYING:	//游戏状态
	case US_OFFLINE:	//断线状态
		{
			return pIServerUserItem->GetUserScore()-m_pGameServiceOption->lCellScore;
		}
	case US_LOOKON:		//旁观状态
		{
			return pIServerUserItem->GetUserScore();
		}
	}

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
	WriteDebugInfo(_T("游戏开始"));
	//库存
	if (g_lRoomStorageCurrent > 0 )
	{
		bool bUpdate = false;
		for (int i = 0; i < m_cbPlayerCount; ++i)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			if (!pIServerUserItem->IsAndroidUser())
			{
				bUpdate=true;
				break;
			}
		}
		if (bUpdate)
		{
			g_lRoomStorageCurrent = g_lRoomStorageCurrent - g_lRoomStorageCurrent*g_lStorageDeductRoom / 1000;
			TCHAR strInfo[MAX_PATH] = _T("");
			_stprintf_s(strInfo, _T("当前库存：%I64d"), g_lRoomStorageCurrent);
			WriteDebugInfo(strInfo);
		}
	}

	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_PLAYING);
	//时间
	m_cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
	m_cbTimeRangeCard = m_pGameCustomRule->cbTimeRangeCard;

	BYTE bCardData[52];
	m_GameLogic.RandCardList(bCardData, 52);
#ifdef DEBUG
	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	CString strSaveFileName;
	strSaveFileName.Format(TEXT("%s\\ConfigCard\\GameCardData\\ThirteenZJ.card"), szPath);

	CFile file;
	if (file.Open(strSaveFileName, CFile::modeRead))
	{
		file.Read(bCardData, 52);
		file.Close();
	}
#endif
	BYTE cbPlayingCount = 0;
	//设置状态
	for(BYTE i=0;i<m_cbPlayerCount;i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i) ;
		if (pServerUserItem != NULL)
		{
			m_bGameStatus[i] = true;
			cbPlayingCount++;
			//更新房间用户信息
			UpdateRoomUserInfo(pServerUserItem, USER_SITDOWN);
			m_lUserChip[i] = m_pITableFrame->GetCellScore();
			CopyMemory(m_bUserCardData[i], &bCardData[i*HAND_CARD_COUNT], sizeof(BYTE)*HAND_CARD_COUNT);
		}
		else
			m_bGameStatus[i] = false;
	}
	
	//变量定义
	ROOMUSERDEBUG roomuserdebug;
	ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
	POSITION posKeyList;
	//调试
	if (m_pServerDebug != NULL && AnalyseRoomUserDebug(roomuserdebug, posKeyList))
	{
		time_t aa = time(NULL);
		//校验数据
		ASSERT(roomuserdebug.roomUserInfo.wChairID != INVALID_CHAIR && roomuserdebug.userDebug.cbDebugCount != 0 && roomuserdebug.userDebug.debug_type != CONTINUE_CANCEL);
		if (m_pServerDebug->DebugResult(m_bUserCardData, roomuserdebug, m_cbMaCard, m_wBanker))
		{
			//获取元素
			ROOMUSERDEBUG &tmproomuserdebug = g_ListRoomUserDebug.GetAt(posKeyList);

			//校验数据
			ASSERT(roomuserdebug.userDebug.cbDebugCount == tmproomuserdebug.userDebug.cbDebugCount);

			//调试局数
			tmproomuserdebug.userDebug.cbDebugCount--;

			CMD_S_UserDebugComplete UserDebugComplete;
			ZeroMemory(&UserDebugComplete, sizeof(UserDebugComplete));
			UserDebugComplete.dwGameID = roomuserdebug.roomUserInfo.dwGameID;
			CopyMemory(UserDebugComplete.szNickName, roomuserdebug.roomUserInfo.szNickName, sizeof(UserDebugComplete.szNickName));
			UserDebugComplete.debugType = roomuserdebug.userDebug.debug_type;
			UserDebugComplete.cbRemainDebugCount = tmproomuserdebug.userDebug.cbDebugCount;

			m_pITableFrame->SendRoomData(NULL, SUB_S_USER_DEBUG_COMPLETE, &UserDebugComplete, sizeof(UserDebugComplete));
		}
	}
	else
	{
		AnalyseCard();
		m_bTest = false;
	}

	//确定码牌玩家
	if (m_cbMaCard != 0)
	{
		m_wMaCardUser = INVALID_CHAIR;
		//确定码牌玩家
		for (int i = 0; i < m_cbPlayerCount; i++)
		{
			if (m_bUserCardData[i][0]==0) continue;

			for (int j = 0; j < HAND_CARD_COUNT; j++)
			{
				//有码牌的为码牌玩家
				if (m_cbMaCard == m_bUserCardData[i][j])
				{
					m_wMaCardUser = i;
					CString cc;
					cc.Format(_T("马牌=%0x,马牌玩家%d"), m_cbMaCard, m_wMaCardUser);
					WriteDebugInfo(cc);
					break;
				}
			}

			if (m_wMaCardUser != INVALID_CHAIR) break;
		}
	}

	if (m_pGameVideo)
	{
		m_pGameVideo->StartVideo(m_pITableFrame);
	}
	if (m_bHaveBanker == false)
		m_wBanker = INVALID_CHAIR;

	CMD_S_SendCard SendCard ;
	ZeroMemory(&SendCard, sizeof(SendCard)) ;

	//设置庄家
	SendCard.wBanker = m_wBanker;
	SendCard.cbPlayCount = GetPlayerCount();
	CopyMemory(SendCard.bGameStatus,m_bGameStatus,sizeof(m_bGameStatus));

	bool bFirstRecord = true;

	//发送扑克
	for(WORD i = 0; i < m_cbPlayerCount; i++)
	{			
		if (m_bGameStatus[i] == false) continue;
		CopyMemory(SendCard.bCardData, m_bUserCardData[i], sizeof(BYTE)*HAND_CARD_COUNT);
		SendCard.wCurrentUser = i ;
		SendCard.lChipArray[i]=m_lUserChip[i];

		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if ((pIServerUserItem != NULL))
		{
			//发送消息
			m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD_EX, &SendCard, sizeof(SendCard));
			m_pITableFrame->SendLookonData(i, SUB_S_SEND_CARD_EX, &SendCard, sizeof(SendCard));
			if (pIServerUserItem->IsAndroidUser() == false)
			{
				ZeroMemory(&m_MobilePutCard[i], sizeof(m_MobilePutCard[i]));
				m_MobilePutCard[i].cbSpecialType = m_GameLogic.GetSpecialType(m_bUserCardData[i], 13);
				if (m_MobilePutCard[i].cbSpecialType > CT_EX_INVALID)
				{
					m_GameLogic.GetSpecialCardData(m_bUserCardData[i], 13, m_MobilePutCard[i].cbSpecialData);
					if (m_GameLogic.CompareCard(m_MobilePutCard[i].cbSpecialData[1], m_MobilePutCard[i].cbSpecialData[2], 5, 5, true) == enCRLess)
					{
						BYTE cbTemp[5] = { 0 };
						CopyMemory(cbTemp, m_MobilePutCard[i].cbSpecialData[1], 5);
						CopyMemory(m_MobilePutCard[i].cbSpecialData[1], m_MobilePutCard[i].cbSpecialData[2], 5);
						CopyMemory(m_MobilePutCard[i].cbSpecialData[2], cbTemp, 5);
					}
				}
				BYTE cbTempCard[13];
				m_MobilePutCard[i].cbMobilePutCount = m_GameLogic.AutoPutCard(m_bUserCardData[i], cbTempCard, true, m_MobilePutCard[i].cbMobilePutCard);
				for (int j = 0; j < m_MobilePutCard[i].cbMobilePutCount; j++)
				{
					BYTE cbTemp = 0;
					m_MobilePutCard[i].cbMobilePutCardType[j][0] = m_GameLogic.GetCardType(&m_MobilePutCard[i].cbMobilePutCard[j][0], 3, cbTemp);
					m_MobilePutCard[i].cbMobilePutCardType[j][1] = m_GameLogic.GetCardType(&m_MobilePutCard[i].cbMobilePutCard[j][3], 5, cbTemp);
					m_MobilePutCard[i].cbMobilePutCardType[j][2] = m_GameLogic.GetCardType(&m_MobilePutCard[i].cbMobilePutCard[j][8], 5, cbTemp);
				}

				m_pITableFrame->SendTableData(i, SUB_S_MOBILE_PUTCARD, &m_MobilePutCard[i], sizeof(CMD_S_MobilePutCard));
			}

			//游戏视频
			if (m_pGameVideo)
			{
				Video_SendCard video;
				lstrcpyn(video.szNickName, pIServerUserItem->GetNickName(), CountArray(video.szNickName));
				video.wTableID = i;
				video.wCurrentUser = i;
				video.wBanker = m_wBanker;
				CopyMemory(video.bCardData, m_bUserCardData[i], sizeof(BYTE)*HAND_CARD_COUNT);
				video.cbPlayCount = cbPlayingCount;
				CopyMemory(video.bGameStatus, m_bGameStatus, sizeof(m_bGameStatus));
				CopyMemory(video.lChipArray, m_lUserChip, sizeof(m_lUserChip));
				video.lScore = pIServerUserItem->GetUserScore();
				video.lCellScore = m_pITableFrame->GetCellScore();
				video.cbMaCard = m_cbMaCard;
				m_pGameVideo->AddVideoData(SUB_S_SEND_CARD_EX, &video,sizeof(video), bFirstRecord);
				bFirstRecord = false;

			}

		}
	}

	for(WORD i = 0; i < m_cbPlayerCount; i++)
	{			
		if (m_bUserCardData[i][0]==0)continue;
		m_GameLogic.SortCardList(m_bUserCardData[i], HAND_CARD_COUNT) ;
		CString cc;
		cc.Format(_T("玩家%d的牌:"), i);
		for (int k = 0; k < HAND_CARD_COUNT; k++)
			cc.AppendFormat(_T("%0x,"), m_bUserCardData[i][k]);
		WriteDebugInfo(cc);
	}

	//设置时间
	m_dwShowTime = (DWORD)time(NULL);

	m_pITableFrame->SetGameTimer(IDI_SHOW_CARD, (m_cbTimeRangeCard+TIME_DISPATCH_CARD)*1000,1, NULL);
	
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_DISMISS:			//游戏解散
		{
			WriteDebugInfo(_T("游戏解散"));
			m_pITableFrame->KillGameTimer(IDI_SHOW_CARD);
			m_pITableFrame->KillGameTimer(IDI_WAIT_COMAPRE);
			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));

			GameEnd.bEndMode = GER_DISMISS ;

			for (WORD i = 0; i < m_cbPlayerCount; ++i)
			{
				m_HistoryScore.OnEventUserLeave(i);

				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (pIServerUserItem && 0 != m_lUserChip[i])
				{
					//自动摆牌
					if (m_MobilePutCard[i].cbMobilePutCount > 0)
					{
						CopyMemory(m_bSegmentCard[i][0], &m_MobilePutCard[i].cbMobilePutCard[0][0], 3);
						CopyMemory(m_bSegmentCard[i][1], &m_MobilePutCard[i].cbMobilePutCard[0][3], 5);
						CopyMemory(m_bSegmentCard[i][2], &m_MobilePutCard[i].cbMobilePutCard[0][8], 5);
						CopyMemory(GameEnd.cbHandCard[i], &m_MobilePutCard[i].cbMobilePutCard[0], HAND_CARD_COUNT);
					}
					else
					{
						BYTE bPutCard[13] = { 0 };
						m_GameLogic.AutoPutCard(m_bUserCardData[i], bPutCard, false, NULL);
						CopyMemory(m_bSegmentCard[i][0], &bPutCard[0], 3);
						CopyMemory(m_bSegmentCard[i][1], &bPutCard[3], 5);
						CopyMemory(m_bSegmentCard[i][2], &bPutCard[8], 5);
						CopyMemory(GameEnd.cbHandCard[i], bPutCard, HAND_CARD_COUNT);
					}
				}
			}

			//发送数据


			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));


			//结束游戏
			m_pITableFrame->ConcludeGame(GS_WK_FREE);

			if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//房卡模式
			{
				if (m_pITableFrame->IsPersonalRoomDisumme())//当前朋友局解散清理记录
				{
					ZeroMemory(&m_stRecord, sizeof(m_stRecord));
				}
			}
			//更新房间用户信息
			for (WORD i = 0; i < m_cbPlayerCount; i++)
			{
				//获取用户
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}
			return true;
		}
	case GER_NORMAL:			//常规结束
		{
			WriteDebugInfo(_T("正常结束"));
			m_pITableFrame->KillGameTimer(IDI_SHOW_CARD);
			m_pITableFrame->KillGameTimer(IDI_WAIT_COMAPRE);
			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));
			GameEnd = m_stGameEnd;
			//历史积分
			for (WORD wID = 0; wID < m_cbPlayerCount; wID++)
			{
				m_HistoryScore.OnEventUserScore(wID, GameEnd.lGameScore[wID]);
				//朋友局记录
				if (m_stRecord.nCount < MAX_RECORD_COUNT)
				{
					m_stRecord.lDetailScore[wID][m_stRecord.nCount] = GameEnd.lGameScore[wID];
					m_stRecord.lAllScore[wID] += GameEnd.lGameScore[wID];
					if (GameEnd.lGameScore[wID]>0)
						m_stRecord.cbWinCount[wID]++;
					else if (GameEnd.lGameScore[wID] < 0)
						m_stRecord.cbLoseCount[wID]++;
					else
						m_stRecord.cbEqualCount[wID]++;
				}
				m_vecRecord[wID].push_back(GameEnd.lGameScore[wID]);
				if (m_vecRecord[wID].size()>30)
					m_vecRecord[wID].erase(m_vecRecord[wID].begin());

				//玩家手牌
				CopyMemory(&GameEnd.cbHandCard[wID][0], m_bSegmentCard[wID][0], 3);
				CopyMemory(&GameEnd.cbHandCard[wID][3], m_bSegmentCard[wID][1], 5);
				CopyMemory(&GameEnd.cbHandCard[wID][8], m_bSegmentCard[wID][2], 5);
			}
			m_stRecord.nCount++;

			if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//房卡模式
			{
				CMD_S_Record RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

				if (m_pITableFrame->GetDataBaseMode() == 0)
				{
					CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
				}
				else
				{
					for (int j = 0; j < m_cbPlayerCount; j++)
					{
						if (m_bGameStatus[j] == false)	continue;
						RoomCardRecord.nCount = m_vecRecord[j].size();
						for (int i = 0; i < RoomCardRecord.nCount; i++)
						{
							RoomCardRecord.lDetailScore[j][i] = m_vecRecord[j][i];
						}
						m_pITableFrame->SendTableData(j, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
					}
				}
				
			}
			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			//停止录像
			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd,sizeof(GameEnd));
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
			}
			//计算税收
			LONGLONG lRobotGameScore = 0;
			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD || m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)
			{
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
				
				for (int i=0;i<m_cbPlayerCount;i++)
				{		
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if(m_bGameStatus[i] == false) continue;

					if (pIServerUserItem)
					{
						LONGLONG lTmpTax = 0;
						if (GameEnd.lGameScore[i] > 0L && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0
							|| ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)))
						{
							lTmpTax = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
						}
							
						GameEnd.lGameTax	  += lTmpTax;
						GameEnd.lGameScore[i] -= lTmpTax;

						ScoreInfoArray[i].lScore   = GameEnd.lGameScore[i];
						ScoreInfoArray[i].cbType   = (GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
						ScoreInfoArray[i].lRevenue = lTmpTax; 
						
						//防止数据库负分
						if (ScoreInfoArray[i].lScore + pIServerUserItem->GetUserScore()<0 && 0!=m_pITableFrame->GetDataBaseMode())
							ScoreInfoArray[i].lScore = -pIServerUserItem->GetUserScore();

						m_pITableFrame->WriteUserScore(i,ScoreInfoArray[i]);
						CString cc;
						cc.Format(_T("玩家%d得分%I64d,税收%I64d"), i, GameEnd.lGameScore[i], GameEnd.lGameTax);
						WriteDebugInfo(cc);
					}
				}		
			}			

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_WK_FREE);	

			if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//房卡模式
			{
				if (m_pITableFrame->IsPersonalRoomDisumme())//当前朋友局解散清理记录
				{
					ZeroMemory(&m_stRecord, sizeof(m_stRecord));
				}
			}

			//更新房间用户信息
			for (WORD i = 0; i < m_cbPlayerCount; i++)
			{
				//获取用户
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}
				//库存累计
				if (!pIServerUserItem->IsAndroidUser())
					g_lRoomStorageCurrent -= GameEnd.lGameScore[i];

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}
			
			CMD_S_ADMIN_STORAGE_INFO StorageInfo;
			ZeroMemory(&StorageInfo, sizeof(StorageInfo));
			StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
			StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
			StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
			StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
			StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
			StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
			StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
			m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
	case GER_NETWORK_ERROR:
		{
			CString cc;
			cc.Format(_T("玩家%d强退，cbreson=%d"), wChairID, cbReason);
			WriteDebugInfo(cc);
			if (m_bStartCompare == false)
			{
				m_bGameStatus[wChairID] = false;
				StartCompareCard();
			}

            return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_WK_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));

			//设置变量
			StatusFree.lBaseScore = m_pITableFrame->GetCellScore();

			//自定义规则
			StatusFree.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
			StatusFree.cbTimeCallBanker = m_pGameCustomRule->cbTimeCallBanker;
			StatusFree.cbTimeSetChip = m_pGameCustomRule->cbTimeSetChip;
			StatusFree.cbTimeRangeCard = m_pGameCustomRule->cbTimeRangeCard;
			StatusFree.cbTimeShowCard = m_pGameCustomRule->cbTimeShowCard;
			StatusFree.bHaveBanker = m_bHaveBanker;
			StatusFree.nAllWinTimes = m_nAllWinTimes;
			StatusFree.cbMaCard		= m_cbMaCard;
			StatusFree.cbPlayCount = m_cbPlayerCount;
			StatusFree.cbTongHuaMode = m_cbTongHuaMode;
			StatusFree.cbQuickMode = m_cbQuickMode;
			//房间标识
			StatusFree.wServerID = m_pGameServiceOption->wServerID;

			//设置状态
			for(WORD i=0;i<m_cbPlayerCount;i++)
			{
				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i) ;
				if (pServerUserItem != NULL) m_bGameStatus[i]=true;
			}

			CopyMemory(StatusFree.bGameStatus, m_bGameStatus, sizeof(m_bGameStatus));

			//历史成绩
			for (WORD i=0;i<m_cbPlayerCount;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusFree.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i] = pHistoryScore->lCollectScore;
			}
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)!=0)
				StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			//发送场景
			bool bSuccess =  m_pITableFrame->SendGameScene( pIServerUserItem, &StatusFree, sizeof(StatusFree));
			if (bSuccess)
			{
				if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//房卡模式
				{
					CMD_S_Record RoomCardRecord;
					ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

					if (m_pITableFrame->GetDataBaseMode() == 0)
						CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
					else
					{
						RoomCardRecord.nCount = m_vecRecord[wChairID].size();
						for (int i = 0; i < RoomCardRecord.nCount; i++)
						{
							RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
						}
					}

					m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
				}

				if (pIServerUserItem->IsAndroidUser())
				{
					//获取自定义配置
					tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
					ASSERT(pCustomRule);

					tagCustomAndroid CustomAndroid;
					ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
					CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
					CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
					CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
					CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
					CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;

					m_pITableFrame->SendTableData(wChairID, SUB_S_ANDROID_INFO, &CustomAndroid, sizeof(CustomAndroid));
				}

			}
			return bSuccess;
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			CString cc;
			cc.Format(_T("玩家%d断线重连"), wChairID);
			WriteDebugInfo(cc);
			//更新房间用户信息
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));

			StatusPlay.lBaseScore = m_pITableFrame->GetCellScore();
			StatusPlay.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
			StatusPlay.cbTimeCallBanker = m_pGameCustomRule->cbTimeCallBanker;
			StatusPlay.cbTimeSetChip = m_pGameCustomRule->cbTimeSetChip;
			StatusPlay.cbTimeRangeCard = m_pGameCustomRule->cbTimeRangeCard;

			DWORD Time = (DWORD)time(NULL);
			BYTE cbTimeLeave = max(min(Time-m_dwShowTime,m_cbTimeRangeCard),0);

			StatusPlay.cbTimeShowCard = m_pGameCustomRule->cbTimeShowCard;
			StatusPlay.cbTimeLeave = m_cbTimeRangeCard - cbTimeLeave;
			//房间标识
			StatusPlay.wServerID = m_pGameServiceOption->wServerID;

			StatusPlay.cbPlayCount = m_cbPlayerCount;
			StatusPlay.wBanker	= m_wBanker ;
			StatusPlay.bHaveBanker = m_bHaveBanker;
			StatusPlay.nAllWinTimes = m_nAllWinTimes;
			StatusPlay.cbMaCard		= m_cbMaCard;
			StatusPlay.cbTongHuaMode = m_cbTongHuaMode;
			StatusPlay.cbQuickMode = m_cbQuickMode;
			CopyMemory(StatusPlay.bHandCardData , m_bUserCardData[wChairID] , HAND_CARD_COUNT) ;
			CopyMemory(StatusPlay.bSegmentCard , m_bSegmentCard , sizeof(m_bSegmentCard)) ;
			CopyMemory(StatusPlay.bFinishSegment , m_bFinishSegment , sizeof(m_bFinishSegment)) ;
			CopyMemory(StatusPlay.lChip , m_lUserChip , sizeof(m_lUserChip)) ;
			CopyMemory(StatusPlay.lScoreTimes, m_lScoreTimesExEx, sizeof(m_lScoreTimesExEx));
			CopyMemory(StatusPlay.lScoreTimesPlus, m_lscoreTimesPlus, sizeof(m_lscoreTimesPlus));
			CopyMemory(StatusPlay.bSpecialType, m_bSpecialType, sizeof(m_bSpecialType));
			CopyMemory(StatusPlay.bGameStatus,m_bGameStatus,sizeof(m_bGameStatus));

			CopyMemory(StatusPlay.bAllWin,m_bAllWin,sizeof(m_bAllWin));

			//是否特殊牌
			bool bHaveSpecial = false;

			for(WORD j=0 ; j<m_cbPlayerCount ; j++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(j);
				if ((pIServerUserItem == NULL) || (m_bGameStatus[j] == false)) continue;

				if (m_cbShowSpecialType[j] > CT_EX_INVALID)
				{
					bHaveSpecial=true;
					break;
				}
			}

			WORD wAllKillUser = INVALID_CHAIR;
			//全垒打
			if(bHaveSpecial==false && GetPlayerCount()==4 && m_bHaveBanker==false)
			{
				for(WORD i=0; i<m_cbPlayerCount;i++)
				{
					if (m_bGameStatus[i] == false) continue;
					//全垒打
					BYTE cbAllCount=0;
					for(WORD j=0;j<m_cbPlayerCount;j++)
					{
						if(i==j || m_bGameStatus[j] == false) continue;
						if(m_bAllWin[i][j]>0)
						{
							cbAllCount++;
						}
						else break;
					}
					if(cbAllCount+1==GetPlayerCount() && GetPlayerCount()==4)
					{
						wAllKillUser = i;
						break;
					}
				}
			}
			StatusPlay.wAllKillUser=wAllKillUser;
			//历史成绩
			for (WORD i=0;i<m_cbPlayerCount;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusPlay.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)!=0)
				StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();

			//发送场景
			bool bSuccess =  m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			//手机玩家发送组合牌
			if (pIServerUserItem->IsMobileUser() && m_bGameStatus[wChairID] != false)
			{
				m_pITableFrame->SendTableData(wChairID, SUB_S_MOBILE_PUTCARD, &m_MobilePutCard[wChairID], sizeof(CMD_S_MobilePutCard));
			}
			
			if (bSuccess)
			{
				if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//房卡模式
				{
					CMD_S_Record RoomCardRecord;
					ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

					if (m_pITableFrame->GetDataBaseMode() == 0)
						CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
					else
					{
						RoomCardRecord.nCount = m_vecRecord[wChairID].size();
						for (int i = 0; i < RoomCardRecord.nCount; i++)
						{
							RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
						}
					}
						
					m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
				}

				if (pIServerUserItem->IsAndroidUser())
				{
					//获取自定义配置
					tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
					ASSERT(pCustomRule);

					tagCustomAndroid CustomAndroid;
					ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
					CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
					CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
					CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
					CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
					CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;

					m_pITableFrame->SendTableData(wChairID, SUB_S_ANDROID_INFO, &CustomAndroid, sizeof(CustomAndroid));
				}
			}
			return bSuccess;
		}
	}

	return false;
}

//时间事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	if (wTimerID == IDI_SHOW_CARD)
	{
		m_pITableFrame->KillGameTimer(IDI_SHOW_CARD);
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0 || 1 == m_pITableFrame->GetDataBaseMode())
		{
			for (int i = 0; i < m_cbPlayerCount; i++)
			{
				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (pServerUserItem == NULL) continue;
				if (m_bGameStatus[i] == false) continue;

				if (m_bFinishSegment[i] == false)
				{
					CString cc;
					cc.Format(_T("给玩家%d自动亮牌"),i);
					WriteDebugInfo(cc);
					BYTE cbTempCard[13];
					ZeroMemory(cbTempCard, sizeof(cbTempCard));

					m_GameLogic.AutoPutCard(m_bUserCardData[i], cbTempCard, false, NULL);

					CMD_C_ShowCard UserShowCard;
					ZeroMemory(&UserShowCard, sizeof(CMD_C_ShowCard));
					CopyMemory(UserShowCard.bFrontCard, cbTempCard, sizeof(BYTE) * 3);
					CopyMemory(UserShowCard.bMidCard, &cbTempCard[3], sizeof(BYTE) * 5);
					CopyMemory(UserShowCard.bBackCard, &cbTempCard[8], sizeof(BYTE) * 5);

					OnUserShowCard(i, (void *)&UserShowCard, sizeof(CMD_C_ShowCard));
				}
			}
		}
	
		return true;
	}
	else if (wTimerID == IDI_WAIT_COMAPRE)
	{
		if (m_pITableFrame->GetGameStatus() != GS_WK_PLAYING)
			return true;
		WriteDebugInfo(_T("比牌倒计时结束"));
		return OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
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
	return true;
}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_SHOWARD:				//玩家摊牌
		{
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			return OnUserShowCard(pUserData->wChairID , pData, wDataSize) ;
		}
	case SUB_C_TRUSTEE:				//玩家托管
		{
			//长度校验
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pData;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;

			//用户校验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			//构造数据
			CMD_S_Trustee Trustee;
			ZeroMemory(&Trustee, sizeof(Trustee));
			Trustee.wChairID = pUserData->wChairID;
			Trustee.bTrustee = pTrustee->bTrustee;

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));

			return true;
		}
	case SUB_C_COMPLETE_COMPARE:	//完成比较
		{
			if (m_pITableFrame->GetGameStatus() != GS_WK_PLAYING)
				return true;
			if (m_bCompleteCompare[pIServerUserItem->GetChairID()] == false)
			{
				CString cc;
				cc.Format(_T("玩家%d比牌完成"), pIServerUserItem->GetChairID());
				WriteDebugInfo(cc);
				m_wCompleteCompareCount++;
				m_bCompleteCompare[pIServerUserItem->GetChairID()] = true;
				if (GetPlayerCount() <= m_wCompleteCompareCount)
				{
					return OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
				}
			}
			return true ;
		}
	}
	
	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// 消息处理
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
		case SUB_C_STORAGE:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UpdateStorage));
			if (wDataSize != sizeof(CMD_C_UpdateStorage)) return false;

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			CMD_C_UpdateStorage *pUpdateStorage = (CMD_C_UpdateStorage *)pData;
			g_lRoomStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
			g_lStorageDeductRoom = pUpdateStorage->lRoomStorageDeduct;

			//20条操作记录
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 调试账户[%s],修改当前库存为 %I64d,衰减值为 %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(),
				g_lRoomStorageCurrent, g_lStorageDeductRoom);

			g_ListOperationRecord.AddTail(strOperationRecord);

			//写入日志
			strOperationRecord += TEXT("\n");
			WriteDebugInfo(strOperationRecord);

			//变量定义
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//数组下标
			while (posListRecord)
			{
				CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

				CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
		case SUB_C_STORAGEMAXMUL:
		{
			ASSERT(wDataSize == sizeof(CMD_C_ModifyStorage));
			if (wDataSize != sizeof(CMD_C_ModifyStorage)) return false;

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pData;
			g_lStorageMax1Room = pModifyStorage->lMaxRoomStorage[0];
			g_lStorageMax2Room = pModifyStorage->lMaxRoomStorage[1];
			g_lStorageMul1Room = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
			g_lStorageMul2Room = (SCORE)(pModifyStorage->wRoomStorageMul[1]);

			//20条操作记录
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d,调试账户[%s], 修改库存上限值1为 %I64d,赢分概率1为 %I64d,上限值2为 %I64d,赢分概率2为 %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), g_lStorageMax1Room, g_lStorageMul1Room, g_lStorageMax2Room, g_lStorageMul2Room);

			g_ListOperationRecord.AddTail(strOperationRecord);

			//写入日志
			strOperationRecord += TEXT("\n");
			WriteDebugInfo(strOperationRecord);

			//变量定义
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//数组下标
			while (posListRecord)
			{
				CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

				CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
		case SUB_C_REQUEST_QUERY_USER:
		{
			ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
			if (wDataSize != sizeof(CMD_C_RequestQuery_User))
			{
				return false;
			}

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
			{
				return false;
			}

			CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pData;

			//遍历映射
			POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			CMD_S_RequestQueryResult QueryResult;
			ZeroMemory(&QueryResult, sizeof(QueryResult));

			while (ptHead)
			{
				g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
				if (pQuery_User->dwGameID == userinfo.dwGameID || _tcscmp(pQuery_User->szNickName, userinfo.szNickName) == 0)
				{
					//拷贝用户信息数据
					QueryResult.bFind = true;
					CopyMemory(&(QueryResult.userinfo), &userinfo, sizeof(userinfo));

					ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
					CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
				}
			}

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

			return true;
		}
		case SUB_C_USER_DEBUG:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UserDebug));
			if (wDataSize != sizeof(CMD_C_UserDebug))
			{
				return false;
			}

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_C_UserDebug *pUserDebug = (CMD_C_UserDebug *)pData;

			//遍历映射
			POSITION ptMapHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			//20条操作记录
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			//变量定义
			CMD_S_UserDebug serverUserDebug;
			ZeroMemory(&serverUserDebug, sizeof(serverUserDebug));

			TCHAR szNickName[LEN_NICKNAME];
			ZeroMemory(szNickName, sizeof(szNickName));

			//激活调试
			if (pUserDebug->userDebugInfo.bCancelDebug == false)
			{
				ASSERT(pUserDebug->userDebugInfo.debug_type == CONTINUE_WIN || pUserDebug->userDebugInfo.debug_type == CONTINUE_LOST);

				while (ptMapHead)
				{
					g_MapRoomUserInfo.GetNextAssoc(ptMapHead, dwUserID, userinfo);

					if (_tcscmp(pUserDebug->szNickName, szNickName) == 0 && _tcscmp(userinfo.szNickName, szNickName) == 0)
					{
						continue;
					}

					if (pUserDebug->dwGameID == userinfo.dwGameID || _tcscmp(pUserDebug->szNickName, userinfo.szNickName) == 0)
					{
						//激活调试标识
						bool bEnableDebug = false;
						IsSatisfyDebug(userinfo, bEnableDebug);

						//满足调试
						if (bEnableDebug)
						{
							ROOMUSERDEBUG roomuserdebug;
							ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
							CopyMemory(&(roomuserdebug.roomUserInfo), &userinfo, sizeof(userinfo));
							CopyMemory(&(roomuserdebug.userDebug), &(pUserDebug->userDebugInfo), sizeof(roomuserdebug.userDebug));


							//遍历链表，除重
							TravelDebugList(roomuserdebug);

							//压入链表（不压入同GAMEID和NICKNAME)
							g_ListRoomUserDebug.AddHead(roomuserdebug);

							//复制数据
							serverUserDebug.dwGameID = userinfo.dwGameID;
							CopyMemory(serverUserDebug.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
							serverUserDebug.debugResult = DEBUG_SUCCEED;
							serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
							serverUserDebug.cbDebugCount = pUserDebug->userDebugInfo.cbDebugCount;

							//操作记录
							CString strOperationRecord;
							CString strDebugType;
							GetDebugTypeString(serverUserDebug.debugType, strDebugType);
							CTime time = CTime::GetCurrentTime();
							strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 调试账户[%s], 调试玩家%s,%s,调试局数%d "),
								time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName, strDebugType, serverUserDebug.cbDebugCount);

							g_ListOperationRecord.AddTail(strOperationRecord);

							//写入日志
							strOperationRecord += TEXT("\n");
							WriteDebugInfo(strOperationRecord);
						}
						else	//不满足
						{
							//复制数据
							serverUserDebug.dwGameID = userinfo.dwGameID;
							CopyMemory(serverUserDebug.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
							serverUserDebug.debugResult = DEBUG_FAIL;
							serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
							serverUserDebug.cbDebugCount = 0;

							//操作记录
							CString strOperationRecord;
							CString strDebugType;
							GetDebugTypeString(serverUserDebug.debugType, strDebugType);
							CTime time = CTime::GetCurrentTime();
							strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 调试账户[%s], 调试玩家%s,%s,失败！"),
								time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName, strDebugType);

							g_ListOperationRecord.AddTail(strOperationRecord);

							//写入日志
							strOperationRecord += TEXT("\n");
							WriteDebugInfo(strOperationRecord);
						}

						//发送数据
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

						CMD_S_Operation_Record OperationRecord;
						ZeroMemory(&OperationRecord, sizeof(OperationRecord));
						POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
						WORD wIndex = 0;//数组下标
						while (posListRecord)
						{
							CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

							CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
							wIndex++;
						}

						ASSERT(wIndex <= MAX_OPERATION_RECORD);

						//发送数据
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
						return true;
					}
				}

				ASSERT(FALSE);
				return false;
			}
			else	//取消调试
			{
				ASSERT(pUserDebug->userDebugInfo.debug_type == CONTINUE_CANCEL);

				POSITION ptListHead = g_ListRoomUserDebug.GetHeadPosition();
				POSITION ptTemp;
				ROOMUSERDEBUG roomuserdebug;
				ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

				//遍历链表
				while (ptListHead)
				{
					ptTemp = ptListHead;
					roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);
					if (pUserDebug->dwGameID == roomuserdebug.roomUserInfo.dwGameID || _tcscmp(pUserDebug->szNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
					{
						//复制数据
						serverUserDebug.dwGameID = roomuserdebug.roomUserInfo.dwGameID;
						CopyMemory(serverUserDebug.szNickName, roomuserdebug.roomUserInfo.szNickName, sizeof(roomuserdebug.roomUserInfo.szNickName));
						serverUserDebug.debugResult = DEBUG_CANCEL_SUCCEED;
						serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
						serverUserDebug.cbDebugCount = 0;

						//移除元素
						g_ListRoomUserDebug.RemoveAt(ptTemp);

						//发送数据
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

						//操作记录
						CString strOperationRecord;
						CTime time = CTime::GetCurrentTime();
						strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 调试账户[%s], 取消对玩家%s的调试！"),
							time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName);

						g_ListOperationRecord.AddTail(strOperationRecord);

						//写入日志
						strOperationRecord += TEXT("\n");
						WriteDebugInfo(strOperationRecord);

						CMD_S_Operation_Record OperationRecord;
						ZeroMemory(&OperationRecord, sizeof(OperationRecord));
						POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
						WORD wIndex = 0;//数组下标
						while (posListRecord)
						{
							CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

							CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
							wIndex++;
						}

						ASSERT(wIndex <= MAX_OPERATION_RECORD);

						//发送数据
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

						return true;
					}
				}

				//复制数据
				serverUserDebug.dwGameID = pUserDebug->dwGameID;
				CopyMemory(serverUserDebug.szNickName, pUserDebug->szNickName, sizeof(serverUserDebug.szNickName));
				serverUserDebug.debugResult = DEBUG_CANCEL_INVALID;
				serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
				serverUserDebug.cbDebugCount = 0;

				//发送数据
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

				//操作记录
				CString strOperationRecord;
				CTime time = CTime::GetCurrentTime();
				strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 调试账户[%s], 取消对玩家%s的调试，操作无效！"),
					time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName);

				g_ListOperationRecord.AddTail(strOperationRecord);

				//写入日志
				strOperationRecord += TEXT("\n");
				WriteDebugInfo(strOperationRecord);

				CMD_S_Operation_Record OperationRecord;
				ZeroMemory(&OperationRecord, sizeof(OperationRecord));
				POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
				WORD wIndex = 0;//数组下标
				while (posListRecord)
				{
					CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

					CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
					wIndex++;
				}

				ASSERT(wIndex <= MAX_OPERATION_RECORD);

				//发送数据
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			}

			return true;
		}
		case SUB_C_REQUEST_UDPATE_ROOMINFO:
		{
			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_S_RequestUpdateRoomInfo_Result RoomInfo_Result;
			ZeroMemory(&RoomInfo_Result, sizeof(RoomInfo_Result));

			RoomInfo_Result.lRoomStorageCurrent = g_lRoomStorageCurrent;


			DWORD dwKeyGameID = g_CurrentQueryUserInfo.dwGameID;
			TCHAR szKeyNickName[LEN_NICKNAME];
			ZeroMemory(szKeyNickName, sizeof(szKeyNickName));
			CopyMemory(szKeyNickName, g_CurrentQueryUserInfo.szNickName, sizeof(szKeyNickName));

			//遍历映射
			POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			while (ptHead)
			{
				g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
				if (dwKeyGameID == userinfo.dwGameID && _tcscmp(szKeyNickName, userinfo.szNickName) == 0)
				{
					//拷贝用户信息数据
					CopyMemory(&(RoomInfo_Result.currentqueryuserinfo), &userinfo, sizeof(userinfo));

					ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
					CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
				}
			}


			//
			//变量定义
			POSITION ptListHead = g_ListRoomUserDebug.GetHeadPosition();
			POSITION ptTemp;
			ROOMUSERDEBUG roomuserdebug;
			ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

			//遍历链表
			while (ptListHead)
			{
				ptTemp = ptListHead;
				roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

				//寻找玩家
				if ((dwKeyGameID == roomuserdebug.roomUserInfo.dwGameID) &&
					_tcscmp(szKeyNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
				{
					RoomInfo_Result.bExistDebug = true;
					CopyMemory(&(RoomInfo_Result.currentuserdebug), &(roomuserdebug.userDebug), sizeof(roomuserdebug.userDebug));
					break;
				}
			}

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));

			CMD_S_ADMIN_STORAGE_INFO StorageInfo;
			ZeroMemory(&StorageInfo, sizeof(StorageInfo));
			StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
			StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
			StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
			StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
			StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
			StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
			StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
			m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));

			return true;
		}
		case SUB_C_CLEAR_CURRENT_QUERYUSER:
		{
			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

			return true;
		}
		}
	}
	return false;
}
//玩家摊牌
bool CTableFrameSink::OnUserShowCard(const WORD wChairID , const void * pDataBuffer, WORD wDataSize)
{
	//数据验证 
	ASSERT(INVALID_CHAIR!=wChairID) ;
	if(INVALID_CHAIR==wChairID) return false ;

	ASSERT(sizeof(CMD_C_ShowCard)==wDataSize) ;
	if(sizeof(CMD_C_ShowCard)!=wDataSize) return false ;

	if (m_bFinishSegment[wChairID] == true)
		return false;

	CMD_C_ShowCard *pSetSeg = (CMD_C_ShowCard*) pDataBuffer ;
	
	//校验扑克
	BYTE cbCardDataTemp[HAND_CARD_COUNT];
	ZeroMemory(cbCardDataTemp, sizeof(cbCardDataTemp));
	CopyMemory(&cbCardDataTemp[0], pSetSeg->bFrontCard, 3);
	CopyMemory(&cbCardDataTemp[3], pSetSeg->bMidCard,	5);
	CopyMemory(&cbCardDataTemp[8], pSetSeg->bBackCard,  5);

	m_GameLogic.SortCardList(cbCardDataTemp,13);
	m_GameLogic.SortCardList(m_bUserCardData[wChairID],13);

	CString cc;
	cc.Format(_T("玩家%d亮牌,特殊牌型%d:%0x,%0x,%0x,%0x,%0x,%0x,%0x,%0x,%0x,%0x,%0x,%0x,%0x"), wChairID, pSetSeg->cbSpecialCardType, pSetSeg->bFrontCard[0], pSetSeg->bFrontCard[1], pSetSeg->bFrontCard[2],
		pSetSeg->bMidCard[0], pSetSeg->bMidCard[1], pSetSeg->bMidCard[2], pSetSeg->bMidCard[3], pSetSeg->bMidCard[4],
		pSetSeg->bBackCard[0], pSetSeg->bBackCard[1], pSetSeg->bBackCard[2], pSetSeg->bBackCard[3], pSetSeg->bBackCard[4]);
	WriteDebugInfo(cc);

	for(BYTE i=0; i<HAND_CARD_COUNT; i++)
	{
		if(cbCardDataTemp[i] != m_bUserCardData[wChairID][i])
		{
			WriteDebugInfo(_T("牌数据不一致"));
			return false;
		}
	}

	//防止乌龙
	if(	 pSetSeg->cbSpecialCardType<CT_EX_INVALID &&
		(enCRGreater != m_GameLogic.CompareCard(pSetSeg->bFrontCard, pSetSeg->bMidCard, 3, 5, false) ||
		enCRGreater != m_GameLogic.CompareCard(pSetSeg->bMidCard, pSetSeg->bBackCard, 5, 5, true))
		)
	{
		WriteDebugInfo(_T("乌龙牌"));
		return false;
	}

	//设置数据
	CopyMemory(m_bSegmentCard[wChairID][0], pSetSeg->bFrontCard, 3);
	CopyMemory(m_bSegmentCard[wChairID][1], pSetSeg->bMidCard,	 5);
	CopyMemory(m_bSegmentCard[wChairID][2], pSetSeg->bBackCard,  5);
	if(pSetSeg->cbSpecialCardType>CT_EX_INVALID)
	{
		m_cbShowSpecialType[wChairID]=pSetSeg->cbSpecialCardType;
	}
	m_bFinishSegment[wChairID] = true;
	m_wShowCardCount++;

	//通知开牌
	CMD_S_ShowCard  ShowCard ;
	ZeroMemory(&ShowCard, sizeof(ShowCard));
	for (WORD i = 0; i < m_cbPlayerCount; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if ((pIServerUserItem == NULL))	continue;

		ShowCard.wCurrentUser = wChairID;

		if (i == wChairID)
		{
			ShowCard.bCanSeeShowCard  = true;
			CopyMemory(ShowCard.bFrontCard, m_bSegmentCard[wChairID][0], 3);
			CopyMemory(ShowCard.bMidCard,   m_bSegmentCard[wChairID][1], 5);
			CopyMemory(ShowCard.bBackCard,  m_bSegmentCard[wChairID][2], 5);
		}
		else
			ShowCard.bCanSeeShowCard  = false;

		m_pITableFrame->SendTableData(i,SUB_S_SHOW_CARD,&ShowCard,sizeof(ShowCard));
		m_pITableFrame->SendLookonData(i,SUB_S_SHOW_CARD,&ShowCard,sizeof(ShowCard)); 
	}
	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_SHOW_CARD, &ShowCard,sizeof(ShowCard));
	}
	//开始比牌
	StartCompareCard();

	return true ;
}

//获取下注
void CTableFrameSink::GetChips( LONGLONG lChipArray[GAME_PLAYER][SET_CHIP_COUNT])
{
	//结果清零
	ZeroMemory(lChipArray, sizeof(lChipArray));
	
	LONGLONG lBankerScore = m_pITableFrame->GetTableUserItem( m_wBanker )->GetUserScore();
	LONGLONG m_wMaxChip = m_pGameServiceOption->lRestrictScore;
	for(BYTE i=0; i<m_cbPlayerCount;i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if ((pIServerUserItem == NULL)) continue;

		//最大下注
		LONGLONG lMeScore, lMaxChip ;
		lMeScore = pIServerUserItem->GetUserScore() ;

		if (lMeScore > m_wMaxChip && m_wMaxChip != 0) lMeScore = m_wMaxChip;
		if (lBankerScore > m_wMaxChip && m_wMaxChip != 0) lBankerScore = m_wMaxChip;

		lMaxChip = (min(lMeScore,  lBankerScore))/36;

		for (BYTE cbChipIndex = 0; cbChipIndex < SET_CHIP_COUNT; ++cbChipIndex)
		{
			lChipArray[i][cbChipIndex] = LONGLONG ((cbChipIndex + 1.0 ) / SET_CHIP_COUNT * lMaxChip) ;

		}
	}
	
	return;
}

//获取下注
void CTableFrameSink::GetChips(WORD wChairID, LONGLONG lChipArray[SET_CHIP_COUNT])
{
	//结果清零
	ZeroMemory(lChipArray, sizeof(lChipArray));
	
	LONGLONG lBankerScore = m_pITableFrame->GetTableUserItem( m_wBanker )->GetUserScore();
	LONGLONG m_wMaxChip = m_pGameServiceOption->lRestrictScore;
	
	//最大下注
	LONGLONG lMeScore, lMaxChip ;
	lMeScore = m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore() ;

	if (lMeScore > m_wMaxChip && m_wMaxChip != 0) lMeScore = m_wMaxChip;
	if (lBankerScore > m_wMaxChip && m_wMaxChip != 0) lBankerScore = m_wMaxChip;

	lMaxChip = (min(lMeScore,  lBankerScore))/36;

	for (BYTE cbChipIndex = 0; cbChipIndex < SET_CHIP_COUNT; ++cbChipIndex)
	{
		lChipArray[cbChipIndex] = LONGLONG ((cbChipIndex + 1.0 ) / SET_CHIP_COUNT * lMaxChip) ;

		////整百处理
		//if (lChipArray[cbChipIndex] > 100)
		//{
		//	LONGLONG lHundredCount = lChipArray[cbChipIndex] / 100;
		//	lChipArray[cbChipIndex] = lHundredCount * 100 ;
		//}
	}

	return;
}

//获取倍数
int CTableFrameSink::GetCardTimes(BYTE bCard[13])
{
	//组合扑克
	BYTE bCardData[13] = {0};
	CopyMemory(bCardData,	&bCard[0], sizeof(BYTE)*3);
	CopyMemory(bCardData+3, &bCard[3], sizeof(BYTE)*5);
	CopyMemory(bCardData+8, &bCard[8], sizeof(BYTE)*5);

	//获取牌型
	BYTE cbType = m_GameLogic.GetSpecialType(bCard, 13);

	if (cbType == CT_EX_ZHIZUNQINGLONG) return 36;
	else if (cbType == CT_EX_YITIAOLONG) return 16;
	else if (cbType == CT_EX_LIUDUIBAN) return 3;
	else if (cbType == CT_EX_SANSHUNZI) return 3;
	else if (cbType == CT_EX_SANTONGHUA) return 3;

	//牌型翻倍
	BYTE cbMaxCard = 0;
	int nMultiple[3] = {1,1,1};
	BYTE bFrontCT = m_GameLogic.GetCardType(&bCardData[0], 3, cbMaxCard);
	BYTE bMidCT	  = m_GameLogic.GetCardType(&bCardData[3], 5, cbMaxCard);
	BYTE bBackCT  = m_GameLogic.GetCardType(&bCardData[8], 5, cbMaxCard);
			
	//第一注
	if (bFrontCT == CT_THREE)							nMultiple[0] = 3;		//三条
				
	//第二注
	if (bMidCT >= CT_FIVE_STRAIGHT_FLUSH_FIRST_A)		nMultiple[1] = 10;		//同花顺
	else if (bMidCT == CT_FIVE_FOUR_ONE)				nMultiple[1] = 8;		//铁支					
	else if (bMidCT == CT_FIVE_THREE_DEOUBLE)			nMultiple[1] = 3;		//葫芦

	//第三注			
	if (bBackCT >= CT_FIVE_STRAIGHT_FLUSH_FIRST_A)		nMultiple[2] = 5;		//同花顺			
	else if (bBackCT == CT_FIVE_FOUR_ONE)				nMultiple[2] = 4;		 //铁支

	int nResult = nMultiple[0]+nMultiple[1]+nMultiple[2];
	return nResult;

}

SCORE CTableFrameSink::JudgeStock()
{
	//确定码牌玩家
	if (m_cbMaCard != 0)
	{
		//确定码牌玩家
		for (int i = 0; i < m_cbPlayerCount; i++)
		{
			bool bMaCard = false;

			for (int j = 0; j < HAND_CARD_COUNT; j++)
			{
				//有码牌的为码牌玩家
				if (m_cbMaCard == m_bUserCardData[i][j])
				{
					m_wMaCardUser = i;
					bMaCard = true;
					break;
				}
			}

			if (bMaCard) break;
		}
	}

	for (int i = 0; i < m_cbPlayerCount; i++)
	{
		if (m_bGameStatus[i] == true)
		{
			CMD_S_MobilePutCard MobilePutCard;
			ZeroMemory(&MobilePutCard, sizeof(CMD_S_MobilePutCard));
			MobilePutCard.cbSpecialType = m_GameLogic.GetSpecialType(m_bUserCardData[i], 13);
			if (MobilePutCard.cbSpecialType > CT_EX_INVALID)
			{
				m_GameLogic.GetSpecialCardData(m_bUserCardData[i], 13, MobilePutCard.cbSpecialData);
				m_bSpecialType[i] = MobilePutCard.cbSpecialType;
				CopyMemory(m_bSegmentCard[i][0], MobilePutCard.cbSpecialData[0], 3);
				CopyMemory(m_bSegmentCard[i][1], MobilePutCard.cbSpecialData[1], 5);
				CopyMemory(m_bSegmentCard[i][2], MobilePutCard.cbSpecialData[2], 5);
			}
			else
			{
				BYTE cbTemp[13] = { 0 };
				m_GameLogic.AutoPutCard(m_bUserCardData[i], cbTemp, false, NULL);
				CopyMemory(m_bSegmentCard[i][0], &cbTemp[0], 3);
				CopyMemory(m_bSegmentCard[i][1], &cbTemp[3], 5);
				CopyMemory(m_bSegmentCard[i][2], &cbTemp[8], 5);
			}
		}
	}

	StartCompareCard();

	//机器人数
	bool bIsAiBanker = false;
	for (WORD i = 0; i < m_cbPlayerCount; i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL)
		{
			if (m_bGameStatus[i] == FALSE)continue;
			if (pIServerUserItem->IsAndroidUser())
			{
				if (!bIsAiBanker && i == m_wBanker)
					bIsAiBanker = true;
			}
		}
	}

	SCORE lAndroidScore = 0;
	//机器庄家
	if (bIsAiBanker)
	{
		//对比扑克
		for (WORD i = 0; i < m_cbPlayerCount; i++)
		{
			//用户过滤
			if ((i == m_wBanker) || (m_bGameStatus[i] == FALSE)) continue;

			//获取用户
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

			//机器过滤
			if ((pIServerUserItem != NULL) && (pIServerUserItem->IsAndroidUser())) continue;

			lAndroidScore -= m_lTestScore[i];
		}
	}
	else//用户庄家或无庄
	{
		//对比扑克
		for (WORD i = 0; i < m_cbPlayerCount; i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

			//用户过滤
			if ((i == m_wBanker) || (pIServerUserItem == NULL) || !(pIServerUserItem->IsAndroidUser())) continue;

			lAndroidScore += m_lTestScore[i];
		}
	}

	return lAndroidScore;
}

void CTableFrameSink::CalculateScore()
{
	ZeroMemory(m_lscoreTimesPlus, sizeof(m_lscoreTimesPlus));
	ZeroMemory(&m_stGameEnd, sizeof(m_stGameEnd));
	m_stGameEnd.bEndMode = GER_NORMAL;
	//庄闲比
	if (m_bHaveBanker)
	{
		for (WORD i = 0; i < m_cbPlayerCount; ++i)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (m_bGameStatus[i] == false) continue;

			if (pIServerUserItem)
			{
				if (i != m_wBanker)
				{
					if (m_bSpecialType[i] > CT_EX_INVALID || m_bSpecialType[m_wBanker] > CT_EX_INVALID)
					{
						int nMulOnSD = 0;
						BYTE bMaxSpecialType = max(m_bSpecialType[i], m_bSpecialType[m_wBanker]);
						nMulOnSD = m_GameLogic.GetSpecialCardMutile(bMaxSpecialType);
						if (m_bSpecialType[i] > m_bSpecialType[m_wBanker]) nMulOnSD *= 1;
						else nMulOnSD *= -1;

						//如果有码牌，则输赢翻倍
						if (m_cbMaCard != 0 && (i == m_wMaCardUser || m_wBanker == m_wMaCardUser)) nMulOnSD *= 2;

						m_stGameEnd.lGameScore[i] = nMulOnSD * m_lUserChip[i];
						m_stGameEnd.lGameScore[m_wBanker] -= nMulOnSD * m_lUserChip[i];

						//分数倍数
						m_stGameEnd.lScoreTimes[i] = nMulOnSD;
						m_stGameEnd.lScoreTimes[m_wBanker] -= nMulOnSD;

						if (nMulOnSD == 0)			m_stGameEnd.bWinInfo[i] = 1;
						else if (nMulOnSD > 0)		m_stGameEnd.bWinInfo[i] = 2;
						else if (nMulOnSD < 0)		m_stGameEnd.bWinInfo[i] = 0;
					}
					//普通牌型
					else
					{
						//如果有码牌，则输赢翻倍
						int nTimes = 1;
						if (m_cbMaCard != 0 && (i == m_wMaCardUser || m_wBanker == m_wMaCardUser))
							nTimes *= 2;

						//打枪
						if (m_bAllWin[m_wBanker][i] != 0)
							nTimes *= 2;

						m_stGameEnd.lScoreTimes[i] = nTimes*m_lScoreTimesEx[i][m_wBanker];
						m_stGameEnd.lScoreTimes[m_wBanker] -= nTimes*m_lScoreTimesEx[i][m_wBanker];

						if (m_lScoreTimesEx[i][m_wBanker] == 0)			m_stGameEnd.bWinInfo[i] = 1;
						else if (m_lScoreTimesEx[i][m_wBanker] > 0)		m_stGameEnd.bWinInfo[i] = 2;
						else if (m_lScoreTimesEx[i][m_wBanker] < 0)		m_stGameEnd.bWinInfo[i] = 0;

						m_stGameEnd.lGameScore[i] = m_lScoreTimesEx[i][m_wBanker]*nTimes * m_lUserChip[i];
						m_stGameEnd.lGameScore[m_wBanker] -= m_lScoreTimesEx[i][m_wBanker] * nTimes * m_lUserChip[i];

						for (int j = 0; j < 3; j++)
						{
							m_lscoreTimesPlus[i][j] = nTimes*m_lScoreTimesExEx[i][m_wBanker][j];
							m_lscoreTimesPlus[m_wBanker][j] -= nTimes*m_lScoreTimesExEx[i][m_wBanker][j];
						}
					}
				}
			}
		}
	}
	else	//通比
	{
		SCORE lSpecialTimes[GAME_PLAYER];
		ZeroMemory(lSpecialTimes, sizeof(lSpecialTimes));
		bool bHaveSpecial = false;

		//特殊牌型(互相抵扣)
		for (int i = 0; i < m_cbPlayerCount; ++i)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (m_bGameStatus[i] == false || pIServerUserItem == NULL) continue;

			if (m_bSpecialType[i] > CT_EX_INVALID)
			{
				for (int j = 0; j < m_cbPlayerCount; j++)
				{
					IServerUserItem *pIServerUserItemJ = m_pITableFrame->GetTableUserItem(i);
					if (m_bGameStatus[j] == false || pIServerUserItemJ == NULL || i == j) continue;

					int nMulOnSD = 0;
					nMulOnSD = m_GameLogic.GetSpecialCardMutile(m_bSpecialType[i]);
					//如果有码牌，则输赢翻倍
					if (m_cbMaCard != 0 && (i == m_wMaCardUser || j == m_wMaCardUser)) nMulOnSD *= 2;

					lSpecialTimes[i] += nMulOnSD;
					lSpecialTimes[j] -= nMulOnSD;

					bHaveSpecial = true;
				}
			}
		}

		//计算倍数
		for (int i = 0; i < m_cbPlayerCount; ++i)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (m_bGameStatus[i] == false) continue;

			if (pIServerUserItem)
			{
				//最终结算
				int nAllMultiple = 0;
				
				//最终结算
				for (WORD j = 0; j < m_cbPlayerCount; j++)
				{
					if (m_bGameStatus[j] == false || i == j) continue;

					//如果有码牌，则输赢翻倍
					int nTimes = 1;
					if (m_cbMaCard != 0 && (i == m_wMaCardUser || j == m_wMaCardUser))
						nTimes *= 2;

					if (m_bAllWin[i][j] != 0)
					{
						nTimes *= 2;
					}

					if (i == m_wAllKillUser || j == m_wAllKillUser)
						nTimes *= 2;

					nAllMultiple += nTimes*m_lScoreTimesEx[i][j];

					for (int k = 0; k < 3; k++)
					{
						m_lscoreTimesPlus[i][k] += nTimes*m_lScoreTimesExEx[i][j][k];
					}

				}


				nAllMultiple += lSpecialTimes[i];

				m_stGameEnd.lScoreTimes[i] = nAllMultiple;

				if (nAllMultiple == 0)			m_stGameEnd.bWinInfo[i] = 1;
				else if (nAllMultiple > 0)		m_stGameEnd.bWinInfo[i] = 2;
				else if (nAllMultiple < 0)		m_stGameEnd.bWinInfo[i] = 0;

				m_stGameEnd.lGameScore[i] = nAllMultiple * m_lUserChip[i];
			}
		}
	}

	ZeroMemory(m_lTestScore, sizeof(m_lTestScore));
	CopyMemory(m_lTestScore, m_stGameEnd.lGameScore, sizeof(m_lTestScore));
}

//获取倍数
void CTableFrameSink::CompareCardGetTimes()
{
	ZeroMemory(m_bAllWin,sizeof(m_bAllWin));
	ZeroMemory(m_lScoreTimesEx, sizeof(m_lScoreTimesEx));
	ZeroMemory(m_lScoreTimesExEx, sizeof(m_lScoreTimesExEx));
	//庄闲比牌
	if(m_bHaveBanker)
	{
		for(WORD i=0 ; i<m_cbPlayerCount ; i++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if ((pIServerUserItem == NULL) || (m_bGameStatus[i] == false)) continue;

			//计算输赢
			if (i != m_wBanker)
			{
				if(m_bSpecialType[i]!=0||m_bSpecialType[m_wBanker]!=0)
				{
					continue;
				}

				//变量定义
				BYTE bWindInfo[3] = {0};

				//检查胜负	
				if(enCRGreater == m_GameLogic.CompareCard(m_bSegmentCard[m_wBanker][0] , m_bSegmentCard[i][0] , 3 , 3 , true)) bWindInfo[0] = TRUE;
				else bWindInfo[0] = FALSE;
				if(enCRGreater == m_GameLogic.CompareCard(m_bSegmentCard[m_wBanker][1] , m_bSegmentCard[i][1] , 5 , 5 , true)) bWindInfo[1] = TRUE;
				else bWindInfo[1] = FALSE;
				if(enCRGreater == m_GameLogic.CompareCard(m_bSegmentCard[m_wBanker][2] , m_bSegmentCard[i][2] , 5 , 5 , true)) bWindInfo[2] = TRUE;
				else bWindInfo[2] = FALSE;

				//判断输赢
				for (BYTE bIdx=0; bIdx<3; bIdx++)
				{
					WORD wWinUser = (bWindInfo[bIdx] == TRUE) ? i : m_wBanker;
					INT nMultiple = (bWindInfo[bIdx] == TRUE) ? 1 : -1;

					//牌型翻倍
					BYTE cbMaxCard = 0;
					
					//第一注
					if (bIdx == 0)
					{					
						BYTE bFrontCT = m_GameLogic.GetCardType(m_bSegmentCard[wWinUser][0], 3, cbMaxCard);
						//第一注三条
						if (bFrontCT == CT_THREE)						nMultiple *= 3;
					}

					//第二注
					if (bIdx == 1)
					{						
						BYTE bMidCT = m_GameLogic.GetCardType(m_bSegmentCard[wWinUser][1], 5, cbMaxCard);
						//同花顺
						if (bMidCT >= CT_FIVE_STRAIGHT_FLUSH)	nMultiple *= 10;

						//铁支		
						if (bMidCT == CT_FIVE_FOUR_ONE)					nMultiple *= 8;

						//葫芦
						if (bMidCT == CT_FIVE_THREE_DEOUBLE)			nMultiple *= 2;
					}

					//第三注
					if (bIdx == 2)
					{
						BYTE bBackCT = m_GameLogic.GetCardType(m_bSegmentCard[wWinUser][2], 5, cbMaxCard);
						//同花顺
						if (bBackCT >= CT_FIVE_STRAIGHT_FLUSH)	nMultiple *= 5;

						//铁支
						if (bBackCT == CT_FIVE_FOUR_ONE)				nMultiple *= 4;
					}

					m_lScoreTimes[i][bIdx]		   = nMultiple;
					m_lScoreTimes[m_wBanker][bIdx]-= nMultiple;
					m_lScoreTimesExEx[i][m_wBanker][bIdx] = nMultiple;
					m_lScoreTimesExEx[m_wBanker][i][bIdx] -= nMultiple;

					m_lScoreTimesEx[i][m_wBanker] += nMultiple;
					m_lScoreTimesEx[m_wBanker][i] -= nMultiple;
				}

				if(m_lScoreTimes[i][0]>0 && m_lScoreTimes[i][1]>0 && m_lScoreTimes[i][2]>0)
				{
					m_bAllWin[m_wBanker][i] = -1;
					m_bAllWin[i][m_wBanker] = 1;
				}
				else if(m_lScoreTimes[i][0]<0 && m_lScoreTimes[i][1]<0 && m_lScoreTimes[i][2]<0)
				{
					m_bAllWin[m_wBanker][i] = 1;
					m_bAllWin[i][m_wBanker] = -1;
				}
			}
		}
	}
	//通比模式
	else
	{
		for(WORD i=0 ; i<m_cbPlayerCount-1 ; i++)
		{
			IServerUserItem *pIServerUserItemI = m_pITableFrame->GetTableUserItem(i);
			if ((pIServerUserItemI == NULL) || (m_bGameStatus[i] == false)) continue;
			if(m_bSpecialType[i]>CT_EX_INVALID) continue;

			for(WORD j=i+1;j<m_cbPlayerCount;j++)
			{
				IServerUserItem *pIServerUserItemJ = m_pITableFrame->GetTableUserItem(i);
				if ((pIServerUserItemJ == NULL) || (m_bGameStatus[j] == false)) continue;

				if(m_bSpecialType[j]>CT_EX_INVALID)	continue;
	
				//变量定义
				BYTE bWindInfo[3] = {0};

				//检查胜负	
				if(enCRGreater == m_GameLogic.CompareCard(m_bSegmentCard[j][0] , m_bSegmentCard[i][0] , 3 , 3 , true)) bWindInfo[0] = TRUE;
				else bWindInfo[0] = FALSE;
				if(enCRGreater == m_GameLogic.CompareCard(m_bSegmentCard[j][1] , m_bSegmentCard[i][1] , 5 , 5 , true)) bWindInfo[1] = TRUE;
				else bWindInfo[1] = FALSE;
				if(enCRGreater == m_GameLogic.CompareCard(m_bSegmentCard[j][2] , m_bSegmentCard[i][2] , 5 , 5 , true)) bWindInfo[2] = TRUE;
				else bWindInfo[2] = FALSE;


				//判断输赢
				INT cbTotalMultiple = 0;
				for (BYTE bIdx=0; bIdx<3; bIdx++)
				{
					WORD wWinUser = (bWindInfo[bIdx] == TRUE) ? i : j;
					INT nMultiple = (bWindInfo[bIdx] == TRUE) ? 1 : -1;
				
					//牌型翻倍
					BYTE cbMaxCard = 0;
					
					//第一注
					if (bIdx == 0)
					{					
						BYTE bFrontCT = m_GameLogic.GetCardType(m_bSegmentCard[wWinUser][0], 3, cbMaxCard);
						//第一注三条
						if (bFrontCT == CT_THREE)						nMultiple *= 3;
					}

					//第二注
					if (bIdx == 1)
					{						
						BYTE bMidCT = m_GameLogic.GetCardType(m_bSegmentCard[wWinUser][1], 5, cbMaxCard);
						//同花顺
						if (bMidCT >= CT_FIVE_STRAIGHT_FLUSH)	nMultiple *= 10;

						//铁支		
						if (bMidCT == CT_FIVE_FOUR_ONE)					nMultiple *= 8;

						//葫芦
						if (bMidCT == CT_FIVE_THREE_DEOUBLE)			nMultiple *= 2;
					}

					//第三注
					if (bIdx == 2)
					{
						BYTE bBackCT = m_GameLogic.GetCardType(m_bSegmentCard[wWinUser][2], 5, cbMaxCard);
						//同花顺
						if (bBackCT >= CT_FIVE_STRAIGHT_FLUSH)	nMultiple *= 5;	

						//铁支
						if (bBackCT == CT_FIVE_FOUR_ONE)				nMultiple *= 4;
					}

					m_lScoreTimes[i][bIdx]	+= nMultiple;
					m_lScoreTimes[j][bIdx]	-= nMultiple;
					m_lScoreTimesExEx[i][j][bIdx] = nMultiple;
					m_lScoreTimesExEx[j][i][bIdx] -= nMultiple;

					m_lScoreTimesEx[i][j] += nMultiple;
					m_lScoreTimesEx[j][i] -= nMultiple;

					cbTotalMultiple += nMultiple;
				}

				if((bWindInfo[0]==TRUE && bWindInfo[1]==TRUE && bWindInfo[2]==TRUE) || (bWindInfo[0]==FALSE && bWindInfo[1]==FALSE && bWindInfo[2]==FALSE))
				{
					m_bAllWin[i][j] = cbTotalMultiple;
					m_bAllWin[j][i] = -1*cbTotalMultiple;
				}
			}

		}
	}

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (m_bGameStatus[i]==FALSE) continue;
		for (int j = 0; j < GAME_PLAYER; j++)
		{
			if (j == i || m_bGameStatus[j] == FALSE) continue;

			m_lScoreTimesExEx[i][i][0] += m_lScoreTimesExEx[i][j][0];
			m_lScoreTimesExEx[i][i][1] += m_lScoreTimesExEx[i][j][1];
			m_lScoreTimesExEx[i][i][2] += m_lScoreTimesExEx[i][j][2];
		}
	}

}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserEnter(wChairID);

		//私人房设置游戏模式
		if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0)
		{		
			BYTE *pGameRule = m_pITableFrame->GetGameRule();

			m_cbPlayerCount = pGameRule[1];
			m_bHaveBanker	= pGameRule[2]!=0;

			if(0==pGameRule[4])
			{
				m_cbMaCard	= 0x00;
			}
			else if(3==pGameRule[4])
			{
				m_cbMaCard	= 0x35;
			}
			else if(2==pGameRule[4])
			{
				m_cbMaCard	= 0x3A;
			}
			else if(1==pGameRule[4])
			{
				m_cbMaCard	= 0x31;
			}

			m_cbTongHuaMode = pGameRule[5];
			m_GameLogic.SetTongHuaMode(m_cbTongHuaMode);

			m_cbQuickMode = pGameRule[6];

			if (pGameRule[7] == 1)//勾选动态加入
			{
				m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
			}
			else
			{
				m_pITableFrame->SetStartMode(START_MODE_FULL_READY);
			}

			m_nAllWinTimes=3;				
		}

		if(m_bHaveBanker)
		{
			for(int i=0;i<m_cbPlayerCount;i++)
			{
				if(m_wUserEnter[i] == INVALID_CHAIR)
				{
					m_wUserEnter[i]=wChairID;
					break;
				}
			}

			if(m_wBanker==INVALID_CHAIR && m_wUserEnter[0]!=INVALID_CHAIR)
			{
				m_wBanker=m_wUserEnter[0];
			}
		}

	}

	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	m_vecRecord[wChairID].clear();

	CString cc;
	cc.Format(_T("玩家坐下,chair=%d,userid=%d,name=%s"), wChairID,pIServerUserItem->GetUserID(),pIServerUserItem->GetNickName());
	WriteDebugInfo(cc);
	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(false == bLookonUser)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserLeave(wChairID);

		if(m_bHaveBanker)
		{
			for(int i=0;i<m_cbPlayerCount;i++)
			{
				if(m_wUserEnter[i] == wChairID)
				{
					//移动数组
					for(int j=i;j<m_cbPlayerCount-1;j++)
					{
						m_wUserEnter[j]=m_wUserEnter[j+1];
					}
					break;
				}
			}

			//切换庄家
			if(m_wBanker==wChairID)
			{
				m_wBanker=m_wUserEnter[0];
			}
		}
	}

	if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//房卡模式
	{
		if (m_pITableFrame->IsPersonalRoomDisumme())//当前朋友局解散清理记录
		{
			ZeroMemory(&m_stRecord, sizeof(m_stRecord));
		}
	}

	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
	m_vecRecord[wChairID].clear();
	CString cc;
	cc.Format(_T("玩家起立,chair=%d,userid=%d,name=%s"), wChairID, pIServerUserItem->GetUserID(), pIServerUserItem->GetNickName());
	WriteDebugInfo(cc);
	return true;
}


//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//数据验证 
	ASSERT(INVALID_CHAIR != wChairID);
	if (INVALID_CHAIR == wChairID) return false;
	CString cc;
	cc.Format(_T("玩家%d掉线"), wChairID);
	WriteDebugInfo(cc);
	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);
	return true;
}
//用户重入
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}

//游戏人数
DWORD CTableFrameSink::GetPlayerCount()
{
	//计算人数
	WORD wPlayerCount = 0;
	for(WORD i = 0; i < m_cbPlayerCount; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (m_bGameStatus[i] == false)continue;
		if (pIServerUserItem) wPlayerCount++;
	}

	return wPlayerCount;
}

//开始比牌
VOID CTableFrameSink::StartCompareCard()
{
	if (m_bTest || GetPlayerCount() == m_wShowCardCount)
	{
		m_pITableFrame->KillGameTimer(IDI_SHOW_CARD);
		CMD_S_CompareCard CompareCard;
		ZeroMemory (&CompareCard, sizeof(CompareCard));

		CompareCard.cbPlayCount = GetPlayerCount();
		CompareCard.wBankUser = m_wBanker;
		CopyMemory(CompareCard.bSegmentCard, m_bSegmentCard, sizeof(m_bSegmentCard));
		//是否特殊牌
		bool bHaveSpecial = false;

		for(WORD j=0 ; j<m_cbPlayerCount ; j++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(j);
			if ((pIServerUserItem == NULL) || (m_bGameStatus[j] == false)) continue;

			if (m_cbShowSpecialType[j] > CT_EX_INVALID)
			{
				m_bSpecialType[j] = m_cbShowSpecialType[j];
				bHaveSpecial=true;
			}
		}

		CompareCardGetTimes();
		CopyMemory(CompareCard.bAllWin,m_bAllWin,sizeof(m_bAllWin));

		CopyMemory(CompareCard.bSpecialType, m_bSpecialType, sizeof(m_bSpecialType));
		//CopyMemory(CompareCard.lScoreTimes, m_lScoreTimes, sizeof(m_lScoreTimes));

		m_wAllKillUser = INVALID_CHAIR;
		//全垒打
		if(bHaveSpecial==false && GetPlayerCount()==4 && m_bHaveBanker==false)
		{
			for(WORD i=0; i<m_cbPlayerCount;i++)
			{
				if (m_bGameStatus[i] == false) continue;
				//全垒打
				BYTE cbAllCount=0;
				for(WORD j=0;j<m_cbPlayerCount;j++)
				{
					if(i==j || m_bGameStatus[j] == false) continue;
					if(m_bAllWin[i][j]>0)
					{
						cbAllCount++;
					}
					else break;
				}
				if(cbAllCount+1==GetPlayerCount())
				{
					m_wAllKillUser = i;
					break;
				}
			}
		}
		CompareCard.wAllKillUser = m_wAllKillUser;

		//计算最终得分
		CalculateScore();
		for (int i = 0; i < GAME_PLAYER; i++)
			m_lscoreTimesPlus[i][3] = m_stGameEnd.lGameScore[i];
		CopyMemory(CompareCard.lScoreTimesPlus, m_lscoreTimesPlus, sizeof(m_lscoreTimesPlus));
		ZeroMemory(CompareCard.lScoreTimes, sizeof(CompareCard.lScoreTimes));
		CopyMemory(CompareCard.lScoreTimes, m_lScoreTimesExEx, sizeof(m_lScoreTimesExEx));
		if (m_bTest == false)
		{
			
			for (int x = 0; x < GAME_PLAYER; x++)
			{
				CString cc;
				if (m_bGameStatus[x]==FALSE) continue;
					cc.Format(_T("玩家%d比牌信息:\n"), x);
				for (int y = 0; y < GAME_PLAYER; y++)
				{
					if (y == x) continue;
					if (m_bGameStatus[y] == FALSE) continue;
					cc.AppendFormat(_T("与玩家%d:%I64d,%I64d,%I64d\n"), y, m_lScoreTimesExEx[x][y][0], m_lScoreTimesExEx[x][y][1], m_lScoreTimesExEx[x][y][2]);
				}
				cc.AppendFormat(_T("每道得分:%I64d,%I64d,%I64d,%I64d"), m_lscoreTimesPlus[x][0], m_lscoreTimesPlus[x][1], m_lscoreTimesPlus[x][2], m_lscoreTimesPlus[x][3]);
				WriteDebugInfo(cc);
			}

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
			m_bStartCompare = true;
			WriteDebugInfo(_T("开始比牌"));
			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_COMPARE_CARD, &CompareCard,sizeof(CompareCard));
			}

			m_pITableFrame->SetGameTimer(IDI_WAIT_COMAPRE, 15 * 1000, 1, 0);
		}

	}

	return;
}

//除重用户调试
void CTableFrameSink::TravelDebugList(ROOMUSERDEBUG keyroomuserdebug)
{
	//变量定义
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERDEBUG roomuserdebug;

	//初始化
	ptListHead = g_ListRoomUserDebug.GetHeadPosition();
	ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

	//遍历链表
	while (ptListHead)
	{
		ptTemp = ptListHead;
		roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

		//寻找已存在调试玩家在用一张桌子切换椅子
		if ((keyroomuserdebug.roomUserInfo.dwGameID == roomuserdebug.roomUserInfo.dwGameID) &&
			_tcscmp(keyroomuserdebug.roomUserInfo.szNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
		{
			g_ListRoomUserDebug.RemoveAt(ptTemp);
		}
	}
}


//是否满足调试条件
void CTableFrameSink::IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnableDebug)
{
	if (userInfo.wChairID == INVALID_CHAIR || userInfo.wTableID == INVALID_TABLE)
	{
		bEnableDebug = FALSE;
		return;
	}

	if (userInfo.cbUserStatus == US_SIT || userInfo.cbUserStatus == US_READY || userInfo.cbUserStatus == US_PLAYING)
	{
		bEnableDebug = TRUE;
		return;
	}
	else
	{
		bEnableDebug = FALSE;
		return;
	}
}

void CTableFrameSink::GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr)
{
	switch (debugType)
	{
	case CONTINUE_WIN:
	{
		debugTypestr = TEXT("调试类型为连赢");
		break;
	}
	case CONTINUE_LOST:
	{
		debugTypestr = TEXT("调试类型为连输");
		break;
	}
	case CONTINUE_CANCEL:
	{
		debugTypestr = TEXT("调试类型为取消");
		break;
	}
	}
}

bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	return true; 
}

int CTableFrameSink::CardTypeToScore(BYTE bCardType,BYTE bSpecies)
{
	//特殊牌型
	if(bSpecies == 0)
	{
		switch(bCardType)
		{
		case CT_EX_SANTONGHUA:			return 4;
		case CT_EX_SANSHUNZI:			return 4;
		case CT_EX_LIUDUIBAN:			return 4;
		case CT_EX_SITAOSANTIAO:		return 8;
		case CT_EX_SANFENGTIANXIA:		return 16;
		case CT_EX_SANTONGHUASHUN:		return 18;
		case CT_EX_YITIAOLONG:			return 26;
		case CT_EX_ZHIZUNQINGLONG:		return 108;
		}
		return 0;
	}
	//第一注
	if (bSpecies == 1)
	{	
		switch(bCardType)
		{
		case CT_THREE://三条
			{
				return 3;
			}
		}
		return 1;
	}
	//第二注
	if (bSpecies == 2)
	{	
		switch(bCardType)
		{
		case CT_FIVE_STRAIGHT_FLUSH_FIRST_A://A同花顺
		case CT_FIVE_STRAIGHT_FLUSH://同花顺牌:
			{
				return 10;
			}
		case CT_FIVE_FOUR_ONE://铁支                           
		case CT_FIVE_THREE_DEOUBLE://葫芦
			{
				return 8;
			}
		}
		return 1;
	}
	//第三注
	if (bSpecies == 3)
	{
		switch(bCardType)
		{
		case CT_FIVE_STRAIGHT_FLUSH_FIRST_A://A同花顺
		case CT_FIVE_STRAIGHT_FLUSH://同花顺牌:
			{
				return 5;
			}
		case CT_FIVE_FOUR_ONE://铁支                           
			{
				return 4;
			}
		}
		return 1;
	}
	return 0;

}

void CTableFrameSink::AnalyseCard()
{
	time_t aa = time(NULL);
	m_bTest = true;
	WORD m_wPlayerCount = m_cbPlayerCount;
	//机器人数
	bool bIsAiBanker = false;
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL)
		{
			if (m_bGameStatus[i] == FALSE)continue;
			if (pIServerUserItem->IsAndroidUser())
			{
				wAiCount++;
				if (!bIsAiBanker && i == m_wBanker)bIsAiBanker = true;
			}
			wPlayerCount++;
		}
	}

	//全部机器
	if (wPlayerCount == wAiCount || wAiCount == 0)
	{
		return;
	}

	//变量定义
	SCORE lAndroidScore = JudgeStock();
	LONGLONG lGameEndStorage = g_lRoomStorageCurrent+lAndroidScore;

	WORD wMaxUser = INVALID_CHAIR;
	WORD wMinAndroid = INVALID_CHAIR;
	WORD wMaxAndroid = INVALID_CHAIR;

	bool bAndroid[GAME_PLAYER];
	ZeroMemory(bAndroid, sizeof(bAndroid));
	SCORE lTempTestScore[GAME_PLAYER];
	ZeroMemory(lTempTestScore, sizeof(lTempTestScore));

	for (WORD i = 0; i < m_cbPlayerCount; i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->IsAndroidUser())
			bAndroid[i] = true;
		lTempTestScore[i] = m_lTestScore[i];
	}

	//查找特殊玩家
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		//真人玩家
		if (pIServerUserItem->IsAndroidUser() == false)
		{
			//初始设置
			if (wMaxUser == INVALID_CHAIR) wMaxUser = i;

			//获取较大者
			if (m_lTestScore[i]>m_lTestScore[wMaxUser])
			{
				wMaxUser = i;
			}
		}

		//机器玩家
		if (pIServerUserItem->IsAndroidUser() == true)
		{
			//初始设置
			if (wMinAndroid == INVALID_CHAIR) wMinAndroid = i;
			if (wMaxAndroid == INVALID_CHAIR) wMaxAndroid = i;

			//获取较小者
			if (m_lTestScore[i]<m_lTestScore[wMinAndroid])
			{
				wMinAndroid = i;
			}

			//获取较大者
			if (m_lTestScore[i]>m_lTestScore[wMaxAndroid])
			{
				wMaxAndroid = i;
			}
		}
	}

	//库存判断
	if (lGameEndStorage < 0 || (lGameEndStorage < (g_lRoomStorageCurrent * (double)(1 - (double)5 / (double)100))))
	{
		//机器坐庄
		if (bIsAiBanker)
		{
			//变量定义
			WORD wWinUser = wMaxUser;
			//查找数据
			for (WORD i = 0; i < m_wPlayerCount; i++)
			{
				//用户过滤
				if (m_bGameStatus[i] == FALSE) continue;

				//获取较大者
				if (m_lTestScore[i]>m_lTestScore[wWinUser])
				{
					wWinUser = i;
				}
			}

			//交换数据
			BYTE cbTempData[HAND_CARD_COUNT];
			CopyMemory(cbTempData, m_bUserCardData[m_wBanker], HAND_CARD_COUNT);
			CopyMemory(m_bUserCardData[m_wBanker], m_bUserCardData[wWinUser], HAND_CARD_COUNT);
			CopyMemory(m_bUserCardData[wWinUser], cbTempData, HAND_CARD_COUNT);
		}
		else
		{
			if (m_bHaveBanker)//真人庄家
			{
				//变量定义
				WORD wMinUser = wMinAndroid;
				//查找数据
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					//用户过滤
					if (m_bGameStatus[i] == FALSE) continue;

					//获取较大者
					if (m_lTestScore[i] < m_lTestScore[wMinUser])
					{
						wMinUser = i;
					}
				}

				//交换数据
				BYTE cbTempData[HAND_CARD_COUNT];
				CopyMemory(cbTempData, m_bUserCardData[m_wBanker], HAND_CARD_COUNT);
				CopyMemory(m_bUserCardData[m_wBanker], m_bUserCardData[wMinUser], HAND_CARD_COUNT);
				CopyMemory(m_bUserCardData[wMinUser], cbTempData, HAND_CARD_COUNT);
			}
			else//通比
			{
				SCORE lMinAndroidScore,lMaxPlayerScore = 0;
				WORD nMinAndroidIndex, nMaxPlayerIndex = 0;
				//最小换最大，保证不会出现负库存
				for (WORD i = 0; i < m_cbPlayerCount; i++)
				{
					if (bAndroid[i] == true && m_lTestScore[i] < 0 && m_lTestScore[i] < lMinAndroidScore)
					{
						lMinAndroidScore = m_lTestScore[i];
						nMinAndroidIndex = i;
					}
				}


				for (WORD j = 0; j < m_cbPlayerCount; j++)
				{
					if (bAndroid[j] == false && m_lTestScore[j]>0 && m_lTestScore[j] > lMaxPlayerScore)
					{
						lMaxPlayerScore = m_lTestScore[j];
						nMaxPlayerIndex = j;
					}
				}

				//交换数据
				BYTE cbTempData[HAND_CARD_COUNT];
				CopyMemory(cbTempData, m_bUserCardData[nMinAndroidIndex], HAND_CARD_COUNT);
				CopyMemory(m_bUserCardData[nMinAndroidIndex], m_bUserCardData[nMaxPlayerIndex], HAND_CARD_COUNT);
				CopyMemory(m_bUserCardData[nMaxPlayerIndex], cbTempData, HAND_CARD_COUNT);

				//交换分数
				SCORE lTempScore = m_lTestScore[nMinAndroidIndex];
				m_lTestScore[nMinAndroidIndex] = m_lTestScore[nMaxPlayerIndex];
				m_lTestScore[nMaxPlayerIndex] = lTempScore;

				//交换机器人标记
				bool bTempFlag = bAndroid[nMinAndroidIndex];
				bAndroid[nMinAndroidIndex] = bAndroid[nMaxPlayerIndex];
				bAndroid[nMaxPlayerIndex] = bTempFlag;

			}

		}
	}
	else if (g_lRoomStorageCurrent > 0  && g_lRoomStorageCurrent > g_lStorageMax2Room && g_lRoomStorageCurrent - lAndroidScore > 0 && rand() % 100 < g_lStorageMul2Room)
	{
		if (lTempTestScore[wMaxAndroid]>lTempTestScore[wMaxUser])
		{
			//交换数据
			BYTE cbTempData[HAND_CARD_COUNT];
			CopyMemory(cbTempData, m_bUserCardData[wMaxUser], HAND_CARD_COUNT);
			CopyMemory(m_bUserCardData[wMaxUser], m_bUserCardData[wMaxAndroid], HAND_CARD_COUNT);
			CopyMemory(m_bUserCardData[wMaxAndroid], cbTempData, HAND_CARD_COUNT);

			//库存不够换回来
			if (g_lRoomStorageCurrent+ JudgeStock() < 0)
			{
				CopyMemory(cbTempData, m_bUserCardData[wMaxUser], HAND_CARD_COUNT);
				CopyMemory(m_bUserCardData[wMaxUser], m_bUserCardData[wMaxAndroid], HAND_CARD_COUNT);
				CopyMemory(m_bUserCardData[wMaxAndroid], cbTempData, HAND_CARD_COUNT);
			}
		}
	}
	else if (g_lRoomStorageCurrent > 0 /*&& lAndroidScore>0*/ && g_lRoomStorageCurrent > g_lStorageMax1Room && g_lRoomStorageCurrent - lAndroidScore > 0 && rand() % 100 < g_lStorageMul1Room)
	{
		if (lTempTestScore[wMaxAndroid]>lTempTestScore[wMaxUser])
		{
			//交换数据
			BYTE cbTempData[HAND_CARD_COUNT];
			CopyMemory(cbTempData, m_bUserCardData[wMaxUser], HAND_CARD_COUNT);
			CopyMemory(m_bUserCardData[wMaxUser], m_bUserCardData[wMaxAndroid], HAND_CARD_COUNT);
			CopyMemory(m_bUserCardData[wMaxAndroid], cbTempData, HAND_CARD_COUNT);

			//库存不够换回来
			if (g_lRoomStorageCurrent+JudgeStock() < 0)
			{
				CopyMemory(cbTempData, m_bUserCardData[wMaxUser], HAND_CARD_COUNT);
				CopyMemory(m_bUserCardData[wMaxUser], m_bUserCardData[wMaxAndroid], HAND_CARD_COUNT);
				CopyMemory(m_bUserCardData[wMaxAndroid], cbTempData, HAND_CARD_COUNT);
			}
		}
	}

	ZeroMemory(m_lScoreTimes, sizeof(m_lScoreTimes));
	ZeroMemory(m_lScoreTimesEx, sizeof(m_lScoreTimesEx));
	ZeroMemory(m_lScoreTimesExEx, sizeof(m_lScoreTimesExEx));
	ZeroMemory(m_bSpecialType, sizeof(m_bSpecialType));
	ZeroMemory(m_bSegmentCard, sizeof(m_bSegmentCard));
	ZeroMemory(m_bFinishSegment, sizeof(m_bFinishSegment));
	ZeroMemory(m_cbShowSpecialType, sizeof(m_cbShowSpecialType));

	TCHAR szLog[MAX_PATH] = { 0 };
	_stprintf_s(szLog, _T("AI调试生效 消耗时间=%I64d"), time(NULL) - aa);
	WriteDebugInfo(szLog);
	return;
}


//读取配置
void CTableFrameSink::ReadConfigInformation()
{
	ASSERT(m_pGameCustomRule);
	m_bHaveBanker = m_pGameCustomRule->bHaveBanker;
	m_cbMaCard = m_pGameCustomRule->cbMaCard;
	m_nAllWinTimes = 3;
	m_cbTongHuaMode = m_pGameCustomRule->cbTongHuaMode;
	m_cbQuickMode = m_pGameCustomRule->cbQuickMode;
	m_GameLogic.SetTongHuaMode(m_cbTongHuaMode);
	g_lRoomStorageStart = m_pGameCustomRule->lRoomStorageStart;
	g_lRoomStorageCurrent = m_pGameCustomRule->lRoomStorageStart;
	g_lStorageDeductRoom = m_pGameCustomRule->lRoomStorageDeduct;
	g_lStorageMax1Room = m_pGameCustomRule->lRoomStorageMax1;
	g_lStorageMul1Room = m_pGameCustomRule->lRoomStorageMul1;
	g_lStorageMax2Room = m_pGameCustomRule->lRoomStorageMax2;
	g_lStorageMul2Room = m_pGameCustomRule->lRoomStorageMul2;

	if (g_lStorageDeductRoom < 0 || g_lStorageDeductRoom > 1000)
		g_lStorageDeductRoom = 0;

	if (g_lStorageMul1Room < 0 || g_lStorageMul1Room > 100)
		g_lStorageMul1Room = 50;
	if (g_lStorageMul2Room < 0 || g_lStorageMul2Room > 100)
		g_lStorageMul2Room = 80;
}


//更新房间用户信息
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction)
{
	//变量定义
	ROOMUSERINFO roomUserInfo;
	ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
	roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

	roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//用户坐下和重连
	if (userAction == USER_SITDOWN || userAction == USER_RECONNECT)
	{
		roomUserInfo.wChairID = pIServerUserItem->GetChairID();
		roomUserInfo.wTableID = pIServerUserItem->GetTableID() + 1;
	}
	else if (userAction == USER_STANDUP || userAction == USER_OFFLINE)
	{
		roomUserInfo.wChairID = INVALID_CHAIR;
		roomUserInfo.wTableID = INVALID_TABLE;
	}

	g_MapRoomUserInfo.SetAt(pIServerUserItem->GetUserID(), roomUserInfo);

	//遍历映射，删除离开房间的玩家，
	POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
	DWORD dwUserID = 0;
	ROOMUSERINFO userinfo;
	ZeroMemory(&userinfo, sizeof(userinfo));
	TCHAR szNickName[LEN_NICKNAME];
	ZeroMemory(szNickName, sizeof(szNickName));
	DWORD *pdwRemoveKey = new DWORD[g_MapRoomUserInfo.GetSize()];
	ZeroMemory(pdwRemoveKey, sizeof(DWORD) * g_MapRoomUserInfo.GetSize());
	WORD wRemoveKeyIndex = 0;

	while (ptHead)
	{
		g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

		if (userinfo.dwGameID == 0 && (_tcscmp(szNickName, userinfo.szNickName) == 0) && userinfo.cbUserStatus == 0)
		{
			pdwRemoveKey[wRemoveKeyIndex++] = dwUserID;
		}

	}

	for (WORD i = 0; i < wRemoveKeyIndex; i++)
	{
		g_MapRoomUserInfo.RemoveKey(pdwRemoveKey[i]);

		CString strtip;
		strtip.Format(TEXT("RemoveKey,wRemoveKeyIndex = %d, g_MapRoomUserInfosize = %d \n"), wRemoveKeyIndex, g_MapRoomUserInfo.GetSize());

		WriteDebugInfo(strtip);
	}

	delete[] pdwRemoveKey;

}

bool CTableFrameSink::AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList)
{
	//变量定义
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERDEBUG roomuserdebug;

	//遍历链表
	for (WORD i = 0; i < m_cbPlayerCount; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}

		//初始化
		ptListHead = g_ListRoomUserDebug.GetHeadPosition();
		ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

		//遍历链表
		while (ptListHead)
		{
			ptTemp = ptListHead;
			roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

			//寻找玩家
			if ((pIServerUserItem->GetGameID() == roomuserdebug.roomUserInfo.dwGameID) &&
				_tcscmp(pIServerUserItem->GetNickName(), roomuserdebug.roomUserInfo.szNickName) == 0)
			{
				//清空调试局数为0的元素
				if (roomuserdebug.userDebug.cbDebugCount == 0)
				{
					g_ListRoomUserDebug.RemoveAt(ptTemp);
					break;
				}

				if (roomuserdebug.userDebug.debug_type == CONTINUE_CANCEL)
				{
					g_ListRoomUserDebug.RemoveAt(ptTemp);
					break;
				}

				//拷贝数据
				CopyMemory(&Keyroomuserdebug, &roomuserdebug, sizeof(roomuserdebug));
				ptList = ptTemp;
				Keyroomuserdebug.roomUserInfo.wChairID = i;
				return true;
			}

		}

	}

	return false;
}

void CTableFrameSink::WriteDebugInfo(LPCTSTR pszString)
{
	//CString strtip;
	//strtip.Format(TEXT("房间号%d--%s"), m_pITableFrame->GetTableID(), pszString);
	//m_pITableFrame->SendGameLogData(strtip);

	//设置语言区域
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	//创建目录
	TCHAR szFilePack[MAX_PATH];
	GetModuleFileName(NULL, szFilePack, MAX_PATH);
	(wcsrchr(szFilePack, '\\'))[0] = 0;
	_stprintf(szFilePack, _T("%s\\十三水日志目录"), szFilePack);
	DWORD dwAttrib = GetFileAttributes(szFilePack);
	if (INVALID_FILE_ATTRIBUTES == dwAttrib)
	{
		CreateDirectory(szFilePack, NULL);
	}

	//创建文件
	CTime time = CTime::GetCurrentTime();
	static CString strFile;
	if (strFile.IsEmpty())
		strFile.Format(_T("%s//[%s]日志%d-%d-%d-%02d%02d%02d.log"), szFilePack, m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	//写文件
	CStdioFile myFile;
	BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		if (myFile.GetLength() > 50 * 1024 * 1024)//50M分文件 
		{
			myFile.Close();
			strFile.Format(_T("%s//[%s]日志%d-%d-%d-%02d%02d%02d.log"), szFilePack, m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
			if (bOpen == false)
				return;
		}

		CString strtip;
		strtip.Format(TEXT("房间号%d,%d/%d/%d_%d:%d:%d--%s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pszString);

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

