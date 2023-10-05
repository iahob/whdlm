#pragma once


// 提示信息
struct tagStringInfo
{
	colorref						Color;			// 颜色
	tchar							Info[256];		// 信息
	CSize							Size;			// 大小
};

// 赢跑马信息
struct tagWinMarqueesInfo
{
	CWHArray< tagStringInfo >		ArrayStringInfo;
	double							dStringBegin;
	int								nStringBegin;
	int								nStringLen;
	int								nShowCount;
};


// 赢跑马灯
class CWinMarquees
{
	// 信息变量
public:
	CArray< tagWinMarqueesInfo * >  m_ArrayWinMarquees;			// 跑马信息
	CFont							m_FontInfo;					// 字体信息
	CD3DFontEx						m_D3DFontInfo;				// 字体信息
	CRect							m_RectDraw;					// 绘画区域

	// 缓冲函数
public:
	CImage							m_ImageBuffer;
	CSize							m_SizeBuffer;

	// 类函数
public:
	// 构造函数
	CWinMarquees();
	// 析构函数
	~CWinMarquees();

	// 消息函数
public:
	// 创建函数
	int Create( CD3DDevice * pD3DDevice );
	// 设置区域
	void SetRect( CRect RectDraw ) { m_RectDraw = RectDraw; }
	// 绘画函数
	void DrawView( CD3DDevice * pD3DDevice );
	// 动画驱动
	void CartoonMovie();

	// 功能函数
public:
	// 添加信息
	void AppendWinMarquees( const tchar * pszString, int nGemIndex, int nGemCount, longlong lGemScore );
	// 生成字符串
	void GenerateString( const tchar * pszString, colorref Color, tagStringInfo & TStringInfo );
	// 生成字符串
	void GenerateString( longlong lString, colorref Color, tagStringInfo & TStringInfo );
	// 获取跑马末尾长度
	int GetWinMarqueesLastLen();
	// 绘画字符
	void DrawTextString( CD3DDevice * pD3DDevice,  const tchar * pszString, CRect rcDraw, uint nFormat, colorref crText, colorref crFrame);

};
