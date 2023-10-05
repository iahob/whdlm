#include "StdAfx.h"
#include "DialogDexter.h"
#include "ClientDebugItemSink.h"

BEGIN_MESSAGE_MAP(CDialogDexter, CDialog)
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
END_MESSAGE_MAP()

// ���캯��
CDialogDexter::CDialogDexter(UINT nIDTemplate, CClientDebugItemSinkDlg *pParentWnd) : CDialog(nIDTemplate, pParentWnd)
{
    m_pClientDebugItemSinkDlg = pParentWnd;
    m_bShowScroll = false;
    m_nScrollMax = 0;
    m_nScrollPos = 0;
}

// ��������
CDialogDexter::~CDialogDexter()
{
}

// ������Ϣ
bool CDialogDexter::SendDebugMessage(UINT nMessageID, void *pData, UINT nSize)
{
    m_pClientDebugItemSinkDlg->SendDebugMessage(nMessageID, pData, nSize);
    return true;
}

// ������Ϣ
bool CDialogDexter::SendDebugMessage(UINT nMessageID, WORD wTableID, void *pData, UINT nSize)
{
	m_pClientDebugItemSinkDlg->SendDebugMessage(nMessageID, wTableID, pData, nSize);
	return true;
}

// ��ʼ������
BOOL CDialogDexter::OnInitDialog()
{
    // ��ʼ������
    __super::OnInitDialog();

    // ���ù���
    CRect rect;
    GetWindowRect(rect);
    m_nScrollMax = rect.Height();
    m_bShowScroll = m_nShowMax < m_nScrollMax;

    return TRUE;
}

// ��������Ϣ
void CDialogDexter::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    //��ȡ����
    CRect rcClient;
    GetClientRect(&rcClient);

    //��������
    INT nLastScrollPos = m_nScrollPos;

    //�ƶ�����
    switch(nSBCode)
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
    m_nScrollPos = __max(m_nScrollPos, 0L);
    m_nScrollPos = __min(m_nScrollPos, m_nScrollMax - rcClient.bottom);

    //��������
    if(nLastScrollPos != m_nScrollPos)
    {
        SetScrollPos(SB_VERT, m_nScrollPos);
        ScrollWindow(0L, nLastScrollPos - m_nScrollPos, NULL, NULL);
    }

    __super::OnVScroll(nSBCode, nPos, pScrollBar);
}

// ��껬��
BOOL CDialogDexter::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    //��ȡλ��
    CRect rcClient;
    GetClientRect(&rcClient);

    //���ù���
    INT nLastPos = m_nScrollPos;
    INT nMaxPos = m_nScrollMax - rcClient.bottom;
    m_nScrollPos = __max(__min(m_nScrollPos - (zDelta / WHEEL_DELTA * 20), nMaxPos), 0L);

    //��������
    SetScrollPos(SB_VERT, m_nScrollPos);
    ScrollWindow(0, nLastPos - m_nScrollPos, NULL, NULL);

    return TRUE;
}

// ���ڱ仯
void CDialogDexter::OnSize(UINT nType, int cx, int cy)
{
    // ����ִ��
    __super::OnSize(nType, cx, cy);

    if(m_bShowScroll)
    {
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
}

void CDialogDexter::OnTableQuery()
{
}

void CDialogDexter::SetTableCount(WORD wTableCount)
{

}

//�����н���
WORD CDialogDexter::CalcWinChance(double dSystemStorage, double dUserStorage, int nParameterK)
{
	WORD wUserWinChance = INVALID_WORD;

	SCORE lDVal = abs(dSystemStorage - dUserStorage);
	SCORE lMaxCtrlStorage = max(dSystemStorage, dUserStorage);
	bool bSysWin = dSystemStorage > dUserStorage;

	if (dSystemStorage <= 0 && dUserStorage <= 0 || nParameterK <= 0)
	{
		return 0;
	}

	if (dUserStorage <= 0)
	{
		wUserWinChance = 0;
	}
	else if (dSystemStorage <= 0)
	{
		wUserWinChance = 100;
	}
	else
	{
		//��������ж�
		LONGLONG lCurParameterKScore = lMaxCtrlStorage * nParameterK / 100;
		if (lDVal <= lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 50 : 50;
		}
		else if (lDVal > lCurParameterKScore && lDVal <= 1.5 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 40 : 60;
		}
		else if (lDVal > 1.5 * lCurParameterKScore && lDVal <= 2 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 30 : 70;
		}
		else if (lDVal > 2 * lCurParameterKScore && lDVal <= 3 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 20 : 80;
		}
		else if (lDVal > 3 * lCurParameterKScore && lDVal <= 4 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 10 : 90;
		}
		else
		{
			wUserWinChance = bSysWin ? 0 : 100;
		}
	}

	return wUserWinChance;
}

//����״̬
CString CDialogDexter::GetDebugStatus(int nDebugStatus)
{
	switch (nDebugStatus)
	{
	case RUNNNING_STATUS:
	{
		return TEXT("������");
	}
	case WIAT_STATUS:
	{
		return TEXT("������");
	}
	case CANCEL_STATUS:
	{
		return TEXT("��ȡ��");
	}
	case FINISH_STATUS:
	{
		return TEXT("�����");
	}
	default:
		break;
	}

	return TEXT("����״̬");
}