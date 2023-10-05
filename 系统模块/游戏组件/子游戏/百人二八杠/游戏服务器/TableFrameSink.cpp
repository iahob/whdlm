#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//常量定义
#define SEND_COUNT					600									//发送次数

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//下注时间
#define IDI_FREE					1									//空闲时间
#ifdef _DEBUG
#define TIME_FREE					6									//空闲时间
#else
#define TIME_FREE					10									//空闲时间
#endif

//下注时间
#define IDI_PLACE_JETTON			2									//下注时间

#ifdef _DEBUG
#define TIME_PLACE_JETTON			5									//下注时间
#else
#define TIME_PLACE_JETTON			15									//下注时间
#endif

//结束时间
#define IDI_GAME_END				3									//结束时间

#ifdef _DEBUG
#define TIME_GAME_END				20									//结束时间
#else
#define TIME_GAME_END				20									//结束时间
#endif

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;				//游戏人数

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//总下注数
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
	
	//个人下注
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//控制变量
	m_cbWinSideControl=0;
	m_nSendCardCount=0;

	//玩家成绩	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//扑克信息
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	ZeroMemory(m_cbTableHavaSendCardArray,sizeof(m_cbTableHavaSendCardArray));
	ZeroMemory(m_cbTableHavaSendCount,sizeof(m_cbTableHavaSendCount));


	//状态变量
	m_dwJettonTime=0L;

	//庄家信息
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker=INVALID_CHAIR;
	m_wBankerTime=0;
	m_lBankerWinScore=0L;		
	m_lBankerCurGameScore=0L;
	m_bEnableSysBanker=true;
	m_cbLeftCardCount=0;
	m_bContiueCard=true;

	//记录变量
	ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));
	m_nRecordFirst=0;
	m_nRecordLast=0;
	m_dwRecordCount=0;

	//设置文件名
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	myprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\28GangBattleConfig.ini"),szPath);

	

	//机器人控制
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	m_nProbeTimes = 0;
	m_lStorageStart = 0;
	m_nStorageDeduct = 0;
	m_nDispatchCardTime = 0;
	m_bCanCatchBanker = false;
	m_nBankerReduceCoin=30000;

	//服务控制
	m_hInst = NULL;
	m_pServerContro = NULL;
	m_hInst = LoadLibrary(TEXT("28GangBattleServerControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}

	//库存
	m_StorageWin=0;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
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
}

//接口查询
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//游戏配置
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//房间名称
	memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));


	//控制变量
	LONGLONG lMaxScore=2147483647;
	m_lApplyBankerCondition=GetPrivateProfileInt(m_szRoomName, TEXT("Score"), 100, m_szConfigFileName);
	m_lAreaLimitScore=GetPrivateProfileInt(m_szRoomName, TEXT("AreaLimitScore"), (int)lMaxScore, m_szConfigFileName);
	m_lUserLimitScore=GetPrivateProfileInt(m_szRoomName, TEXT("UserLimitScore"), (int)lMaxScore, m_szConfigFileName);
	BYTE cbEnableSysBanker=GetPrivateProfileInt(m_szRoomName, TEXT("EnableSysBanker"), 1, m_szConfigFileName);
	m_bEnableSysBanker=cbEnableSysBanker?true:false;

	m_nMaxChipRobot = 10;
	ReadConfigInformation(true);
	return true;
}

//复位桌子
VOID  CTableFrameSink::RepositionSink()
{
	//总下注数
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
	
	//个人下注
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//玩家成绩	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//机器人控制
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	m_nProbeTimes = 0;

	//库存
	m_StorageWin=0;

	return;
}

//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{
	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//获取庄家
	IServerUserItem *pIBankerServerUserItem=NULL;
	if (INVALID_CHAIR!=m_wCurrentBanker) 
	{
		pIBankerServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		m_lBankerScore=pIBankerServerUserItem->GetUserScore();
	}

	//设置变量
	GameStart.cbTimeLeave=m_nPlaceJettonTime;
	GameStart.wBankerUser=m_wCurrentBanker;
	if (pIBankerServerUserItem!=NULL) GameStart.lBankerScore=m_lBankerScore;
	GameStart.bContiueCard=m_bContiueCard;

	//m_wBankerTime

	GameStart.cbLeftCardCount=m_cbLeftCardCount+8;

	for (int i = 0;i<4;i++)
	{	
		CopyMemory(GameStart.cbTableHavaSendCardArray[i],m_cbTableHavaSendCardArray[i],CountArray(m_cbTableHavaSendCardArray[i]));
	}

	CopyMemory(GameStart.cbTableHavaSendCount,m_cbTableHavaSendCount,CountArray(m_cbTableHavaSendCount));

    
	//下注机器人数量
	int nChipRobotCount = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}

	GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);


    // 机器人列表人数
    nChipRobotCount = 0;
    for (int i = 0; i < m_ApplyUserArray.GetCount(); i++) 
    {
        IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray.GetAt(i));
        if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
            nChipRobotCount++;
    }

    if(nChipRobotCount > 0)
        GameStart.nAndriodCount=nChipRobotCount-1;

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
		GameStart.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore);

		m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	
	}

	return true;
}

//游戏结束
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束	
		{
			bool bSuccess = false;
			m_nProbeTimes = 0;
			m_StorageWin=0;
			if(m_pServerContro && m_pServerContro->NeedControl())
			{
				//WinnerCard();//星之光注释
				//获取控制目标牌型
				m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
				m_pServerContro->CompleteControl();
				ProbeJudge();
			}
			else
			{
				do 
				{
					//派发扑克
					DispatchTableCard();
					//试探性判断
					bSuccess = ProbeJudge();

				} while (!bSuccess);
			}

			//计算分数
			LONGLONG lBankerWinScore=CalculateScore(false);

			//递增次数
			m_wBankerTime++;
			m_nDispatchCardTime++;
			if (m_wBankerTime==1)
			{
				m_bContiueCard = true;
			}
			//结束消息
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//庄家信息
			GameEnd.nBankerTime = m_wBankerTime;
			GameEnd.lBankerTotallScore=m_lBankerWinScore;
			GameEnd.lBankerScore=lBankerWinScore;
			GameEnd.bcFirstCard = m_bcFirstPostCard;
			GameEnd.bcNextCard = m_bcNextPostCard;
			//扑克信息
			CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));

			GameEnd.cbLeftCardCount=m_cbLeftCardCount+8;
			for (int i = 0;i<4;i++)
			{
				CopyMemory(&m_cbTableHavaSendCardArray[i][m_cbTableHavaSendCount[i]],GameEnd.cbTableCardArray[i],2);		
				m_cbTableHavaSendCount[i]+=2;
				CopyMemory(GameEnd.cbTableHavaSendCardArray[i],m_cbTableHavaSendCardArray[i],CountArray(m_cbTableHavaSendCardArray[i]));
			}
			
			CopyMemory(GameEnd.cbTableHavaSendCount,m_cbTableHavaSendCount,CountArray(m_cbTableHavaSendCount));
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
				if (m_lUserRevenue[wUserIndex]>0) GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
				else if (m_wCurrentBanker!=INVALID_CHAIR) GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
				else GameEnd.lRevenue=0;

				//发送消息					
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			return true;
		}
	case GER_USER_LEAVE:		//用户离开
	case GER_NETWORK_ERROR:
		{
			
			//闲家判断
			if (m_wCurrentBanker!=wChairID)
			{
				
				//是否开牌
				if (m_pITableFrame->GetGameStatus() == GS_PLACE_JETTON)
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
					//修改积分
					tagScoreInfo ScoreInfoArray[GAME_PLAYER];
					ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

					//变量定义
					LONGLONG lScore=0;
					LONGLONG lRevenue=0;
				


					//库存金币
					if (!pIServerUserItem->IsAndroidUser())
						m_lStorageStart -= m_lUserWinScore[wChairID];

					ScoreInfoArray[wChairID].lRevenue=m_lUserRevenue[wChairID];
					ScoreInfoArray[wChairID].lScore=m_lUserWinScore[wChairID];;
					ScoreInfoArray[wChairID].cbType=(m_lUserWinScore[wChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;;

					//写入积分
					//if (lScore!=0L) m_pITableFrame->WriteUserScore(pIServerUserItem, lScore,lRevenue, ScoreKind);
					m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

					m_lUserRevenue[wChairID]=0;
					m_lUserWinScore[wChairID]=0;

					return true;
				}
				
				
				
				
			}
			

			//状态判断
			if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
			{
				//提示消息
				TCHAR szTipMsg[128];
				myprintf(szTipMsg,CountArray(szTipMsg),TEXT("由于庄家[ %s ]强退，游戏提前结束！"),pIServerUserItem->GetNickName());

				//发送消息
				SendGameMessage(INVALID_CHAIR,szTipMsg);	

				//设置状态
				m_pITableFrame->SetGameStatus(GS_GAME_END);

				//设置时间
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);

				bool bSuccess = false;
				if(m_pServerContro && m_pServerContro->NeedControl())
				{
					//WinnerCard();//星之光注释
					//获取控制目标牌型
					m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
					m_pServerContro->CompleteControl();
					ProbeJudge();
				}
				else
				{
					do 
					{
						//派发扑克
						DispatchTableCard();
						//试探性判断
						bSuccess = ProbeJudge();
					} while (!bSuccess);
				}

				//计算分数
				CalculateScore(false);

				//结束消息
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));

				//庄家信息
				GameEnd.nBankerTime = m_wBankerTime;
				GameEnd.lBankerTotallScore=m_lBankerWinScore;
				if (m_lBankerWinScore>0) GameEnd.lBankerScore=0;

				//扑克信息
				CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
				GameEnd.cbLeftCardCount=m_cbLeftCardCount;

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
					if (m_lUserRevenue[wUserIndex]>0) GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
					else if (m_wCurrentBanker!=INVALID_CHAIR) GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
					else GameEnd.lRevenue=0;

					//发送消息					
					m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				}
			}

			//修改积分
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

			ScoreInfoArray[m_wCurrentBanker].lScore=m_lUserWinScore[m_wCurrentBanker];
			ScoreInfoArray[m_wCurrentBanker].cbType=SCORE_TYPE_FLEE;

			//扣除分数
			if (m_lUserWinScore[m_wCurrentBanker]<0)
			{
				//m_pITableFrame->WriteUserScore(m_wCurrentBanker,m_lUserWinScore[m_wCurrentBanker], m_lUserRevenue[m_wCurrentBanker], enScoreKind_Flee);

				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
			}


			//库存金币
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if (pIServerUserItem != NULL && !pIServerUserItem->IsAndroidUser())
				m_lStorageStart -= m_lUserWinScore[m_wCurrentBanker];

			m_lUserWinScore[m_wCurrentBanker] = 0;

			//切换庄家
			ChangeBanker(true);

			return true;
		}
	}

	return false;
}

//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:			//空闲状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);

			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));			

			//控制信息
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			StatusFree.lAreaLimitScore = m_lAreaLimitScore;

			//扑克信息
			StatusFree.cbLeftCardCount=m_cbLeftCardCount;
			for (int i = 0;i<4;i++)
			{	
				CopyMemory(StatusFree.cbTableHavaSendCardArray[i],m_cbTableHavaSendCardArray[i],CountArray(m_cbTableHavaSendCardArray[i]));
			}

			CopyMemory(StatusFree.cbTableHavaSendCount,m_cbTableHavaSendCount,CountArray(m_cbTableHavaSendCount));

			//庄家信息
			StatusFree.bEnableSysBanker=m_bEnableSysBanker;
			StatusFree.wBankerUser=m_wCurrentBanker;	
			StatusFree.cbBankerTime=m_wBankerTime;
			StatusFree.lBankerWinScore=m_lBankerWinScore;
			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusFree.lBankerScore=pIServerUserItem->GetUserScore();
			}

			//玩家信息
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusFree.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore); 
			}

			//房间名称
			CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));

			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(m_nFreeTime-(__min(dwPassTime,m_nFreeTime)));

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			//限制提示
			TCHAR szTipMsg[128];
			myprintf(szTipMsg,CountArray(szTipMsg),TEXT("本房间上庄条件为：%I64d,区域限制为：%I64d,玩家限制为：%I64d"),m_lApplyBankerCondition,
				m_lAreaLimitScore,m_lUserLimitScore);
			
			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
						
			//发送申请者
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	case GS_PLACE_JETTON:		//游戏状态
	case GS_GAME_END:			//结束状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);		

			//构造数据
			CMD_S_StatusPlay StatusPlay={0};

			//全局下注
			CopyMemory(StatusPlay.lAllJettonScore,m_lAllJettonScore,sizeof(StatusPlay.lAllJettonScore));

			//玩家下注
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
					StatusPlay.lUserJettonScore[nAreaIndex] = m_lUserJettonScore[nAreaIndex][wChiarID];

				//最大下注
				StatusPlay.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore);
			}

			//控制信息
			StatusPlay.lApplyBankerCondition=m_lApplyBankerCondition;		
			StatusPlay.lAreaLimitScore=m_lAreaLimitScore;		

			//庄家信息
			StatusPlay.bEnableSysBanker=m_bEnableSysBanker;
			StatusPlay.wBankerUser=m_wCurrentBanker;			
			StatusPlay.cbBankerTime=m_wBankerTime;
			StatusPlay.lBankerWinScore=m_lBankerWinScore;	


			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			for (int i = 0;i<4;i++)
			{
				CopyMemory(StatusPlay.cbTableHavaSendCardArray[i],m_cbTableHavaSendCardArray[i],CountArray(m_cbTableHavaSendCardArray[i]));
			}

			CopyMemory(StatusPlay.cbTableHavaSendCount,m_cbTableHavaSendCount,CountArray(m_cbTableHavaSendCount));


			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusPlay.lBankerScore=pIServerUserItem->GetUserScore();
			}

			//房间名称
			CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));

			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusPlay.cbTimeLeave=(BYTE)(m_nFreeTime-(__min(dwPassTime,m_nFreeTime)));
			StatusPlay.cbGameStatus=m_pITableFrame->GetGameStatus();			

			//结束判断
			if (cbGameStatus==GS_GAME_END)
			{
				//设置成绩
				StatusPlay.lEndUserScore=m_lUserWinScore[wChiarID];

				//返还积分
				StatusPlay.lEndUserReturnScore=m_lUserReturnScore[wChiarID];

				//设置税收
				if (m_lUserRevenue[wChiarID]>0) StatusPlay.lEndRevenue=m_lUserRevenue[wChiarID];
				else if (m_wCurrentBanker!=INVALID_CHAIR) StatusPlay.lEndRevenue=m_lUserRevenue[m_wCurrentBanker];
				else StatusPlay.lEndRevenue=0;

				//庄家成绩
				StatusPlay.lEndBankerScore=m_lBankerCurGameScore;

				//扑克信息
				CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			}

			//发送申请者
			SendApplyUser( pIServerUserItem );

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	return false;
}

//定时器事件
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE:		//空闲时间
		{
			if (m_wCurrentBanker==INVALID_CHAIR)
			{
				TrySelectBanker();
			}
			//开始游戏
			//m_pITableFrameControl->StartGame();

			m_pITableFrame->StartGame();

			//设置时间
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_nPlaceJettonTime*1000,1,0L);

			//设置状态
			m_pITableFrame->SetGameStatus(GS_PLACE_JETTON);

			return true;
		}
	case IDI_PLACE_JETTON:		//下注时间
		{
			//状态判断(防止强退重复设置)
			if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
			{
				//设置状态
				m_pITableFrame->SetGameStatus(GS_GAME_END);			

				//结束游戏
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

				//设置时间
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);			
			}

			return true;
		}
	case IDI_GAME_END:			//结束游戏
		{
			
			//修改积分
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
			
			//写入积分
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) continue;

				//胜利类型
				//enScoreKind ScoreKind=(m_lUserWinScore[wUserChairID]>0L)?enScoreKind_Win:enScoreKind_Lost;


				ScoreInfoArray[wUserChairID].lScore=m_lUserWinScore[wUserChairID];
				ScoreInfoArray[wUserChairID].cbType=(m_lUserWinScore[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;;
				ScoreInfoArray[wUserChairID].lRevenue=m_lUserRevenue[wUserChairID];
				//写入积分
				//if (m_lUserWinScore[wUserChairID]!=0L) m_pITableFrame->WriteUserScore(wUserChairID,m_lUserWinScore[wUserChairID], m_lUserRevenue[wUserChairID], ScoreKind);

				if (!pIServerUserItem->IsAndroidUser())
					m_lStorageStart -= m_lUserWinScore[wUserChairID];

				//坐庄判断
				LONGLONG lUserScore=pIServerUserItem->GetUserScore();
				if (wUserChairID!=m_wCurrentBanker && lUserScore<m_lApplyBankerCondition)
				{
					for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						if (wUserChairID!=m_ApplyUserArray[i]) continue;

						//删除玩家
						m_ApplyUserArray.RemoveAt(i);

						//发送消息
						CMD_S_CancelBanker CancelBanker;
						ZeroMemory(&CancelBanker,sizeof(CancelBanker));

						//设置变量
						lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

						//发送消息
						m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
						m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

						//提示消息
						TCHAR szTipMsg[128];
						myprintf(szTipMsg,CountArray(szTipMsg),TEXT("由于你的金币数少于坐庄必须金币数（%I64d）,你自动下庄！"),m_lApplyBankerCondition);
						SendGameMessage(wUserChairID,szTipMsg);

						break;
					}
				}
			}

			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			
			//读取配置
			//if (m_bRefreshCfg)
			ReadConfigInformation(true,false);

			//切换庄家
			ChangeBanker(false);

			TrySelectBanker();

			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,0L);

			//发送消息
			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree,sizeof(GameFree));
			GameFree.cbTimeLeave=m_nFreeTime;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));

			return true;
		}
	}

	return false;
}

//选择庄家
void CTableFrameSink::TrySelectBanker()
{
	if (m_wCurrentBanker==INVALID_CHAIR&&m_ApplyUserArray.GetCount()>0)
	{
		////需要重新抢庄
		//if (m_bReCatchBanker)
		//{
		//	//提示消息
		//	TCHAR szTipMsg[128];
		//	myprintf(szTipMsg,CountArray(szTipMsg),TEXT("重新抢庄！"));

		//	//发送消息
		//	SendGameMessage(INVALID_CHAIR,szTipMsg);	

		//	//发送消息
		//	CMD_S_CatchBanker CatchBanker;
		//	ZeroMemory(&CatchBanker,sizeof(CatchBanker));
		//	CatchBanker.wCatchUser=INVALID_CHAIR;

		//	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));
		//	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));

		//	m_bCanCatchBanker=true;

		//	return;
		//}
		
		m_wCurrentBanker = m_ApplyUserArray[0];

		//切换判断
		{
			//设置变量
			m_wBankerTime = 0;
			m_lBankerWinScore=0;
			m_nDispatchCardTime = 0;
			m_bContiueCard = false;

			//发送消息
			CMD_S_ChangeBanker ChangeBanker;
			ZeroMemory(&ChangeBanker,sizeof(ChangeBanker));
			ChangeBanker.wBankerUser=m_wCurrentBanker;
			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				ChangeBanker.lBankerScore=pIServerUserItem->GetUserScore();
			}
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));

			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				//读取消息
				LONGLONG lMessageCount=GetPrivateProfileInt(m_szRoomName,TEXT("MessageCount"),0,m_szConfigFileName);
				if (lMessageCount!=0)
				{
					//读取配置
					LONGLONG lIndex=rand()%lMessageCount;
					TCHAR szKeyName[32],szMessage1[256],szMessage2[256];				
					myprintf(szKeyName,CountArray(szKeyName),TEXT("Item%I64d"),lIndex);
					GetPrivateProfileString(m_szRoomName,szKeyName,TEXT("恭喜[ %s ]上庄"),szMessage1,CountArray(szMessage1),m_szConfigFileName);

					//获取玩家
					IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

					//发送消息
					myprintf(szMessage2,CountArray(szMessage2),szMessage1,pIServerUserItem->GetNickName());
					SendGameMessage(INVALID_CHAIR,szMessage2);
				}
			}
		}

		m_bCanCatchBanker=true;

		//发送消息
		CMD_S_CatchBanker CatchBanker;
		ZeroMemory(&CatchBanker,sizeof(CatchBanker));
		CatchBanker.wCatchUser=INVALID_CHAIR;

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));
	}
}

//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			if (wDataSize!=sizeof(CMD_C_PlaceJetton)) return false;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//消息处理
			CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pDataBuffer;
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
	case SUB_C_CATCH_BANKER:        //用户抢庄
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;

			return OnUserCatchBanker(pIServerUserItem);	
		}
	case SUB_C_SELECT_CARD_MODE:    //发牌模式
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;
			//消息处理
			CMD_C_SelectCardMode * pMode=(CMD_C_SelectCardMode *)pDataBuffer;

			CMD_S_SelectCardMode  SeletctCardMode;
			SeletctCardMode.cbPutCardMode = pMode->cbPutCardMode;
			SeletctCardMode.cbSendCardMode = pMode->cbSendCardMode;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if ( pIServerUserItem == NULL )
					continue;
				m_pITableFrame->SendTableData(i,SUB_S_SELECT_CARD_MODE,&SeletctCardMode,sizeof(SeletctCardMode));
					
			}

			//提示消息
			TCHAR szTipMsg[128];
			myprintf(szTipMsg,CountArray(szTipMsg),TEXT("庄家[ %s ]重置发牌模式！"),pIServerUserItem->GetNickName());

			//发送消息
			SendGameMessage(INVALID_CHAIR,szTipMsg);	

			return true;

		}
	case SUB_C_CONTINUE_CARD:		//继续发牌
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;
			if (pUserData->wChairID!=m_wCurrentBanker) return true;
			if (m_cbLeftCardCount < 8) return true;

			if (!m_bContiueCard)
			{
				//设置变量
				m_bContiueCard=true;

				//发送消息
				SendGameMessage(pUserData->wChairID,TEXT("设置成功，下一局将继续发牌！"));

			}
			
			return true;
		}
	case SUB_C_REDISPATCH_CARD:		//重新洗牌
		{

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;
			if (pUserData->wChairID!=m_wCurrentBanker) return true;
			
			//设置变量
			m_bContiueCard=false;

			//发送消息
			SendGameMessage(pUserData->wChairID,TEXT("设置成功，下一局将重新洗牌！"));
			return true;
		}
	case SUB_C_AMDIN_COMMAND:
		{
			ASSERT(wDataSize==sizeof(CMD_C_AdminReq));
			if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

			if ( m_pServerContro == NULL)
				return false;

			return m_pServerContro->ServerControl(wSubCmdID, pDataBuffer, wDataSize, pIServerUserItem, m_pITableFrame);
		}
	}

	return false;
}

//框架消息处理
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//设置时间
	if ((bLookonUser==false)&&(m_dwJettonTime==0L))
	{
		m_dwJettonTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
	}

	//限制提示
	TCHAR szTipMsg[128];
	myprintf(szTipMsg,CountArray(szTipMsg),TEXT("本房间上庄条件为：%I64d,区域限制为：%I64d,玩家限制为：%I64d"),m_lApplyBankerCondition,
		m_lAreaLimitScore,m_lUserLimitScore);
	m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);

	return true;
}

//用户起来
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
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
			lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}

		return true;
	}

	return true;
}

//加注事件
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
	//效验参数
	ASSERT((cbJettonArea<=ID_JIAO_R && cbJettonArea>=ID_SHANG_MEN)&&(lJettonScore>0L));
	if ((cbJettonArea>ID_JIAO_R)||(lJettonScore<=0L) || cbJettonArea<ID_SHANG_MEN) return false;

	//效验状态
	//ASSERT(m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON);
	if (m_pITableFrame->GetGameStatus()!=GS_PLACE_JETTON)
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea=cbJettonArea;
		PlaceJettonFail.lPlaceScore=lJettonScore;
		PlaceJettonFail.wPlaceUser=wChairID;

		//发送消息
		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
		return true;
	}

	//庄家判断
	if (m_wCurrentBanker==wChairID) return true;
	if (m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR) return true;

	//变量定义
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	LONGLONG lJettonCount=0L;
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];

	//玩家积分
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//合法校验
	if (lUserScore < lJettonCount + lJettonScore) return true;
	if (m_lUserLimitScore < lJettonCount + lJettonScore) return true;

	//成功标识
	bool bPlaceJettonSuccess=true;

	//合法验证
	if (GetUserMaxJetton(wChairID) >= lJettonScore)
	{
		
		//机器人验证
		if(pIServerUserItem->IsAndroidUser())
		{
			//long lTemp = lJettonScore;
			//MyDebug(_T(" [机器人%d ,最大:%d,请求: %I64d]\r\n"),pIServerUserItem->GetUserID(),GetUserMaxJetton(wChairID),lJettonScore);
			
			//区域限制
			if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lAreaLimitScore)//m_lRobotAreaLimit
				return true;

			//数目限制
			bool bHaveChip = false;
			for (int i = 0; i < AREA_COUNT; i++)
			{
				if (m_lUserJettonScore[i+1][wChairID] != 0)
					bHaveChip = true;
			}

			if (!bHaveChip && bPlaceJettonSuccess)
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
		//变量定义
		CMD_S_PlaceJetton PlaceJetton;
		ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

		//构造变量
		PlaceJetton.wChairID=wChairID;
		PlaceJetton.cbJettonArea=cbJettonArea;
		PlaceJetton.lJettonScore=lJettonScore;
		PlaceJetton.bIsAndroid=pIServerUserItem->IsAndroidUser();

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
	}
	else
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea=cbJettonArea;
		PlaceJettonFail.lPlaceScore=lJettonScore;
		PlaceJettonFail.wPlaceUser=wChairID;

		//发送消息
		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
	}

	return true;
}

//发送扑克
bool CTableFrameSink::DispatchTableCard()
{
	//继续发牌
	int const static nDispatchCardCount=8;
	bool bContiueCard=m_bContiueCard;
	if (m_cbLeftCardCount<nDispatchCardCount) bContiueCard=false;

	//继续发牌
	if (true==bContiueCard)
	{
		//重新洗牌
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];

		//设置扑克
		CopyMemory(&m_cbTableCardArray[0][0],m_cbTableCard,sizeof(m_cbTableCardArray));
		m_cbLeftCardCount -= nDispatchCardCount;
		if (m_cbLeftCardCount>0) CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
	}
	else
	{
		//重新洗牌
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];
		 for (int i = 0;i<2;i++)
		 {
			 m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));	
		 }
		//设置扑克
		CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));

		m_cbLeftCardCount=CountArray(m_cbTableCard)-nDispatchCardCount;
		CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
				
		for (int i = 0;i<4;i++){ memset(&m_cbTableHavaSendCardArray[i],0,2) ;m_cbTableHavaSendCount[i] = 0; }

	}

	//发牌标志
	//m_bContiueCard=false;
    return true;
}
void CTableFrameSink::RandList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{

		//混乱准备
		BYTE *cbCardData = new BYTE[cbBufferCount];
		CopyMemory(cbCardData,cbCardBuffer,cbBufferCount);

		//混乱扑克
		BYTE cbRandCount=0,cbPosition=0;
		do
		{
			cbPosition=rand()%(cbBufferCount-cbRandCount);
			cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
			cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
		} while (cbRandCount<cbBufferCount);

		delete []cbCardData;
		cbCardData = NULL;

		return;
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
			//CMD_S_ApplyBanker ApplyBanker;
			//ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));
			////设置变量
			//ApplyBanker.wApplyUser=wApplyUserChairID;
			//m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_FAIL, &ApplyBanker, sizeof(ApplyBanker));
			//m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_FAIL, &ApplyBanker, sizeof(ApplyBanker));
			return true;
		}
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
	if (m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE && m_ApplyUserArray.GetCount()==1)
	{
		ChangeBanker(false);
//#ifdef _DEBUG
//
//		WORD wChairID=m_ApplyUserArray[0];
//		m_wCurrentBanker = wChairID;
//		//发送消息
//		CMD_S_ChangeBanker ChangeBanker;
//		ZeroMemory(&ChangeBanker,sizeof(ChangeBanker));
//		ChangeBanker.wBankerUser=m_wCurrentBanker;
//		if (m_wCurrentBanker!=INVALID_CHAIR)
//		{
//			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
//			ChangeBanker.lBankerScore=pIServerUserItem->GetUserScore();
//		}
//		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));
//		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));
//
//#endif
	}

	return true;
}

//取消申请
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//当前庄家
	if (pICancelServerUserItem->GetChairID()==m_wCurrentBanker && m_pITableFrame->GetGameStatus()!=GAME_STATUS_FREE)
	{
		//发送消息
		m_pITableFrame->SendGameMessage(pICancelServerUserItem,TEXT("游戏已经开始，不可以取消当庄！"),SMT_CHAT|SMT_EJECT);

	/*	CMD_S_bCanCancelBanker CanCelBanker;
		CanCelBanker.blCancel = false;
		CanCelBanker.wChariID = m_wCurrentBanker;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_CAN, &CanCelBanker, sizeof(CanCelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_CAN, &CanCelBanker, sizeof(CanCelBanker));*/
		return true;
	}

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

		if (m_wCurrentBanker!=wChairID)
		{
			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//设置变量
			lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker==wChairID)
		{
			//切换庄家 
			m_wCurrentBanker=INVALID_CHAIR;
		}
	
		return true;
	}

	return true;
}

//申请抢庄
bool CTableFrameSink::OnUserCatchBanker(IServerUserItem *pIApplyServerUserItem)
{
	
	if (m_pITableFrame->GetGameStatus()!=GAME_STATUS_FREE) 
        return true;



	if (pIApplyServerUserItem->GetUserScore()-m_nBankerReduceCoin<m_lApplyBankerCondition) 
	{
		//提示消息
		TCHAR szTipMsg[128];
		myprintf(szTipMsg,CountArray(szTipMsg),TEXT("扣除%I64d金币后不能满足上庄条件，所以抢庄失败！"),m_nBankerReduceCoin);
		SendGameMessage(pIApplyServerUserItem->GetChairID(),szTipMsg);	
		return true;

	}

	
	//存在判断
	WORD wApplyUserChairID=pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];
		if (wChairID==wApplyUserChairID)
		{

			if (m_bCanCatchBanker)
			{
				m_bCanCatchBanker = false;
				//排在列表的第一位
				m_ApplyUserArray.RemoveAt(nUserIdx);
				m_ApplyUserArray.InsertAt(0,wApplyUserChairID);
				//提示消息
				TCHAR szTipMsg[128];
				myprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ] 抢庄成功,系统扣除金币%I64d"),pIApplyServerUserItem->GetNickName(),m_nBankerReduceCoin);

				//发送消息
				SendGameMessage(INVALID_CHAIR,szTipMsg);	

				//发送消息
				CMD_S_CatchBanker CatchBanker;
				ZeroMemory(&CatchBanker,sizeof(CatchBanker));
				CatchBanker.wCatchUser=wApplyUserChairID;
				
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));

				//胜利类型
				//enScoreKind ScoreKind=enScoreKind_Lost;
				LONGLONG lReduceScore=-m_nBankerReduceCoin;

				//修改积分
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

				ScoreInfoArray[wChairID].lScore=lReduceScore;
				ScoreInfoArray[wChairID].cbType=SCORE_TYPE_SERVICE;

				//写入积分
				//m_pITableFrame->WriteUserScore(pIApplyServerUserItem->GetChairID(),lReduceScore, 0, ScoreKind);
				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

				
			}

			return true;
		}
	}

	return true;
}

//更换庄家
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	//做庄次数
	WORD wBankerTime=GetPrivateProfileInt(m_szRoomName, TEXT("Time"), 10, m_szConfigFileName);
	WORD wAddTime=GetPrivateProfileInt(m_szRoomName, TEXT("TimeAdd"), 5, m_szConfigFileName);
    LONGLONG lBigScore = 0; 

	bool bChangeBanker =false;

	for(int i = 0;i<m_ApplyUserArray.GetCount();i++)
	{
		    WORD wChairID=m_ApplyUserArray[i];
			if(wChairID==m_wCurrentBanker)
			{
				continue;
			}
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
			if(pIServerUserItem->GetUserScore()>lBigScore)
			{
				lBigScore = pIServerUserItem->GetUserScore();

			}	
	}
	if(m_wCurrentBanker!=INVALID_CHAIR)
	{
		IServerUserItem *pTempIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if(pTempIServerUserItem!=NULL)
		{
			LONGLONG lBankerTempScore=pTempIServerUserItem->GetUserScore();

			if(lBigScore<lBankerTempScore)
			{
				wBankerTime+=wAddTime;
			}
		}
		
	}
	

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

		m_wBankerTime = 0;
		m_lBankerWinScore=0;
		m_nDispatchCardTime = 0;
		m_bContiueCard = false;
		bChangeBanker=true;
	}

	//轮庄判断
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		//获取庄家
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

		if(pIServerUserItem!= NULL)
		{
			LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

			//次数判断
			if (wBankerTime<=m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
			{
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

				//设置庄家
				m_wCurrentBanker=INVALID_CHAIR;
				//提示消息
				TCHAR szTipMsg[128];
				if (lBankerScore<m_lApplyBankerCondition)
					myprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]分数少于(%I64d)，强行换庄!"),pIServerUserItem->GetNickName(),m_lApplyBankerCondition);
				else
					myprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]做庄次数达到(%d)，强行换庄!"),pIServerUserItem->GetNickName(),wBankerTime);

				//发送消息
				SendGameMessage(INVALID_CHAIR,szTipMsg);	
				bChangeBanker=true;
			}

		}
		else
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
		}
		
	}

	if (bChangeBanker)
	{
		//设置变量
		m_wBankerTime = 0;
		m_lBankerWinScore=0;

		//发送消息
		CMD_S_ChangeBanker ChangeBanker;
		ZeroMemory(&ChangeBanker,sizeof(ChangeBanker));
		ChangeBanker.wBankerUser=m_wCurrentBanker;
		if (m_wCurrentBanker!=INVALID_CHAIR)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			ChangeBanker.lBankerScore=pIServerUserItem->GetUserScore();
		}
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));

	}
	

	return true;
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

//用户断线
bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) 
{
	//切换庄家
//	if (wChairID==m_wCurrentBanker) ChangeBanker(true);

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
		lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}

	return true;
}

//最大下注
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID)
{
	//获取玩家
	IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL==pIMeServerUserItem) return 0L;

	//已下注额
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
	
	//庄家金币
	LONGLONG lBankerScore=2147483647;
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		lBankerScore=m_lBankerScore;
	}

	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) 
	{
		lBankerScore-=m_lAllJettonScore[nAreaIndex];
		lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];
	}

	//个人限制
	LONGLONG lMeMaxScore = min((pIMeServerUserItem->GetUserScore()-lNowJetton), m_lUserLimitScore);

	//区域限制
	lMeMaxScore=min(lMeMaxScore,m_lAreaLimitScore);

	//庄家限制
	lMeMaxScore=min(lMeMaxScore,lBankerScore);
	
	lMeMaxScore = max(lMeMaxScore, 0);

	return lMeMaxScore;
}

//计算得分
LONGLONG CTableFrameSink::CalculateScore(bool bProbeJudgeScore)
{
	//变量定义
	float static cbRevenue=m_pGameServiceOption->wRevenueRatio;

	//推断玩家
	bool static bWinShangMen, bWinTianMen, bWinXiaMen;
	DeduceWinner(bWinShangMen, bWinTianMen, bWinXiaMen);

	if (!bProbeJudgeScore)
	{
		//游戏记录
		tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
		GameRecord.bWinShangMen=bWinShangMen;
		GameRecord.bWinTianMen=bWinTianMen;
		GameRecord.bWinXiaMen=bWinXiaMen;

		//移动下标
		m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
		if ( m_nRecordLast == m_nRecordFirst ) m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
	}
	

	//庄家总量
	LONGLONG lBankerWinScore = 0;

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//玩家下注
	LONGLONG *const pUserScore[] = {NULL, m_lUserJettonScore[ID_SHANG_MEN], m_lUserJettonScore[ID_JIAO_L], m_lUserJettonScore[ID_QIAO],
		m_lUserJettonScore[ID_TIAN_MEN], m_lUserJettonScore[ID_XIA_MEN], m_lUserJettonScore[ID_JIAO_R]};	

	//区域倍率
	BYTE static const cbMultiple[] = {0, 1, 1, 1, 1, 1, 1};

	//胜利标识
	bool static bWinFlag[AREA_COUNT+1];
	bWinFlag[0]=false;
	bWinFlag[ID_SHANG_MEN]=bWinShangMen;
	bWinFlag[ID_JIAO_L]=(true==bWinShangMen && true==bWinTianMen) ? true : false;
	bWinFlag[ID_QIAO]=(true==bWinShangMen && true==bWinXiaMen) ? true : false;
	bWinFlag[ID_TIAN_MEN]=bWinTianMen;
	bWinFlag[ID_XIA_MEN]=bWinXiaMen;
	bWinFlag[ID_JIAO_R]=(true==bWinXiaMen && true==bWinTianMen) ? true : false;

	//角标识
	bool static bWinBankerJiaoL,bWinBankerJiaoR,bWinBankerQiao;
	bWinBankerJiaoL=(false==bWinShangMen && false==bWinTianMen) ? true : false;
	bWinBankerJiaoR=(false==bWinXiaMen && false==bWinTianMen) ? true : false;
	bWinBankerQiao=(false==bWinShangMen && false==bWinXiaMen) ? true : false;

	//计算积分
	for (WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
	{
		//庄家判断
		if (m_wCurrentBanker==wChairID) continue;

		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		for (WORD wAreaIndex = ID_SHANG_MEN; wAreaIndex <= ID_JIAO_R; ++wAreaIndex)
		{
			//角判断
			bool bReturnScore=false;
			if (ID_JIAO_L==wAreaIndex && false==bWinBankerJiaoL) bReturnScore=true;
			if (ID_JIAO_R==wAreaIndex && false==bWinBankerJiaoR) bReturnScore=true;
			if (ID_QIAO==wAreaIndex && false==bWinBankerQiao) bReturnScore=true;

			if (true==bWinFlag[wAreaIndex]) 
			{
				m_lUserWinScore[wChairID] += ( pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex] ) ;
				//m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
				lBankerWinScore -= ( pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex] ) ;
			}
			else if (true==bReturnScore)
			{
				m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
			}
			else
			{
				lUserLostScore[wChairID] -= pUserScore[wAreaIndex][wChairID];
				lBankerWinScore += pUserScore[wAreaIndex][wChairID];
			}
		}

		//计算税收
		if (0 < m_lUserWinScore[wChairID])
		{
			float fRevenuePer=float(cbRevenue/1000);
			m_lUserRevenue[wChairID]  = LONGLONG(m_lUserWinScore[wChairID]*fRevenuePer+0.5);
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}
	  //	总的分数
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
	}

	//庄家成绩
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;

		//计算税收
		if (0 < m_lUserWinScore[m_wCurrentBanker])
		{
			float fRevenuePer=float(cbRevenue/1000);
			if(cbRevenue<=0)
			{
				fRevenuePer = 0;
			}
			m_lUserRevenue[m_wCurrentBanker]  = LONGLONG(m_lUserWinScore[m_wCurrentBanker]*fRevenuePer+0.5);
			m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
		}		
	}

	if (!bProbeJudgeScore)
	{
		//累计积分
		m_lBankerWinScore += lBankerWinScore;

		//当前积分
		m_lBankerCurGameScore=lBankerWinScore;	
	}
	

	return lBankerWinScore;
}
void CTableFrameSink::ChangeMaxUser(bool blMin)
{
	int i ;
	BYTE m_cbTableCardArrayTemp[4][2];
	bool  bWinShangMen = 0;
	bool bWinTianMen = 0;
	bool bWinXiaMen = 0;
	CopyMemory(&m_cbTableCardArrayTemp[0][0],m_cbTableCardArray,sizeof(m_cbTableCardArray));
	for (i= 0;i<4;i++)
	{
		  bWinShangMen=m_GameLogic.CompareCard(m_cbTableCardArray[i],2,m_cbTableCardArray[(i+1)%4],2)==1?true:false;
		 bWinTianMen=m_GameLogic.CompareCard(m_cbTableCardArray[i],2,m_cbTableCardArray[(i+2)%4],2)==1?true:false;
		 bWinXiaMen=m_GameLogic.CompareCard(m_cbTableCardArray[i],2,m_cbTableCardArray[(i+3)%4],2)==1?true:false;
		if(bWinShangMen==bWinTianMen&&bWinXiaMen==bWinTianMen &&bWinTianMen==blMin)
		{
			CopyMemory(m_cbTableCardArray[0],m_cbTableCardArrayTemp[i],sizeof(m_cbTableCardArrayTemp[i]));
			CopyMemory(m_cbTableCardArray[1],m_cbTableCardArrayTemp[(i+1)%4],sizeof(m_cbTableCardArrayTemp[(i+1)%4]));
			CopyMemory(m_cbTableCardArray[2],m_cbTableCardArrayTemp[(i+2)%4],sizeof(m_cbTableCardArrayTemp[(i+2)%4]));
			CopyMemory(m_cbTableCardArray[3],m_cbTableCardArrayTemp[(i+3)%4],sizeof(m_cbTableCardArrayTemp[(i+3)%4]));
			break ;
		}
	}
	
}


//试探性判断
bool CTableFrameSink::ProbeJudge()
{
	
	//系统输赢
	LONGLONG lSystemScore = 0l;
	//庄家是不是机器人
	bool bIsBankerAndroidUser = false;
	IServerUserItem * pIBankerUserItem =(m_wCurrentBanker!=INVALID_CHAIR)?m_pITableFrame->GetTableUserItem(m_wCurrentBanker):NULL;
	if (pIBankerUserItem != NULL) bIsBankerAndroidUser = pIBankerUserItem->IsAndroidUser();

	//庄家金币
	LONGLONG lBankerScore = 0;
	if(m_wCurrentBanker != INVALID_CHAIR)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if(pIServerUserItem)
			lBankerScore = pIServerUserItem->GetUserScore();
	}

	//系统坐庄，或机器人坐庄，考虑库存
	if (m_wCurrentBanker == INVALID_CHAIR||bIsBankerAndroidUser)
	{
		lSystemScore = CalculateScore(true);
		m_nProbeTimes++;

		if (m_nProbeTimes>10)
		{
			//最大胜利分数
			BYTE bTempStorage=0;
			for(BYTE i=0;i<4;i++)
			{
				WinnerCardEx();
				if(lSystemScore > CalculateScore(true))
				{
					lSystemScore=CalculateScore(true);
					bTempStorage=m_StorageWin;
				}
				m_StorageWin++;
			}
			m_StorageWin=bTempStorage;
			WinnerCardEx();
			lSystemScore = CalculateScore(true);

			m_nProbeTimes = 0;
			m_lStorageStart += lSystemScore;
			

			if (m_lStorageStart>0)
			{
				m_lStorageStart = m_lStorageStart - (m_lStorageStart * m_nStorageDeduct / 1000);
			}
			else
			{
				m_lStorageStart = m_lStorageStart + (m_lStorageStart * m_nStorageDeduct / 1000);
			}

			return true;
		}
		//系统分值计算
		if ((lSystemScore + m_lStorageStart) < 0l)
		{
			return false;
		}
		else
		{
			m_lStorageStart += lSystemScore;
			
			if (m_lStorageStart>0)
			{
				m_lStorageStart = m_lStorageStart - (m_lStorageStart * m_nStorageDeduct / 1000);
			}
			else
			{
				m_lStorageStart = m_lStorageStart + (m_lStorageStart * m_nStorageDeduct / 1000);
			}
			return true;
		}


	}
	else
	{
		LONGLONG lBankerWinScore=0; 
		lBankerWinScore = CalculateScore(true);
		//庄家扣分计算
		if(lBankerWinScore < 0L && (lBankerScore+lBankerWinScore) < 0L) return false;

	}
	
	return true;
}

bool CTableFrameSink::OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer)
{

	//如果不具有管理员权限 则返回错误
	if( CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
	{
		return false;
	}


	return true;
}

void CTableFrameSink::DeduceWinner(bool &bWinShangMen, bool &bWinTianMen, bool &bWinXiaMen)
{
	//大小比较
	bWinShangMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHANG_MEN_INDEX],2)==1?true:false;
	bWinTianMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[TIAN_MEN_INDEX],2)==1?true:false;
	bWinXiaMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[XIA_MEN_INDEX],2)==1?true:false;
}

//推算牌型
void CTableFrameSink::WinnerCard()
{
	//BYTE bWinControl=m_pServerContro->GetWinControl();//星之光注释
	BYTE bWinControl=1;
	BYTE cbCardDate[8];
	BYTE bLastCard[8];
	ZeroMemory(bLastCard,sizeof(bLastCard));

	//继续发牌
	int const static nDispatchCardCount=8;
	bool bContiueCard=m_bContiueCard;
	if (m_cbLeftCardCount<nDispatchCardCount) bContiueCard=false;

	//继续发牌
	if (true==bContiueCard)
	{
		//重新洗牌
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];

		//设置扑克
		//CopyMemory(&m_cbTableCardArray[0][0],m_cbTableCard,sizeof(m_cbTableCardArray));
		CopyMemory(cbCardDate,m_cbTableCard,sizeof(cbCardDate));
		m_cbLeftCardCount -= nDispatchCardCount;
		if (m_cbLeftCardCount>0) 
			CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
	}
	else
	{
		//重新洗牌
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];
		for (int i = 0;i<2;i++)
		{
			m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));	
		}
		//设置扑克
		//CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));
		CopyMemory(cbCardDate,m_cbTableCard,sizeof(cbCardDate));

		m_cbLeftCardCount=CountArray(m_cbTableCard)-nDispatchCardCount;
		CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);

		for (int i = 0;i<4;i++)
		{ 
			memset(&m_cbTableHavaSendCardArray[i],0,2) ;
			m_cbTableHavaSendCount[i] = 0; 
		}
	}

	//推算第一牌型
	BYTE bTempCard[2];
	bTempCard[0]=cbCardDate[0];
	bTempCard[1]=cbCardDate[1];
	for(BYTE i=0;i<8;i++)
	{
		for(BYTE j=i+1;j<7;j++)
		{
			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard,bTempCard,sizeof(bTempCard));

	//删除第一牌型
	bool bDelete[2];
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//推算第二牌型
	ZeroMemory(bTempCard,sizeof(bTempCard));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		if(bTempCard[0] == 0)bTempCard[0]=cbCardDate[i];
		for(BYTE j=i+1;j<7;j++)
		{
			if(cbCardDate[j] == 0)continue;
			if(bTempCard[1] == 0)bTempCard[1]=cbCardDate[j];

			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard+2,bTempCard,sizeof(bTempCard));

	//删除第二牌型
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//推算第三牌型
	ZeroMemory(bTempCard,sizeof(bTempCard));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		if(bTempCard[0] == 0)bTempCard[0]=cbCardDate[i];
		for(BYTE j=i+1;j<7;j++)
		{
			if(cbCardDate[j] == 0)continue;
			if(bTempCard[1] == 0)bTempCard[1]=cbCardDate[j];

			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard+4,bTempCard,sizeof(bTempCard));

	//删除第三牌型
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//整理数组
	BYTE bCardNum=6;
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		bLastCard[bCardNum++]=cbCardDate[i];
	}

