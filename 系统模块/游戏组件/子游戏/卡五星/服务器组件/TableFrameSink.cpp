#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////
#define IDI_OUT_CARD 1
#define IDI_CALL_PIAO 2
#define IDI_OPERATE	3

//////////////////////////////////////////////////////////////////////////
//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��Ϸ����
	m_lSiceCount=0;
	m_pGameCustomRule=NULL;
	m_wBankerUser=INVALID_CHAIR;
	m_cbPiaoMode=0;
	m_cbPlayerCount=GAME_PLAYER;
	m_bFinishPiao=false;
	m_cbMaMode=0;
	m_cbMaCount=0;
	m_cbRule=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory(m_cbListenStatus,sizeof(m_cbListenStatus));
	ZeroMemory( m_lGangScore,sizeof(m_lGangScore) );
	ZeroMemory(m_cbSelectCard,sizeof(m_cbSelectCard));
	ZeroMemory(m_cbSelectCount,sizeof(m_cbSelectCount));
	
	m_cbGangStatus=WIK_GANERAL;
	m_cbPlayerCount=GAME_PLAYER;
	//������Ϣ
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//������Ϣ
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//������Ϣ
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	m_cbEndLeftCount = 0;

	//���б���
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;
	m_wFirstListenUser=INVALID_CHAIR;

	//״̬����
	m_bSendStatus=false;
	m_bGangPao=false;
	m_cbGangCount=0;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));

	//�û�״̬
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//����˿�
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//������Ϣ
	m_cbChiHuCard=0;
	ZeroMemory( m_dwChiHuKind,sizeof(m_dwChiHuKind) );

	//�������
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	ZeroMemory(m_cbUserPiao,sizeof(m_cbUserPiao));
	ZeroMemory(m_cbHuCardCount,sizeof(m_cbHuCardData));
	ZeroMemory(m_cbHuCardData,sizeof(m_cbHuCardData));
	ZeroMemory(&m_stRecordInfo,sizeof(m_stRecordInfo));
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_bPlayStatus, sizeof(m_bPlayStatus));
	//��Ϸ��Ƶ
	m_hVideoInst = NULL;
	m_pGameVideo = NULL;
	m_hVideoInst = LoadLibrary(TEXT("SparrowXYKWXGameVideo.dll"));
	if (m_hVideoInst)
	{
		typedef void * (*CREATE)();
		CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
		if (GameVideo)
		{
			m_pGameVideo = static_cast<IGameVideo*>(GameVideo());
		}
	}

	m_cbControlGameCount = 0;
	ZeroMemory(m_cbControlRepertoryCard, sizeof(m_cbControlRepertoryCard));
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
	ASSERT(m_pGameServiceOption);
	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//�Զ�����
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	//��ҳ�Ĭ������
	m_cbPiaoMode = 2;
	m_cbMaMode = 0;
	m_cbMaCount = 0;
	m_cbRule = 0;
		
	return true;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	//��Ϸ����
	m_lSiceCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory(m_cbListenStatus,sizeof(m_cbListenStatus));
	ZeroMemory( m_lGangScore,sizeof(m_lGangScore) );
	ZeroMemory(m_cbSelectCard,sizeof(m_cbSelectCard));
	ZeroMemory(m_cbSelectCount,sizeof(m_cbSelectCount));
	
	m_cbGangStatus=WIK_GANERAL;

	//������Ϣ
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//������Ϣ
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//������Ϣ
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	m_cbEndLeftCount = 0;

	//���б���
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;
	m_wFirstListenUser=INVALID_CHAIR;

	//״̬����
	m_bSendStatus=false;
	m_bGangPao=false;
	m_cbGangCount=0;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));

	//�û�״̬
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//����˿�
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//������Ϣ
	m_cbChiHuCard=0;
	ZeroMemory(m_dwChiHuKind,sizeof(m_dwChiHuKind));

	ZeroMemory(m_cbHuCardCount,sizeof(m_cbHuCardCount));
	ZeroMemory(m_cbHuCardData,sizeof(m_cbHuCardData));

	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));

	ZeroMemory(m_cbUserPiao,sizeof(m_cbUserPiao));
	ZeroMemory(m_bPlayStatus, sizeof(m_bPlayStatus));
	return;
}

//��������
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{

	return (SCORE)0;
}

//���ٻ���
SCORE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return (SCORE)0;
}

//�����¼�
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}
//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return true;
}
//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	CString cc;
	cc.Format(_T("��Ϸ��ʼ"));
	WriteDebugInfo(cc);

	if (m_pGameVideo)
	{
		m_pGameVideo->StartVideo(m_pITableFrame);
	}

	m_lCellScore = m_pITableFrame->GetCellScore();
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		IServerUserItem *pUser = m_pITableFrame->GetTableUserItem(i);
		if(pUser && pUser->GetUserStatus()>=US_PLAYING)
		{
			m_bPlayStatus[i] = true;
			m_lUserScore[i] = pUser->GetUserScore();
			cc.Format(_T("���%d������Ϸ"),i);
			WriteDebugInfo(cc);
		}
	}
	if( (m_cbPiaoMode==1 && m_bFinishPiao==false) || m_cbPiaoMode==2)
	{
		WriteDebugInfo(_T("��ʼ��Ư"));
		if(m_cbPiaoMode==1)
			m_bFinishPiao=true;

		m_pITableFrame->SetGameStatus(GS_MJ_PIAO);

		CMD_S_StartCallPiao StartCallPiao;
		CopyMemory(StartCallPiao.bPlayStatus, m_bPlayStatus, sizeof(StartCallPiao.bPlayStatus));
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CALLPIAO, &StartCallPiao, sizeof(StartCallPiao));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALLPIAO, &StartCallPiao, sizeof(StartCallPiao));
		
		if (IsRoomCardScoreType())
			m_pITableFrame->SetGameTimer(IDI_CALL_PIAO, (m_pGameCustomRule->cbTimeCallPiao+5) * 1000, 1, 0);
	}
	else
	{
		GameStart();
	}

	return true;
}

void CTableFrameSink::GameStart()
{
	WriteDebugInfo(_T("��ʼ����"));
	//����״̬
	m_pITableFrame->KillGameTimer(IDI_CALL_PIAO);
	m_pITableFrame->SetGameStatus(GS_MJ_PLAY);
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	//�����˿�
	if( m_wBankerUser == INVALID_CHAIR )
		m_lSiceCount = MAKELONG(MAKEWORD(rand()%6+1,rand()%6+1),MAKEWORD(rand()%6+1,rand()%6+1));
	else
		m_lSiceCount = MAKELONG(MAKEWORD(rand()%6+1,rand()%6+1),0);

	m_cbLeftCardCount=CountArray(m_cbRepertoryCard);
	m_GameLogic.RandCardData(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));
	m_GameLogic.SetSiGuiYi(m_cbRule==0);

	if (m_cbControlGameCount == 0)
	{
		TCHAR szPath[MAX_PATH] = TEXT("");
		GetCurrentDirectory(sizeof(szPath), szPath);
		CString strSaveFileName;
		strSaveFileName.Format(TEXT("%s\\SparrowKWX.dat"), szPath);

		CFile file;
		if (file.Open(strSaveFileName, CFile::modeRead))
		{
			file.Read(&m_wBankerUser, sizeof(m_wBankerUser));
			file.Read(m_cbRepertoryCard, sizeof(m_cbRepertoryCard));
			file.Close();
			WriteDebugInfo(_T("��ȡ���óɹ�"));
		}
	}
	else
	{
		m_cbControlGameCount = 0;
		m_wBankerUser = 0;
		CopyMemory(m_cbRepertoryCard, m_cbControlRepertoryCard, sizeof(m_cbRepertoryCard));
	}

	//�ַ��˿�
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		if (m_bPlayStatus[i]==false)
			continue;
		m_cbLeftCardCount-=(MAX_COUNT-1);
		m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);
	}

	if( m_wBankerUser == INVALID_CHAIR || m_bPlayStatus[m_wBankerUser]==false)
	{
		WORD wSice = (WORD)(m_lSiceCount>>16);
		m_wBankerUser = (HIBYTE(wSice)+LOBYTE(wSice))%m_cbPlayerCount;
		for (int i = 0; i < m_cbPlayerCount; i++)
		{
			if (m_bPlayStatus[m_wBankerUser])
				break;
			m_wBankerUser = (m_wBankerUser + 1) % m_cbPlayerCount;
		}
	}

	//�����˿�
	m_cbSendCardCount++;
	m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	//���ñ���
	m_cbProvideCard=m_cbSendCardData;
	m_wProvideUser=m_wBankerUser;
	m_wCurrentUser=m_wBankerUser;

	//������Ϣ
	WORD wSice = WORD(m_lSiceCount&0xffff);
	BYTE cbSiceTakeCount= HIBYTE(wSice)+LOBYTE(wSice);
	WORD wTakeChairID=(m_wBankerUser+cbSiceTakeCount-1)%GAME_PLAYER;
	while( cbSiceTakeCount*2 > HEAP_FULL_COUNT )
	{
		wTakeChairID = (wTakeChairID-1+GAME_PLAYER)%GAME_PLAYER;
		cbSiceTakeCount -= HEAP_FULL_COUNT/2;
	}
	m_wHeapTail = wTakeChairID;
	BYTE cbTakeCount=MAX_REPERTORY-m_cbLeftCardCount;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//������Ŀ
		BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSiceTakeCount)*2:0);
		BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

		//��ȡ�˿�
		cbTakeCount-=cbRemoveCount;
		m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;

		//����ж�
		if (cbTakeCount==0)
		{
			m_wHeapHead=wTakeChairID;
			break;
		}
		//�л�����
		wTakeChairID=(wTakeChairID-1+GAME_PLAYER)%GAME_PLAYER;
	}

	//ׯ���ж�
	if ( m_wBankerUser!=INVALID_CHAIR )
	{		
		//�����ж�
		tagGangCardResult GangCardResult;
		m_cbUserAction[m_wBankerUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[m_wBankerUser],NULL,0,GangCardResult);

		//�����ж�
		DWORD chr;
		m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]--;
		m_cbUserAction[m_wBankerUser]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[m_wBankerUser],NULL,0,m_cbSendCardData,chr);
		m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

		//�����ж�
		BYTE cbRes = m_GameLogic.AnalyseTingCard(m_cbCardIndex[m_wBankerUser], 0, 0);
		if (cbRes == WIK_LISTEN)
		{
			if(SendTingData(m_wBankerUser, m_cbCardIndex[m_wBankerUser], 0, 0))
				m_cbUserAction[m_wBankerUser] |= WIK_LISTEN;
		}
	}

	//��������
	CMD_S_GameStart GameStart;
	GameStart.lSiceCount=m_lSiceCount;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser= m_wCurrentUser;
	GameStart.wHeapHead = m_wHeapHead;
	GameStart.wHeapTail = m_wHeapTail;
	CopyMemory(GameStart.cbHeapCardInfo,m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));
	CopyMemory(GameStart.bPlayStatus, m_bPlayStatus, sizeof(GameStart.bPlayStatus));
	//��������
	bool bFirst = true;
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem == NULL)
			continue;

		//���ñ���
		GameStart.cbUserAction=m_cbUserAction[i];

		ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
		if (m_bPlayStatus[i])
			m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData);

		//��������
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

		if (m_pGameVideo && m_bPlayStatus[i])
		{
			Video_GameStart video;
			ZeroMemory(&video, sizeof(video));
			lstrcpyn(video.szNickName, pServerUserItem->GetNickName(), CountArray(video.szNickName));
			video.lScore = pServerUserItem->GetUserScore();
			video.lCellScore = m_lCellScore;
			video.wChairID = i;
			video.wBankerUser = m_wBankerUser;
			video.lSiceCount = m_lSiceCount;
			video.cbUserAction = m_cbUserAction[i];
			CopyMemory(video.cbCardData, GameStart.cbCardData, sizeof(BYTE)*MAX_COUNT);

			//����
			video.cbPlayerCount = m_cbPlayerCount;
			video.cbPiaoMode = m_cbPiaoMode;
			video.cbMaMode = m_cbMaMode;
			video.cbMaCount = m_cbMaCount;
			video.cbRule =	m_cbRule;

			video.cbUserPiao = m_cbUserPiao[i];
			m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video,sizeof(video),bFirst);
			bFirst = false;
		}
	}

	if (!IsRoomCardScoreType())
	{
		if (m_bTrustee[m_wCurrentUser])
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD, 10 * 1000, 1, 0);
		else
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD, (m_pGameCustomRule->cbTimeOutCard+10) * 1000, 1, 0);
	}

	if ((m_cbUserAction[m_wBankerUser] & WIK_LISTEN) != 0)
		SendTingData(m_wBankerUser, m_cbCardIndex[m_wBankerUser], 0, 0);

	CString cc;
	cc.Format(_T("���ƽ���,ׯ��%d�ɶ���%d"), m_wBankerUser, m_cbUserAction[m_wBankerUser]);
	WriteDebugInfo(cc);
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{
			m_pITableFrame->KillGameTimer(IDI_CALL_PIAO);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
			for (WORD i = 0; i < m_cbPlayerCount; i++)
				m_pITableFrame->KillGameTimer(IDI_OPERATE + i);
			//��������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			GameEnd.cbProvideCard = m_cbProvideCard;
			GameEnd.wProvideUser = m_wProvideUser;
			

			//������Ϣ
			BYTE cbWinUserCount=0;
			WORD wWinUser[GAME_PLAYER];
			for (WORD i=0;i<m_cbPlayerCount;i++)
			{
				if (m_bPlayStatus[i])
				{
					GameEnd.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameEnd.cbCardData[i]);
					GameEnd.cbWeaveItemCount[i] = m_cbWeaveItemCount[i];
					CopyMemory(GameEnd.WeaveItemArray[i], m_WeaveItemArray[i], sizeof(GameEnd.WeaveItemArray[i]));
				}
				GameEnd.dwChiHuKind[i]=m_dwChiHuKind[i];
				if( m_dwChiHuKind[i] != WIK_CHI_HU )
					continue;
				wWinUser[cbWinUserCount++]=i;
				FiltrateRight(i,m_ChiHuRight[i]);
				GameEnd.dwChiHuRight[i]=m_ChiHuRight[i];
			}

			//ͳ�ƻ���
			if (cbWinUserCount>0)
			{
				SCORE lTempScore=0;

				if(m_wProvideUser == wWinUser[0])//����
				{
					//�����
					BYTE cbMaMultiples = GetMaMultiples(wWinUser[0],m_cbProvideCard);
					if(m_cbLeftCardCount>0 && (m_cbMaMode==1 || (m_cbMaMode==2 && m_cbListenStatus[wWinUser[0]]>0)))
					{
						int nCount=1;
						if(m_cbMaCount==2)//��6��
							nCount=6;
						for(int i=0;i<nCount && i<m_cbLeftCardCount;i++)
						{
							GameEnd.cbMaData[i]=m_cbRepertoryCard[i];
							GameEnd.cbMaCount++;
						}
					}

					//���Ʒ�
					WORD wFan = m_GameLogic.GetChiHuActionRank( m_ChiHuRight[wWinUser[0]] );
					GameEnd.cbFanCount[wWinUser[0]] = wFan*(m_cbListenStatus[wWinUser[0]]>0?2:1);
					for(int i=0;i<m_cbPlayerCount;i++)
					{
						if(i == wWinUser[0] || m_bPlayStatus[i]==false)
							continue;
						BYTE cbTime=1;
						if(m_cbListenStatus[wWinUser[0]]>0 || m_cbListenStatus[i]>0)//����һ���������ӱ�
							cbTime=2;
						lTempScore = wFan*cbTime + m_cbUserPiao[wWinUser[0]] + m_cbUserPiao[i]+cbMaMultiples;					
						
						lTempScore *= m_lCellScore;
						lTempScore = __min(lTempScore,m_lUserScore[i]);

						//���Ʒ�
						GameEnd.lGameScore[wWinUser[0]] += lTempScore;
						m_lUserScore[wWinUser[0]] += lTempScore;
						GameEnd.lGameScore[i] -= lTempScore;
						m_lUserScore[i] -= lTempScore;

						//Ư��
						GameEnd.lPiaoScore[wWinUser[0]] += (m_cbUserPiao[wWinUser[0]]+m_cbUserPiao[i])*m_lCellScore;
						GameEnd.lPiaoScore[i] -= (m_cbUserPiao[wWinUser[0]]+m_cbUserPiao[i])*m_lCellScore;

						//���
						GameEnd.lMaScore[wWinUser[0]] += cbMaMultiples*m_lCellScore;
						GameEnd.lPiaoScore[i] -= cbMaMultiples*m_lCellScore;
					}

					//������¼
					m_stRecordInfo.cbZiMo[wWinUser[0]]++;
				}
				else
				{
					for(int i=0;i<cbWinUserCount;i++)
					{
						//���Ʒ���
						WORD wFan = m_GameLogic.GetChiHuActionRank( m_ChiHuRight[wWinUser[i]] );
						GameEnd.cbFanCount[wWinUser[i]] = wFan*(m_cbListenStatus[wWinUser[i]]>0?2:1);

						BYTE cbTime=1;
						if(m_cbListenStatus[wWinUser[i]]>0 || m_cbListenStatus[m_wProvideUser]>0)//����һ���������ӱ�
							cbTime = 2;

						lTempScore = wFan*cbTime + m_cbUserPiao[wWinUser[i]] + m_cbUserPiao[m_wProvideUser];					
						lTempScore *= m_lCellScore;
						lTempScore = __min(lTempScore,m_lUserScore[m_wProvideUser]);

						//���Ʒ�
						GameEnd.lGameScore[wWinUser[i]] += lTempScore;
						m_lUserScore[wWinUser[i]] += lTempScore;
						GameEnd.lGameScore[m_wProvideUser] -= lTempScore;
						m_lUserScore[m_wProvideUser] -=  lTempScore;
						//Ư��
						GameEnd.lPiaoScore[wWinUser[i]] += (m_cbUserPiao[wWinUser[i]]+m_cbUserPiao[m_wProvideUser])*m_lCellScore;
						GameEnd.lPiaoScore[m_wProvideUser] -= (m_cbUserPiao[wWinUser[i]]+m_cbUserPiao[m_wProvideUser])*m_lCellScore;

						m_stRecordInfo.cbJiePao[wWinUser[i]]++;
						m_stRecordInfo.cbDianPao[m_wProvideUser]++;
					}
				}

			}
			else
			{
				//���ֽ���
				if (m_wFirstListenUser!=INVALID_CHAIR)
				{
					SCORE lTempScore = (m_cbPlayerCount-1)*m_lCellScore;
					lTempScore = __min(lTempScore,m_lUserScore[m_wFirstListenUser]);
					GameEnd.lGameScore[m_wFirstListenUser] =-1*lTempScore;

					for (BYTE i=0;i<m_cbPlayerCount;i++)
					{
						if(i!=m_wFirstListenUser)
						{
							GameEnd.lGameScore[i] = lTempScore/(m_cbPlayerCount-1);
						}

					}
				}				
			}

			//���ϸܷ�
			for (BYTE User=0;User<m_cbPlayerCount;User++)
			{				
				GameEnd.lGameScore[User]+= m_lGangScore[User];
				GameEnd.lGangScore[User] = m_lGangScore[User];
			}

			//ͳ�ƻ���
			SCORE lGameTax[GAME_PLAYER];
			ZeroMemory(lGameTax,sizeof(lGameTax));


			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			//ͳ�ƻ���
			for (WORD i=0;i<m_cbPlayerCount;i++)
			{
				if (m_bPlayStatus[i]==false) continue;
				if (GameEnd.lGameScore[i]>0L)
				{
					lGameTax[i] = GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenueRatio/1000L;
					GameEnd.lGameScore[i] -= lGameTax[i];
				}
				ScoreInfo[i].lScore = GameEnd.lGameScore[i];
				ScoreInfo[i].lRevenue = lGameTax[i];

				if( GameEnd.lGameScore[i] == 0 ) 
					ScoreInfo[i].cbType = SCORE_TYPE_DRAW;
				else  
					ScoreInfo[i].cbType = (GameEnd.lGameScore[i]>0?SCORE_TYPE_WIN:SCORE_TYPE_LOSE);
		
				if (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL && m_stRecordInfo.nCount<32)
				{
					m_stRecordInfo.lAllScore[i] += GameEnd.lGameScore[i];
					m_stRecordInfo.lDetailScore[i][m_stRecordInfo.nCount] = GameEnd.lGameScore[i];
				}

			}
			m_stRecordInfo.nCount++;
			if (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)
			{
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecordInfo, sizeof(m_stRecordInfo));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecordInfo, sizeof(m_stRecordInfo));
			}
			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd, sizeof(GameEnd), true);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID(),m_cbPlayerCount);
			}

			//д�����
			for (WORD i = 0; i < m_cbPlayerCount; i++)
			{
				if (m_bPlayStatus[i] == false) continue;
				m_pITableFrame->WriteUserScore(i, ScoreInfo[i]);
			}

			//���ñ���
			if (cbWinUserCount > 0)
			{
				if(cbWinUserCount==1)
					m_wBankerUser = wWinUser[0];
				else
					m_wBankerUser = m_wProvideUser;
			}
			else
			{
				//���ֽ���
				m_wBankerUser = m_wBankerUser;
			}			

			//������Ϸ
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);

			return true;
		}
	case GER_DISMISS:		//��Ϸ��ɢ
		{
			m_pITableFrame->KillGameTimer(IDI_CALL_PIAO);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
			for (WORD i = 0; i < m_cbPlayerCount; i++)
				m_pITableFrame->KillGameTimer(IDI_OPERATE + i);
			//��������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//���ñ���
			GameEnd.wProvideUser=INVALID_CHAIR;

			//�����˿�
			for (WORD i=0;i<m_cbPlayerCount;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//������Ϸ
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);

			return true;
		}
	case GER_USER_LEAVE:	//�û�ǿ��
	case GER_NETWORK_ERROR:	//�����ж�
		{
			CString cc;
			cc.Format(_T("���ǿ��:wchair=%d,cbreson=%d"), wChairID, cbReason);
			WriteDebugInfo(cc);
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
	case GS_MJ_FREE:	//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));
			StatusFree.cbTimeStart = m_pGameCustomRule->cbTimeStart;
			StatusFree.cbTimeCallPiao = m_pGameCustomRule->cbTimeCallPiao;
			StatusFree.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
			StatusFree.cbTimeOperate = m_pGameCustomRule->cbTimeOperate;
			//��������
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			StatusFree.cbPlayerCount=m_cbPlayerCount;
			StatusFree.cbPiaoMode=m_cbPiaoMode;
			StatusFree.cbMaMode = m_cbMaMode;
			StatusFree.cbMaCount=m_cbMaCount;
			StatusFree.cbRule = m_cbRule;
			StatusFree.wTableOwnerID = m_pITableFrame->GetTableOwner();
			StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_MJ_PIAO://��Ư״̬
		{
			//��������
			CMD_S_StatusPiao StatusPiao;
			ZeroMemory(&StatusPiao, sizeof(StatusPiao));
			StatusPiao.cbTimeStart = m_pGameCustomRule->cbTimeStart;
			StatusPiao.cbTimeCallPiao = m_pGameCustomRule->cbTimeCallPiao;
			StatusPiao.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
			StatusPiao.cbTimeOperate = m_pGameCustomRule->cbTimeOperate;
			//��������
			StatusPiao.lCellScore = m_pGameServiceOption->lCellScore;
			StatusPiao.cbPlayerCount = m_cbPlayerCount;
			StatusPiao.cbPiaoMode = m_cbPiaoMode;
			StatusPiao.cbMaMode = m_cbMaMode;
			StatusPiao.cbMaCount = m_cbMaCount;
			StatusPiao.cbRule = m_cbRule;
			StatusPiao.wTableOwnerID = m_pITableFrame->GetTableOwner();

			CopyMemory(StatusPiao.bPlayStatus, m_bPlayStatus, sizeof(StatusPiao.bPlayStatus));
			CopyMemory(StatusPiao.bTrustee, m_bTrustee, sizeof(StatusPiao.bTrustee));

			CopyMemory(StatusPiao.bCallPiao, m_bResponse, sizeof(StatusPiao.bCallPiao));
			CopyMemory(StatusPiao.cbUserPiao, m_cbUserPiao, sizeof(StatusPiao.cbUserPiao));
			StatusPiao.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPiao, sizeof(StatusPiao));
		}
	case GS_MJ_PLAY:	//��Ϸ״̬
		{
			//��������
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));
			StatusPlay.cbTimeStart = m_pGameCustomRule->cbTimeStart;
			StatusPlay.cbTimeCallPiao = m_pGameCustomRule->cbTimeCallPiao;
			StatusPlay.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
			StatusPlay.cbTimeOperate = m_pGameCustomRule->cbTimeOperate;
			//��Ϸ����
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;
			StatusPlay.cbPlayerCount=m_cbPlayerCount;
			StatusPlay.cbPiaoMode=m_cbPiaoMode;
			StatusPlay.cbMaMode = m_cbMaMode;
			StatusPlay.cbMaCount=m_cbMaCount;
			StatusPlay.cbRule = m_cbRule;
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//״̬����
			StatusPlay.cbActionCard=m_cbProvideCard;
			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			StatusPlay.cbActionMask=(m_bResponse[wChairID]==false)?m_cbUserAction[wChairID]:WIK_NULL;

			//����״̬
			CopyMemory( StatusPlay.cbHearStatus,m_cbListenStatus,sizeof(m_cbListenStatus) );
			for(int i=0;i<m_cbPlayerCount;i++)
			{
				if(i == wChairID || m_cbListenStatus[i] > 0)
				{
					CopyMemory(StatusPlay.cbCardIndex[i],m_cbCardIndex[i],sizeof(StatusPlay.cbCardIndex[i]));
					CopyMemory(StatusPlay.cbLinstenCardData[i],m_cbSelectCard[i],sizeof(StatusPlay.cbLinstenCardData[i]));	
				}

			}
		

			//��ʷ��¼
			StatusPlay.wOutCardUser=m_wOutCardUser;
			StatusPlay.cbOutCardData=m_cbOutCardData;
			CopyMemory(StatusPlay.cbDiscardCard,m_cbDiscardCard,sizeof(StatusPlay.cbDiscardCard));
			CopyMemory(StatusPlay.cbDiscardCount,m_cbDiscardCount,sizeof(StatusPlay.cbDiscardCount));

			//����˿�
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveCount,m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

			//������Ϣ
			StatusPlay.wHeapHead = m_wHeapHead;
			StatusPlay.wHeapTail = m_wHeapTail;
			CopyMemory(StatusPlay.cbHeapCardInfo,m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));			

			//�˿�����			
			for (int i = 0; i < m_cbPlayerCount; i++)
			{
				if (m_bPlayStatus[i])
					StatusPlay.cbCardCount[i] = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
			}
			if (m_bPlayStatus[wChairID])
				m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID],StatusPlay.cbCardData);
			StatusPlay.cbSendCardData=(m_wProvideUser==wChairID)?m_cbProvideCard:0x00;
			
			CopyMemory(StatusPlay.cbHuCardCount,m_cbHuCardCount,sizeof(m_cbHuCardCount));
			CopyMemory(StatusPlay.cbHuCardData,m_cbHuCardData,sizeof(m_cbHuCardData));
			//����
			StatusPlay.wTableOwnerID = m_pITableFrame->GetTableOwner();
			//Ư
			CopyMemory(StatusPlay.cbUserPiao,m_cbUserPiao,sizeof(StatusPlay.cbUserPiao));

			CopyMemory(StatusPlay.bPlayStatus, m_bPlayStatus, sizeof(StatusPlay.bPlayStatus));
			StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			//���ͳ���
			m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			if (m_wCurrentUser == wChairID && m_GameLogic.AnalyseTingCard(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID])==WIK_LISTEN)
				SendTingData(wChairID, m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID]);

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	if(wTimerID == IDI_OUT_CARD)
	{
		m_pITableFrame->KillGameTimer(wTimerID);
		CString cc;
		cc.Format(_T("���Ƶ���ʱ��������ǰ���%d"), m_wCurrentUser);
		WriteDebugInfo(cc);
		return OnUserTrustee(m_wCurrentUser,true);
	}
	else if (wTimerID == IDI_CALL_PIAO)
	{
		m_pITableFrame->KillGameTimer(wTimerID);
		for (WORD i = 0; i < m_cbPlayerCount; i++)
		{
			if (m_bPlayStatus[i] && m_bResponse[i] == false)
			{
				CString cc;
				cc.Format(_T("��Ư����ʱ���������%d"), i);
				WriteDebugInfo(cc);
				OnUserTrustee(i, true);
			}
		}
		return true;
	}
	else if (wTimerID >= IDI_OPERATE && wTimerID < IDI_OPERATE + GAME_PLAYER)
	{
		m_pITableFrame->KillGameTimer(wTimerID);
		WORD wChairID = wTimerID - IDI_OPERATE;
		CString cc;
		cc.Format(_T("��������ʱ���������%d"), wChairID);
		WriteDebugInfo(cc);
		return OnUserTrustee(wChairID, true);
	}

	return false;
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_CALLPIAO://��Ư
		{
			//Ч����Ϣ
			ASSERT(wDataSize==sizeof(CMD_C_CallPiao));
			if (wDataSize!=sizeof(CMD_C_CallPiao)) return false;

			//�û�Ч��
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��Ϣ����
			CMD_C_CallPiao * pCallPiao=(CMD_C_CallPiao*)pData;
			return OnUserCallPiao(pIServerUserItem->GetChairID(),pCallPiao->cbPiao);
		}
	case SUB_C_OUT_CARD:		//������Ϣ
		{
			//Ч����Ϣ
			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
			if (wDataSize!=sizeof(CMD_C_OutCard)) return false;

			//�û�Ч��
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��Ϣ����
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			return OnUserOutCard(pIServerUserItem->GetChairID(),pOutCard->cbCardData);
		}
	case SUB_C_OPERATE_CARD:	//������Ϣ
		{
			//Ч����Ϣ
			ASSERT(wDataSize==sizeof(CMD_C_OperateCard));
			if (wDataSize!=sizeof(CMD_C_OperateCard)) return false;

			//�û�Ч��
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��Ϣ����
			CMD_C_OperateCard * pOperateCard=(CMD_C_OperateCard *)pData;
			return OnUserOperateCard(pIServerUserItem->GetChairID(),pOperateCard->cbOperateCode,pOperateCard->cbOperateCard);
		}
	case SUB_C_TRUSTEE:
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pData;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;

			//�û�Ч��
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;
			if (IsRoomCardScoreType()) return true;//Լռ�������й�
			CString cc;
			cc.Format(_T("�յ�����й�����chair=%d,btru=%d"), pIServerUserItem->GetChairID(), pTrustee->bTrustee);
			WriteDebugInfo(cc);
			return OnUserTrustee(pIServerUserItem->GetChairID(),pTrustee->bTrustee);
		}
	case SUB_C_LISTEN:
		{
			//Ч����Ϣ
			ASSERT(wDataSize==sizeof(CMD_C_Listen));
			if (wDataSize!=sizeof(CMD_C_Listen)) return false;

			//�û�Ч��
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			CMD_C_Listen *pListen = (CMD_C_Listen *)pData;
			ASSERT((pListen->cbSelectCount%3)==0);

			m_cbSelectCount[pIServerUserItem->GetChairID()]=pListen->cbSelectCount;
			CopyMemory(m_cbSelectCard[pIServerUserItem->GetChairID()],pListen->cbSelectCard,sizeof(BYTE)*pListen->cbSelectCount);
			return OnUserListenCard(pIServerUserItem->GetChairID(),pListen->cbListen);
		}
	case SUB_C_RECORD:
		{
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_RECORD, &m_stRecordInfo, sizeof(m_stRecordInfo));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_RECORD, &m_stRecordInfo, sizeof(m_stRecordInfo));
			return true;
		}
	}

	return false;
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (wSubCmdID == SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		m_cbControlGameCount = 1;
		CopyMemory(m_cbControlRepertoryCard, (BYTE*)pData, wDataSize);
		return true;
	}
	return false;
}
//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	CString cc;
	cc.Format(_T("���%d����"), wChairID);
	WriteDebugInfo(cc);
	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	CString cc;
	cc.Format(_T("��ҽ���:chair=%d,gameid=%d,blook=%d,name=%s"), wChairID, pIServerUserItem->GetGameID(),bLookonUser, pIServerUserItem->GetNickName());
	WriteDebugInfo(cc);
	if (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)
	{
		//��ȡ��������
		BYTE* pSetInfo = m_pITableFrame->GetGameRule();
		if (pSetInfo[0] == 1)
		{
			m_cbPlayerCount = pSetInfo[1];
			BYTE cbMaxPlayer = pSetInfo[2];
			m_cbPiaoMode = pSetInfo[3];//Ưģʽ 0��Ư��1Ưһ�Σ�2ÿ��Ư
			m_cbMaMode = pSetInfo[4];//0������1��������2������������
			if (m_cbMaMode > 0)
			{
				m_cbMaCount = pSetInfo[5];//0�ޣ�1����1��2����6��3����1��1
				//ASSERT(m_cbMaCount > 0);
			}
			m_cbRule = pSetInfo[6];//0ȫƵ����1��Ƶ��
			m_pITableFrame->SetTableChairCount(m_cbPlayerCount);
			m_pITableFrame->SetStartMode(START_MODE_FULL_READY);
		}
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{

	if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)
	{
		m_bFinishPiao=false;
		ZeroMemory(&m_stRecordInfo,sizeof(m_stRecordInfo));
		m_bFinishPiao=false;
	}

	CString cc;
	cc.Format(_T("����뿪:chair=%d,gameid=%d"), wChairID, pIServerUserItem->GetGameID());
	WriteDebugInfo(cc);
	return true;
}

//�û�ͬ��
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) 
{
	return true;

}


//�û�����
bool CTableFrameSink::OnUserListenCard(WORD wChairID,BYTE cbListen)
{
	//Ч��״̬
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//Ч�����
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbListenStatus[wChairID]==0));
	if ((wChairID!=m_wCurrentUser)||(m_cbListenStatus[wChairID]!=0)) return false;

	CString ss;
	ss.Format(_T("���%d����"), wChairID);
	WriteDebugInfo(ss);
	//���ñ���
	m_cbListenStatus[wChairID] = cbListen;
	if(m_wFirstListenUser == INVALID_CHAIR)
		m_wFirstListenUser=wChairID;

	//��������
	CMD_S_Listen ListenCard;
	ListenCard.wChairId=wChairID;
	ListenCard.cbSelectCount=m_cbSelectCount[wChairID];
	CopyMemory(ListenCard.cbSelectCard,m_cbSelectCard[wChairID],sizeof(ListenCard.cbSelectCard));
	CopyMemory(ListenCard.cbCardIndex,m_cbCardIndex[wChairID],sizeof(ListenCard.cbCardIndex));
	
	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LISTEN,&ListenCard,sizeof(ListenCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LISTEN,&ListenCard,sizeof(ListenCard));

	return true;
}

//�û���Ư
bool CTableFrameSink::OnUserCallPiao(WORD wChairID,BYTE cbPiao)
{
	//Ч��״̬
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PIAO);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PIAO) return true;

	ASSERT(!m_bResponse[wChairID] && m_bPlayStatus[wChairID]);
	if (m_bResponse[wChairID] || !m_bPlayStatus[wChairID])
		return false;
	
	CString ss;
	ss.Format(_T("���%d��Ư%d"), wChairID,cbPiao);
	WriteDebugInfo(ss);
	m_bResponse[wChairID]=true;
	m_cbUserPiao[wChairID]=cbPiao;

	CMD_S_CallPiao pack;
	pack.cbPiao = cbPiao;
	pack.wUser = wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PIAO,&pack,sizeof(pack));
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PIAO,&pack,sizeof(pack));

	bool bFlag=true;
	for(WORD i=0;i<m_cbPlayerCount;i++)
	{
		if(m_bPlayStatus[i]==true && m_bResponse[i]==false)
		{
			bFlag=false;
			break;
		}
	}
	if(bFlag)//����Ӧ��
		GameStart();
	

	return true;
}

bool CTableFrameSink::OnUserTrustee(WORD wChairID,bool bTrustee)
{
	//Ч��״̬
	ASSERT((wChairID < m_cbPlayerCount));
	if ((wChairID>=m_cbPlayerCount)) return false;

	if (IsRoomCardScoreType() && m_cbListenStatus[wChairID] == 0)
		return true;
	bool bChange = false;
	if(bTrustee != m_bTrustee[wChairID] && m_cbListenStatus[wChairID]==0)
	{
		bChange = true;
		m_bTrustee[wChairID]=bTrustee;
		CMD_S_Trustee Trustee;
		Trustee.bTrustee = bTrustee;
		Trustee.wChairID = wChairID;
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));

		if (bTrustee)
		{
			CString cc;
			cc.Format(_T("���%d�й�"), wChairID);
			WriteDebugInfo(cc);
		}
		else
		{
			CString cc;
			cc.Format(_T("���%dȡ���й�"), wChairID);
			WriteDebugInfo(cc);
		}
	}

	if(bTrustee)
	{
		CString ss;
		ss.Format(_T("���%d�йܲ���"), wChairID);
		WriteDebugInfo(ss);
		if(m_pITableFrame->GetGameStatus() == GS_MJ_PIAO)
		{
			if(m_bResponse[wChairID]==false)
				OnUserCallPiao(wChairID,0);
		}
		else if(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY)
		{
			if(wChairID == m_wCurrentUser)
			{
				BYTE cardindex=INVALID_BYTE;
				if(m_cbListenStatus[wChairID]>0 && m_cbHuCardCount[wChairID]==0)//ѡ�������ƣ���û��
				{
					BYTE cbOutCardCount=0;
					BYTE cbOutCardData[MAX_COUNT]={0};
					BYTE cbCardIndexTemp[MAX_INDEX]={0};
					CopyMemory( cbCardIndexTemp,m_cbCardIndex[wChairID],sizeof(cbCardIndexTemp) );

					//�������Щ������
					for( BYTE i = 0; i < MAX_INDEX; i++ )
					{
						if( cbCardIndexTemp[i] == 0 ) continue;
						cbCardIndexTemp[i]--;

						bool bHu=false;
						for( BYTE j = 0; j < MAX_INDEX; j++ )
						{
							BYTE cbCurrentCard = m_GameLogic.SwitchToCardData(j);
							DWORD chr;
							if( WIK_CHI_HU == m_GameLogic.AnalyseChiHuCard(cbCardIndexTemp,m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID],cbCurrentCard,chr) )
							{
								bHu=true;
								break;
							}
						}
						if(bHu)
							cbOutCardData[cbOutCardCount++]=m_GameLogic.SwitchToCardData(i);

						cbCardIndexTemp[i]++;
					}
					//�����Լ���ס����
					if(m_cbSelectCount[wChairID]>0)
					{
						for(int i=0;i<cbOutCardCount;i++)
						{
							for(int j=0;j<m_cbSelectCount[wChairID];j++)
							{
								if(cbOutCardData[i]==m_cbSelectCard[wChairID][j])
								{
									cbOutCardData[i]=0;
									break;
								}
							}
						}
					}

					//��������������ҵĺ���
					for (int i = 0; i < m_cbPlayerCount; i++)
					{
						if (i == wChairID || m_cbHuCardCount[i] == 0)continue;
						for (int j = 0; j < m_cbHuCardCount[i]; j++)
						{
							for (int k = 0; k < cbOutCardCount; k++)
							{
								if (cbOutCardData[k] == m_cbHuCardData[i][j])
									cbOutCardData[k] = 0;
							}
						}
					}

					//ѡһ�ſɳ�����
					for(int i=0;i<cbOutCardCount;i++)
					{
						if (cbOutCardData[i] != 0)
						{
							cardindex = m_GameLogic.SwitchToCardIndex(cbOutCardData[i]);
							break;
						}
					}

					//���˺�û���ˣ�ֻ�����һ�Ÿ����˺�
					if (cardindex == INVALID_BYTE)
					{
						for (int i = 0; i < cbOutCardCount; i++)
						{
							if (cbOutCardData[i] != 0)
							{
								cardindex = m_GameLogic.SwitchToCardIndex(cbOutCardData[i]);
								break;
							}
						}
					}

				}
				else
				{
					if (m_cbSendCardData != 0 && bCanOut(wChairID, m_cbSendCardData))
						cardindex = m_GameLogic.SwitchToCardIndex(m_cbSendCardData);
					else
					{
						for(int i=0;i<MAX_INDEX;i++)
						{
							if (m_cbCardIndex[wChairID][i]>0 && bCanOut(wChairID, m_GameLogic.SwitchToCardData(i)))
							{
								cardindex = i;
								break;
							}
						}
					}
					if (cardindex == INVALID_BYTE)
					{
						for (int i = 0; i < MAX_INDEX; i++)
						{
							if (m_cbCardIndex[wChairID][i] > 0)
							{
								cardindex = i;
								break;
							}
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
		
	}
	else
	{
		if (m_wCurrentUser == wChairID && bChange)
		{
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeOutCard * 1000, 1, 0);
		}
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData,bool bSysOut/*=false*/)
{
	CString ss;
	ss.Format(_T("���%d����%0x,curuser=%d,sysout=%d,trustee=%d"), wChairID, cbCardData, m_wCurrentUser, bSysOut ? 1 : 0, m_bTrustee[wChairID] ? 1 : 0);
	WriteDebugInfo(ss);
	//Ч��״̬
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//�������
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//Ч�����
	if (wChairID!=m_wCurrentUser) return false;
	if (m_GameLogic.IsValidCard(cbCardData)==false) return false;	

	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
 	if(m_cbListenStatus[wChairID]==0)
 	{
 		//û���Ƶ���Ҳ��ܳ���������ҵĺ���
 		for(WORD i=0;i<m_cbPlayerCount;i++)
 		{
 			if(i == wChairID)
 				continue;
 
 			if(m_cbListenStatus[i] > 0)
 			{
 				for(int j=0;j<m_cbHuCardCount[i];j++)
 				{
 					if(cbCardData == m_cbHuCardData[i][j])
 					{
 						CString str;
 						str.Format(TEXT("���%s����������ҵĺ���"),m_pITableFrame->GetTableUserItem(wChairID)->GetNickName());
 						CTraceService::TraceString(str,TraceLevel_Exception);
 						return false;
 					}
 				}
 			}
 		}
 	}
	//ɾ���˿�
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);
		return false;
	}	

	//���ñ���
	m_bSendStatus=true;
	
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;
	m_cbGangStatus=WIK_GANERAL;

	//���Ƽ�¼
	m_cbOutCardCount++;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData;
	m_cbSendCardData = 0;
	//��������
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;
	OutCard.bSysOut = bSysOut;
	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_OUT_CARD, &OutCard, sizeof(OutCard), true);
	}

	if(m_cbListenStatus[wChairID] >0 && m_cbHuCardCount[wChairID] == 0)
	{
		BYTE cbWeaveCount=m_cbWeaveItemCount[wChairID];
		m_cbHuCardCount[wChairID] = m_GameLogic.GetHuCard(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],cbWeaveCount,m_cbHuCardData[wChairID]);

		CMD_S_HuCard HuCard;
		HuCard.wUser = wChairID;
		HuCard.cbHuCount = m_cbHuCardCount[wChairID];
		CopyMemory(HuCard.cbHuCardData,m_cbHuCardData[wChairID],sizeof(HuCard.cbHuCardData));

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_HU_CARD,&HuCard,sizeof(HuCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_HU_CARD,&HuCard,sizeof(HuCard));
	}

	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;
	if (m_cbGangStatus != WIK_GANERAL)
	{
        m_cbGangStatus = WIK_GANERAL;
		m_bGangPao = true;
	}
	else
		m_bGangPao = false;

	//�û��л�
	m_wCurrentUser=(wChairID+1)%m_cbPlayerCount;
	for (int i = 0; i < m_cbPlayerCount; i++)
	{
		if (m_bPlayStatus[m_wCurrentUser])
			break;
		m_wCurrentUser = (m_wCurrentUser + 1) % m_cbPlayerCount;
	}

	//��Ӧ�ж�
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//�ɷ��˿�
	if (bAroseAction == false)
	{
		m_cbGangCount = 0;
		DispatchCardData(m_wCurrentUser);
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard[3])
{
	//Ч��״̬
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY)
		return true;

	//Ч���û� ע�⣺���л�����ʱ�п��ܻᷢ���˶���
	//ASSERT((wChairID==m_wCurrentUser)||(m_wCurrentUser==INVALID_CHAIR));
	if ((wChairID!=m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR)) 
		return true;

	//��������
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//Ч��״̬
		if (m_bResponse[wChairID]==true) return true;
		if( m_cbUserAction[wChairID] == WIK_NULL ) return true;
		if ((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)==0)) return true;

		//�رն�ʱ��
		m_pITableFrame->KillGameTimer(IDI_OPERATE + wChairID);

		//��������
		WORD wTargetUser=wChairID;
		BYTE cbTargetAction=cbOperateCode;

		//���ñ���
		m_bResponse[wChairID]=true;
		m_cbPerformAction[wChairID]=cbOperateCode;
		if(cbOperateCard[0]==0)
			m_cbOperateCard[wChairID][0] = m_cbProvideCard;
		else CopyMemory( m_cbOperateCard[wChairID],cbOperateCard,sizeof(m_cbOperateCard[wChairID]) );

		//ִ���ж�
		for (WORD i=0;i<m_cbPlayerCount;i++)
		{
			//��ȡ����
			BYTE cbUserAction=(m_bResponse[i]==false)?m_cbUserAction[i]:m_cbPerformAction[i];

			//���ȼ���
			BYTE cbUserActionRank=m_GameLogic.GetUserActionRank(cbUserAction);
			BYTE cbTargetActionRank=m_GameLogic.GetUserActionRank(cbTargetAction);

			//�����ж�
			if (cbUserActionRank>cbTargetActionRank)
			{
				wTargetUser=i;
				cbTargetAction=cbUserAction;
			}
		}
		if (m_bResponse[wTargetUser]==false) 
			return true;

		//�Ժ��ȴ�
		if (cbTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<m_cbPlayerCount;i++)
			{
				if ((m_bResponse[i]==false)&&(m_cbUserAction[i]&WIK_CHI_HU))
					return true;
			}
		}

		//�ر����ж�ʱ��
		for (WORD i = 0; i < m_cbPlayerCount; i++)
			m_pITableFrame->KillGameTimer(IDI_OPERATE + i);

		//��������
		if (cbTargetAction==WIK_NULL)
		{
			//�û�״̬
			ZeroMemory(m_bResponse,sizeof(m_bResponse));
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
			ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));
			m_cbGangCount = 0;
			//�����˿�
			DispatchCardData(m_wResumeUser);

			return true;
		}

		//��������
		BYTE cbTargetCard=m_cbOperateCard[wTargetUser][0];

		//���Ʊ���
		m_cbOutCardData=0;
		m_bSendStatus=true;
		m_wOutCardUser=INVALID_CHAIR;
		m_cbSendCardData = 0;
		//���Ʋ���
		if (cbTargetAction==WIK_CHI_HU)
		{
			//������Ϣ
			m_cbChiHuCard=cbTargetCard;

			for ( WORD i=0;i<m_cbPlayerCount;i++)
			{
				//�����ж�
				if ((m_cbPerformAction[i]&WIK_CHI_HU)==0)
					continue;

				//����У��
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[i];
				
				m_dwChiHuKind[i] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveItemCount,m_cbChiHuCard,m_ChiHuRight[i]);
				//�����˿�
				if (m_dwChiHuKind[i]==WIK_CHI_HU) 
				{
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
				}
			}

			//������Ϸ
			OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);

			return true;
		}

		//����˿�
		ASSERT(m_cbWeaveItemCount[wTargetUser]<MAX_WEAVE);
		WORD wIndex=m_cbWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard=cbTargetCard;
		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind=cbTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;
		m_WeaveItemArray[wTargetUser][wIndex].cbCardData[0] = cbTargetCard;
		if( cbTargetAction&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
		{
			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[1] = m_cbOperateCard[wTargetUser][1];
			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[2] = m_cbOperateCard[wTargetUser][2];
		}
		else
		{
			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[1] = cbTargetCard;
			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[2] = cbTargetCard;
			if( cbTargetAction & WIK_GANG )
				m_WeaveItemArray[wTargetUser][wIndex].cbCardData[3] = cbTargetCard;
		}

		//ɾ���˿�
		switch (cbTargetAction)
		{
		case WIK_LEFT:		//���Ʋ���
			{
				//ɾ���˿�
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],&m_cbOperateCard[wTargetUser][1],2) )
				{
					ASSERT( FALSE );
					return false;
				}

				break;
			}
		case WIK_RIGHT:		//���Ʋ���
			{
				//ɾ���˿�
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],&m_cbOperateCard[wTargetUser][1],2) )
				{
					ASSERT( FALSE );
					return false;
				}

				break;
			}
		case WIK_CENTER:	//���Ʋ���
			{
				//ɾ���˿�
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],&m_cbOperateCard[wTargetUser][1],2) )
				{
					ASSERT( FALSE );
					return false;
				}

				break;
			}
		case WIK_PENG:		//���Ʋ���
			{
				//ɾ���˿�
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard};
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) )
				{
					ASSERT( FALSE );
					return false;
				}

				break;
			}
		case WIK_GANG:		//���Ʋ���
			{
				//ɾ���˿�,��������ֻ���ڷŸ�
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard};
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard)) )
				{
					ASSERT( FALSE );
					return false;
				}

				if(m_cbListenStatus[wTargetUser]>0)
				{
					m_GameLogic.RemoveCard(m_cbSelectCard[wTargetUser],m_cbSelectCount[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));
				}
				break;
			}
		default:
			ASSERT( FALSE );
			return false;
		}

		//������
		CMD_S_OperateResult OperateResult;
		ZeroMemory( &OperateResult,sizeof(OperateResult) );
		OperateResult.wOperateUser=wTargetUser;
		OperateResult.cbOperateCode=cbTargetAction;
		OperateResult.wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;
		OperateResult.cbOperateCard[0] = cbTargetCard;
		if( cbTargetAction&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
			CopyMemory( &OperateResult.cbOperateCard[1],&m_cbOperateCard[wTargetUser][1],2*sizeof(BYTE) );
		else if( cbTargetAction&WIK_PENG )
		{
			OperateResult.cbOperateCard[1] = cbTargetCard;
			OperateResult.cbOperateCard[2] = cbTargetCard;
		}

		//�����ж�
		BYTE cbRes = WIK_NULL;
		if (m_cbListenStatus[wTargetUser] == 0)
		{
			cbRes = m_GameLogic.AnalyseTingCard(m_cbCardIndex[wTargetUser], m_WeaveItemArray[wTargetUser], m_cbWeaveItemCount[wTargetUser]);
			if (cbRes == WIK_LISTEN)
			{
				if(SendTingData(wTargetUser, m_cbCardIndex[wTargetUser], m_WeaveItemArray[wTargetUser], m_cbWeaveItemCount[wTargetUser]))
					OperateResult.cbActionMask |= WIK_LISTEN;
			}
		}

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

		if (m_pGameVideo)
		{
			m_pGameVideo->AddVideoData(SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult), true);
		}

		//�û�״̬
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
		ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
		ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

		//�����û�
		m_wCurrentUser=wTargetUser;

		//���ƴ���
		if (cbTargetAction == WIK_GANG)
		{
			m_cbGangStatus = WIK_FANG_GANG;
			CMD_S_GangCard GangCard;
			ZeroMemory(GangCard.cbGangScore, sizeof(GangCard.cbGangScore));
			if (m_cbListenStatus[wTargetUser] > 0)
			{
				CopyMemory(GangCard.cbCardIndex, m_cbCardIndex[wTargetUser], sizeof(GangCard.cbCardIndex));
				if (m_cbSelectCount[wTargetUser] > 0)
				{
					GangCard.cbSelectCount = m_cbSelectCount[wTargetUser];
					CopyMemory(GangCard.cbSelectCard, m_cbSelectCard[wTargetUser], m_cbSelectCount[wTargetUser]);
				}
				GangCard.wChairId = wTargetUser;
			}

			SCORE lTempScore = m_lCellScore * 2 * pow(2, m_cbGangCount);
			lTempScore = __min(lTempScore, m_lUserScore[OperateResult.wProvideUser]);

			GangCard.cbGangScore[wTargetUser] = lTempScore;
			GangCard.cbGangScore[OperateResult.wProvideUser] = -1 * lTempScore;

			m_stRecordInfo.cbMingGang[wTargetUser]++;
			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GANG, &GangCard, sizeof(GangCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GANG, &GangCard, sizeof(GangCard));

			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GANG, &GangCard, sizeof(GangCard), true);
			}

			DispatchCardData(wTargetUser, true);
		}
		else
		{
			m_cbGangCount = 0;
			if (!IsRoomCardScoreType())
				m_pITableFrame->SetGameTimer(IDI_OUT_CARD, (m_pGameCustomRule->cbTimeOutCard + 5) * 1000, 1, 0);
		}

		return true;
	}

	//��������
	if (m_wCurrentUser==wChairID)
	{
		//�˿�Ч��
		ASSERT((cbOperateCode==WIK_NULL)||(cbOperateCode==WIK_CHI_HU)||(m_GameLogic.IsValidCard(cbOperateCard[0])==true));
		if ((cbOperateCode!=WIK_NULL)&&(cbOperateCode!=WIK_CHI_HU)&&(m_GameLogic.IsValidCard(cbOperateCard[0])==false)) 
			return false;

		//���ñ���
		m_bSendStatus=true;
		m_cbUserAction[m_wCurrentUser]=WIK_NULL;
		m_cbPerformAction[m_wCurrentUser]=WIK_NULL;

		//ִ�ж���
		switch (cbOperateCode)
		{
		case WIK_GANG:			//���Ʋ���
			{
				//��������
				BYTE cbWeaveIndex=0xFF;
				BYTE cbCardIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard[0]);

				//���ƴ���
				if (m_cbCardIndex[wChairID][cbCardIndex]==1)
				{
					//Ѱ�����
					for (BYTE i=0;i<m_cbWeaveItemCount[wChairID];i++)
					{
						BYTE cbWeaveKind=m_WeaveItemArray[wChairID][i].cbWeaveKind;
						BYTE cbCenterCard=m_WeaveItemArray[wChairID][i].cbCenterCard;
						if ((cbCenterCard==cbOperateCard[0])&&(cbWeaveKind==WIK_PENG))
						{
							cbWeaveIndex=i;
							break;
						}
					}

					//Ч�鶯��
					ASSERT(cbWeaveIndex!=0xFF);
					if (cbWeaveIndex==0xFF) return false;

					//����˿�
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=TRUE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard[0];
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCardData[3]=cbOperateCard[0];
					m_cbGangStatus=WIK_MING_GANG;

					m_stRecordInfo.cbMingGang[wChairID]++;
				}
				else
				{
					//�˿�Ч��
					ASSERT(m_cbCardIndex[wChairID][cbCardIndex]==4);
					if (m_cbCardIndex[wChairID][cbCardIndex]!=4) 
						return false;

					//���ñ���
					cbWeaveIndex=m_cbWeaveItemCount[wChairID]++;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=FALSE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard[0];
					for( BYTE j = 0; j < 4; j++ ) 
						m_WeaveItemArray[wChairID][cbWeaveIndex].cbCardData[j] = cbOperateCard[0];

					m_cbGangStatus=WIK_AN_GANG;
					m_stRecordInfo.cbAnGang[wChairID]++;

					if(m_cbListenStatus[wChairID]>0 && m_cbSelectCount[wChairID]>0)
					{
						BYTE cbRemoveCard[]={cbOperateCard[0],cbOperateCard[0],cbOperateCard[0]};
						m_GameLogic.RemoveCard(m_cbSelectCard[wChairID],m_cbSelectCount[wChairID],cbRemoveCard,CountArray(cbRemoveCard));
					}
				}

				//������
				CMD_S_OperateResult OperateResult;
				ZeroMemory( &OperateResult,sizeof(OperateResult) );
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.cbOperateCode=cbOperateCode;
				OperateResult.cbOperateCard[0]=cbOperateCard[0];				

				//������Ϣ
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				
				if (m_pGameVideo)
				{
					m_pGameVideo->AddVideoData(SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult), true);
				}
				//ɾ���˿�
				m_cbCardIndex[wChairID][cbCardIndex]=0;

				//�ּܷ���
				CMD_S_GangCard GangCard;
				ZeroMemory(GangCard.cbGangScore,sizeof(GangCard.cbGangScore));
				GangCard.wChairId=wChairID;

				for (BYTE i=0;i<m_cbPlayerCount;i++)
				{
					SCORE lTempScore = m_lCellScore*pow(2.0, m_cbGangCount)*(m_cbGangStatus ==WIK_MING_GANG? 1 : 2);
					if (i!=wChairID)
					{
						lTempScore = __min(lTempScore,m_lUserScore[i]);

						GangCard.cbGangScore[i]=-1*lTempScore;
						GangCard.cbGangScore[wChairID] += lTempScore;
					}
				}

				if (m_cbListenStatus[wChairID]>0)
				{	
					CopyMemory(GangCard.cbCardIndex,m_cbCardIndex[wChairID],sizeof(GangCard.cbCardIndex));
					if(m_cbSelectCount[wChairID]>0)
					{
						GangCard.cbSelectCount=m_cbSelectCount[wChairID];
						CopyMemory(GangCard.cbSelectCard,m_cbSelectCard[wChairID],m_cbSelectCount[wChairID]);
					}
				}					
				//������Ϣ
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GANG,&GangCard,sizeof(GangCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GANG,&GangCard,sizeof(GangCard));

				if (m_pGameVideo)
				{
					m_pGameVideo->AddVideoData(SUB_S_GANG, &GangCard, sizeof(GangCard), true);
				}

				//Ч�鶯��
				bool bAroseAction=false;
				if (m_cbGangStatus==WIK_MING_GANG) 
					bAroseAction=EstimateUserRespond(wChairID,cbOperateCard[0],EstimatKind_GangCard);

				//�����˿�
				if (bAroseAction==false)
				{
					DispatchCardData(wChairID,true);
				}

				return true;
			}
		case WIK_CHI_HU:		//�Ժ�����
			{
				//��ͨ����
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],&m_cbProvideCard,1) )
				{
					ASSERT( FALSE );
					return false;
				}				
				
				m_dwChiHuKind[wChairID] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],cbWeaveItemCount,m_cbProvideCard,m_ChiHuRight[wChairID]);
				m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;

				//������Ϣ
				m_cbChiHuCard=m_cbProvideCard;

				//������Ϸ
				OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);

				return true;
			}
		}

		return true;
	}

	return false;
}

//���Ͳ���
bool CTableFrameSink::SendOperateNotify()
{
	//������ʾ
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		if (m_bPlayStatus[i])
		{
			//��������
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.wResumeUser=m_wProvideUser;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.cbActionMask=m_cbUserAction[i];

			//��������
			m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));

		}
	}

	if (!IsRoomCardScoreType())//��ҳ����ö�ʱ��
	{
		for (WORD i = 0; i < m_cbPlayerCount; i++)
		{
			if (m_bPlayStatus[i] && m_cbUserAction[i] != WIK_NULL)
			{
				CString cc;
				cc.Format(_T("���%d�ɶ���%d"), i, m_cbUserAction[i]);
				WriteDebugInfo(cc);
				m_pITableFrame->SetGameTimer(IDI_OPERATE + i, (m_pGameCustomRule->cbTimeOperate + 5) * 1000, 1, 0);
			}
		}
	}

	for(WORD i=0;i<m_cbPlayerCount;i++)
	{
		if(m_bPlayStatus[i] && m_cbUserAction[i]!=WIK_NULL && m_cbListenStatus[i]>0)
		{
			if ((m_cbUserAction[i] & WIK_CHI_HU) != 0)
			{
				CString ss;
				ss.Format(_T("�������%d������"), i);
				WriteDebugInfo(ss);
				BYTE cbOperateCard[3] = { m_cbProvideCard};
				OnUserOperateCard(i, WIK_CHI_HU, cbOperateCard);
			}
			else if( (m_cbUserAction[i]&WIK_GANG) !=0)
			{
				CString ss;
				ss.Format(_T("�������%d������"), i);
				WriteDebugInfo(ss);
				BYTE cbOperateCard[3]={m_cbProvideCard,m_cbProvideCard,m_cbProvideCard};
				OnUserOperateCard(i,WIK_GANG,cbOperateCard);
			}
		}
	}
	return true;
}

//�ɷ��˿�
bool CTableFrameSink::DispatchCardData(WORD wCurrentUser,bool bTail)
{
	//״̬Ч��
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	if (wCurrentUser==INVALID_CHAIR)
		return false;
	ASSERT( m_bSendStatus );
	if( m_bSendStatus == false ) return false;

	//�����˿�
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData;
	}

	//��ׯ����
	if (m_cbLeftCardCount==m_cbEndLeftCount)
	{
		WriteDebugInfo(_T("��ׯ"));
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);
		return true;
	}

	//���ñ���
	m_cbOutCardData=0;
	m_wCurrentUser=wCurrentUser;
	m_wOutCardUser=INVALID_CHAIR;
	m_wProvideUser=wCurrentUser;

	if (bTail)
		CalGangScore();

	//�����˿�
	m_cbSendCardCount++;
	m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
	m_cbProvideCard=m_cbSendCardData;
	//����
	BYTE cbTempIndex[MAX_INDEX];
	ZeroMemory(cbTempIndex,sizeof(cbTempIndex));
	CopyMemory(cbTempIndex,m_cbCardIndex[wCurrentUser],sizeof(cbTempIndex));
	m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	BYTE cbReMoveCount=0;	
	bool bGang=false;

	BYTE cbWeaveCount=m_cbWeaveItemCount[m_wCurrentUser];
	//�����ж�
	if ((m_bEnjoinChiPeng[m_wCurrentUser]==false)&&(m_cbLeftCardCount>m_cbEndLeftCount))
	{
		if(m_cbListenStatus[m_wCurrentUser]>0)
		{
			for (int j=0;j<m_cbSelectCount[m_wCurrentUser];j++)
			{
				if (m_cbSelectCard[m_wCurrentUser][j]==m_cbProvideCard)
				{
					m_cbUserAction[m_wCurrentUser] |= WIK_GANG;
					break;
				}
			}
		}
		else
		{
			tagGangCardResult GangCardResult;
			m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseGangCardEx(m_cbCardIndex[m_wCurrentUser],m_WeaveItemArray[m_wCurrentUser],cbWeaveCount,m_cbProvideCard,GangCardResult);
		}
	}
	
	if( !m_bEnjoinChiHu[m_wCurrentUser] )
	{
		//�����ж�
		DWORD chr;			

		m_cbCardIndex[m_wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]--;
		m_cbUserAction[m_wCurrentUser] |= m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[m_wCurrentUser],m_WeaveItemArray[m_wCurrentUser],cbWeaveCount,m_cbProvideCard,chr);
		m_cbCardIndex[m_wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;

		//�����ж�
		BYTE cbRes = WIK_NULL;
		if (m_cbListenStatus[wCurrentUser] == 0)
		{
			cbRes = m_GameLogic.AnalyseTingCard(m_cbCardIndex[wCurrentUser], m_WeaveItemArray[wCurrentUser], m_cbWeaveItemCount[wCurrentUser]);
			if (cbRes == WIK_LISTEN)
			{
				if(SendTingData(wCurrentUser, m_cbCardIndex[wCurrentUser], m_WeaveItemArray[wCurrentUser], m_cbWeaveItemCount[wCurrentUser]))
					m_cbUserAction[wCurrentUser] |= WIK_LISTEN;
			}
		}
	}		
	
	//������Ϣ
	ASSERT( m_wHeapHead != INVALID_CHAIR && m_wHeapTail != INVALID_CHAIR );
	if( !bTail )
	{
		//�л�����
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapHead][0]+m_cbHeapCardInfo[m_wHeapHead][1];
		if (cbHeapCount==HEAP_FULL_COUNT) 
			m_wHeapHead=(m_wHeapHead-1+GAME_PLAYER)%CountArray(m_cbHeapCardInfo);
		m_cbHeapCardInfo[m_wHeapHead][0]++;
	}
	else
	{
		//�л�����
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapTail][0]+m_cbHeapCardInfo[m_wHeapTail][1];
		if (cbHeapCount==HEAP_FULL_COUNT) 
			m_wHeapTail=(m_wHeapTail+1)%GAME_PLAYER;
		m_cbHeapCardInfo[m_wHeapTail][1]++;
	}

	//��������
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser= m_wCurrentUser;
	SendCard.bTail = bTail;
	SendCard.cbActionMask=m_cbUserAction[m_wCurrentUser];
	SendCard.cbCardData=m_cbProvideCard;

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	CString cc;
	cc.Format(_T("�����%d����%0x,����=%d,����״̬=%d"), m_wCurrentUser, m_cbProvideCard, m_cbUserAction[m_wCurrentUser],m_cbListenStatus[m_wCurrentUser]);
	WriteDebugInfo(cc);
	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard, sizeof(SendCard), true);
	}

	if(m_cbListenStatus[m_wCurrentUser]>0 || m_bTrustee[m_wCurrentUser])
	{
		if(m_cbListenStatus[m_wCurrentUser])//��������Զ�������
		{
			if( (m_cbUserAction[m_wCurrentUser]&WIK_CHI_HU)!=0)
				return OnUserOperateCard(m_wCurrentUser,WIK_CHI_HU,NULL);
			else if( (m_cbUserAction[m_wCurrentUser]&WIK_GANG) !=0)
			{
				BYTE cbOperateCard[3]={m_cbSendCardData};
				return OnUserOperateCard(m_wCurrentUser,WIK_GANG,cbOperateCard);
			}
		}

		m_pITableFrame->SetGameTimer(IDI_OUT_CARD, 2*1000, 1, 0);
	}
	else if (!IsRoomCardScoreType())
		m_pITableFrame->SetGameTimer(IDI_OUT_CARD, (m_pGameCustomRule->cbTimeOutCard + 5) * 1000, 1, 0);

	return true;
}

