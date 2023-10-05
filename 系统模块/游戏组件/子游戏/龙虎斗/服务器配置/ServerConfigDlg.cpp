// ServerConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ServerConfigDlg.h"
#include "..\��Ϣ����\CMD_Game.h"

tagCustomRule					g_CustomRule;						// ��Ϸ����
MSXML2::IXMLDOMDocument2Ptr	g_plDomDocument;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServerConfigDlg, CDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTabOptisons)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CServerConfigDlg::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_READ, &CServerConfigDlg::OnBnClickedButtonRead)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//����Ŀ¼
bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
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

//��ȡ�շű�����
EM_TABLE_TYPE GetTableType(LONGLONG lTableWinScore)
{
	//��Ӯ >= 100WΪ��������Ӯ>0��С��100WΪ���� ��Ӯ����0Ϊƽ���
	//��Ӯ<0��>-100WΪ������Ӯ<=-100wΪ�󸺱�
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
		//�ڵ�����
		_variant_t strNodeName = pChild->nodeName;

		//�ڵ�����
		MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

		//��������
		LONG lAttrListLen = pDOMAttrList->Getlength();
		if (lAttrListLen > 0)
		{
			//��ȡ��һ�����Ե�����
			MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(0);
			CString strNodeElement = ((LPCTSTR)pDOMAttr->nodeName);

			//�շż�¼
			//LineId��ʶΨһһ����¼����һλ��ʾ�����ڶ�����λ��ʾ�����嵽��λ��ʾ��
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
			//����Ӯ
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
			//��λѡ��
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
			//��淶Χ
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
			//ͨ������
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
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//����XML�ļ�
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//���ڵ�
		MSXML2::IXMLDOMNodePtr pXMLRoot = g_plDomDocument->documentElement;
		//���ڵ�����
		_variant_t strRootName = pXMLRoot->nodeName;

		//�ݹ����
		TravelXMLChildNode(pXMLRoot);
	}
}

//�޸��շű�
void ModifyXMLNode(CString strXMLFileName, CString strAttributeNode, SCORE lCtrlVal, SCORE lDifficulty)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//����XML�ļ�
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//���ڵ�
		MSXML2::IXMLDOMNodePtr pParent = g_plDomDocument->documentElement;

		for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
		{
			//�ڵ�����
			_variant_t strNodeName = pChild->nodeName;

			//�ڵ�����
			MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

			//��������
			if (pDOMAttrList->Getlength() > 0)
			{
				MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(0);

				if (pDOMAttr->GetnodeTypedValue() == strAttributeNode)
				{
					//���ö��
					pDOMAttr = pDOMAttrList->Getitem(1);
					_variant_t val = (__int64)lCtrlVal;
					pDOMAttr->PutnodeTypedValue(val);

					//�����Ѷ�
					pDOMAttr = pDOMAttrList->Getitem(2);
					val = (__int64)lDifficulty;
					pDOMAttr->PutnodeTypedValue(val);

					break;
				}
			}
		}

		//����XML�ļ�
		g_plDomDocument->save(bstrFileName);
	}
}

//�޸�ͨ������ 
void ModifyXMLNode(CString strXMLFileName, CString strAttributeNode, SCORE lVal)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//����XML�ļ�
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//���ڵ�
		MSXML2::IXMLDOMNodePtr pParent = g_plDomDocument->documentElement;

		for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
		{
			//�ڵ�����
			_variant_t strNodeName = pChild->nodeName;

			//�ڵ�����
			MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

			//��������
			if (pDOMAttrList->Getlength() > 0)
			{
				MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(1);
				CString strNodeElement = ((LPCTSTR)pDOMAttr->nodeName);
				if (strNodeElement == strAttributeNode)
				{
					//����ֵ
					_variant_t val = (__int64)lVal;
					pDOMAttr->PutnodeTypedValue(val);

					break;
				}
			}
		}

		//����XML�ļ�
		g_plDomDocument->save(bstrFileName);
	}
}

//�޸ı���Ӯ
void ModifyXMLNode(CString strXMLFileName, SCORE lGradeWinLostID, SCORE lTableId, SCORE lSum)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//����XML�ļ�
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//���ڵ�
		MSXML2::IXMLDOMNodePtr pParent = g_plDomDocument->documentElement;

		for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
		{
			//�ڵ�����
			_variant_t strNodeName = pChild->nodeName;

			//�ڵ�����
			MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

			//��������
			if (pDOMAttrList->Getlength() > 0)
			{
				MSXML2::IXMLDOMNodePtr pDOMAttr1 = pDOMAttrList->Getitem(0);
				SCORE lTempGradeWinLostID = pDOMAttr1->GetnodeTypedValue();

				MSXML2::IXMLDOMNodePtr pDOMAttr2 = pDOMAttrList->Getitem(1);
				SCORE lTempTableId = pDOMAttr2->GetnodeTypedValue();

				if (lTempGradeWinLostID == lGradeWinLostID && lTempTableId == lTableId)
				{
					//����ֵ
					_variant_t val = (__int64)lSum;
					MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(2);
					pDOMAttr->PutnodeTypedValue(val);

					break;
				}
			}
		}

		//����XML�ļ�
		g_plDomDocument->save(bstrFileName);
	}
}

//�޸ĵ�λѡ��
void ModifyXMLGradeSelNode(CString strXMLFileName, SCORE lGradeSelID, SCORE lRoomTotalBetMin, SCORE lRoomTotalBetMax)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//����XML�ļ�
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//���ڵ�
		MSXML2::IXMLDOMNodePtr pParent = g_plDomDocument->documentElement;

		for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
		{
			//�ڵ�����
			_variant_t strNodeName = pChild->nodeName;

			//�ڵ�����
			MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

			//��������
			if (pDOMAttrList->Getlength() > 0)
			{
				MSXML2::IXMLDOMNodePtr pDOMAttr1 = pDOMAttrList->Getitem(0);
				SCORE lTempGradeSelID = pDOMAttr1->GetnodeTypedValue();

				if (lTempGradeSelID == lGradeSelID)
				{
					//����ֵ
					_variant_t val = (__int64)lRoomTotalBetMin;
					MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(1);
					pDOMAttr->PutnodeTypedValue(val);

					//����ֵ
					val = (__int64)lRoomTotalBetMax;
					pDOMAttr = pDOMAttrList->Getitem(2);
					pDOMAttr->PutnodeTypedValue(val);

					break;
				}
			}
		}

		//����XML�ļ�
		g_plDomDocument->save(bstrFileName);
	}
}

//�޸Ŀ�淶Χ
void ModifyXMLStorageRangeNode(CString strXMLFileName, SCORE lGradeStorageID, SCORE lStorageRangeID, SCORE lStorageMin, SCORE lStorageMax)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	CString szXMLInfoPath;
	szXMLInfoPath.Format(TEXT("%s\\%s"), szDirectory, strXMLFileName);
	_bstr_t bstrFileName = szXMLInfoPath.AllocSysString();

	//����XML�ļ�
	variant_t vResult = g_plDomDocument->load(bstrFileName);

	if (((bool)vResult) == TRUE)
	{
		//���ڵ�
		MSXML2::IXMLDOMNodePtr pParent = g_plDomDocument->documentElement;

		for (MSXML2::IXMLDOMNodePtr pChild = pParent->firstChild; pChild != NULL; pChild = pChild->nextSibling)
		{
			//�ڵ�����
			_variant_t strNodeName = pChild->nodeName;

			//�ڵ�����
			MSXML2::IXMLDOMNamedNodeMapPtr pDOMAttrList = pChild->Getattributes();

			//��������
			if (pDOMAttrList->Getlength() > 0)
			{
				MSXML2::IXMLDOMNodePtr pDOMAttr1 = pDOMAttrList->Getitem(0);
				SCORE lTempGradeStorageID = pDOMAttr1->GetnodeTypedValue();

				MSXML2::IXMLDOMNodePtr pDOMAttr2 = pDOMAttrList->Getitem(1);
				SCORE lTempStorageRangeID = pDOMAttr2->GetnodeTypedValue();

				if (lTempGradeStorageID == lGradeStorageID && lTempStorageRangeID == lStorageRangeID)
				{
					//����ֵ
					_variant_t val = (__int64)lStorageMin;
					MSXML2::IXMLDOMNodePtr pDOMAttr = pDOMAttrList->Getitem(2);
					pDOMAttr->PutnodeTypedValue(val);

					//����ֵ
					val = (__int64)lStorageMax;
					pDOMAttr = pDOMAttrList->Getitem(3);
					pDOMAttr->PutnodeTypedValue(val);

					break;
				}
			}
		}

		//����XML�ļ�
		g_plDomDocument->save(bstrFileName);
	}
}

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

	//XML COM�ĳ�ʼ��
	::CoInitialize(NULL);
	HRESULT hr = g_plDomDocument.CreateInstance(MSXML2::CLSID_DOMDocument);

	if (FAILED(hr))
	{
		_com_error er(hr);
		return false;
	}

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
	if (g_CustomRule.lTransferStoragePercent + g_CustomRule.lTransferChargePercent != 100)
	{
		AfxMessageBox(TEXT("����ˮת�Ʊ���֮��Ϊ100�����������ã�"), MB_ICONSTOP);
		return;
	}

	//д��XMLͨ������
	ModifyXMLNode(TEXT("GeneralConfig.xml"), TEXT("lConfigStorage"), g_CustomRule.lConfigStorage);
	ModifyXMLNode(TEXT("GeneralConfig.xml"), TEXT("lTransferStoragePercent"), g_CustomRule.lTransferStoragePercent);
	ModifyXMLNode(TEXT("GeneralConfig.xml"), TEXT("lTransferChargePercent"), g_CustomRule.lTransferChargePercent);

	//д��XML�շű� 
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
				//д��XML����Ӯ
				ModifyXMLNode(TEXT("TableWinLost.xml"), wGradeIndex + 1, wTableIndex + 1, lTableWinScore);
			}
		}
	}

	//��ˮ��Χ��Ӧ��λ
	ModifyXMLGradeSelNode(TEXT("GradeSel.xml"), 1, 0, g_CustomRule.gradeRange[0].lRoomTotalBetMax);

	for (WORD i = 1; i < MAX_GRADE - 1; i++)
	{
		ModifyXMLGradeSelNode(TEXT("GradeSel.xml"), i + 1, g_CustomRule.gradeRange[i].lRoomTotalBetMin, g_CustomRule.gradeRange[i].lRoomTotalBetMax);
	}

	ModifyXMLGradeSelNode(TEXT("GradeSel.xml"), 5, g_CustomRule.gradeRange[4].lRoomTotalBetMin, g_CustomRule.gradeRange[4].lRoomTotalBetMax);

	//��淶Χ��Ӧ�շű�����
	for (WORD i = 0; i < MAX_GRADE; i++)
	{
		ModifyXMLStorageRangeNode(TEXT("StorageRange.xml"), i + 1, 1, 0, g_CustomRule.storageRange[i][0].lStorageMax);

		for (WORD j = 1; j < MAX_TABLE_TYPE - 1; j++)
		{
			ModifyXMLStorageRangeNode(TEXT("StorageRange.xml"), i + 1, j + 1, g_CustomRule.storageRange[i][j].lStorageMin, g_CustomRule.storageRange[i][j].lStorageMax);
		}

		ModifyXMLStorageRangeNode(TEXT("StorageRange.xml"), i + 1, 9, g_CustomRule.storageRange[i][8].lStorageMin, g_CustomRule.storageRange[i][8].lStorageMax);
	}

	AfxMessageBox(TEXT("����XML���óɹ�"), MB_ICONINFORMATION);
}

void CServerConfigDlg::OnBnClickedButtonRead()
{
	//��ȡXMLͨ������ 
	ReadXMLFile(TEXT("GeneralConfig.xml"));

	//��ȡXML�շű�
	ReadXMLFile(TEXT("GradeConfig_1.xml"));
	ReadXMLFile(TEXT("GradeConfig_2.xml"));
	ReadXMLFile(TEXT("GradeConfig_3.xml"));
	ReadXMLFile(TEXT("GradeConfig_4.xml"));
	ReadXMLFile(TEXT("GradeConfig_5.xml"));

	//��ȡ����Ӯ
	ReadXMLFile(TEXT("TableWinLost.xml"));

	//��ȡ��λѡ��
	ReadXMLFile(TEXT("GradeSel.xml"));

	//��ȡ��淶Χ
	ReadXMLFile(TEXT("StorageRange.xml"));

	//���¿ؼ�
	m_DialogCustom[CUSTOM_GENERAL]->UpdateData(false);
	m_DialogCustom[CUSTOM_ADVANCED]->UpdateData(false);
	m_DialogCustom[CUSTOM_ADVANCED]->OnTableQuery();
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

// ���ƶԻ���
IMPLEMENT_DYNAMIC(CDialogGeneral, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogGeneral, CDialogDexter)
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
	DDX_Text(pDX, IDC_EDIT_TRANS_STORPER, g_CustomRule.lTransferStoragePercent);
	DDX_Text(pDX, IDC_EDIT_TRANS_CHARGEPER, g_CustomRule.lTransferChargePercent);
}

// ��ʼ������
BOOL CDialogGeneral::OnInitDialog()
{
    // ��ʼ������
    __super::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_STORAGE))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_TRANS_STORPER))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_TRANS_CHARGEPER))->LimitText(2);

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// ���ƶԻ���
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

// ��ʼ������
BOOL CDialogAdvanced::OnInitDialog()
{
    // ��ʼ������
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
	
	//���ÿ�淶Χ
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

//��ȡ�ؼ�����
SCORE CDialogAdvanced::GetCtrlIDInter(WORD wCtrlID)
{
	CString strCtrlVal1;
	GetDlgItemText(wCtrlID, strCtrlVal1);

	//У���Ƿ�Ϊ��ֵ
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

	//���ÿ�淶Χ
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

	//�Զ��Ϊ׼�������Ѷ�
	CString strlDifficulty;
	GetDlgItemText(IDC_EDIT_DIFFICULTY, strlDifficulty);

	//У���Ƿ�Ϊ��ֵ
	if (strlDifficulty != TEXT(""))
	{
		strlDifficulty.TrimLeft();
		strlDifficulty.TrimRight();

		LPTSTR lpszDifficulty = new TCHAR[strlDifficulty.GetLength() + 1];
		_tcscpy_s(lpszDifficulty, strlDifficulty.GetLength() + 1, strlDifficulty);
		SCORE lDifficulty = _ttoi64(lpszDifficulty);
		delete[] lpszDifficulty;

		//�������
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

	//��ֵ�Ѷ�
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].lDifficulty = lDifficulty;
	g_CustomRule.lConfigGradeArray[nComboGradeSel].lTableArray[nComboTableSel].debugLineArray[nComboLineSel].bChange = true;

	//���Ѷ�Ϊ׼��������ȵ�����ֵ
	CString strlDebugValue;
	GetDlgItemText(IDC_EDIT_DEBUG_VALUE, strlDebugValue);

	//У���Ƿ�Ϊ��ֵ
	if (strlDebugValue != TEXT(""))
	{
		strlDebugValue.TrimLeft();
		strlDebugValue.TrimRight();

		//��ȡ�Ѷ�ֵ
		LPTSTR lpszDebugValue = new TCHAR[strlDebugValue.GetLength() + 1];
		_tcscpy_s(lpszDebugValue, strlDebugValue.GetLength() + 1, strlDebugValue);
		SCORE lDebugValue = _ttoi64(lpszDebugValue);
		delete[] lpszDebugValue;

		//��������ֵ
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
			AfxMessageBox(TEXT("����д����"), MB_ICONSTOP);

			return;
		}

		if (lCtrlVal1 >= lCtrlVal2)
		{
			AfxMessageBox(TEXT("���ݴ�С����ȷ"), MB_ICONSTOP);

			return;
		}
	}

	//��ȡ����
	g_CustomRule.gradeRange[0].lRoomTotalBetMax = GetCtrlIDInter(IDC_EDIT_GRADEINDEX_MAX0);
	WORD wCtrlID = IDC_EDIT_GRADEINDEX_MIN1;
	for (WORD i = 1; i < MAX_GRADE - 1; i++)
	{
		g_CustomRule.gradeRange[i].lRoomTotalBetMin = GetCtrlIDInter(wCtrlID);
		g_CustomRule.gradeRange[i].lRoomTotalBetMax = GetCtrlIDInter(wCtrlID + 1);
		wCtrlID += 2;
	}
	g_CustomRule.gradeRange[4].lRoomTotalBetMin = GetCtrlIDInter(IDC_EDIT_GRADEINDEX_MIN4);

	AfxMessageBox(TEXT("���ݱ���ɹ�"), MB_ICONINFORMATION);
}

void CDialogAdvanced::OnBnClickedButtonSetstoragerange()
{
	for (WORD i = IDC_EDIT_STORAGE_MAX_0; i < IDC_EDIT_STORAGE_MIN_8; i++)
	{
		SCORE lCtrlVal1 = GetCtrlIDInter(i);
		SCORE lCtrlVal2 = GetCtrlIDInter(i + 1);

		if (lCtrlVal1 == 0 || lCtrlVal2 == 0)
		{
			AfxMessageBox(TEXT("����д����"), MB_ICONSTOP);

			return;
		}

		if (lCtrlVal1 >= lCtrlVal2)
		{
			AfxMessageBox(TEXT("���ݴ�С����ȷ"), MB_ICONSTOP);

			return;
		}
	}

	//��ȡ����
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

	AfxMessageBox(TEXT("���ݱ���ɹ�"), MB_ICONINFORMATION);
}


