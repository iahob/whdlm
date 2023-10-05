#ifndef GUID_37582BCB_F8DF_4819_899A_411C8EEBBA8B
#define GUID_37582BCB_F8DF_4819_899A_411C8EEBBA8B

#pragma once

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 区域大小类
class DEXTER_D3D_CLASS CD3DSize
{
	// 变量声明
public:
	float				fWidth;
	float				fHeight;

	// 函数定义
public:
	CD3DSize();
	CD3DSize( CD3DSize& Right );
	CD3DSize( float fW, float fH );
	~CD3DSize();

	// 操作函数
public:
	bool operator==(CD3DSize& size);
	bool operator!=(CD3DSize& size);
	void operator+=(CD3DSize& size);
	void operator-=(CD3DSize& size);
	CD3DSize operator+(CD3DSize& size);
	CD3DSize operator-(CD3DSize& size);
	CD3DSize operator-();

	// 功能函数
public:
	void SetSize(float fW, float fH);
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif