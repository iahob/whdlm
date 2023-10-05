#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "DialogDexter.h"
#include "..\��Ϣ����\CMD_Game.h"
#include <cstdint>
#include <vector>

#pragma warning(disable : 4244)
#pragma warning(disable : 4800)

enum
{
	CUSTOM_CONFIG=0,
	CUSTOM_DEBUG,
	CUSTOM_LOG,
	//CUSTOM_WEIGHT,
	MAX_CUSTOM,
};

// ���ԶԻ���
class CClientConfigDlg : public CDialog, public IClientDebug
{
    // �Ի�������
    enum { IDD = IDD_DIALOG_MAIN };

    // �����
public:
    CWnd 									*m_pParentWnd;					// ������
    IClientDebugCallback 					*m_pIClientDebugCallback;		// �ص��ӿ�
public:
	GameConfig								m_gameConfig;

    // �����Ϣ
public:
    CDialogDexter 							*m_DialogCustom[MAX_CUSTOM];
	CDialogDexter							*m_pDialogWeight;

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

public:
	//
	void OnWieightDestory(){ OnShowWeight(); }

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
	//
	afx_msg void OnShowWeight();
};


//////////////////////////////////////////////////////////////////////////////////
// ���ԶԻ���
class CDialogConfig : public CDialogDexter
{
	CClientConfigDlg*	m_pParentDlg;

	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_CONFIG };

	// �ຯ��
public:
	// ���캯��
	CDialogConfig(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
	// ��������
	virtual ~CDialogConfig();

	//
public:
	//
	void InitUI();
	//
	bool OnDebugEvent(void *pData, WORD nSize);

	//����ˢ��
protected:
	bool m_bFirstInit[MaxCtrl];
	//
	void RefreshCurSystemStorage(StorageInfo* pInfo);
	void RefreshCurGameReward(GameRewardInfo* pInfo);
	void RefreshCurGameTax(GameTaxInfo* pInfo);
	void RefreshCurGameExtra(GameExtraInfo* pInfo);
	void RefreshCurGameStatistics(StatisticsInfo* pInfo);
	//
	afx_msg void OnSetSystemStorage();
	afx_msg void OnSetGameReward();
	afx_msg void OnSetGameTax();
	afx_msg void OnSetGameExtra();

	// ���ں���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);

	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

	// �̳к���
public:
	DECLARE_DYNAMIC(CDialogConfig)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
};

class CDialogDebug : public CDialogDexter
{
	CClientConfigDlg*	m_pParentDlg;

	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_DEBUG };

	// �����
public:

	// �ຯ��
public:
	// ���캯��
	CDialogDebug(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
	// ��������
	virtual ~CDialogDebug();

	//
public:
	//
	void InitUI();
	//
	bool OnDebugEvent(void *pData, WORD nSize);

	//����ˢ��
protected:
	bool m_bFirstInit[MaxCtrl];
	//
	void RefreshCurRoomStorage(StorageInfo* pInfo);
	void RefreshRoomTaskList(StorageInfo* pInfo);
	void RefreshCurPlayerStorage(GameDebugInfo* pInfo);
	void RefreshPlayerTaskList(GameDebugInfo* pInfo);
	void RefreshCurGameStatistics(StatisticsInfo* pInfo);
	//
	afx_msg void OnSetRoomStorage();
	afx_msg void OnCancelRoomTask();
	afx_msg void OnSetPlayerStorage();
	afx_msg void OnCancelPlayerTask();

	// ���ں���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

	// �̳к���
public:
	DECLARE_DYNAMIC(CDialogDebug)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};


class CDialogLog : public CDialogDexter
{

	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_LOG };

	// �����
public:

	// �ຯ��
public:
	// ���캯��
	CDialogLog(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
	// ��������
	virtual ~CDialogLog();

	//
public:
	//
	void InitUI();
	//
	bool OnDebugEvent(void *pData, WORD nSize);

	//����ˢ��
public:
	void RefreshSystemTaskList(StorageInfo* pInfo);
	void RefreshRoomTaskList(StorageInfo* pInfo);
	void RefreshPlayerTaskList(GameDebugInfo* pInfo);
	//
	afx_msg void OnCancelRoomTask();
	afx_msg void OnCancelRoomTaskAll();
	afx_msg void OnCancelPlayerTask();
	afx_msg void OnCancelPlayerTaskAll();

	// ���ں���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

	// �̳к���
public:
	DECLARE_DYNAMIC(CDialogLog)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

class CDialogWeight : public CDialogDexter
{
	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_WEIGHT };

	// �����
public:

	// �ຯ��
public:
	// ���캯��
	CDialogWeight(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
	// ��������
	virtual ~CDialogWeight();

	//
public:
	bool				m_bInit;
	//
	void RefreshConfig(void *pData, WORD nSize);
	//
	bool OnDebugEvent(void *pData, WORD nSize);

	//
public:
	//
	void InitUI();

	//����ˢ��
public:
	//
	void RefreshWeightConfig(WeightConfig* pInfo,int nSize);
	afx_msg void OnWieightConfigSet();

	// ���ں���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

public:
	// ��������
	bool Create(CWnd *pParentWnd);
	// ��ʾ����
	bool ShowWindow(bool bShow);

	// �̳к���
public:
	DECLARE_DYNAMIC(CDialogWeight)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	afx_msg void OnBtnConfirm();
};

#endif