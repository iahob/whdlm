#ifndef GUID_0572D054_8406_4968_8950_2948844D36F0
#define GUID_0572D054_8406_4968_8950_2948844D36F0

#pragma once

#include "D3DCamera.h"
#include "D3DMatrix.h"
#include "D3DColor.h"
#include "D3DLight.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ��˵��
class CD3DDevice;
class CD3DCamera;
class CD3DTextureBuffer;

// �豸�ӿ�
interface ID3DDeviceSink
{
	// �豸�¼�
public:
	// �����豸
	virtual void OnInitDevice(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// ��ʧ�豸
	virtual void OnLostDevice(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// �����豸
	virtual void OnResetDevice(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// ���Եƹ�
	virtual void OnDebugLight(CD3DDevice * pD3DDevice) = NULL;
};

// �豸�ӿ�
interface ID3DDirectUISink
{
	// �豸�¼�
public:
	// �����豸
	virtual void OnInitDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// ��ʧ�豸
	virtual void OnLostDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// �����豸
	virtual void OnResetDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// �������
	virtual void OnUpdataTextureBuffer(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// ��Ⱦ����
	virtual void OnRenderDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// ���ڶ���
	virtual void OnMovieDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
};

////////////////////////////////////////////////////////////////////////////////// 

// �豸����
class DEXTER_D3D_CLASS CD3DDevice
{
	// ��������
public:
	HWND 							m_hWndDevice;						// �豸����
	CSize							m_SizeWindow;						// ���ڴ�С

	// ��������
public:
	HWND 							m_hRenderWnd;						// �豸����

	// ״̬����
protected:
	EnumRenderState					m_nRenderState;						// ��Ⱦ״̬
	EnumFilterState					m_nFilterState;						// ����״̬

	// ���ñ���
protected:
	CD3DColor						m_crBackColor;						// ������ɫ
	bool							m_bMultiThreaded;					// ���߳�ֵ
	bool							m_bEnableMultiSample;				// ������ʶ	
	bool							m_bAnisotropy;						// �������Բ���
	bool							m_bConstantlyUpdata;				// ʱʱˢ��
	uint							m_nMaxAnisotropy;					// ������
	uint							m_nMaxActiveLights;					// ���ƹ�����

	// ͸�Ӿ���
protected:
	CD3DMatrix						m_MatrixOrthoProjection;			// ����ͶӰ
	CD3DMatrix						m_MatrixPerspectiveProjection;		// ͸��ͶӰ

	// ��Ӱ��Ϣ
protected:
	D3DMATERIAL9					m_ShadowMaterial;					// ��Ӱ����
	D3DXVECTOR4						m_ShadowLightDirection;				// ��Ӱ��ķ���
	D3DXPLANE						m_ShadowGroundPlane;				// ��Ӱ��Ӧƽ��

	// �ӿڱ���
protected:
	ID3DDeviceSink *				m_pID3DDeviceSink;					// �ص��ӿ�
	ID3DDirectUISink *				m_pID3DDirectUISink;				// DUI�ص��ӿ�
	IDirect3D9 *					m_pIDirect3D;						// ��������
	IDirect3DDevice9 *				m_pIDirect3DDevice;					// �豸����

	// ͼƬ����
protected:
	bool							m_bTextureBuffer;					// ����滭
	CSize							m_SizeTextureBuffer;				// �����С
	CD3DMatrix						m_MatrixTextureBuffer;				// ����ͶӰ
	IDirect3DSurface9 *				m_pIDirect3DSurface;				// �滭����

	// �����
protected:
	CD3DCamera						m_3DCamera;							// 3D�����
	CD3DCamera						m_2DCamera;							// 2D�����

	// ��̬����
protected:
	static CD3DDevice				m_D3DDevice;						// ����ָ��

	// ��������
protected:
	// ���캯��
	CD3DDevice();
	// ��������
	virtual ~CD3DDevice();

	// ��Ϣ����
public:
	// ��ȡ����
	HWND GetWndDevice() { return m_hWndDevice; }
	// ��ȡ��С
	CSize GetSizeWindow() { return m_bTextureBuffer ? m_SizeTextureBuffer : m_SizeWindow; }
	// ��ȡ�Ƿ�ʱʱˢ��
	bool GetConstantlyUpdata() { return m_bConstantlyUpdata; }

	// ������Ϣ
public:
	// ��ȡ3D�����
	CD3DCamera* Get3DCamera() { return &m_3DCamera; }
	// ��ȡ3D��ǰ�۲��
	CD3DMatrix* Get3DViewMatrix() { return m_3DCamera.GetCurrentView(); }
	// ��ȡ2D�����
	CD3DCamera* Get2DCamera() { return &m_2DCamera; }
	// ��ȡ2D��ǰ�۲��
	CD3DMatrix* Get2DViewMatrix() { return m_2DCamera.GetCurrentView(); }

	// ��Ӱ��Ϣ
public:
	// ��ȡ��Ӱ����
	D3DMATERIAL9* GetShadowMaterial() { return &m_ShadowMaterial; }
	// ��ȡ��Ӱ�ⷽ��
	D3DXVECTOR4* GetShadowLightDirection() { return &m_ShadowLightDirection; }
	// ��ȡ��Ӱ͸ƽƽ��
	D3DXPLANE* GetShadowGroundPlan() { return &m_ShadowGroundPlane; }

	// ���ýӿ�
public:
	// ����߳�
	void SetMultiThreaded( bool bMultiThreaded );
	// ���ò���
	void SetEnableMultiSample( bool bEnableMultiSample ) { m_bEnableMultiSample = bEnableMultiSample; }
	// ���ø������Բ���
	void SetAnisotropy( bool bAnisotropy ) { m_bAnisotropy = bAnisotropy; }
	// ������ɫ
	void SetBackColor( CD3DColor crBackColor ) { m_crBackColor = crBackColor; }
	// �����Ƿ�ʱʱˢ��
	void SetConstantlyUpdata( bool bConstantlyUpdata ) { m_bConstantlyUpdata = bConstantlyUpdata; }
	// ������Ӱ��ɫ
	void SetShadowColor( CD3DColor crShadowColor );
	// ������Ӱ�ⷽ��
	void SetShadowLightDirection( float fX, float fY, float fZ );
	// ������Ӱ͸ƽƽ��
	void SetShadowGroundPlane( float fX, float fY, float fZ );
	// ���õƹ�
	bool SetLight( uint nIindex, CD3DLight* pD3DLight );
	// ����ƹ�
	bool LightEnable( uint nIindex, bool bEnable );

	// ���ܺ���
public:
	// λ��ת��
	CPoint WorldMatrixToWindow( CD3DMatrix* pD3DMatrix );

	// ��ȡ�ӿ�
public:
	// �ص��ӿ�
	ID3DDeviceSink * GetD3DDeviceSink() { return m_pID3DDeviceSink; }
	// DUI�ص��ӿ�
	ID3DDirectUISink * GetD3DDirectUISink() { return m_pID3DDirectUISink; }
	// �豸�ӿ�
	IDirect3DDevice9 * GetDirect3DDevice() { return m_pIDirect3DDevice; }

	// ���Ժ���
public:
	// �����豸
	bool ResetD3DDevice();
	// ��Ⱦ�豸
	bool RenderD3DDevice();
	// �����豸
	bool CreateD3DDevice(HWND hWndDevice, ID3DDeviceSink * pID3DDeviceSink, ID3DDirectUISink * pID3DDirectUISink);

	// ��Ⱦ״̬
public:
	// ��Ⱦ����
	HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	// ��Ⱦ����
	HRESULT GetRenderState(D3DRENDERSTATETYPE State, DWORD & Value);

	// ���ú���
public:
	// ������Ⱦ״̬
	void SetRenderState( EnumRenderState nRenderState );
	// ���ù���״̬
	void SetFilterState( EnumFilterState nFilterState );

	// �ɰ庯��
private:
	// ��ʼ�ɰ�
	void BeginMask();
	// �����ɰ�
	void EndMask();
	// ����ɰ�
	void ClearMask();

	// ͼƬ����
public:
	// ��ʼͼƬ����
	bool BeginTextureBuffer( CD3DTextureBuffer * pD3DTextureBuffer, CD3DColor D3DColorBack );
	// ����ͼƬ����
	void EndTextureBuffer();

	// ͶӰ״̬
public:
	// ��ȡ����ͶӰ
	CD3DMatrix*	GetOrthoProjection();
	// ��ȡ͸��ͶӰ
	CD3DMatrix*	GetPerspectiveProjection();
	// ��������ͶӰ
	CD3DMatrix* SetOrthoProjection( int nWidth, int nHeight, float fNear, float fFar );
	// ����͸��ͶӰ
	CD3DMatrix* SetPerspectiveProjection( float fovy, float Aspect, float fNear, float fFar );

	// �ڲ�����
private:
	// ���û���
	void InitD3DDevice();
	// ��������
	void InitPresentParameters(D3DPRESENT_PARAMETERS & PresentParameters, HWND hWnd, int nWidth, int nHeight, D3DFORMAT BufferFormat);

	// ��̬����
public:
	// ��ȡ����
	Static CD3DDevice * _Object() { return &m_D3DDevice; }
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif