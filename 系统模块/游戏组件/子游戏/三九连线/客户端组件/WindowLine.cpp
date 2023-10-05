#include "StdAfx.h"
#include ".\windowline.h"

// 消息定义
BEGIN_WHDUI_MESSAGE_MAP(CWindowLine, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
END_WHDUI_MESSAGE_MAP()

// 构造函数
CWindowLine::CWindowLine()
{
	// 设置变量
	m_nMovieCount = 0;
	ZeroMemory( m_bWinLine, sizeof(m_bWinLine) );
}

// 析构函数
CWindowLine::~CWindowLine()
{
}


// 窗口创建 
void CWindowLine::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 创建图片
	m_TextureLineWidth.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("赢线_横") );
	m_TextureLineHeight.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("赢线_竖") );
	m_TextureLineOblique1.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("赢线_斜1") );
	m_TextureLineOblique2.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("赢线_斜2") );

	// 设置背景颜色
	SetBackColor( D3DCOLOR_ARGB(0, 200, 200, 200) );
}

// 窗口动画
void CWindowLine::OnWindowMovie()
{
	// 动画计数
	m_nMovieCount += 15;
}

// 窗口绘画 
void CWindowLine::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 渐隐
	byte cbAlpha = (byte)(abs( (long)(m_nMovieCount / 10 % 100 - 50) ) + 205);

	// 绘画赢线
	if( m_bWinLine[LINE_WIDTH_1] )
		m_TextureLineWidth.DrawTexture(pD3DDevice, 15, 99, cbAlpha );
	if( m_bWinLine[LINE_WIDTH_2] )
		m_TextureLineWidth.DrawTexture(pD3DDevice, 15, 247, cbAlpha );
	if( m_bWinLine[LINE_WIDTH_3] )
		m_TextureLineWidth.DrawTexture(pD3DDevice, 15, 394, cbAlpha );
	if( m_bWinLine[LINE_HEIGHT_4] )
		m_TextureLineHeight.DrawTexture(pD3DDevice, 100, 9, cbAlpha );
	if( m_bWinLine[LINE_HEIGHT_5] )
		m_TextureLineHeight.DrawTexture(pD3DDevice, 260, 9, cbAlpha );
	if( m_bWinLine[LINE_HEIGHT_6] )
		m_TextureLineHeight.DrawTexture(pD3DDevice, 420, 9, cbAlpha );
	if( m_bWinLine[LINE_OBLIQUE_7] )
		m_TextureLineOblique1.DrawTexture(pD3DDevice, 36, 8, cbAlpha );
	if( m_bWinLine[LINE_OBLIQUE_8] )
		m_TextureLineOblique2.DrawTexture(pD3DDevice, 37, 31, cbAlpha );
}

// 设置赢线
void CWindowLine::SetWinLine( bool bWinLine[LINE_INDEX_MAX] )
{
	// 设置信息
	m_nMovieCount = 50;
	CopyMemory( m_bWinLine, bWinLine, sizeof(m_bWinLine) );
}

// 清除赢线
void CWindowLine::ClearWinLine()
{
	ZeroMemory( m_bWinLine, sizeof(m_bWinLine) );
}
