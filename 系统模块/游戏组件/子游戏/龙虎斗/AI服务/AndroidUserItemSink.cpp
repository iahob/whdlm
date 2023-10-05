#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_REQUEST_OCCUPYSEAT		100									//请求占位
#define IDI_REQUEST_BANKER			101									//申请上庄
#define IDI_QUIT_BANKER				102									//下庄
#define IDI_BANK_OPERATE			103									//银行定时
#define IDI_PLACE_JETTON			104									//下注定义 (预留110-160)

//最大下注次数
#define MAX_CHIP_TIME				50

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//占位
	m_bEnableRobotOccupySeat = true;
	m_lOccupySeatFree = 1000000;
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;
	}

	//上庄变量
	m_bEnableBanker = true;
	m_lBankerCondition = 10000000;
	m_wCurrentBanker = INVALID_CHAIR;
	m_bRobotBanker = true;
	m_bMeApplyBanker = false;
	ZeroMemory(&m_tmAICancelRequest, sizeof(m_tmAICancelRequest));
	ZeroMemory(&m_tmAIQuitBanker, sizeof(m_tmAIQuitBanker));
	m_ApplyUserArray.RemoveAll();

	//机器人存取款
	m_lPlayTimeCount = 0;
	m_cbSave = FALSE;
	m_wAIBankOperateChange = 0;
	m_dAIBankOperateScore = 0;

	//游戏变量
	m_lMaxChipUser = 0;
	m_nChipTime = 0;
	m_nChipTimeCount = 0;
	ZeroMemory(m_lCurUserInAllScore, sizeof(m_lCurUserInAllScore));
	m_cbTimeLeave = 0;
	m_lMinTableScore = 0;

	ZeroMemory(m_lAreaChip, sizeof(m_lAreaChip));
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
	m_nRobotBetTimeLimit[0] = 3;
	m_nRobotBetTimeLimit[1] = 8;
	m_lRobotJettonLimit[0] = 10;
	m_lRobotJettonLimit[1] = 1000000;

	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink, Guid, dwQueryVer);
	return NULL;
}

//初始接口
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IAndroidUserItem);
	if (m_pIAndroidUserItem == NULL) return false;

	return true;
}

//重置接口
bool  CAndroidUserItemSink::RepositionSink()
{
	m_ApplyUserArray.RemoveAll();
	ZeroMemory(&m_tmAICancelRequest, sizeof(m_tmAICancelRequest));
	ZeroMemory(&m_tmAIQuitBanker, sizeof(m_tmAIQuitBanker));

	//游戏变量
	m_lMaxChipUser = 0;
	m_nChipTime = 0;
	m_nChipTimeCount = 0;
	ZeroMemory(m_lCurUserInAllScore, sizeof(m_lCurUserInAllScore));
	m_cbTimeLeave = 0;

	ZeroMemory(m_lAreaChip, sizeof(m_lAreaChip));
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));

	m_bMeApplyBanker = false;

	return true;
}

//时间消息
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_REQUEST_OCCUPYSEAT:
	{
								   m_pIAndroidUserItem->KillGameTimer(nTimerID);

								   //判断挂机
								   //if (m_pIAndroidUserItem->IsAiHanging())
								   //{
									  // return false;
								   //}

								   //不支持机器人占位
								   if (!m_bEnableRobotOccupySeat)
								   {
									   return false;
								   }

								   //判断占位条件
								   IServerUserItem *pIUserItem = m_pIAndroidUserItem->GetMeUserItem();
								   if (pIUserItem->GetUserScore() < m_lOccupySeatFree)
								   {
									   return false;
								   }

								   //判断已经占位
								   for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
								   {
									   //占位失败
									   if (m_wOccupySeatChairIDArray[i] == pIUserItem->GetChairID())
									   {
										   return false;
									   }
								   }

								   //有效占位索引
								   BYTE cbValidIndexArray[MAX_OCCUPY_SEAT_COUNT];
								   memset(cbValidIndexArray, INVALID_BYTE, sizeof(cbValidIndexArray));
								   BYTE cbValidIndexCount = 0;

								   for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
								   {
									   //占位
									   if (m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
									   {
										   cbValidIndexArray[cbValidIndexCount++] = i;
									   }
								   }

								   if (cbValidIndexCount > 0)
								   {
									   //发送占位消息
									   CMD_C_OccupySeat OccupySeat;
									   ZeroMemory(&OccupySeat, sizeof(OccupySeat));

									   OccupySeat.wOccupySeatChairID = pIUserItem->GetChairID();
									   OccupySeat.cbOccupySeatIndex = cbValidIndexArray[rand() % cbValidIndexCount];

									   ASSERT(OccupySeat.cbOccupySeatIndex >= SEAT_LEFT1_INDEX && OccupySeat.cbOccupySeatIndex <= SEAT_RIGHT3_INDEX);
									   if (OccupySeat.cbOccupySeatIndex >= SEAT_LEFT1_INDEX && OccupySeat.cbOccupySeatIndex <= SEAT_RIGHT3_INDEX)
									   {
										   //发送消息
										   m_pIAndroidUserItem->SendSocketData(SUB_C_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));
									   }
								   }

								   return false;
	}
	case IDI_REQUEST_BANKER:	//申请上庄
	{
									if (!m_bEnableBanker)
									{
										return false;
									}

									m_pIAndroidUserItem->KillGameTimer(nTimerID);
									m_pIAndroidUserItem->SendSocketData(SUB_C_APPLY_BANKER);

									return false;
	}
	case IDI_QUIT_BANKER:		//申请下庄
	{
									if (!m_bEnableBanker)
									{
										return false;
									}

									m_pIAndroidUserItem->KillGameTimer(nTimerID);
									m_pIAndroidUserItem->SendSocketData(SUB_C_CANCEL_BANKER);

									return false;
	}
	case IDI_BANK_OPERATE:		//银行操作
	{
									m_pIAndroidUserItem->KillGameTimer(nTimerID);

									//判断存取
									if (m_cbSave == TRUE)
									{
										m_pIAndroidUserItem->PerformSaveScore(m_dAIBankOperateScore);
									}
									else if (m_cbSave == FALSE)
									{
										m_pIAndroidUserItem->PerformTakeScore(m_dAIBankOperateScore);
									}

									//校验坐下最低成绩
									if ((m_pIAndroidUserItem->GetMeUserItem()->GetUserScore() < m_lMinTableScore))
									{
										SCORE dCarryScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
										SCORE	dAITakeScore = ((dCarryScore < (SCORE)0) ? (-dCarryScore + m_lMinTableScore) : (m_lMinTableScore - dCarryScore));

										m_pIAndroidUserItem->PerformTakeScore(dAITakeScore);
									}

									return false;
	}
	default:
	{
			   if (nTimerID >= IDI_PLACE_JETTON && nTimerID <= IDI_PLACE_JETTON + MAX_CHIP_TIME)
			   {
				   srand(GetTickCount());

				   //下注分析
				   AnalyseAddScore();

				   m_pIAndroidUserItem->KillGameTimer(nTimerID);

				   return false;
			   }
	}
	}
	return false;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
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
		return OnSubUserApplyBanker(pBuffer, wDataSize);
	}
	case SUB_S_CANCEL_BANKER:		//取消做庄 
	{
		return OnSubUserCancelBanker(pBuffer, wDataSize);
	}
	case SUB_S_CHANGE_BANKER:		//切换庄家 
	{
		return OnSubChangeBanker(pBuffer, wDataSize);
	}
	case SUB_S_GAME_END:			//游戏结束 
	{
		return OnSubGameEnd(pBuffer, wDataSize);
	}
	case SUB_S_OCCUPYSEAT:
	{
		return OnSubOccupySeat(pBuffer, wDataSize);
	}
	case SUB_S_OCCUPYSEAT_FAIL:
	{
		return OnSubOccupySeatFail(pBuffer, wDataSize);
	}
	case SUB_S_UPDATE_OCCUPYSEAT:
	{
		return OnSubUpdateOccupySeat(pBuffer, wDataSize);
	}
	case SUB_S_OCCUPYSEAT_STANDUP:
	{
		return OnSubOccupySeatStandUp(pBuffer, wDataSize);
	}
	case SUB_S_FREE_AICONFIG:
	{
		return OnSubFreeAIConfig(pBuffer, wDataSize);
	}
	case SUB_S_PLACE_JETTON_FAIL:
	{
		return OnSubPlaceJettonFail(pBuffer, wDataSize);
	}
	default:
		return true;
	}

	return true;
}

//场景消息
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:				//空闲状态
	{
											//效验数据
											ASSERT(wDataSize == sizeof(CMD_S_StatusFree));
											if (wDataSize != sizeof(CMD_S_StatusFree)) return false;

											//消息处理
											CMD_S_StatusFree * pStatusFree = (CMD_S_StatusFree *)pData;

											m_lUserLimitScore = pStatusFree->lUserMaxScore;
											m_lAreaLimitScore = pStatusFree->lAreaLimitScore;
											m_lBankerCondition = pStatusFree->lApplyBankerCondition;

											CopyMemory(m_RobotInfo.nChip, pStatusFree->lChipArray, sizeof(m_RobotInfo.nChip));

											m_bEnableBanker = pStatusFree->bEnableBanker;
											ReadConfigInformation(&pStatusFree->CustomAndroid);
											m_bEnableRobotOccupySeat = (pStatusFree->CustomAndroid.nEnableRobotOccupySeat == TRUE ? true : false);
											m_lOccupySeatFree = pStatusFree->lOccupySeatFree;
											CopyMemory(m_wOccupySeatChairIDArray, pStatusFree->wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

											//读取AIConfig
											CopyMemory(&m_CustomAIConfig, &(pStatusFree->CustomAndroid.CustomAIConfig), sizeof(m_CustomAIConfig));

											return true;
	}
	case GAME_SCENE_PLACE_JETTON:		//游戏状态
	case GAME_SCENE_GAME_END:			//结束状态
	{
											//效验数据
											ASSERT(wDataSize == sizeof(CMD_S_StatusPlay));
											if (wDataSize != sizeof(CMD_S_StatusPlay)) return false;

											//消息处理
											CMD_S_StatusPlay * pStatusPlay = (CMD_S_StatusPlay *)pData;

											m_wCurrentBanker = pStatusPlay->wBankerUser;
											m_lUserLimitScore = pStatusPlay->lUserMaxScore;
											m_lAreaLimitScore = pStatusPlay->lAreaLimitScore;
											m_lBankerCondition = pStatusPlay->lApplyBankerCondition;
											m_bEnableRobotOccupySeat = (pStatusPlay->CustomAndroid.nEnableRobotOccupySeat == TRUE ? true : false);
											m_lOccupySeatFree = pStatusPlay->lOccupySeatFree;
											CopyMemory(m_wOccupySeatChairIDArray, pStatusPlay->wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

											CopyMemory(m_RobotInfo.nChip, pStatusPlay->lChipArray, sizeof(m_RobotInfo.nChip));

											m_bEnableBanker = pStatusPlay->bEnableBanker;
											ReadConfigInformation(&pStatusPlay->CustomAndroid);

											//读取AIConfig
											CopyMemory(&m_CustomAIConfig, &(pStatusPlay->CustomAndroid.CustomAIConfig), sizeof(m_CustomAIConfig));

											return true;
	}
	}

	return true;
}

//游戏空闲
bool CAndroidUserItemSink::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//消息处理
	CMD_S_GameFree* pGameFree = (CMD_S_GameFree *)pBuffer;

	//获取机器人占位概率
	LONG lAIOccupySeatChange = GetRandAIOccupySeatChange();
	lAIOccupySeatChange = (lAIOccupySeatChange == INVALID_DWORD ? 4 : lAIOccupySeatChange);

	//获取随机数值
	LONG lRandVal = rand() % 100;

	//判断概率
	if (lRandVal < lAIOccupySeatChange && !m_pIAndroidUserItem->IsAiHanging())
	{
		//设置占位定时器(2秒后)
		SetGameTimerEx(IDI_REQUEST_OCCUPYSEAT, 2 + (rand() % 2));
	}

	//获取机器人上庄概率
	LONG lAIRequestBankerChange = GetRandAIRequestBankerChange();
	lAIRequestBankerChange = (lAIRequestBankerChange == INVALID_DWORD ? 4 : lAIRequestBankerChange);

	if (m_wCurrentBanker == m_pIAndroidUserItem->GetChairID())
	{
		//已上庄的机器人每局开始有3%申请下庄
		if (lRandVal < m_CustomAIConfig.AIRequestBankerConfig.wAIQBChange && !m_pIAndroidUserItem->IsAiHanging())
		{
			SetGameTimerEx(IDI_QUIT_BANKER, rand() % 2 + 1);

			m_tmAIQuitBanker = CTime::GetCurrentTime();
			m_bMeApplyBanker = false;
		}
	}
	else
	{
		//当前机器人在上庄列表
		if (IsInApplyUserArray())
		{
			//上庄列表包含真人
			if (IsApplyUserArrayContainRealPlayer())
			{
				if (lRandVal < m_CustomAIConfig.AIRequestBankerConfig.wAICRChangeContainRealPlayer && !m_pIAndroidUserItem->IsAiHanging())
				{
					SetGameTimerEx(IDI_QUIT_BANKER, rand() % 2 + 1);

					m_tmAICancelRequest = CTime::GetCurrentTime();
					m_bMeApplyBanker = false;
				}
			}
			//上庄列表不包含真人
			else
			{
				if (lRandVal < m_CustomAIConfig.AIRequestBankerConfig.wAICRChange && !m_pIAndroidUserItem->IsAiHanging())
				{
					SetGameTimerEx(IDI_QUIT_BANKER, rand() % 2 + 1);

					m_tmAICancelRequest = CTime::GetCurrentTime();
					m_bMeApplyBanker = false;
				}
			}
		}
		else
		{
			//判断概率
			if (lRandVal < lAIRequestBankerChange && !m_pIAndroidUserItem->IsAiHanging() && EfficacyRequestTm()
				&& m_bRobotBanker && !m_bMeApplyBanker)
			{
				//设置上庄定时器(2秒后)
				SetGameTimerEx(IDI_REQUEST_BANKER, 2 + (rand() % 2));

				ZeroMemory(&m_tmAICancelRequest, sizeof(m_tmAICancelRequest));
				ZeroMemory(&m_tmAIQuitBanker, sizeof(m_tmAIQuitBanker));
			}
		}
	}

	//机器人存取款
	if (m_CustomAIConfig.AIBankOperateConfig.wVerifyTurn <= 0)
	{
		m_CustomAIConfig.AIBankOperateConfig.wVerifyTurn = 5;
	}

	if ((m_lPlayTimeCount % (m_CustomAIConfig.AIBankOperateConfig.wVerifyTurn)) == 0 && !m_pIAndroidUserItem->IsAiHanging())
	{
		//获取机器人银行操作参数
		GetAIOperateBankPara(m_cbSave, m_wAIBankOperateChange, m_dAIBankOperateScore);

		if (lRandVal < m_wAIBankOperateChange)
		{
			SetGameTimerEx(IDI_BANK_OPERATE, 2 + (rand() % 2));
		}
	}

	//校验坐下最低成绩
	if ((m_pIAndroidUserItem->GetMeUserItem()->GetUserScore() < m_lMinTableScore))
	{
		SCORE dCarryScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
		SCORE	dAITakeScore = ((dCarryScore < (SCORE)0) ? (-dCarryScore + m_lMinTableScore) : (m_lMinTableScore - dCarryScore));

		m_pIAndroidUserItem->PerformTakeScore(dAITakeScore);
	}

	return true;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_GameStart));
	if (wDataSize != sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart = (CMD_S_GameStart *)pBuffer;

	srand(GetTickCount());

	//自己当庄或无下注机器人
	if (pGameStart->wBankerUser == m_pIAndroidUserItem->GetChairID() || pGameStart->nChipRobotCount <= 0)
	{
		return true;
	}

	//最大下注机器人数目
	if (pGameStart->nChipRobotCount <= 0)
	{
		return true;
	}

	//设置变量
	m_lMaxChipUser = pGameStart->lUserMaxScore / m_RobotInfo.nMaxTime;
	m_wCurrentBanker = pGameStart->wBankerUser;
	m_nChipTimeCount = 0;
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));

	//区域下注限制
	for (int i = 0; i < AREA_MAX; i++)
	{
		m_lAreaChip[i] = m_lAreaLimitScore;
	}

	//计算下注次数

	WORD wMyID = m_pIAndroidUserItem->GetChairID();

	if (m_nRobotBetTimeLimit[0] == m_nRobotBetTimeLimit[1])
		m_nChipTime = m_nRobotBetTimeLimit[0];
	else
		m_nChipTime = (rand() + wMyID) % (m_nRobotBetTimeLimit[1] - m_nRobotBetTimeLimit[0] + 1) + m_nRobotBetTimeLimit[0];
	ASSERT(m_nChipTime >= 0);
	if (m_nChipTime <= 0)	return false;								//的确,2个都带等于
	if (m_nChipTime > MAX_CHIP_TIME)	m_nChipTime = MAX_CHIP_TIME;	//限定MAX_CHIP次下注

	//计算范围
	if (!CalcJettonRange(m_lMaxChipUser, m_lRobotJettonLimit, m_nChipTime, m_nChipLimit))
	{
		CString stTrace;
		CTime Time(CTime::GetCurrentTime());
		stTrace.Format(TEXT("%d-%d %d:%d:%d |  AISubGameStart, m_lMaxChipUser=%I64d,JettonLimit0=%I64d,JettonLimit1=%I64d,m_nChipTime=%d,ChipLimit0=%d,ChipLimit1=%d,nChip0=%I64d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), 
			m_lMaxChipUser, m_lRobotJettonLimit[0], m_lRobotJettonLimit[1], m_nChipTime, m_nChipLimit[0], m_nChipLimit[1], m_RobotInfo.nChip[0]);
		//WriteInfo(TEXT("DragonTigerBattlePlace.log"), stTrace);

		return true;
	}

	//下注过滤挂机和庄家
	if (m_pIAndroidUserItem->IsAiHanging() || m_pIAndroidUserItem->GetChairID() == m_wCurrentBanker)
	{
		return true;
	}

	bool bMeOccupySeat = false;
	BYTE cbOccupySeatAICount = 0;
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		if (m_wOccupySeatChairIDArray[i] != INVALID_CHAIR && m_wOccupySeatChairIDArray[i] == m_pIAndroidUserItem->GetChairID())
		{
			bMeOccupySeat = true;
		}

		if (m_wOccupySeatChairIDArray[i] != INVALID_CHAIR && m_pIAndroidUserItem->GetTableUserItem(m_wOccupySeatChairIDArray[i]) &&
			m_pIAndroidUserItem->GetTableUserItem(m_wOccupySeatChairIDArray[i])->IsAndroidUser())
		{
			cbOccupySeatAICount++;
		}
	}

	bool bPlaceJetton = ((bMeOccupySeat || cbOccupySeatAICount <= 2) ? rand() % 100 < 100 : rand() % 100 < 70);

	if (bPlaceJetton)
	{
		//设置时间
		//m_nChipTime = min(m_nChipTime, 15);
		int nTimeZoom = 1000 / 200;
		int nTimeGrid = int(pGameStart->cbTimeLeave) * 800 / m_nChipTime;
		int nElapse = 0;
		int nOriginVal = 0;

		bool bValid = false;
		for (int i = 0; i < m_nChipTime; i++)
		{
			int nRandRage = int(nTimeGrid * i / (1500 * sqrt((double)m_nChipTime))) + 1;		//波动范围
			nElapse = nOriginVal + (nTimeGrid * i) / (1000 / nTimeZoom) + ((rand() + wMyID) % (nRandRage * 2) - (nRandRage - 1));
			ASSERT(nElapse >= nOriginVal && nElapse <= pGameStart->cbTimeLeave * nTimeZoom);
			nElapse += rand() % nTimeZoom;

			if (nElapse < nOriginVal || nElapse > pGameStart->cbTimeLeave * nTimeZoom)	{ continue; }

			bValid = true;
			m_pIAndroidUserItem->SetGameTimer(IDI_PLACE_JETTON + i + 1, nElapse);
		}

		if (bValid)
		{
			CString stTrace;
			CTime Time(CTime::GetCurrentTime());
			stTrace.Format(TEXT("%d-%d %d:%d:%d |  IDI_PLACE_JETTON, PlaceJettonRate0=%d, PlaceJettonRate1=%d, PlaceJettonRate2=%d, PlaceJettonRate3=%d, PlaceJettonRate4=%d, PlaceJettonRate5=%d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(),
				m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[0], m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[1], m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[2]
				, m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[3], m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[4], m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[5]);

			//WriteInfo(TEXT("DragonTigerBattlePlace.log"), stTrace);
		}
	}
	
	return true;
}

//用户加注
bool CAndroidUserItemSink::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_PlaceJetton));
	if (wDataSize != sizeof(CMD_S_PlaceJetton)) return false;

	//消息处理
	CMD_S_PlaceJetton * pPlaceJetton = (CMD_S_PlaceJetton *)pBuffer;

	//设置变量
	m_lAreaChip[pPlaceJetton->cbJettonArea] -= pPlaceJetton->lJettonScore;
	if (pPlaceJetton->wChairID == m_pIAndroidUserItem->GetChairID())
	{
		m_lMaxChipUser -= pPlaceJetton->lJettonScore;
		m_lCurUserInAllScore[pPlaceJetton->cbJettonArea] += pPlaceJetton->lJettonScore;
	}

	//bool bMeOccupySeat = false;
	//BYTE cbOccupySeatAICount = 0;
	//for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	//{
	//	if (m_wOccupySeatChairIDArray[i] != INVALID_CHAIR && m_wOccupySeatChairIDArray[i] == m_pIAndroidUserItem->GetChairID())
	//	{
	//		bMeOccupySeat = true;
	//	}

	//	if (m_wOccupySeatChairIDArray[i] != INVALID_CHAIR && m_pIAndroidUserItem->GetTableUserItem(m_wOccupySeatChairIDArray[i]) &&
	//		m_pIAndroidUserItem->GetTableUserItem(m_wOccupySeatChairIDArray[i])->IsAndroidUser())
	//	{
	//		cbOccupySeatAICount++;
	//	}
	//}

	//bool bPlaceJetton = ((bMeOccupySeat || cbOccupySeatAICount <= 2) ? rand() % 100 < 100 : rand() % 100 < 70);

	//SCORE lCurUserInAllScore = 0;
	//for (WORD i = 0; i < AREA_MAX; i++)
	//{
	//	lCurUserInAllScore += m_lCurUserInAllScore[i];
	//}
	//
	//if (D_LessThen(lCurUserInAllScore, m_RobotInfo.nChip[3]))
	//{
	//	AnalyseAddScore();
	//}

	return true;
}

//申请做庄
bool CAndroidUserItemSink::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
	if (!m_bEnableBanker)
	{
		return false;
	}

	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_ApplyBanker));
	if (wDataSize != sizeof(CMD_S_ApplyBanker)) return false;

	//消息处理
	CMD_S_ApplyBanker * pApplyBanker = (CMD_S_ApplyBanker *)pBuffer;

	//自己判断
	if (m_pIAndroidUserItem->GetChairID() == pApplyBanker->wApplyUser)
	{
		m_bMeApplyBanker = true;
	}

	//更新上庄列表
	m_ApplyUserArray.Add(pApplyBanker->wApplyUser);

	return true;
}

//取消做庄
bool CAndroidUserItemSink::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
	if (!m_bEnableBanker)
	{
		return false;
	}

	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_CancelBanker));
	if (wDataSize != sizeof(CMD_S_CancelBanker)) return false;

	//消息处理
	CMD_S_CancelBanker * pCancelBanker = (CMD_S_CancelBanker *)pBuffer;

	//自己判断
	if (m_pIAndroidUserItem->GetChairID() == pCancelBanker->wCancelUser)
	{
		m_bMeApplyBanker = false;
	}

	//存在判断
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
	{
		//获取玩家
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(wChairID);

		//条件过滤
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (pCancelBanker->wCancelUser == wChairID)
		{
			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			break;
		}
	}

	return true;
}

//切换庄家
bool CAndroidUserItemSink::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
	if (!m_bEnableBanker)
	{
		return false;
	}

	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_ChangeBanker));
	if (wDataSize != sizeof(CMD_S_ChangeBanker)) return false;

	//消息处理
	CMD_S_ChangeBanker * pChangeBanker = (CMD_S_ChangeBanker *)pBuffer;

	//当前切换为庄家
	if (m_pIAndroidUserItem->GetChairID() == pChangeBanker->wBankerUser)
	{
		m_pIAndroidUserItem->SetAndroidAction(ANDROID_BEBANKER, true);
	}
	//当前切换下庄
	else if (m_wCurrentBanker == m_pIAndroidUserItem->GetChairID() && m_wCurrentBanker != pChangeBanker->wBankerUser)
	{
		m_bMeApplyBanker = false;

		m_pIAndroidUserItem->SetAndroidAction(ANDROID_BEBANKER, false);
	}

	m_wCurrentBanker = pChangeBanker->wBankerUser;

	//存在判断
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
	{
		//获取玩家
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(wChairID);

		//条件过滤
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (pChangeBanker->wBankerUser == wChairID)
		{
			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			break;
		}
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_GameEnd));
	if (wDataSize != sizeof(CMD_S_GameEnd)) return false;

	//游戏局数累加
	m_lPlayTimeCount++;
	ZeroMemory(m_lCurUserInAllScore, sizeof(m_lCurUserInAllScore));

	return true;
}

//占位结果
bool CAndroidUserItemSink::OnSubOccupySeat(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_OccupySeat));
	if (wDataSize != sizeof(CMD_S_OccupySeat))
	{
		return false;
	}

	//消息处理
	CMD_S_OccupySeat * pOccupySeat = (CMD_S_OccupySeat *)pBuffer;

	//占位椅子ID
	CopyMemory(m_wOccupySeatChairIDArray, pOccupySeat->wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

	bool bMeOccupySeat = false;
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		if (m_wOccupySeatChairIDArray[i] != INVALID_CHAIR && m_wOccupySeatChairIDArray[i] == m_pIAndroidUserItem->GetChairID())
		{
			bMeOccupySeat = true;
		}
	}

	m_pIAndroidUserItem->SetAndroidAction(ANDROID_OCCUPYSEAT, bMeOccupySeat);

	return true;
}

//占位失败
bool CAndroidUserItemSink::OnSubOccupySeatFail(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_OccupySeat_Fail));
	if (wDataSize != sizeof(CMD_S_OccupySeat_Fail))
	{
		return false;
	}

	//消息处理
	CMD_S_OccupySeat_Fail * pOccupySeat_Fail = (CMD_S_OccupySeat_Fail *)pBuffer;

	//占位椅子ID
	CopyMemory(m_wOccupySeatChairIDArray, pOccupySeat_Fail->wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

	bool bMeOccupySeat = false;
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		if (m_wOccupySeatChairIDArray[i] != INVALID_CHAIR && m_wOccupySeatChairIDArray[i] == m_pIAndroidUserItem->GetChairID())
		{
			bMeOccupySeat = true;
		}
	}

	m_pIAndroidUserItem->SetAndroidAction(ANDROID_OCCUPYSEAT, bMeOccupySeat);

	return true;
}

//更新占位
bool CAndroidUserItemSink::OnSubUpdateOccupySeat(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_UpdateOccupySeat));
	if (wDataSize != sizeof(CMD_S_UpdateOccupySeat))
	{
		return false;
	}

	//消息处理
	CMD_S_UpdateOccupySeat * pUpdateOccupySeat = (CMD_S_UpdateOccupySeat *)pBuffer;

	//占位椅子ID
	CopyMemory(m_wOccupySeatChairIDArray, pUpdateOccupySeat->wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

	bool bMeOccupySeat = false;
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		if (m_wOccupySeatChairIDArray[i] != INVALID_CHAIR && m_wOccupySeatChairIDArray[i] == m_pIAndroidUserItem->GetChairID())
		{
			bMeOccupySeat = true;
		}
	}

	m_pIAndroidUserItem->SetAndroidAction(ANDROID_OCCUPYSEAT, bMeOccupySeat);

	return true;
}

//占位起立(连续三局没有押注)
bool CAndroidUserItemSink::OnSubOccupySeatStandUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_OccupySeatStandUp));
	if (wDataSize != sizeof(CMD_S_OccupySeatStandUp))
	{
		return false;
	}

	//消息处理
	CMD_S_OccupySeatStandUp * pOccupySeatStandUp = (CMD_S_OccupySeatStandUp *)pBuffer;

	//占位椅子ID
	CopyMemory(m_wOccupySeatChairIDArray, pOccupySeatStandUp->wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

	return true;
}

bool CAndroidUserItemSink::OnSubFreeAIConfig(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_GameFreeAIConfig));
	if (wDataSize != sizeof(CMD_S_GameFreeAIConfig))
	{
		return false;
	}

	//消息处理
	CMD_S_GameFreeAIConfig * pGameFreeAIConfig = (CMD_S_GameFreeAIConfig *)pBuffer;

	//读取AIConfig
	CopyMemory(&m_CustomAIConfig, &(pGameFreeAIConfig->CustomAIConfig), sizeof(m_CustomAIConfig));

	return true;
}

bool CAndroidUserItemSink::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_PlaceJettonFail));
	if (wDataSize != sizeof(CMD_S_PlaceJettonFail))
	{
		return false;
	}

	//消息处理
	CMD_S_PlaceJettonFail * pPlaceJettonFail = (CMD_S_PlaceJettonFail *)pBuffer;

	//下注过滤挂机和庄家
	if (m_pIAndroidUserItem->IsAiHanging() || m_pIAndroidUserItem->GetChairID() == m_wCurrentBanker
	 || m_pIAndroidUserItem->GetChairID() != pPlaceJettonFail->wPlaceUser)
	{
		return true;
	}

	CMD_C_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton, sizeof(PlaceJetton));

	//构造变量
	PlaceJetton.cbJettonArea = (pPlaceJettonFail->lJettonArea <= AREA_PING ? AREA_HU : AREA_LONG);
	PlaceJetton.lJettonScore = pPlaceJettonFail->lPlaceScore;

	//发送消息
	m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));

	return true;
}

//读取配置
void CAndroidUserItemSink::ReadConfigInformation(tagCustomAndroid *pCustomAndroid)
{
	//次数限制
	m_nRobotBetTimeLimit[0] = pCustomAndroid->lRobotMinBetTime;
	m_nRobotBetTimeLimit[1] = pCustomAndroid->lRobotMaxBetTime;

	if (m_nRobotBetTimeLimit[0] < 0) m_nRobotBetTimeLimit[0] = 0;
	if (m_nRobotBetTimeLimit[1] < m_nRobotBetTimeLimit[0]) m_nRobotBetTimeLimit[1] = m_nRobotBetTimeLimit[0];
	if (m_nRobotBetTimeLimit[0] == m_nRobotBetTimeLimit[1] && m_nRobotBetTimeLimit[0] == 0)
	{
		m_nRobotBetTimeLimit[0] = 4;
		m_nRobotBetTimeLimit[1] = 8;
	}

	//筹码限制
	m_lRobotJettonLimit[0] = m_RobotInfo.nChip[0];
	m_lRobotJettonLimit[1] = m_RobotInfo.nChip[MAX_CHIP_COUNT - 1];

	//是否坐庄
	m_bRobotBanker = true;
	m_lMinTableScore = pCustomAndroid->dMinTableScore;

	return;
}

//计算范围	(返回值表示是否可以通过下降下限达到下注)
bool CAndroidUserItemSink::CalcJettonRange(SCORE lMaxScore, SCORE lChipLmt[], int & nChipTime, int lJetLmt[])
{
	//定义变量
	bool bHaveSetMinChip = false;

	//不够一注
	if (lMaxScore < m_RobotInfo.nChip[0])	return false;

	//配置范围
	for (int i = 0; i < CountArray(m_RobotInfo.nChip); i++)
	{
		if (!bHaveSetMinChip && m_RobotInfo.nChip[i] >= lChipLmt[0])
		{
			lJetLmt[0] = i;
			bHaveSetMinChip = true;
		}
		if (m_RobotInfo.nChip[i] <= lChipLmt[1])
			lJetLmt[1] = i;
	}
	if (lJetLmt[0] > lJetLmt[1])	lJetLmt[0] = lJetLmt[1];

	return true;
}

//获取机器人占位概率
LONG CAndroidUserItemSink::GetRandAIOccupySeatChange()
{
	//获取真人玩家个数
	LONG lRealPlayerCount = m_pIAndroidUserItem->GetSitRealUserCount();

	//数据大小校验
	DWORD lTempArray[4] = { m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[2][1]
	};

	//获取范围
	if (lRealPlayerCount > lTempArray[0])
	{
		return m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatChange[0];
	}
	else if (lRealPlayerCount > lTempArray[2] && lRealPlayerCount <= lTempArray[1])
	{
		return m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatChange[1];
	}
	else if (lRealPlayerCount >= 0 && lRealPlayerCount <= lTempArray[3])
	{
		return m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatChange[2];
	}

	return INVALID_DWORD;
}

//获取机器人上庄概率
LONG CAndroidUserItemSink::GetRandAIRequestBankerChange()
{
	//获取真人玩家个数
	LONG lRealPlayerCount = m_pIAndroidUserItem->GetSitRealUserCount();

	//数据大小校验
	DWORD lTempArray[4] = { m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[2][1]
	};

	//获取范围
	if (lRealPlayerCount > lTempArray[0])
	{
		return m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerChange[0];
	}
	else if (lRealPlayerCount > lTempArray[2] && lRealPlayerCount <= lTempArray[1])
	{
		return m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerChange[1];
	}
	else if (lRealPlayerCount >= 0 && lRealPlayerCount <= lTempArray[3])
	{
		return m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerChange[2];
	}

	return INVALID_DWORD;
}

//上庄列表是否包含真人
bool CAndroidUserItemSink::IsApplyUserArrayContainRealPlayer()
{
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		//获取玩家
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(wChairID);

		//条件过滤
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (!pIServerUserItem->IsAndroidUser())
		{
			return true;
		}
	}

	return false;
}

//当前机器人是否在上庄列表
bool CAndroidUserItemSink::IsInApplyUserArray()
{
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		//获取玩家
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(wChairID);

		//条件过滤
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (wChairID == m_pIAndroidUserItem->GetChairID())
		{
			return true;
		}
	}

	return false;
}

//校验申请上庄时间
bool CAndroidUserItemSink::EfficacyRequestTm()
{
	CTime tmCur = CTime::GetCurrentTime();
	CTimeSpan tmSpanCR = tmCur - m_tmAICancelRequest;
	CTimeSpan tmSpanQB = tmCur - m_tmAIQuitBanker;

	if (m_tmAICancelRequest.GetTime() != 0 && tmSpanCR.GetTotalSeconds() < (m_CustomAIConfig.AIRequestBankerConfig.wAICRChangeFreeMin * 60))
	{
		return false;
	}

	if (m_tmAIQuitBanker.GetTime() != 0 && tmSpanQB.GetTotalSeconds() < (m_CustomAIConfig.AIRequestBankerConfig.wAIQBChangeFreeMin * 60))
	{
		return false;
	}

	return true;
}

//获取机器人银行操作参数
void CAndroidUserItemSink::GetAIOperateBankPara(BYTE &cbSave, DWORD &wAIBankOperateChange, SCORE &dAIBankOperateScore)
{
	SCORE dMaxJetton = m_RobotInfo.nChip[MAX_CHIP_COUNT - 1];
	SCORE dMinJetton = m_RobotInfo.nChip[0];
	SCORE dCarryScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();

	bool bOperateBankValid = false;

	for (int i = AI_BANKOPERATE_CONFIGCOUNT - 1; i >= 0; i--)
	{
		if (i >= 4)
		{
			if (dCarryScore > m_CustomAIConfig.AIBankOperateConfig.lAICarryScoreRange[i][0] * dMaxJetton
				&& dCarryScore <= m_CustomAIConfig.AIBankOperateConfig.lAICarryScoreRange[i][1] * dMaxJetton)
			{
				cbSave = TRUE;
				wAIBankOperateChange = m_CustomAIConfig.AIBankOperateConfig.wAIBankOperateChange[i];

				LONGLONG lOperateScore = RANDVAL(m_CustomAIConfig.AIBankOperateConfig.lAIBankOperateScoreRange[i][0], m_CustomAIConfig.AIBankOperateConfig.lAIBankOperateScoreRange[i][1]);

				dAIBankOperateScore = lOperateScore * dMinJetton;
				bOperateBankValid = true;

				break;
			}
		}
		else
		{
			if (dCarryScore > m_CustomAIConfig.AIBankOperateConfig.lAICarryScoreRange[i][0] * dMinJetton
				&& dCarryScore <= m_CustomAIConfig.AIBankOperateConfig.lAICarryScoreRange[i][1] * dMinJetton && i < 4)
			{
				cbSave = FALSE;
				wAIBankOperateChange = m_CustomAIConfig.AIBankOperateConfig.wAIBankOperateChange[i];

				LONGLONG lOperateScore = RANDVAL(m_CustomAIConfig.AIBankOperateConfig.lAIBankOperateScoreRange[i][0], m_CustomAIConfig.AIBankOperateConfig.lAIBankOperateScoreRange[i][1]);

				dAIBankOperateScore = lOperateScore * dMinJetton;
				bOperateBankValid = true;

				break;
			}
		}
	}

	//不满足任何条件
	if (!bOperateBankValid)
	{
		cbSave = FALSE;
		wAIBankOperateChange = m_CustomAIConfig.AIBankOperateConfig.wAIBankOperateChange[0];

		LONGLONG lOperateScore = RANDVAL(m_CustomAIConfig.AIBankOperateConfig.lAIBankOperateScoreRange[0][0], m_CustomAIConfig.AIBankOperateConfig.lAIBankOperateScoreRange[0][1]);

		dAIBankOperateScore = lOperateScore * dMinJetton;
	}
}

//启动定时器
void CAndroidUserItemSink::SetGameTimerEx(UINT nTimerID, UINT nElapse)
{
	m_pIAndroidUserItem->SetGameTimer(nTimerID, nElapse * 1000 / 200);
}

//下注分析
VOID CAndroidUserItemSink::AnalyseAddScore()
{
	//变量定义
	int nRandNum = 0, nChipArea = 0, nCurChip = 0, nACTotal = 0, nCurJetLmt[2] = {};
	SCORE lMaxChipLmt = m_lMaxChipUser;			//最大可下注值
	WORD wMyID = m_pIAndroidUserItem->GetChairID();
	for (int i = 0; i < AREA_MAX; i++)
		nACTotal += m_RobotInfo.nAreaChance[i];

	//统计次数
	m_nChipTimeCount++;

	//检测退出
	if (lMaxChipLmt < m_RobotInfo.nChip[m_nChipLimit[0]])
	{
		//CString strInfo;
		//strInfo.Format(TEXT("m_nChipLimit[0] = %d, lMaxChipLmt=%.2f, nChip=%.2f"), m_nChipLimit[0], lMaxChipLmt, m_RobotInfo.nChip[m_nChipLimit[0]]);
		//CTraceService::TraceString(strInfo, TraceLevel_Exception);

		return;
	}

	//下注区域
	ASSERT(nACTotal>0);
	static int nStFluc = 1;				//随机辅助
	if (nACTotal <= 0)
	{
		return;
	}

	bool bAreaAttempArray[AREA_MAX];
	ZeroMemory(bAreaAttempArray, sizeof(bAreaAttempArray));

	do
	{
		BYTE cbAreaAttempCount = 0;
		for (int i = 0; i < AREA_MAX; i++)
		{
			if (bAreaAttempArray[i])
			{
				cbAreaAttempCount++;
			}
		}

		//全部区域都不满足下注条件
		if (cbAreaAttempCount == AREA_MAX)
		{
			return;
		}

		nRandNum = (rand() + wMyID + nStFluc * 3) % nACTotal;
		for (int i = 0; i < AREA_MAX; i++)
		{
			nRandNum -= m_RobotInfo.nAreaChance[i];
			if (nRandNum < 0)
			{
				nChipArea = i;
				bAreaAttempArray[nChipArea] = true;

				break;
			}
		}
	} while (m_lAreaChip[nChipArea] < m_RobotInfo.nChip[m_nChipLimit[0]]);
	nStFluc = nStFluc % 3 + 1;

	//下注大小
	if (m_nChipLimit[0] == m_nChipLimit[1])
		nCurChip = m_nChipLimit[0];
	else
	{
		int nPJRateTotal = 0;

		for (WORD i = 0; i < MAX_CHIP_COUNT; i++)
		{
			nPJRateTotal += m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[i];
		}

		//默认随机筹码下注
		if (nPJRateTotal == 0)
		{
			//设置变量
			lMaxChipLmt = min(lMaxChipLmt, m_lAreaChip[nChipArea]);
			nCurJetLmt[0] = m_nChipLimit[0];
			nCurJetLmt[1] = m_nChipLimit[0];

			//计算当前最大筹码
			for (int i = m_nChipLimit[1]; i >= m_nChipLimit[0]; i--)
			{
				if (lMaxChipLmt > m_RobotInfo.nChip[i])
				{
					nCurJetLmt[1] = i;
					break;
				}
			}

			//随机下注
			nRandNum = (rand() + wMyID) % (nCurJetLmt[1] - nCurJetLmt[0] + 1);
			nCurChip = nCurJetLmt[0] + nRandNum;
		}
		//筹码权重下注
		else
		{
			//计算当前最大筹码
			int nCurMaxChip = 0;
			for (int i = m_nChipLimit[1]; i >= m_nChipLimit[0]; i--)
			{
				if (lMaxChipLmt > m_RobotInfo.nChip[i])
				{
					nCurMaxChip = i;
					break;
				}
			}

			static int nPJStFluc = 1;				//随机辅助
			nRandNum = (rand() + wMyID + nPJStFluc * 3) % nPJRateTotal;
			for (int i = 0; i < MAX_CHIP_COUNT; i++)
			{
				nRandNum -= m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[i];
				if (nRandNum < 0)
				{
					nCurChip = i;
					break;
				}
			}

			nPJStFluc = nPJStFluc % 3 + 1;

			//nCurChip = min(nCurChip, nCurMaxChip);

			if (nCurChip > nCurMaxChip)
			{
				bool bValid = false;
				for (WORD i = 0; i <= nCurMaxChip; i++)
				{
					if (m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[i] > 0)
					{
						nCurChip = i;
						bValid = true;

						break;
					}
				}

				if (!bValid)
				{
					return;
				}
			}
		}

		//多下控制 (当庄家金币较少时会尽量保证下足次数)
		//if (m_nChipTimeCount < m_nChipTime)
		//{
		//	SCORE lLeftJetton =  (lMaxChipLmt-m_RobotInfo.nChip[nCurChip])/(m_nChipTime-m_nChipTimeCount);

		//	//不够次数 (即全用最小限制筹码下注也少了)
		//	if (D_LessThen(lLeftJetton , m_RobotInfo.nChip[m_nChipLimit[0]]) && nCurChip > m_nChipLimit[0])
		//		nCurChip--;
		//}
	}

	//变量定义
	CMD_C_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton, sizeof(PlaceJetton));

	//如果龙虎区域差距大于倍数*最小筹码
	if ((abs(m_lAreaChip[AREA_LONG] - m_lAreaChip[AREA_HU]) > (m_CustomAIConfig.AIPlaceJettonConfig.lAIWinLostLimitTimes * m_RobotInfo.nChip[0])) && nChipArea != AREA_PING)
	{
		if ((m_lAreaChip[AREA_LONG] > m_lAreaChip[AREA_HU]) && nChipArea != AREA_LONG)
		{
			nChipArea = AREA_LONG;
		}
		else if ((m_lAreaChip[AREA_LONG] < m_lAreaChip[AREA_HU]) && nChipArea != AREA_HU)
		{
			nChipArea = AREA_HU;
		}
	}

	//构造变量
	PlaceJetton.cbJettonArea = nChipArea;

	//不满足条件下最小
	if (!(nCurChip >= 0 && nCurChip < MAX_CHIP_COUNT))
	{
		nCurChip = 0;
	}

	PlaceJetton.lJettonScore = m_RobotInfo.nChip[nCurChip];

	//校验是否满足个人限制
	SCORE lCurUserInAllScore = 0;
	for (WORD i = 0; i < AREA_MAX; i++)
	{
		lCurUserInAllScore += m_lCurUserInAllScore[i];
	}

	if (lCurUserInAllScore + PlaceJetton.lJettonScore > m_lUserLimitScore)
	{
		//CString strInfo;
		//strInfo.Format(TEXT("lCurUserInAllScore=%I64d, m_lUserLimitScore=%I64d"), lCurUserInAllScore, m_lUserLimitScore);
		//CTraceService::TraceString(strInfo, TraceLevel_Exception);

		return;
	}

	//发送消息
	m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));
}

//测试写信息
void CAndroidUserItemSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
	//设置语言区域
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
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

	//还原区域设定
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}
//////////////////////////////////////////////////////////////////////////
//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);