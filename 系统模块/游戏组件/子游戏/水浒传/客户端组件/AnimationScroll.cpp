#include "StdAfx.h"
#include "AnimationScroll.h"

CAnimationScroll::CAnimationScroll(void)
{
	m_bScrollX = false;
	m_nPixels = 1;
	m_nSteps = 1;
}

CAnimationScroll::~CAnimationScroll(void)
{
}


//动画驱动
bool CAnimationScroll::CartoonMovie()
{
	//动画
	if (m_bShowImage && m_pTextureImage != NULL && ++m_nLapseIndex >= m_nLapseCount)
	{
		m_nLapseIndex = 0;
		m_nCurIndex+=m_nSteps;
		if (m_nCurIndex % 7 == 0)
		{
			if (m_bScrollX)
			{
				int nImageWidth = m_pTextureImage[0].GetWidth() / m_nImageX;
				if ((m_nSteps + 1) * m_nPixels < nImageWidth * 3 / 4)
				{
					m_nSteps++;
				}
			}
			else
			{				
				int nImageHeight = m_pTextureImage[0].GetHeight() / m_nImageY;
				if ((m_nSteps + 1) * m_nPixels < nImageHeight * 3 / 4)
				{
					m_nSteps++;
				}
			}			
		}
		
		if (m_nCurIndex >= m_nMaxIndex)
		{
			if (m_enPlayMode == enOnce)
			{
				m_bShowImage = false;
			}

			m_nCurIndex = 0;
		}

		return true;
	}

	return false;
}


//绘画界面
VOID CAnimationScroll::DrawAnimationScroll(dex::CD3DDevice * pD3DDevice)
{
	DrawAnimationScroll(pD3DDevice, m_BenchmarkPos.x, m_BenchmarkPos.y, m_XCollocateMode, m_YCollocateMode);
}

//绘画界面
VOID CAnimationScroll::DrawAnimationScroll(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, enXCollocateMode XCollocateMode/* = enXCenter*/, enYCollocateMode YCollocateMode/* = enYCenter*/)
{
	DrawAnimationScroll(pD3DDevice, nXPos, nYPos, m_nCurIndex, XCollocateMode, YCollocateMode);
}

//绘画界面
VOID CAnimationScroll::DrawAnimationScroll(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, INT nDrawIndex, enXCollocateMode XCollocateMode/* = enXCenter*/, enYCollocateMode YCollocateMode/* = enYCenter*/)
{
	ASSERT(nDrawIndex < m_nMaxIndex);
	if (!m_bShowImage || m_pTextureImage == NULL || nDrawIndex >= m_nMaxIndex) return;

	int nShowX = nXPos;
	int nShowY = nYPos;

	if (m_nImageCount > 1)
	{
		if (XCollocateMode == enXCenter)
		{
			nShowX -= m_pTextureImage[nDrawIndex].GetWidth() / 2;
		}
		else if (XCollocateMode == enXRight)
		{
			nShowX -= m_pTextureImage[nDrawIndex].GetWidth();
		}

		if (YCollocateMode == enYCenter)
		{
			nShowY -= m_pTextureImage[nDrawIndex].GetHeight() / 2;
		}
		else if (YCollocateMode == enYBottom)
		{
			nShowY -= m_pTextureImage[nDrawIndex].GetHeight();
		}

		m_pTextureImage[nDrawIndex].DrawTexture(pD3DDevice, nShowX, nShowY);
	}
	else
	{
		DrawAnimationScrollPos(pD3DDevice, nXPos, nYPos, nDrawIndex * m_nPixels, XCollocateMode, YCollocateMode);		
	}
}

//绘画界面
VOID CAnimationScroll::DrawAnimationScrollPos(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, INT nDrawPos, enXCollocateMode XCollocateMode/* = enXCenter*/, enYCollocateMode YCollocateMode/* = enYCenter*/)
{
	if (nDrawPos < 0)
	{
		nDrawPos = 0;
	}
	int nImageWidth = m_pTextureImage[0].GetWidth() / m_nImageX;
	int nImageHeight = m_pTextureImage[0].GetHeight() / m_nImageY;

	int nShowX = nXPos;
	int nShowY = nYPos;

	if (XCollocateMode == enXCenter)
	{
		nShowX -= nImageWidth / 2;
	}
	else if (XCollocateMode == enXRight)
	{
		nShowX -= nImageWidth;
	}

	if (YCollocateMode == enYCenter)
	{
		nShowY -= nImageHeight / 2;
	}
	else if (YCollocateMode == enYBottom)
	{
		nShowY -= nImageHeight;
	}

	if (m_bScrollX)
	{
		m_pTextureImage[0].DrawTexture(pD3DDevice, nShowX, nShowY, nImageWidth, nImageHeight, 
			nDrawPos, 0);
	}
	else
	{
		int nDrarSize = (nDrawPos + nImageHeight) - m_pTextureImage->GetHeight();
		if (nDrarSize > 0)
		{
			m_pTextureImage[0].DrawTexture(pD3DDevice, nShowX, nShowY, nImageWidth, nImageHeight - nDrarSize, 
				0, nDrawPos);

			m_pTextureImage[0].DrawTexture(pD3DDevice, nShowX, nShowY + (nImageHeight - nDrarSize), nImageWidth, nDrarSize, 
				0, 0);
		}
		else
		{
			m_pTextureImage[0].DrawTexture(pD3DDevice, nShowX, nShowY, nImageWidth, nImageHeight, 
				0, nDrawPos);
		}
	}
}

//启动动画
VOID CAnimationScroll::ShowAnimation( bool bShow, int nLapseCount /*= 1*/, enPlayMode PlayMode/* = enOnce*/)
{
	m_nSteps = 1;
	m_bShowImage = bShow;
	m_nLapseIndex = 0;
	m_nLapseCount = nLapseCount;
	m_enPlayMode = PlayMode;

	if (bShow)
	{
		m_nCurIndex = 0;
	}	
}

//配置设备
bool CAnimationScroll::InitAnimationSingle(dex::CD3DDevice * pD3DDevice, CString strImageName, CString strImageKey, int nImageX, int nImageY, bool bScrollX, int nPixels)
{
	__super::InitAnimationSingle(pD3DDevice, strImageName, strImageKey, nImageX, nImageY);

	m_bScrollX = bScrollX;
	m_nPixels = nPixels;
	m_nCurIndex = 0;
	if (m_pTextureImage != NULL)
	{
		if (bScrollX)
		{
			m_nMaxIndex = m_pTextureImage->GetWidth() / nPixels;
		}
		else
		{
			m_nMaxIndex = m_pTextureImage->GetHeight() / nPixels;
		}

		return true;
	}
	
	return false;
}