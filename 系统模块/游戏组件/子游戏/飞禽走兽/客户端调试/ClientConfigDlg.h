#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "DialogDexter.h"
#include "..\��Ϣ����\CMD_ZodiacBattle.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4800)

#define CUSTOM_GENERAL			0
#define CUSTOM_ADVANCED			1
#define MAX_CUSTOM				2
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
	tagCustomRule					m_CustomRule;					//���ýṹ
	SCORE							m_lCurSysStorage;				//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE							m_lCurPlayerStorage;			//��ǰ��ҿ��(���ҪӮ��Ǯ)
	SCORE							m_lCurParameterK;				//��ǰ����ϵ��(�ٷֱ�):
	BOOL							m_nSysCtrlStorageDeadLine;
	SCORE							m_lStorageResetCount;
	SCORE							m_lSysCtrlDeadLineLimit;
	SCORE							m_lSysCtrlSysWin;
	SCORE							m_lSysCtrlPlayerWin;

	//�������
protected:
	BYTE							m_cbWinArea;					//Ӯ������
	BYTE							m_cbExecuteTimes;				//ִ�д���

	//�����ע
protected:
	CMD_CF_UserInfo					m_UserInfo;						//�����Ϣ
	CListCtrl						m_listUserBet;					//�б�ؼ�
	LONGLONG						m_lQueryGameID;					//��ѯID

protected:
	CUserBetArray					m_UserBetArray;					//�û���ע

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

	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pBuffer, WORD wDataSize);

public:
	//�ж���Ч����
	bool IsValidNum(CString strEdit);

	//������ע
	void UpdateUserBet(bool bReSet);
	//ȡ���û���Ϣ
	int GetUserInfoIndex(WORD wChairID, WORD wTableID = 0);

public:
	//������
	virtual bool __cdecl ReqResult(const void *pBuffer);

	// �̳к���
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void  OnBnClickedBtnCancelAreaCtrl();
	afx_msg void  OnBnClickedBtnSetAreaCtrl();
	
	afx_msg void OnBnClickedButtonSetRule();
	afx_msg void OnLvnItemchangedListRoom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSetRoomCtrl();

	void UpdateSysCtrlDesc();
	afx_msg void OnEnChangeEditRoomctrlSysstorage();
	afx_msg void OnEnChangeEditRoomctrlPlayerstorage();
	afx_msg void OnEnChangeEditRoomctrlParameterK();
	void UpdateRoomCtrlDesc(SCORE lCurSysStorage, SCORE lCurPlayerStorage, SCORE lCurParameterK);

	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);

	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();
	afx_msg void OnEnSetfocusEditUserId();
};

class CDialogAdvanced : public CDialogDexter
{
	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_ADVANCED };

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

	virtual void CleanCtrlList();

	// �̳к���
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedBtnCancelRoomCtrl();

	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
};


#endif