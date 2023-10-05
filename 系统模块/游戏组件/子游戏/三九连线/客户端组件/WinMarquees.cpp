#include "StdAfx.h"
#include ".\winmarquees.h"

// 定时器
#define IDI_TIME_UPDATA				101


// 构造函数
CWinMarquees::CWinMarquees()
{
}

// 析构函数
CWinMarquees::~CWinMarquees()
{
	// 遍历信息
	for ( int nIndex = 0; nIndex < m_ArrayWinMarquees.GetCount(); ++nIndex )
	{
		// 获取信息
		tagWinMarqueesInfo *& pWinMarqueesInfo = m_ArrayWinMarquees.GetAt( nIndex );

		// 删除信息
		SafeDelete(pWinMarqueesInfo);
	}
}

// 创建函数
int CWinMarquees::Create( CD3DDevice * pD3DDevice )
{
	// 清空缓冲
	m_SizeBuffer.SetSize( 0, 0 );

	// 创建字体
	m_FontInfo.CreateFont( 15, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体") );
	m_D3DFontInfo.CreateFont( pD3DDevice, 15, 400, _T("宋体") );

	return 1;
}


// 绘画背景
void CWinMarquees::DrawView( CD3DDevice * pD3DDevice )
{
	// 窗口大小
	CSize SizeClient(m_RectDraw.Width(), m_RectDraw.Height());

	// 绘画背景
	pD3DDevice->FillRect( m_RectDraw, D3DCOLOR_XRGB(139, 93, 21) );

	// 遍历信息
	for ( int nIndex = 0; nIndex < m_ArrayWinMarquees.GetCount(); ++nIndex )
	{
		// 获取信息
		tagWinMarqueesInfo * pWinMarqueesInfo = m_ArrayWinMarquees.GetAt( nIndex );

		// 字符长度
		int nStringLen = 0;

		// 遍历数据
		for ( int nStringIndex = 0; nStringIndex < pWinMarqueesInfo->ArrayStringInfo.GetCount(); ++nStringIndex )
		{
			// 获取信息
			tagStringInfo & TStringInfo = pWinMarqueesInfo->ArrayStringInfo.GetAt(nStringIndex);

			// 生成区域
			CRect RectString( pWinMarqueesInfo->nStringBegin + nStringLen, m_RectDraw.top, pWinMarqueesInfo->nStringBegin + nStringLen + TStringInfo.Size.cx, m_RectDraw.bottom );

			// 判断区域
			CRect RectIntersect(0, 0, 0, 0);
			RectIntersect.IntersectRect( RectString, m_RectDraw );

			// 绘画信息
			if( !RectIntersect.IsRectEmpty() )
				DrawTextString( pD3DDevice, TStringInfo.Info, RectString, DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE, TStringInfo.Color, D3DCOLOR_XRGB(55, 25, 0) );

			// 偏移位置
			nStringLen += TStringInfo.Size.cx;
		}
	}
}

// 动画驱动
void CWinMarquees::CartoonMovie()
{
	// 遍历信息
	for ( int nIndex = 0; nIndex < m_ArrayWinMarquees.GetCount(); )
	{
		// 获取信息
		tagWinMarqueesInfo * pWinMarqueesInfo = m_ArrayWinMarquees.GetAt( nIndex );

		// 更新位置
		pWinMarqueesInfo->dStringBegin -= 1.5f;
		pWinMarqueesInfo->nStringBegin = static_cast<int>(pWinMarqueesInfo->dStringBegin);

		// 判断位置
		if( pWinMarqueesInfo->nStringBegin + pWinMarqueesInfo->nStringLen < 0 )
		{
			// 显示次数
			pWinMarqueesInfo->nShowCount++;

			// 最多显示3次
			if ( pWinMarqueesInfo->nShowCount >= 10 )
			{
				// 删除信息
				SafeDelete( pWinMarqueesInfo );
				m_ArrayWinMarquees.RemoveAt( nIndex );
				continue;
			}

			// 更新位置
			pWinMarqueesInfo->nStringBegin = GetWinMarqueesLastLen();
			pWinMarqueesInfo->dStringBegin = pWinMarqueesInfo->nStringBegin;
		}

		// 下一个
		++nIndex;
	}
}

// 添加信息
void CWinMarquees::AppendWinMarquees( const tchar * pszString, int nGemIndex, int nGemCount, longlong lGemScore )
{
	// 定义指针
	tagWinMarqueesInfo * pWinMarqueesInfo = new tagWinMarqueesInfo;

	// 归零信息
	pWinMarqueesInfo->nShowCount = 0;
	pWinMarqueesInfo->dStringBegin = 0.0;
	pWinMarqueesInfo->nStringBegin = 0;
	pWinMarqueesInfo->nStringLen = 0;
	pWinMarqueesInfo->ArrayStringInfo.RemoveAll();

	// 名字
	CString StrName;
	tagStringInfo TStringName;
	StrName.Format( _T("玩家 %s 连中 "), pszString);
	GenerateString(StrName, D3DCOLOR_XRGB(255, 209, 60), TStringName);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringName );
	pWinMarqueesInfo->nStringLen += TStringName.Size.cx;

	// 倍数
	tagStringInfo TStringCount;
	GenerateString(nGemCount, D3DCOLOR_XRGB(255, 54, 13), TStringCount);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringCount );
	pWinMarqueesInfo->nStringLen += TStringCount.Size.cx;

	// 个
	tagStringInfo TStringGe;
	GenerateString( _T("个 "), D3DCOLOR_XRGB(255, 209, 60), TStringGe);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringGe );
	pWinMarqueesInfo->nStringLen += TStringGe.Size.cx;

	// 宝石名字
	tchar szGemName[GEMINDEX_MAX][10] = 
	{ 
		_T("白玉"), _T("碧玉"), _T("黑玉"), _T("玛瑙"), _T("琥珀"),
		_T("祖母绿"), _T("猫眼石"), _T("紫水晶"), _T("翡翠"), _T("珍珠"),
		_T("红宝石"), _T("绿宝石"), _T("黄宝石"), _T("蓝宝石"), _T("砖石"),
		_T("钻头")
	};
	tagStringInfo TStringGenNmae;
	GenerateString( szGemName[nGemIndex], D3DCOLOR_XRGB(255, 54, 13), TStringGenNmae);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringGenNmae );
	pWinMarqueesInfo->nStringLen += TStringGenNmae.Size.cx;

	// 获得
	tagStringInfo TStringHuoDe;
	GenerateString( _T(" 获得 "), D3DCOLOR_XRGB(255, 209, 60), TStringHuoDe);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringHuoDe );
	pWinMarqueesInfo->nStringLen += TStringHuoDe.Size.cx;

	// 分数
	tagStringInfo TStringScore;
	GenerateString(lGemScore, D3DCOLOR_XRGB(255, 54, 13), TStringScore);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringScore );
	pWinMarqueesInfo->nStringLen += TStringScore.Size.cx;

	// 点数
	tagStringInfo TStringFenShu;
	GenerateString( _T(" 点数！"), D3DCOLOR_XRGB(255, 209, 60), TStringFenShu);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringFenShu );
	pWinMarqueesInfo->nStringLen += TStringFenShu.Size.cx;

	// 更新位置
	pWinMarqueesInfo->nStringLen += 50;
	pWinMarqueesInfo->nStringBegin = GetWinMarqueesLastLen();
	pWinMarqueesInfo->dStringBegin = pWinMarqueesInfo->nStringBegin;

	// 添加信息
	m_ArrayWinMarquees.Add( pWinMarqueesInfo );
}

// 生成字符串
void CWinMarquees::GenerateString( longlong lString, colorref Color, tagStringInfo & TStringInfo )
{
	CString String;
	String.Format( _T("%I64d"), lString );
	GenerateString( String, Color, TStringInfo );
}

// 生成字符串
void CWinMarquees::GenerateString( const tchar * pszString, colorref Color, tagStringInfo & TStringInfo )
{
	// 创建缓冲
	Static CImage ImageBuffer;
	if ( ImageBuffer.IsNull() )
		ImageBuffer.Create(128, 50, 32);

	// 获取DC
	CImageDC ImageDC(ImageBuffer);
	CDC * pBufferDC = CDC::FromHandle(ImageDC);

	// 设置字体
	CFont * pFont = pBufferDC->SelectObject( &m_FontInfo );

	// 获取字符大小
	CRect RectText(0, 0, 1, 1 );
	pBufferDC->DrawText( pszString, -1, &RectText, DT_CALCRECT|DT_NOCLIP|DT_SINGLELINE );

	// 还原信息
	pBufferDC->SelectObject(pFont);

	// 生成信息
	TStringInfo.Color = Color;
	TStringInfo.Size.SetSize( RectText.Width(), RectText.Height() );
	_sntprintf(TStringInfo.Info, CountArray(TStringInfo.Info), TEXT("%s"), pszString);
	
}

// 获取跑马末尾长度
int CWinMarquees::GetWinMarqueesLastLen()
{
	// 最末位置
	int nLastSite = m_RectDraw.right;

	// 遍历信息
	for ( int nIndex = 0; nIndex < m_ArrayWinMarquees.GetCount(); ++nIndex )
	{
		// 获取信息
		tagWinMarqueesInfo * pWinMarqueesInfo = m_ArrayWinMarquees.GetAt( nIndex );

		// 更新位置
		nLastSite = max(nLastSite, pWinMarqueesInfo->nStringBegin + pWinMarqueesInfo->nStringLen);
	}

	return nLastSite;
}

// 绘画字符
void CWinMarquees::DrawTextString( CD3DDevice * pD3DDevice, const tchar * pszString, CRect rcDraw, uint nFormat, colorref crText, colorref crFrame )
{
	// 变量定义
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	// 绘画边框
	for (int nIndex=0;nIndex<CountArray(nXExcursion);nIndex++)
	{
		// 计算位置
		CRect rcFrame( rcDraw );
		rcFrame.OffsetRect( nXExcursion[nIndex], nYExcursion[nIndex] );

		// 绘画字符
		m_D3DFontInfo.DrawText( pszString, rcFrame, crFrame, nFormat );
	}

	// 绘画字符
	m_D3DFontInfo.DrawText( pszString, rcDraw, crText, nFormat );
}

