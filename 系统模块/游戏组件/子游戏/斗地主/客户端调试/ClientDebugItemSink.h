#pragma once
#include "Stdafx.h"
#include "ClientDebug.h"

class AFX_EXT_CLASS CClientDebugItemSinkDlg : public IClientDebugDlg, public IClientDebug
{
	DECLARE_DYNAMIC(CClientDebugItemSinkDlg)
protected:
	CWnd 							*m_pParentWnd;					// 父窗口
	IClientDebugCallback 			*m_pIClientDebugCallback;		// 回调接口

public:
	CClientDebugItemSinkDlg(CWnd *pParent = NULL); 
	virtual ~CClientDebugItemSinkDlg();

	enum { IDD = IDD_CLIENT_DEBUG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	// 继承函数
public:
	// 释放接口
	virtual void Release();
	// 创建函数
	virtual bool Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback);
	// 显示窗口
	virtual bool ShowWindow(bool bShow);
	// 消息函数
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);
	// 调试信息
	bool SendDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);

public:
	//初始化对话框
	virtual BOOL OnInitDialog();
	//消息预处理
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	//获得索引
	int GetItemIndex(CMD_S_CurDebugInfo *pCurDebugInfo, bool &bAddItem);
	//获得索引
	int GetItemIndex(DWORD dwDebugID);
	//获得等级
	CString GetLevelName(int nLevelScore);
	//获得等级
	int GetLevelIndex(CString strLevelName);
	//设置单项显示
	void UpdateItemText(int nUpdateItem, CMD_S_CurDebugInfo *pCurDebugInfo);

//映射函数
public:
	afx_msg void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);


public:
	//控件变量
	CSkinEdit				m_editTableID;
	CSkinEdit				m_editCardChance;
	CSkinButton				m_btQueryTableList;
	CSkinButton				m_btAddDebug;
	CSkinButton				m_btModifyDebug;
	CSkinButton				m_btDeleteDebug;
	CSkinButton				m_btCurTable;
	CSkinButton				m_btHistoryTable;
	CSkinListCtrl			m_listCtrlTable;
	CSkinListCtrl			m_listCtrlHistoryTable;
	CSkinComboBox			m_ComboCardLevel;
	CSkinComboBox			m_ComboStartHour;
	CSkinComboBox			m_ComboStartMinute;
	CSkinComboBox			m_ComboStartSceond;
	CSkinComboBox			m_ComboEndHour;
	CSkinComboBox			m_ComboEndMinute;
	CSkinComboBox			m_ComboEndSceond;

	afx_msg void OnBnClickedBtQueryTableList();
	afx_msg void OnBnClickedBtAddDebug();
	afx_msg void OnBnClickedBtModifyDebug();
	afx_msg void OnBnClickedBtDeleteDebug();
	afx_msg void OnNMClickListTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtCurTable();
	afx_msg void OnBnClickedBtHistoryTable();
};
