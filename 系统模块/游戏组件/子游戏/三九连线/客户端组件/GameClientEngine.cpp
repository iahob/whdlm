#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"
#include ".\gameclientengine.h"

//////////////////////////////////////////////////////////////////////////////////
// 定时器
#define		IDI_TIME_OPEN_WIN				101					// 打开窗口
#define		IDI_TIME_CLOSE_WIN				102					// 关闭窗口


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

// 构造函数
CGameClientEngine::CGameClientEngine() : m_GameClientView( m_GameFrameViewDexter.m_GameClientView )
{
	// 操作信息
	m_bPlayGame = false;
	m_bPlayHosted = false;

	// 下注信息
	m_nBetIndex = 0;
	ZeroMemory( m_nBetScore, sizeof(m_nBetScore) );
	for( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex)
		m_bBetLine[nIndex] = true;

	int nBetScore[BET_INDEX_MAX] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	CopyMemory( m_nBetScore, nBetScore, sizeof(m_nBetScore) );

	// 赢信息
	m_lPlayWinScore = 0l;
	m_nPlayTaskScore = 0;
	m_nPlayMultipleIndex = 0;
	m_bMagnification = false;
	ZeroMemory( m_bTaskComplete, sizeof(m_bTaskComplete) );
	ZeroMemory( m_bPlayWinLine, sizeof(m_bPlayWinLine) );
	ZeroMemory( m_nElementIndex, sizeof(m_nElementIndex) );

}

// 析构函数
CGameClientEngine::~CGameClientEngine()
{
	ReleaseInterface( m_hControlInst, m_pIClientControl );
}

// 创建引擎
bool CGameClientEngine::OnInitGameEngine()
{
	// 设置属性
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	// 标识1
	if( lstrcmp( m_ClientCompilationSink->GetCompilation(), szCompilation ) == 0 )
		m_bLegalOne = true;
	else
		m_bLegalOne = false;

	// 读取配置
	g_GameOption.InitParameter( AfxGetAppName() );

	// 加载参数
	g_GameOption.LoadParameter();

	// 创建声音
	CSound::_Object()->CreateD3DSound( AfxGetMainWnd()->GetSafeHwnd() );
	CMusic::_Object()->CreateD3DSound( AfxGetMainWnd()->GetSafeHwnd() );

	// 切换按钮
	m_GameClientView.m_ButtonSoundOn.ShowWindow( g_GameOption.bSound == TRUE, dex::WindowShow_Size );
	m_GameClientView.m_ButtonSoundOff.ShowWindow( g_GameOption.bSound == FALSE, dex::WindowShow_Size );

	// 背景音乐
	CMusic::_Object()->AppendMusic( _T("BackSound.ogg") );
	if( g_GameOption.bSound )
		CMusic::_Object()->PlayMusic();

	// 设置标识2
	m_bLagalTwo = m_bLegalOne;

	// 创建控制
	CreateInterface(m_hControlInst, m_pIClientControl, IClientControl, "LineClientControl.dll", "CreateClientControl" );

	// 创建窗口
	if ( m_pIClientControl ) m_pIClientControl->Create( GetDesktopWindow(), this );

	return true;
}

// 重置函数
bool CGameClientEngine::OnResetGameEngine()
{
	return true;
}

// 时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

// 时钟信息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	return false;
}

// 旁观消息
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

// 游戏消息
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

// 场景消息
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	// 处理信息
	if( !m_bLagalTwo || !m_bLegalOne )
	{
		AfxMessageBox( _T("非法组件！") );
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return false;
	}

	// 判断
	if( lstrcmp( m_ClientCompilationSink->GetCompilation(), szCompilation ) != 0 )
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return false;
	}

	// 执行消息
	switch ( cbGameStatus )
	{
	case GAME_SCENE_FREE:
	case GAME_SCENE_PLAY:
		{
			// 效验数据
			ASSERT( wDataSize == sizeof(CMD_S_Status) );
			if ( wDataSize != sizeof(CMD_S_Status) ) return false;

			// 定义变量
			CMD_S_Status * pScene = (CMD_S_Status*)pData;

			// 设置信息
			CopyMemory( m_nBetScore, pScene->nBetScore, sizeof(m_nBetScore) );

			// 设置界面
			m_GameClientView.SetRoomLotteryPool( pScene->nRoomLotteryPool );
			m_GameClientView.SetThreeLineMultiple( pScene->nThreeLineMultiple );
			m_GameClientView.SetNineLineMultiple( pScene->nNineLineMultiple );
			m_GameClientView.SetTaskProgress( pScene->nTaskProgress );
			m_GameClientView.SetTaskTarget( pScene->nTaskTarget );
			m_GameClientView.SetTaskCount( pScene->nTaskCount );
			m_GameClientView.SetTaskScore( pScene->nTaskScore );

			// 设置指针
			m_GameClientView.SetClientOneSelf( GetMeUserItem() );
			m_GameClientView.SetBetScore( m_nBetScore[m_nBetIndex] );
			m_GameClientView.SetBetLine( m_bBetLine );

			// 保存路单
			for ( int nRoadIndex = 0; nRoadIndex < ROAD_MULTIPLE_MAX; ++nRoadIndex )
			{
				if( pScene->nRoadMultipleIndex[nRoadIndex] != MULTIPLE_INVALID )
				{
					m_GameClientView.m_WindowRoad.AppendRoad( pScene->nRoadMultipleIndex[nRoadIndex] );
				}
			}

			//控制按钮
			if( CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_pIClientControl )
			{
				m_GameClientView.m_ButtonControl.ShowWindow( true );
			}

			return true;
		}
	}

	return false;
}

// 控制信息
bool CGameClientEngine::SendControlMessage( uint nMessageID, void * pData, uint nSize )
{
	// 构造信息
	CMD_C_Control CMDCControl;
	ZeroMemory( &CMDCControl, sizeof(CMDCControl) );

	// 判断信息
	ASSERT( nSize < sizeof(CMDCControl.cbMessageInfo) );

	// 赋值信息
	CMDCControl.nMessageID = nMessageID;
	CMDCControl.nMessageSize = nSize;
	if( pData != NULL && nSize != 0 )
		CopyMemory( CMDCControl.cbMessageInfo, pData, nSize );

	//发送消息
	SendSocketData(SUB_C_CONTROL, &CMDCControl, sizeof(CMDCControl));

	return true;
}


// 定时器消息
void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	// 判断定时器
	switch( nIDEvent )
	{
	case IDI_TIME_OPEN_WIN:
		{
			// 删除定时
			KillTimer( IDI_TIME_OPEN_WIN );

			// 关闭窗口
			m_GameClientView.m_WindowEgg.ShowWindow( false, dex::WindowShow_Size );

			// 开启积分提示
			if ( m_nPlayMultipleIndex >= 0 && m_nPlayMultipleIndex < MultipleThree_Max + MultipleNine_Max )
			{
				// 开启关闭定时
				SetTimer( IDI_TIME_CLOSE_WIN, 5000, NULL );

				// 播放音效
				if( m_nPlayMultipleIndex >= MultipleThree_Max )
				{
					CSound::_Object()->PlaySound( _T("GoldBigMove.ogg") );
				}
				else
				{
					CSound::_Object()->PlaySound( _T("GoldMove.ogg") );
				}

				// 弹出赢界面
				m_GameClientView.m_WindowWin.SetShowTime( 200 );
				m_GameClientView.m_WindowWin.AppendGold( m_bMagnification );
				m_GameClientView.m_WindowWin.SetWinScore( m_nPlayMultipleIndex, m_lPlayWinScore, m_nPlayTaskScore, m_bTaskComplete );
				m_GameClientView.m_WindowWin.ShowWindow( true, dex::WindowShow_Size );
			}
			else
			{
				// 结束游戏
				m_bPlayGame = false;

				// 托管中自动开始
				if( m_bPlayHosted )
				{
					OnMessageStartGame(0, 0);
				}
			}
		}
		break;
	case IDI_TIME_CLOSE_WIN:
		{
			// 删除定时
			KillTimer( IDI_TIME_CLOSE_WIN );

			// 关闭窗口
			m_GameClientView.m_WindowWin.ShowWindow( false, dex::WindowShow_Size );

			// 结束游戏
			m_bPlayGame = false;

			// 托管中自动开始
			if( m_bPlayHosted )
			{
				OnMessageStartGame(0, 0);
			}
		}
		break;
	}

	// 基类执行
	CGameFrameEngine::OnTimer(nIDEvent);
}

// 元素消息
bool CGameClientEngine::OnSubElementStop( VOID * pData, WORD wDataSize )
{
	// 效验数据
	ASSERT( wDataSize == sizeof(CMD_S_ElementStop) );
	if ( wDataSize != sizeof(CMD_S_ElementStop) ) return false;

	// 定义变量
	CMD_S_ElementStop * pElementStop = (CMD_S_ElementStop*)pData;

	// 保存信息
	m_lPlayWinScore = pElementStop->lPlayWinScore;
	m_nPlayTaskScore = pElementStop->nPlayTaskScore;
	m_nPlayMultipleIndex = pElementStop->nMultipleIndex;
	m_bMagnification = pElementStop->bMagnification;
	CopyMemory( m_bTaskComplete, pElementStop->bTaskComplete, sizeof(m_bTaskComplete) );
	CopyMemory( m_bPlayWinLine, pElementStop->bPlayWinLine, sizeof(m_bPlayWinLine) );
	CopyMemory( m_nElementIndex, pElementStop->nElementIndex, sizeof(m_nElementIndex) );

	// 停止旋转
	m_GameClientView.m_WindowTurntable.StopElement( pElementStop->nElementIndex, pElementStop->bStopNew );

	return true;
}

// 彩池消息
bool CGameClientEngine::OnSubLotteryPool( VOID * pData, WORD wDataSize )
{
	// 效验数据
	ASSERT( wDataSize == sizeof(CMD_S_LotteryPool) );
	if ( wDataSize != sizeof(CMD_S_LotteryPool) ) return false;

	// 定义变量
	CMD_S_LotteryPool * pLotteryPool = (CMD_S_LotteryPool*)pData;

	// 设置界面
	m_GameClientView.SetRoomLotteryPool( pLotteryPool->nRoomLotteryPool );

	return true;
}

// 任务信息
bool CGameClientEngine::OnSubTaskInfo( VOID * pData, WORD wDataSize )
{
	// 效验数据
	ASSERT( wDataSize == sizeof(CMD_S_TaskInfo) );
	if ( wDataSize != sizeof(CMD_S_TaskInfo) ) return false;

	// 定义变量
	CMD_S_TaskInfo * pTaskInfo = (CMD_S_TaskInfo*)pData;

	// 设置进度
	m_GameClientView.SetTaskProgress( pTaskInfo->nTaskProgress );

	return true;
}

// 中奖信息
bool CGameClientEngine::OnSubAwardInfo( VOID * pData, WORD wDataSize )
{
	// 效验数据
	ASSERT( wDataSize == sizeof(CMD_S_AwardInfo) );
	if ( wDataSize != sizeof(CMD_S_AwardInfo) ) return false;

	// 定义变量
	CMD_S_AwardInfo * pAwardInfo = (CMD_S_AwardInfo*)pData;

	// 用户指针
	IClientUserItem * pMeUserItem = GetMeUserItem();

	// 非自己动画
	if ( pMeUserItem == NULL || pMeUserItem->GetTableID() != pAwardInfo->nTableID )
	{
		// 添加动画
		m_GameClientView.AppendAward( pAwardInfo->szPlayName, pAwardInfo->nMultipleIndex, pAwardInfo->nAwardScore );
	}
	else
	{
		// 定义中奖
		tagAwardInfo TAwardInfo;
		_sntprintf(TAwardInfo.szPlayName, CountArray(TAwardInfo.szPlayName), TEXT("%s"), pAwardInfo->szPlayName);
		TAwardInfo.nMultipleIndex = pAwardInfo->nMultipleIndex;
		TAwardInfo.nAwardScore = pAwardInfo->nAwardScore;

		// 保存信息
		m_ArrayAwardInfo.Add(TAwardInfo);
	}

	return true;
}

//控制消息
bool CGameClientEngine::OnSubControl( VOID * pData, WORD wDataSize )
{
	// 效验数据
	ASSERT(wDataSize==sizeof(CMD_C_Control));
	if (wDataSize!=sizeof(CMD_C_Control)) return false;

	// 定义变量
	CMD_C_Control * pControl = (CMD_C_Control*)pData;

	// 判断指针
	if( m_pIClientControl == NULL )
		return true;

	// 调用函数
	m_pIClientControl->OnControlMessage( pControl->nMessageID, pControl->cbMessageInfo, pControl->nMessageSize );

	return true;
}



// 旋转结束
LRESULT CGameClientEngine::OnMessageTurnOver( WPARAM wParam, LPARAM lParam )
{
	// 修改按钮
	m_GameClientView.m_ButtonStartGame.ShowWindow( true, dex::WindowShow_Size );
	m_GameClientView.m_ButtonStopGame.ShowWindow( false, dex::WindowShow_Size );

	//加注按钮
	m_GameClientView.m_ButtonScoreLess.EnableWindow( true && !m_bPlayHosted );
	m_GameClientView.m_ButtonScoreAdd.EnableWindow( true && !m_bPlayHosted );
	m_GameClientView.m_ButtonScoreMax.EnableWindow( true && !m_bPlayHosted );

	// 删除以前的提示
	m_GameClientView.ClearMineAward();
	m_GameClientView.m_WindowLine.ClearWinLine();

	// 开启提示
	m_GameClientView.m_WindowTurntable.LinePrompt();

	// 设置赢信息
	m_GameClientView.SetPlayWinScore( m_lPlayWinScore + m_nPlayTaskScore );
	m_GameClientView.SetPlayWinLine( m_bPlayWinLine );
	m_GameClientView.m_WindowLine.SetWinLine( m_bPlayWinLine );

	// 弹出癞子蛋
	bool bShowEgg = false;

	// 提示中奖
	if( m_nPlayMultipleIndex >= 0 && m_nPlayMultipleIndex < MultipleThree_Max + MultipleNine_Max )
	{
		// 九连
		if( m_nPlayMultipleIndex >= MultipleThree_Max )
		{
			// 添加中奖
			m_GameClientView.AppendMineAward( m_nPlayMultipleIndex );

			// 切换界面
			m_GameClientView.SetLineShow( MultipleType_Nine );
		}
		// 计算3连
		else
		{
			// 添加中奖
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

			// 切换界面
			m_GameClientView.SetLineShow( MultipleType_Three );
		}

		// 癞子计算
		if ( CGameLogic::_Object()->DeformCount(m_nElementIndex) > 0 )
		{
			// 定义位置
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

			// 癞子蛋数据
			CWHArray< int > ArrayEggElement;

			// 遍历线判断
			for( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
			{
				// 获取数值
				int nLineElement[3];
				for( int nLineElemenIndex = 0; nLineElemenIndex < 3; ++nLineElemenIndex )
				{
					nLineElement[nLineElemenIndex] = m_nElementIndex[ PointLine[nIndex][nLineElemenIndex].x ][ PointLine[nIndex][nLineElemenIndex].y ];
				}

				// 判断数值
				if ( CGameLogic::_Object()->ThreeLineDeform(nLineElement[0], nLineElement[1], nLineElement[2]) )
				{
					// 添加变量
					for( int nLineElemenIndex = 0; nLineElemenIndex < 3; ++nLineElemenIndex )
					{
						if( nLineElement[nLineElemenIndex] != m_nElementIndex[ PointLine[nIndex][nLineElemenIndex].x ][ PointLine[nIndex][nLineElemenIndex].y ] )
						{
							ArrayEggElement.Add( nLineElement[nLineElemenIndex] );
						}
					}
				}
			}

			// 有效提示
			if( ArrayEggElement.GetCount() > 0 )
			{
				// 弹出界面
				bShowEgg = true;
				m_GameClientView.m_WindowEgg.SetShowTime( 200 );
				m_GameClientView.m_WindowEgg.SetEggElement( ArrayEggElement );
				m_GameClientView.m_WindowEgg.ShowWindow( true, dex::WindowShow_Size );
			}
		}
	}

	// 提示中奖
	for( int nAwardIndex = 0; nAwardIndex < m_ArrayAwardInfo.GetCount(); ++nAwardIndex )
	{
		// 获取信息
		tagAwardInfo & TAwardInfo = m_ArrayAwardInfo.GetAt( nAwardIndex );

		// 添加动画
		m_GameClientView.AppendAward( TAwardInfo.szPlayName, TAwardInfo.nMultipleIndex, TAwardInfo.nAwardScore );
	}

	// 删除信息
	m_ArrayAwardInfo.RemoveAll();

	// 保存路单
	m_GameClientView.m_WindowRoad.AppendRoad( m_nPlayMultipleIndex );

	// 弹出定时
	SetTimer( IDI_TIME_OPEN_WIN, bShowEgg ? 4000 : 2000, NULL );

	// 播放音效
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


// 开始消息
LRESULT CGameClientEngine::OnMessageStartGame( WPARAM wParam, LPARAM lParam )
{
	// 删除定时
	KillTimer( IDI_TIME_OPEN_WIN );
	KillTimer( IDI_TIME_CLOSE_WIN );

	// 玩家线数
	int nBetLineCount = 0;
	for ( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
		nBetLineCount += ( m_bBetLine[nIndex] ? 1 : 0 );

	// 用户指针
	IClientUserItem * pMeUserItem = GetMeUserItem();

	// 有效玩家
	if ( pMeUserItem == NULL )
	{
		// 提示信息
		CGlobal::_Object()->MessageBox( _T("连接服务器失败，请重启游戏！") );

		return 0;
	}

	// 判断金币
	if ( pMeUserItem->GetUserScore() < m_nBetScore[m_nBetIndex] * nBetLineCount )
	{
		// 提示信息
		CGlobal::_Object()->MessageBox( _T("游戏币不足，请充值！") );

		return 0;
	}

	// 修改按钮
	m_GameClientView.m_ButtonStartGame.ShowWindow( false, dex::WindowShow_Size );
	m_GameClientView.m_ButtonStopGame.ShowWindow( true, dex::WindowShow_Size );

	// 加注按钮
	m_GameClientView.m_ButtonScoreLess.EnableWindow( false );
	m_GameClientView.m_ButtonScoreAdd.EnableWindow( false );
	m_GameClientView.m_ButtonScoreMax.EnableWindow( false );

	// 关闭窗口
	m_GameClientView.m_WindowWin.ShowWindow( false, dex::WindowShow_Size );
	m_GameClientView.m_WindowEgg.ShowWindow( false, dex::WindowShow_Size );

	// 删除以前的提示
	m_GameClientView.ClearMineAward();
	m_GameClientView.m_WindowLine.ClearWinLine();

	// 开始旋转
	m_GameClientView.m_WindowTurntable.TurnElement();

	// 发送准备
	SendUserReady( NULL, 0 );

	// 开始游戏
	m_bPlayGame = true;

	// 赢信息
	m_lPlayWinScore = 0l;
	m_nPlayTaskScore = 0;
	m_nPlayMultipleIndex = 0;
	m_bMagnification = false;
	ZeroMemory( m_bTaskComplete, sizeof(m_bTaskComplete) );
	ZeroMemory( m_bPlayWinLine, sizeof(m_bPlayWinLine) );
	ZeroMemory( m_nElementIndex, sizeof(m_nElementIndex) );

	// 定义消息
	CMD_C_Start CMDCStart;
	ZeroMemory( &CMDCStart, sizeof(CMD_C_Start) );

	// 生成消息
	CMDCStart.nBetScore = m_nBetScore[m_nBetIndex];
	CopyMemory( CMDCStart.nBetLine, m_bBetLine, sizeof(CMDCStart.nBetLine) );

	// 发送消息
	SendSocketData( SUB_C_START, &CMDCStart, sizeof(CMD_C_Start) );

	// 设置赢信息
	m_GameClientView.SetPlayWinScore( m_lPlayWinScore );
	m_GameClientView.SetPlayWinLine( m_bPlayWinLine );

	return 0;
}

// 停止消息
LRESULT CGameClientEngine::OnMessageStopGame( WPARAM wParam, LPARAM lParam )
{
	// 判断是否可直接结束
	if( !m_GameClientView.m_WindowTurntable.ForcedStopElement() )
	{
		// 定义消息
		CMD_C_Stop CMDCStop;
		ZeroMemory( &CMDCStop, sizeof(CMD_C_Stop) );

		// 发送消息
		SendSocketData( SUB_C_STOP, &CMDCStop, sizeof(CMD_C_Stop) );
	}

	return 0;
}

// 开始托管
LRESULT CGameClientEngine::OnMessageStartHosted( WPARAM wParam, LPARAM lParam )
{
	// 修改按钮
	m_GameClientView.m_ButtonStartHosted.ShowWindow( false, dex::WindowShow_Size );
	m_GameClientView.m_ButtonStopHosted.ShowWindow( true, dex::WindowShow_Size );

	// 设置变量
	m_bPlayHosted = true;
	m_GameClientView.m_WindowTurntable.SetSpeedUp(m_bPlayHosted);

	//加注按钮
	m_GameClientView.m_ButtonScoreLess.EnableWindow( false );
	m_GameClientView.m_ButtonScoreAdd.EnableWindow( false );
	m_GameClientView.m_ButtonScoreMax.EnableWindow( false );

	// 未在游戏中
	if( !m_bPlayGame )
		OnMessageStartGame(0, 0);

	return 0;
}

// 停止托管
LRESULT CGameClientEngine::OnMessageStopHosted( WPARAM wParam, LPARAM lParam )
{
	// 修改按钮
	m_GameClientView.m_ButtonStartHosted.ShowWindow( true, dex::WindowShow_Size );
	m_GameClientView.m_ButtonStopHosted.ShowWindow( false, dex::WindowShow_Size );

	// 设置变量
	m_bPlayHosted = false;
	m_GameClientView.m_WindowTurntable.SetSpeedUp(m_bPlayHosted);

	// 修改按钮
	if(m_GameClientView.m_ButtonStartGame.IsWindowVisible())
	{
		//加注按钮
		m_GameClientView.m_ButtonScoreLess.EnableWindow( true );
		m_GameClientView.m_ButtonScoreAdd.EnableWindow( true );
		m_GameClientView.m_ButtonScoreMax.EnableWindow( true );
	}

	return 0;
}

// 分数减少
LRESULT CGameClientEngine::OnMessageScoreLess( WPARAM wParam, LPARAM lParam )
{
	// 较少索引
	if ( m_nBetIndex > 0  )
	{
		// 临时索引
		int nBetIndex = m_nBetIndex;
	
		// 切换倍数
		do 
		{
			nBetIndex = (nBetIndex + BET_INDEX_MAX - 1) % BET_INDEX_MAX;

		} while ( m_nBetScore[nBetIndex] == 0 && nBetIndex > 0 );

		// 判断有效
		if( m_nBetScore[nBetIndex] != 0 )
		{
			m_nBetIndex = nBetIndex;
		}
	}

	// 设置界面
	m_GameClientView.SetBetScore( m_nBetScore[m_nBetIndex] );

	return 0;
}

// 分数增加
LRESULT CGameClientEngine::OnMessageScoreAdd( WPARAM wParam, LPARAM lParam )
{
	// 增加索引
	if ( m_nBetIndex < BET_INDEX_MAX - 1 )
	{
		// 临时索引
		int nBetIndex = m_nBetIndex;

		// 切换倍数
		do 
		{
			nBetIndex = (nBetIndex + 1) % BET_INDEX_MAX;

		} while ( m_nBetScore[nBetIndex] == 0 && nBetIndex < BET_INDEX_MAX - 1  );

		// 判断有效
		if( m_nBetScore[nBetIndex] != 0 )
		{
			m_nBetIndex = nBetIndex;
		}
	}

	// 设置界面
	m_GameClientView.SetBetScore( m_nBetScore[m_nBetIndex] );

	return 0;
}

// 分数最大
LRESULT CGameClientEngine::OnMessageScoreMax( WPARAM wParam, LPARAM lParam )
{
	// 分数最大
	int nScoreMax = 0;
	for ( int nIndex = 0; nIndex < BET_INDEX_MAX; ++nIndex )
	{
		// 分数判断
		if ( m_nBetScore[nIndex] > nScoreMax )
		{
			// 保存信息
			m_nBetIndex = nIndex;
			nScoreMax = m_nBetScore[nIndex];
		}
	}

	// 设置界面
	m_GameClientView.SetBetScore( m_nBetScore[m_nBetIndex] );

	return 0;
}

// 线数选择
LRESULT CGameClientEngine::OnMessageLineCheck( WPARAM wParam, LPARAM lParam )
{
	// 获取变量
	bool bLineCheck = ( wParam != FALSE );
	int nLineIndex = static_cast<int>( lParam );

	// 判断信息
	ASSERT( nLineIndex < LINE_INDEX_MAX );

	// 当前线数
	int nBetLineCount = 0;
	for ( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
		nBetLineCount += ( m_bBetLine[nIndex] ? 1 : 0 );

	// 唯一线数，不能取消
	if ( !bLineCheck && nBetLineCount == 1 && m_bBetLine[nLineIndex] )
		return 0;

	// 设置变量
	m_bBetLine[nLineIndex] = bLineCheck;

	// 设置界面
	m_GameClientView.SetBetLine( m_bBetLine );
	m_GameClientView.m_ButtonLineCheck[nLineIndex].ShowWindow( bLineCheck, dex::WindowShow_Size );
	m_GameClientView.m_ButtonLineUnCheck[nLineIndex].ShowWindow( !bLineCheck, dex::WindowShow_Size );

	return 0;
}

// 打开控制
LRESULT CGameClientEngine::OnMessageOpenControl( WPARAM wParam, LPARAM lParam )
{
	// 显示
	if ( m_pIClientControl ) m_pIClientControl->ShowWindow( true );

	return 0;
}