//��Ӧ�ж�
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind,bool bSend/*=true*/)
{
	//��������
	bool bAroseAction=false;
	BYTE cbReMoveCount=0;

	//�û�״̬
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//�����ж�
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		//�û�����
		if (wCenterUser==i || m_bTrustee[i] || m_bPlayStatus[i]==false) continue;

		//��������
		if (EstimatKind==EstimatKind_OutCard)
		{
			//�����ж�
			if (m_bEnjoinChiPeng[i]==false)
			{
				if(m_cbListenStatus[i]==0)
					m_cbUserAction[i]|=m_GameLogic.EstimatePengCard(m_cbCardIndex[i],cbCenterCard);

				bool bGang=false;
				//�����ж�
				if (m_cbLeftCardCount>m_cbEndLeftCount) 
				{					
					if(m_cbListenStatus[i]>0)//������Ҽ������Ƿ��ܸ�
					{
						for (int j=0;j<m_cbSelectCount[i];j++)
						{
							if (m_cbSelectCard[i][j]==cbCenterCard)
							{
								m_cbUserAction[i] |= WIK_GANG;
								break;
							}
						}

					}
					else
						m_cbUserAction[i]|=m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
					
				}
			}
		}
		
		//�����ж�
		if (m_bEnjoinChiHu[i]==false)
		{
			//�Ժ��ж�
			DWORD chr;
			BYTE cbWeaveCount=m_cbWeaveItemCount[i];		
			BYTE cbAction = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,chr);
			if(cbAction == WIK_CHI_HU)
			{
				if(m_bGangPao)
					chr |= CHR_GANG_PAO;
				if (m_cbGangStatus==WIK_MING_GANG)
					chr |= CHR_QIANG_GANG;
				if(m_cbListenStatus[i]==0 && m_GameLogic.GetChiHuActionRank(chr)==1 && m_cbListenStatus[wCenterUser]==0)//û�����Ƶ�ʱ������û�����Ƶ���Ҵ��С�����ܺ�
					cbAction=WIK_NULL;
			}
			
			m_cbUserAction[i] |= cbAction;
		}

		//����ж�
		if (m_cbUserAction[i]!=WIK_NULL) 
			bAroseAction=true;
	}

	//�������
	if (bAroseAction==true) 
	{
		//���ñ���
		m_wProvideUser=wCenterUser;
		m_cbProvideCard=cbCenterCard;
		m_wResumeUser=m_wCurrentUser;
		m_wCurrentUser=INVALID_CHAIR;
		
		//������ʾ
		if(bSend)
			SendOperateNotify();

		return true;
	}

	return false;
}

//���
BYTE CTableFrameSink::CalScore( WORD wWinner, SCORE lScore[GAME_PLAYER] )
{
	//��ʼ��
	ZeroMemory( lScore,sizeof(SCORE)*GAME_PLAYER );

	//���Ʒ���
	WORD wFanShuo = m_GameLogic.GetChiHuActionRank( m_ChiHuRight[wWinner] );
	WORD wTempFan=0;
	//���Ƹ���
	ASSERT( wFanShuo > 0 );
	if( wFanShuo == 0 ) return 0;

	if (wWinner==m_wProvideUser)
	{
		for (WORD i=0;i<m_cbPlayerCount;i++)
		{
			if(i!=wWinner)
			{
				if(m_cbListenStatus[i]>0||m_cbListenStatus[wWinner]>0)
					wTempFan=2*wFanShuo;
				else
					wTempFan=wFanShuo;

				wTempFan = min(8,wTempFan);
				lScore[i] -= wTempFan;
				lScore[wWinner] += wTempFan;
			}
		}			
	}
	else
	{
		if (m_cbListenStatus[wWinner]>0||m_cbListenStatus[m_wProvideUser]>0)
		{
			if(m_cbListenStatus[wWinner]>0&&m_cbListenStatus[m_wProvideUser]<=0)
			{
				wTempFan = min(8,4*wFanShuo);
				lScore[wWinner] += wTempFan;		
				lScore[m_wProvideUser] -= wTempFan;
				m_ChiHuRight[wWinner]|=CHR_LIANG_DAO;
			}
			else
			{
				wTempFan = min(8,2*wFanShuo);
				lScore[wWinner] += wTempFan;		
				lScore[m_wProvideUser] -= wTempFan;
				m_ChiHuRight[wWinner]|=CHR_LIANG_DAO;
			}			
		}
		else
		{
			wTempFan = min(8,wFanShuo);
			lScore[wWinner] += wTempFan;		
			lScore[m_wProvideUser] -=wTempFan;
		}
	}	

	return wTempFan;
}

//��ϵͳ���ƻ�ȡ��������
BYTE CTableFrameSink::GetMagicIndex( BYTE cbFanCard )
{
	BYTE cbZPaiIndex = m_GameLogic.SwitchToCardIndex(cbFanCard);
	if( cbZPaiIndex < 27 )
	{
		if( (cbZPaiIndex+1)%9 == 0 )
			cbZPaiIndex -= 8;
		else
			cbZPaiIndex++;
	}
	else
	{
		if( cbZPaiIndex + 1 == MAX_INDEX )
			cbZPaiIndex = 27;
		else
			cbZPaiIndex++;
	}
	return cbZPaiIndex;
}

//��ȡ��������
BYTE CTableFrameSink::GetDiscardCount( BYTE cbCardData )
{
	BYTE cbCount = 0;
	//����������
	for( WORD i = 0; i < m_cbPlayerCount; i++ )
	{
		for( BYTE j = 0; j < m_cbDiscardCount[i]; j++ )
		{
			if( m_cbDiscardCard[i][j] == cbCardData )
				if( ++cbCount == 4 ) break;
		}
	}
	if( cbCount < 4 )
	{
		//������
		for (WORD i = 0; i < m_cbPlayerCount; i++)
		{
			for( BYTE j = 0; j < m_cbWeaveItemCount[i]; j++ )
			{
				if( !m_WeaveItemArray[i][j].cbPublicCard ) continue;
				BYTE cbCardBuffer[4];
				BYTE cbCardCount = m_GameLogic.GetWeaveCard( m_WeaveItemArray[i][j].cbWeaveKind,
					m_WeaveItemArray[i][j].cbCenterCard,cbCardBuffer );
				for( BYTE k = 0; k < cbCardCount; k++ )
					if( cbCardBuffer[k] == cbCardData )
						if( ++cbCount == 4 ) break;
			}
		}
	}

	return cbCount;
}

//Ȩλ����
void CTableFrameSink::FiltrateRight( WORD wWinner, DWORD &chr )
{
	/*	����Ȩλ	*/

	//����
	if( wWinner==m_wProvideUser )
	{
		chr |= CHR_ZI_MO;
		if (m_cbGangStatus != WIK_GANERAL)
			chr |= CHR_GANG_KAI;//���Ͽ���
	}
	else
	{
		if (m_bGangPao)
			chr |= CHR_GANG_PAO;
		else if (m_cbGangStatus != WIK_GANERAL)
			chr |= CHR_QIANG_GANG;
	}

	if (m_cbListenStatus[wWinner]>0)
	{
		chr |= CHR_LIANG_DAO;
	}
	//��������
	if( m_cbLeftCardCount==m_cbEndLeftCount && wWinner != INVALID_CHAIR )
		chr |= CHR_HAI_DI;	

	/*	����Ȩλ	*/	
	//���Ͽ���	��������
	if( !(chr&CHR_GANG_KAI))
		chr &= ~CHR_ZI_MO;
	if( !(chr&CHR_PENG_PENG) )
		chr &= ~CHR_PING_HU;
	if( !(chr&CHR_SHI_GUI_YI_MING) )
		chr &= ~CHR_PING_HU;
	if( !(chr&CHR_SHI_GUI_YI_AN) )
		chr &= ~CHR_PING_HU;
	if( !(chr&CHR_KA_5_XING) )
		chr &= ~CHR_PING_HU;
	if( !(chr&CHR_QI_DUI) )
		chr &= ~CHR_PING_HU;
	if( !(chr&CHR_QING_YI_SE) )
		chr &= ~CHR_PING_HU;
	if( !(chr&CHR_XIAO_SAN_YUAN) )
		chr &= ~CHR_PING_HU;
	if( !(chr&CHR_QUAN_QIU_REN) )
	{
		chr &= ~CHR_PING_HU;
		chr &= ~CHR_PENG_PENG;
	}
	//���߶Ժ�˫���߶� ���ټ��㰵�Ĺ�һ
	if( !(chr&CHR_LONG_QI_DUI) )
	{
		chr &= ~CHR_PING_HU;
		chr &= ~CHR_SHI_GUI_YI_AN;
	}
	if( !(chr&CHR_HAOHUA_LONG_QI) )
	{
		chr &= ~CHR_PING_HU;
		chr &= ~CHR_SHI_GUI_YI_AN;
	}
	if( !(chr&CHR_DA_SAN_YUAN) )
		chr &= ~CHR_PING_HU;
}

void CTableFrameSink::SendSetParam(WORD wChair)
{

}

BYTE CTableFrameSink::GetMaMultiples(WORD wHuUser,BYTE cbHuCard)
{
	if(m_cbMaMode==0 || m_cbLeftCardCount==0)//�������ʣ����Ϊ0
		return 0;
	if(m_cbMaMode==2 && m_cbListenStatus[wHuUser]==0)//������������
		return 0;
	//ASSERT(m_cbMaCount != 0);

	BYTE cbRes=0;
	if(m_cbMaCount !=2)
	{
		if( (m_cbRepertoryCard[0]&0xF0)==0x30)
			cbRes = 10;
		else
			cbRes = (m_cbRepertoryCard[0]&0x0F);

		if(m_cbMaCount==3)//��һ��һ
			cbRes *=2;
	}
	else//��6��
	{
		BYTE cbValue= (cbHuCard&0x0F);
		BYTE cbColor= (cbHuCard&0xF0);
		BYTE cbKey = (cbValue%3);
		if(cbColor == 0x30)
		{
			if(cbValue==5)
				cbKey=1;
			else if(cbValue==6)
				cbKey=2;
			else if(cbValue==7)
				cbKey=0;
		}

		for(int i=0;i<6 && i<m_cbLeftCardCount;i++)
		{
			BYTE cbTempKey=(m_cbRepertoryCard[i]&0x0F)%3;
			BYTE cbTempColor=m_cbRepertoryCard[i]&0xF0;
			if( cbTempKey==cbKey || cbTempColor==0x30)
				cbRes++;
		}

		if(cbColor == 0x30 && cbRes == 0)//���з�����6�벻�У��㱦��6��
			cbRes=6;

	}

	return cbRes;
}

bool CTableFrameSink::bCanOut(WORD wChairID,BYTE cbOutCard)
{
	if(m_cbListenStatus[wChairID]>0)
		return true;

	for(int i=0;i<m_cbPlayerCount;i++)
	{
		if(i == wChairID || m_cbListenStatus[i]==0)
			continue;
		for(int j=0;j<m_cbHuCardCount[i];j++)
		{
			if(m_cbHuCardData[i][j] == cbOutCard)
				return false;
		}
	}

	return true;
}

bool CTableFrameSink::SendTingData(WORD wChairID, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	CMD_S_TING_DATA TingData;
	ZeroMemory(&TingData, sizeof(TingData));
	m_GameLogic.GetTingDataEx(cbCardIndex, WeaveItem, cbWeaveCount, TingData.cbOutCardCount, TingData.cbOutCardData, TingData.cbHuCardCount, TingData.cbHuCardData, TingData.cbHuFan);
	for (int i = 0; i < MAX_COUNT; i++)
	{
		if (TingData.cbHuCardCount[i] > 0)
		{
			for (int j = 0; j < TingData.cbHuCardCount[i]; j++)
			{
				TingData.cbHuCardRemainingCount[i][j] = GetRemainingCount(wChairID, TingData.cbHuCardData[i][j]);
			}
		}
		else
			break;
	}
	ASSERT(TingData.cbOutCardCount > 0);

	BYTE cbOutCardCount = TingData.cbOutCardCount;
	BYTE cbOutCardData[14] = { 0 };
	CopyMemory(cbOutCardData, TingData.cbOutCardData, TingData.cbOutCardCount);
	for (int i = 0; i < cbOutCardCount; i++)
	{
		for (int j = 0; j < m_cbPlayerCount; j++)
		{
			if (j == wChairID || m_cbHuCardCount[j] == 0)
				continue;
			for (int k = 0; k < m_cbHuCardCount[j]; k++)
			{
				if (cbOutCardData[i] == m_cbHuCardData[j][k])
				{
					TingData.cbOutCardData[i] = 0;
					TingData.cbOutCardCount--;
					TingData.cbHuCardCount[i] = 0;
					ZeroMemory(TingData.cbHuCardData[i], sizeof(TingData.cbHuCardData[i]));
					ZeroMemory(TingData.cbHuFan[i], sizeof(TingData.cbHuFan[i]));
					ZeroMemory(TingData.cbHuCardRemainingCount[i], sizeof(TingData.cbHuCardRemainingCount[i]));
					break;
				}
			}
			if (TingData.cbOutCardCount == 0)
				return false;
		}
		if (TingData.cbOutCardCount == 0)
			return false;
	}

	if (TingData.cbOutCardCount == 0)
		return false;

	for (int i = 0; i < cbOutCardCount-1; i++)
	{
		if (TingData.cbOutCardData[i] == 0)
		{
			bool bFlag = false;
			for (int j = i + 1; j < cbOutCardCount; j++)
			{
				if (TingData.cbOutCardData[j] != 0)
				{
					TingData.cbOutCardData[i] = TingData.cbOutCardData[j];
					TingData.cbHuCardCount[i] = TingData.cbHuCardCount[j];
					CopyMemory(TingData.cbHuCardData[i], TingData.cbHuCardData[j], sizeof(TingData.cbHuCardData[j]));
					CopyMemory(TingData.cbHuFan[i], TingData.cbHuFan[j], sizeof(TingData.cbHuCardData[j]));
					CopyMemory(TingData.cbHuCardRemainingCount[i], TingData.cbHuCardRemainingCount[j], sizeof(TingData.cbHuCardData[j]));

					TingData.cbOutCardData[j] = 0;
					TingData.cbHuCardCount[j] = 0;
					ZeroMemory(TingData.cbHuCardData[j], sizeof(TingData.cbHuCardData[j]));
					ZeroMemory(TingData.cbHuFan[j], sizeof(TingData.cbHuFan[j]));
					ZeroMemory(TingData.cbHuCardRemainingCount[j], sizeof(TingData.cbHuCardRemainingCount[j]));
					bFlag = true;
					break;
				}
			}
			if (bFlag==false)
				break;
		}
	}
	//��ȡ�ɸǵ���
	BYTE cbTempCardIndex[MAX_INDEX];
	CopyMemory(cbTempCardIndex, cbCardIndex, sizeof(cbTempCardIndex));
	bool bBreak = false;
	BYTE cbDuiCount = 0;
	DWORD dwAllchr = 0;
	for (BYTE i = 0; i < MAX_INDEX; i++)
	{
		if (cbTempCardIndex[i] % 2 == 0 && cbTempCardIndex[i] != 0) cbDuiCount++;
		if (cbTempCardIndex[i] >= 3)
		{
			cbTempCardIndex[i] -= 3;
			cbTempCardIndex[27] = 3;
			dwAllchr = 0;
			for (BYTE y = 0; y < MAX_INDEX; y++)
			{
				if (cbTempCardIndex[y] == 0) continue;
				cbTempCardIndex[y]--;
				BYTE cbTempHuCount = 0;
				BYTE cbTempHuData[34] = { 0 };
				for (BYTE k = 0; k < MAX_INDEX; k++)
				{
					BYTE cbCurrentCard = m_GameLogic.SwitchToCardData(k);
					DWORD chr;
					if (WIK_CHI_HU == m_GameLogic.AnalyseChiHuCard(cbTempCardIndex, m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], cbCurrentCard, chr))
					{
						dwAllchr |= chr;
						bBreak = true;
						cbTempHuData[cbTempHuCount++] = cbCurrentCard;
					}
				}
				cbTempCardIndex[y]++;
				if (cbTempHuCount > 0)//�����סӰ����ƽ�����ܸ�
				{
					bool bSame = false;
					BYTE cbTempOutCard = m_GameLogic.SwitchToCardData(y);
					for (int a = 0; a < TingData.cbOutCardCount; a++)
					{
						if (TingData.cbOutCardData[a] == cbTempOutCard)
						{
							if (TingData.cbHuCardCount[a] == cbTempHuCount)
							{
								bool bflag = true;
								for (int b = 0; b < TingData.cbHuCardCount[a]; b++)
								{
									if (TingData.cbHuCardData[a][b] != cbTempHuData[b])
									{
										bflag = false;
										break;
									}
								}
								if (bflag)
								{
									bSame = true;
									break;
								}
							}

						}
					}
					if (bSame == false)
					{
						bBreak = false;
						break;
					}
				}
			}

			if (bBreak)
			{
				TingData.cbSelectCardData[TingData.cbSelectCount++] = m_GameLogic.SwitchToCardData(i);
				bBreak = false;
			}
			cbTempCardIndex[i] += 3;
			cbTempCardIndex[27] = 0;
		}
	}

	m_pITableFrame->SendTableData(wChairID, SUB_S_TING_DATA, &TingData, sizeof(TingData));
	return true;
}


BYTE CTableFrameSink::GetRemainingCount(WORD wChairID, BYTE cbCardData)
{
	BYTE cbIndex = m_GameLogic.SwitchToCardIndex(cbCardData);
	BYTE nCount = 0;
	for (int i = 0; i < m_cbLeftCardCount; i++)
	{
		if (m_cbRepertoryCard[i] == cbCardData)
			nCount++;
	}
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (i == wChairID)
			continue;

		nCount += m_cbCardIndex[i][cbIndex];
	}
	return nCount;
}

void CTableFrameSink::CalGangScore()
{
	tagWeaveItem item = m_WeaveItemArray[m_wCurrentUser][m_cbWeaveItemCount[m_wCurrentUser]];
	if (m_cbGangStatus == WIK_FANG_GANG)//�Ÿ�һ�ҿ۷�
	{
		m_lGangScore[item.wProvideUser] -= 2*m_lCellScore*pow(2.0,m_cbGangCount);
		m_lGangScore[m_wCurrentUser] += 2 * m_lCellScore*pow(2.0, m_cbGangCount);
		//��¼���ܴ���
		m_stRecordInfo.cbMingGang[m_wCurrentUser]++;
	}
	else if (m_cbGangStatus == WIK_MING_GANG)//����ÿ�ҳ�1��
	{
		for (int i = 0; i < m_cbPlayerCount; i++)
		{
			if (i != m_wCurrentUser && m_bPlayStatus[i])
			{
				m_lGangScore[i] -= m_lCellScore*pow(2.0, m_cbGangCount);
				m_lGangScore[m_wCurrentUser] += m_lCellScore*pow(2.0, m_cbGangCount);
			}
		}
		//��¼���ܴ���
		m_stRecordInfo.cbMingGang[m_wCurrentUser]++;
	}
	else if (m_cbGangStatus == WIK_AN_GANG)//����ÿ�ҳ�2��
	{
		for (int i = 0; i < m_cbPlayerCount; i++)
		{
			if (i != m_wCurrentUser && m_bPlayStatus[i])
			{
				m_lGangScore[i] -= 2 * m_lCellScore*pow(2.0, m_cbGangCount);
				m_lGangScore[m_wCurrentUser] += 2 * m_lCellScore*pow(2.0, m_cbGangCount);
			}
		}
		//��¼���ܴ���
		m_stRecordInfo.cbAnGang[m_wCurrentUser]++;
	}

	m_cbGangCount++;
}

bool CTableFrameSink::IsRoomCardScoreType()
{
	return (m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

void CTableFrameSink::WriteDebugInfo(LPCTSTR pszString)
{
	//������������
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	//����Ŀ¼
	TCHAR szFilePack[MAX_PATH];
	GetModuleFileName(NULL, szFilePack, MAX_PATH);
	(wcsrchr(szFilePack, '\\'))[0] = 0;
	_stprintf(szFilePack, _T("%s\\��������־Ŀ¼"), szFilePack);
	DWORD dwAttrib = GetFileAttributes(szFilePack);
	if (INVALID_FILE_ATTRIBUTES == dwAttrib)
	{
		CreateDirectory(szFilePack, NULL);
	}

	//�����ļ�
	CTime time = CTime::GetCurrentTime();
	static CString strFile;
	if (strFile.IsEmpty())
		strFile.Format(_T("%s//[%s]��־%d-%d-%d-%02d%02d%02d.log"), szFilePack, m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	//д�ļ�
	CStdioFile myFile;
	BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		if (myFile.GetLength() > 50 * 1024 * 1024)//50M���ļ� 
		{
			myFile.Close();
			strFile.Format(_T("%s//[%s]��־%d-%d-%d-%02d%02d%02d.log"), szFilePack, m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
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
//////////////////////////////////////////////////////////////////////////////////
