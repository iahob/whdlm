#ifndef GUID_0EA097AD_8FB5_41F8_86F4_FB3924D94E83
#define GUID_0EA097AD_8FB5_41F8_86F4_FB3924D94E83

#pragma once

#include "D3DDevice.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 变换颜色
#define D3DCOLOR_TO_FLOATARRAY(D3DColor)					\
{															\
	(LOBYTE((D3DColor)>>16)) / 255.f,						\
	(LOBYTE(((WORD)(D3DColor)) >> 8)) / 255.f,				\
	(LOBYTE(D3DColor)) / 255.f,								\
	(LOBYTE((D3DColor)>>24)) / 255.f						\
}															

// 纹理特效
enum EnumTextureEffectType
{
	TextureEffect_Cover = 0, 						// 全覆盖绘画
	TextureEffect_WindowBack = TextureEffect_Cover, // 窗口背景
	TextureEffect_Shine, 							// 发亮
	TextureEffect_Gray, 							// 灰度
	TextureEffect_Relief, 							// 浮雕
	TextureEffect_Border, 							// 描边
	TextureEffect_Gradient, 						// 渐变
	TextureEffect_GradientBorder, 					// 渐变后描边
	TextureEffect_Texture, 							// 纹理叠加
	TextureEffect_TextureBorder, 					// 纹理描边
	TextureEffect_Blurry, 							// 纹理模糊
	TextureEffect_Normal, 							// 正常
	TextureEffect_Max = TextureEffect_Normal,		// 最大值
};

// 模型特效
enum EnumModelEffectType
{
	ModelEffect_3DMaxLight = 0,						// 3DMAX灯效
	ModelEffect_Normal, 							// 正常
	ModelEffect_Max = ModelEffect_Normal,			// 最大值
};

////////////////////////////////////////////////////////////////////////////////// 

// 特效定义
typedef CMap<CString, const tchar *, CArray<ID3DXEffect *> *, CArray<ID3DXEffect *> *> CMapEffect;

// 效果池
class DEXTER_D3D_CLASS CD3DEffectPool
{
	// 纹理变量
private:
	ID3DXEffect*									m_pTextureEffect;				// 纹理效果
	ID3DXEffect*									m_pModel3DMaxLightEffect;		// 模型效果

public:
	ID3DXEffect*									m_pSkinModel3DMaxLightEffect;	// 模型效果

	// 模型变量
private:
	CMapEffect										m_MapModelEffect;				// 模型效果

	// 静态变量
protected:
	static CD3DEffectPool							m_D3DEffectPool;				// 对象变量

	// 函数定义
protected:
	// 构造函数
	CD3DEffectPool();

	// 函数定义
public:
	// 析构函数
	virtual ~CD3DEffectPool();

	// 静态函数
public:
	// 返回对象
	inline static CD3DEffectPool* _Object() { return &m_D3DEffectPool; }

	// 功能函数
public:
	// 创建效果
	ID3DXEffect* CreateEffect( CD3DDevice * pD3DDevice, const tchar * pszFileName );
	// 释放效果
	bool ReleaseEffect( ID3DXEffect* pID3DXEffect );

	// 功能函数
public:
	// 创建效果
	bool InitializeEffect( CD3DDevice * pD3DDevice );
	// 丢失设备
	void OnLostDevice();
	// 重置设备
	void OnResetDevice();
	// 释放效果
	void DestroyEffect();

	// 纹理功能
public:
	// 获取纹理特效
	ID3DXEffect* GetTextureEffect() { return m_pTextureEffect; }
	// 获取模型特效
	ID3DXEffect* GetModelEffect( EnumModelEffectType nEffectType );
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif