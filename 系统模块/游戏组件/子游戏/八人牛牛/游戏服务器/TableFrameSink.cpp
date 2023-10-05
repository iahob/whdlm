#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <conio.h>
#include <locale>
#include "CLog.h"
#include <chrono>

DWORD	g_dwGroupID = 100001;
//////////////////////////////////////////////////////////////////////////

//房间玩家信息
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo;	//玩家USERID映射玩家信息

//通用全局变量
SCORE							g_lCurAnChouRate = 5;									//当前暗抽比例
SCORE							g_lCurAnChouTotal = 0;									//当前暗抽总数
SCORE							g_lStatisticSysWin = 0;									//统计(包括通用库存，房间牌桌调试中系统输赢和暗抽大奖)
SCORE							g_lStatisticService = 0;								//服务比例总数


//控制递增索引(调试ID)
DWORD							g_dwSysCtrlIncreaseIndex = 0;
DWORD							g_dwRoomCtrlIncreaseIndex = 0;
DWORD							g_dwTableCtrlIncreaseIndex = 0;

CList<SYSCTRL, SYSCTRL&>  CTableFrameSink::m_listSysCtrl;
CList<ROOMCTRL, ROOMCTRL&> CTableFrameSink::m_listRoomCtrl;
CList<TABLECTRL, TABLECTRL&> CTableFrameSink::m_listTableCtrl;

//////////////////////////////////////////////////////////////////////////

#define	IDI_SO_OPERATE							2							//代打定时器
#define	IDI_TIME_ELAPSE							12							//流逝定时器
#define	IDI_DELAY_GAMEFREE						13							//延迟空闲定时器

//前端所有的操作时间都缩短为10秒
#define	TIME_SO_OPERATE							12000						//代打定时器
#define	TIME_DELAY_GAMEFREE						6000						//延迟空闲定时器
//
#define TIME_CALLBANKER		12000
#define TIME_ADDSCORE		12000
#define	TIME_OPENCARD		12000
#define TIME_GAMEFREE		12000

#define	IDI_OFFLINE_TRUSTEE_0					3
#define	IDI_OFFLINE_TRUSTEE_1					4
#define	IDI_OFFLINE_TRUSTEE_2					5
#define	IDI_OFFLINE_TRUSTEE_3					6
#define	IDI_OFFLINE_TRUSTEE_4					7
#define	IDI_OFFLINE_TRUSTEE_5					8
#define	IDI_OFFLINE_TRUSTEE_6					9
#define	IDI_OFFLINE_TRUSTEE_7					10

//////////////////////////////////////////////////////////////////////////
#define LOG_TABLE(...)	LOG_DEBUG(L"桌子号:",m_pITableFrame->GetTableID(),##__VA_ARGS__)

CString GetFileDialogPath()
{
	CString strFileDlgPath;
	TCHAR szModuleDirectory[MAX_PATH];	//模块目录
	GetModuleFileName(AfxGetInstanceHandle(), szModuleDirectory, sizeof(szModuleDirectory));
	int nModuleLen = lstrlen(szModuleDirectory);
	int nProcessLen = lstrlen(AfxGetApp()->m_pszExeName) + lstrlen(TEXT(".EXE")) + 1;
	if (nModuleLen <= nProcessLen)
	{
		return TEXT("");
	}
	szModuleDirectory[nModuleLen - nProcessLen] = 0;
	strFileDlgPath = szModuleDirectory;
	return strFileDlgPath;
}

//构造函数
CTableFrameSink::CTableFrameSink()
{
    //游戏变量
    m_wPlayerCount = GAME_PLAYER;

    m_lExitScore = 0;

    m_cbTimeRemain = TIME_SO_OPERATE / 1000;
    m_wBankerUser = INVALID_CHAIR;
    m_wFirstEnterUser = INVALID_CHAIR;
    m_listEnterUser.RemoveAll();
    m_listCardTypeOrder.RemoveAll();
    m_bReNewTurn = true;
	ZeroMemory(m_tagGameScoreInfo, sizeof(m_tagGameScoreInfo));

    //用户状态
    ZeroMemory(m_cbDynamicJoin, sizeof(m_cbDynamicJoin));
    ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
    ZeroMemory(m_cbCallBankerStatus, sizeof(m_cbCallBankerStatus));
    ZeroMemory(m_cbCallBankerTimes, sizeof(m_cbCallBankerTimes));
    ZeroMemory(m_cbPrevCallBankerTimes, sizeof(m_cbPrevCallBankerTimes));

    ZeroMemory(m_bOpenCard, sizeof(m_bOpenCard));

    //扑克变量
    ZeroMemory(m_cbOriginalCardData, sizeof(m_cbOriginalCardData));
    ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
    ZeroMemory(m_bSpecialCard, sizeof(m_bSpecialCard));
    ZeroMemory(m_cbOriginalCardType, sizeof(m_cbOriginalCardType));
    ZeroMemory(m_cbCombineCardType, sizeof(m_cbCombineCardType));

    //下注信息
    ZeroMemory(m_lTurnMaxScore, sizeof(m_lTurnMaxScore));

    ZeroMemory(m_bBuckleServiceCharge, sizeof(m_bBuckleServiceCharge));

    //组件变量
    m_pITableFrame = NULL;
    m_pGameServiceOption = NULL;

    m_ctConfig = CT_ADDTIMES_;
    m_stConfig = ST_SENDFOUR_;
    m_gtConfig = GT_HAVEKING_;
    m_bgtConfig = BGT_ROB_;
    m_btConfig = BT_FREE_;
    m_tyConfig = BT_TUI_INVALID_;

    m_lFreeConfig[0] = 200;
    m_lFreeConfig[1] = 500;
    m_lFreeConfig[2] = 800;
    m_lFreeConfig[3] = 1100;
    m_lFreeConfig[4] = 1500;

    ZeroMemory(m_lPercentConfig, sizeof(m_lPercentConfig));

    m_lMaxCardTimes = 0;

    ZeroMemory(&m_stRecord, sizeof(m_stRecord));
    for(WORD i = 0; i < GAME_PLAYER; i++)
    {
        m_listWinScoreRecord[i].RemoveAll();
		m_listGameEndScore[i].RemoveAll();
    }

    m_cbTrusteeDelayTime = 3;

	m_wCtrlChairID = INVALID_CHAIR;
	m_bWin = false;
	ZeroMemory(&m_keyroomuserinfo, sizeof(m_keyroomuserinfo));
	m_keyroomuserinfo.tablectrl_type = TABLECTRL_INVALID;
	m_emCtrlType = SYS_CTRL;
	ZeroMemory(&m_CurSubStorageItem, sizeof(m_CurSubStorageItem));
	ZeroMemory(&m_ConfigSubStorageItem, sizeof(m_ConfigSubStorageItem));

	m_nTimeStart = 10;
	m_nTimeCallBanker = 10;
	m_nTimeAddScore = 10;
	m_nTimeOpenCard = 10;

    //g_ListRoomUserDebug.RemoveAll();
    //g_ListOperationRecord.RemoveAll();
    //ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

    //服务调试
    m_hInst = NULL;
    m_pServerDebug = NULL;
    m_hInst = LoadLibrary(TEXT("OxEightServerDebug.dll"));
    if(m_hInst)
    {
        typedef void *(*CREATE)();
        CREATE ServerDebug = (CREATE)GetProcAddress(m_hInst, "CreateServerDebug");
        if(ServerDebug)
        {
            m_pServerDebug = static_cast<IServerDebug *>(ServerDebug());
        }
    }

    //游戏视频
    m_hVideoInst = NULL;
    m_pGameVideo = NULL;
    m_hVideoInst = LoadLibrary(TEXT("OxEightGameVideo.dll"));
    if(m_hVideoInst)
    {
        typedef void *(*CREATE)();
        CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
        if(GameVideo)
        {
            m_pGameVideo = static_cast<IGameVideo *>(GameVideo());
        }
    }

    ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));

    //////////////////////////优化内容
    m_lBeBankerCondition = INVALID_DWORD;
    m_lPlayerBetTimes = INVALID_DWORD;
    m_cbAdmitRevCard = TRUE;
    m_cbMaxCallBankerTimes = 4;
    for(WORD i = 0; i < MAX_SPECIAL_CARD_TYPE; i++)
    {
        m_cbEnableCardType[i] = TRUE;
    }
    m_cbClassicTypeConfig = 0;

    m_lBgtDespotWinScore = 0L;
    m_wBgtRobNewTurnChairID = INVALID_CHAIR;
	m_cbRCOfflineTrustee = TRUE;

    ZeroMemory(m_bLastTurnBeBanker, sizeof(m_bLastTurnBeBanker));
    ZeroMemory(m_lLastTurnWinScore, sizeof(m_lLastTurnWinScore));
    ZeroMemory(m_bLastTurnBetBtEx, sizeof(m_bLastTurnBetBtEx));
    ZeroMemory(m_lPlayerBetBtEx, sizeof(m_lPlayerBetBtEx));

	ZeroMemory(&m_GameEndEx, sizeof(m_GameEndEx));

    srand(time(NULL));

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

	if (g_MapRoomUserInfo.GetCount() > 0)
	{
		g_MapRoomUserInfo.RemoveAll();
}

	if (m_listSysCtrl.GetCount() > 0)
	{
		m_listSysCtrl.RemoveAll();
	}

	if (m_listRoomCtrl.GetCount() > 0)
	{
		m_listRoomCtrl.RemoveAll();
	}

	if (m_listTableCtrl.GetCount() > 0)
	{
		m_listTableCtrl.RemoveAll();
	}
}

//接口查询--检测相关信息版本
void *CTableFrameSink::QueryInterface(const IID &Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
    QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
	QUERYINTERFACE(ITableShareStorage, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
    return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
    //查询接口
    ASSERT(pIUnknownEx != NULL);
    m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
    if(m_pITableFrame == NULL) { return false; }

    m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

    //游戏配置
    m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
    m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

    //读取配置
    ReadConfigInformation();

	//开始记录时间
	m_tmStartRecord = CTime::GetCurrentTime();

	//日志
	CString strDir = GetFileDialogPath() + TEXT("\\") + GAME_NAME + TEXT("日志");
	CreateDirectory(strDir, NULL);
	strDir.AppendFormat(TEXT("\\%s(%d)"), GAME_NAME, m_pITableFrame->GetGameServiceOption()->wServerID);
	EasyLog::CLog::m_log.SetName(strDir.GetBuffer());
	LOG_TABLE(TEXT("桌子初始化!"));
	
	//BYTE cb[MAX_CARDCOUNT] = { 0X1, 0X11, 0x21, 0x4, 0x14 };
	//int ret = m_TableDebug.GetCardHelper().GetType(cb);
	//配牌初始化

	//int a = 5;
	//BYTE cbArray[4] = { 0 };
	//CopyMemory(cbArray, &a, sizeof(a));

	//int a1 = -5;
	//BYTE cbArray1[4] = { 0 };
	//CopyMemory(cbArray, &a1, sizeof(a1));
#if 1
	//桌子启动即洗牌，配合调试器
	m_GameLogic.RandPool(m_gtConfig == GT_HAVEKING_ ? true : false);
	m_TableDebug.GetCardHelper().Read();
	m_TableDebug.RefreshCardPool(m_GameLogic.GetPoolData(), m_GameLogic.GetPoolLeftCount());
#else
	m_GameLogic.RandPool(false);
	m_TableDebug.GetCardHelper().RandPattern(m_GameLogic.GetPoolData(), m_GameLogic.GetPoolLeftCount());
	//m_TableDebug.GetCardHelper().RandPattern(m_GameLogic.GetPoolData(), 30);
	m_TableDebug.GetCardHelper().Write();
	CTraceService::TraceString(L"获取结果完成", enTraceLevel::TraceLevel_Debug);
#endif
	
	//共享库存
	//tagShareStorageRule * pShareStorageRule = (tagShareStorageRule*)(&(m_pGameServiceOption->cbShareStorageRule));

    return true;
}

//复位桌子
void CTableFrameSink::RepositionSink()
{
    //游戏变量
    m_lExitScore = 0;
    m_cbTimeRemain = TIME_SO_OPERATE / 1000;
	ZeroMemory(m_tagGameScoreInfo, sizeof(m_tagGameScoreInfo));

    //用户状态
    ZeroMemory(m_cbDynamicJoin, sizeof(m_cbDynamicJoin));
    ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
    ZeroMemory(m_bBuckleServiceCharge, sizeof(m_bBuckleServiceCharge));
    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
    ZeroMemory(m_cbCallBankerStatus, sizeof(m_cbCallBankerStatus));
    ZeroMemory(m_cbCallBankerTimes, sizeof(m_cbCallBankerTimes));

    ZeroMemory(m_bOpenCard, sizeof(m_bOpenCard));

    //扑克变量
    ZeroMemory(m_cbOriginalCardData, sizeof(m_cbOriginalCardData));
    ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
    ZeroMemory(m_bSpecialCard, sizeof(m_bSpecialCard));
    ZeroMemory(m_cbOriginalCardType, sizeof(m_cbOriginalCardType));
    ZeroMemory(m_cbCombineCardType, sizeof(m_cbCombineCardType));

    //下注信息
    ZeroMemory(m_lTurnMaxScore, sizeof(m_lTurnMaxScore));
    m_wBgtRobNewTurnChairID = INVALID_CHAIR;

    //重置推注变量
    ZeroMemory(m_lPlayerBetBtEx, sizeof(m_lPlayerBetBtEx));

	ZeroMemory(&m_GameEndEx, sizeof(m_GameEndEx));

	m_wCtrlChairID = INVALID_CHAIR;
	m_bWin = false;
	ZeroMemory(&m_keyroomuserinfo, sizeof(m_keyroomuserinfo));
	m_keyroomuserinfo.tablectrl_type = TABLECTRL_INVALID;
	m_emCtrlType = SYS_CTRL;
	ZeroMemory(&m_CurSubStorageItem, sizeof(m_CurSubStorageItem));
	ZeroMemory(&m_ConfigSubStorageItem, sizeof(m_ConfigSubStorageItem));

    return;
}

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
{
    //更新房间用户信息

    //UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);

    //设置托管定时器
    //金币场和金币房卡默认托管，积分房卡房卡创建界面可以勾选托管
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_cbRCOfflineTrustee == TRUE)
      || (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
    {
        pIServerUserItem->SetTrusteeUser(true);
        switch(m_pITableFrame->GetGameStatus())
        {
        case GS_TK_CALL:
        {
            if(m_cbCallBankerStatus[wChairID] == FALSE)
            {
                if(wChairID == m_wBgtRobNewTurnChairID && m_wBgtRobNewTurnChairID != INVALID_CHAIR)
                {
                    OnUserCallBanker(wChairID, true, 1);
                }
                else
                {
                    OnUserCallBanker(wChairID, false, m_cbPrevCallBankerTimes[wChairID]);
                }
            }
            break;
        }
        case GS_TK_SCORE:
        {
            if(m_lTableScore[wChairID] > 0 || wChairID == m_wBankerUser)
            {
                break;
            }
            if(m_lTurnMaxScore[wChairID] > 0)
            {
                if(m_btConfig == BT_FREE_)
                {
                    OnUserAddScore(wChairID, m_lFreeConfig[0] * m_pITableFrame->GetCellScore());
                }
                else if(m_btConfig == BT_PENCENT_)
                {
                    OnUserAddScore(wChairID, m_lTurnMaxScore[wChairID] * m_lPercentConfig[0] / 100);
                }
            }
            else
            {
                OnUserAddScore(wChairID, 1);
            }
            break;
        }
        case GS_TK_PLAYING:
        {
            if(m_bOpenCard[wChairID] == false)
            {
                //获取牛牛牌型
                BYTE cbTempHandCardData[MAX_CARDCOUNT];
                ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
                CopyMemory(cbTempHandCardData, m_cbHandCardData[wChairID], sizeof(m_cbHandCardData[wChairID]));

                m_GameLogic.GetOxCard(cbTempHandCardData, MAX_CARDCOUNT);
                OnUserOpenCard(wChairID, cbTempHandCardData);
            }
            break;
        }
        default:
            break;
        }
    }

    return true;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //历史积分
    if(bLookonUser == false) { m_HistoryScore.OnEventUserEnter(pIServerUserItem->GetChairID()); }

    if(m_pITableFrame->GetGameStatus() != GS_TK_FREE)
    {
        m_cbDynamicJoin[pIServerUserItem->GetChairID()] = TRUE;
    }

    //更新房间用户信息
    UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

    //更新同桌用户调试

   // UpdateUserDebug(pIServerUserItem);

    //霸王庄 首进玩家
    if(m_wFirstEnterUser == INVALID_CHAIR && m_bgtConfig == BGT_DESPOT_)
    {
        m_wFirstEnterUser = wChairID;
    }

    m_listEnterUser.AddTail(wChairID);

	m_listGameEndScore[wChairID].RemoveAll();

    return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //历史积分
    if(bLookonUser == false)
    {
        m_HistoryScore.OnEventUserLeave(pIServerUserItem->GetChairID());
        m_cbDynamicJoin[pIServerUserItem->GetChairID()] = FALSE;
    }

    //更新房间用户信息
    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

	//移除牌桌调试庄家类型
	RemoveTableCtrlBankerType(pIServerUserItem);

    //非房卡房间
    if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
    {
        if(m_bgtConfig == BGT_NIUNIU_ || m_bgtConfig == BGT_NONIUNIU_)
        {
            if(wChairID == m_wBankerUser)
            {
                m_wBankerUser = INVALID_CHAIR;
            }
        }
    }

    POSITION ptListHead = m_listEnterUser.GetHeadPosition();
    POSITION ptTemp;

    //删除离开玩家
    while(ptListHead)
    {
        ptTemp = ptListHead;
        if(m_listEnterUser.GetNext(ptListHead) == wChairID)
        {
            m_listEnterUser.RemoveAt(ptTemp);
            break;
        }
    }

    ptListHead = m_listCardTypeOrder.GetHeadPosition();

    //删除离开玩家
    while(ptListHead)
    {
        ptTemp = ptListHead;
        if(m_listCardTypeOrder.GetNext(ptListHead) == wChairID)
        {
            m_listCardTypeOrder.RemoveAt(ptTemp);
            break;
        }
    }

    //房卡模式
    if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
    {
        //当前约战解散清理记录
        if(m_pITableFrame->IsPersonalRoomDisumme())
        {
            ZeroMemory(&m_stRecord, sizeof(m_stRecord));
            ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_wFirstEnterUser = INVALID_CHAIR;

            if(!m_listEnterUser.IsEmpty())
            {
                m_listEnterUser.RemoveAll();
            }

            m_wBankerUser = INVALID_CHAIR;
            m_lBgtDespotWinScore = 0L;

            //重置推注变量
            ZeroMemory(m_bLastTurnBeBanker, sizeof(m_bLastTurnBeBanker));
            ZeroMemory(m_lLastTurnWinScore, sizeof(m_lLastTurnWinScore));
            ZeroMemory(m_bLastTurnBetBtEx, sizeof(m_bLastTurnBetBtEx));
            ZeroMemory(m_lPlayerBetBtEx, sizeof(m_lPlayerBetBtEx));
        }
    }
    //非房卡房间
    else
    {
        BYTE bUserCount = 0;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == wChairID)
            {
                continue;
            }

            if(m_pITableFrame->GetTableUserItem(i))
            {
                bUserCount++;
            }
        }

        if(bUserCount == 0 && !m_listEnterUser.IsEmpty())
        {
            m_listEnterUser.RemoveAll();
        }
    }

    if(m_bgtConfig == BGT_NIUNIU_)
    {
        m_bReNewTurn = m_listCardTypeOrder.IsEmpty();

        if(!m_listCardTypeOrder.IsEmpty())
        {
            m_wBankerUser = m_listCardTypeOrder.GetHead();
        }
    }
    else
    {
        m_bReNewTurn = true;
    }

    //金币房卡
    if((m_pITableFrame->GetDataBaseMode() == 1) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
    {
        m_listWinScoreRecord[wChairID].RemoveAll();
    }

    m_bLastTurnBeBanker[wChairID] = false;
    m_lLastTurnWinScore[wChairID] = 0;
    m_bLastTurnBetBtEx[wChairID] = false;
    m_lPlayerBetBtEx[wChairID] = 0;

	m_listGameEndScore[wChairID].RemoveAll();

    return true;
}

//用户同意
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize)
{
    //私人房设置游戏模式
    if(((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0)
    {
        //cbGameRule[1] 为  2345678分别对应桌子最大人数
		if (m_pITableFrame->GetStartMode() != START_MODE_ALL_READY)
		{
			m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
		}
    }

	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (!pIServerUserItem->IsClientReady() || !pIServerUserItem->IsAndroidUser())
		{
			continue;
		}

		m_pITableFrame->SendTableData(i, SUB_S_ANDROID_READY);
	}

    return true;
}

//校验共享库存
bool CTableFrameSink::EfficacyConfigSubStorage(tagShareStorageItem *ptagShareStorageItem, TCHAR szResultDescripe[512])
{
	bool bIsGroupTreasureRoom = (CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule)) && (m_pITableFrame->GetDataBaseMode() == 1);

	if (!bIsGroupTreasureRoom)
	{
		TCHAR szString[32] = { TEXT("校验无效,房间类型错误") };
		CopyMemory(szResultDescripe, szString, sizeof(szResultDescripe));

		CString strTrace;
		strTrace.Format(TEXT("groupid=%d,bsupport=%d,datebase=%d"), m_pITableFrame->GetGroupID(), CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule), m_pITableFrame->GetDataBaseMode());
		CTraceService::TraceString(strTrace, enTraceLevel::TraceLevel_Debug);

		return false;
	}
	else if (bIsGroupTreasureRoom && !(ptagShareStorageItem->emModifySubItemType == _MODIFY_SUBSTORAGE_ || ptagShareStorageItem->emModifySubItemType == _MODIFY_SUBTEMPSTORAGE_))
	{
		TCHAR szString[32] = { TEXT("校验成功") };
		CopyMemory(szResultDescripe, szString, sizeof(szResultDescripe));

		return true;
	}

	//八人牛牛校验玩家库存
	//(底分 x 最大下注倍数 x 最大抢庄倍数 x 最大牌型倍数 x (GAME_PLAYER - 1))

	//变量定义
	tagCustomRule *pCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();
	BYTE cbMaxCardTypeTimes = INVALID_BYTE;
	BYTE cbMaxCallBankerTimes = 4;
	LONG cbMaxBetTimes = pCustomRule->lFreeConfig[MAX_CONFIG - 2];
	LONG lCellScore = m_pITableFrame->GetCellScore();

	if (m_ctConfig == CT_CLASSIC_)
	{
		cbMaxCardTypeTimes = pCustomRule->cbCardTypeTimesClassic[0];
		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			if (pCustomRule->cbCardTypeTimesClassic[i] > cbMaxCardTypeTimes)
			{
				cbMaxCardTypeTimes = pCustomRule->cbCardTypeTimesClassic[i];
			}
		}
	}
	else if (m_ctConfig == CT_ADDTIMES_)
	{
		cbMaxCardTypeTimes = pCustomRule->cbCardTypeTimesAddTimes[0];
		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			if (pCustomRule->cbCardTypeTimesAddTimes[i] > cbMaxCardTypeTimes)
			{
				cbMaxCardTypeTimes = pCustomRule->cbCardTypeTimesAddTimes[i];
			}
		}
	}
	ASSERT(cbMaxCardTypeTimes != INVALID_BYTE);

	SCORE lConfigPlayerStorage = ptagShareStorageItem->ConfigSubStorageItemInfo.lConfigPlayerStorage;
	SCORE lConfigTempPlayerStorage = ptagShareStorageItem->ConfigSubStorageItemInfo.lConfigTempPlayerStorage;
	SCORE lMinConfigPlayerStorage = lCellScore * cbMaxBetTimes * cbMaxCallBankerTimes * cbMaxCardTypeTimes * (GAME_PLAYER - 1);

	if (ptagShareStorageItem->emModifySubItemType == _MODIFY_SUBSTORAGE_ && lConfigPlayerStorage < lMinConfigPlayerStorage)
	{
		CString strTip;
		strTip.Format(TEXT("配置玩家库存至少大于 %I64d"), lMinConfigPlayerStorage);
		CopyMemory(szResultDescripe, strTip, sizeof(TCHAR)* strTip.GetLength());

		return false;
	}
	else if (ptagShareStorageItem->emModifySubItemType == _MODIFY_SUBTEMPSTORAGE_ && lConfigTempPlayerStorage < lMinConfigPlayerStorage)
	{
		CString strTip;
		strTip.Format(TEXT("配置临时玩家库存至少大于 %I64d"), lMinConfigPlayerStorage);
		CopyMemory(szResultDescripe, strTip, sizeof(TCHAR)* strTip.GetLength());

		return false;
	}

	TCHAR szString[32] = { TEXT("校验成功") };
	CopyMemory(szResultDescripe, szString, sizeof(szResultDescripe));

	return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	tagGameStaus	GameStaus;
	GameStaus.cbStatus = GAME_START;
	//用户状态
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL)
		{
			m_pITableFrame->SendTableData(i, SUB_S_GAME_STAUTS, &GameStaus, sizeof(GameStaus));
		}
	}

	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_STAUTS, &GameStaus, sizeof(GameStaus));

    //重置推注变量
    ZeroMemory(m_lPlayerBetBtEx, sizeof(m_lPlayerBetBtEx));

    if(m_pGameVideo)
    {
        m_pGameVideo->StartVideo(m_pITableFrame, m_wPlayerCount);

		Viedeo_GameConfig videoGameConfig;
		ZeroMemory(&videoGameConfig, sizeof(videoGameConfig));
		for (int i = 0; i < MAX_CARD_TYPE; ++i)
			videoGameConfig.wCardTypeTimes[i] = m_GameLogic.m_cbCardTypeTimes[i];
		m_pGameVideo->AddVideoData(SUB_S_VIDEO_START, &videoGameConfig);
    }

	//牌型最大倍数
	tagCustomRule *pCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();
	m_lMaxCardTimes = INVALID_BYTE;

	if (pCustomRule->ctConfig == CT_CLASSIC_)
	{
		m_lMaxCardTimes = pCustomRule->cbCardTypeTimesClassic[0];
		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			if (pCustomRule->cbCardTypeTimesClassic[i] > m_lMaxCardTimes)
			{
				m_lMaxCardTimes = pCustomRule->cbCardTypeTimesClassic[i];
			}
		}
	}
	else if (pCustomRule->ctConfig == CT_ADDTIMES_)
	{
		m_lMaxCardTimes = pCustomRule->cbCardTypeTimesAddTimes[0];
		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			if (pCustomRule->cbCardTypeTimesAddTimes[i] > m_lMaxCardTimes)
			{
				m_lMaxCardTimes = pCustomRule->cbCardTypeTimesAddTimes[i];
			}
		}
	}
	ASSERT(m_lMaxCardTimes != INVALID_BYTE);

	//设置牌型倍数

	if (m_cbClassicTypeConfig == INVALID_BYTE)
	{
		m_GameLogic.SetCardTypeTimes(pCustomRule->cbCardTypeTimesAddTimes);
	}
	else if (m_cbClassicTypeConfig == 0)
	{
		//牌型倍数
		m_GameLogic.SetCardTypeTimes(pCustomRule->cbCardTypeTimesClassic);
	}
	else if (m_cbClassicTypeConfig == 1)
	{
		//牌型倍数
		m_GameLogic.SetCardTypeTimes(pCustomRule->cbCardTypeTimesClassic);
	}

	//设置牌型激活
	m_GameLogic.SetEnableCardType(m_cbEnableCardType);

    //用户状态
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //获取用户
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
        {
            m_cbPlayStatus[i] = FALSE;
        }
        else
        {
			if (pIServerUserItem->GetUserStatus() <= US_READY)
			{
				m_cbDynamicJoin[i] = TRUE;
				m_cbPlayStatus[i] = FALSE;
			}
			else if (pIServerUserItem->GetUserStatus() == US_OFFLINE)
			{
				m_cbPlayStatus[i] = FALSE;
			}
			else
			{
				m_cbPlayStatus[i] = TRUE;
			}
        }
    }

    CopyMemory(m_RoomCardRecord.cbPlayStatus, m_cbPlayStatus, sizeof(m_RoomCardRecord.cbPlayStatus));

	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
    //随机扑克
	m_GameLogic.RandPool(m_gtConfig == GT_HAVEKING_ ? true : false);
	
	m_emCtrlType = GetUserCtrlType(m_wCtrlChairID, m_bWin, m_keyroomuserinfo);

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUser == NULL && m_cbDynamicJoin[i])
        {
            continue;
        }
		if (m_cbDynamicJoin[i] || m_cbPlayStatus[i] == FALSE)
			continue;

		BYTE cbSendCount = 0;
		stServerConfig* pCurUserConfig = m_TableDebug.GetUserConfig(i);

		if (pCurUserConfig != nullptr && pCurUserConfig->stConfig.cbType == 2 && m_emCtrlType == CLIENT_CARDCONFIG_CTRL)
		{
			WORD wDebugChair = pCurUserConfig->stConfig.wDebugUserChair;
			memcpy(m_cbHandCardData[wDebugChair], pCurUserConfig->stConfig.cbGameCards, pCurUserConfig->nCurConfigCardCount*sizeof(BYTE));
			m_TableDebug.GetDebugUser(wDebugChair)->Lock();
			m_GameLogic.RemoveCards(m_cbHandCardData[wDebugChair], pCurUserConfig->nCurConfigCardCount);
			m_TableDebug.m_bDebugDoneChair[wDebugChair] = true;
			cbSendCount = pCurUserConfig->nCurConfigCardCount;
		}
		else
			m_GameLogic.GetCards(m_cbHandCardData[i] + cbSendCount, MAX_CARDCOUNT - cbSendCount);
    }


	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUser == NULL && m_cbDynamicJoin[i])
		{
			continue;
		}
		if (m_cbDynamicJoin[i] || m_cbPlayStatus[i] == FALSE)
			continue;

		//发送剩余手牌
		stServerConfig* pCurUserConfig = m_TableDebug.GetUserConfig(i);
		if (pCurUserConfig != nullptr && pCurUserConfig->stConfig.cbType == 2 && pCurUserConfig->nCurConfigCardCount != MAX_CARDCOUNT && m_emCtrlType == CLIENT_CARDCONFIG_CTRL)
		{
			WORD wDebugChair = pCurUserConfig->stConfig.wDebugUserChair;
			m_GameLogic.GetCards(m_cbHandCardData[i] + pCurUserConfig->nCurConfigCardCount, MAX_CARDCOUNT - pCurUserConfig->nCurConfigCardCount);
		}

		CString str;
		str.Format(L"[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]", m_cbHandCardData[i][0], m_cbHandCardData[i][1], m_cbHandCardData[i][2], m_cbHandCardData[i][3], m_cbHandCardData[i][4]);
		LOG_TABLE(L"玩家:", (int)i, L"游戏开始,手牌:", str);
		//
		m_TableDebug.GetDebugUser(i)->Init(m_cbHandCardData[i], m_stConfig == ST_SENDFOUR_ ? 4 : MAX_CARDCOUNT);
	}

	//
	m_TableDebug.RefreshCardPool(m_GameLogic.GetPoolData(), m_GameLogic.GetPoolLeftCount());

    //临时扑克
    BYTE cbTempHandCardData[GAME_PLAYER][MAX_CARDCOUNT];
    ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
    CopyMemory(cbTempHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUser == NULL || pIServerUser != NULL && pIServerUser->GetUserStatus() <= US_READY)
        {
            continue;
        }

        m_bSpecialCard[i] = (m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig) > CT_CLASSIC_OX_VALUENIUNIU ? true : false);

        //特殊牌型
        if(m_bSpecialCard[i])
        {
            m_cbOriginalCardType[i] = m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
        }
        else
        {
            //获取牛牛牌型
            m_GameLogic.GetOxCard(cbTempHandCardData[i], MAX_CARDCOUNT);

            m_cbOriginalCardType[i] = m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
        }
    }

    //获取坐庄模式
    ASSERT(m_bgtConfig != BGT_INVALID_);

    bool bRoomServerType = ((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0;

    //庄家配置
    switch(m_bgtConfig)
    {
    //霸王庄
    case BGT_DESPOT_:
    {
        //初始默认庄家
        InitialBanker();

        //霸王庄模式下 m_lBeBankerCondition为INVALID_DWORD表明设置无，其他模式下没有该选项也为INVALID_DWORD

        break;
    }
    //倍数抢庄
    case BGT_ROB_:
    {
        {
            m_wBgtRobNewTurnChairID = INVALID_CHAIR;

            //非房卡场设置定时器
            if(!IsRoomCardType())
            {
                m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
				m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, (m_nTimeCallBanker + 2) * 1000, 1, 0);
            }

            //设置离线代打定时器
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
                {
                    m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
                }
            }

            //设置状态
            m_pITableFrame->SetGameStatus(GS_TK_CALL);
			EnableTimeElapse(true, TIME_CALLBANKER);

            CMD_S_CallBanker CallBanker;
            ZeroMemory(&CallBanker, sizeof(CallBanker));
            CallBanker.ctConfig = m_ctConfig;
            CallBanker.stConfig = m_stConfig;
            CallBanker.bgtConfig = m_bgtConfig;

            BYTE *pGameRule = m_pITableFrame->GetGameRule();
            CallBanker.wGamePlayerCountRule = pGameRule[1];
            CallBanker.cbMaxCallBankerTimes = m_cbMaxCallBankerTimes;
            CallBanker.wBgtRobNewTurnChairID = m_wBgtRobNewTurnChairID;


            //发送数据
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] != TRUE)
                {
                    continue;
                }
                m_pITableFrame->SendTableData(i, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));
            }
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));

            if(m_pGameVideo)
            {
                m_pGameVideo->AddVideoData(SUB_S_CALL_BANKER, &CallBanker);
            }

            //发四等五
            if(m_stConfig == ST_SENDFOUR_)
            {
                //设置变量
                CMD_S_SendFourCard SendFourCard;
                ZeroMemory(&SendFourCard, sizeof(SendFourCard));

                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE)
                    {
                        continue;
                    }

                    //派发扑克(开始只发四张牌)
                    CopyMemory(SendFourCard.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
                }

				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (m_cbPlayStatus[i] == FALSE)
					{
						continue;
					}

					m_pITableFrame->SendTableData(i, SUB_S_SEND_FOUR_CARD, &SendFourCard, sizeof(SendFourCard));
					m_pITableFrame->SendLookonData(i, SUB_S_SEND_FOUR_CARD, &SendFourCard, sizeof(SendFourCard));
				}

				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** 游戏开始 发四张牌 发送消息 wSubCmdID = %d"), SUB_S_SEND_FOUR_CARD);
				OutputDebugString(szInfo);

            }
        }

        break;
    }
    //牛牛上庄
    //无牛下庄
    case BGT_NIUNIU_:
    case BGT_NONIUNIU_:
    {
        //新开一局
        if(m_bReNewTurn == true)
        {
            //初始默认庄家
            InitialBanker();
        }
        else
        {
            ASSERT(m_wBankerUser != INVALID_CHAIR);

            m_bBuckleServiceCharge[m_wBankerUser] = true;

            //非房卡场设置定时器
            if(!IsRoomCardType())
            {
                m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
				m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, (m_nTimeAddScore + 2) * 1000, 1, 0);
            }

            //设置离线代打定时器
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
                {
                    m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
                }
            }

            //设置状态
            m_pITableFrame->SetGameStatus(GS_TK_SCORE);
			EnableTimeElapse(true, TIME_ADDSCORE);


            //获取最大下注
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] != TRUE || i == m_wBankerUser)
                {
                    continue;
                }

                //下注变量
                m_lTurnMaxScore[i] = GetUserMaxTurnScore(i);
            }

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                if(m_bLastTurnBetBtEx[i] == true)
                {
                    m_bLastTurnBetBtEx[i] = false;
                }
            }

            m_lPlayerBetBtEx[m_wBankerUser] = 0;

            //设置变量
            CMD_S_GameStart GameStart;
            ZeroMemory(&GameStart, sizeof(GameStart));
            GameStart.wBankerUser = m_wBankerUser;
            CopyMemory(GameStart.cbPlayerStatus, m_cbPlayStatus, sizeof(m_cbPlayStatus));

            //发四等五
            if(m_stConfig == ST_SENDFOUR_)
            {
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                    {
                        continue;
                    }

                    //派发扑克(开始只发四张牌)
                    CopyMemory(GameStart.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);

					if (m_cbDynamicJoin[i] == TRUE)
					{
						ZeroMemory(GameStart.cbCardData[i], MAX_CARDCOUNT);
					}
                }
            }

            GameStart.stConfig = m_stConfig;
            GameStart.bgtConfig = m_bgtConfig;
            GameStart.btConfig = m_btConfig;
            GameStart.gtConfig = m_gtConfig;

            CopyMemory(GameStart.lFreeConfig, m_lFreeConfig, sizeof(GameStart.lFreeConfig));
            CopyMemory(GameStart.lPercentConfig, m_lPercentConfig, sizeof(GameStart.lPercentConfig));
            CopyMemory(GameStart.lPlayerBetBtEx, m_lPlayerBetBtEx, sizeof(GameStart.lPlayerBetBtEx));

            bool bFirstRecord = true;

            WORD wRealPlayerCount = 0;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }

                if(!pServerUserItem)
                {
                    continue;
                }

                wRealPlayerCount++;
            }

            BYTE *pGameRule = m_pITableFrame->GetGameRule();

            //最大下注
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }
                GameStart.lTurnMaxScore = m_lTurnMaxScore[i];
                m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

				//更新房间用户信息
				UpdateRoomUserInfo(pServerUserItem, USER_NULL);

                if(m_pGameVideo)
                {
                    Video_GameStart video;
                    ZeroMemory(&video, sizeof(video));
                    video.lCellScore = m_pITableFrame->GetCellScore();
                    video.wPlayerCount = wRealPlayerCount;
                    video.wGamePlayerCountRule = pGameRule[1];
                    video.wBankerUser = GameStart.wBankerUser;
                    CopyMemory(video.cbPlayerStatus, GameStart.cbPlayerStatus, sizeof(video.cbPlayerStatus));
                    video.lTurnMaxScore = GameStart.lTurnMaxScore;
                    CopyMemory(video.cbCardData, GameStart.cbCardData, sizeof(video.cbCardData));
                    video.ctConfig = m_ctConfig;
                    video.stConfig = GameStart.stConfig;
                    video.bgtConfig = GameStart.bgtConfig;
                    video.btConfig = GameStart.btConfig;
                    video.gtConfig = GameStart.gtConfig;

                    CopyMemory(video.lFreeConfig, GameStart.lFreeConfig, sizeof(video.lFreeConfig));
                    CopyMemory(video.lPercentConfig, GameStart.lPercentConfig, sizeof(video.lPercentConfig));
                    CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
                    video.wChairID = i;
                    video.lScore = pServerUserItem->GetUserScore();
                    m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video,bFirstRecord);

                    if(bFirstRecord == true)
                    {
                        bFirstRecord = false;
                    }
                }
            }
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
        }

        break;
    }
    //自由抢庄
    case BGT_FREEBANKER_:
    {
        //非房卡场设置定时器
        if(!IsRoomCardType())
        {
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
			m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, (m_nTimeCallBanker + 2) * 1000, 1, 0);
        }

        //设置离线代打定时器
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
            }
        }

        //设置状态
        m_pITableFrame->SetGameStatus(GS_TK_CALL);
		EnableTimeElapse(true, TIME_CALLBANKER);

        CMD_S_CallBanker CallBanker;
        ZeroMemory(&CallBanker, sizeof(CallBanker));
        CallBanker.ctConfig = m_ctConfig;
        CallBanker.stConfig = m_stConfig;
        CallBanker.bgtConfig = m_bgtConfig;

        BYTE *pGameRule = m_pITableFrame->GetGameRule();
        CallBanker.wGamePlayerCountRule = pGameRule[1];
        CallBanker.cbMaxCallBankerTimes = m_cbMaxCallBankerTimes;
        CallBanker.wBgtRobNewTurnChairID = m_wBgtRobNewTurnChairID;


        //发送数据
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] != TRUE)
            {
                continue;
            }
            m_pITableFrame->SendTableData(i, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_CALL_BANKER, &CallBanker);
        }

        break;
    }
    //通比玩法
    case BGT_TONGBI_:
    {
        bool bFirstRecord = true;

        WORD wRealPlayerCount = 0;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            if(!pServerUserItem)
            {
                continue;
            }

            wRealPlayerCount++;
        }

        BYTE *pGameRule = m_pITableFrame->GetGameRule();

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            if(m_pGameVideo)
            {
                Video_GameStart video;
                ZeroMemory(&video, sizeof(video));
                video.lCellScore = m_pITableFrame->GetCellScore();
                video.wPlayerCount = wRealPlayerCount;
                video.wGamePlayerCountRule = pGameRule[1];
                video.wBankerUser = INVALID_CHAIR;// 通比玩家没有庄家
                CopyMemory(video.cbPlayerStatus, m_cbPlayStatus, sizeof(video.cbPlayerStatus));
                video.lTurnMaxScore = 0;
                CopyMemory(video.cbCardData, m_cbHandCardData[i], sizeof(video.cbCardData));
                video.ctConfig = m_ctConfig;
                video.stConfig = m_stConfig;
                video.bgtConfig = m_bgtConfig;
                video.btConfig = m_btConfig;
                video.gtConfig = m_gtConfig;

                CopyMemory(video.lFreeConfig, m_lFreeConfig, sizeof(video.lFreeConfig));
                CopyMemory(video.lPercentConfig, m_lPercentConfig, sizeof(video.lPercentConfig));
                CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
                video.wChairID = i;
                video.lScore = pServerUserItem->GetUserScore();
				//for (int i = 0; i < MAX_CARD_TYPE; ++i)
				//	video.wCardTypeTimes[i] = m_GameLogic.m_cbCardTypeTimes[i];
                m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video, bFirstRecord);

                if(bFirstRecord == true)
                {
                    bFirstRecord = false;
                }
            }
        }

        m_wBankerUser = INVALID_CHAIR;

        //非房卡场设置定时器
        if(!IsRoomCardType())
        {
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
			m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, (m_nTimeAddScore + 2) * 1000, 1, 0);
        }

        //设置离线代打定时器
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
            }
        }

        //设置状态
        m_pITableFrame->SetGameStatus(GS_TK_PLAYING);
		EnableTimeElapse(true, TIME_ADDSCORE);

        //更新房间用户信息
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //获取用户
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            if(m_cbPlayStatus[i] == TRUE)
            {
                //下注金币(通比玩家下底注)
                m_lTableScore[i] = m_pITableFrame->GetCellScore();
                m_bBuckleServiceCharge[i] = true;
            }
        }

        //构造数据
        CMD_S_SendCard SendCard;
        ZeroMemory(SendCard.cbCardData, sizeof(SendCard.cbCardData));

        //临时扑克,因为分析和调试扑克，重算原始牌型
        BYTE cbTempHandCardData[GAME_PLAYER][MAX_CARDCOUNT];
        ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
        CopyMemory(cbTempHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUser == NULL)
            {
                continue;
            }

            m_bSpecialCard[i] = (m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig) > CT_CLASSIC_OX_VALUENIUNIU ? true : false);

            //特殊牌型
            if(m_bSpecialCard[i])
            {
                m_cbOriginalCardType[i] = m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
            }
            else
            {
                //获取牛牛牌型
                m_GameLogic.GetOxCard(cbTempHandCardData[i], MAX_CARDCOUNT);

                m_cbOriginalCardType[i] = m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
            }
        }

        CopyMemory(SendCard.cbCardData, m_cbHandCardData, sizeof(SendCard.cbCardData));
        CopyMemory(SendCard.bSpecialCard, m_bSpecialCard, sizeof(SendCard.bSpecialCard));
        CopyMemory(SendCard.cbOriginalCardType, m_cbOriginalCardType, sizeof(SendCard.cbOriginalCardType));

        //发送数据
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
			if (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard);
        }

        break;
    }
    default:
        break;
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
		//牌池重置
		m_GameLogic.RandPool(m_gtConfig == GT_HAVEKING_ ? true : false);
		m_TableDebug.RefreshCardPool(m_GameLogic.GetPoolData(), m_GameLogic.GetPoolLeftCount());
		m_TableDebug.OnEnd();
        //构造数据
        CMD_S_GameEnd GameEnd = { 0 };

        //发送信息
        m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

        //结束游戏
		EnableTimeElapse(false, TIME_GAMEFREE);
        m_pITableFrame->ConcludeGame(GS_TK_FREE);

        if(!IsRoomCardType())
        {
            //删除时间
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

            //删除离线代打定时器
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
            }
        }

        m_wBankerUser = INVALID_CHAIR;
        m_wFirstEnterUser = INVALID_CHAIR;

        //房卡模式
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            //解散清理记录
            if(m_pITableFrame->IsPersonalRoomDisumme())
            {
                ZeroMemory(&m_stRecord, sizeof(m_stRecord));
                m_lBgtDespotWinScore = 0L;
            }
        }

        //更新房间用户信息
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //获取用户
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            if(!pIServerUserItem)
            {
                continue;
            }

            UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
        }

        m_bReNewTurn = true;
        return true;
    }
    case GER_NORMAL:		//常规结束
    {
        //定义变量
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd, sizeof(GameEnd));
		//牌池重置
		m_GameLogic.RandPool(m_gtConfig == GT_HAVEKING_ ? true : false);
		m_TableDebug.RefreshCardPool(m_GameLogic.GetPoolData(), m_GameLogic.GetPoolLeftCount());
		m_TableDebug.OnEnd();
        //保存扑克
        BYTE cbUserCardData[GAME_PLAYER][MAX_CARDCOUNT];
        CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(cbUserCardData));

        //赋值最后一张牌
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            GameEnd.cbLastSingleCardData[i] = m_cbOriginalCardData[i][4];
        }

        WORD wWinTimes[GAME_PLAYER];

        //非通比玩法
        if(m_bgtConfig != BGT_TONGBI_)
        {
            WORD wWinCount[GAME_PLAYER];
            ZeroMemory(wWinCount, sizeof(wWinCount));
            ZeroMemory(wWinTimes, sizeof(wWinTimes));

            //倍数抢庄 结算需要乘以cbMaxCallBankerTimes
            BYTE cbMaxCallBankerTimes = 1;
            if(m_bgtConfig == BGT_ROB_)
            {
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxCallBankerTimes)
                    {
                        cbMaxCallBankerTimes = m_cbCallBankerTimes[i];
                    }
                }
            }

            //庄家倍数
            wWinTimes[m_wBankerUser] = m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[m_wBankerUser]);

            //对比玩家
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE) { continue; }

                //对比扑克
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[i], m_cbCombineCardType[m_wBankerUser]))
                {
                    wWinCount[i]++;
                    //获取倍数
                    wWinTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[i]);
                }
                else
                {
                    wWinCount[m_wBankerUser]++;
                }
            }

            //临时下注数目
            LONGLONG lTempTableScore[GAME_PLAYER];
            ZeroMemory(lTempTableScore, sizeof(lTempTableScore));
            CopyMemory(lTempTableScore, m_lTableScore, sizeof(lTempTableScore));

            //统计得分
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)continue;

				if (wWinCount[i]>0)	//闲家胜利
				{
					LONGLONG lXianWinScore = m_lTableScore[i] * wWinTimes[i] * cbMaxCallBankerTimes;
					LONGLONG lCarryScore = m_pITableFrame->GetTableUserItem(i)->GetUserScore();

					//玩家单局最大赢分以身上携带分值为上限
					lXianWinScore = min(lXianWinScore, lCarryScore);

					GameEnd.lGameScore[i] = lXianWinScore;
					GameEnd.lGameScore[m_wBankerUser] -= lXianWinScore;
					m_lTableScore[i] = 0;
				}
				else					//庄家胜利
				{
					//玩家身上有多少分就赔付多少分。
					LONGLONG lXianLostScore = m_lTableScore[i] * wWinTimes[m_wBankerUser] * cbMaxCallBankerTimes;
					LONGLONG lCarryScore = m_pITableFrame->GetTableUserItem(i)->GetUserScore();

					lXianLostScore = -min(lXianLostScore, lCarryScore);

					GameEnd.lGameScore[i] = lXianLostScore;
					GameEnd.lGameScore[m_wBankerUser] += (-1)*GameEnd.lGameScore[i];
					m_lTableScore[i] = 0;
				}
			}

            //闲家强退分数
            GameEnd.lGameScore[m_wBankerUser] += m_lExitScore;

            //离开用户
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_lTableScore[i] > 0) { GameEnd.lGameScore[i] = -m_lTableScore[i]; }
            }

            //调整结算得分(庄家携带分不够赔的情况)
            ASSERT(m_wBankerUser != INVALID_CHAIR);

            LONGLONG lAllPlayerWinScore = 0;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                lAllPlayerWinScore += GameEnd.lGameScore[i];
            }

			//庄家不够赔
			ASSERT(lAllPlayerWinScore == -GameEnd.lGameScore[m_wBankerUser]);
			if (lAllPlayerWinScore > 0 && (m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore() + GameEnd.lGameScore[m_wBankerUser]) < 0)
			{
				LONGLONG lBankerRemainScore = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore();
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] >= 0)
					{
						continue;
					}

					lBankerRemainScore += (-GameEnd.lGameScore[i]);
				}

				LONGLONG lUserNeedScore = 0;
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] <= 0)
					{
						continue;
					}

					lUserNeedScore += GameEnd.lGameScore[i];
				}

				LONGLONG lTotalMidVal = 0;
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] <= 0)
					{
						continue;
					}

					GameEnd.lGameScore[i] = (double)(((double)lBankerRemainScore) / ((double)(lUserNeedScore))) * GameEnd.lGameScore[i];

					//玩家赢分改成：玩家单局最大赢分以身上携带分值为上限。
					LONGLONG lUserCarryScore = m_pITableFrame->GetTableUserItem(i)->GetUserScore();
					if (GameEnd.lGameScore[i] > lUserCarryScore)
					{
						GameEnd.lGameScore[i] = lUserCarryScore;
						LONGLONG lMidVal = GameEnd.lGameScore[i] - lUserCarryScore;
						lTotalMidVal += lMidVal;
					}
				}

				GameEnd.lGameScore[m_wBankerUser] = -m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore() + lTotalMidVal;
			}

			//庄家赢分改成：庄家单局最大赢分以身上携带分值为上限。
			if (lAllPlayerWinScore < 0 && (-lAllPlayerWinScore) > m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore())
			{
				LONGLONG lUserNeedScore = 0;
				LONGLONG lBankerCarryScore = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore();
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] >= 0)
					{
						continue;
					}

					lUserNeedScore += GameEnd.lGameScore[i];
				}

				//计算闲家赢分 
				LONGLONG lUserWinScore = 0;
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] <= 0)
					{
						continue;
					}

					//闲家赢的总分
					lUserWinScore += GameEnd.lGameScore[i];
				}

				//把闲家赢分转嫁到庄家携带的金币
				lBankerCarryScore += lUserWinScore;

				//调整闲家按照比例赔付
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] >= 0)
					{
						continue;
					}

					GameEnd.lGameScore[i] = -((double)((double)GameEnd.lGameScore[i] / (double)lUserNeedScore)) * lBankerCarryScore;
				}

				GameEnd.lGameScore[m_wBankerUser] = 0;
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
					{
						continue;
					}

					GameEnd.lGameScore[m_wBankerUser] += (-GameEnd.lGameScore[i]);
				}
			}

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                m_lLastTurnWinScore[i] = GameEnd.lGameScore[i];
                m_bLastTurnBeBanker[i] = (i == m_wBankerUser ? true : false);

                //获取用户
                IServerUserItem *pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);

                //写日志
                CString strUser;
                strUser.Format(TEXT("TABLEID = %d, CHAIRID = %d USERID = %d， 庄家标志 %d, 得分 %I64d ,房主USERID = %d, m_bgtConfig = %d"), m_pITableFrame->GetTableID(), i, pIKeyServerUserItem->GetUserID(), m_bLastTurnBeBanker[i], m_lLastTurnWinScore[i], m_pITableFrame->GetTableOwner(), m_bgtConfig);
				CString strFileName = TEXT("流程日志");

                tagLogUserInfo LogUserInfo;
                ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
                CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
                CopyMemory(LogUserInfo.szLogContent, strUser, sizeof(LogUserInfo.szLogContent));
                //m_pITableFrame->SendGameLogData(LogUserInfo);

				strUser += TEXT("\n");
				WriteInfo(TEXT("八人牛调试日志.log"), strUser);
            }
        }
        else
        {
            ASSERT(m_wBankerUser == INVALID_CHAIR);

            //大家一起比牌，最大的牌型通吃。
            //胜利玩家
            WORD wWinner = INVALID_CHAIR;

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE) { continue; }

                if(wWinner == INVALID_CHAIR)
                {
                    wWinner = i;
                }

                //对比扑克
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinner], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[i], m_cbCombineCardType[wWinner]))
                {
                    wWinner = i;
                }
            }

            WORD wWinTimes = m_GameLogic.GetTimes(cbUserCardData[wWinner], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[wWinner]);

            //统计得分
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(i == wWinner || m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                GameEnd.lGameScore[i] = -min(m_pITableFrame->GetTableUserItem(i)->GetUserScore(), m_lTableScore[i] * wWinTimes);
                m_lTableScore[i] = 0;

                GameEnd.lGameScore[wWinner] += (-GameEnd.lGameScore[i]);
            }

            GameEnd.lGameScore[wWinner] += m_lExitScore;
            m_lTableScore[wWinner] = 0;

            //离开用户
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_lTableScore[i] > 0) { GameEnd.lGameScore[i] = -m_lTableScore[i]; }
            }
        }

        //修改积分
        tagScoreInfo ScoreInfoArray[GAME_PLAYER];
        ZeroMemory(ScoreInfoArray, sizeof(ScoreInfoArray));

        //积分税收
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE) { continue; }

            if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                    ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
            {
                if(GameEnd.lGameScore[i] > 0L)
                {
                    GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                    if(GameEnd.lGameTax[i] > 0)
                    {
                        GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
                    }
                }
            }

            //历史积分
            m_HistoryScore.OnEventUserScore(i, GameEnd.lGameScore[i]);

            //保存积分
            ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
            ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
            ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

            //约战记录
            if(m_stRecord.nCount < MAX_RECORD_COUNT)
            {
                if(GameEnd.lGameScore[i] > 0)
                {
                    m_stRecord.lUserWinCount[i]++;
                }
                else
                {
                    m_stRecord.lUserLostCount[i]++;
                }
            }

            //房卡模式
            //积分房卡
            if((m_pITableFrame->GetDataBaseMode() == 0) && !m_pITableFrame->IsPersonalRoomDisumme() && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_RoomCardRecord.nCount < MAX_RECORD_COUNT))
            {
                m_RoomCardRecord.lDetailScore[i][m_RoomCardRecord.nCount] = ScoreInfoArray[i].lScore;
            }
            //金币房卡
            else if((m_pITableFrame->GetDataBaseMode() == 1) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
            {
                m_listWinScoreRecord[i].AddHead(ScoreInfoArray[i].lScore);
            }
			else
			{
				m_listGameEndScore[i].AddHead(ScoreInfoArray[i].lScore);
        }

			m_tagGameScoreInfo[i].lWinScore = ScoreInfoArray[i].lScore;
			m_tagGameScoreInfo[i].lRevenue = ScoreInfoArray[i].lRevenue;
        }

		if (IsContainAIAndRealPlayer())
		{
			//获取当前共享库存子项
			GetCurSubStorageItem();

			UpdateCtrlRes(m_emCtrlType);

			//修改共享库存子项
			ChangeCurSubStorageItem();
		}

		CMD_S_StatisticService StatisticService;
		ZeroMemory(&StatisticService, sizeof(StatisticService));

		StatisticService.lStatisticSysWin = g_lStatisticSysWin;
		StatisticService.lStatisticService = g_lStatisticService;

		// 发送消息
		m_pITableFrame->SendRoomData(NULL, SUB_S_STATISTIC_SERVICE, &StatisticService, sizeof(StatisticService));

		CMD_S_AnChou AnChou;
		ZeroMemory(&AnChou, sizeof(AnChou));

		AnChou.lCurAnChouRate = g_lCurAnChouRate;
		AnChou.lCurAnChouTotal = g_lCurAnChouTotal;

		//发送消息
		m_pITableFrame->SendRoomData(NULL, SUB_S_ANCHOU, &AnChou, sizeof(AnChou));

        m_stRecord.nCount++;

        //房卡模式
        if((m_pITableFrame->GetDataBaseMode() == 0) && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && (m_RoomCardRecord.nCount < MAX_RECORD_COUNT))
        {
            m_RoomCardRecord.nCount++;
        }

        //房卡模式
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));

            CMD_S_RoomCardRecord RoomCardRecord;
            ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        }

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE || m_bSpecialCard[i] == false)
            {
                continue;
            }

            m_GameLogic.GetSpecialSortCard(m_cbCombineCardType[i], m_cbHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
        }

        CopyMemory(GameEnd.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

        //获取玩家牌型
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
				GameEnd.cbCardType[i] = 255;
				ZeroMemory(GameEnd.cbHandCardData[i], MAX_CARDCOUNT);
                continue;
            }
			
            GameEnd.cbCardType[i] = m_cbCombineCardType[i];
        }

        //拷贝牌型倍数
        CopyMemory(GameEnd.wCardTypeTimes, wWinTimes, sizeof(wWinTimes));


		//发送游戏结束
		tagGameStaus	GameStaus;
		GameStaus.cbStatus = GAME_END;
		//用户状态
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem != NULL)
			{
				m_pITableFrame->SendTableData(i, SUB_S_GAME_STAUTS, &GameStaus, sizeof(GameStaus));
			}
		}

		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_STAUTS, &GameStaus, sizeof(GameStaus));


        //发送信息
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
			if (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE) { continue; }
            m_pITableFrame->SendTableData(i, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        }

        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		CopyMemory(&m_GameEndEx, &GameEnd, sizeof(GameEnd));
		m_GameEndEx.dwTickCountGameEnd = (DWORD)time(NULL);

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
        }

        if(m_pGameVideo)
        {
            m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
        }

        TryWriteTableScore(ScoreInfoArray);

		//发送信息
		for (WORD i = 0; i<m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL)
			{
				continue;
			}

			if (m_cbPlayStatus[i] == FALSE&&m_cbDynamicJoin[i] == FALSE)
			{
				continue;
			}

			if (!pIServerUserItem->IsAndroidUser())
			{
				continue;
			}

			if (!pIServerUserItem->IsClientReady())
			{
				continue;
			}

			m_pITableFrame->SendTableData(i, SUB_S_ANDROID_BANKOPER);
		}


        //牛牛当庄 场上有牛牛以及牛牛以上的牌型，由最大的牌型玩家当庄, 如果没有还是原来的当庄
        if(m_bgtConfig == BGT_NIUNIU_)
        {
            //牌型最大玩家
            WORD wMaxPlayerUser = INVALID_CHAIR;

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //获取用户
                IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                if(pIServerUserItem == NULL)
                {
                    continue;
                }

                if(wMaxPlayerUser == INVALID_CHAIR)
                {
                    wMaxPlayerUser = i;
                }

                //获取较大者
                if(m_GameLogic.CompareCard(cbUserCardData[i], m_cbHandCardData[wMaxPlayerUser], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[i], m_cbCombineCardType[wMaxPlayerUser]) == true)
                {
                    wMaxPlayerUser = i;
                }
            }

            BYTE cbMaxCardType = ((m_cbCombineCardType[wMaxPlayerUser] == INVALID_BYTE) ? (m_GameLogic.GetCardType(m_cbHandCardData[wMaxPlayerUser], MAX_CARDCOUNT, m_ctConfig)) : m_cbCombineCardType[wMaxPlayerUser]);

            //如果牌型比牛牛少原来的玩家当庄
            if(cbMaxCardType >= CT_ADDTIMES_OX_VALUENIUNIU)
            {
                //牌型最大玩家当庄
                m_wBankerUser = wMaxPlayerUser;
            }

            //按照牌型排序大小
            //保存扑克
            m_listCardTypeOrder.RemoveAll();
            BYTE cbTempCardData[GAME_PLAYER][MAX_CARDCOUNT];
            CopyMemory(cbTempCardData, m_cbHandCardData, sizeof(cbTempCardData));

            BYTE cbTempCombineCardType[GAME_PLAYER];
            CopyMemory(cbTempCombineCardType, m_cbCombineCardType, sizeof(cbTempCombineCardType));

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //获取用户
                IServerUserItem *pIServerUserItemI = m_pITableFrame->GetTableUserItem(i);
                if(pIServerUserItemI == NULL)
                {
                    continue;
                }

                for(WORD j = 0; j < m_wPlayerCount - i - 1; j++)
                {
                    //获取用户
                    IServerUserItem *pIServerUserItemJ = m_pITableFrame->GetTableUserItem(j);
                    if(pIServerUserItemJ == NULL)
                    {
                        continue;
                    }

                    //获取较大者
                    if(m_GameLogic.CompareCard(cbTempCardData[i], cbTempCardData[j], MAX_CARDCOUNT, m_ctConfig, cbTempCombineCardType[i], cbTempCombineCardType[j]) == false)
                    {
                        BYTE cbTempData[MAX_CARDCOUNT];
                        CopyMemory(cbTempData, cbTempCardData[j], sizeof(cbTempData));
                        CopyMemory(cbTempCardData[j], cbTempCardData[i], sizeof(cbTempCardData[j]));
                        CopyMemory(cbTempCardData[i], cbTempData, sizeof(cbTempCardData[i]));

                        BYTE cbTempType = cbTempCombineCardType[j];
                        cbTempCombineCardType[j] = cbTempCombineCardType[i];
                        cbTempCombineCardType[i] = cbTempType;
                    }
                }
            }

            for(WORD i = 0; i < GAME_PLAYER; i++)
            {
                if(cbTempCardData[i][0] == 0)
                {
                    continue;
                }

                WORD wKeyChairID = SearchKeyCardChairID(cbTempCardData[i]);
                ASSERT(wKeyChairID != INVALID_CHAIR);

                m_listCardTypeOrder.AddTail(wKeyChairID);
            }
        }
        //无牛下庄
        else if(m_bgtConfig == BGT_NONIUNIU_)
        {
            //庄家无牛轮到下一位上庄
            ASSERT(m_wBankerUser != INVALID_CHAIR);
            BYTE cbBankerCardType = ((m_cbCombineCardType[m_wBankerUser] == INVALID_BYTE) ? (m_GameLogic.GetCardType(m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig)) : m_cbCombineCardType[m_wBankerUser]);
            if(cbBankerCardType == CT_CLASSIC_OX_VALUE0
                    || cbBankerCardType == CT_ADDTIMES_OX_VALUE0)
            {
                //始叫用户
                while(true)
                {
                    m_wBankerUser = (m_wBankerUser + 1) % m_wPlayerCount;
                    if(m_pITableFrame->GetTableUserItem(m_wBankerUser) != NULL && m_cbPlayStatus[m_wBankerUser] == TRUE)
                    {
                        break;
                    }
                }
            }
        }



        //房卡模式
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            //解散清理记录
            if(m_pITableFrame->IsPersonalRoomDisumme())
            {
                ZeroMemory(&m_stRecord, sizeof(m_stRecord));
                ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
                m_lBgtDespotWinScore = 0L;
            }
        }

        //私人房霸王庄庄家统计输赢
        BOOL bEndLoop = FALSE;
        if(m_bgtConfig == BGT_DESPOT_ && m_lBeBankerCondition != INVALID_DWORD)
        {
            m_lBgtDespotWinScore += GameEnd.lGameScore[m_wBankerUser];

            if((-m_lBgtDespotWinScore) > m_lBeBankerCondition)
            {
                bEndLoop = TRUE;
            }
        }

		EnableTimeElapse(false, TIME_GAMEFREE);
		m_pITableFrame->SetGameStatus(GS_TK_WAITEND);
		m_pITableFrame->SetGameTimer(IDI_DELAY_GAMEFREE, TIME_DELAY_GAMEFREE, 1, (WPARAM)(&bEndLoop));
		//m_pITableFrame->ConcludeGame(GS_TK_FREE, bEndLoop);

        if(!IsRoomCardType())
        {
            //删除时间
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

            //删除离线代打定时器
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
            }
        }


        m_bReNewTurn = false;

        //修复漏洞
        //先拿 1号椅子创建房间，创建通比玩法，然后完了之后解散，这个时候再创建一个牛牛上庄或者无牛下庄，如果刚好那是拿1号椅子创建，m_wBankerUser就为无效服务端崩溃
        if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_pITableFrame->IsPersonalRoomDisumme())
        {
            m_bReNewTurn = true;
            m_wBankerUser = INVALID_CHAIR;
        }

		//最近有玩家连赢三局
		WORD wSerialWinChairID = INVALID_CHAIR;
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL)
			{
				continue;
			}

			BYTE cbWinCount = 0;
			BYTE cbCirCount = 0;

			POSITION posTail = m_listGameEndScore[i].GetTailPosition();
			while (posTail)
			{
				if (cbCirCount >= 3)
				{
					break;
				}

				cbCirCount++;
				if (m_listGameEndScore[i].GetPrev(posTail) > 0)
				{
					cbWinCount++;
				}
				else
				{
					cbWinCount = 0;
				}

				if (cbWinCount >= 3)
				{
					wSerialWinChairID = i;

					break;
				}
			}

			if (wSerialWinChairID != INVALID_CHAIR)
			{
				break;
			}
		}

		if (wSerialWinChairID != INVALID_CHAIR)
		{
			WORD wRandChairIndex = rand() % m_wPlayerCount;
			for (WORD i = 0; i < m_wPlayerCount; i++)
			{
				wRandChairIndex = (wRandChairIndex + 1) % m_wPlayerCount;
				if (m_cbPlayStatus[wRandChairIndex] == FALSE && m_cbDynamicJoin[wRandChairIndex] == FALSE)
				{
					continue;
				}

				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wRandChairIndex);
				if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser() && pIServerUserItem->IsClientReady()
					&& wRandChairIndex != wSerialWinChairID)
				{
					m_pITableFrame->SendTableData(wRandChairIndex, SUB_S_ANDROID_NOTIFYWIN);
					break;
				}
			}
		}

		//牌池重置
		m_GameLogic.RandPool(m_gtConfig == GT_HAVEKING_ ? true : false);
		m_TableDebug.RefreshCardPool(m_GameLogic.GetPoolData(), m_GameLogic.GetPoolLeftCount());
        return true;
    }
    case GER_USER_LEAVE:		//用户强退
    case GER_NETWORK_ERROR:
    {
        //旗舰平台没有强退
        //效验参数
        ASSERT(pIServerUserItem != NULL);
        ASSERT(wChairID < m_wPlayerCount && (m_cbPlayStatus[wChairID] == TRUE || m_cbDynamicJoin[wChairID] == FALSE));

        if(m_cbPlayStatus[wChairID] == FALSE) { return true; }
        //设置状态
        m_cbPlayStatus[wChairID] = FALSE;
        m_cbDynamicJoin[wChairID] = FALSE;

        //定义变量
        CMD_S_PlayerExit PlayerExit;
        PlayerExit.wPlayerID = wChairID;

        //发送信息
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == wChairID || (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)) { continue; }
            m_pITableFrame->SendTableData(i, SUB_S_PLAYER_EXIT, &PlayerExit, sizeof(PlayerExit));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PLAYER_EXIT, &PlayerExit, sizeof(PlayerExit));

        ////////////////////////////////////
        if(m_bgtConfig == BGT_TONGBI_)
        {
            //逃跑那个玩家直接扣底住，然后直接写分，如果剩下超过2个以上的玩家继续比牌，  那逃跑玩家的底住加到那个赢的玩家身上，  如果逃跑后只剩一个玩家，就结束
            m_lExitScore += m_lTableScore[wChairID];

            tagScoreInfo ScoreInfo;
            ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
            ScoreInfo.lScore = -m_lTableScore[wChairID];
            ScoreInfo.cbType = SCORE_TYPE_FLEE;

            m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);

            m_lTableScore[wChairID] = 0;

            //获取用户
            IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(wChairID);


            //玩家人数
            WORD wUserCount = 0;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE)
                {
                    wUserCount++;
                }
            }

            //结束游戏
            if(wUserCount == 1)
            {
                //定义变量
                CMD_S_GameEnd GameEnd;
                ZeroMemory(&GameEnd, sizeof(GameEnd));
                CopyMemory(GameEnd.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

                //赋值最后一张牌
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    GameEnd.cbLastSingleCardData[i] = m_cbOriginalCardData[i][4];
                }

                //获取玩家牌型
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && i != wChairID)
                    {
                        continue;
                    }

                    if(i == wChairID)
                    {
                        GameEnd.cbCardType[i] = (m_cbCombineCardType[i] == 0 ? m_cbOriginalCardType[i] : m_cbCombineCardType[i]);
                        continue;
                    }

                    GameEnd.lGameScore[i] = m_pITableFrame->GetCellScore();
                    if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                            ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
                    {
                        GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                    }
                    GameEnd.lGameScore[i] = GameEnd.lGameScore[i] - GameEnd.lGameTax[i];
                    GameEnd.cbCardType[i] = (m_cbCombineCardType[i] == 0 ? m_cbOriginalCardType[i] : m_cbCombineCardType[i]);
                }

                //发送信息
                m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                if(m_pGameVideo)
                {
                    m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                    m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                }

                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE)
                    {
                        continue;
                    }

                    //修改积分
                    tagScoreInfo ScoreInfo;
                    ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
                    ScoreInfo.lScore = GameEnd.lGameScore[i];
                    ScoreInfo.lRevenue = GameEnd.lGameTax[i];
                    ScoreInfo.cbType = (GameEnd.lGameScore[i] > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE);

                    m_pITableFrame->WriteUserScore(i, ScoreInfo);

                    //获取用户
                    IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(i);

				}



                //结束游戏
                m_pITableFrame->ConcludeGame(GS_TK_FREE);

                if(!IsRoomCardType())
                {
                    //删除时间
                    m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                    //删除离线代打定时器
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                    }
                }

                UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                //更新房间用户信息
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == wChairID)
                    {
                        continue;
                    }

                    //获取用户
                    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                    if(!pIServerUserItem)
                    {
                        continue;
                    }

                }

                m_bReNewTurn = false;

                return true;
            }

            OnUserOpenCard(wChairID, m_cbHandCardData[wChairID]);
        }

        ////////////////////////////////////
        WORD wWinTimes[GAME_PLAYER];
        ZeroMemory(wWinTimes, sizeof(wWinTimes));

        //倍数抢庄 结算需要乘以cbMaxCallBankerTimes
        BYTE cbMaxCallBankerTimes = 1;
        if(m_bgtConfig == BGT_ROB_)
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxCallBankerTimes)
                {
                    cbMaxCallBankerTimes = m_cbCallBankerTimes[i];
                }
            }
        }

        //游戏进行 （下注后）
        if(m_pITableFrame->GetGameStatus() == GS_TK_PLAYING)
        {
            if(wChairID == m_wBankerUser)	//庄家强退
            {
                //定义变量
                CMD_S_GameEnd GameEnd;
                ZeroMemory(&GameEnd, sizeof(GameEnd));
                ZeroMemory(wWinTimes, sizeof(wWinTimes));
                CopyMemory(GameEnd.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

                BYTE cbUserCardData[GAME_PLAYER][MAX_CARDCOUNT];
                CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(cbUserCardData));

                //赋值最后一张牌
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    GameEnd.cbLastSingleCardData[i] = m_cbOriginalCardData[i][4];
                }

                //得分倍数
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE) { continue; }
                    wWinTimes[i] = (m_pITableFrame->GetGameStatus() != GS_TK_PLAYING) ? (1) : (m_GameLogic.GetTimes(cbUserCardData[i], MAX_CARDCOUNT, m_ctConfig, INVALID_BYTE));
                }

                //统计得分 已下或没下
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE) { continue; }
                    GameEnd.lGameScore[i] = m_lTableScore[i] * wWinTimes[i] * cbMaxCallBankerTimes;
                    GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameScore[i];
                    m_lTableScore[i] = 0;
                }

                //修改积分
                tagScoreInfo ScoreInfoArray[GAME_PLAYER];
                ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

                //积分税收
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && i != m_wBankerUser) { continue; }

                    if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                            ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
                    {
                        if(GameEnd.lGameScore[i] > 0L)
                        {
                            GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                            if(GameEnd.lGameTax[i] > 0)
                            {
                                GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
                            }
                        }
                    }

                    //保存积分
                    ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
                    ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];

                    if(i == m_wBankerUser)
                    {
                        ScoreInfoArray[i].cbType = SCORE_TYPE_FLEE;
                    }
                    else if(m_cbPlayStatus[i] == TRUE)
                    {
                        ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
                    }

                    m_pITableFrame->WriteUserScore(i, ScoreInfoArray[i]);
                }

                //获取玩家牌型
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE)
                    {
                        continue;
                    }

                    GameEnd.cbCardType[i] = (m_cbCombineCardType[i] == 0 ? m_cbOriginalCardType[i] : m_cbCombineCardType[i]);
                }

                //拷贝牌型倍数
                CopyMemory(GameEnd.wCardTypeTimes, wWinTimes, sizeof(wWinTimes));

                //发送信息
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
					if (i == m_wBankerUser || (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE )) { continue; }
                    m_pITableFrame->SendTableData(i, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                }
                m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                if(m_pGameVideo)
                {
                    m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                }

                if(m_pGameVideo)
                {
                    m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                }

                //TryWriteTableScore(ScoreInfoArray);

                //写入库存
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && i != m_wBankerUser) { continue; }

                    //获取用户
                    IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(i);


                }
                //结束游戏
                m_pITableFrame->ConcludeGame(GS_TK_FREE);

                if(!IsRoomCardType())
                {
                    //删除时间
                    m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                    //删除离线代打定时器
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                    }
                }

                UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                //更新房间用户信息
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == wChairID)
                    {
                        continue;
                    }

                    //获取用户
                    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                    if(!pIServerUserItem)
                    {
                        continue;
                    }

                }

                m_bReNewTurn = false;

                return true;
            }
            else						//闲家强退
            {
                //已经下注
				if (m_lTableScore[wChairID] > 0L)
				{
					ZeroMemory(wWinTimes, sizeof(wWinTimes));

					//用户扑克
					BYTE cbUserCardData[MAX_CARDCOUNT];
					CopyMemory(cbUserCardData, m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT);

					//用户倍数
					wWinTimes[m_wBankerUser] = (m_pITableFrame->GetGameStatus() == GS_TK_SCORE) ? (1) : (m_GameLogic.GetTimes(cbUserCardData, MAX_CARDCOUNT, m_ctConfig, INVALID_BYTE));

					//修改积分
					LONGLONG lScore = -m_lTableScore[wChairID] * wWinTimes[m_wBankerUser] * cbMaxCallBankerTimes;
					m_lExitScore += (-1 * lScore);
					m_lTableScore[wChairID] = (-1 * lScore);

					tagScoreInfo ScoreInfo;
					ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
					ScoreInfo.lScore = lScore;
					ScoreInfo.cbType = SCORE_TYPE_FLEE;

					m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);

					//获取用户
					IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(wChairID);
				}

                //玩家人数
                WORD wUserCount = 0;
                for(WORD i = 0; i < m_wPlayerCount; i++)if(m_cbPlayStatus[i] == TRUE) { wUserCount++; }

                //结束游戏
                if(wUserCount == 1)
                {
                    //定义变量
                    CMD_S_GameEnd GameEnd;
                    ZeroMemory(&GameEnd, sizeof(GameEnd));
                    CopyMemory(GameEnd.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
                    ASSERT(m_lExitScore >= 0L);

                    //赋值最后一张牌
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        GameEnd.cbLastSingleCardData[i] = m_cbOriginalCardData[i][4];

                    }
                    //统计得分
                    GameEnd.lGameScore[m_wBankerUser] += m_lExitScore;
                    if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                            ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
                    {
                        GameEnd.lGameTax[m_wBankerUser] = m_pITableFrame->CalculateRevenue(m_wBankerUser, GameEnd.lGameScore[m_wBankerUser]);
                    }
                    GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameTax[m_wBankerUser];

                    //获取玩家牌型
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        if(m_cbPlayStatus[i] == FALSE)
                        {
                            continue;
                        }

                        GameEnd.cbCardType[i] = (m_cbCombineCardType[i] == 0 ? m_cbOriginalCardType[i] : m_cbCombineCardType[i]);
                    }

                    //拷贝牌型倍数
                    CopyMemory(GameEnd.wCardTypeTimes, wWinTimes, sizeof(wWinTimes));

                    //发送信息
                    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                    if(m_pGameVideo)
                    {
                        m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                        m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                    }

                    WORD Zero = 0;
                    for(; Zero < m_wPlayerCount; Zero++)if(m_lTableScore[Zero] != 0) { break; }
                    if(Zero != m_wPlayerCount)
                    {
                        //修改积分
                        tagScoreInfo ScoreInfo;
                        ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
                        ScoreInfo.lScore = GameEnd.lGameScore[m_wBankerUser];
                        ScoreInfo.lRevenue = GameEnd.lGameTax[m_wBankerUser];
                        ScoreInfo.cbType = SCORE_TYPE_WIN;

                        m_pITableFrame->WriteUserScore(m_wBankerUser, ScoreInfo);

                        //获取用户
                        IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(wChairID);


                    }

                    //结束游戏
                    m_pITableFrame->ConcludeGame(GS_TK_FREE);

                    if(!IsRoomCardType())
                    {
                        //删除时间
                        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                        //删除离线代打定时器
                        for(WORD i = 0; i < m_wPlayerCount; i++)
                        {
                            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                        }
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                    //更新房间用户信息
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        if(i == wChairID)
                        {
                            continue;
                        }

                        //获取用户
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                        if(!pIServerUserItem)
                        {
                            continue;
                        }
                    }

                    m_bReNewTurn = false;

                    return true;
                }

                OnUserOpenCard(wChairID, m_cbHandCardData[wChairID]);
            }
        }
        //下注逃跑
        else if(m_pITableFrame->GetGameStatus() == GS_TK_SCORE)
        {
            //剩余玩家人数
            WORD wUserCount = 0;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE)
                {
                    wUserCount++;
                }
            }

            //发四等五 (已发4张牌) 按照最小筹码进行赔付
            if(m_stConfig == ST_SENDFOUR_)
            {
                //获取最小筹码
                LONGLONG lMinJetton = 0L;

                //自由配置额度
                if(m_btConfig == BT_FREE_)
                {
                    lMinJetton = m_lFreeConfig[0];
                }
                //百分比配置额度
                else if(m_btConfig == BT_PENCENT_)
                {
                    //最小下注玩家
                    WORD wMinJettonChairID = INVALID_CHAIR;
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        //获取用户
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                        if(pIServerUserItem == NULL || m_cbPlayStatus[i] == FALSE)
                        {
                            continue;
                        }

                        if(wMinJettonChairID == INVALID_CHAIR) { wMinJettonChairID = i; }

                        //获取较大者
                        if(m_lTurnMaxScore[i] < m_lTurnMaxScore[wMinJettonChairID])
                        {
                            wMinJettonChairID = i;
                        }
                    }

                    ASSERT(wMinJettonChairID != INVALID_CHAIR);

                    lMinJetton = m_lTurnMaxScore[wMinJettonChairID] * m_lPercentConfig[0] / 100;
                }

                CMD_S_GameEnd GameEnd;
                ZeroMemory(&GameEnd, sizeof(GameEnd));
                CopyMemory(GameEnd.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

                //赋值最后一张牌
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    GameEnd.cbLastSingleCardData[i] = m_cbOriginalCardData[i][4];
                }

                tagScoreInfo ScoreInfoArray[GAME_PLAYER];
                ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

                //庄家逃跑结束游戏
                if(wChairID == m_wBankerUser)
                {
                    //发四等五	庄家逃跑，不显示牌型
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        GameEnd.cbCardType[i] = INVALID_BYTE;
                        //if(m_cbPlayStatus[i]==FALSE)
                        //{
                        //	continue;
                        //}

                        //GameEnd.cbCardType[i] = (m_cbCombineCardType[i] == 0 ? m_cbOriginalCardType[i] : m_cbCombineCardType[i]);
                    }

                    LONGLONG lBankerScore = 0L;
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        //获取用户
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                        if(pIServerUserItem == NULL || m_cbPlayStatus[i] == FALSE || i == m_wBankerUser)
                        {
                            continue;
                        }

                        //统计得分
                        GameEnd.lGameScore[i] += lMinJetton;
                        if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                                ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
                        {
                            GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                        }
                        GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];

                        //写分
                        ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
                        ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
                        ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;

                        lBankerScore -= lMinJetton;
                    }

                    GameEnd.lGameScore[m_wBankerUser] = lBankerScore;
                    ScoreInfoArray[m_wBankerUser].lScore = GameEnd.lGameScore[m_wBankerUser];
                    ScoreInfoArray[m_wBankerUser].cbType = SCORE_TYPE_FLEE;

                    //发送信息
                    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                    if(m_pGameVideo)
                    {
                        m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                        m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                    }

                    //写分
                    //TryWriteTableScore(ScoreInfoArray);

                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        //获取用户
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                        if(pIServerUserItem == NULL)
                        {
                            continue;
                        }

                        m_pITableFrame->WriteUserScore(i, ScoreInfoArray[i]);
                    }

                    //结束游戏
                    m_pITableFrame->ConcludeGame(GS_TK_FREE);

                    if(!IsRoomCardType())
                    {
                        //删除时间
                        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                        //删除离线代打定时器
                        for(WORD i = 0; i < m_wPlayerCount; i++)
                        {
                            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                        }
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                    //更新房间用户信息
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        if(i == wChairID)
                        {
                            continue;
                        }

                        //获取用户
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                        if(!pIServerUserItem)
                        {
                            continue;
                        }
                    }

                    m_bReNewTurn = false;

                    return true;
                }
                //闲家逃跑，只剩下一个玩家
                else if(wChairID != m_wBankerUser && wUserCount == 1)
                {
                    //统计得分
                    GameEnd.lGameScore[wChairID] -= lMinJetton;
                    GameEnd.lGameScore[m_wBankerUser] += lMinJetton;
                    if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                            ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
                    {
                        GameEnd.lGameTax[m_wBankerUser] = m_pITableFrame->CalculateRevenue(m_wBankerUser, GameEnd.lGameScore[m_wBankerUser]);
                    }
                    GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameTax[m_wBankerUser];

                    //写分
                    ScoreInfoArray[wChairID].lScore = GameEnd.lGameScore[wChairID];
                    ScoreInfoArray[wChairID].cbType = SCORE_TYPE_FLEE;
                    ScoreInfoArray[m_wBankerUser].lScore = GameEnd.lGameScore[m_wBankerUser];
                    ScoreInfoArray[m_wBankerUser].cbType = SCORE_TYPE_WIN;
                    ScoreInfoArray[m_wBankerUser].lRevenue = GameEnd.lGameTax[m_wBankerUser];

                    //发送信息
                    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                    if(m_pGameVideo)
                    {
                        m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                        m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                    }

                    //写分
                    //TryWriteTableScore(ScoreInfoArray);

                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        //获取用户
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                        if(pIServerUserItem == NULL)
                        {
                            continue;
                        }

                        m_pITableFrame->WriteUserScore(i, ScoreInfoArray[i]);
                    }

                    //结束游戏
                    m_pITableFrame->ConcludeGame(GS_TK_FREE);

                    if(!IsRoomCardType())
                    {
                        //删除时间
                        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                        //删除离线代打定时器
                        for(WORD i = 0; i < m_wPlayerCount; i++)
                        {
                            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                        }
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                    //更新房间用户信息
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        if(i == wChairID)
                        {
                            continue;
                        }

                        //获取用户
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                        if(!pIServerUserItem)
                        {
                            continue;
                        }

                    }

                    m_bReNewTurn = false;

                    return true;
                }
                //闲家逃跑，剩下2个以上玩家 继续游戏
                else
                {
                    m_lExitScore += lMinJetton;
                    m_lTableScore[wChairID] = 0;

                    tagScoreInfo ScoreInfoArray[GAME_PLAYER];
                    ZeroMemory(ScoreInfoArray, sizeof(ScoreInfoArray));
                    ScoreInfoArray[wChairID].lScore -= lMinJetton;
                    ScoreInfoArray[wChairID].cbType = SCORE_TYPE_FLEE;

                    //TryWriteTableScore(ScoreInfoArray);
                    m_pITableFrame->WriteUserScore(wChairID, ScoreInfoArray[wChairID]);

                    OnUserAddScore(wChairID, 0);
                }
            }
            //下注发牌 (未发牌不需要赔付)
            else if(m_stConfig == ST_BETFIRST_)
            {
                //庄家逃跑结束游戏 或只剩下一个玩家
                if(wChairID == m_wBankerUser || wUserCount == 1)
                {
                    CMD_S_GameEnd GameEnd;
                    ZeroMemory(&GameEnd, sizeof(GameEnd));

                    //发送信息
                    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                    if(m_pGameVideo)
                    {
                        m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                        m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                    }

                    //结束游戏
                    m_pITableFrame->ConcludeGame(GS_TK_FREE);

                    if(!IsRoomCardType())
                    {
                        //删除时间
                        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                        //删除离线代打定时器
                        for(WORD i = 0; i < m_wPlayerCount; i++)
                        {
                            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                        }
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                    //更新房间用户信息
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        if(i == wChairID)
                        {
                            continue;
                        }

                        //获取用户
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                        if(!pIServerUserItem)
                        {
                            continue;
                        }

                    }

                    m_bReNewTurn = false;

                    return true;
                }
                //闲家逃跑 且游戏人数大于1  继续游戏
                else
                {
                    ASSERT(wUserCount >= 2);

                    OnUserAddScore(wChairID, 0);
                    m_lTableScore[wChairID] = 0L;
                }
            }
        }
        //叫庄状态逃跑
        else
        {
            //玩家人数
            WORD wUserCount = 0;
            for(WORD i = 0; i < m_wPlayerCount; i++)if(m_cbPlayStatus[i] == TRUE) { wUserCount++; }

            //结束游戏
            if(wUserCount == 1)
            {
                //定义变量
                CMD_S_GameEnd GameEnd;
                ZeroMemory(&GameEnd, sizeof(GameEnd));

                //发送信息
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE) { continue; }
                    m_pITableFrame->SendTableData(i, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                }
                m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                if(m_pGameVideo)
                {
                    m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                }

                if(m_pGameVideo)
                {
                    m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                }

                //结束游戏
                m_pITableFrame->ConcludeGame(GS_TK_FREE);

                if(!IsRoomCardType())
                {
                    //删除时间
                    m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                    //删除离线代打定时器
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                    }
                }

                UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                //更新房间用户信息
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == wChairID)
                    {
                        continue;
                    }

                    //获取用户
                    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                    if(!pIServerUserItem)
                    {
                        continue;
                    }

                }

                m_bReNewTurn = false;

                return true;
            }
            else
            {
                OnUserCallBanker(wChairID, false, 0);
            }
        }

		//牌池重置
		m_GameLogic.RandPool(m_gtConfig == GT_HAVEKING_ ? true : false);
		m_TableDebug.RefreshCardPool(m_GameLogic.GetPoolData(), m_GameLogic.GetPoolLeftCount());
        return true;
    }
    }

    return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		UpdateConfigPersonRoom();

    switch(cbGameStatus)
    {
    case GAME_STATUS_FREE:		//空闲状态
    {
        //私人房间
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            //cbGameRule[1] 为  2 、3 、4, 5, 6 0分别对应 2人 、 3人 、 4人 、5, 6 2-6人 这几种配置
            BYTE *pGameRule = m_pITableFrame->GetGameRule();
            if(pGameRule[1] != 0)
            {
                m_wPlayerCount = pGameRule[1];

                //设置人数
                m_pITableFrame->SetTableChairCount(m_wPlayerCount);
            }
            else
            {
                m_wPlayerCount = GAME_PLAYER;

                //设置人数
                m_pITableFrame->SetTableChairCount(GAME_PLAYER);
            }

            ASSERT(pGameRule[3] == 22 || pGameRule[3] == 23);
            if(pGameRule[3] == 22 || pGameRule[3] == 23)
            {
                //默认经典模式
                m_ctConfig = (CARDTYPE_CONFIG)(pGameRule[3]);
            }

            ASSERT(pGameRule[4] == 32 || pGameRule[4] == 33);
            if(pGameRule[4] == 32 || pGameRule[4] == 33)
            {
                //默认发四等五
                m_stConfig = (SENDCARDTYPE_CONFIG)(pGameRule[4]);
            }

            //ASSERT (pGameRule[5] == 42 || pGameRule[5] == 43);
            //m_gtConfig = (KING_CONFIG)(pGameRule[5]);

            ASSERT(pGameRule[6] == 52 || pGameRule[6] == 53 || pGameRule[6] == 54 || pGameRule[6] == 55 || pGameRule[6] == 56 || pGameRule[6] == 57);
            if(pGameRule[6] == 52 || pGameRule[6] == 53 || pGameRule[6] == 54 || pGameRule[6] == 55 || pGameRule[6] == 56 || pGameRule[6] == 57)
            {
                //默认霸王庄
                m_bgtConfig = (BANERGAMETYPE_CONFIG)(pGameRule[6]);
            }

            //下注配置只能在后台配置
            //优化内容配置
            LONG *plConfig = new LONG;
            ZeroMemory(plConfig, sizeof(LONG));

            CopyMemory(plConfig, &(pGameRule[7]), sizeof(LONG));
            m_lBeBankerCondition = *plConfig;

            CopyMemory(plConfig, &(pGameRule[7 + sizeof(LONG)]), sizeof(LONG));
            m_lPlayerBetTimes = *plConfig;

            m_cbAdmitRevCard = pGameRule[15];
            m_cbMaxCallBankerTimes = pGameRule[16];

            for(WORD i = 0; i < MAX_SPECIAL_CARD_TYPE - 1; i++)
            {
                m_cbEnableCardType[i] = pGameRule[17 + i];
            }

            //默认有大小王
            m_gtConfig = (pGameRule[25] == TRUE ? GT_HAVEKING_ : GT_NOKING_);

            m_cbClassicTypeConfig = pGameRule[26];
            if(m_ctConfig == CT_ADDTIMES_)
            {
                m_cbClassicTypeConfig = INVALID_BYTE;
            }

            //积分房卡
            if(IsRoomCardType())
            {
				//推注类型
                m_tyConfig = (pGameRule[27] == TRUE ? BT_TUI_DOUBLE_ : BT_TUI_NONE_);

				//房卡断线代打标识，金币房卡默认m_cbRCOfflineTrustee为TRUE
				m_cbRCOfflineTrustee = pGameRule[28];
            }
			else
			{
				m_cbRCOfflineTrustee = TRUE;
			}
        }

        //构造数据
        CMD_S_StatusFree StatusFree;
        ZeroMemory(&StatusFree, sizeof(StatusFree));

        //CString cs;
        //cs.Format(TEXT("CellScore = %d"), m_pITableFrame->GetCellScore());
        //CTraceService::TraceString(cs,TraceLevel_Exception);

        //设置变量
        StatusFree.lCellScore = m_pITableFrame->GetCellScore();
        StatusFree.ctConfig = m_ctConfig;
        StatusFree.stConfig = m_stConfig;
        StatusFree.bgtConfig = m_bgtConfig;
        StatusFree.btConfig = m_btConfig;
        StatusFree.gtConfig = m_gtConfig;

        //历史积分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusFree.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusFree.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

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
        StatusFree.wGamePlayerCountRule = pGameRule[1];
        StatusFree.cbAdmitRevCard = m_cbAdmitRevCard;
        StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();
		for (int i = 0; i < MAX_CARD_TYPE; ++i)
			StatusFree.wCardTypeTimes[i] = m_GameLogic.m_cbCardTypeTimes[i];
        //防调试
        if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
        {
            StatusFree.bIsAllowAvertDebug = true;
        }


        //房卡模式
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));

            CMD_S_RoomCardRecord RoomCardRecord;
            ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        }

		//金币场和金币房卡可以托管，积分房卡也托管
		//断线回来取消代打
		//if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
		//        || (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
		{
			//重连取消托管标志
			pIServerUserItem->SetTrusteeUser(false);
			m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
		}

		StatusFree.nTimeStart = m_nTimeStart;
		StatusFree.nTimeCallBanker = m_nTimeCallBanker;
		StatusFree.nTimeAddScore = m_nTimeAddScore;
		StatusFree.nTimeOpenCard = m_nTimeOpenCard;

        //发送场景
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
    }
    case GS_TK_CALL:	//叫庄状态
    {
        //构造数据
        CMD_S_StatusCall StatusCall;
        ZeroMemory(&StatusCall, sizeof(StatusCall));

        //历史积分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusCall.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusCall.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //设置变量
        StatusCall.lCellScore = m_pITableFrame->GetCellScore();
        StatusCall.cbDynamicJoin = m_cbDynamicJoin[wChairID];
        CopyMemory(StatusCall.cbPlayStatus, m_cbPlayStatus, sizeof(StatusCall.cbPlayStatus));

        StatusCall.ctConfig = m_ctConfig;
        StatusCall.stConfig = m_stConfig;
        StatusCall.bgtConfig = m_bgtConfig;
        StatusCall.btConfig = m_btConfig;
        StatusCall.gtConfig = m_gtConfig;

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
        CopyMemory(&StatusCall.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
        StatusCall.wGamePlayerCountRule = pGameRule[1];
        StatusCall.cbAdmitRevCard = m_cbAdmitRevCard;
        StatusCall.cbMaxCallBankerTimes = m_cbMaxCallBankerTimes;
        StatusCall.wBgtRobNewTurnChairID = m_wBgtRobNewTurnChairID;
        StatusCall.cbPlayMode = m_pITableFrame->GetDataBaseMode();
		for (int i = 0; i < MAX_CARD_TYPE; ++i)
			StatusCall.wCardTypeTimes[i] = m_GameLogic.m_cbCardTypeTimes[i];
        //防调试
        if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
        {
            StatusCall.bIsAllowAvertDebug = true;
        }

        CopyMemory(StatusCall.cbCallBankerStatus, m_cbCallBankerStatus, sizeof(StatusCall.cbCallBankerStatus));
        CopyMemory(StatusCall.cbCallBankerTimes, m_cbCallBankerTimes, sizeof(StatusCall.cbCallBankerTimes));

        //更新房间用户信息
        UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

		StatusCall.nTimeStart = m_nTimeStart;
		StatusCall.nTimeCallBanker = m_nTimeCallBanker;
		StatusCall.nTimeAddScore = m_nTimeAddScore;
		StatusCall.nTimeOpenCard = m_nTimeOpenCard;

        //发送场景
        bool bResult = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusCall, sizeof(StatusCall));

        //房卡模式
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));

            CMD_S_RoomCardRecord RoomCardRecord;
            ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        }

        //发四等五
        if(m_stConfig == ST_SENDFOUR_ && m_bgtConfig == BGT_ROB_)
        {
            //设置变量
            CMD_S_SendFourCard SendFourCard;
            ZeroMemory(&SendFourCard, sizeof(SendFourCard));

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }

                //派发扑克(开始只发四张牌)
                CopyMemory(SendFourCard.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
            }

			if (pIServerUserItem->GetUserStatus() == US_LOOKON)
			{
				tagGameStaus GameStaus;
				GameStaus.cbStatus = 2;
				m_pITableFrame->SendLookonData(wChairID, SUB_S_GAME_STAUTS, &GameStaus, sizeof(GameStaus));

				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** 场景消息 给旁观玩家 uerid = %d  发送消息 wSubCmdID = %d  SUB_S_GAME_STAUTS"), pIServerUserItem->GetUserID(), SUB_S_SEND_FOUR_CARD);
				OutputDebugString(szInfo);

				m_pITableFrame->SendLookonData(wChairID, SUB_S_SEND_FOUR_CARD, &SendFourCard, sizeof(SendFourCard));
			}
			else
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** 场景消息 给非旁观玩家 uerid = %d  发送消息 wSubCmdID = %d"), pIServerUserItem->GetUserID(), SUB_S_SEND_FOUR_CARD);
				OutputDebugString(szInfo);
				m_pITableFrame->SendTableData(wChairID, SUB_S_SEND_FOUR_CARD, &SendFourCard, sizeof(SendFourCard));
			}
           
			
        }
		

        //金币场和金币房卡可以托管，积分房卡也托管
		//断线回来取消代打
        //if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
        //        || (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
        {
            //重连取消托管标志
            pIServerUserItem->SetTrusteeUser(false);
            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
        }

        return bResult;
    }
    case GS_TK_SCORE:	//下注状态
    {
        //构造数据
        CMD_S_StatusScore StatusScore;
        memset(&StatusScore, 0, sizeof(StatusScore));

        //历史积分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusScore.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusScore.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //加注信息
        StatusScore.lCellScore = m_pITableFrame->GetCellScore();
        StatusScore.lTurnMaxScore = m_lTurnMaxScore[wChairID];
        StatusScore.wBankerUser = m_wBankerUser;
        StatusScore.cbDynamicJoin = m_cbDynamicJoin[wChairID];
        CopyMemory(StatusScore.cbPlayStatus, m_cbPlayStatus, sizeof(StatusScore.cbPlayStatus));
		for (int i = 0; i < MAX_CARD_TYPE; ++i)
			StatusScore.wCardTypeTimes[i] = m_GameLogic.m_cbCardTypeTimes[i];
        //发四等五
        if(m_stConfig == ST_SENDFOUR_)
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }

                //派发扑克(开始只发四张牌)
                CopyMemory(StatusScore.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
            }
        }

        StatusScore.ctConfig = m_ctConfig;
        StatusScore.stConfig = m_stConfig;
        StatusScore.bgtConfig = m_bgtConfig;
        StatusScore.btConfig = m_btConfig;
        StatusScore.gtConfig = m_gtConfig;

        CopyMemory(StatusScore.lFreeConfig, m_lFreeConfig, sizeof(StatusScore.lFreeConfig));
        CopyMemory(StatusScore.lPercentConfig, m_lPercentConfig, sizeof(StatusScore.lPercentConfig));
        CopyMemory(StatusScore.lPlayerBetBtEx, m_lPlayerBetBtEx, sizeof(StatusScore.lPlayerBetBtEx));

        //设置积分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE) { continue; }
            StatusScore.lTableScore[i] = m_lTableScore[i];
        }

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
        CopyMemory(&StatusScore.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
        StatusScore.wGamePlayerCountRule = pGameRule[1];
        StatusScore.cbAdmitRevCard = m_cbAdmitRevCard;
        StatusScore.cbPlayMode = m_pITableFrame->GetDataBaseMode();

        //防调试
        if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
        {
            StatusScore.bIsAllowAvertDebug = true;
        }

		CopyMemory(StatusScore.cbCallBankerTimes, m_cbCallBankerTimes, sizeof(StatusScore.cbCallBankerTimes));

        //更新房间用户信息
        UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);


        //房卡模式
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));

            CMD_S_RoomCardRecord RoomCardRecord;
            ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        }

		//金币场和金币房卡可以托管，积分房卡也托管
		//断线回来取消代打
		//if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
		//        || (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
        {
            //重连取消托管标志
            pIServerUserItem->SetTrusteeUser(false);
            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
        }

		StatusScore.nTimeStart = m_nTimeStart;
		StatusScore.nTimeCallBanker = m_nTimeCallBanker;
		StatusScore.nTimeAddScore = m_nTimeAddScore;
		StatusScore.nTimeOpenCard = m_nTimeOpenCard;

        //发送场景
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusScore, sizeof(StatusScore));
    }
    case GS_TK_PLAYING:	//游戏状态
	case GS_TK_WAITEND: //结算状态
    {
        //构造数据
        CMD_S_StatusPlay StatusPlay;
        memset(&StatusPlay, 0, sizeof(StatusPlay));

        //历史积分
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusPlay.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusPlay.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //设置信息
        StatusPlay.lCellScore = m_pITableFrame->GetCellScore();
        StatusPlay.lTurnMaxScore = m_lTurnMaxScore[wChairID];
        StatusPlay.wBankerUser = m_wBankerUser;
        StatusPlay.cbDynamicJoin = m_cbDynamicJoin[wChairID];
        CopyMemory(StatusPlay.bOpenCard, m_bOpenCard, sizeof(StatusPlay.bOpenCard));
        CopyMemory(StatusPlay.bSpecialCard, m_bSpecialCard, sizeof(StatusPlay.bSpecialCard));
        CopyMemory(StatusPlay.cbOriginalCardType, m_cbOriginalCardType, sizeof(StatusPlay.cbOriginalCardType));
        CopyMemory(StatusPlay.cbCombineCardType, m_cbCombineCardType, sizeof(StatusPlay.cbCombineCardType));

        CopyMemory(StatusPlay.cbPlayStatus, m_cbPlayStatus, sizeof(StatusPlay.cbPlayStatus));

        StatusPlay.ctConfig = m_ctConfig;
        StatusPlay.stConfig = m_stConfig;
        StatusPlay.bgtConfig = m_bgtConfig;
        StatusPlay.btConfig = m_btConfig;
        StatusPlay.gtConfig = m_gtConfig;

        CopyMemory(StatusPlay.lFreeConfig, m_lFreeConfig, sizeof(StatusPlay.lFreeConfig));
        CopyMemory(StatusPlay.lPercentConfig, m_lPercentConfig, sizeof(StatusPlay.lPercentConfig));
        CopyMemory(StatusPlay.lPlayerBetBtEx, m_lPlayerBetBtEx, sizeof(StatusPlay.lPlayerBetBtEx));

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
        StatusPlay.wGamePlayerCountRule = pGameRule[1];
        StatusPlay.cbAdmitRevCard = m_cbAdmitRevCard;
        StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();
		for (int i = 0; i < MAX_CARD_TYPE; ++i)
			StatusPlay.wCardTypeTimes[i] = m_GameLogic.m_cbCardTypeTimes[i];
        //防调试
        if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
        {
            StatusPlay.bIsAllowAvertDebug = true;
        }

        //设置扑克
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE) { continue; }
            WORD j = i;
            StatusPlay.lTableScore[j] = m_lTableScore[j];
            CopyMemory(StatusPlay.cbHandCardData[j], m_cbHandCardData[j], MAX_CARDCOUNT);
        }

		CopyMemory(StatusPlay.cbCallBankerTimes, m_cbCallBankerTimes, sizeof(StatusPlay.cbCallBankerTimes));

        //更新房间用户信息
        UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

        //房卡模式
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));

            CMD_S_RoomCardRecord RoomCardRecord;
            ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        }

		//金币场和金币房卡可以托管，积分房卡也托管
		//断线回来取消代打
		//if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
		//        || (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
        {
            //重连取消托管标志
            pIServerUserItem->SetTrusteeUser(false);
            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
        }

		//当前tm
		DWORD dwCurTickCount = (DWORD)time(NULL);
		DWORD dwDTmVal = dwCurTickCount - m_GameEndEx.dwTickCountGameEnd;

		//if (dwDTmVal >= 1 && dwDTmVal <= TIME_DELAY_GAMEFREE / 1000 && !pIServerUserItem->IsAndroidUser())
		{
			StatusPlay.bDelayFreeDynamicJoin = m_pITableFrame->GetGameStatus() == GS_TK_WAITEND ? true : false;
		}

		CopyMemory(StatusPlay.lTurnScore, m_GameEndEx.GameEnd.lGameScore, sizeof(StatusPlay.lTurnScore));

		StatusPlay.nTimeStart = m_nTimeStart;
		StatusPlay.nTimeCallBanker = m_nTimeCallBanker;
		StatusPlay.nTimeAddScore = m_nTimeAddScore;
		StatusPlay.nTimeOpenCard = m_nTimeOpenCard;

        //发送场景
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
    }
    }
    //效验错误
    ASSERT(FALSE);

    return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{
    switch(dwTimerID)
    {
	case IDI_DELAY_GAMEFREE:
	{
		//删除时间
		m_pITableFrame->KillGameTimer(IDI_DELAY_GAMEFREE);

		BOOL bEndLoop = *((BOOL*)wBindParam);
		m_pITableFrame->ConcludeGame(GS_TK_FREE, bEndLoop);

		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			IServerUserItem* pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (!pIServerUserItem || !pIServerUserItem->IsClientReady())
			{
				continue;
			}

			m_pITableFrame->SendTableData(i, SUB_S_GAMEFREE_NOTIFY);
			m_pITableFrame->SendLookonData(i, SUB_S_GAMEFREE_NOTIFY);
		}

		return true;
	}
    case IDI_SO_OPERATE:
    {
        //删除时间
        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
		//return true;

        //游戏状态
        switch(m_pITableFrame->GetGameStatus())
        {
        case GS_TK_CALL:			//用户叫庄
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] != TRUE)
                {
                    continue;
                }
                if(m_cbCallBankerStatus[i] == TRUE)
                {
                    continue;
                }

                if(i == m_wBgtRobNewTurnChairID && m_wBgtRobNewTurnChairID != INVALID_CHAIR)
                {
                    OnUserCallBanker(i, true, 1);
                }
                else
                {
                    OnUserCallBanker(i, false, m_cbPrevCallBankerTimes[i]);
                }
            }

            break;
        }
        case GS_TK_SCORE:			//下注操作
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_lTableScore[i] > 0L || m_cbPlayStatus[i] == FALSE || i == m_wBankerUser)
                {
                    continue;
                }

                if(m_lTurnMaxScore[i] > 0)
                {
                    if(m_btConfig == BT_FREE_)
                    {
                        OnUserAddScore(i, m_lFreeConfig[0] * m_pITableFrame->GetCellScore());
                    }
                    else if(m_btConfig == BT_PENCENT_)
                    {
                        OnUserAddScore(i, m_lTurnMaxScore[i] * m_lPercentConfig[0] / 100);
                    }
                }
                else
                {
                    OnUserAddScore(i, 1);
                }
            }

            break;
        }
        case GS_TK_PLAYING:			//用户开牌
        {
										for (WORD i = 0; i < m_wPlayerCount; i++)
										{
											if (m_bOpenCard[i] == true || m_cbPlayStatus[i] == FALSE)
											{
												continue;
											}
											//AI自动操作
											IServerUserItem* pUserItem = m_pITableFrame->GetTableUserItem(i);
											if (pUserItem == nullptr)
												continue;
											if (!pUserItem->IsAndroidUser())
												continue;

											//获取牛牛牌型
											BYTE cbTempHandCardData[MAX_CARDCOUNT];
											ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
											CopyMemory(cbTempHandCardData, m_cbHandCardData[i], sizeof(m_cbHandCardData[i]));

											m_GameLogic.GetOxCard(cbTempHandCardData, MAX_CARDCOUNT);

											OnUserOpenCard(i, cbTempHandCardData);
										}
            break;
        }
        default:
        {
            break;
        }
        }

        if(m_pITableFrame->GetGameStatus() != GS_TK_FREE)
        {
            m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, TIME_SO_OPERATE, 1, 0);
        }
        return true;
    }
    case IDI_OFFLINE_TRUSTEE_0:
    case IDI_OFFLINE_TRUSTEE_1:
    case IDI_OFFLINE_TRUSTEE_2:
    case IDI_OFFLINE_TRUSTEE_3:
    case IDI_OFFLINE_TRUSTEE_4:
    case IDI_OFFLINE_TRUSTEE_5:
    case IDI_OFFLINE_TRUSTEE_6:
    case IDI_OFFLINE_TRUSTEE_7:
    {
        m_pITableFrame->KillGameTimer(dwTimerID);
        WORD wOfflineTrustee = dwTimerID - IDI_OFFLINE_TRUSTEE_0;

        if(m_pITableFrame->GetTableUserItem(wOfflineTrustee) && m_pITableFrame->GetTableUserItem(wOfflineTrustee)->IsTrusteeUser())
        {
            //游戏状态
            switch(m_pITableFrame->GetGameStatus())
            {
            case GS_TK_CALL:			//用户叫庄
            {
                if(m_cbPlayStatus[wOfflineTrustee] != TRUE)
                {
                    break;
                }
                if(m_cbCallBankerStatus[wOfflineTrustee] == TRUE)
                {
                    break;
                }

                if(wOfflineTrustee == m_wBgtRobNewTurnChairID && m_wBgtRobNewTurnChairID != INVALID_CHAIR)
                {
                    OnUserCallBanker(wOfflineTrustee, true, 1);
                }
                else
                {
                    OnUserCallBanker(wOfflineTrustee, false, 0);
                }

                break;
            }
            case GS_TK_SCORE:			//下注操作
            {
                if(m_lTableScore[wOfflineTrustee] > 0L || m_cbPlayStatus[wOfflineTrustee] == FALSE || wOfflineTrustee == m_wBankerUser)
                {
                    break;
                }

                if(m_lTurnMaxScore[wOfflineTrustee] > 0)
                {
                    if(m_btConfig == BT_FREE_)
                    {
                        OnUserAddScore(wOfflineTrustee, m_lFreeConfig[0] * m_pITableFrame->GetCellScore());
                    }
                    else if(m_btConfig == BT_PENCENT_)
                    {
                        OnUserAddScore(wOfflineTrustee, m_lTurnMaxScore[wOfflineTrustee] * m_lPercentConfig[0] / 100);
                    }
                }
                else
                {
                    OnUserAddScore(wOfflineTrustee, 1);
                }

                break;
            }
            case GS_TK_PLAYING:			//用户开牌
            {
                if(m_bOpenCard[wOfflineTrustee] == true || m_cbPlayStatus[wOfflineTrustee] == FALSE)
                {
                    break;
                }

                //获取牛牛牌型
                BYTE cbTempHandCardData[MAX_CARDCOUNT];
                ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
                CopyMemory(cbTempHandCardData, m_cbHandCardData[wOfflineTrustee], sizeof(m_cbHandCardData[wOfflineTrustee]));

                m_GameLogic.GetOxCard(cbTempHandCardData, MAX_CARDCOUNT);

                OnUserOpenCard(wOfflineTrustee, cbTempHandCardData);

                break;
            }
            default:
            {
                break;
            }
            }
        }

        return true;
    }
    case IDI_TIME_ELAPSE:
    {
        if(m_cbTimeRemain > 0)
        {
            m_cbTimeRemain--;

			//玩家操作时间多于5~8，发消息AI
			BYTE cbRandVal = 5 + rand() % 3;
			if (m_cbTimeRemain <= TIME_SO_OPERATE / 1000 - cbRandVal)
			{
				WORD wRandChairIndex = rand() % m_wPlayerCount;
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					wRandChairIndex = (wRandChairIndex + 1) % m_wPlayerCount;
					if (m_cbPlayStatus[wRandChairIndex] == FALSE && m_cbDynamicJoin[wRandChairIndex] == FALSE)
					{
						continue;
        }

					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wRandChairIndex);
					if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser() && pIServerUserItem->IsClientReady())
					{
						m_pITableFrame->SendTableData(wRandChairIndex, SUB_S_ANDROID_USEROVERTIME);
						break;
					}
				}
			}
        }

        return true;
    }
    }
    return false;
}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("处理玩家消息 wSubCmdID = %d, GameID:%d"), wSubCmdID,pIServerUserItem->GetGameID());
	//LOG_TABLE(szInfo);

    bool bResult = false;
    switch(wSubCmdID)
    {
    case SUB_C_CALL_BANKER:			//用户叫庄
    {
        //效验数据
        ASSERT(wDataSize == sizeof(CMD_C_CallBanker));
        if(wDataSize != sizeof(CMD_C_CallBanker)) { return false; }

        //变量定义
        CMD_C_CallBanker *pCallBanker = (CMD_C_CallBanker *)pDataBuffer;

        //用户效验
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        //if (pUserData->cbUserStatus!=US_PLAYING) return true;

        //状态判断
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] != TRUE)
        {
			LOG_TABLE(L"非游戏玩家");
            return false;
        }

        //消息处理
        bResult = OnUserCallBanker(pUserData->wChairID, pCallBanker->bBanker, pCallBanker->cbBankerTimes);
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
        //if (pUserData->cbUserStatus!=US_PLAYING) return true;

        //状态判断
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] != TRUE)
        {
			LOG_TABLE(L"非游戏玩家");
            return false;
        }

        //消息处理
        bResult = OnUserAddScore(pUserData->wChairID, pAddScore->lScore);
        break;
    }
    case SUB_C_OPEN_CARD:			//用户摊牌
    {
        ASSERT(wDataSize == sizeof(CMD_C_OpenCard));
        if(wDataSize != sizeof(CMD_C_OpenCard)) { return false; }

        CMD_C_OpenCard *pOpenCard = (CMD_C_OpenCard *)pDataBuffer;

        //用户效验
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();

        //状态判断
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE)
        {
			LOG_TABLE(L"非游戏玩家");
            return true;
        }

        //消息处理
        bResult = OnUserOpenCard(pUserData->wChairID, pOpenCard->cbCombineCardData);
        break;
    }
    case SUB_C_REQUEST_RCRecord:
    {
        //ASSERT((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0);
        if(!((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
        {
            return true;
        }

        if(m_pITableFrame->IsPersonalRoomDisumme())
        {
            return true;
        }

        ASSERT(pIServerUserItem->IsMobileUser());
        if(!pIServerUserItem->IsMobileUser())
        {
            return false;
        }

        CMD_S_RoomCardRecord RoomCardRecord;
        ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

        //积分房卡
        if((m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0))
        {
            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
        }
        else
        {
            WORD wChairID = pIServerUserItem->GetChairID();
            POSITION pos = m_listWinScoreRecord[wChairID].GetHeadPosition();

            WORD wIndex = 0;
            while(pos)
            {
                RoomCardRecord.lDetailScore[wChairID][wIndex++] = m_listWinScoreRecord[wChairID].GetNext(pos);

                if(wIndex >= MAX_RECORD_COUNT)
                {
                    break;
                }
            }

            RoomCardRecord.nCount = wIndex;
        }

        m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));

        return true;
    }
	case SUB_C_CONFIG_DEBUG:								//请求调试
	{
		return OnConfigDebug(pIServerUserItem,pDataBuffer,wDataSize);
	}
	case SUB_C_GET_POOL_CARD:									//获取剩余牌数据
	{
		return OnGetPoolCard(pIServerUserItem, pDataBuffer, wDataSize);
	}
	case SUB_C_REQUEST_CARDTYPE:									//请求 换牌
	{
		return OnChangeCardType(pIServerUserItem, pDataBuffer, wDataSize);
	}
	case SUB_C_CONFIG_CANCEL:
	{
								if (wDataSize != sizeof(stConfigDebug))
								{
									LOG_TABLE(L"取消调试，无效请求");
									return true;
								}
								//权限判断
								//if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
								if (!IS_DEBUG_USER(pIServerUserItem->GetUserRight()))
								{
									LOG_TABLE(L"无调试权限,玩家:", pIServerUserItem->GetGameID());
									//SendGameMessage(pIServerUserItem, L"无调试权限");
									return true;
								}

								LOG_TABLE(L"取消调试，玩家:", pIServerUserItem->GetGameID());
								m_TableDebug.RemoveConfig((stConfigDebug*)pDataBuffer);
								SendUserConfigDebug(pIServerUserItem, ((stConfigDebug*)pDataBuffer)->wDebugUserChair);
								return true;
	}
	case SUB_C_REQUEST_CURCONFIG:
	{
								if (wDataSize != sizeof(stConfigDebug))
								{
									LOG_TABLE(L"请求调试，无效请求");
									return true;
								}
								//权限判断
								//if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
								if (!IS_DEBUG_USER(pIServerUserItem->GetUserRight()))
								{
									LOG_TABLE(L"无调试权限,玩家:", pIServerUserItem->GetGameID());
									//SendGameMessage(pIServerUserItem, L"无调试权限");
									return true;
								}
								SendUserConfigDebug(pIServerUserItem, ((stConfigDebug*)pDataBuffer)->wDebugUserChair);
		return true;
	}
    }

    BYTE cbGameStatus = m_pITableFrame->GetGameStatus();

    return true;
}


//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
    // 消息处理
    if(wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
    {
		//权限判断
		if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
		{
			return false;
		}

        switch(wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
        {
		case SUB_C_ANCHOU:
        {

			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_AnChou));
			if (wDataSize != sizeof(CMD_C_AnChou))
            {
                return false;
            }


			CMD_C_AnChou *pAnChou = (CMD_C_AnChou *)pData;

			g_lCurAnChouRate = pAnChou->lConfigAnChouRate;

			CMD_S_AnChou AnChou;
			ZeroMemory(&AnChou, sizeof(AnChou));

			AnChou.lCurAnChouRate = g_lCurAnChouRate;
			AnChou.lCurAnChouTotal = g_lCurAnChouTotal;

			//发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ANCHOU, &AnChou, sizeof(AnChou));

			// 定义信息
			CMD_S_DebugText DebugText;
			ZeroMemory(&DebugText, sizeof(DebugText));

			TCHAR szMessageText[256] = TEXT("抽水配置成功!\n");
			CopyMemory(DebugText.szMessageText, szMessageText, sizeof(szMessageText));

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_DEBUGTEXT_GENERAL, &DebugText, sizeof(DebugText));

			return true;
		}
		case SUB_C_SYS_CTRL:
            {

			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_SysCtrl));
			if (wDataSize != sizeof(CMD_C_SysCtrl))
			{
				return false;
            }


			CMD_C_SysCtrl *pSysCtrl = (CMD_C_SysCtrl *)pData;

			//寻找正在执行的列表(有且只有一个正在执行的列表)
			POSITION pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				if (sysctrl.sysCtrlStatus == PROGRESSINGEX)
				{
					sysctrl.sysCtrlStatus = (sysctrl.lSysCtrlSysWin == 0) ? CANCELEX : EXECUTE_CANCELEX;

					break;
				}
			}

			//插入系统控制列表
			g_dwSysCtrlIncreaseIndex++;

			SYSCTRL sysctrl;
			ZeroMemory(&sysctrl, sizeof(sysctrl));

			sysctrl.dwCtrlIndex = g_dwSysCtrlIncreaseIndex;
			sysctrl.lSysCtrlConfigSysStorage = pSysCtrl->lSysCtrlSysStorage;
			sysctrl.lSysCtrlConfigPlayerStorage = pSysCtrl->lSysCtrlPlayerStorage;
			sysctrl.lSysCtrlCurSysStorage = pSysCtrl->lSysCtrlSysStorage;;
			sysctrl.lSysCtrlCurPlayerStorage = pSysCtrl->lSysCtrlPlayerStorage;
			sysctrl.lSysCtrlParameterK = pSysCtrl->lSysCtrlParameterK;
			sysctrl.lSysCtrlSysStorResetRate = pSysCtrl->lSysCtrlSysStorResetRate;
			sysctrl.lSysCtrlResetCount = 0;
			sysctrl.lSysCtrlSysWin = 0;
			sysctrl.tmResetTime = CTime::GetCurrentTime();
			sysctrl.sysCtrlStatus = PROGRESSINGEX;

			//插入列表
			m_listSysCtrl.AddTail(sysctrl);

            //变量定义
			CMD_S_CurSysCtrlInfo CurSysCtrlInfo;
			ZeroMemory(&CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

			CurSysCtrlInfo.lSysCtrlCurSysStorage = pSysCtrl->lSysCtrlSysStorage;
			CurSysCtrlInfo.lSysCtrlCurPlayerStorage = sysctrl.lSysCtrlCurPlayerStorage;
			CurSysCtrlInfo.lSysCtrlParameterK = pSysCtrl->lSysCtrlParameterK;
			CurSysCtrlInfo.lSysCtrlSysStorResetRate = pSysCtrl->lSysCtrlSysStorResetRate;
			CurSysCtrlInfo.lSysCtrlResetCount = 0;
			CurSysCtrlInfo.lSysCtrlSysWin = CalListSysCtrlSysWin();

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_SYSCTRL_INFO, &CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

			// 定义信息
			CMD_S_DebugText DebugText;
			ZeroMemory(&DebugText, sizeof(DebugText));

			TCHAR szMessageText[256] = TEXT("系统控制库存配置成功!\n");
			CopyMemory(DebugText.szMessageText, szMessageText, sizeof(szMessageText));

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_DEBUGTEXT_GENERAL, &DebugText, sizeof(DebugText));

			return true;
		}
		case SUB_C_REFRESH_CUR_SYSCTRL_INFO:
            {
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
			}

			//变量定义
			CMD_S_CurSysCtrlInfo CurSysCtrlInfo;
			ZeroMemory(&CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

			//寻找正在执行的列表(有且只有一个正在执行的列表)
			POSITION pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				if (sysctrl.sysCtrlStatus == PROGRESSINGEX)
				{
					CurSysCtrlInfo.lSysCtrlCurSysStorage = sysctrl.lSysCtrlCurSysStorage;
					CurSysCtrlInfo.lSysCtrlCurPlayerStorage = sysctrl.lSysCtrlCurPlayerStorage;
					CurSysCtrlInfo.lSysCtrlParameterK = sysctrl.lSysCtrlParameterK;
					CurSysCtrlInfo.lSysCtrlSysStorResetRate = sysctrl.lSysCtrlSysStorResetRate;
					CurSysCtrlInfo.lSysCtrlResetCount = sysctrl.lSysCtrlResetCount;
					CurSysCtrlInfo.lSysCtrlSysWin = CalListSysCtrlSysWin();

					break;
            }
			}


            //发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_SYSCTRL_INFO, &CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

            return true;
        }
		case SUB_C_ROOM_CTRL:
        {
			ASSERT(wDataSize == sizeof(CMD_C_RoomCtrl));
			if (wDataSize != sizeof(CMD_C_RoomCtrl))
            {
                return false;
            }


			CMD_C_RoomCtrl *pRoomCtrl = (CMD_C_RoomCtrl *)pData;


			//配置调试
			if (!pRoomCtrl->bCancelRoomCtrl)
            {
				//新建调试
				if (pRoomCtrl->dwSelCtrlIndex == INVALID_DWORD)
				{
					//寻找正在执行的列表(有且只有一个正在执行的列表)
					POSITION pos = m_listRoomCtrl.GetHeadPosition();
					while (pos)
					{
						ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

						if (roomctrl.roomCtrlStatus == QUEUE || roomctrl.roomCtrlStatus == PROGRESSING)
						{
							roomctrl.roomCtrlStatus = (roomctrl.lRoomCtrlSysWin == 0) ? CANCEL : EXECUTE_CANCEL;
            }
					}

					ROOMCTRL roomctrl;
					ZeroMemory(&roomctrl, sizeof(roomctrl));

					g_dwRoomCtrlIncreaseIndex++;

					roomctrl.dwCtrlIndex = g_dwRoomCtrlIncreaseIndex;
					roomctrl.lRoomCtrlConfigSysStorage = pRoomCtrl->lRoomCtrlCurSysStorage;
					roomctrl.lRoomCtrlConfigPlayerStorage = pRoomCtrl->lRoomCtrlCurPlayerStorage;
					roomctrl.lRoomCtrlCurSysStorage = pRoomCtrl->lRoomCtrlCurSysStorage;
					roomctrl.lRoomCtrlCurPlayerStorage = pRoomCtrl->lRoomCtrlCurPlayerStorage;
					roomctrl.lRoomCtrlParameterK = pRoomCtrl->lRoomCtrlParameterK;
					roomctrl.lRoomCtrlSysStorResetRate = pRoomCtrl->lRoomCtrlSysStorResetRate;
					roomctrl.lRoomCtrlSysWin = 0;
					roomctrl.tmConfigTime = CTime::GetCurrentTime();
					roomctrl.roomCtrlStatus = QUEUE;
					roomctrl.dwDebugUserGameID = pIServerUserItem->GetGameID();

					//插入列表
					m_listRoomCtrl.AddTail(roomctrl);
				}
			}
			//取消调试
			else
			{
				//查找房间控制
				POSITION posKey = FindRoomCtrl(pRoomCtrl->dwSelCtrlIndex);
				if (posKey)
				{
					//设置房间记录状态
					ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(posKey);

 
					roomctrl.roomCtrlStatus = (roomctrl.lRoomCtrlSysWin == 0) ? CANCEL : EXECUTE_CANCEL;
				}
			}

			//房间调试列表
			POSITION pos = m_listRoomCtrl.GetHeadPosition();
			POSITION firstQueueCtrlPos = NULL;
			bool bSendCurRoomCtrlInfo = false;

			while (pos)
            {

				POSITION posTemp = pos;
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//发送结果
					CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
					ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

					CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
					CurRoomCtrlInfo.lRoomCtrlCurSysStorage = roomctrl.lRoomCtrlCurSysStorage;
					CurRoomCtrlInfo.lRoomCtrlCurPlayerStorage = roomctrl.lRoomCtrlCurPlayerStorage;
					CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
					CurRoomCtrlInfo.lRoomCtrlSysStorResetRate = roomctrl.lRoomCtrlSysStorResetRate;
					CurRoomCtrlInfo.lRoomCtrlSysWin = CalListRoomCtrlSysWin();

					// 发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
					firstQueueCtrlPos = NULL;
					bSendCurRoomCtrlInfo = true;

					break;
            }
				else if (roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
				{
					firstQueueCtrlPos = posTemp;
				}
			}

			//若没有一条在进行中状态，发送第一条队列
			if (firstQueueCtrlPos)
			{
				ROOMCTRL &temproomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);

				//发送结果
				CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
				ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

				CurRoomCtrlInfo.dwCtrlIndex = temproomctrl.dwCtrlIndex;
				CurRoomCtrlInfo.lRoomCtrlCurSysStorage = temproomctrl.lRoomCtrlCurSysStorage;
				CurRoomCtrlInfo.lRoomCtrlCurPlayerStorage = temproomctrl.lRoomCtrlCurPlayerStorage;
				CurRoomCtrlInfo.lRoomCtrlParameterK = temproomctrl.lRoomCtrlParameterK;
				CurRoomCtrlInfo.lRoomCtrlSysStorResetRate = temproomctrl.lRoomCtrlSysStorResetRate;
				CurRoomCtrlInfo.lRoomCtrlSysWin = CalListRoomCtrlSysWin();

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
				bSendCurRoomCtrlInfo = true;
        }

			if (!bSendCurRoomCtrlInfo)
        {
				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO);
			}

			//当前房间列表信息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_ROOMCTRL_INFO);
			pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
            {
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == QUEUE || roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//发送结果
					CMD_S_ListCurRoomCtrlInfo ListCurRoomCtrlInfo;
					ZeroMemory(&ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));

					CopyMemory(&(ListCurRoomCtrlInfo.curRoomCtrlListInfo), &roomctrl, sizeof(roomctrl));

					// 发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_ROOMCTRL_INFO, &ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));
            }
			}

			return true;
		}
		case SUB_C_REFRESH_CUR_ROOMCTRL_INFO:
            {
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
                return false;
            }

			//房间调试列表
			POSITION pos = m_listRoomCtrl.GetHeadPosition();
			POSITION firstQueueCtrlPos = NULL;
			bool bSendCurRoomCtrlInfo = false;

			while (pos)
			{
				POSITION posTemp = pos;
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//发送结果
					CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
					ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

					CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
					CurRoomCtrlInfo.lRoomCtrlCurSysStorage = roomctrl.lRoomCtrlCurSysStorage;
					CurRoomCtrlInfo.lRoomCtrlCurPlayerStorage = roomctrl.lRoomCtrlCurPlayerStorage;
					CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
					CurRoomCtrlInfo.lRoomCtrlSysStorResetRate = roomctrl.lRoomCtrlSysStorResetRate;
					CurRoomCtrlInfo.lRoomCtrlSysWin = CalListRoomCtrlSysWin();

					// 发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
					firstQueueCtrlPos = NULL;
					bSendCurRoomCtrlInfo = true;

					break;
				}
				else if (roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
            {
					firstQueueCtrlPos = posTemp;
				}
			}

			//若没有一条在进行中状态，发送第一条队列
			if (firstQueueCtrlPos)
                {
				ROOMCTRL &temproomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);

				//发送结果
				CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
				ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

				CurRoomCtrlInfo.dwCtrlIndex = temproomctrl.dwCtrlIndex;
				CurRoomCtrlInfo.lRoomCtrlCurSysStorage = temproomctrl.lRoomCtrlCurSysStorage;
				CurRoomCtrlInfo.lRoomCtrlCurPlayerStorage = temproomctrl.lRoomCtrlCurPlayerStorage;
				CurRoomCtrlInfo.lRoomCtrlParameterK = temproomctrl.lRoomCtrlParameterK;
				CurRoomCtrlInfo.lRoomCtrlSysStorResetRate = temproomctrl.lRoomCtrlSysStorResetRate;
				CurRoomCtrlInfo.lRoomCtrlSysWin = CalListRoomCtrlSysWin();

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
				bSendCurRoomCtrlInfo = true;
                }

			if (!bSendCurRoomCtrlInfo)
			{
				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO);
            }


            return true;
        }
		case SUB_C_REFRESH_LIST_CUR_ROOMCTRL_INFO:
        {
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
            {
                return false;
            }
			//当前房间列表信息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_ROOMCTRL_INFO);
			POSITION pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
            {
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == QUEUE || roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//发送结果
					CMD_S_ListCurRoomCtrlInfo ListCurRoomCtrlInfo;
					ZeroMemory(&ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));

					CopyMemory(&(ListCurRoomCtrlInfo.curRoomCtrlListInfo), &roomctrl, sizeof(roomctrl));

					// 发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_ROOMCTRL_INFO, &ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));
				}
			}

			return true;
		}
		case SUB_C_QUERY_GAMEID:
		{
			ASSERT(wDataSize == sizeof(CMD_C_QueryGameID));
			if (wDataSize != sizeof(CMD_C_QueryGameID))
			{
                return false;
            }

			CMD_C_QueryGameID* pQueryGameID = (CMD_C_QueryGameID*)pData;

			ROOMUSERINFO roomuserinfo;
			ZeroMemory(&roomuserinfo, sizeof(roomuserinfo));


			if (FindRoomUserInfo(pQueryGameID->dwQueryUserID, roomuserinfo) == FALSE)
            {
				// 定义信息
				CMD_S_DebugText DebugText;
				ZeroMemory(&DebugText, sizeof(DebugText));

				_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("GAMEID[%d]玩家暂未坐下"), pQueryGameID->dwQueryGameID);

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_DEBUGTEXT_ADVANCED, &DebugText, sizeof(DebugText));

				return true;
            }

			//发送结果
			CMD_S_QueryGameID QueryGameID;
			ZeroMemory(&QueryGameID, sizeof(QueryGameID));

			CopyMemory(&(QueryGameID.QueryRoomUserInfo), &roomuserinfo, sizeof(roomuserinfo));
			QueryGameID.bgtConfig = m_bgtConfig;

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_QUERY_GAMEID, &QueryGameID, sizeof(QueryGameID));

			//牌桌调试列表
			POSITION pos = m_listTableCtrl.GetHeadPosition();
			while (pos)
            {
				TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

				if (tablectrl.roomUserInfo.dwGameID == roomuserinfo.dwGameID && tablectrl.tableCtrlStatus == TABLECTRL_PROGRESSING)
                {
					//发送结果
					CMD_S_CurTableCtrlInfo CurTableCtrlInfo;
					ZeroMemory(&CurTableCtrlInfo, sizeof(CurTableCtrlInfo));

					CurTableCtrlInfo.cbCtrlPosIndex = (BYTE)(tablectrl.roomUserInfo.tablectrl_type);
					CurTableCtrlInfo.dwCtrlIndex = tablectrl.dwCtrlIndex;
					CurTableCtrlInfo.wTableID = tablectrl.roomUserInfo.wTableID;
					CurTableCtrlInfo.wBankerChairID = tablectrl.roomUserInfo.wBankerChairID;
					CurTableCtrlInfo.wCurTablePlayerCount = tablectrl.roomUserInfo.wCurTablePlayerCount;
					CurTableCtrlInfo.cbConfigCtrlTimes = tablectrl.roomUserInfo.cbConfigCtrlTimes;
					CurTableCtrlInfo.cbCurCtrlTimes = tablectrl.roomUserInfo.cbCurCtrlTimes;
					CurTableCtrlInfo.lTableCtrlSysWin = CalListTableCtrlSysWin();

					// 发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_TABLECTRL_INFO, &CurTableCtrlInfo, sizeof(CurTableCtrlInfo));

					break;
                    }
			}

			return true;
		}
		case SUB_C_TABLE_CTRL:
                    {
			ASSERT(wDataSize == sizeof(CMD_C_TableCtrl));
			if (wDataSize != sizeof(CMD_C_TableCtrl))
			{
				return false;
			}

			CMD_C_TableCtrl* pTableCtrl = (CMD_C_TableCtrl*)pData;
			ROOMUSERINFO roomuserinfo;
			ZeroMemory(&roomuserinfo, sizeof(roomuserinfo));

			if (FindRoomUserInfo(pTableCtrl->dwCtrlUserID, roomuserinfo) == FALSE)
                        {
				// 定义信息
				CMD_S_DebugText DebugText;
				ZeroMemory(&DebugText, sizeof(DebugText));

				_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("GAMEID[%d]玩家暂未坐下"), pTableCtrl->dwCtrlGameID);

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_DEBUGTEXT_ADVANCED, &DebugText, sizeof(DebugText));

				return true;
			}

			if (roomuserinfo.wChairID == INVALID_CHAIR && (!pTableCtrl->bCancelTableCtrl))
			{
				// 定义信息
				CMD_S_DebugText DebugText;
				ZeroMemory(&DebugText, sizeof(DebugText));

				_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("GAMEID[%d]玩家椅子号无效"), pTableCtrl->dwCtrlGameID);

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_DEBUGTEXT_ADVANCED, &DebugText, sizeof(DebugText));

				return true;
			}


			//同桌调试校验
			if (TravelSameTableCtrlList(roomuserinfo) && (!pTableCtrl->bCancelTableCtrl))
                        {
				// 定义信息
				CMD_S_DebugText DebugText;
				ZeroMemory(&DebugText, sizeof(DebugText));

				_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("GAMEID[%d]玩家所在桌子已有调试"), pTableCtrl->dwCtrlGameID);

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_DEBUGTEXT_ADVANCED, &DebugText, sizeof(DebugText));

				return true;
                        }

			if ((!pTableCtrl->bCancelTableCtrl) && (((TABLECTRL_TYPE)(pTableCtrl->cbCtrlPosIndex)) == BANKER_WIN || ((TABLECTRL_TYPE)(pTableCtrl->cbCtrlPosIndex)) == BANKER_LOST) && m_wBankerUser == INVALID_CHAIR)
            {
				// 定义信息
				CMD_S_DebugText DebugText;
				ZeroMemory(&DebugText, sizeof(DebugText));

				_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("TABLE[%d]庄家未确定"), roomuserinfo.wTableID);

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_DEBUGTEXT_ADVANCED, &DebugText, sizeof(DebugText));

                        return true;
			}


			//设置牌桌
			if (!pTableCtrl->bCancelTableCtrl)
            {

				TABLECTRL tablectrl;
				ZeroMemory(&tablectrl, sizeof(tablectrl));


				g_dwTableCtrlIncreaseIndex++;

				tablectrl.dwCtrlIndex = g_dwTableCtrlIncreaseIndex;
				tablectrl.lTableCtrlSysWin = 0;
				tablectrl.tmConfigTime = CTime::GetCurrentTime();
				tablectrl.tableCtrlStatus = TABLECTRL_PROGRESSING;
				tablectrl.tablectrl_type = (TABLECTRL_TYPE)(pTableCtrl->cbCtrlPosIndex);
				tablectrl.dwDebugUserGameID = pIServerUserItem->GetGameID();

				roomuserinfo.tablectrl_type = (TABLECTRL_TYPE)(pTableCtrl->cbCtrlPosIndex);
				roomuserinfo.cbConfigCtrlTimes = pTableCtrl->cbConfigCtrlTimes;
				roomuserinfo.cbCurCtrlTimes = 0;
				
				CopyMemory(&(tablectrl.roomUserInfo), &roomuserinfo, sizeof(roomuserinfo));

				//插入列表
				m_listTableCtrl.AddTail(tablectrl);

				g_MapRoomUserInfo.SetAt(pTableCtrl->dwCtrlUserID, roomuserinfo);

				//发送结果
				CMD_S_CurTableCtrlInfo CurTableCtrlInfo;
				ZeroMemory(&CurTableCtrlInfo, sizeof(CurTableCtrlInfo));

				CurTableCtrlInfo.cbCtrlPosIndex = (BYTE)(tablectrl.roomUserInfo.tablectrl_type);
				CurTableCtrlInfo.dwCtrlIndex = tablectrl.dwCtrlIndex;
				CurTableCtrlInfo.wTableID = tablectrl.roomUserInfo.wTableID;
				CurTableCtrlInfo.wBankerChairID = tablectrl.roomUserInfo.wBankerChairID;
				CurTableCtrlInfo.wCurTablePlayerCount = tablectrl.roomUserInfo.wCurTablePlayerCount;
				CurTableCtrlInfo.cbConfigCtrlTimes = tablectrl.roomUserInfo.cbConfigCtrlTimes;
				CurTableCtrlInfo.cbCurCtrlTimes = tablectrl.roomUserInfo.cbCurCtrlTimes;
				CurTableCtrlInfo.lTableCtrlSysWin = CalListTableCtrlSysWin();

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_TABLECTRL_INFO, &CurTableCtrlInfo, sizeof(CurTableCtrlInfo));

				//当前房间列表信息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_TABLECTRL_INFO);

				//发送结果
				CMD_S_ListCurTableCtrlInfo ListCurTableCtrlInfo;
				ZeroMemory(&ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));
				CopyMemory(&(ListCurTableCtrlInfo.curTableCtrlListInfo), &tablectrl, sizeof(tablectrl));

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_TABLECTRL_INFO, &ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));
			}
			//取消牌桌
			else
                {

				//牌桌调试列表
				POSITION pos = m_listTableCtrl.GetHeadPosition();
				while (pos)
                    {

					TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

					if (tablectrl.dwCtrlIndex == pTableCtrl->dwCtrlIndex)
					{
						tablectrl.tableCtrlStatus = (tablectrl.lTableCtrlSysWin == 0) ? TABLECTRL_CANCEL : TABLECTRL_EXECUTE_CANCEL;


						// 发送消息
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_TABLECTRL_INFO);

						break;
					}
				}


				roomuserinfo.tablectrl_type = TABLECTRL_INVALID;
				g_MapRoomUserInfo.SetAt(pTableCtrl->dwCtrlUserID, roomuserinfo);

				//当前房间列表信息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_TABLECTRL_INFO);

				pos = FindTableCtrl(roomuserinfo);
				if (pos)
				{
					TABLECTRL & tablectrl = m_listTableCtrl.GetAt(pos);
					CopyMemory(&(tablectrl.roomUserInfo), &roomuserinfo, sizeof(roomuserinfo));
				}


				//发送第一条正在执行的记录
				pos = m_listTableCtrl.GetHeadPosition();
				while (pos)
                        {

					TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

					if (tablectrl.tableCtrlStatus == TABLECTRL_PROGRESSING)
					{
						//发送结果
						CMD_S_ListCurTableCtrlInfo ListCurTableCtrlInfo;
						ZeroMemory(&ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));

						CopyMemory(&(ListCurTableCtrlInfo.curTableCtrlListInfo), &tablectrl, sizeof(tablectrl));

						// 发送消息
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_TABLECTRL_INFO, &ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));

						break;
                        }
				}
			}


			//当前房间列表信息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_TABLECTRL_INFO);
			POSITION pos = m_listTableCtrl.GetHeadPosition();
			while (pos)
			{
				TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

				if (tablectrl.tableCtrlStatus == TABLECTRL_PROGRESSING)
				{
					//发送结果
					CMD_S_ListCurTableCtrlInfo ListCurTableCtrlInfo;
					ZeroMemory(&ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));

					CopyMemory(&(ListCurTableCtrlInfo.curTableCtrlListInfo), &tablectrl, sizeof(tablectrl));

					// 发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_TABLECTRL_INFO, &ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));
				}
			}

                        return true;
                    }
		case SUB_C_REFRESH_CUR_TABLECTRL_INFO:
		{
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
                }

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_TABLECTRL_INFO);

			POSITION pos = m_listTableCtrl.GetHeadPosition();
			POSITION posLast = NULL;

			while (pos)
			{
				posLast = pos;
				TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

				if (tablectrl.tableCtrlStatus == TABLECTRL_PROGRESSING)
				{
					//发送结果
					CMD_S_CurTableCtrlInfo CurTableCtrlInfo;
					ZeroMemory(&CurTableCtrlInfo, sizeof(CurTableCtrlInfo));

					CurTableCtrlInfo.cbCtrlPosIndex = (BYTE)(tablectrl.roomUserInfo.tablectrl_type);
					CurTableCtrlInfo.dwCtrlIndex = tablectrl.dwCtrlIndex;
					CurTableCtrlInfo.wTableID = tablectrl.roomUserInfo.wTableID;
					CurTableCtrlInfo.wBankerChairID = tablectrl.roomUserInfo.wBankerChairID;
					CurTableCtrlInfo.wCurTablePlayerCount = tablectrl.roomUserInfo.wCurTablePlayerCount;
					CurTableCtrlInfo.cbConfigCtrlTimes = tablectrl.roomUserInfo.cbConfigCtrlTimes;
					CurTableCtrlInfo.cbCurCtrlTimes = tablectrl.roomUserInfo.cbCurCtrlTimes;
					CurTableCtrlInfo.lTableCtrlSysWin = CalListTableCtrlSysWin();

					// 发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_TABLECTRL_INFO, &CurTableCtrlInfo, sizeof(CurTableCtrlInfo));


					posLast = NULL;

					break;
                }
			}


			//发送最近一个(不存在有效的)
			if (posLast)
			{
				TABLECTRL &tablectrl = m_listTableCtrl.GetAt(posLast);


				//发送结果
				CMD_S_CurTableCtrlInfo CurTableCtrlInfo;
				ZeroMemory(&CurTableCtrlInfo, sizeof(CurTableCtrlInfo));

				CurTableCtrlInfo.cbCtrlPosIndex = (BYTE)(tablectrl.roomUserInfo.tablectrl_type);
				CurTableCtrlInfo.dwCtrlIndex = tablectrl.dwCtrlIndex;
				CurTableCtrlInfo.wTableID = tablectrl.roomUserInfo.wTableID;
				CurTableCtrlInfo.wBankerChairID = tablectrl.roomUserInfo.wBankerChairID;
				CurTableCtrlInfo.wCurTablePlayerCount = tablectrl.roomUserInfo.wCurTablePlayerCount;
				CurTableCtrlInfo.cbConfigCtrlTimes = tablectrl.roomUserInfo.cbConfigCtrlTimes;
				CurTableCtrlInfo.cbCurCtrlTimes = tablectrl.roomUserInfo.cbCurCtrlTimes;
				CurTableCtrlInfo.lTableCtrlSysWin = CalListTableCtrlSysWin();

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_TABLECTRL_INFO, &CurTableCtrlInfo, sizeof(CurTableCtrlInfo));
            }

            return true;
        }

		case SUB_C_REFRESH_LIST_CUR_TABLECTRL_INFO:
        {

			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
            {
                return false;
            }


			//当前房间列表信息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_TABLECTRL_INFO);
			POSITION pos = m_listTableCtrl.GetHeadPosition();
			while (pos)
			{
				TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

				if (tablectrl.tableCtrlStatus == TABLECTRL_PROGRESSING)
				{
					//发送结果
					CMD_S_ListCurTableCtrlInfo ListCurTableCtrlInfo;
					ZeroMemory(&ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));

					CopyMemory(&(ListCurTableCtrlInfo.curTableCtrlListInfo), &tablectrl, sizeof(tablectrl));


					// 发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_TABLECTRL_INFO, &ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));
				}
			}

			return true;
		}
		case SUB_C_REFRESH_STATISTIC_SERVICE:
            {
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
                {
				return false;
			}


			CMD_S_StatisticService StatisticService;
			ZeroMemory(&StatisticService, sizeof(StatisticService));

			StatisticService.lStatisticSysWin = g_lStatisticSysWin;
			StatisticService.lStatisticService = g_lStatisticService;

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_STATISTIC_SERVICE, &StatisticService, sizeof(StatisticService));

			return true;
                }
		case SUB_C_REFRESH_ANCHOU:
		{
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
            }

			CMD_S_AnChou AnChou;
			ZeroMemory(&AnChou, sizeof(AnChou));

			AnChou.lCurAnChouRate = g_lCurAnChouRate;
			AnChou.lCurAnChouTotal = g_lCurAnChouTotal;

			//发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ANCHOU, &AnChou, sizeof(AnChou));

			return true;
		}
		case SUB_C_REFRESH_TRACELOG:
            {

			POSITION pos = NULL;

			//通用库存调试列表
			pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
             {
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				//发送结果
				CMD_S_SysCtrlTraceLog sysCtrlResult;
				ZeroMemory(&sysCtrlResult, sizeof(sysCtrlResult));

				CopyMemory(&(sysCtrlResult.SysCtrlTraceLog), &sysctrl, sizeof(sysctrl));

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_SYSCTRL_TRACELOG, &sysCtrlResult, sizeof(sysCtrlResult));
			}


			//房间调试列表
			pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				//发送结果
				CMD_S_RoomCtrlTraceLog roomCtrlResult;
				ZeroMemory(&roomCtrlResult, sizeof(roomCtrlResult));


				CopyMemory(&(roomCtrlResult.RoomCtrlTraceLog), &roomctrl, sizeof(roomctrl));

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_ROOMCTRL_TRACELOG, &roomCtrlResult, sizeof(roomCtrlResult));
        	}


			//牌桌调试列表
			pos = m_listTableCtrl.GetHeadPosition();
			while (pos)
        	{

				TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

				//发送结果
				CMD_S_TableCtrlTraceLog tableCtrlResult;
				ZeroMemory(&tableCtrlResult, sizeof(tableCtrlResult));

				CopyMemory(&(tableCtrlResult.TableCtrlTraceLog), &tablectrl, sizeof(tablectrl));

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_TABLECTRL_TRACELOG, &tableCtrlResult, sizeof(tableCtrlResult));
            }
			CMD_S_StartRecordTraceLog ListWinLostInfo;
			ZeroMemory(&ListWinLostInfo, sizeof(ListWinLostInfo));

			ListWinLostInfo.tmStartRecord = m_tmStartRecord;

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_STARTRECORD_TRACELOG, &ListWinLostInfo, sizeof(ListWinLostInfo));

            return true;
        }
        }
    }
    return false;
}

//叫庄事件
bool CTableFrameSink::OnUserCallBanker(WORD wChairID, bool bBanker, BYTE cbBankerTimes)
{
    //状态效验
    BYTE cbGameStatus = m_pITableFrame->GetGameStatus();

    if(cbGameStatus != GS_TK_CALL) { return true; }

    //设置变量
    m_cbCallBankerStatus[wChairID] = TRUE;
    m_cbCallBankerTimes[wChairID] = cbBankerTimes;

    //设置变量
    CMD_S_CallBankerInfo CallBanker;
    ZeroMemory(&CallBanker, sizeof(CallBanker));
	CallBanker.wChairID = wChairID;
    CopyMemory(CallBanker.cbCallBankerStatus, m_cbCallBankerStatus, sizeof(m_cbCallBankerStatus));
    CopyMemory(CallBanker.cbCallBankerTimes, m_cbCallBankerTimes, sizeof(m_cbCallBankerTimes));

    //发送数据
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
		if (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE) { continue; }
        m_pITableFrame->SendTableData(i, SUB_S_CALL_BANKERINFO, &CallBanker, sizeof(CallBanker));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BANKERINFO, &CallBanker, sizeof(CallBanker));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_CALL_BANKERINFO, &CallBanker);
    }

    //叫庄人数
    WORD wCallUserCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE) { wCallUserCount++; }
        else if(m_cbPlayStatus[i] != TRUE) { wCallUserCount++; }
    }

    //全部人叫完庄，下注开始
    if(wCallUserCount == m_wPlayerCount)
    {
		//发送开始下注状态
		tagGameStaus	GameStaus;
		GameStaus.cbStatus = GAME_NOTE;
		//用户状态
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem != NULL)
			{
				m_pITableFrame->SendTableData(i, SUB_S_GAME_STAUTS, &GameStaus, sizeof(GameStaus));
			}
		}

		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_STAUTS, &GameStaus, sizeof(GameStaus));


        CopyMemory(m_cbPrevCallBankerTimes, m_cbCallBankerTimes, sizeof(m_cbPrevCallBankerTimes));

        //
        if(m_wBgtRobNewTurnChairID == INVALID_CHAIR)
        {
            //叫庄最大倍数
            BYTE cbMaxBankerTimes = cbBankerTimes;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxBankerTimes)
                {
                    cbMaxBankerTimes = m_cbCallBankerTimes[i];
                }
            }

            //叫庄最大倍数的人数和CHAIRID
            BYTE cbMaxBankerCount = 0;
            WORD *pwMaxBankerTimesChairID = new WORD[m_wPlayerCount];
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] == cbMaxBankerTimes)
                {
                    pwMaxBankerTimesChairID[cbMaxBankerCount++] = i;
                }
            }

            ASSERT(cbMaxBankerCount <= m_wPlayerCount);
			if (cbMaxBankerCount != 0)
			{
				m_wBankerUser = pwMaxBankerTimesChairID[rand() % cbMaxBankerCount];
			}
            
            delete[] pwMaxBankerTimesChairID;
        }
        else
        {
            m_wBankerUser = m_wBgtRobNewTurnChairID;
        }

        m_bBuckleServiceCharge[m_wBankerUser] = true;

        bool bTrusteeUser = false;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                bTrusteeUser = true;
            }
        }

        //非房卡场设置定时器
        if(!IsRoomCardType()/* || bTrusteeUser*/)
        {
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
			m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, (m_nTimeAddScore + 2) * 1000, 1, 0);
        }

        //设置离线代打定时器
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
            }
        }

        //设置状态
        m_pITableFrame->SetGameStatus(GS_TK_SCORE);
		EnableTimeElapse(true, TIME_ADDSCORE);


        //获取最大下注
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] != TRUE || i == m_wBankerUser)
            {
                continue;
            }

            //下注变量
            m_lTurnMaxScore[i] = GetUserMaxTurnScore(i);
        }

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            if(m_bLastTurnBetBtEx[i] == true)
            {
                m_bLastTurnBetBtEx[i] = false;
            }
        }

        m_lPlayerBetBtEx[m_wBankerUser] = 0;

        //设置变量
        CMD_S_GameStart GameStart;
        ZeroMemory(&GameStart, sizeof(GameStart));
        GameStart.wBankerUser = m_wBankerUser;
        CopyMemory(GameStart.cbPlayerStatus, m_cbPlayStatus, sizeof(m_cbPlayStatus));

        //发四等五
        if(m_stConfig == ST_SENDFOUR_)
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }

                //派发扑克(开始只发四张牌)
                CopyMemory(GameStart.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
            }
        }

        GameStart.stConfig = m_stConfig;
        GameStart.bgtConfig = m_bgtConfig;
        GameStart.btConfig = m_btConfig;
        GameStart.gtConfig = m_gtConfig;

        CopyMemory(GameStart.lFreeConfig, m_lFreeConfig, sizeof(GameStart.lFreeConfig));
        CopyMemory(GameStart.lPercentConfig, m_lPercentConfig, sizeof(GameStart.lPercentConfig));
        CopyMemory(GameStart.lPlayerBetBtEx, m_lPlayerBetBtEx, sizeof(GameStart.lPlayerBetBtEx));

        bool bFirstRecord = true;

        WORD wRealPlayerCount = 0;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            if(!pServerUserItem)
            {
                continue;
            }

            wRealPlayerCount++;
        }

        BYTE *pGameRule = m_pITableFrame->GetGameRule();

        //最大下注
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (m_cbPlayStatus[i] == FALSE)// && m_cbDynamicJoin[i] == FALSE)
            {
				if (pServerUserItem)	 m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
                continue;
            }
            GameStart.lTurnMaxScore = m_lTurnMaxScore[i];

            m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

			//更新房间用户信息
			UpdateRoomUserInfo(pServerUserItem, USER_NULL);

            if(m_pGameVideo)
            {
                Video_GameStart video;
                ZeroMemory(&video, sizeof(video));
                video.lCellScore = m_pITableFrame->GetCellScore();
                video.wPlayerCount = wRealPlayerCount;
                video.wGamePlayerCountRule = pGameRule[1];
                video.wBankerUser = GameStart.wBankerUser;
                CopyMemory(video.cbPlayerStatus, GameStart.cbPlayerStatus, sizeof(video.cbPlayerStatus));
                video.lTurnMaxScore = GameStart.lTurnMaxScore;
                CopyMemory(video.cbCardData, GameStart.cbCardData, sizeof(video.cbCardData));
                video.ctConfig = m_ctConfig;
                video.stConfig = GameStart.stConfig;
                video.bgtConfig = GameStart.bgtConfig;
                video.btConfig = GameStart.btConfig;
                video.gtConfig = GameStart.gtConfig;

                CopyMemory(video.lFreeConfig, GameStart.lFreeConfig, sizeof(video.lFreeConfig));
                CopyMemory(video.lPercentConfig, GameStart.lPercentConfig, sizeof(video.lPercentConfig));
                CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
                video.wChairID = i;
                video.lScore = pServerUserItem->GetUserScore();
				//for (int i = 0; i < MAX_CARD_TYPE; ++i)
				//	video.wCardTypeTimes[i] = m_GameLogic.m_cbCardTypeTimes[i];
                m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video, bFirstRecord);

                if(bFirstRecord == true)
                {
                    bFirstRecord = false;
                }
            }
        }
		
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

		m_emCtrlType = GetUserCtrlType(m_wCtrlChairID, m_bWin, m_keyroomuserinfo);
    }

    return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore)
{
    //状态效验
    BYTE cbGameStatus = m_pITableFrame->GetGameStatus();

    if(cbGameStatus != GS_TK_SCORE)
    {
		LOG_TABLE(L"非下注状态");
        return true;
    }

    //庄家校验
    if(wChairID == m_wBankerUser)
    {
		LOG_TABLE(L"庄家不可下注");
        return false;
    }

    //金币效验
    if(m_cbPlayStatus[wChairID] == TRUE)
    {
		//下注筹码校验
		bool bValidBet = false;
		for (WORD i = 0; i<MAX_CONFIG - 1; i++)
		{
			if (lScore == m_lFreeConfig[i] * m_pITableFrame->GetCellScore())
			{
				bValidBet = true;
				break;
			}
		}

		if (lScore == m_lPlayerBetBtEx[wChairID])
			bValidBet = true;

		//无效下注筹码
		if (!bValidBet || lScore <= 0)
		{
			LOG_TABLE(L"无效下注");
			return false;
		}

        if(lScore > m_lTurnMaxScore[wChairID])
        {
            lScore = m_lTurnMaxScore[wChairID];
        }
    }
    else //没下注玩家强退
    {
        if(lScore != 0)
        {
			LOG_TABLE(L"非游戏玩家不可下注");
            return false;
        }
    }

    if(lScore > 0L)
    {
        //下注金币
        m_lTableScore[wChairID] = lScore;
        m_bBuckleServiceCharge[wChairID] = true;
        //构造数据
        CMD_S_AddScore AddScore;
        AddScore.wAddScoreUser = wChairID;
        AddScore.lAddScoreCount = m_lTableScore[wChairID];

        //发送数据
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
			if (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE) { continue; }
            m_pITableFrame->SendTableData(i, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_ADD_SCORE, &AddScore);
        }
    }

    if(lScore == m_lPlayerBetBtEx[wChairID])
    {
        m_bLastTurnBetBtEx[wChairID] = true;
    }

    //下注人数
    BYTE bUserCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_lTableScore[i] > 0L && m_cbPlayStatus[i] == TRUE)
        {
            bUserCount++;
        }
        else if(m_cbPlayStatus[i] == FALSE || i == m_wBankerUser)
        {
            bUserCount++;
        }
    }

	CString str;
	str.Format(L"[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]", m_cbHandCardData[wChairID][0], m_cbHandCardData[wChairID][1], m_cbHandCardData[wChairID][2], m_cbHandCardData[wChairID][3], m_cbHandCardData[wChairID][4]);
	LOG_TABLE(L"玩家:", (int)wChairID, L"调试前手牌:", str);

	//AI或者系统当庄
	bool bAIOrSysBanker = ((m_wBankerUser == INVALID_CHAIR) || (m_wBankerUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wBankerUser) && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser()));

	//获取当前共享库存子项
	GetCurSubStorageItem();

	//机器人下注
	if (m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser())
	{
		//真人坐庄，抽取机器人的暗抽和彩金
		if (!bAIOrSysBanker)
		{
			m_tagGameScoreInfo[wChairID].lInScore = lScore;

			//房间同时存在AI和真人玩家
			if (IsContainAIAndRealPlayer())
			{
				SCORE lAnChou = 0;

				//获取共享库存子项
				if (IsGroupTreasureRoom() && m_CurSubStorageItem.lCurAnChouRate > 0)
				{
					lAnChou = ((SCORE)(lScore * m_CurSubStorageItem.lCurAnChouRate) / (SCORE)100);
					m_CurSubStorageItem.lCurAnChouTotal += lAnChou;
					m_CurSubStorageItem.lCurStatisticSysWin += lAnChou;
				}
				else if (!IsGroupTreasureRoom() && g_lCurAnChouRate > 0)
				{
					lAnChou = ((SCORE)(lScore * g_lCurAnChouRate) / (SCORE)100);
					g_lCurAnChouTotal += lAnChou;
					g_lStatisticSysWin += lAnChou;
				}

				m_tagGameScoreInfo[wChairID].lAnChou += lAnChou;
			}
		}
	}
	//真人下注
	else
	{
		//AI或者系统当庄,抽取真人的暗抽和彩金
		if (bAIOrSysBanker)
		{
			m_tagGameScoreInfo[wChairID].lInScore = lScore;

			//房间同时存在AI和真人玩家
			if (IsContainAIAndRealPlayer())
			{
				SCORE lAnChou = 0;

				//获取共享库存子项
				if (IsGroupTreasureRoom() && m_CurSubStorageItem.lCurAnChouRate > 0)
				{
					lAnChou = ((SCORE)(lScore * m_CurSubStorageItem.lCurAnChouRate) / (SCORE)100);
					m_CurSubStorageItem.lCurAnChouTotal += lAnChou;
					m_CurSubStorageItem.lCurStatisticSysWin += lAnChou;
				}
				else if (!IsGroupTreasureRoom() && g_lCurAnChouRate > 0)
				{
					lAnChou = ((SCORE)(lScore * g_lCurAnChouRate) / (SCORE)100);
					g_lCurAnChouTotal += lAnChou;
					g_lStatisticSysWin += lAnChou;
				}

				m_tagGameScoreInfo[wChairID].lAnChou += lAnChou;
			}
		}
	}

	//修改共享库存子项
	ChangeCurSubStorageItem();

    //闲家全到
    if(bUserCount == m_wPlayerCount)
    {
		//发最后一张牌
		tagGameStaus	GameStaus;
		GameStaus.cbStatus = GAME_SEND_LAST_CARD;
		//用户状态
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem != NULL)
			{
				m_pITableFrame->SendTableData(i, SUB_S_GAME_STAUTS, &GameStaus, sizeof(GameStaus));
			}
		}

		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_STAUTS, &GameStaus, sizeof(GameStaus));

        bool bTrusteeUser = false;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                bTrusteeUser = true;
            }
        }

        //非房卡场设置定时器
        if(!IsRoomCardType()/* || bTrusteeUser*/)
        {
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
			m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, (m_nTimeOpenCard + 2) * 1000, 1, 0);
        }

        //设置离线代打定时器
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
            }
        }

        //设置状态
        m_pITableFrame->SetGameStatus(GS_TK_PLAYING);
		EnableTimeElapse(true, TIME_OPENCARD);

		m_emCtrlType = GetUserCtrlType(m_wCtrlChairID, m_bWin, m_keyroomuserinfo);

		if (m_emCtrlType == CLIENT_CARDCONFIG_CTRL)
		{
		if (m_stConfig == ST_SENDFOUR_)
		{
			//发第五张牌
			for (int i = 0; i < GAME_PLAYER; ++i)
			{
				m_TableDebug.GetDebugUser(i)->AddCard(m_cbHandCardData[i][MAX_CARDCOUNT - 1]);
			}
		}

		stServerConfig* pUserConfig = nullptr;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (m_cbPlayStatus[i] != TRUE)
				continue;

			pUserConfig = m_TableDebug.GetUserConfig(i);
			if (pUserConfig != nullptr && pUserConfig->stConfig.cbType == 1)
				break;
		}
		if (pUserConfig != nullptr && pUserConfig->stConfig.cbType == 1)
		{
			m_TableDebug.m_bDebugDoneChair[pUserConfig->stConfig.wDebugUserChair] = true;
			
			int64_t nCalcStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			WORD& wCurDebugChair = pUserConfig->stConfig.wDebugUserChair;
			if (m_cbPlayStatus[wCurDebugChair] && !m_bOpenCard[wCurDebugChair] && m_TableDebug.GetDebugUser(wCurDebugChair)->CanSwitchCard())
			{
				bool bWin = pUserConfig->stConfig.cbGameWin == 2 ? true : false;
				LOG_TABLE(L"开始调试结果:", bWin ? L"赢" : L"输", L"玩家椅子号:", wCurDebugChair);
				//
				bool bDebugCardType[MAX_CARD_TYPE];
				memset(bDebugCardType, 0, sizeof(bDebugCardType));
				GetCurValidType(bDebugCardType, wCurDebugChair);

				int nStartTypeIdx = EM_DEBUG_CARD_OX_0;
				int nEndTypeIdx = EM_DEBUG_CARD_MAX -1;
				if (bWin)
				{
					nStartTypeIdx = nEndTypeIdx;
					nEndTypeIdx = EM_DEBUG_CARD_OX_0;
				}
				int nDebugUserCardType = EM_DEBUG_CARD_OX_0;
				//
				int i = nStartTypeIdx;
				do
				{
					if (!bDebugCardType[i])
					{
						nStartTypeIdx > nEndTypeIdx ? --i : ++i;
						continue;
					}

					m_GameLogic.AddCards(m_cbHandCardData[wCurDebugChair],MAX_CARDCOUNT);
					GetTypeCards((emUserDebugType)i,m_cbHandCardData[wCurDebugChair]);
					m_GameLogic.RemoveCards(m_cbHandCardData[wCurDebugChair], MAX_CARDCOUNT);
					LOG_TABLE(L"控玩家牌型:",(int)i);
					nDebugUserCardType = i;
					break;
				}while (i != nEndTypeIdx);

				//控制庄家牌需要小或大于该玩家牌
				if (m_wBankerUser < GAME_PLAYER && m_wBankerUser != wCurDebugChair && m_cbPlayStatus[m_wBankerUser] && !m_bOpenCard[m_wBankerUser] && m_TableDebug.GetDebugUser(m_wBankerUser)->CanSwitchCard())
				{
					//m_TableDebug.RefreshCardPool(m_GameLogic.GetPoolData(), m_GameLogic.GetPoolLeftCount());
					memset(bDebugCardType, 0, sizeof(bDebugCardType));
					GetCurValidType(bDebugCardType, m_wBankerUser);
					nStartTypeIdx = nDebugUserCardType + (nStartTypeIdx > nEndTypeIdx ? -1 : +1);
					i = nStartTypeIdx;
					do
					{
						if (!bDebugCardType[i])
						{
							nStartTypeIdx > nEndTypeIdx ? --i : ++i;
							continue;
						}

						LOG_TABLE(L"控庄家牌型:", (int)i);
						m_GameLogic.AddCards(m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT);
						GetTypeCards((emUserDebugType)i, m_cbHandCardData[m_wBankerUser]);
						m_GameLogic.RemoveCards(m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT);
						nDebugUserCardType = i;
						break;
					} while (i != nEndTypeIdx);
				}

			}
			int64_t nCalcEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			//LOG_TABLE(L"调试完成，游戏模式:", m_stConfig == ST_SENDFOUR_ ? L"发四等五" : L"全发",L"消耗时间(ms):",nCalcEndTime-nCalcStartTime);
		}
		}
		//其他调试
		else if (m_emCtrlType == TABLE_CTRL)
		{
			ASSERT(m_wCtrlChairID != INVALID_CHAIR);

			//调试 (下注发牌和发四等五区分调试)
			if (m_pServerDebug != NULL && m_pServerDebug->DebugResult(m_cbHandCardData, m_wCtrlChairID, m_bWin, m_stConfig, m_ctConfig, m_gtConfig, m_wPlayerCount) 
				&& (m_keyroomuserinfo.cbCurCtrlTimes < m_keyroomuserinfo.cbConfigCtrlTimes))
			{
				m_keyroomuserinfo.cbCurCtrlTimes++;

				if (m_keyroomuserinfo.cbCurCtrlTimes == m_keyroomuserinfo.cbConfigCtrlTimes)
				{
					m_keyroomuserinfo.tablectrl_type = TABLECTRL_INVALID;
				}

				g_MapRoomUserInfo.SetAt(m_keyroomuserinfo.dwUserID, m_keyroomuserinfo);

				//当前房间列表信息
				TABLECTRL keytablectrl;
				ZeroMemory(&keytablectrl, sizeof(keytablectrl));

				m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_CUR_TABLECTRL_INFO);
				POSITION pos = m_listTableCtrl.GetHeadPosition();
				while (pos)
				{
					TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

					if (tablectrl.tableCtrlStatus == TABLECTRL_PROGRESSING && tablectrl.roomUserInfo.dwGameID == m_keyroomuserinfo.dwGameID)
					{
						CopyMemory(&(tablectrl.roomUserInfo), &m_keyroomuserinfo, sizeof(m_keyroomuserinfo));
						if (m_keyroomuserinfo.cbCurCtrlTimes == m_keyroomuserinfo.cbConfigCtrlTimes)
						{
							tablectrl.tableCtrlStatus = TABLECTRL_FINISH;
						}

						//发送结果
						CMD_S_ListCurTableCtrlInfo ListCurTableCtrlInfo;
						ZeroMemory(&ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));

						CopyMemory(&(ListCurTableCtrlInfo.curTableCtrlListInfo), &tablectrl, sizeof(tablectrl));

						// 发送消息
						m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_CUR_TABLECTRL_INFO, &ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));

						CopyMemory(&keytablectrl, &tablectrl, sizeof(tablectrl));
					}
					else if (tablectrl.tableCtrlStatus == TABLECTRL_PROGRESSING)
					{
						//发送结果
						CMD_S_ListCurTableCtrlInfo ListCurTableCtrlInfo;
						ZeroMemory(&ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));

						CopyMemory(&(ListCurTableCtrlInfo.curTableCtrlListInfo), &tablectrl, sizeof(tablectrl));

						// 发送消息
						m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_CUR_TABLECTRL_INFO, &ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));
					}
				}

				//发送结果
				CMD_S_CurTableCtrlInfo CurTableCtrlInfo;
				ZeroMemory(&CurTableCtrlInfo, sizeof(CurTableCtrlInfo));

				CurTableCtrlInfo.cbCtrlPosIndex = (BYTE)(keytablectrl.roomUserInfo.tablectrl_type);
				CurTableCtrlInfo.dwCtrlIndex = keytablectrl.dwCtrlIndex;
				CurTableCtrlInfo.wTableID = keytablectrl.roomUserInfo.wTableID;
				CurTableCtrlInfo.wBankerChairID = keytablectrl.roomUserInfo.wBankerChairID;
				CurTableCtrlInfo.wCurTablePlayerCount = keytablectrl.roomUserInfo.wCurTablePlayerCount;
				CurTableCtrlInfo.cbConfigCtrlTimes = keytablectrl.roomUserInfo.cbConfigCtrlTimes;
				CurTableCtrlInfo.cbCurCtrlTimes = keytablectrl.roomUserInfo.cbCurCtrlTimes;
				CurTableCtrlInfo.lTableCtrlSysWin = CalListTableCtrlSysWin();

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_TABLECTRL_INFO, &CurTableCtrlInfo, sizeof(CurTableCtrlInfo));

				//当前房间列表信息
				m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_CUR_TABLECTRL_INFO);
				pos = m_listTableCtrl.GetHeadPosition();
				while (pos)
				{
					TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

					if (tablectrl.tableCtrlStatus == TABLECTRL_PROGRESSING)
					{
						//发送结果
						CMD_S_ListCurTableCtrlInfo ListCurTableCtrlInfo;
						ZeroMemory(&ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));

						CopyMemory(&(ListCurTableCtrlInfo.curTableCtrlListInfo), &tablectrl, sizeof(tablectrl));

						// 发送消息
						m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_CUR_TABLECTRL_INFO, &ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));
					}
				}
			}
		}
		//其他调试
		else
		{
			//获取用户调试类型
			bool bUserCtrl = false;
			SCORE lKeyPlayerStorage = 0;
			SCORE lBankerWinScore = 0;

			WORD wUserWinChance = INVALID_WORD;
			SCORE lDVal = 0;
			SCORE lMaxCtrlStorage = 0;
			SCORE lSysCtrlParameterK = 0;
			bool bSysWin = false;

			switch (m_emCtrlType)
			{
			case ROOM_CTRL:
			{
				if (IsGroupTreasureRoom())
				{
					
					SCORE lTempRoomCtrlSysStorage = m_CurSubStorageItem.lCurTempSysStorage;
					SCORE lTempRoomCtrlPlayerStorage = m_CurSubStorageItem.lCurTempPlayerStorage;

					//绝对差值
					lDVal = abs(lTempRoomCtrlSysStorage - lTempRoomCtrlPlayerStorage);
					lMaxCtrlStorage = max(lTempRoomCtrlSysStorage, lTempRoomCtrlPlayerStorage);
					lSysCtrlParameterK = m_CurSubStorageItem.lCurTempParameterK;
					bSysWin = lTempRoomCtrlSysStorage > lTempRoomCtrlPlayerStorage;
					lKeyPlayerStorage = lTempRoomCtrlPlayerStorage;

					//系统和玩家库存其中一个配置0
					if (lMaxCtrlStorage == lDVal)
					{
						wUserWinChance = (lTempRoomCtrlSysStorage > 0 && lTempRoomCtrlPlayerStorage == 0) ? 0 : 70;
					}
				}
				else
				{
					//查找房间调试
					POSITION pos = m_listRoomCtrl.GetHeadPosition();
					while (pos)
					{
						ROOMCTRL roomctrl = m_listRoomCtrl.GetNext(pos);

						if (roomctrl.roomCtrlStatus == PROGRESSING)
						{
							//小于0当成0算概率
							SCORE lTempRoomCtrlSysStorage = roomctrl.lRoomCtrlCurSysStorage;
							SCORE lTempRoomCtrlPlayerStorage = roomctrl.lRoomCtrlCurPlayerStorage;

							lTempRoomCtrlSysStorage = max(0, lTempRoomCtrlSysStorage);
							lTempRoomCtrlPlayerStorage = max(0, lTempRoomCtrlPlayerStorage);

							//绝对差值
							lDVal = abs(lTempRoomCtrlSysStorage - lTempRoomCtrlPlayerStorage);
							lMaxCtrlStorage = max(lTempRoomCtrlSysStorage, lTempRoomCtrlPlayerStorage);
							lSysCtrlParameterK = roomctrl.lRoomCtrlParameterK;
							bSysWin = lTempRoomCtrlSysStorage > lTempRoomCtrlPlayerStorage;
							lKeyPlayerStorage = lTempRoomCtrlPlayerStorage;

							//系统和玩家库存其中一个配置0
							if (lMaxCtrlStorage == lDVal)
							{
								wUserWinChance = (lTempRoomCtrlSysStorage > 0 && lTempRoomCtrlPlayerStorage == 0) ? 0 : 70;
							}

							break;
						}
					}
				}
			
				break;
			}
			case SYS_CTRL:
			{
				if (IsGroupTreasureRoom())
				{
					SCORE lTempCurSysStorage = m_CurSubStorageItem.lCurSysStorage;
					SCORE lTempCurPlayerStorage = m_CurSubStorageItem.lCurPlayerStorage;

					//绝对差值
					lDVal = abs(lTempCurSysStorage - lTempCurPlayerStorage);
					lMaxCtrlStorage = max(lTempCurSysStorage, lTempCurPlayerStorage);
					lSysCtrlParameterK = m_CurSubStorageItem.lCurParameterK;
					bSysWin = lTempCurSysStorage > lTempCurPlayerStorage;
					lKeyPlayerStorage = lTempCurPlayerStorage;
				}
				else
				{
					//查找系统调试
					POSITION pos = m_listSysCtrl.GetHeadPosition();
					while (pos)
					{
						SYSCTRL sysctrl = m_listSysCtrl.GetNext(pos);

						if (sysctrl.sysCtrlStatus == PROGRESSINGEX)
						{
							//小于0当成0算概率
							SCORE lTempCurSysStorage = sysctrl.lSysCtrlCurSysStorage;
							SCORE lTempCurPlayerStorage = sysctrl.lSysCtrlCurPlayerStorage;

							lTempCurSysStorage = max(0, lTempCurSysStorage);
							lTempCurPlayerStorage = max(0, lTempCurPlayerStorage);

							//绝对差值
							lDVal = abs(lTempCurSysStorage - lTempCurPlayerStorage);
							lMaxCtrlStorage = max(lTempCurSysStorage, lTempCurPlayerStorage);
							lSysCtrlParameterK = sysctrl.lSysCtrlParameterK;
							bSysWin = lTempCurSysStorage > lTempCurPlayerStorage;
							lKeyPlayerStorage = lTempCurPlayerStorage;

							//系统和玩家库存其中一个配置0
							if (lMaxCtrlStorage == lDVal)
							{
								wUserWinChance = (lTempCurSysStorage > 0 && lTempCurPlayerStorage == 0) ? 0 : 70;
							}

							break;
						}
					}
				}
			

				break;
			}
			default:
				break;
			}

			if (wUserWinChance == INVALID_WORD)
			{
				//区间概率判断
				if (lDVal <= lMaxCtrlStorage * lSysCtrlParameterK / 100)
				{
					wUserWinChance = bSysWin ? 15 : 20;
				}
				else if (lDVal > lMaxCtrlStorage * lSysCtrlParameterK / 100 && lDVal <= 1.5 * lMaxCtrlStorage * lSysCtrlParameterK / 100)
				{
					wUserWinChance = bSysWin ? 12 : 30;
				}
				else if (lDVal > 1.5 * lMaxCtrlStorage * lSysCtrlParameterK / 100 && lDVal <= 2 * lMaxCtrlStorage * lSysCtrlParameterK / 100)
				{
					wUserWinChance = bSysWin ? 9 : 40;
				}
				else if (lDVal > 2 * lMaxCtrlStorage * lSysCtrlParameterK / 100 && lDVal <= 3 * lMaxCtrlStorage * lSysCtrlParameterK / 100)
				{
					wUserWinChance = bSysWin ? 7 : 50;
				}
				else if (lDVal > 3 * lMaxCtrlStorage * lSysCtrlParameterK / 100)
				{
					wUserWinChance = bSysWin ? 0 : 70;
				}
			}

			BYTE cbSysWin = INVALID_BYTE;
			BYTE cbRandVal = rand() % 100;

			cbSysWin = cbRandVal < wUserWinChance ? FALSE : TRUE;

			//分析牌数据，下注发牌和发四等五区分处理
			UINT nCirCount = 0;
			while (true)
			{
				if (nCirCount > 50000)
				{
					break;
				}

				nCirCount++;
				bool AnalyseCardValid = true;

				AnalyseCard(m_stConfig, cbSysWin);

				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (m_cbPlayStatus[i] == TRUE && m_cbHandCardData[i][0] == 0 && m_cbHandCardData[i][1] == 0 && m_cbHandCardData[i][2] == 0)
					{
						AnalyseCardValid = false;
						break;
					}
				}

				//当牌数据无效，可能都是【0】【0】【0】【0】【0】
				if (AnalyseCardValid == false)
				{
					//随机扑克
					BYTE bTempArray[GAME_PLAYER * MAX_CARDCOUNT];
					m_GameLogic.RandCardList(bTempArray, sizeof(bTempArray), (m_gtConfig == GT_HAVEKING_ ? true : false));

					for (WORD i = 0; i < m_wPlayerCount; i++)
					{
						if (m_cbPlayStatus[i] == TRUE)
						{
							//派发扑克
							CopyMemory(m_cbHandCardData[i], &bTempArray[i * MAX_CARDCOUNT], MAX_CARDCOUNT);
						}
					}
				}
				else
				{
					break;
				}
			}
		}

		/////////
		for (int i = 0; i < m_wPlayerCount; ++i)
		{
			if (m_cbPlayStatus[i] == FALSE)
			{
				continue;
			}

			memcpy(m_cbOriginalCardData[i], m_cbHandCardData[i], sizeof(BYTE)*MAX_CARDCOUNT);

			m_cbOriginalCardType[i] = m_TableDebug.GetCardHelper().GetType(m_cbHandCardData[i]);
			m_bSpecialCard[i] = m_cbOriginalCardType[i] > CT_CLASSIC_OX_VALUENIUNIU ? true : false;
		}

        //构造数据
        CMD_S_SendCard SendCard;
        ZeroMemory(SendCard.cbCardData, sizeof(SendCard.cbCardData));

		CopyMemory(SendCard.cbCardData, m_cbOriginalCardData, sizeof(SendCard.cbCardData));
        CopyMemory(SendCard.bSpecialCard, m_bSpecialCard, sizeof(SendCard.bSpecialCard));
        CopyMemory(SendCard.cbOriginalCardType, m_cbOriginalCardType, sizeof(SendCard.cbOriginalCardType));

        //发送数据
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
			if (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

			if (m_cbDynamicJoin[i] == TRUE)
			{
				ZeroMemory(SendCard.cbCardData[i], MAX_CARDCOUNT);
				SendCard.bSpecialCard[i] = 0;
				SendCard.cbOriginalCardType[i] = 0;
			}

			str.Format(L"[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]", SendCard.cbCardData[i][0], SendCard.cbCardData[i][1], SendCard.cbCardData[i][2], SendCard.cbCardData[i][3], SendCard.cbCardData[i][4]);
			//LOG_TABLE(L"玩家:", (int)i, L"下注后发送手牌:", str, L"牌型:", (int)m_cbOriginalCardType[i]);
            m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard);
        }
    }

    return true;
}

//摊牌事件
bool CTableFrameSink::OnUserOpenCard(WORD wChairID, BYTE cbCombineCardData[MAX_CARDCOUNT])
{
    //状态效验
    BYTE cbGameStatus = m_pITableFrame->GetGameStatus();

    if(cbGameStatus != GS_TK_PLAYING) { return true; }
    if(m_bOpenCard[wChairID] != false) { return true; }

    //摊牌标志
    m_bOpenCard[wChairID] = true;

	//CString str;
	//str.Format(L"[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]", m_cbHandCardData[wChairID][0], m_cbHandCardData[wChairID][1], m_cbHandCardData[wChairID][2], m_cbHandCardData[wChairID][3], m_cbHandCardData[wChairID][4]);
	//LOG_TABLE(L"玩家:", (int)wChairID, L"原始手牌:", str);

	memcpy(m_cbOriginalCardData[wChairID], m_cbHandCardData[wChairID], sizeof(BYTE)*MAX_CARDCOUNT);

	BYTE cbTempHandCardData[MAX_CARDCOUNT];
	ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
	CopyMemory(cbTempHandCardData, m_cbHandCardData[wChairID], sizeof(cbTempHandCardData));
	m_cbCombineCardType[wChairID] = m_TableDebug.GetCardHelper().GetType(cbTempHandCardData, (BYTE*)m_cbHandCardData[wChairID]);
	m_bSpecialCard[wChairID] = m_cbCombineCardType[wChairID] > CT_CLASSIC_OX_VALUENIUNIU ? true : false;

	
	//str.Format(L"[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]", m_cbHandCardData[wChairID][0], m_cbHandCardData[wChairID][1], m_cbHandCardData[wChairID][2], m_cbHandCardData[wChairID][3], m_cbHandCardData[wChairID][4]);
	//LOG_TABLE(L"玩家:", (int)wChairID, L"整理后手牌:", str, L"牌型:", (int)m_cbCombineCardType[wChairID]);
	/*
    //八人牛牛客户端不组合牌，直接摊牌
    BYTE cbTempHandCardData[MAX_CARDCOUNT];
    ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
    CopyMemory(cbTempHandCardData, m_cbHandCardData[wChairID], sizeof(cbTempHandCardData));




	LOG_DEBUG(L"牌型:", (int)m_cbOriginalCardType[wChairID], L"玩家椅子:", (int)wChairID);
	//
	BYTE cbUserCardType = m_GameLogic.GetCardType(cbTempHandCardData, MAX_CARDCOUNT, m_ctConfig);
	bool bSpecialCard = cbUserCardType > CT_CLASSIC_OX_VALUENIUNIU ? true : false;
    if(!bSpecialCard)
    {
        //获取牛牛牌型
        m_GameLogic.GetOxCard(cbTempHandCardData, MAX_CARDCOUNT);

        CopyMemory(cbCombineCardData, cbTempHandCardData, sizeof(cbTempHandCardData));
    }
	else
    {
        //特殊牌型 赋值初始牌型
        if(m_bSpecialCard[wChairID])
        {
			m_cbCombineCardType[wChairID] = cbUserCardType; m_cbOriginalCardType[wChairID];
        }
        else
        {
            //前面三张王牌张数
            BYTE cbFirstKingCount = m_GameLogic.GetKingCount(cbCombineCardData, 3);
            BYTE cbSecondKingCount = m_GameLogic.GetKingCount(&cbCombineCardData[3], 2);

            if(cbFirstKingCount == 0)
            {
                //前面三张逻辑值
                BYTE cbFirstNNLogicValue = 0;
                for(WORD i = 0; i < 3; i++)
                {
                    cbFirstNNLogicValue += m_GameLogic.GetNNCardLogicValue(cbCombineCardData[i]);
                }

                if(cbFirstNNLogicValue % 10 != 0)
                {
                    m_cbCombineCardType[wChairID] = CT_CLASSIC_OX_VALUE0;
                }
                else
                {
                    if(cbSecondKingCount != 0)
                    {
                        m_cbCombineCardType[wChairID] = CT_CLASSIC_OX_VALUENIUNIU;
                    }
                    else
                    {
                        BYTE cbSecondNNLogicValue = 0;
                        for(WORD i = 3; i < 5; i++)
                        {
                            cbSecondNNLogicValue += m_GameLogic.GetNNCardLogicValue(cbCombineCardData[i]);
                        }

                        m_cbCombineCardType[wChairID] = ((cbSecondNNLogicValue % 10 == 0) ? CT_CLASSIC_OX_VALUENIUNIU : (cbSecondNNLogicValue % 10));
                    }
                }
            }
            else
            {
                if(cbSecondKingCount != 0)
                {
                    m_cbCombineCardType[wChairID] = CT_CLASSIC_OX_VALUENIUNIU;
                }
                else
                {
                    BYTE cbSecondNNLogicValue = 0;
                    for(WORD i = 3; i < 5; i++)
                    {
                        cbSecondNNLogicValue += m_GameLogic.GetNNCardLogicValue(cbCombineCardData[i]);
                    }

                    m_cbCombineCardType[wChairID] = ((cbSecondNNLogicValue % 10 == 0) ? CT_CLASSIC_OX_VALUENIUNIU : (cbSecondNNLogicValue % 10));
                }
            }

            //重置组合过的扑克
            CopyMemory(m_cbHandCardData[wChairID], cbCombineCardData, sizeof(m_cbHandCardData[wChairID]));
        }
    }
	*/

    //摊牌人数
    BYTE bUserCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_bOpenCard[i] == true && m_cbPlayStatus[i] == TRUE) { bUserCount++; }
        else if(m_cbPlayStatus[i] == FALSE) { bUserCount++; }
    }

    //构造变量
    CMD_S_Open_Card OpenCard;
    ZeroMemory(&OpenCard, sizeof(OpenCard));

    //设置变量
    OpenCard.bOpenCard = true;
    OpenCard.wOpenChairID = wChairID;

    //发送数据
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
		if (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE) { continue; }
        m_pITableFrame->SendTableData(i, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_OPEN_CARD, &OpenCard);
    }

    //结束游戏
    if(bUserCount == m_wPlayerCount)
    {
        return OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
    }

    return true;
}

//扑克分析

void CTableFrameSink::AnalyseCard(SENDCARDTYPE_CONFIG stConfig, BYTE cbSysWin)
{
    //AI数
    bool bIsAiBanker = false;
    WORD wAiCount = 0;
    WORD wPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //获取用户
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE)
        {
            if(pIServerUserItem->IsAndroidUser())
            {
                wAiCount++;
                if(i == m_wBankerUser)
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
    BYTE cbUserCardData[GAME_PLAYER][MAX_CARDCOUNT];
    CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

    //获取最大牌型
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            m_GameLogic.GetOxCard(cbUserCardData[i], MAX_CARDCOUNT);
        }
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
            cbCardTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_CARDCOUNT, m_ctConfig);
        }
    }

    //倍数抢庄 结算需要乘以cbMaxCallBankerTimes
    BYTE cbMaxCallBankerTimes = 1;
    if(m_bgtConfig == BGT_ROB_)
    {
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxCallBankerTimes)
            {
                cbMaxCallBankerTimes = m_cbCallBankerTimes[i];
            }
        }
    }

    //机器庄家
    if(bIsAiBanker)
    {
        //对比扑克
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //用户过滤
            if((i == m_wBankerUser) || (m_cbPlayStatus[i] == FALSE)) { continue; }

            //获取用户
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            //机器过滤
            if((pIServerUserItem != NULL) && (pIServerUserItem->IsAndroidUser())) { continue; }

            //对比扑克
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig) == true)
            {
                lAndroidScore -= cbCardTimes[i] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
            else
            {
                lAndroidScore += cbCardTimes[m_wBankerUser] * m_lTableScore[i] * cbMaxCallBankerTimes;
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
            if((i == m_wBankerUser) || (pIServerUserItem == NULL) || !(pIServerUserItem->IsAndroidUser())) { continue; }

            //对比扑克
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig) == true)
            {
                lAndroidScore += cbCardTimes[i] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
            else
            {
                lAndroidScore -= cbCardTimes[m_wBankerUser] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
        }
    }

    LONGLONG lGameEndStorage = lAndroidScore;

	BYTE cbRandVal = rand() % 100;

    //下注发牌
    if(stConfig == ST_BETFIRST_)
    {
        //变量定义
        WORD wMaxUser = INVALID_CHAIR;
        WORD wMinAndroid = INVALID_CHAIR;
        WORD wMaxAndroid = INVALID_CHAIR;

        //查找特殊玩家
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //获取用户
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserItem == NULL) { continue; }

            //真人玩家
            if(pIServerUserItem->IsAndroidUser() == false)
            {
                //初始设置
                if(wMaxUser == INVALID_CHAIR) { wMaxUser = i; }

                //获取较大者
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wMaxUser], MAX_CARDCOUNT, m_ctConfig) == true)
                {
                    wMaxUser = i;
                }
            }

            //机器玩家
            if(pIServerUserItem->IsAndroidUser() == true)
            {
                //初始设置
                if(wMinAndroid == INVALID_CHAIR) { wMinAndroid = i; }
                if(wMaxAndroid == INVALID_CHAIR) { wMaxAndroid = i; }

                //获取较小者
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wMinAndroid], MAX_CARDCOUNT, m_ctConfig) == false)
                {
                    wMinAndroid = i;
                }

                //获取较大者
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wMaxAndroid], MAX_CARDCOUNT, m_ctConfig) == true)
                {
                    wMaxAndroid = i;
                }
            }
        }

		if (cbSysWin)
		{
			//最大玩家（包括真人和AI）
			WORD wWinUser = wMaxUser;
			//最小玩家（包括真人和AI）
			WORD wLostUser = wMaxUser;

			//查找最大玩家
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{

				//用户过滤
				if (m_cbPlayStatus[i] == FALSE) continue;

				//获取较大者
				if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinUser], MAX_CARDCOUNT, m_ctConfig) == true)
				{
					wWinUser = i;
				}
			}

			//查找最小玩家
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{

				//用户过滤
				if (m_cbPlayStatus[i] == FALSE) continue;

				//获取较大者
				if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wLostUser], MAX_CARDCOUNT, m_ctConfig) == false)
				{
					wLostUser = i;
				}
			}

			//AI当庄
			if (bIsAiBanker)
			{
				//交换数据
				BYTE cbTempData[MAX_CARDCOUNT];
				CopyMemory(cbTempData, m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT);
				CopyMemory(m_cbHandCardData[m_wBankerUser], m_cbHandCardData[wWinUser], MAX_CARDCOUNT);
				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, MAX_CARDCOUNT);
			}
			else
			{
				//最小的牌给真人玩家
				BYTE cbTempData[MAX_CARDCOUNT];
				CopyMemory(cbTempData, m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT);
				CopyMemory(m_cbHandCardData[m_wBankerUser], m_cbHandCardData[wLostUser], MAX_CARDCOUNT);
				CopyMemory(m_cbHandCardData[wLostUser], cbTempData, MAX_CARDCOUNT);
			}
		}
		else
		{

				if (m_GameLogic.CompareCard(cbUserCardData[wMaxAndroid], cbUserCardData[wMaxUser], MAX_CARDCOUNT, m_ctConfig) == true)
				{
					//交换数据
					BYTE cbTempData[MAX_CARDCOUNT];
					CopyMemory(cbTempData, m_cbHandCardData[wMaxUser], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[wMaxUser], m_cbHandCardData[wMaxAndroid], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[wMaxAndroid], cbTempData, MAX_CARDCOUNT);


				}
		}	
	}	
    else if(stConfig == ST_SENDFOUR_)
    {
        //扑克链表
        CList<BYTE, BYTE &> cardlist;
        cardlist.RemoveAll();

        //含大小王
        if(m_gtConfig == GT_HAVEKING_)
        {
            for(WORD i = 0; i < 54; i++)
            {
                cardlist.AddTail(m_GameLogic.m_cbCardListDataHaveKing[i]);
            }
        }
        else if(m_gtConfig == GT_NOKING_)
        {
            for(WORD i = 0; i < 52; i++)
            {
                cardlist.AddTail(m_GameLogic.m_cbCardListDataNoKing[i]);
            }
        }

        //删除扑克 （删除前面4张，构造后面一张）
        for(WORD i = 0; i < GAME_PLAYER; i++)
        {
            for(WORD j = 0; j < MAX_CARDCOUNT - 1; j++)
            {
                if(m_cbHandCardData[i][j] != 0)
                {
                    POSITION ptListHead = cardlist.GetHeadPosition();
                    POSITION ptTemp;
                    BYTE cbCardData = INVALID_BYTE;

                    //遍历链表
                    while(ptListHead)
                    {
                        ptTemp = ptListHead;
                        if(cardlist.GetNext(ptListHead) == m_cbHandCardData[i][j])
                        {
                            cardlist.RemoveAt(ptTemp);
                            break;
                        }
                    }
                }
            }
        }

		//库存判断

		if (cbSysWin)
		{
			//这局机器人赢

			//机器人坐庄, 剩下真人中，抽取可以已知最大的牌型A ， 机器人庄家构造比A大
			if (m_wBankerUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
			{
				BYTE cbMaxCardTypeSendFour = m_GameLogic.GetMaxCardTypeSendFourRealPlayer(cardlist, m_cbHandCardData, m_ctConfig, m_pITableFrame, m_cbPlayStatus, m_cbDynamicJoin, m_wPlayerCount);
				cbMaxCardTypeSendFour = (cbMaxCardTypeSendFour >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSendFour);

				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[m_wBankerUser], cbMaxCardTypeSendFour + 1, m_gtConfig);
				if (cbKeyCardData != INVALID_BYTE)
				{
					m_cbHandCardData[m_wBankerUser][4] = cbKeyCardData;
				}
			}
			//真人坐庄 ,为了保证机器人赢，所有机器人构造比真人庄家大的牌 (机器人5张牌都可以换 )
			else if (m_wBankerUser != INVALID_CHAIR && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
			{
				BYTE cbMaxCardTypeSingle = m_GameLogic.GetMaxCardTypeSingle(cardlist, m_cbHandCardData[m_wBankerUser], m_ctConfig);
				cbMaxCardTypeSingle = (cbMaxCardTypeSingle >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSingle);

				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					//获取用户
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
					{
						//AI
						if (pIServerUserItem->IsAndroidUser() == true)
						{
							BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], cbMaxCardTypeSingle + 1, m_gtConfig);
							if (cbKeyCardData != INVALID_BYTE)
							{
								m_cbHandCardData[i][4] = cbKeyCardData;
							}
						}
					}
				}
			}

			//牛五为基准点，AI从牛六往上开始构造， 普通玩家从牛五往下开始构造
			//for (WORD i = 0; i<m_wPlayerCount; i++)
			//{
			//	//获取用户
			//	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			//	if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
			//	{
			//		//AI
			//		if (pIServerUserItem->IsAndroidUser() == true)
			//		{
			//			//AI从牛六往上开始构造
			//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE6; wCardTypeIndex <= CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex++)
			//			{
			//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
			//				if (cbKeyCardData == INVALID_BYTE)
			//				{
			//					continue;
			//				}
			//				else
			//				{
			//					m_cbHandCardData[i][4] = cbKeyCardData;
			//					break;
			//				}
			//			}
			//		}
			//		else if (pIServerUserItem->IsAndroidUser() == false)
			//		{
			//			//普通玩家从牛五往下开始构造
			//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE5; wCardTypeIndex >= CT_CLASSIC_OX_VALUE0; wCardTypeIndex--)
			//			{
			//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
			//				if (cbKeyCardData == INVALID_BYTE)
			//				{
			//					continue;
			//				}
			//				else
			//				{
			//					m_cbHandCardData[i][4] = cbKeyCardData;
			//					break;
			//				}
			//			}
			//		}
			//	}
			//}
		}
		else
		{

				//这局真人赢

				//机器人坐庄, 为了保证真人赢，所有真人构造比机器人庄家大的牌 
				if (m_wBankerUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
				{
					BYTE cbMaxCardTypeSingle = m_GameLogic.GetMaxCardTypeSingle(cardlist, m_cbHandCardData[m_wBankerUser], m_ctConfig);
					cbMaxCardTypeSingle = (cbMaxCardTypeSingle >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSingle);

					for (WORD i = 0; i<m_wPlayerCount; i++)
					{
						//获取用户
						IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
						{
							//真人
							if (pIServerUserItem->IsAndroidUser() == false)
							{
								BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], cbMaxCardTypeSingle + 1, m_gtConfig);
								if (cbKeyCardData != INVALID_BYTE)
								{
									m_cbHandCardData[i][4] = cbKeyCardData;
								}
							}
						}
					}					
				}
				//真人坐庄 ,剩下机器人中，抽取可以已知最大的牌型A ， 真人庄家构造比A大
				else if (m_wBankerUser != INVALID_CHAIR && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
				{
					BYTE cbMaxCardTypeSendFour = m_GameLogic.GetMaxCardTypeSendFourRealPlayer(cardlist, m_cbHandCardData, m_ctConfig, m_pITableFrame, m_cbPlayStatus, m_cbDynamicJoin, m_wPlayerCount);
					cbMaxCardTypeSendFour = (cbMaxCardTypeSendFour >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSendFour);

					BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[m_wBankerUser], cbMaxCardTypeSendFour + 1, m_gtConfig);
					if (cbKeyCardData != INVALID_BYTE)
					{
						m_cbHandCardData[m_wBankerUser][4] = cbKeyCardData;
					}
				}

				//牛五为基准点，AI从牛五往下开始构造 普通玩家从牛六往上开始构造
				//for (WORD i = 0; i<m_wPlayerCount; i++)
				//{
				//	//获取用户
				//	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				//	if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
				//	{
				//		//AI
				//		if (pIServerUserItem->IsAndroidUser() == true)
				//		{
				//			//AI从牛五往下开始构造
				//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE5; wCardTypeIndex >= CT_CLASSIC_OX_VALUE0; wCardTypeIndex--)
				//			{
				//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
				//				if (cbKeyCardData == INVALID_BYTE)
				//				{
				//					continue;
				//				}
				//				else
				//				{
				//					m_cbHandCardData[i][4] = cbKeyCardData;
				//					break;
				//				}
				//			}
				//		}
				//		else if (pIServerUserItem->IsAndroidUser() == false)
				//		{
				//			//普通玩家从牛六往上开始构造
				//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE6; wCardTypeIndex <= CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex++)
				//			{
				//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
				//				if (cbKeyCardData == INVALID_BYTE)
				//				{
				//					continue;
				//				}
				//				else
				//				{
				//					m_cbHandCardData[i][4] = cbKeyCardData;
				//					break;
				//				}
				//			}
				//		}
				//	}
				//}
			}
			
								}
			
    return;
}

//判断库存
bool CTableFrameSink::JudgeStock()
{
    //AI当庄标识
    bool bIsAiBanker = false;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //获取用户
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser() && m_cbPlayStatus[i] == TRUE && i == m_wBankerUser)
        {
            bIsAiBanker = true;
        }
    }

    //扑克变量
    BYTE cbUserCardData[GAME_PLAYER][MAX_CARDCOUNT];
    CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

    //获取最大牌型
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            m_GameLogic.GetOxCard(cbUserCardData[i], MAX_CARDCOUNT);
        }
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
            cbCardTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_CARDCOUNT, m_ctConfig);
        }
    }

    //倍数抢庄 结算需要乘以cbMaxCallBankerTimes
    BYTE cbMaxCallBankerTimes = 1;
    if(m_bgtConfig == BGT_ROB_)
    {
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxCallBankerTimes)
            {
                cbMaxCallBankerTimes = m_cbCallBankerTimes[i];
            }
        }
    }

    //机器庄家
    if(bIsAiBanker)
    {
        //对比扑克
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //用户过滤
            if((i == m_wBankerUser) || (m_cbPlayStatus[i] == FALSE)) { continue; }

            //获取用户
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            //机器过滤
            if((pIServerUserItem != NULL) && (pIServerUserItem->IsAndroidUser())) { continue; }

            //对比扑克
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig) == true)
            {
                lAndroidScore -= cbCardTimes[i] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
            else
            {
                lAndroidScore += cbCardTimes[m_wBankerUser] * m_lTableScore[i] * cbMaxCallBankerTimes;
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
            if((i == m_wBankerUser) || (pIServerUserItem == NULL) || !(pIServerUserItem->IsAndroidUser())) { continue; }

            //对比扑克
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig) == true)
            {
                lAndroidScore += cbCardTimes[i] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
            else
            {
                lAndroidScore -= cbCardTimes[m_wBankerUser] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
        }
    }




	return true;

}

//查询是否扣服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
    for(BYTE i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pUserItem == NULL) { continue; }

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
        if(pItem != NULL && m_cbPlayStatus[i] == TRUE)
        {
            beforeScore[i] = pItem->GetUserScore();
            m_pITableFrame->WriteUserScore(i, ScoreInfo[i]);
        }
    }

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
                strInfo.Format(TEXT("USERID[%d] 出现负分, 写分前分数：%I64d, 写入积分 %I64d，税收 %I64d, 写分后分数：%I64d"), pItem->GetUserID(), beforeScore[i], ScoreInfoArray[i].lScore, ScoreInfoArray[i].lRevenue, afterScore[i]);
				CString strFileName = TEXT("调试日志");

                tagLogUserInfo LogUserInfo;
                ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
                CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
                CopyMemory(LogUserInfo.szLogContent, strInfo, sizeof(LogUserInfo.szLogContent));
                //m_pITableFrame->SendGameLogData(LogUserInfo);
            }

        }
    }
    return true;
}

//最大下分
SCORE CTableFrameSink::GetUserMaxTurnScore(WORD wChairID)
{
    SCORE lMaxTurnScore = 0L;
    if(wChairID == m_wBankerUser) { return 0; }
    //庄家积分
    IServerUserItem *pIBankerItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
    LONGLONG lBankerScore = 0L;
    if(pIBankerItem != NULL)
    {
        lBankerScore = pIBankerItem->GetUserScore();
    }

    //玩家人数
    WORD wUserCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
        if(m_cbPlayStatus[i] == TRUE) { wUserCount++; }

    //获取用户
    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

    BYTE cbMaxCallBankerTimes = 1;
    if(m_bgtConfig == BGT_ROB_)
    {
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxCallBankerTimes)
            {
                cbMaxCallBankerTimes = m_cbCallBankerTimes[i];
            }
        }
    }

    //推注功能只在积分房卡中，金币房卡和金币场不支持
    //前端积分房卡创建界面可以选择:无推注和翻倍推注
    //这一局可以推注的 条件 1. 这局是闲家 2. 上一局赢钱且闲家 3.上一把没有推注
    //某闲家可推注额度: 上局赢的总额的翻倍为本局推注的额度（大厅服务分配积分无限大，不用考虑不够赔的情况）
    //不可以连续推注
    //推注筹码是5的倍数且限制推注最高100

    //积分房卡
	if (IsRoomCardType() && m_tyConfig == BT_TUI_DOUBLE_ && m_bgtConfig != BGT_TONGBI_)
    {
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            if(m_bLastTurnBeBanker[i] == false && m_lLastTurnWinScore[i] > 0 && !m_bLastTurnBetBtEx[i])
            {
                m_lPlayerBetBtEx[i] = m_lLastTurnWinScore[i] * 2 / m_pITableFrame->GetCellScore();

                //推注是5的倍数
                int nDiv = m_lPlayerBetBtEx[i] / 5;
                if(nDiv > 0)
                {
                    m_lPlayerBetBtEx[i] = nDiv * 5;
                }
                else
                {
                    m_lPlayerBetBtEx[i] = 0;
                }

                m_lPlayerBetBtEx[i] = (m_lPlayerBetBtEx[i] > 100 ? 100 : m_lPlayerBetBtEx[i]);
            }
        }
    }
    else
    {
        //重置推注变量
        ZeroMemory(m_bLastTurnBeBanker, sizeof(m_bLastTurnBeBanker));
        ZeroMemory(m_lLastTurnWinScore, sizeof(m_lLastTurnWinScore));
        ZeroMemory(m_bLastTurnBetBtEx, sizeof(m_bLastTurnBetBtEx));
        ZeroMemory(m_lPlayerBetBtEx, sizeof(m_lPlayerBetBtEx));
    }

    //计算自由下注模式
    //房卡和普通金币场最大下注，前端判断某个下注筹码是否禁用
    if(pIServerUserItem != NULL && m_btConfig == BT_FREE_)
    {
        //获取积分
        LONGLONG lScore = pIServerUserItem->GetUserScore();
        lMaxTurnScore = lScore /*/ m_lMaxCardTimes / cbMaxCallBankerTimes / wUserCount*/;

        if(m_lPlayerBetBtEx[wChairID] != 0)
        {
            lMaxTurnScore = max(lMaxTurnScore, m_lPlayerBetBtEx[wChairID]);
        }

		//最小可以下第一个筹码
		lMaxTurnScore = max(lMaxTurnScore, m_lFreeConfig[0] * m_pITableFrame->GetCellScore());

        return lMaxTurnScore;
    }

    return lMaxTurnScore;
}

