#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "DialogDexter.h"

#pragma warning(disable : 4244)
#define CUSTOM_GENERAL			0
#define CUSTOM_ADVANCED			1
#define MAX_CUSTOM				2

// 配置窗口
class CServerConfigDlg : public CDialog
{
    // 对话框数据
    enum { IDD = IDD_DIALOG_MAIN };

    //控件变量
protected:
    CDialogDexter 					*m_DialogCustom[MAX_CUSTOM];

    // 函数定义
public:
    // 构造函数
    CServerConfigDlg();
    // 析构函数
    virtual ~CServerConfigDlg();
    // 窗口销毁
    void OnDestroy();

    // 重载函数
protected:
    // 控件绑定
    virtual VOID DoDataExchange(CDataExchange *pDX);
    // 初始化函数
    virtual BOOL OnInitDialog();

	// 继承函数
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonRead();
    afx_msg void OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult);

	//辅助函数
public:
	//获取收放表类型
	EM_TABLE_TYPE GetTableType(LONGLONG lTableWinScore);
};

//////////////////////////////////////////////////////////////////////////////////
// 控制对话框
class CDialogGeneral : public CDialogDexter
{
    // 资源ID
public:
    enum { IDD = IDD_CUSTOM_GENERAL };

    // 类变量
public:

    // 类函数
public:
    // 构造函数
    CDialogGeneral(int nShowMax = 0, CServerConfigDlg *pParent = NULL);
    // 析构函数
    virtual ~CDialogGeneral();

    // 窗口函数
protected:
    //初始化窗口
    virtual BOOL OnInitDialog();
    // 控件绑定
    virtual void DoDataExchange(CDataExchange *pDX);

    // 继承函数
public:
    DECLARE_DYNAMIC(CDialogGeneral)
    DECLARE_MESSAGE_MAP()
};


// 控制对话框
class CDialogAdvanced : public CDialogDexter
{
    // 资源ID
public:
    enum { IDD = IDD_CUSTOM_ADVANCED };

    // 类变量
public:

    // 类函数
public:
    // 构造函数
    CDialogAdvanced(int nShowMax = 0, CServerConfigDlg *pParent = NULL);
    // 析构函数
    virtual ~CDialogAdvanced();

    // 窗口函数
protected:
    //初始化窗口
    virtual BOOL OnInitDialog();
    // 控件绑定
    virtual void DoDataExchange(CDataExchange *pDX);

    // 继承函数
public:
    DECLARE_DYNAMIC(CDialogAdvanced)
    DECLARE_MESSAGE_MAP()
	
	afx_msg void OnBnClickedButtonTableBefore();
	afx_msg void OnBnClickedButtonTableNext();
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnLvnItemchangedListTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditDebugValue();
	afx_msg void OnEnChangeEditDifficulty();
	afx_msg void OnBnClickedButtonSetgraderange();
	afx_msg void OnBnClickedButtonSetstoragerange();

	//辅助函数
public:
	//表查询
	virtual void OnTableQuery();
	//获取收放表类型
	EM_TABLE_TYPE GetTableType(LONGLONG lTableWinScore);
	//获取控件数据
	SCORE GetCtrlIDInter(WORD wCtrlID);
};

#endif