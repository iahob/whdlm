#pragma once
#include "Resource.h"

// ���ƶԻ���
class CDialogControl : public CDialog, public IClientControl
{
	// ��ԴID
public:
	enum { IDD = IDD_DIALOG_MAIN };

	// �����
public:
	HICON										m_hIcon;						// ����ͼ��
	CWnd *										m_pParentWnd;					// ������
	IClientControlCallback *					m_pIClientControlCallback;		// �ص��ӿ�

	// �ຯ��
public:
	// ���캯��
	CDialogControl(CWnd* pParent = NULL); 
	// ��������
	virtual ~CDialogControl();

	// ���ں���
protected:
	// �ؼ���
	virtual void DoDataExchange(CDataExchange* pDX); 

	// �̳к���
public:
	// �ͷŽӿ�
	virtual void Release();
	// ��������
	virtual bool Create( CWnd * pParentWnd, IClientControlCallback * pIClientControlCallback );
	// ��ʾ����
	virtual bool ShowWindow( bool bShow );
	// ��Ϣ����
	virtual bool OnControlMessage( uint nMessageID, void * pData, uint nSize );

	// ��ť����
public:
	// ����ѯ
	afx_msg void OnBnClickedButtonStockInquiry();
	// �������
	afx_msg void OnBnClickedButtonStockSet();
	// �Ѷ�����
	afx_msg void OnBnClickedButtonDifficultySet();
	// �Ѷ�ɾ��
	afx_msg void OnBnClickedButtonDifficultyDelete();
	// �ѶȲ�ѯ
	afx_msg void OnBnClickedButtonDifficultyInquiry();
	// �Ѷ����
	afx_msg void OnBnClickedButtonDifficultyClear();
	// ��������
	afx_msg void OnBnClickedButtonGiftSet();
	// ����ɾ��
	afx_msg void OnBnClickedButtonGiftDelete();
	// ���Ͳ�ѯ
	afx_msg void OnBnClickedButtonGiftInquiry();
	// �������
	afx_msg void OnBnClickedButtonGiftClear();
	// ��Ӯ����
	afx_msg void OnBnClickedButtonWinLoseUpdate();
	// ��־����
	afx_msg void OnBnClickedButtonLogUpdate();

	// ���ܺ���
public:
	// ��������
	CString GiftType( uint nGiftType );
	// ��ʽ�ַ���
	CString FormatString( tchar * pText, ... );

	DECLARE_DYNAMIC(CDialogControl)
	DECLARE_MESSAGE_MAP()
};
