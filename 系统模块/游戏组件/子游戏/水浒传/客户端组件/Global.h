#pragma once

#include "D3DTextureex.h"

// 绘画数字
#define DN_TOP                      0x00000000
#define DN_LEFT                     0x00000000
#define DN_CENTER                   0x00000001
#define DN_RIGHT                    0x00000002
#define DN_VCENTER                  0x00000004
#define DN_BOTTOM                   0x00000008
	
// 提示类型
enum EnumWindowMessageType
{
	WindowMessageType_Ok,
	WindowMessageType_Ok_Cancel,
};

// 全部变量
class CGlobal
{

	// 资源句柄
public:
	HMODULE								hModuleResource;			// DLL句柄

	// 静态变量
private:
	Static CGlobal						g_Global;					// 全局变量

	// 类函数
private:
	// 构造函数
	CGlobal();
	// 析构函数
	~CGlobal();

public:
	// 返回对象
	inline Static CGlobal* _Object() { return &g_Global; }

	// 界面函数
public: 
	// 绘画数字
	CRect DrawNumber( dex::CD3DDevice * pD3DDevice , CD3DTextureEx & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat = DN_CENTER|DN_TOP, byte cbAlpha = 255 );
	// 绘画数字
	CRect DrawNumber( dex::CD3DDevice * pD3DDevice , CD3DTextureEx & ImageNumber, const tchar * pszImageNum, const tchar * pszOutNum, int nXPos, int nYPos, uint uFormat = DN_CENTER|DN_TOP, byte cbAlpha = 255 );
	// 数字矩形
	CRect NumberRect( CD3DTextureEx & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat = DN_CENTER|DN_TOP );
	// 数字矩形
	CRect NumberRect( CD3DTextureEx & ImageNumber, const tchar * pszImageNum, const tchar * pszOutNum, int nXPos, int nYPos, uint uFormat = DN_CENTER|DN_TOP );
	// 添加逗号
	CString AppendComma( CString & StrNumber );
	// 添加逗号
	CString AppendComma( __int64 nNumber );

	// 消息函数
public: 
	// 弹出消息
	int64 MessageBox( const tchar * pszMessage, EnumWindowMessageType nWindowMessageType, dex::CDirectUIWindow * pWindowParent = NULL );

};
