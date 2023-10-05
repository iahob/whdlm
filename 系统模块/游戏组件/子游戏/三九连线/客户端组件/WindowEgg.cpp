#include "StdAfx.h"
#include ".\windowegg.h"

// 消息定义
BEGIN_WHDUI_MESSAGE_MAP(CWindowEgg, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
END_WHDUI_MESSAGE_MAP()

// 构造函数
CWindowEgg::CWindowEgg()
{
	// 动画变量
	m_nBackIndex = 0;
	m_nLightIndex = -1;
}

// 析构函数
CWindowEgg::~CWindowEgg()
{
}

// 窗口创建 
void CWindowEgg::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 创建图片
	m_TextureBack[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_1") );
	m_TextureBack[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_2") );
	m_TextureBack[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_3") );
	m_TextureBack[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_4") );
	m_TextureBack[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_5") );
	m_TextureBack[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_6") );
	m_TextureBack[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_7") );
	m_TextureBack[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_8") );
	m_TextureBack[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_9") );
	m_TextureBack[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_10") );
	m_TextureBack[10].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_11") );
	m_TextureBack[11].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_12") );
	m_TextureBack[12].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_13") );
	m_TextureBack[13].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_14") );
	m_TextureBack[14].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_15") );
	m_TextureBack[15].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_16") );
	m_TextureBack[16].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_动画_17") );
	m_TextureLight[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_特效_1") );
	m_TextureLight[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_特效_2") );
	m_TextureLight[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_特效_3") );
	m_TextureLight[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_特效_4") );
	m_TextureLight[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_特效_5") );
	m_TextureLight[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_特效_6") );
	m_TextureLight[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_特效_7") );
	m_TextureLight[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_特效_8") );
	m_TextureLight[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_特效_9") );
	m_TextureLight[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("癞子蛋"), _T("资源"), _T("癞子_特效_10") );

	//// 设置背景颜色
	//SetBackColor( D3DCOLOR_ARGB(0, 255, 200, 200) );
}

// 窗口动画
void CWindowEgg::OnWindowMovie()
{
	// 背景索引
	if ( m_nLCBackTime.GetLapseCount(90) )
	{
		// 背景动画
		m_nBackIndex = min( TEXTRUE_BACK_MAX - 1, m_nBackIndex + 1 );

		// 闪光动画
		if( m_nBackIndex == 13 )
		{
			// 开启闪光
			m_nLightIndex = 0;
			m_nLCLightTime.Initialization();
		}
	}

	// 闪光索引
	if ( m_nLightIndex != -1 && m_nLCLightTime.GetLapseCount(45) )
	{
		// 闪光动画
		m_nLightIndex = min( TEXTRUE_LIGHT_MAX, m_nLightIndex + 1 );

		// 结束动画
		if ( m_nLightIndex == TEXTRUE_LIGHT_MAX )
		{
			// 结束动画
			m_nLightIndex = -1;
		}
	}
}

// 窗口绘画 
void CWindowEgg::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 绘画闪电
	if ( m_nLightIndex != -1 )
	{
		m_TextureLight[m_nLightIndex].SetEffectType( dex::TextureEffect_WindowBack );
		m_TextureLight[m_nLightIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureLight[m_nLightIndex].GetWidth() / 2, nHeight / 2 - m_TextureLight[m_nLightIndex].GetHeight() / 2);
		m_TextureLight[m_nLightIndex].SetEffectType( dex::TextureEffect_Shine );
		m_TextureLight[m_nLightIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureLight[m_nLightIndex].GetWidth() / 2, nHeight / 2 - m_TextureLight[m_nLightIndex].GetHeight() / 2);
	}

	// 绘画图片
	m_TextureBack[m_nBackIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureBack[m_nBackIndex].GetWidth() / 2 + 12, nHeight / 2 - m_TextureBack[m_nBackIndex].GetHeight() / 2);

	// 绘画闪电
	if ( m_nLightIndex != -1 )
	{
		m_TextureLight[m_nLightIndex].SetEffectType( dex::TextureEffect_Shine );
		m_TextureLight[m_nLightIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureLight[m_nLightIndex].GetWidth() / 2, nHeight / 2 - m_TextureLight[m_nLightIndex].GetHeight() / 2);
	}

	// 绘画图标
	if( m_nBackIndex > 15 )
	{
		// 绘画信息
		CSize SizeEggElement( 180, 180 );
		CPoint PointEggElement( nWidth / 2 - ((int)m_ArrayEggElement.GetCount() * SizeEggElement.cx) / 2, nHeight / 2 - SizeEggElement.cy / 2 );
	
		// 高度偏移
		int nHeightOffSet = abs( (int)(35 - (GetTickCount() / 25 % 70)) );

		// 绘画图片
		for( int nIndex = 0; nIndex < m_ArrayEggElement.GetCount(); ++nIndex )
		{
			// 获取信息
			EnumTextureElement nTextureElement = (EnumTextureElement)m_ArrayEggElement.GetAt( nIndex );

			// 绘画信息
			CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, PointEggElement.x, PointEggElement.y + nHeightOffSet, SizeEggElement.cx, SizeEggElement.cy );

			// 偏移位置
			PointEggElement.x += SizeEggElement.cx;
		}
	}
}

// 设置癞子蛋数据
void CWindowEgg::SetEggElement( CWHArray< int > & ArrayEggElement )
{
	// 删除原有
	m_ArrayEggElement.RemoveAll();

	// 添加新的
	m_ArrayEggElement.Append( ArrayEggElement );

	// 设置变量
	m_nBackIndex = 0;
	m_nLightIndex = -1;
	m_nLCBackTime.Initialization();
	m_nLCLightTime.Initialization();
}
