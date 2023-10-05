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

// �궨��
#define	BOUNDING_BOX_MAX			8			// �߽����

// �߽����
class DEXTER_D3D_CLASS CD3DBoundingBox
{
	// ��������
protected:
	CD3DVector						m_vMin;								// ��С��
	CD3DVector						m_vMax;								// ����
	CD3DVector						m_vVector[BOUNDING_BOX_MAX];		// ��ȷ��

	// �ຯ��
public:
	// ���캯��
	CD3DBoundingBox();
	// ���캯��
	CD3DBoundingBox( CD3DBoundingBox& Right );
	// ���캯��
	CD3DBoundingBox( CD3DVector vMin, CD3DVector vMax );
	// ��������
	virtual ~CD3DBoundingBox();

	// ��ȡ����
public:
	// �պ���
	bool IsEmpty();
	// ��ȡ����
	CD3DVector GetMax();
	// ��ȡ��С��
	CD3DVector GetMin();
	// ��ȡ��ȷ��
	void GetVector( CD3DVector vVector[BOUNDING_BOX_MAX] );

	// ���ú���
public:
	// ���ú���
	void SetBox( CD3DVector vMin, CD3DVector vMax );

	// ���ܺ���
public:
	// �жϵ��Ƿ��ں�����
	bool IsPointInside( CD3DMatrix* pD3DMatBox, CD3DVector Vector );
	// �滭����
	void DrawBox( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatBox, CD3DColor D3DColor );
};

// �߽���
class DEXTER_D3D_CLASS CD3DBoundingSphere
{
	// ��������
protected:
	CD3DVector						m_vCenter;							// ���ĵ�
	float							m_fRadius;							// �뾶

	// �ຯ��
public:
	// ���캯��
	CD3DBoundingSphere();
	// ���캯��
	CD3DBoundingSphere( CD3DBoundingSphere& Right );
	// ���캯��
	CD3DBoundingSphere( CD3DVector vCenter, float fRadius );
	// ��������
	virtual ~CD3DBoundingSphere();


	// ��ȡ����
public:
	// ��Բ
	bool IsEmpty();
	// ��ȡԲ��
	CD3DVector GetCenter();
	// ��ȡ�뾶
	float GetRadius();

	// ���ú���
public:
	// ����Բ
	void SetSphere( CD3DVector vCenter, float fRadius );

	// ���ܺ���
public:
	// �жϵ��Ƿ��ں�����
	bool IsPointInside( CD3DMatrix* pD3DMatSphere, CD3DVector Vector );
	// �滭��
	void DrawSphere(CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DColor D3DColor );
};

// �ϲ��߽��
CD3DBoundingBox DEXTER_D3D_API MergeBoundingBox( CD3DBoundingBox* pBoundingBoxOne, CD3DBoundingBox* pBoundingBoxTwo );

// �ϲ��߽�Բ
CD3DBoundingSphere DEXTER_D3D_API MergeBoundingSphere( CD3DBoundingSphere* pBoundingSphereOne, CD3DBoundingSphere* pBoundingSphereTwo );

// �жϵ��Ƿ��ں�����
bool DEXTER_D3D_API IsPointInsideBoundingBox( CD3DBoundingBox* pBoundingBox, CD3DMatrix* pD3DMatBox, CD3DVector Vector );

// �жϵ��Ƿ���Բ��
bool DEXTER_D3D_API IsPointInsideBoundingSphere( CD3DBoundingSphere* pBoundingSphere, CD3DMatrix* pD3DMatSphere, CD3DVector Vector );

// �жϱ߽��ཻ
bool DEXTER_D3D_API IntersectBoundingBox( CD3DBoundingBox* pBoundingBoxOne, CD3DMatrix* pD3DMatBoxOne, CD3DBoundingBox* pBoundingBoxTwo, CD3DMatrix* pD3DMatBoxTwo );
bool DEXTER_D3D_API IntersectBoundingSphere( CD3DBoundingSphere* pBoundingSphereOne, CD3DMatrix* pD3DMatSphereOne, CD3DBoundingSphere* pBoundingSphereTwo, CD3DMatrix* pD3DMatSphereTwo );
bool DEXTER_D3D_API IntersectBoundingBoxSphere( CD3DBoundingSphere* pBoundingSphere, CD3DMatrix* pD3DMatSphere, CD3DBoundingBox* pBoundingBox, CD3DMatrix* pD3DMatBox );
bool DEXTER_D3D_API IntersectBoundingBoxSphere( CD3DBoundingBox* pBoundingBox, CD3DMatrix* pD3DMatBox, CD3DBoundingSphere* pBoundingSphere, CD3DMatrix* pD3DMatSphere );

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif