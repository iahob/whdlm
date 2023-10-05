#pragma once
#include "Resource.h"
#include "afxwin.h"

#define CUSTOM_SPECIAL			0
#define CUSTOM_GENERAL			1
#define CUSTOM_GIFT				2
#define CUSTOM_DELIVERY			3
#define MAX_CUSTOM				4

class CDialogDexter;
// ���ԶԻ���
class CDialogDebug : public CDialog, public IClientDebug
{
    // ��ԴID
public:
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
    CDialogDebug(CWnd *pParent = NULL);
    // ��������
    virtual ~CDialogDebug();

    // ���ں���
public:
    // �ؼ���
    virtual void DoDataExchange(CDataExchange *pDX);
    // ������Ϣ
    bool SendDebugMessage(uint nMessageID, void *pData = NULL, uint nSize = 0);

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

    DECLARE_DYNAMIC(CDialogDebug)
    DECLARE_MESSAGE_MAP()

    // �任ѡ��
    afx_msg void OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonDefault();
    afx_msg void OnBnClickedButtonRefreshRule();
    afx_msg void OnBnClickedButtonModifyRule();
};

// ������
class CDialogDexter : public CDialog
{
public:
    CDialogDebug				*m_pParentWnd;						// ��Ϣ����

    // ��������
public:
    bool						m_bShowScroll;						// ��ʾ����
    int							m_nShowMax;							// �����ʾ
    int							m_nScrollMax;						// ���λ��
    int							m_nScrollPos;						// ����λ��

    // �ຯ��
public:
    // ���캯��
    CDialogDexter(UINT nIDTemplate, int nShowMax, CDialogDebug *pParentWnd = NULL);
    // ��������
    ~CDialogDexter();

    // ʵ�ֺ���
public:
    //��ʼ������
    virtual BOOL OnInitDialog();
    // ������Ϣ
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    // ��껬��
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    // ���ڱ仯
    afx_msg void OnSize(UINT nType, int cx, int cy);
    // ��Ϣ����
    virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize) = NULL;
    // ������Ϣ
    virtual bool SendDebugMessage(uint nMessageID, void *pData = NULL, uint nSize = 0);

    DECLARE_MESSAGE_MAP()
};

// ���ԶԻ���
class CDialogSpecial : public CDialogDexter
{
    // ��ԴID
public:
    enum { IDD = IDD_CUSTOM_SPECIAL };

    // �����
public:

    // �����Ϣ
public:
    CEdit										m_EditTable;					// ����
    CComboBox									m_ComboGiftType;				// �޸�����

    // �ຯ��
public:
    // ���캯��
    CDialogSpecial(int nShowMax = 0, CDialogDebug *pParent = NULL);
    // ��������
    virtual ~CDialogSpecial();

    // ���ں���
protected:
    //��ʼ������
    virtual BOOL OnInitDialog();
    // �ؼ���
    virtual void DoDataExchange(CDataExchange *pDX);
    //��������
    int GetKindRewardIndex(CString strKindRewardName);

    // �̳к���
public:
    // ��Ϣ����
    virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

    DECLARE_DYNAMIC(CDialogSpecial)
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedBtnRefreshStock();
    afx_msg void OnBnClickedBtnModifyStock();
    afx_msg void OnBnClickedBtnGlod();
    afx_msg void OnBnClickedBtnGift();
    afx_msg void OnCbnSelchangeComboGiftType();
    afx_msg void OnBnClickedBtnQueryId();
    afx_msg void OnBnClickedBtnShard();
    afx_msg void OnBnClickedBtnGiftOpenClose();
};

// ���ԶԻ���
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
    CDialogGeneral(int nShowMax = 0, CDialogDebug *pParent = NULL);
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
    // ��Ϣ����
    virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

    DECLARE_DYNAMIC(CDialogGeneral)
    DECLARE_MESSAGE_MAP()
};


// ���ԶԻ���
class CDialogGift : public CDialogDexter
{
    // ��ԴID
public:
    enum { IDD = IDD_CUSTOM_GIFT };

    // �����
public:

    // �ຯ��
public:
    // ���캯��
    CDialogGift(int nShowMax = 0, CDialogDebug *pParent = NULL);
    // ��������
    virtual ~CDialogGift();

    // ���ں���
protected:
    //��ʼ������
    virtual BOOL OnInitDialog();
    // �ؼ���
    virtual void DoDataExchange(CDataExchange *pDX);

    // �̳к���
public:
    // ��Ϣ����
    virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

    DECLARE_DYNAMIC(CDialogGift)
    DECLARE_MESSAGE_MAP()

};

// ���ԶԻ���
class CDialogDelivery : public CDialogDexter
{
    // ��ԴID
public:
    enum { IDD = IDD_CUSTOM_GIFT };

    // �����
public:

    // �ຯ��
public:
    // ���캯��
    CDialogDelivery(int nShowMax = 0, CDialogDebug *pParent = NULL);
    // ��������
    virtual ~CDialogDelivery();

    // ���ں���
protected:
    //��ʼ������
    virtual BOOL OnInitDialog();
    // �ؼ���
    virtual void DoDataExchange(CDataExchange *pDX);

    // �̳к���
public:
    // ��Ϣ����
    virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

    DECLARE_DYNAMIC(CDialogDelivery)
    DECLARE_MESSAGE_MAP()

};

