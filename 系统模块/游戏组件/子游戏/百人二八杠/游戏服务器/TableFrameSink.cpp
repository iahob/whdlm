#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//��������
#define SEND_COUNT					600									//���ʹ���

//��������
#define INDEX_PLAYER				0									//�м�����
#define INDEX_BANKER				1									//ׯ������

//��עʱ��
#define IDI_FREE					1									//����ʱ��
#ifdef _DEBUG
#define TIME_FREE					6									//����ʱ��
#else
#define TIME_FREE					10									//����ʱ��
#endif

//��עʱ��
#define IDI_PLACE_JETTON			2									//��עʱ��

#ifdef _DEBUG
#define TIME_PLACE_JETTON			5									//��עʱ��
#else
#define TIME_PLACE_JETTON			15									//��עʱ��
#endif

//����ʱ��
#define IDI_GAME_END				3									//����ʱ��

#ifdef _DEBUG
#define TIME_GAME_END				20									//����ʱ��
#else
#define TIME_GAME_END				20									//����ʱ��
#endif

//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;				//��Ϸ����

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//����ע��
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
	
	//������ע
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//���Ʊ���
	m_cbWinSideControl=0;
	m_nSendCardCount=0;

	//��ҳɼ�	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//�˿���Ϣ
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	ZeroMemory(m_cbTableHavaSendCardArray,sizeof(m_cbTableHavaSendCardArray));
	ZeroMemory(m_cbTableHavaSendCount,sizeof(m_cbTableHavaSendCount));


	//״̬����
	m_dwJettonTime=0L;

	//ׯ����Ϣ
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker=INVALID_CHAIR;
	m_wBankerTime=0;
	m_lBankerWinScore=0L;		
	m_lBankerCurGameScore=0L;
	m_bEnableSysBanker=true;
	m_cbLeftCardCount=0;
	m_bContiueCard=true;

	//��¼����
	ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));
	m_nRecordFirst=0;
	m_nRecordLast=0;
	m_dwRecordCount=0;

	//�����ļ���
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	myprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\28GangBattleConfig.ini"),szPath);

	

	//�����˿���
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	m_nProbeTimes = 0;
	m_lStorageStart = 0;
	m_nStorageDeduct = 0;
	m_nDispatchCardTime = 0;
	m_bCanCatchBanker = false;
	m_nBankerReduceCoin=30000;

	//�������
	m_hInst = NULL;
	m_pServerContro = NULL;
	m_hInst = LoadLibrary(TEXT("28GangBattleServerControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}

	//���
	m_StorageWin=0;

	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
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
}

//�ӿڲ�ѯ
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//��Ϸ����
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//��������
	memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));


	//���Ʊ���
	LONGLONG lMaxScore=2147483647;
	m_lApplyBankerCondition=GetPrivateProfileInt(m_szRoomName, TEXT("Score"), 100, m_szConfigFileName);
	m_lAreaLimitScore=GetPrivateProfileInt(m_szRoomName, TEXT("AreaLimitScore"), (int)lMaxScore, m_szConfigFileName);
	m_lUserLimitScore=GetPrivateProfileInt(m_szRoomName, TEXT("UserLimitScore"), (int)lMaxScore, m_szConfigFileName);
	BYTE cbEnableSysBanker=GetPrivateProfileInt(m_szRoomName, TEXT("EnableSysBanker"), 1, m_szConfigFileName);
	m_bEnableSysBanker=cbEnableSysBanker?true:false;

	m_nMaxChipRobot = 10;
	ReadConfigInformation(true);
	return true;
}

//��λ����
VOID  CTableFrameSink::RepositionSink()
{
	//����ע��
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
	
	//������ע
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//��ҳɼ�	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//�����˿���
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	m_nProbeTimes = 0;

	//���
	m_StorageWin=0;

	return;
}

//��Ϸ��ʼ
bool  CTableFrameSink::OnEventGameStart()
{
	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//��ȡׯ��
	IServerUserItem *pIBankerServerUserItem=NULL;
	if (INVALID_CHAIR!=m_wCurrentBanker) 
	{
		pIBankerServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		m_lBankerScore=pIBankerServerUserItem->GetUserScore();
	}

	//���ñ���
	GameStart.cbTimeLeave=m_nPlaceJettonTime;
	GameStart.wBankerUser=m_wCurrentBanker;
	if (pIBankerServerUserItem!=NULL) GameStart.lBankerScore=m_lBankerScore;
	GameStart.bContiueCard=m_bContiueCard;

	//m_wBankerTime

	GameStart.cbLeftCardCount=m_cbLeftCardCount+8;

	for (int i = 0;i<4;i++)
	{	
		CopyMemory(GameStart.cbTableHavaSendCardArray[i],m_cbTableHavaSendCardArray[i],CountArray(m_cbTableHavaSendCardArray[i]));
	}

	CopyMemory(GameStart.cbTableHavaSendCount,m_cbTableHavaSendCount,CountArray(m_cbTableHavaSendCount));

    
	//��ע����������
	int nChipRobotCount = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}

	GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);


    // �������б�����
    nChipRobotCount = 0;
    for (int i = 0; i < m_ApplyUserArray.GetCount(); i++) 
    {
        IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray.GetAt(i));
        if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
            nChipRobotCount++;
    }

    if(nChipRobotCount > 0)
        GameStart.nAndriodCount=nChipRobotCount-1;

	//�����˿���
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	
	
	//�Թ����
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	

	//��Ϸ���
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//���û���
		GameStart.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore);

		m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	
	}

	return true;
}

//��Ϸ����
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������	
		{
			bool bSuccess = false;
			m_nProbeTimes = 0;
			m_StorageWin=0;
			if(m_pServerContro && m_pServerContro->NeedControl())
			{
				//WinnerCard();//��֮��ע��
				//��ȡ����Ŀ������
				m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
				m_pServerContro->CompleteControl();
				ProbeJudge();
			}
			else
			{
				do 
				{
					//�ɷ��˿�
					DispatchTableCard();
					//��̽���ж�
					bSuccess = ProbeJudge();

				} while (!bSuccess);
			}

			//�������
			LONGLONG lBankerWinScore=CalculateScore(false);

			//��������
			m_wBankerTime++;
			m_nDispatchCardTime++;
			if (m_wBankerTime==1)
			{
				m_bContiueCard = true;
			}
			//������Ϣ
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//ׯ����Ϣ
			GameEnd.nBankerTime = m_wBankerTime;
			GameEnd.lBankerTotallScore=m_lBankerWinScore;
			GameEnd.lBankerScore=lBankerWinScore;
			GameEnd.bcFirstCard = m_bcFirstPostCard;
			GameEnd.bcNextCard = m_bcNextPostCard;
			//�˿���Ϣ
			CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));

			GameEnd.cbLeftCardCount=m_cbLeftCardCount+8;
			for (int i = 0;i<4;i++)
			{
				CopyMemory(&m_cbTableHavaSendCardArray[i][m_cbTableHavaSendCount[i]],GameEnd.cbTableCardArray[i],2);		
				m_cbTableHavaSendCount[i]+=2;
				CopyMemory(GameEnd.cbTableHavaSendCardArray[i],m_cbTableHavaSendCardArray[i],CountArray(m_cbTableHavaSendCardArray[i]));
			}
			
			CopyMemory(GameEnd.cbTableHavaSendCount,m_cbTableHavaSendCount,CountArray(m_cbTableHavaSendCount));
			//���ͻ���
			GameEnd.cbTimeLeave=m_nGameEndTime;	
			for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
				if ( pIServerUserItem == NULL ) continue;

				//���óɼ�
				GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

				//��������
				GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

				//����˰��
				if (m_lUserRevenue[wUserIndex]>0) GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
				else if (m_wCurrentBanker!=INVALID_CHAIR) GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
				else GameEnd.lRevenue=0;

				//������Ϣ					
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			return true;
		}
	case GER_USER_LEAVE:		//�û��뿪
	case GER_NETWORK_ERROR:
		{
			
			//�м��ж�
			if (m_wCurrentBanker!=wChairID)
			{
				
				//�Ƿ���
				if (m_pITableFrame->GetGameStatus() == GS_PLACE_JETTON)
				{
					//������ע
					for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) 
					{

						if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
						{
							CMD_S_PlaceJettonFail PlaceJettonFail;
							ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
							PlaceJettonFail.lJettonArea=nAreaIndex;
							PlaceJettonFail.lPlaceScore=m_lUserJettonScore[nAreaIndex][wChairID];
							PlaceJettonFail.wPlaceUser=wChairID;

							//��Ϸ���
							for (WORD i=0; i<GAME_PLAYER; ++i)
							{
								IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
								if (pIServerUserItem==NULL) 
									continue;

								m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
							}

							m_lAllJettonScore[nAreaIndex] -= m_lUserJettonScore[nAreaIndex][wChairID];
							m_lUserJettonScore[nAreaIndex][wChairID] = 0;
						}
					}
				}
				else
				{
					//�޸Ļ���
					tagScoreInfo ScoreInfoArray[GAME_PLAYER];
					ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

					//��������
					LONGLONG lScore=0;
					LONGLONG lRevenue=0;
				


					//�����
					if (!pIServerUserItem->IsAndroidUser())
						m_lStorageStart -= m_lUserWinScore[wChairID];

					ScoreInfoArray[wChairID].lRevenue=m_lUserRevenue[wChairID];
					ScoreInfoArray[wChairID].lScore=m_lUserWinScore[wChairID];;
					ScoreInfoArray[wChairID].cbType=(m_lUserWinScore[wChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;;

					//д�����
					//if (lScore!=0L) m_pITableFrame->WriteUserScore(pIServerUserItem, lScore,lRevenue, ScoreKind);
					m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

					m_lUserRevenue[wChairID]=0;
					m_lUserWinScore[wChairID]=0;

					return true;
				}
				
				
				
				
			}
			

			//״̬�ж�
			if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
			{
				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				myprintf(szTipMsg,CountArray(szTipMsg),TEXT("����ׯ��[ %s ]ǿ�ˣ���Ϸ��ǰ������"),pIServerUserItem->GetNickName());

				//������Ϣ
				SendGameMessage(INVALID_CHAIR,szTipMsg);	

				//����״̬
				m_pITableFrame->SetGameStatus(GS_GAME_END);

				//����ʱ��
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);

				bool bSuccess = false;
				if(m_pServerContro && m_pServerContro->NeedControl())
				{
					//WinnerCard();//��֮��ע��
					//��ȡ����Ŀ������
					m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
					m_pServerContro->CompleteControl();
					ProbeJudge();
				}
				else
				{
					do 
					{
						//�ɷ��˿�
						DispatchTableCard();
						//��̽���ж�
						bSuccess = ProbeJudge();
					} while (!bSuccess);
				}

				//�������
				CalculateScore(false);

				//������Ϣ
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));

				//ׯ����Ϣ
				GameEnd.nBankerTime = m_wBankerTime;
				GameEnd.lBankerTotallScore=m_lBankerWinScore;
				if (m_lBankerWinScore>0) GameEnd.lBankerScore=0;

				//�˿���Ϣ
				CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
				GameEnd.cbLeftCardCount=m_cbLeftCardCount;

				//���ͻ���
				GameEnd.cbTimeLeave=m_nGameEndTime;	
				for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
					if ( pIServerUserItem == NULL ) continue;

					//���óɼ�
					GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

					//��������
					GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

					//����˰��
					if (m_lUserRevenue[wUserIndex]>0) GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
					else if (m_wCurrentBanker!=INVALID_CHAIR) GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
					else GameEnd.lRevenue=0;

					//������Ϣ					
					m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				}
			}

			//�޸Ļ���
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

			ScoreInfoArray[m_wCurrentBanker].lScore=m_lUserWinScore[m_wCurrentBanker];
			ScoreInfoArray[m_wCurrentBanker].cbType=SCORE_TYPE_FLEE;

			//�۳�����
			if (m_lUserWinScore[m_wCurrentBanker]<0)
			{
				//m_pITableFrame->WriteUserScore(m_wCurrentBanker,m_lUserWinScore[m_wCurrentBanker], m_lUserRevenue[m_wCurrentBanker], enScoreKind_Flee);

				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
			}


			//�����
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if (pIServerUserItem != NULL && !pIServerUserItem->IsAndroidUser())
				m_lStorageStart -= m_lUserWinScore[m_wCurrentBanker];

			m_lUserWinScore[m_wCurrentBanker] = 0;

			//�л�ׯ��
			ChangeBanker(true);

			return true;
		}
	}

	return false;
}

//���ͳ���
bool  CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:			//����״̬
		{
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);

			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));			

			//������Ϣ
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			StatusFree.lAreaLimitScore = m_lAreaLimitScore;

			//�˿���Ϣ
			StatusFree.cbLeftCardCount=m_cbLeftCardCount;
			for (int i = 0;i<4;i++)
			{	
				CopyMemory(StatusFree.cbTableHavaSendCardArray[i],m_cbTableHavaSendCardArray[i],CountArray(m_cbTableHavaSendCardArray[i]));
			}

			CopyMemory(StatusFree.cbTableHavaSendCount,m_cbTableHavaSendCount,CountArray(m_cbTableHavaSendCount));

			//ׯ����Ϣ
			StatusFree.bEnableSysBanker=m_bEnableSysBanker;
			StatusFree.wBankerUser=m_wCurrentBanker;	
			StatusFree.cbBankerTime=m_wBankerTime;
			StatusFree.lBankerWinScore=m_lBankerWinScore;
			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusFree.lBankerScore=pIServerUserItem->GetUserScore();
			}

			//�����Ϣ
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusFree.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore); 
			}

			//��������
			CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));

			//ȫ����Ϣ
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(m_nFreeTime-(__min(dwPassTime,m_nFreeTime)));

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			//������ʾ
			TCHAR szTipMsg[128];
			myprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d"),m_lApplyBankerCondition,
				m_lAreaLimitScore,m_lUserLimitScore);
			
			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
						
			//����������
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	case GS_PLACE_JETTON:		//��Ϸ״̬
	case GS_GAME_END:			//����״̬
		{
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);		

			//��������
			CMD_S_StatusPlay StatusPlay={0};

			//ȫ����ע
			CopyMemory(StatusPlay.lAllJettonScore,m_lAllJettonScore,sizeof(StatusPlay.lAllJettonScore));

			//�����ע
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
					StatusPlay.lUserJettonScore[nAreaIndex] = m_lUserJettonScore[nAreaIndex][wChiarID];

				//�����ע
				StatusPlay.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore);
			}

			//������Ϣ
			StatusPlay.lApplyBankerCondition=m_lApplyBankerCondition;		
			StatusPlay.lAreaLimitScore=m_lAreaLimitScore;		

			//ׯ����Ϣ
			StatusPlay.bEnableSysBanker=m_bEnableSysBanker;
			StatusPlay.wBankerUser=m_wCurrentBanker;			
			StatusPlay.cbBankerTime=m_wBankerTime;
			StatusPlay.lBankerWinScore=m_lBankerWinScore;	


			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			for (int i = 0;i<4;i++)
			{
				CopyMemory(StatusPlay.cbTableHavaSendCardArray[i],m_cbTableHavaSendCardArray[i],CountArray(m_cbTableHavaSendCardArray[i]));
			}

			CopyMemory(StatusPlay.cbTableHavaSendCount,m_cbTableHavaSendCount,CountArray(m_cbTableHavaSendCount));


			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusPlay.lBankerScore=pIServerUserItem->GetUserScore();
			}

			//��������
			CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));

			//ȫ����Ϣ
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusPlay.cbTimeLeave=(BYTE)(m_nFreeTime-(__min(dwPassTime,m_nFreeTime)));
			StatusPlay.cbGameStatus=m_pITableFrame->GetGameStatus();			

			//�����ж�
			if (cbGameStatus==GS_GAME_END)
			{
				//���óɼ�
				StatusPlay.lEndUserScore=m_lUserWinScore[wChiarID];

				//��������
				StatusPlay.lEndUserReturnScore=m_lUserReturnScore[wChiarID];

				//����˰��
				if (m_lUserRevenue[wChiarID]>0) StatusPlay.lEndRevenue=m_lUserRevenue[wChiarID];
				else if (m_wCurrentBanker!=INVALID_CHAIR) StatusPlay.lEndRevenue=m_lUserRevenue[m_wCurrentBanker];
				else StatusPlay.lEndRevenue=0;

				//ׯ�ҳɼ�
				StatusPlay.lEndBankerScore=m_lBankerCurGameScore;

				//�˿���Ϣ
				CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			}

			//����������
			SendApplyUser( pIServerUserItem );

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	return false;
}

//��ʱ���¼�
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE:		//����ʱ��
		{
			if (m_wCurrentBanker==INVALID_CHAIR)
			{
				TrySelectBanker();
			}
			//��ʼ��Ϸ
			//m_pITableFrameControl->StartGame();

			m_pITableFrame->StartGame();

			//����ʱ��
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_nPlaceJettonTime*1000,1,0L);

			//����״̬
			m_pITableFrame->SetGameStatus(GS_PLACE_JETTON);

			return true;
		}
	case IDI_PLACE_JETTON:		//��עʱ��
		{
			//״̬�ж�(��ֹǿ���ظ�����)
			if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
			{
				//����״̬
				m_pITableFrame->SetGameStatus(GS_GAME_END);			

				//������Ϸ
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

				//����ʱ��
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);			
			}

			return true;
		}
	case IDI_GAME_END:			//������Ϸ
		{
			
			//�޸Ļ���
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
			
			//д�����
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) continue;

				//ʤ������
				//enScoreKind ScoreKind=(m_lUserWinScore[wUserChairID]>0L)?enScoreKind_Win:enScoreKind_Lost;


				ScoreInfoArray[wUserChairID].lScore=m_lUserWinScore[wUserChairID];
				ScoreInfoArray[wUserChairID].cbType=(m_lUserWinScore[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;;
				ScoreInfoArray[wUserChairID].lRevenue=m_lUserRevenue[wUserChairID];
				//д�����
				//if (m_lUserWinScore[wUserChairID]!=0L) m_pITableFrame->WriteUserScore(wUserChairID,m_lUserWinScore[wUserChairID], m_lUserRevenue[wUserChairID], ScoreKind);

				if (!pIServerUserItem->IsAndroidUser())
					m_lStorageStart -= m_lUserWinScore[wUserChairID];

				//��ׯ�ж�
				LONGLONG lUserScore=pIServerUserItem->GetUserScore();
				if (wUserChairID!=m_wCurrentBanker && lUserScore<m_lApplyBankerCondition)
				{
					for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						if (wUserChairID!=m_ApplyUserArray[i]) continue;

						//ɾ�����
						m_ApplyUserArray.RemoveAt(i);

						//������Ϣ
						CMD_S_CancelBanker CancelBanker;
						ZeroMemory(&CancelBanker,sizeof(CancelBanker));

						//���ñ���
						lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

						//������Ϣ
						m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
						m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

						//��ʾ��Ϣ
						TCHAR szTipMsg[128];
						myprintf(szTipMsg,CountArray(szTipMsg),TEXT("������Ľ����������ׯ����������%I64d��,���Զ���ׯ��"),m_lApplyBankerCondition);
						SendGameMessage(wUserChairID,szTipMsg);

						break;
					}
				}
			}

			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			
			//��ȡ����
			//if (m_bRefreshCfg)
			ReadConfigInformation(true,false);

			//�л�ׯ��
			ChangeBanker(false);

			TrySelectBanker();

			//����ʱ��
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,0L);

			//������Ϣ
			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree,sizeof(GameFree));
			GameFree.cbTimeLeave=m_nFreeTime;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));

			return true;
		}
	}

	return false;
}

//ѡ��ׯ��
void CTableFrameSink::TrySelectBanker()
{
	if (m_wCurrentBanker==INVALID_CHAIR&&m_ApplyUserArray.GetCount()>0)
	{
		////��Ҫ������ׯ
		//if (m_bReCatchBanker)
		//{
		//	//��ʾ��Ϣ
		//	TCHAR szTipMsg[128];
		//	myprintf(szTipMsg,CountArray(szTipMsg),TEXT("������ׯ��"));

		//	//������Ϣ
		//	SendGameMessage(INVALID_CHAIR,szTipMsg);	

		//	//������Ϣ
		//	CMD_S_CatchBanker CatchBanker;
		//	ZeroMemory(&CatchBanker,sizeof(CatchBanker));
		//	CatchBanker.wCatchUser=INVALID_CHAIR;

		//	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));
		//	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));

		//	m_bCanCatchBanker=true;

		//	return;
		//}
		
		m_wCurrentBanker = m_ApplyUserArray[0];

		//�л��ж�
		{
			//���ñ���
			m_wBankerTime = 0;
			m_lBankerWinScore=0;
			m_nDispatchCardTime = 0;
			m_bContiueCard = false;

			//������Ϣ
			CMD_S_ChangeBanker ChangeBanker;
			ZeroMemory(&ChangeBanker,sizeof(ChangeBanker));
			ChangeBanker.wBankerUser=m_wCurrentBanker;
			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				ChangeBanker.lBankerScore=pIServerUserItem->GetUserScore();
			}
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));

			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				//��ȡ��Ϣ
				LONGLONG lMessageCount=GetPrivateProfileInt(m_szRoomName,TEXT("MessageCount"),0,m_szConfigFileName);
				if (lMessageCount!=0)
				{
					//��ȡ����
					LONGLONG lIndex=rand()%lMessageCount;
					TCHAR szKeyName[32],szMessage1[256],szMessage2[256];				
					myprintf(szKeyName,CountArray(szKeyName),TEXT("Item%I64d"),lIndex);
					GetPrivateProfileString(m_szRoomName,szKeyName,TEXT("��ϲ[ %s ]��ׯ"),szMessage1,CountArray(szMessage1),m_szConfigFileName);

					//��ȡ���
					IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

					//������Ϣ
					myprintf(szMessage2,CountArray(szMessage2),szMessage1,pIServerUserItem->GetNickName());
					SendGameMessage(INVALID_CHAIR,szMessage2);
				}
			}
		}

		m_bCanCatchBanker=true;

		//������Ϣ
		CMD_S_CatchBanker CatchBanker;
		ZeroMemory(&CatchBanker,sizeof(CatchBanker));
		CatchBanker.wCatchUser=INVALID_CHAIR;

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));
	}
}

//��Ϸ��Ϣ����
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//�û���ע
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			if (wDataSize!=sizeof(CMD_C_PlaceJetton)) return false;

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//��Ϣ����
			CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pDataBuffer;
			return OnUserPlaceJetton(pUserData->wChairID,pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
		}
	case SUB_C_APPLY_BANKER:		//������ׯ
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;

			return OnUserApplyBanker(pIServerUserItem);	
		}
	case SUB_C_CANCEL_BANKER:		//ȡ����ׯ
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;

			return OnUserCancelBanker(pIServerUserItem);	
		}
	case SUB_C_CATCH_BANKER:        //�û���ׯ
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;

			return OnUserCatchBanker(pIServerUserItem);	
		}
	case SUB_C_SELECT_CARD_MODE:    //����ģʽ
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;
			//��Ϣ����
			CMD_C_SelectCardMode * pMode=(CMD_C_SelectCardMode *)pDataBuffer;

			CMD_S_SelectCardMode  SeletctCardMode;
			SeletctCardMode.cbPutCardMode = pMode->cbPutCardMode;
			SeletctCardMode.cbSendCardMode = pMode->cbSendCardMode;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if ( pIServerUserItem == NULL )
					continue;
				m_pITableFrame->SendTableData(i,SUB_S_SELECT_CARD_MODE,&SeletctCardMode,sizeof(SeletctCardMode));
					
			}

			//��ʾ��Ϣ
			TCHAR szTipMsg[128];
			myprintf(szTipMsg,CountArray(szTipMsg),TEXT("ׯ��[ %s ]���÷���ģʽ��"),pIServerUserItem->GetNickName());

			//������Ϣ
			SendGameMessage(INVALID_CHAIR,szTipMsg);	

			return true;

		}
	case SUB_C_CONTINUE_CARD:		//��������
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;
			if (pUserData->wChairID!=m_wCurrentBanker) return true;
			if (m_cbLeftCardCount < 8) return true;

			if (!m_bContiueCard)
			{
				//���ñ���
				m_bContiueCard=true;

				//������Ϣ
				SendGameMessage(pUserData->wChairID,TEXT("���óɹ�����һ�ֽ��������ƣ�"));

			}
			
			return true;
		}
	case SUB_C_REDISPATCH_CARD:		//����ϴ��
		{

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) return true;
			if (pUserData->wChairID!=m_wCurrentBanker) return true;
			
			//���ñ���
			m_bContiueCard=false;

			//������Ϣ
			SendGameMessage(pUserData->wChairID,TEXT("���óɹ�����һ�ֽ�����ϴ�ƣ�"));
			return true;
		}
	case SUB_C_AMDIN_COMMAND:
		{
			ASSERT(wDataSize==sizeof(CMD_C_AdminReq));
			if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

			if ( m_pServerContro == NULL)
				return false;

			return m_pServerContro->ServerControl(wSubCmdID, pDataBuffer, wDataSize, pIServerUserItem, m_pITableFrame);
		}
	}

	return false;
}

//�����Ϣ����
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�û�����
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//����ʱ��
	if ((bLookonUser==false)&&(m_dwJettonTime==0L))
	{
		m_dwJettonTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
	}

	//������ʾ
	TCHAR szTipMsg[128];
	myprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d"),m_lApplyBankerCondition,
		m_lAreaLimitScore,m_lUserLimitScore);
	m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);

	return true;
}

//�û�����
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��¼�ɼ�
	if (bLookonUser==false)
	{
		//�л�ׯ��
		if (wChairID==m_wCurrentBanker)
		{
			ChangeBanker(true);
		}

		
		//ȡ������
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID!=m_ApplyUserArray[i]) continue;

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//���ñ���
			lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}

		return true;
	}

	return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
	//Ч�����
	ASSERT((cbJettonArea<=ID_JIAO_R && cbJettonArea>=ID_SHANG_MEN)&&(lJettonScore>0L));
	if ((cbJettonArea>ID_JIAO_R)||(lJettonScore<=0L) || cbJettonArea<ID_SHANG_MEN) return false;

	//Ч��״̬
	//ASSERT(m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON);
	if (m_pITableFrame->GetGameStatus()!=GS_PLACE_JETTON)
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea=cbJettonArea;
		PlaceJettonFail.lPlaceScore=lJettonScore;
		PlaceJettonFail.wPlaceUser=wChairID;

		//������Ϣ
		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
		return true;
	}

	//ׯ���ж�
	if (m_wCurrentBanker==wChairID) return true;
	if (m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR) return true;

	//��������
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	LONGLONG lJettonCount=0L;
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];

	//��һ���
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//�Ϸ�У��
	if (lUserScore < lJettonCount + lJettonScore) return true;
	if (m_lUserLimitScore < lJettonCount + lJettonScore) return true;

	//�ɹ���ʶ
	bool bPlaceJettonSuccess=true;

	//�Ϸ���֤
	if (GetUserMaxJetton(wChairID) >= lJettonScore)
	{
		
		//��������֤
		if(pIServerUserItem->IsAndroidUser())
		{
			//long lTemp = lJettonScore;
			//MyDebug(_T(" [������%d ,���:%d,����: %I64d]\r\n"),pIServerUserItem->GetUserID(),GetUserMaxJetton(wChairID),lJettonScore);
			
			//��������
			if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lAreaLimitScore)//m_lRobotAreaLimit
				return true;

			//��Ŀ����
			bool bHaveChip = false;
			for (int i = 0; i < AREA_COUNT; i++)
			{
				if (m_lUserJettonScore[i+1][wChairID] != 0)
					bHaveChip = true;
			}

			if (!bHaveChip && bPlaceJettonSuccess)
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
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;
		}

		
		
		if (bPlaceJettonSuccess)
		{
			//������ע
			m_lAllJettonScore[cbJettonArea] += lJettonScore;
			m_lUserJettonScore[cbJettonArea][wChairID] += lJettonScore;			
		}
	}
	else
	{
		bPlaceJettonSuccess=false;
	}

	if (bPlaceJettonSuccess)
	{
		//��������
		CMD_S_PlaceJetton PlaceJetton;
		ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

		//�������
		PlaceJetton.wChairID=wChairID;
		PlaceJetton.cbJettonArea=cbJettonArea;
		PlaceJetton.lJettonScore=lJettonScore;
		PlaceJetton.bIsAndroid=pIServerUserItem->IsAndroidUser();

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
	}
	else
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea=cbJettonArea;
		PlaceJettonFail.lPlaceScore=lJettonScore;
		PlaceJettonFail.wPlaceUser=wChairID;

		//������Ϣ
		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
	}

	return true;
}

//�����˿�
bool CTableFrameSink::DispatchTableCard()
{
	//��������
	int const static nDispatchCardCount=8;
	bool bContiueCard=m_bContiueCard;
	if (m_cbLeftCardCount<nDispatchCardCount) bContiueCard=false;

	//��������
	if (true==bContiueCard)
	{
		//����ϴ��
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];

		//�����˿�
		CopyMemory(&m_cbTableCardArray[0][0],m_cbTableCard,sizeof(m_cbTableCardArray));
		m_cbLeftCardCount -= nDispatchCardCount;
		if (m_cbLeftCardCount>0) CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
	}
	else
	{
		//����ϴ��
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];
		 for (int i = 0;i<2;i++)
		 {
			 m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));	
		 }
		//�����˿�
		CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));

		m_cbLeftCardCount=CountArray(m_cbTableCard)-nDispatchCardCount;
		CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
				
		for (int i = 0;i<4;i++){ memset(&m_cbTableHavaSendCardArray[i],0,2) ;m_cbTableHavaSendCount[i] = 0; }

	}

	//���Ʊ�־
	//m_bContiueCard=false;
    return true;
}
void CTableFrameSink::RandList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{

		//����׼��
		BYTE *cbCardData = new BYTE[cbBufferCount];
		CopyMemory(cbCardData,cbCardBuffer,cbBufferCount);

		//�����˿�
		BYTE cbRandCount=0,cbPosition=0;
		do
		{
			cbPosition=rand()%(cbBufferCount-cbRandCount);
			cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
			cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
		} while (cbRandCount<cbBufferCount);

		delete []cbCardData;
		cbCardData = NULL;

		return;
}
//����ׯ��
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	//�Ϸ��ж�
	LONGLONG lUserScore=pIApplyServerUserItem->GetUserScore();
	if (lUserScore<m_lApplyBankerCondition)
	{
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("��Ľ�Ҳ���������ׯ�ң�����ʧ�ܣ�"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	//�����ж�
	WORD wApplyUserChairID=pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];
		if (wChairID==wApplyUserChairID)
		{
			m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("���Ѿ�������ׯ�ң�����Ҫ�ٴ����룡"),SMT_CHAT|SMT_EJECT);
			//CMD_S_ApplyBanker ApplyBanker;
			//ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));
			////���ñ���
			//ApplyBanker.wApplyUser=wApplyUserChairID;
			//m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_FAIL, &ApplyBanker, sizeof(ApplyBanker));
			//m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_FAIL, &ApplyBanker, sizeof(ApplyBanker));
			return true;
		}
	}

	//������Ϣ 
	m_ApplyUserArray.Add(wApplyUserChairID);

	//�������
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));

	//���ñ���
	ApplyBanker.wApplyUser=wApplyUserChairID;

	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	//�л��ж�
	if (m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE && m_ApplyUserArray.GetCount()==1)
	{
		ChangeBanker(false);
//#ifdef _DEBUG
//
//		WORD wChairID=m_ApplyUserArray[0];
//		m_wCurrentBanker = wChairID;
//		//������Ϣ
//		CMD_S_ChangeBanker ChangeBanker;
//		ZeroMemory(&ChangeBanker,sizeof(ChangeBanker));
//		ChangeBanker.wBankerUser=m_wCurrentBanker;
//		if (m_wCurrentBanker!=INVALID_CHAIR)
//		{
//			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
//			ChangeBanker.lBankerScore=pIServerUserItem->GetUserScore();
//		}
//		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));
//		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));
//
//#endif
	}

	return true;
}

//ȡ������
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//��ǰׯ��
	if (pICancelServerUserItem->GetChairID()==m_wCurrentBanker && m_pITableFrame->GetGameStatus()!=GAME_STATUS_FREE)
	{
		//������Ϣ
		m_pITableFrame->SendGameMessage(pICancelServerUserItem,TEXT("��Ϸ�Ѿ���ʼ��������ȡ����ׯ��"),SMT_CHAT|SMT_EJECT);

	/*	CMD_S_bCanCancelBanker CanCelBanker;
		CanCelBanker.blCancel = false;
		CanCelBanker.wChariID = m_wCurrentBanker;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_CAN, &CanCelBanker, sizeof(CanCelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_CAN, &CanCelBanker, sizeof(CanCelBanker));*/
		return true;
	}

	//�����ж�
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		//��ȡ���
		WORD wChairID=m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

		//��������
		if (pIServerUserItem==NULL) continue;
		if (pIServerUserItem->GetUserID()!=pICancelServerUserItem->GetUserID()) continue;


		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		if (m_wCurrentBanker!=wChairID)
		{
			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//���ñ���
			lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker==wChairID)
		{
			//�л�ׯ�� 
			m_wCurrentBanker=INVALID_CHAIR;
		}
	
		return true;
	}

	return true;
}

//������ׯ
bool CTableFrameSink::OnUserCatchBanker(IServerUserItem *pIApplyServerUserItem)
{
	
	if (m_pITableFrame->GetGameStatus()!=GAME_STATUS_FREE) 
        return true;



	if (pIApplyServerUserItem->GetUserScore()-m_nBankerReduceCoin<m_lApplyBankerCondition) 
	{
		//��ʾ��Ϣ
		TCHAR szTipMsg[128];
		myprintf(szTipMsg,CountArray(szTipMsg),TEXT("�۳�%I64d��Һ���������ׯ������������ׯʧ�ܣ�"),m_nBankerReduceCoin);
		SendGameMessage(pIApplyServerUserItem->GetChairID(),szTipMsg);	
		return true;

	}

	
	//�����ж�
	WORD wApplyUserChairID=pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];
		if (wChairID==wApplyUserChairID)
		{

			if (m_bCanCatchBanker)
			{
				m_bCanCatchBanker = false;
				//�����б�ĵ�һλ
				m_ApplyUserArray.RemoveAt(nUserIdx);
				m_ApplyUserArray.InsertAt(0,wApplyUserChairID);
				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				myprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ] ��ׯ�ɹ�,ϵͳ�۳����%I64d"),pIApplyServerUserItem->GetNickName(),m_nBankerReduceCoin);

				//������Ϣ
				SendGameMessage(INVALID_CHAIR,szTipMsg);	

				//������Ϣ
				CMD_S_CatchBanker CatchBanker;
				ZeroMemory(&CatchBanker,sizeof(CatchBanker));
				CatchBanker.wCatchUser=wApplyUserChairID;
				
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CATCH_BANKER,&CatchBanker,sizeof(CatchBanker));

				//ʤ������
				//enScoreKind ScoreKind=enScoreKind_Lost;
				LONGLONG lReduceScore=-m_nBankerReduceCoin;

				//�޸Ļ���
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

				ScoreInfoArray[wChairID].lScore=lReduceScore;
				ScoreInfoArray[wChairID].cbType=SCORE_TYPE_SERVICE;

				//д�����
				//m_pITableFrame->WriteUserScore(pIApplyServerUserItem->GetChairID(),lReduceScore, 0, ScoreKind);
				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

				
			}

			return true;
		}
	}

	return true;
}

//����ׯ��
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	//��ׯ����
	WORD wBankerTime=GetPrivateProfileInt(m_szRoomName, TEXT("Time"), 10, m_szConfigFileName);
	WORD wAddTime=GetPrivateProfileInt(m_szRoomName, TEXT("TimeAdd"), 5, m_szConfigFileName);
    LONGLONG lBigScore = 0; 

	bool bChangeBanker =false;

	for(int i = 0;i<m_ApplyUserArray.GetCount();i++)
	{
		    WORD wChairID=m_ApplyUserArray[i];
			if(wChairID==m_wCurrentBanker)
			{
				continue;
			}
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
			if(pIServerUserItem->GetUserScore()>lBigScore)
			{
				lBigScore = pIServerUserItem->GetUserScore();

			}	
	}
	if(m_wCurrentBanker!=INVALID_CHAIR)
	{
		IServerUserItem *pTempIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if(pTempIServerUserItem!=NULL)
		{
			LONGLONG lBankerTempScore=pTempIServerUserItem->GetUserScore();

			if(lBigScore<lBankerTempScore)
			{
				wBankerTime+=wAddTime;
			}
		}
		
	}
	

	//ȡ����ǰ
	if (bCancelCurrentBanker)
	{
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			//��ȡ���
			WORD wChairID=m_ApplyUserArray[i];

			//��������
			if (wChairID!=m_wCurrentBanker) continue;

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		//����ׯ��
		m_wCurrentBanker=INVALID_CHAIR;

		m_wBankerTime = 0;
		m_lBankerWinScore=0;
		m_nDispatchCardTime = 0;
		m_bContiueCard = false;
		bChangeBanker=true;
	}

	//��ׯ�ж�
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		//��ȡׯ��
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

		if(pIServerUserItem!= NULL)
		{
			LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

			//�����ж�
			if (wBankerTime<=m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
			{
				//�������
				for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
				{
					//��ȡ���
					WORD wChairID=m_ApplyUserArray[i];

					//��������
					if (wChairID!=m_wCurrentBanker) continue;

					//ɾ�����
					m_ApplyUserArray.RemoveAt(i);

					break;
				}

				//����ׯ��
				m_wCurrentBanker=INVALID_CHAIR;
				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				if (lBankerScore<m_lApplyBankerCondition)
					myprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��������(%I64d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),m_lApplyBankerCondition);
				else
					myprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��ׯ�����ﵽ(%d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),wBankerTime);

				//������Ϣ
				SendGameMessage(INVALID_CHAIR,szTipMsg);	
				bChangeBanker=true;
			}

		}
		else
		{
			for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
			{
				//��ȡ���
				WORD wChairID=m_ApplyUserArray[i];

				//��������
				if (wChairID!=m_wCurrentBanker) continue;

				//ɾ�����
				m_ApplyUserArray.RemoveAt(i);

				break;
			}
			//����ׯ��
			m_wCurrentBanker=INVALID_CHAIR;
		}
		
	}

	if (bChangeBanker)
	{
		//���ñ���
		m_wBankerTime = 0;
		m_lBankerWinScore=0;

		//������Ϣ
		CMD_S_ChangeBanker ChangeBanker;
		ZeroMemory(&ChangeBanker,sizeof(ChangeBanker));
		ChangeBanker.wBankerUser=m_wCurrentBanker;
		if (m_wCurrentBanker!=INVALID_CHAIR)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			ChangeBanker.lBankerScore=pIServerUserItem->GetUserScore();
		}
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));

	}
	

	return true;
}



//����ׯ��
void CTableFrameSink::SendApplyUser( IServerUserItem *pRcvServerUserItem )
{
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];

		//��ȡ���
		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem) continue;

		//ׯ���ж�
		if (pServerUserItem->GetChairID()==m_wCurrentBanker) continue;

		//�������
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser=wChairID;

		//������Ϣ
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
}

//�û�����
bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) 
{
	//�л�ׯ��
//	if (wChairID==m_wCurrentBanker) ChangeBanker(true);

	//ȡ������
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID!=m_ApplyUserArray[i]) continue;

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		//�������
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker,sizeof(CancelBanker));

		//���ñ���
		lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}

	return true;
}

//�����ע
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID)
{
	//��ȡ���
	IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL==pIMeServerUserItem) return 0L;

	//����ע��
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
	
	//ׯ�ҽ��
	LONGLONG lBankerScore=2147483647;
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		lBankerScore=m_lBankerScore;
	}

	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) 
	{
		lBankerScore-=m_lAllJettonScore[nAreaIndex];
		lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];
	}

	//��������
	LONGLONG lMeMaxScore = min((pIMeServerUserItem->GetUserScore()-lNowJetton), m_lUserLimitScore);

	//��������
	lMeMaxScore=min(lMeMaxScore,m_lAreaLimitScore);

	//ׯ������
	lMeMaxScore=min(lMeMaxScore,lBankerScore);
	
	lMeMaxScore = max(lMeMaxScore, 0);

	return lMeMaxScore;
}

//����÷�
LONGLONG CTableFrameSink::CalculateScore(bool bProbeJudgeScore)
{
	//��������
	float static cbRevenue=m_pGameServiceOption->wRevenueRatio;

	//�ƶ����
	bool static bWinShangMen, bWinTianMen, bWinXiaMen;
	DeduceWinner(bWinShangMen, bWinTianMen, bWinXiaMen);

	if (!bProbeJudgeScore)
	{
		//��Ϸ��¼
		tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
		GameRecord.bWinShangMen=bWinShangMen;
		GameRecord.bWinTianMen=bWinTianMen;
		GameRecord.bWinXiaMen=bWinXiaMen;

		//�ƶ��±�
		m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
		if ( m_nRecordLast == m_nRecordFirst ) m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
	}
	

	//ׯ������
	LONGLONG lBankerWinScore = 0;

	//��ҳɼ�
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//�����ע
	LONGLONG *const pUserScore[] = {NULL, m_lUserJettonScore[ID_SHANG_MEN], m_lUserJettonScore[ID_JIAO_L], m_lUserJettonScore[ID_QIAO],
		m_lUserJettonScore[ID_TIAN_MEN], m_lUserJettonScore[ID_XIA_MEN], m_lUserJettonScore[ID_JIAO_R]};	

	//������
	BYTE static const cbMultiple[] = {0, 1, 1, 1, 1, 1, 1};

	//ʤ����ʶ
	bool static bWinFlag[AREA_COUNT+1];
	bWinFlag[0]=false;
	bWinFlag[ID_SHANG_MEN]=bWinShangMen;
	bWinFlag[ID_JIAO_L]=(true==bWinShangMen && true==bWinTianMen) ? true : false;
	bWinFlag[ID_QIAO]=(true==bWinShangMen && true==bWinXiaMen) ? true : false;
	bWinFlag[ID_TIAN_MEN]=bWinTianMen;
	bWinFlag[ID_XIA_MEN]=bWinXiaMen;
	bWinFlag[ID_JIAO_R]=(true==bWinXiaMen && true==bWinTianMen) ? true : false;

	//�Ǳ�ʶ
	bool static bWinBankerJiaoL,bWinBankerJiaoR,bWinBankerQiao;
	bWinBankerJiaoL=(false==bWinShangMen && false==bWinTianMen) ? true : false;
	bWinBankerJiaoR=(false==bWinXiaMen && false==bWinTianMen) ? true : false;
	bWinBankerQiao=(false==bWinShangMen && false==bWinXiaMen) ? true : false;

	//�������
	for (WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
	{
		//ׯ���ж�
		if (m_wCurrentBanker==wChairID) continue;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		for (WORD wAreaIndex = ID_SHANG_MEN; wAreaIndex <= ID_JIAO_R; ++wAreaIndex)
		{
			//���ж�
			bool bReturnScore=false;
			if (ID_JIAO_L==wAreaIndex && false==bWinBankerJiaoL) bReturnScore=true;
			if (ID_JIAO_R==wAreaIndex && false==bWinBankerJiaoR) bReturnScore=true;
			if (ID_QIAO==wAreaIndex && false==bWinBankerQiao) bReturnScore=true;

			if (true==bWinFlag[wAreaIndex]) 
			{
				m_lUserWinScore[wChairID] += ( pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex] ) ;
				//m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
				lBankerWinScore -= ( pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex] ) ;
			}
			else if (true==bReturnScore)
			{
				m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
			}
			else
			{
				lUserLostScore[wChairID] -= pUserScore[wAreaIndex][wChairID];
				lBankerWinScore += pUserScore[wAreaIndex][wChairID];
			}
		}

		//����˰��
		if (0 < m_lUserWinScore[wChairID])
		{
			float fRevenuePer=float(cbRevenue/1000);
			m_lUserRevenue[wChairID]  = LONGLONG(m_lUserWinScore[wChairID]*fRevenuePer+0.5);
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}
	  //	�ܵķ���
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
	}

	//ׯ�ҳɼ�
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;

		//����˰��
		if (0 < m_lUserWinScore[m_wCurrentBanker])
		{
			float fRevenuePer=float(cbRevenue/1000);
			if(cbRevenue<=0)
			{
				fRevenuePer = 0;
			}
			m_lUserRevenue[m_wCurrentBanker]  = LONGLONG(m_lUserWinScore[m_wCurrentBanker]*fRevenuePer+0.5);
			m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
		}		
	}

	if (!bProbeJudgeScore)
	{
		//�ۼƻ���
		m_lBankerWinScore += lBankerWinScore;

		//��ǰ����
		m_lBankerCurGameScore=lBankerWinScore;	
	}
	

	return lBankerWinScore;
}
void CTableFrameSink::ChangeMaxUser(bool blMin)
{
	int i ;
	BYTE m_cbTableCardArrayTemp[4][2];
	bool  bWinShangMen = 0;
	bool bWinTianMen = 0;
	bool bWinXiaMen = 0;
	CopyMemory(&m_cbTableCardArrayTemp[0][0],m_cbTableCardArray,sizeof(m_cbTableCardArray));
	for (i= 0;i<4;i++)
	{
		  bWinShangMen=m_GameLogic.CompareCard(m_cbTableCardArray[i],2,m_cbTableCardArray[(i+1)%4],2)==1?true:false;
		 bWinTianMen=m_GameLogic.CompareCard(m_cbTableCardArray[i],2,m_cbTableCardArray[(i+2)%4],2)==1?true:false;
		 bWinXiaMen=m_GameLogic.CompareCard(m_cbTableCardArray[i],2,m_cbTableCardArray[(i+3)%4],2)==1?true:false;
		if(bWinShangMen==bWinTianMen&&bWinXiaMen==bWinTianMen &&bWinTianMen==blMin)
		{
			CopyMemory(m_cbTableCardArray[0],m_cbTableCardArrayTemp[i],sizeof(m_cbTableCardArrayTemp[i]));
			CopyMemory(m_cbTableCardArray[1],m_cbTableCardArrayTemp[(i+1)%4],sizeof(m_cbTableCardArrayTemp[(i+1)%4]));
			CopyMemory(m_cbTableCardArray[2],m_cbTableCardArrayTemp[(i+2)%4],sizeof(m_cbTableCardArrayTemp[(i+2)%4]));
			CopyMemory(m_cbTableCardArray[3],m_cbTableCardArrayTemp[(i+3)%4],sizeof(m_cbTableCardArrayTemp[(i+3)%4]));
			break ;
		}
	}
	
}


//��̽���ж�
bool CTableFrameSink::ProbeJudge()
{
	
	//ϵͳ��Ӯ
	LONGLONG lSystemScore = 0l;
	//ׯ���ǲ��ǻ�����
	bool bIsBankerAndroidUser = false;
	IServerUserItem * pIBankerUserItem =(m_wCurrentBanker!=INVALID_CHAIR)?m_pITableFrame->GetTableUserItem(m_wCurrentBanker):NULL;
	if (pIBankerUserItem != NULL) bIsBankerAndroidUser = pIBankerUserItem->IsAndroidUser();

	//ׯ�ҽ��
	LONGLONG lBankerScore = 0;
	if(m_wCurrentBanker != INVALID_CHAIR)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if(pIServerUserItem)
			lBankerScore = pIServerUserItem->GetUserScore();
	}

	//ϵͳ��ׯ�����������ׯ�����ǿ��
	if (m_wCurrentBanker == INVALID_CHAIR||bIsBankerAndroidUser)
	{
		lSystemScore = CalculateScore(true);
		m_nProbeTimes++;

		if (m_nProbeTimes>10)
		{
			//���ʤ������
			BYTE bTempStorage=0;
			for(BYTE i=0;i<4;i++)
			{
				WinnerCardEx();
				if(lSystemScore > CalculateScore(true))
				{
					lSystemScore=CalculateScore(true);
					bTempStorage=m_StorageWin;
				}
				m_StorageWin++;
			}
			m_StorageWin=bTempStorage;
			WinnerCardEx();
			lSystemScore = CalculateScore(true);

			m_nProbeTimes = 0;
			m_lStorageStart += lSystemScore;
			

			if (m_lStorageStart>0)
			{
				m_lStorageStart = m_lStorageStart - (m_lStorageStart * m_nStorageDeduct / 1000);
			}
			else
			{
				m_lStorageStart = m_lStorageStart + (m_lStorageStart * m_nStorageDeduct / 1000);
			}

			return true;
		}
		//ϵͳ��ֵ����
		if ((lSystemScore + m_lStorageStart) < 0l)
		{
			return false;
		}
		else
		{
			m_lStorageStart += lSystemScore;
			
			if (m_lStorageStart>0)
			{
				m_lStorageStart = m_lStorageStart - (m_lStorageStart * m_nStorageDeduct / 1000);
			}
			else
			{
				m_lStorageStart = m_lStorageStart + (m_lStorageStart * m_nStorageDeduct / 1000);
			}
			return true;
		}


	}
	else
	{
		LONGLONG lBankerWinScore=0; 
		lBankerWinScore = CalculateScore(true);
		//ׯ�ҿ۷ּ���
		if(lBankerWinScore < 0L && (lBankerScore+lBankerWinScore) < 0L) return false;

	}
	
	return true;
}

bool CTableFrameSink::OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer)
{

	//��������й���ԱȨ�� �򷵻ش���
	if( CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
	{
		return false;
	}


	return true;
}

void CTableFrameSink::DeduceWinner(bool &bWinShangMen, bool &bWinTianMen, bool &bWinXiaMen)
{
	//��С�Ƚ�
	bWinShangMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHANG_MEN_INDEX],2)==1?true:false;
	bWinTianMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[TIAN_MEN_INDEX],2)==1?true:false;
	bWinXiaMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[XIA_MEN_INDEX],2)==1?true:false;
}

//��������
void CTableFrameSink::WinnerCard()
{
	//BYTE bWinControl=m_pServerContro->GetWinControl();//��֮��ע��
	BYTE bWinControl=1;
	BYTE cbCardDate[8];
	BYTE bLastCard[8];
	ZeroMemory(bLastCard,sizeof(bLastCard));

	//��������
	int const static nDispatchCardCount=8;
	bool bContiueCard=m_bContiueCard;
	if (m_cbLeftCardCount<nDispatchCardCount) bContiueCard=false;

	//��������
	if (true==bContiueCard)
	{
		//����ϴ��
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];

		//�����˿�
		//CopyMemory(&m_cbTableCardArray[0][0],m_cbTableCard,sizeof(m_cbTableCardArray));
		CopyMemory(cbCardDate,m_cbTableCard,sizeof(cbCardDate));
		m_cbLeftCardCount -= nDispatchCardCount;
		if (m_cbLeftCardCount>0) 
			CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
	}
	else
	{
		//����ϴ��
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];
		for (int i = 0;i<2;i++)
		{
			m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));	
		}
		//�����˿�
		//CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));
		CopyMemory(cbCardDate,m_cbTableCard,sizeof(cbCardDate));

		m_cbLeftCardCount=CountArray(m_cbTableCard)-nDispatchCardCount;
		CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);

		for (int i = 0;i<4;i++)
		{ 
			memset(&m_cbTableHavaSendCardArray[i],0,2) ;
			m_cbTableHavaSendCount[i] = 0; 
		}
	}

	//�����һ����
	BYTE bTempCard[2];
	bTempCard[0]=cbCardDate[0];
	bTempCard[1]=cbCardDate[1];
	for(BYTE i=0;i<8;i++)
	{
		for(BYTE j=i+1;j<7;j++)
		{
			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard,bTempCard,sizeof(bTempCard));

	//ɾ����һ����
	bool bDelete[2];
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//����ڶ�����
	ZeroMemory(bTempCard,sizeof(bTempCard));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		if(bTempCard[0] == 0)bTempCard[0]=cbCardDate[i];
		for(BYTE j=i+1;j<7;j++)
		{
			if(cbCardDate[j] == 0)continue;
			if(bTempCard[1] == 0)bTempCard[1]=cbCardDate[j];

			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard+2,bTempCard,sizeof(bTempCard));

	//ɾ���ڶ�����
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//�����������
	ZeroMemory(bTempCard,sizeof(bTempCard));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		if(bTempCard[0] == 0)bTempCard[0]=cbCardDate[i];
		for(BYTE j=i+1;j<7;j++)
		{
			if(cbCardDate[j] == 0)continue;
			if(bTempCard[1] == 0)bTempCard[1]=cbCardDate[j];

			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard+4,bTempCard,sizeof(bTempCard));

	//ɾ����������
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//��������
	BYTE bCardNum=6;
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		bLastCard[bCardNum++]=cbCardDate[i];
	}

//#define BANKER_INDEX				0									//ׯ������
//#define SHANG_MEN_INDEX				1									//��������
//#define TIAN_MEN_INDEX				2									//��������
//#define XIA_MEN_INDEX				3									//��������
	//��������
	switch(bWinControl)
	{
	case 0:
		{
			//����
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 1:
		{
			//���ź�����
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 2:
		{
			//����
			CopyMemory(&m_cbTableCardArray[2], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);

			break;
		}
	case 3:
		{
			//���ź�����
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 4:
		{
			//����
			CopyMemory(&m_cbTableCardArray[3], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 5:
		{
			//���ź�����
			CopyMemory(&m_cbTableCardArray[2], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 6:
		{
			//ׯ��
			CopyMemory(&m_cbTableCardArray[0], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	}

	//���ƴ���
	//m_pServerContro->SetExcuteTimes();//��֮��ע��

	//��֤
	for(BYTE i=0;i<4;i++)
	{
		for(BYTE j=0;j<2;j++)
		{
			if(m_cbTableCardArray[i][j] == 0)
			{
				ASSERT(FALSE);
				return;
			}
		}
	}
}
//��������
void CTableFrameSink::WinnerCardEx()
{
	BYTE bWinControl=m_StorageWin;
	BYTE cbCardDate[8];
	BYTE bLastCard[8];
	ZeroMemory(bLastCard,sizeof(bLastCard));

	//��������
	int const static nDispatchCardCount=8;
	bool bContiueCard=m_bContiueCard;
	if (m_cbLeftCardCount<nDispatchCardCount) bContiueCard=false;

	//��������
	if (true==bContiueCard)
	{
		//����ϴ��
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];

		//�����˿�
		//CopyMemory(&m_cbTableCardArray[0][0],m_cbTableCard,sizeof(m_cbTableCardArray));
		CopyMemory(cbCardDate,m_cbTableCard,sizeof(cbCardDate));
		m_cbLeftCardCount -= nDispatchCardCount;
		if (m_cbLeftCardCount>0) 
			CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
	}
	else
	{
		//����ϴ��
		//BYTE bcCard[CARD_COUNT];
		//m_GameLogic.RandCardList(bcCard,CARD_COUNT);
		BYTE Sizi[]={1,2,3,4,5,6,1,2,3,4,5,6};
		RandCount(Sizi,12);
		m_bcFirstPostCard = Sizi[0];
		m_bcNextPostCard = Sizi[1];
		for (int i = 0;i<2;i++)
		{
			m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));	
		}
		//�����˿�
		//CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));
		CopyMemory(cbCardDate,m_cbTableCard,sizeof(cbCardDate));

		m_cbLeftCardCount=CountArray(m_cbTableCard)-nDispatchCardCount;
		CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);

		for (int i = 0;i<4;i++)
		{ 
			memset(&m_cbTableHavaSendCardArray[i],0,2) ;
			m_cbTableHavaSendCount[i] = 0; 
		}
	}

	//�����һ����
	BYTE bTempCard[2];
	bTempCard[0]=cbCardDate[0];
	bTempCard[1]=cbCardDate[1];
	for(BYTE i=0;i<8;i++)
	{
		for(BYTE j=i+1;j<7;j++)
		{
			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard,bTempCard,sizeof(bTempCard));

	//ɾ����һ����
	bool bDelete[2];
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//����ڶ�����
	ZeroMemory(bTempCard,sizeof(bTempCard));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		if(bTempCard[0] == 0)bTempCard[0]=cbCardDate[i];
		for(BYTE j=i+1;j<7;j++)
		{
			if(cbCardDate[j] == 0)continue;
			if(bTempCard[1] == 0)bTempCard[1]=cbCardDate[j];

			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard+2,bTempCard,sizeof(bTempCard));

	//ɾ���ڶ�����
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//�����������
	ZeroMemory(bTempCard,sizeof(bTempCard));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		if(bTempCard[0] == 0)bTempCard[0]=cbCardDate[i];
		for(BYTE j=i+1;j<7;j++)
		{
			if(cbCardDate[j] == 0)continue;
			if(bTempCard[1] == 0)bTempCard[1]=cbCardDate[j];

			BYTE bCard[2];
			bCard[0]=cbCardDate[i];
			bCard[1]=cbCardDate[j];
			if(m_GameLogic.CompareCard(bTempCard,2,bCard,2) == 1)
			{
				CopyMemory(bTempCard,bCard,sizeof(bCard));
			}
		}
	}
	CopyMemory(bLastCard+4,bTempCard,sizeof(bTempCard));

	//ɾ����������
	memset(bDelete,true,sizeof(bDelete));
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == bTempCard[0] && bDelete[0] == true)
		{
			cbCardDate[i]=0;
			bDelete[0]=false;
		}
		else if(cbCardDate[i] == bTempCard[1] && bDelete[1] == true)
		{
			cbCardDate[i]=0;
			bDelete[1]=false;
		}
	}

	//��������
	BYTE bCardNum=6;
	for(BYTE i=0;i<8;i++)
	{
		if(cbCardDate[i] == 0)continue;
		bLastCard[bCardNum++]=cbCardDate[i];
	}

	//#define BANKER_INDEX				0									//ׯ������
	//#define SHANG_MEN_INDEX				1									//��������
	//#define TIAN_MEN_INDEX				2									//��������
	//#define XIA_MEN_INDEX				3									//��������
	//��������
	switch(bWinControl)
	{
	case 0:
		{
			//����
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 1:
		{
			//���ź�����
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 2:
		{
			//����
			CopyMemory(&m_cbTableCardArray[2], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);

			break;
		}
	case 3:
		{
			//���ź�����
			CopyMemory(&m_cbTableCardArray[1], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 4:
		{
			//����
			CopyMemory(&m_cbTableCardArray[3], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 5:
		{
			//���ź�����
			CopyMemory(&m_cbTableCardArray[2], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[0], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	case 6:
		{
			//ׯ��
			CopyMemory(&m_cbTableCardArray[0], bLastCard, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[1], bLastCard+2, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[2], bLastCard+4, sizeof(BYTE)*2);
			CopyMemory(&m_cbTableCardArray[3], bLastCard+6, sizeof(BYTE)*2);
			break;
		}
	}

	//���ƴ���
	//m_pServerContro->SetExcuteTimes();//��֮��ע��

	//��֤
	for(BYTE i=0;i<4;i++)
	{
		for(BYTE j=0;j<2;j++)
		{
			if(m_cbTableCardArray[i][j] == 0)
			{
				ASSERT(FALSE);
				return;
			}
		}
	}
}
//���ͼ�¼
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{
	WORD wBufferSize=0;
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	int nIndex = m_nRecordFirst;
	while ( nIndex != m_nRecordLast )
	{
		if ((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
		{
			m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
			wBufferSize=0;
		}
		CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
		wBufferSize+=sizeof(tagServerGameRecord);

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
	}
	if (wBufferSize>0) m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
}

//������Ϣ
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID==INVALID_CHAIR)
	{
		//��Ϸ���
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT);
		}

		//�Թ����
		WORD wIndex=0;
		do {
			IServerUserItem *pILookonServerUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pILookonServerUserItem==NULL) break;

			m_pITableFrame->SendGameMessage(pILookonServerUserItem,pszTipMsg,SMT_CHAT);

		}while(true);
	}
	else
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem!=NULL) m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT|SMT_EJECT);
	}
}

#ifdef __BANKER___
bool  CTableFrameSink::OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}
#endif


//��ȡ����
void CTableFrameSink::ReadConfigInformation(bool bReadFresh,bool bReadBaseConfig)
{
	//��������
	TCHAR OutBuf[255] = {};

	//ÿ��ˢ��
	if (bReadFresh)
	{
		//ÿ��ˢ��
		BYTE cbRefreshCfg = GetPrivateProfileInt(m_szRoomName, TEXT("Refresh"), 0, m_szConfigFileName);
		m_bRefreshCfg = cbRefreshCfg?true:false;
	}
	//��ׯ����
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("score"), _T("1000"), OutBuf, 255, m_szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lApplyBankerCondition);

	//��ׯ����
	m_nBankerTimeLimit = GetPrivateProfileInt(m_szRoomName, TEXT("Time"), 10, m_szConfigFileName);
	m_nBankerTimeAdd = GetPrivateProfileInt(m_szRoomName, TEXT("TimeAdd"), 10, m_szConfigFileName);

	//��������
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("BankerScore"), _T("9000000000"), OutBuf, 255, m_szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lExtraBankerScore);

	m_nExtraBankerTime = GetPrivateProfileInt(m_szRoomName, TEXT("BankerAdd"), 10, m_szConfigFileName);

	//��ע����
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("AreaLimitScore"), _T("2147483647"), OutBuf, 255, m_szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lAreaLimitScore);

	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("UserLimitScore"), _T("2147483647"), OutBuf, 255, m_szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lUserLimitScore);

	//��ׯ�۳����
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("CatchBankerReduceCoin"), _T("30000"), OutBuf, 255, m_szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_nBankerReduceCoin);
	if (m_nBankerReduceCoin<0) m_nBankerReduceCoin=30000;


	if(bReadBaseConfig)
	{
		
		BYTE cbEnableSysBanker=GetPrivateProfileInt(m_szRoomName, TEXT("EnableSysBanker"), 1, m_szConfigFileName);
		m_bEnableSysBanker=cbEnableSysBanker?true:false;
		
		//ʱ������
		m_nFreeTime = GetPrivateProfileInt(m_szRoomName, TEXT("FreeTime"), 5, m_szConfigFileName);
		m_nPlaceJettonTime = GetPrivateProfileInt(m_szRoomName, TEXT("BetTime"), 15, m_szConfigFileName);
		m_nGameEndTime = GetPrivateProfileInt(m_szRoomName, TEXT("EndTime"), 15, m_szConfigFileName);
		if (m_nFreeTime <= 5		|| m_nFreeTime > 99)			m_nFreeTime = 5;
		if (m_nPlaceJettonTime <= 15 || m_nPlaceJettonTime > 99)		m_nPlaceJettonTime = 15;
		if (m_nGameEndTime <= 15		|| m_nGameEndTime > 99)			m_nGameEndTime = 20;

		//˰�տ���
		m_nStorageDeduct = GetPrivateProfileInt(m_szRoomName, TEXT("StorageDeduct"), 1, m_szConfigFileName);
		m_lStorageStart = GetPrivateProfileInt(m_szRoomName, TEXT("StorageStart"), 0, m_szConfigFileName);
		if (m_lStorageStart <= 0)			m_lStorageStart = 0;
		if (m_nStorageDeduct <= 0		|| m_nStorageDeduct > 50)			m_nStorageDeduct = 1;
	}

#ifdef _DEBUG
	m_nPlaceJettonTime = 15;
#endif

}

//��ѯ�����
bool  CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pUserItem==NULL) continue;
		if(wChairID==i)
		{
			//������ע
			for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) 
			{

				if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
				{
					return true;
				}
			}
			break;
		}
	}
	return false;
}
//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	SCORE consumeQuota=0L;
	SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	if(m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE)
	{
		consumeQuota=pIServerUserItem->GetUserScore()-lMinTableScore;

	}
	return consumeQuota;
}
//////////////////////////////////////////////////////////////////////////
