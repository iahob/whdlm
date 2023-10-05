// CoinDozerConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientConfigDlg.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////
BYTE g_cbAreaExecuteTimesArray[MAX_EXECUTE_TIMES] = { 1, 3, 5, 10, 15 };

// 调试对话框
IMPLEMENT_DYNAMIC(CClientConfigDlg, CDialog)
BEGIN_MESSAGE_MAP(CClientConfigDlg, CDialog)
	ON_WM_DESTROY()
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTabOptisons)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

// 构造函数
CClientConfigDlg::CClientConfigDlg(CWnd *pParent /*=NULL*/) : CDialog(IDD_DIALOG_MAIN, pParent)
{
    // 设置变量
    m_pParentWnd = NULL;
    m_pIClientDebugCallback = NULL;
}

// 析构函数
CClientConfigDlg::~CClientConfigDlg()
{
}

// 窗口销毁
void CClientConfigDlg::OnDestroy()
{
    // 销毁窗口
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        m_DialogCustom[nIndex]->DestroyWindow();
        delete m_DialogCustom[nIndex];
    }

    CDialog::OnDestroy();
}

// 控件绑定
VOID CClientConfigDlg::DoDataExchange(CDataExchange *pDX)
{
    __super::DoDataExchange(pDX);
}

// 释放接口
void CClientConfigDlg::Release()
{
    delete this;
}

// 创建函数
bool CClientConfigDlg::Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback)
{
    // 设置变量
    m_pParentWnd = pParentWnd;
    m_pIClientDebugCallback = pIClientDebugCallback;

    // 创建窗口
    __super::Create(IDD_DIALOG_MAIN, pParentWnd);

    // 获取分页
    CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);

    // 设置选项
    pTabCtrl->InsertItem(0, _T("配置设置"));
	pTabCtrl->InsertItem(1, _T("区域调试"));
	pTabCtrl->InsertItem(2, _T("日志记录"));
    pTabCtrl->SetCurSel(0);

    // 获取分页
    CRect RectWindow;
    CWnd *pWindowShow = GetDlgItem(IDC_STATIC_SHOW);
    pWindowShow->ShowWindow(SW_HIDE);
    pWindowShow->GetWindowRect(RectWindow);
    ScreenToClient(RectWindow);

    // 创建窗口
    m_DialogCustom[CUSTOM_GENERAL] = new CDialogGeneral(RectWindow.Height(), this);
    m_DialogCustom[CUSTOM_GENERAL]->Create(IDD_CUSTOM_GENERAL, this);

	m_DialogCustom[CUSTOM_ADVANCED] = new CDialogAdvanced(RectWindow.Height(), this);
	m_DialogCustom[CUSTOM_ADVANCED]->Create(IDD_CUSTOM_ADVANCED, this);

	m_DialogCustom[CUSTOM_TRACELOG] = new CDialogTraceLog(RectWindow.Height(), this);
	m_DialogCustom[CUSTOM_TRACELOG]->Create(IDD_CUSTOM_TRACELOG, this);

    // 窗口位置
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        CRect RectCustom;
        CSize SizeCustom;
        m_DialogCustom[nIndex]->GetClientRect(RectCustom);
        SizeCustom.SetSize(min(RectWindow.Width(), RectCustom.Width()), min(RectWindow.Height(), RectCustom.Height()));

		m_DialogCustom[nIndex]->SetWindowPos(NULL, RectWindow.left + RectWindow.Width() / 2 - SizeCustom.cx / 2, RectWindow.top, SizeCustom.cx, SizeCustom.cy, SWP_NOZORDER);
        m_DialogCustom[nIndex]->ShowWindow((nIndex == 0) ? SW_SHOW : SW_HIDE);
    }

    // 关闭窗口
    __super::ShowWindow(SW_HIDE);

    return true;

}

// 显示窗口
bool CClientConfigDlg::ShowWindow(bool bShow)
{
    // 显示窗口
    __super::ShowWindow(bShow ? SW_SHOW : SW_HIDE);

    // 显示窗口
    if(bShow && m_pParentWnd)
    {
        // 获取父窗口
        CRect RectParent;
        m_pParentWnd->GetWindowRect(RectParent);

        // 获取当前窗口
        CRect RectWindow;
        GetWindowRect(RectWindow);

        // 移动位置
        SetWindowPos(NULL, RectParent.left + RectParent.Width() / 2 - RectWindow.Width() / 2, RectParent.top + RectParent.Height() / 2 - RectWindow.Height() / 2, 0, 0, SWP_NOSIZE);
    }

    return true;
}

// 消息函数
bool CClientConfigDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
    // 无效接口
    if(m_pIClientDebugCallback == NULL)
    {
        return false;
    }

    // 判断窗口
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        if(m_DialogCustom[nIndex]->OnDebugMessage(nMessageID, wTableID, pData, nSize))
        {
            return true;
        }
    }

    return true;
}

// 调试信息
bool CClientConfigDlg::SendDebugMessage(UINT nMessageID, void *pData, UINT nSize)
{
    if(m_pIClientDebugCallback != NULL)
    {
        // 发送消息
        m_pIClientDebugCallback->OnDebugInfo(nMessageID, 0, pData, nSize);
    }
    return true;
}

// 调试信息
bool CClientConfigDlg::SendDebugMessage(UINT nMessageID, WORD wTableID, void *pData, UINT nSize)
{
	if (m_pIClientDebugCallback != NULL)
	{
		// 发送消息
		m_pIClientDebugCallback->OnDebugInfo(nMessageID, wTableID, pData, nSize);
	}
	return true;
}

// 变换选项
void CClientConfigDlg::OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult)
{
    // 获取分页
    CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);

    // 获取选择项目
    int nCurSel = pTabCtrl->GetCurSel();
    if(nCurSel >= 0 && nCurSel < MAX_CUSTOM)
    {
        // 判断窗口
        for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
        {
            m_DialogCustom[nIndex]->ShowWindow((nIndex == nCurSel) ? SW_SHOW : SW_HIDE);
        }

		if (nCurSel == CUSTOM_TRACELOG)
		{
			//控制控制列表
			m_DialogCustom[nCurSel]->CleanCtrlList();

			SendDebugMessage(SUB_C_REFRESH_TRACELOG);
		}
    }

    *pResult = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 调试对话框
BEGIN_MESSAGE_MAP(CDialogGeneral, CDialogDexter)
	ON_BN_CLICKED(IDC_BUTTON_ANCHOU, &CDialogGeneral::OnBnClickedButtonAnchou)
	ON_BN_CLICKED(IDC_BUTTON_MOSGOLD, &CDialogGeneral::OnBnClickedButtonMosgold)
	ON_BN_CLICKED(IDC_BUTTON_ROOMCTRL, &CDialogGeneral::OnBnClickedButtonRoomctrl)
	ON_BN_CLICKED(IDC_BUTTON_SYSCTRL, &CDialogGeneral::OnBnClickedButtonSysctrl)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_ROOMCTRL, &CDialogGeneral::OnBnClickedButtonCancelRoomctrl)
END_MESSAGE_MAP()

// 构造函数
CDialogGeneral::CDialogGeneral(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_GENERAL, nShowMax, pParent)
{
	m_lAreaLimitScore = 0;
}

// 析构函数
CDialogGeneral::~CDialogGeneral()
{
}

// 初始化窗口
BOOL CDialogGeneral::OnInitDialog()
{
	// 初始化窗口
	__super::OnInitDialog();

	//限制输入
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

	//当前房间控制列表
	CListCtrl *pListCurRoomCtrlInfo = (CListCtrl *)GetDlgItem(IDC_LIST_CUR_ROOMCTRL_INFO);
	pListCurRoomCtrlInfo->SetExtendedStyle(pListCurRoomCtrlInfo->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	INT nColumnCount = 0;
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("调试ID"), LVCFMT_CENTER, 60);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("配置系统库存"), LVCFMT_CENTER, 100);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("配置玩家库存"), LVCFMT_CENTER, 100);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("当前系统库存"), LVCFMT_CENTER, 100);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("当前玩家库存"), LVCFMT_CENTER, 100);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("调节系数"), LVCFMT_CENTER, 60);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("中奖率"), LVCFMT_CENTER, 60);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("库存统计"), LVCFMT_CENTER, 100);
	pListCurRoomCtrlInfo->InsertColumn(nColumnCount++, TEXT("当前状态"), LVCFMT_CENTER, 60);

	//刷新
	SendDebugMessage(SUB_C_REFRESH_STATISTIC_SERVICE);
	SendDebugMessage(SUB_C_REFRESH_ANCHOU_MOSGOLD);
	SendDebugMessage(SUB_C_REFRESH_CUR_SYSCTRL_INFO);
	SendDebugMessage(SUB_C_REFRESH_CUR_ROOMCTRL_INFO);
	SendDebugMessage(SUB_C_REFRESH_LIST_CUR_ROOMCTRL_INFO);
	
	return TRUE;
}

// 控件绑定
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

		//赋值区域限制
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
			//定义信息
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
					pListCurRoomCtrlInfo->SetItemText(nItem, 8, TEXT("队列"));
				}
				else if ( pCurRoomCtrlInfo->curRoomCtrlListInfo.roomCtrlStatus == PROGRESSING)
				{
					pListCurRoomCtrlInfo->SetItemText(nItem, 8, TEXT("进行中"));
				}
				else if ( pCurRoomCtrlInfo->curRoomCtrlListInfo.roomCtrlStatus == FINISH)
				{
					pListCurRoomCtrlInfo->SetItemText(nItem, 8, TEXT("完成"));
				}
				else if ( pCurRoomCtrlInfo->curRoomCtrlListInfo.roomCtrlStatus == CANCEL)
				{
					pListCurRoomCtrlInfo->SetItemText(nItem, 8, TEXT("取消"));
				}
				else if ( pCurRoomCtrlInfo->curRoomCtrlListInfo.roomCtrlStatus == EXECUTE_CANCEL)
				{
					pListCurRoomCtrlInfo->SetItemText(nItem, 8, TEXT("已执行"));
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
		//校验大小
		ASSERT(sizeof(CMD_S_DebugText) == wDataSize);
		if (sizeof(CMD_S_DebugText) != wDataSize)
		{
			return false;
		}

		// 定义信息
		CMD_S_DebugText *pDebugText = (CMD_S_DebugText *)pBuffer;

		MessageBox(pDebugText->szMessageText, TEXT("温馨提示"));

		return true;
	}
	default:
		break;
	}

	return false;
}

//获取中奖率
WORD CDialogGeneral::GetWinRate(SCORE lSysStorage, SCORE lPlayerStorage, SCORE lParameterK)
{
	//小于0当成0算概率
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
		//区间概率判断
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

	//判空校验
	if (strConfigAnChouRate == TEXT(""))
	{
		AfxMessageBox(TEXT("抽水比例不能为空，请重新设置！"), MB_ICONSTOP);
		return;
	}

	//配置暗抽
	CMD_C_AnChou AnChou;
	ZeroMemory(&AnChou, sizeof(AnChou));
	
	AnChou.lConfigAnChouRate = GetDlgItemInt(IDC_EDIT_CONFIG_ANCHOU_RATE);
	
	//校验
	if (!(AnChou.lConfigAnChouRate >= 0 && AnChou.lConfigAnChouRate <= 30))
	{
		AfxMessageBox(TEXT("抽水比例范围0~30，请重新设置！"), MB_ICONSTOP);
		return;
	}

	// 发送消息
	SendDebugMessage(SUB_C_ANCHOU, &AnChou, sizeof(AnChou));
}

void CDialogGeneral::OnBnClickedButtonMosgold()
{
	CString strConfigMosgoldRate;
	GetDlgItemText(IDC_EDIT_CONFIG_MOSGOLD_RATE, strConfigMosgoldRate);

	//判空校验
	if (strConfigMosgoldRate == TEXT(""))
	{
		AfxMessageBox(TEXT("大奖抽取比例不能为空，请重新设置！"), MB_ICONSTOP);
		return;
	}

	//配置大奖
	CMD_C_Mosgold Mosgold;
	ZeroMemory(&Mosgold, sizeof(Mosgold));

	Mosgold.lConfigMosgoldRate = GetDlgItemInt(IDC_EDIT_CONFIG_MOSGOLD_RATE);
	Mosgold.lConfigMosgoldDispatchCon = 0/*(SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_MOSGOLDSTORAGE_RATE)*/;
	Mosgold.lConfigMosgoldDispatchRate = GetDlgItemInt(IDC_EDIT_CONFIG_MOSGOLD_DISRATE);
	Mosgold.lConfigMosgoldStorageRate = GetDlgItemInt(IDC_EDIT_CONFIG_MOSGOLDSTORAGE_RATE);

	//校验
	if (!(Mosgold.lConfigMosgoldRate >= 0 && Mosgold.lConfigMosgoldRate <= 30))
	{
		AfxMessageBox(TEXT("大奖抽取比例范围0~30，请重新设置！"), MB_ICONSTOP);
		return;
	}

	//校验
	if (!(Mosgold.lConfigMosgoldDispatchRate > 0 && Mosgold.lConfigMosgoldDispatchRate <= 100))
	{
		AfxMessageBox(TEXT("派奖概率范围1~100，请重新设置！"), MB_ICONSTOP);
		return;
	}

	//校验
	if (!(Mosgold.lConfigMosgoldStorageRate > 0 && Mosgold.lConfigMosgoldStorageRate <= 100))
	{
		AfxMessageBox(TEXT("大奖库存比例范围1~100，请重新设置！"), MB_ICONSTOP);
		return;
	}

	// 发送消息
	SendDebugMessage(SUB_C_MOSGOLD, &Mosgold, sizeof(Mosgold));
}

void CDialogGeneral::OnBnClickedButtonSysctrl()
{
	//系统调试
	CMD_C_SysCtrl SysCtrl;
	ZeroMemory(&SysCtrl, sizeof(SysCtrl));

	SysCtrl.lSysCtrlSysStorage = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_SYSCTRL_SYSSTORAGE);
	SysCtrl.lSysCtrlPlayerStorage = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_SYSCTRL_PLAYERSTORAGE);
	SysCtrl.lSysCtrlParameterK = GetDlgItemInt(IDC_EDIT_CONFIG_SYSCTRL_PARAMETER_K);
	SysCtrl.lSysCtrlSysStorResetRate = GetDlgItemInt(IDC_EDIT_CONFIG_SYSCTRL_SYSSTORAGE_RESETRATE);

	//校验
	if (SysCtrl.lSysCtrlPlayerStorage < m_lAreaLimitScore * MAX_AREA_TIMES)
	{
		LONGLONG lAreaLimitScore = (LONGLONG)m_lAreaLimitScore;

		CString strTip;
		strTip.Format(TEXT("当前区域限制为[ %I64d ],玩家库存必须大于等于区域限制的[ %d ]倍,请重新设置！"), lAreaLimitScore, MAX_AREA_TIMES);

		AfxMessageBox(strTip, MB_ICONSTOP);
		return;
	}

	//校验
	if (!(SysCtrl.lSysCtrlParameterK > 0 && SysCtrl.lSysCtrlParameterK <= 100))
	{
		AfxMessageBox(TEXT("调节系数范围1~100，请重新设置！"), MB_ICONSTOP);
		return;
	}

	//校验
	if (!(SysCtrl.lSysCtrlSysStorResetRate > 0 && SysCtrl.lSysCtrlSysStorResetRate <= 100))
	{
		AfxMessageBox(TEXT("强制重置范围1~100，请重新设置！"), MB_ICONSTOP);
		return;
	}

	// 发送消息
	SendDebugMessage(SUB_C_SYS_CTRL, &SysCtrl, sizeof(SysCtrl));
}

void CDialogGeneral::OnBnClickedButtonRoomctrl()
{
	//房间调试
	CMD_C_RoomCtrl RoomCtrl;
	ZeroMemory(&RoomCtrl, sizeof(RoomCtrl));

	RoomCtrl.dwSelCtrlIndex = INVALID_DWORD;
	RoomCtrl.lRoomCtrlCurSysStorage = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_ROOMCTRL_SYSSTORAGE);
	RoomCtrl.lRoomCtrlCurPlayerStorage = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_ROOMCTRL_PLAYERSTORAGE);
	RoomCtrl.lRoomCtrlParameterK = GetDlgItemInt(IDC_EDIT_CONFIG_ROOMCTRL_PARAMETER_K);
	RoomCtrl.lRoomCtrlSysStorResetRate = GetDlgItemInt(IDC_EDIT_CONFIG_ROOMCTRL_SYSSTORAGE_RESETRATE);
	RoomCtrl.bCancelRoomCtrl = false;

	//校验
	if (RoomCtrl.lRoomCtrlCurPlayerStorage < m_lAreaLimitScore * MAX_AREA_TIMES)
	{
		LONGLONG lAreaLimitScore = (LONGLONG)m_lAreaLimitScore;

		CString strTip;
		strTip.Format(TEXT("当前区域限制为[ %I64d ],玩家库存必须大于等于区域限制的[ %d ]倍,请重新设置！"), lAreaLimitScore, MAX_AREA_TIMES);

		AfxMessageBox(strTip, MB_ICONSTOP);
		return;
	}

	//校验
	if (!(RoomCtrl.lRoomCtrlParameterK > 0 && RoomCtrl.lRoomCtrlParameterK <= 100))
	{
		AfxMessageBox(TEXT("调节系数范围1~100，请重新设置！"), MB_ICONSTOP);
		return;
	}

	//校验
	if (!(RoomCtrl.lRoomCtrlSysStorResetRate > 0 && RoomCtrl.lRoomCtrlSysStorResetRate <= 100))
	{
		AfxMessageBox(TEXT("强制重置范围1~100，请重新设置！"), MB_ICONSTOP);
		return;
	}

	// 发送消息
	SendDebugMessage(SUB_C_ROOM_CTRL, &RoomCtrl, sizeof(RoomCtrl));
}

void CDialogGeneral::OnBnClickedButtonCancelRoomctrl()
{
	CListCtrl *pListCurRoomCtrlInfo = (CListCtrl *)GetDlgItem(IDC_LIST_CUR_ROOMCTRL_INFO);
	POSITION pos = pListCurRoomCtrlInfo->GetFirstSelectedItemPosition();

	if (!pos)
	{
		AfxMessageBox(TEXT("请选中某行！"), MB_ICONSTOP);

		return;
	}

	int nItem = pListCurRoomCtrlInfo->GetNextSelectedItem(pos);

	//房间调试
	CMD_C_RoomCtrl RoomCtrl;
	ZeroMemory(&RoomCtrl, sizeof(RoomCtrl));

	RoomCtrl.dwSelCtrlIndex = _ttoi64(pListCurRoomCtrlInfo->GetItemText(nItem, 0));;
	RoomCtrl.bCancelRoomCtrl = true;

	// 发送消息
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

// 构造函数
CDialogAdvanced::CDialogAdvanced(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_ADVANCED, nShowMax, pParent)
{
	ZeroMemory(&m_UserInfo, sizeof(m_UserInfo));
	ZeroMemory(&m_SendUserBetInfo, sizeof(m_SendUserBetInfo));
	memset(m_RoomUserInfo.dwGameID, INVALID_DWORD, sizeof(m_RoomUserInfo.dwGameID));
	ZeroMemory(m_RoomUserInfo.szNickName, sizeof(m_RoomUserInfo.szNickName));

	m_UserBetArray.RemoveAll();
	m_UserFocusArray.RemoveAll();
}

// 析构函数
CDialogAdvanced::~CDialogAdvanced()
{
}

// 初始化窗口
BOOL CDialogAdvanced::OnInitDialog()
{
	// 初始化窗口
	__super::OnInitDialog();

	//限制输入
	((CEdit *)GetDlgItem(IDC_EDIT_FOCUS_PLAYER_GAMEID))->LimitText(7);

	//当前房间控制列表
	CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_USER_BET);
	pListUserBet->SetExtendedStyle(pListUserBet->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	INT nColumnCount = 0;
	pListUserBet->InsertColumn(nColumnCount++, TEXT("关注玩家"), LVCFMT_CENTER, 80);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("玩家ID"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("玩家昵称"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("龙"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("和"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("虎"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("总下注"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("身上金币+银行存款"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("银行存款"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("当前房间总输赢"), LVCFMT_CENTER, 120);
	pListUserBet->InsertColumn(nColumnCount++, TEXT("历史总输赢"), LVCFMT_CENTER, 120);

	//关注玩家列表
	CListCtrl *pListFocusPlayer = (CListCtrl *)GetDlgItem(IDC_LIST_FOCUS_PLAYER);
	pListFocusPlayer->SetExtendedStyle(pListFocusPlayer->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListFocusPlayer->InsertColumn(nColumnCount++, TEXT("玩家ID"), LVCFMT_CENTER, 110);
	pListFocusPlayer->InsertColumn(nColumnCount++, TEXT("玩家昵称"), LVCFMT_CENTER, 110);

	//调试局数
	for (WORD i = 0; i < MAX_EXECUTETIMES_CONFIG; i++)
	{
		CString strCtrlTimes;
		strCtrlTimes.Format(TEXT("%d"), g_cbAreaExecuteTimesArray[i]);
		
		((CComboBox *)GetDlgItem(IDC_COMBO_AREACTRL_TIMES))->AddString(strCtrlTimes);
	}
	((CComboBox *)GetDlgItem(IDC_COMBO_AREACTRL_TIMES))->SetCurSel(0);

	//显示筛选
	((CComboBox *)GetDlgItem(IDC_COMBO_SEL_PLAYER_TYPE))->AddString(TEXT("所有玩家"));
	((CComboBox *)GetDlgItem(IDC_COMBO_SEL_PLAYER_TYPE))->AddString(TEXT("关注玩家"));
	((CComboBox *)GetDlgItem(IDC_COMBO_SEL_PLAYER_TYPE))->SetCurSel(0);

	//刷新
	SendDebugMessage(SUB_C_REFRESH_CUR_AREACTRL_INFO);

	//区域下注显示
	SetDlgItemInt(IDC_EDIT_ADVANCED_AREALONG_INSCORE, 0);
	SetDlgItemInt(IDC_EDIT_ADVANCED_AREAPING_INSCORE, 0);
	SetDlgItemInt(IDC_EDIT_ADVANCED_AREAHU_INSCORE, 0);

	return TRUE;
}

// 控件绑定
void CDialogAdvanced::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);
}

// 消息函数
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
		//效验数据
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
		//效验数据
		ASSERT(nSize == sizeof(CMD_S_SendUserBetInfo));
		if (nSize != sizeof(CMD_S_SendUserBetInfo))
		{
			return false;
		}

		//更新用户列表
		SendDebugMessage(SUB_GET_ROMM_ALL_USER);

		//消息处理
		CMD_S_SendUserBetInfo *pSendUserBetInfo = (CMD_S_SendUserBetInfo *)pData;

		//区域下注显示(只显示真人下注)
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREALONG_INSCORE, pSendUserBetInfo->lAreaRealPlayerInAllScore[AREA_LONG]);
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREAPING_INSCORE, pSendUserBetInfo->lAreaRealPlayerInAllScore[AREA_PING]);
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREAHU_INSCORE, pSendUserBetInfo->lAreaRealPlayerInAllScore[AREA_HU]);

		//隐藏AI下注
		BYTE cbHideAIInscore = ((CButton *)GetDlgItem(IDC_CHECK_ADVANCED_HIDE_AI_INSCORE))->GetCheck();
		
		//在线人数
		SetDlgItemInt(IDC_STATIC_ADVANCED_ONLINE_PLAYER, (cbHideAIInscore == TRUE ? pSendUserBetInfo->wOnLineRealPlayerCount : pSendUserBetInfo->wOnLineAICount + pSendUserBetInfo->wOnLineRealPlayerCount));

		//下注人数
		SetDlgItemInt(IDC_STATIC_ADVANCED_INSCORE_PLAYER, (cbHideAIInscore == TRUE ? pSendUserBetInfo->wInScoreRealPlayerCount : pSendUserBetInfo->wInScoreAICount + pSendUserBetInfo->wInScoreRealPlayerCount));

		//拷贝下注数据
		CopyMemory(&m_SendUserBetInfo, pSendUserBetInfo, sizeof(m_SendUserBetInfo));

		//更新下注列表
		UpdateUserBetList(pSendUserBetInfo->dwCurAllScoreGameID);

		return true;
	}
	case SUB_S_CLEAR_USER_BET_INFO:
	{
		//效验数据
		ASSERT(nSize == 0);
		if (nSize != 0)
		{
			return false;
		}

		//区域下注显示
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREALONG_INSCORE, 0);
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREAPING_INSCORE, 0);
		SetDlgItemInt(IDC_EDIT_ADVANCED_AREAHU_INSCORE, 0);

		//在线人数
		SetDlgItemInt(IDC_STATIC_ADVANCED_ONLINE_PLAYER, 0);

		//下注人数
		SetDlgItemInt(IDC_STATIC_ADVANCED_INSCORE_PLAYER, 0);

		//删除下注列表
		CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_USER_BET);
		pListUserBet->DeleteAllItems();

		m_UserBetArray.RemoveAll();
		ZeroMemory(&m_SendUserBetInfo, sizeof(m_SendUserBetInfo));

		return true;
	}
	case SUB_S_CUR_AREACTRL_INFO:
	{
		//校验大小
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
			// 定义信息
			CMD_S_CurAreaCtrlInfo *pCurAreaCtrlInfo = (CMD_S_CurAreaCtrlInfo *)pData;

			//清空
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

//取得用户信息
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

//更新下注列表
void CDialogAdvanced::UpdateUserBetList(DWORD dwCurAllScoreGameID)
{
	//隐藏AI下注
	BYTE cbHideAIInscore = ((CButton *)GetDlgItem(IDC_CHECK_ADVANCED_HIDE_AI_INSCORE))->GetCheck();

	//所有玩家
	BYTE cbAllPlayer = ((CComboBox *)GetDlgItem(IDC_COMBO_SEL_PLAYER_TYPE))->GetCurSel() == 0 ? TRUE : FALSE;

	//下注列表
	CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_USER_BET);

	//切换刷新
	if (dwCurAllScoreGameID == INVALID_DWORD)
	{
		pListUserBet->DeleteAllItems();

		//下注数组筛选
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

			//构造数据
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

			//历史总输赢
			//UserBet.lUserHistoryWinLost = m_UserInfo.lHistoryKindIDWin[nUserIndex] + UserBet.lUserWinLost;

			//插入数据
			pUserBetArray->Add(UserBet);
		}

		if (0 == pUserBetArray->GetCount())
		{
			return;
		}

		//关注玩家排序
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
			//插入数据
			pListUserBet->InsertItem(nItemIndex, TEXT(""));

			//关注标识
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
	//更新当前下注玩家
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

					//更新下注
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

		//未找到插入记录
		if (!bFindUser)
		{
			//下注数组筛选
			CUserBetArray *pUserBetArray = &m_UserBetArray;
			WORD ChairID = GetChairIDEx(dwCurAllScoreGameID);

			if (ChairID == INVALID_CHAIR)
			{
				return;
			}

			//获取Index
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

			//构造数据
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

			//历史总输赢
			//UserBet.lUserHistoryWinLost = m_UserInfo.lHistoryKindIDWin[wUserIndex] + UserBet.lUserWinLost;

			//插入数据
			pUserBetArray->Add(UserBet);	

			//插入索引
			WORD wItemIndex = /*UserBet.bFocusUser ? 0 : */pListUserBet->GetItemCount();

			//插入数据
			pListUserBet->InsertItem(wItemIndex, TEXT(""));

			//关注标识
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

//是否关注玩家
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

//是否AI
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

//获取下注信息
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

//获取ChairID
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

//获取Index
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

//排列函数
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
	//包括系统输赢AREA_MAX + 2
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
		AfxMessageBox(TEXT("请选择调试次数以及调试区域!"), MB_ICONSTOP);
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
		AfxMessageBox(TEXT("区域调试ID为无效!"), MB_ICONSTOP);

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
		AfxMessageBox(TEXT("关注玩家GameID为无效!"), MB_ICONSTOP);

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
		AfxMessageBox(TEXT("关注玩家不在房间!"), MB_ICONSTOP);

		return;
	}

	//校验已关注玩家
	if (IsFocusUser(dwFocusGameID))
	{
		AfxMessageBox(TEXT("该玩家已被关注!"), MB_ICONSTOP);

		return;
	}

	//关注成功
	m_UserFocusArray.Add(dwFocusGameID);

	//关注列表
	CListCtrl *pListFocusPlayer = (CListCtrl *)GetDlgItem(IDC_LIST_FOCUS_PLAYER);
	WORD wItemIndex = pListFocusPlayer->GetItemCount();
	
	//插入数据
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
		AfxMessageBox(TEXT("请选中某行！"), MB_ICONSTOP);

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
		AfxMessageBox(TEXT("关注玩家无效!"), MB_ICONSTOP);

		return;
	}

	//关注列表
	pListFocusPlayer->DeleteItem(nItem);
}

void CDialogAdvanced::OnBnClickedCheckAdvancedHideAiInscore()
{
	//隐藏AI下注
	BYTE cbHideAIInscore = ((CButton *)GetDlgItem(IDC_CHECK_ADVANCED_HIDE_AI_INSCORE))->GetCheck();

	//在线人数
	SetDlgItemInt(IDC_STATIC_ADVANCED_ONLINE_PLAYER, (cbHideAIInscore == TRUE ? m_SendUserBetInfo.wOnLineRealPlayerCount : m_SendUserBetInfo.wOnLineAICount + m_SendUserBetInfo.wOnLineRealPlayerCount));

	//下注人数
	SetDlgItemInt(IDC_STATIC_ADVANCED_INSCORE_PLAYER, (cbHideAIInscore == TRUE ? m_SendUserBetInfo.wInScoreRealPlayerCount : m_SendUserBetInfo.wInScoreAICount + m_SendUserBetInfo.wInScoreRealPlayerCount));

	//更新下注列表
	UpdateUserBetList(INVALID_DWORD);
}

void CDialogAdvanced::OnCbnSelchangeComboSelPlayerType()
{
	//更新下注列表
	UpdateUserBetList(INVALID_DWORD);
}

void CDialogAdvanced::OnLvnColumnclickListUserBet(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	WORD wColumnIndex = pNMLV->iSubItem;

	CListCtrl *pListUserBet = (CListCtrl *)GetDlgItem(IDC_LIST_USER_BET);

	//排列列表
	pListUserBet->SortItems(SortFunction, (LPARAM)(&wColumnIndex));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// 构造函数
CDialogTraceLog::CDialogTraceLog(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_TRACELOG, nShowMax, pParent)
{
}

// 析构函数
CDialogTraceLog::~CDialogTraceLog()
{
}

// 控件绑定
void CDialogTraceLog::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);
}

// 初始化窗口
BOOL CDialogTraceLog::OnInitDialog()
{
	// 初始化窗口
	__super::OnInitDialog();

	INT nColumnCount = 0;

	//库存配置
	CListCtrl *pListSysCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_SYSCTRL_TRACELOG);
	pListSysCtrlTraceLog->SetExtendedStyle(pListSysCtrlTraceLog->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("调试ID"), LVCFMT_CENTER, 60);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("配置系统库存"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("配置玩家库存"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("当前系统库存"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("当前玩家库存"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("调节系数"), LVCFMT_CENTER, 60);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("库存统计"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("库存重置次数"), LVCFMT_CENTER, 100);
	pListSysCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("时间"), LVCFMT_CENTER, 100);

	//房间调试
	CListCtrl *pListRoomCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_ROOMCTRL_TRACELOG);
	pListRoomCtrlTraceLog->SetExtendedStyle(pListRoomCtrlTraceLog->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("调试ID"), LVCFMT_CENTER, 60);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("配置系统库存"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("配置玩家库存"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("当前系统库存"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("当前玩家库存"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("调节系数"), LVCFMT_CENTER, 60);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("库存统计"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("操作人"), LVCFMT_CENTER, 60);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("时间"), LVCFMT_CENTER, 100);
	pListRoomCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("状态"), LVCFMT_CENTER, 60);

	//区域调试
	CListCtrl *pListAreaCtrlTraceLog = (CListCtrl *)GetDlgItem(IDC_LIST_AREACTRL_TRACELOG);
	pListAreaCtrlTraceLog->SetExtendedStyle(pListAreaCtrlTraceLog->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	nColumnCount = 0;
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("调试ID"), LVCFMT_CENTER, 60);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("调试位置"), LVCFMT_CENTER, 60);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("调试局数"), LVCFMT_CENTER, 60);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("库存统计"), LVCFMT_CENTER, 100);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("操作人"), LVCFMT_CENTER, 60);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("时间"), LVCFMT_CENTER, 100);
	pListAreaCtrlTraceLog->InsertColumn(nColumnCount++, TEXT("状态"), LVCFMT_CENTER, 60);

	//刷新
	SendDebugMessage(SUB_C_REFRESH_TRACELOG);

	return TRUE;
}

// 消息函数
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
			//定义信息
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
			//定义信息
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

				//操作人
				strCtrlVal.Format(TEXT("%d"), pRoomCtrlResult->RoomCtrlTraceLog.dwDebugUserGameID);
				pListRoomCtrlTraceLog->SetItemText(nItem, 7, strCtrlVal);

				strCtrlVal.Format(TEXT("%d-%d,%d:%d"), pRoomCtrlResult->RoomCtrlTraceLog.tmConfigTime.GetMonth(), pRoomCtrlResult->RoomCtrlTraceLog.tmConfigTime.GetDay(), pRoomCtrlResult->RoomCtrlTraceLog.tmConfigTime.GetHour(), pRoomCtrlResult->RoomCtrlTraceLog.tmConfigTime.GetMinute());
				pListRoomCtrlTraceLog->SetItemText(nItem, 8, strCtrlVal);

				if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == QUEUE)
				{
					 pListRoomCtrlTraceLog->SetItemText(nItem, 9, TEXT("队列"));
				}
				else if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == PROGRESSING)
				{
					pListRoomCtrlTraceLog->SetItemText(nItem, 9, TEXT("进行中"));
				}
				else if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == FINISH)
				{
					pListRoomCtrlTraceLog->SetItemText(nItem, 9, TEXT("完成"));
				}
				else if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == CANCEL)
				{
					pListRoomCtrlTraceLog->SetItemText(nItem, 9, TEXT("取消"));
				}
				else if (pRoomCtrlResult->RoomCtrlTraceLog.roomCtrlStatus == EXECUTE_CANCEL)
				{
					pListRoomCtrlTraceLog->SetItemText(nItem, 9, TEXT("已执行"));
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
			//定义信息
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

				//操作人
				strCtrlVal.Format(TEXT("%d"), pAreaCtrlResult->AreaCtrlTraceLog.dwDebugUserGameID);
				pListAreaCtrlTraceLog->SetItemText(nItem, 4, strCtrlVal);

				strCtrlVal.Format(TEXT("%d-%d,%d:%d"), pAreaCtrlResult->AreaCtrlTraceLog.tmConfigTime.GetMonth(), pAreaCtrlResult->AreaCtrlTraceLog.tmConfigTime.GetDay(), pAreaCtrlResult->AreaCtrlTraceLog.tmConfigTime.GetHour(), pAreaCtrlResult->AreaCtrlTraceLog.tmConfigTime.GetMinute());
				pListAreaCtrlTraceLog->SetItemText(nItem, 5, strCtrlVal);

				if (pAreaCtrlResult->AreaCtrlTraceLog.areaCtrlStatus == AREACTRL_PROGRESSING)
				{
					pListAreaCtrlTraceLog->SetItemText(nItem, 6, TEXT("进行中"));
				}
				else if (pAreaCtrlResult->AreaCtrlTraceLog.areaCtrlStatus == AREACTRL_FINISH)
				{
					pListAreaCtrlTraceLog->SetItemText(nItem, 6, TEXT("完成"));
				}
				else if (pAreaCtrlResult->AreaCtrlTraceLog.areaCtrlStatus == AREACTRL_CANCEL)
				{
					pListAreaCtrlTraceLog->SetItemText(nItem, 6, TEXT("取消"));
				}
				else if (pAreaCtrlResult->AreaCtrlTraceLog.areaCtrlStatus == AREACTRL_EXECUTE_CANCEL)
				{
					pListAreaCtrlTraceLog->SetItemText(nItem, 6, TEXT("已执行"));
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

		//定义信息
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


