#ifndef GUID_A22A16AF_B1DC_4BD4_BC2F_9698ECB363A3
#define GUID_A22A16AF_B1DC_4BD4_BC2F_9698ECB363A3

#pragma once

#include "D3DDevice.h"
#include "DexterD3DHead.h"
#include "D3DTexture.h"
#include "D3DBounding.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 无效动画
#define INVALID_ANIMATION					UINT_MAX

// 结构定义
struct D3DXFRAME_DEX : public D3DXFRAME
{
	// 实际矩阵
	CD3DMatrix       CombinedTransformationMatrix;

	// 构造函数
	D3DXFRAME_DEX()
	{
		// 设置变量
		pMeshContainer = NULL;
		pFrameSibling = NULL;
		pFrameFirstChild = NULL;

		// 设置矩阵
		D3DXMatrixIdentity(&TransformationMatrix);
		D3DXMatrixIdentity(&CombinedTransformationMatrix);
	};

};

struct D3DXMESHCONTAINER_DEX : public D3DXMESHCONTAINER
{
	CD3DBoundingBox		 BoundingBox;
	CD3DBoundingSphere	 BoundingSphere;
	CD3DTexture*		 pD3DTextures;        
	LPD3DXMESH           pOrigMesh;
	DWORD                NumAttributeGroups; 
	DWORD                NumInfl;
	LPD3DXBUFFER         pBoneCombinationBuf;
	D3DXMATRIX**         ppBoneMatrixPtrs;
	D3DXMATRIX*          pBoneOffsetMatrices;
	DWORD                NumPaletteEntries;
	bool                 UseSoftwareVP;

	// 构造函数
	D3DXMESHCONTAINER_DEX()
	{
		// 设置变量
		pMaterials = NULL;
		pEffects = NULL;
		pAdjacency = NULL;
		pSkinInfo = NULL;
		pNextMeshContainer = NULL;
		pOrigMesh = NULL;
		pBoneCombinationBuf = NULL;
		ppBoneMatrixPtrs = NULL;
		ppBoneMatrixPtrs = NULL;
		pBoneOffsetMatrices = NULL;	
	}
};

struct tagEffectParam
{
	char					szParamName[MAX_PATH];
	uint					nParamSize;
	byte*					pParamValue;
	CD3DTexture*			pTexture;        
};

/////////////////////////////////////////////////////////////////////////////////////////
// 动态模型
class DEXTER_D3D_CLASS CD3DModel : public ID3DXAllocateHierarchy
{
	// 变量定义
public:
	bool						m_bCloneModel;				// 克隆模型
	bool						m_bCloseLight;				// 关闭灯光
	uint						m_nMaxVertexBlendMatrices;	// 设置绘画能力
	TCHAR						m_szFileDir[MAX_PATH];		// 文件目录
	CD3DDevice *					m_pD3DDevice;				// 设备指针
	D3DXFRAME*					m_pD3DFrame;				// 模型网格

	// 动画变量
protected:
	bool						m_bMoving;					// 移动标识
	ID3DXAnimationDebugler*	m_pID3DXAnimationDebugler;// 动画调试
	ID3DXAnimationSet**			m_ppID3DXAnimationSet;		// 动画调试
	TCHAR**						m_ppszAnimationName;		// 动画名字
	uint						m_nAnimationSetCount;		// 动画数量

	// 时间变量
protected:
	double						m_dElapsedTime;				// 流逝时间
	double						m_dCreateTime;				// 创建时间
	double						m_dAnimationVelocity;		// 动画速度

	// 模型材质
protected:
	bool						m_bShadowMaterial;			// 阴影材质
	bool						m_bStartMaterial;			// 启用材质
	D3DMATERIAL9				m_CustomMaterial;			// 自定材质

	// 模型边界
protected:
	CD3DBoundingBox				m_BoundingBox;				// 盒子
	CD3DBoundingSphere			m_BoundingSphere;			// 圆

	// 特效信息
protected:
	EnumModelEffectType			m_nEffectType;				// 特效类型
	ID3DXEffect*				m_pID3DXEffect[ModelEffect_Max];

	CD3DMatrix*					m_pBoneMatrixs;
	uint						m_nNumBoneMatricesMax;

#ifdef DEX_MODEL_3DMAX
	CArray<tagEffectParam*>		m_ArrayEffectParam;			
#endif

	// 函数定义
public:
	// 构造函数
	CD3DModel();
	// 析构函数
	virtual ~CD3DModel();

	// 动画调试
public:
	// 设置动画
	bool ActivateAnimationName( const tchar * pszAnimationName );
	// 设置动画
	bool ActivateAnimationIndex( uint nAnimationIndex );
	// 动画速度
	void SetAnimationVelocity( float fVelocity );
	// 重置动画
	void ResetAnimation();
	// 获取动画数量
	uint GetAnimationCount();
	// 获取动画名字
	const tchar * GetAnimationName( uint nAnimationIndex );
	// 获取动画周期
	double GetAnimationPeriod( uint nAnimationIndex );

	// 功能函数
public:
	// 销毁模型
	void Destory();
	// 载入模型
	bool LoadModel( CD3DDevice * pD3DDevice, TCHAR* strFileName );
	// 克隆模型
	bool LoadModel( CD3DModel & DynamicModel );

	// 绘画函数
public:
	// 绘画模型
	bool DrawModel( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix );
	// 绘画模型
	bool DrawModel( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, byte cbAlpha );
	// 绘画阴影
	bool DrawShadow( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix );
	// 绘画边界框
	bool DrawBoundingBox( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DColor D3DColor );
	// 绘画边界圆
	bool DrawBoundingSphere( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DColor D3DColor );

	// 特效函数
public:
	// 设置特效
	void SetEffectType( EnumModelEffectType nEffectType );
	// 设置特效参数
	bool SetEffectParameter( EnumModelEffectType nEffectType, char* pszParameter, void* pData, uint nSize );

	// 功能函数
public:
	// 关闭灯光
	void CloseLight( bool bCloseLight );
	// 设置材质
	void SetCustomMaterial( D3DMATERIAL9* pMaterial );
	// 鼠标判断
	float MousePick( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CPoint ptMouse );

	// 内部函数
protected:
	// 更新动画
	void UpdateMove();
	// 更新矩阵
	void UpdateMatrix( D3DXFRAME* pFrameBase, D3DXMATRIX* pParentMatrix );
	// 绘画边界框
	void DrawBox( CD3DDevice * pD3DDevice, D3DXFRAME* pFrame, CD3DColor D3DColor );
	// 绘画边界圆
	void DrawSphere( CD3DDevice * pD3DDevice, D3DXFRAME* pFrame, CD3DColor D3DColor );
	// 绘画网格
	void DrawFrame( CD3DDevice * pD3DDevice, D3DXFRAME* pFrame );
	// 内部绘画
	void DrawMesh( CD3DDevice * pD3DDevice, D3DXMESHCONTAINER* pMeshContainerBase, D3DXFRAME* pFrameBase );
	// 开始阴影
	void BeginShadowState( CD3DDevice * pD3DDevice );
	// 结束阴影
	void EndShadowState( CD3DDevice * pD3DDevice );
	// 判断鼠标
	float PickFrame( CD3DDevice * pD3DDevice, D3DXFRAME* pFrame, CD3DVector vNear, CD3DVector vDir, float fDist );
	// 判断鼠标
	float PickMesh( CD3DDevice * pD3DDevice, D3DXMESHCONTAINER* pMeshContainerBase, CD3DVector vNear, CD3DVector vDir, float fDist );
	// 绑定动画
	HRESULT GenerateSkinnedMesh( CD3DDevice * pD3DDevice, D3DXMESHCONTAINER_DEX *pMeshContainer );
	// 安装指针
	bool SetupBoneMatrixPointersOnMesh( D3DXMESHCONTAINER* pMeshContainerBase );
	// 安装框架
	bool SetupBoneMatrixPointers( D3DXFRAME* pFrame );
	// 安装动画
	void SetupAnimationDebugler();
	// 分配名称
	HRESULT AllocateName( LPCSTR Name, LPSTR* pNewName );

	// 继承函数
public:
	// 创建框架
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, D3DXFRAME* *ppNewFrame);
	// 创建容器
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData,CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, D3DXMESHCONTAINER* *ppNewMeshContainer);
	// 销毁框架
	STDMETHOD(DestroyFrame)(THIS_ D3DXFRAME* pFrameToFree);
	// 销毁容器
	STDMETHOD(DestroyMeshContainer)(THIS_ D3DXMESHCONTAINER* pMeshContainerBase);	
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif