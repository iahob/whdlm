#ifndef GUID_37582BCB_F8DF_4819_899A_411C8EEBBA8B
#define GUID_37582BCB_F8DF_4819_899A_411C8EEBBA8B

#pragma once

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// �����С��
class DEXTER_D3D_CLASS CD3DSize
{
	// ��������
public:
	float				fWidth;
	float				fHeight;

	// ��������
public:
	CD3DSize();
	CD3DSize( CD3DSize& Right );
	CD3DSize( float fW, float fH );
	~CD3DSize();

	// ��������
public:
	bool operator==(CD3DSize& size);
	bool operator!=(CD3DSize& size);
	void operator+=(CD3DSize& size);
	void operator-=(CD3DSize& size);
	CD3DSize operator+(CD3DSize& size);
	CD3DSize operator-(CD3DSize& size);
	CD3DSize operator-();

	// ���ܺ���
public:
	void SetSize(float fW, float fH);
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif