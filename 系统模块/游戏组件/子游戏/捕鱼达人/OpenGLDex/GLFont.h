#pragma once

// GL文件
#include "OpenGL.h"
#include "OpenglEngineHead.h"

// 字体信息数组
struct mapFTextInfo
{
	UCHAR*	pBits;					// BITMAP指针
	INT		nWidtht;				// 文字宽度
	INT		nHeight;				// 文字高度
	DWORD	dwRetentionTime;		// 停留时间

	mapFTextInfo( UCHAR* pParBits, INT nParWidtht, INT nParHeight )
	{
		pBits = pParBits;
		nWidtht = nParWidtht;
		nHeight = nParHeight;
		dwRetentionTime = GetTickCount();
	}
};
typedef map< CString , mapFTextInfo >			mapText;
class CMapText
{
protected:
	mapText										m_mapText;

public:
	CMapText(){}
	~CMapText(){}

public:
	mapText*	GetObject() { return &m_mapText; }
};


class DEXTER_OPENGL_CLASS CGLFont  
{

protected:
	HFONT							m_hFont;				// 字体
	HFONT							m_hOldFont;				// 旧字体
	HDC								m_Hdc;					// 窗口句柄
	CMapText*						m_mapText;

	// 
public:
	CGLFont();
	virtual ~CGLFont();

	// 创建函数
public:
	// 删除字体
	bool DeleteFont();
	// 删除缓存
	bool DeleteCache();
	// 创建字体
	bool CreateFont(int nHeight, int nWeight, const TCHAR * lpszFaceName);
	// 是否为空
	bool IsEmpty();

	// 输出字体
public:
	// 输出字体
	bool DrawText(CGLDevice* pOpenGL, const TCHAR * pszString, CRect rcDraw, UINT uFormat, COLORREF GLColor);
	// 输出文字
	bool TextOut(CGLDevice* pOpenGL, const TCHAR * pszString, int nPosX, int nPosY, COLORREF GLColor);
	// 获取文字大小
	CSize GetTextSize(const TCHAR * pszString);

};