//查询限额
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem *pIServerUserItem)
{
    return 0L;
}

//是否衰减(一桌同时存在真人和机器人才衰减)
bool CTableFrameSink::NeedDeductStorage()
{
	BYTE cbRealPlayerCount = 0;
	BYTE cbAICount = 0;

    for(int i = 0; i < m_wPlayerCount; ++i)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL) 
		{ 
			continue; 
		}

        if(pIServerUserItem->IsAndroidUser())
        {
			cbAICount++;
        }
		else
		{
			cbRealPlayerCount++;
		}
    }

	return (cbRealPlayerCount > 0 && cbAICount > 0);
}

//读取配置
void CTableFrameSink::ReadConfigInformation()
{
    //获取自定义配置
    tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
    ASSERT(pCustomRule);


    m_ctConfig = pCustomRule->ctConfig;
    m_stConfig = pCustomRule->stConfig;
    m_gtConfig = pCustomRule->gtConfig;
    m_bgtConfig = pCustomRule->bgtConfig;
    m_btConfig = pCustomRule->btConfig;

	m_nTimeStart = pCustomRule->nTimeStart;
	m_nTimeCallBanker = pCustomRule->nTimeCallBanker;
	m_nTimeAddScore = pCustomRule->nTimeAddScore;
	m_nTimeOpenCard = pCustomRule->nTimeOpenCard;

    CopyMemory(m_lFreeConfig, pCustomRule->lFreeConfig, sizeof(m_lFreeConfig));
    CopyMemory(m_lPercentConfig, pCustomRule->lPercentConfig, sizeof(m_lPercentConfig));

    if(m_ctConfig == CT_ADDTIMES_)
    {
        //五小牛牌型激活标注
        m_cbEnableCardType[7] = pCustomRule->cbCardTypeTimesAddTimes[18] == 0 ? FALSE : TRUE;

        m_cbClassicTypeConfig = INVALID_BYTE;
    }
    else if(m_ctConfig == CT_CLASSIC_)
    {
        //五小牛牌型激活标注
        m_cbEnableCardType[7] = pCustomRule->cbCardTypeTimesClassic[18] == 0 ? FALSE : TRUE;

        if(pCustomRule->cbCardTypeTimesClassic[7] == 1 && pCustomRule->cbCardTypeTimesClassic[8] == 2
                && pCustomRule->cbCardTypeTimesClassic[9] == 2 && pCustomRule->cbCardTypeTimesClassic[10] == 3)
        {
            m_cbClassicTypeConfig = 1;
        }
        else if(pCustomRule->cbCardTypeTimesClassic[7] == 2 && pCustomRule->cbCardTypeTimesClassic[8] == 2
                && pCustomRule->cbCardTypeTimesClassic[9] == 3 && pCustomRule->cbCardTypeTimesClassic[10] == 4)
        {
            m_cbClassicTypeConfig = 0;
        }
    }

    m_cbTrusteeDelayTime = pCustomRule->cbTrusteeDelayTime;
	
	UpdateConfigPersonRoom();

	if (m_pITableFrame->GetTableID() == 0)
	{
		g_dwSysCtrlIncreaseIndex++;
		g_lCurAnChouRate = pCustomRule->lConfigAnChouRate;

		//插入系统控制列表
		SYSCTRL sysctrl;
		ZeroMemory(&sysctrl, sizeof(sysctrl));

		sysctrl.dwCtrlIndex = g_dwSysCtrlIncreaseIndex;
		sysctrl.lSysCtrlConfigSysStorage = pCustomRule->lSysCtrlSysStorage;;
		sysctrl.lSysCtrlConfigPlayerStorage = pCustomRule->lSysCtrlPlayerStorage;
		sysctrl.lSysCtrlCurSysStorage = pCustomRule->lSysCtrlSysStorage;;
		sysctrl.lSysCtrlCurPlayerStorage = pCustomRule->lSysCtrlPlayerStorage;
		sysctrl.lSysCtrlParameterK = pCustomRule->lSysCtrlParameterK;
		sysctrl.lSysCtrlSysStorResetRate = pCustomRule->lSysCtrlSysStorResetRate;
		sysctrl.lSysCtrlResetCount = 0;
		sysctrl.lSysCtrlSysWin = 0;
		sysctrl.tmResetTime = CTime::GetCurrentTime();
		sysctrl.sysCtrlStatus = PROGRESSINGEX;

		//插入列表
		m_listSysCtrl.AddTail(sysctrl);
	}

    return;
}

//更新房间用户信息
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction)
{
    ROOMUSERINFO roomUserInfo;
    ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	DWORD dwUserID = pIServerUserItem->GetUserID();
	BOOL cbLookup = g_MapRoomUserInfo.Lookup(dwUserID, roomUserInfo);

	switch (userAction)
    {
	case USER_OFFLINE:
	case USER_RECONNECT:
    {
			break;
    }
	case USER_NULL:
    {
			if (cbLookup == TRUE)
        	{
				roomUserInfo.wBankerChairID = m_wBankerUser;

				DWORD dwUserID = pIServerUserItem->GetUserID();
				g_MapRoomUserInfo.SetAt(dwUserID, roomUserInfo);

				POSITION pos = FindTableCtrl(roomUserInfo);
				if (pos)
    			{
					TABLECTRL & tablectrl = m_listTableCtrl.GetAt(pos);
					CopyMemory(&(tablectrl.roomUserInfo), &roomUserInfo, sizeof(roomUserInfo));
    			}
			}
			break;
	}
	case USER_STANDUP:
	{
			if (cbLookup == TRUE)
			{
				roomUserInfo.wChairID = INVALID_CHAIR;
				roomUserInfo.wTableID = INVALID_TABLE;
				roomUserInfo.wBankerChairID = INVALID_CHAIR;
				roomUserInfo.wCurTablePlayerCount = 0;

				DWORD dwUserID = pIServerUserItem->GetUserID();
				g_MapRoomUserInfo.SetAt(dwUserID, roomUserInfo);

				POSITION pos = FindTableCtrl(roomUserInfo);
				if (pos)
    			{
					TABLECTRL & tablectrl = m_listTableCtrl.GetAt(pos);
					CopyMemory(&(tablectrl.roomUserInfo), &roomUserInfo, sizeof(roomUserInfo));
				}

				//更新同桌其他玩家椅子数目
				UpdateSameTableChairCount(pIServerUserItem, userAction, m_pITableFrame->GetTableID());
			}

			break;
        }
	case USER_SITDOWN:
	{
			if (cbLookup == TRUE)
    		{
				roomUserInfo.wChairID = pIServerUserItem->GetChairID();
				roomUserInfo.wTableID = pIServerUserItem->GetTableID();
				roomUserInfo.wCurTablePlayerCount = 0;

				for (WORD i = 0; i < m_wPlayerCount; i++)
        		{
					IServerUserItem *pKeyIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (!pKeyIServerUserItem)
					{
						continue;
        			}


					roomUserInfo.wCurTablePlayerCount++;
   				}

				DWORD dwUserID = pIServerUserItem->GetUserID();
				g_MapRoomUserInfo.SetAt(dwUserID, roomUserInfo);

				POSITION pos = FindTableCtrl(roomUserInfo);
				if (pos)
    			{
					TABLECTRL & tablectrl = m_listTableCtrl.GetAt(pos);
					CopyMemory(&(tablectrl.roomUserInfo), &roomUserInfo, sizeof(roomUserInfo));
    			}
			}
    		else
   			{
				ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

				roomUserInfo.wChairID = pIServerUserItem->GetChairID();
				roomUserInfo.wTableID = pIServerUserItem->GetTableID();
				roomUserInfo.tablectrl_type = TABLECTRL_INVALID;

				roomUserInfo.wCurTablePlayerCount = 1;
			    for(WORD i = 0; i < m_wPlayerCount; i++)
			    {

					IServerUserItem *pKeyIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (!pKeyIServerUserItem || pIServerUserItem == pKeyIServerUserItem)
			        {
			            continue;
			        }

					roomUserInfo.wCurTablePlayerCount++;
                }

				roomUserInfo.wBankerChairID = m_wBankerUser;
				roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
				roomUserInfo.dwUserID = pIServerUserItem->GetUserID();
				CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
				roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
				roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

				roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

				DWORD dwUserID = pIServerUserItem->GetUserID();
				g_MapRoomUserInfo.SetAt(dwUserID, roomUserInfo);

				POSITION pos = FindTableCtrl(roomUserInfo);
				if (pos)
				{
					TABLECTRL & tablectrl = m_listTableCtrl.GetAt(pos);
					CopyMemory(&(tablectrl.roomUserInfo), &roomUserInfo, sizeof(roomUserInfo));
        }
    }

			//更新同桌其他玩家椅子数目
			UpdateSameTableChairCount(pIServerUserItem, userAction, m_pITableFrame->GetTableID());

        break;
    }
	default:
        break;
    }
}



//判断房卡房间
bool CTableFrameSink::IsRoomCardType()
{
    //金币场和金币房卡可以托管，积分房卡不托管
    return (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 0);
}

//初始默认庄家
void CTableFrameSink::InitialBanker()
{
    //房主强制为庄，若房主不参与游戏，则第一个进去此游戏的玩家强制为庄
    //获取房主
    WORD wRoomOwenrChairID = INVALID_CHAIR;
    DWORD dwRoomOwenrUserID = INVALID_DWORD;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //获取用户
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(!pIServerUserItem)
        {
            continue;
        }

        m_cbCallBankerStatus[i] = TRUE;

        if(pIServerUserItem->GetUserID() == m_pITableFrame->GetTableOwner() && IsRoomCardType())
        {
            dwRoomOwenrUserID = pIServerUserItem->GetUserID();
            wRoomOwenrChairID = pIServerUserItem->GetChairID();
            //break;
        }
    }

    //房主参与游戏
    if(dwRoomOwenrUserID != INVALID_DWORD && wRoomOwenrChairID != INVALID_CHAIR)
    {
        m_wBankerUser = wRoomOwenrChairID;
    }
    //房主不参与游戏和非房卡房间
    else
    {
        ASSERT(m_listEnterUser.IsEmpty() == false);
        m_wBankerUser = m_listEnterUser.GetHead();
    }

    ASSERT(m_wBankerUser != INVALID_CHAIR);

    m_bBuckleServiceCharge[m_wBankerUser] = true;

    bool bTrusteeUser = false;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
        {
            bTrusteeUser = true;
        }
    }

    //非房卡场设置定时器
    if(!IsRoomCardType()/* || bTrusteeUser*/)
    {
        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, (m_nTimeAddScore + 2) * 1000, 1, 0);
    }

    //设置离线代打定时器
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
        {
            m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
        }
    }

    //设置状态
    m_pITableFrame->SetGameStatus(GS_TK_SCORE);
	EnableTimeElapse(true, TIME_ADDSCORE);


    //获取最大下注
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] != TRUE || i == m_wBankerUser)
        {
            continue;
        }

        //下注变量
        m_lTurnMaxScore[i] = GetUserMaxTurnScore(i);
    }

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
        {
            continue;
        }

        if(m_bLastTurnBetBtEx[i] == true)
        {
            m_bLastTurnBetBtEx[i] = false;
        }
    }

    m_lPlayerBetBtEx[m_wBankerUser] = 0;

    //设置变量
    CMD_S_GameStart GameStart;
    ZeroMemory(&GameStart, sizeof(GameStart));
    GameStart.wBankerUser = m_wBankerUser;
    CopyMemory(GameStart.cbPlayerStatus, m_cbPlayStatus, sizeof(m_cbPlayStatus));

    //发四等五
    if(m_stConfig == ST_SENDFOUR_)
    {
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            //派发扑克(开始只发四张牌)
            CopyMemory(GameStart.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
        }
    }

    GameStart.stConfig = m_stConfig;
    GameStart.bgtConfig = m_bgtConfig;
    GameStart.btConfig = m_btConfig;
    GameStart.gtConfig = m_gtConfig;

    CopyMemory(GameStart.lFreeConfig, m_lFreeConfig, sizeof(GameStart.lFreeConfig));
    CopyMemory(GameStart.lPercentConfig, m_lPercentConfig, sizeof(GameStart.lPercentConfig));
    CopyMemory(GameStart.lPlayerBetBtEx, m_lPlayerBetBtEx, sizeof(GameStart.lPlayerBetBtEx));

    bool bFirstRecord = true;

    WORD wRealPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }

        if(!pServerUserItem)
        {
            continue;
        }

        wRealPlayerCount++;
    }

    BYTE *pGameRule = m_pITableFrame->GetGameRule();

    //最大下注
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }
        GameStart.lTurnMaxScore = m_lTurnMaxScore[i];
        m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

		//更新房间用户信息
		UpdateRoomUserInfo(pServerUserItem, USER_NULL);

        if(m_pGameVideo)
        {
            Video_GameStart video;
            ZeroMemory(&video, sizeof(video));
            video.lCellScore = m_pITableFrame->GetCellScore();
            video.wPlayerCount = wRealPlayerCount;
            video.wGamePlayerCountRule = pGameRule[1];
            video.wBankerUser = GameStart.wBankerUser;
            CopyMemory(video.cbPlayerStatus, GameStart.cbPlayerStatus, sizeof(video.cbPlayerStatus));
            video.lTurnMaxScore = GameStart.lTurnMaxScore;
            CopyMemory(video.cbCardData, GameStart.cbCardData, sizeof(video.cbCardData));
            video.ctConfig = m_ctConfig;
            video.stConfig = GameStart.stConfig;
            video.bgtConfig = GameStart.bgtConfig;
            video.btConfig = GameStart.btConfig;
            video.gtConfig = GameStart.gtConfig;

            CopyMemory(video.lFreeConfig, GameStart.lFreeConfig, sizeof(video.lFreeConfig));
            CopyMemory(video.lPercentConfig, GameStart.lPercentConfig, sizeof(video.lPercentConfig));
            CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
            video.wChairID = i;
            video.lScore = pServerUserItem->GetUserScore();
			//for (int i = 0; i < MAX_CARD_TYPE; ++i)
			//	video.wCardTypeTimes[i] = m_GameLogic.m_cbCardTypeTimes[i];
            m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video, bFirstRecord);

            if(bFirstRecord == true)
            {
                bFirstRecord = false;
            }
        }
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
}

//搜索特定扑克玩家
WORD CTableFrameSink::SearchKeyCardChairID(BYTE cbKeyCardData[MAX_CARDCOUNT])
{
    WORD wKeyChairID = INVALID_CHAIR;

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //获取用户
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)
        {
            continue;
        }

        if(cbKeyCardData[0] == m_cbHandCardData[i][0] && cbKeyCardData[1] == m_cbHandCardData[i][1]
                && cbKeyCardData[2] == m_cbHandCardData[i][2] && cbKeyCardData[3] == m_cbHandCardData[i][3]
                && cbKeyCardData[4] == m_cbHandCardData[i][4])
        {
            wKeyChairID = i;
            break;
        }
    }

    return wKeyChairID;
}

//流逝时间
void CTableFrameSink::EnableTimeElapse(bool bEnable, UINT nAssignTime)
{
	if (bEnable)
	{
		m_pITableFrame->KillGameTimer(IDI_TIME_ELAPSE);
		m_cbTimeRemain = nAssignTime / 1000;
		m_pITableFrame->SetGameTimer(IDI_TIME_ELAPSE, 1000, -1, 0);
	}
	else
	{
		m_pITableFrame->KillGameTimer(IDI_TIME_ELAPSE);
		m_cbTimeRemain = nAssignTime / 1000;
	}
}

//游戏状态判断
bool CTableFrameSink::JudgePlayingStatus(IServerUserItem *pIServerUserItem)
{
	return pIServerUserItem != NULL && pIServerUserItem->GetUserStatus() <= US_READY;
}

//测试写信息
void CTableFrameSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
	//设置语言牌桌
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("%s"), pszFileName);
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		myFile.SeekToEnd();
		myFile.WriteString(pszString);
		myFile.Flush();
		myFile.Close();
	}

	//还原牌桌设定
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}
WORD CTableFrameSink::GetUserChair(WORD wGameID)
{
	IServerUserItem* pDebugUser = nullptr;
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		pDebugUser = m_pITableFrame->EnumLookonUserItem(i);
		if (pDebugUser != nullptr)
		{
			if (pDebugUser->GetGameID() == wGameID)
				return i;
		}
	}

	return INVALID_CHAIR;
};
//
CUserDebug*	CTableFrameSink::GetDebugUserByGameID(WORD wGameID)
{
	IServerUserItem* pDebugUser = nullptr;
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		pDebugUser = m_pITableFrame->EnumLookonUserItem(i);
		if (pDebugUser != nullptr)
		{
			if (pDebugUser->GetGameID() == wGameID)
				break;

			pDebugUser = nullptr;
		}
	}
	if (pDebugUser == nullptr)
		return nullptr;

	return m_TableDebug.GetDebugUser(pDebugUser->GetChairID());
};
void CTableFrameSink::UpdateUserCards(WORD wChairID,BYTE* pCards, BYTE cbCount)
{
	if (wChairID < 0 || wChairID >= GAME_PLAYER)
	{
		ASSERT(false);
		return;
	}

	memcpy(m_cbHandCardData[wChairID], pCards, cbCount*sizeof(BYTE));
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard, sizeof(SendCard));;
	ZeroMemory(SendCard.cbCardData, sizeof(SendCard.cbCardData));
	//memcpy(SendCard.cbCardData,)
	//

};
//配置调试
bool CTableFrameSink::OnConfigDebug(IServerUserItem* pIServerUserItem, void* pData, WORD wDataSize)
{
	//权限判断
	//if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
	if (!IS_DEBUG_USER(pIServerUserItem->GetUserRight()))
	{
		LOG_TABLE(L"无调试权限,玩家:",pIServerUserItem->GetGameID());
		SendGameMessage(pIServerUserItem, L"无调试权限");
		return true;
	}
	if (wDataSize != sizeof(CMD_C_ConfigDebug))
		return true;

	CMD_C_ConfigDebug* pRequest = (CMD_C_ConfigDebug*)pData;
	WORD wDebugUserChair = pRequest->stConfig.wDebugUserChair;

	LOG_TABLE(L"执行调试，玩家:", pIServerUserItem->GetGameID(), L"被调试玩家椅子：", (int)wDebugUserChair);

	if (wDebugUserChair < 0 || wDebugUserChair > GAME_PLAYER)
	{
		SendGameMessage(pIServerUserItem, L"无效调试玩家");
		return true;
	}
	
	CUserDebug* pUser = m_TableDebug.GetDebugUser(wDebugUserChair);
	if (pUser == nullptr)
	{
		LOG_TABLE(L"错误,找不到玩家调试信息!");
		ASSERT(false);
		return true;
	}

	emUserDebugType emDebugType = EM_DEBUG_NULL;
	//执行调试
	if (pRequest->stConfig.cbType == 1)
	{
		if (pRequest->stConfig.cbGameWin == 1) emDebugType = EM_DEBUG_LOSE;
		else if (pRequest->stConfig.cbGameWin == 2) emDebugType = EM_DEBUG_WIN;
		if (pRequest->stConfig.cbGameCount == 0)
		{
			LOG_TABLE(L"无效调试局数,玩家:",pIServerUserItem->GetGameID());
			SendGameMessage(pIServerUserItem, L"无效调试局数");
			return true;
		}

		m_TableDebug.AddConfig(pRequest->stConfig);
		SendUserConfigDebug(pIServerUserItem, wDebugUserChair);
		return true;
	}
	if (pRequest->stConfig.cbType != 2)
	{
		LOG_TABLE(L"无效调试类型,玩家:", pIServerUserItem->GetGameID());
		SendGameMessage(pIServerUserItem, L"无效调试类型");
		return true;
	}

	//
	stServerConfig* pOldConfig = m_TableDebug.GetUserConfig(wDebugUserChair);
	if (pOldConfig && pOldConfig->stConfig.cbType == 2)
	{
		m_GameLogic.AddCards(pOldConfig->stConfig.cbGameCards, pOldConfig->nCurConfigCardCount);
	}
	m_TableDebug.AddConfig(pRequest->stConfig);
	stServerConfig* pNewConfig = m_TableDebug.GetUserConfig(wDebugUserChair);
	ASSERT(pNewConfig != nullptr);
	if (pNewConfig == nullptr)
	{
		LOG_TABLE(L"配置失败,玩家:", pIServerUserItem->GetGameID());
		SendGameMessage(pIServerUserItem, L"配置失败");
		return true;
	}
	//空闲状态配置下局手牌，需要更新牌池，防止配置冲突
	if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
		m_GameLogic.RemoveCards(pRequest->stConfig.cbGameCards, MAX_CARDCOUNT);
	CString str;
	str.Format(L"[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]", pNewConfig->stConfig.cbGameCards[0], pNewConfig->stConfig.cbGameCards[1], pNewConfig->stConfig.cbGameCards[2], pNewConfig->stConfig.cbGameCards[3], pNewConfig->stConfig.cbGameCards[4]);
	LOG_TABLE(L"玩家:", (int)wDebugUserChair, L"配置手牌,手牌:", str, L"亮牌标识:", (int)m_bOpenCard[wDebugUserChair], L"手牌数量:", (int)pUser->GetCount());
	//发四等五,牌已发完且不是自己不即时调试
	if (m_stConfig == ST_SENDFOUR_ && pUser->GetCount() == MAX_CARDCOUNT && pIServerUserItem->GetChairID() != wDebugUserChair)
	{
		SendUserConfigDebug(pIServerUserItem, wDebugUserChair);
		return true;
	}
	//如果游戏已开始并处于未亮牌状态, 即时调试
	if (!m_bOpenCard[wDebugUserChair] && pUser->GetCount() > 0)
	{
		//检查牌数据是否有效
		BYTE cbTmpCheckCard[MAX_CARDCOUNT] = {0};
		memcpy(cbTmpCheckCard, pNewConfig->stConfig.cbGameCards, sizeof(BYTE)*MAX_CARDCOUNT);
		for (int i = 0; i < MAX_CARDCOUNT; ++i)
		{
			BYTE cbCheckCard = cbTmpCheckCard[i];
			if (cbCheckCard == 0)
				continue;
			bool bFind = false;
			for (int nCheckHandIdx = 0; nCheckHandIdx < pUser->GetCount(); nCheckHandIdx++)
			{
				if (cbCheckCard == pUser->GetCards()[nCheckHandIdx])
				{
					bFind = true;
					cbTmpCheckCard[i] = 0;
					break;
				}
			}
			if (bFind)
				continue;

			BYTE* pPool = m_GameLogic.GetPoolData();
			for (int nCheckPoolIdx = 0; nCheckPoolIdx < m_GameLogic.GetPoolLeftCount(); nCheckPoolIdx++)
			{
				if (cbCheckCard == pPool[nCheckPoolIdx])
				{
					bFind = true;
					break;
				}
			}
			if (bFind)
				continue;

			if (!bFind)
			{
				LOG_TABLE(L"配牌冲突，该牌型已失效,玩家:", pIServerUserItem->GetGameID());
				SendGameMessage(pIServerUserItem, L"配牌冲突，该牌型已失效");
				return true;
			}
		}

		LOG_TABLE(L"即时调试玩家椅子:",(int)wDebugUserChair);
		m_GameLogic.AddCards(pUser->GetCards(), pUser->GetCount());
		pUser->SetCards(pNewConfig->stConfig.cbGameCards, MAX_CARDCOUNT);
		pUser->Lock();
		m_TableDebug.m_bDebugDoneChair[wDebugUserChair] = true;
		m_GameLogic.RemoveCards(pUser->GetCards(), pUser->GetCount());
		//
		UpdateUserCards(wDebugUserChair, pUser->GetCards(), pUser->GetCount());
		//
		if (m_stConfig == ST_SENDFOUR_)
			pUser->SetCurCount(MAX_CARDCOUNT - 1);
	}
	m_TableDebug.RefreshCardPool(m_GameLogic.GetPoolData(), m_GameLogic.GetPoolLeftCount());

	SendUserConfigDebug(pIServerUserItem, wDebugUserChair);
	return true;
};
//获取牌池数据
bool CTableFrameSink::OnGetPoolCard(IServerUserItem* pIServerUserItem, void* pData, WORD wDataSize)
{
	//权限判断
	//if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
	if (!IS_DEBUG_USER(pIServerUserItem->GetUserRight()))
	{
		LOG_TABLE(L"无调试权限,玩家:", pIServerUserItem->GetGameID());
		SendGameMessage(pIServerUserItem, L"无调试权限");
		return true;
	}

	CMD_S_PoolCard reply;
	ZeroMemory(&reply, sizeof(reply));
	BYTE* pPool = m_GameLogic.GetPoolData();
	for (int i = 0; i < m_GameLogic.GetPoolLeftCount(); ++i)
	{
		unsigned int cbIdx = m_TableDebug.GetCardHelper().GetCardIndex(pPool[i]);
		reply.bCards[cbIdx] = true;
	}
	
	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_GET_POOL_CARD, &reply, sizeof(reply));
	return true;
};
//换牌
bool CTableFrameSink::OnChangeCardType(IServerUserItem* pIServerUserItem, void* pData, WORD wDataSize)
{
	LOG_TABLE(L"获取换牌，玩家:", pIServerUserItem->GetGameID());
	//权限判断
	//if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
	if (!IS_DEBUG_USER(pIServerUserItem->GetUserRight()))
	{
		LOG_TABLE(L"无调试权限,玩家:", pIServerUserItem->GetGameID());
		SendGameMessage(pIServerUserItem, L"无调试权限");
		return true;
	}
	if (wDataSize != sizeof(CMD_C_RequestCardType))
		return true;

	CMD_C_RequestCardType* pRequest = (CMD_C_RequestCardType*)pData;

	CMD_S_RequestCardType reply;
	ZeroMemory(&reply,sizeof(reply));

	if (pRequest->cbCardType < 0 || pRequest->cbCardType >= MAX_CARD_TYPE)
	{
		SendGameMessage(pIServerUserItem, L"无效牌型");
		return true;
	}
	WORD wDebugUserChair = pRequest->wDebugUserChair;
	if (wDebugUserChair < 0 || wDebugUserChair >= GAME_PLAYER)
	{
		SendGameMessage(pIServerUserItem, L"无效调试玩家");
		return true;
	}
	CUserDebug* pUser = m_TableDebug.GetDebugUser(wDebugUserChair);

	memcpy(reply.cbGameCards, pUser->GetCards(), sizeof(reply.cbGameCards));
	bool bRet = GetTypeCards((emUserDebugType)pRequest->cbCardType, reply.cbGameCards);
	if (!bRet)
	{
		//ASSERT(false);
		SendGameMessage(pIServerUserItem, L"牌型冲突，请求失败");
		return true;
	}
	
	CString str;
	str.Format(L"[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,]", reply.cbGameCards[0], reply.cbGameCards[1], reply.cbGameCards[2], reply.cbGameCards[3], reply.cbGameCards[4]);
	LOG_TABLE(L"玩家:",pIServerUserItem->GetGameID(),L"牌数据:",str);
	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_REQUEST_CARDTYPE, &reply, sizeof(reply));
	return true;
};

void CTableFrameSink::SendUserConfigDebug(IServerUserItem* pIServerUserItem, WORD wDebugChair)
{
	if (wDebugChair < 0 || wDebugChair >= GAME_PLAYER)
	{
		LOG_TABLE(L"无效调试椅子号");
		return;
	}
	CMD_S_ConfigDebug reply;
	ZeroMemory(&reply, sizeof(reply));

	WORD wDebugUserChair = wDebugChair;
	CUserDebug* pUser = m_TableDebug.GetDebugUser(wDebugUserChair);
	if (wDebugUserChair == INVALID_CHAIR || pUser == nullptr)
	{
		LOG_TABLE(L"获取调试玩家错误!");
		ASSERT(false);
		return;
	}

	stServerConfig* pUserConfig = m_TableDebug.GetUserConfig(wDebugChair);
	if (pUserConfig == nullptr)
	{
		reply.stConfig.wDebugUserChair = wDebugChair;
		reply.stConfig.wConfigUserChair = pIServerUserItem->GetChairID();
		GetCurValidType(reply.bValidDebugType, wDebugChair);
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_CONFIG_DEBUG, &reply, sizeof(reply));
		return;
	}

	memcpy(&reply.stConfig, &(pUserConfig->stConfig), sizeof(reply.stConfig));
	memcpy(reply.cCards, pUser->GetCards(), sizeof(BYTE)*pUser->GetCount());
	reply.cbCurDebugCount = pUserConfig->nCurDebugCount;
	reply.cbCurCardType = EM_DEBUG_CARD_OX_0;
	if (reply.stConfig.cbType == 2 && pUserConfig->nCurConfigCardCount == MAX_CONFIG_CARD_COUNT)
		reply.cbCurCardType = m_TableDebug.GetCardHelper().GetType(reply.stConfig.cbGameCards);
	GetCurValidType(reply.bValidDebugType, wDebugChair);

	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_CONFIG_DEBUG, &reply, sizeof(reply));
};
//
BYTE CTableFrameSink::GetMaxCardChair(BYTE* pUserCards[GAME_PLAYER])
{
	WORD wMaxCardChair = INVALID_CHAIR;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == FALSE)
			continue;
		ASSERT(pUserCards[i] != nullptr);
		if (pUserCards[i] == nullptr)
			continue;

		if (wMaxCardChair == INVALID_CHAIR)
		{
			wMaxCardChair = i;
			continue;
		}
		if (m_GameLogic.CompareCard(pUserCards[wMaxCardChair], pUserCards[i], MAX_CARDCOUNT, m_ctConfig, INVALID_BYTE, INVALID_BYTE))
		{
			wMaxCardChair = i;
		}

	}

	//LOG_TABLE(L"调试结果，最大牌玩家椅子:",(int)wMaxCardChair);

	ASSERT(wMaxCardChair != INVALID_CHAIR);
		
	return wMaxCardChair;
};
void CTableFrameSink::GetCurValidType(bool bDebugCardType[MAX_CARD_TYPE], WORD wDebugChair)
{
	CUserDebug* pUser = m_TableDebug.GetDebugUser(wDebugChair);
	if (pUser == nullptr)
	{
		ASSERT(false);
		return;
	}
	//未发牌
	if (pUser->GetCount() == 0)
	{
		for (int i = EM_DEBUG_CARD_OX_0; i < EM_DEBUG_CARD_MAX; ++i)
		{
			if (i > EM_DEBUG_CARD_OX && !m_cbEnableCardType[i - EM_DEBUG_CARD_OX - 1])
				continue;
			bDebugCardType[i] = true;
		}
		return;
	}

	//发四等五
	if (pUser->GetCount() > 0 && m_stConfig == ST_SENDFOUR_ && !m_bOpenCard[wDebugChair])
	{
		BYTE cbTmpCards[MAX_CARDCOUNT];
		memcpy(cbTmpCards, pUser->GetCards(), sizeof(cbTmpCards));

		BYTE* pPool = m_GameLogic.GetPoolData();
		for (int i = 0; i < m_GameLogic.GetPoolLeftCount(); ++i)
		{
			cbTmpCards[MAX_CARDCOUNT - 1] = pPool[i];
			emUserDebugType emCurGetType = m_TableDebug.GetCardHelper().GetType(cbTmpCards);
			if (emCurGetType <= EM_DEBUG_CARD_OX || m_cbEnableCardType[emCurGetType - EM_DEBUG_CARD_OX-1])
				bDebugCardType[emCurGetType] = true;
			else
				bDebugCardType[emCurGetType] = false;
		}

		return;
	}

	m_TableDebug.GetCurValidDebugType(bDebugCardType);
};
bool CTableFrameSink::GetTypeCards(emUserDebugType emType, BYTE* pCards)
{
	if (m_stConfig == ST_SENDFOUR_ && m_TableDebug.GetDebugUser(0)->GetCount() > 0)
	{
		BYTE* pPool = m_GameLogic.GetPoolData();
		for (int i = 0; i < m_GameLogic.GetPoolLeftCount(); ++i)
		{
			pCards[MAX_CARDCOUNT - 1] = pPool[i];
			if (m_TableDebug.GetCardHelper().GetType(pCards) == emType)
				return true;
		}

		return false;
	}

	return m_TableDebug.GetCardHelper().GetTypeCards(emType, pCards);
};
void CTableFrameSink::SendGameMessage(IServerUserItem* pIServerUserItem,CString str)
{
	LOG_TABLE(str);

	CMD_S_ErrorNotify notify;
	if (str.GetLength() > sizeof(notify.tErrorInfo) / sizeof(notify.tErrorInfo[0]))
	{
		ASSERT(false);
		return;
	}

	wsprintf(notify.tErrorInfo, L"%s", str);
	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_ERROR_NOTIFY, &notify, sizeof(notify));

};
//更新配置(私人房)
void CTableFrameSink::UpdateConfigPersonRoom()
{
	//牌型最大倍数
	tagCustomRule *pCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();
	m_lMaxCardTimes = INVALID_BYTE;

	if (pCustomRule->ctConfig == CT_CLASSIC_)
	{
		m_lMaxCardTimes = pCustomRule->cbCardTypeTimesClassic[0];
		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			if (pCustomRule->cbCardTypeTimesClassic[i] > m_lMaxCardTimes)
			{
				m_lMaxCardTimes = pCustomRule->cbCardTypeTimesClassic[i];
			}
		}
	}
	else if (pCustomRule->ctConfig == CT_ADDTIMES_)
	{
		m_lMaxCardTimes = pCustomRule->cbCardTypeTimesAddTimes[0];
		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			if (pCustomRule->cbCardTypeTimesAddTimes[i] > m_lMaxCardTimes)
			{
				m_lMaxCardTimes = pCustomRule->cbCardTypeTimesAddTimes[i];
			}
		}
	}
	ASSERT(m_lMaxCardTimes != INVALID_BYTE);

	//设置牌型倍数

	if (m_cbClassicTypeConfig == INVALID_BYTE)
	{
		m_GameLogic.SetCardTypeTimes(pCustomRule->cbCardTypeTimesAddTimes);
	}
	else if (m_cbClassicTypeConfig == 0)
	{
		//牌型倍数
		m_GameLogic.SetCardTypeTimes(pCustomRule->cbCardTypeTimesClassic);
	}
	else if (m_cbClassicTypeConfig == 1)
	{
		//牌型倍数
		m_GameLogic.SetCardTypeTimes(pCustomRule->cbCardTypeTimesClassic);
	}

	//设置牌型激活
	m_GameLogic.SetEnableCardType(m_cbEnableCardType);
};

//获取玩家调试类型
EM_CTRL_TYPE CTableFrameSink::GetUserCtrlType(WORD & wCtrlChairID, bool & bWin, ROOMUSERINFO & keyroomuserinfo)
{
	//默认收放表调试
	EM_CTRL_TYPE ctrlType = SYS_CTRL;

	if (IsGroupTreasureRoom())
	{
		//获取当前共享库存子项
		GetCurSubStorageItem();

		if (m_CurSubStorageItem.lCurTempSysStorage > 0 && m_CurSubStorageItem.lCurTempPlayerStorage)
		{
			return ROOM_CTRL;
		}

		return SYS_CTRL;
	}
	else
	{
		//（优先级，牌桌>房间>系统）
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

			if (!pIServerUserItem)
			{
				continue;
			}

			stServerConfig* pCurUserConfig = m_TableDebug.GetUserConfig(i);

			if (pCurUserConfig)
			{
				return CLIENT_CARDCONFIG_CTRL;
			}

		}

		//查找牌桌控制
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

			if (!pIServerUserItem)
			{
				continue;
			}

			ROOMUSERINFO roomuserinfo;
			ZeroMemory(&roomuserinfo, sizeof(roomuserinfo));

			FindRoomUserInfo(pIServerUserItem->GetUserID(), roomuserinfo);

			if (roomuserinfo.tablectrl_type != TABLECTRL_INVALID && (roomuserinfo.tablectrl_type == BANKER_WIN || roomuserinfo.tablectrl_type == BANKER_LOST)
				&& (roomuserinfo.cbCurCtrlTimes < roomuserinfo.cbConfigCtrlTimes))
			{
				wCtrlChairID = m_wBankerUser;
				bWin = roomuserinfo.tablectrl_type == BANKER_WIN ? true : false;
				CopyMemory(&keyroomuserinfo, &roomuserinfo, sizeof(roomuserinfo));

				return TABLE_CTRL;
			}
			else if (roomuserinfo.tablectrl_type != TABLECTRL_INVALID && (roomuserinfo.tablectrl_type == CURPLAYER_WIN || roomuserinfo.tablectrl_type == CURPLAYER_LOST)
				&& (roomuserinfo.cbCurCtrlTimes < roomuserinfo.cbConfigCtrlTimes))
			{
				wCtrlChairID = roomuserinfo.wChairID;
				bWin = roomuserinfo.tablectrl_type == CURPLAYER_WIN ? true : false;
				CopyMemory(&keyroomuserinfo, &roomuserinfo, sizeof(roomuserinfo));

				return TABLE_CTRL;
			}
		}

		//查找房间调试
		POSITION pos = m_listRoomCtrl.GetHeadPosition();
		POSITION firstQueueCtrlPos = NULL;
		while (pos)
		{
			POSITION posTemp = pos;

			ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

			if (roomctrl.roomCtrlStatus == PROGRESSING)
			{
				return ROOM_CTRL;
			}
			else if (roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
			{
				firstQueueCtrlPos = posTemp;
			}
		}

		//设置第一条记录为进行状态
		if (firstQueueCtrlPos)
		{
			ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);
			roomctrl.roomCtrlStatus = PROGRESSING;

			return ROOM_CTRL;
		}
	}

	return SYS_CTRL;
}

//更新控制结果
void CTableFrameSink::UpdateCtrlRes(EM_CTRL_TYPE emCtrlType)
{
	//AI或者系统当庄
	bool bAIOrSysBanker = ((m_wBankerUser == INVALID_CHAIR) || (m_wBankerUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wBankerUser) && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser()));

	switch (emCtrlType)
	{
	case ROOM_CTRL:
	{
		//查找房间调试
		POSITION pos = m_listRoomCtrl.GetHeadPosition();

		//当系统库存为负数,达到初始值的库存重置比例,重置库存
		bool bResetStorage = false;

		if (IsGroupTreasureRoom())
		{
			for (WORD wChairID = 0; wChairID < m_wPlayerCount; wChairID++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
				if (pIServerUserItem == NULL)
				{
					continue;
				}

				//真人当庄，真人不走库存
				//AI或者系统当庄,AI不走库存
				if ((!bAIOrSysBanker && !pIServerUserItem->IsAndroidUser()) || (bAIOrSysBanker && pIServerUserItem->IsAndroidUser()))
				{
					continue;
				}

				//真人
				if (!pIServerUserItem->IsAndroidUser())
				{
					//该牌桌赢分
					if (m_tagGameScoreInfo[wChairID].lWinScore > 0)
					{
						m_CurSubStorageItem.lCurTempPlayerStorage -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurStatisticSysWin -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
					}
					//该牌桌输分
					else if (m_tagGameScoreInfo[wChairID].lWinScore < 0)
					{
						m_CurSubStorageItem.lCurTempSysStorage -= (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurStatisticSysWin += (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
					}
				}
				//AI
				else
				{
					//该牌桌赢分
					if (m_tagGameScoreInfo[wChairID].lWinScore > 0)
					{
						m_CurSubStorageItem.lCurTempSysStorage -= (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurStatisticSysWin += (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
					}
					//该牌桌输分
					else if (m_tagGameScoreInfo[wChairID].lWinScore < 0)
					{
						m_CurSubStorageItem.lCurTempPlayerStorage -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurStatisticSysWin -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
					}
				}
			}

			//统计税收
			for (WORD wChairID = 0; wChairID < m_wPlayerCount; wChairID++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
				if (pIServerUserItem == NULL || pIServerUserItem->IsAndroidUser())
				{
					continue;
				}

				//真人统计税收
				m_CurSubStorageItem.lCurStatisticService += m_tagGameScoreInfo[wChairID].lRevenue;
			}

			//获取配置共享库存子项
			GetConfigSubStorageItem();

			if (m_CurSubStorageItem.lCurTempSysStorResetRate > 0 && m_CurSubStorageItem.lCurTempSysStorage < 0 && (-m_CurSubStorageItem.lCurTempSysStorage) >(m_ConfigSubStorageItem.lConfigTempSysStorage * m_CurSubStorageItem.lCurTempSysStorResetRate / 100))
			{
				bResetStorage = true;
			}

			//调试完成
			if ((m_CurSubStorageItem.lCurTempSysStorage <= 0 && m_CurSubStorageItem.lCurTempPlayerStorage <= 0) || bResetStorage)
			{
				//完成设置房间记录状态
				m_CurSubStorageItem.lCurTempSysStorage = 0;							
				m_CurSubStorageItem.lCurTempPlayerStorage = 0;							
				m_CurSubStorageItem.lCurTempParameterK = 0;								
				m_CurSubStorageItem.lCurTempSysStorResetRate = 0;						
				m_CurSubStorageItem.lCurTempWinChance = 0;						
			}
		}
		else
		{
			while (pos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == PROGRESSING)
				{
					for (WORD wChairID = 0; wChairID < m_wPlayerCount; wChairID++)
					{
						//获取用户
						IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
						if (pIServerUserItem == NULL)
						{
							continue;
						}

						//真人当庄，真人不走库存
						//AI或者系统当庄,AI不走库存
						if ((!bAIOrSysBanker && !pIServerUserItem->IsAndroidUser()) || (bAIOrSysBanker && pIServerUserItem->IsAndroidUser()))
						{
							continue;
						}

						//真人
						if (!pIServerUserItem->IsAndroidUser())
						{
							//该牌桌赢分
							if (m_tagGameScoreInfo[wChairID].lWinScore > 0)
							{
								roomctrl.lRoomCtrlCurPlayerStorage -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);

								roomctrl.lRoomCtrlSysWin -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
								g_lStatisticSysWin -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
							}
							//该牌桌输分
							else if (m_tagGameScoreInfo[wChairID].lWinScore < 0)
							{
								roomctrl.lRoomCtrlCurSysStorage -= (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);

								roomctrl.lRoomCtrlSysWin += (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
								g_lStatisticSysWin += (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
							}
						}
						//AI
						else
						{
							//该牌桌赢分
							if (m_tagGameScoreInfo[wChairID].lWinScore > 0)
							{
								roomctrl.lRoomCtrlCurSysStorage -= (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);

								roomctrl.lRoomCtrlSysWin += (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
								g_lStatisticSysWin += (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
							}
							//该牌桌输分
							else if (m_tagGameScoreInfo[wChairID].lWinScore < 0)
							{
								roomctrl.lRoomCtrlCurPlayerStorage -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);

								roomctrl.lRoomCtrlSysWin -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
								g_lStatisticSysWin -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
							}
						}
					}

					//统计税收
					for (WORD wChairID = 0; wChairID < m_wPlayerCount; wChairID++)
					{
						//获取用户
						IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
						if (pIServerUserItem == NULL || pIServerUserItem->IsAndroidUser())
						{
							continue;
						}

						//真人统计税收
						g_lStatisticService += m_tagGameScoreInfo[wChairID].lRevenue;
					}

					if (roomctrl.lRoomCtrlSysStorResetRate > 0 && roomctrl.lRoomCtrlCurSysStorage < 0 && (-roomctrl.lRoomCtrlCurSysStorage) >(roomctrl.lRoomCtrlConfigSysStorage * roomctrl.lRoomCtrlSysStorResetRate / 100))
					{
						bResetStorage = true;
					}

					//调试完成
					if ((roomctrl.lRoomCtrlCurSysStorage <= 0 && roomctrl.lRoomCtrlCurPlayerStorage <= 0) || bResetStorage)
					{
						//完成设置房间记录状态
						roomctrl.roomCtrlStatus = FINISH;
					}
					else
					{
						//发送结果
						CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
						ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

						CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
						CurRoomCtrlInfo.lRoomCtrlCurSysStorage = roomctrl.lRoomCtrlCurSysStorage;
						CurRoomCtrlInfo.lRoomCtrlCurPlayerStorage = roomctrl.lRoomCtrlCurPlayerStorage;
						CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
						CurRoomCtrlInfo.lRoomCtrlSysStorResetRate = roomctrl.lRoomCtrlSysStorResetRate;
						CurRoomCtrlInfo.lRoomCtrlSysWin = CalListRoomCtrlSysWin();

						// 发送消息
						m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
					}

					break;
				}
			}

			// 发送消息
			m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL_TRACELOG);
			pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				//发送结果
				CMD_S_RoomCtrlTraceLog roomCtrlResult;
				ZeroMemory(&roomCtrlResult, sizeof(roomCtrlResult));

				CopyMemory(&(roomCtrlResult.RoomCtrlTraceLog), &roomctrl, sizeof(roomctrl));

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL_TRACELOG, &roomCtrlResult, sizeof(roomCtrlResult));
			}

			//当前房间列表信息
			m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_CUR_ROOMCTRL_INFO);
			pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == QUEUE || roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//发送结果
					CMD_S_ListCurRoomCtrlInfo ListCurRoomCtrlInfo;
					ZeroMemory(&ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));

					CopyMemory(&(ListCurRoomCtrlInfo.curRoomCtrlListInfo), &roomctrl, sizeof(roomctrl));

					// 发送消息
					m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_CUR_ROOMCTRL_INFO, &ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));
				}
			}
		}

		break;
	}
	case SYS_CTRL:
	{
		//玩家赢
		//差值
		static SCORE lDivPlayerStorage = 0;
		static SCORE lDivSysStorage = 0;

		//当系统库存为负数,达到初始值的库存重置比例,重置库存
		bool bResetStorage = false;

		if (IsGroupTreasureRoom())
		{
			for (WORD wChairID = 0; wChairID < m_wPlayerCount; wChairID++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
				if (pIServerUserItem == NULL)
				{
					continue;
				}

				//真人当庄，真人不走库存
				//AI或者系统当庄,AI不走库存
				if ((!bAIOrSysBanker && !pIServerUserItem->IsAndroidUser()) || (bAIOrSysBanker && pIServerUserItem->IsAndroidUser()))
				{
					continue;
				}

				//真人
				if (!pIServerUserItem->IsAndroidUser())
				{
					//该牌桌赢分
					if (m_tagGameScoreInfo[wChairID].lWinScore > 0)
					{
						m_CurSubStorageItem.lCurPlayerStorage -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurSysWin -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurStatisticSysWin -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
					}
					//该牌桌输分
					else if (m_tagGameScoreInfo[wChairID].lWinScore < 0)
					{
						m_CurSubStorageItem.lCurSysStorage -= (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurSysWin += (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurStatisticSysWin += (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
					}
				}
				//AI
				else
				{
					//该牌桌赢分
					if (m_tagGameScoreInfo[wChairID].lWinScore > 0)
					{
						m_CurSubStorageItem.lCurSysStorage -= (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurSysWin += (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurStatisticSysWin += (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
					}
					//该牌桌输分
					else if (m_tagGameScoreInfo[wChairID].lWinScore < 0)
					{
						m_CurSubStorageItem.lCurPlayerStorage -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurSysWin -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
						m_CurSubStorageItem.lCurStatisticSysWin -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
					}
				}
			}

			//统计税收
			for (WORD wChairID = 0; wChairID < m_wPlayerCount; wChairID++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
				if (pIServerUserItem == NULL || pIServerUserItem->IsAndroidUser())
				{
					continue;
				}

				//真人统计税收
				m_CurSubStorageItem.lCurStatisticService += m_tagGameScoreInfo[wChairID].lRevenue;
			}

			m_CurSubStorageItem.lCurPlayerStorage = (m_CurSubStorageItem.lCurPlayerStorage <= 0 ? ((lDivPlayerStorage = -m_CurSubStorageItem.lCurPlayerStorage), m_CurSubStorageItem.lCurPlayerStorage) : m_CurSubStorageItem.lCurPlayerStorage);
			m_CurSubStorageItem.lCurSysStorage = (m_CurSubStorageItem.lCurSysStorage <= 0 ? ((lDivSysStorage = -m_CurSubStorageItem.lCurSysStorage), m_CurSubStorageItem.lCurSysStorage) : m_CurSubStorageItem.lCurSysStorage);

			//获取配置共享库存子项
			GetConfigSubStorageItem();

			if (m_CurSubStorageItem.lCurSysStorResetRate >  0 && m_CurSubStorageItem.lCurSysStorage < 0 && (-m_CurSubStorageItem.lCurSysStorage) >(m_ConfigSubStorageItem.lConfigSysStorage * m_CurSubStorageItem.lCurSysStorResetRate / 100))
			{
				bResetStorage = true;
				lDivPlayerStorage = -m_CurSubStorageItem.lCurPlayerStorage;
			}

			//库存重置
			if ((m_CurSubStorageItem.lCurSysStorage <= 0 && m_CurSubStorageItem.lCurPlayerStorage <= 0) || bResetStorage)
			{
				//次数累加
				m_CurSubStorageItem.lCurResetCount++;

				tagConfigSubStorageItem* pConfigSubStorageItem = m_pITableFrame->GetConfigSubStorageItem();
				if (pConfigSubStorageItem)
				{
					m_CurSubStorageItem.lCurSysStorage = pConfigSubStorageItem->lConfigSysStorage;
					m_CurSubStorageItem.lCurPlayerStorage = pConfigSubStorageItem->lConfigPlayerStorage;
				}

				m_CurSubStorageItem.lCurSysStorage -= lDivSysStorage;
				m_CurSubStorageItem.lCurPlayerStorage -= lDivPlayerStorage;

				//重置一次不够赔为0
				if (m_CurSubStorageItem.lCurSysStorage < 0)
				{
					m_CurSubStorageItem.lCurSysStorage = 0;
				}

				if (m_CurSubStorageItem.lCurPlayerStorage < 0)
				{
					m_CurSubStorageItem.lCurPlayerStorage = 0;
				}

				//差值初始化
				lDivPlayerStorage = 0;
				lDivSysStorage = 0;
			}
		}
		else
		{
			//查找系统调试
			POSITION pos = m_listSysCtrl.GetHeadPosition();

			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				if (sysctrl.sysCtrlStatus == PROGRESSINGEX)
				{
					for (WORD wChairID = 0; wChairID < m_wPlayerCount; wChairID++)
					{
						//获取用户
						IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
						if (pIServerUserItem == NULL)
						{
							continue;
						}

						//真人当庄，真人不走库存
						//AI或者系统当庄,AI不走库存
						if ((!bAIOrSysBanker && !pIServerUserItem->IsAndroidUser()) || (bAIOrSysBanker && pIServerUserItem->IsAndroidUser()))
						{
							continue;
						}

						//真人
						if (!pIServerUserItem->IsAndroidUser())
						{
							//该牌桌赢分
							if (m_tagGameScoreInfo[wChairID].lWinScore > 0)
							{
								sysctrl.lSysCtrlCurPlayerStorage -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);

								sysctrl.lSysCtrlSysWin -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
								g_lStatisticSysWin -= (m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
							}
							//该牌桌输分
							else if (m_tagGameScoreInfo[wChairID].lWinScore < 0)
							{
								sysctrl.lSysCtrlCurSysStorage -= (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);

								sysctrl.lSysCtrlSysWin += (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
								g_lStatisticSysWin += (-m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
							}
						}
						//AI
						else
						{
							//该牌桌赢分
							if (m_tagGameScoreInfo[wChairID].lWinScore > 0)
							{
								sysctrl.lSysCtrlCurSysStorage -= (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);

								sysctrl.lSysCtrlSysWin += (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
								g_lStatisticSysWin += (m_tagGameScoreInfo[wChairID].lWinScore - m_tagGameScoreInfo[wChairID].lAnChou);
							}
							//该牌桌输分
							else if (m_tagGameScoreInfo[wChairID].lWinScore < 0)
							{
								sysctrl.lSysCtrlCurPlayerStorage -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);

								sysctrl.lSysCtrlSysWin -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
								g_lStatisticSysWin -= (-m_tagGameScoreInfo[wChairID].lWinScore + m_tagGameScoreInfo[wChairID].lAnChou);
							}
						}
					}

					//统计税收
					for (WORD wChairID = 0; wChairID < m_wPlayerCount; wChairID++)
					{
						//获取用户
						IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
						if (pIServerUserItem == NULL || pIServerUserItem->IsAndroidUser())
						{
							continue;
						}

						//真人统计税收
						g_lStatisticService += m_tagGameScoreInfo[wChairID].lRevenue;
					}

					sysctrl.lSysCtrlCurPlayerStorage = (sysctrl.lSysCtrlCurPlayerStorage <= 0 ? ((lDivPlayerStorage = -sysctrl.lSysCtrlCurPlayerStorage), sysctrl.lSysCtrlCurPlayerStorage) : sysctrl.lSysCtrlCurPlayerStorage);
					sysctrl.lSysCtrlCurSysStorage = (sysctrl.lSysCtrlCurSysStorage <= 0 ? ((lDivSysStorage = -sysctrl.lSysCtrlCurSysStorage), sysctrl.lSysCtrlCurSysStorage) : sysctrl.lSysCtrlCurSysStorage);

					if (sysctrl.lSysCtrlSysStorResetRate >  0 && sysctrl.lSysCtrlCurSysStorage < 0 && (-sysctrl.lSysCtrlCurSysStorage) >(sysctrl.lSysCtrlConfigSysStorage * sysctrl.lSysCtrlSysStorResetRate / 100))
					{
						bResetStorage = true;
						lDivPlayerStorage = -sysctrl.lSysCtrlCurPlayerStorage;
					}

					//库存重置
					if ((sysctrl.lSysCtrlCurSysStorage <= 0 && sysctrl.lSysCtrlCurPlayerStorage <= 0) || bResetStorage)
					{
						//次数累加
						sysctrl.lSysCtrlResetCount++;

						sysctrl.lSysCtrlCurSysStorage = sysctrl.lSysCtrlConfigSysStorage;
						sysctrl.lSysCtrlCurPlayerStorage = sysctrl.lSysCtrlConfigPlayerStorage;

						sysctrl.lSysCtrlCurSysStorage -= lDivSysStorage;
						sysctrl.lSysCtrlCurPlayerStorage -= lDivPlayerStorage;

						//重置一次不够赔为0
						if (sysctrl.lSysCtrlCurSysStorage < 0)
						{
							sysctrl.lSysCtrlCurSysStorage = 0;
						}

						if (sysctrl.lSysCtrlCurPlayerStorage < 0)
						{
							sysctrl.lSysCtrlCurPlayerStorage = 0;
						}

						//差值初始化
						lDivPlayerStorage = 0;
						lDivSysStorage = 0;

						// 定义信息
						CMD_S_DebugText DebugText;
						ZeroMemory(&DebugText, sizeof(DebugText));

						_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("当前库存已重置,重置次数%I64d"), sysctrl.lSysCtrlResetCount);

						// 发送消息
						m_pITableFrame->SendRoomData(NULL, SUB_S_DEBUGTEXT_GENERAL, &DebugText, sizeof(DebugText));
					}

					//变量定义
					CMD_S_CurSysCtrlInfo CurSysCtrlInfo;
					ZeroMemory(&CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

					CurSysCtrlInfo.lSysCtrlCurSysStorage = sysctrl.lSysCtrlCurSysStorage;
					CurSysCtrlInfo.lSysCtrlCurPlayerStorage = sysctrl.lSysCtrlCurPlayerStorage;
					CurSysCtrlInfo.lSysCtrlParameterK = sysctrl.lSysCtrlParameterK;
					CurSysCtrlInfo.lSysCtrlSysStorResetRate = sysctrl.lSysCtrlSysStorResetRate;
					CurSysCtrlInfo.lSysCtrlResetCount = sysctrl.lSysCtrlResetCount;
					CurSysCtrlInfo.lSysCtrlSysWin = CalListSysCtrlSysWin();

					//发送数据
					m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_SYSCTRL_INFO, &CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

					//通用库存调试列表
					pos = m_listSysCtrl.GetHeadPosition();
					while (pos)
					{
						SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

						//发送结果
						CMD_S_SysCtrlTraceLog sysCtrlResult;
						ZeroMemory(&sysCtrlResult, sizeof(sysCtrlResult));

						CopyMemory(&(sysCtrlResult.SysCtrlTraceLog), &sysctrl, sizeof(sysctrl));

						// 发送消息
						m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL_TRACELOG, &sysCtrlResult, sizeof(sysCtrlResult));
					}

					break;
				}
			}
		}

		break;
	}
	default:
		break;
	}

	CMD_S_StartRecordTraceLog StartRecordTraceLog;
	ZeroMemory(&StartRecordTraceLog, sizeof(StartRecordTraceLog));

	StartRecordTraceLog.tmStartRecord = m_tmStartRecord;

	// 发送消息
	m_pITableFrame->SendRoomData(NULL, SUB_S_STARTRECORD_TRACELOG, &StartRecordTraceLog, sizeof(StartRecordTraceLog));
}

//查找房间控制
POSITION CTableFrameSink::FindRoomCtrl(DWORD dwSelCtrlIndex)
{
	POSITION posKey = NULL;

	ROOMCTRL roomctrl;
	ZeroMemory(&roomctrl, sizeof(roomctrl));
	POSITION posHead = m_listRoomCtrl.GetHeadPosition();

	while (posHead)
	{
		posKey = posHead;
		ZeroMemory(&roomctrl, sizeof(roomctrl));

		roomctrl = m_listRoomCtrl.GetNext(posHead);
		if (roomctrl.dwCtrlIndex == dwSelCtrlIndex)
		{
			return posKey;
		}
	}

	return NULL;
}

//查找牌桌控制
POSITION CTableFrameSink::FindTableCtrl(ROOMUSERINFO userinfo)
{
	POSITION posKey = NULL;

	TABLECTRL tablectrl;
	ZeroMemory(&tablectrl, sizeof(tablectrl));
	POSITION posHead = m_listTableCtrl.GetHeadPosition();

	while (posHead)
	{
		posKey = posHead;
		ZeroMemory(&tablectrl, sizeof(tablectrl));

		tablectrl = m_listTableCtrl.GetNext(posHead);
		if (tablectrl.roomUserInfo.dwGameID == userinfo.dwGameID)
		{
			return posKey;
		}
	}

	return NULL;
}

//房间同时存在AI和真人玩家
bool CTableFrameSink::IsContainAIAndRealPlayer()
{
	WORD wRealPlayerInScoreCount = 0;
	WORD wAIInScoreCount = 0;

	for (WORD wChairID = 0; wChairID < m_wPlayerCount; wChairID++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (pIServerUserItem->IsAndroidUser())
		{
			wAIInScoreCount++;
		}
		else
		{
			wRealPlayerInScoreCount++;
		}
	}

	return (wAIInScoreCount > 0 && wRealPlayerInScoreCount > 0);
}

//牌桌控制有效
bool CTableFrameSink::IsTableCtrlSysValid()
{
	//真人坐庄，真人玩的情况
	WORD wAIInScoreCount = 0;

	for (WORD wChairID = 0; wChairID < m_wPlayerCount; wChairID++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		SCORE lUserAllBet = 0;
		//for (int nAreaIndex = 0; nAreaIndex<AREA_MAX; nAreaIndex++)
		//{
		//	lUserAllBet += m_lUserInAllScore[wChairID][nAreaIndex];
		//}

		if (lUserAllBet <= 0)
		{
			continue;
		}

		if (pIServerUserItem->IsAndroidUser())
		{
			wAIInScoreCount++;
		}
	}

	if (m_wBankerUser != INVALID_CHAIR && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser() && wAIInScoreCount <= 0)
	{
		return false;
	}

	return true;
}

//统计房间控制列表
SCORE CTableFrameSink::CalListSysCtrlSysWin()
{
	SCORE lSysCtrlSysWin = 0;

	POSITION pos = m_listSysCtrl.GetHeadPosition();
	while (pos)
	{
		SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

		lSysCtrlSysWin += sysctrl.lSysCtrlSysWin;
	}

	return lSysCtrlSysWin;
}

//统计房间控制列表
SCORE CTableFrameSink::CalListRoomCtrlSysWin()
{
	SCORE lRoomCtrlSysWin = 0;

	POSITION pos = m_listRoomCtrl.GetHeadPosition();
	while (pos)
	{
		ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

		lRoomCtrlSysWin += roomctrl.lRoomCtrlSysWin;
	}

	return lRoomCtrlSysWin;
}

//统计牌桌控制列表
SCORE CTableFrameSink::CalListTableCtrlSysWin()
{
	SCORE lTableCtrlSysWin = 0;

	POSITION pos = m_listTableCtrl.GetHeadPosition();
	while (pos)
	{
		TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

		lTableCtrlSysWin += tablectrl.lTableCtrlSysWin;
	}

	return lTableCtrlSysWin;
}

//查找房间玩家
BOOL CTableFrameSink::FindRoomUserInfo(DWORD dwCtrlUserID, ROOMUSERINFO & roomuserinfo)
{
	return g_MapRoomUserInfo.Lookup(dwCtrlUserID, roomuserinfo);
}

//查重同桌调试
bool CTableFrameSink::TravelSameTableCtrlList(ROOMUSERINFO keyroomuserinfo)
{
	//遍历映射
	POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
	DWORD dwUserID = 0;
	ROOMUSERINFO userinfo;
	ZeroMemory(&userinfo, sizeof(userinfo));

	while (ptHead)
	{
		g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

		if ((userinfo.wTableID == keyroomuserinfo.wTableID) && (userinfo.tablectrl_type != TABLECTRL_INVALID))
		{
			return true;
		}
	}

	return false;
}

//移除牌桌调试庄家类型
void CTableFrameSink::RemoveTableCtrlBankerType(IServerUserItem *pIServerUserItem)
{
	//遍历映射
	POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
	DWORD dwUserID = 0;
	ROOMUSERINFO userinfo;
	ZeroMemory(&userinfo, sizeof(userinfo));
	bool bValid = false;

	while (ptHead)
	{
		g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

		if ((userinfo.dwGameID == pIServerUserItem->GetGameID()) && ((userinfo.tablectrl_type == BANKER_WIN) || (userinfo.tablectrl_type == BANKER_LOST)))
		{
			userinfo.tablectrl_type = TABLECTRL_INVALID;

			g_MapRoomUserInfo.SetAt(pIServerUserItem->GetUserID(), userinfo);

			bValid = true;

			break;
		}
	}

	POSITION pos = m_listTableCtrl.GetHeadPosition();
	while (pos)
	{
		TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

		if (bValid && (tablectrl.tableCtrlStatus == TABLECTRL_PROGRESSING) && (tablectrl.roomUserInfo.dwGameID == pIServerUserItem->GetGameID())
		&& ((tablectrl.roomUserInfo.tablectrl_type == BANKER_WIN) || (tablectrl.roomUserInfo.tablectrl_type == BANKER_LOST)))
		{
			tablectrl.tableCtrlStatus = (tablectrl.lTableCtrlSysWin == 0) ? TABLECTRL_CANCEL : TABLECTRL_EXECUTE_CANCEL;

			CopyMemory(&(tablectrl.roomUserInfo), &userinfo, sizeof(userinfo));

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_TABLECTRL_INFO);

			break;
		}
	}

	//当前房间列表信息
	m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_TABLECTRL_INFO);
	pos = m_listTableCtrl.GetHeadPosition();
	while (pos)
	{
		TABLECTRL &tablectrl = m_listTableCtrl.GetNext(pos);

		if (tablectrl.tableCtrlStatus == TABLECTRL_PROGRESSING)
		{
			//发送结果
			CMD_S_ListCurTableCtrlInfo ListCurTableCtrlInfo;
			ZeroMemory(&ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));

			CopyMemory(&(ListCurTableCtrlInfo.curTableCtrlListInfo), &tablectrl, sizeof(tablectrl));

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_TABLECTRL_INFO, &ListCurTableCtrlInfo, sizeof(ListCurTableCtrlInfo));
		}
	}
}

//是否大联盟房间
bool CTableFrameSink::IsGroupTreasureRoom()
{
	return (m_pITableFrame->GetGroupID() > 0) && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && m_pITableFrame->GetDataBaseMode() == 1;
}

//更新同桌其他玩家椅子数目
void CTableFrameSink::UpdateSameTableChairCount(IServerUserItem *pIServerUserItem, USERACTION userAction, WORD wTableID)
{
	//遍历映射
	POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
	DWORD dwUserID = 0;
	ROOMUSERINFO userinfo;
	ZeroMemory(&userinfo, sizeof(userinfo));

	while (ptHead)
	{
		g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

		if (userinfo.wTableID != wTableID || userinfo.dwGameID == pIServerUserItem->GetGameID())
		{
			continue;
		}

		if (userAction == USER_SITDOWN)
		{
			userinfo.wCurTablePlayerCount++;

			g_MapRoomUserInfo.SetAt(userinfo.dwUserID, userinfo);

			POSITION pos = FindTableCtrl(userinfo);
			if (pos)
			{
				TABLECTRL & tablectrl = m_listTableCtrl.GetAt(pos);
				CopyMemory(&(tablectrl.roomUserInfo), &userinfo, sizeof(userinfo));
			}
		}
		else if (userAction == USER_STANDUP)
		{
			userinfo.wCurTablePlayerCount--;

			g_MapRoomUserInfo.SetAt(userinfo.dwUserID, userinfo);

			POSITION pos = FindTableCtrl(userinfo);
			if (pos)
			{
				TABLECTRL & tablectrl = m_listTableCtrl.GetAt(pos);
				CopyMemory(&(tablectrl.roomUserInfo), &userinfo, sizeof(userinfo));
			}
		}
	}
}

//获取当前共享库存子项
void CTableFrameSink::GetCurSubStorageItem()
{
	if (IsGroupTreasureRoom())
	{
		ZeroMemory(&m_CurSubStorageItem, sizeof(m_CurSubStorageItem));

		tagCurSubStorageItem* pCurSubStorageItem = m_pITableFrame->GetCurSubStorageItem();
		if (pCurSubStorageItem)
		{
			CopyMemory(&m_CurSubStorageItem, pCurSubStorageItem, sizeof(tagCurSubStorageItem));
		}
	}
}

//获取配置共享库存子项
void CTableFrameSink::GetConfigSubStorageItem()
{
	if (IsGroupTreasureRoom())
	{
		ZeroMemory(&m_ConfigSubStorageItem, sizeof(m_ConfigSubStorageItem));

		tagConfigSubStorageItem* pConfigSubStorageItem = m_pITableFrame->GetConfigSubStorageItem();
		if (pConfigSubStorageItem)
		{
			CopyMemory(&m_ConfigSubStorageItem, pConfigSubStorageItem, sizeof(tagConfigSubStorageItem));
		}
	}
}

//修改共享库存子项
void CTableFrameSink::ChangeCurSubStorageItem()
{
	if (IsGroupTreasureRoom())
	{
		m_pITableFrame->ChangeCurSubStorageItem(_MODIFY_SUBITEM_, m_CurSubStorageItem);
	}
}

//////////////////////////////////////////////////////////////////////////
