// ServerConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ServerConfigDlg.h"
#include "..\消息定义\CMD_ZodiacBattle.h"

tagCustomRule					g_CustomRule;						// 游戏规则

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServerConfigDlg, CDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTabOptisons)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CServerConfigDlg::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_READ, &CServerConfigDlg::OnBnClickedButtonRead)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

// 构造函数
CServerConfigDlg::CServerConfigDlg() : CDialog(IDD_DIALOG_MAIN)
{
    g_CustomRule.DefaultCustomRule();
}

// 析构函数
CServerConfigDlg::~CServerConfigDlg()
{
}

// 窗口销毁
void CServerConfigDlg::OnDestroy()
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
VOID CServerConfigDlg::DoDataExchange(CDataExchange *pDX)
{
    __super::DoDataExchange(pDX);
}

// 初始化函数
BOOL CServerConfigDlg::OnInitDialog()
{
    __super::OnInitDialog();

    // 获取分页
    CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);

    // 设置选项
    pTabCtrl->InsertItem(0, _T("通用配置"));
    pTabCtrl->InsertItem(1, _T("高级设置"));
    pTabCtrl->SetCurSel(0);

    // 获取分页
    CRect RectWindow;
    CWnd *pWindowShow = GetDlgItem(IDC_STATIC_SHOW);
    pWindowShow->ShowWindow(SW_HIDE);
    pWindowShow->GetWindowRect(RectWindow);
    ScreenToClient(RectWindow);

    // 金币配置
    m_DialogCustom[CUSTOM_GENERAL] = new CDialogGeneral(RectWindow.Height(), this);
    m_DialogCustom[CUSTOM_GENERAL]->Create(IDD_CUSTOM_GENERAL, this);

    // 礼物配置
    m_DialogCustom[CUSTOM_ADVANCED] = new CDialogAdvanced(RectWindow.Height(), this);
    m_DialogCustom[CUSTOM_ADVANCED]->Create(IDD_CUSTOM_ADVANCED, this);

    // 窗口位置
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        CRect RectCustom;
        CSize SizeCustom;
        m_DialogCustom[nIndex]->GetClientRect(RectCustom);
        SizeCustom.SetSize(min(RectWindow.Width(), RectCustom.Width()), min(RectWindow.Height(), RectCustom.Height()));
        m_DialogCustom[nIndex]->SetWindowPos(NULL, RectWindow.left + RectWindow.Width() / 2 - SizeCustom.cx / 2, RectWindow.top + RectWindow.Height() / 2 - SizeCustom.cy / 2, SizeCustom.cx, SizeCustom.cy, SWP_NOZORDER);
        m_DialogCustom[nIndex]->ShowWindow((nIndex == 0) ? SW_SHOW : SW_HIDE);
    }
	
	//读取配置
	OnBnClickedButtonRead();

    return FALSE;
}

// 变换选项
void CServerConfigDlg::OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult)
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
    }

    *pResult = 0;

    return;
}

void CServerConfigDlg::OnBnClickedButtonSave()
{
	// 更新信息
	m_DialogCustom[CUSTOM_GENERAL]->UpdateData(TRUE);

	//校验转移比例
	if (g_CustomRule.lTransferStoragePercent + g_CustomRule.lTransferChargePercent + g_CustomRule.lTransferJackPotPercent != 100)
	{
		AfxMessageBox(TEXT("库存抽水和奖池的转移比例之和为100，请重新设置！"), MB_ICONSTOP);
		return;
	}

	//写入通用配置
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

	//写入高级配置
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
	//读取通用配置
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

	//读取配置
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

	//更新控件
	m_DialogCustom[CUSTOM_GENERAL]->UpdateData(false);
	m_DialogCustom[CUSTOM_GENERAL]->OnShowTrend();
	m_DialogCustom[CUSTOM_ADVANCED]->OnTableQuery();
}

//工作目录
bool CServerConfigDlg::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//模块路径
	TCHAR szModulePath[MAX_PATH] = TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(), szModulePath, CountArray(szModulePath));

	//分析文件
	for (INT i = lstrlen(szModulePath); i >= 0; i--)
	{
		if (szModulePath[i] == TEXT('\\'))
		{
			szModulePath[i] = 0;
			break;
		}
	}

	//设置结果
	ASSERT(szModulePath[0] != 0);
	lstrcpyn(szWorkDirectory, szModulePath, wBufferCount);

	return true;
}

//写入数据
void CServerConfigDlg::WriteFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, SCORE lValue)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//构造路径
	TCHAR szServerInfoPath[MAX_PATH] = TEXT("");
	_sntprintf_s(szServerInfoPath, CountArray(szServerInfoPath), TEXT("%s\\AnimalBattleConfig.ini"), szDirectory);

	//构造数据
	TCHAR szString[16] = TEXT("");
	_sntprintf_s(szString, CountArray(szString), TEXT("%d"), lValue);

	//写入数据
	WritePrivateProfileString(pszKeyName, pszItemName, szString, szServerInfoPath);

	return;
}

//读取数据
SCORE CServerConfigDlg::ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//构造路径
	TCHAR szServerInfoPath[MAX_PATH] = TEXT("");
	_sntprintf_s(szServerInfoPath, CountArray(szServerInfoPath), TEXT("%s\\AnimalBattleConfig.ini"), szDirectory);

	//读取数据
	UINT nValue = GetPrivateProfileInt(pszKeyName, pszItemName, nDefault, szServerInfoPath);

	return (SCORE)nValue;
}

//获取收放表类型
EM_TABLE_TYPE CServerConfigDlg::GetTableType(LONGLONG lTableWinScore)
{
	//输赢 >= 100W为大正表，输赢>0且小于100W为正表， 输赢等于0为平衡表
	//输赢<0且>-100W为负表，输赢<=-100w为大负表
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

// 调试对话框
IMPLEMENT_DYNAMIC(CDialogGeneral, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogGeneral, CDialogDexter)
	ON_BN_CLICKED(IDC_CHECK_SHOWTREND, &CDialogGeneral::OnBnClickedCheckShowtrend)
END_MESSAGE_MAP()

// 构造函数
CDialogGeneral::CDialogGeneral(int nShowMax/* = 0*/, CServerConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_GENERAL, nShowMax, pParent)
{
}

// 析构函数
CDialogGeneral::~CDialogGeneral()
{
}

// 控件绑定
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

// 初始化窗口
BOOL CDialogGeneral::OnInitDialog()
{
    // 初始化窗口
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

// 调试对话框
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

// 构造函数
CDialogAdvanced::CDialogAdvanced(int nShowMax/* = 0*/, CServerConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_ADVANCED, nShowMax, pParent)
{
}

// 析构函数
CDialogAdvanced::~CDialogAdvanced()
{
}

// 控件绑定
void CDialogAdvanced::DoDataExchange(CDataExchange *pDX)
{
    CDialogDexter::DoDataExchange(pDX);
}

// 初始化窗口
BOOL CDialogAdvanced::OnInitDialog()
{
    // 初始化窗口
    __super::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_EDIT_DEBUG_VALUE))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_DIFFICULTY))->LimitText(2);

	//控件
	CListCtrl *pListTable = (CListCtrl *)GetDlgItem(IDC_LIST_TABLE);
	CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
	CComboBox *pComboTable = ((CComboBox*)GetDlgItem(IDC_COMBO_TABLESEL));
	CComboBox *pComboLine = ((CComboBox*)GetDlgItem(IDC_COMBO_LINE));

	pListTable->SetExtendedStyle(pListTable->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	INT nColumnCount = 0;
	pListTable->InsertColumn(nColumnCount++, TEXT("序号"), LVCFMT_CENTER, 50);
	pListTable->InsertColumn(nColumnCount++, TEXT("额度"), LVCFMT_CENTER, 120);
	pListTable->InsertColumn(nColumnCount++, TEXT("难度"), LVCFMT_CENTER, 125);

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

	//默认值
	pComboGradeSel->SetCurSel(0);
	pComboTable->SetCurSel(0);
	pComboLine->SetCurSel(0);
	pListTable->InsertItem(pListTable->GetItemCount(), TEXT("表输赢"));

	int nComboGradeSel = pComboGradeSel->GetCurSel();
	int nComboTableSel = pComboTable->GetCurSel();
	int nComboLineSel = pComboLine->GetCurSel();
	CString strTableDesc;
	strTableDesc.Format(TEXT("当前选择第【%d】档，第【%d】张表，第【%d】行"), nComboGradeSel + 1, nComboTableSel + 1, nComboLineSel + 1);
	SetDlgItemText(IDC_STATIC_TABLEDESC, strTableDesc);

	//重新查询
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

	//校验有效
	if (!(nComboGradeSel >= 0 && nComboGradeSel < MAX_GRADE && nComboTableSel >= 0 && nComboTableSel < MAX_EVEGRADE_TABLE
		&& nComboLineSel >= 0 && nComboLineSel < MAX_EVETABLE_DEBUG_LINE))
	{
		AfxMessageBox(TEXT("查询表信息错误，请重新选择！"), MB_ICONSTOP);
		return;
	}

	//设置表数据
	TABLE tempTable = g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel];
	
	//赋值信息
	for (WORD i = 0; i < MAX_EVETABLE_DEBUG_LINE; i++)
	{
		CString strDebugValue;
		strDebugValue.Format(TEXT("%I64d"), tempTable.debugLineArray[i].lDebugValue);
		pListTable->SetItemText(i, 1, strDebugValue);

		CString strDifficulty;
		strDifficulty.Format(TEXT("%I64d"), tempTable.debugLineArray[i].lDifficulty);
		pListTable->SetItemText(i, 2, strDifficulty);
	}

	//表总输赢
	CString strTableWinScore;
	strTableWinScore.Format(TEXT("%I64d"), tempTable.lTableWinScore);
	pListTable->SetItemText(MAX_EVETABLE_DEBUG_LINE, 1, strTableWinScore);

	//设置选中状态
	pListTable->SetItemState(nComboLineSel, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	//设置难度和额度
	SetDlgItemInt(IDC_EDIT_DEBUG_VALUE, tempTable.debugLineArray[nComboLineSel].lDebugValue);
	SetDlgItemInt(IDC_EDIT_DIFFICULTY, tempTable.debugLineArray[nComboLineSel].lDifficulty);
}

//获取收放表类型
EM_TABLE_TYPE CDialogAdvanced::GetTableType(LONGLONG lTableWinScore)
{
	//输赢 >= 100W为大正表，输赢>0且小于100W为正表， 输赢等于0为平衡表
	//输赢<0且>-100W为负表，输赢<=-100w为大负表
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
		AfxMessageBox(TEXT("已经到表头了，请重新选择！"), MB_ICONSTOP);
		return;
	}

	pComboTable->SetCurSel(--nComboTableSel);
	nComboTableSel = pComboTable->GetCurSel();
	CString strTableDesc;
	strTableDesc.Format(TEXT("当前选择第【%d】档，第【%d】张表，第【%d】行"), nComboGradeSel + 1, nComboTableSel + 1, nComboLineSel + 1);
	SetDlgItemText(IDC_STATIC_TABLEDESC, strTableDesc);

	//重新查询
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
		AfxMessageBox(TEXT("已经到表尾了，请重新选择！"), MB_ICONSTOP);
		return;
	}

	pComboTable->SetCurSel(++nComboTableSel);
	nComboTableSel = pComboTable->GetCurSel();
	CString strTableDesc;
	strTableDesc.Format(TEXT("当前选择第【%d】档，第【%d】张表，第【%d】行"), nComboGradeSel + 1, nComboTableSel + 1, nComboLineSel + 1);
	SetDlgItemText(IDC_STATIC_TABLEDESC, strTableDesc);

	//重新查询
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
	strTableDesc.Format(TEXT("当前选择第【%d】档，第【%d】张表，第【%d】行"), nComboGradeSel + 1, nComboTableSel + 1, nComboLineSel + 1);
	SetDlgItemText(IDC_STATIC_TABLEDESC, strTableDesc);

	//重新查询
	OnTableQuery();
}


void CDialogAdvanced::OnLvnItemchangedListTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ((pNMLV->uChanged == LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		//处理内容
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
			strTableDesc.Format(TEXT("当前选择第【%d】档，第【%d】张表，第【%d】行"), nComboGradeSel + 1, nComboTableSel + 1, nComboLineSel + 1);
			SetDlgItemText(IDC_STATIC_TABLEDESC, strTableDesc);

			TABLE tempTable = g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel];

			//设置难度和额度
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

	//校验是否为空值
	if (strlDebugValue == TEXT(""))
	{
		AfxMessageBox(TEXT("额度为空值，请重新设置"), MB_ICONSTOP);

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

	//赋值额度
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].lDebugValue = lDebugValue;
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].lRemainCtrlValue = lDebugValue;

	//统计表输赢
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

	//校验是否为空值
	if (strlDifficulty == TEXT(""))
	{
		AfxMessageBox(TEXT("难度为空值，请重新设置"), MB_ICONSTOP);

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

	//赋值额度
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].lDifficulty = lDifficulty;
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].bChange = true;

	delete lpszDifficulty;
}



