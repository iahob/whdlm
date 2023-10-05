#include "StdAfx.h"
#include ".\windowgameover.h"

//////////////////////////////////////////////////////////////////////////
#define	 IDC_BT_CLOSE							301				// �رհ�ť

//////////////////////////////////////////////////////////////////////////
CWindowGameOver::CWindowGameOver(void)
{
	m_lPlayScore = 0l;
	m_lPlayWinLose = 0l;
}

CWindowGameOver::~CWindowGameOver(void)
{
}

// ������Ϣ
VOID CWindowGameOver::OnWindowCreate( CD3DDevice * pD3DDevice )
{
	// ��������
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	// ͼƬ����
	m_ImageBack.LoadImage(pD3DDevice, hResInstance, TEXT("GAME_OVER_BACK"), TEXT("GAME_OVER") );
	m_ImageNumer.LoadImage(pD3DDevice, hResInstance, TEXT("LINE_SHOW"), TEXT("NUMBERPNG"));

	// ��ť����
	m_btClosed.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_CLOSE,GetVirtualEngine(),this);
	m_btClosed.SetButtonImage(pD3DDevice,TEXT("BT_FAN_HUI"),TEXT("GAME_OVER"),hResInstance);

	// ��ȡ��С
	CSize SizeGameScore;
	SizeGameScore.SetSize(m_ImageBack.GetWidth(),m_ImageBack.GetHeight());

	// �ƶ�����
	SetWindowPos(0,0,SizeGameScore.cx,SizeGameScore.cy,0);

	// ��ťλ��
	m_btClosed.SetWindowPos(100,170,0,0,SWP_NOSIZE);
}

// λ����Ϣ
VOID CWindowGameOver::OnWindowPosition( CD3DDevice * pD3DDevice )
{
	
}

// ����¼�
VOID CWindowGameOver::OnEventMouse( UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos )
{

}

// ��ť�¼�
VOID CWindowGameOver::OnEventButton( UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos )
{
	// �رմ���
	if (uButtonID==IDC_BT_CLOSE)
	{
		// ��������
		if ( g_GameOption.m_bSoundVoice && g_GameOption.m_nCurrentVolumeSound > -MAX_VOLUME )
			CSound::_Object()->PlaySound( _T("/Sound/BUTTON_SOUND.wav"));

		ShowWindow(false);
		CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_OVER, 0, 0);

		return;
	}
}

// �滭����
VOID CWindowGameOver::OnEventDrawWindow( CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos )
{
	// ����
	m_ImageBack.DrawImage( pD3DDevice, nXOriginPos, nYOriginPos );

	// ��Ϣ
	TCHAR szInfo[128];
	_sntprintf(szInfo, sizeof(szInfo), TEXT("%I64d"), m_lPlayScore );
	DrawNumber( pD3DDevice, &m_ImageNumer, TEXT("-0123456789"), szInfo, nXOriginPos + 388, nYOriginPos + 80, DT_RIGHT );

	_sntprintf(szInfo, sizeof(szInfo), TEXT("%I64d"), ((m_lPlayWinLose < 0) ? 0 : m_lPlayWinLose) );
	DrawNumber( pD3DDevice, &m_ImageNumer, TEXT("-0123456789"), szInfo, nXOriginPos + 388, nYOriginPos + 120, DT_RIGHT );
}

// �滭����
void CWindowGameOver::DrawNumber( CD3DDevice * pGLDevice , CD3DTexture* ImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/, BYTE cbAlpha /*= 255*/ )
{
	// ������Դ
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