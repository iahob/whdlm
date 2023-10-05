#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////

//����ʱ��
#define IDI_FREE					1									//����ʱ��
#define TIME_FREE					10									//����ʱ��

//��עʱ��
#define IDI_BET						2									//��עʱ��
#ifdef _DEBUG
	#define TIME_BET				20									//��עʱ��
#else
	#define TIME_BET				30									//��עʱ��
#endif 


//����ʱ��
#define IDI_END						3									//����ʱ��
#define TIME_END					17									//����ʱ��


LONGLONG		CTableFrameSink::m_lStorageStart = 0L;
LONGLONG		CTableFrameSink::m_lStorageDeduct = 1L;

LONGLONG         g_lChipArray[6] = { 1, 2, 5, 10, 20, 50 };

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��ע��
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory(m_HPlayBet,sizeof(m_HPlayBet));
	ZeroMemory(m_FPlayBet,sizeof(m_FPlayBet));
	ZeroMemory(m_APlayBet,sizeof(m_APlayBet));
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

	//��ҷ���
	ZeroMemory(m_dwDrawStartTime, sizeof(m_dwDrawStartTime));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	ZeroMemory(m_lPlayOriginalScore, sizeof(m_lPlayOriginalScore));

	//����
	ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
	ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
	ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
	ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

	//ת��·��
	m_bTurnTwoTime = FALSE;
	ZeroMemory(m_nTurnTableTarget, sizeof(m_nTurnTableTarget));

	//ת�̱���
	m_nLastMultipleIndex = 0;
	ZeroMemory(m_nAnimalMultiple, sizeof(m_nAnimalMultiple));
	
	//���ɼ���
	ZeroMemory(m_nGenerationChance, sizeof(m_nGenerationChance));

	//״̬����
	m_dwBetTime=0L;

	//ʱ������
	m_cbFreeTime = TIME_FREE;
	m_cbBetTime = TIME_BET;
	m_cbEndTime = TIME_END;

	//�л�����
	m_lAddScore[0]=1000;
	m_lAddScore[1]=10000;
	m_lAddScore[2]=100000;
	m_lAddScore[3]=500000;
	//���ñ���
	m_lAreaLimitScore = 0;
	m_lPlayLimitScore = 0;

	//�������
	m_hInst = NULL;
	m_pServerContro = NULL;
	m_bControl = FALSE;
	m_hInst = LoadLibrary(TEXT("SharkBattleServerDebug.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}

	m_bIsCheat = false;
	m_lRobotloseMax = 0;
	m_lRobotLoseMaxChange = 0;
	m_btRobotLoseChangePercent = 0;
	m_lRealBankerRobotLoseMax = 0;
	m_lRobotProbabilityTanking = 0;
	m_lRobotDefineTanking = 0;
	m_btRobotTankingPercent = 0;
	m_lRealBankerRobotLoseMaxChange = 0;
	m_btRealBankerRobotLoseChangePercent = 0;
	m_btUsuallyChangePercent = 0;
	m_btRobotWinScorePercent = 0;
	m_lRobotWinChiefScore = 0;
	m_lRobotResultScore = 0;
	m_lRobotAchievement = 0;

	m_IsCheatUser = FALSE;
	m_dwUserID = 0;
	m_dwUserLoseWinPercent = 0;
	m_bLoseWin = FALSE;
	m_lWinMaxGold = 0;
	m_lLoseMaxGold = 0;
	m_lAllUserGold = 0;
	m_lUserScore = 0;

	return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
}
//�ͷŶ���
VOID  CTableFrameSink::Release()
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
	//��ѯ�ӿ�
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	//�����ж�
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"),TraceLevel_Exception);
		return false;
	}

	//��ȡ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	//�����ļ���
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\SharkBattleControl\\SharkBattle.ini"),szPath);
	_sntprintf(m_szConfigFilePath,sizeof(m_szConfigFilePath),TEXT("%s\\SharkBattleControl\\User.ini"),szPath);
	_sntprintf(m_szGameRoomName, sizeof(m_szGameRoomName), TEXT("%d"),m_pGameServiceOption->wServerID);

	ReadConfigInformation();

	return true;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	WriteGameINI();
	m_lRobotAchievement = 0;
	m_lUserScore = 0;

	//��ע��
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory(m_HPlayBet,sizeof(m_HPlayBet));
	ZeroMemory(m_FPlayBet,sizeof(m_FPlayBet));
	ZeroMemory(m_APlayBet,sizeof(m_APlayBet));
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

	//����
	ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
	ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
	//ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
	ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

	//ת��·��
	m_bTurnTwoTime = FALSE;
	ZeroMemory(m_nTurnTableTarget, sizeof(m_nTurnTableTarget));

	//ת�̱���
	//m_nLastMultipleIndex = 0;
	ZeroMemory(m_nAnimalMultiple, sizeof(m_nAnimalMultiple));

	m_bControl = FALSE;

	return;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL://��������
		{
			//������Ϸ
			int nPrizesMultiple = GameOver();

			//���ͽ�����Ϣ
			CMD_S_GameEnd CMDGameEnd;
			ZeroMemory(&CMDGameEnd, sizeof(CMDGameEnd));
			CMDGameEnd.cbTimeLeave = m_cbEndTime;
			CMDGameEnd.bTurnTwoTime = m_bTurnTwoTime;
			CMDGameEnd.nPrizesMultiple = nPrizesMultiple;
			CMDGameEnd.lPlayShowPrizes = rand()*85 + 15;

			//����
			CMDGameEnd.lServiceScore = 0;

			CopyMemory(CMDGameEnd.nTurnTableTarget, m_nTurnTableTarget, sizeof(CMDGameEnd.nTurnTableTarget));
			for (int i = 0; i < GAME_PLAYER; ++i )
			{
				IServerUserItem* pServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if ( pServerUserItem == NULL || !pServerUserItem->IsClientReady() )
					continue;

				CMDGameEnd.lPlayWin[0] = m_lPlayWinScore[i][0];
				CMDGameEnd.lPlayWin[1] = m_lPlayWinScore[i][1];
				CMDGameEnd.lPlayPrizes = m_lPlayWinPrizes[i];
				//����
				CMDGameEnd.lGameScore = pServerUserItem->GetUserScore();
				m_pITableFrame->SendUserItemData(pServerUserItem, SUB_S_GAME_END, &CMDGameEnd, sizeof(CMDGameEnd));
			}

			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END, &CMDGameEnd, sizeof(CMDGameEnd));

			return true;
		}
	case GER_NETWORK_ERROR:		//�����ж�
	case GER_USER_LEAVE:		//�û��뿪
		{
			//�ǽ���״̬�� ������ע���
			if (m_pITableFrame->GetGameStatus() != GAME_SCENE_END)
			{
				//������Ϣ
				CMD_S_BetClear CMDBetClear;
				CMDBetClear.wChairID = wChairID;
				//_tcscpy(CMDBetClear.szPlayerName, pIServerUserItem->GetUserInfo()->szNickName);
				CopyMemory(CMDBetClear.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDBetClear.lPlayBet));

				//�����ע
				BOOL bSendMessage = FALSE;
				for ( int i = 0; i < ANIMAL_MAX; ++i )
				{
					if ( m_lPlayBet[wChairID][i] > 0 )
					{
						bSendMessage = TRUE;
						m_lAllBet[i] -= m_lPlayBet[wChairID][i];
						m_lPlayBet[wChairID][i] = 0;
					}
				}

				//������Ϣ
				if ( bSendMessage )
				{
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
				}
				return true;
			}

			//�۳�����
			m_lPlayScore[wChairID] += m_lPlayWinScore[wChairID][0]+m_lPlayWinScore[wChairID][1];

			if ( m_lPlayScore[wChairID] < 0 )
			{
				ASSERT(FALSE);
				m_lPlayScore[wChairID] = 0;
			}

			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if(pIServerUserItem != NULL)
				UpdataScore(wChairID, pIServerUserItem);
	
			return true;
		}
	}
	return false;
}

void CTableFrameSink::UpdataScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{

	if(pIServerUserItem == NULL) return;
	//д��
	LONGLONG lScore = m_lPlayScore[wChairID] - m_lPlayOriginalScore[wChairID];

	m_lPlayOriginalScore[wChairID] = m_lPlayScore[wChairID];

	if ( lScore != 0 )
	{
		//��ֹ����
		if ( lScore < 0 && (-lScore) > pIServerUserItem->GetUserScore() )
		{
			lScore = -pIServerUserItem->GetUserScore();
		}

		//����˰��
		LONGLONG lRevenue = 0;
		if ( lScore > 0 )
		{
			lRevenue = m_pITableFrame->CalculateRevenue(wChairID, lScore);
			lScore -= lRevenue;
		}

		//д��
		tagScoreInfo ScoreInfo;
		ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
		ScoreInfo.lScore = lScore;
		ScoreInfo.lRevenue = lRevenue;
		if ( lScore > 0 )
			ScoreInfo.cbType = SCORE_TYPE_WIN;
		else if ( lScore < 0 )
			ScoreInfo.cbType = SCORE_TYPE_LOSE;
		else
			ScoreInfo.cbType = SCORE_TYPE_DRAW;

		m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);
	}
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch( cbGameStatus )
	{
	case GAME_SCENE_FREE:
		{
			//��������
			CMD_S_StatusFree CMDStatusFree;
			ZeroMemory(&CMDStatusFree,sizeof(CMDStatusFree));

			//ʱ����Ϣ
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			CMDStatusFree.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));

			//������Ϣ
			//CMDStatusFree.lPlayScore = pIServerUserItem->GetUserScore();
			CMDStatusFree.lStorageStart = m_lStorageStart;

			CMDStatusFree.lAreaLimitScore = m_lAreaLimitScore;
			CMDStatusFree.lPlayLimitScore = m_lPlayLimitScore;
			//CMDStatusFree.szGameRoomID=m_pGameServiceOption->wServerID;

			//��Ϸ��¼
			for ( int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				CMDStatusFree.nTurnTableRecord[i] = INT_MAX;
				if( i < m_ArrayTurnTableRecord.GetCount() )
					CMDStatusFree.nTurnTableRecord[i] = m_ArrayTurnTableRecord[i];
			}

			CMDStatusFree.lStartStorage = m_lStorageStart;
			CMDStatusFree.lCellScore = 1;
			CopyMemory(CMDStatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(CMDStatusFree.szGameRoomName));
			CMDStatusFree.bGenreEducate = false;
			CopyMemory(CMDStatusFree.dChipArray, g_lChipArray, sizeof(g_lChipArray));

			//�����л�����
			SendSwitchNum(pIServerUserItem);
			//���ͳ���
			bool bReturn = m_pITableFrame->SendGameScene(pIServerUserItem,&CMDStatusFree,sizeof(CMDStatusFree));
			return bReturn;
		}
		break;
	case GAME_SCENE_BET:
		{
			//��������
			CMD_S_StatusPlay CMDStatusPlay;
			ZeroMemory(&CMDStatusPlay,sizeof(CMDStatusPlay));

			//ʱ����Ϣ
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			CMDStatusPlay.cbTimeLeave = (BYTE)(m_cbBetTime - __min(dwPassTime, m_cbBetTime));

			//������Ϣ
			CMDStatusPlay.lStorageStart = m_lStorageStart;

			CMDStatusPlay.lAreaLimitScore = m_lAreaLimitScore;
			CMDStatusPlay.lPlayLimitScore = m_lPlayLimitScore;

			CopyMemory(CMDStatusPlay.nAnimalMultiple, m_nAnimalMultiple, sizeof(CMDStatusPlay.nAnimalMultiple));
			CopyMemory(CMDStatusPlay.lAllBet, m_lAllBet, sizeof(CMDStatusPlay.lAllBet));
			CopyMemory(CMDStatusPlay.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDStatusPlay.lPlayBet));

			//��Ϸ��¼
			for (int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				CMDStatusPlay.nTurnTableRecord[i] = INT_MAX;
				if(i < m_ArrayTurnTableRecord.GetCount() )
					CMDStatusPlay.nTurnTableRecord[i] = m_ArrayTurnTableRecord[i];
			}

			CMDStatusPlay.lStartStorage = m_lStorageStart;
			CMDStatusPlay.lCellScore = 1;
			CopyMemory(CMDStatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(CMDStatusPlay.szGameRoomName));
			CMDStatusPlay.bGenreEducate = false;
			CopyMemory(CMDStatusPlay.dChipArray, g_lChipArray, sizeof(g_lChipArray));

			//�����л�����
			SendSwitchNum(pIServerUserItem);
			//���ͳ���
			bool bReturn = m_pITableFrame->SendGameScene(pIServerUserItem,&CMDStatusPlay,sizeof(CMDStatusPlay));
			return bReturn;
		}
		break;
	case GAME_SCENE_END:
		{
			//��������
			CMD_S_StatusEnd CMDStatusEnd;
			ZeroMemory(&CMDStatusEnd,sizeof(CMDStatusEnd));

			//ʱ����Ϣ
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			CMDStatusEnd.cbTimeLeave = (BYTE)(m_cbEndTime - __min(dwPassTime, m_cbEndTime));

			//������Ϣ
			CMDStatusEnd.lStorageStart = m_lStorageStart;

			CMDStatusEnd.lAreaLimitScore = m_lAreaLimitScore;
			CMDStatusEnd.lPlayLimitScore = m_lPlayLimitScore;

			CopyMemory( CMDStatusEnd.nAnimalMultiple, m_nAnimalMultiple, sizeof(CMDStatusEnd.nAnimalMultiple));
			CopyMemory( CMDStatusEnd.lAllBet, m_lAllBet, sizeof(CMDStatusEnd.lAllBet));
			CopyMemory( CMDStatusEnd.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDStatusEnd.lPlayBet));

			//��Ϸ��¼
			for ( int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				CMDStatusEnd.nTurnTableRecord[i] = INT_MAX;
				if( i < m_ArrayTurnTableRecord.GetCount() )
					CMDStatusEnd.nTurnTableRecord[i] = m_ArrayTurnTableRecord[i];
			}

			CMDStatusEnd.lStartStorage = m_lStorageStart;
			CMDStatusEnd.lCellScore = 1;
			CopyMemory(CMDStatusEnd.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(CMDStatusEnd.szGameRoomName));
			CMDStatusEnd.bGenreEducate = false;
			CopyMemory(CMDStatusEnd.dChipArray, g_lChipArray, sizeof(g_lChipArray));

			//�����л�����
			SendSwitchNum(pIServerUserItem);
			//���ͳ���
			bool bReturn = m_pITableFrame->SendGameScene(pIServerUserItem,&CMDStatusEnd,sizeof(CMDStatusEnd));
			return bReturn;
		}
		break;
	}
	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE://��ʼ��Ϸ
		{
			m_pITableFrame->StartGame();
			//-------------------------------------------------------
			//����ʱ��
			m_dwBetTime = (DWORD)time(NULL);

			//��ʾ���
			//static int	nCurrentHour = 0;							//��ʾ���
			//CTime time = CTime::GetCurrentTime();					//��ȡ��ǰʱ��. 
			//if ( time.GetHour() != nCurrentHour )
			//{
			//	TCHAR szServiceInfo[128] = {0};
			//	_sntprintf(szServiceInfo,sizeof(szServiceInfo), TEXT("��ǰ�������%I64d"), m_lStorageStart);
			//	CTraceService::TraceString(szServiceInfo,TraceLevel_Info);
			//	nCurrentHour = time.GetHour();
			//}

			//��Ϣ����
			CMD_S_GameStart CMDGameStart;
			ZeroMemory(&CMDGameStart,sizeof(CMDGameStart));

			//���ñ���
			CMDGameStart.cbTimeLeave = m_cbBetTime;
			CMDGameStart.lStorageStart = m_lStorageStart;

			//��������
			int nAnimalMultiple[2][ANIMAL_MAX] = 
			{
				{ 12,  8,  8,  6, 12,  8,  8,  6, 24, 24,  2,  2},
				{ 24, 12,  8,  4, 24, 12,  8,  4, 24, 24,  2,  2},
			};

			int nLastMultipleIndex = m_nLastMultipleIndex;
			do 
			{
				m_nLastMultipleIndex = rand()%2;
			} while ( nLastMultipleIndex == m_nLastMultipleIndex );
			CopyMemory(m_nAnimalMultiple, nAnimalMultiple[m_nLastMultipleIndex], sizeof(m_nAnimalMultiple));
			CopyMemory(CMDGameStart.nAnimalMultiple, m_nAnimalMultiple, sizeof(CMDGameStart.nAnimalMultiple));
			
			
			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, &CMDGameStart, sizeof(CMDGameStart));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START, &CMDGameStart, sizeof(CMDGameStart));

			//������ʱ��
			m_pITableFrame->SetGameTimer(IDI_BET, m_cbBetTime*1000, 1, NULL);

			//����״̬
			m_pITableFrame->SetGameStatus(GAME_SCENE_BET);

			return true;
		}
	case IDI_BET:		//��עʱ��
		{
			//״̬�ж�(��ֹǿ���ظ�����)
			if ( m_pITableFrame->GetGameStatus() != GAME_SCENE_END )
			{
				//����״̬
				m_pITableFrame->SetGameStatus(GAME_SCENE_END);

				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
				//����ʱ��
				m_dwBetTime = (DWORD)time(NULL);
				if ( CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]) == ANIMAL_GOLD_SHARK )
				{
					m_pITableFrame->SetGameTimer(IDI_END, (m_cbEndTime + 30)*1000, 1, NULL);	
				}
				else if( CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]) == ANIMAL_SLIVER_SHARK )
				{
					m_pITableFrame->SetGameTimer(IDI_END, (m_cbEndTime + 17)*1000, 1, NULL);	
				}
				else
				{
					m_pITableFrame->SetGameTimer(IDI_END, m_cbEndTime*1000, 1, NULL);	
				}	
			}
			return true;
		}
	case IDI_END:				//������Ϸ
		{
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			//д�����
			/*
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) continue;

				ScoreInfo[wUserChairID].cbType=(m_lPlayWinChip[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
				ScoreInfo[wUserChairID].lRevenue=m_pITableFrame->CalculateRevenue(wUserChairID, m_lPlayWinChip[wUserChairID]);
				ScoreInfo[wUserChairID].lScore=m_lPlayWinChip[wUserChairID];

				}
			}
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));*/
			//������Ϸ
			m_dwBetTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
			m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,0L);
			GameOverWriteScore();

			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree,sizeof(GameFree));
			GameFree.cbTimeLeave = m_cbFreeTime;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));

			m_pITableFrame->ConcludeGame(GER_NORMAL);	
			m_bRefreshCfg = GetPrivateProfileInt(m_szGameRoomName, TEXT("Refresh"), 0, m_szConfigFileName);
			//��ȡ����
			if (m_bRefreshCfg)
				ReadConfigInformation();
			return true;
		}
	}

	return false;
}

//�����¼�
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return false;
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch(wSubCmdID)
	{
	case SUB_C_SWITCT_NUM:{
		SendSwitchNum(pIServerUserItem);
		return true;}
	case SUB_C_PLAY_BET:			//�û���ע
		return OnSubPlayBet( pData, wDataSize, pIServerUserItem );
	case SUB_C_BET_CLEAR:			//�����ע
		return OnSubBetClear( pData, wDataSize, pIServerUserItem );
	//case SUB_C_AMDIN_COMMAND:
	//	{
	//		
	//		ASSERT(wDataSize==sizeof(CMD_C_AdminReq));
	//		if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

	//		//Ȩ���ж�
	//		if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())==false)
	//			return false;

	//		
	//		if ( m_pServerContro == NULL)
	//			return true;

	//		const CMD_C_AdminReq* AdminReq=static_cast<const CMD_C_AdminReq*>(pData);
	//		if(AdminReq->cbReqType == RQ_SET_WIN_AREA)
	//			m_bControl = TRUE;
	//		
	//		return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame);
	//	}
	//case SUB_C_UPDATE_STORAGE:
	//	{
	//		ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
	//		if(wDataSize!=sizeof(CMD_C_UpdateStorage)) return false;

	//		//Ȩ���ж�
	//		if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
	//			return false;

	//		//��Ϣ����
	//		CMD_C_UpdateStorage * pUpdateStorage=(CMD_C_UpdateStorage *)pData;
	//		if (pUpdateStorage->cbReqType==RQ_SET_STORAGE)
	//		{
	//			OutputDebugString(TEXT("RQ_SET_STORAGE"));
	//			m_lStorageStart=pUpdateStorage->lStorage;
	//			m_lStorageDeduct=pUpdateStorage->lStorageDeduct;
	//		}

	//		CMD_S_UpdateStorage updateStorage;
	//		updateStorage.lStorage=m_lStorageStart;
	//		updateStorage.lStorageDeduct=m_lStorageDeduct;

	//		m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
	//		return true;
	//	}
	}
	return false;
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (wSubCmdID < SUB_GF_FRAME_MESSAG_GAME_MIN || wSubCmdID > SUB_GF_FRAME_MESSAG_GAME_MAX)
		return false;

	switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
	{
			case SUB_C_AMDIN_COMMAND:
		{
			
			ASSERT(wDataSize==sizeof(CMD_C_AdminReq));
			if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

			//Ȩ���ж�
			if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())==false)
				return false;

			
			if ( m_pServerContro == NULL)
				return true;

			const CMD_C_AdminReq* AdminReq=static_cast<const CMD_C_AdminReq*>(pData);
			if(AdminReq->cbReqType == RQ_SET_WIN_AREA)
				m_bControl = TRUE;
			
			return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame);
		}
	case SUB_C_UPDATE_STORAGE:
		{
			ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
			if(wDataSize!=sizeof(CMD_C_UpdateStorage)) return false;

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			//��Ϣ����
			CMD_C_UpdateStorage * pUpdateStorage=(CMD_C_UpdateStorage *)pData;
			if (pUpdateStorage->cbReqType==RQ_SET_STORAGE)
			{
				OutputDebugString(TEXT("RQ_SET_STORAGE"));
				m_lStorageStart=pUpdateStorage->lStorage;
				m_lStorageDeduct=pUpdateStorage->lStorageDeduct;
			}

			CMD_S_UpdateStorage updateStorage;
			updateStorage.lStorage=m_lStorageStart;
			updateStorage.lStorageDeduct=m_lStorageDeduct;

			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_UPDATE_STORAGE, &updateStorage, sizeof(updateStorage));
			return true;
	    }
	}
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if ( bLookonUser )
		return true;

	if(m_dwBetTime == 0)
	{
		m_dwBetTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
	}

	//���ñ���
	m_dwDrawStartTime[wChairID] = (DWORD)time(NULL);
	m_lPlayOriginalScore[wChairID] = pIServerUserItem->GetUserScore();
	m_lPlayScore[wChairID] = pIServerUserItem->GetUserScore();
	m_lPlayWinChip[wChairID] = 0;
	m_lPlayWinPrizes[wChairID] = 0;
	ZeroMemory(m_lPlayWinScore[wChairID], sizeof(LONGLONG)*2);
	ZeroMemory(m_lPlayWinAnimal[wChairID], sizeof(LONGLONG)*ANIMAL_MAX);

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if ( bLookonUser )
		return true;

	//���ñ���
	m_dwDrawStartTime[wChairID] = 0;
	m_lPlayOriginalScore[wChairID] = 0;
	m_lPlayScore[wChairID] = 0;
	m_lPlayWinChip[wChairID] = 0;
	m_lPlayWinPrizes[wChairID] = 0;
	ZeroMemory(m_lPlayWinScore[wChairID], sizeof(LONGLONG)*2);
	ZeroMemory(m_lPlayWinAnimal[wChairID], sizeof(LONGLONG)*ANIMAL_MAX);

	////����Ƿ�����
	//for ( WORD i = 0; i < GAME_PLAYER; ++i )
	//{
	//	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
	//	if (pIServerUserItem == NULL) continue;

	//	return true;
	//}

	//������Ϸ
	//m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

	return true;
}

//�û���ע��Ϣ
bool CTableFrameSink::OnSubPlayBet(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч������
	ASSERT( wDataSize == sizeof(CMD_C_PlayBet) );
	if ( wDataSize != sizeof(CMD_C_PlayBet) )
	{
		OutputDebugString(TEXT(">>>>>>>>>>>>>>>>>0"));
		return false;
	}
	//��Ϣ����
	CMD_C_PlayBet * pPlayBet = (CMD_C_PlayBet *)pData;
	WORD wChairID = pIServerUserItem->GetChairID();

	//��ע�쳣
	if ( pPlayBet->nAnimalIndex < 0 || pPlayBet->nAnimalIndex >= ANIMAL_MAX || pPlayBet->lBetChip > m_lPlayScore[wChairID] || pPlayBet->lBetChip < 0 )
	{
		OutputDebugString(TEXT(">>>>>>>>>>>>>>>>>1"));
		ASSERT(FALSE);
		return false;
	}

	//��עΪ0
	if( pPlayBet->lBetChip == 0 ) 
	{
		OutputDebugString(TEXT(">>>>>>>>>>>>>>>>>2"));

		ASSERT(FALSE);
		return true;
	}
	//�жϳ���
	if ( (m_lAreaLimitScore != 0 && m_lAllBet[pPlayBet->nAnimalIndex] + pPlayBet->lBetChip > m_lAreaLimitScore)
		|| (m_lPlayLimitScore != 0 && m_lPlayBet[wChairID][pPlayBet->nAnimalIndex] + pPlayBet->lBetChip > m_lPlayLimitScore)
		|| ( m_pITableFrame->GetGameStatus() != GAME_SCENE_BET ) )
	{
		OutputDebugString(TEXT(">>>>>>>>>>>>>>>>>3"));

		//��עʧ��
		CMD_S_PlayBetFail CMDPlayBetFail;
		CMDPlayBetFail.wChairID = wChairID;
		CMDPlayBetFail.nAnimalIndex = pPlayBet->nAnimalIndex;
		CMDPlayBetFail.lBetChip = pPlayBet->lBetChip;
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_PLAY_BET_FAIL, &CMDPlayBetFail, sizeof(CMDPlayBetFail));
		return true;
	}
	//��ע�ɹ�
	m_lPlayScore[wChairID] -= pPlayBet->lBetChip;
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
	//if (pIServerUserItem && pIServerUserItem->GetUserInfo()->cbMemberLevel>0)
		m_HPlayBet[pPlayBet->nAnimalIndex]+=pPlayBet->lBetChip;
	if (pIServerUserItem && pIServerUserItem->IsAndroidUser())
		m_APlayBet[pPlayBet->nAnimalIndex]+=pPlayBet->lBetChip;
	//if (pIServerUserItem && pIServerUserItem->GetUserInfo()->cbMemberLevel==0)
		m_FPlayBet[pPlayBet->nAnimalIndex]+=pPlayBet->lBetChip;
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
	m_lPlayBet[wChairID][pPlayBet->nAnimalIndex] += pPlayBet->lBetChip;
	m_lAllBet[pPlayBet->nAnimalIndex] += pPlayBet->lBetChip;

	//������Ϣ
	CMD_S_PlayBet CMDPlayBet;
	CMDPlayBet.wChairID = wChairID;
	CMDPlayBet.nAnimalIndex = pPlayBet->nAnimalIndex;
	CMDPlayBet.lBetChip = pPlayBet->lBetChip;
	CMDPlayBet.bAndroid = false;
	if (pIServerUserItem)
	{
		/*memcpy(CMDPlayBet.szPlayerName, pIServerUserItem->GetUserInfo()->szNickName, LEN_NICKNAME);
		if(!pIServerUserItem->IsAndroidUser())
			CMDPlayBet.mHBet=pPlayBet->lBetChip;*/

		CMDPlayBet.bAndroid = pIServerUserItem->IsAndroidUser();
	}
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLAY_BET, &CMDPlayBet, sizeof(CMDPlayBet));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLAY_BET, &CMDPlayBet, sizeof(CMDPlayBet));

	//���������ע��Ϣ
	if (!pIServerUserItem->IsAndroidUser())
	{
		//������������ע����  ֱ�ӷ��͸������û�
		BYTE cbBuffer[SOCKET_TCP_BUFFER];
		ZeroMemory(cbBuffer, sizeof(cbBuffer));
		DWORD dwPaketSize = 0;

		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pServerUserItem == NULL) continue;
			if (pServerUserItem->IsAndroidUser()) continue;

			LONGLONG lUserAllBet = 0;
			for (BYTE j = 0; j < ANIMAL_MAX; j++)
			{
				lUserAllBet += m_lPlayBet[j][i];
				TCHAR szDes[200];
				_sntprintf(szDes, 200, TEXT("��ע[%d]"), m_lPlayBet[j][i]);
				CTraceService::TraceString(szDes, TraceLevel_Debug);
			}
			if (0 == lUserAllBet) continue;

			//��������
			tagUserBet *UserBet = (tagUserBet*)(cbBuffer + dwPaketSize);
			CopyMemory(UserBet->szNickName, pServerUserItem->GetNickName(), sizeof(UserBet->szNickName));
			UserBet->dwUserGameID = pServerUserItem->GetGameID();
			UserBet->lUserStartScore = m_lPlayOriginalScore[i];
			UserBet->lUserWinLost = pServerUserItem->GetUserScore() - UserBet->lUserStartScore;
			for (BYTE k = 0; k < ANIMAL_MAX; k++)
			{
				UserBet->lUserBet[k] = m_lPlayBet[k][i];
				TCHAR szDes[200];
				_sntprintf(szDes, 200, TEXT("��ע1[%d]"), m_lPlayBet[k][i]);
				CTraceService::TraceString(szDes, TraceLevel_Debug);
			}

			dwPaketSize += sizeof(tagUserBet);
		}
		CTraceService::TraceString(TEXT("������ע"), TraceLevel_Debug);
		m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, cbBuffer, dwPaketSize);

	}

	return true;
}

//�����ע��Ϣ
bool CTableFrameSink::OnSubBetClear(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч������
	ASSERT( wDataSize == NULL );
	if ( wDataSize != NULL ) return false;

	//��Ϣ����
	WORD wChairID = pIServerUserItem->GetChairID();

	//������Ϣ
	CMD_S_BetClear CMDBetClear;
	CMDBetClear.wChairID = wChairID;
	//_tcscpy(CMDBetClear.szPlayerName, pIServerUserItem->GetUserInfo()->szNickName);
	CopyMemory(CMDBetClear.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDBetClear.lPlayBet));

	//�����ע
	BOOL bSendMessage = FALSE;
	for ( int i = 0; i < ANIMAL_MAX; ++i )
	{
		if ( m_lPlayBet[wChairID][i] > 0 )
		{
			bSendMessage = TRUE;
			m_lPlayScore[wChairID] += m_lPlayBet[wChairID][i];
			m_lAllBet[i] -= m_lPlayBet[wChairID][i];
			m_lPlayBet[wChairID][i] = 0;
		}
	}
	
	//������Ϣ
	if ( bSendMessage )
	{
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
	}
	
	return true;
}

//����ת��
bool CTableFrameSink::CreateTableTable()
 {
	//���ñ���
	m_bTurnTwoTime = FALSE;

	//������һ��ͼ
	int nWinTable = CGameLogic::GetInstance()->RandomArea(TURAN_TABLE_MAX, 
	m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3,
	m_nGenerationChance[ANIMAL_GOLD_SHARK]/4, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3,
	m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_GOLD_SHARK]/4,
	m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_RABBIT]/3,
	m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_GOLD_SHARK]/4, m_nGenerationChance[ANIMAL_SWALLOW]/3,
	m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_PIGEON]/2,
	m_nGenerationChance[ANIMAL_PIGEON]/2, m_nGenerationChance[ANIMAL_GOLD_SHARK]/4, m_nGenerationChance[ANIMAL_PEACOCK]/2, m_nGenerationChance[ANIMAL_PEACOCK]/2);

	
	////int nTemp[] = { 0, 4 };
	//static nWin = 0;
	//nWin = (nWin + 1)%CountArray(nTemp);
	//nWinTable = nTemp[nWin];
	//nWinTable = 4;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//m_nTurnTableTarget[0]//���Ƶ�һ���ֵö���
	m_nTurnTableTarget[0] = nWinTable;
	BYTE ProbilityRand=GetProbility();
	bool bControl = false;
	if( m_pServerContro != NULL && m_pServerContro->NeedControl() )//�ڴ˴��������
	{
		if(m_pServerContro->ControlResult(&m_nTurnTableTarget[0]))
		{
			bControl=true;
			m_pServerContro->CompleteControl();
			nWinTable=m_nTurnTableTarget[0];
			if ( CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_GOLD_SHARK || CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_SLIVER_SHARK ) 
			m_bTurnTwoTime = TRUE;
		}			
	}
	else if(rand()%100<ProbilityRand)
	{
		int minArea1=0;
		int minArea2=0;
		int minArea3=0;
		int minArea4=0;
		LONGLONG AreaBet[8];
		AreaBet[0]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_LION]*m_nAnimalMultiple[ANIMAL_LION];
		AreaBet[1]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_PANDA]*m_nAnimalMultiple[ANIMAL_PANDA];
		AreaBet[2]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_MONKEY]*m_nAnimalMultiple[ANIMAL_MONKEY];
		AreaBet[3]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_RABBIT]*m_nAnimalMultiple[ANIMAL_RABBIT];
		AreaBet[4]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_EAGLE]*m_nAnimalMultiple[ANIMAL_EAGLE];
		AreaBet[5]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PEACOCK]*m_nAnimalMultiple[ANIMAL_PEACOCK];
		AreaBet[6]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PIGEON]*m_nAnimalMultiple[ANIMAL_PIGEON];
		AreaBet[7]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_SWALLOW]*m_nAnimalMultiple[ANIMAL_SWALLOW];
		LONGLONG maxAreaBet=0;
		int maxArea=0;
		for	 (int i=0;i<8;i++)
		{
			if (maxAreaBet<=AreaBet[i])
			{
				maxAreaBet=AreaBet[i];
				maxArea=i;
			}
		}
		if (maxAreaBet!=0)
		{
			LONGLONG AreaBet1=AreaBet[maxArea];
			LONGLONG AreaBet2=AreaBet[maxArea];
			LONGLONG AreaBet3=AreaBet[maxArea];
			LONGLONG AreaBet4=AreaBet[maxArea];
			for	 (int i=0;i<8;i++)
			{
				if (AreaBet1>=AreaBet[i])
				{
					AreaBet1=AreaBet[i];
					minArea1=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet2>=AreaBet[i]&&i!=minArea1)
				{
					AreaBet2=AreaBet[i];
					minArea2=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet3>=AreaBet[i]&&i!=minArea1&&i!=minArea2)
				{
					AreaBet3=AreaBet[i];
					minArea3=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet4>=AreaBet[i]&&i!=minArea1&&i!=minArea2&&i!=minArea3)
				{
					AreaBet4=AreaBet[i];
					minArea4=i;
				}
			}
		/*	switch(rand()%4)
			{
			case 0:
				nWinTable=minArea1;
				break;
			case 1:
				nWinTable=minArea2;
				break;
			case 2:
				nWinTable=minArea3;
				break;
			case 3:
				nWinTable=minArea4;
				break;
			}*/
			nWinTable=minArea1;
			switch(nWinTable)
			{
			case 0:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=5;
						break;
					case 1:
						nWinTable=6;
						break;
					case 2:
						nWinTable=7;
						break;
					}
				}
				break;
			case 1:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=9;
						break;
					case 1:
						nWinTable=10;
						break;
					}
				}
				break;
			case 2:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=12;
						break;
					case 1:
						nWinTable=13;
						break;
					}
				}
				break;
			case 3:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=15;
						break;
					case 1:
						nWinTable=16;
						break;
					case 2:
						nWinTable=17;
						break;
					}
				}
				break;
			case 4:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=1;
						break;
					case 1:
						nWinTable=2;
						break;
					case 2:
						nWinTable=3;
						break;
					}
				}
				break;
			case 5:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=26;
						break;
					case 1:
						nWinTable=27;
						break;
					}
				}
				break;
			case 6:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=23;
						break;
					case 1:
						nWinTable=24;
						break;
					}
				}
				break;
			case 7:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=19;
						break;
					case 1:
						nWinTable=20;
						break;
					case 2:
						nWinTable=21;
						break;
					}
				}
				break;
			}
		}
		m_nTurnTableTarget[0] = nWinTable;
		//��������㣬������һ��ͼ
		if ( CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_GOLD_SHARK || CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_SLIVER_SHARK )
		{
			m_bTurnTwoTime = TRUE;


			//�����ڶ���ͼ
			nWinTable = CGameLogic::GetInstance()->RandomArea(TURAN_TABLE_MAX, 
				0, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3,
				0, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3,
				0, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_PANDA]/2, 0,
				m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_MONKEY]/2, 0, m_nGenerationChance[ANIMAL_RABBIT]/3,
				m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_RABBIT]/3, 0, m_nGenerationChance[ANIMAL_SWALLOW]/3,
				m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SWALLOW]/3, 0, m_nGenerationChance[ANIMAL_PIGEON]/2,
				m_nGenerationChance[ANIMAL_PIGEON]/2, 0, m_nGenerationChance[ANIMAL_PEACOCK]/2, m_nGenerationChance[ANIMAL_PEACOCK]/2);

			////nWinTable=3;
			//m_nTurnTableTarget[1] //���Ƶ����ֽ�������֮�󣬵ڶ��γ��ֵĶ���
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			m_nTurnTableTarget[1] = nWinTable;
			int minArea1=0;
			int minArea2=0;
			int minArea3=0;
			int minArea4=0;
			LONGLONG AreaBet[8];
			AreaBet[0]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_LION]*m_nAnimalMultiple[ANIMAL_LION];
			AreaBet[1]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_PANDA]*m_nAnimalMultiple[ANIMAL_PANDA];
			AreaBet[2]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_MONKEY]*m_nAnimalMultiple[ANIMAL_MONKEY];
			AreaBet[3]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_RABBIT]*m_nAnimalMultiple[ANIMAL_RABBIT];
			AreaBet[4]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_EAGLE]*m_nAnimalMultiple[ANIMAL_EAGLE];
			AreaBet[5]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PEACOCK]*m_nAnimalMultiple[ANIMAL_PEACOCK];
			AreaBet[6]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PIGEON]*m_nAnimalMultiple[ANIMAL_PIGEON];
			AreaBet[7]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_SWALLOW]*m_nAnimalMultiple[ANIMAL_SWALLOW];
			LONGLONG maxAreaBet=0;
			int maxArea=0;
			for	 (int i=0;i<8;i++)
			{
				if (maxAreaBet<=AreaBet[i])
				{
					maxAreaBet=AreaBet[i];
					maxArea=i;
				}
			}
			if (maxAreaBet!=0)
			{
				LONGLONG AreaBet1=AreaBet[maxArea];
				LONGLONG AreaBet2=AreaBet[maxArea];
				LONGLONG AreaBet3=AreaBet[maxArea];
				LONGLONG AreaBet4=AreaBet[maxArea];
			for	 (int i=0;i<8;i++)
			{
				if (AreaBet1>=AreaBet[i])
				{
					AreaBet1=AreaBet[i];
					minArea1=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet2>=AreaBet[i]&&i!=minArea1)
				{
					AreaBet2=AreaBet[i];
					minArea2=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet3>=AreaBet[i]&&i!=minArea1&&i!=minArea2)
				{
					AreaBet3=AreaBet[i];
					minArea3=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet4>=AreaBet[i]&&i!=minArea1&&i!=minArea2&&i!=minArea3)
				{
					AreaBet4=AreaBet[i];
					minArea4=i;
				}
			}
			/*switch(rand()%4)
			{
			case 0:
				nWinTable=minArea1;
				break;
			case 1:
				nWinTable=minArea2;
				break;
			case 2:
				nWinTable=minArea3;
				break;
			case 3:
				nWinTable=minArea4;
				break;
			}*/
			nWinTable=minArea1;
			switch(nWinTable)
			{
			case 0:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=5;
						break;
					case 1:
						nWinTable=6;
						break;
					case 2:
						nWinTable=7;
						break;
					}
				}
				break;
			case 1:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=9;
						break;
					case 1:
						nWinTable=10;
						break;
					}
				}
				break;
			case 2:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=12;
						break;
					case 1:
						nWinTable=13;
						break;
					}
				}
				break;
			case 3:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=15;
						break;
					case 1:
						nWinTable=16;
						break;
					case 2:
						nWinTable=17;
						break;
					}
				}
				break;
			case 4:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=1;
						break;
					case 1:
						nWinTable=2;
						break;
					case 2:
						nWinTable=3;
						break;
					}
				}
				break;
			case 5:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=26;
						break;
					case 1:
						nWinTable=27;
						break;
					}
				}
				break;
			case 6:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=23;
						break;
					case 1:
						nWinTable=24;
						break;
					}
				}
				break;
			case 7:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=19;
						break;
					case 1:
						nWinTable=20;
						break;
					case 2:
						nWinTable=21;
						break;
					}
				}
				break;
			}
			m_nTurnTableTarget[1] = nWinTable;
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
	else
	{
		//�����ڶ���ͼ
		nWinTable = CGameLogic::GetInstance()->RandomArea(TURAN_TABLE_MAX, 
			0, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3,
			0, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3,
			0, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_PANDA]/2, 0,
			m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_MONKEY]/2, 0, m_nGenerationChance[ANIMAL_RABBIT]/3,
			m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_RABBIT]/3, 0, m_nGenerationChance[ANIMAL_SWALLOW]/3,
			m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SWALLOW]/3, 0, m_nGenerationChance[ANIMAL_PIGEON]/2,
			m_nGenerationChance[ANIMAL_PIGEON]/2, 0, m_nGenerationChance[ANIMAL_PEACOCK]/2, m_nGenerationChance[ANIMAL_PEACOCK]/2);

		////nWinTable=3;
		//m_nTurnTableTarget[1] //���Ƶ����ֽ�������֮�󣬵ڶ��γ��ֵĶ���
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		m_nTurnTableTarget[1] = nWinTable;
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

//��Ϸ��������
int CTableFrameSink::GameOver()
{
	//�������
	int nPrizesMultiple = 0;
	bool bSuccess = false;
	//ϵͳ��Ӯ
	LONGLONG lSystemScore = 0l;
	//�þֿ��Ƶ������Ӯ
	LONGLONG lPlayerScore = 0l;

	//LONGLONG lTempCalculateScore = 0l;
	//LONGLONG lMaxSystemScore = LLONG_MIN;
	//int nMaxTurnTableTarget[2] = {0};
	//int nWhileCount = 0;

	//�������
	//do 
	//{
		//��Ӯ����
		ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
		ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
		ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
		ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

		//��ȡ����
		LoadGameINI();

		if( m_pServerContro != NULL && m_pServerContro->NeedControl() )//�ڴ˴��������
		{
			if(m_pServerContro->ControlResult(&m_nTurnTableTarget[0]))
			{
				m_pServerContro->CompleteControl();
				int nWinTable=m_nTurnTableTarget[0];
				if ( CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_GOLD_SHARK || CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_SLIVER_SHARK )
					m_bTurnTwoTime = TRUE;
			}	//��̽���ж�
			CalculateScore( true, CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]), nPrizesMultiple );
			if ( m_bTurnTwoTime)
				CalculateScore( false, CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[1]), nPrizesMultiple );		
		}
		else
		{
			while (true)
			{
				//��Ӯ����
				ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
				ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
				ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
				ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));
				lSystemScore = 0l;
				lPlayerScore = 0l;

				//�ɷ��˿�
				CreateTableTable();

				int nWinTable=m_nTurnTableTarget[0];
				if ( CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_GOLD_SHARK || CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_SLIVER_SHARK )
					m_bTurnTwoTime = TRUE;
				lSystemScore += CalculateScore( true, CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]), nPrizesMultiple );
				if ( m_bTurnTwoTime)
					lSystemScore += CalculateScore( false, CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[1]), nPrizesMultiple );

				for (WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
				{
					//��ȡ�û�
					IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem==NULL) 
						continue;
					if(pIServerUserItem->GetGameID() == m_dwUserID)
					{
						LONGLONG lWinScore = 0;
						int nWinAnimal = CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]);
						if(nWinAnimal == ANIMAL_GOLD_SHARK)
							lWinScore = (m_lPlayBet[wChairID][nWinAnimal] * nPrizesMultiple);
						lPlayerScore += (m_lPlayWinScore[wChairID][0]/*+lWinScore*/);
						if ( m_bTurnTwoTime)
							lPlayerScore += m_lPlayWinScore[wChairID][1];
						break;
					}
				}

				//��̽���ж�
				if (WhetherControl(lSystemScore, lPlayerScore))
				{
					break;
				}
			}
		}

	//	//ϵͳ��ֵ����
	//	if(m_bControl==TRUE)
	//	{
	//		break;
	//	}
	//	else if ((lSystemScore + m_lStorageStart) < 0l)
	//	{
	//		bSuccess =  false;

	//		nWhileCount++;
	//		if( lSystemScore > lMaxSystemScore )
	//		{
	//			lMaxSystemScore = lSystemScore;
	//			CopyMemory(nMaxTurnTableTarget, m_nTurnTableTarget, sizeof(nMaxTurnTableTarget));
	//		}

	//		if( nWhileCount >= 100000 && lMaxSystemScore != LLONG_MIN )
	//		{
	//			//ǿ�ƽ���
	//			m_lStorageStart += lMaxSystemScore;
	//			CopyMemory(m_nTurnTableTarget, nMaxTurnTableTarget, sizeof(m_nTurnTableTarget));
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		m_lStorageStart += lSystemScore;
	//		
	//		//���˲���������ע��˥�����
	//		for( int i = 0; i < GAME_PLAYER; ++i )
	//		{
	//			if( m_pITableFrame->GetTableUserItem(i) != NULL && lSystemScore != 0 )
	//			{
	//				m_lStorageStart = m_lStorageStart - (m_lStorageStart * m_lStorageDeduct / 1000);
	//				break;
	//			}
	//		}

	//		bSuccess = true;
	//	}

	//} while (!bSuccess);

	//��Ӽ�¼
	m_ArrayTurnTableRecord.Add( m_nTurnTableTarget[0] );
	if( m_ArrayTurnTableRecord.GetCount() > RECORD_COUNT_MAX )
	{
		m_ArrayTurnTableRecord.RemoveAt(0);
	}

	return nPrizesMultiple;
}

//����÷�
LONGLONG CTableFrameSink::CalculateScore( bool bFirst, int nWinAnimal, int& nPrizesMultiple )
{
	//ϵͳ��Ӯ
	LONGLONG lSystemScore = 0l;
	
	//�ʽ�
	if ( bFirst && nWinAnimal == ANIMAL_GOLD_SHARK )
	{
		nPrizesMultiple = (rand()%6) + 6;
	}

	//�������
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		//�Ƿ������
		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		for ( int nAnimalIndex = 0; nAnimalIndex < ANIMAL_MAX; ++nAnimalIndex )
		{
			if ( nWinAnimal == nAnimalIndex || 
				(nAnimalIndex == ANIMAL_BIRD && CGameLogic::GetInstance()->AnimalType(nWinAnimal) == ANIMAL_TYPE_BIRD ) ||
				(nAnimalIndex == ANIMAL_BEAST && CGameLogic::GetInstance()->AnimalType(nWinAnimal) == ANIMAL_TYPE_BEAST ) )
			{
				//���Ӯ��
				LONGLONG lWinScore = m_lPlayBet[wChairID][nAnimalIndex] * m_nAnimalMultiple[nAnimalIndex];
				m_lPlayWinChip[wChairID] += lWinScore;
				m_lPlayWinAnimal[wChairID][nAnimalIndex] += lWinScore;

				if ( bFirst )
					m_lPlayWinScore[wChairID][0] += lWinScore;
				else
					m_lPlayWinScore[wChairID][1] += lWinScore;

				//ϵͳ���
				if ( !bIsAndroidUser )
					lSystemScore -= (lWinScore - m_lPlayBet[wChairID][nAnimalIndex]);
			}
			else if( bFirst )
			{
				//������
				LONGLONG lLoseScore = m_lPlayBet[wChairID][nAnimalIndex];

				//ϵͳӮ��
				if ( !bIsAndroidUser )
					lSystemScore += lLoseScore;
			}
		}

		//����ʽ�
		if ( nPrizesMultiple != 0 && bFirst )
		{
			//���Ӯ��
			LONGLONG lWinScore = (m_lPlayBet[wChairID][nWinAnimal] * nPrizesMultiple);
			m_lPlayWinChip[wChairID] += lWinScore;
			m_lPlayWinAnimal[wChairID][nWinAnimal] += lWinScore;
			m_lPlayWinPrizes[wChairID] += lWinScore;
			m_lPlayWinScore[wChairID][0] += lWinScore;

			//ϵͳ���
			if ( !bIsAndroidUser )
				lSystemScore -= lWinScore;
		}

	}

	return lSystemScore;
}

//����д��
void CTableFrameSink::GameOverWriteScore()
{
	//д�����
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if(pIServerUserItem == NULL) continue;

		//�۳�����
		m_lPlayScore[wChairID] += m_lPlayWinScore[wChairID][0]+m_lPlayWinScore[wChairID][1];

		if ( m_lPlayScore[wChairID] < 0 )
		{
			ASSERT(FALSE);
			m_lPlayScore[wChairID] = 0;
		}

		UpdataScore(wChairID, pIServerUserItem);
	}

	//�����ע
	ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
	ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
	ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
	ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

	//��ע��
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory(m_HPlayBet,sizeof(m_HPlayBet));
	ZeroMemory(m_FPlayBet,sizeof(m_FPlayBet));
	ZeroMemory(m_APlayBet,sizeof(m_APlayBet));
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
}

//��ȡ�����ļ�
BYTE CTableFrameSink::GetProbility()
{
	return ::GetPrivateProfileInt(m_pGameServiceOption->szServerName, _T("Problility"), 30, m_szConfigFileName);
}

void CTableFrameSink::LoadGameINI()
{
	//�Ƿ����׻���
	m_bIsCheat = GetPrivateProfileInt(m_szGameRoomName,TEXT("Ischeat"),0,m_szConfigFileName);
	//��������ׯ������������������и��ʻ���
	m_lRobotloseMax = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotLoseMax"),-2000000,m_szConfigFileName);
	//��������ׯ������������������ٷְٻ���
	m_lRobotLoseMaxChange = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotLoseMaxChange"),-5000000,m_szConfigFileName);
	//��������ׯ�������˻��Ƶİٷֱ�
	m_btRobotLoseChangePercent = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotLoseChangePercent"),50,m_szConfigFileName);

	//�и��ʻ��Ƶ���ʼ��ַ���
	m_lRealBankerRobotLoseMax = GetPrivateProfileInt(m_szGameRoomName,TEXT("RealBankerRobotLoseMax"),10000000,m_szConfigFileName);
	//�ٷְٻ��ƣ������������ʼ����
	m_lRealBankerRobotLoseMaxChange = GetPrivateProfileInt(m_szGameRoomName,TEXT("RealBankerRobotLoseMaxChange"),30000000,m_szConfigFileName);
	//���ưٷֱ�
	m_btRealBankerRobotLoseChangePercent = GetPrivateProfileInt(m_szGameRoomName,TEXT("RealBankerRobotLoseChangePercent"),30,m_szConfigFileName);

	//Ӯ�Ľ�������������и��ʷ�ˮ
	m_lRobotProbabilityTanking = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotProbabilityTanking"),80000000,m_szConfigFileName);
	//Ӯ�Ľ�����������˰ٷְٷ�ˮ
	m_lRobotDefineTanking = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotDefineTanking"),100000000,m_szConfigFileName);
	//�����˷�ˮ�İٷֱ�
	m_btRobotTankingPercent = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotTankingPercent"),20,m_szConfigFileName);

	//ͨ������£����ưٷֱ�
	m_btUsuallyChangePercent = GetPrivateProfileInt(m_szGameRoomName,TEXT("UsuallyChangePercent"),5,m_szConfigFileName);
	//�۳�������Ӯ�ķ����İٷֱ�
	m_btRobotWinScorePercent = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotWinScorePercent"),5,m_szConfigFileName);
	//�������ۼƵ���Ӯ����
	m_lRobotResultScore = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotResultScore"),100,m_szConfigFileName);
	//�۳��Ļ�����Ӯ�ķ����ܺ�
	if(m_lRobotWinChiefScore == 0)
		m_lRobotWinChiefScore = GetPrivateProfileInt(m_szGameRoomName,TEXT("RobotWinChiefScore"),0,m_szConfigFileName);

	//�Ƿ�������
	m_IsCheatUser = GetPrivateProfileInt(TEXT("1"),TEXT("IsCheatUser"),0,m_szConfigFilePath);
	//������Ӯ�����ID (0��ʾ������)
	m_dwUserID = GetPrivateProfileInt(TEXT("1"),TEXT("UserID"),0,m_szConfigFilePath);
	//���������Ӯ���ܾ�����ÿ����һ�ּ�һ��
	m_dwUserLoseWinPercent = GetPrivateProfileInt(TEXT("1"),TEXT("UserLoseWinPercent"),0,m_szConfigFilePath);
	//���������Ӯ�����䣨1��ʾӮ 0��ʾ�䣩
	m_bLoseWin = GetPrivateProfileInt(TEXT("1"),TEXT("LoseWin"),0,m_szConfigFilePath);
	//�������Ӯ�Ľ���������
	m_lWinMaxGold = GetPrivateProfileInt(TEXT("1"),TEXT("WinMaxGold"),0,m_szConfigFilePath);
	//���������Ľ����������
	m_lLoseMaxGold = GetPrivateProfileInt(TEXT("1"),TEXT("LoseMaxGold"),0,m_szConfigFilePath);
	//������ҵ���Ӯ���
	m_lAllUserGold = GetPrivateProfileInt(TEXT("1"),TEXT("UserGold"),0,m_szConfigFilePath);
}

void CTableFrameSink::WriteGameINI()
{
	//д������
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szFileName[MAX_PATH];

	if (GetCurrentDirectory(sizeof(szPath), szPath))
	{
		TCHAR szRobotResultScore[MAX_PATH]=TEXT("");
		//��ҿ���
		if(m_IsCheatUser && m_lUserScore!=0)
		{
			_sntprintf(szRobotResultScore,sizeof(szRobotResultScore),TEXT("%d"),m_lAllUserGold);
			WritePrivateProfileString(TEXT("1"),TEXT("UserGold"),szRobotResultScore,m_szConfigFilePath);
		}
		if(((m_bLoseWin && m_lAllUserGold>m_lWinMaxGold) || (!m_bLoseWin && m_lAllUserGold<m_lLoseMaxGold)) && m_IsCheatUser)
			WritePrivateProfileString(TEXT("1"),TEXT("IsCheatUser"),TEXT("0"),m_szConfigFilePath);

		//�����˿���
		if(m_lRobotAchievement != 0)
		{
			m_lRobotResultScore+=m_lRobotAchievement;
			if(m_lRobotResultScore>=m_lRobotDefineTanking)
			{
				long lMoney = m_lRobotResultScore*( m_btRobotWinScorePercent*0.01);
				m_lRobotWinChiefScore += lMoney;
				m_lRobotResultScore -= lMoney;
				//�۳��Ļ�����Ӯ�ķ����ܺ�
				_sntprintf(szRobotResultScore,sizeof(szRobotResultScore),TEXT("%d"),m_lRobotWinChiefScore);
				WritePrivateProfileString(m_szGameRoomName,TEXT("RobotWinChiefScore"),szRobotResultScore,m_szConfigFileName);
			}
			//�������ۼƵ���Ӯ����
			_sntprintf(szRobotResultScore,sizeof(szRobotResultScore),TEXT("%d"),m_lRobotResultScore);
			WritePrivateProfileString(m_szGameRoomName,TEXT("RobotResultScore"),szRobotResultScore,m_szConfigFileName);
		}

		static bool bControl = true;
		TCHAR szBunko[MAX_PATH]=TEXT("");
		CString str;
		CTime tm;
		tm = CTime::GetCurrentTime();
		if(tm.GetHour() == 11)
		{
			bControl = true;
		}
		if(tm.GetHour() == 14)
		{
			if(bControl)
			{
				bControl = false;
				_sntprintf(szFileName,sizeof(szFileName),TEXT("%s\\GameLog\\AnimalBattle.log"),szPath);
				str = tm.Format("%Y-%m-%d %X");
				str+= "\tʤ��";
				_sntprintf(szBunko,sizeof(szBunko),TEXT("%d"),m_lRobotResultScore+m_lRobotWinChiefScore);
				WritePrivateProfileString(m_szGameRoomName,str,szBunko,szFileName);
				//bControl = false;
				m_lRobotWinChiefScore = 0;
				m_lRobotResultScore = 0;
				//�۳��Ļ�����Ӯ�ķ����ܺ�
				_sntprintf(szRobotResultScore,sizeof(szRobotResultScore),TEXT("%d"),m_lRobotWinChiefScore);
				WritePrivateProfileString(m_szGameRoomName,TEXT("RobotWinChiefScore"),szRobotResultScore,m_szConfigFileName);
				//�������ۼƵ���Ӯ����
				_sntprintf(szRobotResultScore,sizeof(szRobotResultScore),TEXT("%d"),m_lRobotResultScore);
				WritePrivateProfileString(m_szGameRoomName,TEXT("RobotResultScore"),szRobotResultScore,m_szConfigFileName);
			}
		}
	}
}

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{
	//��������
	TCHAR OutBuf[255] = {};

	//��λ����
	/*m_lCellScore = GetPrivateProfileInt(m_szGameRoomName, TEXT("CellScore"), 1, m_szConfigFileName);
	if ( m_lCellScore < 1 )
		m_lCellScore = 1;*/

	//��������
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szGameRoomName, TEXT("AreaLimitScore"), _T("0"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lAreaLimitScore);

	//�����������
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szGameRoomName, TEXT("PlayLimitScore"), _T("0"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lPlayLimitScore);

	//���
	//ZeroMemory(OutBuf, sizeof(OutBuf));
	//GetPrivateProfileString(m_szGameRoomName, TEXT("StorageStart"), _T("0"), OutBuf, 255, m_szConfigFileName);
	//_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lStorageStart);

	//˥��ֵ
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szGameRoomName, TEXT("StorageDeduct"), _T("1"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lStorageDeduct);

	//��ע��������Ŀ
	m_nMaxChipRobot = GetPrivateProfileInt(m_szGameRoomName, TEXT("RobotBetCount"), 10, m_szConfigFileName);
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 10;

	//������
	m_nRobotListMaxCount = GetPrivateProfileInt(m_szGameRoomName, _T("RobotListMaxCount"), 5, m_szConfigFileName);	

	//��������ע����
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szGameRoomName, TEXT("RobotAreaLimit"), TEXT("50000000"), OutBuf, 255*sizeof(TCHAR), m_szConfigFileName);
	_snwscanf(OutBuf, wcslen(OutBuf), TEXT("%I64d"), &m_lRobotAreaLimit);

	//�������
	m_nGenerationChance[0] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat1"), 8, m_szConfigFileName);
	m_nGenerationChance[1] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat2"), 8, m_szConfigFileName);
	m_nGenerationChance[2] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat3"), 12, m_szConfigFileName);
	m_nGenerationChance[3] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat4"), 6, m_szConfigFileName);
	m_nGenerationChance[4] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat5"), 12, m_szConfigFileName);
	m_nGenerationChance[5] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat6"), 6, m_szConfigFileName);
	m_nGenerationChance[6] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat7"), 6, m_szConfigFileName);
	m_nGenerationChance[7] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat8"), 6, m_szConfigFileName);
	m_nGenerationChance[8] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat9"), 1, m_szConfigFileName);
	m_nGenerationChance[9] = GetPrivateProfileInt(m_szGameRoomName, _T("AreaCheat10"), 1, m_szConfigFileName);

	//ʱ�����
	m_cbFreeTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("FreeTime"), TIME_FREE, m_szConfigFileName);

	m_cbBetTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("BetTime"), TIME_BET, m_szConfigFileName);

	m_cbEndTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("EndTime"), TIME_END, m_szConfigFileName);

	//�л�����
	m_lAddScore[0]=GetPrivateProfileInt(m_szGameRoomName, TEXT("AddScore1"), 1000, m_szConfigFileName);
	m_lAddScore[1]=GetPrivateProfileInt(m_szGameRoomName, TEXT("AddScore2"), 10000, m_szConfigFileName);
	m_lAddScore[2]=GetPrivateProfileInt(m_szGameRoomName, TEXT("AddScore3"), 100000, m_szConfigFileName);
	m_lAddScore[3]=GetPrivateProfileInt(m_szGameRoomName, TEXT("AddScore4"), 500000, m_szConfigFileName);

	if (m_cbFreeTime <= 0 || m_cbFreeTime > 99)			
		m_cbFreeTime = TIME_FREE;
	if (m_cbBetTime <= 0 || m_cbBetTime > 99)		
		m_cbBetTime = TIME_BET;
	if (m_cbEndTime <= 0 || m_cbEndTime > 99)			
		m_cbEndTime = TIME_END;


	if( m_lStorageDeduct < 1 )
	{
		m_lStorageDeduct = 1;
	}	
}


//��̽���ж�
bool CTableFrameSink::WhetherControl(LONGLONG lSystemScore, LONGLONG lUserScore)
{
	//����
	if(m_IsCheatUser && lUserScore!=0)
	{
		if(m_bLoseWin)
		{
			if((rand()%100<m_dwUserLoseWinPercent && lUserScore<0) /*|| ((m_lUserScore+m_lAllUserGold) > (m_lWinMaxGold+m_lWinMaxGold/10))*/)
				return false;
		}
		else
		{
			if((rand()%100<m_dwUserLoseWinPercent && lUserScore>0) /*|| ((m_lUserScore+m_lAllUserGold) < (m_lLoseMaxGold+m_lLoseMaxGold/10))*/)
				return false;
		}
	}
	else
	{
		if(m_bIsCheat)
		{	
			//�Ƿ����㻻������(��ˮ)
			if((m_lRobotResultScore>=m_lRobotDefineTanking) || (m_lRobotResultScore>=m_lRobotProbabilityTanking && rand()%100 < m_btRobotTankingPercent))
			{
				if(lSystemScore>0)
				{
					printf("\n >>>>>>>>>>>�����˽����ˮ״̬��");
					return false;
				}
			}
			//�Ƿ����㻻������
			else if((m_lRobotResultScore<=m_lRobotLoseMaxChange) || (m_lRobotResultScore<=m_lRobotloseMax && rand()%100 < m_btRobotLoseChangePercent) ||
				(rand()%100 < m_btUsuallyChangePercent) || ((lSystemScore*-1)>=m_lRealBankerRobotLoseMaxChange) || 
				((lSystemScore*-1)>=m_lRealBankerRobotLoseMax && rand()%100 < m_btRealBankerRobotLoseChangePercent))
			{
				if(lSystemScore < 0)
				{
					printf("\n >>>>>>>>>>>�����˽��뻻��״̬��");
					return false;
				}
			}
		}
	}
	m_lUserScore = lUserScore;
	m_lAllUserGold += lUserScore;
	m_lRobotAchievement = lSystemScore;

	return true;
}
void CTableFrameSink::SendSwitchNum(IServerUserItem * pIServerUserItem)
{
	//������Ϣ
	CMD_S_SwitchNum SwitchNum;
	SwitchNum.lAddScore1 = m_lAddScore[0];
	SwitchNum.lAddScore2 = m_lAddScore[1];
	SwitchNum.lAddScore3 = m_lAddScore[2];
	SwitchNum.lAddScore4 = m_lAddScore[3];
	
	m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_SWITCT_NUM, &SwitchNum, sizeof(SwitchNum));
	m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(),SUB_S_SWITCT_NUM, &SwitchNum, sizeof(SwitchNum));
	
	return;
}
//////////////////////////////////////////////////////////////////////////////////
