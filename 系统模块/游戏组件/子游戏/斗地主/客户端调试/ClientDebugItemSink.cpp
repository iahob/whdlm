#include "stdafx.h"
#include "Resource.h"
#include "ClientDebugItemSink.h"

////////////////////////////////////////////////////////////////////////////////
int g_nCardLevelScore[5] = { 50, 40, 30, 20, 10 };											//等级分数
CString g_strCardLevel[5] = { TEXT("A"), TEXT("B"), TEXT("C"), TEXT("D"), TEXT("E") };		//等级分数
////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(CClientDebugItemSinkDlg, IClientDebugDlg)

BEGIN_MESSAGE_MAP(CClientDebugItemSinkDlg, IClientDebugDlg)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BT_QUERY_TABLE_LIST, &CClientDebugItemSinkDlg::OnBnClickedBtQueryTableList)
	ON_BN_CLICKED(IDC_BT_ADD_DEBUG, &CClientDebugItemSinkDlg::OnBnClickedBtAddDebug)
	ON_BN_CLICKED(IDC_BT_MODIFY_DEBUG, &CClientDebugItemSinkDlg::OnBnClickedBtModifyDebug)
	ON_BN_CLICKED(IDC_BT_DELETE_DEBUG, &CClientDebugItemSinkDlg::OnBnClickedBtDeleteDebug)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TABLE, &CClientDebugItemSinkDlg::OnNMClickListTable)
	ON_BN_CLICKED(IDC_BT_CUR_TABLE, &CClientDebugItemSinkDlg::OnBnClickedBtCurTable)
	ON_BN_CLICKED(IDC_BT_HISTORY_TABLE, &CClientDebugItemSinkDlg::OnBnClickedBtHistoryTable)
END_MESSAGE_MAP()

CClientDebugItemSinkDlg::CClientDebugItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientDebugDlg(CClientDebugItemSinkDlg::IDD, pParent)
{

}

CClientDebugItemSinkDlg::~CClientDebugItemSinkDlg()
{

}

void CClientDebugItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientDebugDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TABLE_ID, m_editTableID);
	DDX_Control(pDX, IDC_EDIT_CARD_CHANCE, m_editCardChance);
	DDX_Control(pDX, IDC_BT_QUERY_TABLE_LIST, m_btQueryTableList);
	DDX_Control(pDX, IDC_BT_ADD_DEBUG, m_btAddDebug);
	DDX_Control(pDX, IDC_BT_MODIFY_DEBUG, m_btModifyDebug);
	DDX_Control(pDX, IDC_BT_DELETE_DEBUG, m_btDeleteDebug);
	DDX_Control(pDX, IDC_BT_CUR_TABLE, m_btCurTable);
	DDX_Control(pDX, IDC_BT_HISTORY_TABLE, m_btHistoryTable);
	DDX_Control(pDX, IDC_LIST_TABLE, m_listCtrlTable);
	DDX_Control(pDX, IDC_LIST_HISTORY_TABLE, m_listCtrlHistoryTable);
	DDX_Control(pDX, IDC_COMBO_CARD_LEVEL, m_ComboCardLevel);
	DDX_Control(pDX, IDC_COMBO_START_HOUR, m_ComboStartHour);
	DDX_Control(pDX, IDC_COMBO_START_MINUTE, m_ComboStartMinute);
	DDX_Control(pDX, IDC_COMBO_START_SCEOND, m_ComboStartSceond);
	DDX_Control(pDX, IDC_COMBO_END_HOUR, m_ComboEndHour);
	DDX_Control(pDX, IDC_COMBO_END_MINUTE, m_ComboEndMinute);
	DDX_Control(pDX, IDC_COMBO_END_SCEOND, m_ComboEndSceond);
}

//初始化
BOOL CClientDebugItemSinkDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	SetWindowText(TEXT("斗地主调试端"));
	
	//桌子列表
	m_listCtrlTable.SetExtendedStyle(m_listCtrlTable.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrlTable.GetHeaderCtrl()->EnableWindow(false);
	INT nColumnCount=0;
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("桌子号"), LVCFMT_CENTER, 60);
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("好牌等级"), LVCFMT_CENTER, 60);
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("出牌概率"), LVCFMT_CENTER, 60);
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("开始时间"), LVCFMT_CENTER, 140);
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("结束时间"), LVCFMT_CENTER, 140);
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("系统收支"), LVCFMT_CENTER, 80);

	//历史桌子列表
	m_listCtrlHistoryTable.SetExtendedStyle(m_listCtrlHistoryTable.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrlHistoryTable.GetHeaderCtrl()->EnableWindow(false);
	nColumnCount = 0;
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("桌子号"), LVCFMT_CENTER, 60);
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("好牌等级"), LVCFMT_CENTER, 60);
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("出牌概率"), LVCFMT_CENTER, 60);
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("开始时间"), LVCFMT_CENTER, 140);
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("结束时间"), LVCFMT_CENTER, 140);
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("系统收支"), LVCFMT_CENTER, 80);
	
	CString str;
	//好牌等级
	for (int i = 0; i < CountArray(g_nCardLevelScore); i++)
	{
		m_ComboCardLevel.AddString(g_strCardLevel[i]);
	}
	

	// 时
	for (int i = 0; i < 24; i++)
	{
		str.Format(TEXT("%02d"), i);
		m_ComboStartHour.AddString(str);
		m_ComboEndHour.AddString(str);
	}

	// 分钞
	for (int i = 0; i < 60; i++)
	{
		str.Format(TEXT("%02d"), i);
		m_ComboStartMinute.AddString(str);
		m_ComboEndMinute.AddString(str);

		m_ComboStartSceond.AddString(str);
		m_ComboEndSceond.AddString(str);
	}

	m_ComboCardLevel.SetCurSel(0);
	m_ComboStartHour.SetCurSel(0);
	m_ComboStartMinute.SetCurSel(0);
	m_ComboStartSceond.SetCurSel(0);
	m_ComboEndHour.SetCurSel(0);
	m_ComboEndMinute.SetCurSel(0);
	m_ComboEndSceond.SetCurSel(0);

	//发送消息
	SendDebugMessage(SUB_C_REQUEST_CUR_DEBUG);

	return TRUE; 
}

void CClientDebugItemSinkDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

HBRUSH CClientDebugItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientDebugDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

void CClientDebugItemSinkDlg::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);
}	

BOOL CClientDebugItemSinkDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			return true;
		}
	}

	return false;
}

// 释放接口
void CClientDebugItemSinkDlg::Release()
{
	delete this;
}

// 创建函数
bool CClientDebugItemSinkDlg::Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback)
{
	// 设置变量
	m_pParentWnd = pParentWnd;
 	m_pIClientDebugCallback = pIClientDebugCallback;

	if (::IsWindow(GetSafeHwnd()))
	{
		ShowWindow(true);
		return true;
	}
	// 创建窗口
	__super::Create(IDD_CLIENT_DEBUG, GetDesktopWindow());

	return true;

}

// 显示窗口
bool CClientDebugItemSinkDlg::ShowWindow(bool bShow)
{
	// 显示窗口
	IClientDebugDlg::ShowWindow(bShow ? SW_SHOW : SW_HIDE);

	return true;
}

// 消息函数
bool CClientDebugItemSinkDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD wDataSize)
{
	// 无效接口
	if (m_pIClientDebugCallback == NULL)
	{
		return false;
	}

 	switch (nMessageID)
 	{
	case SUB_GET_ROMM_ALL_USER_RESULT:
	{
		ASSERT(wDataSize == sizeof(CMD_CF_UserInfo));
		if (wDataSize != sizeof(CMD_CF_UserInfo))
		{
			return false;
		}
		return true;
	}
	case  SUB_S_DEBUG_TEXT:
	{
		//校验大小
		ASSERT(sizeof(CMD_S_DebugText) == wDataSize);
		if (sizeof(CMD_S_DebugText) != wDataSize)
		{
			return false;
		}

		// 定义信息
		CMD_S_DebugText *pDebugText = (CMD_S_DebugText *)pData;

		MessageBox(pDebugText->szMessageText, TEXT("温馨提示"));

		return true;
	}
	case SUB_S_REQUEST_CUR_DEBUG:			//查询当前调试
	{
		ASSERT(wDataSize % sizeof(CMD_S_CurDebugInfo) == 0);
		if (wDataSize % sizeof(CMD_S_CurDebugInfo) != 0)
		{
			return false;
		}
		if (wDataSize == 0)
		{
			m_btQueryTableList.EnableWindow(true);
			return true;
		}
		
		CMD_S_CurDebugInfo *pCurDebugInfo = (CMD_S_CurDebugInfo *)pData;
		int nSendCount = wDataSize / sizeof(CMD_S_CurDebugInfo);
		WORD wTableID = 0;
		CString strText;
		int nHour = 0;
		int nMinute = 0;
		int nSecond = 0;
		int nItemTimeSize = 0;
		int nComtrolTimeSize = 0;
		int nUpdateItem = -1;
		bool bAddItem;

		for (int i = 0; i < nSendCount; i++)
		{
			//获得索引
			nUpdateItem = GetItemIndex(&pCurDebugInfo[i], bAddItem);			

			//插入新记录
			if (nUpdateItem == -1)
			{
				nUpdateItem = m_listCtrlTable.InsertItem(m_listCtrlTable.GetItemCount(), _T(""));
			}
			else
			{
				if (bAddItem)
				{
					nUpdateItem = m_listCtrlTable.InsertItem(nUpdateItem, _T(""));
				}
			}

			//设置单项显示
			UpdateItemText(nUpdateItem, &pCurDebugInfo[i]);
		}
		return true;
	}
	case SUB_S_REQUEST_HISTORY_DEBUG: //查询历史调试
	{
		ASSERT(wDataSize % sizeof(CMD_S_HistoryDebugInfo) == 0);
		if (wDataSize % sizeof(CMD_S_HistoryDebugInfo) != 0)
		{
			return false;
		}
		if (wDataSize == 0)
		{
			m_btQueryTableList.EnableWindow(true);
			return true;
		}

		CMD_S_HistoryDebugInfo *pHistoryDebugInfo = (CMD_S_HistoryDebugInfo *)pData;
		int nSendCount = wDataSize / sizeof(CMD_S_HistoryDebugInfo);
		WORD wTableID = 0;
		CString strText;
		int nHour = 0;
		int nMinute = 0;
		int nSecond = 0;
		int nItemTimeSize = 0;
		int nComtrolTimeSize = 0;
		int nUpdateItem = -1;

		for (int i = 0; i < nSendCount; i++)
		{
			//获得索引
			nUpdateItem = m_listCtrlHistoryTable.InsertItem(m_listCtrlHistoryTable.GetItemCount(), _T(""));

			//桌子号
			strText.Format(TEXT("%d"), pHistoryDebugInfo->wTableID + 1);
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 0, strText);

			//好牌等级
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 1, GetLevelName(pHistoryDebugInfo->nCardLevel));

			//出牌概率
			strText.Format(TEXT("%d"), pHistoryDebugInfo->cbCardChance);
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 2, strText);

			//开始时间
			CTime time;
			time = CTime(pHistoryDebugInfo->nDebugTime[0], pHistoryDebugInfo->nDebugTime[1], pHistoryDebugInfo->nDebugTime[2], pHistoryDebugInfo->nStartTime / 3600, (pHistoryDebugInfo->nStartTime % 3600) / 60, pHistoryDebugInfo->nStartTime % 60);
			strText.Format(TEXT("%04d-%02d-%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 3, strText);

			//结束时间
			time = CTime(pHistoryDebugInfo->nDebugTime[0], pHistoryDebugInfo->nDebugTime[1], pHistoryDebugInfo->nDebugTime[2], (pHistoryDebugInfo->nEndTime % 86400) / 3600, ((pHistoryDebugInfo->nEndTime % 86400) % 3600) / 60, (pHistoryDebugInfo->nEndTime % 86400) % 60);
			if (pHistoryDebugInfo->nEndTime >= 86400)
			{
				CTimeSpan timeSpan(1, 0, 0, 0);
				time += timeSpan;
			}
			strText.Format(TEXT("%04d-%02d-%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 4, strText);

			//系统收支
			strText.Format(TEXT("%I64d"), pHistoryDebugInfo->lSystemScore);
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 5, strText);

			//下一个
			pHistoryDebugInfo++;
		}
		return true;
	}
	case SUB_S_REQUEST_DELETE_DEBUG:
	{
		ASSERT(wDataSize == sizeof(CMD_S_DeleteDebug));
		if (wDataSize != sizeof(CMD_S_DeleteDebug))
		{
			return false;
		}

		CMD_S_DeleteDebug *pDeleteDebug = (CMD_S_DeleteDebug *)pData;

		//获得索引
		int nUpdateItem = GetItemIndex(pDeleteDebug->dwDebugID);

		//删除记录
		if (nUpdateItem != -1)
		{
			m_listCtrlTable.DeleteItem(nUpdateItem);
		}

		return true;
	}
	case SUB_S_UPDATE_CUR_DEBUG:
	{
		ASSERT(wDataSize == sizeof(CMD_S_UpdateDebug));
		if (wDataSize != sizeof(CMD_S_UpdateDebug))
		{
			return false;
		}

		CMD_S_UpdateDebug *pUpdateDebug = (CMD_S_UpdateDebug *)pData;

		//获得索引
		int nUpdateItem = GetItemIndex(pUpdateDebug->dwDebugID);

		//删除记录
		if (nUpdateItem != -1)
		{
			//系统收支
			CString strText;
			strText.Format(TEXT("%I64d"), pUpdateDebug->lSystemScore);
			m_listCtrlTable.SetItemText(nUpdateItem, 5, strText);
		}

		return true;
	}
 	}
 	return false;
}

//获得索引
int CClientDebugItemSinkDlg::GetItemIndex(CMD_S_CurDebugInfo *pCurDebugInfo, bool &bAddItem)
{
	WORD wTableID = 0;
	CString strText;
	int nHour = 0;
	int nMinute = 0;
	int nSecond = 0;
	int nNewTimeSize = 0;
	int nDebugTimeSize = 0;
	int nUpdateItem = -1;
	bAddItem = true;

	for (int nItem = 0; nItem < m_listCtrlTable.GetItemCount(); nItem++)
	{
		//调试ID相同直接修改
		if (pCurDebugInfo->dwDebugID == m_listCtrlTable.GetItemData(nItem))
		{
			nUpdateItem = nItem;
			bAddItem = false;
			break;
		}

		wTableID = _ttoi(m_listCtrlTable.GetItemText(nItem, 0)) - 1;

		//桌子号优先排序
		if (wTableID > pCurDebugInfo->wTableID)
		{
			nUpdateItem = nItem;
			break;
		}

		//桌子号相同
		if (wTableID == pCurDebugInfo->wTableID)
		{
			strText = m_listCtrlTable.GetItemText(nItem, 3);
			_stscanf_s(strText, TEXT("%02d:%02d:%02d"), &nHour, &nMinute, &nSecond);
			nNewTimeSize = nHour * 3600 + nMinute * 60 + nSecond;

			//开始时间优先排序
			if (nNewTimeSize > pCurDebugInfo->nStartTime)
			{
				nUpdateItem = nItem;
				break;
			}
			//开始时间相同
			if (nNewTimeSize == pCurDebugInfo->nStartTime)
			{
				strText = m_listCtrlTable.GetItemText(nItem, 4);
				_stscanf_s(strText, TEXT("%02d:%02d:%02d"), &nHour, &nMinute, &nSecond);
				nNewTimeSize = nHour * 3600 + nMinute * 60 + nSecond;
				if (pCurDebugInfo->nStartTime >= nNewTimeSize)
				{
					nNewTimeSize += 86400;
				}

				//结束时间优先排序
				if (nNewTimeSize > pCurDebugInfo->nEndTime)
				{
					nUpdateItem = nItem;
					break;
				}
			}
		}
	}
	return nUpdateItem;
}

//获得索引
int CClientDebugItemSinkDlg::GetItemIndex(DWORD dwDebugID)
{
	int nUpdateItem = -1;
	for (int nItem = 0; nItem < m_listCtrlTable.GetItemCount(); nItem++)
	{
		//调试ID相同
		if (dwDebugID == m_listCtrlTable.GetItemData(nItem))
		{
			nUpdateItem = nItem;
			break;
		}
	}
	return nUpdateItem;
}

//获得等级
CString CClientDebugItemSinkDlg::GetLevelName(int nLevelScore)
{
	for (int i = 0; i < CountArray(g_nCardLevelScore); i++)
	{
		if (nLevelScore == g_nCardLevelScore[i])
		{
			return g_strCardLevel[i];
		}
	}

	return TEXT("错误等级");
}

//获得等级
int CClientDebugItemSinkDlg::GetLevelIndex(CString strLevelName)
{
	for (int i = 0; i < CountArray(g_nCardLevelScore); i++)
	{
		if (g_strCardLevel[i] == strLevelName)
		{
			return i;
		}
	}

	return 0;
}

//设置单项显示
void CClientDebugItemSinkDlg::UpdateItemText(int nUpdateItem, CMD_S_CurDebugInfo *pCurDebugInfo)
{
	CString strText;

	//桌子号
	strText.Format(TEXT("%d"), pCurDebugInfo->wTableID + 1);
	m_listCtrlTable.SetItemText(nUpdateItem, 0, strText);

	//好牌等级
	m_listCtrlTable.SetItemText(nUpdateItem, 1, GetLevelName(pCurDebugInfo->nCardLevel));

	//出牌概率
	strText.Format(TEXT("%d"), pCurDebugInfo->cbCardChance);
	m_listCtrlTable.SetItemText(nUpdateItem, 2, strText);

	//开始时间
	strText.Format(TEXT("%02d:%02d:%02d"), pCurDebugInfo->nStartTime / 3600, (pCurDebugInfo->nStartTime % 3600) / 60, pCurDebugInfo->nStartTime % 60);
	m_listCtrlTable.SetItemText(nUpdateItem, 3, strText);

	//结束时间
	strText.Format(TEXT("%02d:%02d:%02d"), (pCurDebugInfo->nEndTime % 86400) / 3600, ((pCurDebugInfo->nEndTime % 86400) % 3600) / 60, (pCurDebugInfo->nEndTime % 86400) % 60);
	m_listCtrlTable.SetItemText(nUpdateItem, 4, strText);

	//系统收支
	strText.Format(TEXT("%I64d"), pCurDebugInfo->lSystemScore);
	m_listCtrlTable.SetItemText(nUpdateItem, 5, strText);

	//调试ID
	m_listCtrlTable.SetItemData(nUpdateItem, pCurDebugInfo->dwDebugID);
}

// 调试信息
bool CClientDebugItemSinkDlg::SendDebugMessage(UINT nMessageID, void *pData, UINT nSize)
{
	if (m_pIClientDebugCallback != NULL)
	{
		// 获取信息
		CString StrTableID;
		//GetDlgItem(IDC_EDIT_TABLE_ID)->GetWindowText(StrTableID);

		// 判断有效值
		if (StrTableID.IsEmpty())
		{
			StrTableID = TEXT("0");
			//// 提示信息
			//::MessageBox( GetSafeHwnd(), _T("指定桌子接收消息,请输入有效参数。"), _T("提示"), MB_OK );

			//return false;
		}
		// 发送消息
		m_pIClientDebugCallback->OnDebugInfo(nMessageID, _ttoi(StrTableID), pData, nSize);
	}
	return true;
}

void CClientDebugItemSinkDlg::OnBnClickedBtQueryTableList()
{
	if (m_listCtrlTable.IsWindowVisible())
	{
		//发送消息
		SendDebugMessage(SUB_C_REQUEST_CUR_DEBUG);
		m_listCtrlTable.DeleteAllItems();
		
	}
	else
	{
		//发送消息
		SendDebugMessage(SUB_C_REQUEST_HISTORY_DEBUG);
		m_listCtrlHistoryTable.DeleteAllItems();
	}
	m_btQueryTableList.EnableWindow(false);
}


void CClientDebugItemSinkDlg::OnBnClickedBtAddDebug()
{
	//增加调试
	CMD_C_AddDebug AddDebug;
	int nTableID = GetDlgItemInt(IDC_EDIT_TABLE_ID);
	int nCardChance = GetDlgItemInt(IDC_EDIT_CARD_CHANCE);
	if (nTableID <= 0)
	{
		::MessageBox(GetSafeHwnd(), _T("桌子号设置错误，请重新设置"), TEXT("提示"), MB_OK);
		return;
	}
	if (nCardChance <= 0 || nCardChance > 100)
	{
		::MessageBox(GetSafeHwnd(), _T("出牌概率不能小于等于0或大于100，请重新设置"), TEXT("提示"), MB_OK);
		return;
	}

	AddDebug.wTableID = (WORD)nTableID - 1;
	AddDebug.nCardLevel = g_nCardLevelScore[m_ComboCardLevel.GetCurSel()];
	AddDebug.cbCardChance = (BYTE)nCardChance;
	AddDebug.nStartTime = m_ComboStartHour.GetCurSel() * 3600 + m_ComboStartMinute.GetCurSel() * 60 + m_ComboStartSceond.GetCurSel();
	AddDebug.nEndTime = m_ComboEndHour.GetCurSel() * 3600 + m_ComboEndMinute.GetCurSel() * 60 + m_ComboEndSceond.GetCurSel();
	if (AddDebug.nEndTime < AddDebug.nStartTime)
	{
		AddDebug.nEndTime += 86400;
	}
	
	//发送消息
	SendDebugMessage(SUB_C_REQUEST_ADD_DEBUG, &AddDebug, sizeof(AddDebug));
}


void CClientDebugItemSinkDlg::OnBnClickedBtModifyDebug()
{
	int nLineIndex = m_listCtrlTable.GetSelectionMark();
	if (nLineIndex < 0 || nLineIndex >= m_listCtrlTable.GetItemCount())
	{
		::MessageBox(GetSafeHwnd(), _T("请选择要修改的索引"), TEXT("提示"), MB_OK);
		return;
	}

	//修改调试
	CMD_C_ModifyDebug ModifyDebug;
	int nTableID = GetDlgItemInt(IDC_EDIT_TABLE_ID);
	int nCardChance = GetDlgItemInt(IDC_EDIT_CARD_CHANCE);
	if (nTableID <= 0)
	{
		::MessageBox(GetSafeHwnd(), _T("桌子号设置错误，请重新设置"), TEXT("提示"), MB_OK);
		return;
	}
	if (nCardChance <= 0 || nCardChance > 100)
	{
		::MessageBox(GetSafeHwnd(), _T("出牌概率不能小于等于0或大于100，请重新设置"), TEXT("提示"), MB_OK);
		return;
	}

	ModifyDebug.dwDebugID = m_listCtrlTable.GetItemData(nLineIndex);
	ModifyDebug.wTableID = (WORD)nTableID - 1;
	ModifyDebug.nCardLevel = g_nCardLevelScore[m_ComboCardLevel.GetCurSel()];
	ModifyDebug.cbCardChance = (BYTE)nCardChance;
	ModifyDebug.nStartTime = m_ComboStartHour.GetCurSel() * 3600 + m_ComboStartMinute.GetCurSel() * 60 + m_ComboStartSceond.GetCurSel();
	ModifyDebug.nEndTime = m_ComboEndHour.GetCurSel() * 3600 + m_ComboEndMinute.GetCurSel() * 60 + m_ComboEndSceond.GetCurSel();
	if (ModifyDebug.nEndTime < ModifyDebug.nStartTime)
	{
		ModifyDebug.nEndTime += 86400;
	}

	//发送消息
	SendDebugMessage(SUB_C_REQUEST_MODIFY_DEBUG, &ModifyDebug, sizeof(ModifyDebug));
}


void CClientDebugItemSinkDlg::OnBnClickedBtDeleteDebug()
{
	int nLineIndex = m_listCtrlTable.GetSelectionMark();
	if (nLineIndex < 0 || nLineIndex >= m_listCtrlTable.GetItemCount())
	{
		::MessageBox(GetSafeHwnd(), _T("请选择要删除的索引"), TEXT("提示"), MB_OK);
		return;
	}
	//删除调试
	CMD_C_DeleteDebug DeleteDebug;
	int nTableID = GetDlgItemInt(IDC_EDIT_TABLE_ID);
	if (nTableID <= 0)
	{
		::MessageBox(GetSafeHwnd(), _T("桌子号设置错误，请重新设置"), TEXT("提示"), MB_OK);
		return;
	}

	DeleteDebug.dwDebugID = m_listCtrlTable.GetItemData(nLineIndex);


	//发送消息
	SendDebugMessage(SUB_C_REQUEST_DELETE_DEBUG, &DeleteDebug, sizeof(DeleteDebug));
}

void CClientDebugItemSinkDlg::OnNMClickListTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	//获取选中行号
	int nLineIndex = m_listCtrlTable.GetSelectionMark();
	int nHour;
	int nMinute;
	int nSecond;
	CString strInfo;

	//桌子号
	strInfo = m_listCtrlTable.GetItemText(nLineIndex, 0);
	SetDlgItemText(IDC_EDIT_TABLE_ID, strInfo);

	//好牌等级
	strInfo = m_listCtrlTable.GetItemText(nLineIndex, 1);
	m_ComboCardLevel.SetCurSel(GetLevelIndex(strInfo));

	//出牌概率
	strInfo = m_listCtrlTable.GetItemText(nLineIndex, 2);
	SetDlgItemText(IDC_EDIT_CARD_CHANCE, strInfo);

	//开始时间
	strInfo = m_listCtrlTable.GetItemText(nLineIndex, 3);
	_stscanf_s(strInfo, TEXT("%02d:%02d:%02d"), &nHour, &nMinute, &nSecond);
	m_ComboStartHour.SetCurSel(nHour % 24);
	m_ComboStartMinute.SetCurSel(nMinute % 60);
	m_ComboStartSceond.SetCurSel(nSecond % 60);

	//结束时间
	strInfo = m_listCtrlTable.GetItemText(nLineIndex, 4);
	_stscanf_s(strInfo, TEXT("%02d:%02d:%02d"), &nHour, &nMinute, &nSecond);
	m_ComboEndHour.SetCurSel(nHour % 24);
	m_ComboEndMinute.SetCurSel(nMinute % 60);
	m_ComboEndSceond.SetCurSel(nSecond % 60);

	*pResult = 0;
}


void CClientDebugItemSinkDlg::OnBnClickedBtCurTable()
{
	if (!m_listCtrlTable.IsWindowVisible())
	{
		m_btCurTable.EnableWindow(FALSE);
		m_btHistoryTable.EnableWindow(TRUE);
		m_btAddDebug.EnableWindow(TRUE);
		m_btModifyDebug.EnableWindow(TRUE);
		m_btDeleteDebug.EnableWindow(TRUE);
		m_listCtrlTable.ShowWindow(SW_SHOW);
		m_listCtrlHistoryTable.ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_STATIC_TIP, TEXT("当前调试"));
	}
}


void CClientDebugItemSinkDlg::OnBnClickedBtHistoryTable()
{
	if (!m_listCtrlHistoryTable.IsWindowVisible())
	{
		m_btCurTable.EnableWindow(TRUE);
		m_btHistoryTable.EnableWindow(FALSE);
		m_btAddDebug.EnableWindow(FALSE);
		m_btModifyDebug.EnableWindow(FALSE);
		m_btDeleteDebug.EnableWindow(FALSE);
		m_listCtrlTable.ShowWindow(SW_HIDE);
		m_listCtrlHistoryTable.ShowWindow(SW_SHOW);
		SetDlgItemText(IDC_STATIC_TIP, TEXT("历史记录"));
	}
}
