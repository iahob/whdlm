#pragma once

//��Ϸ���ƻ���
class IClientControlDlg : public CSkinDialog
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}
};
