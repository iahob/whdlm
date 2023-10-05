#pragma once
#include "ClientControl.h"
#include "../../../������/Include/ClientDebug.h "
#include "afxcmn.h"

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientDebug
{
public:
	IClientDebugCallback *					m_pIClientDebugCallback;		// �ص��ӿ�
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	//���ÿ��ƶ���Ϣ
	virtual void SetControlDlgInfo(WORD wTable, TCHAR szServerName[LEN_SERVER]);
	//����������ʼ���
	virtual void SetStartStorageTable(LONGLONG lStorageScore);
	//���÷�����
	virtual void SetRoomStorage(LONGLONG lRoomStorageStart, LONGLONG lRoomStorageCurrent, LONGLONG lRoomStorageInput, LONGLONG lRoomStockRecoverScore);
	//���µ�ǰ���
	virtual void UpdateCurrentStorage(LONGLONG lStorage);
	//���ÿ����Ϣ
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo);
	//��ѯ�û����
	virtual void RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult);
	//�����û����ƽ��
	virtual void RoomUserControlResult(CMD_S_UserControl *pUserControl);
	//�û��������
	virtual void UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete);
	//������¼
	virtual void UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record);
	//���·����û��б�
	virtual void UpdateRoomUserListCtrl(CMD_S_UpdateRoomUserList *pUpdateRoomUserList);
	//��������Ȩ��
	virtual void SetCheatRight(bool bStart); 
	//��ʼ���Ի���
	virtual BOOL OnInitDialog();
	//��ϢԤ����
	virtual BOOL PreTranslateMessage(MSG *pMsg);

//ӳ�亯��
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

//���ܺ���
public:
	//��ȡ�û�״̬
	void GetUserStatusString(BYTE cbUserStatus, CString &userStatus);
	//��ȡ��Ϸ״̬
	void GetGameStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &gameStatus);
	//��ȡ�Ƿ��������
	void GetSatisfyControlString(CMD_S_RequestQueryResult *pQueryResult, CString &satisfyControl, bool &bEnableControl);
	//��ȡ��������
	void GetControlTypeString(USERCONTROL &UserControl, CString &controlTypestr);
	//��ȡ��������
	CString GetControlDataString(USERCONTROL &UserControl);
	// ������Ϣ
	bool SendDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
public:
	// �ͷŽӿ�
	virtual void Release();
	// ��������
	virtual bool Create(CWnd * pParentWnd, IClientDebugCallback * pIClientDebugCallback);
	// ��ʾ����
	virtual bool ShowWindow(bool bShow);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize);
	//���ø��¶�ʱ��
	virtual void SetUpdateIDI();
	
protected:
	//��Ϸ����
	LONGLONG				m_lMaxStorageRoom[2];					//ȫ�ֿ������
	WORD					m_wStorageMulRoom[2];					//ȫ��Ӯ�ָ���
	WORD					m_wGameTwo;								//�ȱ�����
	WORD					m_wGameTwoDeduct;						//�ȱ�����
	WORD					m_wGameThree;							//С��������
	WORD					m_wGameThreeDeduct;						//С��������
	LONGLONG				m_lStorageCurrentTable;					//��ǰ���ӿ��
	LONGLONG				m_lStorageDeductRoom;					//ȫ�ֿ��˥��
	DWORD					m_dwQueryUserGameID;						//��ѯGAMEID
	TCHAR					m_szQueryUserNickName[LEN_NICKNAME];	//��ѯ�û��ǳ�
	WORD					m_wTableID;								//��ǰ����ID
	TCHAR					m_szServerName[LEN_SERVER];				//��ǰ��������
	CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO>	m_RoomUserInfoMap;			//�����û���Ϣӳ��

public:
	//�ؼ�����
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
