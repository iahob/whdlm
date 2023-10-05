#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"
#include ".\gameclientengine.h"

//////////////////////////////////////////////////////////////////////////////////
// ��ʱ��
#define		IDI_TIME_OPEN_WIN				101					// �򿪴���
#define		IDI_TIME_CLOSE_WIN				102					// �رմ���


//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_TURN_OVER, OnMessageTurnOver)
	ON_MESSAGE(IDM_START_GAME, OnMessageStartGame)
	ON_MESSAGE(IDM_STOP_GAME, OnMessageStopGame)
	ON_MESSAGE(IDM_START_HOSTED, OnMessageStartHosted)
	ON_MESSAGE(IDM_STOP_HOSTED, OnMessageStopHosted)
	ON_MESSAGE(IDM_SCORE_LESS, OnMessageScoreLess)
	ON_MESSAGE(IDM_SCORE_ADD, OnMessageScoreAdd)
	ON_MESSAGE(IDM_SCORE_MAX, OnMessageScoreMax)
	ON_MESSAGE(IDM_LINE_CHECK, OnMessageLineCheck)
	ON_MESSAGE(IDM_OPEN_CONTROL,OnMessageOpenControl)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

// ���캯��
CGameClientEngine::CGameClientEngine() : m_GameClientView( m_GameFrameViewDexter.m_GameClientView )
{
	// ������Ϣ
	m_bPlayGame = false;
	m_bPlayHosted = false;

	// ��ע��Ϣ
	m_nBetIndex = 0;
	ZeroMemory( m_nBetScore, sizeof(m_nBetScore) );
	for( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex)
		m_bBetLine[nIndex] = true;

	int nBetScore[BET_INDEX_MAX] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	CopyMemory( m_nBetScore, nBetScore, sizeof(m_nBetScore) );

	// Ӯ��Ϣ
	m_lPlayWinScore = 0l;
	m_nPlayTaskScore = 0;
	m_nPlayMultipleIndex = 0;
	m_bMagnification = false;
	ZeroMemory( m_bTaskComplete, sizeof(m_bTaskComplete) );
	ZeroMemory( m_bPlayWinLine, sizeof(m_bPlayWinLine) );
	ZeroMemory( m_nElementIndex, sizeof(m_nElementIndex) );

}

// ��������
CGameClientEngine::~CGameClientEngine()
{
	ReleaseInterface( m_hControlInst, m_pIClientControl );
}

// ��������
bool CGameClientEngine::OnInitGameEngine()
{
	// ��������
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	// ��ʶ1
	if( lstrcmp( m_ClientCompilationSink->GetCompilation(), szCompilation ) == 0 )
		m_bLegalOne = true;
	else
		m_bLegalOne = false;

	// ��ȡ����
	g_GameOption.InitParameter( AfxGetAppName() );

	// ���ز���
	g_GameOption.LoadParameter();

	// ��������
	CSound::_Object()->CreateD3DSound( AfxGetMainWnd()->GetSafeHwnd() );
	CMusic::_Object()->CreateD3DSound( AfxGetMainWnd()->GetSafeHwnd() );

	// �л���ť
	m_GameClientView.m_ButtonSoundOn.ShowWindow( g_GameOption.bSound == TRUE, dex::WindowShow_Size );
	m_GameClientView.m_ButtonSoundOff.ShowWindow( g_GameOption.bSound == FALSE, dex::WindowShow_Size );

	// ��������
	CMusic::_Object()->AppendMusic( _T("BackSound.ogg") );
	if( g_GameOption.bSound )
		CMusic::_Object()->PlayMusic();

	// ���ñ�ʶ2
	m_bLagalTwo = m_bLegalOne;

	// ��������
	CreateInterface(m_hControlInst, m_pIClientControl, IClientControl, "LineClientControl.dll", "CreateClientControl" );

	// ��������
	if ( m_pIClientControl ) m_pIClientControl->Create( GetDesktopWindow(), this );

	return true;
}

// ���ú���
bool CGameClientEngine::OnResetGameEngine()
{
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
	return false;
}

// �Թ���Ϣ
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

// ��Ϸ��Ϣ
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch( wSubCmdID )
	{
	case SUB_S_ELEMENT_STOP:
		return OnSubElementStop( pData, wDataSize );
	case SUB_S_LOTTERY_POOL:
		return OnSubLotteryPool( pData, wDataSize );
	case SUB_S_TASK_INFO:
		return OnSubTaskInfo( pData, wDataSize );
	case SUB_S_AWARD_INFO:
		return OnSubAwardInfo( pData, wDataSize );
	case SUB_S_CONTROL:
		return OnSubControl( pData, wDataSize );
	}
	return false;
}

// ������Ϣ
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	// ������Ϣ
	if( !m_bLagalTwo || !m_bLegalOne )
	{
		AfxMessageBox( _T("�Ƿ������") );
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return false;
	}

	// �ж�
	if( lstrcmp( m_ClientCompilationSink->GetCompilation(), szCompilation ) != 0 )
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return false;
	}

	// ִ����Ϣ
	switch ( cbGameStatus )
	{
	case GAME_SCENE_FREE:
	case GAME_SCENE_PLAY:
		{
			// Ч������
			ASSERT( wDataSize == sizeof(CMD_S_Status) );
			if ( wDataSize != sizeof(CMD_S_Status) ) return false;

			// �������
			CMD_S_Status * pScene = (CMD_S_Status*)pData;

			// ������Ϣ
			CopyMemory( m_nBetScore, pScene->nBetScore, sizeof(m_nBetScore) );

			// ���ý���
			m_GameClientView.SetRoomLotteryPool( pScene->nRoomLotteryPool );
			m_GameClientView.SetThreeLineMultiple( pScene->nThreeLineMultiple );
			m_GameClientView.SetNineLineMultiple( pScene->nNineLineMultiple );
			m_GameClientView.SetTaskProgress( pScene->nTaskProgress );
			m_GameClientView.SetTaskTarget( pScene->nTaskTarget );
			m_GameClientView.SetTaskCount( pScene->nTaskCount );
			m_GameClientView.SetTaskScore( pScene->nTaskScore );

			// ����ָ��
			m_GameClientView.SetClientOneSelf( GetMeUserItem() );
			m_GameClientView.SetBetScore( m_nBetScore[m_nBetIndex] );
			m_GameClientView.SetBetLine( m_bBetLine );

			// ����·��
			for ( int nRoadIndex = 0; nRoadIndex < ROAD_MULTIPLE_MAX; ++nRoadIndex )
			{
				if( pScene->nRoadMultipleIndex[nRoadIndex] != MULTIPLE_INVALID )
				{
					m_GameClientView.m_WindowRoad.AppendRoad( pScene->nRoadMultipleIndex[nRoadIndex] );
				}
			}

			//���ư�ť
			if( CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_pIClientControl )
			{
				m_GameClientView.m_ButtonControl.ShowWindow( true );
			}

			return true;
		}
	}

	return false;
}

// ������Ϣ
bool CGameClientEngine::SendControlMessage( uint nMessageID, void * pData, uint nSize )
{
	// ������Ϣ
	CMD_C_Control CMDCControl;
	ZeroMemory( &CMDCControl, sizeof(CMDCControl) );

	// �ж���Ϣ
	ASSERT( nSize < sizeof(CMDCControl.cbMessageInfo) );

	// ��ֵ��Ϣ
	CMDCControl.nMessageID = nMessageID;
	CMDCControl.nMessageSize = nSize;
	if( pData != NULL && nSize != 0 )
		CopyMemory( CMDCControl.cbMessageInfo, pData, nSize );

	//������Ϣ
	SendSocketData(SUB_C_CONTROL, &CMDCControl, sizeof(CMDCControl));

	return true;
}


// ��ʱ����Ϣ
void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	// �ж϶�ʱ��
	switch( nIDEvent )
	{
	case IDI_TIME_OPEN_WIN:
		{
			// ɾ����ʱ
			KillTimer( IDI_TIME_OPEN_WIN );

			// �رմ���
			m_GameClientView.m_WindowEgg.ShowWindow( false, dex::WindowShow_Size );

			// ����������ʾ
			if ( m_nPlayMultipleIndex >= 0 && m_nPlayMultipleIndex < MultipleThree_Max + MultipleNine_Max )
			{
				// �����رն�ʱ
				SetTimer( IDI_TIME_CLOSE_WIN, 5000, NULL );

				// ������Ч
				if( m_nPlayMultipleIndex >= MultipleThree_Max )
				{
					CSound::_Object()->PlaySound( _T("GoldBigMove.ogg") );
				}
				else
				{
					CSound::_Object()->PlaySound( _T("GoldMove.ogg") );
				}

				// ����Ӯ����
				m_GameClientView.m_WindowWin.SetShowTime( 200 );
				m_GameClientView.m_WindowWin.AppendGold( m_bMagnification );
				m_GameClientView.m_WindowWin.SetWinScore( m_nPlayMultipleIndex, m_lPlayWinScore, m_nPlayTaskScore, m_bTaskComplete );
				m_GameClientView.m_WindowWin.ShowWindow( true, dex::WindowShow_Size );
			}
			else
			{
				// ������Ϸ
				m_bPlayGame = false;

				// �й����Զ���ʼ
				if( m_bPlayHosted )
				{
					OnMessageStartGame(0, 0);
				}
			}
		}
		break;
	case IDI_TIME_CLOSE_WIN:
		{
			// ɾ����ʱ
			KillTimer( IDI_TIME_CLOSE_WIN );

			// �رմ���
			m_GameClientView.m_WindowWin.ShowWindow( false, dex::WindowShow_Size );

			// ������Ϸ
			m_bPlayGame = false;

			// �й����Զ���ʼ
			if( m_bPlayHosted )
			{
				OnMessageStartGame(0, 0);
			}
		}
		break;
	}

	// ����ִ��
	CGameFrameEngine::OnTimer(nIDEvent);
}

// Ԫ����Ϣ
bool CGameClientEngine::OnSubElementStop( VOID * pData, WORD wDataSize )
{
	// Ч������
	ASSERT( wDataSize == sizeof(CMD_S_ElementStop) );
	if ( wDataSize != sizeof(CMD_S_ElementStop) ) return false;

	// �������
	CMD_S_ElementStop * pElementStop = (CMD_S_ElementStop*)pData;

	// ������Ϣ
	m_lPlayWinScore = pElementStop->lPlayWinScore;
	m_nPlayTaskScore = pElementStop->nPlayTaskScore;
	m_nPlayMultipleIndex = pElementStop->nMultipleIndex;
	m_bMagnification = pElementStop->bMagnification;
	CopyMemory( m_bTaskComplete, pElementStop->bTaskComplete, sizeof(m_bTaskComplete) );
	CopyMemory( m_bPlayWinLine, pElementStop->bPlayWinLine, sizeof(m_bPlayWinLine) );
	CopyMemory( m_nElementIndex, pElementStop->nElementIndex, sizeof(m_nElementIndex) );

	// ֹͣ��ת
	m_GameClientView.m_WindowTurntable.StopElement( pElementStop->nElementIndex, pElementStop->bStopNew );

	return true;
}

// �ʳ���Ϣ
bool CGameClientEngine::OnSubLotteryPool( VOID * pData, WORD wDataSize )
{
	// Ч������
	ASSERT( wDataSize == sizeof(CMD_S_LotteryPool) );
	if ( wDataSize != sizeof(CMD_S_LotteryPool) ) return false;

	// �������
	CMD_S_LotteryPool * pLotteryPool = (CMD_S_LotteryPool*)pData;

	// ���ý���
	m_GameClientView.SetRoomLotteryPool( pLotteryPool->nRoomLotteryPool );

	return true;
}

// ������Ϣ
bool CGameClientEngine::OnSubTaskInfo( VOID * pData, WORD wDataSize )
{
	// Ч������
	ASSERT( wDataSize == sizeof(CMD_S_TaskInfo) );
	if ( wDataSize != sizeof(CMD_S_TaskInfo) ) return false;

	// �������
	CMD_S_TaskInfo * pTaskInfo = (CMD_S_TaskInfo*)pData;

	// ���ý���
	m_GameClientView.SetTaskProgress( pTaskInfo->nTaskProgress );

	return true;
}

// �н���Ϣ
bool CGameClientEngine::OnSubAwardInfo( VOID * pData, WORD wDataSize )
{
	// Ч������
	ASSERT( wDataSize == sizeof(CMD_S_AwardInfo) );
	if ( wDataSize != sizeof(CMD_S_AwardInfo) ) return false;

	// �������
	CMD_S_AwardInfo * pAwardInfo = (CMD_S_AwardInfo*)pData;

	// �û�ָ��
	IClientUserItem * pMeUserItem = GetMeUserItem();

	// ���Լ�����
	if ( pMeUserItem == NULL || pMeUserItem->GetTableID() != pAwardInfo->nTableID )
	{
		// ��Ӷ���
		m_GameClientView.AppendAward( pAwardInfo->szPlayName, pAwardInfo->nMultipleIndex, pAwardInfo->nAwardScore );
	}
	else
	{
		// �����н�
		tagAwardInfo TAwardInfo;
		_sntprintf(TAwardInfo.szPlayName, CountArray(TAwardInfo.szPlayName), TEXT("%s"), pAwardInfo->szPlayName);
		TAwardInfo.nMultipleIndex = pAwardInfo->nMultipleIndex;
		TAwardInfo.nAwardScore = pAwardInfo->nAwardScore;

		// ������Ϣ
		m_ArrayAwardInfo.Add(TAwardInfo);
	}

	return true;
}

//������Ϣ
bool CGameClientEngine::OnSubControl( VOID * pData, WORD wDataSize )
{
	// Ч������
	ASSERT(wDataSize==sizeof(CMD_C_Control));
	if (wDataSize!=sizeof(CMD_C_Control)) return false;

	// �������
	CMD_C_Control * pControl = (CMD_C_Control*)pData;

	// �ж�ָ��
	if( m_pIClientControl == NULL )
		return true;

	// ���ú���
	m_pIClientControl->OnControlMessage( pControl->nMessageID, pControl->cbMessageInfo, pControl->nMessageSize );

	return true;
}



// ��ת����
LRESULT CGameClientEngine::OnMessageTurnOver( WPARAM wParam, LPARAM lParam )
{
	// �޸İ�ť
	m_GameClientView.m_ButtonStartGame.ShowWindow( true, dex::WindowShow_Size );
	m_GameClientView.m_ButtonStopGame.ShowWindow( false, dex::WindowShow_Size );

	//��ע��ť
	m_GameClientView.m_ButtonScoreLess.EnableWindow( true && !m_bPlayHosted );
	m_GameClientView.m_ButtonScoreAdd.EnableWindow( true && !m_bPlayHosted );
	m_GameClientView.m_ButtonScoreMax.EnableWindow( true && !m_bPlayHosted );

	// ɾ����ǰ����ʾ
	m_GameClientView.ClearMineAward();
	m_GameClientView.m_WindowLine.ClearWinLine();

	// ������ʾ
	m_GameClientView.m_WindowTurntable.LinePrompt();

	// ����Ӯ��Ϣ
	m_GameClientView.SetPlayWinScore( m_lPlayWinScore + m_nPlayTaskScore );
	m_GameClientView.SetPlayWinLine( m_bPlayWinLine );
	m_GameClientView.m_WindowLine.SetWinLine( m_bPlayWinLine );

	// ������ӵ�
	bool bShowEgg = false;

	// ��ʾ�н�
	if( m_nPlayMultipleIndex >= 0 && m_nPlayMultipleIndex < MultipleThree_Max + MultipleNine_Max )
	{
		// ����
		if( m_nPlayMultipleIndex >= MultipleThree_Max )
		{
			// ����н�
			m_GameClientView.AppendMineAward( m_nPlayMultipleIndex );

			// �л�����
			m_GameClientView.SetLineShow( MultipleType_Nine );
		}
		// ����3��
		else
		{
			// ����н�
			if ( m_bPlayWinLine[LINE_WIDTH_1] )
				m_GameClientView.AppendMineAward( CGameLogic::_Object()->ISThreeLineWidth1(m_nElementIndex) );
			if ( m_bPlayWinLine[LINE_WIDTH_2] )
				m_GameClientView.AppendMineAward( CGameLogic::_Object()->ISThreeLineWidth2(m_nElementIndex) );
			if ( m_bPlayWinLine[LINE_WIDTH_3] )
				m_GameClientView.AppendMineAward( CGameLogic::_Object()->ISThreeLineWidth3(m_nElementIndex) );
			if ( m_bPlayWinLine[LINE_HEIGHT_4] )
				m_GameClientView.AppendMineAward( CGameLogic::_Object()->ISThreeLineHeight4(m_nElementIndex) );
			if ( m_bPlayWinLine[LINE_HEIGHT_5] )
				m_GameClientView.AppendMineAward( CGameLogic::_Object()->ISThreeLineHeight5(m_nElementIndex) );
			if ( m_bPlayWinLine[LINE_HEIGHT_6] )
				m_GameClientView.AppendMineAward( CGameLogic::_Object()->ISThreeLineHeight6(m_nElementIndex) );
			if ( m_bPlayWinLine[LINE_OBLIQUE_7] )
				m_GameClientView.AppendMineAward( CGameLogic::_Object()->ISThreeLineOblique7(m_nElementIndex) );
			if ( m_bPlayWinLine[LINE_OBLIQUE_8] )
				m_GameClientView.AppendMineAward( CGameLogic::_Object()->ISThreeLineOblique8(m_nElementIndex) );

			// �л�����
			m_GameClientView.SetLineShow( MultipleType_Three );
		}

		// ��Ӽ���
		if ( CGameLogic::_Object()->DeformCount(m_nElementIndex) > 0 )
		{
			// ����λ��
			CPoint PointLine[LINE_INDEX_MAX][3] = 
			{
				CPoint( 0, 0 ), CPoint( 1, 0 ), CPoint( 2, 0 ),
				CPoint( 0, 1 ), CPoint( 1, 1 ), CPoint( 2, 1 ), 
				CPoint( 0, 2 ), CPoint( 1, 2 ), CPoint( 2, 2 ), 
				CPoint( 0, 0 ), CPoint( 0, 1 ), CPoint( 0, 2 ), 
				CPoint( 1, 0 ), CPoint( 1, 1 ), CPoint( 1, 2 ), 
				CPoint( 2, 0 ), CPoint( 2, 1 ), CPoint( 2, 2 ), 
				CPoint( 2, 0 ), CPoint( 1, 1 ), CPoint( 0, 2 ), 
				CPoint( 2, 2 ), CPoint( 1, 1 ), CPoint( 0, 0 ), 
			};

			// ��ӵ�����
			CWHArray< int > ArrayEggElement;

			// �������ж�
			for( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
			{
				// ��ȡ��ֵ
				int nLineElement[3];
				for( int nLineElemenIndex = 0; nLineElemenIndex < 3; ++nLineElemenIndex )
				{
					nLineElement[nLineElemenIndex] = m_nElementIndex[ PointLine[nIndex][nLineElemenIndex].x ][ PointLine[nIndex][nLineElemenIndex].y ];
				}

				// �ж���ֵ
				if ( CGameLogic::_Object()->ThreeLineDeform(nLineElement[0], nLineElement[1], nLineElement[2]) )
				{
					// ��ӱ���
					for( int nLineElemenIndex = 0; nLineElemenIndex < 3; ++nLineElemenIndex )
					{
						if( nLineElement[nLineElemenIndex] != m_nElementIndex[ PointLine[nIndex][nLineElemenIndex].x ][ PointLine[nIndex][nLineElemenIndex].y ] )
						{
							ArrayEggElement.Add( nLineElement[nLineElemenIndex] );
						}
					}
				}
			}

			// ��Ч��ʾ
			if( ArrayEggElement.GetCount() > 0 )
			{
				// ��������
				bShowEgg = true;
				m_GameClientView.m_WindowEgg.SetShowTime( 200 );
				m_GameClientView.m_WindowEgg.SetEggElement( ArrayEggElement );
				m_GameClientView.m_WindowEgg.ShowWindow( true, dex::WindowShow_Size );
			}
		}
	}

	// ��ʾ�н�
	for( int nAwardIndex = 0; nAwardIndex < m_ArrayAwardInfo.GetCount(); ++nAwardIndex )
	{
		// ��ȡ��Ϣ
		tagAwardInfo & TAwardInfo = m_ArrayAwardInfo.GetAt( nAwardIndex );

		// ��Ӷ���
		m_GameClientView.AppendAward( TAwardInfo.szPlayName, TAwardInfo.nMultipleIndex, TAwardInfo.nAwardScore );
	}

	// ɾ����Ϣ
	m_ArrayAwardInfo.RemoveAll();

	// ����·��
	m_GameClientView.m_WindowRoad.AppendRoad( m_nPlayMultipleIndex );

	// ������ʱ
	SetTimer( IDI_TIME_OPEN_WIN, bShowEgg ? 4000 : 2000, NULL );

	// ������Ч
	if ( m_lPlayWinScore > 0 || m_nPlayTaskScore > 0 )
	{
		CSound::_Object()->PlaySound( _T("GameWin.ogg") );
	}
	else
	{
		CSound::_Object()->PlaySound( _T("GameFail.ogg") );
	}

	return 0;
}


// ��ʼ��Ϣ
LRESULT CGameClientEngine::OnMessageStartGame( WPARAM wParam, LPARAM lParam )
{
	// ɾ����ʱ
	KillTimer( IDI_TIME_OPEN_WIN );
	KillTimer( IDI_TIME_CLOSE_WIN );

	// �������
	int nBetLineCount = 0;
	for ( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
		nBetLineCount += ( m_bBetLine[nIndex] ? 1 : 0 );

	// �û�ָ��
	IClientUserItem * pMeUserItem = GetMeUserItem();

	// ��Ч���
	if ( pMeUserItem == NULL )
	{
		// ��ʾ��Ϣ
		CGlobal::_Object()->MessageBox( _T("���ӷ�����ʧ�ܣ���������Ϸ��") );

		return 0;
	}

	// �жϽ��
	if ( pMeUserItem->GetUserScore() < m_nBetScore[m_nBetIndex] * nBetLineCount )
	{
		// ��ʾ��Ϣ
		CGlobal::_Object()->MessageBox( _T("��Ϸ�Ҳ��㣬���ֵ��") );

		return 0;
	}

	// �޸İ�ť
	m_GameClientView.m_ButtonStartGame.ShowWindow( false, dex::WindowShow_Size );
	m_GameClientView.m_ButtonStopGame.ShowWindow( true, dex::WindowShow_Size );

	// ��ע��ť
	m_GameClientView.m_ButtonScoreLess.EnableWindow( false );
	m_GameClientView.m_ButtonScoreAdd.EnableWindow( false );
	m_GameClientView.m_ButtonScoreMax.EnableWindow( false );

	// �رմ���
	m_GameClientView.m_WindowWin.ShowWindow( false, dex::WindowShow_Size );
	m_GameClientView.m_WindowEgg.ShowWindow( false, dex::WindowShow_Size );

	// ɾ����ǰ����ʾ
	m_GameClientView.ClearMineAward();
	m_GameClientView.m_WindowLine.ClearWinLine();

	// ��ʼ��ת
	m_GameClientView.m_WindowTurntable.TurnElement();

	// ����׼��
	SendUserReady( NULL, 0 );

	// ��ʼ��Ϸ
	m_bPlayGame = true;

	// Ӯ��Ϣ
	m_lPlayWinScore = 0l;
	m_nPlayTaskScore = 0;
	m_nPlayMultipleIndex = 0;
	m_bMagnification = false;
	ZeroMemory( m_bTaskComplete, sizeof(m_bTaskComplete) );
	ZeroMemory( m_bPlayWinLine, sizeof(m_bPlayWinLine) );
	ZeroMemory( m_nElementIndex, sizeof(m_nElementIndex) );

	// ������Ϣ
	CMD_C_Start CMDCStart;
	ZeroMemory( &CMDCStart, sizeof(CMD_C_Start) );

	// ������Ϣ
	CMDCStart.nBetScore = m_nBetScore[m_nBetIndex];
	CopyMemory( CMDCStart.nBetLine, m_bBetLine, sizeof(CMDCStart.nBetLine) );

	// ������Ϣ
	SendSocketData( SUB_C_START, &CMDCStart, sizeof(CMD_C_Start) );

	// ����Ӯ��Ϣ
	m_GameClientView.SetPlayWinScore( m_lPlayWinScore );
	m_GameClientView.SetPlayWinLine( m_bPlayWinLine );

	return 0;
}

// ֹͣ��Ϣ
LRESULT CGameClientEngine::OnMessageStopGame( WPARAM wParam, LPARAM lParam )
{
	// �ж��Ƿ��ֱ�ӽ���
	if( !m_GameClientView.m_WindowTurntable.ForcedStopElement() )
	{
		// ������Ϣ
		CMD_C_Stop CMDCStop;
		ZeroMemory( &CMDCStop, sizeof(CMD_C_Stop) );

		// ������Ϣ
		SendSocketData( SUB_C_STOP, &CMDCStop, sizeof(CMD_C_Stop) );
	}

	return 0;
}

// ��ʼ�й�
LRESULT CGameClientEngine::OnMessageStartHosted( WPARAM wParam, LPARAM lParam )
{
	// �޸İ�ť
	m_GameClientView.m_ButtonStartHosted.ShowWindow( false, dex::WindowShow_Size );
	m_GameClientView.m_ButtonStopHosted.ShowWindow( true, dex::WindowShow_Size );

	// ���ñ���
	m_bPlayHosted = true;
	m_GameClientView.m_WindowTurntable.SetSpeedUp(m_bPlayHosted);

	//��ע��ť
	m_GameClientView.m_ButtonScoreLess.EnableWindow( false );
	m_GameClientView.m_ButtonScoreAdd.EnableWindow( false );
	m_GameClientView.m_ButtonScoreMax.EnableWindow( false );

	// δ����Ϸ��
	if( !m_bPlayGame )
		OnMessageStartGame(0, 0);

	return 0;
}

// ֹͣ�й�
LRESULT CGameClientEngine::OnMessageStopHosted( WPARAM wParam, LPARAM lParam )
{
	// �޸İ�ť
	m_GameClientView.m_ButtonStartHosted.ShowWindow( true, dex::WindowShow_Size );
	m_GameClientView.m_ButtonStopHosted.ShowWindow( false, dex::WindowShow_Size );

	// ���ñ���
	m_bPlayHosted = false;
	m_GameClientView.m_WindowTurntable.SetSpeedUp(m_bPlayHosted);

	// �޸İ�ť
	if(m_GameClientView.m_ButtonStartGame.IsWindowVisible())
	{
		//��ע��ť
		m_GameClientView.m_ButtonScoreLess.EnableWindow( true );
		m_GameClientView.m_ButtonScoreAdd.EnableWindow( true );
		m_GameClientView.m_ButtonScoreMax.EnableWindow( true );
	}

	return 0;
}

// ��������
LRESULT CGameClientEngine::OnMessageScoreLess( WPARAM wParam, LPARAM lParam )
{
	// ��������
	if ( m_nBetIndex > 0  )
	{
		// ��ʱ����
		int nBetIndex = m_nBetIndex;
	
		// �л�����
		do 
		{
			nBetIndex = (nBetIndex + BET_INDEX_MAX - 1) % BET_INDEX_MAX;

		} while ( m_nBetScore[nBetIndex] == 0 && nBetIndex > 0 );

		// �ж���Ч
		if( m_nBetScore[nBetIndex] != 0 )
		{
			m_nBetIndex = nBetIndex;
		}
	}

	// ���ý���
	m_GameClientView.SetBetScore( m_nBetScore[m_nBetIndex] );

	return 0;
}

// ��������
LRESULT CGameClientEngine::OnMessageScoreAdd( WPARAM wParam, LPARAM lParam )
{
	// ��������
	if ( m_nBetIndex < BET_INDEX_MAX - 1 )
	{
		// ��ʱ����
		int nBetIndex = m_nBetIndex;

		// �л�����
		do 
		{
			nBetIndex = (nBetIndex + 1) % BET_INDEX_MAX;

		} while ( m_nBetScore[nBetIndex] == 0 && nBetIndex < BET_INDEX_MAX - 1  );

		// �ж���Ч
		if( m_nBetScore[nBetIndex] != 0 )
		{
			m_nBetIndex = nBetIndex;
		}
	}

	// ���ý���
	m_GameClientView.SetBetScore( m_nBetScore[m_nBetIndex] );

	return 0;
}

// �������
LRESULT CGameClientEngine::OnMessageScoreMax( WPARAM wParam, LPARAM lParam )
{
	// �������
	int nScoreMax = 0;
	for ( int nIndex = 0; nIndex < BET_INDEX_MAX; ++nIndex )
	{
		// �����ж�
		if ( m_nBetScore[nIndex] > nScoreMax )
		{
			// ������Ϣ
			m_nBetIndex = nIndex;
			nScoreMax = m_nBetScore[nIndex];
		}
	}

	// ���ý���
	m_GameClientView.SetBetScore( m_nBetScore[m_nBetIndex] );

	return 0;
}

// ����ѡ��
LRESULT CGameClientEngine::OnMessageLineCheck( WPARAM wParam, LPARAM lParam )
{
	// ��ȡ����
	bool bLineCheck = ( wParam != FALSE );
	int nLineIndex = static_cast<int>( lParam );

	// �ж���Ϣ
	ASSERT( nLineIndex < LINE_INDEX_MAX );

	// ��ǰ����
	int nBetLineCount = 0;
	for ( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
		nBetLineCount += ( m_bBetLine[nIndex] ? 1 : 0 );

	// Ψһ����������ȡ��
	if ( !bLineCheck && nBetLineCount == 1 && m_bBetLine[nLineIndex] )
		return 0;

	// ���ñ���
	m_bBetLine[nLineIndex] = bLineCheck;

	// ���ý���
	m_GameClientView.SetBetLine( m_bBetLine );
	m_GameClientView.m_ButtonLineCheck[nLineIndex].ShowWindow( bLineCheck, dex::WindowShow_Size );
	m_GameClientView.m_ButtonLineUnCheck[nLineIndex].ShowWindow( !bLineCheck, dex::WindowShow_Size );

	return 0;
}

// �򿪿���
LRESULT CGameClientEngine::OnMessageOpenControl( WPARAM wParam, LPARAM lParam )
{
	// ��ʾ
	if ( m_pIClientControl ) m_pIClientControl->ShowWindow( true );

	return 0;
}





