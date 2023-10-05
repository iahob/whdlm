#ifndef GUID_203BC29C_1D9B_4AC8_AF4B_05E9FA20D8CC
#define GUID_203BC29C_1D9B_4AC8_AF4B_05E9FA20D8CC

// ����汾
#define DexterD3DVersion			(1, 0, 0, 1)

////////////////////////////////////////////////////////////////////////////////// 
// �����ļ�

// MFC �ļ�
#include <Afxmt.h>
#include <AtlBase.h>
#include <AfxTempl.h>
#include <MMSystem.h>

// D3D �ļ�
#include <D3D9.h>
#include <D3Dx9.h>
#include <DSound.h>
#include <DMusici.h>

// ȫ�ֶ���
#include "Dexter.h"
#pragma warning (disable : 4204)

////////////////////////////////////////////////////////////////////////////////// 

#define DEXTER_D3D_CLASS	
#define DEXTER_D3D_API		

#define GAME_RESOURCE_NAME		_T("GameResource.dat")

#define namespace_dex_begin	namespace dex {
#define namespace_dex_end	}

////////////////////////////////////////////////////////////////////////////////// 
// ��������

// ���BUFF
#define VERTEX_MAX_BUFF				128

// ͶӰ����Ľ�Զ��
#define	NEAR_POINT_Z				1.0f
#define FAR_POINT_Z					1000.0f

// �����ʽ
#define D3DFVF_COLOR				(D3DFVF_XYZ|D3DFVF_DIFFUSE)			// �����ʽ
#define D3DFVF_TEXTURE				(D3DFVF_XYZ|D3DFVF_TEX1)			// �����ʽ
#define D3DFVF_TEXTURETWO			(D3DFVF_XYZ|D3DFVF_TEX2)			// �����ʽ

// �궨��
#define SafeLostDevice(pObject)		{ if (pObject!=NULL) { pObject->OnLostDevice();} }
#define SafeResetDevice(pObject)	{ if (pObject!=NULL) { pObject->OnResetDevice();} }

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

// ��Ⱦ״̬
enum EnumRenderState
{
	RenderState_Un,														// δ֪ģʽ
	RenderState_2D,														// 2Dģʽ
	RenderState_3D,														// 3Dģʽ
	RenderState_3DBasic,												// 3D����ģʽ
};

// ����״̬
enum EnumFilterState
{
	FilterState_Un,														// δ֪ģʽ
	FilterState_Fast,													// ���ٵ�Ч��ģʽ
	FilterState_Medium,													// ������Ч��ģʽ
	FilterState_Efficient,												// ���ٸ�Ч��ģʽ
};

// ��Դ��Ϣ
struct tagResourceInfo
{
	uint							nSize;								// ��Դ��С
	byte *							pcbBuffer;							// ��Դ�ڴ�

	tagResourceInfo() : nSize(0), pcbBuffer(NULL) {}
	~tagResourceInfo() { if ( pcbBuffer != NULL ) { delete[] pcbBuffer; } pcbBuffer = NULL; }
};

////////////////////////////////////////////////////////////////////////////////// 

// ���ؽṹ
struct tagD3DXCOLORVertex
{
	float							x, y, z;							// ����
	DWORD							color;								// ��ɫ
};

// ����ṹ
struct tagD3DTextureVertex
{
	float							x, y, z;							// ����
	float							tu, tv;								// ������
};

// ������ṹ��2�ţ�
struct tagD3DTextureTwoVertex
{
	float							x, y, z;							// ����
	float							tu0, tv0;							// ������
	float							tu1, tv1;							// ������
};

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 
// �����ļ�

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