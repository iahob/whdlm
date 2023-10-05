#pragma once
#include "ClientControl.h"

typedef CWHArray<tagUserBet, tagUserBet&> CUserBetArray;
// CClientControlItemSinkDlg 对话框

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientDebug
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	BYTE m_cbWinArea[2];				//赢牌区域
	BYTE m_cbExcuteTimes;				//执行次数
	//更新库存
	LONGLONG m_lStorage;
	LONGLONG m_lDeduct;
	CWnd 										*m_pParentWnd;					// 父窗口
	IClientDebugCallback 					*m_pIClientDebugCallback;		// 回调接口
	CSkinEdit						m_edUserID;
	LONGLONG						m_lQueryGameID;					//查询ID
	CListCtrl						m_listUserBet;					//列表控件
	CListCtrl						m_listUserBetAll;				//列表控件
	CUserBetArray					m_UserBetArray;					// 用户下注
	CSkinEdit						m_edStorageStart;				//起始库存

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClientControlItemSinkDlg();

// 对话框数据
	enum { IDD = IDD_CLIENT_CONTROL_EX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	// 继承函数
public:
	// 释放接口
	virtual void Release();
	// 创建函数
	virtual bool Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback);
	// 显示窗口
	virtual bool ShowWindow(bool bShow);
	// 消息函数
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);
	//控制信息
	bool SendDebugMessage(UINT nMessageID, void * pData = NULL, UINT nSize = 0);

public:
	//更新控制
	virtual void __cdecl OnAllowControl(bool bEnable);
	//申请结果
	virtual bool __cdecl ReqResult(const void * pBuffer);

	virtual BOOL  OnInitDialog();
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void  OnSexClicked(UINT nCmdID);
	afx_msg void  OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


	void RequestUpdateStorage();
	bool UpdateStorage(const void * pBuffer);
	//占时不需要
	//void ChangeBetList(int idx, PlayerBetList list);
	//更新下注
	virtual void UpdateUserBet(bool bReSet);
	virtual void ClearText();
	void ClearClist();

protected:
	virtual void OnCancel();

public:
//	afx_msg void OnTimer(UINT nIDEvent);
	
	afx_msg void OnBnClickedBtnUpdateStorage();
	afx_msg void OnBnClickedBtnUpdateStorage2();
	afx_msg void OnCbnSelchangeComboTimes();
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();
	afx_msg void OnEnSetfocusEditUserId();
	//设置颜色
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
