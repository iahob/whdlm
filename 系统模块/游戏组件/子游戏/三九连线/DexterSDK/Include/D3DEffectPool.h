#ifndef GUID_0EA097AD_8FB5_41F8_86F4_FB3924D94E83
#define GUID_0EA097AD_8FB5_41F8_86F4_FB3924D94E83

#pragma once

#include "D3DDevice.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// �任��ɫ
#define D3DCOLOR_TO_FLOATARRAY(D3DColor)					\
{															\
	(LOBYTE((D3DColor)>>16)) / 255.f,						\
	(LOBYTE(((WORD)(D3DColor)) >> 8)) / 255.f,				\
	(LOBYTE(D3DColor)) / 255.f,								\
	(LOBYTE((D3DColor)>>24)) / 255.f						\
}															

// ������Ч
enum EnumTextureEffectType
{
	TextureEffect_Cover = 0, 						// ȫ���ǻ滭
	TextureEffect_WindowBack = TextureEffect_Cover, // ���ڱ���
	TextureEffect_Shine, 							// ����
	TextureEffect_Gray, 							// �Ҷ�
	TextureEffect_Relief, 							// ����
	TextureEffect_Border, 							// ���
	TextureEffect_Gradient, 						// ����
	TextureEffect_GradientBorder, 					// ��������
	TextureEffect_Texture, 							// �������
	TextureEffect_TextureBorder, 					// �������
	TextureEffect_Blurry, 							// ����ģ��
	TextureEffect_Normal, 							// ����
	TextureEffect_Max = TextureEffect_Normal,		// ���ֵ
};

// ģ����Ч
enum EnumModelEffectType
{
	ModelEffect_3DMaxLight = 0,						// 3DMAX��Ч
	ModelEffect_Normal, 							// ����
	ModelEffect_Max = ModelEffect_Normal,			// ���ֵ
};

////////////////////////////////////////////////////////////////////////////////// 

// ��Ч����
typedef CMap<CString, const tchar *, CArray<ID3DXEffect *> *, CArray<ID3DXEffect *> *> CMapEffect;

// Ч����
class DEXTER_D3D_CLASS CD3DEffectPool
{
	// �������
private:
	ID3DXEffect*									m_pTextureEffect;				// ����Ч��
	ID3DXEffect*									m_pModel3DMaxLightEffect;		// ģ��Ч��

public:
	ID3DXEffect*									m_pSkinModel3DMaxLightEffect;	// ģ��Ч��

	// ģ�ͱ���
private:
	CMapEffect										m_MapModelEffect;				// ģ��Ч��

	// ��̬����
protected:
	static CD3DEffectPool							m_D3DEffectPool;				// �������

	// ��������
protected:
	// ���캯��
	CD3DEffectPool();

	// ��������
public:
	// ��������
	virtual ~CD3DEffectPool();

	// ��̬����
public:
	// ���ض���
	inline static CD3DEffectPool* _Object() { return &m_D3DEffectPool; }

	// ���ܺ���
public:
	// ����Ч��
	ID3DXEffect* CreateEffect( CD3DDevice * pD3DDevice, const tchar * pszFileName );
	// �ͷ�Ч��
	bool ReleaseEffect( ID3DXEffect* pID3DXEffect );

	// ���ܺ���
public:
	// ����Ч��
	bool InitializeEffect( CD3DDevice * pD3DDevice );
	// ��ʧ�豸
	void OnLostDevice();
	// �����豸
	void OnResetDevice();
	// �ͷ�Ч��
	void DestroyEffect();

	// ������
public:
	// ��ȡ������Ч
	ID3DXEffect* GetTextureEffect() { return m_pTextureEffect; }
	// ��ȡģ����Ч
	ID3DXEffect* GetModelEffect( EnumModelEffectType nEffectType );
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif