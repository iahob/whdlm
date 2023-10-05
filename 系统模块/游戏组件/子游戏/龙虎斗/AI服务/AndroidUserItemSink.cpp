#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_REQUEST_OCCUPYSEAT		100									//����ռλ
#define IDI_REQUEST_BANKER			101									//������ׯ
#define IDI_QUIT_BANKER				102									//��ׯ
#define IDI_BANK_OPERATE			103									//���ж�ʱ
#define IDI_PLACE_JETTON			104									//��ע���� (Ԥ��110-160)

//�����ע����
#define MAX_CHIP_TIME				50

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//ռλ
	m_bEnableRobotOccupySeat = true;
	m_lOccupySeatFree = 1000000;
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;
	}

	//��ׯ����
	m_bEnableBanker = true;
	m_lBankerCondition = 10000000;
	m_wCurrentBanker = INVALID_CHAIR;
	m_bRobotBanker = true;
	m_bMeApplyBanker = false;
	ZeroMemory(&m_tmAICancelRequest, sizeof(m_tmAICancelRequest));
	ZeroMemory(&m_tmAIQuitBanker, sizeof(m_tmAIQuitBanker));
	m_ApplyUserArray.RemoveAll();

	//�����˴�ȡ��
	m_lPlayTimeCount = 0;
	m_cbSave = FALSE;
	m_wAIBankOperateChange = 0;
	m_dAIBankOperateScore = 0;

	//��Ϸ����
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

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
void *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink, Guid, dwQueryVer);
	return NULL;
}

//��ʼ�ӿ�
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIAndroidUserItem = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IAndroidUserItem);
	if (m_pIAndroidUserItem == NULL) return false;

	return true;
}

//���ýӿ�
bool  CAndroidUserItemSink::RepositionSink()
{
	m_ApplyUserArray.RemoveAll();
	ZeroMemory(&m_tmAICancelRequest, sizeof(m_tmAICancelRequest));
	ZeroMemory(&m_tmAIQuitBanker, sizeof(m_tmAIQuitBanker));

	//��Ϸ����
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

//ʱ����Ϣ
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_REQUEST_OCCUPYSEAT:
	{
								   m_pIAndroidUserItem->KillGameTimer(nTimerID);

								   //�жϹһ�
								   //if (m_pIAndroidUserItem->IsAiHanging())
								   //{
									  // return false;
								   //}

								   //��֧�ֻ�����ռλ
								   if (!m_bEnableRobotOccupySeat)
								   {
									   return false;
								   }

								   //�ж�ռλ����
								   IServerUserItem *pIUserItem = m_pIAndroidUserItem->GetMeUserItem();
								   if (pIUserItem->GetUserScore() < m_lOccupySeatFree)
								   {
									   return false;
								   }

								   //�ж��Ѿ�ռλ
								   for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
								   {
									   //ռλʧ��
									   if (m_wOccupySeatChairIDArray[i] == pIUserItem->GetChairID())
									   {
										   return false;
									   }
								   }

								   //��Чռλ����
								   BYTE cbValidIndexArray[MAX_OCCUPY_SEAT_COUNT];
								   memset(cbValidIndexArray, INVALID_BYTE, sizeof(cbValidIndexArray));
								   BYTE cbValidIndexCount = 0;

								   for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
								   {
									   //ռλ
									   if (m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
									   {
										   cbValidIndexArray[cbValidIndexCount++] = i;
									   }
								   }

								   if (cbValidIndexCount > 0)
								   {
									   //����ռλ��Ϣ
									   CMD_C_OccupySeat OccupySeat;
									   ZeroMemory(&OccupySeat, sizeof(OccupySeat));

									   OccupySeat.wOccupySeatChairID = pIUserItem->GetChairID();
									   OccupySeat.cbOccupySeatIndex = cbValidIndexArray[rand() % cbValidIndexCount];

									   ASSERT(OccupySeat.cbOccupySeatIndex >= SEAT_LEFT1_INDEX && OccupySeat.cbOccupySeatIndex <= SEAT_RIGHT3_INDEX);
									   if (OccupySeat.cbOccupySeatIndex >= SEAT_LEFT1_INDEX && OccupySeat.cbOccupySeatIndex <= SEAT_RIGHT3_INDEX)
									   {
										   //������Ϣ
										   m_pIAndroidUserItem->SendSocketData(SUB_C_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));
									   }
								   }

								   return false;
	}
	case IDI_REQUEST_BANKER:	//������ׯ
	{
									if (!m_bEnableBanker)
									{
										return false;
									}

									m_pIAndroidUserItem->KillGameTimer(nTimerID);
									m_pIAndroidUserItem->SendSocketData(SUB_C_APPLY_BANKER);

									return false;
	}
	case IDI_QUIT_BANKER:		//������ׯ
	{
									if (!m_bEnableBanker)
									{
										return false;
									}

									m_pIAndroidUserItem->KillGameTimer(nTimerID);
									m_pIAndroidUserItem->SendSocketData(SUB_C_CANCEL_BANKER);

									return false;
	}
	case IDI_BANK_OPERATE:		//���в���
	{
									m_pIAndroidUserItem->KillGameTimer(nTimerID);

									//�жϴ�ȡ
									if (m_cbSave == TRUE)
									{
										m_pIAndroidUserItem->PerformSaveScore(m_dAIBankOperateScore);
									}
									else if (m_cbSave == FALSE)
									{
										m_pIAndroidUserItem->PerformTakeScore(m_dAIBankOperateScore);
									}

									//У��������ͳɼ�
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

				   //��ע����
				   AnalyseAddScore();

				   m_pIAndroidUserItem->KillGameTimer(nTimerID);

				   return false;
			   }
	}
	}
	return false;
}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_FREE:			//��Ϸ���� 
	{
		return OnSubGameFree(pBuffer, wDataSize);
	}
	case SUB_S_GAME_START:			//��Ϸ��ʼ
	{
		return OnSubGameStart(pBuffer, wDataSize);
	}
	case SUB_S_PLACE_JETTON:		//�û���ע
	{
		return OnSubPlaceJetton(pBuffer, wDataSize);
	}
	case SUB_S_APPLY_BANKER:		//������ׯ 
	{
		return OnSubUserApplyBanker(pBuffer, wDataSize);
	}
	case SUB_S_CANCEL_BANKER:		//ȡ����ׯ 
	{
		return OnSubUserCancelBanker(pBuffer, wDataSize);
	}
	case SUB_S_CHANGE_BANKER:		//�л�ׯ�� 
	{
		return OnSubChangeBanker(pBuffer, wDataSize);
	}
	case SUB_S_GAME_END:			//��Ϸ���� 
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

//������Ϣ
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:				//����״̬
	{
											//Ч������
											ASSERT(wDataSize == sizeof(CMD_S_StatusFree));
											if (wDataSize != sizeof(CMD_S_StatusFree)) return false;

											//��Ϣ����
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

											//��ȡAIConfig
											CopyMemory(&m_CustomAIConfig, &(pStatusFree->CustomAndroid.CustomAIConfig), sizeof(m_CustomAIConfig));

											return true;
	}
	case GAME_SCENE_PLACE_JETTON:		//��Ϸ״̬
	case GAME_SCENE_GAME_END:			//����״̬
	{
											//Ч������
											ASSERT(wDataSize == sizeof(CMD_S_StatusPlay));
											if (wDataSize != sizeof(CMD_S_StatusPlay)) return false;

											//��Ϣ����
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

											//��ȡAIConfig
											CopyMemory(&m_CustomAIConfig, &(pStatusPlay->CustomAndroid.CustomAIConfig), sizeof(m_CustomAIConfig));

											return true;
	}
	}

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//��Ϣ����
	CMD_S_GameFree* pGameFree = (CMD_S_GameFree *)pBuffer;

	//��ȡ������ռλ����
	LONG lAIOccupySeatChange = GetRandAIOccupySeatChange();
	lAIOccupySeatChange = (lAIOccupySeatChange == INVALID_DWORD ? 4 : lAIOccupySeatChange);

	//��ȡ�����ֵ
	LONG lRandVal = rand() % 100;

	//�жϸ���
	if (lRandVal < lAIOccupySeatChange && !m_pIAndroidUserItem->IsAiHanging())
	{
		//����ռλ��ʱ��(2���)
		SetGameTimerEx(IDI_REQUEST_OCCUPYSEAT, 2 + (rand() % 2));
	}

	//��ȡ��������ׯ����
	LONG lAIRequestBankerChange = GetRandAIRequestBankerChange();
	lAIRequestBankerChange = (lAIRequestBankerChange == INVALID_DWORD ? 4 : lAIRequestBankerChange);

	if (m_wCurrentBanker == m_pIAndroidUserItem->GetChairID())
	{
		//����ׯ�Ļ�����ÿ�ֿ�ʼ��3%������ׯ
		if (lRandVal < m_CustomAIConfig.AIRequestBankerConfig.wAIQBChange && !m_pIAndroidUserItem->IsAiHanging())
		{
			SetGameTimerEx(IDI_QUIT_BANKER, rand() % 2 + 1);

			m_tmAIQuitBanker = CTime::GetCurrentTime();
			m_bMeApplyBanker = false;
		}
	}
	else
	{
		//��ǰ����������ׯ�б�
		if (IsInApplyUserArray())
		{
			//��ׯ�б��������
			if (IsApplyUserArrayContainRealPlayer())
			{
				if (lRandVal < m_CustomAIConfig.AIRequestBankerConfig.wAICRChangeContainRealPlayer && !m_pIAndroidUserItem->IsAiHanging())
				{
					SetGameTimerEx(IDI_QUIT_BANKER, rand() % 2 + 1);

					m_tmAICancelRequest = CTime::GetCurrentTime();
					m_bMeApplyBanker = false;
				}
			}
			//��ׯ�б���������
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
			//�жϸ���
			if (lRandVal < lAIRequestBankerChange && !m_pIAndroidUserItem->IsAiHanging() && EfficacyRequestTm()
				&& m_bRobotBanker && !m_bMeApplyBanker)
			{
				//������ׯ��ʱ��(2���)
				SetGameTimerEx(IDI_REQUEST_BANKER, 2 + (rand() % 2));

				ZeroMemory(&m_tmAICancelRequest, sizeof(m_tmAICancelRequest));
				ZeroMemory(&m_tmAIQuitBanker, sizeof(m_tmAIQuitBanker));
			}
		}
	}

	//�����˴�ȡ��
	if (m_CustomAIConfig.AIBankOperateConfig.wVerifyTurn <= 0)
	{
		m_CustomAIConfig.AIBankOperateConfig.wVerifyTurn = 5;
	}

	if ((m_lPlayTimeCount % (m_CustomAIConfig.AIBankOperateConfig.wVerifyTurn)) == 0 && !m_pIAndroidUserItem->IsAiHanging())
	{
		//��ȡ���������в�������
		GetAIOperateBankPara(m_cbSave, m_wAIBankOperateChange, m_dAIBankOperateScore);

		if (lRandVal < m_wAIBankOperateChange)
		{
			SetGameTimerEx(IDI_BANK_OPERATE, 2 + (rand() % 2));
		}
	}

	//У��������ͳɼ�
	if ((m_pIAndroidUserItem->GetMeUserItem()->GetUserScore() < m_lMinTableScore))
	{
		SCORE dCarryScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
		SCORE	dAITakeScore = ((dCarryScore < (SCORE)0) ? (-dCarryScore + m_lMinTableScore) : (m_lMinTableScore - dCarryScore));

		m_pIAndroidUserItem->PerformTakeScore(dAITakeScore);
	}

	return true;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_GameStart));
	if (wDataSize != sizeof(CMD_S_GameStart)) return false;

	//��Ϣ����
	CMD_S_GameStart * pGameStart = (CMD_S_GameStart *)pBuffer;

	srand(GetTickCount());

	//�Լ���ׯ������ע������
	if (pGameStart->wBankerUser == m_pIAndroidUserItem->GetChairID() || pGameStart->nChipRobotCount <= 0)
	{
		return true;
	}

	//�����ע��������Ŀ
	if (pGameStart->nChipRobotCount <= 0)
	{
		return true;
	}

	//���ñ���
	m_lMaxChipUser = pGameStart->lUserMaxScore / m_RobotInfo.nMaxTime;
	m_wCurrentBanker = pGameStart->wBankerUser;
	m_nChipTimeCount = 0;
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));

	//������ע����
	for (int i = 0; i < AREA_MAX; i++)
	{
		m_lAreaChip[i] = m_lAreaLimitScore;
	}

	//������ע����

	WORD wMyID = m_pIAndroidUserItem->GetChairID();

	if (m_nRobotBetTimeLimit[0] == m_nRobotBetTimeLimit[1])
		m_nChipTime = m_nRobotBetTimeLimit[0];
	else
		m_nChipTime = (rand() + wMyID) % (m_nRobotBetTimeLimit[1] - m_nRobotBetTimeLimit[0] + 1) + m_nRobotBetTimeLimit[0];
	ASSERT(m_nChipTime >= 0);
	if (m_nChipTime <= 0)	return false;								//��ȷ,2����������
	if (m_nChipTime > MAX_CHIP_TIME)	m_nChipTime = MAX_CHIP_TIME;	//�޶�MAX_CHIP����ע

	//���㷶Χ
	if (!CalcJettonRange(m_lMaxChipUser, m_lRobotJettonLimit, m_nChipTime, m_nChipLimit))
	{
		CString stTrace;
		CTime Time(CTime::GetCurrentTime());
		stTrace.Format(TEXT("%d-%d %d:%d:%d |  AISubGameStart, m_lMaxChipUser=%I64d,JettonLimit0=%I64d,JettonLimit1=%I64d,m_nChipTime=%d,ChipLimit0=%d,ChipLimit1=%d,nChip0=%I64d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), 
			m_lMaxChipUser, m_lRobotJettonLimit[0], m_lRobotJettonLimit[1], m_nChipTime, m_nChipLimit[0], m_nChipLimit[1], m_RobotInfo.nChip[0]);
		//WriteInfo(TEXT("DragonTigerBattlePlace.log"), stTrace);

		return true;
	}

	//��ע���˹һ���ׯ��
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
		//����ʱ��
		//m_nChipTime = min(m_nChipTime, 15);
		int nTimeZoom = 1000 / 200;
		int nTimeGrid = int(pGameStart->cbTimeLeave) * 800 / m_nChipTime;
		int nElapse = 0;
		int nOriginVal = 0;

		bool bValid = false;
		for (int i = 0; i < m_nChipTime; i++)
		{
			int nRandRage = int(nTimeGrid * i / (1500 * sqrt((double)m_nChipTime))) + 1;		//������Χ
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

//�û���ע
bool CAndroidUserItemSink::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_PlaceJetton));
	if (wDataSize != sizeof(CMD_S_PlaceJetton)) return false;

	//��Ϣ����
	CMD_S_PlaceJetton * pPlaceJetton = (CMD_S_PlaceJetton *)pBuffer;

	//���ñ���
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

//������ׯ
bool CAndroidUserItemSink::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
	if (!m_bEnableBanker)
	{
		return false;
	}

	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_ApplyBanker));
	if (wDataSize != sizeof(CMD_S_ApplyBanker)) return false;

	//��Ϣ����
	CMD_S_ApplyBanker * pApplyBanker = (CMD_S_ApplyBanker *)pBuffer;

	//�Լ��ж�
	if (m_pIAndroidUserItem->GetChairID() == pApplyBanker->wApplyUser)
	{
		m_bMeApplyBanker = true;
	}

	//������ׯ�б�
	m_ApplyUserArray.Add(pApplyBanker->wApplyUser);

	return true;
}

//ȡ����ׯ
bool CAndroidUserItemSink::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
	if (!m_bEnableBanker)
	{
		return false;
	}

	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_CancelBanker));
	if (wDataSize != sizeof(CMD_S_CancelBanker)) return false;

	//��Ϣ����
	CMD_S_CancelBanker * pCancelBanker = (CMD_S_CancelBanker *)pBuffer;

	//�Լ��ж�
	if (m_pIAndroidUserItem->GetChairID() == pCancelBanker->wCancelUser)
	{
		m_bMeApplyBanker = false;
	}

	//�����ж�
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
	{
		//��ȡ���
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(wChairID);

		//��������
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (pCancelBanker->wCancelUser == wChairID)
		{
			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			break;
		}
	}

	return true;
}

//�л�ׯ��
bool CAndroidUserItemSink::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
	if (!m_bEnableBanker)
	{
		return false;
	}

	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_ChangeBanker));
	if (wDataSize != sizeof(CMD_S_ChangeBanker)) return false;

	//��Ϣ����
	CMD_S_ChangeBanker * pChangeBanker = (CMD_S_ChangeBanker *)pBuffer;

	//��ǰ�л�Ϊׯ��
	if (m_pIAndroidUserItem->GetChairID() == pChangeBanker->wBankerUser)
	{
		m_pIAndroidUserItem->SetAndroidAction(ANDROID_BEBANKER, true);
	}
	//��ǰ�л���ׯ
	else if (m_wCurrentBanker == m_pIAndroidUserItem->GetChairID() && m_wCurrentBanker != pChangeBanker->wBankerUser)
	{
		m_bMeApplyBanker = false;

		m_pIAndroidUserItem->SetAndroidAction(ANDROID_BEBANKER, false);
	}

	m_wCurrentBanker = pChangeBanker->wBankerUser;

	//�����ж�
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
	{
		//��ȡ���
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(wChairID);

		//��������
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (pChangeBanker->wBankerUser == wChairID)
		{
			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			break;
		}
	}

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_GameEnd));
	if (wDataSize != sizeof(CMD_S_GameEnd)) return false;

	//��Ϸ�����ۼ�
	m_lPlayTimeCount++;
	ZeroMemory(m_lCurUserInAllScore, sizeof(m_lCurUserInAllScore));

	return true;
}

