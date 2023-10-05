#pragma once
#include "Stdafx.h"
#include "ClientControl.h"


// CClientControlItemSinkDlg 对话框
#ifndef _UNICODE
#define _snprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define _snprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#define myatoi      _wtoi
#define myatoi64	_wtoi64
#endif


class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientControl
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)
protected:
	CWnd 							*m_pParentWnd;					// 父窗口
	IClientControlCallback 			*m_pIClientControlCallback;		// 回调接口

protected:
	BYTE m_cbControlArea;
	BYTE m_cbControlStyle;
	BYTE m_cbExcuteTimes;

	LONGLONG						m_lStorageStart;				//起始库存
	LONGLONG						m_lStorageDeduct;				//库存衰减
	LONGLONG						m_lStorageCurrent;				//当前库存
	LONGLONG						m_lCurrentBonus;				//当前彩金池
	LONGLONG						m_lStorageMax1;					//库存上限1
	LONGLONG						m_lStorageMul1;					//系统输分概率1
	LONGLONG						m_lStorageMax2;					//库存上限2
	LONGLONG						m_lStorageMul2;					//系统输分概率2

	CMD_CF_UserInfo					m_UserInfo;						//玩家信息
	CListCtrl						m_listUserBet;					//列表控件
	CListCtrl						m_listUserBetAll;				//列表控件
	LONGLONG						m_lQueryGameID;					//查询ID
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
	virtual bool Create(CWnd *pParentWnd, IClientControlCallback *pIClientControlCallback);
	// 显示窗口
	virtual bool ShowWindow(bool bShow);
	// 消息函数
	virtual bool OnControlMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);
	// 控制信息
	bool SendControlMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
	//取得用户信息
	int GetUserInfoIndex(WORD wChairID, WORD wTableID = 0);

public:
	//更新控制
	virtual void  UpdateControl(CMD_S_ControlReturns* pControlReturns);

protected:
	//信息解析
	void PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes);
	//重置界面
	void ReSetAdminWnd();
	//更新控件
	virtual void __cdecl UpdateControl();

public:
	//初始化
	virtual BOOL OnInitDialog();
	//设置颜色
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//取消控制
	afx_msg void OnBnClickedButtonReset();
	//开启控制
	afx_msg void OnBnClickedButtonOk();
	//取消关闭
	afx_msg void OnBnClickedButtonCancel();	
	afx_msg void OnBnClickedBtnUpdateStorage();
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();
	afx_msg void OnEnSetfocusEditUserId();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	virtual void  SetStorageCurrentValue(LONGLONG lCurrentStorage, LONGLONG lCurrentBonus);
	//更新下注
	virtual void __cdecl UpdateUserBet(bool bReSet);
	//更新库存
	virtual bool __cdecl UpdateStorage(const void * pBuffer);
   	virtual     void ClearText();

	bool CheckDataMinMax(LONGLONG valueMax);
	void RequestUpdateStorage();
	void ClearClist();
	
};


