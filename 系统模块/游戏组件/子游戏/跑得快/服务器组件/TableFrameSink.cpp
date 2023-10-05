#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////

enum TimerDefine
{
	IDI_TIMER_NULL = 0,
	IDI_USER_TRUSTEE_MIN,							//����йܶ�ʱ��
	IDI_USER_TRUSTEE_MAX = IDI_USER_TRUSTEE_MIN + GAME_PLAYER,
	IDI_GAME_FREE,									//����
	IDI_SO_OPERATE,									//��ʱ��ʱ��
	IDI_WAITEND,									//����ȴ�
	IDI_TIMER_MAX,
};

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wBeforWiner=INVALID_CHAIR;
	ZeroMemory(m_bBombCount,sizeof(m_bBombCount));
	ZeroMemory(m_lBombScore, sizeof(m_lBombScore));
	ZeroMemory(m_bAutoPlayer,sizeof(m_bAutoPlayer));
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	m_cbTrusteeDelayTime = 5;

	m_wPlayerCount	= GAME_PLAYER;
	m_bSixTheenCard = false;
	m_bNoForceClose = true;
	m_bNoRemoveBomb = false;
	m_bSpadeBanker	= false;
	m_bFirstOutSpade= false;

	m_bForceOutCard = true;
	m_bEchelonConclude = false;
	m_cbSpadeBankerFirstOutCardData = INVALID_BYTE;
	m_cbWinBankerFirstOutCardData = INVALID_BYTE;

	//��Ϸ��Ƶ
	m_hVideoInst = NULL;
	m_pGameVideo = NULL;
	m_hVideoInst = LoadLibrary(TEXT("RunFastGameVideo.dll"));
	if (m_hVideoInst)
	{
		typedef void *(*CREATE)();
		CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
		if (GameVideo)
		{
			m_pGameVideo = static_cast<IGameVideo *>(GameVideo());
		}
	}

	ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		m_listWinScoreRecord[i].RemoveAll();
	}

	//��ʷ����
	ZeroMemory(m_lAllTurnScore,sizeof(m_lAllTurnScore));
	ZeroMemory(m_lLastTurnScore,sizeof(m_lLastTurnScore));

	//������Ϣ
	m_wBaoPeiUser = INVALID_CHAIR;
	m_bTurnCardCount=0;
	m_bTurnCardType=CT_ERROR;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));

	//�˿���Ϣ
	ZeroMemory(m_bCardCount,sizeof(m_bCardCount));
	ZeroMemory(m_bHandCardData,sizeof(m_bHandCardData));

	//�������
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pGameCustomRule=NULL;

	InitializeListRemainCard();

#ifdef  CARD_DISPATCHER_CONTROL
	m_bConfigCard = false;
	ZeroMemory(m_cbConfigItemInfo, sizeof(m_cbConfigItemInfo));
#endif

	m_cbStatusConfigTime = 0;
	m_ctStatusStartTime = CTime::GetCurrentTime();
	return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
	if (m_pGameVideo)
	{
		delete m_pGameVideo;
		m_pGameVideo = NULL;
	}

	if (m_hVideoInst)
	{
		FreeLibrary(m_hVideoInst);
		m_hVideoInst = NULL;
	}
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
// 	CStringA lyt;
// 	lyt.Format("lytlog::��������  Initialization ");
// 	OutputDebugStringA(lyt);

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

	//�Զ�����
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	m_wPlayerCount   = m_pGameCustomRule->wPlayerCount;
	m_bSixTheenCard  = m_pGameCustomRule->bSixTheenCard;
	m_bNoForceClose = true; //m_pGameCustomRule->bNoForceClose;
	m_bNoRemoveBomb = false; //m_pGameCustomRule->bNoRemoveBomb;
	m_bSpadeBanker   = m_pGameCustomRule->bSpadeBanker;
	//m_bFirstOutSpade = m_pGameCustomRule->bFirstOutSpade;

	m_bForceOutCard = m_pGameCustomRule->bForceOutCard;
	m_bEchelonConclude = m_pGameCustomRule->bEchelonConclude;
	m_cbTrusteeDelayTime = m_pGameCustomRule->cbTrusteeDelayTime;

	m_lCellScore=m_pGameServiceOption->lCellScore;
	if(m_lCellScore<=0) m_lCellScore=1;
	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//BYTE cbTempCardData[NORMAL_CARD_COUNT];
	//ZeroMemory(cbTempCardData, sizeof(cbTempCardData));

	//cbTempCardData[0] = 40;
	//cbTempCardData[1] = 56;
	//cbTempCardData[2] = 25;
	//cbTempCardData[3] = 57;

	//m_bTurnCardData[0] = 0x2A;
	//m_bTurnCardData[1] = 0x2B;
	//m_bTurnCardData[2] = 0x2C;
	//m_bTurnCardData[3] = 0x1D;
	//m_bTurnCardData[4] = 0x21;
	//m_bTurnCardData[5] = 5;
	//m_bTurnCardData[6] = 36;
	//m_bTurnCardData[7] = 20;
	//m_bTurnCardData[8] = 4;
	//m_bTurnCardData[9] = 35;

	//�����򣨵���˳�ӱ���Ӵ�С���㣩
	//

	//BYTE cbCardType = 

	BYTE cbTempCardDataB[NORMAL_CARD_COUNT];
	ZeroMemory(cbTempCardDataB, sizeof(cbTempCardDataB));

	cbTempCardDataB[0] = 0x17;
	cbTempCardDataB[1] = 0x37;
	cbTempCardDataB[2] = 0x27;
	cbTempCardDataB[3] = 0x06;
	cbTempCardDataB[4] = 0x36;
	cbTempCardDataB[5] = 0x26;
	cbTempCardDataB[6] = 0x15;
	cbTempCardDataB[7] = 0x25;
	cbTempCardDataB[8] = 0x15;
	cbTempCardDataB[9] = 0x39;
	cbTempCardDataB[10] = 0x18;

	m_GameLogic.SortCardList(cbTempCardDataB, 10);
	BYTE cbres = m_GameLogic.GetCardType(cbTempCardDataB, 10);
	return true;
	//m_bTurnCardCount = 0;
	//m_bTurnCardType = 0;

	//�˿˷���
	//tagOutCardResult OutCardResult;
	//ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//m_GameLogic.SearchOutCard(cbTempCardDataB, 11, m_bTurnCardData, 5, CT_SINGLE_LINE, false, OutCardResult);

	//BYTE cbResultCount = 0;
	//m_GameLogic.SearchOutCard(cbTempCardDataB, 9, cbResultCount);

	return true;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_bBombCount,sizeof(m_bBombCount));
	ZeroMemory(m_lBombScore, sizeof(m_lBombScore));
	ZeroMemory(m_bAutoPlayer,sizeof(m_bAutoPlayer));
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	m_cbSpadeBankerFirstOutCardData = INVALID_BYTE;
	m_cbWinBankerFirstOutCardData = INVALID_BYTE;

	//������Ϣ
	m_wBaoPeiUser = INVALID_CHAIR;
	m_bTurnCardCount=0;
	m_bTurnCardType=CT_ERROR;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));

	//�˿���Ϣ
	ZeroMemory(m_bCardCount,sizeof(m_bCardCount));
	ZeroMemory(m_bHandCardData,sizeof(m_bHandCardData));

	return;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	m_cbSpadeBankerFirstOutCardData = INVALID_BYTE;
	m_cbWinBankerFirstOutCardData = INVALID_BYTE;

	InitializeListRemainCard();

	//�û�״̬
	for (WORD i = 0; i<m_wPlayerCount; i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
		{
			m_cbPlayStatus[i] = FALSE;
		}
		else
		{
			m_cbPlayStatus[i] = TRUE;
		}
	}

	CopyMemory(m_RoomCardRecord.cbPlayStatus, m_cbPlayStatus, sizeof(m_RoomCardRecord.cbPlayStatus));

	//����״̬
	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

	//�����˿�
	BYTE bRandCard[48];
	ZeroMemory(bRandCard, sizeof(bRandCard));
	m_GameLogic.RandCardList(bRandCard,sizeof(BYTE)*(m_bSixTheenCard==true?48:45));

#ifdef  CARD_DISPATCHER_CONTROL
	if (m_bConfigCard)
	{
		CopyMemory(bRandCard, m_cbConfigItemInfo, sizeof(bRandCard));
		m_bConfigCard = false;
	}

#endif

	CStringA lyt;
	lyt.Format("lytlog::GameBegin() m_bSixTheenCard=%d,m_bNoForceClose=%d,m_bNoRemoveBomb=%d,m_bSpadeBanker=%d,m_bFirstOutSpade=%d,m_wPlayerCount=%d", m_bSixTheenCard, m_bNoForceClose, m_bNoRemoveBomb, m_bSpadeBanker, m_bFirstOutSpade, m_wPlayerCount);
	OutputDebugStringA(lyt);

	//�ַ��˿�
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_bSixTheenCard==true?m_bCardCount[i]=16:m_bCardCount[i]=15;
		CopyMemory(&m_bHandCardData[i],&bRandCard[i*m_bCardCount[i]],sizeof(BYTE)*m_bCardCount[i]);
		m_GameLogic.SortCardList(m_bHandCardData[i],m_bCardCount[i]);
	}

	//����ׯ��
	//Ӯ���׳�
	//���ֺ��һ�ѣ���Ҫ�����ѡһλ������ɳ��ƣ�����Ҫ���к���3�ȳ�
	if (m_wBeforWiner == INVALID_CHAIR && m_bSpadeBanker == false)
	{
		BYTE cbKeyCardArray[13] = { 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x31, 0x32 };
		BYTE cbKeyIndex = 0;

		//���������п��ܶ�û�к���3��
		while (true)
		{
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				bool bFlag = false;
				for (BYTE j = 0; j<m_bCardCount[i]; j++)
				{
					if (m_bHandCardData[i][j] == cbKeyCardArray[cbKeyIndex])
					{
						//�����û�
						m_wBankerUser = i;
						m_wCurrentUser = i;
						bFlag = true;

						m_cbSpadeBankerFirstOutCardData = INVALID_BYTE;
						m_cbWinBankerFirstOutCardData = cbKeyCardArray[cbKeyIndex];

						break;
					}
				}
				if (bFlag) break;
			}

			if (m_cbWinBankerFirstOutCardData != INVALID_BYTE)
			{
				break;
			}
			else
			{
				cbKeyIndex++;
			}
		}
	}
	//Ӯ���׳�
	else if(m_wBeforWiner != INVALID_CHAIR && m_bSpadeBanker==false)
	{
		//�����û�
		m_wBankerUser = m_wBeforWiner;
		m_wCurrentUser = m_wBeforWiner;

		m_cbSpadeBankerFirstOutCardData = INVALID_BYTE;
		m_cbWinBankerFirstOutCardData = INVALID_BYTE;
	}
	//����3�׳�
	else
	{
		BYTE cbKeyCardArray[13] = { 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x31, 0x32 };
		BYTE cbKeyIndex = 0;

		//���������п��ܶ�û�к���3��
		while (true)
		{
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				bool bFlag = false;
				for (BYTE j = 0; j<m_bCardCount[i]; j++)
				{
					if (m_bHandCardData[i][j] == cbKeyCardArray[cbKeyIndex])
					{
						//�����û�
						m_wBankerUser = i;
						m_wCurrentUser = i;
						bFlag = true;

						m_cbSpadeBankerFirstOutCardData = cbKeyCardArray[cbKeyIndex];
						m_cbWinBankerFirstOutCardData = INVALID_BYTE;

						break;
					}
				}
				if (bFlag) break;
			}

			if (m_cbSpadeBankerFirstOutCardData != INVALID_BYTE)
			{
				break;
			}
			else
			{
				cbKeyIndex++;
			}
		}
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	m_GameLogic.AnalysebCardData(m_bHandCardData[m_wCurrentUser],m_bCardCount[m_wCurrentUser],AnalyseResult);

	//�����˿�
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));
	GameStart.wBankerUser	= m_wBankerUser;
	GameStart.wCurrentUser	= m_wCurrentUser;
	GameStart.wBeforWiner	= m_wBeforWiner;
	GameStart.bSixTheenCard = m_bSixTheenCard;			//16����
	GameStart.bNoForceClose = m_bNoForceClose;			//����ǿ��
	GameStart.bNoRemoveBomb = m_bNoRemoveBomb;			//ը�����ɲ�
	GameStart.bSpadeBanker  = m_bSpadeBanker;			//����3ׯ
	GameStart.cbSpadeBankerFirstOutCardData = m_cbSpadeBankerFirstOutCardData;
	GameStart.cbWinBankerFirstOutCardData = m_cbWinBankerFirstOutCardData;
	GameStart.bFirstOutSpade = m_bFirstOutSpade;		//�׳��ش�����3

	GameStart.bForceOutCard = m_bForceOutCard;
	GameStart.bEchelonConclude = m_bEchelonConclude;

	REMAINCARDCOUNT remaincard;
	ZeroMemory(&remaincard, sizeof(remaincard));
	GetRemainCard(remaincard);

	CopyMemory(&(GameStart.remainCardCount), &remaincard, sizeof(remaincard));

	if (m_pGameVideo)
	{
		m_pGameVideo->StartVideo(m_pITableFrame, m_wPlayerCount);
	}

	bool bFirstRecord = true;

	WORD wRealPlayerCount = 0;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pServerUserItem)
		{
			continue;
		}

		wRealPlayerCount++;
	}

	BYTE *pGameRule = m_pITableFrame->GetGameRule();

	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if(m_pITableFrame->GetTableUserItem(i) == NULL)	continue;
		CopyMemory(GameStart.cbCardData,m_bHandCardData[i],sizeof(GameStart.cbCardData));
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	

		if (m_pGameVideo)
		{
			Video_GameStart video;
			ZeroMemory(&video, sizeof(video));
			video.lBaseScore = m_lCellScore;
			video.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			video.wPlayerCount = wRealPlayerCount;
			video.wGamePlayerCountRule = pGameRule[1];

			video.wBankerUser = m_wBankerUser;
			video.wBeforWiner = m_wBeforWiner;
			video.wCurrentUser = m_wCurrentUser;
			CopyMemory(video.cbCardData, m_bHandCardData[i], sizeof(video.cbCardData));
			video.bSixTheenCard = m_bSixTheenCard;
			video.bNoForceClose = m_bNoForceClose;
			video.bNoRemoveBomb = m_bNoRemoveBomb;
			video.bSpadeBanker = m_bSpadeBanker;
			video.cbSpadeBankerFirstOutCardData = m_cbSpadeBankerFirstOutCardData;
			video.cbWinBankerFirstOutCardData = m_cbWinBankerFirstOutCardData;

			video.bFirstOutSpade = m_bFirstOutSpade;

			video.bForceOutCard = m_bForceOutCard;
			video.bEchelonConclude = m_bEchelonConclude;

			CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
			video.wChairID = i;
			video.lScore = pServerUserItem->GetUserScore();

			m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video, bFirstRecord);

			if (bFirstRecord == true)
			{
				bFirstRecord = false;
			}
		}
		
		ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
	}

	if (m_wCurrentUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentUser) && m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
	{
		StartTimer(IDI_USER_TRUSTEE_MIN + m_wCurrentUser, m_wCurrentUser);
	}

	//ʤ������
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameConclude(m_wBankerUser,NULL,GER_NORMAL);

	//��ҳ��ͽ�ҷ��������ö�ʱ��
	if (!IsRoomCardType())
	{
		StartTimer(IDI_SO_OPERATE);
	}

	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	ZeroMemory(m_bAutoPlayer,sizeof(m_bAutoPlayer));
	//ȡ������й�
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		IServerUserItem* pUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pUserItem)
			pUserItem->SetTrusteeUser(false);
	}

	switch (cbReason)
	{
	case GER_DISMISS:		//��Ϸ��ɢ
		{
			KillAllTimer();

			//��������
			CMD_S_GameEnd& GameEnd = m_stGameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//ʣ���˿�
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(GameEnd.remainCardCount), &remaincard, sizeof(remaincard));
			CopyMemory(GameEnd.bBombCount, m_bBombCount, sizeof(m_bBombCount));
			CopyMemory(GameEnd.lBombScore, m_lBombScore, sizeof(m_lBombScore));

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			return true;
		}
	case GER_NORMAL:		//�������
		{
			KillAllTimer();

			//�������
			CMD_S_GameEnd& GameEnd = m_stGameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//ʣ���˿�
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
				//�ȳ�����ΪӮ��
				if(0 == m_bCardCount[i] && false==m_bSpadeBanker)
				{
					m_wBeforWiner = i;				
				}
			}

			//ͳ�ƻ���
			LONGLONG lCellScore = (SCORE)(m_pITableFrame->GetCellScore());

			//���������
			//�ݶȽ���
			if (m_bEchelonConclude)
			{
				//16���淨
				if (m_bSixTheenCard)
				{
					//����ͳ��
					for (WORD i = 0; i<m_wPlayerCount; i++)
					{
						if (i == wChairID)
						{
							continue;
						}

						LONGLONG lScoreTimes = 0;
						if (m_bCardCount[i] == 1)
						{
							lScoreTimes = 0;
						}
						else if (m_bCardCount[i] > 1 && m_bCardCount[i] <= 5)
						{
							lScoreTimes = 1;
						}
						else if (m_bCardCount[i] >= 6 && m_bCardCount[i] <= 10)
						{
							lScoreTimes = 2;
						}
						else if (m_bCardCount[i] >= 11 && m_bCardCount[i] <= 15)
						{
							lScoreTimes = 3;
						}
						else
						{
							//ȫ��
							lScoreTimes = 4 * 2;
						}
							
						GameEnd.lGameScore[i] -= m_bCardCount[i] * lCellScore*lScoreTimes;
						GameEnd.lGameScore[wChairID] += m_bCardCount[i] * lCellScore*lScoreTimes;
					}
				}
				else
				{
					//����ͳ��
					for (WORD i = 0; i<m_wPlayerCount; i++)
					{
						if (i == wChairID)
						{
							continue;
						}

						LONGLONG lScoreTimes = 0;
						if (m_bCardCount[i] == 1)
						{
							lScoreTimes = 0;
						}
						else if (m_bCardCount[i] > 1 && m_bCardCount[i] <= 5)
						{
							lScoreTimes = 1;
						}
						else if (m_bCardCount[i] >= 6 && m_bCardCount[i] <= 10)
						{
							lScoreTimes = 2;
						}
						else if (m_bCardCount[i] >= 11 && m_bCardCount[i] <= 14)
						{
							lScoreTimes = 3;
						}
						else
						{
							//ȫ��
							lScoreTimes = 4 * 2;
						}

						GameEnd.lGameScore[i] -= m_bCardCount[i] * lCellScore*lScoreTimes;
						GameEnd.lGameScore[wChairID] += m_bCardCount[i] * lCellScore*lScoreTimes;
					}
				}
			}
			//�������
			else
			{
				//����ͳ��
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					if (i == wChairID)
					{
						continue;
					}

					//��Ϸ����
					if (m_bCardCount[i]>1)
					{
						LONGLONG lScoreTimes = 0;
						if (m_bSixTheenCard == true)
						{
							lScoreTimes = (m_bCardCount[i] == 16) ? 2L : 1L;
						}
						else
						{
							lScoreTimes = (m_bCardCount[i] == 15) ? 2L : 1L;
						}
						GameEnd.lGameScore[i] -= m_bCardCount[i] * lCellScore*lScoreTimes;
						GameEnd.lGameScore[wChairID] += m_bCardCount[i] * lCellScore*lScoreTimes;
					}
				}
			}
				
			//������Ҵ���
			if (m_wBaoPeiUser != INVALID_CHAIR && m_wPlayerCount == 3)
			{
				ASSERT(GameEnd.lGameScore[m_wBaoPeiUser] <= 0);

				CString strTrace;
				strTrace.Format(TEXT("before lGameScore0=%I64d lGameScore1=%I64d lGameScore2=%I64d"), GameEnd.lGameScore[0], GameEnd.lGameScore[1], GameEnd.lGameScore[2]);

				//д��־
				CString strFileName;
				strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

				tagLogUserInfo LogUserInfo;
				ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
				CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
				CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
				m_pITableFrame->SendGameLogData(LogUserInfo);

				WORD wOtherChairID = (m_wBaoPeiUser + 2) % m_wPlayerCount;
				GameEnd.lGameScore[m_wBaoPeiUser] += GameEnd.lGameScore[wOtherChairID];
				GameEnd.lGameScore[wOtherChairID] = 0L;

				strTrace.Format(TEXT("m_wBaoPeiUser=%d lGameScore0=%I64d lGameScore1=%I64d lGameScore2=%I64d "), m_wBaoPeiUser, GameEnd.lGameScore[0], GameEnd.lGameScore[1], GameEnd.lGameScore[2]);
				
				//д��־
				CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
				m_pITableFrame->SendGameLogData(LogUserInfo);
			}

			//ը������
			for (WORD i = 0; i < m_wPlayerCount; i++)
			{
				GameEnd.lGameScore[i] += m_lBombScore[i];
			}
			
			//����ͳ��
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				m_lAllTurnScore[i]+=GameEnd.lGameScore[i];
				m_lLastTurnScore[i]=GameEnd.lGameScore[i];
			}

			//��������
			LONGLONG lRevenue=0L;
			LONGLONG lUserScore[GAME_PLAYER];
			ZeroMemory(lUserScore,sizeof(lUserScore));
			CopyMemory(lUserScore,GameEnd.lGameScore,sizeof(lUserScore));

			//��ҳ�����⸶����
			if (!IsRoomCardType())
			{
				//Ӯȡ����
				if (lUserScore[wChairID] > m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore())
				{
					SCORE lOffsetScore = lUserScore[wChairID] - m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore();
					for (int i = 0; i < m_wPlayerCount; ++i)
					{
						if (i == wChairID)
							continue;
						//���������ֵȱ�������
						lUserScore[i] -= ((double)lUserScore[i] / lUserScore[wChairID]) * lOffsetScore;
					}
					lUserScore[wChairID] -= lOffsetScore;
				}
				//�������
				for (int i = 0; i < m_wPlayerCount; ++i)
				{
					if (i == wChairID)
						continue;
					SCORE lOffsetScore = m_pITableFrame->GetTableUserItem(i)->GetUserScore() + lUserScore[i];
					if (lOffsetScore < 0)
					{
						lUserScore[i] -= lOffsetScore;
						//����Ӯ����ҷ���
						lUserScore[wChairID] += lOffsetScore;
					}
				}
			}


			//ͳ�ƻ���
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				lRevenue = m_pITableFrame->CalculateRevenue(i, lUserScore[i]);
				GameEnd.lGameScore[i] = lUserScore[i] - lRevenue;
				GameEnd.lGameTax += lRevenue;

				CString strTrace;
				strTrace.Format(TEXT("CHAIRID=%d, lGameScore=%I64d��CardCount=%d, lCellScore=%I64d, lRevenue=%I64d, lBombScore=%I64d"), i, GameEnd.lGameScore[i], m_bCardCount[i], lCellScore, lRevenue, m_lBombScore[i]);

				//д��־
				CString strFileName;
				strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

				tagLogUserInfo LogUserInfo;
				ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
				CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
				CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
				m_pITableFrame->SendGameLogData(LogUserInfo);

				BYTE ScoreKind=(GameEnd.lGameScore[i]>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
				ScoreInfo[i].cbType = ScoreKind;
				ScoreInfo[i].lRevenue = lRevenue;
				ScoreInfo[i].lScore = GameEnd.lGameScore[i];

				//��ʷ����
				m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);

				//����ģʽ
				//���ַ���
				if ((m_pITableFrame->GetDataBaseMode() == 0) && !m_pITableFrame->IsPersonalRoomDisumme() && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_RoomCardRecord.nCount < MAX_RECORD_COUNT))
				{
					m_RoomCardRecord.lDetailScore[i][m_RoomCardRecord.nCount] = ScoreInfo[i].lScore;
					m_RoomCardRecord.dwBombCount[i] += m_bBombCount[i];
					
				}
				//��ҷ���
				else if ((m_pITableFrame->GetDataBaseMode() == 1) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
				{
					m_listWinScoreRecord[i].AddHead(ScoreInfo[i].lScore);
				}
			}

			//����ģʽ
			if ((m_pITableFrame->GetDataBaseMode() == 0) && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && (m_RoomCardRecord.nCount < MAX_RECORD_COUNT))
			{
				m_RoomCardRecord.nCount++;
			}

			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
			}
			//д�����
			m_pITableFrame->WriteTableScore(ScoreInfo, m_pITableFrame->GetChairCount());

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(GameEnd.remainCardCount), &remaincard, sizeof(remaincard));
			CopyMemory(GameEnd.bBombCount, m_bBombCount, sizeof(m_bBombCount));
			CopyMemory(GameEnd.lBombScore, m_lBombScore, sizeof(m_lBombScore));

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//���ַ���
			if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0)
			{
				CMD_S_RoomCardRecord RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

				//���ַ���
				if ((m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0))
				{
					CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
				}
				else
				{
					for (WORD i = 0; i < m_wPlayerCount; i++)
					{
						IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (!pIServerUserItem)
						{
							continue;
						}
						
						WORD wChairID = pIServerUserItem->GetChairID();
						POSITION pos = m_listWinScoreRecord[wChairID].GetHeadPosition();

						WORD wIndex = 0;
						while (pos)
						{
							RoomCardRecord.lDetailScore[wChairID][wIndex++] = m_listWinScoreRecord[wChairID].GetNext(pos);

							if (wIndex >= MAX_RECORD_COUNT)
							{
								break;
							}
						}

						RoomCardRecord.nCount = wIndex;
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
					}
				}			
			}
			
			//����ģʽ
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_pITableFrame->IsPersonalRoomDisumme())
			{
				//Լս��ɢ
				ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
			}

			m_pITableFrame->SetGameStatus(GAME_SCENE_WAITEND);
			StartTimer(IDI_WAITEND);

			return true;
		}
	case GER_USER_LEAVE:		//�û�ǿ��
	case GER_NETWORK_ERROR:
		{
			KillAllTimer();

			//�������
			CMD_S_GameEnd& GameEnd = m_stGameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//ʣ���˿�
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//ͳ�ƻ���
			GameEnd.lGameScore[wChairID] = -30L * (SCORE)(m_pITableFrame->GetCellScore());
			GameEnd.lGameScore[(wChairID + 1) % m_wPlayerCount] = 15L * (SCORE)(m_pITableFrame->GetCellScore());
			GameEnd.lGameScore[(wChairID + 2) % m_wPlayerCount] = 15L * (SCORE)(m_pITableFrame->GetCellScore());
	

			//�޸Ļ���
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==wChairID)
				{
					ScoreInfo[i].lScore=GameEnd.lGameScore[wChairID];
					ScoreInfo[i].cbType=SCORE_TYPE_FLEE;
				}
				else
				{
					ScoreInfo[i].lScore=GameEnd.lGameScore[i];
					ScoreInfo[i].cbType=SCORE_TYPE_WIN;

					//����˰��
					ScoreInfo[i].lRevenue=m_pITableFrame->CalculateRevenue(i,ScoreInfo[i].lScore);
					if (ScoreInfo[i].lRevenue>0L) ScoreInfo[i].lScore-=ScoreInfo[i].lRevenue;
				}

				//��ʷ����
				GameEnd.lGameScore[i]=ScoreInfo[i].lScore;
				m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);
			}			
			
			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(GameEnd.remainCardCount), &remaincard, sizeof(remaincard));
			CopyMemory(GameEnd.bBombCount, m_bBombCount, sizeof(m_bBombCount));
			CopyMemory(GameEnd.lBombScore, m_lBombScore, sizeof(m_lBombScore));

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			m_pITableFrame->WriteTableScore(ScoreInfo, m_pITableFrame->GetChairCount());
			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

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
	case GAME_SCENE_FREE:		//����״̬
		{
			//˽�˷�������Ϸģʽ
			if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0)
			{
				BYTE *pGameRule = m_pITableFrame->GetGameRule();

				m_wPlayerCount = pGameRule[1];				//�������
				m_bSixTheenCard = pGameRule[2] != 0;		//16����
				m_bNoForceClose = true;						//pGameRule[3] != 0;		//����ǿ��
				m_bNoRemoveBomb = false;						//pGameRule[4] != 0;		//ը�����ɲ�
				m_bSpadeBanker = pGameRule[5] != 0;		//����3ׯ
				//m_bFirstOutSpade= pGameRule[6] != 0;		//�׳��ش�����3(����)

				m_bForceOutCard = pGameRule[7] != 0;
				m_bEchelonConclude = pGameRule[8] != 0;

				if ((SCORE)(m_pITableFrame->GetCellScore()) == 0)
				{
					CTraceService::TraceString(TEXT("CellScore=0"), TraceLevel_Exception);
					m_pITableFrame->SetCellScore(1);
				}

				m_lCellScore = (SCORE)(m_pITableFrame->GetCellScore());
			}
			else
			{
				m_pITableFrame->SetTableChairCount(m_wPlayerCount);
			}

			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//��ʷ����
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusFree.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//���ñ���
			StatusFree.lBaseScore=m_lCellScore;

			//�Զ�����
// 			StatusFree.wPlayerCount   = m_pGameCustomRule->wPlayerCount;
// 			StatusFree.bSixTheenCard  = m_pGameCustomRule->bSixTheenCard;
// 			StatusFree.bNoForceClose  = m_pGameCustomRule->bNoForceClose;
// 			StatusFree.bNoRemoveBomb  = m_pGameCustomRule->bNoRemoveBomb;
// 			StatusFree.bSpadeBanker   = m_pGameCustomRule->bSpadeBanker;
// 			StatusFree.bFirstOutSpade = m_pGameCustomRule->bFirstOutSpade;

			StatusFree.wPlayerCount   = m_wPlayerCount;
			StatusFree.bSixTheenCard  = m_bSixTheenCard;
			StatusFree.bNoForceClose  = m_bNoForceClose;
			StatusFree.bNoRemoveBomb  = m_bNoRemoveBomb;
			StatusFree.bSpadeBanker   = m_bSpadeBanker;
			StatusFree.bFirstOutSpade = m_bFirstOutSpade;

			StatusFree.bForceOutCard = m_bForceOutCard;
			StatusFree.bEchelonConclude = m_bEchelonConclude;
			StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			StatusFree.wServerLevel = m_pGameServiceOption->wServerLevel;

			CopyMemory(StatusFree.bAutoStatus,m_bAutoPlayer,sizeof(StatusFree.bAutoStatus));

			//����ģʽ
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				CMD_S_RoomCardRecord RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

				if (m_pITableFrame->GetDataBaseMode() == 0)
				{
					CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
				}
				else
				{
					WORD wChairID = pIServerUserItem->GetChairID();
					POSITION pos = m_listWinScoreRecord[wChairID].GetHeadPosition();

					WORD wIndex = 0;
					while (pos)
					{
						RoomCardRecord.lDetailScore[wChairID][wIndex++] = m_listWinScoreRecord[wChairID].GetNext(pos);

						if (wIndex >= MAX_RECORD_COUNT)
						{
							break;
						}
					}

					RoomCardRecord.nCount = wIndex;
				}

				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			}

			InitializeListRemainCard();
			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(StatusFree.remainCardCount), &remaincard, sizeof(remaincard));

			//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
				|| (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
			{
				//����ȡ���йܶ�ʱ��
				//m_pITableFrame->KillGameTimer(IDI_USER_TRUSTEE_0 + wChairID);
				StopTimer(IDI_USER_TRUSTEE_MIN + wChairID);
				pIServerUserItem->SetTrusteeUser(false);
				m_bAutoPlayer[wChairID] = false;
			}
			//ʱ������
			StatusFree.cbTimeoutOutCard = m_pGameCustomRule->cbTimeoutOutCard;				//���Ƴ�ʱ
			StatusFree.cbTimeoutGameEnd = m_pGameCustomRule->cbTimeoutGameEnd;				//���㳬ʱ
			StatusFree.cbTimeoutReady = m_pGameCustomRule->cbTimeoutReady;					//׼����ʱ

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SCENE_PLAY:		//��Ϸ״̬
		{
			//��������
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//���ñ���
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wLastOutUser=m_wTurnWiner;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.wBeforWiner=m_wBeforWiner;
			StatusPlay.bTurnCardCount=m_bTurnCardCount;
			StatusPlay.lBaseScore=m_lCellScore;
			StatusPlay.wRoomType=m_pGameServiceOption->wServerType;
			CopyMemory(StatusPlay.bBombCount,m_bBombCount,sizeof(m_bBombCount));
			CopyMemory(StatusPlay.bCardCount,m_bCardCount,sizeof(m_bCardCount));
			CopyMemory(StatusPlay.bTurnCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
			CopyMemory(StatusPlay.bCardData,m_bHandCardData[wChairID],sizeof(BYTE)*m_bCardCount[wChairID]);
			CopyMemory(StatusPlay.bAutoStatus,m_bAutoPlayer,sizeof(StatusPlay.bAutoStatus));

			//�Զ�����
// 			StatusPlay.wPlayerCount   = m_pGameCustomRule->wPlayerCount;
// 			StatusPlay.bSixTheenCard  = m_pGameCustomRule->bSixTheenCard;
// 			StatusPlay.bNoForceClose  = m_pGameCustomRule->bNoForceClose;
// 			StatusPlay.bNoRemoveBomb  = m_pGameCustomRule->bNoRemoveBomb;
// 			StatusPlay.bSpadeBanker   = m_pGameCustomRule->bSpadeBanker;
// 			StatusPlay.bFirstOutSpade = m_pGameCustomRule->bFirstOutSpade;

			StatusPlay.wPlayerCount   = m_wPlayerCount;
			StatusPlay.bSixTheenCard  = m_bSixTheenCard;
			StatusPlay.bNoForceClose  = m_bNoForceClose;
			StatusPlay.bNoRemoveBomb  = m_bNoRemoveBomb;
			StatusPlay.bSpadeBanker   = m_bSpadeBanker;

			StatusPlay.cbSpadeBankerFirstOutCardData = m_cbSpadeBankerFirstOutCardData;
			StatusPlay.cbWinBankerFirstOutCardData = m_cbWinBankerFirstOutCardData;

			StatusPlay.bFirstOutSpade = m_bFirstOutSpade;

			StatusPlay.bForceOutCard = m_bForceOutCard;
			StatusPlay.bEchelonConclude = m_bEchelonConclude;
			StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			StatusPlay.wServerLevel = m_pGameServiceOption->wServerLevel;

			//��ʷ����
			CopyMemory(StatusPlay.lAllTurnScore,m_lAllTurnScore,sizeof(m_lAllTurnScore));
			CopyMemory(StatusPlay.lLastTurnScore,m_lLastTurnScore,sizeof(m_lLastTurnScore));

			//��ʷ����
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusPlay.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//����ģʽ
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				CMD_S_RoomCardRecord RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

				if (m_pITableFrame->GetDataBaseMode() == 0)
				{
					CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
				}
				else
				{
					WORD wChairID = pIServerUserItem->GetChairID();
					POSITION pos = m_listWinScoreRecord[wChairID].GetHeadPosition();

					WORD wIndex = 0;
					while (pos)
					{
						RoomCardRecord.lDetailScore[wChairID][wIndex++] = m_listWinScoreRecord[wChairID].GetNext(pos);

						if (wIndex >= MAX_RECORD_COUNT)
						{
							break;
						}
					}

					RoomCardRecord.nCount = wIndex;
				}

				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			}

			//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
				|| (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
			{
				//����ȡ���йܶ�ʱ��
				StopTimer(IDI_USER_TRUSTEE_MIN + wChairID);
				pIServerUserItem->SetTrusteeUser(false);
				m_bAutoPlayer[wChairID] = false;
			}

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(StatusPlay.remainCardCount), &remaincard, sizeof(remaincard));

			StatusPlay.cbTimeRemain = GetStatusLeftTime();
			//ʱ������
			StatusPlay.cbTimeoutOutCard = m_pGameCustomRule->cbTimeoutOutCard;				//���Ƴ�ʱ
			StatusPlay.cbTimeoutGameEnd = m_pGameCustomRule->cbTimeoutGameEnd;				//���㳬ʱ
			StatusPlay.cbTimeoutReady = m_pGameCustomRule->cbTimeoutReady;					//׼����ʱ

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	case GAME_SCENE_WAITEND:	//����״̬
	{
			//˽�˷�������Ϸģʽ
			if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0)
			{
				BYTE *pGameRule = m_pITableFrame->GetGameRule();

				m_wPlayerCount = pGameRule[1];				//�������
				m_bSixTheenCard = pGameRule[2] != 0;		//16����
				m_bNoForceClose = true;						//pGameRule[3] != 0;		//����ǿ��
				m_bNoRemoveBomb = false;						//pGameRule[4] != 0;		//ը�����ɲ�
				m_bSpadeBanker = pGameRule[5] != 0;		//����3ׯ
				//m_bFirstOutSpade= pGameRule[6] != 0;		//�׳��ش�����3(����)

				m_bForceOutCard = pGameRule[7] != 0;
				m_bEchelonConclude = pGameRule[8] != 0;

				if ((SCORE)(m_pITableFrame->GetCellScore()) == 0)
				{
					CTraceService::TraceString(TEXT("CellScore=0"), TraceLevel_Exception);
					m_pITableFrame->SetCellScore(1);
				}

				m_lCellScore = (SCORE)(m_pITableFrame->GetCellScore());
			}
			else
			{
				m_pITableFrame->SetTableChairCount(m_wPlayerCount);
			}

			//��������
			CMD_S_StatusGameEnd Status;
			ZeroMemory(&Status, sizeof(Status));
			//��ʷ����
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				tagHistoryScore * pHistoryScore = m_HistoryScore.GetHistoryScore(i);
				Status.lTurnScore[i] = pHistoryScore->lTurnScore;
				Status.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			//���ñ���
			Status.lBaseScore = m_lCellScore;

			Status.wPlayerCount = m_wPlayerCount;
			Status.bSixTheenCard = m_bSixTheenCard;
			Status.bNoForceClose = m_bNoForceClose;
			Status.bNoRemoveBomb = m_bNoRemoveBomb;
			Status.bSpadeBanker = m_bSpadeBanker;
			Status.bFirstOutSpade = m_bFirstOutSpade;

			Status.bForceOutCard = m_bForceOutCard;
			Status.bEchelonConclude = m_bEchelonConclude;
			Status.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			Status.wServerLevel = m_pGameServiceOption->wServerLevel;

			CopyMemory(Status.bAutoStatus, m_bAutoPlayer, sizeof(Status.bAutoStatus));

			//����ģʽ
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				CMD_S_RoomCardRecord RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

				if (m_pITableFrame->GetDataBaseMode() == 0)
				{
					CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
				}
				else
				{
					WORD wChairID = pIServerUserItem->GetChairID();
					POSITION pos = m_listWinScoreRecord[wChairID].GetHeadPosition();

					WORD wIndex = 0;
					while (pos)
					{
						RoomCardRecord.lDetailScore[wChairID][wIndex++] = m_listWinScoreRecord[wChairID].GetNext(pos);

						if (wIndex >= MAX_RECORD_COUNT)
						{
							break;
						}
					}

					RoomCardRecord.nCount = wIndex;
				}

				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			}

			InitializeListRemainCard();
			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(Status.remainCardCount), &remaincard, sizeof(remaincard));

			//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
				|| (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
			{
				//����ȡ���йܶ�ʱ��
				//m_pITableFrame->KillGameTimer(IDI_USER_TRUSTEE_0 + wChairID);
				StopTimer(IDI_USER_TRUSTEE_MIN + wChairID);
				pIServerUserItem->SetTrusteeUser(false);
				m_bAutoPlayer[wChairID] = false;
			}
			//ʱ������
			Status.cbTimeoutOutCard = m_pGameCustomRule->cbTimeoutOutCard;				//���Ƴ�ʱ
			Status.cbTimeoutGameEnd = m_pGameCustomRule->cbTimeoutGameEnd;				//���㳬ʱ
			Status.cbTimeoutReady = m_pGameCustomRule->cbTimeoutReady;					//׼����ʱ
			//��������
			CopyMemory(&Status.stGameEnd, &m_stGameEnd, sizeof(Status.stGameEnd));
			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem, &Status, sizeof(Status));
	}break;
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	if (wTimerID == IDI_WAITEND)
	{
		if (m_pITableFrame->GetGameStatus() != GAME_SCENE_FREE)
		{
			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
		}
		return true;
	}
	if (wTimerID >= IDI_USER_TRUSTEE_MIN && wTimerID < IDI_USER_TRUSTEE_MAX)
	{
		StopTimer(wTimerID);

		//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
		if (!(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
			|| (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0))
		{
			return true;
		}

		WORD wTrusteeChairID = wTimerID - IDI_USER_TRUSTEE_MIN;
		BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
			
		//
		ASSERT(m_pITableFrame->GetTableUserItem(wTrusteeChairID)->IsTrusteeUser());

		//�жϵ�ǰ���
		if (wTrusteeChairID == m_wCurrentUser && m_pITableFrame->GetTableUserItem(wTrusteeChairID)->IsTrusteeUser())
		{
			switch (cbGameStatus)
			{
			case GAME_SCENE_PLAY:
				{
					if (m_bTurnCardCount == 0)
					{
						//�˿˷���
						tagOutCardResult OutCardResult;	
						ZeroMemory(&OutCardResult, sizeof(OutCardResult));

						m_GameLogic.SearchOutCard(m_bHandCardData[m_wCurrentUser], m_bCardCount[m_wCurrentUser], m_bTurnCardData, m_bTurnCardCount, m_bTurnCardType, m_bNoRemoveBomb, OutCardResult);

						if (OutCardResult.cbCardCount == 0)
						{
							CTraceService::TraceString(TEXT("OutCardResult"), TraceLevel_Exception);
						}
						OnUserOutCard(m_wCurrentUser, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					}
					else
					{
						//���д�س�
						if (!m_bForceOutCard)
						{
							OnUserPassCard(m_wCurrentUser);
						}
						//�д�س�
						else
						{
							//�˿˷���
							tagOutCardResult OutCardResult;
							ZeroMemory(&OutCardResult, sizeof(OutCardResult));

							m_GameLogic.SearchOutCard(m_bHandCardData[m_wCurrentUser], m_bCardCount[m_wCurrentUser], m_bTurnCardData, m_bTurnCardCount, m_bTurnCardType, m_bNoRemoveBomb, OutCardResult);

							if (OutCardResult.cbCardCount != 0)
							{
								OnUserOutCard(m_wCurrentUser, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
							}
							else
							{
								OnUserPassCard(m_wCurrentUser);
							}
						}
					}

					break;
				}
			}

			return true;
		}
	}
	else if (wTimerID == IDI_SO_OPERATE)
	{
		StopTimer(wTimerID);

		//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
		if (!(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
			|| (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0))
		{
			return true;
		}

		if (m_wCurrentUser != INVALID_CHAIR)
		{
			m_bAutoPlayer[m_wCurrentUser] = true;
			m_pITableFrame->GetTableUserItem(m_wCurrentUser)->SetTrusteeUser(true);

			CMD_S_UserAutomatism UserAutomatismData;
			ZeroMemory(&UserAutomatismData, sizeof(UserAutomatismData));

			UserAutomatismData.bTrusee = true;
			UserAutomatismData.wChairID = m_wCurrentUser;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_AUTOMATISM, &UserAutomatismData, sizeof(UserAutomatismData));

			BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
			switch (cbGameStatus)
			{
			case GAME_SCENE_PLAY:
			{
				if (m_bTurnCardCount == 0)
				{
					//�˿˷���
					tagOutCardResult OutCardResult;
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));

					m_GameLogic.SearchOutCard(m_bHandCardData[m_wCurrentUser], m_bCardCount[m_wCurrentUser], m_bTurnCardData, m_bTurnCardCount, m_bTurnCardType, m_bNoRemoveBomb, OutCardResult);

					if (OutCardResult.cbCardCount == 0)
					{
						CTraceService::TraceString(TEXT("OutCardResult"), TraceLevel_Exception);
					}
					OnUserOutCard(m_wCurrentUser, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				}
				else
				{
					//���д�س�
					if (!m_bForceOutCard)
					{
						OnUserPassCard(m_wCurrentUser);
					}
					//�д�س�
					else
					{
						//�˿˷���
						tagOutCardResult OutCardResult;
						ZeroMemory(&OutCardResult, sizeof(OutCardResult));

						m_GameLogic.SearchOutCard(m_bHandCardData[m_wCurrentUser], m_bCardCount[m_wCurrentUser], m_bTurnCardData, m_bTurnCardCount, m_bTurnCardType, m_bNoRemoveBomb, OutCardResult);

						if (OutCardResult.cbCardCount != 0)
						{
							OnUserOutCard(m_wCurrentUser, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
						}
						else
						{
							OnUserPassCard(m_wCurrentUser);
						}
					}
				}

				break;
			}

			default:
				break;
			}
		}

		return true;
	}

	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
	|| (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
	{
		//���ǵ�ǰ���
		if (wChairID != m_wCurrentUser && m_wCurrentUser != INVALID_CHAIR)
		{
			//�����йܱ�־
			pIServerUserItem->SetTrusteeUser(true);
		}
	}

	return true;
}

//�û�����
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʷ����
	if (bLookonUser==false) m_HistoryScore.OnEventUserEnter(wChairID);

// 	CStringA lyt;
// 	lyt.Format("lytlog::OnActionUserSitDown()---------------------");
// 	OutputDebugStringA(lyt);
	return true;
}

//�û�����
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʷ����
	if (bLookonUser==false) m_HistoryScore.OnEventUserLeave(wChairID);

	m_wBeforWiner = INVALID_CHAIR;

	//Լս��ɢ
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_pITableFrame->IsPersonalRoomDisumme())
	{
		ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
	}

	//��ҷ���
	if ((m_pITableFrame->GetDataBaseMode() == 1) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
	{
		m_listWinScoreRecord[wChairID].RemoveAll();
	}

	return true;
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_AUTOMATISM:
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_Automatism));
			if (wDataSize!=sizeof(CMD_C_Automatism)) return false;

			//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
			if (!(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
				|| (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0))
			{
				return false;
			}

			CMD_C_Automatism * pAutomatism =(CMD_C_Automatism *)pData;

			m_bAutoPlayer[pIServerUserItem->GetChairID()]=pAutomatism->bAutomatism;

			CMD_S_UserAutomatism UserAutomatismData;
			UserAutomatismData.bTrusee=pAutomatism->bAutomatism;
			UserAutomatismData.wChairID=pIServerUserItem->GetChairID();
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_AUTOMATISM,&UserAutomatismData,sizeof(UserAutomatismData));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_AUTOMATISM,&UserAutomatismData,sizeof(UserAutomatismData));

			//ɾ���йܶ�ʱ��
			//m_pITableFrame->KillGameTimer(IDI_USER_TRUSTEE_0 + pIServerUserItem->GetChairID());
			StopTimer(IDI_USER_TRUSTEE_MIN + pIServerUserItem->GetChairID());

			//
			if (pAutomatism->bAutomatism)
			{
				//�����йܱ�־
				pIServerUserItem->SetTrusteeUser(true);
				//m_pITableFrame->SetGameTimer(IDI_USER_TRUSTEE_0 + pIServerUserItem->GetChairID(), 1000, -1, NULL);
				StartTimer(IDI_USER_TRUSTEE_MIN + pIServerUserItem->GetChairID(), pIServerUserItem->GetChairID());
			}
			else
			{
				WORD wChairID = pIServerUserItem->GetChairID();
				pIServerUserItem->SetTrusteeUser(false);
			}

			return true;
		}
	case SUB_C_OUT_CART:		//�û�����
		{
			//��������
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->bCardData);

			//Ч������
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//��Ϣ����
			return OnUserOutCard(pUserData->wChairID,pOutCard->bCardData,pOutCard->bCardCount, true);
		}
	case SUB_C_PASS_CARD:
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//��Ϣ����
			return OnUserPassCard(pUserData->wChairID);
		}
	case SUB_C_REQUEST_RCRecord:
		{
			if (!((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				return true;
			}

			if (m_pITableFrame->IsPersonalRoomDisumme())
			{
				return true;
			}

			ASSERT(pIServerUserItem->IsMobileUser());
			if (!pIServerUserItem->IsMobileUser())
			{
				return false;
			}

			CMD_S_RoomCardRecord RoomCardRecord;
			ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

			CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));

			return true;
		}
	}


	return false;
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
#ifdef  CARD_DISPATCHER_CONTROL
	// ��Ϣ����
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
		case SUB_GF_FRAME_MESSAG_GAME_MAX - SUB_GF_FRAME_MESSAG_GAME_MIN:
		{
			if (wDataSize == 0)
			{
				m_bConfigCard = false;
			}
			else
			{
				ASSERT(wDataSize == sizeof(m_cbConfigItemInfo));
				if (wDataSize != sizeof(m_cbConfigItemInfo))
				{
					return false;
				}

				m_bConfigCard = true;
				CopyMemory(m_cbConfigItemInfo, pData, sizeof(m_cbConfigItemInfo));
			}

			return false;
		}
		default:
			break;
		}
	}
#endif

	return false;
}

//�û�����
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount, bool bClient)
{
	//Ч��״̬
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY)
	{
		//д��־
		CString strTrace = TEXT("GAME_SCENEINVALID");
		CString strFileName;
		strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

		tagLogUserInfo LogUserInfo;
		ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
		CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
		CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
		m_pITableFrame->SendGameLogData(LogUserInfo);

		return false;
	}

	//Ч�鵱ǰ���
	if (wChairID != m_wCurrentUser)
	{
		//д��־
		CString strTrace = TEXT("m_wCurrentUserINVALID");
		CString strFileName;
		strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

		tagLogUserInfo LogUserInfo;
		ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
		CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
		CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
		m_pITableFrame->SendGameLogData(LogUserInfo);

		return false;
	}

	//Ч������
	BYTE cbTempCardData[NORMAL_CARD_COUNT];
	ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
	CopyMemory(cbTempCardData, bCardData, sizeof(BYTE)* bCardCount);

	//�����򣨵���˳�ӱ���Ӵ�С���㣩
	m_GameLogic.SortCardList(cbTempCardData, bCardCount);

	BYTE cbCardType = m_GameLogic.GetCardType(cbTempCardData, bCardCount);
	if (cbCardType == CT_ERROR)
	{
		CString strCardData;
		for (WORD i = 0; i < bCardCount; i++)
		{
			CString str;
			str.Format(TEXT("[%d]"), bCardData[i]);
			strCardData += str;
		}

		CString strTrace;
		strTrace.Format(TEXT("GAMEID = %d, CardType == CT_ERROR�� android = %d , bClient = %d"), m_pITableFrame->GetTableUserItem(wChairID)->GetGameID(), m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser(), bClient);

		//д��־
		CString strFileName;
		strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

		tagLogUserInfo LogUserInfo;
		ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
		CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
		CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
		m_pITableFrame->SendGameLogData(LogUserInfo);

		CopyMemory(LogUserInfo.szLogContent, strCardData, sizeof(LogUserInfo.szLogContent));
		m_pITableFrame->SendGameLogData(LogUserInfo);

		return false;
	}

	if (m_bSpadeBanker && m_cbSpadeBankerFirstOutCardData != INVALID_BYTE)
	{
		bool bValid = false;
		for (WORD i = 0; i < bCardCount; i++)
		{
			if (bCardData[i] == m_cbSpadeBankerFirstOutCardData)
			{
				bValid = true;
				break;
			}
		}

		if (!bValid)
		{
			CString strTrace;
			strTrace.Format(TEXT("m_cbSpadeBankerFirstOutCardData = %d"), m_cbSpadeBankerFirstOutCardData);

			//д��־
			CString strFileName;
			strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

			tagLogUserInfo LogUserInfo;
			ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
			CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
			m_pITableFrame->SendGameLogData(LogUserInfo);

			return false;
		}
	}
	else if (!m_bSpadeBanker && m_cbWinBankerFirstOutCardData != INVALID_BYTE)
	{
		bool bValid = false;
		for (WORD i = 0; i < bCardCount; i++)
		{
			if (bCardData[i] == m_cbWinBankerFirstOutCardData)
			{
				bValid = true;
				break;
			}
		}

		if (!bValid)
		{
			CString strTrace;
			strTrace.Format(TEXT("m_cbWinBankerFirstOutCardData = %d"), m_cbWinBankerFirstOutCardData);

			//д��־
			CString strFileName;
			strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

			tagLogUserInfo LogUserInfo;
			ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
			CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
			m_pITableFrame->SendGameLogData(LogUserInfo);

			return false;
		}
	}

	//У������
	//���һ����������Ȩ���Գ�
	if (cbCardType == CT_THREE_LINE || cbCardType == CT_THREE_TAKE_SINGLE
	 || cbCardType == CT_THREE_LINE_TAKE_N || cbCardType == CT_THREE_LINE_TAKE_MASS)
	{
		if (m_bTurnCardCount != 0)
		{
			//д��־
			CString strTrace = TEXT("m_bTurnCardCount!=0");
			CString strFileName;
			strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

			tagLogUserInfo LogUserInfo;
			ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
			CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
			m_pITableFrame->SendGameLogData(LogUserInfo);

			return false;
		}

		//У���Ƿ����һ����
		BYTE cbTempCardCount = m_bCardCount[wChairID];
		BYTE cbTempHandCardData[16];
		ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
		CopyMemory(cbTempHandCardData, m_bHandCardData[wChairID], sizeof(cbTempHandCardData));

		if (!(m_GameLogic.RemoveCard(bCardData, bCardCount, cbTempHandCardData, cbTempCardCount) == true && cbTempCardCount == bCardCount))
		{
			//д��־
			CString strTrace = TEXT("NOLASTCARD!=0");
			CString strFileName;
			strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

			tagLogUserInfo LogUserInfo;
			ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
			CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
			m_pITableFrame->SendGameLogData(LogUserInfo);

			return false;
		}
	}

	//�Ƚϴ�С
	if (m_bTurnCardCount != 0)
	{
		BYTE cbTempCardData[NORMAL_CARD_COUNT];
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
		CopyMemory(cbTempCardData, bCardData, sizeof(BYTE)* bCardCount);

		//�����򣨵���˳�ӱ���Ӵ�С���㣩
		m_GameLogic.SortCardList(cbTempCardData, bCardCount);

		BYTE cbTempTurnCardData[NORMAL_CARD_COUNT];
		ZeroMemory(cbTempTurnCardData, sizeof(cbTempTurnCardData));
		CopyMemory(cbTempTurnCardData, m_bTurnCardData, sizeof(BYTE)* m_bTurnCardCount);

		//�����򣨵���˳�ӱ���Ӵ�С���㣩
		m_GameLogic.SortCardList(cbTempTurnCardData, m_bTurnCardCount);

		if (m_GameLogic.CompareCard(cbTempCardData, cbTempTurnCardData, bCardCount, m_bTurnCardCount) == false)
		{
			CString strTurnCardData;
			for (WORD i = 0; i < m_bTurnCardCount; i++)
			{
				CString str;
				str.Format(TEXT("[%d]"), m_bTurnCardData[i]);
				strTurnCardData += str;
			}

			CString strCardData;
			for (WORD i = 0; i < bCardCount; i++)
			{
				CString str;
				str.Format(TEXT("[%d]"), bCardData[i]);
				strCardData += str;
			}

			CString strTrace;
			strTrace.Format(TEXT("GAMEID = %d, CompareInvalid�� android = %d, bClient = %d"), m_pITableFrame->GetTableUserItem(wChairID)->GetGameID(), m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser(), bClient);

			//д��־
			CString strFileName;
			strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

			tagLogUserInfo LogUserInfo;
			ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
			CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
			m_pITableFrame->SendGameLogData(LogUserInfo);

			CopyMemory(LogUserInfo.szLogContent, strTurnCardData, sizeof(LogUserInfo.szLogContent));
			m_pITableFrame->SendGameLogData(LogUserInfo);

			CopyMemory(LogUserInfo.szLogContent, strCardData, sizeof(LogUserInfo.szLogContent));
			m_pITableFrame->SendGameLogData(LogUserInfo);

			return false;
		}
	}

	//Ч��ɾ��
	BYTE cbTempCardCount = m_bCardCount[wChairID];
	BYTE cbTempHandCardData[16];
	ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
	CopyMemory(cbTempHandCardData, m_bHandCardData[wChairID], sizeof(cbTempHandCardData));

	if (m_GameLogic.RemoveCard(bCardData, bCardCount, m_bHandCardData[wChairID], m_bCardCount[wChairID]) == false)
	{
		CString strActualCardData;
		for (WORD i = 0; i < bCardCount; i++)
		{
			CString str;
			str.Format(TEXT("[%d]"), bCardData[i]);
			strActualCardData += str;
		}

		CString strHandCardData;
		for (WORD i = 0; i < m_bCardCount[wChairID]; i++)
		{
			CString str;
			str.Format(TEXT("[%d]"), m_bHandCardData[wChairID][i]);
			strHandCardData += str;
		}

		CString strTrace;
		strTrace.Format(TEXT("GAMEID = %d, RemoveCardInvalid�� android = %d, bClient = %d"), m_pITableFrame->GetTableUserItem(wChairID)->GetGameID(), m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser(), bClient);

		//д��־
		CString strFileName;
		strFileName.Format(TEXT("�ܵÿ�[%s]����"), m_pGameServiceOption->szServerName);

		tagLogUserInfo LogUserInfo;
		ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
		CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
		CopyMemory(LogUserInfo.szLogContent, strTrace, sizeof(LogUserInfo.szLogContent));
		m_pITableFrame->SendGameLogData(LogUserInfo);

		CopyMemory(LogUserInfo.szLogContent, strActualCardData, sizeof(LogUserInfo.szLogContent));
		m_pITableFrame->SendGameLogData(LogUserInfo);

		CopyMemory(LogUserInfo.szLogContent, strHandCardData, sizeof(LogUserInfo.szLogContent));
		m_pITableFrame->SendGameLogData(LogUserInfo);

		return false;
	}

	//ը���ж�, ABC��һֱ������ը��ֻ�����һ������ը���˵ķ�
	//if (cbCardType == CT_BOMB)
	//{
	//	m_bBombCount[wChairID]++;

	//	//2��
	//	if (m_wPlayerCount == 2)
	//	{
	//		m_lBombScore[wChairID] += (10 * (SCORE)(m_pITableFrame->GetCellScore()));
	//		m_lBombScore[(wChairID + 1) % m_wPlayerCount] += (-10 * (SCORE)(m_pITableFrame->GetCellScore()));
	//	}
	//	else if (m_wPlayerCount == GAME_PLAYER)
	//	{
	//		m_lBombScore[wChairID] += (20 * (SCORE)(m_pITableFrame->GetCellScore()));
	//		m_lBombScore[(wChairID + 1) % m_wPlayerCount] += (-10 * (SCORE)(m_pITableFrame->GetCellScore()));
	//		m_lBombScore[(wChairID + 2) % m_wPlayerCount] += (-10 * (SCORE)(m_pITableFrame->GetCellScore()));
	//	}
	//}

	//�л��û�
	m_bCardCount[wChairID] -= bCardCount;
	m_wTurnWiner=wChairID;
	if (m_bCardCount[wChairID] != 0)
	{
		m_wCurrentUser = (m_wCurrentUser + 1) % m_wPlayerCount;
	}
	else
	{
		m_wCurrentUser = INVALID_CHAIR;

		//���һ����ը��Ҳ��ը��
		if (cbCardType == CT_BOMB)
		{
			m_bBombCount[wChairID]++;

			//2��
			if (m_wPlayerCount == 2)
			{
				m_lBombScore[wChairID] += (10 * (SCORE)(m_pITableFrame->GetCellScore()));
				m_lBombScore[(wChairID + 1) % m_wPlayerCount] += (-10 * (SCORE)(m_pITableFrame->GetCellScore()));
			}
			else if (m_wPlayerCount == GAME_PLAYER)
			{
				m_lBombScore[wChairID] += (20 * (SCORE)(m_pITableFrame->GetCellScore()));
				m_lBombScore[(wChairID + 1) % m_wPlayerCount] += (-10 * (SCORE)(m_pITableFrame->GetCellScore()));
				m_lBombScore[(wChairID + 2) % m_wPlayerCount] += (-10 * (SCORE)(m_pITableFrame->GetCellScore()));
			}
		}
	}

	m_cbSpadeBankerFirstOutCardData = INVALID_BYTE;
	m_cbWinBankerFirstOutCardData = INVALID_BYTE;

	//�����ж�
	//����Ȩ���¼�ʣһ����
	//�г���Ȩ��������ȳ������д���һ������ϵ��ƣ�������ӡ�������˳�ӵȣ���������¼��ж�Ϊ���߰���
	if (m_wCurrentUser != INVALID_CHAIR && m_bTurnCardCount == 0 && m_bCardCount[(wChairID + 1) % m_wPlayerCount] == 1)
	{
		BYTE cbResultCount = 0;
		m_GameLogic.SearchOutCard(cbTempHandCardData, cbTempCardCount, m_bNoRemoveBomb, cbResultCount);

		//�ж�����ȫ��
		//����ȫ�������������������ȳ�����������¼��ж�Ϊ���߰���
		if (cbResultCount == 0)
		{
			if (bCardCount == 1 && bCardData[0] != cbTempHandCardData[0])
			{
				m_wBaoPeiUser = wChairID;
			}
		}
		else
		{
			if (!(cbCardType >= CT_SINGLE_LINE))
			{
				m_wBaoPeiUser = wChairID;
			}
		}
	}
	//���Ƶ���ұ������ȳ��������ܽӵ��������
	else if (m_wCurrentUser != INVALID_CHAIR && m_bTurnCardCount != 0 && m_bCardCount[(wChairID + 1) % m_wPlayerCount] == 1)
	{
		if (cbCardType == CT_SINGLE && m_bTurnCardCount == 1 && bCardData[0] != cbTempHandCardData[0])
		{
			m_wBaoPeiUser = wChairID;
		}
	}

	//���Ƽ�¼
	m_bTurnCardType = cbCardType;
	m_bTurnCardCount = bCardCount;
	CopyMemory(m_bTurnCardData, bCardData, sizeof(BYTE)*bCardCount);

	//��������
	CMD_S_OutCard OutCard;
	ZeroMemory(&OutCard, sizeof(OutCard));

	OutCard.bCardCount = bCardCount;
	OutCard.wOutCardUser = wChairID;
	OutCard.wCurrentUser = m_wCurrentUser;
	CopyMemory(OutCard.bCardData, m_bTurnCardData, m_bTurnCardCount*sizeof(BYTE));

	for (WORD i = 0; i<bCardCount; i++)
	{
		POSITION ptkey = m_llistRemainCard.Find(bCardData[i]);

		if (ptkey != NULL)
		{
			m_llistRemainCard.RemoveAt(ptkey);
		}
	}

	REMAINCARDCOUNT remaincard;
	ZeroMemory(&remaincard, sizeof(remaincard));
	GetRemainCard(remaincard);

	CopyMemory(&(OutCard.remainCardCount), &remaincard, sizeof(remaincard));

	//��������
	WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_OUT_CARD, &OutCard);
	}

	//�����ж�
	if (m_wCurrentUser == INVALID_CHAIR)
	{
		OnEventGameConclude(wChairID, NULL, GER_NORMAL);
		return true;
	}
	else
	{
		if (m_pITableFrame->GetTableUserItem(m_wCurrentUser) && m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
		{
			StartTimer(IDI_USER_TRUSTEE_MIN + m_wCurrentUser, m_wCurrentUser);
		}

		//��ҳ��ͽ�ҷ��������ö�ʱ��
		if (!IsRoomCardType())
		{
			StopTimer(IDI_SO_OPERATE);
			StartTimer(IDI_SO_OPERATE);
		}
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//Ч��״̬
	if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;
	if ((wChairID!=m_wCurrentUser)||(m_bTurnCardCount==0)) return false;

	//�˿˷���
	tagOutCardResult OutCardResult;
	m_GameLogic.SearchOutCard(m_bHandCardData[wChairID],m_bCardCount[wChairID],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardType,m_bNoRemoveBomb,OutCardResult);

	//�����ж�
	if (OutCardResult.cbCardCount != 0 && m_bForceOutCard) return false;

	//���ñ���
	m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	if (m_wCurrentUser==m_wTurnWiner)
	{
		//ը���ж�, ABC��һֱ������ը��ֻ�����һ������ը���˵ķ�
		BYTE cbCardType = m_GameLogic.GetCardType(m_bTurnCardData, m_bTurnCardCount);
		if (cbCardType == CT_BOMB)
		{
			m_bBombCount[m_wTurnWiner]++;

			//2��
			if (m_wPlayerCount == 2)
			{
				m_lBombScore[m_wTurnWiner] += (10 * (SCORE)(m_pITableFrame->GetCellScore()));
				m_lBombScore[(m_wTurnWiner + 1) % m_wPlayerCount] += (-10 * (SCORE)(m_pITableFrame->GetCellScore()));
			}
			else if (m_wPlayerCount == GAME_PLAYER)
			{
				m_lBombScore[m_wTurnWiner] += (20 * (SCORE)(m_pITableFrame->GetCellScore()));
				m_lBombScore[(m_wTurnWiner + 1) % m_wPlayerCount] += (-10 * (SCORE)(m_pITableFrame->GetCellScore()));
				m_lBombScore[(m_wTurnWiner + 2) % m_wPlayerCount] += (-10 * (SCORE)(m_pITableFrame->GetCellScore()));
			}
		}

		//���ñ���
		m_bTurnCardType = CT_ERROR;
		m_bTurnCardCount = 0;
		ZeroMemory(m_bTurnCardData, sizeof(m_bTurnCardData));
	}

	//���ð������
	if (m_wPlayerCount == GAME_PLAYER && m_wBaoPeiUser != INVALID_CHAIR && wChairID == (m_wBaoPeiUser + 1) % m_wPlayerCount)
	{
		m_wBaoPeiUser = INVALID_CHAIR;
	}

	//��������
	CMD_S_PassCard PassCard;
	ZeroMemory(&PassCard, sizeof(PassCard));
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bNewTurn=(m_bTurnCardCount==0)?TRUE:FALSE;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_PASS_CARD, &PassCard);
	}

	if (m_pITableFrame->GetTableUserItem(m_wCurrentUser) && m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
	{
		StartTimer(IDI_USER_TRUSTEE_MIN + m_wCurrentUser, m_wCurrentUser);
	}

	//��ҳ��ͽ�ҷ��������ö�ʱ��
	if (!IsRoomCardType())
	{
		StopTimer(IDI_SO_OPERATE);
		StartTimer(IDI_SO_OPERATE);
	}

	return true;
}

void CTableFrameSink::InitializeListRemainCard()
{
	m_llistRemainCard.RemoveAll();

	//�����˿�
	BYTE bRandCard[48];
	ZeroMemory(bRandCard, sizeof(bRandCard));

	BYTE cbCardCount = (m_bSixTheenCard == true ? 48 : 45);

	m_GameLogic.RandCardList(bRandCard, sizeof(BYTE)*cbCardCount);

	for (WORD i = 0; i<cbCardCount; i++)
	{
		m_llistRemainCard.AddTail(bRandCard[i]);
	}
}

//��ȡʣ������
void CTableFrameSink::GetRemainCard(REMAINCARDCOUNT &remaincard)
{
	ZeroMemory(&remaincard, sizeof(remaincard));

	POSITION ptHead = m_llistRemainCard.GetHeadPosition();
	while (ptHead)
	{
		BYTE &cbCardData = m_llistRemainCard.GetNext(ptHead);
		BYTE cbLogicValue = m_GameLogic.GetCardLogicValue(cbCardData);

		switch (cbLogicValue)
		{
		case 15://2
		{
					remaincard.cbRemainCount[0]++;
					break;
		}
		case 14://A
		{
					remaincard.cbRemainCount[1]++;
					break;
		}

		case 13://K
		{
					remaincard.cbRemainCount[2]++;
					break;
		}
		case 12://Q
		{
					remaincard.cbRemainCount[3]++;
					break;
		}
		case 11://J
		{
					remaincard.cbRemainCount[4]++;
					break;
		}
		case 10://10
		{
					remaincard.cbRemainCount[5]++;
					break;
		}

		case 9://9
		{
				   remaincard.cbRemainCount[6]++;
				   break;
		}
		case 8://8
		{
				   remaincard.cbRemainCount[7]++;
				   break;
		}
		case 7://7
		{
				   remaincard.cbRemainCount[8]++;
				   break;
		}
		case 6://6
		{
				   remaincard.cbRemainCount[9]++;
				   break;
		}

		case 5://5
		{
				   remaincard.cbRemainCount[10]++;
				   break;
		}
		case 4://4
		{
				   remaincard.cbRemainCount[11]++;
				   break;
		}
		case 3://3
		{
				   remaincard.cbRemainCount[12]++;
				   break;
		}
		default:
			ASSERT(false);
		}
	}
}

//�жϷ�������
bool CTableFrameSink::IsRoomCardType()
{
	//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
	return (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 0);
}

/*
IDI_USER_TRUSTEE_MIN,							//����йܶ�ʱ��
IDI_USER_TRUSTEE_MAX = IDI_USER_TRUSTEE_MIN + GAME_PLAYER,
IDI_SO_OPERATE,									//��ʱ��ʱ��
IDI_WAITEND,									//����ȴ�
*/
void CTableFrameSink::StartTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	if (dwTimerID >= IDI_USER_TRUSTEE_MIN && dwTimerID < IDI_USER_TRUSTEE_MAX)
	{
		m_pITableFrame->SetGameTimer(dwTimerID, GetTimerConfigTime(dwTimerID) * 1000, 1, wBindParam);
		return;
	}

	m_ctStatusStartTime = CTime::GetCurrentTime();
	m_cbStatusConfigTime = GetTimerConfigTime(dwTimerID);
	m_pITableFrame->SetGameTimer(dwTimerID, DWORD(m_cbStatusConfigTime) * 1000, 1, wBindParam);
};
void CTableFrameSink::StopTimer(DWORD dwTimerID)
{
	m_pITableFrame->KillGameTimer(dwTimerID);
};
void CTableFrameSink::KillAllTimer()
{
	m_cbStatusConfigTime = 0;
	for (DWORD i = IDI_TIMER_NULL + 1; i < IDI_TIMER_MAX; ++i)
	{
		m_pITableFrame->KillGameTimer(i);
	}
};
//��ȡ��ʱ������ʱ��
int CTableFrameSink::GetTimerConfigTime(DWORD dwTimerID)
{
	if (dwTimerID >= IDI_USER_TRUSTEE_MIN && dwTimerID < IDI_USER_TRUSTEE_MAX)
	{
		return m_pGameCustomRule->cbTrusteeDelayTime;
	}

	switch (dwTimerID)
	{
	case IDI_SO_OPERATE:
		return m_pGameCustomRule->cbTimeoutOutCard;
	case IDI_WAITEND:
		return m_pGameCustomRule->cbTimeoutGameEnd;
	case IDI_GAME_FREE:
		return m_pGameCustomRule->cbTimeoutReady;
	default:
		//assert(false);
		break;
	}

	return 1;
};
//��ȡ״̬ʣ��ʱ��
BYTE CTableFrameSink::GetStatusLeftTime()
{
	CTimeSpan span = CTime::GetCurrentTime() - m_ctStatusStartTime;
	BYTE cbLeftTime = m_cbStatusConfigTime - span.GetSeconds();
	cbLeftTime > m_cbStatusConfigTime ? cbLeftTime = 0 : 0;
	return cbLeftTime;
};
//////////////////////////////////////////////////////////////////////////////////
