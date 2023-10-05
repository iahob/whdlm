#include "Stdafx.h"
#include "Resource.h"
#include "InitParameter.h"
#include "AccessServerDlg.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAccessServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()	
	ON_BN_CLICKED(IDC_LOAD_OPTION, OnBnClickedLoadOption)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CAccessServerDlg::CAccessServerDlg() : CDialog(IDD_DLG_ACCESS_SERVER)
{
}

//��������
CAccessServerDlg::~CAccessServerDlg()
{
}

//�ؼ���
VOID CAccessServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_RichEditTrace);
}

//��ʼ������
BOOL CAccessServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("��������� -- [ ֹͣ ]"));

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	return TRUE;
}

//ȷ����Ϣ
VOID CAccessServerDlg::OnOK()
{
	return;
}

//ȡ������
VOID CAccessServerDlg::OnCancel()
{
	//�ر�ѯ��
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		LPCTSTR pszQuestion=TEXT("������������������У���ȷʵҪ�رշ�������");
		if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
	}

	//ֹͣ����
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//��Ϣ����
BOOL CAccessServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����״̬
VOID CAccessServerDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//״̬����
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//ֹͣ״̬
		{
			//���ñ���
			SetWindowStatusText(TEXT("ֹͣ"));

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_LOAD_OPTION)->EnableWindow(TRUE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("����ֹͣ�ɹ�");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Config:	//����״̬
		{
			//���ñ���
			SetWindowStatusText(TEXT("��ʼ��"));

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_LOAD_OPTION)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("���ڳ�ʼ�����...");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Service:	//����״̬
		{
			//���ñ���
			SetWindowStatusText(TEXT("����"));

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_LOAD_OPTION)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("���������ɹ�");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	}

	return;
}

//��������
VOID CAccessServerDlg::OnBnClickedStartService()
{
	//���ýӿ�
	if (m_ServiceUnits.SetServiceUnitsSink(this)==false)
	{
		CTraceService::TraceString(TEXT("�󶨷������״̬�ӿ�ʧ��"),TraceLevel_Exception);
		return;
	}

	//��������
	try
	{
		m_ServiceUnits.StartService();
	}
	catch (...)
	{
		//ASSERT(FALSE);
	}

	return;
}

//ֹͣ����
VOID CAccessServerDlg::OnBnClickedStopService()
{

	//ֹͣ����
	try
	{
		m_ServiceUnits.ConcludeService();
	}
	catch (...)
	{
		//ASSERT(FALSE);
	}

	return;
}

//��������
VOID CAccessServerDlg::OnBnClickedLoadOption()
{

}

//���ñ���
VOID CAccessServerDlg::SetWindowStatusText(LPCTSTR pszServiceStatus)
{	
	//��������
	TCHAR szWindowText[128]=TEXT("");
	_snwprintf_s(szWindowText, CountArray(szWindowText), TEXT("���������[ %s ]"), pszServiceStatus);

	//���ñ���
	SetWindowText(szWindowText);

	return;
}

//�ر�ѯ��
BOOL CAccessServerDlg::OnQueryEndSession()
{
	//��ʾ��Ϣ
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		CTraceService::TraceString(TEXT("�������������У�ϵͳҪ��ע���ػ�����ʧ��"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
