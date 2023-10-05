#pragma once
#include "Stdafx.h"
#include "ClientDebug.h"

class AFX_EXT_CLASS CClientDebugItemSinkDlg : public IClientDebugDlg, public IClientDebug
{
	DECLARE_DYNAMIC(CClientDebugItemSinkDlg)
protected:
	CWnd 							*m_pParentWnd;					// ������
	IClientDebugCallback 			*m_pIClientDebugCallback;		// �ص��ӿ�

public:
	CClientDebugItemSinkDlg(CWnd *pParent = NULL); 
	virtual ~CClientDebugItemSinkDlg();

	enum { IDD = IDD_CLIENT_DEBUG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	// �̳к���
public:
	// �ͷŽӿ�
	virtual void Release();
	// ��������
	virtual bool Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback);
	// ��ʾ����
	virtual bool ShowWindow(bool bShow);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);
	// ������Ϣ
	bool SendDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);

public:
	//��ʼ���Ի���
	virtual BOOL OnInitDialog();
	//��ϢԤ����
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	//�������
	int GetItemIndex(CMD_S_CurDebugInfo *pCurDebugInfo, bool &bAddItem);
	//�������
	int GetItemIndex(DWORD dwDebugID);
	//��õȼ�
	CString GetLevelName(int nLevelScore);
	//��õȼ�
	int GetLevelIndex(CString strLevelName);
	//���õ�����ʾ
	void UpdateItemText(int nUpdateItem, CMD_S_CurDebugInfo *pCurDebugInfo);

//ӳ�亯��
public:
	afx_msg void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);


public:
	//�ؼ�����
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
