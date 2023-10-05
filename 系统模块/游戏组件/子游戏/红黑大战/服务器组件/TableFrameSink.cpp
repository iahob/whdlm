#include "StdAfx.h"
#include "DlgCustomRule.h"
#include "TableFrameSink.h"
#include <locale>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////
//��������
#define SEND_COUNT					100									//���ʹ���

//��������
#define INDEX_PLAYER				0									//�м�����
#define INDEX_BANKER				1									//ׯ������

//ʱ�䶨��
#define IDI_FREE					1									//����ʱ��
#define IDI_PLACE_JETTON			2									//��עʱ��
#define IDI_GAME_END				3									//����ʱ��

#define TIME_FREE					5									//����ʱ��
#define TIME_PLACE_JETTON			10									//��עʱ��
#define TIME_GAME_END				20									//����ʱ��
//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;				//��Ϸ����

//////////////////////////////////////////////////////////////////////////
//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��ʼ����
	ZeroMemory(m_lUserStartScore,sizeof(m_lUserStartScore));

	//����ע��
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//������ע
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//�Ͼ���Ϣ
	ZeroMemory(m_lLastAllJettonPlace, sizeof(m_lLastAllJettonPlace));
	ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

	//���Ʊ���
	m_cbWinSideControl=0;
	m_nSendCardCount=0;
	m_bControl=false;

	m_bWinRed = false;
	m_bWinBlack = false;
	m_bWinPoker = false;
	ZeroMemory(m_cbPokerType, sizeof(m_cbPokerType));
	//��ҳɼ�	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//�˿���Ϣ
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//״̬����
	m_dwJettonTime=0L;
	m_bExchangeBanker=true;
	m_wAddTime=0;
	CopyMemory(m_szControlName,TEXT("����"),sizeof(m_szControlName));

	//ׯ����Ϣ
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker=INVALID_CHAIR;
	m_wOfflineBanker = INVALID_CHAIR;
	m_wBankerTime=0;
	m_lPlayerBankerMAX = 0l;
	m_lBankerWinScore=0L;		
	m_lBankerCurGameScore=0L;
	m_bEnableSysBanker=true;
	m_lBankerScore = 0l;

	//��¼����
	ZeroMemory(m_cbRBHistory, sizeof(m_cbRBHistory));
	ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));
	m_nRecordFirst=0;
	m_nRecordLast=0;
	m_dwRecordCount=0;

	m_nRBCurrentColumn = 0;
	m_nRBCurrentRow = 0;

	//�����˿���
	m_nChipRobotCount = 0;
	m_nRobotListMaxCount =0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	ZeroMemory(&m_occupyseatConfig, sizeof(m_occupyseatConfig));

	//ռλ
	for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
	{
		m_cbDeskChairID[i] = INVALID_BYTE;
	}

	//�������
	m_hInst = NULL;
	m_pServerContro = NULL;
	m_hInst = LoadLibrary(TEXT("RedBlackBattleServerDebug.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}

	return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
}

VOID CTableFrameSink::Release()
{
	if (m_pServerContro)
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

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();

	ASSERT(m_pGameServiceOption!=NULL);
	ASSERT(m_pGameServiceAttrib!=NULL);
	//��������
	memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));

	ReadConfigInformation();

	return true;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	m_wOfflineBanker = INVALID_CHAIR;

	//����ע��
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//������ע
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//�������һע
	ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

	m_bWinRed = false;
	m_bWinBlack = false;
	m_bWinPoker = false;
	ZeroMemory(m_cbPokerType, sizeof(m_cbPokerType));
	//��ҳɼ�	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//�����˿���
	m_nChipRobotCount = 0;
	m_bControl=false;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	
	m_lBankerScore = 0l;

	return;
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
		return __max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore, 0L);
	}
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//��ׯ�ҷ����ڿ���ʱ��䶯ʱ(��ׯ�ҽ����˴�ȡ��)У��ׯ�ҵ���ׯ����
	if(wChairID == m_wCurrentBanker && m_pITableFrame->GetGameStatus()==GAME_SCENE_FREE)
	{
		ChangeBanker(false);
	}
	
	//У���Ƿ�����ռλ�������
	if (pIServerUserItem->GetUserScore() < m_occupyseatConfig.lForceStandUpCondition)
	{
		bool bvalid = false;
		//У������

		WORD wDeskChairID = INVALID_BYTE;
		for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
		{
			if (m_cbDeskChairID[i] == wChairID)
			{
				bvalid = true;
				wDeskChairID = i;
				//������Ч
				m_cbDeskChairID[i] = INVALID_BYTE;

				break;
			}
		}

		if (bvalid && wDeskChairID != INVALID_BYTE)
		{
			CMD_S_GameDown GameDown;
			ZeroMemory(&GameDown, sizeof(GameDown));
			GameDown.wChairID = wChairID;
			GameDown.wDeskChairID = wDeskChairID + 1;

			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
		}

	}

	return true;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//�����������
	srand(GetTickCount());

	//���Ϳ����Ϣ
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if ( pIServerUserItem == NULL )
			continue;

		if( CUserRight::IsGameDebugUser(m_pITableFrame->GetTableUserItem(i)->GetUserRight()))
		{
			CString strInfo;
			strInfo.Format(TEXT("��ǰ��棺%I64d"), m_lStorageCurrent);

			m_pITableFrame->SendGameMessage(pIServerUserItem,strInfo,SMT_CHAT);
		}	
	}
	
	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//��ȡׯ��
	IServerUserItem* pIBankerServerUserItem = NULL;
	if ( m_wCurrentBanker == INVALID_CHAIR )
	{
		m_lBankerScore = 1000000000;
	}
	else
	{
		IServerUserItem* pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if ( pIBankerServerUserItem != NULL )
		{
			m_lBankerScore = pIBankerServerUserItem->GetUserScore();
		}
	}

	//���ñ���
	GameStart.cbTimeLeave=m_nPlaceJettonTime;
	GameStart.wBankerUser=m_wCurrentBanker;
	GameStart.lBankerScore=m_lBankerScore;
	GameStart.lStorageStart = m_lStorageCurrent;	

	//��ע����������
	int nChipRobotCount = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}

	GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);
	
	nChipRobotCount = 0;
	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++) 
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray.GetAt(i));
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}

	if(nChipRobotCount > 0)
		GameStart.nAndriodApplyCount=nChipRobotCount-1;

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
		GameStart.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*MAX_RATE);

		m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	
	}

	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������	
		{
			//��Ϸ����
			if( m_pServerContro != NULL && m_pServerContro->NeedControl() && m_pServerContro->ControlResult(m_cbTableCardArray))
			{
				//��ȡ����Ŀ������
				m_bControl=true;
				m_pServerContro->CompleteControl();
			}
			else
			{	
				
				//ϵͳ��ׯ�ͻ�������ׯ�Ż����
				if ((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
				 || (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true))
				{
					StorageOptimize();
				}	
				else if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false)
				{
					//ѭ������
					LONGLONG lCirculateCount = 0L;
					while(true)
					{
						if (lCirculateCount > 10000)
						{
							break;
						}

						//������ҵ÷�
						LONGLONG lRealPlayerWinScore = CalRealPlayerBankerScore();

						//���������Ǯ
						if (lRealPlayerWinScore <= 0)
						{
							break;
						}
						else		//�������ӮǮ
						{
							if (m_lStorageCurrent - lRealPlayerWinScore  <= 0)
							{
								DispatchTableCard();
								lCirculateCount++;
								continue;
							}

							//��������⸶����
							LONGLONG lStorageCurrent = m_lStorageCurrent - lRealPlayerWinScore;
							double llCompensateRadio = (double)((double)30 / (double)100);
							if (m_lStorageCurrent * (1 - llCompensateRadio) >= lStorageCurrent)
							{
								DispatchTableCard();
								lCirculateCount++;
								continue;
							}

							//�ж�����
							if (((m_lStorageCurrent > m_lStorageMax1) && (rand()%100 < m_lStorageMul1))
								|| ((m_lStorageCurrent > m_lStorageMax2) && (rand()%100 < m_lStorageMul2))
								|| rand() % 100 < 45)
							{
								break;
							}
							else
							{
								DispatchTableCard();
								lCirculateCount++;
								continue;
							}
						}
					}
					
				}
			}

			//�����Ͼ���ע
			CopyMemory(m_lLastAllJettonPlace, m_lUserJettonScore, sizeof(m_lLastAllJettonPlace));
			//��ձ������һע
			ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

			//�������
			LONGLONG lBankerWinScore=CalculateScore();

			//��������
			m_wBankerTime++;

			//������Ϣ
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//ռλ��ҳɼ�
			for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
			{
				if (m_cbDeskChairID[i] == INVALID_BYTE) continue;
				IServerUserItem *pIOccupySeatServerUserItem = m_pITableFrame->GetTableUserItem(m_cbDeskChairID[i]);
				if (!pIOccupySeatServerUserItem) continue;

				GameEnd.lDeskScore[i] = m_lUserWinScore[m_cbDeskChairID[i]] - m_lUserRevenue[m_cbDeskChairID[i]];
				GameEnd.lDeskScoreT[i] = m_lUserWinScore[m_cbDeskChairID[i]];
			}

			GameEnd.bWinRed = m_bWinRed;
			GameEnd.bWinBlack = m_bWinBlack;
			GameEnd.bWinPoker = m_bWinPoker;
			GameEnd.wCurrentBanker = m_wCurrentBanker;
			//ׯ����Ϣ
			GameEnd.nBankerTime = m_wBankerTime;
			GameEnd.lBankerTotallScore=m_lBankerWinScore;
			GameEnd.lBankerScore=lBankerWinScore;

			//�˿���Ϣ
			CopyMemory(GameEnd.cbPokerType, m_cbPokerType, sizeof(m_cbPokerType));
			CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));

			//���ͻ���
			GameEnd.cbTimeLeave=m_nGameEndTime;	
			for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
				if ( pIServerUserItem == NULL ) continue;

				//���óɼ� ��ȡ��˰��
				GameEnd.lUserScore=m_lUserWinScore[wUserIndex] - m_lUserRevenue[wUserIndex];
				//ԭʼ�÷� û����ȡ˰��
				GameEnd.lUserScoreT=m_lUserWinScore[wUserIndex];
				//��������
				GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

				//������Ϣ					
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}
			return true;
		}
	case GER_USER_LEAVE:		//�û��뿪
	case GER_NETWORK_ERROR:
		{
			//ֻҪ�뿪��������û���¼
			m_tagUserRecord[wChairID].RemoveAll();
			//�м��ж�
			if (m_wCurrentBanker!=wChairID)
			{
				//��������
				LONGLONG lRevenue=0;

				//�Ƿ���
				if (m_pITableFrame->GetGameStatus() == GAME_SCENE_PLACE_JETTON)
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
					//д�����
					if (m_lUserWinScore[wChairID]!=0L) 
					{
						tagScoreInfo ScoreInfo[GAME_PLAYER];
						ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
						//�����
						if (!pIServerUserItem->IsAndroidUser())
							m_lStorageCurrent -= (m_lUserWinScore[wChairID]);						
						ScoreInfo[wChairID].lScore = m_lUserWinScore[wChairID];
						ScoreInfo[wChairID].cbType=m_lUserWinScore[wChairID]>0?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
						ScoreInfo[wChairID].lRevenue = m_lUserRevenue[wChairID];
						m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
						m_lUserWinScore[wChairID] = 0;
					}
					//�����ע
					for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
					{
						m_lUserJettonScore[nAreaIndex][wChairID] = 0;
					}
				}

				return true;
			}

			//״̬�ж�
			if (m_pITableFrame->GetGameStatus()!= GAME_SCENE_GAME_END)
			{
				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("����ׯ��[ %s ]ǿ�ˣ���Ϸ��ǰ������"),pIServerUserItem->GetNickName());

				//������Ϣ
				SendGameMessage(INVALID_CHAIR,szTipMsg);	

				//����״̬
				m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

				//����ʱ��
				m_dwJettonTime=(DWORD)time(NULL);
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);

				//��Ϸ����
				m_bControl = FALSE;
				if (m_pServerContro != NULL && m_pServerContro->NeedControl() && m_pServerContro->ControlResult(m_cbTableCardArray))
				{
					//��ȡ����Ŀ������
					m_bControl = TRUE;
					m_pServerContro->CompleteControl();
				}

				//�������
				CalculateScore();

				//������Ϣ
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));

				//ռλ��ҳɼ�
				for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
				{
					if (m_cbDeskChairID[i] == INVALID_BYTE)
					{
						continue;
					}
					IServerUserItem *pIOccupySeatServerUserItem = m_pITableFrame->GetTableUserItem(m_cbDeskChairID[i]);
					if (!pIOccupySeatServerUserItem)
					{
						continue;
					}

					GameEnd.lDeskScoreT[i] = m_lUserWinScore[m_cbDeskChairID[i]];
				}

				//ׯ����Ϣ
				GameEnd.nBankerTime = m_wBankerTime;
				GameEnd.lBankerTotallScore=m_lBankerWinScore;
				GameEnd.lBankerScore=m_lUserWinScore[m_wCurrentBanker];

				//�˿���Ϣ
				CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));

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
					//if (m_lUserRevenue[wUserIndex]>0) GameEnd.L=m_lUserRevenue[wUserIndex];
					//else if (m_wCurrentBanker!=INVALID_CHAIR) GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
					//else GameEnd.lRevenue=0;

					//������Ϣ					
					m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				}
			}

			//�۳�����
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			ScoreInfo[m_wCurrentBanker].lScore =m_lUserWinScore[m_wCurrentBanker];
			ScoreInfo[m_wCurrentBanker].lRevenue = m_lUserRevenue[m_wCurrentBanker];
			ScoreInfo[m_wCurrentBanker].cbType = (m_lUserWinScore[m_wCurrentBanker]>0?SCORE_TYPE_WIN:SCORE_TYPE_LOSE);
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

			//�����
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if (pIServerUserItem != NULL && !pIServerUserItem->IsAndroidUser())
				m_lStorageCurrent -= (m_lUserWinScore[m_wCurrentBanker]);

			m_lUserWinScore[m_wCurrentBanker] = 0;

			//�л�ׯ��
			ChangeBanker(true);

			return true;
		}
	}
	ASSERT(FALSE);
	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:			//����״̬
		{
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);

			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));			

			//������Ϣ
			StatusFree.bIsAllowApplyBanker = true;
			StatusFree.lLimitScore = m_occupyseatConfig.lOccupySeatFree;
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			CopyMemory(StatusFree.lAreaLimitScore, m_lAreaLimitScore, sizeof(StatusFree.lAreaLimitScore));
			StatusFree.wBankerUser=m_wCurrentBanker;	
			StatusFree.wBankerTime=m_wBankerTime;
			StatusFree.lBankerWinScore=m_lBankerWinScore;
			//��ȡׯ��
			IServerUserItem* pIBankerServerUserItem = NULL;
			if ( m_wCurrentBanker == INVALID_CHAIR )
			{
				m_lBankerScore = 1000000000;
			}
			else
			{
				IServerUserItem* pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				if ( pIBankerServerUserItem != NULL )
				{
					m_lBankerScore = pIBankerServerUserItem->GetUserScore();
				}
			}

			StatusFree.lBankerScore = m_lBankerScore;

			//�����Ϣ
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusFree.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*10); 
			}

			//��������
			CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));
			//ռλ
			CopyMemory(StatusFree.cbDeskChairID, m_cbDeskChairID, sizeof(m_cbDeskChairID));

			//ȫ����Ϣ
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(m_nFreeTime-__min(dwPassTime,(DWORD)m_nFreeTime));
			
			//����������
			if(pIServerUserItem->IsAndroidUser())
			{
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
				ASSERT(pCustomConfig);

				CopyMemory(&StatusFree.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
			}
			
			//��������
			tagRobotInfo tmpRobotInfo;
			CopyMemory(&StatusFree.lBet, &tmpRobotInfo.nChip, sizeof(tmpRobotInfo.nChip));

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			//������ʾ
			TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d"),m_lApplyBankerCondition,
				m_lAreaLimitScore,m_lUserLimitScore);

			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);

			//����������
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	case GAME_SCENE_PLACE_JETTON:		//��Ϸ״̬
	case GAME_SCENE_GAME_END:			//����״̬
		{
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);		

			//��������
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));
			StatusPlay.bIsAllowApplyBanker = true;
			//ȫ����ע
			CopyMemory(StatusPlay.lAllJettonScore,&m_lAllJettonScore[1],sizeof(StatusPlay.lAllJettonScore));

			//�����ע
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				//CopyMemory(StatusPlay.lUserJettonScore, &m_lUserJettonScore[1], sizeof(StatusPlay.lUserJettonScore));
				for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
					StatusPlay.lUserJettonScore[nAreaIndex-1] = m_lUserJettonScore[nAreaIndex][wChairID];

				//�����ע
				StatusPlay.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*MAX_RATE);
			}

			//������Ϣ
			StatusPlay.lApplyBankerCondition = m_lApplyBankerCondition;
			CopyMemory(StatusPlay.lAreaLimitScore, m_lAreaLimitScore, sizeof(StatusPlay.lAreaLimitScore));
			StatusPlay.lLimitScore = m_occupyseatConfig.lOccupySeatFree;
			StatusPlay.lStorageStart = m_lStorageCurrent;
			//ׯ����Ϣ
			StatusPlay.wBankerUser=m_wCurrentBanker;			
			StatusPlay.wBankerTime=m_wBankerTime;
			StatusPlay.lBankerWinScore=m_lBankerWinScore;	
			//��ȡׯ��
			IServerUserItem* pIBankerServerUserItem = NULL;
			if ( m_wCurrentBanker == INVALID_CHAIR )
			{
				m_lBankerScore = 1000000000;
			}
			else
			{
				IServerUserItem* pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				if ( pIBankerServerUserItem != NULL )
				{
					m_lBankerScore = pIBankerServerUserItem->GetUserScore();
				}
			}

			StatusPlay.lBankerScore = m_lBankerScore;

			//ȫ����Ϣ
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			int	nTotalTime = (cbGameStatus==GAME_SCENE_PLACE_JETTON?m_nPlaceJettonTime:m_nGameEndTime);
			StatusPlay.cbTimeLeave=(BYTE)(nTotalTime-__min(dwPassTime,(DWORD)nTotalTime));
			StatusPlay.cbGameStatus=m_pITableFrame->GetGameStatus();			
			StatusPlay.bIsJettonStatus = (cbGameStatus == GAME_SCENE_PLACE_JETTON);
			//��������
			CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));
			//ռλ
			CopyMemory(StatusPlay.cbDeskChairID, m_cbDeskChairID, sizeof(m_cbDeskChairID));
			//��������
			tagRobotInfo tmpRobotInfo;
			CopyMemory(&StatusPlay.lBet, &tmpRobotInfo.nChip, sizeof(tmpRobotInfo.nChip));
			//�����ж�
			if (cbGameStatus==GAME_SCENE_GAME_END)
			{
				//���óɼ�
				StatusPlay.lEndUserScore=m_lUserWinScore[wChairID];

				//��������
				StatusPlay.lEndUserReturnScore=m_lUserReturnScore[wChairID];

				//����˰��
				if (m_lUserRevenue[wChairID]>0) StatusPlay.lEndRevenue=m_lUserRevenue[wChairID];
				else if (m_wCurrentBanker!=INVALID_CHAIR) StatusPlay.lEndRevenue=m_lUserRevenue[m_wCurrentBanker];
				else StatusPlay.lEndRevenue=0;

				//ׯ�ҳɼ�
				StatusPlay.lEndBankerScore=m_lBankerCurGameScore;

				//�˿���Ϣ
				CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			}
			
			//����������
			if(pIServerUserItem->IsAndroidUser())
			{
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
				ASSERT(pCustomConfig);

				CopyMemory(&StatusPlay.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
			}

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			//����������
			SendApplyUser( pIServerUserItem );

			return bSuccess;
		}
	}

	return false;
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

			//����ʱ��
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_nPlaceJettonTime*1000,1,0L);

			//�ɷ��˿�
			DispatchTableCard();

			//����״̬
			m_pITableFrame->SetGameStatus(GAME_SCENE_PLACE_JETTON);

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

				//����ʱ��
				m_dwJettonTime=(DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);		
			}

			return true;
		}
	case IDI_GAME_END:			//������Ϸ
		{
			//д�����
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);				
				if ( pIServerUserItem == NULL||(QueryBuckleServiceCharge(wUserChairID)==false)) continue;				
				
				//ʤ������
				BYTE ScoreKind=(m_lUserWinScore[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
				ScoreInfo[wUserChairID].lScore = m_lUserWinScore[wUserChairID] - m_lUserRevenue[wUserChairID];
				ScoreInfo[wUserChairID].lRevenue = m_lUserRevenue[wUserChairID];
				ScoreInfo[wUserChairID].cbType = ScoreKind;

				//�����
				if (!pIServerUserItem->IsAndroidUser())
				{
					m_lStorageCurrent -= (m_lUserWinScore[wUserChairID]);					
				}
			}
			
			//д�����
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

			LONGLONG lTempDeduct=0;
			lTempDeduct=m_nStorageDeduct;
			bool bDeduct=NeedDeductStorage();
			lTempDeduct=bDeduct?lTempDeduct:0;
			//���˥��
			if (m_lStorageCurrent > 0)
				m_lStorageCurrent = m_lStorageCurrent - m_lStorageCurrent*lTempDeduct/1000;

			//����ʱ��
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,0L);

			//������Ϣ
			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree,sizeof(GameFree));
			GameFree.cbTimeLeave=m_nFreeTime;
			GameFree.lStorageStart=m_lStorageCurrent;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			
			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
			
			//�л�ׯ��
			ChangeBanker(false);
			return true;
		}
	}

	return false;
}

bool cmpRoomUserInfo(tagRoomUserInfo st1, tagRoomUserInfo st2)
{
	if (st1.lBetRank20Score > st2.lBetRank20Score)
	{
		return true;
	}
	return false;
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_CLICK_REJETTON:		//�û���ע
	{
		ASSERT(wDataSize == sizeof(CMD_C_UserReJetton));
		if (wDataSize != sizeof(CMD_C_UserReJetton)) return false;

		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		CMD_C_UserReJetton * pUserReJetton = (CMD_C_UserReJetton *)pData;
		ReJetton(pIServerUserItem);
		return true;
	}
	case SUB_C_QURARY_RECORD:
	{
		//�û�Ч��
		//tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		//if (pUserData->cbUserStatus != US_PLAYING) return true;

		SendGameRecord(pIServerUserItem);
		return true;
	}
	case SUB_C_QURARY_LOTTORY_INFO:
	{
		return true;
	}
	case SUB_C_QURARY_ROOM_USERS_INFO:
	{
		vecTagRoomUserInfo tagRoomUserInfoList;
		//������Ϸ��¼
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL) continue;

			tagRoomUserInfo stTagRoomInfo;
			stTagRoomInfo.lBetRank20Score = pIServerUserItem->GetUserScore();
			stTagRoomInfo.wChairID = wChairID;

			//��������ע��ʤ����
			SCORE nTotalBetNum = 0;
			SCORE nTotalWinNum = 0;
			POSITION posTail = m_tagUserRecord[wChairID].GetTailPosition();
			while (posTail)
			{
				tagStatisticsRecord tagStatistRecord;
				tagStatistRecord = m_tagUserRecord[wChairID].GetPrev(posTail);

				nTotalBetNum += tagStatistRecord.lPlayerBetScore;
				if (tagStatistRecord.lPlayerWinScore > 0)
				{
					nTotalWinNum += 1;
				}
			}
			stTagRoomInfo.lBetRank20Jetton = nTotalBetNum;
			stTagRoomInfo.nWinCount = nTotalWinNum;
			tagRoomUserInfoList.push_back(stTagRoomInfo);
		}
		sort(tagRoomUserInfoList.begin(), tagRoomUserInfoList.end(), cmpRoomUserInfo);
		//���������ɷ�������
		CMD_S_SendRoomUserInfo roomUserInfo;
		ZeroMemory(&roomUserInfo, sizeof(CMD_S_SendRoomUserInfo));
		int nPlayerNum = 0;
		for (auto &userInfo : tagRoomUserInfoList)
		{
			roomUserInfo.wChairID[nPlayerNum] = userInfo.wChairID;
			roomUserInfo.nWinCount[nPlayerNum] = userInfo.nWinCount;
			roomUserInfo.lBetRank20Score[nPlayerNum] = userInfo.lBetRank20Score;
			roomUserInfo.lBetRank20Jetton[nPlayerNum] = userInfo.lBetRank20Jetton;
			nPlayerNum++;
			if (nPlayerNum>=20)
			{
				break;
			}
		}
		roomUserInfo.nPlayerCount = nPlayerNum;
		//������Ϣ
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_QURARY_ROOM_PLAYERS_INFO, &roomUserInfo, sizeof(CMD_S_SendRoomUserInfo));
		return true;
	}
	case SUB_C_PLACE_JETTON:		//�û���ע
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			if (wDataSize!=sizeof(CMD_C_PlaceJetton)) return false;

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//��Ϣ����
			CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pData;
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
	case SUB_C_APPLY_DESK:
		{
			//Ч������
			ASSERT (wDataSize == sizeof(CMD_C_ApplyDesk));
			if (wDataSize != sizeof(CMD_C_ApplyDesk)) return true;

			//�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus == US_LOOKON) return true;

			//��Ϣ����
			CMD_C_ApplyDesk *pOccupySeat = (CMD_C_ApplyDesk *)pData;
			if (pOccupySeat->bApply)
			{
				return OnUserOccupySeat(pUserData->wChairID, pOccupySeat->wDeskChairID);
			}
			else
			{
				bool bvalid = false;
				//У������
				WORD wDeskChairID = INVALID_BYTE;
				for (WORD i = 0; i<MAX_OCCUPY_SEAT_COUNT; i++)
				{
					if (m_cbDeskChairID[i] == pUserData->wChairID)
					{
						bvalid = true;
						wDeskChairID = i;
						//������Ч
						m_cbDeskChairID[i] = INVALID_BYTE;
						break;
					}
				}

				if (bvalid == true && wDeskChairID != INVALID_BYTE)
				{
					CMD_S_GameDown GameDown;
					ZeroMemory(&GameDown, sizeof(GameDown));
					GameDown.wChairID = pUserData->wChairID;
					GameDown.wDeskChairID = wDeskChairID + 1;

					//��������
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
				}
			}
		}
	}

	return false;
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
		case SUB_C_AMDIN_COMMAND:
		{

			ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
			if (wDataSize != sizeof(CMD_C_AdminReq)) return false;

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			if (m_pServerContro == NULL) return false;
			CopyMemory(m_szControlName, pIServerUserItem->GetNickName(), sizeof(m_szControlName));

			return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
		}
		case SUB_C_UPDATE_STORAGE:		//���¿��
		{
			ASSERT(wDataSize == sizeof(CMD_C_UpdateStorage));
			if (wDataSize != sizeof(CMD_C_UpdateStorage)) return false;

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			//��Ϣ����
			CMD_C_UpdateStorage * pUpdateStorage = (CMD_C_UpdateStorage *)pData;

			m_lStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
			m_nStorageDeduct = pUpdateStorage->lRoomStorageDeduct;

			CMD_S_ADMIN_STORAGE_INFO StorageInfo;
			ZeroMemory(&StorageInfo, sizeof(StorageInfo));

			StorageInfo.lRoomStorageStart = m_lStorageStart;
			StorageInfo.lRoomStorageDeduct = m_nStorageDeduct;
			StorageInfo.lRoomStorageCurrent = m_lStorageCurrent;
			StorageInfo.lMaxRoomStorage[0] = m_lStorageMax1;
			StorageInfo.wRoomStorageMul[0] = m_lStorageMul1;
			StorageInfo.lMaxRoomStorage[1] = m_lStorageMax2;
			StorageInfo.wRoomStorageMul[1] = m_lStorageMul2;

			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));

			return true;
		}
		case SUB_C_STORAGEMAXMUL:
		{
			ASSERT(wDataSize == sizeof(CMD_C_ModifyStorage));
			if (wDataSize != sizeof(CMD_C_ModifyStorage)) return false;

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pData;
			m_lStorageMax1 = pModifyStorage->lMaxRoomStorage[0];
			m_lStorageMax2 = pModifyStorage->lMaxRoomStorage[1];
			m_lStorageMul1 = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
			m_lStorageMul2 = (SCORE)(pModifyStorage->wRoomStorageMul[1]);


			CMD_S_ADMIN_STORAGE_INFO StorageInfo;
			ZeroMemory(&StorageInfo, sizeof(StorageInfo));
			StorageInfo.lRoomStorageStart = m_lStorageStart;
			StorageInfo.lRoomStorageDeduct = m_nStorageDeduct;
			StorageInfo.lRoomStorageCurrent = m_lStorageCurrent;
			StorageInfo.lMaxRoomStorage[0] = m_lStorageMax1;
			StorageInfo.wRoomStorageMul[0] = m_lStorageMul1;
			StorageInfo.lMaxRoomStorage[1] = m_lStorageMax2;
			StorageInfo.wRoomStorageMul[1] = m_lStorageMul2;

			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));

			return true;
		}
		case SUB_C_REFRESH_ROOM_STORAGESTATUS:
		{
			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false) return false;

			CMD_S_ADMIN_STORAGE_INFO StorageInfo;
			ZeroMemory(&StorageInfo, sizeof(StorageInfo));
			StorageInfo.lRoomStorageStart = m_lStorageStart;
			StorageInfo.lRoomStorageDeduct = m_nStorageDeduct;
			StorageInfo.lRoomStorageCurrent = m_lStorageCurrent;
			StorageInfo.lMaxRoomStorage[0] = m_lStorageMax1;
			StorageInfo.wRoomStorageMul[0] = m_lStorageMul1;
			StorageInfo.lMaxRoomStorage[1] = m_lStorageMax2;
			StorageInfo.wRoomStorageMul[1] = m_lStorageMul2;

			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			return true;
		}
		}
	}
	return false;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem)
{
	//����ׯ��
	if (wChairID == m_wCurrentBanker && pIServerUserItem->IsAndroidUser() == false)
	{
		m_wOfflineBanker = wChairID;
	}

	if(pIServerUserItem == NULL) return false;
	//�л�ׯ��
	if (wChairID==m_wCurrentBanker) ChangeBanker(true);

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
		CancelBanker.wCancelUser=wChairID;

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}
	return true;

}
//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʼ����
	m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();

	//����ʱ��
	if ((bLookonUser==false)&&(m_dwJettonTime==0L))
	{
		m_dwJettonTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
	}
	
	//������ʾ
	TCHAR szTipMsg[128];
	_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ��ұ�����ڣ�%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d"),m_lApplyBankerCondition,
		m_lAreaLimitScore,m_lUserLimitScore);
	m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT|SMT_EJECT);

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	ASSERT(pIServerUserItem !=NULL);

	//��ʼ����
	m_lUserStartScore[wChairID] = 0;
	
	bool bvalid = false;
	//У������
	WORD wDeskChairID = INVALID_BYTE;
	for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
	{
		if (m_cbDeskChairID[i] == wChairID)
		{
			bvalid = true;
			wDeskChairID = i;
			//������Ч
			m_cbDeskChairID[i] = INVALID_BYTE;

			break;
		}
	}

	if (bvalid == true && wDeskChairID != INVALID_BYTE)
	{
		CMD_S_GameDown GameDown;
		ZeroMemory(&GameDown, sizeof(GameDown));
		GameDown.wChairID = wChairID;
		GameDown.wDeskChairID = wDeskChairID + 1;
		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_DOWN_DESK, &GameDown, sizeof(GameDown));
	}

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
			CancelBanker.wCancelUser=wChairID;

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}
	}

	return true;
}


