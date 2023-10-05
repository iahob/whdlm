#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "DialogDexter.h"
#include "..\��Ϣ����\CMD_Game.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4800)

#define CUSTOM_GENERAL			0
#define CUSTOM_CONTROL			1
#define CUSTOM_ADVANCED			2

#define MAX_CUSTOM				3
#define MAX_EXECUTE_TIMES		15

// ���ԶԻ���
class CClientConfigDlg : public CDialog, public IClientDebug
{
    // �Ի�������
    enum { IDD = IDD_DIALOG_MAIN };

    // �����
public:
    CWnd 									*m_pParentWnd;					// ������
    IClientDebugCallback 					*m_pIClientDebugCallback;		// �ص��ӿ�

    // �����Ϣ
public:
    CDialogDexter 							*m_DialogCustom[MAX_CUSTOM];
    CDialogDexter                           *m_DialogSetting;

    // �ຯ��
public:
    // ���캯��
    CClientConfigDlg(CWnd *pParent = NULL);
    // ��������
    virtual ~CClientConfigDlg();
    // ��������
    void OnDestroy();

    // ���ں���
public:
    // �ؼ���
    virtual void DoDataExchange(CDataExchange *pDX);
    // ������Ϣ
    bool SendDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
	// ������Ϣ
	bool SendDebugMessage(UINT nMessageID, WORD wTableID, void *pData = NULL, UINT nSize = 0);

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

    DECLARE_DYNAMIC(CClientConfigDlg)
    DECLARE_MESSAGE_MAP()
   
    // �任ѡ��
    afx_msg void OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult);
};

//////////////////////////////////////////////////////////////////////////////////
// ���ԶԻ���
class CDialogGeneral : public CDialogDexter
{
	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_GENERAL };

	//���ñ���
protected:

    CFont                           m_FontFrame;
    CFont                           m_Font;
  
	// �ຯ��
public:
	// ���캯��
	CDialogGeneral(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
	// ��������
	virtual ~CDialogGeneral();

	// ���ں���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);
    void DDX_DOUBLE(CDataExchange* pDX, int nIDC, DOUBLE &value);
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pBuffer, WORD wDataSize);

public:
	//�ж���Ч����
	bool IsValidNum(CString strEdit);


	// �̳к���
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonSetRule();
	afx_msg void OnLvnItemchangedListRoom(NMHDR *pNMHDR, LRESULT *pResult);
	

	void UpdateSysCtrlDesc();
	afx_msg void OnEnChangeEditRoomctrlSysstorage();
	afx_msg void OnEnChangeEditRoomctrlPlayerstorage();
	afx_msg void OnEnChangeEditRoomctrlParameterK();
	void UpdateRoomCtrlDesc(SCORE lCurSysStorage, SCORE lCurPlayerStorage, int nCurParameterK);
    void UpdateUserCtrlDesc(SCORE lCurSysStorage, SCORE lCurPlayerStorage, int nCurParameterK);
    
   
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);

    afx_msg void OnBnClickedBtnUserBetQuery();
    afx_msg void OnBnClickedBtContinueWin();
    afx_msg void OnBnClickedBtContinueLost();
    afx_msg void OnBnClickedBtContinueCancel();
   
    afx_msg void OnBnClickedButtonChou();
    afx_msg void OnBnClickedBtnSetRoomctrl();
    afx_msg void OnBnClickedButtonCancellRoom();
};


// ���ԶԻ���
class CDialogControl : public CDialogDexter
{
    // ��ԴID
public:
    enum { IDD = IDD_CUSTOM_CONTROL };

    //����
public:
    CFont      m_FontTitle;
    WORD       m_tableID;
    // �ຯ��
public:
    // ���캯��
    CDialogControl(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
    // ��������
    virtual ~CDialogControl();

    // ���ں���
protected:
    //��ʼ������
    virtual BOOL OnInitDialog();
    // �ؼ���
    virtual void DoDataExchange(CDataExchange *pDX);

    virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pBuffer, WORD wDataSize);
    //�ж���Ч����
    bool IsValidNum(CString strEdit);

    void UpdateRoomCtrlDesc(SCORE lCurSysStorage, SCORE lCurPlayerStorage, int nCurParameterK);
    void UpdateUserCtrlDesc(SCORE lCurSysStorage, SCORE lCurPlayerStorage, int nCurParameterK);
    // �̳к���
public:
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedBtnCancelUser();
   
    afx_msg void OnBnClickedBtnSetUserctrl();
    afx_msg void OnBnClickedBtnDebugExc();
    afx_msg void OnBnClickedBtnDebugCancell();
    afx_msg void OnBnClickedBtnQuary();
   
};

class CDialogAdvanced : public CDialogDexter
{
	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_ADVANCED };

	// �����
public:
    CFont      m_FontTitle;
	// �ຯ��
public:
	// ���캯��
	CDialogAdvanced(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
	// ��������
	virtual ~CDialogAdvanced();

	// ���ں���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);
    void UpdateList(int nItem, CListCtrl &pListCtrlUser, CMD_S_UserCtrlResult &pUserCtrlResult);
	
	// �̳к���
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedBtnCancelRoomCtrl();

    afx_msg void OnBnClickedBtnCancelUserctrl();
    afx_msg void OnBnClickedBtnCancelRoomctrl2();
    afx_msg void OnBnClickedBtnCancelUserctrl2();
    afx_msg void OnBnClickedBtnClearDesk();
    afx_msg void OnBnClickedBtnClearDeskall();
};

#endif