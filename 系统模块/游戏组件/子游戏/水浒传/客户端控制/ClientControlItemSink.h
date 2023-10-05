#pragma once
#include "ClientControl.h"
#include "../../../开发库/Include/ClientDebug.h "
#include "afxcmn.h"

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientDebug
{
public:
	IClientDebugCallback *					m_pIClientDebugCallback;		// 回调接口
	bool					m_bAutoUpdata;
	int						m_nRoomUserList;
	int						m_nRoomStorage;
	int						m_nRoomTableStorage;
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

public:
	CClientControlItemSinkDlg(CWnd *pParent = NULL); 
	virtual ~CClientControlItemSinkDlg();

	enum { IDD = IDD_CLIENT_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	//设置控制端信息
	virtual void SetControlDlgInfo(WORD wTable, TCHAR szServerName[LEN_SERVER]);
	//设置桌子起始库存
	virtual void SetStartStorageTable(LONGLONG lStorageScore);
	//设置房间库存
	virtual void SetRoomStorage(LONGLONG lRoomStorageStart, LONGLONG lRoomStorageCurrent, LONGLONG lRoomStorageInput, LONGLONG lRoomStockRecoverScore);
	//更新当前库存
	virtual void UpdateCurrentStorage(LONGLONG lStorage);
	//设置库存信息
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo);
	//查询用户结果
	virtual void RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult);
	//房间用户控制结果
	virtual void RoomUserControlResult(CMD_S_UserControl *pUserControl);
	//用户控制完成
	virtual void UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete);
	//操作记录
	virtual void UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record);
	//更新房间用户列表
	virtual void UpdateRoomUserListCtrl(CMD_S_UpdateRoomUserList *pUpdateRoomUserList);
	//设置特殊权限
	virtual void SetCheatRight(bool bStart); 
	//初始化对话框
	virtual BOOL OnInitDialog();
	//消息预处理
	virtual BOOL PreTranslateMessage(MSG *pMsg);

//映射函数
public:
	afx_msg void OnCancel();
	afx_msg void OnModifyRoomConfig();
	afx_msg void OnRoomAdvance();
	afx_msg void OnUpdateStorageTable();
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnSetfocusEditUserId();
	afx_msg void OnContinueControlWin();
	afx_msg void OnContinueControlLost();
	afx_msg void OnContinueControlCancel();
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnClickListRoomUser(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnQueryUserGameID();
	afx_msg void OnQueryUserNickName();
	afx_msg void OnSelchangeComboType();

//功能函数
public:
	//获取用户状态
	void GetUserStatusString(BYTE cbUserStatus, CString &userStatus);
	//获取游戏状态
	void GetGameStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &gameStatus);
	//获取是否满足控制
	void GetSatisfyControlString(CMD_S_RequestQueryResult *pQueryResult, CString &satisfyControl, bool &bEnableControl);
	//获取控制类型
	void GetControlTypeString(USERCONTROL &UserControl, CString &controlTypestr);
	//获取控制类型
	CString GetControlDataString(USERCONTROL &UserControl);
	// 调试信息
	bool SendDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
public:
	// 释放接口
	virtual void Release();
	// 创建函数
	virtual bool Create(CWnd * pParentWnd, IClientDebugCallback * pIClientDebugCallback);
	// 显示窗口
	virtual bool ShowWindow(bool bShow);
	// 消息函数
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize);
	//设置更新定时器
	virtual void SetUpdateIDI();
	
protected:
	//游戏变量
	LONGLONG				m_lMaxStorageRoom[2];					//全局库存上限
	WORD					m_wStorageMulRoom[2];					//全局赢分概率
	WORD					m_wGameTwo;								//比倍概率
	WORD					m_wGameTwoDeduct;						//比倍概率
	WORD					m_wGameThree;							//小玛丽概率
	WORD					m_wGameThreeDeduct;						//小玛丽概率
	LONGLONG				m_lStorageCurrentTable;					//当前桌子库存
	LONGLONG				m_lStorageDeductRoom;					//全局库存衰减
	DWORD					m_dwQueryUserGameID;						//查询GAMEID
	TCHAR					m_szQueryUserNickName[LEN_NICKNAME];	//查询用户昵称
	WORD					m_wTableID;								//当前桌子ID
	TCHAR					m_szServerName[LEN_SERVER];				//当前房间名称
	CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO>	m_RoomUserInfoMap;			//房间用户信息映射

public:
	//控件变量
	CSkinEdit				m_editCurrentStorageTable;
	CSkinEdit				m_editStorageDeductRoom;
	CSkinEdit				m_editStorageMax1Room;
	CSkinEdit				m_editStorageMul1Room;
	CSkinEdit				m_editStorageMax2Room;
	CSkinEdit				m_editStorageMul2Room;
	CSkinEdit				m_editGameTwo;
	CSkinEdit				m_editGameTwoDeduct;
	CSkinEdit				m_editGameThree;
	CSkinEdit				m_editGameThreeDeduct;
	CSkinEdit				m_editUserID;
	CSkinEdit				m_editControlCount;
	CSkinEdit				m_editZhongJiang;
	CSkinEdit				m_editZhongJiangJian;
	CSkinButton				m_btUpdateStorageTable;
	CSkinButton				m_btModifyRoomConfig;
	CSkinButton				m_btRoomAdvance;
	CSkinButton				m_btContinueWin;
	CSkinButton				m_btContinueLost;
	CSkinButton				m_btContinueCancel;
	CSkinButton				m_btQueryUser;
	CSkinRichEdit			m_richEditUserDescription;
	CSkinRichEdit			m_richEditUserControl;
	CSkinRichEdit			m_richEditOperationRecord;
	CSkinListCtrl			m_listCtrlRoomUser;
	CSkinComboBox			m_ComboCount;
	CSkinComboBox			m_ComboType;

	CToolTipCtrl			m_toolTip;
};
