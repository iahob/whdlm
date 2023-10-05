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

// ��������
bool CD3DTextureIndex::LoadImage( CD3DDevice * pD3DDevice, HINSTANCE hInstance, int nWidthIndex, int nHeightIndex, LPCTSTR pszResource, LPCTSTR pszTypeName )
{
	ASSERT( nWidthIndex > 0 && nHeightIndex > 0 );
	m_nWidthIndex = nWidthIndex;
	m_nHeightIndex = nHeightIndex;
	return CD3DTexture::LoadImage( pD3DDevice, hInstance, pszResource, pszTypeName );
}

// �����滭
bool CD3DTextureIndex::DrawImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, INT nIndex )
{
	// �������� �滭ȫ��
	if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	{
		return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest );
	}

	// �����滭
	int nWidthIndex = nIndex%m_nWidthIndex;
	int nHeightIndex = nIndex/m_nWidthIndex;
	CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy );
}

// �����滭
bool CD3DTextureIndex::DrawImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex )
{
	// �������� �滭ȫ��
	if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	{
		return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, cbAlpha );
	}

	// �����滭
	int nWidthIndex = nIndex%m_nWidthIndex;
	int nHeightIndex = nIndex/m_nWidthIndex;
	CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy, cbAlpha );
}

// �����滭
bool CD3DTextureIndex::DrawImageIndex( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, INT nIndex )
{
	// �������� �滭ȫ��
	if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	{
		return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest );
	}

	// �����滭
	int nWidthIndex = nIndex%m_nWidthIndex;
	int nHeightIndex = nIndex/m_nWidthIndex;
	CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy );
}

// �����滭
bool CD3DTextureIndex::DrawImageIndex( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex )
{
	// �������� �滭ȫ��
	if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	{
		return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, cbAlpha );
	}

	// �����滭
	int nWidthIndex = nIndex%m_nWidthIndex;
	int nHeightIndex = nIndex/m_nWidthIndex;
	CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy, cbAlpha );
}


// �����滭
bool CD3DTextureIndex::DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, INT nIndex )
{
	return true;

	////�������� �滭ȫ��
	// if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	// {
	// 	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest );
	// }

	////�����滭
	// int nWidthIndex = nIndex%m_nWidthIndex;
	// int nHeightIndex = nIndex/m_nWidthIndex;
	// CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	// return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy );
}

// �����滭
bool CD3DTextureIndex::DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex )
{
	return true;

	////�������� �滭ȫ��
	// if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	// {
	// 	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, cbAlpha );
	// }

	////�����滭
	// int nWidthIndex = nIndex%m_nWidthIndex;
	// int nHeightIndex = nIndex/m_nWidthIndex;
	// CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	// return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy, cbAlpha );
}

// �����滭
bool CD3DTextureIndex::DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, INT nIndex )
{
	return true;

	////�������� �滭ȫ��
	// if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	// {
	// 	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest );
	// }

	////�����滭
	// int nWidthIndex = nIndex%m_nWidthIndex;
	// int nHeightIndex = nIndex/m_nWidthIndex;
	// CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	// return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy );
}

// �����滭
bool CD3DTextureIndex::DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex )
{
	return true;

	////�������� �滭ȫ��
	// if ( nIndex >= m_nWidthIndex * m_nHeightIndex )
	// {
	// 	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest );
	// }

	////�����滭
	// int nWidthIndex = nIndex%m_nWidthIndex;
	// int nHeightIndex = nIndex/m_nWidthIndex;
	// CSize sizeIndex(m_ImageSize.cx / m_nWidthIndex, m_ImageSize.cy / m_nHeightIndex);
	// return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, sizeIndex.cx, sizeIndex.cy, sizeIndex.cx * nWidthIndex, sizeIndex.cy * nHeightIndex, sizeIndex.cx, sizeIndex.cy, cbAlpha );
}


