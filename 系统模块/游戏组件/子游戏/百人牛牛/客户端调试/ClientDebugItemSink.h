#pragma once
#include "Stdafx.h"

// CClientDebugItemSinkDlg �Ի���

class AFX_EXT_CLASS CClientDebugItemSinkDlg : public CDialog, public IClientDebug
{
    DECLARE_DYNAMIC(CClientDebugItemSinkDlg)

protected:
	CUserBetArray					m_UserBetArray;					//�û���ע
    CWnd 							*m_pParentWnd;					// ������
    IClientDebugCallback 			*m_pIClientDebugCallback;		// �ص��ӿ�

    //������
protected:
	tagCustomRule					m_CustomRule;						//���ýṹ
	SCORE							m_lCurSysStorage;					//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE							m_lCurPlayerStorage;				//��ǰ��ҿ��(���ҪӮ��Ǯ)
	SCORE							m_lCurParameterK;					//��ǰ����ϵ��(�ٷֱ�):

    //�������
protected:
    bool m_bWinArea[DEBUG_AREA];
    BYTE m_cbDebugStyle;
    BYTE m_cbExcuteTimes;

    //�����ע
protected:
    CMD_CF_UserInfo					m_UserInfo;						//�����Ϣ
    CListCtrl						m_listUserBet;					//�б�ؼ�
    CListCtrl						m_listUserBetAll;				//�б�ؼ�
    LONGLONG						m_lQueryGameID;					//��ѯID

public:
    CClientDebugItemSinkDlg(CWnd *pParent = NULL);   // ��׼���캯��
    virtual ~CClientDebugItemSinkDlg();

    // �Ի�������
    enum { IDD = IDD_DIALOG_ADMIN };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV ֧��

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
    //ȡ���û���Ϣ
    int GetUserInfoIndex(WORD wChairID, WORD wTableID = 0);
	//�ж���Ч����
	bool IsValidNum(CString strEdit);

public:
	virtual BOOL OnInitDialog();
    //������
    virtual bool __cdecl ReqResult(const void *pBuffer);
    //������ע
    virtual void __cdecl UpdateUserBet(bool bReSet);

public:
    //���ý���
    void ReSetAdminWnd();

public:
    //��ʾ״̬
    void PrintCurStatus();

public:
    afx_msg void  OnReSet();
    afx_msg void  OnRefresh();
    afx_msg void  OnExcute();
    afx_msg void  OnRadioClick();

public:
	afx_msg void OnBnClickedButtonRefreshRule();
	afx_msg void OnBnClickedButtonSetRule();
	afx_msg void OnBnClickedBtnRefreshRoomlist();
	afx_msg void OnLvnItemchangedListRoom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSetRoomCtrl();
	afx_msg void OnBnClickedBtnCancelRoomCtrl();

    afx_msg void OnBnClickedBtnUserBetQuery();
    afx_msg void OnBnClickedBtnUserBetAll();
    afx_msg void OnEnSetfocusEditUserId();

	afx_msg void OnEnChangeEditCurSysstorage();
	afx_msg void OnEnChangeEditCurPlayerstorage();
	afx_msg void OnEnChangeEditCurParameterK();
	void UpdateSysCtrlDesc();
	afx_msg void OnEnChangeEditRoomctrlSysstorage();
	afx_msg void OnEnChangeEditRoomctrlPlayerstorage();
	afx_msg void OnEnChangeEditRoomctrlParameterK();
	void UpdateRoomCtrlDesc();

    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
};
