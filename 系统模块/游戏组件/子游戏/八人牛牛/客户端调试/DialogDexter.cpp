#include "StdAfx.h"
#include "DialogDexter.h"
#include "ClientConfigDlg.h"

BEGIN_MESSAGE_MAP(CDialogDexter, CDialog)
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
END_MESSAGE_MAP()

// ���캯��
CDialogDexter::CDialogDexter(UINT nIDTemplate, int nShowMax, CClientConfigDlg *pParentWnd) : CDialog(nIDTemplate, pParentWnd)
{
    m_pParentWnd = pParentWnd;
    m_bShowScroll = false;
    m_nShowMax = nShowMax;
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
    m_pParentWnd->SendDebugMessage(nMessageID, pData, nSize);
    return true;
}

// ������Ϣ
bool CDialogDexter::SendDebugMessage(UINT nMessageID, WORD wTableID, void *pData, UINT nSize)
{
	m_pParentWnd->SendDebugMessage(nMessageID, wTableID, pData, nSize);
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

//��ȡ��������λ������
CString CDialogDexter::GetTableCtrlPosDesc(TABLECTRL_TYPE emtablectrl_type)
{
	switch (emtablectrl_type)
	{
	case CURPLAYER_WIN:
	{
		return TEXT("���Ӯ");
	}
	case CURPLAYER_LOST:
	{
		return TEXT("�����");
	}
	case BANKER_WIN:
	{
		return TEXT("ׯ��Ӯ");
	}
	case BANKER_LOST:
	{
		return TEXT("ׯ����");
	}
	case TABLECTRL_INVALID:
	{
		return TEXT("����������Ч");
	}
	default:
		break;
	}

	return TEXT("");
}
