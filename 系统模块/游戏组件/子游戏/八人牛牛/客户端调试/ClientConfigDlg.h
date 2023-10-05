#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "DialogDexter.h"
#include "..\消息定义\CMD_Ox.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4800)

#define CUSTOM_GENERAL			0
#define CUSTOM_ADVANCED			1
#define CUSTOM_TRACELOG			2
#define MAX_CUSTOM				3
#define MAX_EXECUTE_TIMES		15
#define MAX_EXECUTETIMES_CONFIG	5

// 调试对话框
class CClientConfigDlg : public CDialog, public IClientDebug
{
    // 对话框数据
    enum { IDD = IDD_DIALOG_MAIN };

    // 类变量
public:
    CWnd 									*m_pParentWnd;					// 父窗口
    IClientDebugCallback 					*m_pIClientDebugCallback;		// 回调接口

    // 组件信息
public:
    CDialogDexter 							*m_DialogCustom[MAX_CUSTOM];

    // 类函数
public:
    // 构造函数
    CClientConfigDlg(CWnd *pParent = NULL);
    // 析构函数
    virtual ~CClientConfigDlg();
    // 窗口销毁
    void OnDestroy();

    // 窗口函数
public:
    // 控件绑定
    virtual void DoDataExchange(CDataExchange *pDX);
    // 调试信息
    bool SendDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
	// 调试信息
	bool SendDebugMessage(UINT nMessageID, WORD wTableID, void *pData = NULL, UINT nSize = 0);

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

    DECLARE_DYNAMIC(CClientConfigDlg)
    DECLARE_MESSAGE_MAP()

    // 变换选项
    afx_msg void OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult);
};

//////////////////////////////////////////////////////////////////////////////////
// 调试对话框
class CDialogGeneral : public CDialogDexter
{
	// 资源ID
public:
	enum { IDD = IDD_CUSTOM_GENERAL };

	// 类函数
public:
	// 构造函数
	CDialogGeneral(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
	// 析构函数
	virtual ~CDialogGeneral();

	// 窗口函数
protected:
	//初始化窗口
	virtual BOOL OnInitDialog();
	// 控件绑定
	virtual void DoDataExchange(CDataExchange *pDX);
	//消息处理
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pBuffer, WORD wDataSize);

public:
	//获取中奖率
	WORD GetWinRate(SCORE lSysStorage, SCORE lPlayerStorage, SCORE lParameterK);

	// 继承函数
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonAnchou();
	afx_msg void OnBnClickedButtonSysctrl();
	afx_msg void OnBnClickedButtonRoomctrl();
	afx_msg void OnBnClickedButtonCancelRoomctrl();
};

class CDialogAdvanced : public CDialogDexter
{
	// 资源ID
public:
	enum { IDD = IDD_CUSTOM_ADVANCED };

	//玩家下注
protected:
	CMD_CF_UserInfo					m_UserInfo;						//玩家信息
	ROOMUSERINFO					m_curQueryRoomUserInfo;			//当前查询玩家信息
	BANERGAMETYPE_CONFIG			m_bgtConfig;

	// 类变量
public:

	// 类函数
public:
	// 构造函数
	CDialogAdvanced(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
	// 析构函数
	virtual ~CDialogAdvanced();

	// 窗口函数
protected:
	//初始化窗口
	virtual BOOL OnInitDialog();
	// 控件绑定
	virtual void DoDataExchange(CDataExchange *pDX);
	// 消息函数
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

public:
	//用户是否有效
	bool IsValidGameID(DWORD dwGameID);
	//获取UserID
	DWORD GetUserIDEx(DWORD dwGameID);

	//静态函数
protected:
	//排列函数
	static INT CALLBACK SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

	// 继承函数
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonQueryPlayerGameID();
	afx_msg void OnBnClickedButtonTablectrl();
	afx_msg void OnBnClickedButtonCancelTablectrl();
	afx_msg void OnLvnColumnclickListUserBet(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPlayerGameid();
};

class CDialogTraceLog : public CDialogDexter
{
	// 资源ID
public:
	enum { IDD = IDD_CUSTOM_TRACELOG };

	// 类变量
public:

	// 类函数
public:
	// 构造函数
	CDialogTraceLog(int nShowMax = 0, CClientConfigDlg *pParent = NULL);
	// 析构函数
	virtual ~CDialogTraceLog();

	// 窗口函数
protected:
	//初始化窗口
	virtual BOOL OnInitDialog();
	// 控件绑定
	virtual void DoDataExchange(CDataExchange *pDX);
	// 消息函数
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

	virtual void CleanCtrlList();
};

#endif