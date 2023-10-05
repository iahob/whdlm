#include "StdAfx.h"
#include "TableFrameSink.h"
#include <conio.h>

//////////////////////////////////////////////////////////////////////////

//静态变量
WORD				CTableFrameSink::m_wPlayerCount = GAME_PLAYER;			//游戏人数
bool				CTableFrameSink::m_bFirstInit = true;
LONGLONG			CTableFrameSink::m_lStockScore = 0;
//////////////////////////////////////////////////////////////////////////

#define	TIME_OPERATE							1000						//代打定时器

#define	IDI_GAME_FREE							10							//空闲定时器
#define	IDI_CALL_BANKER							11							//叫庄定时器
#define	IDI_ADD_SCORE							12							//下注定时器
#define	IDI_SEND_CARD							13							//发牌定时器


//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
    //游戏变量
    m_lExitScore = 0;
    m_lDynamicScore = 0;
    m_wBankerUser = INVALID_CHAIR;
    m_wFisrtCallUser = INVALID_CHAIR;
    m_wCurrentUser = INVALID_CHAIR;

    m_cbTimeStartGame = 20;					//开始时间
    m_cbTimeCallBanker = 10;				//叫庄时间
    m_cbTimeAddScore = 10;					//下注时间
    m_cbTimeOpenCard = 20;					//开牌时间

    //房卡变量
    m_cbBaseScore = 1;						//下注底分
    m_bCallBanker = true;					//是否抢庄
    ZeroMemory(&m_stRecord, sizeof(m_stRecord));

    //用户状态
    ZeroMemory(m_cbDynamicJoin, sizeof(m_cbDynamicJoin));
    ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
    ZeroMemory(m_nCallStatus, sizeof(m_nCallStatus));
    ZeroMemory(m_bSpecialClient, sizeof(m_bSpecialClient));
    ZeroMemory(m_bOperation, sizeof(m_bOperation));

    m_cbCallCount = 0;
    m_cbAddCount = 0;
    m_cbOpenCount = 0;

    //扑克变量
    ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));

    ZeroMemory(m_bBuckleServiceCharge, sizeof(m_bBuckleServiceCharge));

    //组件变量
    m_pITableFrame = NULL;
    m_pGameServiceOption = NULL;

    //服务控制
    //     m_hInst = NULL;
    //     m_pServerContro = NULL;
    //     m_hInst = LoadLibrary(TEXT("SanGongServerControl.dll"));
    //     if(m_hInst)
    //     {
    //         typedef void *(*CREATE)();
    //         CREATE ServerControl = (CREATE)GetProcAddress(m_hInst, "CreateServerControl");
    //         if(ServerControl)
    //         {
    //             m_pServerContro = static_cast<IServerControl *>(ServerControl());
    //         }
    //     }

    //游戏视频
    m_hVideoInst = NULL;
    m_pGameVideo = NULL;
    m_hVideoInst = LoadLibrary(TEXT("SanGongGameVideo.dll"));
    if(m_hVideoInst)
    {
        typedef void *(*CREATE)();
        CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
        if(GameVideo)
        {
            m_pGameVideo = static_cast<IGameVideo *>(GameVideo());
        }
    }

    ZeroMemory(&m_stRecord, sizeof(m_stRecord));

    if(m_bFirstInit)
    {
        CString strName = GetFileDialogPath() +	"\\SanGongBattle.log";
        CFileFind findLogFile;
        if(findLogFile.FindFile(strName))
        {
            ::DeleteFile(strName);
        }

        m_bFirstInit = false;
    }
    return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
    if(m_pServerContro)
    {
        delete m_pServerContro;
        m_pServerContro = NULL;
    }

    if(m_hInst)
    {
        FreeLibrary(m_hInst);
        m_hInst = NULL;
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

//接口查询--检测相关信息版本
void *CTableFrameSink::QueryInterface(const IID &Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
    QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
    return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
    //查询接口
    ASSERT(pIUnknownEx != NULL);
    m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
    if(m_pITableFrame == NULL)
    {
        return false;
    }

    m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

    //游戏配置
    m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
    m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

    //自定规则
    ASSERT(m_pITableFrame->GetCustomRule() != NULL);
    m_pGameCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();

    m_cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
    m_cbTimeCallBanker = m_pGameCustomRule->cbTimeCallBanker;
    m_cbTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
    m_cbTimeOpenCard = m_pGameCustomRule->cbTimeOpenCard;

    //房卡配置
    m_cbBaseScore	= m_pGameCustomRule->cbBaseScore;
    m_bCallBanker	= m_pGameCustomRule->bCallBanker;
    ZeroMemory(&m_stRecord, sizeof(m_stRecord));

    //获取目录
    TCHAR szPath[MAX_PATH] = TEXT("");
    GetCurrentDirectory(CountArray(szPath), szPath);

    //读取配置
    TCHAR szFileName[MAX_PATH] = TEXT("");
    _sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\SanGongConfig.ini"), szPath);

    //开始存储数目
    TCHAR szServerName[MAX_PATH] = TEXT("");
    _sntprintf(szServerName, CountArray(szServerName), TEXT("%s"), m_pGameServiceOption->szServerName);
    m_lStockScore = GetPrivateProfileInt(szServerName, TEXT("StorageStart"), 0, szFileName);
    m_lStorageDeduct = GetPrivateProfileInt(szServerName, TEXT("StorageDeduct"), 0, szFileName);
    m_nStorageMul = GetPrivateProfileInt(szServerName, TEXT("StorageMul"), 50, szFileName);

    TCHAR OutBuf[255] = {};
    ZeroMemory(OutBuf, sizeof(OutBuf));
    GetPrivateProfileString(szServerName, TEXT("StorageMax"), TEXT("50000000"), OutBuf, 255, szFileName);
    _sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lStorageMax);

    m_lStockLimit = 0;

    if(m_lStockScore >= m_lStockLimit)
    {
        m_lStockLimit = 0;
    }

    if(m_lStorageDeduct < 0 || m_lStorageDeduct > 1000)
    {
        m_lStorageDeduct = 0;
    }
    if(m_lStorageDeduct > 1000)
    {
        m_lStorageDeduct = 10;
    }
    if(m_nStorageMul < 0 || m_nStorageMul > 100)
    {
        m_nStorageMul = 50;
    }

    return true;
}

//复位桌子
void CTableFrameSink::RepositionSink()
{
    CStringA lyt;
    lyt.Format("lytlog::-------- 复位桌子 -RepositionSink()---------- ");
    OutputDebugStringA(lyt);

    //游戏变量
    m_lExitScore = 0;
    m_wCurrentUser = INVALID_CHAIR;
    m_lDynamicScore = 0;

    //用户状态
    ZeroMemory(m_cbDynamicJoin, sizeof(m_cbDynamicJoin));
    ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
    ZeroMemory(m_nCallStatus, sizeof(m_nCallStatus));
    ZeroMemory(m_bSpecialClient, sizeof(m_bSpecialClient));
    ZeroMemory(m_bOperation, sizeof(m_bOperation));

    m_cbCallCount = 0;
    m_cbAddCount = 0;
    m_cbOpenCount = 0;
    //m_bAllReady = false;

    //扑克变量
    ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));

    return;
}

//游戏状态
// bool CTableFrameSink::IsUserPlaying(WORD wChairID)
// {
//     ASSERT(wChairID < m_wPlayerCount && m_cbPlayStatus[wChairID] == TRUE);
//     if(wChairID < m_wPlayerCount && m_cbPlayStatus[wChairID] == TRUE)
//     {
//         return true;
//     }
//     return false;
// }

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    CStringA lyt;
    //历史积分
    if(bLookonUser == false)
    {
        m_HistoryScore.OnEventUserEnter(pIServerUserItem->GetChairID());
    }

    if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //房卡模式
    {
        //m_bFirstRound = true;

        // 		for (int i = 0; i < m_wPlayerCount; i++)
        // 		{
        // 			//获取用户
        // 			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        // 			if (pIServerUserItem == NULL)
        // 			{
        // 				continue;
        // 			}
        // 			pIServerUserItem->SetUserStatus(US_PLAYING, pIServerUserItem->GetTableID(), i);
        // 		}

        //获取房卡规则
        BYTE *pSetInfo = m_pITableFrame->GetGameRule();
        if(pSetInfo[0] == 1)
        {
            // 			BYTE cbChairCount	 = pSetInfo[1];
            // 			BYTE cbMaxChairCount = pSetInfo[2];

            m_wPlayerCount	= pSetInfo[1];			//游戏人数
            m_cbBaseScore	= pSetInfo[3];			//底分
            m_bCallBanker	= pSetInfo[4];			//是否抢庄

			TCHAR szDes[200];
			_sntprintf(szDes, 200, TEXT("房卡规则[%d][%d][%d]"), pSetInfo[1], pSetInfo[3], pSetInfo[4]);
			CTraceService::TraceString(szDes, TraceLevel_Debug);

            lyt.Format("lytlog:: 用户坐下-------游戏人数= %d 底分= %d 是否抢庄= %d  ", m_wPlayerCount, m_cbBaseScore, m_bCallBanker);
            OutputDebugStringA(lyt);
        }
    }

    if(m_pITableFrame->GetGameStatus() != GS_TK_FREE)
    {
        m_cbDynamicJoin[pIServerUserItem->GetChairID()] = true;
    }

    return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //     BYTE cbUserCount = 0;
    //     WORD wUser[GAME_PLAYER];
    //     memset(wUser, -1, sizeof(wUser));
    //     for(int i = 0; i < m_wPlayerCount; i++)
    //     {
    //         if(m_pITableFrame->GetTableUserItem(i) == NULL)
    //         {
    //             continue;
    //         }
    //         wUser[cbUserCount++] = i;
    //     }
    //     if(wChairID == m_wFisrtCallUser)
    //     {
    //         if(cbUserCount >= 1)
    //         {
    //             CMD_S_UserExit UserExit;
    //             UserExit.wChairID = wUser[0];
    //             m_wFisrtCallUser = wUser[0];
    //             m_pITableFrame->SendTableData(wUser[0], SUB_S_USE_EXIT, &UserExit, sizeof(CMD_S_UserExit));
    //         }
    //     }
    //历史积分
    if(bLookonUser == false)
    {
        m_HistoryScore.OnEventUserLeave(pIServerUserItem->GetChairID());
        m_cbDynamicJoin[pIServerUserItem->GetChairID()] = false;
        m_bSpecialClient[pIServerUserItem->GetChairID()] = false;
    }

    if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //房卡模式
    {
        if(m_pITableFrame->IsPersonalRoomDisumme()) //当前朋友局解散清理记录
        {
            ZeroMemory(&m_stRecord, sizeof(m_stRecord));
        }
    }

    return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//波波
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, NULL, 0);
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, NULL, 0);

    //库存
    if(m_lStockScore > 0)
    {
        m_lStockScore = m_lStockScore - m_lStockScore * m_lStorageDeduct / 1000;
    }

    CString strInfo;
    strInfo.Format(TEXT("【%s】 -----------当前库存：%I64d"), m_pGameServiceOption->szServerName, m_lStockScore);
    NcaTextOut(strInfo);

    m_cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
    m_cbTimeCallBanker = m_pGameCustomRule->cbTimeCallBanker;
    m_cbTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
    m_cbTimeOpenCard = m_pGameCustomRule->cbTimeOpenCard;

    //     CStringA lyt;
    //     lyt.Format("lytlog::-----------游戏开始----m_cbTimeStartGame=%d--m_cbTimeCallBanker=%d---m_cbTimeAddScore=%d---m_cbTimeOpenCard=%d-", m_cbTimeStartGame, m_cbTimeCallBanker, m_cbTimeAddScore, m_cbTimeOpenCard);
    //     OutputDebugStringA(lyt);

    ZeroMemory(m_bBuckleServiceCharge, sizeof(m_bBuckleServiceCharge));
    bool bFirstRecord = true;
    WORD wRealPlayerCount = 0;
    //用户状态
    for(int i = 0; i < m_wPlayerCount; i++)
    {
        //获取用户
        IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);

        if(pIServerUser == NULL)
        {
            m_cbPlayStatus[i] = FALSE;
        }
        else
        {
            m_cbPlayStatus[i] = TRUE;
            wRealPlayerCount++;

            strInfo.Format(TEXT("【%s】 -----------玩家%d 状态：%d"), m_pGameServiceOption->szServerName, i, pIServerUser->GetUserStatus());
            NcaTextOut(strInfo);
        }
        // 		if(pIServerUser!=NULL&&pIServerUser->IsAndroidUser())
        // 		{
        // 			m_pITableFrame->SendTableData(i,SUB_S_BANKER_OPERATE);
        // 		}
    }

    if(m_pGameVideo)
    {
        m_pGameVideo->StartVideo(m_pITableFrame);
    }

    //游戏视频
    if(m_pGameVideo)
    {
        Video_Bein video;
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] != TRUE)
            {
                continue;
            }

            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            //获取房主
            DWORD OwnerId = m_pITableFrame->GetTableOwner();
            IServerUserItem *pOwnerItem = m_pITableFrame->SearchUserItem(OwnerId);
            if(pOwnerItem && pOwnerItem->GetChairID() != INVALID_CHAIR)
            {
                video.wRoomID = pOwnerItem->GetChairID();
            }

            video.wPlayerCount = wRealPlayerCount;
            video.wGamePlayerCountRule = pGameRule[1];
            video.wChairID = i;
            video.lScore = pIServerUserItem->GetUserScore();
            video.lCellScore = m_pITableFrame->GetCellScore();
            //m_pGameVideo->AddVideoData(SUB_S_VEDIO_BEGIN, &video, bFirstRecord);
            m_pGameVideo->AddVideoData(SUB_S_VEDIO_BEGIN, &video, sizeof(video), bFirstRecord);

            if(bFirstRecord == true)
            {
                bFirstRecord = false;
            }
        }
    }

    //抢庄
    if(m_bCallBanker)
    {
        //设置状态
        m_pITableFrame->SetGameStatus(GS_TK_CALL);

        //设置变量
        CMD_S_CallBegin CallBegin;
        CallBegin.bCallBanker = true;

        //时间定义
        CallBegin.cbTimeStartGame = m_cbTimeStartGame;					//开始时间
        CallBegin.cbTimeCallBanker = m_cbTimeCallBanker;				//叫庄时间
        CallBegin.cbTimeAddScore = m_cbTimeAddScore;					//下注时间
        CallBegin.cbTimeOpenCard = m_cbTimeOpenCard;					//开牌时间

        //发送数据
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] != TRUE)
            {
                continue;
            }
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
            m_pITableFrame->SendTableData(i, SUB_S_CALL_BEGIN, &CallBegin, sizeof(CallBegin));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BEGIN, &CallBegin, sizeof(CallBegin));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_CALL_BEGIN, &CallBegin, sizeof(CallBegin), true);
        }
        //if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) //房卡模式
        {
            m_pITableFrame->SetGameTimer(IDI_CALL_BANKER, TIME_OPERATE, m_cbTimeCallBanker, 0);
        }
    }
    else	//9点上庄
    {
        //设置状态
        m_pITableFrame->SetGameStatus(GS_TK_SCORE);

        //第一局随机定庄 或者 庄家强退、逃跑
        if(INVALID_CHAIR == m_wBankerUser || (INVALID_CHAIR != m_wBankerUser && m_pITableFrame->GetTableUserItem(m_wBankerUser) == NULL))
        {
            m_wBankerUser = rand() % m_wPlayerCount;
            while(m_pITableFrame->GetTableUserItem(m_wBankerUser) == NULL)
            {
                m_wBankerUser = (m_wBankerUser + 1) % m_wPlayerCount;
            }
        }

        for(int i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
            if(pUserItem && pUserItem->GetChairID() != INVALID_CHAIR)
            {
                strInfo.Format(TEXT("【%s】 -------用户昵称 %s----玩家坐位号：%d，当前庄家：%d"), m_pGameServiceOption->szServerName, pUserItem->GetNickName(), pUserItem->GetChairID(), m_wBankerUser);
                NcaTextOut(strInfo);
            }
        }

        strInfo.Format(TEXT("【%s】 ---------当前庄家：%d 昵称 %s 坐位号：%d"), m_pGameServiceOption->szServerName, m_wBankerUser, m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetNickName(), m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetChairID());
        NcaTextOut(strInfo);

        //设置变量
        CMD_S_AddBegin AddBegin;
        AddBegin.wBankerUser = m_wBankerUser;
        AddBegin.lCellScore = m_cbBaseScore;

        //时间定义
        AddBegin.cbTimeStartGame = m_cbTimeStartGame;					//开始时间
        AddBegin.cbTimeCallBanker = m_cbTimeCallBanker;					//叫庄时间
        AddBegin.cbTimeAddScore = m_cbTimeAddScore;						//下注时间
        AddBegin.cbTimeOpenCard = m_cbTimeOpenCard;						//开牌时间
        AddBegin.bCallBanker = false;

        //发送数据
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }
            m_pITableFrame->SendTableData(i, SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin));
        if(m_pGameVideo)
        {
            //m_pGameVideo->AddVideoData(SUB_S_ADD_BEGIN, &AddBegin);
            m_pGameVideo->AddVideoData(SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin), true);
        }

        //if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) //房卡模式
        {
            m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, TIME_OPERATE, m_cbTimeAddScore, 0);
        }
    }

    //随机扑克
    BYTE bTempArray[GAME_PLAYER * MAX_COUNT];
    m_GameLogic.RandCardList(bTempArray, sizeof(bTempArray));

    // 	BYTE bTemp[GAME_PLAYER * MAX_COUNT] = { 0x04, 0x25, 0x16,
    // 											0x14, 0x17, 0x18,
    // 											0x24, 0x21, 0x28,
    // 											0x01, 0x02, 0x03,
    // 											0x11, 0x12, 0x19,
    // 											0x34, 0x36, 0x37 };
    //
    // 	CopyMemory(bTempArray, bTemp, sizeof(bTempArray));

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUser == NULL)
        {
            continue;
        }
        //派发扑克
        CopyMemory(m_cbHandCardData[i], &bTempArray[i * MAX_COUNT], MAX_COUNT);
    }

    return true;
}
//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    switch(cbReason)
    {
    case GER_DISMISS:		//游戏解散
    {
        //构造数据
        CMD_S_GameEnd GameEnd = {0};

        //发送信息
        m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

        //删除时间
        m_pITableFrame->KillGameTimer(IDI_CALL_BANKER);
        m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
        m_pITableFrame->KillGameTimer(IDI_SEND_CARD);
        //结束游戏
        m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //房卡模式
        {
            if(m_pITableFrame->IsPersonalRoomDisumme()) //当前朋友局解散清理记录
            {
                ZeroMemory(&m_stRecord, sizeof(m_stRecord));
            }
        }

        return true;
    }
    case GER_NORMAL:		//常规结束
    {
        CString strInfo;

        //删除时间
        m_pITableFrame->KillGameTimer(IDI_CALL_BANKER);
        m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
        m_pITableFrame->KillGameTimer(IDI_SEND_CARD);

        //定义变量
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd, sizeof(GameEnd));
        WORD wWinTimes[GAME_PLAYER], wWinCount[GAME_PLAYER];
        ZeroMemory(wWinCount, sizeof(wWinCount));
        ZeroMemory(wWinTimes, sizeof(wWinTimes));

        //保存扑克
        BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
        CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(cbUserCardData));

        for(int i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            GameEnd.cbType[i] = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
        }

        if(m_wBankerUser != INVALID_CHAIR)
        {
            wWinTimes[m_wBankerUser] = m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser], MAX_COUNT);
        }

        //对比玩家
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            //对比扑克
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_COUNT))
            {
                wWinCount[i]++;
                wWinTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_COUNT);
            }
            else
            {
                wWinCount[m_wBankerUser]++;
            }
        }

        //统计得分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            if(wWinCount[i] > 0)	//闲家胜利
            {
                GameEnd.lGameScore[i] = (m_lTableScore[i]) * wWinTimes[i];
                GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameScore[i];
                m_lTableScore[i] = 0;
            }
            else					//庄家胜利
            {
                GameEnd.lGameScore[i] = (-1) * ((m_lTableScore[i]) * wWinTimes[m_wBankerUser]);
                GameEnd.lGameScore[m_wBankerUser] += (-1) * GameEnd.lGameScore[i];
                m_lTableScore[i] = 0;
            }
        }

        //         for(int i = 0; i < m_wPlayerCount; i++)
        //         {
        //             strInfo.Format(TEXT("【%s】 常规结束0000 玩家%d 结算分 %I64d"), m_pGameServiceOption->szServerName, i, GameEnd.lGameScore[i]);
        //             NcaTextOut(strInfo);
        //         }
        //赢家9点上庄
        if(!m_bCallBanker)
        {
            WORD wMaxUser = INVALID_CHAIR;
            BYTE cbCardType[GAME_PLAYER];
            ZeroMemory(cbCardType, sizeof(cbCardType));
            BYTE cbMaxType;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }
                if(wWinCount[i] > 0 && 9 == m_GameLogic.GetAllCardValue(cbUserCardData[i], MAX_COUNT))	//胜利
                {
                    cbCardType[i] = m_GameLogic.GetCardType(cbUserCardData[i], MAX_COUNT);
                }
            }
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(0 != cbCardType[i])
                {
                    cbMaxType = cbCardType[i];
                    m_wBankerUser = i;
                    break;
                }
            }
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(cbMaxType < cbCardType[i])
                {
                    cbMaxType = cbCardType[i];
                    m_wBankerUser = i;
                }
            }
        }

        //修改积分
        tagScoreInfo ScoreInfoArray[GAME_PLAYER];
        ZeroMemory(ScoreInfoArray, sizeof(ScoreInfoArray));

        //积分税收
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)  continue;

			//不是积分房卡 都需要结算税收
            if(GameEnd.lGameScore[i] > 0L && !IsRoomCardScoreType())
            {
                GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]); //GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenueRatio/1000l;
                if(GameEnd.lGameTax[i] > 0)
                {
                    GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
                }
            }

            //历史积分
            m_HistoryScore.OnEventUserScore(i, GameEnd.lGameScore[i]);

            //保存积分
            ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
            ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
            ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

            //总结算记录
            //if(m_stRecord.nCount<MAX_RECORD_COUNT)
            {
                m_stRecord.lAllScore[i] += GameEnd.lGameScore[i];
            }

            //房卡模式
            if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_stRecord.nCount[i] < MAX_RECORD_COUNT))
            {
                m_stRecord.lDetailScore[i][m_stRecord.nCount[i]] = ScoreInfoArray[i].lScore;
            }

			m_stRecord.nCount[i]++;
        }


        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //房卡模式
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
        }

        //发送信息
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }
            m_pITableFrame->SendTableData(i, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        }

        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd, sizeof(GameEnd), true);
            m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID(), m_wPlayerCount);
        }

        //库存统计
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //获取用户
            IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserIte == NULL) continue;

            //库存累计
            if(!pIServerUserIte->IsAndroidUser())
            {
                m_lStockScore -= GameEnd.lGameScore[i];
            }
        }

        for(WORD y = 0; y < m_wPlayerCount; y++)
        {
            IServerUserItem *pICallUser = m_pITableFrame->GetTableUserItem(y);
            if(pICallUser != NULL)
            {
                if(pICallUser->GetUserScore() < m_cbBaseScore * 40)
                {
                    if(pICallUser->IsAndroidUser())
                    {
                        m_pITableFrame->PerformStandUpAction(pICallUser);
                    }
                    else
                    {
                        m_pITableFrame->SendGameMessage(pICallUser, TEXT("您的金币不满足桌主底分设置！"), SMT_CHAT | SMT_EJECT | SMT_CLOSE_GAME);
                    }
                }
            }
        }

        //写入积分
        m_pITableFrame->WriteTableScore(ScoreInfoArray, m_wPlayerCount);

        //结束游戏
        m_pITableFrame->ConcludeGame(GS_TK_FREE);

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //房卡模式
        {
            if(m_pITableFrame->IsPersonalRoomDisumme()) //当前朋友局解散清理记录
            {
                ZeroMemory(&m_stRecord, sizeof(m_stRecord));
            }
        }

        return true;
    }
    case GER_USER_LEAVE:		//用户强退
    case GER_NETWORK_ERROR:
    {
        //效验参数
        ASSERT(pIServerUserItem != NULL);
        ASSERT(wChairID < m_wPlayerCount && (m_cbPlayStatus[wChairID] == TRUE || m_cbDynamicJoin[wChairID] == FALSE));
        //
        //         //设置状态
        //         //m_cbPlayStatus[wChairID]=FALSE;
        //         //m_cbDynamicJoin[wChairID]=FALSE;

        //定义变量
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd, sizeof(GameEnd));
        WORD wWinTimes[GAME_PLAYER], wWinCount[GAME_PLAYER];
        ZeroMemory(wWinCount, sizeof(wWinCount));
        ZeroMemory(wWinTimes, sizeof(wWinTimes));

        //保存扑克
        BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
        CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(cbUserCardData));

        for(int i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            GameEnd.cbType[i] = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
        }

        if(m_wBankerUser != INVALID_CHAIR)
        {
            wWinTimes[m_wBankerUser] = m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser], MAX_COUNT);
        }

        //对比玩家
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            //对比扑克
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_COUNT))
            {
                wWinCount[i]++;
                wWinTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_COUNT);
            }
            else
            {
                wWinCount[m_wBankerUser]++;
            }
        }

        //统计得分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            if(wWinCount[i] > 0)	//闲家胜利
            {
                GameEnd.lGameScore[i] = (m_lTableScore[i]) * wWinTimes[i];
                GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameScore[i];
                m_lTableScore[i] = 0;
            }
            else					//庄家胜利
            {
                GameEnd.lGameScore[i] = (-1) * ((m_lTableScore[i]) * wWinTimes[m_wBankerUser]);
                GameEnd.lGameScore[m_wBankerUser] += (-1) * GameEnd.lGameScore[i];
                m_lTableScore[i] = 0;
            }
        }

        //修改积分
        tagScoreInfo ScoreInfoArray[GAME_PLAYER];
        ZeroMemory(ScoreInfoArray, sizeof(ScoreInfoArray));

        //积分税收
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            if(GameEnd.lGameScore[i] > 0L && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL == 0))
            {
                GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                if(GameEnd.lGameTax[i] > 0)
                {
                    GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
                }
            }

            //历史积分
            m_HistoryScore.OnEventUserScore(i, GameEnd.lGameScore[i]);

            //保存积分
            ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
            ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
            ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
        }

        //写入积分
        m_pITableFrame->WriteTableScore(ScoreInfoArray, m_wPlayerCount);

        //定义变量
        CMD_S_PlayerExit PlayerExit;
        PlayerExit.wPlayerID = wChairID;

        //发送信息
        for(int i = 0; i < m_wPlayerCount; i++)
        {
            if(i == wChairID || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            m_pITableFrame->SendTableData(i, SUB_S_PLAYER_EXIT, &PlayerExit, sizeof(PlayerExit));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PLAYER_EXIT, &PlayerExit, sizeof(PlayerExit));

        //结束游戏
        //m_pITableFrame->ConcludeGame(GS_TK_FREE);
        return true;
    }
    }

    return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
    switch(cbGameStatus)
    {
    case GAME_STATUS_FREE:		//空闲状态
    {
        //构造数据
        CMD_S_StatusFree StatusFree;
        ZeroMemory(&StatusFree, sizeof(StatusFree));

        //自定规则
        StatusFree.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
        StatusFree.cbTimeCallBanker = m_pGameCustomRule->cbTimeCallBanker;
        StatusFree.cbTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
        StatusFree.cbTimeOpenCard = m_pGameCustomRule->cbTimeOpenCard;
        //规则
		StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();
        StatusFree.cbBaseScore	= m_cbBaseScore;
        StatusFree.bCallBanker	= m_bCallBanker;
		StatusFree.cbTrusteeMode = m_cbTrusteeMode;

		//TCHAR szDes[200];
		//_sntprintf(szDes, 200, TEXT("托管模式 0下注最小额度，1下注最大额度 = [%d]"), m_cbTrusteeMode);
		//CTraceService::TraceString(szDes, TraceLevel_Exception);


        //         CStringA lyt;
        //         lyt.Format("lytlog::-----------空闲状态----m_cbTimeStartGame=%d--m_cbTimeCallBanker=%d---m_cbTimeAddScore=%d---m_cbTimeOpenCard=%d-", m_cbTimeStartGame, m_cbTimeCallBanker, m_cbTimeAddScore, m_cbTimeOpenCard);
        //         OutputDebugStringA(lyt);

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusFree.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusFree.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //房间名称
        CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //房卡模式
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
        }

        //发送场景
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
    }
    case GS_TK_CALL:	//叫庄状态
    {
        //构造数据
        CMD_S_StatusCall StatusCall;
        ZeroMemory(&StatusCall, sizeof(StatusCall));

        //自定规则
        StatusCall.cbTimeStartGame = m_cbTimeStartGame;
        StatusCall.cbTimeCallBanker = m_cbTimeCallBanker;
        StatusCall.cbTimeAddScore = m_cbTimeAddScore;
        StatusCall.cbTimeOpenCard = m_cbTimeOpenCard;
        //规则
		StatusCall.cbPlayMode = m_pITableFrame->GetDataBaseMode();
        StatusCall.cbBaseScore	= m_cbBaseScore;
        StatusCall.bCallBanker	= m_bCallBanker;
		StatusCall.cbTrusteeMode = m_cbTrusteeMode;

        //历史积分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusCall.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusCall.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //设置变量
        CopyMemory(StatusCall.nCallStatus, m_nCallStatus, sizeof(StatusCall.nCallStatus));
        StatusCall.cbDynamicJoin = m_cbDynamicJoin[wChiarID];
        CopyMemory(StatusCall.cbPlayStatus, m_cbPlayStatus, sizeof(StatusCall.cbPlayStatus));

        //房间名称
        CopyMemory(StatusCall.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusCall.szGameRoomName));

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //房卡模式
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
        }

        //发送场景
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusCall, sizeof(StatusCall));
    }
    case GS_TK_SCORE:	//下注状态
    {
        //构造数据
        CMD_S_StatusScore StatusScore;
        memset(&StatusScore, 0, sizeof(StatusScore));

        //         CStringA lyt;
        //         lyt.Format("lytlog::-----------断线重连  下注状态----------------");
        //         OutputDebugStringA(lyt);

        //自定规则
        StatusScore.cbTimeStartGame = m_cbTimeStartGame;
        StatusScore.cbTimeCallBanker = m_cbTimeCallBanker;
        StatusScore.cbTimeAddScore = m_cbTimeAddScore;
        StatusScore.cbTimeOpenCard = m_cbTimeOpenCard;
        //规则
		StatusScore.cbPlayMode = m_pITableFrame->GetDataBaseMode();
        StatusScore.cbBaseScore	= m_cbBaseScore;
        StatusScore.bCallBanker	= m_bCallBanker;
		StatusScore.cbTrusteeMode = m_cbTrusteeMode;

        //历史积分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusScore.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusScore.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        StatusScore.wBankerUser = m_wBankerUser;
        StatusScore.cbDynamicJoin = m_cbDynamicJoin[wChiarID];
        CopyMemory(StatusScore.cbPlayStatus, m_cbPlayStatus, sizeof(StatusScore.cbPlayStatus));

        //设置积分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            StatusScore.lTableScore[i] = m_lTableScore[i];
        }

        //房间名称
        CopyMemory(StatusScore.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusScore.szGameRoomName));

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //房卡模式
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
        }

        //发送场景
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusScore, sizeof(StatusScore));
    }
    case GS_TK_PLAYING:	//游戏状态
    {
        //构造数据
        CMD_S_StatusPlay StatusPlay;
        memset(&StatusPlay, 0, sizeof(StatusPlay));

        //自定规则
        StatusPlay.cbTimeStartGame = m_cbTimeStartGame;
        StatusPlay.cbTimeCallBanker = m_cbTimeCallBanker;
        StatusPlay.cbTimeAddScore = m_cbTimeAddScore;
        StatusPlay.cbTimeOpenCard = m_cbTimeOpenCard;

        //规则
		StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();
        StatusPlay.cbBaseScore	= m_cbBaseScore;
        StatusPlay.bCallBanker	= m_bCallBanker;
		StatusPlay.cbTrusteeMode = m_cbTrusteeMode;

        //历史积分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusPlay.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusPlay.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //设置信息
        StatusPlay.wBankerUser = m_wBankerUser;
        StatusPlay.cbDynamicJoin = m_cbDynamicJoin[wChiarID];
        CopyMemory(StatusPlay.cbPlayStatus, m_cbPlayStatus, sizeof(StatusPlay.cbPlayStatus));
        for(int i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            StatusPlay.cbType[i] = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
        }

        CopyMemory(StatusPlay.bOperation, m_bOperation, sizeof(StatusPlay.bOperation));

        //房间名称
        CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));

        //设置扑克
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            StatusPlay.lTableScore[i] = m_lTableScore[i];
            CopyMemory(StatusPlay.cbHandCardData[i], m_cbHandCardData[i], sizeof(BYTE)*MAX_COUNT);
        }

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //房卡模式
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
        }

        //发送场景
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
    }
    }
    //效验错误
    ASSERT(FALSE);

    return true;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{
    switch(dwTimerID)
    {
    case IDI_CALL_BANKER:			//用户叫庄
    {
        m_cbTimeCallBanker--;
        if(m_cbTimeCallBanker <= 0 || m_cbCallCount == GetPlayerCount())
        {
            m_pITableFrame->KillGameTimer(IDI_CALL_BANKER);
            if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)  //非房卡模式
            {
                OnBeginAddScore();
            }
        }

        return true;
    }
    case IDI_ADD_SCORE:			//下注操作
    {
        m_cbTimeAddScore--;
        if(m_cbTimeAddScore <= 0 || (m_cbAddCount + 1) == GetPlayerCount())
        {
            m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
            if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)  //非房卡模式
            {
                OnBeginSendCard();
            }

        }

        return true;
    }
    case IDI_SEND_CARD:			//用户开牌
    {
        m_cbTimeOpenCard--;
        if(m_cbTimeOpenCard <= 0 || m_cbOpenCount == GetPlayerCount())
        {
            m_pITableFrame->KillGameTimer(IDI_SEND_CARD);
            if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)  //非房卡模式
            {
                OnBeginOpenCard();
            }
        }
        return true;
    }
    case IDI_GAME_FREE:			//用户空闲
    {
        m_cbTimeStartGame--;
        if(m_cbTimeStartGame <= 0)
        {
            m_pITableFrame->KillGameTimer(IDI_GAME_FREE);
            for(int i = 0; i < m_wPlayerCount; i++)
            {
                //获取用户
                IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                if(pIServerUserItem == NULL)
                {
                    continue;
                }
                pIServerUserItem->SetUserStatus(US_PLAYING, pIServerUserItem->GetTableID(), i);
            }


			//检查机器人存储款
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//获取用户
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if(!pIServerUserItem)
					continue;
				if(!pIServerUserItem->IsAndroidUser())
					continue;
				m_pITableFrame->SendTableData(i,SUB_S_ANDROID_BANKOPERATOR);
			}

        }
        return true;
    }
    default:
    {
        break;
    }
    }

    return false;
}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    switch(wSubCmdID)
    {
    case SUB_C_CALL_BANKER:			//用户叫庄
    {
        //效验数据
        ASSERT(wDataSize == sizeof(CMD_C_CallBanker));
        if(wDataSize != sizeof(CMD_C_CallBanker))
        {
            return false;
        }

        //变量定义
        CMD_C_CallBanker *pCallBanker = (CMD_C_CallBanker *)pDataBuffer;

        //用户效验
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            return true;
        }

        //状态判断
        ASSERT(pUserData->wChairID < m_wPlayerCount && m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(pUserData->wChairID >= m_wPlayerCount || m_cbPlayStatus[pUserData->wChairID] != TRUE)
        {
            return false;
        }

        //消息处理
        return OnUserCallBanker(pUserData->wChairID, pCallBanker->bBanker);
        break;
    }
    case SUB_C_ADD_SCORE:			//用户加注
    {
        //效验数据
        ASSERT(wDataSize == sizeof(CMD_C_AddScore));
        if(wDataSize != sizeof(CMD_C_AddScore))
        {
            return false;
        }

        //变量定义
        CMD_C_AddScore *pAddScore = (CMD_C_AddScore *)pDataBuffer;

        //用户效验
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            return true;
        }

        //状态判断
        ASSERT(pUserData->wChairID < m_wPlayerCount && m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(pUserData->wChairID >= m_wPlayerCount || m_cbPlayStatus[pUserData->wChairID] != TRUE)
        {
            return false;
        }

        //消息处理
        return OnUserAddScore(pUserData->wChairID, pAddScore->lScore);
        break;
    }
    case SUB_C_OPEN_CARD:			//用户摊牌
    {
        //用户效验
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            return true;
        }

        //状态判断
        ASSERT(m_cbPlayStatus[pUserData->wChairID] != FALSE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE)
        {
            return false;
        }
        //消息处理
        return OnUserOpenCard(pUserData->wChairID);
        break;
    }
	case SUB_C_TRUSTEE:			//托管
    {
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_C_Trustee));
		if (wDataSize != sizeof(CMD_C_Trustee)) return false;

		CMD_C_Trustee *pUserTrustee = (CMD_C_Trustee *)pDataBuffer;

		tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();

		//m_bUserTrustee[pUserTrustee->wUserChairID] = pUserTrustee->bTrustee;
		CMD_S_Trustee Trustee;
		Trustee.bTrustee = pUserTrustee->bTrustee;
		Trustee.wTrusteeUser = pUserData->wChairID;

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(CMD_S_Trustee));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(CMD_S_Trustee));
        break;
    }
	case SUB_C_CHIPMODE:			//最大最小托管下注
    {			
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_C_ChipMode));
		if (wDataSize != sizeof(CMD_C_ChipMode)) return false;

		CMD_C_ChipMode *pChipMode = (CMD_C_ChipMode *)pDataBuffer;

		tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();

		CMD_S_ChipMode ChipMode;
		/*ChipMode.cbTrusteeMode = pChipMode->cbTrusteeMode;*/
		ChipMode.cbTrusteeMode = m_cbTrusteeMode;


		//m_pITableFrame->SendTableData(pUserData->wChairID, SUB_S_CHIPMODE, (void*)pDataBuffer, wDataSize);
		//m_pITableFrame->SendLookonData(pUserData->wChairID, SUB_S_CHIPMODE, (void*)pDataBuffer, wDataSize);
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CHIPMODE, &ChipMode, sizeof(ChipMode));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CHIPMODE, &ChipMode, sizeof(ChipMode));

        break;
    }
   }

    //操作定时器
    // 	if(bResult)
    // 	{
    // 		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
    // 		return true;
    // 	}

    return true;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	// 消息处理
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
          case SUB_C_AMDIN_COMMAND:
         {
             ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
             if(wDataSize != sizeof(CMD_C_AdminReq))
             {
                 return false;
             }
    
             ASSERT(pIServerUserItem->GetUserStatus() != GS_TK_PLAYING);
             if(pIServerUserItem->GetUserStatus() == GS_TK_PLAYING)
             {
                 return true;
             }
    
             if(m_pServerContro == NULL)
             {
                 return true;
             }
    
            // return m_pServerContro->ServerControl(wSubCmdID, pDataBuffer, wDataSize, pIServerUserItem, m_pITableFrame);
         }
	   }
	}
    return false;
}

//游戏人数
DWORD CTableFrameSink::GetPlayerCount()
{
    //计算人数
    WORD wPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        // IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(m_cbPlayStatus[i] == false)
        {
            continue;
        }
        //if(pIServerUserItem)
        {
            wPlayerCount++;
        }
    }

    return wPlayerCount;
}

//叫庄事件
bool CTableFrameSink::OnUserCallBanker(WORD wChairID, bool bBanker)
{
    //状态效验
    ASSERT(m_pITableFrame->GetGameStatus() == GS_TK_CALL);
    if(m_pITableFrame->GetGameStatus() != GS_TK_CALL)
    {
        return true;
    }

    //设置变量
    if(0 == m_nCallStatus[wChairID])
    {
        m_cbCallCount++;
    }
    //是否抢庄
    if(bBanker)
    {
        m_nCallStatus[wChairID] = 1;
    }
    else
    {
        m_nCallStatus[wChairID] = -1;
    }

    //设置变量
    CMD_S_CallBanker CallBanker;
    CallBanker.wCallBankerUser = wChairID;
    CallBanker.bCallBanker     = bBanker;

    //发送数据
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }
        m_pITableFrame->SendTableData(i, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));

    if(m_pGameVideo)
    {
        //m_pGameVideo->AddVideoData(SUB_S_CALL_BANKER, &CallBanker);
        m_pGameVideo->AddVideoData(SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker), true);
    }

    //下注开始
    if(m_cbCallCount == GetPlayerCount())
    {
        OnBeginAddScore();
    }

    return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore)
{
    //状态效验	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
    if(m_pITableFrame->GetGameStatus() != GS_TK_SCORE)
    {
        return true;
    }

    //金币效验
    if(m_cbPlayStatus[wChairID])
    {
        ASSERT(lScore > 0 && lScore <= m_cbBaseScore * 20);
        if(lScore <= 0 || lScore > m_cbBaseScore * 20)
        {
            return false;
        }
    }
    //     else //没下注玩家强退
    //     {
    //         ASSERT(lScore == 0);
    //         if(lScore != 0)
    //         {
    //             return false;
    //         }
    //     }

    if(lScore > 0L && wChairID != m_wBankerUser)
    {
        //设置变量
        m_cbAddCount++;
    }

    //下注金币
    m_lTableScore[wChairID] += lScore;

    //构造数据
    CMD_S_AddScore AddScore;
    AddScore.wAddScoreUser = wChairID;
    AddScore.lAddScoreCount = m_lTableScore[wChairID];

    //发送数据
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(!m_cbPlayStatus[i])
        {
            continue;
        }
        m_pITableFrame->SendTableData(i, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
    if(m_pGameVideo)
    {
        //m_pGameVideo->AddVideoData(SUB_S_ADD_SCORE, &AddScore);
        m_pGameVideo->AddVideoData(SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore), true);
    }

    //闲家全到
    if((m_cbAddCount + 1) == GetPlayerCount())
    {
        OnBeginSendCard();
    }

    return true;
}

//托管
bool CTableFrameSink::OnUsertrustee(WORD wChairID)
{
	//CTraceService::TraceString(TEXT("托管"), TraceLevel_Debug);
	return true;
}

//最大最小托管下注
bool CTableFrameSink::OnUserchipnode(WORD wChairID)
{
	//CTraceService::TraceString(TEXT("最大最小托管下注"), TraceLevel_Debug);
	return true;
}

//摊牌事件
bool CTableFrameSink::OnUserOpenCard(WORD wChairID)
{
    //状态效验
    ASSERT(m_pITableFrame->GetGameStatus() == GS_TK_PLAYING);
    if(m_pITableFrame->GetGameStatus() != GS_TK_PLAYING)
    {
        return true;
    }

    //状态效验
    ASSERT(wChairID < GAME_PLAYER);
    if(wChairID >= GAME_PLAYER)
    {
        return true;
    }

    ASSERT(!m_bOperation[wChairID]);
    if(m_bOperation[wChairID])
    {
        return true;
    }

    //设置变量
    m_cbOpenCount++;
    m_bOperation[wChairID] = true;

    //构造变量
    CMD_S_Open_Card OpenCard;
    ZeroMemory(&OpenCard, sizeof(OpenCard));

    //设置变量
    OpenCard.wPlayerID = wChairID;
    OpenCard.bOperation = m_bOperation[wChairID];
    for(int i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE)
        {
            continue;
        }
        OpenCard.cbType[i] = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
    }

    //发送数据
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }
        m_pITableFrame->SendTableData(i, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard), true);
    }

    //结束游戏
    if(m_cbOpenCount == GetPlayerCount())
    {
        //         CString strInfo;
        //         strInfo.Format(TEXT("   常规结束   OnUserOpenCard"));
        //         NcaTextOut(strInfo);

        return OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
    }

    return true;
}


