#include "StdAfx.h"
#include "TableFrameSink.h"
#include "GameServerManager.h"
#include <locale>
#include "CLog.h"

#define IPC_PACKAGE				8192
//////////////////////////////////////////////////////////////////////////

#define IDI_CLEAR_RECORD        1

//ȫ�ֱ���
SCOREEX                         g_lRevenue = 0;                                         //ϵͳ�ۼ�˰��

CGameControl		CTableFrameSink::m_gameStorage;

static CString GetFileDialogPath()
{
	CString strFileDlgPath;
	TCHAR szModuleDirectory[MAX_PATH];	//ģ��Ŀ¼
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

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//�������
	srand(time(NULL));

	//�������
	m_pITableFrame = NULL;
	m_pGameServiceOption = NULL;
	m_pGameServiceAttrib = NULL;
	m_pGameCustomRule = NULL;

	//��Ϸ����
	//��ʼ������
	ZeroMemory(&m_Bonus, sizeof(m_Bonus));
	m_Bonus.lBonus5K = rand() % 100 + 100;
	m_Bonus.lBonusRS = rand() % 300 + 300;
	m_Bonus.lBonusSF = rand() % 500 + 500;
	m_Bonus.lBonus4K = rand() % 1000 + 1000;

	//��ʼ����¼
	ZeroMemory(&m_recordInfo, sizeof(m_recordInfo));
	m_lbureauCount = 0;
	m_lBet = 0;
	ZeroMemory(m_lBetRadio, sizeof(m_lBetRadio));
	m_balreadySwitch = false;
	m_bGuess = false;
	m_wCurrentGuessCount = 0;
	m_bLuckyTimePause = false;
	m_bAuto = false;
	ZeroMemory(m_guessRecord, sizeof(m_guessRecord));
	ZeroMemory(m_bSwitchFlag, sizeof(m_bSwitchFlag));
	ZeroMemory(m_cbGuessCardResultRecord, sizeof(m_cbGuessCardResultRecord));

	//�˿˱���
	ZeroMemory(m_cbFirstCardData, sizeof(m_cbFirstCardData));
	ZeroMemory(m_cbSecondCardData, sizeof(m_cbSecondCardData));
	ZeroMemory(m_cbSwitchCardData, sizeof(m_cbSwitchCardData));
	ZeroMemory(m_cbLuckyCardData, sizeof(m_cbLuckyCardData));
	m_GussIndex = 255;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{

}

//�ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(const IID &Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);

	return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx != NULL);

	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
	if (m_pITableFrame == NULL)
	{
		return false;
	}

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//��ȡ����
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
	ASSERT(m_pGameServiceOption != NULL && m_pGameServiceAttrib != NULL);

	m_pGameCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();
	ASSERT(m_pGameCustomRule != NULL);

	CopyMemory(m_lBetItem, m_pGameCustomRule->lBet, sizeof(m_lBetItem));

	//��ʼ��¼ʱ��
	m_tmStartRecord = CTime::GetCurrentTime();

	//��־
	CString strDir = GetFileDialogPath() + TEXT("\\") + GAME_NAME + TEXT("��־");
	CreateDirectory(strDir, NULL);
	strDir.AppendFormat(TEXT("\\%s(%d)"), GAME_NAME, m_pITableFrame->GetGameServiceOption()->wServerID);
	EasyLog::CLog::m_log.SetName(strDir.GetBuffer());
	LOG_DEBUG(m_pITableFrame->GetTableID(), TEXT("���ӳ�ʼ��!"));

	//ϵͳ����ʼ��
	InitSystemStorage();
	return true;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	//��Ϸ����
	m_lBet = 0;
	ZeroMemory(m_lBetRadio, sizeof(m_lBetRadio));
	m_balreadySwitch = false;
	m_bGuess = false;
	m_wCurrentGuessCount = 0;
	m_bLuckyTimePause = false;
	m_bAuto = false;
	ZeroMemory(m_bSwitchFlag, sizeof(m_bSwitchFlag));
	ZeroMemory(m_guessRecord, sizeof(m_guessRecord));


	//�˿˱���
	ZeroMemory(m_cbFirstCardData, sizeof(m_cbFirstCardData));
	ZeroMemory(m_cbSecondCardData, sizeof(m_cbSecondCardData));
	ZeroMemory(m_cbSwitchCardData, sizeof(m_cbSwitchCardData));
	ZeroMemory(m_cbLuckyCardData, sizeof(m_cbLuckyCardData));
}

//��ѯ�޶�
SCOREEX CTableFrameSink::QueryConsumeQuota(IServerUserItem *pIServerUserItem)
{
	if (pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		return 0L;
	}
	else
	{
		return __max(pIServerUserItem->GetUserScore() - m_pGameServiceOption->lMinTableScore, 0L);
	}
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//����״̬
	m_pITableFrame->SetGameStatus(GS_GAME_PLAY);

	while (true)
	{
		//��������
		DispatchCardData();

		if (m_GameLogic.IsContainKingCard(m_cbFirstCardData, MAX_CARD_COUNT) || m_GameLogic.IsContainKingCard(m_cbSecondCardData, MAX_CARD_COUNT))
		{
			if (rand() % 1000 < 5)
			{
				break;
			}
		}
		else if (!m_GameLogic.IsContainKingCard(m_cbFirstCardData, MAX_CARD_COUNT) && !m_GameLogic.IsContainKingCard(m_cbSecondCardData, MAX_CARD_COUNT))
		{
			break;
		}
	}

	//ϵͳ��ߺ���͵÷�
	SCOREEX lSysMaxScore = 0LL;
	SCOREEX lSysMinScore = 0LL;
	IServerUserItem * pIServerUserItem = NULL;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL)
			break;

	}

	bool bUserWin = DebugWinLose(pIServerUserItem);
	//�н�����Ȩ��
	LONGLONG lCurMinTimes = -1, lCurMaxTimes = -1;
	m_gameStorage.m_gameWieght.GetRandTimes(lCurMinTimes, lCurMaxTimes);
	//��ȡ������
	SCOREEX  dSysStorage = 0, dPlayerStorage = 0;
	m_gameStorage.GetCurStorage(dSysStorage, dPlayerStorage, pIServerUserItem->GetGameID());
	ASSERT(GAME_PLAYER == 1);

	LOG_DEBUG(L"�������:", bUserWin ? L"Ӯ" : L"��", L"��С����:", lCurMinTimes, L"�����:", lCurMaxTimes);

	int nCount = 0;
	BYTE	cbMaxCardData[MAX_CARD_COUNT];
	ZeroMemory(cbMaxCardData, sizeof(cbMaxCardData));

	while (TRUE)
	{
		nCount++;

		//��������
		INT nEnumCardCount = m_GameLogic.EnumCardDataCount(m_cbFirstCardData, m_cbSecondCardData);
		ENUMCARDTYPE *pEnumCardType = new ENUMCARDTYPE[nEnumCardCount];
		ZeroMemory(pEnumCardType, sizeof(ENUMCARDTYPE)* nEnumCardCount);

		//��ȡö���˿�
		m_GameLogic.GetEnumCardData(pEnumCardType, nEnumCardCount);

		//ϵͳ��ߺ���͵÷�
		lSysMaxScore = 0LL;
		lSysMinScore = 0LL;

		//����ϵͳ�÷�
		LONG lTotalOpenRatio = CalculateScore(pEnumCardType, nEnumCardCount, lSysMaxScore, lSysMinScore, cbMaxCardData);

		//�����ڴ�
		delete[] pEnumCardType;

#define MAX_CIRCOUNT	500
		if (nCount >= MAX_CIRCOUNT)
		{
			LOG_DEBUG(TEXT("����������������"));
			break;
		}

		if (m_GameLogic.GetCardType(m_cbFirstCardData, MAX_CARD_COUNT) != CT_INVALID
			|| m_GameLogic.GetCardType(m_cbSecondCardData, MAX_CARD_COUNT) != CT_INVALID)
		{
			if (rand() % 100 > 15)
			{
				DispatchCardData();
				continue;
			}
		}
		if (!bUserWin)
		{
			if (lSysMinScore >= 0)
			{
				break;
			}
			else
			{
				DispatchCardData();
				continue;
			}
		}
		else
		{
			//��ҿ�����
			if (D_GreaterThen(-lSysMinScore, dPlayerStorage))
				continue;
			//��������
			if (lTotalOpenRatio < lCurMinTimes || lTotalOpenRatio > lCurMaxTimes)
				continue;

			if (lSysMinScore <= 0)
			{
				break;
			}
			else
			{
				DispatchCardData();
				continue;
			}
		}
	}

	//����������֤�׸����ǿ��ƽ��
	ArrangeControlCards(m_cbFirstCardData, m_cbSecondCardData, cbMaxCardData);

	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));
	CopyMemory(GameStart.cbFirstCardArray, m_cbFirstCardData, sizeof(m_cbFirstCardData));
	CopyMemory(GameStart.cbSecondCardArray, m_cbSecondCardData, sizeof(m_cbSecondCardData));

	//��������
	for (INT i = 0; i<GAME_PLAYER; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pServerUserItem)
		{
			continue;
		}

		m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
		m_pITableFrame->SendLookonData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
	}

	LOG_DEBUG(L"��һ���˿�����:", (int)m_GameLogic.GetCardType(m_cbFirstCardData, MAX_CARD_COUNT), L"�ڶ����˿����ͣ�", (int)m_GameLogic.GetCardType(m_cbSecondCardData, MAX_CARD_COUNT));
	return true;
}

//��Ϸ����
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������	
	{
								//���ü�¼
								m_recordInfo.lPlayTotal++;
								CMD_S_GameConclude GameConclude;
								ZeroMemory(&GameConclude, sizeof(GameConclude));

								bool bHeldFlag[MAX_CARD_COUNT];
								ZeroMemory(bHeldFlag, sizeof(bHeldFlag));
								//���ͱ�ʶ
								bool bMark[MAX_CARD_CT];
								ZeroMemory(bMark, sizeof(bMark));
								m_GameLogic.AnalyseHeldCard((m_cbSwitchCardData[0] == 0 ? m_cbFirstCardData : m_cbSwitchCardData), MAX_CARD_COUNT, bHeldFlag, bMark);

								BYTE cbCardType = CT_INVALID;
								//��ȡ����
								cbCardType = m_GameLogic.GetCardType((m_cbSwitchCardData[0] == 0 ? m_cbFirstCardData : m_cbSwitchCardData), MAX_CARD_COUNT);

								//���ֱ���
								tagScoreInfo ScoreInfoArray[GAME_PLAYER];
								ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

								//��������
								m_lbureauCount++;

								//ʤ��
								if (cbCardType != CT_INVALID)
								{
									//ͳ�ƻ���
									for (WORD i = 0; i<GAME_PLAYER; i++)
									{
										IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
										if (!pIServerUserItem)
										{
											continue;
										}

										for (INT j = 0; j<MAX_CARD_CT; j++)
										{
											if (!bMark[j])
											{
												continue;
											}

											ScoreInfoArray[i].lScore += GetCardTypeRadio(j) * (m_pGameCustomRule->lExchangeRadio);

											//4K���ͽ�����Ӧ���ذٷ�֮80
											if (j == CT_4K)
											{
												LONG lbonus4k = (m_Bonus.lBonus4K) * 80 / 100;
												ScoreInfoArray[i].lScore += (lbonus4k * m_pGameCustomRule->lExchangeRadio);
												m_Bonus.lBonus4K -= lbonus4k;
											}
											else if (j == CT_SF)
											{
												LONG lbonussf = (m_Bonus.lBonusSF) * 80 / 100;
												ScoreInfoArray[i].lScore += (lbonussf * m_pGameCustomRule->lExchangeRadio);
												m_Bonus.lBonusSF -= lbonussf;
											}
											else if (j == CT_RS)
											{
												LONG lbonusrs = (m_Bonus.lBonusRS) * 80 / 100;
												ScoreInfoArray[i].lScore += (lbonusrs * m_pGameCustomRule->lExchangeRadio);
												m_Bonus.lBonusRS -= lbonusrs;
											}
											else if (j == CT_5K)
											{
												LONG lbonus5k = (m_Bonus.lBonus5K) * 80 / 100;
												ScoreInfoArray[i].lScore += (lbonus5k * m_pGameCustomRule->lExchangeRadio);
												m_Bonus.lBonus5K -= lbonus5k;
											}

											UpdateRecord(j);

											break;
										}
									}

									ASSERT(ScoreInfoArray[wChairID].lScore > 0);

									//ѹ��С����
									if (m_bGuess)
									{
										for (INT i = 0; i<MAX_GUESS_COUNT; i++)
										{
											if (m_guessRecord[i].bValid  && !m_guessRecord[i].bGuessRight)
											{
												ScoreInfoArray[wChairID].lScore = -(m_lBet * (m_pGameCustomRule->lExchangeRadio));
												break;
											}
											else if (m_guessRecord[i].bValid  && m_guessRecord[i].bGuessRight)
											{
												ScoreInfoArray[wChairID].lScore *= 2;
											}
										}
									}

									//����˰��
									if (ScoreInfoArray[wChairID].lScore > 0)
									{
										ScoreInfoArray[wChairID].cbType = SCORE_TYPE_WIN;
										ScoreInfoArray[wChairID].lRevenue = m_pITableFrame->CalculateRevenue(wChairID, ScoreInfoArray[wChairID].lScore);
										if (ScoreInfoArray[wChairID].lRevenue > 0)
										{
											ScoreInfoArray[wChairID].lScore -= ScoreInfoArray[wChairID].lRevenue;
										}

									}
									else
									{
										ScoreInfoArray[wChairID].cbType = SCORE_TYPE_LOSE;
									}
								}
								else
								{
									ScoreInfoArray[wChairID].lScore = -(m_lBet * (m_pGameCustomRule->lExchangeRadio));
									ScoreInfoArray[wChairID].cbType = SCORE_TYPE_LOSE;

									//���ص�����ÿ���ֵ�����
									if (m_lbureauCount % 3 == 0)
									{
										IncreaseBonus();
									}

									m_recordInfo.lInvalidBet++;
								}

								if (cbCardType != CT_INVALID)
								{
									if (ScoreInfoArray[wChairID].cbType == SCORE_TYPE_WIN)
									{
										GameConclude.lUserScore = ScoreInfoArray[wChairID].lScore + m_lBet * m_pGameCustomRule->lExchangeRadio;
									}
									else if (ScoreInfoArray[wChairID].cbType == SCORE_TYPE_LOSE)
									{
										GameConclude.lUserScore = ScoreInfoArray[wChairID].lScore;
									}
								}
								else
								{
									GameConclude.lUserScore = -(m_lBet * (m_pGameCustomRule->lExchangeRadio));
								}

								GameConclude.lbureauCount = m_lbureauCount;
								GameConclude.Bonus = m_Bonus;
								GameConclude.bGuess = m_bGuess;
								CopyMemory(&(GameConclude.recordInfo), &m_recordInfo, sizeof(m_recordInfo));
								GameConclude.lExchangeRadio = m_pGameCustomRule->lExchangeRadio;
								GameConclude.llastBet = m_lBet;

								//��ˮ
								//ScoreInfoArray[wChairID].lChipSerialScore = abs(ScoreInfoArray[wChairID].lScore);
								//����ˢ��
								IServerUserItem* pCurUserItem = nullptr;
								pCurUserItem = m_pITableFrame->GetTableUserItem(wChairID);
								m_gameStorage.m_gameStatisticsInfo.lTotalServiceTax += ScoreInfoArray[wChairID].lRevenue;
								m_gameStorage.LineGameUpdateStorage(m_lBet * m_pGameCustomRule->lExchangeRadio, GameConclude.lUserScore, pCurUserItem == nullptr ? -1 : pCurUserItem->GetGameID());
								m_gameStorage.RefreshEvent(m_pITableFrame, nullptr);

								//д�����
								m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));

								//��������
								m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
								m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

								//������Ϸ
								m_pITableFrame->ConcludeGame(GS_GAME_FREE);

								m_GussIndex = 255;
								return true;
	}
	case GER_DISMISS:				//��Ϸ��ɢ
	{
										//��������
										m_lbureauCount++;
										//���ü�¼
										m_recordInfo.lPlayTotal++;
										//���ص�����ÿ���ֵ�����
										if (m_lbureauCount % 3 == 0)
										{
											IncreaseBonus();
										}

										//���ֱ���
										tagScoreInfo ScoreInfoArray[GAME_PLAYER];
										ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

										ScoreInfoArray[wChairID].lScore = -(m_lBet * (m_pGameCustomRule->lExchangeRadio));
										ScoreInfoArray[wChairID].cbType = SCORE_TYPE_NULL;

										CMD_S_GameConclude GameConclude;
										ZeroMemory(&GameConclude, sizeof(GameConclude));
										GameConclude.lUserScore = ScoreInfoArray[wChairID].lScore;
										GameConclude.lbureauCount = m_lbureauCount;
										GameConclude.Bonus = m_Bonus;
										GameConclude.lExchangeRadio = m_pGameCustomRule->lExchangeRadio;
										GameConclude.llastBet = m_lBet;

										//��������
										m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
										m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

										//д�����
										m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));

										//������Ϸ
										m_pITableFrame->ConcludeGame(GS_GAME_FREE);

										return true;
	}
	case GER_NETWORK_ERROR:
	case GER_USER_LEAVE:		//�û��뿪
	{
									//��������
									m_lbureauCount++;
									//���ü�¼
									m_recordInfo.lPlayTotal++;
									//���ص�����ÿ���ֵ�����
									if (m_lbureauCount % 3 == 0)
									{
										IncreaseBonus();
									}

									//���ֱ���
									tagScoreInfo ScoreInfoArray[GAME_PLAYER];
									ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

									ScoreInfoArray[wChairID].lScore = -(m_lBet * (m_pGameCustomRule->lExchangeRadio));
									ScoreInfoArray[wChairID].cbType = SCORE_TYPE_FLEE;

									CMD_S_GameConclude GameConclude;
									ZeroMemory(&GameConclude, sizeof(GameConclude));
									GameConclude.lUserScore = ScoreInfoArray[wChairID].lScore;
									GameConclude.lbureauCount = m_lbureauCount;
									GameConclude.Bonus = m_Bonus;
									GameConclude.lExchangeRadio = m_pGameCustomRule->lExchangeRadio;
									GameConclude.llastBet = m_lBet;

									//��������
									m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
									m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

									//д�����
									m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));

									//������Ϸ
									m_pITableFrame->ConcludeGame(GS_GAME_FREE);

									return true;
	}
		ASSERT(false);
	}

	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_GAME_FREE:			//����״̬
	{
									CMD_S_StatusGameFree StatusGameFree;
									ZeroMemory(&StatusGameFree, sizeof(StatusGameFree));

									StatusGameFree.lExchangeRadio = m_pGameCustomRule->lExchangeRadio;
									StatusGameFree.bonus = m_Bonus;
									StatusGameFree.lbureauCount = m_lbureauCount;
									CopyMemory(&(StatusGameFree.recordInfo), &m_recordInfo, sizeof(m_recordInfo));

									StatusGameFree.wTableID = m_pITableFrame->GetTableID() + 1;

									for (int i = 0; i < 10; i++)
									{
										StatusGameFree.lBetItem[i] = m_lBetItem[i];
									}
									StatusGameFree.lBet = m_lBet;
									CopyMemory(StatusGameFree.szServerName, m_pGameServiceOption->szServerName, sizeof(StatusGameFree.szServerName));

									//���ͳ���
									return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusGameFree, sizeof(StatusGameFree));
	}
	case GS_GAME_PLAY:			//��Ϸ״̬
	{
									CMD_S_StatusGamePlay StatusGamePlay;
									ZeroMemory(&StatusGamePlay, sizeof(StatusGamePlay));

									StatusGamePlay.bonus = m_Bonus;
									StatusGamePlay.lbureauCount = m_lbureauCount;
									StatusGamePlay.lExchangeRadio = m_pGameCustomRule->lExchangeRadio;
									for (int i = 0; i < 10; i++)
									{
										StatusGamePlay.lBetItem[i] = m_lBetItem[i];
									}

									StatusGamePlay.balreadySwitch = m_balreadySwitch;
									StatusGamePlay.bAuto = m_bAuto;
									CopyMemory(StatusGamePlay.bSwitchFlag, m_bSwitchFlag, sizeof(m_bSwitchFlag));
									CopyMemory(&(StatusGamePlay.recordInfo), &m_recordInfo, sizeof(m_recordInfo));
									StatusGamePlay.wTableID = m_pITableFrame->GetTableID() + 1;
									CopyMemory(StatusGamePlay.szServerName, m_pGameServiceOption->szServerName, sizeof(StatusGamePlay.szServerName));

									//�����˿�
									CopyMemory(StatusGamePlay.cbFirstCardData, m_cbFirstCardData, sizeof(m_cbFirstCardData));
									CopyMemory(StatusGamePlay.cbSecondCardData, m_cbSecondCardData, sizeof(m_cbSecondCardData));
									CopyMemory(StatusGamePlay.cbSwitchCardData, m_cbSwitchCardData, sizeof(m_cbSwitchCardData));

									StatusGamePlay.lBet = m_lBet;

									//���ͳ���
									return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusGamePlay, sizeof(StatusGamePlay));
	}
	case GS_GAME_LUCKYTIME:			//LUCKYTIME״̬
	{
										CMD_S_StatusLuckyTime StatusLuckyTime;
										ZeroMemory(&StatusLuckyTime, sizeof(StatusLuckyTime));

										StatusLuckyTime.bonus = m_Bonus;
										StatusLuckyTime.lBet = m_lBet;
										StatusLuckyTime.lbureauCount = m_lbureauCount;
										StatusLuckyTime.lExchangeRadio = m_pGameCustomRule->lExchangeRadio;
										StatusLuckyTime.wCurrentGuessCount = m_wCurrentGuessCount;
										StatusLuckyTime.bLuckyTimePause = m_bLuckyTimePause;
										CopyMemory(&(StatusLuckyTime.recordInfo), &m_recordInfo, sizeof(m_recordInfo));
										CopyMemory(StatusLuckyTime.cbGuessCardResultRecord, m_cbGuessCardResultRecord, sizeof(m_cbGuessCardResultRecord));
										CopyMemory(StatusLuckyTime.guessRecord, m_guessRecord, sizeof(m_guessRecord));
										CopyMemory(StatusLuckyTime.cbLuckyCardData, m_cbLuckyCardData, sizeof(m_cbLuckyCardData));
										CopyMemory(StatusLuckyTime.cbHandCardData, (m_cbSwitchCardData[0] == 0 ? m_cbFirstCardData : m_cbSwitchCardData), sizeof(m_cbSwitchCardData));

										StatusLuckyTime.wTableID = m_pITableFrame->GetTableID() + 1;
										CopyMemory(StatusLuckyTime.szServerName, m_pGameServiceOption->szServerName, sizeof(StatusLuckyTime.szServerName));

										for (int i = 0; i < 10; i++)
										{
											StatusLuckyTime.lBetItem[i] = m_lBetItem[i];
										}

										//���ͳ���
										return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusLuckyTime, sizeof(StatusLuckyTime));
	}
	}

	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_CLEAR_RECORD:
	{
							 DWORD dWuserID = wBindParam;
							 CStringW textUserID;
							 textUserID.Format(L"%d", dWuserID);

							 for (int i = 0; i < 12; i++)
							 {
								 CStringW strT;
								 strT.Format(L"%d", i);
								 CStringW textCord = L"cord_";
								 textCord += strT;
								 WriteFileValue(textUserID, textCord, 0);
							 }

							 IServerUserItem * pIServerUserItem = NULL;
							 for (int i = 0; i < GAME_PLAYER; i++)
							 {
								 pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
								 if (pIServerUserItem != NULL)
								 {
									 if (wBindParam == pIServerUserItem->GetUserID())
									 {
										 ZeroMemory(&m_recordInfo, sizeof(m_recordInfo));
									 }
									 break;
								 }
							 }

							 break;
	}
	}
	return false;
}

//��Ϸ��Ϣ����
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_CURRENT_BET:
	{
							  //Ч������
							  ASSERT(wDataSize == sizeof(CMD_C_CurrentBet));
							  if (wDataSize != sizeof(CMD_C_CurrentBet))
							  {
								  return false;
							  }

							  //״̬Ч��
							  //	ASSERT(m_pITableFrame->GetGameStatus() == GS_GAME_FREE);
							  if (m_pITableFrame->GetGameStatus() != GS_GAME_FREE)
							  {
								  return true;
							  }

							  //��������
							  CMD_C_CurrentBet *pCurrentBet = (CMD_C_CurrentBet*)pData;
							  m_lBet = pCurrentBet->lBet;

							  //���±���
							  UpdatePokerPoints();

							  CMD_S_CurrentBet CurrentBet;
							  ZeroMemory(&CurrentBet, sizeof(CurrentBet));
							  CurrentBet.lBet = m_lBet;

							  //��������
							  m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_CURRENT_BET, &CurrentBet, sizeof(CurrentBet));
							  m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_CURRENT_BET, &CurrentBet, sizeof(CurrentBet));

							  return true;
	}
	case SUB_C_SWITCH_CARD:
	{
							  //Ч������
							  ASSERT(wDataSize == sizeof(CMD_C_SwitchCard));
							  if (wDataSize != sizeof(CMD_C_SwitchCard))
							  {
								  return false;
							  }

							  //״̬Ч��
							  //ASSERT(m_pITableFrame->GetGameStatus() == GS_GAME_PLAY);
							  if (m_pITableFrame->GetGameStatus() != GS_GAME_PLAY)
							  {
								  return true;
							  }

							  //��������
							  CMD_C_SwitchCard *pSwitchCard = (CMD_C_SwitchCard*)pData;
							  CopyMemory(m_bSwitchFlag, (pSwitchCard->switchCard.bSwitchFlag), sizeof(m_bSwitchFlag));
							  m_balreadySwitch = pSwitchCard->bSwitchCard;
							  CopyMemory(m_cbSwitchCardData, pSwitchCard->cbSwitchCardData, sizeof(m_cbSwitchCardData));

							  CMD_S_SwitchCard SwitchCard;
							  ZeroMemory(&SwitchCard, sizeof(SwitchCard));
							  CopyMemory(&SwitchCard.switchCard, &(pSwitchCard->switchCard), sizeof(SwitchCard.switchCard));

							  BYTE cbCardType = CT_INVALID;
							  //��ȡ����
							  cbCardType = m_GameLogic.GetCardType(m_cbSwitchCardData, MAX_CARD_COUNT);
							  if (cbCardType != CT_INVALID)
								  SwitchCard.bGuss = true;

							  //��������
							  m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_SWITCH_CARD, &SwitchCard, sizeof(SwitchCard));
							  m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_SWITCH_CARD, &SwitchCard, sizeof(SwitchCard));

							  return true;
	}
	case SUB_C_GAME_END:
	{
						   //Ч������
						   ASSERT(wDataSize == sizeof(CMD_C_GameEnd));
						   if (wDataSize != sizeof(CMD_C_GameEnd))
						   {
							   return false;
						   }

						   //״̬Ч��
						   //ASSERT(m_pITableFrame->GetGameStatus() == GS_GAME_PLAY || m_pITableFrame->GetGameStatus() == GS_GAME_LUCKYTIME);
						   if (m_pITableFrame->GetGameStatus() != GS_GAME_PLAY && m_pITableFrame->GetGameStatus() != GS_GAME_LUCKYTIME)
						   {
							   return true;
						   }

						   //��������
						   CMD_C_GameEnd *pGameEnd = (CMD_C_GameEnd*)pData;

						   //У������
						   //ASSERT(m_lBet == pGameEnd->lBet);

						   CopyMemory(m_cbSwitchCardData, pGameEnd->cbHandCardData, sizeof(m_cbSwitchCardData));

						   return OnEventGameConclude(pGameEnd->wChairID, NULL, GER_NORMAL);
	}
	case SUB_C_LUCKYTIME:
	{
							//״̬Ч��
							//	ASSERT(m_pITableFrame->GetGameStatus() == GS_GAME_PLAY);
							if (m_pITableFrame->GetGameStatus() != GS_GAME_PLAY)
							{
								return true;
							}

							//����״̬
							m_pITableFrame->SetGameStatus(GS_GAME_LUCKYTIME);


							//���ñ�ʶ
							m_bGuess = true;

							//�����˿�
							m_GameLogic.RandLuckyCardList(m_cbLuckyCardData, LUCKYTIME_CARDDATA_COUNT);

							//��������
							CMD_S_LuckyTime luckyTime;
							ZeroMemory(&luckyTime, sizeof(luckyTime));
							CopyMemory(luckyTime.cbLuckyCardData, m_cbLuckyCardData, sizeof(m_cbLuckyCardData));
							CopyMemory(luckyTime.cbGuessCardResultRecord, m_cbGuessCardResultRecord, sizeof(m_cbGuessCardResultRecord));
							//��������
							m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_LUCKYTIME, &luckyTime, sizeof(luckyTime));
							m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_LUCKYTIME, &luckyTime, sizeof(luckyTime));

							return true;
	}
	case   SUB_C_GAME_CLICK_GUSS:
	{
									CMD_S_CLICK_Guess s_guss;
									s_guss.cbIndex = rand() % LUCKYTIME_CARDDATA_COUNT;
									m_GussIndex = s_guss.cbIndex;
									m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_GUSS_INDEX, &s_guss, sizeof(s_guss));
									return true;
	}
	case SUB_C_GUESS:
	{
						//Ч������
						ASSERT(wDataSize == sizeof(CMD_C_Guess));
						if (wDataSize != sizeof(CMD_C_Guess))
						{
							return false;
						}

						//״̬Ч��
						ASSERT(m_pITableFrame->GetGameStatus() == GS_GAME_LUCKYTIME);
						if (m_pITableFrame->GetGameStatus() != GS_GAME_LUCKYTIME)
						{
							return true;
						}

						//��������
						CMD_C_Guess *pGuess = (CMD_C_Guess*)pData;
						ASSERT(m_GussIndex == pGuess->guessInfo.wLuckyCardIndex);
						if (m_GussIndex != pGuess->guessInfo.wLuckyCardIndex)
						{
							return true;
						}

						//У���˿�����
						ASSERT(pGuess->cbKeyCardData == m_cbLuckyCardData[pGuess->guessInfo.wLuckyCardIndex]);

						ASSERT(pGuess->guessInfo.wGuessCount - 1 < MAX_GUESS_COUNT);

						GUESSRECORD guessrecord;
						ZeroMemory(&guessrecord, sizeof(guessrecord));
						m_GameLogic.AnalyseGuess(pGuess->cbKeyCardData, pGuess->guessInfo.bBig, guessrecord);

#ifdef ATT_SERVER_CARD_CONFIG
						ReadCardConfig(m_cbFirstCardData, m_cbSecondCardData, &guessrecord);
#endif

						//������¼
						CopyMemory(&m_guessRecord[m_wCurrentGuessCount], &guessrecord, sizeof(guessrecord));
						bool bFullFlag = true;
						for (int i = 0; i < 6; i++)
						{
							if (m_cbGuessCardResultRecord[i] == 0)
							{
								m_cbGuessCardResultRecord[i] = pGuess->cbKeyCardData;
								bFullFlag = false;
								break;
							}
						}
						if (bFullFlag)
						{
							BYTE cbGuessCardResultRecordT[MAX_GUESS_COUNT];	//�´�С�Ƽ�¼
							CopyMemory(cbGuessCardResultRecordT, m_cbGuessCardResultRecord, sizeof(cbGuessCardResultRecordT));
							for (int i = 1; i < 6; i++)
							{
								m_cbGuessCardResultRecord[i - 1] = cbGuessCardResultRecordT[i];
							}
							m_cbGuessCardResultRecord[5] = pGuess->cbKeyCardData;
						}
						m_wCurrentGuessCount++;

						//��ͣ��ʶ
						m_bLuckyTimePause = true;

						CMD_S_Guess serverGuess;
						ZeroMemory(&serverGuess, sizeof(serverGuess));
						CopyMemory(serverGuess.cbGuessCardResultRecord, m_cbGuessCardResultRecord, sizeof(m_cbGuessCardResultRecord));
						CopyMemory(serverGuess.guessRecord, m_guessRecord, sizeof(m_guessRecord));
						serverGuess.wCurrentGuessCount = m_wCurrentGuessCount;

						//��������
						m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GUESS, &serverGuess, sizeof(serverGuess));
						m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GUESS, &serverGuess, sizeof(serverGuess));

						//�����ж�
						if (guessrecord.bValid  && !guessrecord.bGuessRight)
						{
							//��ͣ��ʶ
							m_bLuckyTimePause = false;
							OnEventGameConclude(pIServerUserItem->GetChairID(), NULL, GER_NORMAL);
						}

						//�²�����ж�
						if (pGuess->guessInfo.wGuessCount >= MAX_GUESS_COUNT &&
							((guessrecord.bValid  && guessrecord.bGuessRight  && guessrecord.bIsAlreadyGuess) ||
							(!guessrecord.bValid && !guessrecord.bGuessRight && guessrecord.bIsAlreadyGuess)))
						{
							//��ͣ��ʶ
							m_bLuckyTimePause = false;
							OnEventGameConclude(pIServerUserItem->GetChairID(), NULL, GER_NORMAL);
						}

						return true;
	}
	case SUB_C_CONTINUE_GUESS:		//��Ѻ
	{
										//״̬Ч��
										ASSERT(m_pITableFrame->GetGameStatus() == GS_GAME_LUCKYTIME);
										if (m_pITableFrame->GetGameStatus() != GS_GAME_LUCKYTIME)
										{
											return true;
										}

										//��ͣ��ʶ
										m_bLuckyTimePause = false;

										//��������
										m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CONTINUE_GUESS);
										m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CONTINUE_GUESS);

										return true;
	}
	case SUB_C_GAME_RECORD:
	{
							  //Ч������
							  ASSERT(wDataSize == sizeof(CMD_C_GameRecord));
							  if (wDataSize != sizeof(CMD_C_GameRecord))
							  {
								  return false;
							  }

							  //״̬Ч��
							  ASSERT(m_pITableFrame->GetGameStatus() == GS_GAME_FREE);
							  if (m_pITableFrame->GetGameStatus() != GS_GAME_FREE)
							  {
								  return true;
							  }

							  //��������
							  CMD_C_GameRecord *pGameRecord = (CMD_C_GameRecord*)pData;

							  CMD_S_GameRecord GameRecord;
							  ZeroMemory(&GameRecord, sizeof(GameRecord));
							  GameRecord.bShowRecord = pGameRecord->bShowRecord;

							  //��������
							  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_RECORD, &GameRecord, sizeof(GameRecord));
							  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_RECORD, &GameRecord, sizeof(GameRecord));

							  return true;
	}
	case SUB_C_GAME_AUTO:
	{
							//Ч������
							ASSERT(wDataSize == sizeof(CMD_C_GameAuto));
							if (wDataSize != sizeof(CMD_C_GameAuto))
							{
								return false;
							}

							//��������
							CMD_C_GameAuto *pGameAuto = (CMD_C_GameAuto*)pData;

							CMD_S_GameAuto GameAuto;
							ZeroMemory(&GameAuto, sizeof(GameAuto));
							GameAuto.bAuto = pGameAuto->bAuto;
							m_bAuto = pGameAuto->bAuto;

							//��������
							m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_AUTO, &GameAuto, sizeof(GameAuto));
							m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_AUTO, &GameAuto, sizeof(GameAuto));

							return true;
	}
	}

	return false;
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ȩ���ж�
	if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) || pIServerUserItem->IsAndroidUser())
	{
		return false;
	}
	// ��Ϣ����
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
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

//�����¼�
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID *pData, WORD wDataSize)
{
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
{
	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{

	DWORD dWuserID = pIServerUserItem->GetUserID();
	CStringW textUserID;
	textUserID.Format(L"%d", dWuserID);
	for (int i = 0; i < 6; i++)
	{
		CStringW strT;
		strT.Format(L"%d", i);
		CStringW textCard = L"card_";
		textCard += strT;
		m_cbGuessCardResultRecord[i] = ReadFileValue(textUserID, textCard, 0);
	}
	SCORE lRecordT[12];

	for (int i = 0; i < 12; i++)
	{
		CStringW strT;
		strT.Format(L"%d", i);
		CStringW textCord = L"cord_";
		textCord += strT;
		lRecordT[i] = ReadFileValue(textUserID, textCord, 0);
	}
    CopyMemory(&m_recordInfo, &lRecordT, sizeof(lRecordT));

	if (m_recordInfo.lPlayTotal == 0)
	{
		m_pITableFrame->SetGameTimer(IDI_CLEAR_RECORD, 7 * 24 * 60 * 60 * 1000, 1, dWuserID);
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
	DWORD dWuserID = pIServerUserItem->GetUserID();
	CStringW textUserID;
	textUserID.Format(L"%d", dWuserID);
	for (int i = 0; i < 6; i++)
	{
		CStringW strT;
		strT.Format(L"%d", i);
		CStringW textCard = L"card_";
		textCard += strT;
		WriteFileValue(textUserID, textCard, m_cbGuessCardResultRecord[i]);
	}
	int lRecordT[12];
	CopyMemory(lRecordT, &m_recordInfo, sizeof(lRecordT));
	for (int i = 0; i < 12; i++)
	{
		CStringW strT;
		strT.Format(L"%d", i);
		CStringW textCord = L"cord_";
		textCord += strT;
		WriteFileValue(textUserID, textCord, lRecordT[i]);
	}
	ZeroMemory(m_cbGuessCardResultRecord, sizeof(m_cbGuessCardResultRecord));
	ZeroMemory(&m_recordInfo, sizeof(m_recordInfo));
	return true;
}

//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	return true;
}

//���ص���
void CTableFrameSink::IncreaseBonus()
{
	m_Bonus.lBonus5K += 1;
	m_Bonus.lBonus4K += 1;
	m_Bonus.lBonusRS += 1;
	m_Bonus.lBonusSF += 1;
}

//����poker points
void CTableFrameSink::UpdatePokerPoints()
{
	BET_RADIO lTmpBetRadio[MAX_CARD_CT];
	ZeroMemory(lTmpBetRadio, sizeof(lTmpBetRadio));

	//��ʼ������
	//5k
	lTmpBetRadio[CT_5K].lHighRadio = 500;
	lTmpBetRadio[CT_5K].lMidRadio = 400;
	lTmpBetRadio[CT_5K].lLowRadio = 300;

	//RS
	lTmpBetRadio[CT_RS].lHighRadio = 250;
	lTmpBetRadio[CT_RS].lMidRadio = 200;
	lTmpBetRadio[CT_RS].lLowRadio = 150;

	//SF
	lTmpBetRadio[CT_SF].lHighRadio = 140;
	lTmpBetRadio[CT_SF].lMidRadio = 120;
	lTmpBetRadio[CT_SF].lLowRadio = 100;

	//4K
	lTmpBetRadio[CT_4K].lHighRadio = 60;
	lTmpBetRadio[CT_4K].lMidRadio = 50;
	lTmpBetRadio[CT_4K].lLowRadio = 40;

	//FH
	lTmpBetRadio[CT_FH].lHighRadio = 12;
	lTmpBetRadio[CT_FH].lMidRadio = 10;
	lTmpBetRadio[CT_FH].lLowRadio = 8;

	//FL
	lTmpBetRadio[CT_FL].lHighRadio = 8;
	lTmpBetRadio[CT_FL].lMidRadio = 7;
	lTmpBetRadio[CT_FL].lLowRadio = 6;

	//ST
	lTmpBetRadio[CT_ST].lHighRadio = 6;
	lTmpBetRadio[CT_ST].lMidRadio = 5;
	lTmpBetRadio[CT_ST].lLowRadio = 4;

	//3K
	lTmpBetRadio[CT_3K].lHighRadio = 3;
	lTmpBetRadio[CT_3K].lMidRadio = 3;
	lTmpBetRadio[CT_3K].lLowRadio = 3;

	//2P
	lTmpBetRadio[CT_2P].lHighRadio = 2;
	lTmpBetRadio[CT_2P].lMidRadio = 2;
	lTmpBetRadio[CT_2P].lLowRadio = 2;

	//1P
	lTmpBetRadio[CT_1P].lHighRadio = 1;
	lTmpBetRadio[CT_1P].lMidRadio = 1;
	lTmpBetRadio[CT_1P].lLowRadio = 1;

	for (INT i = 0; i<MAX_CARD_CT; i++)
	{
		m_lBetRadio[i].lHighRadio = lTmpBetRadio[i].lHighRadio * m_lBet;
		m_lBetRadio[i].lMidRadio = lTmpBetRadio[i].lMidRadio * m_lBet;
		m_lBetRadio[i].lLowRadio = lTmpBetRadio[i].lLowRadio * m_lBet;
	}
}

//����record
void CTableFrameSink::UpdateRecord(BYTE cbCardTypeIndex)
{
	switch (cbCardTypeIndex)
	{
	case CT_5K:
	{
				  m_recordInfo.l5KBet++;
				  break;
	}
	case CT_RS:
	{
				  m_recordInfo.lRSBet++;
				  break;
	}
	case CT_SF:
	{
				  m_recordInfo.lSFBet++;
				  break;
	}
	case CT_4K:
	{
				  m_recordInfo.l4KBet++;
				  break;
	}
	case CT_FH:
	{
				  m_recordInfo.lFHBet++;
				  break;
	}
	case CT_FL:
	{
				  m_recordInfo.lFLBet++;
				  break;
	}
	case CT_ST:
	{
				  m_recordInfo.lSTBet++;
				  break;
	}
	case CT_3K:
	{
				  m_recordInfo.l3KBet++;
				  break;
	}
	case CT_2P:
	{
				  m_recordInfo.l2PBet++;
				  break;
	}
	case CT_1P:
	{
				  m_recordInfo.l1PBet++;
				  break;
	}
		ASSERT(false);
	}
}

//������ͱ���
LONG CTableFrameSink::GetCardTypeRadio(BYTE cbCardTypeIndex)
{
	ASSERT(cbCardTypeIndex != CT_INVALID);

	switch (cbCardTypeIndex)
	{
	case CT_1P:
	case CT_2P:
	case CT_3K:
	case CT_5K:
	case CT_RS:
	case CT_SF:
	case CT_4K:
	{
				  //������
				  INT randResult = rand() % 3;
				  if (randResult == 0)
				  {
					  return m_lBetRadio[cbCardTypeIndex].lHighRadio;
				  }
				  else if (randResult == 1)
				  {
					  return m_lBetRadio[cbCardTypeIndex].lMidRadio;
				  }
				  else
				  {
					  return m_lBetRadio[cbCardTypeIndex].lLowRadio;
				  }

				  ASSERT(false);
				  break;
	}
	case CT_FH:
	case CT_FL:
	case CT_ST:
	{
				  return m_lBetRadio[cbCardTypeIndex].lHighRadio;
	}
		ASSERT(false);
	}

	return 0;
}

//д��־�ļ�
void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{
	//������������
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName = TEXT("ATT����LOG.txt");
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


//����
void CTableFrameSink::DispatchCardData()
{
	//�����˿�
	BYTE cbRandCard[FULL_COUNT];
	ZeroMemory(cbRandCard, sizeof(cbRandCard));
	m_GameLogic.RandCardList(cbRandCard, CountArray(cbRandCard));

	//�����˿�
	CopyMemory(m_cbFirstCardData, cbRandCard, sizeof(m_cbFirstCardData));
	CopyMemory(m_cbSecondCardData, &cbRandCard[FULL_COUNT - MAX_CARD_COUNT], sizeof(m_cbSecondCardData));
}

//�������
LONG CTableFrameSink::CalculateScore(ENUMCARDTYPE *pEnumCardType, INT nEnumCardCount, SCOREEX &lSysMaxScore, SCOREEX &lSysMinScore, BYTE cbMaxCardData[MAX_CARD_COUNT])
{
	//��ȡ�������
	BYTE cbMaxCardType = pEnumCardType[0].cbCardType;
	BYTE cbMaxIndex = 0;
	for (WORD i = 1; i<nEnumCardCount; i++)
	{
		if (cbMaxCardType > pEnumCardType[i].cbCardType)
		{
			cbMaxCardType = pEnumCardType[i].cbCardType;
			cbMaxIndex = i;
		}
	}

	//��ȡ��С����
	BYTE cbMinCardType = pEnumCardType[0].cbCardType;
	BYTE cbMinIndex = 0;
	for (WORD i = 1; i<nEnumCardCount; i++)
	{
		if (cbMinCardType < pEnumCardType[i].cbCardType)
		{
			cbMinCardType = pEnumCardType[i].cbCardType;
			cbMinIndex = i;
		}
	}
	//
	CopyMemory(cbMaxCardData, pEnumCardType[cbMaxIndex].cbEnumCardData, MAX_CARD_COUNT*sizeof(BYTE));

	//�������Ϊ��Ч����
	if (cbMaxCardType == CT_INVALID)
	{
		lSysMaxScore = (m_lBet * (m_pGameCustomRule->lExchangeRadio));
		lSysMinScore = (m_lBet * (m_pGameCustomRule->lExchangeRadio));

		return 0;
	}

	bool bHeldFlag[MAX_CARD_COUNT];
	ZeroMemory(bHeldFlag, sizeof(bHeldFlag));
	//���ͱ�ʶ
	bool bMark[MAX_CARD_CT];
	ZeroMemory(bMark, sizeof(bMark));
	m_GameLogic.AnalyseHeldCard(pEnumCardType[cbMaxIndex].cbEnumCardData, MAX_CARD_COUNT, bHeldFlag, bMark);

	//�����ܱ���
	LONG lTotalOpenRatio = 0;
	//ͳ�ƻ���
	for (WORD i = 0; i<GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}

		for (INT j = 0; j<MAX_CARD_CT; j++)
		{
			if (!bMark[j])
			{
				continue;
			}

			lSysMinScore -= GetCardTypeRadio(j) * (m_pGameCustomRule->lExchangeRadio);
			lTotalOpenRatio += GetCardTypeRadio(j);

			//4K���ͽ�����Ӧ���ذٷ�֮80
			if (j == CT_4K)
			{
				LONG lbonus4k = (m_Bonus.lBonus4K) * 80 / 100;
				lSysMinScore -= (lbonus4k * m_pGameCustomRule->lExchangeRadio);
			}
			else if (j == CT_SF)
			{
				LONG lbonussf = (m_Bonus.lBonusSF) * 80 / 100;
				lSysMinScore -= (lbonussf * m_pGameCustomRule->lExchangeRadio);
			}
			else if (j == CT_RS)
			{
				LONG lbonusrs = (m_Bonus.lBonusRS) * 80 / 100;
				lSysMinScore -= (lbonusrs * m_pGameCustomRule->lExchangeRadio);
			}
			else if (j == CT_5K)
			{
				LONG lbonus5k = (m_Bonus.lBonus5K) * 80 / 100;
				lSysMinScore -= (lbonus5k * m_pGameCustomRule->lExchangeRadio);
			}
			break;
		}
	}

	lSysMaxScore = (m_lBet * (m_pGameCustomRule->lExchangeRadio));

	return lTotalOpenRatio;
}

//������ƽ��
void  CTableFrameSink::ArrangeControlCards(BYTE cbFirstCards[MAX_CARD_COUNT], BYTE cbSceconCards[MAX_CARD_COUNT], BYTE cbMaxCards[MAX_CARD_COUNT])
{
	BYTE tmpCards[MAX_CARD_COUNT * 2];
	CopyMemory(tmpCards, cbFirstCards, sizeof(BYTE)*MAX_CARD_COUNT);
	CopyMemory(tmpCards + MAX_CARD_COUNT, cbSceconCards, sizeof(BYTE)*MAX_CARD_COUNT);
	CopyMemory(cbFirstCards, cbMaxCards, sizeof(BYTE)*MAX_CARD_COUNT);

	int nCount = 0;
	for (int i = 0; i < MAX_CARD_COUNT * 2; i++)
	{
		bool bFind = false;
		for (int j = 0; j < MAX_CARD_COUNT; j++)
		{
			if (tmpCards[i] == cbFirstCards[j])
			{
				bFind = true;
				break;
			}
		}

		if (!bFind)
		{
			cbSceconCards[nCount++] = tmpCards[i];
			if (nCount >= MAX_CARD_COUNT)
				break;
		}
	}
};
//������Ӯ
bool CTableFrameSink::DebugWinLose(IServerUserItem * pIServerUserItem)
{
	return rand() % 100 < m_gameStorage.GetUserWinLoseRatio(pIServerUserItem == nullptr ? -1 : pIServerUserItem->GetGameID());
}

//����Ŀ¼
bool CTableFrameSink::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//ģ��·��
	TCHAR szModulePath[MAX_PATH] = TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(), szModulePath, CountArray(szModulePath));

	//�����ļ�
	for (INT i = lstrlen(szModulePath); i >= 0; i--)
	{
		if (szModulePath[i] == TEXT('\\'))
		{
			szModulePath[i] = 0;
			break;
		}
	}

	//���ý��
	ASSERT(szModulePath[0] != 0);
	lstrcpyn(szWorkDirectory, szModulePath, wBufferCount);

	return true;
}

//д������
void CTableFrameSink::WriteFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, SCORE lValue)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH] = TEXT("");
	_sntprintf_s(szServerInfoPath, CountArray(szServerInfoPath), TEXT("%s\\ATT_Record.ini"), szDirectory);

	//��������
	TCHAR szString[16] = TEXT("");
	_sntprintf_s(szString, CountArray(szString), TEXT("%d"), lValue);

	//д������
	WritePrivateProfileString(pszKeyName, pszItemName, szString, szServerInfoPath);

	return;
}

//��ȡ����
SCORE CTableFrameSink::ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH] = TEXT("");
	_sntprintf_s(szServerInfoPath, CountArray(szServerInfoPath), TEXT("%s\\ATT_Record.ini"), szDirectory);

	//��ȡ����
	UINT nValue = GetPrivateProfileInt(pszKeyName, pszItemName, nDefault, szServerInfoPath);

	return (SCORE)nValue;
}


#ifdef ATT_SERVER_CARD_CONFIG
//�����˿�
void CTableFrameSink::ReadCardConfig(BYTE cbFirstCardData[MAX_CARD_COUNT], BYTE cbSecondCardData[MAX_CARD_COUNT], GUESSRECORD *pGuessRecord)
{
	//��ȡĿ¼
	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(CountArray(szPath), szPath);

	//��ȡ����
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\ATT.ini"), szPath);

	//��������
	TCHAR szServerName[MAX_PATH] = TEXT("");
	_sntprintf(szServerName, CountArray(szServerName), TEXT("%s"), m_pGameServiceOption->szServerName);

	//�����ʶ
	bool bEnable = (GetPrivateProfileInt(szServerName, TEXT("ENABLE"), 0, szFileName) == 1 ? true : false);
	bool bEnableGuess = (GetPrivateProfileInt(szServerName, TEXT("ENABLEGUESS"), 0, szFileName) == 1 ? true : false);

	if (bEnable)
	{
		//��ȡ��һ���˿�
		CString strFirstCard;
		for (INT i = 0; i<MAX_CARD_COUNT; i++)
		{
			strFirstCard.Format(TEXT("FIRSTCARD%d"), i);
			cbFirstCardData[i] = GetPrivateProfileInt(szServerName, strFirstCard, 0, szFileName);
		}

		//��ȡ�ڶ����˿�
		CString strSecondCard;
		for (INT i = 0; i<MAX_CARD_COUNT; i++)
		{
			strSecondCard.Format(TEXT("SECONDCARD%d"), i);
			cbSecondCardData[i] = GetPrivateProfileInt(szServerName, strSecondCard, 0, szFileName);
		}
	}

	if (pGuessRecord != NULL && bEnableGuess)
	{
		pGuessRecord->bGuessRight = true;
		pGuessRecord->bIsAlreadyGuess = true;
		pGuessRecord->bValid = true;
	}

	return;
}


#endif
//////////////////////////////////////////////////////////////////////////////////
//���Կ������
//
void CTableFrameSink::InitSystemStorage()
{
	BaseCrontol* pCtrl = m_gameStorage.GetCurTypeCtrl(SysCtrl);
	//�Ƿ��ѳ�ʼ��
	if (pCtrl == nullptr)
	{
		tagCustomRule * pCustomConfig = (tagCustomRule *)m_pITableFrame->GetCustomRule();
		ASSERT(pCustomConfig);

		StorageInfo newInfo;
		newInfo.lCurSysStorage = newInfo.lConfigSysStorage = m_pGameCustomRule->lSysStorage;
		newInfo.lCurPlayerStorage = newInfo.lConfigPlayerStorage = pCustomConfig->lUserStorage;
		newInfo.lCurParameterK = newInfo.lConfigParameterK = pCustomConfig->nDebugPercent;
		m_gameStorage.AddControl(SysCtrl, newInfo);
		m_gameStorage.GetCurTypeCtrl(SysCtrl)->m_lStorageResetCount -= 1;;

		//�ʽ��ʼ��
		m_gameStorage.m_gameReward.lCurStorage = m_gameStorage.m_gameReward.lConfigStorage = 0;
		m_gameStorage.m_gameReward.lDispatchStorage = 0;
		m_gameStorage.m_gameReward.lTaxRatio = 0;
		m_gameStorage.m_gameStatisticsInfo.stGameConfig.lStorageMax = -1;

		//��С����ϵͳ������ƣ����ע*�������*��Ϸ��С��
		m_gameStorage.m_gameStatisticsInfo.stGameConfig.lStorageMin = m_pITableFrame->GetCellScore() * m_pGameCustomRule->lBet[sizeof(m_pGameCustomRule->lBet) / sizeof(m_pGameCustomRule->lBet[0]) - 1] * m_pGameCustomRule->lExchangeRadio;

		//Ȩ������
		m_gameStorage.m_gameWieght.Add(0, 0, 480);
		m_gameStorage.m_gameWieght.Add(1, 5, 240);
		m_gameStorage.m_gameWieght.Add(6, 10, 160);
		m_gameStorage.m_gameWieght.Add(11, 15, 120);
		m_gameStorage.m_gameWieght.Add(16, 20, 96);
		m_gameStorage.m_gameWieght.Add(21, 30, 80);
		m_gameStorage.m_gameWieght.Add(31, 50, 48);
		m_gameStorage.m_gameWieght.Add(51, 75, 32);
		m_gameStorage.m_gameWieght.Add(76, 100, 24);
		m_gameStorage.m_gameWieght.Add(101, 150, 16);
		m_gameStorage.m_gameWieght.Add(151, 200, 12);
		m_gameStorage.m_gameWieght.Add(201, 400, 6);
		m_gameStorage.m_gameWieght.Add(401, 800, 6);
		m_gameStorage.m_gameWieght.Add(800, -1, 3);
	}
};
//�¼�����
bool CTableFrameSink::OnDebugEvent(VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	//��������
	if (wDataSize == 0)
	{
		m_gameStorage.RefreshAll(m_pITableFrame, pIServerUserItem);
		return true;
	}

	if (wDataSize < sizeof(ControlUpdateEvent))
	{
		LOG_WARN(L"��Ч�����¼�����");
		return false;
	}

	ControlUpdateEvent* pEvent = (ControlUpdateEvent*)pData;
	if (wDataSize != pEvent->nDataLen + sizeof(ControlUpdateEvent))
	{
		LOG_WARN(L"��Ч�����¼���������ȣ�");
		return false;
	}
	switch (pEvent->emEventType)
	{
	case EventSysCtrl:			//ϵͳ���Ը���
	case EventRoomCtrl:			//������Ը���
	{
									if (pEvent->nDataLen != sizeof(StorageInfo))
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									StorageInfo* pRequst = (StorageInfo*)(pEvent + 1);
									pRequst->lOperateUser = pIServerUserItem->GetUserID();
									m_gameStorage.AddControl(pEvent->emEventType == EventSysCtrl ? SysCtrl : RoomCtrl, *pRequst);
	}break;
	case EventUserCtrl:			//��ҵ��Ը���
	{
									if (pEvent->nDataLen != sizeof(GameDebugInfo))
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									GameDebugInfo* pRequst = (GameDebugInfo*)(pEvent + 1);
									pRequst->lOperateUser = pIServerUserItem->GetUserID();
									m_gameStorage.AddControl(UserCtrl, *pRequst);
	}break;
	case EventGameTax:			//��Ϸ��ˮ����
	{
									if (pEvent->nDataLen != sizeof(GameTaxInfo))
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									GameTaxInfo* pRequst = (GameTaxInfo*)(pEvent + 1);
									m_gameStorage.AddControl(GameTax, *pRequst);
	}break;
	case EventGameReward:		//��Ϸ�ʽ����
	{
									if (pEvent->nDataLen != sizeof(GameRewardInfo))
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									GameRewardInfo* pRequst = (GameRewardInfo*)(pEvent + 1);
									m_gameStorage.AddControl(GameReward, *pRequst);
	}break;
	case EventGameExtra:		//��Ϸ�������ݸ���
	{
									if (pEvent->nDataLen != sizeof(GameExtraInfo))
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									GameExtraInfo* pRequst = (GameExtraInfo*)(pEvent + 1);
									m_gameStorage.AddControl(GameExtra, *pRequst);
	}break;
	case EventGameWeightConfig:	//Ȩ�����ø���
	{
									int nSize = pEvent->nDataLen%sizeof(WeightConfig);
									if (nSize != 0)
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									WeightConfig* pConfig = (WeightConfig*)(pEvent + 1);
									m_gameStorage.AddControl(GameWeight, (WeightConfig*)(pEvent + 1), pEvent->nDataLen / sizeof(WeightConfig));
	}break;
	//case EventGameStatistics:	//��Ϸ�ۺ����ݸ���
	default:
		LOG_WARN(L"��Ч�����¼����ͣ�");
		return false;
	}

	m_gameStorage.RefreshEvent(m_pITableFrame, pIServerUserItem);
	return true;
};
//////////////////////////////////////////////////////////////////////////////////
