// CoinDozerConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientConfigDlg.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////


BYTE g_cbExecuteTimesArray[MAX_EXECUTE_TIMES] = { 1, 3, 5, 10, 15 };

// ���ԶԻ���
IMPLEMENT_DYNAMIC(CClientConfigDlg, CDialog)
BEGIN_MESSAGE_MAP(CClientConfigDlg, CDialog)
	ON_WM_DESTROY()
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTabOptisons)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

// ���캯��
CClientConfigDlg::CClientConfigDlg(CWnd *pParent /*=NULL*/) : CDialog(IDD_DIALOG_MAIN, pParent)
{
    // ���ñ���
    m_pParentWnd = NULL;
    m_pIClientDebugCallback = NULL;
}

// ��������
CClientConfigDlg::~CClientConfigDlg()
{
}

// ��������
void CClientConfigDlg::OnDestroy()
{
    // ���ٴ���
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        m_DialogCustom[nIndex]->DestroyWindow();
        delete m_DialogCustom[nIndex];
    }

    CDialog::OnDestroy();
}

// �ؼ���
VOID CClientConfigDlg::DoDataExchange(CDataExchange *pDX)
{
    __super::DoDataExchange(pDX);
}

// �ͷŽӿ�
void CClientConfigDlg::Release()
{
    delete this;
}

// ��������
bool CClientConfigDlg::Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback)
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
	pTabCtrl->InsertItem(1, _T("��־��¼"));
    pTabCtrl->SetCurSel(0);

    // ��ȡ��ҳ
    CRect RectWindow;
    CWnd *pWindowShow = GetDlgItem(IDC_STATIC_SHOW);
    pWindowShow->ShowWindow(SW_HIDE);
    pWindowShow->GetWindowRect(RectWindow);
    ScreenToClient(RectWindow);

    // ��������
    m_DialogCustom[CUSTOM_GENERAL] = new CDialogGeneral(RectWindow.Height(), this);
    m_DialogCustom[CUSTOM_GENERAL]->Create(IDD_CUSTOM_GENERAL, this);

	m_DialogCustom[CUSTOM_ADVANCED] = new CDialogAdvanced(RectWindow.Height(), this);
	m_DialogCustom[CUSTOM_ADVANCED]->Create(IDD_CUSTOM_ADVANCED, this);

    // ����λ��
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        CRect RectCustom;
        CSize SizeCustom;
        m_DialogCustom[nIndex]->GetClientRect(RectCustom);
        SizeCustom.SetSize(min(RectWindow.Width(), RectCustom.Width()), min(RectWindow.Height(), RectCustom.Height()));

		m_DialogCustom[nIndex]->SetWindowPos(NULL, RectWindow.left + RectWindow.Width() / 2 - SizeCustom.cx / 2, RectWindow.top, SizeCustom.cx, SizeCustom.cy, SWP_NOZORDER);
        m_DialogCustom[nIndex]->ShowWindow((nIndex == 0) ? SW_SHOW : SW_HIDE);
    }

    // �رմ���
    __super::ShowWindow(SW_HIDE);

    return true;

}

// ��ʾ����
bool CClientConfigDlg::ShowWindow(bool bShow)
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
bool CClientConfigDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
    // ��Ч�ӿ�
    if(m_pIClientDebugCallback == NULL)
    {
        return false;
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
bool CClientConfigDlg::SendDebugMessage(UINT nMessageID, void *pData, UINT nSize)
{
    if(m_pIClientDebugCallback != NULL)
    {
        // ������Ϣ
        m_pIClientDebugCallback->OnDebugInfo(nMessageID, 0, pData, nSize);
    }
    return true;
}

// ������Ϣ
bool CClientConfigDlg::SendDebugMessage(UINT nMessageID, WORD wTableID, void *pData, UINT nSize)
{
	if (m_pIClientDebugCallback != NULL)
	{
		// ������Ϣ
		m_pIClientDebugCallback->OnDebugInfo(nMessageID, wTableID, pData, nSize);
	}
	return true;
}

// �任ѡ��
void CClientConfigDlg::OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult)
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

		if (nCurSel == CUSTOM_ADVANCED)
		{
			//���ƿ����б�
			m_DialogCustom[nCurSel]->CleanCtrlList();

			SendDebugMessage(SUB_C_ADVANCED_REFRESH_ALLCTRLLIST);
		}
    }

    *pResult = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ���ԶԻ���
BEGIN_MESSAGE_MAP(CDialogGeneral, CDialogDexter)
	ON_BN_CLICKED(IDC_BUTTON_SETRULE, OnBnClickedButtonSetRule)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROOM, OnLvnItemchangedListRoom)
	ON_BN_CLICKED(IDC_BTN_SET_ROOMCTRL, OnBnClickedBtnSetRoomCtrl)
	ON_BN_CLICKED(IDC_BTN_CANCEL_AREACTRL, OnBnClickedBtnCancelAreaCtrl)
	ON_BN_CLICKED(IDC_BTN_SET_AREACTRL, OnBnClickedBtnSetAreaCtrl)
	ON_EN_CHANGE(IDC_EDIT_ROOMCTRL_SYSSTORAGE, OnEnChangeEditRoomctrlSysstorage)
	ON_EN_CHANGE(IDC_EDIT_ROOMCTRL_PLAYERSTORAGE, OnEnChangeEditRoomctrlPlayerstorage)
	ON_EN_CHANGE(IDC_EDIT_ROOMCTRL_PARAMETER_K, OnEnChangeEditRoomctrlParameterK)
	ON_BN_CLICKED(IDC_BTN_USER_BET_QUERY, OnBnClickedBtnUserBetQuery)
	ON_BN_CLICKED(IDC_BTN_USER_BET_ALL, OnBnClickedBtnUserBetAll)
	ON_EN_SETFOCUS(IDC_EDIT_USER_ID, OnEnSetfocusEditUserId)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// ���캯��
CDialogGeneral::CDialogGeneral(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_GENERAL, nShowMax, pParent)
{
	ZeroMemory(&m_CustomRule, sizeof(m_CustomRule));
	m_lCurSysStorage = 0;
	m_lCurPlayerStorage = 0;
	m_lCurParameterK = 0;
	m_nSysCtrlStorageDeadLine = TRUE;
	m_lStorageResetCount = 0;
	m_lSysCtrlDeadLineLimit = 0;
	m_lSysCtrlSysWin = 0;
	m_lSysCtrlPlayerWin = 0;

	m_cbWinArea = 0xFF;
	m_cbExecuteTimes = 0;

	ZeroMemory(&m_UserInfo, sizeof(m_UserInfo));
	m_lQueryGameID = -1;
	m_UserBetArray.RemoveAll();
}

// ��������
CDialogGeneral::~CDialogGeneral()
{
}

// �ؼ���
void CDialogGeneral::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_CONFIG_SYSSTORAGE, m_CustomRule.lConfigSysStorage);
	DDX_Text(pDX, IDC_EDIT_CONFIG_PLAYERSTORAGE, m_CustomRule.lConfigPlayerStorage);
	DDX_Text(pDX, IDC_EDIT_CONFIG_PARAMETER_K, m_CustomRule.lConfigParameterK);
	DDX_Check(pDX, IDC_CHECK_SYSCTRL_STORAGE_DEADLINE, m_nSysCtrlStorageDeadLine);

	DDX_Text(pDX, IDC_STATIC_CUR_SYSSTORAGE, m_lCurSysStorage);
	DDX_Text(pDX, IDC_STATIC_CUR_PLAYERSTORAGE, m_lCurPlayerStorage);
	DDX_Text(pDX, IDC_STATIC_CUR_PARAMETER_K, m_lCurParameterK);
	DDX_Text(pDX, IDC_STATIC_SYSCTRL_RESETCOUNT, m_lStorageResetCount);
	DDX_Text(pDX, IDC_STATIC_SYSCTRL_DEADLINELIMIT, m_lSysCtrlDeadLineLimit);
	DDX_Text(pDX, IDC_STATIC_SYSCTRL_SYSWIN, m_lSysCtrlSysWin);
	DDX_Text(pDX, IDC_STATIC_SYSCTRL_PLAYERWIN, m_lSysCtrlPlayerWin);

	DDX_Control(pDX, IDC_LIST_USER_BET, m_listUserBet);
}

void CDialogGeneral::OnBnClickedBtnCancelAreaCtrl()
{
	CString strAreaCtrlID;
	GetDlgItemText(IDC_STATIC_AREACTRL_ID, strAreaCtrlID);
	strAreaCtrlID.TrimLeft();
	strAreaCtrlID.TrimRight();

	if (!IsValidNum(strAreaCtrlID) || strAreaCtrlID == TEXT(""))
	{
		SetDlgItemText(IDC_STATIC_CONTROL_INFO, TEXT("�������IDΪ��Ч!"));

		return;
	}

	CMD_C_AdminReq adminReq;
	ZeroMemory(&adminReq, sizeof(adminReq));

	adminReq.cbReqType = RQ_RESET_DEBUG;

	DWORD dwCtrlIndex = _ttoi(strAreaCtrlID);
	CopyMemory(adminReq.cbExtendData, &dwCtrlIndex, sizeof(dwCtrlIndex));

	SendDebugMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
}

void CDialogGeneral::OnBnClickedBtnSetAreaCtrl()
{
	CMD_C_AdminReq adminReq;
	ZeroMemory(&adminReq, sizeof(adminReq));

	//����ϵͳ��ӮAREA_MAX + 2
	BYTE cbAreaIndex = 0;
	for (cbAreaIndex = 0; cbAreaIndex < AREA_MAX + 2; cbAreaIndex++)
	{
		if (((CButton *)GetDlgItem(IDC_RADIO_TU_ZI + cbAreaIndex))->GetCheck())
		{
			break;
		}
	}

	int nSelectTimes = (INT)((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel();

	if (cbAreaIndex >= 0 && cbAreaIndex < AREA_MAX + 2 && nSelectTimes >= 0 && nSelectTimes < MAX_EXECUTE_TIMES)
	{
		adminReq.cbReqType = RQ_SET_WIN_AREA;
		adminReq.cbExtendData[0] = (BYTE)cbAreaIndex;
		adminReq.cbExtendData[1] = g_cbExecuteTimesArray[nSelectTimes];
		m_cbWinArea = adminReq.cbExtendData[0];
		m_cbExecuteTimes = adminReq.cbExtendData[1];

		SendDebugMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
	}
	else
	{
		MessageBox(TEXT("��ѡ����Դ����Լ���������!"), TEXT("��ʾ"));
	}
}

bool CDialogGeneral::ReqResult(const void *pBuffer)
{
	const CMD_S_CommandResult *pResult = (CMD_S_CommandResult *)pBuffer;
	CString str;
	switch (pResult->cbAckType)
	{
	case ACK_SET_WIN_AREA:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			switch (pResult->cbExtendData[0] + 1)
			{
			case ID_TU_ZI:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_YAN_ZI:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_GE_ZI:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_KONG_QUE:
				str.Format(TEXT("ִ�е���,ʤ������:��ȸ,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_LAO_YING:
				str.Format(TEXT("ִ�е���,ʤ������:��ӥ,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_SHI_ZI:
				str.Format(TEXT("ִ�е���,ʤ������:ʨ��,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_XIONG_MAO:
				str.Format(TEXT("ִ�е���,ʤ������:��è,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_HOU_ZI:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_SHA_YU:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_FEI_QIN:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_ZOU_SHOU:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_GLOD_SHA:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case AREA_SYSLOST:
				str.Format(TEXT("ִ�е���,ʤ������:����ϵͳ��,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case AREA_SYSWIN:
				str.Format(TEXT("ִ�е���,ʤ������:����ϵͳӮ,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			default:
				break;
			}
		}
		else
		{
			str.Format(TEXT("ִ�е���ʧ��!"));
			m_cbExecuteTimes = 0;
			m_cbWinArea = 0xFF;
		}
		break;
	}
	case ACK_RESET_DEBUG:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("ȡ������!");
			for (int nIndex = 0; nIndex < AREA_MAX + 2; nIndex++)
			{
				((CButton *)GetDlgItem(IDC_RADIO_TU_ZI + nIndex))->SetCheck(0);
			}
			if (m_cbExecuteTimes > 0)
			{
				((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(-1);
			}
			m_cbWinArea = 0xFF;
			m_cbExecuteTimes = 0;

			SetDlgItemText(IDC_STATIC_AREACTRL_POS, TEXT(""));
			SetDlgItemText(IDC_STATIC_AREACTRL_ID, TEXT(""));
			SetDlgItemText(IDC_STATIC_AREACTRL_REMAINCOUNT, TEXT(""));
			SetDlgItemText(IDC_STATIC_AREACTRL_SYSWIN, TEXT(""));
			SetDlgItemText(IDC_STATIC_AREACTRL_PLAYERWIN, TEXT(""));
		}
		break;
	}
	case ACK_PRINT_SYN:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			switch (pResult->cbExtendData[0] + 1)
			{
			case ID_TU_ZI:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_YAN_ZI:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_GE_ZI:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_KONG_QUE:
				str.Format(TEXT("ִ�е���,ʤ������:��ȸ,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_LAO_YING:
				str.Format(TEXT("ִ�е���,ʤ������:��ӥ,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_SHI_ZI:
				str.Format(TEXT("ִ�е���,ʤ������:ʨ��,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_XIONG_MAO:
				str.Format(TEXT("ִ�е���,ʤ������:��è,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_HOU_ZI:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_SHA_YU:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_FEI_QIN:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_ZOU_SHOU:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case ID_GLOD_SHA:
				str.Format(TEXT("ִ�е���,ʤ������:����,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case AREA_SYSLOST:
				str.Format(TEXT("ִ�е���,ʤ������:����ϵͳ��,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			case AREA_SYSWIN:
				str.Format(TEXT("ִ�е���,ʤ������:����ϵͳӮ,ʣ�����:%d"), pResult->cbExtendData[1]);
				break;
			default:
				break;
			}
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

	GetDlgItem(IDC_STATIC_CONTROL_INFO)->SetWindowText(str);

	return true;
}

bool CDialogGeneral::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pBuffer, WORD wDataSize)
{
	switch (nMessageID)
	{
	case SUB_S_REFRESH_RULE_RESULT:
	{
		ASSERT(wDataSize == sizeof(CMD_S_RefreshRuleResult));
		if (wDataSize != sizeof(CMD_S_RefreshRuleResult))
		{
			return false;
		}

		CMD_S_RefreshRuleResult *pRefreshRuleResult = (CMD_S_RefreshRuleResult *)pBuffer;

		//��ֵλ�ñ任
		m_CustomRule.lConfigSysStorage = pRefreshRuleResult->lConfigSysStorage;
		m_CustomRule.lConfigPlayerStorage = pRefreshRuleResult->lConfigPlayerStorage;
		m_CustomRule.lConfigParameterK = pRefreshRuleResult->lConfigParameterK;

		m_lCurSysStorage = pRefreshRuleResult->lCurSysStorage;
		m_lCurPlayerStorage = pRefreshRuleResult->lCurPlayerStorage;
		m_lCurParameterK = pRefreshRuleResult->lCurParameterK;
		m_nSysCtrlStorageDeadLine = pRefreshRuleResult->nSysCtrlStorageDeadLine;
		m_lStorageResetCount = pRefreshRuleResult->lStorageResetCount;
		m_lSysCtrlDeadLineLimit = pRefreshRuleResult->lSysCtrlDeadLineLimit;
		m_lSysCtrlSysWin = pRefreshRuleResult->lSysCtrlSysWin;
		m_lSysCtrlPlayerWin = pRefreshRuleResult->lSysCtrlPlayerWin;

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

		m_CustomRule.lConfigSysStorage = pSetRuleResult->lConfigSysStorage;
		m_CustomRule.lConfigPlayerStorage = pSetRuleResult->lConfigPlayerStorage;
		m_CustomRule.lConfigParameterK = pSetRuleResult->lConfigParameterK;

		m_lCurSysStorage = m_CustomRule.lConfigSysStorage;
		m_lCurPlayerStorage = m_CustomRule.lConfigPlayerStorage;
		m_lCurParameterK = m_CustomRule.lConfigParameterK;
		m_nSysCtrlStorageDeadLine = pSetRuleResult->nSysCtrlStorageDeadLine;

		SetDlgItemText(IDC_STATIC_SETDESC, TEXT("�������óɹ�!"));

		UpdateData(FALSE);
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
	case SUB_S_AMDIN_COMMAND:
	{
		ASSERT(wDataSize == sizeof(CMD_S_CommandResult));
		if (wDataSize != sizeof(CMD_S_CommandResult))
		{
			return false;
		}
		return ReqResult(pBuffer);
	}
	case SUB_S_CUR_ROOMCTRL_INFO:
	{
		//У���С
		ASSERT(sizeof(CMD_S_CurRoomCtrlInfo) == wDataSize);
		if (sizeof(CMD_S_CurRoomCtrlInfo) != wDataSize)
		{
			return false;
		}

		// ������Ϣ
		CMD_S_CurRoomCtrlInfo *pCurRoomCtrlInfo = (CMD_S_CurRoomCtrlInfo *)pBuffer;

		SetDlgItemInt(IDC_STATIC_ROOMCTRL_ID, pCurRoomCtrlInfo->dwCtrlIndex);
		SetDlgItemInt(IDC_STATIC_ROOMCTRL_SYSSTORAGE, pCurRoomCtrlInfo->lRoomCtrlSysStorage);
		SetDlgItemInt(IDC_STATIC_ROOMCTRL_PLAYERSTORAGE, pCurRoomCtrlInfo->lRoomCtrlPlayerStorage);
		SetDlgItemInt(IDC_STATIC_ROOMCTRL_PARAMETER_K, pCurRoomCtrlInfo->lRoomCtrlParameterK);
		SetDlgItemInt(IDC_STATIC_ROOMCTRL_DEADLINELIMIT, pCurRoomCtrlInfo->lRoomCtrlDeadLineLimit);
		SetDlgItemInt(IDC_STATIC_ROOMCTRL_SYSWIN, pCurRoomCtrlInfo->lRoomCtrlSysWin);
		SetDlgItemInt(IDC_STATIC_ROOMCTRL_PLAYERWIN, pCurRoomCtrlInfo->lRoomCtrlPlayerWin);

		UpdateRoomCtrlDesc(pCurRoomCtrlInfo->lRoomCtrlSysStorage, pCurRoomCtrlInfo->lRoomCtrlPlayerStorage, pCurRoomCtrlInfo->lRoomCtrlParameterK);

		return true;
	}
	case SUB_S_CUR_AREACTRL_INFO:
	{
		//У���С
		ASSERT(sizeof(CMD_S_CurAreaCtrlInfo) == wDataSize);
		if (sizeof(CMD_S_CurAreaCtrlInfo) != wDataSize)
		{
			return false;
		}

		// ������Ϣ
		CMD_S_CurAreaCtrlInfo *pCurAreaCtrlInfo = (CMD_S_CurAreaCtrlInfo *)pBuffer;

		SetDlgItemText(IDC_STATIC_AREACTRL_POS, GetAreaCtrlPosDesc(pCurAreaCtrlInfo->cbCtrlPosIndex));
		SetDlgItemInt(IDC_STATIC_AREACTRL_ID, pCurAreaCtrlInfo->dwCtrlIndex);
		SetDlgItemInt(IDC_STATIC_AREACTRL_CONFIGCOUNT, pCurAreaCtrlInfo->cbConfigCtrlTimes);
		SetDlgItemInt(IDC_STATIC_AREACTRL_REMAINCOUNT, pCurAreaCtrlInfo->cbCurCtrlTimes);
		SetDlgItemInt(IDC_STATIC_AREACTRL_SYSWIN, pCurAreaCtrlInfo->lAreaCtrlSysWin);
		SetDlgItemInt(IDC_STATIC_AREACTRL_PLAYERWIN, pCurAreaCtrlInfo->lAreaCtrlPlayerWin);

		return true;
	}
	case SUB_GET_ROMM_ALL_USER_RESULT:
	{
		ASSERT(wDataSize == sizeof(CMD_CF_UserInfo));
		if (wDataSize != sizeof(CMD_CF_UserInfo))
		{
			return false;
		}

		CopyMemory(&m_UserInfo, pBuffer, sizeof(m_UserInfo));

		return true;
	}
	case SUB_S_SEND_USER_BET_INFO:
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_S_SendUserBetInfo));
		if (wDataSize != sizeof(CMD_S_SendUserBetInfo))
		{
			return false;
		}

		//�����û��б�
		SendDebugMessage(SUB_GET_ROMM_ALL_USER);

		//��Ϣ����
		CMD_S_SendUserBetInfo *pSendUserBetInfo = (CMD_S_SendUserBetInfo *)pBuffer;
		LONGLONG lUserJettonScore[GAME_PLAYER][AREA_MAX];

		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; wAreaIndex++)
			{
				lUserJettonScore[i][wAreaIndex] = (LONGLONG)pSendUserBetInfo->lUserJettonScore[i][wAreaIndex];
			}
		}

		CUserBetArray *pUserBetArray = &m_UserBetArray;
		pUserBetArray->RemoveAll();

		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			WORD nUserIndex = GetUserInfoIndex(i);
			if (nUserIndex == MAX_CHAIR)
			{
				continue;
			}

			if (m_UserInfo.bIsAndroid[nUserIndex])
			{
				continue;
			}

			LONGLONG lUserAllBet = 0;
			for (BYTE j = 0; j < AREA_MAX; j++)
			{
				lUserAllBet += lUserJettonScore[i][j];
			}
			if (0 == lUserAllBet)
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
			for (BYTE k = 0; k < AREA_MAX; k++)
			{
				UserBet.lUserBet[k] = lUserJettonScore[i][k];
			}

			//��������
			pUserBetArray->Add(UserBet);
		}

		//���¿ؼ�
		UpdateUserBet(false);

		return true;
	}
	default:
		break;
	}

	return false;
}

// ��ʼ������
BOOL CDialogGeneral::OnInitDialog()
{
	// ��ʼ������
	__super::OnInitDialog();

	//��������
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_SYSSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_PLAYERSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_PARAMETER_K))->LimitText(3);

	((CEdit *)GetDlgItem(IDC_EDIT_ROOMCTRL_SYSSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_ROOMCTRL_PLAYERSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_ROOMCTRL_PARAMETER_K))->LimitText(3);

	//�������
	((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
	((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
	((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));
	((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("10"));
	((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("15"));

	((CComboBox *)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(0);

	GetDlgItem(IDC_STATIC_CONTROL_INFO)->SetWindowText(TEXT(""));
	((CEdit *)GetDlgItem(IDC_EDIT_USER_ID))->LimitText(9);

	//�����ע
	CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_USER_BET);
	pListUserBet->SetExtendedStyle(pListUserBet->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	INT nColumnCount = 0;
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���"), LVCFMT_CENTER, 60);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���ID"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("������"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("����Ӯ"), LVCFMT_CENTER, 60);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���ӡ�6"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���ӡ�6"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���ӡ�8"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��ȸ��8"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��ӥ��12"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("ʨ�ӡ�12"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��è��8"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���ӡ�8"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("�����24"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���ݡ�2"), LVCFMT_CENTER, 70);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���ޡ�2"), LVCFMT_CENTER, 70);

	SetDlgItemText(IDC_EDIT_USER_ID, TEXT("���������ID"));
	GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
	GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

	//ˢ��
	SendDebugMessage(SUB_C_REFRESH_RULE);
	SendDebugMessage(SUB_C_REFRESH_CUR_ROOMCTRL_INFO);
	SendDebugMessage(SUB_C_REFRESH_CUR_AREACTRL_INFO);
	
	UpdateSysCtrlDesc();

	return TRUE;
}

bool CDialogGeneral::IsValidNum(CString strEdit)
{
	SCORE lScoreValue = _ttoi64(strEdit);
	CString strCmp;
	strCmp.Format(TEXT("%I64d"), lScoreValue);

	return strCmp == strEdit;
}

void CDialogGeneral::OnBnClickedButtonSetRule()
{
	UpdateData(TRUE);

	if (!(m_lCurParameterK > 0 && m_lCurParameterK <= 100))
	{
		SetDlgItemText(IDC_STATIC_SETDESC, TEXT("��ǰ����ϵ����������������!"));
		return;
	}

	CMD_C_SetRule SetRule;
	ZeroMemory(&SetRule, sizeof(SetRule));

	SetRule.lConfigSysStorage = m_CustomRule.lConfigSysStorage;
	SetRule.lConfigPlayerStorage = m_CustomRule.lConfigPlayerStorage;
	SetRule.lConfigParameterK = m_CustomRule.lConfigParameterK;
	SetRule.nSysCtrlStorageDeadLine = m_nSysCtrlStorageDeadLine;

	SendDebugMessage(SUB_C_SET_RULE, &SetRule, sizeof(SetRule));

	return;
}

void CDialogGeneral::OnLvnItemchangedListRoom(NMHDR *pNMHDR, LRESULT *pResult)
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

void CDialogGeneral::OnBnClickedBtnSetRoomCtrl()
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

	//�½�
	CMD_C_RoomCtrl RoomCtrl;
	ZeroMemory(&RoomCtrl, sizeof(RoomCtrl));

	RoomCtrl.lRoomCtrlSysStorage = lRoomCtrlSysStorage;
	RoomCtrl.lRoomCtrlPlayerStorage = lRoomCtrlPlayerStorage;
	RoomCtrl.lRoomCtrlParameterK = lRoomCtrlParameterK;
	RoomCtrl.bCancelRoomCtrl = false;
	RoomCtrl.nRoomCtrlStorageDeadLine = ((CButton*)GetDlgItem(IDC_CHECK_ROOMCTRL_STORAGE_DEADLINE))->GetCheck();

	// ������Ϣ
	SendDebugMessage(SUB_C_ROOM_CTRL, &RoomCtrl, sizeof(RoomCtrl));
	
	delete[] lpszRoomCtrlSysStorage;
	delete[] lpszRoomCtrlPlayerStorage;
	delete[] lpszRoomCtrlParameterK;
}

void CDialogGeneral::UpdateSysCtrlDesc()
{
	CString strCurSysStorage;
	GetDlgItemText(IDC_STATIC_CUR_SYSSTORAGE, strCurSysStorage);
	strCurSysStorage.TrimLeft();
	strCurSysStorage.TrimRight();

	CString strCurPlayerStorage;
	GetDlgItemText(IDC_STATIC_CUR_PLAYERSTORAGE, strCurPlayerStorage);
	strCurPlayerStorage.TrimLeft();
	strCurPlayerStorage.TrimRight();

	CString strCurParameterK;
	GetDlgItemText(IDC_STATIC_CUR_PARAMETER_K, strCurParameterK);
	strCurParameterK.TrimLeft();
	strCurParameterK.TrimRight();

	if (!IsValidNum(strCurSysStorage) || !IsValidNum(strCurPlayerStorage) || !IsValidNum(strCurParameterK))
	{
		SetDlgItemText(IDC_STATIC_SYS_CTRLDESC, TEXT("�޷������н���!"));

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
	SetDlgItemText(IDC_STATIC_SYS_CTRLDESC, strSysCtrlDesc);
}

void CDialogGeneral::OnEnChangeEditRoomctrlSysstorage()
{
	//UpdateRoomCtrlDesc();
}

void CDialogGeneral::OnEnChangeEditRoomctrlPlayerstorage()
{
	//UpdateRoomCtrlDesc();
}

void CDialogGeneral::OnEnChangeEditRoomctrlParameterK()
{
	//UpdateRoomCtrlDesc();
}

void CDialogGeneral::UpdateRoomCtrlDesc(SCORE lCurSysStorage, SCORE lCurPlayerStorage, SCORE lCurParameterK)
{
	//CString strCurRoomCtrlSysStorage;
	//GetDlgItemText(IDC_EDIT_ROOMCTRL_SYSSTORAGE, strCurRoomCtrlSysStorage);
	//strCurRoomCtrlSysStorage.TrimLeft();
	//strCurRoomCtrlSysStorage.TrimRight();

	//CString strCurRoomCtrlPlayerStorage;
	//GetDlgItemText(IDC_EDIT_ROOMCTRL_PLAYERSTORAGE, strCurRoomCtrlPlayerStorage);
	//strCurRoomCtrlPlayerStorage.TrimLeft();
	//strCurRoomCtrlPlayerStorage.TrimRight();

	//CString strCurRoomCtrlParameterK;
	//GetDlgItemText(IDC_EDIT_ROOMCTRL_PARAMETER_K, strCurRoomCtrlParameterK);
	//strCurRoomCtrlParameterK.TrimLeft();
	//strCurRoomCtrlParameterK.TrimRight();

	//if (!IsValidNum(strCurRoomCtrlSysStorage) || !IsValidNum(strCurRoomCtrlPlayerStorage) || !IsValidNum(strCurRoomCtrlParameterK))
	//{
	//	SetDlgItemText(IDC_STATIC_ROOM_CTRLDESC, TEXT("�޷������н���!"));

	//	return;
	//}

	//SCORE lCurSysStorage = _ttoi64(strCurRoomCtrlSysStorage);
	//SCORE lCurPlayerStorage = _ttoi64(strCurRoomCtrlPlayerStorage);
	//SCORE lCurParameterK = _ttoi64(strCurRoomCtrlParameterK);

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
	SetDlgItemText(IDC_STATIC_ROOM_CTRLDESC, strRoomCtrlDesc);
}

void CDialogGeneral::OnBnClickedBtnUserBetQuery()
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

void CDialogGeneral::OnBnClickedBtnUserBetAll()
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

void CDialogGeneral::OnEnSetfocusEditUserId()
{
	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
	return;
}

//������ע
void CDialogGeneral::UpdateUserBet(bool bReSet)
{
	//����б�
	m_listUserBet.DeleteAllItems();

	if (bReSet)
	{
		m_lQueryGameID = -1;

		SetDlgItemText(IDC_EDIT_USER_ID, TEXT("���������ID"));

		GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
		GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

		return;
	}

	if (0 == m_UserBetArray.GetCount())
	{
		return;
	}

	INT nItemIndex = 0;
	for (INT i = 0; i < m_UserBetArray.GetCount(); i++)
	{
		//У���û�ID
		if (-1 != m_lQueryGameID && m_UserBetArray[i].dwUserGameID != m_lQueryGameID)
		{
			continue;
		}

		//��������
		m_listUserBet.InsertItem(nItemIndex, TEXT(""));

		CString strInfo;
		strInfo.Format(TEXT("%d"), nItemIndex + 1);
		m_listUserBet.SetItemText(nItemIndex, 0, strInfo);

		strInfo.Format(TEXT("%d"), m_UserBetArray[i].dwUserGameID);
		m_listUserBet.SetItemText(nItemIndex, 1, strInfo);

		strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserStartScore);
		m_listUserBet.SetItemText(nItemIndex, 2, strInfo);

		strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserWinLost);
		m_listUserBet.SetItemText(nItemIndex, 3, strInfo);

		INT nSubItem = -1;
		for (BYTE j = 1; j < AREA_MAX + 1; j++)
		{
			switch (j)
			{
			case ID_TU_ZI:
			{
				nSubItem = 4;
				break;
			}
			case ID_YAN_ZI:
			{
				nSubItem = 5;
				break;
			}
			case ID_GE_ZI:
			{
				nSubItem = 6;
				break;
			}
			case ID_KONG_QUE:
			{
				nSubItem = 7;
				break;
			}
			case ID_LAO_YING:
			{
				nSubItem = 8;
				break;
			}
			case ID_SHI_ZI:
			{
				nSubItem = 9;
				break;
			}
			case ID_XIONG_MAO:
			{
				nSubItem = 10;
				break;
			}
			case ID_HOU_ZI:
			{
				nSubItem = 11;
				break;
			}
			case ID_SHA_YU:
			{
				nSubItem = 12;
				break;
			}
			case ID_FEI_QIN:
			{
				nSubItem = 13;
				break;
			}
			case ID_ZOU_SHOU:
			{
				nSubItem = 14;
				break;
			}
			case ID_GLOD_SHA:
			{
				nSubItem = 15;
				break;
			}
			default:
				break;
			}

			strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserBet[j]);
			m_listUserBet.SetItemText(nItemIndex, nSubItem, strInfo);
		}

		nItemIndex++;
	}

	return;
}

//ȡ���û���Ϣ
int CDialogGeneral::GetUserInfoIndex(WORD wChairID, WORD wTableID)
{
	for (INT i = 00; i < m_UserInfo.wUserCount; i++)
	{
		if (m_UserInfo.wTableID[i] == wTableID && m_UserInfo.wChairID[i] == wChairID)
		{
			return i;
		}
	}

	return MAX_CHAIR;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

HBRUSH CDialogGeneral::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

BEGIN_MESSAGE_MAP(CDialogAdvanced, CDialogDexter)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ROOMCTRL, OnBnClickedBtnCancelRoomCtrl)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// ���캯��
CDialogAdvanced::CDialogAdvanced(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_ADVANCED, nShowMax, pParent)
{
}

// ��������
CDialogAdvanced::~CDialogAdvanced()
{
}

// �ؼ���
void CDialogAdvanced::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);
}

// ��Ϣ����
bool CDialogAdvanced::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
	switch (nMessageID)
	{
	case SUB_S_REFRESH_ROOMCTRL:
	{
		ASSERT(nSize % sizeof(CMD_S_RoomCtrlResult) == 0);
		if (nSize % sizeof(CMD_S_RoomCtrlResult) != 0)
		{
			return false;
		}

		CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);

		if (nSize == 0)
		{
			pListCtrlRoom->DeleteAllItems();
		}
		else
		{
			//������Ϣ
			CMD_S_RoomCtrlResult *pRoomCtrlResult = (CMD_S_RoomCtrlResult *)pData;

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

				strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlDeadLineLimit);
				pListCtrlRoom->SetItemText(nItem, 4, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlSysWin);
				pListCtrlRoom->SetItemText(nItem, 5, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pRoomCtrlResult->lRoomCtrlPlayerWin);
				pListCtrlRoom->SetItemText(nItem, 6, strCtrlVal);

				strCtrlVal.Format(TEXT("%d-%d:%d-%d"), pRoomCtrlResult->tmConfigTime.GetMonth(), pRoomCtrlResult->tmConfigTime.GetDay(), pRoomCtrlResult->tmConfigTime.GetHour(), pRoomCtrlResult->tmConfigTime.GetMinute());
				pListCtrlRoom->SetItemText(nItem, 7, strCtrlVal);

				if (pRoomCtrlResult->roomCtrlStatus == QUEUE)
				{
					 pListCtrlRoom->SetItemText(nItem, 8, TEXT("����"));
				}
				else if (pRoomCtrlResult->roomCtrlStatus == PROGRESSING)
				{
					pListCtrlRoom->SetItemText(nItem, 8, TEXT("������"));
				}
				else if (pRoomCtrlResult->roomCtrlStatus == FINISH)
				{
					pListCtrlRoom->SetItemText(nItem, 8, TEXT("���"));
				}
				else if (pRoomCtrlResult->roomCtrlStatus == CANCEL)
				{
					pListCtrlRoom->SetItemText(nItem, 8, TEXT("ȡ��"));
				}
				else if (pRoomCtrlResult->roomCtrlStatus == EXECUTE_CANCEL)
				{
					pListCtrlRoom->SetItemText(nItem, 8, TEXT("��ִ��"));
				}

				if (pRoomCtrlResult->roomCtrlStatus == PROGRESSING)
				{
					pListCtrlRoom->SetItemState(nItem, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
				}
			}
		}

		return true;
	}
	case SUB_S_REFRESH_SYSCTRL:
	{
		ASSERT(nSize % sizeof(CMD_S_SysCtrlResult) == 0);
		if (nSize % sizeof(CMD_S_SysCtrlResult) != 0)
		{
			return false;
		}

		CListCtrl *pListSysStorage = (CListCtrl *)GetDlgItem(IDC_LIST_SYSSTORAGE);

		if (nSize == 0)
		{
			pListSysStorage->DeleteAllItems();
		}
		else
		{
			//������Ϣ
			CMD_S_SysCtrlResult *pSysCtrlResult = (CMD_S_SysCtrlResult *)pData;

			int nItemIndex = -1;
			for (WORD wIndex = 0; wIndex<pListSysStorage->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListSysStorage->GetItemText(wIndex, 0)) == pSysCtrlResult->dwCtrlIndex)
				{
					nItemIndex = wIndex;
					break;
				}
			}

			if (nItemIndex == -1)
			{
				CString strCtrlIndex;
				strCtrlIndex.Format(TEXT("%d"), pSysCtrlResult->dwCtrlIndex);

				int nItem = pListSysStorage->GetItemCount();
				nItemIndex = pListSysStorage->InsertItem(nItem, strCtrlIndex);

				CString strCtrlVal;
				strCtrlVal.Format(TEXT("%I64d"), pSysCtrlResult->lSysCtrlSysStorage);
				pListSysStorage->SetItemText(nItem, 1, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pSysCtrlResult->lSysCtrlPlayerStorage);
				pListSysStorage->SetItemText(nItem, 2, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pSysCtrlResult->lSysCtrlParameterK);
				pListSysStorage->SetItemText(nItem, 3, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pSysCtrlResult->lSysCtrlDeadLineLimit);
				pListSysStorage->SetItemText(nItem, 4, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pSysCtrlResult->lSysCtrlSysWin);
				pListSysStorage->SetItemText(nItem, 5, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pSysCtrlResult->lSysCtrlPlayerWin);
				pListSysStorage->SetItemText(nItem, 6, strCtrlVal);

				strCtrlVal.Format(TEXT("%d-%d:%d-%d"), pSysCtrlResult->tmResetTime.GetMonth(), pSysCtrlResult->tmResetTime.GetDay(), pSysCtrlResult->tmResetTime.GetHour(), pSysCtrlResult->tmResetTime.GetMinute());
				pListSysStorage->SetItemText(nItem, 7, strCtrlVal);

				if (pSysCtrlResult->sysCtrlStatus == PROGRESSINGEX)
				{
					pListSysStorage->SetItemState(nItem, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
				}
			}
		}

		return true;
	}
	case SUB_S_REFRESH_AREACTRL:
	{
		ASSERT(nSize % sizeof(CMD_S_AreaCtrlResult) == 0);
		if (nSize % sizeof(CMD_S_AreaCtrlResult) != 0)
		{
			return false;
		}

		CListCtrl *pListAreaCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_AREACTRL);

		if (nSize == 0)
		{
			pListAreaCtrl->DeleteAllItems();
		}
		else
		{
			//������Ϣ
			CMD_S_AreaCtrlResult *pAreaCtrlResult = (CMD_S_AreaCtrlResult *)pData;

			int nItemIndex = -1;
			for (WORD wIndex = 0; wIndex<pListAreaCtrl->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListAreaCtrl->GetItemText(wIndex, 0)) == pAreaCtrlResult->dwCtrlIndex)
				{
					nItemIndex = wIndex;
					break;
				}
			}

			if (nItemIndex == -1)
			{
				CString strCtrlIndex;
				strCtrlIndex.Format(TEXT("%d"), pAreaCtrlResult->dwCtrlIndex);

				int nItem = pListAreaCtrl->GetItemCount();
				nItemIndex = pListAreaCtrl->InsertItem(nItem, strCtrlIndex);

				pListAreaCtrl->SetItemText(nItem, 1, GetAreaCtrlPosDesc(pAreaCtrlResult->cbCtrlPosIndex));
				
				CString strCtrlVal;
				strCtrlVal.Format(TEXT("%d"), pAreaCtrlResult->cbCurCtrlTimes);
				pListAreaCtrl->SetItemText(nItem, 2, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pAreaCtrlResult->lAreaCtrlSysWin);
				pListAreaCtrl->SetItemText(nItem, 3, strCtrlVal);

				strCtrlVal.Format(TEXT("%I64d"), pAreaCtrlResult->lAreaCtrlPlayerWin);
				pListAreaCtrl->SetItemText(nItem, 4, strCtrlVal);

				strCtrlVal.Format(TEXT("%d-%d:%d-%d"), pAreaCtrlResult->tmConfigTime.GetMonth(), pAreaCtrlResult->tmConfigTime.GetDay(), pAreaCtrlResult->tmConfigTime.GetHour(), pAreaCtrlResult->tmConfigTime.GetMinute());
				pListAreaCtrl->SetItemText(nItem, 5, strCtrlVal);

				if (pAreaCtrlResult->areaCtrlStatus == VALID)
				{
					pListAreaCtrl->SetItemText(nItem, 6, TEXT("��Ч"));
				}
				else if (pAreaCtrlResult->areaCtrlStatus == INVALID)
				{
					pListAreaCtrl->SetItemText(nItem, 6, TEXT("��Ч"));
				}

				if (pAreaCtrlResult->areaCtrlStatus == VALID)
				{
					pListAreaCtrl->SetItemState(nItem, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
				}
			}
		}

		return true;
	}
	case SUB_S_LIST_WINLOST_INFO:
	{
		ASSERT(nSize % sizeof(CMD_S_ListWinLostInfo) == 0);
		if (nSize % sizeof(CMD_S_ListWinLostInfo) != 0)
		{
			return false;
		}

		//������Ϣ
		CMD_S_ListWinLostInfo *pListWinLostInfo = (CMD_S_ListWinLostInfo *)pData;

		SetDlgItemInt(IDC_STATIC_LIST_SYSSTORAGE_SYSWIN, pListWinLostInfo->lSysCtrlSysWin);
		SetDlgItemInt(IDC_STATIC_LIST_SYSSTORAGE_PLAYERWIN, pListWinLostInfo->lSysCtrlPlayerWin);
		SetDlgItemInt(IDC_STATIC_LIST_ROOM_SYSWIN, pListWinLostInfo->lRoomCtrlSysWin);
		SetDlgItemInt(IDC_STATIC_LIST_ROOM_PLAYERWIN, pListWinLostInfo->lRoomCtrlPlayerWin);
		SetDlgItemInt(IDC_STATIC_LIST_AREACTRL_SYSWIN, pListWinLostInfo->lAreaCtrlSysWin);
		SetDlgItemInt(IDC_STATIC_LIST_AREACTRL_PLAYERWIN, pListWinLostInfo->lAreaCtrlPlayerWin);

		SCORE lTotalSysWin = (pListWinLostInfo->lSysCtrlSysWin - pListWinLostInfo->lSysCtrlPlayerWin) + (pListWinLostInfo->lRoomCtrlSysWin - pListWinLostInfo->lRoomCtrlPlayerWin)
			+ (pListWinLostInfo->lAreaCtrlSysWin - pListWinLostInfo->lAreaCtrlPlayerWin);
		SetDlgItemInt(IDC_STATIC_TOTAL_SYSWIN, lTotalSysWin);
		SetDlgItemInt(IDC_STATICT_TOTAL_PLAYERWIN, -lTotalSysWin);

		CString strStartRecord;
		strStartRecord.Format(TEXT("%d/%2d/%2d-%2d/%2d/%2d"), pListWinLostInfo->tmStartRecord.GetYear(), pListWinLostInfo->tmStartRecord.GetMonth(), pListWinLostInfo->tmStartRecord.GetDay(),
			pListWinLostInfo->tmStartRecord.GetHour(), pListWinLostInfo->tmStartRecord.GetMinute(), pListWinLostInfo->tmStartRecord.GetSecond());
		SetDlgItemText(IDC_STATIC_BEGIN_RECORDTIME, strStartRecord);

		return true;
	}

	default:
		break;
	}

	return false;
}

// ��ʼ������
BOOL CDialogAdvanced::OnInitDialog()
{
	// ��ʼ������
	__super::OnInitDialog();

	INT nColumnCount = 0;

	//�������
	CListCtrl *pListSysStorage = (CListCtrl *)GetDlgItem(IDC_LIST_SYSSTORAGE);
	pListSysStorage->SetExtendedStyle(pListSysStorage->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListSysStorage->InsertColumn(nColumnCount++, TEXT("���ID"), LVCFMT_CENTER, 60);
	pListSysStorage->InsertColumn(nColumnCount++, TEXT("ϵͳ���"), LVCFMT_CENTER, 70);
	pListSysStorage->InsertColumn(nColumnCount++, TEXT("��ҿ��"), LVCFMT_CENTER, 70);
	pListSysStorage->InsertColumn(nColumnCount++, TEXT("����ϵ��"), LVCFMT_CENTER, 60);
	pListSysStorage->InsertColumn(nColumnCount++, TEXT("��ֵֹ"), LVCFMT_CENTER, 70);
	pListSysStorage->InsertColumn(nColumnCount++, TEXT("ϵͳ��Ӯ"), LVCFMT_CENTER, 80);
	pListSysStorage->InsertColumn(nColumnCount++, TEXT("�����Ӯ"), LVCFMT_CENTER, 80);
	pListSysStorage->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 60);

	//�������
	CListCtrl *pListRoomCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);
	pListRoomCtrl->SetExtendedStyle(pListRoomCtrl->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListRoomCtrl->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
	pListRoomCtrl->InsertColumn(nColumnCount++, TEXT("ϵͳ���"), LVCFMT_CENTER, 70);
	pListRoomCtrl->InsertColumn(nColumnCount++, TEXT("��ҿ��"), LVCFMT_CENTER, 70);
	pListRoomCtrl->InsertColumn(nColumnCount++, TEXT("����ϵ��"), LVCFMT_CENTER, 60);
	pListRoomCtrl->InsertColumn(nColumnCount++, TEXT("��ֵֹ"), LVCFMT_CENTER, 70);
	pListRoomCtrl->InsertColumn(nColumnCount++, TEXT("ϵͳ��Ӯ"), LVCFMT_CENTER, 70);
	pListRoomCtrl->InsertColumn(nColumnCount++, TEXT("�����Ӯ"), LVCFMT_CENTER, 70);
	pListRoomCtrl->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 60);
	pListRoomCtrl->InsertColumn(nColumnCount++, TEXT("��ǰ״̬"), LVCFMT_CENTER, 60);

	//�������
	CListCtrl *pListAreaCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_AREACTRL);
	pListAreaCtrl->SetExtendedStyle(pListAreaCtrl->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListAreaCtrl->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
	pListAreaCtrl->InsertColumn(nColumnCount++, TEXT("����λ��"), LVCFMT_CENTER, 70);
	pListAreaCtrl->InsertColumn(nColumnCount++, TEXT("��ǰ����"), LVCFMT_CENTER, 70);
	pListAreaCtrl->InsertColumn(nColumnCount++, TEXT("ϵͳ��Ӯ"), LVCFMT_CENTER, 80);
	pListAreaCtrl->InsertColumn(nColumnCount++, TEXT("�����Ӯ"), LVCFMT_CENTER, 80);
	pListAreaCtrl->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 70);

	return TRUE;
}

void CDialogAdvanced::OnBnClickedBtnCancelRoomCtrl()
{
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);
	POSITION pos = pListCtrlRoom->GetFirstSelectedItemPosition();

	if (!pos)
	{
		AfxMessageBox(TEXT("��ѡ��ĳ�У�"), MB_ICONSTOP);

		return;
	}

	int nItem = pListCtrlRoom->GetNextSelectedItem(pos);

	if (pListCtrlRoom->GetItemText(nItem, 8) == TEXT("ȡ��"))
	{
		AfxMessageBox(TEXT("������¼��ȡ����"), MB_ICONSTOP);

		return;
	}

	if (pListCtrlRoom->GetItemText(nItem, 8) == TEXT("���"))
	{
		AfxMessageBox(TEXT("������¼����ɣ�"), MB_ICONSTOP);

		return;
	}

	CMD_C_RoomCtrl RoomCtrl;
	ZeroMemory(&RoomCtrl, sizeof(RoomCtrl));

	RoomCtrl.dwSelCtrlIndex = _ttoi64(pListCtrlRoom->GetItemText(nItem, 0));
	RoomCtrl.bCancelRoomCtrl = true;

	// ������Ϣ
	SendDebugMessage(SUB_C_ROOM_CTRL, &RoomCtrl, sizeof(RoomCtrl));

	pListCtrlRoom->DeleteAllItems();
}

void CDialogAdvanced::CleanCtrlList()
{
	CListCtrl *pListSysStorage = (CListCtrl *)GetDlgItem(IDC_LIST_SYSSTORAGE);
	pListSysStorage->DeleteAllItems();

	CListCtrl *pListRoomCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);
	pListRoomCtrl->DeleteAllItems();

	CListCtrl *pListAreaCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_AREACTRL);
	pListAreaCtrl->DeleteAllItems();
}

HBRUSH CDialogAdvanced::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC1 || pWnd->GetDlgCtrlID() == IDC_STATIC_LIST_SYSSTORAGE_SYSWIN
		|| pWnd->GetDlgCtrlID() == IDC_STATIC3 || pWnd->GetDlgCtrlID() == IDC_STATIC_LIST_ROOM_SYSWIN
		|| pWnd->GetDlgCtrlID() == IDC_STATIC5 || pWnd->GetDlgCtrlID() == IDC_STATIC_LIST_AREACTRL_SYSWIN)
	{
		pDC->SetTextColor(RGB(0, 0, 255));
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC2 || pWnd->GetDlgCtrlID() == IDC_STATIC_LIST_SYSSTORAGE_PLAYERWIN
		|| pWnd->GetDlgCtrlID() == IDC_STATIC4 || pWnd->GetDlgCtrlID() == IDC_STATIC_LIST_ROOM_PLAYERWIN
		|| pWnd->GetDlgCtrlID() == IDC_STATIC6 || pWnd->GetDlgCtrlID() == IDC_STATIC_LIST_AREACTRL_PLAYERWIN)
	{
		pDC->SetTextColor(RGB(42, 168, 42));
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC7 || pWnd->GetDlgCtrlID() == IDC_STATIC_TOTAL_SYSWIN)
	{
		pDC->SetTextColor(RGB(253, 128, 253));
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC8 || pWnd->GetDlgCtrlID() == IDC_STATICT_TOTAL_PLAYERWIN)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}

	return hbr;
}