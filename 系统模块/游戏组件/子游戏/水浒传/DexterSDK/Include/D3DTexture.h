#ifndef GUID_9C228A8E_C78D_40F9_8A49_DFB98A0EA36F
#define GUID_9C228A8E_C78D_40F9_8A49_DFB98A0EA36F

#pragma once

#include "D3DDevice.h"
#include "DexterD3DHead.h"
#include "D3DEffectPool.h"
#include "D3DSize.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ��ת����
enum EnumTextureRotationDirection
{
	TextureRotation_Null = 0, 
	TextureRotation_X, 
	TextureRotation_Y, 
	TextureRotation_Z, 
};

// ���BUFF
#define CUSTOM_MAX_BUFF				128

// ����ṹ
struct tagTextureInfo
{
	IDirect3DTexture9 *				pTexture;
	uint							nWidth;
	uint							nHeight;
};

// WHG�ṹ
struct tagWHGTextureInfo
{
	uint							nPosX;
	uint							nPosY;
	uint							nWidth;
	uint							nHeight;
};

// WHGͷ�ṹ
struct tagWHGTextureHead
{
	TCHAR							szName[MAX_PATH];
	uint							nPosX;
	uint							nPosY;
	uint							nWidth;
	uint							nHeight;
};

// ������
typedef CMap<CString, const tchar *, tagTextureInfo *, tagTextureInfo *> CMapTexture;
typedef CMap<CString, const tchar *, tagWHGTextureInfo *, tagWHGTextureInfo *> CMapWHGInfo;
typedef CMap<CString, const tchar *, CMapWHGInfo *, CMapWHGInfo *> CMapWHGTexture;
////////////////////////////////////////////////////////////////////////////////// 

// ������
class CD3DTextureBuffer;

// �������
class DEXTER_D3D_CLASS CD3DTexture
{
	// ͼƬ��Ϣ
protected:
	CString							m_TextureKey;						// ��ʶ��Ϣ
	CPoint							m_TexturePoint;						// ͼƬλ��
	CSize							m_TextureSize;						// ͼƬ��С
	CSize							m_MemorySize;						// �ڴ��С
	tagD3DTextureVertex				m_TextureVertex[4];					// ��������

	// ��ת��Ϣ
protected:
	EnumTextureRotationDirection	m_chDirection;						// ��ת����
	float							m_fAngle;							// ��ת����
	float							m_fRotationX;						// ��ת����
	float							m_fRotationY;						// ��ת����

	// ֡����Ϣ
public:
	uint							m_nDrawFrame;						// �滭֡��
	uint							m_nWidthFrame;						// ���֡��
	uint							m_nHeightFrame;						// �߶�֡��

	// �ԳƱ���
protected:
	bool							m_bSymmetry;						// �Գƻ滭

	// ģ������
protected:
	int								m_nBlurryDegree;					// ģ���̶�

	// ��Ч����
protected:
	EnumTextureEffectType			m_nEffectType;						// ��Ч����
	CD3DColor						m_dwTextureColor;					// ��������ɫ
	CD3DColor						m_dwTextureBorderColor;				// ���������ɫ
	CD3DColor						m_dwTextureTopColor;				// ������ͷ��ɫ
	CD3DColor						m_dwTextureBottomColor;				// ������β��ɫ

	// ���������
protected:
	CD3DTexture *					m_pUpperTexture;					// �ϲ�����
	tagD3DTextureTwoVertex			m_TextureTwoVertex[4];				// ��������

	// ����ָ��
protected:
	static CMapTexture				m_MapTexture;						// ����ָ��
	static CMapWHGTexture			m_MapWHGTexture;					// WHG����ָ��

	// ��������
public:
	// ���캯��
	CD3DTexture();
	// ��������
	virtual ~CD3DTexture();

	// ͼƬ��Ϣ
public:
	// �����ж�
	bool IsNull();
	// ֡��ͼƬ
	bool IsTextureFrame() { return m_nWidthFrame != 0 && m_nHeightFrame != 0; }
	// ������
	int GetWidth() { return m_TextureSize.cx; }
	// ����߶�
	int GetHeight() { return m_TextureSize.cy; }
	// ֡�����
	int GetFrameWidth() { return !IsTextureFrame() ? m_TextureSize.cx : m_TextureSize.cx / m_nWidthFrame; }
	// ֡���߶�
	int GetFrameHeight() { return !IsTextureFrame() ? m_TextureSize.cy : m_TextureSize.cy / m_nHeightFrame; }
	// ��ȡ֡��
	uint GetTextureFrame() { return m_nWidthFrame * m_nHeightFrame; }

	// ��ȡͼ��
public:
	// ��ȡͼ��
	IDirect3DTexture9 * GetTexture();

	// ���ƺ���
public:
	// ������ת
	void SetRotation( EnumTextureRotationDirection chDirection, float fRotationX, float fRotationY, float fAngle );
	// ������ת
	void SetRotation( EnumTextureRotationDirection chDirection, int nRotationX, int nRotationY, float fAngle );
	// ���öԳ�
	void SetSymmetry( bool bSymmetry );
	// ����͸��
	void SetTextureAlpha( byte cbAlpha );
	// ����͸��
	void SetTextureAlpha( float fAlpha );
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
	// ����֡��
	void SetTextureFrame( uint nWidthFrame, uint nHeightFrame );
	// �滭֡��
	void SetDrawFrame( uint nDrawFrame );
	// ����ģ���̶�
	void SetBlurryDegree( int nBlurryDegree );

	// �������
public:
	// ��������
	bool Destory();

	// ���ܺ���
public:
	// ��������
	bool LoadTexture( CD3DDevice * pD3DDevice, int nWidth, int nHeight, D3DCOLOR * pTextureColor );
	// ��������
	bool LoadTexture( CD3DDevice * pD3DDevice, hinstance hInstance, uint nIDResourc, CD3DColor ColorKey );
	// ��������
	bool LoadTexture( CD3DDevice * pD3DDevice, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName );
	// ��������
	bool LoadTexture( CD3DDevice * pD3DDevice, hinstance hInstance, uint nIDResource, const tchar * pszTypeName );
	// ��������
	bool LoadTexture( CD3DDevice * pD3DDevice, const tchar * pszFileName );
	// ��������
	bool LoadTexture( CD3DDevice * pD3DDevice, CD3DTextureBuffer & TextureBuffer );
	// ��������
	bool LoadTexture( CD3DTexture & Texture );
	// ��������
	bool LoadTexture( CD3DTexture & Texture, CRect RectImage );
	// ��������
	bool LoadWHGTexture( CD3DDevice * pD3DDevice, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, const tchar * pszImageKey );
	// ��������
	bool LoadWHGTexture( CD3DDevice * pD3DDevice, const tchar * pszFileName, const tchar * pszImageKey );

	// �滭����
public:
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight);

	// ͸���滭
public:
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, byte cbAlpha);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, byte cbAlpha);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, byte cbAlpha);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, byte cbAlpha);

	// �˻��滭
public:
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, CD3DColor D3DColor);
	// �滭ͼƬ
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, CD3DColor D3DColor);
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

	// ��������
protected:
	// �滭ͼƬ
	bool DrawTexture( CD3DDevice * pD3DDevice, void * pVertexData, uint nVertexSize, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, D3DCOLOR dwTextureColor );
	// ��תͼƬ
	void RotationTexture( CD3DMatrix* pD3DMatrix );
	// ���ö���
	void SetTextureVertex( CD3DDevice * pD3DDevice, tagTextureInfo * pTextureInfo, void** ppVertexData, uint* pVertexSize, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight );
	// ���ö���
	void SetTextureVertex( CD3DDevice * pD3DDevice, tagTextureInfo * pTextureInfo, void** ppVertexData, uint* pVertexSize, CPoint ptDest[4], int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CSize SizeWindow  );
	// ���ö���
	void SetTexture3DVertex( CD3DDevice * pD3DDevice, tagTextureInfo * pTextureInfo, void** ppVertexData, uint* pVertexSize, CD3DSize D3DSize, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight );
	// ������ͼ
	void SetTexturePos( tagD3DTextureVertex * pTextureVertex, float fXSource, float fYSource, float fSourceWidth, float fSourceHeight );
	// ������ͼ
	void SetTexturePos( tagD3DTextureTwoVertex * pTextureVertex, float fXSource, float fYSource, float fSourceWidth, float fSourceHeight );
	// ���λ��
	void SetWindowPos( tagD3DTextureVertex * pTextureVertex, float fDestWidth, float fDestHeight );
	// ���λ��
	void SetWindowPos( tagD3DTextureTwoVertex * pTextureVertex, float fDestWidth, float fDestHeight );
	// ���λ��
	void SetWindowPos( tagD3DTextureVertex * pTextureVertex, CPoint ptDest[4], CSize SizeWindow );
	// ���λ��
	void SetWindowPos( tagD3DTextureTwoVertex * pTextureVertex, CPoint ptDest[4], CSize SizeWindow );
	// ���λ��
	void SetWindow3DPos( tagD3DTextureVertex * pTextureVertex, float fDestWidth, float fDestHeight );
	// ���λ��
	void SetWindow3DPos( tagD3DTextureTwoVertex * pTextureVertex, float fDestWidth, float fDestHeight );

};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif