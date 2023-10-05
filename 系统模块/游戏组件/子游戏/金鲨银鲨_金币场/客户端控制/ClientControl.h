#pragma once

//游戏控制基类
class IClientControlDlg : public CSkinDialog
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}
};
