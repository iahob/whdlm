#pragma once
#include "stdafx.h"
#include "../游戏客户端/ClientControl.h"
#include "../../../开发库/Include/ClientDebug.h "

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg,public IClientDebug
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)
public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClientControlItemSinkDlg();
	enum { IDD = IDD_CLIENT_CONTROL_EX };

protected:
	int m_curSelUsr;
	WORD m_userChairID[GAME_PLAYER];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual void __cdecl OnAllowControl(bool bEnable);
	virtual bool __cdecl ReqResult(const void * pBuffer);
	virtual bool __cdecl AckReq(const void * pBuffer);
public:
	afx_msg void  OnControl();
	afx_msg void  OnGetInfo();
	afx_msg void  OnUpdateStorage();
	afx_msg void  OnRadioClick();
	afx_msg void OnUserChange();
protected:
	void _getSCSAAnimalTypeAndColorType(CMD_C_Control &controlReq);
	void _getSCMAAnimalTypeAndColorType(CMD_C_Control &controlReq);
	void _getMCSAAnimalTypeAndColorType(CMD_C_Control &controlReq);
public:
	virtual BOOL OnInitDialog();
public:
	// 释放接口
	virtual void Release();
	// 创建函数
	virtual bool Create(CWnd * pParentWnd, IClientDebugCallback * pIClientControlCallback);
	// 显示窗口
	virtual bool ShowWindow(bool bShow);
	// 消息函数
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize);
	// 控制信息
	bool SendControlMessage(UINT nMessageID, void * pData = NULL, UINT nSize = 0);
protected:
	IClientDebugCallback *						m_pIClientControlCallback;		// 回调接口
};
