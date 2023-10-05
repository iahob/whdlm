#pragma once

class CServerConfigDlg;

// ������
class CDialogDexter : public CDialog
{
public:
    CServerConfigDlg			*m_pParentWnd;						// ��Ϣ����

    // ��������
public:
    bool						m_bShowScroll;						// ��ʾ����
    int							m_nShowMax;							// �����ʾ
    int							m_nScrollMax;						// ���λ��
    int							m_nScrollPos;						// ����λ��

    // �ຯ��
public:
    // ���캯��
    CDialogDexter(UINT nIDTemplate, int nShowMax, CServerConfigDlg *pParentWnd = NULL);
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
    virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize) { return false; };
    // ������Ϣ
    virtual bool SendDebugMessage(uint nMessageID, void *pData = NULL, uint nSize = 0);

	virtual void OnTableQuery();

	virtual void OnShowTrend();

    DECLARE_DYNAMIC(CDialogDexter)
    DECLARE_MESSAGE_MAP()
};