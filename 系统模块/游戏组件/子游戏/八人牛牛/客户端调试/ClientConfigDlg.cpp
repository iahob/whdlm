// CoinDozerConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientConfigDlg.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////
BYTE g_cbAreaExecuteTimesArray[MAX_EXECUTE_TIMES] = { 1, 3, 5, 10, 15 };

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
	pTabCtrl->InsertItem(1, _T("��������"));
	pTabCtrl->InsertItem(2, _T("��־��¼"));
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

	m_DialogCustom[CUSTOM_TRACELOG] = new CDialogTraceLog(RectWindow.Height(), this);
	m_DialogCustom[CUSTOM_TRACELOG]->Create(IDD_CUSTOM_TRACELOG, this);

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

		if (nCurSel == CUSTOM_TRACELOG)
		{
			//���ƿ����б�
			m_DialogCustom[nCurSel]->CleanCtrlList();

			SendDebugMessage(SUB_C_REFRESH_TRACELOG);
		}
    }

    *pResult = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ���ԶԻ���
BEGIN_MESSAGE_MAP(CDialogGeneral, CDialogDexter)
	ON_BN_CLICKED(IDC_BUTTON_ANCHOU, &CDialogGeneral::OnBnClickedButtonAnchou)
	ON_BN_CLICKED(IDC_BUTTON_ROOMCTRL, &CDialogGeneral::OnBnClickedButtonRoomctrl)
	ON_BN_CLICKED(IDC_BUTTON_SYSCTRL, &CDialogGeneral::OnBnClickedButtonSysctrl)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_ROOMCTRL, &CDialogGeneral::OnBnClickedButtonCancelRoomctrl)
END_MESSAGE_MAP()

// ���캯��
CDialogGeneral::CDialogGeneral(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_GENERAL, nShowMax, pParent)
{
}

// ��������
CDialogGeneral::~CDialogGeneral()
{
}

// ��ʼ������
BOOL CDialogGeneral::OnInitDialog()
{
	// ��ʼ������
	__super::OnInitDialog();

	//��������
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_ANCHOU_RATE))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_SYSCTRL_SYSSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_SYSCTRL_PLAYERSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_SYSCTRL_PARAMETER_K))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_SYSCTRL_SYSSTORAGE_RESETRATE))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_ROOMCTRL_SYSSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_ROOMCTRL_PLAYERSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_ROOMCTRL_PARAMETER_K))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_ROOMCTRL_SYSSTORAGE_RESETRATE))->LimitText(3);

	//��ǰ��������б�
	CListCtrl *pListCurRoomCtrlInfo = (CListCtrl *)GetDlgItem(IDC_LIST_CUR_ROOMCTRL_INFO);
	pListCurRoomCtrlInfo->SetExtendedStyle(pListCurRoomCtrlInfo->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	INT nColumnCount = 0;
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("����ϵͳ���"), LVCFMT_CENTER, 100);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("������ҿ��"), LVCFMT_CENTER, 100);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("��ǰϵͳ���"), LVCFMT_CENTER, 100);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("��ǰ��ҿ��"), LVCFMT_CENTER, 100);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("����ϵ��"), LVCFMT_CENTER, 60);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("�н���"), LVCFMT_CENTER, 60);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("���ͳ��"), LVCFMT_CENTER, 100);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("��ǰ״̬"), LVCFMT_CENTER, 60);

	//ˢ��
	SendDebugMessage(SUB_C_REFRESH_STATISTIC_SERVICE);
	SendDebugMessage(SUB_C_REFRESH_ANCHOU);
	SendDebugMessage(SUB_C_REFRESH_CUR_SYSCTRL_INFO);
	SendDebugMessage(SUB_C_REFRESH_CUR_ROOMCTRL_INFO);
	SendDebugMessage(SUB_C_REFRESH_LIST_CUR_ROOMCTRL_INFO);
	
	return TRUE;
}

// �ؼ���
void CDialogGeneral::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);
}

bool CDialogGeneral::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pBuffer, WORD wDataSize)
{
	switch (nMessageID)
	{
	case SUB_S_STATISTIC_SERVICE:
	{
		ASSERT(wDataSize == sizeof(CMD_S_StatisticService));
		if (wDataSize != sizeof(CMD_S_StatisticService))
		{
			return false;
		}

		CMD_S_StatisticService *pStatisticService = (CMD_S_StatisticService *)pBuffer;

		CString strStatisticSysWin;
		strStatisticSysWin.Format(SCORE_STRING, pStatisticService->lStatisticSysWin);
		SetDlgItemText(IDC_EDIT_STATISTIC_SYSWIN, strStatisticSysWin);

		CString strStatisticService;
		strStatisticService.Format(SCORE_STRING, pStatisticService->lStatisticService);
		SetDlgItemText(IDC_EDIT_STATISTIC_SERVICE, strStatisticService);

		return true;
	}
	case SUB_S_ANCHOU:
	{
		ASSERT(wDataSize == sizeof(CMD_S_AnChou));
		if (wDataSize != sizeof(CMD_S_AnChou))
		{
			return false;
		}

		CMD_S_AnChou *pAnChou = (CMD_S_AnChou *)pBuffer;

		SetDlgItemInt(IDC_STATIC_CUR_ANCHOU_RATE, pAnChou->lCurAnChouRate);

		CString strAnChouTotal;
		strAnChouTotal.Format(SCORE_STRING, pAnChou->lCurAnChouTotal);
		SetDlgItemText(IDC_STATIC_CUR_ANCHOU_TOTAL, strAnChouTotal);

		return true;
	}
	case SUB_S_CUR_SYSCTRL_INFO:
	{
		ASSERT(wDataSize == sizeof(CMD_S_CurSysCtrlInfo));
		if (wDataSize != sizeof(CMD_S_CurSysCtrlInfo))
		{
			return false;
		}

		CMD_S_CurSysCtrlInfo *pCurSysCtrlInfo = (CMD_S_CurSysCtrlInfo *)pBuffer;

		CString strSysCtrlCurSysStorage;
		strSysCtrlCurSysStorage.Format(SCORE_STRING, pCurSysCtrlInfo->lSysCtrlCurSysStorage);
		SetDlgItemText(IDC_STATIC_CUR_SYSCTRL_SYSSTORAGE, strSysCtrlCurSysStorage);

		CString strSysCtrlCurPlayerStorage;
		strSysCtrlCurPlayerStorage.Format(SCORE_STRING, pCurSysCtrlInfo->lSysCtrlCurPlayerStorage);
		SetDlgItemText(IDC_STATIC_CUR_SYSCTRL_PLAYERSTORAGE, strSysCtrlCurPlayerStorage);

		CString strParameterK;
		strParameterK.Format(TEXT("%I64d %%"), pCurSysCtrlInfo->lSysCtrlParameterK);
		SetDlgItemText(IDC_STATIC_CUR_SYSCTRL_PARAMETER_K, strParameterK);

		CString strResetRate;
		strResetRate.Format(TEXT("%I64d %%"), pCurSysCtrlInfo->lSysCtrlSysStorResetRate);
		SetDlgItemText(IDC_STATIC_CUR_SYSCTRL_SYSSTORAGE_RESETRATE, strResetRate);

		CString strWinRate;
		strWinRate.Format(TEXT("%d %%"), GetWinRate(pCurSysCtrlInfo->lSysCtrlCurSysStorage, pCurSysCtrlInfo->lSysCtrlCurPlayerStorage, pCurSysCtrlInfo->lSysCtrlParameterK));
		SetDlgItemText(IDC_STATIC_CUR_SYSCTRL_WINRATE, strWinRate);

		SetDlgItemInt(IDC_STATIC_CUR_SYSCTRL_RESETCOUNT, pCurSysCtrlInfo->lSysCtrlResetCount);
	
		CString strSysCtrlSysWin;
		strSysCtrlSysWin.Format(SCORE_STRING, pCurSysCtrlInfo->lSysCtrlSysWin);
		SetDlgItemText(IDC_STATIC_CUR_SYSCTRL_SYSWIN, strSysCtrlSysWin);

		return true;
	}
	case SUB_S_CUR_ROOMCTRL_INFO:
	{
		ASSERT(wDataSize % sizeof(CMD_S_CurRoomCtrlInfo) == 0);
		if (wDataSize % sizeof(CMD_S_CurRoomCtrlInfo) != 0)
		{
			return false;
		}

		if (wDataSize == 0)
		{
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_SYSSTORAGE, TEXT(""));
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_PLAYERSTORAGE, TEXT(""));
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_PARAMETER_K, TEXT(""));
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_SYSSTORAGE_RESETRATE, TEXT(""));
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_WINRATE, TEXT(""));
		}
		else
		{
			CMD_S_CurRoomCtrlInfo *pCurRoomCtrlInfo = (CMD_S_CurRoomCtrlInfo *)pBuffer;

			CString strRoomCtrlCurSysStorage;
			strRoomCtrlCurSysStorage.Format(SCORE_STRING, pCurRoomCtrlInfo->lRoomCtrlCurSysStorage);
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_SYSSTORAGE, strRoomCtrlCurSysStorage);

			CString strRoomCtrlCurPlayerStorage;
			strRoomCtrlCurPlayerStorage.Format(SCORE_STRING, pCurRoomCtrlInfo->lRoomCtrlCurPlayerStorage);
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_PLAYERSTORAGE, strRoomCtrlCurPlayerStorage);

			CString strParameterK;
			strParameterK.Format(TEXT("%I64d %%"), pCurRoomCtrlInfo->lRoomCtrlParameterK);
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_PARAMETER_K, strParameterK);

			CString strResetRate;
			strResetRate.Format(TEXT("%I64d %%"), pCurRoomCtrlInfo->lRoomCtrlSysStorResetRate);
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_SYSSTORAGE_RESETRATE, strResetRate);

			CString strWinRate;
			strWinRate.Format(TEXT("%d %%"), GetWinRate(pCurRoomCtrlInfo->lRoomCtrlCurSysStorage, pCurRoomCtrlInfo->lRoomCtrlCurPlayerStorage, pCurRoomCtrlInfo->lRoomCtrlParameterK));
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_WINRATE, strWinRate);

			CString strRoomCtrlSysWin;
			strRoomCtrlSysWin.Format(SCORE_STRING, pCurRoomCtrlInfo->lRoomCtrlSysWin);
			SetDlgItemText(IDC_STATIC_CUR_ROOMCTRL_SYSWIN, strRoomCtrlSysWin);
		}

		return true;
	}
	case SUB_S_LIST_CUR_ROOMCTRL_INFO:
	{
		ASSERT(wDataSize % sizeof(CMD_S_ListCurRoomCtrlInfo) == 0);
		if (wDataSize % sizeof(CMD_S_ListCurRoomCtrlInfo) != 0)
		{
			return false;
		}

		CListCtrl *pListCurRoomCtrlInfo = (CListCtrl *)GetDlgItem(IDC_LIST_CUR_ROOMCTRL_INFO);

		if (wDataSize == 0)
		{
			pListCurRoomCtrlInfo->DeleteAllItems();
		}
		else
		{
			//������Ϣ
			CMD_S_ListCurRoomCtrlInfo *pCurRoomCtrlInfo = (CMD_S_ListCurRoomCtrlInfo *)pBuffer;

			int nItemIndex = -1;
			for (WORD wIndex = 0; wIndex<pListCurRoomCtrlInfo->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListCurRoomCtrlInfo->GetItemText(wIndex, 0)) == pCurRoomCtrlInfo->curRoomCtrlListInfo.dwCtrlIndex)
				{
					nItemIndex = wIndex;
					break;
				}
			}

			if (nItemIndex == -1)
			{
				CString strCtrlIndex;
				strCtrlIndex.Format(TEXT("%d"), pCurRoomCtrlInfo->curRoomCtrlListInfo.dwCtrlIndex);

				int nItem = pListCurRoomCtrlInfo->GetItemCount();
				nItemIndex = pListCurRoomCtrlInfo->InsertItem(nItem, strCtrlIndex);

				CString strCtrlVal;
				strCtrlVal.Format(SCORE_STRING, pCurRoomCtrlInfo->curRoomCtrlListInfo.lRoomCtrlConfigSysStorage);
				pListCurRoomCtrlInfo->SetItemText(nItem, 1, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pCurRoomCtrlInfo->curRoomCtrlListInfo.lRoomCtrlConfigPlayerStorage);
				pListCurRoomCtrlInfo->SetItemText(nItem, 2, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pCurRoomCtrlInfo->curRoomCtrlListInfo.lRoomCtrlCurSysStorage);
				pListCurRoomCtrlInfo->SetItemText(nItem, 3, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pCurRoomCtrlInfo->curRoomCtrlListInfo.lRoomCtrlCurPlayerStorage);
				pListCurRoomCtrlInfo->SetItemText(nItem, 4, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pCurRoomCtrlInfo->curRoomCtrlListInfo.lRoomCtrlParameterK);
				pListCurRoomCtrlInfo->SetItemText(nItem, 5, strCtrlVal);

				WORD wWinRate = GetWinRate(pCurRoomCtrlInfo->curRoomCtrlListInfo.lRoomCtrlCurSysStorage, pCurRoomCtrlInfo->curRoomCtrlListInfo.lRoomCtrlCurPlayerStorage, pCurRoomCtrlInfo->curRoomCtrlListInfo.lRoomCtrlParameterK);
				strCtrlVal.Format(TEXT("%d"), wWinRate);
				pListCurRoomCtrlInfo->SetItemText(nItem, 6, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pCurRoomCtrlInfo->curRoomCtrlListInfo.lRoomCtrlSysWin);
				pListCurRoomCtrlInfo->SetItemText(nItem, 7, strCtrlVal);

				if ( pCurRoomCtrlInfo->curRoomCtrlListInfo.roomCtrlStatus == QUEUE)
				{
					pListCurRoomCtrlInfo->SetItemText(nItem, 8, TEXT("����"));
				}
				else if ( pCurRoomCtrlInfo->curRoomCtrlListInfo.roomCtrlStatus == PROGRESSING)
				{
					pListCurRoomCtrlInfo->SetItemText(nItem, 8, TEXT("������"));
				}
				else if ( pCurRoomCtrlInfo->curRoomCtrlListInfo.roomCtrlStatus == FINISH)
				{
					pListCurRoomCtrlInfo->SetItemText(nItem, 8, TEXT("���"));
				}
				else if ( pCurRoomCtrlInfo->curRoomCtrlListInfo.roomCtrlStatus == CANCEL)
				{
					pListCurRoomCtrlInfo->SetItemText(nItem, 8, TEXT("ȡ��"));
				}
				else if ( pCurRoomCtrlInfo->curRoomCtrlListInfo.roomCtrlStatus == EXECUTE_CANCEL)
				{
					pListCurRoomCtrlInfo->SetItemText(nItem, 8, TEXT("��ִ��"));
				}

				if ( pCurRoomCtrlInfo->curRoomCtrlListInfo.roomCtrlStatus == PROGRESSING)
				{
					pListCurRoomCtrlInfo->SetItemState(nItem, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
				}
			}
		}

		return true;
	}
	case  SUB_S_DEBUGTEXT_GENERAL:
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
	default:
		break;
	}

	return false;
}

//��ȡ�н���
WORD CDialogGeneral::GetWinRate(SCORE lSysStorage, SCORE lPlayerStorage, SCORE lParameterK)
{
	//С��0����0�����
	lSysStorage = (lSysStorage < 0 ? 0 : lSysStorage);
	lPlayerStorage = (lPlayerStorage < 0 ? 0 : lPlayerStorage);

	WORD wUserWinRate = INVALID_WORD;

	SCORE lDVal = abs(lSysStorage - lPlayerStorage);
	SCORE lMaxCtrlStorage = max(lSysStorage, lPlayerStorage);
	bool bSysWin = lSysStorage > lPlayerStorage;

	if (lMaxCtrlStorage == lDVal)
	{
		wUserWinRate = (lSysStorage > 0 && lPlayerStorage == 0) ? 0 : 70;
	}
	else
	{
		//��������ж�
		if (lDVal <= lMaxCtrlStorage * lParameterK / 100)
		{
			wUserWinRate = bSysWin ? 15 : 20;

		}
		else if (lDVal > lMaxCtrlStorage * lParameterK / 100 && lDVal <= 1.5 * lMaxCtrlStorage * lParameterK / 100)
		{
			wUserWinRate = bSysWin ? 12 : 30;

		}
		else if (lDVal > 1.5 * lMaxCtrlStorage * lParameterK / 100 && lDVal <= 2 * lMaxCtrlStorage * lParameterK / 100)
		{
			wUserWinRate = bSysWin ? 9 : 40;

		}
		else if (lDVal > 2 * lMaxCtrlStorage * lParameterK / 100 && lDVal <= 3 * lMaxCtrlStorage * lParameterK / 100)
		{
			wUserWinRate = bSysWin ? 7 : 50;

		}
		else if (lDVal > 3 * lMaxCtrlStorage * lParameterK / 100)
		{
			wUserWinRate = bSysWin ? 0 : 70;
		}
	}

	return wUserWinRate;
}

void CDialogGeneral::OnBnClickedButtonAnchou()
{
	CString strConfigAnChouRate;
	GetDlgItemText(IDC_EDIT_CONFIG_ANCHOU_RATE, strConfigAnChouRate);

	//�п�У��
	if (strConfigAnChouRate == TEXT(""))
	{
		AfxMessageBox(TEXT("��ˮ��������Ϊ�գ����������ã�"), MB_ICONSTOP);
		return;
	}

	//���ð���
	CMD_C_AnChou AnChou;
	ZeroMemory(&AnChou, sizeof(AnChou));
	
	AnChou.lConfigAnChouRate = GetDlgItemInt(IDC_EDIT_CONFIG_ANCHOU_RATE);
	
	//У��
	if (!(AnChou.lConfigAnChouRate >= 0 && AnChou.lConfigAnChouRate <= 30))
	{
		AfxMessageBox(TEXT("��ˮ������Χ0~30�����������ã�"), MB_ICONSTOP);
		return;
	}

	// ������Ϣ
	SendDebugMessage(SUB_C_ANCHOU, &AnChou, sizeof(AnChou));
}

void CDialogGeneral::OnBnClickedButtonSysctrl()
{
	//ϵͳ����
	CMD_C_SysCtrl SysCtrl;
	ZeroMemory(&SysCtrl, sizeof(SysCtrl));

	SysCtrl.lSysCtrlSysStorage = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_SYSCTRL_SYSSTORAGE);
	SysCtrl.lSysCtrlPlayerStorage = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_SYSCTRL_PLAYERSTORAGE);
	SysCtrl.lSysCtrlParameterK = GetDlgItemInt(IDC_EDIT_CONFIG_SYSCTRL_PARAMETER_K);
	SysCtrl.lSysCtrlSysStorResetRate = GetDlgItemInt(IDC_EDIT_CONFIG_SYSCTRL_SYSSTORAGE_RESETRATE);

	//У��
	if (!(SysCtrl.lSysCtrlParameterK > 0 && SysCtrl.lSysCtrlParameterK <= 100))
	{
		AfxMessageBox(TEXT("����ϵ����Χ1~100�����������ã�"), MB_ICONSTOP);
		return;
	}

	//У��
	if (!(SysCtrl.lSysCtrlSysStorResetRate > 0 && SysCtrl.lSysCtrlSysStorResetRate <= 100))
	{
		AfxMessageBox(TEXT("ǿ�����÷�Χ1~100�����������ã�"), MB_ICONSTOP);
		return;
	}

	// ������Ϣ
	SendDebugMessage(SUB_C_SYS_CTRL, &SysCtrl, sizeof(SysCtrl));
}

void CDialogGeneral::OnBnClickedButtonRoomctrl()
{
	//�������
	CMD_C_RoomCtrl RoomCtrl;
	ZeroMemory(&RoomCtrl, sizeof(RoomCtrl));

	RoomCtrl.dwSelCtrlIndex = INVALID_DWORD;
	RoomCtrl.lRoomCtrlCurSysStorage = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_ROOMCTRL_SYSSTORAGE);
	RoomCtrl.lRoomCtrlCurPlayerStorage = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_ROOMCTRL_PLAYERSTORAGE);
	RoomCtrl.lRoomCtrlParameterK = GetDlgItemInt(IDC_EDIT_CONFIG_ROOMCTRL_PARAMETER_K);
	RoomCtrl.lRoomCtrlSysStorResetRate = GetDlgItemInt(IDC_EDIT_CONFIG_ROOMCTRL_SYSSTORAGE_RESETRATE);
	RoomCtrl.bCancelRoomCtrl = false;

	//У��
	if (!(RoomCtrl.lRoomCtrlParameterK > 0 && RoomCtrl.lRoomCtrlParameterK <= 100))
	{
		AfxMessageBox(TEXT("����ϵ����Χ1~100�����������ã�"), MB_ICONSTOP);
		return;
	}

	//У��
	if (!(RoomCtrl.lRoomCtrlSysStorResetRate > 0 && RoomCtrl.lRoomCtrlSysStorResetRate <= 100))
	{
		AfxMessageBox(TEXT("ǿ�����÷�Χ1~100�����������ã�"), MB_ICONSTOP);
		return;
	}

	// ������Ϣ
	SendDebugMessage(SUB_C_ROOM_CTRL, &RoomCtrl, sizeof(RoomCtrl));
}

void CDialogGeneral::OnBnClickedButtonCancelRoomctrl()
{
	CListCtrl *pListCurRoomCtrlInfo = (CListCtrl *)GetDlgItem(IDC_LIST_CUR_ROOMCTRL_INFO);
	POSITION pos = pListCurRoomCtrlInfo->GetFirstSelectedItemPosition();

	if (!pos)
	{
		AfxMessageBox(TEXT("��ѡ��ĳ�У�"), MB_ICONSTOP);

		return;
	}

	int nItem = pListCurRoomCtrlInfo->GetNextSelectedItem(pos);

	//�������
	CMD_C_RoomCtrl RoomCtrl;
	ZeroMemory(&RoomCtrl, sizeof(RoomCtrl));

	RoomCtrl.dwSelCtrlIndex = _ttoi64(pListCurRoomCtrlInfo->GetItemText(nItem, 0));;
	RoomCtrl.bCancelRoomCtrl = true;

	// ������Ϣ
	SendDebugMessage(SUB_C_ROOM_CTRL, &RoomCtrl, sizeof(RoomCtrl));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDialogAdvanced, CDialogDexter)
	ON_BN_CLICKED(IDC_BUTTON_QUERY_PLAYER_GAMEID, &CDialogAdvanced::OnBnClickedButtonQueryPlayerGameID)
	ON_BN_CLICKED(IDC_BUTTON_TABLECTRL, &CDialogAdvanced::OnBnClickedButtonTablectrl)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_TABLECTRL, &CDialogAdvanced::OnBnClickedButtonCancelTablectrl)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CUR_TABLECTRL_INFO, &CDialogAdvanced::OnLvnColumnclickListUserBet)
	ON_EN_CHANGE(IDC_EDIT_PLAYER_GAMEID, &CDialogAdvanced::OnEnChangeEditPlayerGameid)
END_MESSAGE_MAP()

// ���캯��
CDialogAdvanced::CDialogAdvanced(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_ADVANCED, nShowMax, pParent)
{
	ZeroMemory(&m_UserInfo, sizeof(m_UserInfo));
	ZeroMemory(&m_curQueryRoomUserInfo, sizeof(m_curQueryRoomUserInfo));
	m_curQueryRoomUserInfo.wChairID = INVALID_CHAIR;
	m_curQueryRoomUserInfo.wTableID = INVALID_TABLE;
	m_curQueryRoomUserInfo.wBankerChairID = INVALID_CHAIR;
	m_curQueryRoomUserInfo.tablectrl_type = TABLECTRL_INVALID;
	m_bgtConfig = BGT_INVALID_;
}

// ��������
CDialogAdvanced::~CDialogAdvanced()
{
}

// ��ʼ������
BOOL CDialogAdvanced::OnInitDialog()
{
	// ��ʼ������
	__super::OnInitDialog();

	//��������
	((CEdit *)GetDlgItem(IDC_EDIT_PLAYER_GAMEID))->LimitText(7);

	//���������б�
	CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_CUR_TABLECTRL_INFO);
	pListUserBet->SetExtendedStyle(pListUserBet->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	INT nColumnCount = 0;
	pListUserBet->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("�������GAMEID"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("������Ӯ"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���Ծ���"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��ǰ����"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��ǰ���Ӻ�"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��ǰ���Ӻ�"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("������GAMEID"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��ǰ״̬"), LVCFMT_CENTER, 120);

	//���Ծ���
	for (WORD i = 0; i < MAX_EXECUTETIMES_CONFIG; i++)
	{
		CString strCtrlTimes;
		strCtrlTimes.Format(TEXT("%d"), g_cbAreaExecuteTimesArray[i]);
		
		((CComboBox *)GetDlgItem(IDC_COMBO_TABLECTRL_TIMES))->AddString(strCtrlTimes);
	}
	((CComboBox *)GetDlgItem(IDC_COMBO_TABLECTRL_TIMES))->SetCurSel(0);

	//ˢ��
	SendDebugMessage(SUB_C_REFRESH_CUR_TABLECTRL_INFO);
	SendDebugMessage(SUB_C_REFRESH_LIST_CUR_TABLECTRL_INFO);

	//�����û��б�
	SendDebugMessage(SUB_GET_ROMM_ALL_USER);

	//���ð�ť
	//((CButton *)GetDlgItem(IDC_BUTTON_TABLECTRL))->EnableWindow(FALSE);
	//((CButton *)GetDlgItem(IDC_BUTTON_CANCEL_TABLECTRL))->EnableWindow(TRUE);

	return TRUE;
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
	case  SUB_S_DEBUGTEXT_ADVANCED:
	{
		//У���С
		ASSERT(sizeof(CMD_S_DebugText) == nSize);
		if (sizeof(CMD_S_DebugText) != nSize)
		{
			return false;
		}

		// ������Ϣ
		CMD_S_DebugText *pDebugText = (CMD_S_DebugText *)pData;

		MessageBox(pDebugText->szMessageText, TEXT("��ܰ��ʾ"));

		//���ð�ť
		//((CButton *)GetDlgItem(IDC_BUTTON_TABLECTRL))->EnableWindow(FALSE);
		//((CButton *)GetDlgItem(IDC_BUTTON_CANCEL_TABLECTRL))->EnableWindow(FALSE);

		return true;
	}
	case SUB_GET_ROMM_ALL_USER_RESULT:
	{
		ASSERT(nSize == sizeof(CMD_CF_UserInfo));
		if (nSize != sizeof(CMD_CF_UserInfo))
		{
			return false;
		}

		CopyMemory(&m_UserInfo, pData, sizeof(m_UserInfo));

		return true;
	}
	case SUB_S_QUERY_GAMEID:
	{
		ASSERT(nSize == sizeof(CMD_S_QueryGameID));
		if (nSize != sizeof(CMD_S_QueryGameID))
		{
			return false;
		}

		// ������Ϣ
		CMD_S_QueryGameID *pQueryGameID = (CMD_S_QueryGameID *)pData;

		CopyMemory(&m_curQueryRoomUserInfo, &(pQueryGameID->QueryRoomUserInfo), sizeof(m_curQueryRoomUserInfo));
		m_bgtConfig = pQueryGameID->bgtConfig;

		//���ť
		//((CButton *)GetDlgItem(IDC_BUTTON_TABLECTRL))->EnableWindow(TRUE);
		//((CButton *)GetDlgItem(IDC_BUTTON_CANCEL_TABLECTRL))->EnableWindow(TRUE);

		return true;
	}
	case SUB_S_CUR_TABLECTRL_INFO:
	{
		//У���С
		ASSERT(nSize % sizeof(CMD_S_CurTableCtrlInfo) == 0);
		if (nSize % sizeof(CMD_S_CurTableCtrlInfo) != 0)
		{
			return false;
		}

		if (nSize == 0)
		{
			SetDlgItemText(IDC_STATIC_TABLECTRL_ID, TEXT(""));
			SetDlgItemText(IDC_STATIC_TABLECTRL_CURTABLEID, TEXT(""));
			SetDlgItemText(IDC_STATIC_TABLECTRL_CURTABLE_BANKERCHAIRID, TEXT(""));
			SetDlgItemText(IDC_STATIC_TABLECTRL_CURTABLE_PLAYERCOUNT, TEXT(""));
			SetDlgItemText(IDC_STATIC_TABLECTRL_CONFIGCOUNT, TEXT(""));
			SetDlgItemText(IDC_STATIC_TABLECTRL_CURCOUNT, TEXT(""));
		}
		else
		{
			// ������Ϣ
			CMD_S_CurTableCtrlInfo *pCurTableCtrlInfo = (CMD_S_CurTableCtrlInfo *)pData;

			//���
			if (pCurTableCtrlInfo->cbConfigCtrlTimes == pCurTableCtrlInfo->cbCurCtrlTimes)
			{
				SetDlgItemText(IDC_STATIC_TABLECTRL_ID, TEXT(""));
				SetDlgItemText(IDC_STATIC_TABLECTRL_CURTABLEID, TEXT(""));
				SetDlgItemText(IDC_STATIC_TABLECTRL_CURTABLE_BANKERCHAIRID, TEXT(""));
				SetDlgItemText(IDC_STATIC_TABLECTRL_CURTABLE_PLAYERCOUNT, TEXT(""));
				SetDlgItemText(IDC_STATIC_TABLECTRL_CONFIGCOUNT, TEXT(""));
				SetDlgItemText(IDC_STATIC_TABLECTRL_CURCOUNT, TEXT(""));

				CString strSysWin;
				strSysWin.Format(SCORE_STRING, pCurTableCtrlInfo->lTableCtrlSysWin);
				SetDlgItemText(IDC_STATIC_CUR_TABLECTRL_SYSWIN, strSysWin);

				//
				for (BYTE cbPosIndex = 0; cbPosIndex < MAX_TABLECTRL_TYPE; cbPosIndex++)
				{
					((CButton *)GetDlgItem(IDC_RADIO_TABLECTRL_CURPLAYER_WIN + cbPosIndex))->SetCheck(FALSE);
				}

				((CComboBox *)GetDlgItem(IDC_COMBO_TABLECTRL_TIMES))->SetCurSel(0);
			}
			else
			{
				SetDlgItemInt(IDC_STATIC_TABLECTRL_ID, pCurTableCtrlInfo->dwCtrlIndex);
				SetDlgItemInt(IDC_STATIC_TABLECTRL_CURTABLEID, pCurTableCtrlInfo->wTableID);

				if (pCurTableCtrlInfo->wBankerChairID != INVALID_CHAIR)
				{
					SetDlgItemInt(IDC_STATIC_TABLECTRL_CURTABLE_BANKERCHAIRID, pCurTableCtrlInfo->wBankerChairID);
				}
				else
				{
					SetDlgItemText(IDC_STATIC_TABLECTRL_CURTABLE_BANKERCHAIRID, TEXT("��Ч"));
				}
				
				SetDlgItemInt(IDC_STATIC_TABLECTRL_CURTABLE_PLAYERCOUNT, pCurTableCtrlInfo->wCurTablePlayerCount);
				SetDlgItemInt(IDC_STATIC_TABLECTRL_CONFIGCOUNT, pCurTableCtrlInfo->cbConfigCtrlTimes);
				SetDlgItemInt(IDC_STATIC_TABLECTRL_CURCOUNT, pCurTableCtrlInfo->cbCurCtrlTimes);

				CString strSysWin;
				strSysWin.Format(SCORE_STRING, pCurTableCtrlInfo->lTableCtrlSysWin);
				SetDlgItemText(IDC_STATIC_CUR_TABLECTRL_SYSWIN, strSysWin);

				//
				for (WORD i = 0; i < MAX_TABLECTRL_TYPE; i++)
				{
					((CButton *)GetDlgItem(IDC_RADIO_TABLECTRL_CURPLAYER_WIN + i))->SetCheck(FALSE);
				}

				((CButton *)GetDlgItem(IDC_RADIO_TABLECTRL_CURPLAYER_WIN + pCurTableCtrlInfo->cbCtrlPosIndex))->SetCheck(TRUE);

				for (WORD i = 0; i < MAX_EXECUTE_TIMES; i++)
				{
					if (g_cbAreaExecuteTimesArray[i] == pCurTableCtrlInfo->cbConfigCtrlTimes)
					{
						((CComboBox *)GetDlgItem(IDC_COMBO_TABLECTRL_TIMES))->SetCurSel(i);

						break;
					}
				}
			}
		}

		return true;
	}
	case SUB_S_LIST_CUR_TABLECTRL_INFO:
	{
		ASSERT(nSize % sizeof(CMD_S_ListCurTableCtrlInfo) == 0);
		if (nSize % sizeof(CMD_S_ListCurTableCtrlInfo) != 0)
		{
			return false;
		}

		CListCtrl *pListCurTableCtrlInfo = (CListCtrl *)GetDlgItem(IDC_LIST_CUR_TABLECTRL_INFO);

		if (nSize == 0)
		{
			pListCurTableCtrlInfo->DeleteAllItems();
		}
		else
		{
			//������Ϣ
			CMD_S_ListCurTableCtrlInfo *pCurTableCtrlInfo = (CMD_S_ListCurTableCtrlInfo *)pData;

			int nItemIndex = -1;
			for (WORD wIndex = 0; wIndex<pListCurTableCtrlInfo->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListCurTableCtrlInfo->GetItemText(wIndex, 0)) == pCurTableCtrlInfo->curTableCtrlListInfo.dwCtrlIndex)
				{
					nItemIndex = wIndex;
					break;
				}
			}

			if (nItemIndex == -1)
			{
				CString strCtrlIndex;
				strCtrlIndex.Format(TEXT("%d"), pCurTableCtrlInfo->curTableCtrlListInfo.dwCtrlIndex);

				int nItem = pListCurTableCtrlInfo->GetItemCount();
				nItemIndex = pListCurTableCtrlInfo->InsertItem(nItem, strCtrlIndex);

				CString strCtrlVal;
				strCtrlVal.Format(TEXT("%d"), pCurTableCtrlInfo->curTableCtrlListInfo.roomUserInfo.dwGameID);
				pListCurTableCtrlInfo->SetItemText(nItem, 1, strCtrlVal);

				pListCurTableCtrlInfo->SetItemText(nItem, 2, GetTableCtrlPosDesc(pCurTableCtrlInfo->curTableCtrlListInfo.tablectrl_type));

				strCtrlVal.Format(TEXT("%d"), pCurTableCtrlInfo->curTableCtrlListInfo.roomUserInfo.cbConfigCtrlTimes);
				pListCurTableCtrlInfo->SetItemText(nItem, 3, strCtrlVal);

				strCtrlVal.Format(TEXT("%d"), pCurTableCtrlInfo->curTableCtrlListInfo.roomUserInfo.cbCurCtrlTimes);
				pListCurTableCtrlInfo->SetItemText(nItem, 4, strCtrlVal);

				strCtrlVal.Format(TEXT("%d"), pCurTableCtrlInfo->curTableCtrlListInfo.roomUserInfo.wTableID);
				pListCurTableCtrlInfo->SetItemText(nItem, 5, strCtrlVal);

				strCtrlVal.Format(TEXT("%d"), pCurTableCtrlInfo->curTableCtrlListInfo.roomUserInfo.wChairID);
				pListCurTableCtrlInfo->SetItemText(nItem, 6, strCtrlVal);

				//������
				strCtrlVal.Format(TEXT("%d"), pCurTableCtrlInfo->curTableCtrlListInfo.dwDebugUserGameID);
				pListCurTableCtrlInfo->SetItemText(nItem, 7, strCtrlVal);

				strCtrlVal.Format(TEXT("%d-%d,%d:%d"), pCurTableCtrlInfo->curTableCtrlListInfo.tmConfigTime.GetMonth(), pCurTableCtrlInfo->curTableCtrlListInfo.tmConfigTime.GetDay(), pCurTableCtrlInfo->curTableCtrlListInfo.tmConfigTime.GetHour(), pCurTableCtrlInfo->curTableCtrlListInfo.tmConfigTime.GetMinute());
				pListCurTableCtrlInfo->SetItemText(nItem, 8, strCtrlVal);

				if (pCurTableCtrlInfo->curTableCtrlListInfo.tableCtrlStatus == TABLECTRL_PROGRESSING)
				{
					pListCurTableCtrlInfo->SetItemText(nItem, 9, TEXT("������"));
				}
				else if (pCurTableCtrlInfo->curTableCtrlListInfo.tableCtrlStatus == TABLECTRL_FINISH)
				{
					pListCurTableCtrlInfo->SetItemText(nItem, 9, TEXT("���"));
				}
				else if (pCurTableCtrlInfo->curTableCtrlListInfo.tableCtrlStatus == TABLECTRL_CANCEL)
				{
					pListCurTableCtrlInfo->SetItemText(nItem, 9, TEXT("ȡ��"));
				}
				else if (pCurTableCtrlInfo->curTableCtrlListInfo.tableCtrlStatus == TABLECTRL_EXECUTE_CANCEL)
				{
					pListCurTableCtrlInfo->SetItemText(nItem, 9, TEXT("��ִ��"));
				}

				if (pCurTableCtrlInfo->curTableCtrlListInfo.tableCtrlStatus == TABLECTRL_PROGRESSING)
				{
					pListCurTableCtrlInfo->SetItemState(nItem, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
				}
			}
		}

		return true;
	}
	default:
		break;
	}
										
	return false;
}

//�Ƿ�AI
bool CDialogAdvanced::IsValidGameID(DWORD dwGameID)
{
	bool bValid = false;

	for (WORD i = 0; i < MAX_CHAIR; i++)
	{
		if (m_UserInfo.wGameID[i] == dwGameID)
		{
			bValid = true;

			break;
		}
	}

	return bValid;
}

//��ȡUserID
DWORD CDialogAdvanced::GetUserIDEx(DWORD dwGameID)
{
	DWORD dwUserID = INVALID_DWORD;

	for (WORD i = 0; i < MAX_CHAIR; i++)
	{
		if (m_UserInfo.wGameID[i] == dwGameID)
		{
			dwUserID = m_UserInfo.wUserID[i];
			break;
		}
	}

	return dwUserID;
}

//���к���
INT CALLBACK CDialogAdvanced::SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
	//
	return 0;
}

void CDialogAdvanced::OnBnClickedButtonQueryPlayerGameID()
{	
	ZeroMemory(&m_curQueryRoomUserInfo, sizeof(m_curQueryRoomUserInfo));
	m_curQueryRoomUserInfo.wChairID = INVALID_CHAIR;
	m_curQueryRoomUserInfo.wTableID = INVALID_TABLE;
	m_curQueryRoomUserInfo.wBankerChairID = INVALID_CHAIR;
	m_curQueryRoomUserInfo.tablectrl_type = TABLECTRL_INVALID;

	//�����û��б�
	SendDebugMessage(SUB_GET_ROMM_ALL_USER);

	CString strGameID;
	GetDlgItemText(IDC_EDIT_PLAYER_GAMEID, strGameID);
	strGameID.TrimLeft();
	strGameID.TrimRight();

	DWORD dwGameID = _ttoi(strGameID);

	if (strGameID == TEXT("") || dwGameID == 0)
	{
		AfxMessageBox(TEXT("���GAMEIDΪ��Ч!"), MB_ICONSTOP);

		return;
	}
	else if (!IsValidGameID(dwGameID))
	{
		AfxMessageBox(TEXT("���GAMEID�����ڷ���!"), MB_ICONSTOP);

		return;
	}

	CMD_C_QueryGameID QueryGameID;
	ZeroMemory(&QueryGameID, sizeof(QueryGameID));

	QueryGameID.dwQueryGameID = dwGameID;
	QueryGameID.dwQueryUserID = GetUserIDEx(dwGameID);
	
	SendDebugMessage(SUB_C_QUERY_GAMEID, &QueryGameID, sizeof(QueryGameID));

	//���ť
	//((CButton *)GetDlgItem(IDC_BUTTON_TABLECTRL))->EnableWindow(FALSE);
	//((CButton *)GetDlgItem(IDC_BUTTON_CANCEL_TABLECTRL))->EnableWindow(FALSE);
}

void CDialogAdvanced::OnBnClickedButtonTablectrl()
{
	//�����û��б�
	SendDebugMessage(SUB_GET_ROMM_ALL_USER);

	CString strGameID;
	GetDlgItemText(IDC_EDIT_PLAYER_GAMEID, strGameID);
	strGameID.TrimLeft();
	strGameID.TrimRight();

	DWORD dwGameID = _ttoi(strGameID);

	if (strGameID == TEXT("") || dwGameID == 0)
	{
		AfxMessageBox(TEXT("���GAMEIDΪ��Ч!"), MB_ICONSTOP);

		return;
	}

	//if (m_curQueryRoomUserInfo.dwGameID == 0)
	//{
	//	AfxMessageBox(TEXT("��ѯ�����Ч,�޷�����!"), MB_ICONSTOP);
	//	return;
	//}
	//else if (m_curQueryRoomUserInfo.wChairID == INVALID_CHAIR)
	//{
	//	AfxMessageBox(TEXT("��ѯ���������Ч,�޷�����!"), MB_ICONSTOP);
	//	return;
	//}

	BYTE cbPosIndex = 0;
	for (cbPosIndex = 0; cbPosIndex < MAX_TABLECTRL_TYPE; cbPosIndex++)
	{
		if (((CButton *)GetDlgItem(IDC_RADIO_TABLECTRL_CURPLAYER_WIN + cbPosIndex))->GetCheck())
		{
			break;
		}
	}

	if ((cbPosIndex == 2 || cbPosIndex == 3) && (m_bgtConfig == BGT_TONGBI_))
	{
		AfxMessageBox(TEXT("ͨ���淨û��ׯ��,�޷�����!"), MB_ICONSTOP);
		return;
	}

	CMD_C_TableCtrl TableCtrl;
	ZeroMemory(&TableCtrl, sizeof(TableCtrl));

	int nSelectTimes = (INT)((CComboBox *)GetDlgItem(IDC_COMBO_TABLECTRL_TIMES))->GetCurSel();

	if (cbPosIndex >= 0 && cbPosIndex < MAX_TABLECTRL_TYPE && nSelectTimes >= 0 && nSelectTimes < MAX_EXECUTE_TIMES)
	{
		TableCtrl.dwCtrlGameID = dwGameID;
		TableCtrl.dwCtrlUserID = GetUserIDEx(dwGameID);
		TableCtrl.cbCtrlPosIndex = cbPosIndex;
		TableCtrl.cbConfigCtrlTimes = g_cbAreaExecuteTimesArray[nSelectTimes];;
		TableCtrl.bCancelTableCtrl = false;

		SendDebugMessage(SUB_C_TABLE_CTRL, &TableCtrl, sizeof(TableCtrl));
	}
	else
	{
		AfxMessageBox(TEXT("��ѡ����Դ����Լ���������!"), MB_ICONSTOP);
		return;
	}
}

void CDialogAdvanced::OnBnClickedButtonCancelTablectrl()
{
	CListCtrl *pListCurTableCtrlInfo = (CListCtrl *)GetDlgItem(IDC_LIST_CUR_TABLECTRL_INFO);
	POSITION pos = pListCurTableCtrlInfo->GetFirstSelectedItemPosition();

	if (!pos)
	{
		AfxMessageBox(TEXT("��ѡ��ĳ�У�"), MB_ICONSTOP);

		return;
	}

	int nItem = pListCurTableCtrlInfo->GetNextSelectedItem(pos);
	
	CMD_C_TableCtrl TableCtrl;
	ZeroMemory(&TableCtrl, sizeof(TableCtrl));
	
	TableCtrl.dwCtrlIndex = _ttoi(pListCurTableCtrlInfo->GetItemText(nItem, 0));
	TableCtrl.dwCtrlGameID = _ttoi(pListCurTableCtrlInfo->GetItemText(nItem, 1));
	TableCtrl.dwCtrlUserID = GetUserIDEx(TableCtrl.dwCtrlGameID);
	TableCtrl.bCancelTableCtrl = true;
	
	SendDebugMessage(SUB_C_TABLE_CTRL, &TableCtrl, sizeof(TableCtrl));
}

void CDialogAdvanced::OnLvnColumnclickListUserBet(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	WORD wColumnIndex = pNMLV->iSubItem;

	CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_CUR_TABLECTRL_INFO);

	//�����б�
	pListUserBet->SortItems(SortFunction, (LPARAM)(&wColumnIndex));
}

void CDialogAdvanced::OnEnChangeEditPlayerGameid()
{
	ZeroMemory(&m_curQueryRoomUserInfo, sizeof(m_curQueryRoomUserInfo));
	m_curQueryRoomUserInfo.wChairID = INVALID_CHAIR;
	m_curQueryRoomUserInfo.wTableID = INVALID_TABLE;
	m_curQueryRoomUserInfo.wBankerChairID = INVALID_CHAIR;
	m_curQueryRoomUserInfo.tablectrl_type = TABLECTRL_INVALID;

	//���ť
	//((CButton *)GetDlgItem(IDC_BUTTON_TABLECTRL))->EnableWindow(FALSE);
	//((CButton *)GetDlgItem(IDC_BUTTON_CANCEL_TABLECTRL))->EnableWindow(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// ���캯��
CDialogTraceLog::CDialogTraceLog(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_TRACELOG, nShowMax, pParent)
{
}

// ��������
CDialogTraceLog::~CDialogTraceLog()
{
}

// �ؼ���
void CDialogTraceLog::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);
}

// ��ʼ������
BOOL CDialogTraceLog::OnInitDialog()
{
	// ��ʼ������
	__super::OnInitDialog();

	INT nColumnCount = 0;

	//�������
	CListCtrl *pListSysCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_SYSCTRL_TRACELOG);
	pListSysCtrlTraceLog->SetExtendedStyle(pListSysCtrlTraceLog->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("����ϵͳ���"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("������ҿ��"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("��ǰϵͳ���"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("��ǰ��ҿ��"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("����ϵ��"), LVCFMT_CENTER, 60);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("���ͳ��"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("������ô���"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 100);

	//�������
	CListCtrl *pListRoomCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_ROOMCTRL_TRACELOG);
	pListRoomCtrlTraceLog->SetExtendedStyle(pListRoomCtrlTraceLog->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("����ϵͳ���"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("������ҿ��"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("��ǰϵͳ���"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("��ǰ��ҿ��"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("����ϵ��"), LVCFMT_CENTER, 60);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("���ͳ��"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("������"), LVCFMT_CENTER, 60);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("״̬"), LVCFMT_CENTER, 60);

	//��������
	CListCtrl *pListTableCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_TABLECTRL_TRACELOG);
	pListTableCtrlTraceLog->SetExtendedStyle(pListTableCtrlTraceLog->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListTableCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
	pListTableCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("������Ӯ"), LVCFMT_CENTER, 60);
	pListTableCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("���Ծ���"), LVCFMT_CENTER, 60);
	pListTableCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("���Ӻ�"), LVCFMT_CENTER, 100);
	pListTableCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("������"), LVCFMT_CENTER, 60);
	pListTableCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 100);
	pListTableCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("״̬"), LVCFMT_CENTER, 60);

	//ˢ��
	SendDebugMessage(SUB_C_REFRESH_TRACELOG);

	return TRUE;
}

// ��Ϣ����
bool CDialogTraceLog::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
	switch (nMessageID)
	{
	case SUB_S_REFRESH_SYSCTRL_TRACELOG:
	{
		ASSERT(nSize % sizeof(CMD_S_SysCtrlTraceLog) == 0);
		if (nSize % sizeof(CMD_S_SysCtrlTraceLog) != 0)
		{
			return false;
		}

		CListCtrl *pListSysCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_SYSCTRL_TRACELOG);

		if (nSize == 0)
		{
			pListSysCtrlTraceLog->DeleteAllItems();
		}
		else
		{
			//������Ϣ
			CMD_S_SysCtrlTraceLog *pSysCtrlResult = (CMD_S_SysCtrlTraceLog *)pData;

			int nItemIndex = -1;
			for (WORD wIndex = 0; wIndex<pListSysCtrlTraceLog->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListSysCtrlTraceLog->GetItemText(wIndex, 0)) == pSysCtrlResult->SysCtrlTraceLog.dwCtrlIndex)
				{
					nItemIndex = wIndex;
					break;
				}
			}

			if (nItemIndex == -1)
			{
				CString strCtrlIndex;
				strCtrlIndex.Format(TEXT("%d"), pSysCtrlResult->SysCtrlTraceLog.dwCtrlIndex);

				int nItem = pListSysCtrlTraceLog->GetItemCount();
				nItemIndex = pListSysCtrlTraceLog->InsertItem(nItem, strCtrlIndex);

				CString strCtrlVal;
				strCtrlVal.Format(SCORE_STRING, pSysCtrlResult->SysCtrlTraceLog.lSysCtrlConfigSysStorage);
				pListSysCtrlTraceLog->SetItemText(nItem, 1, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pSysCtrlResult->SysCtrlTraceLog.lSysCtrlConfigSysStorage);
				pListSysCtrlTraceLog->SetItemText(nItem, 2, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pSysCtrlResult->SysCtrlTraceLog.lSysCtrlCurSysStorage);
				pListSysCtrlTraceLog->SetItemText(nItem, 3, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pSysCtrlResult->SysCtrlTraceLog.lSysCtrlCurPlayerStorage);
				pListSysCtrlTraceLog->SetItemText(nItem, 4, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pSysCtrlResult->SysCtrlTraceLog.lSysCtrlParameterK);
				pListSysCtrlTraceLog->SetItemText(nItem, 5, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pSysCtrlResult->SysCtrlTraceLog.lSysCtrlSysWin);
				pListSysCtrlTraceLog->SetItemText(nItem, 6, strCtrlVal);

				strCtrlVal.Format(TEXT("%d"), pSysCtrlResult->SysCtrlTraceLog.lSysCtrlResetCount);
				pListSysCtrlTraceLog->SetItemText(nItem, 7, strCtrlVal);

				strCtrlVal.Format(TEXT("%d-%d,%d:%d"), pSysCtrlResult->SysCtrlTraceLog.tmResetTime.GetMonth(), pSysCtrlResult->SysCtrlTraceLog.tmResetTime.GetDay(), pSysCtrlResult->SysCtrlTraceLog.tmResetTime.GetHour(), pSysCtrlResult->SysCtrlTraceLog.tmResetTime.GetMinute());
				pListSysCtrlTraceLog->SetItemText(nItem, 8, strCtrlVal);

				if (pSysCtrlResult->SysCtrlTraceLog.sysCtrlStatus == PROGRESSINGEX)
				{
					pListSysCtrlTraceLog->SetItemState(nItem, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
				}
			}
		}

		return true;
	}
	case SUB_S_REFRESH_ROOMCTRL_TRACELOG:
	{
		ASSERT(nSize % sizeof(CMD_S_RoomCtrlTraceLog) == 0);
		if (nSize % sizeof(CMD_S_RoomCtrlTraceLog) != 0)
		{
			return false;
		}

		CListCtrl *pListRoomCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_ROOMCTRL_TRACELOG);

		if (nSize == 0)
		{
			pListRoomCtrlTraceLog->DeleteAllItems();
		}
		else
		{
			//������Ϣ
			CMD_S_RoomCtrlTraceLog *pRoomCtrlResult = (CMD_S_RoomCtrlTraceLog *)pData;

			int nItemIndex = -1;
			for (WORD wIndex = 0; wIndex<pListRoomCtrlTraceLog->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListRoomCtrlTraceLog->GetItemText(wIndex, 0)) == pRoomCtrlResult->RoomCtrlTraceLog.dwCtrlIndex)
				{
					nItemIndex = wIndex;
					break;
				}
			}

			if (nItemIndex == -1)
			{
				CString strCtrlIndex;
				strCtrlIndex.Format(TEXT("%d"), pRoomCtrlResult->RoomCtrlTraceLog.dwCtrlIndex);

				int nItem = pListRoomCtrlTraceLog->GetItemCount();
				nItemIndex = pListRoomCtrlTraceLog->InsertItem(nItem, strCtrlIndex);

				CString strCtrlVal;
				strCtrlVal.Format(SCORE_STRING, pRoomCtrlResult->RoomCtrlTraceLog.lRoomCtrlConfigSysStorage);
				pListRoomCtrlTraceLog->SetItemText(nItem, 1, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pRoomCtrlResult->RoomCtrlTraceLog.lRoomCtrlConfigPlayerStorage);
				pListRoomCtrlTraceLog->SetItemText(nItem, 2, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pRoomCtrlResult->RoomCtrlTraceLog.lRoomCtrlCurSysStorage);
				pListRoomCtrlTraceLog->SetItemText(nItem, 3, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pRoomCtrlResult->RoomCtrlTraceLog.lRoomCtrlCurPlayerStorage);
				pListRoomCtrlTraceLog->SetItemText(nItem, 4, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pRoomCtrlResult->RoomCtrlTraceLog.lRoomCtrlParameterK);
				pListRoomCtrlTraceLog->SetItemText(nItem, 5, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pRoomCtrlResult->RoomCtrlTraceLog.lRoomCtrlSysWin);
				pListRoomCtrlTraceLog->SetItemText(nItem, 6, strCtrlVal);

				//������
				strCtrlVal.Format(TEXT("%d"), pRoomCtrlResult->RoomCtrlTraceLog.dwDebugUserGameID);
				pListRoomCtrlTraceLog->SetItemText(nItem, 7, strCtrlVal);

				strCtrlVal.Format(TEXT("%d-%d,%d:%d"), pRoomCtrlResult->RoomCtrlTraceLog.tmConfigTime.GetMonth(), pRoomCtrlResult->RoomCtrlTraceLog.tmConfigTime.GetDay(), pRoomCtrlResult->RoomCtrlTraceLog.tmConfigTime.GetHour(), pRoomCtrlResult->RoomCtrlTraceLog.tmConfigTime.GetMinute());
				pListRoomCtrlTraceLog->SetItemText(nItem, 8, strCtrlVal);

				if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == QUEUE)
				{
					 pListRoomCtrlTraceLog->SetItemText(nItem, 9, TEXT("����"));
				}
				else if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == PROGRESSING)
				{
					pListRoomCtrlTraceLog->SetItemText(nItem, 9, TEXT("������"));
				}
				else if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == FINISH)
				{
					pListRoomCtrlTraceLog->SetItemText(nItem, 9, TEXT("���"));
				}
				else if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == CANCEL)
				{
					pListRoomCtrlTraceLog->SetItemText(nItem, 9, TEXT("ȡ��"));
				}
				else if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == EXECUTE_CANCEL)
				{
					pListRoomCtrlTraceLog->SetItemText(nItem, 9, TEXT("��ִ��"));
				}

				if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == PROGRESSING)
				{
					pListRoomCtrlTraceLog->SetItemState(nItem, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
				}
			}
		}

		return true;
	}
	case SUB_S_REFRESH_TABLECTRL_TRACELOG:
	{
		ASSERT(nSize % sizeof(CMD_S_TableCtrlTraceLog) == 0);
		if (nSize % sizeof(CMD_S_TableCtrlTraceLog) != 0)
		{
			return false;
		}

		CListCtrl *pListTableCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_TABLECTRL_TRACELOG);

		if (nSize == 0)
		{
			pListTableCtrlTraceLog->DeleteAllItems();
		}
		else
		{
			//������Ϣ
			CMD_S_TableCtrlTraceLog *pTableCtrlResult = (CMD_S_TableCtrlTraceLog *)pData;

			int nItemIndex = -1;
			for (WORD wIndex = 0; wIndex<pListTableCtrlTraceLog->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListTableCtrlTraceLog->GetItemText(wIndex, 0)) == pTableCtrlResult->TableCtrlTraceLog.dwCtrlIndex)
				{
					nItemIndex = wIndex;
					break;
				}
			}

			if (nItemIndex == -1)
			{
				CString strCtrlIndex;
				strCtrlIndex.Format(TEXT("%d"), pTableCtrlResult->TableCtrlTraceLog.dwCtrlIndex);

				int nItem = pListTableCtrlTraceLog->GetItemCount();
				nItemIndex = pListTableCtrlTraceLog->InsertItem(nItem, strCtrlIndex);

				pListTableCtrlTraceLog->SetItemText(nItem, 1, GetTableCtrlPosDesc(pTableCtrlResult->TableCtrlTraceLog.tablectrl_type));
				
				CString strCtrlVal;
				strCtrlVal.Format(TEXT("%d"), pTableCtrlResult->TableCtrlTraceLog.roomUserInfo.cbConfigCtrlTimes);
				pListTableCtrlTraceLog->SetItemText(nItem, 2, strCtrlVal);

				strCtrlVal.Format(TEXT("%d"), pTableCtrlResult->TableCtrlTraceLog.roomUserInfo.wTableID);
				pListTableCtrlTraceLog->SetItemText(nItem, 3, strCtrlVal);

				//������
				strCtrlVal.Format(TEXT("%d"), pTableCtrlResult->TableCtrlTraceLog.dwDebugUserGameID);
				pListTableCtrlTraceLog->SetItemText(nItem, 4, strCtrlVal);

				strCtrlVal.Format(TEXT("%d-%d,%d:%d"), pTableCtrlResult->TableCtrlTraceLog.tmConfigTime.GetMonth(), pTableCtrlResult->TableCtrlTraceLog.tmConfigTime.GetDay(), pTableCtrlResult->TableCtrlTraceLog.tmConfigTime.GetHour(), pTableCtrlResult->TableCtrlTraceLog.tmConfigTime.GetMinute());
				pListTableCtrlTraceLog->SetItemText(nItem, 5, strCtrlVal);

				if (pTableCtrlResult->TableCtrlTraceLog.tableCtrlStatus == TABLECTRL_PROGRESSING)
				{
					pListTableCtrlTraceLog->SetItemText(nItem, 6, TEXT("������"));
				}
				else if (pTableCtrlResult->TableCtrlTraceLog.tableCtrlStatus == TABLECTRL_FINISH)
				{
					pListTableCtrlTraceLog->SetItemText(nItem, 6, TEXT("���"));
				}
				else if (pTableCtrlResult->TableCtrlTraceLog.tableCtrlStatus == TABLECTRL_CANCEL)
				{
					pListTableCtrlTraceLog->SetItemText(nItem, 6, TEXT("ȡ��"));
				}
				else if (pTableCtrlResult->TableCtrlTraceLog.tableCtrlStatus == TABLECTRL_EXECUTE_CANCEL)
				{
					pListTableCtrlTraceLog->SetItemText(nItem, 6, TEXT("��ִ��"));
				}

				if (pTableCtrlResult->TableCtrlTraceLog.tableCtrlStatus == TABLECTRL_PROGRESSING)
				{
					pListTableCtrlTraceLog->SetItemState(nItem, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
				}
			}
		}

		return true;
	}
	case SUB_S_STARTRECORD_TRACELOG:
	{
		ASSERT(nSize % sizeof(CMD_S_StartRecordTraceLog) == 0);
		if (nSize % sizeof(CMD_S_StartRecordTraceLog) != 0)
		{
			return false;
		}

		//������Ϣ
		CMD_S_StartRecordTraceLog *pStartRecordTraceLog = (CMD_S_StartRecordTraceLog *)pData;

		CString strStartRecord;
		strStartRecord.Format(TEXT("%d/%2d/%2d-%2d/%2d/%2d"), pStartRecordTraceLog->tmStartRecord.GetYear(), pStartRecordTraceLog->tmStartRecord.GetMonth(), pStartRecordTraceLog->tmStartRecord.GetDay(),
			pStartRecordTraceLog->tmStartRecord.GetHour(), pStartRecordTraceLog->tmStartRecord.GetMinute(), pStartRecordTraceLog->tmStartRecord.GetSecond());
		SetDlgItemText(IDC_STATIC_TRACELOG_BEGIN_RECORDTIME, strStartRecord);

		return true;
	}

	default:
		break;
	}

	return false;
}

void CDialogTraceLog::CleanCtrlList()
{
	CListCtrl *pListSysCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_SYSCTRL_TRACELOG);
	pListSysCtrlTraceLog->DeleteAllItems();

	CListCtrl *pListRoomCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_ROOMCTRL_TRACELOG);
	pListRoomCtrlTraceLog->DeleteAllItems();

	CListCtrl *pListTableCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_TABLECTRL_TRACELOG);
	pListTableCtrlTraceLog->DeleteAllItems();
}
