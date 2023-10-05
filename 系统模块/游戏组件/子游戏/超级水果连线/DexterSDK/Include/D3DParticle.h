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

// �������
#define PARTICLE_MAX						INT_MAX

// ���ӵĻ�����Ϣ
struct D3DParticleInfo
{
	CD3DVector						vPosition;				// λ��
	CD3DColor						cColor;					// ��ɫ
	bool							bAlive;					// �Ƿ񼤻�
};

// ������
class DEXTER_D3D_CLASS CD3DParticle
{

	// ������Ϣ
protected:
	int									m_nMaxParticle;					// �������ӵ��������
	uint								m_nLastTime;					// �ϴ�ʱ��
	bool								m_bStartPlay;					// ��ʼ����
	bool								m_bShineEffect;					// ����Ч��
	CD3DMatrix							m_D3DMatrix;					// ����λ�þ���
	CArray<D3DParticleInfo *>			m_ArrayParticle;				// ������Ϣ

	// ��������
protected:
	float								m_fParticleSize;				// ���Ӵ�С

	// ��Դ��Ϣ
protected:
	IDirect3DTexture9*					m_pTexture;						// ���ӵ�����
	tagD3DXCOLORVertex*					m_pD3DFVFParticle;				// ����BUFF


protected:
	CD3DParticle(void);
	~CD3DParticle(void);

	// ���غ���
protected:
	// ����һ������
	virtual D3DParticleInfo * CreateParticle() = NULL;
	// ����һ������
	virtual void ResetParticle( D3DParticleInfo * pParticle  ) = NULL;
	// ��������
	virtual void UpdateParticle( D3DParticleInfo * pParticle, float fElapsedTime ) = NULL;

	// ��Ⱦ����
protected:
	// ��Ⱦ״̬
	void SetRenderState( CD3DDevice * pD3DDevice );
	// ��ԭ��Ⱦ״̬
	void RestoreRenderState( CD3DDevice * pD3DDevice );

	// ���ܺ���
public:
	// ��ʼ����
	void Begin( int nBeginCount = PARTICLE_MAX );
	// ��������
	void Stop();
	// ������������
	void Reset();
	// �Ƿ���������
	bool IsAllDeath();
	// �������Ӵ�С
	void SetParticleSize( float fParticleSize ) { m_fParticleSize = fParticleSize; }
	// ���÷���Ч��
	void SetShineEffect( bool bShineEffect ) { m_bShineEffect = bShineEffect; }

	// ��������
public:
	// ���þ���
	void SetMatrix( CD3DMatrix& D3DMatrix ) { m_D3DMatrix = D3DMatrix; }
	// ���þ���
	void SetMatrix( CD3DMatrix* pD3DMatrix ) { m_D3DMatrix = *pD3DMatrix; }
	// ��ʼ������
	bool InitParticle( CD3DDevice * pD3DDevice , int nMaxParticle );
	// �滭����
	void DrawParticle( CD3DDevice * pD3DDevice );

};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif