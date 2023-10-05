#include "StdAfx.h"
#include "DialogCustom.h"
#include ".\dialogcustom.h"

// 自定配置
tagCustomRule				g_CustomRule;

BEGIN_MESSAGE_MAP(CDialogCustom2, CDialogDexter)
	ON_BN_CLICKED(IDC_CHECK_TIME, OnBnClickedCheckTime)
	ON_EN_CHANGE(IDC_EDIT_SYSTEM_STORAGE, OnEnChangeEditSystemStorage)
	ON_EN_CHANGE(IDC_EDIT_USER_STORAGE, OnEnChangeEditUserStorage)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER_K, OnEnChangeEditParameterK)
END_MESSAGE_MAP()

void CDialogCustom2::OnBnClickedCheckTime()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_TIME))->GetCheck()==BST_CHECKED)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_63))->EnableWindow(TRUE);
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_EDIT_63))->EnableWindow(FALSE);
	}
}

void CDialogCustom2::OnEnChangeEditSystemStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("中奖率：%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_SYSTEM, strValue);
}

void CDialogCustom2::OnEnChangeEditUserStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	WORD wUserWinChance = CalcWinChance(lSystemStorage, lUserStorage, nParameterK);
	strValue.Format(TEXT("中奖率：%d%%"), wUserWinChance);
	SetDlgItemText(IDC_STATIC_SYSTEM, strValue);
}

void CDialogCustom2::OnEnChangeEditParameterK()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	WORD wUserWinChance = CalcWinChance(lSystemStorage, lUserStorage, nParameterK);
	strValue.Format(TEXT("中奖率：%d%%"), wUserWinChance);
	SetDlgItemText(IDC_STATIC_SYSTEM, strValue);
}