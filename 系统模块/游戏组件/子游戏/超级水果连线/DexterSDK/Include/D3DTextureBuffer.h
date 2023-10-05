#ifndef GUID_B216E494_71B4_435D_A22A_E71E5073790B
#define GUID_B216E494_71B4_435D_A22A_E71E5073790B

#pragma once

#include "D3DDevice.h"
#include "DexterD3DHead.h"
#include "D3DTexture.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ������	
class DEXTER_D3D_CLASS CD3DTextureBuffer : public CD3DTexture
{

	// ͼƬ��Ϣ
protected:
	CString							m_TextureBufferKey;					// ��ʶ��Ϣ
	CD3DDevice *					m_pD3DDevice;						// �豸ָ��
	CSize							m_SizeBuffer;						// �����С

	// ָ����Ϣ
protected:
	IDirect3DTexture9 *				m_pTextureInfo;						// ����ָ��
	IDirect3DSurface9 *				m_pSurfaceInfo;						// �滭����

	// ��̬����
public:
	static CArray< CD3DTextureBuffer * >		m_ArrayTextureBuffe;		// ��ָ��

	// ��������
public:
	// ���캯��
	CD3DTextureBuffer();
	// ��������
	virtual ~CD3DTextureBuffer();

	// ��ȡͼ��
public:
	// ��ȡͼ��
	IDirect3DTexture9 * GetTexture();
	// ��ȡ����
	IDirect3DSurface9 * GetSurface();

	// ͼƬ��Ϣ
public:
	// �����ж�
	bool IsNull();
	// ������
	int GetWidth() { return m_TextureSize.cx; }
	// ����߶�
	int GetHeight() { return m_TextureSize.cy; }

	// ���Ժ���
public:
	// ������ת
	void SetRotation( EnumTextureRotationDirection chDirection, float fRotationX, float fRotationY, float fAngle );
	// ������ת
	void SetRotation( EnumTextureRotationDirection chDirection, int nRotationX, int nRotationY, float fAngle );
	// ���öԳ�
	void SetSymmetry( bool bSymmetry );
	// ����͸��
	void SetTextureAlpha( byte cbAlpha );
	// ���ó˻�
	void SetTextureColor( CD3DColor D3DColor );
	// �������
	void SetTextureBorderColor( CD3DColor D3DColor );
	// ���ý���
	void SetGradientColor( CD3DColor D3DColorTop, CD3DColor D3DColorBottom );
	// �����ϲ�
	void SetUpperTexture( CD3DTexture * pUpperTexture );
	// ������Ч
	void SetEffectType( EnumTextureEffectType nEffectType );

	// �������
public:
	// ���ٻ���
	bool Destory();

	// ���ܺ���
public:
	// ��������
	bool CreateBuffer( CD3DDevice * pD3DDevice, int nWidth, int nHeight );
	// ����ͼƬ
	bool SaveImage( const tchar * pszSaveFile );

	// �滭����
public:
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight);

	// ͸���滭
public:
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, byte cbAlpha);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, byte cbAlpha);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, byte cbAlpha);

	// �˻��滭
public:
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, CD3DColor D3DColor);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, CD3DColor D3DColor);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CD3DColor D3DColor);

	// ���λ滭
public:
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, CPoint ptDest[4], CRect rcImage );
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, CPoint ptDest[4], CRect rcImage, byte cbAlpha);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, CPoint ptDest[4], CRect rcImage, CD3DColor D3DColor);

	// �滭�����ͼƬ
public:
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize );
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize, byte cbAlpha );
	// �滭ͼƬ
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize, CD3DColor D3DColor );
	// �滭ͼƬ
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CD3DColor D3DColor );

	// �滭�ռ�ͼƬ
public:
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize );
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize, byte cbAlpha );
	// �滭ͼƬ
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize, CD3DColor D3DColor );
	// �滭ͼƬ
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CD3DColor D3DColor );

	// ȫ�ֺ���
public:
	// ��ʧ�豸
	static void OnLostDevice();
	// �����豸
	static void OnResetDevice();
};


////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif