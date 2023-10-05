#include "StdAfx.h"
#include ".\directuibuttonexx.h"


BEGIN_WHDUI_MESSAGE_MAP(CDirectUIButtonExx, CDirectUIButton)
	ON_WHDUI_WINDOW_RENDER()
END_WHDUI_MESSAGE_MAP()

CDirectUIButtonExx::CDirectUIButtonExx(void)
{
}

CDirectUIButtonExx::~CDirectUIButtonExx(void)
{
}

// 绘画消息
void CDirectUIButtonExx::OnWindowRender(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight)
{
	if(m_bVisible && !m_D3DTextureButton.IsNull())
	{
		//获取按钮大小
		CRect rt;
		GetWindowRect(rt);
		if(!m_bEnable)
		{
			m_D3DTextureButton.DrawTexture(pD3DDevice, 0, 0, rt.Width(), rt.Height(), m_D3DTextureButton.GetWidth() / 5 * 4, 0,  m_D3DTextureButton.GetWidth() / 5,  m_D3DTextureButton.GetHeight());
		}
		else if(m_bMouseMove)
		{
			m_D3DTextureButton.DrawTexture(pD3DDevice, 0, 0, rt.Width(), rt.Height(), m_D3DTextureButton.GetWidth() / 5 * 2, 0,  m_D3DTextureButton.GetWidth() / 5,  m_D3DTextureButton.GetHeight());
		}
		else if(m_bMouseDown)
		{
			m_D3DTextureButton.DrawTexture(pD3DDevice, 0, 0, rt.Width(), rt.Height(), m_D3DTextureButton.GetWidth() / 5 * 1, 0,  m_D3DTextureButton.GetWidth() / 5,  m_D3DTextureButton.GetHeight());
		}
		else
		{
			m_D3DTextureButton.DrawTexture(pD3DDevice, 0, 0, rt.Width(), rt.Height(), 0, 0,  m_D3DTextureButton.GetWidth() / 5,  m_D3DTextureButton.GetHeight());
		}
	}
}