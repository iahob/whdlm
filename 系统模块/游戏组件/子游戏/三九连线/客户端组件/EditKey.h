#pragma once


class CEditKey : public CEdit
{
	// ������Ϣ	
public:
	CFont							m_Font;					// �ַ�
	CBrush							m_Brush;				// ��ˢ
	CString							m_strKey;				// ��ֵ�ַ�
	UINT							m_nKey;					// ��ֵ

	static CWHArray<CEditKey*>		m_ArrayKey;				// ���м�ֵ

	// ���캯��
public:
	CEditKey(void);
	~CEditKey(void);
	
	// ���ܺ���
public:
	// ��ȡ�ؼ�ֵ
	UINT GetKey() { return m_nKey; }
	// ���ùؼ�ֵ
	void SetKey( UINT nKey );

	// ���ܺ���
protected:
	// ת���ؼ�ֵ
	void KeyToStr();

	// ��Ϣ����
public:
	// ��������
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// �ַ���Ϣ
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	// ���̵��
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	// ������ɫ
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	// �����Ҽ�
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
