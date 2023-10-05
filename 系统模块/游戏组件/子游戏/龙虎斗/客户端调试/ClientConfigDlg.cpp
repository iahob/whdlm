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
	pTabCtrl->InsertItem(1, _T("�������"));
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
	ON_BN_CLICKED(IDC_BUTTON_MOSGOLD, &CDialogGeneral::OnBnClickedButtonMosgold)
	ON_BN_CLICKED(IDC_BUTTON_ROOMCTRL, &CDialogGeneral::OnBnClickedButtonRoomctrl)
	ON_BN_CLICKED(IDC_BUTTON_SYSCTRL, &CDialogGeneral::OnBnClickedButtonSysctrl)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_ROOMCTRL, &CDialogGeneral::OnBnClickedButtonCancelRoomctrl)
END_MESSAGE_MAP()

// ���캯��
CDialogGeneral::CDialogGeneral(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_GENERAL, nShowMax, pParent)
{
	m_lAreaLimitScore = 0;
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
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_MOSGOLD_RATE))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_MOSGOLD_DISRATE))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_MOSGOLDSTORAGE_RATE))->LimitText(3);
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
	SendDebugMessage(SUB_C_REFRESH_ANCHOU_MOSGOLD);
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

		//��ֵ��������
		m_lAreaLimitScore = pStatisticService->lAreaLimitScore;
		CDialogGeneral::m_wChairCount = pStatisticService->wChairCount;

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

		//SetDlgItemText(IDC_EDIT_CONFIG_ANCHOU_RATE, TEXT(""));

		return true;
	}
	case SUB_S_MOSGOLD:
	{
		ASSERT(wDataSize == sizeof(CMD_S_Mosgold));
		if (wDataSize != sizeof(CMD_S_Mosgold))
		{
			return false;
		}

		CMD_S_Mosgold *pMosgold = (CMD_S_Mosgold *)pBuffer;

		CString strMosgoldStorage;
		strMosgoldStorage.Format(SCORE_STRING, pMosgold->lCurMosgoldStorage);
		SetDlgItemText(IDC_STATIC_CUR_MOSGOLD_STORAGE, strMosgoldStorage);
		
		SetDlgItemInt(IDC_STATIC_CUR_MOSGOLD_RATE, pMosgold->lCurMosgoldRate);

		SetDlgItemInt(IDC_STATIC_CUR_MOSGOLD_DISRATE, pMosgold->lCurMosgoldDispatchRate);
		SetDlgItemInt(IDC_STATIC_CUR_MOSGOLD_STORAGERATE, pMosgold->lCurMosgoldStorageRate);

		//SetDlgItemText(IDC_EDIT_CONFIG_MOSGOLD_RATE, TEXT(""));
		//SetDlgItemText(IDC_EDIT_CONFIG_MOSGOLDSTORAGE_RATE, TEXT(""));
		//SetDlgItemText(IDC_EDIT_CONFIG_MOSGOLD_DISRATE, TEXT(""));

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

		//SetDlgItemText(IDC_EDIT_CONFIG_ROOMCTRL_SYSSTORAGE, TEXT(""));
		//SetDlgItemText(IDC_EDIT_CONFIG_ROOMCTRL_PLAYERSTORAGE, TEXT(""));
		//SetDlgItemText(IDC_EDIT_CONFIG_ROOMCTRL_PARAMETER_K, TEXT(""));
		//SetDlgItemText(IDC_EDIT_CONFIG_ROOMCTRL_SYSSTORAGE_RESETRATE, TEXT(""));

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

void CDialogGeneral::OnBnClickedButtonMosgold()
{
	CString strConfigMosgoldRate;
	GetDlgItemText(IDC_EDIT_CONFIG_MOSGOLD_RATE, strConfigMosgoldRate);

	//�п�У��
	if (strConfigMosgoldRate == TEXT(""))
	{
		AfxMessageBox(TEXT("�󽱳�ȡ��������Ϊ�գ����������ã�"), MB_ICONSTOP);
		return;
	}

	//���ô�
	CMD_C_Mosgold Mosgold;
	ZeroMemory(&Mosgold, sizeof(Mosgold));

	Mosgold.lConfigMosgoldRate = GetDlgItemInt(IDC_EDIT_CONFIG_MOSGOLD_RATE);
	Mosgold.lConfigMosgoldDispatchCon = 0/*(SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_MOSGOLDSTORAGE_RATE)*/;
	Mosgold.lConfigMosgoldDispatchRate = GetDlgItemInt(IDC_EDIT_CONFIG_MOSGOLD_DISRATE);
	Mosgold.lConfigMosgoldStorageRate = GetDlgItemInt(IDC_EDIT_CONFIG_MOSGOLDSTORAGE_RATE);

	//У��
	if (!(Mosgold.lConfigMosgoldRate >= 0 && Mosgold.lConfigMosgoldRate <= 30))
	{
		AfxMessageBox(TEXT("�󽱳�ȡ������Χ0~30�����������ã�"), MB_ICONSTOP);
		return;
	}

	//У��
	if (!(Mosgold.lConfigMosgoldDispatchRate > 0 && Mosgold.lConfigMosgoldDispatchRate <= 100))
	{
		AfxMessageBox(TEXT("�ɽ����ʷ�Χ1~100�����������ã�"), MB_ICONSTOP);
		return;
	}

	//У��
	if (!(Mosgold.lConfigMosgoldStorageRate > 0 && Mosgold.lConfigMosgoldStorageRate <= 100))
	{
		AfxMessageBox(TEXT("�󽱿�������Χ1~100�����������ã�"), MB_ICONSTOP);
		return;
	}

	// ������Ϣ
	SendDebugMessage(SUB_C_MOSGOLD, &Mosgold, sizeof(Mosgold));
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
	if (SysCtrl.lSysCtrlPlayerStorage < m_lAreaLimitScore * MAX_AREA_TIMES)
	{
		LONGLONG lAreaLimitScore = (LONGLONG)m_lAreaLimitScore;

		CString strTip;
		strTip.Format(TEXT("��ǰ��������Ϊ[ %I64d ],��ҿ�������ڵ����������Ƶ�[ %d ]��,���������ã�"), lAreaLimitScore, MAX_AREA_TIMES);

		AfxMessageBox(strTip, MB_ICONSTOP);
		return;
	}

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
	if (RoomCtrl.lRoomCtrlCurPlayerStorage < m_lAreaLimitScore * MAX_AREA_TIMES)
	{
		LONGLONG lAreaLimitScore = (LONGLONG)m_lAreaLimitScore;

		CString strTip;
		strTip.Format(TEXT("��ǰ��������Ϊ[ %I64d ],��ҿ�������ڵ����������Ƶ�[ %d ]��,���������ã�"), lAreaLimitScore, MAX_AREA_TIMES);

		AfxMessageBox(strTip, MB_ICONSTOP);
		return;
	}

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
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED_AREALONG, &CDialogAdvanced::OnBnClickedButtonAdvancedArealong)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED_AREAPING, &CDialogAdvanced::OnBnClickedButtonAdvancedAreaping)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED_AREAHU, &CDialogAdvanced::OnBnClickedButtonAdvancedAreahu)
	ON_BN_CLICKED(IDC_BUTTON_AREACTRL, &CDialogAdvanced::OnBnClickedButtonAreactrl)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_AREACTRL, &CDialogAdvanced::OnBnClickedButtonCancelAreactrl)
	ON_BN_CLICKED(IDC_BUTTON_ADD_FOCUS_PLAYER, &CDialogAdvanced::OnBnClickedButtonAddFocusPlayer)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_FOCUS_PLAYER, &CDialogAdvanced::OnBnClickedButtonDeleteFocusPlayer)
	ON_BN_CLICKED(IDC_CHECK_ADVANCED_HIDE_AI_INSCORE, &CDialogAdvanced::OnBnClickedCheckAdvancedHideAiInscore)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_PLAYER_TYPE, &CDialogAdvanced::OnCbnSelchangeComboSelPlayerType)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_USER_BET, &CDialogAdvanced::OnLvnColumnclickListUserBet)
END_MESSAGE_MAP()

// ���캯��
CDialogAdvanced::CDialogAdvanced(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_ADVANCED, nShowMax, pParent)
{
	ZeroMemory(&m_UserInfo, sizeof(m_UserInfo));
	ZeroMemory(&m_SendUserBetInfo, sizeof(m_SendUserBetInfo));
	memset(m_RoomUserInfo.dwGameID, INVALID_DWORD, sizeof(m_RoomUserInfo.dwGameID));
	ZeroMemory(m_RoomUserInfo.szNickName, sizeof(m_RoomUserInfo.szNickName));

	m_UserBetArray.RemoveAll();
	m_UserFocusArray.RemoveAll();
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
	((CEdit *)GetDlgItem(IDC_EDIT_FOCUS_PLAYER_GAMEID))->LimitText(7);

	//��ǰ��������б�
	CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_USER_BET);
	pListUserBet->SetExtendedStyle(pListUserBet->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	INT nColumnCount = 0;
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��ע���"), LVCFMT_CENTER, 80);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���ID"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("����ǳ�"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("����ע"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���Ͻ��+���д��"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("���д��"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��ǰ��������Ӯ"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("��ʷ����Ӯ"), LVCFMT_CENTER, 120);

	//��ע����б�
	CListCtrl *pListFocusPlayer = (CListCtrl *)GetDlgItem(IDC_LIST_FOCUS_PLAYER);
	pListFocusPlayer->SetExtendedStyle(pListFocusPlayer->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListFocusPlayer->InsertColumn(nColumnCount++, TEXT("���ID"), LVCFMT_CENTER, 110);
	pListFocusPlayer->InsertColumn(nColumnCount++, TEXT("����ǳ�"), LVCFMT_CENTER, 110);

	//���Ծ���
	for (WORD i = 0; i < MAX_EXECUTETIMES_CONFIG; i++)
	{
		CString strCtrlTimes;
		strCtrlTimes.Format(TEXT("%d"), g_cbAreaExecuteTimesArray[i]);
		
		((CComboBox *)GetDlgItem(IDC_COMBO_AREACTRL_TIMES))->AddString(strCtrlTimes);
	}
	((CComboBox *)GetDlgItem(IDC_COMBO_AREACTRL_TIMES))->SetCurSel(0);

	//��ʾɸѡ
	((CComboBox *)GetDlgItem(IDC_COMBO_SEL_PLAYER_TYPE))->AddString(TEXT("�������"));
	((CComboBox *)GetDlgItem(IDC_COMBO_SEL_PLAYER_TYPE))->AddString(TEXT("��ע���"));
	((CComboBox *)GetDlgItem(IDC_COMBO_SEL_PLAYER_TYPE))->SetCurSel(0);

	//ˢ��
	SendDebugMessage(SUB_C_REFRESH_CUR_AREACTRL_INFO);

	//������ע��ʾ
	SetDlgItemInt(IDC_EDIT_ADVANCED_AREALONG_INSCORE, 0);
	SetDlgItemInt(IDC_EDIT_ADVANCED_AREAPING_INSCORE, 0);
	SetDlgItemInt(IDC_EDIT_ADVANCED_AREAHU_INSCORE, 0);

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
	case SUB_S_SEND_ROOM_USER_INFO:
	{
		//Ч������
		ASSERT(nSize == sizeof(CMD_S_RoomUserInfo));
		if (nSize != sizeof(CMD_S_RoomUserInfo))
		{
			return false;
		}

		memset(m_RoomUserInfo.dwGameID, INVALID_DWORD, sizeof(m_RoomUserInfo.dwGameID));
		ZeroMemory(m_RoomUserInfo.szNickName, sizeof(m_RoomUserInfo.szNickName));

		CopyMemory(&m_RoomUserInfo, pData, sizeof(m_RoomUserInfo));

		return true;
	}
	case SUB_S_SEND_USER_BET_INFO:
	{
		//Ч������
		ASSERT(nSize == sizeof(CMD_S_SendUserBetInfo));
		if (nSize != sizeof(CMD_S_SendUserBetInfo))
		{
			return false;
		}

		//�����û��б�
		SendDebugMessage(SUB_GET_ROMM_ALL_USER);

		//��Ϣ����
		CMD_S_SendUserBetInfo *pSendUserBetInfo = (CMD_S_SendUserBetInfo *)pData;

		//������ע��ʾ(ֻ��ʾ������ע)
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREALONG_INSCORE, pSendUserBetInfo->lAreaRealPlayerInAllScore[AREA_LONG]);
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREAPING_INSCORE, pSendUserBetInfo->lAreaRealPlayerInAllScore[AREA_PING]);
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREAHU_INSCORE, pSendUserBetInfo->lAreaRealPlayerInAllScore[AREA_HU]);

		//����AI��ע
		BYTE cbHideAIInscore = ((CButton *)GetDlgItem(IDC_CHECK_ADVANCED_HIDE_AI_INSCORE))->GetCheck();
		
		//��������
		SetDlgItemInt(IDC_STATIC_ADVANCED_ONLINE_PLAYER, (cbHideAIInscore == TRUE ? pSendUserBetInfo->wOnLineRealPlayerCount : pSendUserBetInfo->wOnLineAICount + pSendUserBetInfo->wOnLineRealPlayerCount));

		//��ע����
		SetDlgItemInt(IDC_STATIC_ADVANCED_INSCORE_PLAYER, (cbHideAIInscore == TRUE ? pSendUserBetInfo->wInScoreRealPlayerCount : pSendUserBetInfo->wInScoreAICount + pSendUserBetInfo->wInScoreRealPlayerCount));

		//������ע����
		CopyMemory(&m_SendUserBetInfo, pSendUserBetInfo, sizeof(m_SendUserBetInfo));

		//������ע�б�
		UpdateUserBetList(pSendUserBetInfo->dwCurAllScoreGameID);

		return true;
	}
	case SUB_S_CLEAR_USER_BET_INFO:
	{
		//Ч������
		ASSERT(nSize == 0);
		if (nSize != 0)
		{
			return false;
		}

		//������ע��ʾ
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREALONG_INSCORE, 0);
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREAPING_INSCORE, 0);
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREAHU_INSCORE, 0);

		//��������
		SetDlgItemInt(IDC_STATIC_ADVANCED_ONLINE_PLAYER, 0);

		//��ע����
		SetDlgItemInt(IDC_STATIC_ADVANCED_INSCORE_PLAYER, 0);

		//ɾ����ע�б�
		CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_USER_BET);
		pListUserBet->DeleteAllItems();

		m_UserBetArray.RemoveAll();
		ZeroMemory(&m_SendUserBetInfo, sizeof(m_SendUserBetInfo));

		return true;
	}
	case SUB_S_CUR_AREACTRL_INFO:
	{
		//У���С
		ASSERT(nSize % sizeof(CMD_S_CurAreaCtrlInfo) == 0);
		if (nSize % sizeof(CMD_S_CurAreaCtrlInfo) != 0)
		{
			return false;
		}

		if (nSize == 0)
		{
			SetDlgItemText(IDC_STATIC_AREACTRL_ID, TEXT(""));
			SetDlgItemText(IDC_STATIC_AREACTRL_POS, TEXT(""));
			SetDlgItemText(IDC_STATIC_AREACTRL_CONFIGCOUNT, TEXT(""));
			SetDlgItemText(IDC_STATIC_AREACTRL_CURCOUNT, TEXT(""));
		}
		else
		{
			// ������Ϣ
			CMD_S_CurAreaCtrlInfo *pCurAreaCtrlInfo = (CMD_S_CurAreaCtrlInfo *)pData;

			//���
			if (pCurAreaCtrlInfo->cbConfigCtrlTimes == pCurAreaCtrlInfo->cbCurCtrlTimes)
			{
				SetDlgItemText(IDC_STATIC_AREACTRL_ID, TEXT(""));
				SetDlgItemText(IDC_STATIC_AREACTRL_POS, TEXT(""));
				SetDlgItemText(IDC_STATIC_AREACTRL_CONFIGCOUNT, TEXT(""));
				SetDlgItemText(IDC_STATIC_AREACTRL_CURCOUNT, TEXT(""));

				CString strSysWin;
				strSysWin.Format(SCORE_STRING, pCurAreaCtrlInfo->lAreaCtrlSysWin);
				SetDlgItemText(IDC_STATIC_CUR_AREACTRL_SYSWIN, strSysWin);

				//
				for (BYTE cbAreaIndex = 0; cbAreaIndex < AREA_MAX + 2; cbAreaIndex++)
				{
					((CButton *)GetDlgItem(IDC_RADIO_AREACTRL_LONG + cbAreaIndex))->SetCheck(FALSE);
				}

				((CComboBox *)GetDlgItem(IDC_COMBO_AREACTRL_TIMES))->SetCurSel(0);
			}
			else
			{
				SetDlgItemInt(IDC_STATIC_AREACTRL_ID, pCurAreaCtrlInfo->dwCtrlIndex);
				SetDlgItemText(IDC_STATIC_AREACTRL_POS, GetAreaCtrlPosDesc(pCurAreaCtrlInfo->cbCtrlPosIndex));
				SetDlgItemInt(IDC_STATIC_AREACTRL_CONFIGCOUNT, pCurAreaCtrlInfo->cbConfigCtrlTimes);
				SetDlgItemInt(IDC_STATIC_AREACTRL_CURCOUNT, pCurAreaCtrlInfo->cbCurCtrlTimes);

				CString strSysWin;
				strSysWin.Format(SCORE_STRING, pCurAreaCtrlInfo->lAreaCtrlSysWin);
				SetDlgItemText(IDC_STATIC_CUR_AREACTRL_SYSWIN, strSysWin);

				//
				for (WORD i = 0; i < AREA_MAX + 2; i++)
				{
					((CButton *)GetDlgItem(IDC_RADIO_AREACTRL_LONG + i))->SetCheck(FALSE);
				}

				((CButton *)GetDlgItem(IDC_RADIO_AREACTRL_LONG + pCurAreaCtrlInfo->cbCtrlPosIndex))->SetCheck(TRUE);

				for (WORD i = 0; i < MAX_EXECUTE_TIMES; i++)
				{
					if (g_cbAreaExecuteTimesArray[i] == pCurAreaCtrlInfo->cbConfigCtrlTimes)
					{
						((CComboBox *)GetDlgItem(IDC_COMBO_AREACTRL_TIMES))->SetCurSel(i);

						break;
					}
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

//ȡ���û���Ϣ
int CDialogAdvanced::GetUserInfoIndex(WORD wChairID, WORD wTableID)
{
	for (INT i = 0; i < m_UserInfo.wUserCount; i++)
	{
		if (m_UserInfo.wTableID[i] == wTableID && m_UserInfo.wChairID[i] == wChairID)
		{
			return i;
		}
	}

	return MAX_CHAIR;
}

//������ע�б�
void CDialogAdvanced::UpdateUserBetList(DWORD dwCurAllScoreGameID)
{
	//����AI��ע
	BYTE cbHideAIInscore = ((CButton *)GetDlgItem(IDC_CHECK_ADVANCED_HIDE_AI_INSCORE))->GetCheck();

	//�������
	BYTE cbAllPlayer = ((CComboBox *)GetDlgItem(IDC_COMBO_SEL_PLAYER_TYPE))->GetCurSel() == 0 ? TRUE : FALSE;

	//��ע�б�
	CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_USER_BET);

	//�л�ˢ��
	if (dwCurAllScoreGameID == INVALID_DWORD)
	{
		pListUserBet->DeleteAllItems();

		//��ע����ɸѡ
		CUserBetArray *pUserBetArray = &m_UserBetArray;
		pUserBetArray->RemoveAll();

		for (WORD i = 0; i < CDialogAdvanced::m_wChairCount; i++)
		{
			WORD nUserIndex = GetUserInfoIndex(i);
			if (nUserIndex == MAX_CHAIR)
			{
				continue;
			}

			if (cbHideAIInscore == TRUE && m_UserInfo.bIsAndroid[nUserIndex])
			{
				continue;
			}

			if (cbAllPlayer == FALSE && !IsFocusUser(m_UserInfo.wGameID[nUserIndex]))
			{
				continue;
			}

			SCORE lUserAllBet = 0;
			for (BYTE j = 0; j < AREA_MAX; j++)
			{
				lUserAllBet += m_SendUserBetInfo.lUserJettonScore[i][j];
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
			UserBet.lUserStartScore = m_SendUserBetInfo.lUserStartScore[i];
			UserBet.lUserScore = m_UserInfo.lUserScore[nUserIndex];
			UserBet.lUserInsure = m_UserInfo.lInsureScore[nUserIndex];
			UserBet.lUserWinLost = m_UserInfo.lUserScore[nUserIndex] - UserBet.lUserStartScore;
			UserBet.bFocusUser = IsFocusUser(UserBet.dwUserGameID);

			for (BYTE k = 0; k < AREA_MAX; k++)
			{
				UserBet.lUserBet[k] = m_SendUserBetInfo.lUserJettonScore[i][k];
			}

			//��ʷ����Ӯ
			//UserBet.lUserHistoryWinLost = m_UserInfo.lHistoryKindIDWin[nUserIndex] + UserBet.lUserWinLost;

			//��������
			pUserBetArray->Add(UserBet);
		}

		if (0 == pUserBetArray->GetCount())
		{
			return;
		}

		//��ע�������
		CUserBetArray tempSortUserBetArray;
		tempSortUserBetArray.RemoveAll();

		for (INT i = 0; i < pUserBetArray->GetCount(); i++)
		{
			if (!m_UserBetArray[i].bFocusUser)
			{
				continue;
			}
			
			tempSortUserBetArray.Add(m_UserBetArray[i]);
		}

		for (INT i = 0; i < pUserBetArray->GetCount(); i++)
		{
			if (m_UserBetArray[i].bFocusUser)
			{
				continue;
			}

			tempSortUserBetArray.Add(m_UserBetArray[i]);
		}

		ASSERT(pUserBetArray->GetCount() == tempSortUserBetArray.GetCount());
		
		pUserBetArray->RemoveAll();
		pUserBetArray->Copy(tempSortUserBetArray);

		INT nItemIndex = 0;
		for (INT i = 0; i < pUserBetArray->GetCount(); i++)
		{
			//��������
			pListUserBet->InsertItem(nItemIndex, TEXT(""));

			//��ע��ʶ
			pListUserBet->SetItemText(nItemIndex, 0, m_UserBetArray[i].bFocusUser ? TEXT("*") : TEXT(""));

			CString strInfo;
			strInfo.Format(TEXT("%d"), m_UserBetArray[i].dwUserGameID);
			pListUserBet->SetItemText(nItemIndex, 1, strInfo);

			pListUserBet->SetItemText(nItemIndex, 2, m_UserBetArray[i].szNickName);

			SCORE lUserAllBet = 0;
			for (BYTE j = 0; j < AREA_MAX; j++)
			{
				strInfo.Format(SCORE_STRING, m_UserBetArray[i].lUserBet[j]);
				pListUserBet->SetItemText(nItemIndex, 3 + j, strInfo);

				lUserAllBet += m_UserBetArray[i].lUserBet[j];
			}

			strInfo.Format(SCORE_STRING, lUserAllBet);
			pListUserBet->SetItemText(nItemIndex, 6, strInfo);

			strInfo.Format(SCORE_STRING, m_UserBetArray[i].lUserScore + m_UserBetArray[i].lUserInsure);
			pListUserBet->SetItemText(nItemIndex, 7, strInfo);

			strInfo.Format(SCORE_STRING, m_UserBetArray[i].lUserInsure);
			pListUserBet->SetItemText(nItemIndex, 8, strInfo);

			strInfo.Format(SCORE_STRING, m_UserBetArray[i].lUserWinLost);
			pListUserBet->SetItemText(nItemIndex, 9, strInfo);

			strInfo.Format(SCORE_STRING, m_UserBetArray[i].lUserHistoryWinLost);
			pListUserBet->SetItemText(nItemIndex, 10, strInfo);

			nItemIndex++;
		}
	}
	//���µ�ǰ��ע���
	else
	{
		bool bFindUser = false;
		for (WORD wItemIndex = 0; wItemIndex < pListUserBet->GetItemCount(); wItemIndex++)
		{
			if ((dwCurAllScoreGameID == _ttoi64(pListUserBet->GetItemText(wItemIndex, 1))) && GetUserBet(dwCurAllScoreGameID))
			{
				tagUserBet * ptagUserBet = GetUserBet(dwCurAllScoreGameID);
				WORD ChairID = GetChairIDEx(dwCurAllScoreGameID);

				if (ChairID != INVALID_CHAIR)
				{
					//
					for (BYTE k = 0; k < AREA_MAX; k++)
					{
						ptagUserBet->lUserBet[k] = 0;
						ptagUserBet->lUserBet[k] = m_SendUserBetInfo.lUserJettonScore[ChairID][k];
					}

					bFindUser = true;

					//������ע
					SCORE lUserAllBet = 0;
					CString strInfo;

					for (BYTE j = 0; j < AREA_MAX; j++)
					{
						strInfo.Format(SCORE_STRING, ptagUserBet->lUserBet[j]);
						pListUserBet->SetItemText(wItemIndex, 3 + j, strInfo);

						lUserAllBet += ptagUserBet->lUserBet[j];
					}

					strInfo.Format(SCORE_STRING, lUserAllBet);
					pListUserBet->SetItemText(wItemIndex, 6, strInfo);
				}

				break;
			}
		}

		//δ�ҵ������¼
		if (!bFindUser)
		{
			//��ע����ɸѡ
			CUserBetArray *pUserBetArray = &m_UserBetArray;
			WORD ChairID = GetChairIDEx(dwCurAllScoreGameID);

			if (ChairID == INVALID_CHAIR)
			{
				return;
			}

			//��ȡIndex
			WORD wUserIndex = GetCFUserInfoIndex(dwCurAllScoreGameID);

			if (cbHideAIInscore == TRUE && IsAIUser(dwCurAllScoreGameID))
			{
				return;
			}

			if (cbAllPlayer == FALSE && !IsFocusUser(dwCurAllScoreGameID))
			{
				return;
			}

			SCORE lUserAllBet = 0;
			for (BYTE j = 0; j < AREA_MAX; j++)
			{
				lUserAllBet += m_SendUserBetInfo.lUserJettonScore[ChairID][j];
			}

			if (0 == lUserAllBet)
			{
				return;
			}

			//��������
			tagUserBet UserBet;
			ZeroMemory(&UserBet, sizeof(UserBet));

			CopyMemory(UserBet.szNickName, m_UserInfo.szNickName[wUserIndex], sizeof(UserBet.szNickName));
			UserBet.dwUserGameID = m_UserInfo.wGameID[wUserIndex];
			UserBet.lUserStartScore = m_SendUserBetInfo.lUserStartScore[ChairID];
			UserBet.lUserScore = m_UserInfo.lUserScore[wUserIndex];
			UserBet.lUserInsure = m_UserInfo.lInsureScore[wUserIndex];
			UserBet.lUserWinLost = m_UserInfo.lUserScore[wUserIndex] - UserBet.lUserStartScore;
			UserBet.bFocusUser = IsFocusUser(UserBet.dwUserGameID);

			for (BYTE k = 0; k < AREA_MAX; k++)
			{
				UserBet.lUserBet[k] = m_SendUserBetInfo.lUserJettonScore[ChairID][k];
			}

			//��ʷ����Ӯ
			//UserBet.lUserHistoryWinLost = m_UserInfo.lHistoryKindIDWin[wUserIndex] + UserBet.lUserWinLost;

			//��������
			pUserBetArray->Add(UserBet);	

			//��������
			WORD wItemIndex = /*UserBet.bFocusUser ? 0 : */pListUserBet->GetItemCount();

			//��������
			pListUserBet->InsertItem(wItemIndex, TEXT(""));

			//��ע��ʶ
			pListUserBet->SetItemText(wItemIndex, 0, UserBet.bFocusUser ? TEXT("*") : TEXT(""));

			CString strInfo;
			strInfo.Format(TEXT("%d"), UserBet.dwUserGameID);
			pListUserBet->SetItemText(wItemIndex, 1, strInfo);

			pListUserBet->SetItemText(wItemIndex, 2, UserBet.szNickName);

			lUserAllBet = 0;
			for (BYTE j = 0; j < AREA_MAX; j++)
			{
				strInfo.Format(SCORE_STRING, UserBet.lUserBet[j]);
				pListUserBet->SetItemText(wItemIndex, 3 + j, strInfo);

				lUserAllBet += UserBet.lUserBet[j];
			}

			strInfo.Format(SCORE_STRING, lUserAllBet);
			pListUserBet->SetItemText(wItemIndex, 6, strInfo);

			strInfo.Format(SCORE_STRING, UserBet.lUserScore + UserBet.lUserInsure);
			pListUserBet->SetItemText(wItemIndex, 7, strInfo);

			strInfo.Format(SCORE_STRING, UserBet.lUserInsure);
			pListUserBet->SetItemText(wItemIndex, 8, strInfo);

			strInfo.Format(SCORE_STRING, UserBet.lUserWinLost);
			pListUserBet->SetItemText(wItemIndex, 9, strInfo);

			strInfo.Format(SCORE_STRING, UserBet.lUserHistoryWinLost);
			pListUserBet->SetItemText(wItemIndex, 10, strInfo);
		}
	}
}

//�Ƿ��ע���
bool CDialogAdvanced::IsFocusUser(DWORD dwGameID)
{
	bool bFocusUser = false;

	for (WORD i = 0; i < m_UserFocusArray.GetCount(); i++)
	{
		if (m_UserFocusArray.GetAt(i) == dwGameID)
		{
			bFocusUser = true;

			break;
		}
	}

	return bFocusUser;
}

//�Ƿ�AI
bool CDialogAdvanced::IsAIUser(DWORD dwGameID)
{
	bool bAIUser = false;

	for (WORD i = 0; i < MAX_CHAIR; i++)
	{
		if (m_UserInfo.wGameID[i] == dwGameID)
		{
			bAIUser = m_UserInfo.bIsAndroid[i];

			break;
		}
	}

	return bAIUser;
}

//��ȡ��ע��Ϣ
tagUserBet * CDialogAdvanced::GetUserBet(DWORD dwGameID)
{
	tagUserBet * ptagUserBet = NULL;

	for (WORD i = 0; i < m_UserBetArray.GetCount(); i++)
	{
		ptagUserBet = &m_UserBetArray.GetAt(i);
		if (ptagUserBet->dwUserGameID == dwGameID)
		{
			break;
		}
	}

	return ptagUserBet;
}

//��ȡChairID
WORD CDialogAdvanced::GetChairIDEx(DWORD dwGameID)
{
	WORD wChairID = INVALID_CHAIR;

	for (WORD i = 0; i < MAX_CHAIR; i++)
	{
		if (m_UserInfo.wGameID[i] == dwGameID)
		{
			wChairID = m_UserInfo.wChairID[i];
			break;
		}
	}

	return wChairID;
}

//��ȡIndex
WORD CDialogAdvanced::GetCFUserInfoIndex(DWORD dwGameID)
{
	WORD wIndex = INVALID_WORD;

	for (WORD i = 0; i < MAX_CHAIR; i++)
	{
		if (m_UserInfo.wGameID[i] == dwGameID)
		{
			wIndex = i;
			break;
		}
	}

	return wIndex;
}

//���к���
INT CALLBACK CDialogAdvanced::SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
	//
	return 0;
}

void CDialogAdvanced::OnBnClickedButtonAdvancedArealong()
{	
	CMD_C_AreaCtrl AreaCtrl;
	ZeroMemory(&AreaCtrl, sizeof(AreaCtrl));
	
	AreaCtrl.cbCtrlPosIndex = AREA_LONG;
	AreaCtrl.cbConfigCtrlTimes = 1;
	AreaCtrl.bCancelAreaCtrl = false;
	
	SendDebugMessage(SUB_C_AREA_CTRL, &AreaCtrl, sizeof(AreaCtrl));
}

void CDialogAdvanced::OnBnClickedButtonAdvancedAreaping()
{
	CMD_C_AreaCtrl AreaCtrl;
	ZeroMemory(&AreaCtrl, sizeof(AreaCtrl));

	AreaCtrl.cbCtrlPosIndex = AREA_PING;
	AreaCtrl.cbConfigCtrlTimes = 1;
	AreaCtrl.bCancelAreaCtrl = false;

	SendDebugMessage(SUB_C_AREA_CTRL, &AreaCtrl, sizeof(AreaCtrl));
}

void CDialogAdvanced::OnBnClickedButtonAdvancedAreahu()
{
	CMD_C_AreaCtrl AreaCtrl;
	ZeroMemory(&AreaCtrl, sizeof(AreaCtrl));

	AreaCtrl.cbCtrlPosIndex = AREA_HU;
	AreaCtrl.cbConfigCtrlTimes = 1;
	AreaCtrl.bCancelAreaCtrl = false;

	SendDebugMessage(SUB_C_AREA_CTRL, &AreaCtrl, sizeof(AreaCtrl));
}

void CDialogAdvanced::OnBnClickedButtonAreactrl()
{
	//����ϵͳ��ӮAREA_MAX + 2
	BYTE cbAreaIndex = 0;
	for (cbAreaIndex = 0; cbAreaIndex < AREA_MAX + 2; cbAreaIndex++)
	{
		if (((CButton *)GetDlgItem(IDC_RADIO_AREACTRL_LONG + cbAreaIndex))->GetCheck())
		{
			break;
		}
	}

	CMD_C_AreaCtrl AreaCtrl;
	ZeroMemory(&AreaCtrl, sizeof(AreaCtrl));

	int nSelectTimes = (INT)((CComboBox *)GetDlgItem(IDC_COMBO_AREACTRL_TIMES))->GetCurSel();

	if (cbAreaIndex >= 0 && cbAreaIndex < AREA_MAX + 2 && nSelectTimes >= 0 && nSelectTimes < MAX_EXECUTE_TIMES)
	{
		AreaCtrl.cbCtrlPosIndex = cbAreaIndex;
		AreaCtrl.cbConfigCtrlTimes = g_cbAreaExecuteTimesArray[nSelectTimes];;
		AreaCtrl.bCancelAreaCtrl = false;

		SendDebugMessage(SUB_C_AREA_CTRL, &AreaCtrl, sizeof(AreaCtrl));
	}
	else
	{
		AfxMessageBox(TEXT("��ѡ����Դ����Լ���������!"), MB_ICONSTOP);
	}
}

void CDialogAdvanced::OnBnClickedButtonCancelAreactrl()
{
	CString strAreaCtrlID;
	GetDlgItemText(IDC_STATIC_AREACTRL_ID, strAreaCtrlID);
	strAreaCtrlID.TrimLeft();
	strAreaCtrlID.TrimRight();
	
	if (strAreaCtrlID == TEXT(""))
	{
		AfxMessageBox(TEXT("�������IDΪ��Ч!"), MB_ICONSTOP);

		return;
	}
	
	CMD_C_AreaCtrl AreaCtrl;
	ZeroMemory(&AreaCtrl, sizeof(AreaCtrl));
	
	AreaCtrl.dwCtrlIndex = _ttoi(strAreaCtrlID);
	AreaCtrl.bCancelAreaCtrl = true;
	
	SendDebugMessage(SUB_C_AREA_CTRL, &AreaCtrl, sizeof(AreaCtrl));
}

void CDialogAdvanced::OnBnClickedButtonAddFocusPlayer()
{
	CString strFocusGameID;
	GetDlgItemText(IDC_EDIT_FOCUS_PLAYER_GAMEID, strFocusGameID);
	strFocusGameID.TrimLeft();
	strFocusGameID.TrimRight();

	if (strFocusGameID == TEXT(""))
	{
		AfxMessageBox(TEXT("��ע���GameIDΪ��Ч!"), MB_ICONSTOP);

		return;
	}

	DWORD dwFocusGameID = _ttoi(strFocusGameID);
	TCHAR szFocusNickName[LEN_NICKNAME];
	ZeroMemory(szFocusNickName, sizeof(szFocusNickName));

	bool bValidUser = false;

	for (WORD wChairID = 0; wChairID < CDialogAdvanced::m_wChairCount; wChairID++)
	{
		if (m_RoomUserInfo.dwGameID[wChairID] == dwFocusGameID)
		{
			CopyMemory(szFocusNickName, m_RoomUserInfo.szNickName[wChairID], sizeof(szFocusNickName));
			bValidUser = true;
			break;
		}
	}

	if (!bValidUser)
	{
		AfxMessageBox(TEXT("��ע��Ҳ��ڷ���!"), MB_ICONSTOP);

		return;
	}

	//У���ѹ�ע���
	if (IsFocusUser(dwFocusGameID))
	{
		AfxMessageBox(TEXT("������ѱ���ע!"), MB_ICONSTOP);

		return;
	}

	//��ע�ɹ�
	m_UserFocusArray.Add(dwFocusGameID);

	//��ע�б�
	CListCtrl *pListFocusPlayer = (CListCtrl *)GetDlgItem(IDC_LIST_FOCUS_PLAYER);
	WORD wItemIndex = pListFocusPlayer->GetItemCount();
	
	//��������
	pListFocusPlayer->InsertItem(wItemIndex, TEXT(""));

	CString strGameID;
	strGameID.Format(TEXT("%d"), dwFocusGameID);
	pListFocusPlayer->SetItemText(wItemIndex, 0, strGameID);
	pListFocusPlayer->SetItemText(wItemIndex, 1, szFocusNickName);
}

void CDialogAdvanced::OnBnClickedButtonDeleteFocusPlayer()
{
	CListCtrl *pListFocusPlayer = (CListCtrl *)GetDlgItem(IDC_LIST_FOCUS_PLAYER);
	POSITION pos = pListFocusPlayer->GetFirstSelectedItemPosition();

	if (!pos)
	{
		AfxMessageBox(TEXT("��ѡ��ĳ�У�"), MB_ICONSTOP);

		return;
	}

	int nItem = pListFocusPlayer->GetNextSelectedItem(pos);
	DWORD dwFocusGameID = _ttoi64(pListFocusPlayer->GetItemText(nItem, 0));
	
	bool bValidUser = false;
	
	for (WORD i = 0; i < m_UserFocusArray.GetCount(); i++)
	{
		if (m_UserFocusArray.GetAt(i) == dwFocusGameID)
		{
			bValidUser = true;
			m_UserFocusArray.RemoveAt(i);

			break;
		}
	}

	if (!bValidUser)
	{
		AfxMessageBox(TEXT("��ע�����Ч!"), MB_ICONSTOP);

		return;
	}

	//��ע�б�
	pListFocusPlayer->DeleteItem(nItem);
}

void CDialogAdvanced::OnBnClickedCheckAdvancedHideAiInscore()
{
	//����AI��ע
	BYTE cbHideAIInscore = ((CButton *)GetDlgItem(IDC_CHECK_ADVANCED_HIDE_AI_INSCORE))->GetCheck();

	//��������
	SetDlgItemInt(IDC_STATIC_ADVANCED_ONLINE_PLAYER, (cbHideAIInscore == TRUE ? m_SendUserBetInfo.wOnLineRealPlayerCount : m_SendUserBetInfo.wOnLineAICount + m_SendUserBetInfo.wOnLineRealPlayerCount));

	//��ע����
	SetDlgItemInt(IDC_STATIC_ADVANCED_INSCORE_PLAYER, (cbHideAIInscore == TRUE ? m_SendUserBetInfo.wInScoreRealPlayerCount : m_SendUserBetInfo.wInScoreAICount + m_SendUserBetInfo.wInScoreRealPlayerCount));

	//������ע�б�
	UpdateUserBetList(INVALID_DWORD);
}

void CDialogAdvanced::OnCbnSelchangeComboSelPlayerType()
{
	//������ע�б�
	UpdateUserBetList(INVALID_DWORD);
}

void CDialogAdvanced::OnLvnColumnclickListUserBet(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	WORD wColumnIndex = pNMLV->iSubItem;

	CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_USER_BET);

	//�����б�
	pListUserBet->SortItems(SortFunction, (LPARAM)(&wColumnIndex));
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

	//�������
	CListCtrl *pListAreaCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_AREACTRL_TRACELOG);
	pListAreaCtrlTraceLog->SetExtendedStyle(pListAreaCtrlTraceLog->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("����λ��"), LVCFMT_CENTER, 60);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("���Ծ���"), LVCFMT_CENTER, 60);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("���ͳ��"), LVCFMT_CENTER, 100);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("������"), LVCFMT_CENTER, 60);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 100);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("״̬"), LVCFMT_CENTER, 60);

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
	case SUB_S_REFRESH_AREACTRL_TRACELOG:
	{
		ASSERT(nSize % sizeof(CMD_S_AreaCtrlTraceLog) == 0);
		if (nSize % sizeof(CMD_S_AreaCtrlTraceLog) != 0)
		{
			return false;
		}

		CListCtrl *pListAreaCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_AREACTRL_TRACELOG);

		if (nSize == 0)
		{
			pListAreaCtrlTraceLog->DeleteAllItems();
		}
		else
		{
			//������Ϣ
			CMD_S_AreaCtrlTraceLog *pAreaCtrlResult = (CMD_S_AreaCtrlTraceLog *)pData;

			int nItemIndex = -1;
			for (WORD wIndex = 0; wIndex<pListAreaCtrlTraceLog->GetItemCount(); wIndex++)
			{
				if (_ttoi64(pListAreaCtrlTraceLog->GetItemText(wIndex, 0)) == pAreaCtrlResult->AreaCtrlTraceLog.dwCtrlIndex)
				{
					nItemIndex = wIndex;
					break;
				}
			}

			if (nItemIndex == -1)
			{
				CString strCtrlIndex;
				strCtrlIndex.Format(TEXT("%d"), pAreaCtrlResult->AreaCtrlTraceLog.dwCtrlIndex);

				int nItem = pListAreaCtrlTraceLog->GetItemCount();
				nItemIndex = pListAreaCtrlTraceLog->InsertItem(nItem, strCtrlIndex);

				pListAreaCtrlTraceLog->SetItemText(nItem, 1, GetAreaCtrlPosDesc(pAreaCtrlResult->AreaCtrlTraceLog.cbCtrlPosIndex));
				
				CString strCtrlVal;
				strCtrlVal.Format(TEXT("%d"), pAreaCtrlResult->AreaCtrlTraceLog.cbConfigCtrlTimes);
				pListAreaCtrlTraceLog->SetItemText(nItem, 2, strCtrlVal);

				strCtrlVal.Format(SCORE_STRING, pAreaCtrlResult->AreaCtrlTraceLog.lAreaCtrlSysWin);
				pListAreaCtrlTraceLog->SetItemText(nItem, 3, strCtrlVal);

				//������
				strCtrlVal.Format(TEXT("%d"), pAreaCtrlResult->AreaCtrlTraceLog.dwDebugUserGameID);
				pListAreaCtrlTraceLog->SetItemText(nItem, 4, strCtrlVal);

				strCtrlVal.Format(TEXT("%d-%d,%d:%d"), pAreaCtrlResult->AreaCtrlTraceLog.tmConfigTime.GetMonth(), pAreaCtrlResult->AreaCtrlTraceLog.tmConfigTime.GetDay(), pAreaCtrlResult->AreaCtrlTraceLog.tmConfigTime.GetHour(), pAreaCtrlResult->AreaCtrlTraceLog.tmConfigTime.GetMinute());
				pListAreaCtrlTraceLog->SetItemText(nItem, 5, strCtrlVal);

				if (pAreaCtrlResult->AreaCtrlTraceLog.areaCtrlStatus == AREACTRL_PROGRESSING)
				{
					pListAreaCtrlTraceLog->SetItemText(nItem, 6, TEXT("������"));
				}
				else if (pAreaCtrlResult->AreaCtrlTraceLog.areaCtrlStatus == AREACTRL_FINISH)
				{
					pListAreaCtrlTraceLog->SetItemText(nItem, 6, TEXT("���"));
				}
				else if (pAreaCtrlResult->AreaCtrlTraceLog.areaCtrlStatus == AREACTRL_CANCEL)
				{
					pListAreaCtrlTraceLog->SetItemText(nItem, 6, TEXT("ȡ��"));
				}
				else if (pAreaCtrlResult->AreaCtrlTraceLog.areaCtrlStatus == AREACTRL_EXECUTE_CANCEL)
				{
					pListAreaCtrlTraceLog->SetItemText(nItem, 6, TEXT("��ִ��"));
				}

				if (pAreaCtrlResult->AreaCtrlTraceLog.areaCtrlStatus == AREACTRL_PROGRESSING)
				{
					pListAreaCtrlTraceLog->SetItemState(nItem, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
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

	CListCtrl *pListAreaCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_AREACTRL_TRACELOG);
	pListAreaCtrlTraceLog->DeleteAllItems();
}


