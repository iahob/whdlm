#pragma once

class CClientDebugItemSinkDlg;

// ������
class CDialogDexter : public CDialog
{
public:
	CClientDebugItemSinkDlg		*m_pClientDebugItemSinkDlg;			// ��Ϣ����

    // ��������
public:
    bool						m_bShowScroll;						// ��ʾ����
    int							m_nShowMax;							// �����ʾ
    int							m_nScrollMax;						// ���λ��
    int							m_nScrollPos;						// ����λ��

    // �ຯ��
public:
    // ���캯��
	CDialogDexter(UINT nIDTemplate, CClientDebugItemSinkDlg *pParentWnd = NULL);
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

	//������ҵ���
	virtual void DeleteUserDebugInfo(DWORD dwDebugIndex) = NULL;
	//������ҵ���
	virtual void UpdateUserDebugInfo(tagHistoryRoomUserInfo *pHistoryRoomUserInfo) = NULL;
	//������ҵ���
	virtual void UpdateUserDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime) = NULL;

	//���·������
	virtual void DeleteRoomDebugInfo(DWORD dwDebugIndex) = NULL;
	//���·������
	virtual void UpdateRoomDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo) = NULL;
	//���·������
	virtual void UpdateRoomDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime) = NULL;

	//����ϵͳ����
	virtual void UpdateSysDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo) = NULL;
	//����ϵͳ����
	virtual void UpdateSysDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime) = NULL;

	virtual void OnTableQuery();

	virtual void SetTableCount(WORD	wTableCount);

	//�����н���
	WORD CalcWinChance(LONGLONG lSystemStorage, LONGLONG lUserStorage, LONGLONG nParameterK);
	//����״̬
	CString GetDebugStatus(int nDebugStatus);

    DECLARE_MESSAGE_MAP()
};