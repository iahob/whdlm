// ServerConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ServerConfigDlg.h"
#include "..\消息定义\CMD_Game.h"

tagCustomRule					g_CustomRule;						// 游戏规则
MSXML2::IXMLDOMDocument2Ptr	g_plDomDocument;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServerConfigDlg, CDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTabOptisons)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CServerConfigDlg::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_READ, &CServerConfigDlg::OnBnClickedButtonRead)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//工作目录
bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
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

//获取收放表类型
EM_TABLE_TYPE GetTableType(LONGLONG lTableWinScore)
{
	//输赢 >= 100W为大正表，输赢>0且小于100W为正表， 输赢等于0为平衡表
	//输赢<0且>-100W为负表，输赢<=-100w为大负表
	if (lTableWinScore == 0)
	{
		return EQUAL;
	}

	if (lTableWinScore < 1000000 && lTableWinScore > 0)
	{
		return POSITIVE_TENPERCENT;
	}
	else if (lTableWinScore >= 1000000 && lTableWinScore < 2500000)
	{
		return POSITIVE_TWENTYPERCENT;
	}
	else if (lTableWinScore >= 2500000 && lTableWinScore < 7500000)
	{
		return POSITIVE_THIRTYPERCENT;
	}
	else if (lTableWinScore >= 7500000)
	{
		POSITIVE_FORTYPERCENT;
	}

	if (lTableWinScore <0 && lTableWinScore > -1000000)
	{
		return NEGTIVE_TENPERCENT;
	}
	else if (lTableWinScore <= -1000000 && lTableWinScore > -2500000)
	{
		return NEGTIVE_TWENTYPERCENT;
	}
	else if (lTableWinScore <= -2500000 && lTableWinScore > -7500000)
	{
		return NEGTIVE_THIRTYPERCENT;
	}
	else if (lTableWinScore <= -7500000)
	{
		return NEGTIVE_FORTYPERCENT;
	}

	return EQUAL;
}

SCORE CstringToScore(CString strVal)
{
	LPTSTR lpszVal = new TCHAR[strVal.GetLength() + 1];
	_tcscpy_s(lpszVal, strVal.GetLength() + 1, strVal);
	SCORE lVal = _ttoi64(lpszVal);
	delete[] lpszVal;

	return lVal;
}

void TravelXMLChildNode(MSXML2::IXMLDOMNodePtr pParent)
{
	for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
	{
		//节点名字
		_variant_t strNodeName = pChild->nodeName;

		//节点属性
		MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

		//存在属性
		LONG lAttrListLen = pDOMAttrList->Getlength();
		if (lAttrListLen > 0)
		{
			//获取第一个属性的名字
			MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(0);
			CString strNodeElement = ((LPCTSTR)pDOMAttr->nodeName);

			//收放记录
			//LineId标识唯一一条记录，第一位表示档，第二到四位表示表，第五到六位表示行
			if (strNodeElement == TEXT("LineId"))
			{
				CString strnodeTypedValue = pDOMAttr->GetnodeTypedValue();
				SCORE lnodeTypedValue = CstringToScore(strnodeTypedValue);
				WORD wGradeIndex = lnodeTypedValue / 100000 - 1;
				WORD wTableIndex = (lnodeTypedValue - (wGradeIndex + 1) * 100000) / 100 - 1;
				WORD wLineIndex = (lnodeTypedValue % 10 == 0 ? 10 : lnodeTypedValue % 10) - 1;

				MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(1);

				//DebugVal
				g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDebugValue = CstringToScore(pDOMAttr->GetnodeTypedValue());
				g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lRemainCtrlValue = CstringToScore(pDOMAttr->GetnodeTypedValue());

				pDOMAttr = pDOMAttrList->Getitem(2);
				//Difficulty
				g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDifficulty = CstringToScore(pDOMAttr->GetnodeTypedValue());
			}
			//表输赢
			else if (strNodeElement == TEXT("GradeWinLostID"))
			{
				CString strnodeTypedValue = pDOMAttr->GetnodeTypedValue();
				SCORE lnodeTypedValue = CstringToScore(strnodeTypedValue);
				WORD wGradeIndex = lnodeTypedValue - 1;

				MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(1);
				WORD wTableIndex = CstringToScore(pDOMAttr->GetnodeTypedValue()) - 1;

				pDOMAttr = pDOMAttrList->Getitem(2);
				SCORE lTableWinScore = CstringToScore(pDOMAttr->GetnodeTypedValue());

				g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].lTableWinScore = lTableWinScore;
				g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].bPositive = lTableWinScore > 0 ? true : false;
				g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].emTableType = GetTableType(lTableWinScore);
			}
			//挡位选择
			else if (strNodeElement == TEXT("GradeSelID"))
			{
				CString strnodeTypedValue = pDOMAttr->GetnodeTypedValue();
				SCORE lnodeTypedValue = CstringToScore(strnodeTypedValue);
				WORD wGradeIndex = lnodeTypedValue - 1;

				MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(1);
				g_CustomRule.gradeRange[wGradeIndex].lRoomTotalBetMin = CstringToScore(pDOMAttr->GetnodeTypedValue());

				pDOMAttr = pDOMAttrList->Getitem(2);
				g_CustomRule.gradeRange[wGradeIndex].lRoomTotalBetMax = (CstringToScore(pDOMAttr->GetnodeTypedValue()) == -1 ? INVALID_LONGLONG : CstringToScore(pDOMAttr->GetnodeTypedValue()));
			}
			//库存范围
			else if (strNodeElement == TEXT("GradeStorageID"))
			{
				CString strnodeTypedValue = pDOMAttr->GetnodeTypedValue();
				SCORE lnodeTypedValue = CstringToScore(strnodeTypedValue);
				WORD wGradeIndex = lnodeTypedValue - 1;

				MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(1);
				WORD wStorageIndex = CstringToScore(pDOMAttr->GetnodeTypedValue()) - 1;

				pDOMAttr = pDOMAttrList->Getitem(2);
				g_CustomRule.storageRange[wGradeIndex][wStorageIndex].lStorageMin = CstringToScore(pDOMAttr->GetnodeTypedValue());

				pDOMAttr = pDOMAttrList->Getitem(3);
				g_CustomRule.storageRange[wGradeIndex][wStorageIndex].lStorageMax = (CstringToScore(pDOMAttr->GetnodeTypedValue()) == -1 ? INVALID_LONGLONG : CstringToScore(pDOMAttr->GetnodeTypedValue()));
			}
			//通用配置
			else if (strNodeElement == TEXT("GeneralfigIndex"))
			{
				CString strnodeTypedValue = pDOMAttr->GetnodeTypedValue();
				SCORE lnodeTypedValue = CstringToScore(strnodeTypedValue);

				MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(1);
				SCORE lVal = CstringToScore(pDOMAttr->GetnodeTypedValue());

				switch (lnodeTypedValue)
				{
				case 1:
				{
						  g_CustomRule.lConfigStorage = lVal;
						  break;
				}
				case 2:
				{
						  g_CustomRule.lTransferStoragePercent = lVal;
						  break;
				}
				case 3:
				{
						  g_CustomRule.lTransferChargePercent = lVal;
						  break;
				}
				}
			}
		}
	}
}

void ReadXMLFile(CString strXMLFileName)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//构造路径
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//加载XML文件
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//根节点
		MSXML2::IXMLDOMNodePtr pXMLRoot = g_plDomDocument->documentElement;
		//根节点名字
		_variant_t strRootName = pXMLRoot->nodeName;

		//递归遍历
		TravelXMLChildNode(pXMLRoot);
	}
}

//修改收放表
void ModifyXMLNode(CString strXMLFileName, CString strAttributeNode, SCORE lCtrlVal, SCORE lDifficulty)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//构造路径
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//加载XML文件
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//根节点
		MSXML2::IXMLDOMNodePtr pParent = g_plDomDocument->documentElement;

		for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
		{
			//节点名字
			_variant_t strNodeName = pChild->nodeName;

			//节点属性
			MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

			//存在属性
			if (pDOMAttrList->Getlength() > 0)
			{
				MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(0);

				if (pDOMAttr->GetnodeTypedValue() == strAttributeNode)
				{
					//设置额度
					pDOMAttr = pDOMAttrList->Getitem(1);
					_variant_t val = (__int64)lCtrlVal;
					pDOMAttr->PutnodeTypedValue(val);

					//设置难度
					pDOMAttr = pDOMAttrList->Getitem(2);
					val = (__int64)lDifficulty;
					pDOMAttr->PutnodeTypedValue(val);

					break;
				}
			}
		}

		//保存XML文件
		g_plDomDocument->save(bstrFileName);
	}
}

//修改通用配置 
void ModifyXMLNode(CString strXMLFileName, CString strAttributeNode, SCORE lVal)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//构造路径
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//加载XML文件
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//根节点
		MSXML2::IXMLDOMNodePtr pParent = g_plDomDocument->documentElement;

		for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
		{
			//节点名字
			_variant_t strNodeName = pChild->nodeName;

			//节点属性
			MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

			//存在属性
			if (pDOMAttrList->Getlength() > 0)
			{
				MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(1);
				CString strNodeElement = ((LPCTSTR)pDOMAttr->nodeName);
				if (strNodeElement == strAttributeNode)
				{
					//设置值
					_variant_t val = (__int64)lVal;
					pDOMAttr->PutnodeTypedValue(val);

					break;
				}
			}
		}

		//保存XML文件
		g_plDomDocument->save(bstrFileName);
	}
}

//修改表输赢
void ModifyXMLNode(CString strXMLFileName, SCORE lGradeWinLostID, SCORE lTableId, SCORE lSum)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//构造路径
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//加载XML文件
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//根节点
		MSXML2::IXMLDOMNodePtr pParent = g_plDomDocument->documentElement;

		for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
		{
			//节点名字
			_variant_t strNodeName = pChild->nodeName;

			//节点属性
			MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

			//存在属性
			if (pDOMAttrList->Getlength() > 0)
			{
				MSXML2::IXMLDOMNodePtr pDOMAttr1 = pDOMAttrList->Getitem(0);
				SCORE lTempGradeWinLostID = pDOMAttr1->GetnodeTypedValue();

				MSXML2::IXMLDOMNodePtr pDOMAttr2 = pDOMAttrList->Getitem(1);
				SCORE lTempTableId = pDOMAttr2->GetnodeTypedValue();

				if (lTempGradeWinLostID == lGradeWinLostID && lTempTableId == lTableId)
				{
					//设置值
					_variant_t val = (__int64)lSum;
					MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(2);
					pDOMAttr->PutnodeTypedValue(val);

					break;
				}
			}
		}

		//保存XML文件
		g_plDomDocument->save(bstrFileName);
	}
}

//修改挡位选择
void ModifyXMLGradeSelNode(CString strXMLFileName, SCORE lGradeSelID, SCORE lRoomTotalBetMin, SCORE lRoomTotalBetMax)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//构造路径
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//加载XML文件
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//根节点
		MSXML2::IXMLDOMNodePtr pParent = g_plDomDocument->documentElement;

		for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
		{
			//节点名字
			_variant_t strNodeName = pChild->nodeName;

			//节点属性
			MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

			//存在属性
			if (pDOMAttrList->Getlength() > 0)
			{
				MSXML2::IXMLDOMNodePtr pDOMAttr1 = pDOMAttrList->Getitem(0);
				SCORE lTempGradeSelID = pDOMAttr1->GetnodeTypedValue();

				if (lTempGradeSelID == lGradeSelID)
				{
					//设置值
					_variant_t val = (__int64)lRoomTotalBetMin;
					MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(1);
					pDOMAttr->PutnodeTypedValue(val);

					//设置值
					val = (__int64)lRoomTotalBetMax;
					pDOMAttr = pDOMAttrList->Getitem(2);
					pDOMAttr->PutnodeTypedValue(val);

					break;
				}
			}
		}

		//保存XML文件
		g_plDomDocument->save(bstrFileName);
	}
}

//修改库存范围
void ModifyXMLStorageRangeNode(CString strXMLFileName, SCORE lGradeStorageID, SCORE lStorageRangeID, SCORE lStorageMin, SCORE lStorageMax)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//构造路径
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//加载XML文件
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//根节点
		MSXML2::IXMLDOMNodePtr pParent = g_plDomDocument->documentElement;

		for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
		{
			//节点名字
			_variant_t strNodeName = pChild->nodeName;

			//节点属性
			MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

			//存在属性
			if (pDOMAttrList->Getlength() > 0)
			{
				MSXML2::IXMLDOMNodePtr pDOMAttr1 = pDOMAttrList->Getitem(0);
				SCORE lTempGradeStorageID = pDOMAttr1->GetnodeTypedValue();

				MSXML2::IXMLDOMNodePtr pDOMAttr2 = pDOMAttrList->Getitem(1);
				SCORE lTempStorageRangeID = pDOMAttr2->GetnodeTypedValue();

				if (lTempGradeStorageID == lGradeStorageID && lTempStorageRangeID == lStorageRangeID)
				{
					//设置值
					_variant_t val = (__int64)lStorageMin;
					MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(2);
					pDOMAttr->PutnodeTypedValue(val);

					//设置值
					val = (__int64)lStorageMax;
					pDOMAttr = pDOMAttrList->Getitem(3);
					pDOMAttr->PutnodeTypedValue(val);

					break;
				}
			}
		}

		//保存XML文件
		g_plDomDocument->save(bstrFileName);
	}
}

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

	//XML COM的初始化
	::CoInitialize(NULL);
	HRESULT hr = g_plDomDocument.CreateInstance(MSXML2::CLSID_DOMDocument);

	if (FAILED(hr))
	{
		_com_error er(hr);
		return false;
	}

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
	if (g_CustomRule.lTransferStoragePercent + g_CustomRule.lTransferChargePercent != 100)
	{
		AfxMessageBox(TEXT("库存抽水转移比例之和为100，请重新设置！"), MB_ICONSTOP);
		return;
	}

	//写入XML通用配置
	ModifyXMLNode(TEXT("GeneralConfig.xml"), TEXT("lConfigStorage"), g_CustomRule.lConfigStorage);
	ModifyXMLNode(TEXT("GeneralConfig.xml"), TEXT("lTransferStoragePercent"), g_CustomRule.lTransferStoragePercent);
	ModifyXMLNode(TEXT("GeneralConfig.xml"), TEXT("lTransferChargePercent"), g_CustomRule.lTransferChargePercent);

	//写入XML收放表 
	for (WORD wGradeIndex = 0; wGradeIndex < MAX_GRADE; wGradeIndex++)
	{
		for (WORD wTableIndex = 0; wTableIndex < MAX_EVEGRADE_TABLE; wTableIndex++)
		{
			LONGLONG lTableWinScore = 0;
			bool bTableChange = false;
			for (WORD wLineIndex = 0; wLineIndex < MAX_EVETABLE_DEBUG_LINE; wLineIndex++)
			{
				if (g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].bChange)
				{
					CString strXMLFileName;
					strXMLFileName.Format(TEXT("GradeConfig_%d.xml"), wGradeIndex + 1);

					LONG lLineID = (wGradeIndex + 1) * 100000 + (wTableIndex + 1) * 100 + wLineIndex + 1;
					CString strLineID;
					strLineID.Format(TEXT("%d"), lLineID);
					SCORE lDebugValue = g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDebugValue;
					SCORE lDifficulty = g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDifficulty;

					ModifyXMLNode(strXMLFileName, strLineID, lDebugValue, lDifficulty);

					g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].bChange = false;
					bTableChange = true;
				}

				lTableWinScore += g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].debugLineArray[wLineIndex].lDebugValue;
			}

			g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].lTableWinScore = lTableWinScore;
			g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].bPositive = lTableWinScore > 0 ? true : false;
			g_CustomRule.lConfigGradeArray[wGradeIndex].lTableArray[wTableIndex].emTableType = GetTableType(lTableWinScore);

			if (bTableChange)
			{
				//写入XML表输赢
				ModifyXMLNode(TEXT("TableWinLost.xml"), wGradeIndex + 1, wTableIndex + 1, lTableWinScore);
			}
		}
	}

	//流水范围对应档位
	ModifyXMLGradeSelNode(TEXT("GradeSel.xml"), 1, 0, g_CustomRule.gradeRange[0].lRoomTotalBetMax);

	for (WORD i = 1; i < MAX_GRADE - 1; i++)
	{
		ModifyXMLGradeSelNode(TEXT("GradeSel.xml"), i + 1, g_CustomRule.gradeRange[i].lRoomTotalBetMin, g_CustomRule.gradeRange[i].lRoomTotalBetMax);
	}

	ModifyXMLGradeSelNode(TEXT("GradeSel.xml"), 5, g_CustomRule.gradeRange[4].lRoomTotalBetMin, g_CustomRule.gradeRange[4].lRoomTotalBetMax);

	//库存范围对应收放表类型
	for (WORD i = 0; i < MAX_GRADE; i++)
	{
		ModifyXMLStorageRangeNode(TEXT("StorageRange.xml"), i + 1, 1, 0, g_CustomRule.storageRange[i][0].lStorageMax);

		for (WORD j = 1; j < MAX_TABLE_TYPE - 1; j++)
		{
			ModifyXMLStorageRangeNode(TEXT("StorageRange.xml"), i + 1, j + 1, g_CustomRule.storageRange[i][j].lStorageMin, g_CustomRule.storageRange[i][j].lStorageMax);
		}

		ModifyXMLStorageRangeNode(TEXT("StorageRange.xml"), i + 1, 9, g_CustomRule.storageRange[i][8].lStorageMin, g_CustomRule.storageRange[i][8].lStorageMax);
	}

	AfxMessageBox(TEXT("保存XML配置成功"), MB_ICONINFORMATION);
}

void CServerConfigDlg::OnBnClickedButtonRead()
{
	//读取XML通用配置 
	ReadXMLFile(TEXT("GeneralConfig.xml"));

	//读取XML收放表
	ReadXMLFile(TEXT("GradeConfig_1.xml"));
	ReadXMLFile(TEXT("GradeConfig_2.xml"));
	ReadXMLFile(TEXT("GradeConfig_3.xml"));
	ReadXMLFile(TEXT("GradeConfig_4.xml"));
	ReadXMLFile(TEXT("GradeConfig_5.xml"));

	//读取表输赢
	ReadXMLFile(TEXT("TableWinLost.xml"));

	//读取挡位选择
	ReadXMLFile(TEXT("GradeSel.xml"));

	//读取库存范围
	ReadXMLFile(TEXT("StorageRange.xml"));

	//更新控件
	m_DialogCustom[CUSTOM_GENERAL]->UpdateData(false);
	m_DialogCustom[CUSTOM_ADVANCED]->UpdateData(false);
	m_DialogCustom[CUSTOM_ADVANCED]->OnTableQuery();
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

	if (lTableWinScore < 1000000 && lTableWinScore > 0)
	{
		return POSITIVE_TENPERCENT;
	}
	else if (lTableWinScore >= 1000000 && lTableWinScore < 2500000)
	{
		return POSITIVE_TWENTYPERCENT;
	}
	else if (lTableWinScore >= 2500000 && lTableWinScore < 7500000)
	{
		return POSITIVE_THIRTYPERCENT;
	}
	else if (lTableWinScore >= 7500000)
	{
		POSITIVE_FORTYPERCENT;
	}

	if (lTableWinScore <0 && lTableWinScore > -1000000)
	{
		return NEGTIVE_TENPERCENT;
	}
	else if (lTableWinScore <= -1000000 && lTableWinScore > -2500000)
	{
		return NEGTIVE_TWENTYPERCENT;
	}
	else if (lTableWinScore <= -2500000 && lTableWinScore > -7500000)
	{
		return NEGTIVE_THIRTYPERCENT;
	}
	else if (lTableWinScore <= -7500000)
	{
		return NEGTIVE_FORTYPERCENT;
	}

	return EQUAL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 控制对话框
IMPLEMENT_DYNAMIC(CDialogGeneral, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogGeneral, CDialogDexter)
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
	DDX_Text(pDX, IDC_EDIT_TRANS_STORPER, g_CustomRule.lTransferStoragePercent);
	DDX_Text(pDX, IDC_EDIT_TRANS_CHARGEPER, g_CustomRule.lTransferChargePercent);
}

// 初始化窗口
BOOL CDialogGeneral::OnInitDialog()
{
    // 初始化窗口
    __super::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_STORAGE))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_TRANS_STORPER))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_TRANS_CHARGEPER))->LimitText(2);

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// 控制对话框
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
	ON_BN_CLICKED(IDC_BUTTON_SETGRADERANGE, &CDialogAdvanced::OnBnClickedButtonSetgraderange)
	ON_BN_CLICKED(IDC_BUTTON_SETSTORAGERANGE, &CDialogAdvanced::OnBnClickedButtonSetstoragerange)
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

	DDX_Text(pDX, IDC_EDIT_GRADEINDEX_MAX0, g_CustomRule.gradeRange[0].lRoomTotalBetMax);
	DDX_Text(pDX, IDC_EDIT_GRADEINDEX_MIN1, g_CustomRule.gradeRange[1].lRoomTotalBetMin);
	DDX_Text(pDX, IDC_EDIT_GRADEINDEX_MAX1, g_CustomRule.gradeRange[1].lRoomTotalBetMax);
	DDX_Text(pDX, IDC_EDIT_GRADEINDEX_MIN2, g_CustomRule.gradeRange[2].lRoomTotalBetMin);
	DDX_Text(pDX, IDC_EDIT_GRADEINDEX_MAX2, g_CustomRule.gradeRange[2].lRoomTotalBetMax);
	DDX_Text(pDX, IDC_EDIT_GRADEINDEX_MIN3, g_CustomRule.gradeRange[3].lRoomTotalBetMin);
	DDX_Text(pDX, IDC_EDIT_GRADEINDEX_MAX3, g_CustomRule.gradeRange[3].lRoomTotalBetMax);
	DDX_Text(pDX, IDC_EDIT_GRADEINDEX_MIN4, g_CustomRule.gradeRange[4].lRoomTotalBetMin);

	CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
	int nComboGradeSel = (pComboGradeSel->GetCurSel() == -1 ? 0 : pComboGradeSel->GetCurSel());

	DDX_Text(pDX, IDC_EDIT_STORAGE_MAX_0, g_CustomRule.storageRange[nComboGradeSel][0].lStorageMax);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MIN_1, g_CustomRule.storageRange[nComboGradeSel][1].lStorageMin);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MAX_1, g_CustomRule.storageRange[nComboGradeSel][1].lStorageMax);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MIN_2, g_CustomRule.storageRange[nComboGradeSel][2].lStorageMin);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MAX_2, g_CustomRule.storageRange[nComboGradeSel][2].lStorageMax);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MIN_3, g_CustomRule.storageRange[nComboGradeSel][3].lStorageMin);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MAX_3, g_CustomRule.storageRange[nComboGradeSel][3].lStorageMax);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MIN_4, g_CustomRule.storageRange[nComboGradeSel][4].lStorageMin);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MAX_4, g_CustomRule.storageRange[nComboGradeSel][4].lStorageMax);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MIN_5, g_CustomRule.storageRange[nComboGradeSel][5].lStorageMin);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MAX_5, g_CustomRule.storageRange[nComboGradeSel][5].lStorageMax);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MIN_6, g_CustomRule.storageRange[nComboGradeSel][6].lStorageMin);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MAX_6, g_CustomRule.storageRange[nComboGradeSel][6].lStorageMax);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MIN_7, g_CustomRule.storageRange[nComboGradeSel][7].lStorageMin);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MAX_7, g_CustomRule.storageRange[nComboGradeSel][7].lStorageMax);
	DDX_Text(pDX, IDC_EDIT_STORAGE_MIN_8, g_CustomRule.storageRange[nComboGradeSel][8].lStorageMin);
}

// 初始化窗口
BOOL CDialogAdvanced::OnInitDialog()
{
    // 初始化窗口
    __super::OnInitDialog();

	for (WORD i = IDC_EDIT_GRADEINDEX_MAX0; i <= IDC_EDIT_GRADEINDEX_MIN4; i++)
	{
		((CEdit*)GetDlgItem(i))->LimitText(13);
	}

	for (WORD i = IDC_EDIT_STORAGE_MAX_0; i <= IDC_EDIT_STORAGE_MIN_8; i++)
	{
		((CEdit*)GetDlgItem(i))->LimitText(13);
	}

	((CEdit*)GetDlgItem(IDC_EDIT_DEBUG_VALUE))->LimitText(9);

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
	
	//设置库存范围
	SetDlgItemInt(IDC_EDIT_STORAGE_MAX_0, g_CustomRule.storageRange[nComboGradeSel][0].lStorageMax);
	WORD wCtrlID = IDC_EDIT_STORAGE_MIN_1;
	for (WORD i = 1; i < MAX_TABLE_TYPE - 1; i++)
	{
		SetDlgItemInt(wCtrlID, g_CustomRule.storageRange[nComboGradeSel][i].lStorageMin);
		SetDlgItemInt(wCtrlID + 1, g_CustomRule.storageRange[nComboGradeSel][i].lStorageMax);
		wCtrlID += 2;
	}
	SetDlgItemInt(IDC_EDIT_STORAGE_MIN_8, g_CustomRule.storageRange[nComboGradeSel][8].lStorageMin);

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

	if (lTableWinScore < 1000000 && lTableWinScore > 0)
	{
		return POSITIVE_TENPERCENT;
	}
	else if (lTableWinScore >= 1000000 && lTableWinScore < 2500000)
	{
		return POSITIVE_TWENTYPERCENT;
	}
	else if (lTableWinScore >= 2500000 && lTableWinScore < 7500000)
	{
		return POSITIVE_THIRTYPERCENT;
	}
	else if (lTableWinScore >= 7500000)
	{
		POSITIVE_FORTYPERCENT;
	}

	if (lTableWinScore <0 && lTableWinScore > -1000000)
	{
		return NEGTIVE_TENPERCENT;
	}
	else if (lTableWinScore <= -1000000 && lTableWinScore > -2500000)
	{
		return NEGTIVE_TWENTYPERCENT;
	}
	else if (lTableWinScore <= -2500000 && lTableWinScore > -7500000)
	{
		return NEGTIVE_THIRTYPERCENT;
	}
	else if (lTableWinScore <= -7500000)
	{
		return NEGTIVE_FORTYPERCENT;
	}

	return EQUAL;
}

//获取控件数据
SCORE CDialogAdvanced::GetCtrlIDInter(WORD wCtrlID)
{
	CString strCtrlVal1;
	GetDlgItemText(wCtrlID, strCtrlVal1);

	//校验是否为空值
	if (strCtrlVal1 == TEXT(""))
	{
		return 0;
	}

	strCtrlVal1.TrimLeft();
	strCtrlVal1.TrimRight();

	LPTSTR lpszCtrlVal1 = new TCHAR[strCtrlVal1.GetLength() + 1];
	_tcscpy_s(lpszCtrlVal1, strCtrlVal1.GetLength() + 1, strCtrlVal1);
	SCORE lCtrlVal1 = _ttoi64(lpszCtrlVal1);

	delete lpszCtrlVal1;

	return lCtrlVal1;
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

	//设置库存范围
	SetDlgItemInt(IDC_EDIT_STORAGE_MAX_0, g_CustomRule.storageRange[nComboGradeSel][0].lStorageMax);
	WORD wCtrlID = IDC_EDIT_STORAGE_MIN_1;
	for (WORD i = 1; i < MAX_TABLE_TYPE - 1; i++)
	{
		SetDlgItemInt(wCtrlID, g_CustomRule.storageRange[nComboGradeSel][i].lStorageMin);
		SetDlgItemInt(wCtrlID + 1, g_CustomRule.storageRange[nComboGradeSel][i].lStorageMax);
		wCtrlID += 2;
	}
	SetDlgItemInt(IDC_EDIT_STORAGE_MIN_8, g_CustomRule.storageRange[nComboGradeSel][8].lStorageMin);
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

	//以额度为准来调整难度
	CString strlDifficulty;
	GetDlgItemText(IDC_EDIT_DIFFICULTY, strlDifficulty);

	//校验是否为空值
	if (strlDifficulty != TEXT(""))
	{
		strlDifficulty.TrimLeft();
		strlDifficulty.TrimRight();

		LPTSTR lpszDifficulty = new TCHAR[strlDifficulty.GetLength() + 1];
		_tcscpy_s(lpszDifficulty, strlDifficulty.GetLength() + 1, strlDifficulty);
		SCORE lDifficulty = _ttoi64(lpszDifficulty);
		delete[] lpszDifficulty;

		//调整额度
		if ((lDifficulty > 50 && lDebugValue < 0 && lDifficulty != 0) || (lDifficulty < 50 && lDebugValue > 0 && lDifficulty != 0))
		{
			lDifficulty = 100 - lDifficulty;
			SetDlgItemInt(IDC_EDIT_DIFFICULTY, lDifficulty);
		}
	}

	delete[] lpszDebugValue;
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

	//赋值难度
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].lDifficulty = lDifficulty;
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].bChange = true;

	//以难度为准来调整额度的正负值
	CString strlDebugValue;
	GetDlgItemText(IDC_EDIT_DEBUG_VALUE, strlDebugValue);

	//校验是否为空值
	if (strlDebugValue != TEXT(""))
	{
		strlDebugValue.TrimLeft();
		strlDebugValue.TrimRight();

		//获取难度值
		LPTSTR lpszDebugValue = new TCHAR[strlDebugValue.GetLength() + 1];
		_tcscpy_s(lpszDebugValue, strlDebugValue.GetLength() + 1, strlDebugValue);
		SCORE lDebugValue = _ttoi64(lpszDebugValue);
		delete[] lpszDebugValue;

		//调整正负值
		if ((lDifficulty > 50 && lDebugValue < 0) || (lDifficulty < 50 && lDebugValue > 0))
		{
			lDebugValue = -lDebugValue;
			SetDlgItemInt(IDC_EDIT_DEBUG_VALUE, lDebugValue);
		}
	}

	delete[] lpszDifficulty;
}

void CDialogAdvanced::OnBnClickedButtonSetgraderange()
{
	for (WORD i = IDC_EDIT_GRADEINDEX_MAX0; i < IDC_EDIT_GRADEINDEX_MIN4; i++)
	{
		SCORE lCtrlVal1 = GetCtrlIDInter(i);
		SCORE lCtrlVal2 = GetCtrlIDInter(i + 1);

		if (lCtrlVal1 == 0 || lCtrlVal2 == 0)
		{
			AfxMessageBox(TEXT("请填写数据"), MB_ICONSTOP);

			return;
		}

		if (lCtrlVal1 >= lCtrlVal2)
		{
			AfxMessageBox(TEXT("数据大小不正确"), MB_ICONSTOP);

			return;
		}
	}

	//获取数据
	g_CustomRule.gradeRange[0].lRoomTotalBetMax = GetCtrlIDInter(IDC_EDIT_GRADEINDEX_MAX0);
	WORD wCtrlID = IDC_EDIT_GRADEINDEX_MIN1;
	for (WORD i = 1; i < MAX_GRADE - 1; i++)
	{
		g_CustomRule.gradeRange[i].lRoomTotalBetMin = GetCtrlIDInter(wCtrlID);
		g_CustomRule.gradeRange[i].lRoomTotalBetMax = GetCtrlIDInter(wCtrlID + 1);
		wCtrlID += 2;
	}
	g_CustomRule.gradeRange[4].lRoomTotalBetMin = GetCtrlIDInter(IDC_EDIT_GRADEINDEX_MIN4);

	AfxMessageBox(TEXT("数据保存成功"), MB_ICONINFORMATION);
}

void CDialogAdvanced::OnBnClickedButtonSetstoragerange()
{
	for (WORD i = IDC_EDIT_STORAGE_MAX_0; i < IDC_EDIT_STORAGE_MIN_8; i++)
	{
		SCORE lCtrlVal1 = GetCtrlIDInter(i);
		SCORE lCtrlVal2 = GetCtrlIDInter(i + 1);

		if (lCtrlVal1 == 0 || lCtrlVal2 == 0)
		{
			AfxMessageBox(TEXT("请填写数据"), MB_ICONSTOP);

			return;
		}

		if (lCtrlVal1 >= lCtrlVal2)
		{
			AfxMessageBox(TEXT("数据大小不正确"), MB_ICONSTOP);

			return;
		}
	}

	//获取数据
	CComboBox *pComboGradeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADESEL));
	int nComboGradeSel = pComboGradeSel->GetCurSel();

	g_CustomRule.storageRange[nComboGradeSel][0].lStorageMax = GetCtrlIDInter(IDC_EDIT_STORAGE_MAX_0);
	WORD wCtrlID = IDC_EDIT_STORAGE_MIN_1;
	for (WORD i = 1; i < MAX_TABLE_TYPE - 1; i++)
	{
		g_CustomRule.storageRange[nComboGradeSel][i].lStorageMin = GetCtrlIDInter(wCtrlID);
		g_CustomRule.storageRange[nComboGradeSel][i].lStorageMax = GetCtrlIDInter(wCtrlID + 1);
		wCtrlID += 2;
	}
	g_CustomRule.storageRange[nComboGradeSel][8].lStorageMin = GetCtrlIDInter(IDC_EDIT_STORAGE_MIN_8);

	AfxMessageBox(TEXT("数据保存成功"), MB_ICONINFORMATION);
}


