#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "DialogDexter.h"
#include "..\消息定义\CMD_ZodiacBattle.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4800)

#define CUSTOM_GENERAL			0
#define CUSTOM_ADVANCED			1
#define MAX_CUSTOM				2
#define MAX_EXECUTE_TIMES		15

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

	//配置变量
protected:
	tagCustomRule					m_CustomRule;					//配置结构
	SCORE							m_lCurSysStorage;				//当前系统库存(系统要赢的钱)
	SCORE							m_lCurPlayerStorage;			//当前玩家库存(玩家要赢的钱)
	SCORE							m_lCurParameterK;				//当前调节系数(百分比):
	BOOL							m_nSysCtrlStorageDeadLine;
	SCORE							m_lStorageResetCount;
	SCORE							m_lSysCtrlDeadLineLimit;
	SCORE							m_lSysCtrlSysWin;
	SCORE							m_lSysCtrlPlayerWin;

	//区域控制
protected:
	BYTE							m_cbWinArea;					//赢牌区域
	BYTE							m_cbExecuteTimes;				//执行次数

	//玩家下注
protected:
	CMD_CF_UserInfo					m_UserInfo;						//玩家信息
	CListCtrl						m_listUserBet;					//列表控件
	LONGLONG						m_lQueryGameID;					//查询ID

protected:
	CUserBetArray					m_UserBetArray;					//用户下注

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

	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pBuffer, WORD wDataSize);

public:
	//判断有效数字
	bool IsValidNum(CString strEdit);

	//更新下注
	void UpdateUserBet(bool bReSet);
	//取得用户信息
	int GetUserInfoIndex(WORD wChairID, WORD wTableID = 0);

public:
	//申请结果
	virtual bool __cdecl ReqResult(const void *pBuffer);

	// 继承函数
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void  OnBnClickedBtnCancelAreaCtrl();
	afx_msg void  OnBnClickedBtnSetAreaCtrl();
	
	afx_msg void OnBnClickedButtonSetRule();
	afx_msg void OnLvnItemchangedListRoom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSetRoomCtrl();

	void UpdateSysCtrlDesc();
	afx_msg void OnEnChangeEditRoomctrlSysstorage();
	afx_msg void OnEnChangeEditRoomctrlPlayerstorage();
	afx_msg void OnEnChangeEditRoomctrlParameterK();
	void UpdateRoomCtrlDesc(SCORE lCurSysStorage, SCORE lCurPlayerStorage, SCORE lCurParameterK);

	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);

	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();
	afx_msg void OnEnSetfocusEditUserId();
};

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

	virtual void CleanCtrlList();

	// 继承函数
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedBtnCancelRoomCtrl();

	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
};


#endif