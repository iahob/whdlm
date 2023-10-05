// FishConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FishConfig.h"
#include "FishConfigDlg.h"
#include ".\fishconfigdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ��Ϣע��
BEGIN_MESSAGE_MAP(CFishConfigDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_OPTISONS, OnTcnSelchangeTabOptisons)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

#include <math.h>
#define GL_PI ((FLOAT) 3.141592654f)
// ���Ի���
CFishConfigDlg::CFishConfigDlg(CWnd* pParent /*=NULL*/) : CDialog(CFishConfigDlg::IDD, pParent)
{

}

// �ؼ���
void CFishConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

// ��ʼ��
BOOL CFishConfigDlg::OnInitDialog()
{
	// ����ִ��
	CDialog::OnInitDialog();

	// ��ȡ��ҳ
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_OPTISONS);
	
	// ����ѡ��
	pTabCtrl->InsertItem(CUSTOM_MULTIPLE, _T("��������"));
	pTabCtrl->InsertItem(CUSTOM_STOCK_DEBUG, _T("��������"));
	pTabCtrl->InsertItem(CUSTOM_SCENES, _T("��������"));
	pTabCtrl->InsertItem(CUSTOM_ACTIVITY, _T("�����"));
	pTabCtrl->InsertItem(CUSTOM_FISH, _T("������"));
	pTabCtrl->InsertItem(CUSTOM_ZHONG_JIAN, _T("�н���Ϣ����"));
	// 	pTabCtrl->InsertItem(CUSTOM_YUAN_BAO, _T("Ԫ������"));

	// ��ȡ��ҳ
	CRect RectWindow;
	CWnd * pWindowShow = GetDlgItem(IDC_STATIC_SHOW);
	pWindowShow->ShowWindow( SW_HIDE );
	pWindowShow->GetWindowRect(RectWindow);
	ScreenToClient(RectWindow);

	// ��������

	// ��������
	m_DialogCustom[CUSTOM_MULTIPLE] = new CDialogCustom1;
	m_DialogCustom[CUSTOM_MULTIPLE]->Create( IDD_DIALOG_1, this );

	// ��������
	m_DialogCustom[CUSTOM_STOCK_DEBUG] = new CDialogCustom2;
	m_DialogCustom[CUSTOM_STOCK_DEBUG]->Create( IDD_DIALOG_2, this );

	// ��������
	m_DialogCustom[CUSTOM_SCENES] = new CDialogCustom3;
	m_DialogCustom[CUSTOM_SCENES]->Create( IDD_DIALOG_3, this );
	CComboBox * pComboBox = (CComboBox *)m_DialogCustom[CUSTOM_SCENES]->GetDlgItem(IDC_COMBO1);
	pComboBox->InsertString( 0, _T("��������") );
	pComboBox->InsertString( 1, _T("���Ľ��") );
	pComboBox->SetCurSel( 0 );

	// �����
	m_DialogCustom[CUSTOM_ACTIVITY] = new CDialogCustom4;
	m_DialogCustom[CUSTOM_ACTIVITY]->Create( IDD_DIALOG_4, this );

	// ������
	m_DialogCustom[CUSTOM_FISH] = new CDialogCustom5;
	m_DialogCustom[CUSTOM_FISH]->Create( IDD_DIALOG_5, this );

	// ������
	m_DialogCustom[CUSTOM_ZHONG_JIAN] = new CDialogCustom6;
	m_DialogCustom[CUSTOM_ZHONG_JIAN]->Create( IDD_DIALOG_6, this );
// 	// Ԫ��
// 	m_DialogCustom[CUSTOM_YUAN_BAO] = new CDialogCustomYuanBao;
// 	m_DialogCustom[CUSTOM_YUAN_BAO]->Create( IDD_DIALOG_YUANBAO, this );
// 	CComboBox * pGoldComboBox = (CComboBox *)m_DialogCustom[CUSTOM_YUAN_BAO]->GetDlgItem(IDC_GOLD_COMBO);
// 	pGoldComboBox->InsertString( 0, _T("��������") );
// 	pGoldComboBox->InsertString( 1, _T("���Ľ��") );
// 	pGoldComboBox->SetCurSel( 0 );
	// ����λ��
	for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		CRect RectCustom;
		CSize SizeCustom;
		m_DialogCustom[nIndex]->GetClientRect( RectCustom );
		SizeCustom.SetSize( min(RectWindow.Width(), RectCustom.Width()), min(RectWindow.Height(), RectCustom.Height()) );
		m_DialogCustom[nIndex]->SetWindowPos( NULL, RectWindow.left + RectWindow.Width() / 2 - SizeCustom.cx / 2, RectWindow.top + RectWindow.Height() / 2 - SizeCustom.cy / 2, SizeCustom.cx, SizeCustom.cy, SWP_NOZORDER );
		m_DialogCustom[nIndex]->ShowWindow( ( nIndex == 0 ) ? SW_SHOW : SW_HIDE );

		//// ������Ϣ
		//CWnd* pWndChild = m_DialogCustom[nIndex]->GetWindow(GW_CHILD);    
		//while (pWndChild)
		//{       
		//	// ��ȡ��������
		//	tchar szClassName[256] = { _T("") };
		//	GetClassName( pWndChild->GetSafeHwnd(), szClassName, 256);

		//	// �жϴ���δ��Ч
		//	if ( !pWndChild->IsWindowEnabled() && (_tcsicmp( szClassName, _T("STATIC") ) == 0 || _tcsicmp( szClassName, _T("BUTTON")) == 0 ) )
		//	{
		//		pWndChild->SetWindowText( _T("��δ����") );
		//	}

		//	// ��һ������
		//	pWndChild = pWndChild->GetNextWindow();
		//} 
	}

	return TRUE; 
}

// �任ѡ��
void CFishConfigDlg::OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult)
{
	// ��ȡ��ҳ
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_OPTISONS);

	// ��ȡѡ����Ŀ
	int nCurSel = pTabCtrl->GetCurSel();
	if( nCurSel >= 0 && nCurSel < MAX_CUSTOM )
	{
		// �жϴ���
		for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
		{
			m_DialogCustom[nIndex]->ShowWindow( ( nIndex == nCurSel ) ? SW_SHOW : SW_HIDE );
		}
	}

	*pResult = 0;
}

// ��������
void CFishConfigDlg::OnDestroy()
{
	// ���ٴ���
	for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		m_DialogCustom[nIndex]->DestroyWindow();
		delete m_DialogCustom[nIndex];
	}

	CDialog::OnDestroy();
}

// Ĭ������
void CFishConfigDlg::OnBnClickedButton3()
{
	// Ĭ������
	g_CustomRule.DefaultCustomRule();

	// ���½���
	for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		m_DialogCustom[nIndex]->UpdateData(FALSE);
	}
}

// ������
void CFishConfigDlg::OnBnClickedButton2()
{
	// ���ļ�
	CFileDialog FileDialog( TRUE, NULL, m_StrFilePath, OFN_READONLY, _T("fdx files(*.fdx)|*.fdx||"), this );

	// ѡ���ļ�
	if ( FileDialog.DoModal() == IDOK )
	{
		// ������Ϣ
		m_StrFilePath = FileDialog.GetPathName();

		// ���ļ�
		FILE * pFile = NULL;
		fopen_s(&pFile, CT2A(m_StrFilePath), "rb");
		if ( pFile != NULL )
		{
			// ��ȡ�ļ�
			try
			{
				fread(&g_CustomRule, sizeof(g_CustomRule), 1, pFile );
			}
			catch (...)
			{
				g_CustomRule.DefaultCustomRule();
			}

			// �ر��ļ�
			fclose(pFile);

			// ���½���
			for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
			{
				m_DialogCustom[nIndex]->UpdateData(FALSE);
			}
		}
	}
}

// ��������
void CFishConfigDlg::OnBnClickedButton1()
{
	// ���ļ�
	CFileDialog FileDialog( FALSE, NULL, m_StrFilePath, OFN_OVERWRITEPROMPT, _T("fdx files(*.fdx)|*.fdx||"), this );

	// ѡ���ļ�
	if ( FileDialog.DoModal() == IDOK )
	{
		// ������Ϣ
		m_StrFilePath = FileDialog.GetPathName();

		// �ԱȺ�׺��
		CString StrFileExt = FileDialog.GetFileExt();
		if ( StrFileExt != _T("fdx") )
		{
			m_StrFilePath = m_StrFilePath.Left( m_StrFilePath.GetLength() - FileDialog.GetFileName().GetLength() );
			m_StrFilePath += FileDialog.GetFileTitle();
			m_StrFilePath += _T(".fdx");
		}
		
		// ������Ϣ
		for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
		{
			m_DialogCustom[nIndex]->UpdateData(TRUE);
		}

		// �жϹ�����Ϣ
		for ( int nIndex = 0; nIndex < Multiple_Max; ++nIndex )
		{
			if ( g_CustomRule.nMultipleValue[nIndex] <= 0 )
			{
				AfxMessageBox( _T("���ñ����������0��") );

				return;
			}
		}

		// ���ļ�
		FILE * pFile = NULL;
		fopen_s(&pFile, CT2A(m_StrFilePath), "wb");
		if ( pFile != NULL )
		{
			// д���ļ�
			try
			{
				fwrite(&g_CustomRule, sizeof(g_CustomRule), 1, pFile );
			}
			catch (...)
			{
				
			}

			// �ر��ļ�
			fflush(pFile);
			fclose(pFile);
		}
	}
}
