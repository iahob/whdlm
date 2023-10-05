#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////

#define IDI_OUT_CARD				1									//出牌定时器
#define IDI_OPERATE					2									//操作定时器
//构造函数
CTableFrameSink::CTableFrameSink()
{
	m_wPlayerCount = GAME_PLAYER;

	//组件变量
	m_pITableFrame = NULL;
	m_pGameCustomRule = NULL;
	m_pGameServiceOption = NULL;
	m_pGameServiceAttrib = NULL;	

	//游戏变量
	m_wSiceCount = 0;
	m_cbFanCardData=0;
	m_wBankerUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	memset(m_cbMagicIndex,MAX_INDEX,sizeof(m_cbMagicIndex));
	ZeroMemory(m_bTing, sizeof(m_bTing));
	ZeroMemory(m_bTrustee, sizeof(m_bTrustee));

	//堆立信息
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory(m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));
	ZeroMemory(m_cbHeapCount,sizeof(m_cbHeapCount));
	//运行变量
	m_cbProvideCard = 0;
	m_wResumeUser = INVALID_CHAIR;
	m_wProvideUser = INVALID_CHAIR;

	//用户状态
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray, sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard = 0;
	ZeroMemory(m_dwChiHuKind, sizeof(m_dwChiHuKind));

	//状态变量
	m_bGangOutCard = false;
	m_enSendStatus = Not_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;
	ZeroMemory(m_bEnjoinChiHu, sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng, sizeof(m_bEnjoinChiPeng));
	ZeroMemory(m_bEnjoinGang, sizeof(m_bEnjoinGang));
	ZeroMemory(m_bGangCard, sizeof(m_bGangCard));
	ZeroMemory(m_cbChiPengCount, sizeof(m_cbChiPengCount));	
	ZeroMemory(m_cbGangCount, sizeof(m_cbGangCount));

	//出牌信息
	m_cbOutCardData = 0;
	m_cbOutCardCount = 0;
	m_wOutCardUser = INVALID_CHAIR;

	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));

	//出牌信息
	m_cbMinusHeadCount = 0;
	m_cbMinusLastCount = 0;
	m_cbLeftCardCount = 0;
	m_cbEndLeftCount = 0;
	m_cbSendCardCount = 0;
	m_wLastCatchCardUser=INVALID_CHAIR;
	//扑克信息
	ZeroMemory(m_cbCardIndex, sizeof(m_cbCardIndex));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));

	ZeroMemory(m_lUserGangScore,sizeof(m_lUserGangScore));
	ZeroMemory(m_bPlayStatus,sizeof(m_bPlayStatus));

	ZeroMemory(m_cbHuCardCount,sizeof(m_cbHuCardCount));
	ZeroMemory(m_cbHuCardData,sizeof(m_cbHuCardData));
	ZeroMemory(m_bCanDiHu,sizeof(m_bCanDiHu));
	m_bGenZhuangSucceed=false;
	//房卡变量
	m_cbMaCount=0;
	m_cbPlayerCount=0;
	m_cbAllCardCount=0;
	m_bQiangGangHu=false;
	m_bHuQiDui=false;
	m_bNoMagicDouble=false;
	m_bHaveZiCard=false;
	m_cbMagicMode=0;

	ZeroMemory(&m_stRecord,sizeof(m_stRecord));

#ifdef  CARD_DISPATCHER_CONTROL
	m_cbControlGameCount = 0;
#endif

	//游戏视频
	m_hVideoInst = NULL;
	m_pGameVideo = NULL;
	m_hVideoInst = LoadLibrary(TEXT("SparrowGDEXGameVideo.dll"));
	if ( m_hVideoInst )
	{
		typedef void * (*CREATE)(); 
		CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst,"CreateGameVideo"); 
		if ( GameVideo )
		{
			m_pGameVideo = static_cast<IGameVideo*>(GameVideo());
		}
	}	
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
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
	m_wSiceCount = 0;
	m_cbFanCardData=0;
	m_wCurrentUser = INVALID_CHAIR;
	memset(m_cbMagicIndex,MAX_INDEX,sizeof(m_cbMagicIndex));
	m_cbMinusHeadCount = 0;
	m_cbMinusLastCount = 0;
	m_cbLeftCardCount = 0;
	m_cbEndLeftCount = 0;
	m_cbSendCardCount = 0;
	ZeroMemory(m_bTing, sizeof(m_bTing));
	ZeroMemory(m_bTrustee, sizeof(m_bTrustee));

	//堆立信息
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory(m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

	//运行变量
	m_cbProvideCard = 0;
	m_wResumeUser = INVALID_CHAIR;
	m_wProvideUser = INVALID_CHAIR;

	//状态变量
	m_bGangOutCard = false;
	m_enSendStatus = Not_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;
	ZeroMemory(m_bEnjoinChiHu, sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng, sizeof(m_bEnjoinChiPeng));	
	ZeroMemory(m_bEnjoinGang, sizeof(m_bEnjoinGang));
	ZeroMemory(m_cbChiPengCount, sizeof(m_cbChiPengCount));
	ZeroMemory(m_bGangCard, sizeof(m_bGangCard));	
	ZeroMemory(m_cbGangCount, sizeof(m_cbGangCount));

	for(int i = 0; i < GAME_PLAYER; i++)
	{
		m_vecEnjoinChiHu[i].clear();
		m_vecEnjoinChiPeng[i].clear();	
	}

	//用户状态
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray, sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(m_dwChiHuKind, sizeof(m_dwChiHuKind));
	
	//出牌信息
	m_cbOutCardData = 0;
	m_cbOutCardCount = 0;
	m_wOutCardUser = INVALID_CHAIR;

	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));

	//扑克信息
	ZeroMemory(m_cbCardIndex, sizeof(m_cbCardIndex));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory(m_lUserGangScore,sizeof(m_lUserGangScore));
	ZeroMemory(m_bPlayStatus,sizeof(m_bPlayStatus));

	m_wLastCatchCardUser=INVALID_CHAIR;

	ZeroMemory(m_cbHuCardCount,sizeof(m_cbHuCardCount));
	ZeroMemory(m_cbHuCardData,sizeof(m_cbHuCardData));
	ZeroMemory(m_bCanDiHu,sizeof(m_bCanDiHu));
	m_bGenZhuangSucceed=false;
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

	//游戏配置
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//自定规则
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	m_cbPlayerCount = GAME_PLAYER;
	//配置参数
	m_cbMaCount = m_pGameCustomRule->cbMaCount;
	m_cbMagicMode = m_pGameCustomRule->cbMagicMode;
	//默认玩法
	m_bQiangGangHu = m_pGameCustomRule->bQiangGangHu;
	m_bHuQiDui = m_pGameCustomRule->bHuQiDui;
	m_bNoMagicDouble = m_pGameCustomRule->bNoMagicDouble;
	m_bHaveZiCard = m_pGameCustomRule->bHaveZiCard;
	
	if(m_bHaveZiCard)
		m_cbAllCardCount=MAX_REPERTORY;
	else if(m_cbMagicMode == 1)//白板鬼
		m_cbAllCardCount=MAX_REPERTORY-4*6;
	else
		m_cbAllCardCount=MAX_REPERTORY-4*7;

	ZeroMemory(&m_stRecord,sizeof(m_stRecord));

	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	if(m_cbAllCardCount == MAX_REPERTORY || m_cbAllCardCount == (MAX_REPERTORY-4*6))
	{
		for(int i=0;i<GAME_PLAYER;i++)
			m_cbHeapCount[i] = m_cbAllCardCount/GAME_PLAYER;
	}
	else if(m_cbAllCardCount == (MAX_REPERTORY-4*7) )
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(i%2 == 0)
				m_cbHeapCount[i] = m_cbAllCardCount/GAME_PLAYER + 1;
			else
				m_cbHeapCount[i] = m_cbAllCardCount/GAME_PLAYER - 1;
		}
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
	//金币游戏
	if (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{		
		return 64 * m_pITableFrame->GetCellScore();
	}
	return 0;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	CString cc;
	cc.Format(_T("游戏开始 outtime=%d,operatetime=%d"), m_pGameCustomRule->cbTimeOutCard, m_pGameCustomRule->cbTimeOperateCard);
	WriteDebugInfo(cc);
	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

	//随机扑克
	BYTE Sice1 = rand()%6 + 1;
	BYTE Sice2 = rand()%6 + 1;
	BYTE minSice = min(Sice1,Sice2);
	m_wSiceCount = MAKEWORD(Sice1,Sice2);
	m_cbSendCardCount = 0;
	m_enSendStatus = Not_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	//取牌堆，参数不同，取到的牌不一样,gamelogic牌堆调整顺序
	m_GameLogic.RandCardList(m_cbRepertoryCard,m_cbAllCardCount);

	m_cbLeftCardCount = m_cbAllCardCount;

#ifdef  CARD_DISPATCHER_CONTROL

	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	CString strSaveFileName;
	strSaveFileName.Format(TEXT("%s\\SparrowGDEX.dat"), szPath);
	//非控制状态
	if(m_cbControlGameCount==0)
	{
		CFile file;
		if (file.Open(strSaveFileName, CFile::modeRead))
		{
			file.Read(&m_wBankerUser,sizeof(m_wBankerUser));
			file.Read(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));		
			file.Close();
		}		
	}
	else
	{
		m_wBankerUser = m_wControBankerUser;
		CopyMemory(m_cbRepertoryCard, m_cbControlRepertoryCard, sizeof(m_cbRepertoryCard));
		m_cbControlGameCount--;
	}

#endif

	//分发扑克
	for (WORD i = 0; i < m_cbPlayerCount; i++)
	{
		IServerUserItem* pUser = m_pITableFrame->GetTableUserItem(i);
		if(pUser == NULL)
			continue;

		m_bPlayStatus[i]=true;
		m_bCanDiHu[i]=true;
		m_cbLeftCardCount -= (MAX_COUNT - 1);
		m_cbMinusHeadCount += (MAX_COUNT - 1);
		m_cbHandCardCount[i] = (MAX_COUNT - 1);
		m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount], MAX_COUNT - 1, m_cbCardIndex[i]);
	}

	if(m_wBankerUser == INVALID_CHAIR && (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)!=0 )//房卡模式下先把庄家给房主
	{
		DWORD OwnerId =  m_pITableFrame->GetTableOwner();
		IServerUserItem *pOwnerItem = m_pITableFrame->SearchUserItem(OwnerId);
		if(pOwnerItem && pOwnerItem->GetChairID() != INVALID_CHAIR)
			m_wBankerUser = pOwnerItem->GetChairID();
	}

	//确定庄家
	if(m_wBankerUser == INVALID_CHAIR || m_bPlayStatus[m_wBankerUser]==false)
	{
		m_wBankerUser = rand() % m_cbPlayerCount;
		while(m_pITableFrame->GetTableUserItem(m_wBankerUser)==NULL)
		{
			m_wBankerUser = (m_wBankerUser+1)%m_cbPlayerCount;
		}
	}

	m_bCanDiHu[m_wBankerUser]=false;
	//发送扑克
	m_cbMinusHeadCount++;
	m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
	//设置变量
	m_cbProvideCard = m_cbSendCardData;
	m_wProvideUser = m_wBankerUser;
	m_wCurrentUser = m_wBankerUser;

	//堆立信息
	WORD wSiceCount = LOBYTE(m_wSiceCount) + HIBYTE(m_wSiceCount);
	WORD wTakeChairID = (m_wBankerUser + wSiceCount - 1) % GAME_PLAYER;
	BYTE cbTakeCount = m_cbPlayerCount*13+1+(m_cbMagicMode>=2?1:0);
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//计算数目
		BYTE cbValidCount = m_cbHeapCount[wTakeChairID] - m_cbHeapCardInfo[wTakeChairID][1] - ((i == 0) ? (minSice) * 2 : 0);
		BYTE cbRemoveCount = __min(cbValidCount, cbTakeCount);

		//提取扑克
		cbTakeCount -= cbRemoveCount;
		m_cbHeapCardInfo[wTakeChairID][(i == 0) ? 1 : 0] += cbRemoveCount;

		//完成判断
		if (cbTakeCount == 0)
		{
			m_wHeapHead = wTakeChairID;
			m_wHeapTail = (m_wBankerUser + wSiceCount - 1) % GAME_PLAYER;
			break;
		}
		//切换索引
		wTakeChairID = (wTakeChairID + GAME_PLAYER - 1) % GAME_PLAYER;
	}


	//翻财神
	if(m_cbMagicMode == 1)//白板鬼
		m_cbMagicIndex[0] = m_GameLogic.SwitchToCardIndex(0x37);
	else if(m_cbMagicMode >= 2)//翻鬼
	{
		m_cbFanCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
		m_cbMinusHeadCount++;
		BYTE cbColor = m_cbFanCardData&MASK_COLOR;
		BYTE cbValue = m_cbFanCardData&MASK_VALUE;
		BYTE cbMajicData1=0;
		BYTE cbMajicData2=0;
		if(cbColor == 0x30)
		{
			cbMajicData1 = cbColor+cbValue%7 + 1;
			cbMajicData2 = cbColor+(cbValue+1)%7 + 1;
		}
		else
		{
			cbMajicData1 = cbColor+cbValue%9 + 1;
			cbMajicData2 = cbColor+(cbValue+1)%9 + 1;
		}
		m_cbMagicIndex[0] = m_GameLogic.SwitchToCardIndex(cbMajicData1);

		if(m_cbMagicMode == 3)
			m_cbMagicIndex[1] = m_GameLogic.SwitchToCardIndex(cbMajicData2);
	}

	//设置鬼牌及玩法
	m_GameLogic.SetMagicIndex(m_cbMagicIndex);
	m_GameLogic.SetUserRule(m_bHuQiDui);

	//操作分析
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));

	//杠牌判断
	tagGangCardResult GangCardResult;
	m_cbUserAction[m_wBankerUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[m_wBankerUser],NULL,0,0,GangCardResult);

	//胡牌判断
	CChiHuRight chr;
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]--;
	m_cbUserAction[m_wBankerUser]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[m_wBankerUser],NULL,0,m_cbSendCardData,chr);
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
	m_cbHandCardCount[m_wBankerUser]++;

	//听牌判断
	BYTE cbRes = m_GameLogic.AnalyseTingCard(m_cbCardIndex[m_wBankerUser],0,0);
	if(cbRes == WIK_LISTEN)
	{
		if(G_CAN_TING_CARD)
			m_cbUserAction[m_wBankerUser] |= WIK_LISTEN;
		if(G_SHOW_TING_DATA)
			SendTingData(m_wBankerUser,m_cbCardIndex[m_wBankerUser],0,0);
	}
	if(m_pGameVideo)
	{
		m_pGameVideo->StartVideo(m_pITableFrame);
	}
	
	//构造变量
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	GameStart.wBankerUser = m_wBankerUser;
	GameStart.wSiceCount = m_wSiceCount;
	GameStart.cbFanCardData = m_cbFanCardData;
	GameStart.wHeapHead = m_wHeapHead;
	GameStart.wHeapTail = m_wHeapTail;
	CopyMemory(GameStart.cbMagicIndex, m_cbMagicIndex, sizeof(GameStart.cbMagicIndex));
	CopyMemory(GameStart.cbHeapCardInfo, m_cbHeapCardInfo, sizeof(GameStart.cbHeapCardInfo));

	bool bFirst = true;
	//发送数据
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i) ;
		if(pServerUserItem == NULL)
			continue;

		GameStart.cbUserAction = m_cbUserAction[i];
		ZeroMemory(GameStart.cbCardData, sizeof(GameStart.cbCardData));
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameStart.cbCardData);

		m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
		m_pITableFrame->SendLookonData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));		

		if (m_pGameVideo)
		{
			Video_GameStart video;
			lstrcpyn(video.szNickName,pServerUserItem->GetNickName(),CountArray(video.szNickName));
			video.lScore			= pServerUserItem->GetUserScore();
			video.lCellScore		= m_pITableFrame->GetCellScore();
			video.wChairID = i;
			video.wBankerUser = m_wBankerUser;
			video.wSiceCount = m_wSiceCount;
			CopyMemory(video.cbCardData,GameStart.cbCardData,sizeof(BYTE)*MAX_COUNT);
			CopyMemory(video.cbMagicIndex,m_cbMagicIndex,sizeof(BYTE)*MAX_MAGIC_COUNT);
			video.cbUserAction = m_cbUserAction[i];
			video.cbFanCardData = m_cbFanCardData;
			//规则
			video.cbAllCardCount = m_cbAllCardCount;
			video.cbPlayerCount = m_cbPlayerCount;
			video.cbMaCount = m_cbMaCount;
			video.cbMagicMode = m_cbMagicMode;
			video.bQiangGangHu = m_bQiangGangHu;
			video.bHuQiDui = m_bHuQiDui;
			video.bHaveZiCard = m_bHaveZiCard;
			video.bNoMagicDouble = m_bNoMagicDouble;
			m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video,sizeof(video), bFirst);
			bFirst = false;
		}
	}


	m_pITableFrame->SetGameTimer(IDI_OUT_CARD, (m_pGameCustomRule->cbTimeOutCard + TIME_DISPATCH_CARD) * 1000, 1, 0);
	m_tActionStartTime = time(0);
	cc.Format(_T("游戏开始1,庄家%d可动作%d"), m_wBankerUser, m_cbUserAction[m_wBankerUser]);
	WriteDebugInfo(cc);
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
			m_pITableFrame->KillGameTimer(IDI_OPERATE);
			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude, sizeof(GameConclude));
			GameConclude.cbSendCardData = m_cbSendCardData;
			GameConclude.wProvideUser = m_wProvideUser;
			GameConclude.cbProvideCard = m_cbProvideCard;
			GameConclude.wFleeUser = INVALID_CHAIR;
			memcpy(GameConclude.cbMagicIndex, m_cbMagicIndex, sizeof(m_cbMagicIndex));
			//结束信息
			for (WORD i = 0; i < m_cbPlayerCount; i++)
			{
				GameConclude.dwChiHuKind[i] = m_dwChiHuKind[i];
				if(m_dwChiHuKind[i] == WIK_CHI_HU)
				{
					FiltrateRight(i, m_ChiHuRight[i]);
					m_ChiHuRight[i].GetRightData(GameConclude.dwChiHuRight[i], MAX_RIGHT_COUNT);
				}
				GameConclude.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameConclude.cbHandCardData[i]);
				GameConclude.cbWeaveItemCount[i] = m_cbWeaveItemCount[i];
				memcpy(GameConclude.WeaveItemArray[i], m_WeaveItemArray[i], sizeof(GameConclude.WeaveItemArray[i]));
			}

			//计算输赢分
			CalGameScore(GameConclude);

			//积分变量
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

			//统计积分
			for (WORD i = 0; i < m_cbPlayerCount; i++)
			{
				if(!m_bPlayStatus[i])
					continue;

				//收税
				if (GameConclude.lGameScore[i]>0 && IsRoomCardScoreType()==false)
				{
					GameConclude.lRevenue[i] = m_pITableFrame->CalculateRevenue(i,GameConclude.lGameScore[i]);
					GameConclude.lGameScore[i] -= GameConclude.lRevenue[i];
				}

				ScoreInfoArray[i].lScore = GameConclude.lGameScore[i];
				ScoreInfoArray[i].lRevenue = GameConclude.lRevenue[i];
				ScoreInfoArray[i].cbType = ScoreInfoArray[i].lScore > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

				//历史积分
				m_HistoryScore.OnEventUserScore(i, GameConclude.lGameScore[i]);

				//朋友局记录
				if(m_stRecord.nCount<MAX_RECORD_COUNT)
				{
					m_stRecord.lDetailScore[i][m_stRecord.nCount]=GameConclude.lGameScore[i];
					m_stRecord.lAllScore[i] += GameConclude.lGameScore[i];
				}

				m_vecRecord[i].push_back(GameConclude.lGameScore[i]);
				if (m_vecRecord[i].size()>30)
					m_vecRecord[i].erase(m_vecRecord[i].begin());

			}

			if (m_stRecord.nCount<MAX_RECORD_COUNT)
				m_stRecord.nCount++;
			
			if (IsRoomCardScoreType())
			{
				CMD_S_RECORD RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));
				CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			}
			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude),true);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
			}
			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray, m_cbPlayerCount);

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
			m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
			
			return true;
		}
	case GER_NETWORK_ERROR:	//网络中断
	case GER_USER_LEAVE:	//用户强退
		{
			CString cc;
			cc.Format(_T("玩家强退:wchair=%d,cbreson=%d"), wChairID, cbReason);
			WriteDebugInfo(cc);
			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
			m_pITableFrame->KillGameTimer(IDI_OPERATE);
			WriteDebugInfo(_T("解散"));
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude, sizeof(GameConclude));
			GameConclude.wFleeUser=INVALID_CHAIR;

			m_wBankerUser = INVALID_CHAIR;

			GameConclude.cbSendCardData = m_cbSendCardData;
			memcpy(GameConclude.cbMagicIndex, m_cbMagicIndex, sizeof(m_cbMagicIndex));
			//用户扑克
			BYTE cbCardIndex=0;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				GameConclude.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameConclude.cbHandCardData[i]);
				GameConclude.cbWeaveItemCount[i] = m_cbWeaveItemCount[i];
				memcpy(GameConclude.WeaveItemArray[i], m_WeaveItemArray[i], sizeof(GameConclude.WeaveItemArray[i]));
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude),true);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
			}
			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
			m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//房卡模式
			{
				if(m_pITableFrame->IsPersonalRoomDisumme() )//当前朋友局解散清理记录
				{
					ZeroMemory(&m_stRecord,sizeof(m_stRecord));
				}
			}
			
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
	case GAME_SCENE_FREE:	//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));
			CopyMemory(StatusFree.cbHeapCount,m_cbHeapCount,sizeof(m_cbHeapCount));
			//设置变量
			StatusFree.lCellScore = m_pITableFrame->GetCellScore();
			StatusFree.cbMaCount = m_cbMaCount;
			StatusFree.cbPlayerCount = m_cbPlayerCount;
			StatusFree.cbMagicMode = m_cbMagicMode;
			StatusFree.bQiangGangHu = m_bQiangGangHu;
			StatusFree.bHuQiDui = m_bHuQiDui;
			StatusFree.bHaveZiCard = m_bHaveZiCard;
			StatusFree.bNoMagicDouble = m_bNoMagicDouble;
			StatusFree.cbAllCardCount = m_cbAllCardCount;
			StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			//自定规则
			StatusFree.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusFree.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
			StatusFree.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
			StatusFree.cbTimeWaitEnd = m_pGameCustomRule->cbTimeWaitEnd;
			//历史积分
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore = m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusFree.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			//发送场景
			m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));

			if (IsRoomCard())
			{
				CMD_S_RECORD RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));
				if (IsRoomCardScoreType())
				{
					if (m_stRecord.nCount > 0)
					{
						CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
						m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
					}

				}
				else if (m_vecRecord[wChairID].size()>0)
				{
					RoomCardRecord.cbAnGang[wChairID] = m_stRecord.cbAnGang[wChairID];
					RoomCardRecord.cbHuCount[wChairID] = m_stRecord.cbHuCount[wChairID];
					RoomCardRecord.cbMaCount[wChairID] = m_stRecord.cbMaCount[wChairID];
					RoomCardRecord.cbMingGang[wChairID] = m_stRecord.cbMingGang[wChairID];
					RoomCardRecord.nCount = m_vecRecord[wChairID].size();
					RoomCardRecord.lAllScore[wChairID] = 0;
					for (int i = 0; i < RoomCardRecord.nCount; i++)
					{
						RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
						RoomCardRecord.lAllScore[wChairID] += m_vecRecord[wChairID][i];
					}
					m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
				}
			}

			return true;
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			CString cc;
			cc.Format(_T("玩家%d断线重连"), wChairID);
			WriteDebugInfo(cc);
			//重入取消托管
			OnUserTrustee(wChairID,false);
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));
			CopyMemory(StatusPlay.cbHeapCount,m_cbHeapCount,sizeof(StatusPlay.cbHeapCount));
			//自定规则
			StatusPlay.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusPlay.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
			StatusPlay.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
			StatusPlay.cbTimeWaitEnd = m_pGameCustomRule->cbTimeWaitEnd;
			//规则
			StatusPlay.cbMaCount = m_cbMaCount;
			StatusPlay.cbPlayerCount = m_cbPlayerCount;
			StatusPlay.cbMagicMode = m_cbMagicMode;
			StatusPlay.bQiangGangHu = m_bQiangGangHu;
			StatusPlay.bHuQiDui = m_bHuQiDui;
			StatusPlay.bHaveZiCard = m_bHaveZiCard;
			StatusPlay.bNoMagicDouble = m_bNoMagicDouble;
			StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			//游戏变量
			StatusPlay.wBankerUser = m_wBankerUser;
			StatusPlay.wCurrentUser = m_wCurrentUser;
			StatusPlay.lCellScore = m_pITableFrame->GetCellScore();
			CopyMemory(StatusPlay.cbMagicIndex, m_cbMagicIndex, sizeof(m_cbMagicIndex));
			CopyMemory(StatusPlay.bTrustee, m_bTrustee, sizeof(m_bTrustee));

			//状态变量
			StatusPlay.cbActionCard = m_cbProvideCard;
			StatusPlay.cbAllCardCount = m_cbAllCardCount;
			StatusPlay.cbLeftCardCount = m_cbLeftCardCount;
			StatusPlay.cbActionMask = !m_bResponse[wChairID] ? m_cbUserAction[wChairID] : WIK_NULL;
			StatusPlay.wProvideUser = m_wProvideUser;
			CopyMemory(StatusPlay.bTrustee, m_bTrustee, sizeof(StatusPlay.bTrustee));
	
			//历史记录
			StatusPlay.wOutCardUser = m_wOutCardUser;
			StatusPlay.cbOutCardData = m_cbOutCardData;
			CopyMemory(StatusPlay.cbDiscardCard, m_cbDiscardCard, sizeof(StatusPlay.cbDiscardCard));
			CopyMemory(StatusPlay.cbDiscardCount, m_cbDiscardCount, sizeof(StatusPlay.cbDiscardCount));

			//组合扑克
			CopyMemory(StatusPlay.WeaveItemArray, m_WeaveItemArray, sizeof(StatusPlay.WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveItemCount, m_cbWeaveItemCount, sizeof(StatusPlay.cbWeaveItemCount));

			//堆立信息
			StatusPlay.wHeapHead = m_wHeapHead;
			StatusPlay.wHeapTail = m_wHeapTail;
			CopyMemory(StatusPlay.cbHeapCardInfo, m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

			//扑克数据
			for(int i = 0; i < GAME_PLAYER; i++)
			{
				StatusPlay.cbCardCount[i] = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
			}
			m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID], StatusPlay.cbCardData);
			StatusPlay.cbSendCardData = (m_wCurrentUser == wChairID) ? m_cbSendCardData : 0x00;

			//历史积分
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore = m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusPlay.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			time_t curTime = time(0);
			if (m_wCurrentUser != INVALID_CHAIR)
			{
				time_t tRes = m_tActionStartTime + m_pGameCustomRule->cbTimeOutCard - curTime;
				if (tRes > 0)
					StatusPlay.cbRemainingTime = (BYTE)tRes;
			}
			else if (m_wCurrentUser == INVALID_CHAIR && m_cbUserAction[wChairID] != WIK_NULL)
			{
				time_t tRes = m_tActionStartTime + m_pGameCustomRule->cbTimeOperateCard - curTime;
				if (tRes > 0)
					StatusPlay.cbRemainingTime = (BYTE)tRes;
			}

			//发送场景
			bool bRes = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));

			if(bRes)
			{
				if (IsRoomCard())
				{
					CMD_S_RECORD RoomCardRecord;
					ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));
					if (IsRoomCardScoreType() && m_stRecord.nCount>0)
					{
						CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
						m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
					}
					else if (m_vecRecord[wChairID].size() > 0)
					{
						RoomCardRecord.cbAnGang[wChairID] = m_stRecord.cbAnGang[wChairID];
						RoomCardRecord.cbHuCount[wChairID] = m_stRecord.cbHuCount[wChairID];
						RoomCardRecord.cbMaCount[wChairID] = m_stRecord.cbMaCount[wChairID];
						RoomCardRecord.cbMingGang[wChairID] = m_stRecord.cbMingGang[wChairID];
						RoomCardRecord.nCount = m_vecRecord[wChairID].size();
						RoomCardRecord.lAllScore[wChairID] = 0;
						for (int i = 0; i < RoomCardRecord.nCount; i++)
						{
							RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
							RoomCardRecord.lAllScore[wChairID] += m_vecRecord[wChairID][i];
						}
						m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
					}
				}

				if(G_SHOW_TING_DATA)
					SendTingData(wChairID,m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID]);
			}

			return bRes;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//时间事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	if(wTimerID == IDI_OUT_CARD)
	{
		m_pITableFrame->KillGameTimer(wTimerID);
		CString cc;
		cc.Format(_T("出牌倒计时结束，当前玩家%d"), m_wCurrentUser);
		WriteDebugInfo(cc);
		return OnUserTrustee(m_wCurrentUser, true);
	}
	else if (wTimerID == IDI_OPERATE)
	{
		m_pITableFrame->KillGameTimer(wTimerID);

		for (WORD i = 0; i < m_cbPlayerCount; i++)
		{
			if (m_cbUserAction[i] != WIK_NULL && m_bResponse[i] == false)
			{
				CString cc;
				cc.Format(_T("操作倒计时结束，玩家%d"), i);
				WriteDebugInfo(cc);
				OnUserTrustee(i, true);
			}
		}

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
	m_cbWaitTime = 0;
	switch (wSubCmdID)
	{
	case SUB_C_OUT_CARD:
		{
			//效验消息
			ASSERT(wDataSize == sizeof(CMD_C_OutCard));
			if (wDataSize != sizeof(CMD_C_OutCard)) return false;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//消息处理
			CMD_C_OutCard * pOutCard = (CMD_C_OutCard *)pData;
			return OnUserOutCard(pIServerUserItem->GetChairID(), pOutCard->cbCardData);
		}
	case SUB_C_OPERATE_CARD:	//操作消息
		{
			//效验消息
			ASSERT(wDataSize == sizeof(CMD_C_OperateCard));
			if (wDataSize != sizeof(CMD_C_OperateCard)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//消息处理
			CMD_C_OperateCard * pOperateCard = (CMD_C_OperateCard *)pData;
			return OnUserOperateCard(pIServerUserItem->GetChairID(), pOperateCard->cbOperateCode, pOperateCard->cbOperateCard);
		}
	case SUB_C_LISTEN_CARD:
		{
			//效验消息
			ASSERT(wDataSize == sizeof(CMD_C_ListenCard));
			if (wDataSize != sizeof(CMD_C_ListenCard)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//消息处理
			CMD_C_ListenCard * pOperateCard = (CMD_C_ListenCard *)pData;
			return OnUserListenCard(pIServerUserItem->GetChairID(), pOperateCard->bListenCard);
		}
	case SUB_C_TRUSTEE:
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pData;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;

			if (IsRoomCardScoreType()) return true;
			return OnUserTrustee(pIServerUserItem->GetChairID(), pTrustee->bTrustee);
		}
	case SUB_C_REQUEST_RCRecord:
	{
		ASSERT(IsRoomCard());
		if (IsRoomCard() == false)
		{
			return true;
		}
		WORD wChairID = pIServerUserItem->GetChairID();
		CMD_S_RECORD RoomCardRecord;
		ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));
		if (IsRoomCardScoreType())
			CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
		else
		{
			RoomCardRecord.cbAnGang[wChairID] = m_stRecord.cbAnGang[wChairID];
			RoomCardRecord.cbHuCount[wChairID] = m_stRecord.cbHuCount[wChairID];
			RoomCardRecord.cbMaCount[wChairID] = m_stRecord.cbMaCount[wChairID];
			RoomCardRecord.cbMingGang[wChairID] = m_stRecord.cbMingGang[wChairID];
			RoomCardRecord.nCount = m_vecRecord[wChairID].size();
			RoomCardRecord.lAllScore[wChairID] = 0;
			for (int i = 0; i < RoomCardRecord.nCount; i++)
			{
				RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
				RoomCardRecord.lAllScore[wChairID] += m_vecRecord[wChairID][i];
			}
		}
		m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));

		return true;
	}
	default:
		break;
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
	CString cc;
	cc.Format(_T("玩家进入:chair=%d,userid=%d,name=%s"), wChairID, pIServerUserItem->GetUserID(), pIServerUserItem->GetNickName());
	WriteDebugInfo(cc);
	if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//房卡模式
	{
		//获取房卡规则
		BYTE* pSetInfo = m_pITableFrame->GetGameRule();
		if(pSetInfo[0] == 1)
		{
			BYTE cbChairCount=pSetInfo[1];
			BYTE cbMaxChairCount=pSetInfo[2];
			m_cbMaCount = pSetInfo[3];
			m_cbMagicMode = pSetInfo[4];
			m_bHaveZiCard = pSetInfo[5];
			m_bNoMagicDouble = pSetInfo[6];
			m_bQiangGangHu = pSetInfo[7];
			m_bHuQiDui = pSetInfo[8];
		}

		if(m_bHaveZiCard)
			m_cbAllCardCount=MAX_REPERTORY;
		else if(m_cbMagicMode == 1)//白板鬼
			m_cbAllCardCount=MAX_REPERTORY-4*6;
		else
			m_cbAllCardCount=MAX_REPERTORY-4*7;

		if(m_cbAllCardCount == MAX_REPERTORY || m_cbAllCardCount == (MAX_REPERTORY-4*6))
		{
			for(int i=0;i<GAME_PLAYER;i++)
				m_cbHeapCount[i] = m_cbAllCardCount/GAME_PLAYER;
		}
		else if(m_cbAllCardCount == (MAX_REPERTORY-4*7) )
		{
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(i%2 == 0)
					m_cbHeapCount[i] = m_cbAllCardCount/GAME_PLAYER + 1;
				else
					m_cbHeapCount[i] = m_cbAllCardCount/GAME_PLAYER - 1;
			}
		}

		if (m_pITableFrame->GetDataBaseMode() == 1)
		{
			m_vecRecord[wChairID].clear();
			m_stRecord.lAllScore[wChairID] = 0;
			m_stRecord.cbAnGang[wChairID] = 0;
			m_stRecord.cbHuCount[wChairID] = 0;
			m_stRecord.cbMaCount[wChairID] = 0;
			m_stRecord.cbMingGang[wChairID] = 0;
		}

	}

	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserEnter(wChairID);
		m_wBankerUser = INVALID_CHAIR;
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
		m_wBankerUser = INVALID_CHAIR;
	}

	if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//房卡模式
	{
		if(m_pITableFrame->IsPersonalRoomDisumme() )//当前朋友局解散清理记录
		{
			ZeroMemory(&m_stRecord,sizeof(m_stRecord));
		}
		if (m_pITableFrame->GetDataBaseMode() == 1)
		{
			m_vecRecord[wChairID].clear();
			m_stRecord.lAllScore[wChairID] = 0;
			m_stRecord.cbAnGang[wChairID] = 0;
			m_stRecord.cbHuCount[wChairID] = 0;
			m_stRecord.cbMaCount[wChairID] = 0;
			m_stRecord.cbMingGang[wChairID] = 0;
		}

	}
	CString cc;
	cc.Format(_T("玩家离开:chair=%d,userid=%d,name=%s"), wChairID, pIServerUserItem->GetUserID(), pIServerUserItem->GetNickName());
	WriteDebugInfo(cc);
	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData,bool bSysOut/*=false*/)
{
	CString ss;
	ss.Format(_T("玩家%d出牌%0x,curuser=%d,sysout%d,trustee%d"), wChairID, cbCardData, m_wCurrentUser, bSysOut ? 1 : 0, m_bTrustee[wChairID] ? 1 : 0);
	WriteDebugInfo(ss);
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY);
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY)
	{
		WriteDebugInfo(_T("出牌error1"));
		return true;
	}
	//错误断言
	ASSERT(wChairID == m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData));

	//效验参数
	if (wChairID != m_wCurrentUser)
	{
		WriteDebugInfo(_T("出牌error2"));
		return false;
	}
	if (!m_GameLogic.IsValidCard(cbCardData))
	{
		WriteDebugInfo(_T("出牌error3"));
		return false;
	}
	//删除扑克
 	if (!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbCardData))
 	{
		WriteDebugInfo(_T("出牌error4"));
 		ASSERT(FALSE);
 		return false;
 	}

	//删除定时器
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);

	//清除禁止
	m_bEnjoinChiHu[wChairID] = false;
	m_bEnjoinChiPeng[wChairID] = false;
	m_vecEnjoinChiPeng[wChairID].clear();
	m_vecEnjoinChiHu[wChairID].clear();
	m_bCanDiHu[wChairID]=false;
	//设置变量
	m_enSendStatus = OutCard_Send;
	m_cbSendCardData = 0;
	m_cbUserAction[wChairID] = WIK_NULL;
	m_cbPerformAction[wChairID] = WIK_NULL;
	//出牌记录
	m_wOutCardUser = wChairID;
	m_cbOutCardData = cbCardData;
	
	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser = wChairID;
	OutCard.cbOutCardData = cbCardData;
	OutCard.bSysOut=bSysOut;
	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OUT_CARD, &OutCard, sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OUT_CARD, &OutCard, sizeof(OutCard));
	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_OUT_CARD,&OutCard,sizeof(OutCard),true);
	}
	//跟庄判断
	if(G_GEN_ZHUANG && m_bGenZhuangSucceed==false && m_cbDiscardCount[wChairID]==0 && m_wBankerUser==((wChairID+1)%GAME_PLAYER))
	{
		bool bFlag=true;
		for(int i=m_wBankerUser;i != wChairID;i=(i+1)%GAME_PLAYER )
		{
			if(m_cbDiscardCount[i] != 1 || m_cbDiscardCard[i][0] != cbCardData)
			{
				bFlag=false;
				break;
			}
		}
		if(bFlag)
			m_bGenZhuangSucceed=true;
	}

	m_wProvideUser = wChairID;
	m_cbProvideCard = cbCardData;

	//用户切换
	m_wCurrentUser = (wChairID + 1) % m_cbPlayerCount;
	for(int i=0;i<m_cbPlayerCount;i++)
	{
		if(m_bPlayStatus[m_wCurrentUser])
			break;
		m_wCurrentUser = (m_wCurrentUser+1)%m_cbPlayerCount;
	}

	//响应判断
 	bool bAroseAction = EstimateUserRespond(wChairID, cbCardData, EstimatKind_OutCard);
 
	if(m_cbGangStatus != WIK_GANERAL)
	{
		m_bGangOutCard = true;
		m_cbGangStatus = WIK_GANERAL;
		m_wProvideGangUser = INVALID_CHAIR;
	}	
	else
	{
		m_bGangOutCard = false;
	}

 	//派发扑克
 	if (!bAroseAction) 
	{
		DispatchCardData(m_wCurrentUser);
	}

	return true;
}

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard[3])
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY);
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY) return true;

	//效验用户 注意：AI有可能发生此断言
	ASSERT((wChairID == m_wCurrentUser) || (m_wCurrentUser == INVALID_CHAIR));
	if ((wChairID != m_wCurrentUser) && (m_wCurrentUser != INVALID_CHAIR))  return true;

 	//被动动作
 	if (m_wCurrentUser == INVALID_CHAIR)
 	{
 		//效验状态
 		ASSERT(!m_bResponse[wChairID]);
 		ASSERT(m_cbUserAction[wChairID] != WIK_NULL);
 		ASSERT((cbOperateCode == WIK_NULL) || ((m_cbUserAction[wChairID]&cbOperateCode) != 0));
 
 		//效验状态
 		if (m_bResponse[wChairID]) return true;
 		if(m_cbUserAction[wChairID] == WIK_NULL) return true;
 		if ((cbOperateCode!=WIK_NULL) && ((m_cbUserAction[wChairID]&cbOperateCode) == 0)) return true;
		CString cc;
		cc.Format(_T("被动动作 玩家%d执行操作%d"), wChairID, cbOperateCode);
		WriteDebugInfo(cc);
 		//变量定义
 		WORD wTargetUser = wChairID;
 		BYTE cbTargetAction = cbOperateCode;
 
 		//设置变量
		m_bEnjoinGang[wTargetUser] = false;
 		m_bResponse[wTargetUser] = true;
 		m_cbPerformAction[wTargetUser] = cbOperateCode;
 		if(cbOperateCard[0] == 0)
		{
			m_cbOperateCard[wTargetUser][0] = m_cbProvideCard;
		}
 		else 
		{
			CopyMemory(m_cbOperateCard[wTargetUser], cbOperateCard, sizeof(m_cbOperateCard[wTargetUser]));
		}
 
		//放弃操作
		if (cbTargetAction == WIK_NULL)
		{
			////禁止这轮吃胡
			if((m_cbUserAction[wTargetUser] & WIK_CHI_HU) != 0)
			{
				m_bEnjoinChiHu[wTargetUser]=true;
			}
		}

 		//执行判断
 		for (WORD i = 0; i < m_cbPlayerCount; i++)
 		{
 			//获取动作
 			BYTE cbUserAction = (!m_bResponse[i]) ? m_cbUserAction[i] : m_cbPerformAction[i];
 
 			//优先级别
 			BYTE cbUserActionRank = m_GameLogic.GetUserActionRank(cbUserAction);
 			BYTE cbTargetActionRank = m_GameLogic.GetUserActionRank(cbTargetAction);
 
 			//动作判断
 			if (cbUserActionRank > cbTargetActionRank)
 			{
 				wTargetUser = i;
 				cbTargetAction = cbUserAction;
 			}
 		}
 		if (!m_bResponse[wTargetUser]) 
 			return true;
 

		if(cbTargetAction == WIK_CHI_HU)
		{

			if(G_ONLY_ONE_HU)//只能一个人胡，逆时针近者胡
			{
				BYTE offset1 = (wChairID-m_wResumeUser+GAME_PLAYER)%GAME_PLAYER;
 				for (WORD i = 0; i < GAME_PLAYER; i++)
 				{
 					if (!m_bResponse[i] && (m_cbUserAction[i]&WIK_CHI_HU))
					{
						BYTE offset2=(i-m_wResumeUser+GAME_PLAYER)%GAME_PLAYER;
						if(offset2<offset1)
 							return true;
					}
 				}
			}
			else//胡牌等待
			{
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					if (!m_bResponse[i] && (m_cbUserAction[i]&WIK_CHI_HU))
						return true;
				}
			}

		}

		//关闭定时器
		m_pITableFrame->KillGameTimer(IDI_OPERATE);

 		//放弃操作
 		if (cbTargetAction == WIK_NULL)
 		{
			//用户状态
 			ZeroMemory(m_bResponse, sizeof(m_bResponse));
 			ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
 			ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
 			ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));
 
			DispatchCardData(m_wResumeUser,m_cbGangStatus != WIK_GANERAL);
 			return true;
 		}
 
 		//变量定义
 		BYTE cbTargetCard = m_cbOperateCard[wTargetUser][0];
 
 		//出牌变量
 		m_enSendStatus = Gang_Send;
		m_cbSendCardData = 0;
 		m_wOutCardUser = INVALID_CHAIR;
		m_cbOutCardData = 0;
 
 		//胡牌操作
 		if (cbTargetAction == WIK_CHI_HU)
 		{

#if G_ONLY_ONE_HU//只能一人胡 清楚其他玩家胡牌动作
			for(int i=0;i<m_cbPlayerCount;i++)
			{
				if((m_cbPerformAction[i]&WIK_CHI_HU)!=0 && i != wTargetUser)
					m_cbPerformAction[i]=WIK_NULL;
			}
#endif

 			//结束信息
 			m_cbChiHuCard = cbTargetCard;
 
			WORD wChiHuUser = m_wBankerUser;
			for(int i = 0; i < m_cbPlayerCount; i++)
 			{
 				wChiHuUser = (m_wBankerUser + i) % m_cbPlayerCount;
				//过虑判断
 				if ((m_cbPerformAction[wChiHuUser]&WIK_CHI_HU) == 0)
 					continue;
 
 				//胡牌判断
 				BYTE cbWeaveItemCount = m_cbWeaveItemCount[wChiHuUser];
 				tagWeaveItem * pWeaveItem = m_WeaveItemArray[wChiHuUser];
 				m_dwChiHuKind[wChiHuUser] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChiHuUser], pWeaveItem, cbWeaveItemCount, m_cbChiHuCard, m_ChiHuRight[wChiHuUser]);

 				//插入扑克
 				if (m_dwChiHuKind[wChiHuUser] != WIK_NULL) 
 				{
 					wTargetUser = wChiHuUser;
 				}				
 			}
 
 			//结束游戏
 			ASSERT(m_dwChiHuKind[wTargetUser] != WIK_NULL);
 			OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
 
 			return true;
 		}
 
 		//组合扑克
 		ASSERT(m_cbWeaveItemCount[wTargetUser] < MAX_WEAVE);
 		WORD wIndex = m_cbWeaveItemCount[wTargetUser]++;
 		m_WeaveItemArray[wTargetUser][wIndex].cbParam = WIK_GANERAL;
 		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard = cbTargetCard;
 		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind = cbTargetAction;
 		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;
 		m_WeaveItemArray[wTargetUser][wIndex].cbCardData[0] = cbTargetCard;
 		if(cbTargetAction&(WIK_LEFT | WIK_CENTER | WIK_RIGHT))
 		{
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[1] = m_cbOperateCard[wTargetUser][1];
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[2] = m_cbOperateCard[wTargetUser][2];
 		}
 		else
 		{
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[1] = cbTargetCard;
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[2] = cbTargetCard;
 			if(cbTargetAction & WIK_GANG)
			{
				m_WeaveItemArray[wTargetUser][wIndex].cbParam = WIK_FANG_GANG;
				m_WeaveItemArray[wTargetUser][wIndex].cbCardData[3] = cbTargetCard;
			}
 		}
 
 		//删除扑克
 		switch (cbTargetAction)
 		{
 		case WIK_LEFT:		//上牌操作
 			{
 				//删除扑克
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], &m_cbOperateCard[wTargetUser][1], 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;
 
 				break;
 			}
 		case WIK_RIGHT:		//上牌操作
 			{
 				//删除扑克
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], &m_cbOperateCard[wTargetUser][1], 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;
 
 				break;
 			}
 		case WIK_CENTER:	//上牌操作
 			{
 				//删除扑克
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], &m_cbOperateCard[wTargetUser][1], 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;
 
 				break;
 			}
 		case WIK_PENG:		//碰牌操作
 			{
 				//删除扑克
 				BYTE cbRemoveCard[] = {cbTargetCard,cbTargetCard};
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], cbRemoveCard, 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;

				BYTE cbOffSet = (wTargetUser-m_wBankerUser+GAME_PLAYER)%GAME_PLAYER;
				for(int i=1;i<=cbOffSet;i++)
				{
					m_bCanDiHu[(m_wBankerUser+i)%GAME_PLAYER]=false;
				}
 				break;
 			}
 		case WIK_GANG:		//杠牌操作
 			{
 				//删除扑克,被动动作只存在放杠
 				BYTE cbRemoveCard[] = {cbTargetCard, cbTargetCard, cbTargetCard};
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], cbRemoveCard, CountArray(cbRemoveCard)))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				
				BYTE cbOffSet = (wTargetUser-m_wBankerUser+GAME_PLAYER)%GAME_PLAYER;
				for(int i=1;i<=cbOffSet;i++)
				{
					m_bCanDiHu[(m_wBankerUser+i)%GAME_PLAYER]=false;
				}
 				break;
 			}
 		default:
 			ASSERT(FALSE);
 			return false;
 		}
 
 		//构造结果
 		CMD_S_OperateResult OperateResult;
 		ZeroMemory(&OperateResult, sizeof(OperateResult));
 		OperateResult.wOperateUser = wTargetUser;
 		OperateResult.cbOperateCode = cbTargetAction;
 		OperateResult.wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;
 		OperateResult.cbOperateCard[0] = cbTargetCard;
 		if(cbTargetAction & (WIK_LEFT | WIK_CENTER | WIK_RIGHT))
		{
			CopyMemory(&OperateResult.cbOperateCard[1], &m_cbOperateCard[wTargetUser][1], 2 * sizeof(BYTE));
		}
 		else if(cbTargetAction&WIK_PENG)
 		{
 			OperateResult.cbOperateCard[1] = cbTargetCard;
 			OperateResult.cbOperateCard[2] = cbTargetCard;
 		}
 
 		//用户状态
 		ZeroMemory(m_bResponse, sizeof(m_bResponse));
 		ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
 		ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
 		ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));
 
 		//如果非杠牌
 		if(cbTargetAction != WIK_GANG)
 		{
			m_wProvideUser = INVALID_CHAIR;
			m_cbProvideCard = 0;

 			tagGangCardResult gcr;
 			m_cbUserAction[wTargetUser] |= m_GameLogic.AnalyseGangCard(m_cbCardIndex[wTargetUser], m_WeaveItemArray[wTargetUser], m_cbWeaveItemCount[wTargetUser],0, gcr);

			if(m_bTing[wTargetUser] == false)
			{
				BYTE cbRes = m_GameLogic.AnalyseTingCard(m_cbCardIndex[wTargetUser],m_WeaveItemArray[wTargetUser],m_cbWeaveItemCount[wTargetUser]);
				if(cbRes == WIK_LISTEN)
				{
					if(G_CAN_TING_CARD)
						m_cbUserAction[wTargetUser] |= WIK_LISTEN; 

					if(G_SHOW_TING_DATA)
						SendTingData(wTargetUser,m_cbCardIndex[wTargetUser],m_WeaveItemArray[wTargetUser],m_cbWeaveItemCount[wTargetUser]);
				}
			}

 			OperateResult.cbActionMask |= m_cbUserAction[wTargetUser];
 		}
 
 		//发送消息
 		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 		if(m_pGameVideo)
		{
			m_pGameVideo->AddVideoData(SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult),true);
		}
 		//设置用户
 		m_wCurrentUser = wTargetUser;
 
 		//杠牌处理
 		if (cbTargetAction == WIK_GANG)
 		{
 			m_cbGangStatus = WIK_FANG_GANG;
			m_wProvideGangUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;
			m_bGangCard[wTargetUser] = true;
			m_cbGangCount[wTargetUser]++;
 			DispatchCardData(wTargetUser, true);
 		}
		else
		{
			if (m_bTrustee[m_wCurrentUser])
				m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeTrustee*1000, 1, 0);
			else
				m_pITableFrame->SetGameTimer(IDI_OUT_CARD, (m_pGameCustomRule->cbTimeOutCard + 2) * 1000, 1, 0);

			m_tActionStartTime = time(0);
		}
 
 		return true;
 	}
 
 	//主动动作
 	if (m_wCurrentUser == wChairID)
 	{
 		//扑克效验
 		ASSERT((cbOperateCode == WIK_NULL) || (cbOperateCode == WIK_CHI_HU) || (m_GameLogic.IsValidCard(cbOperateCard[0])));
 		if ((cbOperateCode != WIK_NULL) && (cbOperateCode != WIK_CHI_HU) && (!m_GameLogic.IsValidCard(cbOperateCard[0]))) return false;
 
		if (cbOperateCode == WIK_NULL)
		{
			CString cc;
			cc.Format(_T("主动动作%d玩家%d放弃"), m_cbUserAction[m_wCurrentUser], wChairID);
			WriteDebugInfo(cc);
		}
 		//设置变量
 		m_cbUserAction[m_wCurrentUser] = WIK_NULL;
 		m_cbPerformAction[m_wCurrentUser] = WIK_NULL;
 
 		//执行动作
 		switch (cbOperateCode)
 		{
 		case WIK_GANG:			//杠牌操作
 			{
				m_enSendStatus = Gang_Send;

 				//变量定义
 				BYTE cbWeaveIndex = 0xFF;
 				BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(cbOperateCard[0]);
				WORD wProvideUser = wChairID;
				BYTE cbGangKind = WIK_MING_GANG;
 				//杠牌处理
 				if (m_cbCardIndex[wChairID][cbCardIndex] == 1)
 				{
 					//寻找组合
 					for (BYTE i = 0; i < m_cbWeaveItemCount[wChairID]; i++)
 					{
 						BYTE cbWeaveKind = m_WeaveItemArray[wChairID][i].cbWeaveKind;
 						BYTE cbCenterCard = m_WeaveItemArray[wChairID][i].cbCenterCard;
 						if ((cbCenterCard == cbOperateCard[0]) && (cbWeaveKind == WIK_PENG))
 						{
 							cbWeaveIndex = i;
 							break;
 						}
 					}
 
 					//效验动作
 					ASSERT(cbWeaveIndex != 0xFF);
 					if (cbWeaveIndex == 0xFF) return false;
 
					cbGangKind = WIK_MING_GANG;

 					//组合扑克
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbParam = WIK_MING_GANG;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind = cbOperateCode;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard = cbOperateCard[0];
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCardData[3] = cbOperateCard[0];
 
 					//杠牌得分
 					wProvideUser = m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser;
 				}
 				else
 				{
 					//扑克效验
 					ASSERT(m_cbCardIndex[wChairID][cbCardIndex] == 4);
 					if (m_cbCardIndex[wChairID][cbCardIndex] != 4) 
 						return false;
 
 					//设置变量
 					cbGangKind = WIK_AN_GANG;
 					cbWeaveIndex = m_cbWeaveItemCount[wChairID]++;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbParam = WIK_AN_GANG;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser = wChairID;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind = cbOperateCode;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard = cbOperateCard[0];
 					for(BYTE j = 0; j < 4; j++) 
					{
						m_WeaveItemArray[wChairID][cbWeaveIndex].cbCardData[j] = cbOperateCard[0];
					}
 				}
 
 				//删除扑克
 				m_cbCardIndex[wChairID][cbCardIndex] = 0;
				m_cbGangStatus = cbGangKind;
				m_wProvideGangUser = wProvideUser;
				m_bGangCard[wChairID] = true;
				m_cbGangCount[wChairID]++;
 
 				//构造结果
 				CMD_S_OperateResult OperateResult;
 				ZeroMemory(&OperateResult, sizeof(OperateResult));
 				OperateResult.wOperateUser = wChairID;
 				OperateResult.wProvideUser = wProvideUser;
 				OperateResult.cbOperateCode = cbOperateCode;
 				OperateResult.cbOperateCard[0] = cbOperateCard[0];
 
 				//发送消息
 				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
				if(m_pGameVideo)
				{
					m_pGameVideo->AddVideoData(SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult),true);
				}
 				//效验动作
 				bool bAroseAction = false;

 				if(cbGangKind == WIK_MING_GANG) 
				{
					bAroseAction=EstimateUserRespond(wChairID, cbOperateCard[0], EstimatKind_GangCard);
				}

 				//发送扑克
 				if (!bAroseAction)
 				{
 					DispatchCardData(wChairID, true);
 				}
 
 				return true;
 			}
 		case WIK_CHI_HU:		//自摸
 			{
 				//普通胡牌
 				BYTE cbWeaveItemCount = m_cbWeaveItemCount[wChairID];
 				tagWeaveItem * pWeaveItem = m_WeaveItemArray[wChairID];
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], &m_cbSendCardData, 1))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
 				m_dwChiHuKind[wChairID] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID], pWeaveItem, cbWeaveItemCount, m_cbSendCardData, m_ChiHuRight[wChairID]);
 
 				//结束信息
				m_cbChiHuCard = m_cbSendCardData;
				m_cbProvideCard = m_cbSendCardData;
				
 				//结束游戏
 				OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
 
 				return true;
 			}
 		}
 
 		return true;
 	}

	return false;
}


//用户听牌
bool CTableFrameSink::OnUserListenCard(WORD wChairID, bool bListenCard)
{
	if(bListenCard)
	{
		ASSERT(!m_bTing[wChairID]);
		if(WIK_LISTEN == m_GameLogic.AnalyseTingCard(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID]))
		{
			m_bTing[wChairID] = true;
			CMD_S_ListenCard ListenCard;
			ZeroMemory(&ListenCard,sizeof(ListenCard));

			ListenCard.wListenUser = wChairID;
			ListenCard.bListen = true;
			//发给消息
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LISTEN_CARD,&ListenCard,sizeof(ListenCard));
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LISTEN_CARD,&ListenCard,sizeof(ListenCard));

			if(G_SHOW_HU_DATA)
			{
				SendHuData(wChairID);
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{

		ASSERT(m_bTing[wChairID]);
		m_bTing[wChairID] = false;

		CMD_S_ListenCard ListenCard;
		ListenCard.wListenUser = wChairID;
		ListenCard.bListen = false;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_LISTEN_CARD, &ListenCard, sizeof(ListenCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_LISTEN_CARD, &ListenCard, sizeof(ListenCard));
		return true;
	}
}

//用户托管
bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	//效验状态
	ASSERT((wChairID < m_cbPlayerCount));
	if ((wChairID>=m_cbPlayerCount)) return false;

	if (IsRoomCardScoreType()) return true;

	bool bChange = false;
	if (m_bTrustee[wChairID] != bTrustee)
	{
		m_bTrustee[wChairID] = bTrustee;
		bChange = true;
		CMD_S_Trustee Trustee;
		Trustee.bTrustee = bTrustee;
		Trustee.wChairID = wChairID;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));

		if (bTrustee)
		{
			CString cc;
			cc.Format(_T("玩家%d托管"), wChairID);
			WriteDebugInfo(cc);
		}
		else
		{
			CString cc;
			cc.Format(_T("玩家%d取消托管"), wChairID);
			WriteDebugInfo(cc);
		}
	}

	if(bTrustee)
	{
		if(wChairID == m_wCurrentUser)
		{
			
			BYTE cardindex=INVALID_BYTE;
			if(m_cbSendCardData != 0)
				cardindex = m_GameLogic.SwitchToCardIndex(m_cbSendCardData);
			else
			{
				for(int i=0;i<MAX_INDEX;i++)
				{
					if(m_cbCardIndex[wChairID][i]>0)
					{
						cardindex = i;
						break;
					}
				}
			}

			ASSERT(cardindex != INVALID_BYTE);
			OnUserOutCard(wChairID,m_GameLogic.SwitchToCardData(cardindex),true);
		}
		else if(m_wCurrentUser == INVALID_CHAIR && m_bResponse[wChairID]==false && m_cbUserAction[wChairID] != WIK_NULL )
		{
			BYTE operatecard[3]={0};
			OnUserOperateCard(wChairID,WIK_NULL,operatecard);
		}
	}
	else
	{
		if (m_wCurrentUser == wChairID && bChange)
		{
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeOutCard*1000, 1, 0);
			m_tActionStartTime = time(0);
		}
	}

	return true;
}

//发送操作
bool CTableFrameSink::SendOperateNotify()
{
	//发送提示
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		//构造数据
		CMD_S_OperateNotify OperateNotify;
		OperateNotify.wProvideUser = m_wProvideUser;
		OperateNotify.cbActionCard=m_cbProvideCard;
		OperateNotify.cbActionMask=m_cbUserAction[i];

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		if (m_pGameVideo)
		{
			m_pGameVideo->AddVideoData(SUB_S_OPERATE_NOTIFY, &OperateNotify, sizeof(OperateNotify), true);
		}

		if (OperateNotify.cbActionMask != WIK_NULL)
		{
			CString cc;
			cc.Format(_T("玩家%d可动作%d"), i, m_cbUserAction[i]);
			WriteDebugInfo(cc);
		}
	}


	m_pITableFrame->SetGameTimer(IDI_OPERATE, (m_pGameCustomRule->cbTimeOperateCard + 2) * 1000, 1, 0);
	m_tActionStartTime = time(0);
	return true;
}

//取得扑克
BYTE CTableFrameSink::GetSendCard(bool bTail)
{
	//发送扑克
	m_cbSendCardCount++;
	m_cbLeftCardCount--;

	BYTE cbSendCardData = 0;
	
	BYTE cbIndexCard;
	if(bTail)
	{	
		cbSendCardData = m_cbRepertoryCard[m_cbMinusLastCount];
		m_cbMinusLastCount++;
	}
	else
	{
		m_cbMinusHeadCount++;
		cbIndexCard = m_cbAllCardCount - m_cbMinusHeadCount;
		cbSendCardData=m_cbRepertoryCard[cbIndexCard];
	}	

	//堆立信息
	ASSERT(m_wHeapHead != INVALID_CHAIR && m_wHeapTail != INVALID_CHAIR);
	if(!bTail)
	{
		//切换索引
		BYTE cbHeapCount = m_cbHeapCardInfo[m_wHeapHead][0] + m_cbHeapCardInfo[m_wHeapHead][1];
		if (cbHeapCount == m_cbHeapCount[m_wHeapHead]) 
		{
			m_wHeapHead = (m_wHeapHead + GAME_PLAYER - 1) % CountArray(m_cbHeapCardInfo);
		}
		m_cbHeapCardInfo[m_wHeapHead][0]++;
	}
	else
	{
		//切换索引
		BYTE cbHeapCount = m_cbHeapCardInfo[m_wHeapTail][0] + m_cbHeapCardInfo[m_wHeapTail][1];
		if (cbHeapCount == m_cbHeapCount[m_wHeapTail]) 
		{
			m_wHeapTail = (m_wHeapTail + 1) % CountArray(m_cbHeapCardInfo);
		}
		m_cbHeapCardInfo[m_wHeapTail][1]++;
	}

	return cbSendCardData;
}

//派发扑克
bool CTableFrameSink::DispatchCardData(WORD wSendCardUser, bool bTail)
{
	//状态效验
	ASSERT(wSendCardUser != INVALID_CHAIR);
	if (wSendCardUser == INVALID_CHAIR) return false;

	ASSERT(m_enSendStatus != Not_Send);
	if(m_enSendStatus == Not_Send) return false;

	WORD wCurrentUser = wSendCardUser;

	//丢弃扑克
	if ((m_wOutCardUser != INVALID_CHAIR) && (m_cbOutCardData != 0))
	{
		m_cbOutCardCount++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]] = m_cbOutCardData;
		m_cbDiscardCount[m_wOutCardUser]++;
	}

	//荒庄结束
	if (m_cbLeftCardCount <= 14 )//广东麻将留14张牌
	{
		WriteDebugInfo(_T("荒庄"));
		m_cbChiHuCard = 0;
		m_wProvideUser = INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);

		return true;
	}

	//发送扑克
	m_cbProvideCard = GetSendCard(bTail);	
	m_cbSendCardData = m_cbProvideCard;
	m_wLastCatchCardUser = wSendCardUser;
	//清除禁止胡牌的牌
	m_bEnjoinChiHu[wCurrentUser] = false;
	m_vecEnjoinChiHu[wCurrentUser].clear();
	m_bEnjoinChiPeng[wCurrentUser] = false;		
	m_vecEnjoinChiPeng[wCurrentUser].clear();
	m_bEnjoinGang[wCurrentUser] = false;
	
	//设置变量	
	m_wOutCardUser = INVALID_CHAIR;
	m_cbOutCardData = 0;	
	m_wCurrentUser = wCurrentUser;
	m_wProvideUser = wCurrentUser;		
	m_bGangOutCard = false;
	
	if(bTail)//从尾部取牌，说明玩家杠牌了,计算分数
	{
		CalGangScore();
	}

	//加牌
	m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;

	//胡牌判断
	CChiHuRight chr;
	m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]--;
	m_cbUserAction[wCurrentUser] |= m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wCurrentUser], m_WeaveItemArray[wCurrentUser], m_cbWeaveItemCount[wCurrentUser], m_cbSendCardData, chr);
	m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	//杠牌判断
	if ((!m_bEnjoinGang[wCurrentUser]) && (m_cbLeftCardCount > m_cbEndLeftCount) && !m_bTing[wCurrentUser])
	{
		tagGangCardResult GangCardResult;
		m_cbUserAction[wCurrentUser] |= m_GameLogic.AnalyseGangCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser], m_cbWeaveItemCount[wCurrentUser], m_cbProvideCard, GangCardResult);
	}

	//听牌判断
	BYTE cbRes = WIK_NULL;
	if(m_bTing[wCurrentUser] == false)
	{
		cbRes =m_GameLogic.AnalyseTingCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser]);
		if(cbRes == WIK_LISTEN)
		{
			if(G_CAN_TING_CARD)
				m_cbUserAction[wCurrentUser] |= WIK_LISTEN; 

			if(G_SHOW_TING_DATA)
				SendTingData(wCurrentUser,m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser]);
		}
	}

	//构造数据
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard,sizeof(SendCard));
	SendCard.wSendCardUser = wSendCardUser;
	SendCard.wCurrentUser = wCurrentUser;
	SendCard.bTail = bTail;

	for (int i = 0; i < m_cbPlayerCount; i++)
	{
		if (m_bPlayStatus[i] == false) continue;

		if (i == wCurrentUser)
		{
			SendCard.cbActionMask = m_cbUserAction[wCurrentUser];
			SendCard.cbCardData = m_cbProvideCard;

			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard, sizeof(SendCard), true);
			}
		}
		else
		{
			SendCard.cbActionMask = WIK_NULL;
			SendCard.cbCardData = 0;
		}

		//发送数据
		m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
		m_pITableFrame->SendLookonData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	}

	if (m_bTrustee[wCurrentUser])
		m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeTrustee * 1000, 1, 0);
	else
		m_pITableFrame->SetGameTimer(IDI_OUT_CARD, (m_pGameCustomRule->cbTimeOutCard + 2) * 1000, 1, 0);

	m_tActionStartTime = time(0);

	return true;
}

//响应判断
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//变量定义
	bool bAroseAction = false;

	//用户状态
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

	//动作判断
	for (WORD i = 0; i < m_cbPlayerCount; i++)
	{
		//用户过滤
		if (wCenterUser == i || !m_bPlayStatus[i] || m_bTrustee[i]) continue;

		//出牌类型
		if (EstimatKind == EstimatKind_OutCard )
		{
			//吃碰判断
			if(!m_bEnjoinChiPeng[i])
			{
				bool bPeng = true;
				for(UINT j = 0; j < m_vecEnjoinChiPeng[i].size(); j++)
				{
					if(m_vecEnjoinChiPeng[i][j] == cbCenterCard)
					{
						bPeng = false;
					}
				}

				if(bPeng)
				{
					//碰牌判断
					m_cbUserAction[i] |= m_GameLogic.EstimatePengCard(m_cbCardIndex[i], cbCenterCard);
				}

				if(G_CAN_CHI_CARD)
				{
 					//吃牌判断
 					WORD wEatUser = (wCenterUser + 1) % GAME_PLAYER;
 					if (wEatUser == i)
 					{
 						m_cbUserAction[i] |= m_GameLogic.EstimateEatCard(m_cbCardIndex[i], cbCenterCard);
 					}
				}

				//杠牌判断
				if(m_cbLeftCardCount > m_cbEndLeftCount && !m_bEnjoinGang[i]) 
				{
					m_cbUserAction[i] |= m_GameLogic.EstimateGangCard(m_cbCardIndex[i], cbCenterCard);
				}
			}

			if(G_CHI_HU || (G_GANG_SHANG_PAO&&m_bGangOutCard))//吃胡和杠上炮
			{
				if (m_bEnjoinChiHu[i]==false)
				{
					bool bChiHu = true;
					for(UINT j = 0; j < m_vecEnjoinChiHu[i].size(); j++)
					{
						if(m_vecEnjoinChiHu[i][j] == cbCenterCard)
						{
							bChiHu = false;
							break;
						}
					}

					if(bChiHu)
					{
						CChiHuRight chr;
						m_cbUserAction[i] |= m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],m_cbWeaveItemCount[i],cbCenterCard,chr);
					}
				}
			}
		}
		else if (G_QIANG_GANG_HU && EstimatKind == EstimatKind_GangCard)//检查抢杠胡
   		{
			if(m_GameLogic.IsMagicCard(cbCenterCard) == FALSE)
			{
				bool bChiHu = true;
				for(UINT j = 0; j < m_vecEnjoinChiHu[i].size(); j++)
				{
					if(m_vecEnjoinChiHu[i][j] == cbCenterCard)
					{
						bChiHu = false;
						break;
					}
				}
				if(bChiHu)
				{
					//吃胡判断
					CChiHuRight chr;
					BYTE cbWeaveCount = m_cbWeaveItemCount[i];
					BYTE cbAction = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i], m_WeaveItemArray[i], cbWeaveCount, cbCenterCard, chr);
					m_cbUserAction[i] |= cbAction;
				}
			}
   		}

		//结果判断
		if (m_cbUserAction[i] != WIK_NULL) 
			bAroseAction = true;
	}

	//结果处理
	if (bAroseAction) 
	{
		//设置变量
		m_wProvideUser = wCenterUser;
		m_cbProvideCard = cbCenterCard;
		m_wResumeUser = m_wCurrentUser;
		m_wCurrentUser = INVALID_CHAIR;

		//发送提示
		SendOperateNotify();

		return true;
	}

	return false;
}

//算分
void CTableFrameSink::CalGameScore(CMD_S_GameConclude &GameConclude)
{
	bool bNoNegative = (m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0 || (m_pGameServiceOption->wServerType & SCORE_GENRE_POSITIVE) != 0;

	SCORE lCellScore = m_pITableFrame->GetCellScore();

	SCORE lUserScore[GAME_PLAYER] = {0};//玩家手上分
	SCORE lTempScore = 0;
	SCORE lTempAllScore = 0;
	for(int i = 0; i < GAME_PLAYER; i++)
	{
		if(!m_bPlayStatus[i])
			continue;

		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		ASSERT(pIServerUserItem);
		if(pIServerUserItem)
		{
			lUserScore[i] = pIServerUserItem->GetUserScore();
		}		
	}

	WORD wWinUser[GAME_PLAYER] = {INVALID_CHAIR};
	BYTE cbCount=0;

	for(int i = 0; i < GAME_PLAYER; i++)
	{
		WORD wChairID = (m_wBankerUser + i) % GAME_PLAYER;
		if(WIK_CHI_HU == m_dwChiHuKind[wChairID])
		{
			wWinUser[cbCount++] = wChairID;

			//统计胡牌次数
			m_stRecord.cbHuCount[wChairID]++;
		}
	}

	if(cbCount>0)//有人胡牌
	{
		bool bZiMo= (m_wProvideUser == wWinUser[0]);
		BYTE cbHuFan = GetUserHuFan(wWinUser[0]);
		BYTE cbZhongMaCount= GetUserZhongMaCount(wWinUser[0],GameConclude.cbZhongMaData);
		m_stRecord.cbMaCount[wWinUser[0]] += cbZhongMaCount;
		BYTE cbAddTime = 1;
		if(m_bNoMagicDouble && IsNoMagicCard(wWinUser[0]))
		{
			cbAddTime = 2;
			GameConclude.bNoMagicCard=TRUE;
		}
		//拷贝码数据
		GameConclude.cbMaCount = m_cbMaCount;
		CopyMemory(GameConclude.cbMaData,&m_cbRepertoryCard[m_cbAllCardCount-m_cbMinusHeadCount-m_cbMaCount],m_cbMaCount*sizeof(BYTE));
		GameConclude.cbZhongMaCount = cbZhongMaCount;

		SCORE lLoseScore = cbHuFan*lCellScore*cbAddTime;
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(!m_bPlayStatus[i])
				continue;

			if(i != wWinUser[0])
			{
				if(bZiMo)
				{
					GameConclude.lHuScore[i] -= lLoseScore;
					GameConclude.lMaScore[i] -= lLoseScore*cbZhongMaCount;
					GameConclude.lGameScore[i] -= lLoseScore*(1+cbZhongMaCount);
				}
				else//抢杠胡，胡牌分由被抢杠玩家出
				{
					GameConclude.lHuScore[m_wProvideUser] -= lLoseScore;
					GameConclude.lGameScore[m_wProvideUser] -= lLoseScore;

					GameConclude.lMaScore[i] -= lLoseScore*cbZhongMaCount;
					GameConclude.lGameScore[i] -= lLoseScore*cbZhongMaCount;
				}

				GameConclude.lHuScore[wWinUser[0]] += lLoseScore;
				GameConclude.lMaScore[wWinUser[0]] += lLoseScore*cbZhongMaCount;
				GameConclude.lGameScore[wWinUser[0]] += lLoseScore*(1+cbZhongMaCount);
			}
		}
		
		//庄家胡牌则继续当庄，否则庄家下一位坐庄
		if(m_wBankerUser != wWinUser[0])
			m_wBankerUser = (m_wBankerUser+1)%GAME_PLAYER;

	}
	else//荒庄
	{
		m_wBankerUser = (m_wBankerUser+1)%GAME_PLAYER; 
	}

	//最后加上杠的输赢分
	for(int i=0;i<GAME_PLAYER;i++)
	{
		GameConclude.lGangScore[i] += m_lUserGangScore[i];
		GameConclude.lGameScore[i] += m_lUserGangScore[i];
	}

}

void CTableFrameSink::CalGangScore()
{
	SCORE lcell = m_pITableFrame->GetCellScore();
	if(m_cbGangStatus == WIK_FANG_GANG)//放杠一家扣分
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(!m_bPlayStatus[i])
				continue;
			if(i != m_wCurrentUser)
			{
				m_lUserGangScore[m_wProvideGangUser] -= lcell;
				m_lUserGangScore[m_wCurrentUser] += lcell;
			}
		}
		//记录明杠次数
		m_stRecord.cbMingGang[m_wCurrentUser]++;
	}
	else if(m_cbGangStatus == WIK_MING_GANG)//明杠每家出1倍
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(!m_bPlayStatus[i])
				continue;
			if(i != m_wCurrentUser)
			{
				m_lUserGangScore[i] -= lcell;
				m_lUserGangScore[m_wCurrentUser] += lcell;
			}
		}
		//记录明杠次数
		m_stRecord.cbMingGang[m_wCurrentUser]++;
	}
	else if(m_cbGangStatus == WIK_AN_GANG)//暗杠每家出2倍
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(!m_bPlayStatus[i])
				continue;
			if(i != m_wCurrentUser)
			{
				m_lUserGangScore[i] -= 2*lcell;
				m_lUserGangScore[m_wCurrentUser] += 2*lcell;
			}
		}
		//记录暗杠次数
		m_stRecord.cbAnGang[m_wCurrentUser]++;
	}
}
//权位过滤
void CTableFrameSink::FiltrateRight(WORD wWinner, CChiHuRight &chr)
{
	//天胡判断
	if(m_cbSendCardCount == 0 && wWinner == m_wBankerUser)
		chr |= CHR_TIAN_HU;
	if(m_bCanDiHu[wWinner])
		chr |= CHR_DI_HU;

	if(wWinner == m_wProvideUser)
	{
		if(m_cbGangStatus != WIK_GANERAL)
			chr |= CHR_GANG_SHANG_HUA;
	}
	else
	{
		if(m_bGangOutCard)//杠上炮
		{
			chr |= CHR_GANG_SHANG_PAO;
		}
		else if(m_cbGangStatus == WIK_MING_GANG)
		{
			chr |= CHR_QIANG_GANG_HU;
		}
	}
}

//设置基数
void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
//断线不马上操作，等倒计时结束
// 	if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 0)//约占积分
// 		return true;
// 
// 	return OnUserTrustee(wChairID,true);
	return true;
}

BYTE CTableFrameSink::GetUserZhongMaCount(WORD wChairId, BYTE cbData[])
{
	BYTE cbZhongMaCount = 0;
	ASSERT(m_cbMaCount >= 0 && m_cbMaCount <= 8);

	BYTE cbMaData[MAX_MA_COUNT]={0};
	CopyMemory(cbMaData,&m_cbRepertoryCard[m_cbAllCardCount-m_cbMinusHeadCount-m_cbMaCount],m_cbMaCount*sizeof(BYTE));

	BYTE OffSet = (wChairId-m_wBankerUser+GAME_PLAYER)%GAME_PLAYER;
	for(int i=0;i<m_cbMaCount;i++)
	{
		BYTE cardvalue = cbMaData[i]&0x0F;

		if(OffSet == 0)//庄家159东
		{
			if(cardvalue%4 == 1 && cbMaData[i] != 0x35)
				cbData[cbZhongMaCount++] = cbMaData[i];
		}
		else if(OffSet == 1)//下家26东南
		{
			if(cbMaData[i]<0x36)
			{
				if(cardvalue%4 == 2 || cbMaData[i] == 0x35)
					cbData[cbZhongMaCount++] = cbMaData[i];
			}
		}
		else if(OffSet == 2)//对家发西
		{
			if(cardvalue%4 == 3 || cbMaData[i] == 0x36)
				cbData[cbZhongMaCount++] = cbMaData[i];
		}
		else if(OffSet == 3)//上家48北白
		{
			if(cardvalue%4 == 0 || cbMaData[i] == 0x37)
				cbData[cbZhongMaCount++] = cbMaData[i];
		}
	}
	

	return cbZhongMaCount;
}

BYTE CTableFrameSink::GetRemainingCount(WORD wChairID,BYTE cbCardData)
{
	BYTE cbIndex = m_GameLogic.SwitchToCardIndex(cbCardData);
	BYTE nCount=0;
	for(int i=m_cbMinusLastCount;i<m_cbAllCardCount-m_cbMinusHeadCount;i++)
	{
		if(m_cbRepertoryCard[i] == cbCardData)
			nCount++;
	}
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if( i == wChairID)
			continue;

		nCount += m_cbCardIndex[i][cbIndex];
	}
	return nCount;
}

void CTableFrameSink::SendTingData(WORD wChairID,const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	CMD_S_TING_DATA TingData;
	ZeroMemory(&TingData,sizeof(TingData));

	m_GameLogic.GetTingDataEx(m_cbCardIndex[wChairID],WeaveItem,cbWeaveCount,TingData.cbOutCardCount,TingData.cbOutCardData,TingData.cbHuCardCount,TingData.cbHuCardData,TingData.cbHuFan);
	for(int i=0;i<MAX_COUNT;i++)
	{
		if(TingData.cbHuCardCount[i]>0)
		{
			for(int j=0;j<TingData.cbHuCardCount[i];j++)
			{
				TingData.cbHuCardRemainingCount[i][j] = GetRemainingCount(wChairID,TingData.cbHuCardData[i][j]);
			}
		}
		else
			break;
	}

	m_pITableFrame->SendTableData(wChairID,SUB_S_TING_DATA,&TingData,sizeof(TingData));
}

void CTableFrameSink::SendHuData(WORD wChairID)
{
	//计算胡几张字
	CMD_S_HU_DATA pack;
	pack.wUser = wChairID;

	m_cbHuCardCount[wChairID] = m_GameLogic.GetHuCard(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID],m_cbHuCardData[wChairID]);
	ASSERT(m_cbHuCardCount[wChairID]>0);

	pack.cbHuCardCount = m_cbHuCardCount[wChairID];
	CopyMemory(pack.cbHuCardData,m_cbHuCardData[wChairID],sizeof(pack.cbHuCardData));

	m_pITableFrame->SendTableData(wChairID,SUB_S_HU_DATA,&pack,sizeof(pack));
}

bool CTableFrameSink::IsRoomCard()
{
	return ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0);
}

bool CTableFrameSink::IsRoomCardScoreType()
{
	return (m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

bool CTableFrameSink::IsRoomCardTreasureType()
{
	return (m_pITableFrame->GetDataBaseMode() == 1) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

BYTE CTableFrameSink::GetUserHuFan(WORD wChairId)
{
	ASSERT(wChairId>= 0 && wChairId<GAME_PLAYER);
	if( (m_ChiHuRight[wChairId] & CHK_MASK_BIG) != 0)
		return 4;
	else if( (m_ChiHuRight[wChairId] & CHK_MASK_SMALL) != 0)
		return 2;

	ASSERT(FALSE);
	return 0;
}

bool CTableFrameSink::IsNoMagicCard(WORD wHuUser)
{
	if(m_cbMagicMode==0)
		return false;
	BYTE bRes = WIK_NULL;
	CGameLogic tempLogic;
	tempLogic = m_GameLogic;
	tempLogic.SetMagicIndex(NULL);

	CChiHuRight chr;
	bRes =tempLogic.AnalyseChiHuCard(m_cbCardIndex[wHuUser], m_WeaveItemArray[wHuUser],m_cbWeaveItemCount[wHuUser], m_cbProvideCard, chr);
	return bRes==WIK_CHI_HU;
}

bool CTableFrameSink::OnUserFlee(WORD wChairID)
{
	if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//房卡模式
		return true;

	if(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY)
	{
		m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
		m_pITableFrame->KillGameTimer(IDI_OPERATE);
		//逃跑，赔其他玩家小胡自摸分
		
		CMD_S_GameConclude GameConclude;
		ZeroMemory(&GameConclude, sizeof(GameConclude));
		GameConclude.cbSendCardData=m_cbSendCardData;
		GameConclude.wFleeUser = wChairID;
		GameConclude.wProvideUser=INVALID_CHAIR;
		memcpy(GameConclude.cbMagicIndex, m_cbMagicIndex, sizeof(m_cbMagicIndex));
		SCORE lLoseScore = (m_cbPlayerCount-1)*m_pITableFrame->GetCellScore()*2*(m_cbPlayerCount-1);
		if(lLoseScore > m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore())
			lLoseScore = m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore();

		GameConclude.lGameScore[wChairID] -= lLoseScore;
		for(WORD i=0;i<m_cbPlayerCount;i++)
		{
			if(!m_bPlayStatus[i])
				continue;
			if(i != wChairID)
			{
				GameConclude.lGameScore[i] += ( lLoseScore/(m_cbPlayerCount-1) );
			}
			GameConclude.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameConclude.cbHandCardData[i]);
			GameConclude.cbWeaveItemCount[i] = m_cbWeaveItemCount[i];
			memcpy(GameConclude.WeaveItemArray[i], m_WeaveItemArray[i], sizeof(GameConclude.WeaveItemArray[i]));
		}

		//积分变量
		tagScoreInfo ScoreInfoArray[GAME_PLAYER];
		ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

		//统计积分
		for (WORD i = 0; i < m_cbPlayerCount; i++)
		{
			if(!m_bPlayStatus[i])
				continue;

			//收税
			if (GameConclude.lGameScore[i]>0 && (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0)
			{
				GameConclude.lRevenue[i] = m_pITableFrame->CalculateRevenue(i,GameConclude.lGameScore[i]);
				GameConclude.lGameScore[i] -= GameConclude.lRevenue[i];
			}

			ScoreInfoArray[i].lScore = GameConclude.lGameScore[i];
			ScoreInfoArray[i].lRevenue = GameConclude.lRevenue[i];
			ScoreInfoArray[i].cbType = ScoreInfoArray[i].lScore > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_FLEE;

			//历史积分
			m_HistoryScore.OnEventUserScore(i, GameConclude.lGameScore[i]);

		}

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
		if (m_pGameVideo)
		{
			m_pGameVideo->AddVideoData(SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude),true);
			m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
		}
		//写入积分
		m_pITableFrame->WriteTableScore(ScoreInfoArray, m_cbPlayerCount);

		//结束游戏
		m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
	}

	return true;
}


void CTableFrameSink::WriteDebugInfo(LPCTSTR pszString)
{
	CString strtip;
	strtip.Format(TEXT("房间号%d--%s\n"), m_pITableFrame->GetTableID(),pszString);
	m_pITableFrame->SendGameLogData(strtip);
}

//////////////////////////////////////////////////////////////////////////////////
