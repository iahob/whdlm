// DialogDebug.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "DialogDebug.h"
#include "..\��Ϣ����\CMD_Game.h"

tagCustomRule					g_CustomRule;						// ��Ϸ����

// ʵ����Ϣ
TCHAR g_szKindRewardName[KindReward_Max][64] = { TEXT("��Чʵ��"), TEXT("���1��"), TEXT("���2��"), TEXT("���3��"), TEXT("ʵ��1��"), TEXT("ʵ��2��"),
                                                 TEXT("ʵ��3��"), TEXT("ʵ��4��"), TEXT("ʵ��5��"), TEXT("ʵ��6��"), TEXT("ʵ��7��"),
                                                 TEXT("ʵ��8��"), TEXT("ʵ��9��"), TEXT("ʵ��10��"), TEXT("ʵ��11��"), TEXT("ʵ��12��"),
                                                 TEXT("ʵ��13��"), TEXT("ʵ��14��"), TEXT("ʵ��15��")
                                               };


BEGIN_MESSAGE_MAP(CDialogDexter, CDialog)
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
END_MESSAGE_MAP()

// ���캯��
CDialogDexter::CDialogDexter(UINT nIDTemplate, int nShowMax, CDialogDebug *pParentWnd) : CDialog(nIDTemplate, pParentWnd)
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
bool CDialogDexter::SendDebugMessage(uint nMessageID, void *pData, uint nSize)
{
    m_pParentWnd->SendDebugMessage(nMessageID, pData, nSize);
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

// ���ԶԻ���
IMPLEMENT_DYNAMIC(CDialogDebug, CDialog)
BEGIN_MESSAGE_MAP(CDialogDebug, CDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTabOptisons)
    ON_BN_CLICKED(IDC_BUTTON_DEFAULT, &CDialogDebug::OnBnClickedButtonDefault)
    ON_BN_CLICKED(IDC_BUTTON_REFRESH_RULE, &CDialogDebug::OnBnClickedButtonRefreshRule)
    ON_BN_CLICKED(IDC_BUTTON_MODIFY_RULE, &CDialogDebug::OnBnClickedButtonModifyRule)
END_MESSAGE_MAP()

// ���캯��
CDialogDebug::CDialogDebug(CWnd *pParent /*=NULL*/) : CDialog(IDD_DIALOG_MAIN, pParent)
{
    // ���ñ���
    m_pParentWnd = NULL;
    m_pIClientDebugCallback = NULL;
}

// ��������
CDialogDebug::~CDialogDebug()
{
}

// �ؼ���
void CDialogDebug::DoDataExchange(CDataExchange *pDX)
{
    __super::DoDataExchange(pDX);
}

// �ͷŽӿ�
void CDialogDebug::Release()
{
    delete this;
}

// ��������
bool CDialogDebug::Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback)
{
    // ���ñ���
    m_pParentWnd = pParentWnd;
    m_pIClientDebugCallback = pIClientDebugCallback;

    // ��������
    __super::Create(IDD_DIALOG_MAIN, pParentWnd);

    // ��ȡ��ҳ
    CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);

    // ����ѡ��
    pTabCtrl->InsertItem(0, _T("��������"));
    pTabCtrl->InsertItem(1, _T("�������"));
    pTabCtrl->InsertItem(2, _T("��������"));
    pTabCtrl->InsertItem(3, _T("��������"));



    pTabCtrl->SetCurSel(0);

    // ��ȡ��ҳ
    CRect RectWindow;
    CWnd *pWindowShow = GetDlgItem(IDC_STATIC_SHOW);
    pWindowShow->ShowWindow(SW_HIDE);
    pWindowShow->GetWindowRect(RectWindow);
    ScreenToClient(RectWindow);

    // ��������

    // ��������
    m_DialogCustom[CUSTOM_SPECIAL] = new CDialogSpecial(RectWindow.Height(), this);
    m_DialogCustom[CUSTOM_SPECIAL]->Create(IDD_CUSTOM_SPECIAL, this);

    // �������
    m_DialogCustom[CUSTOM_GENERAL] = new CDialogGeneral(RectWindow.Height(), this);
    m_DialogCustom[CUSTOM_GENERAL]->Create(IDD_CUSTOM_GENERAL, this);

    // ��������
    m_DialogCustom[CUSTOM_GIFT] = new CDialogGift(RectWindow.Height(), this);
    m_DialogCustom[CUSTOM_GIFT]->Create(IDD_CUSTOM_GIFT, this);

    // ��������
    m_DialogCustom[CUSTOM_DELIVERY] = new CDialogDelivery(RectWindow.Height(), this);
    m_DialogCustom[CUSTOM_DELIVERY]->Create(IDD_CUSTOM_DELIVERY, this);

    // ����λ��
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        CRect RectCustom;
        CSize SizeCustom;
        m_DialogCustom[nIndex]->GetClientRect(RectCustom);
        SizeCustom.SetSize(min(RectWindow.Width(), RectCustom.Width()), min(RectWindow.Height(), RectCustom.Height()));
        m_DialogCustom[nIndex]->SetWindowPos(NULL, RectWindow.left + RectWindow.Width() / 2 - SizeCustom.cx / 2, RectWindow.top + RectWindow.Height() / 2 - SizeCustom.cy / 2, SizeCustom.cx, SizeCustom.cy, SWP_NOZORDER);
        m_DialogCustom[nIndex]->ShowWindow((nIndex == 0) ? SW_SHOW : SW_HIDE);
    }

    // �رմ���
    __super::ShowWindow(SW_HIDE);

    bool bAutoRefresh = true;
    SendDebugMessage(SUB_C_DEBUG_REFRESH_RULE, &bAutoRefresh, sizeof(bAutoRefresh));

    return true;

}

// ��ʾ����
bool CDialogDebug::ShowWindow(bool bShow)
{
    // ��ʾ����
    __super::ShowWindow(bShow ? SW_SHOW : SW_HIDE);

    // ��ʾ����
    if(bShow && m_pParentWnd)
    {
        // ��ȡ������
        CRect RectParent;
        m_pParentWnd->GetWindowRect(RectParent);

        // ��ȡ��ǰ����
        CRect RectWindow;
        GetWindowRect(RectWindow);

        // �ƶ�λ��
        SetWindowPos(NULL, RectParent.left + RectParent.Width() / 2 - RectWindow.Width() / 2, RectParent.top + RectParent.Height() / 2 - RectWindow.Height() / 2, 0, 0, SWP_NOSIZE);
    }

    return true;
}

// ��Ϣ����
bool CDialogDebug::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
    // ��Ч�ӿ�
    if(m_pIClientDebugCallback == NULL)
    {
        return false;
    }

    switch(nMessageID)
    {
    case  SUB_S_DEBUG_REFRESH_RULE:
    {
        //У���С
        ASSERT(sizeof(CMD_S_CustomRule) == nSize);
        if(sizeof(CMD_S_CustomRule) != nSize)
        {
            return false;
        }

        CMD_S_CustomRule *pCustomRule = (CMD_S_CustomRule *)pData;
        CopyMemory(&g_CustomRule, &pCustomRule->CustomRule, sizeof(g_CustomRule));
        // �жϴ���
        for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
        {
            m_DialogCustom[nIndex]->UpdateData(false);
        }

        if(pCustomRule->cbUpdateType == DEBUG_REFRESH_RULE)
        {
            MessageBox(TEXT("��Ϸ����ˢ�³ɹ���"), TEXT("��ܰ��ʾ"));
        }
        else if(pCustomRule->cbUpdateType == DEBUG_MODIFY_RULE)
        {
            MessageBox(TEXT("��Ϸ�����޸ĳɹ���"), TEXT("��ܰ��ʾ"));
        }


        return true;
    }
    default:
        break;
    }

    // �жϴ���
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        if(m_DialogCustom[nIndex]->OnDebugMessage(nMessageID, wTableID, pData, nSize))
        {
            return true;
        }
    }

    return true;
}

// ������Ϣ
bool CDialogDebug::SendDebugMessage(uint nMessageID, void *pData, uint nSize)
{
    if(m_pIClientDebugCallback != NULL)
    {
        // ������Ϣ
        m_pIClientDebugCallback->OnDebugInfo(nMessageID, 0, pData, nSize);
    }
    return true;
}

// �任ѡ��
void CDialogDebug::OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult)
{
    // ��ȡ��ҳ
    CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);

    // ��ȡѡ����Ŀ
    int nCurSel = pTabCtrl->GetCurSel();
    if(nCurSel >= 0 && nCurSel < MAX_CUSTOM)
    {
        // �жϴ���
        for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
        {
            m_DialogCustom[nIndex]->ShowWindow((nIndex == nCurSel) ? SW_SHOW : SW_HIDE);
        }
    }

    *pResult = 0;
}

void CDialogDebug::OnBnClickedButtonDefault()
{
    g_CustomRule.DefaultCustomRule();
    // �жϴ���
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        m_DialogCustom[nIndex]->UpdateData(false);
    }
}


void CDialogDebug::OnBnClickedButtonRefreshRule()
{
    // ˢ������
    bool bAutoRefresh = false;
    SendDebugMessage(SUB_C_DEBUG_REFRESH_RULE, &bAutoRefresh, sizeof(bAutoRefresh));

    //��ȡ����
    SendDebugMessage(SUB_S_REFRESH_GIFT);
}


void CDialogDebug::OnBnClickedButtonModifyRule()
{
    // �жϴ���
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        m_DialogCustom[nIndex]->UpdateData(true);
    }

    // �޸�����
    SendDebugMessage(SUB_C_DEBUG_MODIFY_RULE, &g_CustomRule, sizeof(g_CustomRule));

    // ��ȡ����
    SendDebugMessage(SUB_S_REFRESH_GIFT);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

// ���ԶԻ���
IMPLEMENT_DYNAMIC(CDialogSpecial, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogSpecial, CDialogDexter)
    ON_BN_CLICKED(IDC_BTN_REFRESH_STOCK, &CDialogSpecial::OnBnClickedBtnRefreshStock)
    ON_BN_CLICKED(IDC_BTN_MODIFY_STOCK, &CDialogSpecial::OnBnClickedBtnModifyStock)
    ON_BN_CLICKED(IDC_BTN_GLOD, &CDialogSpecial::OnBnClickedBtnGlod)
    ON_BN_CLICKED(IDC_BTN_GIFT, &CDialogSpecial::OnBnClickedBtnGift)
    ON_CBN_SELCHANGE(IDC_COMBO_GIFT_TYPE, &CDialogSpecial::OnCbnSelchangeComboGiftType)
    ON_BN_CLICKED(IDC_BTN_QUERY_ID, &CDialogSpecial::OnBnClickedBtnQueryId)
    ON_BN_CLICKED(IDC_BTN_SHARD, &CDialogSpecial::OnBnClickedBtnShard)
    ON_BN_CLICKED(IDC_BTN_GIFT_OPEN_CLOSE, &CDialogSpecial::OnBnClickedBtnGiftOpenClose)
END_MESSAGE_MAP()

// ���캯��
CDialogSpecial::CDialogSpecial(int nShowMax/* = 0*/, CDialogDebug *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_SPECIAL, nShowMax, pParent)
{
    // ���ñ���
}

// ��������
CDialogSpecial::~CDialogSpecial()
{
}

// �ؼ���
void CDialogSpecial::DoDataExchange(CDataExchange *pDX)
{
    CDialogDexter::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_GIFT_TYPE, m_ComboGiftType);
}

// ��ʼ������
BOOL CDialogSpecial::OnInitDialog()
{
    // ��ʼ������
    __super::OnInitDialog();

    //ˢ�¿��
    OnBnClickedBtnRefreshStock();

    //��ȡ����
    SendDebugMessage(SUB_S_REFRESH_GIFT);

    return TRUE;
}

// ��Ϣ����
bool CDialogSpecial::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
    // �ж���Ϣ
    switch(nMessageID)
    {
    case SUB_S_REFRESH_STOCK:		//ˢ�¿��
    {
        ASSERT(sizeof(CMD_S_StockInfo) == nSize);
        if(sizeof(CMD_S_StockInfo) != nSize)
        {
            return false;
        }

        // ������Ϣ
        CMD_S_StockInfo *pStockInfo = (CMD_S_StockInfo *)pData;

        CString strValue;
        strValue.Format(TEXT("%I64d"), pStockInfo->lRoomStorage);
        SetDlgItemText(IDC_EDIT_STOCK, strValue);
        SetDlgItemInt(IDC_EDIT_DROP_PROBABILITY_0, pStockInfo->cbDropProbability[0]);
        SetDlgItemInt(IDC_EDIT_DROP_PROBABILITY_1, pStockInfo->cbDropProbability[1]);

        CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_GIFT);

        if(pButton != NULL)
        {
            pButton->SetCheck(pStockInfo->bGiftFeatures);
        }
        SetDlgItemText(IDC_BTN_GIFT_OPEN_CLOSE, pStockInfo->bGiftFeatures ? TEXT("�ر����﹦��") : TEXT("�����﹦��"));

        return true;
    }
    case SUB_S_MODIFY_STOCK:		//�޸Ŀ��
    {
        ASSERT(sizeof(CMD_S_StockInfo) == nSize);
        if(sizeof(CMD_S_StockInfo) != nSize)
        {
            return false;
        }

        // ������Ϣ
        CMD_S_StockInfo *pStockInfo = (CMD_S_StockInfo *)pData;

        CString strValue;
        strValue.Format(TEXT("%I64d"), pStockInfo->lRoomStorage);
        SetDlgItemText(IDC_EDIT_STOCK, strValue);
        SetDlgItemInt(IDC_EDIT_DROP_PROBABILITY_0, pStockInfo->cbDropProbability[0]);
        SetDlgItemInt(IDC_EDIT_DROP_PROBABILITY_1, pStockInfo->cbDropProbability[1]);

        MessageBox(TEXT("�޸Ŀ��ɹ���"), TEXT("��ܰ��ʾ"));

        return true;
    }
    case SUB_S_REFRESH_GIFT:		//ˢ������
    {
        ASSERT(sizeof(CMD_S_GiftType) == nSize);
        if(sizeof(CMD_S_GiftType) != nSize)
        {
            return false;
        }

        // ������Ϣ
        CMD_S_GiftType *pGiftType = (CMD_S_GiftType *)pData;

        //����б�
        m_ComboGiftType.ResetContent();

        //��������
        m_ComboGiftType.AddString(g_szKindRewardName[KindReward_Null]);
        for(int i = 0; i < pGiftType->cbGiftCount; i++)
        {
            if(pGiftType->cbGiftType[i] != KindReward_Null && pGiftType->cbGiftType[i] < KindReward_Max)
            {
                m_ComboGiftType.AddString(g_szKindRewardName[pGiftType->cbGiftType[i]]);
            }
        }

        return true;
    }
    case SUB_S_QUERY_DEBUG_FAIL:			//�޸�ʧ��
    {
        ASSERT(sizeof(CMD_S_QueryDebugFail) == nSize);
        if(sizeof(CMD_S_QueryDebugFail) != nSize)
        {
            return false;
        }

        // ������Ϣ
        CMD_S_QueryDebugFail *pQueryDebugFail = (CMD_S_QueryDebugFail *)pData;

        // ʵ����Ϣ
        TCHAR szOperateName[4][64] = { TEXT("�޸�����"), TEXT("�޸Ľ��"), TEXT("��ѯ�޸�"), TEXT("�޸���Ƭ") };
        CString strValue;
        strValue.Format(TEXT("���GAMEID��%d��%sʧ�ܣ�"), pQueryDebugFail->dwGameID, szOperateName[pQueryDebugFail->cbQueryType]);
        MessageBox(strValue, TEXT("��ܰ��ʾ"));

        return true;
    }
    case SUB_S_QUERY_DEBUG_SUCCESS:		//�޸ĳɹ�
    {
        ASSERT(sizeof(CMD_S_QueryDebugSuccess) == nSize);
        if(sizeof(CMD_S_QueryDebugSuccess) != nSize)
        {
            return false;
        }

        // ������Ϣ
        CMD_S_QueryDebugSuccess *pQueryDebugSuccess = (CMD_S_QueryDebugSuccess *)pData;

        CString strValue;
        strValue.Format(TEXT("���GAMEID��%d\n���н���ң�%I64d\n������Ƭ��%I64d\n���н�����1��%s\n���н�����2��%s\n���н�����3��%s"), pQueryDebugSuccess->dwGameID, pQueryDebugSuccess->lGlodTotalScore, pQueryDebugSuccess->lGiftTotalScore, g_szKindRewardName[pQueryDebugSuccess->cbGiftType[0]], g_szKindRewardName[pQueryDebugSuccess->cbGiftType[1]], g_szKindRewardName[pQueryDebugSuccess->cbGiftType[2]]);
        TCHAR szOperateName[4][64] = { TEXT("�޸�����ɹ�"), TEXT("�޸Ľ�ҳɹ�"), TEXT("��ѯ�޸ĳɹ�"), TEXT("�޸���Ƭ�ɹ�") };
        MessageBox(strValue, szOperateName[pQueryDebugSuccess->cbQueryType]);

        return true;
    }

    break;
    }

    return true;
}

//��������
int CDialogSpecial::GetKindRewardIndex(CString strKindRewardName)
{
    for(int i = 1; i < KindReward_Max; i++)
    {
        if(g_szKindRewardName[i] == strKindRewardName)
        {
            return i;
        }
    }

    return 0;
}

//ˢ�¿��
void CDialogSpecial::OnBnClickedBtnRefreshStock()
{
    // ������Ϣ
    SendDebugMessage(SUB_C_REFRESH_STOCK);
}

//�޸Ŀ��
void CDialogSpecial::OnBnClickedBtnModifyStock()
{
    CString strValue;
    CMD_C_ModifyStock ModifyStock;
    GetDlgItemText(IDC_EDIT_STOCK, strValue);
    ModifyStock.lRoomStorage = _ttoi64(strValue);
    ModifyStock.cbDropProbability[0] = GetDlgItemInt(IDC_EDIT_DROP_PROBABILITY_0);
    ModifyStock.cbDropProbability[1] = GetDlgItemInt(IDC_EDIT_DROP_PROBABILITY_1);

    if(ModifyStock.cbDropProbability[0] > 100 || ModifyStock.cbDropProbability[1] > 100 || ModifyStock.cbDropProbability[0] > ModifyStock.cbDropProbability[1])
    {
        MessageBox(TEXT("�����������Ϊ0��100�ҵ�һ����Ҫ�ȵڶ�����С����������д��"), TEXT("��ܰ��ʾ"));
        return;
    }

    // ������Ϣ
    SendDebugMessage(SUB_C_MODIFY_STOCK, &ModifyStock, sizeof(ModifyStock));
}


void CDialogSpecial::OnBnClickedBtnGlod()
{
    CString strValue;
    CMD_C_DebugGlod DebugGlod;
    GetDlgItemText(IDC_EDIT_GAME_ID, strValue);
    DebugGlod.dwGameID = _ttoi(strValue);
    if(DebugGlod.dwGameID <= 0)
    {
        MessageBox(TEXT("���ID������������д��"), TEXT("��ܰ��ʾ"));
        return;
    }

    GetDlgItemText(IDC_EDIT_GLOD, strValue);
    DebugGlod.lGoldScore = _ttoi64(strValue);
    if(DebugGlod.lGoldScore < 0)
    {
        MessageBox(TEXT("�޸Ľ��������С��0����������д��"), TEXT("��ܰ��ʾ"));
        return;
    }

    // ������Ϣ
    SendDebugMessage(SUB_C_DEBUG_GLOD, &DebugGlod, sizeof(DebugGlod));
}


void CDialogSpecial::OnBnClickedBtnGift()
{
    CString strValue;
    CMD_C_DebugGift DebugGift;
    GetDlgItemText(IDC_EDIT_GAME_ID, strValue);
    DebugGift.dwGameID = _ttoi(strValue);
    if(DebugGift.dwGameID <= 0)
    {
        MessageBox(TEXT("���ID������������д��"), TEXT("��ܰ��ʾ"));
        return;
    }
    int nSelectIndex = m_ComboGiftType.GetCurSel();
    if(nSelectIndex < 0)
    {
        MessageBox(TEXT("��ѡ����ȷ�������������д��"), TEXT("��ܰ��ʾ"));
        return;
    }
    CString strText;
    m_ComboGiftType.GetLBText(nSelectIndex, strText);
    DebugGift.cbGiftType = GetKindRewardIndex(strText);
    if(DebugGift.cbGiftType <= 0 && DebugGift.cbGiftType >= KindReward_Max)
    {
        MessageBox(TEXT("��ѡ����ȷ�������������д��"), TEXT("��ܰ��ʾ"));
        return;
    }

    // ������Ϣ
    SendDebugMessage(SUB_C_DEBUG_GIFT, &DebugGift, sizeof(DebugGift));
}


void CDialogSpecial::OnCbnSelchangeComboGiftType()
{
}


void CDialogSpecial::OnBnClickedBtnQueryId()
{
    CString strValue;
    CMD_C_QueryDebug QueryDebug;
    GetDlgItemText(IDC_EDIT_GAME_ID, strValue);
    QueryDebug.dwGameID = _ttoi(strValue);
    if(QueryDebug.dwGameID <= 0)
    {
        MessageBox(TEXT("���ID������������д��"), TEXT("��ܰ��ʾ"));
        return;
    }


    // ������Ϣ
    SendDebugMessage(SUB_C_QUERY_DEBUG, &QueryDebug, sizeof(QueryDebug));
}


void CDialogSpecial::OnBnClickedBtnShard()
{
    CString strValue;
    CMD_C_DebugGlod DebugGlod;
    GetDlgItemText(IDC_EDIT_GAME_ID, strValue);
    DebugGlod.dwGameID = _ttoi(strValue);
    if(DebugGlod.dwGameID <= 0)
    {
        MessageBox(TEXT("���ID������������д��"), TEXT("��ܰ��ʾ"));
        return;
    }

    GetDlgItemText(IDC_EDIT_SHARD, strValue);
    DebugGlod.lGoldScore = _ttoi64(strValue);
    if(DebugGlod.lGoldScore < 0)
    {
        MessageBox(TEXT("�޸���Ƭ������С��0����������д��"), TEXT("��ܰ��ʾ"));
        return;
    }

    // ������Ϣ
    SendDebugMessage(SUB_C_DEBUG_SHARD, &DebugGlod, sizeof(DebugGlod));
}


void CDialogSpecial::OnBnClickedBtnGiftOpenClose()
{
    CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_GIFT);

    if(pButton != NULL)
    {
        int nGiftFeatures = pButton->GetCheck() == 1 ? 0 : 1;

        // ������Ϣ
        SendDebugMessage(SUB_C_DEBUG_GIFT_FEATURES, &nGiftFeatures, sizeof(nGiftFeatures));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ���ԶԻ���
IMPLEMENT_DYNAMIC(CDialogGeneral, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogGeneral, CDialogDexter)
END_MESSAGE_MAP()

// ���캯��
CDialogGeneral::CDialogGeneral(int nShowMax/* = 0*/, CDialogDebug *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_SPECIAL, nShowMax, pParent)
{
}

// ��������
CDialogGeneral::~CDialogGeneral()
{
}

// �ؼ���
void CDialogGeneral::DoDataExchange(CDataExchange *pDX)
{
    CDialogDexter::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, g_CustomRule.lGoldScore[0]);
    DDX_Text(pDX, IDC_EDIT2, g_CustomRule.lGoldScore[1]);
    DDX_Text(pDX, IDC_EDIT3, g_CustomRule.lGoldScore[2]);
    DDX_Text(pDX, IDC_EDIT4, g_CustomRule.lGoldScore[3]);
    DDX_Text(pDX, IDC_EDIT5, g_CustomRule.lGoldScore[4]);
    DDX_Text(pDX, IDC_EDIT6, g_CustomRule.lGoldScore[5]);
    DDX_Text(pDX, IDC_EDIT7, g_CustomRule.lGoldScore[6]);
    DDX_Text(pDX, IDC_EDIT8, g_CustomRule.lGoldScore[7]);
    DDX_Text(pDX, IDC_EDIT9, g_CustomRule.lGoldScore[8]);
    DDX_Text(pDX, IDC_EDIT10, g_CustomRule.lGoldScore[9]);
    DDX_Text(pDX, IDC_EDIT11, g_CustomRule.lStorageStart);
    DDX_Text(pDX, IDC_EDIT28, g_CustomRule.cbNormalDropProbability[0]);
    DDX_Text(pDX, IDC_EDIT180, g_CustomRule.cbNormalDropProbability[1]);

    DDX_Text(pDX, IDC_EDIT12, g_CustomRule.cbNewDropProbability[0]);
    DDX_Text(pDX, IDC_EDIT13, g_CustomRule.cbNewDropProbability[1]);
    DDX_Text(pDX, IDC_EDIT172, g_CustomRule.nNewUserLineCount);
    DDX_Text(pDX, IDC_EDIT176, g_CustomRule.cbBigDropProbability[0]);
    DDX_Text(pDX, IDC_EDIT20, g_CustomRule.cbBigDropProbability[1]);
    DDX_Text(pDX, IDC_EDIT173, g_CustomRule.nBigUserLineCount);
    DDX_Text(pDX, IDC_EDIT174, g_CustomRule.lBigUserRechargeCount);
    DDX_Text(pDX, IDC_EDIT175, g_CustomRule.lBigUserGoldScore);
    DDX_Text(pDX, IDC_EDIT21, g_CustomRule.nPlatformGoldTimeCount);
    DDX_Text(pDX, IDC_EDIT22, g_CustomRule.nCxceedProportion);
    DDX_Text(pDX, IDC_EDIT177, g_CustomRule.nRecoverProportion);
    DDX_Text(pDX, IDC_EDIT23, g_CustomRule.nChouFangShuiTimeCount);
    DDX_Text(pDX, IDC_EDIT24, g_CustomRule.cbChouDropProbability[0]);
    DDX_Text(pDX, IDC_EDIT25, g_CustomRule.cbChouDropProbability[1]);
    DDX_Text(pDX, IDC_EDIT178, g_CustomRule.cbFangDropProbability[0]);
    DDX_Text(pDX, IDC_EDIT26, g_CustomRule.cbFangDropProbability[1]);
    DDX_Text(pDX, IDC_EDIT179, g_CustomRule.lChouFangShuiWarningScore);
    DDX_Text(pDX, IDC_EDIT18, g_CustomRule.nDynamicTimeCount);
    DDX_Text(pDX, IDC_EDIT19, g_CustomRule.cbDynamicDropProbabilityLimit);
    DDX_Text(pDX, IDC_EDIT14, g_CustomRule.cbDynamicDropProbability[0]);
    DDX_Text(pDX, IDC_EDIT15, g_CustomRule.cbDynamicDropProbability[1]);
    DDX_Text(pDX, IDC_EDIT31, g_CustomRule.nDynamicReturnRatio);

    DDX_Text(pDX, IDC_EDIT29, g_CustomRule.nMaxGoldCount);
    DDX_Text(pDX, IDC_EDIT30, g_CustomRule.nFanShuiGoldCount);
}

// ��ʼ������
BOOL CDialogGeneral::OnInitDialog()
{
    // ��ʼ������
    __super::OnInitDialog();

    return TRUE;
}

// ��Ϣ����
bool CDialogGeneral::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
    // �ж���Ϣ
    switch(nMessageID)
    {
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// ���ԶԻ���
IMPLEMENT_DYNAMIC(CDialogGift, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogGift, CDialogDexter)

END_MESSAGE_MAP()

// ���캯��
CDialogGift::CDialogGift(int nShowMax/* = 0*/, CDialogDebug *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_GIFT, nShowMax, pParent)
{
}

// ��������
CDialogGift::~CDialogGift()
{
}

// �ؼ���
void CDialogGift::DoDataExchange(CDataExchange *pDX)
{
    CDialogDexter::DoDataExchange(pDX);

    DDX_Check(pDX, IDC_CHECK29, g_CustomRule.nGiftFeatures);

    DDX_Text(pDX, IDC_EDIT196, g_CustomRule.nGiftNoDropProbability);
    DDX_Text(pDX, IDC_EDIT68, g_CustomRule.nGoldNoDropProbability);
    DDX_Text(pDX, IDC_EDIT69, g_CustomRule.nGameDifficulty);

    DDX_Check(pDX, IDC_CHECK1, g_CustomRule.nGiftEnabled[0]);
    DDX_Check(pDX, IDC_CHECK2, g_CustomRule.nGiftEnabled[1]);
    DDX_Check(pDX, IDC_CHECK3, g_CustomRule.nGiftEnabled[2]);
    DDX_Check(pDX, IDC_CHECK4, g_CustomRule.nGiftEnabled[3]);
    DDX_Check(pDX, IDC_CHECK5, g_CustomRule.nGiftEnabled[4]);
    DDX_Check(pDX, IDC_CHECK6, g_CustomRule.nGiftEnabled[5]);
    DDX_Check(pDX, IDC_CHECK7, g_CustomRule.nGiftEnabled[6]);
    DDX_Check(pDX, IDC_CHECK8, g_CustomRule.nGiftEnabled[7]);
    DDX_Check(pDX, IDC_CHECK9, g_CustomRule.nGiftEnabled[8]);
    DDX_Check(pDX, IDC_CHECK10, g_CustomRule.nGiftEnabled[9]);
    DDX_Check(pDX, IDC_CHECK11, g_CustomRule.nGiftEnabled[10]);
    DDX_Check(pDX, IDC_CHECK12, g_CustomRule.nGiftEnabled[11]);
    DDX_Check(pDX, IDC_CHECK13, g_CustomRule.nGiftEnabled[12]);
    DDX_Check(pDX, IDC_CHECK14, g_CustomRule.nGiftEnabled[13]);
    DDX_Check(pDX, IDC_CHECK15, g_CustomRule.nGiftEnabled[14]);
    DDX_Check(pDX, IDC_CHECK16, g_CustomRule.nGiftEnabled[15]);
    DDX_Check(pDX, IDC_CHECK17, g_CustomRule.nGiftEnabled[16]);
    DDX_Check(pDX, IDC_CHECK18, g_CustomRule.nGiftEnabled[17]);

    DDX_Text(pDX, IDC_EDIT70, g_CustomRule.lGiftScore[0]);
    DDX_Text(pDX, IDC_EDIT71, g_CustomRule.lGiftScore[1]);
    DDX_Text(pDX, IDC_EDIT72, g_CustomRule.lGiftScore[2]);
    DDX_Text(pDX, IDC_EDIT73, g_CustomRule.lGiftScore[3]);
    DDX_Text(pDX, IDC_EDIT74, g_CustomRule.lGiftScore[4]);
    DDX_Text(pDX, IDC_EDIT75, g_CustomRule.lGiftScore[5]);
    DDX_Text(pDX, IDC_EDIT76, g_CustomRule.lGiftScore[6]);
    DDX_Text(pDX, IDC_EDIT77, g_CustomRule.lGiftScore[7]);
    DDX_Text(pDX, IDC_EDIT78, g_CustomRule.lGiftScore[8]);
    DDX_Text(pDX, IDC_EDIT79, g_CustomRule.lGiftScore[9]);
    DDX_Text(pDX, IDC_EDIT80, g_CustomRule.lGiftScore[10]);
    DDX_Text(pDX, IDC_EDIT81, g_CustomRule.lGiftScore[11]);
    DDX_Text(pDX, IDC_EDIT82, g_CustomRule.lGiftScore[12]);
    DDX_Text(pDX, IDC_EDIT83, g_CustomRule.lGiftScore[13]);
    DDX_Text(pDX, IDC_EDIT84, g_CustomRule.lGiftScore[14]);
    DDX_Text(pDX, IDC_EDIT85, g_CustomRule.lGiftScore[15]);
    DDX_Text(pDX, IDC_EDIT86, g_CustomRule.lGiftScore[16]);
    DDX_Text(pDX, IDC_EDIT87, g_CustomRule.lGiftScore[17]);

    DDX_Text(pDX, IDC_EDIT88, g_CustomRule.nGiftShowTime[0]);
    DDX_Text(pDX, IDC_EDIT89, g_CustomRule.nGiftShowTime[1]);
    DDX_Text(pDX, IDC_EDIT90, g_CustomRule.nGiftShowTime[2]);
    DDX_Text(pDX, IDC_EDIT91, g_CustomRule.nGiftShowTime[3]);
    DDX_Text(pDX, IDC_EDIT92, g_CustomRule.nGiftShowTime[4]);
    DDX_Text(pDX, IDC_EDIT93, g_CustomRule.nGiftShowTime[5]);
    DDX_Text(pDX, IDC_EDIT94, g_CustomRule.nGiftShowTime[6]);
    DDX_Text(pDX, IDC_EDIT95, g_CustomRule.nGiftShowTime[7]);
    DDX_Text(pDX, IDC_EDIT96, g_CustomRule.nGiftShowTime[8]);
    DDX_Text(pDX, IDC_EDIT97, g_CustomRule.nGiftShowTime[9]);
    DDX_Text(pDX, IDC_EDIT98, g_CustomRule.nGiftShowTime[10]);
    DDX_Text(pDX, IDC_EDIT99, g_CustomRule.nGiftShowTime[11]);
    DDX_Text(pDX, IDC_EDIT100, g_CustomRule.nGiftShowTime[12]);
    DDX_Text(pDX, IDC_EDIT101, g_CustomRule.nGiftShowTime[13]);
    DDX_Text(pDX, IDC_EDIT102, g_CustomRule.nGiftShowTime[14]);
    DDX_Text(pDX, IDC_EDIT103, g_CustomRule.nGiftShowTime[15]);
    DDX_Text(pDX, IDC_EDIT104, g_CustomRule.nGiftShowTime[16]);
    DDX_Text(pDX, IDC_EDIT105, g_CustomRule.nGiftShowTime[17]);

    DDX_Text(pDX, IDC_EDIT106, g_CustomRule.nGiftDropProbability[0]);
    DDX_Text(pDX, IDC_EDIT107, g_CustomRule.nGiftDropProbability[1]);
    DDX_Text(pDX, IDC_EDIT108, g_CustomRule.nGiftDropProbability[2]);
    DDX_Text(pDX, IDC_EDIT109, g_CustomRule.nGiftDropProbability[3]);
    DDX_Text(pDX, IDC_EDIT110, g_CustomRule.nGiftDropProbability[4]);
    DDX_Text(pDX, IDC_EDIT111, g_CustomRule.nGiftDropProbability[5]);
    DDX_Text(pDX, IDC_EDIT112, g_CustomRule.nGiftDropProbability[6]);
    DDX_Text(pDX, IDC_EDIT113, g_CustomRule.nGiftDropProbability[7]);
    DDX_Text(pDX, IDC_EDIT114, g_CustomRule.nGiftDropProbability[8]);
    DDX_Text(pDX, IDC_EDIT115, g_CustomRule.nGiftDropProbability[9]);
    DDX_Text(pDX, IDC_EDIT116, g_CustomRule.nGiftDropProbability[10]);
    DDX_Text(pDX, IDC_EDIT117, g_CustomRule.nGiftDropProbability[11]);
    DDX_Text(pDX, IDC_EDIT118, g_CustomRule.nGiftDropProbability[12]);
    DDX_Text(pDX, IDC_EDIT119, g_CustomRule.nGiftDropProbability[13]);
    DDX_Text(pDX, IDC_EDIT120, g_CustomRule.nGiftDropProbability[14]);
    DDX_Text(pDX, IDC_EDIT121, g_CustomRule.nGiftDropProbability[15]);
    DDX_Text(pDX, IDC_EDIT122, g_CustomRule.nGiftDropProbability[16]);
    DDX_Text(pDX, IDC_EDIT123, g_CustomRule.nGiftDropProbability[17]);


    DDX_Text(pDX, IDC_EDIT124, g_CustomRule.nGiftScoreDropProbability[0]);
    DDX_Text(pDX, IDC_EDIT125, g_CustomRule.nGiftScoreDropProbability[1]);
    DDX_Text(pDX, IDC_EDIT126, g_CustomRule.nGiftScoreDropProbability[2]);
    DDX_Text(pDX, IDC_EDIT127, g_CustomRule.nGiftScoreDropProbability[3]);
    DDX_Text(pDX, IDC_EDIT128, g_CustomRule.nGiftScoreDropProbability[4]);
    DDX_Text(pDX, IDC_EDIT129, g_CustomRule.nGiftScoreDropProbability[5]);
    DDX_Text(pDX, IDC_EDIT130, g_CustomRule.nGiftScoreDropProbability[6]);
    DDX_Text(pDX, IDC_EDIT131, g_CustomRule.nGiftScoreDropProbability[7]);
    DDX_Text(pDX, IDC_EDIT132, g_CustomRule.nGiftScoreDropProbability[8]);
    DDX_Text(pDX, IDC_EDIT133, g_CustomRule.nGiftScoreDropProbability[9]);
    DDX_Text(pDX, IDC_EDIT134, g_CustomRule.nGiftScoreDropProbability[10]);
    DDX_Text(pDX, IDC_EDIT135, g_CustomRule.nGiftScoreDropProbability[11]);
    DDX_Text(pDX, IDC_EDIT136, g_CustomRule.nGiftScoreDropProbability[12]);
    DDX_Text(pDX, IDC_EDIT137, g_CustomRule.nGiftScoreDropProbability[13]);
    DDX_Text(pDX, IDC_EDIT138, g_CustomRule.nGiftScoreDropProbability[14]);
    DDX_Text(pDX, IDC_EDIT139, g_CustomRule.nGiftScoreDropProbability[15]);
    DDX_Text(pDX, IDC_EDIT140, g_CustomRule.nGiftScoreDropProbability[16]);
    DDX_Text(pDX, IDC_EDIT141, g_CustomRule.nGiftScoreDropProbability[17]);
}

// ��ʼ������
BOOL CDialogGift::OnInitDialog()
{
    // ��ʼ������
    __super::OnInitDialog();

    return TRUE;
}

// ��Ϣ����
bool CDialogGift::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////

// ���ԶԻ���
IMPLEMENT_DYNAMIC(CDialogDelivery, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogDelivery, CDialogDexter)

END_MESSAGE_MAP()

// ���캯��
CDialogDelivery::CDialogDelivery(int nShowMax/* = 0*/, CDialogDebug *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_GIFT, nShowMax, pParent)
{
}

// ��������
CDialogDelivery::~CDialogDelivery()
{
}

// �ؼ���
void CDialogDelivery::DoDataExchange(CDataExchange *pDX)
{
    CDialogDexter::DoDataExchange(pDX);

    DDX_Check(pDX, IDC_CHECK19, g_CustomRule.nKindRewardEnabled[0]);
    DDX_Check(pDX, IDC_CHECK20, g_CustomRule.nKindRewardEnabled[1]);
    DDX_Check(pDX, IDC_CHECK21, g_CustomRule.nKindRewardEnabled[2]);
    DDX_Check(pDX, IDC_CHECK22, g_CustomRule.nKindRewardEnabled[3]);
    DDX_Check(pDX, IDC_CHECK23, g_CustomRule.nKindRewardEnabled[4]);
    DDX_Check(pDX, IDC_CHECK24, g_CustomRule.nKindRewardEnabled[5]);
    DDX_Check(pDX, IDC_CHECK25, g_CustomRule.nKindRewardEnabled[6]);
    DDX_Check(pDX, IDC_CHECK26, g_CustomRule.nKindRewardEnabled[7]);
    DDX_Check(pDX, IDC_CHECK27, g_CustomRule.nKindRewardEnabled[8]);
    DDX_Check(pDX, IDC_CHECK28, g_CustomRule.nKindRewardEnabled[9]);

    DDX_Text(pDX, IDC_EDIT142, g_CustomRule.lKindRewardRecharge[0]);
    DDX_Text(pDX, IDC_EDIT143, g_CustomRule.lKindRewardRecharge[1]);
    DDX_Text(pDX, IDC_EDIT144, g_CustomRule.lKindRewardRecharge[2]);
    DDX_Text(pDX, IDC_EDIT145, g_CustomRule.lKindRewardRecharge[3]);
    DDX_Text(pDX, IDC_EDIT146, g_CustomRule.lKindRewardRecharge[4]);
    DDX_Text(pDX, IDC_EDIT147, g_CustomRule.lKindRewardRecharge[5]);
    DDX_Text(pDX, IDC_EDIT148, g_CustomRule.lKindRewardRecharge[6]);
    DDX_Text(pDX, IDC_EDIT149, g_CustomRule.lKindRewardRecharge[7]);
    DDX_Text(pDX, IDC_EDIT150, g_CustomRule.lKindRewardRecharge[8]);
    DDX_Text(pDX, IDC_EDIT151, g_CustomRule.lKindRewardRecharge[9]);
    DDX_Text(pDX, IDC_EDIT181, g_CustomRule.lKindRewardRecharge[10]);
    DDX_Text(pDX, IDC_EDIT184, g_CustomRule.lKindRewardRecharge[11]);
    DDX_Text(pDX, IDC_EDIT187, g_CustomRule.lKindRewardRecharge[12]);
    DDX_Text(pDX, IDC_EDIT190, g_CustomRule.lKindRewardRecharge[13]);
    DDX_Text(pDX, IDC_EDIT193, g_CustomRule.lKindRewardRecharge[14]);

    DDX_Text(pDX, IDC_EDIT152, g_CustomRule.lKindRewardScore[0]);
    DDX_Text(pDX, IDC_EDIT153, g_CustomRule.lKindRewardScore[1]);
    DDX_Text(pDX, IDC_EDIT154, g_CustomRule.lKindRewardScore[2]);
    DDX_Text(pDX, IDC_EDIT155, g_CustomRule.lKindRewardScore[3]);
    DDX_Text(pDX, IDC_EDIT156, g_CustomRule.lKindRewardScore[4]);
    DDX_Text(pDX, IDC_EDIT157, g_CustomRule.lKindRewardScore[5]);
    DDX_Text(pDX, IDC_EDIT158, g_CustomRule.lKindRewardScore[6]);
    DDX_Text(pDX, IDC_EDIT159, g_CustomRule.lKindRewardScore[7]);
    DDX_Text(pDX, IDC_EDIT160, g_CustomRule.lKindRewardScore[8]);
    DDX_Text(pDX, IDC_EDIT161, g_CustomRule.lKindRewardScore[9]);
    DDX_Text(pDX, IDC_EDIT182, g_CustomRule.lKindRewardScore[10]);
    DDX_Text(pDX, IDC_EDIT185, g_CustomRule.lKindRewardScore[11]);
    DDX_Text(pDX, IDC_EDIT188, g_CustomRule.lKindRewardScore[12]);
    DDX_Text(pDX, IDC_EDIT191, g_CustomRule.lKindRewardScore[13]);
    DDX_Text(pDX, IDC_EDIT194, g_CustomRule.lKindRewardScore[14]);

    DDX_Text(pDX, IDC_EDIT162, g_CustomRule.nKindRewardScoreTime[0]);
    DDX_Text(pDX, IDC_EDIT163, g_CustomRule.nKindRewardScoreTime[1]);
    DDX_Text(pDX, IDC_EDIT164, g_CustomRule.nKindRewardScoreTime[2]);
    DDX_Text(pDX, IDC_EDIT165, g_CustomRule.nKindRewardScoreTime[3]);
    DDX_Text(pDX, IDC_EDIT166, g_CustomRule.nKindRewardScoreTime[4]);
    DDX_Text(pDX, IDC_EDIT167, g_CustomRule.nKindRewardScoreTime[5]);
    DDX_Text(pDX, IDC_EDIT168, g_CustomRule.nKindRewardScoreTime[6]);
    DDX_Text(pDX, IDC_EDIT169, g_CustomRule.nKindRewardScoreTime[7]);
    DDX_Text(pDX, IDC_EDIT170, g_CustomRule.nKindRewardScoreTime[8]);
    DDX_Text(pDX, IDC_EDIT171, g_CustomRule.nKindRewardScoreTime[9]);
    DDX_Text(pDX, IDC_EDIT183, g_CustomRule.nKindRewardScoreTime[10]);
    DDX_Text(pDX, IDC_EDIT186, g_CustomRule.nKindRewardScoreTime[11]);
    DDX_Text(pDX, IDC_EDIT189, g_CustomRule.nKindRewardScoreTime[12]);
    DDX_Text(pDX, IDC_EDIT192, g_CustomRule.nKindRewardScoreTime[13]);
    DDX_Text(pDX, IDC_EDIT195, g_CustomRule.nKindRewardScoreTime[14]);
}

// ��ʼ������
BOOL CDialogDelivery::OnInitDialog()
{
    // ��ʼ������
    __super::OnInitDialog();

    return TRUE;
}

// ��Ϣ����
bool CDialogDelivery::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

