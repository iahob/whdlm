#include "StdAfx.h"
#include ".\gameframeviewdexter.h"

// ��ʱ��
#define	IDI_TIME_UPDATA						int_max			

// ��Ϣע��
BEGIN_MESSAGE_MAP(CGameFrameViewDexter, CGameFrameView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SETTEXT,OnSetTextMesage)
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// ���캯��
CGameFrameViewDexter::CGameFrameViewDexter() : m_GameClientView( this )
{
	// ���ñ���
	m_bInitD3D = false;
}

// ��������
CGameFrameViewDexter::~CGameFrameViewDexter()
{

}

// ���ý���
VOID CGameFrameViewDexter::ResetGameView()
{
	// ���ý���
	m_GameClientView.ResetGameView();
}

// �����ؼ�
VOID CGameFrameViewDexter::RectifyControl( INT nWidth, INT nHeight )
{
	// �����ɹ�
	if( m_bInitD3D )
	{
		// �����ؼ�
		m_GameClientView.SetWindowPos( 0, 0, nWidth, nHeight, 0 );
	}
}

// �������
VOID CGameFrameViewDexter::InvalidGameView( INT nXPos, INT nYPos, INT nWidth, INT nHeight )
{

}

// �����豸
void CGameFrameViewDexter::OnInitDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight)
{
	// ��������
	m_GameClientView.ActiveWindow( _T("GameMainWindow"), CRect(0, 0, nWidth, nHeight), WS_VISIBLE, 0, NULL );

	// ����������
	dex::CDirectUIEngine::_Object()->SetWindowMain( &m_GameClientView );

	// ���ö�ʱ��
	SetTimer( IDI_TIME_UPDATA, 1, NULL );
}

// ��ʧ�豸
void CGameFrameViewDexter::OnLostDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight)
{

}

// �����豸
void CGameFrameViewDexter::OnResetDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight)
{

}

// ���Ƶƹ�
void CGameFrameViewDexter::OnControlLight(dex::CD3DDevice * pD3DDevice)
{

}

// ��С��Ϣ
VOID CGameFrameViewDexter::OnSize( UINT nType, INT cx, INT cy )
{
	// ��Ч����
	if ( cx > 0L && cy > 0L )
	{
		// δ����
		if ( !m_bInitD3D )
		{
			//�����豸
			if ( !dex::CD3DDevice::_Object()->CreateD3DDevice(GetSafeHwnd(), this, dex::CDirectUIEngine::_Object() ) )
			{
				ASSERT(FALSE);
				return;
			}
		}
		else
		{
			// �����豸
			dex::CD3DDevice::_Object()->ResetD3DDevice();
		}
	}

	// ����ִ��
	__super::OnSize( nType, cx, cy );
}

// ��ʱ����Ϣ
void CGameFrameViewDexter::OnTimer(UINT nIDEvent)
{
	// ˢ�½���
	if ( nIDEvent == IDI_TIME_UPDATA )
	{
		dex::CD3DDevice::_Object()->RenderD3DDevice();
	}

	// ����ִ��
	__super::OnTimer( nIDEvent );
}

// ��Ϣ����
BOOL CGameFrameViewDexter::PreTranslateMessage( MSG * pMsg )
{
	// ����ִ��
	return __super::PreTranslateMessage(pMsg);
}

// Ĭ�ϻص�
LRESULT CGameFrameViewDexter::DefWindowProc( UINT nMessage, WPARAM wParam, LPARAM lParam )
{
	// ������
	if ( dex::CDirectUIEngine::_Object()->DefWindowProc(nMessage,wParam,lParam) )
		return 0L;

	// ����ִ��
	return __super::DefWindowProc(nMessage,wParam,lParam);
}

//������Ϣ
LRESULT CGameFrameViewDexter::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	// Ĭ�ϵ���
	LRESULT lResult = DefWindowProc(WM_SETTEXT, wParam, lParam);

	// ��ȡ����
	CString StrTextName( (const tchar *)lParam );

	// ���ñ���
	m_GameClientView.SetGameText( StrTextName );

	return lResult;
}

// �Ҽ�̸��
void CGameFrameViewDexter::OnRButtonUp(UINT nFlags, CPoint point)
{

}

// ������Ϣ
bool CGameFrameViewDexter::OnEventInsureMessage( word wSubCmdID, VOID * pData, word wDataSize )
{
	return m_GameClientView.OnEventInsureMessage( wSubCmdID, pData, wDataSize );
}

