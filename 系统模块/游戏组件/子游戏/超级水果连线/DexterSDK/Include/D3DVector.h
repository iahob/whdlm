#ifndef GUID_2F56194F_BC61_42B3_A114_7904DDD70012
#define GUID_2F56194F_BC61_42B3_A114_7904DDD70012

#pragma once

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 类声明
class CD3DMatrix;

// 3D向量类
class DEXTER_D3D_CLASS CD3DVector : public D3DXVECTOR3
{
	// 构造函数
public:
	CD3DVector();
	CD3DVector( const float * fValue );
	CD3DVector( const D3DVECTOR* pValue );
	CD3DVector( const D3DVECTOR& vValue );
	CD3DVector( const float fX, const float fY, const float fZ );

	// 调试函数
public:
	// 设置向量
	void SetVector( const float fX, const float fY, const float fZ );
	// 偏移向量
	void OffsetVector( const float fX, const float fY, const float fZ );
	// 获取长度
	float VectorLength();


	// 操作函数
public:
	// 格式化
	CD3DVector& VectorNormalize();
	// 矩阵变换
	CD3DVector& MatrixTransform( CD3DMatrix* pD3DMatrix );

	// 比较函数
public:
	// 比较大小
	bool operator<( CD3DVector& Vector );
	bool operator>( CD3DVector& Vector);
	bool operator==( CD3DVector& Vector );
	bool operator!=( CD3DVector& Vector );
};

// 向量相差距离
float DEXTER_D3D_API VectorDistance( CD3DVector ValueOne, CD3DVector ValueTwo );

// 点到面的距离
float DEXTER_D3D_API VectorToPlane( CD3DVector PlaneOne, CD3DVector PlaneTwo, CD3DVector PlaneThree, CD3DVector Vector );

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif