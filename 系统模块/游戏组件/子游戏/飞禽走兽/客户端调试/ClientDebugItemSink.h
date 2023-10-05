#pragma once
#include "Stdafx.h"
#include "ClientControl.h"


// CClientControlItemSinkDlg �Ի���
#ifndef _UNICODE
#define _snprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define _snprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#define myatoi      _wtoi
#define myatoi64	_wtoi64
#endif


class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientControl
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)
protected:
	CWnd 							*m_pParentWnd;					// ������
	IClientControlCallback 			*m_pIClientControlCallback;		// �ص��ӿ�

protected:
	BYTE m_cbControlArea;
	BYTE m_cbControlStyle;
	BYTE m_cbExcuteTimes;

	LONGLONG						m_lStorageStart;				//��ʼ���
	LONGLONG						m_lStorageDeduct;				//���˥��
	LONGLONG						m_lStorageCurrent;				//��ǰ���
	LONGLONG						m_lCurrentBonus;				//��ǰ�ʽ��
	LONGLONG						m_lStorageMax1;					//�������1
	LONGLONG						m_lStorageMul1;					//ϵͳ��ָ���1
	LONGLONG						m_lStorageMax2;					//�������2
	LONGLONG						m_lStorageMul2;					//ϵͳ��ָ���2

	CMD_CF_UserInfo					m_UserInfo;						//�����Ϣ
	CListCtrl						m_listUserBet;					//�б�ؼ�
	CListCtrl						m_listUserBetAll;				//�б�ؼ�
	LONGLONG						m_lQueryGameID;					//��ѯID
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
	virtual bool Create(CWnd *pParentWnd, IClientControlCallback *pIClientControlCallback);
	// ��ʾ����
	virtual bool ShowWindow(bool bShow);
	// ��Ϣ����
	virtual bool OnControlMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);
	// ������Ϣ
	bool SendControlMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
	//ȡ���û���Ϣ
	int GetUserInfoIndex(WORD wChairID, WORD wTableID = 0);

public:
	//���¿���
	virtual void  UpdateControl(CMD_S_ControlReturns* pControlReturns);

protected:
	//��Ϣ����
	void PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes);
	//���ý���
	void ReSetAdminWnd();
	//���¿ؼ�
	virtual void __cdecl UpdateControl();

public:
	//��ʼ��
	virtual BOOL OnInitDialog();
	//������ɫ
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//ȡ������
	afx_msg void OnBnClickedButtonReset();
	//��������
	afx_msg void OnBnClickedButtonOk();
	//ȡ���ر�
	afx_msg void OnBnClickedButtonCancel();	
	afx_msg void OnBnClickedBtnUpdateStorage();
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();
	afx_msg void OnEnSetfocusEditUserId();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	virtual void  SetStorageCurrentValue(LONGLONG lCurrentStorage, LONGLONG lCurrentBonus);
	//������ע
	virtual void __cdecl UpdateUserBet(bool bReSet);
	//���¿��
	virtual bool __cdecl UpdateStorage(const void * pBuffer);
   	virtual     void ClearText();

	bool CheckDataMinMax(LONGLONG valueMax);
	void RequestUpdateStorage();
	void ClearClist();
	
};


