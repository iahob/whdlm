#include "StdAfx.h"
#include ".\dialogdexter.h"

BEGIN_MESSAGE_MAP(CDialogDexter, CDialog)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// ���캯��
CDialogDexter::CDialogDexter( UINT nIDTemplate, CWnd* pParentWnd ) : CDialog( nIDTemplate, pParentWnd )
{
	m_nScrollMax = 0;
	m_nScrollPos = 0; 
}

// ��������
CDialogDexter::~CDialogDexter()
{
}

// ��ʼ������
BOOL CDialogDexter::OnInitDialog()
{
	// ��ʼ������
	CDialog::OnInitDialog();

	// ���ù���
	CRect rect;
	GetWindowRect(rect);
	m_nScrollMax = rect.Height();

	return TRUE; 
}

// ��������Ϣ
void CDialogDexter::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//��ȡ����
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	INT nLastScrollPos=m_nScrollPos;

	//�ƶ�����
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

	//����λ��
	m_nScrollPos=__max(m_nScrollPos,0L);
	m_nScrollPos=__min(m_nScrollPos,m_nScrollMax - rcClient.bottom);

	//��������
	if (nLastScrollPos!=m_nScrollPos)
	{
		SetScrollPos(SB_VERT,m_nScrollPos);
		ScrollWindow(0L,nLastScrollPos-m_nScrollPos,NULL,NULL);
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

// ��껬��
BOOL CDialogDexter::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//���ù���
	INT nLastPos=m_nScrollPos;
	INT nMaxPos=m_nScrollMax-rcClient.bottom;
	m_nScrollPos=__max(__min(m_nScrollPos-(zDelta/WHEEL_DELTA*20),nMaxPos),0L);

	//��������
	SetScrollPos(SB_VERT,m_nScrollPos);
	ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);

	return TRUE;
}

// ���ڱ仯
void CDialogDexter::OnSize(UINT nType, int cx, int cy)
{
	// ����ִ��
	CDialog::OnSize(nType, cx, cy);

	// ���û�����
	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL; 
	ScrollInfo.nMin = 0;
	ScrollInfo.nMax = m_nScrollMax;
	ScrollInfo.nPage = cy;
	ScrollInfo.nPos = 0;
	SetScrollInfo(SB_VERT, &ScrollInfo, TRUE); 	
}


//�����н���
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
		//��������ж�
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