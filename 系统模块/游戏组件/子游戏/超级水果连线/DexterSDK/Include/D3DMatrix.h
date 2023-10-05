#ifndef GUID_05244B7E_9EC9_4E40_B6B7_710DEF81486E
#define GUID_05244B7E_9EC9_4E40_B6B7_710DEF81486E

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 类声明
class CD3DVector;

// D3D 矩阵
class DEXTER_D3D_CLASS CD3DMatrix : public D3DXMATRIX
{
	// 函数定义
public:
	// 构造函数
	CD3DMatrix();
	CD3DMatrix( const float * pValue );
	CD3DMatrix( const D3DMATRIX* pValue );
	CD3DMatrix( const D3DMATRIX& Value );
	CD3DMatrix( const D3DXFLOAT16 * pValue );
	CD3DMatrix( float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44 );
	// 析构函数
	virtual ~CD3DMatrix();

	// 操作函数
public:
	// 单位矩阵
	CD3DMatrix * MatrixIdentity();

	// 偏移矩阵
public:
	// 偏移矩阵
	CD3DMatrix * MatrixTranslation( D3DXVECTOR3 * pPosVector3 );
	// 偏移矩阵
	CD3DMatrix * MatrixTranslation(int XPos, int YPos, int ZPos);
	// 偏移矩阵
	CD3DMatrix * MatrixTranslation(float XPos, float YPos, float ZPos);

	// 旋转矩阵
public:
	// 旋转矩阵
	CD3DMatrix * MatrixRotationX(float XAngle);
	// 旋转矩阵
	CD3DMatrix * MatrixRotationY(float YAngle);
	// 旋转矩阵
	CD3DMatrix * MatrixRotationZ(float ZAngle);
	// 旋转矩阵
	CD3DMatrix * MatrixRotationAxis( D3DXVECTOR3 * pVectorAxis, float fAngle );

	// 缩放矩阵
public:
	// 缩放矩阵
	CD3DMatrix * MatrixScalingX(float XScaling);
	// 缩放矩阵
	CD3DMatrix * MatrixScalingY(float YScaling);
	// 缩放矩阵
	CD3DMatrix * MatrixScalingZ(float ZScaling);
	// 缩放矩阵
	CD3DMatrix * MatrixScalingXYZ(float fScaling);
	// 缩放矩阵
	CD3DMatrix * MatrixScalingXYZ(float XScaling, float YScaling, float ZScaling);

	// 获取函数
public:
	// 获取偏移
	CD3DVector GetTranslation();
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif