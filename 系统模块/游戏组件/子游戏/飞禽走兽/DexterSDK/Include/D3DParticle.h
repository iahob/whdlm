#ifndef GUID_7410B9D3_1FF6_419A_9537_291257B29E6F
#define GUID_7410B9D3_1FF6_419A_9537_291257B29E6F
#pragma once

#include "DexterD3DHead.h"
#include "D3DDevice.h"
#include "D3DTexture.h"
#include "D3DVector.h"
#include "D3DColor.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 最大数量
#define PARTICLE_MAX						INT_MAX

// 粒子的基本信息
struct D3DParticleInfo
{
	CD3DVector						vPosition;				// 位置
	CD3DColor						cColor;					// 颜色
	bool							bAlive;					// 是否激活
};

// 粒子类
class DEXTER_D3D_CLASS CD3DParticle
{

	// 粒子信息
protected:
	int									m_nMaxParticle;					// 存在粒子的最大数量
	uint								m_nLastTime;					// 上次时间
	bool								m_bStartPlay;					// 开始播放
	bool								m_bShineEffect;					// 发亮效果
	CD3DMatrix							m_D3DMatrix;					// 粒子位置矩阵
	CArray<D3DParticleInfo *>			m_ArrayParticle;				// 粒子信息

	// 粒子设置
protected:
	float								m_fParticleSize;				// 粒子大小

	// 资源信息
protected:
	IDirect3DTexture9*					m_pTexture;						// 粒子的纹理
	tagD3DXCOLORVertex*					m_pD3DFVFParticle;				// 缓冲BUFF


protected:
	CD3DParticle(void);
	~CD3DParticle(void);

	// 重载函数
protected:
	// 创建一个粒子
	virtual D3DParticleInfo * CreateParticle() = NULL;
	// 重置一个粒子
	virtual void ResetParticle( D3DParticleInfo * pParticle  ) = NULL;
	// 更新粒子
	virtual void UpdateParticle( D3DParticleInfo * pParticle, float fElapsedTime ) = NULL;

	// 渲染函数
protected:
	// 渲染状态
	void SetRenderState( CD3DDevice * pD3DDevice );
	// 还原渲染状态
	void RestoreRenderState( CD3DDevice * pD3DDevice );

	// 功能函数
public:
	// 开始动画
	void Begin( int nBeginCount = PARTICLE_MAX );
	// 结束动画
	void Stop();
	// 重置所有粒子
	void Reset();
	// 是否所有死亡
	bool IsAllDeath();
	// 设置粒子大小
	void SetParticleSize( float fParticleSize ) { m_fParticleSize = fParticleSize; }
	// 设置发亮效果
	void SetShineEffect( bool bShineEffect ) { m_bShineEffect = bShineEffect; }

	// 公开函数
public:
	// 设置矩阵
	void SetMatrix( CD3DMatrix& D3DMatrix ) { m_D3DMatrix = D3DMatrix; }
	// 设置矩阵
	void SetMatrix( CD3DMatrix* pD3DMatrix ) { m_D3DMatrix = *pD3DMatrix; }
	// 初始化粒子
	bool InitParticle( CD3DDevice * pD3DDevice , int nMaxParticle );
	// 绘画粒子
	void DrawParticle( CD3DDevice * pD3DDevice );

};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif