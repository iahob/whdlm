#include "Stdafx.h"
#include "AIDistribute.h"
#include "AIDistributeDlg.h"    

//////////////////////////////////////////////////////////////////////////////////

//�������
CAIDistributeApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAIDistributeApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAIDistributeApp::CAIDistributeApp()
{
}

//��������
BOOL CAIDistributeApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//����ע���
	SetRegistryKey(szProduct);

	//��ʾ����
	CAIDistributeDlg AIDistributeDlg;
	m_pMainWnd = &AIDistributeDlg;
	AIDistributeDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
