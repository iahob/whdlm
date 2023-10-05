#include "StdAfx.h"
#include "DialogCustom.h"

// ×Ô¶¨ÅäÖÃ
tagCustomRule				g_CustomRule;

BEGIN_MESSAGE_MAP(CDialogCustom2, CDialogDexter)
    ON_BN_CLICKED(IDC_CHECK_TIME, OnBnClickedCheckTime)
END_MESSAGE_MAP()

void CDialogCustom2::OnBnClickedCheckTime()
{
    if(((CButton *)GetDlgItem(IDC_CHECK_TIME))->GetCheck() == BST_CHECKED)
    {
        ((CEdit *)GetDlgItem(IDC_EDIT_63))->EnableWindow(TRUE);
    }
    else
    {
        ((CEdit *)GetDlgItem(IDC_EDIT_63))->EnableWindow(FALSE);
    }
}