#include "Stdafx.h"
#include "AccessServer.h"
#include "AccessServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//�������
CAccessServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAccessServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CAccessServerApp::CAccessServerApp()
{
}

//��������
BOOL CAccessServerApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//����ע���
	SetRegistryKey(szProduct);

	//��ʾ����
	CAccessServerDlg AccessServerDlg;
	m_pMainWnd=&AccessServerDlg;
	AccessServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
