#pragma once
#include "Stdafx.h"
#include "ClientDebug.h"
#include "DialogDexter.h"

#define CUSTOM_GAME_RULE		0
#define CUSTOM_GENERAL			1
#define CUSTOM_ADVANCED			2
#define CUSTOM_WATCH			3
#define MAX_CUSTOM				4

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
	double									m_dInitSystemStorage;			// ��ʼϵͳ���
	double									m_dSystemStorage;				// ϵͳ���
	double									m_dInitUserStorage;				// ��ʼ��ҿ��
	double									m_dUserStorage;					// ��ҿ��
	int										m_nInitParameterK;				// ��ʼ����K(�ٷֱ�)
	int										m_nResetCount;					// ������ô���
	double									m_dEndStorageScore;				// ϵͳϵͳ����ֹ
	double									m_dTotalSystemStorage;			// ϵͳϵͳ��Ӯ����
	double									m_dTotalUserStorage;			// ϵͳ�����Ӯ����
	double									m_dDartStock;					// ���ڿ��
	double									m_dWaitDartStock;				// ���ڿ��
	longlong								m_lRoomRevenue;					// ����ʵ��˰��
	double									m_dStorageCurrent;				// ʵʱ��Ӯ

	DWORD									m_dwRoomDebugIndex;				// �����������
	double									m_dRoomInitSystemStorage;		// �����ʼϵͳ���
	double									m_dRoomInitUserStorage;			// �����ʼ��ҿ��
	int										m_nRoomParameterK;				// �����ʼ����K(�ٷֱ�)
	double									m_dRoomEndStorageScore;			// ����ϵͳ����ֹ
	double									m_dRoomTotalSystemStorage;		// ����ϵͳ��Ӯ����
	double									m_dRoomTotalUserStorage;		// ���������Ӯ����

	DWORD									m_dwUserDebugCount;				// ��ҵ�������
	DWORD									m_dwUserDebugIndex;				// ��ҵ�������
	DWORD									m_dwGameID;						// �������ID
	double									m_dUserInitSystemStorage;		// ��ҳ�ʼϵͳ���
	double									m_dUserInitUserStorage;			// ��ҳ�ʼ��ҿ��
	int										m_nUserParameterK;				// ��ҳ�ʼ����K(�ٷֱ�)
	double									m_dUserEndStorageScore;			// ���ϵͳ����ֹ
	double									m_dUserTotalSystemStorage;		// ���ϵͳ��Ӯ����
	double									m_dUserTotalUserStorage;		// ��������Ӯ����

	bool									m_bInit;						// ��ʼ��
	CListHistoryRoomUserInfo				m_listHistoryUserDebugInfo;		// ��ҵ��Լ�¼
	CListHistoryRoomDebugInfo				m_listHistoryRoomDebugInfo;		// ������Լ�¼
	CListHistoryRoomDebugInfo				m_listHistorySysDebugInfo;		// ϵͳ���Լ�¼

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
	virtual void DeleteUserDebugInfo(DWORD dwDebugIndex);
	//������ҵ���
	void UpdateUserDebugInfo(tagHistoryRoomUserInfo *pHistoryRoomUserInfo);
	//������ҵ���
	void UpdateUserDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//���·������
	virtual void DeleteRoomDebugInfo(DWORD dwDebugIndex);
	//���·������
	void UpdateRoomDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//���·������
	void UpdateRoomDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//����ϵͳ����
	void UpdateSysDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//����ϵͳ����
	void UpdateSysDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//����������Ϣ
	virtual void UpdateOtherInfo();

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
	virtual void UpdateUserDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//���·������
	virtual void DeleteRoomDebugInfo(DWORD dwDebugIndex);
	//���·������
	virtual void UpdateRoomDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//���·������
	virtual void UpdateRoomDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//����ϵͳ����
	virtual void UpdateSysDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//����ϵͳ����
	virtual void UpdateSysDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//����������Ϣ
	virtual void UpdateOtherInfo();


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


	//ϵͳ���仯
	afx_msg void OnEnChangeEditRoomSystemStorage();
	//��ҿ��仯
	afx_msg void OnEnChangeEditRoomUserStorage();
	//����ϵ���仯
	afx_msg void OnEnChangeEditRoomParameterK();
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
	virtual void UpdateUserDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//���·������
	virtual void DeleteRoomDebugInfo(DWORD dwDebugIndex);
	//���·������
	virtual void UpdateRoomDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//���·������
	virtual void UpdateRoomDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//����ϵͳ����
	virtual void UpdateSysDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo);
	//����ϵͳ����
	virtual void UpdateSysDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime);

	//����������Ϣ
	virtual void UpdateOtherInfo();

	//����ϵͳ������
	void UpdateAllDebugInfo();

	// �̳к���
public:
	DECLARE_DYNAMIC(CDialogAdvanced)
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonRoomCancel();
	afx_msg void OnBnClickedButtonUserCancel();
};


//��������
class CDialogCustom3 : public CDialogDexter
{
	// ��������
	CMDC_C_RuleScene					m_RuleScene;

	//ȫ�ֱ�ը���
	CMDC_C_RuleExplosion					m_RuleExplosion;

	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_RULE };

	// �����
public:

	// �ຯ��
public:
	// ���캯��
	CDialogCustom3(CClientDebugItemSinkDlg *pParent = NULL);
	// ��������
	virtual ~CDialogCustom3();

	// ���ں���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

	// �̳к���
public:
	DECLARE_DYNAMIC(CDialogCustom3)
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonSetScene();
	afx_msg void OnBnClickedButtonSetExplosion();
};

#define MAX_TABLE_COUNT 100
//��������
class CDialogWatch : public CDialogDexter
{
	// ��ԴID
public:
	enum { IDD = IDD_CUSTOM_WATCH };

	// �����
    CListCtrl						m_listUserWatch;					//�б�ؼ�
																		//������ע
	void UpdateUserWatch();
	void checkUserOffline();
	void doTableErase(int tableID);//����������

public:

	// �ຯ��
public:
	// ���캯��
	CDialogWatch(CClientDebugItemSinkDlg *pParent = NULL);
	// ��������
	virtual ~CDialogWatch();

	// ���ں���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	// �ؼ���
	virtual void DoDataExchange(CDataExchange *pDX);
	// ��Ϣ����
	virtual bool OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

	// �̳к���
protected:
	CMap<DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo>	m_RoomUserInfoMap;			//�����û���Ϣӳ��
	int64 m_nTableRefreshArr[MAX_TABLE_COUNT];
public:
	DECLARE_DYNAMIC(CDialogWatch)
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};