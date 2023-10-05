#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"

////////////////////////////////////////////////////////////////////////////////// 

//��Ϸʱ��
#define IDI_START_GAME				200									//���ƶ�ʱ��
#define IDI_USER_OPERATE			201									//��ʼ��ʱ��
#define IDI_GAME_SHUO_MING			202									//��ʼ��ʱ��
#define IDI_AUTO_GAME				203									//��ʼ��ʱ��
#define IDI_ENTER					204									//��ʼ��ʱ��

////////////////////////////////////////////////////////////////////////////////// 

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_GAME_CLOSE, OnMessageGameClose)
	ON_MESSAGE(IDM_GAME_SOUND,OnMessageGameSound)
	ON_MESSAGE(IDM_GAME_OPTION,OnMessageGameOption)
	ON_MESSAGE(IDM_GAME_OPERATE,OnMessageGameOperate)
	ON_MESSAGE(IDM_GAME_BET,OnMessageGameBet)
	ON_MESSAGE(IDM_QUICKEN_FINISH, OnUserActionFinish)
	ON_MESSAGE(IDM_QUICKEN_FINISH_STEPS, OnUserActionFinishSteps)
	ON_MESSAGE(IDM_USER_ACTION_FINISH, OnUserActionFinish)
	ON_WM_TIMER()

	
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////// 

// ���캯��
CGameClientEngine::CGameClientEngine() : m_GameClientView( m_GameFrameViewDexter.m_GameClientView )
{
	m_wClockID = 0;
	m_wClockCount = 0;

	m_bShuoMing = true;
	m_bAutoStart = false;
	m_lSaveScore = 0;
	m_cbGameMode = GM_NULL;
	m_lCellScore = 1;
	m_lOneGameScore = 0;
	m_lTwoGameScore = 0;
	m_lThreeGameScore = 0;
	m_lBet = 0;
	m_cbBetCount = 1;
	ZeroMemory(m_lBetScore, sizeof(m_lBetScore));
	m_lBetScore[0] = 1;
	m_lLine = 9;
	for (int i = 0; i < ITEM_Y_COUNT; i++)
	{
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			m_cbItemInfo[i][j] = CT_SHUIHUZHUAN;
		}
	}
	for (int i = 0; i < ITEM_COUNT; i++)
	{
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			m_ptZhongJiang[i][j].SetPoint(0xFF, 0xFF);
		}
	}
	m_bWaitOpenOne = true;

	//ZeroMemory(m_cbThreeItemInfo, sizeof(m_cbThreeItemInfo));
	m_bWaitOpenTwo = false;
	m_bWaitBetTwo = false;
	m_bOpenTwo = false;
	m_cbOpenTwoMode = 0;

	m_bWaitOpenThree = false;
	m_bWaitBetThree = false;
	m_bOpenThree = false;

	//������
	m_UserActionArray.RemoveAll();
}

// ��������
CGameClientEngine::~CGameClientEngine()
{
}

// ��������
bool CGameClientEngine::OnInitGameEngine()
{
	// ��������
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	// ���ñ�ʶ1
	m_bLegalOne = (lstrcmp( m_ClientCompilationSink->GetCompilation(), szCompilation ) == 0);

	// ��ʼ������
	g_GameOption.InitParameter( AfxGetAppName() );

	// ��������
	CSound::_Object()->CreateD3DSound( GetSafeHwnd() );
	CMusic::_Object()->CreateD3DSound( GetSafeHwnd() );

	// ���ñ�ʶ2
	m_bLagalTwo = m_bLegalOne;

	m_GameClientView.SetGameClientEngine(this);

	SetTimer(IDI_GAME_SHUO_MING, 250, NULL);

	return true;
}

// ���ú���
bool CGameClientEngine::OnResetGameEngine()
{
	CSound::_Object()->StopSound();
	CMusic::_Object()->StopMusic();

	return true;
}

// ʱ��ɾ��
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

// ʱ����Ϣ
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	m_wClockCount = nElapse;
	switch (wClockID)
	{
	case IDI_AUTO_GAME:
		{
			if ((wChairID==GetMeChairID())&&(!IsLookonMode()) && m_bAutoStart)
			{
				if(nElapse<=0)
				{
					OnMessageGameOperate(VK_SPACE, 1);
					KillGameClockEx(IDI_AUTO_GAME);
				}
				else
				{
					CString str;
					str.Format(TEXT("���ո񡿼� ��ʼ��%d����Զ���ʼ��"), nElapse);
					m_GameClientView.SetTip(str);
				}
			}

			if(nElapse<=0 && GetGameStatus() == GAME_SCENE_THREE)
			{
				OnMessageGameOperate(VK_SPACE, 1);
				KillGameClockEx(IDI_AUTO_GAME);
			}
			return true;
		}
	case IDI_USER_OPERATE:
		{
			if ((wChairID==GetMeChairID())&&(!IsLookonMode()) && GetGameStatus() == GAME_SCENE_TWO)
			{
				if(nElapse<=0)
				{
					m_cbOpenTwoMode = 0xFF;
					//������Ϣ
					SendSocketData(SUB_C_TWO_GIVEUP);
					PlaySoundEx(TEXT("coins_fly_in.wav"));
					m_GameClientView.ExitTwo();
					EnterOne();
					if(m_bAutoStart)
					{
						OnMessageGameOperate(VK_SPACE, 1);
					}
				}
			}
			return true;
		}
	}
	return false;
}

// ��������
bool CGameClientEngine::AllowBackGroundSound(bool bAllowSound)
{
	CGlobalUnits *pGlobalUnits=CGlobalUnits::GetInstance();
	if (bAllowSound&& !pGlobalUnits->m_bMuteStatuts)
	{
		//PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));	
	}
	else if(!bAllowSound|| pGlobalUnits->m_bMuteStatuts)
	{
		StopSound();
		CSound::_Object()->StopSound();
	}

	return true;
}

// �Թ���Ϣ
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_TWO_START:	//��Ϸ����
		{
			return OnSubTwoStart(pData,wDataSize);
		}
	case SUB_S_THREE_START:	//��Ϸ����
		{
			return OnSubThreeStart(pData,wDataSize);
		}
	case SUB_S_THREE_KAI_JIAN:	//��Ϸ����
		{
			return OnSubThreeKaiJian(pData,wDataSize);
		}
	case SUB_S_THREE_END:		//��Ϸ����
		{
			return OnSubThreeEnd(pData,wDataSize);
		}
	case SUB_S_GAME_CONCLUDE:	//��Ϸ����
		{
			return OnSubGameConclude(pData,wDataSize);
		}	
	case SUB_S_ADMIN_STORAGE_INFO:	
		{
			ASSERT(wDataSize == sizeof(CMD_S_ADMIN_STORAGE_INFO));
			if (wDataSize != sizeof(CMD_S_ADMIN_STORAGE_INFO))
			{
				return false;
			}

			//�ж�Ȩ��
			if (CUserRight::IsGameDebugUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				|| NULL == m_GameClientView.m_pClientControlDlg)
			{
				return true;
			}

			CMD_S_ADMIN_STORAGE_INFO *pStorage = (CMD_S_ADMIN_STORAGE_INFO *)pData;
			m_GameClientView.m_pClientControlDlg->SetStorageInfo(pStorage);

			return true;
		}
	case SUB_S_REQUEST_QUERY_RESULT:
		{
			ASSERT(wDataSize == sizeof(CMD_S_RequestQueryResult));
			if (wDataSize != sizeof(CMD_S_RequestQueryResult))
			{
				return false;
			}

			//�ж�Ȩ��
			if (CUserRight::IsGameDebugUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				|| NULL == m_GameClientView.m_pClientControlDlg)
			{
				return true;
			}

			CMD_S_RequestQueryResult *pQueryResult = (CMD_S_RequestQueryResult *)pData;
			m_GameClientView.m_pClientControlDlg->RequestQueryResult(pQueryResult);

			return true;
		}
	case SUB_S_USER_CONTROL:
		{
			ASSERT(wDataSize == sizeof(CMD_S_UserControl));
			if (wDataSize != sizeof(CMD_S_UserControl))
			{
				return false;
			}

			//�ж�Ȩ��
			if (CUserRight::IsGameDebugUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				|| NULL == m_GameClientView.m_pClientControlDlg)
			{
				return true;
			}

			CMD_S_UserControl *pUserControl = (CMD_S_UserControl *)pData;
			m_GameClientView.m_pClientControlDlg->RoomUserControlResult(pUserControl);

			return true;
		}
	case SUB_S_OPERATION_RECORD:
		{
			ASSERT(wDataSize == sizeof(CMD_S_Operation_Record));
			if (wDataSize != sizeof(CMD_S_Operation_Record))
			{
				return false;
			}

			//�ж�Ȩ��
			if (CUserRight::IsGameDebugUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				|| NULL == m_GameClientView.m_pClientControlDlg)
			{
				return true;
			}

			CMD_S_Operation_Record *pOperation_Record = (CMD_S_Operation_Record *)pData;
			m_GameClientView.m_pClientControlDlg->UpdateOperationRecord(pOperation_Record);

			return true;
		}
	case SUB_S_UPDATE_ROOM_STORAGE:
		{
			ASSERT(wDataSize == sizeof(CMD_S_UpdateRoomStorage));
			if (wDataSize != sizeof(CMD_S_UpdateRoomStorage))
			{
				return false;
			}

			//�ж�Ȩ��
			if (CUserRight::IsGameDebugUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				|| NULL == m_GameClientView.m_pClientControlDlg)
			{
				return true;
			}

			CMD_S_UpdateRoomStorage *pUpdateRoomStorage = (CMD_S_UpdateRoomStorage *)pData;
			m_GameClientView.m_pClientControlDlg->SetRoomStorage(pUpdateRoomStorage->lRoomStorageStart, pUpdateRoomStorage->lRoomStorageCurrent, pUpdateRoomStorage->lRoomStorageInput, pUpdateRoomStorage->lRoomStockRecoverScore);

			return true;
		}
	case SUB_S_UPDATE_ROOMUSERLIST:
		{
			ASSERT(wDataSize == sizeof(CMD_S_UpdateRoomUserList));
			if (wDataSize != sizeof(CMD_S_UpdateRoomUserList))
			{
				return false;
			}

			//�ж�Ȩ��
			if (CUserRight::IsGameDebugUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				|| NULL == m_GameClientView.m_pClientControlDlg)
			{
				return true;
			}

			CMD_S_UpdateRoomUserList *pUpdateRoomUserList = (CMD_S_UpdateRoomUserList *)pData;
			m_GameClientView.m_pClientControlDlg->UpdateRoomUserListCtrl(pUpdateRoomUserList);

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}


// ������Ϣ
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	// �ж���Ϣ
	if( !m_bLagalTwo )
	{
		AfxMessageBox(TEXT("�Ƿ������"));
	}

	// ������Ϣ
	if( !m_bLagalTwo || !m_bLegalOne )
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);

		return false;
	}

	m_GameClientView.ShowClientControlDlg(CUserRight::IsGameDebugUser(m_pIClientKernel->GetUserAttribute()->dwUserRight));
	m_GameClientView.SetClientUserItem(GetTableUserItem(GetMeChairID()));
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��������
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			m_lCellScore = pStatusFree->lCellScore;
			m_cbBetCount = pStatusFree->cbBetCount;
			CopyMemory(m_lBetScore, pStatusFree->lBetScore, sizeof(m_lBetScore));

			m_GameClientView.SetOneBet(m_lBetScore[m_lBet], m_lBetScore[m_lBet] * m_lCellScore * m_lLine);

			m_GameClientView.SetOneScore(0, 0);

			return true;
		}
	case GAME_SCENE_TWO:	//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusTwo));
			if (wDataSize!=sizeof(CMD_S_StatusTwo)) return false;

			//��������
			CMD_S_StatusTwo * pStatusTwo=(CMD_S_StatusTwo *)pData;

			m_lCellScore = pStatusTwo->lCellScore;
			m_cbBetCount = pStatusTwo->cbBetCount;
			CopyMemory(m_lBetScore, pStatusTwo->lBetScore, sizeof(m_lBetScore));
			m_lBet = pStatusTwo->lBet;
			m_cbOpenTwoMode = pStatusTwo->cbTwoMode;
			m_lOneGameScore = pStatusTwo->lOneGameScore;
			m_lTwoGameScore = pStatusTwo->lTwoGameScore;
			CopyMemory(m_cbItemInfo, pStatusTwo->cbItemInfo, sizeof(m_cbItemInfo));
			m_GameClientView.SetItemInfo(m_cbItemInfo);
			m_GameClientView.SetOneBet(m_lBetScore[m_lBet], m_lBetScore[m_lBet] * m_lCellScore * m_lLine);
			m_GameClientView.SetOneScore(m_lOneGameScore);
			m_GameClientView.SetTwoScore(__max(0, m_lTwoGameScore));


			//��û����
			if(pStatusTwo->cbTwoMode == 0xFF)
			{
				m_cbGameMode = GM_TWO;
				m_bWaitOpenOne = false;
				m_GameClientView.StartKaiJangEnd(m_lOneGameScore, false);

				m_lTwoGameScore = m_lOneGameScore;
				m_GameClientView.SetWaitMode(GAME_SCENE_TWO);
				m_GameClientView.SetTip(TEXT("���ո񡿼� ȡ�� ��������� ������ȫ�� ����������"));				
			}
			else
			{
				m_cbGameMode = GM_TWO_WAIT;
				m_bOpenTwo = false;
				m_bWaitOpenTwo = true;
				m_bWaitBetTwo = true;

				//����ȱ�
				EnterTwo();

				SCORE lBet = ((m_cbOpenTwoMode == 0) ? __max(1, m_lOneGameScore / 2) : (m_cbOpenTwoMode == 1 ? m_lOneGameScore : m_lOneGameScore * 2));
				m_lTwoGameScore -= lBet;
				m_GameClientView.SetTwoScore(__max(0, m_lTwoGameScore), lBet, m_lBetScore[m_lBet] * m_lCellScore * m_lLine);

			}

			return true;
		}


	}

	//�������
	ASSERT(FALSE);

	return false;
}


// �û�����
VOID CGameClientEngine::OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)
{

}

// �û��뿪
VOID CGameClientEngine::OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)
{

}

//��Ϸ��ʼ
bool CGameClientEngine::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	KillGameClockEx(IDI_AUTO_GAME);
	StopSound();
	CSound::_Object()->StopSound();

	//��������
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	m_bWaitOpenOne = true;
	m_bWaitOpenTwo = false;
	m_bOpenTwo = false;
	m_bWaitBetTwo = false;

	m_bWaitOpenThree = false;
	m_bOpenThree = false;
	m_bWaitBetThree = false;

	m_lOneGameScore = pGameStart->lScore;
	m_lTwoGameScore = 0;
	m_lThreeGameScore = 0;
	m_cbGameMode = GM_ONE;
	CopyMemory(m_cbItemInfo, pGameStart->cbItemInfo, sizeof(m_cbItemInfo));

	m_GameLogic.GetZhongJiangInfo(m_cbItemInfo, m_ptZhongJiang);

	SetGameStatus(GAME_SCENE_ONE);
	m_GameClientView.SetGameStatus(GAME_SCENE_ONE);

	m_GameClientView.SetOneScore(0, m_lCellScore * m_lBetScore[m_lBet] * m_lLine);

	m_GameClientView.SetTip(TEXT("���ո񡿼� ȫͣ"));

	m_UserActionArray.RemoveAll();

	//��������
	tagActionOneGunDong *pActionOneGunDong = (tagActionOneGunDong *)ActiveUserAction(AK_One_GunDong);
	ASSERT(pActionOneGunDong);
	if( !pActionOneGunDong ) return false;
	pActionOneGunDong->nCurIndex = 0;
	pActionOneGunDong->nMaxIndex = 15;
	pActionOneGunDong->lScore = pGameStart->lScore;

	m_UserActionArray.Add(pActionOneGunDong);

	if (m_lOneGameScore > 0)
	{		
		LONGLONG lGameScore = 0;
		for (int i = 0; i < ITEM_COUNT; i++)
		{
			if (m_GameLogic.GetZhongJiangInfo(i, m_cbItemInfo))
			{
				tagActionOneYaXian *pActionOneYaXian = (tagActionOneYaXian *)ActiveUserAction(AK_One_YaXian);
				ASSERT(pActionOneYaXian);
				if( !pActionOneYaXian ) return false;
				pActionOneYaXian->nZhongJiangXian = i;
				pActionOneYaXian->lXianScore = m_GameLogic.GetZhongJiangTime(i, m_cbItemInfo) * m_lCellScore * m_lBetScore[m_lBet];
				lGameScore += pActionOneYaXian->lXianScore;
				pActionOneYaXian->lScore = lGameScore;
				CopyMemory(pActionOneYaXian->ptXian, m_ptZhongJiang[pActionOneYaXian->nZhongJiangXian], sizeof(pActionOneYaXian->ptXian));

				m_UserActionArray.Add(pActionOneYaXian);
			}
		}

		tagActionOneKaiJian *pActionOneKaiJian = (tagActionOneKaiJian *)ActiveUserAction(AK_One_KaiJian);
		ASSERT(pActionOneKaiJian);
		if( !pActionOneKaiJian ) return false;
		pActionOneKaiJian->nCurIndex = 0;
		pActionOneKaiJian->nMaxIndex = 9;
		pActionOneKaiJian->cbGameMode = pGameStart->cbGameMode;
		pActionOneKaiJian->lQuanPanScore = pGameStart->lScore/* - lGameScore*/;
		pActionOneKaiJian->lScore = pGameStart->lScore;

 		if(m_GameLogic.GetQuanPanJiangTime(m_cbItemInfo))
 		{
 			for (int i = 0; i < ITEM_Y_COUNT; i++)
 			{
 				for (int j = 0; j < ITEM_X_COUNT; j++)
 				{
 					pActionOneKaiJian->bZhongJiang[i][j] = true;
 				}
 			}
 		}
 		else
		{	
			for (int i = 0; i < ITEM_COUNT; i++)
			{
				for (int j = 0; j < ITEM_X_COUNT; j++)
				{
					if(m_ptZhongJiang[i][j].x != 0xff)
					{
						pActionOneKaiJian->bZhongJiang[m_ptZhongJiang[i][j].x][m_ptZhongJiang[i][j].y] = true;
					}
				}
			}
		}

		m_UserActionArray.Add(pActionOneKaiJian);

		if (pGameStart->cbGameMode == GM_THREE)
		{
			m_GameClientView.EnterScene(3);
		}
		else 
		{
			m_GameClientView.EnterScene(2);
		}
	}
	//��������
	BeginUserAction();

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubTwoStart(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameTwoStart));
	if (wDataSize!=sizeof(CMD_S_GameTwoStart)) return false;

	//��������
	CMD_S_GameTwoStart * pTwoStart=(CMD_S_GameTwoStart *)pData;

	//׼��״̬
	tagActionTwoKaiJian *pActionTwoKaiJian = (tagActionTwoKaiJian *)ActiveUserAction(AK_Two_KaiJian);
	ASSERT(pActionTwoKaiJian);
	if( !pActionTwoKaiJian ) return false;
	pActionTwoKaiJian->nCurIndex = 0;
	pActionTwoKaiJian->nMaxIndex = 2;
	pActionTwoKaiJian->cbUser[0] = 0;		//�Ȳ�
	pActionTwoKaiJian->cbUser[1] = 0;		//�ɹٴ�����
	pActionTwoKaiJian->cbUser[2] = 0;		//�Ȳ�
	pActionTwoKaiJian->lScore = pTwoStart->lScore;
	CopyMemory(pActionTwoKaiJian->cbSice, pTwoStart->cbOpenSize, sizeof(pActionTwoKaiJian->cbSice));
	m_UserActionArray.Add(pActionTwoKaiJian);

	BYTE cbUser = 1;
	if (pTwoStart->lScore  > 0)
	{
		cbUser = (rand() % 2 == 0 ? 2 : 3);
	}

	tagActionTwoWaitBet *pActionTwoWaitBet = (tagActionTwoWaitBet *)ActiveUserAction(AK_Two_WaitBet);
	ASSERT(pActionTwoWaitBet);
	if( !pActionTwoWaitBet ) return false;
	pActionTwoWaitBet->bOne = true;
	pActionTwoWaitBet->cbExp = 2;			//����
	pActionTwoWaitBet->lScore = pTwoStart->lScore;
	pActionTwoWaitBet->nCurIndex = 0;
	pActionTwoWaitBet->nMaxIndex = (pTwoStart->lScore  > 0 ? 2 : 1);
	pActionTwoWaitBet->cbUser[0] = pTwoStart->lScore > 0 ? 1 : 3;		//1���� 3����
	pActionTwoWaitBet->cbUser[1] = cbUser;								//2 3�ɹ����˷�ŭ���� 1�ɹٿ��ı���
	pActionTwoWaitBet->cbUser[2] = pTwoStart->lScore > 0 ? 1 : 3;		//1���� 3����
	m_UserActionArray.Add(pActionTwoWaitBet);

	//��������
	BeginUserAction();

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubThreeStart(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameThreeStart));
	if (wDataSize!=sizeof(CMD_S_GameThreeStart)) return false;

	//��������
	CMD_S_GameThreeStart * pGameThreeStart=(CMD_S_GameThreeStart *)pData;

	//CopyMemory(m_cbThreeItemInfo, pGameThreeStart->cbItem, sizeof(m_cbThreeItemInfo));

	//׼��״̬
	tagActionThreeGunDong *pActionThreeGunDong = (tagActionThreeGunDong *)ActiveUserAction(AK_Three_GunDong);
	ASSERT(pActionThreeGunDong);
	if( !pActionThreeGunDong ) return false;
	pActionThreeGunDong->nCurIndex = 0;
	pActionThreeGunDong->nMaxIndex = 4;
	pActionThreeGunDong->lScore = pGameThreeStart->lScore;
	pActionThreeGunDong->cbBounsGameCount = pGameThreeStart->cbBounsGameCount;
	CopyMemory(pActionThreeGunDong->cbItem, pGameThreeStart->cbItem, sizeof(pActionThreeGunDong->cbItem));
	m_UserActionArray.Add(pActionThreeGunDong);

	//��������
	BeginUserAction();

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubThreeKaiJian(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameThreeKaiJiang));
	if (wDataSize!=sizeof(CMD_S_GameThreeKaiJiang)) return false;

	//��������
	CMD_S_GameThreeKaiJiang * pGameThreeKaiJiang=(CMD_S_GameThreeKaiJiang *)pData;

	//׼��״̬
	tagActionThreeKaiJian *pActionThreeKaiJian = (tagActionThreeKaiJian *)ActiveUserAction(AK_Three_KaiJian);
	ASSERT(pActionThreeKaiJian);
	if( !pActionThreeKaiJian ) return false;
	pActionThreeKaiJian->nCurIndex = 0;
	pActionThreeKaiJian->nMaxIndex = 1;
	pActionThreeKaiJian->cbIndex = pGameThreeKaiJiang->cbIndex;
	pActionThreeKaiJian->nTime = pGameThreeKaiJiang->nTime;
	pActionThreeKaiJian->lScore = pGameThreeKaiJiang->lScore;
	m_UserActionArray.Add(pActionThreeKaiJian);

	//׼��״̬
	tagActionThreeKaiJianEnd *pActionThreeKaiJianEnd = (tagActionThreeKaiJianEnd *)ActiveUserAction(AK_Three_KaiJianEnd);
	ASSERT(pActionThreeKaiJianEnd);
	if( !pActionThreeKaiJianEnd ) return false;
	pActionThreeKaiJianEnd->nCurIndex = 0;
	pActionThreeKaiJianEnd->nMaxIndex = 1;
	pActionThreeKaiJianEnd->cbIndex = pGameThreeKaiJiang->cbIndex;
	pActionThreeKaiJianEnd->lScore = pGameThreeKaiJiang->lScore;
	m_UserActionArray.Add(pActionThreeKaiJianEnd);

	//��������
	BeginUserAction();

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubThreeEnd(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==0);
	if (wDataSize!=0) return false;


	//׼��״̬
	tagActionThreeEnd *pActionThreeEnd = (tagActionThreeEnd *)ActiveUserAction(AK_Three_End);
	ASSERT(pActionThreeEnd);
	if( !pActionThreeEnd ) return false;
	m_UserActionArray.Add(pActionThreeEnd);

	//��������
	BeginUserAction();

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubGameConclude(VOID * pData, WORD wDataSize)
{
	//׼��״̬
	tagActionGameConclude *pActionGameConclude = (tagActionGameConclude *)ActiveUserAction(AK_GameConclude);
	ASSERT(pActionGameConclude);
	if( !pActionGameConclude ) return false;
	pActionGameConclude->nCurIndex = 0;
	pActionGameConclude->nMaxIndex = 1;

	m_UserActionArray.Add(pActionGameConclude);

	//��������
	if(m_UserActionArray.GetCount()==1)	BeginUserAction();

	return true;
}

//ɾ��ʱ��
VOID CGameClientEngine::KillGameClockEx(WORD wClockID)
{
	m_wClockID = INVALID_CHAIR;
	m_wClockCount = 0;
	KillGameClock(wClockID);
}

//����ʱ��
VOID CGameClientEngine::SetGameClockEx(WORD wChairID, WORD wClockID, UINT nElapse)
{
	m_wClockID = wClockID;
	m_wClockCount = nElapse;
	SetGameClock(wChairID, wClockID, nElapse);
}

//ʱ����Ϣ
VOID CGameClientEngine::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	/*case IDI_USER_OPERATE:
		{
			if (GetGameStatus() == GAME_SCENE_TWO)
			{
				m_cbOpenTwoMode = 0xFF;
				//������Ϣ
				SendSocketData(SUB_C_TWO_GIVEUP);
				m_GameClientView.ExitTwo();
				EnterOne();
			}
			KillTimer(IDI_USER_OPERATE);
			break;
		}*/
	case IDI_ENTER:
		{
			KillTimer(IDI_ENTER);
			OnUserActionFinish(0, 0);			
		}
	case IDI_GAME_SHUO_MING:
		{
			static int n = 0;
			BYTE cbPos = m_GameClientView.GetShuoMingPos();
			if (cbPos < 100)
			{
				m_GameClientView.SetShuoMingPos(++cbPos);
			}
			if(++n >= 20)
			{
				n = 0;
				BYTE cbIndex = m_GameClientView.GetShuoMingIndex();
				if (cbIndex < 4)
				{
					cbIndex++;
					m_GameClientView.SetShuoMingIndex(cbIndex);
				}
				else if (cbIndex == 4)
				{
					if(m_GameClientView.GetImageLoad())
					{
						m_bShuoMing = false;
						m_GameClientView.ShowShuoMing(false);
						KillTimer(IDI_GAME_SHUO_MING);
						m_GameClientView.EnterScene(1);
					}
					else
					{
						SetTimer(IDI_GAME_SHUO_MING, 50, NULL);
					}
				}
			}
			break;
		}
	/*case IDI_AUTO_GAME:
		{
			OnMessageOperate(VK_SPACE, 1);
			KillGameClockEx(IDI_AUTO_GAME);
			break;
		}*/
	}

	__super::OnTimer(nIDEvent);
}

//������ť
LRESULT	CGameClientEngine::OnMessageGameOperate(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_F3 || (m_bShuoMing && lParam == 0))
	{
		if(wParam == VK_F3)
		{
			if (m_bShuoMing)
			{
				if (m_GameClientView.GetImageLoad())
				{
					m_bShuoMing = false;
					m_GameClientView.ShowShuoMing(false);
					m_GameClientView.SetShuoMingPos(100);
					KillTimer(IDI_GAME_SHUO_MING);
					m_GameClientView.EnterScene(1);
				}
			}
			else
			{
				EnterShuoMing();
			}
		}		
		return 0;
	}
	else if (wParam == VK_ESCAPE)
	{
		OnMessageGameClose(1,0);

		return 0;
	}
	else if (wParam == VK_F8)
	{
		OnMessageGameSound(0, 0);
		return 0;
	}
	else if(wParam == VK_F4 && !IsLookonMode())
	{		
		m_bAutoStart = !m_bAutoStart;
		m_GameClientView.SetAutoGame(m_bAutoStart);
		
		if(m_cbGameMode == GM_NULL && GAME_SCENE_FREE == GetGameStatus())
		{			
			if(!m_bAutoStart)
			{
				m_GameClientView.SetTip( TEXT("���ո񡿼� ��ʼ"));
				KillGameClockEx(IDI_AUTO_GAME);
			}
			else
			{
				EnterOne();
			}
		}
	}

	if (GetGameStatus() == GAME_SCENE_ONE && !IsLookonMode() && wParam == VK_SPACE)
	{
		if (m_cbGameMode == GM_ONE)
		{
			//if( m_UserActionArray.GetCount() > 0 )
			//{
			//	enmActionKind ActionKind = m_UserActionArray[0]->enActionKind;
			//	if (ActionKind == AK_One_GunDong)
			//	{
			//		m_GameClientView.StartOneGuanDong(false);
			//	}
			//}
			StopUserAction();
			StopSound();
			CSound::_Object()->StopSound();
		}		
	}
	else if (GetGameStatus() == GAME_SCENE_FREE)
	{
		if (m_cbGameMode == GM_NULL)
		{
// 			if (wParam == VK_F3)
// 			{
// 				EnterShuoMing();
// 				return 0;
// 			}

			if(IsLookonMode()) return 0;

			if (wParam == VK_F6)
			{
				m_lBet++;
				if (m_lBet >= m_cbBetCount)
				{
					m_lBet = m_cbBetCount - 1;
				}
				else
				{
					PlaySoundEx(TEXT("coinCollide.wav"));
				}
			}
			else if (wParam == VK_F5)
			{
				m_lBet--;
				if (m_lBet < 1)
				{
					m_lBet = 0;
				}
				else
				{
					PlaySoundEx(TEXT("coinCollide.wav"));
				}
			}
			else if ((wParam == VK_SPACE))
			{
				ASSERT(GetMeUserItem() != NULL);
				if(GetMeUserItem()->GetUserScore() - m_lSaveScore < m_lBetScore[m_lBet] * m_lCellScore * m_lLine)
				{
					PostMessage(IDM_GAME_CLOSE, 2, 0);
					return 0;
				}
				//��������
				CMD_C_OneStart OneStart;
				OneStart.lBet = m_lBet;	

				//������Ϣ
				SendUserReady(NULL, NULL);
				SendSocketData(SUB_C_ONE_START, &OneStart, sizeof(OneStart));
				m_cbGameMode = GM_ONE;
			}
			if (wParam == VK_F2)
			{
				SCORE lScore = __max(1, GetMeUserItem()->GetUserScore() / 100);
				m_lSaveScore += lScore;
				if (m_lSaveScore > GetMeUserItem()->GetUserScore())
				{
					m_lSaveScore = GetMeUserItem()->GetUserScore();
				}
			}
			else if (wParam == VK_F1)
			{
				SCORE lScore = __max(1, GetMeUserItem()->GetUserScore() / 100);
				m_lSaveScore -= lScore;
				if (m_lSaveScore < 0)
				{
					m_lSaveScore = 0;
				}
			}

			//������ע
			m_GameClientView.SetSaveScore(m_lSaveScore);
			m_GameClientView.SetOneBet(m_lBetScore[m_lBet], m_lBetScore[m_lBet] * m_lCellScore * m_lLine);
		}
	}
	else if (GetGameStatus() == GAME_SCENE_TWO)
	{
		if(IsLookonMode()) return 0;

		if (!m_bOpenTwo)
		{
			if (m_bWaitOpenTwo)
			{
				if (m_bWaitBetTwo)
				{
					CMD_C_TwoStart TwoStart;
					TwoStart.cbOpenArea = 0xFF;
					if (wParam == VK_LEFT)
					{
						TwoStart.cbOpenArea = 0;
					}
					else if (wParam == VK_DOWN)
					{
						TwoStart.cbOpenArea = 1;
					}
					else if (wParam == VK_RIGHT)
					{
						TwoStart.cbOpenArea = 2;
					}
					else
					{
						return 0;
					}

					m_GameClientView.SetTwoBetIndex(TwoStart.cbOpenArea);
					PlaySoundEx(TEXT("xiabi.wav"));

					//������Ϣ
					SendSocketData(SUB_C_TWO_START, &TwoStart, sizeof(TwoStart));
					KillGameClockEx(IDI_USER_OPERATE);

					m_bOpenTwo = true;
					m_bWaitBetTwo = false;
				}

			}
			else
			{
				if (wParam == VK_SPACE)			//ȡ���ȱ�
				{
					m_cbOpenTwoMode = 0xFF;
					//������Ϣ
					SendSocketData(SUB_C_TWO_GIVEUP);
					KillGameClockEx(IDI_USER_OPERATE);

					PlaySoundEx(TEXT("coins_fly_in.wav"));

					return 0;
				}
				else if (m_GameClientView.GetImageLoad())
				{
					if (wParam == VK_LEFT)
					{
						m_cbOpenTwoMode = 0;
					}
					else if (wParam == VK_DOWN)
					{
						m_cbOpenTwoMode = 1;
					}
					else if (wParam == VK_RIGHT)
					{
						m_cbOpenTwoMode = 2;
						if(GetMeUserItem()->GetUserScore() < m_lOneGameScore)
						{
							static CInformation *pInformation = NULL;
							if(pInformation == NULL)
							{
								//��ʾ��Ϣ
								CInformation Information(this);
								pInformation = &Information;
								Information.ShowMessageBox(TEXT("���������� ���ܽ��б��ȣ�"),MB_ICONQUESTION|MB_OK,0);
								pInformation = NULL;
							}

							return 0;
						}
					}
					else if (wParam == VK_SPACE)			//ȡ���ȱ�
					{
						m_cbOpenTwoMode = 0xFF;
						//������Ϣ
						SendSocketData(SUB_C_TWO_GIVEUP);
						PlaySoundEx(TEXT("coins_fly_in.wav"));
						EnterOne();
						return 0;
					}
					else
					{
						return 0;
					}


					//׼��״̬
					tagActionEnter *pActionEnter = (tagActionEnter *)ActiveUserAction(AK_Enter);
					ASSERT(pActionEnter);
					if( !pActionEnter ) return false;
					pActionEnter->cbEnterMode = 2;

					m_UserActionArray.Add(pActionEnter);

					//��������
					BeginUserAction();

					////������Ϣ
					//CMD_C_TwoMode TwoMode;
					//TwoMode.cbOpenMode = m_cbOpenTwoMode;					
					//SendSocketData(SUB_C_TWO_MODE, &TwoMode, sizeof(TwoMode));

					m_bWaitOpenTwo = true;
					m_bWaitBetTwo = false;

					////����ȱ�
					//EnterTwo();
				}
			}		
		}
		else if (m_cbGameMode == GM_TWO_WAIT)
		{
			if (wParam == VK_SPACE)
			{
				m_cbOpenTwoMode = 0xFF;
				//������Ϣ
				SendSocketData(SUB_C_TWO_GIVEUP);
				PlaySoundEx(TEXT("coins_fly_in.wav"));
				m_GameClientView.ExitTwo();
				EnterOne();
			}
			else if (wParam == VK_RETURN)			//�����ȱ�
			{
				m_bOpenTwo = false;
				m_bWaitOpenTwo = true;
				m_bWaitBetTwo = false;
				//����ȱ�
				EnterTwo();
				return 0;
			}
		}

	}
	else if (GetGameStatus() == GAME_SCENE_THREE)
	{
		if(IsLookonMode()) return 0;

		if (!m_bOpenThree)
		{
			if (wParam == VK_SPACE)			//ȡ���ȱ�
			{
				m_bOpenThree = true;

				//׼��״̬
				tagActionEnter *pActionEnter = (tagActionEnter *)ActiveUserAction(AK_Enter);
				ASSERT(pActionEnter);
				if( !pActionEnter ) return false;
				pActionEnter->cbEnterMode = 3;

				m_UserActionArray.Add(pActionEnter);

				//��������
				BeginUserAction();

				////����С����
				//EnterThree();
			}
			return 0;						
		}
	}

	return 0;
}

//�Ӽ���ע
LRESULT CGameClientEngine::OnMessageGameBet(WPARAM wParam, LPARAM lParam)
{
	if (GetGameStatus() == GAME_SCENE_FREE)
	{
		if (m_cbGameMode == GM_NULL)
		{
			if(wParam == 1)
			{
				m_lBet++;
				if (m_lBet >= m_cbBetCount)
				{
					m_lBet = m_cbBetCount - 1;
				}
				else
				{
					PlaySoundEx(TEXT("coinCollide.wav"));
				}
			}
			else if(wParam == 0)
			{
				m_lBet--;
				if (m_lBet < 1)
				{
					m_lBet = 0;
				}
				else
				{
					PlaySoundEx(TEXT("coinCollide.wav"));
				}
			}
			else
			{
				return 0;
			}

			m_GameClientView.SetOneBet(m_lBetScore[m_lBet], m_lBetScore[m_lBet] * m_lCellScore * m_lLine);
		}
	}
	return 0;
}

//��Ϸ����
LRESULT CGameClientEngine::OnMessageGameSound(WPARAM wParam, LPARAM lParam)
{
	CGlobalUnits* pGlabalUnits = CGlobalUnits::GetInstance();
	pGlabalUnits->m_bMuteStatuts = !pGlabalUnits->m_bMuteStatuts;
	HINSTANCE hInstance=AfxGetInstanceHandle(); 
	if(pGlabalUnits->m_bMuteStatuts)
	{
		StopSound();
		CSound::_Object()->StopSound();
		m_GameClientView.m_ButtonSound.SetButtonTexture( m_GameClientView.m_pD3DDevice,CGlobal::_Object()->hModuleResource,_T("����"), _T("��Դ"), _T("BT_SOUND_CLOSE"));
	}
	else
	{
		m_GameClientView.m_ButtonSound.SetButtonTexture( m_GameClientView.m_pD3DDevice,CGlobal::_Object()->hModuleResource,_T("����"), _T("��Դ"), _T("BT_SOUND_OPEN"));
		if( m_UserActionArray.GetCount() > 0 )
		{
			enmActionKind ActionKind = m_UserActionArray[0]->enActionKind;
			if (ActionKind == AK_One_GunDong)
			{
				PlaySoundEx(TEXT("gundong.mp3"));
			}
			else if (ActionKind == AK_One_YaXian)
			{
				PlaySoundEx(TEXT("gundong_1.mp3"));
			}
			else if (ActionKind == AK_One_KaiJian)
			{
				bool bZhongJiang[ITEM_Y_COUNT][ITEM_X_COUNT] = {0};
				for (int i = 0; i < ITEM_COUNT; i++)
				{
					for (int j = 0; j < ITEM_X_COUNT; j++)
					{
						if(m_ptZhongJiang[i][j].x != 0xff)
						{
							bZhongJiang[m_ptZhongJiang[i][j].x][m_ptZhongJiang[i][j].y] = true;
						}
					}
				}

				BYTE cbIndex = CT_SHUIHUZHUAN;
				for (int i = 0; i < ITEM_Y_COUNT; i++)
				{
					for (int j = 0; j < ITEM_X_COUNT; j++)
					{
						if(bZhongJiang[i][j] && CT_SHUIHUZHUAN != m_cbItemInfo[i][j])
						{
							if (cbIndex < m_cbItemInfo[i][j] || cbIndex == CT_SHUIHUZHUAN)
							{
								cbIndex = m_cbItemInfo[i][j];
							}
						}
					}
				}

				if (cbIndex == CT_FUTOU)
				{
					PlaySoundEx(TEXT("winsound.mp3"));
				}
				else if(cbIndex == CT_YINGQIANG)
				{
					PlaySoundEx(TEXT("winsound.mp3"));
				}
				else if(cbIndex == CT_DADAO)
				{
					PlaySoundEx(TEXT("winsound.mp3"));
				}
				else if(cbIndex == CT_LU)
				{
					PlaySoundEx(TEXT("luzhisheng.mp3"));
				}
				else if(cbIndex == CT_LIN)
				{
					PlaySoundEx(TEXT("linchong.mp3"));
				}
				else if(cbIndex == CT_SONG)
				{
					PlaySoundEx(TEXT("songjiang.mp3"));
				}
				else if(cbIndex == CT_TITIANXINGDAO)
				{
					PlaySoundEx(TEXT("titianxingdao.mp3"));
				}
				else if(cbIndex == CT_ZHONGYITANG)
				{
					PlaySoundEx(TEXT("zhongyitang.mp3"));
				}
				else if(cbIndex == CT_SHUIHUZHUAN)
				{
					PlaySoundEx(TEXT("shuihuchuan.mp3"));
				}
			}
		}
	}
	return 0;
}

//��Ϸ�ر�
LRESULT CGameClientEngine::OnMessageGameClose(WPARAM wParam, LPARAM lParam)
{
	if(!IsLookonMode())
	{
		if(1 == wParam)
		{
			IClientUserItem * pIClientUserItem=GetTableUserItem(GetMeChairID());
			if(pIClientUserItem!=NULL)
			{
				if(pIClientUserItem->GetUserStatus() == US_PLAYING)
				{
					//��ʾ��Ϣ
					static CInformation *pInformation = NULL;
					if(pInformation == NULL)
					{
						CInformation Information(this);
						pInformation = &Information;
						INT nRes=Information.ShowMessageBox(TEXT("��������Ϸ�У�ȷ��Ҫǿ����"),MB_ICONQUESTION|MB_YESNO,0);
						pInformation = NULL;
						if (nRes!=IDYES)
						{
							return 0;
						}
					}
					else
					{
						return 0;
					}
				}
			}
		}
		if(2 == wParam)
		{
			static CInformation *pInformation = NULL;
			if(pInformation == NULL)
			{
				//��ʾ��Ϣ
				CInformation Information(this);
				pInformation = &Information;
				Information.ShowMessageBox(TEXT("���������������Ϸ֣�"),MB_ICONQUESTION|MB_OK,0);
				pInformation = NULL;
				m_bAutoStart = false;
				EnterOne();
			}

			return 0;
		}
	}
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
	if(m_GameClientView.m_pMyUIThread != NULL)
	{
		TerminateThread(m_GameClientView.m_pMyUIThread->m_hThread, 0);
		m_GameClientView.m_pMyUIThread = NULL;
	}
	return 0;
}

//��Ϸ����
LRESULT CGameClientEngine::OnMessageGameOption(WPARAM wParam, LPARAM lParam)
{
	//��ʾ����
	CDlgGameOption DlgGameOption;
	if (DlgGameOption.CreateGameOption(NULL,0)==true)
	{
		//��������
		ASSERT(CGlobalUnits::GetInstance()!=NULL);
		CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
		AllowBackGroundSound(pGlobalUnits->m_bAllowBackGroundSound);
		return 0;
	}

	return 0;
}

//����˵��
VOID CGameClientEngine::EnterShuoMing()
{
	SetTimer(IDI_GAME_SHUO_MING, 250, NULL);
	m_bShuoMing = true;
	m_GameClientView.ShowShuoMing(true);
	m_GameClientView.SetShuoMingIndex(0);
}

//����Ѻ��
VOID CGameClientEngine::EnterOne()
{
	//StopSound();
	//PlayBackGroundSound(TEXT("xiongdiwushu.wav"));

	KillGameClockEx(IDI_USER_OPERATE);
	//KillTimer(IDI_GAME_SHUO_MING);

	//����Ѻ��
	m_cbGameMode = GM_NULL;

	SetGameStatus(GAME_SCENE_FREE);
	m_GameClientView.SetGameStatus(GAME_SCENE_FREE);
	m_GameClientView.SetOneScore(0);
	m_GameClientView.SetOneTotalWin(0, 0);
	m_GameClientView.SetTip(m_bAutoStart ? TEXT("���ո񡿼� ��ʼ��2����Զ���ʼ��") : TEXT("���ո񡿼� ��ʼ"));	
	m_GameClientView.EnterScene(1);
	
	if(m_bAutoStart)
	{
		SetGameClockEx(GetMeChairID(),IDI_AUTO_GAME,2);
	}
}

//����ȱ�
VOID CGameClientEngine::EnterTwo()
{
// 	PlayBackGroundSound(TEXT("bibeibgsound.wav"));
	StopSound();
	CSound::_Object()->StopSound();
	CMusic::_Object()->AppendMusic(TEXT("bibeibgsound.wav"));
	CMusic::_Object()->PlayMusic();

	m_GameClientView.SetTip(TEXT(""));

	KillGameClockEx(IDI_USER_OPERATE);
	m_cbGameMode = GM_TWO;
	m_GameClientView.SetGameStatus(GAME_SCENE_TWO);	
	BYTE cb[2] = {0xFF, 0xFF};
	m_GameClientView.SetTwoSice(cb);
	SCORE lBet = ((m_cbOpenTwoMode == 0) ? __max(1, m_lOneGameScore / 2) : (m_cbOpenTwoMode == 1 ? m_lOneGameScore : m_lOneGameScore * 2));
	m_GameClientView.SetTwoScore(m_lTwoGameScore, lBet, m_lBetScore[m_lBet] * m_lCellScore  * m_lLine + lBet - m_lOneGameScore);
	m_GameClientView.SetOneScore(0);
	m_GameClientView.SetTwoScore(0);
	m_GameClientView.SetTwoBetIndex(0xFF);

	//׼��״̬
	tagActionTwoWaitBet *pActionTwoWaitBet = (tagActionTwoWaitBet *)ActiveUserAction(AK_Two_WaitBet);
	ASSERT(pActionTwoWaitBet);
	if( !pActionTwoWaitBet ) return ;
	pActionTwoWaitBet->bOne = true;
	pActionTwoWaitBet->cbExp = 0;
	pActionTwoWaitBet->nCurIndex = 0;
	pActionTwoWaitBet->nMaxIndex = 1;
	pActionTwoWaitBet->cbUser[0] = 2;		//Ĭ��
	pActionTwoWaitBet->cbUser[1] = 5;		//Ĭ��
	pActionTwoWaitBet->cbUser[2] = 2;		//Ĭ��
	m_UserActionArray.Add(pActionTwoWaitBet);

	pActionTwoWaitBet = (tagActionTwoWaitBet *)ActiveUserAction(AK_Two_WaitBet);
	ASSERT(pActionTwoWaitBet);
	if( !pActionTwoWaitBet ) return ;
	pActionTwoWaitBet->bOne = true;
	pActionTwoWaitBet->cbExp = 3;			//�������Ӻͽ�Ԫ��
	pActionTwoWaitBet->nCurIndex = 0;
	pActionTwoWaitBet->nMaxIndex = 2;
	pActionTwoWaitBet->cbUser[0] = 0;		//�Ȳ�
	pActionTwoWaitBet->cbUser[1] = 4;		//�ɹ�ҡ����
	pActionTwoWaitBet->cbUser[2] = 0;		//�Ȳ�
	m_UserActionArray.Add(pActionTwoWaitBet);

	//��������
	BeginUserAction();
}

//����ȱ�
VOID CGameClientEngine::EnterThree()
{
	//PlayBackGroundSound(TEXT("xiongdiwushu3.wav"));

	StopSound();
	CSound::_Object()->StopSound();

	m_cbGameMode = GM_THREE;
	m_GameClientView.SetTip(TEXT(""));

	m_GameClientView.SetGameStatus(GAME_SCENE_THREE);	

	m_GameClientView.SetThreeScore(0, m_lBetScore[m_lBet] * m_lCellScore * m_lLine, 0, -m_lOneGameScore-m_lBetScore[m_lBet] * m_lCellScore * m_lLine);
	m_GameClientView.SetOneScore(0);

	//������Ϣ
	SendSocketData(SUB_C_THREE_START);
}

//��������
VOID CGameClientEngine::PlaySoundEx(CString strSound)
{
	CGlobalUnits* pGlabalUnits = CGlobalUnits::GetInstance();
	if(!pGlabalUnits->m_bMuteStatuts)
	{
		//��������
		CSound::_Object()->StopSound();
		CSound::_Object()->PlaySound(strSound);
	}
}

//�������
LRESULT CGameClientEngine::OnUserActionFinishSteps(WPARAM wParam, LPARAM lParam)
{
	PlaySoundEx(TEXT("Threegundong.wav"));

	return 0;
}

//�������
LRESULT CGameClientEngine::OnUserActionFinish(WPARAM wParam, LPARAM lParam)
{
	//У��
	ASSERT(m_UserActionArray.GetCount() > 0);
	if (m_UserActionArray.GetCount() == 0) return 0;

	//ö�ٶ���
	switch (m_UserActionArray[0]->enActionKind)
	{
	case AK_One_GunDong:			//Ѻ�߿���
		{
			OnActionOneGunDong();
			break;
		}
	case AK_One_YaXian:
		{
			OnActionOneYaXian();
			break;
		}
	case AK_One_KaiJian:
		{
			OnActionOneKaiJian();
			break;
		}
	case AK_Two_WaitBet:
		{
			OnActionTwoWaitBet();
			break;
		}
	case AK_Two_KaiJian:
		{
			OnActionTwoKaiJian();
			break;
		}
	case AK_Three_GunDong:
		{
			OnActionThreeGunDong();
			break;
		}
	case AK_Three_KaiJian:
		{
			OnActionThreeKaiJian();
			break;
		}
	case AK_Three_KaiJianEnd:
		{
			OnActionThreeKaiJianEnd();
			break;
		}
	case AK_Three_End:
		{
			OnActionThreeEnd();
			break;
		}
	case AK_GameConclude:
		{
			OnActionGameConclude();
			break;
		}
	case AK_Enter:
		{
			OnActionEnter();
			break;
		}
	default:
		ASSERT(FALSE);
		return 0;
	}

	return 0;
}

//��ʼ��Ҷ���
bool CGameClientEngine::BeginUserAction()
{
	if( m_UserActionArray.GetCount() == 0 )
		return false;

	//ö�ٶ���
	switch( m_UserActionArray[0]->enActionKind )
	{
	case AK_One_GunDong:			//Ѻ�߿���
		return BeginActionOneGunDong();
	case AK_One_YaXian:
		return BeginActionOneYaXian();
	case AK_One_KaiJian:
		return BeginActionOneKaiJian();
	case AK_Two_WaitBet:
		return BeginActionTwoWaitBet();
	case AK_Two_KaiJian:
		return BeginActionTwoKaiJian();
	case AK_Three_GunDong:
		return BeginActionThreeGunDong();
	case AK_Three_KaiJian:
		return BeginActionThreeKaiJian();
	case AK_Three_KaiJianEnd:
		return BeginActionThreeKaiJianEnd();
	case AK_Three_End:
		return BeginActionThreeEnd();
	case AK_GameConclude:
		return BeginActionGameConclude();
	case AK_Enter:
		return BeginActionEnter();
	default:
		ASSERT(FALSE);
		return false;
	}

	return false;
}

//ֹͣ��Ҷ���
bool CGameClientEngine::StopUserAction()
{
	while( BeginUserAction() )
	{
		//�ж�����
		ASSERT(m_UserActionArray.GetCount() > 0 );
		if( m_UserActionArray.GetCount() > 0 )
		{
			enmActionKind ActionKind = m_UserActionArray[0]->enActionKind;
			if (ActionKind == AK_One_GunDong)
			{
				m_GameClientView.StartOneGuanDong(false);
			}
			else if (ActionKind == AK_One_YaXian)
			{
				m_GameClientView.StartZhongJiangXian(0xFF);
			}
			else if (ActionKind == AK_One_KaiJian)
			{
				m_GameClientView.StartKaiJang();
			}
		}
	}

	StopSound();
	CSound::_Object()->StopSound();

	return true;
}

//�Ƴ���ǰ����
bool CGameClientEngine::RemoveCurrentAction()
{
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//ɾ����ǰ����
	tagUserAction *pUserAction = m_UserActionArray[0];
	m_UserActionArray.RemoveAt(0);
	m_UserActionStorage.Add(pUserAction);
	pUserAction->ResetAction();

	return true;
}

//�����
tagUserAction *CGameClientEngine::ActiveUserAction(enmActionKind enActionKind)
{
	//��������
	tagUserAction *pUserAction = NULL;

	//Ѱ�ҿ��
	for( INT_PTR i = 0; i < m_UserActionStorage.GetCount(); i++ )
	{
		if( m_UserActionStorage[i]->enActionKind == enActionKind )
		{
			pUserAction = m_UserActionStorage[i];
			m_UserActionStorage.RemoveAt(i);
			return pUserAction;
		}
	}

	//�´���
	switch( enActionKind )
	{
	case AK_One_GunDong:
		{
			pUserAction = new tagActionOneGunDong;
			return pUserAction;
		}
	case AK_One_YaXian:
		{
			pUserAction = new tagActionOneYaXian;
			return pUserAction;
		}
	case AK_One_KaiJian:
		{
			pUserAction = new tagActionOneKaiJian;
			return pUserAction;
		}
	case AK_Two_WaitBet:
		{
			pUserAction = new tagActionTwoWaitBet;
			return pUserAction;
		}
	case AK_Two_KaiJian:
		{
			pUserAction = new tagActionTwoKaiJian;
			return pUserAction;
		}
	case AK_Three_GunDong:
		{
			pUserAction = new tagActionThreeGunDong;
			return pUserAction;
		}
	case AK_Three_KaiJian:
		{
			pUserAction = new tagActionThreeKaiJian;
			return pUserAction;
		}
	case AK_Three_KaiJianEnd:
		{
			pUserAction = new tagActionThreeKaiJianEnd;
			return pUserAction;
		}
	case AK_Three_End:
		{
			pUserAction = new tagActionThreeEnd;
			return pUserAction;
		}
	case AK_GameConclude:
		{
			pUserAction = new tagActionGameConclude;
			return pUserAction;
		}
	case AK_Enter:
		{
			pUserAction = new tagActionEnter;
			return pUserAction;
		}
	default:
		{
			ASSERT(FALSE);
			break;
		}
	}

	return pUserAction;
}

//Ѻ�߿���
bool CGameClientEngine::BeginActionOneGunDong()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionOneGunDong *pActionOneGunDong = (tagActionOneGunDong *)m_UserActionArray[0];	

	pActionOneGunDong->bHandleStatus = TRUE;

	m_GameClientView.StartOneGuanDong(true, m_cbItemInfo);
	PlaySoundEx(TEXT("gundong.mp3"));

	return true;
}

//Ѻ��
bool CGameClientEngine::BeginActionOneYaXian()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionOneYaXian *pActionOneYaXian = (tagActionOneYaXian *)m_UserActionArray[0];

	pActionOneYaXian->bHandleStatus = TRUE;

	m_GameClientView.StartZhongJiangXian(pActionOneYaXian->nZhongJiangXian, pActionOneYaXian->ptXian);
	PlaySoundEx(TEXT("gundong_1.mp3"));

	if(pActionOneYaXian->lScore > 0)
	{
		m_GameClientView.SetOneTotalWin(pActionOneYaXian->lScore, pActionOneYaXian->lXianScore);
	}

	return true;
}

//����
bool CGameClientEngine::BeginActionOneKaiJian()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionOneKaiJian *pActionOneKaiJian = (tagActionOneKaiJian *)m_UserActionArray[0];

	pActionOneKaiJian->bHandleStatus = TRUE;

	m_GameClientView.StartZhongJiangXian(0xFF);
	m_GameClientView.StartKaiJang(pActionOneKaiJian->bZhongJiang);
	m_GameClientView.SetTip(TEXT(""));
	m_GameClientView.SetOneTotalWin(pActionOneKaiJian->lScore, pActionOneKaiJian->lQuanPanScore);

	BYTE cbIndex = CT_SHUIHUZHUAN;
	for (int i = 0; i < ITEM_Y_COUNT; i++)
	{
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			if(pActionOneKaiJian->bZhongJiang[i][j] && CT_SHUIHUZHUAN != m_cbItemInfo[i][j])
			{
				if (cbIndex < m_cbItemInfo[i][j] || cbIndex == CT_SHUIHUZHUAN)
				{
					cbIndex = m_cbItemInfo[i][j];
				}
			}
		}
	}

	if (cbIndex == CT_FUTOU)
	{
		PlaySoundEx(TEXT("winsound.mp3"));
	}
	else if(cbIndex == CT_YINGQIANG)
	{
		PlaySoundEx(TEXT("winsound.mp3"));
	}
	else if(cbIndex == CT_DADAO)
	{
		PlaySoundEx(TEXT("winsound.mp3"));
	}
	else if(cbIndex == CT_LU)
	{
		PlaySoundEx(TEXT("luzhisheng.mp3"));
	}
	else if(cbIndex == CT_LIN)
	{
		PlaySoundEx(TEXT("linchong.mp3"));
	}
	else if(cbIndex == CT_SONG)
	{
		PlaySoundEx(TEXT("songjiang.mp3"));
	}
	else if(cbIndex == CT_TITIANXINGDAO)
	{
		PlaySoundEx(TEXT("titianxingdao.mp3"));
	}
	else if(cbIndex == CT_ZHONGYITANG)
	{
		PlaySoundEx(TEXT("zhongyitang.mp3"));
	}
	else if(cbIndex == CT_SHUIHUZHUAN)
	{
		PlaySoundEx(TEXT("shuihuchuan.mp3"));
	}

	return true;
}

//�ȴ�����
bool CGameClientEngine::BeginActionTwoWaitBet()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionTwoWaitBet *pActionTwoWaitBet = (tagActionTwoWaitBet *)m_UserActionArray[0];

	pActionTwoWaitBet->bHandleStatus = TRUE;
	m_GameClientView.SetUser(pActionTwoWaitBet->cbUser, pActionTwoWaitBet->bOne, pActionTwoWaitBet->cbExp);

	if (pActionTwoWaitBet->cbUser[1] == 4)
	{
		PlaySoundEx(TEXT("yaosaizi.mp3"));
	}

	return true;
}

//����
bool CGameClientEngine::BeginActionTwoKaiJian()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionTwoKaiJian *pActionTwoKaiJian = (tagActionTwoKaiJian *)m_UserActionArray[0];

	pActionTwoKaiJian->bHandleStatus = TRUE;

	m_GameClientView.SetUser(pActionTwoKaiJian->cbUser, true, 0);

	BYTE cbSice = pActionTwoKaiJian->cbSice[0] + pActionTwoKaiJian->cbSice[1];
	if (cbSice >= 2 && cbSice <= 12)
	{
		CString str;
		str.Format(TEXT("%ddian.mp3"), cbSice);
		PlaySoundEx(str);
	}

	return true;
}

//��ʼ
bool CGameClientEngine::BeginActionThreeGunDong()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionThreeGunDong *pActionThreeGunDong = (tagActionThreeGunDong *)m_UserActionArray[0];

	pActionThreeGunDong->bHandleStatus = TRUE;

	m_GameClientView.StartThreeGunDong(pActionThreeGunDong->cbItem);
	m_GameClientView.SetThreeTimes(pActionThreeGunDong->cbBounsGameCount);
	PlaySoundEx(TEXT("gundong.mp3"));

	return true;
}

//�������
bool CGameClientEngine::BeginActionThreeKaiJian()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionThreeKaiJian *pActionThreeKaiJian = (tagActionThreeKaiJian *)m_UserActionArray[0];

	pActionThreeKaiJian->bHandleStatus = TRUE;

	m_GameClientView.StartThreeKaiJian(pActionThreeKaiJian->cbIndex);

	return true;
}

//�������
bool CGameClientEngine::BeginActionThreeKaiJianEnd()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionThreeKaiJianEnd *pActionThreeKaiJianEnd = (tagActionThreeKaiJianEnd *)m_UserActionArray[0];

	pActionThreeKaiJianEnd->bHandleStatus = TRUE;

	m_GameClientView.StartThreeKaiJianEnd(pActionThreeKaiJianEnd->lScore);

	return true;
}

//�������
bool CGameClientEngine::BeginActionThreeEnd()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionThreeEnd *pActionThreenEnd = (tagActionThreeEnd *)m_UserActionArray[0];

	pActionThreenEnd->bHandleStatus = TRUE;

	return OnActionThreeEnd();
}

//�������
bool CGameClientEngine::BeginActionGameConclude()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionGameConclude *pActionGameConclude = (tagActionGameConclude *)m_UserActionArray[0];

	pActionGameConclude->bHandleStatus = TRUE;

	//��Ϸ����
	EnterOne();

	return OnActionGameConclude();
}

//��Ϸ����
bool CGameClientEngine::BeginActionEnter()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;
	if( m_UserActionArray[0]->bHandleStatus ) return true;

	//��������
	tagActionEnter *pActionEnter = (tagActionEnter *)m_UserActionArray[0];

	pActionEnter->bHandleStatus = TRUE;

	PlaySoundEx(TEXT("coins_fly_in.wav"));

	SetTimer(IDI_ENTER, 1000, NULL);

	return true;
}

//Ѻ�߿������
bool CGameClientEngine::OnActionOneGunDong()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionOneGunDong *pActionOneGunDong = (tagActionOneGunDong *)m_UserActionArray[0];

	if (++pActionOneGunDong->nCurIndex < pActionOneGunDong->nMaxIndex)
	{
		return true;
	}

	if (pActionOneGunDong->lScore <= 0)
	{
		m_bWaitOpenOne = true;
		SendSocketData(SUB_C_ONE_END);
		SetGameStatus(GAME_SCENE_FREE);
	}

	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}

//Ѻ�����
bool CGameClientEngine::OnActionOneYaXian()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionOneYaXian *pActionOneYaXian = (tagActionOneYaXian *)m_UserActionArray[0];

	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}

//�������
bool CGameClientEngine::OnActionOneKaiJian()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionOneKaiJian *pActionOneKaiJian = (tagActionOneKaiJian *)m_UserActionArray[0];


	if (++pActionOneKaiJian->nCurIndex < pActionOneKaiJian->nMaxIndex)
	{
		return true;
	}

	if(m_lOneGameScore > 0)
	{
		m_cbGameMode = pActionOneKaiJian->cbGameMode;
		m_bWaitOpenOne = false;
		m_GameClientView.StartKaiJangEnd(m_lOneGameScore, false);

		if (m_cbGameMode == GM_TWO)
		{
			m_lTwoGameScore = 0;
			SetGameStatus(GAME_SCENE_TWO);
			m_GameClientView.SetWaitMode(GAME_SCENE_TWO);
			m_GameClientView.SetTip(TEXT("���ո񡿼� ȡ�� ��������� ������ȫ�� ����������"));
			SetGameClockEx(GetMeChairID(),IDI_USER_OPERATE,60);
		}
		else
		{
			SetGameStatus(GAME_SCENE_THREE);
			if(m_bAutoStart)
			{
				m_GameClientView.SetTip(TEXT("���ո񡿼� ����С����ģʽ��2����Զ����룩"));
				m_GameClientView.SetWaitMode(GAME_SCENE_THREE);
				SetGameClockEx(GetMeChairID(),IDI_AUTO_GAME,2);
			}
			else
			{
				m_GameClientView.SetTip(TEXT("���ո񡿼� ����С����ģʽ��30����Զ����룩"));
				m_GameClientView.SetWaitMode(GAME_SCENE_THREE);
				SetGameClockEx(GetMeChairID(),IDI_AUTO_GAME,30);
			}
		}
	}
	else
	{
		SendSocketData(SUB_C_ONE_END);
		SetGameStatus(GAME_SCENE_FREE);
	}

	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}

//�ȴ��������
bool CGameClientEngine::OnActionTwoWaitBet()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionTwoWaitBet *pActionTwoWaitBet = (tagActionTwoWaitBet *)m_UserActionArray[0];

	if (++pActionTwoWaitBet->nCurIndex < pActionTwoWaitBet->nMaxIndex)
	{
		if (pActionTwoWaitBet->cbExp == 2)
		{
			m_GameClientView.SetTwoShowSice(false, true);
		}
		if (pActionTwoWaitBet->cbExp == 3 && pActionTwoWaitBet->nCurIndex == 1)
		{
			SetGameClockEx(GetMeChairID(),IDI_USER_OPERATE,60);
			m_GameClientView.SetTwoDouDong();
			PlaySoundEx((rand() % 2 ? TEXT("xia1.wav") : TEXT("xia.wav")));
		}
		return true;
	}

	if (pActionTwoWaitBet->cbExp == 3)
	{
		BYTE cbUser[3] = {2, 5, 2};
		m_GameClientView.SetUser(cbUser, false, 0);
		m_GameClientView.StartTwoWiatAddScore(true);
		m_bWaitBetTwo = true;
	}
	else if (pActionTwoWaitBet->cbExp == 2)
	{
		//�ȱ�����
		BYTE cbUser[3] = {2, 5, 2};
		m_GameClientView.SetUser(cbUser, false, 0);
		m_GameClientView.SetTwoShowSice(false, true);
		if (pActionTwoWaitBet->lScore > 0)
		{			
			//�ȴ�����
			m_cbGameMode = GM_TWO_WAIT;
			SetGameClockEx(GetMeChairID(),IDI_USER_OPERATE,60);
		}
		else
		{
			SendSocketData(SUB_C_TWO_GIVEUP);
			m_GameClientView.ExitTwo();
			EnterOne();
		}

	}

	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}

//�������
bool CGameClientEngine::OnActionTwoKaiJian()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionTwoKaiJian *pActionTwoKaiJian = (tagActionTwoKaiJian *)m_UserActionArray[0];

	if (++pActionTwoKaiJian->nCurIndex < pActionTwoKaiJian->nMaxIndex)
	{
		m_GameClientView.SetTwoSice(pActionTwoKaiJian->cbSice);
		m_GameClientView.SetTwoShowSice(true, false);
		return true;
	}

	m_lTwoGameScore += pActionTwoKaiJian->lScore;
	m_GameClientView.SetTwoShowSice(true, true);

	SCORE lBet = ((m_cbOpenTwoMode == 0) ? __max(1, m_lOneGameScore / 2) : (m_cbOpenTwoMode == 1 ? m_lOneGameScore : m_lOneGameScore * 2));
	m_GameClientView.SetTwoScore(__max(0, m_lTwoGameScore), lBet , pActionTwoKaiJian->lScore > 0 ? m_lBetScore[m_lBet] * m_lCellScore * m_lLine + lBet - m_lOneGameScore : 0);
	m_GameClientView.SetTwoScore(pActionTwoKaiJian->lScore);

	PlaySoundEx((pActionTwoKaiJian->lScore > 0 ? TEXT("ying.mp3") : TEXT("shu.mp3")));
	CMusic::_Object()->StopMusic();
	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}

//����
bool CGameClientEngine::OnActionThreeGunDong()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionThreeGunDong *pActionThreeGunDong = (tagActionThreeGunDong *)m_UserActionArray[0];

	if (++pActionThreeGunDong->nCurIndex < pActionThreeGunDong->nMaxIndex)
	{
		return true;
	}

	BYTE cbPos;
	m_GameLogic.GetThreeZhongJiangInfo(pActionThreeGunDong->cbItem, cbPos);
	m_GameClientView.StartThreeGunDongEnd(pActionThreeGunDong->lScore, cbPos);

	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}

//�������
bool CGameClientEngine::OnActionThreeKaiJian()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionThreeKaiJian *pActionThreeKaiJian = (tagActionThreeKaiJian *)m_UserActionArray[0];

	if (++pActionThreeKaiJian->nCurIndex < pActionThreeKaiJian->nMaxIndex)
	{
		return true;
	}

	m_GameClientView.StartThreeKaiJianEnd(pActionThreeKaiJian->lScore);
	m_GameClientView.SetThreeTotalWin(pActionThreeKaiJian->lScore) ;
	//m_GameClientView.SetThreeTimes(pActionThreeKaiJian->nTime);


	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}

//�������
bool CGameClientEngine::OnActionThreeKaiJianEnd()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionThreeKaiJianEnd *pActionThreeKaiJianEnd = (tagActionThreeKaiJianEnd *)m_UserActionArray[0];

	if (++pActionThreeKaiJianEnd->nCurIndex < pActionThreeKaiJianEnd->nMaxIndex)
	{
		return true;
	}

	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}

//�������
bool CGameClientEngine::OnActionThreeEnd()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionThreeEnd *pActionThreeEnd = (tagActionThreeEnd *)m_UserActionArray[0];

	//������Ϣ
	SendSocketData(SUB_C_THREE_END);

	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnActionGameConclude()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionGameConclude *pActionGameConclude = (tagActionGameConclude *)m_UserActionArray[0];

	if (++pActionGameConclude->nCurIndex < pActionGameConclude->nMaxIndex)
	{
		return true;
	}

	EnterOne();

	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnActionEnter()
{
	//Ч��
	ASSERT( m_UserActionArray.GetCount() > 0 );
	if( m_UserActionArray.GetCount() == 0 ) return false;

	//��������
	tagActionEnter *pActionEnter = (tagActionEnter *)m_UserActionArray[0];

	if(pActionEnter->cbEnterMode == 2)
	{
		//������Ϣ
		CMD_C_TwoMode TwoMode;
		TwoMode.cbOpenMode = m_cbOpenTwoMode;					
		SendSocketData(SUB_C_TWO_MODE, &TwoMode, sizeof(TwoMode));

		m_bWaitOpenTwo = true;
		m_bWaitBetTwo = false;

		//����ȱ�
		EnterTwo();
	}
	else if(pActionEnter->cbEnterMode == 3)
	{
		//����С����
		EnterThree();
	}
	//ɾ������
	RemoveCurrentAction();

	//�¸�����
	BeginUserAction();

	return true;
}
