#pragma once
#include "Stdafx.h"
#include "ClientDebug.h"
#include "DialogDexter.h"

#define CUSTOM_GENERAL			0
#define CUSTOM_ADVANCED			1
#define MAX_CUSTOM				2

// ���ԶԻ���
class CClientDebugItemSinkDlg : public CDialog, public IClientDebug
{
	// �Ի�������
	enum { IDD = IDD_DIALOG_MAIN };

	// �����
public:
	CTime									m_tStartDebugTime;				// ����������ʱ��
	bool									m_bSystemStorageEnd;			// ϵͳ����ֹ	
	CTime									m_tResetSystemStorage;			// �������ʱ��
	DWORD									m_dwSysDebugIndex;				// ϵͳ��������
	LONGLONG								m_lLastInitSystemStorage;		// ��ʼϵͳ���
	LONGLONG								m_lInitSystemStorage;			// ��ʼϵͳ���
	LONGLONG								m_lSystemStorage;				// ϵͳ���
	LONGLONG								m_lLastInitUserStorage;			// ��ʼ��ҿ��
	LONGLONG								m_lInitUserStorage;				// ��ʼ��ҿ��
	LONGLONG								m_lUserStorage;					// ��ҿ��
	int										m_nInitParameterK;				// ��ʼ����K(�ٷֱ�)
	int										m_nResetCount;					// ������ô���
	LONGLONG								m_lEndStorageScore;				// ϵͳϵͳ����ֹ
	LONGLONG								m_lTotalSystemStorage;			// ϵͳϵͳ��Ӯ����
	LONGLONG								m_lTotalUserStorage;			// ϵͳ�����Ӯ����

	DWORD									m_dwRoomDebugIndex;				// �����������
	LONGLONG								m_lRoomInitSystemStorage;		// �����ʼϵͳ���
	LONGLONG								m_lRoomInitUserStorage;			// �����ʼ��ҿ��
	int										m_nRoomParameterK;				// �����ʼ����K(�ٷֱ�)
	LONGLONG								m_lRoomEndStorageScore;			// ����ϵͳ����ֹ
	LONGLONG								m_lRoomTotalSystemStorage;		// ����ϵͳ��Ӯ����
	LONGLONG								m_lRoomTotalUserStorage;		// ���������Ӯ����

	DWORD									m_dwUserDebugCount;				// ��ҵ�������
	DWORD									m_dwUserDebugIndex;				// ��ҵ�������
	DWORD									m_dwGameID;						// �������ID
	LONGLONG								m_lUserInitSystemStorage;		// ��ҳ�ʼϵͳ���
	LONGLONG								m_lUserInitUserStorage;			// ��ҳ�ʼ��ҿ��
	int										m_nUserParameterK;				// ��ҳ�ʼ����K(�ٷֱ�)
	LONGLONG								m_lUserEndStorageScore;			// ���ϵͳ����ֹ
	LONGLONG								m_lUserTotalSystemStorage;		// ���ϵͳ��Ӯ����
	LONGLONG								m_lUserTotalUserStorage;		// ��������Ӯ����

	bool									m_bInit;						// ��ʼ��
	CList<tagHistoryRoomUserInfo, tagHistoryRoomUserInfo&> m_listHistoryRoomUserInfo;		//��ҵ��Լ�¼
	CList<tagHistoryRoomDebugInfo, tagHistoryRoomDebugInfo&> m_listHistoryRoomDebugInfo;	//������Լ�¼
	CList<tagHistoryRoomDebugInfo, tagHistoryRoomDebugInfo&> m_listHistorySysDebugInfo;		//ϵͳ���Լ�¼

	CWnd 									*m_pParentWnd;					// ������
	IClientDebugCallback 					*m_pIClientDebugCallback;		// �ص��ӿ�

	// �����Ϣ
public:
	CDialogDexter 							*m_DialogCustom[MAX_CUSTOM];

	// �ຯ��
public:
	// ���캯��
	CClientDebugItemSinkDlg(CWnd *pParent = NULL);
	// ��������
	virtual ~CClientDebugItemSinkDlg();
	// ��������
	void OnDestroy();

	// ���ں���
public:
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);
	// ������Ϣ
	bool SendDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
	// ������Ϣ
	bool SendDebugMessage(UINT nMessageID, WORD wTableID, void *pData = NULL, UINT nSize = 0);

	// �̳к���
public:
	// �ͷŽӿ�
	virtual void Release();
	// ��������
	virtual bool Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback);
	// ��ʾ����
	virtual bool ShowWindow(bool bShow);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

	DECLARE_DYNAMIC(CClientDebugItemSinkDlg)
	DECLARE_MESSAGE_MAP()

	// �任ѡ��
	afx_msg void OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult);

	//������ҵ���
	void DeleteUserDebugInfo(DWORD dwDebugIndex);
	//������ҵ���
	void UpdateUserDebugInfo(tagHistoryRoomUserInfo *pHistoryRoomUserInfo);
	//������ҵ���
	void UpdateUserDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//���·������
	void DeleteRoomDebugInfo(DWORD dwDebugIndex);
	//���·������
	void UpdateRoomDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//���·������
	void UpdateRoomDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//����ϵͳ����
	void UpdateSysDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//����ϵͳ����
	void UpdateSysDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

public:
	//ȡ����ҵ���
	POSITION GetUserDebugInfo(DWORD dwDebugIndex, tagHistoryRoomUserInfo &HistoryRoomUserInfo);
	//ȡ�÷������
	POSITION GetRoomDebugInfo(DWORD dwDebugIndex, tagHistoryRoomDebugInfo &HistoryRoomDebugInfo);
	//ȡ��ϵͳ����
	POSITION GetSysDebugInfo(DWORD dwDebugIndex, tagHistoryRoomDebugInfo &HistoryRoomDebugInfo);
};


class AFX_EXT_CLASS CDialogGeneral : public CDialogDexter
{
	DECLARE_DYNAMIC(CDialogGeneral)

public:
	CDialogGeneral(CClientDebugItemSinkDlg *pParent = NULL);
	virtual ~CDialogGeneral();

	enum { IDD = IDD_CUSTOM_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	// �̳к���
public:
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

public:
	//��ʼ���Ի���
	virtual BOOL OnInitDialog();
	//��ϢԤ����
	virtual BOOL PreTranslateMessage(MSG *pMsg);

//ӳ�亯��
public:
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//������ҵ���
	virtual void DeleteUserDebugInfo(DWORD dwDebugIndex);
	//������ҵ���
	virtual void UpdateUserDebugInfo(tagHistoryRoomUserInfo *pHistoryRoomUserInfo);
	//������ҵ���
	virtual void UpdateUserDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//���·������
	virtual void DeleteRoomDebugInfo(DWORD dwDebugIndex);
	//���·������
	virtual void UpdateRoomDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//���·������
	virtual void UpdateRoomDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//����ϵͳ����
	virtual void UpdateSysDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//����ϵͳ����
	virtual void UpdateSysDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime);


protected:
	WORD					m_wTableID;								//��ǰ����ID
	TCHAR					m_szServerName[LEN_SERVER];				//��ǰ��������
	CMap<DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo>	m_RoomUserInfoMap;			//�����û���Ϣӳ��
public:
	//ˢ�¿��
	afx_msg void OnBnClickedButtonRefreshrule();
	//ϵͳ���仯
	afx_msg void OnEnChangeEditSystemStorage();
	//��ҿ��仯
	afx_msg void OnEnChangeEditUserStorage();
	//����ϵ���仯
	afx_msg void OnEnChangeEditParameterK();
	//���õ�ǰ���
	afx_msg void OnBnClickedButtonCurStorage();
	//���ó�ʼ���
	afx_msg void OnBnClickedButtonInitStorage();

	//ϵͳ���仯
	afx_msg void OnEnChangeEditRoomSystemStorage();
	//��ҿ��仯
	afx_msg void OnEnChangeEditRoomUserStorage();
	//����ϵ���仯
	afx_msg void OnEnChangeEditRoomParameterK();
	//ˢ�·�����
	afx_msg void OnBnClickedBtnRefreshRoomList();
	//���ÿ��
	afx_msg void OnBnClickedBtnSetRoomStorage();
	//ȡ��������
	afx_msg void OnBnClickedBtnCancelRoomStorage();
	//���������б�
	afx_msg void OnLvnItemchangedListRoom(NMHDR *pNMHDR, LRESULT *pResult);

	//ϵͳ���仯
	afx_msg void OnEnChangeEditUserSystemStorage();
	//��ҿ��仯
	afx_msg void OnEnChangeEditUserUserStorage();
	//����ϵ���仯
	afx_msg void OnEnChangeEditUserParameterK();
	//ˢ�·�����
	afx_msg void OnBnClickedBtnRefreshUserList();
	//������ҿ��
	afx_msg void OnBnClickedBtnSetUserStorage();
	//ȡ����ҿ��
	afx_msg void OnBnClickedBtnCancelUserStorage();
	//��������б�
	afx_msg void OnLvnItemchangedListUser(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCurCaiJing();
};

class CDialogAdvanced : public CDialogDexter
{
	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_ADVANCED };

	// �����
public:

	// �ຯ��
public:
	// ���캯��
	CDialogAdvanced(CClientDebugItemSinkDlg *pParent = NULL);
	// ��������
	virtual ~CDialogAdvanced();

	// ���ں���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

	//������ҵ���
	virtual void DeleteUserDebugInfo(DWORD dwDebugIndex);
	//������ҵ���
	virtual void UpdateUserDebugInfo(tagHistoryRoomUserInfo *pHistoryRoomUserInfo);
	//������ҵ���
	virtual void UpdateUserDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//���·������
	virtual void DeleteRoomDebugInfo(DWORD dwDebugIndex);
	//���·������
	virtual void UpdateRoomDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//���·������
	virtual void UpdateRoomDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//����ϵͳ����
	virtual void UpdateSysDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//����ϵͳ����
	virtual void UpdateSysDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime);
	//����ϵͳ������
	void UpdateAllDebugInfo();

	// �̳к���
public:
	DECLARE_DYNAMIC(CDialogAdvanced)
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonRefreshHistory();
	afx_msg void OnBnClickedButtonRoomCancel();
	afx_msg void OnBnClickedButtonUserCancel();
};