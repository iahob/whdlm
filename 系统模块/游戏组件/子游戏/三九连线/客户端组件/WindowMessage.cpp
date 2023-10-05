#include "StdAfx.h"
#include ".\windowmessage.h"

// ��ť����
#define IDC_BT_OK					101

// ��Ϣ����
BEGIN_WHDUI_MESSAGE_MAP(CWindowMessage, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_KEYDOWN()	
	ON_CONRTOL_BN_CLICKED( IDC_BT_OK, OnClickedOK )
END_WHDUI_MESSAGE_MAP()

// ���캯��
CWindowMessage::CWindowMessage( const tchar * pszMessage )
{
	// ������Ϣ
	m_StrMessage = pszMessage;
}

// ��������
CWindowMessage::~CWindowMessage()
{
}


// ���ڴ��� 
void CWindowMessage::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ��������
	m_D3DFont.CreateFont( 12, 400, _T("����") );

	// ����ͼƬ
	m_TextureBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("��ʾ_����") );

	// ������ť
	m_ButtonOK.ActiveWindow( NULL, CRect( 122, 136, 0, 0 ), WS_VISIBLE, IDC_BT_OK, this );
	m_ButtonOK.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_��ʾ_ȷ��") );

	// ���õ���ʱ��
	SetShowTime( 200 );

	// ���ര��
	CRect RectParent;
	AfxGetMainWnd()->GetClientRect( RectParent );

	// ���ô��ڴ�С
	SetWindowPos( RectParent.left + RectParent.Width() / 2 - m_TextureBack.GetWidth() / 2, RectParent.top + RectParent.Height() / 2 - m_TextureBack.GetHeight() / 2, m_TextureBack.GetWidth(), m_TextureBack.GetHeight(), 0 );
}

// ���ڶ���
void CWindowMessage::OnWindowMovie()
{

}

// ���ڻ滭 
void CWindowMessage::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// �滭����
	m_TextureBack.SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureBack.DrawTexture( pD3DDevice, 0, 0 );

	// �滭��Ϣ
	m_D3DFont.DrawText( pD3DDevice, m_StrMessage, CRect(40, 27 + 1, nWidth - 40, 127 + 1), D3DCOLOR_ARGB(255,0,0,0), DT_CENTER|DT_VCENTER|DT_EDITCONTROL|DT_WORDBREAK );
	m_D3DFont.DrawText( pD3DDevice, m_StrMessage, CRect(40, 27, nWidth - 40, 127), D3DCOLOR_ARGB(255,255,255,255), DT_CENTER|DT_VCENTER|DT_EDITCONTROL|DT_WORDBREAK );
}

// �������� 
void CWindowMessage::OnKeyDown( uint nChar, uint nRepCnt, uint nFlags )
{
	// �رմ���
	if( nChar == VK_ESCAPE || nChar == VK_RETURN || nChar == VK_SPACE )
	{
		// �رմ���
		EndModal(0, dex::WindowShow_Size );

		// ������Ч
		CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );
	}
}	


// ȷ����ť
void CWindowMessage::OnClickedOK()
{
	// �رմ���
	EndModal(0, dex::WindowShow_Size );

	// ������Ч
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );
}