//ռλ���
bool CAndroidUserItemSink::OnSubOccupySeat(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_OccupySeat));
	if (wDataSize != sizeof(CMD_S_OccupySeat))
	{
		return false;
	}

	//��Ϣ����
	CMD_S_OccupySeat * pOccupySeat = (CMD_S_OccupySeat *)pBuffer;

	//ռλ����ID
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

//ռλʧ��
bool CAndroidUserItemSink::OnSubOccupySeatFail(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_OccupySeat_Fail));
	if (wDataSize != sizeof(CMD_S_OccupySeat_Fail))
	{
		return false;
	}

	//��Ϣ����
	CMD_S_OccupySeat_Fail * pOccupySeat_Fail = (CMD_S_OccupySeat_Fail *)pBuffer;

	//ռλ����ID
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

//����ռλ
bool CAndroidUserItemSink::OnSubUpdateOccupySeat(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_UpdateOccupySeat));
	if (wDataSize != sizeof(CMD_S_UpdateOccupySeat))
	{
		return false;
	}

	//��Ϣ����
	CMD_S_UpdateOccupySeat * pUpdateOccupySeat = (CMD_S_UpdateOccupySeat *)pBuffer;

	//ռλ����ID
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

//ռλ����(��������û��Ѻע)
bool CAndroidUserItemSink::OnSubOccupySeatStandUp(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_OccupySeatStandUp));
	if (wDataSize != sizeof(CMD_S_OccupySeatStandUp))
	{
		return false;
	}

	//��Ϣ����
	CMD_S_OccupySeatStandUp * pOccupySeatStandUp = (CMD_S_OccupySeatStandUp *)pBuffer;

	//ռλ����ID
	CopyMemory(m_wOccupySeatChairIDArray, pOccupySeatStandUp->wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

	return true;
}

bool CAndroidUserItemSink::OnSubFreeAIConfig(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_GameFreeAIConfig));
	if (wDataSize != sizeof(CMD_S_GameFreeAIConfig))
	{
		return false;
	}

	//��Ϣ����
	CMD_S_GameFreeAIConfig * pGameFreeAIConfig = (CMD_S_GameFreeAIConfig *)pBuffer;

	//��ȡAIConfig
	CopyMemory(&m_CustomAIConfig, &(pGameFreeAIConfig->CustomAIConfig), sizeof(m_CustomAIConfig));

	return true;
}

bool CAndroidUserItemSink::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_PlaceJettonFail));
	if (wDataSize != sizeof(CMD_S_PlaceJettonFail))
	{
		return false;
	}

	//��Ϣ����
	CMD_S_PlaceJettonFail * pPlaceJettonFail = (CMD_S_PlaceJettonFail *)pBuffer;

	//��ע���˹һ���ׯ��
	if (m_pIAndroidUserItem->IsAiHanging() || m_pIAndroidUserItem->GetChairID() == m_wCurrentBanker
	 || m_pIAndroidUserItem->GetChairID() != pPlaceJettonFail->wPlaceUser)
	{
		return true;
	}

	CMD_C_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton, sizeof(PlaceJetton));

	//�������
	PlaceJetton.cbJettonArea = (pPlaceJettonFail->lJettonArea <= AREA_PING ? AREA_HU : AREA_LONG);
	PlaceJetton.lJettonScore = pPlaceJettonFail->lPlaceScore;

	//������Ϣ
	m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));

	return true;
}

//��ȡ����
void CAndroidUserItemSink::ReadConfigInformation(tagCustomAndroid *pCustomAndroid)
{
	//��������
	m_nRobotBetTimeLimit[0] = pCustomAndroid->lRobotMinBetTime;
	m_nRobotBetTimeLimit[1] = pCustomAndroid->lRobotMaxBetTime;

	if (m_nRobotBetTimeLimit[0] < 0) m_nRobotBetTimeLimit[0] = 0;
	if (m_nRobotBetTimeLimit[1] < m_nRobotBetTimeLimit[0]) m_nRobotBetTimeLimit[1] = m_nRobotBetTimeLimit[0];
	if (m_nRobotBetTimeLimit[0] == m_nRobotBetTimeLimit[1] && m_nRobotBetTimeLimit[0] == 0)
	{
		m_nRobotBetTimeLimit[0] = 4;
		m_nRobotBetTimeLimit[1] = 8;
	}

	//��������
	m_lRobotJettonLimit[0] = m_RobotInfo.nChip[0];
	m_lRobotJettonLimit[1] = m_RobotInfo.nChip[MAX_CHIP_COUNT - 1];

	//�Ƿ���ׯ
	m_bRobotBanker = true;
	m_lMinTableScore = pCustomAndroid->dMinTableScore;

	return;
}

//���㷶Χ	(����ֵ��ʾ�Ƿ����ͨ���½����޴ﵽ��ע)
bool CAndroidUserItemSink::CalcJettonRange(SCORE lMaxScore, SCORE lChipLmt[], int & nChipTime, int lJetLmt[])
{
	//�������
	bool bHaveSetMinChip = false;

	//����һע
	if (lMaxScore < m_RobotInfo.nChip[0])	return false;

	//���÷�Χ
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

//��ȡ������ռλ����
LONG CAndroidUserItemSink::GetRandAIOccupySeatChange()
{
	//��ȡ������Ҹ���
	LONG lRealPlayerCount = m_pIAndroidUserItem->GetSitRealUserCount();

	//���ݴ�СУ��
	DWORD lTempArray[4] = { m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[2][1]
	};

	//��ȡ��Χ
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

//��ȡ��������ׯ����
LONG CAndroidUserItemSink::GetRandAIRequestBankerChange()
{
	//��ȡ������Ҹ���
	LONG lRealPlayerCount = m_pIAndroidUserItem->GetSitRealUserCount();

	//���ݴ�СУ��
	DWORD lTempArray[4] = { m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[2][1]
	};

	//��ȡ��Χ
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

//��ׯ�б��Ƿ��������
bool CAndroidUserItemSink::IsApplyUserArrayContainRealPlayer()
{
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		//��ȡ���
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(wChairID);

		//��������
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

//��ǰ�������Ƿ�����ׯ�б�
bool CAndroidUserItemSink::IsInApplyUserArray()
{
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		//��ȡ���
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(wChairID);

		//��������
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

//У��������ׯʱ��
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

//��ȡ���������в�������
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

	//�������κ�����
	if (!bOperateBankValid)
	{
		cbSave = FALSE;
		wAIBankOperateChange = m_CustomAIConfig.AIBankOperateConfig.wAIBankOperateChange[0];

		LONGLONG lOperateScore = RANDVAL(m_CustomAIConfig.AIBankOperateConfig.lAIBankOperateScoreRange[0][0], m_CustomAIConfig.AIBankOperateConfig.lAIBankOperateScoreRange[0][1]);

		dAIBankOperateScore = lOperateScore * dMinJetton;
	}
}

//������ʱ��
void CAndroidUserItemSink::SetGameTimerEx(UINT nTimerID, UINT nElapse)
{
	m_pIAndroidUserItem->SetGameTimer(nTimerID, nElapse * 1000 / 200);
}

//��ע����
VOID CAndroidUserItemSink::AnalyseAddScore()
{
	//��������
	int nRandNum = 0, nChipArea = 0, nCurChip = 0, nACTotal = 0, nCurJetLmt[2] = {};
	SCORE lMaxChipLmt = m_lMaxChipUser;			//������עֵ
	WORD wMyID = m_pIAndroidUserItem->GetChairID();
	for (int i = 0; i < AREA_MAX; i++)
		nACTotal += m_RobotInfo.nAreaChance[i];

	//ͳ�ƴ���
	m_nChipTimeCount++;

	//����˳�
	if (lMaxChipLmt < m_RobotInfo.nChip[m_nChipLimit[0]])
	{
		//CString strInfo;
		//strInfo.Format(TEXT("m_nChipLimit[0] = %d, lMaxChipLmt=%.2f, nChip=%.2f"), m_nChipLimit[0], lMaxChipLmt, m_RobotInfo.nChip[m_nChipLimit[0]]);
		//CTraceService::TraceString(strInfo, TraceLevel_Exception);

		return;
	}

	//��ע����
	ASSERT(nACTotal>0);
	static int nStFluc = 1;				//�������
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

		//ȫ�����򶼲�������ע����
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

	//��ע��С
	if (m_nChipLimit[0] == m_nChipLimit[1])
		nCurChip = m_nChipLimit[0];
	else
	{
		int nPJRateTotal = 0;

		for (WORD i = 0; i < MAX_CHIP_COUNT; i++)
		{
			nPJRateTotal += m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[i];
		}

		//Ĭ�����������ע
		if (nPJRateTotal == 0)
		{
			//���ñ���
			lMaxChipLmt = min(lMaxChipLmt, m_lAreaChip[nChipArea]);
			nCurJetLmt[0] = m_nChipLimit[0];
			nCurJetLmt[1] = m_nChipLimit[0];

			//���㵱ǰ������
			for (int i = m_nChipLimit[1]; i >= m_nChipLimit[0]; i--)
			{
				if (lMaxChipLmt > m_RobotInfo.nChip[i])
				{
					nCurJetLmt[1] = i;
					break;
				}
			}

			//�����ע
			nRandNum = (rand() + wMyID) % (nCurJetLmt[1] - nCurJetLmt[0] + 1);
			nCurChip = nCurJetLmt[0] + nRandNum;
		}
		//����Ȩ����ע
		else
		{
			//���㵱ǰ������
			int nCurMaxChip = 0;
			for (int i = m_nChipLimit[1]; i >= m_nChipLimit[0]; i--)
			{
				if (lMaxChipLmt > m_RobotInfo.nChip[i])
				{
					nCurMaxChip = i;
					break;
				}
			}

			static int nPJStFluc = 1;				//�������
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

		//���¿��� (��ׯ�ҽ�ҽ���ʱ�ᾡ����֤�������)
		//if (m_nChipTimeCount < m_nChipTime)
		//{
		//	SCORE lLeftJetton =  (lMaxChipLmt-m_RobotInfo.nChip[nCurChip])/(m_nChipTime-m_nChipTimeCount);

		//	//�������� (��ȫ����С���Ƴ�����עҲ����)
		//	if (D_LessThen(lLeftJetton , m_RobotInfo.nChip[m_nChipLimit[0]]) && nCurChip > m_nChipLimit[0])
		//		nCurChip--;
		//}
	}

	//��������
	CMD_C_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton, sizeof(PlaceJetton));

	//���������������ڱ���*��С����
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

	//�������
	PlaceJetton.cbJettonArea = nChipArea;

	//��������������С
	if (!(nCurChip >= 0 && nCurChip < MAX_CHIP_COUNT))
	{
		nCurChip = 0;
	}

	PlaceJetton.lJettonScore = m_RobotInfo.nChip[nCurChip];

	//У���Ƿ������������
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

	//������Ϣ
	m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));
}

//����д��Ϣ
void CAndroidUserItemSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
	//������������
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

	//��ԭ�����趨
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}
//////////////////////////////////////////////////////////////////////////
//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);