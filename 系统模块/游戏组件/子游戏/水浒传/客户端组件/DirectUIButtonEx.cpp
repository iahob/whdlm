#include "StdAfx.h"
#include ".\directuibuttonex.h"

BEGIN_WHDUI_MESSAGE_MAP(CDirectUIButtonEx, CDirectUIButton)
	ON_WHDUI_WINDOW_RENDER()
END_WHDUI_MESSAGE_MAP()

//构造函数
CDirectUIButtonEx::CDirectUIButtonEx(void)
{
}

//析构函数
CDirectUIButtonEx::~CDirectUIButtonEx(void)
{
}


// 加载纹理
void CDirectUIButtonEx::SetNumberTexture( CD3DTextureEx& Texture )
{
	m_D3DTextureNum.Destory();
	m_D3DTextureNum.LoadTexture(Texture);
	// 更新窗口
	if ( IsWindowActive() )
	{
		// 调整界面
		RectifyControl();

		// 更新界面
		if ( IsWindowVisible() ) InvalidWindow(false);
	}
}
// 加载位图
void CDirectUIButtonEx::SetNumberTexture( dex::CD3DDevice * pD3DDevice, hinstance hResInstance, const tchar * pszResource, const tchar * pszTypeName)
{
	// 加载纹理
	m_D3DTextureNum.Destory();
	m_D3DTextureNum.LoadTexture(pD3DDevice, hResInstance, pszResource, pszTypeName);

	// 更新窗口
	if ( IsWindowActive() )
	{
		// 调整界面
		RectifyControl();

		// 更新界面
		if ( IsWindowVisible() ) InvalidWindow(false);
	}
}

// 加载纹理
void CDirectUIButtonEx::SetNumberTexture( dex::CD3DDevice * pD3DDevice, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, const tchar * pszImageKey )
{
	// 加载纹理
	m_D3DTextureNum.Destory();
	m_D3DTextureNum.LoadWHGTexture(pD3DDevice, hInstance, pszResource, pszTypeName, pszImageKey);

	// 更新窗口
	if ( IsWindowActive() )
	{
		// 调整界面
		RectifyControl();

		// 更新界面
		if ( IsWindowVisible() ) InvalidWindow(false);
	}
}
// 设置数字
void CDirectUIButtonEx::SetNumber(int nNum)
{
	m_nNumber = nNum;
	// 更新窗口
	if ( IsWindowActive() )
	{
		// 调整界面
		RectifyControl();

		// 更新界面
		if ( IsWindowVisible() ) InvalidWindow(false);
	}
}

// 绘画消息
void CDirectUIButtonEx::OnWindowRender(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight)
{
	__super::OnWindowRender(pD3DDevice,nWidth,nHeight);


	if(m_D3DTextureNum.IsNull() == false)
	{
		//获取按钮大小
		CRect rt;
		GetWindowRect(rt);

		//变量设定
		int nNumCount=0;
		int nTempNum = m_nNumber;

		//数字长度
		do
		{
			nNumCount++;
			nTempNum = nTempNum/10;
		}while(nTempNum!=0);
		
		//数字大小
		CSize szTextureNum;
		szTextureNum.SetSize(m_D3DTextureNum.GetWidth()/10,m_D3DTextureNum.GetHeight());
		//绘画起点
		int nBeginX = (rt.Width()-nNumCount*szTextureNum.cx)/2;
		int nBeginY = (rt.Width()-szTextureNum.cy)/2;
		//绘画数字纹理
		int nRenderNum = m_nNumber;
		for(int i=0; i<nNumCount; i++)
		{
			m_D3DTextureNum.DrawTexture(pD3DDevice, nBeginX+(nNumCount-i-1)*szTextureNum.cx,nBeginY,szTextureNum.cx,szTextureNum.cy,
													nRenderNum%10*szTextureNum.cx,0,szTextureNum.cx,szTextureNum.cy);
			nRenderNum = nRenderNum/10;
		}
	}

}
