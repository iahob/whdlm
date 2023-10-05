#ifndef GUID_C88A427A_F7CB_4586_ADD2_7EA391B77E1D
#define GUID_C88A427A_F7CB_4586_ADD2_7EA391B77E1D

#pragma once

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 颜色类
class DEXTER_D3D_CLASS CD3DColor : public D3DXCOLOR
{
	// 构造函数
public:
	CD3DColor();
	CD3DColor( D3DCOLOR dwARGB );
	CD3DColor( const D3DXCOLOR* pValue);
	CD3DColor( const CD3DColor& cValue );
	CD3DColor( const D3DCOLORVALUE& cValue );
	CD3DColor( float fA, float fR, float fG, float fB );
	CD3DColor( byte cbA, byte cbR, byte cbG, byte cbB );
	CD3DColor( int nA, int nR, int nG, int nB );

	// 获取函数
public:
	// 获取色值
	byte GetA();
	byte GetR();
	byte GetG();
	byte GetB();

	// 获取浮点色值
	float GetAf();
	float GetRf();
	float GetGf();
	float GetBf();

	// 设置函数
public:
	// 设置颜色
	CD3DColor& SetColor( D3DCOLOR dwARGB );
	// 设置颜色
	CD3DColor& SetColor( const D3DXCOLOR* pValue);
	// 设置颜色
	CD3DColor& SetColor( const CD3DColor& cValue );
	// 设置颜色
	CD3DColor& SetColor( const D3DCOLORVALUE& cValue );
	// 设置颜色
	CD3DColor& SetColor( float fA, float fR, float fG, float fB );
	// 设置颜色
	CD3DColor& SetColor( byte cbA, byte cbR, byte cbG, byte cbB );
	// 设置颜色
	CD3DColor& SetColor( int nA, int nR, int nG, int nB );
	// 设置A值
	CD3DColor& SetA( byte cbA );
	// 设置A值
	CD3DColor& SetA( float fA );
	// 设置R值
	CD3DColor& SetR( byte cbR );
	// 设置R值
	CD3DColor& SetR( float fR );
	// 设置G值
	CD3DColor& SetG( byte cbG );
	// 设置G值
	CD3DColor& SetG( float fG );
	// 设置B值
	CD3DColor& SetB( byte cbB );
	// 设置B值
	CD3DColor& SetB( float fB );

	// 功能函数
public:
	// 判断透明
	bool IsTransparent();
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif