#ifndef  GUID_CDAEADCF_86DA_458A_830C_DE7BFB922901
#define  GUID_CDAEADCF_86DA_458A_830C_DE7BFB922901

#pragma once

#include "D3DDevice.h"
#include "D3DVector.h"
#include "D3DColor.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 宏定义
#define	BOUNDING_BOX_MAX			8			// 边界盒子

// 边界盒子
class DEXTER_D3D_CLASS CD3DBoundingBox
{
	// 变量定义
protected:
	CD3DVector						m_vMin;								// 最小点
	CD3DVector						m_vMax;								// 最大点
	CD3DVector						m_vVector[BOUNDING_BOX_MAX];		// 精确点

	// 类函数
public:
	// 构造函数
	CD3DBoundingBox();
	// 构造函数
	CD3DBoundingBox( CD3DBoundingBox& Right );
	// 构造函数
	CD3DBoundingBox( CD3DVector vMin, CD3DVector vMax );
	// 析构函数
	virtual ~CD3DBoundingBox();

	// 获取函数
public:
	// 空盒子
	bool IsEmpty();
	// 获取最大点
	CD3DVector GetMax();
	// 获取最小点
	CD3DVector GetMin();
	// 获取精确点
	void GetVector( CD3DVector vVector[BOUNDING_BOX_MAX] );

	// 设置函数
public:
	// 设置盒子
	void SetBox( CD3DVector vMin, CD3DVector vMax );

	// 功能函数
public:
	// 判断点是否在盒子中
	bool IsPointInside( CD3DMatrix* pD3DMatBox, CD3DVector Vector );
	// 绘画盒子
	void DrawBox( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatBox, CD3DColor D3DColor );
};

// 边界球
class DEXTER_D3D_CLASS CD3DBoundingSphere
{
	// 变量定义
protected:
	CD3DVector						m_vCenter;							// 中心点
	float							m_fRadius;							// 半径

	// 类函数
public:
	// 构造函数
	CD3DBoundingSphere();
	// 构造函数
	CD3DBoundingSphere( CD3DBoundingSphere& Right );
	// 构造函数
	CD3DBoundingSphere( CD3DVector vCenter, float fRadius );
	// 析构函数
	virtual ~CD3DBoundingSphere();


	// 获取函数
public:
	// 空圆
	bool IsEmpty();
	// 获取圆心
	CD3DVector GetCenter();
	// 获取半径
	float GetRadius();

	// 设置函数
public:
	// 设置圆
	void SetSphere( CD3DVector vCenter, float fRadius );

	// 功能函数
public:
	// 判断点是否在盒子中
	bool IsPointInside( CD3DMatrix* pD3DMatSphere, CD3DVector Vector );
	// 绘画球
	void DrawSphere(CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DColor D3DColor );
};

// 合并边界框
CD3DBoundingBox DEXTER_D3D_API MergeBoundingBox( CD3DBoundingBox* pBoundingBoxOne, CD3DBoundingBox* pBoundingBoxTwo );

// 合并边界圆
CD3DBoundingSphere DEXTER_D3D_API MergeBoundingSphere( CD3DBoundingSphere* pBoundingSphereOne, CD3DBoundingSphere* pBoundingSphereTwo );

// 判断点是否在盒子中
bool DEXTER_D3D_API IsPointInsideBoundingBox( CD3DBoundingBox* pBoundingBox, CD3DMatrix* pD3DMatBox, CD3DVector Vector );

// 判断点是否在圆中
bool DEXTER_D3D_API IsPointInsideBoundingSphere( CD3DBoundingSphere* pBoundingSphere, CD3DMatrix* pD3DMatSphere, CD3DVector Vector );

// 判断边界相交
bool DEXTER_D3D_API IntersectBoundingBox( CD3DBoundingBox* pBoundingBoxOne, CD3DMatrix* pD3DMatBoxOne, CD3DBoundingBox* pBoundingBoxTwo, CD3DMatrix* pD3DMatBoxTwo );
bool DEXTER_D3D_API IntersectBoundingSphere( CD3DBoundingSphere* pBoundingSphereOne, CD3DMatrix* pD3DMatSphereOne, CD3DBoundingSphere* pBoundingSphereTwo, CD3DMatrix* pD3DMatSphereTwo );
bool DEXTER_D3D_API IntersectBoundingBoxSphere( CD3DBoundingSphere* pBoundingSphere, CD3DMatrix* pD3DMatSphere, CD3DBoundingBox* pBoundingBox, CD3DMatrix* pD3DMatBox );
bool DEXTER_D3D_API IntersectBoundingBoxSphere( CD3DBoundingBox* pBoundingBox, CD3DMatrix* pD3DMatBox, CD3DBoundingSphere* pBoundingSphere, CD3DMatrix* pD3DMatSphere );

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif