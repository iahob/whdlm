#pragma once
#include "Stdafx.h"
#include "ClientControl.h"
#include "..\..\..\..\ϵͳģ��\�ͻ������\������\ClientControl.h"

// CClientControlItemSinkDlg �Ի���
typedef IClientUserItem * CTableUserItemArray[GAME_PLAYER];				//��Ϸ����
class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg, public IClientDebug
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

//������
protected:
	CWnd *							m_pParentWnd;					// ������
	IClientDebugCallback *			m_pIClientControlCallback;		// �ص��ӿ�

	LONGLONG						m_lStorageStart;				//��ʼ���
	LONGLONG						m_lStorageDeduct;				//���˥��
	LONGLONG						m_lStorageCurrent;				//��ǰ���
	LONGLONG						m_lStorageMax1;					//�������1
	LONGLONG						m_lStorageMul1;					//ϵͳ��ָ���1
	LONGLONG						m_lStorageMax2;					//�������2
	LONGLONG						m_lStorageMul2;					//ϵͳ��ָ���2

	CTableUserItemArray		        m_TableUserItemArray;			//���ߵ��û��б�
	CSkinButton						m_btUpdateStorage;
	CSkinButton						m_btUpdateStorage2;
	CSkinButton						m_btExcute;
	CSkinButton						m_btReset;
	CSkinButton						m_btUserBetQuery;
	CSkinButton						m_btUserBetAll;
	CSkinButton						m_btCurset;
	CSkinEdit						m_edStorageStart;				//��ʼ���
	CSkinEdit						m_edStorageDeduct;				//���˥��
	CSkinEdit						m_edStorageCurrent;				//��ǰ���
	CSkinEdit						m_edStorageMax1;				//�������1
	CSkinEdit						m_edStorageMul1;				//ϵͳ��ָ���1
	CSkinEdit						m_edStorageMax2;				//�������2
	CSkinEdit						m_edStorageMul2;				//ϵͳ��ָ���2
	CSkinEdit						m_edUserID;

	//�������
protected:
	bool m_bWinArea[enAreaCount];
	BYTE m_cbControlStyle;
	BYTE m_cbExcuteTimes;
	
	//�����ע
protected:
	CListCtrl						m_listUserBet;					//�б�ؼ�
	CListCtrl						m_listUserBetAll;				//�б�ؼ�
	LONGLONG						m_lQueryGameID;					//��ѯID
	CUserBetArray			        m_UserBetArray;

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClientControlItemSinkDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ADMIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
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

public:
	//���¿���
	 void  OnAllowControl(bool bEnable);
	//������
	 bool  ReqResult( void * pBuffer, WORD wDataSize);
	//���¿��
	 bool  UpdateStorage( void * pBuffer, WORD wDataSize);
	//������ע
	 void  UpdateUserBet(bool bReSet);
	//���¿ؼ�
	 void  UpdateControl();

public:
	//���ý���
	void ReSetAdminWnd();

public:
	//��ʾ״̬
	void PrintCurStatus();

public:
	afx_msg void  OnCancel();
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void  OnRadioClick();
protected:
	void RequestUpdateStorage();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnUpdateStorage();
	afx_msg void OnBnClickedBtnUpdateStorage2();
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();
	afx_msg void OnEnSetfocusEditUserId();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	//���ܺ���
public:
	//������Ϣ
	bool SendControlMessage(UINT nMessageID, void * pData = NULL, UINT nSize = 0);
	//�û�����
	virtual VOID	OnEventUserEnter(IClientUserItem * pIClientUserItem);
	//�û��뿪
	virtual VOID	OnEventUserLeave(IClientUserItem * pIClientUserItem);
	//�û�״̬
	virtual VOID   OnEventUserStatus(IClientUserItem *pIClientUserItem);
};
