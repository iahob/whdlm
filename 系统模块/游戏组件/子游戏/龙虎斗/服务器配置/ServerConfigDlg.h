#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "DialogDexter.h"

#pragma warning(disable : 4244)
#define CUSTOM_GENERAL			0
#define CUSTOM_ADVANCED			1
#define MAX_CUSTOM				2

// ���ô���
class CServerConfigDlg : public CDialog
{
    // �Ի�������
    enum { IDD = IDD_DIALOG_MAIN };

    //�ؼ�����
protected:
    CDialogDexter 					*m_DialogCustom[MAX_CUSTOM];

    // ��������
public:
    // ���캯��
    CServerConfigDlg();
    // ��������
    virtual ~CServerConfigDlg();
    // ��������
    void OnDestroy();

    // ���غ���
protected:
    // �ؼ���
    virtual VOID DoDataExchange(CDataExchange *pDX);
    // ��ʼ������
    virtual BOOL OnInitDialog();

	// �̳к���
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonRead();
    afx_msg void OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult);

	//��������
public:
	//��ȡ�շű�����
	EM_TABLE_TYPE GetTableType(LONGLONG lTableWinScore);
};

//////////////////////////////////////////////////////////////////////////////////
// ���ƶԻ���
class CDialogGeneral : public CDialogDexter
{
    // ��ԴID
public:
    enum { IDD = IDD_CUSTOM_GENERAL };

    // �����
public:

    // �ຯ��
public:
    // ���캯��
    CDialogGeneral(int nShowMax = 0, CServerConfigDlg *pParent = NULL);
    // ��������
    virtual ~CDialogGeneral();

    // ���ں���
protected:
    //��ʼ������
    virtual BOOL OnInitDialog();
    // �ؼ���
    virtual void DoDataExchange(CDataExchange *pDX);

    // �̳к���
public:
    DECLARE_DYNAMIC(CDialogGeneral)
    DECLARE_MESSAGE_MAP()
};


// ���ƶԻ���
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
    CDialogAdvanced(int nShowMax = 0, CServerConfigDlg *pParent = NULL);
    // ��������
    virtual ~CDialogAdvanced();

    // ���ں���
protected:
    //��ʼ������
    virtual BOOL OnInitDialog();
    // �ؼ���
    virtual void DoDataExchange(CDataExchange *pDX);

    // �̳к���
public:
    DECLARE_DYNAMIC(CDialogAdvanced)
    DECLARE_MESSAGE_MAP()
	
	afx_msg void OnBnClickedButtonTableBefore();
	afx_msg void OnBnClickedButtonTableNext();
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnLvnItemchangedListTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditDebugValue();
	afx_msg void OnEnChangeEditDifficulty();
	afx_msg void OnBnClickedButtonSetgraderange();
	afx_msg void OnBnClickedButtonSetstoragerange();

	//��������
public:
	//���ѯ
	virtual void OnTableQuery();
	//��ȡ�շű�����
	EM_TABLE_TYPE GetTableType(LONGLONG lTableWinScore);
	//��ȡ�ؼ�����
	SCORE GetCtrlIDInter(WORD wCtrlID);
};

#endif