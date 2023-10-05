// FishConfigDlg.h : ͷ�ļ�
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

// ���Ի���
class CFishConfigDlg : public CDialog
{

	// ��������
public:
	CString						m_StrFilePath;		
	CDialogDexter *				m_DialogCustom[MAX_CUSTOM];

	// �ຯ��
public:
	// ���캯��
	CFishConfigDlg(CWnd* pParent = NULL);	

	// �Ի�������
	enum { IDD = IDD_FISHCONFIG_DIALOG };

	// ʵ��
protected:
	// �ؼ���
	virtual void DoDataExchange(CDataExchange* pDX);
	// ��ʼ��
	virtual BOOL OnInitDialog();
	// �任ѡ��
	afx_msg void OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult);
	// ��������
	afx_msg void OnDestroy();
	// Ĭ�ϰ�ť
	afx_msg void OnBnClickedButton3();
	// �򿪰�ť
	afx_msg void OnBnClickedButton2();
	// ���水ť
	afx_msg void OnBnClickedButton1();

	DECLARE_MESSAGE_MAP()
};
