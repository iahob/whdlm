#include "StdAfx.h"
#include "TableFrameSink.h"
#include <locale>
#include <math.h>
#include "CLog.h"
//////////////////////////////////////////////////////////////////////////////////
#define IDI_ROOM_LOTTERY		1														//更新彩金
#define IPC_PACKAGE				8192

CGameControl		CTableFrameSink::m_gameStorage;

SCOREEX							g_dSummationScore = 0;									//系统累加输赢

//调试映射
CMap<DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo> CTableFrameSink::m_mapRoomUserInfo;

//构造函数
CTableFrameSink::CTableFrameSink()
{
	m_bGameStart = false;
	m_cbLineCount = MAX_LINE_COUNT;
    m_cbBetIndex = 0;
    m_dUserBetScore = 0;
    m_dUserBetTotalScore = 0;
	m_dUserTotalGameScore = 0;
    m_nGiveAwayCount = 0;
	ZeroMemory(m_cbItemInfo, sizeof(m_cbItemInfo));
	ZeroMemory(m_dLineScore, sizeof(m_dLineScore));
	memset(m_cbItemType, INVALID_ITEM, sizeof(m_cbItemType));
	m_nSuperRabbit = 0;
	m_dSuperRabbitScore = 0;
	m_bSuperRabbit = false;
	m_cbBetCount = 10;
	SCOREEX dTempBetScore[MAX_BET_COUNT] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	CopyMemory(m_dBetScore, dTempBetScore, sizeof(dTempBetScore));
	m_dUserRevenue = 0;

    //组件变量
    m_pITableFrame = NULL;
    m_pGameServiceOption = NULL;
    m_pGameServiceAttrib = NULL;
    m_pGameInfoWR = NULL;

	srand((unsigned int)time(NULL));
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
	m_mapRoomUserInfo.RemoveAll();
}

//接口查询
VOID *CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
    QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
    return NULL;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
    m_dUserBetTotalScore = 0;
    m_dUserTotalGameScore = 0;
	m_dUserRevenue = 0;
	ZeroMemory(m_dLineScore, sizeof(m_dLineScore));
	memset(m_cbItemType, INVALID_ITEM, sizeof(m_cbItemType));

    return;
}

//配置桌子
bool CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
    //查询接口
    m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);

    //错误判断
    if(m_pITableFrame == NULL)
    {
        CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"), TraceLevel_Exception);
        return NcaTextOut(__LINE__);
    }

    //开始模式
    m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

    //游戏配置
    m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
    m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

    // 获取游戏信息组件
    m_pGameInfoWR = CGameInfoWR::GetInstance();

	m_pITableFrame->SetGameTimer(IDI_ROOM_LOTTERY, 1000 * 60, -1, NULL);

	//系统库存初始化
	InitSystemStorage();

	//日志
	CString strDir = GetFileDialogPath() + TEXT("\\") + GAME_NAME + TEXT("日志");
	CreateDirectory(strDir, NULL);
	strDir.AppendFormat(TEXT("\\%s(%d)"), GAME_NAME, m_pITableFrame->GetGameServiceOption()->wServerID);
	EasyLog::CLog::m_log.SetName(strDir.GetBuffer());
	LOG_DEBUG(m_pITableFrame->GetTableID(), TEXT("桌子初始化!"));

    return true;
}

//消费能力
SCOREEX CTableFrameSink::QueryConsumeQuota(IServerUserItem *pIServerUserItem)
{
    return 0L;
}

//最少积分
SCOREEX CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem *pIServerUserItem)
{
    return 0;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
    //设置状态
    if(m_bGameStart)
    {
        m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);
    }
    else
    {
        m_pITableFrame->SetGameStatus(GAME_SCENE_WIAT);
    }

    return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    switch(cbReason)
    {
    case GER_NORMAL:		//常规结束
    {
        if(m_bGameStart)
        {
            //积分变量
            tagScoreInfo ScoreInfoArray[GAME_PLAYER];
            ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

            if(pIServerUserItem != NULL)
            {
				CString strDebug;
				strDebug.Format(TEXT("writescore = %.2lf"), m_dUserBetTotalScore + m_dUserTotalGameScore);
				//CTraceService::TraceString(strDebug, TraceLevel_Exception);

				if (m_nSuperRabbit > 0 && D_GreaterThen(m_dSuperRabbitScore, (SCOREEX)0))
				{
					ScoreInfoArray[0].lScore = m_dSuperRabbitScore + m_dUserBetTotalScore;
				}
				else
				{
					if (m_bSuperRabbit)
					{
						SCOREEX dLineScore = 0;
						for (WORD i = 0; i < MAX_LINE_COUNT; i++)
						{
							dLineScore += m_dLineScore[i];
						}

						ScoreInfoArray[0].lScore = m_dUserBetTotalScore + dLineScore;
					}
					else
					{
						ScoreInfoArray[0].lScore = m_dUserBetTotalScore + m_dUserTotalGameScore;
					}
				}
				
				ScoreInfoArray[0].cbType = (D_GreaterThenEquals(ScoreInfoArray[0].lScore, (SCOREEX)0)) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

                //计算税收
				ScoreInfoArray[0].lRevenue = m_dUserRevenue;
				
				if (D_GreaterThen(ScoreInfoArray[0].lRevenue, (SCOREEX)0)) { ScoreInfoArray[0].lScore -= ScoreInfoArray[0].lRevenue; }
				//流水
				//ScoreInfoArray[0].lChipSerialScore = abs(ScoreInfoArray[0].lScore);
                //AI不写库存
                if(!pIServerUserItem->IsAndroidUser())
                {
					if (D_LessThen(ScoreInfoArray[0].lScore + pIServerUserItem->GetUserScore(), (SCOREEX)0))
                    {
                        CString str;
                        WORD wTableID = m_pITableFrame->GetTableID();
                        str.Format(TEXT("【%d桌】 玩家【%d】【%s】金币不足,携带金币:%I64d,成绩:%I64d"), m_pITableFrame->GetTableID() + 1,  pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(), pIServerUserItem->GetUserScore(), ScoreInfoArray[0].lScore);
                        NcaTextOut(str, m_pGameServiceOption->szServerName);
                        ScoreInfoArray[0].lScore  = -pIServerUserItem->GetUserScore();
                    }
                }

                //写入积分
                m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));
				
				//更新税收库存
				m_gameStorage.m_gameStatisticsInfo.lTotalServiceTax += ScoreInfoArray[0].lRevenue;
				m_gameStorage.RefreshEvent(m_pITableFrame,nullptr);
            }

            m_bGameStart = false;
			m_bSuperRabbit = false;

            //发送数据
			if (pIServerUserItem->IsClientReady())
			{
				m_pITableFrame->SendTableData(wChairID, SUB_S_GAME_CONCLUDE);
			}
			
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN, ScoreInfoArray[0].lScore);

			//房间彩金
			CMD_S_Lottery Lottery;
			ZeroMemory(&Lottery, sizeof(Lottery));

			Lottery.dLotteryScore = m_gameStorage.m_gameReward.lCurStorage + m_gameStorage.m_gameReward.lVirtualStorage;

			if (pIServerUserItem->IsClientReady())
			{
				m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_GAME_LOTTERY, &Lottery, sizeof(Lottery));
			}
		
            //结束游戏
            m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
        }
        else
        {
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

            //结束游戏
            m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
        }

        return true;
    }
    case GER_DISMISS:		//游戏解散
    {
        m_bGameStart = false;

        //发送数据
        m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE);

		UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

        //结束游戏
        m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

        return true;
    }
    case GER_USER_LEAVE:	//用户强退
    case GER_NETWORK_ERROR:	//网络中断
    {
        if(m_bGameStart)
        {
            m_bGameStart = false;
            //积分变量
            tagScoreInfo ScoreInfoArray[GAME_PLAYER];
            ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

            if(pIServerUserItem != NULL)
            {
                //计算积分
                ScoreInfoArray[0].lScore = m_dUserBetTotalScore + m_dUserTotalGameScore;
				ScoreInfoArray[0].cbType = (D_GreaterThenEquals(ScoreInfoArray[0].lScore, (SCOREEX)0)) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

                //计算税收
                ScoreInfoArray[0].lRevenue = m_dUserRevenue;
				if (D_GreaterThen(ScoreInfoArray[0].lRevenue, (SCOREEX)0)) { ScoreInfoArray[0].lScore -= ScoreInfoArray[0].lRevenue; }

                //AI不写库存
                if(!pIServerUserItem->IsAndroidUser())
                {
					if (D_LessThen(ScoreInfoArray[0].lScore + pIServerUserItem->GetUserScore(), (SCOREEX)0))
                    {
                        CString str;
                        WORD wTableID = m_pITableFrame->GetTableID();
                        str.Format(TEXT("【%d桌】 玩家%s[%ld] 金币不足,携带金币:%I64d,成绩:%I64d"), m_pITableFrame->GetTableID() + 1, pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), pIServerUserItem->GetUserScore(), ScoreInfoArray[0].lScore);
                        NcaTextOut(str, m_pGameServiceOption->szServerName);
                        ScoreInfoArray[0].lScore  = -pIServerUserItem->GetUserScore();
                    }
                }
                //写入积分
                m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));
				//
				m_gameStorage.m_gameStatisticsInfo.lTotalServiceTax += ScoreInfoArray[0].lRevenue;
				m_gameStorage.RefreshEvent(m_pITableFrame,nullptr);
            }

			UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP, ScoreInfoArray[0].lScore);

            //结束游戏
            m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
        }
        else
        {
			UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

            //结束游戏
            m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
        }

        return true;
    }
    }

    //错误断言
    ASSERT(FALSE);

    return NcaTextOut(__LINE__, m_pITableFrame->GetTableUserItem(wChairID));
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
    switch(cbGameStatus)
    {
    case GAME_SCENE_FREE:	//空闲状态
    case GAME_SCENE_WIAT:	//等待状态
    {
        //构造数据
        CMD_S_StatusFree StatusFree;
        ZeroMemory(&StatusFree, sizeof(StatusFree));

        StatusFree.lCellScore = m_pITableFrame->GetCellScore();
		StatusFree.dLotteryScore = m_gameStorage.m_gameReward.lCurStorage + m_gameStorage.m_gameReward.lVirtualStorage;

        //下注值
        StatusFree.cbBetCount = m_cbBetCount;
        CopyMemory(StatusFree.dBetScore, m_dBetScore, sizeof(StatusFree.dBetScore));

		//更新房间用户信息
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

        //发送场景
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
    }
    case GAME_SCENE_PLAY:	//游戏状态
    {
        //构造数据
        CMD_S_StatusPlay StatusPlay;
        ZeroMemory(&StatusPlay, sizeof(StatusPlay));

		StatusPlay.lCellScore = m_pITableFrame->GetCellScore();
        StatusPlay.cbLineCount = m_cbLineCount;
        StatusPlay.cbBetIndex = m_cbBetIndex;
        StatusPlay.nGiveAwayCount = m_nGiveAwayCount;
        StatusPlay.dTotalGameScore = m_dUserTotalGameScore;
		StatusPlay.dLotteryScore = m_gameStorage.m_gameReward.lCurStorage + m_gameStorage.m_gameReward.lVirtualStorage;

		//拷贝数据
		CopyMemory(StatusPlay.dLineScore, m_dLineScore, sizeof(m_dLineScore));
		CopyMemory(StatusPlay.cbItemType, m_cbItemType, sizeof(m_cbItemType));
		StatusPlay.nSuperRabbit = m_nSuperRabbit;
		StatusPlay.dSuperRabbitScore = m_dSuperRabbitScore;

        //下注值
        StatusPlay.cbBetCount = m_cbBetCount;
        //CopyMemory(StatusPlay.dBetScore, m_dBetScore, sizeof(StatusPlay.dBetScore));
		for (int i = 0; i < sizeof(m_dBetScore) / sizeof(m_dBetScore[0]); ++i)
			StatusPlay.dBetScore[i] = m_dBetScore[i];

		//更新房间用户信息
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

        //发送场景
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
    }
    }

    //错误断言
    ASSERT(FALSE);

    return NcaTextOut(__LINE__, m_pITableFrame->GetTableUserItem(wChairID));
}

//时间事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
    if(IDI_ROOM_LOTTERY == wTimerID)
    {
		//房间彩金
		CMD_S_Lottery Lottery;
		ZeroMemory(&Lottery, sizeof(Lottery));

		Lottery.dLotteryScore = m_gameStorage.m_gameReward.lCurStorage + m_gameStorage.m_gameReward.lVirtualStorage;
		m_pITableFrame->SendUserItemData(NULL, SUB_S_GAME_LOTTERY, &Lottery, sizeof(Lottery));

        return true;
    }

    return false;
}

//数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID *pData, WORD wDataSize)
{
    return false;
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    return false;
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    switch(wSubCmdID)
    {
    case SUB_C_GAME_START:					//押线开始
    {
        //效验数据
        ASSERT(wDataSize == sizeof(CMD_C_GameStart));
        if(wDataSize != sizeof(CMD_C_GameStart)) { return NcaTextOut(__LINE__, pIServerUserItem); }

        //状态效验
        ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_WIAT);
        if(m_pITableFrame->GetGameStatus() != GAME_SCENE_WIAT) { return NcaTextOut(__LINE__, pIServerUserItem); }

        //用户效验
        ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
        if(pIServerUserItem->GetUserStatus() != US_PLAYING) { return NcaTextOut(__LINE__, pIServerUserItem); }

        //变量定义
        CMD_C_GameStart *pOneStart = (CMD_C_GameStart *)pData;

        return OnUserGameStart(pIServerUserItem, pOneStart->cbBetIndex, pOneStart->cbLineCount);
    }
    case SUB_C_GIVE_AWAY_START:					//押线开始
    {
        //效验数据
        ASSERT(wDataSize == 0);
        if(wDataSize != 0) { return NcaTextOut(__LINE__, pIServerUserItem); }

        //状态效验
        ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY);
        if(m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY) { return NcaTextOut(__LINE__, pIServerUserItem); }

        //用户效验
        ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
        if(pIServerUserItem->GetUserStatus() != US_PLAYING) { return NcaTextOut(__LINE__, pIServerUserItem); }

        return OnUserGiveAwayStart(pIServerUserItem, true);
    }
    case SUB_C_GAME_CONCLUDE:					//结束押线
    {
        //状态效验
        ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY || m_pITableFrame->GetGameStatus() == GAME_SCENE_WIAT);
        if(m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY && m_pITableFrame->GetGameStatus() != GAME_SCENE_WIAT) { return true; }

        //用户效验
        ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
        if(pIServerUserItem->GetUserStatus() != US_PLAYING) { return true; }

        return OnUserGameConclude(pIServerUserItem);
    }
	case SUB_C_SUPERRABBIT_START:
	{
		//状态效验
		ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY || m_pITableFrame->GetGameStatus() == GAME_SCENE_WIAT);
		if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY && m_pITableFrame->GetGameStatus() != GAME_SCENE_WIAT) { return true; }

		//用户效验
		ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
		if (pIServerUserItem->GetUserStatus() != US_PLAYING) { return true; }

		if (m_nSuperRabbit == 0)
		{
			return true;
		}

		CMD_S_SupperRabbitStart SupperRabbitStart;
		ZeroMemory(&SupperRabbitStart, sizeof(SupperRabbitStart));

		SupperRabbitStart.nSuperRabbit = m_nSuperRabbit;
		SupperRabbitStart.dSuperRabbitScore = m_dSuperRabbitScore;

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SUPERRABBIT_START, &SupperRabbitStart, sizeof(SupperRabbitStart));

		return true;
	}
	case SUB_C_SUPERRABBIT_END:
	{
		//状态效验
		ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY || m_pITableFrame->GetGameStatus() == GAME_SCENE_WIAT);
		if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY && m_pITableFrame->GetGameStatus() != GAME_SCENE_WIAT) { return true; }

		//用户效验
		ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
		if (pIServerUserItem->GetUserStatus() != US_PLAYING) { return true; }

		if (m_nSuperRabbit == 0)
		{
			return true;
		}

		//变量定义
		CMD_C_SuperRabbitEnd *pSuperRabbitEnd = (CMD_C_SuperRabbitEnd *)pData;

		SCOREEX dWinLineScore = m_dSuperRabbitScore / m_nSuperRabbit;
		m_nSuperRabbit = pSuperRabbitEnd->nCurSuperRabbit;
		m_nSuperRabbit += 1;
		m_dSuperRabbitScore = dWinLineScore * m_nSuperRabbit;
	
		CMD_S_SupperRabbitEnd SupperRabbitEnd;
		ZeroMemory(&SupperRabbitEnd, sizeof(SupperRabbitEnd));

		SupperRabbitEnd.nCurSuperRabbit = m_nSuperRabbit;
		SupperRabbitEnd.dCurSuperRabbitScore = m_dSuperRabbitScore;

		m_bSuperRabbit = true;

		//小游戏扣彩金池
		m_gameStorage.AddStorage(EventGameReward, -m_dSuperRabbitScore);
		//税收计算
		if (m_dSuperRabbitScore > 0)
			m_dUserRevenue += m_pITableFrame->CalculateRevenue(0, m_dSuperRabbitScore);
		m_gameStorage.RefreshEvent(m_pITableFrame, nullptr);

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SUPERRABBIT_END, &SupperRabbitEnd, sizeof(SupperRabbitEnd));

		return true;
	}
    }

    return NcaTextOut(__LINE__, pIServerUserItem);
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    // 消息处理
    if(wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
    {
        // 判断权限
        if(!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
        {
            return false;
        }

		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
		case SUB_C_EVENT_UPDATE:
			return OnDebugEvent(pData, wDataSize, pIServerUserItem);
		default:
			break;
		}
    }
    return false;
}

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
{
    if(m_pITableFrame->GetGameStatus() != GAME_SCENE_FREE)
    {
        OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);
    }
    return true;
}

//用户重入
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem *pIServerUserItem)
{
    return true;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //历史积分
    if(bLookonUser == false)
    {
        ASSERT(wChairID != INVALID_CHAIR);

        //初始用户信息
        InitializationUserInfo();

        if(pIServerUserItem->IsAndroidUser() == false)
        {
            tagUserDebug UserDebug;
            ZeroMemory(&UserDebug, sizeof(UserDebug));

            if(m_pGameInfoWR->ReadInfo(m_pGameServiceOption->wServerID, pIServerUserItem->GetUserID(), &UserDebug, sizeof(UserDebug)))
            {
                if(UserDebug.nGiveAwayCount > 0)
                {
                    m_cbLineCount = UserDebug.cbLineCount;
                    m_nGiveAwayCount = UserDebug.nGiveAwayCount;
                    m_cbBetIndex = UserDebug.cbBetIndex;
                    m_dUserBetScore = UserDebug.dBetScore;

                    //索引超过当前配置
                    if(m_cbBetIndex >= m_cbBetCount)
                    {
                        m_cbBetIndex = m_cbBetCount - 1;
                    }

                    //下注分超过上次赠送的下注分
					if (D_GreaterThen(m_dUserBetScore, m_pITableFrame->GetCellScore() * m_dBetScore[m_cbBetIndex]))
                    {
                        m_dUserBetScore = m_pITableFrame->GetCellScore() * m_dBetScore[m_cbBetIndex];
                    }

                    m_bGameStart = true;

                    // 强制设置成游戏状态
                    m_pITableFrame->StartGame();
                }
            }
        }

		//更新房间用户信息
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
    }

    return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //历史积分
    if(bLookonUser == false)
    {
        ASSERT(wChairID != INVALID_CHAIR);

        tagUserDebug UserDebug;
		ZeroMemory(&UserDebug, sizeof(UserDebug));

        UserDebug.cbBetIndex = m_cbBetIndex;
        UserDebug.cbLineCount = m_cbLineCount;
        UserDebug.dBetScore = m_dUserBetScore;
        UserDebug.nGiveAwayCount = m_nGiveAwayCount;

        m_pGameInfoWR->WriteInfo(m_pGameServiceOption->wServerID, pIServerUserItem->GetUserID(), &UserDebug, sizeof(UserDebug));

        //初始用户信息
        InitializationUserInfo();

		//删除信息
		//m_mapRoomUserInfo.RemoveKey(pIServerUserItem->GetGameID());
    }

    return true;
}

//用户同意
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize)
{
    return true;
}

//押线开始
bool CTableFrameSink::OnUserGameStart(IServerUserItem *pIServerUserItem, BYTE cbBetIndex, BYTE cbLineCount)
{
    if(m_bGameStart) { return NcaTextOut(__LINE__, pIServerUserItem); }
    if(m_nGiveAwayCount > 0) { return NcaTextOut(__LINE__, pIServerUserItem); }
    if(cbBetIndex < 0 || cbBetIndex >= m_cbBetCount) { return NcaTextOut(__LINE__, pIServerUserItem); }
    if(cbLineCount < 1 || cbLineCount > MAX_LINE_COUNT) { return NcaTextOut(__LINE__, pIServerUserItem); }

	ASSERT(D_GreaterThenEquals(pIServerUserItem->GetUserScore(), m_pITableFrame->GetCellScore() * m_dBetScore[cbBetIndex] * cbLineCount));
	if (D_LessThen(pIServerUserItem->GetUserScore(), m_pITableFrame->GetCellScore() * m_dBetScore[cbBetIndex] * cbLineCount)) { return NcaTextOut(__LINE__, pIServerUserItem); }
	
    m_cbBetIndex = cbBetIndex;
    m_cbLineCount = cbLineCount;
    m_dUserBetScore = m_pITableFrame->GetCellScore() * m_dBetScore[m_cbBetIndex];
    m_dUserBetTotalScore = -m_dUserBetScore * m_cbLineCount;
    m_bGameStart = true;

    return OnUserGiveAwayStart(pIServerUserItem, false);
}

//押线开始
bool CTableFrameSink::OnUserGiveAwayStart(IServerUserItem *pIServerUserItem, bool bUserOperate)
{
	//bUserOperate 使用赠送次数标识
    if(!m_bGameStart) { return NcaTextOut(__LINE__, pIServerUserItem); }
    if(bUserOperate)
    {
        ASSERT(m_nGiveAwayCount > 0);
        if(m_nGiveAwayCount <= 0) { return NcaTextOut(__LINE__, pIServerUserItem); }
        m_nGiveAwayCount--;
    }

    //设置状态
    m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

	ZeroMemory(m_dLineScore, sizeof(m_dLineScore));
	memset(m_cbItemType, INVALID_ITEM, sizeof(m_cbItemType));
	m_nSuperRabbit = 0;
	m_dSuperRabbitScore = 0;
	m_bSuperRabbit = false;

	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	//中奖率固定百分之50
	//获取用户调试类型
	EM_CTRL_TYPE userCtrlType = GetUserCtrlType(pIServerUserItem);

    //混乱扑克
    SCOREEX dTotalGameScore = 0;
    int nGiveAwayCount = 0;
    int nRewardCount = 0;
    int nLineMultiple[MAX_LINE_COUNT] = {0};
	BYTE cbItemType[MAX_LINE_COUNT];
	memset(cbItemType, INVALID_ITEM, sizeof(cbItemType));
    BYTE cbLineCount[MAX_LINE_COUNT] = {0};

    CString strPath;
    GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
    strPath.ReleaseBuffer();
    strPath = strPath.Left(strPath.ReverseFind('\\'));
    CString strSaveFileName;
    strSaveFileName.Format(TEXT("%s\\ConfigCard\\SuperFruit.card"), strPath);
	CFile file;
    
	//配牌不算进任何调试
    if(file.Open(strSaveFileName, CFile::modeRead))
    {
        BYTE cbCardData[15] = {0};
        file.Read(&cbCardData, sizeof(cbCardData));
        file.Close();
        CopyMemory(m_cbItemInfo, cbCardData, sizeof(m_cbItemInfo));

        dTotalGameScore = GetZhongJiangScore(nLineMultiple, cbItemType, cbLineCount, nGiveAwayCount, nRewardCount);
    }
    else
    {
		RandCardData(pIServerUserItem,bUserOperate);
		dTotalGameScore = GetZhongJiangScore(nLineMultiple, cbItemType, cbLineCount, nGiveAwayCount, nRewardCount);
    }

	//累加免费次数
    m_nGiveAwayCount += nGiveAwayCount;

    //构造变量
    CMD_S_GameStart GameStart;
    ZeroMemory(&GameStart, sizeof(GameStart));

    GameStart.nGiveAwayCount = m_nGiveAwayCount;
    CopyMemory(GameStart.cbItemInfo, m_cbItemInfo, sizeof(GameStart.cbItemInfo));
	memset(GameStart.cbItemType, INVALID_ITEM, sizeof(GameStart.cbItemType));

	//压线得分
	SCOREEX dLineWinScore = 0;
	bool bCTSEVEN = false;

    for(int i = 0; i < MAX_LINE_COUNT; i++)
    {
        if(nLineMultiple[i] > 0)
        {
			GameStart.cbItemType[i] = cbItemType[i];

			if (cbItemType[i] == CT_ICECREAM)
            {
				//累加免费玩次数
				//m_nGiveAwayCount += nLineMultiple[i];
            }
			else if (cbItemType[i] == CT_SEVEN)
            {
				GameStart.dLineScore[i] = m_gameStorage.m_gameReward.lCurStorage * ((float)nLineMultiple[i] / 100);

				//派彩
				m_gameStorage.m_gameReward.lCurStorage -= GameStart.dLineScore[i];
				bCTSEVEN = true;
            }
			else if (cbItemType[i] == CT_COMPOTE)
			{
				GameStart.nSuperRabbit = nLineMultiple[i];

				//连续3个果盘后的线分
				BYTE cbFirstItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, 0);
				BYTE cbSecondItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, 1);
				BYTE cbLastItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, ITEM_X_COUNT - 1);

				ASSERT(cbFirstItemType != CT_COMPOTE);

				//只算从第一个点开始前4个点
				if (cbFirstItemType != cbLastItemType && cbLastItemType != CT_COMPOTE)
				{
					GameStart.dLineScore[i] = m_GameLogic.GetZhongJiangMultiple(cbFirstItemType, 4) * m_dUserBetScore;
				}
				//只算从第一个点开始前5个点
				else if (cbFirstItemType != cbLastItemType && cbLastItemType == CT_COMPOTE)
				{
					GameStart.dLineScore[i] = m_GameLogic.GetZhongJiangMultiple(cbFirstItemType, 5) * m_dUserBetScore;
				}
				//只算从第二个点开始前4个点
				else if (cbFirstItemType != cbSecondItemType && cbSecondItemType != CT_COMPOTE)
				{
					GameStart.dLineScore[i] = m_GameLogic.GetZhongJiangMultiple(cbSecondItemType, 4) * m_dUserBetScore;
				}

				dLineWinScore += GameStart.dLineScore[i];

			}
			else
			{
				GameStart.dLineScore[i] = m_dUserBetScore * nLineMultiple[i];

				dLineWinScore += GameStart.dLineScore[i];
			}
        }
    }

	//一局中不能又中兔子又有彩金
	if (GameStart.nSuperRabbit > 0)
	{
		for (int i = 0; i < MAX_LINE_COUNT; i++)
		{
			if (cbItemType[i] == CT_SEVEN)
			{
				GameStart.dLineScore[i] = 0;
				GameStart.cbItemType[i] = INVALID_ITEM;
			}
		}
	}

	GameStart.dSuperRabbitScore = dLineWinScore * GameStart.nSuperRabbit;

	CString strDebug;
	strDebug.Format(TEXT("m_dUserBetTotalScore = %.2lf,dLineWinScore = %.2lf,dTotalGameScore = %.2lf"), m_dUserBetTotalScore, dLineWinScore, dTotalGameScore);
	//CTraceService::TraceString(strDebug, TraceLevel_Exception);

	m_dUserTotalGameScore += dTotalGameScore;

    //发送数据
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

	//拷贝数据
	CopyMemory(m_dLineScore, GameStart.dLineScore, sizeof(m_dLineScore));
	CopyMemory(m_cbItemType, GameStart.cbItemType, sizeof(m_cbItemType));
	m_nSuperRabbit = GameStart.nSuperRabbit;
	m_dSuperRabbitScore = GameStart.dSuperRabbitScore;

	if (bCTSEVEN)
	{
		//房间彩金
		CMD_S_Lottery Lottery;
		ZeroMemory(&Lottery, sizeof(Lottery));

		Lottery.dLotteryScore = m_gameStorage.m_gameReward.lCurStorage + m_gameStorage.m_gameReward.lVirtualStorage;
		m_pITableFrame->SendUserItemData(NULL, SUB_S_GAME_LOTTERY, &Lottery, sizeof(Lottery));
	}

	CString str(TEXT("("));
	for (int i = 0; i < MAX_LINE_COUNT; i++)
		str.AppendFormat(TEXT("%.2lf "), GameStart.dLineScore[i]);
	str.AppendFormat(TEXT("]"));
	LOG_DEBUG(TEXT("各线得分:"), str, TEXT("当前免费次数:"), m_nGiveAwayCount, TEXT("超级兔子:"), m_nSuperRabbit);

	
	SCOREEX dBetTxChange = 0;
	SCOREEX dRewardChange = 0;
	SCOREEX dTmpBetScore = 0;
	if (!bUserOperate)
	{
		dTmpBetScore = m_dUserBetTotalScore;
		m_dUserRevenue = 0;
	}

	//更新库存
	m_gameStorage.LineGameUpdateStorage(-dTmpBetScore, dTotalGameScore, pIServerUserItem->GetGameID());
	
	//税收计算
	if (dTotalGameScore + dTmpBetScore > 0)
		m_dUserRevenue += m_pITableFrame->CalculateRevenue(0, dTotalGameScore + dTmpBetScore);

	m_gameStorage.RefreshEvent(m_pITableFrame, nullptr);
    return true;
}

//游戏结束
bool CTableFrameSink::OnUserGameConclude(IServerUserItem *pIServerUserItem)
{
    OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);

    return true;
}
//获取开奖索引
int CTableFrameSink::GetRandIndex(LONGLONG lMinTimes, LONGLONG lMaxTimes)
{
	//打乱倍数表
	int nRandIndex[IMAGE_COUNT * 3];
	for (int i = 0; i<IMAGE_COUNT * 3; ++i)
		nRandIndex[i] = i;

	int nMaxRandIndex = IMAGE_COUNT * 3 - 1;
	int nCurRandIndex = 0;
	while (true)
	{
		nCurRandIndex = rand() % nMaxRandIndex;
		int tmpValue = nRandIndex[nMaxRandIndex];
		nRandIndex[nMaxRandIndex] = nRandIndex[nCurRandIndex];
		nRandIndex[nCurRandIndex] = tmpValue;
		if (--nMaxRandIndex <= 0)
			break;
	}

	//获取倍数
	for (int i = 0; i<IMAGE_COUNT * 3; ++i)
	{
		int tmpIndex = nRandIndex[i];
		int nTmpRatio = m_GameLogic.GetZhongJiangMultiple(tmpIndex / 3, 3 + tmpIndex % 3);
		if (nTmpRatio > lMinTimes && nTmpRatio <= lMaxTimes)
		{
			return tmpIndex;
		}
	}

	return IMAGE_COUNT * 3;
};
//随机生成数据
LONGLONG CTableFrameSink::RandCardData(IServerUserItem *pIServerUserItem, bool bGiveAway)
{
	BYTE cbLineType[MAX_LINE_COUNT];
	for (WORD i = 0; i < MAX_LINE_COUNT; i++)
	{
		cbLineType[i] = INVALID_ITEM;
	}

    BYTE cbLineCount[MAX_LINE_COUNT] = {0};
	BYTE cbOpenLineType[MAX_LINE_COUNT];
	ZeroMemory(cbOpenLineType, sizeof(cbOpenLineType));
	
	int lCirCount = 0;
	LONG MaxCirCount = 0;
	BYTE cbRandVal = rand() % 100;
	int nWinChance = m_gameStorage.GetUserWinLoseRatio(pIServerUserItem->GetGameID());
	bool bUserWin = cbRandVal < nWinChance ? true : false;

	SCOREEX dUserBetTotadScore = m_dUserBetTotalScore;
	SCOREEX dCurPlayerStorage = m_gameStorage.GetCurPlayerStorage(pIServerUserItem->GetGameID());

	//中奖倍率权重
	LONGLONG lCurMinTimes = -1, lCurMaxTimes = -1;
	if (bUserWin)
	{
		lCurMinTimes = MAX_LINE_COUNT;
		lCurMaxTimes = MAX_LONGLONG;
	}
	else
	{
		lCurMinTimes = 0;
		lCurMaxTimes = MAX_LINE_COUNT;
	}
	m_gameStorage.m_gameWieght.GetRandLimitTimes(lCurMinTimes, lCurMaxTimes);

	LOG_DEBUG(L"控制玩家:", bUserWin ? L"赢" : L"输", L"控制概率:", nWinChance, L"最小倍数:", lCurMinTimes, L"最大倍数:", lCurMaxTimes);

	//优先级:派彩>小游戏>免费>权重
	bool bFreeGame = rand() % 100 < m_gameStorage.m_gameExtraInfo.lFreeGameRatio ? true : false;
	bool bExtraGame = rand() % 100 < m_gameStorage.m_gameExtraInfo.lExtraGameRatio ? true : false;
	bool bRewardPool = rand() % 100 < m_gameStorage.m_gameReward.lDispatchRatio ? true : false;
	//小游戏受控概率必须满足派彩起点
	if (!D_GreaterThen(m_gameStorage.m_gameReward.lCurStorage, m_gameStorage.m_gameReward.lDispatchStorage))
	{
		bExtraGame = false;
		bRewardPool = false;
	}
	//各个图标的分数是以进入BONUS时玩家的总押注作为基础，最大爆池20倍,固定3次机会
	if (!(D_GreaterThen(m_gameStorage.m_gameReward.lCurStorage + m_dUserBetTotalScore * 20 * 3, (SCOREEX)0)))
	{
		LOG_DEBUG(L"彩金池不足，不开小游戏!");
		bExtraGame = false;
	}
	if (!bUserWin)
	{
		bFreeGame = false;
		bExtraGame = false;
		bRewardPool = false;
	}

	while (true)
	{
		bool bSpecialType = false;
		//随机生成中奖
		ZeroMemory(m_cbItemInfo, sizeof(m_cbItemInfo));
		ZeroMemory(cbLineCount, sizeof(cbLineCount));
		for (WORD i = 0; i < MAX_LINE_COUNT; i++)
		{
			cbLineType[i] = INVALID_ITEM;
		}

		int nZhongJianCount = 0;
		lCirCount++;
		MaxCirCount++;

		int  nCurOpenRatio = static_cast<int>(lCurMaxTimes);
		for (int i = 0; i < MAX_LINE_COUNT; i++)
		{
			int nRandIndex = IMAGE_COUNT * 3;
			BYTE nRandType = IMAGE_COUNT;
			cbLineType[nZhongJianCount] = INVALID_ITEM;

			//免费游戏
			if (bFreeGame && !bGiveAway)
			{
				bSpecialType = true;
				bFreeGame ? nRandType = CT_ICECREAM : 0;
			}
			//小游戏
			else if (bExtraGame)
			{
				bSpecialType = true;
				bExtraGame ? nRandType = CT_COMPOTE : 0;
			}
			//奖池
			else if (bRewardPool)
			{
				bSpecialType = true;
				bRewardPool ? nRandType = CT_SEVEN : 0;
			}
			else
			{
				//nRandIndex = GetRandIndex(lCurMinTimes, nCurOpenRatio);
				//if (nRandIndex >= IMAGE_COUNT * 3)
				nRandIndex = GetRandIndex(0, nCurOpenRatio);
				nRandType = nRandIndex / 3;
			}

			if (!bFreeGame && nRandType == CT_ICECREAM) continue;
			else if (!bExtraGame && nRandType == CT_COMPOTE) continue;
			//else if (!bRewardPool && nRandType == CT_SEVEN) continue;
			if (nRandType >= IMAGE_COUNT) continue;
			//有小游戏不开奖池
			if ((bExtraGame || cbOpenLineType[CT_COMPOTE]) && nRandType == CT_SEVEN) continue;

			//中奖啦
			cbLineType[nZhongJianCount] = nRandType;
			BYTE cbTempLineCount = 3 + (nRandIndex % 3);

			//分析可以派彩的条件
			if (cbLineType[nZhongJianCount] == CT_SEVEN)
			{
				//派彩的条件
				if (!(D_GreaterThenEquals(m_gameStorage.m_gameReward.lCurStorage, m_gameStorage.m_gameReward.lDispatchStorage)))
				{
					continue;
				}
			}
			//分析贪吃兔子（从一般奖池出）
			else if (cbLineType[nZhongJianCount] == CT_COMPOTE)
			{
				int nMultiple = m_GameLogic.GetZhongJiangMultiple(CT_COMPOTE, cbTempLineCount);

				if (!(D_GreaterThenEquals(m_gameStorage.m_gameReward.lCurStorage, (-dUserBetTotadScore) * 20 * nMultiple)))
				{
					LOG_WARN(L"库存不足以开出果盘大奖!");
					bExtraGame = false;
					continue;
				}
			}
			//圣代免费次数
			else if (cbLineType[nZhongJianCount] == CT_ICECREAM)
			{
				int nGiveAwayCount = m_GameLogic.GetZhongJiangMultiple(CT_ICECREAM, cbTempLineCount);

				if (m_nGiveAwayCount + nGiveAwayCount >= 4)
				{
					continue;
				}
			}
			else
			{
				//玩家库存防负
				int nMultiple = m_GameLogic.GetZhongJiangMultiple(cbLineType[nZhongJianCount], cbTempLineCount);
				if (!(D_GreaterThenEquals(dCurPlayerStorage, (-dUserBetTotadScore) * nMultiple)))
				{
					continue;
				}
				nCurOpenRatio -= nMultiple;
			}

			cbLineCount[nZhongJianCount++] = cbTempLineCount;

			cbOpenLineType[nRandType] = true;

			if (lCurMaxTimes - nCurOpenRatio > lCurMinTimes)
				break;
		}

		//生成数据
		m_GameLogic.RandCardAll(m_cbItemInfo, cbLineType, cbLineCount);

		SCOREEX dTotalGameScore = 0;
		int nGiveAwayCount = 0;
		int nRewardCount = 0;
		int nLineMultiple[MAX_LINE_COUNT] = { 0 };
		BYTE cbItemType[MAX_LINE_COUNT];
		memset(cbItemType, INVALID_ITEM, sizeof(cbItemType));
		ZeroMemory(cbLineCount, sizeof(cbLineCount));
		dTotalGameScore = GetZhongJiangScore(nLineMultiple, cbItemType, cbLineCount, nGiveAwayCount, nRewardCount);

#define MAX_CIRCOUNT	1000
		if (lCirCount >= MAX_CIRCOUNT)
		{
			//以中奖率为主，权重如果找不到,获取下一个权重配置
			if (bUserWin && MaxCirCount<MAX_CIRCOUNT * 10)
			{
				lCurMaxTimes = MAX_LINE_COUNT;
				lCurMaxTimes = MAX_LONGLONG;
				m_gameStorage.m_gameWieght.GetRandLimitTimes(lCurMinTimes, lCurMaxTimes);
				lCirCount = 0;
				continue;
			}
			LOG_DEBUG(TEXT("超过最大随机次数！"));
			ZeroMemory(m_cbItemInfo, sizeof(m_cbItemInfo));
			for (WORD i = 0; i < MAX_LINE_COUNT; i++)
			{
				if (cbLineType[i] == CT_SEVEN || cbLineType[i] == CT_COMPOTE || cbLineType[i] == CT_ICECREAM)
					continue;

				cbLineType[i] = INVALID_ITEM;
				cbLineCount[i] = 0;
			}
			//生成数据
			m_GameLogic.RandCardAll(m_cbItemInfo, cbLineType, cbLineCount);
			break;
		}

		nCurOpenRatio = 0;
		for (int i = 0; i<MAX_LINE_COUNT; i++)
			nCurOpenRatio += nLineMultiple[i];

		if (nCurOpenRatio > lCurMinTimes && nCurOpenRatio <= lCurMaxTimes && !bUserWin && D_UnGreaterThen(dUserBetTotadScore + dTotalGameScore, 0))
		{
			break;
		}
		else if ((nCurOpenRatio > lCurMinTimes && nCurOpenRatio <= lCurMaxTimes || bSpecialType) && bUserWin && D_UnLessThen(dUserBetTotadScore + dTotalGameScore, 0))
		{
			break;
		}
	}

    return 0;
}

//中奖分数
SCOREEX CTableFrameSink::GetZhongJiangScore(int nLineMultiple[MAX_LINE_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT], int &nGiveAwayCount, int &nRewardCount)
{
    SCOREEX dTotadScore = 0;
	int nSuperRabbit = 0;
    nGiveAwayCount = 0;
    nRewardCount = 0;
    ZeroMemory(nLineMultiple, sizeof(int) * MAX_LINE_COUNT);
    ZeroMemory(cbItemType, sizeof(BYTE) * MAX_LINE_COUNT);
    ZeroMemory(cbLineCount, sizeof(BYTE) * MAX_LINE_COUNT);

	bool bSuperRabbit = false;
    if(m_GameLogic.GetZhongJiangLineInfo(m_cbItemInfo, cbItemType, cbLineCount, m_cbLineCount))
    {
        for(int i = 0; i < m_cbLineCount; i++)
        {
            if(cbLineCount[i] > 0)
            {
                nLineMultiple[i] = m_GameLogic.GetZhongJiangMultiple(cbItemType[i], cbLineCount[i]);
				if (cbItemType[i] == CT_COMPOTE)
				{
					bSuperRabbit = true;
					nSuperRabbit = nLineMultiple[i];

					//连续3个果盘后的线分
					BYTE cbFirstItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, 0);
					BYTE cbSecondItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, 1);
					BYTE cbLastItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, ITEM_X_COUNT - 1);

					ASSERT(cbFirstItemType != CT_COMPOTE);

					//只算从第一个点开始前4个点
					if (cbFirstItemType != cbLastItemType && cbLastItemType != CT_COMPOTE)
					{
						dTotadScore += m_GameLogic.GetZhongJiangMultiple(cbFirstItemType, 4) * m_dUserBetScore;
					}
					//只算从第一个点开始前5个点
					else if (cbFirstItemType != cbLastItemType && cbLastItemType == CT_COMPOTE)
					{
						dTotadScore += m_GameLogic.GetZhongJiangMultiple(cbFirstItemType, 5) * m_dUserBetScore;
					}
					//只算从第二个点开始前4个点
					else if (cbFirstItemType != cbSecondItemType && cbSecondItemType != CT_COMPOTE)
					{
						dTotadScore += m_GameLogic.GetZhongJiangMultiple(cbSecondItemType, 4) * m_dUserBetScore;
					}
				}
				else if (cbItemType[i] == CT_ICECREAM)
                {
                    nGiveAwayCount += nLineMultiple[i];
                }
				else if (cbItemType[i] == CT_SEVEN)
                {
					//彩金百分比
                    nRewardCount = __min(100, nRewardCount + nLineMultiple[i]);				
                }
                else
                {
                    dTotadScore += nLineMultiple[i] * m_dUserBetScore;
                }
            }
        }
    }

	//贪吃兔子结算得分为总得分*贪吃兔游戏中奖倍率
	if (bSuperRabbit)
	{
		return dTotadScore * nSuperRabbit;
	}

	return dTotadScore + (m_gameStorage.m_gameReward.lCurStorage * ((float)nRewardCount / 100));
}

void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
    //发送数据
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SET_BASESCORE, &lBaseScore, sizeof(lBaseScore));
}

//更新房间用户信息
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, EM_USER_ACTION emUserAction, SCOREEX dGameScore)
{
	if (pIServerUserItem->IsAndroidUser()) 
	{
		return;
	}

	//变量定义
	tagRoomUserInfo roomUserInfo;
	ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(roomUserInfo.szNickName, pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
	roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();
	roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//用户坐下和重连
	if (emUserAction == USER_SITDOWN || emUserAction == USER_RECONNECT)
	{
		roomUserInfo.wTableID = pIServerUserItem->GetTableID();
	}
	else if (emUserAction == USER_STANDUP || emUserAction == USER_OFFLINE)
	{
		roomUserInfo.wTableID = INVALID_TABLE;
	}

	//查找已存在的用户
	tagRoomUserInfo lastroomUserInfo;
	ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
	if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), lastroomUserInfo))
	{
		roomUserInfo.dGameScore = lastroomUserInfo.dGameScore + dGameScore;
		roomUserInfo.bDebugIndividual = lastroomUserInfo.bDebugIndividual;
		roomUserInfo.dIndividualCtrlSysStorage = lastroomUserInfo.dIndividualCtrlSysStorage;
		roomUserInfo.dIndividualCtrlPlayerStorage = lastroomUserInfo.dIndividualCtrlPlayerStorage;
		roomUserInfo.lIndividualCtrlParameterK = lastroomUserInfo.lIndividualCtrlParameterK;
	}

	m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
}

//初始用户信息
void CTableFrameSink::InitializationUserInfo()
{
    m_dUserBetScore = 0;
    m_dUserBetTotalScore = 0;
    m_dUserTotalGameScore = 0;
    m_nGiveAwayCount = 0;
    m_cbLineCount = MAX_LINE_COUNT;
    m_cbBetIndex = 0;
}

//获取玩家调试类型
EM_CTRL_TYPE CTableFrameSink::GetUserCtrlType(IServerUserItem *pIServerUserItem)
{
	//（优先级，个人>房间>系统调试）
	BaseCrontol*pCurCtrl = m_gameStorage.GetCurValidControl(pIServerUserItem->GetGameID());
	if (pCurCtrl != nullptr)
	{
		if (pCurCtrl->m_ctrlType == UserCtrl)
			return USER_CTRL;
		else if (pCurCtrl->m_ctrlType == RoomCtrl)
			return ROOM_CTRL;
	}

	return SYS_CTRL;
}

//获取通用概率
void CTableFrameSink::GetCommonMulti(LONGLONG lCommon, LONGLONG lMulti[], LONGLONG lLineChance[IMAGE_COUNT * 3 + 1])
{
	LONGLONG lTempChance = 0;
	for (WORD i = 0; i < IMAGE_COUNT * 3; i++)
	{
		if (lMulti[i] == 0)
		{
			lLineChance[i] = 0;
			continue;
		}

		lLineChance[i] = lCommon / lMulti[i];

		lTempChance += lLineChance[i];
	}

	//赋值不出的结果
	lLineChance[IMAGE_COUNT * 3] = lTempChance;
}

//////////////////////////////////////////////////////////////////////////////////
//调试控制相关
//
void CTableFrameSink::InitSystemStorage()
{
	BaseCrontol* pCtrl = m_gameStorage.GetCurTypeCtrl(SysCtrl);
	//是否已初始化
	if (pCtrl == nullptr)
	{
		tagCustomRule * pCustomConfig = (tagCustomRule *)m_pITableFrame->GetCustomRule();
		ASSERT(pCustomConfig);

		StorageInfo newInfo;
		newInfo.lCurSysStorage = newInfo.lConfigSysStorage = pCustomConfig->dConfigSysStorage;
		newInfo.lCurPlayerStorage = newInfo.lConfigPlayerStorage = pCustomConfig->dConfigPlayerStorage;
		newInfo.lCurParameterK = newInfo.lConfigParameterK = pCustomConfig->lConfigParameterK;
		m_gameStorage.AddControl(SysCtrl, newInfo);
		m_gameStorage.GetCurTypeCtrl(SysCtrl)->m_lStorageResetCount -= 1;;

		//彩金初始化
		m_gameStorage.m_gameReward.lCurStorage = m_gameStorage.m_gameReward.lConfigStorage = 0;
		m_gameStorage.m_gameReward.lDispatchStorage = (SCOREEX)pCustomConfig->dDispatchMosaicGoldCon;
		m_gameStorage.m_gameReward.lTaxRatio = pCustomConfig->lMosaicGoldShowPercent;
		m_gameStorage.m_gameStatisticsInfo.stGameConfig.lStorageMax = -1;
		//最小配置系统库存限制：最大单注*最多线数*游戏最小倍
		m_gameStorage.m_gameStatisticsInfo.stGameConfig.lStorageMin = m_pITableFrame->GetCellScore() * m_dBetScore[sizeof(m_dBetScore) / sizeof(m_dBetScore[0])-1] * MAX_LINE_COUNT * 5;

		//权重配置
		m_gameStorage.m_gameWieght.Add(0, 0, 240);
		m_gameStorage.m_gameWieght.Add(1, 5, 240);
		m_gameStorage.m_gameWieght.Add(5, 10, 240);
		m_gameStorage.m_gameWieght.Add(10, 15, 160);
		m_gameStorage.m_gameWieght.Add(15, 20, 120);
		m_gameStorage.m_gameWieght.Add(20, 30, 96);
		m_gameStorage.m_gameWieght.Add(30, 50, 80);
		m_gameStorage.m_gameWieght.Add(50, 75, 48);
		m_gameStorage.m_gameWieght.Add(75, 100, 32);
		m_gameStorage.m_gameWieght.Add(100, 150, 24);
		m_gameStorage.m_gameWieght.Add(150, 200, 16);
		m_gameStorage.m_gameWieght.Add(200, 400, 12);
		m_gameStorage.m_gameWieght.Add(400, 800, 6);
		m_gameStorage.m_gameWieght.Add(800, -1, 3);
	}
};
//事件处理
bool CTableFrameSink::OnDebugEvent(VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	//请求配置
	if (wDataSize == 0)
	{
		m_gameStorage.RefreshAll(m_pITableFrame,pIServerUserItem);
		return true;
	}

	if (wDataSize < sizeof(ControlUpdateEvent))
	{
		LOG_WARN(L"无效控制事件请求！");
		return false;
	}

	ControlUpdateEvent* pEvent = (ControlUpdateEvent*)pData;
	if (wDataSize != pEvent->nDataLen + sizeof(ControlUpdateEvent))
	{
		LOG_WARN(L"无效控制事件请求包长度！");
		return false;
	}
	switch (pEvent->emEventType)
	{
	case EventSysCtrl:			//系统调试更新
	case EventRoomCtrl:			//房间调试更新
	{
									if (pEvent->nDataLen != sizeof(StorageInfo))
									{
										LOG_WARN(L"无效控制请求数据包类型！");
										return false;
									}
									StorageInfo* pRequst = (StorageInfo*)(pEvent + 1);
									m_gameStorage.AddControl(pEvent->emEventType == EventSysCtrl ? SysCtrl : RoomCtrl, *pRequst);
	}break;
	case EventUserCtrl:			//玩家调试更新
	{
									if (pEvent->nDataLen != sizeof(GameDebugInfo))
									{
										LOG_WARN(L"无效控制请求数据包类型！");
										return false;
									}
									GameDebugInfo* pRequst = (GameDebugInfo*)(pEvent + 1);
									m_gameStorage.AddControl(UserCtrl, *pRequst);
	}break;
	case EventGameTax:			//游戏抽水更新
	{
									if (pEvent->nDataLen != sizeof(GameTaxInfo))
									{
										LOG_WARN(L"无效控制请求数据包类型！");
										return false;
									}
									GameTaxInfo* pRequst = (GameTaxInfo*)(pEvent + 1);
									m_gameStorage.AddControl(GameTax, *pRequst);
	}break;
	case EventGameReward:		//游戏彩金更新
	{
									if (pEvent->nDataLen != sizeof(GameRewardInfo))
									{
										LOG_WARN(L"无效控制请求数据包类型！");
										return false;
									}
									GameRewardInfo* pRequst = (GameRewardInfo*)(pEvent + 1);
									m_gameStorage.AddControl(GameReward, *pRequst);
	}break;
	case EventGameExtra:		//游戏杂项数据更新
	{
									if (pEvent->nDataLen != sizeof(GameExtraInfo))
									{
										LOG_WARN(L"无效控制请求数据包类型！");
										return false;
									}
									GameExtraInfo* pRequst = (GameExtraInfo*)(pEvent + 1);
									m_gameStorage.AddControl(GameExtra, *pRequst);
	}break;
	case EventGameWeightConfig:	//权重配置更新
	{
									int nSize = pEvent->nDataLen%sizeof(WeightConfig);
									if (nSize != 0)
									{
										LOG_WARN(L"无效控制请求数据包类型！");
										return false;
									}
									WeightConfig* pConfig = (WeightConfig*)(pEvent + 1);
									m_gameStorage.AddControl(GameWeight, (WeightConfig*)(pEvent + 1), static_cast<int>(pEvent->nDataLen / sizeof(WeightConfig)));
	}break;
	//case EventGameStatistics:	//游戏综合数据更新
	default:
		LOG_WARN(L"无效控制事件类型！");
		return false;
	}

	m_gameStorage.RefreshEvent(m_pITableFrame,pIServerUserItem);
	return true;
};
//////////////////////////////////////////////////////////////////////////////////
