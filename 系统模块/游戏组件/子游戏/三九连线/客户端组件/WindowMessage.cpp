#include "StdAfx.h"
#include ".\windowmessage.h"

// 按钮定义
#define IDC_BT_OK					101

// 消息定义
BEGIN_WHDUI_MESSAGE_MAP(CWindowMessage, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_KEYDOWN()	
	ON_CONRTOL_BN_CLICKED( IDC_BT_OK, OnClickedOK )
END_WHDUI_MESSAGE_MAP()

// 构造函数
CWindowMessage::CWindowMessage( const tchar * pszMessage )
{
	// 设置消息
	m_StrMessage = pszMessage;
}

// 析构函数
CWindowMessage::~CWindowMessage()
{
}


// 窗口创建 
void CWindowMessage::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 创建字体
	m_D3DFont.CreateFont( 12, 400, _T("宋体") );

	// 加载图片
	m_TextureBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("提示_背景") );

	// 创建按钮
	m_ButtonOK.ActiveWindow( NULL, CRect( 122, 136, 0, 0 ), WS_VISIBLE, IDC_BT_OK, this );
	m_ButtonOK.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_提示_确定") );

	// 设置弹出时间
	SetShowTime( 200 );

	// 父类窗口
	CRect RectParent;
	AfxGetMainWnd()->GetClientRect( RectParent );

	// 设置窗口大小
	SetWindowPos( RectParent.left + RectParent.Width() / 2 - m_TextureBack.GetWidth() / 2, RectParent.top + RectParent.Height() / 2 - m_TextureBack.GetHeight() / 2, m_TextureBack.GetWidth(), m_TextureBack.GetHeight(), 0 );
}

// 窗口动画
void CWindowMessage::OnWindowMovie()
{

}

// 窗口绘画 
void CWindowMessage::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 绘画背景
	m_TextureBack.SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureBack.DrawTexture( pD3DDevice, 0, 0 );

	// 绘画信息
	m_D3DFont.DrawText( pD3DDevice, m_StrMessage, CRect(40, 27 + 1, nWidth - 40, 127 + 1), D3DCOLOR_ARGB(255,0,0,0), DT_CENTER|DT_VCENTER|DT_EDITCONTROL|DT_WORDBREAK );
	m_D3DFont.DrawText( pD3DDevice, m_StrMessage, CRect(40, 27, nWidth - 40, 127), D3DCOLOR_ARGB(255,255,255,255), DT_CENTER|DT_VCENTER|DT_EDITCONTROL|DT_WORDBREAK );
}

// 按键点下 
void CWindowMessage::OnKeyDown( uint nChar, uint nRepCnt, uint nFlags )
{
	// 关闭窗口
	if( nChar == VK_ESCAPE || nChar == VK_RETURN || nChar == VK_SPACE )
	{
		// 关闭窗口
		EndModal(0, dex::WindowShow_Size );

		// 播放音效
		CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );
	}
}	


// 确定按钮
void CWindowMessage::OnClickedOK()
{
	// 关闭窗口
	EndModal(0, dex::WindowShow_Size );

	// 播放音效
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );
}

