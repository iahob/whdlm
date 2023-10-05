#include "StdAfx.h"
#include ".\indianaroll.h"

CIndianaRoll::CIndianaRoll(void)
{
}

CIndianaRoll::~CIndianaRoll(void)
{
}
//初始化
void CIndianaRoll::InitIndiana(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{	
	//变量定义
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	//载入资源
	m_ImageGem.LoadImage(pD3DDevice, hResInstance, 6, 5, TEXT("INDIANA_BALL"), TEXT("PNG"));
	m_ImageBack.LoadImage(pD3DDevice, hResInstance, TEXT("GAME_BACK_02"), TEXT("PNG"));

	m_sizeGem.SetSize(m_ImageGem.GetWidth() / m_ImageGem.GetWidthIndex(), m_ImageGem.GetHeight() / m_ImageGem.GetHeightIndex());

}


double							dVelocity = 0.30;				//速度
double							dSpan = 10.48;					//跨度
double							dHeight = 0.48;					//高度
int nCount = 0;

//动画驱动
void CIndianaRoll::CartoonMovie()
{
	////宝石散开
	if ( m_IndianaMove.Move() )
	{
		m_IndianaMove.BeginMove( m_IndianaMove.ptPosition, DPOINT(0,280 + nCount * 100), 30, 0.30, 10.48, 0.48, false);
		nCount++;
		if ( nCount > 10 )
		{
			nCount = 0;
		}
	}

	//if ( m_IndianaMoveTow.Move() )
	//{
	//}
	
}

int xxx = 660;
int yyy = 180;
//绘画函数
void CIndianaRoll::DrwaIndiana( CD3DDevice * pD3DDevice, INT nWidth, INT nHeight )
{
	m_ImageBack.DrawImage(pD3DDevice, 0, 0);

	//m_IndianaMove.BeginMove(DPOINT(536.82, 381), DPOINT(0,481), 30, dVelocity, dSpan, dHeight,false);
	//for ( int i = 0;  i < 100; ++i )
	//{
	//	m_IndianaMove.Move();
		m_ImageGem.DrawImageIndex( pD3DDevice, CPoint(m_sizeGem.cx / 2, m_sizeGem.cy / 2), m_IndianaMove.fAngle, 'z', m_IndianaMove.ptPosition.x - m_sizeGem.cx / 2, m_IndianaMove.ptPosition.y - m_sizeGem.cy / 2, GetTickCount()/30%29 );
	//}
	
	//m_IndianaMove.BeginMove(DPOINT(536.82, 381), DPOINT(0,481), 30, dVelocity, dSpan, dHeight,true);
	//for ( int i = 0;  i < 100; ++i )
	//{
	//	m_IndianaMove.Move();
		//m_ImageGem.DrawImage( pD3DDevice, CPoint(m_sizeGem.cx / 2, m_sizeGem.cy / 2), m_IndianaMove.fAngle, 'z', m_IndianaMove.ptPosition.x - m_sizeGem.cx / 2, m_IndianaMove.ptPosition.y - m_sizeGem.cy / 2 );
	//}

	//m_ImageGem.DrawImage( pD3DDevice, xxx - m_sizeGem.cx / 2, yyy - m_sizeGem.cy / 2 );
	//TRACE(TEXT("%0.2f, %0.2f, %0.2f \n"),dVelocity, dSpan, dHeight );
}

//开始移动
void CIndianaRoll::BeginMove()
{
	m_IndianaMove.BeginMove(DPOINT(668, 180), DPOINT(0,280 + nCount * 100), 30, dVelocity, dSpan, dHeight,true);
	nCount++;
}

//接受消息
BOOL CIndianaRoll::PreTranslateMessage(MSG* pMsg)
{
	// 键盘消息
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			switch ( pMsg->wParam )
			{
			case VK_UP:   
				dSpan += 0.01;
				break;
			case VK_DOWN: 
				dSpan -= 0.01;
				break;
			case VK_LEFT: 
				//dHeight -= 0.01;
				break;
			case VK_RIGHT:  
				//dHeight += 0.01;
				break;
			}
		}
	}
	return TRUE;
}