void CTableFrameSink::AnalyseCard()
{
    try
    {
        //机器人数
        bool bIsAiBanker = false;
        WORD wAiCount = 0;
        WORD wPlayerCount = 0;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //获取用户
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserItem != NULL)
            {
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }
                if(pIServerUserItem->IsAndroidUser())
                {
                    wAiCount++ ;
                    if(!bIsAiBanker && i == m_wBankerUser)
                    {
                        bIsAiBanker = true;
                    }
                }
                wPlayerCount++;
            }
        }

        //全部机器
        if(wPlayerCount == wAiCount || wAiCount == 0)
        {
            return;
        }

        //扑克变量
        BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
        CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

        //排列扑克
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            m_GameLogic.SortCardList(cbUserCardData[i], MAX_COUNT);
        }

        //变量定义
        LONGLONG lAndroidScore = 0;

        //倍数变量
        BYTE cbCardTimes[GAME_PLAYER];
        ZeroMemory(cbCardTimes, sizeof(cbCardTimes));

        //查找倍数
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE)
            {
                cbCardTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_COUNT);
            }
        }

        //机器庄家
        if(bIsAiBanker)
        {
            //对比扑克
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //用户过滤
                if((i == m_wBankerUser) || (m_cbPlayStatus[i] == FALSE))
                {
                    continue;
                }

                //获取用户
                IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                //机器过滤
                if((pIServerUserItem != NULL) && (pIServerUserItem->IsAndroidUser()))
                {
                    continue;
                }

                //对比扑克
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_COUNT) == true)
                {
                    lAndroidScore -= cbCardTimes[i] * m_lTableScore[i];
                }
                else
                {
                    lAndroidScore += cbCardTimes[m_wBankerUser] * m_lTableScore[i];
                }
            }
        }
        else//用户庄家
        {
            //对比扑克
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //获取用户
                IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                //用户过滤
                if((i == m_wBankerUser) || (pIServerUserItem == NULL) || !(pIServerUserItem->IsAndroidUser()))
                {
                    continue;
                }

                //对比扑克
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_COUNT) == true)
                {
                    lAndroidScore += cbCardTimes[i] * m_lTableScore[i];
                }
                else
                {
                    lAndroidScore -= cbCardTimes[m_wBankerUser] * m_lTableScore[i];
                }
            }
        }

        //送分判断
        bool bGiving = false;
        if(m_lStockScore > m_lStorageMax && lAndroidScore > 0 && m_lStockScore - lAndroidScore > 0 && rand() % 100 < m_nStorageMul)
        {
            bGiving = true;
        }

        //库存判断
        bool bCon[] = {bGiving, ((m_lStockScore + lAndroidScore) < 0), false};
        bool bMaxOrMin[] = {!bIsAiBanker, bIsAiBanker, !bIsAiBanker};
        for(WORD j = 0; j < CountArray(bCon); j++)
        {
            if(!bCon[j])
            {
                continue;
            }
            //变量定义
            WORD wWinUser = INVALID_CHAIR;

            //判断最大
            bool bCardMax = bMaxOrMin[j];

            //查找数据
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //用户过滤
                if(m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                //设置用户
                if(wWinUser == INVALID_CHAIR)
                {
                    wWinUser = i;
                    continue;
                }

                //对比扑克
                if(m_GameLogic.CompareCard(cbUserCardData[i], m_cbHandCardData[wWinUser], MAX_COUNT) == bCardMax)
                {
                    wWinUser = i;
                }
            }

            //交换数据
            BYTE cbTempData[MAX_COUNT];
            CopyMemory(cbTempData, m_cbHandCardData[m_wBankerUser], MAX_COUNT);
            CopyMemory(m_cbHandCardData[m_wBankerUser], m_cbHandCardData[wWinUser], MAX_COUNT);
            CopyMemory(m_cbHandCardData[wWinUser], cbTempData, MAX_COUNT);
            if(bCon[j])
            {
                break;
            }
        }
    }
    catch(...)
    {
        CTraceService::TraceString(TEXT("AnalyseCard异常"), TraceLevel_Debug);
    }

    return;
}


//查询是否扣服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
    for(BYTE i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pUserItem == NULL)
        {
            continue;
        }

        if(m_bBuckleServiceCharge[i] && i == wChairID)
        {
            return true;
        }
    }
    return false;
}

bool CTableFrameSink::TryWriteTableScore(tagScoreInfo ScoreInfoArray[])
{
    //修改积分
    tagScoreInfo ScoreInfo[GAME_PLAYER];
    ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
    memcpy(&ScoreInfo, ScoreInfoArray, sizeof(ScoreInfo));
    //记录异常
    LONGLONG beforeScore[GAME_PLAYER];
    ZeroMemory(beforeScore, sizeof(beforeScore));
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pItem = m_pITableFrame->GetTableUserItem(i);
        if(pItem != NULL)
        {
            beforeScore[i] = pItem->GetUserScore();
        }
    }
    m_pITableFrame->WriteTableScore(ScoreInfo, CountArray(ScoreInfo));
    LONGLONG afterScore[GAME_PLAYER];
    ZeroMemory(afterScore, sizeof(afterScore));
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pItem = m_pITableFrame->GetTableUserItem(i);
        if(pItem != NULL)
        {
            afterScore[i] = pItem->GetUserScore();

            if(afterScore[i] < 0)
            {
                //异常写入日志
                CString strInfo;
                strInfo.Format(TEXT("[%s] 出现负分"), pItem->GetNickName());
                NcaTextOut(strInfo);

                strInfo.Format(TEXT("写分前分数：%I64d"), beforeScore[i]);
                NcaTextOut(strInfo);

                strInfo.Format(TEXT("写分信息：写入积分 %I64d，税收 %I64d"), ScoreInfoArray[i].lScore, ScoreInfoArray[i].lRevenue);
                NcaTextOut(strInfo);

                strInfo.Format(TEXT("写分后分数：%I64d"), afterScore[i]);
                NcaTextOut(strInfo);
            }
        }
    }
    return true;
}

//最大下分
SCORE CTableFrameSink::GetUserMaxTurnScore(WORD wChairID)
{
    SCORE lMaxTurnScore = 0L;
    if(wChairID == m_wBankerUser)
    {
        return 0;
    }
    //庄家积分
    IServerUserItem *pIBankerItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
    LONGLONG lBankerScore = 0L;
    if(pIBankerItem != NULL)
    {
        lBankerScore = pIBankerItem->GetUserScore();
    }

    IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
    LONGLONG lUserScore = 0L;
    if(pIUserItem != NULL)
    {
        lUserScore = pIUserItem->GetUserScore();
    }

    BYTE cbUserCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE && i != m_wBankerUser)
        {
            cbUserCount++;
        }
    }

    lMaxTurnScore = __min(lBankerScore / cbUserCount / MAX_TIMES, lUserScore / MAX_TIMES);

    return lMaxTurnScore;
}

//是否可加
bool CTableFrameSink::UserCanAddScore(WORD wChairID, LONGLONG lAddScore)
{
    //获取用户
    SCORE lScore = GetUserMaxTurnScore(wChairID);

    if(lAddScore > lScore)
    {
        return false;
    }

    return true;
}

//查询限额
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem *pIServerUserItem)
{
    SCORE consumeQuota = 0L;
    SCORE lMinTableScore = m_pGameServiceOption->lMinTableScore;
    if(m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
    {
        consumeQuota = pIServerUserItem->GetUserScore() - lMinTableScore;

    }
    return consumeQuota;
}

//开始下注
void CTableFrameSink::OnBeginAddScore()
{
    //设置状态
    m_pITableFrame->SetGameStatus(GS_TK_SCORE);
    m_pITableFrame->KillGameTimer(IDI_CALL_BANKER);

    //有人抢庄时定庄
    LONGLONG lMaxScore = 0L;
    for(int i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)
        {
            continue;
        }
        if(1 == m_nCallStatus[i] && pIServerUserItem->GetUserScore() > lMaxScore)
        {
            m_wBankerUser = i;
            lMaxScore = pIServerUserItem->GetUserScore();
        }
    }
    //无人抢庄则随机定庄
    if(m_wBankerUser == INVALID_CHAIR)
    {
        m_wBankerUser = rand() % m_wPlayerCount;
        while(m_pITableFrame->GetTableUserItem(m_wBankerUser) == NULL)
        {
            m_wBankerUser = (m_wBankerUser + 1) % m_wPlayerCount;
        }
    }

    m_wCurrentUser = INVALID_CHAIR;

    CString strInfo;
    strInfo.Format(TEXT("【%s】 开始下注 -----------当前庄家：%d"), m_pGameServiceOption->szServerName, m_wBankerUser);
    NcaTextOut(strInfo);

    //设置变量
    CMD_S_AddBegin AddBegin;
    AddBegin.wBankerUser = m_wBankerUser;
    AddBegin.lCellScore = m_cbBaseScore;

    //发送数据
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }
        m_pITableFrame->SendTableData(i, SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin), true);
    }

    //设置定时器
    m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, TIME_OPERATE, m_cbTimeAddScore, 0);

    return;
}

