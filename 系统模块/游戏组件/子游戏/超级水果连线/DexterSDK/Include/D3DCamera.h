#ifndef GUID_74623CA5_EA53_4E3F_B922_0B5B386F1B06
#define GUID_74623CA5_EA53_4E3F_B922_0B5B386F1B06

#pragma once

#include "D3DMatrix.h"
#include "D3DVector.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 摄像头类
class DEXTER_D3D_CLASS CD3DCamera
{
	// 调试变量
public:
	bool							m_bKeyboardDebug;			// 键盘调试

	// 观察变量
public:
	float							m_fViewDistance;			// 观察距离
	CD3DVector						m_vEye;						// 摄象机的位置
	CD3DVector						m_vLookat;					// 摄象机目标的位置
	CD3DVector						m_vUp;						// 摄象机的正上方
	CD3DMatrix						m_CurrentView;				// 当前认观察矩阵

	// 函数定义
public:
	// 构造函数
	CD3DCamera();
	// 析构函数
	virtual ~CD3DCamera();

	// 获取函数
public:
	// 获取当前观察点
	CD3DMatrix* GetCurrentView() { return &m_CurrentView; };
	// 获取眼睛
	CD3DVector GetEye() { return m_vEye; };
	// 获取观察点
	CD3DVector GetLookat() { return m_vLookat; };
	// 获取正方向
	CD3DVector GetUp() { return m_vUp; };

	// 设置函数
public:
	// 设置键盘调试
	void SetKeyboardDebug( bool bKeyboardDebug );
	// 设置眼睛
	void SetEye( CD3DVector vEye );
	// 设置观察点
	void SetLookat( CD3DVector vLookat );
	// 设置正方向
	void SetUp( CD3DVector vUp );

	// 功能函数
public:
	// 更新观察距离
	void UpdateViewDistance();

	// 调试函数
public:
	// 摄像机X自转
	void EyeRotationX( float fRadian );
	// 摄像机Y自转
	void EyeRotationY( float fRadian );
	// 摄像机X观察点转
	void LookatRotationX( float fRadian );
	// 摄像机Y观察点转
	void LookatRotationY( float fRadian );
	// 左右移动
	void MoveLeftRight( float fRadian );
	// 上下移动
	void MoveTopBottom( float fRadian );
	// 前后移动
	void MoveFrontBack( float fRadian );

	// 更新函数
public:
	// 更新摄像机
	void UpdateCamera();
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif