// FishConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FishConfig.h"
#include "FishConfigDlg.h"
#include ".\fishconfigdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 消息注册
BEGIN_MESSAGE_MAP(CFishConfigDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_OPTISONS, OnTcnSelchangeTabOptisons)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

#include <math.h>
#define GL_PI ((FLOAT) 3.141592654f)
// 主对话框
CFishConfigDlg::CFishConfigDlg(CWnd* pParent /*=NULL*/) : CDialog(CFishConfigDlg::IDD, pParent)
{

}

// 控件绑定
void CFishConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

// 初始化
BOOL CFishConfigDlg::OnInitDialog()
{
	// 基类执行
	CDialog::OnInitDialog();

	// 获取分页
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_OPTISONS);
	
	// 设置选项
	pTabCtrl->InsertItem(CUSTOM_MULTIPLE, _T("倍数配置"));
	pTabCtrl->InsertItem(CUSTOM_STOCK_DEBUG, _T("调试配置"));
	pTabCtrl->InsertItem(CUSTOM_SCENES, _T("场景配置"));
	pTabCtrl->InsertItem(CUSTOM_ACTIVITY, _T("活动配置"));
	pTabCtrl->InsertItem(CUSTOM_FISH, _T("鱼配置"));
	pTabCtrl->InsertItem(CUSTOM_ZHONG_JIAN, _T("中奖消息配置"));
	// 	pTabCtrl->InsertItem(CUSTOM_YUAN_BAO, _T("元宝配置"));

	// 获取分页
	CRect RectWindow;
	CWnd * pWindowShow = GetDlgItem(IDC_STATIC_SHOW);
	pWindowShow->ShowWindow( SW_HIDE );
	pWindowShow->GetWindowRect(RectWindow);
	ScreenToClient(RectWindow);

	// 创建窗口

	// 倍数配置
	m_DialogCustom[CUSTOM_MULTIPLE] = new CDialogCustom1;
	m_DialogCustom[CUSTOM_MULTIPLE]->Create( IDD_DIALOG_1, this );

	// 调试配置
	m_DialogCustom[CUSTOM_STOCK_DEBUG] = new CDialogCustom2;
	m_DialogCustom[CUSTOM_STOCK_DEBUG]->Create( IDD_DIALOG_2, this );

	// 场景配置
	m_DialogCustom[CUSTOM_SCENES] = new CDialogCustom3;
	m_DialogCustom[CUSTOM_SCENES]->Create( IDD_DIALOG_3, this );
	CComboBox * pComboBox = (CComboBox *)m_DialogCustom[CUSTOM_SCENES]->GetDlgItem(IDC_COMBO1);
	pComboBox->InsertString( 0, _T("开炮数量") );
	pComboBox->InsertString( 1, _T("消耗金币") );
	pComboBox->SetCurSel( 0 );

	// 活动配置
	m_DialogCustom[CUSTOM_ACTIVITY] = new CDialogCustom4;
	m_DialogCustom[CUSTOM_ACTIVITY]->Create( IDD_DIALOG_4, this );

	// 鱼配置
	m_DialogCustom[CUSTOM_FISH] = new CDialogCustom5;
	m_DialogCustom[CUSTOM_FISH]->Create( IDD_DIALOG_5, this );

	// 鱼配置
	m_DialogCustom[CUSTOM_ZHONG_JIAN] = new CDialogCustom6;
	m_DialogCustom[CUSTOM_ZHONG_JIAN]->Create( IDD_DIALOG_6, this );
// 	// 元宝
// 	m_DialogCustom[CUSTOM_YUAN_BAO] = new CDialogCustomYuanBao;
// 	m_DialogCustom[CUSTOM_YUAN_BAO]->Create( IDD_DIALOG_YUANBAO, this );
// 	CComboBox * pGoldComboBox = (CComboBox *)m_DialogCustom[CUSTOM_YUAN_BAO]->GetDlgItem(IDC_GOLD_COMBO);
// 	pGoldComboBox->InsertString( 0, _T("开炮数量") );
// 	pGoldComboBox->InsertString( 1, _T("消耗金币") );
// 	pGoldComboBox->SetCurSel( 0 );
	// 窗口位置
	for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		CRect RectCustom;
		CSize SizeCustom;
		m_DialogCustom[nIndex]->GetClientRect( RectCustom );
		SizeCustom.SetSize( min(RectWindow.Width(), RectCustom.Width()), min(RectWindow.Height(), RectCustom.Height()) );
		m_DialogCustom[nIndex]->SetWindowPos( NULL, RectWindow.left + RectWindow.Width() / 2 - SizeCustom.cx / 2, RectWindow.top + RectWindow.Height() / 2 - SizeCustom.cy / 2, SizeCustom.cx, SizeCustom.cy, SWP_NOZORDER );
		m_DialogCustom[nIndex]->ShowWindow( ( nIndex == 0 ) ? SW_SHOW : SW_HIDE );

		//// 设置信息
		//CWnd* pWndChild = m_DialogCustom[nIndex]->GetWindow(GW_CHILD);    
		//while (pWndChild)
		//{       
		//	// 获取窗口类名
		//	tchar szClassName[256] = { _T("") };
		//	GetClassName( pWndChild->GetSafeHwnd(), szClassName, 256);

		//	// 判断窗口未无效
		//	if ( !pWndChild->IsWindowEnabled() && (_tcsicmp( szClassName, _T("STATIC") ) == 0 || _tcsicmp( szClassName, _T("BUTTON")) == 0 ) )
		//	{
		//		pWndChild->SetWindowText( _T("暂未开放") );
		//	}

		//	// 下一个窗口
		//	pWndChild = pWndChild->GetNextWindow();
		//} 
	}

	return TRUE; 
}

// 变换选项
void CFishConfigDlg::OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 获取分页
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_OPTISONS);

	// 获取选择项目
	int nCurSel = pTabCtrl->GetCurSel();
	if( nCurSel >= 0 && nCurSel < MAX_CUSTOM )
	{
		// 判断窗口
		for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
		{
			m_DialogCustom[nIndex]->ShowWindow( ( nIndex == nCurSel ) ? SW_SHOW : SW_HIDE );
		}
	}

	*pResult = 0;
}

// 窗口销毁
void CFishConfigDlg::OnDestroy()
{
	// 销毁窗口
	for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		m_DialogCustom[nIndex]->DestroyWindow();
		delete m_DialogCustom[nIndex];
	}

	CDialog::OnDestroy();
}

// 默认配置
void CFishConfigDlg::OnBnClickedButton3()
{
	// 默认配置
	g_CustomRule.DefaultCustomRule();

	// 更新界面
	for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		m_DialogCustom[nIndex]->UpdateData(FALSE);
	}
}

// 打开配置
void CFishConfigDlg::OnBnClickedButton2()
{
	// 打开文件
	CFileDialog FileDialog( TRUE, NULL, m_StrFilePath, OFN_READONLY, _T("fdx files(*.fdx)|*.fdx||"), this );

	// 选择文件
	if ( FileDialog.DoModal() == IDOK )
	{
		// 更新信息
		m_StrFilePath = FileDialog.GetPathName();

		// 打开文件
		FILE * pFile = NULL;
		fopen_s(&pFile, CT2A(m_StrFilePath), "rb");
		if ( pFile != NULL )
		{
			// 读取文件
			try
			{
				fread(&g_CustomRule, sizeof(g_CustomRule), 1, pFile );
			}
			catch (...)
			{
				g_CustomRule.DefaultCustomRule();
			}

			// 关闭文件
			fclose(pFile);

			// 更新界面
			for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
			{
				m_DialogCustom[nIndex]->UpdateData(FALSE);
			}
		}
	}
}

// 保存配置
void CFishConfigDlg::OnBnClickedButton1()
{
	// 打开文件
	CFileDialog FileDialog( FALSE, NULL, m_StrFilePath, OFN_OVERWRITEPROMPT, _T("fdx files(*.fdx)|*.fdx||"), this );

	// 选择文件
	if ( FileDialog.DoModal() == IDOK )
	{
		// 更新信息
		m_StrFilePath = FileDialog.GetPathName();

		// 对比后缀名
		CString StrFileExt = FileDialog.GetFileExt();
		if ( StrFileExt != _T("fdx") )
		{
			m_StrFilePath = m_StrFilePath.Left( m_StrFilePath.GetLength() - FileDialog.GetFileName().GetLength() );
			m_StrFilePath += FileDialog.GetFileTitle();
			m_StrFilePath += _T(".fdx");
		}
		
		// 更新信息
		for ( int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
		{
			m_DialogCustom[nIndex]->UpdateData(TRUE);
		}

		// 判断规则信息
		for ( int nIndex = 0; nIndex < Multiple_Max; ++nIndex )
		{
			if ( g_CustomRule.nMultipleValue[nIndex] <= 0 )
			{
				AfxMessageBox( _T("设置倍数必须大于0！") );

				return;
			}
		}

		// 打开文件
		FILE * pFile = NULL;
		fopen_s(&pFile, CT2A(m_StrFilePath), "wb");
		if ( pFile != NULL )
		{
			// 写入文件
			try
			{
				fwrite(&g_CustomRule, sizeof(g_CustomRule), 1, pFile );
			}
			catch (...)
			{
				
			}

			// 关闭文件
			fflush(pFile);
			fclose(pFile);
		}
	}
}