//开始发牌
void CTableFrameSink::OnBeginSendCard()
{
    //设置状态
    m_pITableFrame->SetGameStatus(GS_TK_PLAYING);
    m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
    //下注时间到，未进行下注的玩家处理
    for(int j = 0; j < m_wPlayerCount; j++)
    {
        if(m_lTableScore[j] > 0 || j == m_wBankerUser)
        {
            continue;
        }
        //下注金币
        m_lTableScore[j] += m_cbBaseScore;
        //m_bBuckleServiceCharge[j] = true;
    }

    //构造数据
    CMD_S_SendCard SendCard;
    ZeroMemory(SendCard.cbCardData, sizeof(SendCard.cbCardData));
    SendCard.cbUserCount = 0;
    //分析扑克
    AnalyseCard();

    //发送扑克
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE)
        {
            continue;
        }
        SendCard.cbUserCount++;
        //派发扑克
        CopyMemory(SendCard.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE)*MAX_COUNT);
    }

    bool bFirst = true;
    //发送数据
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }

        m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

        if(m_pGameVideo)
        {
            //m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard, bFirst);
            m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard, sizeof(SendCard), bFirst);
        }
        if(bFirst)
        {
            bFirst = false;
        }
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

    //设置定时器
    //if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) //房卡模式
    {
        m_pITableFrame->SetGameTimer(IDI_SEND_CARD, TIME_OPERATE, m_cbTimeOpenCard, 0);
    }

    return;
}

//开始比牌
void CTableFrameSink::OnBeginOpenCard()
{
    //设置状态
    m_pITableFrame->SetGameStatus(GS_TK_FREE);
    m_pITableFrame->KillGameTimer(IDI_SEND_CARD);

    OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

    return;
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

//////////////////////////////////////////////////////////////////////////
