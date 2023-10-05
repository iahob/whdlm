#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "RenderCanvas.h"
#include ".\gameclientdlg.h"
#include ".\gamesound.h"
//#include ".\log\log.h"
#include "ccrashcatch.h"
CGameClientDlg*	g_GameClientDlg = NULL;
//////////////////////////////////////////////////////////////////////////

//计时器标识
#define IDI_FREE					99									//空闲时间
#define IDI_PLACE_JETTON			100									//下注时间
#define IDI_DISPATCH_CARD			301									//发牌时间

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameEngine)
	//ON_MESSAGE(IDM_APPLY_BANKER, OnApplyBanker)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg()
{
	//限制信息
	m_iMeMaxScore=0L;
	m_iApplyBankerCondition=0L;

	//个人下注
	ZeroMemory(m_iMeAreaScoreArray, sizeof(m_iMeAreaScoreArray));

	//庄家信息
	m_iBankerScore=0L;
	m_wCurrentBanker=0L;

	//状态变量
	m_bMeApplyBanker=false;
	g_GameClientDlg = this;

	return;

	
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
 
	//CLog::Instance()->UnInit();
	//CLog::DestroyInstance();
	 
}

//初始函数
bool CGameClientDlg::OnInitGameEngine()
{
	SetUnhandledExceptionFilter(CleanToolExceptionFun);
	//CLog::Instance()->Init("3d森林王国客户端日志","3d Animal");
	//LOG_FILE("日志启动");
	//设置标题
	SetWindowText(TEXT("3D森林王国游戏"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);//少了这一句，出现除0错误
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//加载声音
	VERIFY(m_DTSDBackground.Create(TEXT("BACK_GROUND")));
	VERIFY(m_DTSDCheer[0].Create(TEXT("CHEER1")));
	VERIFY(m_DTSDCheer[1].Create(TEXT("CHEER2")));
	VERIFY(m_DTSDCheer[2].Create(TEXT("CHEER3")));
	//VERIFY(m_DTStopJetton.Create("STOP_JETTON"));

	return true;
}

void CGameClientDlg::OnIdle()
{
	m_GameClientView.OnIdle();
	GameSound::Instance()->SetEnable(true);
}

//时钟删除
bool CGameClientDlg::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//重置框架
bool CGameClientDlg::OnResetGameEngine()
{
	//限制信息
	m_iMeMaxScore=0L;			

	//个人下注
	ZeroMemory(m_iMeAreaScoreArray, sizeof(m_iMeAreaScoreArray));

	//庄家信息
	m_iBankerScore=0L;
	m_wCurrentBanker=0L;

	//状态变量
	m_bMeApplyBanker=false;

	return true;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	return;
}

//时间消息
bool CGameClientDlg::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD nTimerID)
{
	if ((nTimerID==IDI_PLACE_JETTON)&&(nElapse==0))
	{

	}

	//if (IsEnableSound()) 
	{
		if (nTimerID==IDI_PLACE_JETTON&&nElapse<=5) 
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("TIME_WARIMG"));
			if( nElapse <= 0 )
			{
				PlayGameSound(AfxGetInstanceHandle(), TEXT("STOP_JETTON"));
			}
		}
	}

	return true;
}

//旁观状态
bool CGameClientDlg::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

//网络消息
bool CGameClientDlg::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
	//_RPT1(_CRT_WARN, "OnGameMessage = %d\n", wSubCmdID);
	//char str[32];
	//_snprintf(str,32,"OnGameMessage is %d",wSubCmdID);
	//LOG_FILE(str);
	try 
	{
		switch (wSubCmdID)
		{
		case SUB_S_GAME_FREE:		//游戏空闲
			{
				return OnSubGameFree(pBuffer,wDataSize);
			}
		case SUB_S_GAME_START:		//游戏开始
			{
				return OnSubGameStart(pBuffer,wDataSize);
			}
		case SUB_S_GAME_END:		//游戏结束
			{
				return OnSubGameEnd(pBuffer,wDataSize);
			}
		case SUB_S_SEND_RECORD:		//游戏记录
			{
				return OnSubGameRecord(pBuffer,wDataSize);			
			}
		case SUB_S_PLACE_JETTON:	// 用户下注回调
			{
				return OnSubPlaceJetton(pBuffer, wDataSize);
			}
		case SUB_S_PLACE_JETTON_FAIL:	//下注失败
			{
				return OnSubPlaceJettonFail(pBuffer,wDataSize);
			}
		case SUB_S_CLEAR_JETTON:		// 下注清空
			{
				return OnSubJettonClear(pBuffer, wDataSize);
			}
		case SUB_S_APPLY_BANKER:	//申请做庄
			{
				//AfxMessageBox("sdfsdfsdf");
				//return OnSubUserApplyBanker(pBuffer, wDataSize);
			}
		case SUB_S_CANCEL_BANKER:	//取消做庄
			{
				//AfxMessageBox("1111111111111111");
				//return OnSubUserCancelBanker(pBuffer, wDataSize);
			}
		case SUB_S_CHANGE_BANKER:	//切换庄家
			{
				//AfxMessageBox("3333333333333");
				//return OnSubChangeBanker(pBuffer, wDataSize);
			}
		case SUB_S_SEND_INFO:
			{
				ASSERT(wDataSize==sizeof(CMD_S_Control));
				if(wDataSize!=sizeof(CMD_S_Control)) return false;

				if( NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd() )
				{
					m_GameClientView.m_pClientControlDlg->ReqResult(pBuffer);
				}

				return true;
			}
		case SUB_S_ACK_UPDATE_STORAGE:
		case SUB_S_ACK_SET_PRIZE:
			{
				ASSERT(wDataSize==sizeof(CMD_S_ACK_Control));
				if(wDataSize!=sizeof(CMD_S_ACK_Control)) return false;

				if( NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd() )
				{
					m_GameClientView.m_pClientControlDlg->AckReq(pBuffer);
				}

				return true;

			}
		}
	}
	catch(...)
	{
		TCHAR str[32];
		_sntprintf(str,32,TEXT("OnGameMessage error is %d"),wSubCmdID);
		AfxMessageBox((str));
	}

	//错误断言
	//ASSERT(FALSE);

	return true;
}

//游戏场景
bool CGameClientDlg::OnEventSceneMessage(BYTE cbGameStation, bool bLookonOther, VOID * pBuffer, WORD wDataSize)
{
	//_RPT1(_CRT_WARN, "OnGameSceneMessage = %d\n", cbGameStation);
	//char str[32];
	//_snprintf(str,32,"OnGameSceneMessage is %d",cbGameStation);
	//LOG_FILE(str);
	try 
	{
	switch (cbGameStation)
	{
	case GAME_STATUS_FREE:			//空闲状态
		{
			//效验数据
        	ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) 
				return false;

			//消息处理
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;
			if( pStatusFree == NULL )
				return FALSE;

			//设置时间
			SetGameClock(GetMeChairID(),IDI_FREE,pStatusFree->cbTimeLeave);

			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

			if(RenderCanvas::Instance()->IsValid())
				RenderCanvas::Instance()->OnGSFree(pBuffer, wDataSize);

            //玩家信息
// 			m_iMeMaxScore=pStatusFree->iUserMaxScore;
// 
// 			m_GameClientView.SetMeMaxScore(m_iMeMaxScore);
// 			WORD wMeChairID=GetMeChairID();
// 		
// 			WORD wSwitchViewChairID=SwitchViewChairID(wMeChairID);
// 			m_GameClientView.SetMeChairID(wSwitchViewChairID);
// 			
//             //庄家信息
// 			SetBankerInfo(pStatusFree->wBankerUser,pStatusFree->iBankerScore);
// 			m_GameClientView.SetBankerScore(pStatusFree->cbBankerTime,pStatusFree->iBankerWinScore);
// 			m_bEnableSysBanker=pStatusFree->bEnableSysBanker;
// 			m_GameClientView.EnableSysBanker(m_bEnableSysBanker);
// 
//             //控制信息
// 			m_iApplyBankerCondition=pStatusFree->iApplyBankerCondition;
// 			m_GameClientView.SetAreaLimitScore(pStatusFree->iAreaLimitScore, JETTON_AREA_COUNT);
// 
// 			//设置状态
// 			SetGameStatus(GS_FREE);
			// 
			// 			//更新控制
			// 			UpdateButtonContron();
			// 			m_GameClientView.UpdateGameView(NULL);

			return true;
		}break;
	case GS_PLACE_JETTON:	//游戏状态
	case GS_GAME_END:		//结束状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) 
				return false;

			//消息处理
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
			if( pStatusPlay == NULL )
				return FALSE;

			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

			if(pStatusPlay->cbGameStatus==GS_PLACE_JETTON)
			{
				if(RenderCanvas::Instance()->IsValid())
					RenderCanvas::Instance()->OnGSPlaying(pBuffer, wDataSize);
			}
			else
			{
				if(RenderCanvas::Instance()->IsValid())
					RenderCanvas::Instance()->OnGSEnd(pBuffer, wDataSize);
			}

			////全局下注
			//	for( int i=0; i<JETTON_AREA_COUNT; i++)
			//		m_GameClientView.PlaceUserJetton(i, pStatusPlay->iTotalAreaScore[i]);

			//	//玩家下注
			//	for( int i=0; i<JETTON_AREA_COUNT; i++ )
			//		SetMePlaceJetton(i, pStatusPlay->iUserAreaScore[i]);

			//	//玩家积分
			//	m_iMeMaxScore=pStatusPlay->iUserMaxScore;			
			//	m_GameClientView.SetMeMaxScore(m_iMeMaxScore);
			//	WORD wMeChairID=GetMeChairID();
			//	m_GameClientView.SetMeChairID(SwitchViewChairID(wMeChairID));

			//	//控制信息
			//	m_iApplyBankerCondition=pStatusPlay->iApplyBankerCondition;
			//	m_GameClientView.SetAreaLimitScore(pStatusPlay->iAreaScoreLimit, JETTON_AREA_COUNT);

			//	if (pStatusPlay->cbGameStatus==GS_GAME_END)
			//	{
			//		//设置成绩
			//		m_GameClientView.SetCurGameScore(pStatusPlay->iEndUserScore,pStatusPlay->iEndUserReturnScore,pStatusPlay->iEndBankerScore,pStatusPlay->iEndRevenue);
			//	}
			//	else
			//	{
			//		//播放声音
			//		if (IsEnableSound()) m_DTSDBackground.Play(0,true);
			//	}

			//	//庄家信息
			//	SetBankerInfo(pStatusPlay->wBankerUser,pStatusPlay->iBankerScore);
			//	m_GameClientView.SetBankerScore(pStatusPlay->cbBankerTime,pStatusPlay->iBankerWinScore);
			//	m_bEnableSysBanker=pStatusPlay->bEnableSysBanker;
			//	m_GameClientView.EnableSysBanker(m_bEnableSysBanker);

			//	//设置状态
			//	SetGameStatus(pStatusPlay->cbGameStatus);

			//	//设置时间
			//	SetGameTimer(GetMeChairID(),pStatusPlay->cbGameStatus==GS_GAME_END?IDI_DISPATCH_CARD:IDI_PLACE_JETTON,pStatusPlay->cbTimeLeave);

			//更新按钮
			//	UpdateButtonContron();
			//	m_GameClientView.UpdateGameView(NULL);

			return true;
		}break;
	}
	}
	catch(...)
	{
		TCHAR str[32];
		_sntprintf(str,32,TEXT("OnGameMessage error is %d"),cbGameStation);
		AfxMessageBox((str));
	}
	return false;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//_RPT0(_CRT_WARN, "OnSubGameStart\n");
	//效验数据
	int pp=sizeof(CMD_S_GameStart);
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) 
		return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
	if( pGameStart == NULL )
	{
		_ASSERT(false);
		return FALSE;
	}
	
	if(RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnSubGameStart(pBuffer, wDataSize);


	return true;
}

//游戏空闲
bool CGameClientDlg::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//_RPT0(_CRT_WARN, "OnSubGameFree\n");

	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameFree));
	if (wDataSize!=sizeof(CMD_S_GameFree)) 
		return false;

	//消息处理
	CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;
	if( pGameFree == NULL )
	{
		_ASSERT(false);
		return false;
	}
	
	if(RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnSubGameFree(pBuffer, wDataSize);

	//设置时间
	//SetGameTimer(GetMeChairID(),IDI_FREE,pGameFree->cbTimeLeave);


	//UpdateButtonContron();

	return true;
}
//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	_RPT0(_CRT_WARN, "OnSubGameEnd\n");
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) 
		return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	if( pGameEnd == NULL )
	{
		_ASSERT(false);
		return FALSE;
	}

	if(RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnSubGameEnd(pBuffer, wDataSize);
	
	return true;
}
// 游戏记录
bool CGameClientDlg::OnSubGameRecord(const void * pBuffer, WORD wDataSize)
{
	_RPT0(_CRT_WARN, "OnSubGameRecord\n");
	//效验数据
	ASSERT(wDataSize % sizeof(tagServerGameRecord) == 0);
	if (wDataSize % sizeof(tagServerGameRecord) != 0) 
		return false;

	//消息处理
	tagServerGameRecord * pRecordInfo=(tagServerGameRecord *)pBuffer;
	if( pRecordInfo == NULL )
	{
		_ASSERT(false);
		return false;
	}

 //
	//char str[64];
	//_snprintf(str,64,"OnSubGameRecord wDataSize is %d",wDataSize);
	//LOG_FILE(str);

	if(RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnSubGameRecord(pBuffer, wDataSize);

	return true;
}

//用户加注
bool CGameClientDlg::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)
{
	_RPT0(_CRT_WARN, "OnSubPlaceJetton\n");
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJetton));
	if (wDataSize!=sizeof(CMD_S_PlaceJetton)) 
		return false;

	//消息处理
	CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;
	if( pPlaceJetton == NULL )
	{
		_ASSERT(false);
		return false;
	}

	if(RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnSubPlaceJetton(pBuffer, wDataSize);
    
	return true;
}
//下注清空
bool CGameClientDlg::OnSubJettonClear(const void * pBuffer, WORD wDataSize)
{
	_RPT0(_CRT_WARN, "OnSubJettonClear\n");
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ClearJetton));
	if (wDataSize!=sizeof(CMD_S_ClearJetton)) 
		return false;

	//消息处理
	CMD_S_ClearJetton* pPlaceJettonClear=(CMD_S_ClearJetton*)pBuffer;
	if( pPlaceJettonClear == NULL )
	{
		_ASSERT(false);
		return FALSE;
	}

	if(RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnSubJettonClear(pBuffer, wDataSize);

	return true;
}
//下注失败
bool CGameClientDlg::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
	_RPT0(_CRT_WARN, "OnSubPlaceJettonFail\n");
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJettonFail));
	if (wDataSize!=sizeof(CMD_S_PlaceJettonFail)) 
		return false;

	//消息处理
	CMD_S_PlaceJettonFail* pPlaceJettonFail=(CMD_S_PlaceJettonFail *)pBuffer;
	if( pPlaceJettonFail == NULL )
	{
		_ASSERT(false);
		return FALSE;
	}

	if(RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnSubPlaceJettonFail(pBuffer, wDataSize);

	return true;
}



//加注消息
void CGameClientDlg::OnPlaceJetton(BYTE iJettonArea, __int64 iJettonNum)
{
	//变量定义
	BYTE cbJettonArea=iJettonArea;			//下注区域
	__int64 iJettonScore=iJettonNum;		//筹码数量

	if( cbJettonArea > ID_SMALL_SNAKE || cbJettonArea < ID_BIG_TIGER )
		return;

	//庄家不能下注
	if ( GetMeChairID() == m_wCurrentBanker )
		return;

	//非下注状态，直接返回
	if (GetGameStatus()!=GS_PLACE_JETTON)
	{
		//UpdateButtonContron();
		return;
	}

	//根据下注区域，设置本人下注筹码
	m_iMeAreaScoreArray[cbJettonArea] += iJettonScore;
	//m_GameClientView.SetMePlaceJetton(cbJettonArea, m_iMeAreaScoreArray[cbJettonArea]);

	//变量定义
	CMD_C_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

	//构造变量
// 	PlaceJetton.cbJettonArea=cbJettonArea;
// 	PlaceJetton.iJettonScore=iJettonScore;
// 
// 	//发送网络消息
// 	SendData(SUB_C_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

	//更新按钮
	//UpdateButtonContron();

	//播放声音
	//if (IsEnableSound()) 
	{
		if (iJettonScore==5000000) 
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
		else 
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
		m_DTSDCheer[rand()%3].Play();
	}

	return;
}

int CGameClientDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGameFrameEngine::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 禁止缩放窗口
	//ModifyStyle(WS_MAXIMIZE, 0);
	//ModifyStyle(WS_MAXIMIZEBOX , 0);
	//ModifyStyle(WS_SIZEBOX , 0);

	return 0;
}

void CGameClientDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CGameFrameEngine::OnKeyDown(nChar, nRepCnt, nFlags);
}

//申请做庄
bool CGameClientDlg::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
//	//效验数据
//	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
//	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) 
//		return false;
//
//	//消息处理
//	CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;
//	if( pApplyBanker == NULL )
//		return FALSE;
//
//	//获取玩家
//	//tagUserData const *pUserData=GetUserData(pApplyBanker->wApplyUser);
//	//if( pUserData == NULL )
//	//	return FALSE;
//
//	//插入玩家
////	if (m_wCurrentBanker != pApplyBanker->wApplyUser)
//	{
//		tagApplyUser ApplyUser;
//		::ZeroMemory(&ApplyUser, sizeof(ApplyUser));
//
//		//lstrcpyn(ApplyUser.szUserName, pUserData->szName, sizeof(ApplyUser.szUserName));
//
//		//ApplyUser.iUserScore=pUserData->lGameGold;
//		ApplyUser.dwUserID = pApplyBanker->wApplyUser;
//		m_GameClientView.m_ApplyUser.InserUser(ApplyUser);
//
//		char str[32];
//		_snprintf(str,32,"OnSubUserApplyBanker is %d",pApplyBanker->wApplyUser);
//		LOG_FILE(str);
//	}
//
//	//自己判断
//	if (IsLookonMode()==false && GetMeChairID()==pApplyBanker->wApplyUser) 
//		m_bMeApplyBanker=true;
//
//	//更新控件
//	//UpdateButtonContron();

	return true;
}

//取消做庄
bool CGameClientDlg::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
	////效验数据
	//ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
	//if (wDataSize!=sizeof(CMD_S_CancelBanker)) 
	//	return false;

	////消息处理
	//CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;
	//if( pCancelBanker == NULL )
	//	return FALSE;

	////删除玩家
	//tagApplyUser ApplyUser;
	//::ZeroMemory(&ApplyUser, sizeof(ApplyUser));
	//lstrcpyn(ApplyUser.szUserName, pCancelBanker->szCancelUser, lstrlen(pCancelBanker->szCancelUser));
	//ApplyUser.iUserScore=0;
	//ApplyUser.dwUserID = 0xFF;
	//for (WORD wUserIndex = 0; wUserIndex < MAX_CHAIR; ++wUserIndex)
	//{
	//	tagUserData const *pUserData = GetUserData(wUserIndex);

	//	if ( pUserData == NULL ) 
	//		continue;
	//	if(!lstrcmpi(pUserData->szName, pCancelBanker->szCancelUser))
	//	{
	//		ApplyUser.dwUserID = pUserData->dwGameID;
	//		break;
	//	}
	//}
	////if( ApplyUser.dwUserID != 0xFF)
	//ApplyUser.dwUserID=pCancelBanker->dwUserID;
	//	m_GameClientView.m_ApplyUser.DeleteUser(ApplyUser);

	//	char str[32];
	//	_snprintf(str,32,"OnSubUserCancelBanker is %d",pCancelBanker->dwUserID);
	//	LOG_FILE(str);

	////自己判断
	//const tagUserData *pMeUserData=GetUserData(GetMeChairID());
	//if( pMeUserData == NULL )
	//	return FALSE;

	////本人取消做庄
	//if (IsLookonMode()==false && !lstrcmp(pMeUserData->szName,pCancelBanker->szCancelUser)) 
	//	m_bMeApplyBanker=false;

	////更新控件
	/////UpdateButtonContron();

	return true;
}

//切换庄家
bool CGameClientDlg::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
	////效验数据
	//ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
	//if (wDataSize!=sizeof(CMD_S_ChangeBanker)) 
	//	return false;

	////消息处理
	//CMD_S_ChangeBanker * pChangeBanker=(CMD_S_ChangeBanker *)pBuffer;
	//if( pChangeBanker == NULL )
	//	return FALSE;

	////显示图片
	////m_GameClientView.ShowChangeBanker(m_wCurrentBanker!=pChangeBanker->wBankerUser);

	////自己判断
	//if (m_wCurrentBanker==GetMeChairID() && IsLookonMode() == false && pChangeBanker->wBankerUser!=GetMeChairID()) 
	//{
	//	m_bMeApplyBanker=false;
	//}
	//else if (IsLookonMode() == false && pChangeBanker->wBankerUser==GetMeChairID())
	//{
	//	m_bMeApplyBanker=true;
	//}

	////庄家信
	////SetBankerInfo(pChangeBanker->wBankerUser,pChangeBanker->iBankerScore);
	////m_GameClientView.SetBankerScore(0,0);

	////删除玩家
	////if (m_wCurrentBanker!=INVALID_CHAIR)
	////{
	//	//tagUserData const *pBankerUserData=GetUserData( pChangeBanker->wBankerUser);
	//	//if (pBankerUserData != NULL)
	//	{
	//		tagApplyUser ApplyUser;
	//		::ZeroMemory(&ApplyUser, sizeof(ApplyUser));
	//		//lstrcpyn(ApplyUser.szUserName, pBankerUserData->szName, lstrlen(pBankerUserData->szName));
	//		ApplyUser.dwUserID = pChangeBanker->wBankerUser;
	//		m_GameClientView.m_ApplyUser.DeleteUser(ApplyUser);

	//		char str[32];
	//		_snprintf(str,32,"OnSubChangeBanker is %d",pChangeBanker->wBankerUser);
	//		LOG_FILE(str);
	//	}
	////}

	////更新界面
	////UpdateButtonContron();
	////m_GameClientView.UpdateGameView(NULL);

	return true;
}

