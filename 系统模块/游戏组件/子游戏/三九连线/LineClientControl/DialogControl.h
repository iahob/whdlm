#pragma once
#include "Resource.h"

// 控制对话框
class CDialogControl : public CDialog, public IClientControl
{
	// 资源ID
public:
	enum { IDD = IDD_DIALOG_MAIN };

	// 类变量
public:
	HICON										m_hIcon;						// 窗口图标
	CWnd *										m_pParentWnd;					// 父窗口
	IClientControlCallback *					m_pIClientControlCallback;		// 回调接口

	// 类函数
public:
	// 构造函数
	CDialogControl(CWnd* pParent = NULL); 
	// 析构函数
	virtual ~CDialogControl();

	// 窗口函数
protected:
	// 控件绑定
	virtual void DoDataExchange(CDataExchange* pDX); 

	// 继承函数
public:
	// 释放接口
	virtual void Release();
	// 创建函数
	virtual bool Create( CWnd * pParentWnd, IClientControlCallback * pIClientControlCallback );
	// 显示窗口
	virtual bool ShowWindow( bool bShow );
	// 消息函数
	virtual bool OnControlMessage( uint nMessageID, void * pData, uint nSize );

	// 按钮函数
public:
	// 库存查询
	afx_msg void OnBnClickedButtonStockInquiry();
	// 库存设置
	afx_msg void OnBnClickedButtonStockSet();
	// 难度设置
	afx_msg void OnBnClickedButtonDifficultySet();
	// 难度删除
	afx_msg void OnBnClickedButtonDifficultyDelete();
	// 难度查询
	afx_msg void OnBnClickedButtonDifficultyInquiry();
	// 难度清空
	afx_msg void OnBnClickedButtonDifficultyClear();
	// 赠送设置
	afx_msg void OnBnClickedButtonGiftSet();
	// 赠送删除
	afx_msg void OnBnClickedButtonGiftDelete();
	// 赠送查询
	afx_msg void OnBnClickedButtonGiftInquiry();
	// 赠送清空
	afx_msg void OnBnClickedButtonGiftClear();
	// 输赢更新
	afx_msg void OnBnClickedButtonWinLoseUpdate();
	// 日志更新
	afx_msg void OnBnClickedButtonLogUpdate();

	// 功能函数
public:
	// 赠送类型
	CString GiftType( uint nGiftType );
	// 格式字符串
	CString FormatString( tchar * pText, ... );

	DECLARE_DYNAMIC(CDialogControl)
	DECLARE_MESSAGE_MAP()
};
