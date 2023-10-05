#include "StdAfx.h"
#include ".\windowgameover.h"

//////////////////////////////////////////////////////////////////////////
#define	 IDC_BT_CLOSE							301				// 关闭按钮

//////////////////////////////////////////////////////////////////////////
CWindowGameOver::CWindowGameOver(void)
{
	m_lPlayScore = 0l;
	m_lPlayWinLose = 0l;
}

CWindowGameOver::~CWindowGameOver(void)
{
}

// 创建消息
VOID CWindowGameOver::OnWindowCreate( CD3DDevice * pD3DDevice )
{
	// 变量定义
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	// 图片载入
	m_ImageBack.LoadImage(pD3DDevice, hResInstance, TEXT("GAME_OVER_BACK"), TEXT("GAME_OVER") );
	m_ImageNumer.LoadImage(pD3DDevice, hResInstance, TEXT("LINE_SHOW"), TEXT("NUMBERPNG"));

	// 按钮载入
	m_btClosed.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_CLOSE,GetVirtualEngine(),this);
	m_btClosed.SetButtonImage(pD3DDevice,TEXT("BT_FAN_HUI"),TEXT("GAME_OVER"),hResInstance);

	// 获取大小
	CSize SizeGameScore;
	SizeGameScore.SetSize(m_ImageBack.GetWidth(),m_ImageBack.GetHeight());

	// 移动窗口
	SetWindowPos(0,0,SizeGameScore.cx,SizeGameScore.cy,0);

	// 按钮位置
	m_btClosed.SetWindowPos(100,170,0,0,SWP_NOSIZE);
}

// 位置消息
VOID CWindowGameOver::OnWindowPosition( CD3DDevice * pD3DDevice )
{
	
}

// 鼠标事件
VOID CWindowGameOver::OnEventMouse( UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos )
{

}

// 按钮事件
VOID CWindowGameOver::OnEventButton( UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos )
{
	// 关闭窗口
	if (uButtonID==IDC_BT_CLOSE)
	{
		// 播放声音
		if ( g_GameOption.m_bSoundVoice && g_GameOption.m_nCurrentVolumeSound > -MAX_VOLUME )
			CSound::_Object()->PlaySound( _T("/Sound/BUTTON_SOUND.wav"));

		ShowWindow(false);
		CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_OVER, 0, 0);

		return;
	}
}

// 绘画窗口
VOID CWindowGameOver::OnEventDrawWindow( CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos )
{
	// 背景
	m_ImageBack.DrawImage( pD3DDevice, nXOriginPos, nYOriginPos );

	// 信息
	TCHAR szInfo[128];
	_sntprintf(szInfo, sizeof(szInfo), TEXT("%I64d"), m_lPlayScore );
	DrawNumber( pD3DDevice, &m_ImageNumer, TEXT("-0123456789"), szInfo, nXOriginPos + 388, nYOriginPos + 80, DT_RIGHT );

	_sntprintf(szInfo, sizeof(szInfo), TEXT("%I64d"), ((m_lPlayWinLose < 0) ? 0 : m_lPlayWinLose) );
	DrawNumber( pD3DDevice, &m_ImageNumer, TEXT("-0123456789"), szInfo, nXOriginPos + 388, nYOriginPos + 120, DT_RIGHT );
}

// 绘画数字
void CWindowGameOver::DrawNumber( CD3DDevice * pGLDevice , CD3DTexture* ImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/, BYTE cbAlpha /*= 255*/ )
{
	// 加载资源
	INT nNumberHeight=ImageNumber->GetHeight();
	INT nNumberWidth=ImageNumber->GetWidth()/lstrlen(szImageNum);

	if ( uFormat == DT_CENTER )
	{
		nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
	}
	else if ( uFormat == DT_RIGHT )
	{
		nXPos -= lstrlen(szOutNum) * nNumberWidth;
	}

	for ( INT i = 0; i < lstrlen(szOutNum); ++i )
	{
		for ( INT j = 0; j < lstrlen(szImageNum); ++j )
		{
			if ( szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0' )
			{
				ImageNumber->DrawImage(pGLDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight, cbAlpha);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}