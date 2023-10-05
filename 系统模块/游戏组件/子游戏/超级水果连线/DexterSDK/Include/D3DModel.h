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

// ��Ч����
#define INVALID_ANIMATION					UINT_MAX

// �ṹ����
struct D3DXFRAME_DEX : public D3DXFRAME
{
	// ʵ�ʾ���
	CD3DMatrix       CombinedTransformationMatrix;

	// ���캯��
	D3DXFRAME_DEX()
	{
		// ���ñ���
		pMeshContainer = NULL;
		pFrameSibling = NULL;
		pFrameFirstChild = NULL;

		// ���þ���
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

	// ���캯��
	D3DXMESHCONTAINER_DEX()
	{
		// ���ñ���
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
// ��̬ģ��
class DEXTER_D3D_CLASS CD3DModel : public ID3DXAllocateHierarchy
{
	// ��������
public:
	bool						m_bCloneModel;				// ��¡ģ��
	bool						m_bCloseLight;				// �رյƹ�
	uint						m_nMaxVertexBlendMatrices;	// ���û滭����
	TCHAR						m_szFileDir[MAX_PATH];		// �ļ�Ŀ¼
	CD3DDevice *					m_pD3DDevice;				// �豸ָ��
	D3DXFRAME*					m_pD3DFrame;				// ģ������

	// ��������
protected:
	bool						m_bMoving;					// �ƶ���ʶ
	ID3DXAnimationDebugler*	m_pID3DXAnimationDebugler;// ��������
	ID3DXAnimationSet**			m_ppID3DXAnimationSet;		// ��������
	TCHAR**						m_ppszAnimationName;		// ��������
	uint						m_nAnimationSetCount;		// ��������

	// ʱ�����
protected:
	double						m_dElapsedTime;				// ����ʱ��
	double						m_dCreateTime;				// ����ʱ��
	double						m_dAnimationVelocity;		// �����ٶ�

	// ģ�Ͳ���
protected:
	bool						m_bShadowMaterial;			// ��Ӱ����
	bool						m_bStartMaterial;			// ���ò���
	D3DMATERIAL9				m_CustomMaterial;			// �Զ�����

	// ģ�ͱ߽�
protected:
	CD3DBoundingBox				m_BoundingBox;				// ����
	CD3DBoundingSphere			m_BoundingSphere;			// Բ

	// ��Ч��Ϣ
protected:
	EnumModelEffectType			m_nEffectType;				// ��Ч����
	ID3DXEffect*				m_pID3DXEffect[ModelEffect_Max];

	CD3DMatrix*					m_pBoneMatrixs;
	uint						m_nNumBoneMatricesMax;

#ifdef DEX_MODEL_3DMAX
	CArray<tagEffectParam*>		m_ArrayEffectParam;			
#endif

	// ��������
public:
	// ���캯��
	CD3DModel();
	// ��������
	virtual ~CD3DModel();

	// ��������
public:
	// ���ö���
	bool ActivateAnimationName( const tchar * pszAnimationName );
	// ���ö���
	bool ActivateAnimationIndex( uint nAnimationIndex );
	// �����ٶ�
	void SetAnimationVelocity( float fVelocity );
	// ���ö���
	void ResetAnimation();
	// ��ȡ��������
	uint GetAnimationCount();
	// ��ȡ��������
	const tchar * GetAnimationName( uint nAnimationIndex );
	// ��ȡ��������
	double GetAnimationPeriod( uint nAnimationIndex );

	// ���ܺ���
public:
	// ����ģ��
	void Destory();
	// ����ģ��
	bool LoadModel( CD3DDevice * pD3DDevice, TCHAR* strFileName );
	// ��¡ģ��
	bool LoadModel( CD3DModel & DynamicModel );

	// �滭����
public:
	// �滭ģ��
	bool DrawModel( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix );
	// �滭ģ��
	bool DrawModel( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, byte cbAlpha );
	// �滭��Ӱ
	bool DrawShadow( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix );
	// �滭�߽��
	bool DrawBoundingBox( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DColor D3DColor );
	// �滭�߽�Բ
	bool DrawBoundingSphere( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DColor D3DColor );

	// ��Ч����
public:
	// ������Ч
	void SetEffectType( EnumModelEffectType nEffectType );
	// ������Ч����
	bool SetEffectParameter( EnumModelEffectType nEffectType, char* pszParameter, void* pData, uint nSize );

	// ���ܺ���
public:
	// �رյƹ�
	void CloseLight( bool bCloseLight );
	// ���ò���
	void SetCustomMaterial( D3DMATERIAL9* pMaterial );
	// ����ж�
	float MousePick( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CPoint ptMouse );

	// �ڲ�����
protected:
	// ���¶���
	void UpdateMove();
	// ���¾���
	void UpdateMatrix( D3DXFRAME* pFrameBase, D3DXMATRIX* pParentMatrix );
	// �滭�߽��
	void DrawBox( CD3DDevice * pD3DDevice, D3DXFRAME* pFrame, CD3DColor D3DColor );
	// �滭�߽�Բ
	void DrawSphere( CD3DDevice * pD3DDevice, D3DXFRAME* pFrame, CD3DColor D3DColor );
	// �滭����
	void DrawFrame( CD3DDevice * pD3DDevice, D3DXFRAME* pFrame );
	// �ڲ��滭
	void DrawMesh( CD3DDevice * pD3DDevice, D3DXMESHCONTAINER* pMeshContainerBase, D3DXFRAME* pFrameBase );
	// ��ʼ��Ӱ
	void BeginShadowState( CD3DDevice * pD3DDevice );
	// ������Ӱ
	void EndShadowState( CD3DDevice * pD3DDevice );
	// �ж����
	float PickFrame( CD3DDevice * pD3DDevice, D3DXFRAME* pFrame, CD3DVector vNear, CD3DVector vDir, float fDist );
	// �ж����
	float PickMesh( CD3DDevice * pD3DDevice, D3DXMESHCONTAINER* pMeshContainerBase, CD3DVector vNear, CD3DVector vDir, float fDist );
	// �󶨶���
	HRESULT GenerateSkinnedMesh( CD3DDevice * pD3DDevice, D3DXMESHCONTAINER_DEX *pMeshContainer );
	// ��װָ��
	bool SetupBoneMatrixPointersOnMesh( D3DXMESHCONTAINER* pMeshContainerBase );
	// ��װ���
	bool SetupBoneMatrixPointers( D3DXFRAME* pFrame );
	// ��װ����
	void SetupAnimationDebugler();
	// ��������
	HRESULT AllocateName( LPCSTR Name, LPSTR* pNewName );

	// �̳к���
public:
	// �������
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, D3DXFRAME* *ppNewFrame);
	// ��������
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData,CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, D3DXMESHCONTAINER* *ppNewMeshContainer);
	// ���ٿ��
	STDMETHOD(DestroyFrame)(THIS_ D3DXFRAME* pFrameToFree);
	// ��������
	STDMETHOD(DestroyMeshContainer)(THIS_ D3DXMESHCONTAINER* pMeshContainerBase);	
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif