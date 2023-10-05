#include "StdAfx.h"
#include ".\windowover.h"

CWindowOver::CWindowOver(void)
{
	m_lPlayScore = 0;
}

CWindowOver::~CWindowOver(void)
{
}

//������Ϣ
VOID CWindowOver::OnWindowMovie()
{

}

//������Ϣ
VOID CWindowOver::OnWindowCreate( CD3DDevice * pD3DDevice )
{
	//���뱳��
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageBack.LoadImage(pD3DDevice, hResInstance, TEXT("OVER_BACK"), TEXT("OVER"));
	m_ImageNumber.LoadImage(pD3DDevice, hResInstance, TEXT("OVER_NUMBER"), TEXT("OVER"));

	//��ȡ��С
	CSize SizeGameOver;
	SizeGameOver.SetSize(m_ImageBack.GetWidth(), m_ImageBack.GetHeight());

	//�ƶ�����
	SetWindowPos(0,0,SizeGameOver.cx,SizeGameOver.cy,0);
}

//����¼�
VOID CWindowOver::OnEventMouse( UINT uMessage, UINT nFlags, int nXMousePos, int nYMousePos )
{
}

//��ť�¼�
VOID CWindowOver::OnEventButton( UINT uButtonID, UINT uMessage, int nXMousePos, int nYMousePos )
{
	
}

//�滭����
VOID CWindowOver::OnEventDrawWindow( CD3DDevice * pD3DDevice, int nXOriginPos, int nYOriginPos )
{
	m_ImageBack.DrawImage(pD3DDevice, nXOriginPos, nYOriginPos);

	TCHAR szNumber[128];
	_sntprintf(szNumber, CountArray(szNumber), TEXT("%I64d"), m_lPlayScore);
	DrawNumber(pD3DDevice, &m_ImageNumber, TEXT("-0123456789"), szNumber, nXOriginPos + 179, nYOriginPos + 76, DT_CENTER);
}


// �滭����
void CWindowOver::DrawNumber(CD3DDevice * pD3DDevice , CD3DTexture* pImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/, BYTE cbAlpha /*= 255*/)
{
	// ������Դ
	INT nNumberHeight=pImageNumber->GetHeight();
	INT nNumberWidth=pImageNumber->GetWidth()/lstrlen(szImageNum);

	if ( uFormat == DT_CENTER )
	{
		nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
	}
	else if ( uFormat == DT_RIGHT )
	{
		nXPos -= lstrlen(szOutNum) * nNumberWidth;
	}

	for ( INT i = 0; i < lstrlen(szOutNum); ++i )
	{
		for ( INT j = 0; j < lstrlen(szImageNum); ++j )
		{
			if ( szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0' )
			{
				pImageNumber->DrawImage(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight, cbAlpha);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}


//�滭�ַ�
VOID CWindowOver::DrawTextString(CD3DDevice* pD3DDevice, CD3DFont * pD3DFont, LPCTSTR pszString, CRect rcDraw, UINT nFormat, COLORREF crText, COLORREF crFrame)
{
	//��������
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//�滭�߿�
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		//����λ��
		CRect rcFrame;
		rcFrame.top=rcDraw.top+nYExcursion[i];
		rcFrame.left=rcDraw.left+nXExcursion[i];
		rcFrame.right=rcDraw.right+nXExcursion[i];
		rcFrame.bottom=rcDraw.bottom+nYExcursion[i];

		//�滭�ַ�
		pD3DFont->DrawText(pD3DDevice, pszString,&rcFrame,nFormat,crFrame);
	}

	//�滭�ַ�
	pD3DFont->DrawText(pD3DDevice, pszString,&rcDraw, nFormat, crText);

	return;
}