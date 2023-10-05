#ifndef GUID_74623CA5_EA53_4E3F_B922_0B5B386F1B06
#define GUID_74623CA5_EA53_4E3F_B922_0B5B386F1B06

#pragma once

#include "D3DMatrix.h"
#include "D3DVector.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ����ͷ��
class DEXTER_D3D_CLASS CD3DCamera
{
	// ���Ա���
public:
	bool							m_bKeyboardDebug;			// ���̵���

	// �۲����
public:
	float							m_fViewDistance;			// �۲����
	CD3DVector						m_vEye;						// �������λ��
	CD3DVector						m_vLookat;					// �����Ŀ���λ��
	CD3DVector						m_vUp;						// ����������Ϸ�
	CD3DMatrix						m_CurrentView;				// ��ǰ�Ϲ۲����

	// ��������
public:
	// ���캯��
	CD3DCamera();
	// ��������
	virtual ~CD3DCamera();

	// ��ȡ����
public:
	// ��ȡ��ǰ�۲��
	CD3DMatrix* GetCurrentView() { return &m_CurrentView; };
	// ��ȡ�۾�
	CD3DVector GetEye() { return m_vEye; };
	// ��ȡ�۲��
	CD3DVector GetLookat() { return m_vLookat; };
	// ��ȡ������
	CD3DVector GetUp() { return m_vUp; };

	// ���ú���
public:
	// ���ü��̵���
	void SetKeyboardDebug( bool bKeyboardDebug );
	// �����۾�
	void SetEye( CD3DVector vEye );
	// ���ù۲��
	void SetLookat( CD3DVector vLookat );
	// ����������
	void SetUp( CD3DVector vUp );

	// ���ܺ���
public:
	// ���¹۲����
	void UpdateViewDistance();

	// ���Ժ���
public:
	// �����X��ת
	void EyeRotationX( float fRadian );
	// �����Y��ת
	void EyeRotationY( float fRadian );
	// �����X�۲��ת
	void LookatRotationX( float fRadian );
	// �����Y�۲��ת
	void LookatRotationY( float fRadian );
	// �����ƶ�
	void MoveLeftRight( float fRadian );
	// �����ƶ�
	void MoveTopBottom( float fRadian );
	// ǰ���ƶ�
	void MoveFrontBack( float fRadian );

	// ���º���
public:
	// ���������
	void UpdateCamera();
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif