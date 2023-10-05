#include "StdAfx.h"
#include "DlgCustomRule.h"
#include "TableFrameSink.h"
#include <locale>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////
//常量定义
#define SEND_COUNT					100									//发送次数

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//时间定义
#define IDI_FREE					1									//空闲时间
#define IDI_PLACE_JETTON			2									//下注时间
#define IDI_GAME_END				3									//结束时间

#define TIME_FREE					5									//空闲时间
#define TIME_PLACE_JETTON			10									//下注时间
#define TIME_GAME_END				20									//结束时间
//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;				//游戏人数

//////////////////////////////////////////////////////////////////////////
//构造函数
CTableFrameSink::CTableFrameSink()
{
	//起始分数
	ZeroMemory(m_lUserStartScore,sizeof(m_lUserStartScore));

	//总下注数
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//个人下注
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//上局信息
	ZeroMemory(m_lLastAllJettonPlace, sizeof(m_lLastAllJettonPlace));
	ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

	//控制变量
	m_cbWinSideControl=0;
	m_nSendCardCount=0;
	m_bControl=false;

	m_bWinRed = false;
	m_bWinBlack = false;
	m_bWinPoker = false;
	ZeroMemory(m_cbPokerType, sizeof(m_cbPokerType));
	//玩家成绩	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//扑克信息
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//状态变量
	m_dwJettonTime=0L;
	m_bExchangeBanker=true;
	m_wAddTime=0;
	CopyMemory(m_szControlName,TEXT("无人"),sizeof(m_szControlName));

	//庄家信息
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker=INVALID_CHAIR;
	m_wOfflineBanker = INVALID_CHAIR;
	m_wBankerTime=0;
	m_lPlayerBankerMAX = 0l;
	m_lBankerWinScore=0L;		
	m_lBankerCurGameScore=0L;
	m_bEnableSysBanker=true;
	m_lBankerScore = 0l;

	//记录变量
	ZeroMemory(m_cbRBHistory, sizeof(m_cbRBHistory));
	ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));
	m_nRecordFirst=0;
	m_nRecordLast=0;
	m_dwRecordCount=0;

	m_nRBCurrentColumn = 0;
	m_nRBCurrentRow = 0;

	//机器人控制
	m_nChipRobotCount = 0;
	m_nRobotListMaxCount =0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	ZeroMemory(&m_occupyseatConfig, sizeof(m_occupyseatConfig));

	//占位
	for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
	{
		m_cbDeskChairID[i] = INVALID_BYTE;
	}

	//服务控制
	m_hInst = NULL;
	m_pServerContro = NULL;
	m_hInst = LoadLibrary(TEXT("RedBlackBattleServerDebug.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
}

VOID CTableFrameSink::Release()
{
	if (m_pServerContro)
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

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();

	ASSERT(m_pGameServiceOption!=NULL);
	ASSERT(m_pGameServiceAttrib!=NULL);
	//房间名称
	memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));

	ReadConfigInformation();

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	m_wOfflineBanker = INVALID_CHAIR;

	//总下注数
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//个人下注
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//本局最后一注
	ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

	m_bWinRed = false;
	m_bWinBlack = false;
	m_bWinPoker = false;
	ZeroMemory(m_cbPokerType, sizeof(m_cbPokerType));
	//玩家成绩	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//机器人控制
	m_nChipRobotCount = 0;
	m_bControl=false;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	
	m_lBankerScore = 0l;

	return;
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
		return __max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore, 0L);
	}
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//当庄家分数在空闲时间变动时(即庄家进行了存取款)校验庄家的上庄条件
	if(wChairID == m_wCurrentBanker && m_pITableFrame->GetGameStatus()==GAME_SCENE_FREE)
	{
		ChangeBanker(false);
	}
	
	//校验是否满足占位最低条件
	if (pIServerUserItem->GetUserScore() < m_occupyseatConfig.lForceStandUpCondition)
	{
		bool bvalid = false;
		//校验数据

		WORD wDeskChairID = INVALID_BYTE;
		for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
		{
			if (m_cbDeskChairID[i] == wChairID)
			{
				bvalid = true;
				wDeskChairID = i;
				//重置无效
				m_cbDeskChairID[i] = INVALID_BYTE;

				break;
			}
		}

		if (bvalid && wDeskChairID != INVALID_BYTE)
		{
			CMD_S_GameDown GameDown;
			ZeroMemory(&GameDown, sizeof(GameDown));
			GameDown.wChairID = wChairID;
			GameDown.wDeskChairID = wDeskChairID + 1;

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
		}

	}

	return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置随机种子
	srand(GetTickCount());

	//发送库存消息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if ( pIServerUserItem == NULL )
			continue;

		if( CUserRight::IsGameDebugUser(m_pITableFrame->GetTableUserItem(i)->GetUserRight()))
		{
			CString strInfo;
			strInfo.Format(TEXT("当前库存：%I64d"), m_lStorageCurrent);

			m_pITableFrame->SendGameMessage(pIServerUserItem,strInfo,SMT_CHAT);
		}	
	}
	
	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//获取庄家
	IServerUserItem* pIBankerServerUserItem = NULL;
	if ( m_wCurrentBanker == INVALID_CHAIR )
	{
		m_lBankerScore = 1000000000;
	}
	else
	{
		IServerUserItem* pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if ( pIBankerServerUserItem != NULL )
		{
			m_lBankerScore = pIBankerServerUserItem->GetUserScore();
		}
	}

	//设置变量
	GameStart.cbTimeLeave=m_nPlaceJettonTime;
	GameStart.wBankerUser=m_wCurrentBanker;
	GameStart.lBankerScore=m_lBankerScore;
	GameStart.lStorageStart = m_lStorageCurrent;	

	//下注机器人数量
	int nChipRobotCount = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}

	GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);
	
	nChipRobotCount = 0;
	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++) 
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray.GetAt(i));
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}

	if(nChipRobotCount > 0)
		GameStart.nAndriodApplyCount=nChipRobotCount-1;

	//机器人控制
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	//旁观玩家
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	

	//游戏玩家
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//设置积分
		GameStart.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*MAX_RATE);

		m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	
	}

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束	
		{
			//游戏作弊
			if( m_pServerContro != NULL && m_pServerContro->NeedControl() && m_pServerContro->ControlResult(m_cbTableCardArray))
			{
				//获取控制目标牌型
				m_bControl=true;
				m_pServerContro->CompleteControl();
			}
			else
			{	
				
				//系统坐庄和机器人坐庄优化库存
				if ((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
				 || (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true))
				{
					StorageOptimize();
				}	
				else if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false)
				{
					//循环计数
					LONGLONG lCirculateCount = 0L;
					while(true)
					{
						if (lCirculateCount > 10000)
						{
							break;
						}

						//真人玩家得分
						LONGLONG lRealPlayerWinScore = CalRealPlayerBankerScore();

						//真人玩家输钱
						if (lRealPlayerWinScore <= 0)
						{
							break;
						}
						else		//真人玩家赢钱
						{
							if (m_lStorageCurrent - lRealPlayerWinScore  <= 0)
							{
								DispatchTableCard();
								lCirculateCount++;
								continue;
							}

							//检验最大赔付比例
							LONGLONG lStorageCurrent = m_lStorageCurrent - lRealPlayerWinScore;
							double llCompensateRadio = (double)((double)30 / (double)100);
							if (m_lStorageCurrent * (1 - llCompensateRadio) >= lStorageCurrent)
							{
								DispatchTableCard();
								lCirculateCount++;
								continue;
							}

							//判断条件
							if (((m_lStorageCurrent > m_lStorageMax1) && (rand()%100 < m_lStorageMul1))
								|| ((m_lStorageCurrent > m_lStorageMax2) && (rand()%100 < m_lStorageMul2))
								|| rand() % 100 < 45)
							{
								break;
							}
							else
							{
								DispatchTableCard();
								lCirculateCount++;
								continue;
							}
						}
					}
					
				}
			}

			//保存上局总注
			CopyMemory(m_lLastAllJettonPlace, m_lUserJettonScore, sizeof(m_lLastAllJettonPlace));
			//清空本局最后一注
			ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

			//计算分数
			LONGLONG lBankerWinScore=CalculateScore();

			//递增次数
			m_wBankerTime++;

			//结束消息
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//占位玩家成绩
			for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
			{
				if (m_cbDeskChairID[i] == INVALID_BYTE) continue;
				IServerUserItem *pIOccupySeatServerUserItem = m_pITableFrame->GetTableUserItem(m_cbDeskChairID[i]);
				if (!pIOccupySeatServerUserItem) continue;

				GameEnd.lDeskScore[i] = m_lUserWinScore[m_cbDeskChairID[i]] - m_lUserRevenue[m_cbDeskChairID[i]];
				GameEnd.lDeskScoreT[i] = m_lUserWinScore[m_cbDeskChairID[i]];
			}

			GameEnd.bWinRed = m_bWinRed;
			GameEnd.bWinBlack = m_bWinBlack;
			GameEnd.bWinPoker = m_bWinPoker;
			GameEnd.wCurrentBanker = m_wCurrentBanker;
			//庄家信息
			GameEnd.nBankerTime = m_wBankerTime;
			GameEnd.lBankerTotallScore=m_lBankerWinScore;
			GameEnd.lBankerScore=lBankerWinScore;

			//扑克信息
			CopyMemory(GameEnd.cbPokerType, m_cbPokerType, sizeof(m_cbPokerType));
			CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));

			//发送积分
			GameEnd.cbTimeLeave=m_nGameEndTime;	
			for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
				if ( pIServerUserItem == NULL ) continue;

				//设置成绩 收取了税收
				GameEnd.lUserScore=m_lUserWinScore[wUserIndex] - m_lUserRevenue[wUserIndex];
				//原始得分 没有收取税收
				GameEnd.lUserScoreT=m_lUserWinScore[wUserIndex];
				//返还积分
				GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

				//发送消息					
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}
			return true;
		}
	case GER_USER_LEAVE:		//用户离开
	case GER_NETWORK_ERROR:
		{
			//只要离开，就清除用户记录
			m_tagUserRecord[wChairID].RemoveAll();
			//闲家判断
			if (m_wCurrentBanker!=wChairID)
			{
				//变量定义
				LONGLONG lRevenue=0;

				//是否开牌
				if (m_pITableFrame->GetGameStatus() == GAME_SCENE_PLACE_JETTON)
				{
					//返回下注
					for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) 
					{
						if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
						{
							CMD_S_PlaceJettonFail PlaceJettonFail;
							ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
							PlaceJettonFail.lJettonArea=nAreaIndex;
							PlaceJettonFail.lPlaceScore=m_lUserJettonScore[nAreaIndex][wChairID];
							PlaceJettonFail.wPlaceUser=wChairID;

							//游戏玩家
							for (WORD i=0; i<GAME_PLAYER; ++i)
							{
								IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
								if (pIServerUserItem==NULL) 
									continue;

								m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
							}

							m_lAllJettonScore[nAreaIndex] -= m_lUserJettonScore[nAreaIndex][wChairID];
							m_lUserJettonScore[nAreaIndex][wChairID] = 0;
						}
					}
				}
				else
				{
					//写入积分
					if (m_lUserWinScore[wChairID]!=0L) 
					{
						tagScoreInfo ScoreInfo[GAME_PLAYER];
						ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
						//库存金币
						if (!pIServerUserItem->IsAndroidUser())
							m_lStorageCurrent -= (m_lUserWinScore[wChairID]);						
						ScoreInfo[wChairID].lScore = m_lUserWinScore[wChairID];
						ScoreInfo[wChairID].cbType=m_lUserWinScore[wChairID]>0?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
						ScoreInfo[wChairID].lRevenue = m_lUserRevenue[wChairID];
						m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
						m_lUserWinScore[wChairID] = 0;
					}
					//清除下注
					for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
					{
						m_lUserJettonScore[nAreaIndex][wChairID] = 0;
					}
				}

				return true;
			}

			//状态判断
			if (m_pITableFrame->GetGameStatus()!= GAME_SCENE_GAME_END)
			{
				//提示消息
				TCHAR szTipMsg[128];
				_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("由于庄家[ %s ]强退，游戏提前结束！"),pIServerUserItem->GetNickName());

				//发送消息
				SendGameMessage(INVALID_CHAIR,szTipMsg);	

				//设置状态
				m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

				//设置时间
				m_dwJettonTime=(DWORD)time(NULL);
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);

				//游戏作弊
				m_bControl = FALSE;
				if (m_pServerContro != NULL && m_pServerContro->NeedControl() && m_pServerContro->ControlResult(m_cbTableCardArray))
				{
					//获取控制目标牌型
					m_bControl = TRUE;
					m_pServerContro->CompleteControl();
				}

				//计算分数
				CalculateScore();

				//结束消息
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));

				//占位玩家成绩
				for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
				{
					if (m_cbDeskChairID[i] == INVALID_BYTE)
					{
						continue;
					}
					IServerUserItem *pIOccupySeatServerUserItem = m_pITableFrame->GetTableUserItem(m_cbDeskChairID[i]);
					if (!pIOccupySeatServerUserItem)
					{
						continue;
					}

					GameEnd.lDeskScoreT[i] = m_lUserWinScore[m_cbDeskChairID[i]];
				}

				//庄家信息
				GameEnd.nBankerTime = m_wBankerTime;
				GameEnd.lBankerTotallScore=m_lBankerWinScore;
				GameEnd.lBankerScore=m_lUserWinScore[m_wCurrentBanker];

				//扑克信息
				CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));

				//发送积分
				GameEnd.cbTimeLeave=m_nGameEndTime;	
				for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
					if ( pIServerUserItem == NULL ) continue;

					//设置成绩
					GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

					//返还积分
					GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

					//设置税收
					//if (m_lUserRevenue[wUserIndex]>0) GameEnd.L=m_lUserRevenue[wUserIndex];
					//else if (m_wCurrentBanker!=INVALID_CHAIR) GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
					//else GameEnd.lRevenue=0;

					//发送消息					
					m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				}
			}

			//扣除分数
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			ScoreInfo[m_wCurrentBanker].lScore =m_lUserWinScore[m_wCurrentBanker];
			ScoreInfo[m_wCurrentBanker].lRevenue = m_lUserRevenue[m_wCurrentBanker];
			ScoreInfo[m_wCurrentBanker].cbType = (m_lUserWinScore[m_wCurrentBanker]>0?SCORE_TYPE_WIN:SCORE_TYPE_LOSE);
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

			//库存金币
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if (pIServerUserItem != NULL && !pIServerUserItem->IsAndroidUser())
				m_lStorageCurrent -= (m_lUserWinScore[m_wCurrentBanker]);

			m_lUserWinScore[m_wCurrentBanker] = 0;

			//切换庄家
			ChangeBanker(true);

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
	case GAME_SCENE_FREE:			//空闲状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);

			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));			

			//控制信息
			StatusFree.bIsAllowApplyBanker = true;
			StatusFree.lLimitScore = m_occupyseatConfig.lOccupySeatFree;
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			CopyMemory(StatusFree.lAreaLimitScore, m_lAreaLimitScore, sizeof(StatusFree.lAreaLimitScore));
			StatusFree.wBankerUser=m_wCurrentBanker;	
			StatusFree.wBankerTime=m_wBankerTime;
			StatusFree.lBankerWinScore=m_lBankerWinScore;
			//获取庄家
			IServerUserItem* pIBankerServerUserItem = NULL;
			if ( m_wCurrentBanker == INVALID_CHAIR )
			{
				m_lBankerScore = 1000000000;
			}
			else
			{
				IServerUserItem* pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				if ( pIBankerServerUserItem != NULL )
				{
					m_lBankerScore = pIBankerServerUserItem->GetUserScore();
				}
			}

			StatusFree.lBankerScore = m_lBankerScore;

			//玩家信息
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusFree.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*10); 
			}

			//房间名称
			CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));
			//占位
			CopyMemory(StatusFree.cbDeskChairID, m_cbDeskChairID, sizeof(m_cbDeskChairID));

			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(m_nFreeTime-__min(dwPassTime,(DWORD)m_nFreeTime));
			
			//机器人配置
			if(pIServerUserItem->IsAndroidUser())
			{
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
				ASSERT(pCustomConfig);

				CopyMemory(&StatusFree.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
			}
			
			//筹码配置
			tagRobotInfo tmpRobotInfo;
			CopyMemory(&StatusFree.lBet, &tmpRobotInfo.nChip, sizeof(tmpRobotInfo.nChip));

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			//限制提示
			TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("本房间上庄条件为：%I64d,区域限制为：%I64d,玩家限制为：%I64d"),m_lApplyBankerCondition,
				m_lAreaLimitScore,m_lUserLimitScore);

			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);

			//发送申请者
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	case GAME_SCENE_PLACE_JETTON:		//游戏状态
	case GAME_SCENE_GAME_END:			//结束状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);		

			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));
			StatusPlay.bIsAllowApplyBanker = true;
			//全局下注
			CopyMemory(StatusPlay.lAllJettonScore,&m_lAllJettonScore[1],sizeof(StatusPlay.lAllJettonScore));

			//玩家下注
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				//CopyMemory(StatusPlay.lUserJettonScore, &m_lUserJettonScore[1], sizeof(StatusPlay.lUserJettonScore));
				for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
					StatusPlay.lUserJettonScore[nAreaIndex-1] = m_lUserJettonScore[nAreaIndex][wChairID];

				//最大下注
				StatusPlay.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*MAX_RATE);
			}

			//控制信息
			StatusPlay.lApplyBankerCondition = m_lApplyBankerCondition;
			CopyMemory(StatusPlay.lAreaLimitScore, m_lAreaLimitScore, sizeof(StatusPlay.lAreaLimitScore));
			StatusPlay.lLimitScore = m_occupyseatConfig.lOccupySeatFree;
			StatusPlay.lStorageStart = m_lStorageCurrent;
			//庄家信息
			StatusPlay.wBankerUser=m_wCurrentBanker;			
			StatusPlay.wBankerTime=m_wBankerTime;
			StatusPlay.lBankerWinScore=m_lBankerWinScore;	
			//获取庄家
			IServerUserItem* pIBankerServerUserItem = NULL;
			if ( m_wCurrentBanker == INVALID_CHAIR )
			{
				m_lBankerScore = 1000000000;
			}
			else
			{
				IServerUserItem* pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				if ( pIBankerServerUserItem != NULL )
				{
					m_lBankerScore = pIBankerServerUserItem->GetUserScore();
				}
			}

			StatusPlay.lBankerScore = m_lBankerScore;

			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			int	nTotalTime = (cbGameStatus==GAME_SCENE_PLACE_JETTON?m_nPlaceJettonTime:m_nGameEndTime);
			StatusPlay.cbTimeLeave=(BYTE)(nTotalTime-__min(dwPassTime,(DWORD)nTotalTime));
			StatusPlay.cbGameStatus=m_pITableFrame->GetGameStatus();			
			StatusPlay.bIsJettonStatus = (cbGameStatus == GAME_SCENE_PLACE_JETTON);
			//房间名称
			CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));
			//占位
			CopyMemory(StatusPlay.cbDeskChairID, m_cbDeskChairID, sizeof(m_cbDeskChairID));
			//筹码配置
			tagRobotInfo tmpRobotInfo;
			CopyMemory(&StatusPlay.lBet, &tmpRobotInfo.nChip, sizeof(tmpRobotInfo.nChip));
			//结束判断
			if (cbGameStatus==GAME_SCENE_GAME_END)
			{
				//设置成绩
				StatusPlay.lEndUserScore=m_lUserWinScore[wChairID];

				//返还积分
				StatusPlay.lEndUserReturnScore=m_lUserReturnScore[wChairID];

				//设置税收
				if (m_lUserRevenue[wChairID]>0) StatusPlay.lEndRevenue=m_lUserRevenue[wChairID];
				else if (m_wCurrentBanker!=INVALID_CHAIR) StatusPlay.lEndRevenue=m_lUserRevenue[m_wCurrentBanker];
				else StatusPlay.lEndRevenue=0;

				//庄家成绩
				StatusPlay.lEndBankerScore=m_lBankerCurGameScore;

				//扑克信息
				CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			}
			
			//机器人配置
			if(pIServerUserItem->IsAndroidUser())
			{
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
				ASSERT(pCustomConfig);

				CopyMemory(&StatusPlay.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
			}

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			//发送申请者
			SendApplyUser( pIServerUserItem );

			return bSuccess;
		}
	}

	return false;
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

			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_nPlaceJettonTime*1000,1,0L);

			//派发扑克
			DispatchTableCard();

			//设置状态
			m_pITableFrame->SetGameStatus(GAME_SCENE_PLACE_JETTON);

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

				//设置时间
				m_dwJettonTime=(DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);		
			}

			return true;
		}
	case IDI_GAME_END:			//结束游戏
		{
			//写入积分
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);				
				if ( pIServerUserItem == NULL||(QueryBuckleServiceCharge(wUserChairID)==false)) continue;				
				
				//胜利类型
				BYTE ScoreKind=(m_lUserWinScore[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
				ScoreInfo[wUserChairID].lScore = m_lUserWinScore[wUserChairID] - m_lUserRevenue[wUserChairID];
				ScoreInfo[wUserChairID].lRevenue = m_lUserRevenue[wUserChairID];
				ScoreInfo[wUserChairID].cbType = ScoreKind;

				//库存金币
				if (!pIServerUserItem->IsAndroidUser())
				{
					m_lStorageCurrent -= (m_lUserWinScore[wUserChairID]);					
				}
			}
			
			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

			LONGLONG lTempDeduct=0;
			lTempDeduct=m_nStorageDeduct;
			bool bDeduct=NeedDeductStorage();
			lTempDeduct=bDeduct?lTempDeduct:0;
			//库存衰减
			if (m_lStorageCurrent > 0)
				m_lStorageCurrent = m_lStorageCurrent - m_lStorageCurrent*lTempDeduct/1000;

			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,0L);

			//发送消息
			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree,sizeof(GameFree));
			GameFree.cbTimeLeave=m_nFreeTime;
			GameFree.lStorageStart=m_lStorageCurrent;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			
			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
			
			//切换庄家
			ChangeBanker(false);
			return true;
		}
	}

	return false;
}

bool cmpRoomUserInfo(tagRoomUserInfo st1, tagRoomUserInfo st2)
{
	if (st1.lBetRank20Score > st2.lBetRank20Score)
	{
		return true;
	}
	return false;
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_CLICK_REJETTON:		//用户加注
	{
		ASSERT(wDataSize == sizeof(CMD_C_UserReJetton));
		if (wDataSize != sizeof(CMD_C_UserReJetton)) return false;

		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		CMD_C_UserReJetton * pUserReJetton = (CMD_C_UserReJetton *)pData;
		ReJetton(pIServerUserItem);
		return true;
	}
	case SUB_C_QURARY_RECORD:
	{
		//用户效验
		//tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		//if (pUserData->cbUserStatus != US_PLAYING) return true;

		SendGameRecord(pIServerUserItem);
		return true;
	}
	case SUB_C_QURARY_LOTTORY_INFO:
	{
		return true;
	}
	case SUB_C_QURARY_ROOM_USERS_INFO:
	{
		vecTagRoomUserInfo tagRoomUserInfoList;
		//计算游戏记录
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL) continue;

			tagRoomUserInfo stTagRoomInfo;
			stTagRoomInfo.lBetRank20Score = pIServerUserItem->GetUserScore();
			stTagRoomInfo.wChairID = wChairID;

			//计算总下注和胜利数
			SCORE nTotalBetNum = 0;
			SCORE nTotalWinNum = 0;
			POSITION posTail = m_tagUserRecord[wChairID].GetTailPosition();
			while (posTail)
			{
				tagStatisticsRecord tagStatistRecord;
				tagStatistRecord = m_tagUserRecord[wChairID].GetPrev(posTail);

				nTotalBetNum += tagStatistRecord.lPlayerBetScore;
				if (tagStatistRecord.lPlayerWinScore > 0)
				{
					nTotalWinNum += 1;
				}
			}
			stTagRoomInfo.lBetRank20Jetton = nTotalBetNum;
			stTagRoomInfo.nWinCount = nTotalWinNum;
			tagRoomUserInfoList.push_back(stTagRoomInfo);
		}
		sort(tagRoomUserInfoList.begin(), tagRoomUserInfoList.end(), cmpRoomUserInfo);
		//遍历，生成返回数据
		CMD_S_SendRoomUserInfo roomUserInfo;
		ZeroMemory(&roomUserInfo, sizeof(CMD_S_SendRoomUserInfo));
		int nPlayerNum = 0;
		for (auto &userInfo : tagRoomUserInfoList)
		{
			roomUserInfo.wChairID[nPlayerNum] = userInfo.wChairID;
			roomUserInfo.nWinCount[nPlayerNum] = userInfo.nWinCount;
			roomUserInfo.lBetRank20Score[nPlayerNum] = userInfo.lBetRank20Score;
			roomUserInfo.lBetRank20Jetton[nPlayerNum] = userInfo.lBetRank20Jetton;
			nPlayerNum++;
			if (nPlayerNum>=20)
			{
				break;
			}
		}
		roomUserInfo.nPlayerCount = nPlayerNum;
		//发送消息
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_QURARY_ROOM_PLAYERS_INFO, &roomUserInfo, sizeof(CMD_S_SendRoomUserInfo));
		return true;
	}
	case SUB_C_PLACE_JETTON:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			if (wDataSize!=sizeof(CMD_C_PlaceJetton)) return false;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//消息处理
			CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pData;
			return OnUserPlaceJetton(pUserData->wChairID,pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
		}
	case SUB_C_APPLY_BANKER:		//申请做庄
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;

			return OnUserApplyBanker(pIServerUserItem);	
		}
	case SUB_C_CANCEL_BANKER:		//取消做庄
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;

			return OnUserCancelBanker(pIServerUserItem);	
		}
	case SUB_C_APPLY_DESK:
		{
			//效验数据
			ASSERT (wDataSize == sizeof(CMD_C_ApplyDesk));
			if (wDataSize != sizeof(CMD_C_ApplyDesk)) return true;

			//用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus == US_LOOKON) return true;

			//消息处理
			CMD_C_ApplyDesk *pOccupySeat = (CMD_C_ApplyDesk *)pData;
			if (pOccupySeat->bApply)
			{
				return OnUserOccupySeat(pUserData->wChairID, pOccupySeat->wDeskChairID);
			}
			else
			{
				bool bvalid = false;
				//校验数据
				WORD wDeskChairID = INVALID_BYTE;
				for (WORD i = 0; i<MAX_OCCUPY_SEAT_COUNT; i++)
				{
					if (m_cbDeskChairID[i] == pUserData->wChairID)
					{
						bvalid = true;
						wDeskChairID = i;
						//重置无效
						m_cbDeskChairID[i] = INVALID_BYTE;
						break;
					}
				}

				if (bvalid == true && wDeskChairID != INVALID_BYTE)
				{
					CMD_S_GameDown GameDown;
					ZeroMemory(&GameDown, sizeof(GameDown));
					GameDown.wChairID = pUserData->wChairID;
					GameDown.wDeskChairID = wDeskChairID + 1;

					//发送数据
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
				}
			}
		}
	}

	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
		case SUB_C_AMDIN_COMMAND:
		{

			ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
			if (wDataSize != sizeof(CMD_C_AdminReq)) return false;

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			if (m_pServerContro == NULL) return false;
			CopyMemory(m_szControlName, pIServerUserItem->GetNickName(), sizeof(m_szControlName));

			return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
		}
		case SUB_C_UPDATE_STORAGE:		//更新库存
		{
			ASSERT(wDataSize == sizeof(CMD_C_UpdateStorage));
			if (wDataSize != sizeof(CMD_C_UpdateStorage)) return false;

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			//消息处理
			CMD_C_UpdateStorage * pUpdateStorage = (CMD_C_UpdateStorage *)pData;

			m_lStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
			m_nStorageDeduct = pUpdateStorage->lRoomStorageDeduct;

			CMD_S_ADMIN_STORAGE_INFO StorageInfo;
			ZeroMemory(&StorageInfo, sizeof(StorageInfo));

			StorageInfo.lRoomStorageStart = m_lStorageStart;
			StorageInfo.lRoomStorageDeduct = m_nStorageDeduct;
			StorageInfo.lRoomStorageCurrent = m_lStorageCurrent;
			StorageInfo.lMaxRoomStorage[0] = m_lStorageMax1;
			StorageInfo.wRoomStorageMul[0] = m_lStorageMul1;
			StorageInfo.lMaxRoomStorage[1] = m_lStorageMax2;
			StorageInfo.wRoomStorageMul[1] = m_lStorageMul2;

			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));

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
			m_lStorageMax1 = pModifyStorage->lMaxRoomStorage[0];
			m_lStorageMax2 = pModifyStorage->lMaxRoomStorage[1];
			m_lStorageMul1 = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
			m_lStorageMul2 = (SCORE)(pModifyStorage->wRoomStorageMul[1]);


			CMD_S_ADMIN_STORAGE_INFO StorageInfo;
			ZeroMemory(&StorageInfo, sizeof(StorageInfo));
			StorageInfo.lRoomStorageStart = m_lStorageStart;
			StorageInfo.lRoomStorageDeduct = m_nStorageDeduct;
			StorageInfo.lRoomStorageCurrent = m_lStorageCurrent;
			StorageInfo.lMaxRoomStorage[0] = m_lStorageMax1;
			StorageInfo.wRoomStorageMul[0] = m_lStorageMul1;
			StorageInfo.lMaxRoomStorage[1] = m_lStorageMax2;
			StorageInfo.wRoomStorageMul[1] = m_lStorageMul2;

			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));

			return true;
		}
		case SUB_C_REFRESH_ROOM_STORAGESTATUS:
		{
			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false) return false;

			CMD_S_ADMIN_STORAGE_INFO StorageInfo;
			ZeroMemory(&StorageInfo, sizeof(StorageInfo));
			StorageInfo.lRoomStorageStart = m_lStorageStart;
			StorageInfo.lRoomStorageDeduct = m_nStorageDeduct;
			StorageInfo.lRoomStorageCurrent = m_lStorageCurrent;
			StorageInfo.lMaxRoomStorage[0] = m_lStorageMax1;
			StorageInfo.wRoomStorageMul[0] = m_lStorageMul1;
			StorageInfo.lMaxRoomStorage[1] = m_lStorageMax2;
			StorageInfo.wRoomStorageMul[1] = m_lStorageMul2;

			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			return true;
		}
		}
	}
	return false;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem)
{
	//离线庄家
	if (wChairID == m_wCurrentBanker && pIServerUserItem->IsAndroidUser() == false)
	{
		m_wOfflineBanker = wChairID;
	}

	if(pIServerUserItem == NULL) return false;
	//切换庄家
	if (wChairID==m_wCurrentBanker) ChangeBanker(true);

	//取消申请
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID!=m_ApplyUserArray[i]) continue;

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		//构造变量
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker,sizeof(CancelBanker));

		//设置变量
		CancelBanker.wCancelUser=wChairID;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}
	return true;

}
//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//起始分数
	m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();

	//设置时间
	if ((bLookonUser==false)&&(m_dwJettonTime==0L))
	{
		m_dwJettonTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
	}
	
	//限制提示
	TCHAR szTipMsg[128];
	_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("本房间上庄金币必须大于：%I64d,区域限制为：%I64d,玩家限制为：%I64d"),m_lApplyBankerCondition,
		m_lAreaLimitScore,m_lUserLimitScore);
	m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT|SMT_EJECT);

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	ASSERT(pIServerUserItem !=NULL);

	//起始分数
	m_lUserStartScore[wChairID] = 0;
	
	bool bvalid = false;
	//校验数据
	WORD wDeskChairID = INVALID_BYTE;
	for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
	{
		if (m_cbDeskChairID[i] == wChairID)
		{
			bvalid = true;
			wDeskChairID = i;
			//重置无效
			m_cbDeskChairID[i] = INVALID_BYTE;

			break;
		}
	}

	if (bvalid == true && wDeskChairID != INVALID_BYTE)
	{
		CMD_S_GameDown GameDown;
		ZeroMemory(&GameDown, sizeof(GameDown));
		GameDown.wChairID = wChairID;
		GameDown.wDeskChairID = wDeskChairID + 1;
		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
	}

	//记录成绩
	if (bLookonUser==false)
	{
		//切换庄家
		if (wChairID==m_wCurrentBanker)
		{
			ChangeBanker(true);
		}

		//取消申请
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID!=m_ApplyUserArray[i]) continue;

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//设置变量
			CancelBanker.wCancelUser=wChairID;

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}
	}

	return true;
}


//加注事件
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
	//效验参数
	ASSERT((cbJettonArea<= POKER_TYPE_INDEX && cbJettonArea>= RED_INDEX)&&(lJettonScore>0L));
	if ((cbJettonArea>POKER_TYPE_INDEX)||(lJettonScore<=0L) || cbJettonArea<RED_INDEX) return false;
	
	if (cbJettonArea == RED_INDEX)
	{
		if (m_lUserJettonScore[BLACK_INDEX][wChairID] > 0)
		{
			CMD_S_PlaceJettonFail PlaceJettonFail;
			ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
			PlaceJettonFail.lJettonArea = cbJettonArea;
			PlaceJettonFail.lPlaceScore = lJettonScore;
			PlaceJettonFail.wPlaceUser = wChairID;
			PlaceJettonFail.cbReason = 3;//黑方和红方不能同时下注
			//发送消息
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
			return true;
		}
	}
	else if (cbJettonArea == BLACK_INDEX)
	{
		if (m_lUserJettonScore[RED_INDEX][wChairID] > 0)
		{
			CMD_S_PlaceJettonFail PlaceJettonFail;
			ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
			PlaceJettonFail.lJettonArea = cbJettonArea;
			PlaceJettonFail.lPlaceScore = lJettonScore;
			PlaceJettonFail.wPlaceUser = wChairID;
			PlaceJettonFail.cbReason = 3;//黑方和红方不能同时下注
			//发送消息
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
			return true;
		}
	}
	if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLACE_JETTON)
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea=cbJettonArea;
		PlaceJettonFail.lPlaceScore=lJettonScore;
		PlaceJettonFail.wPlaceUser=wChairID;
		PlaceJettonFail.cbReason = 0; //当前不处于下注状态
		//发送消息
		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
		return true;
	}
	//庄家判断
	if (m_wCurrentBanker==wChairID)
	{
		return true;
	}
	if (m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR) 
	{
		return true;
	}

	//庄家积分
	LONGLONG lBankerScore = 0;
	if(INVALID_CHAIR != m_wCurrentBanker)
	{
		IServerUserItem * pIServerUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		lBankerScore = pIServerUserItemBanker->GetUserScore();
	}

	//变量定义
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	LONGLONG lJettonCount=0L;
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];
	LONGLONG lAllJettonCount=0L;
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lAllJettonCount += m_lAllJettonScore[nAreaIndex];

	//玩家积分
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//合法校验
	if (m_lUserLimitScore < lJettonCount + lJettonScore || lUserScore < lJettonCount + lJettonScore)
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea = cbJettonArea;
		PlaceJettonFail.lPlaceScore = lJettonScore;
		PlaceJettonFail.wPlaceUser = wChairID;
		PlaceJettonFail.cbReason = 1;	//当前下注超过了用户的积分限制
		//发送消息
		m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
		return true;
	}

	//下注总额超过庄家能支付上限时提前结束游戏
	if (INVALID_CHAIR != m_wCurrentBanker)
	{
		if (lBankerScore <= (lAllJettonCount + lJettonScore + 100) * 10)
		{
			CMD_S_PlaceJettonFail PlaceJettonFail;
			ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
			PlaceJettonFail.lJettonArea = cbJettonArea;
			PlaceJettonFail.lPlaceScore = lJettonScore;
			PlaceJettonFail.wPlaceUser = wChairID;
			PlaceJettonFail.cbReason = 5;	//当前下注超过了用户的积分限制
											//发送消息
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
			return true;
		}
	}

	//成功标识
	bool bPlaceJettonSuccess=true;

	//合法验证
	if (GetUserMaxJetton(wChairID, cbJettonArea) >= lJettonScore)
	{
		//机器人验证
		if(pIServerUserItem->IsAndroidUser())
		{
			//区域限制
			if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
				return true;

			//数目限制
			bool bHaveChip = false;
			for (int i = 1; i <= AREA_COUNT; i++)
			{
				if (m_lUserJettonScore[i+1][wChairID] != 0)
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
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;
		}

		if (bPlaceJettonSuccess)
		{
			for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
			{
				//清空上局总注
				m_lLastAllJettonPlace[nAreaIndex][wChairID] = 0;
				//清空本局最后一注
				m_lLastAllJettonArea[nAreaIndex][wChairID] = 0;
			}

			m_lLastAllJettonArea[cbJettonArea][wChairID] += lJettonScore;
			//保存下注
			m_lAllJettonScore[cbJettonArea] += lJettonScore;
			m_lUserJettonScore[cbJettonArea][wChairID] += lJettonScore;			
		}
	}
	else
	{
		bPlaceJettonSuccess=false;
	}

	if (bPlaceJettonSuccess)
	{
		for(WORD i=0; i<GAME_PLAYER; i++)
		{
			IServerUserItem *pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
			if ( pIServerUserItemSend == NULL ) continue;
			
			bool bIsGameCheatUser =  CUserRight::IsGameDebugUser(pIServerUserItemSend->GetUserRight());

			//变量定义
			CMD_S_PlaceJetton PlaceJetton;
			ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

			//构造变量
			PlaceJetton.wChairID=wChairID;
			PlaceJetton.cbJettonArea=cbJettonArea;
			PlaceJetton.lJettonScore=lJettonScore;
			PlaceJetton.bIsAndroid = m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser();
			//发送消息
			m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
			m_pITableFrame->SendLookonData(i,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
		}

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
				for (BYTE j = 1; j <= AREA_COUNT; j++)
				{
					lUserAllBet += m_lUserJettonScore[j][i];
				}
				if (0 == lUserAllBet) continue;

				//构造数据
				tagUserBet *UserBet = (tagUserBet*)(cbBuffer + dwPaketSize);
				CopyMemory(UserBet->szNickName, pServerUserItem->GetNickName(), sizeof(UserBet->szNickName));
				UserBet->dwUserGameID = pServerUserItem->GetGameID();
				UserBet->lUserStartScore = m_lUserStartScore[i];
				UserBet->lUserWinLost = pServerUserItem->GetUserScore() - UserBet->lUserStartScore;
				for (BYTE k = 1; k <= AREA_COUNT; k++)
				{
					UserBet->lUserBet[k-1] = m_lUserJettonScore[k][i];
				}

				dwPaketSize += sizeof(tagUserBet);
			}

			m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, cbBuffer, dwPaketSize);

		}
	}
	else
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea=cbJettonArea;
		PlaceJettonFail.lPlaceScore=lJettonScore;
		PlaceJettonFail.wPlaceUser=wChairID;
		PlaceJettonFail.cbReason = 1;	//下注分数超过了玩家的下注分数
		//发送消息
		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
	}

	return true;
}

//发送扑克
bool CTableFrameSink::DispatchTableCard()
{

	//重新洗牌
	m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));	
	//设置扑克
	CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));
	
	//m_cbTableCardArray[0][0] = 0x06;
	//m_cbTableCardArray[0][1] = 0x16;
	//m_cbTableCardArray[0][2] = 0x1A;

	//m_cbTableCardArray[1][0] = 0x03;
	//m_cbTableCardArray[1][1] = 0x04;
	//m_cbTableCardArray[1][2] = 0x05;
	return true;
}

//申请庄家
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	//合法判断
	LONGLONG lUserScore=pIApplyServerUserItem->GetUserScore();
	if (lUserScore<m_lApplyBankerCondition)
	{
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("你的金币不足以申请庄家，申请失败！"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	//存在判断
	WORD wApplyUserChairID=pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];
		if (wChairID==wApplyUserChairID)
		{
			m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("你已经申请了庄家，不需要再次申请！"),SMT_CHAT|SMT_EJECT);
			return true;
		}
	}

	if (pIApplyServerUserItem->IsAndroidUser()&&(m_ApplyUserArray.GetCount())>m_nRobotListMaxCount)
	{
		return true;
	}

	//保存信息 
	m_ApplyUserArray.Add(wApplyUserChairID);

	//构造变量
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));

	//设置变量
	ApplyBanker.wApplyUser=wApplyUserChairID;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	//切换判断
	if (m_pITableFrame->GetGameStatus()==GAME_SCENE_FREE && m_ApplyUserArray.GetCount()==1)
	{
		ChangeBanker(false);
	}

	return true;
}

//取消申请
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//当前庄家
	if (pICancelServerUserItem->GetChairID()==m_wCurrentBanker && m_pITableFrame->GetGameStatus()!=GAME_SCENE_FREE)
	{
		//发送消息
		m_pITableFrame->SendGameMessage(pICancelServerUserItem,TEXT("游戏已经开始，不可以取消当庄！"),SMT_CHAT|SMT_EJECT);
		return true;
	}
	
	bool bValidQuitBanker = true;
	//存在判断
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		//获取玩家
		WORD wChairID=m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

		//条件过滤
		if (pIServerUserItem==NULL) continue;
		if (pIServerUserItem->GetUserID()!=pICancelServerUserItem->GetUserID()) continue;

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		bValidQuitBanker = false;

		if (m_wCurrentBanker!=wChairID)
		{
			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//设置变量
			CancelBanker.bAuto = false;	//手动取消庄家
			CancelBanker.wCancelUser = wChairID;
			CancelBanker.bIsAllowApplyBanker = false;
			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker==wChairID)
		{
			//切换庄家 
			m_wCurrentBanker=INVALID_CHAIR;
			ChangeBanker(true);
		}

		return true;
	}
	
	if (bValidQuitBanker == true)
	{
		m_wCurrentBanker=INVALID_CHAIR;
		ChangeBanker(true);
		return true;
	}

	return true;
}

//用户占位
bool CTableFrameSink::OnUserOccupySeat(WORD wOccupyChairID, BYTE cbOccupySeatIndex)
{
	//校验用户
	ASSERT (wOccupyChairID != INVALID_CHAIR);
	if (wOccupyChairID == INVALID_CHAIR) return true;

	ASSERT (cbOccupySeatIndex >= SEAT_LEFT1_INDEX && cbOccupySeatIndex <= SEAT_RIGHT3_INDEX);
	if (!(cbOccupySeatIndex >= SEAT_LEFT1_INDEX && cbOccupySeatIndex <= SEAT_RIGHT3_INDEX))
	{
		return true;
	}

	//庄家不能申请坐下
	if (m_wCurrentBanker == wOccupyChairID)
	{
		CMD_S_UpDesk UpDesk;
		ZeroMemory(&UpDesk, sizeof(UpDesk));
		UpDesk.bUpDeskOk = false;
		UpDesk.wChairID = wOccupyChairID;
		UpDesk.wDeskChairID = cbOccupySeatIndex;
		UpDesk.cbFailReason = 5;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		return true;
	}

	//占位失败 没有空余的位置
	if (m_cbDeskChairID[cbOccupySeatIndex-1] != INVALID_BYTE)
	{
		CMD_S_UpDesk UpDesk;
		ZeroMemory(&UpDesk, sizeof(UpDesk));
		UpDesk.bUpDeskOk = false;
		UpDesk.wChairID = wOccupyChairID;
		UpDesk.wDeskChairID = cbOccupySeatIndex;
		UpDesk.cbFailReason = 1;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		return true;
	}
	ASSERT (m_cbDeskChairID[cbOccupySeatIndex-1] == INVALID_BYTE);

	//校验是否已经占位
	for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
	{
		//占位失败 您已经在座位上了
		if (m_cbDeskChairID[i] == wOccupyChairID)
		{
			CMD_S_UpDesk UpDesk;
			ZeroMemory(&UpDesk, sizeof(UpDesk));
			UpDesk.bUpDeskOk = false;
			UpDesk.wChairID = wOccupyChairID;
			UpDesk.wDeskChairID = cbOccupySeatIndex;
			UpDesk.cbFailReason = 3;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
			return true;
		}
	}

	//占位失败  财富不足
	ASSERT (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() >= m_occupyseatConfig.lOccupySeatFree);
	if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < m_occupyseatConfig.lOccupySeatFree)
	{
		CMD_S_UpDesk UpDesk;
		ZeroMemory(&UpDesk, sizeof(UpDesk));
		UpDesk.bUpDeskOk = false;
		UpDesk.wChairID = wOccupyChairID;
		UpDesk.wDeskChairID = cbOccupySeatIndex;
		UpDesk.cbFailReason = 2;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		return true;
	}


	//占位成功
	m_cbDeskChairID[cbOccupySeatIndex-1] = wOccupyChairID;

	CMD_S_UpDesk UpDesk;
	ZeroMemory(&UpDesk, sizeof(UpDesk));
	UpDesk.bUpDeskOk = true;
	UpDesk.wChairID = wOccupyChairID;
	UpDesk.wDeskChairID = cbOccupySeatIndex;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));

	return true;
}

//更换庄家
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	//切换标识
	bool bChangeBanker=false;

	//取消当前
	if (bCancelCurrentBanker)
	{
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			//获取玩家
			WORD wChairID=m_ApplyUserArray[i];

			//条件过滤
			if (wChairID!=m_wCurrentBanker) continue;

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		//设置庄家
		m_wCurrentBanker=INVALID_CHAIR;

		//轮换判断
		TakeTurns();

		//设置变量
		bChangeBanker=true;
		m_bExchangeBanker = true;
	}
	//轮庄判断
	else if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		{
			//获取庄家
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if(pIServerUserItem)
			{
				LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

				//次数判断
				if (m_lPlayerBankerMAX<=m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
				{
					//庄家增加判断 同一个庄家情况下只判断一次
					if(m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker && lBankerScore>=m_lApplyBankerCondition)
					{
						//加庄局数设置：当庄家坐满设定的局数之后(m_nBankerTimeLimit)，
						//所带金币值还超过下面申请庄家列表里面所有玩家金币时，
						//可以再加坐庄m_nBankerTimeAdd局，加庄局数可设置。

						//金币超过m_lExtraBankerScore之后，
						//就算是下面玩家的金币值大于他的金币值，他也可以再加庄m_nExtraBankerTime局。
						bool bScoreMAX = true;
						m_bExchangeBanker = false;

						for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
						{
							//获取玩家
							WORD wChairID = m_ApplyUserArray[i];
							IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
							LONGLONG lScore = pIServerUserItem->GetUserScore();

							if ( wChairID != m_wCurrentBanker && lBankerScore <= lScore )
							{
								bScoreMAX = false;
								break;
							}
						}

						if ( bScoreMAX || (lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l) )
						{
							if ( bScoreMAX )
							{
								m_lPlayerBankerMAX += m_nBankerTimeAdd;
							}
							if ( lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l )
							{
								m_lPlayerBankerMAX += m_nExtraBankerTime;
							}
							return true;
						}
					}

					//设置庄家
					m_wCurrentBanker=INVALID_CHAIR;

					//轮换判断
					TakeTurns();

					//撤销玩家
					for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						//获取玩家
						WORD wChairID=m_ApplyUserArray[i];

						//条件过滤
						if (wChairID!=m_wCurrentBanker) continue;

						//删除玩家
						m_ApplyUserArray.RemoveAt(i);

						break;
					}

					bChangeBanker=true;
					m_bExchangeBanker = true;

					//提示消息
					TCHAR szTipMsg[128];
					if (lBankerScore<m_lApplyBankerCondition)
						_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]分数少于(%I64d)，强行换庄!"),pIServerUserItem->GetNickName(),m_lApplyBankerCondition);
					else
						_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]做庄次数达到(%d)，强行换庄!"),pIServerUserItem->GetNickName(),m_lPlayerBankerMAX);

					//发送消息
					SendGameMessage(INVALID_CHAIR,szTipMsg);	
				}
			}
		}
	}
	//系统做庄
	else if (m_wCurrentBanker == INVALID_CHAIR && m_ApplyUserArray.GetCount()!=0)
	{
		//轮换判断
		TakeTurns();

		//撤销玩家
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			//获取玩家
			WORD wChairID=m_ApplyUserArray[i];

			//条件过滤
			if (wChairID!=m_wCurrentBanker) continue;

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		bChangeBanker=true;
		m_bExchangeBanker = true;
	}

	//切换判断
	if (bChangeBanker)
	{
		//最大坐庄数
		m_lPlayerBankerMAX = m_nBankerTimeLimit;

		//设置变量
		m_wBankerTime = 0;
		m_lBankerWinScore=0;

		//发送消息
		CMD_S_ChangeBanker stChangeBanker;
		ZeroMemory(&stChangeBanker,sizeof(stChangeBanker));
		stChangeBanker.wBankerUser = m_wCurrentBanker;
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			stChangeBanker.lBankerScore = pIServerUserItem->GetUserScore();
		}
		else
		{
			stChangeBanker.lBankerScore = 1000000000;
		}

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&stChangeBanker,sizeof(CMD_S_ChangeBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&stChangeBanker,sizeof(CMD_S_ChangeBanker));

		if (m_wCurrentBanker!=INVALID_CHAIR)
		{
			//获取参数
			m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
			ASSERT(m_pGameServiceOption!=NULL);

			//读取消息
			int nMessageCount = 3;

			//读取配置
			INT nIndex=rand()%nMessageCount;
			TCHAR szMessage1[256],szMessage2[256];
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomConfig);
			if(0 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem1, sizeof(pCustomConfig->CustomGeneral.szMessageItem1));
			}
			else if(1 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem2, sizeof(pCustomConfig->CustomGeneral.szMessageItem2));
			}
			else if(2 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem3, sizeof(pCustomConfig->CustomGeneral.szMessageItem3));
			}

			//获取玩家
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

			//发送消息
			_sntprintf(szMessage2,CountArray(szMessage2),TEXT("【 %s 】上庄了，%s"),pIServerUserItem->GetNickName(), szMessage1);
			SendGameMessage(INVALID_CHAIR,szMessage2);
		}
	}

	return bChangeBanker;
}

//轮换判断
void CTableFrameSink::TakeTurns()
{
	//变量定义
	int nInvalidApply = 0;

	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE)
		{
			//获取分数
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray[i]);
			if (pIServerUserItem != NULL)
			{
				if (pIServerUserItem->GetUserScore() >= m_lApplyBankerCondition)
				{
					m_wCurrentBanker=m_ApplyUserArray[i];
					break;
				}
				else
				{
					nInvalidApply = i + 1;

					//发送消息
					CMD_S_CancelBanker CancelBanker = {};

					//设置变量
					CancelBanker.wCancelUser=pIServerUserItem->GetChairID();					

					//发送消息
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

					//提示消息
					TCHAR szTipMsg[128] = {};
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("由于你的金币数（%I64d）少于坐庄必须金币数（%I64d）,你无法上庄！"),
						pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
					SendGameMessage(m_ApplyUserArray[i],szTipMsg);
				}
			}
		}
	}

	//删除玩家
	if (nInvalidApply != 0)
		m_ApplyUserArray.RemoveAt(0, nInvalidApply);
}

//发送庄家
void CTableFrameSink::SendApplyUser( IServerUserItem *pRcvServerUserItem )
{
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];

		//获取玩家
		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem) continue;

		//庄家判断
		if (pServerUserItem->GetChairID()==m_wCurrentBanker) continue;

		//构造变量
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser=wChairID;

		//发送消息
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
}


//最大下注
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID, BYTE cbJettonArea)
{
	IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL==pIMeServerUserItem) return 0L;

	//已下注额
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];

	//庄家金币
	LONGLONG lBankerScore=m_lBankerScore;
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lBankerScore-=m_lAllJettonScore[nAreaIndex]* nAreaIndex == POKER_TYPE_INDEX ? MAX_RATE : RATE_PAIR;

	//区域限制
	LONGLONG lAreaLimitScore = cbJettonArea == POKER_TYPE_INDEX ?  m_lAreaLimitScore[1] - m_lAllJettonScore[cbJettonArea] : m_lAreaLimitScore[0] - m_lAllJettonScore[cbJettonArea];

	LONGLONG lMeMaxScore = 0;
	//牌型区域有下注
	if (m_lUserJettonScore[POKER_TYPE_INDEX][wChairID] > 0)
	{
		lNowJetton -= m_lUserJettonScore[POKER_TYPE_INDEX][wChairID];
		lNowJetton += m_lUserJettonScore[POKER_TYPE_INDEX][wChairID] * MAX_RATE;
	}

	//个人限制
	if (cbJettonArea == POKER_TYPE_INDEX)
		lMeMaxScore = min((pIMeServerUserItem->GetUserScore() - lNowJetton) / MAX_RATE, m_lUserLimitScore);
	else
		lMeMaxScore = min(pIMeServerUserItem->GetUserScore() - lNowJetton, m_lUserLimitScore);

	//区域限制
	lMeMaxScore=min(lMeMaxScore,lAreaLimitScore);

	//庄家限制
	if (cbJettonArea == POKER_TYPE_INDEX)
		lMeMaxScore=min(lMeMaxScore,lBankerScore/MAX_RATE);
	else
		lMeMaxScore = min(lMeMaxScore, lBankerScore);

	//非零限制
	ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return (lMeMaxScore);

}
//计算得分
LONGLONG CTableFrameSink::CalculateScore()
{
	//推断输赢
	DeduceWinner(m_bWinRed, m_bWinBlack, m_bWinPoker,m_cbPokerType);

	//游戏记录

	//在添加之前效验20条记录是否已经满了？  如果满了 就先移除第一条  始终保持记录20条数据记录
	if (m_dwRecordCount >= MAX_SCORE_HISTORY)
	{
		MoveMemory(&m_GameRecordArrary[0], &m_GameRecordArrary[1], sizeof(tagServerGameRecord) * 19);
		m_dwRecordCount--;
	}
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_dwRecordCount];
	GameRecord.bWinRed = m_bWinRed;
	GameRecord.bWinBlack = m_bWinBlack;
	GameRecord.bWinPoker = m_bWinPoker;
	GameRecord.cbPokerType= m_bWinRed ? m_cbPokerType[0] : m_cbPokerType[1];
	m_dwRecordCount++;

	BYTE cbWin = m_bWinRed ? 1 : 2;

	//上局成绩
	if (m_cbRBHistory[m_nRBCurrentColumn][m_nRBCurrentRow] != 0)
	{
		//判断是否连赢
		if (m_cbRBHistory[m_nRBCurrentColumn][m_nRBCurrentRow] == cbWin)
		{
			//连赢局数只记录6局
			if (m_nRBCurrentRow+1 >= 6) // 只记录6局 超过6局 重新起一列进行记录
			{
				m_nRBCurrentColumn++;

				if(m_nRBCurrentColumn >= 20)
				{ 
					//删除
					MoveMemory(&m_cbRBHistory[0][0], &m_cbRBHistory[1][0], sizeof(m_cbRBHistory) - sizeof(m_cbRBHistory[0]));
					m_nRBCurrentColumn--;
					ZeroMemory(m_cbRBHistory[m_nRBCurrentColumn], sizeof(m_cbRBHistory[0]));	//清除原本数据
				}
				m_nRBCurrentRow = 0;
			}
			else
			{
				m_nRBCurrentRow++;
			}

			m_cbRBHistory[m_nRBCurrentColumn][m_nRBCurrentRow] = cbWin;
		}
		else  //不是连赢  那么重新起一列进行记录
		{
			m_nRBCurrentColumn++;

			if (m_nRBCurrentColumn >= 20)
			{
				//删除
				MoveMemory(&m_cbRBHistory[0][0], &m_cbRBHistory[1][0], sizeof(m_cbRBHistory) - sizeof(m_cbRBHistory[0]));
				m_nRBCurrentColumn--;
				ZeroMemory(m_cbRBHistory[m_nRBCurrentColumn],sizeof(m_cbRBHistory[0]));  //清除原本数据
			}
			m_nRBCurrentRow = 0;
			m_cbRBHistory[m_nRBCurrentColumn][m_nRBCurrentRow] = cbWin;
		}
	}
	else   //再这里增加一条记录 
	{
		//m_nRBCurrentColumn++;
		if (m_nRBCurrentColumn+1 >= 20)
		{
			//删除
			MoveMemory(&m_cbRBHistory[0][0], &m_cbRBHistory[1][0], sizeof((&m_cbRBHistory[1][0])));
			//m_nRBCurrentColumn--;
		}
		m_nRBCurrentRow = 0;
		m_cbRBHistory[m_nRBCurrentColumn][m_nRBCurrentRow] = cbWin;
	}


	//庄家总量
	LONGLONG lBankerWinScore = 0;

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//默认全是押1付1
	BYTE cbMultiple[] = {1,1,1,1};
	//如果牌型区域赢了的话 按照牌型得到倍率
	if (m_bWinPoker) cbMultiple[POKER_TYPE_INDEX] = m_GameLogic.GetTimes(m_bWinRed ? m_cbPokerType[0] : m_cbPokerType[1]);

	//赢家标识
	bool static bWinFlag[AREA_COUNT+1];
	bWinFlag[0] = false;
	bWinFlag[RED_INDEX] = m_bWinRed;
	bWinFlag[BLACK_INDEX] = m_bWinBlack;
	bWinFlag[POKER_TYPE_INDEX] = m_bWinPoker;
	//计算积分
	for (WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
	{
		//庄家判断
		if (m_wCurrentBanker==wChairID) continue;

		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		for (WORD wAreaIndex = RED_INDEX; wAreaIndex <= POKER_TYPE_INDEX; ++wAreaIndex)
		{
			if (m_lUserJettonScore[wAreaIndex][wChairID] != 0)
			{
				if (true == bWinFlag[wAreaIndex])
				{
					m_lUserWinScore[wChairID] += (m_lUserJettonScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex]);
					//m_lUserReturnScore[wChairID] += m_lUserJettonScore[wAreaIndex][wChairID];
					lBankerWinScore -= (m_lUserJettonScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex]);
				}
				else
				{
					lUserLostScore[wChairID] -= m_lUserJettonScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex];
					lBankerWinScore += m_lUserJettonScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex];
				}
			}
		}
		
		//总的分数
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
		//计算税收
		if (0 < m_lUserWinScore[wChairID])
		{
			//计算税收
			m_lUserRevenue[wChairID] = m_pITableFrame->CalculateRevenue(wChairID, m_lUserWinScore[wChairID]);
		}
	}

	//庄家成绩
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;

		//计算税收
		if (0 < m_lUserWinScore[m_wCurrentBanker])
		{
			m_pITableFrame->CalculateRevenue(m_wCurrentBanker, m_lUserWinScore[m_wCurrentBanker]);
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker] - m_lUserRevenue[m_wCurrentBanker];
		}	
	}

	//离线庄家成绩
	if (m_wOfflineBanker!=INVALID_CHAIR)
	{
		m_lUserWinScore[m_wOfflineBanker] = lBankerWinScore;

		//计算税收
		if (0 < m_lUserWinScore[m_wOfflineBanker])
		{
			m_lUserRevenue[m_wOfflineBanker] = m_pITableFrame->CalculateRevenue(m_wOfflineBanker, m_lUserWinScore[m_wOfflineBanker]);
			lBankerWinScore = m_lUserWinScore[m_wOfflineBanker] - m_lUserRevenue[m_wOfflineBanker];
		}	
	}

	//累计积分
	m_lBankerWinScore += lBankerWinScore;

	//当前积分
	m_lBankerCurGameScore=lBankerWinScore;

	//计算本局游戏记录
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		//生成本局战绩
		tagStatisticsRecord tagRecord;
		tagRecord.lPlayerWinScore = m_lUserWinScore[wChairID];
		//计算下注
		SCORE lBetScoreTmp = 0;
		for (WORD wAreaIndex = RED_INDEX; wAreaIndex <= POKER_TYPE_INDEX; ++wAreaIndex)
		{
			lBetScoreTmp += m_lUserJettonScore[wAreaIndex][wChairID];
		}
		tagRecord.lPlayerBetScore = lBetScoreTmp;

		//判断是否超出20的长度
		if (m_tagUserRecord[wChairID].GetCount() >= 20) {
			m_tagUserRecord[wChairID].RemoveHead();
		}
		m_tagUserRecord[wChairID].AddTail(tagRecord);
	}

	return lBankerWinScore;
}


void CTableFrameSink::DeduceWinner(bool &bWinRed, bool &bWinBlack, bool &bWinPoker,BYTE cbPokerType[])
{
	bWinRed = false;
	bWinBlack = false;
	bWinPoker = false;
	BYTE cbWin = m_GameLogic.CompareCard(m_cbTableCardArray[0], m_cbTableCardArray[1], MAX_CARD, cbPokerType);
	if(cbWin)bWinRed = true;
	else bWinBlack = true;

	BYTE cbPk = bWinRed ? cbPokerType[0] : cbPokerType[1];
	if (cbPk > CT_DOUBLE) bWinPoker = true;

}

//发送记录
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{

	CMD_S_GameHistoryRecord GameRecord;
	ZeroMemory(&GameRecord, sizeof(GameRecord));
	CopyMemory(GameRecord.cbRBHistory, m_cbRBHistory, sizeof(GameRecord.cbRBHistory));

	for (int i = 0; i < m_dwRecordCount; i++)
	{
		GameRecord.cbPokerType[i] = m_GameRecordArrary[i].cbPokerType;
		GameRecord.cbRB_WL_History[i] = m_GameRecordArrary[i].bWinRed ? 1 : 2;
	}
	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_HISTORY_RECORD, &GameRecord, sizeof(GameRecord));
}

//发送消息
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID==INVALID_CHAIR)
	{
		//游戏玩家
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT);
		}

		//旁观玩家
		WORD wIndex=0;
		do {
			IServerUserItem *pILookonServerUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pILookonServerUserItem==NULL) break;

			m_pITableFrame->SendGameMessage(pILookonServerUserItem,pszTipMsg,SMT_CHAT);

		}while(true);
	}
	else
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem!=NULL) m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT|SMT_EJECT);
	}
}

//读取配置
void CTableFrameSink::ReadConfigInformation()
{
	//获取自定义配置
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomConfig);

	//上庄
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_nBankerTimeLimit = pCustomConfig->CustomGeneral.lBankerTime;
	m_nBankerTimeAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lExtraBankerScore = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_nExtraBankerTime = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker = (pCustomConfig->CustomGeneral.nEnableSysBanker == TRUE)?true:false;

	//占位
	CopyMemory(&m_occupyseatConfig, &(pCustomConfig->CustomGeneral.occupyseatConfig), sizeof(m_occupyseatConfig));

	//时间
	m_nFreeTime = pCustomConfig->CustomGeneral.lFreeTime;
	m_nPlaceJettonTime = pCustomConfig->CustomGeneral.lBetTime;
	m_nGameEndTime = pCustomConfig->CustomGeneral.lEndTime;
	if(m_nFreeTime < TIME_FREE	|| m_nFreeTime > 99) m_nFreeTime = TIME_FREE;
	if(m_nPlaceJettonTime < TIME_PLACE_JETTON || m_nPlaceJettonTime > 99) m_nPlaceJettonTime = TIME_PLACE_JETTON;
	if(m_nGameEndTime < TIME_GAME_END || m_nGameEndTime > 99) m_nGameEndTime = TIME_GAME_END;

	//下注
	CopyMemory(m_lAreaLimitScore,(pCustomConfig->CustomGeneral.lAreaLimitScore), sizeof(m_lAreaLimitScore));
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;

	//库存
	m_lStorageStart = pCustomConfig->CustomGeneral.StorageStart;
	m_lStorageCurrent = m_lStorageStart;
	m_nStorageDeduct = pCustomConfig->CustomGeneral.StorageDeduct;
	m_lStorageMax1 = pCustomConfig->CustomGeneral.StorageMax1;
	m_lStorageMul1 = pCustomConfig->CustomGeneral.StorageMul1;
	m_lStorageMax2 = pCustomConfig->CustomGeneral.StorageMax2;
	m_lStorageMul2 = pCustomConfig->CustomGeneral.StorageMul2;
	if(m_lStorageMul1 < 0 || m_lStorageMul1 > 100 ) m_lStorageMul1 = 50;
	if(m_lStorageMul2 < 0 || m_lStorageMul2 > 100 ) m_lStorageMul2 = 80;
	
	//机器人
	m_nRobotListMaxCount = pCustomConfig->CustomAndroid.lRobotListMaxCount;

	LONGLONG lRobotBetMinCount = pCustomConfig->CustomAndroid.lRobotBetMinCount;
	LONGLONG lRobotBetMaxCount = pCustomConfig->CustomAndroid.lRobotBetMaxCount;
	m_nMaxChipRobot = rand()%(lRobotBetMaxCount-lRobotBetMinCount+1) + lRobotBetMinCount;
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 8;
	m_lRobotAreaLimit = pCustomConfig->CustomAndroid.lRobotAreaLimit;
	
	return;
}

// 库存优化
VOID CTableFrameSink::StorageOptimize()
{
	bool blRealPlayerWin = true;
	if (((m_lStorageCurrent > m_lStorageMax1) && (rand()%100 < m_lStorageMul1))
		|| ((m_lStorageCurrent > m_lStorageMax2) && (rand()%100 < m_lStorageMul2))
		|| rand() % 100 < 30)
	{
		blRealPlayerWin = true;
	}
	else
	{
		blRealPlayerWin = false;
	}

	LONGLONG lCirculateCount = 0L;

	while(true)
	{
		//真人玩家得分
		LONGLONG lRealPlayerWinScore = CalSysOrAndroidBankerScore();

		if (lCirculateCount > 10000)
		{
			//blRealPlayerWin = false;
			
			if (m_lStorageCurrent - lRealPlayerWinScore < 0 /*&& blRealPlayerWin == true*/)
			{
				blRealPlayerWin = false;
				lCirculateCount = 0;
				DispatchTableCard();
				continue;
			}

			break;
		}

		//库存不能为负数
		if (m_lStorageCurrent - lRealPlayerWinScore  <= 0)
		{
			DispatchTableCard();
			lCirculateCount++;
			continue;
		}

		//检验最大赔付比例
		LONGLONG lStorageCurrent = m_lStorageCurrent - lRealPlayerWinScore;
		double llCompensateRadio = (double)((double)30 / (double)100);
		if (m_lStorageCurrent * (1 - llCompensateRadio) >= lStorageCurrent)
		{
			DispatchTableCard();
			lCirculateCount++;
			continue;
		}

		//真人玩家输钱
		if (lRealPlayerWinScore <= 0 && blRealPlayerWin == false)
		{
			break;
		}
		
		if (lRealPlayerWinScore >= 0 && blRealPlayerWin == true)
		{
			break;
		}
		
		DispatchTableCard();
		lCirculateCount++;
	}
	
	//庄家赢牌
	//if (m_lStorageCurrent - CalSysOrAndroidBankerScore() < 0)
	//{
	//	CString strFileName;
	//	strFileName.Format(TEXT("百人牛牛[%s]下注 信息.log"), m_pGameServiceOption->szServerName);
	//
	//	WriteInfo(strFileName, TEXT("庄家赢牌"));

	//	m_pServerContro->GetBankerWinResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
	//}
}

//计算系统及机器人坐庄时真人玩家得分
LONGLONG CTableFrameSink::CalSysOrAndroidBankerScore()
{
	
	ASSERT ((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
		|| (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true));

	//比较倍数
	
	bool static bWinRed, bWinBlack, bWinPoker;
	BYTE cbPokerType[2];
	DeduceWinner(bWinRed, bWinBlack, bWinPoker,  cbPokerType);
	BYTE cbMultiple[] = { 1, 1, 1, 1 };
	if (m_bWinPoker) cbMultiple[POKER_TYPE_INDEX] = m_GameLogic.GetTimes(m_bWinRed ? m_cbPokerType[0] : m_cbPokerType[1]);

	bool static bWinFlag[AREA_COUNT + 1];
	bWinFlag[0] = false;
	bWinFlag[RED_INDEX] = bWinRed;
	bWinFlag[BLACK_INDEX] = bWinBlack;
	bWinFlag[POKER_TYPE_INDEX] = bWinPoker;
	


	//系统及机器人坐庄时真人玩家得分
	LONGLONG lRealPlayerWinScore = 0L;

	// 计算系统输赢
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		// 获取玩家
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		// 过滤庄家
		if (wChairID == m_wCurrentBanker || pIServerUserItem == NULL)
		{
			continue;
		}

		//过滤机器人
		if (pIServerUserItem->IsAndroidUser() == true)
		{
			continue;
		}

		// 计算玩家下注
		for (int nAarea = 1; nAarea <= AREA_COUNT; nAarea++)
		{
			if ( m_lUserJettonScore[nAarea+1][wChairID] != 0 )
			{
				if(bWinFlag[nAarea] )
				{				
					lRealPlayerWinScore += m_lUserJettonScore[nAarea+1][wChairID] * cbMultiple[nAarea];
				}
				else
				{
					lRealPlayerWinScore -= m_lUserJettonScore[nAarea+1][wChairID] * cbMultiple[nAarea];
				}
			}
		}
	}

	return lRealPlayerWinScore;
}

