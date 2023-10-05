// FishConfigDlg.h : 头文件
//

#pragma once
#include "DialogCustom.h"

#define CUSTOM_MULTIPLE			0
#define CUSTOM_STOCK_DEBUG	1
#define CUSTOM_SCENES			2
#define CUSTOM_ACTIVITY			3
#define CUSTOM_FISH				4
#define CUSTOM_ZHONG_JIAN		5
#define CUSTOM_YUAN_BAO			100
#define CUSTOM_YUANBAO			200
#define MAX_CUSTOM				6

// 主对话框
class CFishConfigDlg : public CDialog
{

	// 变量定义
public:
	CString						m_StrFilePath;		
	CDialogDexter *				m_DialogCustom[MAX_CUSTOM];

	// 类函数
public:
	// 构造函数
	CFishConfigDlg(CWnd* pParent = NULL);	

	// 对话框数据
	enum { IDD = IDD_FISHCONFIG_DIALOG };

	// 实现
protected:
	// 控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);
	// 初始化
	virtual BOOL OnInitDialog();
	// 变换选项
	afx_msg void OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult);
	// 窗口销毁
	afx_msg void OnDestroy();
	// 默认按钮
	afx_msg void OnBnClickedButton3();
	// 打开按钮
	afx_msg void OnBnClickedButton2();
	// 保存按钮
	afx_msg void OnBnClickedButton1();

	DECLARE_MESSAGE_MAP()
};
