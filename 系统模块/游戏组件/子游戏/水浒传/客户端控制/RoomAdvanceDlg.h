#pragma once

typedef struct
{
	LONGLONG						lTableCurrentStorage;				//桌子当前库存
	LONGLONG						lTableStorageInput;					//桌子库存投入	
	LONGLONG						lTableWinScore;						//本桌输赢
}TABLESTORAGEINFO;

class AFX_EXT_CLASS CRoomAdvanceDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CRoomAdvanceDlg)

public:
	CRoomAdvanceDlg(CWnd *pParent = NULL); 
	virtual ~CRoomAdvanceDlg();

	enum { IDD = IDD_ROOM_ADVANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	//初始化对话框
	virtual BOOL OnInitDialog();
	//消息预处理
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
	
	//功能函数
public:
	//更新桌号映射库存
	void UpdateTableMapStorage(CMD_S_UpdateTableMapStorage *pUpdateTableMapStorage);
	//显示桌号映射库存
	void ShowTableMapStorage();

	//游戏变量
protected:
	CMap<WORD, WORD, TABLESTORAGEINFO, TABLESTORAGEINFO>	m_MapTableStorage;						//桌子号映射每张桌子的库存
	CList<WORD, WORD&>										m_ListBatchTableID;
	WORD													m_wTableID;
	LONGLONG												m_lModifyStorageSingle;
	LONGLONG												m_lModifyStorageBatch;

public:
	//控件变量
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
