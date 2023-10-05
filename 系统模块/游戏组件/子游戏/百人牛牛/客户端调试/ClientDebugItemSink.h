#pragma once
#include "Stdafx.h"

// CClientDebugItemSinkDlg 对话框

class AFX_EXT_CLASS CClientDebugItemSinkDlg : public CDialog, public IClientDebug
{
    DECLARE_DYNAMIC(CClientDebugItemSinkDlg)

protected:
	CUserBetArray					m_UserBetArray;					//用户下注
    CWnd 							*m_pParentWnd;					// 父窗口
    IClientDebugCallback 			*m_pIClientDebugCallback;		// 回调接口

    //库存调试
protected:
	tagCustomRule					m_CustomRule;						//配置结构
	SCORE							m_lCurSysStorage;					//当前系统库存(系统要赢的钱)
	SCORE							m_lCurPlayerStorage;				//当前玩家库存(玩家要赢的钱)
	SCORE							m_lCurParameterK;					//当前调节系数(百分比):

    //区域调试
protected:
    bool m_bWinArea[DEBUG_AREA];
    BYTE m_cbDebugStyle;
    BYTE m_cbExcuteTimes;

    //玩家下注
protected:
    CMD_CF_UserInfo					m_UserInfo;						//玩家信息
    CListCtrl						m_listUserBet;					//列表控件
    CListCtrl						m_listUserBetAll;				//列表控件
    LONGLONG						m_lQueryGameID;					//查询ID

public:
    CClientDebugItemSinkDlg(CWnd *pParent = NULL);   // 标准构造函数
    virtual ~CClientDebugItemSinkDlg();

    // 对话框数据
    enum { IDD = IDD_DIALOG_ADMIN };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV 支持

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
    // 调试信息
    bool SendDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
    //取得用户信息
    int GetUserInfoIndex(WORD wChairID, WORD wTableID = 0);
	//判断有效数字
	bool IsValidNum(CString strEdit);

public:
	virtual BOOL OnInitDialog();
    //申请结果
    virtual bool __cdecl ReqResult(const void *pBuffer);
    //更新下注
    virtual void __cdecl UpdateUserBet(bool bReSet);

public:
    //重置界面
    void ReSetAdminWnd();

public:
    //显示状态
    void PrintCurStatus();

public:
    afx_msg void  OnReSet();
    afx_msg void  OnRefresh();
    afx_msg void  OnExcute();
    afx_msg void  OnRadioClick();

public:
	afx_msg void OnBnClickedButtonRefreshRule();
	afx_msg void OnBnClickedButtonSetRule();
	afx_msg void OnBnClickedBtnRefreshRoomlist();
	afx_msg void OnLvnItemchangedListRoom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSetRoomCtrl();
	afx_msg void OnBnClickedBtnCancelRoomCtrl();

    afx_msg void OnBnClickedBtnUserBetQuery();
    afx_msg void OnBnClickedBtnUserBetAll();
    afx_msg void OnEnSetfocusEditUserId();

	afx_msg void OnEnChangeEditCurSysstorage();
	afx_msg void OnEnChangeEditCurPlayerstorage();
	afx_msg void OnEnChangeEditCurParameterK();
	void UpdateSysCtrlDesc();
	afx_msg void OnEnChangeEditRoomctrlSysstorage();
	afx_msg void OnEnChangeEditRoomctrlPlayerstorage();
	afx_msg void OnEnChangeEditRoomctrlParameterK();
	void UpdateRoomCtrlDesc();

    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
};
