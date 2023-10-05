#ifndef GUID_FFDBE2CD_65F9_4ca6_90D4_B8A92F01C7F3
#define GUID_FFDBE2CD_65F9_4ca6_90D4_B8A92F01C7F3

#pragma once

#include "D3DDevice.h"
#include "DexterD3DHead.h"
#include "D3DTexture.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 字体结构
struct tagFreeTypeTexture
{
	int			  nTop;
	CD3DTexture	  D3DTexture;
};

// 缓冲结构
struct tagFreeTypeBuffer
{
	int			  nReferenceCount;
	byte *		  pDataBuffer;
};

// 纹理定义
typedef CMap<uint, uint, tagFreeTypeTexture *, tagFreeTypeTexture *> CMapFreeTypeTexture;
typedef CMap< CString, const tchar *, tagFreeTypeBuffer *, tagFreeTypeBuffer * > CMapFreeTypeBuffer;

// 自由字体
class DEXTER_D3D_CLASS CD3DFreeType
{
	// 定义信息
public:
	int										m_nTextInterval;				// 字体间隔
	void *									m_pFTLib;						// 字体指针
	void *									m_pFTFace;						// 字体指针
	CString									m_StrBuffer;					// 缓冲关键字
	CMapFreeTypeTexture						m_MapFreeTypeTexture;			// 字体纹理
	Static CMapFreeTypeBuffer				m_MapFreeTypeBuffer;			// 字体缓冲

	// 函数定义
public:
	// 构造函数
	CD3DFreeType();
	// 析构函数
	~CD3DFreeType();

	// 判断函数
public:
	// 是否为空
	bool IsNull();

	// 创建函数
public:
	// 删除字体
	bool DeleteFont();
	// 创建字体
	bool CreateFont( int nHeight, int nWidth, const tchar * pFaceName );
	// 创建字体
	bool CreateFont( int nHeight, int nWidth, const byte * pData, uint nSize );
	// 创建字体
	bool CreateFont( int nHeight, int nWidth, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName );

	// 功能函数
public:
	// 设置字体间隔
	void SetTextInterval( int nTextInterval ) { m_nTextInterval = nTextInterval; }
	// 获得一个字符大小
	CSize GetTextSize( CD3DDevice * pD3DDevice, tchar szText );
	// 获得一句话大小
	CSize GetTextSize( CD3DDevice * pD3DDevice, const tchar * pszString );

	// 绘画字体
public:
	// 绘画字体
	void DrawText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DColor dwColor, uint nFormat);
	// 绘画字体
	void DrawText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DColor dwColor, uint nFormat);

	// 字体函数
public:
	// 创建文字
	tagFreeTypeTexture * CreateTextTexture( CD3DDevice * pD3DDevice, tchar szText );
	// 创建文字
	void CreateTextTexture( CD3DDevice * pD3DDevice, const tchar * pszString );
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif
