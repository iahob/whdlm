#include "Stdafx.h"
#include "AIDistribute.h"
#include "AIDistributeDlg.h"    

//////////////////////////////////////////////////////////////////////////////////

//程序对象
CAIDistributeApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAIDistributeApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAIDistributeApp::CAIDistributeApp()
{
}

//启动函数
BOOL CAIDistributeApp::InitInstance()
{
	__super::InitInstance();

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szProduct);

	//显示窗口
	CAIDistributeDlg AIDistributeDlg;
	m_pMainWnd = &AIDistributeDlg;
	AIDistributeDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
