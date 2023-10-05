#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"

//房间玩家信息
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo;	//玩家USERID映射玩家信息
//房间用户调试
CList<ROOMUSERDEBUG, ROOMUSERDEBUG &> g_ListRoomUserDebug;		//房间用户调试链表
//操作调试记录
CList<CString, CString &> g_ListOperationRecord;						//操作调试记录

ROOMUSERINFO	g_CurrentQueryUserInfo;								//当前查询用户信息

//全局变量
LONGLONG						g_lRoomStorageStart = 0LL;								//房间起始库存
LONGLONG						g_lRoomStorageCurrent = 0LL;							//总输赢分
LONGLONG						g_lStorageDeductRoom = 0LL;								//回扣变量
LONGLONG						g_lStorageMax1Room = 0LL;								//库存封顶
LONGLONG						g_lStorageMul1Room = 0LL;								//系统输钱比例
LONGLONG						g_lStorageMax2Room = 0LL;								//库存封顶
LONGLONG						g_lStorageMul2Room = 0LL;								//系统输钱比例


//定时器 0~30
#define IDI_ADD_SCORE						3									//下注定时器
#define TIME_ADD_SCORE						30									//下注时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
    //游戏变量
    m_wPlayerCount = GAME_PLAYER;
    m_wBankerUser = INVALID_CHAIR;
    m_wCurrentUser = INVALID_CHAIR;
	m_wCurrentCompareUser = INVALID_CHAIR;
    m_lCompareScore = 0;
	m_bShowHandByPoor = false;
	m_bShowHand = false;
	m_lMaxCellScore = 0L;
	m_lCellScore = 0L;
	m_lCurrentTimes = 0;
	m_lPersonalCellScore = 0;
	m_lPersonalInitScore = 0;
	m_cbMaxTurnCount = 0;
	m_cbCurTurnCount = 0;
	m_cbMinCompareCardTurnCount = 0;
	m_cbMinLookCardTurnCount = 0;
	m_cbAiCount = 0;
	m_cbPlayingCount = 0;

	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));
	ZeroMemory(m_bMingZhu, sizeof(m_bMingZhu));
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
	ZeroMemory(m_lUserMaxScore, sizeof(m_lUserMaxScore));
	ZeroMemory(m_lNewTurnScore, sizeof(m_lNewTurnScore));
	ZeroMemory(m_lReturnScore, sizeof(m_lReturnScore));
	ZeroMemory(m_nButtonValue, sizeof(m_nButtonValue));
	ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
	ZeroMemory(m_lostScoreInfo, sizeof(m_lostScoreInfo));
	ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
    for(int i = 0; i < GAME_PLAYER; i++)
    {
        m_wCompardUser[i].RemoveAll();
    }
	
    //组件变量
    m_pITableFrame = NULL;
    m_pGameServiceOption = NULL;

    //清空链表
    g_ListRoomUserDebug.RemoveAll();
    g_ListOperationRecord.RemoveAll();
	
    //服务调试
    m_hDebugInst = NULL;
    m_pServerDebug = NULL;
    m_hDebugInst = LoadLibrary(TEXT("ThreeMadnessServerDebug.dll"));
    if(m_hDebugInst)
    {
        typedef void *(*CREATE)();
        CREATE ServerDebug = (CREATE)GetProcAddress(m_hDebugInst, "CreateServerDebug");
        if(ServerDebug)
        {
            m_pServerDebug = static_cast<IServerDebug *>(ServerDebug());
        }
    }

    //游戏视频
    m_hVideoInst = NULL;
    m_pGameVideo = NULL;

    return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
    if(m_pServerDebug)
    {
        delete m_pServerDebug;
        m_pServerDebug = NULL;
    }

    if(m_hDebugInst)
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

//释放对象
VOID CTableFrameSink::Release()
{
    if(m_pServerDebug)
    {
        delete m_pServerDebug;
        m_pServerDebug = NULL;
    }

    if(m_hDebugInst)
    {
        FreeLibrary(m_hDebugInst);
        m_hDebugInst = NULL;
    }

    delete this;
}

//接口查询--检测相关信息版本
void   *CTableFrameSink::QueryInterface(const IID &Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
    QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
    return NULL;
}

//初始化
bool  CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
    //查询接口
    ASSERT(pIUnknownEx != NULL);
    m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
    if(m_pITableFrame == NULL)
    {
        return false;
    }

    //开始模式
    m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

    //获取参数
    m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
    ASSERT(m_pGameServiceOption != NULL);

    //读取配置
    ReadConfigInformation();

	//房卡初始化视频组件
	if(IsRoomCard())
	{
		m_hVideoInst = LoadLibrary(TEXT("ThreeMadnessGameVideo.dll"));
		if(m_hVideoInst)
		{
			typedef void *(*CREATE)();
			CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
			if(GameVideo)
			{
				m_pGameVideo = static_cast<IGameVideo *>(GameVideo());
			}
		}

	}

    return true;
}

//复位桌子
void  CTableFrameSink::RepositionSink()
{
    //游戏变量
    m_wCurrentUser = INVALID_CHAIR;
    m_lCompareScore = 0;
	m_lMaxCellScore = 0L;
	m_lCellScore = 0L;
	m_lCurrentTimes = 0L;
	m_wCurrentCompareUser = INVALID_CHAIR;
	m_cbCurTurnCount = 0;
	m_bShowHand = false;
	m_bShowHandByPoor = false;
	m_cbAiCount = 0;
	m_cbPlayingCount = 0;

    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_bMingZhu, sizeof(m_bMingZhu));
    ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
    ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));
    ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
    ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
    ZeroMemory(m_lUserMaxScore, sizeof(m_lUserMaxScore));
    ZeroMemory(m_lostScoreInfo, sizeof(m_lostScoreInfo));
	ZeroMemory(m_lReturnScore, sizeof(m_lReturnScore));
	ZeroMemory(m_lNewTurnScore, sizeof(m_lNewTurnScore));
	ZeroMemory(m_lostScoreInfo, sizeof(m_lostScoreInfo));
	for (int i = 0; i < m_wPlayerCount; i++)
	{
		m_wCompardUser[i].RemoveAll();
	}

	m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
    return;
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    return false;
}


//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
{
    //更新房间用户信息
    UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);

	CString cc;
	cc.Format(_T("玩家%d掉线,userid=%d"), wChairID, pIServerUserItem->GetChairID());
	WriteInfo(cc);
    return true;
}

//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{
	WriteInfo(_T("游戏开始"));

    //设置状态
    m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);

	//随机扑克
	BYTE cbTempCard[GAME_PLAYER][MAX_COUNT] = { 0 };
	m_GameLogic.RandCardList(cbTempCard[0],GAME_PLAYER*MAX_COUNT);


    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //获取用户
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)
            continue;

        m_cbPlayStatus[i] = TRUE;
		CopyMemory(m_cbHandCardData[i], cbTempCard[i], MAX_COUNT);

		if(IsRoomCardScoreType())//约战积分场
			m_lUserMaxScore[i]=m_lPersonalInitScore;
        else
            m_lUserMaxScore[i] = pIServerUserItem->GetUserScore();

        m_cbPlayingCount++;
        if(pIServerUserItem->IsAndroidUser())
        {
            m_cbAiCount++;
            m_cbAndroidStatus[i] = TRUE;
        }
        else
        {
            m_cbRealPlayer[i] = TRUE;
        }
    }

	//更新库存
	if (g_lRoomStorageCurrent > 0 && m_cbAiCount != m_cbPlayingCount && m_cbAiCount>0)
    {
        g_lRoomStorageCurrent = g_lRoomStorageCurrent - g_lRoomStorageCurrent * g_lStorageDeductRoom / 1000;
    }
    
    //底注
	if (IsRoomCardScoreType())//积分场
	{
		m_lCellScore = m_lPersonalCellScore;
		m_lMaxCellScore = m_nButtonValue[3];
		m_lCurrentTimes = m_lCellScore;
	}
	else
	{
		m_lCellScore = m_pITableFrame->GetCellScore();
		m_lMaxCellScore = m_lCellScore * 10;
		m_lCurrentTimes = 1;
	}


    //变量定义
    ROOMUSERDEBUG roomuserdebug;
    ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
    POSITION posKeyList;

    //检查调试信息
	if (m_pServerDebug != NULL && AnalyseRoomUserDebug(roomuserdebug, posKeyList))
	{
		//校验数据
		ASSERT(roomuserdebug.roomUserInfo.wChairID != INVALID_CHAIR && roomuserdebug.userDebug.cbDebugCount != 0
			&& roomuserdebug.userDebug.debug_type != CONTINUE_CANCEL);

		if (m_pServerDebug->DebugResult(m_cbHandCardData, roomuserdebug))
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

			CString cc;
			cc.Format(_T("调试玩家%d成功，类型%d,剩余局数%d,gameid=%d,name=%s"),roomuserdebug.roomUserInfo.wChairID, UserDebugComplete.debugType, UserDebugComplete.cbRemainDebugCount,
				UserDebugComplete.dwGameID, UserDebugComplete.szNickName);
			WriteInfo(cc);
		}
	}
	else
	{
		AnalyseStartCard();
	}

	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	CString strSaveFileName;
	strSaveFileName.Format(TEXT("%s\\ConfigCard\\GameCardData\\ThreeMadness.card"), szPath);
	CFile file;
	if (file.Open(strSaveFileName, CFile::modeRead))
	{
		ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
		file.Read(m_cbHandCardData, 15);
		file.Close();
	}

    //AI消息
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbAndroidStatus[i] == TRUE)
        {
            //AI数据
            CMD_S_AndroidCard AndroidCard ;
            ZeroMemory(&AndroidCard, sizeof(AndroidCard)) ;

            //设置变量
            AndroidCard.lStockScore = g_lRoomStorageCurrent;
            CopyMemory(AndroidCard.cbRealPlayer, m_cbRealPlayer, sizeof(m_cbRealPlayer));
            CopyMemory(AndroidCard.cbAndroidStatus, m_cbAndroidStatus, sizeof(m_cbAndroidStatus));
            CopyMemory(AndroidCard.cbAllHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
            m_pITableFrame->SendTableData(i, SUB_S_ANDROID_CARD, &AndroidCard, sizeof(AndroidCard));
        }
    }

     //确定庄家
    if(m_wBankerUser == INVALID_CHAIR)
    {
        m_wBankerUser = rand() % m_wPlayerCount;
    }

    while(m_cbPlayStatus[m_wBankerUser] == FALSE)
    {
        m_wBankerUser = (m_wBankerUser + 1) % m_wPlayerCount;
    }

    //下底注
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            m_lTableScore[i] = m_lCellScore;

			BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
			if(IsRoomCardScoreType())
			{
				m_RoomCardRecord.cbCardTypeCount[i][cbCardType-1]++;//牌型统计
			}
			CString cc;
			cc.Format(_T("玩家%d手牌=%0x,%0x,%0x,type=%d"), i, m_cbHandCardData[i][0], m_cbHandCardData[i][1], m_cbHandCardData[i][2], cbCardType);
			WriteInfo(cc);
        }
    }

    //当前用户
    m_wCurrentUser = (m_wBankerUser + 1) % m_wPlayerCount;
    while(m_cbPlayStatus[m_wCurrentUser] == FALSE)
    {
        m_wCurrentUser = (m_wCurrentUser + 1) % m_wPlayerCount;
    }

    //构造数据
    CMD_S_GameStart GameStart;
    ZeroMemory(&GameStart, sizeof(GameStart));
    GameStart.lMaxScore = m_lMaxCellScore;
    GameStart.lCellScore = m_lCellScore;
    GameStart.lCurrentTimes = m_lCurrentTimes;
    GameStart.wCurrentUser = m_wCurrentUser;
    GameStart.wBankerUser = m_wBankerUser;
    CopyMemory(GameStart.cbPlayStatus, m_cbPlayStatus, sizeof(BYTE)*GAME_PLAYER);

    if(m_pGameVideo)
    {
        m_pGameVideo->StartVideo(m_pITableFrame);
    }

    //发送数据
	bool bFirstRecord=true;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            GameStart.lUserMaxScore = m_lUserMaxScore[i];
            m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
			m_pITableFrame->SendLookonData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

            if(m_pGameVideo)
            {
				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);

                Video_GameStart video;
                ZeroMemory(&video, sizeof(video));
                CopyMemory(video.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
                video.wPlayerCount = m_cbPlayingCount;
                video.wGamePlayerCountRule = m_wPlayerCount;
                video.lMaxScore = m_lMaxCellScore;
                video.lCellScore = m_lCellScore;
                video.lCurrentTimes = m_lCurrentTimes;
                video.lUserMaxScore = GameStart.lUserMaxScore;
                video.wBankerUser = m_wBankerUser;
                video.wCurrentUser = m_wCurrentUser;
                CopyMemory(video.cbPlayStatus, m_cbPlayStatus, sizeof(m_cbPlayStatus));
                CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
                video.wChairID = i;
                video.lScore = m_lUserMaxScore[i];

                m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video,sizeof(video),bFirstRecord);
                bFirstRecord = false;
            }
        }
        
    }

    //服务调试
    if(m_pServerDebug)
    {
        //服务信息
        BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT];
        CopyMemory(cbHandCardData, m_cbHandCardData, sizeof(cbHandCardData));

        //信息结算
        m_pServerDebug->ServerDebug(cbHandCardData, m_pITableFrame, m_wPlayerCount);
    }

    //设置定时器
	m_tActionStartTime = time(0);
    m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, (TIME_ADD_SCORE +TIME_DISPATCH_CARD)* 1000, 1, 0);

    return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    switch(cbReason)
    {
    case GER_COMPARECARD:	//比牌结束
    case GER_NO_PLAYER:		//没有玩家
    {
		WriteInfo(_T("游戏结算:"));
        //定义变量
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd, sizeof(GameEnd));
		memset(GameEnd.wCompareUser,INVALID_CHAIR,sizeof(GameEnd.wCompareUser));
		GameEnd.wServerType = m_pGameServiceOption->wServerType;

        //唯一玩家
        WORD wWinner, wWinnerCount = 0;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE)
            {
                wWinnerCount++;
                wWinner = i;
            }
        }
		
		if(wWinnerCount==1)//赢家当庄
			m_wBankerUser=wWinner;

        //计算总注
        LONGLONG lAllTableScore = 0L;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            lAllTableScore += m_lTableScore[i];
        }

		if(wWinnerCount==1)//只有一个赢家
		{
			for(WORD i = 0; i < m_wPlayerCount; i++)
			{
				if(i == wWinner)
					continue;

				GameEnd.lGameScore[i] = -m_lTableScore[i];
			}

			GameEnd.lGameScore[wWinner] = lAllTableScore - m_lTableScore[wWinner];
			//处理税收
			if (GameEnd.lGameScore[wWinner] > 0 && IsRoomCardScoreType()==false)
			{
				GameEnd.lGameTax = m_pITableFrame->CalculateRevenue(wWinner, GameEnd.lGameScore[wWinner]);
				GameEnd.lGameScore[wWinner] -= GameEnd.lGameTax;
			}

		}
		else if(wWinnerCount>1)
		{
			ASSERT(wWinnerCount>=3);//至少有三家才可能多人赢
			for(WORD i = 0; i < m_wPlayerCount; i++)
			{
				if(m_cbPlayStatus[i]==TRUE)
				{
					lAllTableScore -= m_lTableScore[i];//赢家各自取回自己下的注
				}
			}
			//平分剩下分
			ASSERT(lAllTableScore>=0);
			for(WORD i = 0; i < m_wPlayerCount; i++)
			{
				if(m_cbPlayStatus[i]==TRUE)
				{
					GameEnd.lGameScore[i] = lAllTableScore/wWinnerCount;
					
					if (GameEnd.lGameScore[i] > 0)
					{
						GameEnd.lGameTax = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
						GameEnd.lGameScore[i] -= GameEnd.lGameTax;
					}
				}
				else
					GameEnd.lGameScore[i] = -m_lTableScore[i];
			}
		}

		 //扑克数据
        CopyMemory(GameEnd.cbCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
			if (m_lTableScore[i] > 0)
				GameEnd.cbCardType[i] = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
            WORD wCount = 0;
            while(m_wCompardUser[i].GetCount() > 0)
            {
                GameEnd.wCompareUser[i][wCount++] = m_wCompardUser[i].GetAt(0);
                m_wCompardUser[i].RemoveAt(0);
            }
        }

        GameEnd.wEndState = cbReason;

        //修改积分
        tagScoreInfo ScoreInfo[GAME_PLAYER];
        ZeroMemory(ScoreInfo, sizeof(ScoreInfo));

        if(wWinnerCount >= 1)
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE)
                    continue;

                ScoreInfo[i].cbType = (GameEnd.lGameScore[i] >0) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
                ScoreInfo[i].lRevenue = GameEnd.lGameTax;
                ScoreInfo[i].lScore = GameEnd.lGameScore[i];

            }
        }

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
			//房卡模式
			if(IsRoomCardScoreType() && m_RoomCardRecord.nCount < MAX_RECORD_COUNT)
			{
				m_RoomCardRecord.lDetailScore[i][m_RoomCardRecord.nCount] = GameEnd.lGameScore[i];
				m_RoomCardRecord.lAllScore[i] += GameEnd.lGameScore[i];
			}

			if (IsRoomCardTreasureType())
			{
				IServerUserItem *pUser = m_pITableFrame->GetTableUserItem(i);
				if (pUser && GameEnd.lGameScore[i]!=0)
				{
					m_vecRecord[i].push_back(GameEnd.lGameScore[i]);
					if (m_vecRecord[i].size()>30)
						m_vecRecord[i].erase(m_vecRecord[i].begin());
				}

			}

        }


		//约战记录
		if (IsRoomCard())
		{
			if (IsRoomCardScoreType() && m_RoomCardRecord.nCount < MAX_RECORD_COUNT)
			{
				m_RoomCardRecord.nCount++;
			}

			CMD_S_RoomCardRecord RoomCardRecord;
			ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

			if (m_pITableFrame->GetDataBaseMode() == 0)
			{
				CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			}
			else
			{
				for (int j = 0; j < m_wPlayerCount; j++)
				{
					RoomCardRecord.nCount = m_vecRecord[j].size();
					if (RoomCardRecord.nCount == 0) continue;
					for (int i = 0; i < RoomCardRecord.nCount; i++)
					{
						RoomCardRecord.lDetailScore[j][i] = m_vecRecord[j][i];
					}
					m_pITableFrame->SendTableData(j, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
				}
			}

		}      

        //发送信息
        m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd,sizeof(GameEnd));
            m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
        }

        //写分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
			if (m_pITableFrame->GetTableUserItem(i) == NULL) continue;

            if(m_cbPlayStatus[i] == TRUE)
            {
                m_pITableFrame->WriteUserScore(i, ScoreInfo[i]);

				if (m_cbRealPlayer[i])
				{
					UpdateStorage(-ScoreInfo[i].lScore);
				}

				CString cc;
				cc.Format(_T("玩家%d结算写分%I64d"), i, ScoreInfo[i].lScore);
				WriteInfo(cc);
            }
			else if (m_lostScoreInfo[i].cbType != SCORE_TYPE_NULL)
			{
				m_pITableFrame->WriteUserScore(i, m_lostScoreInfo[i]);

				if (m_cbRealPlayer[i])
				{
					UpdateStorage(-m_lostScoreInfo[i].lScore);
				}

				CString cc;
				cc.Format(_T("玩家%d结算写分%I64d"), i, m_lostScoreInfo[i].lScore);
				WriteInfo(cc);
			}


        }

        //结束游戏
        m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
        m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);


        return true;
    }
    case GER_USER_LEAVE:		//用户强退
    case GER_NETWORK_ERROR:		//网络中断
    {
        if(m_cbPlayStatus[wChairID] == FALSE)
        {
            return true;
        }

        //效验参数
        ASSERT(pIServerUserItem != NULL);
        ASSERT(wChairID < m_wPlayerCount);

        //强退处理
        return OnUserGiveUp(wChairID);
    }
    case GER_DISMISS:		//游戏解散
    {
        //变量定义
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd, sizeof(GameEnd));

        //发送信息
        m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));


        //结束游戏
        m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
        m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);

        return true;
    }
    }

    return false;
}

//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
    switch(cbGameStatus)
    {
    case GAME_STATUS_FREE:		//空闲状态
    {
        //构造数据
        CMD_S_StatusFree StatusFree;
        ZeroMemory(&StatusFree, sizeof(StatusFree));

        //设置变量
        StatusFree.lCellScore = m_lCellScore;
        StatusFree.lRoomStorageStart = g_lRoomStorageStart;
        StatusFree.lRoomStorageCurrent = g_lRoomStorageCurrent;

		StatusFree.lInitScore = m_lPersonalInitScore;
		StatusFree.cbMinCompareCardTurnCount=m_cbMinCompareCardTurnCount;
		StatusFree.cbMinLookCardTurnCount=m_cbMinLookCardTurnCount;
		StatusFree.cbMaxTurnCount = m_cbMaxTurnCount;
		CopyMemory(StatusFree.nButtonValue, m_nButtonValue, sizeof(m_nButtonValue));
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
        CopyMemory(&StatusFree.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
		StatusFree.wGamePlayerCountRule = m_wPlayerCount;
		StatusFree.cbMinAllinTurnCount = m_cbMinAllinTurnCount;

        CopyMemory(StatusFree.szServerName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szServerName));
		StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();

        //发送场景
        m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));

		if (IsRoomCard())
		{
			CMD_S_RoomCardRecord RoomCardRecord;
			ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));
			if (IsRoomCardScoreType())
				CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
			else
			{
				RoomCardRecord.nCount = m_vecRecord[wChairID].size();
				for (int i = 0; i < RoomCardRecord.nCount; i++)
				{
					RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
					RoomCardRecord.lAllScore[wChairID] += m_vecRecord[wChairID][i];
				}
			}
			m_pITableFrame->SendTableData(wChairID, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
		}


		return true;
    }
    case GAME_STATUS_PLAY:	//游戏状态
    {
        //构造数据
        CMD_S_StatusPlay StatusPlay;
        memset(&StatusPlay, 0, sizeof(StatusPlay));

        //加注信息
        StatusPlay.lMaxCellScore = m_lMaxCellScore;
        StatusPlay.lCellScore = m_lCellScore;
        StatusPlay.lCurrentTimes = m_lCurrentTimes;
        StatusPlay.lUserMaxScore = m_lUserMaxScore[wChairID];

		StatusPlay.lInitScore = m_lPersonalInitScore;
		StatusPlay.cbMinCompareCardTurnCount=m_cbMinCompareCardTurnCount;
		StatusPlay.cbMinLookCardTurnCount=m_cbMinLookCardTurnCount;
		StatusPlay.cbMaxTurnCount = m_cbMaxTurnCount;
		StatusPlay.cbCurTurnCount=m_cbCurTurnCount;
		StatusPlay.bShowHand=m_bShowHand;
		CopyMemory(StatusPlay.nButtonValue, m_nButtonValue, sizeof(m_nButtonValue));
        //设置变量
        StatusPlay.wBankerUser = m_wBankerUser;
        StatusPlay.wCurrentUser = m_wCurrentUser;
        CopyMemory(StatusPlay.bMingZhu, m_bMingZhu, sizeof(m_bMingZhu));
        CopyMemory(StatusPlay.lTableScore, m_lTableScore, sizeof(m_lTableScore));
        CopyMemory(StatusPlay.cbPlayStatus, m_cbPlayStatus, sizeof(m_cbPlayStatus));
        StatusPlay.lRoomStorageStart = g_lRoomStorageStart;
        StatusPlay.lRoomStorageCurrent = g_lRoomStorageCurrent;

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
        CopyMemory(&StatusPlay.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
		StatusPlay.wGamePlayerCountRule = m_wPlayerCount;
		StatusPlay.cbMinAllinTurnCount = m_cbMinAllinTurnCount;
        //当前状态
        StatusPlay.bCompareState = (m_lCompareScore <= 0) ? false : true;

        //设置扑克
        if(m_bMingZhu[wChairID])
        {
            CopyMemory(&StatusPlay.cbHandCardData, &m_cbHandCardData[wChairID], MAX_COUNT);
			StatusPlay.cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT);
        }

		time_t curTime = time(0);
		time_t tRes = m_tActionStartTime + TIME_ADD_SCORE - curTime;
		if (tRes > 0)
			StatusPlay.cbRemainingTime = (BYTE)tRes;

        //更新房间用户信息
        UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

        CopyMemory(StatusPlay.szServerName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szServerName));
		StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();

        //发送场景
        m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));

		if (IsRoomCard())
		{
			CMD_S_RoomCardRecord RoomCardRecord;
			ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));
			if (IsRoomCardScoreType())
				CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
			else
			{
				RoomCardRecord.nCount = m_vecRecord[wChairID].size();
				for (int i = 0; i < RoomCardRecord.nCount; i++)
				{
					RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
					RoomCardRecord.lAllScore[wChairID] += m_vecRecord[wChairID][i];
				}
			}
			m_pITableFrame->SendTableData(wChairID, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
		}


		return true;
    }
    }

    //效验错误
    ASSERT(FALSE);

    return false;
}

//定时器事件
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
    if(wTimerID == IDI_ADD_SCORE)
    {
        m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
		if (IsRoomCardScoreType()) return true;
        if(m_wCurrentUser != INVALID_CHAIR)
        {
			CString cc;
			cc.Format(_T("当前玩家%d超时"), m_wCurrentUser);
			WriteInfo(cc);

           return OnUserGiveUp(m_wCurrentUser);
        }
    }

    return false;
}

//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    switch(wSubCmdID)
    {
    case SUB_C_GIVE_UP:			//用户放弃
    {
        //用户效验
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            CString str;
            str.Format(_T("error,UserStatus is not US_PLAYING , CAN NOT GIVE UP, wchair=%d"),pUserData->wChairID);
			WriteInfo(str);
            return true;
        }

        //状态判断
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE)
        {
            CString str;
            str.Format(_T("error,m_cbPlayStatus  IS  FALSE , CAN NOT GIVE UP,wchair=%d"),pUserData->wChairID);
			WriteInfo(str);
            return false;
        }

        //消息处理
        return OnUserGiveUp(pUserData->wChairID);
    }
    case SUB_C_LOOK_CARD:		//用户看牌
    {
        //用户效验
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            CString str;
            str.Format(_T("error,UserStatus is not US_PLAYING , CAN NOT LOOK CARD,wchair=%d"),pUserData->wChairID);
            WriteInfo(str);
            return true;
        }

        //状态判断
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE)
        {
            CString str;
            str.Format(_T("error,m_cbPlayStatus  IS  FALSE , CAN NOT LOOK CARD,wchair=%d"),pUserData->wChairID);
            WriteInfo(str);
            return true;
        }

        //消息处理
        return OnUserLookCard(pUserData->wChairID);
    }
    case SUB_C_COMPARE_CARD:	//用户比牌
    {
		tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();

        //效验数据
        ASSERT(wDataSize == sizeof(CMD_C_CompareCard));
        if(wDataSize != sizeof(CMD_C_CompareCard))
        {
			CString str;
			str.Format(_T("error,COMPARE error ,wchair=%d"), pUserData->wChairID);
			WriteInfo(str);
            return false;
        }

        //变量定义
        CMD_C_CompareCard *pCompareCard = (CMD_C_CompareCard *)pData;

        //用户效验
        if(pUserData->cbUserStatus != US_PLAYING)
        {
			CString str;
			str.Format(_T("error,UserStatus is not US_PLAYING , CAN NOT COMPARE CARD,wchair=%d"), pUserData->wChairID);
			WriteInfo(str);
            return true;
        }

        //参数效验
        ASSERT(pUserData->wChairID == m_wCurrentUser);
        if(pUserData->wChairID != m_wCurrentUser)
        {
            CString str;
            str.Format(_T("error,Not m_wCurrentUser , CAN NOT COMPARE CARD,wchair=%d"),pUserData->wChairID);
			WriteInfo(str);
            return false;
        }

        //状态判断
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE && m_cbPlayStatus[pCompareCard->wCompareUser] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE || m_cbPlayStatus[pCompareCard->wCompareUser] == FALSE)
        {
            CString str;
            str.Format(_T("error,UserStatus is FALSE , CAN NOT COMPARE CARD,wchair=%d,wchair2=%d"),pUserData->wChairID,pCompareCard->wCompareUser);
			WriteInfo(str);
            return false;
        }

        //消息处理
        return OnUserCompareCard(pUserData->wChairID, pCompareCard->wCompareUser);
    }
    case SUB_C_ADD_SCORE:		//用户加注
    {
		tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        //效验数据
        ASSERT(wDataSize == sizeof(CMD_C_AddScore));
        if(wDataSize != sizeof(CMD_C_AddScore))
        {
			CString str;
			str.Format(_T("error,ADDSCORE error ,wchair=%d"), pUserData->wChairID);
			WriteInfo(str);
            return false;
        }



        //用户效验
        if(pUserData->cbUserStatus != US_PLAYING)
        {
			CString str;
			str.Format(_T("error,UserStatus is not US_PLAYING , CAN NOT ADD SCORE,wchair=%d"), pUserData->wChairID);
			WriteInfo(str);
            return true;
        }

		//变量定义
		CMD_C_AddScore *pAddScore = (CMD_C_AddScore *)pData;

        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE && m_lCompareScore == 0 && m_wCurrentCompareUser==INVALID_CHAIR);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE || m_lCompareScore > 0 || m_wCurrentCompareUser != INVALID_CHAIR)
        {
            CString str;
            str.Format(_T("error,UserStatus is FALSE , CAN NOT ADD_SCORE,wchair=%d"),pUserData->wChairID);
            WriteInfo(str);
            return false;
        }

        //当前状态
        if(pAddScore->wState > 0)
        {
			m_wCurrentCompareUser = pUserData->wChairID;
            m_lCompareScore = pAddScore->lScore;
        }

        //消息处理
        return OnUserAddScore(pUserData->wChairID, pAddScore->lScore, false, ((pAddScore->wState > 0) ? true : false));
    }
    case SUB_C_WAIT_COMPARE:	//等待比牌
    {
        //用户效验
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            return true;
        }

        //状态判断
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE)
        {
            CString str;
            str.Format(L"UserStatus is FALSE , CAN NOT WAIT_COMPARE.");
            WriteInfo(str);
            return false;
        }

        //参数效验
        ASSERT(pUserData->wChairID == m_wCurrentUser);
        if(pUserData->wChairID != m_wCurrentUser)
        {
            CString str;
            str.Format(L"Not m_wCurrentUser , CAN NOT WAIT_COMPARE.");
            WriteInfo(str);
            return false;
        }

        //消息处理
        CMD_S_WaitCompare WaitCompare;
        WaitCompare.wCompareUser = pUserData->wChairID;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_WAIT_COMPARE, &WaitCompare, sizeof(WaitCompare));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_WAIT_COMPARE, &WaitCompare, sizeof(WaitCompare));

        return true;
    }
    case SUB_C_FINISH_FLASH:	//完成动画
    {
        return true;
    }
	case SUB_C_REQUEST_RCRecord:
	{
		ASSERT(IsRoomCard() && (!m_pITableFrame->IsPersonalRoomDisumme()));
		if (IsRoomCard()==false || m_pITableFrame->IsPersonalRoomDisumme())
		{
			return true;
		}
		WORD wChairID = pIServerUserItem->GetChairID();
		CMD_S_RoomCardRecord RoomCardRecord;
		ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));
		if (IsRoomCardScoreType())
			CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
		else
		{
			RoomCardRecord.nCount = m_vecRecord[wChairID].size();
			for (int i = 0; i < RoomCardRecord.nCount; i++)
			{
				RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
				RoomCardRecord.lAllScore[wChairID] += m_vecRecord[wChairID][i];
			}
		}
		m_pITableFrame->SendTableData(wChairID, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));

		return true;
	}

    }

    return false;
}

//框架消息处理
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    // 消息处理
    if(wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
    {
        switch(wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
        {
        case SUB_C_STORAGE:
        {
            ASSERT(wDataSize == sizeof(CMD_C_UpdateStorage));
            if(wDataSize != sizeof(CMD_C_UpdateStorage))
            {
                return false;
            }

            //权限判断
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
            {
                return false;
            }

            CMD_C_UpdateStorage *pUpdateStorage = (CMD_C_UpdateStorage *)pData;
            g_lRoomStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
            g_lStorageDeductRoom = pUpdateStorage->lRoomStorageDeduct;

            //20条操作记录
            if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
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
            WriteInfo(strOperationRecord);

            //变量定义
            CMD_S_Operation_Record OperationRecord;
            ZeroMemory(&OperationRecord, sizeof(OperationRecord));
            POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
            WORD wIndex = 0;//数组下标
            while(posListRecord)
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
            if(wDataSize != sizeof(CMD_C_ModifyStorage))
            {
                return false;
            }

            //权限判断
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
            {
                return false;
            }

            CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pData;
            g_lStorageMax1Room = pModifyStorage->lMaxRoomStorage[0];
            g_lStorageMax2Room = pModifyStorage->lMaxRoomStorage[1];
            g_lStorageMul1Room = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
            g_lStorageMul2Room = (SCORE)(pModifyStorage->wRoomStorageMul[1]);

            //20条操作记录
            if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
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

            WriteInfo(strOperationRecord);

            //变量定义
            CMD_S_Operation_Record OperationRecord;
            ZeroMemory(&OperationRecord, sizeof(OperationRecord));
            POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
            WORD wIndex = 0;//数组下标
            while(posListRecord)
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
            if(wDataSize != sizeof(CMD_C_RequestQuery_User))
            {
                return false;
            }

            //权限判断
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
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

            while(ptHead)
            {
                g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
                if(pQuery_User->dwGameID == userinfo.dwGameID || _tcscmp(pQuery_User->szNickName, userinfo.szNickName) == 0)
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
            if(wDataSize != sizeof(CMD_C_UserDebug))
            {
                return false;
            }

            //权限判断
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
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
            if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
            {
                g_ListOperationRecord.RemoveHead();
            }

            //变量定义
            CMD_S_UserDebug serverUserDebug;
            ZeroMemory(&serverUserDebug, sizeof(serverUserDebug));

            TCHAR szNickName[LEN_NICKNAME];
            ZeroMemory(szNickName, sizeof(szNickName));

            //激活调试
            if(pUserDebug->userDebugInfo.bCancelDebug == false)
            {
                ASSERT(pUserDebug->userDebugInfo.debug_type == CONTINUE_WIN || pUserDebug->userDebugInfo.debug_type == CONTINUE_LOST);

                while(ptMapHead)
                {
                    g_MapRoomUserInfo.GetNextAssoc(ptMapHead, dwUserID, userinfo);

                    if(_tcscmp(pUserDebug->szNickName, szNickName) == 0 && _tcscmp(userinfo.szNickName, szNickName) == 0)
                    {
                        continue;
                    }

                    if(pUserDebug->dwGameID == userinfo.dwGameID || _tcscmp(pUserDebug->szNickName, userinfo.szNickName) == 0)
                    {
                        //激活调试标识
                        bool bEnableDebug = false;
                        IsSatisfyDebug(userinfo, bEnableDebug);

                        //满足调试
                        if(bEnableDebug)
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
                            WriteInfo(strOperationRecord);
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

                            WriteInfo(strOperationRecord);
                        }

                        //发送数据
                        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

                        CMD_S_Operation_Record OperationRecord;
                        ZeroMemory(&OperationRecord, sizeof(OperationRecord));
                        POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
                        WORD wIndex = 0;//数组下标
                        while(posListRecord)
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
                while(ptListHead)
                {
                    ptTemp = ptListHead;
                    roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);
                    if(pUserDebug->dwGameID == roomuserdebug.roomUserInfo.dwGameID || _tcscmp(pUserDebug->szNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
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

                        WriteInfo(strOperationRecord);

                        CMD_S_Operation_Record OperationRecord;
                        ZeroMemory(&OperationRecord, sizeof(OperationRecord));
                        POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
                        WORD wIndex = 0;//数组下标
                        while(posListRecord)
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

                WriteInfo(strOperationRecord);

                CMD_S_Operation_Record OperationRecord;
                ZeroMemory(&OperationRecord, sizeof(OperationRecord));
                POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
                WORD wIndex = 0;//数组下标
                while(posListRecord)
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
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
            {
                return false;
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

            //发送数据
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));


            return true;
        }
        case SUB_C_CLEAR_CURRENT_QUERYUSER:
        {
            //权限判断
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
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



//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //更新房间用户信息
    UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	//私人房设置游戏模式
	if (IsRoomCard())
	{
		BYTE *pSetInfo = m_pITableFrame->GetGameRule();
		if (IsRoomCardScoreType())//积分场
		{
			//m_wPlayerCount = pSetInfo[1];//玩家人数0表示2-5人，2345表示对应人数
			m_wPlayerCount = pSetInfo[2];//最大椅子数

			m_lPersonalInitScore = pSetInfo[3] * 100;//初始分
			m_lPersonalCellScore = pSetInfo[4];//配置底分
			m_lCellScore = m_lPersonalCellScore;
			m_cbMinCompareCardTurnCount = pSetInfo[5];//比牌轮数
			m_cbMinLookCardTurnCount = pSetInfo[6];//看牌轮数
			m_cbMaxTurnCount = pSetInfo[7];//最大下注轮数

			m_pITableFrame->SetTableChairCount(m_wPlayerCount);
			m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
		}
		else if (IsRoomCardTreasureType())//金币场
		{
			m_wPlayerCount = GAME_PLAYER;
			m_pITableFrame->SetTableChairCount(m_wPlayerCount);
			m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
			m_lCellScore = m_pITableFrame->GetCellScore();//底分
			m_cbMinCompareCardTurnCount = pSetInfo[5];//比牌轮数
			m_cbMinLookCardTurnCount = pSetInfo[6];//看牌轮数
			m_cbMaxTurnCount = pSetInfo[7];//最大下注轮数
			//m_cbMinAllinTurnCount = pSetInfo[8];//最小全压轮数
			m_vecRecord[wChairID].clear();
		}

	}

	ConfigAddButtonValue();

	CString cc;
	cc.Format(_T("玩家%d坐下，userid=%d,score=%I64d,AI=%d,name=%s"), wChairID, pIServerUserItem->GetUserID(), pIServerUserItem->GetUserScore(),pIServerUserItem->IsAndroidUser(), pIServerUserItem->GetNickName());
	WriteInfo(cc);
	
    return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //更新房间用户信息
    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

    //约战解散
	if(IsRoomCard() && m_pITableFrame->IsPersonalRoomDisumme())
	{
		ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
	}

	if (IsRoomCardTreasureType())
		m_vecRecord[wChairID].clear();

	CString cc;
	cc.Format(_T("玩家%d起立，userid=%d,score=%I64d,name=%s"), wChairID, pIServerUserItem->GetUserID(), pIServerUserItem->GetUserScore(), pIServerUserItem->GetNickName());
	WriteInfo(cc);
    return true;
}

//用户同意
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize)
{
	CString cc;
	cc.Format(_T("玩家%d准备，userid=%d,score=%I64d,name=%s"), wChairID, pIServerUserItem->GetUserID(), pIServerUserItem->GetUserScore(), pIServerUserItem->GetNickName());
	WriteInfo(cc);
    return true;
}

//放弃事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID)
{
	CString cc;
	cc.Format(_T("玩家%d弃牌"), wChairID);
	WriteInfo(cc);
    //设置数据
    m_cbPlayStatus[wChairID] = FALSE;
	if (m_wCurrentCompareUser == wChairID)
	{
		m_wCurrentCompareUser = INVALID_CHAIR;
		m_lCompareScore = 0;
	}

    //发送消息
    CMD_S_GiveUp GiveUp;
    GiveUp.wGiveUpUser = wChairID;
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GIVE_UP, &GiveUp, sizeof(GiveUp));
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GIVE_UP, &GiveUp, sizeof(GiveUp));


	if (wChairID == m_wCurrentUser)
	{
		m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
	}

	//弃牌时没看牌，下发自己的牌
	if (m_bMingZhu[wChairID] == false)
	{
		CMD_S_LookCard LookCard;
		LookCard.wLookCardUser = wChairID;
		CopyMemory(LookCard.cbCardData, m_cbHandCardData[wChairID], MAX_COUNT);
		LookCard.cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[wChairID], MAX_COUNT);
		m_pITableFrame->SendTableData(wChairID, SUB_S_LOOK_CARD, &LookCard, sizeof(LookCard));
	}

	IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(wChairID);

	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_GIVE_UP, &GiveUp, sizeof(GiveUp));
		if (IsRoomCardScoreType() == false)//非积分场
			m_pITableFrame->WriteTableVideoPlayer(pUserItem->GetUserID(), m_pGameVideo->GetVideoNumber());
	}

    //修改积分
    tagScoreInfo ScoreInfo;
    ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
    ScoreInfo.cbType = SCORE_TYPE_LOSE;
    ScoreInfo.lRevenue = 0;
    ScoreInfo.lScore = -m_lTableScore[wChairID];
	if (IsRoomCardScoreType() == false)//非积分场
	{
		m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);
		cc.Format(_T("玩家%d弃牌写分%I64d"), wChairID, ScoreInfo.lScore);
		WriteInfo(cc);
		ScoreInfo.cbType = SCORE_TYPE_NULL;
	}

	CopyMemory(&m_lostScoreInfo[wChairID], &ScoreInfo, sizeof(ScoreInfo));

	if (pUserItem)
	{
		if (pUserItem->GetUserStatus() == US_PLAYING)
			pUserItem->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), pUserItem->GetChairID());
	}
	if (m_cbRealPlayer[wChairID])
		UpdateStorage(m_lTableScore[wChairID]);

    //房卡模式
    if(IsRoomCardScoreType() && m_RoomCardRecord.nCount < MAX_RECORD_COUNT)
    {
        m_RoomCardRecord.lDetailScore[wChairID][m_RoomCardRecord.nCount] = ScoreInfo.lScore;
    }

    //人数统计
    WORD wPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            wPlayerCount++;
        }
    }

    //判断结束
    if(wPlayerCount >= 2)
    {
        if(m_wCurrentUser == wChairID)
        {
            OnUserAddScore(wChairID, 0L, true, false);
        }
    }
    else
    {
        OnEventGameConclude(INVALID_CHAIR, NULL, GER_NO_PLAYER);
    }

    return true;
}

//看牌事件
bool CTableFrameSink::OnUserLookCard(WORD wChairID)
{
	CString cc;
	cc.Format(_T("玩家%d看牌"), wChairID);
	WriteInfo(cc);

    //参数效验
    ASSERT(!m_bMingZhu[wChairID]);
    if(m_bMingZhu[wChairID])
    {
		WriteInfo(_T("OnUserLookCard error1"));
        return true;
    }

    //设置参数
    m_bMingZhu[wChairID] = true;
    //构造数据
    CMD_S_LookCard LookCard;
    ZeroMemory(&LookCard, sizeof(LookCard));
    LookCard.wLookCardUser = wChairID;

    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_LOOK_CARD, &LookCard, sizeof(LookCard));

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)
        {
            continue;
        }
        if(i == wChairID)
        {
            CopyMemory(LookCard.cbCardData, m_cbHandCardData[wChairID], sizeof(m_cbHandCardData[0]));
			LookCard.cbCardType = m_GameLogic.GetCardType(LookCard.cbCardData, MAX_COUNT);
        }
        else
        {
			LookCard.cbCardType = 0;
            ZeroMemory(LookCard.cbCardData, sizeof(LookCard.cbCardData));
        }
        m_pITableFrame->SendTableData(i, SUB_S_LOOK_CARD, &LookCard, sizeof(LookCard));
    }

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_LOOK_CARD, &LookCard,sizeof(LookCard));
    }

    return true;
}

//比牌事件
bool CTableFrameSink::OnUserCompareCard(WORD wFirstChairID, WORD wNextChairID)
{
	CString cc;
	cc.Format(_T("当前比牌玩家%d,选择的两个玩家%d,%d,comparecount=%I64d"), m_wCurrentCompareUser, wFirstChairID, wNextChairID, m_lCompareScore);
	WriteInfo(cc);
    //效验参数
    ASSERT(m_lCompareScore > 0 && m_wCurrentCompareUser==wFirstChairID);
    if(m_lCompareScore <=0 || m_wCurrentCompareUser != wFirstChairID)
    {

		WriteInfo(_T("cmp,error0"));
        return false;
    }

    ASSERT(m_wCurrentUser == m_wBankerUser || m_lTableScore[m_wCurrentUser] - m_lCompareScore >= m_lCellScore);
    if(!(m_wCurrentUser == m_wBankerUser || m_lTableScore[m_wCurrentUser] - m_lCompareScore >= m_lCellScore))
    {
		WriteInfo(_T("cmp,error1"));
        return false;
    }

	ASSERT(wFirstChairID >= 0 && wFirstChairID < GAME_PLAYER && m_cbPlayStatus[wFirstChairID] == TRUE);
	if (wFirstChairID < 0 || wFirstChairID >= GAME_PLAYER || m_cbPlayStatus[wFirstChairID] == FALSE)
	{
		WriteInfo(_T("cmp,error3"));
		return false;
	}

	ASSERT(wNextChairID >= 0 && wNextChairID < GAME_PLAYER && m_cbPlayStatus[wNextChairID] == TRUE);
	if (wNextChairID < 0 || wNextChairID >= GAME_PLAYER || m_cbPlayStatus[wNextChairID] == FALSE)
	{
		WriteInfo(_T("cmp,error4"));
		return false;
	}
    //删除定时器
    m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);

    //比较大小
    BYTE cbWin = m_GameLogic.CompareCard(m_cbHandCardData[wFirstChairID], m_cbHandCardData[wNextChairID], MAX_COUNT);

    //状态设置
    m_lCompareScore = 0;
	m_wCurrentCompareUser = INVALID_CHAIR;
    //胜利用户
    WORD wLostUser, wWinUser;
    if(cbWin == TRUE)
    {
        wWinUser = wFirstChairID;
        wLostUser = wNextChairID;
    }
    else
    {
        wWinUser = wNextChairID;
        wLostUser = wFirstChairID;
    }

    //设置数据
    m_wCompardUser[wLostUser].Add(wWinUser);
    m_wCompardUser[wWinUser].Add(wLostUser);
    m_cbPlayStatus[wLostUser] = FALSE;

    //人数统计
    WORD wPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            wPlayerCount++;
        }
    }

    //继续游戏
	bool bShowHand = false;
    if(wPlayerCount >= 2)
    {
        //用户切换
        WORD wNextPlayer = INVALID_CHAIR;
        for(WORD i = 1; i < m_wPlayerCount; i++)
        {
            //设置变量
            wNextPlayer = (m_wCurrentUser + i) % m_wPlayerCount;

			if(wNextPlayer==(m_wBankerUser+1)%m_wPlayerCount)
			{
				m_cbCurTurnCount++;
				CMD_S_TurnCount TurnCount;
				TurnCount.cbTurnCount=m_cbCurTurnCount;
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TURN_COUNT,&TurnCount,sizeof(TurnCount));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TURN_COUNT,&TurnCount,sizeof(TurnCount));
			}

            //继续判断
            if(m_cbPlayStatus[wNextPlayer] == TRUE)
            {
                break;
            }
        }

        //设置用户
        m_wCurrentUser = wNextPlayer;
		LONGLONG lMinScore = m_lCurrentTimes*m_lCellScore*(m_bMingZhu[m_wCurrentUser] ? 2 : 1);
		if ((m_lUserMaxScore[m_wCurrentUser] - m_lTableScore[m_wCurrentUser] < lMinScore))
		{
			bShowHand = true;//当前玩家分不够了，只能梭哈
		}

    }
    else
    {
        m_wCurrentUser = INVALID_CHAIR;
    }

    //构造数据
    CMD_S_CompareCard CompareCard;
	ZeroMemory(&CompareCard, sizeof(CompareCard));
    CompareCard.wCurrentUser = m_wCurrentUser;
    CompareCard.wLostUser = wLostUser;
    CompareCard.wCompareUser[0] = wFirstChairID;
    CompareCard.wCompareUser[1] = wNextChairID;
	CompareCard.bShowHand = bShowHand;
    //发送数据
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_COMPARE_CARD, &CompareCard, sizeof(CompareCard));

	if (m_wCurrentUser != INVALID_CHAIR)
	{
		m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, TIME_ADD_SCORE * 1000, 1, 0);
		m_tActionStartTime = time(0);
	}

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_COMPARE_CARD, &CompareCard,sizeof(CompareCard));
		m_pITableFrame->WriteTableVideoPlayer(m_pITableFrame->GetTableUserItem(wLostUser)->GetUserID(), m_pGameVideo->GetVideoNumber());
    }

    //修改积分
    tagScoreInfo ScoreInfo;
    ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
    ScoreInfo.cbType = SCORE_TYPE_LOSE;
    ScoreInfo.lRevenue = 0;
    ScoreInfo.lScore = -m_lTableScore[wLostUser];
	m_pITableFrame->WriteUserScore(wLostUser, ScoreInfo);

	ScoreInfo.cbType = SCORE_TYPE_NULL;
    CopyMemory(&m_lostScoreInfo[wLostUser], &ScoreInfo, sizeof(ScoreInfo));
	cc.Format(_T("玩家%d比牌输写分%I64d"), wLostUser, ScoreInfo.lScore);
	WriteInfo(cc);

	if (m_cbRealPlayer[wLostUser])
		UpdateStorage(m_lTableScore[wLostUser]);

    //结束游戏
    if(wPlayerCount < 2)
    {
        m_wBankerUser = wWinUser;
        OnEventGameConclude(INVALID_CHAIR, NULL, GER_COMPARECARD);
    }

    return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore, bool bGiveUp, bool bCompareCard)
{
	CString cc;
	cc.Format(_T("玩家%d下注%I64d,弃牌%d,比牌%d,当前time=%I64d,看牌%d"), wChairID, lScore, bGiveUp, bCompareCard, m_lCurrentTimes, m_bMingZhu[wChairID]);
	WriteInfo(cc);
    if(bGiveUp == false)				//设置数据
    {
        //状态效验
        ASSERT(m_wCurrentUser == wChairID);
        if(m_wCurrentUser != wChairID)
        {
            CString strdebug;
            strdebug.Format(TEXT("error1,当前玩家=%d,android=%d\n"),m_wCurrentUser, m_cbAndroidStatus[wChairID]);
			WriteInfo(strdebug);

            if(m_cbAndroidStatus[wChairID]==TRUE)
                return true;
            else
                return false;
        }
		//下注合理判断
		ASSERT((lScore + m_lTableScore[wChairID]) <= m_lUserMaxScore[wChairID]);
		if ((lScore + m_lTableScore[wChairID]) > m_lUserMaxScore[wChairID])
		{
			CString strdebug;
			strdebug.Format(TEXT("error2 下注超过最大分 已下注%I64d 最大下注%I64d,android=%d"), m_lTableScore[wChairID], m_lUserMaxScore[wChairID],m_cbAndroidStatus[wChairID]);
            WriteInfo(strdebug);

			if (m_cbAndroidStatus[wChairID] == TRUE)
				return true;
			else
				return false;
		}

        //梭哈判断
		bool bCurUserShowHand = false;
		if((lScore + m_lTableScore[wChairID])==m_lUserMaxScore[wChairID])
		{
			m_lTableScore[wChairID] += lScore;

			bCurUserShowHand = true;
			m_bShowHand = true;
			m_lReturnScore[wChairID]=lScore;//记录梭哈前剩下

			//是否分数不够导致的全压
			if (m_bShowHandByPoor == false)
			{
				LONGLONG lTempCellScore = m_lCellScore;
				if (IsRoomCardScoreType())
					lTempCellScore = 1;

				LONGLONG lMinScore = m_lCurrentTimes*lTempCellScore*(m_bMingZhu[m_wCurrentUser] ? 2 : 1);
				if (lScore < lMinScore)
				{
					m_bShowHandByPoor = true;
					WriteInfo(_T("分数不够"));
				}
			}


			CString cc;
			cc.Format(_T("玩家%d梭哈"), wChairID);
			WriteInfo(cc);
		}	

		if (bCurUserShowHand == false)
		{
			//当前倍数
			LONGLONG lTempCellScore=m_lCellScore;
			if(IsRoomCardScoreType())
				lTempCellScore=1;

			LONGLONG lTimes = m_bMingZhu[wChairID] ? 2 : 1;
			if (bCompareCard)
				lTimes *= 2;

			LONGLONG lTemp = lScore /lTempCellScore/ lTimes;
			ASSERT(m_lCurrentTimes <= lTemp && lTemp <= (m_lMaxCellScore/lTempCellScore));
			if (m_lCurrentTimes > lTemp || (lTemp > (m_lMaxCellScore/lTempCellScore)))
			{
				CString strdebug;
				strdebug.Format(TEXT("error2 倍数错误 curTime=%I64d,time=%I64d,android=%d\n"),m_lCurrentTimes,lTemp, m_cbAndroidStatus[wChairID]);
                WriteInfo(strdebug);

				if (m_cbAndroidStatus[wChairID] == TRUE)
					return true;
				else
					return false;
			}

			m_lCurrentTimes = lTemp;
			m_lTableScore[wChairID] += lScore;
		}

		m_lNewTurnScore[wChairID] = lScore;
        
    }

	bool bOneTurnFinish = false;
    //设置用户
    if(!bCompareCard)
    {
        //用户切换
        WORD wNextPlayer = INVALID_CHAIR;
        for(WORD i = 1; i < m_wPlayerCount; i++)
        {
             wNextPlayer = (m_wCurrentUser + i) % m_wPlayerCount;
			//轮数判断
			if(wNextPlayer==(m_wBankerUser+1)%m_wPlayerCount)
			{
				m_cbCurTurnCount++;
				cc.Format(_T("第%d轮了"), m_cbCurTurnCount);
				WriteInfo(cc);

				bOneTurnFinish = true;
				if (m_bShowHandByPoor==false)
					ZeroMemory(&m_lNewTurnScore, sizeof(m_lNewTurnScore));

				if (m_cbMaxTurnCount==0 || m_cbCurTurnCount < m_cbMaxTurnCount)
				{
					CMD_S_TurnCount TurnCount;
					TurnCount.cbTurnCount = m_cbCurTurnCount;
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TURN_COUNT, &TurnCount, sizeof(TurnCount));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TURN_COUNT, &TurnCount, sizeof(TurnCount));
				}

			}

            if(m_cbPlayStatus[wNextPlayer] == TRUE)
            {
                break;
            }
        }
		ASSERT(wNextPlayer != INVALID_CHAIR);
        m_wCurrentUser = wNextPlayer;
    }


    //构造数据
    CMD_S_AddScore AddScore;
	ZeroMemory(&AddScore, sizeof(AddScore));
    AddScore.lCurrentTimes = m_lCurrentTimes;
	AddScore.wAddScoreUser = wChairID;
    AddScore.lAddScoreCount = lScore;
    AddScore.wCompareState = (bCompareCard) ? TRUE : FALSE;
	
	if (bOneTurnFinish && m_bShowHandByPoor)
	{
		m_wCurrentUser = INVALID_CHAIR;
		WriteInfo(_T("分数不够梭哈结束"));

		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			if (m_cbPlayStatus[i] == TRUE )
			{
				cc.Format(_T("玩家%dNewScore=%I64d,returnscore=%I64d"), i, m_lNewTurnScore[i], m_lReturnScore[i]);
				WriteInfo(cc);
				if (m_lNewTurnScore[i]>0 && m_lReturnScore[i] == 0)
				m_lReturnScore[i] = m_lNewTurnScore[i];
			}
		}
	}
	else if(m_bShowHand && m_lTableScore[m_wCurrentUser]==m_lUserMaxScore[m_wCurrentUser])
	{
		m_wCurrentUser=INVALID_CHAIR;
		WriteInfo(_T("全部梭哈结束"));
	}
	else if (IsRoomCard() && m_cbCurTurnCount >= m_cbMaxTurnCount)
	{
		m_wCurrentUser = INVALID_CHAIR;
		CString cc;
		cc.Format(_T("约占场当前轮数%d达到最大轮数%d结束"), m_cbCurTurnCount, m_cbMaxTurnCount);
		WriteInfo(cc);
	}

	AddScore.wCurrentUser = m_wCurrentUser;
	if (m_wCurrentUser != INVALID_CHAIR)
	{
		if (m_bShowHand)//有人全压了，只能全压
		{
			AddScore.bShowHand = true;
		}
		else//分不够只能全压
		{
			LONGLONG lTempCellScore = m_lCellScore;
			if (IsRoomCardScoreType())
				lTempCellScore = 1;

			LONGLONG lMinScore = m_lCurrentTimes*lTempCellScore*(m_bMingZhu[m_wCurrentUser] ? 2 : 1);
			if (m_lUserMaxScore[m_wCurrentUser] - m_lTableScore[m_wCurrentUser] < lMinScore)
			{
				AddScore.bShowHand = true;
			}
		}
	}

    //发送数据
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_ADD_SCORE, &AddScore,sizeof(AddScore));
    }

    if(m_wCurrentUser != INVALID_CHAIR)//设置定时器
    {
        m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, TIME_ADD_SCORE * 1000, 1, 0);
		m_tActionStartTime = time(0);
    }
	else//开牌
	{
		//添加比牌标记
		for(int i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE)
				continue;
			for(int j=0;j<m_wPlayerCount;j++)
			{
				if(m_cbPlayStatus[j]==FALSE || j==i)
					continue;
				m_wCompardUser[i].Add(j);
			}
		}

		//找出最终赢家，一样大比花色
		bool bBaoZi=false;
		bool bSpecial=false;
		bool bOther=false;

		for(int i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE)
				continue;
			BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[i],MAX_COUNT);
			if(cbCardType == CT_BAO_ZI)
				bBaoZi=true;
			else if(cbCardType == CT_SPECIAL)
				bSpecial=true;
			else
				bOther=true;
		}
		if(!bBaoZi || !bSpecial || !bOther)//没有特殊情况
		{
			//先计算返还分
			LONGLONG lAnScore=0;
			LONGLONG lMingScore=0;

			int	nMinChairID=0;
			for(int i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)
					continue;

				LONGLONG lTempScore=m_lReturnScore[i];
				if(m_bMingZhu[i])
					lTempScore = lTempScore/2;
				if(lAnScore==0 || lTempScore<lAnScore)
				{
					lAnScore=lTempScore;
					nMinChairID=i;
				}
			}

			lMingScore=2*lAnScore;
			cc.Format(_T("梭哈最小暗注%I64d,明注%I64d,最小玩家%d"), lAnScore, lMingScore, nMinChairID);
			WriteInfo(cc);

			for(int i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)	continue;
				if(i==nMinChairID)
					m_lReturnScore[i]=0;
				else
					m_lReturnScore[i]=m_lReturnScore[i]-(m_bMingZhu[i]?lMingScore:lAnScore);

				ASSERT(m_lReturnScore[i]>= 0 && m_lTableScore[i]>m_lReturnScore[i]);
				if (m_lReturnScore[i] < 0 || m_lTableScore[i] <= m_lReturnScore[i])
				{
					cc.Format(_T("error,返还出错"));
					WriteInfo(cc);
				}
				m_lTableScore[i] -= m_lReturnScore[i];
				cc.Format(_T("返还玩家%d分数%I64d,实际下注%I64d"), i, m_lReturnScore[i], m_lTableScore[i]);
				WriteInfo(cc);
			}

			int nWinner=INVALID_CHAIR;
			for(int i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)
					continue;
				if(nWinner==INVALID_CHAIR)
					nWinner=i;
				else
				{
					if(m_GameLogic.CompareCard(m_cbHandCardData[i],m_cbHandCardData[nWinner],MAX_COUNT,TRUE)==TRUE)
					{
						m_cbPlayStatus[nWinner]=FALSE;

						tagScoreInfo ScoreInfo;
						ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
						ScoreInfo.cbType = SCORE_TYPE_LOSE;
						ScoreInfo.lRevenue = 0;
						ScoreInfo.lScore = -m_lTableScore[nWinner];
						CopyMemory(&m_lostScoreInfo[nWinner], &ScoreInfo, sizeof(ScoreInfo));

						nWinner=i;
					}
					else
					{
						m_cbPlayStatus[i]=FALSE;

						tagScoreInfo ScoreInfo;
						ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
						ScoreInfo.cbType = SCORE_TYPE_LOSE;
						ScoreInfo.lRevenue = 0;
						ScoreInfo.lScore = -m_lTableScore[i];
						CopyMemory(&m_lostScoreInfo[i], &ScoreInfo, sizeof(ScoreInfo));
					}
				}
			}			
		}
		
		OnEventGameConclude(INVALID_CHAIR,NULL,GER_COMPARECARD);
	}


    return true;
}

//扑克分析
void CTableFrameSink::AnalyseStartCard()
{
    if(m_cbPlayingCount == m_cbAiCount || m_cbAiCount == 0)
    {
        return;
    }

    //扑克变量
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT] = {0};
    CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

    //排列扑克
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        m_GameLogic.SortCardList(cbUserCardData[i], MAX_COUNT);
    }

    //变量定义
    WORD wWinUser = INVALID_CHAIR;

    //查找数据
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //用户过滤
        if(m_cbPlayStatus[i] == FALSE)
            continue;

        //设置用户
        if(wWinUser == INVALID_CHAIR)
        {
            wWinUser = i;
            continue;
        }

        //对比扑克
        if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinUser], MAX_COUNT) >= TRUE)
        {
            wWinUser = i;
        }
    }

    //库存判断
    if(g_lRoomStorageCurrent < 0)
    {
		if (m_cbAndroidStatus[wWinUser] == TRUE)
			return;

        //随机赢家
        WORD wWinAndroid = INVALID_CHAIR;

        do
        {
            wWinAndroid = rand() % m_wPlayerCount;
        }
        while(m_cbAndroidStatus[wWinAndroid]==FALSE);

        if(wWinAndroid != wWinUser)
        {
            //交换数据
            BYTE cbTempData[MAX_COUNT];
            CopyMemory(cbTempData, m_cbHandCardData[wWinAndroid], sizeof(BYTE)*MAX_COUNT);
            CopyMemory(m_cbHandCardData[wWinAndroid], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
            CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
			CString cc;
			cc.Format(_T("debuginfo1,AiChair=%d,userChair=%d"), wWinAndroid, wWinUser);
			WriteInfo(cc);
        }
    }
    //调试输分
	else if (g_lRoomStorageCurrent >= g_lStorageMax1Room && g_lRoomStorageCurrent<g_lStorageMax2Room)
	{
		if (rand() % 100 <= g_lStorageMul1Room)//玩家赢
		{
			if (m_cbRealPlayer[wWinUser] == TRUE)
				return;

			//随机赢家
			WORD wWinPlayer = INVALID_CHAIR;

			do
			{
				wWinPlayer = rand() % m_wPlayerCount;
			} while (m_cbRealPlayer[wWinPlayer] == FALSE);

			if (wWinPlayer != wWinUser)
			{
				//交换数据
				BYTE cbTempData[MAX_COUNT];
				CopyMemory(cbTempData, m_cbHandCardData[wWinPlayer], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinPlayer], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
				CString cc;
				cc.Format(_T("debuginfo2,AiChair=%d,userChair=%d"), wWinPlayer, wWinUser);
				WriteInfo(cc);
			}
		}
		else//玩家输
		{
			if (m_cbAndroidStatus[wWinUser] == TRUE)
				return;

			//随机赢家
			WORD wWinAndroid = INVALID_CHAIR;

			do
			{
				wWinAndroid = rand() % m_wPlayerCount;
			} while (m_cbAndroidStatus[wWinAndroid] == FALSE);

			if (wWinAndroid != wWinUser)
			{
				//交换数据
				BYTE cbTempData[MAX_COUNT];
				CopyMemory(cbTempData, m_cbHandCardData[wWinAndroid], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinAndroid], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
				CString cc;
				cc.Format(_T("debuginfo3,AiChair=%d,userChair=%d"), wWinAndroid, wWinUser);
				WriteInfo(cc);
			}
		}

	}
	else if (g_lRoomStorageCurrent >= g_lStorageMax2Room)
	{
		if (rand() % 100 <= g_lStorageMul2Room)//玩家赢
		{
			if (m_cbRealPlayer[wWinUser] == TRUE)
				return;

			//随机赢家
			WORD wWinPlayer = INVALID_CHAIR;

			do
			{
				wWinPlayer = rand() % m_wPlayerCount;
			} while (m_cbRealPlayer[wWinPlayer] == FALSE);

			if (wWinPlayer != wWinUser)
			{
				//交换数据
				BYTE cbTempData[MAX_COUNT];
				CopyMemory(cbTempData, m_cbHandCardData[wWinPlayer], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinPlayer], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
				CString cc;
				cc.Format(_T("debuginfo4,AiChair=%d,userChair=%d"), wWinPlayer, wWinUser);
				WriteInfo(cc);
			}
		}
		else//玩家输
		{
			if (m_cbAndroidStatus[wWinUser] == TRUE)
				return;

			//随机赢家
			WORD wWinAndroid = INVALID_CHAIR;

			do
			{
				wWinAndroid = rand() % m_wPlayerCount;
			} while (m_cbAndroidStatus[wWinAndroid] == FALSE);

			if (wWinAndroid != wWinUser)
			{
				//交换数据
				BYTE cbTempData[MAX_COUNT];
				CopyMemory(cbTempData, m_cbHandCardData[wWinAndroid], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinAndroid], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
				CString cc;
				cc.Format(_T("debuginfo5,AiChair=%d,userChair=%d"), wWinAndroid, wWinUser);
				WriteInfo(cc);
			}
		}

	}

    return;
}

//读取配置
void CTableFrameSink::ReadConfigInformation()
{
    //获取自定义配置
    tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
    ASSERT(pCustomRule);

    g_lRoomStorageStart = pCustomRule->lRoomStorageStart;
    g_lRoomStorageCurrent = pCustomRule->lRoomStorageStart;
    g_lStorageDeductRoom = pCustomRule->lRoomStorageDeduct;
    g_lStorageMax1Room = pCustomRule->lRoomStorageMax1;
    g_lStorageMul1Room = pCustomRule->lRoomStorageMul1;
    g_lStorageMax2Room = pCustomRule->lRoomStorageMax2;
    g_lStorageMul2Room = pCustomRule->lRoomStorageMul2;

    if(g_lStorageDeductRoom < 0 || g_lStorageDeductRoom > 1000)
    {
        g_lStorageDeductRoom = 0;
    }
    if(g_lStorageDeductRoom > 1000)
    {
        g_lStorageDeductRoom = 1000;
    }
    if(g_lStorageMul1Room < 0 || g_lStorageMul1Room > 100)
    {
        g_lStorageMul1Room = 50;
    }
    if(g_lStorageMul2Room < 0 || g_lStorageMul2Room > 100)
    {
        g_lStorageMul2Room = 80;
    }

	m_cbMinAllinTurnCount = 3;// pCustomRule->nMinAllinTurnCount;
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
    if(userAction == USER_SITDOWN || userAction == USER_RECONNECT)
    {
        roomUserInfo.wChairID = pIServerUserItem->GetChairID();
        roomUserInfo.wTableID = pIServerUserItem->GetTableID() + 1;
    }
    else if(userAction == USER_STANDUP || userAction == USER_OFFLINE)
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
    DWORD *pdwRemoveKey	= new DWORD[g_MapRoomUserInfo.GetSize()];
    ZeroMemory(pdwRemoveKey, sizeof(DWORD) * g_MapRoomUserInfo.GetSize());
    WORD wRemoveKeyIndex = 0;

    while(ptHead)
    {
        g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

        if(userinfo.dwGameID == 0 && (_tcscmp(szNickName, userinfo.szNickName) == 0) && userinfo.cbUserStatus == 0)
        {
            pdwRemoveKey[wRemoveKeyIndex++] = dwUserID;
        }

    }

    for(WORD i = 0; i < wRemoveKeyIndex; i++)
    {
        g_MapRoomUserInfo.RemoveKey(pdwRemoveKey[i]);
    }

    delete[] pdwRemoveKey;
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
    while(ptListHead)
    {
        ptTemp = ptListHead;
        roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

        //寻找已存在调试玩家在用一张桌子切换椅子
        if((keyroomuserdebug.roomUserInfo.dwGameID == roomuserdebug.roomUserInfo.dwGameID) &&
                _tcscmp(keyroomuserdebug.roomUserInfo.szNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
        {
            g_ListRoomUserDebug.RemoveAt(ptTemp);
        }
    }
}

//是否满足调试条件
void CTableFrameSink::IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnableDebug)
{
    if(userInfo.wChairID == INVALID_CHAIR || userInfo.wTableID == INVALID_TABLE)
    {
        bEnableDebug = FALSE;
        return;
    }

    if(userInfo.cbUserStatus == US_SIT || userInfo.cbUserStatus == US_READY || userInfo.cbUserStatus == US_PLAYING)
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

//分析房间用户调试
bool CTableFrameSink::AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList)
{
    //变量定义
    POSITION ptListHead;
    POSITION ptTemp;
    ROOMUSERDEBUG roomuserdebug;

    //遍历链表
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(!pIServerUserItem)
        {
            continue;
        }

        //初始化
        ptListHead = g_ListRoomUserDebug.GetHeadPosition();
        ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

        //遍历链表
        while(ptListHead)
        {
            ptTemp = ptListHead;
            roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

            //寻找玩家
            if((pIServerUserItem->GetGameID() == roomuserdebug.roomUserInfo.dwGameID) &&
                    _tcscmp(pIServerUserItem->GetNickName(), roomuserdebug.roomUserInfo.szNickName) == 0)
            {
                //清空调试局数为0的元素
                if(roomuserdebug.userDebug.cbDebugCount == 0)
                {
                    g_ListRoomUserDebug.RemoveAt(ptTemp);
                    break;
                }

                if(roomuserdebug.userDebug.debug_type == CONTINUE_CANCEL)
                {
                    g_ListRoomUserDebug.RemoveAt(ptTemp);
                    break;
                }

                //拷贝数据
                CopyMemory(&Keyroomuserdebug, &roomuserdebug, sizeof(roomuserdebug));
				Keyroomuserdebug.roomUserInfo.wChairID = pIServerUserItem->GetChairID();
                ptList = ptTemp;

                return true;
            }

        }

    }

    return false;
}

void CTableFrameSink::GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr)
{
    switch(debugType)
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
        debugTypestr = TEXT("调试类型为取消调试");
        break;
    }
    }
}

//写日志文件
void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{
	//韩湘 2020/01/05 屏蔽机器人日志
#ifndef DEBUG
	return;
#endif // !DEBUG
	//设置语言区域
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	//创建目录
	TCHAR szFilePack[MAX_PATH] = _T(".//疯狂三张日志目录");
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

//修改库存
bool CTableFrameSink::UpdateStorage(LONGLONG lScore)
{
	if (m_cbAiCount == m_cbPlayingCount || m_cbAiCount==0)
    {
        return true;
    }

    g_lRoomStorageCurrent += lScore;

    return true;
}

//乱序密钥
void CTableFrameSink::RandAESKey(unsigned char chAESKeyBuffer[], BYTE cbBufferCount)
{
    for(WORD i = 0; i < cbBufferCount; i++)
    {
        chAESKeyBuffer[i] = rand() % 256;
    }
}

bool CTableFrameSink::IsRoomCard()
{
	return ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0);
}

//判断积分约占房间
bool CTableFrameSink::IsRoomCardScoreType()
{
    return (m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

//判断金币约占房间
bool CTableFrameSink::IsRoomCardTreasureType()
{
    return (m_pITableFrame->GetDataBaseMode() == 1) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

void CTableFrameSink::ConfigAddButtonValue()
{
	//获取自定义配置
	tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomRule);

	if (IsRoomCardScoreType())
	{
		if (m_lCellScore == 1)
		{
			CopyMemory(m_nButtonValue, pCustomRule->nButtonValue1, sizeof(m_nButtonValue));
		}
		else if (m_lCellScore == 2)
		{
			CopyMemory(m_nButtonValue, pCustomRule->nButtonValue2, sizeof(m_nButtonValue));
		}
		else if (m_lCellScore == 3)
		{
			CopyMemory(m_nButtonValue, pCustomRule->nButtonValue3, sizeof(m_nButtonValue));
		}
	}
	else
	{
		m_nButtonValue[0] = 2;
		m_nButtonValue[1] = 5;
		m_nButtonValue[2] = 8;
		m_nButtonValue[3] = 10;
	}

}

//////////////////////////////////////////////////////////////////////////
