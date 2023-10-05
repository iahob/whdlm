#pragma once

typedef struct
{
	LONGLONG						lTableCurrentStorage;				//���ӵ�ǰ���
	LONGLONG						lTableStorageInput;					//���ӿ��Ͷ��	
	LONGLONG						lTableWinScore;						//������Ӯ
}TABLESTORAGEINFO;

class AFX_EXT_CLASS CRoomAdvanceDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CRoomAdvanceDlg)

public:
	CRoomAdvanceDlg(CWnd *pParent = NULL); 
	virtual ~CRoomAdvanceDlg();

	enum { IDD = IDD_ROOM_ADVANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	//��ʼ���Ի���
	virtual BOOL OnInitDialog();
	//��ϢԤ����
	virtual BOOL PreTranslateMessage(MSG *pMsg);

	afx_msg void OnCancel();
	afx_msg void OnClickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnKillFocusListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnSetFocusListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSingleModeConfirm();
	afx_msg void OnBatchConfirm();
	afx_msg void OnConfirmSelected();
	afx_msg void OnDeleteSelected();
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	
	//���ܺ���
public:
	//��������ӳ����
	void UpdateTableMapStorage(CMD_S_UpdateTableMapStorage *pUpdateTableMapStorage);
	//��ʾ����ӳ����
	void ShowTableMapStorage();

	//��Ϸ����
protected:
	CMap<WORD, WORD, TABLESTORAGEINFO, TABLESTORAGEINFO>	m_MapTableStorage;						//���Ӻ�ӳ��ÿ�����ӵĿ��
	CList<WORD, WORD&>										m_ListBatchTableID;
	WORD													m_wTableID;
	LONGLONG												m_lModifyStorageSingle;
	LONGLONG												m_lModifyStorageBatch;

public:
	//�ؼ�����
	CSkinListCtrl									m_listTableMapStorage;
	CSkinListCtrl									m_listTableMapStorageAll;
	CSkinEdit										m_editTableID;
	CSkinEdit										m_editModifySingleTable;
	CSkinEdit										m_editBatchStorage;
	CSkinButton										m_btSingleModeConfirm;
	CSkinButton										m_btBatchConfirm;
	CSkinButton										m_btConfirmSelected;
	CSkinButton										m_btDeleteSelected;
	CSkinRichEdit									m_richEditBatchDescription;
};
