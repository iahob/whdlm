#include "Stdafx.h"
#include "AccessServer.h"
#include "AccessServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CAccessServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAccessServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CAccessServerApp::CAccessServerApp()
{
}

//启动函数
BOOL CAccessServerApp::InitInstance()
{
	__super::InitInstance();

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szProduct);

	//显示窗口
	CAccessServerDlg AccessServerDlg;
	m_pMainWnd=&AccessServerDlg;
	AccessServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
