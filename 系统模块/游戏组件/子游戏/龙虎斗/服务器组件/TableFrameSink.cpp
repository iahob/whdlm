#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <locale>
#include "Dump.h"

//////////////////////////////////////////////////////////////////////////////////

//房间名称
TCHAR g_szServerName[LEN_SERVER];


//下注时间
#define IDI_FREE					1									//空闲时间
#define IDI_PLACE_JETTON			2									//下注时间
#define IDI_GAME_END				3									//结束时间
#define TIME_FREE					1									//空闲时间
#define TIME_PLACE_JETTON			10									//下注时间
#define TIME_GAME_END				10									//结束时间

//通用全局变量
SCORE							g_lCurAnChouRate = 5;									//当前暗抽比例
SCORE							g_lCurMosgoldRate = 3;									//当前大奖抽取比例
SCORE							g_lCurMosgoldStorage = 0;								//当前大奖库存
SCORE							g_lCurMosgoldDispatchCon = 0;							//当前派奖起点
SCORE							g_lCurMosgoldDispatchRate = 30;							//当前派奖概率
SCORE							g_lCurMosgoldStorageRate = 30;							//大奖库存比例(每次重置把玩家库存对应比例放到大奖库存中)
SCORE							g_lCurAnChouTotal = 0;									//当前暗抽总数
SCORE							g_lStatisticSysWin = 0;									//统计(包括通用库存，房间区域调试中系统输赢和暗抽大奖)
SCORE							g_lStatisticService = 0;								//服务比例总数

//控制递增索引(调试ID)
DWORD							g_dwSysCtrlIncreaseIndex = 0;
DWORD							g_dwRoomCtrlIncreaseIndex = 0;
DWORD							g_dwAreaCtrlIncreaseIndex = 0;

CList<SYSCTRL, SYSCTRL&>  CTableFrameSink::m_listSysCtrl;
CList<ROOMCTRL, ROOMCTRL&> CTableFrameSink::m_listRoomCtrl;
CList<AREACTRL, AREACTRL&> CTableFrameSink::m_listAreaCtrl;

//////////////////////////////////////////////////////////////////////////
//构造函数
CTableFrameSink::CTableFrameSink()
{
	//下注变量
	ZeroMemory(m_lAreaInAllScore,sizeof(m_lAreaInAllScore));
	ZeroMemory(m_lUserInAllScore,sizeof(m_lUserInAllScore));
	ZeroMemory(m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));
	ZeroMemory(m_lUserStartScore, sizeof(m_lUserStartScore));
	m_lAreaLimitScore = 0;
	m_lUserLimitScore = 0;
	ZeroMemory(m_tagUserAreaInfo, sizeof(m_tagUserAreaInfo));

	//上局信息
	ZeroMemory(m_lLastAllJettonPlace, sizeof(m_lLastAllJettonPlace));
	ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

	//成绩	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));
	m_lBankerWinScore = 0;
	m_lCurTurnAnChouWin = 0;
    ZeroMemory(m_lRealAllJettonScore, sizeof(m_lRealAllJettonScore));

    m_lCurrentRobotJettonScore = 0;

	//扑克信息
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//庄家信息
	m_bEnableBanker = true;
	m_lApplyBankerCondition = 10000000;
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker = INVALID_CHAIR;
	m_wBankerTime = 0;
	m_lBankerWinScore = 0L;
	m_bEnableSysBanker = true;

    m_bEnableRobotOccupySeat = true;
    m_nRobotOccupySeatTable = 4;
    m_nRobotQuitSeatTurn = 10;
    ZeroMemory(m_lRobotOccupySeatPlayCount, sizeof(m_lRobotOccupySeatPlayCount));
	//庄家设置
	m_lBankerMAX = 0l;
	m_lBankerAdd = 0l;
	m_lBankerScoreMAX = 0l;
	m_lBankerScoreAdd = 0l;
	m_lPlayerBankerMAX = 0l;
	m_bExchangeBanker = true;
	m_bWaitGiveUpBanker = false;

	//占位
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;
	}
	ZeroMemory(m_bContinueNoBetCount, sizeof(m_bContinueNoBetCount));

	//机器人上庄
	m_nRobotRequestBankerCount = 4;
	m_lRobotLostScoreLimitQB = 100000;
	m_wRobotMaxSerialTurn = 10;
	m_lRobotBankerWinScore = 0;

	//机器人控制
	m_nMaxChipRobot = 0;
	m_nChipRobotCount = 0;
	m_lRobotAreaLimit = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	m_nRobotListMaxCount = 0;
    m_lLimitRobotJettonScore = 0;
	//时间设置
	m_dwJettonTime=0L;
	m_cbFreeTime = TIME_FREE;
	m_cbBetTime = TIME_PLACE_JETTON;
	m_cbEndTime = TIME_GAME_END;

	//记录变量
	m_lCurGameCount = 0;
	ZeroMemory(m_SimpleChartArray, sizeof(m_SimpleChartArray));
	ZeroMemory(m_AdvancedChartArray, sizeof(m_AdvancedChartArray));

	m_wChairCount = GAME_PLAYER;
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		m_listStatisticsRecord[wChairID].RemoveAll();
	}
	
	//组件变量
	m_hInst = NULL;
	m_pServerDebug = NULL;
	m_pITableFrame = NULL;
	m_hInst = LoadLibrary(TEXT("DragonTigerBattleServerDebug.dll"));
	if (m_hInst)
	{
		typedef void * (*CREATE)(); 
		CREATE ServerDebug = (CREATE)GetProcAddress(m_hInst,"CreateServerDebug"); 
		if ( ServerDebug )
		{
			m_pServerDebug = static_cast<IServerDebug*>(ServerDebug());
		}
	}

	srand(GetTickCount());

	m_listSysCtrl.RemoveAll();
	m_listRoomCtrl.RemoveAll();
	m_listAreaCtrl.RemoveAll();

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{


}

//释放对象
VOID  CTableFrameSink::Release()
{
	if( m_pServerDebug )
	{
		delete m_pServerDebug;
		m_pServerDebug = NULL;
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
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)WHCrashHandler);

	//查询接口
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	ASSERT(m_pITableFrame!=NULL);
	//错误判断
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"),TraceLevel_Exception);
		return false;
	}

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();

	ASSERT(m_pGameServiceOption!=NULL);
	ASSERT(m_pGameServiceAttrib!=NULL);

	//设置大联盟房间椅子数目
	if (IsGroupPersonalRoom())
	{
		m_wChairCount = GAME_PLAYER / m_pGameServiceOption->wTableCount;
		m_pITableFrame->SetTableChairCount(m_wChairCount);
	}

	ReadAIConfig();
	ReadConfigInformation(true);

	//开始记录时间
	m_tmStartRecord = CTime::GetCurrentTime();

	//配置筹码
	SCORE	dBetValueArray[MAX_AI_CHIP_COUNT];
	ZeroMemory(dBetValueArray, sizeof(dBetValueArray));

	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
	dBetValueArray[0] = pCustomConfig->CustomGeneral.lChipArray[0];
	dBetValueArray[1] = pCustomConfig->CustomGeneral.lChipArray[1];
	dBetValueArray[2] = pCustomConfig->CustomGeneral.lChipArray[2];
	dBetValueArray[3] = pCustomConfig->CustomGeneral.lChipArray[3];
	dBetValueArray[4] = pCustomConfig->CustomGeneral.lChipArray[MAX_CHIP_COUNT - 1];

	m_pITableFrame->SetBetValueArray(dBetValueArray);
	pCustomConfig->CustomAndroid.dMinTableScore = m_pGameServiceOption->lMinTableScore;

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//下注变量
	ZeroMemory(m_lAreaInAllScore, sizeof(m_lAreaInAllScore));
	ZeroMemory(m_lUserInAllScore, sizeof(m_lUserInAllScore));
	ZeroMemory(m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));
	ZeroMemory(m_tagUserAreaInfo, sizeof(m_tagUserAreaInfo));

	//成绩	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	m_lBankerWinScore = 0;
	m_lCurTurnAnChouWin = 0;
    m_lCurrentRobotJettonScore = 0;
	//扑克信息
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	//机器人变量
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	return;
}

//查询服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	for (int nAreaIndex = 0; nAreaIndex<AREA_MAX; nAreaIndex++)
	{
		if (m_lUserInAllScore[wChairID][nAreaIndex] > 0)
		{
			return true;
		}
	}

	if (m_wCurrentBanker != INVALID_CHAIR && wChairID == m_wCurrentBanker)
	{
		return true;
	}

	return false;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//发送库存消息
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		pIServerUserItem->SetUserStatus(US_READY, m_pITableFrame->GetTableID(), i);
	}

	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//CString strStorage;
	//CTime Time(CTime::GetCurrentTime());
	//strStorage.Format(TEXT(" 房间: %s | 时间: %d-%d-%d %d:%d:%d | 库存: %I64d \n"), m_pGameServiceOption->szServerName, Time.GetYear(), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_StorageStart);
	//WriteInfo(TEXT("百人龙虎斗库存记录.log"),strStorage);

	//获取庄家
	IServerUserItem *pIBankerServerUserItem = NULL;
	if (INVALID_CHAIR != m_wCurrentBanker)
	{
		pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		pIBankerServerUserItem->SetUserStatus(US_PLAYING, m_pITableFrame->GetTableID(), m_wCurrentBanker);
	}

	m_lBankerScore = 1000000000.00;

	//设置变量
	GameStart.wBankerUser = m_wCurrentBanker;
	GameStart.lBankerScore = m_lBankerScore;
	
	if (pIBankerServerUserItem != NULL)
	{
		GameStart.lBankerScore = pIBankerServerUserItem->GetUserScore();
		m_lBankerScore = GameStart.lBankerScore;
	}

	int nChipRobotCount = 0;
	for (int i = 0; i < m_wChairCount; i++)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
		{
			nChipRobotCount++;
		}
	}

	if (m_cbBetTime <= 0)
	{
		//获取自定义配置
		tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
		ASSERT(pCustomConfig);

		m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
		if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
	}

	GameStart.cbTimeLeave = m_cbBetTime;
	GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);

	//游戏玩家
	for (WORD wChairID = 0; wChairID < m_wChairCount; ++wChairID)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//设置积分
		GameStart.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore);
		
		m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	
	}

	CString stTrace;
	CTime Time(CTime::GetCurrentTime());
	stTrace.Format(TEXT("%d-%d %d:%d:%d |  GAME_STARTGAME_START = %d, m_lBankerScore=%I64d, nChipRobotCount=%d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), GameStart.cbTimeLeave, m_lBankerScore, GameStart.nChipRobotCount);
	//WriteInfo(TEXT("DragonTigerBattlePlace.log"), stTrace);

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
            SaveData();
			//占位玩家连续不下注次数
			for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
			{
				if (m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
				{
					continue;
				}
				
				SCORE lTotalBet = 0;
				for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; wAreaIndex++)
				{
					lTotalBet += m_lUserInAllScore[m_wOccupySeatChairIDArray[i]][wAreaIndex];
				}

				if (lTotalBet == 0)
				{
					m_bContinueNoBetCount[i]++;
				}
			}

			m_lBankerWinScore = GameOver();

			//机器人坐庄
			if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker) && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser())
			{
				m_lRobotBankerWinScore += m_lBankerWinScore;
			}

			bool bUserBet = false;
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				for (int nAreaIndex = 0; nAreaIndex<AREA_MAX; nAreaIndex++)
				{
					if (m_lUserInAllScore[i][nAreaIndex] > 0)
					{
						bUserBet = true;
						break;
					}
				}

				if (bUserBet)
				{
					break;
				}
			}


			if ((!IsGroupPersonalRoom()) || (IsGroupPersonalRoom() && bUserBet))
			{
				//庄家坐庄递增次数
				m_wBankerTime++;
			}

			//结束消息
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			if (m_cbEndTime <= 0)
			{
				//获取自定义配置
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
				if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;
			}

			GameEnd.cbTimeLeave = m_cbEndTime;
			GameEnd.wBankerTime = m_wBankerTime;
          
			//扑克信息
			CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			CopyMemory(GameEnd.lUserInAllScore, m_lUserInAllScore, sizeof(m_lUserInAllScore));
			CopyMemory(GameEnd.lUserWinScore, m_lUserWinScore, sizeof(m_lUserWinScore));
			CopyMemory(GameEnd.lUserAreaWinScore, m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));
			CopyMemory(GameEnd.lUserRevenue, m_lUserRevenue, sizeof(m_lUserRevenue));

			//发送积分
			bool bfirstRecordAndroid = false;
			bool bfirstRecordRealPlayer = false;

			for ( WORD wUserIndex = 0; wUserIndex < m_wChairCount; ++wUserIndex )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
				if ( pIServerUserItem == NULL ) continue;

				//设置成绩
				GameEnd.lUserScore=m_lUserWinScore[wUserIndex];
				GameEnd.lBankerWinScore = m_lBankerWinScore;
                //除去庄家和占座玩家和自己每个区域的下注
                for(WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; wAreaIndex++)
                {
                    SCORE lOtherUserInAllScore = 0;
                    SCORE lOtherUserAreaWinScore = 0;
                    for(WORD i = 0; i < m_wChairCount; i++)
                    {
                        if(!FilterUser(wUserIndex, i))
                        {
                            continue;
                        }

                        lOtherUserInAllScore += m_lUserInAllScore[i][wAreaIndex];
                        lOtherUserAreaWinScore += m_lUserAreaWinScore[i][wAreaIndex];

                    }

                    GameEnd.lOtherUserInAllScore[wAreaIndex] = lOtherUserInAllScore;
                    GameEnd.lOtherUserAreaWinScore[wAreaIndex] = lOtherUserAreaWinScore;
                }
				//发送消息					
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

				/////////////////
				CString strTrace;
				CTime Time(CTime::GetCurrentTime());

				if (pIServerUserItem->IsAndroidUser() && !bfirstRecordAndroid)
				{
					strTrace.Format(TEXT("%d:%d:%d, GAMEENDwUserIndex=%d, cbTimeLeave=%d,CardArray1=%d,CardArray2=%d,lUserScore=%I64d\n"), Time.GetHour(), Time.GetMinute(), Time.GetSecond(),
						wUserIndex, GameEnd.cbTimeLeave, GameEnd.cbTableCardArray[0], GameEnd.cbTableCardArray[1], GameEnd.lUserScore);
					WriteInfo(TEXT("DragonTigerBattleTrace.log"), strTrace);

					bfirstRecordAndroid = true;
				}
				else if (!pIServerUserItem->IsAndroidUser() && !bfirstRecordRealPlayer)
				{
					strTrace.Format(TEXT("%d:%d:%d, GAMEENDwUserIndex=%d, cbTimeLeave=%d,CardArray1=%d,CardArray2=%d,lUserScore=%I64d\n"), Time.GetHour(), Time.GetMinute(), Time.GetSecond(),
						wUserIndex, GameEnd.cbTimeLeave, GameEnd.cbTableCardArray[0], GameEnd.cbTableCardArray[1], GameEnd.lUserScore);
					WriteInfo(TEXT("DragonTigerBattleTrace.log"), strTrace);

					bfirstRecordRealPlayer = true;
				}

				///////////////////
			}
			
			//////////////////////////////////////////////////
			CString stTrace;
			CTime Time(CTime::GetCurrentTime());
			stTrace.Format(TEXT("%d-%d %d:%d:%d |  SEND_SUB_S_GAME_END = %d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_pITableFrame->GetGameStatus());
			WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);
			//////////////////////////////////////////////////

			//保存上局总注
			CopyMemory(m_lLastAllJettonPlace, m_lUserInAllScore, sizeof(m_lLastAllJettonPlace));

			//清空本局最后一注
			ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

			for (WORD wUserChairID = 0; wUserChairID < m_wChairCount; ++wUserChairID)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if (pIServerUserItem == NULL)
				{
					continue;
				}

				if (pIServerUserItem->IsAndroidUser())
				{
					continue;
				}

				//发送记录
				SendGameRecord(pIServerUserItem, m_pITableFrame->GetGameStatus());
			}

			//统计记录最近20局
			for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
				if (pIServerUserItem == NULL)
				{
					continue;
				}

				tagStatisticsRecord StatisticsRecord;
				ZeroMemory(&StatisticsRecord, sizeof(StatisticsRecord));

				StatisticsRecord.bBeBanker = (m_wCurrentBanker == wChairID ? true : false);

				if (!StatisticsRecord.bBeBanker)
				{
					for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
					{
						StatisticsRecord.lInAllScore += m_lUserInAllScore[wChairID][nAreaIndex];
					}
				}

				StatisticsRecord.lWinScore = m_lUserWinScore[wChairID];

				if (m_listStatisticsRecord[wChairID].GetSize() < STATISTICS_RECORD_COUNT)
				{
					m_listStatisticsRecord[wChairID].AddTail(StatisticsRecord);
				}
				else
				{
					m_listStatisticsRecord[wChairID].RemoveHead();
					m_listStatisticsRecord[wChairID].AddTail(StatisticsRecord);
				}
			}

			return true;
		}
	case GER_USER_LEAVE:		//玩家离开
		{
			//闲家判断
			if (m_wCurrentBanker != wChairID)
			{
				//下注逃跑返回下注
				if (m_pITableFrame->GetGameStatus() != GAME_SCENE_GAME_END)
				{
					for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex)
					{
						if (m_lUserInAllScore[wChairID][nAreaIndex] != 0)
						{
							CMD_S_PlaceJettonFail PlaceJettonFail;
							ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
							PlaceJettonFail.lJettonArea = nAreaIndex;
							PlaceJettonFail.lPlaceScore = m_lUserInAllScore[wChairID][nAreaIndex];
							PlaceJettonFail.wPlaceUser = wChairID;

							//游戏玩家
							for (WORD i = 0; i < m_wChairCount; ++i)
							{
								IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
								if (pIServerUserItem == NULL)
									continue;

								m_pITableFrame->SendTableData(i, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
							}

							m_lAreaInAllScore[nAreaIndex] -= m_lUserInAllScore[wChairID][nAreaIndex];
							m_lUserInAllScore[wChairID][nAreaIndex] = 0;
						}
					}
				}
				else
				{
					//积分变量
					tagScoreInfo ScoreInfoArray[GAME_PLAYER];
					ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

					//写入积分
					if (m_lUserWinScore[wChairID] != 0)
					{
						tagScoreInfo ScoreInfo[GAME_PLAYER];
						ZeroMemory(ScoreInfo, sizeof(ScoreInfo));
						ScoreInfo[wChairID].cbType = m_lUserWinScore[wChairID] > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;;
						ScoreInfo[wChairID].lScore = m_lUserWinScore[wChairID];
						ScoreInfo[wChairID].lRevenue = m_lUserRevenue[wChairID];
						m_lUserWinScore[wChairID] = 0;

						m_pITableFrame->WriteUserScore(wChairID, ScoreInfo[wChairID]);
					}
					for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex)
					{
						if (m_lUserInAllScore[wChairID][nAreaIndex] != 0)
						{
							m_lUserInAllScore[wChairID][nAreaIndex] = 0;
						}
					}

				}

				return true;
			}


			//状态判断
			if (m_pITableFrame->GetGameStatus() != GAME_SCENE_GAME_END)
			{
				//提示消息
				TCHAR szTipMsg[128];
				_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("由于庄家[ %s ]强退，游戏提前结束！"), pIServerUserItem->GetNickName());

				//发送消息
				SendGameMessage(INVALID_CHAIR, szTipMsg);

				//设置状态
				m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

				if (m_cbEndTime <= 0)
				{
					//获取自定义配置
					tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
					ASSERT(pCustomConfig);

					m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
					if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;
				}

				//设置时间
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_dwJettonTime = (DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_GAME_END, m_cbEndTime * 1000, 1, 0L);

				SCORE lBankerWinScore = GameOver();

				//////////////////////////////////////////////////
				CString stTrace;
				CTime Time(CTime::GetCurrentTime());
				stTrace.Format(TEXT("%d-%d %d:%d:%d |  USER_LEAVE SETIDI_GAME_END, gamestatus=%d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_pITableFrame->GetGameStatus());
				WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);
				//////////////////////////////////////////////////

				//结束消息
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd, sizeof(GameEnd));

				//庄家信息
				GameEnd.cbTimeLeave = m_cbEndTime;
				CopyMemory(GameEnd.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
				GameEnd.wBankerTime = m_wBankerTime;

				CopyMemory(GameEnd.lUserInAllScore, m_lUserInAllScore, sizeof(m_lUserInAllScore));
				CopyMemory(GameEnd.lUserWinScore, m_lUserWinScore, sizeof(m_lUserWinScore));
				CopyMemory(GameEnd.lUserAreaWinScore, m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));
				CopyMemory(GameEnd.lUserRevenue, m_lUserRevenue, sizeof(m_lUserRevenue));

				for (WORD wUserIndex = 0; wUserIndex < m_wChairCount; ++wUserIndex)
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
					if (pIServerUserItem == NULL) continue;

					//设置成绩
					GameEnd.lUserScore = m_lUserWinScore[wUserIndex];
					GameEnd.lBankerWinScore = m_lBankerWinScore;
                    //除去庄家和占座玩家和自己每个区域的下注
                    for(WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; wAreaIndex++)
                    {
                        SCORE lOtherUserInAllScore = 0;
						SCORE lOtherUserAreaWinScore = 0;
                        for(WORD i = 0; i < m_wChairCount; i++)
                        {
                            if(!FilterUser(wUserIndex, i))
                            {
                                continue;
                            }

                            lOtherUserInAllScore += m_lUserInAllScore[i][wAreaIndex];
                            lOtherUserAreaWinScore += m_lUserAreaWinScore[i][wAreaIndex];

                        }

                        GameEnd.lOtherUserInAllScore[wAreaIndex] = lOtherUserInAllScore;
                        GameEnd.lOtherUserAreaWinScore[wAreaIndex] = lOtherUserAreaWinScore;
                    }
					//发送消息					
					m_pITableFrame->SendTableData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
				}
			}

			//扣除分数
			if (m_lUserWinScore[m_wCurrentBanker] != 0)
			{
				tagScoreInfo ScoreInfo[GAME_PLAYER];
				ZeroMemory(ScoreInfo, sizeof(ScoreInfo));
				ScoreInfo[m_wCurrentBanker].cbType = SCORE_TYPE_FLEE;
				ScoreInfo[m_wCurrentBanker].lRevenue = m_lUserRevenue[m_wCurrentBanker];
				ScoreInfo[m_wCurrentBanker].lScore = m_lUserWinScore[m_wCurrentBanker];

				m_pITableFrame->WriteUserScore(m_wCurrentBanker, ScoreInfo[m_wCurrentBanker]);

				m_lUserWinScore[m_wCurrentBanker] = 0;
			}

			//切换庄家
			ChangeBanker(true);

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
		//空闲状态
	case GAME_SCENE_FREE:	
		{
			//获取自定义配置
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);

			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));			

			if (m_cbFreeTime <= 0)
			{
				//获取自定义配置
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				m_cbFreeTime = pCustomConfig->CustomGeneral.cbBetTime;
				if (m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
			}

			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(m_cbFreeTime-__min(dwPassTime,m_cbFreeTime));
			StatusFree.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore);
			StatusFree.lAreaLimitScore = m_lAreaLimitScore;
			CopyMemory(StatusFree.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusFree.lChipArray));

			//上庄及庄家
			StatusFree.bEnableBanker = m_bEnableBanker;
			StatusFree.wBankerUser = m_wCurrentBanker;
			StatusFree.wBankerTime = m_wBankerTime;
			StatusFree.lBankerWinScore = m_lBankerWinScore;
			StatusFree.lBankerScore = 0x7ffffffffff;
			if (m_wCurrentBanker != INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusFree.lBankerScore = pIServerUserItem->GetUserScore();
			}
			StatusFree.bEnableSysBanker = m_bEnableSysBanker;
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			
			//占位
			StatusFree.lOccupySeatFree = pCustomConfig->CustomGeneral.lOccupySeatFree;
			CopyMemory(StatusFree.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

			//机器人配置
			if (pIServerUserItem->IsAndroidUser())
			{
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);
				CopyMemory(&StatusFree.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));

				CopyMemory(&(StatusFree.CustomAndroid.CustomAIConfig), &m_CustomAIConfig, sizeof(m_CustomAIConfig));
			}

			StatusFree.bIsGroupPersonalRoom = IsGroupPersonalRoom();

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));

			//发送记录
			SendGameRecord(pIServerUserItem, cbGameStatus);

			//发送上庄列表
			SendApplyUser(pIServerUserItem);

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
			{
				//发送房间用户信息
				SendRoomUserInfo();
			}

			return bSuccess;
		}
		//叫分状态
	case GAME_SCENE_PLACE_JETTON:
	case GAME_SCENE_GAME_END:	
		{
			//获取自定义配置
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);

			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			if (m_cbBetTime <= 0)
			{
				//获取自定义配置
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
				if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
			}

			DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;
			StatusPlay.cbTimeLeave = (BYTE)(m_cbBetTime - __min(dwPassTime, m_cbBetTime));

			//状态判断
			if (cbGameStatus == GAME_SCENE_GAME_END)
			{
				StatusPlay.cbTimeLeave = (BYTE)(m_cbEndTime - __min(dwPassTime, m_cbEndTime));
			}
			StatusPlay.cbGameStatus = m_pITableFrame->GetGameStatus();

			//玩家下注
			if (pIServerUserItem->GetUserStatus() != US_LOOKON)
			{
				//最大下注
				StatusPlay.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore);
			}
			StatusPlay.lAreaLimitScore = m_lAreaLimitScore;
			CopyMemory(StatusPlay.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusPlay.lChipArray));

			//结束判断
			if (cbGameStatus == GAME_SCENE_GAME_END)
			{
				//扑克信息
				CopyMemory(StatusPlay.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
			}

			//下注
			memcpy(StatusPlay.lAreaInAllScore, m_lAreaInAllScore, sizeof(StatusPlay.lAreaInAllScore));
			memcpy(StatusPlay.lUserInAllScore, m_lUserInAllScore[wChairID], sizeof(StatusPlay.lUserInAllScore));

			//上庄及庄家
			StatusPlay.bEnableBanker = m_bEnableBanker;
			StatusPlay.wBankerUser = m_wCurrentBanker;
			StatusPlay.wBankerTime = m_wBankerTime;
			StatusPlay.lBankerScore = 0x7ffffffffff;
			if (m_wCurrentBanker != INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				if (pIServerUserItem)
				{
					StatusPlay.lBankerScore = pIServerUserItem->GetUserScore();
				}
			}
			StatusPlay.bEnableSysBanker = m_bEnableSysBanker;
			StatusPlay.lApplyBankerCondition = m_lApplyBankerCondition;

			//占位
			StatusPlay.lOccupySeatFree = pCustomConfig->CustomGeneral.lOccupySeatFree;
			CopyMemory(StatusPlay.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

			CopyMemory(StatusPlay.lUserAllScore, m_lUserInAllScore, sizeof(m_lUserInAllScore));

			//除去庄家和占座玩家和自己每个区域的下注
			for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; wAreaIndex++)
			{
                SCORE lOtherUserInAllScore = 0;
                SCORE lOtherUserAreaWinScore = 0;
				for (WORD i = 0; i < m_wChairCount; i++)
				{
					if (!FilterUser(wChairID, i))
					{
						continue;
					}

					lOtherUserInAllScore += m_lUserInAllScore[i][wAreaIndex];
					lOtherUserAreaWinScore += m_lUserAreaWinScore[i][wAreaIndex];

				}

				StatusPlay.lOtherUserInAllScore[wAreaIndex] = lOtherUserInAllScore;
				StatusPlay.lOtherUserAreaWinScore[wAreaIndex] = lOtherUserAreaWinScore;
			}

			//结束信息
			if (cbGameStatus == GAME_SCENE_GAME_END)
			{
				StatusPlay.lEndUserScore = m_lUserWinScore[wChairID];
				StatusPlay.lBankerWinScore = m_lBankerWinScore;

				CopyMemory(StatusPlay.lUserWinScore, m_lUserWinScore, sizeof(m_lUserWinScore));
				CopyMemory(StatusPlay.lUserAreaWinScore, m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));

				CopyMemory(StatusPlay.lUserRevenue, m_lUserRevenue, sizeof(m_lUserRevenue));
			}
			
			//机器人配置
			if(pIServerUserItem->IsAndroidUser())
			{
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				CopyMemory(&StatusPlay.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
				CopyMemory(&(StatusPlay.CustomAndroid.CustomAIConfig), &m_CustomAIConfig, sizeof(m_CustomAIConfig));
			}

			StatusPlay.bIsGroupPersonalRoom = IsGroupPersonalRoom();

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			//发送记录
			SendGameRecord(pIServerUserItem, cbGameStatus);

			//发送上庄列表
			SendApplyUser(pIServerUserItem);

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
			{
				//发送下注信息
				SendUserBetInfo(INVALID_DWORD);

				//发送房间用户信息
				SendRoomUserInfo();
			}

			return bSuccess;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//查询限额
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		return 0L;
	}
	else
	{		
		return max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore, 0L);
	}
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE:		//空闲时间
		{
			//开始游戏
			m_pITableFrame->StartGame();

			if (m_cbBetTime <= 0)
			{
				//获取自定义配置
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
				if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
			}

			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_cbBetTime*1000,1,0L);

			//设置状态
			m_pITableFrame->SetGameStatus(GAME_SCENE_PLACE_JETTON);

			m_tmStartPlaceJetton = CTime::GetCurrentTime();

			//////////////////////////////////////////////////
			CString stTrace;
			CTime Time(CTime::GetCurrentTime());
			stTrace.Format(TEXT("%d-%d %d:%d:%d |  SETIDI_PLACE_JETTON, gamestatus=%d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_pITableFrame->GetGameStatus());
			WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);
			//////////////////////////////////////////////////

			return true;
		}
	case IDI_PLACE_JETTON:		//下注时间
		{
			//状态判断(防止强退重复设置)
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_GAME_END)
			{
				//设置状态
				m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);			

				//结束游戏
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

				if (m_cbEndTime <= 0)
				{
					//获取自定义配置
					tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
					ASSERT(pCustomConfig);

					m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
					if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;
				}

				//设置时间
				m_dwJettonTime=(DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_cbEndTime*1000,1,0L);			

				//////////////////////////////////////////////////
				CString stTrace;
				CTime Time(CTime::GetCurrentTime());
				stTrace.Format(TEXT("%d-%d %d:%d:%d |  SETIDI_GAME_END, gamestatus=%d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_pITableFrame->GetGameStatus());
				WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);
				//////////////////////////////////////////////////
			}

			return true;
		}
	case IDI_GAME_END:			//结束游戏
		{
			//写入积分
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			for ( WORD wUserChairID = 0; wUserChairID < m_wChairCount; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL||(QueryBuckleServiceCharge(wUserChairID)==false)) continue;

				//胜利类型
				BYTE ScoreKind = m_lUserWinScore[wUserChairID] > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
				
				//写入积分
				ScoreInfo[wUserChairID].lScore = m_lUserWinScore[wUserChairID];
				ScoreInfo[wUserChairID].lRevenue = m_lUserRevenue[wUserChairID];
				ScoreInfo[wUserChairID].cbType = ScoreKind;

				//写入流水
				//庄家：总结算为负，则输的额度全部计入流水。总结算为正，则以税后总赢额度计入流水。 
				//闲家：未押中区域（总押注额度计入流水） + 押中区域（该区域税后总赢计入流水）

				if (wUserChairID == m_wCurrentBanker)
				{
					ScoreInfo[wUserChairID].lChipSerialScore = abs(m_lUserWinScore[wUserChairID]);
				}
				else
				{
					for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
					{
						if ((m_tagUserAreaInfo[wUserChairID][nAreaIndex].lAreaWinScore == (SCORE)0))
						{
							continue;
						}

						//区域赢分
						if ((m_tagUserAreaInfo[wUserChairID][nAreaIndex].lAreaWinScore > 0))
						{
							ScoreInfo[wUserChairID].lChipSerialScore += abs(m_tagUserAreaInfo[wUserChairID][nAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wUserChairID][nAreaIndex].lAreaRevenue);
						}
						//区域输分
						else
						{
							ScoreInfo[wUserChairID].lChipSerialScore += abs(m_tagUserAreaInfo[wUserChairID][nAreaIndex].lAreaInScore);
						}
					}
				}

				ASSERT((ScoreInfo[wUserChairID].lChipSerialScore >= (SCORE)0));

				//庄家且无人下注
				if ((m_lUserWinScore[wUserChairID] == (SCORE)0) && (ScoreInfo[wUserChairID].lChipSerialScore == (SCORE)0))
				{
					ScoreInfo[wUserChairID].cbType = SCORE_TYPE_DRAW;
				}

				if (ScoreInfo[wUserChairID].lScore == (SCORE)0)
				{
					continue;
				}

				m_pITableFrame->WriteUserScore(wUserChairID, ScoreInfo[wUserChairID]);
			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//////////////////////////////////////////////////
			CString stTrace;
			CTime Time(CTime::GetCurrentTime());
			stTrace.Format(TEXT("%d-%d %d:%d:%d |  m_cbFreeTime = %d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_cbFreeTime);
			WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);

			if (m_cbFreeTime <= 0)
			{
				//获取自定义配置
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				m_cbFreeTime = pCustomConfig->CustomGeneral.cbBetTime;
				if (m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
			}

			//////////////////////////////////////////////////

			if ((!IsGroupPersonalRoom()) || (IsGroupPersonalRoom() && m_wCurrentBanker != INVALID_CHAIR))
			{
				//设置时间
				m_dwJettonTime = (DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, 0L);

				//发送消息
				CMD_S_GameFree GameFree;
				ZeroMemory(&GameFree, sizeof(GameFree));
				GameFree.cbTimeLeave = m_cbFreeTime;
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));
			}

            //累加机器人退出占位局数
            for(WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
            {
                if(m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
                {
                    continue;
                }

                WORD wTempChairID = m_wOccupySeatChairIDArray[i];

                //机器人
                if(m_pITableFrame->GetTableUserItem(wTempChairID) && m_pITableFrame->GetTableUserItem(wTempChairID)->IsAndroidUser())
                {
                    m_lRobotOccupySeatPlayCount[wTempChairID]++;
                }
            }

            //重设机器人占位参数
            //RectifyRobotOccupySeatPara();
			if (!m_bWaitGiveUpBanker)
			{
				//切换庄家
				ChangeBanker(false);
			}
			else
			{
				if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker))
				{
					CString strTip;
					strTip.Format(TEXT("庄家[%s]下庄成功!"), m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->GetNickName());
					SendGameMessage(INVALID_CHAIR, strTip);
				}

				//切换庄家
				ChangeBanker(true);
				m_bWaitGiveUpBanker = false;
			}

			//空闲读取
			ReadAIConfig();

			CMD_S_GameFreeAIConfig GameFreeAIConfig;
			ZeroMemory(&GameFreeAIConfig, sizeof(GameFreeAIConfig));
			CopyMemory(&(GameFreeAIConfig.CustomAIConfig), &m_CustomAIConfig, sizeof(m_CustomAIConfig));

			for (WORD wUserChairID = 0; wUserChairID < m_wChairCount; ++wUserChairID)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if (pIServerUserItem == NULL)
				{
					continue;
				}

				if (pIServerUserItem->IsAndroidUser())
				{
					m_pITableFrame->SendTableData(wUserChairID, SUB_S_FREE_AICONFIG, &GameFreeAIConfig, sizeof(GameFreeAIConfig));

					continue;
				}

				//发送记录
				SendGameRecord(pIServerUserItem, m_pITableFrame->GetGameStatus());
			}
			
			// 发送消息
			m_pITableFrame->SendRoomData(NULL, SUB_S_CLEAR_USER_BET_INFO);

			//占位玩家连续不下注次数
			bool bvalid = false;
			for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
			{
				if (m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
				{
					continue;
				}
              
				if (m_bContinueNoBetCount[i] >= MAX_FREEBET_OCCUPY_SEAT)
				{
					//重置无效
					bvalid = true;
					WORD wTempChairID = m_wOccupySeatChairIDArray[i];
                    //机器人过滤
                    if(m_pITableFrame->GetTableUserItem(wTempChairID) && m_pITableFrame->GetTableUserItem(wTempChairID)->IsAndroidUser())
                    {
                        continue;
                    }
					m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;
					m_bContinueNoBetCount[i] = 0;

					CMD_S_OccupySeatStandUp OccupySeatStandUp;
					ZeroMemory(&OccupySeatStandUp, sizeof(OccupySeatStandUp));

					CopyMemory(OccupySeatStandUp.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
					OccupySeatStandUp.wStandUpOccupySeatChairID = wTempChairID;

					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OCCUPYSEAT_STANDUP, &OccupySeatStandUp, sizeof(OccupySeatStandUp));
				}
			}
            //机器人占位满足退出占位局数	
            for(WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
            {
                if(m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
                {
                    continue;
                }

                WORD wTempChairID = m_wOccupySeatChairIDArray[i];

                //机器人
                if(m_pITableFrame->GetTableUserItem(wTempChairID) && m_pITableFrame->GetTableUserItem(wTempChairID)->IsAndroidUser()
                   && m_lRobotOccupySeatPlayCount[wTempChairID] > m_nRobotQuitSeatTurn)
                {
                    //重置无效
                    bvalid = true;
                    WORD wTempChairID = m_wOccupySeatChairIDArray[i];
                    m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;
                    m_bContinueNoBetCount[i] = 0;
                    m_lRobotOccupySeatPlayCount[wTempChairID] = 0;

                    CMD_S_OccupySeatStandUp OccupySeatStandUp;
                    ZeroMemory(&OccupySeatStandUp, sizeof(OccupySeatStandUp));

                    CopyMemory(OccupySeatStandUp.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
                    OccupySeatStandUp.wStandUpOccupySeatChairID = wTempChairID;

                    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OCCUPYSEAT_STANDUP, &OccupySeatStandUp, sizeof(OccupySeatStandUp));
                }
            }
			if (bvalid)
			{
				CMD_S_UpdateOccupySeat UpdateOccupySeat;
				ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
				CopyMemory(UpdateOccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
				UpdateOccupySeat.wQuitOccupySeatChairID = INVALID_CHAIR;

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
			}

			stTrace.Format(TEXT("%d-%d %d:%d:%d |  GAME_FREEGAME_FREE\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond());
			//WriteInfo(TEXT("DragonTigerBattlePlace.log"), stTrace);

			return true;	
		}
	}
	return false;
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			if (wDataSize != sizeof(CMD_C_PlaceJetton))
			{
				return true;
			}

			//用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus < US_READY)
			{
				SendGameMessage(pIServerUserItem->GetChairID(), TEXT("请下一轮下注！"));

				return true;
			}

			//状态效验
			if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLACE_JETTON)
			{
				return true;
			}

			//消息处理
			CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pData;
			return OnUserPlaceJetton(pUserData->wChairID,pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
		}
	case SUB_C_CONTINUE_JETTON:
		{
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return pIServerUserItem->IsAndroidUser();
			}

			//用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING && pIServerUserItem->IsAndroidUser())
			{
				return true;
			}

			if (!pIServerUserItem->IsAndroidUser() && pUserData->cbUserStatus < US_READY)
			{
				return true;
			}

			//状态效验
			if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLACE_JETTON)
			{
				return pIServerUserItem->IsAndroidUser();
			}

			if (pIServerUserItem->GetChairID() == m_wCurrentBanker)
			{
				return pIServerUserItem->IsAndroidUser();
			}

			//玩家积分
			WORD wChairID = pIServerUserItem->GetChairID();
            SCORE lUserScore = pIServerUserItem->GetUserScore();

            SCORE lLastAllJettonPlace = 0L;
            SCORE lUserJettonScore = 0L;

			//统计本局已下注
			for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
			{
				lUserJettonScore += m_lUserInAllScore[wChairID][nAreaIndex];
                if(m_lUserInAllScore[wChairID][nAreaIndex] != 0)
                {
                    m_lLastAllJettonPlace[wChairID][nAreaIndex] = 0;
                }
			}
            //统计上局总下注
            for(int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
            {
                lLastAllJettonPlace += m_lLastAllJettonPlace[wChairID][nAreaIndex];
            }
		
			//检查下注标识
			bool bCheckPlaceJettonIsOk = true;

			//判断用户金币是否足够,是否超出个人限制
			if (lUserScore - lUserJettonScore < lLastAllJettonPlace
				|| lUserJettonScore + lLastAllJettonPlace > m_lUserLimitScore)
			{
				bCheckPlaceJettonIsOk = false;
			}
			
            //区域限制	
            for(int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
            {
                if(m_lLastAllJettonPlace[wChairID][nAreaIndex] + m_lUserInAllScore[wChairID][nAreaIndex] > m_lAreaLimitScore)
                {
                    bCheckPlaceJettonIsOk = false;

                    break;
                }
            }

			if (!bCheckPlaceJettonIsOk)
			{
				m_pITableFrame->SendTableData(wChairID, SUB_S_CONTINUE_JETTON_FAIL);

				return true;
			}
            else
			{
				//////////////////////////////////////////////////
				CString stTrace;
				CTime Time(CTime::GetCurrentTime());
				stTrace.Format(TEXT("%d-%d %d:%d:%d |  CONTINUE_JETTONBEGIN = %d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_pITableFrame->GetGameStatus());
				WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);
				//////////////////////////////////////////////////

				if (pIServerUserItem->GetUserStatus() != US_PLAYING)
				{
					pIServerUserItem->SetUserStatus(US_PLAYING, m_pITableFrame->GetTableID(), wChairID);
				}
                tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
                ASSERT(pCustomConfig);
                   
               
                for(int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
                {
                    if(m_lLastAllJettonPlace[wChairID][nAreaIndex] != 0)
                    {
                        CMD_C_PlaceJetton pPlaceJetton;
                        pPlaceJetton.cbJettonArea = nAreaIndex;
                        SCORE lJettonScore = m_lLastAllJettonPlace[wChairID][nAreaIndex];

						int nChipIndex = MAX_CHIP_COUNT - 1;
						while ( lJettonScore > 0 && nChipIndex >= 0)
                        {
							if (lJettonScore >= pCustomConfig->CustomGeneral.lChipArray[nChipIndex])
							{
								lJettonScore -= pCustomConfig->CustomGeneral.lChipArray[nChipIndex];
								pPlaceJetton.lJettonScore = pCustomConfig->CustomGeneral.lChipArray[nChipIndex];
								OnUserPlaceJetton(wChairID, pPlaceJetton.cbJettonArea, pPlaceJetton.lJettonScore);
							}
							else
							{
								nChipIndex--;
							}
                        } 
                    }
                   
                }
               
				//给超端发送下注信息
				SendUserBetInfo(pIServerUserItem->GetGameID());

				m_pITableFrame->SendTableData(wChairID, SUB_S_CONTINU_JETTON);

				//////////////////////////////////////////////////
				stTrace.Format(TEXT("%d-%d %d:%d:%d |  CONTINUE_JETTONEND = %d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_pITableFrame->GetGameStatus());
				WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);
				//////////////////////////////////////////////////
			}

			return true;
		}
	 case SUB_C_QUIT_OCCUPYSEAT:
		{
			bool binvalid = false;

			//校验数据
			for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
			{
				if (m_wOccupySeatChairIDArray[i] == pIServerUserItem->GetChairID())
				{
					binvalid = true;

					//重置无效
					m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;
					m_bContinueNoBetCount[i] = 0;

					break;
				}
			}

			ASSERT(binvalid == true);

			CMD_S_UpdateOccupySeat UpdateOccupySeat;
			ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
			CopyMemory(UpdateOccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
			UpdateOccupySeat.wQuitOccupySeatChairID = pIServerUserItem->GetChairID();

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

			return true;
		}
	 case SUB_C_OCCUPYSEAT:
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_OccupySeat));
			if (wDataSize != sizeof(CMD_C_OccupySeat))
			{
				return pIServerUserItem->IsAndroidUser();
			}

			//消息处理
			CMD_C_OccupySeat *pOccupySeat = (CMD_C_OccupySeat *)pData;

			if (pOccupySeat->wOccupySeatChairID != pIServerUserItem->GetChairID())
			{
				return pIServerUserItem->IsAndroidUser();
			}

			return OnUserOccupySeat(pOccupySeat->wOccupySeatChairID, pOccupySeat->cbOccupySeatIndex);
		}
	 case SUB_C_APPLY_BANKER:		//申请做庄
		{
			//用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();

			//用户效验
			if (pUserData->cbUserStatus == US_LOOKON)
			{
				return true;
			}

			return OnUserApplyBanker(pIServerUserItem);
		}
	 case SUB_C_CANCEL_BANKER:		//取消做庄
		{
			//用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();

			//用户效验
			if (pUserData->cbUserStatus == US_LOOKON)
			{
				return true;
			}

			return OnUserCancelBanker(pIServerUserItem);
		}
	 case SUB_C_REQUEST_STATISTICS_RECORD:
		{
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return pIServerUserItem->IsAndroidUser();
			}

			//////////////////////////////////////////////////
			CString stTrace;
			CTime Time(CTime::GetCurrentTime());
			stTrace.Format(TEXT("%d-%d %d:%d:%d |  STATISTICS_RECORDBEGIN = %d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_pITableFrame->GetGameStatus());
			WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);
			//////////////////////////////////////////////////

			BYTE cbDataBuffer[SOCKET_TCP_PACKET];
			ZeroMemory(cbDataBuffer, sizeof(cbDataBuffer));

			CMD_S_StatisticsRecord *pStatisticsRecord = (CMD_S_StatisticsRecord*)cbDataBuffer;

			WORD wSendDataSize = 0;

			for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
				if (pIServerUserItem == NULL)
				{
					continue;
				}

				//发送判断				
				if (wSendDataSize + sizeof(CMD_S_StatisticsRecord) > sizeof(cbDataBuffer))
				{
					//发送数据
					m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_STATISTICS_RECORD, cbDataBuffer, wSendDataSize);

					pStatisticsRecord = (CMD_S_StatisticsRecord*)cbDataBuffer;
					ZeroMemory(cbDataBuffer, sizeof(cbDataBuffer));
					wSendDataSize = 0;
				}

				POSITION posHead = m_listStatisticsRecord[wChairID].GetHeadPosition();
				pStatisticsRecord->wChairID = wChairID;

				while (posHead)
				{
					tagStatisticsRecord StatisticsRecord;
					ZeroMemory(&StatisticsRecord, sizeof(StatisticsRecord));

					StatisticsRecord = m_listStatisticsRecord[wChairID].GetNext(posHead);

					if (!StatisticsRecord.bBeBanker)
					{
						pStatisticsRecord->lRecordInAllScore += StatisticsRecord.lInAllScore;
					}
					
					//(0分不累加胜局)
					pStatisticsRecord->lWinCount += (StatisticsRecord.lWinScore > 0 ? 1 : 0);
				}

				wSendDataSize += sizeof(CMD_S_StatisticsRecord);
				pStatisticsRecord++;
			}

			//发送判断	
			if (wSendDataSize > 0)
			{
				//发送数据
				m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_STATISTICS_RECORD, cbDataBuffer, wSendDataSize);
			}

			//////////////////////////////////////////////////
			stTrace.Format(TEXT("%d-%d %d:%d:%d |  STATISTICS_RECORDEND = %d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_pITableFrame->GetGameStatus());
			WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);
			//////////////////////////////////////////////////

			return true;
		}

	default:
		return true;
	}

	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// 消息处理
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		//权限判断
		if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
		{
			return false;
		}

		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
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
		case SUB_C_MOSGOLD:
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_Mosgold));
			if (wDataSize != sizeof(CMD_C_Mosgold))
			{
				return false;
			}

			CMD_C_Mosgold *pMosgold = (CMD_C_Mosgold *)pData;

			g_lCurMosgoldRate = pMosgold->lConfigMosgoldRate;
			g_lCurMosgoldDispatchCon = pMosgold->lConfigMosgoldDispatchCon;
			g_lCurMosgoldDispatchRate = pMosgold->lConfigMosgoldDispatchRate;
			g_lCurMosgoldStorageRate = pMosgold->lConfigMosgoldStorageRate;

			CMD_S_Mosgold Mosgold;
			ZeroMemory(&Mosgold, sizeof(Mosgold));

			Mosgold.lCurMosgoldStorage = g_lCurMosgoldStorage;
			Mosgold.lCurMosgoldRate = g_lCurMosgoldRate;
			Mosgold.lCurMosgoldDispatchCon = g_lCurMosgoldDispatchCon;
			Mosgold.lCurMosgoldDispatchRate = g_lCurMosgoldDispatchRate;
			Mosgold.lCurMosgoldStorageRate = g_lCurMosgoldStorageRate;

			//发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_MOSGOLD, &Mosgold, sizeof(Mosgold));

			// 定义信息
			CMD_S_DebugText DebugText;
			ZeroMemory(&DebugText, sizeof(DebugText));

			TCHAR szMessageText[256] = TEXT("大奖配置成功!\n");
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

			bool bTempChouMosgoldStorage = false;

			//获取自定义配置
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);

			//寻找正在执行的列表(有且只有一个正在执行的列表)
			POSITION pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				if (sysctrl.sysCtrlStatus == PROGRESSINGEX)
				{
					sysctrl.sysCtrlStatus = (sysctrl.lSysCtrlSysWin == 0) ? CANCELEX : EXECUTE_CANCELEX;

					if (sysctrl.lSysCtrlConfigSysStorage - sysctrl.lSysCtrlCurSysStorage >= sysctrl.lSysCtrlChouMosgoldStorage && sysctrl.lSysCtrlChouMosgoldStorage > 0)
					{
						bTempChouMosgoldStorage = true;
					}
					else if (sysctrl.lSysCtrlChouMosgoldStorage == 0 && (sysctrl.lSysCtrlConfigSysStorage - sysctrl.lSysCtrlCurSysStorage >= sysctrl.lSysCtrlConfigPlayerStorage * g_lCurMosgoldStorageRate / 100))
					{
						bTempChouMosgoldStorage = true;
					}

					break;
				}
			}

			//插入系统控制列表
			g_dwSysCtrlIncreaseIndex++;

			SCORE lTempChouMosgoldStorage = (bTempChouMosgoldStorage ? pSysCtrl->lSysCtrlPlayerStorage * g_lCurMosgoldStorageRate / 100 : 0);

			SYSCTRL sysctrl;
			ZeroMemory(&sysctrl, sizeof(sysctrl));

			sysctrl.dwCtrlIndex = g_dwSysCtrlIncreaseIndex;
			sysctrl.lSysCtrlConfigSysStorage = pSysCtrl->lSysCtrlSysStorage;
			sysctrl.lSysCtrlConfigPlayerStorage = pSysCtrl->lSysCtrlPlayerStorage - lTempChouMosgoldStorage;
			sysctrl.lSysCtrlCurSysStorage = pSysCtrl->lSysCtrlSysStorage;;
			sysctrl.lSysCtrlCurPlayerStorage = pSysCtrl->lSysCtrlPlayerStorage - lTempChouMosgoldStorage;
			sysctrl.lSysCtrlParameterK = pSysCtrl->lSysCtrlParameterK;
			sysctrl.lSysCtrlSysStorResetRate = pSysCtrl->lSysCtrlSysStorResetRate;
			sysctrl.lSysCtrlResetCount = 0;
			sysctrl.lSysCtrlSysWin = 0;
			sysctrl.tmResetTime = CTime::GetCurrentTime();
			sysctrl.sysCtrlStatus = PROGRESSINGEX;
			sysctrl.lSysCtrlChouMosgoldStorage = lTempChouMosgoldStorage;

			//插入列表
			m_listSysCtrl.AddTail(sysctrl);

			//初始大奖库存增加
			g_lCurMosgoldStorage += lTempChouMosgoldStorage;
			g_lStatisticSysWin += lTempChouMosgoldStorage;

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

			CMD_S_Mosgold Mosgold;
			ZeroMemory(&Mosgold, sizeof(Mosgold));

			Mosgold.lCurMosgoldStorage = g_lCurMosgoldStorage;
			Mosgold.lCurMosgoldRate = g_lCurMosgoldRate;
			Mosgold.lCurMosgoldDispatchCon = g_lCurMosgoldDispatchCon;
			Mosgold.lCurMosgoldDispatchRate = g_lCurMosgoldDispatchRate;
			Mosgold.lCurMosgoldStorageRate = g_lCurMosgoldStorageRate;

			//发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_MOSGOLD, &Mosgold, sizeof(Mosgold));

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

					CMD_S_Mosgold Mosgold;
					ZeroMemory(&Mosgold, sizeof(Mosgold));

					Mosgold.lCurMosgoldStorage = g_lCurMosgoldStorage;
					Mosgold.lCurMosgoldRate = g_lCurMosgoldRate;
					Mosgold.lCurMosgoldDispatchCon = g_lCurMosgoldDispatchCon;
					Mosgold.lCurMosgoldDispatchRate = g_lCurMosgoldDispatchRate;
					Mosgold.lCurMosgoldStorageRate = g_lCurMosgoldStorageRate;

					//发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_MOSGOLD, &Mosgold, sizeof(Mosgold));
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
		case SUB_C_AREA_CTRL:
		{
			ASSERT(wDataSize == sizeof(CMD_C_AreaCtrl));
			if (wDataSize != sizeof(CMD_C_AreaCtrl))
			{
				return false;
			}

			if (m_pServerDebug == NULL)
			{
				return false;
			}

			CMD_C_AreaCtrl* pAreaCtrl = (CMD_C_AreaCtrl*)pData;

			//设置区域
			if (!pAreaCtrl->bCancelAreaCtrl)
			{
				//寻找正在执行的列表(有且只有一个正在执行的列表)
				POSITION pos = m_listAreaCtrl.GetHeadPosition();
				while (pos)
				{
					AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

					if (areactrl.areaCtrlStatus == AREACTRL_PROGRESSING)
					{
						areactrl.areaCtrlStatus = (areactrl.lAreaCtrlSysWin == 0) ? AREACTRL_CANCEL : AREACTRL_EXECUTE_CANCEL;

						break;
					}
				}

				AREACTRL areactrl;
				ZeroMemory(&areactrl, sizeof(areactrl));

				g_dwAreaCtrlIncreaseIndex++;

				areactrl.dwCtrlIndex = g_dwAreaCtrlIncreaseIndex;
				areactrl.cbCtrlPosIndex = pAreaCtrl->cbCtrlPosIndex;
				areactrl.cbConfigCtrlTimes = pAreaCtrl->cbConfigCtrlTimes;
				areactrl.cbCurCtrlTimes = 0;
				areactrl.lAreaCtrlSysWin = 0;
				areactrl.tmConfigTime = CTime::GetCurrentTime();
				areactrl.areaCtrlStatus = AREACTRL_PROGRESSING;
				areactrl.dwDebugUserGameID = pIServerUserItem->GetGameID();

				//插入列表
				m_listAreaCtrl.AddTail(areactrl);

				//发送结果
				CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
				ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
				CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
				CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;			
				CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				CurAreaCtrlInfo.lAreaCtrlSysWin = CalListAreaCtrlSysWin();

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));
			}
			//取消区域
			else
			{
				//区域调试列表
				POSITION pos = m_listAreaCtrl.GetHeadPosition();
				while (pos)
				{
					AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

					if (areactrl.dwCtrlIndex == pAreaCtrl->dwCtrlIndex)
					{
						areactrl.areaCtrlStatus = (areactrl.lAreaCtrlSysWin == 0) ? AREACTRL_CANCEL : AREACTRL_EXECUTE_CANCEL;

						// 发送消息
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO);

						break;
					}
				}
			}

			return m_pServerDebug->ServerDebug(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
		}
		case SUB_C_REFRESH_CUR_AREACTRL_INFO:
		{
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
			}

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO);

			POSITION pos = m_listAreaCtrl.GetHeadPosition();
			POSITION posLast = NULL;

			while (pos)
			{
				posLast = pos;
				AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

				if (areactrl.areaCtrlStatus == AREACTRL_PROGRESSING)
				{
					//发送结果
					CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
					ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
					CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
					CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
					CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
					CurAreaCtrlInfo.lAreaCtrlSysWin = CalListAreaCtrlSysWin();

					// 发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					posLast = NULL;

					break;
				}
			}

			//发送最近一个(不存在有效的)
			if (posLast)
			{
				AREACTRL &areactrl = m_listAreaCtrl.GetAt(posLast);

				//发送结果
				CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
				ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
				CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
				CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
				CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				CurAreaCtrlInfo.lAreaCtrlSysWin = CalListAreaCtrlSysWin();

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));
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
			StatisticService.lAreaLimitScore = m_lAreaLimitScore;
			StatisticService.wChairCount = m_wChairCount;

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_STATISTIC_SERVICE, &StatisticService, sizeof(StatisticService));

			return true;
		}
		case SUB_C_REFRESH_ANCHOU_MOSGOLD:
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

			CMD_S_Mosgold Mosgold;
			ZeroMemory(&Mosgold, sizeof(Mosgold));

			Mosgold.lCurMosgoldStorage = g_lCurMosgoldStorage;
			Mosgold.lCurMosgoldRate = g_lCurMosgoldRate;
			Mosgold.lCurMosgoldDispatchCon = g_lCurMosgoldDispatchCon;
			Mosgold.lCurMosgoldDispatchRate = g_lCurMosgoldDispatchRate;
			Mosgold.lCurMosgoldStorageRate = g_lCurMosgoldStorageRate;

			//发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_MOSGOLD, &Mosgold, sizeof(Mosgold));

			//发送房间用户信息
			SendRoomUserInfo();

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

			//区域调试列表
			pos = m_listAreaCtrl.GetHeadPosition();
			while (pos)
			{
				 AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

				 //发送结果
				 CMD_S_AreaCtrlTraceLog areaCtrlResult;
				 ZeroMemory(&areaCtrlResult, sizeof(areaCtrlResult));

				 CopyMemory(&(areaCtrlResult.AreaCtrlTraceLog), &areactrl, sizeof(areactrl));

				 // 发送消息
				 m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_AREACTRL_TRACELOG, &areaCtrlResult, sizeof(areaCtrlResult));
			}

			CMD_S_StartRecordTraceLog ListWinLostInfo;
			ZeroMemory(&ListWinLostInfo, sizeof(ListWinLostInfo));

			ListWinLostInfo.tmStartRecord = m_tmStartRecord;

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_STARTRECORD_TRACELOG, &ListWinLostInfo, sizeof(ListWinLostInfo));

			return true;
		}
		case SUB_C_QUERY_AICONFIG_DEBUG:
		{
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
			}

			CMD_S_AIConfigDebugResult AIConfigDebugResult;
			ZeroMemory(&AIConfigDebugResult, sizeof(AIConfigDebugResult));

			AIConfigDebugResult.cbType = TRUE;
			AIConfigDebugResult.wKindID = m_pGameServiceOption->wKindID;
			AIConfigDebugResult.wServerID = m_pGameServiceOption->wServerID;
			CopyMemory(&(AIConfigDebugResult.AIConfigDebugResult), &m_CustomAIConfig, sizeof(m_CustomAIConfig));

			//发送消息
			m_pITableFrame->SendRoomAIConfigData(pIServerUserItem, SUB_S_QUERY_AICONFIG_DEBUG_RESULT, &AIConfigDebugResult, sizeof(AIConfigDebugResult));

			return true;
		}
		case SUB_C_SAVE_AICONFIG_DEBUG:
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_SaveAIConfigDebug));
			if (wDataSize != sizeof(CMD_C_SaveAIConfigDebug))
			{
				return false;
			}

			CMD_C_SaveAIConfigDebug* pSaveAIConfigDebug = (CMD_C_SaveAIConfigDebug*)pData;

			//拷贝配置 
			CopyMemory(&m_CustomAIConfig, &(pSaveAIConfigDebug->SaveAIConfigDebug), sizeof(m_CustomAIConfig));

			CMD_S_AIConfigDebugResult AIConfigDebugResult;
			ZeroMemory(&AIConfigDebugResult, sizeof(AIConfigDebugResult));

			AIConfigDebugResult.cbType = FALSE;
			AIConfigDebugResult.wKindID = m_pGameServiceOption->wKindID;
			AIConfigDebugResult.wServerID = m_pGameServiceOption->wServerID;
			CopyMemory(&(AIConfigDebugResult.AIConfigDebugResult), &m_CustomAIConfig, sizeof(m_CustomAIConfig));

			//发送消息
			m_pITableFrame->SendRoomAIConfigData(pIServerUserItem, SUB_S_QUERY_AICONFIG_DEBUG_RESULT, &AIConfigDebugResult, sizeof(AIConfigDebugResult));

			CMD_S_GameFreeAIConfig GameFreeAIConfig;
			ZeroMemory(&GameFreeAIConfig, sizeof(GameFreeAIConfig));
			CopyMemory(&(GameFreeAIConfig.CustomAIConfig), &m_CustomAIConfig, sizeof(m_CustomAIConfig));

			for (WORD wUserChairID = 0; wUserChairID < m_wChairCount; ++wUserChairID)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if (pIServerUserItem == NULL || !pIServerUserItem->IsAndroidUser())
				{
					continue;
				}

				m_pITableFrame->SendTableData(wUserChairID, SUB_S_FREE_AICONFIG, &GameFreeAIConfig, sizeof(GameFreeAIConfig));
			}

			return true;
		}
		}
	}
	return false;
}

//用户断线
bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//切换庄家
	if (wChairID == m_wCurrentBanker) ChangeBanker(true);

	//取消申请
	for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID != m_ApplyUserArray[i]) continue;

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		//构造变量
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker, sizeof(CancelBanker));

		//设置变量
		CancelBanker.wCancelUser = wChairID;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}

	return true;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//记录起始分数
	m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();
	m_listStatisticsRecord[wChairID].RemoveAll();
	pIServerUserItem->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), wChairID);

	//设置时间
	if ((bLookonUser == false) && (m_dwJettonTime == 0L) && !IsGroupPersonalRoom())
	{
		if (m_cbFreeTime <= 0)
		{
			//获取自定义配置
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);

			m_cbFreeTime = pCustomConfig->CustomGeneral.cbBetTime;
			if (m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
		}

		m_dwJettonTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);

		//////////////////////////////////////////////////
		CString stTrace;
		CTime Time(CTime::GetCurrentTime());
		stTrace.Format(TEXT("%d-%d %d:%d:%d |  FIRST_SETIDI_FREE, gamestatus=%d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_pITableFrame->GetGameStatus());
		WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);
		//////////////////////////////////////////////////
	}

	if (IsGroupPersonalRoom() && m_wCurrentBanker == INVALID_CHAIR)
	{
		SendGameMessage(wChairID, TEXT("等待用户上庄!"));
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);

		//获取大联盟房间规则
		GetGroupPersonalRoomRule();

		m_pITableFrame->KillGameTimer(IDI_FREE);
		m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
		m_pITableFrame->KillGameTimer(IDI_GAME_END);
	}

	//发送房间用户信息
	SendRoomUserInfo();

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//发送房间用户信息
	SendRoomUserInfo();

	//起始分数重置
	m_lUserStartScore[wChairID] = 0;
	m_listStatisticsRecord[wChairID].RemoveAll();

	bool bvalid = false;
	//校验数据
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		if (m_wOccupySeatChairIDArray[i] == wChairID)
		{
			bvalid = true;

			//重置无效
			m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;
			m_bContinueNoBetCount[i] = 0;

			break;
		}
	}

	if (bvalid == true)
	{
		CMD_S_UpdateOccupySeat UpdateOccupySeat;
		ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
		CopyMemory(UpdateOccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
		UpdateOccupySeat.wQuitOccupySeatChairID = wChairID;

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
	}

	//记录成绩
	if (bLookonUser == false)
	{
		//取消申请
		for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID != m_ApplyUserArray[i]) continue;

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			//设置变量
			CancelBanker.wCancelUser = wChairID;

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}
		//切换庄家
		if (wChairID == m_wCurrentBanker) ChangeBanker(true);
		return true;
	}

	return true;
}

void CTableFrameSink::SaveData()
{
    SCORE tempRealScore = 0L;

    //玩家当局下注数据记录
	for (int k = 0; k < m_wChairCount; k++)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(k);
        if(pIServerUserItem == NULL) continue;
        SCORE lJettonScore = 0L;
        if(!pIServerUserItem->IsAndroidUser())
        {
            for(int i = 0; i < 3; i++)
            {
                lJettonScore += m_lUserInAllScore[k][i];
            }
            tempRealScore += lJettonScore;
        }
    }

    bool flag = false;
    int countT = 0;
    for(int i = 0; i < 10; i++)
    {
        if(m_lRealAllJettonScore[i] == 0)
        {
            countT = i + 1;
            m_lRealAllJettonScore[i] = tempRealScore;
            flag = true;
            break;
        }
    }
    if(flag)
    {
        SCORE temp = 0L;
        for(int i = 0; i < countT; i++)
        {
            temp += m_lRealAllJettonScore[i];
        }

		if (countT <= 0)
		{
			CString strTrace;
			strTrace.Format(TEXT("countT = %d"), countT);
			WriteInfo(TEXT("DragonTigerBattleServerDebug.log"), strTrace);

			countT = (countT <= 0 ? 1 : countT);
		}

        m_lLimitRobotJettonScore = temp / countT;
    }

    if(!flag)
    {
        SCORE Temp[10];
        CopyMemory(Temp, m_lRealAllJettonScore, sizeof(Temp));
        for(int i = 1; i < 10; i++)
        {
            m_lRealAllJettonScore[i - 1] = Temp[i];
        }
        m_lRealAllJettonScore[9] = tempRealScore;
        SCORE temp = 0L;
        for(int i = 0; i < 10; i++)
        {
            temp += m_lRealAllJettonScore[i];
        }
        m_lLimitRobotJettonScore = temp / 10;
    }

    SCORE lTotalLimit = m_lAreaLimitScore*3;
    if(m_lLimitRobotJettonScore < (lTotalLimit / 1000))
    {
        m_lLimitRobotJettonScore = (lTotalLimit / 100) * 3;
    }
	else if (m_lLimitRobotJettonScore , (lTotalLimit / 100))
    {
        m_lLimitRobotJettonScore = (lTotalLimit / 100) * 5;
    }
	else if (m_lLimitRobotJettonScore , (lTotalLimit / 100) * 3)
    {
        m_lLimitRobotJettonScore *= 5;
    }
	else if (m_lLimitRobotJettonScore , (lTotalLimit / 100) * 6)
    {
        m_lLimitRobotJettonScore *= 3;
    }
	else if (m_lLimitRobotJettonScore , (lTotalLimit / 10))
    {
        m_lLimitRobotJettonScore *= 2;
    }
	else if (m_lLimitRobotJettonScore , (lTotalLimit / 5))
    {
        m_lLimitRobotJettonScore *= 1;
    }
	else if (m_lLimitRobotJettonScore , (lTotalLimit / 10) * 3)
    {
        m_lLimitRobotJettonScore *= 0.6;
    }
	else if (m_lLimitRobotJettonScore , (lTotalLimit / 10) * 4)
    {
        m_lLimitRobotJettonScore *= 0.4;
    }
	else if (m_lLimitRobotJettonScore , (lTotalLimit / 2))
    {
        m_lLimitRobotJettonScore *= 0.2;
    }
    else
    {
        m_lLimitRobotJettonScore = (lTotalLimit / 100) * 5;
    }

    return;
}


//加注事件
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, SCORE lJettonScore)
{
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

	//效验参数
	ASSERT((cbJettonArea >= 0 && cbJettonArea < AREA_MAX) && lJettonScore > 0);
	if (!(cbJettonArea >= 0 && cbJettonArea < AREA_MAX) && lJettonScore > 0)
	{
		return true;
	}

	if (wChairID == m_wCurrentBanker)
	{
		return true;
	}

	//效验下注筹码
	//获取自定义配置
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
	ASSERT(pCustomConfig);

	bool bBetValid = false;
	for (WORD i = 0; i < MAX_CHIP_COUNT; i++)
	{
		if (pCustomConfig->CustomGeneral.lChipArray[i] == lJettonScore)
		{
			bBetValid = true;
			break;
		}
	}

	if (!bBetValid)
	{
		return true;
	}


	//已下注额
    SCORE lJettonCount = 0l;
	for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
	{
		lJettonCount += m_lUserInAllScore[wChairID][nAreaIndex];
	}
		
    SCORE lUserScore = pIServerUserItem->GetUserScore();

	//合法校验
	if (lUserScore < lJettonCount + lJettonScore)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pIServerUserItem->IsAndroidUser())
		{
			CString strTrace;
			strTrace.Format(TEXT("携带金币为%I64d, 已下注金币为%I64d, 当前下注金币为%I64d, 下注失败!"), lUserScore, lJettonCount, lJettonScore);

			//发送消息
			SendGameMessage(wChairID, strTrace);

			return true;
		}

		return true;
	}

	if (m_lUserLimitScore < lJettonCount + lJettonScore)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pIServerUserItem->IsAndroidUser())
		{
			CString strTrace;
			strTrace.Format(TEXT("个人限制为%I64d, 已下注金币为%I64d, 当前下注金币为%I64d, 下注失败!"), m_lUserLimitScore, lJettonCount, lJettonScore);
			//CTraceService::TraceString(strTrace, TraceLevel_Exception);

			//发送消息
			SendGameMessage(wChairID, strTrace);

			return true;
		}

		return true;
	}

	//成功标识
	bool bPlaceJettonSuccess=true;

	//AI或者系统当庄
	bool bAIOrSysBanker = ((m_wCurrentBanker == INVALID_CHAIR) || (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker) && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser()));

	//合法验证
	if (GetMaxPlayerScore(cbJettonArea, wChairID) >= lJettonScore)
	{
		//机器人验证
		if(pIServerUserItem->IsAndroidUser())
		{
			//判断剩余下注额度
			if (!IsRobotPlaceJettonValid(cbJettonArea, lJettonScore, wChairID))
			{
				//CTraceService::TraceString(TEXT("KKK3"), TraceLevel_Exception);

				return true;
			}

			//数目限制
			bool bHaveChip = false;
			for (int i = 0; i < AREA_MAX; i++)
			{
				if (m_lUserInAllScore[wChairID][i] > 0)
					bHaveChip = true;
			}

			if (!bHaveChip)
			{
				if (m_nChipRobotCount+1 > m_nMaxChipRobot)
				{
					bPlaceJettonSuccess = false;
				}
				else
					m_nChipRobotCount++;
			}

			//统计分数
			if (bPlaceJettonSuccess)
			{
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;

				//真人坐庄，抽取机器人的暗抽和彩金
				if (!bAIOrSysBanker)
				{
					m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaInScore += lJettonScore;

					//房间同时存在AI和真人玩家
					if (g_lCurAnChouRate > 0 && IsContainAIAndRealPlayer(pIServerUserItem->IsAndroidUser()))
					{
						g_lCurAnChouTotal += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
						m_lCurTurnAnChouWin += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
						g_lStatisticSysWin += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);

						m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaAnChou += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
					}

					//房间同时存在AI和真人玩家
					if (g_lCurMosgoldRate > 0 && IsContainAIAndRealPlayer(pIServerUserItem->IsAndroidUser()))
					{
						g_lCurMosgoldStorage += ((SCORE)(lJettonScore * g_lCurMosgoldRate) / (SCORE)100);
						g_lStatisticSysWin += ((SCORE)(lJettonScore * g_lCurMosgoldRate) / (SCORE)100);

						m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaMosgold += ((SCORE)(lJettonScore * g_lCurMosgoldRate) / (SCORE)100);
					}
				}
			}
		}
		//真人下注累加暗抽
		else
		{
			//AI或者系统当庄,抽取真人的暗抽和彩金
			if (bAIOrSysBanker)
			{
				m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaInScore += lJettonScore;

				//房间同时存在AI和真人玩家
				if (g_lCurAnChouRate > 0 && IsContainAIAndRealPlayer(pIServerUserItem->IsAndroidUser()))
				{
					g_lCurAnChouTotal += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
					m_lCurTurnAnChouWin += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
					g_lStatisticSysWin += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);

					m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaAnChou += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
				}

				//房间同时存在AI和真人玩家
				if (g_lCurMosgoldRate > 0 && IsContainAIAndRealPlayer(pIServerUserItem->IsAndroidUser()))
				{
					g_lCurMosgoldStorage += ((SCORE)(lJettonScore * g_lCurMosgoldRate) / (SCORE)100);
					g_lStatisticSysWin += ((SCORE)(lJettonScore * g_lCurMosgoldRate) / (SCORE)100);

					m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaMosgold += ((SCORE)(lJettonScore * g_lCurMosgoldRate) / (SCORE)100);
				}
			}
		}

		if (bPlaceJettonSuccess)
		{
			//保存下注
			m_lAreaInAllScore[cbJettonArea] += lJettonScore;
			m_lUserInAllScore[wChairID][cbJettonArea] += lJettonScore;		
		}			
	}
	else 
	{
		bPlaceJettonSuccess = false;
	}

	if (bPlaceJettonSuccess)
	{
		if (pIServerUserItem->GetUserStatus() != US_PLAYING)
		{
			pIServerUserItem->SetUserStatus(US_PLAYING, m_pITableFrame->GetTableID(), wChairID);
		}
       
		//变量定义
		CMD_S_PlaceJetton PlaceJetton;
		ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

		//构造变量
		PlaceJetton.wChairID=wChairID;
		PlaceJetton.cbJettonArea=cbJettonArea;
		PlaceJetton.lJettonScore=lJettonScore;

		//获取用户
		IServerUserItem * pIServerUser=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUser != NULL) 
			PlaceJetton.cbAndroid = pIServerUser->IsAndroidUser()? TRUE : FALSE;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
		
		//发送玩家下注信息
		SendUserBetInfo(pIServerUserItem->GetGameID());
       
		if (pIServerUserItem->IsAndroidUser())
        {
            m_lCurrentRobotJettonScore += lJettonScore;
        }

		//占位玩家连续不下注次数
		for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
		{
			if (m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
			{
				continue;
			}

			if (m_wOccupySeatChairIDArray[i] == wChairID)
			{
				m_bContinueNoBetCount[i] = 0;
			}
		}

		CString stTrace;
		CTime Time(CTime::GetCurrentTime());
		stTrace.Format(TEXT("%d-%d %d:%d:%d |  wChairID = %d, lJettonScore=%I64d,cbJettonArea=%d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), wChairID, lJettonScore, cbJettonArea);
		//WriteInfo(TEXT("DragonTigerBattlePlace.log"), stTrace);
	}
	else
	{
		if (!pIServerUserItem->IsAndroidUser())
		{
			CMD_S_PlaceJettonFail PlaceJettonFail;
			ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));

			PlaceJettonFail.lJettonArea = cbJettonArea;
			PlaceJettonFail.lPlaceScore = lJettonScore;
			PlaceJettonFail.wPlaceUser = wChairID;

			//发送消息
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
		}
	}

	return true;
}

//用户占位
bool CTableFrameSink::OnUserOccupySeat(WORD wOccupyChairID, BYTE cbOccupySeatIndex)
{
	//校验用户
	ASSERT(wOccupyChairID != INVALID_CHAIR);
	if (wOccupyChairID == INVALID_CHAIR)
	{
		return false;
	}

	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wOccupyChairID);
	ASSERT(cbOccupySeatIndex >= SEAT_LEFT1_INDEX && cbOccupySeatIndex <= SEAT_RIGHT3_INDEX);
	if (!(cbOccupySeatIndex >= SEAT_LEFT1_INDEX && cbOccupySeatIndex <= SEAT_RIGHT3_INDEX))
	{
		return pIServerUserItem->IsAndroidUser();
	}

	//庄家不能占位
	if (wOccupyChairID == m_wCurrentBanker)
	{
		return true;
	}

	if (m_wOccupySeatChairIDArray[cbOccupySeatIndex] != INVALID_CHAIR)
	{
		CMD_S_OccupySeat_Fail OccupySeat_Fail;
		ZeroMemory(&OccupySeat_Fail, sizeof(OccupySeat_Fail));

		OccupySeat_Fail.bSeatBusy = true;
		CopyMemory(OccupySeat_Fail.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
		OccupySeat_Fail.wAlreadyOccupySeatChairID = INVALID_CHAIR;
		OccupySeat_Fail.cbAlreadyOccupySeatIndex = INVALID_BYTE;

		//发送数据
		m_pITableFrame->SendTableData(wOccupyChairID, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));

		return true;
	}

	ASSERT(m_wOccupySeatChairIDArray[cbOccupySeatIndex] == INVALID_CHAIR);

	//校验是否已经占位
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		//占位失败
		if (m_wOccupySeatChairIDArray[i] == wOccupyChairID)
		{
			CMD_S_OccupySeat_Fail OccupySeat_Fail;
			ZeroMemory(&OccupySeat_Fail, sizeof(OccupySeat_Fail));

			OccupySeat_Fail.bSeatBusy = false;
			CopyMemory(OccupySeat_Fail.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
			OccupySeat_Fail.wAlreadyOccupySeatChairID = wOccupyChairID;
			OccupySeat_Fail.cbAlreadyOccupySeatIndex = i;

			//发送数据
			m_pITableFrame->SendTableData(wOccupyChairID, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));

			return true;
		}
	}
    //获取自定义配置
    tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
    ASSERT(pCustomConfig);

    ASSERT(m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() >= pCustomConfig->CustomGeneral.lOccupySeatFree);
    if(m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < pCustomConfig->CustomGeneral.lOccupySeatFree
       && !m_pITableFrame->GetTableUserItem(wOccupyChairID)->IsAndroidUser())
    {
		return pIServerUserItem->IsAndroidUser();
    }

    //校验机器人占位
    if(m_pITableFrame->GetTableUserItem(wOccupyChairID)->IsAndroidUser())
    {
		//重新读取配置参数(机器人占位人数随着真人人数变化)
		RectifyRobotOccupySeatPara();

		///////////////////////
		//CString strTrace;
		//strTrace.Format(TEXT("m_nRobotOccupySeatTable=%d"), m_nRobotOccupySeatTable);
		//CTraceService::TraceString(strTrace, TraceLevel_Exception);
		////////////////////////////

        //不支持机器人占位
        if(!m_bEnableRobotOccupySeat)
        {
            return true;
        }

		//校验占位条件
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < pCustomConfig->CustomGeneral.lOccupySeatFree)
		{
			return true;
		}

        //校验占位桌数
        int	nCurRobotOccupySeatTable = 0;
        for(WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
        {
            if(m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
            {
                continue;
            }

            WORD wTempChairID = m_wOccupySeatChairIDArray[i];

            //机器人
            if(m_pITableFrame->GetTableUserItem(wTempChairID) && m_pITableFrame->GetTableUserItem(wTempChairID)->IsAndroidUser())
            {
                nCurRobotOccupySeatTable++;
            }
        }

        if(nCurRobotOccupySeatTable + 1 > m_nRobotOccupySeatTable)
        {
            return true;
        }

        //校验退出占位局数	
        //ASSERT(m_lRobotOccupySeatPlayCount[wOccupyChairID] == 0);
        if(m_lRobotOccupySeatPlayCount[wOccupyChairID] != 0)
        {
            m_lRobotOccupySeatPlayCount[wOccupyChairID] = 0;
        }

        //机器人占位成功
    }

	//占位成功
	m_wOccupySeatChairIDArray[cbOccupySeatIndex] = wOccupyChairID;
	m_bContinueNoBetCount[cbOccupySeatIndex] = 0;

	CMD_S_OccupySeat OccupySeat;
	ZeroMemory(&OccupySeat, sizeof(OccupySeat));
	CopyMemory(OccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
	OccupySeat.wOccupySeatChairID = wOccupyChairID;
	OccupySeat.cbOccupySeatIndex = cbOccupySeatIndex;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));

	return true;
}

//申请庄家
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	if (!m_bEnableBanker)
	{
		return pIApplyServerUserItem->IsAndroidUser();
	}

	if (m_wCurrentBanker != INVALID_CHAIR && pIApplyServerUserItem->GetChairID() == m_wCurrentBanker)
	{
		return true;
	}

	//合法判断
	SCORE lUserScore = pIApplyServerUserItem->GetUserScore();
	if (lUserScore < m_lApplyBankerCondition)
	{
		SendGameMessage(pIApplyServerUserItem->GetChairID(), TEXT("你的金币不足以申请庄家，申请失败！"));
		return true;
	}

	//存在判断
	WORD wApplyUserChairID = pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx = 0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];
		if (wChairID == wApplyUserChairID)
		{
			SendGameMessage(pIApplyServerUserItem->GetChairID(), TEXT("你已经申请了庄家，不需要再次申请！"));
			return true;
		}
	}


	//机器人申请判断
	if (pIApplyServerUserItem->IsAndroidUser())
	{
		RectifyRobotRequestBankerPara();

		if (GetApplyUserArrayRobotCount() >= m_nRobotRequestBankerCount)
		{
			return true;
		}
	}

	//保存信息 
	m_ApplyUserArray.Add(wApplyUserChairID);

	//构造变量
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker, sizeof(ApplyBanker));

	//设置变量
	ApplyBanker.wApplyUser = wApplyUserChairID;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	//切换判断
	if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE && m_ApplyUserArray.GetCount() == 1)
	{
		ChangeBanker(false);
	}

	return true;
}

//取消申请
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	if (!m_bEnableBanker)
	{
		return pICancelServerUserItem->IsAndroidUser();
	}

	//当前庄家
	if (pICancelServerUserItem->GetChairID() == m_wCurrentBanker && m_pITableFrame->GetGameStatus() != GAME_STATUS_FREE)
	{
		//发送消息
		SendGameMessage(pICancelServerUserItem->GetChairID(), TEXT("申请成功，等待游戏结束后主动下庄!"));
		m_bWaitGiveUpBanker = true;

		//发送消息
		m_pITableFrame->SendTableData(pICancelServerUserItem->GetChairID(), SUB_S_BANKER_QUIT_RESULT);

		return true;
	}

	//存在判断
	for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		//获取玩家
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		//条件过滤
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->GetUserID() != pICancelServerUserItem->GetUserID()) continue;

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		if (m_wCurrentBanker != wChairID)
		{
			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			//设置变量
			CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker == wChairID)
		{
			//切换庄家 
			m_wCurrentBanker = INVALID_CHAIR;
			ChangeBanker(true);
		}

		return true;
	}

	return true;
}

//发送扑克
bool CTableFrameSink::DispatchTableCard()
{
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	m_GameLogic.RandCardList(m_cbTableCardArray,sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0]));

	return true;
}

//发送庄家
void CTableFrameSink::SendApplyUser(IServerUserItem *pRcvServerUserItem)
{
	if (!m_bEnableBanker)
	{
		return;
	}

	for (INT_PTR nUserIdx = 0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];

		//获取玩家
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem) continue;

		//庄家判断
		if (pServerUserItem->GetChairID() == m_wCurrentBanker) continue;

		//构造变量
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser = wChairID;

		//发送消息
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
}

//更换庄家
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	if (!m_bEnableBanker)
	{
		return true;
	}

	//只有空闲状态允许换庄
	if (m_pITableFrame->GetGameStatus() != GAME_STATUS_FREE)
	{
		return true;
	}

	//切换标识
	bool bChangeBanker = false;

	//检验上庄列表中是否都满足上庄条件
	for (WORD i = 0; i<m_ApplyUserArray.GetCount(); i++)
	{
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		ASSERT(pIServerUserItem != NULL);
		if (pIServerUserItem->GetUserScore() < m_lApplyBankerCondition)
		{
			//删除玩家
			m_ApplyUserArray.RemoveAt(i);
		}
	}

	//取消当前
	if (bCancelCurrentBanker)
	{
		for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			//获取玩家
			WORD wChairID = m_ApplyUserArray[i];

			//条件过滤
			if (wChairID != m_wCurrentBanker) continue;

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			//设置变量
			CancelBanker.wCancelUser = wChairID;

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}

		//设置庄家
		m_wCurrentBanker = INVALID_CHAIR;

		//轮换判断
		if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE && m_ApplyUserArray.GetCount() != 0)
		{
			m_wCurrentBanker = m_ApplyUserArray[0];
		}

		//设置变量
		bChangeBanker = true;
		m_bExchangeBanker = true;
	}
	//轮庄判断
	else if (m_wCurrentBanker != INVALID_CHAIR)
	{
		//获取庄家
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

		if (pIServerUserItem != NULL)
		{
			SCORE lBankerScore = pIServerUserItem->GetUserScore();
			bool bAndroidBanker = pIServerUserItem->IsAndroidUser();

			//次数判断
			if (m_lPlayerBankerMAX <= m_wBankerTime || lBankerScore < m_lApplyBankerCondition /*|| bAndroidBanker*/)
			{
				//庄家增加判断 同一个庄家情况下只判断一次
				if (/*!bAndroidBanker && */m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker && lBankerScore >= m_lApplyBankerCondition)
				{
					//加庄局数设置：当庄家坐满设定的局数之后(m_lBankerMAX)，
					//所带金币值还超过下面申请庄家列表里面所有玩家金币时，
					//可以再加坐庄m_lBankerAdd局，加庄局数可设置。

					//金币超过m_lBankerScoreMAX之后，
					//就算是下面玩家的金币值大于他的金币值，他也可以再加庄m_lBankerScoreAdd局。
					bool bScoreMAX = true;
					m_bExchangeBanker = false;

					for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						//获取玩家
						WORD wChairID = m_ApplyUserArray[i];
						IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
						ASSERT(pIUserItem != NULL);
						SCORE lScore = pIServerUserItem->GetUserScore();

						if (wChairID != m_wCurrentBanker && lBankerScore <= lScore)
						{
							bScoreMAX = false;
							break;
						}
					}

					if ((bScoreMAX || (lBankerScore > m_lBankerScoreMAX && m_lBankerScoreMAX != 0l)) && !IsGroupPersonalRoom())
					{
						if (bScoreMAX && m_lBankerAdd != 0)
						{
							m_lPlayerBankerMAX += m_lBankerAdd;

						}
						if (lBankerScore > m_lBankerScoreMAX && m_lBankerScoreMAX != 0 && m_lBankerScoreAdd != 0)
						{
							m_lPlayerBankerMAX += m_lBankerScoreAdd;
						}
						return true;
					}
				}

				//if (!((bAndroidBanker && m_wBankerTime > m_wRobotMaxSerialTurn) ||
				//	(bAndroidBanker && -m_lRobotBankerWinScore >= m_lRobotLostScoreLimitQB) ||
				//	(bAndroidBanker && D_LessThen(lBankerScore, m_lApplyBankerCondition))))
				//{
				//	return true;
				//}

				//撤销玩家
				for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
				{
					//获取玩家
					WORD wChairID = m_ApplyUserArray[i];

					//条件过滤
					if (wChairID != m_wCurrentBanker) continue;

					//删除玩家
					m_ApplyUserArray.RemoveAt(i);

					break;
				}

				//设置庄家
				m_wCurrentBanker = INVALID_CHAIR;

				//轮换判断
				if (m_ApplyUserArray.GetCount() != 0)
				{
					m_wCurrentBanker = m_ApplyUserArray[0];
				}

				bChangeBanker = true;
				m_bExchangeBanker = true;

				//提示消息
				TCHAR szTipMsg[128];
				if (lBankerScore<m_lApplyBankerCondition)
					_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]分数少于(%I64d)，强行换庄!"), pIServerUserItem->GetNickName(), m_lApplyBankerCondition);
				else
					_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]做庄次数达到(%I64d)，强行换庄!"), pIServerUserItem->GetNickName(), m_lPlayerBankerMAX);

				//发送消息
				SendGameMessage(INVALID_CHAIR, szTipMsg);
			}
		}
		else
		{
			for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
			{
				//获取玩家
				WORD wChairID = m_ApplyUserArray[i];

				//条件过滤
				if (wChairID != m_wCurrentBanker) continue;

				//删除玩家
				m_ApplyUserArray.RemoveAt(i);

				break;
			}
			//设置庄家
			m_wCurrentBanker = INVALID_CHAIR;
		}
	}
	//系统做庄
	else if (m_wCurrentBanker == INVALID_CHAIR && m_ApplyUserArray.GetCount() != 0)
	{
		m_wCurrentBanker = m_ApplyUserArray[0];
		bChangeBanker = true;
		m_bExchangeBanker = true;
	}

	//切换判断
	if (bChangeBanker)
	{
		//最大坐庄数
		m_lPlayerBankerMAX = m_lBankerMAX;

		//设置变量
		m_wBankerTime = 0;
		m_lBankerWinScore = 0;

		m_lRobotBankerWinScore = 0;

		//发送消息
		CMD_S_ChangeBanker ChangeBanker;
		ZeroMemory(&ChangeBanker, sizeof(ChangeBanker));
		ChangeBanker.wBankerUser = m_wCurrentBanker;
		ChangeBanker.lBankerScore = 0x7ffffffffff;
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			ChangeBanker.lBankerScore = pIServerUserItem->GetUserScore();
		}
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CHANGE_BANKER, &ChangeBanker, sizeof(ChangeBanker));

		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			ASSERT(m_pGameServiceOption != NULL);

			//读取消息
			int nMessageCount = 3;

			//读取配置
			INT nIndex = rand() % nMessageCount;
			TCHAR szMessage1[256], szMessage2[256];
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomConfig);
			if (0 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem1, sizeof(pCustomConfig->CustomGeneral.szMessageItem1));
			}
			else if (1 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem2, sizeof(pCustomConfig->CustomGeneral.szMessageItem2));
			}
			else if (2 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem3, sizeof(pCustomConfig->CustomGeneral.szMessageItem3));
			}

			//获取玩家
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

			//发送消息
			_sntprintf_s(szMessage2, CountArray(szMessage2), TEXT("【 %s 】上庄了，%s"), pIServerUserItem->GetNickName(), szMessage1);
			SendGameMessage(INVALID_CHAIR, szMessage2);

			//判断当前庄家是否占位，如果占位则退出
			BYTE cbBankerOccupySeatIndex = INVALID_BYTE;
			for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
			{
				if (m_wOccupySeatChairIDArray[i] == m_wCurrentBanker)
				{
					cbBankerOccupySeatIndex = i;

					break;
				}
			}

			if (cbBankerOccupySeatIndex != INVALID_BYTE)
			{
				m_wOccupySeatChairIDArray[cbBankerOccupySeatIndex] = INVALID_CHAIR;
				m_bContinueNoBetCount[m_wCurrentBanker] = 0;

				CMD_S_UpdateOccupySeat UpdateOccupySeat;
				ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
				CopyMemory(UpdateOccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
				UpdateOccupySeat.wQuitOccupySeatChairID = m_wCurrentBanker;

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

			}
		}

		//是否大联盟金币约战房间
		if (IsGroupPersonalRoom())
		{
			if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker) && !m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser())
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				m_pITableFrame->SetGroupMaxChairBankerUserID(m_wCurrentBanker, pIServerUserItem->GetUserID());

				//if (m_dwJettonTime == 0L)
				{
					if (m_cbFreeTime <= 0)
					{
						//获取自定义配置
						tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
						ASSERT(pCustomConfig);

						m_cbFreeTime = pCustomConfig->CustomGeneral.cbBetTime;
						if (m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
					}

					m_dwJettonTime = (DWORD)time(NULL);
					m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, NULL);
					m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
				}
			}
			else
			{
				m_pITableFrame->SetGroupMaxChairBankerUserID(INVALID_CHAIR, 0);

				SendGameMessage(INVALID_CHAIR, TEXT("等待用户上庄!"));

				//重置定时器
				m_dwJettonTime = 0L;
				m_pITableFrame->KillGameTimer(IDI_FREE);
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->KillGameTimer(IDI_GAME_END);
				
				//设置状态
				m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
			}
		}
	}

	return bChangeBanker;
}

//最大下注
SCORE CTableFrameSink::GetMaxPlayerScore(BYTE cbJettonArea, WORD wChairID)
{
	//获取玩家
	IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL == pIMeServerUserItem)
	{
		return 0L;
	}
		
	//已下注额
	SCORE lNowJetton = 0l;
	for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
	{
		lNowJetton += m_lUserInAllScore[wChairID][nAreaIndex];
	}

	//区域限制
	SCORE lAreaLimit = m_lAreaLimitScore - m_lAreaInAllScore[cbJettonArea];

	//玩家限制
	SCORE lUserLimit = m_lUserLimitScore - m_lUserInAllScore[wChairID][cbJettonArea];

	//最大下注
	SCORE lMaxJetton = 0L;

	//庄家金币
	SCORE lBankerScore = m_lBankerScore;

	//区域倍率
	BYTE cbMultiple[AREA_MAX] = { MULTIPLE_LONG, MULTIPLE_PING, MULTIPLE_HU };

	//庄家赔分
	SCORE lBankerLostScore[AREA_MAX];
	ZeroMemory(lBankerLostScore, sizeof(lBankerLostScore));

	//开龙区域，庄家赔分
	for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
	{
		if (wAreaIndex == AREA_LONG)
		{
			lBankerLostScore[AREA_LONG] -= (m_lAreaInAllScore[wAreaIndex] * (cbMultiple[wAreaIndex]));
		}
		else
		{
			lBankerLostScore[AREA_LONG] += m_lAreaInAllScore[wAreaIndex];
		}
	}

	//开虎区域，庄家赔分
	for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
	{
		if (wAreaIndex == AREA_HU)
		{
			lBankerLostScore[AREA_HU] -= (m_lAreaInAllScore[wAreaIndex] * (cbMultiple[wAreaIndex]));
		}
		else
		{
			lBankerLostScore[AREA_HU] += m_lAreaInAllScore[wAreaIndex];
		}
	}

	//开和区域，庄家赔分
	for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
	{
		if (wAreaIndex == AREA_PING)
		{
			lBankerLostScore[AREA_PING] -= (m_lAreaInAllScore[wAreaIndex] * (cbMultiple[wAreaIndex]));
		}
	}

	//庄家最大赔分
	SCORE lMaxBankerLostScore = min(min(lBankerLostScore[AREA_LONG], lBankerLostScore[AREA_HU]), lBankerLostScore[AREA_PING]);

	lBankerScore += lMaxBankerLostScore;

	//for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex)
	//{
	//	lBankerScore = lBankerScore + m_lAreaInAllScore[nAreaIndex];
	//}

	//最大下注
	lMaxJetton = min(pIMeServerUserItem->GetUserScore() - lNowJetton, lUserLimit);

	lMaxJetton = min(lMaxJetton, lAreaLimit);

	lMaxJetton = min(lMaxJetton, (lBankerScore / cbMultiple[cbJettonArea]));

	//非零限制
	ASSERT(lMaxJetton >= 0);
	lMaxJetton = max(lMaxJetton, 0);

	return lMaxJetton;
}

//计算分数
SCORE CTableFrameSink::Calculate(BYTE cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker, BYTE cbWinAre[3])
{
    //系统输赢
	SCORE lSysWinScore = 0l;

    //区域输赢(针对下注者)
    INT nAreaWin[AREA_MAX] = {0};

    //推断区域
    //获取逻辑值
    BYTE cbDragonLogicValue = GetCardValue(cbTableCardArray[0]);
    BYTE cbTigerLogicValue = GetCardValue(cbTableCardArray[1]);

    //初始化区域
    for(int i = 0; i < AREA_MAX; i++)
    {
        nAreaWin[i] = RESULT_LOSE;
    }

    //比较两门大小
    if(cbDragonLogicValue == cbTigerLogicValue)
    {
        nAreaWin[AREA_PING] = RESULT_WIN;
        cbWinAre[AREA_PING] = 1;
    }
    else if(cbDragonLogicValue > cbTigerLogicValue)
    {
        nAreaWin[AREA_LONG] = RESULT_WIN;
        cbWinAre[AREA_LONG] = 1;
    }
    else
    {
        nAreaWin[AREA_HU] = RESULT_WIN;
        cbWinAre[AREA_HU] = 1;

    }
    
    //区域倍率
    BYTE cbMultiple[AREA_MAX] = {MULTIPLE_LONG, MULTIPLE_PING, MULTIPLE_HU};

    //系统当庄或者机器人当庄
    if((wCurrentBanker == INVALID_CHAIR) || ((wCurrentBanker != INVALID_CHAIR) && m_pITableFrame->GetTableUserItem(wCurrentBanker) && m_pITableFrame->GetTableUserItem(wCurrentBanker)->IsAndroidUser()))
    {
        //只考虑真人玩家
        //计算积分
        SCORE lRealPlayerWinScore = 0;
        for(WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
        {
            //获取用户
            IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
            if(pIServerUserItem == NULL)
            {
                continue;
            }

            if(pIServerUserItem->IsAndroidUser())
            {
                continue;
            }

            //每个人定注数
            for(WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
            {
                if(nAreaWin[wAreaIndex] == RESULT_WIN)
                {
                    lRealPlayerWinScore += (SCORE)(lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
                }
                else
                {
                    //下注龙或下注虎, 开和时退下注金额.
                    if(nAreaWin[AREA_PING] == RESULT_WIN && (wAreaIndex == AREA_LONG || wAreaIndex == AREA_HU))
                    {
                    }
                    else
                    {
                        lRealPlayerWinScore -= lUserInAllScore[wChairID][wAreaIndex];
                    }
                }
            }
        }

        lSysWinScore = -lRealPlayerWinScore;
    }
    //真人玩家做庄
    else if((wCurrentBanker != INVALID_CHAIR) && m_pITableFrame->GetTableUserItem(wCurrentBanker) && !m_pITableFrame->GetTableUserItem(wCurrentBanker)->IsAndroidUser())
    {
        //只考虑机器人玩家
        //计算积分
        SCORE lAIWinScore = 0;
        for(WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
        {
            //获取用户
            IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
            if(pIServerUserItem == NULL)
            {
                continue;
            }

            if(!pIServerUserItem->IsAndroidUser())
            {
                continue;
            }

            //每个人定注数
            for(WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
            {
                if(nAreaWin[wAreaIndex] == RESULT_WIN)
                {
                    lAIWinScore += (SCORE)(lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
                }
                else
                {
                    //下注龙或下注虎, 开和时退下注金额.
                    if(nAreaWin[AREA_PING] == RESULT_WIN && (wAreaIndex == AREA_LONG || wAreaIndex == AREA_HU))
                    {
                    }
                    else
                    {
                        lAIWinScore -= lUserInAllScore[wChairID][wAreaIndex];
                    }
                }
            }
        }

        lSysWinScore = lAIWinScore;
    }

    return lSysWinScore;
}

//计算得分
SCORE CTableFrameSink::CalculateScore(OUT SCORE& lBankerWinScore)
{
	//区域输赢(针对下注者)
	INT nAreaWin[AREA_MAX] = {0};

	BYTE Result = 0;
	lBankerWinScore = 0;

	//推断区域
	DeduceWinner(nAreaWin, Result);

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	ZeroMemory(m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));

	//重置区域得分和区域税收
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
		{
			m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = 0;
			m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue = 0;
		}
	}

	//区域倍率
	BYTE cbMultiple[AREA_MAX] = { MULTIPLE_LONG, MULTIPLE_PING, MULTIPLE_HU };

	//计算闲家得分
	for (WORD wChairID=0; wChairID < m_wChairCount; wChairID++)
	{
		//庄家判断
		if (m_wCurrentBanker == wChairID)
		{
			continue;
		}

		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		//每个人定注数
		for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
		{
			if (nAreaWin[wAreaIndex] == RESULT_WIN && m_lUserInAllScore[wChairID][wAreaIndex] > 0)
			{
                m_lUserAreaWinScore[wChairID][wAreaIndex] = (SCORE)(m_lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));

				//真人玩家
				//if (!bIsAndroidUser)
				{
					m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = (SCORE)(m_lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
					m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue = m_pITableFrame->CalculateRevenue(wChairID, m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);
				}
				////AI不收税
				//else
				//{
				//	m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = (SCORE)(m_lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
				//}
			}
			else if (nAreaWin[wAreaIndex] == RESULT_LOSE && m_lUserInAllScore[wChairID][wAreaIndex] > 0)
			{
				//下注龙或下注虎, 开和时退下注金额.
				if ( nAreaWin[AREA_PING] == RESULT_WIN && (wAreaIndex == AREA_LONG || wAreaIndex == AREA_HU))
				{
				}
				else
				{
					m_lUserAreaWinScore[wChairID][wAreaIndex] = -m_lUserInAllScore[wChairID][wAreaIndex];

					//真人玩家
					//if (!bIsAndroidUser)
					{
						m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = -m_lUserInAllScore[wChairID][wAreaIndex];
					}
					////AI不收税
					//else
					//{
					//	m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = -m_lUserInAllScore[wChairID][wAreaIndex];
					//}
				}
			}
		}

		//计算税收
		for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
		{
			m_lUserWinScore[wChairID] += m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore;
			m_lUserRevenue[wChairID] += m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue;
		}

		if (m_lUserRevenue[wChairID] > 0)
		{
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}
	}

	//庄家成绩
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		//庄家判断
		if (m_wCurrentBanker != wChairID)
		{
			continue;
		}

		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		ASSERT(pIServerUserItem);

		if (pIServerUserItem == NULL)
		{
			continue;
		}

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		//每个人定注数
		for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
		{
			if (nAreaWin[wAreaIndex] == RESULT_WIN && m_lAreaInAllScore[wAreaIndex] > 0)
			{
				//真人玩家
				//if (!bIsAndroidUser)
				{
					m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = -(SCORE)(m_lAreaInAllScore[wAreaIndex] * (cbMultiple[wAreaIndex]));
				}
				//AI不收税
				//else
				//{
				//	m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = -(SCORE)(m_lAreaInAllScore[wAreaIndex] * (cbMultiple[wAreaIndex]));
				//}
			}
			else if (nAreaWin[wAreaIndex] == RESULT_LOSE && m_lAreaInAllScore[wAreaIndex] > 0)
			{
				//下注龙或下注虎, 开和时退下注金额.
				if (nAreaWin[AREA_PING] == RESULT_WIN && (wAreaIndex == AREA_LONG || wAreaIndex == AREA_HU))
				{
				}
				else
				{
					//真人玩家
					//if (!bIsAndroidUser)
					{
						m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = m_lAreaInAllScore[wAreaIndex];
						m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue = m_pITableFrame->CalculateRevenue(wChairID, m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);
					}
					//AI不收税
					//else
					//{
					//	m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = m_lAreaInAllScore[wAreaIndex];
					//}
				}
			}
		}

		//总的分数
		for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
		{
			m_lUserWinScore[wChairID] += m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore;
			m_lUserRevenue[wChairID] += m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue;
		}

		if (m_lUserRevenue[wChairID] > 0)
		{
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}

		lBankerWinScore = m_lUserWinScore[wChairID];

		break;
	}

	//真人玩家得分
    SCORE lRealPlayerWinScore = 0;
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (pIServerUserItem->IsAndroidUser())
		{
			continue;
		}

		lRealPlayerWinScore += m_lUserWinScore[wChairID];
	}

	return lRealPlayerWinScore;
}

void CTableFrameSink::DeduceWinner(OUT INT nAreaWin[AREA_MAX], OUT BYTE& cbResult)
{
	//获取逻辑值
	BYTE cbDragonLogicValue = m_GameLogic.GetCardValue(m_cbTableCardArray[0]);
	BYTE cbTigerLogicValue = m_GameLogic.GetCardValue(m_cbTableCardArray[1]);

	//初始化区域
	for (int i = 0; i < AREA_MAX; i++)
	{
		nAreaWin[i] = RESULT_LOSE;
	}
		
	//比较两门大小
	if ( cbDragonLogicValue == cbTigerLogicValue )
	{
		cbResult = AREA_PING;
		nAreaWin[AREA_PING] = RESULT_WIN;
	}
	else if ( cbDragonLogicValue > cbTigerLogicValue )
	{
		cbResult = AREA_LONG;
		nAreaWin[AREA_LONG] = RESULT_WIN;
	}
	else
	{
		cbResult = AREA_HU;
		nAreaWin[AREA_HU] = RESULT_WIN;
	}
}

//发送记录
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem, BYTE cbGameStatus)
{
	if (pIServerUserItem->IsClientReady())
	{
		CMD_S_SimpleChart SimpleChart;
		ZeroMemory(&SimpleChart, sizeof(SimpleChart));

		CopyMemory(SimpleChart.ServerGameRecordArray, m_SimpleChartArray, sizeof(m_SimpleChartArray));
		SimpleChart.cbGameStatus = cbGameStatus;

		// 发送消息
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_SIMPLE_CHART, &SimpleChart, sizeof(SimpleChart));

		CMD_S_AdvancedChart AdvancedChart;
		ZeroMemory(&AdvancedChart, sizeof(AdvancedChart));

		CopyMemory(AdvancedChart.ServerGameRecordArray, m_AdvancedChartArray, sizeof(m_AdvancedChartArray));
		AdvancedChart.cbGameStatus = cbGameStatus;

		// 发送消息
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_ADVANCED_CHART, &AdvancedChart, sizeof(AdvancedChart));
	}
}

SCORE CTableFrameSink::GameOver()
{
	//获取用户调试类型
	bool bUserCtrl = false;
	EM_CTRL_TYPE userCtrlType = GetUserCtrlType();
	SCORE lKeyPlayerStorage = 0;
	SCORE lBankerWinScore = 0;

	WORD wUserWinChance = INVALID_WORD;
    SCORE lDVal = 0;
    SCORE lMaxCtrlStorage = 0;
    SCORE lSysCtrlParameterK = 0;
	bool bSysWin = false;

	switch (userCtrlType)
	{
	case ROOM_CTRL:
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

				lTempRoomCtrlSysStorage = max(0 , lTempRoomCtrlSysStorage);
				lTempRoomCtrlPlayerStorage = max(0 , lTempRoomCtrlPlayerStorage);

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

		break;
	}
	case SYS_CTRL:
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

				lTempCurSysStorage = max(0 , lTempCurSysStorage);
				lTempCurPlayerStorage = max(0 , lTempCurPlayerStorage);

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

	//系统赢标志， INVALID_BYTE为难度50随机
	BYTE cbSysWin = INVALID_BYTE;
	BYTE cbRandVal = rand() % 100;

	cbSysWin = cbRandVal < wUserWinChance ? FALSE : TRUE;

	//超端控制
	bool bDebug = false;
	BYTE cbWinSideDebug = 0xFF;
	bool bNeedDebug = false;

	if (m_pServerDebug != NULL)
	{
		bNeedDebug = m_pServerDebug->NeedDebug(cbWinSideDebug);
	}

	//区域调试校验参数
	if (bNeedDebug && (((cbWinSideDebug == AREA_SYSLOST || cbWinSideDebug == AREA_SYSWIN) && IsAreaCtrlSysValid()) || (cbWinSideDebug != AREA_SYSLOST && cbWinSideDebug != AREA_SYSWIN)) 
	&& m_pServerDebug->DebugResult(m_cbTableCardArray, m_lUserInAllScore, m_wCurrentBanker))
	{
		//调试完成
		bDebug = true;

		//更新区域控制记录
		POSITION pos = m_listAreaCtrl.GetHeadPosition();
		while (pos)
		{
			AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

			if (areactrl.areaCtrlStatus == AREACTRL_PROGRESSING)
			{
				areactrl.cbCurCtrlTimes++;

				//真人玩家得分
				SCORE lRealPlayerWinScore = CalculateScore(lBankerWinScore);
				areactrl.lAreaCtrlSysWin -= lRealPlayerWinScore;

				if (areactrl.cbCurCtrlTimes == areactrl.cbConfigCtrlTimes)
				{
					areactrl.areaCtrlStatus = AREACTRL_FINISH;
				}

				//发送结果
				CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
				ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
				CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
				CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
				CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				CurAreaCtrlInfo.lAreaCtrlSysWin = CalListAreaCtrlSysWin();

				//发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				//更新统计
				g_lStatisticSysWin -= lRealPlayerWinScore;

				//更新服务比例
				for (WORD i = 0; i < m_wChairCount; i++)
				{
					IServerUserItem *pKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (!pKeyServerUserItem)
					{
						continue;
					}

					if (pKeyServerUserItem->IsAndroidUser())
					{
						continue;
					}

					for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
					{
						g_lStatisticService += m_tagUserAreaInfo[i][wAreaIndex].lAreaRevenue;
					}
				}

				break;
			}
		}
      
	}
	else
	{
		LONGLONG lCirculateCount = 0L;
		LONGLONG lDispatchMosgoldCount = 0L;

		//派奖标识(开和)
		bool bDispatchMosgold = false;
		if (g_lCurMosgoldStorage > g_lCurMosgoldDispatchCon)
		{
			bDispatchMosgold = rand() % 100 < g_lCurMosgoldDispatchRate;
		}

		while (true)
		{
			if (lCirculateCount > 40000)
			{
				cbSysWin = !cbSysWin;
				lCirculateCount = 0;
	
				continue;
			}

			if (lDispatchMosgoldCount == 20000 && bDispatchMosgold)
			{
				bDispatchMosgold = !bDispatchMosgold;
				lDispatchMosgoldCount++;

				continue;
			}

			//派发扑克
			DispatchTableCard();

			//试探性判断
			//真人玩家得分
			SCORE lRealPlayerWinScore = CalculateScore(lBankerWinScore);

			//区域输赢(针对下注者)
			INT nAreaWin[AREA_MAX] = { 0 };

			BYTE cbResult = 0;

			//推断区域
			DeduceWinner(nAreaWin, cbResult);

			//派奖(开和)
			if (bDispatchMosgold)
			{
				if (cbResult == AREA_PING && g_lCurMosgoldStorage >= lRealPlayerWinScore)
				{
					break;
				}
			}
			else
			{
				if (cbResult == AREA_PING)
				{
					continue;
				}

				//真人玩家输钱
				if (cbSysWin == FALSE && lRealPlayerWinScore >= 0 && lKeyPlayerStorage >= lRealPlayerWinScore)
				{
					break;
				}
				else if (cbSysWin == TRUE && lRealPlayerWinScore <= 0)
				{
					break;
				}
			}

			lCirculateCount++;
			lDispatchMosgoldCount++;
		}
	}

	//系统和房间调试
	if (!bDebug)
	{
		//更新调试结果
		UpdateCtrlRes(userCtrlType);
	}
    else
    {
        // 发送消息
        m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_AREACTRL_TRACELOG);

        //区域调试列表
        POSITION pos = m_listAreaCtrl.GetHeadPosition();
        while(pos)
        {
            AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

			//发送结果
			CMD_S_AreaCtrlTraceLog areaCtrlResult;
			ZeroMemory(&areaCtrlResult, sizeof(areaCtrlResult));

			CopyMemory(&(areaCtrlResult.AreaCtrlTraceLog), &areactrl, sizeof(areactrl));

            // 发送消息
			m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_AREACTRL_TRACELOG, &areaCtrlResult, sizeof(areaCtrlResult));
        }
    }

	CMD_S_StatisticService StatisticService;
	ZeroMemory(&StatisticService, sizeof(StatisticService));

	StatisticService.lStatisticSysWin = g_lStatisticSysWin;
	StatisticService.lStatisticService = g_lStatisticService;
	StatisticService.lAreaLimitScore = m_lAreaLimitScore;
	StatisticService.wChairCount = m_wChairCount;

	// 发送消息
	m_pITableFrame->SendRoomData(NULL, SUB_S_STATISTIC_SERVICE, &StatisticService, sizeof(StatisticService));

	CMD_S_AnChou AnChou;
	ZeroMemory(&AnChou, sizeof(AnChou));

	AnChou.lCurAnChouRate = g_lCurAnChouRate;
	AnChou.lCurAnChouTotal = g_lCurAnChouTotal;

	//发送消息
	m_pITableFrame->SendRoomData(NULL, SUB_S_ANCHOU, &AnChou, sizeof(AnChou));

	CMD_S_Mosgold Mosgold;
	ZeroMemory(&Mosgold, sizeof(Mosgold));

	Mosgold.lCurMosgoldStorage = g_lCurMosgoldStorage;
	Mosgold.lCurMosgoldRate = g_lCurMosgoldRate;
	Mosgold.lCurMosgoldDispatchCon = g_lCurMosgoldDispatchCon;
	Mosgold.lCurMosgoldDispatchRate = g_lCurMosgoldDispatchRate;
	Mosgold.lCurMosgoldStorageRate = g_lCurMosgoldStorageRate;

	//发送消息
	m_pITableFrame->SendRoomData(NULL, SUB_S_MOSGOLD, &Mosgold, sizeof(Mosgold));

	//游戏记录
	tagServerGameRecord GameRecord;
	ZeroMemory(&GameRecord, sizeof(GameRecord));

	GameRecord.cbLong = (m_cbTableCardArray[0]&0x0F);
	GameRecord.cbHu = (m_cbTableCardArray[1]&0x0F);

	//简洁路单记录(最近的20局胜数结果，不清空)
	BYTE cbSimpleChartIndex = m_lCurGameCount % MAX_SIMPLE_CHART_COUNT;
	if (m_lCurGameCount < MAX_SIMPLE_CHART_COUNT)
	{
		m_SimpleChartArray[cbSimpleChartIndex] = GameRecord;
	}
	else
	{
		memmove(m_SimpleChartArray, &m_SimpleChartArray[1], sizeof(tagServerGameRecord)* (MAX_SIMPLE_CHART_COUNT - 1));
		m_SimpleChartArray[MAX_SIMPLE_CHART_COUNT - 1] = GameRecord;
	}

	//高级路单记录(显示最近 78局，可以左右拖动   满足100局清空)
	BYTE cbAdvancedChartIndex = m_lCurGameCount % MAX_ADVANCED_CHART_COUNT;
	if (m_lCurGameCount < MAX_ADVANCED_CHART_COUNT)
	{
		m_AdvancedChartArray[cbAdvancedChartIndex] = GameRecord;
	}
	else
	{
		memmove(m_AdvancedChartArray, &m_AdvancedChartArray[1], sizeof(tagServerGameRecord)* (MAX_ADVANCED_CHART_COUNT - 1));
		m_AdvancedChartArray[MAX_ADVANCED_CHART_COUNT - 1] = GameRecord;
	}

	//递增
	m_lCurGameCount++;

	return lBankerWinScore;
}
//读取配置
void CTableFrameSink::ReadConfigInformation(bool bReadFresh)
{
	//获取自定义配置
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
	ASSERT(pCustomConfig);

	m_bEnableBanker = pCustomConfig->bEnableBanker;
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_lBankerMAX = pCustomConfig->CustomGeneral.lBankerTime;
	m_lBankerAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lBankerScoreMAX = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_lBankerScoreAdd = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker =  true;

	//时间
	m_cbFreeTime = pCustomConfig->CustomGeneral.cbFreeTime;
	m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
	m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
	if(m_cbFreeTime < TIME_FREE	|| m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
	if(m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
	if(m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;

	//下注
	m_lAreaLimitScore = pCustomConfig->CustomGeneral.lAreaLimitScore;
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;

	LONGLONG lRobotBetMinCount = pCustomConfig->CustomAndroid.lRobotBetMinCount;
	LONGLONG lRobotBetMaxCount = pCustomConfig->CustomAndroid.lRobotBetMaxCount;
	lRobotBetMinCount = MAX_CHAIR / 2;
	lRobotBetMaxCount = MAX_CHAIR;

	LONGLONG lDVal = lRobotBetMaxCount - lRobotBetMinCount + 1;
	lDVal = (lDVal <= 0 ? 1 : lDVal);

	m_nMaxChipRobot = rand() % lDVal + lRobotBetMinCount;
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 8;
	m_lRobotAreaLimit = pCustomConfig->CustomAndroid.lRobotAreaLimit;
	m_nRobotListMaxCount = pCustomConfig->CustomAndroid.lRobotListMaxCount;
	pCustomConfig->CustomAndroid.lRobotMinBetTime = 15;
	pCustomConfig->CustomAndroid.lRobotMaxBetTime = 30;

    //重设机器人占位参数
    RectifyRobotOccupySeatPara();

	if (m_pITableFrame->GetTableID() == 0)
	{
		g_dwSysCtrlIncreaseIndex++;
		pCustomConfig->CustomGeneral.lConfigMosgoldDispatchCon = 0;

		g_lCurAnChouRate = pCustomConfig->CustomGeneral.lConfigAnChouRate;
		g_lCurMosgoldRate = pCustomConfig->CustomGeneral.lConfigMosgoldRate;
		g_lCurMosgoldStorage = pCustomConfig->CustomGeneral.lConfigMosgoldStorage;
		g_lCurMosgoldDispatchCon = pCustomConfig->CustomGeneral.lConfigMosgoldDispatchCon;
		g_lCurMosgoldDispatchRate = pCustomConfig->CustomGeneral.lConfigMosgoldDispatchRate;
		g_lCurMosgoldStorageRate = pCustomConfig->CustomGeneral.lConfigMosgoldStorageRate;

		//插入系统控制列表
		SYSCTRL sysctrl;
		ZeroMemory(&sysctrl, sizeof(sysctrl));

		SCORE lTempChouMosgoldStorage = pCustomConfig->CustomGeneral.lSysCtrlPlayerStorage * g_lCurMosgoldStorageRate / 100;

		sysctrl.dwCtrlIndex = g_dwSysCtrlIncreaseIndex;
		sysctrl.lSysCtrlConfigSysStorage = pCustomConfig->CustomGeneral.lSysCtrlSysStorage;;
		sysctrl.lSysCtrlConfigPlayerStorage = pCustomConfig->CustomGeneral.lSysCtrlPlayerStorage - lTempChouMosgoldStorage;
		sysctrl.lSysCtrlCurSysStorage = pCustomConfig->CustomGeneral.lSysCtrlSysStorage;;
		sysctrl.lSysCtrlCurPlayerStorage = pCustomConfig->CustomGeneral.lSysCtrlPlayerStorage - lTempChouMosgoldStorage;
		sysctrl.lSysCtrlParameterK = pCustomConfig->CustomGeneral.lSysCtrlParameterK;
		sysctrl.lSysCtrlSysStorResetRate = pCustomConfig->CustomGeneral.lSysCtrlSysStorResetRate;
		sysctrl.lSysCtrlResetCount = 0;
		sysctrl.lSysCtrlSysWin = 0;
		sysctrl.tmResetTime = CTime::GetCurrentTime();
		sysctrl.sysCtrlStatus = PROGRESSINGEX;
		sysctrl.lSysCtrlChouMosgoldStorage = lTempChouMosgoldStorage;

		//插入列表
		m_listSysCtrl.AddTail(sysctrl);

		//初始大奖库存增加
		g_lCurMosgoldStorage += lTempChouMosgoldStorage;
		g_lStatisticSysWin += lTempChouMosgoldStorage;
	}
}

//测试写信息
void CTableFrameSink::WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString )
{
	//设置语言区域
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	DWORD dwAttrib = GetFileAttributes(_T(".//龙虎斗日志目录"));
	if (INVALID_FILE_ATTRIBUTES == dwAttrib)
	{
		CreateDirectory(_T(".//龙虎斗日志目录"), NULL);
	}

	CTime time = CTime::GetCurrentTime();
	static CString strFile;
	if (strFile.IsEmpty())
		strFile.Format(_T(".//龙虎斗日志目录//[%s]日志%d-%d-%d-%02d%02d%02d.log"), m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	CStdioFile myFile;
	BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		if (myFile.GetLength() > 50 * 1024 * 1024)//50M分文件 
		{
			myFile.Close();
			strFile.Format(_T(".//龙虎斗日志目录//[%s]日志%d-%d-%d-%02d%02d%02d.log"), m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
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

void CTableFrameSink::SendUserBetInfo(DWORD dwCurAllScoreGameID)
{
	//变量定义
	CMD_S_SendUserBetInfo SendUserBetInfo;
	ZeroMemory(&SendUserBetInfo, sizeof(SendUserBetInfo));

	SendUserBetInfo.dwCurAllScoreGameID = dwCurAllScoreGameID;
	CopyMemory(SendUserBetInfo.lUserStartScore, m_lUserStartScore, sizeof(m_lUserStartScore));
	CopyMemory(SendUserBetInfo.lUserJettonScore, m_lUserInAllScore, sizeof(m_lUserInAllScore));
	CopyMemory(SendUserBetInfo.lAreaInAllScore, m_lAreaInAllScore, sizeof(m_lAreaInAllScore));

	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}

		if (pIServerUserItem->IsAndroidUser())
		{
			SendUserBetInfo.wOnLineAICount++;
		}
		else
		{
			SendUserBetInfo.wOnLineRealPlayerCount++;
		}

		SCORE	lUserInAllScore = 0;
		for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
		{
			lUserInAllScore += m_lUserInAllScore[i][wAreaIndex];
		}

		if (lUserInAllScore > 0 && pIServerUserItem->IsAndroidUser())
		{
			SendUserBetInfo.wInScoreAICount++;
		}
		else if (lUserInAllScore > 0 && !pIServerUserItem->IsAndroidUser())
		{
			SendUserBetInfo.wInScoreRealPlayerCount++;
		}
	}

	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem || pIServerUserItem->IsAndroidUser())
		{
			continue;
		}

		for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
		{
			SendUserBetInfo.lAreaRealPlayerInAllScore[wAreaIndex] += m_lUserInAllScore[i][wAreaIndex];
		}
	}

	//发送消息	
	m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, &SendUserBetInfo, sizeof(SendUserBetInfo));
}

//发送房间用户信息
void CTableFrameSink::SendRoomUserInfo()
{
	CMD_S_RoomUserInfo RoomUserInfo;
	memset(RoomUserInfo.dwGameID, INVALID_DWORD, sizeof(RoomUserInfo.dwGameID));
	ZeroMemory(RoomUserInfo.szNickName, sizeof(RoomUserInfo.szNickName));

	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}

		RoomUserInfo.dwGameID[i] = pIServerUserItem->GetGameID();
		CopyMemory(RoomUserInfo.szNickName[i], pIServerUserItem->GetNickName(), sizeof(RoomUserInfo.szNickName[i]));
	}

	//发送消息	
	m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_ROOM_USER_INFO, &RoomUserInfo, sizeof(RoomUserInfo));
}

//更新控制结果
void CTableFrameSink::UpdateCtrlRes(EM_CTRL_TYPE emCtrlType)
{
	//区域输赢(针对下注者)
	INT nAreaWin[AREA_MAX] = { 0 };

	BYTE cbResult = 0;

	//推断区域
	DeduceWinner(nAreaWin, cbResult);

	//AI或者系统当庄
	bool bAIOrSysBanker = ((m_wCurrentBanker == INVALID_CHAIR) || (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker) && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser()));

	switch (emCtrlType)
	{
	case ROOM_CTRL:
	{
		//查找房间调试
		POSITION pos = m_listRoomCtrl.GetHeadPosition();

		//当系统库存为负数,达到初始值的库存重置比例,重置库存
		bool bResetStorage = false;

		while (pos)
		{
			ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

			if (roomctrl.roomCtrlStatus == PROGRESSING)
			{
				for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
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
						
					//每个人定注数
					for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
					{
						//真人
						if (!pIServerUserItem->IsAndroidUser())
						{
							//该区域赢分
							if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore > 0)
							{
								//开平从大奖库存里面扣除
								if (cbResult == AREA_PING)
								{
									g_lCurMosgoldStorage -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);

									g_lStatisticSysWin -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);
								}
								else
								{
									roomctrl.lRoomCtrlCurPlayerStorage -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);

									roomctrl.lRoomCtrlSysWin -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
									g_lStatisticSysWin -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
								}
							}
							//该区域输分
							else if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore < 0)
							{
								roomctrl.lRoomCtrlCurSysStorage -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);

								roomctrl.lRoomCtrlSysWin += (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
								g_lStatisticSysWin += (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
							}
						}
						//AI
						else
						{
							//该区域赢分
							if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore > 0)
							{
								//开平从大奖库存里面扣除
								//if (cbResult == AREA_PING)
								//{
								//	g_lCurMosgoldStorage -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);

								//	g_lStatisticSysWin -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);
								//}
								//else
								{
									roomctrl.lRoomCtrlCurSysStorage -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);

									roomctrl.lRoomCtrlSysWin += (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
									g_lStatisticSysWin += (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
								}
							}
							//该区域输分
							else if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore < 0)
							{
								roomctrl.lRoomCtrlCurPlayerStorage -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);

								roomctrl.lRoomCtrlSysWin -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
								g_lStatisticSysWin -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
							}
						}
					}
				}

				//统计税收
				for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
				{
					//获取用户
					IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem == NULL || pIServerUserItem->IsAndroidUser())
					{
						continue;
					}

					//每个人定注数
					for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
					{
						//真人统计税收
						g_lStatisticService += m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue;
					}
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

		//查找系统调试
		POSITION pos = m_listSysCtrl.GetHeadPosition();

		while (pos)
		{
			SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

			if (sysctrl.sysCtrlStatus == PROGRESSINGEX)
			{
				for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
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

					//每个人定注数
					for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
					{
						//真人
						if (!pIServerUserItem->IsAndroidUser())
						{
							//该区域赢分
							if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore > 0)
							{
								//开平从大奖库存里面扣除
								if (cbResult == AREA_PING)
								{
									g_lCurMosgoldStorage -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);

									g_lStatisticSysWin -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);
								}
								else
								{
									sysctrl.lSysCtrlCurPlayerStorage -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);

									sysctrl.lSysCtrlSysWin -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
									g_lStatisticSysWin -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
								}
							}
							//该区域输分
							else if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore < 0)
							{
								sysctrl.lSysCtrlCurSysStorage -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);

								sysctrl.lSysCtrlSysWin += (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
								g_lStatisticSysWin += (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
							}
						}
						//AI
						else
						{
							//该区域赢分
							if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore > 0)
							{
								//开平从大奖库存里面扣除
								//if (cbResult == AREA_PING)
								//{
								//	g_lCurMosgoldStorage -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);

								//	g_lStatisticSysWin -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);
								//}
								//else
								{
									sysctrl.lSysCtrlCurSysStorage -= (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);

									sysctrl.lSysCtrlSysWin += (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
									g_lStatisticSysWin += (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou - m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
								}
							}
							//该区域输分
							else if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore < 0)
							{
								sysctrl.lSysCtrlCurPlayerStorage -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);

								sysctrl.lSysCtrlSysWin -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
								g_lStatisticSysWin -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
							}
						}	
					}
				}

				//统计税收
				for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
				{
					//获取用户
					IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem == NULL || pIServerUserItem->IsAndroidUser())
					{
						continue;
					}

					//每个人定注数
					for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
					{
						//真人统计税收
						g_lStatisticService += m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue;
					}
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

					//上次没抽大奖
					if (sysctrl.lSysCtrlChouMosgoldStorage == 0)
					{
						SCORE lTempChouMosgoldStorage = sysctrl.lSysCtrlConfigPlayerStorage * g_lCurMosgoldStorageRate / 100;
						sysctrl.lSysCtrlConfigPlayerStorage -= lTempChouMosgoldStorage;
						sysctrl.lSysCtrlChouMosgoldStorage = lTempChouMosgoldStorage;
					}

					//大奖库存增加
					g_lCurMosgoldStorage += sysctrl.lSysCtrlChouMosgoldStorage;
					g_lStatisticSysWin += sysctrl.lSysCtrlChouMosgoldStorage;

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

	return posKey;
}

//发送消息
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	CMD_S_SysTip SysTip;
	ZeroMemory(&SysTip, sizeof(SysTip));

	CString strTip;
	strTip.Format(TEXT("%s"), pszTipMsg);

	CopyMemory(SysTip.szTip, strTip, sizeof(TCHAR)* strTip.GetLength());

	if (wChairID == INVALID_CHAIR)
	{
		//游戏玩家
		for (WORD i = 0; i < m_wChairCount; ++i)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;
			if (!pIServerUserItem->IsClientReady()) continue;

			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SYSTIP, &SysTip, sizeof(SysTip));
		}
	}
	else
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem != NULL && pIServerUserItem->IsClientReady())
		{
			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SYSTIP, &SysTip, sizeof(SysTip));
		}
	}
}

//重设机器人占位参数
void CTableFrameSink::RectifyRobotOccupySeatPara()
{
	//获取自定义配置
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
	ASSERT(pCustomConfig);

	//m_bEnableRobotOccupySeat = (pCustomConfig->CustomAndroid.nEnableRobotOccupySeat == TRUE ? true : false);

	//LONGLONG lRobotOccupySeatTableMin = pCustomConfig->CustomAndroid.lRobotOccupySeatTableMin;
	//LONGLONG lRobotOccupySeatTableMax = pCustomConfig->CustomAndroid.lRobotOccupySeatTableMax;
	//m_nRobotOccupySeatTable = rand() % (lRobotOccupySeatTableMax - lRobotOccupySeatTableMin + 1) + lRobotOccupySeatTableMin;

	//LONGLONG lRobotQuitSeatTurnMin = pCustomConfig->CustomAndroid.lRobotQuitSeatTurnMin;
	//LONGLONG lRobotQuitSeatTurnMax = pCustomConfig->CustomAndroid.lRobotQuitSeatTurnMax;
	//m_nRobotQuitSeatTurn = rand() % (lRobotQuitSeatTurnMax - lRobotQuitSeatTurnMin + 1) + lRobotQuitSeatTurnMin;

	//读取新配置
	m_bEnableRobotOccupySeat = true;
	m_nRobotQuitSeatTurn = RANDVAL(m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatTurnCountMin, m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatTurnCountMax);

	//机器人占位人数随着真人人数变化
	m_nRobotOccupySeatTable = GetRandRobotOccupySeatCount();
	m_nRobotOccupySeatTable = (m_nRobotOccupySeatTable == INVALID_DWORD ? (rand() % MAX_OCCUPY_SEAT_COUNT) : m_nRobotOccupySeatTable);
}

//除去庄家和占座玩家和自己的有效玩家
bool CTableFrameSink::FilterUser(WORD wMeChairID, WORD wChairID)
{
    //过滤庄家和占座玩家和自己
    if(wMeChairID == wChairID)
    {
        return false;
    }

    if(m_wCurrentBanker != INVALID_CHAIR && m_wCurrentBanker == wChairID)
    {
        return false;
    }

    if(!m_pITableFrame->GetTableUserItem(wChairID))
    {
        return false;
    }

    for(WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
    {
        if(m_wOccupySeatChairIDArray[i] != INVALID_CHAIR && m_wOccupySeatChairIDArray[i] == wChairID)
        {
            return false;
        }
    }

    return true;
}

//获取玩家调试类型
EM_CTRL_TYPE CTableFrameSink::GetUserCtrlType()
{
	//（优先级，系统=区域>房间>收放表）
	//默认收放表调试
	EM_CTRL_TYPE ctrlType = SYS_CTRL;

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

	return SYS_CTRL;
}

//读取AIConfig
void CTableFrameSink::ReadAIConfig()
{
	//屏蔽读取
	return;

	BYTE cbValid = ReadFileValue(TEXT("GeneralConfig"), TEXT("Valid"), 0);

	if (cbValid == FALSE)
	{
		return;
	}

	//读取通用配置
	m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[0][0] = ReadFileValue(TEXT("AIOccupySeatConfig"), TEXT("OC_RPLAYER_MIN0"), 0);
	m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[2][1] = ReadFileValue(TEXT("AIOccupySeatConfig"), TEXT("OC_RPLAYER_MAX2"), 0);
	m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][0] = ReadFileValue(TEXT("AIOccupySeatConfig"), TEXT("OC_RPLAYER_MIN1"), 0);
	m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][1] = ReadFileValue(TEXT("AIOccupySeatConfig"), TEXT("OC_RPLAYER_MAX1"), 0);

	for (WORD i = 0; i < AI_OCCUPYSEAT_CONFIGCOUNT; i++)
	{
		CString strCountMin;
		strCountMin.Format(TEXT("OC_COUNT_MIN%d"), i);

		CString strCountMax;
		strCountMax.Format(TEXT("OC_COUNT_MAX%d"), i);

		m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatCountRange[i][0] = ReadFileValue(TEXT("AIOccupySeatConfig"), strCountMin, 0);
		m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatCountRange[i][1] = ReadFileValue(TEXT("AIOccupySeatConfig"), strCountMax, 0);
	}

	m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatTurnCountMin = ReadFileValue(TEXT("AIOccupySeatConfig"), TEXT("OC_TURNCOUNT_MIN"), 0);
	m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatTurnCountMax = ReadFileValue(TEXT("AIOccupySeatConfig"), TEXT("OC_TURNCOUNT_MAX"), 0);

	m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[0][0] = ReadFileValue(TEXT("AIRequestBankerConfig"), TEXT("RB_RPLAYER_MIN0"), 0);
	m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[2][1] = ReadFileValue(TEXT("AIRequestBankerConfig"), TEXT("RB_RPLAYER_MAX2"), 0);
	m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][0] = ReadFileValue(TEXT("AIRequestBankerConfig"), TEXT("RB_RPLAYER_MIN1"), 0);
	m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][1] = ReadFileValue(TEXT("AIRequestBankerConfig"), TEXT("RB_RPLAYER_MAX1"), 0);

	for (WORD i = 0; i < AI_REQUESTBANKER_CONFIGCOUNT; i++)
	{
		CString strCountMin;
		strCountMin.Format(TEXT("RB_COUNT_MIN%d"), i);

		CString strCountMax;
		strCountMax.Format(TEXT("RB_COUNT_MAX%d"), i);

		m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerCountRange[i][0] = ReadFileValue(TEXT("AIRequestBankerConfig"), strCountMin, 0);
		m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerCountRange[i][1] = ReadFileValue(TEXT("AIRequestBankerConfig"), strCountMax, 0);
	}

	m_CustomAIConfig.AIRequestBankerConfig.wAICRChange = ReadFileValue(TEXT("AIRequestBankerConfig"), TEXT("CRB_CHANCE"), 0);
	m_CustomAIConfig.AIRequestBankerConfig.wAICRChangeContainRealPlayer = ReadFileValue(TEXT("AIRequestBankerConfig"), TEXT("CRB_CHANCE_CONTAINRP"), 0);
	m_CustomAIConfig.AIRequestBankerConfig.wAIQBChange = ReadFileValue(TEXT("AIRequestBankerConfig"), TEXT("QB_CHANCE"), 0);

	for (WORD i = 0; i < AI_PLACEJETTON_CONFIGCOUNT; i++)
	{
		CString strCountDownPerRangeMin;
		strCountDownPerRangeMin.Format(TEXT("PJ_COUNTDOWN_MIN%d"), i);

		CString strCountDownPerRangeMax;
		strCountDownPerRangeMax.Format(TEXT("PJ_COUNTDOWN_MAX%d"), i);

		m_CustomAIConfig.AIPlaceJettonConfig.wCountDownPerRange[i][0] = ReadFileValue(TEXT("AIPlaceJettonConfig"), strCountDownPerRangeMin, 0);
		m_CustomAIConfig.AIPlaceJettonConfig.wCountDownPerRange[i][1] = ReadFileValue(TEXT("AIPlaceJettonConfig"), strCountDownPerRangeMax, 0);

		CString strLitRangeMin;
		strLitRangeMin.Format(TEXT("PJ_LITRANGE_MIN%d"), i);

		CString strLitRangeMax;
		strLitRangeMax.Format(TEXT("PJ_LITRANGE_MAX%d"), i);

		m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonLimitRange[i][0] = ReadFileValue(TEXT("AIPlaceJettonConfig"), strLitRangeMin, 0);
		m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonLimitRange[i][1] = ReadFileValue(TEXT("AIPlaceJettonConfig"), strLitRangeMax, 0);
	}

	for (WORD i = 0; i < MAX_CHIP_COUNT; i++)
	{
		CString strPlaceJettonRate;
		strPlaceJettonRate.Format(TEXT("PJ_RATE%d"), i);

		m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[i] = ReadFileValue(TEXT("AIPlaceJettonConfig"), strPlaceJettonRate, 0);
	}

	//机器人存取
	m_CustomAIConfig.AIBankOperateConfig.wVerifyTurn = ReadFileValue(TEXT("AIBankOperateConfig"), TEXT("wVerifyTurn"), 0);

	for (WORD i = 0; i < AI_BANKOPERATE_CONFIGCOUNT; i++)
	{
		CString strAICarryScoreRangeMin;
		strAICarryScoreRangeMin.Format(TEXT("lAICarryScoreRange%d_0"), i);

		CString strAICarryScoreRangeMax;
		strAICarryScoreRangeMax.Format(TEXT("lAICarryScoreRange%d_1"), i);

		CString strSave;
		strSave.Format(TEXT("cbSave%d"), i);

		CString strAIBankOperateChange;
		strAIBankOperateChange.Format(TEXT("wAIBankOperateChange%d"), i);

		CString strAIBankOperateScoreRangeMin;
		strAIBankOperateScoreRangeMin.Format(TEXT("lAIBankOperateScoreRange%d_0"), i);

		CString strAIBankOperateScoreRangeMax;
		strAIBankOperateScoreRangeMax.Format(TEXT("lAIBankOperateScoreRange%d_1"), i);

		m_CustomAIConfig.AIBankOperateConfig.lAICarryScoreRange[i][0] = ReadFileValue(TEXT("AIBankOperateConfig"), strAICarryScoreRangeMin, 0);
		m_CustomAIConfig.AIBankOperateConfig.lAICarryScoreRange[i][1] = ReadFileValue(TEXT("AIBankOperateConfig"), strAICarryScoreRangeMax, 0);

		m_CustomAIConfig.AIBankOperateConfig.cbSave[i] = ReadFileValue(TEXT("AIBankOperateConfig"), strSave, 0);
		m_CustomAIConfig.AIBankOperateConfig.wAIBankOperateChange[i] = ReadFileValue(TEXT("AIBankOperateConfig"), strAIBankOperateChange, 0);

		m_CustomAIConfig.AIBankOperateConfig.lAIBankOperateScoreRange[i][0] = ReadFileValue(TEXT("AIBankOperateConfig"), strAIBankOperateScoreRangeMin, 0);
		m_CustomAIConfig.AIBankOperateConfig.lAIBankOperateScoreRange[i][1] = ReadFileValue(TEXT("AIBankOperateConfig"), strAIBankOperateScoreRangeMax, 0);
	}

	return;
}

//工作目录
bool CTableFrameSink::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//模块路径
	TCHAR szModulePath[MAX_PATH] = TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(), szModulePath, CountArray(szModulePath));

	//分析文件
	for (INT i = lstrlen(szModulePath); i >= 0; i--)
	{
		if (szModulePath[i] == TEXT('\\'))
		{
			szModulePath[i] = 0;
			break;
		}
	}

	//设置结果
	ASSERT(szModulePath[0] != 0);
	lstrcpyn(szWorkDirectory, szModulePath, wBufferCount);

	return true;
}

//读取数据
SCORE CTableFrameSink::ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//构造路径
	TCHAR szServerInfoPath[MAX_PATH] = TEXT("");
	_sntprintf_s(szServerInfoPath, CountArray(szServerInfoPath), TEXT("%s\\AIConfig\\AIConfig_SERVERID%d.ini"), szDirectory, m_pGameServiceOption->wServerID);

	//读取数据
	UINT nValue = GetPrivateProfileInt(pszKeyName, pszItemName, nDefault, szServerInfoPath);

	return (SCORE)nValue;
}

//获取真人玩家个数
LONG CTableFrameSink::GetRealPlayerCount()
{
	LONG lRealPlayerCount = 0;

	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pIServerUserItem)
		{
			continue;
		}

		if (!pIServerUserItem->IsAndroidUser())
		{
			lRealPlayerCount++;
		}
	}

	return lRealPlayerCount;
}

//获取机器人占位人数
LONG CTableFrameSink::GetRandRobotOccupySeatCount()
{
	//获取真人玩家个数
	LONG lRealPlayerCount = GetRealPlayerCount();

	//数据大小校验
	DWORD lTempArray[4] = { m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[2][1]
	};

	//获取范围
	if (lRealPlayerCount > lTempArray[0])
	{
		return RANDVAL(m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatCountRange[0][0], m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatCountRange[0][1]);
	}
	else if (lRealPlayerCount > lTempArray[2] && lRealPlayerCount <= lTempArray[1])
	{
		return RANDVAL(m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatCountRange[1][0], m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatCountRange[1][1]);
	}
	else if (lRealPlayerCount >= 0 && lRealPlayerCount <= lTempArray[3])
	{
		return RANDVAL(m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatCountRange[2][0], m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatCountRange[2][1]);
	}

	return INVALID_DWORD;
}

//获取机器人上庄人数
LONG CTableFrameSink::GetRandRobotRequestBankerCount()
{
	//获取真人玩家个数
	LONG lRealPlayerCount = GetRealPlayerCount();

	//数据大小校验
	DWORD lTempArray[4] = { m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[2][1]
	};

	//获取范围
	if (lRealPlayerCount > lTempArray[0])
	{
		return RANDVAL(m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerCountRange[0][0], m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerCountRange[0][1]);
	}
	else if (lRealPlayerCount > lTempArray[2] && lRealPlayerCount <= lTempArray[1])
	{
		return RANDVAL(m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerCountRange[1][0], m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerCountRange[1][1]);
	}
	else if (lRealPlayerCount >= 0 && lRealPlayerCount <= lTempArray[3])
	{
		return RANDVAL(m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerCountRange[2][0], m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerCountRange[2][1]);
	}

	return INVALID_DWORD;
}

//获取两个数之间的随机数
DWORD CTableFrameSink::GetRandVal(DWORD dwVal1, DWORD dwVal2)
{
	if (dwVal1 == dwVal2)
	{
		return dwVal1;
	}

	if (dwVal1 > dwVal2)
	{
		return INVALID_DWORD;
	}

	return rand() % dwVal2 + dwVal1;
}

//重设机器人上庄参数
void CTableFrameSink::RectifyRobotRequestBankerPara()
{
	m_nRobotRequestBankerCount = GetRandRobotRequestBankerCount();
	m_nRobotRequestBankerCount = (m_nRobotRequestBankerCount == INVALID_DWORD ? 4 : m_nRobotRequestBankerCount);

	m_lRobotLostScoreLimitQB = m_CustomAIConfig.AIRequestBankerConfig.lAILostScoreLimitQB;
	m_wRobotMaxSerialTurn = m_CustomAIConfig.AIRequestBankerConfig.wMaxSerialTurn;
}

//获取申请列表机器人数目
LONG CTableFrameSink::GetApplyUserArrayRobotCount()
{
	LONG lApplyUserArrayRobotCount = 0;
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_ApplyUserArray[i]);
		if (!pIServerUserItem)
		{
			continue;
		}

		if (pIServerUserItem->IsAndroidUser())
		{
			lApplyUserArrayRobotCount++;
		}
	}

	return lApplyUserArrayRobotCount;
}

//校验机器人下注是否有效
bool CTableFrameSink::IsRobotPlaceJettonValid(BYTE cbJettonArea, SCORE lJettonScore, WORD wChairID)
{
	CTime tmCur = CTime::GetCurrentTime();
	CTimeSpan tmSpan = tmCur - m_tmStartPlaceJetton;
	LONGLONG lElaseTm = tmSpan.GetTotalSeconds();

	//获取下注额比值
	LONG lJettonLitPercent = GetPlaceJettonLitPercent(lElaseTm);

	//剩余该区域下注最大分
	SCORE dRemainMaxPlayerScore = GetMaxPlayerScore(cbJettonArea, wChairID);

	if ((m_lAreaInAllScore[cbJettonArea] + lJettonScore) > (dRemainMaxPlayerScore * lJettonLitPercent / 100))
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		CString strInfo;
		strInfo.Format(TEXT("cbJettonArea=%d,lAreaInAllScore0=%I64d,lAreaInAllScore1=%I64d,lAreaInAllScore2=%I64d, lJettonScore=%I64d, dRemainMaxPlayerScore=%I64d, lJettonLitPercent=%d, CarryScore=%I64d\n "), cbJettonArea, m_lAreaInAllScore[AREA_LONG], m_lAreaInAllScore[AREA_PING], m_lAreaInAllScore[AREA_HU], lJettonScore,
			dRemainMaxPlayerScore, lJettonLitPercent, pIServerUserItem->GetUserScore());
		//CTraceService::TraceString(strInfo, TraceLevel_Exception);

		CString stTrace;
		CTime Time(CTime::GetCurrentTime());
		stTrace.Format(TEXT("%d-%d %d:%d:%d |  m_cbFreeTime = %d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_cbFreeTime);
		//WriteInfo(TEXT("DragonTigerBattlePlace.log"), stTrace);
		//WriteInfo(TEXT("DragonTigerBattlePlace.log"), strInfo);

		return false;
	}

	return true;
}

//获取下注额比值
LONG CTableFrameSink::GetPlaceJettonLitPercent(LONGLONG lElaseTm)
{
	for (int i = AI_PLACEJETTON_CONFIGCOUNT - 1; i >= 0; i--)
	{
		WORD wCountDownPerRangMin = m_CustomAIConfig.AIPlaceJettonConfig.wCountDownPerRange[i][0];
		WORD wCountDownPerRangMax = m_CustomAIConfig.AIPlaceJettonConfig.wCountDownPerRange[i][1];

		if (lElaseTm >= m_cbBetTime * wCountDownPerRangMin / 100 && lElaseTm <= m_cbBetTime * wCountDownPerRangMax / 100)
		{
			DWORD wPlaceJettonLimitRangeMin = m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonLimitRange[i][0];
			DWORD wPlaceJettonLimitRangeMax = m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonLimitRange[i][1];

			return RANDVAL(wPlaceJettonLimitRangeMin, wPlaceJettonLimitRangeMax);
		}

	}

	return 0;
}

//房间同时存在AI和真人玩家
bool CTableFrameSink::IsContainAIAndRealPlayer(bool bCurPlaceAndroidUser)
{
	//真人坐庄，		没有机器人下注		 不抽
	//系统坐庄，		没有真人下注         不抽
	//机器人坐庄，		没有真人下注         不抽
	//其他情况都抽暗抽和彩金

	WORD wRealPlayerInScoreCount = 0;
	WORD wAIInScoreCount = 0;

	if (bCurPlaceAndroidUser)
	{
		wAIInScoreCount++;
	}
	else
	{
		wRealPlayerInScoreCount++;
	}

	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		SCORE lUserAllBet = 0;
		for (int nAreaIndex = 0; nAreaIndex<AREA_MAX; nAreaIndex++)
		{
			lUserAllBet += m_lUserInAllScore[wChairID][nAreaIndex];
		}

		if (lUserAllBet <= 0)
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

	bool bRealPlayerBanker = (m_wCurrentBanker != INVALID_CHAIR && !m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser());
	bool bSysBanker = (m_wCurrentBanker == INVALID_CHAIR);
	bool bAIBanker = (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser());
	
	return !((bRealPlayerBanker && wAIInScoreCount <= 0) || (bSysBanker && wRealPlayerInScoreCount <= 0) || (bAIBanker && wRealPlayerInScoreCount <= 0));
}

//区域控制有效
bool CTableFrameSink::IsAreaCtrlSysValid()
{
	//真人坐庄，真人玩的情况
	WORD wAIInScoreCount = 0;

	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		SCORE lUserAllBet = 0;
		for (int nAreaIndex = 0; nAreaIndex<AREA_MAX; nAreaIndex++)
		{
			lUserAllBet += m_lUserInAllScore[wChairID][nAreaIndex];
		}

		if (lUserAllBet <= 0)
		{
			continue;
		}

		if (pIServerUserItem->IsAndroidUser())
		{
			wAIInScoreCount++;
		}
	}

	if (m_wCurrentBanker != INVALID_CHAIR && !m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() && wAIInScoreCount <= 0)
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

//统计区域控制列表
SCORE CTableFrameSink::CalListAreaCtrlSysWin()
{
	SCORE lAreaCtrlSysWin = 0;

	POSITION pos = m_listAreaCtrl.GetHeadPosition();
	while (pos)
	{
		AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

		lAreaCtrlSysWin += areactrl.lAreaCtrlSysWin;
	}

	return lAreaCtrlSysWin;
}

//是否大联盟金币约战房间
bool CTableFrameSink::IsGroupPersonalRoom()
{
	return (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0) && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule);
}

//获取大联盟房间规则
void CTableFrameSink::GetGroupPersonalRoomRule()
{
	if (!IsGroupPersonalRoom())
	{
		return;
	}

	//获取大联盟金币约战规则
	BYTE *pGameRule = m_pITableFrame->GetGameRule();

	if (pGameRule[0] != 1)
	{
		return;
	}

	//个人下注限制(8字节)
	m_lUserLimitScore = *((LONGLONG *)&pGameRule[1]);

	//上庄条件(8字节)
	m_lApplyBankerCondition = *((LONGLONG *)&pGameRule[1 + sizeof(LONGLONG)]);

	//上庄局数(2字节)
	m_lBankerMAX = *((WORD *)&pGameRule[1 + sizeof(LONGLONG)* 2]);
}

//////////////////////////////////////////////////////////////////////////////////
