#pragma once
#include "ClientControl.h"
#include "../../../������/Include/ClientDebug.h "


class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientDebug
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	CWnd *										m_pParentWnd;					// ������
	IClientDebugCallback *						m_pIClientControlCallback;		// �ص��ӿ�

	LONGLONG				m_lMaxRoomStorage[2];						//�������
	WORD					m_wRoomStorageMul[2];						//Ӯ�ָ���
	LONGLONG				m_lRoomStorageCurrent;						//�����ֵ
	LONGLONG				m_lRoomStorageDeduct;						//���˥��

	CUserBetArray			m_UserBetArray;
	DWORD					m_dwQueryGameID;
	bool					m_bWinArea[CONTROL_AREA];					//�ܿ�����
	BYTE					m_cbExcuteTimes;							//ִ�д���

public:
	//�ؼ�����
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
	//�����ע
	CSkinEdit				m_editUserID;
	CSkinButton				m_btUserBetQuery;
	CSkinButton				m_btUserAllBet;

	CSkinListCtrl						m_listUserBet;					//�б�ؼ�
	CSkinListCtrl						m_listUserBetAll;				//�б�ؼ�

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClientControlItemSinkDlg();

// �Ի�������
	enum { IDD = IDD_CLIENT_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	// �̳к���
public:
	// �ͷŽӿ�
	virtual void Release();
	// ��������
	virtual bool Create(CWnd * pParentWnd, IClientDebugCallback * pIClientControlCallback);
	// ��ʾ����
	virtual bool ShowWindow(bool bShow);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize);

public:
	//���÷�����
	virtual void __cdecl UpdateUserBet(bool bReSet);
	virtual void SetRoomStorage(LONGLONG lRoomStartStorage, LONGLONG lRoomCurrentStorage);
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo);
	//�û��������
	//virtual void UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete);
	//���ø��¶�ʱ��
	virtual void SetUpdateIDI(); 
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void  OnCancel();

	//�޸Ŀ������
	afx_msg void OnModifyStorage();
	//���¿��
	afx_msg void OnUpdateStorage();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnContinueControlWin();
	afx_msg void OnContinueControlCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();

//���ܺ���
public:
	// ������Ϣ
	bool SendControlMessage(UINT nMessageID, void * pData = NULL, UINT nSize = 0);
};
