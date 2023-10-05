// ClientDebugItemSink.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientDebugItemSink.h"


// CClientDebugItemSinkDlg �Ի���

IMPLEMENT_DYNAMIC(CClientDebugItemSinkDlg, CDialog)
BEGIN_MESSAGE_MAP(CClientDebugItemSinkDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_REFRESHRULE, OnBnClickedButtonRefreshRule)
	ON_BN_CLICKED(IDC_BUTTON_SETRULE, OnBnClickedButtonSetRule)
	ON_BN_CLICKED(IDC_BTN_REFRESH_ROOMLIST, OnBnClickedBtnRefreshRoomlist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROOM, OnLvnItemchangedListRoom)
	ON_BN_CLICKED(IDC_BTN_SET_ROOMCTRL, OnBnClickedBtnSetRoomCtrl)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ROOMCTRL, OnBnClickedBtnCancelRoomCtrl)

    ON_BN_CLICKED(IDC_BT_RESET, OnReSet)
    ON_BN_CLICKED(IDC_BT_EXCUTE, OnExcute)
    ON_BN_CLICKED(IDC_BT_CURSET, OnRefresh)
    ON_BN_CLICKED(IDC_RADIO_CT_AREA, OnRadioClick)
    ON_BN_CLICKED(IDC_RADIO_CT_BANKER, OnRadioClick)

    ON_BN_CLICKED(IDC_BTN_USER_BET_QUERY, OnBnClickedBtnUserBetQuery)
    ON_BN_CLICKED(IDC_BTN_USER_BET_ALL, OnBnClickedBtnUserBetAll)
    ON_EN_SETFOCUS(IDC_EDIT_USER_ID, OnEnSetfocusEditUserId)

	ON_EN_CHANGE(IDC_EDIT_INITIAL_SYSSTORAGE, OnEnChangeEditCurSysstorage)
	ON_EN_CHANGE(IDC_EDIT_INITIAL_PLAYERSTORAGE, OnEnChangeEditCurPlayerstorage)
	ON_EN_CHANGE(IDC_EDIT_INITIAL_PARAMETER_K, OnEnChangeEditCurParameterK)
	ON_EN_CHANGE(IDC_EDIT_ROOMCTRL_SYSSTORAGE, OnEnChangeEditRoomctrlSysstorage)
	ON_EN_CHANGE(IDC_EDIT_ROOMCTRL_PLAYERSTORAGE, OnEnChangeEditRoomctrlPlayerstorage)
	ON_EN_CHANGE(IDC_EDIT_ROOMCTRL_PARAMETER_K, OnEnChangeEditRoomctrlParameterK)

    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// CClientDebugItemSinkDlg ��Ϣ�������
CClientDebugItemSinkDlg::CClientDebugItemSinkDlg(CWnd *pParent /*=NULL*/)
    : CDialog(CClientDebugItemSinkDlg::IDD, pParent)
{
	ZeroMemory(&m_CustomRule, sizeof(m_CustomRule));
	m_lCurSysStorage = 0;
	m_lCurPlayerStorage = 0;
	m_lCurParameterK = 0;

    m_UserBetArray.RemoveAll();
    m_lQueryGameID = -1;

    ZeroMemory(&m_UserInfo, sizeof(m_UserInfo));
    m_pParentWnd = NULL;
    m_pIClientDebugCallback = NULL;
}

CClientDebugItemSinkDlg::~CClientDebugItemSinkDlg()
{
}

void CClientDebugItemSinkDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_LIST_USER_BET, m_listUserBet);
    DDX_Control(pDX, IDC_LIST_USER_BET_ALL, m_listUserBetAll);

	DDX_Text(pDX, IDC_EDIT_INITIAL_SYSSTORAGE, m_CustomRule.lConfigSysStorage);
	DDX_Text(pDX, IDC_EDIT_CUR_SYSSTORAGE, m_lCurSysStorage);
	DDX_Text(pDX, IDC_EDIT_INITIAL_PLAYERSTORAGE, m_CustomRule.lConfigPlayerStorage);
	DDX_Text(pDX, IDC_EDIT_CUR_PLAYERSTORAGE, m_lCurPlayerStorage);
	DDX_Text(pDX, IDC_EDIT_INITIAL_PARAMETER_K, m_CustomRule.lConfigParameterK);
	DDX_Text(pDX, IDC_EDIT_CUR_PARAMETER_K, m_lCurParameterK);
}

//������ע
void __cdecl CClientDebugItemSinkDlg::UpdateUserBet(bool bReSet)
{
    //����б�
    m_listUserBet.DeleteAllItems();

    if(bReSet)
    {
        m_lQueryGameID = -1;

        SetDlgItemText(IDC_EDIT_USER_ID, TEXT("���������ID"));

        for(BYTE cbSubItem = 3; cbSubItem <= 8; cbSubItem++)
        {
            m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
        }

        GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
        GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

        return;
    }

    if(0 == m_UserBetArray.GetCount())
    {
        return;
    }

    LONGLONG lAllBetScore[AREA_COUNT + 1];
    ZeroMemory(lAllBetScore, sizeof(lAllBetScore));

    INT nItemIndex = 0;
    for(INT i = 0; i < m_UserBetArray.GetCount(); i++)
    {
        //У���û�ID
        if(-1 != m_lQueryGameID && m_UserBetArray[i].dwUserGameID != m_lQueryGameID)
        {
            continue;
        }

        //��������
        m_listUserBet.InsertItem(nItemIndex, TEXT(""));

        CString strInfo;
        strInfo.Format(TEXT("%d"), nItemIndex + 1);
        m_listUserBet.SetItemText(nItemIndex, 1, strInfo);

        m_listUserBet.SetItemText(nItemIndex, 2, m_UserBetArray[i].szNickName);

        strInfo.Format(TEXT("%u"), m_UserBetArray[i].dwUserGameID);
        m_listUserBet.SetItemText(nItemIndex, 3, strInfo);

        strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserStartScore);
        m_listUserBet.SetItemText(nItemIndex, 4, strInfo);

        strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserWinLost);
        m_listUserBet.SetItemText(nItemIndex, 5, strInfo);

        INT nSubItem = -1;
        for(BYTE j = 1; j <= AREA_COUNT; j++)
        {
            nSubItem = j + 5;

            strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserBet[j]);
            m_listUserBet.SetItemText(nItemIndex, nSubItem, strInfo);

            lAllBetScore[j] += m_UserBetArray[i].lUserBet[j];
        }

        nItemIndex++;
    }

    INT nSubItemAll = -1;
    for(BYTE k = 1; k <= AREA_COUNT; k++)
    {
        nSubItemAll = k + 2;

        CString strInfoAll;
        strInfoAll.Format(TEXT("%I64d"), lAllBetScore[k]);
        m_listUserBetAll.SetItemText(0, nSubItemAll, strInfoAll);
    }

    return;
}

//������
bool  CClientDebugItemSinkDlg::ReqResult(const void *pBuffer)
{
    const CMD_S_CommandResult *pResult = (CMD_S_CommandResult *)pBuffer;
    CString str;
    switch(pResult->cbAckType)
    {
    case ACK_SET_WIN_AREA:
    {
        if(pResult->cbResult == CR_ACCEPT)
        {
            str = TEXT("��Ӯ���������Ѿ����ܣ�");
            switch(m_cbDebugStyle)
            {
            case CS_BET_AREA:
            {
                str = TEXT("ʤ������:");
                BYTE cbIndex = 0;
                bool bFlags = false;
                for(cbIndex = 0; cbIndex < AREA_COUNT; cbIndex++)
                {
                    if(m_bWinArea[cbIndex])
                    {
                        bFlags = true;
                        switch(cbIndex)
                        {
                        case 0:
                            str += TEXT("�� ");
                            break;
                        case 1:
                            str += TEXT("�� ");
                            break;
                        case 2:
                            str += TEXT("�� ");
                            break;
                        case 3:
                            str += TEXT("�� ");
                            break;
                        default:
                            break;
                        }
                    }
                }
                if(!bFlags)
                {
                    str += TEXT("ׯ��ͨɱ");
                }
                str.AppendFormat(TEXT(",ִ�д���:%d"), m_cbExcuteTimes);
                break;
            }
            case CS_BANKER_WIN:
            {
                str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:Ӯ,ִ�д���:%d"), m_cbExcuteTimes);
                break;
            }
            case CS_BANKER_LOSE:
            {
                str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:��,ִ�д���:%d"), m_cbExcuteTimes);
                break;
            }
            default:
                str = TEXT("�ǵ���״̬");
                break;
            }

        }
        else
        {
            str.Format(TEXT("����ʧ��!"));
            m_cbExcuteTimes = 0;
            m_cbDebugStyle = 0;
            ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
        }
        break;
    }
    case ACK_RESET_DEBUG:
    {
        if(pResult->cbResult == CR_ACCEPT)
        {
            m_cbDebugStyle = 0;
            m_cbExcuteTimes = 0;
            ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
            ReSetAdminWnd();
            str = TEXT("ȡ������!");
        }
        break;
    }
    case ACK_PRINT_SYN:
    {
        if(pResult->cbResult == CR_ACCEPT)
        {
            str = TEXT("��ǰ������ͬ��!");

            tagAdminReq *pAdminReq = (tagAdminReq *)pResult->cbExtendData;
            m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
            m_cbDebugStyle = pAdminReq->m_cbDebugStyle;
            memcpy(m_bWinArea, pAdminReq->m_bWinArea, sizeof(m_bWinArea));
            ReSetAdminWnd();
        }
        else
        {
            str.Format(TEXT("��ǰ����ͬ��ʧ��!"));
        }
        break;
    }

    default:
        break;
    }

    //SetWindowText(str);
    GetDlgItem(IDC_STATIC_CONTROL_INFO)->SetWindowText(str);

    return true;
}

void CClientDebugItemSinkDlg::ReSetAdminWnd()
{
    if(m_cbDebugStyle != CS_BANKER_WIN && m_cbDebugStyle != CS_BANKER_LOSE)
    {
        ((CButton *)GetDlgItem(IDC_RADIO_WIN))->SetCheck(0);
        ((CButton *)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(0);
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_RADIO_CT_BANKER))->SetCheck(1);
        ((CButton *)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(0);
        ((CButton *)GetDlgItem(IDC_RADIO_WIN))->SetCheck(m_cbDebugStyle == CS_BANKER_WIN ? 1 : 0);
        ((CButton *)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(m_cbDebugStyle == CS_BANKER_LOSE ? 1 : 0);
    }

    if(m_cbDebugStyle == CS_BET_AREA)
    {
        ((CButton *)GetDlgItem(IDC_RADIO_CT_BANKER))->SetCheck(0);
        ((CButton *)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
    }

    BYTE cbIndex = 0;
    for(; cbIndex < DEBUG_AREA; cbIndex++)
    {
        ((CButton *)GetDlgItem(IDC_CHECK_TIAN + cbIndex))->SetCheck(m_bWinArea[cbIndex] ? 1 : 0);
    }
    ((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(m_cbExcuteTimes > 0 ? (m_cbExcuteTimes - 1) : -1);
    OnRadioClick();
}

void CClientDebugItemSinkDlg::PrintCurStatus()
{
    CString str;
    switch(m_cbDebugStyle)
    {
    case CS_BET_AREA:
    {
        str = TEXT("ʤ������:");
        BYTE cbIndex = 0;
        bool bFlags = false;
        for(cbIndex = 0; cbIndex < AREA_COUNT; cbIndex++)
        {
            if(m_bWinArea[cbIndex])
            {
                bFlags = true;
                switch(cbIndex)
                {
                case 0:
                    str += TEXT("�� ");
                    break;
                case 1:
                    str += TEXT("�� ");
                    break;
                case 2:
                    str += TEXT("�� ");
                    break;
                case 3:
                    str += TEXT("�� ");
                    break;
                default:
                    break;
                }
            }
        }
        if(!bFlags)
        {
            str += TEXT("ׯ��ͨɱ");
        }
        str.AppendFormat(TEXT(",ִ�д���:%d"), m_cbExcuteTimes);
        break;
    }
    case CS_BANKER_WIN:
    {
        str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:Ӯ,ִ�д���:%d"), m_cbExcuteTimes);
        break;
    }
    case CS_BANKER_LOSE:
    {
        str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:��,ִ�д���:%d"), m_cbExcuteTimes);
        break;
    }
    default:
        str = TEXT("�ǵ���״̬");
        break;
    }

    SetWindowText(str);
}

void CClientDebugItemSinkDlg::OnRadioClick()
{
    if(((CButton *)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck() == 1)
    {
        GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(TRUE);
        GetDlgItem(IDC_RADIO_WIN)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHECK_TIAN)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_DI)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_XUAN)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_HUANG)->EnableWindow(FALSE);
    }
    else
    {
        if(((CButton *)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
        {
            GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(FALSE);
            GetDlgItem(IDC_RADIO_WIN)->EnableWindow(FALSE);
            GetDlgItem(IDC_CHECK_TIAN)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHECK_DI)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHECK_XUAN)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHECK_HUANG)->EnableWindow(TRUE);
        }
    }
}

void CClientDebugItemSinkDlg::OnReSet()
{
    CMD_C_AdminReq adminReq;
    adminReq.cbReqType = RQ_RESET_DEBUG;

    SendDebugMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
}

void CClientDebugItemSinkDlg::OnRefresh()
{
    CMD_C_AdminReq adminReq;
    adminReq.cbReqType = RQ_PRINT_SYN;

    SendDebugMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
}

void CClientDebugItemSinkDlg::OnExcute()
{
    BYTE cbIndex = 0;
    bool bFlags = false;
    m_cbDebugStyle = 0;
    m_cbExcuteTimes = 0;
    ZeroMemory(m_bWinArea, sizeof(m_bWinArea));

    //����ׯ��
    if(((CButton *)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck())
    {
        if(((CButton *)GetDlgItem(IDC_RADIO_WIN))->GetCheck())
        {
            m_cbDebugStyle = CS_BANKER_WIN;
            bFlags = true;
        }
        else
        {
            if(((CButton *)GetDlgItem(IDC_RADIO_LOSE))->GetCheck())
            {
                m_cbDebugStyle = CS_BANKER_LOSE;
                bFlags = true;
            }
        }
    }
    else //��������
    {
        if(((CButton *)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
        {
            m_cbDebugStyle = CS_BET_AREA;
            for(cbIndex = 0; cbIndex < DEBUG_AREA; cbIndex++)
            {
                m_bWinArea[cbIndex] = ((CButton *)GetDlgItem(IDC_CHECK_TIAN + cbIndex))->GetCheck() ? true : false;
            }
            bFlags = true;
        }
    }

    m_cbExcuteTimes = (BYTE)((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel() + 1;
    //��ȡִ�д���
    if(m_cbExcuteTimes <= 0)
    {
        bFlags = false;
    }


    if(bFlags) //������Ч
    {
        CMD_C_AdminReq adminReq;
        ZeroMemory(&adminReq, sizeof(CMD_C_AdminReq));
        adminReq.cbReqType = RQ_SET_WIN_AREA;
        tagAdminReq *pAdminReq = (tagAdminReq *)adminReq.cbExtendData;
        pAdminReq->m_cbExcuteTimes = m_cbExcuteTimes;
        pAdminReq->m_cbDebugStyle = m_cbDebugStyle;
        memcpy(pAdminReq->m_bWinArea, m_bWinArea, sizeof(m_bWinArea));
        //AfxGetMainWnd()->SendMessage(IDM_ADMIN_COMMDN,&adminReq,0);

        SendDebugMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
    }
    else
    {
        AfxMessageBox(TEXT("��ѡ���ܿش����Լ��ܿط�ʽ!"));
        OnRefresh();
    }
}

//��ʼ��
BOOL CClientDebugItemSinkDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

	//��������
	((CEdit *)GetDlgItem(IDC_EDIT_CUR_SYSSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CUR_PLAYERSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CUR_PARAMETER_K))->LimitText(3);

	((CEdit *)GetDlgItem(IDC_EDIT_ROOMCTRL_SYSSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_ROOMCTRL_PLAYERSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_ROOMCTRL_PARAMETER_K))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_USER_ID))->LimitText(9);

    //�������
    ((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
    ((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
    ((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
    ((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
    ((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));

    ((CButton *)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
    OnRadioClick();

    GetDlgItem(IDC_STATIC_CONTROL_INFO)->SetWindowText(TEXT(""));

	//��������б�
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);
	pListCtrlRoom->SetExtendedStyle(pListCtrlRoom->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	INT nColumnCount = 0;
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("����"), LVCFMT_CENTER, 60);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("�������ϵͳ���"), LVCFMT_CENTER, 135);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("���������ҿ��"), LVCFMT_CENTER, 135);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("�������ϵ��"), LVCFMT_CENTER, 100);

    //�����ע
    m_listUserBet.SetExtendedStyle(m_listUserBet.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listUserBet.GetHeaderCtrl()->EnableWindow(false);
    nColumnCount = 0;
    m_listUserBet.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 0);
    m_listUserBet.InsertColumn(nColumnCount++, TEXT("���"), LVCFMT_CENTER, 60);
    m_listUserBet.InsertColumn(nColumnCount++, TEXT("����ǳ�"), LVCFMT_CENTER, 120);
    m_listUserBet.InsertColumn(nColumnCount++, TEXT("���ID"), LVCFMT_CENTER, 100);
    m_listUserBet.InsertColumn(nColumnCount++, TEXT("������"), LVCFMT_CENTER, 120);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("����Ӯ"), LVCFMT_CENTER, 100);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("��"), LVCFMT_CENTER, 100);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("��"), LVCFMT_CENTER, 100);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("��"), LVCFMT_CENTER, 100);
    m_listUserBet.InsertColumn(nColumnCount++, TEXT("��"), LVCFMT_CENTER, 100);

    m_listUserBetAll.SetExtendedStyle(m_listUserBetAll.GetExtendedStyle() | LVS_EX_GRIDLINES);
    nColumnCount = 0;
    m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 0);
    m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 280);
    m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 220);
	m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 100);
	m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 100);
	m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 100);
	m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 100);
    m_listUserBetAll.InsertItem(0, TEXT(""));
    m_listUserBetAll.SetItemText(0, 1, TEXT("��ʵ�����עͳ��"));
    for(BYTE cbSubItem = 3; cbSubItem <= 6; cbSubItem++)
    {
        m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
    }

    SetDlgItemText(IDC_EDIT_USER_ID, TEXT("���������ID"));

    GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
    GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

	//ˢ��
	OnBnClickedBtnRefreshRoomlist();
	SendDebugMessage(SUB_C_REFRESH_RULE);

	UpdateSysCtrlDesc();

    return TRUE;

}

void CClientDebugItemSinkDlg::OnBnClickedBtnUserBetQuery()
{
    //��ȡ�û�ID
    CString strUserID = TEXT("");
    GetDlgItemText(IDC_EDIT_USER_ID, strUserID);

    //ȥ���ո�
    strUserID.TrimLeft();
    strUserID.TrimRight();

    m_lQueryGameID = StrToInt(strUserID);

    GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(true);

    //���¿ؼ�
    UpdateUserBet(false);
}

void CClientDebugItemSinkDlg::OnBnClickedBtnUserBetAll()
{
    //��ʾȫ��
    m_lQueryGameID = -1;

    SetDlgItemText(IDC_EDIT_USER_ID, TEXT("���������ID"));

    GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
    GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

    //���¿ؼ�
    UpdateUserBet(false);

    return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientDebugItemSinkDlg::OnEnSetfocusEditUserId()
{
    SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
    return;
}

HBRUSH CClientDebugItemSinkDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    return hbr;
}

// �ͷŽӿ�
void CClientDebugItemSinkDlg::Release()
{
    delete this;
}

// ��������
bool CClientDebugItemSinkDlg::Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback)
{
    // ���ñ���
    m_pParentWnd = pParentWnd;
    m_pIClientDebugCallback = pIClientDebugCallback;

    // ��������
    __super::Create(IDD_DIALOG_ADMIN, GetDesktopWindow());

    return true;

}

// ��ʾ����
bool CClientDebugItemSinkDlg::ShowWindow(bool bShow)
{
    // ��ʾ����
    CDialog::ShowWindow(bShow ? SW_SHOW : SW_HIDE);

    return true;
}

// ��Ϣ����
bool CClientDebugItemSinkDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pBuffer, WORD wDataSize)
{
    // ��Ч�ӿ�
    if(m_pIClientDebugCallback == NULL)
    {
        return false;
    }

    switch(nMessageID)
    {
	case SUB_S_REFRESH_RULE_RESULT:
	{
		ASSERT(wDataSize == sizeof(CMD_S_RefreshRuleResult));
		if (wDataSize != sizeof(CMD_S_RefreshRuleResult))
		{
			return false;
		}

		CMD_S_RefreshRuleResult *pRefreshRuleResult = (CMD_S_RefreshRuleResult *)pBuffer;

		//m_CustomRule.lConfigSysStorage = pRefreshRuleResult->lInitialSysStorage;
		//m_CustomRule.lConfigPlayerStorage = pRefreshRuleResult->lInitialPlayerStorage;
		//m_CustomRule.lConfigParameterK = pRefreshRuleResult->lInitialParameterK;

		//m_lCurSysStorage = pRefreshRuleResult->lCurSysStorage;
		//m_lCurPlayerStorage = pRefreshRuleResult->lCurPlayerStorage;
		//m_lCurParameterK = pRefreshRuleResult->lCurParameterK;

		//��ֵλ�ñ任
		m_CustomRule.lConfigSysStorage = pRefreshRuleResult->lCurSysStorage;
		m_CustomRule.lConfigPlayerStorage = pRefreshRuleResult->lCurPlayerStorage;
		m_CustomRule.lConfigParameterK = pRefreshRuleResult->lCurParameterK;

		m_lCurSysStorage = pRefreshRuleResult->lInitialSysStorage;
		m_lCurPlayerStorage = pRefreshRuleResult->lInitialPlayerStorage;
		m_lCurParameterK = pRefreshRuleResult->lInitialParameterK;

		SetDlgItemText(IDC_STATIC_SETDESC, TEXT("ˢ�����óɹ�!"));

		UpdateData(FALSE);
		UpdateSysCtrlDesc();

		return true;
	}
	case SUB_S_SET_RULE_RESULT:
	{
		ASSERT(wDataSize == sizeof(CMD_S_SetRuleResult));
		if (wDataSize != sizeof(CMD_S_SetRuleResult))
		{
			return false;
		}

		CMD_S_SetRuleResult *pSetRuleResult = (CMD_S_SetRuleResult *)pBuffer;

		m_lCurSysStorage = pSetRuleResult->lCurSysStorage;
		m_lCurPlayerStorage = pSetRuleResult->lCurPlayerStorage;
		m_lCurParameterK = pSetRuleResult->lCurParameterK;

		SetDlgItemText(IDC_STATIC_SETDESC, TEXT("�������óɹ�!"));

		UpdateData(FALSE);
		return true;
	}
	case SUB_S_REFRESH_ROOMCTRL:
	{
		ASSERT(wDataSize % sizeof(CMD_S_RoomCtrlResult) == 0);
		if (wDataSize % sizeof(CMD_S_RoomCtrlResult) != 0)
		{
			return false;
		}

		CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);

		if (wDataSize == 0)
		{
			//�÷���ɾ��������
			//for (WORD wIndex = 0; wIndex<pListCtrlRoom->GetItemCount(); wIndex++)
			//{
			//	pListCtrlRoom->DeleteItem(wIndex);
			//}

			pListCtrlRoom->DeleteAllItems();
		}
		else
		{
			//������Ϣ
			CMD_S_RoomCtrlResult *pRoomCtrlResult = (CMD_S_RoomCtrlResult *)pBuffer;

			int nItemIndex = -1;
			for (WORD wIndex = 0; wIndex<pListCtrlRoom->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListCtrlRoom->GetItemText(wIndex, 0)) == pRoomCtrlResult->dwCtrlIndex)
				{
					nItemIndex = wIndex;
					break;
				}
			}

			if (nItemIndex == -1)
			{
				CString strCtrlIndex;
				strCtrlIndex.Format(TEXT("%d"), pRoomCtrlResult->dwCtrlIndex);

				int nItem = pListCtrlRoom->GetItemCount();
				nItemIndex = pListCtrlRoom->InsertItem(nItem, strCtrlIndex);

				CString strCtrlVal;
				strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlSysStorage);
				pListCtrlRoom->SetItemText(nItem, 1, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlPlayerStorage);
				pListCtrlRoom->SetItemText(nItem, 2, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlParameterK);
				pListCtrlRoom->SetItemText(nItem, 3, strCtrlVal);
			}
			else
			{
				CString strCtrlVal;
				strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlSysStorage);
				pListCtrlRoom->SetItemText(nItemIndex, 1, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlPlayerStorage);
				pListCtrlRoom->SetItemText(nItemIndex, 2, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlParameterK);
				pListCtrlRoom->SetItemText(nItemIndex, 3, strCtrlVal);
			}
		}

		return true;
	}
	case SUB_S_ROOMCTRL_RES:
	{
		ASSERT(sizeof(CMD_S_RoomCtrlResult) == wDataSize);
		if (sizeof(CMD_S_RoomCtrlResult) != wDataSize)
		{
			return false;
		}

		//������Ϣ
		CMD_S_RoomCtrlResult *pRoomCtrlResult = (CMD_S_RoomCtrlResult *)pBuffer;
		CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);

		if (!pRoomCtrlResult->bCancelSucceed)
		{
			int nItemIndex = -1;
			for (WORD wIndex = 0; wIndex<pListCtrlRoom->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListCtrlRoom->GetItemText(wIndex, 0)) == pRoomCtrlResult->dwCtrlIndex)
				{
					nItemIndex = wIndex;
					break;
				}
			}

			CString strCtrlIndex;
			strCtrlIndex.Format(TEXT("%d"), pRoomCtrlResult->dwCtrlIndex);

			int nItem = pListCtrlRoom->GetItemCount();

			if (nItemIndex == -1)
			{
				pListCtrlRoom->InsertItem(nItem, strCtrlIndex);
			}
			else
			{
				nItem = nItemIndex;
			}

			CString strCtrlVal;
			strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlSysStorage);
			pListCtrlRoom->SetItemText(nItem, 1, strCtrlVal);

			strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlPlayerStorage);
			pListCtrlRoom->SetItemText(nItem, 2, strCtrlVal);

			strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlParameterK);
			pListCtrlRoom->SetItemText(nItem, 3, strCtrlVal);
		}
		else
		{
			//ɾ������
			for (WORD wIndex = 0; wIndex<pListCtrlRoom->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListCtrlRoom->GetItemText(wIndex, 0)) == pRoomCtrlResult->dwCtrlIndex)
				{
					pListCtrlRoom->DeleteItem(wIndex);
					break;
				}
			}
		}

		return true;
	}
	case  SUB_S_DEBUG_TEXT:
	{
		//У���С
		ASSERT(sizeof(CMD_S_DebugText) == wDataSize);
		if (sizeof(CMD_S_DebugText) != wDataSize)
		{
			return false;
		}

		// ������Ϣ
		CMD_S_DebugText *pDebugText = (CMD_S_DebugText *)pBuffer;

		MessageBox(pDebugText->szMessageText, TEXT("��ܰ��ʾ"));

		return true;
	}
	case SUB_S_CLEAN_ROOMCTRL:
	{
		CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);

		//�÷���ɾ��������
		//for (WORD wIndex = 0; wIndex<pListCtrlRoom->GetItemCount(); wIndex++)
		//{
		//	pListCtrlRoom->DeleteItem(wIndex);
		//}

		pListCtrlRoom->DeleteAllItems();

		return true;
	}
    case SUB_GET_ROMM_ALL_USER_RESULT:
    {
        ASSERT(wDataSize == sizeof(CMD_CF_UserInfo));
        if(wDataSize != sizeof(CMD_CF_UserInfo))
        {
            return false;
        }

        CopyMemory(&m_UserInfo, pBuffer, sizeof(m_UserInfo));

        return true;
    }
    case SUB_S_AMDIN_COMMAND:
    {
        return ReqResult(pBuffer);
    }
    case SUB_S_SEND_USER_BET_INFO:	//���¿��
    {
        //Ч������
        ASSERT(wDataSize == sizeof(CMD_S_SendUserBetInfo));
        if(wDataSize != sizeof(CMD_S_SendUserBetInfo))
        {
            return false;
        }

        //�����û��б�
        SendDebugMessage(SUB_GET_ROMM_ALL_USER);

        //��Ϣ����
        CMD_S_SendUserBetInfo *pSendUserBetInfo = (CMD_S_SendUserBetInfo *)pBuffer;
        LONGLONG lUserJettonScore[AREA_COUNT + 1][GAME_PLAYER];

        for(WORD warea = 0; warea < AREA_COUNT + 1; warea++)
        {
            for(WORD i = 0; i < GAME_PLAYER; i++)
            {
                lUserJettonScore[warea][i] = (LONGLONG)(pSendUserBetInfo->lUserJettonScore[warea][i]);
            }
        }

        CUserBetArray *pUserBetArray = &m_UserBetArray;
        pUserBetArray->RemoveAll();

        for(WORD i = 0; i < GAME_PLAYER; i++)
        {
            WORD nUserIndex = GetUserInfoIndex(i);
            if(nUserIndex == MAX_CHAIR)
            {
                continue;
            }

            LONGLONG lUserAllBet = 0;
            for(BYTE j = 1; j <= AREA_COUNT; j++)
            {
                lUserAllBet += lUserJettonScore[j][i];
            }
            if(0 == lUserAllBet)
            {
                continue;
            }

            //��������
            tagUserBet UserBet;
            ZeroMemory(&UserBet, sizeof(UserBet));

            CopyMemory(UserBet.szNickName, m_UserInfo.szNickName[nUserIndex], sizeof(UserBet.szNickName));
            UserBet.dwUserGameID = m_UserInfo.wGameID[nUserIndex];
            UserBet.lUserStartScore = pSendUserBetInfo->lUserStartScore[i];
            UserBet.lUserWinLost = m_UserInfo.lUserScore[nUserIndex] - UserBet.lUserStartScore;
            for(BYTE k = 1; k <= AREA_COUNT; k++)
            {
                UserBet.lUserBet[k] = lUserJettonScore[k][i];
            }

            //��������
            pUserBetArray->Add(UserBet);
        }

        //���¿ؼ�
        UpdateUserBet(false);

        return true;
    }
    }
    return false;
}

// ������Ϣ
bool CClientDebugItemSinkDlg::SendDebugMessage(UINT nMessageID, void *pBuffer, UINT wDataSize)
{
    if(m_pIClientDebugCallback != NULL)
    {
        // ��ȡ��Ϣ
        CString StrTableID;

        // �ж���Чֵ
        if(StrTableID.IsEmpty())
        {
            StrTableID = TEXT("0");
        }
        // ������Ϣ
        m_pIClientDebugCallback->OnDebugInfo(nMessageID, _ttoi(StrTableID), pBuffer, wDataSize);
    }

    return true;
}

//ȡ���û���Ϣ
int CClientDebugItemSinkDlg::GetUserInfoIndex(WORD wChairID, WORD wTableID)
{
    for(INT i = 00; i < m_UserInfo.wUserCount; i++)
    {
        if(m_UserInfo.wTableID[i] == wTableID && m_UserInfo.wChairID[i] == wChairID)
        {
            return i;
        }
    }

    return MAX_CHAIR;
}

bool CClientDebugItemSinkDlg::IsValidNum(CString strEdit)
{
	SCORE lScoreValue = _ttoi64(strEdit);
	CString strCmp;
	strCmp.Format(TEXT("%I64d"), lScoreValue);

	return strCmp == strEdit;
}

void CClientDebugItemSinkDlg::OnBnClickedButtonRefreshRule()
{
	SendDebugMessage(SUB_C_REFRESH_RULE);

	return;
}

void CClientDebugItemSinkDlg::OnBnClickedButtonSetRule()
{
	UpdateData(TRUE);

	if (!(m_lCurParameterK > 0 && m_lCurParameterK <= 100))
	{
		SetDlgItemText(IDC_STATIC_SETDESC, TEXT("��ǰ����ϵ����������������!"));
		return;
	}

	CMD_C_SetRule SetRule;
	ZeroMemory(&SetRule, sizeof(SetRule));

	SetRule.lCurSysStorage = m_lCurSysStorage;
	SetRule.lCurPlayerStorage = m_lCurPlayerStorage;
	SetRule.lCurParameterK = m_lCurParameterK;

	SendDebugMessage(SUB_C_SET_RULE, &SetRule, sizeof(SetRule));

	return;
}

void CClientDebugItemSinkDlg::OnBnClickedBtnRefreshRoomlist()
{
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);

	//�÷���ɾ��������
	//for (WORD wIndex = 0; wIndex<pListCtrlRoom->GetItemCount(); wIndex++)
	//{
	//	pListCtrlRoom->DeleteItem(wIndex);
	//}

	pListCtrlRoom->DeleteAllItems();

	SendDebugMessage(SUB_C_REFRESH_ROOMCTRL);
}

void CClientDebugItemSinkDlg::OnLvnItemchangedListRoom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ((pNMLV->uChanged == LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		//��������
		int nItem = pNMLV->iItem;
		if (nItem >= 0)
		{
			CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);

			SetDlgItemText(IDC_EDIT_ROOMCTRL_SYSSTORAGE, pListCtrlRoom->GetItemText(nItem, 1));
			SetDlgItemText(IDC_EDIT_ROOMCTRL_PLAYERSTORAGE, pListCtrlRoom->GetItemText(nItem, 2));
			SetDlgItemText(IDC_EDIT_ROOMCTRL_PARAMETER_K, pListCtrlRoom->GetItemText(nItem, 3));

		}
	}

	*pResult = 0;
}

void CClientDebugItemSinkDlg::OnBnClickedBtnSetRoomCtrl()
{
	CString strRoomCtrlSysStorage;
	GetDlgItemText(IDC_EDIT_ROOMCTRL_SYSSTORAGE, strRoomCtrlSysStorage);
	strRoomCtrlSysStorage.TrimLeft();
	strRoomCtrlSysStorage.TrimRight();

	if (!IsValidNum(strRoomCtrlSysStorage) || strRoomCtrlSysStorage == TEXT("0") || strRoomCtrlSysStorage.IsEmpty())
	{
		MessageBox(TEXT("�������ϵͳ�����д��Ч��"), TEXT("��ܰ��ʾ"));
		return;
	}

	LPTSTR lpszRoomCtrlSysStorage = new TCHAR[strRoomCtrlSysStorage.GetLength() + 1];
	_tcscpy_s(lpszRoomCtrlSysStorage, strRoomCtrlSysStorage.GetLength() + 1, strRoomCtrlSysStorage);
	SCORE lRoomCtrlSysStorage = _ttoi64(lpszRoomCtrlSysStorage);

	CString strRoomCtrlPlayerStorage;
	GetDlgItemText(IDC_EDIT_ROOMCTRL_PLAYERSTORAGE, strRoomCtrlPlayerStorage);
	strRoomCtrlPlayerStorage.TrimLeft();
	strRoomCtrlPlayerStorage.TrimRight();

	if (!IsValidNum(strRoomCtrlPlayerStorage) || strRoomCtrlPlayerStorage == TEXT("0") || strRoomCtrlPlayerStorage.IsEmpty())
	{
		MessageBox(TEXT("���������ҿ����д��Ч��"), TEXT("��ܰ��ʾ"));
		return;
	}

	LPTSTR lpszRoomCtrlPlayerStorage = new TCHAR[strRoomCtrlPlayerStorage.GetLength() + 1];
	_tcscpy_s(lpszRoomCtrlPlayerStorage, strRoomCtrlPlayerStorage.GetLength() + 1, strRoomCtrlPlayerStorage);
	SCORE lRoomCtrlPlayerStorage = _ttoi64(lpszRoomCtrlPlayerStorage);

	CString strRoomCtrlParameterK;
	GetDlgItemText(IDC_EDIT_ROOMCTRL_PARAMETER_K, strRoomCtrlParameterK);
	strRoomCtrlParameterK.TrimLeft();
	strRoomCtrlParameterK.TrimRight();

	if (!IsValidNum(strRoomCtrlParameterK) || strRoomCtrlParameterK == TEXT("0") || strRoomCtrlParameterK.IsEmpty())
	{
		MessageBox(TEXT("�������ϵ����д��Ч��"), TEXT("��ܰ��ʾ"));
		return;
	}

	LPTSTR lpszRoomCtrlParameterK = new TCHAR[strRoomCtrlParameterK.GetLength() + 1];
	_tcscpy_s(lpszRoomCtrlParameterK, strRoomCtrlParameterK.GetLength() + 1, strRoomCtrlParameterK);
	SCORE lRoomCtrlParameterK = _ttoi64(lpszRoomCtrlParameterK);

	//У��
	if (!(lRoomCtrlParameterK > 0 && lRoomCtrlParameterK <= 100))
	{
		AfxMessageBox(TEXT("�������ϵ����Χ0~100�����������ã�"), MB_ICONSTOP);
		return;
	}

	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);
	POSITION pos = pListCtrlRoom->GetFirstSelectedItemPosition();

	//û��ѡ���У��½�
	if (!pos)
	{
		CMD_C_RoomCtrl RoomCtrl;
		ZeroMemory(&RoomCtrl, sizeof(RoomCtrl));

		RoomCtrl.lRoomCtrlSysStorage = lRoomCtrlSysStorage;
		RoomCtrl.lRoomCtrlPlayerStorage = lRoomCtrlPlayerStorage;
		RoomCtrl.lRoomCtrlParameterK = lRoomCtrlParameterK;
		RoomCtrl.bCancelRoomCtrl = false;

		// ������Ϣ
		SendDebugMessage(SUB_C_ROOM_CTRL, &RoomCtrl, sizeof(RoomCtrl));
	}
	else
	{
		int nItem = pListCtrlRoom->GetNextSelectedItem(pos);

		CMD_C_RoomCtrl RoomCtrl;
		ZeroMemory(&RoomCtrl, sizeof(RoomCtrl));

		RoomCtrl.dwSelCtrlIndex = _ttoi64(pListCtrlRoom->GetItemText(nItem, 0));
		RoomCtrl.lRoomCtrlSysStorage = lRoomCtrlSysStorage;
		RoomCtrl.lRoomCtrlPlayerStorage = lRoomCtrlPlayerStorage;
		RoomCtrl.lRoomCtrlParameterK = lRoomCtrlParameterK;
		RoomCtrl.bCancelRoomCtrl = false;

		// ������Ϣ
		SendDebugMessage(SUB_C_ROOM_CTRL, &RoomCtrl, sizeof(RoomCtrl));
	}

	delete[] lpszRoomCtrlSysStorage;
	delete[] lpszRoomCtrlPlayerStorage;
	delete[] lpszRoomCtrlParameterK;
}

void CClientDebugItemSinkDlg::OnBnClickedBtnCancelRoomCtrl()
{
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);
	POSITION pos = pListCtrlRoom->GetFirstSelectedItemPosition();

	if (!pos)
	{
		AfxMessageBox(TEXT("��ѡ��ĳ�У�"), MB_ICONSTOP);

		return;
	}

	int nItem = pListCtrlRoom->GetNextSelectedItem(pos);

	CMD_C_RoomCtrl RoomCtrl;
	ZeroMemory(&RoomCtrl, sizeof(RoomCtrl));

	RoomCtrl.dwSelCtrlIndex = _ttoi64(pListCtrlRoom->GetItemText(nItem, 0));
	RoomCtrl.bCancelRoomCtrl = true;

	// ������Ϣ
	SendDebugMessage(SUB_C_ROOM_CTRL, &RoomCtrl, sizeof(RoomCtrl));
}

void CClientDebugItemSinkDlg::OnEnChangeEditCurSysstorage()
{
	UpdateSysCtrlDesc();
}

void CClientDebugItemSinkDlg::OnEnChangeEditCurPlayerstorage()
{
	UpdateSysCtrlDesc();
}

void CClientDebugItemSinkDlg::OnEnChangeEditCurParameterK()
{
	UpdateSysCtrlDesc();
}

void CClientDebugItemSinkDlg::UpdateSysCtrlDesc()
{
	CString strCurSysStorage;
	GetDlgItemText(IDC_EDIT_CUR_SYSSTORAGE, strCurSysStorage);
	strCurSysStorage.TrimLeft();
	strCurSysStorage.TrimRight();

	CString strCurPlayerStorage;
	GetDlgItemText(IDC_EDIT_CUR_PLAYERSTORAGE, strCurPlayerStorage);
	strCurPlayerStorage.TrimLeft();
	strCurPlayerStorage.TrimRight();

	CString strCurParameterK;
	GetDlgItemText(IDC_EDIT_CUR_PARAMETER_K, strCurParameterK);
	strCurParameterK.TrimLeft();
	strCurParameterK.TrimRight();

	if (!IsValidNum(strCurSysStorage) || !IsValidNum(strCurPlayerStorage) || !IsValidNum(strCurParameterK))
	{
		SetDlgItemText(IDC_EDIT_SYS_CTRLDESC, TEXT("�޷������н���!"));

		return;
	}

	SCORE lCurSysStorage = _ttoi64(strCurSysStorage);
	SCORE lCurPlayerStorage = _ttoi64(strCurPlayerStorage);
	SCORE lCurParameterK = _ttoi64(strCurParameterK);

	//С��0����0�����
	lCurSysStorage = (lCurSysStorage < 0 ? 0 : lCurSysStorage);
	lCurPlayerStorage = (lCurPlayerStorage < 0 ? 0 : lCurPlayerStorage);

	WORD wUserWinChance = INVALID_WORD;
	WORD wSysWinChance = INVALID_WORD;

	SCORE lDVal = abs(lCurSysStorage - lCurPlayerStorage);
	SCORE lMaxCtrlStorage = max(lCurSysStorage, lCurPlayerStorage);
	bool bSysWin = lCurSysStorage > lCurPlayerStorage;

	if (lMaxCtrlStorage == lDVal)
	{
		wUserWinChance = (lCurSysStorage > 0 && lCurPlayerStorage == 0) ? 0 : 100;
		wSysWinChance = (lCurSysStorage > 0 && lCurPlayerStorage == 0) ? 100 : 0;
	}
	else
	{
		//��������ж�
		if (lDVal <= lMaxCtrlStorage * lCurParameterK / 100)
		{
			wUserWinChance = bSysWin ? 50 : 20;
			wSysWinChance = bSysWin ? 20 : 50;
		}
		else if (lDVal > lMaxCtrlStorage * lCurParameterK / 100 && lDVal <= 1.5 * lMaxCtrlStorage * lCurParameterK / 100)
		{
			wUserWinChance = bSysWin ? 40 : 30;
			wSysWinChance = bSysWin ? 30 : 40;
		}
		else if (lDVal > 1.5 * lMaxCtrlStorage * lCurParameterK / 100 && lDVal <= 2 * lMaxCtrlStorage * lCurParameterK / 100)
		{
			wUserWinChance = bSysWin ? 30 : 40;
			wSysWinChance = bSysWin ? 40 : 30;
		}
		else if (lDVal > 2 * lMaxCtrlStorage * lCurParameterK / 100 && lDVal <= 3 * lMaxCtrlStorage * lCurParameterK / 100)
		{
			wUserWinChance = bSysWin ? 20 : 50;
			wSysWinChance = bSysWin ? 50 : 20;
		}
		else if (lDVal > 3 * lMaxCtrlStorage * lCurParameterK / 100)
		{
			wUserWinChance = bSysWin ? 0 : 100;
			wSysWinChance = bSysWin ? 100 : 0;
		}
	}

	CString strSysCtrlDesc;
	strSysCtrlDesc.Format(TEXT("%d%%"), wUserWinChance);
	SetDlgItemText(IDC_EDIT_SYS_CTRLDESC, strSysCtrlDesc);
}

void CClientDebugItemSinkDlg::OnEnChangeEditRoomctrlSysstorage()
{
	UpdateRoomCtrlDesc();
}

void CClientDebugItemSinkDlg::OnEnChangeEditRoomctrlPlayerstorage()
{
	UpdateRoomCtrlDesc();
}

void CClientDebugItemSinkDlg::OnEnChangeEditRoomctrlParameterK()
{
	UpdateRoomCtrlDesc();
}

void CClientDebugItemSinkDlg::UpdateRoomCtrlDesc()
{
	CString strCurRoomCtrlSysStorage;
	GetDlgItemText(IDC_EDIT_ROOMCTRL_SYSSTORAGE, strCurRoomCtrlSysStorage);
	strCurRoomCtrlSysStorage.TrimLeft();
	strCurRoomCtrlSysStorage.TrimRight();

	CString strCurRoomCtrlPlayerStorage;
	GetDlgItemText(IDC_EDIT_ROOMCTRL_PLAYERSTORAGE, strCurRoomCtrlPlayerStorage);
	strCurRoomCtrlPlayerStorage.TrimLeft();
	strCurRoomCtrlPlayerStorage.TrimRight();

	CString strCurRoomCtrlParameterK;
	GetDlgItemText(IDC_EDIT_ROOMCTRL_PARAMETER_K, strCurRoomCtrlParameterK);
	strCurRoomCtrlParameterK.TrimLeft();
	strCurRoomCtrlParameterK.TrimRight();

	if (!IsValidNum(strCurRoomCtrlSysStorage) || !IsValidNum(strCurRoomCtrlPlayerStorage) || !IsValidNum(strCurRoomCtrlParameterK))
	{
		SetDlgItemText(IDC_EDIT_ROOM_CTRLDESC, TEXT("�޷������н���!"));

		return;
	}

	SCORE lCurSysStorage = _ttoi64(strCurRoomCtrlSysStorage);
	SCORE lCurPlayerStorage = _ttoi64(strCurRoomCtrlPlayerStorage);
	SCORE lCurParameterK = _ttoi64(strCurRoomCtrlParameterK);

	//С��0����0�����
	lCurSysStorage = (lCurSysStorage < 0 ? 0 : lCurSysStorage);
	lCurPlayerStorage = (lCurPlayerStorage < 0 ? 0 : lCurPlayerStorage);

	WORD wUserWinChance = INVALID_WORD;
	WORD wSysWinChance = INVALID_WORD;

	SCORE lDVal = abs(lCurSysStorage - lCurPlayerStorage);
	SCORE lMaxCtrlStorage = max(lCurSysStorage, lCurPlayerStorage);
	bool bSysWin = lCurSysStorage > lCurPlayerStorage;

	if (lMaxCtrlStorage == lDVal)
	{
		wUserWinChance = (lCurSysStorage > 0 && lCurPlayerStorage == 0) ? 0 : 100;
		wSysWinChance = (lCurSysStorage > 0 && lCurPlayerStorage == 0) ? 100 : 0;
	}
	else
	{
		//��������ж�
		if (lDVal <= lMaxCtrlStorage * lCurParameterK / 100)
		{
			wUserWinChance = bSysWin ? 50 : 20;
			wSysWinChance = bSysWin ? 20 : 50;
		}
		else if (lDVal > lMaxCtrlStorage * lCurParameterK / 100 && lDVal <= 1.5 * lMaxCtrlStorage * lCurParameterK / 100)
		{
			wUserWinChance = bSysWin ? 40 : 30;
			wSysWinChance = bSysWin ? 30 : 40;
		}
		else if (lDVal > 1.5 * lMaxCtrlStorage * lCurParameterK / 100 && lDVal <= 2 * lMaxCtrlStorage * lCurParameterK / 100)
		{
			wUserWinChance = bSysWin ? 30 : 40;
			wSysWinChance = bSysWin ? 40 : 30;
		}
		else if (lDVal > 2 * lMaxCtrlStorage * lCurParameterK / 100 && lDVal <= 3 * lMaxCtrlStorage * lCurParameterK / 100)
		{
			wUserWinChance = bSysWin ? 20 : 50;
			wSysWinChance = bSysWin ? 50 : 20;
		}
		else if (lDVal > 3 * lMaxCtrlStorage * lCurParameterK / 100)
		{
			wUserWinChance = bSysWin ? 0 : 100;
			wSysWinChance = bSysWin ? 100 : 0;
		}
	}

	CString strRoomCtrlDesc;
	strRoomCtrlDesc.Format(TEXT("%d%%"), wUserWinChance);
	SetDlgItemText(IDC_EDIT_ROOM_CTRLDESC, strRoomCtrlDesc);
}