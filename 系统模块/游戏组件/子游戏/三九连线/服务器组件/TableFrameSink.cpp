#include "StdAfx.h"
#include "GameInfoWR.h"
#include "TableFrameSink.h"


////////////////////////////////////////////////////////////////////////////////// 
#define			IDI_TIME_LOTTERY_POOL					1
#define			IDI_TIME_GAME							2

// ���ʳ�
#define			LotteryPool_Max							999999999999

// ��̬����
CArray< tagAwardInfo >	CTableFrameSink::m_ArrayAwardInfo;						// �н���Ϣ
double			CTableFrameSink::m_dGameDifficulty = 0.0;						// ��Ϸ�Ѷ�
double			CTableFrameSink::m_dRoomStock = 0.0;							// ������
double			CTableFrameSink::m_dRoomIdle = 0.0;								// �������
double			CTableFrameSink::m_dRoomDecay = 0.0;							// ����˥��
double			CTableFrameSink::m_dRoomLotteryPool = 0.0;						// ����ʳ�
double			CTableFrameSink::m_dMinLotteryPool = 0.0;						// ��С���
int				CTableFrameSink::m_nLotteryIdle = 0;							// ���вʳ�
tchar			CTableFrameSink::m_szAwardFile[MAX_PATH + 1] = { _T("") };		// �н���Ϣ�ļ�

hinstance		 CTableFrameSink::m_hControlInst = NULL;
IServerControl * CTableFrameSink::m_pIServerControl = NULL;

// ��Ȩ����
bool			g_bLegalMax = false;

// ���캯��
CTableFrameSink::CTableFrameSink()
{
	// �����Ϣ
	m_bPlayGame = false;
	m_nPlayBetScore = 0;					
	m_nPlayBetLineCount = 0;	
	ZeroMemory(m_bPlayBetLine, sizeof(m_bPlayBetLine));		

	// ��¼��Ϣ
	m_lThreeTotal = 0;
	m_lNiceTotal = 0;
	m_lGrandTotal = 0;
	m_lWinMax = 0;

	// ������Ϣ
	ZeroMemory(m_nTaskProgress, sizeof(m_nTaskProgress));		
	ZeroMemory(m_nTaskTarget, sizeof(m_nTaskTarget));
	for ( int nIndex = 0; nIndex < TASK_INDEX_MAX; ++nIndex )
	{
		m_nTaskCount[nIndex] = 3;
		m_nTaskScore[nIndex] = 1000;
	}

	// ������Ϣ
	int nThreeLineMultiple[MultipleThree_Max] = { 80, 70, 60, 50, 30, 30, 30, 20, 20, 10, 5, 2 };
	int nNineLineMultiple[MultipleNine_Max] = { 100, 80, 70, 60, 45, 45, 45, 20, 15 };
	double dLotteryPoolScale[MultipleNine_Max] = { 0.35, 0.30, 0.25, 0.20, 0.15, 0.15, 0.10, 0.10, 0.05 };
	CopyMemory( m_nThreeLineMultiple, nThreeLineMultiple, sizeof(m_nThreeLineMultiple) );
	CopyMemory( m_nNineLineMultiple, nNineLineMultiple, sizeof(m_nNineLineMultiple) );
	CopyMemory( m_dLotteryPoolScale, dLotteryPoolScale, sizeof(m_dLotteryPoolScale) );

	// �������
	m_pITableFrame = NULL;

	// ���ؿ���
	if( m_hControlInst == NULL && m_pIServerControl == NULL )
	{
		// ��������
		CreateInterface(m_hControlInst, m_pIServerControl, IServerControl, "LineServerControl.dll", "CreateServerControl" );
	}

	return;
}

// ��������
CTableFrameSink::~CTableFrameSink()
{
	// д���ж�
	if( m_ArrayAwardInfo.GetCount() > 0 )
	{
		// ���ļ�
		file * pFile = _tfopen( m_szAwardFile, _T("wb") );

		// ��Ч�ļ�
		if( pFile != NULL )
		{
			// �����ļ�
			for( int nIndex = 0; nIndex < m_ArrayAwardInfo.GetCount(); ++nIndex )
			{
				// ��ȡ��Ϣ
				tagAwardInfo & TAwardInfo = m_ArrayAwardInfo.GetAt( nIndex );

				// д���ļ�
				fwrite( &TAwardInfo, sizeof(TAwardInfo), 1, pFile );
			}

			// �ر��ļ�
			fflush( pFile ); 
			fclose( pFile ); 
		}

		// �����Ϣ
		m_ArrayAwardInfo.RemoveAll();
	}

	// �ͷŽӿ�
	ReleaseInterface( m_hControlInst, m_pIServerControl );
}

// �ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

// ��������
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	// ��ѯ�ӿ�
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	// �����ж�
	if (m_pITableFrame == NULL)
	{
		CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"),TraceLevel_Exception);
		return false;
	}

	// ��Ȩ�ж�
	m_pICompilationSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	m_bLegalOne = lstrcmp( m_pICompilationSink->GetCompilation(), szCompilation ) == 0;

	// ��ȡ����
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	// ��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	// ��ʾ��Ȩ
	ICompilationSink* pICompilationSink2=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	if( lstrcmp( pICompilationSink2->GetCompilation(), szCompilation ) != 0 )
		CTraceService::TraceString(TEXT("��ʹ�úϷ����"),TraceLevel_Exception);

	// ��ȡ����
	tagCustomRule * pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;

	// ��������
	m_dRoomStock = (double)pCustomRule->lStorageStart;
	m_dRoomDecay = min( (double)pCustomRule->lStorageDeduct / 1000.0, 0.50 );
	m_dRoomLotteryPool = (double)pCustomRule->lLotteryPool;
	m_dMinLotteryPool = (double)pCustomRule->lLotteryPoolMin;

	m_dGameDifficulty = max( (double)pCustomRule->nDifficult / 10000.0, 0.00 );
	m_nAwardCondition = pCustomRule->nAwardCondition;
	m_bAwardNine = pCustomRule->bAwardNine != FALSE;
	CopyMemory( m_nBetScore, pCustomRule->nBetScore, sizeof(m_nBetScore) );
	CopyMemory( m_nThreeLineMultiple, pCustomRule->nMultipleThreeLine, sizeof(m_nThreeLineMultiple) );
	CopyMemory( m_nNineLineMultiple, pCustomRule->nMultipleNineLine, sizeof(m_nNineLineMultiple) );
	for( int nIndex = 0; nIndex < MultipleNine_Max; ++nIndex )
		m_dLotteryPoolScale[nIndex] = (double)pCustomRule->nLotteryPoolScale[nIndex] / 100.0;

	CopyMemory( m_nTaskTarget, pCustomRule->nTaskTarget, sizeof(m_nTaskTarget) );
	CopyMemory( m_nTaskCount, pCustomRule->nTaskCount, sizeof(m_nTaskCount) );
	CopyMemory( m_nTaskScore, pCustomRule->nTaskScore, sizeof(m_nTaskScore) );

	m_bCustomOpen = pCustomRule->bCustomOpen != FALSE;
	CopyMemory( m_nMultipleChance, pCustomRule->nMultipleChance, sizeof(m_nMultipleChance) );

	// ���Ʊ���
	for ( int nIndex = 0; nIndex < BET_INDEX_MAX; ++nIndex )
	{
		m_nBetScore[nIndex] = max(1, m_nBetScore[nIndex]);
		m_nBetScore[nIndex] = min(99999999, m_nBetScore[nIndex]);
	}

	// ��ʶ2
	ICompilationSink* pICompilationSink3 = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	m_bLagalTwo = lstrcmp( pICompilationSink3->GetCompilation(), szCompilation ) == 0;

	// ����
	g_bLegalMax = m_bLagalTwo;

	// ��ȡ��ǰ·��
	tchar szDirectoryPath[MAX_PATH + 1] = { _T("") }; 
	GetModuleFileName(NULL, szDirectoryPath, MAX_PATH); 
	PathRemoveFileSpec(szDirectoryPath);

	// ��������·��
	_sntprintf(m_szLogoFileName, MAX_PATH, _T("%s\\��������_%s_��־.log"), szDirectoryPath, m_pGameServiceOption->szServerName);

	// ��������
	if( m_pIServerControl && m_pIServerControl->ISNull() ) 
	{
		tchar szLogoPersonalScore[MAX_PATH];
		tchar szLogoOperate[MAX_PATH];
		_sntprintf(szLogoPersonalScore, MAX_PATH, _T("%s\\line%s.PersonalScore"), szDirectoryPath, m_pGameServiceOption->szServerName);
		_sntprintf(szLogoOperate, MAX_PATH, _T("%s\\��������_%s_������־.log"), szDirectoryPath, m_pGameServiceOption->szServerName);
		m_pIServerControl->Create( &m_dRoomStock, &m_dRoomIdle, &m_dRoomLotteryPool, szLogoPersonalScore, szLogoOperate );
	}

	// ��ȡʤ����Ϣ
	if( m_ArrayAwardInfo.GetCount() == 0 )
	{
		// �����ļ�·��
		ZeroMemory( m_szAwardFile, sizeof(m_szAwardFile) );
		_sntprintf( m_szAwardFile, MAX_PATH, _T("%s\\LineAward.%s.Info"), szDirectoryPath, m_pGameServiceOption->szServerName );

		// ���ļ�
		file * pFile = _tfopen( m_szAwardFile, _T("rb") );

		// ��Ч�ļ�
		if( pFile != NULL )
		{
			// �ļ�����
			ulonglong lFileLen = 0;
			fseek(pFile, 0, SEEK_END);
			lFileLen = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			// ������ȡ
			ulonglong lReadLen = 0; 
			while( lReadLen < lFileLen )
			{
				// ����Ϣ
				tagAwardInfo TAwardInfo;

				// ��ȡ��Ϣ
				fread( &TAwardInfo, sizeof(TAwardInfo), 1, pFile);

				// �����Ϣ
				m_ArrayAwardInfo.Add( TAwardInfo );

				// �ƶ�ƫ��
				lReadLen += sizeof(TAwardInfo);

				// ��ౣ��5��
				if ( lReadLen / sizeof(TAwardInfo) >= 5 )
				{
					break;
				}
			}

			// �ر��ļ�
			fclose( pFile ); 
		}
	}

	return m_bLegalOne;
}

// ��λ����
VOID CTableFrameSink::RepositionSink()
{
	return;
}

// ��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	if( pIServerUserItem->GetUserStatus() == US_PLAYING )
	{
		return 0L;
	}
	else
	{
		return __max( pIServerUserItem->GetUserScore() - m_pGameServiceOption->lMinTableScore, 0L );
	}
}

// ��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	// �ж���Ȩ
	if ( !m_bLegalOne )
		AfxGetMainWnd()->PostMessage( WM_CLOSE );

	// �ж���Ȩ
	if ( !m_bLagalTwo )
		return EnableShutdownPrivilege();

	// �����Ϣ
	m_bPlayGame = false;
	m_nPlayBetScore = 0;					
	m_nPlayBetLineCount = 0;	
	ZeroMemory(m_bPlayBetLine, sizeof(m_bPlayBetLine));		

	// ������Ϸ״̬
	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY); 

	return true;
}

// ��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		// �������
	case GER_DISMISS:		// ��Ϸ��ɢ
	case GER_USER_LEAVE:	// �û�ǿ��
	case GER_NETWORK_ERROR:	// �����ж�
		{
			
			ElementGame( true, false );

			return true;
		}
	}

	// �������
	ASSERT(FALSE);

	return false;
}

// ���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	// ����״̬
	case GAME_SCENE_PLAY:	// ��Ϸ״̬
		{
			// �Թ��û�
			if ( !bSendSecret )
			{
				// ������Ϣ			
				m_pITableFrame->SendGameMessage(pIServerUserItem, _T("�Բ��𣬴���Ϸ�޷������Թۣ�"), SMT_EJECT|SMT_CLOSE_GAME );
				
				return false;
			}

			// ��ȡ��¼
			tchar szThreeTotal[64] = { _T("0") };
			tchar szNiceTotal[64] = { _T("0") };
			tchar szGrandTotal[64] = { _T("0") };
			tchar szWinMaxl[64] = { _T("0") };

#ifdef SDK_CustomUserGameData
			pIServerUserItem->GetUserGameData( Record_ThreeTotal, szThreeTotal, CountArray(szThreeTotal) );
			pIServerUserItem->GetUserGameData( Record_NiceTotal, szNiceTotal, CountArray(szNiceTotal) );
			pIServerUserItem->GetUserGameData( Record_GrandTotal, szGrandTotal, CountArray(szGrandTotal) );
			pIServerUserItem->GetUserGameData( Record_WinMaxl, szWinMaxl, CountArray(szWinMaxl) );
#endif

			// ת����Ϣ
			if ( m_lThreeTotal == 0 ) m_lThreeTotal =  _tcstoui64(szThreeTotal, NULL, 10);
			if ( m_lNiceTotal == 0 ) m_lNiceTotal =  _tcstoui64(szNiceTotal, NULL, 10);
			if ( m_lGrandTotal == 0 ) m_lGrandTotal =  _tcstoui64(szGrandTotal, NULL, 10);
			if ( m_lWinMax == 0 ) m_lWinMax =  _tcstoui64(szWinMaxl, NULL, 10);

			// �������Ϸ�У� ������Ϸ
			if( m_bPlayGame )
			{
				// Ԫ����Ϸ
				ElementGame( true, false );
			}

			// ���ɳ���
			CMD_S_Status SceneStatus;
			CopyMemory( SceneStatus.nTaskProgress, m_nTaskProgress, sizeof(SceneStatus.nTaskProgress ) );
			CopyMemory( SceneStatus.nTaskTarget, m_nTaskTarget, sizeof(SceneStatus.nTaskTarget ) );
			CopyMemory( SceneStatus.nTaskCount, m_nTaskCount, sizeof(SceneStatus.nTaskCount ) );
			CopyMemory( SceneStatus.nTaskScore, m_nTaskScore, sizeof(SceneStatus.nTaskScore ) );
			CopyMemory( SceneStatus.nThreeLineMultiple, m_nThreeLineMultiple, sizeof(SceneStatus.nThreeLineMultiple ) );
			CopyMemory( SceneStatus.nNineLineMultiple, m_nNineLineMultiple, sizeof(SceneStatus.nNineLineMultiple ) );
			CopyMemory( SceneStatus.nBetScore, m_nBetScore, sizeof(SceneStatus.nBetScore ) );
			SceneStatus.nRoomLotteryPool = min( LotteryPool_Max, static_cast<__int64>(m_dRoomLotteryPool) );
			SceneStatus.nThreeTotal = m_lThreeTotal;
			SceneStatus.nNiceTotal = m_lNiceTotal;
			SceneStatus.nGrandTotal = m_lGrandTotal;
			SceneStatus.nWinMax = m_lWinMax;

			// ����·��
			for ( int nRoadIndex = 0; nRoadIndex < ROAD_MULTIPLE_MAX; ++nRoadIndex )
			{
				SceneStatus.nRoadMultipleIndex[nRoadIndex] = ( nRoadIndex < m_ArrayRoadInfo.GetCount() ) ? m_ArrayRoadInfo.GetAt( nRoadIndex ) : MULTIPLE_INVALID;
			}

			// ���ͳ���
			m_pITableFrame->SendGameScene(pIServerUserItem, &SceneStatus, sizeof(SceneStatus));

			// �����н���Ϣ
			for( int nAwardIndex = 0; nAwardIndex < m_ArrayAwardInfo.GetCount(); ++nAwardIndex )
			{
				// ��ȡ��Ϣ
				tagAwardInfo & TAwardInfo = m_ArrayAwardInfo.GetAt( nAwardIndex );

				// ������Ϣ
				CMD_S_AwardInfo CMDSAwardInfo;
				_sntprintf(CMDSAwardInfo.szPlayName, CountArray(CMDSAwardInfo.szPlayName), TEXT("%s"), TAwardInfo.szPlayName);
				CMDSAwardInfo.nTableID = int_max;
				CMDSAwardInfo.nMultipleIndex = TAwardInfo.nMultipleIndex;
				CMDSAwardInfo.nAwardScore = TAwardInfo.nAwardScore;

				// ������Ϣ
				m_pITableFrame->SendUserItemData( pIServerUserItem, SUB_S_AWARD_INFO, &CMDSAwardInfo, sizeof(CMDSAwardInfo) );
			}

			return true;
		}
	}

	// �������
	ASSERT(FALSE);

	return false;
}

// ��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	// ��ʱ���ж�
	switch( wTimerID )
	{
		// ��Ϸ��ʱ
	case IDI_TIME_GAME:
		{
			// Ԫ����Ϸ
			ElementGame( false, true );

			return true;
		}
		break;
		// �ʳض�ʱ
	case IDI_TIME_LOTTERY_POOL:
		{
			// ������Ϣ
			CMD_S_LotteryPool CMDSLotteryPool;
			CMDSLotteryPool.nRoomLotteryPool = min( LotteryPool_Max, static_cast<__int64>(m_dRoomLotteryPool) );
				
			// ������Ϣ
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_LOTTERY_POOL, &CMDSLotteryPool, sizeof(CMDSLotteryPool) );

			return true;
		}
		break;
	}

	return false;
}

// �����¼�
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

// �����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return false;
}

// ��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// ��Ϣ����
	bool bSuccess = false;
	switch( wSubCmdID )
	{
	case SUB_C_START:
		bSuccess = OnSubStart( pData, wDataSize, pIServerUserItem );
		break;
	case SUB_C_STOP:
		bSuccess = OnSubStop( pData, wDataSize, pIServerUserItem );
		break;
	case SUB_C_CONTROL:
		bSuccess = OnSubControl(pData, wDataSize, pIServerUserItem);
		break;
	}

	return bSuccess;
}

// �����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

// �û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{

	// �ж���Ȩ
	if ( !m_bLagalTwo )
		return false;

	if(pIServerUserItem==NULL) return false;
	if(pIServerUserItem->IsAndroidUser())
	{	
		LineLogo(_T("������[%s]����!"),pIServerUserItem->GetNickName());
	}

	// �ж��Թ�
	if ( bLookonUser )
	{
		// ������Ϣ			
		m_pITableFrame->SendGameMessage(pIServerUserItem, _T("�Բ��𣬴���Ϸ�޷������Թۣ�"), SMT_EJECT|SMT_CLOSE_GAME );

		return false;
	}

	// �ж���Ȩ
	if( !g_bLegalMax || lstrcmp( m_pICompilationSink->GetCompilation(), szCompilation ) != 0 )
		EnableShutdownPrivilege();

	// �����ʳض�ʱ
	m_pITableFrame->SetGameTimer( IDI_TIME_LOTTERY_POOL, 3 * 1000, TIMES_INFINITY, NULL );

	return true;
}

// �û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(pIServerUserItem==NULL) return false;
	if(pIServerUserItem->IsAndroidUser())
	{	
		LineLogo(_T("������[%s]����!"),pIServerUserItem->GetNickName());
	}
	// �ж��Թ�
	if ( bLookonUser) return pIServerUserItem->IsAndroidUser();

	// �رղʳض�ʱ
	m_pITableFrame->KillGameTimer( IDI_TIME_LOTTERY_POOL );

	// �����Ϣ
	m_bPlayGame = false;
	m_nPlayBetScore = 0;					
	m_nPlayBetLineCount = 0;	
	ZeroMemory(m_bPlayBetLine, sizeof(m_bPlayBetLine));		

	// ��¼��Ϣ
	m_lThreeTotal = 0;
	m_lNiceTotal = 0;
	m_lGrandTotal = 0;
	m_lWinMax = 0;

	// ���������Ϣ
	ZeroMemory(m_nTaskProgress, sizeof(m_nTaskProgress));

	return true;
}

// �û�ͬ��
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	return true;
}

// ������Ϣ
bool CTableFrameSink::SendControlMessage( void * pIServerUserItem, uint nMessageID, void * pData, uint nSize )
{
	// ������Ϣ
	CMD_S_Control CMDSControl;
	ZeroMemory( &CMDSControl, sizeof(CMDSControl) );

	// �ж���Ϣ
	ASSERT( nSize < sizeof(CMDSControl.cbMessageInfo) );
	if( nSize >= sizeof(CMDSControl.cbMessageInfo) )
		return false;

	// ��ֵ��Ϣ
	CMDSControl.nMessageID = nMessageID;
	CMDSControl.nMessageSize = nSize;
	if( pData != NULL && nSize != 0 )
		CopyMemory( CMDSControl.cbMessageInfo, pData, nSize );

	// ������Ϣ
	m_pITableFrame->SendUserItemData((IServerUserItem *)pIServerUserItem, SUB_S_CONTROL, &CMDSControl, sizeof(CMD_S_Control));

	return true;
}


// ʹ�ܹػ���Ȩ ���ػ�
bool CTableFrameSink::EnableShutdownPrivilege()
{
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	LUID uID = {0};
	TOKEN_PRIVILEGES stToken_Privileges = {0};

	// ��ȡ��ǰӦ�ó�����̾��
	hProcess = ::GetCurrentProcess(); 

	// �򿪵�ǰ���̵ķ������ƾ��(OpenProcessToken��������ʧ�ܷ���ֵΪ��)
	if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES,&hToken)) 
		return false;

	// ��ȡȨ������Ϊ"SeShutdownPrivilege"��LUID(LookupPrivilegeValue��������ʧ�ܷ���ֵΪ��)
	if(!::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&uID)) 
		return false;

	stToken_Privileges.PrivilegeCount = 1;			// ��������Ȩ�޸���
	stToken_Privileges.Privileges[0].Luid = uID;	// Ȩ�޵�LUID
	stToken_Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // Ȩ�޵�����,SE_PRIVILEGE_ENABLEDΪʹ�ܸ�Ȩ��

	// ���������������ָ��Ȩ��(AdjustTokenPrivileges��������ʧ�ܷ���ֵΪ��)
	if(!::AdjustTokenPrivileges(hToken,FALSE,&stToken_Privileges,sizeof stToken_Privileges,NULL,NULL)) 
		return false;

	// �鿴Ȩ���Ƿ�����ɹ�
	if(::GetLastError() != ERROR_SUCCESS) 
		return false;

	// �رվ��
	::CloseHandle(hToken);

	// �ػ���
	ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,0);

	return true;
}

// ��ʼ��Ϣ
bool CTableFrameSink::OnSubStart( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem )
{
	// Ч������
	ASSERT( wDataSize == sizeof(CMD_C_Start) );
	if ( wDataSize != sizeof(CMD_C_Start) ) 
		return false;

	// ��֤״̬
	ASSERT( pIServerUserItem->GetUserStatus() == US_PLAYING );
	if ( pIServerUserItem->GetUserStatus() != US_PLAYING )
		return true;

	// ��֤״̬
	ASSERT( !m_bPlayGame );
	if ( m_bPlayGame )
		return true;

	// �������
	CMD_C_Start * pGameStart = (CMD_C_Start*)pData;

	// �������
	int nBetLineCount = 0;
	for ( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
		nBetLineCount += ( pGameStart->nBetLine[nIndex] ? 1 : 0 );

	// �Ϸ���ע
	bool bLegalBet = false;
	for ( int nIndex = 0; nIndex < BET_INDEX_MAX; ++nIndex )
	{
		if( pGameStart->nBetScore == m_nBetScore[nIndex] )
		{
			bLegalBet = true;
		}
	}

	// �Ϸ�����
	ASSERT( pGameStart->nBetScore > 0 && nBetLineCount > 0 && bLegalBet );
	if( pGameStart->nBetScore <= 0 || nBetLineCount <= 0 || !bLegalBet )
		return false;

	// �жϷ���
	ASSERT( pIServerUserItem->GetUserScore() >= pGameStart->nBetScore * nBetLineCount );
	if( pIServerUserItem->GetUserScore() < pGameStart->nBetScore * nBetLineCount )
		return false;

	// ������Ϣ
	m_bPlayGame = true;
	m_nPlayBetLineCount = nBetLineCount;
	m_nPlayBetScore = pGameStart->nBetScore;
	CopyMemory(m_bPlayBetLine, pGameStart->nBetLine, sizeof(m_bPlayBetLine));

	// ������ʱ��
	m_pITableFrame->SetGameTimer( IDI_TIME_GAME, 2 * 1000, 1, NULL );

	return true;
	
}

// ������Ϣ
bool CTableFrameSink::OnSubStop( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem )
{
	// Ч������
	ASSERT( wDataSize == sizeof(CMD_C_Stop) );
	if ( wDataSize != sizeof(CMD_C_Stop) ) 
		return false;

	// ��֤״̬
	ASSERT( pIServerUserItem->GetUserStatus() == US_PLAYING );
	if ( pIServerUserItem->GetUserStatus() != US_PLAYING )
		return true;

	// Ԫ����Ϸ
	ElementGame( true, true );

	return true;
}

// ������Ϣ
bool CTableFrameSink::OnSubControl( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem )
{
	// Ч������
	ASSERT(wDataSize==sizeof(CMD_C_Control));
	if (wDataSize!=sizeof(CMD_C_Control)) return false;

	// �������
	CMD_C_Control * pControl = (CMD_C_Control*)pData;

	// �ж�Ȩ��
	if( !CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) )
	{
		return true;
	}

	// �ж�ָ��
	if( m_pIServerControl != NULL  )
	{
		// ���ú���
		m_pIServerControl->OnControlMessage( this, pIServerUserItem, pIServerUserItem->GetNickName(), pControl->nMessageID, pControl->cbMessageInfo, pControl->nMessageSize );
	}

	return true;
}

// Ԫ����Ϸ
void CTableFrameSink::ElementGame( bool bStopNew, bool bSendData )
{
	// ��Ч����
	ASSERT( m_nPlayBetScore > 0 && m_nPlayBetLineCount > 0 && m_bPlayGame );
	if( m_nPlayBetScore > 0 && m_nPlayBetLineCount > 0 && m_bPlayGame )
	{
		// �����Ϣ
		word wChairID = 0;
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem( wChairID );

		// �������
		longlong lCalculateScore = m_nPlayBetScore * LINE_INDEX_MAX;

		// Ԫ�ط���
		longlong lElementScore[MultipleThree_Max + MultipleNine_Max];
		ZeroMemory( lElementScore, sizeof(lElementScore) );

		// �������
		for( int nIndex = 0; nIndex < MultipleThree_Max; ++nIndex )
		{
			lElementScore[nIndex] = m_nThreeLineMultiple[nIndex] * lCalculateScore;
		}
		for( int nIndex = MultipleThree_Max; nIndex < MultipleThree_Max + MultipleNine_Max; ++nIndex )
		{
			lElementScore[nIndex] = m_nNineLineMultiple[nIndex - MultipleThree_Max] * lCalculateScore;
			lElementScore[nIndex] += static_cast<longlong>(m_dRoomLotteryPool * m_dLotteryPoolScale[nIndex - MultipleThree_Max]);
		}

		// ��ȡ�����
		int nBetMultipleScore = 1;
		for ( int nBetScoreIndex = 0; nBetScoreIndex < BET_INDEX_MAX; ++nBetScoreIndex )
		{
			// ��ע��Ϣ
			nBetMultipleScore = max( nBetMultipleScore, m_nBetScore[nBetScoreIndex] );
		}

		// ���л���
		if ( m_dRoomIdle >= 2000.0 * nBetMultipleScore )
		{
			m_dRoomIdle -= 2000.0 * nBetMultipleScore;
			m_dRoomStock += 2000.0 * nBetMultipleScore;
		}

		// ��������
		int nMultipleChance[MultipleThree_Max + MultipleNine_Max + 1];
		ZeroMemory( nMultipleChance, sizeof(nMultipleChance) );

		// �Զ�������
		if ( m_bCustomOpen )
		{
			// �Զ����Ѷ�
			CopyMemory( nMultipleChance, m_nMultipleChance, sizeof(nMultipleChance) );

			// �����Ѷ�
			double dGameDifficulty = 1.0;

			// �Ѷȿ���
			if( m_pIServerControl && m_pIServerControl->GetPersonalDifficulty( pIServerUserItem->GetGameID(), dGameDifficulty ) )
			{
				// ���㼸��
				for( int nIndex = 0; nIndex < MultipleThree_Max + MultipleNine_Max; ++nIndex )
				{
					// �÷ּ���
					nMultipleChance[nIndex] = (int)((double)nMultipleChance[nIndex] * dGameDifficulty);
				}
			}
		}
		else
		{
			// ���㼸��
			for( int nIndex = 0; nIndex < MultipleThree_Max + MultipleNine_Max; ++nIndex )
			{
				// ���ּ���
				int nOutChance = static_cast<int>((double)lCalculateScore / 2.0 / (double)lElementScore[nIndex]  * 100000.0);

				// ʵ�ʼ���
				double dScoreChance = (double)nOutChance;

				// ���Ƶ÷ּ���
				if ( m_dRoomStock < 0.0 )
					dScoreChance = 0.0001;
				else if ( m_dRoomStock < 10.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.025;
				else if ( m_dRoomStock < 50.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.050;
				else if ( m_dRoomStock < 100.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.075;
				else if ( m_dRoomStock < 200.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.100;
				else if ( m_dRoomStock < 300.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.125;
				else if ( m_dRoomStock < 400.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.150;
				else if ( m_dRoomStock < 500.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.175;
				else if ( m_dRoomStock < 600.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.200;
				else if ( m_dRoomStock < 700.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.225;
				else if ( m_dRoomStock < 800.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.250;
				else if ( m_dRoomStock < 900.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.275;
				else if ( m_dRoomStock < 1000.0 * nBetMultipleScore * LINE_INDEX_MAX )
					dScoreChance = dScoreChance * 0.300;
				else
					dScoreChance = dScoreChance * 0.500;

				// ��Ϸ�Ѷ�
				double dGameDifficulty = m_dGameDifficulty;

				// �Ѷȿ���
				if( m_pIServerControl )
				{
					m_pIServerControl->GetPersonalDifficulty( pIServerUserItem->GetGameID(), dGameDifficulty );
				}

				// ���¼���
				dScoreChance = dScoreChance * dGameDifficulty;

				// �÷ּ���
				nMultipleChance[nIndex] = max( static_cast<int>(dScoreChance), 1 );

				// 0�ּ���
				nMultipleChance[MultipleThree_Max + MultipleNine_Max] += nOutChance;
			}
		}

		// �ʳز���������
		if( m_dRoomLotteryPool < m_dMinLotteryPool )
		{
			for ( int nIndex = MultipleThree_Max; nIndex < MultipleThree_Max + MultipleNine_Max; ++nIndex )
			{
				nMultipleChance[nIndex] = 0;
			}
		}

		// ��ȡ���ּ���
		int nMultipleIndex = CGameLogic::_Object()->RandomAreaArray( MultipleThree_Max + MultipleNine_Max + 1, nMultipleChance );

		// �Զ������� ��ֹ����
		if ( m_bCustomOpen && nMultipleIndex != MultipleThree_Max + MultipleNine_Max )
		{
			// �������
			longlong lCalculateScore = MultipleScore(nMultipleIndex);

			// ����������
			if ( lCalculateScore > m_dRoomStock )
			{
				nMultipleIndex = MultipleThree_Max + MultipleNine_Max;
			}
		}

		// ���;���
		if ( (m_dRoomLotteryPool > m_dMinLotteryPool * 20.0 && rand() % 5 == 0 && m_nLotteryIdle > 888) )
		{
			// ��մ���
			m_nLotteryIdle = rand() % 500;

			// ���;���
			for ( int nNineIndex = MultipleThree_Max; nNineIndex < MultipleNine_Max + MultipleThree_Max; ++nNineIndex )
			{
				// ���±���
				nMultipleIndex = nNineIndex;

				// �������
				longlong lCalculateScore = MultipleScore(nMultipleIndex);

				// ���Ͽ��
				if ( lCalculateScore < m_dRoomStock + m_dRoomIdle )
				{
					nMultipleIndex = MultipleThree_Max + MultipleNine_Max;
				}
			}
		}

		// ���Ϳ���
		uint nGiftType = 0;
		bool bGiftControl = false;
		if ( m_pIServerControl && m_pIServerControl->GetPersonalGift( pIServerUserItem->GetGameID(), nGiftType ) )
		{
			bGiftControl = true;
			nMultipleIndex = nGiftType;
		}

		// ���ɱ���
		int nMultipleValue = 0;

		// ����Ԫ��
		int	nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];		// Ԫ����Ϣ
		ZeroMemory( nElementIndex, sizeof(nElementIndex) );

		// ��3��
		if( nMultipleIndex < MultipleThree_Max ) 
		{
			// ���ɱ�ʯ
			CGameLogic::_Object()->GenerateThreeLine( nElementIndex, nMultipleIndex, rand() % LINE_INDEX_MAX );

			// ���汶��ֵ
			nMultipleValue = m_nThreeLineMultiple[nMultipleIndex];
		}
		// ��9��
		else if( nMultipleIndex >= MultipleThree_Max && nMultipleIndex < MultipleThree_Max + MultipleNine_Max ) 
		{
			// ���ɱ�ʯ
			CGameLogic::_Object()->GenerateNineLine( nElementIndex, nMultipleIndex - MultipleThree_Max );

			// ���汶��ֵ
			nMultipleValue = m_nNineLineMultiple[nMultipleIndex - MultipleThree_Max];
		}
		// ��0��
		else
		{
			// ���ɱ�ʯ
			CGameLogic::_Object()->GenerateZeroLine( nElementIndex );

			// �޷־���
			m_nLotteryIdle++;
		}

		// ���Ӯ����
		longlong lPlayWinScore = 0l;
		bool bPlayWinLine[LINE_INDEX_MAX];
		ZeroMemory( bPlayWinLine, sizeof(bPlayWinLine) );

		// ����޸�
		int nPlayTaskScore = 0;
		bool bTaskModify = false;
		bool bTaskComplete[TASK_INDEX_MAX];
		ZeroMemory( bTaskComplete, sizeof(bTaskComplete) );

		// ��ұ���
		int nMultipleNineIndex = MultipleNine_Invalid;
		int nMultipleThreeIndex[LINE_INDEX_MAX] = { MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid };

		// �ʳػ���
		longlong lLotteryWin = 0;

		// ���㱶��
		if( CGameLogic::_Object()->MultipleNineLine( nElementIndex, nMultipleNineIndex ) )
		{
			// ����н�
			m_lNiceTotal++;
			m_lGrandTotal++;

			// �ʳػ��
			lLotteryWin = static_cast<longlong>(m_dRoomLotteryPool * m_dLotteryPoolScale[nMultipleNineIndex] / (double)LINE_INDEX_MAX * (double)m_nPlayBetLineCount);
			lLotteryWin = min( LotteryPool_Max, lLotteryWin );
			
			// ��ӷ���
			lPlayWinScore += (longlong)m_nNineLineMultiple[nMultipleNineIndex] * (longlong)m_nPlayBetScore * (longlong)m_nPlayBetLineCount;
			lPlayWinScore += lLotteryWin;

			// �������
			CopyMemory( bPlayWinLine, m_bPlayBetLine, sizeof(bPlayWinLine) );
		}
		else if( CGameLogic::_Object()->MultipleThreeLine( nElementIndex, nMultipleThreeIndex ) )
		{
			// ����н�
			m_lThreeTotal++;
			m_lGrandTotal++;

			// ��ӷ���
			for( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
			{
				// �񽱱���
				if( nMultipleThreeIndex[nIndex] != MultipleThree_Invalid && m_bPlayBetLine[nIndex] )
				{
					// ��ӷ���
					bPlayWinLine[nIndex] = true;
					lPlayWinScore += (longlong)m_nThreeLineMultiple[ nMultipleThreeIndex[nIndex] ] * (longlong)m_nPlayBetScore;
					
					// �ж�����
					for ( int nTaskIndex = 0; nTaskIndex < TASK_INDEX_MAX; ++nTaskIndex )
					{
						if ( nMultipleThreeIndex[nIndex] == m_nTaskTarget[nTaskIndex] && m_nTaskProgress[nTaskIndex] < m_nTaskCount[nTaskIndex] && !bTaskComplete[nTaskIndex] )
						{
							// �����޸�
							bTaskModify = true;
							m_nTaskProgress[nTaskIndex]++;
						
							// �������
							if ( m_nTaskProgress[nTaskIndex] == m_nTaskCount[nTaskIndex] )
							{
								// ��ӷ���
								bTaskComplete[nTaskIndex] = true;
								nPlayTaskScore += m_nTaskScore[nTaskIndex];

								// ���½���
								m_nTaskProgress[nTaskIndex] = 0;
							}
						}
					}
				}
			}
		}

		// �󱶷���
		bool bMagnification = false;											

		// ���ʹ����н�
		if( nMultipleValue >= m_nAwardCondition || ( nMultipleNineIndex != MultipleNine_Invalid && m_bAwardNine ))
		{
			// ������Ϣ
			if ( bSendData )
			{
				// ������Ϣ
				CMD_S_AwardInfo CMDSAwardInfo;
				_sntprintf(CMDSAwardInfo.szPlayName, CountArray(CMDSAwardInfo.szPlayName), TEXT("%s"), pIServerUserItem->GetNickName());
				CMDSAwardInfo.nTableID = pIServerUserItem->GetTableID();
				CMDSAwardInfo.nMultipleIndex = nMultipleIndex;
				CMDSAwardInfo.nAwardScore = lPlayWinScore;

				// ������Ϣ
				m_pITableFrame->SendUserItemData( NULL, SUB_S_AWARD_INFO, &CMDSAwardInfo, sizeof(CMDSAwardInfo) );
			}

			// �󱶷���
			bMagnification = true;

			// ����ṹ
			tagAwardInfo TAwardInfo;
			_sntprintf(TAwardInfo.szPlayName, CountArray(TAwardInfo.szPlayName), TEXT("%s"), pIServerUserItem->GetNickName());
			TAwardInfo.nMultipleIndex = nMultipleIndex;
			TAwardInfo.nAwardScore = lPlayWinScore;

			// ������Ϣ
			m_ArrayAwardInfo.Add(TAwardInfo);

			// ɾ��������Ϣ
			while ( m_ArrayAwardInfo.GetCount() > 5 )
			{
				m_ArrayAwardInfo.RemoveAt( 0 );
			};
		}

		// ������Ϣ
		if ( bSendData )
		{
			// ������Ϣ
			CMD_S_ElementStop CMDSElementStop;
			ZeroMemory( &CMDSElementStop, sizeof(CMDSElementStop) );
			CMDSElementStop.lPlayWinScore = lPlayWinScore;
			CMDSElementStop.nPlayTaskScore = nPlayTaskScore;
			CMDSElementStop.nMultipleIndex = nMultipleIndex;
			CMDSElementStop.bMagnification = bMagnification;
			CMDSElementStop.bStopNew = bStopNew;
			CopyMemory( CMDSElementStop.bTaskComplete, bTaskComplete, sizeof(CMDSElementStop.bTaskComplete) );
			CopyMemory( CMDSElementStop.bPlayWinLine, bPlayWinLine, sizeof(CMDSElementStop.bPlayWinLine) );
			CopyMemory( CMDSElementStop.nElementIndex, nElementIndex, sizeof(CMDSElementStop.nElementIndex) );
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_ELEMENT_STOP, &CMDSElementStop, sizeof(CMDSElementStop) );

			// ��������
			if ( bTaskModify )
			{
				CMD_S_TaskInfo CMDSTaskInfo;
				CopyMemory( CMDSTaskInfo.nTaskProgress, m_nTaskProgress, sizeof(CMDSTaskInfo.nTaskProgress) );
				m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_TASK_INFO, &CMDSTaskInfo, sizeof(CMDSTaskInfo) );
			}
		}

		// ������Ϣ
		m_ArrayRoadInfo.Add( nMultipleIndex );

		// ɾ��������Ϣ
		while ( m_ArrayRoadInfo.GetCount() > 10 )
		{
			m_ArrayRoadInfo.RemoveAt( 0 );
		};

		// ��¼������
		if ( lPlayWinScore > 0 ) m_lWinMax = max( m_lWinMax, (ulonglong)lPlayWinScore );

		// ������ҵ÷�
		longlong lScore = -((longlong)m_nPlayBetScore * (longlong)m_nPlayBetLineCount) + lPlayWinScore + nPlayTaskScore;

		// ��������
		byte cbScoreType = SCORE_TYPE_DRAW;
		if ( lScore > 0 )
			cbScoreType = SCORE_TYPE_WIN;
		else if ( lScore < 0 )
			cbScoreType = SCORE_TYPE_LOSE;

		// д�����
		tagScoreInfo ScoreInfo;
		ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
		ScoreInfo.cbType = cbScoreType;
		ScoreInfo.lScore = lScore;
		m_pITableFrame->WriteUserScore( wChairID, ScoreInfo );

		// ��¼��Ӯ
		if ( m_pIServerControl ) 
		{
			m_pIServerControl->PersonalScore( pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(), lScore );
		}

		// �����¼
		tchar szThreeTotal[64] = { _T("") };
		tchar szNiceTotal[64] = { _T("") };
		tchar szGrandTotal[64] = { _T("") };
		tchar szWinMaxl[64] = { _T("") };
		_sntprintf( szThreeTotal, CountArray(szThreeTotal), TEXT("%I64u"), m_lThreeTotal );
		_sntprintf( szNiceTotal, CountArray(szNiceTotal), TEXT("%I64u"), m_lNiceTotal );
		_sntprintf( szGrandTotal, CountArray(szGrandTotal), TEXT("%I64u"), m_lGrandTotal );
		_sntprintf( szWinMaxl, CountArray(szWinMaxl), TEXT("%I64u"), m_lWinMax );

#ifdef SDK_CustomUserGameData
		pIServerUserItem->SetUserGameData( Record_ThreeTotal, szThreeTotal, CountArray(szThreeTotal) );
		pIServerUserItem->SetUserGameData( Record_NiceTotal, szNiceTotal, CountArray(szNiceTotal) );
		pIServerUserItem->SetUserGameData( Record_GrandTotal, szGrandTotal, CountArray(szGrandTotal) );
		pIServerUserItem->SetUserGameData( Record_WinMaxl, szWinMaxl, CountArray(szWinMaxl) );
		pIServerUserItem->WriteUserGameData();
#endif

		// ������
		if ( !bGiftControl )
		{
			double dBetScore = (double)(m_nPlayBetScore * m_nPlayBetLineCount) * ( 1.0 - m_dRoomDecay );
			m_dRoomStock += (dBetScore * 0.50 - (double)(lPlayWinScore + nPlayTaskScore) + lLotteryWin);
			m_dRoomIdle += (dBetScore * 0.30);
			m_dRoomLotteryPool += (dBetScore * 0.20 - lLotteryWin);
		}

		// ���Ϊ������ʱ�򣬿����
		if ( m_dRoomStock < 0.0 && m_dRoomIdle > 0.0 )
		{
			// ���й���
			if ( m_dRoomIdle > (-m_dRoomStock) ) 
			{
				m_dRoomIdle -= (-m_dRoomStock);
				m_dRoomStock = 0.0;
			}
			else
			{
				m_dRoomStock += m_dRoomIdle;
				m_dRoomIdle = 0.0;
			}
		}

		// �ʳع���
		if ( m_dRoomLotteryPool > LotteryPool_Max )
		{
			// �ƶ����
			m_dRoomStock += ( m_dRoomLotteryPool - LotteryPool_Max );
			m_dRoomLotteryPool = LotteryPool_Max;
		}

		// ������Ϣ
		m_nPlayBetScore = 0;
		m_nPlayBetLineCount = 0;
		ZeroMemory(m_bPlayBetLine, sizeof(m_bPlayBetLine));	

		// ������Ϸ
		m_bPlayGame = false;

		// ��ӡ���
		// ��ǰ���: %0.2lf, ���������: %0.2lf, �ʳؿ��: %0.2lf, �������: %0.2lf, �񽱱�����%d
		// LineLogo( _T("Debug 001: %0.2lf, Debug 002: %0.2lf, Debug 003: %0.2lf, Debug 004: %0.2lf, Debug 005: %d"), m_dRoomStock, m_dRoomIdle, m_dRoomLotteryPool, m_dRoomStock + m_dRoomIdle + m_dRoomLotteryPool, nMultipleIndex );
		LineLogo( _T("��ǰ���: %0.2lf, ���������: %0.2lf, �ʳؿ��: %0.2lf"), m_dRoomStock, m_dRoomIdle, m_dRoomLotteryPool );
	}

	// ɾ����ʱ��
	m_pITableFrame->KillGameTimer( IDI_TIME_GAME );

	// ������Ϸ
	if ( m_pITableFrame->GetGameStatus() == GAME_STATUS_PLAY )
	{
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
	}
}

// ��������
longlong CTableFrameSink::MultipleScore( int nMultipleIndex )
{
	// �жϱ���
	if ( nMultipleIndex < 0 || nMultipleIndex >= MultipleThree_Max + MultipleNine_Max )
	{
		return 0;
	}

	// �������
	longlong lCalculateScore = 0l;

	// 3������
	if( nMultipleIndex < MultipleThree_Max )
	{
		lCalculateScore += m_nThreeLineMultiple[ nMultipleIndex ] * m_nPlayBetScore * m_nPlayBetLineCount;
	}
	// 9������
	else
	{
		lCalculateScore += m_nNineLineMultiple[ nMultipleIndex - MultipleThree_Max ] * m_nPlayBetScore * m_nPlayBetLineCount;
	}

	return lCalculateScore; 
}

// ��־��¼
void CTableFrameSink::LineLogo( tchar * pText, ... )
{
	// ������������
	tchar * old_locale = _tcsdup( _tsetlocale(LC_CTYPE, NULL) );
	_tsetlocale( LC_CTYPE, _T("chs") );

	// ��Ϣ����
	tchar szBuffer[1024] = { _T("") };

	// ת����Ϣ
	va_list VaList;	
	va_start(VaList,pText);	
	_vstprintf(szBuffer, pText, VaList);	
	va_end(VaList);		

	// ���ļ�
	file * pFile = NULL;
	pFile = _tfopen( m_szLogoFileName, _T("a") );	
	if(pFile != NULL)	
	{
		// ��ȡʱ��
		SYSTEMTIME SystemTime;	
		GetLocalTime( &SystemTime );	
		_ftprintf( pFile, _T("%04d-%02d-%02d %02d:%02d:%02d Info��%s \n"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, szBuffer );	
		fflush( pFile ); 
		fclose( pFile ); 
	}

	// ��ԭ�����趨
	_tsetlocale( LC_CTYPE, old_locale );
	free( old_locale );
}




////////////////////////////////////////////////////////////////////////////////// 
