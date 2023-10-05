#include "StdAfx.h"
#include ".\windowhelp.h"

// 按钮定义
#define IDC_BT_CLOSE				202


// 消息定义
BEGIN_WHDUI_MESSAGE_MAP(CWindowHelp, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_CONRTOL_BN_CLICKED( IDC_BT_CLOSE, OnClickedClose )
END_WHDUI_MESSAGE_MAP()

// 构造函数
CWindowHelp::CWindowHelp()
{
}

// 析构函数
CWindowHelp::~CWindowHelp()
{
}


// 窗口创建 
void CWindowHelp::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 创建图片
	m_TextureBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("帮助_背景") );

	// 创建按钮
	m_ButtonClose.ActiveWindow( NULL, CRect( 574, 93, 0, 0 ), WS_VISIBLE, IDC_BT_CLOSE, this );
	m_ButtonClose.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_窗口_关闭") );

	// 设置窗口大小
	SetWindowPos( 0, 0, m_TextureBack.GetWidth(), m_TextureBack.GetHeight(), SWP_NOMOVE );
}

// 窗口动画
void CWindowHelp::OnWindowMovie()
{
}

// 窗口绘画 
void CWindowHelp::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 绘画背景
	m_TextureBack.SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureBack.DrawTexture( pD3DDevice, 0, 0 );
}

// 关闭
void CWindowHelp::OnClickedClose()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );
	
	// 关闭窗口
	ShowWindow( false, dex::WindowShow_Size );
}
