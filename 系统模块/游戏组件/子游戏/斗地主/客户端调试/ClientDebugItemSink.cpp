#include "stdafx.h"
#include "Resource.h"
#include "ClientDebugItemSink.h"

////////////////////////////////////////////////////////////////////////////////
int g_nCardLevelScore[5] = { 50, 40, 30, 20, 10 };											//�ȼ�����
CString g_strCardLevel[5] = { TEXT("A"), TEXT("B"), TEXT("C"), TEXT("D"), TEXT("E") };		//�ȼ�����
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

//��ʼ��
BOOL CClientDebugItemSinkDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	SetWindowText(TEXT("���������Զ�"));
	
	//�����б�
	m_listCtrlTable.SetExtendedStyle(m_listCtrlTable.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrlTable.GetHeaderCtrl()->EnableWindow(false);
	INT nColumnCount=0;
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("���Ӻ�"), LVCFMT_CENTER, 60);
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("���Ƶȼ�"), LVCFMT_CENTER, 60);
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("���Ƹ���"), LVCFMT_CENTER, 60);
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("��ʼʱ��"), LVCFMT_CENTER, 140);
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("����ʱ��"), LVCFMT_CENTER, 140);
	m_listCtrlTable.InsertColumn(nColumnCount++, TEXT("ϵͳ��֧"), LVCFMT_CENTER, 80);

	//��ʷ�����б�
	m_listCtrlHistoryTable.SetExtendedStyle(m_listCtrlHistoryTable.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrlHistoryTable.GetHeaderCtrl()->EnableWindow(false);
	nColumnCount = 0;
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("���Ӻ�"), LVCFMT_CENTER, 60);
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("���Ƶȼ�"), LVCFMT_CENTER, 60);
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("���Ƹ���"), LVCFMT_CENTER, 60);
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("��ʼʱ��"), LVCFMT_CENTER, 140);
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("����ʱ��"), LVCFMT_CENTER, 140);
	m_listCtrlHistoryTable.InsertColumn(nColumnCount++, TEXT("ϵͳ��֧"), LVCFMT_CENTER, 80);
	
	CString str;
	//���Ƶȼ�
	for (int i = 0; i < CountArray(g_nCardLevelScore); i++)
	{
		m_ComboCardLevel.AddString(g_strCardLevel[i]);
	}
	

	// ʱ
	for (int i = 0; i < 24; i++)
	{
		str.Format(TEXT("%02d"), i);
		m_ComboStartHour.AddString(str);
		m_ComboEndHour.AddString(str);
	}

	// �ֳ�
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

	//������Ϣ
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

	if (::IsWindow(GetSafeHwnd()))
	{
		ShowWindow(true);
		return true;
	}
	// ��������
	__super::Create(IDD_CLIENT_DEBUG, GetDesktopWindow());

	return true;

}

// ��ʾ����
bool CClientDebugItemSinkDlg::ShowWindow(bool bShow)
{
	// ��ʾ����
	IClientDebugDlg::ShowWindow(bShow ? SW_SHOW : SW_HIDE);

	return true;
}

// ��Ϣ����
bool CClientDebugItemSinkDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD wDataSize)
{
	// ��Ч�ӿ�
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
		//У���С
		ASSERT(sizeof(CMD_S_DebugText) == wDataSize);
		if (sizeof(CMD_S_DebugText) != wDataSize)
		{
			return false;
		}

		// ������Ϣ
		CMD_S_DebugText *pDebugText = (CMD_S_DebugText *)pData;

		MessageBox(pDebugText->szMessageText, TEXT("��ܰ��ʾ"));

		return true;
	}
	case SUB_S_REQUEST_CUR_DEBUG:			//��ѯ��ǰ����
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
			//�������
			nUpdateItem = GetItemIndex(&pCurDebugInfo[i], bAddItem);			

			//�����¼�¼
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

			//���õ�����ʾ
			UpdateItemText(nUpdateItem, &pCurDebugInfo[i]);
		}
		return true;
	}
	case SUB_S_REQUEST_HISTORY_DEBUG: //��ѯ��ʷ����
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
			//�������
			nUpdateItem = m_listCtrlHistoryTable.InsertItem(m_listCtrlHistoryTable.GetItemCount(), _T(""));

			//���Ӻ�
			strText.Format(TEXT("%d"), pHistoryDebugInfo->wTableID + 1);
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 0, strText);

			//���Ƶȼ�
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 1, GetLevelName(pHistoryDebugInfo->nCardLevel));

			//���Ƹ���
			strText.Format(TEXT("%d"), pHistoryDebugInfo->cbCardChance);
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 2, strText);

			//��ʼʱ��
			CTime time;
			time = CTime(pHistoryDebugInfo->nDebugTime[0], pHistoryDebugInfo->nDebugTime[1], pHistoryDebugInfo->nDebugTime[2], pHistoryDebugInfo->nStartTime / 3600, (pHistoryDebugInfo->nStartTime % 3600) / 60, pHistoryDebugInfo->nStartTime % 60);
			strText.Format(TEXT("%04d-%02d-%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 3, strText);

			//����ʱ��
			time = CTime(pHistoryDebugInfo->nDebugTime[0], pHistoryDebugInfo->nDebugTime[1], pHistoryDebugInfo->nDebugTime[2], (pHistoryDebugInfo->nEndTime % 86400) / 3600, ((pHistoryDebugInfo->nEndTime % 86400) % 3600) / 60, (pHistoryDebugInfo->nEndTime % 86400) % 60);
			if (pHistoryDebugInfo->nEndTime >= 86400)
			{
				CTimeSpan timeSpan(1, 0, 0, 0);
				time += timeSpan;
			}
			strText.Format(TEXT("%04d-%02d-%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 4, strText);

			//ϵͳ��֧
			strText.Format(TEXT("%I64d"), pHistoryDebugInfo->lSystemScore);
			m_listCtrlHistoryTable.SetItemText(nUpdateItem, 5, strText);

			//��һ��
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

		//�������
		int nUpdateItem = GetItemIndex(pDeleteDebug->dwDebugID);

		//ɾ����¼
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

		//�������
		int nUpdateItem = GetItemIndex(pUpdateDebug->dwDebugID);

		//ɾ����¼
		if (nUpdateItem != -1)
		{
			//ϵͳ��֧
			CString strText;
			strText.Format(TEXT("%I64d"), pUpdateDebug->lSystemScore);
			m_listCtrlTable.SetItemText(nUpdateItem, 5, strText);
		}

		return true;
	}
 	}
 	return false;
}

//�������
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
		//����ID��ֱͬ���޸�
		if (pCurDebugInfo->dwDebugID == m_listCtrlTable.GetItemData(nItem))
		{
			nUpdateItem = nItem;
			bAddItem = false;
			break;
		}

		wTableID = _ttoi(m_listCtrlTable.GetItemText(nItem, 0)) - 1;

		//���Ӻ���������
		if (wTableID > pCurDebugInfo->wTableID)
		{
			nUpdateItem = nItem;
			break;
		}

		//���Ӻ���ͬ
		if (wTableID == pCurDebugInfo->wTableID)
		{
			strText = m_listCtrlTable.GetItemText(nItem, 3);
			_stscanf_s(strText, TEXT("%02d:%02d:%02d"), &nHour, &nMinute, &nSecond);
			nNewTimeSize = nHour * 3600 + nMinute * 60 + nSecond;

			//��ʼʱ����������
			if (nNewTimeSize > pCurDebugInfo->nStartTime)
			{
				nUpdateItem = nItem;
				break;
			}
			//��ʼʱ����ͬ
			if (nNewTimeSize == pCurDebugInfo->nStartTime)
			{
				strText = m_listCtrlTable.GetItemText(nItem, 4);
				_stscanf_s(strText, TEXT("%02d:%02d:%02d"), &nHour, &nMinute, &nSecond);
				nNewTimeSize = nHour * 3600 + nMinute * 60 + nSecond;
				if (pCurDebugInfo->nStartTime >= nNewTimeSize)
				{
					nNewTimeSize += 86400;
				}

				//����ʱ����������
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

//�������
int CClientDebugItemSinkDlg::GetItemIndex(DWORD dwDebugID)
{
	int nUpdateItem = -1;
	for (int nItem = 0; nItem < m_listCtrlTable.GetItemCount(); nItem++)
	{
		//����ID��ͬ
		if (dwDebugID == m_listCtrlTable.GetItemData(nItem))
		{
			nUpdateItem = nItem;
			break;
		}
	}
	return nUpdateItem;
}

//��õȼ�
CString CClientDebugItemSinkDlg::GetLevelName(int nLevelScore)
{
	for (int i = 0; i < CountArray(g_nCardLevelScore); i++)
	{
		if (nLevelScore == g_nCardLevelScore[i])
		{
			return g_strCardLevel[i];
		}
	}

	return TEXT("����ȼ�");
}

//��õȼ�
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

//���õ�����ʾ
void CClientDebugItemSinkDlg::UpdateItemText(int nUpdateItem, CMD_S_CurDebugInfo *pCurDebugInfo)
{
	CString strText;

	//���Ӻ�
	strText.Format(TEXT("%d"), pCurDebugInfo->wTableID + 1);
	m_listCtrlTable.SetItemText(nUpdateItem, 0, strText);

	//���Ƶȼ�
	m_listCtrlTable.SetItemText(nUpdateItem, 1, GetLevelName(pCurDebugInfo->nCardLevel));

	//���Ƹ���
	strText.Format(TEXT("%d"), pCurDebugInfo->cbCardChance);
	m_listCtrlTable.SetItemText(nUpdateItem, 2, strText);

	//��ʼʱ��
	strText.Format(TEXT("%02d:%02d:%02d"), pCurDebugInfo->nStartTime / 3600, (pCurDebugInfo->nStartTime % 3600) / 60, pCurDebugInfo->nStartTime % 60);
	m_listCtrlTable.SetItemText(nUpdateItem, 3, strText);

	//����ʱ��
	strText.Format(TEXT("%02d:%02d:%02d"), (pCurDebugInfo->nEndTime % 86400) / 3600, ((pCurDebugInfo->nEndTime % 86400) % 3600) / 60, (pCurDebugInfo->nEndTime % 86400) % 60);
	m_listCtrlTable.SetItemText(nUpdateItem, 4, strText);

	//ϵͳ��֧
	strText.Format(TEXT("%I64d"), pCurDebugInfo->lSystemScore);
	m_listCtrlTable.SetItemText(nUpdateItem, 5, strText);

	//����ID
	m_listCtrlTable.SetItemData(nUpdateItem, pCurDebugInfo->dwDebugID);
}

// ������Ϣ
bool CClientDebugItemSinkDlg::SendDebugMessage(UINT nMessageID, void *pData, UINT nSize)
{
	if (m_pIClientDebugCallback != NULL)
	{
		// ��ȡ��Ϣ
		CString StrTableID;
		//GetDlgItem(IDC_EDIT_TABLE_ID)->GetWindowText(StrTableID);

		// �ж���Чֵ
		if (StrTableID.IsEmpty())
		{
			StrTableID = TEXT("0");
			//// ��ʾ��Ϣ
			//::MessageBox( GetSafeHwnd(), _T("ָ�����ӽ�����Ϣ,��������Ч������"), _T("��ʾ"), MB_OK );

			//return false;
		}
		// ������Ϣ
		m_pIClientDebugCallback->OnDebugInfo(nMessageID, _ttoi(StrTableID), pData, nSize);
	}
	return true;
}

void CClientDebugItemSinkDlg::OnBnClickedBtQueryTableList()
{
	if (m_listCtrlTable.IsWindowVisible())
	{
		//������Ϣ
		SendDebugMessage(SUB_C_REQUEST_CUR_DEBUG);
		m_listCtrlTable.DeleteAllItems();
		
	}
	else
	{
		//������Ϣ
		SendDebugMessage(SUB_C_REQUEST_HISTORY_DEBUG);
		m_listCtrlHistoryTable.DeleteAllItems();
	}
	m_btQueryTableList.EnableWindow(false);
}


void CClientDebugItemSinkDlg::OnBnClickedBtAddDebug()
{
	//���ӵ���
	CMD_C_AddDebug AddDebug;
	int nTableID = GetDlgItemInt(IDC_EDIT_TABLE_ID);
	int nCardChance = GetDlgItemInt(IDC_EDIT_CARD_CHANCE);
	if (nTableID <= 0)
	{
		::MessageBox(GetSafeHwnd(), _T("���Ӻ����ô�������������"), TEXT("��ʾ"), MB_OK);
		return;
	}
	if (nCardChance <= 0 || nCardChance > 100)
	{
		::MessageBox(GetSafeHwnd(), _T("���Ƹ��ʲ���С�ڵ���0�����100������������"), TEXT("��ʾ"), MB_OK);
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
	
	//������Ϣ
	SendDebugMessage(SUB_C_REQUEST_ADD_DEBUG, &AddDebug, sizeof(AddDebug));
}


void CClientDebugItemSinkDlg::OnBnClickedBtModifyDebug()
{
	int nLineIndex = m_listCtrlTable.GetSelectionMark();
	if (nLineIndex < 0 || nLineIndex >= m_listCtrlTable.GetItemCount())
	{
		::MessageBox(GetSafeHwnd(), _T("��ѡ��Ҫ�޸ĵ�����"), TEXT("��ʾ"), MB_OK);
		return;
	}

	//�޸ĵ���
	CMD_C_ModifyDebug ModifyDebug;
	int nTableID = GetDlgItemInt(IDC_EDIT_TABLE_ID);
	int nCardChance = GetDlgItemInt(IDC_EDIT_CARD_CHANCE);
	if (nTableID <= 0)
	{
		::MessageBox(GetSafeHwnd(), _T("���Ӻ����ô�������������"), TEXT("��ʾ"), MB_OK);
		return;
	}
	if (nCardChance <= 0 || nCardChance > 100)
	{
		::MessageBox(GetSafeHwnd(), _T("���Ƹ��ʲ���С�ڵ���0�����100������������"), TEXT("��ʾ"), MB_OK);
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

	//������Ϣ
	SendDebugMessage(SUB_C_REQUEST_MODIFY_DEBUG, &ModifyDebug, sizeof(ModifyDebug));
}


void CClientDebugItemSinkDlg::OnBnClickedBtDeleteDebug()
{
	int nLineIndex = m_listCtrlTable.GetSelectionMark();
	if (nLineIndex < 0 || nLineIndex >= m_listCtrlTable.GetItemCount())
	{
		::MessageBox(GetSafeHwnd(), _T("��ѡ��Ҫɾ��������"), TEXT("��ʾ"), MB_OK);
		return;
	}
	//ɾ������
	CMD_C_DeleteDebug DeleteDebug;
	int nTableID = GetDlgItemInt(IDC_EDIT_TABLE_ID);
	if (nTableID <= 0)
	{
		::MessageBox(GetSafeHwnd(), _T("���Ӻ����ô�������������"), TEXT("��ʾ"), MB_OK);
		return;
	}

	DeleteDebug.dwDebugID = m_listCtrlTable.GetItemData(nLineIndex);


	//������Ϣ
	SendDebugMessage(SUB_C_REQUEST_DELETE_DEBUG, &DeleteDebug, sizeof(DeleteDebug));
}

void CClientDebugItemSinkDlg::OnNMClickListTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	//��ȡѡ���к�
	int nLineIndex = m_listCtrlTable.GetSelectionMark();
	int nHour;
	int nMinute;
	int nSecond;
	CString strInfo;

	//���Ӻ�
	strInfo = m_listCtrlTable.GetItemText(nLineIndex, 0);
	SetDlgItemText(IDC_EDIT_TABLE_ID, strInfo);

	//���Ƶȼ�
	strInfo = m_listCtrlTable.GetItemText(nLineIndex, 1);
	m_ComboCardLevel.SetCurSel(GetLevelIndex(strInfo));

	//���Ƹ���
	strInfo = m_listCtrlTable.GetItemText(nLineIndex, 2);
	SetDlgItemText(IDC_EDIT_CARD_CHANCE, strInfo);

	//��ʼʱ��
	strInfo = m_listCtrlTable.GetItemText(nLineIndex, 3);
	_stscanf_s(strInfo, TEXT("%02d:%02d:%02d"), &nHour, &nMinute, &nSecond);
	m_ComboStartHour.SetCurSel(nHour % 24);
	m_ComboStartMinute.SetCurSel(nMinute % 60);
	m_ComboStartSceond.SetCurSel(nSecond % 60);

	//����ʱ��
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
		SetDlgItemText(IDC_STATIC_TIP, TEXT("��ǰ����"));
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
		SetDlgItemText(IDC_STATIC_TIP, TEXT("��ʷ��¼"));
	}
}
