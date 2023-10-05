#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <locale>
#include "Dump.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
TCHAR g_szServerName[LEN_SERVER];


//��עʱ��
#define IDI_FREE					1									//����ʱ��
#define IDI_PLACE_JETTON			2									//��עʱ��
#define IDI_GAME_END				3									//����ʱ��
#define TIME_FREE					1									//����ʱ��
#define TIME_PLACE_JETTON			10									//��עʱ��
#define TIME_GAME_END				10									//����ʱ��

//ͨ��ȫ�ֱ���
SCORE							g_lCurAnChouRate = 5;									//��ǰ�������
SCORE							g_lCurMosgoldRate = 3;									//��ǰ�󽱳�ȡ����
SCORE							g_lCurMosgoldStorage = 0;								//��ǰ�󽱿��
SCORE							g_lCurMosgoldDispatchCon = 0;							//��ǰ�ɽ����
SCORE							g_lCurMosgoldDispatchRate = 30;							//��ǰ�ɽ�����
SCORE							g_lCurMosgoldStorageRate = 30;							//�󽱿�����(ÿ�����ð���ҿ���Ӧ�����ŵ��󽱿����)
SCORE							g_lCurAnChouTotal = 0;									//��ǰ��������
SCORE							g_lStatisticSysWin = 0;									//ͳ��(����ͨ�ÿ�棬�������������ϵͳ��Ӯ�Ͱ����)
SCORE							g_lStatisticService = 0;								//�����������

//���Ƶ�������(����ID)
DWORD							g_dwSysCtrlIncreaseIndex = 0;
DWORD							g_dwRoomCtrlIncreaseIndex = 0;
DWORD							g_dwAreaCtrlIncreaseIndex = 0;

CList<SYSCTRL, SYSCTRL&>  CTableFrameSink::m_listSysCtrl;
CList<ROOMCTRL, ROOMCTRL&> CTableFrameSink::m_listRoomCtrl;
CList<AREACTRL, AREACTRL&> CTableFrameSink::m_listAreaCtrl;

//////////////////////////////////////////////////////////////////////////
//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��ע����
	ZeroMemory(m_lAreaInAllScore,sizeof(m_lAreaInAllScore));
	ZeroMemory(m_lUserInAllScore,sizeof(m_lUserInAllScore));
	ZeroMemory(m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));
	ZeroMemory(m_lUserStartScore, sizeof(m_lUserStartScore));
	m_lAreaLimitScore = 0;
	m_lUserLimitScore = 0;
	ZeroMemory(m_tagUserAreaInfo, sizeof(m_tagUserAreaInfo));

	//�Ͼ���Ϣ
	ZeroMemory(m_lLastAllJettonPlace, sizeof(m_lLastAllJettonPlace));
	ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

	//�ɼ�	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));
	m_lBankerWinScore = 0;
	m_lCurTurnAnChouWin = 0;
    ZeroMemory(m_lRealAllJettonScore, sizeof(m_lRealAllJettonScore));

    m_lCurrentRobotJettonScore = 0;

	//�˿���Ϣ
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//ׯ����Ϣ
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
	//ׯ������
	m_lBankerMAX = 0l;
	m_lBankerAdd = 0l;
	m_lBankerScoreMAX = 0l;
	m_lBankerScoreAdd = 0l;
	m_lPlayerBankerMAX = 0l;
	m_bExchangeBanker = true;
	m_bWaitGiveUpBanker = false;

	//ռλ
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;
	}
	ZeroMemory(m_bContinueNoBetCount, sizeof(m_bContinueNoBetCount));

	//��������ׯ
	m_nRobotRequestBankerCount = 4;
	m_lRobotLostScoreLimitQB = 100000;
	m_wRobotMaxSerialTurn = 10;
	m_lRobotBankerWinScore = 0;

	//�����˿���
	m_nMaxChipRobot = 0;
	m_nChipRobotCount = 0;
	m_lRobotAreaLimit = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	m_nRobotListMaxCount = 0;
    m_lLimitRobotJettonScore = 0;
	//ʱ������
	m_dwJettonTime=0L;
	m_cbFreeTime = TIME_FREE;
	m_cbBetTime = TIME_PLACE_JETTON;
	m_cbEndTime = TIME_GAME_END;

	//��¼����
	m_lCurGameCount = 0;
	ZeroMemory(m_SimpleChartArray, sizeof(m_SimpleChartArray));
	ZeroMemory(m_AdvancedChartArray, sizeof(m_AdvancedChartArray));

	m_wChairCount = GAME_PLAYER;
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		m_listStatisticsRecord[wChairID].RemoveAll();
	}
	
	//�������
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

//��������
CTableFrameSink::~CTableFrameSink()
{


}

//�ͷŶ���
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

//�ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)WHCrashHandler);

	//��ѯ�ӿ�
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	ASSERT(m_pITableFrame!=NULL);
	//�����ж�
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"),TraceLevel_Exception);
		return false;
	}

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();

	ASSERT(m_pGameServiceOption!=NULL);
	ASSERT(m_pGameServiceAttrib!=NULL);

	//���ô����˷���������Ŀ
	if (IsGroupPersonalRoom())
	{
		m_wChairCount = GAME_PLAYER / m_pGameServiceOption->wTableCount;
		m_pITableFrame->SetTableChairCount(m_wChairCount);
	}

	ReadAIConfig();
	ReadConfigInformation(true);

	//��ʼ��¼ʱ��
	m_tmStartRecord = CTime::GetCurrentTime();

	//���ó���
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

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	//��ע����
	ZeroMemory(m_lAreaInAllScore, sizeof(m_lAreaInAllScore));
	ZeroMemory(m_lUserInAllScore, sizeof(m_lUserInAllScore));
	ZeroMemory(m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));
	ZeroMemory(m_tagUserAreaInfo, sizeof(m_tagUserAreaInfo));

	//�ɼ�	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	m_lBankerWinScore = 0;
	m_lCurTurnAnChouWin = 0;
    m_lCurrentRobotJettonScore = 0;
	//�˿���Ϣ
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	//�����˱���
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	return;
}

//��ѯ�����
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

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//���Ϳ����Ϣ
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		pIServerUserItem->SetUserStatus(US_READY, m_pITableFrame->GetTableID(), i);
	}

	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//CString strStorage;
	//CTime Time(CTime::GetCurrentTime());
	//strStorage.Format(TEXT(" ����: %s | ʱ��: %d-%d-%d %d:%d:%d | ���: %I64d \n"), m_pGameServiceOption->szServerName, Time.GetYear(), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_StorageStart);
	//WriteInfo(TEXT("��������������¼.log"),strStorage);

	//��ȡׯ��
	IServerUserItem *pIBankerServerUserItem = NULL;
	if (INVALID_CHAIR != m_wCurrentBanker)
	{
		pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		pIBankerServerUserItem->SetUserStatus(US_PLAYING, m_pITableFrame->GetTableID(), m_wCurrentBanker);
	}

	m_lBankerScore = 1000000000.00;

	//���ñ���
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
		//��ȡ�Զ�������
		tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
		ASSERT(pCustomConfig);

		m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
		if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
	}

	GameStart.cbTimeLeave = m_cbBetTime;
	GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);

	//��Ϸ���
	for (WORD wChairID = 0; wChairID < m_wChairCount; ++wChairID)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//���û���
		GameStart.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore);
		
		m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	
	}

	CString stTrace;
	CTime Time(CTime::GetCurrentTime());
	stTrace.Format(TEXT("%d-%d %d:%d:%d |  GAME_STARTGAME_START = %d, m_lBankerScore=%I64d, nChipRobotCount=%d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), GameStart.cbTimeLeave, m_lBankerScore, GameStart.nChipRobotCount);
	//WriteInfo(TEXT("DragonTigerBattlePlace.log"), stTrace);

	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{
            SaveData();
			//ռλ�����������ע����
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

			//��������ׯ
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
				//ׯ����ׯ��������
				m_wBankerTime++;
			}

			//������Ϣ
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			if (m_cbEndTime <= 0)
			{
				//��ȡ�Զ�������
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
				if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;
			}

			GameEnd.cbTimeLeave = m_cbEndTime;
			GameEnd.wBankerTime = m_wBankerTime;
          
			//�˿���Ϣ
			CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			CopyMemory(GameEnd.lUserInAllScore, m_lUserInAllScore, sizeof(m_lUserInAllScore));
			CopyMemory(GameEnd.lUserWinScore, m_lUserWinScore, sizeof(m_lUserWinScore));
			CopyMemory(GameEnd.lUserAreaWinScore, m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));
			CopyMemory(GameEnd.lUserRevenue, m_lUserRevenue, sizeof(m_lUserRevenue));

			//���ͻ���
			bool bfirstRecordAndroid = false;
			bool bfirstRecordRealPlayer = false;

			for ( WORD wUserIndex = 0; wUserIndex < m_wChairCount; ++wUserIndex )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
				if ( pIServerUserItem == NULL ) continue;

				//���óɼ�
				GameEnd.lUserScore=m_lUserWinScore[wUserIndex];
				GameEnd.lBankerWinScore = m_lBankerWinScore;
                //��ȥׯ�Һ�ռ����Һ��Լ�ÿ���������ע
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
				//������Ϣ					
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

			//�����Ͼ���ע
			CopyMemory(m_lLastAllJettonPlace, m_lUserInAllScore, sizeof(m_lLastAllJettonPlace));

			//��ձ������һע
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

				//���ͼ�¼
				SendGameRecord(pIServerUserItem, m_pITableFrame->GetGameStatus());
			}

			//ͳ�Ƽ�¼���20��
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
	case GER_USER_LEAVE:		//����뿪
		{
			//�м��ж�
			if (m_wCurrentBanker != wChairID)
			{
				//��ע���ܷ�����ע
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

							//��Ϸ���
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
					//���ֱ���
					tagScoreInfo ScoreInfoArray[GAME_PLAYER];
					ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

					//д�����
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


			//״̬�ж�
			if (m_pITableFrame->GetGameStatus() != GAME_SCENE_GAME_END)
			{
				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("����ׯ��[ %s ]ǿ�ˣ���Ϸ��ǰ������"), pIServerUserItem->GetNickName());

				//������Ϣ
				SendGameMessage(INVALID_CHAIR, szTipMsg);

				//����״̬
				m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

				if (m_cbEndTime <= 0)
				{
					//��ȡ�Զ�������
					tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
					ASSERT(pCustomConfig);

					m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
					if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;
				}

				//����ʱ��
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

				//������Ϣ
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd, sizeof(GameEnd));

				//ׯ����Ϣ
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

					//���óɼ�
					GameEnd.lUserScore = m_lUserWinScore[wUserIndex];
					GameEnd.lBankerWinScore = m_lBankerWinScore;
                    //��ȥׯ�Һ�ռ����Һ��Լ�ÿ���������ע
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
					//������Ϣ					
					m_pITableFrame->SendTableData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
				}
			}

			//�۳�����
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

			//�л�ׯ��
			ChangeBanker(true);

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
		//����״̬
	case GAME_SCENE_FREE:	
		{
			//��ȡ�Զ�������
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);

			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));			

			if (m_cbFreeTime <= 0)
			{
				//��ȡ�Զ�������
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

			//��ׯ��ׯ��
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
			
			//ռλ
			StatusFree.lOccupySeatFree = pCustomConfig->CustomGeneral.lOccupySeatFree;
			CopyMemory(StatusFree.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

			//����������
			if (pIServerUserItem->IsAndroidUser())
			{
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);
				CopyMemory(&StatusFree.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));

				CopyMemory(&(StatusFree.CustomAndroid.CustomAIConfig), &m_CustomAIConfig, sizeof(m_CustomAIConfig));
			}

			StatusFree.bIsGroupPersonalRoom = IsGroupPersonalRoom();

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));

			//���ͼ�¼
			SendGameRecord(pIServerUserItem, cbGameStatus);

			//������ׯ�б�
			SendApplyUser(pIServerUserItem);

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
			{
				//���ͷ����û���Ϣ
				SendRoomUserInfo();
			}

			return bSuccess;
		}
		//�з�״̬
	case GAME_SCENE_PLACE_JETTON:
	case GAME_SCENE_GAME_END:	
		{
			//��ȡ�Զ�������
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);

			//��������
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			if (m_cbBetTime <= 0)
			{
				//��ȡ�Զ�������
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
				if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
			}

			DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;
			StatusPlay.cbTimeLeave = (BYTE)(m_cbBetTime - __min(dwPassTime, m_cbBetTime));

			//״̬�ж�
			if (cbGameStatus == GAME_SCENE_GAME_END)
			{
				StatusPlay.cbTimeLeave = (BYTE)(m_cbEndTime - __min(dwPassTime, m_cbEndTime));
			}
			StatusPlay.cbGameStatus = m_pITableFrame->GetGameStatus();

			//�����ע
			if (pIServerUserItem->GetUserStatus() != US_LOOKON)
			{
				//�����ע
				StatusPlay.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore);
			}
			StatusPlay.lAreaLimitScore = m_lAreaLimitScore;
			CopyMemory(StatusPlay.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusPlay.lChipArray));

			//�����ж�
			if (cbGameStatus == GAME_SCENE_GAME_END)
			{
				//�˿���Ϣ
				CopyMemory(StatusPlay.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
			}

			//��ע
			memcpy(StatusPlay.lAreaInAllScore, m_lAreaInAllScore, sizeof(StatusPlay.lAreaInAllScore));
			memcpy(StatusPlay.lUserInAllScore, m_lUserInAllScore[wChairID], sizeof(StatusPlay.lUserInAllScore));

			//��ׯ��ׯ��
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

			//ռλ
			StatusPlay.lOccupySeatFree = pCustomConfig->CustomGeneral.lOccupySeatFree;
			CopyMemory(StatusPlay.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

			CopyMemory(StatusPlay.lUserAllScore, m_lUserInAllScore, sizeof(m_lUserInAllScore));

			//��ȥׯ�Һ�ռ����Һ��Լ�ÿ���������ע
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

			//������Ϣ
			if (cbGameStatus == GAME_SCENE_GAME_END)
			{
				StatusPlay.lEndUserScore = m_lUserWinScore[wChairID];
				StatusPlay.lBankerWinScore = m_lBankerWinScore;

				CopyMemory(StatusPlay.lUserWinScore, m_lUserWinScore, sizeof(m_lUserWinScore));
				CopyMemory(StatusPlay.lUserAreaWinScore, m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));

				CopyMemory(StatusPlay.lUserRevenue, m_lUserRevenue, sizeof(m_lUserRevenue));
			}
			
			//����������
			if(pIServerUserItem->IsAndroidUser())
			{
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				CopyMemory(&StatusPlay.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
				CopyMemory(&(StatusPlay.CustomAndroid.CustomAIConfig), &m_CustomAIConfig, sizeof(m_CustomAIConfig));
			}

			StatusPlay.bIsGroupPersonalRoom = IsGroupPersonalRoom();

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			//���ͼ�¼
			SendGameRecord(pIServerUserItem, cbGameStatus);

			//������ׯ�б�
			SendApplyUser(pIServerUserItem);

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
			{
				//������ע��Ϣ
				SendUserBetInfo(INVALID_DWORD);

				//���ͷ����û���Ϣ
				SendRoomUserInfo();
			}

			return bSuccess;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//��ѯ�޶�
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

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE:		//����ʱ��
		{
			//��ʼ��Ϸ
			m_pITableFrame->StartGame();

			if (m_cbBetTime <= 0)
			{
				//��ȡ�Զ�������
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
				if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
			}

			//����ʱ��
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_cbBetTime*1000,1,0L);

			//����״̬
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
	case IDI_PLACE_JETTON:		//��עʱ��
		{
			//״̬�ж�(��ֹǿ���ظ�����)
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_GAME_END)
			{
				//����״̬
				m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);			

				//������Ϸ
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

				if (m_cbEndTime <= 0)
				{
					//��ȡ�Զ�������
					tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
					ASSERT(pCustomConfig);

					m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
					if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;
				}

				//����ʱ��
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
	case IDI_GAME_END:			//������Ϸ
		{
			//д�����
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			for ( WORD wUserChairID = 0; wUserChairID < m_wChairCount; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL||(QueryBuckleServiceCharge(wUserChairID)==false)) continue;

				//ʤ������
				BYTE ScoreKind = m_lUserWinScore[wUserChairID] > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
				
				//д�����
				ScoreInfo[wUserChairID].lScore = m_lUserWinScore[wUserChairID];
				ScoreInfo[wUserChairID].lRevenue = m_lUserRevenue[wUserChairID];
				ScoreInfo[wUserChairID].cbType = ScoreKind;

				//д����ˮ
				//ׯ�ң��ܽ���Ϊ��������Ķ��ȫ��������ˮ���ܽ���Ϊ��������˰����Ӯ��ȼ�����ˮ�� 
				//�мң�δѺ��������Ѻע��ȼ�����ˮ�� + Ѻ�����򣨸�����˰����Ӯ������ˮ��

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

						//����Ӯ��
						if ((m_tagUserAreaInfo[wUserChairID][nAreaIndex].lAreaWinScore > 0))
						{
							ScoreInfo[wUserChairID].lChipSerialScore += abs(m_tagUserAreaInfo[wUserChairID][nAreaIndex].lAreaWinScore - m_tagUserAreaInfo[wUserChairID][nAreaIndex].lAreaRevenue);
						}
						//�������
						else
						{
							ScoreInfo[wUserChairID].lChipSerialScore += abs(m_tagUserAreaInfo[wUserChairID][nAreaIndex].lAreaInScore);
						}
					}
				}

				ASSERT((ScoreInfo[wUserChairID].lChipSerialScore >= (SCORE)0));

				//ׯ����������ע
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

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//////////////////////////////////////////////////
			CString stTrace;
			CTime Time(CTime::GetCurrentTime());
			stTrace.Format(TEXT("%d-%d %d:%d:%d |  m_cbFreeTime = %d\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_cbFreeTime);
			WriteInfo(TEXT("DragonTigerBattleTrace.log"), stTrace);

			if (m_cbFreeTime <= 0)
			{
				//��ȡ�Զ�������
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
				ASSERT(pCustomConfig);

				m_cbFreeTime = pCustomConfig->CustomGeneral.cbBetTime;
				if (m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
			}

			//////////////////////////////////////////////////

			if ((!IsGroupPersonalRoom()) || (IsGroupPersonalRoom() && m_wCurrentBanker != INVALID_CHAIR))
			{
				//����ʱ��
				m_dwJettonTime = (DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, 0L);

				//������Ϣ
				CMD_S_GameFree GameFree;
				ZeroMemory(&GameFree, sizeof(GameFree));
				GameFree.cbTimeLeave = m_cbFreeTime;
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));
			}

            //�ۼӻ������˳�ռλ����
            for(WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
            {
                if(m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
                {
                    continue;
                }

                WORD wTempChairID = m_wOccupySeatChairIDArray[i];

                //������
                if(m_pITableFrame->GetTableUserItem(wTempChairID) && m_pITableFrame->GetTableUserItem(wTempChairID)->IsAndroidUser())
                {
                    m_lRobotOccupySeatPlayCount[wTempChairID]++;
                }
            }

            //���������ռλ����
            //RectifyRobotOccupySeatPara();
			if (!m_bWaitGiveUpBanker)
			{
				//�л�ׯ��
				ChangeBanker(false);
			}
			else
			{
				if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker))
				{
					CString strTip;
					strTip.Format(TEXT("ׯ��[%s]��ׯ�ɹ�!"), m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->GetNickName());
					SendGameMessage(INVALID_CHAIR, strTip);
				}

				//�л�ׯ��
				ChangeBanker(true);
				m_bWaitGiveUpBanker = false;
			}

			//���ж�ȡ
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

				//���ͼ�¼
				SendGameRecord(pIServerUserItem, m_pITableFrame->GetGameStatus());
			}
			
			// ������Ϣ
			m_pITableFrame->SendRoomData(NULL, SUB_S_CLEAR_USER_BET_INFO);

			//ռλ�����������ע����
			bool bvalid = false;
			for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
			{
				if (m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
				{
					continue;
				}
              
				if (m_bContinueNoBetCount[i] >= MAX_FREEBET_OCCUPY_SEAT)
				{
					//������Ч
					bvalid = true;
					WORD wTempChairID = m_wOccupySeatChairIDArray[i];
                    //�����˹���
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
            //������ռλ�����˳�ռλ����	
            for(WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
            {
                if(m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
                {
                    continue;
                }

                WORD wTempChairID = m_wOccupySeatChairIDArray[i];

                //������
                if(m_pITableFrame->GetTableUserItem(wTempChairID) && m_pITableFrame->GetTableUserItem(wTempChairID)->IsAndroidUser()
                   && m_lRobotOccupySeatPlayCount[wTempChairID] > m_nRobotQuitSeatTurn)
                {
                    //������Ч
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

				//��������
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
			}

			stTrace.Format(TEXT("%d-%d %d:%d:%d |  GAME_FREEGAME_FREE\n"), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond());
			//WriteInfo(TEXT("DragonTigerBattlePlace.log"), stTrace);

			return true;	
		}
	}
	return false;
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//�û���ע
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			if (wDataSize != sizeof(CMD_C_PlaceJetton))
			{
				return true;
			}

			//�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus < US_READY)
			{
				SendGameMessage(pIServerUserItem->GetChairID(), TEXT("����һ����ע��"));

				return true;
			}

			//״̬Ч��
			if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLACE_JETTON)
			{
				return true;
			}

			//��Ϣ����
			CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pData;
			return OnUserPlaceJetton(pUserData->wChairID,pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
		}
	case SUB_C_CONTINUE_JETTON:
		{
			//Ч������
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return pIServerUserItem->IsAndroidUser();
			}

			//�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING && pIServerUserItem->IsAndroidUser())
			{
				return true;
			}

			if (!pIServerUserItem->IsAndroidUser() && pUserData->cbUserStatus < US_READY)
			{
				return true;
			}

			//״̬Ч��
			if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLACE_JETTON)
			{
				return pIServerUserItem->IsAndroidUser();
			}

			if (pIServerUserItem->GetChairID() == m_wCurrentBanker)
			{
				return pIServerUserItem->IsAndroidUser();
			}

			//��һ���
			WORD wChairID = pIServerUserItem->GetChairID();
            SCORE lUserScore = pIServerUserItem->GetUserScore();

            SCORE lLastAllJettonPlace = 0L;
            SCORE lUserJettonScore = 0L;

			//ͳ�Ʊ�������ע
			for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
			{
				lUserJettonScore += m_lUserInAllScore[wChairID][nAreaIndex];
                if(m_lUserInAllScore[wChairID][nAreaIndex] != 0)
                {
                    m_lLastAllJettonPlace[wChairID][nAreaIndex] = 0;
                }
			}
            //ͳ���Ͼ�����ע
            for(int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
            {
                lLastAllJettonPlace += m_lLastAllJettonPlace[wChairID][nAreaIndex];
            }
		
			//�����ע��ʶ
			bool bCheckPlaceJettonIsOk = true;

			//�ж��û�����Ƿ��㹻,�Ƿ񳬳���������
			if (lUserScore - lUserJettonScore < lLastAllJettonPlace
				|| lUserJettonScore + lLastAllJettonPlace > m_lUserLimitScore)
			{
				bCheckPlaceJettonIsOk = false;
			}
			
            //��������	
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
               
				//�����˷�����ע��Ϣ
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

			//У������
			for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
			{
				if (m_wOccupySeatChairIDArray[i] == pIServerUserItem->GetChairID())
				{
					binvalid = true;

					//������Ч
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

			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

			return true;
		}
	 case SUB_C_OCCUPYSEAT:
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_OccupySeat));
			if (wDataSize != sizeof(CMD_C_OccupySeat))
			{
				return pIServerUserItem->IsAndroidUser();
			}

			//��Ϣ����
			CMD_C_OccupySeat *pOccupySeat = (CMD_C_OccupySeat *)pData;

			if (pOccupySeat->wOccupySeatChairID != pIServerUserItem->GetChairID())
			{
				return pIServerUserItem->IsAndroidUser();
			}

			return OnUserOccupySeat(pOccupySeat->wOccupySeatChairID, pOccupySeat->cbOccupySeatIndex);
		}
	 case SUB_C_APPLY_BANKER:		//������ׯ
		{
			//�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();

			//�û�Ч��
			if (pUserData->cbUserStatus == US_LOOKON)
			{
				return true;
			}

			return OnUserApplyBanker(pIServerUserItem);
		}
	 case SUB_C_CANCEL_BANKER:		//ȡ����ׯ
		{
			//�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();

			//�û�Ч��
			if (pUserData->cbUserStatus == US_LOOKON)
			{
				return true;
			}

			return OnUserCancelBanker(pIServerUserItem);
		}
	 case SUB_C_REQUEST_STATISTICS_RECORD:
		{
			//Ч������
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

				//�����ж�				
				if (wSendDataSize + sizeof(CMD_S_StatisticsRecord) > sizeof(cbDataBuffer))
				{
					//��������
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
					
					//(0�ֲ��ۼ�ʤ��)
					pStatisticsRecord->lWinCount += (StatisticsRecord.lWinScore > 0 ? 1 : 0);
				}

				wSendDataSize += sizeof(CMD_S_StatisticsRecord);
				pStatisticsRecord++;
			}

			//�����ж�	
			if (wSendDataSize > 0)
			{
				//��������
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

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// ��Ϣ����
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		//Ȩ���ж�
		if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
		{
			return false;
		}

		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
		case SUB_C_ANCHOU:
		{
			//Ч������
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

			//������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ANCHOU, &AnChou, sizeof(AnChou));

			// ������Ϣ
			CMD_S_DebugText DebugText;
			ZeroMemory(&DebugText, sizeof(DebugText));

			TCHAR szMessageText[256] = TEXT("��ˮ���óɹ�!\n");
			CopyMemory(DebugText.szMessageText, szMessageText, sizeof(szMessageText));

			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_DEBUGTEXT_GENERAL, &DebugText, sizeof(DebugText));

			return true;
		}
		case SUB_C_MOSGOLD:
		{
			//Ч������
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

			//������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_MOSGOLD, &Mosgold, sizeof(Mosgold));

			// ������Ϣ
			CMD_S_DebugText DebugText;
			ZeroMemory(&DebugText, sizeof(DebugText));

			TCHAR szMessageText[256] = TEXT("�����óɹ�!\n");
			CopyMemory(DebugText.szMessageText, szMessageText, sizeof(szMessageText));

			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_DEBUGTEXT_GENERAL, &DebugText, sizeof(DebugText));

			return true;
		}
		case SUB_C_SYS_CTRL:
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_SysCtrl));
			if (wDataSize != sizeof(CMD_C_SysCtrl))
			{
				return false;
			}

			CMD_C_SysCtrl *pSysCtrl = (CMD_C_SysCtrl *)pData;

			bool bTempChouMosgoldStorage = false;

			//��ȡ�Զ�������
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);

			//Ѱ������ִ�е��б�(����ֻ��һ������ִ�е��б�)
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

			//����ϵͳ�����б�
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

			//�����б�
			m_listSysCtrl.AddTail(sysctrl);

			//��ʼ�󽱿������
			g_lCurMosgoldStorage += lTempChouMosgoldStorage;
			g_lStatisticSysWin += lTempChouMosgoldStorage;

			//��������
			CMD_S_CurSysCtrlInfo CurSysCtrlInfo;
			ZeroMemory(&CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

			CurSysCtrlInfo.lSysCtrlCurSysStorage = pSysCtrl->lSysCtrlSysStorage;
			CurSysCtrlInfo.lSysCtrlCurPlayerStorage = sysctrl.lSysCtrlCurPlayerStorage;
			CurSysCtrlInfo.lSysCtrlParameterK = pSysCtrl->lSysCtrlParameterK;
			CurSysCtrlInfo.lSysCtrlSysStorResetRate = pSysCtrl->lSysCtrlSysStorResetRate;
			CurSysCtrlInfo.lSysCtrlResetCount = 0;
			CurSysCtrlInfo.lSysCtrlSysWin = CalListSysCtrlSysWin();

			//��������
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_SYSCTRL_INFO, &CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

			CMD_S_Mosgold Mosgold;
			ZeroMemory(&Mosgold, sizeof(Mosgold));

			Mosgold.lCurMosgoldStorage = g_lCurMosgoldStorage;
			Mosgold.lCurMosgoldRate = g_lCurMosgoldRate;
			Mosgold.lCurMosgoldDispatchCon = g_lCurMosgoldDispatchCon;
			Mosgold.lCurMosgoldDispatchRate = g_lCurMosgoldDispatchRate;
			Mosgold.lCurMosgoldStorageRate = g_lCurMosgoldStorageRate;

			//������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_MOSGOLD, &Mosgold, sizeof(Mosgold));

			// ������Ϣ
			CMD_S_DebugText DebugText;
			ZeroMemory(&DebugText, sizeof(DebugText));

			TCHAR szMessageText[256] = TEXT("ϵͳ���ƿ�����óɹ�!\n");
			CopyMemory(DebugText.szMessageText, szMessageText, sizeof(szMessageText));

			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_DEBUGTEXT_GENERAL, &DebugText, sizeof(DebugText));

			return true;
		}
		case SUB_C_REFRESH_CUR_SYSCTRL_INFO:
		{
			//Ч������
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
			}

			//��������
			CMD_S_CurSysCtrlInfo CurSysCtrlInfo;
			ZeroMemory(&CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

			//Ѱ������ִ�е��б�(����ֻ��һ������ִ�е��б�)
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

			//��������
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

			//���õ���
			if (!pRoomCtrl->bCancelRoomCtrl)
			{
				//�½�����
				if (pRoomCtrl->dwSelCtrlIndex == INVALID_DWORD)
				{
					//Ѱ������ִ�е��б�(����ֻ��һ������ִ�е��б�)
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

					//�����б�
					m_listRoomCtrl.AddTail(roomctrl);

					CMD_S_Mosgold Mosgold;
					ZeroMemory(&Mosgold, sizeof(Mosgold));

					Mosgold.lCurMosgoldStorage = g_lCurMosgoldStorage;
					Mosgold.lCurMosgoldRate = g_lCurMosgoldRate;
					Mosgold.lCurMosgoldDispatchCon = g_lCurMosgoldDispatchCon;
					Mosgold.lCurMosgoldDispatchRate = g_lCurMosgoldDispatchRate;
					Mosgold.lCurMosgoldStorageRate = g_lCurMosgoldStorageRate;

					//������Ϣ
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_MOSGOLD, &Mosgold, sizeof(Mosgold));
				}
			}
			//ȡ������
			else
			{
				//���ҷ������
				POSITION posKey = FindRoomCtrl(pRoomCtrl->dwSelCtrlIndex);
				if (posKey)
				{
					//���÷����¼״̬
					ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(posKey);

					roomctrl.roomCtrlStatus = (roomctrl.lRoomCtrlSysWin == 0) ? CANCEL : EXECUTE_CANCEL;
				}
			}

			//��������б�
			POSITION pos = m_listRoomCtrl.GetHeadPosition();
			POSITION firstQueueCtrlPos = NULL;
			bool bSendCurRoomCtrlInfo = false;

			while (pos)
			{
				POSITION posTemp = pos;
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//���ͽ��
					CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
					ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

					CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
					CurRoomCtrlInfo.lRoomCtrlCurSysStorage = roomctrl.lRoomCtrlCurSysStorage;
					CurRoomCtrlInfo.lRoomCtrlCurPlayerStorage = roomctrl.lRoomCtrlCurPlayerStorage;
					CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
					CurRoomCtrlInfo.lRoomCtrlSysStorResetRate = roomctrl.lRoomCtrlSysStorResetRate;
					CurRoomCtrlInfo.lRoomCtrlSysWin = CalListRoomCtrlSysWin();

					// ������Ϣ
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

			//��û��һ���ڽ�����״̬�����͵�һ������
			if (firstQueueCtrlPos)
			{
				ROOMCTRL &temproomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);

				//���ͽ��
				CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
				ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

				CurRoomCtrlInfo.dwCtrlIndex = temproomctrl.dwCtrlIndex;
				CurRoomCtrlInfo.lRoomCtrlCurSysStorage = temproomctrl.lRoomCtrlCurSysStorage;
				CurRoomCtrlInfo.lRoomCtrlCurPlayerStorage = temproomctrl.lRoomCtrlCurPlayerStorage;
				CurRoomCtrlInfo.lRoomCtrlParameterK = temproomctrl.lRoomCtrlParameterK;
				CurRoomCtrlInfo.lRoomCtrlSysStorResetRate = temproomctrl.lRoomCtrlSysStorResetRate;
				CurRoomCtrlInfo.lRoomCtrlSysWin = CalListRoomCtrlSysWin();

				// ������Ϣ
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
				bSendCurRoomCtrlInfo = true;
			}

			if (!bSendCurRoomCtrlInfo)
			{
				// ������Ϣ
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO);
			}

			//��ǰ�����б���Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_ROOMCTRL_INFO);
			pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == QUEUE || roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//���ͽ��
					CMD_S_ListCurRoomCtrlInfo ListCurRoomCtrlInfo;
					ZeroMemory(&ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));

					CopyMemory(&(ListCurRoomCtrlInfo.curRoomCtrlListInfo), &roomctrl, sizeof(roomctrl));

					// ������Ϣ
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_ROOMCTRL_INFO, &ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));
				}
			}

			return true;
		}
		case SUB_C_REFRESH_CUR_ROOMCTRL_INFO:
		{
			//Ч������
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
			}

			//��������б�
			POSITION pos = m_listRoomCtrl.GetHeadPosition();
			POSITION firstQueueCtrlPos = NULL;
			bool bSendCurRoomCtrlInfo = false;

			while (pos)
			{
				POSITION posTemp = pos;
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//���ͽ��
					CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
					ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

					CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
					CurRoomCtrlInfo.lRoomCtrlCurSysStorage = roomctrl.lRoomCtrlCurSysStorage;
					CurRoomCtrlInfo.lRoomCtrlCurPlayerStorage = roomctrl.lRoomCtrlCurPlayerStorage;
					CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
					CurRoomCtrlInfo.lRoomCtrlSysStorResetRate = roomctrl.lRoomCtrlSysStorResetRate;
					CurRoomCtrlInfo.lRoomCtrlSysWin = CalListRoomCtrlSysWin();

					// ������Ϣ
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

			//��û��һ���ڽ�����״̬�����͵�һ������
			if (firstQueueCtrlPos)
			{
				ROOMCTRL &temproomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);

				//���ͽ��
				CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
				ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

				CurRoomCtrlInfo.dwCtrlIndex = temproomctrl.dwCtrlIndex;
				CurRoomCtrlInfo.lRoomCtrlCurSysStorage = temproomctrl.lRoomCtrlCurSysStorage;
				CurRoomCtrlInfo.lRoomCtrlCurPlayerStorage = temproomctrl.lRoomCtrlCurPlayerStorage;
				CurRoomCtrlInfo.lRoomCtrlParameterK = temproomctrl.lRoomCtrlParameterK;
				CurRoomCtrlInfo.lRoomCtrlSysStorResetRate = temproomctrl.lRoomCtrlSysStorResetRate;
				CurRoomCtrlInfo.lRoomCtrlSysWin = CalListRoomCtrlSysWin();

				// ������Ϣ
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
				bSendCurRoomCtrlInfo = true;
			}

			if (!bSendCurRoomCtrlInfo)
			{
				// ������Ϣ
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO);
			}

			return true;
		}
		case SUB_C_REFRESH_LIST_CUR_ROOMCTRL_INFO:
		{
			//Ч������
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
			}

			//��ǰ�����б���Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_LIST_CUR_ROOMCTRL_INFO);
			POSITION pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == QUEUE || roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//���ͽ��
					CMD_S_ListCurRoomCtrlInfo ListCurRoomCtrlInfo;
					ZeroMemory(&ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));

					CopyMemory(&(ListCurRoomCtrlInfo.curRoomCtrlListInfo), &roomctrl, sizeof(roomctrl));

					// ������Ϣ
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

			//��������
			if (!pAreaCtrl->bCancelAreaCtrl)
			{
				//Ѱ������ִ�е��б�(����ֻ��һ������ִ�е��б�)
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

				//�����б�
				m_listAreaCtrl.AddTail(areactrl);

				//���ͽ��
				CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
				ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
				CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
				CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;			
				CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				CurAreaCtrlInfo.lAreaCtrlSysWin = CalListAreaCtrlSysWin();

				// ������Ϣ
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));
			}
			//ȡ������
			else
			{
				//��������б�
				POSITION pos = m_listAreaCtrl.GetHeadPosition();
				while (pos)
				{
					AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

					if (areactrl.dwCtrlIndex == pAreaCtrl->dwCtrlIndex)
					{
						areactrl.areaCtrlStatus = (areactrl.lAreaCtrlSysWin == 0) ? AREACTRL_CANCEL : AREACTRL_EXECUTE_CANCEL;

						// ������Ϣ
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO);

						break;
					}
				}
			}

			return m_pServerDebug->ServerDebug(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
		}
		case SUB_C_REFRESH_CUR_AREACTRL_INFO:
		{
			//Ч������
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
			}

			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO);

			POSITION pos = m_listAreaCtrl.GetHeadPosition();
			POSITION posLast = NULL;

			while (pos)
			{
				posLast = pos;
				AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

				if (areactrl.areaCtrlStatus == AREACTRL_PROGRESSING)
				{
					//���ͽ��
					CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
					ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
					CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
					CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
					CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
					CurAreaCtrlInfo.lAreaCtrlSysWin = CalListAreaCtrlSysWin();

					// ������Ϣ
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					posLast = NULL;

					break;
				}
			}

			//�������һ��(��������Ч��)
			if (posLast)
			{
				AREACTRL &areactrl = m_listAreaCtrl.GetAt(posLast);

				//���ͽ��
				CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
				ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
				CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
				CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
				CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				CurAreaCtrlInfo.lAreaCtrlSysWin = CalListAreaCtrlSysWin();

				// ������Ϣ
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));
			}

			return true;
		}
		case SUB_C_REFRESH_STATISTIC_SERVICE:
		{
			//Ч������
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

			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_STATISTIC_SERVICE, &StatisticService, sizeof(StatisticService));

			return true;
		}
		case SUB_C_REFRESH_ANCHOU_MOSGOLD:
		{
			//Ч������
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
			}

			CMD_S_AnChou AnChou;
			ZeroMemory(&AnChou, sizeof(AnChou));

			AnChou.lCurAnChouRate = g_lCurAnChouRate;
			AnChou.lCurAnChouTotal = g_lCurAnChouTotal;

			//������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ANCHOU, &AnChou, sizeof(AnChou));

			CMD_S_Mosgold Mosgold;
			ZeroMemory(&Mosgold, sizeof(Mosgold));

			Mosgold.lCurMosgoldStorage = g_lCurMosgoldStorage;
			Mosgold.lCurMosgoldRate = g_lCurMosgoldRate;
			Mosgold.lCurMosgoldDispatchCon = g_lCurMosgoldDispatchCon;
			Mosgold.lCurMosgoldDispatchRate = g_lCurMosgoldDispatchRate;
			Mosgold.lCurMosgoldStorageRate = g_lCurMosgoldStorageRate;

			//������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_MOSGOLD, &Mosgold, sizeof(Mosgold));

			//���ͷ����û���Ϣ
			SendRoomUserInfo();

			return true;
		}
		case SUB_C_REFRESH_TRACELOG:
		{
			POSITION pos = NULL;

			//ͨ�ÿ������б�
			pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				 SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				 //���ͽ��
				 CMD_S_SysCtrlTraceLog sysCtrlResult;
				 ZeroMemory(&sysCtrlResult, sizeof(sysCtrlResult));

				 CopyMemory(&(sysCtrlResult.SysCtrlTraceLog), &sysctrl, sizeof(sysctrl));

				 // ������Ϣ
				 m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_SYSCTRL_TRACELOG, &sysCtrlResult, sizeof(sysCtrlResult));
			}

			//��������б�
			pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
			{
				 ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				 //���ͽ��
				 CMD_S_RoomCtrlTraceLog roomCtrlResult;
				 ZeroMemory(&roomCtrlResult, sizeof(roomCtrlResult));

				 CopyMemory(&(roomCtrlResult.RoomCtrlTraceLog), &roomctrl, sizeof(roomctrl));

				 // ������Ϣ
				 m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_ROOMCTRL_TRACELOG, &roomCtrlResult, sizeof(roomCtrlResult));
			}

			//��������б�
			pos = m_listAreaCtrl.GetHeadPosition();
			while (pos)
			{
				 AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

				 //���ͽ��
				 CMD_S_AreaCtrlTraceLog areaCtrlResult;
				 ZeroMemory(&areaCtrlResult, sizeof(areaCtrlResult));

				 CopyMemory(&(areaCtrlResult.AreaCtrlTraceLog), &areactrl, sizeof(areactrl));

				 // ������Ϣ
				 m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_AREACTRL_TRACELOG, &areaCtrlResult, sizeof(areaCtrlResult));
			}

			CMD_S_StartRecordTraceLog ListWinLostInfo;
			ZeroMemory(&ListWinLostInfo, sizeof(ListWinLostInfo));

			ListWinLostInfo.tmStartRecord = m_tmStartRecord;

			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_STARTRECORD_TRACELOG, &ListWinLostInfo, sizeof(ListWinLostInfo));

			return true;
		}
		case SUB_C_QUERY_AICONFIG_DEBUG:
		{
			//Ч������
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

			//������Ϣ
			m_pITableFrame->SendRoomAIConfigData(pIServerUserItem, SUB_S_QUERY_AICONFIG_DEBUG_RESULT, &AIConfigDebugResult, sizeof(AIConfigDebugResult));

			return true;
		}
		case SUB_C_SAVE_AICONFIG_DEBUG:
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_SaveAIConfigDebug));
			if (wDataSize != sizeof(CMD_C_SaveAIConfigDebug))
			{
				return false;
			}

			CMD_C_SaveAIConfigDebug* pSaveAIConfigDebug = (CMD_C_SaveAIConfigDebug*)pData;

			//�������� 
			CopyMemory(&m_CustomAIConfig, &(pSaveAIConfigDebug->SaveAIConfigDebug), sizeof(m_CustomAIConfig));

			CMD_S_AIConfigDebugResult AIConfigDebugResult;
			ZeroMemory(&AIConfigDebugResult, sizeof(AIConfigDebugResult));

			AIConfigDebugResult.cbType = FALSE;
			AIConfigDebugResult.wKindID = m_pGameServiceOption->wKindID;
			AIConfigDebugResult.wServerID = m_pGameServiceOption->wServerID;
			CopyMemory(&(AIConfigDebugResult.AIConfigDebugResult), &m_CustomAIConfig, sizeof(m_CustomAIConfig));

			//������Ϣ
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

//�û�����
bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//�л�ׯ��
	if (wChairID == m_wCurrentBanker) ChangeBanker(true);

	//ȡ������
	for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID != m_ApplyUserArray[i]) continue;

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		//�������
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker, sizeof(CancelBanker));

		//���ñ���
		CancelBanker.wCancelUser = wChairID;

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��¼��ʼ����
	m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();
	m_listStatisticsRecord[wChairID].RemoveAll();
	pIServerUserItem->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), wChairID);

	//����ʱ��
	if ((bLookonUser == false) && (m_dwJettonTime == 0L) && !IsGroupPersonalRoom())
	{
		if (m_cbFreeTime <= 0)
		{
			//��ȡ�Զ�������
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
		SendGameMessage(wChairID, TEXT("�ȴ��û���ׯ!"));
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);

		//��ȡ�����˷������
		GetGroupPersonalRoomRule();

		m_pITableFrame->KillGameTimer(IDI_FREE);
		m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
		m_pITableFrame->KillGameTimer(IDI_GAME_END);
	}

	//���ͷ����û���Ϣ
	SendRoomUserInfo();

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//���ͷ����û���Ϣ
	SendRoomUserInfo();

	//��ʼ��������
	m_lUserStartScore[wChairID] = 0;
	m_listStatisticsRecord[wChairID].RemoveAll();

	bool bvalid = false;
	//У������
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		if (m_wOccupySeatChairIDArray[i] == wChairID)
		{
			bvalid = true;

			//������Ч
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

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
	}

	//��¼�ɼ�
	if (bLookonUser == false)
	{
		//ȡ������
		for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID != m_ApplyUserArray[i]) continue;

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			//���ñ���
			CancelBanker.wCancelUser = wChairID;

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}
		//�л�ׯ��
		if (wChairID == m_wCurrentBanker) ChangeBanker(true);
		return true;
	}

	return true;
}

void CTableFrameSink::SaveData()
{
    SCORE tempRealScore = 0L;

    //��ҵ�����ע���ݼ�¼
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


//��ע�¼�
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, SCORE lJettonScore)
{
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

	//Ч�����
	ASSERT((cbJettonArea >= 0 && cbJettonArea < AREA_MAX) && lJettonScore > 0);
	if (!(cbJettonArea >= 0 && cbJettonArea < AREA_MAX) && lJettonScore > 0)
	{
		return true;
	}

	if (wChairID == m_wCurrentBanker)
	{
		return true;
	}

	//Ч����ע����
	//��ȡ�Զ�������
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


	//����ע��
    SCORE lJettonCount = 0l;
	for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
	{
		lJettonCount += m_lUserInAllScore[wChairID][nAreaIndex];
	}
		
    SCORE lUserScore = pIServerUserItem->GetUserScore();

	//�Ϸ�У��
	if (lUserScore < lJettonCount + lJettonScore)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pIServerUserItem->IsAndroidUser())
		{
			CString strTrace;
			strTrace.Format(TEXT("Я�����Ϊ%I64d, ����ע���Ϊ%I64d, ��ǰ��ע���Ϊ%I64d, ��עʧ��!"), lUserScore, lJettonCount, lJettonScore);

			//������Ϣ
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
			strTrace.Format(TEXT("��������Ϊ%I64d, ����ע���Ϊ%I64d, ��ǰ��ע���Ϊ%I64d, ��עʧ��!"), m_lUserLimitScore, lJettonCount, lJettonScore);
			//CTraceService::TraceString(strTrace, TraceLevel_Exception);

			//������Ϣ
			SendGameMessage(wChairID, strTrace);

			return true;
		}

		return true;
	}

	//�ɹ���ʶ
	bool bPlaceJettonSuccess=true;

	//AI����ϵͳ��ׯ
	bool bAIOrSysBanker = ((m_wCurrentBanker == INVALID_CHAIR) || (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker) && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser()));

	//�Ϸ���֤
	if (GetMaxPlayerScore(cbJettonArea, wChairID) >= lJettonScore)
	{
		//��������֤
		if(pIServerUserItem->IsAndroidUser())
		{
			//�ж�ʣ����ע���
			if (!IsRobotPlaceJettonValid(cbJettonArea, lJettonScore, wChairID))
			{
				//CTraceService::TraceString(TEXT("KKK3"), TraceLevel_Exception);

				return true;
			}

			//��Ŀ����
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

			//ͳ�Ʒ���
			if (bPlaceJettonSuccess)
			{
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;

				//������ׯ����ȡ�����˵İ���Ͳʽ�
				if (!bAIOrSysBanker)
				{
					m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaInScore += lJettonScore;

					//����ͬʱ����AI���������
					if (g_lCurAnChouRate > 0 && IsContainAIAndRealPlayer(pIServerUserItem->IsAndroidUser()))
					{
						g_lCurAnChouTotal += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
						m_lCurTurnAnChouWin += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
						g_lStatisticSysWin += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);

						m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaAnChou += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
					}

					//����ͬʱ����AI���������
					if (g_lCurMosgoldRate > 0 && IsContainAIAndRealPlayer(pIServerUserItem->IsAndroidUser()))
					{
						g_lCurMosgoldStorage += ((SCORE)(lJettonScore * g_lCurMosgoldRate) / (SCORE)100);
						g_lStatisticSysWin += ((SCORE)(lJettonScore * g_lCurMosgoldRate) / (SCORE)100);

						m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaMosgold += ((SCORE)(lJettonScore * g_lCurMosgoldRate) / (SCORE)100);
					}
				}
			}
		}
		//������ע�ۼӰ���
		else
		{
			//AI����ϵͳ��ׯ,��ȡ���˵İ���Ͳʽ�
			if (bAIOrSysBanker)
			{
				m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaInScore += lJettonScore;

				//����ͬʱ����AI���������
				if (g_lCurAnChouRate > 0 && IsContainAIAndRealPlayer(pIServerUserItem->IsAndroidUser()))
				{
					g_lCurAnChouTotal += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
					m_lCurTurnAnChouWin += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
					g_lStatisticSysWin += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);

					m_tagUserAreaInfo[wChairID][cbJettonArea].lAreaAnChou += ((SCORE)(lJettonScore * g_lCurAnChouRate) / (SCORE)100);
				}

				//����ͬʱ����AI���������
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
			//������ע
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
       
		//��������
		CMD_S_PlaceJetton PlaceJetton;
		ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

		//�������
		PlaceJetton.wChairID=wChairID;
		PlaceJetton.cbJettonArea=cbJettonArea;
		PlaceJetton.lJettonScore=lJettonScore;

		//��ȡ�û�
		IServerUserItem * pIServerUser=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUser != NULL) 
			PlaceJetton.cbAndroid = pIServerUser->IsAndroidUser()? TRUE : FALSE;

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
		
		//���������ע��Ϣ
		SendUserBetInfo(pIServerUserItem->GetGameID());
       
		if (pIServerUserItem->IsAndroidUser())
        {
            m_lCurrentRobotJettonScore += lJettonScore;
        }

		//ռλ�����������ע����
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

			//������Ϣ
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
		}
	}

	return true;
}

//�û�ռλ
bool CTableFrameSink::OnUserOccupySeat(WORD wOccupyChairID, BYTE cbOccupySeatIndex)
{
	//У���û�
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

	//ׯ�Ҳ���ռλ
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

		//��������
		m_pITableFrame->SendTableData(wOccupyChairID, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));

		return true;
	}

	ASSERT(m_wOccupySeatChairIDArray[cbOccupySeatIndex] == INVALID_CHAIR);

	//У���Ƿ��Ѿ�ռλ
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		//ռλʧ��
		if (m_wOccupySeatChairIDArray[i] == wOccupyChairID)
		{
			CMD_S_OccupySeat_Fail OccupySeat_Fail;
			ZeroMemory(&OccupySeat_Fail, sizeof(OccupySeat_Fail));

			OccupySeat_Fail.bSeatBusy = false;
			CopyMemory(OccupySeat_Fail.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
			OccupySeat_Fail.wAlreadyOccupySeatChairID = wOccupyChairID;
			OccupySeat_Fail.cbAlreadyOccupySeatIndex = i;

			//��������
			m_pITableFrame->SendTableData(wOccupyChairID, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));

			return true;
		}
	}
    //��ȡ�Զ�������
    tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
    ASSERT(pCustomConfig);

    ASSERT(m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() >= pCustomConfig->CustomGeneral.lOccupySeatFree);
    if(m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < pCustomConfig->CustomGeneral.lOccupySeatFree
       && !m_pITableFrame->GetTableUserItem(wOccupyChairID)->IsAndroidUser())
    {
		return pIServerUserItem->IsAndroidUser();
    }

    //У�������ռλ
    if(m_pITableFrame->GetTableUserItem(wOccupyChairID)->IsAndroidUser())
    {
		//���¶�ȡ���ò���(������ռλ�����������������仯)
		RectifyRobotOccupySeatPara();

		///////////////////////
		//CString strTrace;
		//strTrace.Format(TEXT("m_nRobotOccupySeatTable=%d"), m_nRobotOccupySeatTable);
		//CTraceService::TraceString(strTrace, TraceLevel_Exception);
		////////////////////////////

        //��֧�ֻ�����ռλ
        if(!m_bEnableRobotOccupySeat)
        {
            return true;
        }

		//У��ռλ����
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < pCustomConfig->CustomGeneral.lOccupySeatFree)
		{
			return true;
		}

        //У��ռλ����
        int	nCurRobotOccupySeatTable = 0;
        for(WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
        {
            if(m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
            {
                continue;
            }

            WORD wTempChairID = m_wOccupySeatChairIDArray[i];

            //������
            if(m_pITableFrame->GetTableUserItem(wTempChairID) && m_pITableFrame->GetTableUserItem(wTempChairID)->IsAndroidUser())
            {
                nCurRobotOccupySeatTable++;
            }
        }

        if(nCurRobotOccupySeatTable + 1 > m_nRobotOccupySeatTable)
        {
            return true;
        }

        //У���˳�ռλ����	
        //ASSERT(m_lRobotOccupySeatPlayCount[wOccupyChairID] == 0);
        if(m_lRobotOccupySeatPlayCount[wOccupyChairID] != 0)
        {
            m_lRobotOccupySeatPlayCount[wOccupyChairID] = 0;
        }

        //������ռλ�ɹ�
    }

	//ռλ�ɹ�
	m_wOccupySeatChairIDArray[cbOccupySeatIndex] = wOccupyChairID;
	m_bContinueNoBetCount[cbOccupySeatIndex] = 0;

	CMD_S_OccupySeat OccupySeat;
	ZeroMemory(&OccupySeat, sizeof(OccupySeat));
	CopyMemory(OccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
	OccupySeat.wOccupySeatChairID = wOccupyChairID;
	OccupySeat.cbOccupySeatIndex = cbOccupySeatIndex;

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));

	return true;
}

//����ׯ��
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

	//�Ϸ��ж�
	SCORE lUserScore = pIApplyServerUserItem->GetUserScore();
	if (lUserScore < m_lApplyBankerCondition)
	{
		SendGameMessage(pIApplyServerUserItem->GetChairID(), TEXT("��Ľ�Ҳ���������ׯ�ң�����ʧ�ܣ�"));
		return true;
	}

	//�����ж�
	WORD wApplyUserChairID = pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx = 0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];
		if (wChairID == wApplyUserChairID)
		{
			SendGameMessage(pIApplyServerUserItem->GetChairID(), TEXT("���Ѿ�������ׯ�ң�����Ҫ�ٴ����룡"));
			return true;
		}
	}


	//�����������ж�
	if (pIApplyServerUserItem->IsAndroidUser())
	{
		RectifyRobotRequestBankerPara();

		if (GetApplyUserArrayRobotCount() >= m_nRobotRequestBankerCount)
		{
			return true;
		}
	}

	//������Ϣ 
	m_ApplyUserArray.Add(wApplyUserChairID);

	//�������
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker, sizeof(ApplyBanker));

	//���ñ���
	ApplyBanker.wApplyUser = wApplyUserChairID;

	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	//�л��ж�
	if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE && m_ApplyUserArray.GetCount() == 1)
	{
		ChangeBanker(false);
	}

	return true;
}

//ȡ������
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	if (!m_bEnableBanker)
	{
		return pICancelServerUserItem->IsAndroidUser();
	}

	//��ǰׯ��
	if (pICancelServerUserItem->GetChairID() == m_wCurrentBanker && m_pITableFrame->GetGameStatus() != GAME_STATUS_FREE)
	{
		//������Ϣ
		SendGameMessage(pICancelServerUserItem->GetChairID(), TEXT("����ɹ����ȴ���Ϸ������������ׯ!"));
		m_bWaitGiveUpBanker = true;

		//������Ϣ
		m_pITableFrame->SendTableData(pICancelServerUserItem->GetChairID(), SUB_S_BANKER_QUIT_RESULT);

		return true;
	}

	//�����ж�
	for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		//��ȡ���
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		//��������
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->GetUserID() != pICancelServerUserItem->GetUserID()) continue;

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		if (m_wCurrentBanker != wChairID)
		{
			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			//���ñ���
			CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker == wChairID)
		{
			//�л�ׯ�� 
			m_wCurrentBanker = INVALID_CHAIR;
			ChangeBanker(true);
		}

		return true;
	}

	return true;
}

//�����˿�
bool CTableFrameSink::DispatchTableCard()
{
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	m_GameLogic.RandCardList(m_cbTableCardArray,sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0]));

	return true;
}

//����ׯ��
void CTableFrameSink::SendApplyUser(IServerUserItem *pRcvServerUserItem)
{
	if (!m_bEnableBanker)
	{
		return;
	}

	for (INT_PTR nUserIdx = 0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];

		//��ȡ���
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem) continue;

		//ׯ���ж�
		if (pServerUserItem->GetChairID() == m_wCurrentBanker) continue;

		//�������
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser = wChairID;

		//������Ϣ
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
}

//����ׯ��
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	if (!m_bEnableBanker)
	{
		return true;
	}

	//ֻ�п���״̬����ׯ
	if (m_pITableFrame->GetGameStatus() != GAME_STATUS_FREE)
	{
		return true;
	}

	//�л���ʶ
	bool bChangeBanker = false;

	//������ׯ�б����Ƿ�������ׯ����
	for (WORD i = 0; i<m_ApplyUserArray.GetCount(); i++)
	{
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		ASSERT(pIServerUserItem != NULL);
		if (pIServerUserItem->GetUserScore() < m_lApplyBankerCondition)
		{
			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);
		}
	}

	//ȡ����ǰ
	if (bCancelCurrentBanker)
	{
		for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			//��ȡ���
			WORD wChairID = m_ApplyUserArray[i];

			//��������
			if (wChairID != m_wCurrentBanker) continue;

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			//���ñ���
			CancelBanker.wCancelUser = wChairID;

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}

		//����ׯ��
		m_wCurrentBanker = INVALID_CHAIR;

		//�ֻ��ж�
		if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE && m_ApplyUserArray.GetCount() != 0)
		{
			m_wCurrentBanker = m_ApplyUserArray[0];
		}

		//���ñ���
		bChangeBanker = true;
		m_bExchangeBanker = true;
	}
	//��ׯ�ж�
	else if (m_wCurrentBanker != INVALID_CHAIR)
	{
		//��ȡׯ��
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

		if (pIServerUserItem != NULL)
		{
			SCORE lBankerScore = pIServerUserItem->GetUserScore();
			bool bAndroidBanker = pIServerUserItem->IsAndroidUser();

			//�����ж�
			if (m_lPlayerBankerMAX <= m_wBankerTime || lBankerScore < m_lApplyBankerCondition /*|| bAndroidBanker*/)
			{
				//ׯ�������ж� ͬһ��ׯ�������ֻ�ж�һ��
				if (/*!bAndroidBanker && */m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker && lBankerScore >= m_lApplyBankerCondition)
				{
					//��ׯ�������ã���ׯ�������趨�ľ���֮��(m_lBankerMAX)��
					//�������ֵ��������������ׯ���б�����������ҽ��ʱ��
					//�����ټ���ׯm_lBankerAdd�֣���ׯ���������á�

					//��ҳ���m_lBankerScoreMAX֮��
					//������������ҵĽ��ֵ�������Ľ��ֵ����Ҳ�����ټ�ׯm_lBankerScoreAdd�֡�
					bool bScoreMAX = true;
					m_bExchangeBanker = false;

					for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						//��ȡ���
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

				//�������
				for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
				{
					//��ȡ���
					WORD wChairID = m_ApplyUserArray[i];

					//��������
					if (wChairID != m_wCurrentBanker) continue;

					//ɾ�����
					m_ApplyUserArray.RemoveAt(i);

					break;
				}

				//����ׯ��
				m_wCurrentBanker = INVALID_CHAIR;

				//�ֻ��ж�
				if (m_ApplyUserArray.GetCount() != 0)
				{
					m_wCurrentBanker = m_ApplyUserArray[0];
				}

				bChangeBanker = true;
				m_bExchangeBanker = true;

				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				if (lBankerScore<m_lApplyBankerCondition)
					_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]��������(%I64d)��ǿ�л�ׯ!"), pIServerUserItem->GetNickName(), m_lApplyBankerCondition);
				else
					_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]��ׯ�����ﵽ(%I64d)��ǿ�л�ׯ!"), pIServerUserItem->GetNickName(), m_lPlayerBankerMAX);

				//������Ϣ
				SendGameMessage(INVALID_CHAIR, szTipMsg);
			}
		}
		else
		{
			for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
			{
				//��ȡ���
				WORD wChairID = m_ApplyUserArray[i];

				//��������
				if (wChairID != m_wCurrentBanker) continue;

				//ɾ�����
				m_ApplyUserArray.RemoveAt(i);

				break;
			}
			//����ׯ��
			m_wCurrentBanker = INVALID_CHAIR;
		}
	}
	//ϵͳ��ׯ
	else if (m_wCurrentBanker == INVALID_CHAIR && m_ApplyUserArray.GetCount() != 0)
	{
		m_wCurrentBanker = m_ApplyUserArray[0];
		bChangeBanker = true;
		m_bExchangeBanker = true;
	}

	//�л��ж�
	if (bChangeBanker)
	{
		//�����ׯ��
		m_lPlayerBankerMAX = m_lBankerMAX;

		//���ñ���
		m_wBankerTime = 0;
		m_lBankerWinScore = 0;

		m_lRobotBankerWinScore = 0;

		//������Ϣ
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

			//��ȡ��Ϣ
			int nMessageCount = 3;

			//��ȡ����
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

			//��ȡ���
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

			//������Ϣ
			_sntprintf_s(szMessage2, CountArray(szMessage2), TEXT("�� %s ����ׯ�ˣ�%s"), pIServerUserItem->GetNickName(), szMessage1);
			SendGameMessage(INVALID_CHAIR, szMessage2);

			//�жϵ�ǰׯ���Ƿ�ռλ�����ռλ���˳�
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

				//��������
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

			}
		}

		//�Ƿ�����˽��Լս����
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
						//��ȡ�Զ�������
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

				SendGameMessage(INVALID_CHAIR, TEXT("�ȴ��û���ׯ!"));

				//���ö�ʱ��
				m_dwJettonTime = 0L;
				m_pITableFrame->KillGameTimer(IDI_FREE);
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->KillGameTimer(IDI_GAME_END);
				
				//����״̬
				m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
			}
		}
	}

	return bChangeBanker;
}

//�����ע
SCORE CTableFrameSink::GetMaxPlayerScore(BYTE cbJettonArea, WORD wChairID)
{
	//��ȡ���
	IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL == pIMeServerUserItem)
	{
		return 0L;
	}
		
	//����ע��
	SCORE lNowJetton = 0l;
	for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; nAreaIndex++)
	{
		lNowJetton += m_lUserInAllScore[wChairID][nAreaIndex];
	}

	//��������
	SCORE lAreaLimit = m_lAreaLimitScore - m_lAreaInAllScore[cbJettonArea];

	//�������
	SCORE lUserLimit = m_lUserLimitScore - m_lUserInAllScore[wChairID][cbJettonArea];

	//�����ע
	SCORE lMaxJetton = 0L;

	//ׯ�ҽ��
	SCORE lBankerScore = m_lBankerScore;

	//������
	BYTE cbMultiple[AREA_MAX] = { MULTIPLE_LONG, MULTIPLE_PING, MULTIPLE_HU };

	//ׯ�����
	SCORE lBankerLostScore[AREA_MAX];
	ZeroMemory(lBankerLostScore, sizeof(lBankerLostScore));

	//��������ׯ�����
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

	//��������ׯ�����
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

	//��������ׯ�����
	for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
	{
		if (wAreaIndex == AREA_PING)
		{
			lBankerLostScore[AREA_PING] -= (m_lAreaInAllScore[wAreaIndex] * (cbMultiple[wAreaIndex]));
		}
	}

	//ׯ��������
	SCORE lMaxBankerLostScore = min(min(lBankerLostScore[AREA_LONG], lBankerLostScore[AREA_HU]), lBankerLostScore[AREA_PING]);

	lBankerScore += lMaxBankerLostScore;

	//for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex)
	//{
	//	lBankerScore = lBankerScore + m_lAreaInAllScore[nAreaIndex];
	//}

	//�����ע
	lMaxJetton = min(pIMeServerUserItem->GetUserScore() - lNowJetton, lUserLimit);

	lMaxJetton = min(lMaxJetton, lAreaLimit);

	lMaxJetton = min(lMaxJetton, (lBankerScore / cbMultiple[cbJettonArea]));

	//��������
	ASSERT(lMaxJetton >= 0);
	lMaxJetton = max(lMaxJetton, 0);

	return lMaxJetton;
}

