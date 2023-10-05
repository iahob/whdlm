#include "StdAfx.h"
#include ".\d3dtextureindex.h"

CD3DTextureIndex::CD3DTextureIndex(void)
{
	m_nWidthIndex = 0;
	m_nHeightIndex = 0;
}

CD3DTextureIndex::~CD3DTextureIndex(void)
{
}

// Ë÷ÒýÔØÈë
bool CD3DTextureIndex::LoadImage( CD3DDevice * pD3DDevice, HINSTANCE hInstance, int nWidthIndex, int nHeightIndex, LPCTSTR pszResource, LPCTSTR pszTypeName )
{
	ASSERT( nWidthIndex > 0 && nHeightIndex > 0 );
	m_nWidthIndex = nWidthIndex;
	m_nHeightIndex = nHeightIndex;
	return CD3DTexture::LoadImage( pD3DDevice, hInstance, pszResource, pszTypeName );
}

// Ë÷Òý»æ»­
bool CD3DTextureIndex::DrawImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, INT nIndex )
{
	// ³¬³öË÷Òý »æ»­È«²¿
	if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	{
		return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest );
	}

	// Ë÷Òý»æ»­
	int nWidthIndex = nIndex%m_nWidthIndex;
	int nHeightIndex = nIndex/m_nWidthIndex;
	CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy );
}

// Ë÷Òý»æ»­
bool CD3DTextureIndex::DrawImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex )
{
	// ³¬³öË÷Òý »æ»­È«²¿
	if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	{
		return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, cbAlpha );
	}

	// Ë÷Òý»æ»­
	int nWidthIndex = nIndex%m_nWidthIndex;
	int nHeightIndex = nIndex/m_nWidthIndex;
	CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy, cbAlpha );
}

// Ë÷Òý»æ»­
bool CD3DTextureIndex::DrawImageIndex( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, INT nIndex )
{
	// ³¬³öË÷Òý »æ»­È«²¿
	if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	{
		return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest );
	}

	// Ë÷Òý»æ»­
	int nWidthIndex = nIndex%m_nWidthIndex;
	int nHeightIndex = nIndex/m_nWidthIndex;
	CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy );
}

// Ë÷Òý»æ»­
bool CD3DTextureIndex::DrawImageIndex( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex )
{
	// ³¬³öË÷Òý »æ»­È«²¿
	if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	{
		return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, cbAlpha );
	}

	// Ë÷Òý»æ»­
	int nWidthIndex = nIndex%m_nWidthIndex;
	int nHeightIndex = nIndex/m_nWidthIndex;
	CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy, cbAlpha );
}


// Ë÷Òý»æ»­
bool CD3DTextureIndex::DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, INT nIndex )
{
	return true;

	////³¬³öË÷Òý »æ»­È«²¿
	// if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	// {
	// 	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest );
	// }

	////Ë÷Òý»æ»­
	// int nWidthIndex = nIndex%m_nWidthIndex;
	// int nHeightIndex = nIndex/m_nWidthIndex;
	// CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	// return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy );
}

// Ë÷Òý»æ»­
bool CD3DTextureIndex::DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex )
{
	return true;

	////³¬³öË÷Òý »æ»­È«²¿
	// if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	// {
	// 	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, cbAlpha );
	// }

	////Ë÷Òý»æ»­
	// int nWidthIndex = nIndex%m_nWidthIndex;
	// int nHeightIndex = nIndex/m_nWidthIndex;
	// CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	// return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy, cbAlpha );
}

// Ë÷Òý»æ»­
bool CD3DTextureIndex::DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, INT nIndex )
{
	return true;

	////³¬³öË÷Òý »æ»­È«²¿
	// if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	// {
	// 	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest );
	// }

	////Ë÷Òý»æ»­
	// int nWidthIndex = nIndex%m_nWidthIndex;
	// int nHeightIndex = nIndex/m_nWidthIndex;
	// CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	// return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy );
}

// Ë÷Òý»æ»­
bool CD3DTextureIndex::DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex )
{
	return true;

	////³¬³öË÷Òý »æ»­È«²¿
	// if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	// {
	// 	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest );
	// }

	////Ë÷Òý»æ»­
	// int nWidthIndex = nIndex%m_nWidthIndex;
	// int nHeightIndex = nIndex/m_nWidthIndex;
	// CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	// return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy, cbAlpha );
}


