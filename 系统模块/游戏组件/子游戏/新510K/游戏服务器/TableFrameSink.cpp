#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////
#define IDI_CHECK_TABLE				1					//�������
#define IDI_USER_ACTION			2					//��Ҳ���
//���캯��
CTableFrameSink::CTableFrameSink()
{
	//�������
	m_pITableFrame=NULL;
	m_pGameCustomRule=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));


	//������Ϣ
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//�˿���Ϣ
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	ZeroMemory(m_bAddTimes,sizeof(m_bAddTimes));
	ZeroMemory(m_bWinFlag,sizeof(m_bWinFlag));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	m_b2BiggestMode=false;
	m_bCurrenUserDone=false;
	m_cbFriendFlag=0;
	m_bShuangKou=false;
	m_bGameFinish=false;
	return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{

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
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//������Ϣ
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//�˿���Ϣ
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
	m_bGameFinish=false;
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
	m_b2BiggestMode = m_pGameCustomRule->cb2BiggestMode;
	m_GameLogic.Set2Biggest(m_b2BiggestMode);

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
	//�����Ϸ
	if (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{		
		return 48 * m_pITableFrame->GetCellScore();
	}
	return 0L;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//������Ϣ
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wXuanZhanUser=INVALID_CHAIR;
	m_wAskUser=INVALID_CHAIR;
	m_cbFinishUserCount=0;
	m_cbBaseTimes=1;
	m_cbCurTurnScore=0;
	m_bCurrenUserDone=false;
	m_cbFriendFlag=0;
	ZeroMemory(m_wFinishUser,sizeof(m_wFinishUser));
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_bAddTimes,sizeof(m_bAddTimes));
	ZeroMemory(m_cbAskStatus,sizeof(m_cbAskStatus));
	ZeroMemory(m_bAddTimes,sizeof(m_bAddTimes));
	ZeroMemory(m_cbGameScore,sizeof(m_cbGameScore));
	ZeroMemory(m_bWinFlag,sizeof(m_bWinFlag));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	memset(m_wFriend,INVALID_CHAIR,sizeof(m_wFriend));
	m_bShuangKou=false;
	//����״̬
	m_pITableFrame->SetGameStatus(GS_XUAN_ZHAN);

	//�����˿�
	BYTE cbRandCard[FULL_COUNT];
	m_GameLogic.RandCardList(cbRandCard,CountArray(cbRandCard));
	
	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	CString strSaveFileName;
	strSaveFileName.Format(TEXT("%s\\configcard\\510K.card"), szPath);

	CFile file;
	if (file.Open(strSaveFileName, CFile::modeRead))
	{
		file.Read(cbRandCard,sizeof(cbRandCard));		
		file.Close();
	}		

	//�û��˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHandCardCount[i]=NORMAL_CARD_COUNT;
		CopyMemory(&m_cbHandCardData[i],&cbRandCard[i*NORMAL_CARD_COUNT],sizeof(m_cbHandCardData[i]));

		if(m_wBankerUser == INVALID_CHAIR)
		{
			BYTE card = (m_b2BiggestMode==true?0x23:0x22);
			for(int j=0;j<NORMAL_CARD_COUNT;j++)
			{
				if(m_cbHandCardData[i][j] == card)//�ҳ�ׯ�����
				{
					m_wBankerUser = i;
					break;
				}
			}
		}

	}

	//�����û�
	m_wCurrentUser=(m_wBankerUser+1)%GAME_PLAYER;

	//�������
	CMD_S_GameStart GameStart;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.wBanker=m_wBankerUser;
	GameStart.b2Biggest = m_b2BiggestMode;
	//��������
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		CopyMemory(GameStart.cbCardData,m_cbHandCardData[i],sizeof(GameStart.cbCardData));

		//��������
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	//�����˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i],ST_ORDER);
	}

	//�жϽ��
	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		m_pITableFrame->SetGameTimer(IDI_CHECK_TABLE,30000,-1,NULL);
		m_cbWaitTime=0;
	}
	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		m_pITableFrame->KillGameTimer(IDI_CHECK_TABLE);

	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{
			m_pITableFrame->KillGameTimer(IDI_USER_ACTION);
			//��������
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude, sizeof(GameConclude));
			GameConclude.bShuangKou=m_bShuangKou;
			CopyMemory(GameConclude.cbScore,m_cbGameScore,sizeof(GameConclude.cbScore));

			//���ñ���
			GameConclude.cbBaseTimes = m_cbBaseTimes;
			GameConclude.lCellScore = m_pITableFrame->GetCellScore();

			//�û��˿�
			BYTE cbCardIndex = 0;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//�����˿�
				GameConclude.cbCardCount[i] = m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex], m_cbHandCardData[i], m_cbHandCardCount[i]*sizeof(BYTE));

				GameConclude.cbUserTimes[i] = m_cbBaseTimes*(m_bAddTimes[i]?2:1);
				//��������
				cbCardIndex += m_cbHandCardCount[i];
			}

			BYTE cbWinUserCount=0;
			BYTE cbLoseUserCount=0;
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bWinFlag[i])
					cbWinUserCount++;
				else
					cbLoseUserCount++;
			}
			//���ֱ���
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

			//ͳ�ƻ���

			SCORE lCellScore = m_pITableFrame->GetCellScore();

			SCORE lUserScore[GAME_PLAYER]={0};
			for(WORD i=0;i<GAME_PLAYER;i++)
				lUserScore[i] = m_pITableFrame->GetTableUserItem(i)->GetUserScore();

			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_bWinFlag[i] == TRUE)
				{

					for(WORD j=0;j<GAME_PLAYER;j++)
					{
						SCORE LoseScore = lCellScore*m_cbBaseTimes;
						if(m_bAddTimes[i])
							LoseScore *= 2;
						
						if(m_bShuangKou)
							LoseScore *= 2;

						if(m_bWinFlag[j] == FALSE)
						{
							if(m_bAddTimes[j])
								LoseScore *= 2;

							if(LoseScore > lUserScore[j])
							{
								LoseScore = lUserScore[j];
							}

							lUserScore[j] -= LoseScore;
							ScoreInfoArray[j].lScore -= LoseScore;
							ScoreInfoArray[j].cbType = SCORE_TYPE_LOSE;

							ScoreInfoArray[i].lScore += LoseScore;
						}
					}
					
					ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
					ScoreInfoArray[i].lRevenue = m_pITableFrame->CalculateRevenue(i,ScoreInfoArray[i].lScore);
					if(ScoreInfoArray[i].lRevenue > 0L)
						ScoreInfoArray[i].lScore -= ScoreInfoArray[i].lRevenue;
				}

			}

			for(int i=0;i<GAME_PLAYER;i++)
			{
				GameConclude.lGameScore[i] = ScoreInfoArray[i].lScore;
				m_HistoryScore.OnEventUserScore(i,GameConclude.lGameScore[i]);
			}
			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

			//д�����
			m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));

			//������Ϸ
			m_pITableFrame->ConcludeGame(GS_GAME_FREE);

			return true;
		}
	case GER_USER_LEAVE:	//�û�ǿ��
		{
			if((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0)//����ģʽ������
				return true;

			if(m_pITableFrame->GetGameStatus() == GS_PLAY)//��ͨ���ƽ׶��й�
				return OnUserTrustee(wChairID,true);

			return OnUserFlee(wChairID);//���ƽ׶�����
		}
	case GER_NETWORK_ERROR:	//�����ж�
		{
			if((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0)//����ģʽ������
				return true;

			if(m_pITableFrame->GetGameStatus() == GS_PLAY)//��ͨ���ƽ׶��й�
				return OnUserTrustee(wChairID,true);

			return OnUserFlee(wChairID);//���ƽ׶�����
		}
	case GER_DISMISS:		//��Ϸ��ɢ
		{
			m_pITableFrame->KillGameTimer(IDI_USER_ACTION);
			//��������
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude, sizeof(GameConclude));

			//�û��˿�
			BYTE cbCardIndex=0;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//�����˿�
				GameConclude.cbCardCount[i] = m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex], m_cbHandCardData[i], m_cbHandCardCount[i]*sizeof(BYTE));

				//��������
				cbCardIndex += m_cbHandCardCount[i];
			}

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

			//������Ϸ
			m_pITableFrame->ConcludeGame(GS_GAME_FREE);

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
	case GS_GAME_FREE:	//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));

			//���ñ���
			StatusFree.lCellScore = m_pITableFrame->GetCellScore();
			StatusFree.b2Biggest = m_b2BiggestMode;
			//�Զ�����
			StatusFree.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusFree.cbTimeStart = m_pGameCustomRule->cbTimeStartGame;
			StatusFree.cbTimeXuanZhan=m_pGameCustomRule->cbTimeXuanZhan;
			StatusFree.cbTimeFindFriend=m_pGameCustomRule->cbTimeFindFriend;
			StatusFree.cbTimeAskFriend=m_pGameCustomRule->cbTimeAskFriend;
			StatusFree.cbTimeAddTimes=m_pGameCustomRule->cbTimeAddTimes;
			//��ʷ����
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//��������
				tagHistoryScore* pHistoryScore = m_HistoryScore.GetHistoryScore(i);

				//���ñ���
				StatusFree.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
		}
	case GS_XUAN_ZHAN:
	case GS_FIND_FRIEND:
	case GS_ASK_FRIEND:
	case GS_ADD_TIMES:
	case GS_PLAY:	//��Ϸ״̬
		{
			OnUserTrustee(wChairID,false);//ȡ���й�
			//��������
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));

			//��Ԫ����
			StatusPlay.lCellScore = m_pITableFrame->GetCellScore();
			StatusPlay.b2Biggest = m_b2BiggestMode;
			StatusPlay.cbBaseTimes=m_cbBaseTimes;
			//�Զ�����
			StatusPlay.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusPlay.cbTimeStart = m_pGameCustomRule->cbTimeStartGame;
			StatusPlay.cbTimeXuanZhan=m_pGameCustomRule->cbTimeXuanZhan;
			StatusPlay.cbTimeFindFriend=m_pGameCustomRule->cbTimeFindFriend;
			StatusPlay.cbTimeAskFriend=m_pGameCustomRule->cbTimeAskFriend;
			StatusPlay.cbTimeAddTimes=m_pGameCustomRule->cbTimeAddTimes;

			//��Ϸ����
			StatusPlay.wBankerUser = m_wBankerUser;
			StatusPlay.cbSelectedCardData = m_cbSelectedCardData;
			StatusPlay.wAskUser = m_wAskUser;
			StatusPlay.wCurrentUser = m_wCurrentUser;
			StatusPlay.cbGameStatus = m_pITableFrame->GetGameStatus();
			StatusPlay.cbFriendFlag=m_cbFriendFlag;
			StatusPlay.wXuanZhanUser=m_wXuanZhanUser;
			CopyMemory(StatusPlay.bAddTimes,m_bAddTimes,sizeof(StatusPlay.bAddTimes));
			CopyMemory(StatusPlay.wFriend,m_wFriend,sizeof(StatusPlay.wFriend));

			if(StatusPlay.cbGameStatus == GS_ASK_FRIEND)
			{
				StatusPlay.bEnabledAskFriend=true;
				if(m_wXuanZhanUser ==m_wBankerUser && m_wCurrentUser == m_wBankerUser)
					StatusPlay.bEnabledAskFriend=false;
			}
			else if(StatusPlay.cbGameStatus == GS_ADD_TIMES)
			{
				StatusPlay.bEnabledAddTimes=(m_cbAskStatus[wChairID] != 0xFF);
				
				if(m_cbFriendFlag == 6 && (wChairID == m_wAskUser || wChairID == m_wFriend[m_wAskUser]) )
				{
					bool bFlag=false;
					for(int i=0;i<GAME_PLAYER;i++)
					{
						if(i != m_wAskUser && i != m_wFriend[m_wAskUser] && m_cbAskStatus[i] == 0xFF && m_bAddTimes[i])
							bFlag=true;
					}

					if(!bFlag)
						StatusPlay.bEnabledAddTimes=false;
				}
				else if(m_cbFriendFlag == 1 && m_wXuanZhanUser != INVALID_CHAIR)
				{
					bool bFlag=false;
					for(int i=0;i<GAME_PLAYER;i++)
					{
						if(i != m_wXuanZhanUser && m_cbAskStatus[i] == 0xFF && m_bAddTimes[i])
							bFlag=true;
					}
					if(!bFlag)
						StatusPlay.bEnabledAddTimes=false;
				}
			}
			else if(StatusPlay.cbGameStatus == GS_PLAY)
			{
				//������Ϣ
				StatusPlay.wTurnWiner = m_wTurnWiner;
				StatusPlay.cbTurnCardCount = m_cbTurnCardCount;
				StatusPlay.cbTunrScore = m_cbCurTurnScore;
				
				CopyMemory(StatusPlay.cbGameScore,m_cbGameScore,sizeof(StatusPlay.cbGameScore));
				CopyMemory(StatusPlay.cbTurnCardData, m_cbTurnCardData, m_cbTurnCardCount*sizeof(BYTE));
				CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(StatusPlay.bTrustee));
			}
			
			//�˿���Ϣ
			CopyMemory(StatusPlay.cbHandCardCount, m_cbHandCardCount, sizeof(m_cbHandCardCount));
			CopyMemory(StatusPlay.cbHandCardData, m_cbHandCardData[wChairID], sizeof(BYTE)*m_cbHandCardCount[wChairID]);

			//��ʷ����
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//��������
				tagHistoryScore* pHistoryScore = m_HistoryScore.GetHistoryScore(i);

				//���ñ���
				StatusPlay.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		if(wTimerID==IDI_CHECK_TABLE)
		{
			m_cbWaitTime++;
			if(m_cbWaitTime>=3)
			{
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS);
			}
			else if(m_cbWaitTime>=2)
			{
				for(int i=0;i<GAME_PLAYER;i++)
				{
					IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(i);
					if(pServerUserItem!=NULL && pServerUserItem->GetUserStatus()==US_OFFLINE)return true;				
				}
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS);
			}
			return true;
		}
	}
	else if(wTimerID == IDI_USER_ACTION)
	{
		m_pITableFrame->KillGameTimer(IDI_USER_ACTION);
		AutomatismOutCard();
		return true;
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
	m_cbWaitTime=0;
	switch (wSubCmdID)
	{
	case SUB_C_XUAN_ZHAN:	//�û���ս
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_XuanZhan));
			if (wDataSize!=sizeof(CMD_C_XuanZhan)) return false;

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GS_XUAN_ZHAN);
			if (m_pITableFrame->GetGameStatus()!=GS_XUAN_ZHAN) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��������
			CMD_C_XuanZhan * pXuanZhan=(CMD_C_XuanZhan *)pData;

			//��Ϣ����
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserXuanZhan(wChairID,pXuanZhan->bXuanZhan);
		}
	case SUB_C_FIND_FRIEND:	//�û���ͬ��
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_FindFriend));
			if (wDataSize!=sizeof(CMD_C_FindFriend)) return false;

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GS_FIND_FRIEND);
			if (m_pITableFrame->GetGameStatus()!=GS_FIND_FRIEND) return true;
			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;
			//��������
			CMD_C_FindFriend * pFindFriend=(CMD_C_FindFriend *)pData;

			//��Ϣ����
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserFindFriend(wChairID,pFindFriend->cbCardData);
		}
	case SUB_C_ASK_FRIEND:	//�û���ͬ��
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_AskFriend));
			if (wDataSize!=sizeof(CMD_C_AskFriend)) return false;

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GS_ASK_FRIEND);
			if (m_pITableFrame->GetGameStatus()!=GS_ASK_FRIEND) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��������
			CMD_C_AskFriend * pAsk=(CMD_C_AskFriend *)pData;

			//��Ϣ����
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserAskFriend(wChairID,pAsk->bAsk);
		}
	case SUB_C_ADD_TIMES:	//�û��ӱ�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_AddTimes));
			if (wDataSize!=sizeof(CMD_C_AddTimes)) return false;

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GS_ADD_TIMES);
			if (m_pITableFrame->GetGameStatus()!=GS_ADD_TIMES) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��������
			CMD_C_AddTimes * pAddTimes=(CMD_C_AddTimes *)pData;

			//��Ϣ����
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserAddTimes(wChairID,pAddTimes->bAddTimes);
		}
	case SUB_C_OUT_CARD:	//�û�����
		{
			//��������
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//Ч������
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE))));
			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE)))) return false;

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GS_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GS_PLAY) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��Ϣ����
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserOutCard(wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
	case SUB_C_PASS_CARD:	//�û�����
		{
			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GS_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GS_PLAY) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��Ϣ����
			return OnUserPassCard(pIServerUserItem->GetChairID());
		}
	case SUB_C_TRUSTEE://�й�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_Trustee));
			if (wDataSize!=sizeof(CMD_C_Trustee)) return false;

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GS_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GS_PLAY) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			CMD_C_Trustee *pack = (CMD_C_Trustee*)pData;
			return OnUserTrustee(pIServerUserItem->GetChairID(),pack->bTrustee);
		}
	}

	return false;
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ȡ��������
	if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)
	{
		BYTE* pSetInfo = m_pITableFrame->GetGameRule();
		if(pSetInfo[0] == 1)
		{
			m_b2BiggestMode = (pSetInfo[3]==1);
			m_GameLogic.Set2Biggest(m_b2BiggestMode);
		}
	}



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
	//��ʷ����
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserLeave(wChairID);
	}

	return true;
}

bool CTableFrameSink::OnUserXuanZhan(WORD wChairID, bool bXuanZhan)
{
	ASSERT(m_wXuanZhanUser==INVALID_CHAIR);
	if(m_wXuanZhanUser != INVALID_CHAIR) return false;

	CMD_S_XuanZhan pack;
	ZeroMemory(&pack,sizeof(pack));
	pack.wXuanZhanUser=wChairID;
	pack.bXuanZhan = bXuanZhan;
	
	if(bXuanZhan)
	{
		m_cbFriendFlag=1;
		m_cbBaseTimes *=4;
		m_wXuanZhanUser = wChairID;
		pack.wCurrentUser = m_wBankerUser;
		m_wAskUser = m_wXuanZhanUser;
		m_pITableFrame->SetGameStatus(GS_ADD_TIMES);
	}
	else
	{
		WORD wUser = (wChairID+1)%GAME_PLAYER;
		if(wUser == m_wBankerUser)//���Ҷ�û����ս
		{
			m_pITableFrame->SetGameStatus(GS_FIND_FRIEND);
		}

		m_wCurrentUser=wUser;
		pack.wCurrentUser = wUser;
	}
	pack.cbFriendFlag=m_cbFriendFlag;
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_XUAN_ZHAN,&pack,sizeof(pack));
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_XUAN_ZHAN,&pack,sizeof(pack));
	
	return true;
}

bool CTableFrameSink::OnUserFindFriend(WORD wChairID, BYTE cbCardData)
{
	m_cbSelectedCardData = cbCardData;

	CMD_S_FindFriend pack;
	ZeroMemory(&pack,sizeof(pack));
	pack.wFindUser = m_wBankerUser;
	pack.wCurrentUser = m_wBankerUser;
	pack.cbSelectedCardData = cbCardData;
	WORD wFriend=INVALID_CHAIR;
	for(int i=0;i<GAME_PLAYER;i++)
	{
		for(int j=0;j<NORMAL_CARD_COUNT;j++)
		{
			if(m_cbHandCardData[i][j] == cbCardData)
			{
				wFriend = i;
				break;
			}
		}
	}
	ASSERT(wFriend != INVALID_CHAIR);
	pack.wSelectedUser=wFriend;
	if(wFriend == m_wBankerUser)//ѡ�����Լ�
	{
		m_cbFriendFlag=2;
		m_wXuanZhanUser = m_wBankerUser;
		m_cbBaseTimes *= 2;//��һ��
		pack.bEnabled = FALSE;//Ĭ���Լ�����
		m_cbAskStatus[m_wBankerUser]=0xFF;
	}
	else
	{
		m_cbFriendFlag=5;
		//��Ƕ���
		m_wFriend[wFriend]=m_wBankerUser;
		m_wFriend[m_wBankerUser]=wFriend;
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(i != wFriend && i != m_wBankerUser)
			{
				for(int j=i+1;j<GAME_PLAYER;j++)
				{
					if(j != wFriend && j != m_wBankerUser)
					{
						m_wFriend[i]=j;
						m_wFriend[j]=i;
						break;
					}
				}
				break;
			}
		}
		CopyMemory(pack.wFriend,m_wFriend,sizeof(pack.wFriend));
		pack.bEnabled=TRUE;
	}

	m_pITableFrame->SetGameStatus(GS_ASK_FRIEND);

	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FIND_FRIEND,&pack,sizeof(pack));
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FIND_FRIEND,&pack,sizeof(pack));
	return true;
}

bool CTableFrameSink::OnUserAskFriend(WORD wChairID,bool bAsk)
{
	CMD_S_ASKFriend pack;
	ZeroMemory(&pack,sizeof(pack));

	pack.bAsk = bAsk;
	pack.wAskUser=wChairID;
	
	if(bAsk)
	{
		m_cbAskStatus[wChairID]=1;
		m_cbBaseTimes *= 2;

		//��������
		m_wAskUser = wChairID;
		if(m_cbFriendFlag==5)//����
			m_cbFriendFlag=6;
		else if(m_cbFriendFlag==2)//����������
		{
			memset(m_cbAskStatus,0xFF,sizeof(m_cbAskStatus));
			m_cbFriendFlag=4;
		}


		m_pITableFrame->SetGameStatus(GS_ADD_TIMES);
		m_wCurrentUser = m_wBankerUser;
		pack.wXuanZhanUser = m_wXuanZhanUser;
		CopyMemory(pack.wFriend,m_wFriend,sizeof(pack.wFriend));
		pack.cbFriendFlag=m_cbFriendFlag;

		///////////////////////////////////////////////////////////

		//����Ƿ��еж��ܼӱ���������ܣ�ֱ�ӿ�ʼ��Ϸ
		bool bFlag=false;
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(i != m_wAskUser && i != m_wFriend[m_wAskUser])
			{
				if(m_cbAskStatus[i] != 0xFF)
					bFlag=true;
			}
		}

		if(bFlag == false)
		{
			m_pITableFrame->SetGameStatus(GS_PLAY);
			m_wCurrentUser = m_wBankerUser;

			pack.bStart=true;
			pack.wCurrentUser=m_wCurrentUser;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ASK_FRIEND,&pack,sizeof(pack));
		}
		else
		{
			//1ѡ�����ʵ�����Լ�û���ü��ʵ���ҿ��Լӱ�,2ѯ�ʺ�Ҫ�ȵжԼӱ������ѡ��ӱ�
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbAskStatus[i] == 0xFF || i == m_wAskUser || i == m_wFriend[m_wAskUser])
					pack.wCurrentUser = INVALID_CHAIR;
				else
					pack.wCurrentUser=i;

				m_pITableFrame->SendTableData(i,SUB_S_ASK_FRIEND,&pack,sizeof(pack));
			}
		}
	}
	else
	{
		m_cbAskStatus[wChairID]=0xFF;
		WORD wUser = (wChairID+1)%GAME_PLAYER;
		if(wUser == m_wBankerUser)//�����ʣ�ֱ�ӳ���
		{
			m_pITableFrame->SetGameStatus(GS_PLAY);
		}
		m_wCurrentUser = wUser;
		pack.wCurrentUser = wUser;
		pack.cbFriendFlag=m_cbFriendFlag;
		CopyMemory(pack.wFriend,m_wFriend,sizeof(pack.wFriend));
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ASK_FRIEND,&pack,sizeof(pack));
	}

	
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ASK_FRIEND,&pack,sizeof(pack));
	

	return true;
}
//�û��ӱ�
bool CTableFrameSink::OnUserAddTimes(WORD wChairID, bool bAddTimes)
{
	//Ч�����
	ASSERT(m_bAddTimes[wChairID]==FALSE);
	if(m_bAddTimes[wChairID]==TRUE) return false;

	m_bAddTimes[wChairID]=bAddTimes;
	m_cbAskStatus[wChairID]=0xFF;

	if(bAddTimes == false)//�жԷ������ӱ���ѯ�ʷ����ܼӱ�
	{
		BYTE cbCount=0;
		for(WORD i=0;i<GAME_PLAYER;i++)
		{
			if(i != m_wAskUser && i != m_wFriend[m_wAskUser])
			{
				if(m_cbAskStatus[i] == 0xFF && m_bAddTimes[i]==false)
					cbCount++;
			}
		}

		if(cbCount >= 2)
		{
			if(m_cbFriendFlag == 6)
			{
				m_cbAskStatus[m_wAskUser]=0xFF;
				m_bAddTimes[m_wAskUser]=false;
				m_cbAskStatus[m_wFriend[m_wAskUser]]=0xFF;
				m_bAddTimes[m_wFriend[m_wAskUser]]=false;
			}
			else if(m_cbFriendFlag == 1 && cbCount == 3)
			{
				m_cbAskStatus[m_wXuanZhanUser]=0xFF;
				m_bAddTimes[m_wXuanZhanUser]=false;
			}
		}

	}


	CMD_S_AddTimes pack;
	ZeroMemory(&pack,sizeof(pack));
	pack.bAddTimes=bAddTimes;
	pack.wAddTimesUser=wChairID;
	pack.wCurrentUser=m_wBankerUser;
	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbAskStatus[i] != 0xFF)
		{
			//������û�лش�
			pack.wCurrentUser=INVALID_CHAIR;
			break;
		}
	}
	if(pack.wCurrentUser != INVALID_CHAIR)
	{
		m_pITableFrame->SetGameStatus(GS_PLAY);
		m_wCurrentUser=m_wBankerUser;
	}
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_TIMES,&pack,sizeof(pack));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_TIMES,&pack,sizeof(pack));

	if(pack.wCurrentUser == INVALID_CHAIR && bAddTimes)//ѯ�ʷ��ĵжԼӱ���֪ͨѯ�ʷ��ܼӱ�����Ҳ���
	{
		CMD_S_AddTimes packNotify;
		ZeroMemory(&packNotify,sizeof(packNotify));
		packNotify.wAddTimesUser=INVALID_CHAIR;
		packNotify.wCurrentUser=INVALID_CHAIR;

		if(m_cbFriendFlag == 1 && m_cbAskStatus[m_wXuanZhanUser] != 0xFF)
		{
			m_pITableFrame->SendTableData(m_wXuanZhanUser,SUB_S_ADD_TIMES,&packNotify,sizeof(packNotify));
		}
		else if(m_cbFriendFlag == 6)
		{
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(i != wChairID && i != m_wFriend[wChairID] && m_cbAskStatus[i] != 0xFF)
				{
					m_pITableFrame->SendTableData(i,SUB_S_ADD_TIMES,&packNotify,sizeof(packNotify));
				}
			}
		}

	}

	return true;
}


//�û�����
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount,bool bTrusteeOut/*=false*/)
{
	//Ч��״̬
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return false;

	//��ȡ����
	BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);

	//�����ж�
	if (cbCardType==CT_ERROR) 
	{
		ASSERT(FALSE);
		return false;
	}

	//�����ж�
	if (m_cbTurnCardCount!=0)
	{
		//�Ա��˿�
		if (m_GameLogic.CompareCard(m_cbTurnCardData,cbCardData,m_cbTurnCardCount,cbCardCount)==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//ɾ���˿�
	if (m_GameLogic.RemoveCardList(cbCardData,cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//���Ʊ���
	m_cbOutCardCount[wChairID]++;

	//���ñ���
	m_cbTurnCardCount=cbCardCount;
	m_cbHandCardCount[wChairID]-=cbCardCount;
	CopyMemory(m_cbTurnCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//ͳ����һ���Ʒ���
	for(int i=0;i<m_cbTurnCardCount;i++)
	{
		BYTE value = m_GameLogic.GetCardLogicValue(m_cbTurnCardData[i]);
		if(value==5 || value==10 || value==13)
		{
			m_cbCurTurnScore += (value==5?5:10);
		}
	}
	
	if(m_bGameFinish)//��Ϸ�Ѿ�����
	{
		m_cbTurnCardCount=0;
		m_cbGameScore[wChairID] += m_cbCurTurnScore;
		m_cbCurTurnScore=0;

		return OnEventGameConclude(wChairID,NULL,GER_NORMAL);
	}
	//�л��û�
	m_wTurnWiner=wChairID;

	if(m_cbFinishUserCount<GAME_PLAYER)
	{
		do
		{
			m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
		}while(m_cbHandCardCount[m_wCurrentUser]==0);
	}

	if (m_cbHandCardCount[wChairID] == 0)//��ҳ������ˣ��ж��Ƿ����
	{
		m_wFinishUser[m_cbFinishUserCount++]=wChairID;

		if(m_wXuanZhanUser != INVALID_CHAIR)//��������ֱ�ӽ���
		{
			//m_wCurrentUser=INVALID_CHAIR;
			m_bGameFinish=true;
			if(wChairID == m_wXuanZhanUser)
				m_bWinFlag[wChairID]=TRUE;
			else
			{
				for(int i=0;i<GAME_PLAYER;i++)
				{
					if(i != m_wXuanZhanUser)
					{
						m_bWinFlag[i]=TRUE;
					}
				}
			}
		}
		else
		{
			if(m_cbFinishUserCount==1)
			{
// 				WORD wFirst = m_wFinishUser[0];
// 				if(m_cbGameScore[wFirst] + m_cbGameScore[m_wFriend[wFirst]]>=50 )//���ߵ�һ��������50,�Ѿ�Ӯ��
// 				{
// 					m_wCurrentUser=INVALID_CHAIR;
// 					m_bWinFlag[wFirst]=TRUE;
// 					m_bWinFlag[m_wFriend[wFirst]]=TRUE;
// 				}
			}
			else if(m_cbFinishUserCount == 2)
			{
				WORD wFirst = m_wFinishUser[0];
				if( (m_wFriend[wFirst] == m_wFinishUser[1]) || (m_cbGameScore[wFirst] + m_cbGameScore[m_wFriend[wFirst]]>=50) )//һ�����������ߵ�һ��������50
				{
					//m_wCurrentUser=INVALID_CHAIR;
					m_bGameFinish=true;
					m_bWinFlag[wFirst]=TRUE;
					m_bWinFlag[m_wFriend[wFirst]]=TRUE;
					if(m_wFriend[wFirst] == m_wFinishUser[1])//һ�����ߣ�˫��
						m_bShuangKou=true;
				}
			}
			else if(m_cbFinishUserCount == 3)
			{
				//m_wCurrentUser=INVALID_CHAIR;
				m_bGameFinish=true;
				WORD wFirst = m_wFinishUser[0];
				WORD wSecond = m_wFinishUser[1];
				BYTE cbFirstScore = m_cbGameScore[wFirst]+m_cbGameScore[m_wFriend[wFirst]];
				BYTE cbSecondScore = m_cbGameScore[wSecond]+m_cbGameScore[m_wFriend[wSecond]];
				if(m_wFriend[wFirst] == wChairID)
				{
					cbFirstScore += 30;
					cbSecondScore -= 30;

					m_cbGameScore[wFirst]+=30;
					m_cbGameScore[m_wFriend[wSecond]]-=30;
					
				}

				if(cbFirstScore >= cbSecondScore)
				{
					m_bWinFlag[wFirst]=TRUE;
					m_bWinFlag[m_wFriend[wFirst]]=TRUE;
				}
				else
				{
					m_bWinFlag[wSecond]=TRUE;
					m_bWinFlag[m_wFriend[wSecond]]=TRUE;
				}
			}

		}

	}
	//��������
	CMD_S_OutCard OutCard;
	ZeroMemory(&OutCard,sizeof(OutCard));
	OutCard.wOutCardUser=wChairID;
	OutCard.cbCardCount=cbCardCount;
	OutCard.wCurrentUser=m_wCurrentUser;
	OutCard.cbCurTurnScore=m_cbCurTurnScore;
	OutCard.bTrusteeOut = bTrusteeOut;
	CopyMemory(OutCard.cbCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	if(m_cbFriendFlag==5 || m_cbFriendFlag==2)//ͬ��δ֪����£�ѡ����ƴ����ѡ����ƣ���ʾ��ϵ
	{
		for(int i=0;i<m_cbTurnCardCount;i++)
		{
			if(m_cbSelectedCardData == m_cbTurnCardData[i])
			{
				if(m_cbFriendFlag==5)
					m_cbFriendFlag=6;//������ʾ
				else
					m_cbFriendFlag=3;//������ʾ

				CMD_S_Friend pack;
				pack.cbFriendFlag=m_cbFriendFlag;
				pack.wXuanZhanUser=m_wXuanZhanUser;
				CopyMemory(pack.wFriend,m_wFriend,sizeof(pack.wFriend));
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FRIEND,&pack,sizeof(pack));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FRIEND,&pack,sizeof(pack));
				break;
			}
		}
	}


	m_bCurrenUserDone=false;

	//�����ж�,˫�ۡ���������������ս������£������ж�����������
	if (m_bGameFinish && (m_cbCurTurnScore==0 || m_wXuanZhanUser != INVALID_CHAIR || (m_cbFinishUserCount==2 && m_wFinishUser[0]==m_wFinishUser[1]) ))
	{
		OnEventGameConclude(wChairID,NULL,GER_NORMAL);
	}
	else if(m_bTrustee[m_wCurrentUser] == true)
	{
		m_bCurrenUserDone=true;
		m_pITableFrame->SetGameTimer(IDI_USER_ACTION,1000,-1,NULL);
	}
	return true;
}

//�û�����
bool CTableFrameSink::OnUserPassCard(WORD wChairID,bool bTrusteePass/*=false*/)
{
	//Ч��״̬
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbTurnCardCount!=0));
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return false;

	do
	{
		m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
		if (m_wCurrentUser==m_wTurnWiner)
		{
			m_cbTurnCardCount=0;
			m_cbGameScore[m_wTurnWiner] += m_cbCurTurnScore;
			m_cbCurTurnScore=0;

			if(m_bGameFinish)
			{
				return OnEventGameConclude(wChairID,NULL,GER_NORMAL);
			}
			if(m_cbFriendFlag == 6 && m_cbHandCardCount[m_wCurrentUser]==0)//�����ƣ���ͬ��ȷ������£���ͬ�˽ӷ�
			{
				ASSERT(m_cbHandCardCount[m_wFriend[m_wCurrentUser]] > 0);
				m_wCurrentUser = m_wFriend[m_wCurrentUser];
			}
		}

	}while(m_cbHandCardCount[m_wCurrentUser]==0 );

	if(m_cbFinishUserCount==2)
	{
		WORD wFirstFinishUser = m_wFinishUser[0];
		BYTE cbScore = m_cbGameScore[wFirstFinishUser] + m_cbGameScore[m_wFriend[wFirstFinishUser]];
		if(cbScore>=50)
			m_wCurrentUser=INVALID_CHAIR;
	}
	//������Ϣ
	CMD_S_PassCard PassCard;
	PassCard.wPassCardUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.cbTurnOver=(m_cbTurnCardCount==0)?TRUE:FALSE;
	PassCard.cbTurnWinnerScore = m_cbGameScore[m_wTurnWiner];
	PassCard.wTurnWinner=m_wTurnWiner;
	PassCard.bTrusteePass=bTrusteePass;
	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	m_bCurrenUserDone=false;
	if(m_wCurrentUser != INVALID_CHAIR && m_bTrustee[m_wCurrentUser])
	{
		m_bCurrenUserDone=true;
		m_pITableFrame->SetGameTimer(IDI_USER_ACTION,1000,-1,NULL);
	}
	
	if(m_wCurrentUser == INVALID_CHAIR)
	{
		WORD wFirst=m_wFinishUser[0];
		m_bWinFlag[wFirst]=TRUE;
		m_bWinFlag[m_wFriend[wFirst]]=TRUE;
		OnEventGameConclude(wChairID,NULL,GER_NORMAL);
	}
	return true;
}

bool CTableFrameSink::OnUserTrustee(WORD wChairID,bool bTrustee)
{
	if(m_bTrustee[wChairID] != bTrustee)
	{
		CMD_S_Trustee pack;
		pack.wChair = wChairID;
		pack.bTrustee = bTrustee;

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&pack,sizeof(pack));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&pack,sizeof(pack));
	}

	m_bTrustee[wChairID]=bTrustee;

	if(bTrustee && m_wCurrentUser == wChairID && !m_bCurrenUserDone)
	{
		AutomatismOutCard();
	}

	return true;
}


//�Զ�����
bool CTableFrameSink::AutomatismOutCard()
{
	//���ƴ���
	m_GameLogic.SearchOutCard(m_cbHandCardData[m_wCurrentUser],m_cbHandCardCount[m_wCurrentUser],m_cbTurnCardData,m_cbTurnCardCount,&m_SearchCardResult);
	//���ý���
	if (m_SearchCardResult.cbSearchCount>0)
	{
		OnUserOutCard(m_wCurrentUser,m_SearchCardResult.cbResultCard[0],m_SearchCardResult.cbCardCount[0],true);
		return true;
	}
	else if(m_cbTurnCardCount != 0)
	{
		OnUserPassCard(m_wCurrentUser,true);
		return true;
	}

	return true;
}


//���û���
void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

bool CTableFrameSink::OnUserFlee(WORD wChairID)
{
	m_pITableFrame->KillGameTimer(IDI_USER_ACTION);
	//��������
	CMD_S_GameConclude GameConclude;
	ZeroMemory(&GameConclude, sizeof(GameConclude));

	//���ñ���
	GameConclude.cbBaseTimes = m_cbBaseTimes;
	GameConclude.lCellScore = m_pITableFrame->GetCellScore();
	CopyMemory(GameConclude.cbScore,m_cbGameScore,sizeof(GameConclude.cbScore));
	//�û��˿�
	BYTE cbCardIndex = 0;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//�����˿�
		GameConclude.cbCardCount[i] = m_cbHandCardCount[i];
		CopyMemory(&GameConclude.cbHandCardData[cbCardIndex], m_cbHandCardData[i], m_cbHandCardCount[i]*sizeof(BYTE));

		//��������
		cbCardIndex += m_cbHandCardCount[i];
	}

	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		m_bWinFlag[i] = (i!=wChairID);

		GameConclude.cbUserTimes[i]=m_cbBaseTimes*(m_bAddTimes[i]?2:1);
	}
	BYTE cbWinUserCount=3;
	BYTE cbLoseUserCount=1;

	//���ֱ���
	tagScoreInfo ScoreInfoArray[GAME_PLAYER];
	ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

	//ͳ�ƻ���

	SCORE lCellScore = m_pITableFrame->GetCellScore();

	SCORE lUserScore[GAME_PLAYER]={0};
	for(WORD i=0;i<GAME_PLAYER;i++)
		lUserScore[i] = m_pITableFrame->GetTableUserItem(i)->GetUserScore();

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_bWinFlag[i] == TRUE)
		{

			for(WORD j=0;j<GAME_PLAYER;j++)
			{
				SCORE LoseScore = lCellScore*m_cbBaseTimes;
				if(m_bAddTimes[i])
					LoseScore *= 2;

				if(m_bWinFlag[j] == FALSE)
				{
					if(m_bAddTimes[j])
						LoseScore *= 2;

					if(LoseScore > lUserScore[j])
					{
						LoseScore = lUserScore[j];
					}

					lUserScore[j] -= LoseScore;
					ScoreInfoArray[j].lScore -= LoseScore;
					ScoreInfoArray[j].cbType = SCORE_TYPE_FLEE;

					ScoreInfoArray[i].lScore += LoseScore;
				}
			}

			ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
			ScoreInfoArray[i].lRevenue = m_pITableFrame->CalculateRevenue(i,ScoreInfoArray[i].lScore);
			if(ScoreInfoArray[i].lRevenue > 0L)
				ScoreInfoArray[i].lScore -= ScoreInfoArray[i].lRevenue;
		}

	}

	for(int i=0;i<GAME_PLAYER;i++)
	{
		GameConclude.lGameScore[i] = ScoreInfoArray[i].lScore;
		m_HistoryScore.OnEventUserScore(i,GameConclude.lGameScore[i]);
	}
	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

	//д�����
	m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));

	//������Ϸ
	m_pITableFrame->ConcludeGame(GS_GAME_FREE);

	return true;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
