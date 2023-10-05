#pragma once

// 窗口类
class CDialogDexter : public CDialog
{
	// 滑动变量
public:
	int							m_nScrollMax;						// 最大位置
	int							m_nScrollPos;						// 滑动位置

	// 类函数
public:
	// 构造函数
	CDialogDexter( UINT nIDTemplate, CWnd* pParentWnd = NULL );
	// 析构函数
	~CDialogDexter();

	// 实现函数
public:
	//初始化窗口
	virtual BOOL OnInitDialog();
	// 滑动消息
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// 鼠标滑轮
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	// 窗口变化
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

	//计算中奖率
	WORD CalcWinChance(LONGLONG lSystemStorage, LONGLONG lUserStorage, int nParameterK);
};
