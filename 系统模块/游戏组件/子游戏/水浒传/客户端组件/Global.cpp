#include "StdAfx.h"
#include "Global.h"

// 声明变量
CGlobal						CGlobal::g_Global;

// 构造函数
CGlobal::CGlobal()
{
	// 当前目录
	tchar szDirectoryPath[MAX_PATH + 1] = { _T("") }; 
	GetModuleFileName(NULL, szDirectoryPath, MAX_PATH); 
	PathRemoveFileSpec(szDirectoryPath);

	// 生成DLL目录
	tchar szResourcePath[MAX_PATH] = { _T("") };
	_sntprintf(szResourcePath, CountArray(szResourcePath), TEXT("%s\\WaterMargin\\%s"), szDirectoryPath, _T("WaterMarginResource.dat"));

	// 加载DLL
	hModuleResource = LoadLibrary(szResourcePath);
}


// 析构函数
CGlobal::~CGlobal()
{
	// 释放DLL
	FreeLibrary( hModuleResource );
}

// 绘画数字
CRect CGlobal::DrawNumber( dex::CD3DDevice * pD3DDevice , CD3DTextureEx & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/, byte cbAlpha /*= 255 */ )
{
	// 数字信息
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), lOutNum );
	return DrawNumber( pD3DDevice, ImageNumber, pszImageNum, StrNumber, nXPos, nYPos, uFormat, cbAlpha );
}


// 绘画数字
CRect CGlobal::DrawNumber( dex::CD3DDevice * pD3DDevice , CD3DTextureEx & ImageNumber, const tchar * pszImageNum, const tchar * pszOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/, byte cbAlpha /*= 255*/ )
{
	// 信息长度
	int nImageNumberLen = lstrlen(pszImageNum);
	int nOutNumberLen = lstrlen(pszOutNum);

	// 加载资源
	int nNumberHeight = ImageNumber.GetHeight();
	int nNumberWidth = ImageNumber.GetWidth() / nImageNumberLen;

	// 偏移位置
	if ( (uFormat & DN_CENTER) == DN_CENTER )
		nXPos -= (int)(((DOUBLE)(nOutNumberLen) / 2.0) * nNumberWidth);
	else if ( (uFormat & DT_RIGHT) == DT_RIGHT )
		nXPos -= nOutNumberLen * nNumberWidth;

	if ( (uFormat & DN_VCENTER) == DN_VCENTER )
		nYPos -= nNumberHeight / 2;
	else if ( (uFormat & DN_BOTTOM) == DN_BOTTOM )
		nYPos -= nNumberHeight;

	// 绘画矩形
	CRect RrctDraw( CPoint(nXPos, nYPos), CSize(nNumberWidth * nOutNumberLen, nNumberHeight) );

	// 绘画数字
	for ( int nOutIndex = 0; nOutIndex < nOutNumberLen; ++nOutIndex )
	{
		for ( int nImageIndex = 0; nImageIndex < nImageNumberLen; ++nImageIndex )
		{
			if ( pszOutNum[nOutIndex] == pszImageNum[nImageIndex] && pszOutNum[nOutIndex] != '\0' )
			{
				ImageNumber.DrawTexture(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, nImageIndex * nNumberWidth, 0, nNumberWidth, nNumberHeight, cbAlpha);
				nXPos += nNumberWidth;
				break;
			}
		}
	}

	return RrctDraw;
}
// 数字矩形
CRect CGlobal::NumberRect( CD3DTextureEx & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/ )
{
	// 数字信息
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), lOutNum );
	return NumberRect( ImageNumber, pszImageNum, StrNumber, nXPos, nYPos, uFormat );
}

// 数字矩形
CRect CGlobal::NumberRect( CD3DTextureEx & ImageNumber, const tchar * pszImageNum, const tchar * pszOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/ )
{
	// 信息长度
	int nImageNumberLen = lstrlen(pszImageNum);
	int nOutNumberLen = lstrlen(pszOutNum);

	// 加载资源
	int nNumberHeight = ImageNumber.GetHeight();
	int nNumberWidth = ImageNumber.GetWidth() / nImageNumberLen;

	// 偏移位置
	if ( (uFormat & DN_CENTER) == DN_CENTER )
		nXPos -= (int)(((DOUBLE)(nOutNumberLen) / 2.0) * nNumberWidth);
	else if ( (uFormat & DT_RIGHT) == DT_RIGHT )
		nXPos -= nOutNumberLen * nNumberWidth;

	if ( (uFormat & DN_VCENTER) == DN_VCENTER )
		nYPos -= nNumberHeight / 2;
	else if ( (uFormat & DN_BOTTOM) == DN_BOTTOM )
		nYPos -= nNumberHeight;

	// 绘画矩形
	return CRect( CPoint(nXPos, nYPos), CSize(nNumberWidth * nOutNumberLen, nNumberHeight) );
}

// 添加逗号
CString CGlobal::AppendComma( CString & StrNumber )
{
	// 重新设置
	int nIndex = StrNumber.GetLength() - 3;
	while ( nIndex > 0 )
	{
		// 添加逗号
		StrNumber.Insert( nIndex, ',' );
		nIndex -= 3;

		// 判断符号
		if( nIndex == 1 && StrNumber.GetAt(0) == '-' )
		{
			break;
		}
	}

	return StrNumber;
}

// 添加逗号
CString CGlobal::AppendComma( __int64 nNumber )
{
	// 构造信息
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), nNumber );

	// 返回信息
	return AppendComma( StrNumber );
}

// 弹出消息
int64 CGlobal::MessageBox( const tchar * pszMessage, EnumWindowMessageType nWindowMessageType, dex::CDirectUIWindow * pWindowParent /*= NULL*/ )
{
	return 0;
}
