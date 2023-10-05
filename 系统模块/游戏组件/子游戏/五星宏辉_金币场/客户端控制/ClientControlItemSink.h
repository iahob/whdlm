#pragma once
#include "Stdafx.h"
#include "ClientControl.h"
#include "..\..\..\..\系统模块\客户端组件\管理工具\ClientControl.h"

// CClientControlItemSinkDlg 对话框
typedef IClientUserItem * CTableUserItemArray[GAME_PLAYER];				//游戏数组
class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientDebug
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

//库存控制
protected:
	CWnd *							m_pParentWnd;					// 父窗口
	IClientDebugCallback *			m_pIClientControlCallback;		// 回调接口

	LONGLONG						m_lStorageStart;				//起始库存
	LONGLONG						m_lStorageDeduct;				//库存衰减
	LONGLONG						m_lStorageCurrent;				//当前库存
	LONGLONG						m_lStorageMax1;					//库存上限1
	LONGLONG						m_lStorageMul1;					//系统输分概率1
	LONGLONG						m_lStorageMax2;					//库存上限2
	LONGLONG						m_lStorageMul2;					//系统输分概率2

	CTableUserItemArray		        m_TableUserItemArray;			//在线的用户列表
	CSkinButton						m_btUpdateStorage;
	CSkinButton						m_btUpdateStorage2;
	CSkinButton						m_btExcute;
	CSkinButton						m_btReset;
	CSkinButton						m_btUserBetQuery;
	CSkinButton						m_btUserBetAll;
	CSkinButton						m_btCurset;
	CSkinEdit						m_edStorageStart;				//起始库存
	CSkinEdit						m_edStorageDeduct;				//库存衰减
	CSkinEdit						m_edStorageCurrent;				//当前库存
	CSkinEdit						m_edStorageMax1;				//库存上限1
	CSkinEdit						m_edStorageMul1;				//系统输分概率1
	CSkinEdit						m_edStorageMax2;				//库存上限2
	CSkinEdit						m_edStorageMul2;				//系统输分概率2
	CSkinEdit						m_edUserID;

	//区域控制
protected:
	bool m_bWinArea[enAreaCount];
	BYTE m_cbControlStyle;
	BYTE m_cbExcuteTimes;
	
	//玩家下注
protected:
	CListCtrl						m_listUserBet;					//列表控件
	CListCtrl						m_listUserBetAll;				//列表控件
	LONGLONG						m_lQueryGameID;					//查询ID
	CUserBetArray			        m_UserBetArray;

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClientControlItemSinkDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ADMIN };

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

public:
	//更新控制
	 void  OnAllowControl(bool bEnable);
	//申请结果
	 bool  ReqResult( void * pBuffer, WORD wDataSize);
	//更新库存
	 bool  UpdateStorage( void * pBuffer, WORD wDataSize);
	//更新下注
	 void  UpdateUserBet(bool bReSet);
	//更新控件
	 void  UpdateControl();

public:
	//重置界面
	void ReSetAdminWnd();

public:
	//显示状态
	void PrintCurStatus();

public:
	afx_msg void  OnCancel();
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void  OnRadioClick();
protected:
	void RequestUpdateStorage();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnUpdateStorage();
	afx_msg void OnBnClickedBtnUpdateStorage2();
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();
	afx_msg void OnEnSetfocusEditUserId();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	//功能函数
public:
	//控制信息
	bool SendControlMessage(UINT nMessageID, void * pData = NULL, UINT nSize = 0);
	//用户进入
	virtual VOID	OnEventUserEnter(IClientUserItem * pIClientUserItem);
	//用户离开
	virtual VOID	OnEventUserLeave(IClientUserItem * pIClientUserItem);
	//用户状态
	virtual VOID   OnEventUserStatus(IClientUserItem *pIClientUserItem);
};
