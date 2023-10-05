#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////////////
//游戏时间
#define IDI_START_GAME				200									//开始定时器
#define IDI_XUAN_ZHAN				201									//宣战定时器
#define IDI_FIND_FRIEND				202									//找同盟定时器
#define IDI_ASK_FRIEND				203									//问同盟定时器
#define IDI_ADD_TIMES				204									//加倍定时器
#define IDI_OUT_CARD					205									//出牌定时器

//游戏时间
#define IDI_MOST_CARD				300									//最大定时器
#define IDI_LAST_TURN				301									//上轮定时器

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)

	//系统消息
	ON_WM_TIMER()

	//游戏消息
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

	//辅助消息
	ON_MESSAGE(WM_ENABLE_HISTORY,OnEnableHistory)
	ON_MESSAGE(IDM_OUT_CARD_FINISH,OnOutCardFinish)
	ON_MESSAGE(IDM_DISPATCH_FINISH,OnDispatchFinish)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//辅助变量
	m_bTrustee=false;
	m_bLastTurn=false;
	m_cbSortType=ST_ORDER;
	m_bBackGroundSound = false;


	//时间定义
	m_cbTimeOutCard=0;
	m_cbTimeXuanZhan=0;
	m_cbTimeFindFriend=0;
	m_cbTimeAskFriend=0;
	m_cbTimeAddTimes=0;

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wMostCardUser=INVALID_CHAIR;
	m_cbFriendFlag=0;
	m_bSelfDoneAddTimes=false;
	//搜索变量
	m_cbSearchResultIndex = 0;
	ZeroMemory( &m_SearchCardResult,sizeof(m_SearchCardResult) );
	m_nCurSearchType = -1;
	m_cbEachSearchIndex = 0;
	ZeroMemory( &m_EachSearchResult,sizeof(m_EachSearchResult) );

	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
	
	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//创建引擎
bool CGameClientEngine::OnInitGameEngine()
{
	//设置属性
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	//播放声音
	CGlobalUnits *pGlobalUnits=CGlobalUnits::GetInstance();
	if ( pGlobalUnits->m_bAllowBackGroundSound )
	{
		m_bBackGroundSound = true;
		PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_MUSIC"));
	}


	return true;
}

//重置函数
bool CGameClientEngine::OnResetGameEngine()
{
	//删除时间
	KillTimer(IDI_MOST_CARD);
	KillTimer(IDI_LAST_TURN);

	//辅助变量
	m_bTrustee=false;
	m_bLastTurn=false;
	m_cbSortType=ST_ORDER;


	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wMostCardUser=INVALID_CHAIR;

	//搜索变量
	m_cbSearchResultIndex = 0;
	ZeroMemory( &m_SearchCardResult,sizeof(m_SearchCardResult) );
	m_nCurSearchType = -1;
	m_cbEachSearchIndex = 0;
	ZeroMemory( &m_EachSearchResult,sizeof(m_EachSearchResult) );


	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	return true;
}

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//时钟信息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	switch (wClockID)
	{
	case IDI_START_GAME:	//游戏开始
		{
			//关闭游戏
			if ( nElapse <= 0 && !IsLookonMode() && wChairID == GetMeChairID() )
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//超时警告
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{				
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_XUAN_ZHAN:	//用户宣战
		{
			//自动处理
			if (( nElapse <= 0 ) && wChairID == GetMeChairID())
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//超时警告
			if ((nElapse<=5 && nElapse >0)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_FIND_FRIEND:	//找同盟
		{
			//自动处理
			if (( nElapse <= 0 ) && wChairID == GetMeChairID())
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//超时警告
			if ((nElapse<=5 && nElapse >0)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_ASK_FRIEND:	//问同盟
		{
			//自动处理
			if (( nElapse <= 0 ) && wChairID == GetMeChairID())
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//超时警告
			if ((nElapse<=5 && nElapse >0)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_ADD_TIMES:	//加倍
		{
			//自动处理
			if (( nElapse <= 0 ) && wChairID == GetMeChairID())
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//超时警告
			if ((nElapse<=5 && nElapse >0)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_OUT_CARD:		//用户出牌
		{
			if (IsLookonMode()) return true;

			//自动处理
			if (wChairID == GetMeChairID() &&	nElapse <= 0)
			{
				m_GameClientView.SetUserCountWarn(ME_VIEW_ID,FALSE);
				//托管
				if (!m_bTrustee && nElapse == 0)
				{
					OnTrusteeControl(0,0);
				}
				return true;
			}

			//超时警告
			if ((nElapse<=5 && nElapse >0)&&(wChairID==GetMeChairID())) 
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	}
	return false;
}

//旁观消息
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//旁观控制
	bool bAllowLookon=IsAllowLookon();

	//扑克控制
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

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);


	return true;
}

//游戏消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_XUAN_ZHAN:		//用户宣战
		{
			return OnSubXuanZhan(pData,wDataSize);
		}
	case SUB_S_FIND_FRIEND:		//找同盟
		{
			return OnSubFindFriend(pData,wDataSize);
		}
	case SUB_S_ASK_FRIEND:		//问同盟
		{
			return OnSubAskFriend(pData,wDataSize);
		}
	case SUB_S_ADD_TIMES:		//加倍
		{
			return OnSubAddTimes(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_PASS_CARD:		//用户放弃
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
	case SUB_S_GAME_CONCLUDE:	//游戏结束
		{
			return OnSubGameConclude(pData,wDataSize);
		}
	case SUB_S_SET_BASESCORE:	//设置基数
		{
			//效验数据
			ASSERT(wDataSize==sizeof(LONG));
			if(wDataSize!=sizeof(LONG)) return false;

			LONG lCellScore=*(LONG*)pData;
			m_GameClientView.SetCellScore(lCellScore);
			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//场景消息
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_GAME_FREE:	//空闲状态
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_S_StatusFree));
			if (wDataSize != sizeof(CMD_S_StatusFree))
				return false;

			//变量定义
			CMD_S_StatusFree* pStatusFree = (CMD_S_StatusFree*)pData;

			//设置界面
			//m_GameClientView.m_btScore.ShowWindow(true);
			m_GameClientView.SetCellScore(pStatusFree->lCellScore);
			m_GameClientView.m_b2Biggest=pStatusFree->b2Biggest;
			m_GameClientView.m_GameLogic.Set2Biggest(pStatusFree->b2Biggest);
			m_GameLogic.Set2Biggest(pStatusFree->b2Biggest);
			//时间定义
			m_cbTimeStartGame=pStatusFree->cbTimeStart;
			m_cbTimeOutCard=pStatusFree->cbTimeOutCard;
			m_cbTimeXuanZhan=pStatusFree->cbTimeXuanZhan;
			m_cbTimeFindFriend=pStatusFree->cbTimeFindFriend;
			m_cbTimeAskFriend=pStatusFree->cbTimeAskFriend;
			m_cbTimeAddTimes=pStatusFree->cbTimeAddTimes;
			
			//玩家设置
			if (IsLookonMode()==false)
			{
				//托管按钮
				m_GameClientView.m_btTrustee.ShowWindow(true);
				m_GameClientView.m_btCancelTrustee.ShowWindow(false);
				m_GameClientView.m_btTrustee.EnableWindow(false);

				//开始设置
				if (GetMeUserItem()->GetUserStatus()!=US_READY)
				{
					m_GameClientView.m_btStart.ShowWindow(TRUE);
					WORD wMeChairID=GetMeChairID();
					SetGameClock(wMeChairID,IDI_START_GAME,m_cbTimeStartGame);
				}
			}

			//历史成绩
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//获取变量
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置积分
				pHistoryScore->lTurnScore=pStatusFree->lTurnScore[i];
				pHistoryScore->lCollectScore=pStatusFree->lCollectScore[i];

				//绑定设置
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}

			//旁观设置界面
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
	case GS_PLAY:	//游戏状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//变量定义
			WORD wMeChairID=GetMeChairID();
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//设置变量
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
			//时间定义
			m_cbTimeStartGame=pStatusPlay->cbTimeStart;
			m_cbTimeOutCard=pStatusPlay->cbTimeOutCard;
			m_cbTimeXuanZhan=pStatusPlay->cbTimeXuanZhan;
			m_cbTimeFindFriend=pStatusPlay->cbTimeFindFriend;
			m_cbTimeAskFriend=pStatusPlay->cbTimeAskFriend;
			m_cbTimeAddTimes=pStatusPlay->cbTimeAddTimes;

			//扑克数据
			CopyMemory(m_cbHandCardCount,pStatusPlay->cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(m_cbHandCardData,pStatusPlay->cbHandCardData,m_cbHandCardCount[wMeChairID]*sizeof(BYTE));

			WORD wViewChairID=SwitchViewChairID(m_wBankerUser);

			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//获取位置
				WORD wViewChairID=SwitchViewChairID(i);

				//设置扑克
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

			//庄家设置
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
				//出牌变量
				m_cbTurnCardCount=pStatusPlay->cbTurnCardCount;
				CopyMemory(m_cbTurnCardData,pStatusPlay->cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));
				
				m_GameClientView.SetAddTimes(INVALID_CHAIR,false);
				m_GameClientView.SetAsk(INVALID_CHAIR,false);
				m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);

				//出牌界面
				if (pStatusPlay->wTurnWiner!=INVALID_CHAIR)
				{
					WORD wViewChairID=SwitchViewChairID(pStatusPlay->wTurnWiner);
					m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbTurnCardData,m_cbTurnCardCount);
				}

				//玩家控制
				if (IsLookonMode()==false )	
				{
					m_GameClientView.m_btTrustee.ShowWindow(true);
					m_GameClientView.m_btCancelTrustee.ShowWindow(false);
					m_GameClientView.m_btTrustee.EnableWindow(true);
					m_GameClientView.m_btSortCard.EnableWindow(true);

					UpdateButtonControl();
				}

				//控制设置
				bool bPositively=(IsLookonMode()==false);
				bool bDisplayItem=((IsLookonMode()==false)||(IsAllowLookon()==true));
				m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetPositively(bPositively);
				m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetDisplayItem(bDisplayItem);

				//当前玩家
				if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
				{
					//显示按钮
					m_GameClientView.m_btOutCard.ShowWindow(true);
					m_GameClientView.m_btPassCard.ShowWindow(true);
					m_GameClientView.m_btOutPrompt.ShowWindow(true);

					//启用按钮
					m_GameClientView.m_btPassCard.EnableWindow((m_cbTurnCardCount>0)?true:false);
					m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?true:false);

					//搜索出牌
					if( pStatusPlay->wTurnWiner == wMeChairID )
					{
						m_GameLogic.SearchOutCard(m_cbHandCardData, m_cbHandCardCount[wMeChairID], NULL, 0, &m_SearchCardResult);
					}
					else
					{
						m_GameLogic.SearchOutCard(m_cbHandCardData, m_cbHandCardCount[wMeChairID], m_cbTurnCardData, m_cbTurnCardCount, &m_SearchCardResult);
					}
				}

				//历史成绩
				for (BYTE i=0;i<GAME_PLAYER;i++)
				{
					//获取变量
					tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

					//设置积分
					pHistoryScore->lTurnScore=pStatusPlay->lTurnScore[i];
					pHistoryScore->lCollectScore=pStatusPlay->lCollectScore[i];

					//绑定设置
					WORD wViewChairID=SwitchViewChairID(i);
					m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
					m_GameClientView.SetTrusteeFlag(wViewChairID,pStatusPlay->bTrustee[i]);
				}

				//设置时间
				SetGameClock(m_wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);

				//清除当前操作
				m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
				m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);

				SetFriendFlag();
			}

			for(int i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetGameScore(SwitchViewChairID(i),pStatusPlay->cbGameScore[i]);
				m_GameClientView.SetTurnScore(pStatusPlay->cbTunrScore);
			}

			//设置旁观界面
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

	//错误断言
	ASSERT(FALSE);
	return false;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//设置状态
	SetGameStatus(GS_XUAN_ZHAN);

	//搜索变量
	m_cbSearchResultIndex = 0;
	ZeroMemory( &m_SearchCardResult,sizeof(m_SearchCardResult) );
	m_nCurSearchType = -1;
	m_cbEachSearchIndex = 0;
	ZeroMemory( &m_EachSearchResult,sizeof(m_EachSearchResult) );

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_wBankerUser=pGameStart->wBanker;
	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//历史记录
	m_HistoryCard.ResetData();

	//界面庄家
	m_GameClientView.SetBankerUser(INVALID_CHAIR);

	//成绩窗口
	m_GameClientView.m_ScoreControl.CloseControl();

	//状态设置
	m_GameClientView.SetUserPassState(INVALID_CHAIR,false);
	m_GameClientView.SetUserCountWarn(INVALID_CHAIR,false);

	//玩家扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取位置
		WORD wViewChairID=SwitchViewChairID(i);

		//扑克背面
		m_GameClientView.m_HandCardControl[i].SetBackGround(CARD_BOOR);
		m_GameClientView.m_UserCardControl[i].SetBackGround(CARD_BOOR);

		//设置扑克
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

	//设置扑克
	WORD wMeChairID=GetMeChairID();
	for (WORD i=0;i<GAME_PLAYER;i++) 
		m_cbHandCardCount[i]=NORMAL_CARD_COUNT;

	CopyMemory(m_cbHandCardData,pGameStart->cbCardData,sizeof(m_cbHandCardData));

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//发牌动画
	WORD wViewStartUser=SwitchViewChairID(0);
	m_GameClientView.ShowDispatchCard(m_cbHandCardData, NORMAL_CARD_COUNT, wViewStartUser);

	//环境设置
	if ((IsLookonMode()==false)&&(m_bTrustee==false)) ActiveGameFrame();

	//更新界面
	m_GameClientView.RefreshGameView();
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

	return true;
}
bool CGameClientEngine::OnSubXuanZhan(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_XuanZhan));
	if (wDataSize!=sizeof(CMD_S_XuanZhan)) return false;

	//停止发牌
	m_GameClientView.StopDispatchCard();
	//排列扑克
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],ST_ORDER,true);
	//设置扑克
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetCardData(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()]);
	
	
	KillGameClock(IDI_XUAN_ZHAN);

	//变量定义
	CMD_S_XuanZhan * pPack=(CMD_S_XuanZhan *)pData;

	m_GameClientView.SetXuanZhan(SwitchViewChairID(pPack->wXuanZhanUser),pPack->bXuanZhan);
	m_wCurrentUser = pPack->wCurrentUser;
	m_cbFriendFlag=pPack->cbFriendFlag;
	if(pPack->bXuanZhan)
	{
		m_wXuanZhanUser = pPack->wXuanZhanUser;
		//进入加倍状态
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
			//进入找同盟状态
			SetGameStatus(GS_FIND_FRIEND);
			SetGameClock(m_wBankerUser,IDI_FIND_FRIEND,m_cbTimeFindFriend);
			if(IsLookonMode()==FALSE && m_wBankerUser == GetMeChairID())
			{
				//我开始找盟友
				if (m_bTrustee==false) ActiveGameFrame();
				m_GameClientView.ShowFindFriend(true);
				m_GameClientView.m_bWaitFindFriend=false;
			}
			else
			{
				//等待其他玩家找盟友
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
				//等待其他玩家宣战
				m_GameClientView.m_bWaitXuanZhan=true;
			}
		}
	}


	return true;
}

bool CGameClientEngine::OnSubFindFriend(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_FindFriend));
	if (wDataSize!=sizeof(CMD_S_FindFriend)) return false;

	KillGameClock(IDI_FIND_FRIEND);
	m_GameClientView.m_bWaitFindFriend=false;
	//变量定义
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
{	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_ASKFriend));
	if (wDataSize!=sizeof(CMD_S_ASKFriend)) return false;

	KillGameClock(IDI_ASK_FRIEND);
	m_GameClientView.m_bWaitAskFriend=false;
	m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);
	//变量定义
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
			//有人问了，进入加倍
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
		else//直接开始
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

			//开打允许托管
			m_GameClientView.m_btTrustee.EnableWindow(true);
		}


		m_GameClientView.m_bWaitAskFriend=false;
	}
	else
	{
		if(m_wCurrentUser == m_wBankerUser)
		{
			SetFriendFlag();
			//都没有问，开打
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

			//开打允许托管
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
//用户加倍
bool CGameClientEngine::OnSubAddTimes(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_AddTimes));
	if (wDataSize!=sizeof(CMD_S_AddTimes)) return false;

	m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);
	m_GameClientView.SetAsk(INVALID_CHAIR,false);
	//变量定义
	CMD_S_AddTimes * pAddTimes=(CMD_S_AddTimes *)pData;

	if(pAddTimes->wAddTimesUser != INVALID_CHAIR)
	{
		//设置参数
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
	
	//设置时间
	if (pAddTimes->wCurrentUser!=INVALID_CHAIR)
	{
		//开打了
		KillGameClock(IDI_ADD_TIMES);
		SetGameStatus(GS_PLAY);
		SetGameClock(pAddTimes->wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);

		m_GameClientView.InvalidGameView(0,0,0,0);
		m_wCurrentUser=pAddTimes->wCurrentUser;
		if ((IsLookonMode()==false)&&(pAddTimes->wCurrentUser==GetMeChairID()))
		{
			//激活框架
			ActiveGameFrame();

			//显示按钮
			m_GameClientView.m_btPassCard.ShowWindow(true);
			m_GameClientView.m_btOutCard.ShowWindow(true);
			m_GameClientView.m_btOutPrompt.ShowWindow(true);
			//禁用按钮
			m_GameClientView.m_btPassCard.EnableWindow(false);
			m_GameClientView.m_btOutCard.EnableWindow(false);
			m_GameClientView.m_btOutPrompt.EnableWindow(true);

			//搜索提示
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

		//开打允许托管
		m_GameClientView.m_btTrustee.EnableWindow(true);
	}

	return true;
}

//用户出牌
bool CGameClientEngine::OnSubOutCard(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);
	m_GameClientView.SetTurnScore(pOutCard->cbCurTurnScore);
	BYTE cbTurnCardType=m_GameLogic.GetCardType(pOutCard->cbCardData,pOutCard->cbCardCount);

	//删除时间
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
	//历史清理
	if (m_cbTurnCardCount==0)
	{
		//用户扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//保存数据
			m_HistoryCard.OnEventSaveData(i);

			//桌面清理
			if (i!=pOutCard->wOutCardUser)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(0);
			}
		}

		//放弃标志
		m_GameClientView.SetUserPassState(INVALID_CHAIR,false);
		m_GameClientView.SetAddTimes(INVALID_CHAIR,false);
		m_GameClientView.SetAsk(INVALID_CHAIR,false);
		m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);
	}
	
	//更新记牌器数字
	if(IsLookonMode() == false)
	{
		m_GameClientView.m_RememberCardWnd.UpdateCardCount(pOutCard->cbCardData,pOutCard->cbCardCount);
	}
	

	//出牌动作
	if ((IsLookonMode()==true)||(pOutCard->wOutCardUser!=wMeChairID) || pOutCard->bTrusteeOut)
	{
		//当前扑克
		SwitchToCurrentCard();

		//播放声音
		BYTE cbCardType=m_GameLogic.GetCardType(pOutCard->cbCardData,pOutCard->cbCardCount);
		if ( m_cbTurnCardCount != 0 
			&& !(cbCardType == CT_THREE_LINE_TAKE_TWO)
			&& !(cbCardType == CT_THREE_TAKE_TWO)
			&& !(cbCardType == CT_BOMB_CARD))
		{
			//变量定义
			LPCTSTR pszSoundBoy=TEXT("OUT_CARD_BOY");
			LPCTSTR pszSoundGirl=TEXT("OUT_CARD_GIRL");
			IClientUserItem * pIClientUserItem=GetTableUserItem(pOutCard->wOutCardUser);

			//播放声音
			BYTE cbGender=pIClientUserItem->GetGender();
			PlayGameSound(AfxGetInstanceHandle(),(cbGender==GENDER_FEMALE)?pszSoundGirl:pszSoundBoy);
		}
		else
		{
			//播放声音
			PlayOutCardSound(pOutCard->wOutCardUser,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
		//启动动画
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

		//出牌历史
		m_HistoryCard.OnEventUserOutCard(pOutCard->wOutCardUser,pOutCard->cbCardData,pOutCard->cbCardCount);

		//显示出牌
		WORD wViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);
		m_GameClientView.m_UserCardControl[wViewChairID].ShowOutCard(pOutCard->cbCardData,pOutCard->cbCardCount);
		
		//删除扑克
		if (pOutCard->wOutCardUser==wMeChairID)
		{
			//删除扑克
			BYTE cbSourceCount=m_cbHandCardCount[wMeChairID];
			m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
			m_GameLogic.RemoveCardList(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbHandCardData,cbSourceCount);

			//排列扑克
			m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],m_cbSortType,true);

			//设置扑克
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);
		}
		else
		{
			//设置扑克
			m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardCount[pOutCard->wOutCardUser]);
		}
	}

	//出牌变量
	m_wCurrentUser=pOutCard->wCurrentUser;
	m_cbTurnCardCount=pOutCard->cbCardCount;
	CopyMemory(m_cbTurnCardData,pOutCard->cbCardData,pOutCard->cbCardCount*sizeof(BYTE));

	//报牌警告
	if ( m_cbHandCardCount[pOutCard->wOutCardUser] <= 2 && m_cbHandCardCount[pOutCard->wOutCardUser] > 0 )
	{
		//设置报警
		m_GameClientView.SetUserCountWarn(wViewChairID,true);

		//播放报警
		PlayGameSound(AfxGetInstanceHandle(),TEXT("COUNT_WARN"));
	}
	else
		m_GameClientView.SetUserCountWarn(wViewChairID,false);

	//最大判断
	if (m_wCurrentUser==pOutCard->wOutCardUser)
	{
		//设置用户
		m_wCurrentUser=INVALID_CHAIR;
		m_wMostCardUser=pOutCard->wCurrentUser;

		//出牌变量
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

		//放弃动作
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//用户过虑
			if (pOutCard->wOutCardUser==i) continue;

			//放弃记录
			m_HistoryCard.OnEventSaveData(i);
			m_HistoryCard.OnEventUserPass(i);

			//放弃界面
			WORD wViewChairID=SwitchViewChairID(i);
			m_GameClientView.SetUserPassState(wViewChairID,true);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(0);
		}

		//设置时间
		SetTimer(IDI_MOST_CARD,3000,NULL);
	}

	//更新界面
	m_GameClientView.RefreshGameView();

	return true;
}

//用户放弃
bool CGameClientEngine::OnSubPassCard(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PassCard));
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;

	//变量定义
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pData;

	//设置变量
	m_wCurrentUser=pPassCard->wCurrentUser;

	//放弃设置
	if ((IsLookonMode()==true)||(pPassCard->wPassCardUser!=GetMeChairID()) || pPassCard->bTrusteePass)
	{
		//当前扑克
		SwitchToCurrentCard();

		//删除时间
		KillGameClock(IDI_OUT_CARD);

		//历史记录
		m_HistoryCard.OnEventUserPass(pPassCard->wPassCardUser);

		//放弃设置
		m_GameClientView.SetUserPassState(SwitchViewChairID(pPassCard->wPassCardUser),true);

		//播放声音
		IClientUserItem * pClientUserItem=GetTableUserItem(pPassCard->wPassCardUser);
		PlayGameSound(AfxGetInstanceHandle(),(pClientUserItem->GetGender()==GENDER_FEMALE)?TEXT("PASS_CARD_GIRL"):TEXT("PASS_CARD_BOY"));
	}

	//玩家设置
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//保存数据
		m_HistoryCard.OnEventSaveData(m_wCurrentUser);

		//变量定义
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);

		//门前清理
		m_GameClientView.SetUserPassState(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(0);
	}

	//一轮判断
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

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		//激活框架
		if (m_bTrustee==false)
		{
			ActiveGameFrame();

			//显示按钮
			m_GameClientView.m_btOutCard.ShowWindow(true);
			m_GameClientView.m_btPassCard.ShowWindow(true);
			m_GameClientView.m_btOutPrompt.ShowWindow(true);
			m_GameClientView.m_btPassCard.EnableWindow((m_cbTurnCardCount>0)?true:false);
			m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?true:false);
		}

		//搜索提示
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


	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		SetGameClock(m_wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);

		//清楚当前操作
		m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
		m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);

		m_GameClientView.InvalidGameView(0,0,0,0);
	}

	return true;
}

bool CGameClientEngine::OnSubTrustee(VOID * pData, WORD wDataSize)
{

	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Trustee));
	if (wDataSize!=sizeof(CMD_S_Trustee)) return false;

	//变量定义
	CMD_S_Trustee * pPack=(CMD_S_Trustee *)pData;
	IClientUserItem *pUserItem = GetTableUserItem(pPack->wChair);
	TCHAR strinfo[MAX_PATH]={0};
	if(pPack->bTrustee)
		_sntprintf(strinfo,CountArray(strinfo),_T("玩家%s托管了。"),pUserItem->GetNickName());
	else
		_sntprintf(strinfo,CountArray(strinfo),_T("玩家%s取消托管。"),pUserItem->GetNickName());
	
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

//游戏结束
bool CGameClientEngine::OnSubGameConclude(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameConclude));
	if (wDataSize!=sizeof(CMD_S_GameConclude)) return false;

	//变量定义
	CMD_S_GameConclude * pGameConclude=(CMD_S_GameConclude *)pData;

	//当前状态
	SwitchToCurrentCard();

	//设置状态
	SetGameStatus(GS_GAME_FREE);

	//停止发牌
	m_GameClientView.StopDispatchCard();
	m_GameClientView.SetAddTimes(INVALID_CHAIR,false);
	m_GameClientView.SetAsk(INVALID_CHAIR,false);
	m_GameClientView.SetXuanZhan(INVALID_CHAIR,false);
	//删除时间
	KillTimer(IDI_MOST_CARD);
	KillGameClock(IDI_OUT_CARD);
	KillGameClock(IDI_XUAN_ZHAN);
	KillGameClock(IDI_FIND_FRIEND);
	KillGameClock(IDI_ASK_FRIEND);
	KillGameClock(IDI_ADD_TIMES);
	//控制按钮
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
	//禁用记牌器
	if(IsLookonMode() == false)
	{
		if(m_GameClientView.m_RememberCardWnd.IsWindowActive() == true)
		{
			m_GameClientView.m_RememberCardWnd.DeleteWindow();
		}
		//重置牌链表
		m_GameClientView.m_RememberCardWnd.DestroyCardList();
		m_GameClientView.m_RememberCardWnd.InitializeCardList();
	}

	//变量定义
	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

	//成绩变量
	ScoreInfo.wMeChairID=GetMeChairID();
	ScoreInfo.wBankerUser=m_wBankerUser;
	ScoreInfo.lCellScore=pGameConclude->lCellScore;
	ScoreInfo.cbBaseTimes=pGameConclude->cbBaseTimes;

	CString strOverInfo = TEXT("本局结束,成绩统计: \n");
	//用户成绩
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		IClientUserItem * pIClientUserItem=GetTableUserItem(i);
		tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);


		//设置成绩
		ScoreInfo.lGameScore[i]=pGameConclude->lGameScore[i];
		ScoreInfo.lCollectScore[i]=pHistoryScore->lCollectScore+pGameConclude->lGameScore[i];
		ScoreInfo.cbScore[i]=pGameConclude->cbScore[i];
		ScoreInfo.cbUserTimes[i]=pGameConclude->cbUserTimes[i];
		//用户信息
		ScoreInfo.pIClientUserItem[i] = pIClientUserItem;
		ScoreInfo.dwUserID[i]=pIClientUserItem->GetUserID();
		lstrcpyn(ScoreInfo.szNickName[i],pIClientUserItem->GetNickName(),CountArray(ScoreInfo.szNickName[i]));

		//历史成绩
		m_HistoryScore.OnEventUserScore(i,pGameConclude->lGameScore[i]);

		CString str;
		str.Format(TEXT("%s : %I64d \n"), ScoreInfo.szNickName[i], ScoreInfo.lGameScore[i]);
		strOverInfo += str;
	}

	if ( m_pIStringMessage )
		m_pIStringMessage->InsertSystemString(strOverInfo);

	//显示成绩
	m_GameClientView.ShowGameScoreInfo(ScoreInfo);

	//设置扑克
	BYTE cbCardIndex=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置扑克
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayItem(true);
		m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(&pGameConclude->cbHandCardData[cbCardIndex],pGameConclude->cbCardCount[i]);

		//设置索引
		cbCardIndex+=pGameConclude->cbCardCount[i];
	}

	//设置界面
	m_GameClientView.SetUserPassState(INVALID_CHAIR,false);
	m_GameClientView.SetUserCountWarn(INVALID_CHAIR,false);

	//播放声音
	SCORE lMeScore=pGameConclude->lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOSE"));
	if (lMeScore==0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_CONCLUDE"));

	//取消托管
	if(m_bTrustee)
		OnTrusteeControl(0,0);

	//开始设置
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(true);
		//PerformAutoStart();
		WORD wMeChairID=GetMeChairID();
		SetGameClock(wMeChairID,IDI_START_GAME,m_cbTimeStartGame);
	}

	//更新界面
	m_GameClientView.RefreshGameView();

	return true;
}

bool CGameClientEngine::OnSubFriend(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Friend));
	if (wDataSize!=sizeof(CMD_S_Friend)) return false;

	//变量定义
	CMD_S_Friend * pPack=(CMD_S_Friend *)pData;

	m_cbFriendFlag = pPack->cbFriendFlag;
	CopyMemory(m_wFriend,pPack->wFriend,sizeof(m_wFriend));
	m_wXuanZhanUser = pPack->wXuanZhanUser;
	SetFriendFlag();

	return true;
}

//验证出牌
bool CGameClientEngine::VerdictOutCard()
{
	//状态判断
 	if (m_wCurrentUser!=GetMeChairID()) return false;
 	if (GetGameStatus()!=GS_PLAY)  return false;

	//获取扑克
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_ID].GetShootCard(cbCardData,CountArray(cbCardData));

	//出牌判断
	if (cbShootCount>0L)
	{
		//类型判断
		m_GameLogic.SortCardList(cbCardData,cbShootCount,ST_ORDER);
		if (m_GameLogic.GetCardType(cbCardData,cbShootCount)==CT_ERROR) return false;

		//跟牌判断
		if (m_cbTurnCardCount==0) return true;
		return m_GameLogic.CompareCard(m_cbTurnCardData,cbCardData,m_cbTurnCardCount,cbShootCount);
	}

	return false;
}

//自动开始
bool CGameClientEngine::PerformAutoStart()
{
	//变量定义
	WORD wTableUserCount=0,wReadyUserCount=0;
	IClientUserItem * pIClientUserItem=GetMeUserItem();

	//状态判断
	if ((pIClientUserItem==NULL)||(pIClientUserItem->GetUserStatus()==US_READY)) return false;
	if ((IsLookonMode()==true)||(m_pIClientKernel->GetClockChairID()==IDI_START_GAME)) return false;

	//统计用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pIClientUserItem=GetTableUserItem(i);

		//数目统计
		if (pIClientUserItem!=NULL) 
		{
			wTableUserCount++;
			if (pIClientUserItem->GetUserStatus()==US_READY) wReadyUserCount++;
		}
	}

	//3个准备，1个未准备时启动开始定时器
	if ((wTableUserCount==GAME_PLAYER)&&((wReadyUserCount+1)==wTableUserCount))
	{
		//开始设置
		WORD wMeChairID=GetMeChairID();
		SetGameClock(wMeChairID,IDI_START_GAME,m_cbTimeStartGame);
		return true;
	}

	return true;
}

//当前扑克
bool CGameClientEngine::SwitchToCurrentCard()
{
	//处理判断
	if (m_bLastTurn==false) return false;

	//设置变量
	m_bLastTurn=false;

	//删除时间
	KillTimer(IDI_LAST_TURN);

	//设置界面
	m_GameClientView.SetLastTurnState(false);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);
		BYTE cbEventFlag=m_HistoryCard.GetCurrentEvent(i);

		//获取扑克
		BYTE cbCardData[MAX_CARD_COUNT];
		BYTE cbCardCount=m_HistoryCard.GetCurrentCard(i,cbCardData,CountArray(cbCardData));

		//设置界面
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

	//更新界面
	m_GameClientView.RefreshGameView();

	return true;
}

//自动出牌
bool CGameClientEngine::AutomatismOutCard()
{
	//状态判断
	if (GetGameStatus()!=GS_PLAY) return false;
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID())) return false;

	//当前弹起
	CVirtualButton & btOutCard=m_GameClientView.m_btOutCard;
	if ((btOutCard.IsWindowEnable())&&(m_wCurrentUser==GetMeChairID()))
	{
		OnOutCard(0, 0);
		return true;
	}

	//出牌处理
	if ((m_cbTurnCardCount==0)||(m_bTrustee==true))
	{
		//设置界面
		if (m_SearchCardResult.cbSearchCount>0)
		{
			for(BYTE cbSearchIndex = 0;cbSearchIndex < m_SearchCardResult.cbSearchCount;cbSearchIndex++)
			{
				m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetShootCard( m_SearchCardResult.cbResultCard[cbSearchIndex],m_SearchCardResult.cbCardCount[cbSearchIndex] );
				bool bOutCard=VerdictOutCard();
				if(bOutCard)
				{
					m_GameClientView.m_btOutCard.EnableWindow(TRUE);
					//出牌动作
					OnOutCard(0,0);
					return true;
				}
			}			
			OnPassCard(0,0);
			return true;
		}
	}
	//一轮过后轮到自己
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
			//校验合法性
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
				//出牌动作
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

	//放弃出牌
	if (m_cbTurnCardCount>0) OnPassCard(0,0);

	return true;
}

//更新按钮
VOID CGameClientEngine::UpdateButtonControl()
{
	//效验
	if( IsLookonMode() && !IsAllowLookon() ) return;

	//变量定义
	WORD wMeChairId = GetMeChairID();
	if( m_cbHandCardCount[wMeChairId] == 0 ) return;

	//变量定义
	BYTE cbHandCardCount = m_cbHandCardCount[wMeChairId];
	BYTE cbHandCardData[MAX_CARD_COUNT] = {0};
	CopyMemory( cbHandCardData,m_cbHandCardData,sizeof(BYTE)*cbHandCardCount );

	//排序
	m_GameLogic.SortCardList( cbHandCardData,cbHandCardCount,ST_ORDER,true );

	//清空搜索
	m_nCurSearchType = -1;
	m_cbEachSearchIndex = 0;
	ZeroMemory( &m_EachSearchResult,sizeof(m_EachSearchResult) );

	return;
}

//播放声音
VOID CGameClientEngine::PlayOutCardSound(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT((wChairID<GAME_PLAYER)&&(cbCardCount>0));
	if ((wChairID>=GAME_PLAYER)||(cbCardCount==0)) return;
	
	//获取类型
	BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);

	//获取用户
	IClientUserItem * pIClientUserItem=GetTableUserItem(wChairID);

	//播放声音
	switch (cbCardType)
	{
	case CT_SINGLE:					//单牌类型
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();
			BYTE cbCardValue=m_GameLogic.GetCardValue(cbCardData[0]);

			//构造声音
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("SINGLE_%d_%s"),cbCardValue,(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_DOUBLE:					//对牌类型
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();

			//构造声音
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("DOUBLE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_THREE:					//三条类型
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();

			//构造声音
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("THREE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_SINGLE_LINE:			//单连类型
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();

			//构造声音
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("SINGLE_LINE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_DOUBLE_LINE:			//对连类型
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();

			//构造声音
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("DOUBLE_LINE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_THREE_LINE:				//三连类型
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();

			//构造声音
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("THREE_LINE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_THREE_TAKE_ONE:			//三带一
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();

			//构造声音
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("THREE_TAKE_ONE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_THREE_TAKE_TWO:			//三带二
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();

			//构造声音
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("THREE_TAKE_TWO_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_THREE_LINE_TAKE_ONE:
	case CT_THREE_LINE_TAKE_TWO:	
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();

			//构造声音
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("THREE_ONE_LINE_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	case CT_BOMB_CARD:				//炸弹类型
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();

			//构造声音
			TCHAR szSoundName[32]=TEXT("");
			_sntprintf(szSoundName,CountArray(szSoundName),TEXT("BOMB_CARD_%s"),(cbGender==GENDER_FEMALE)?TEXT("GIRL"):TEXT("BOY"));

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),szSoundName);

			return;
		}
	}

	return;
}


//用户状态
VOID CGameClientEngine::OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//变量定义
	IClientUserItem * pIMySelfUserItem=GetMeUserItem();
	BYTE cbUserStatus=pIClientUserItem->GetUserStatus();

	//停止开始
	if ((pIClientUserItem==pIMySelfUserItem)&&(cbUserStatus>=US_READY)) 
	{
		KillGameClock(IDI_START_GAME);
		m_GameClientView.m_btStart.ShowWindow(false);
	}
}

//时间消息
VOID CGameClientEngine::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case IDI_MOST_CARD:			//最大扑克
		{
			//用户效验
			ASSERT(m_wMostCardUser!=INVALID_CHAIR);
			if (m_wMostCardUser==INVALID_CHAIR) return;

			//动画判断
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_GameClientView.m_UserCardControl[i].IsOutCard()==true)
				{
					return;
				}
			}

			//删除时间
			KillTimer(IDI_MOST_CARD);

			//设置变量
			m_wCurrentUser=m_wMostCardUser;
			m_wMostCardUser=INVALID_CHAIR;

			//上轮设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_HistoryCard.OnEventSaveData(i);
			}

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetUserPassState(i,false);
				m_GameClientView.m_UserCardControl[i].SetCardData(0);
			}

			//玩家设置
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				//激活框架
				if (m_bTrustee==false) ActiveGameFrame();

				//设置按钮
				m_GameClientView.m_btOutCard.ShowWindow(true);
				m_GameClientView.m_btPassCard.ShowWindow(true);
				m_GameClientView.m_btPassCard.EnableWindow(false);
				m_GameClientView.m_btOutPrompt.ShowWindow(true);
				m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?true:false);

				//搜索提示
				try
				{
					m_GameLogic.SearchOutCard(m_cbHandCardData, m_cbHandCardCount[m_wCurrentUser], m_cbTurnCardData, m_cbTurnCardCount, &m_SearchCardResult);
				}catch(...)
				{
					ASSERT(FALSE);
					m_SearchCardResult.cbSearchCount = 0;
				}
			}

			//设置时间
			SetGameClock(m_wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);

			//清楚当前操作
			m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
			m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);

			m_GameClientView.InvalidGameView(0,0,0,0);

			return;
		}
	case IDI_LAST_TURN:			//上轮扑克
		{
			//还原界面
			SwitchToCurrentCard();

			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//开始消息
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	//删除时间
	KillGameClock(IDI_START_GAME);

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetDisplayItem(false);
	}

	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetPositively(false);

	//界面庄家
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	//状态设置
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
	//设置界面
	m_GameClientView.m_btStart.ShowWindow(false);
	m_GameClientView.m_ScoreControl.CloseControl();

	//更新界面
	m_GameClientView.RefreshGameView();

	//发送消息
	SendUserReady(NULL,0);

	return 0L;
}

//出牌消息
LRESULT CGameClientEngine::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowEnable()==false) return 0;
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==false) return 0;

	//当前扑克
	SwitchToCurrentCard();

	//删除时间
	KillGameClock(IDI_OUT_CARD);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_cbSearchResultIndex = 0;
	m_SearchCardResult.cbSearchCount = 0;

	//设置界面
	m_GameClientView.m_btOutCard.ShowWindow(false);
	m_GameClientView.m_btPassCard.ShowWindow(false);
	m_GameClientView.m_btOutPrompt.ShowWindow(false);
	m_GameClientView.m_btOutCard.EnableWindow(false);
	m_GameClientView.m_btPassCard.EnableWindow(false);

	//获取扑克
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_ID].GetShootCard(cbCardData,CountArray(cbCardData));

	//排列扑克
	m_GameLogic.SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//出牌设置
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].RemoveShootItem();
	m_GameClientView.m_UserCardControl[ME_VIEW_ID].ShowOutCard(cbCardData,cbCardCount);

	//删除扑克
	WORD wMeChairID=GetMeChairID();
	m_cbHandCardCount[wMeChairID]-=cbCardCount;
	m_GameLogic.RemoveCardList(cbCardData,cbCardCount,m_cbHandCardData,m_cbHandCardCount[wMeChairID]+cbCardCount);

	//按钮设置
	UpdateButtonControl();

	//播放声音
	BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);
	if ( m_cbTurnCardCount != 0 
		&& !(cbCardType == CT_THREE_LINE_TAKE_TWO)
		&& !(cbCardType == CT_THREE_TAKE_TWO)
		&& !(cbCardType == CT_BOMB_CARD))
	{
		//变量定义
		LPCTSTR pszSoundBoy=TEXT("OUT_CARD_BOY");
		LPCTSTR pszSoundGirl=TEXT("OUT_CARD_GIRL");
		IClientUserItem * pIClientUserItem=GetTableUserItem(wMeChairID);
		
		//播放声音
		BYTE cbGender=pIClientUserItem->GetGender();
		PlayGameSound(AfxGetInstanceHandle(),(cbGender==GENDER_FEMALE)?pszSoundGirl:pszSoundBoy);
	}
	else
	{
		//播放声音
		PlayOutCardSound(wMeChairID,cbCardData,cbCardCount);
	}

	//启动动画
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

	//出牌历史
	m_HistoryCard.OnEventUserOutCard(wMeChairID,cbCardData,cbCardCount);

	//构造数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardCount=cbCardCount;
	CopyMemory(OutCard.cbCardData,cbCardData,cbCardCount*sizeof(BYTE));

	//发送数据
	SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)-sizeof(OutCard.cbCardData)+OutCard.cbCardCount*sizeof(BYTE));

	return 0;
}

//PASS消息
LRESULT CGameClientEngine::OnPassCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btPassCard.IsWindowEnable()==false) return 0;
	if (m_GameClientView.m_btPassCard.IsWindowVisible()==false) return 0;

	//当前扑克
	SwitchToCurrentCard();

	//删除时间
	KillGameClock(IDI_OUT_CARD);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_cbSearchResultIndex = 0;
	m_SearchCardResult.cbSearchCount = 0;

	//设置界面
	m_GameClientView.m_btOutCard.ShowWindow(false);
	m_GameClientView.m_btPassCard.ShowWindow(false);
	m_GameClientView.m_btOutPrompt.ShowWindow(false);
	m_GameClientView.m_btOutCard.EnableWindow(false);
	m_GameClientView.m_btPassCard.EnableWindow(false);

	//历史记录
	m_HistoryCard.OnEventUserPass(GetMeChairID());

	//设置放弃
	m_GameClientView.SetUserPassState(ME_VIEW_ID,true);

	//设置扑克
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetShootCard(NULL,0);

	//更新界面
	m_GameClientView.RefreshGameView();

	//播放声音
	IClientUserItem * pClientUserItem=GetMeUserItem();
	PlayGameSound(AfxGetInstanceHandle(),(pClientUserItem->GetGender()==GENDER_FEMALE)?TEXT("PASS_CARD_GIRL"):TEXT("PASS_CARD_BOY"));

	//发送数据
	SendSocketData(SUB_C_PASS_CARD);

	return 0L;
}

//提示消息
LRESULT CGameClientEngine::OnOutPrompt(WPARAM wParam, LPARAM lParam)
{
	WORD wMeChairID = GetMeChairID();

	//有大过牌
	if( m_SearchCardResult.cbSearchCount > 0 )
	{
		//设置界面
		m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetShootCard(m_SearchCardResult.cbResultCard[m_cbSearchResultIndex],
			m_SearchCardResult.cbCardCount[m_cbSearchResultIndex]);

		//设置控件
		bool bOutCard=VerdictOutCard();
		m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);

		//更新界面
		m_GameClientView.InvalidGameView(0,0,0,0);

		//设置变量
		m_cbSearchResultIndex = (m_cbSearchResultIndex+1)%m_SearchCardResult.cbSearchCount;

		return 0;
	}

	//放弃出牌
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
			//校验合法性
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
				//出牌动作
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

//叫分消息
LRESULT CGameClientEngine::OnCallScore(WPARAM wParam, LPARAM lParam)
{

	return 0L;
}
LRESULT CGameClientEngine::OnXuanZhan(WPARAM wParam, LPARAM lParam)
{
	//删除时间
	KillGameClock(IDI_XUAN_ZHAN);

	//设置界面
	m_GameClientView.m_btXuan.ShowWindow(false);
	m_GameClientView.m_btNoXuan.ShowWindow(false);

	//发送数据
	CMD_C_XuanZhan CallScore;
	CallScore.bXuanZhan=(BYTE)wParam;
	SendSocketData(SUB_C_XUAN_ZHAN,&CallScore,sizeof(CallScore));
	return 0L;
}

LRESULT CGameClientEngine::OnFindFriend(WPARAM wParam, LPARAM lParam)
{
	//删除时间
	KillGameClock(IDI_FIND_FRIEND);

	//发送数据
	CMD_C_FindFriend pack;
	pack.cbCardData=(BYTE)wParam;
	SendSocketData(SUB_C_FIND_FRIEND,&pack,sizeof(pack));

	return 0L;
}

LRESULT CGameClientEngine::OnAskFriend(WPARAM wParam, LPARAM lParam)
{
	//删除时间
	KillGameClock(IDI_ASK_FRIEND);

	//设置界面
	m_GameClientView.m_btAsk.ShowWindow(false);
	m_GameClientView.m_btNoAsk.ShowWindow(false);
	m_GameClientView.m_btRespond.ShowWindow(false);
	m_GameClientView.m_btNoRespond.ShowWindow(false);
	//发送数据
	CMD_C_AskFriend pack;
	pack.bAsk=(BYTE)wParam;
	SendSocketData(SUB_C_ASK_FRIEND,&pack,sizeof(pack));
	return 0L;
}

LRESULT CGameClientEngine::OnAddTimes(WPARAM wParam, LPARAM lParam)
{
	//删除时间
	KillGameClock(IDI_ADD_TIMES);

	//设置界面
	m_GameClientView.m_btAdd.ShowWindow(false);
	m_GameClientView.m_btNoAdd.ShowWindow(false);

	//发送数据
	CMD_C_AddTimes pack;
	pack.bAddTimes=(BYTE)wParam;
	SendSocketData(SUB_C_ADD_TIMES,&pack,sizeof(pack));

	m_GameClientView.m_bWaitAddTimes=true;
	return 0L;
}
//左键扑克
LRESULT CGameClientEngine::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	//设置控件
	bool bOutCard=VerdictOutCard();
	m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);

	return 0L;
}

//排列扑克
LRESULT CGameClientEngine::OnSortHandCard(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_cbSortType=(m_cbSortType!=ST_ORDER)?ST_ORDER:ST_COUNT;

	//排列扑克
	WORD wMeChairID=GetMeChairID();
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],m_cbSortType,TRUE);

	//获取扑克
	BYTE cbShootCard[MAX_CARD_COUNT];
	BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_ID].GetShootCard(cbShootCard,CountArray(cbShootCard));

	//设置扑克
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetCardData(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);

	//提起扑克
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetShootCard(cbShootCard,cbShootCount);

	//设置按钮
	LPCTSTR pszButtonImage=(m_cbSortType==ST_ORDER)?TEXT("BT_SORT_CARD_COUNT"):TEXT("BT_SORT_CARD_ORDER");
	m_GameClientView.m_btSortCard.SetButtonImage(&m_GameClientView.m_D3DDevice,pszButtonImage,TEXT("PNG"),AfxGetInstanceHandle());

	//更新界面
	m_GameClientView.RefreshGameView();

	return 0;
}

//查看上轮
LRESULT CGameClientEngine::OnLastTurnCard(WPARAM wParam, LPARAM lParam)
{
	if (m_bLastTurn==false)
	{
		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//变量定义
			WORD wViewChairID=SwitchViewChairID(i);
			BYTE cbEventFlag=m_HistoryCard.GetHistoryEvent(i);

			//获取扑克
			BYTE cbCardData[MAX_CARD_COUNT];
			BYTE cbCardCount=m_HistoryCard.GetHistoryCard(i,cbCardData,CountArray(cbCardData));

			//设置界面
			m_GameClientView.SetUserPassState(wViewChairID,cbEventFlag==AF_PASS);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(cbCardData,cbCardCount);
		}

		//设置时间
		SetTimer(IDI_LAST_TURN,3000,NULL);

		//环境设置
		m_bLastTurn=true;
		m_GameClientView.SetLastTurnState(true);

		//更新界面
		m_GameClientView.RefreshGameView();
	}
	else
	{
		//当前扑克
		SwitchToCurrentCard();
	}

	return 0;
}

//拖管控制
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

	//更新界面
	m_GameClientView.RefreshGameView();
	return 0L;
}

//启用上轮
LRESULT CGameClientEngine::OnEnableHistory(WPARAM wParam, LPARAM lParam)
{
	//启用上轮
	m_GameClientView.m_btLastTurn.EnableWindow(true);

	return 0;
}

//出牌完成
LRESULT CGameClientEngine::OnOutCardFinish(WPARAM wParam, LPARAM lParam)
{
	//动画判断
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_GameClientView.m_UserCardControl[i].IsOutCard()==true)
		{
			return 0L;
		}
	}

	//出牌设置
	if ((GetGameStatus()==GS_PLAY)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		//变量定义
		WORD wMeChairID=GetMeChairID();

		//清理扑克
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(0);

		//出牌按钮
		if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
		{
			//激活框架
			if (m_bTrustee==false) 
			{
				ActiveGameFrame();

				//显示按钮
				m_GameClientView.m_btOutCard.ShowWindow(true);
				m_GameClientView.m_btPassCard.ShowWindow(true);
				m_GameClientView.m_btOutPrompt.ShowWindow(true);

				//启用按钮
				m_GameClientView.m_btPassCard.EnableWindow((m_cbTurnCardCount>0)?true:false);
				m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?true:false);
			}
			//搜索提示
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

		//设置时间
		SetGameClock(m_wCurrentUser,IDI_OUT_CARD,m_cbTimeOutCard);

		//清除当前操作
		m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
		m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);

		m_GameClientView.InvalidGameView(0,0,0,0);
	}

	//更新按钮
	UpdateButtonControl();

	return 0L;
}

//发牌完成
LRESULT CGameClientEngine::OnDispatchFinish(WPARAM wParam, LPARAM lParam)
{
	//状态效验
	ASSERT(GetGameStatus()==GS_XUAN_ZHAN);
	if (GetGameStatus()!=GS_XUAN_ZHAN) return 0L;

	//设置界面
	m_GameClientView.m_btSortCard.EnableWindow(!IsLookonMode());

	//排列扑克
	if(m_GameLogic.IsHave510K(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()]))
		m_cbSortType = ST_510K;
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],m_cbSortType,true);

	//设置扑克
	m_GameClientView.m_HandCardControl[ME_VIEW_ID].SetCardData(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()]);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	//玩家控制
	if (IsLookonMode()==false)	
	{
		m_GameClientView.m_btSortCard.EnableWindow(true);
		//m_GameClientView.m_btRememberCard.ShowWindow(TRUE);
		//m_GameClientView.m_btRememberCard.EnableWindow(TRUE);

		//显示按钮
		if (GetMeChairID()==m_wCurrentUser)
		{
			//控制按钮
			m_GameClientView.m_btXuan.EnableWindow(true);
			m_GameClientView.m_btNoXuan.EnableWindow(true);

			//显示按钮
			m_GameClientView.m_btXuan.ShowWindow(true);
			m_GameClientView.m_btNoXuan.ShowWindow(true);
		}
		else
			m_GameClientView.m_bWaitXuanZhan=true;
	}
	
	//更新按钮
	UpdateButtonControl();

	//设置时间
	SetGameClock(m_wCurrentUser,IDI_XUAN_ZHAN,m_cbTimeXuanZhan);

	//清楚当前操作
	m_GameClientView.m_UserCardControl[SwitchViewChairID(m_wCurrentUser)].SetCardData(0);
	m_GameClientView.SetUserPassState(SwitchViewChairID(m_wCurrentUser), false);

	m_GameClientView.InvalidGameView(0,0,0,0);

	//激活框架
	if ((IsLookonMode()==false)&&(m_bTrustee==false)) ActiveGameFrame();

	return 0L;
}

//声音控制
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

	if(m_cbFriendFlag == 1)//宣战
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
	else if(m_cbFriendFlag == 2)//暗独
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
	else if(m_cbFriendFlag == 3)//暗独明示
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
	else if(m_cbFriendFlag == 4)//明独
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
	else if(m_cbFriendFlag == 5)//选队友
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
	else if(m_cbFriendFlag == 6)//队友明示
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
