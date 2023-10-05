#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
//#define IDI_FREE					1									//空闲时间
#define IDI_PLACE_JETTON			102									//下注时间
//#define IDI_DISPATCH_CARD			301									//发牌时间

#define IDI_PLACE_JETTON1			103									//下注定时
#define IDI_PLACE_JETTON2			104									//下注定时
#define IDI_PLACE_JETTON3			105									//下注定时
#define IDI_PLACE_JETTON4			106									//下注定时
#define IDI_PLACE_JETTON5			107									//下注定时

#define IDI_GIVEUP_BANKER            101									//下庄

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink():
m_wCurrentBanker(INVALID_CHAIR)
{


	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	m_wCurrentBanker = INVALID_CHAIR;
	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	
	switch(nTimerID)
	{
	case IDI_GIVEUP_BANKER:
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);
			m_pIAndroidUserItem->SendSocketData(SUB_C_CANCEL_BANKER);
		}
	    break;
	default:
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);

			int JetScore[]={100, 100, 100, 1000, 100, 1000, 100, 1000, 100, 100};
			int JetScoreMax[] = { 1000, 10000, 100000, 200000, 1000, 10000, 100000, 200000, 500000, 500000 };
			//发送消息
			CMD_C_PlaceJetton JettonInfo;
			INT32 nRandGameType = RandInt(0,1);
			if (nRandGameType == eGambleType_AnimalGame)
			{
				JettonInfo.eGamble = eGambleType_AnimalGame;
				JettonInfo.eEnjoyGameInfo = eEnjoyGameType_Invalid;

				JettonInfo.eGamble = eGambleType_AnimalGame;
				JettonInfo.stAnimalInfo.eAnimal = (eAnimalType) RandInt(eAnimalType_Lion,eAnimalType_Rabbit);
				JettonInfo.stAnimalInfo.eColor = (eColorType)RandInt(eColorType_Red,eColorType_Yellow);
				JettonInfo.eEnjoyGameInfo = eEnjoyGameType_Invalid;
				if (m_lMinEnterScore >200000)
				{
					JettonInfo.iPlaceJettonScore = JetScoreMax[RandInt(0, 9)];
				}
				else
				{
					JettonInfo.iPlaceJettonScore = JetScore[RandInt(0, 9)];
				}
				
			}
			else
			{
				JettonInfo.eGamble = eGambleType_EnjoyGame;
				JettonInfo.stAnimalInfo.eAnimal = (eAnimalType)0;
				JettonInfo.stAnimalInfo.eColor = (eColorType)0;
				JettonInfo.eEnjoyGameInfo = (eEnjoyGameType)RandInt(eEnjoyGameType_Zhuang,eEnjoyGameType_He);
				if (m_lMinEnterScore >200000)
				{
					JettonInfo.iPlaceJettonScore = JetScoreMax[RandInt(0, 9)];
				}
				else
				{
					JettonInfo.iPlaceJettonScore = JetScore[RandInt(0, 9)];
				}
			}

			m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_JETTON, &JettonInfo, sizeof(JettonInfo));

		}
		break;
	}
	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_FREE:			//游戏空闲 
		{
			return OnSubGameFree(pBuffer, wDataSize);
		}
	case SUB_S_GAME_START:			//游戏开始
		{
			return OnSubGameStart(pBuffer, wDataSize);
		}
	case SUB_S_PLACE_JETTON:		//用户加注
		{
			return OnSubPlaceJetton(pBuffer, wDataSize);
		}
	case SUB_S_APPLY_BANKER:		//申请做庄 
		{
			return OnSubUserApplyBanker(pBuffer,wDataSize);
		}
	case SUB_S_CANCEL_BANKER:		//取消做庄 
		{
			return OnSubUserCancelBanker(pBuffer,wDataSize);
		}
	case SUB_S_CHANGE_BANKER:		//切换庄家 
		{
			return OnSubChangeBanker(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:			//游戏结束 
		{
			return OnSubGameEnd(pBuffer, wDataSize);
		}
	case SUB_S_SEND_RECORD:			//游戏记录 (忽略)
		{
			return true;
		}
	case SUB_S_PLACE_JETTON_FAIL:	//下注失败 (忽略)
		{
			return true;
		}
	case SUB_S_GAME_OPTION:
		{
			return OnSubGameOption(pBuffer, wDataSize);
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;

	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{

	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:			//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//消息处理
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//m_lUserLimitScore = pStatusFree->lUserMaxScore;
			//m_lAreaLimitScore = pStatusFree->lAreaLimitScore;
			//m_lBankerCondition = pStatusFree->lApplyBankerCondition;

			//memcpy(m_szRoomName, pStatusFree->szGameRoomName, sizeof(m_szRoomName));

			//ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, true);

			////MyDebug(_T("机器人上庄(Free) %d [%d %d] [%d %d]"), m_pIAndroidUserItem->GetChairID(), m_nWaitBanker, 
			////	m_nRobotWaitBanker, m_stlApplyBanker, m_nRobotApplyBanker);

			////上庄处理
			//if (pStatusFree->wBankerUser == INVALID_CHAIR)
			//{
			//	if (m_bRobotBanker && m_nRobotWaitBanker == 0  && m_stlApplyBanker < m_nRobotApplyBanker)
			//	{
			//		//合法判断
			//		IServerUserItem *pIUserItemBanker = m_pIAndroidUserItem->GetMeUserItem();
			//		if (pIUserItemBanker->GetUserScore()->lScore > m_lBankerCondition) 
			//		{
			//			//机器人上庄
			//			m_nBankerCount = 0;
			//			m_stlApplyBanker++;

			//			BYTE cbTime = (pStatusFree->cbTimeLeave>0?(rand()%pStatusFree->cbTimeLeave+1):2);
			//			if (cbTime == 0) cbTime = 2;

			//			m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST_BANKER, cbTime);
			//		}
			//	}
			//}

			return true;
		}
	case GAME_STATUS_PLAY:		//游戏状态
	case GS_GAME_END:		//结束状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			////消息处理
			//CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			////庄家信息
			//m_wCurrentBanker = pStatusPlay->wBankerUser;

			//m_lUserLimitScore = pStatusPlay->lUserMaxScore;
			//m_lAreaLimitScore = pStatusPlay->lAreaLimitScore;
			//m_lBankerCondition = pStatusPlay->lApplyBankerCondition;

			//memcpy(m_szRoomName, pStatusPlay->szGameRoomName, sizeof(m_szRoomName));

			//ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, true);

			return true;
		}
	}

	return true;
}

//用户进入
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

// 所有玩家都开始了
void CAndroidUserItemSink::OnEventAllUserStart(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return ;
}

//游戏空闲
bool CAndroidUserItemSink::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//读取配置
	//if (m_bRefreshCfg)
	//	ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, false);
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameFree));
	if (wDataSize!=sizeof(CMD_S_GameFree)) return false;

	//消息处理
	CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;

	//如果可以申请下庄，就一定概率下庄
	if(pGameFree->cbCanCancelBank == 1)
	{
		int nRandGiveBanker = RandInt(1,100);
		if (nRandGiveBanker < 30)
		{
			m_pIAndroidUserItem->SetGameTimer(IDI_GIVEUP_BANKER, 2);
		}
	}
	return true;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{	
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	srand(GetTickCount());

	if (pGameStart->cbBankerFlag == 1)
	{
		m_wCurrentBanker = m_pIAndroidUserItem->GetChairID();
	}
	else
	{
		//计算下注次数
		int nElapse = 0;
		WORD wMyID = m_pIAndroidUserItem->GetChairID();
		BYTE m_nChipTime = (rand() + wMyID) % 5 + 3;

		//设置时间
		int nTimeGrid = int(pGameStart->cbTimeLeave - 2) * 800 / m_nChipTime;						//时间格,前2秒不下注,所以-2,800表示机器人下注时间范围千分比
		for (int i = 0; i < m_nChipTime; i++)
		{
			int nRandRage = int(nTimeGrid * i / (1500 * sqrt((double)m_nChipTime))) + 1;		//波动范围
			nElapse = 2 + (nTimeGrid*i) / 1000 + ((rand() + wMyID) % (nRandRage * 2) - (nRandRage - 1));
			ASSERT(nElapse >= 2 && nElapse <= pGameStart->cbTimeLeave);
			if (nElapse < 2 || nElapse > pGameStart->cbTimeLeave)	continue;

			m_pIAndroidUserItem->SetGameTimer(IDI_PLACE_JETTON + i + 1, nElapse);
		}

		m_wCurrentBanker = INVALID_CHAIR;
	}


	////自己当庄或无下注机器人
	//if (pGameStart->wBankerUser == m_pIAndroidUserItem->GetChairID() || pGameStart->nChipRobotCount <= 0)
	//	return true;

	////设置变量
	//m_lMaxChipBanker = pGameStart->lBankerScore/m_RobotInfo.nMaxTime;
	//m_lMaxChipUser = __min(pGameStart->lUserMaxScore/m_RobotInfo.nMaxTime, m_lUserLimitScore);
	//m_wCurrentBanker = pGameStart->wBankerUser;
	//m_nChipTimeCount = 0;
	//ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
	//for (int i = 0; i < AREA_COUNT; i++)
	//	m_lAreaChip[i] = m_lAreaLimitScore;

	////系统当庄
	//if (pGameStart->wBankerUser == INVALID_CHAIR)
	//{
	//	m_stlApplyBanker = 0;
	//	m_lMaxChipBanker = 2147483647/m_RobotInfo.nMaxTime;
	//}
	//else
	//	m_lMaxChipUser = __min( m_lMaxChipUser, m_lMaxChipBanker );

	////计算下注次数
	//int nElapse = 0;												
	//WORD wMyID = m_pIAndroidUserItem->GetChairID();

	//if (m_nRobotBetTimeLimit[0] == m_nRobotBetTimeLimit[1])
	//	m_nChipTime = m_nRobotBetTimeLimit[0];
	//else
	//	m_nChipTime = (rand()+wMyID)%(m_nRobotBetTimeLimit[1]-m_nRobotBetTimeLimit[0]+1) + m_nRobotBetTimeLimit[0];
	//ASSERT(m_nChipTime>=0);		
	//if (m_nChipTime <= 0)	return false;								//的确,2个都带等于
	//if (m_nChipTime > MAX_CHIP_TIME)	m_nChipTime = MAX_CHIP_TIME;	//限定MAX_CHIP次下注

	////计算范围
	//if (!CalcJettonRange(__min(m_lMaxChipBanker, m_lMaxChipUser), m_lRobotJettonLimit, m_nChipTime, m_nChipLimit))
	//	return true;

	////设置时间
	//int nTimeGrid = int(pGameStart->cbTimeLeave-2)*800/m_nChipTime;		//时间格,前2秒不下注,所以-2,800表示机器人下注时间范围千分比
	//for (int i = 0; i < m_nChipTime; i++)
	//{
	//	int nRandRage = int( nTimeGrid * i / (1500*sqrt((double)m_nChipTime)) ) + 1;		//波动范围
	//	nElapse = 2 + (nTimeGrid*i)/1000 + ( (rand()+wMyID)%(nRandRage*2) - (nRandRage-1) );
	//	ASSERT(nElapse>=2&&nElapse<=pGameStart->cbTimeLeave);
	//	if (nElapse < 2 || nElapse > pGameStart->cbTimeLeave)	continue;

	//	m_pIAndroidUserItem->SetGameTimer(IDI_PLACE_JETTON+i+1, nElapse);
	//}

	//ASSERT( MyDebug(_T("机器人 %d 下注次数 %d 范围 [%d %d] 总人数 %d 限制 [%I64d %I64d] 上庄 [%d %d]"), wMyID, m_nChipTime, m_nChipLimit[0], m_nChipLimit[1], 
	//	pGameStart->nChipRobotCount, m_lMaxChipBanker, m_lMaxChipUser, m_stlApplyBanker, m_nRobotApplyBanker) );

	return true;
}

//用户加注
bool CAndroidUserItemSink::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJetton));
	if (wDataSize!=sizeof(CMD_S_PlaceJetton)) return false;

	//消息处理
	CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;

	////设置变量
	//m_lMaxChipBanker -= pPlaceJetton->lJettonScore;
	//m_lAreaChip[pPlaceJetton->cbJettonArea-1] -= pPlaceJetton->lJettonScore;
	//if (pPlaceJetton->wChairID == m_pIAndroidUserItem->GetChairID())
	//	m_lMaxChipUser -= pPlaceJetton->lJettonScore;

	return true;
}

//下注失败
bool CAndroidUserItemSink::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//if ( m_wCurrentBanker == INVALID_CHAIR )
	//{
	//	//空庄
	//	m_nWaitBanker++;

	//	//MyDebug(_T("机器人上庄(End) %d [%d %d] [%d %d]"), m_pIAndroidUserItem->GetChairID(), m_nWaitBanker, 
	//	//	m_nRobotWaitBanker, m_stlApplyBanker, m_nRobotApplyBanker);

	//	//机器人上庄
	//	if ( m_bRobotBanker && m_nWaitBanker >= m_nRobotWaitBanker && m_stlApplyBanker < m_nRobotApplyBanker)
	//	{
	//		//合法判断
	//		IServerUserItem *pIUserItemBanker = m_pIAndroidUserItem->GetMeUserItem();
	//		if (pIUserItemBanker->GetUserScore()->lScore > m_lBankerCondition) 
	//		{
	//			//机器人上庄
	//			m_nBankerCount = 0;
	//			m_stlApplyBanker++;
	//			m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST_BANKER, (rand() % pGameEnd->cbTimeLeave) + 1);
	//		}
	//	}
	//}
	//else if ( m_wCurrentBanker == m_pIAndroidUserItem->GetChairID() )
	//{
	//	m_nBankerCount++;
	//	if ( m_nBankerCount >= m_nRobotBankerCount )
	//	{
	//		//机器人走庄
	//		m_nBankerCount = 0;
	//		m_pIAndroidUserItem->SetGameTimer(IDI_GIVEUP_BANKER, pGameEnd->cbTimeLeave + 1);
	//	}
	//}
	//else if ( m_wCurrentBanker != INVALID_CHAIR )
	//{
	//	//其他人坐庄
	//	m_nWaitBanker = 0;
	//}

	return true;
}

//申请做庄
bool CAndroidUserItemSink::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) return false;

	//消息处理
	CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;

	////自己判断
	//if (m_pIAndroidUserItem->GetChairID()==pApplyBanker->wApplyUser) 
	//	m_bMeApplyBanker=true;

	return true;
}

//取消做庄
bool CAndroidUserItemSink::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
	if (wDataSize!=sizeof(CMD_S_CancelBanker)) return false;

	//消息处理
	CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;

	////自己判断
	//if (lstrcmp(m_pIAndroidUserItem->GetMeUserItem()->GetAccounts(),pCancelBanker->szCancelUser)==0) 
	//	m_bMeApplyBanker=false;

	return true;
}

//切换庄家
bool CAndroidUserItemSink::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
	if (wDataSize!=sizeof(CMD_S_ChangeBanker)) return false;

	//消息处理
	CMD_S_ChangeBanker * pChangeBanker = (CMD_S_ChangeBanker *)pBuffer;

	//if ( m_wCurrentBanker == m_pIAndroidUserItem->GetChairID() && m_wCurrentBanker != pChangeBanker->wBankerUser )
	//{
	//	m_stlApplyBanker--;
	//}
	//m_wCurrentBanker = pChangeBanker->wBankerUser;
	//m_nWaitBanker = 0;

	return true;
}

//游戏配置
bool CAndroidUserItemSink::OnSubGameOption(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_Game_Option));
	if (wDataSize != sizeof(CMD_S_Game_Option)) return false;

	//消息处理
	CMD_S_Game_Option* GameOption = (CMD_S_Game_Option *)pBuffer;

	if (GameOption != NULL)
	{
		m_lMinTableScore = GameOption->lMinTableScore;
		m_lMinEnterScore = GameOption->lMinEnterScore;
		m_lMaxEnterScore = GameOption->lMaxEnterScore;
	}

	return true;
}

bool CAndroidUserItemSink::IsBanker(IAndroidUserItem * pIAndroidUserItem)
{
	if (pIAndroidUserItem->GetChairID() == m_wCurrentBanker)
	{
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
