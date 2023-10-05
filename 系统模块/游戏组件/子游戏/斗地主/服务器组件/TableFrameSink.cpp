#include "StdAfx.h"
#include "TableFrameSink.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////////////
#define IDI_CHECK_TABLE				1					//�������
#define IDI_USER_OUT_TIME			6					//��ҳ�ʱ
#define IDI_GOOD_CARD_TIME			2					//������

HINSTANCE				CTableFrameSink::m_hDebugInst = NULL;
IServerDebug*			CTableFrameSink::m_pServerDebug = NULL;

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//�������
	m_pITableFrame=NULL;
	m_pGameCustomRule=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;
	m_bOffLineTrustee = false;

	m_cbTrusteeDelayTime = 5;

	//ը������
	m_wFirstUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));
	ZeroMemory(m_cbUserTrustee,sizeof(m_cbUserTrustee));
	m_lScoreTimes = 0L;

	//��Ϸ����
	m_wTimerDebug=0;
	m_cbBombCount=0;
	ZeroMemory(m_cbEachBombCount,sizeof(m_cbEachBombCount));

	//�з���Ϣ
	m_cbBankerScore=0;
	ZeroMemory(m_cbScoreInfo,sizeof(m_cbScoreInfo));
	m_cbCallScoreCount=0;

	//������Ϣ
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//�˿���Ϣ
	ZeroMemory(m_cbBankerCard,sizeof(m_cbBankerCard));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//�������
	if (m_hDebugInst == NULL)
	{
		m_hDebugInst = LoadLibrary(TEXT("LandServerDebug.dll"));
		if (m_hDebugInst)
		{
			typedef void * (*CREATE)();
			CREATE ServerDebug = (CREATE)GetProcAddress(m_hDebugInst, "CreateServerDebug");
			if (ServerDebug)
			{
				m_pServerDebug = static_cast<IServerDebug*>(ServerDebug());
			}
		}
	}
	//��Ϸ��Ƶ
	m_hVideoInst = NULL;
	m_pGameVideo = NULL;
	m_hVideoInst = LoadLibrary(TEXT("LandGameVideo.dll"));
	if ( m_hVideoInst )
	{
		typedef void * (*CREATE)(); 
		CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst,"CreateGameVideo"); 
		if ( GameVideo )
		{
			m_pGameVideo = static_cast<IGameVideo*>(GameVideo());
		}
	}	
	
	ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		m_listWinScoreRecord[i].RemoveAll();
	}

	ZeroMemory(m_userachievement, sizeof(m_userachievement));
	InitializeListRemainCard();

	return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
	if( m_pServerDebug != NULL)
	{
		delete m_pServerDebug;
		m_pServerDebug = NULL;
	}

	if( m_hDebugInst )
	{
		FreeLibrary(m_hDebugInst);
		m_hDebugInst = NULL;
	}
	
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

//�ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	//��Ϸ����
	m_cbBombCount=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));
	ZeroMemory(m_cbEachBombCount,sizeof(m_cbEachBombCount));
	ZeroMemory(m_cbUserTrustee,sizeof(m_cbUserTrustee));
	m_lScoreTimes = 0L;

	//�з���Ϣ
	m_cbBankerScore=0;
	ZeroMemory(m_cbScoreInfo,sizeof(m_cbScoreInfo));
	m_cbCallScoreCount=0;

	//������Ϣ
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//�˿���Ϣ
	ZeroMemory(m_cbBankerCard,sizeof(m_cbBankerCard));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	return;
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

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//��Ϸ����
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//�Զ�����
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	m_bOffLineTrustee = CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule);
	m_cbTrusteeDelayTime = m_pGameCustomRule->cbTrusteeDelayTime;
		
	//�������Ƹ��¶�ʱ��
	if (m_pServerDebug != NULL && m_pITableFrame->GetTableID() == 0)
	{
		m_pITableFrame->SetGameTimer(IDI_GOOD_CARD_TIME, 1000, -1, NULL);
	}

	return true;
}

//��������
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	return 0L;
}

//���ٻ���
SCORE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return 0L;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	////////////////////////////////////////////////////////////
	
	InitializeListRemainCard();

	//���ַ������˳ɾ�
	if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
	{
		//�µ�˽�˷����ֱ�־
		bool bNewAround = false;
		for (WORD i = 0; i<GAME_PLAYER; i++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (!pIServerUserItem)
			{
				continue;
			}

			if (m_userachievement[i].dwGameID != 0 && m_userachievement[i].dwGameID != pIServerUserItem->GetGameID())
			{
				bNewAround = true;
				break;
			}
		}

		//�¿�������
		if (bNewAround)
		{
			ZeroMemory(m_userachievement, sizeof(m_userachievement));
		}

		for (WORD i = 0; i<GAME_PLAYER; i++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (!pIServerUserItem)
			{
				continue;
			}

			tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
			m_userachievement[i].wFaceID = pUserInfo->wFaceID;
			m_userachievement[i].dwCustomID = pUserInfo->dwCustomID;
			m_userachievement[i].dwGameID = pUserInfo->dwGameID;
			CopyMemory(m_userachievement[i].szNickName, pUserInfo->szNickName, sizeof(m_userachievement[i].szNickName));
		}
	}

	//������Ϣ
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//����״̬
	m_pITableFrame->SetGameStatus(GAME_SCENE_CALL);
	m_ctChangeTime = CTime::GetCurrentTime();

	//�����˿�
	BYTE cbRandCard[FULL_COUNT];
	m_GameLogic.RandCardList(cbRandCard,CountArray(cbRandCard));

// 
 #ifdef _DEBUG

	//0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D, ����
	//0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D, ÷��
	//0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D, ����
	//0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D, ����

 	//BYTE cbTempRandCard[FULL_COUNT]=
 	//{
 	//	0x11,0x11,0x1D,0x0C,0x0B,0x0A,0x0A,0x09,0x07,0x07,0x07,0x07,0x06,0x06,0x06,0x05,0x05,
		//0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
		//0x29,0x2A,0x2B,0x2C,0x2D,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,
		//0x05,0x05,0x04,
 	//};
 
 	//CopyMemory(cbRandCard, cbTempRandCard, sizeof(cbRandCard));
 #endif
	
	//�������������һ������з�
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0 )
	{
		m_wFirstUser = INVALID_CHAIR;
	}

	//��ȡ����
	BYTE cbValidCardData=0;
	BYTE cbValidCardIndex=0;
	WORD wStartUser=m_wFirstUser;
	WORD wCurrentUser=m_wFirstUser;

	//��ȡ���
	if (wStartUser==INVALID_CHAIR)
	{
		//��ȡ�˿�
		cbValidCardIndex=rand()%DISPATCH_COUNT;
		cbValidCardData=cbRandCard[cbValidCardIndex];

		//�����û�
		wStartUser=m_GameLogic.GetCardValue(cbValidCardData)%GAME_PLAYER;
		wCurrentUser=(wStartUser+cbValidCardIndex/NORMAL_COUNT)%GAME_PLAYER;
	}

	
	//�û��˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wUserIndex=(wStartUser+i)%GAME_PLAYER;
		m_cbHandCardCount[wUserIndex]=NORMAL_COUNT;
		CopyMemory(&m_cbHandCardData[wUserIndex],&cbRandCard[i*m_cbHandCardCount[wUserIndex]],sizeof(BYTE)*m_cbHandCardCount[wUserIndex]);
	}

	//���õ���
	CopyMemory(m_cbBankerCard,&cbRandCard[DISPATCH_COUNT],sizeof(m_cbBankerCard));

	//���Ժ���
	if (m_pServerDebug != NULL)
	{
		bool bControlGoodCard = m_pServerDebug->GetDebugInfo(m_pITableFrame, m_cbBankerCard, m_cbHandCardCount, m_cbHandCardData);
	}

	//AI����
	CMD_S_AndroidCard AndroidCard ;
	ZeroMemory(&AndroidCard, sizeof(AndroidCard)) ;

	//�û��˿�
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		CopyMemory(AndroidCard.cbHandCard[i], m_cbHandCardData[i], sizeof(BYTE)*m_cbHandCardCount[i]);
	}

	//�����û�
	m_wFirstUser=wCurrentUser;
	m_wCurrentUser=wCurrentUser;

	//�������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));
	GameStart.lCellScore = m_pITableFrame->GetCellScore();
	GameStart.wStartUser=wStartUser;
	GameStart.wCurrentUser=wCurrentUser;
	GameStart.cbValidCardData=cbValidCardData;
	GameStart.cbValidCardIndex=cbValidCardIndex;

	AndroidCard.wCurrentUser = m_wCurrentUser;
	CopyMemory(AndroidCard.cbBankerCard,m_cbBankerCard,3*sizeof(BYTE));

	if(m_pGameVideo)
	{
		m_pGameVideo->StartVideo(m_pITableFrame);
	}
	//��������
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//�����˿�
		ASSERT(CountArray(GameStart.cbCardData)>=m_cbHandCardCount[i]);
		CopyMemory(GameStart.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);

		//��������
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i) ;
		if(!pServerUserItem->IsAndroidUser())
		{
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		else 
		{
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&AndroidCard,sizeof(AndroidCard));
		}
	
		CString strConclude;
		CTime time = CTime::GetCurrentTime();
		CString strSingleUser;
		if (i == 2)
			strSingleUser.Format(TEXT("��Ϸ��ʼ������ID=%d,����ID=%d,��ҡ�%s��,AI��־��%d \n"), m_pITableFrame->GetTableID(), i, pServerUserItem->GetNickName(), pServerUserItem->IsAndroidUser());
		else
			strSingleUser.Format(TEXT("��Ϸ��ʼ������ID=%d,����ID=%d,��ҡ�%s��,AI��־��%d"), m_pITableFrame->GetTableID(), i,pServerUserItem->GetNickName(), pServerUserItem->IsAndroidUser());
		strConclude += strSingleUser;
		CString strdebugB;
        strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
        m_pITableFrame->SendGameLogData(strdebugB);

		if (m_pGameVideo)
		{
			Video_GameStart video;
			lstrcpyn(video.szNickName,pServerUserItem->GetNickName(),CountArray(video.szNickName));
			video.wTableID			= i;
			video.wStartUser		= wStartUser;
			video.wCurrentUser		= wCurrentUser;
			video.cbValidCardData	= cbValidCardData;
			video.cbValidCardIndex	= cbValidCardIndex;
			CopyMemory(video.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
			video.lScore			= pServerUserItem->GetUserScore();
			video.lCellScore		= m_pITableFrame->GetCellScore();;
			m_pGameVideo->AddVideoData(SUB_S_GAME_START,&video,i==0?true:false);
		}
	}

	//WB��
	SendDebugCard();

	//�����˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i],ST_ORDER);
	}

	//���������й�
	IsOfflineTrustee();
	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("��Ϸ����������ID=%d,����ID=%d,��ҡ�%s��,AI��־��%d "), m_pITableFrame->GetTableID(), wChairID, pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	else 
		strSingleUser.Format(TEXT("��Ϸ����������ID=%d,����ID=%d,cbReason��%d "), m_pITableFrame->GetTableID(), wChairID, cbReason);
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{
			//ɾ����ʱ��ʱ��
			m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				IServerUserItem * pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (!pIKeyServerUserItem)
				{
					continue;
				}

				//�Ǳ����������Ϸ����ȡ���й�
				if (m_pGameServiceOption->wServerType != GAME_GENRE_MATCH)
				{
					pIKeyServerUserItem->SetTrusteeUser(false);
				}
				
			}

			//��������
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude,sizeof(GameConclude));

			//���ñ���
			GameConclude.cbBankerScore=m_cbBankerScore;
			GameConclude.lCellScore=m_pITableFrame->GetCellScore();

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(GameConclude.remainCardCount), &remaincard, sizeof(remaincard));

			//ը����Ϣ
			GameConclude.cbBombCount=m_cbBombCount;
			CopyMemory(GameConclude.cbEachBombCount,m_cbEachBombCount,sizeof(GameConclude.cbEachBombCount));

			//�û��˿�
			BYTE cbCardIndex=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//�����˿�
				GameConclude.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));

				//��������
				cbCardIndex+=m_cbHandCardCount[i];
			}

			//ը��ͳ��
			LONG lScoreTimes=1L * m_cbBankerScore;
			for (BYTE i=0;i<m_cbBombCount;i++) lScoreTimes*=2L;

			//�����ж�
			if (wChairID==m_wBankerUser)
			{
				//�û�����
				WORD wUser1=(m_wBankerUser+1)%GAME_PLAYER;
				WORD wUser2=(m_wBankerUser+2)%GAME_PLAYER;

				//�û��ж�
				if ((m_cbOutCardCount[wUser1]==0)&&(m_cbOutCardCount[wUser2]==0)) 
				{
					lScoreTimes*=2L;
					GameConclude.bChunTian=TRUE;

					//���ַ������˳ɾ�
					if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
					  && m_userachievement[wChairID].dwGameID == m_pITableFrame->GetTableUserItem(wChairID)->GetGameID())
					{
						m_userachievement[wChairID].cbChunTianCount++;
					}
				}
			}

			//�����ж�
			if (wChairID!=m_wBankerUser)
			{
				if (m_cbOutCardCount[m_wBankerUser]==1)
				{
					lScoreTimes*=2L;
					GameConclude.bFanChunTian=TRUE;

					//���ַ������˳ɾ�
					if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
						&& m_userachievement[m_wBankerUser].dwGameID == m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetGameID())
					{
						m_userachievement[m_wBankerUser].cbFanChunTianCount++;
					}
				}
			}

			//��������
			lScoreTimes=__min(m_pGameCustomRule->wMaxScoreTimes,lScoreTimes);

			m_lScoreTimes = lScoreTimes;

			//ũ���й�����
			WORD wTrusteeCount=0;
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
			{
				for(WORD i = 0; i < GAME_PLAYER; i++)
				{
					if (i!=m_wBankerUser)
					{
						if (m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser()==true) wTrusteeCount++;
					}
				}
			}

			//CString strCellScore;
			//strCellScore.Format(TEXT("GameConcludeCellScore = %d"), m_pITableFrame->GetCellScore());
			//CTraceService::TraceString(strCellScore, TraceLevel_Exception);

			//���ֱ���
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			//ũ��÷�
			LONGLONG lPlayerWinScore = 0;

			//ͳ�ƻ���
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//��������
				SCORE lUserScore=0L;
				SCORE lCellScore=m_pITableFrame->GetCellScore();
				bool bTrusteeUser=m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser();
				
				//���ֻ���
				if (i==m_wBankerUser)
				{
					lUserScore=(m_cbHandCardCount[m_wBankerUser]==0)?2L:-2L;
				}
				else
				{
					if (m_cbHandCardCount[m_wBankerUser]==0)
					{
						if (wTrusteeCount>0) lUserScore=(bTrusteeUser==true)?(-2L/wTrusteeCount):0L;
						else lUserScore=-1L;
					}
					else
					{
						if (wTrusteeCount>0) lUserScore=(bTrusteeUser==true)?0L:(2L/wTrusteeCount);
						else lUserScore=1L;
					}
				}

				//���û���
				GameConclude.lGameScore[i] = lUserScore*lCellScore*lScoreTimes;

				if (i != m_wBankerUser && lPlayerWinScore == 0)
				{
					lPlayerWinScore = GameConclude.lGameScore[i];
				}
			}

			//�����÷�
			//����Ӯ
			if (m_cbHandCardCount[m_wBankerUser] == 0)
			{
				LONGLONG lBankerCarryScore = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore();

				//У��ũ���ܷ��⸶
				LONGLONG lBankerWinScore = 0;
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					//
					if (i == m_wBankerUser)
					{
						continue;
					}

					IServerUserItem *pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (!pIKeyServerUserItem)
					{
						continue;
					}

					ASSERT(GameConclude.lGameScore[i] < 0);
					if (-GameConclude.lGameScore[i] > pIKeyServerUserItem->GetUserScore())
					{
						GameConclude.lGameScore[i] = -pIKeyServerUserItem->GetUserScore();
					}

					GameConclude.lGameScore[i] = -(min(-GameConclude.lGameScore[i], lBankerCarryScore / 2));

					lBankerWinScore += (-GameConclude.lGameScore[i]);
				}

				GameConclude.lGameScore[m_wBankerUser] = lBankerWinScore;
			}
			//ũ��Ӯ
			else
			{
				ASSERT(lPlayerWinScore > 0);

				//�жϵ���Я������Ƿ���
				if (m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore() >= 2 * lPlayerWinScore)
				{
					//У��ũ��Ӯ��Ǯ�Ƿ񳬹�����Я��
					LONGLONG lBankerWinScore = 0;
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{
						//
						if (i == m_wBankerUser)
						{
							continue;
						}

						IServerUserItem *pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (!pIKeyServerUserItem)
						{
							continue;
						}

						ASSERT(GameConclude.lGameScore[i] > 0);
						if (GameConclude.lGameScore[i] > pIKeyServerUserItem->GetUserScore())
						{
							GameConclude.lGameScore[i] = pIKeyServerUserItem->GetUserScore();
						}

						lBankerWinScore += (-GameConclude.lGameScore[i]);
					}

					GameConclude.lGameScore[m_wBankerUser] = lBankerWinScore;
				}
				//����������
				else
				{
					//Я����ƽ����
					LONGLONG lBankerCarryScore = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore();
					LONGLONG lBankerWinScore = 0;
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{
						//
						if (i == m_wBankerUser)
						{
							continue;
						}

						IServerUserItem *pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (!pIKeyServerUserItem)
						{
							continue;
						}

						ASSERT(GameConclude.lGameScore[i] > 0);
						if (lBankerCarryScore / 2 > pIKeyServerUserItem->GetUserScore())
						{
							GameConclude.lGameScore[i] = pIKeyServerUserItem->GetUserScore();
						}
						else
						{
							GameConclude.lGameScore[i] = lBankerCarryScore / 2;
						}

						lBankerWinScore += (-GameConclude.lGameScore[i]);
					}

					GameConclude.lGameScore[m_wBankerUser] = lBankerWinScore;
				}
			}

			//����˰��
			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				ScoreInfoArray[i].lScore=GameConclude.lGameScore[i];
				ScoreInfoArray[i].cbType=(ScoreInfoArray[i].lScore>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

				if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
					((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
				{
					//����˰��
					ScoreInfoArray[i].lRevenue = m_pITableFrame->CalculateRevenue(i, ScoreInfoArray[i].lScore);
					if (ScoreInfoArray[i].lRevenue>0L) ScoreInfoArray[i].lScore -= ScoreInfoArray[i].lRevenue;
				}

				//���û���
				GameConclude.lGameScore[i]=ScoreInfoArray[i].lScore;

				//��ʷ����
				m_HistoryScore.OnEventUserScore(i,GameConclude.lGameScore[i]);

				//����ģʽ
				//���ַ���
				if ((m_pITableFrame->GetDataBaseMode() == 0) &&((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && !m_pITableFrame->IsPersonalRoomDisumme())
				{
					if (m_RoomCardRecord.nCount < MAX_RECORD_COUNT)
					{
						m_RoomCardRecord.lDetailScore[i][m_RoomCardRecord.nCount] = ScoreInfoArray[i].lScore;
					}
				}
				//��ҷ���
				else if ((m_pITableFrame->GetDataBaseMode() == 1) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
				{
					m_listWinScoreRecord[i].AddHead(ScoreInfoArray[i].lScore);
				}
			}

			//����ģʽ
			if ((m_pITableFrame->GetDataBaseMode() == 0) && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				m_RoomCardRecord.nCount++;
			}

			//���ַ������˳ɾ�
			if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				m_pITableFrame->GetSpeicalInfo((byte*)(&m_userachievement[0]), sizeof(m_userachievement));
			}

			GameConclude.lScoreTimes = m_lScoreTimes;

			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));

			if(m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_CONCLUDE,&GameConclude);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
			}
			//д�����
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//�л��û�
			m_wFirstUser=wChairID;

			//���µ��Ե÷�
			if (m_pServerDebug != NULL)
			{
				m_pServerDebug->UpdateDebugInfo(m_pITableFrame, ScoreInfoArray);
			}

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
				
			////////////////////////////////////////////////////////////
			CString strConclude;
			CTime time = CTime::GetCurrentTime();
			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				IServerUserItem *pIConcludeUserItem = m_pITableFrame->GetTableUserItem(i);
				if (pIConcludeUserItem)
				{
					CString strSingleUser;
					strSingleUser.Format(TEXT("��Ϸ����������ID=%d,�ǳơ�%s��GAMEID = %d �÷� = %d \n"), m_pITableFrame->GetTableID(), pIConcludeUserItem->GetNickName(), pIConcludeUserItem->GetGameID(), GameConclude.lGameScore[i]);
					strConclude += strSingleUser;
				}
				else
				{
					CString strSingleUser;
					strSingleUser.Format(TEXT("��Ϸ����������ID=%d,chairID = %d �÷� = %d \n"), m_pITableFrame->GetTableID(), i, GameConclude.lGameScore[i]);
					strConclude += strSingleUser;
				}
			}

			CString strdebugB;
            strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s, ���ÿ��� ʣ��������� [%d][%d][%d]\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude, m_cbHandCardCount[0], m_cbHandCardCount[1], m_cbHandCardCount[2]);
            m_pITableFrame->SendGameLogData(strdebugB);
			////////////////////////////////////////////////////////////

			//����ģʽ
			if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_pITableFrame->IsPersonalRoomDisumme())
			{
				//Լս��ɢ
				ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
				ZeroMemory(m_userachievement, sizeof(m_userachievement));
			}

			return true;
		}
	case GER_DISMISS:		//��Ϸ��ɢ
		{
			//ɾ����ʱ��ʱ��
			m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				IServerUserItem * pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (!pIKeyServerUserItem)
				{
					continue;
				}

				//�Ǳ����������Ϸ����ȡ���й�
				if (m_pGameServiceOption->wServerType != GAME_GENRE_MATCH)
				{
					pIKeyServerUserItem->SetTrusteeUser(false);
				}
			}

			//��������
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude,sizeof(GameConclude));

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(GameConclude.remainCardCount), &remaincard, sizeof(remaincard));

			//�û��˿�
			BYTE cbCardIndex=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//�����˿�
				GameConclude.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));

				//��������
				cbCardIndex+=m_cbHandCardCount[i];
			}

			//���ַ������˳ɾ�
			if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				m_pITableFrame->GetSpeicalInfo((byte*)(&m_userachievement[0]), sizeof(m_userachievement));
			}

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			
			//���µ��Ե÷�
			if (m_pServerDebug != NULL)
			{
				//���ֱ���
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
				m_pServerDebug->UpdateDebugInfo(m_pITableFrame, ScoreInfoArray);
			}

			/*if(m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_CONCLUDE,&GameConclude);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
			}*/
			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			return true;
		}
	case GER_USER_LEAVE:	//�û�ǿ��
	case GER_NETWORK_ERROR:	//�����ж�
		{
			//ɾ����ʱ��ʱ��
			m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

			//��������
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude,sizeof(GameConclude));

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(GameConclude.remainCardCount), &remaincard, sizeof(remaincard));

			//���ñ���
			GameConclude.cbBankerScore=m_cbBankerScore;
			GameConclude.lCellScore=m_pITableFrame->GetCellScore();

			//ը����Ϣ
			GameConclude.cbBombCount=m_cbBombCount;
			CopyMemory(GameConclude.cbEachBombCount,m_cbEachBombCount,sizeof(GameConclude.cbEachBombCount));

			//�û��˿�
			BYTE cbCardIndex=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//�����˿�
				GameConclude.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));

				//��������
				cbCardIndex+=m_cbHandCardCount[i];
			}

			//ը��ͳ��
			WORD lScoreTimes=1;
			for (WORD i=0;i<m_cbBombCount;i++) lScoreTimes*=2L;

			//��������
			lScoreTimes=__min(m_pGameCustomRule->wMaxScoreTimes,lScoreTimes);

			//���ֱ���
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			//��������
			SCORE lCellScore=m_pITableFrame->GetCellScore();
			SCORE lUserScore=lCellScore*__max(lScoreTimes,m_pGameCustomRule->wFleeScoreTimes);

			//���ƽ��
			if ((m_pGameServiceOption->wServerType&SCORE_GENRE_POSITIVE)!=0 || (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_EDUCATE))!=0)
			{
				lUserScore=__min(pIServerUserItem->GetUserScore(),lUserScore);
			}

			//���ܷ���
			ScoreInfoArray[wChairID].lScore=-lUserScore;
			ScoreInfoArray[wChairID].cbType=SCORE_TYPE_FLEE;

			//������
			if (m_pGameCustomRule->cbFleeScorePatch==TRUE)
			{
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					//����
					if ( i == wChairID )
						continue;

					//���û���
					ScoreInfoArray[i].lScore=lUserScore/2L;
					ScoreInfoArray[i].cbType=SCORE_TYPE_WIN;

					if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
						((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
					{
						//����˰��
						ScoreInfoArray[i].lRevenue = m_pITableFrame->CalculateRevenue(i, ScoreInfoArray[i].lScore);
						if (ScoreInfoArray[i].lRevenue>0L) ScoreInfoArray[i].lScore -= ScoreInfoArray[i].lRevenue;
					}
				}
			}

			//��ʷ����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//���óɼ�
				GameConclude.lGameScore[i]=ScoreInfoArray[i].lScore;

				//��ʷ�ɼ�
				m_HistoryScore.OnEventUserScore(i,GameConclude.lGameScore[i]);
			}

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			/*
			if(m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_CONCLUDE,&GameConclude);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
			}*/
			//д�����
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//���µ��Ե÷�
			if (m_pServerDebug != NULL)
			{
				m_pServerDebug->UpdateDebugInfo(m_pITableFrame, ScoreInfoArray);
			}

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
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("���ͳ���������ID=%d,����ID=%d,��ҡ�%s��,AI��־��%d����Ϸ״̬��%d"), m_pITableFrame->GetTableID(), wChairID, pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser(), cbGameStatus);
	else 
		strSingleUser.Format(TEXT("���ͳ���������ID=%d,����ID=%d,��Ϸ״̬��%d"), m_pITableFrame->GetTableID(), wChairID,cbGameStatus);
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	//CString strCellScore;
	//strCellScore.Format(TEXT("GameSceneCellScore = %d"), m_pITableFrame->GetCellScore());
	//CTraceService::TraceString(strCellScore, TraceLevel_Exception);

	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//���ñ���
			StatusFree.lCellScore=m_pITableFrame->GetCellScore();

			//�Զ�����
			StatusFree.cbTimeOutCard=m_pGameCustomRule->cbTimeOutCard;		
			StatusFree.cbTimeCallScore=m_pGameCustomRule->cbTimeCallScore;
			StatusFree.cbTimeStartGame=m_pGameCustomRule->cbTimeStartGame;
			StatusFree.cbTimeHeadOutCard=m_pGameCustomRule->cbTimeHeadOutCard;

			InitializeListRemainCard();
			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(StatusFree.remainCardCount), &remaincard, sizeof(remaincard));

			//��ʷ����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//��������
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//���ñ���
				StatusFree.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//0 ���ֳ�	1 ��ҳ�
			StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SCENE_CALL:	//�з�״̬
		{
			//��������
			CMD_S_StatusCall StatusCall;
			ZeroMemory(&StatusCall,sizeof(StatusCall));

			//��Ԫ����
			StatusCall.lCellScore=m_pITableFrame->GetCellScore();

			//�Զ�����
			StatusCall.cbTimeOutCard=m_pGameCustomRule->cbTimeOutCard;		
			StatusCall.cbTimeCallScore=m_pGameCustomRule->cbTimeCallScore;
			StatusCall.cbTimeStartGame=m_pGameCustomRule->cbTimeStartGame;
			StatusCall.cbTimeHeadOutCard=m_pGameCustomRule->cbTimeHeadOutCard;

			//��Ϸ��Ϣ
			StatusCall.wCurrentUser=m_wCurrentUser;
			StatusCall.cbBankerScore=m_cbBankerScore;
			CopyMemory(StatusCall.cbScoreInfo,m_cbScoreInfo,sizeof(m_cbScoreInfo));
			CopyMemory(StatusCall.cbHandCardData,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID]*sizeof(BYTE));
			CopyMemory(StatusCall.cbUserTrustee,m_cbUserTrustee,sizeof(m_cbUserTrustee));

			StatusCall.lScoreTimes = m_lScoreTimes;

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(StatusCall.remainCardCount), &remaincard, sizeof(remaincard));

			//��ʷ����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//��������
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//���ñ���
				StatusCall.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusCall.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//0 ���ֳ�	1 ��ҳ�
			StatusCall.cbPlayMode = m_pITableFrame->GetDataBaseMode();

			CTime ctCurTime = CTime::GetCurrentTime();
			CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;
			StatusCall.cbTimeRemain = StatusCall.cbTimeCallScore - TimeSpan.GetSeconds();

			//���ͳ���
			bool bSendResult = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusCall,sizeof(StatusCall));

			if(pIServerUserItem)
			{
				//�����û�
				if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
				{
					OnDebugCard(pIServerUserItem);
				}
			}

			if (m_bOffLineTrustee && wChairID == m_wCurrentUser)
			{
				pIServerUserItem->SetTrusteeUser(false);
				IsOfflineTrustee();
			}
			else if (m_bOffLineTrustee && wChairID != m_wCurrentUser)
			{
				pIServerUserItem->SetTrusteeUser(false);
			}

			return bSendResult;
		}
	case GAME_SCENE_PLAY:	//��Ϸ״̬
		{
			//��������
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//��Ԫ����
			StatusPlay.lCellScore=m_pITableFrame->GetCellScore();

			//��Ϸ����
			StatusPlay.cbBombCount=m_cbBombCount;
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.cbBankerScore=m_cbBankerScore;

			//�Զ�����
			StatusPlay.cbTimeOutCard=m_pGameCustomRule->cbTimeOutCard;		
			StatusPlay.cbTimeCallScore=m_pGameCustomRule->cbTimeCallScore;
			StatusPlay.cbTimeStartGame=m_pGameCustomRule->cbTimeStartGame;
			StatusPlay.cbTimeHeadOutCard=m_pGameCustomRule->cbTimeHeadOutCard;

			//������Ϣ
			StatusPlay.wTurnWiner=m_wTurnWiner;
			StatusPlay.cbTurnCardCount=m_cbTurnCardCount;
			CopyMemory(StatusPlay.cbTurnCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

			//�˿���Ϣ
			CopyMemory(StatusPlay.cbBankerCard,m_cbBankerCard,sizeof(m_cbBankerCard));
			CopyMemory(StatusPlay.cbHandCardCount,m_cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(StatusPlay.cbHandCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*m_cbHandCardCount[wChairID]);
			CopyMemory(StatusPlay.cbUserTrustee,m_cbUserTrustee,sizeof(m_cbUserTrustee));
			StatusPlay.lScoreTimes = m_lScoreTimes;

			REMAINCARDCOUNT remaincard;
			ZeroMemory(&remaincard, sizeof(remaincard));
			GetRemainCard(remaincard);

			CopyMemory(&(StatusPlay.remainCardCount), &remaincard, sizeof(remaincard));

			//��ʷ����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//��������
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//���ñ���
				StatusPlay.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//0 ���ֳ�	1 ��ҳ�
			StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();

			CTime ctCurTime = CTime::GetCurrentTime();
			CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;

			//�ж��Ƿ��׳�
			BYTE cbOutCardTime = (wChairID == m_wBankerUser && m_cbOutCardCount[wChairID] == 0) ? StatusPlay.cbTimeHeadOutCard : StatusPlay.cbTimeOutCard;
			StatusPlay.cbTimeRemain = cbOutCardTime - TimeSpan.GetSeconds();

			//���ͳ���
			bool bSendResult = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			if(pIServerUserItem)
			{
				//�����û�
				if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
				{
					OnDebugCard(pIServerUserItem);
				}
			}

			if (m_bOffLineTrustee && wChairID == m_wCurrentUser)
			{
				pIServerUserItem->SetTrusteeUser(false);
				IsOfflineTrustee();
			}
			else if (m_bOffLineTrustee && wChairID != m_wCurrentUser)
			{
				pIServerUserItem->SetTrusteeUser(false);
			}

			return bSendResult;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	//if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		if(wTimerID==IDI_USER_OUT_TIME)
		{
			OutputDebugString(TEXT("ptdt *** �����йܴ��� ����"));
			m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

			if (m_wCurrentUser==INVALID_CHAIR) return true;

			IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
			ASSERT(pServerUserItem!=NULL);
			if (pServerUserItem==NULL) return false;

			bool bTrusteeUser=pServerUserItem->IsTrusteeUser();
			BYTE cbGameStatus = m_pITableFrame->GetGameStatus();

			CString strConclude;
			CTime time = CTime::GetCurrentTime();
			CString strSingleUser;
			strSingleUser.Format(TEXT("�û���������ID=%d,����ID=%d,��ҡ�%s��,AI��־��%d,�й�״̬%d,��Ϸ����%d"), m_pITableFrame->GetTableID(), pServerUserItem->GetChairID(), pServerUserItem->GetNickName(), pServerUserItem->IsAndroidUser(), bTrusteeUser, cbGameStatus);
			strConclude += strSingleUser;
			CString strdebugB;
            strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
            m_pITableFrame->SendGameLogData(strdebugB);

			switch(cbGameStatus)
			{
			case GAME_SCENE_CALL:
				{
					WORD wTempCurrentUser = m_wCurrentUser;
					if (m_cbBankerScore == 0)
						OnUserCallScore(m_wCurrentUser, 0x01);
					else
						OnUserCallScore(m_wCurrentUser, 0xFF);

					//��ʱ�Զ��й�
					OnUserTrustee(wTempCurrentUser, true);
					break;
				}
			case GAME_SCENE_PLAY:
				{
					if(m_cbTurnCardCount == 0)
					{
						tagSearchCardResult SearchCardResult;
						m_GameLogic.SearchOutCard( m_cbHandCardData[m_wCurrentUser],m_cbHandCardCount[m_wCurrentUser],NULL,0,&SearchCardResult);
						if (SearchCardResult.cbSearchCount > 0)
						{
							WORD wTempCurrentUser = m_wCurrentUser;
							OnUserOutCard(m_wCurrentUser, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0]);

							//��ʱ�Զ��й�
							OnUserTrustee(wTempCurrentUser, true);
						}
						else
						{
							ASSERT(FALSE);
						}
					}
					else
					{
						tagSearchCardResult SearchCardResult;
						ZeroMemory(&SearchCardResult, sizeof(SearchCardResult));
						WORD wTempCurrentUser = m_wCurrentUser;

						m_GameLogic.SearchOutCard(m_cbHandCardData[m_wCurrentUser], m_cbHandCardCount[m_wCurrentUser], m_cbTurnCardData, m_cbTurnCardCount, &SearchCardResult);
						if (SearchCardResult.cbSearchCount > 0)
						{
							OnUserOutCard(m_wCurrentUser, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0]);

							//��ʱ�Զ��й�
							OnUserTrustee(wTempCurrentUser, true);
						}
						else
						{
							OnUserPassCard(m_wCurrentUser);

							//��ʱ�Զ��й�
							OnUserTrustee(wTempCurrentUser, true);
						}
					}					
					break;
				}
			}
			return true;
		}
	}	

	return false;
}

//�����¼�
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return false;
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("��Ϸ��Ϣ������ID=%d,����ID=%d,��ҡ�%s��,AI��־��%d����Ϸָ�%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser(), wSubCmdID);
	else 
		strSingleUser.Format(TEXT("��Ϸ��Ϣ������ID=%d,����ID=%d����Ϸָ�%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(),wSubCmdID);
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(),time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	m_cbWaitTime=0;
	switch (wSubCmdID)
	{
	case SUB_C_CALL_SCORE:	//�û��з�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_CallScore));
			if (wDataSize!=sizeof(CMD_C_CallScore)) return false;

			//״̬Ч��
			//ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_CALL);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_CALL) return true;

			//�û�Ч��
			//ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��������
			CMD_C_CallScore * pCallScore=(CMD_C_CallScore *)pData;

			//��Ϣ����
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserCallScore(wChairID,pCallScore->cbCallScore);
		}
	case SUB_C_OUT_CARD:	//�û�����
		{
			//��������
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//Ч������
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE))));
			if ((wDataSize < wHeadSize) || (wDataSize != (wHeadSize + pOutCard->cbCardCount*sizeof(BYTE))))
			{
				return false;
			}

			//״̬Ч��
			//ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

			//�û�Ч��
			//ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��Ϣ����
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserOutCard(wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
	case SUB_C_PASS_CARD:	//�û�����
		{
			//״̬Ч��
			//ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

			//�û�Ч��
			//ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��Ϣ����
			return OnUserPassCard(pIServerUserItem->GetChairID());
		}
	case SUB_C_TRUSTEE:
		{
			//�û�Ч��
// 			tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
// 			if (pUserData == NULL || pUserData->cbUserStatus != US_PLAYING)
// 			{
// 				return true;
// 			}
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_TRUSTEE));
			if (wDataSize!=sizeof(CMD_C_TRUSTEE)) return false;

			CMD_C_TRUSTEE* pCTrustee = (CMD_C_TRUSTEE*)pData;
			pIServerUserItem->SetTrusteeUser(pCTrustee->bTrustee == TRUE ? true : false);

			//�йܺ���������
			if (TRUE == pCTrustee->bTrustee && m_wCurrentUser != INVALID_CHAIR && pIServerUserItem->GetChairID() == m_wCurrentUser)
			{
				BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
				switch (cbGameStatus)
				{
					case GAME_SCENE_CALL:
					{
						if (m_cbBankerScore == 0)
							OnUserCallScore(m_wCurrentUser, 0x01);
						else
							OnUserCallScore(m_wCurrentUser, 0xFF);
						break;
					}
				case GAME_SCENE_PLAY:
					{
						if (m_cbTurnCardCount == 0)
						{
							tagSearchCardResult SearchCardResult;
							m_GameLogic.SearchOutCard(m_cbHandCardData[m_wCurrentUser], m_cbHandCardCount[m_wCurrentUser], NULL, 0, &SearchCardResult);
							if (SearchCardResult.cbSearchCount > 0)
							{
								OnUserOutCard(m_wCurrentUser, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0]);
							}
							else
							{
								ASSERT(FALSE);
							}
						}
						else
						{
							tagSearchCardResult SearchCardResult;
							ZeroMemory(&SearchCardResult, sizeof(SearchCardResult));

							m_GameLogic.SearchOutCard(m_cbHandCardData[m_wCurrentUser], m_cbHandCardCount[m_wCurrentUser], m_cbTurnCardData, m_cbTurnCardCount, &SearchCardResult);
							if (SearchCardResult.cbSearchCount > 0)
							{
								OnUserOutCard(m_wCurrentUser, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0]);
							}
							else
							{
								OnUserPassCard(m_wCurrentUser);
							}
						}
						break;
					}
				default:
					break;
				}
			}
			else if (FALSE == pCTrustee->bTrustee && m_wCurrentUser != INVALID_CHAIR && pIServerUserItem->GetChairID() == m_wCurrentUser)
			{
				//ɾ����ʱ��ʱ��
				m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

				BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
				switch (cbGameStatus)
				{
					case GAME_SCENE_CALL:
					{
						CTime ctCurTime = CTime::GetCurrentTime();
						CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;
						BYTE cbTimeRemain = m_pGameCustomRule->cbTimeCallScore - TimeSpan.GetSeconds();
						m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME, cbTimeRemain * 1000, -1, NULL);

						break;
					}
					case GAME_SCENE_PLAY:
					{
						CTime ctCurTime = CTime::GetCurrentTime();
						CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;

						//�ж��Ƿ��׳�
						BYTE cbOutCardTime = (pIServerUserItem->GetChairID() == m_wBankerUser && m_cbOutCardCount[pIServerUserItem->GetChairID()] == 0) ? m_pGameCustomRule->cbTimeHeadOutCard : m_pGameCustomRule->cbTimeOutCard;
						BYTE cbTimeRemain = cbOutCardTime - TimeSpan.GetSeconds();
						m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME, cbTimeRemain * 1000, -1, NULL);

						break;
					}
					default:
						break;
				}
			}

			CMD_S_TRUSTEE pSTrustee;
			pSTrustee.wTrusteeUser = pIServerUserItem->GetChairID();
			pSTrustee.bTrustee = pCTrustee->bTrustee;

			m_cbUserTrustee[pIServerUserItem->GetChairID()] = pCTrustee->bTrustee;

			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&pSTrustee,sizeof(CMD_S_TRUSTEE));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&pSTrustee,sizeof(CMD_S_TRUSTEE));
			return true;
		}
	case SUB_C_REQUEST_RCRecord:
		{
			ASSERT (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) !=0)/* && (m_pITableFrame->IsPersonalRoomDisumme())*/);
			if (!(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) !=0)/* && (m_pITableFrame->IsPersonalRoomDisumme())*/))
			{
				return false;
			}

			ASSERT (pIServerUserItem->IsMobileUser());
			if (!pIServerUserItem->IsMobileUser())
			{
				return false;
			}

			CMD_S_RoomCardRecord RoomCardRecord;
			ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

			//���ַ���
			if ((m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0))
			{
				CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
			}
			else
			{
				WORD wChairID = pIServerUserItem->GetChairID();
				POSITION pos = m_listWinScoreRecord[wChairID].GetHeadPosition();
				CString strUSER;
                strUSER.Format(TEXT("table id is %d, ��������ǳ� %s chairID %d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetNickName(), wChairID);
				CString strSingle;
				while (pos)
				{
					if (RoomCardRecord.nCount >= MAX_RECORD_COUNT)
					{
						break;
					}

					RoomCardRecord.lDetailScore[wChairID][RoomCardRecord.nCount++] = m_listWinScoreRecord[wChairID].GetNext(pos);
					strSingle.Format(TEXT("����%d,�÷� %I64d\n"), RoomCardRecord.nCount, RoomCardRecord.lDetailScore[wChairID][RoomCardRecord.nCount - 1]);
					strUSER += strSingle;
				}
                m_pITableFrame->SendGameLogData(strUSER);
			}
			
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));

			return true;
		}
	}

	return false;
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (m_pServerDebug != NULL)
	{
		// ��Ϣ����
		if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
		{
			// �ж���Ϣ
			return m_pServerDebug->ServerDebug(wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN, pData, wDataSize, pIServerUserItem, m_pITableFrame);
		}
	}

	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("�û����ߣ�����ID=%d,����ID=%d,��ҡ�%s��,AI��־��%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	else
		strSingleUser.Format(TEXT("�û����ߣ�����ID=%d,����ID=%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID());
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(),time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	if(m_bOffLineTrustee)
	{
		//���������й�
		pIServerUserItem->SetTrusteeUser(true);

 		//if (wChairID == m_wCurrentUser)
 		//{
 		//	IsOfflineTrustee(1);
 		//}
	}
	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("�û����룬����ID=%d,����ID=%d,��ҡ�%s��,AI��־��%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	else
		strSingleUser.Format(TEXT("�û����룬����ID=%d,����ID=%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID());
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(),time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	OutputDebugString(TEXT("ptdt *** �û�����"));
	if(m_bOffLineTrustee)
	{
		//if (((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)&&(pIServerUserItem->GetChairID()==m_wCurrentUser))
		{
			//OutputDebugString(TEXT("ptdt *** �û����� ɱ���Զ����ƶ�ʱ��"));
			//m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);
		}
	}
	return true; 
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("�û����£�����ID=%d,����ID=%d,��ҡ�%s��,AI��־��%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	else
		strSingleUser.Format(TEXT("�û����£�����ID=%d,����ID=%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID());
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID() ,time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	//��ʷ����
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserEnter(wChairID);
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	CString strConclude;
	CTime time = CTime::GetCurrentTime();
	CString strSingleUser;
	if (pIServerUserItem)
		strSingleUser.Format(TEXT("�û�����������ID=%d,����ID=%d,��ҡ�%s��,AI��־��%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName(), pIServerUserItem->IsAndroidUser());
	else
		strSingleUser.Format(TEXT("�û�����������ID=%d,����ID=%d"), m_pITableFrame->GetTableID(), pIServerUserItem->GetChairID());
	strConclude += strSingleUser;
	CString strdebugB;
    strdebugB.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d %s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), strConclude);
    m_pITableFrame->SendGameLogData(strdebugB);

	//��ʷ����
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserLeave(wChairID);
	}
	m_cbUserTrustee[wChairID] = 0;

	//��ҷ���
	if ((m_pITableFrame->GetDataBaseMode() == 1) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
	{
		m_listWinScoreRecord[wChairID].RemoveAll();
	}
	//���ַ���
	else if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_pITableFrame->IsPersonalRoomDisumme())
	{
		//Լս��ɢ
		ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
		ZeroMemory(m_userachievement, sizeof(m_userachievement));
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//Ч��״̬
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbTurnCardCount!=0));
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return true;

	//���ñ���
	m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
	if (m_wCurrentUser == m_wTurnWiner)
	{
		m_cbTurnCardCount = 0;
		ZeroMemory(m_cbTurnCardData, sizeof(m_cbTurnCardData));
	}

	m_ctChangeTime = CTime::GetCurrentTime();

	//������Ϣ
	CMD_S_PassCard PassCard;
	PassCard.wPassCardUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.cbTurnOver=(m_cbTurnCardCount==0)?TRUE:FALSE;

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	if (m_pGameVideo)
	{		
		m_pGameVideo->AddVideoData(SUB_S_PASS_CARD,&PassCard);
	}

	////////////////////////////////////////////////////////////
	IServerUserItem *pIOutCardServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	CString strdebug;
	CTime time = CTime::GetCurrentTime();

	if (pIOutCardServerUserItem)
	{
        strdebug.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d �����ǳơ�%s��GAMEID = %d, chairID = %d , ���� ʣ��������� [%d] android = %d\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIOutCardServerUserItem->GetNickName(), pIOutCardServerUserItem->GetGameID(), wChairID, m_cbHandCardCount[wChairID], pIOutCardServerUserItem->IsAndroidUser());
        m_pITableFrame->SendGameLogData(strdebug);
	}
	////////////////////////////////////////////////////////////

	//if(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
		IsOfflineTrustee();

	return true;
}

//�û��з�
bool CTableFrameSink::OnUserCallScore(WORD wChairID, BYTE cbCallScore)
{
	//Ч��״̬
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return true;

	//Ч�����
	//ASSERT(((cbCallScore>=1)&&(cbCallScore<=3)&&(cbCallScore>m_cbBankerScore))||(cbCallScore==255));
	if (((cbCallScore<1)||(cbCallScore>3)||(cbCallScore<=m_cbBankerScore))&&(cbCallScore!=255))
		cbCallScore = 255;

	//����״̬
	if (cbCallScore!=0xFF)
	{
		m_cbBankerScore=cbCallScore;
		m_wBankerUser=m_wCurrentUser;
	}

	//���ýз�
	m_cbScoreInfo[wChairID]=cbCallScore;

	//�����û�
	if ((m_cbBankerScore==3)||(m_wFirstUser==(wChairID+1)%GAME_PLAYER))
	{
		m_wCurrentUser=INVALID_CHAIR;
	}
	else
	{
		m_wCurrentUser=(wChairID+1)%GAME_PLAYER;
	}

	m_ctChangeTime = CTime::GetCurrentTime();

	//�������
	CMD_S_CallScore CallScore;
	CallScore.wCallScoreUser=wChairID;
	CallScore.wCurrentUser=m_wCurrentUser;
	CallScore.cbUserCallScore=cbCallScore;
	CallScore.cbCurrentScore=m_cbBankerScore;

	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CALL_SCORE,&CallScore,sizeof(CallScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_SCORE,&CallScore,sizeof(CallScore));

	if (m_pGameVideo)
	{		
		m_pGameVideo->AddVideoData(SUB_S_CALL_SCORE,&CallScore);
	}

	//��ʼ�ж�
	if ((m_cbBankerScore==3)||(m_wFirstUser==(wChairID+1)%GAME_PLAYER))
	{
		//���˽з�
		if ( m_cbBankerScore == 0 )
		{
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				if(m_cbCallScoreCount<2)
				{
					m_cbCallScoreCount++;
					return OnEventGameStart();
				}
			}
			m_wBankerUser=m_wFirstUser;
			m_cbBankerScore=1;
		}

		//����״̬
		m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

		//���ñ���
		if (m_cbBankerScore==0) m_cbBankerScore=1;
		if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wFirstUser;
		m_cbCallScoreCount=0;

		//���͵���
		m_cbHandCardCount[m_wBankerUser]+=CountArray(m_cbBankerCard);
		CopyMemory(&m_cbHandCardData[m_wBankerUser][NORMAL_COUNT],m_cbBankerCard,sizeof(m_cbBankerCard));

		//�����˿�
		m_GameLogic.SortCardList(m_cbHandCardData[m_wBankerUser],m_cbHandCardCount[m_wBankerUser],ST_ORDER);


		//�����û�
		m_wTurnWiner=m_wBankerUser;
		m_wCurrentUser=m_wBankerUser;

		//������Ϣ
		CMD_S_BankerInfo BankerInfo;
		BankerInfo.wBankerUser=m_wBankerUser;
		BankerInfo.wCurrentUser=m_wCurrentUser;
		BankerInfo.cbBankerScore=m_cbBankerScore;
		CopyMemory(BankerInfo.cbBankerCard,m_cbBankerCard,sizeof(m_cbBankerCard));
		
		//��Ϸ����
		m_lScoreTimes = m_cbBankerScore;
		BankerInfo.lScoreTimes = m_lScoreTimes;

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BANKER_INFO,&BankerInfo,sizeof(BankerInfo));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BANKER_INFO,&BankerInfo,sizeof(BankerInfo));

		if (m_pGameVideo)
		{		
			m_pGameVideo->AddVideoData(SUB_S_BANKER_INFO,&BankerInfo);
		}

		//���������й�
		IsOfflineTrustee();

		return true;

	}

	//���������й�
	IsOfflineTrustee();

	return true;
}

//�û�����
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//Ч��״̬
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return true;

	ASSERT(cbCardCount <= m_cbHandCardCount[wChairID] && cbCardCount > 0);
	if (cbCardCount > m_cbHandCardCount[wChairID] || cbCardCount < 1)
	{
		return false;
	}

	//�ж�����Ч
	for (WORD i = 0; i < cbCardCount; i++)
	{
		if (!m_GameLogic.IsValidCard(cbCardData[i]))
		{
			return false;
		}
	}

	//ɾ���˿�
	//��ɾ������ǰ�˷�����10��1�ȴ������ݵ����ȵ���GetCardType��������
	if (m_GameLogic.RemoveCardList(cbCardData, cbCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]) == false)
	{
		ASSERT(FALSE);

		CString strActualCardData;
		for (WORD i = 0; i < cbCardCount; i++)
		{
			CString str;
			str.Format(TEXT("[%d]"), cbCardData[i]);
			strActualCardData += str;
		}

		CString strHandCardData;
		for (WORD i = 0; i < m_cbHandCardCount[wChairID]; i++)
		{
			CString str;
			str.Format(TEXT("[%d]"), m_cbHandCardData[wChairID][i]);
			strHandCardData += str;
		}

		CString strTrace;
		strTrace.Format(TEXT("GAMEID = %d, RemoveCardInvalid�� android = %d"), m_pITableFrame->GetTableUserItem(wChairID)->GetGameID(), m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser());

		CTraceService::TraceString(strTrace, TraceLevel_Exception);
		CTraceService::TraceString(strActualCardData, TraceLevel_Exception);
		CTraceService::TraceString(strHandCardData, TraceLevel_Exception);

		return false;
	}

	//��ȡ����
	BYTE cbTempCardData[MAX_COUNT];
	ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
	CopyMemory(cbTempCardData, cbCardData, sizeof(BYTE)* cbCardCount);

	//�����򣨵���˳�ӱ���Ӵ�С���㣩
	m_GameLogic.SortCardList(cbTempCardData, cbCardCount, ST_ORDER);
	BYTE cbCardType = m_GameLogic.GetCardType(cbTempCardData, cbCardCount);

	//�����ж�
	if (cbCardType==CT_ERROR) 
	{
		ASSERT(FALSE);

		CString strCardData;
		for (WORD i = 0; i < cbCardCount; i++)
		{
			CString str;
			str.Format(TEXT("[%d]"), cbCardData[i]);
			strCardData += str;
		}

		CString strTrace;
		strTrace.Format(TEXT("GAMEID = %d, CardType == CT_ERROR�� android = %d"), m_pITableFrame->GetTableUserItem(wChairID)->GetGameID(), m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser());

		CTraceService::TraceString(strTrace, TraceLevel_Exception);
		CTraceService::TraceString(strCardData, TraceLevel_Exception);

		return false;
	}

	//�����ж�
	if (m_cbTurnCardCount!=0)
	{
		//�Ա��˿�
		if (m_GameLogic.CompareCard(m_cbTurnCardData,cbCardData,m_cbTurnCardCount,cbCardCount)==false)
		{
			ASSERT(FALSE);

			CString strTurnCardData;
			for (WORD i = 0; i < m_cbTurnCardCount; i++)
			{
				CString str;
				str.Format(TEXT("[%d]"), m_cbTurnCardData[i]);
				strTurnCardData += str;
			}

			CString strCardData;
			for (WORD i = 0; i < cbCardCount; i++)
			{
				CString str;
				str.Format(TEXT("[%d]"), cbCardData[i]);
				strCardData += str;
			}

			CString strTrace;
			strTrace.Format(TEXT("GAMEID = %d, CompareInvalid�� android = %d"), m_pITableFrame->GetTableUserItem(wChairID)->GetGameID(), m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser());

			CTraceService::TraceString(strTrace, TraceLevel_Exception);
			CTraceService::TraceString(strTurnCardData, TraceLevel_Exception);
			CTraceService::TraceString(strCardData, TraceLevel_Exception);

			return false;
		}
	}

	//���Ʊ���
	m_cbOutCardCount[wChairID]++;

	//���ñ���
	m_cbTurnCardCount=cbCardCount;
	m_cbHandCardCount[wChairID]-=cbCardCount;
	CopyMemory(m_cbTurnCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//ը���ж�
	if ((cbCardType==CT_BOMB_CARD)||(cbCardType==CT_MISSILE_CARD)) 
	{
		m_cbBombCount++;
		m_cbEachBombCount[wChairID]++;

		m_lScoreTimes *= 2;
	}
	
	//�л��û�
	m_wTurnWiner=wChairID;
	if (m_cbHandCardCount[wChairID]!=0)
	{
		if (cbCardType!=CT_MISSILE_CARD)
		{
			m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
		}
	}
	else m_wCurrentUser=INVALID_CHAIR;

	m_ctChangeTime = CTime::GetCurrentTime();

	for (WORD i = 0; i<cbCardCount; i++)
	{
		POSITION ptkey = m_llistRemainCard.Find(cbCardData[i]);
		if (ptkey)
		{
			m_llistRemainCard.RemoveAt(ptkey);
		}
	}

	//���ַ������˳ɾ�
	if ((m_pITableFrame->GetDataBaseMode() == 0) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	  && m_userachievement[wChairID].dwGameID == m_pITableFrame->GetTableUserItem(wChairID)->GetGameID())
	{
		if (cbCardType == CT_BOMB_CARD)
		{
			m_userachievement[wChairID].cbBombCount++;
		}
		else if (cbCardType == CT_MISSILE_CARD)
		{
			m_userachievement[wChairID].cbMissileCount++;
		}
		else if ((cbCardType == CT_THREE_LINE)
			|| (cbCardType == CT_THREE_TAKE_ONE && cbCardCount > 4)
			|| (cbCardType == CT_THREE_TAKE_TWO && cbCardCount > 5))
		{
			m_userachievement[wChairID].cbPlaneCount++;
		}
	}

	//��������
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbCardCount=cbCardCount;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.cbCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));
	OutCard.lScoreTimes = m_lScoreTimes;

	REMAINCARDCOUNT remaincard;
	ZeroMemory(&remaincard, sizeof(remaincard));
	GetRemainCard(remaincard);

	CopyMemory(&(OutCard.remainCardCount), &remaincard, sizeof(remaincard));

	//��������
	WORD wHeadSize=sizeof(OutCard)-sizeof(OutCard.cbCardData);
	WORD wSendSize=wHeadSize+OutCard.cbCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_OUT_CARD,&OutCard);
	}

	////////////////////////////////////////////////////////////
	CString strCardData;
	for (WORD i = 0; i < cbCardCount; i++)
	{
		CString str;
		str.Format(TEXT("[%d]"), cbCardData[i]);
		strCardData += str;
	}

	IServerUserItem *pIOutCardServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (pIOutCardServerUserItem)
	{
		CString strdebug;
		CTime time = CTime::GetCurrentTime();
        strdebug.Format(TEXT("table id is %d, %d/%d/%d-%d:%d:%d �����ǳơ�%s��GAMEID = %d, chairID = %d, %s ʣ��������� [%d] android = %d\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIOutCardServerUserItem->GetNickName(), pIOutCardServerUserItem->GetGameID(), wChairID, strCardData, m_cbHandCardCount[wChairID], pIOutCardServerUserItem->IsAndroidUser());
        m_pITableFrame->SendGameLogData(strdebug);
	}
	////////////////////////////////////////////////////////////

	//�������
	if (cbCardType==CT_MISSILE_CARD) m_cbTurnCardCount=0;

	//�����ж�
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameConclude(wChairID,NULL,GER_NORMAL);
	else
	{
		OutputDebugString(TEXT("ptdt *** �û�����"));
		//���������й�
		IsOfflineTrustee();			
	}

	return true;
}

//�й��¼�
bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	//״̬Ч��
	ASSERT(wChairID < GAME_PLAYER);
	if (wChairID >= GAME_PLAYER)
	{
		return true;
	}
	CMD_S_TRUSTEE pSTrustee;
	pSTrustee.wTrusteeUser = wChairID;
	pSTrustee.bTrustee = bTrustee;

	//��ȡ�û�
	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (pIServerUserItem != NULL) pIServerUserItem->SetTrusteeUser(bTrustee);

	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TRUSTEE, &pSTrustee, sizeof(CMD_S_TRUSTEE));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TRUSTEE, &pSTrustee, sizeof(CMD_S_TRUSTEE));
	return true;
}

//�����û�
bool CTableFrameSink::OnDebugCard(IServerUserItem * pIServerUserItem)
{
	if (m_pServerDebug != NULL)
	{
		CMD_S_DebugCard DebugCard ;
		ZeroMemory(&DebugCard, sizeof(DebugCard));

		for(WORD i = 0; i < GAME_PLAYER; i++)
		{
			DebugCard.wCardUser[DebugCard.cbUserCount] = i;
			DebugCard.cbCardCount[DebugCard.cbUserCount] = m_cbHandCardCount[i];
			CopyMemory(DebugCard.cbCardData[DebugCard.cbUserCount++],m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
		}
		//���͵���;
		m_pServerDebug->ServerDebug(&DebugCard, m_pITableFrame, pIServerUserItem);
		
	}

	return true;
}

//���͵�����Ϣ
void CTableFrameSink::SendDebugCard()
{
	if (m_pServerDebug != NULL)
	{
		CMD_S_DebugCard DebugCard ;
		ZeroMemory(&DebugCard, sizeof(DebugCard));

		for(WORD i = 0; i < GAME_PLAYER; i++)
		{
			DebugCard.wCardUser[DebugCard.cbUserCount] = i;
			DebugCard.cbCardCount[DebugCard.cbUserCount] = m_cbHandCardCount[i];
			CopyMemory(DebugCard.cbCardData[DebugCard.cbUserCount++],m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
		}
		m_pServerDebug->ServerDebug(&DebugCard, m_pITableFrame);
	}

	return;
}
//���û���
void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

//�Ƿ��й�
bool CTableFrameSink::IsOfflineTrustee(BYTE cbTrusteeTime)
{
	//�Ǳ���ģʽ
	//if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;
	//�ǳ�����λ
	if(m_wCurrentUser==INVALID_CHAIR) return false;

	//��ҳ��ͽ�ҷ������Զ��ߴ��򣬻��ַ��������ߴ���
	//�������
	if (m_bOffLineTrustee && (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
	|| (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0))
	{
		OutputDebugString(TEXT("ptdt *** �����йܴ���"));
		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
		ASSERT(pServerUserItem!=NULL);
		if (pServerUserItem==NULL) return false;

		//ɾ����ʱ��ʱ��
		m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);

		//��������
		bool bTrusteeUser=pServerUserItem->IsTrusteeUser();
		DWORD dwTimerTime=3;
	
		BYTE cbGameStatus=m_pITableFrame->GetGameStatus();
		if (cbGameStatus==GAME_SCENE_CALL) 
		{
			dwTimerTime=m_pGameCustomRule->cbTimeCallScore;
		}
		if (cbGameStatus==GAME_SCENE_PLAY)
		{
			if (m_cbTurnCardCount==0)
				dwTimerTime=m_pGameCustomRule->cbTimeHeadOutCard;
			else
				dwTimerTime=m_pGameCustomRule->cbTimeOutCard;
		}

		dwTimerTime += 1;
		
		BYTE cbDelayTrusteeTime = ((cbTrusteeTime != INVALID_BYTE) ? cbTrusteeTime : (bTrusteeUser ? m_cbTrusteeDelayTime : dwTimerTime));

		m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME, cbDelayTrusteeTime * 1000, -1, NULL);
		return true;
	}

	return false;
}

//д��־�ļ�
void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{	
	//������������
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("LandServerDEBUG%d.txt"), m_pITableFrame->GetTableID());
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

void CTableFrameSink::InitializeListRemainCard()
{
	m_llistRemainCard.RemoveAll();
	BYTE cbRandCard[FULL_COUNT];
	ZeroMemory(cbRandCard, sizeof(cbRandCard));
	m_GameLogic.RandCardList(cbRandCard, CountArray(cbRandCard));
	for (WORD i = 0; i<FULL_COUNT; i++)
	{
		m_llistRemainCard.AddTail(cbRandCard[i]);
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
		case 17://����
		{
					remaincard.cbRemainCount[0]++;
					break;
		}
		case 16://С��
		{
					remaincard.cbRemainCount[1]++;
					break;
		}
		case 15://2
		{
					remaincard.cbRemainCount[2]++;
					break;
		}
		case 14://A
		{
					remaincard.cbRemainCount[3]++;
					break;
		}

		case 13://K
		{
					remaincard.cbRemainCount[4]++;
					break;
		}
		case 12://Q
		{
					remaincard.cbRemainCount[5]++;
					break;
		}
		case 11://J
		{
					remaincard.cbRemainCount[6]++;
					break;
		}
		case 10://10
		{
					remaincard.cbRemainCount[7]++;
					break;
		}

		case 9://9
		{
				   remaincard.cbRemainCount[8]++;
				   break;
		}
		case 8://8
		{
				   remaincard.cbRemainCount[9]++;
				   break;
		}
		case 7://7
		{
				   remaincard.cbRemainCount[10]++;
				   break;
		}
		case 6://6
		{
				   remaincard.cbRemainCount[11]++;
				   break;
		}

		case 5://5
		{
				   remaincard.cbRemainCount[12]++;
				   break;
		}
		case 4://4
		{
				   remaincard.cbRemainCount[13]++;
				   break;
		}
		case 3://3
		{
				   remaincard.cbRemainCount[14]++;
				   break;
		}
		default:
			ASSERT(false);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
