#ifndef GUID_7891499D_109F_4FC4_957B_C88DF33D5096
#define GUID_7891499D_109F_4FC4_957B_C88DF33D5096

#pragma once

#include "D3DColor.h"
#include "D3DVector.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 光源对象
class DEXTER_D3D_CLASS CD3DLight
{
	// 函数定义
public:
	// 构造函数
	CD3DLight();
	// 析构函数
	virtual ~CD3DLight();

	// 重载函数
public:
	// 返回光源类型
	virtual uint GetLightType() = NULL;
	// 获取光源
	virtual void GetLightInfo(D3DLIGHT9 & D3DLight) = NULL;
	// 设置光源
	virtual void SetLightInfo(D3DLIGHT9 & D3DLight) = NULL;
};

////////////////////////////////////////////////////////////////////////////////// 

// 点光光源
class DEXTER_D3D_CLASS CD3DPointLight : public CD3DLight
{
	// 变量定义
public:
	CD3DColor						Diffuse;							// 反射颜色
	CD3DColor						Ambient;							// 环境颜色
	CD3DColor						Specular;							// 镜面颜色
	CD3DVector						Position;							// 光源位置
	float							Range;								// 光照范围
	float							Attenuation0;						// 光常量
	float							Attenuation1;						// 光线性
	float							Attenuation2;						// 衰减系数

	// 函数定义
public:
	// 构造函数
	CD3DPointLight();
	// 析构函数
	virtual ~CD3DPointLight();

	// 重载函数
public:
	// 返回光源类型
	virtual uint GetLightType() { return D3DLIGHT_POINT; }
	// 获取光源
	virtual void GetLightInfo(D3DLIGHT9 & D3DLight);
	// 设置光源
	virtual void SetLightInfo(D3DLIGHT9 & D3DLight);
};

////////////////////////////////////////////////////////////////////////////////// 

// 方向光源
class DEXTER_D3D_CLASS CD3DDirectionalLight : public CD3DLight
{
	// 变量定义
public:
	CD3DColor						Diffuse;							// 漫射光
	CD3DColor						Specular;							// 镜面光
	CD3DColor						Ambient;							// 环境光
	CD3DVector						Direction;							// 方向

	// 函数定义
public:
	// 构造函数
	CD3DDirectionalLight();
	// 析构函数
	virtual ~CD3DDirectionalLight();

	// 重载函数
public:
	// 返回光源类型
	virtual uint GetLightType() { return D3DLIGHT_DIRECTIONAL; }
	// 获取光源
	virtual void GetLightInfo(D3DLIGHT9 & D3DLight);
	// 设置光源
	virtual void SetLightInfo(D3DLIGHT9 & D3DLight);
};

////////////////////////////////////////////////////////////////////////////////// 

// 聚光光源
class DEXTER_D3D_CLASS CD3DSpotLight : public CD3DLight
{
	// 变量定义
public:
	CD3DColor						Diffuse;							// 漫射光
	CD3DColor						Specular;							// 镜面光
	CD3DColor						Ambient;							// 环境光
	CD3DVector						Position;							// 光源位置
	CD3DVector						Direction;							// 方向
	float							Range;								// 光程
	float							Falloff;							// 内锥到外锥衰减度
	float							Attenuation0;						// 光常量
	float							Attenuation1;						// 光线性
	float							Attenuation2;						// 光2次距离衰减系数
	float							Theta;								// 内锥角
	float							Phi;								// 外锥角

	// 函数定义
public:
	// 构造函数
	CD3DSpotLight();
	// 析构函数
	virtual ~CD3DSpotLight();

	// 重载函数
public:
	// 返回光源类型
	virtual uint GetLightType() { return D3DLIGHT_SPOT; }
	// 获取光源
	virtual void GetLightInfo(D3DLIGHT9 & D3DLight);
	// 设置光源
	virtual void SetLightInfo(D3DLIGHT9 & D3DLight);
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif