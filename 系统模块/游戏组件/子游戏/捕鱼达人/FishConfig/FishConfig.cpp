// FishConfig.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "FishConfig.h"
#include "FishConfigDlg.h"
#include ".\fishconfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFishConfigApp theApp;

BEGIN_MESSAGE_MAP(CFishConfigApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CFishConfigApp::CFishConfigApp()
{
}

BOOL CFishConfigApp::InitInstance()
{
	InitCommonControls();
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	CFishConfigDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
