#include "StdAfx.h"
#include "Animation.h"

CAnimation::CAnimation(void)
{
	m_bInit = false;
	m_bShowImage = false;
	m_nImageCount = 0;
	m_nImageX = 1;
	m_nImageY = 1;
	m_nLapseIndex = 0;
	m_nLapseCount = 50;
	m_bSan = false;
	m_bSanShow = true;

	m_BenchmarkPos.SetPoint(0, 0);
	m_XCollocateMode = enXCenter;
	m_YCollocateMode = enYCenter;

	m_bSendFinish = false;
	m_nCurIndex = 0;
	m_nMaxIndex = 0;
	m_nSendIndex = m_nMaxIndex;
	m_enPlayMode = enOnce;
	m_pTextureImage = NULL;
}

CAnimation::~CAnimation(void)
{
	//删除图片
	DeleteImamge();
}


//删除图片
VOID  CAnimation::DeleteImamge()
{
	if (m_pTextureImage != NULL)
	{
		for (int i = 0; i < m_nImageCount; i++)
		{
			if (!m_pTextureImage[i].IsNull())
			{
				m_pTextureImage[i].Destory();
			}			
		}

		delete []m_pTextureImage;
		m_pTextureImage = NULL;
	}
	m_bInit = false;
}

//动画驱动
bool CAnimation::CartoonMovie()
{
	if (m_bShowImage && m_pTextureImage != NULL && ++m_nLapseIndex >= m_nLapseCount )
	{
		m_nLapseIndex = 0;
		m_nCurIndex++;
		if(m_bSan)
		{
			m_bSanShow = !m_bSanShow;
		}
		if (m_nCurIndex >= m_nMaxIndex)
		{
			m_nCurIndex = m_nMaxIndex;
		}

		if (m_bSendFinish && m_nCurIndex == m_nSendIndex && m_nSendIndex != m_nMaxIndex && m_enPlayMode == enOnce)
		{
			m_nSendIndex = m_nMaxIndex;
			//发送通知
			CGameFrameView::GetInstance()->SendEngineMessage(IDM_USER_ACTION_FINISH, (WPARAM)this, 0);
		}

		if (m_nCurIndex >= m_nMaxIndex)
		{
			if (m_enPlayMode == enOnce)
			{
				m_bShowImage = false;

				if (m_bSendFinish)
				{
					//发送通知
					CGameFrameView::GetInstance()->SendEngineMessage(IDM_USER_ACTION_FINISH, (WPARAM)this, 0);
				}
			}				
			m_nCurIndex = 0;
		}

		return true;
	}

	return false;
}

//配置设备
bool CAnimation::InitAnimation(dex::CD3DDevice * pD3DDevice, CString strImageName, CString strImageKey, int nImageCount, int nImagePageCount)
{	
	ASSERT(nImageCount > 0);

	//删除图片
	DeleteImamge();

	if (nImageCount > 0)
	{
		m_pTextureImage = new CD3DTextureEx[nImageCount];
	}

	if (m_pTextureImage != NULL)
	{
		CString strImageNameTemp;
		CString strImageKeyTemp;
		int nPageCount = nImageCount / nImagePageCount;
		if(nImageCount % nImagePageCount != 0)
		{
			nPageCount++;
		}
		ASSERT(nPageCount > 0);
		for (int i = 0; i < nImageCount; i++)
		{
			if(nPageCount > 1)
			{
				strImageNameTemp.Format(TEXT("%s-%d"), strImageName, i / nImagePageCount);
			}
			else
			{
				strImageNameTemp = strImageName;
			}
			strImageKeyTemp.Format(TEXT("%s%02d"), strImageKey, i);
			m_pTextureImage[i].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, strImageNameTemp, _T("资源"), strImageKeyTemp);

			//加载失败
			if (m_pTextureImage[i].IsNull())
			{
				ASSERT(NULL);
				return false;
			}
		}		
	}

	m_bInit = true;
	m_nImageCount = nImageCount;
	m_nImageX = 1;
	m_nImageY = 1;
	m_nCurIndex = 0;
	m_nMaxIndex = nImageCount;
	m_nSendIndex = m_nMaxIndex;

	return true;
}

//配置设备
bool CAnimation::InitAnimationSingle(dex::CD3DDevice * pD3DDevice, CString strImageName, CString strImageKey, int nImageX, int nImageY)
{
	ASSERT(nImageX > 0 && nImageY > 0);

	//删除图片
	DeleteImamge();

	m_pTextureImage = new CD3DTextureEx[1];

	if (m_pTextureImage != NULL)
	{
		m_pTextureImage[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, strImageName, _T("资源"), strImageKey);

		//加载失败
		if (m_pTextureImage[0].IsNull())
		{
			return false;
		}
	}

	m_bInit = true;
	m_nImageCount = 1;
	m_nImageX = nImageX;
	m_nImageY = nImageY;
	m_nCurIndex = 0;
	m_nMaxIndex = nImageX * nImageY;
	m_nSendIndex = m_nMaxIndex;

	return true;
}

//绘画界面
VOID CAnimation::DrawAnimation(dex::CD3DDevice * pD3DDevice)
{
	DrawAnimation(pD3DDevice, m_BenchmarkPos.x, m_BenchmarkPos.y, m_XCollocateMode, m_YCollocateMode);
}

//绘画界面
VOID CAnimation::DrawAnimation(dex::CD3DDevice * pD3DDevice, INT nDrawIndex)
{
	DrawAnimation(pD3DDevice, m_BenchmarkPos.x, m_BenchmarkPos.y, nDrawIndex, m_XCollocateMode, m_YCollocateMode);
}

//绘画界面
VOID CAnimation::DrawAnimation(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, enXCollocateMode XCollocateMode/* = enXCenter*/, enYCollocateMode YCollocateMode/* = enYCenter*/)
{
	DrawAnimation(pD3DDevice, nXPos, nYPos, m_nCurIndex, XCollocateMode, YCollocateMode);
}

//绘画界面
VOID CAnimation::DrawAnimation(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, INT nDrawIndex, enXCollocateMode XCollocateMode/* = enXCenter*/, enYCollocateMode YCollocateMode/* = enYCenter*/)
{	
	if (!m_bShowImage || m_pTextureImage == NULL || nDrawIndex >= m_nMaxIndex) return;

	if(m_bSan)
	{
		if(!m_bSanShow)
		{
			return;
		}
	}
	ASSERT(nDrawIndex < m_nMaxIndex);
	int nShowX = nXPos;
	int nShowY = nYPos;

	if (m_nImageCount > 1)
	{
		if (!m_pTextureImage[nDrawIndex].IsNull())
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
	}
	else
	{
		int nImageWidth = m_pTextureImage[0].GetWidth() / m_nImageX;
		int nImageHeight = m_pTextureImage[0].GetHeight() / m_nImageY;

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


		m_pTextureImage[0].DrawTexture(pD3DDevice, nShowX, nShowY, nImageWidth, nImageHeight, 
			(nDrawIndex % m_nImageX) * nImageWidth, (nDrawIndex / m_nImageX) * nImageHeight);
	}
}

//启动动画
VOID CAnimation::ShowAnimation( bool bShow, int nLapseCount /*= 1*/, enPlayMode PlayMode/* = enOnce*/)
{
	m_bShowImage = bShow;
	m_nLapseIndex = 0;
	m_nLapseCount = nLapseCount;
	m_enPlayMode = PlayMode;
	m_bSendFinish = false;
	if (bShow)
	{
		m_nCurIndex = 0;
	}	
}

//启动动画
VOID CAnimation::ShowAnimationEx( bool bShow, int nLapseCount /*= 1*/, enPlayMode PlayMode/* = enOnce*/)
{
	m_bShowImage = bShow;
	m_nLapseIndex = 0;
	m_nLapseCount = nLapseCount;
	m_enPlayMode = PlayMode;
	m_bSendFinish = true;
	if (bShow)
	{
		m_nCurIndex = 0;
	}	
}

//停止动画
VOID CAnimation::StopAnimation()
{
	if (m_bShowImage && m_bSendFinish && m_nCurIndex <= m_nSendIndex && m_nSendIndex != m_nMaxIndex && m_enPlayMode == enOnce)
	{
		//发送通知
		CGameFrameView::GetInstance()->SendEngineMessage(IDM_USER_ACTION_FINISH, (WPARAM)this, 0);
	}

	if (m_bShowImage && m_nCurIndex < m_nMaxIndex)
	{
		if (m_enPlayMode == enOnce)
		{
			m_bShowImage = false;

			if (m_bSendFinish)
			{
				//发送通知
				CGameFrameView::GetInstance()->SendEngineMessage(IDM_USER_ACTION_FINISH, (WPARAM)this, 0);
			}
		}				
		m_nCurIndex = 0;
	}
}

//通知索引
VOID CAnimation::SetSendIndex(int nSendIndex)
{
	m_nSendIndex = nSendIndex;
}

//基准位置
VOID CAnimation::SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode/* = enXCenter*/, enYCollocateMode YCollocateMode/* = enYCenter*/)
{
	//设置变量
	m_BenchmarkPos.x=nXPos;
	m_BenchmarkPos.y=nYPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;
}