#include "StdAfx.h"
#include "RememberCardWnd.h"

//////////////////////////////////////////////////////////////////////////////////
typedef list<BYTE>::iterator IT;

CRememberCardWnd::CRememberCardWnd()
{
	InitializeCardList();
}

CRememberCardWnd::~CRememberCardWnd()
{
	DestroyCardList();
}

VOID CRememberCardWnd::OnWindowCreate(CD3DDevice * pD3DDevice)
{
	//������Դ
	m_TextureRememberCard.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("GAME_CARD_REMEMBER"),TEXT("PNG"));
	m_TextureRememberNumber.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("REMEMBER_NUM"),TEXT("PNG"));
	m_TextureWndBack.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("REMEMBER_BACK"),TEXT("PNG"));
	
	m_sizeRememberCard.SetSize(m_TextureRememberCard.GetWidth()/13,m_TextureRememberCard.GetHeight()/2);

	CSize sizeWnd;
	sizeWnd.SetSize(m_TextureWndBack.GetWidth(),m_TextureWndBack.GetHeight());
	SetWindowPos(0,0,sizeWnd.cx,sizeWnd.cy,0);
}

VOID CRememberCardWnd::OnWindowDestory(CD3DDevice * pD3DDevice)
{
	if(m_TextureRememberCard.IsNull() == false)
		m_TextureRememberCard.Destory();
	if(m_TextureRememberNumber.IsNull() == false)
		m_TextureRememberNumber.Destory();
	if(m_TextureWndBack.IsNull() == false)
		m_TextureWndBack.Destory();
}

VOID CRememberCardWnd::OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos)
{
	//�滭����
	m_TextureWndBack.DrawImage(pD3DDevice,nXOriginPos,nYOriginPos);
	//��׼λ��
	INT nBrenchX = nXOriginPos + 115;
	INT nBrenchY = nYOriginPos + 20;
	//���
	INT nGapX = 45;
	INT nGapY = 78;
	//��ʱ�˿�
	BYTE cbCardDataArray[CARD_VALUE_COUNT];
	ZeroMemory(cbCardDataArray,sizeof(cbCardDataArray));
	for(WORD i = 0;i < 13;i++)
	{
		cbCardDataArray[i] = i+1;
	}
	//��С��
	cbCardDataArray[13] = 0x4E;
	cbCardDataArray[14] = 0x4F;
	//��������
	INT nXDrawPos=0,nYDrawPos=0;
	INT nXImagePos=0,nYImagePos=0;
	//�滭��
	for(int i = CARD_VALUE_COUNT - 1;i >= 0;i--)
	{
		BYTE cbCardData = cbCardDataArray[i];
		if ((cbCardData==0x4E)||(cbCardData==0x4F))      
		{
			nXImagePos = ((cbCardData&MASK_VALUE)%14)*m_sizeRememberCard.cx;
			nYImagePos = m_sizeRememberCard.cy;
		}
		else
		{
			nXImagePos= ((cbCardData&MASK_VALUE)-1)*m_sizeRememberCard.cx;
			nYImagePos= 0;
		}
		//�滭�˿�
		m_TextureRememberCard.DrawImage(pD3DDevice,nBrenchX + nGapX * nXDrawPos,nBrenchY + nGapY *nYDrawPos,m_sizeRememberCard.cx,m_sizeRememberCard.cy,nXImagePos,nYImagePos);
		
		DrawNumber(pD3DDevice,&m_TextureRememberNumber,TEXT("0123456789"),m_ListCard[i].size(),nBrenchX + nGapX * nXDrawPos +9,nBrenchY + nGapY *nYDrawPos+45);
		if(m_ListCard[i].size() == 0)
		{
			m_TextureRememberCard.DrawImage(pD3DDevice,nBrenchX + nGapX * nXDrawPos,nBrenchY + nGapY *nYDrawPos,m_sizeRememberCard.cx,m_sizeRememberCard.cy,m_sizeRememberCard.cx*12,m_sizeRememberCard.cy);
		}

		//λ�õ���
		nXDrawPos++;
		if(nXDrawPos%5 == 0)
		{
			nXDrawPos = 0;
			nYDrawPos++;
		}
	}
}

//����������
VOID CRememberCardWnd::UpdateCardCount(const BYTE cbCardData[33], BYTE cbCardCount)
{
	BYTE *pCardArray = new BYTE[cbCardCount];
	CopyMemory(pCardArray,cbCardData,sizeof(BYTE)*cbCardCount);
	for(WORD i = 0;i < cbCardCount;i++)
	{
		BYTE cbCardValue = GetCardValue(pCardArray[i]);
		IT it = find(m_ListCard[cbCardValue-1].begin(),m_ListCard[cbCardValue-1].end(),pCardArray[i]);
		ASSERT(it != m_ListCard[cbCardValue-1].end());
		m_ListCard[cbCardValue-1].erase(it);
	}
	delete[] pCardArray;
}

// �滭����
void CRememberCardWnd::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),SCORE_STRING,lOutNum);

	// ������Դ
	INT nNumberHeight=ImageNumber->GetHeight();
	INT nNumberWidth=ImageNumber->GetWidth()/lstrlen(szImageNum);

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
				ImageNumber->DrawImage(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}

//��ʼ��������
VOID CRememberCardWnd::InitializeCardList()
{
	//��A��K������ѹ������
	for(WORD i = 0;i <= 12;i++)
	{
		for(WORD j = 0;j < 4;j++)
		{
			m_ListCard[i].push_front(j*16+i+1);
			m_ListCard[i].push_front(j*16+i+1);
		}
	}
	//�Ѵ�С��ѹ������
	for(WORD i = 13;i <= 14;i++)
	{
		m_ListCard[i].push_front(64+i+1);
		m_ListCard[i].push_front(64+i+1);
	}
}

//����������
VOID CRememberCardWnd::DestroyCardList()
{
	//�������
	for(WORD i = 0;i < CARD_VALUE_COUNT;i++)
	{
		m_ListCard[i].clear();
	}
}
