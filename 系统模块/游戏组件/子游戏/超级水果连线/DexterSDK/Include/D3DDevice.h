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

// 类说明
class CD3DDevice;
class CD3DCamera;
class CD3DTextureBuffer;

// 设备接口
interface ID3DDeviceSink
{
	// 设备事件
public:
	// 配置设备
	virtual void OnInitDevice(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// 丢失设备
	virtual void OnLostDevice(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// 重置设备
	virtual void OnResetDevice(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// 调试灯光
	virtual void OnDebugLight(CD3DDevice * pD3DDevice) = NULL;
};

// 设备接口
interface ID3DDirectUISink
{
	// 设备事件
public:
	// 配置设备
	virtual void OnInitDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// 丢失设备
	virtual void OnLostDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// 重置设备
	virtual void OnResetDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// 缓冲更新
	virtual void OnUpdataTextureBuffer(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// 渲染窗口
	virtual void OnRenderDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
	// 窗口动画
	virtual void OnMovieDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight) = NULL;
};

////////////////////////////////////////////////////////////////////////////////// 

// 设备对象
class DEXTER_D3D_CLASS CD3DDevice
{
	// 窗口属性
public:
	HWND 							m_hWndDevice;						// 设备窗口
	CSize							m_SizeWindow;						// 窗口大小

	// 窗口属性
public:
	HWND 							m_hRenderWnd;						// 设备窗口

	// 状态变量
protected:
	EnumRenderState					m_nRenderState;						// 渲染状态
	EnumFilterState					m_nFilterState;						// 过滤状态

	// 配置变量
protected:
	CD3DColor						m_crBackColor;						// 背景颜色
	bool							m_bMultiThreaded;					// 多线程值
	bool							m_bEnableMultiSample;				// 采样标识	
	bool							m_bAnisotropy;						// 各向异性采样
	bool							m_bConstantlyUpdata;				// 时时刷新
	uint							m_nMaxAnisotropy;					// 最大采样
	uint							m_nMaxActiveLights;					// 最大灯光数量

	// 透视矩阵
protected:
	CD3DMatrix						m_MatrixOrthoProjection;			// 正交投影
	CD3DMatrix						m_MatrixPerspectiveProjection;		// 透视投影

	// 阴影信息
protected:
	D3DMATERIAL9					m_ShadowMaterial;					// 阴影材质
	D3DXVECTOR4						m_ShadowLightDirection;				// 阴影光的方向
	D3DXPLANE						m_ShadowGroundPlane;				// 阴影对应平面

	// 接口变量
protected:
	ID3DDeviceSink *				m_pID3DDeviceSink;					// 回调接口
	ID3DDirectUISink *				m_pID3DDirectUISink;				// DUI回调接口
	IDirect3D9 *					m_pIDirect3D;						// 环境对象
	IDirect3DDevice9 *				m_pIDirect3DDevice;					// 设备对象

	// 图片缓冲
protected:
	bool							m_bTextureBuffer;					// 缓冲绘画
	CSize							m_SizeTextureBuffer;				// 缓冲大小
	CD3DMatrix						m_MatrixTextureBuffer;				// 缓冲投影
	IDirect3DSurface9 *				m_pIDirect3DSurface;				// 绘画缓冲

	// 摄像机
protected:
	CD3DCamera						m_3DCamera;							// 3D摄像机
	CD3DCamera						m_2DCamera;							// 2D摄像机

	// 静态变量
protected:
	static CD3DDevice				m_D3DDevice;						// 对象指针

	// 函数定义
protected:
	// 构造函数
	CD3DDevice();
	// 析构函数
	virtual ~CD3DDevice();

	// 信息函数
public:
	// 获取窗口
	HWND GetWndDevice() { return m_hWndDevice; }
	// 获取大小
	CSize GetSizeWindow() { return m_bTextureBuffer ? m_SizeTextureBuffer : m_SizeWindow; }
	// 获取是否时时刷新
	bool GetConstantlyUpdata() { return m_bConstantlyUpdata; }

	// 摄像信息
public:
	// 获取3D摄像机
	CD3DCamera* Get3DCamera() { return &m_3DCamera; }
	// 获取3D当前观察点
	CD3DMatrix* Get3DViewMatrix() { return m_3DCamera.GetCurrentView(); }
	// 获取2D摄像机
	CD3DCamera* Get2DCamera() { return &m_2DCamera; }
	// 获取2D当前观察点
	CD3DMatrix* Get2DViewMatrix() { return m_2DCamera.GetCurrentView(); }

	// 阴影信息
public:
	// 获取阴影材质
	D3DMATERIAL9* GetShadowMaterial() { return &m_ShadowMaterial; }
	// 获取阴影光方向
	D3DXVECTOR4* GetShadowLightDirection() { return &m_ShadowLightDirection; }
	// 获取阴影透平平面
	D3DXPLANE* GetShadowGroundPlan() { return &m_ShadowGroundPlane; }

	// 配置接口
public:
	// 设多线程
	void SetMultiThreaded( bool bMultiThreaded );
	// 设置采样
	void SetEnableMultiSample( bool bEnableMultiSample ) { m_bEnableMultiSample = bEnableMultiSample; }
	// 设置各向异性采样
	void SetAnisotropy( bool bAnisotropy ) { m_bAnisotropy = bAnisotropy; }
	// 设置颜色
	void SetBackColor( CD3DColor crBackColor ) { m_crBackColor = crBackColor; }
	// 设置是否时时刷新
	void SetConstantlyUpdata( bool bConstantlyUpdata ) { m_bConstantlyUpdata = bConstantlyUpdata; }
	// 设置阴影颜色
	void SetShadowColor( CD3DColor crShadowColor );
	// 设置阴影光方向
	void SetShadowLightDirection( float fX, float fY, float fZ );
	// 设置阴影透平平面
	void SetShadowGroundPlane( float fX, float fY, float fZ );
	// 设置灯光
	bool SetLight( uint nIindex, CD3DLight* pD3DLight );
	// 激活灯光
	bool LightEnable( uint nIindex, bool bEnable );

	// 功能函数
public:
	// 位置转换
	CPoint WorldMatrixToWindow( CD3DMatrix* pD3DMatrix );

	// 获取接口
public:
	// 回调接口
	ID3DDeviceSink * GetD3DDeviceSink() { return m_pID3DDeviceSink; }
	// DUI回调接口
	ID3DDirectUISink * GetD3DDirectUISink() { return m_pID3DDirectUISink; }
	// 设备接口
	IDirect3DDevice9 * GetDirect3DDevice() { return m_pIDirect3DDevice; }

	// 调试函数
public:
	// 重置设备
	bool ResetD3DDevice();
	// 渲染设备
	bool RenderD3DDevice();
	// 创建设备
	bool CreateD3DDevice(HWND hWndDevice, ID3DDeviceSink * pID3DDeviceSink, ID3DDirectUISink * pID3DDirectUISink);

	// 渲染状态
public:
	// 渲染属性
	HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	// 渲染属性
	HRESULT GetRenderState(D3DRENDERSTATETYPE State, DWORD & Value);

	// 设置函数
public:
	// 设置渲染状态
	void SetRenderState( EnumRenderState nRenderState );
	// 设置过滤状态
	void SetFilterState( EnumFilterState nFilterState );

	// 蒙板函数
private:
	// 开始蒙板
	void BeginMask();
	// 结束蒙板
	void EndMask();
	// 清除蒙板
	void ClearMask();

	// 图片缓冲
public:
	// 开始图片缓冲
	bool BeginTextureBuffer( CD3DTextureBuffer * pD3DTextureBuffer, CD3DColor D3DColorBack );
	// 结束图片缓冲
	void EndTextureBuffer();

	// 投影状态
public:
	// 获取正交投影
	CD3DMatrix*	GetOrthoProjection();
	// 获取透视投影
	CD3DMatrix*	GetPerspectiveProjection();
	// 设置正交投影
	CD3DMatrix* SetOrthoProjection( int nWidth, int nHeight, float fNear, float fFar );
	// 设置透视投影
	CD3DMatrix* SetPerspectiveProjection( float fovy, float Aspect, float fNear, float fFar );

	// 内部函数
private:
	// 设置环境
	void InitD3DDevice();
	// 设置数据
	void InitPresentParameters(D3DPRESENT_PARAMETERS & PresentParameters, HWND hWnd, int nWidth, int nHeight, D3DFORMAT BufferFormat);

	// 静态函数
public:
	// 获取对象
	Static CD3DDevice * _Object() { return &m_D3DDevice; }
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif