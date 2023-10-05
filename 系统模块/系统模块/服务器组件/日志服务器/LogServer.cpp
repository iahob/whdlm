//藏宝库IT社区（28xin.com）
#include "Stdafx.h"
#include "LogServer.h"
#include "LogServerDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//程序对象
CLogServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLogServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CLogServerApp::CLogServerApp()
{
}

//启动函数
BOOL CLogServerApp::InitInstance()
{
	__super::InitInstance();

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szProduct);

	//显示窗口
	CLogServerDlg CorrespondServerDlg;
	m_pMainWnd=&CorrespondServerDlg;
	CorrespondServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
