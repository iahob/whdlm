#pragma once
#include "ClientDebug.h"
#include "../../../������/Include/ClientDebug.h "

typedef enum
{
	QUERY_USER_GAMEID,
	QUERY_USER_NICKNAME,
	QUERY_INVALID
}QUERY_TYPE;

class AFX_EXT_CLASS CClientDebugItemSinkDlg : public IClientDebugDlg, public IClientDebug
{
	DECLARE_DYNAMIC(CClientDebugItemSinkDlg)

protected:
	CWnd *								m_pParentWnd;					// ������
	IClientDebugCallback *			m_pIClientDebugCallback;		// �ص��ӿ�

	LONGLONG				m_lMaxRoomStorage[2];						//�������
	WORD					m_wRoomStorageMul[2];						//Ӯ�ָ���
	LONGLONG				m_lRoomStorageCurrent;						//�����ֵ
	LONGLONG				m_lRoomStorageDeduct;						//���˥��

    DWORD					m_dwQueryUserGameID;						//��ѯGAMEID
	TCHAR					m_szQueryUserNickName[LEN_NICKNAME];		//��ѯ�û��ǳ�
	QUERY_TYPE				m_QueryType;								//��ѯ�û�������

public:
	//�ؼ�����
	CSkinEdit				m_editCurrentStorage;
	CSkinEdit				m_editStorageDeduct;
	CSkinEdit				m_editStorageMax1;
	CSkinEdit				m_editStorageMul1;
	CSkinEdit				m_editStorageMax2;
	CSkinEdit				m_editStorageMul2;
	CSkinEdit				m_editUserID;
	CSkinEdit				m_editDebugCount;
	CSkinButton				m_btUpdateStorage;
	CSkinButton				m_btModifyStorage;
	CSkinButton				m_btContinueWin;
	CSkinButton				m_btContinueLost;
	CSkinButton				m_btContinueCancel;
	CSkinButton				m_btQueryUser;
	CSkinRichEdit			m_richEditUserDescription;
	CSkinRichEdit			m_richEditUserDebug;
	CSkinRichEdit			m_richEditOperationRecord;

public:
	CClientDebugItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClientDebugItemSinkDlg();

// �Ի�������
	enum { IDD = IDD_CLIENT_DEBUG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	// �̳к���
public:
	// �ͷŽӿ�
	virtual void Release();
	// ��������
	virtual bool Create(CWnd * pParentWnd, IClientDebugCallback * pIClientDebugCallback);
	// ��ʾ����
	virtual bool ShowWindow(bool bShow);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize);

public:
	//���÷�����
	virtual void SetRoomStorage(LONGLONG lRoomStartStorage, LONGLONG lRoomCurrentStorage);
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo);
	//��ѯ�û����
	virtual void RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult);
	//�����û����Խ��
	virtual void RoomUserDebugResult(CMD_S_UserDebug *pUserDebug);
	//�û��������
	virtual void UserDebugComplete(CMD_S_UserDebugComplete *pUserDebugComplete);
	//������¼
	virtual void UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record);
	//���ø��¶�ʱ��
	virtual void SetUpdateIDI(); 
	//���·�����Ϣ
	virtual void UpdateRoomInfoResult(CMD_S_RequestUpdateRoomInfo_Result *RoomInfo_Result);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void  OnCancel();

	//�޸Ŀ������
	afx_msg void OnModifyStorage();
	//���¿��
	afx_msg void OnUpdateStorage();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnSetfocusEditUserId();
	afx_msg void OnContinueDebugWin();
	afx_msg void OnContinueDebugLost();
	afx_msg void OnContinueDebugCancel();
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnQueryUserGameID();
	afx_msg void OnQueryUserNickName();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

//���ܺ���
public:
	//��ȡ�û�״̬
	void GetUserStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &userStatus);
	//��ȡ��Ϸ״̬
	void GetGameStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &gameStatus);
	//��ȡ�Ƿ��������
	void GetSatisfyDebugString(CMD_S_RequestQueryResult *pQueryResult, CString &satisfyDebug, bool &bEnableDebug);
	//��ȡ��������
	void GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr);
	// ������Ϣ
	bool SendDebugMessage(UINT nMessageID, void * pData = NULL, UINT nSize = 0);
};