//#define BANKER_INDEX				0									//庄家索引
//#define SHANG_MEN_INDEX				1									//上门索引
//#define TIAN_MEN_INDEX				2									//天门索引
//#define XIA_MEN_INDEX				3									//下门索引
	//排列区域
	switch(bWinControl)
	{
	case 0:
		{
			//上门
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 1:
		{
			//上门和天门
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 2:
		{
			//天门
			CopyMemory(&m_cbTableCardArray[2], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);

			break;
		}
	case 3:
		{
			//上门和下门
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 4:
		{
			//下门
			CopyMemory(&m_cbTableCardArray[3], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 5:
		{
			//下门和天门
			CopyMemory(&m_cbTableCardArray[2], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 6:
		{
			//庄家
			CopyMemory(&m_cbTableCardArray[0], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	}

	//控制次数
	//m_pServerContro->SetExcuteTimes();//星之光注释

	//验证
	for(BYTE i=0;i<4;i++)
	{
		for(BYTE j=0;j<2;j++)
		{
			if(m_cbTableCardArray[i][j] == 0)
			{
				ASSERT(FALSE);
				return;
			}
		}
	}
}
//推算牌型
void CTableFrameSink::WinnerCardEx()
{
	BYTE bWinControl=m_StorageWin;
	BYTE cbCardDate[8];
	BYTE bLastCard[8];
	ZeroMemory(bLastCard,sizeof(bLastCard));

	//继续发牌
	int const static nDispatchCardCount=8;
	bool bContiueCard=m_bContiueCard;
	if (m_cbLeftCardCount<nDispatchCardCount) bContiueCard=false;

	//继续发牌
	if (true==bContiueCard)
	{
		//重新洗牌
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];

		//设置扑克
		//CopyMemory(&m_cbTableCardArray[0][0],m_cbTableCard,sizeof(m_cbTableCardArray));
		CopyMemory(cbCardDate,m_cbTableCard,sizeof(cbCardDate));
		m_cbLeftCardCount -= nDispatchCardCount;
		if (m_cbLeftCardCount>0) 
			CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
	}
	else
	{
		//重新洗牌
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];
		for (int i = 0;i<2;i++)
		{
			m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));	
		}
		//设置扑克
		//CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));
		CopyMemory(cbCardDate,m_cbTableCard,sizeof(cbCardDate));

		m_cbLeftCardCount=CountArray(m_cbTableCard)-nDispatchCardCount;
		CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);

		for (int i = 0;i<4;i++)
		{ 
			memset(&m_cbTableHavaSendCardArray[i],0,2) ;
			m_cbTableHavaSendCount[i] = 0; 
		}
	}

	//推算第一牌型
	BYTE bTempCard[2];
	bTempCard[0]=cbCardDate[0];
	bTempCard[1]=cbCardDate[1];
	for(BYTE i=0;i<8;i++)
	{
		for(BYTE j=i+1;j<7;j++)
		{
			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard,bTempCard,sizeof(bTempCard));

	//删除第一牌型
	bool bDelete[2];
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//推算第二牌型
	ZeroMemory(bTempCard,sizeof(bTempCard));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		if(bTempCard[0] == 0)bTempCard[0]=cbCardDate[i];
		for(BYTE j=i+1;j<7;j++)
		{
			if(cbCardDate[j] == 0)continue;
			if(bTempCard[1] == 0)bTempCard[1]=cbCardDate[j];

			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard+2,bTempCard,sizeof(bTempCard));

	//删除第二牌型
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//推算第三牌型
	ZeroMemory(bTempCard,sizeof(bTempCard));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		if(bTempCard[0] == 0)bTempCard[0]=cbCardDate[i];
		for(BYTE j=i+1;j<7;j++)
		{
			if(cbCardDate[j] == 0)continue;
			if(bTempCard[1] == 0)bTempCard[1]=cbCardDate[j];

			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard+4,bTempCard,sizeof(bTempCard));

	//删除第三牌型
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//整理数组
	BYTE bCardNum=6;
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		bLastCard[bCardNum++]=cbCardDate[i];
	}

	//#define BANKER_INDEX				0									//庄家索引
	//#define SHANG_MEN_INDEX				1									//上门索引
	//#define TIAN_MEN_INDEX				2									//天门索引
	//#define XIA_MEN_INDEX				3									//下门索引
	//排列区域
	switch(bWinControl)
	{
	case 0:
		{
			//上门
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 1:
		{
			//上门和天门
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 2:
		{
			//天门
			CopyMemory(&m_cbTableCardArray[2], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);

			break;
		}
	case 3:
		{
			//上门和下门
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 4:
		{
			//下门
			CopyMemory(&m_cbTableCardArray[3], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 5:
		{
			//下门和天门
			CopyMemory(&m_cbTableCardArray[2], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 6:
		{
			//庄家
			CopyMemory(&m_cbTableCardArray[0], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	}

	//控制次数
	//m_pServerContro->SetExcuteTimes();//星之光注释

	//验证
	for(BYTE i=0;i<4;i++)
	{
		for(BYTE j=0;j<2;j++)
		{
			if(m_cbTableCardArray[i][j] == 0)
			{
				ASSERT(FALSE);
				return;
			}
		}
	}
}
//发送记录
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{
	WORD wBufferSize=0;
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	int nIndex = m_nRecordFirst;
	while ( nIndex != m_nRecordLast )
	{
		if ((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
		{
			m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
			wBufferSize=0;
		}
		CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
		wBufferSize+=sizeof(tagServerGameRecord);

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
	}
	if (wBufferSize>0) m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
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

#ifdef __BANKER___
bool  CTableFrameSink::OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}
#endif


//读取配置
void CTableFrameSink::ReadConfigInformation(bool bReadFresh,bool bReadBaseConfig)
{
	//变量定义
	TCHAR OutBuf[255] = {};

	//每盘刷新
	if (bReadFresh)
	{
		//每盘刷新
		BYTE cbRefreshCfg = GetPrivateProfileInt(m_szRoomName, TEXT("Refresh"), 0, m_szConfigFileName);
		m_bRefreshCfg = cbRefreshCfg?true:false;
	}
	//上庄条件
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("score"), _T("1000"), OutBuf, 255, m_szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lApplyBankerCondition);

	//做庄次数
	m_nBankerTimeLimit = GetPrivateProfileInt(m_szRoomName, TEXT("Time"), 10, m_szConfigFileName);
	m_nBankerTimeAdd = GetPrivateProfileInt(m_szRoomName, TEXT("TimeAdd"), 10, m_szConfigFileName);

	//额外条件
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("BankerScore"), _T("9000000000"), OutBuf, 255, m_szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lExtraBankerScore);

	m_nExtraBankerTime = GetPrivateProfileInt(m_szRoomName, TEXT("BankerAdd"), 10, m_szConfigFileName);

	//下注限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("AreaLimitScore"), _T("2147483647"), OutBuf, 255, m_szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lAreaLimitScore);

	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("UserLimitScore"), _T("2147483647"), OutBuf, 255, m_szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lUserLimitScore);

	//抢庄扣除金币
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("CatchBankerReduceCoin"), _T("30000"), OutBuf, 255, m_szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_nBankerReduceCoin);
	if (m_nBankerReduceCoin<0) m_nBankerReduceCoin=30000;


	if(bReadBaseConfig)
	{
		
		BYTE cbEnableSysBanker=GetPrivateProfileInt(m_szRoomName, TEXT("EnableSysBanker"), 1, m_szConfigFileName);
		m_bEnableSysBanker=cbEnableSysBanker?true:false;
		
		//时间配置
		m_nFreeTime = GetPrivateProfileInt(m_szRoomName, TEXT("FreeTime"), 5, m_szConfigFileName);
		m_nPlaceJettonTime = GetPrivateProfileInt(m_szRoomName, TEXT("BetTime"), 15, m_szConfigFileName);
		m_nGameEndTime = GetPrivateProfileInt(m_szRoomName, TEXT("EndTime"), 15, m_szConfigFileName);
		if (m_nFreeTime <= 5		|| m_nFreeTime > 99)			m_nFreeTime = 5;
		if (m_nPlaceJettonTime <= 15 || m_nPlaceJettonTime > 99)		m_nPlaceJettonTime = 15;
		if (m_nGameEndTime <= 15		|| m_nGameEndTime > 99)			m_nGameEndTime = 20;

		//税收控制
		m_nStorageDeduct = GetPrivateProfileInt(m_szRoomName, TEXT("StorageDeduct"), 1, m_szConfigFileName);
		m_lStorageStart = GetPrivateProfileInt(m_szRoomName, TEXT("StorageStart"), 0, m_szConfigFileName);
		if (m_lStorageStart <= 0)			m_lStorageStart = 0;
		if (m_nStorageDeduct <= 0		|| m_nStorageDeduct > 50)			m_nStorageDeduct = 1;
	}

#ifdef _DEBUG
	m_nPlaceJettonTime = 15;
#endif

}

//查询服务费
bool  CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pUserItem==NULL) continue;
		if(wChairID==i)
		{
			//返回下注
			for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) 
			{

				if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
				{
					return true;
				}
			}
			break;
		}
	}
	return false;
}
//查询限额
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	SCORE consumeQuota=0L;
	SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	if(m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE)
	{
		consumeQuota=pIServerUserItem->GetUserScore()-lMinTableScore;

	}
	return consumeQuota;
}
//////////////////////////////////////////////////////////////////////////
