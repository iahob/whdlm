#ifndef GUID_203BC29C_1D9B_4AC8_AF4B_05E9FA20D8CC
#define GUID_203BC29C_1D9B_4AC8_AF4B_05E9FA20D8CC

// 组件版本
#define DexterD3DVersion			(1, 0, 0, 1)

////////////////////////////////////////////////////////////////////////////////// 
// 包含文件

// MFC 文件
#include <Afxmt.h>
#include <AtlBase.h>
#include <AfxTempl.h>
#include <MMSystem.h>

// D3D 文件
#include <D3D9.h>
#include <D3Dx9.h>
#include <DSound.h>
#include <DMusici.h>

// 全局定义
#include "Dexter.h"
#pragma warning (disable : 4204)

////////////////////////////////////////////////////////////////////////////////// 

#define DEXTER_D3D_CLASS	
#define DEXTER_D3D_API		

#define GAME_RESOURCE_NAME		_T("GameResource.dat")

#define namespace_dex_begin	namespace dex {
#define namespace_dex_end	}

////////////////////////////////////////////////////////////////////////////////// 
// 常量定义

// 最大BUFF
#define VERTEX_MAX_BUFF				128

// 投影矩阵的近远点
#define	NEAR_POINT_Z				1.0f
#define FAR_POINT_Z					1000.0f

// 顶点格式
#define D3DFVF_COLOR				(D3DFVF_XYZ|D3DFVF_DIFFUSE)			// 顶点格式
#define D3DFVF_TEXTURE				(D3DFVF_XYZ|D3DFVF_TEX1)			// 顶点格式
#define D3DFVF_TEXTURETWO			(D3DFVF_XYZ|D3DFVF_TEX2)			// 顶点格式

// 宏定义
#define SafeLostDevice(pObject)		{ if (pObject!=NULL) { pObject->OnLostDevice();} }
#define SafeResetDevice(pObject)	{ if (pObject!=NULL) { pObject->OnResetDevice();} }

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

// 渲染状态
enum EnumRenderState
{
	RenderState_Un,														// 未知模式
	RenderState_2D,														// 2D模式
	RenderState_3D,														// 3D模式
	RenderState_3DBasic,												// 3D基础模式
};

// 过滤状态
enum EnumFilterState
{
	FilterState_Un,														// 未知模式
	FilterState_Fast,													// 快速低效果模式
	FilterState_Medium,													// 中速中效果模式
	FilterState_Efficient,												// 慢速高效果模式
};

// 资源信息
struct tagResourceInfo
{
	uint							nSize;								// 资源大小
	byte *							pcbBuffer;							// 资源内存

	tagResourceInfo() : nSize(0), pcbBuffer(NULL) {}
	~tagResourceInfo() { if ( pcbBuffer != NULL ) { delete[] pcbBuffer; } pcbBuffer = NULL; }
};

////////////////////////////////////////////////////////////////////////////////// 

// 像素结构
struct tagD3DXCOLORVertex
{
	float							x, y, z;							// 坐标
	DWORD							color;								// 颜色
};

// 纹理结构
struct tagD3DTextureVertex
{
	float							x, y, z;							// 坐标
	float							tu, tv;								// 纹理顶点
};

// 多纹理结构（2张）
struct tagD3DTextureTwoVertex
{
	float							x, y, z;							// 坐标
	float							tu0, tv0;							// 纹理顶点
	float							tu1, tv1;							// 纹理顶点
};

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 
// 导出文件

#ifndef DEXTER_D3D_DLL
	#include "Dexter.h"
	#include "D3DAssist.h"
	#include "D3DAssist.inl"
	#include "D3DBounding.h"
	#include "D3DCamera.h"
	#include "D3DColor.h"
	#include "D3DDevice.h"
	#include "D3DDrawBasic.h"
	#include "D3DEffectPool.h"
	#include "D3DFont.h"
	#include "D3DLight.h"
	#include "D3DMatrix.h"
	#include "D3DModel.h"
	#include "D3DParticle.h"
	#include "D3DSize.h"
	#include "D3DSkyBox.h"
	#include "D3DTexture.h"
	#include "D3DTextureBuffer.h"
	#include "D3DVector.h"
	#include "LapseCount.h"
	#include "DirectUIObject.h"
	#include "DirectUIWindow.h"
	#include "DirectUIButton.h"
	#include "DirectUICheck.h"
	#include "DirectUIEdit.h"
	#include "DirectUIEngine.h"
	#include "DexterSound.h"
	#include "D3DFreeType.h"
	#include "D3DMovie.h"
#endif

#ifndef _UNICODE
	#error "The program must be compiled Unicode."
#endif

////////////////////////////////////////////////////////////////////////////////// 

#endif