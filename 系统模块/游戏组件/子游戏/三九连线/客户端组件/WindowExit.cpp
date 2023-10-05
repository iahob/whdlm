#include "StdAfx.h"
#include ".\windowexit.h"

//////////////////////////////////////////////////////////////////////////
#define	 IDC_BT_SAVE_EXIT							501				// �����˳�
#define	 IDC_BT_NO_SAVE_EXIT						502				// �������˳�
#define  IDC_BT_COLSE								407				// �رմ���
//////////////////////////////////////////////////////////////////////////


CWindowExit::CWindowExit(void)
{
}

CWindowExit::~CWindowExit(void)
{
}

// ������Ϣ
VOID CWindowExit::OnWindowCreate( CD3DDevice * pD3DDevice )
{
	// ��������
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	// ͼƬ����
	m_ImageBack.LoadImage(pD3DDevice, hResInstance, TEXT("EXIT_BACK"), TEXT("EXIT") );
	m_ImageNumer.LoadImage(pD3DDevice, hResInstance, TEXT("LINE_SHOW"), TEXT("NUMBERPNG"));
	m_btClosed.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_COLSE,GetVirtualEngine(),this);

	// ��ť����
	m_btSaveExit.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_SAVE_EXIT,GetVirtualEngine(),this);
	m_btNoSaveExit.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_NO_SAVE_EXIT,GetVirtualEngine(),this);

	m_btSaveExit.SetButtonImage(pD3DDevice,TEXT("BT_BAO_LIU"),TEXT("EXIT"),hResInstance);
	m_btNoSaveExit.SetButtonImage(pD3DDevice,TEXT("BT_FANG_QI"),TEXT("EXIT"),hResInstance);
	m_btClosed.SetButtonImage(pD3DDevice,TEXT("BT_CWIN_CLOSE"),TEXT("BTPNG"),hResInstance);

	// ��ȡ��С
	CSize SizeGameScore;
	SizeGameScore.SetSize(m_ImageBack.GetWidth(),m_ImageBack.GetHeight());

	// �ƶ�����
	SetWindowPos(0,0,SizeGameScore.cx,SizeGameScore.cy,0);

	// ��ťλ��
	m_btSaveExit.SetWindowPos( SizeGameScore.cx /2 - 3 - 184, SizeGameScore.cy - 75, 0, 0, SWP_NOSIZE);
	m_btNoSaveExit.SetWindowPos( SizeGameScore.cx /2 + 3, SizeGameScore.cy - 75, 0, 0, SWP_NOSIZE);
	m_btClosed.SetWindowPos( SizeGameScore.cx - 16, 5, 0, 0, SWP_NOSIZE);

	// ���ñ���
	m_plPlayScore = NULL;
	m_plPlayWinLose = NULL;
}

// λ����Ϣ
VOID CWindowExit::OnWindowPosition( CD3DDevice * pD3DDevice )
{

}

// ����¼�
VOID CWindowExit::OnEventMouse( UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos )
{

}

// ��ť�¼�
VOID CWindowExit::OnEventButton( UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos )
{
	// �رմ���
	switch ( uButtonID )
	{
	case IDC_BT_SAVE_EXIT:
		{
			CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_EXIT, TRUE, 0);
			ShowWindow(false);
		}
		break;
	case IDC_BT_NO_SAVE_EXIT:
		{
			CGameFrameEngine::GetInstance()->SendMessage(IDM_GAME_EXIT, FALSE, 0);
			ShowWindow(false);
		}
		break;
	case IDC_BT_COLSE:
		{
			// ��������
			if ( g_GameOption.m_bSoundVoice && g_GameOption.m_nCurrentVolumeSound > -MAX_VOLUME )
				CSound::_Object()->PlaySound( _T("/Sound/BUTTON_SOUND.wav"));

			ShowWindow(false);
		}
		break;
	}
}


// �滭����
VOID CWindowExit::OnEventDrawWindow( CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos )
{
	// ����
	m_ImageBack.DrawImage( pD3DDevice, nXOriginPos, nYOriginPos );

	// ��Ϣ
	TCHAR szInfo[128];

	if ( m_plPlayScore != NULL )
	{
		_sntprintf(szInfo, sizeof(szInfo), TEXT("%I64d"), *m_plPlayScore );
		DrawNumber( pD3DDevice, &m_ImageNumer, TEXT("-0123456789"), szInfo, nXOriginPos + 388, nYOriginPos + 80, DT_RIGHT );
	}

	if ( m_plPlayWinLose != NULL )
	{

		LONGLONG lPlayWinLose = (*m_plPlayWinLose);
		_sntprintf(szInfo, sizeof(szInfo), TEXT("%I64d"), ((lPlayWinLose < 0) ? 0 : lPlayWinLose) );
		DrawNumber( pD3DDevice, &m_ImageNumer, TEXT("-0123456789"), szInfo, nXOriginPos + 388, nYOriginPos + 120, DT_RIGHT );
	}
}

// �滭����
void CWindowExit::DrawNumber( CD3DDevice * pGLDevice , CD3DTexture* ImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/, BYTE cbAlpha /*= 255*/ )
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