//��ע�¼�
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
	//Ч�����
	ASSERT((cbJettonArea<= POKER_TYPE_INDEX && cbJettonArea>= RED_INDEX)&&(lJettonScore>0L));
	if ((cbJettonArea>POKER_TYPE_INDEX)||(lJettonScore<=0L) || cbJettonArea<RED_INDEX) return false;
	
	if (cbJettonArea == RED_INDEX)
	{
		if (m_lUserJettonScore[BLACK_INDEX][wChairID] > 0)
		{
			CMD_S_PlaceJettonFail PlaceJettonFail;
			ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
			PlaceJettonFail.lJettonArea = cbJettonArea;
			PlaceJettonFail.lPlaceScore = lJettonScore;
			PlaceJettonFail.wPlaceUser = wChairID;
			PlaceJettonFail.cbReason = 3;//�ڷ��ͺ췽����ͬʱ��ע
			//������Ϣ
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
			return true;
		}
	}
	else if (cbJettonArea == BLACK_INDEX)
	{
		if (m_lUserJettonScore[RED_INDEX][wChairID] > 0)
		{
			CMD_S_PlaceJettonFail PlaceJettonFail;
			ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
			PlaceJettonFail.lJettonArea = cbJettonArea;
			PlaceJettonFail.lPlaceScore = lJettonScore;
			PlaceJettonFail.wPlaceUser = wChairID;
			PlaceJettonFail.cbReason = 3;//�ڷ��ͺ췽����ͬʱ��ע
			//������Ϣ
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
			return true;
		}
	}
	if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLACE_JETTON)
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea=cbJettonArea;
		PlaceJettonFail.lPlaceScore=lJettonScore;
		PlaceJettonFail.wPlaceUser=wChairID;
		PlaceJettonFail.cbReason = 0; //��ǰ��������ע״̬
		//������Ϣ
		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
		return true;
	}
	//ׯ���ж�
	if (m_wCurrentBanker==wChairID)
	{
		return true;
	}
	if (m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR) 
	{
		return true;
	}

	//ׯ�һ���
	LONGLONG lBankerScore = 0;
	if(INVALID_CHAIR != m_wCurrentBanker)
	{
		IServerUserItem * pIServerUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		lBankerScore = pIServerUserItemBanker->GetUserScore();
	}

	//��������
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	LONGLONG lJettonCount=0L;
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];
	LONGLONG lAllJettonCount=0L;
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lAllJettonCount += m_lAllJettonScore[nAreaIndex];

	//��һ���
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//�Ϸ�У��
	if (m_lUserLimitScore < lJettonCount + lJettonScore || lUserScore < lJettonCount + lJettonScore)
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea = cbJettonArea;
		PlaceJettonFail.lPlaceScore = lJettonScore;
		PlaceJettonFail.wPlaceUser = wChairID;
		PlaceJettonFail.cbReason = 1;	//��ǰ��ע�������û��Ļ�������
		//������Ϣ
		m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
		return true;
	}

	//��ע�ܶ��ׯ����֧������ʱ��ǰ������Ϸ
	if (INVALID_CHAIR != m_wCurrentBanker)
	{
		if (lBankerScore <= (lAllJettonCount + lJettonScore + 100) * 10)
		{
			CMD_S_PlaceJettonFail PlaceJettonFail;
			ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
			PlaceJettonFail.lJettonArea = cbJettonArea;
			PlaceJettonFail.lPlaceScore = lJettonScore;
			PlaceJettonFail.wPlaceUser = wChairID;
			PlaceJettonFail.cbReason = 5;	//��ǰ��ע�������û��Ļ�������
											//������Ϣ
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
			return true;
		}
	}

	//�ɹ���ʶ
	bool bPlaceJettonSuccess=true;

	//�Ϸ���֤
	if (GetUserMaxJetton(wChairID, cbJettonArea) >= lJettonScore)
	{
		//��������֤
		if(pIServerUserItem->IsAndroidUser())
		{
			//��������
			if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
				return true;

			//��Ŀ����
			bool bHaveChip = false;
			for (int i = 1; i <= AREA_COUNT; i++)
			{
				if (m_lUserJettonScore[i+1][wChairID] != 0)
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
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;
		}

		if (bPlaceJettonSuccess)
		{
			for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
			{
				//����Ͼ���ע
				m_lLastAllJettonPlace[nAreaIndex][wChairID] = 0;
				//��ձ������һע
				m_lLastAllJettonArea[nAreaIndex][wChairID] = 0;
			}

			m_lLastAllJettonArea[cbJettonArea][wChairID] += lJettonScore;
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
		for(WORD i=0; i<GAME_PLAYER; i++)
		{
			IServerUserItem *pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
			if ( pIServerUserItemSend == NULL ) continue;
			
			bool bIsGameCheatUser =  CUserRight::IsGameDebugUser(pIServerUserItemSend->GetUserRight());

			//��������
			CMD_S_PlaceJetton PlaceJetton;
			ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

			//�������
			PlaceJetton.wChairID=wChairID;
			PlaceJetton.cbJettonArea=cbJettonArea;
			PlaceJetton.lJettonScore=lJettonScore;
			PlaceJetton.bIsAndroid = m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser();
			//������Ϣ
			m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
			m_pITableFrame->SendLookonData(i,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
		}

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
				for (BYTE j = 1; j <= AREA_COUNT; j++)
				{
					lUserAllBet += m_lUserJettonScore[j][i];
				}
				if (0 == lUserAllBet) continue;

				//��������
				tagUserBet *UserBet = (tagUserBet*)(cbBuffer + dwPaketSize);
				CopyMemory(UserBet->szNickName, pServerUserItem->GetNickName(), sizeof(UserBet->szNickName));
				UserBet->dwUserGameID = pServerUserItem->GetGameID();
				UserBet->lUserStartScore = m_lUserStartScore[i];
				UserBet->lUserWinLost = pServerUserItem->GetUserScore() - UserBet->lUserStartScore;
				for (BYTE k = 1; k <= AREA_COUNT; k++)
				{
					UserBet->lUserBet[k-1] = m_lUserJettonScore[k][i];
				}

				dwPaketSize += sizeof(tagUserBet);
			}

			m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, cbBuffer, dwPaketSize);

		}
	}
	else
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea=cbJettonArea;
		PlaceJettonFail.lPlaceScore=lJettonScore;
		PlaceJettonFail.wPlaceUser=wChairID;
		PlaceJettonFail.cbReason = 1;	//��ע������������ҵ���ע����
		//������Ϣ
		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
	}

	return true;
}

//�����˿�
bool CTableFrameSink::DispatchTableCard()
{

	//����ϴ��
	m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));	
	//�����˿�
	CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));
	
	//m_cbTableCardArray[0][0] = 0x06;
	//m_cbTableCardArray[0][1] = 0x16;
	//m_cbTableCardArray[0][2] = 0x1A;

	//m_cbTableCardArray[1][0] = 0x03;
	//m_cbTableCardArray[1][1] = 0x04;
	//m_cbTableCardArray[1][2] = 0x05;
	return true;
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
			return true;
		}
	}

	if (pIApplyServerUserItem->IsAndroidUser()&&(m_ApplyUserArray.GetCount())>m_nRobotListMaxCount)
	{
		return true;
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
	if (m_pITableFrame->GetGameStatus()==GAME_SCENE_FREE && m_ApplyUserArray.GetCount()==1)
	{
		ChangeBanker(false);
	}

	return true;
}

//ȡ������
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//��ǰׯ��
	if (pICancelServerUserItem->GetChairID()==m_wCurrentBanker && m_pITableFrame->GetGameStatus()!=GAME_SCENE_FREE)
	{
		//������Ϣ
		m_pITableFrame->SendGameMessage(pICancelServerUserItem,TEXT("��Ϸ�Ѿ���ʼ��������ȡ����ׯ��"),SMT_CHAT|SMT_EJECT);
		return true;
	}
	
	bool bValidQuitBanker = true;
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

		bValidQuitBanker = false;

		if (m_wCurrentBanker!=wChairID)
		{
			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//���ñ���
			CancelBanker.bAuto = false;	//�ֶ�ȡ��ׯ��
			CancelBanker.wCancelUser = wChairID;
			CancelBanker.bIsAllowApplyBanker = false;
			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker==wChairID)
		{
			//�л�ׯ�� 
			m_wCurrentBanker=INVALID_CHAIR;
			ChangeBanker(true);
		}

		return true;
	}
	
	if (bValidQuitBanker == true)
	{
		m_wCurrentBanker=INVALID_CHAIR;
		ChangeBanker(true);
		return true;
	}

	return true;
}

//�û�ռλ
bool CTableFrameSink::OnUserOccupySeat(WORD wOccupyChairID, BYTE cbOccupySeatIndex)
{
	//У���û�
	ASSERT (wOccupyChairID != INVALID_CHAIR);
	if (wOccupyChairID == INVALID_CHAIR) return true;

	ASSERT (cbOccupySeatIndex >= SEAT_LEFT1_INDEX && cbOccupySeatIndex <= SEAT_RIGHT3_INDEX);
	if (!(cbOccupySeatIndex >= SEAT_LEFT1_INDEX && cbOccupySeatIndex <= SEAT_RIGHT3_INDEX))
	{
		return true;
	}

	//ׯ�Ҳ�����������
	if (m_wCurrentBanker == wOccupyChairID)
	{
		CMD_S_UpDesk UpDesk;
		ZeroMemory(&UpDesk, sizeof(UpDesk));
		UpDesk.bUpDeskOk = false;
		UpDesk.wChairID = wOccupyChairID;
		UpDesk.wDeskChairID = cbOccupySeatIndex;
		UpDesk.cbFailReason = 5;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		return true;
	}

	//ռλʧ�� û�п����λ��
	if (m_cbDeskChairID[cbOccupySeatIndex-1] != INVALID_BYTE)
	{
		CMD_S_UpDesk UpDesk;
		ZeroMemory(&UpDesk, sizeof(UpDesk));
		UpDesk.bUpDeskOk = false;
		UpDesk.wChairID = wOccupyChairID;
		UpDesk.wDeskChairID = cbOccupySeatIndex;
		UpDesk.cbFailReason = 1;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		return true;
	}
	ASSERT (m_cbDeskChairID[cbOccupySeatIndex-1] == INVALID_BYTE);

	//У���Ƿ��Ѿ�ռλ
	for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
	{
		//ռλʧ�� ���Ѿ�����λ����
		if (m_cbDeskChairID[i] == wOccupyChairID)
		{
			CMD_S_UpDesk UpDesk;
			ZeroMemory(&UpDesk, sizeof(UpDesk));
			UpDesk.bUpDeskOk = false;
			UpDesk.wChairID = wOccupyChairID;
			UpDesk.wDeskChairID = cbOccupySeatIndex;
			UpDesk.cbFailReason = 3;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
			return true;
		}
	}

	//ռλʧ��  �Ƹ�����
	ASSERT (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() >= m_occupyseatConfig.lOccupySeatFree);
	if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < m_occupyseatConfig.lOccupySeatFree)
	{
		CMD_S_UpDesk UpDesk;
		ZeroMemory(&UpDesk, sizeof(UpDesk));
		UpDesk.bUpDeskOk = false;
		UpDesk.wChairID = wOccupyChairID;
		UpDesk.wDeskChairID = cbOccupySeatIndex;
		UpDesk.cbFailReason = 2;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
		return true;
	}


	//ռλ�ɹ�
	m_cbDeskChairID[cbOccupySeatIndex-1] = wOccupyChairID;

	CMD_S_UpDesk UpDesk;
	ZeroMemory(&UpDesk, sizeof(UpDesk));
	UpDesk.bUpDeskOk = true;
	UpDesk.wChairID = wOccupyChairID;
	UpDesk.wDeskChairID = cbOccupySeatIndex;

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UP_DESK, &UpDesk, sizeof(UpDesk));

	return true;
}

//����ׯ��
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	//�л���ʶ
	bool bChangeBanker=false;

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

		//�ֻ��ж�
		TakeTurns();

		//���ñ���
		bChangeBanker=true;
		m_bExchangeBanker = true;
	}
	//��ׯ�ж�
	else if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		{
			//��ȡׯ��
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if(pIServerUserItem)
			{
				LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

				//�����ж�
				if (m_lPlayerBankerMAX<=m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
				{
					//ׯ�������ж� ͬһ��ׯ�������ֻ�ж�һ��
					if(m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker && lBankerScore>=m_lApplyBankerCondition)
					{
						//��ׯ�������ã���ׯ�������趨�ľ���֮��(m_nBankerTimeLimit)��
						//�������ֵ��������������ׯ���б�����������ҽ��ʱ��
						//�����ټ���ׯm_nBankerTimeAdd�֣���ׯ���������á�

						//��ҳ���m_lExtraBankerScore֮��
						//������������ҵĽ��ֵ�������Ľ��ֵ����Ҳ�����ټ�ׯm_nExtraBankerTime�֡�
						bool bScoreMAX = true;
						m_bExchangeBanker = false;

						for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
						{
							//��ȡ���
							WORD wChairID = m_ApplyUserArray[i];
							IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
							LONGLONG lScore = pIServerUserItem->GetUserScore();

							if ( wChairID != m_wCurrentBanker && lBankerScore <= lScore )
							{
								bScoreMAX = false;
								break;
							}
						}

						if ( bScoreMAX || (lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l) )
						{
							if ( bScoreMAX )
							{
								m_lPlayerBankerMAX += m_nBankerTimeAdd;
							}
							if ( lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l )
							{
								m_lPlayerBankerMAX += m_nExtraBankerTime;
							}
							return true;
						}
					}

					//����ׯ��
					m_wCurrentBanker=INVALID_CHAIR;

					//�ֻ��ж�
					TakeTurns();

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

					bChangeBanker=true;
					m_bExchangeBanker = true;

					//��ʾ��Ϣ
					TCHAR szTipMsg[128];
					if (lBankerScore<m_lApplyBankerCondition)
						_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��������(%I64d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),m_lApplyBankerCondition);
					else
						_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��ׯ�����ﵽ(%d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),m_lPlayerBankerMAX);

					//������Ϣ
					SendGameMessage(INVALID_CHAIR,szTipMsg);	
				}
			}
		}
	}
	//ϵͳ��ׯ
	else if (m_wCurrentBanker == INVALID_CHAIR && m_ApplyUserArray.GetCount()!=0)
	{
		//�ֻ��ж�
		TakeTurns();

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

		bChangeBanker=true;
		m_bExchangeBanker = true;
	}

	//�л��ж�
	if (bChangeBanker)
	{
		//�����ׯ��
		m_lPlayerBankerMAX = m_nBankerTimeLimit;

		//���ñ���
		m_wBankerTime = 0;
		m_lBankerWinScore=0;

		//������Ϣ
		CMD_S_ChangeBanker stChangeBanker;
		ZeroMemory(&stChangeBanker,sizeof(stChangeBanker));
		stChangeBanker.wBankerUser = m_wCurrentBanker;
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			stChangeBanker.lBankerScore = pIServerUserItem->GetUserScore();
		}
		else
		{
			stChangeBanker.lBankerScore = 1000000000;
		}

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&stChangeBanker,sizeof(CMD_S_ChangeBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&stChangeBanker,sizeof(CMD_S_ChangeBanker));

		if (m_wCurrentBanker!=INVALID_CHAIR)
		{
			//��ȡ����
			m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
			ASSERT(m_pGameServiceOption!=NULL);

			//��ȡ��Ϣ
			int nMessageCount = 3;

			//��ȡ����
			INT nIndex=rand()%nMessageCount;
			TCHAR szMessage1[256],szMessage2[256];
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomConfig);
			if(0 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem1, sizeof(pCustomConfig->CustomGeneral.szMessageItem1));
			}
			else if(1 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem2, sizeof(pCustomConfig->CustomGeneral.szMessageItem2));
			}
			else if(2 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem3, sizeof(pCustomConfig->CustomGeneral.szMessageItem3));
			}

			//��ȡ���
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

			//������Ϣ
			_sntprintf(szMessage2,CountArray(szMessage2),TEXT("�� %s ����ׯ�ˣ�%s"),pIServerUserItem->GetNickName(), szMessage1);
			SendGameMessage(INVALID_CHAIR,szMessage2);
		}
	}

	return bChangeBanker;
}

//�ֻ��ж�
void CTableFrameSink::TakeTurns()
{
	//��������
	int nInvalidApply = 0;

	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE)
		{
			//��ȡ����
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray[i]);
			if (pIServerUserItem != NULL)
			{
				if (pIServerUserItem->GetUserScore() >= m_lApplyBankerCondition)
				{
					m_wCurrentBanker=m_ApplyUserArray[i];
					break;
				}
				else
				{
					nInvalidApply = i + 1;

					//������Ϣ
					CMD_S_CancelBanker CancelBanker = {};

					//���ñ���
					CancelBanker.wCancelUser=pIServerUserItem->GetChairID();					

					//������Ϣ
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

					//��ʾ��Ϣ
					TCHAR szTipMsg[128] = {};
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("������Ľ������%I64d��������ׯ����������%I64d��,���޷���ׯ��"),
						pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
					SendGameMessage(m_ApplyUserArray[i],szTipMsg);
				}
			}
		}
	}

	//ɾ�����
	if (nInvalidApply != 0)
		m_ApplyUserArray.RemoveAt(0, nInvalidApply);
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


//�����ע
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID, BYTE cbJettonArea)
{
	IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL==pIMeServerUserItem) return 0L;

	//����ע��
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];

	//ׯ�ҽ��
	LONGLONG lBankerScore=m_lBankerScore;
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lBankerScore-=m_lAllJettonScore[nAreaIndex]* nAreaIndex == POKER_TYPE_INDEX ? MAX_RATE : RATE_PAIR;

	//��������
	LONGLONG lAreaLimitScore = cbJettonArea == POKER_TYPE_INDEX ?  m_lAreaLimitScore[1] - m_lAllJettonScore[cbJettonArea] : m_lAreaLimitScore[0] - m_lAllJettonScore[cbJettonArea];

	LONGLONG lMeMaxScore = 0;
	//������������ע
	if (m_lUserJettonScore[POKER_TYPE_INDEX][wChairID] > 0)
	{
		lNowJetton -= m_lUserJettonScore[POKER_TYPE_INDEX][wChairID];
		lNowJetton += m_lUserJettonScore[POKER_TYPE_INDEX][wChairID] * MAX_RATE;
	}

	//��������
	if (cbJettonArea == POKER_TYPE_INDEX)
		lMeMaxScore = min((pIMeServerUserItem->GetUserScore() - lNowJetton) / MAX_RATE, m_lUserLimitScore);
	else
		lMeMaxScore = min(pIMeServerUserItem->GetUserScore() - lNowJetton, m_lUserLimitScore);

	//��������
	lMeMaxScore=min(lMeMaxScore,lAreaLimitScore);

	//ׯ������
	if (cbJettonArea == POKER_TYPE_INDEX)
		lMeMaxScore=min(lMeMaxScore,lBankerScore/MAX_RATE);
	else
		lMeMaxScore = min(lMeMaxScore, lBankerScore);

	//��������
	ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return (lMeMaxScore);

}
//����÷�
LONGLONG CTableFrameSink::CalculateScore()
{
	//�ƶ���Ӯ
	DeduceWinner(m_bWinRed, m_bWinBlack, m_bWinPoker,m_cbPokerType);

	//��Ϸ��¼

	//�����֮ǰЧ��20����¼�Ƿ��Ѿ����ˣ�  ������� �����Ƴ���һ��  ʼ�ձ��ּ�¼20�����ݼ�¼
	if (m_dwRecordCount >= MAX_SCORE_HISTORY)
	{
		MoveMemory(&m_GameRecordArrary[0], &m_GameRecordArrary[1], sizeof(tagServerGameRecord) * 19);
		m_dwRecordCount--;
	}
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_dwRecordCount];
	GameRecord.bWinRed = m_bWinRed;
	GameRecord.bWinBlack = m_bWinBlack;
	GameRecord.bWinPoker = m_bWinPoker;
	GameRecord.cbPokerType= m_bWinRed ? m_cbPokerType[0] : m_cbPokerType[1];
	m_dwRecordCount++;

	BYTE cbWin = m_bWinRed ? 1 : 2;

	//�Ͼֳɼ�
	if (m_cbRBHistory[m_nRBCurrentColumn][m_nRBCurrentRow] != 0)
	{
		//�ж��Ƿ���Ӯ
		if (m_cbRBHistory[m_nRBCurrentColumn][m_nRBCurrentRow] == cbWin)
		{
			//��Ӯ����ֻ��¼6��
			if (m_nRBCurrentRow+1 >= 6) // ֻ��¼6�� ����6�� ������һ�н��м�¼
			{
				m_nRBCurrentColumn++;

				if(m_nRBCurrentColumn >= 20)
				{ 
					//ɾ��
					MoveMemory(&m_cbRBHistory[0][0], &m_cbRBHistory[1][0], sizeof(m_cbRBHistory) - sizeof(m_cbRBHistory[0]));
					m_nRBCurrentColumn--;
					ZeroMemory(m_cbRBHistory[m_nRBCurrentColumn], sizeof(m_cbRBHistory[0]));	//���ԭ������
				}
				m_nRBCurrentRow = 0;
			}
			else
			{
				m_nRBCurrentRow++;
			}

			m_cbRBHistory[m_nRBCurrentColumn][m_nRBCurrentRow] = cbWin;
		}
		else  //������Ӯ  ��ô������һ�н��м�¼
		{
			m_nRBCurrentColumn++;

			if (m_nRBCurrentColumn >= 20)
			{
				//ɾ��
				MoveMemory(&m_cbRBHistory[0][0], &m_cbRBHistory[1][0], sizeof(m_cbRBHistory) - sizeof(m_cbRBHistory[0]));
				m_nRBCurrentColumn--;
				ZeroMemory(m_cbRBHistory[m_nRBCurrentColumn],sizeof(m_cbRBHistory[0]));  //���ԭ������
			}
			m_nRBCurrentRow = 0;
			m_cbRBHistory[m_nRBCurrentColumn][m_nRBCurrentRow] = cbWin;
		}
	}
	else   //����������һ����¼ 
	{
		//m_nRBCurrentColumn++;
		if (m_nRBCurrentColumn+1 >= 20)
		{
			//ɾ��
			MoveMemory(&m_cbRBHistory[0][0], &m_cbRBHistory[1][0], sizeof((&m_cbRBHistory[1][0])));
			//m_nRBCurrentColumn--;
		}
		m_nRBCurrentRow = 0;
		m_cbRBHistory[m_nRBCurrentColumn][m_nRBCurrentRow] = cbWin;
	}


	//ׯ������
	LONGLONG lBankerWinScore = 0;

	//��ҳɼ�
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//Ĭ��ȫ��Ѻ1��1
	BYTE cbMultiple[] = {1,1,1,1};
	//�����������Ӯ�˵Ļ� �������͵õ�����
	if (m_bWinPoker) cbMultiple[POKER_TYPE_INDEX] = m_GameLogic.GetTimes(m_bWinRed ? m_cbPokerType[0] : m_cbPokerType[1]);

	//Ӯ�ұ�ʶ
	bool static bWinFlag[AREA_COUNT+1];
	bWinFlag[0] = false;
	bWinFlag[RED_INDEX] = m_bWinRed;
	bWinFlag[BLACK_INDEX] = m_bWinBlack;
	bWinFlag[POKER_TYPE_INDEX] = m_bWinPoker;
	//�������
	for (WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
	{
		//ׯ���ж�
		if (m_wCurrentBanker==wChairID) continue;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		for (WORD wAreaIndex = RED_INDEX; wAreaIndex <= POKER_TYPE_INDEX; ++wAreaIndex)
		{
			if (m_lUserJettonScore[wAreaIndex][wChairID] != 0)
			{
				if (true == bWinFlag[wAreaIndex])
				{
					m_lUserWinScore[wChairID] += (m_lUserJettonScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex]);
					//m_lUserReturnScore[wChairID] += m_lUserJettonScore[wAreaIndex][wChairID];
					lBankerWinScore -= (m_lUserJettonScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex]);
				}
				else
				{
					lUserLostScore[wChairID] -= m_lUserJettonScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex];
					lBankerWinScore += m_lUserJettonScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex];
				}
			}
		}
		
		//�ܵķ���
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
		//����˰��
		if (0 < m_lUserWinScore[wChairID])
		{
			//����˰��
			m_lUserRevenue[wChairID] = m_pITableFrame->CalculateRevenue(wChairID, m_lUserWinScore[wChairID]);
		}
	}

	//ׯ�ҳɼ�
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;

		//����˰��
		if (0 < m_lUserWinScore[m_wCurrentBanker])
		{
			m_pITableFrame->CalculateRevenue(m_wCurrentBanker, m_lUserWinScore[m_wCurrentBanker]);
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker] - m_lUserRevenue[m_wCurrentBanker];
		}	
	}

	//����ׯ�ҳɼ�
	if (m_wOfflineBanker!=INVALID_CHAIR)
	{
		m_lUserWinScore[m_wOfflineBanker] = lBankerWinScore;

		//����˰��
		if (0 < m_lUserWinScore[m_wOfflineBanker])
		{
			m_lUserRevenue[m_wOfflineBanker] = m_pITableFrame->CalculateRevenue(m_wOfflineBanker, m_lUserWinScore[m_wOfflineBanker]);
			lBankerWinScore = m_lUserWinScore[m_wOfflineBanker] - m_lUserRevenue[m_wOfflineBanker];
		}	
	}

	//�ۼƻ���
	m_lBankerWinScore += lBankerWinScore;

	//��ǰ����
	m_lBankerCurGameScore=lBankerWinScore;

	//���㱾����Ϸ��¼
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		//���ɱ���ս��
		tagStatisticsRecord tagRecord;
		tagRecord.lPlayerWinScore = m_lUserWinScore[wChairID];
		//������ע
		SCORE lBetScoreTmp = 0;
		for (WORD wAreaIndex = RED_INDEX; wAreaIndex <= POKER_TYPE_INDEX; ++wAreaIndex)
		{
			lBetScoreTmp += m_lUserJettonScore[wAreaIndex][wChairID];
		}
		tagRecord.lPlayerBetScore = lBetScoreTmp;

		//�ж��Ƿ񳬳�20�ĳ���
		if (m_tagUserRecord[wChairID].GetCount() >= 20) {
			m_tagUserRecord[wChairID].RemoveHead();
		}
		m_tagUserRecord[wChairID].AddTail(tagRecord);
	}

	return lBankerWinScore;
}


void CTableFrameSink::DeduceWinner(bool &bWinRed, bool &bWinBlack, bool &bWinPoker,BYTE cbPokerType[])
{
	bWinRed = false;
	bWinBlack = false;
	bWinPoker = false;
	BYTE cbWin = m_GameLogic.CompareCard(m_cbTableCardArray[0], m_cbTableCardArray[1], MAX_CARD, cbPokerType);
	if(cbWin)bWinRed = true;
	else bWinBlack = true;

	BYTE cbPk = bWinRed ? cbPokerType[0] : cbPokerType[1];
	if (cbPk > CT_DOUBLE) bWinPoker = true;

}

//���ͼ�¼
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{

	CMD_S_GameHistoryRecord GameRecord;
	ZeroMemory(&GameRecord, sizeof(GameRecord));
	CopyMemory(GameRecord.cbRBHistory, m_cbRBHistory, sizeof(GameRecord.cbRBHistory));

	for (int i = 0; i < m_dwRecordCount; i++)
	{
		GameRecord.cbPokerType[i] = m_GameRecordArrary[i].cbPokerType;
		GameRecord.cbRB_WL_History[i] = m_GameRecordArrary[i].bWinRed ? 1 : 2;
	}
	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_HISTORY_RECORD, &GameRecord, sizeof(GameRecord));
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

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{
	//��ȡ�Զ�������
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomConfig);

	//��ׯ
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_nBankerTimeLimit = pCustomConfig->CustomGeneral.lBankerTime;
	m_nBankerTimeAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lExtraBankerScore = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_nExtraBankerTime = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker = (pCustomConfig->CustomGeneral.nEnableSysBanker == TRUE)?true:false;

	//ռλ
	CopyMemory(&m_occupyseatConfig, &(pCustomConfig->CustomGeneral.occupyseatConfig), sizeof(m_occupyseatConfig));

	//ʱ��
	m_nFreeTime = pCustomConfig->CustomGeneral.lFreeTime;
	m_nPlaceJettonTime = pCustomConfig->CustomGeneral.lBetTime;
	m_nGameEndTime = pCustomConfig->CustomGeneral.lEndTime;
	if(m_nFreeTime < TIME_FREE	|| m_nFreeTime > 99) m_nFreeTime = TIME_FREE;
	if(m_nPlaceJettonTime < TIME_PLACE_JETTON || m_nPlaceJettonTime > 99) m_nPlaceJettonTime = TIME_PLACE_JETTON;
	if(m_nGameEndTime < TIME_GAME_END || m_nGameEndTime > 99) m_nGameEndTime = TIME_GAME_END;

	//��ע
	CopyMemory(m_lAreaLimitScore,(pCustomConfig->CustomGeneral.lAreaLimitScore), sizeof(m_lAreaLimitScore));
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;

	//���
	m_lStorageStart = pCustomConfig->CustomGeneral.StorageStart;
	m_lStorageCurrent = m_lStorageStart;
	m_nStorageDeduct = pCustomConfig->CustomGeneral.StorageDeduct;
	m_lStorageMax1 = pCustomConfig->CustomGeneral.StorageMax1;
	m_lStorageMul1 = pCustomConfig->CustomGeneral.StorageMul1;
	m_lStorageMax2 = pCustomConfig->CustomGeneral.StorageMax2;
	m_lStorageMul2 = pCustomConfig->CustomGeneral.StorageMul2;
	if(m_lStorageMul1 < 0 || m_lStorageMul1 > 100 ) m_lStorageMul1 = 50;
	if(m_lStorageMul2 < 0 || m_lStorageMul2 > 100 ) m_lStorageMul2 = 80;
	
	//������
	m_nRobotListMaxCount = pCustomConfig->CustomAndroid.lRobotListMaxCount;

	LONGLONG lRobotBetMinCount = pCustomConfig->CustomAndroid.lRobotBetMinCount;
	LONGLONG lRobotBetMaxCount = pCustomConfig->CustomAndroid.lRobotBetMaxCount;
	m_nMaxChipRobot = rand()%(lRobotBetMaxCount-lRobotBetMinCount+1) + lRobotBetMinCount;
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 8;
	m_lRobotAreaLimit = pCustomConfig->CustomAndroid.lRobotAreaLimit;
	
	return;
}

// ����Ż�
VOID CTableFrameSink::StorageOptimize()
{
	bool blRealPlayerWin = true;
	if (((m_lStorageCurrent > m_lStorageMax1) && (rand()%100 < m_lStorageMul1))
		|| ((m_lStorageCurrent > m_lStorageMax2) && (rand()%100 < m_lStorageMul2))
		|| rand() % 100 < 30)
	{
		blRealPlayerWin = true;
	}
	else
	{
		blRealPlayerWin = false;
	}

	LONGLONG lCirculateCount = 0L;

	while(true)
	{
		//������ҵ÷�
		LONGLONG lRealPlayerWinScore = CalSysOrAndroidBankerScore();

		if (lCirculateCount > 10000)
		{
			//blRealPlayerWin = false;
			
			if (m_lStorageCurrent - lRealPlayerWinScore < 0 /*&& blRealPlayerWin == true*/)
			{
				blRealPlayerWin = false;
				lCirculateCount = 0;
				DispatchTableCard();
				continue;
			}

			break;
		}

		//��治��Ϊ����
		if (m_lStorageCurrent - lRealPlayerWinScore  <= 0)
		{
			DispatchTableCard();
			lCirculateCount++;
			continue;
		}

		//��������⸶����
		LONGLONG lStorageCurrent = m_lStorageCurrent - lRealPlayerWinScore;
		double llCompensateRadio = (double)((double)30 / (double)100);
		if (m_lStorageCurrent * (1 - llCompensateRadio) >= lStorageCurrent)
		{
			DispatchTableCard();
			lCirculateCount++;
			continue;
		}

		//���������Ǯ
		if (lRealPlayerWinScore <= 0 && blRealPlayerWin == false)
		{
			break;
		}
		
		if (lRealPlayerWinScore >= 0 && blRealPlayerWin == true)
		{
			break;
		}
		
		DispatchTableCard();
		lCirculateCount++;
	}
	
	//ׯ��Ӯ��
	//if (m_lStorageCurrent - CalSysOrAndroidBankerScore() < 0)
	//{
	//	CString strFileName;
	//	strFileName.Format(TEXT("����ţţ[%s]��ע ��Ϣ.log"), m_pGameServiceOption->szServerName);
	//
	//	WriteInfo(strFileName, TEXT("ׯ��Ӯ��"));

	//	m_pServerContro->GetBankerWinResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
	//}
}

//����ϵͳ����������ׯʱ������ҵ÷�
LONGLONG CTableFrameSink::CalSysOrAndroidBankerScore()
{
	
	ASSERT ((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
		|| (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true));

	//�Ƚϱ���
	
	bool static bWinRed, bWinBlack, bWinPoker;
	BYTE cbPokerType[2];
	DeduceWinner(bWinRed, bWinBlack, bWinPoker,  cbPokerType);
	BYTE cbMultiple[] = { 1, 1, 1, 1 };
	if (m_bWinPoker) cbMultiple[POKER_TYPE_INDEX] = m_GameLogic.GetTimes(m_bWinRed ? m_cbPokerType[0] : m_cbPokerType[1]);

	bool static bWinFlag[AREA_COUNT + 1];
	bWinFlag[0] = false;
	bWinFlag[RED_INDEX] = bWinRed;
	bWinFlag[BLACK_INDEX] = bWinBlack;
	bWinFlag[POKER_TYPE_INDEX] = bWinPoker;
	


	//ϵͳ����������ׯʱ������ҵ÷�
	LONGLONG lRealPlayerWinScore = 0L;

	// ����ϵͳ��Ӯ
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		// ��ȡ���
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		// ����ׯ��
		if (wChairID == m_wCurrentBanker || pIServerUserItem == NULL)
		{
			continue;
		}

		//���˻�����
		if (pIServerUserItem->IsAndroidUser() == true)
		{
			continue;
		}

		// ���������ע
		for (int nAarea = 1; nAarea <= AREA_COUNT; nAarea++)
		{
			if ( m_lUserJettonScore[nAarea+1][wChairID] != 0 )
			{
				if(bWinFlag[nAarea] )
				{				
					lRealPlayerWinScore += m_lUserJettonScore[nAarea+1][wChairID] * cbMultiple[nAarea];
				}
				else
				{
					lRealPlayerWinScore -= m_lUserJettonScore[nAarea+1][wChairID] * cbMultiple[nAarea];
				}
			}
		}
	}

	return lRealPlayerWinScore;
}

