#pragma once
#include "Stdafx.h"
#include "ClientDebug.h"


// CClientControlItemSinkDlg 对话框

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientDebug
{
    DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
    CWnd 							*m_pParentWnd;					// 父窗口
    IClientDebugCallback 			*m_pIClientDebugCallback;		// 回调接口

    //库存控制
protected:
    LONGLONG						m_lStorageStart;				//起始库存
    LONGLONG						m_lStorageDeduct;				//库存衰减
    LONGLONG						m_lStorageCurrent;				//当前库存
    LONGLONG						m_lStorageMax1;					//库存上限1
    LONGLONG						m_lStorageMul1;					//系统输分概率1
    LONGLONG						m_lStorageMax2;					//库存上限2
    LONGLONG						m_lStorageMul2;					//系统输分概率2

    //区域控制
protected:
    BYTE m_cbWinArea;				//赢牌区域
    BYTE m_cbExcuteTimes;			//执行次数

    //玩家下注
protected:
    CMD_CF_UserInfo					m_UserInfo;						//玩家信息
    CListCtrl						m_listUserBet;					//列表控件
    CListCtrl						m_listUserBetAll;				//列表控件
    LONGLONG						m_lQueryGameID;					//查询ID

public:
    CClientControlItemSinkDlg(CWnd *pParent = NULL);   // 标准构造函数
    virtual ~CClientControlItemSinkDlg();

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
    virtual bool Create(CWnd *pParentWnd, IClientDebugCallback *pIClientControlCallback);
    // 显示窗口
    virtual bool ShowWindow(bool bShow);
    // 消息函数
    virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);
    // 控制信息
    bool SendDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
    //取得用户信息
    int GetUserInfoIndex(WORD wChairID, WORD wTableID = 0);

public:
    //申请结果
    virtual bool __cdecl ReqResult(const void *pBuffer);
    //更新库存
    virtual bool __cdecl UpdateStorage(const void *pBuffer);
    //更新下注
    virtual void __cdecl UpdateUserBet(bool bReSet);
    //更新控件
    virtual void __cdecl UpdateControl();

    virtual BOOL OnInitDialog();
    afx_msg void  OnReSet();
    afx_msg void  OnRefresh();
    afx_msg void  OnExcute();

public:
    afx_msg void OnBnClickedBtnUpdate();
    afx_msg void OnBnClickedBtnUpdateStorage();
    void RequestUpdateStorage();
    afx_msg void OnBnClickedBtnUserBetQuery();
    afx_msg void OnBnClickedBtnUserBetAll();
    afx_msg void OnEnSetfocusEditUserId();
    afx_msg void OnBnClickedRadioRb();
    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
};
