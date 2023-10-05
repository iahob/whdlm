#include "StdAfx.h"
#include "GameInfoWR.h"
#include "TableFrameSink.h"


////////////////////////////////////////////////////////////////////////////////// 
#define			IDI_TIME_LOTTERY_POOL					1
#define			IDI_TIME_GAME							2

// 最大彩池
#define			LotteryPool_Max							999999999999

// 静态定义
CArray< tagAwardInfo >	CTableFrameSink::m_ArrayAwardInfo;						// 中奖信息
double			CTableFrameSink::m_dGameDifficulty = 0.0;						// 游戏难度
double			CTableFrameSink::m_dRoomStock = 0.0;							// 房间库存
double			CTableFrameSink::m_dRoomIdle = 0.0;								// 房间空闲
double			CTableFrameSink::m_dRoomDecay = 0.0;							// 房间衰减
double			CTableFrameSink::m_dRoomLotteryPool = 0.0;						// 房间彩池
double			CTableFrameSink::m_dMinLotteryPool = 0.0;						// 最小库存
int				CTableFrameSink::m_nLotteryIdle = 0;							// 空闲彩池
tchar			CTableFrameSink::m_szAwardFile[MAX_PATH + 1] = { _T("") };		// 中奖信息文件

hinstance		 CTableFrameSink::m_hControlInst = NULL;
IServerControl * CTableFrameSink::m_pIServerControl = NULL;

// 授权定义
bool			g_bLegalMax = false;

// 构造函数
CTableFrameSink::CTableFrameSink()
{
	// 玩家信息
	m_bPlayGame = false;
	m_nPlayBetScore = 0;					
	m_nPlayBetLineCount = 0;	
	ZeroMemory(m_bPlayBetLine, sizeof(m_bPlayBetLine));		

	// 记录信息
	m_lThreeTotal = 0;
	m_lNiceTotal = 0;
	m_lGrandTotal = 0;
	m_lWinMax = 0;

	// 任务信息
	ZeroMemory(m_nTaskProgress, sizeof(m_nTaskProgress));		
	ZeroMemory(m_nTaskTarget, sizeof(m_nTaskTarget));
	for ( int nIndex = 0; nIndex < TASK_INDEX_MAX; ++nIndex )
	{
		m_nTaskCount[nIndex] = 3;
		m_nTaskScore[nIndex] = 1000;
	}

	// 奖励信息
	int nThreeLineMultiple[MultipleThree_Max] = { 80, 70, 60, 50, 30, 30, 30, 20, 20, 10, 5, 2 };
	int nNineLineMultiple[MultipleNine_Max] = { 100, 80, 70, 60, 45, 45, 45, 20, 15 };
	double dLotteryPoolScale[MultipleNine_Max] = { 0.35, 0.30, 0.25, 0.20, 0.15, 0.15, 0.10, 0.10, 0.05 };
	CopyMemory( m_nThreeLineMultiple, nThreeLineMultiple, sizeof(m_nThreeLineMultiple) );
	CopyMemory( m_nNineLineMultiple, nNineLineMultiple, sizeof(m_nNineLineMultiple) );
	CopyMemory( m_dLotteryPoolScale, dLotteryPoolScale, sizeof(m_dLotteryPoolScale) );

	// 组件变量
	m_pITableFrame = NULL;

	// 加载控制
	if( m_hControlInst == NULL && m_pIServerControl == NULL )
	{
		// 创建控制
		CreateInterface(m_hControlInst, m_pIServerControl, IServerControl, "LineServerControl.dll", "CreateServerControl" );
	}

	return;
}

// 析构函数
CTableFrameSink::~CTableFrameSink()
{
	// 写入判断
	if( m_ArrayAwardInfo.GetCount() > 0 )
	{
		// 打开文件
		file * pFile = _tfopen( m_szAwardFile, _T("wb") );

		// 有效文件
		if( pFile != NULL )
		{
			// 保存文件
			for( int nIndex = 0; nIndex < m_ArrayAwardInfo.GetCount(); ++nIndex )
			{
				// 获取信息
				tagAwardInfo & TAwardInfo = m_ArrayAwardInfo.GetAt( nIndex );

				// 写入文件
				fwrite( &TAwardInfo, sizeof(TAwardInfo), 1, pFile );
			}

			// 关闭文件
			fflush( pFile ); 
			fclose( pFile ); 
		}

		// 清空信息
		m_ArrayAwardInfo.RemoveAll();
	}

	// 释放接口
	ReleaseInterface( m_hControlInst, m_pIServerControl );
}

// 接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

// 配置桌子
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	// 查询接口
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	// 错误判断
	if (m_pITableFrame == NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"),TraceLevel_Exception);
		return false;
	}

	// 授权判断
	m_pICompilationSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	m_bLegalOne = lstrcmp( m_pICompilationSink->GetCompilation(), szCompilation ) == 0;

	// 获取参数
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	// 开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	// 提示授权
	ICompilationSink* pICompilationSink2=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	if( lstrcmp( pICompilationSink2->GetCompilation(), szCompilation ) != 0 )
		CTraceService::TraceString(TEXT("请使用合法组件"),TraceLevel_Exception);

	// 获取规则
	tagCustomRule * pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;

	// 设置配置
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

	// 限制倍数
	for ( int nIndex = 0; nIndex < BET_INDEX_MAX; ++nIndex )
	{
		m_nBetScore[nIndex] = max(1, m_nBetScore[nIndex]);
		m_nBetScore[nIndex] = min(99999999, m_nBetScore[nIndex]);
	}

	// 标识2
	ICompilationSink* pICompilationSink3 = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	m_bLagalTwo = lstrcmp( pICompilationSink3->GetCompilation(), szCompilation ) == 0;

	// 设置
	g_bLegalMax = m_bLagalTwo;

	// 获取当前路径
	tchar szDirectoryPath[MAX_PATH + 1] = { _T("") }; 
	GetModuleFileName(NULL, szDirectoryPath, MAX_PATH); 
	PathRemoveFileSpec(szDirectoryPath);

	// 生成配置路径
	_sntprintf(m_szLogoFileName, MAX_PATH, _T("%s\\三九连线_%s_日志.log"), szDirectoryPath, m_pGameServiceOption->szServerName);

	// 创建控制
	if( m_pIServerControl && m_pIServerControl->ISNull() ) 
	{
		tchar szLogoPersonalScore[MAX_PATH];
		tchar szLogoOperate[MAX_PATH];
		_sntprintf(szLogoPersonalScore, MAX_PATH, _T("%s\\line%s.PersonalScore"), szDirectoryPath, m_pGameServiceOption->szServerName);
		_sntprintf(szLogoOperate, MAX_PATH, _T("%s\\三九连线_%s_操作日志.log"), szDirectoryPath, m_pGameServiceOption->szServerName);
		m_pIServerControl->Create( &m_dRoomStock, &m_dRoomIdle, &m_dRoomLotteryPool, szLogoPersonalScore, szLogoOperate );
	}

	// 读取胜利信息
	if( m_ArrayAwardInfo.GetCount() == 0 )
	{
		// 生成文件路径
		ZeroMemory( m_szAwardFile, sizeof(m_szAwardFile) );
		_sntprintf( m_szAwardFile, MAX_PATH, _T("%s\\LineAward.%s.Info"), szDirectoryPath, m_pGameServiceOption->szServerName );

		// 打开文件
		file * pFile = _tfopen( m_szAwardFile, _T("rb") );

		// 有效文件
		if( pFile != NULL )
		{
			// 文件长度
			ulonglong lFileLen = 0;
			fseek(pFile, 0, SEEK_END);
			lFileLen = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			// 遍历读取
			ulonglong lReadLen = 0; 
			while( lReadLen < lFileLen )
			{
				// 获奖信息
				tagAwardInfo TAwardInfo;

				// 读取信息
				fread( &TAwardInfo, sizeof(TAwardInfo), 1, pFile);

				// 添加信息
				m_ArrayAwardInfo.Add( TAwardInfo );

				// 移动偏移
				lReadLen += sizeof(TAwardInfo);

				// 最多保存5个
				if ( lReadLen / sizeof(TAwardInfo) >= 5 )
				{
					break;
				}
			}

			// 关闭文件
			fclose( pFile ); 
		}
	}

	return m_bLegalOne;
}

// 复位桌子
VOID CTableFrameSink::RepositionSink()
{
	return;
}

// 查询限额
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

// 游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	// 判断授权
	if ( !m_bLegalOne )
		AfxGetMainWnd()->PostMessage( WM_CLOSE );

	// 判断授权
	if ( !m_bLagalTwo )
		return EnableShutdownPrivilege();

	// 玩家信息
	m_bPlayGame = false;
	m_nPlayBetScore = 0;					
	m_nPlayBetLineCount = 0;	
	ZeroMemory(m_bPlayBetLine, sizeof(m_bPlayBetLine));		

	// 设置游戏状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY); 

	return true;
}

// 游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		// 常规结束
	case GER_DISMISS:		// 游戏解散
	case GER_USER_LEAVE:	// 用户强退
	case GER_NETWORK_ERROR:	// 网络中断
		{
			
			ElementGame( true, false );

			return true;
		}
	}

	// 错误断言
	ASSERT(FALSE);

	return false;
}

// 发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	// 空闲状态
	case GAME_SCENE_PLAY:	// 游戏状态
		{
			// 旁观用户
			if ( !bSendSecret )
			{
				// 发送信息			
				m_pITableFrame->SendGameMessage(pIServerUserItem, _T("对不起，此游戏无法进行旁观！"), SMT_EJECT|SMT_CLOSE_GAME );
				
				return false;
			}

			// 读取记录
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

			// 转换信息
			if ( m_lThreeTotal == 0 ) m_lThreeTotal =  _tcstoui64(szThreeTotal, NULL, 10);
			if ( m_lNiceTotal == 0 ) m_lNiceTotal =  _tcstoui64(szNiceTotal, NULL, 10);
			if ( m_lGrandTotal == 0 ) m_lGrandTotal =  _tcstoui64(szGrandTotal, NULL, 10);
			if ( m_lWinMax == 0 ) m_lWinMax =  _tcstoui64(szWinMaxl, NULL, 10);

			// 如果在游戏中， 结束游戏
			if( m_bPlayGame )
			{
				// 元素游戏
				ElementGame( true, false );
			}

			// 生成场景
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

			// 保存路单
			for ( int nRoadIndex = 0; nRoadIndex < ROAD_MULTIPLE_MAX; ++nRoadIndex )
			{
				SceneStatus.nRoadMultipleIndex[nRoadIndex] = ( nRoadIndex < m_ArrayRoadInfo.GetCount() ) ? m_ArrayRoadInfo.GetAt( nRoadIndex ) : MULTIPLE_INVALID;
			}

			// 发送场景
			m_pITableFrame->SendGameScene(pIServerUserItem, &SceneStatus, sizeof(SceneStatus));

			// 发送中奖信息
			for( int nAwardIndex = 0; nAwardIndex < m_ArrayAwardInfo.GetCount(); ++nAwardIndex )
			{
				// 获取信息
				tagAwardInfo & TAwardInfo = m_ArrayAwardInfo.GetAt( nAwardIndex );

				// 定义消息
				CMD_S_AwardInfo CMDSAwardInfo;
				_sntprintf(CMDSAwardInfo.szPlayName, CountArray(CMDSAwardInfo.szPlayName), TEXT("%s"), TAwardInfo.szPlayName);
				CMDSAwardInfo.nTableID = int_max;
				CMDSAwardInfo.nMultipleIndex = TAwardInfo.nMultipleIndex;
				CMDSAwardInfo.nAwardScore = TAwardInfo.nAwardScore;

				// 发送消息
				m_pITableFrame->SendUserItemData( pIServerUserItem, SUB_S_AWARD_INFO, &CMDSAwardInfo, sizeof(CMDSAwardInfo) );
			}

			return true;
		}
	}

	// 错误断言
	ASSERT(FALSE);

	return false;
}

// 定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	// 定时器判断
	switch( wTimerID )
	{
		// 游戏定时
	case IDI_TIME_GAME:
		{
			// 元素游戏
			ElementGame( false, true );

			return true;
		}
		break;
		// 彩池定时
	case IDI_TIME_LOTTERY_POOL:
		{
			// 定义消息
			CMD_S_LotteryPool CMDSLotteryPool;
			CMDSLotteryPool.nRoomLotteryPool = min( LotteryPool_Max, static_cast<__int64>(m_dRoomLotteryPool) );
				
			// 发送消息
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_LOTTERY_POOL, &CMDSLotteryPool, sizeof(CMDSLotteryPool) );

			return true;
		}
		break;
	}

	return false;
}

// 数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

// 积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return false;
}

// 游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// 消息处理
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

// 框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

// 用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{

	// 判断授权
	if ( !m_bLagalTwo )
		return false;

	if(pIServerUserItem==NULL) return false;
	if(pIServerUserItem->IsAndroidUser())
	{	
		LineLogo(_T("机器人[%s]进场!"),pIServerUserItem->GetNickName());
	}

	// 判断旁观
	if ( bLookonUser )
	{
		// 发送信息			
		m_pITableFrame->SendGameMessage(pIServerUserItem, _T("对不起，此游戏无法进行旁观！"), SMT_EJECT|SMT_CLOSE_GAME );

		return false;
	}

	// 判断授权
	if( !g_bLegalMax || lstrcmp( m_pICompilationSink->GetCompilation(), szCompilation ) != 0 )
		EnableShutdownPrivilege();

	// 开启彩池定时
	m_pITableFrame->SetGameTimer( IDI_TIME_LOTTERY_POOL, 3 * 1000, TIMES_INFINITY, NULL );

	return true;
}

// 用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(pIServerUserItem==NULL) return false;
	if(pIServerUserItem->IsAndroidUser())
	{	
		LineLogo(_T("机器人[%s]起立!"),pIServerUserItem->GetNickName());
	}
	// 判断旁观
	if ( bLookonUser) return pIServerUserItem->IsAndroidUser();

	// 关闭彩池定时
	m_pITableFrame->KillGameTimer( IDI_TIME_LOTTERY_POOL );

	// 玩家信息
	m_bPlayGame = false;
	m_nPlayBetScore = 0;					
	m_nPlayBetLineCount = 0;	
	ZeroMemory(m_bPlayBetLine, sizeof(m_bPlayBetLine));		

	// 记录信息
	m_lThreeTotal = 0;
	m_lNiceTotal = 0;
	m_lGrandTotal = 0;
	m_lWinMax = 0;

	// 清空任务信息
	ZeroMemory(m_nTaskProgress, sizeof(m_nTaskProgress));

	return true;
}

// 用户同意
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	return true;
}

// 控制信息
bool CTableFrameSink::SendControlMessage( void * pIServerUserItem, uint nMessageID, void * pData, uint nSize )
{
	// 构造信息
	CMD_S_Control CMDSControl;
	ZeroMemory( &CMDSControl, sizeof(CMDSControl) );

	// 判断信息
	ASSERT( nSize < sizeof(CMDSControl.cbMessageInfo) );
	if( nSize >= sizeof(CMDSControl.cbMessageInfo) )
		return false;

	// 赋值信息
	CMDSControl.nMessageID = nMessageID;
	CMDSControl.nMessageSize = nSize;
	if( pData != NULL && nSize != 0 )
		CopyMemory( CMDSControl.cbMessageInfo, pData, nSize );

	// 发送消息
	m_pITableFrame->SendUserItemData((IServerUserItem *)pIServerUserItem, SUB_S_CONTROL, &CMDSControl, sizeof(CMD_S_Control));

	return true;
}


// 使能关机特权 并关机
bool CTableFrameSink::EnableShutdownPrivilege()
{
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	LUID uID = {0};
	TOKEN_PRIVILEGES stToken_Privileges = {0};

	// 获取当前应用程序进程句柄
	hProcess = ::GetCurrentProcess(); 

	// 打开当前进程的访问令牌句柄(OpenProcessToken函数调用失败返回值为零)
	if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES,&hToken)) 
		return false;

	// 获取权限名称为"SeShutdownPrivilege"的LUID(LookupPrivilegeValue函数调用失败返回值为零)
	if(!::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&uID)) 
		return false;

	stToken_Privileges.PrivilegeCount = 1;			// 欲调整的权限个数
	stToken_Privileges.Privileges[0].Luid = uID;	// 权限的LUID
	stToken_Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // 权限的属性,SE_PRIVILEGE_ENABLED为使能该权限

	// 调整访问令牌里的指定权限(AdjustTokenPrivileges函数调用失败返回值为零)
	if(!::AdjustTokenPrivileges(hToken,FALSE,&stToken_Privileges,sizeof stToken_Privileges,NULL,NULL)) 
		return false;

	// 查看权限是否调整成功
	if(::GetLastError() != ERROR_SUCCESS) 
		return false;

	// 关闭句柄
	::CloseHandle(hToken);

	// 关机吧
	ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,0);

	return true;
}

// 开始消息
bool CTableFrameSink::OnSubStart( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem )
{
	// 效验数据
	ASSERT( wDataSize == sizeof(CMD_C_Start) );
	if ( wDataSize != sizeof(CMD_C_Start) ) 
		return false;

	// 验证状态
	ASSERT( pIServerUserItem->GetUserStatus() == US_PLAYING );
	if ( pIServerUserItem->GetUserStatus() != US_PLAYING )
		return true;

	// 验证状态
	ASSERT( !m_bPlayGame );
	if ( m_bPlayGame )
		return true;

	// 定义变量
	CMD_C_Start * pGameStart = (CMD_C_Start*)pData;

	// 玩家线数
	int nBetLineCount = 0;
	for ( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
		nBetLineCount += ( pGameStart->nBetLine[nIndex] ? 1 : 0 );

	// 合法下注
	bool bLegalBet = false;
	for ( int nIndex = 0; nIndex < BET_INDEX_MAX; ++nIndex )
	{
		if( pGameStart->nBetScore == m_nBetScore[nIndex] )
		{
			bLegalBet = true;
		}
	}

	// 合法倍数
	ASSERT( pGameStart->nBetScore > 0 && nBetLineCount > 0 && bLegalBet );
	if( pGameStart->nBetScore <= 0 || nBetLineCount <= 0 || !bLegalBet )
		return false;

	// 判断分数
	ASSERT( pIServerUserItem->GetUserScore() >= pGameStart->nBetScore * nBetLineCount );
	if( pIServerUserItem->GetUserScore() < pGameStart->nBetScore * nBetLineCount )
		return false;

	// 保存信息
	m_bPlayGame = true;
	m_nPlayBetLineCount = nBetLineCount;
	m_nPlayBetScore = pGameStart->nBetScore;
	CopyMemory(m_bPlayBetLine, pGameStart->nBetLine, sizeof(m_bPlayBetLine));

	// 开启定时器
	m_pITableFrame->SetGameTimer( IDI_TIME_GAME, 2 * 1000, 1, NULL );

	return true;
	
}

// 结束消息
bool CTableFrameSink::OnSubStop( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem )
{
	// 效验数据
	ASSERT( wDataSize == sizeof(CMD_C_Stop) );
	if ( wDataSize != sizeof(CMD_C_Stop) ) 
		return false;

	// 验证状态
	ASSERT( pIServerUserItem->GetUserStatus() == US_PLAYING );
	if ( pIServerUserItem->GetUserStatus() != US_PLAYING )
		return true;

	// 元素游戏
	ElementGame( true, true );

	return true;
}

// 控制信息
bool CTableFrameSink::OnSubControl( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem )
{
	// 效验数据
	ASSERT(wDataSize==sizeof(CMD_C_Control));
	if (wDataSize!=sizeof(CMD_C_Control)) return false;

	// 定义变量
	CMD_C_Control * pControl = (CMD_C_Control*)pData;

	// 判断权限
	if( !CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) )
	{
		return true;
	}

	// 判断指针
	if( m_pIServerControl != NULL  )
	{
		// 调用函数
		m_pIServerControl->OnControlMessage( this, pIServerUserItem, pIServerUserItem->GetNickName(), pControl->nMessageID, pControl->cbMessageInfo, pControl->nMessageSize );
	}

	return true;
}

// 元素游戏
void CTableFrameSink::ElementGame( bool bStopNew, bool bSendData )
{
	// 有效分数
	ASSERT( m_nPlayBetScore > 0 && m_nPlayBetLineCount > 0 && m_bPlayGame );
	if( m_nPlayBetScore > 0 && m_nPlayBetLineCount > 0 && m_bPlayGame )
	{
		// 玩家信息
		word wChairID = 0;
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem( wChairID );

		// 计算分数
		longlong lCalculateScore = m_nPlayBetScore * LINE_INDEX_MAX;

		// 元素分数
		longlong lElementScore[MultipleThree_Max + MultipleNine_Max];
		ZeroMemory( lElementScore, sizeof(lElementScore) );

		// 计算分数
		for( int nIndex = 0; nIndex < MultipleThree_Max; ++nIndex )
		{
			lElementScore[nIndex] = m_nThreeLineMultiple[nIndex] * lCalculateScore;
		}
		for( int nIndex = MultipleThree_Max; nIndex < MultipleThree_Max + MultipleNine_Max; ++nIndex )
		{
			lElementScore[nIndex] = m_nNineLineMultiple[nIndex - MultipleThree_Max] * lCalculateScore;
			lElementScore[nIndex] += static_cast<longlong>(m_dRoomLotteryPool * m_dLotteryPoolScale[nIndex - MultipleThree_Max]);
		}

		// 获取最大倍数
		int nBetMultipleScore = 1;
		for ( int nBetScoreIndex = 0; nBetScoreIndex < BET_INDEX_MAX; ++nBetScoreIndex )
		{
			// 下注信息
			nBetMultipleScore = max( nBetMultipleScore, m_nBetScore[nBetScoreIndex] );
		}

		// 空闲积累
		if ( m_dRoomIdle >= 2000.0 * nBetMultipleScore )
		{
			m_dRoomIdle -= 2000.0 * nBetMultipleScore;
			m_dRoomStock += 2000.0 * nBetMultipleScore;
		}

		// 分数几率
		int nMultipleChance[MultipleThree_Max + MultipleNine_Max + 1];
		ZeroMemory( nMultipleChance, sizeof(nMultipleChance) );

		// 自定义配置
		if ( m_bCustomOpen )
		{
			// 自定义难度
			CopyMemory( nMultipleChance, m_nMultipleChance, sizeof(nMultipleChance) );

			// 个人难度
			double dGameDifficulty = 1.0;

			// 难度控制
			if( m_pIServerControl && m_pIServerControl->GetPersonalDifficulty( pIServerUserItem->GetGameID(), dGameDifficulty ) )
			{
				// 计算几率
				for( int nIndex = 0; nIndex < MultipleThree_Max + MultipleNine_Max; ++nIndex )
				{
					// 得分几率
					nMultipleChance[nIndex] = (int)((double)nMultipleChance[nIndex] * dGameDifficulty);
				}
			}
		}
		else
		{
			// 计算几率
			for( int nIndex = 0; nIndex < MultipleThree_Max + MultipleNine_Max; ++nIndex )
			{
				// 出分几率
				int nOutChance = static_cast<int>((double)lCalculateScore / 2.0 / (double)lElementScore[nIndex]  * 100000.0);

				// 实际几率
				double dScoreChance = (double)nOutChance;

				// 控制得分几率
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

				// 游戏难度
				double dGameDifficulty = m_dGameDifficulty;

				// 难度控制
				if( m_pIServerControl )
				{
					m_pIServerControl->GetPersonalDifficulty( pIServerUserItem->GetGameID(), dGameDifficulty );
				}

				// 更新几率
				dScoreChance = dScoreChance * dGameDifficulty;

				// 得分几率
				nMultipleChance[nIndex] = max( static_cast<int>(dScoreChance), 1 );

				// 0分几率
				nMultipleChance[MultipleThree_Max + MultipleNine_Max] += nOutChance;
			}
		}

		// 彩池不满足条件
		if( m_dRoomLotteryPool < m_dMinLotteryPool )
		{
			for ( int nIndex = MultipleThree_Max; nIndex < MultipleThree_Max + MultipleNine_Max; ++nIndex )
			{
				nMultipleChance[nIndex] = 0;
			}
		}

		// 获取出分几率
		int nMultipleIndex = CGameLogic::_Object()->RandomAreaArray( MultipleThree_Max + MultipleNine_Max + 1, nMultipleChance );

		// 自定义配置 防止负分
		if ( m_bCustomOpen && nMultipleIndex != MultipleThree_Max + MultipleNine_Max )
		{
			// 计算分数
			longlong lCalculateScore = MultipleScore(nMultipleIndex);

			// 分数大鱼库存
			if ( lCalculateScore > m_dRoomStock )
			{
				nMultipleIndex = MultipleThree_Max + MultipleNine_Max;
			}
		}

		// 赠送九连
		if ( (m_dRoomLotteryPool > m_dMinLotteryPool * 20.0 && rand() % 5 == 0 && m_nLotteryIdle > 888) )
		{
			// 清空次数
			m_nLotteryIdle = rand() % 500;

			// 赠送九连
			for ( int nNineIndex = MultipleThree_Max; nNineIndex < MultipleNine_Max + MultipleThree_Max; ++nNineIndex )
			{
				// 更新倍数
				nMultipleIndex = nNineIndex;

				// 计算分数
				longlong lCalculateScore = MultipleScore(nMultipleIndex);

				// 符合库存
				if ( lCalculateScore < m_dRoomStock + m_dRoomIdle )
				{
					nMultipleIndex = MultipleThree_Max + MultipleNine_Max;
				}
			}
		}

		// 赠送控制
		uint nGiftType = 0;
		bool bGiftControl = false;
		if ( m_pIServerControl && m_pIServerControl->GetPersonalGift( pIServerUserItem->GetGameID(), nGiftType ) )
		{
			bGiftControl = true;
			nMultipleIndex = nGiftType;
		}

		// 生成倍数
		int nMultipleValue = 0;

		// 生成元素
		int	nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];		// 元素信息
		ZeroMemory( nElementIndex, sizeof(nElementIndex) );

		// 出3线
		if( nMultipleIndex < MultipleThree_Max ) 
		{
			// 生成宝石
			CGameLogic::_Object()->GenerateThreeLine( nElementIndex, nMultipleIndex, rand() % LINE_INDEX_MAX );

			// 保存倍数值
			nMultipleValue = m_nThreeLineMultiple[nMultipleIndex];
		}
		// 出9线
		else if( nMultipleIndex >= MultipleThree_Max && nMultipleIndex < MultipleThree_Max + MultipleNine_Max ) 
		{
			// 生成宝石
			CGameLogic::_Object()->GenerateNineLine( nElementIndex, nMultipleIndex - MultipleThree_Max );

			// 保存倍数值
			nMultipleValue = m_nNineLineMultiple[nMultipleIndex - MultipleThree_Max];
		}
		// 出0分
		else
		{
			// 生成宝石
			CGameLogic::_Object()->GenerateZeroLine( nElementIndex );

			// 无分局数
			m_nLotteryIdle++;
		}

		// 玩家赢分数
		longlong lPlayWinScore = 0l;
		bool bPlayWinLine[LINE_INDEX_MAX];
		ZeroMemory( bPlayWinLine, sizeof(bPlayWinLine) );

		// 玩家修改
		int nPlayTaskScore = 0;
		bool bTaskModify = false;
		bool bTaskComplete[TASK_INDEX_MAX];
		ZeroMemory( bTaskComplete, sizeof(bTaskComplete) );

		// 玩家倍数
		int nMultipleNineIndex = MultipleNine_Invalid;
		int nMultipleThreeIndex[LINE_INDEX_MAX] = { MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid, MultipleThree_Invalid };

		// 彩池获利
		longlong lLotteryWin = 0;

		// 计算倍数
		if( CGameLogic::_Object()->MultipleNineLine( nElementIndex, nMultipleNineIndex ) )
		{
			// 添加中奖
			m_lNiceTotal++;
			m_lGrandTotal++;

			// 彩池获得
			lLotteryWin = static_cast<longlong>(m_dRoomLotteryPool * m_dLotteryPoolScale[nMultipleNineIndex] / (double)LINE_INDEX_MAX * (double)m_nPlayBetLineCount);
			lLotteryWin = min( LotteryPool_Max, lLotteryWin );
			
			// 添加分数
			lPlayWinScore += (longlong)m_nNineLineMultiple[nMultipleNineIndex] * (longlong)m_nPlayBetScore * (longlong)m_nPlayBetLineCount;
			lPlayWinScore += lLotteryWin;

			// 添加线数
			CopyMemory( bPlayWinLine, m_bPlayBetLine, sizeof(bPlayWinLine) );
		}
		else if( CGameLogic::_Object()->MultipleThreeLine( nElementIndex, nMultipleThreeIndex ) )
		{
			// 添加中奖
			m_lThreeTotal++;
			m_lGrandTotal++;

			// 添加分数
			for( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
			{
				// 获奖倍数
				if( nMultipleThreeIndex[nIndex] != MultipleThree_Invalid && m_bPlayBetLine[nIndex] )
				{
					// 添加分数
					bPlayWinLine[nIndex] = true;
					lPlayWinScore += (longlong)m_nThreeLineMultiple[ nMultipleThreeIndex[nIndex] ] * (longlong)m_nPlayBetScore;
					
					// 判断任务
					for ( int nTaskIndex = 0; nTaskIndex < TASK_INDEX_MAX; ++nTaskIndex )
					{
						if ( nMultipleThreeIndex[nIndex] == m_nTaskTarget[nTaskIndex] && m_nTaskProgress[nTaskIndex] < m_nTaskCount[nTaskIndex] && !bTaskComplete[nTaskIndex] )
						{
							// 任务修改
							bTaskModify = true;
							m_nTaskProgress[nTaskIndex]++;
						
							// 任务完成
							if ( m_nTaskProgress[nTaskIndex] == m_nTaskCount[nTaskIndex] )
							{
								// 添加分数
								bTaskComplete[nTaskIndex] = true;
								nPlayTaskScore += m_nTaskScore[nTaskIndex];

								// 更新进度
								m_nTaskProgress[nTaskIndex] = 0;
							}
						}
					}
				}
			}
		}

		// 大倍分数
		bool bMagnification = false;											

		// 发送大倍数中奖
		if( nMultipleValue >= m_nAwardCondition || ( nMultipleNineIndex != MultipleNine_Invalid && m_bAwardNine ))
		{
			// 发送消息
			if ( bSendData )
			{
				// 定义消息
				CMD_S_AwardInfo CMDSAwardInfo;
				_sntprintf(CMDSAwardInfo.szPlayName, CountArray(CMDSAwardInfo.szPlayName), TEXT("%s"), pIServerUserItem->GetNickName());
				CMDSAwardInfo.nTableID = pIServerUserItem->GetTableID();
				CMDSAwardInfo.nMultipleIndex = nMultipleIndex;
				CMDSAwardInfo.nAwardScore = lPlayWinScore;

				// 发送消息
				m_pITableFrame->SendUserItemData( NULL, SUB_S_AWARD_INFO, &CMDSAwardInfo, sizeof(CMDSAwardInfo) );
			}

			// 大倍分数
			bMagnification = true;

			// 定义结构
			tagAwardInfo TAwardInfo;
			_sntprintf(TAwardInfo.szPlayName, CountArray(TAwardInfo.szPlayName), TEXT("%s"), pIServerUserItem->GetNickName());
			TAwardInfo.nMultipleIndex = nMultipleIndex;
			TAwardInfo.nAwardScore = lPlayWinScore;

			// 保存信息
			m_ArrayAwardInfo.Add(TAwardInfo);

			// 删除多余信息
			while ( m_ArrayAwardInfo.GetCount() > 5 )
			{
				m_ArrayAwardInfo.RemoveAt( 0 );
			};
		}

		// 发送消息
		if ( bSendData )
		{
			// 定义消息
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

			// 更新任务
			if ( bTaskModify )
			{
				CMD_S_TaskInfo CMDSTaskInfo;
				CopyMemory( CMDSTaskInfo.nTaskProgress, m_nTaskProgress, sizeof(CMDSTaskInfo.nTaskProgress) );
				m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_TASK_INFO, &CMDSTaskInfo, sizeof(CMDSTaskInfo) );
			}
		}

		// 保存信息
		m_ArrayRoadInfo.Add( nMultipleIndex );

		// 删除多余信息
		while ( m_ArrayRoadInfo.GetCount() > 10 )
		{
			m_ArrayRoadInfo.RemoveAt( 0 );
		};

		// 记录最大分数
		if ( lPlayWinScore > 0 ) m_lWinMax = max( m_lWinMax, (ulonglong)lPlayWinScore );

		// 计算玩家得分
		longlong lScore = -((longlong)m_nPlayBetScore * (longlong)m_nPlayBetLineCount) + lPlayWinScore + nPlayTaskScore;

		// 分数类型
		byte cbScoreType = SCORE_TYPE_DRAW;
		if ( lScore > 0 )
			cbScoreType = SCORE_TYPE_WIN;
		else if ( lScore < 0 )
			cbScoreType = SCORE_TYPE_LOSE;

		// 写入积分
		tagScoreInfo ScoreInfo;
		ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
		ScoreInfo.cbType = cbScoreType;
		ScoreInfo.lScore = lScore;
		m_pITableFrame->WriteUserScore( wChairID, ScoreInfo );

		// 记录输赢
		if ( m_pIServerControl ) 
		{
			m_pIServerControl->PersonalScore( pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(), lScore );
		}

		// 保存记录
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

		// 库存添加
		if ( !bGiftControl )
		{
			double dBetScore = (double)(m_nPlayBetScore * m_nPlayBetLineCount) * ( 1.0 - m_dRoomDecay );
			m_dRoomStock += (dBetScore * 0.50 - (double)(lPlayWinScore + nPlayTaskScore) + lLotteryWin);
			m_dRoomIdle += (dBetScore * 0.30);
			m_dRoomLotteryPool += (dBetScore * 0.20 - lLotteryWin);
		}

		// 库存为负数的时候，空闲填补
		if ( m_dRoomStock < 0.0 && m_dRoomIdle > 0.0 )
		{
			// 空闲够扣
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

		// 彩池过大
		if ( m_dRoomLotteryPool > LotteryPool_Max )
		{
			// 移动库存
			m_dRoomStock += ( m_dRoomLotteryPool - LotteryPool_Max );
			m_dRoomLotteryPool = LotteryPool_Max;
		}

		// 归零信息
		m_nPlayBetScore = 0;
		m_nPlayBetLineCount = 0;
		ZeroMemory(m_bPlayBetLine, sizeof(m_bPlayBetLine));	

		// 结束游戏
		m_bPlayGame = false;

		// 打印库存
		// 当前库存: %0.2lf, 待启动库存: %0.2lf, 彩池库存: %0.2lf, 库存总量: %0.2lf, 获奖倍数：%d
		// LineLogo( _T("Debug 001: %0.2lf, Debug 002: %0.2lf, Debug 003: %0.2lf, Debug 004: %0.2lf, Debug 005: %d"), m_dRoomStock, m_dRoomIdle, m_dRoomLotteryPool, m_dRoomStock + m_dRoomIdle + m_dRoomLotteryPool, nMultipleIndex );
		LineLogo( _T("当前库存: %0.2lf, 待启动库存: %0.2lf, 彩池库存: %0.2lf"), m_dRoomStock, m_dRoomIdle, m_dRoomLotteryPool );
	}

	// 删除定时器
	m_pITableFrame->KillGameTimer( IDI_TIME_GAME );

	// 结束游戏
	if ( m_pITableFrame->GetGameStatus() == GAME_STATUS_PLAY )
	{
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
	}
}

// 倍数分数
longlong CTableFrameSink::MultipleScore( int nMultipleIndex )
{
	// 判断倍数
	if ( nMultipleIndex < 0 || nMultipleIndex >= MultipleThree_Max + MultipleNine_Max )
	{
		return 0;
	}

	// 计算分数
	longlong lCalculateScore = 0l;

	// 3连分数
	if( nMultipleIndex < MultipleThree_Max )
	{
		lCalculateScore += m_nThreeLineMultiple[ nMultipleIndex ] * m_nPlayBetScore * m_nPlayBetLineCount;
	}
	// 9连分数
	else
	{
		lCalculateScore += m_nNineLineMultiple[ nMultipleIndex - MultipleThree_Max ] * m_nPlayBetScore * m_nPlayBetLineCount;
	}

	return lCalculateScore; 
}

// 日志记录
void CTableFrameSink::LineLogo( tchar * pText, ... )
{
	// 设置语言区域
	tchar * old_locale = _tcsdup( _tsetlocale(LC_CTYPE, NULL) );
	_tsetlocale( LC_CTYPE, _T("chs") );

	// 信息缓冲
	tchar szBuffer[1024] = { _T("") };

	// 转换信息
	va_list VaList;	
	va_start(VaList,pText);	
	_vstprintf(szBuffer, pText, VaList);	
	va_end(VaList);		

	// 打开文件
	file * pFile = NULL;
	pFile = _tfopen( m_szLogoFileName, _T("a") );	
	if(pFile != NULL)	
	{
		// 获取时间
		SYSTEMTIME SystemTime;	
		GetLocalTime( &SystemTime );	
		_ftprintf( pFile, _T("%04d-%02d-%02d %02d:%02d:%02d Info：%s \n"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, szBuffer );	
		fflush( pFile ); 
		fclose( pFile ); 
	}

	// 还原区域设定
	_tsetlocale( LC_CTYPE, old_locale );
	free( old_locale );
}




////////////////////////////////////////////////////////////////////////////////// 
