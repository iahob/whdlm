// ServerConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ServerConfigDlg.h"
#include "..\��Ϣ����\CMD_ZodiacBattle.h"

tagCustomRule					g_CustomRule;						// ��Ϸ����

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServerConfigDlg, CDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTabOptisons)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CServerConfigDlg::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_READ, &CServerConfigDlg::OnBnClickedButtonRead)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

// ���캯��
CServerConfigDlg::CServerConfigDlg() : CDialog(IDD_DIALOG_MAIN)
{
    g_CustomRule.DefaultCustomRule();
}

// ��������
CServerConfigDlg::~CServerConfigDlg()
{
}

// ��������
void CServerConfigDlg::OnDestroy()
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
VOID CServerConfigDlg::DoDataExchange(CDataExchange *pDX)
{
    __super::DoDataExchange(pDX);
}

// ��ʼ������
BOOL CServerConfigDlg::OnInitDialog()
{
    __super::OnInitDialog();

    // ��ȡ��ҳ
    CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);

    // ����ѡ��
    pTabCtrl->InsertItem(0, _T("ͨ������"));
    pTabCtrl->InsertItem(1, _T("�߼�����"));
    pTabCtrl->SetCurSel(0);

    // ��ȡ��ҳ
    CRect RectWindow;
    CWnd *pWindowShow = GetDlgItem(IDC_STATIC_SHOW);
    pWindowShow->ShowWindow(SW_HIDE);
    pWindowShow->GetWindowRect(RectWindow);
    ScreenToClient(RectWindow);

    // �������
    m_DialogCustom[CUSTOM_GENERAL] = new CDialogGeneral(RectWindow.Height(), this);
    m_DialogCustom[CUSTOM_GENERAL]->Create(IDD_CUSTOM_GENERAL, this);

    // ��������
    m_DialogCustom[CUSTOM_ADVANCED] = new CDialogAdvanced(RectWindow.Height(), this);
    m_DialogCustom[CUSTOM_ADVANCED]->Create(IDD_CUSTOM_ADVANCED, this);

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
	
	//��ȡ����
	OnBnClickedButtonRead();

    return FALSE;
}

// �任ѡ��
void CServerConfigDlg::OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult)
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

    return;
}

void CServerConfigDlg::OnBnClickedButtonSave()
{
	// ������Ϣ
	m_DialogCustom[CUSTOM_GENERAL]->UpdateData(TRUE);

	//У��ת�Ʊ���
	if (g_CustomRule.lTransferStoragePercent + g_CustomRule.lTransferChargePercent + g_CustomRule.lTransferJackPotPercent != 100)
	{
		AfxMessageBox(TEXT("����ˮ�ͽ��ص�ת�Ʊ���֮��Ϊ100�����������ã�"), MB_ICONSTOP);
		return;
	}

	//д��ͨ������
	WriteFileValue(TEXT("GeneralConfig"), TEXT("lConfigStorage"), g_CustomRule.lConfigStorage);
	WriteFileValue(TEXT("GeneralConfig"), TEXT("lConfigJackPot"), g_CustomRule.lConfigJackPot);
	WriteFileValue(TEXT("GeneralConfig"), TEXT("lTransferStoragePercent"), g_CustomRule.lTransferStoragePercent);
	WriteFileValue(TEXT("GeneralConfig"), TEXT("lTransferChargePercent"), g_CustomRule.lTransferChargePercent);
	WriteFileValue(TEXT("GeneralConfig"), TEXT("lTransferJackPotPercent"), g_CustomRule.lTransferJackPotPercent);
	WriteFileValue(TEXT("GeneralConfig"), TEXT("lMosaicGoldStart"), g_CustomRule.lMosaicGoldStart);
	WriteFileValue(TEXT("GeneralConfig"), TEXT("lMosaicGoldPercent"), g_CustomRule.lMosaicGoldPercent);
	WriteFileValue(TEXT("GeneralConfig"), TEXT("lDispatchMosaicGoldCon"), g_CustomRule.lDispatchMosaicGoldCon);
	WriteFileValue(TEXT("GeneralConfig"), TEXT("bShowTrend"), g_CustomRule.bShowTrend ? 1 : 0);
	WriteFileValue(TEXT("GeneralConfig"), TEXT("lMinSharkTimes"), g_CustomRule.lMinSharkTimes);
	WriteFileValue(TEXT("GeneralConfig"), TEXT("lMaxSharkTimes"), g_CustomRule.lMaxSharkTimes);

	//д��߼�����
	for (WORD wGradeIndex = 0; wGradeIndex < MAX_GRADE; wGradeIndex++)
	{
		for (WORD wTableIndex = 0; wTableIndex < MAX_EVEGRADE_TABLE; wTableIndex++)
		{
			for (WORD wLineIndex = 0; wLineIndex < MAX_EVETABLE_DEBUG_LINE; wLineIndex++)
			{
				if (g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].bChange)
				{
					CString strDesc;
					strDesc.Format(TEXT("Grade_%d_TABLE_%d_Line_%d_DebugVal"), wGradeIndex + 1, wTableIndex + 1, wLineIndex + 1);
					WriteFileValue(TEXT("AdvancedConfig"), strDesc, g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDebugValue);

					strDesc.Format(TEXT("Grade_%d_TABLE_%d_Line_%d_Difficulty"), wGradeIndex + 1, wTableIndex + 1, wLineIndex + 1);
					WriteFileValue(TEXT("AdvancedConfig"), strDesc, g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDifficulty);

					g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].bChange = false;
					g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lRemainCtrlValue = g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDebugValue;
				}
			}
		}
	}
}

void CServerConfigDlg::OnBnClickedButtonRead()
{
	//��ȡͨ������
	g_CustomRule.lConfigStorage = ReadFileValue(TEXT("GeneralConfig"), TEXT("lConfigStorage"), 0);
	g_CustomRule.lConfigJackPot = ReadFileValue(TEXT("GeneralConfig"), TEXT("lConfigJackPot"), 0);
	g_CustomRule.lTransferStoragePercent = ReadFileValue(TEXT("GeneralConfig"), TEXT("lTransferStoragePercent"), 0);
	g_CustomRule.lTransferChargePercent = ReadFileValue(TEXT("GeneralConfig"), TEXT("lTransferChargePercent"), 0);
	g_CustomRule.lTransferJackPotPercent = ReadFileValue(TEXT("GeneralConfig"), TEXT("lTransferJackPotPercent"), 0);
	g_CustomRule.lMosaicGoldStart = ReadFileValue(TEXT("GeneralConfig"), TEXT("lMosaicGoldStart"), 0);
	g_CustomRule.lMosaicGoldPercent = ReadFileValue(TEXT("GeneralConfig"), TEXT("lMosaicGoldPercent"), 0);
	g_CustomRule.lDispatchMosaicGoldCon = ReadFileValue(TEXT("GeneralConfig"), TEXT("lDispatchMosaicGoldCon"), 0);
	g_CustomRule.bShowTrend = (ReadFileValue(TEXT("GeneralConfig"), TEXT("bShowTrend"), 0) == 1 ? true : false);
	g_CustomRule.lMinSharkTimes = ReadFileValue(TEXT("GeneralConfig"), TEXT("lMinSharkTimes"), 0);
	g_CustomRule.lMaxSharkTimes = ReadFileValue(TEXT("GeneralConfig"), TEXT("lMaxSharkTimes"), 0);

	//��ȡ����
	for (WORD wGradeIndex = 0; wGradeIndex < MAX_GRADE; wGradeIndex++)
	{
		for (WORD wTableIndex = 0; wTableIndex < MAX_EVEGRADE_TABLE; wTableIndex++)
		{
			LONGLONG lTableWinScore = 0;
			for (WORD wLineIndex = 0; wLineIndex < MAX_EVETABLE_DEBUG_LINE; wLineIndex++)
			{
				CString strDesc;
				strDesc.Format(TEXT("Grade_%d_TABLE_%d_Line_%d_DebugVal"), wGradeIndex + 1, wTableIndex + 1, wLineIndex + 1);
				g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDebugValue = ReadFileValue(TEXT("AdvancedConfig"), strDesc, 0);
				g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lRemainCtrlValue = ReadFileValue(TEXT("AdvancedConfig"), strDesc, 0);

				strDesc.Format(TEXT("Grade_%d_TABLE_%d_Line_%d_Difficulty"), wGradeIndex + 1, wTableIndex + 1, wLineIndex + 1);
				g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDifficulty = ReadFileValue(TEXT("AdvancedConfig"), strDesc, 0);

				lTableWinScore += g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDebugValue;
			}

			g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].lTableWinScore = lTableWinScore;
			g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].bPositive = lTableWinScore > 0 ? true : false;
			g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].emTableType = GetTableType(lTableWinScore);
		}
	}

	//���¿ؼ�
	m_DialogCustom[CUSTOM_GENERAL]->UpdateData(false);
	m_DialogCustom[CUSTOM_GENERAL]->OnShowTrend();
	m_DialogCustom[CUSTOM_ADVANCED]->OnTableQuery();
}

//����Ŀ¼
bool CServerConfigDlg::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//ģ��·��
	TCHAR szModulePath[MAX_PATH] = TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(), szModulePath, CountArray(szModulePath));

	//�����ļ�
	for (INT i = lstrlen(szModulePath); i >= 0; i--)
	{
		if (szModulePath[i] == TEXT('\\'))
		{
			szModulePath[i] = 0;
			break;
		}
	}

	//���ý��
	ASSERT(szModulePath[0] != 0);
	lstrcpyn(szWorkDirectory, szModulePath, wBufferCount);

	return true;
}

//д������
void CServerConfigDlg::WriteFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, SCORE lValue)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH] = TEXT("");
	_sntprintf_s(szServerInfoPath, CountArray(szServerInfoPath), TEXT("%s\\AnimalBattleConfig.ini"), szDirectory);

	//��������
	TCHAR szString[16] = TEXT("");
	_sntprintf_s(szString, CountArray(szString), TEXT("%d"), lValue);

	//д������
	WritePrivateProfileString(pszKeyName, pszItemName, szString, szServerInfoPath);

	return;
}

//��ȡ����
SCORE CServerConfigDlg::ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH] = TEXT("");
	_sntprintf_s(szServerInfoPath, CountArray(szServerInfoPath), TEXT("%s\\AnimalBattleConfig.ini"), szDirectory);

	//��ȡ����
	UINT nValue = GetPrivateProfileInt(pszKeyName, pszItemName, nDefault, szServerInfoPath);

	return (SCORE)nValue;
}

//��ȡ�շű�����
EM_TABLE_TYPE CServerConfigDlg::GetTableType(LONGLONG lTableWinScore)
{
	//��Ӯ >= 100WΪ��������Ӯ>0��С��100WΪ���� ��Ӯ����0Ϊƽ���
	//��Ӯ<0��>-100WΪ������Ӯ<=-100wΪ�󸺱�
	if (lTableWinScore == 0)
	{
		return EQUAL;
	}
	else if (lTableWinScore >= 1000000)
	{
		return MAX_POSITIVE;
	}
	else if (lTableWinScore < 1000000 && lTableWinScore > 0)
	{
		return MIN_POSITIVE;
	}
	else if (lTableWinScore <0 && lTableWinScore > -1000000)
	{
		return MIN_NEGTIVE;
	}
	else if (lTableWinScore <= -1000000)
	{
		return MAX_NEGTIVE;
	}

	return EQUAL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ���ԶԻ���
IMPLEMENT_DYNAMIC(CDialogGeneral, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogGeneral, CDialogDexter)
	ON_BN_CLICKED(IDC_CHECK_SHOWTREND, &CDialogGeneral::OnBnClickedCheckShowtrend)
END_MESSAGE_MAP()

// ���캯��
CDialogGeneral::CDialogGeneral(int nShowMax/* = 0*/, CServerConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_GENERAL, nShowMax, pParent)
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

	DDX_Text(pDX, IDC_EDIT_CONFIG_STORAGE, g_CustomRule.lConfigStorage);
	DDX_Text(pDX, IDC_EDIT_CONFIG_JACKPOT, g_CustomRule.lConfigJackPot);
	DDX_Text(pDX, IDC_EDIT_TRANS_STORPER, g_CustomRule.lTransferStoragePercent);
	DDX_Text(pDX, IDC_EDIT_TRANS_CHARGEPER, g_CustomRule.lTransferChargePercent);
	DDX_Text(pDX, IDC_EDIT_TRANS_JACKPOTPER, g_CustomRule.lTransferJackPotPercent);
	DDX_Text(pDX, IDC_EDIT_MOSGOLD_START, g_CustomRule.lMosaicGoldStart);
	DDX_Text(pDX, IDC_EDIT_MOSGOLD_PER, g_CustomRule.lMosaicGoldPercent);
	DDX_Text(pDX, IDC_EDIT_MOSGOLD_DISPATCH, g_CustomRule.lDispatchMosaicGoldCon);
	DDX_Text(pDX, IDC_EDIT_MINSHARKTIMES, g_CustomRule.lMinSharkTimes);
	DDX_Text(pDX, IDC_EDIT_MAXSHARKTIMES, g_CustomRule.lMaxSharkTimes);
}

// ��ʼ������
BOOL CDialogGeneral::OnInitDialog()
{
    // ��ʼ������
    __super::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_STORAGE))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_JACKPOT))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_TRANS_STORPER))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_TRANS_CHARGEPER))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_TRANS_JACKPOTPER))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_MOSGOLD_START))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_MOSGOLD_PER))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_MOSGOLD_DISPATCH))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_MINSHARKTIMES))->LimitText(4);
	((CEdit*)GetDlgItem(IDC_EDIT_MAXSHARKTIMES))->LimitText(4);

    return TRUE;
}

void CDialogGeneral::OnBnClickedCheckShowtrend()
{
	g_CustomRule.bShowTrend = (((CButton*)GetDlgItem(IDC_CHECK_SHOWTREND))->GetCheck() == TRUE ? true : false);
}

void CDialogGeneral::OnShowTrend()
{
	((CButton*)GetDlgItem(IDC_CHECK_SHOWTREND))->SetCheck(g_CustomRule.bShowTrend ? TRUE : FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// ���ԶԻ���
IMPLEMENT_DYNAMIC(CDialogAdvanced, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogAdvanced, CDialogDexter)
	ON_BN_CLICKED(IDC_BUTTON_TABLE_BEFORE, &CDialogAdvanced::OnBnClickedButtonTableBefore)
	ON_BN_CLICKED(IDC_BUTTON_TABLE_NEXT, &CDialogAdvanced::OnBnClickedButtonTableNext)
	ON_CBN_SELCHANGE(IDC_COMBO_GRADESEL, &CDialogAdvanced::OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_TABLESEL, &CDialogAdvanced::OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_LINE, &CDialogAdvanced::OnCbnSelchangeCombo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TABLE, &CDialogAdvanced::OnLvnItemchangedListTable)
	ON_EN_CHANGE(IDC_EDIT_DEBUG_VALUE, &CDialogAdvanced::OnEnChangeEditDebugValue)
	ON_EN_CHANGE(IDC_EDIT_DIFFICULTY, &CDialogAdvanced::OnEnChangeEditDifficulty)
END_MESSAGE_MAP()

// ���캯��
CDialogAdvanced::CDialogAdvanced(int nShowMax/* = 0*/, CServerConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_ADVANCED, nShowMax, pParent)
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

// ��ʼ������
BOOL CDialogAdvanced::OnInitDialog()
{
    // ��ʼ������
    __super::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_EDIT_DEBUG_VALUE))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_DIFFICULTY))->LimitText(2);

	//�ؼ�
	CListCtrl *pListTable = (CListCtrl *)GetDlgItem(IDC_LIST_TABLE);
	CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
	CComboBox *pComboTable = ((CComboBox*)GetDlgItem(IDC_COMBO_TABLESEL));
	CComboBox *pComboLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE));

	pListTable->SetExtendedStyle(pListTable->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	INT nColumnCount = 0;
	pListTable->InsertColumn(nColumnCount++, TEXT("���"), LVCFMT_CENTER, 50);
	pListTable->InsertColumn(nColumnCount++, TEXT("���"), LVCFMT_CENTER, 120);
	pListTable->InsertColumn(nColumnCount++, TEXT("�Ѷ�"), LVCFMT_CENTER, 125);

	for (WORD i = 0; i < MAX_GRADE; i++)
	{
		CString strGradeSel;
		strGradeSel.Format(TEXT("%d"), i + 1);
		pComboGradeSel->AddString(strGradeSel);
	}

	for (WORD i = 0; i < MAX_EVEGRADE_TABLE; i++)
	{
		CString strTableSel;
		strTableSel.Format(TEXT("%d"), i + 1);
		pComboTable->AddString(strTableSel);
	}

	for (WORD i = 0; i < MAX_EVETABLE_DEBUG_LINE; i++)
	{
		CString strLine;
		strLine.Format(TEXT("%d"), i + 1);
		pComboLine->AddString(strLine);

		pListTable->InsertItem(pListTable->GetItemCount(), strLine);
	}

	//Ĭ��ֵ
	pComboGradeSel->SetCurSel(0);
	pComboTable->SetCurSel(0);
	pComboLine->SetCurSel(0);
	pListTable->InsertItem(pListTable->GetItemCount(), TEXT("����Ӯ"));

	int nComboGradeSel = pComboGradeSel->GetCurSel();
	int nComboTableSel = pComboTable->GetCurSel();
	int nComboLineSel = pComboLine->GetCurSel();
	CString strTableDesc;
	strTableDesc.Format(TEXT("��ǰѡ��ڡ�%d�������ڡ�%d���ű��ڡ�%d����"), nComboGradeSel + 1, nComboTableSel + 1, nComboLineSel + 1);
	SetDlgItemText(IDC_STATIC_TABLEDESC, strTableDesc);

	//���²�ѯ
	OnTableQuery();
	
    return TRUE;
}

void CDialogAdvanced::OnTableQuery()
{
	CListCtrl *pListTable = (CListCtrl *)GetDlgItem(IDC_LIST_TABLE);
	CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
	CComboBox *pComboTable = ((CComboBox*)GetDlgItem(IDC_COMBO_TABLESEL));
	CComboBox *pComboLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE));

	int nComboGradeSel = pComboGradeSel->GetCurSel();
	int nComboTableSel = pComboTable->GetCurSel();
	int nComboLineSel = pComboLine->GetCurSel();

	//У����Ч
	if (!(nComboGradeSel >= 0 && nComboGradeSel < MAX_GRADE && nComboTableSel >= 0 && nComboTableSel < MAX_EVEGRADE_TABLE
		&& nComboLineSel >= 0 && nComboLineSel < MAX_EVETABLE_DEBUG_LINE))
	{
		AfxMessageBox(TEXT("��ѯ����Ϣ����������ѡ��"), MB_ICONSTOP);
		return;
	}

	//���ñ�����
	TABLE tempTable = g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel];
	
	//��ֵ��Ϣ
	for (WORD i = 0; i < MAX_EVETABLE_DEBUG_LINE; i++)
	{
		CString strDebugValue;
		strDebugValue.Format(TEXT("%I64d"), tempTable.debugLineArray[i].lDebugValue);
		pListTable->SetItemText(i, 1, strDebugValue);

		CString strDifficulty;
		strDifficulty.Format(TEXT("%I64d"), tempTable.debugLineArray[i].lDifficulty);
		pListTable->SetItemText(i, 2, strDifficulty);
	}

	//������Ӯ
	CString strTableWinScore;
	strTableWinScore.Format(TEXT("%I64d"), tempTable.lTableWinScore);
	pListTable->SetItemText(MAX_EVETABLE_DEBUG_LINE, 1, strTableWinScore);

	//����ѡ��״̬
	pListTable->SetItemState(nComboLineSel, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	//�����ѶȺͶ��
	SetDlgItemInt(IDC_EDIT_DEBUG_VALUE, tempTable.debugLineArray[nComboLineSel].lDebugValue);
	SetDlgItemInt(IDC_EDIT_DIFFICULTY, tempTable.debugLineArray[nComboLineSel].lDifficulty);
}

//��ȡ�շű�����
EM_TABLE_TYPE CDialogAdvanced::GetTableType(LONGLONG lTableWinScore)
{
	//��Ӯ >= 100WΪ��������Ӯ>0��С��100WΪ���� ��Ӯ����0Ϊƽ���
	//��Ӯ<0��>-100WΪ������Ӯ<=-100wΪ�󸺱�
	if (lTableWinScore == 0)
	{
		return EQUAL;
	}
	else if (lTableWinScore >= 1000000)
	{
		return MAX_POSITIVE;
	}
	else if (lTableWinScore < 1000000 && lTableWinScore > 0)
	{
		return MIN_POSITIVE;
	}
	else if (lTableWinScore <0 && lTableWinScore > -1000000)
	{
		return MIN_NEGTIVE;
	}
	else if (lTableWinScore <= -1000000)
	{
		return MAX_NEGTIVE;
	}

	return EQUAL;
}

void CDialogAdvanced::OnBnClickedButtonTableBefore()
{
	CListCtrl *pListTable = (CListCtrl *)GetDlgItem(IDC_LIST_TABLE);
	CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
	CComboBox *pComboTable = ((CComboBox*)GetDlgItem(IDC_COMBO_TABLESEL));
	CComboBox *pComboLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE));

	int nComboGradeSel = pComboGradeSel->GetCurSel();
	int nComboTableSel = pComboTable->GetCurSel();
	int nComboLineSel = pComboLine->GetCurSel();

	if (nComboTableSel <= 0)
	{
		AfxMessageBox(TEXT("�Ѿ�����ͷ�ˣ�������ѡ��"), MB_ICONSTOP);
		return;
	}

	pComboTable->SetCurSel(--nComboTableSel);
	nComboTableSel = pComboTable->GetCurSel();
	CString strTableDesc;
	strTableDesc.Format(TEXT("��ǰѡ��ڡ�%d�������ڡ�%d���ű��ڡ�%d����"), nComboGradeSel + 1, nComboTableSel + 1, nComboLineSel + 1);
	SetDlgItemText(IDC_STATIC_TABLEDESC, strTableDesc);

	//���²�ѯ
	OnTableQuery();
}

void CDialogAdvanced::OnBnClickedButtonTableNext()
{
	CListCtrl *pListTable = (CListCtrl *)GetDlgItem(IDC_LIST_TABLE);
	CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
	CComboBox *pComboTable = ((CComboBox*)GetDlgItem(IDC_COMBO_TABLESEL));
	CComboBox *pComboLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE));

	int nComboGradeSel = pComboGradeSel->GetCurSel();
	int nComboTableSel = pComboTable->GetCurSel();
	int nComboLineSel = pComboLine->GetCurSel();

	if (nComboTableSel >= MAX_EVEGRADE_TABLE - 1)
	{
		AfxMessageBox(TEXT("�Ѿ�����β�ˣ�������ѡ��"), MB_ICONSTOP);
		return;
	}

	pComboTable->SetCurSel(++nComboTableSel);
	nComboTableSel = pComboTable->GetCurSel();
	CString strTableDesc;
	strTableDesc.Format(TEXT("��ǰѡ��ڡ�%d�������ڡ�%d���ű��ڡ�%d����"), nComboGradeSel + 1, nComboTableSel + 1, nComboLineSel + 1);
	SetDlgItemText(IDC_STATIC_TABLEDESC, strTableDesc);

	//���²�ѯ
	OnTableQuery();
}

void CDialogAdvanced::OnCbnSelchangeCombo()
{
	CListCtrl *pListTable = (CListCtrl *)GetDlgItem(IDC_LIST_TABLE);
	CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
	CComboBox *pComboTable = ((CComboBox*)GetDlgItem(IDC_COMBO_TABLESEL));
	CComboBox *pComboLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE));

	int nComboGradeSel = pComboGradeSel->GetCurSel();
	int nComboTableSel = pComboTable->GetCurSel();
	int nComboLineSel = pComboLine->GetCurSel();

	CString strTableDesc;
	strTableDesc.Format(TEXT("��ǰѡ��ڡ�%d�������ڡ�%d���ű��ڡ�%d����"), nComboGradeSel + 1, nComboTableSel + 1, nComboLineSel + 1);
	SetDlgItemText(IDC_STATIC_TABLEDESC, strTableDesc);

	//���²�ѯ
	OnTableQuery();
}


void CDialogAdvanced::OnLvnItemchangedListTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ((pNMLV->uChanged == LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		//��������
		int nItem = pNMLV->iItem;
		if (nItem >= 0 && nItem < MAX_EVETABLE_DEBUG_LINE)
		{
			CListCtrl *pListTable = (CListCtrl *)GetDlgItem(IDC_LIST_TABLE);
			CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
			CComboBox *pComboTable = ((CComboBox*)GetDlgItem(IDC_COMBO_TABLESEL));
			CComboBox *pComboLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE));
			pComboLine->SetCurSel(nItem);

			int nComboGradeSel = pComboGradeSel->GetCurSel();
			int nComboTableSel = pComboTable->GetCurSel();
			int nComboLineSel = pComboLine->GetCurSel();

			CString strTableDesc;
			strTableDesc.Format(TEXT("��ǰѡ��ڡ�%d�������ڡ�%d���ű��ڡ�%d����"), nComboGradeSel + 1, nComboTableSel + 1, nComboLineSel + 1);
			SetDlgItemText(IDC_STATIC_TABLEDESC, strTableDesc);

			TABLE tempTable = g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel];

			//�����ѶȺͶ��
			SetDlgItemInt(IDC_EDIT_DEBUG_VALUE, tempTable.debugLineArray[nComboLineSel].lDebugValue);
			SetDlgItemInt(IDC_EDIT_DIFFICULTY, tempTable.debugLineArray[nComboLineSel].lDifficulty);
		}
	}

	*pResult = 0;
}

void CDialogAdvanced::OnEnChangeEditDebugValue()
{
	CString strlDebugValue;
	GetDlgItemText(IDC_EDIT_DEBUG_VALUE, strlDebugValue);

	//У���Ƿ�Ϊ��ֵ
	if (strlDebugValue == TEXT(""))
	{
		AfxMessageBox(TEXT("���Ϊ��ֵ������������"), MB_ICONSTOP);

		return;
	}

	strlDebugValue.TrimLeft();
	strlDebugValue.TrimRight();

	LPTSTR lpszDebugValue = new TCHAR[strlDebugValue.GetLength() + 1];
    _tcscpy_s(lpszDebugValue, strlDebugValue.GetLength() + 1, strlDebugValue);
	SCORE lDebugValue = _ttoi64(lpszDebugValue);

	CListCtrl *pListTable = (CListCtrl *)GetDlgItem(IDC_LIST_TABLE);
	CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
	CComboBox *pComboTable = ((CComboBox*)GetDlgItem(IDC_COMBO_TABLESEL));
	CComboBox *pComboLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE));
	int nComboGradeSel = pComboGradeSel->GetCurSel();
	int nComboTableSel = pComboTable->GetCurSel();
	int nComboLineSel = pComboLine->GetCurSel();

	pListTable->SetItemText(nComboLineSel, 1, lpszDebugValue);

	//��ֵ���
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].lDebugValue = lDebugValue;
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].lRemainCtrlValue = lDebugValue;

	//ͳ�Ʊ���Ӯ
	LONGLONG lTableWinScore = 0;
	for (WORD i = 0; i < MAX_EVETABLE_DEBUG_LINE; i++)
	{
		lTableWinScore += g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[i].lDebugValue;
	}

	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].lTableWinScore = lTableWinScore;
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].bPositive = lTableWinScore > 0 ? true : false;
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].bChange = true;
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].emTableType = GetTableType(lTableWinScore);

	CString strTableWinScore;
	strTableWinScore.Format(TEXT("%I64d"), lTableWinScore);
	pListTable->SetItemText(MAX_EVETABLE_DEBUG_LINE, 1, strTableWinScore);

	delete lpszDebugValue;
}

void CDialogAdvanced::OnEnChangeEditDifficulty()
{
	CString strlDifficulty;
	GetDlgItemText(IDC_EDIT_DIFFICULTY, strlDifficulty);

	//У���Ƿ�Ϊ��ֵ
	if (strlDifficulty == TEXT(""))
	{
		AfxMessageBox(TEXT("�Ѷ�Ϊ��ֵ������������"), MB_ICONSTOP);

		return;
	}

	strlDifficulty.TrimLeft();
	strlDifficulty.TrimRight();

	LPTSTR lpszDifficulty = new TCHAR[strlDifficulty.GetLength() + 1];
    _tcscpy_s(lpszDifficulty, strlDifficulty.GetLength() + 1, strlDifficulty);
	SCORE lDifficulty = _ttoi64(lpszDifficulty);

	CListCtrl *pListTable = (CListCtrl *)GetDlgItem(IDC_LIST_TABLE);
	CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
	CComboBox *pComboTable = ((CComboBox*)GetDlgItem(IDC_COMBO_TABLESEL));
	CComboBox *pComboLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE));
	int nComboGradeSel = pComboGradeSel->GetCurSel();
	int nComboTableSel = pComboTable->GetCurSel();
	int nComboLineSel = pComboLine->GetCurSel();

	pListTable->SetItemText(nComboLineSel, 2, lpszDifficulty);

	//��ֵ���
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].lDifficulty = lDifficulty;
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].bChange = true;

	delete lpszDifficulty;
}