//�������
SCORE CTableFrameSink::Calculate(BYTE cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker, BYTE cbWinAre[3])
{
    //ϵͳ��Ӯ
	SCORE lSysWinScore = 0l;

    //������Ӯ(�����ע��)
    INT nAreaWin[AREA_MAX] = {0};

    //�ƶ�����
    //��ȡ�߼�ֵ
    BYTE cbDragonLogicValue = GetCardValue(cbTableCardArray[0]);
    BYTE cbTigerLogicValue = GetCardValue(cbTableCardArray[1]);

    //��ʼ������
    for(int i = 0; i < AREA_MAX; i++)
    {
        nAreaWin[i] = RESULT_LOSE;
    }

    //�Ƚ����Ŵ�С
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
    
    //������
    BYTE cbMultiple[AREA_MAX] = {MULTIPLE_LONG, MULTIPLE_PING, MULTIPLE_HU};

    //ϵͳ��ׯ���߻����˵�ׯ
    if((wCurrentBanker == INVALID_CHAIR) || ((wCurrentBanker != INVALID_CHAIR) && m_pITableFrame->GetTableUserItem(wCurrentBanker) && m_pITableFrame->GetTableUserItem(wCurrentBanker)->IsAndroidUser()))
    {
        //ֻ�����������
        //�������
        SCORE lRealPlayerWinScore = 0;
        for(WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
        {
            //��ȡ�û�
            IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
            if(pIServerUserItem == NULL)
            {
                continue;
            }

            if(pIServerUserItem->IsAndroidUser())
            {
                continue;
            }

            //ÿ���˶�ע��
            for(WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
            {
                if(nAreaWin[wAreaIndex] == RESULT_WIN)
                {
                    lRealPlayerWinScore += (SCORE)(lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
                }
                else
                {
                    //��ע������ע��, ����ʱ����ע���.
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
    //���������ׯ
    else if((wCurrentBanker != INVALID_CHAIR) && m_pITableFrame->GetTableUserItem(wCurrentBanker) && !m_pITableFrame->GetTableUserItem(wCurrentBanker)->IsAndroidUser())
    {
        //ֻ���ǻ��������
        //�������
        SCORE lAIWinScore = 0;
        for(WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
        {
            //��ȡ�û�
            IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
            if(pIServerUserItem == NULL)
            {
                continue;
            }

            if(!pIServerUserItem->IsAndroidUser())
            {
                continue;
            }

            //ÿ���˶�ע��
            for(WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
            {
                if(nAreaWin[wAreaIndex] == RESULT_WIN)
                {
                    lAIWinScore += (SCORE)(lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
                }
                else
                {
                    //��ע������ע��, ����ʱ����ע���.
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

//����÷�
SCORE CTableFrameSink::CalculateScore(OUT SCORE& lBankerWinScore)
{
	//������Ӯ(�����ע��)
	INT nAreaWin[AREA_MAX] = {0};

	BYTE Result = 0;
	lBankerWinScore = 0;

	//�ƶ�����
	DeduceWinner(nAreaWin, Result);

	//��ҳɼ�
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	ZeroMemory(m_lUserAreaWinScore, sizeof(m_lUserAreaWinScore));

	//��������÷ֺ�����˰��
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; wAreaIndex++)
		{
			m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = 0;
			m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue = 0;
		}
	}

	//������
	BYTE cbMultiple[AREA_MAX] = { MULTIPLE_LONG, MULTIPLE_PING, MULTIPLE_HU };

	//�����мҵ÷�
	for (WORD wChairID=0; wChairID < m_wChairCount; wChairID++)
	{
		//ׯ���ж�
		if (m_wCurrentBanker == wChairID)
		{
			continue;
		}

		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		//ÿ���˶�ע��
		for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
		{
			if (nAreaWin[wAreaIndex] == RESULT_WIN && m_lUserInAllScore[wChairID][wAreaIndex] > 0)
			{
                m_lUserAreaWinScore[wChairID][wAreaIndex] = (SCORE)(m_lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));

				//�������
				//if (!bIsAndroidUser)
				{
					m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = (SCORE)(m_lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
					m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue = m_pITableFrame->CalculateRevenue(wChairID, m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);
				}
				////AI����˰
				//else
				//{
				//	m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = (SCORE)(m_lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
				//}
			}
			else if (nAreaWin[wAreaIndex] == RESULT_LOSE && m_lUserInAllScore[wChairID][wAreaIndex] > 0)
			{
				//��ע������ע��, ����ʱ����ע���.
				if ( nAreaWin[AREA_PING] == RESULT_WIN && (wAreaIndex == AREA_LONG || wAreaIndex == AREA_HU))
				{
				}
				else
				{
					m_lUserAreaWinScore[wChairID][wAreaIndex] = -m_lUserInAllScore[wChairID][wAreaIndex];

					//�������
					//if (!bIsAndroidUser)
					{
						m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = -m_lUserInAllScore[wChairID][wAreaIndex];
					}
					////AI����˰
					//else
					//{
					//	m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = -m_lUserInAllScore[wChairID][wAreaIndex];
					//}
				}
			}
		}

		//����˰��
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

	//ׯ�ҳɼ�
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		//ׯ���ж�
		if (m_wCurrentBanker != wChairID)
		{
			continue;
		}

		//��ȡ�û�
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		ASSERT(pIServerUserItem);

		if (pIServerUserItem == NULL)
		{
			continue;
		}

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		//ÿ���˶�ע��
		for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
		{
			if (nAreaWin[wAreaIndex] == RESULT_WIN && m_lAreaInAllScore[wAreaIndex] > 0)
			{
				//�������
				//if (!bIsAndroidUser)
				{
					m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = -(SCORE)(m_lAreaInAllScore[wAreaIndex] * (cbMultiple[wAreaIndex]));
				}
				//AI����˰
				//else
				//{
				//	m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = -(SCORE)(m_lAreaInAllScore[wAreaIndex] * (cbMultiple[wAreaIndex]));
				//}
			}
			else if (nAreaWin[wAreaIndex] == RESULT_LOSE && m_lAreaInAllScore[wAreaIndex] > 0)
			{
				//��ע������ע��, ����ʱ����ע���.
				if (nAreaWin[AREA_PING] == RESULT_WIN && (wAreaIndex == AREA_LONG || wAreaIndex == AREA_HU))
				{
				}
				else
				{
					//�������
					//if (!bIsAndroidUser)
					{
						m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = m_lAreaInAllScore[wAreaIndex];
						m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue = m_pITableFrame->CalculateRevenue(wChairID, m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore);
					}
					//AI����˰
					//else
					//{
					//	m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore = m_lAreaInAllScore[wAreaIndex];
					//}
				}
			}
		}

		//�ܵķ���
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

	//������ҵ÷�
    SCORE lRealPlayerWinScore = 0;
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		//��ȡ�û�
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
	//��ȡ�߼�ֵ
	BYTE cbDragonLogicValue = m_GameLogic.GetCardValue(m_cbTableCardArray[0]);
	BYTE cbTigerLogicValue = m_GameLogic.GetCardValue(m_cbTableCardArray[1]);

	//��ʼ������
	for (int i = 0; i < AREA_MAX; i++)
	{
		nAreaWin[i] = RESULT_LOSE;
	}
		
	//�Ƚ����Ŵ�С
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

//���ͼ�¼
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem, BYTE cbGameStatus)
{
	if (pIServerUserItem->IsClientReady())
	{
		CMD_S_SimpleChart SimpleChart;
		ZeroMemory(&SimpleChart, sizeof(SimpleChart));

		CopyMemory(SimpleChart.ServerGameRecordArray, m_SimpleChartArray, sizeof(m_SimpleChartArray));
		SimpleChart.cbGameStatus = cbGameStatus;

		// ������Ϣ
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_SIMPLE_CHART, &SimpleChart, sizeof(SimpleChart));

		CMD_S_AdvancedChart AdvancedChart;
		ZeroMemory(&AdvancedChart, sizeof(AdvancedChart));

		CopyMemory(AdvancedChart.ServerGameRecordArray, m_AdvancedChartArray, sizeof(m_AdvancedChartArray));
		AdvancedChart.cbGameStatus = cbGameStatus;

		// ������Ϣ
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_ADVANCED_CHART, &AdvancedChart, sizeof(AdvancedChart));
	}
}

SCORE CTableFrameSink::GameOver()
{
	//��ȡ�û���������
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
		//���ҷ������
		POSITION pos = m_listRoomCtrl.GetHeadPosition();
		while (pos)
		{
			ROOMCTRL roomctrl = m_listRoomCtrl.GetNext(pos);

			if (roomctrl.roomCtrlStatus == PROGRESSING)
			{
				//С��0����0�����
				SCORE lTempRoomCtrlSysStorage = roomctrl.lRoomCtrlCurSysStorage;
				SCORE lTempRoomCtrlPlayerStorage = roomctrl.lRoomCtrlCurPlayerStorage;

				lTempRoomCtrlSysStorage = max(0 , lTempRoomCtrlSysStorage);
				lTempRoomCtrlPlayerStorage = max(0 , lTempRoomCtrlPlayerStorage);

				//���Բ�ֵ
				lDVal = abs(lTempRoomCtrlSysStorage - lTempRoomCtrlPlayerStorage);
				lMaxCtrlStorage = max(lTempRoomCtrlSysStorage, lTempRoomCtrlPlayerStorage);
				lSysCtrlParameterK = roomctrl.lRoomCtrlParameterK;
				bSysWin = lTempRoomCtrlSysStorage > lTempRoomCtrlPlayerStorage;
				lKeyPlayerStorage = lTempRoomCtrlPlayerStorage;

				//ϵͳ����ҿ������һ������0
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
		//����ϵͳ����
		POSITION pos = m_listSysCtrl.GetHeadPosition();
		while (pos)
		{
			SYSCTRL sysctrl = m_listSysCtrl.GetNext(pos);

			if (sysctrl.sysCtrlStatus == PROGRESSINGEX)
			{
				//С��0����0�����
				SCORE lTempCurSysStorage = sysctrl.lSysCtrlCurSysStorage;
				SCORE lTempCurPlayerStorage = sysctrl.lSysCtrlCurPlayerStorage;

				lTempCurSysStorage = max(0 , lTempCurSysStorage);
				lTempCurPlayerStorage = max(0 , lTempCurPlayerStorage);

				//���Բ�ֵ
				lDVal = abs(lTempCurSysStorage - lTempCurPlayerStorage);
				lMaxCtrlStorage = max(lTempCurSysStorage, lTempCurPlayerStorage);
				lSysCtrlParameterK = sysctrl.lSysCtrlParameterK;
				bSysWin = lTempCurSysStorage > lTempCurPlayerStorage;
				lKeyPlayerStorage = lTempCurPlayerStorage;

				//ϵͳ����ҿ������һ������0
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
		//��������ж�
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

	//ϵͳӮ��־�� INVALID_BYTEΪ�Ѷ�50���
	BYTE cbSysWin = INVALID_BYTE;
	BYTE cbRandVal = rand() % 100;

	cbSysWin = cbRandVal < wUserWinChance ? FALSE : TRUE;

	//���˿���
	bool bDebug = false;
	BYTE cbWinSideDebug = 0xFF;
	bool bNeedDebug = false;

	if (m_pServerDebug != NULL)
	{
		bNeedDebug = m_pServerDebug->NeedDebug(cbWinSideDebug);
	}

	//�������У�����
	if (bNeedDebug && (((cbWinSideDebug == AREA_SYSLOST || cbWinSideDebug == AREA_SYSWIN) && IsAreaCtrlSysValid()) || (cbWinSideDebug != AREA_SYSLOST && cbWinSideDebug != AREA_SYSWIN)) 
	&& m_pServerDebug->DebugResult(m_cbTableCardArray, m_lUserInAllScore, m_wCurrentBanker))
	{
		//�������
		bDebug = true;

		//����������Ƽ�¼
		POSITION pos = m_listAreaCtrl.GetHeadPosition();
		while (pos)
		{
			AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

			if (areactrl.areaCtrlStatus == AREACTRL_PROGRESSING)
			{
				areactrl.cbCurCtrlTimes++;

				//������ҵ÷�
				SCORE lRealPlayerWinScore = CalculateScore(lBankerWinScore);
				areactrl.lAreaCtrlSysWin -= lRealPlayerWinScore;

				if (areactrl.cbCurCtrlTimes == areactrl.cbConfigCtrlTimes)
				{
					areactrl.areaCtrlStatus = AREACTRL_FINISH;
				}

				//���ͽ��
				CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
				ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
				CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
				CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
				CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				CurAreaCtrlInfo.lAreaCtrlSysWin = CalListAreaCtrlSysWin();

				//������Ϣ
				m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				//����ͳ��
				g_lStatisticSysWin -= lRealPlayerWinScore;

				//���·������
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

		//�ɽ���ʶ(����)
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

			//�ɷ��˿�
			DispatchTableCard();

			//��̽���ж�
			//������ҵ÷�
			SCORE lRealPlayerWinScore = CalculateScore(lBankerWinScore);

			//������Ӯ(�����ע��)
			INT nAreaWin[AREA_MAX] = { 0 };

			BYTE cbResult = 0;

			//�ƶ�����
			DeduceWinner(nAreaWin, cbResult);

			//�ɽ�(����)
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

				//���������Ǯ
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

	//ϵͳ�ͷ������
	if (!bDebug)
	{
		//���µ��Խ��
		UpdateCtrlRes(userCtrlType);
	}
    else
    {
        // ������Ϣ
        m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_AREACTRL_TRACELOG);

        //��������б�
        POSITION pos = m_listAreaCtrl.GetHeadPosition();
        while(pos)
        {
            AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

			//���ͽ��
			CMD_S_AreaCtrlTraceLog areaCtrlResult;
			ZeroMemory(&areaCtrlResult, sizeof(areaCtrlResult));

			CopyMemory(&(areaCtrlResult.AreaCtrlTraceLog), &areactrl, sizeof(areactrl));

            // ������Ϣ
			m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_AREACTRL_TRACELOG, &areaCtrlResult, sizeof(areaCtrlResult));
        }
    }

	CMD_S_StatisticService StatisticService;
	ZeroMemory(&StatisticService, sizeof(StatisticService));

	StatisticService.lStatisticSysWin = g_lStatisticSysWin;
	StatisticService.lStatisticService = g_lStatisticService;
	StatisticService.lAreaLimitScore = m_lAreaLimitScore;
	StatisticService.wChairCount = m_wChairCount;

	// ������Ϣ
	m_pITableFrame->SendRoomData(NULL, SUB_S_STATISTIC_SERVICE, &StatisticService, sizeof(StatisticService));

	CMD_S_AnChou AnChou;
	ZeroMemory(&AnChou, sizeof(AnChou));

	AnChou.lCurAnChouRate = g_lCurAnChouRate;
	AnChou.lCurAnChouTotal = g_lCurAnChouTotal;

	//������Ϣ
	m_pITableFrame->SendRoomData(NULL, SUB_S_ANCHOU, &AnChou, sizeof(AnChou));

	CMD_S_Mosgold Mosgold;
	ZeroMemory(&Mosgold, sizeof(Mosgold));

	Mosgold.lCurMosgoldStorage = g_lCurMosgoldStorage;
	Mosgold.lCurMosgoldRate = g_lCurMosgoldRate;
	Mosgold.lCurMosgoldDispatchCon = g_lCurMosgoldDispatchCon;
	Mosgold.lCurMosgoldDispatchRate = g_lCurMosgoldDispatchRate;
	Mosgold.lCurMosgoldStorageRate = g_lCurMosgoldStorageRate;

	//������Ϣ
	m_pITableFrame->SendRoomData(NULL, SUB_S_MOSGOLD, &Mosgold, sizeof(Mosgold));

	//��Ϸ��¼
	tagServerGameRecord GameRecord;
	ZeroMemory(&GameRecord, sizeof(GameRecord));

	GameRecord.cbLong = (m_cbTableCardArray[0]&0x0F);
	GameRecord.cbHu = (m_cbTableCardArray[1]&0x0F);

	//���·����¼(�����20��ʤ������������)
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

	//�߼�·����¼(��ʾ��� 78�֣����������϶�   ����100�����)
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

	//����
	m_lCurGameCount++;

	return lBankerWinScore;
}
//��ȡ����
void CTableFrameSink::ReadConfigInformation(bool bReadFresh)
{
	//��ȡ�Զ�������
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
	ASSERT(pCustomConfig);

	m_bEnableBanker = pCustomConfig->bEnableBanker;
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_lBankerMAX = pCustomConfig->CustomGeneral.lBankerTime;
	m_lBankerAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lBankerScoreMAX = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_lBankerScoreAdd = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker =  true;

	//ʱ��
	m_cbFreeTime = pCustomConfig->CustomGeneral.cbFreeTime;
	m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
	m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
	if(m_cbFreeTime < TIME_FREE	|| m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
	if(m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
	if(m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;

	//��ע
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

    //���������ռλ����
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

		//����ϵͳ�����б�
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

		//�����б�
		m_listSysCtrl.AddTail(sysctrl);

		//��ʼ�󽱿������
		g_lCurMosgoldStorage += lTempChouMosgoldStorage;
		g_lStatisticSysWin += lTempChouMosgoldStorage;
	}
}

//����д��Ϣ
void CTableFrameSink::WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString )
{
	//������������
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	DWORD dwAttrib = GetFileAttributes(_T(".//��������־Ŀ¼"));
	if (INVALID_FILE_ATTRIBUTES == dwAttrib)
	{
		CreateDirectory(_T(".//��������־Ŀ¼"), NULL);
	}

	CTime time = CTime::GetCurrentTime();
	static CString strFile;
	if (strFile.IsEmpty())
		strFile.Format(_T(".//��������־Ŀ¼//[%s]��־%d-%d-%d-%02d%02d%02d.log"), m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	CStdioFile myFile;
	BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		if (myFile.GetLength() > 50 * 1024 * 1024)//50M���ļ� 
		{
			myFile.Close();
			strFile.Format(_T(".//��������־Ŀ¼//[%s]��־%d-%d-%d-%02d%02d%02d.log"), m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
			if (bOpen == false)
				return;
		}

		CString strtip;
		strtip.Format(TEXT("�����%d,%d/%d/%d_%d:%d:%d--%s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pszString);

		myFile.SeekToEnd();
		myFile.WriteString(strtip);
		myFile.Flush();
		myFile.Close();
	}

	//��ԭ�����趨
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}

void CTableFrameSink::SendUserBetInfo(DWORD dwCurAllScoreGameID)
{
	//��������
	CMD_S_SendUserBetInfo SendUserBetInfo;
	ZeroMemory(&SendUserBetInfo, sizeof(SendUserBetInfo));

	SendUserBetInfo.dwCurAllScoreGameID = dwCurAllScoreGameID;
	CopyMemory(SendUserBetInfo.lUserStartScore, m_lUserStartScore, sizeof(m_lUserStartScore));
	CopyMemory(SendUserBetInfo.lUserJettonScore, m_lUserInAllScore, sizeof(m_lUserInAllScore));
	CopyMemory(SendUserBetInfo.lAreaInAllScore, m_lAreaInAllScore, sizeof(m_lAreaInAllScore));

	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
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
		//��ȡ�û�
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

	//������Ϣ	
	m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, &SendUserBetInfo, sizeof(SendUserBetInfo));
}

//���ͷ����û���Ϣ
void CTableFrameSink::SendRoomUserInfo()
{
	CMD_S_RoomUserInfo RoomUserInfo;
	memset(RoomUserInfo.dwGameID, INVALID_DWORD, sizeof(RoomUserInfo.dwGameID));
	ZeroMemory(RoomUserInfo.szNickName, sizeof(RoomUserInfo.szNickName));

	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}

		RoomUserInfo.dwGameID[i] = pIServerUserItem->GetGameID();
		CopyMemory(RoomUserInfo.szNickName[i], pIServerUserItem->GetNickName(), sizeof(RoomUserInfo.szNickName[i]));
	}

	//������Ϣ	
	m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_ROOM_USER_INFO, &RoomUserInfo, sizeof(RoomUserInfo));
}

//���¿��ƽ��
void CTableFrameSink::UpdateCtrlRes(EM_CTRL_TYPE emCtrlType)
{
	//������Ӯ(�����ע��)
	INT nAreaWin[AREA_MAX] = { 0 };

	BYTE cbResult = 0;

	//�ƶ�����
	DeduceWinner(nAreaWin, cbResult);

	//AI����ϵͳ��ׯ
	bool bAIOrSysBanker = ((m_wCurrentBanker == INVALID_CHAIR) || (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker) && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser()));

	switch (emCtrlType)
	{
	case ROOM_CTRL:
	{
		//���ҷ������
		POSITION pos = m_listRoomCtrl.GetHeadPosition();

		//��ϵͳ���Ϊ����,�ﵽ��ʼֵ�Ŀ�����ñ���,���ÿ��
		bool bResetStorage = false;

		while (pos)
		{
			ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

			if (roomctrl.roomCtrlStatus == PROGRESSING)
			{
				for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
				{
					//��ȡ�û�
					IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem == NULL)
					{
						continue;
					}

					//���˵�ׯ�����˲��߿��
					//AI����ϵͳ��ׯ,AI���߿��
					if ((!bAIOrSysBanker && !pIServerUserItem->IsAndroidUser()) || (bAIOrSysBanker && pIServerUserItem->IsAndroidUser()))
					{
						continue;
					}
						
					//ÿ���˶�ע��
					for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
					{
						//����
						if (!pIServerUserItem->IsAndroidUser())
						{
							//������Ӯ��
							if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore > 0)
							{
								//��ƽ�Ӵ󽱿������۳�
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
							//���������
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
							//������Ӯ��
							if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore > 0)
							{
								//��ƽ�Ӵ󽱿������۳�
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
							//���������
							else if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore < 0)
							{
								roomctrl.lRoomCtrlCurPlayerStorage -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);

								roomctrl.lRoomCtrlSysWin -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
								g_lStatisticSysWin -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
							}
						}
					}
				}

				//ͳ��˰��
				for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
				{
					//��ȡ�û�
					IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem == NULL || pIServerUserItem->IsAndroidUser())
					{
						continue;
					}

					//ÿ���˶�ע��
					for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
					{
						//����ͳ��˰��
						g_lStatisticService += m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaRevenue;
					}
				}

				if (roomctrl.lRoomCtrlSysStorResetRate > 0 && roomctrl.lRoomCtrlCurSysStorage < 0 && (-roomctrl.lRoomCtrlCurSysStorage) >(roomctrl.lRoomCtrlConfigSysStorage * roomctrl.lRoomCtrlSysStorResetRate / 100))
				{
					bResetStorage = true;
				}

				//�������
				if ((roomctrl.lRoomCtrlCurSysStorage <= 0 && roomctrl.lRoomCtrlCurPlayerStorage <= 0) || bResetStorage)
				{
					//������÷����¼״̬
					roomctrl.roomCtrlStatus = FINISH;
				}
				else
				{
					//���ͽ��
					CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
					ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

					CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
					CurRoomCtrlInfo.lRoomCtrlCurSysStorage = roomctrl.lRoomCtrlCurSysStorage;
					CurRoomCtrlInfo.lRoomCtrlCurPlayerStorage = roomctrl.lRoomCtrlCurPlayerStorage;
					CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
					CurRoomCtrlInfo.lRoomCtrlSysStorResetRate = roomctrl.lRoomCtrlSysStorResetRate;			
					CurRoomCtrlInfo.lRoomCtrlSysWin = CalListRoomCtrlSysWin();
					
					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
				}

				break;
			}
		}

		// ������Ϣ
		m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL_TRACELOG);
		pos = m_listRoomCtrl.GetHeadPosition();
		while (pos)
		{
			ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

			//���ͽ��
			CMD_S_RoomCtrlTraceLog roomCtrlResult;
			ZeroMemory(&roomCtrlResult, sizeof(roomCtrlResult));

			CopyMemory(&(roomCtrlResult.RoomCtrlTraceLog), &roomctrl, sizeof(roomctrl));

			// ������Ϣ
			m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL_TRACELOG, &roomCtrlResult, sizeof(roomCtrlResult));
		}

		//��ǰ�����б���Ϣ
		m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_CUR_ROOMCTRL_INFO);
		pos = m_listRoomCtrl.GetHeadPosition();
		while (pos)
		{
			ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

			if (roomctrl.roomCtrlStatus == QUEUE || roomctrl.roomCtrlStatus == PROGRESSING)
			{
				//���ͽ��
				CMD_S_ListCurRoomCtrlInfo ListCurRoomCtrlInfo;
				ZeroMemory(&ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));

				CopyMemory(&(ListCurRoomCtrlInfo.curRoomCtrlListInfo), &roomctrl, sizeof(roomctrl));

				// ������Ϣ
				m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_CUR_ROOMCTRL_INFO, &ListCurRoomCtrlInfo, sizeof(ListCurRoomCtrlInfo));
			}
		}

		break;
	}
	case SYS_CTRL:
	{
		//���Ӯ
		//��ֵ
        static SCORE lDivPlayerStorage = 0;
        static SCORE lDivSysStorage = 0;

		//��ϵͳ���Ϊ����,�ﵽ��ʼֵ�Ŀ�����ñ���,���ÿ��
		bool bResetStorage = false;

		//����ϵͳ����
		POSITION pos = m_listSysCtrl.GetHeadPosition();

		while (pos)
		{
			SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

			if (sysctrl.sysCtrlStatus == PROGRESSINGEX)
			{
				for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
				{
					//��ȡ�û�
					IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem == NULL)
					{
						continue;
					}

					//���˵�ׯ�����˲��߿��
					//AI����ϵͳ��ׯ,AI���߿��
					if ((!bAIOrSysBanker && !pIServerUserItem->IsAndroidUser()) || (bAIOrSysBanker && pIServerUserItem->IsAndroidUser()))
					{
						continue;
					}

					//ÿ���˶�ע��
					for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
					{
						//����
						if (!pIServerUserItem->IsAndroidUser())
						{
							//������Ӯ��
							if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore > 0)
							{
								//��ƽ�Ӵ󽱿������۳�
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
							//���������
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
							//������Ӯ��
							if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore > 0)
							{
								//��ƽ�Ӵ󽱿������۳�
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
							//���������
							else if (m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore < 0)
							{
								sysctrl.lSysCtrlCurPlayerStorage -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);

								sysctrl.lSysCtrlSysWin -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
								g_lStatisticSysWin -= (-m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaWinScore + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaAnChou + m_tagUserAreaInfo[wChairID][wAreaIndex].lAreaMosgold);
							}
						}	
					}
				}

				//ͳ��˰��
				for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
				{
					//��ȡ�û�
					IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem == NULL || pIServerUserItem->IsAndroidUser())
					{
						continue;
					}

					//ÿ���˶�ע��
					for (WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
					{
						//����ͳ��˰��
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

				//�������
				if ((sysctrl.lSysCtrlCurSysStorage <= 0 && sysctrl.lSysCtrlCurPlayerStorage <= 0) || bResetStorage)
				{
					//�����ۼ�
					sysctrl.lSysCtrlResetCount++;

					//�ϴ�û���
					if (sysctrl.lSysCtrlChouMosgoldStorage == 0)
					{
						SCORE lTempChouMosgoldStorage = sysctrl.lSysCtrlConfigPlayerStorage * g_lCurMosgoldStorageRate / 100;
						sysctrl.lSysCtrlConfigPlayerStorage -= lTempChouMosgoldStorage;
						sysctrl.lSysCtrlChouMosgoldStorage = lTempChouMosgoldStorage;
					}

					//�󽱿������
					g_lCurMosgoldStorage += sysctrl.lSysCtrlChouMosgoldStorage;
					g_lStatisticSysWin += sysctrl.lSysCtrlChouMosgoldStorage;

					sysctrl.lSysCtrlCurSysStorage = sysctrl.lSysCtrlConfigSysStorage;
					sysctrl.lSysCtrlCurPlayerStorage = sysctrl.lSysCtrlConfigPlayerStorage;

					sysctrl.lSysCtrlCurSysStorage -= lDivSysStorage;
					sysctrl.lSysCtrlCurPlayerStorage -= lDivPlayerStorage;

					//����һ�β�����Ϊ0
					if (sysctrl.lSysCtrlCurSysStorage < 0)
					{
						sysctrl.lSysCtrlCurSysStorage = 0;
					}

					if (sysctrl.lSysCtrlCurPlayerStorage < 0)
					{
						sysctrl.lSysCtrlCurPlayerStorage = 0;
					}

					//��ֵ��ʼ��
					lDivPlayerStorage = 0;
					lDivSysStorage = 0;

					// ������Ϣ
					CMD_S_DebugText DebugText;
					ZeroMemory(&DebugText, sizeof(DebugText));

					_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("��ǰ���������,���ô���%I64d"), sysctrl.lSysCtrlResetCount);

					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUB_S_DEBUGTEXT_GENERAL, &DebugText, sizeof(DebugText));
				}

				//��������
				CMD_S_CurSysCtrlInfo CurSysCtrlInfo;
				ZeroMemory(&CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

				CurSysCtrlInfo.lSysCtrlCurSysStorage = sysctrl.lSysCtrlCurSysStorage;
				CurSysCtrlInfo.lSysCtrlCurPlayerStorage = sysctrl.lSysCtrlCurPlayerStorage;
				CurSysCtrlInfo.lSysCtrlParameterK = sysctrl.lSysCtrlParameterK;
				CurSysCtrlInfo.lSysCtrlSysStorResetRate = sysctrl.lSysCtrlSysStorResetRate;
				CurSysCtrlInfo.lSysCtrlResetCount = sysctrl.lSysCtrlResetCount;
				CurSysCtrlInfo.lSysCtrlSysWin = CalListSysCtrlSysWin();

				//��������
				m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_SYSCTRL_INFO, &CurSysCtrlInfo, sizeof(CurSysCtrlInfo));

				//ͨ�ÿ������б�
				pos = m_listSysCtrl.GetHeadPosition();
				while (pos)
				{
					SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

					//���ͽ��
					CMD_S_SysCtrlTraceLog sysCtrlResult;
					ZeroMemory(&sysCtrlResult, sizeof(sysCtrlResult));

					CopyMemory(&(sysCtrlResult.SysCtrlTraceLog), &sysctrl, sizeof(sysctrl));

					// ������Ϣ
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

    // ������Ϣ
	m_pITableFrame->SendRoomData(NULL, SUB_S_STARTRECORD_TRACELOG, &StartRecordTraceLog, sizeof(StartRecordTraceLog));
}

//���ҷ������
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

//������Ϣ
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	CMD_S_SysTip SysTip;
	ZeroMemory(&SysTip, sizeof(SysTip));

	CString strTip;
	strTip.Format(TEXT("%s"), pszTipMsg);

	CopyMemory(SysTip.szTip, strTip, sizeof(TCHAR)* strTip.GetLength());

	if (wChairID == INVALID_CHAIR)
	{
		//��Ϸ���
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

//���������ռλ����
void CTableFrameSink::RectifyRobotOccupySeatPara()
{
	//��ȡ�Զ�������
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
	ASSERT(pCustomConfig);

	//m_bEnableRobotOccupySeat = (pCustomConfig->CustomAndroid.nEnableRobotOccupySeat == TRUE ? true : false);

	//LONGLONG lRobotOccupySeatTableMin = pCustomConfig->CustomAndroid.lRobotOccupySeatTableMin;
	//LONGLONG lRobotOccupySeatTableMax = pCustomConfig->CustomAndroid.lRobotOccupySeatTableMax;
	//m_nRobotOccupySeatTable = rand() % (lRobotOccupySeatTableMax - lRobotOccupySeatTableMin + 1) + lRobotOccupySeatTableMin;

	//LONGLONG lRobotQuitSeatTurnMin = pCustomConfig->CustomAndroid.lRobotQuitSeatTurnMin;
	//LONGLONG lRobotQuitSeatTurnMax = pCustomConfig->CustomAndroid.lRobotQuitSeatTurnMax;
	//m_nRobotQuitSeatTurn = rand() % (lRobotQuitSeatTurnMax - lRobotQuitSeatTurnMin + 1) + lRobotQuitSeatTurnMin;

	//��ȡ������
	m_bEnableRobotOccupySeat = true;
	m_nRobotQuitSeatTurn = RANDVAL(m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatTurnCountMin, m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatTurnCountMax);

	//������ռλ�����������������仯
	m_nRobotOccupySeatTable = GetRandRobotOccupySeatCount();
	m_nRobotOccupySeatTable = (m_nRobotOccupySeatTable == INVALID_DWORD ? (rand() % MAX_OCCUPY_SEAT_COUNT) : m_nRobotOccupySeatTable);
}

//��ȥׯ�Һ�ռ����Һ��Լ�����Ч���
bool CTableFrameSink::FilterUser(WORD wMeChairID, WORD wChairID)
{
    //����ׯ�Һ�ռ����Һ��Լ�
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

//��ȡ��ҵ�������
EM_CTRL_TYPE CTableFrameSink::GetUserCtrlType()
{
	//�����ȼ���ϵͳ=����>����>�շű�
	//Ĭ���շű����
	EM_CTRL_TYPE ctrlType = SYS_CTRL;

	//���ҷ������
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

	//���õ�һ����¼Ϊ����״̬
	if (firstQueueCtrlPos)
	{
		ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);
		roomctrl.roomCtrlStatus = PROGRESSING;

		return ROOM_CTRL;
	}

	return SYS_CTRL;
}

//��ȡAIConfig
void CTableFrameSink::ReadAIConfig()
{
	//���ζ�ȡ
	return;

	BYTE cbValid = ReadFileValue(TEXT("GeneralConfig"), TEXT("Valid"), 0);

	if (cbValid == FALSE)
	{
		return;
	}

	//��ȡͨ������
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

	//�����˴�ȡ
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

//��ȡ����
SCORE CTableFrameSink::ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH] = TEXT("");
	_sntprintf_s(szServerInfoPath, CountArray(szServerInfoPath), TEXT("%s\\AIConfig\\AIConfig_SERVERID%d.ini"), szDirectory, m_pGameServiceOption->wServerID);

	//��ȡ����
	UINT nValue = GetPrivateProfileInt(pszKeyName, pszItemName, nDefault, szServerInfoPath);

	return (SCORE)nValue;
}

//��ȡ������Ҹ���
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

//��ȡ������ռλ����
LONG CTableFrameSink::GetRandRobotOccupySeatCount()
{
	//��ȡ������Ҹ���
	LONG lRealPlayerCount = GetRealPlayerCount();

	//���ݴ�СУ��
	DWORD lTempArray[4] = { m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[2][1]
	};

	//��ȡ��Χ
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

//��ȡ��������ׯ����
LONG CTableFrameSink::GetRandRobotRequestBankerCount()
{
	//��ȡ������Ҹ���
	LONG lRealPlayerCount = GetRealPlayerCount();

	//���ݴ�СУ��
	DWORD lTempArray[4] = { m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[2][1]
	};

	//��ȡ��Χ
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

//��ȡ������֮��������
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

//�����������ׯ����
void CTableFrameSink::RectifyRobotRequestBankerPara()
{
	m_nRobotRequestBankerCount = GetRandRobotRequestBankerCount();
	m_nRobotRequestBankerCount = (m_nRobotRequestBankerCount == INVALID_DWORD ? 4 : m_nRobotRequestBankerCount);

	m_lRobotLostScoreLimitQB = m_CustomAIConfig.AIRequestBankerConfig.lAILostScoreLimitQB;
	m_wRobotMaxSerialTurn = m_CustomAIConfig.AIRequestBankerConfig.wMaxSerialTurn;
}

//��ȡ�����б��������Ŀ
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

//У���������ע�Ƿ���Ч
bool CTableFrameSink::IsRobotPlaceJettonValid(BYTE cbJettonArea, SCORE lJettonScore, WORD wChairID)
{
	CTime tmCur = CTime::GetCurrentTime();
	CTimeSpan tmSpan = tmCur - m_tmStartPlaceJetton;
	LONGLONG lElaseTm = tmSpan.GetTotalSeconds();

	//��ȡ��ע���ֵ
	LONG lJettonLitPercent = GetPlaceJettonLitPercent(lElaseTm);

	//ʣ���������ע����
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

//��ȡ��ע���ֵ
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

//����ͬʱ����AI���������
bool CTableFrameSink::IsContainAIAndRealPlayer(bool bCurPlaceAndroidUser)
{
	//������ׯ��		û�л�������ע		 ����
	//ϵͳ��ׯ��		û��������ע         ����
	//��������ׯ��		û��������ע         ����
	//����������鰵��Ͳʽ�

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

//���������Ч
bool CTableFrameSink::IsAreaCtrlSysValid()
{
	//������ׯ������������
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

//ͳ�Ʒ�������б�
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

//ͳ�Ʒ�������б�
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

//ͳ����������б�
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

//�Ƿ�����˽��Լս����
bool CTableFrameSink::IsGroupPersonalRoom()
{
	return (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0) && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule);
}

//��ȡ�����˷������
void CTableFrameSink::GetGroupPersonalRoomRule()
{
	if (!IsGroupPersonalRoom())
	{
		return;
	}

	//��ȡ�����˽��Լս����
	BYTE *pGameRule = m_pITableFrame->GetGameRule();

	if (pGameRule[0] != 1)
	{
		return;
	}

	//������ע����(8�ֽ�)
	m_lUserLimitScore = *((LONGLONG *)&pGameRule[1]);

	//��ׯ����(8�ֽ�)
	m_lApplyBankerCondition = *((LONGLONG *)&pGameRule[1 + sizeof(LONGLONG)]);

	//��ׯ����(2�ֽ�)
	m_lBankerMAX = *((WORD *)&pGameRule[1 + sizeof(LONGLONG)* 2]);
}

//////////////////////////////////////////////////////////////////////////////////