//计算真人玩家坐庄时真人的得分
LONGLONG CTableFrameSink::CalRealPlayerBankerScore()
{
	
	ASSERT (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false);

	//比较倍数
	bool static bWinRed, bWinBlack, bWinPoker;
	BYTE cbPokerType[2];
	DeduceWinner(bWinRed, bWinBlack, bWinPoker, cbPokerType);
	BYTE cbMultiple[] = { 1, 1, 1, 1 };
	if (m_bWinPoker) cbMultiple[POKER_TYPE_INDEX] = m_GameLogic.GetTimes(m_bWinRed ? m_cbPokerType[0] : m_cbPokerType[1]);

	bool static bWinFlag[AREA_COUNT + 1];
	bWinFlag[0] = false;
	bWinFlag[RED_INDEX] = bWinRed;
	bWinFlag[BLACK_INDEX] = bWinBlack;
	bWinFlag[POKER_TYPE_INDEX] = bWinPoker;

	LONGLONG lAndroidWinScore = 0L;

	// 计算系统输赢
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		// 获取玩家
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		// 过滤庄家
		if (wChairID == m_wCurrentBanker || pIServerUserItem == NULL)
		{
			continue;
		}

		// 计算玩家下注
		for (int nAarea = 1; nAarea <= AREA_COUNT; nAarea++)
		{
			if ( m_lUserJettonScore[nAarea+1][wChairID] != 0 )
			{
				if(bWinFlag[nAarea] )
				{				
					if (pIServerUserItem->IsAndroidUser() == true)
					{
						lAndroidWinScore += m_lUserJettonScore[nAarea+1][wChairID] * cbMultiple[nAarea];
					}
				}
				else
				{
					if (pIServerUserItem->IsAndroidUser() == true)
					{
						lAndroidWinScore -= m_lUserJettonScore[nAarea+1][wChairID] * cbMultiple[nAarea];
					}
				}
			}
		}
	}
	
	return -lAndroidWinScore;
}

//查询是否扣服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pUserItem==NULL) continue;
		if(wChairID==i)
		{
			//返回下注
			for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) 
			{

				if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
				{
					return true;
				}
			}
			break;
		}
	}
	if (wChairID==m_wCurrentBanker)
	{
		return true;
	}

	//离线庄家
	if (wChairID == m_wOfflineBanker)
	{
		return true;
	}

	return false;
}

//是否衰减
bool CTableFrameSink::NeedDeductStorage()
{

	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		if (!pIServerUserItem->IsAndroidUser())
		{
			for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
			{
				if (m_lUserJettonScore[nAreaIndex][i] != 0)
				{
					return true;
				}
			}
		}
	}

	return false;

}

//发送下注信息
void CTableFrameSink::SendUserBetInfo( IServerUserItem *pIServerUserItem )
{
	if(NULL == pIServerUserItem) return;

	//权限判断
	if(!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())) return;

	//构造数据
	//CMD_S_SendUserBetInfo SendUserBetInfo;
	//ZeroMemory(&SendUserBetInfo, sizeof(SendUserBetInfo));
	//SendUserBetInfo.iResult = 0;
	//CopyMemory(&SendUserBetInfo.lUserJettonScore, m_lUserJettonScore, sizeof(m_lUserJettonScore));

	//发送消息	
	//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_REJETTON, &SendUserBetInfo, sizeof(SendUserBetInfo));
	
	return;
}

bool CTableFrameSink::ReJetton(IServerUserItem *pIServerUserItem)
{
	//用户效验

	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	LONGLONG lLastAllJettonArea = 0L;
	LONGLONG lLastAllJettonPlace = 0L;
	LONGLONG lUserJettonScore = 0L;
	//统计本局最后下注
	for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
	{
		lLastAllJettonArea += m_lLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()];
	}

	//统计上局总下注
	for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
	{
		lLastAllJettonPlace += m_lLastAllJettonPlace[nAreaIndex][pIServerUserItem->GetChairID()];
	}

	//统计本局已下注
	for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
	{
		lUserJettonScore += m_lUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()];
	}

	bool bIsLast = false;
	//最后一注
	for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
	{
		//本局有最后下一注
		if (m_lLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()] > 0)
		{
			bIsLast = false;
			break;
		}
		//本局无最后一注
		if (m_lLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()] == 0)
		{
			bIsLast = true;
		}
	}

	bool bCheckPlaceJettonIsOk = true;

	if (bIsLast) //无最后一注
	{
		if (lUserScore - lUserJettonScore < lLastAllJettonPlace)
		{
			bCheckPlaceJettonIsOk = false;
		}
	}
	else
	{
		if (lUserScore - lUserJettonScore < lLastAllJettonArea)
		{
			bCheckPlaceJettonIsOk = false;
		}
	}
	if (!bCheckPlaceJettonIsOk)
	{
		//用户金币不够
		CMD_S_SendBetInfo SendBetInfo;
		ZeroMemory(&SendBetInfo, sizeof(SendBetInfo));
		//构造变量
		SendBetInfo.iResult = 2;	//重复投注失败 金额不足
		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SEND_REJETTON, &SendBetInfo, sizeof(SendBetInfo));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_REJETTON, &SendBetInfo, sizeof(SendBetInfo));
		return true;
	}

	//用户限制
	if (bIsLast) //无最后一注
	{
		if (lUserJettonScore + lLastAllJettonPlace > m_lUserLimitScore)
		{
			bCheckPlaceJettonIsOk = false;
		}
	}
	else
	{
		if (lUserJettonScore + lLastAllJettonArea > m_lUserLimitScore)
		{
			bCheckPlaceJettonIsOk = false;
		}
	}
	if (!bCheckPlaceJettonIsOk)
	{
		//大于积分限制
		return true;
	}

	//区域限制
	if (bIsLast) //无最后一注
	{
		for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
		{
			if (m_lLastAllJettonPlace[nAreaIndex][pIServerUserItem->GetChairID()] + m_lUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()] > (nAreaIndex == POKER_TYPE_INDEX ? m_lAreaLimitScore[1] : m_lAreaLimitScore[0]))
			{
				bCheckPlaceJettonIsOk = false;
				break;
			}
		}
	}
	else  //有最后一注
	{
		for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
		{
			if (m_lLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()] + m_lUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()] > (nAreaIndex == POKER_TYPE_INDEX ? m_lAreaLimitScore[1] : m_lAreaLimitScore[0]))
			{
				bCheckPlaceJettonIsOk = false;
				break;
			}
		}

	}
	if (!bCheckPlaceJettonIsOk)
	{
		//大于区域限制
		return true;
	}

	//保存数据
	LONGLONG  TemplAllJettonScore[AREA_COUNT + 1];           //全体总注
	LONGLONG  TemplUserJettonScore[AREA_COUNT + 1][GAME_PLAYER];   //个人总注
	LONGLONG  TemplLastAllJettonPlace[AREA_COUNT + 1][GAME_PLAYER]; //上局总下注
	LONGLONG  TempLastAllJettonArea[AREA_COUNT + 1][GAME_PLAYER];   //本局最后下注

	CopyMemory(TemplAllJettonScore, m_lAllJettonScore, sizeof(TemplAllJettonScore));
	CopyMemory(TemplUserJettonScore, m_lUserJettonScore, sizeof(m_lUserJettonScore));
	CopyMemory(TemplLastAllJettonPlace, m_lLastAllJettonPlace, sizeof(m_lLastAllJettonPlace));
	CopyMemory(TempLastAllJettonArea, m_lLastAllJettonArea, sizeof(TempLastAllJettonArea));

	//无最后一注 重复上一局总下注
	if (bIsLast == true)
	{
		if (bCheckPlaceJettonIsOk == true)
		{
			//设置数据
			for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
			{
				//上局总下注 不变

				//本局最后一次操作 不变   0 (游戏结束时清空)

				//全体总注
				TemplAllJettonScore[nAreaIndex] += TemplLastAllJettonPlace[nAreaIndex][pIServerUserItem->GetChairID()];

				//个人总注
				TemplUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()] = TemplLastAllJettonPlace[nAreaIndex][pIServerUserItem->GetChairID()];

			}
		}
	}

	//有最后一注 重复本局最后一注
	else
	{

		//有最后一注 只加一注
		if (bCheckPlaceJettonIsOk == true)
		{
			//设置数据
			for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
			{
				//上局总下注 不变     0 (在本局用户下一注时清空)

				//本局最后一次操作 不变

				//全体总注
				TemplAllJettonScore[nAreaIndex] += TempLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()];

				//个人总注
				TemplUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()] = TempLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()];

			}

		}
	}

	if (bCheckPlaceJettonIsOk)
	{
		//变量定义
		CMD_S_SendBetInfo SendBetInfo;
		ZeroMemory(&SendBetInfo, sizeof(SendBetInfo));

		//构造变量
		SendBetInfo.iResult = 0;	//重复投注成功
		//设置变量
		//CopyMemory(m_lAllJettonScore, TemplAllJettonScore, sizeof(TemplAllJettonScore));
		//CopyMemory(m_lUserJettonScore, TemplUserJettonScore, sizeof(TemplUserJettonScore));
		//CopyMemory(m_lLastAllJettonPlace, TemplLastAllJettonPlace, sizeof(TemplLastAllJettonPlace));
		//CopyMemory(m_lLastAllJettonArea, TempLastAllJettonArea, sizeof(TempLastAllJettonArea));

		for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
		{
			SendBetInfo.lUserJettonScore[nAreaIndex-1] += TemplUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()];
		}

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SEND_REJETTON, &SendBetInfo, sizeof(SendBetInfo));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_REJETTON, &SendBetInfo, sizeof(SendBetInfo));
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////
