//藏宝库IT社区（28xin.com）

#pragma once
#include "Stdafx.h"
#include "Resource.h"
// CRoomControlDlg 对话框

class CRoomControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CRoomControlDlg)

public:
	CRoomControlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRoomControlDlg();

// 对话框数据
	enum { IDD = IDD_DLG_ROOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
