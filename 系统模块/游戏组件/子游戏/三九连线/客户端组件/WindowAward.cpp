#include "StdAfx.h"
#include ".\windowaward.h"


// 消息定义
BEGIN_WHDUI_MESSAGE_MAP(CWindowAward, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_LBUTTONDOWN()
END_WHDUI_MESSAGE_MAP()

// 构造函数
CWindowAward::CWindowAward()
{
	// 变量信息
	m_nMultipleType = MultipleType_Three;
	m_nMultipleIndex = 0;
	ZeroMemory(m_szPlayName, sizeof(m_szPlayName));
	m_lAwardScore = 0;
}

// 析构函数
CWindowAward::~CWindowAward()
{
}


// 窗口创建 
void CWindowAward::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 创建字体
	m_D3DFontWidth12.CreateFont( 12, 800, _T("宋体") );

	// 加载图片
	m_TextureNameBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("名字背景") );
	m_TextureLineBack[MultipleType_Three].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("获得三连") );
	m_TextureLineBack[MultipleType_Nine].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("获得九连") );
	m_TextureAwardBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("奖金") );
	m_TextureAwardNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("奖金数字") );
	m_TextureElementNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("中奖_数字") );
	m_TextureElementMultip.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("中奖_乘号") );

	// 设置窗口大小
	SetWindowPos( 0, 0, 203, 65, SWP_NOMOVE );
}

// 窗口动画
void CWindowAward::OnWindowMovie()
{
}


// 窗口绘画 
void CWindowAward::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 获取绘画
	dex::CD3DDrawBasic * pD3DDrawBasic = dex::CD3DDrawBasic::_Object();

	// 设置状态
	m_TextureLineBack[m_nMultipleType].SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureAwardBack.SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureAwardNumber.SetEffectType( dex::TextureEffect_WindowBack );

	// 绘画图标
	CGlobal::_Object()->DrawElement( pD3DDevice, CGlobal::_Object()->MultipleToTextureElement(m_nMultipleIndex), 5, nHeight / 2 - 32, 64, 64, dex::TextureEffect_WindowBack );

	// 绘画几连
	int nElementCount = CGlobal::_Object()->MultipleToElementCount( m_nMultipleIndex );
	CRect RectNumber = CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureElementNumber, _T("0123456789"), nElementCount, 5 + 64, nHeight / 2 + 32, DN_RIGHT|DN_BOTTOM );
	m_TextureElementMultip.DrawTexture( pD3DDevice, RectNumber.left - m_TextureElementMultip.GetWidth(), RectNumber.bottom - m_TextureElementMultip.GetHeight() );

	// 绘画背景
	m_TextureNameBack.DrawTexture( pD3DDevice, 75, 2 );
	m_TextureLineBack[m_nMultipleType].DrawTexture( pD3DDevice, 75, 29 );
	m_TextureAwardBack.DrawTexture( pD3DDevice, 75, 47 );
	CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureAwardNumber, _T("0123456789"), m_lAwardScore, 75 + 40, 48 );

	// 绘画名字
	m_D3DFontWidth12.DrawText( pD3DDevice, m_szPlayName, CRect( CPoint(75 + 5, 3), CSize(116, 23) ), D3DCOLOR_ARGB(255, 18, 15, 5), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	m_D3DFontWidth12.DrawText( pD3DDevice, m_szPlayName, CRect( CPoint(75 + 5, 2), CSize(116, 23) ), D3DCOLOR_ARGB(255, 247, 196, 108), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
}

// 左键点下 
void CWindowAward::OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{
}

// 设置获奖信息
bool CWindowAward::SetAwardInfo( const tchar * pszPlayName, int nMultipleIndex, longlong lAwardScore )
{
	// 清空信息
	m_nMultipleIndex = 0;
	m_nMultipleType = MultipleType_Three;
	ZeroMemory(m_szPlayName, sizeof(m_szPlayName));
	m_lAwardScore = 0;

	// 判断类型
	if ( nMultipleIndex >= 0 && nMultipleIndex < MultipleThree_Max )
	{
		// 设置倍数
		m_nMultipleType = MultipleType_Three;
		
	}
	else if( nMultipleIndex >= MultipleThree_Max && nMultipleIndex < MultipleThree_Max + MultipleNine_Max )
	{
		// 设置倍数
		m_nMultipleType = MultipleType_Nine;
	}
	else
	{
		_Assert( false && "获奖信息错误" );

		return false;
	}

	// 保存信息
	_sntprintf(m_szPlayName, CountArray(m_szPlayName), _T("%s"), pszPlayName);
	m_lAwardScore = lAwardScore;
	m_nMultipleIndex = nMultipleIndex;

	return true;
}
