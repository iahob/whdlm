#pragma once
#include "ClientControl.h"

typedef CWHArray<tagUserBet, tagUserBet&> CUserBetArray;
// CClientControlItemSinkDlg �Ի���

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientDebug
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	BYTE m_cbWinArea[2];				//Ӯ������
	BYTE m_cbExcuteTimes;				//ִ�д���
	//���¿��
	LONGLONG m_lStorage;
	LONGLONG m_lDeduct;
	CWnd 										*m_pParentWnd;					// ������
	IClientDebugCallback 					*m_pIClientDebugCallback;		// �ص��ӿ�
	CSkinEdit						m_edUserID;
	LONGLONG						m_lQueryGameID;					//��ѯID
	CListCtrl						m_listUserBet;					//�б�ؼ�
	CListCtrl						m_listUserBetAll;				//�б�ؼ�
	CUserBetArray					m_UserBetArray;					// �û���ע
	CSkinEdit						m_edStorageStart;				//��ʼ���

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClientControlItemSinkDlg();

// �Ի�������
	enum { IDD = IDD_CLIENT_CONTROL_EX };

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
	//������Ϣ
	bool SendDebugMessage(UINT nMessageID, void * pData = NULL, UINT nSize = 0);

public:
	//���¿���
	virtual void __cdecl OnAllowControl(bool bEnable);
	//������
	virtual bool __cdecl ReqResult(const void * pBuffer);

	virtual BOOL  OnInitDialog();
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void  OnSexClicked(UINT nCmdID);
	afx_msg void  OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


	void RequestUpdateStorage();
	bool UpdateStorage(const void * pBuffer);
	//ռʱ����Ҫ
	//void ChangeBetList(int idx, PlayerBetList list);
	//������ע
	virtual void UpdateUserBet(bool bReSet);
	virtual void ClearText();
	void ClearClist();

protected:
	virtual void OnCancel();

public:
//	afx_msg void OnTimer(UINT nIDEvent);
	
	afx_msg void OnBnClickedBtnUpdateStorage();
	afx_msg void OnBnClickedBtnUpdateStorage2();
	afx_msg void OnCbnSelchangeComboTimes();
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();
	afx_msg void OnEnSetfocusEditUserId();
	//������ɫ
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
