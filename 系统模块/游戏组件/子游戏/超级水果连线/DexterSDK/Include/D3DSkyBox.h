#ifndef GUID_7DA0CC64_A902_40E6_A472_1099D6442F67
#define GUID_7DA0CC64_A902_40E6_A472_1099D6442F67

#pragma once

#include "DexterD3DHead.h"
#include "D3DTexture.h"
#include "D3DCamera.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ���
enum enSkyBox
{
	SkyBox_After = 0,				// ��
	SkyBox_Before,					// ǰ
	SkyBox_Left,					// ��
	SkyBox_Right,					// ��
	SkyBox_Down,					// ��
	SkyBox_Up,						// ��
	SkyBox_Max,						// ���
};

// ��պ���
class DEXTER_D3D_CLASS CD3DSkyBox
{
	// �������
public:
	float								m_fSkySize;
	CD3DTexture							m_TextureSky[SkyBox_Max];
	IDirect3DVertexBuffer9*				m_pIDirect3DVertexBuffer9;
	IDirect3DIndexBuffer9*				m_pIDirect3DIndexBuffer9;

	// �ຯ��
public:
	// ���캯��
	CD3DSkyBox(void);
	// ��������
	~CD3DSkyBox(void);

	// ���ܺ���
public:
	// ���ٺ���
	bool Destory();
	// ��������
	bool CreateSkyBox( CD3DDevice * pD3DDevice );
	// ��ͼͼƬ
	bool LoadTexture( CD3DTexture& TextureSky, enSkyBox nSkyBox );
	// ������մ�С
	void SetSize( float fSize );
	// �滭���
	bool DrawSkyBox( CD3DDevice * pD3DDevice );

};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif