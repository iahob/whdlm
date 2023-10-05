#ifndef GUID_B216E494_71B4_435D_A22A_E71E5073790B
#define GUID_B216E494_71B4_435D_A22A_E71E5073790B

#pragma once

#include "D3DDevice.h"
#include "DexterD3DHead.h"
#include "D3DTexture.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// Œ∆¿Ìª∫≥Â	
class DEXTER_D3D_CLASS CD3DTextureBuffer : public CD3DTexture
{

	// Õº∆¨–≈œ¢
protected:
	CString							m_TextureBufferKey;					// ±Í ∂–≈œ¢
	CD3DDevice *					m_pD3DDevice;						// …Ë±∏÷∏’Î
	CSize							m_SizeBuffer;						// ª∫≥Â¥Û–°

	// ÷∏’Î–≈œ¢
protected:
	IDirect3DTexture9 *				m_pTextureInfo;						// Œ∆¿Ì÷∏’Î
	IDirect3DSurface9 *				m_pSurfaceInfo;						// ªÊª≠ª∫≥Â

	// æ≤Ã¨±‰¡ø
public:
	static CArray< CD3DTextureBuffer * >		m_ArrayTextureBuffe;		// ¿‡÷∏’Î

	// ∫Ø ˝∂®“Â
public:
	// ππ‘Ï∫Ø ˝
	CD3DTextureBuffer();
	// Œˆππ∫Ø ˝
	virtual ~CD3DTextureBuffer();

	// ªÒ»°ÕºœÒ
public:
	// ªÒ»°ÕºœÒ
	IDirect3DTexture9 * GetTexture();
	// ªÒ»°ª∫≥Â
	IDirect3DSurface9 * GetSurface();

	// Õº∆¨–≈œ¢
public:
	// º”‘ÿ≈–∂œ
	bool IsNull();
	// Œ∆¿ÌøÌ∂»
	int GetWidth() { return m_TextureSize.cx; }
	// Œ∆¿Ì∏ﬂ∂»
	int GetHeight() { return m_TextureSize.cy; }

	// µ˜ ‘∫Ø ˝
public:
	// …Ë÷√–˝◊™
	void SetRotation( EnumTextureRotationDirection chDirection, float fRotationX, float fRotationY, float fAngle );
	// …Ë÷√–˝◊™
	void SetRotation( EnumTextureRotationDirection chDirection, int nRotationX, int nRotationY, float fAngle );
	// …Ë÷√∂‘≥∆
	void SetSymmetry( bool bSymmetry );
	// …Ë÷√Õ∏√˜
	void SetTextureAlpha( byte cbAlpha );
	// …Ë÷√≥Àª˝
	void SetTextureColor( CD3DColor D3DColor );
	// …Ë÷√√Ë±ﬂ
	void SetTextureBorderColor( CD3DColor D3DColor );
	// …Ë÷√Ω•±‰
	void SetGradientColor( CD3DColor D3DColorTop, CD3DColor D3DColorBottom );
	// …Ë÷√…œ≤„
	void SetUpperTexture( CD3DTexture * pUpperTexture );
	// …Ë÷√Ãÿ–ß
	void SetEffectType( EnumTextureEffectType nEffectType );

	// Œ∆¿Ìπ‹¿Ì
public:
	// œ˙ªŸª∫≥Â
	bool Destory();

	// π¶ƒ‹∫Ø ˝
public:
	// ¥¥Ω®ª∫≥Â
	bool CreateBuffer( CD3DDevice * pD3DDevice, int nWidth, int nHeight );
	// ±£¥ÊÕº∆¨
	bool SaveImage( const tchar * pszSaveFile );

	// ªÊª≠∫Ø ˝
public:
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest);
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource);
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight);

	// Õ∏√˜ªÊª≠
public:
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, byte cbAlpha);
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, byte cbAlpha);
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, byte cbAlpha);

	// ≥Àª˝ªÊª≠
public:
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, CD3DColor D3DColor);
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, CD3DColor D3DColor);
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CD3DColor D3DColor);

	// ±‰–ŒªÊª≠
public:
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, CPoint ptDest[4], CRect rcImage );
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, CPoint ptDest[4], CRect rcImage, byte cbAlpha);
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, CPoint ptDest[4], CRect rcImage, CD3DColor D3DColor);

	// ªÊª≠π´∏Ê∞ÂÕº∆¨
public:
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize );
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize, byte cbAlpha );
	// ªÊª≠Õº∆¨
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize, CD3DColor D3DColor );
	// ªÊª≠Õº∆¨
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CD3DColor D3DColor );

	// ªÊª≠ø’º‰Õº∆¨
public:
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize );
	// ªÊª≠Õº∆¨
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize, byte cbAlpha );
	// ªÊª≠Õº∆¨
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize, CD3DColor D3DColor );
	// ªÊª≠Õº∆¨
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CD3DColor D3DColor );

	// »´æ÷∫Ø ˝
public:
	// ∂™ ß…Ë±∏
	static void OnLostDevice();
	// ÷ÿ÷√…Ë±∏
	static void OnResetDevice();
};


////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif