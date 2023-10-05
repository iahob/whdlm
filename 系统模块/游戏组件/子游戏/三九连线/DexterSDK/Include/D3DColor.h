#ifndef GUID_C88A427A_F7CB_4586_ADD2_7EA391B77E1D
#define GUID_C88A427A_F7CB_4586_ADD2_7EA391B77E1D

#pragma once

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ��ɫ��
class DEXTER_D3D_CLASS CD3DColor : public D3DXCOLOR
{
	// ���캯��
public:
	CD3DColor();
	CD3DColor( D3DCOLOR dwARGB );
	CD3DColor( const D3DXCOLOR* pValue);
	CD3DColor( const CD3DColor& cValue );
	CD3DColor( const D3DCOLORVALUE& cValue );
	CD3DColor( float fA, float fR, float fG, float fB );
	CD3DColor( byte cbA, byte cbR, byte cbG, byte cbB );
	CD3DColor( int nA, int nR, int nG, int nB );

	// ��ȡ����
public:
	// ��ȡɫֵ
	byte GetA();
	byte GetR();
	byte GetG();
	byte GetB();

	// ��ȡ����ɫֵ
	float GetAf();
	float GetRf();
	float GetGf();
	float GetBf();

	// ���ú���
public:
	// ������ɫ
	CD3DColor& SetColor( D3DCOLOR dwARGB );
	// ������ɫ
	CD3DColor& SetColor( const D3DXCOLOR* pValue);
	// ������ɫ
	CD3DColor& SetColor( const CD3DColor& cValue );
	// ������ɫ
	CD3DColor& SetColor( const D3DCOLORVALUE& cValue );
	// ������ɫ
	CD3DColor& SetColor( float fA, float fR, float fG, float fB );
	// ������ɫ
	CD3DColor& SetColor( byte cbA, byte cbR, byte cbG, byte cbB );
	// ������ɫ
	CD3DColor& SetColor( int nA, int nR, int nG, int nB );
	// ����Aֵ
	CD3DColor& SetA( byte cbA );
	// ����Aֵ
	CD3DColor& SetA( float fA );
	// ����Rֵ
	CD3DColor& SetR( byte cbR );
	// ����Rֵ
	CD3DColor& SetR( float fR );
	// ����Gֵ
	CD3DColor& SetG( byte cbG );
	// ����Gֵ
	CD3DColor& SetG( float fG );
	// ����Bֵ
	CD3DColor& SetB( byte cbB );
	// ����Bֵ
	CD3DColor& SetB( float fB );

	// ���ܺ���
public:
	// �ж�͸��
	bool IsTransparent();
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif