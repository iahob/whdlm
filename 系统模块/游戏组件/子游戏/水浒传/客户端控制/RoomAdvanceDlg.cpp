#include "stdafx.h"
#include "Resource.h"
#include "RoomAdvanceDlg.h"

IMPLEMENT_DYNAMIC(CRoomAdvanceDlg, CSkinDialog)

BEGIN_MESSAGE_MAP(CRoomAdvanceDlg, CSkinDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TABLE_MAP_STORAGE, &CRoomAdvanceDlg::OnClickListCtrl)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_TABLE_MAP_STORAGE, &CRoomAdvanceDlg::OnEnKillFocusListCtrl)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_TABLE_MAP_STORAGE, &CRoomAdvanceDlg::OnEnSetFocusListCtrl)
	ON_BN_CLICKED(IDC_BUTTON_SINGLEMODE_CONFIRM, OnSingleModeConfirm)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_CONFIRM, OnBatchConfirm)
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM_SELECTED, OnConfirmSelected)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_SELECTED, OnDeleteSelected)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CRoomAdvanceDlg::CRoomAdvanceDlg(CWnd* pParent /*=NULL*/)
: CSkinDialog(CRoomAdvanceDlg::IDD, pParent)
{
	m_wTableID = INVALID_WORD;
	m_lModifyStorageSingle = INVALID_DWORD;
	m_lModifyStorageBatch = INVALID_DWORD;
}

CRoomAdvanceDlg::~CRoomAdvanceDlg()
{
}

void CRoomAdvanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TABLE_MAP_STORAGE, m_listTableMapStorage);
	DDX_Control(pDX, IDC_LIST_TABLE_MAP_STORAGE_ALL, m_listTableMapStorageAll);
	DDX_Control(pDX, IDC_EDIT_TABLEID, m_editTableID);
	DDX_Control(pDX, IDC_EDIT_MODIFY_SINGLETABLE, m_editModifySingleTable);
	DDX_Control(pDX, IDC_EDIT_BATCH_STORAGE, m_editBatchStorage);
	DDX_Control(pDX, IDC_BUTTON_SINGLEMODE_CONFIRM, m_btSingleModeConfirm);
	DDX_Control(pDX, IDC_BUTTON_BATCH_CONFIRM, m_btBatchConfirm);
	DDX_Control(pDX, IDC_BUTTON_CONFIRM_SELECTED, m_btConfirmSelected);
	DDX_Control(pDX, IDC_BUTTON_DELETE_SELECTED, m_btDeleteSelected);
	DDX_Control(pDX, IDC_RICHEDIT_BATCH_DESCRIPTION, m_richEditBatchDescription);
}

//初始化
BOOL CRoomAdvanceDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	SetWindowText(TEXT("高级配置"));

	//桌子号映射库存
	m_listTableMapStorage.SetExtendedStyle(m_listTableMapStorage.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listTableMapStorage.GetHeaderCtrl()->EnableWindow(false);
	INT nColumnCount=0;
	m_listTableMapStorage.InsertColumn(nColumnCount++, TEXT("桌子号"), LVCFMT_CENTER, 70);
	m_listTableMapStorage.InsertColumn(nColumnCount++, TEXT("当前库存"), LVCFMT_CENTER, 110);
	m_listTableMapStorage.InsertColumn(nColumnCount++, TEXT("本桌输赢"), LVCFMT_CENTER, 110);
	
	//统计
	m_listTableMapStorageAll.SetExtendedStyle(m_listTableMapStorageAll.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listTableMapStorageAll.GetHeaderCtrl()->EnableWindow(false);
	nColumnCount=0;
	m_listTableMapStorageAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 70);
	m_listTableMapStorageAll.InsertColumn(nColumnCount++, TEXT("统计库存"), LVCFMT_CENTER, 110);
	m_listTableMapStorageAll.InsertColumn(nColumnCount++, TEXT("统计输赢"), LVCFMT_CENTER, 110);
	
	//限制范围
	m_editTableID.LimitText(3);
	m_editModifySingleTable.LimitText(12);
	m_editBatchStorage.LimitText(12);
	
	SetDlgItemText(IDC_BUTTON_SINGLEMODE_CONFIRM, TEXT("单桌确认"));
	SetDlgItemText(IDC_BUTTON_BATCH_CONFIRM, TEXT("批量确认"));
	
	SetDlgItemText(IDC_BUTTON_CONFIRM_SELECTED, TEXT("确定选中"));
	SetDlgItemText(IDC_BUTTON_DELETE_SELECTED, TEXT("删除选中"));

	//禁用控件
	m_richEditBatchDescription.CleanScreen();
	m_richEditBatchDescription.EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT(""));
	m_editBatchStorage.SetWindowText(TEXT(""));
	m_editBatchStorage.EnableWindow(FALSE);
	m_btBatchConfirm.EnableWindow(FALSE);

	return TRUE; 
}

BOOL CRoomAdvanceDlg::PreTranslateMessage(MSG *pMsg)
{
	return FALSE;
}

void CRoomAdvanceDlg::OnCancel()
{
	

	ShowWindow(SW_HIDE);
}

void CRoomAdvanceDlg::OnClickListCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_ListBatchTableID.RemoveAll();

	POSITION pos = m_listTableMapStorage.GetFirstSelectedItemPosition(); //pos选中的首行位置
	if (pos != NULL)
	{
		//如果选择多行
		while(pos)   
		{
			WORD nIndex = -1;
			nIndex = (WORD)m_listTableMapStorage.GetNextSelectedItem(pos);

			m_ListBatchTableID.AddTail(nIndex);
		}
	}
	else
	{
		return;
	}

	//禁用控件
	m_richEditBatchDescription.CleanScreen();
	m_richEditBatchDescription.EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT(""));
	m_editBatchStorage.SetWindowText(TEXT(""));
	m_editBatchStorage.EnableWindow(FALSE);
	m_btBatchConfirm.EnableWindow(FALSE);
	
	//获取最后个选中的
	m_wTableID = m_ListBatchTableID.GetTail();
	CString strTableID;
	strTableID.Format(TEXT("%d"), m_wTableID + 1);
	SetDlgItemText(IDC_EDIT_TABLEID, strTableID);
}

void CRoomAdvanceDlg::OnEnKillFocusListCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	//m_richEditBatchDescription.CleanScreen();
	//
	//CString str = TEXT("选中的桌号为：");
	//
	//if (m_ListBatchTableID.GetSize() == 0)
	//{
	//	return;
	//}

	//CString strTableID;
	//POSITION pos = m_ListBatchTableID.GetHeadPosition();
	//while(pos)
	//{
	//	WORD wTableID = m_ListBatchTableID.GetNext(pos);
	//	strTableID.Format(TEXT("第[%d]桌 "), wTableID + 1);
	//	str += strTableID;
	//}
	//
	//m_richEditBatchDescription.InsertString(str, RGB(255,10,10));
}

void CRoomAdvanceDlg::OnEnSetFocusListCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	//m_richEditBatchDescription.CleanScreen();

	//GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT(""));
	//GetDlgItem(IDC_EDIT_BATCH_STORAGE)->SetWindowText(TEXT(""));
	//
	////取消选中
	//int ItemCount = 0;
	//ItemCount = m_listTableMapStorage.GetItemCount();
	//for (WORD i=0; i<ItemCount; i++)
	//{
	//	m_listTableMapStorage.SetItemState(i, 0, LVIS_SELECTED);
	//}	

	//m_richEditBatchDescription.InsertString(TEXT(""), RGB(255,10,10));

	//m_ListBatchTableID.RemoveAll();
}

void CRoomAdvanceDlg::OnSingleModeConfirm()
{
	//读取桌子ID
	CString strTableID = TEXT("");
	GetDlgItemText(IDC_EDIT_TABLEID, strTableID);
	
	//读取库存
	CString strModifyStorage = TEXT("");
	GetDlgItemText(IDC_EDIT_MODIFY_SINGLETABLE, strModifyStorage);
	
	//校验数值
	if (strTableID == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_SINGLEMODE_DESCRIPTION)->SetWindowText(TEXT("桌子号不能为空"));
		
		if (m_wTableID != INVALID_WORD)
		{
			//还原原来值
			CString strBuf;
			strBuf.Format(TEXT("%d"), m_wTableID + 1);
			GetDlgItem(IDC_EDIT_TABLEID)->SetWindowText(strBuf);
		}

		return;
	}

	if (strModifyStorage == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_SINGLEMODE_DESCRIPTION)->SetWindowText(TEXT("修改库存值不能为空"));
		
		if (m_lModifyStorageSingle != INVALID_DWORD)
		{
			//还原原来值
			CString strBuf;
			strBuf.Format(TEXT("%I64d"), m_lModifyStorageSingle);
			GetDlgItem(IDC_EDIT_MODIFY_SINGLETABLE)->SetWindowText(strBuf);
		}

		return;
	}

	WORD wTableID = StrToInt(strTableID) - 1;
	if (wTableID < 0 || wTableID >= m_MapTableStorage.GetSize())
	{
		GetDlgItem(IDC_STATIC_SINGLEMODE_DESCRIPTION)->SetWindowText(TEXT("该房间不存在该桌子号，请重新输入"));

		if (m_wTableID != INVALID_WORD)
		{
			//还原原来值
			CString strBuf;
			strBuf.Format(TEXT("%d"), m_wTableID + 1);
			GetDlgItem(IDC_EDIT_TABLEID)->SetWindowText(strBuf);
		}

		return;
	}
	

	//拷贝数据
	m_wTableID = wTableID;

	strModifyStorage.TrimLeft();
	strModifyStorage.TrimRight();
	LPTSTR lpsz = new TCHAR[strModifyStorage.GetLength() + 1];
	_tcscpy(lpsz, strModifyStorage);
	m_lModifyStorageSingle = _ttoi64(lpsz);
	delete lpsz;

	CMD_C_SingleMode SingleMode;
	ZeroMemory(&SingleMode, sizeof(SingleMode));
	SingleMode.wTableID = m_wTableID;
	SingleMode.lTableStorage = m_lModifyStorageSingle;

	CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
	if (pGameFrameEngine != NULL)
	{
		pGameFrameEngine->SendSocketData(SUB_C_SINGLEMODE_CONFIRM, &SingleMode, sizeof(SingleMode));
	}

	CString strInfo;
	strInfo.Format(TEXT("第[%d]桌库存修改成功!"), wTableID + 1);

	GetDlgItem(IDC_STATIC_SINGLEMODE_DESCRIPTION)->SetWindowText(strInfo);
}

void CRoomAdvanceDlg::OnBatchConfirm()
{
	//读取批量库存
	CString strBatchStorage = TEXT("");
	GetDlgItemText(IDC_EDIT_BATCH_STORAGE, strBatchStorage);

	//校验数值
	if (strBatchStorage == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT("批量库存值不能为空"));

		if (m_lModifyStorageBatch != INVALID_DWORD)
		{
			//还原原来值
			CString strBuf;
			strBuf.Format(TEXT("%I64d"), m_lModifyStorageBatch);
			GetDlgItem(IDC_EDIT_BATCH_STORAGE)->SetWindowText(strBuf);
		}

		return;
	}

	if (m_ListBatchTableID.GetSize() == 0)
	{
		GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT("并没有选中桌子号"));

		return;
	}

	//拷贝数据
	strBatchStorage.TrimLeft();
	strBatchStorage.TrimRight();
	LPTSTR lpsz = new TCHAR[strBatchStorage.GetLength() + 1];
	_tcscpy(lpsz, strBatchStorage);
	m_lModifyStorageBatch = _ttoi64(lpsz);
	delete lpsz;

	CMD_C_BatchMode BatchMode;
	ZeroMemory(&BatchMode, sizeof(BatchMode));
	BatchMode.wBatchTableCount = m_ListBatchTableID.GetSize();
	BatchMode.lBatchModifyStorage = m_lModifyStorageBatch;

	POSITION pos = m_ListBatchTableID.GetHeadPosition();
	while(pos)
	{
		WORD wTableID = m_ListBatchTableID.GetNext(pos);
		BatchMode.bBatchTableFlag[wTableID] = true;
	}

	CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
	if (pGameFrameEngine != NULL)
	{
		pGameFrameEngine->SendSocketData(SUB_C_BATCHMODE_CONFIRM, &BatchMode, sizeof(BatchMode));
	}

	CString strInfo = TEXT("批量库存修改成功!");
	GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(strInfo);
}

void CRoomAdvanceDlg::OnConfirmSelected()
{
	m_richEditBatchDescription.CleanScreen();

	//校验数据
	if (m_ListBatchTableID.GetSize() == 0)
	{
		m_richEditBatchDescription.InsertString(TEXT("没有桌子被选中，请选中桌子重试！"), RGB(255,10,10));

		GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT(""));

		return;
	}

	//清空屏幕
	m_richEditBatchDescription.CleanScreen();
	
	CString str = TEXT("选中的桌号为：");
	
	if (m_ListBatchTableID.GetSize() == 0)
	{
		return;
	}

	CString strTableID;
	POSITION pos = m_ListBatchTableID.GetHeadPosition();
	while(pos)
	{
		WORD wTableID = m_ListBatchTableID.GetNext(pos);
		strTableID.Format(TEXT("第[%d]桌 "), wTableID + 1);
		str += strTableID;
	}
	
	m_richEditBatchDescription.InsertString(str, RGB(255,10,10));

	//激活控件
	m_richEditBatchDescription.EnableWindow(TRUE);
	m_editBatchStorage.SetWindowText(TEXT(""));
	m_editBatchStorage.EnableWindow(TRUE);
	m_btBatchConfirm.EnableWindow(TRUE);
}

void CRoomAdvanceDlg::OnDeleteSelected()
{
	m_richEditBatchDescription.CleanScreen();

	//校验数据
	if (m_ListBatchTableID.GetSize() == 0)
	{
		m_richEditBatchDescription.InsertString(TEXT("没有桌子被选中，请选中桌子重试！"), RGB(255,10,10));

		GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT(""));

		return;
	}

	//禁用控件
	m_richEditBatchDescription.CleanScreen();
	m_richEditBatchDescription.EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT(""));
	m_editBatchStorage.SetWindowText(TEXT(""));
	m_editBatchStorage.EnableWindow(FALSE);
	m_btBatchConfirm.EnableWindow(FALSE);

	//取消选中
	int ItemCount = 0;
	ItemCount = m_listTableMapStorage.GetItemCount();
	for (WORD i=0; i<ItemCount; i++)
	{
		m_listTableMapStorage.SetItemState(i, 0, LVIS_SELECTED);
	}	

	//清空链表
	m_ListBatchTableID.RemoveAll();

	GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT("批量桌子取消成功"));
}

HBRUSH CRoomAdvanceDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSkinDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_SINGLEMODE_DESCRIPTION || pWnd->GetDlgCtrlID() == IDC_STATIC_BATCH_DESCRIPTION)
	{
		pDC->SetTextColor(RGB(255,10,10));
	}

	return hbr;
}

//更新桌号映射库存
void CRoomAdvanceDlg::UpdateTableMapStorage(CMD_S_UpdateTableMapStorage *pUpdateTableMapStorage)
{
	TABLESTORAGEINFO storageInfo;
	ZeroMemory(&storageInfo, sizeof(storageInfo));
	storageInfo.lTableCurrentStorage = pUpdateTableMapStorage->lTableCurrentStorage;
	storageInfo.lTableStorageInput = pUpdateTableMapStorage->lTableStorageInput;
	storageInfo.lTableWinScore = pUpdateTableMapStorage->lTableCurrentStorage - pUpdateTableMapStorage->lTableStorageInput;

	m_MapTableStorage.SetAt(pUpdateTableMapStorage->wTableID, storageInfo);
	ShowTableMapStorage();
}

//显示桌号映射库存
void CRoomAdvanceDlg::ShowTableMapStorage()
{
	//遍历映射
	POSITION ptHead = m_MapTableStorage.GetStartPosition();
	WORD wTableID = 0;
	TABLESTORAGEINFO storageInfo;
	ZeroMemory(&storageInfo, sizeof(storageInfo));
	
	//插入行
	if (m_listTableMapStorage.GetItemCount() < m_MapTableStorage.GetSize())
	{
		for (WORD i=m_listTableMapStorage.GetItemCount(); i<m_MapTableStorage.GetSize(); i++ )
		{
			//插入数据
			m_listTableMapStorage.InsertItem(i, TEXT(""));

			CString strInfo;
			strInfo.Format(TEXT("第[%d]桌"), i + 1);
			m_listTableMapStorage.SetItemText(i, 0, strInfo);
		}
	}

	LONGLONG lTableCurrentStorageSum = 0LL;
	LONGLONG lTableWinScoreSum = 0LL;
	while(ptHead)
	{
		m_MapTableStorage.GetNextAssoc(ptHead, wTableID, storageInfo);

		CString strInfo;
		strInfo.Format(TEXT("%I64d"), storageInfo.lTableCurrentStorage);
		m_listTableMapStorage.SetItemText(wTableID, 1, strInfo);

		strInfo.Format(TEXT("%I64d"), storageInfo.lTableWinScore);
		m_listTableMapStorage.SetItemText(wTableID, 2, strInfo);

		lTableCurrentStorageSum += storageInfo.lTableCurrentStorage;
		lTableWinScoreSum += storageInfo.lTableWinScore;
	}

	//插入行
	if (m_listTableMapStorageAll.GetItemCount() == 0)
	{			//插入数据
		m_listTableMapStorageAll.InsertItem(0, TEXT(""));
	}
	

	CString strInfo;
	strInfo.Format(TEXT("%I64d"), lTableCurrentStorageSum);
	m_listTableMapStorageAll.SetItemText(0, 1, strInfo);

	strInfo.Format(TEXT("%I64d"), lTableWinScoreSum);
	m_listTableMapStorageAll.SetItemText(0, 2, strInfo);
}
