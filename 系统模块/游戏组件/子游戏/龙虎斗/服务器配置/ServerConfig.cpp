// ServerConfig.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ServerConfig.h"
#include "ServerConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CServerConfigApp theApp;

BEGIN_MESSAGE_MAP(CServerConfigApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CServerConfigApp::CServerConfigApp()
{
}

BOOL CServerConfigApp::InitInstance()
{
    InitCommonControls();
    CWinApp::InitInstance();
	AfxEnableControlContainer();
    SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
    CServerConfigDlg dlg;
    m_pMainWnd = &dlg;
    dlg.DoModal();
    return FALSE;
}
