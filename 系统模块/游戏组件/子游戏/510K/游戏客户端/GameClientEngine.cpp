#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////////////
//��Ϸʱ��
#define IDI_START_GAME				200									//��ʼ��ʱ��
#define IDI_XUAN_ZHAN				201									//��ս��ʱ��
#define IDI_FIND_FRIEND				202									//��ͬ�˶�ʱ��
#define IDI_ASK_FRIEND				203									//��ͬ�˶�ʱ��
#define IDI_ADD_TIMES				204									//�ӱ���ʱ��
#define IDI_OUT_CARD					205									//���ƶ�ʱ��

//��Ϸʱ��
#define IDI_MOST_CARD				300									//���ʱ��
#define IDI_LAST_TURN				301									//���ֶ�ʱ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)

	//ϵͳ��Ϣ
	ON_WM_TIMER()

	//��Ϸ��Ϣ
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_OUT_PROMPT,OnOutPrompt)
	ON_MESSAGE(IDM_CALL_SCORE,OnCallScore)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_SORT_HAND_CARD,OnSortHandCard)
	ON_MESSAGE(IDM_LAST_TURN_CARD,OnLastTurnCard)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnTrusteeControl)
	ON_MESSAGE(IDM_XUAN_ZHAN,OnXuanZhan)
	ON_MESSAGE(IDM_FIND_FRIEND,OnFindFriend)
	ON_MESSAGE(IDM_ASK_FRIEND,OnAskFriend)
	ON_MESSAGE(IDM_ADD_TIMES,OnAddTimes)

	//������Ϣ
	ON_MESSAGE(WM_ENABLE_HISTORY,OnEnableHistory)
	ON_MESSAGE(IDM_OUT_CARD_FINISH,OnOutCardFinish)
	ON_MESSAGE(IDM_DISPATCH_FINISH,OnDispatchFinish)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientEngine::CGameClientEngine()
{
	//��������
	m_bTrustee=false;
	m_bLastTurn=false;
	m_cbSortType=ST_ORDER;
	m_bBackGroundSound = false;


	//ʱ�䶨��
	m_cbTimeOutCard=0;
	m_cbTimeXuanZhan=0;
	m_cbTimeFindFriend=0;
	m_cbTimeAskFriend=0;
	m_cbTimeAddTimes=0;

	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wMostCardUser=INVALID_CHAIR;
	m_cbFriendFlag=0;
	m_bSelfDoneAddTimes=false;
	//��������
	m_cbSearchResultIndex = 0;
	ZeroMemory( &m_SearchCardResult,sizeof(m_SearchCardResult) );
	m_nCurSearchType = -1;
	m_cbEachSearchIndex = 0;
	ZeroMemory( &m_EachSearchResult,sizeof(m_EachSearchResult) );

	//���Ʊ���
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//�˿˱���
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
	
	return;
}

//��������
CGameClientEngine::~CGameClientEngine()
{
}

//��������
bool CGameClientEngine::OnInitGameEngine()
{
	//��������
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	//��������
	CGlobalUnits *pGlobalUnits=CGlobalUnits::GetInstance();
	if ( pGlobalUnits->m_bAllowBackGroundSound )
	{
		m_bBackGroundSound = true;
		PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_MUSIC"));
	}


	return true;
}

//���ú���
bool CGameClientEngine::OnResetGameEngine()
{
	//ɾ��ʱ��
	KillTimer(IDI_MOST_CARD);
	KillTimer(IDI_LAST_TURN);

	//��������
	m_bTrustee=false;
	m_bLastTurn=false;
	m_cbSortType=ST_ORDER;


	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wMostCardUser=INVALID_CHAIR;

	//��������
	m_cbSearchResultIndex = 0;
	ZeroMemory( &m_SearchCardResult,sizeof(m_SearchCardResult) );
	m_nCurSearchType = -1;
	m_cbEachSearchIndex = 0;
	ZeroMemory( &m_EachSearchResult,sizeof(m_EachSearchResult) );


	//�˿˱���
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//���Ʊ���
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	return true;
}

//ʱ��ɾ��
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//ʱ����Ϣ
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	switch (wClockID)
	{
	case IDI_START_GAME:	//��Ϸ��ʼ
		{
			//�ر���Ϸ
			if ( nElapse <= 0 && !IsLookonMode() && wChairID == GetMeChairID() )
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//��ʱ����
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{				
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_XUAN_ZHAN:	//�û���ս
		{
			//�Զ�����
			if (( nElapse <= 0 ) && wChairID == GetMeChairID())
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//��ʱ����
			if ((nElapse<=5 && nElapse >0)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_FIND_FRIEND:	//��ͬ��
		{
			//�Զ�����
			if (( nElapse <= 0 ) && wChairID == GetMeChairID())
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//��ʱ����
			if ((nElapse<=5 && nElapse >0)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_ASK_FRIEND:	//��ͬ��
		{
			//�Զ�����
			if (( nElapse <= 0 ) && wChairID == GetMeChairID())
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//��ʱ����
			if ((nElapse<=5 && nElapse >0)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_ADD_TIMES:	//�ӱ�
		{
			//�Զ�����
			if (( nElapse <= 0 ) && wChairID == GetMeChairID())
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//��ʱ����
			if ((nElapse<=5 && nElapse >0)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_OUT_CARD:		//�û�����
		{
			if (IsLookonMode()) return true;

			//�Զ�����
			if (wChairID == GetMeChairID() &&	nElapse <= 0)
			{
				m_GameClientView.SetUserCountWarn(ME_VIEW_ID,FALSE);
				//�й�
				if (!m_bTrustee && nElapse == 0)
				{
					OnTrusteeControl(0,0);
				}
				return true;
			}

			//��ʱ����
			if ((nElapse<=5 && nElapse >0)&&(wChairID==GetMeChairID())) 
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	}
	return false;
}

//�Թ���Ϣ
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//�Թۿ���
	bool bAllowLookon=IsAllowLookon();

	//�˿˿���
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetDisplayItem(bAllowLookon);

	m_GameClientView.m_btSortCard.EnableWindow(bAllowLookon?TRUE:FALSE);
	if( bAllowLookon )
		UpdateButtonControl();

	if ( IsLookonMode() )
	{
		m_GameClientView.m_btCancelTrustee.EnableWindow(FALSE);
		m_GameClientView.m_btTrustee.EnableWindow(FALSE);
		m_GameClientView.m_btLastTurn.EnableWindow(FALSE);
		m_GameClientView.m_btSortCard.EnableWindow(FALSE);
	}

	//���½���
	m_GameClientView.InvalidGameView(0,0,0,0);


	return true;
}

//��Ϸ��Ϣ
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_XUAN_ZHAN:		//�û���ս
		{
			return OnSubXuanZhan(pData,wDataSize);
		}
	case SUB_S_FIND_FRIEND:		//��ͬ��
		{
			return OnSubFindFriend(pData,wDataSize);
		}
	case SUB_S_ASK_FRIEND:		//��ͬ��
		{
			return OnSubAskFriend(pData,wDataSize);
		}
	case SUB_S_ADD_TIMES:		//�ӱ�
		{
			return OnSubAddTimes(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//����
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_PASS_CARD:		//�û�����
		{
			return OnSubPassCard(pData,wDataSize);
		}
	case SUB_S_TRUSTEE:
		{
			return OnSubTrustee(pData,wDataSize);
		}
	case SUB_S_FRIEND:
		{
			return OnSubFriend(pData,wDataSize);
		}
	case SUB_S_GAME_CONCLUDE:	//��Ϸ����
		{
			return OnSubGameConclude(pData,wDataSize);
		}
	case SUB_S_SET_BASESCORE:	//���û���
		{
			//Ч������
			ASSERT(wDataSize==sizeof(LONG));
			if(wDataSize!=sizeof(LONG)) return false;

			LONG lCellScore=*(LONG*)pData;
			m_GameClientView.SetCellScore(lCellScore);
			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//������Ϣ
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_GAME_FREE:	//����״̬
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_S_StatusFree));
			if (wDataSize != sizeof(CMD_S_StatusFree))
				return false;

			//��������
			CMD_S_StatusFree* pStatusFree = (CMD_S_StatusFree*)pData;

			//���ý���
			//m_GameClientView.m_btScore.ShowWindow(true);
			m_GameClientView.SetCellScore(pStatusFree->lCellScore);
			m_GameClientView.m_b2Biggest=pStatusFree->b2Biggest;
			m_GameClientView.m_GameLogic.Set2Biggest(pStatusFree->b2Biggest);
			m_GameLogic.Set2Biggest(pStatusFree->b2Biggest);
			//ʱ�䶨��
			m_cbTimeStartGame=pStatusFree->cbTimeStart;
			m_cbTimeOutCard=pStatusFree->cbTimeOutCard;
			m_cbTimeXuanZhan=pStatusFree->cbTimeXuanZhan;
			m_cbTimeFindFriend=pStatusFree->cbTimeFindFriend;
			m_cbTimeAskFriend=pStatusFree->cbTimeAskFriend;
			m_cbTimeAddTimes=pStatusFree->cbTimeAddTimes;
			
			//�������
			if (IsLookonMode()==false)
			{
				//�йܰ�ť
				m_GameClientView.m_btTrustee.ShowWindow(true);
				m_GameClientView.m_btCancelTrustee.ShowWindow(false);
				m_GameClientView.m_btTrustee.EnableWindow(false);

				//��ʼ����
				if (GetMeUserItem()->GetUserStatus()!=US_READY)
				{
					m_GameClientView.m_btStart.ShowWindow(TRUE);
					WORD wMeChairID=GetMeChairID();
					SetGameClock(wMeChairID,IDI_START_GAME,m_cbTimeStartGame);
				}
			}

			//��ʷ�ɼ�
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//��ȡ����
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//���û���
				pHistoryScore->lTurnScore=pStatusFree->lTurnScore[i];
				pHistoryScore->lCollectScore=pStatusFree->lCollectScore[i];

				//������
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}

			//�Թ����ý���
			if ( IsLookonMode() )
			{
				m_GameClientView.m_btCancelTrustee.EnableWindow(FALSE);
				m_GameClientView.m_btTrustee.EnableWindow(FALSE);
				m_GameClientView.m_btLastTurn.EnableWindow(FALSE);
				m_GameClientView.m_btSortCard.EnableWindow(FALSE);
			}

			return true;
		}
	case GS_XUAN_ZHAN:
	case GS_FIND_FRIEND:
	case GS_ASK_FRIEND:
	case GS_ADD_TIMES:
	case GS_PLAY:	//��Ϸ״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//��������
			WORD wMeChairID=GetMeChairID();
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//���ñ���
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbFriendFlag=pStatusPlay->cbFriendFlag;
			CopyMemory(m_wFriend,pStatusPlay->wFriend,sizeof(m_wFriend));
			m_wXuanZhanUser=pStatusPlay->wXuanZhanUser;
			m_GameClientView.SetCellScore(pStatusPlay->lCellScore);
			m_GameClientView.m_b2Biggest=pStatusPlay->b2Biggest;
			m_GameClientView.m_GameLogic.Set2Biggest(pStatusPlay->b2Biggest);
			m_GameClientView.m_cbSelectedCardData = pStatusPlay->cbSelectedCardData;
			m_GameLogic.Set2Biggest(pStatusPlay->b2Biggest);
			//ʱ�䶨��
			m_cbTimeStartGame=pStatusPlay->cbTimeStart;
			m_cbTimeOutCard=pStatusPlay->cbTimeOutCard;
			m_cbTimeXuanZhan=pStatusPlay->cbTimeXuanZhan;
			m_cbTimeFindFriend=pStatusPlay->cbTimeFindFriend;
			m_cbTimeAskFriend=pStatusPlay->cbTimeAskFriend;
			m_cbTimeAddTimes=pStatusPlay->cbTimeAddTimes;

			//�˿�����
			CopyMemory(m_cbHandCardCount,pStatusPlay->cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(m_cbHandCardData,pStatusPlay->cbHandCardData,m_cbHandCardCount[wMeChairID]*sizeof(BYTE));

			WORD wViewChairID=SwitchViewChairID(m_wBankerUser);

			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//��ȡλ��
				WORD wViewChairID=SwitchViewChairID(i);

				//�����˿�
				if (wViewChairID==ME_VIEW_ID)
				{
					if(m_GameLogic.IsHave510K(m_cbHandCardData,m_cbHandCardCount[i]))
						m_cbSortType = ST_510K;
					m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[i],m_cbSortType,true);
					
					m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[i]);
					m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayItem(!IsLookonMode());
				}
				else m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardCount[i]);

			}

			//ׯ������
			m_GameClientView.SetBankerUser(wViewChairID);
			if(pStatusPlay->cbGameStatus == GS_XUAN_ZHAN)
			{
				SetGameClock(m_wCurrentUser,IDI_XUAN_ZHAN,m_cbTimeXuanZhan);
				if(!IsLookonMode() && m_wCurrentUser == GetMeChairID())
				{
					m_GameClientView.m_btXuan.ShowWindow(true);
					m_GameClientView.m_btNoXuan.ShowWindow(true);
				}
				else
					m_GameClientView.m_bWaitXuanZhan=true;
			}
			else if(pStatusPlay->cbGameStatus == GS_FIND_FRIEND)
			{
				SetGameClock(m_wCurrentUser,IDI_FIND_FRIEND,m_cbTimeFindFriend);
				if(!IsLookonMode() && m_wCurrentUser == GetMeChairID())
				{
					m_GameClientView.ShowFindFriend(true);
				}
				else
					m_GameClientView.m_bWaitFindFriend=true;
			}
			else if(pStatusPlay->cbGameStatus == GS_ASK_FRIEND)
			{
				SetGameClock(m_wCurrentUser,IDI_ASK_FRIEND,m_cbTimeAskFriend);
				if(!IsLookonMode() && m_wCurrentUser == GetMeChairID())
				{
					if(m_cbFriendFlag==5 && m_wFriend[m_wBankerUser] == GetMeChairID())
					{
						m_GameClientView.m_btRespond.ShowWindow(TRUE);
						m_GameClientView.m_btNoRespond.ShowWindow(TRUE);
					}
					else
					{
						m_GameClientView.m_btAsk.ShowWindow(true);
						m_GameClientView.m_btNoAsk.ShowWindow(true);
						m_GameClientView.m_btAsk.EnableWindow(pStatusPlay->bEnabledAskFriend);
					}

				}
				else
					m_GameClientView.m_bWaitAskFriend=true;

				if(GetMeChairID() == m_wBankerUser || m_wFriend[m_wBankerUser]==GetMeChairID())
					SetFriendFlag();
			}
			else if(pStatusPlay->cbGameStatus == GS_ADD_TIMES)
			{
				if(pStatusPlay->bEnabledAddTimes)
				{
					SetGameClock(GetMeChairID(),IDI_ADD_TIMES,m_cbTimeAddTimes);
					m_GameClientView.m_btAdd.ShowWindow(true);
					m_GameClientView.m_btNoAdd.ShowWindow(true);
				}
				else
					m_GameClientView.m_bWaitAddTimes=true;

				for(WORD i=0;i<GAME_PLAYER;i++)
				{
					if(pStatusPlay->bAddTimes[i])
						m_GameClientView.SetAddTimes(SwitchViewChairID(i),pStatusPlay->bAddTimes[i]);
				}
				SetFriendFlag();
			}
			else if(pStatusPlay->cbGameStatus == GS_PLAY)
			{
				//���Ʊ���
				m_cbTurnCardCount=pStatusPlay->cbTurnCardCount;
				CopyMemory(m_cbTurnCardData,pStatusPlay->cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));
				
				m_GameClientView.SetAddTimes(INVALID_CHAIR,false);
				m_GameClientView.SetAsk(INVALID_CHAIR,false);
				m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);

				//���ƽ���
				if (pStatusPlay->wTurnWiner!=INVALID_CHAIR)
				{
					WORD wViewChairID=SwitchViewChairID(pStatusPlay->wTurnWiner);
					m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbTurnCardData,m_cbTurnCardCount);
				}

				//��ҿ���
				if (IsLookonMode()==false )	
				{
					m_GameClientView.m_btTrustee.ShowWindow(true);
					m_GameClientView.m_btCancelTrustee.ShowWindow(false);
					m_GameClientView.m_btTrustee.EnableWindow(true);
					m_GameClientView.m_btSortCard.EnableWindow(true);

					UpdateButtonControl();
				}

				//��������
				bool bPositively=(IsLookonMode()==false);
				bool bDisplayItem=((IsLookonMode()==false)||(IsAllowLookon()==true));
				m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetPositively(bPositively);
				m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetDisplayItem(bDisplayItem);

				//��ǰ���
				if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
				{
					//��ʾ��ť
					m_GameClientView.m_btOutCard.ShowWindow(true);
					m_GameClientView.m_btPassCard.ShowWindow(true);
					m_GameClientView.m_btOutPrompt.ShowWindow(true);

					//���ð�ť
					m_GameClientView.m_btPassCard.EnableWindow((m_cbTurnCardCount>0)?true:false);
					m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?true:false);

					//��������
					if( pStatusPlay->wTurnWiner == wMeChairID )
					{
						m_GameLogic.SearchOutCard(m_cbHandCardData, m_cbHandCardCount[wMeChairID], NULL, 0, &m_SearchCardResult);
					}
					else
					{
						m_GameLogic.SearchOutCard(m_cbHandCardData, m_cbHandCardCount[wMeChairID], m_cbTurnCardData, m_cbTurnCardCount, &m_SearchCardResult);
					}
				}

				//��ʷ�ɼ�
				for (BYTE i=0;i<GAME_PLAYER;i++)
				{
					//��ȡ����
					tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

					//���û���
					pHistoryScore->lTurnScore=pStatusPlay->lTurnScore[i];
					pHistoryScore->lCollectScore=pStatusPlay->lCollectScore[i];

					//������
					WORD wViewChairID=SwitchViewChairID(i);
					m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
					m_GameClientView.SetTrusteeFlag(wViewChairID,pStatusPlay->bTrustee[i]);
				}

				//����ʱ��
				SetGameClock(m_wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);

				//�����ǰ����
				m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
				m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);

				SetFriendFlag();
			}

			for(int i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetGameScore(SwitchViewChairID(i),pStatusPlay->cbGameScore[i]);
				m_GameClientView.SetTurnScore(pStatusPlay->cbTunrScore);
			}

			//�����Թ۽���
			if ( IsLookonMode() )
			{
				m_GameClientView.m_btCancelTrustee.EnableWindow(FALSE);
				m_GameClientView.m_btTrustee.EnableWindow(FALSE);
				m_GameClientView.m_btLastTurn.EnableWindow(FALSE);
				m_GameClientView.m_btSortCard.EnableWindow(FALSE);
			}

			m_GameClientView.InvalidGameView(0,0,0,0);

			return true;
		}
	}

	//�������
	ASSERT(FALSE);
	return false;
}

//��Ϸ��ʼ
bool CGameClientEngine::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��������
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//����״̬
	SetGameStatus(GS_XUAN_ZHAN);

	//��������
	m_cbSearchResultIndex = 0;
	ZeroMemory( &m_SearchCardResult,sizeof(m_SearchCardResult) );
	m_nCurSearchType = -1;
	m_cbEachSearchIndex = 0;
	ZeroMemory( &m_EachSearchResult,sizeof(m_EachSearchResult) );

	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_wBankerUser=pGameStart->wBanker;
	//���Ʊ���
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//��ʷ��¼
	m_HistoryCard.ResetData();

	//����ׯ��
	m_GameClientView.SetBankerUser(INVALID_CHAIR);

	//�ɼ�����
	m_GameClientView.m_ScoreControl.CloseControl();

	//״̬����
	m_GameClientView.SetUserPassState(INVALID_CHAIR,false);
	m_GameClientView.SetUserCountWarn(INVALID_CHAIR,false);

	//����˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��ȡλ��
		WORD wViewChairID=SwitchViewChairID(i);

		//�˿˱���
		m_GameClientView.m_HandCardControl[i].SetBackGround(CARD_BOOR);
		m_GameClientView.m_UserCardControl[i].SetBackGround(CARD_BOOR);

		//�����˿�
		if (wViewChairID==ME_VIEW_ID)
		{
			bool bShowCard=((IsLookonMode()==false)||(IsAllowLookon()==true));
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetPositively(false);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayItem(bShowCard);
		}
		else
		{
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayItem(false);
		}
	}

	//�����˿�
	WORD wMeChairID=GetMeChairID();
	for (WORD i=0;i<GAME_PLAYER;i++) 
		m_cbHandCardCount[i]=NORMAL_CARD_COUNT;

	CopyMemory(m_cbHandCardData,pGameStart->cbCardData,sizeof(m_cbHandCardData));

	//��������
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//���ƶ���
	WORD wViewStartUser=SwitchViewChairID(0);
	m_GameClientView.ShowDispatchCard(m_cbHandCardData, NORMAL_CARD_COUNT, wViewStartUser);

	//��������
	if ((IsLookonMode()==false)&&(m_bTrustee==false)) ActiveGameFrame();

	//���½���
	m_GameClientView.RefreshGameView();
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

	return true;
}
bool CGameClientEngine::OnSubXuanZhan(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_XuanZhan));
	if (wDataSize!=sizeof(CMD_S_XuanZhan)) return false;

	//ֹͣ����
	m_GameClientView.StopDispatchCard();
	//�����˿�
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],ST_ORDER,true);
	//�����˿�
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetCardData(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()]);
	
	
	KillGameClock(IDI_XUAN_ZHAN);

	//��������
	CMD_S_XuanZhan * pPack=(CMD_S_XuanZhan *)pData;

	m_GameClientView.SetXuanZhan(SwitchViewChairID(pPack->wXuanZhanUser),pPack->bXuanZhan);
	m_wCurrentUser = pPack->wCurrentUser;
	m_cbFriendFlag=pPack->cbFriendFlag;
	if(pPack->bXuanZhan)
	{
		m_wXuanZhanUser = pPack->wXuanZhanUser;
		//����ӱ�״̬
		SetGameStatus(GS_ADD_TIMES);

		if(m_wXuanZhanUser != GetMeChairID())
		{
			SetGameClock(GetMeChairID(),IDI_ADD_TIMES,m_cbTimeAddTimes);
			if (m_bTrustee==false) ActiveGameFrame();
			m_GameClientView.m_btAdd.ShowWindow(TRUE);
			m_GameClientView.m_btNoAdd.ShowWindow(TRUE);
		}
		else
		{
			m_GameClientView.m_bWaitAddTimes=true;
		}


		m_GameClientView.m_bWaitXuanZhan=false;

		SetFriendFlag();
	}
	else
	{
		if(pPack->wCurrentUser == m_wBankerUser)
		{
			//������ͬ��״̬
			SetGameStatus(GS_FIND_FRIEND);
			SetGameClock(m_wBankerUser,IDI_FIND_FRIEND,m_cbTimeFindFriend);
			if(IsLookonMode()==FALSE && m_wBankerUser == GetMeChairID())
			{
				//�ҿ�ʼ������
				if (m_bTrustee==false) ActiveGameFrame();
				m_GameClientView.ShowFindFriend(true);
				m_GameClientView.m_bWaitFindFriend=false;
			}
			else
			{
				//�ȴ��������������
				m_GameClientView.m_bWaitFindFriend=true;
			}

			m_GameClientView.m_bWaitXuanZhan=false;
		}
		else
		{
			SetGameClock(m_wCurrentUser,IDI_XUAN_ZHAN,m_cbTimeXuanZhan);
			if(IsLookonMode() == FALSE && m_wCurrentUser == GetMeChairID())
			{
				if (m_bTrustee==false) ActiveGameFrame();
				m_GameClientView.m_btXuan.ShowWindow(TRUE);
				m_GameClientView.m_btNoXuan.ShowWindow(TRUE);
				m_GameClientView.m_bWaitXuanZhan=false;
			}
			else
			{
				//�ȴ����������ս
				m_GameClientView.m_bWaitXuanZhan=true;
			}
		}
	}


	return true;
}

bool CGameClientEngine::OnSubFindFriend(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_FindFriend));
	if (wDataSize!=sizeof(CMD_S_FindFriend)) return false;

	KillGameClock(IDI_FIND_FRIEND);
	m_GameClientView.m_bWaitFindFriend=false;
	//��������
	CMD_S_FindFriend * pPack=(CMD_S_FindFriend *)pData;
	m_wCurrentUser = pPack->wCurrentUser;
	m_GameClientView.m_cbSelectedCardData = pPack->cbSelectedCardData;
	SetGameStatus(GS_ASK_FRIEND);
	SetGameClock(m_wCurrentUser,IDI_ASK_FRIEND,m_cbTimeAskFriend);

	if(IsLookonMode()==FALSE && m_wCurrentUser == GetMeChairID())
	{
		if (m_bTrustee==false) ActiveGameFrame();
		m_GameClientView.m_btAsk.ShowWindow(TRUE);
		m_GameClientView.m_btNoAsk.ShowWindow(TRUE);
		m_GameClientView.m_btAsk.EnableWindow(pPack->bEnabled);
	}
	else
	{
		m_GameClientView.m_bWaitAskFriend=true;
	}

	if(pPack->wSelectedUser==GetMeChairID())
	{
		m_cbFriendFlag=5;
		CopyMemory(m_wFriend,pPack->wFriend,sizeof(m_wFriend));
		SetFriendFlag();
	}

	m_GameClientView.m_bWaitFindFriend=false;

	return true;
}

bool CGameClientEngine::OnSubAskFriend(VOID * pData, WORD wDataSize)
{	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_ASKFriend));
	if (wDataSize!=sizeof(CMD_S_ASKFriend)) return false;

	KillGameClock(IDI_ASK_FRIEND);
	m_GameClientView.m_bWaitAskFriend=false;
	m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);
	//��������
	CMD_S_ASKFriend * pPack=(CMD_S_ASKFriend *)pData;
	m_wCurrentUser = pPack->wCurrentUser;
	m_cbFriendFlag=pPack->cbFriendFlag;
	m_GameClientView.SetAsk(SwitchViewChairID(pPack->wAskUser),pPack->bAsk,pPack->wAskUser==pPack->wFriend[m_wBankerUser]);
	if(pPack->bAsk)
	{
		CopyMemory(m_wFriend,pPack->wFriend,sizeof(m_wFriend));
		m_wXuanZhanUser = pPack->wXuanZhanUser;
		m_wAskUser = pPack->wAskUser;
		SetFriendFlag();

		if(pPack->bStart == false)
		{
			//�������ˣ�����ӱ�
			SetGameStatus(GS_ADD_TIMES);

			if(IsLookonMode()==FALSE && m_wCurrentUser == GetMeChairID())
			{
				SetGameClock(m_wCurrentUser,IDI_ADD_TIMES,m_cbTimeAddTimes);
				if (m_bTrustee==false) ActiveGameFrame();
				m_GameClientView.m_btAdd.ShowWindow(TRUE);
				m_GameClientView.m_btNoAdd.ShowWindow(TRUE);
			}
			else
				m_GameClientView.m_bWaitAddTimes=true;
		}
		else//ֱ�ӿ�ʼ
		{
			SetGameStatus(GS_PLAY);
			SetGameClock(m_wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);
			if(IsLookonMode()==FALSE && m_wCurrentUser == GetMeChairID())
			{
				if (m_bTrustee==false) ActiveGameFrame();
				m_GameClientView.m_btOutCard.ShowWindow(TRUE);
				m_GameClientView.m_btPassCard.ShowWindow(TRUE);
				m_GameClientView.m_btOutPrompt.ShowWindow(TRUE);

				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btPassCard.EnableWindow(FALSE);
				m_GameClientView.m_btOutPrompt.EnableWindow(TRUE);
			}
			m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetPositively(true);

			m_GameClientView.m_bWaitAddTimes=false;
			m_GameClientView.m_bWaitXuanZhan=false;
			m_GameClientView.m_bWaitFindFriend=false;
			m_GameClientView.m_bWaitAskFriend=false;

			//���������й�
			m_GameClientView.m_btTrustee.EnableWindow(true);
		}


		m_GameClientView.m_bWaitAskFriend=false;
	}
	else
	{
		if(m_wCurrentUser == m_wBankerUser)
		{
			SetFriendFlag();
			//��û���ʣ�����
			SetGameStatus(GS_PLAY);
			SetGameClock(m_wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);
			if(IsLookonMode()==FALSE && m_wCurrentUser == GetMeChairID())
			{
				if (m_bTrustee==false) ActiveGameFrame();
				m_GameClientView.m_btOutCard.ShowWindow(TRUE);
				m_GameClientView.m_btPassCard.ShowWindow(TRUE);
				m_GameClientView.m_btOutPrompt.ShowWindow(TRUE);

				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btPassCard.EnableWindow(FALSE);
				m_GameClientView.m_btOutPrompt.EnableWindow(TRUE);
			}
			m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetPositively(true);

			m_GameClientView.m_bWaitAddTimes=false;
			m_GameClientView.m_bWaitXuanZhan=false;
			m_GameClientView.m_bWaitFindFriend=false;
			m_GameClientView.m_bWaitAskFriend=false;

			//���������й�
			m_GameClientView.m_btTrustee.EnableWindow(true);
		}
		else
		{
			SetGameClock(m_wCurrentUser,IDI_ASK_FRIEND,m_cbTimeAskFriend);
			if(IsLookonMode()==FALSE && m_wCurrentUser == GetMeChairID())
			{
				if (m_bTrustee==false) ActiveGameFrame();
				if(pPack->cbFriendFlag == 5 && pPack->wFriend[m_wBankerUser]==GetMeChairID())
				{
					m_GameClientView.m_btRespond.ShowWindow(TRUE);
					m_GameClientView.m_btNoRespond.ShowWindow(TRUE);
				}
				else
				{
					m_GameClientView.m_btAsk.ShowWindow(TRUE);
					m_GameClientView.m_btNoAsk.ShowWindow(TRUE);
				}

				m_GameClientView.m_bWaitAskFriend=false;
			}
			else
			{
				m_GameClientView.m_bWaitAskFriend=true;
			}
		}

	}

	return true;
}
//�û��ӱ�
bool CGameClientEngine::OnSubAddTimes(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_AddTimes));
	if (wDataSize!=sizeof(CMD_S_AddTimes)) return false;

	m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);
	m_GameClientView.SetAsk(INVALID_CHAIR,false);
	//��������
	CMD_S_AddTimes * pAddTimes=(CMD_S_AddTimes *)pData;

	if(pAddTimes->wAddTimesUser != INVALID_CHAIR)
	{
		//���ò���
		WORD wViewChairID=SwitchViewChairID(pAddTimes->wAddTimesUser);
		m_GameClientView.SetAddTimes(wViewChairID,pAddTimes->bAddTimes);
	}
	else
	{
		if(IsLookonMode() == false)
		{
			ActiveGameFrame();
			SetGameClock(GetMeChairID(),IDI_ADD_TIMES,m_cbTimeAddTimes);
			m_GameClientView.m_btAdd.ShowWindow(true);
			m_GameClientView.m_btNoAdd.ShowWindow(true);
			m_GameClientView.m_bWaitAddTimes=false;
		}

	}
	
	//����ʱ��
	if (pAddTimes->wCurrentUser!=INVALID_CHAIR)
	{
		//������
		KillGameClock(IDI_ADD_TIMES);
		SetGameStatus(GS_PLAY);
		SetGameClock(pAddTimes->wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);

		m_GameClientView.InvalidGameView(0,0,0,0);
		m_wCurrentUser=pAddTimes->wCurrentUser;
		if ((IsLookonMode()==false)&&(pAddTimes->wCurrentUser==GetMeChairID()))
		{
			//������
			ActiveGameFrame();

			//��ʾ��ť
			m_GameClientView.m_btPassCard.ShowWindow(true);
			m_GameClientView.m_btOutCard.ShowWindow(true);
			m_GameClientView.m_btOutPrompt.ShowWindow(true);
			//���ð�ť
			m_GameClientView.m_btPassCard.EnableWindow(false);
			m_GameClientView.m_btOutCard.EnableWindow(false);
			m_GameClientView.m_btOutPrompt.EnableWindow(true);

			//������ʾ
			try
			{
				m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount[m_wCurrentUser],m_cbTurnCardData,m_cbTurnCardCount,&m_SearchCardResult);
			}catch(...)
			{
				ASSERT(FALSE);
				m_SearchCardResult.cbSearchCount = 0;
			}
		}

		m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetPositively(true);
		m_GameClientView.m_bWaitAddTimes=false;
		m_GameClientView.m_bWaitXuanZhan=false;
		m_GameClientView.m_bWaitFindFriend=false;
		m_GameClientView.m_bWaitAskFriend=false;

		//���������й�
		m_GameClientView.m_btTrustee.EnableWindow(true);
	}

	return true;
}

//�û�����
bool CGameClientEngine::OnSubOutCard(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;
	//��������
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);

	//��������
	WORD wMeChairID=GetMeChairID();
	WORD wViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);
	m_GameClientView.SetTurnScore(pOutCard->cbCurTurnScore);
	BYTE cbTurnCardType=m_GameLogic.GetCardType(pOutCard->cbCardData,pOutCard->cbCardCount);

	//ɾ��ʱ��
	KillTimer(IDI_MOST_CARD);
	KillGameClock(IDI_OUT_CARD);

	if(wMeChairID == pOutCard->wOutCardUser)
	{
		m_GameClientView.m_btOutCard.ShowWindow(false);
		m_GameClientView.m_btPassCard.ShowWindow(false);
		m_GameClientView.m_btOutPrompt.ShowWindow(false);
		m_GameClientView.m_btOutCard.EnableWindow(false);
		m_GameClientView.m_btPassCard.EnableWindow(false);
	}
	//��ʷ����
	if (m_cbTurnCardCount==0)
	{
		//�û��˿�
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//��������
			m_HistoryCard.OnEventSaveData(i);

			//��������
			if (i!=pOutCard->wOutCardUser)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(0);
			}
		}

		//������־
		m_GameClientView.SetUserPassState(INVALID_CHAIR,false);
		m_GameClientView.SetAddTimes(INVALID_CHAIR,false);
		m_GameClientView.SetAsk(INVALID_CHAIR,false);
		m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);
	}
	
	//���¼���������
	if(IsLookonMode() == false)
	{
		m_GameClientView.m_RememberCardWnd.UpdateCardCount(pOutCard->cbCardData,pOutCard->cbCardCount);
	}
	

	//���ƶ���
	if ((IsLookonMode()==true)||(pOutCard->wOutCardUser!=wMeChairID) || pOutCard->bTrusteeOut)
	{
		//��ǰ�˿�
		SwitchToCurrentCard();

		//��������
		BYTE cbCardType=m_GameLogic.GetCardType(pOutCard->cbCardData,pOutCard->cbCardCount);
		if ( m_cbTurnCardCount != 0 
			&& !(cbCardType == CT_THREE_LINE_TAKE_TWO)
			&& !(cbCardType == CT_THREE_TAKE_TWO)
			&& !(cbCardType == CT_BOMB_CARD))
		{
			//��������
			LPCTSTR pszSoundBoy=TEXT("OUT_CARD_BOY");
			LPCTSTR pszSoundGirl=TEXT("OUT_CARD_GIRL");
			IClientUserItem * pIClientUserItem=GetTableUserItem(pOutCard->wOutCardUser);

			//��������
			BYTE cbGender=pIClientUserItem->GetGender();
			PlayGameSound(AfxGetInstanceHandle(),(cbGender==GENDER_FEMALE)?pszSoundGirl:pszSoundBoy);
		}
		else
		{
			//��������
			PlayOutCardSound(pOutCard->wOutCardUser,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
		//��������
		if (cbCardType >= CT_510K_FALSE)
		{
			m_GameClientView.ShowCartoonBomb(true);
		}
		else if (cbCardType >= CT_THREE_LINE_TAKE_ONE) 
		{
			//m_GameClientView.ShowCartoonThreeTakeLine(true);
			m_GameClientView.ShowCartoonPlan(true);
		}
		else if(cbCardType == CT_DOUBLE_LINE)
		{
			m_GameClientView.ShowCartoonDoubleLine(true);
		}
		else if(cbCardType == CT_THREE_LINE || cbCardType == CT_THREE)
		{
			m_GameClientView.ShowCartoonThreeLine(true);
		}
		else if(cbCardType == CT_THREE_TAKE_ONE || cbCardType == CT_THREE_TAKE_TWO)
		{
			m_GameClientView.ShowCartoonThreeTake(true);
		}

		//������ʷ
		m_HistoryCard.OnEventUserOutCard(pOutCard->wOutCardUser,pOutCard->cbCardData,pOutCard->cbCardCount);

		//��ʾ����
		WORD wViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);
		m_GameClientView.m_UserCardControl[wViewChairID].ShowOutCard(pOutCard->cbCardData,pOutCard->cbCardCount);
		
		//ɾ���˿�
		if (pOutCard->wOutCardUser==wMeChairID)
		{
			//ɾ���˿�
			BYTE cbSourceCount=m_cbHandCardCount[wMeChairID];
			m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
			m_GameLogic.RemoveCardList(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbHandCardData,cbSourceCount);

			//�����˿�
			m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],m_cbSortType,true);

			//�����˿�
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);
		}
		else
		{
			//�����˿�
			m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardCount[pOutCard->wOutCardUser]);
		}
	}

	//���Ʊ���
	m_wCurrentUser=pOutCard->wCurrentUser;
	m_cbTurnCardCount=pOutCard->cbCardCount;
	CopyMemory(m_cbTurnCardData,pOutCard->cbCardData,pOutCard->cbCardCount*sizeof(BYTE));

	//���ƾ���
	if ( m_cbHandCardCount[pOutCard->wOutCardUser] <= 2 && m_cbHandCardCount[pOutCard->wOutCardUser] > 0 )
	{
		//���ñ���
		m_GameClientView.SetUserCountWarn(wViewChairID,true);

		//���ű���
		PlayGameSound(AfxGetInstanceHandle(),TEXT("COUNT_WARN"));
	}
	else
		m_GameClientView.SetUserCountWarn(wViewChairID,false);

	//����ж�
	if (m_wCurrentUser==pOutCard->wOutCardUser)
	{
		//�����û�
		m_wCurrentUser=INVALID_CHAIR;
		m_wMostCardUser=pOutCard->wCurrentUser;

		//���Ʊ���
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

		//��������
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//�û�����
			if (pOutCard->wOutCardUser==i) continue;

			//������¼
			m_HistoryCard.OnEventSaveData(i);
			m_HistoryCard.OnEventUserPass(i);

			//��������
			WORD wViewChairID=SwitchViewChairID(i);
			m_GameClientView.SetUserPassState(wViewChairID,true);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(0);
		}

		//����ʱ��
		SetTimer(IDI_MOST_CARD,3000,NULL);
	}

	//���½���
	m_GameClientView.RefreshGameView();

	return true;
}

//�û�����
bool CGameClientEngine::OnSubPassCard(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PassCard));
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;

	//��������
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pData;

	//���ñ���
	m_wCurrentUser=pPassCard->wCurrentUser;

	//��������
	if ((IsLookonMode()==true)||(pPassCard->wPassCardUser!=GetMeChairID()) || pPassCard->bTrusteePass)
	{
		//��ǰ�˿�
		SwitchToCurrentCard();

		//ɾ��ʱ��
		KillGameClock(IDI_OUT_CARD);

		//��ʷ��¼
		m_HistoryCard.OnEventUserPass(pPassCard->wPassCardUser);

		//��������
		m_GameClientView.SetUserPassState(SwitchViewChairID(pPassCard->wPassCardUser),true);

		//��������
		IClientUserItem * pClientUserItem=GetTableUserItem(pPassCard->wPassCardUser);
		PlayGameSound(AfxGetInstanceHandle(),(pClientUserItem->GetGender()==GENDER_FEMALE)?TEXT("PASS_CARD_GIRL"):TEXT("PASS_CARD_BOY"));
	}

	//�������
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//��������
		m_HistoryCard.OnEventSaveData(m_wCurrentUser);

		//��������
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);

		//��ǰ����
		m_GameClientView.SetUserPassState(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(0);
	}

	//һ���ж�
	if (pPassCard->cbTurnOver==TRUE)
	{
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
		m_GameClientView.SetTurnScore(0);
		m_GameClientView.SetGameScore(SwitchViewChairID(pPassCard->wTurnWinner),pPassCard->cbTurnWinnerScore);
		if(pPassCard->wTurnWinner != m_wCurrentUser)
		{
			m_GameClientView.m_UserCardControl[SwitchViewChairID(pPassCard->wTurnWinner)].SetCardData(0);
		}
	}

	//�������
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		//������
		if (m_bTrustee==false)
		{
			ActiveGameFrame();

			//��ʾ��ť
			m_GameClientView.m_btOutCard.ShowWindow(true);
			m_GameClientView.m_btPassCard.ShowWindow(true);
			m_GameClientView.m_btOutPrompt.ShowWindow(true);
			m_GameClientView.m_btPassCard.EnableWindow((m_cbTurnCardCount>0)?true:false);
			m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?true:false);
		}

		//������ʾ
		try
		{
			m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount[m_wCurrentUser],m_cbTurnCardData,m_cbTurnCardCount,&m_SearchCardResult);
		}catch(...)
		{
			ASSERT(FALSE);
			m_SearchCardResult.cbSearchCount = 0;
		}
	}
	//else if ( m_wCurrentUser != INVALID_CHAIR )
	//{
	//	m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
	//	m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);
	//}


	//����ʱ��
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		SetGameClock(m_wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);

		//�����ǰ����
		m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
		m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);

		m_GameClientView.InvalidGameView(0,0,0,0);
	}

	return true;
}

bool CGameClientEngine::OnSubTrustee(VOID * pData, WORD wDataSize)
{

	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_Trustee));
	if (wDataSize!=sizeof(CMD_S_Trustee)) return false;

	//��������
	CMD_S_Trustee * pPack=(CMD_S_Trustee *)pData;
	IClientUserItem *pUserItem = GetTableUserItem(pPack->wChair);
	TCHAR strinfo[MAX_PATH]={0};
	if(pPack->bTrustee)
		_sntprintf(strinfo,CountArray(strinfo),_T("���%s�й��ˡ�"),pUserItem->GetNickName());
	else
		_sntprintf(strinfo,CountArray(strinfo),_T("���%sȡ���йܡ�"),pUserItem->GetNickName());
	
	m_pIStringMessage->InsertSystemString(strinfo);

	m_GameClientView.SetTrusteeFlag(SwitchViewChairID(pPack->wChair),pPack->bTrustee);

	if(pPack->wChair == GetMeChairID())
	{
		if (!pPack->bTrustee)
		{
			m_GameClientView.m_btTrustee.ShowWindow(true);
			m_GameClientView.m_btTrustee.EnableWindow(true);
			m_GameClientView.m_btCancelTrustee.ShowWindow(false);
		}
		else
		{
			m_GameClientView.m_btCancelTrustee.EnableWindow(true);
			m_GameClientView.m_btCancelTrustee.ShowWindow(true);
			m_GameClientView.m_btTrustee.ShowWindow(false);
		}
	}
	return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubGameConclude(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameConclude));
	if (wDataSize!=sizeof(CMD_S_GameConclude)) return false;

	//��������
	CMD_S_GameConclude * pGameConclude=(CMD_S_GameConclude *)pData;

	//��ǰ״̬
	SwitchToCurrentCard();

	//����״̬
	SetGameStatus(GS_GAME_FREE);

	//ֹͣ����
	m_GameClientView.StopDispatchCard();
	m_GameClientView.SetAddTimes(INVALID_CHAIR,false);
	m_GameClientView.SetAsk(INVALID_CHAIR,false);
	m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);
	//ɾ��ʱ��
	KillTimer(IDI_MOST_CARD);
	KillGameClock(IDI_OUT_CARD);
	KillGameClock(IDI_XUAN_ZHAN);
	KillGameClock(IDI_FIND_FRIEND);
	KillGameClock(IDI_ASK_FRIEND);
	KillGameClock(IDI_ADD_TIMES);
	//���ư�ť
	m_GameClientView.m_btOutCard.ShowWindow(false);
	m_GameClientView.m_btPassCard.ShowWindow(false);
	m_GameClientView.m_btOutPrompt.ShowWindow(false);

	m_GameClientView.m_btXuan.ShowWindow(false);
	m_GameClientView.m_btNoXuan.ShowWindow(false);
	m_GameClientView.m_btAsk.ShowWindow(false);
	m_GameClientView.m_btNoAsk.ShowWindow(false);
	m_GameClientView.m_btAdd.ShowWindow(false);
	m_GameClientView.m_btNoAdd.ShowWindow(false);

	m_GameClientView.m_btOutCard.EnableWindow(false);
	m_GameClientView.m_btPassCard.EnableWindow(false);
	m_GameClientView.m_btSortCard.EnableWindow(false);
	m_GameClientView.m_btLastTurn.EnableWindow(false);
	m_GameClientView.m_btTrustee.EnableWindow(false);
	m_GameClientView.m_bWaitAddTimes=false;
	m_GameClientView.m_bWaitXuanZhan=false;
	m_GameClientView.m_bWaitFindFriend=false;
	m_GameClientView.m_bWaitAskFriend=false;
	//���ü�����
	if(IsLookonMode() == false)
	{
		if(m_GameClientView.m_RememberCardWnd.IsWindowActive() == true)
		{
			m_GameClientView.m_RememberCardWnd.DeleteWindow();
		}
		//����������
		m_GameClientView.m_RememberCardWnd.DestroyCardList();
		m_GameClientView.m_RememberCardWnd.InitializeCardList();
	}

	//��������
	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

	//�ɼ�����
	ScoreInfo.wMeChairID=GetMeChairID();
	ScoreInfo.wBankerUser=m_wBankerUser;
	ScoreInfo.lCellScore=pGameConclude->lCellScore;
	ScoreInfo.cbBaseTimes=pGameConclude->cbBaseTimes;

	CString strOverInfo = TEXT("���ֽ���,�ɼ�ͳ��: \n");
	//�û��ɼ�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��������
		IClientUserItem * pIClientUserItem=GetTableUserItem(i);
		tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);


		//���óɼ�
		ScoreInfo.lGameScore[i]=pGameConclude->lGameScore[i];
		ScoreInfo.lCollectScore[i]=pHistoryScore->lCollectScore+pGameConclude->lGameScore[i];
		ScoreInfo.cbScore[i]=pGameConclude->cbScore[i];
		ScoreInfo.cbUserTimes[i]=pGameConclude->cbUserTimes[i];
		//�û���Ϣ
		ScoreInfo.pIClientUserItem[i] = pIClientUserItem;
		ScoreInfo.dwUserID[i]=pIClientUserItem->GetUserID();
		lstrcpyn(ScoreInfo.szNickName[i],pIClientUserItem->GetNickName(),CountArray(ScoreInfo.szNickName[i]));

		//��ʷ�ɼ�
		m_HistoryScore.OnEventUserScore(i,pGameConclude->lGameScore[i]);

		CString str;
		str.Format(TEXT("%s : %I64d \n"), ScoreInfo.szNickName[i], ScoreInfo.lGameScore[i]);
		strOverInfo += str;
	}

	if ( m_pIStringMessage )
		m_pIStringMessage->InsertSystemString(strOverInfo);

	//��ʾ�ɼ�
	m_GameClientView.ShowGameScoreInfo(ScoreInfo);

	//�����˿�
	BYTE cbCardIndex=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//�����˿�
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayItem(true);
		m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(&pGameConclude->cbHandCardData[cbCardIndex],pGameConclude->cbCardCount[i]);

		//��������
		cbCardIndex+=pGameConclude->cbCardCount[i];
	}

	//���ý���
	m_GameClientView.SetUserPassState(INVALID_CHAIR,false);
	m_GameClientView.SetUserCountWarn(INVALID_CHAIR,false);

	//��������
	SCORE lMeScore=pGameConclude->lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOSE"));
	if (lMeScore==0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_CONCLUDE"));

	//ȡ���й�
	if(m_bTrustee)
		OnTrusteeControl(0,0);

	//��ʼ����
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(true);
		//PerformAutoStart();
		WORD wMeChairID=GetMeChairID();
		SetGameClock(wMeChairID,IDI_START_GAME,m_cbTimeStartGame);
	}

	//���½���
	m_GameClientView.RefreshGameView();

	return true;
}

bool CGameClientEngine::OnSubFriend(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_Friend));
	if (wDataSize!=sizeof(CMD_S_Friend)) return false;

	//��������
	CMD_S_Friend * pPack=(CMD_S_Friend *)pData;

	m_cbFriendFlag = pPack->cbFriendFlag;
	CopyMemory(m_wFriend,pPack->wFriend,sizeof(m_wFriend));
	m_wXuanZhanUser = pPack->wXuanZhanUser;
	SetFriendFlag();

	return true;
}

//��֤����
bool CGameClientEngine::VerdictOutCard()
{
	//״̬�ж�
 	if (m_wCurrentUser!=GetMeChairID()) return false;
 	if (GetGameStatus()!=GS_PLAY)  return false;

	//��ȡ�˿�
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_ID].GetShootCard(cbCardData,CountArray(cbCardData));

	//�����ж�
	if (cbShootCount>0L)
	{
		//�����ж�
		m_GameLogic.SortCardList(cbCardData,cbShootCount,ST_ORDER);
		if (m_GameLogic.GetCardType(cbCardData,cbShootCount)==CT_ERROR) return false;

		//�����ж�
		if (m_cbTurnCardCount==0) return true;
		return m_GameLogic.CompareCard(m_cbTurnCardData,cbCardData,m_cbTurnCardCount,cbShootCount);
	}

	return false;
}

//�Զ���ʼ
bool CGameClientEngine::PerformAutoStart()
{
	//��������
	WORD wTableUserCount=0,wReadyUserCount=0;
	IClientUserItem * pIClientUserItem=GetMeUserItem();

	//״̬�ж�
	if ((pIClientUserItem==NULL)||(pIClientUserItem->GetUserStatus()==US_READY)) return false;
	if ((IsLookonMode()==true)||(m_pIClientKernel->GetClockChairID()==IDI_START_GAME)) return false;

	//ͳ���û�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��ȡ�û�
		IClientUserItem * pIClientUserItem=GetTableUserItem(i);

		//��Ŀͳ��
		if (pIClientUserItem!=NULL) 
		{
			wTableUserCount++;
			if (pIClientUserItem->GetUserStatus()==US_READY) wReadyUserCount++;
		}
	}

	//3��׼����1��δ׼��ʱ������ʼ��ʱ��
	if ((wTableUserCount==GAME_PLAYER)&&((wReadyUserCount+1)==wTableUserCount))
	{
		//��ʼ����
		WORD wMeChairID=GetMeChairID();
		SetGameClock(wMeChairID,IDI_START_GAME,m_cbTimeStartGame);
		return true;
	}

	return true;
}

//��ǰ�˿�
bool CGameClientEngine::SwitchToCurrentCard()
{
	//�����ж�
	if (m_bLastTurn==false) return false;

	//���ñ���
	m_bLastTurn=false;

	//ɾ��ʱ��
	KillTimer(IDI_LAST_TURN);

	//���ý���
	m_GameClientView.SetLastTurnState(false);

	//�����˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��������
		WORD wViewChairID=SwitchViewChairID(i);
		BYTE cbEventFlag=m_HistoryCard.GetCurrentEvent(i);

		//��ȡ�˿�
		BYTE cbCardData[MAX_CARD_COUNT];
		BYTE cbCardCount=m_HistoryCard.GetCurrentCard(i,cbCardData,CountArray(cbCardData));

		//���ý���
		m_GameClientView.SetUserPassState(wViewChairID,cbEventFlag == AF_PASS);
		if( wViewChairID,cbEventFlag == AF_PASS || wViewChairID,cbEventFlag == AF_NULL )
		{
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		}
		else
		{
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(cbCardData,cbCardCount);
		}
	}

	//���½���
	m_GameClientView.RefreshGameView();

	return true;
}

//�Զ�����
bool CGameClientEngine::AutomatismOutCard()
{
	//״̬�ж�
	if (GetGameStatus()!=GS_PLAY) return false;
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID())) return false;

	//��ǰ����
	CVirtualButton & btOutCard=m_GameClientView.m_btOutCard;
	if ((btOutCard.IsWindowEnable())&&(m_wCurrentUser==GetMeChairID()))
	{
		OnOutCard(0, 0);
		return true;
	}

	//���ƴ���
	if ((m_cbTurnCardCount==0)||(m_bTrustee==true))
	{
		//���ý���
		if (m_SearchCardResult.cbSearchCount>0)
		{
			for(BYTE cbSearchIndex = 0;cbSearchIndex < m_SearchCardResult.cbSearchCount;cbSearchIndex++)
			{
				m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetShootCard( m_SearchCardResult.cbResultCard[cbSearchIndex],m_SearchCardResult.cbCardCount[cbSearchIndex] );
				bool bOutCard=VerdictOutCard();
				if(bOutCard)
				{
					m_GameClientView.m_btOutCard.EnableWindow(TRUE);
					//���ƶ���
					OnOutCard(0,0);
					return true;
				}
			}			
			OnPassCard(0,0);
			return true;
		}
	}
	//һ�ֹ����ֵ��Լ�
	if(m_cbTurnCardCount == 0 && m_SearchCardResult.cbSearchCount == 0)
	{
		BYTE cbRemainCardData[MAX_CARD_COUNT];
		ZeroMemory(cbRemainCardData,sizeof(cbRemainCardData));
		BYTE cbRemainCardCount = m_GameClientView.m_HandCardControl[ME_VIEW_ID].GetCardData(cbRemainCardData,CountArray(cbRemainCardData));

		BYTE *pReverserCardData = new BYTE[cbRemainCardCount];
		BYTE tmpIndex = 0;
		for(int i = cbRemainCardCount - 1;i >= 0;i--)
		{
			pReverserCardData[tmpIndex++] = cbRemainCardData[i];
		}
		ASSERT(tmpIndex == cbRemainCardCount);
		BYTE cbMostOutCardCount = 4;
		while(true)
		{
			BYTE *pKeyCard = new BYTE[cbMostOutCardCount];
			CopyMemory(pKeyCard,pReverserCardData,sizeof(BYTE)*cbMostOutCardCount);
			//У��Ϸ���
			bool bOutCard = true;
			m_GameLogic.SortCardList(pKeyCard,cbMostOutCardCount,ST_ORDER,true);
			if (m_GameLogic.GetCardType(pKeyCard,cbMostOutCardCount)==CT_ERROR) 
			{
				bOutCard = false;
			}
			if(bOutCard)
			{
				m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);
				
				m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetShootCard(pKeyCard,cbMostOutCardCount);
				//���ƶ���
				OnOutCard(0,0);
				delete[] pKeyCard;
				break;
			}
			else
			{
				cbMostOutCardCount--;
				delete[] pKeyCard;
			}
		}
		delete[] pReverserCardData;
	}

	//��������
	if (m_cbTurnCardCount>0) OnPassCard(0,0);

	return true;
}

//���°�ť
VOID CGameClientEngine::UpdateButtonControl()
{
	//Ч��
	if( IsLookonMode() && !IsAllowLookon() ) return;

	//��������
	WORD wMeChairId = GetMeChairID();
	if( m_cbHandCardCount[wMeChairId] == 0 ) return;

	//��������
	BYTE cbHandCardCount = m_cbHandCardCount[wMeChairId];
	BYTE cbHandCardData[MAX_CARD_COUNT] = {0};
	CopyMemory( cbHandCardData,m_cbHandCardData,sizeof(BYTE)*cbHandCardCount );

	//����
	m_GameLogic.SortCardList( cbHandCardData,cbHandCardCount,ST_ORDER,true );

	//�������
	m_nCurSearchType = -1;
	m_cbEachSearchIndex = 0;
	ZeroMemory( &m_EachSearchResult,sizeof(m_EachSearchResult) );

	return;
}

//��������
VOID CGameClientEngine::PlayOutCardSound(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//Ч�����
	ASSERT((wChairID<GAME_PLAYER)&&(cbCardCount>0));
	if ((wChairID>=GAME_PLAYER)||(cbCardCount==0)) return;
	
	//��ȡ����
	BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);

	//��ȡ�û�
	IClientUserItem * pIClientUserItem=GetTableUserItem(wChairID);

	//��������
	switch (cbCardType)
	{
	case CT_SINGLE:					//��������
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();
			BYTE cbCardValue=m_GameLogic.GetCardValue(cbCardData[0]);

			//��������
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("SINGLE_%d_%s"),cbCardValue,(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//��������
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_DOUBLE:					//��������
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();

			//��������
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("DOUBLE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//��������
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_THREE:					//��������
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();

			//��������
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("THREE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//��������
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_SINGLE_LINE:			//��������
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();

			//��������
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("SINGLE_LINE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//��������
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_DOUBLE_LINE:			//��������
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();

			//��������
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("DOUBLE_LINE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//��������
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_THREE_LINE:				//��������
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();

			//��������
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("THREE_LINE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//��������
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_THREE_TAKE_ONE:			//����һ
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();

			//��������
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("THREE_TAKE_ONE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//��������
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_THREE_TAKE_TWO:			//������
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();

			//��������
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("THREE_TAKE_TWO_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//��������
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_THREE_LINE_TAKE_ONE:
	case CT_THREE_LINE_TAKE_TWO:	
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();

			//��������
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("THREE_ONE_LINE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//��������
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_BOMB_CARD:				//ը������
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();

			//��������
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("BOMB_CARD_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//��������
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	}

	return;
}


//�û�״̬
VOID CGameClientEngine::OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//��������
	IClientUserItem * pIMySelfUserItem=GetMeUserItem();
	BYTE cbUserStatus=pIClientUserItem->GetUserStatus();

	//ֹͣ��ʼ
	if ((pIClientUserItem==pIMySelfUserItem)&&(cbUserStatus>=US_READY)) 
	{
		KillGameClock(IDI_START_GAME);
		m_GameClientView.m_btStart.ShowWindow(false);
	}
}

//ʱ����Ϣ
VOID CGameClientEngine::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case IDI_MOST_CARD:			//����˿�
		{
			//�û�Ч��
			ASSERT(m_wMostCardUser!=INVALID_CHAIR);
			if (m_wMostCardUser==INVALID_CHAIR) return;

			//�����ж�
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_GameClientView.m_UserCardControl[i].IsOutCard()==true)
				{
					return;
				}
			}

			//ɾ��ʱ��
			KillTimer(IDI_MOST_CARD);

			//���ñ���
			m_wCurrentUser=m_wMostCardUser;
			m_wMostCardUser=INVALID_CHAIR;

			//��������
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_HistoryCard.OnEventSaveData(i);
			}

			//���ý���
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetUserPassState(i,false);
				m_GameClientView.m_UserCardControl[i].SetCardData(0);
			}

			//�������
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				//������
				if (m_bTrustee==false) ActiveGameFrame();

				//���ð�ť
				m_GameClientView.m_btOutCard.ShowWindow(true);
				m_GameClientView.m_btPassCard.ShowWindow(true);
				m_GameClientView.m_btPassCard.EnableWindow(false);
				m_GameClientView.m_btOutPrompt.ShowWindow(true);
				m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?true:false);

				//������ʾ
				try
				{
					m_GameLogic.SearchOutCard(m_cbHandCardData, m_cbHandCardCount[m_wCurrentUser], m_cbTurnCardData, m_cbTurnCardCount, &m_SearchCardResult);
				}catch(...)
				{
					ASSERT(FALSE);
					m_SearchCardResult.cbSearchCount = 0;
				}
			}

			//����ʱ��
			SetGameClock(m_wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);

			//�����ǰ����
			m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
			m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);

			m_GameClientView.InvalidGameView(0,0,0,0);

			return;
		}
	case IDI_LAST_TURN:			//�����˿�
		{
			//��ԭ����
			SwitchToCurrentCard();

			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//��ʼ��Ϣ
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	//ɾ��ʱ��
	KillGameClock(IDI_START_GAME);

	//�û��˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetDisplayItem(false);
	}

	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetPositively(false);

	//����ׯ��
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	//״̬����
	m_GameClientView.SetUserPassState(INVALID_CHAIR,false);
	m_GameClientView.SetUserCountWarn(INVALID_CHAIR,false);
	m_GameClientView.SetAddTimes(INVALID_CHAIR,false);
	m_GameClientView.SetAsk(INVALID_CHAIR,false);
	m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);
	m_GameClientView.SetTurnScore(0);
	m_GameClientView.SetGameScore(INVALID_CHAIR,0);
	m_GameClientView.SetFriendFlag(INVALID_CHAIR,0);
	m_GameClientView.SetTrusteeFlag(INVALID_CHAIR,0);
	m_GameClientView.m_cbSelectedCardData=0;
	m_wXuanZhanUser=INVALID_CHAIR;
	m_wAskUser=INVALID_CHAIR;
	m_cbFriendFlag=0;
	m_bSelfDoneAddTimes=false;
	//���ý���
	m_GameClientView.m_btStart.ShowWindow(false);
	m_GameClientView.m_ScoreControl.CloseControl();

	//���½���
	m_GameClientView.RefreshGameView();

	//������Ϣ
	SendUserReady(NULL,0);

	return 0L;
}

//������Ϣ
LRESULT CGameClientEngine::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//״̬�ж�
	if (m_GameClientView.m_btOutCard.IsWindowEnable()==false) return 0;
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==false) return 0;

	//��ǰ�˿�
	SwitchToCurrentCard();

	//ɾ��ʱ��
	KillGameClock(IDI_OUT_CARD);

	//���ñ���
	m_wCurrentUser=INVALID_CHAIR;
	m_cbSearchResultIndex = 0;
	m_SearchCardResult.cbSearchCount = 0;

	//���ý���
	m_GameClientView.m_btOutCard.ShowWindow(false);
	m_GameClientView.m_btPassCard.ShowWindow(false);
	m_GameClientView.m_btOutPrompt.ShowWindow(false);
	m_GameClientView.m_btOutCard.EnableWindow(false);
	m_GameClientView.m_btPassCard.EnableWindow(false);

	//��ȡ�˿�
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_ID].GetShootCard(cbCardData,CountArray(cbCardData));

	//�����˿�
	m_GameLogic.SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//��������
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].RemoveShootItem();
	m_GameClientView.m_UserCardControl[ME_VIEW_ID].ShowOutCard(cbCardData,cbCardCount);

	//ɾ���˿�
	WORD wMeChairID=GetMeChairID();
	m_cbHandCardCount[wMeChairID]-=cbCardCount;
	m_GameLogic.RemoveCardList(cbCardData,cbCardCount,m_cbHandCardData,m_cbHandCardCount[wMeChairID]+cbCardCount);

	//��ť����
	UpdateButtonControl();

	//��������
	BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);
	if ( m_cbTurnCardCount != 0 
		&& !(cbCardType == CT_THREE_LINE_TAKE_TWO)
		&& !(cbCardType == CT_THREE_TAKE_TWO)
		&& !(cbCardType == CT_BOMB_CARD))
	{
		//��������
		LPCTSTR pszSoundBoy=TEXT("OUT_CARD_BOY");
		LPCTSTR pszSoundGirl=TEXT("OUT_CARD_GIRL");
		IClientUserItem * pIClientUserItem=GetTableUserItem(wMeChairID);
		
		//��������
		BYTE cbGender=pIClientUserItem->GetGender();
		PlayGameSound(AfxGetInstanceHandle(),(cbGender==GENDER_FEMALE)?pszSoundGirl:pszSoundBoy);
	}
	else
	{
		//��������
		PlayOutCardSound(wMeChairID,cbCardData,cbCardCount);
	}

	//��������
	if (cbCardType >= CT_510K_FALSE)
	{
		m_GameClientView.ShowCartoonBomb(true);
	}
	else if (cbCardType >= CT_THREE_LINE_TAKE_ONE) 
	{
		m_GameClientView.ShowCartoonPlan(true);
	}
	else if(cbCardType == CT_DOUBLE_LINE)
	{
		m_GameClientView.ShowCartoonDoubleLine(true);
	}
	else if(cbCardType == CT_THREE_LINE || cbCardType == CT_THREE)
	{
		m_GameClientView.ShowCartoonThreeLine(true);
	}
	else if(cbCardType == CT_THREE_TAKE_ONE || cbCardType == CT_THREE_TAKE_TWO)
	{
		m_GameClientView.ShowCartoonThreeTake(true);
	}

	//������ʷ
	m_HistoryCard.OnEventUserOutCard(wMeChairID,cbCardData,cbCardCount);

	//��������
	CMD_C_OutCard OutCard;
	OutCard.cbCardCount=cbCardCount;
	CopyMemory(OutCard.cbCardData,cbCardData,cbCardCount*sizeof(BYTE));

	//��������
	SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)-sizeof(OutCard.cbCardData)+OutCard.cbCardCount*sizeof(BYTE));

	return 0;
}

//PASS��Ϣ
LRESULT CGameClientEngine::OnPassCard(WPARAM wParam, LPARAM lParam)
{
	//״̬�ж�
	if (m_GameClientView.m_btPassCard.IsWindowEnable()==false) return 0;
	if (m_GameClientView.m_btPassCard.IsWindowVisible()==false) return 0;

	//��ǰ�˿�
	SwitchToCurrentCard();

	//ɾ��ʱ��
	KillGameClock(IDI_OUT_CARD);

	//���ñ���
	m_wCurrentUser=INVALID_CHAIR;
	m_cbSearchResultIndex = 0;
	m_SearchCardResult.cbSearchCount = 0;

	//���ý���
	m_GameClientView.m_btOutCard.ShowWindow(false);
	m_GameClientView.m_btPassCard.ShowWindow(false);
	m_GameClientView.m_btOutPrompt.ShowWindow(false);
	m_GameClientView.m_btOutCard.EnableWindow(false);
	m_GameClientView.m_btPassCard.EnableWindow(false);

	//��ʷ��¼
	m_HistoryCard.OnEventUserPass(GetMeChairID());

	//���÷���
	m_GameClientView.SetUserPassState(ME_VIEW_ID,true);

	//�����˿�
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetShootCard(NULL,0);

	//���½���
	m_GameClientView.RefreshGameView();

	//��������
	IClientUserItem * pClientUserItem=GetMeUserItem();
	PlayGameSound(AfxGetInstanceHandle(),(pClientUserItem->GetGender()==GENDER_FEMALE)?TEXT("PASS_CARD_GIRL"):TEXT("PASS_CARD_BOY"));

	//��������
	SendSocketData(SUB_C_PASS_CARD);

	return 0L;
}

//��ʾ��Ϣ
LRESULT CGameClientEngine::OnOutPrompt(WPARAM wParam, LPARAM lParam)
{
	WORD wMeChairID = GetMeChairID();

	//�д����
	if( m_SearchCardResult.cbSearchCount > 0 )
	{
		//���ý���
		m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetShootCard(m_SearchCardResult.cbResultCard[m_cbSearchResultIndex],
			m_SearchCardResult.cbCardCount[m_cbSearchResultIndex]);

		//���ÿؼ�
		bool bOutCard=VerdictOutCard();
		m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);

		//���½���
		m_GameClientView.InvalidGameView(0,0,0,0);

		//���ñ���
		m_cbSearchResultIndex = (m_cbSearchResultIndex+1)%m_SearchCardResult.cbSearchCount;

		return 0;
	}

	//��������
	OnPassCard(0,0);

	if(m_cbTurnCardCount == 0 && m_SearchCardResult.cbSearchCount==0 && m_wCurrentUser==GetMeChairID())
	{
		BYTE cbRemainCardData[MAX_CARD_COUNT];
		ZeroMemory(cbRemainCardData,sizeof(cbRemainCardData));
		BYTE cbRemainCardCount = m_GameClientView.m_HandCardControl[ME_VIEW_ID].GetCardData(cbRemainCardData,CountArray(cbRemainCardData));

		BYTE *pReverserCardData = new BYTE[cbRemainCardCount];
		BYTE tmpIndex = 0;
		for(int i = cbRemainCardCount - 1;i >= 0;i--)
		{
			pReverserCardData[tmpIndex++] = cbRemainCardData[i];
		}
		ASSERT(tmpIndex == cbRemainCardCount);
		BYTE cbMostOutCardCount =1;
		while(true)
		{
			BYTE *pKeyCard = new BYTE[cbMostOutCardCount];
			CopyMemory(pKeyCard,pReverserCardData,sizeof(BYTE)*cbMostOutCardCount);
			//У��Ϸ���
			bool bOutCard = true;
			m_GameLogic.SortCardList(pKeyCard,cbMostOutCardCount,ST_ORDER,true);
			if (m_GameLogic.GetCardType(pKeyCard,cbMostOutCardCount)==CT_ERROR) 
			{
				bOutCard = false;
			}
			if(bOutCard)
			{
				m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);

				m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetShootCard(pKeyCard,cbMostOutCardCount);
				//���ƶ���
				//OnOutCard(0,0);
				delete[] pKeyCard;
				break;
			}
			else
			{
				cbMostOutCardCount--;
				delete[] pKeyCard;
			}
		}
		delete[] pReverserCardData;
	}

	
	return 0L;
}

//�з���Ϣ
LRESULT CGameClientEngine::OnCallScore(WPARAM wParam, LPARAM lParam)
{

	return 0L;
}
LRESULT CGameClientEngine::OnXuanZhan(WPARAM wParam, LPARAM lParam)
{
	//ɾ��ʱ��
	KillGameClock(IDI_XUAN_ZHAN);

	//���ý���
	m_GameClientView.m_btXuan.ShowWindow(false);
	m_GameClientView.m_btNoXuan.ShowWindow(false);

	//��������
	CMD_C_XuanZhan CallScore;
	CallScore.bXuanZhan=(BYTE)wParam;
	SendSocketData(SUB_C_XUAN_ZHAN,&CallScore,sizeof(CallScore));
	return 0L;
}

LRESULT CGameClientEngine::OnFindFriend(WPARAM wParam, LPARAM lParam)
{
	//ɾ��ʱ��
	KillGameClock(IDI_FIND_FRIEND);

	//��������
	CMD_C_FindFriend pack;
	pack.cbCardData=(BYTE)wParam;
	SendSocketData(SUB_C_FIND_FRIEND,&pack,sizeof(pack));

	return 0L;
}

LRESULT CGameClientEngine::OnAskFriend(WPARAM wParam, LPARAM lParam)
{
	//ɾ��ʱ��
	KillGameClock(IDI_ASK_FRIEND);

	//���ý���
	m_GameClientView.m_btAsk.ShowWindow(false);
	m_GameClientView.m_btNoAsk.ShowWindow(false);
	m_GameClientView.m_btRespond.ShowWindow(false);
	m_GameClientView.m_btNoRespond.ShowWindow(false);
	//��������
	CMD_C_AskFriend pack;
	pack.bAsk=(BYTE)wParam;
	SendSocketData(SUB_C_ASK_FRIEND,&pack,sizeof(pack));
	return 0L;
}

LRESULT CGameClientEngine::OnAddTimes(WPARAM wParam, LPARAM lParam)
{
	//ɾ��ʱ��
	KillGameClock(IDI_ADD_TIMES);

	//���ý���
	m_GameClientView.m_btAdd.ShowWindow(false);
	m_GameClientView.m_btNoAdd.ShowWindow(false);

	//��������
	CMD_C_AddTimes pack;
	pack.bAddTimes=(BYTE)wParam;
	SendSocketData(SUB_C_ADD_TIMES,&pack,sizeof(pack));

	m_GameClientView.m_bWaitAddTimes=true;
	return 0L;
}
//����˿�
LRESULT CGameClientEngine::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	//���ÿؼ�
	bool bOutCard=VerdictOutCard();
	m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);

	return 0L;
}

//�����˿�
LRESULT CGameClientEngine::OnSortHandCard(WPARAM wParam, LPARAM lParam)
{
	//���ñ���
	m_cbSortType=(m_cbSortType!=ST_ORDER)?ST_ORDER:ST_COUNT;

	//�����˿�
	WORD wMeChairID=GetMeChairID();
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],m_cbSortType,TRUE);

	//��ȡ�˿�
	BYTE cbShootCard[MAX_CARD_COUNT];
	BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_ID].GetShootCard(cbShootCard,CountArray(cbShootCard));

	//�����˿�
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetCardData(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);

	//�����˿�
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetShootCard(cbShootCard,cbShootCount);

	//���ð�ť
	LPCTSTR pszButtonImage=(m_cbSortType==ST_ORDER)?TEXT("BT_SORT_CARD_COUNT"):TEXT("BT_SORT_CARD_ORDER");
	m_GameClientView.m_btSortCard.SetButtonImage(&m_GameClientView.m_D3DDevice,pszButtonImage,TEXT("PNG"),AfxGetInstanceHandle());

	//���½���
	m_GameClientView.RefreshGameView();

	return 0;
}

//�鿴����
LRESULT CGameClientEngine::OnLastTurnCard(WPARAM wParam, LPARAM lParam)
{
	if (m_bLastTurn==false)
	{
		//�����˿�
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//��������
			WORD wViewChairID=SwitchViewChairID(i);
			BYTE cbEventFlag=m_HistoryCard.GetHistoryEvent(i);

			//��ȡ�˿�
			BYTE cbCardData[MAX_CARD_COUNT];
			BYTE cbCardCount=m_HistoryCard.GetHistoryCard(i,cbCardData,CountArray(cbCardData));

			//���ý���
			m_GameClientView.SetUserPassState(wViewChairID,cbEventFlag==AF_PASS);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(cbCardData,cbCardCount);
		}

		//����ʱ��
		SetTimer(IDI_LAST_TURN,3000,NULL);

		//��������
		m_bLastTurn=true;
		m_GameClientView.SetLastTurnState(true);

		//���½���
		m_GameClientView.RefreshGameView();
	}
	else
	{
		//��ǰ�˿�
		SwitchToCurrentCard();
	}

	return 0;
}

//�Ϲܿ���
LRESULT CGameClientEngine::OnTrusteeControl(WPARAM wParam, LPARAM lParam)
{
	m_bTrustee = !m_bTrustee;
	if (!m_bTrustee)
	{
		m_GameClientView.m_btTrustee.ShowWindow(true);
		m_GameClientView.m_btTrustee.EnableWindow(true);
		m_GameClientView.m_btCancelTrustee.ShowWindow(false);
		m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetPositively(true);
	}
	else
	{
		m_GameClientView.m_btCancelTrustee.EnableWindow(true);
		m_GameClientView.m_btCancelTrustee.ShowWindow(true);
		m_GameClientView.m_btTrustee.ShowWindow(false);

		m_GameClientView.m_btOutCard.ShowWindow(false);
		m_GameClientView.m_btPassCard.ShowWindow(false);
		m_GameClientView.m_btOutPrompt.ShowWindow(false);
		m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetPositively(false);
	}

	if(GetGameStatus() == GS_PLAY)
	{
		CMD_C_Trustee pack;
		pack.bTrustee = m_bTrustee;
		SendSocketData(SUB_C_TRUSTEE,&pack,sizeof(pack));
	}

	//���½���
	m_GameClientView.RefreshGameView();
	return 0L;
}

//��������
LRESULT CGameClientEngine::OnEnableHistory(WPARAM wParam, LPARAM lParam)
{
	//��������
	m_GameClientView.m_btLastTurn.EnableWindow(true);

	return 0;
}

//�������
LRESULT CGameClientEngine::OnOutCardFinish(WPARAM wParam, LPARAM lParam)
{
	//�����ж�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_GameClientView.m_UserCardControl[i].IsOutCard()==true)
		{
			return 0L;
		}
	}

	//��������
	if ((GetGameStatus()==GS_PLAY)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		//��������
		WORD wMeChairID=GetMeChairID();

		//�����˿�
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(0);

		//���ư�ť
		if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
		{
			//������
			if (m_bTrustee==false) 
			{
				ActiveGameFrame();

				//��ʾ��ť
				m_GameClientView.m_btOutCard.ShowWindow(true);
				m_GameClientView.m_btPassCard.ShowWindow(true);
				m_GameClientView.m_btOutPrompt.ShowWindow(true);

				//���ð�ť
				m_GameClientView.m_btPassCard.EnableWindow((m_cbTurnCardCount>0)?true:false);
				m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?true:false);
			}
			//������ʾ
			try
			{
				m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount[m_wCurrentUser],m_cbTurnCardData,m_cbTurnCardCount,
					&m_SearchCardResult);
			}catch(...)
			{
				ASSERT(FALSE);
				m_SearchCardResult.cbSearchCount = 0;
			}
		}

		//����ʱ��
		SetGameClock(m_wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);

		//�����ǰ����
		m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
		m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);

		m_GameClientView.InvalidGameView(0,0,0,0);
	}

	//���°�ť
	UpdateButtonControl();

	return 0L;
}

//�������
LRESULT CGameClientEngine::OnDispatchFinish(WPARAM wParam, LPARAM lParam)
{
	//״̬Ч��
	ASSERT(GetGameStatus()==GS_XUAN_ZHAN);
	if (GetGameStatus()!=GS_XUAN_ZHAN) return 0L;

	//���ý���
	m_GameClientView.m_btSortCard.EnableWindow(!IsLookonMode());

	//�����˿�
	if(m_GameLogic.IsHave510K(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()]))
		m_cbSortType = ST_510K;
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],m_cbSortType,true);

	//�����˿�
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetCardData(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()]);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	//��ҿ���
	if (IsLookonMode()==false)	
	{
		m_GameClientView.m_btSortCard.EnableWindow(true);
		//m_GameClientView.m_btRememberCard.ShowWindow(TRUE);
		//m_GameClientView.m_btRememberCard.EnableWindow(TRUE);

		//��ʾ��ť
		if (GetMeChairID()==m_wCurrentUser)
		{
			//���ư�ť
			m_GameClientView.m_btXuan.EnableWindow(true);
			m_GameClientView.m_btNoXuan.EnableWindow(true);

			//��ʾ��ť
			m_GameClientView.m_btXuan.ShowWindow(true);
			m_GameClientView.m_btNoXuan.ShowWindow(true);
		}
		else
			m_GameClientView.m_bWaitXuanZhan=true;
	}
	
	//���°�ť
	UpdateButtonControl();

	//����ʱ��
	SetGameClock(m_wCurrentUser,IDI_XUAN_ZHAN,m_cbTimeXuanZhan);

	//�����ǰ����
	m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
	m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);

	m_GameClientView.InvalidGameView(0,0,0,0);

	//������
	if ((IsLookonMode()==false)&&(m_bTrustee==false)) ActiveGameFrame();

	return 0L;
}

//��������
bool CGameClientEngine::AllowBackGroundSound( bool bAllowSound )
{
	if(bAllowSound && !m_bBackGroundSound )
	{
		PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_MUSIC"));
	}
	else if ( !bAllowSound )
	{
		StopSound();
		m_bBackGroundSound = false;
	}
	return true;
}

VOID CGameClientEngine::SetFriendFlag()
{
	m_GameClientView.SetFriendFlag(INVALID_CHAIR,0);

	if(m_cbFriendFlag == 1)//��ս
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(i == m_wXuanZhanUser)
				m_GameClientView.SetFriendFlag(SwitchViewChairID(i),5);
			else
			{
				if(m_wXuanZhanUser == GetMeChairID())
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),3);
				else
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),6);
			}
		}

	}
	else if(m_cbFriendFlag == 2)//����
	{
		if(m_wXuanZhanUser==GetMeChairID())
		{
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(i == m_wXuanZhanUser)
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),1);
				else
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),3);
			}
		}
		else
		{
			for(int i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetFriendFlag(SwitchViewChairID(i),4);
			}
		}
	}
	else if(m_cbFriendFlag == 3)//������ʾ
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(i == m_wXuanZhanUser)
				m_GameClientView.SetFriendFlag(SwitchViewChairID(i),1);
			else
			{
				if(m_wXuanZhanUser == GetMeChairID())
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),3);
				else
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),6);
			}
		}
	}
	else if(m_cbFriendFlag == 4)//����
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(i == m_wXuanZhanUser)
				m_GameClientView.SetFriendFlag(SwitchViewChairID(i),2);
			else
			{
				if(m_wXuanZhanUser == GetMeChairID())
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),3);
				else
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),6);
			}
		}
	}
	else if(m_cbFriendFlag == 5)//ѡ����
	{
		if(m_wFriend[m_wBankerUser] == GetMeChairID())
		{
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(i == GetMeChairID() || i == m_wFriend[GetMeChairID()])
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),6);
				else
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),3);
			}
		}
		else
		{
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(i != GetMeChairID())
					m_GameClientView.SetFriendFlag(SwitchViewChairID(i),4);
			}
		}

	}
	else if(m_cbFriendFlag == 6)//������ʾ
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(i == GetMeChairID() || i == m_wFriend[GetMeChairID()])
				m_GameClientView.SetFriendFlag(SwitchViewChairID(i),6);
			else
				m_GameClientView.SetFriendFlag(SwitchViewChairID(i),3);
		}
	}

}







//////////////////////////////////////////////////////////////////////////////////
