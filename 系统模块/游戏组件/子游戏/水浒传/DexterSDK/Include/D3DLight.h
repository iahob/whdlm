#ifndef GUID_7891499D_109F_4FC4_957B_C88DF33D5096
#define GUID_7891499D_109F_4FC4_957B_C88DF33D5096

#pragma once

#include "D3DColor.h"
#include "D3DVector.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ��Դ����
class DEXTER_D3D_CLASS CD3DLight
{
	// ��������
public:
	// ���캯��
	CD3DLight();
	// ��������
	virtual ~CD3DLight();

	// ���غ���
public:
	// ���ع�Դ����
	virtual uint GetLightType() = NULL;
	// ��ȡ��Դ
	virtual void GetLightInfo(D3DLIGHT9 & D3DLight) = NULL;
	// ���ù�Դ
	virtual void SetLightInfo(D3DLIGHT9 & D3DLight) = NULL;
};

////////////////////////////////////////////////////////////////////////////////// 

// ����Դ
class DEXTER_D3D_CLASS CD3DPointLight : public CD3DLight
{
	// ��������
public:
	CD3DColor						Diffuse;							// ������ɫ
	CD3DColor						Ambient;							// ������ɫ
	CD3DColor						Specular;							// ������ɫ
	CD3DVector						Position;							// ��Դλ��
	float							Range;								// ���շ�Χ
	float							Attenuation0;						// �ⳣ��
	float							Attenuation1;						// ������
	float							Attenuation2;						// ˥��ϵ��

	// ��������
public:
	// ���캯��
	CD3DPointLight();
	// ��������
	virtual ~CD3DPointLight();

	// ���غ���
public:
	// ���ع�Դ����
	virtual uint GetLightType() { return D3DLIGHT_POINT; }
	// ��ȡ��Դ
	virtual void GetLightInfo(D3DLIGHT9 & D3DLight);
	// ���ù�Դ
	virtual void SetLightInfo(D3DLIGHT9 & D3DLight);
};

////////////////////////////////////////////////////////////////////////////////// 

// �����Դ
class DEXTER_D3D_CLASS CD3DDirectionalLight : public CD3DLight
{
	// ��������
public:
	CD3DColor						Diffuse;							// �����
	CD3DColor						Specular;							// �����
	CD3DColor						Ambient;							// ������
	CD3DVector						Direction;							// ����

	// ��������
public:
	// ���캯��
	CD3DDirectionalLight();
	// ��������
	virtual ~CD3DDirectionalLight();

	// ���غ���
public:
	// ���ع�Դ����
	virtual uint GetLightType() { return D3DLIGHT_DIRECTIONAL; }
	// ��ȡ��Դ
	virtual void GetLightInfo(D3DLIGHT9 & D3DLight);
	// ���ù�Դ
	virtual void SetLightInfo(D3DLIGHT9 & D3DLight);
};

////////////////////////////////////////////////////////////////////////////////// 

// �۹��Դ
class DEXTER_D3D_CLASS CD3DSpotLight : public CD3DLight
{
	// ��������
public:
	CD3DColor						Diffuse;							// �����
	CD3DColor						Specular;							// �����
	CD3DColor						Ambient;							// ������
	CD3DVector						Position;							// ��Դλ��
	CD3DVector						Direction;							// ����
	float							Range;								// ���
	float							Falloff;							// ��׶����׶˥����
	float							Attenuation0;						// �ⳣ��
	float							Attenuation1;						// ������
	float							Attenuation2;						// ��2�ξ���˥��ϵ��
	float							Theta;								// ��׶��
	float							Phi;								// ��׶��

	// ��������
public:
	// ���캯��
	CD3DSpotLight();
	// ��������
	virtual ~CD3DSpotLight();

	// ���غ���
public:
	// ���ع�Դ����
	virtual uint GetLightType() { return D3DLIGHT_SPOT; }
	// ��ȡ��Դ
	virtual void GetLightInfo(D3DLIGHT9 & D3DLight);
	// ���ù�Դ
	virtual void SetLightInfo(D3DLIGHT9 & D3DLight);
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif