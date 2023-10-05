#include "StdAfx.h"
#include ".\directuibuttonex.h"

BEGIN_WHDUI_MESSAGE_MAP(CDirectUIButtonEx, CDirectUIButton)
	ON_WHDUI_WINDOW_RENDER()
END_WHDUI_MESSAGE_MAP()

//���캯��
CDirectUIButtonEx::CDirectUIButtonEx(void)
{
}

//��������
CDirectUIButtonEx::~CDirectUIButtonEx(void)
{
}


// ��������
void CDirectUIButtonEx::SetNumberTexture( CD3DTextureEx& Texture )
{
	m_D3DTextureNum.Destory();
	m_D3DTextureNum.LoadTexture(Texture);
	// ���´���
	if ( IsWindowActive() )
	{
		// ��������
		RectifyControl();

		// ���½���
		if ( IsWindowVisible() ) InvalidWindow(false);
	}
}
// ����λͼ
void CDirectUIButtonEx::SetNumberTexture( dex::CD3DDevice * pD3DDevice, hinstance hResInstance, const tchar * pszResource, const tchar * pszTypeName)
{
	// ��������
	m_D3DTextureNum.Destory();
	m_D3DTextureNum.LoadTexture(pD3DDevice, hResInstance, pszResource, pszTypeName);

	// ���´���
	if ( IsWindowActive() )
	{
		// ��������
		RectifyControl();

		// ���½���
		if ( IsWindowVisible() ) InvalidWindow(false);
	}
}

// ��������
void CDirectUIButtonEx::SetNumberTexture( dex::CD3DDevice * pD3DDevice, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, const tchar * pszImageKey )
{
	// ��������
	m_D3DTextureNum.Destory();
	m_D3DTextureNum.LoadWHGTexture(pD3DDevice, hInstance, pszResource, pszTypeName, pszImageKey);

	// ���´���
	if ( IsWindowActive() )
	{
		// ��������
		RectifyControl();

		// ���½���
		if ( IsWindowVisible() ) InvalidWindow(false);
	}
}
// ��������
void CDirectUIButtonEx::SetNumber(int nNum)
{
	m_nNumber = nNum;
	// ���´���
	if ( IsWindowActive() )
	{
		// ��������
		RectifyControl();

		// ���½���
		if ( IsWindowVisible() ) InvalidWindow(false);
	}
}

// �滭��Ϣ
void CDirectUIButtonEx::OnWindowRender(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight)
{
	__super::OnWindowRender(pD3DDevice,nWidth,nHeight);


	if(m_D3DTextureNum.IsNull() == false)
	{
		//��ȡ��ť��С
		CRect rt;
		GetWindowRect(rt);

		//�����趨
		int nNumCount=0;
		int nTempNum = m_nNumber;

		//���ֳ���
		do
		{
			nNumCount++;
			nTempNum = nTempNum/10;
		}while(nTempNum!=0);
		
		//���ִ�С
		CSize szTextureNum;
		szTextureNum.SetSize(m_D3DTextureNum.GetWidth()/10,m_D3DTextureNum.GetHeight());
		//�滭���
		int nBeginX = (rt.Width()-nNumCount*szTextureNum.cx)/2;
		int nBeginY = (rt.Width()-szTextureNum.cy)/2;
		//�滭��������
		int nRenderNum = m_nNumber;
		for(int i=0; i<nNumCount; i++)
		{
			m_D3DTextureNum.DrawTexture(pD3DDevice, nBeginX+(nNumCount-i-1)*szTextureNum.cx,nBeginY,szTextureNum.cx,szTextureNum.cy,
													nRenderNum%10*szTextureNum.cx,0,szTextureNum.cx,szTextureNum.cy);
			nRenderNum = nRenderNum/10;
		}
	}

}
