#include "StdAfx.h"
#include ".\d3dtextureex.h"

int						CD3DTextureEx::m_nShowWidth = Screen_Width;						//��ʾ��С 
int						CD3DTextureEx::m_nShowHeight = Screen_Height;						//��ʾ��С 
int						CD3DTextureEx::m_nWidth = Screen_Width;							//ʵ�ʴ�С 
int						CD3DTextureEx::m_nHeight = Screen_Height;							//ʵ�ʴ�С 

CD3DTextureEx::CD3DTextureEx(void)
{
}

CD3DTextureEx::~CD3DTextureEx(void)
{
}

// �滭ͼƬ
bool CD3DTextureEx::DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest)
{
	return __super::DrawTexture(pD3DDevice, nXDest * m_nShowWidth / m_nWidth, nYDest * m_nShowHeight / m_nHeight, 
		GetWidth() * m_nShowWidth / m_nWidth, GetHeight() * m_nShowHeight / m_nHeight, 0, 0, GetWidth(), GetHeight());
}

// �滭ͼƬ
bool CD3DTextureEx::DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource)
{
	return __super::DrawTexture(pD3DDevice, nXDest * m_nShowWidth / m_nWidth, nYDest * m_nShowHeight / m_nHeight, 
		nDestWidth * m_nShowWidth / m_nWidth, nDestHeight * m_nShowHeight / m_nHeight, nXSource, nYSource, nDestWidth, nDestHeight);
}

// �滭ͼƬ
bool CD3DTextureEx::DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight)
{
	return __super::DrawTexture(pD3DDevice, nXDest * m_nShowWidth / m_nWidth, nYDest * m_nShowHeight / m_nHeight, 
		nDestWidth * m_nShowWidth / m_nWidth, nDestHeight * m_nShowHeight / m_nHeight, nXSource, nYSource, nSourceWidth, nSourceHeight);
}

// �滭ͼƬ
bool CD3DTextureEx::DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, byte cbAlpha)
{
	return __super::DrawTexture(pD3DDevice, nXDest * m_nShowWidth / m_nWidth, nYDest * m_nShowHeight / m_nHeight, 
		nDestWidth * m_nShowWidth / m_nWidth, nDestHeight * m_nShowHeight / m_nHeight, nXSource, nYSource, nSourceWidth, nSourceHeight, cbAlpha);
}