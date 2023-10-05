#pragma once


class CEditKey : public CEdit
{
	// 变量信息	
public:
	CFont							m_Font;					// 字符
	CBrush							m_Brush;				// 画刷
	CString							m_strKey;				// 键值字符
	UINT							m_nKey;					// 键值

	static CWHArray<CEditKey*>		m_ArrayKey;				// 所有键值

	// 构造函数
public:
	CEditKey(void);
	~CEditKey(void);
	
	// 功能函数
public:
	// 获取关键值
	UINT GetKey() { return m_nKey; }
	// 设置关键值
	void SetKey( UINT nKey );

	// 功能函数
protected:
	// 转换关键值
	void KeyToStr();

	// 消息函数
public:
	// 创建函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// 字符消息
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	// 键盘点击
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	// 背景颜色
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	// 屏蔽右键
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
