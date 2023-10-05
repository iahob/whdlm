#pragma once

#include <cstdint>

class CClientConfigDlg;

// ������
class CDialogDexter : public CDialog
{
public:
    CClientConfigDlg			*m_pParentWnd;						// ��Ϣ����

    // ��������
public:
    bool						m_bShowScroll;						// ��ʾ����
    int							m_nShowMax;							// �����ʾ
    int							m_nScrollMax;						// ���λ��
    int							m_nScrollPos;						// ����λ��

    // �ຯ��
public:
    // ���캯��
    CDialogDexter(UINT nIDTemplate, int nShowMax, CClientConfigDlg *pParentWnd = NULL);
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
    virtual bool SendDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
	// ������Ϣ
	virtual bool SendDebugMessage(UINT nMessageID, WORD wTableID, void *pData = NULL, UINT nSize = 0);

	virtual void OnTableQuery();

	virtual void SetTableCount(WORD	wTableCount);

    DECLARE_MESSAGE_MAP()

	//�����ຯ��
public:
	virtual LONGLONG GetDlgItemLongLong(int nItemID,LONGLONG lDefault = -1);
	//
	virtual void SetDlgItemValue(int nID, double nValue);
	virtual void SetDlgItemValue(int nID, UINT nValue);
	virtual void SetDlgItemValue(int nID, LONGLONG nValue);
	//
	virtual void SetListItemValue(CListCtrl* pList,int nColumn, int nSetIndex, double nValue);
	virtual void SetListItemValue(CListCtrl* pList, int nColumn, int nSetIndex, UINT nValue);
	virtual void SetListItemValue(CListCtrl* pList, int nColumn, int nSetIndex, LONGLONG nValue);
	virtual void SetListItemValue(CListCtrl* pList, int nColumn, int nSetIndex, CString strValue);
	//
	int	GetListTargetColumn(CListCtrl *pList, LONGLONG lCompareValue, int nValueIndex = 0);
	//���ÿؼ������С
	virtual void SetDlgItemFontSize(int nID,int nHeight,int nWidth);
	

public:
	char		 m_eventSendBuffer[MAX_EVENT_SEND_BUFFER];//�¼����ͻ�����
	virtual bool SendEvent(ControlEventType type,int64_t nDataLen,void* pData);
};