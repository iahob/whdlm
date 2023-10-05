#pragma once
#include "../��Ϸ�ͻ���/ClientControl.h"


// CClientControlItemSinkDlg �Ի���

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	bool m_bWinArea[CONTROL_AREA];
	BYTE m_cbControlStyle;
	BYTE m_cbExcuteTimes;

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClientControlItemSinkDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ADMIN };
LONGLONG  m_lAllUserBet[AREA_COUNT];				//�û���ע
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
		//������ע
	virtual void  ResetUserBet();
	//�����ע
	virtual void  SetUserBetScore(BYTE cbArea,LONGLONG lScore);
	//���¿���
	virtual void  OnAllowControl(bool bEnable);
	//������
	virtual bool  ReqResult(const void * pBuffer);

public:
	//���ý���
	void ReSetAdminWnd();

public:
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void  OnRadioClick();
protected:
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
};
