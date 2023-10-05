#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "DialogDexter.h"
#include "..\��Ϣ����\CMD_Ox.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4800)

#define CUSTOM_GENERAL			0
#define CUSTOM_ADVANCED			1
#define CUSTOM_TRACELOG			2
#define MAX_CUSTOM				3
#define MAX_EXECUTE_TIMES		15
#define MAX_EXECUTETIMES_CONFIG	5

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
	//��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pBuffer, WORD wDataSize);

public:
	//��ȡ�н���
	WORD GetWinRate(SCORE lSysStorage, SCORE lPlayerStorage, SCORE lParameterK);

	// �̳к���
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonAnchou();
	afx_msg void OnBnClickedButtonSysctrl();
	afx_msg void OnBnClickedButtonRoomctrl();
	afx_msg void OnBnClickedButtonCancelRoomctrl();
};

class CDialogAdvanced : public CDialogDexter
{
	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_ADVANCED };

	//�����ע
protected:
	CMD_CF_UserInfo					m_UserInfo;						//�����Ϣ
	ROOMUSERINFO					m_curQueryRoomUserInfo;			//��ǰ��ѯ�����Ϣ
	BANERGAMETYPE_CONFIG			m_bgtConfig;

	// �����
public:

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

public:
	//�û��Ƿ���Ч
	bool IsValidGameID(DWORD dwGameID);
	//��ȡUserID
	DWORD GetUserIDEx(DWORD dwGameID);

	//��̬����
protected:
	//���к���
	static INT CALLBACK SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

	// �̳к���
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonQueryPlayerGameID();
	afx_msg void OnBnClickedButtonTablectrl();
	afx_msg void OnBnClickedButtonCancelTablectrl();
	afx_msg void OnLvnColumnclickListUserBet(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPlayerGameid();
};

class CDialogTraceLog : public CDialogDexter
{
	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_TRACELOG };

	// �����
public:

	// �ຯ��
public:
	// ���캯��
	CDialogTraceLog(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
	// ��������
	virtual ~CDialogTraceLog();

	// ���ں���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

	virtual void CleanCtrlList();
};

#endif