//�������������ׯʱ���˵ĵ÷�
LONGLONG CTableFrameSink::CalRealPlayerBankerScore()
{
	
	ASSERT (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false);

	//�Ƚϱ���
	bool static bWinRed, bWinBlack, bWinPoker;
	BYTE cbPokerType[2];
	DeduceWinner(bWinRed, bWinBlack, bWinPoker, cbPokerType);
	BYTE cbMultiple[] = { 1, 1, 1, 1 };
	if (m_bWinPoker) cbMultiple[POKER_TYPE_INDEX] = m_GameLogic.GetTimes(m_bWinRed ? m_cbPokerType[0] : m_cbPokerType[1]);

	bool static bWinFlag[AREA_COUNT + 1];
	bWinFlag[0] = false;
	bWinFlag[RED_INDEX] = bWinRed;
	bWinFlag[BLACK_INDEX] = bWinBlack;
	bWinFlag[POKER_TYPE_INDEX] = bWinPoker;

	LONGLONG lAndroidWinScore = 0L;

	// ����ϵͳ��Ӯ
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		// ��ȡ���
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		// ����ׯ��
		if (wChairID == m_wCurrentBanker || pIServerUserItem == NULL)
		{
			continue;
		}

		// ���������ע
		for (int nAarea = 1; nAarea <= AREA_COUNT; nAarea++)
		{
			if ( m_lUserJettonScore[nAarea+1][wChairID] != 0 )
			{
				if(bWinFlag[nAarea] )
				{				
					if (pIServerUserItem->IsAndroidUser() == true)
					{
						lAndroidWinScore += m_lUserJettonScore[nAarea+1][wChairID] * cbMultiple[nAarea];
					}
				}
				else
				{
					if (pIServerUserItem->IsAndroidUser() == true)
					{
						lAndroidWinScore -= m_lUserJettonScore[nAarea+1][wChairID] * cbMultiple[nAarea];
					}
				}
			}
		}
	}
	
	return -lAndroidWinScore;
}

//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pUserItem==NULL) continue;
		if(wChairID==i)
		{
			//������ע
			for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) 
			{

				if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
				{
					return true;
				}
			}
			break;
		}
	}
	if (wChairID==m_wCurrentBanker)
	{
		return true;
	}

	//����ׯ��
	if (wChairID == m_wOfflineBanker)
	{
		return true;
	}

	return false;
}

//�Ƿ�˥��
bool CTableFrameSink::NeedDeductStorage()
{

	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		if (!pIServerUserItem->IsAndroidUser())
		{
			for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
			{
				if (m_lUserJettonScore[nAreaIndex][i] != 0)
				{
					return true;
				}
			}
		}
	}

	return false;

}

//������ע��Ϣ
void CTableFrameSink::SendUserBetInfo( IServerUserItem *pIServerUserItem )
{
	if(NULL == pIServerUserItem) return;

	//Ȩ���ж�
	if(!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())) return;

	//��������
	//CMD_S_SendUserBetInfo SendUserBetInfo;
	//ZeroMemory(&SendUserBetInfo, sizeof(SendUserBetInfo));
	//SendUserBetInfo.iResult = 0;
	//CopyMemory(&SendUserBetInfo.lUserJettonScore, m_lUserJettonScore, sizeof(m_lUserJettonScore));

	//������Ϣ	
	//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_REJETTON, &SendUserBetInfo, sizeof(SendUserBetInfo));
	
	return;
}

bool CTableFrameSink::ReJetton(IServerUserItem *pIServerUserItem)
{
	//�û�Ч��

	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	LONGLONG lLastAllJettonArea = 0L;
	LONGLONG lLastAllJettonPlace = 0L;
	LONGLONG lUserJettonScore = 0L;
	//ͳ�Ʊ��������ע
	for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
	{
		lLastAllJettonArea += m_lLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()];
	}

	//ͳ���Ͼ�����ע
	for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
	{
		lLastAllJettonPlace += m_lLastAllJettonPlace[nAreaIndex][pIServerUserItem->GetChairID()];
	}

	//ͳ�Ʊ�������ע
	for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
	{
		lUserJettonScore += m_lUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()];
	}

	bool bIsLast = false;
	//���һע
	for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; ++nAreaIndex)
	{
		//�����������һע
		if (m_lLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()] > 0)
		{
			bIsLast = false;
			break;
		}
		//���������һע
		if (m_lLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()] == 0)
		{
			bIsLast = true;
		}
	}

	bool bCheckPlaceJettonIsOk = true;

	if (bIsLast) //�����һע
	{
		if (lUserScore - lUserJettonScore < lLastAllJettonPlace)
		{
			bCheckPlaceJettonIsOk = false;
		}
	}
	else
	{
		if (lUserScore - lUserJettonScore < lLastAllJettonArea)
		{
			bCheckPlaceJettonIsOk = false;
		}
	}
	if (!bCheckPlaceJettonIsOk)
	{
		//�û���Ҳ���
		CMD_S_SendBetInfo SendBetInfo;
		ZeroMemory(&SendBetInfo, sizeof(SendBetInfo));
		//�������
		SendBetInfo.iResult = 2;	//�ظ�Ͷעʧ�� ����
		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SEND_REJETTON, &SendBetInfo, sizeof(SendBetInfo));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_REJETTON, &SendBetInfo, sizeof(SendBetInfo));
		return true;
	}

	//�û�����
	if (bIsLast) //�����һע
	{
		if (lUserJettonScore + lLastAllJettonPlace > m_lUserLimitScore)
		{
			bCheckPlaceJettonIsOk = false;
		}
	}
	else
	{
		if (lUserJettonScore + lLastAllJettonArea > m_lUserLimitScore)
		{
			bCheckPlaceJettonIsOk = false;
		}
	}
	if (!bCheckPlaceJettonIsOk)
	{
		//���ڻ�������
		return true;
	}

	//��������
	if (bIsLast) //�����һע
	{
		for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
		{
			if (m_lLastAllJettonPlace[nAreaIndex][pIServerUserItem->GetChairID()] + m_lUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()] > (nAreaIndex == POKER_TYPE_INDEX ? m_lAreaLimitScore[1] : m_lAreaLimitScore[0]))
			{
				bCheckPlaceJettonIsOk = false;
				break;
			}
		}
	}
	else  //�����һע
	{
		for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
		{
			if (m_lLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()] + m_lUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()] > (nAreaIndex == POKER_TYPE_INDEX ? m_lAreaLimitScore[1] : m_lAreaLimitScore[0]))
			{
				bCheckPlaceJettonIsOk = false;
				break;
			}
		}

	}
	if (!bCheckPlaceJettonIsOk)
	{
		//������������
		return true;
	}

	//��������
	LONGLONG  TemplAllJettonScore[AREA_COUNT + 1];           //ȫ����ע
	LONGLONG  TemplUserJettonScore[AREA_COUNT + 1][GAME_PLAYER];   //������ע
	LONGLONG  TemplLastAllJettonPlace[AREA_COUNT + 1][GAME_PLAYER]; //�Ͼ�����ע
	LONGLONG  TempLastAllJettonArea[AREA_COUNT + 1][GAME_PLAYER];   //���������ע

	CopyMemory(TemplAllJettonScore, m_lAllJettonScore, sizeof(TemplAllJettonScore));
	CopyMemory(TemplUserJettonScore, m_lUserJettonScore, sizeof(m_lUserJettonScore));
	CopyMemory(TemplLastAllJettonPlace, m_lLastAllJettonPlace, sizeof(m_lLastAllJettonPlace));
	CopyMemory(TempLastAllJettonArea, m_lLastAllJettonArea, sizeof(TempLastAllJettonArea));

	//�����һע �ظ���һ������ע
	if (bIsLast == true)
	{
		if (bCheckPlaceJettonIsOk == true)
		{
			//��������
			for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
			{
				//�Ͼ�����ע ����

				//�������һ�β��� ����   0 (��Ϸ����ʱ���)

				//ȫ����ע
				TemplAllJettonScore[nAreaIndex] += TemplLastAllJettonPlace[nAreaIndex][pIServerUserItem->GetChairID()];

				//������ע
				TemplUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()] = TemplLastAllJettonPlace[nAreaIndex][pIServerUserItem->GetChairID()];

			}
		}
	}

	//�����һע �ظ��������һע
	else
	{

		//�����һע ֻ��һע
		if (bCheckPlaceJettonIsOk == true)
		{
			//��������
			for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
			{
				//�Ͼ�����ע ����     0 (�ڱ����û���һעʱ���)

				//�������һ�β��� ����

				//ȫ����ע
				TemplAllJettonScore[nAreaIndex] += TempLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()];

				//������ע
				TemplUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()] = TempLastAllJettonArea[nAreaIndex][pIServerUserItem->GetChairID()];

			}

		}
	}

	if (bCheckPlaceJettonIsOk)
	{
		//��������
		CMD_S_SendBetInfo SendBetInfo;
		ZeroMemory(&SendBetInfo, sizeof(SendBetInfo));

		//�������
		SendBetInfo.iResult = 0;	//�ظ�Ͷע�ɹ�
		//���ñ���
		//CopyMemory(m_lAllJettonScore, TemplAllJettonScore, sizeof(TemplAllJettonScore));
		//CopyMemory(m_lUserJettonScore, TemplUserJettonScore, sizeof(TemplUserJettonScore));
		//CopyMemory(m_lLastAllJettonPlace, TemplLastAllJettonPlace, sizeof(TemplLastAllJettonPlace));
		//CopyMemory(m_lLastAllJettonArea, TempLastAllJettonArea, sizeof(TempLastAllJettonArea));

		for (int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
		{
			SendBetInfo.lUserJettonScore[nAreaIndex-1] += TemplUserJettonScore[nAreaIndex][pIServerUserItem->GetChairID()];
		}

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SEND_REJETTON, &SendBetInfo, sizeof(SendBetInfo));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_REJETTON, &SendBetInfo, sizeof(SendBetInfo));
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////
