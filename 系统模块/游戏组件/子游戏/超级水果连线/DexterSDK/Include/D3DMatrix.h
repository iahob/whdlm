#ifndef GUID_05244B7E_9EC9_4E40_B6B7_710DEF81486E
#define GUID_05244B7E_9EC9_4E40_B6B7_710DEF81486E

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ������
class CD3DVector;

// D3D ����
class DEXTER_D3D_CLASS CD3DMatrix : public D3DXMATRIX
{
	// ��������
public:
	// ���캯��
	CD3DMatrix();
	CD3DMatrix( const float * pValue );
	CD3DMatrix( const D3DMATRIX* pValue );
	CD3DMatrix( const D3DMATRIX& Value );
	CD3DMatrix( const D3DXFLOAT16 * pValue );
	CD3DMatrix( float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44 );
	// ��������
	virtual ~CD3DMatrix();

	// ��������
public:
	// ��λ����
	CD3DMatrix * MatrixIdentity();

	// ƫ�ƾ���
public:
	// ƫ�ƾ���
	CD3DMatrix * MatrixTranslation( D3DXVECTOR3 * pPosVector3 );
	// ƫ�ƾ���
	CD3DMatrix * MatrixTranslation(int XPos, int YPos, int ZPos);
	// ƫ�ƾ���
	CD3DMatrix * MatrixTranslation(float XPos, float YPos, float ZPos);

	// ��ת����
public:
	// ��ת����
	CD3DMatrix * MatrixRotationX(float XAngle);
	// ��ת����
	CD3DMatrix * MatrixRotationY(float YAngle);
	// ��ת����
	CD3DMatrix * MatrixRotationZ(float ZAngle);
	// ��ת����
	CD3DMatrix * MatrixRotationAxis( D3DXVECTOR3 * pVectorAxis, float fAngle );

	// ���ž���
public:
	// ���ž���
	CD3DMatrix * MatrixScalingX(float XScaling);
	// ���ž���
	CD3DMatrix * MatrixScalingY(float YScaling);
	// ���ž���
	CD3DMatrix * MatrixScalingZ(float ZScaling);
	// ���ž���
	CD3DMatrix * MatrixScalingXYZ(float fScaling);
	// ���ž���
	CD3DMatrix * MatrixScalingXYZ(float XScaling, float YScaling, float ZScaling);

	// ��ȡ����
public:
	// ��ȡƫ��
	CD3DVector GetTranslation();
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif