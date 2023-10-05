#include "StdAfx.h"
#include ".\windowroad.h"

// 按钮定义
#define IDC_BT_CLOSE				202

// 消息定义
BEGIN_WHDUI_MESSAGE_MAP(CWindowRoad, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_LBUTTONDOWN()
	ON_CONRTOL_BN_CLICKED( IDC_BT_CLOSE, OnClickedClose )
END_WHDUI_MESSAGE_MAP()

// 构造函数
CWindowRoad::CWindowRoad()
{
}

// 析构函数
CWindowRoad::~CWindowRoad()
{
}

// 窗口创建 
void CWindowRoad::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 字体信息
	m_D3DFont.CreateFont( 12, 400, _T("宋体") );

	// 创建图片
	m_TextureBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("路单_背景") );
	m_TextureElementNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("中奖_数字") );
	m_TextureElementMultip.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("中奖_乘号") );

	// 创建按钮
	m_ButtonClose.ActiveWindow( NULL, CRect( 720, 43, 0, 0 ), WS_VISIBLE, IDC_BT_CLOSE, this );
	m_ButtonClose.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_窗口_关闭") );

	// 设置窗口大小
	SetWindowPos( 0, 0, m_TextureBack.GetWidth(), m_TextureBack.GetHeight(), SWP_NOMOVE );
}

// 窗口动画
void CWindowRoad::OnWindowMovie()
{

}

// 窗口绘画 
void CWindowRoad::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 绘画背景
	m_TextureBack.SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureBack.DrawTexture( pD3DDevice, 0, 0 );

	// 绘画路单
	for ( int nIndex = 0; nIndex < m_ArrayRoad.GetCount(); ++nIndex )
	{
		// 获取信息
		int64 nRoadValue = m_ArrayRoad.GetAt( nIndex );
		int nMultipleIndex = (int)nRoadValue;
		int nMultipleCount = (int)(nRoadValue >> 32);

		// 计算位置
		CPoint PointImage( 38 + nIndex / 5 * 71, 97 + nIndex % 5 * 71 );

		// 绘画图片
		CGlobal::_Object()->DrawElement( pD3DDevice, CGlobal::_Object()->MultipleToTextureElement(nMultipleIndex), PointImage.x + 5, PointImage.y + 5, 58, 58 );

		// 判断图片
		if( nMultipleIndex < MultipleThree_Max + MultipleNine_Max )
		{
			int nElementCount = CGlobal::_Object()->MultipleToElementCount( nMultipleIndex );
			CRect RectNumber = CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureElementNumber, _T("0123456789"), nElementCount, PointImage.x + 5 + 58, PointImage.y + 5 + 58, DN_RIGHT|DN_BOTTOM );
			m_TextureElementMultip.DrawTexture( pD3DDevice, RectNumber.left - m_TextureElementMultip.GetWidth(), RectNumber.bottom - m_TextureElementMultip.GetHeight() );
		}
		else
		{
			CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureElementNumber, _T("0123456789"), nMultipleCount, PointImage.x + 5 + 58, PointImage.y + 5 + 58, DN_RIGHT|DN_BOTTOM );
		}
	}
}

// 左键点下 
void CWindowRoad::OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{

}

// 关闭
void CWindowRoad::OnClickedClose()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// 关闭窗口
	ShowWindow( false, dex::WindowShow_Size );
}


// 添加路单
void CWindowRoad::AppendRoad( int nMultipleIndex )
{
	// 判断信息
	if( nMultipleIndex > MultipleThree_Max + MultipleNine_Max || nMultipleIndex < 0 )
	{
		_Assert( false && "路单信息错误" );
		return;
	}

	// 未中奖
	bool bNotWinModify = false;
	if ( m_ArrayRoad.GetCount() > 0 )
	{
		int64 & nRoadValue = m_ArrayRoad.GetAt( m_ArrayRoad.GetCount() - 1 );
		int nMultipleValue = (int)nRoadValue;
		int nMultipleCount = (int)(nRoadValue >> 32);
		if ( nMultipleValue == MultipleThree_Max + MultipleNine_Max && nMultipleIndex == MultipleThree_Max + MultipleNine_Max )
		{
			nRoadValue = ((int64)( nMultipleCount + 1 ) << 32) | nMultipleValue;
			bNotWinModify = true;
		}
	}

	// 保存信息
	if ( !bNotWinModify )
	{
		__int64 nMultipleCount = CGlobal::_Object()->MultipleToElementCount(nMultipleIndex);
		m_ArrayRoad.Add( (nMultipleCount << 32) | nMultipleIndex );
	}

	// 删除多于
	while( m_ArrayRoad.GetCount() > 50 )
	{
		m_ArrayRoad.RemoveAt( 0 );
	};
}
