#ifndef GUID_7DA0CC64_A902_40E6_A472_1099D6442F67
#define GUID_7DA0CC64_A902_40E6_A472_1099D6442F67

#pragma once

#include "DexterD3DHead.h"
#include "D3DTexture.h"
#include "D3DCamera.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 天空
enum enSkyBox
{
	SkyBox_After = 0,				// 后
	SkyBox_Before,					// 前
	SkyBox_Left,					// 左
	SkyBox_Right,					// 右
	SkyBox_Down,					// 下
	SkyBox_Up,						// 上
	SkyBox_Max,						// 最大
};

// 天空盒子
class DEXTER_D3D_CLASS CD3DSkyBox
{
	// 定义变量
public:
	float								m_fSkySize;
	CD3DTexture							m_TextureSky[SkyBox_Max];
	IDirect3DVertexBuffer9*				m_pIDirect3DVertexBuffer9;
	IDirect3DIndexBuffer9*				m_pIDirect3DIndexBuffer9;

	// 类函数
public:
	// 构造函数
	CD3DSkyBox(void);
	// 析构函数
	~CD3DSkyBox(void);

	// 功能函数
public:
	// 销毁盒子
	bool Destory();
	// 创建盒子
	bool CreateSkyBox( CD3DDevice * pD3DDevice );
	// 载图图片
	bool LoadTexture( CD3DTexture& TextureSky, enSkyBox nSkyBox );
	// 设置天空大小
	void SetSize( float fSize );
	// 绘画天空
	bool DrawSkyBox( CD3DDevice * pD3DDevice );

};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif