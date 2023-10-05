#include "StdAfx.h"
#include ".\windowhelp.h"

// ��ť����
#define IDC_BT_CLOSE				202


// ��Ϣ����
BEGIN_WHDUI_MESSAGE_MAP(CWindowHelp, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_CONRTOL_BN_CLICKED( IDC_BT_CLOSE, OnClickedClose )
END_WHDUI_MESSAGE_MAP()

// ���캯��
CWindowHelp::CWindowHelp()
{
}

// ��������
CWindowHelp::~CWindowHelp()
{
}


// ���ڴ��� 
void CWindowHelp::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ����ͼƬ
	m_TextureBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("����_����") );

	// ������ť
	m_ButtonClose.ActiveWindow( NULL, CRect( 574, 93, 0, 0 ), WS_VISIBLE, IDC_BT_CLOSE, this );
	m_ButtonClose.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_����_�ر�") );

	// ���ô��ڴ�С
	SetWindowPos( 0, 0, m_TextureBack.GetWidth(), m_TextureBack.GetHeight(), SWP_NOMOVE );
}

// ���ڶ���
void CWindowHelp::OnWindowMovie()
{
}

// ���ڻ滭 
void CWindowHelp::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// �滭����
	m_TextureBack.SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureBack.DrawTexture( pD3DDevice, 0, 0 );
}

// �ر�
void CWindowHelp::OnClickedClose()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );
	
	// �رմ���
	ShowWindow( false, dex::WindowShow_Size );
}
