#ifndef GUID_3FBCEB4D_9310_469C_A32D_BC51378F21C1
#define GUID_3FBCEB4D_9310_469C_A32D_BC51378F21C1

#pragma once

#include "D3DColor.h"
#include "D3DDevice.h"
#include "D3DTexture.h"
#include "D3DEffectPool.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 声明类
class CLapseCount;

// 字体信息
struct tagFontInfo
{
	uint					nDrawTime;
	CD3DTexture				D3DTextureText;
};

// 纹理定义
typedef CMap<CString, const tchar *, tagFontInfo *, tagFontInfo *> CMapFont;

// D3D字体
class DEXTER_D3D_CLASS CD3DFont
{
	// 字体对象
public:
	HDC									m_hDC;					// GDI HDC
	CFont								m_GDIFont;				// GDI字体
	CMapFont							m_MapFont;				// 绘画信息

	// 静态变量
public:
	static CLapseCount					m_LapseDelete;			// 删除计时
	static CArray< CD3DFont * >			m_ArrayD3DFontEx;		// 类指针

	// 函数定义
public:
	// 构造函数
	CD3DFont();
	// 析构函数
	virtual ~CD3DFont();

	// 判断函数
public:
	// 是否为空
	bool IsNull();

	// 创建函数
public:
	// 删除字体
	bool DeleteFont();
	// 创建字体
	bool CreateFont(int Height, uint Weight, const tchar * pFaceName);
	// 创建字体
	bool CreateFont(CONST LOGFONT * lpLogFont);
	// 创建字体
	bool CreateFont(CFont& font);
	// 创建字体
	bool CreateFont(CD3DFont& font);

	// 功能函数
public:
	// 获得一个字符大小
	CSize GetTextSize( tchar szText );
	// 获得一句话大小
	CSize GetTextSize( const tchar * pszText );
	// 获取字体信息
	int GetLogFont( LOGFONT* pLogFont );

	// 绘画字体
public:
	// 绘画字体
	void DrawText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DColor dwColor, uint nFormat);
	// 绘画字体
	void DrawText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DColor dwColor, uint nFormat);

	// 描边绘画
public:
	// 描边绘画字体
	void DrawBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DColor dwColor, CD3DColor dwBorderColor, uint nFormat);
	// 描边绘画字体
	void DrawBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DColor dwColor, CD3DColor dwBorderColor, uint nFormat);

	// 渐变绘画
public:
	// 渐变绘画字体
	void DrawGradientText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DColor dwTopColor, CD3DColor dwBottomColor, uint nFormat);
	// 渐变绘画字体
	void DrawGradientText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DColor dwTopColor, CD3DColor dwBottomColor, uint nFormat);

	// 渐变后描边绘画
public:
	// 渐变后描边绘画字体
	void DrawGradientBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DColor dwTopColor, CD3DColor dwBottomColor, CD3DColor dwBorderColor, uint nFormat);
	// 渐变后描边绘画字体
	void DrawGradientBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DColor dwTopColor, CD3DColor dwBottomColor, CD3DColor dwBorderColor, uint nFormat);

	// 纹理绘画
public:
	// 纹理绘画字体
	void DrawTextureText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DTexture* pD3DTexture, uint nFormat);
	// 纹理绘画字体
	void DrawTextureText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DTexture* pD3DTexture, uint nFormat);

	// 纹理叠加然后描边绘画
public:
	// 纹理叠加然后描边绘画字体
	void DrawTextureBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DTexture* pD3DTexture, CD3DColor dwBorderColor, uint nFormat);
	// 纹理叠加然后描边绘画字体
	void DrawTextureBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DTexture* pD3DTexture, CD3DColor dwBorderColor, uint nFormat);

	// 静态函数
public:
	// 删除多余超时绘画
	static void DeleteOvertime();

	// 内部函数
protected:
	// 创建DC
	void CreateFontDC();
	// 查找文字
	tagFontInfo* SearchTextTexture( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, uint nFormat );
	// 创建文字
	tagFontInfo* CreateTextTexture( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, uint nFormat );
	// 绘画字体
	void DrawTextTexture( CD3DDevice * pD3DDevice, tagFontInfo* pFontInfo, const tchar * pszString, LPRECT lpRect, CD3DColor dwColor, uint nFormat);
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif