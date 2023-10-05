#include "StdAfx.h"
#include ".\dialogdexter.h"

BEGIN_MESSAGE_MAP(CDialogDexter, CDialog)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// 构造函数
CDialogDexter::CDialogDexter( UINT nIDTemplate, CWnd* pParentWnd ) : CDialog( nIDTemplate, pParentWnd )
{
	m_nScrollMax = 0;
	m_nScrollPos = 0; 
}

// 析构函数
CDialogDexter::~CDialogDexter()
{
}

// 初始化窗口
BOOL CDialogDexter::OnInitDialog()
{
	// 初始化窗口
	CDialog::OnInitDialog();

	// 设置滚动
	CRect rect;
	GetWindowRect(rect);
	m_nScrollMax = rect.Height();

	return TRUE; 
}

// 滑动条消息
void CDialogDexter::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//获取参数
	CRect rcClient;
	GetClientRect(&rcClient);

	//变量定义
	INT nLastScrollPos=m_nScrollPos;

	//移动坐标
	switch (nSBCode)
	{
	case SB_TOP:
		{
			m_nScrollPos = 0L;
			break;
		}
	case SB_BOTTOM:
		{
			m_nScrollPos = m_nScrollMax - rcClient.Height();
			break;
		}
	case SB_LINEUP:
		{
			m_nScrollPos -= 10;
			break;
		}
	case SB_PAGEUP:
		{
			m_nScrollPos -= rcClient.bottom;
			break;
		}
	case SB_LINEDOWN:
		{
			m_nScrollPos += 10;
			break;
		}
	case SB_PAGEDOWN:
		{
			m_nScrollPos += rcClient.bottom;
			break;
		}
	case SB_THUMBTRACK:
		{
			m_nScrollPos = nPos;
			break;
		}
	}

	//调整位置
	m_nScrollPos=__max(m_nScrollPos,0L);
	m_nScrollPos=__min(m_nScrollPos,m_nScrollMax - rcClient.bottom);

	//滚动窗口
	if (nLastScrollPos!=m_nScrollPos)
	{
		SetScrollPos(SB_VERT,m_nScrollPos);
		ScrollWindow(0L,nLastScrollPos-m_nScrollPos,NULL,NULL);
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

// 鼠标滑轮
BOOL CDialogDexter::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//设置滚动
	INT nLastPos=m_nScrollPos;
	INT nMaxPos=m_nScrollMax-rcClient.bottom;
	m_nScrollPos=__max(__min(m_nScrollPos-(zDelta/WHEEL_DELTA*20),nMaxPos),0L);

	//滚动窗口
	SetScrollPos(SB_VERT,m_nScrollPos);
	ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);

	return TRUE;
}

// 窗口变化
void CDialogDexter::OnSize(UINT nType, int cx, int cy)
{
	// 基类执行
	CDialog::OnSize(nType, cx, cy);

	// 设置滑动条
	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL; 
	ScrollInfo.nMin = 0;
	ScrollInfo.nMax = m_nScrollMax;
	ScrollInfo.nPage = cy;
	ScrollInfo.nPos = 0;
	SetScrollInfo(SB_VERT, &ScrollInfo, TRUE); 	
}


//计算中奖率
WORD CDialogDexter::CalcWinChance(LONGLONG lSystemStorage, LONGLONG lUserStorage, int nParameterK)
{
	WORD wUserWinChance = 0xFFFF;

	LONGLONG lDVal = abs(lSystemStorage - lUserStorage);
	LONGLONG lMaxCtrlStorage = max(lSystemStorage, lUserStorage);
	bool bSysWin = lSystemStorage > lUserStorage;

	if (lSystemStorage <= 0 && lUserStorage <= 0 || nParameterK <= 0)
	{
		return 0;
	}

	if (lUserStorage <= 0)
	{
		wUserWinChance = 0;
	}
	else if (lSystemStorage <= 0)
	{
		wUserWinChance = 100;
	}
	else
	{
		//区间概率判断
		LONGLONG lCurParameterKScore = lMaxCtrlStorage * nParameterK / 100;
		if (lDVal <= lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 50 : 20;
		}
		else if (lDVal > lCurParameterKScore && lDVal <= 1.5 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 40 : 30;
		}
		else if (lDVal > 1.5 * lCurParameterKScore && lDVal <= 2 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 30 : 40;
		}
		else if (lDVal > 2 * lCurParameterKScore && lDVal <= 3 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 20 : 50;
		}
		else if (lDVal > 3 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 0 : 100;
		}
	}

	return wUserWinChance;
}