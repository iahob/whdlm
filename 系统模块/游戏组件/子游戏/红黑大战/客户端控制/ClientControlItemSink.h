#pragma once
#include "ClientControl.h"
#include "../../../开发库/Include/ClientDebug.h "


class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientDebug
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	CWnd *										m_pParentWnd;					// 父窗口
	IClientDebugCallback *						m_pIClientControlCallback;		// 回调接口

	LONGLONG				m_lMaxRoomStorage[2];						//库存上限
	WORD					m_wRoomStorageMul[2];						//赢分概率
	LONGLONG				m_lRoomStorageCurrent;						//库存数值
	LONGLONG				m_lRoomStorageDeduct;						//库存衰减

	CUserBetArray			m_UserBetArray;
	DWORD					m_dwQueryGameID;
	bool					m_bWinArea[CONTROL_AREA];					//受控区域
	BYTE					m_cbExcuteTimes;							//执行次数

public:
	//控件变量
	CSkinEdit				m_editCurrentStorage;
	CSkinEdit				m_editStorageDeduct;
	CSkinEdit				m_editStorageMax1;
	CSkinEdit				m_editStorageMul1;
	CSkinEdit				m_editStorageMax2;
	CSkinEdit				m_editStorageMul2;
	CSkinEdit				m_editControlCount;
	CSkinButton				m_btUpdateStorage;
	CSkinButton				m_btModifyStorage;
	CSkinButton				m_btContinueWin;
	CSkinButton				m_btContinueCancel;
	CSkinRichEdit			m_richEditUserControl;
	//玩家下注
	CSkinEdit				m_editUserID;
	CSkinButton				m_btUserBetQuery;
	CSkinButton				m_btUserAllBet;

	CSkinListCtrl						m_listUserBet;					//列表控件
	CSkinListCtrl						m_listUserBetAll;				//列表控件

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClientControlItemSinkDlg();

// 对话框数据
	enum { IDD = IDD_CLIENT_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	// 继承函数
public:
	// 释放接口
	virtual void Release();
	// 创建函数
	virtual bool Create(CWnd * pParentWnd, IClientDebugCallback * pIClientControlCallback);
	// 显示窗口
	virtual bool ShowWindow(bool bShow);
	// 消息函数
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize);

public:
	//设置房间库存
	virtual void __cdecl UpdateUserBet(bool bReSet);
	virtual void SetRoomStorage(LONGLONG lRoomStartStorage, LONGLONG lRoomCurrentStorage);
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo);
	//用户控制完成
	//virtual void UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete);
	//设置更新定时器
	virtual void SetUpdateIDI(); 
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void  OnCancel();

	//修改库存上限
	afx_msg void OnModifyStorage();
	//更新库存
	afx_msg void OnUpdateStorage();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnContinueControlWin();
	afx_msg void OnContinueControlCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();

//功能函数
public:
	// 控制信息
	bool SendControlMessage(UINT nMessageID, void * pData = NULL, UINT nSize = 0);
};
