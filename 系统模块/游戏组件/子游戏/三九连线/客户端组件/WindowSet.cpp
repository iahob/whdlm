#include "StdAfx.h"
#include ".\windowset.h"

// 控件变量
#define IDC_BT_CLOSE				600									
#define IDC_BT_OK					601		
#define IDC_BT_MUSIC_NO_VOICE		602
#define IDC_BT_SOUND_NO_VOICE		603
#define IDC_BT_MUSIC_VOICE			604
#define IDC_BT_SOUND_VOICE			605
#define IDC_EDIT_START				607
#define IDC_EDIT_LINE_COUNT_UP		608
#define IDC_EDIT_LINE_COUNT_DOWN	609
#define IDC_EDIT_LINE_NUMBER_UP		610
#define IDC_EDIT_LINE_NUMBER_DOWN	611
#define IDC_BT_DEFAULT_KEY			612

// 112 - 195
#define SELECT_BEGIN				215
#define SELECT_END					332

#define MUSIC_Y_POS					92
#define SOUND_Y_POS					138

CWindowSet::CWindowSet(void)
{
	ZeroMemory(m_nCurrentVolume, sizeof(m_nCurrentVolume));
	ZeroMemory(m_nLastVolume, sizeof(m_nLastVolume));
	ZeroMemory(m_bCurrentVoice, sizeof(m_bCurrentVoice));
	ZeroMemory(m_bLastVoice, sizeof(m_bLastVoice));

	ZeroMemory(m_bMouseDown, sizeof(m_bMouseDown));
	ZeroMemory(m_bMouseMove, sizeof(m_bMouseMove));
	ZeroMemory(m_ptLastSite, sizeof(m_ptLastSite));
	ZeroMemory(m_ptLbuttonDown, sizeof(m_ptLbuttonDown));
	ZeroMemory(m_rcMoveSelect, sizeof(m_rcMoveSelect));
}

CWindowSet::~CWindowSet(void)
{
}

// 动画消息
VOID CWindowSet::OnWindowMovie()
{
	for( INT i = 0; i < ST_MAX; ++i )
	{
		if(m_stuMoveFish[i].LapseCount.GetLapseCount(10) > 0)
			m_stuMoveFish[i].Move();
	}
}

// 创建消息
VOID CWindowSet::OnWindowCreate( CD3DDevice * pD3DDevice )
{
	// 变量定义
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	// 图片载入
	m_ImageBack.LoadImage(pD3DDevice, hResInstance, TEXT("SET_BACK"), TEXT("SET") );
	m_ImageSetBars.LoadImage(pD3DDevice, hResInstance, TEXT("SET_BARS"), TEXT("SET") );
	m_ImageSetMove.LoadImage(pD3DDevice, hResInstance, 5, 1, TEXT("SET_BT_MOVE"), TEXT("SET") );

	m_btNoVoice[ST_Music].ActiveWindow(rcCreate,0,IDC_BT_MUSIC_NO_VOICE,GetVirtualEngine(),this);
	m_btNoVoice[ST_Sound].ActiveWindow(rcCreate,0,IDC_BT_SOUND_NO_VOICE,GetVirtualEngine(),this);
	m_btVoice[ST_Music].ActiveWindow(rcCreate,0,IDC_BT_MUSIC_VOICE,GetVirtualEngine(),this);
	m_btVoice[ST_Sound].ActiveWindow(rcCreate,0,IDC_BT_SOUND_VOICE,GetVirtualEngine(),this);
	m_btClose.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_CLOSE,GetVirtualEngine(),this);
	m_btOk.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_OK,GetVirtualEngine(),this);
	m_btDefaultKey.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_DEFAULT_KEY,GetVirtualEngine(),this);

	m_btNoVoice[ST_Music].SetButtonImage(pD3DDevice,TEXT("SET_BT_NO_VOICE"),TEXT("SET"),hResInstance);
	m_btNoVoice[ST_Sound].SetButtonImage(pD3DDevice,TEXT("SET_BT_NO_VOICE"),TEXT("SET"),hResInstance);
	m_btVoice[ST_Music].SetButtonImage(pD3DDevice,TEXT("SET_BT_VOICE"),TEXT("SET"),hResInstance);
	m_btVoice[ST_Sound].SetButtonImage(pD3DDevice,TEXT("SET_BT_VOICE"),TEXT("SET"),hResInstance);
	m_btClose.SetButtonImage(pD3DDevice,TEXT("BT_QU_XIAO"),TEXT("SET"),hResInstance);
	m_btOk.SetButtonImage(pD3DDevice,TEXT("BT_QUE_DING"),TEXT("SET"),hResInstance);
	m_btDefaultKey.SetButtonImage(pD3DDevice,TEXT("BT_DEFAULT_KEY"),TEXT("SET"),hResInstance);

	m_EditStart.Create(0, rcCreate, CGameFrameView::GetInstance(), IDC_EDIT_START);
	m_EditLineCountUp.Create(0, rcCreate, CGameFrameView::GetInstance(), IDC_EDIT_LINE_COUNT_UP);
	m_EditLineCountDown.Create(0, rcCreate, CGameFrameView::GetInstance(), IDC_EDIT_LINE_COUNT_DOWN);
	m_EditLineNumberUp.Create(0, rcCreate, CGameFrameView::GetInstance(), IDC_EDIT_LINE_NUMBER_UP);
	m_EditLineNumberDown.Create(0, rcCreate, CGameFrameView::GetInstance(), IDC_EDIT_LINE_NUMBER_DOWN);

	// 获取大小
	CSize SizeGameScore;
	SizeGameScore.SetSize(m_ImageBack.GetWidth(),m_ImageBack.GetHeight());

	// 移动窗口
	SetWindowPos(0,0,SizeGameScore.cx,SizeGameScore.cy,0);

	m_btNoVoice[ST_Music].SetWindowPos( 23, 90, 0, 0, SWP_NOSIZE);
	m_btVoice[ST_Music].SetWindowPos( 23, 90, 0, 0, SWP_NOSIZE);

	m_btNoVoice[ST_Sound].SetWindowPos( 23, 136, 0, 0, SWP_NOSIZE);
	m_btVoice[ST_Sound].SetWindowPos( 23, 136, 0, 0, SWP_NOSIZE);

	m_btOk.SetWindowPos( SizeGameScore.cx/2 - 144 - 10, SizeGameScore.cy - 80, 0, 0, SWP_NOSIZE);
	m_btClose.SetWindowPos( SizeGameScore.cx/2 + 10, SizeGameScore.cy - 80, 0, 0, SWP_NOSIZE);
	m_btDefaultKey.SetWindowPos( SizeGameScore.cx/2 , SizeGameScore.cy - 120, 0, 0, SWP_NOSIZE);

	m_EditStart.SetKey(g_GameOption.m_nStartKey);
	m_EditLineCountUp.SetKey(g_GameOption.m_nLineCountUpKey);
	m_EditLineCountDown.SetKey(g_GameOption.m_nLineCountDownKey);
	m_EditLineNumberUp.SetKey(g_GameOption.m_nLineNumberUpKey);
	m_EditLineNumberDown.SetKey(g_GameOption.m_nLineNumberDownKey);
}

// 位置消息
VOID CWindowSet::OnWindowPosition( CD3DDevice * pD3DDevice )
{
	m_EditStart.SetWindowPos( NULL,			m_BenchmarkPos.x + 213,		m_BenchmarkPos.y + 222 + 40 * 0, 160, 20, 0);
	m_EditLineCountUp.SetWindowPos( NULL,	m_BenchmarkPos.x + 213,		m_BenchmarkPos.y + 222 + 40 * 1, 160, 20, 0);
	m_EditLineCountDown.SetWindowPos( NULL, m_BenchmarkPos.x + 213,		m_BenchmarkPos.y + 222 + 40 * 2, 160, 20, 0);
	m_EditLineNumberUp.SetWindowPos( NULL,	m_BenchmarkPos.x + 213,		m_BenchmarkPos.y + 222 + 40 * 3, 160, 20, 0);
	m_EditLineNumberDown.SetWindowPos( NULL,m_BenchmarkPos.x + 213,		m_BenchmarkPos.y + 222 + 40 * 4, 160, 20, 0);
}

// 鼠标事件
VOID CWindowSet::OnEventMouse( UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos )
{
	switch(uMessage)
	{
	case WM_LBUTTONDOWN:
		{
			for( INT i = 0; i < ST_MAX; ++i )
			{
				// 选择条点击
				if(m_rcMoveSelect[i].PtInRect(CPoint(nXMousePos, nYMousePos)))
				{
					m_bMouseDown[i] = TRUE;
					m_ptLbuttonDown[i].SetPoint(nXMousePos, nYMousePos);
					m_ptLastSite[i].SetPoint(m_rcMoveSelect[i].left, m_rcMoveSelect[i].top);
					break;
				}

			}
			break;

		}
	case WM_MOUSEMOVE:
		{
			// 拖动选择条
			for( INT i = 0; i < ST_MAX; ++i )
			{
				CRect rect( SELECT_BEGIN - 50 , MUSIC_Y_POS + i * 37, SELECT_END + 68, MUSIC_Y_POS + 23 + i * 37 );
				if(m_bMouseDown[i] && rect.PtInRect(CPoint(nXMousePos, nYMousePos)))
				{
					int nSite = (nXMousePos - m_ptLbuttonDown[i].x) + m_ptLastSite[i].x;
					if ( nSite > SELECT_END )
						nSite = SELECT_END;
					else if ( nSite < SELECT_BEGIN )
						nSite = SELECT_BEGIN;
					m_rcMoveSelect[i].left = nSite;
					m_rcMoveSelect[i].right = nSite + 23;

					if ( m_rcMoveSelect[i].left == SELECT_BEGIN )
						m_nCurrentVolume[i] = -MAX_VOLUME;
					else if ( m_rcMoveSelect[i].left == SELECT_END )
						m_nCurrentVolume[i] = 0;
					else
						m_nCurrentVolume[i] = -(INT)((DOUBLE)(SELECT_END - m_rcMoveSelect[i].left)/(DOUBLE)(SELECT_END - SELECT_BEGIN) * (DOUBLE)MAX_VOLUME);

					CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_SET,TRUE,0);
					break;
				}
				else if( m_bMouseDown[i] )
				{
					m_bMouseDown[i] = FALSE;
				}

				if(m_rcMoveSelect[i].PtInRect(CPoint(nXMousePos, nYMousePos)))
				{
					m_bMouseMove[i] = TRUE;
				}
				else
				{
					m_bMouseMove[i] = FALSE;
				}
			}
			break;	
		}
	case WM_LBUTTONUP:
		{
			// 弹起鼠标
			for( INT i = 0; i < ST_MAX; ++i )
			{
				m_bMouseDown[i] = FALSE;
			}
			break;
		}
	}
}

// 按钮事件
VOID CWindowSet::OnEventButton( UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos )
{
	switch ( uButtonID )
	{
	case IDC_BT_MUSIC_NO_VOICE:
		{
			m_bCurrentVoice[ST_Music] = TRUE;
			m_btVoice[ST_Music].ShowWindow(m_bCurrentVoice[ST_Music] == TRUE);
			m_btNoVoice[ST_Music].ShowWindow(m_bCurrentVoice[ST_Music] != TRUE);

			if( !m_bCurrentVoice[ST_Music] )
				m_stuMoveFish[ST_Music].BeginMove( DPOINT(m_rcMoveSelect[ST_Music].left, m_rcMoveSelect[ST_Music].top), DPOINT( SELECT_BEGIN,m_rcMoveSelect[ST_Music].top), 300, 1);
			else
				m_stuMoveFish[ST_Music].BeginMove( DPOINT(SELECT_BEGIN, m_rcMoveSelect[ST_Music].top), DPOINT( m_rcMoveSelect[ST_Music].left, m_rcMoveSelect[ST_Music].top), 300, 1);

			CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_SET,TRUE,0);

			break;
		}
	case IDC_BT_SOUND_NO_VOICE:
		{
			m_bCurrentVoice[ST_Sound] = TRUE;
			m_btVoice[ST_Sound].ShowWindow(m_bCurrentVoice[ST_Sound] == TRUE);
			m_btNoVoice[ST_Sound].ShowWindow(m_bCurrentVoice[ST_Sound] != TRUE);

			if( !m_bCurrentVoice[ST_Sound] )
				m_stuMoveFish[ST_Sound].BeginMove( DPOINT(m_rcMoveSelect[ST_Sound].left, m_rcMoveSelect[ST_Sound].top), DPOINT( SELECT_BEGIN,m_rcMoveSelect[ST_Sound].top), 300, 1);
			else
				m_stuMoveFish[ST_Sound].BeginMove( DPOINT(SELECT_BEGIN, m_rcMoveSelect[ST_Sound].top), DPOINT( m_rcMoveSelect[ST_Sound].left, m_rcMoveSelect[ST_Sound].top), 300, 1);

			CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_SET,TRUE,0);

			break;
		}
	case IDC_BT_MUSIC_VOICE:
		{
			m_bCurrentVoice[ST_Music] = FALSE;
			m_btVoice[ST_Music].ShowWindow(m_bCurrentVoice[ST_Music] == TRUE);
			m_btNoVoice[ST_Music].ShowWindow(m_bCurrentVoice[ST_Music] != TRUE);

			if( !m_bCurrentVoice[ST_Music] )
				m_stuMoveFish[ST_Music].BeginMove( DPOINT(m_rcMoveSelect[ST_Music].left, m_rcMoveSelect[ST_Music].top), DPOINT( SELECT_BEGIN,m_rcMoveSelect[ST_Music].top), 300, 1);
			else
				m_stuMoveFish[ST_Music].BeginMove( DPOINT(SELECT_BEGIN, m_rcMoveSelect[ST_Music].top), DPOINT( m_rcMoveSelect[ST_Music].left, m_rcMoveSelect[ST_Music].top), 300, 1);

			CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_SET,TRUE,0);

			break;
		}
	case IDC_BT_SOUND_VOICE:
		{
			m_bCurrentVoice[ST_Sound] = FALSE;
			m_btVoice[ST_Sound].ShowWindow(m_bCurrentVoice[ST_Sound] == TRUE);
			m_btNoVoice[ST_Sound].ShowWindow(m_bCurrentVoice[ST_Sound] != TRUE);

			if( !m_bCurrentVoice[ST_Sound] )
				m_stuMoveFish[ST_Sound].BeginMove( DPOINT(m_rcMoveSelect[ST_Sound].left, m_rcMoveSelect[ST_Sound].top), DPOINT( SELECT_BEGIN,m_rcMoveSelect[ST_Sound].top), 300, 1);
			else
				m_stuMoveFish[ST_Sound].BeginMove( DPOINT(SELECT_BEGIN, m_rcMoveSelect[ST_Sound].top), DPOINT( m_rcMoveSelect[ST_Sound].left, m_rcMoveSelect[ST_Sound].top), 300, 1);

			CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_SET,TRUE,0);

			break;
		}
		// 默认配置
	case IDC_BT_DEFAULT_KEY:
		{
			m_EditStart.SetKey(VK_RETURN);
			m_EditLineCountUp.SetKey(VK_UP);
			m_EditLineCountDown.SetKey(VK_DOWN);
			m_EditLineNumberUp.SetKey(VK_RIGHT);
			m_EditLineNumberDown.SetKey(VK_LEFT);

			break;
		}
	case IDC_BT_OK:
		{
			ShowWindow(false);
			for ( int i = 0; i < ST_MAX; ++i )
				m_nCurrentVolume[i] = -(INT)((DOUBLE)(SELECT_END - m_rcMoveSelect[i].left)/(DOUBLE)(SELECT_END - SELECT_BEGIN) * (DOUBLE)MAX_VOLUME);

			CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_SET,TRUE,TRUE);
			break;
		}
	case IDC_BT_CLOSE:
		{
			ShowWindow(false);

			CopyMemory(m_bCurrentVoice, m_bLastVoice, sizeof(m_bCurrentVoice));
			CopyMemory(m_nCurrentVolume, m_nLastVolume, sizeof(m_nCurrentVolume));

			m_EditStart.SetKey(m_nLastStartKey);
			m_EditLineCountUp.SetKey(m_nLastLineCountUpKey);
			m_EditLineCountDown.SetKey(m_nLastLineCountDownKey);
			m_EditLineNumberUp.SetKey(m_nLastLineNumberUpKey);
			m_EditLineNumberDown.SetKey(m_nLastLineNumberDownKey);

			CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_SET,TRUE,0);

			break;
		}

	}
}

// 绘画窗口
VOID CWindowSet::OnEventDrawWindow( CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos )
{
	// 背景
	m_ImageBack.DrawImage( pD3DDevice, nXOriginPos, nYOriginPos );

	// 声音调节栏
	for( INT i = 0; i < ST_MAX; ++i )
	{
		int nXPos = 0;
		int nYPos = 0;
		CPoint ptMove;
		if ( m_stuMoveFish[i].bMove )
		{
			ptMove.SetPoint( nXOriginPos + (int)m_stuMoveFish[i].ptPosition.x ,nYOriginPos + m_rcMoveSelect[i].top);
		}
		else if ( !m_bCurrentVoice[i] )
		{
			ptMove.SetPoint( nXOriginPos + SELECT_BEGIN ,nYOriginPos + m_rcMoveSelect[i].top);	
		}
		else
		{
			ptMove.SetPoint(  nXOriginPos + m_rcMoveSelect[i].left ,nYOriginPos + m_rcMoveSelect[i].top);
		}

		// 标杆条
		if ( (ptMove.x + 10) - (nXOriginPos + 218) > 0 )
		{
			m_ImageSetBars.DrawImage(pD3DDevice, nXOriginPos + 218 ,nYOriginPos + m_rcMoveSelect[i].top + 8, (ptMove.x + 10) - (nXOriginPos + 218), 10, 0 ,0);
		}

		// 选择点
		if( m_bMouseDown[i] )
		{
			m_ImageSetMove.DrawImageIndex(pD3DDevice, ptMove.x, ptMove.y, 1);
		}
		else if ( m_bMouseMove[i] )
		{
			m_ImageSetMove.DrawImageIndex(pD3DDevice, ptMove.x, ptMove.y, 2);
		}
		else
		{
			m_ImageSetMove.DrawImageIndex(pD3DDevice, ptMove.x, ptMove.y, 0);
		}
	}
}

// 显示窗口
VOID CWindowSet::ShowWindow( bool bVisible )
{
	if( bVisible )
	{
		int nMusicBegin = SELECT_END - (INT)((DOUBLE)(0 - m_nCurrentVolume[ST_Music]) / (DOUBLE)MAX_VOLUME * (SELECT_END - SELECT_BEGIN));
		int nSoundBegin = SELECT_END - (INT)((DOUBLE)(0 - m_nCurrentVolume[ST_Sound]) / (DOUBLE)MAX_VOLUME * (SELECT_END - SELECT_BEGIN));
		m_rcMoveSelect[ST_Music].SetRect(nMusicBegin , MUSIC_Y_POS, nMusicBegin + 23, MUSIC_Y_POS + 29);
		m_rcMoveSelect[ST_Sound].SetRect(nSoundBegin , SOUND_Y_POS, nSoundBegin + 23, SOUND_Y_POS + 29);

		CopyMemory(m_nLastVolume, m_nCurrentVolume, sizeof(m_nLastVolume));
		CopyMemory(m_bLastVoice, m_bCurrentVoice, sizeof(m_bLastVoice));

		m_nLastStartKey = m_EditStart.GetKey();
		m_nLastLineCountUpKey = m_EditLineCountUp.GetKey();
		m_nLastLineCountDownKey = m_EditLineCountDown.GetKey();
		m_nLastLineNumberUpKey = m_EditLineNumberUp.GetKey();
		m_nLastLineNumberDownKey = m_EditLineNumberDown.GetKey();

		for ( int i = 0 ; i < ST_MAX; ++i )
		{
			m_btVoice[i].ShowWindow(m_bCurrentVoice[i] == TRUE);
			m_btNoVoice[i].ShowWindow(m_bCurrentVoice[i] != TRUE);
		}

		m_EditStart.ShowWindow(SW_SHOW);
		m_EditLineCountUp.ShowWindow(SW_SHOW);
		m_EditLineCountDown.ShowWindow(SW_SHOW);
		m_EditLineNumberUp.ShowWindow(SW_SHOW);
		m_EditLineNumberDown.ShowWindow(SW_SHOW);
	}
	else
	{
		m_EditStart.ShowWindow(SW_HIDE);
		m_EditLineCountUp.ShowWindow(SW_HIDE);
		m_EditLineCountDown.ShowWindow(SW_HIDE);
		m_EditLineNumberUp.ShowWindow(SW_HIDE);
		m_EditLineNumberDown.ShowWindow(SW_HIDE);
	}


	CVirtualWindow::ShowWindow(bVisible);
}