#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "..\消息定义\CMD_Game.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4800)


// 调试对话框
class CAIConfigDlg : public CDialog, public IAIConfigDebug
{
    // 对话框数据
    enum { IDD = IDD_DIALOG_MAIN };

    // 类变量
public:
    CWnd 									*m_pParentWnd;					// 父窗口
    IAIConfigDebugCallback 					*m_pIAIConfigDebugCallback;		// 回调接口
	tagAIConfig								m_CustomAIConfig;				

    // 类函数
public:
    // 构造函数
    CAIConfigDlg(CWnd *pParent = NULL);
    // 析构函数
    virtual ~CAIConfigDlg();

    // 窗口函数
public:
    // 控件绑定
    virtual void DoDataExchange(CDataExchange *pDX);
    // 调试信息
    bool SendAIConfigDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
	// 调试信息
	bool SendAIConfigDebugMessage(UINT nMessageID, WORD wTableID, void *pData = NULL, UINT nSize = 0);

    // 继承函数
public:
    // 释放接口
    virtual void Release();
    // 创建函数
    virtual bool Create(CWnd *pParentWnd, IAIConfigDebugCallback *pIAIConfigDebugCallback);
    // 显示窗口
    virtual bool ShowWindow(bool bShow);
    // 消息函数
	virtual bool OnAIConfigDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

    DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonQuery();
};

#endif