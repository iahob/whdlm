#ifndef GUID_2F56194F_BC61_42B3_A114_7904DDD70012
#define GUID_2F56194F_BC61_42B3_A114_7904DDD70012

#pragma once

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ������
class CD3DMatrix;

// 3D������
class DEXTER_D3D_CLASS CD3DVector : public D3DXVECTOR3
{
	// ���캯��
public:
	CD3DVector();
	CD3DVector( const float * fValue );
	CD3DVector( const D3DVECTOR* pValue );
	CD3DVector( const D3DVECTOR& vValue );
	CD3DVector( const float fX, const float fY, const float fZ );

	// ���Ժ���
public:
	// ��������
	void SetVector( const float fX, const float fY, const float fZ );
	// ƫ������
	void OffsetVector( const float fX, const float fY, const float fZ );
	// ��ȡ����
	float VectorLength();


	// ��������
public:
	// ��ʽ��
	CD3DVector& VectorNormalize();
	// ����任
	CD3DVector& MatrixTransform( CD3DMatrix* pD3DMatrix );

	// �ȽϺ���
public:
	// �Ƚϴ�С
	bool operator<( CD3DVector& Vector );
	bool operator>( CD3DVector& Vector);
	bool operator==( CD3DVector& Vector );
	bool operator!=( CD3DVector& Vector );
};

// ����������
float DEXTER_D3D_API VectorDistance( CD3DVector ValueOne, CD3DVector ValueTwo );

// �㵽��ľ���
float DEXTER_D3D_API VectorToPlane( CD3DVector PlaneOne, CD3DVector PlaneTwo, CD3DVector PlaneThree, CD3DVector Vector );

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif