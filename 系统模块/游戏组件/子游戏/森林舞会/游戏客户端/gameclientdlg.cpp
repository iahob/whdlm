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

//��ʱ����ʶ
#define IDI_FREE					99									//����ʱ��
#define IDI_PLACE_JETTON			100									//��עʱ��
#define IDI_DISPATCH_CARD			301									//����ʱ��

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameEngine)
	//ON_MESSAGE(IDM_APPLY_BANKER, OnApplyBanker)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientDlg::CGameClientDlg()
{
	//������Ϣ
	m_iMeMaxScore=0L;
	m_iApplyBankerCondition=0L;

	//������ע
	ZeroMemory(m_iMeAreaScoreArray, sizeof(m_iMeAreaScoreArray));

	//ׯ����Ϣ
	m_iBankerScore=0L;
	m_wCurrentBanker=0L;

	//״̬����
	m_bMeApplyBanker=false;
	g_GameClientDlg = this;

	return;

	
}

//��������
CGameClientDlg::~CGameClientDlg()
{
 
	//CLog::Instance()->UnInit();
	//CLog::DestroyInstance();
	 
}

//��ʼ����
bool CGameClientDlg::OnInitGameEngine()
{
	SetUnhandledExceptionFilter(CleanToolExceptionFun);
	//CLog::Instance()->Init("3dɭ�������ͻ�����־","3d Animal");
	//LOG_FILE("��־����");
	//���ñ���
	SetWindowText(TEXT("3Dɭ��������Ϸ"));

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);//������һ�䣬���ֳ�0����
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//��������
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

//ʱ��ɾ��
bool CGameClientDlg::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//���ÿ��
bool CGameClientDlg::OnResetGameEngine()
{
	//������Ϣ
	m_iMeMaxScore=0L;			

	//������ע
	ZeroMemory(m_iMeAreaScoreArray, sizeof(m_iMeAreaScoreArray));

	//ׯ����Ϣ
	m_iBankerScore=0L;
	m_wCurrentBanker=0L;

	//״̬����
	m_bMeApplyBanker=false;

	return true;
}

//��Ϸ����
void CGameClientDlg::OnGameOptionSet()
{
	return;
}

//ʱ����Ϣ
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

//�Թ�״̬
bool CGameClientDlg::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
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
		case SUB_S_GAME_FREE:		//��Ϸ����
			{
				return OnSubGameFree(pBuffer,wDataSize);
			}
		case SUB_S_GAME_START:		//��Ϸ��ʼ
			{
				return OnSubGameStart(pBuffer,wDataSize);
			}
		case SUB_S_GAME_END:		//��Ϸ����
			{
				return OnSubGameEnd(pBuffer,wDataSize);
			}
		case SUB_S_SEND_RECORD:		//��Ϸ��¼
			{
				return OnSubGameRecord(pBuffer,wDataSize);			
			}
		case SUB_S_PLACE_JETTON:	// �û���ע�ص�
			{
				return OnSubPlaceJetton(pBuffer, wDataSize);
			}
		case SUB_S_PLACE_JETTON_FAIL:	//��עʧ��
			{
				return OnSubPlaceJettonFail(pBuffer,wDataSize);
			}
		case SUB_S_CLEAR_JETTON:		// ��ע���
			{
				return OnSubJettonClear(pBuffer, wDataSize);
			}
		case SUB_S_APPLY_BANKER:	//������ׯ
			{
				//AfxMessageBox("sdfsdfsdf");
				//return OnSubUserApplyBanker(pBuffer, wDataSize);
			}
		case SUB_S_CANCEL_BANKER:	//ȡ����ׯ
			{
				//AfxMessageBox("1111111111111111");
				//return OnSubUserCancelBanker(pBuffer, wDataSize);
			}
		case SUB_S_CHANGE_BANKER:	//�л�ׯ��
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

	//�������
	//ASSERT(FALSE);

	return true;
}

//��Ϸ����
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
	case GAME_STATUS_FREE:			//����״̬
		{
			//Ч������
        	ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) 
				return false;

			//��Ϣ����
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;
			if( pStatusFree == NULL )
				return FALSE;

			//����ʱ��
			SetGameClock(GetMeChairID(),IDI_FREE,pStatusFree->cbTimeLeave);

			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

			if(RenderCanvas::Instance()->IsValid())
				RenderCanvas::Instance()->OnGSFree(pBuffer, wDataSize);

            //�����Ϣ
// 			m_iMeMaxScore=pStatusFree->iUserMaxScore;
// 
// 			m_GameClientView.SetMeMaxScore(m_iMeMaxScore);
// 			WORD wMeChairID=GetMeChairID();
// 		
// 			WORD wSwitchViewChairID=SwitchViewChairID(wMeChairID);
// 			m_GameClientView.SetMeChairID(wSwitchViewChairID);
// 			
//             //ׯ����Ϣ
// 			SetBankerInfo(pStatusFree->wBankerUser,pStatusFree->iBankerScore);
// 			m_GameClientView.SetBankerScore(pStatusFree->cbBankerTime,pStatusFree->iBankerWinScore);
// 			m_bEnableSysBanker=pStatusFree->bEnableSysBanker;
// 			m_GameClientView.EnableSysBanker(m_bEnableSysBanker);
// 
//             //������Ϣ
// 			m_iApplyBankerCondition=pStatusFree->iApplyBankerCondition;
// 			m_GameClientView.SetAreaLimitScore(pStatusFree->iAreaLimitScore, JETTON_AREA_COUNT);
// 
// 			//����״̬
// 			SetGameStatus(GS_FREE);
			// 
			// 			//���¿���
			// 			UpdateButtonContron();
			// 			m_GameClientView.UpdateGameView(NULL);

			return true;
		}break;
	case GS_PLACE_JETTON:	//��Ϸ״̬
	case GS_GAME_END:		//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) 
				return false;

			//��Ϣ����
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

			////ȫ����ע
			//	for( int i=0; i<JETTON_AREA_COUNT; i++)
			//		m_GameClientView.PlaceUserJetton(i, pStatusPlay->iTotalAreaScore[i]);

			//	//�����ע
			//	for( int i=0; i<JETTON_AREA_COUNT; i++ )
			//		SetMePlaceJetton(i, pStatusPlay->iUserAreaScore[i]);

			//	//��һ���
			//	m_iMeMaxScore=pStatusPlay->iUserMaxScore;			
			//	m_GameClientView.SetMeMaxScore(m_iMeMaxScore);
			//	WORD wMeChairID=GetMeChairID();
			//	m_GameClientView.SetMeChairID(SwitchViewChairID(wMeChairID));

			//	//������Ϣ
			//	m_iApplyBankerCondition=pStatusPlay->iApplyBankerCondition;
			//	m_GameClientView.SetAreaLimitScore(pStatusPlay->iAreaScoreLimit, JETTON_AREA_COUNT);

			//	if (pStatusPlay->cbGameStatus==GS_GAME_END)
			//	{
			//		//���óɼ�
			//		m_GameClientView.SetCurGameScore(pStatusPlay->iEndUserScore,pStatusPlay->iEndUserReturnScore,pStatusPlay->iEndBankerScore,pStatusPlay->iEndRevenue);
			//	}
			//	else
			//	{
			//		//��������
			//		if (IsEnableSound()) m_DTSDBackground.Play(0,true);
			//	}

			//	//ׯ����Ϣ
			//	SetBankerInfo(pStatusPlay->wBankerUser,pStatusPlay->iBankerScore);
			//	m_GameClientView.SetBankerScore(pStatusPlay->cbBankerTime,pStatusPlay->iBankerWinScore);
			//	m_bEnableSysBanker=pStatusPlay->bEnableSysBanker;
			//	m_GameClientView.EnableSysBanker(m_bEnableSysBanker);

			//	//����״̬
			//	SetGameStatus(pStatusPlay->cbGameStatus);

			//	//����ʱ��
			//	SetGameTimer(GetMeChairID(),pStatusPlay->cbGameStatus==GS_GAME_END?IDI_DISPATCH_CARD:IDI_PLACE_JETTON,pStatusPlay->cbTimeLeave);

			//���°�ť
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

//��Ϸ��ʼ
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//_RPT0(_CRT_WARN, "OnSubGameStart\n");
	//Ч������
	int pp=sizeof(CMD_S_GameStart);
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) 
		return false;

	//��Ϣ����
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

//��Ϸ����
bool CGameClientDlg::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//_RPT0(_CRT_WARN, "OnSubGameFree\n");

	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameFree));
	if (wDataSize!=sizeof(CMD_S_GameFree)) 
		return false;

	//��Ϣ����
	CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;
	if( pGameFree == NULL )
	{
		_ASSERT(false);
		return false;
	}
	
	if(RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnSubGameFree(pBuffer, wDataSize);

	//����ʱ��
	//SetGameTimer(GetMeChairID(),IDI_FREE,pGameFree->cbTimeLeave);


	//UpdateButtonContron();

	return true;
}
//��Ϸ����
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	_RPT0(_CRT_WARN, "OnSubGameEnd\n");
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) 
		return false;

	//��Ϣ����
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
// ��Ϸ��¼
bool CGameClientDlg::OnSubGameRecord(const void * pBuffer, WORD wDataSize)
{
	_RPT0(_CRT_WARN, "OnSubGameRecord\n");
	//Ч������
	ASSERT(wDataSize % sizeof(tagServerGameRecord) == 0);
	if (wDataSize % sizeof(tagServerGameRecord) != 0) 
		return false;

	//��Ϣ����
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

//�û���ע
bool CGameClientDlg::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)
{
	_RPT0(_CRT_WARN, "OnSubPlaceJetton\n");
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJetton));
	if (wDataSize!=sizeof(CMD_S_PlaceJetton)) 
		return false;

	//��Ϣ����
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
//��ע���
bool CGameClientDlg::OnSubJettonClear(const void * pBuffer, WORD wDataSize)
{
	_RPT0(_CRT_WARN, "OnSubJettonClear\n");
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ClearJetton));
	if (wDataSize!=sizeof(CMD_S_ClearJetton)) 
		return false;

	//��Ϣ����
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
//��עʧ��
bool CGameClientDlg::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
	_RPT0(_CRT_WARN, "OnSubPlaceJettonFail\n");
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJettonFail));
	if (wDataSize!=sizeof(CMD_S_PlaceJettonFail)) 
		return false;

	//��Ϣ����
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



//��ע��Ϣ
void CGameClientDlg::OnPlaceJetton(BYTE iJettonArea, __int64 iJettonNum)
{
	//��������
	BYTE cbJettonArea=iJettonArea;			//��ע����
	__int64 iJettonScore=iJettonNum;		//��������

	if( cbJettonArea > ID_SMALL_SNAKE || cbJettonArea < ID_BIG_TIGER )
		return;

	//ׯ�Ҳ�����ע
	if ( GetMeChairID() == m_wCurrentBanker )
		return;

	//����ע״̬��ֱ�ӷ���
	if (GetGameStatus()!=GS_PLACE_JETTON)
	{
		//UpdateButtonContron();
		return;
	}

	//������ע�������ñ�����ע����
	m_iMeAreaScoreArray[cbJettonArea] += iJettonScore;
	//m_GameClientView.SetMePlaceJetton(cbJettonArea, m_iMeAreaScoreArray[cbJettonArea]);

	//��������
	CMD_C_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

	//�������
// 	PlaceJetton.cbJettonArea=cbJettonArea;
// 	PlaceJetton.iJettonScore=iJettonScore;
// 
// 	//����������Ϣ
// 	SendData(SUB_C_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

	//���°�ť
	//UpdateButtonContron();

	//��������
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

	// ��ֹ���Ŵ���
	//ModifyStyle(WS_MAXIMIZE, 0);
	//ModifyStyle(WS_MAXIMIZEBOX , 0);
	//ModifyStyle(WS_SIZEBOX , 0);

	return 0;
}

void CGameClientDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CGameFrameEngine::OnKeyDown(nChar, nRepCnt, nFlags);
}

//������ׯ
bool CGameClientDlg::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
//	//Ч������
//	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
//	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) 
//		return false;
//
//	//��Ϣ����
//	CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;
//	if( pApplyBanker == NULL )
//		return FALSE;
//
//	//��ȡ���
//	//tagUserData const *pUserData=GetUserData(pApplyBanker->wApplyUser);
//	//if( pUserData == NULL )
//	//	return FALSE;
//
//	//�������
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
//	//�Լ��ж�
//	if (IsLookonMode()==false && GetMeChairID()==pApplyBanker->wApplyUser) 
//		m_bMeApplyBanker=true;
//
//	//���¿ؼ�
//	//UpdateButtonContron();

	return true;
}

//ȡ����ׯ
bool CGameClientDlg::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
	////Ч������
	//ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
	//if (wDataSize!=sizeof(CMD_S_CancelBanker)) 
	//	return false;

	////��Ϣ����
	//CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;
	//if( pCancelBanker == NULL )
	//	return FALSE;

	////ɾ�����
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

	////�Լ��ж�
	//const tagUserData *pMeUserData=GetUserData(GetMeChairID());
	//if( pMeUserData == NULL )
	//	return FALSE;

	////����ȡ����ׯ
	//if (IsLookonMode()==false && !lstrcmp(pMeUserData->szName,pCancelBanker->szCancelUser)) 
	//	m_bMeApplyBanker=false;

	////���¿ؼ�
	/////UpdateButtonContron();

	return true;
}

//�л�ׯ��
bool CGameClientDlg::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
	////Ч������
	//ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
	//if (wDataSize!=sizeof(CMD_S_ChangeBanker)) 
	//	return false;

	////��Ϣ����
	//CMD_S_ChangeBanker * pChangeBanker=(CMD_S_ChangeBanker *)pBuffer;
	//if( pChangeBanker == NULL )
	//	return FALSE;

	////��ʾͼƬ
	////m_GameClientView.ShowChangeBanker(m_wCurrentBanker!=pChangeBanker->wBankerUser);

	////�Լ��ж�
	//if (m_wCurrentBanker==GetMeChairID() && IsLookonMode() == false && pChangeBanker->wBankerUser!=GetMeChairID()) 
	//{
	//	m_bMeApplyBanker=false;
	//}
	//else if (IsLookonMode() == false && pChangeBanker->wBankerUser==GetMeChairID())
	//{
	//	m_bMeApplyBanker=true;
	//}

	////ׯ����
	////SetBankerInfo(pChangeBanker->wBankerUser,pChangeBanker->iBankerScore);
	////m_GameClientView.SetBankerScore(0,0);

	////ɾ�����
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

	////���½���
	////UpdateButtonContron();
	////m_GameClientView.UpdateGameView(NULL);

	return true;
}

