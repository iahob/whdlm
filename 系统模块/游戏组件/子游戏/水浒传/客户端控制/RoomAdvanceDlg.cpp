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

//��ʼ��
BOOL CRoomAdvanceDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	SetWindowText(TEXT("�߼�����"));

	//���Ӻ�ӳ����
	m_listTableMapStorage.SetExtendedStyle(m_listTableMapStorage.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listTableMapStorage.GetHeaderCtrl()->EnableWindow(false);
	INT nColumnCount=0;
	m_listTableMapStorage.InsertColumn(nColumnCount++, TEXT("���Ӻ�"), LVCFMT_CENTER, 70);
	m_listTableMapStorage.InsertColumn(nColumnCount++, TEXT("��ǰ���"), LVCFMT_CENTER, 110);
	m_listTableMapStorage.InsertColumn(nColumnCount++, TEXT("������Ӯ"), LVCFMT_CENTER, 110);
	
	//ͳ��
	m_listTableMapStorageAll.SetExtendedStyle(m_listTableMapStorageAll.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listTableMapStorageAll.GetHeaderCtrl()->EnableWindow(false);
	nColumnCount=0;
	m_listTableMapStorageAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 70);
	m_listTableMapStorageAll.InsertColumn(nColumnCount++, TEXT("ͳ�ƿ��"), LVCFMT_CENTER, 110);
	m_listTableMapStorageAll.InsertColumn(nColumnCount++, TEXT("ͳ����Ӯ"), LVCFMT_CENTER, 110);
	
	//���Ʒ�Χ
	m_editTableID.LimitText(3);
	m_editModifySingleTable.LimitText(12);
	m_editBatchStorage.LimitText(12);
	
	SetDlgItemText(IDC_BUTTON_SINGLEMODE_CONFIRM, TEXT("����ȷ��"));
	SetDlgItemText(IDC_BUTTON_BATCH_CONFIRM, TEXT("����ȷ��"));
	
	SetDlgItemText(IDC_BUTTON_CONFIRM_SELECTED, TEXT("ȷ��ѡ��"));
	SetDlgItemText(IDC_BUTTON_DELETE_SELECTED, TEXT("ɾ��ѡ��"));

	//���ÿؼ�
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

	POSITION pos = m_listTableMapStorage.GetFirstSelectedItemPosition(); //posѡ�е�����λ��
	if (pos != NULL)
	{
		//���ѡ�����
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

	//���ÿؼ�
	m_richEditBatchDescription.CleanScreen();
	m_richEditBatchDescription.EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT(""));
	m_editBatchStorage.SetWindowText(TEXT(""));
	m_editBatchStorage.EnableWindow(FALSE);
	m_btBatchConfirm.EnableWindow(FALSE);
	
	//��ȡ����ѡ�е�
	m_wTableID = m_ListBatchTableID.GetTail();
	CString strTableID;
	strTableID.Format(TEXT("%d"), m_wTableID + 1);
	SetDlgItemText(IDC_EDIT_TABLEID, strTableID);
}

void CRoomAdvanceDlg::OnEnKillFocusListCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	//m_richEditBatchDescription.CleanScreen();
	//
	//CString str = TEXT("ѡ�е�����Ϊ��");
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
	//	strTableID.Format(TEXT("��[%d]�� "), wTableID + 1);
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
	////ȡ��ѡ��
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
	//��ȡ����ID
	CString strTableID = TEXT("");
	GetDlgItemText(IDC_EDIT_TABLEID, strTableID);
	
	//��ȡ���
	CString strModifyStorage = TEXT("");
	GetDlgItemText(IDC_EDIT_MODIFY_SINGLETABLE, strModifyStorage);
	
	//У����ֵ
	if (strTableID == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_SINGLEMODE_DESCRIPTION)->SetWindowText(TEXT("���ӺŲ���Ϊ��"));
		
		if (m_wTableID != INVALID_WORD)
		{
			//��ԭԭ��ֵ
			CString strBuf;
			strBuf.Format(TEXT("%d"), m_wTableID + 1);
			GetDlgItem(IDC_EDIT_TABLEID)->SetWindowText(strBuf);
		}

		return;
	}

	if (strModifyStorage == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_SINGLEMODE_DESCRIPTION)->SetWindowText(TEXT("�޸Ŀ��ֵ����Ϊ��"));
		
		if (m_lModifyStorageSingle != INVALID_DWORD)
		{
			//��ԭԭ��ֵ
			CString strBuf;
			strBuf.Format(TEXT("%I64d"), m_lModifyStorageSingle);
			GetDlgItem(IDC_EDIT_MODIFY_SINGLETABLE)->SetWindowText(strBuf);
		}

		return;
	}

	WORD wTableID = StrToInt(strTableID) - 1;
	if (wTableID < 0 || wTableID >= m_MapTableStorage.GetSize())
	{
		GetDlgItem(IDC_STATIC_SINGLEMODE_DESCRIPTION)->SetWindowText(TEXT("�÷��䲻���ڸ����Ӻţ�����������"));

		if (m_wTableID != INVALID_WORD)
		{
			//��ԭԭ��ֵ
			CString strBuf;
			strBuf.Format(TEXT("%d"), m_wTableID + 1);
			GetDlgItem(IDC_EDIT_TABLEID)->SetWindowText(strBuf);
		}

		return;
	}
	

	//��������
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
	strInfo.Format(TEXT("��[%d]������޸ĳɹ�!"), wTableID + 1);

	GetDlgItem(IDC_STATIC_SINGLEMODE_DESCRIPTION)->SetWindowText(strInfo);
}

void CRoomAdvanceDlg::OnBatchConfirm()
{
	//��ȡ�������
	CString strBatchStorage = TEXT("");
	GetDlgItemText(IDC_EDIT_BATCH_STORAGE, strBatchStorage);

	//У����ֵ
	if (strBatchStorage == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT("�������ֵ����Ϊ��"));

		if (m_lModifyStorageBatch != INVALID_DWORD)
		{
			//��ԭԭ��ֵ
			CString strBuf;
			strBuf.Format(TEXT("%I64d"), m_lModifyStorageBatch);
			GetDlgItem(IDC_EDIT_BATCH_STORAGE)->SetWindowText(strBuf);
		}

		return;
	}

	if (m_ListBatchTableID.GetSize() == 0)
	{
		GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT("��û��ѡ�����Ӻ�"));

		return;
	}

	//��������
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

	CString strInfo = TEXT("��������޸ĳɹ�!");
	GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(strInfo);
}

void CRoomAdvanceDlg::OnConfirmSelected()
{
	m_richEditBatchDescription.CleanScreen();

	//У������
	if (m_ListBatchTableID.GetSize() == 0)
	{
		m_richEditBatchDescription.InsertString(TEXT("û�����ӱ�ѡ�У���ѡ���������ԣ�"), RGB(255,10,10));

		GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT(""));

		return;
	}

	//�����Ļ
	m_richEditBatchDescription.CleanScreen();
	
	CString str = TEXT("ѡ�е�����Ϊ��");
	
	if (m_ListBatchTableID.GetSize() == 0)
	{
		return;
	}

	CString strTableID;
	POSITION pos = m_ListBatchTableID.GetHeadPosition();
	while(pos)
	{
		WORD wTableID = m_ListBatchTableID.GetNext(pos);
		strTableID.Format(TEXT("��[%d]�� "), wTableID + 1);
		str += strTableID;
	}
	
	m_richEditBatchDescription.InsertString(str, RGB(255,10,10));

	//����ؼ�
	m_richEditBatchDescription.EnableWindow(TRUE);
	m_editBatchStorage.SetWindowText(TEXT(""));
	m_editBatchStorage.EnableWindow(TRUE);
	m_btBatchConfirm.EnableWindow(TRUE);
}

void CRoomAdvanceDlg::OnDeleteSelected()
{
	m_richEditBatchDescription.CleanScreen();

	//У������
	if (m_ListBatchTableID.GetSize() == 0)
	{
		m_richEditBatchDescription.InsertString(TEXT("û�����ӱ�ѡ�У���ѡ���������ԣ�"), RGB(255,10,10));

		GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT(""));

		return;
	}

	//���ÿؼ�
	m_richEditBatchDescription.CleanScreen();
	m_richEditBatchDescription.EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT(""));
	m_editBatchStorage.SetWindowText(TEXT(""));
	m_editBatchStorage.EnableWindow(FALSE);
	m_btBatchConfirm.EnableWindow(FALSE);

	//ȡ��ѡ��
	int ItemCount = 0;
	ItemCount = m_listTableMapStorage.GetItemCount();
	for (WORD i=0; i<ItemCount; i++)
	{
		m_listTableMapStorage.SetItemState(i, 0, LVIS_SELECTED);
	}	

	//�������
	m_ListBatchTableID.RemoveAll();

	GetDlgItem(IDC_STATIC_BATCH_DESCRIPTION)->SetWindowText(TEXT("��������ȡ���ɹ�"));
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

//��������ӳ����
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

//��ʾ����ӳ����
void CRoomAdvanceDlg::ShowTableMapStorage()
{
	//����ӳ��
	POSITION ptHead = m_MapTableStorage.GetStartPosition();
	WORD wTableID = 0;
	TABLESTORAGEINFO storageInfo;
	ZeroMemory(&storageInfo, sizeof(storageInfo));
	
	//������
	if (m_listTableMapStorage.GetItemCount() < m_MapTableStorage.GetSize())
	{
		for (WORD i=m_listTableMapStorage.GetItemCount(); i<m_MapTableStorage.GetSize(); i++ )
		{
			//��������
			m_listTableMapStorage.InsertItem(i, TEXT(""));

			CString strInfo;
			strInfo.Format(TEXT("��[%d]��"), i + 1);
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

	//������
	if (m_listTableMapStorageAll.GetItemCount() == 0)
	{			//��������
		m_listTableMapStorageAll.InsertItem(0, TEXT(""));
	}
	

	CString strInfo;
	strInfo.Format(TEXT("%I64d"), lTableCurrentStorageSum);
	m_listTableMapStorageAll.SetItemText(0, 1, strInfo);

	strInfo.Format(TEXT("%I64d"), lTableWinScoreSum);
	m_listTableMapStorageAll.SetItemText(0, 2, strInfo);
}
