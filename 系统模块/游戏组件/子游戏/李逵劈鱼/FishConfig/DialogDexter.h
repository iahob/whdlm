#pragma once

// ������
class CDialogDexter : public CDialog
{
	// ��������
public:
	int							m_nScrollMax;						// ���λ��
	int							m_nScrollPos;						// ����λ��

	// �ຯ��
public:
	// ���캯��
	CDialogDexter( UINT nIDTemplate, CWnd* pParentWnd = NULL );
	// ��������
	~CDialogDexter();

	// ʵ�ֺ���
public:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// ������Ϣ
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// ��껬��
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	// ���ڱ仯
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

	//�����н���
	WORD CalcWinChance(LONGLONG lSystemStorage, LONGLONG lUserStorage, int nParameterK);
};
