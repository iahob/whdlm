//�ر���IT������28xin.com��
#include "Stdafx.h"
#include "Resource.h"
#include "LogServerDlg.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLogServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_BUTTON_LOGON_POINT, OnBnClickedLogonPoint)
	ON_BN_CLICKED(IDC_BUTTON_LOGON_IMMEDIATE, OnBnClickedLogonImmediate)
	ON_BN_CLICKED(IDC_BUTTON_GAME_SERVER_POINT, OnBnClickedGameServerPoint)
	ON_BN_CLICKED(IDC_BUTTON_GAME_SERVER_IMMEDIATE, OnBnClickedGameServerImmediate)
	ON_BN_CLICKED(IDC_BUTTON_CHAT_POINT, OnBnClickedChatPoint)
	ON_BN_CLICKED(IDC_BUTTON_CHAT_IMMEDIATE, OnBnClickedChatImmediate)
	ON_BN_CLICKED(IDC_BUTTON_GROUP_POINT, OnBnClickedGroupPoint)
	ON_BN_CLICKED(IDC_BUTTON_GROUP_IMEDITE, OnBnClickedGroupImmediate)

	ON_BN_CLICKED(IDC_BUTTON_LOGON_CLOSE, OnBnClickedLogonClose)
	ON_BN_CLICKED(IDC_BUTTON_GAME_SERVER_CLOSE, OnBnClickedGameServerClose)
	ON_BN_CLICKED(IDC_BUTTON_CHAT_CLOSE, OnBnClickedChatClose)
	ON_BN_CLICKED(IDC_BUTTON_GROUP_CLOSE, OnBnClickedGroupClose)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CLogServerDlg::CLogServerDlg() : CDialog(IDD_DLG_CORRESPOND_SERVER)
{
	m_nCloseLogonServerLog = 0;		//�رյ�¼��������־
	m_nCloseGameServerLog = 0;		//�ر���Ϸ��������־
	m_nCloseChatServerLog = 0;		//�ر������������־
	m_nCloseGroupServerLog = 0;		//�رվ��ֲ���������־
}

//��������
CLogServerDlg::~CLogServerDlg()
{
}

//�ؼ���
VOID CLogServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}

//��ʼ������
BOOL CLogServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("��־������ -- [ ֹͣ ]"));

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//�������
	m_ServiceUnits.SetServiceUnitsSink(this);

	return TRUE;
}

//ȷ����Ϣ
VOID CLogServerDlg::OnOK()
{
	return;
}

//ȡ������
VOID CLogServerDlg::OnCancel()
{
	//�ر�ѯ��
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		LPCTSTR pszQuestion=TEXT("��־���������������У���ȷʵҪ�رշ�������");
		if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
	}

	//ֹͣ����
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//��Ϣ����
BOOL CLogServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����״̬
VOID CLogServerDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//״̬����
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//ֹͣ״̬
		{
			//���ñ���
			SetWindowText(TEXT("��־������ -- [ ֹͣ ]"));

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("����ֹͣ�ɹ�");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Config:	//����״̬
		{
			//���ñ���
			SetWindowText(TEXT("��־������ -- [ ��ʼ�� ]"));

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("���ڳ�ʼ�����...");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Service:	//����״̬
		{
			//���ñ���
			SetWindowText(TEXT("��־������ -- [ ���� ]"));

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("���������ɹ�");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	}

	return;
}

//��������
VOID CLogServerDlg::OnBnClickedStartService()
{
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
VOID CLogServerDlg::OnBnClickedStopService()
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

//�ر�ѯ��
BOOL CLogServerDlg::OnQueryEndSession()
{
	//��ʾ��Ϣ
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		CTraceService::TraceString(TEXT("�������������У�ϵͳҪ��ע���ػ�����ʧ��"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}

//��¼��������ʱд��־
VOID CLogServerDlg::OnBnClickedLogonPoint()
{
	////�û��������ť
	//GetDlgItem(IDC_BUTTON_LOGON_POINT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_LOGON_IMMEDIATE)->EnableWindow(TRUE);

	//m_ServiceUnits.SetLogControl(LOGON_SERVER, POINT_WRITE);
}

//��¼��������ʱд��־
VOID CLogServerDlg::OnBnClickedLogonImmediate()
{
	////�û��������ť
	GetDlgItem(IDC_BUTTON_LOGON_IMMEDIATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_LOGON_CLOSE)->EnableWindow(TRUE);

	m_ServiceUnits.SetLogControl(LOGON_SERVER, OPEN_LOG_EX);
}

//��Ϸ��������ʱд��־
VOID CLogServerDlg::OnBnClickedGameServerPoint()
{
	////�û��������ť
	//GetDlgItem(IDC_BUTTON_GAME_SERVER_POINT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_GAME_SERVER_IMMEDIATE)->EnableWindow(TRUE);

	//m_ServiceUnits.SetLogControl(GAME_SERVER, POINT_WRITE);
}

//��Ϸ��������ʱд��־
VOID CLogServerDlg::OnBnClickedGameServerImmediate()
{
	////�û��������ť
	GetDlgItem(IDC_BUTTON_GAME_SERVER_IMMEDIATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GAME_SERVER_CLOSE)->EnableWindow(TRUE);

	m_ServiceUnits.SetLogControl(GAME_SERVER, OPEN_LOG_EX);
}

//�����������ʱд��־
VOID CLogServerDlg::OnBnClickedChatPoint()
{
	////�û��������ť
	//GetDlgItem(IDC_BUTTON_CHAT_POINT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_CHAT_IMMEDIATE)->EnableWindow(TRUE);

	//m_ServiceUnits.SetLogControl(CHAT_SERVER, POINT_WRITE);
}

//�����������ʱд��־
VOID CLogServerDlg::OnBnClickedChatImmediate()
{
	////�û��������ť
	GetDlgItem(IDC_BUTTON_CHAT_IMMEDIATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CHAT_CLOSE)->EnableWindow(TRUE);

	m_ServiceUnits.SetLogControl(CHAT_SERVER, OPEN_LOG_EX);
}

//���ֲ���ʱд��־
VOID CLogServerDlg::OnBnClickedGroupPoint()
{
	////�û��������ť
	//GetDlgItem(IDC_BUTTON_GROUP_POINT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_GROUP_IMEDITE)->EnableWindow(TRUE);

	//m_ServiceUnits.SetLogControl(GROUP_SERVER, POINT_WRITE);
}

//���ֲ���ʱд��־
VOID CLogServerDlg::OnBnClickedGroupImmediate()
{
	////�û��������ť
	GetDlgItem(IDC_BUTTON_GROUP_IMEDITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GROUP_CLOSE)->EnableWindow(TRUE);

	m_ServiceUnits.SetLogControl(GROUP_SERVER, OPEN_LOG_EX);
}


//�رյ�¼��������־
VOID CLogServerDlg::OnBnClickedLogonClose()
{
	GetDlgItem(IDC_BUTTON_LOGON_IMMEDIATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_LOGON_CLOSE)->EnableWindow(FALSE);

	m_ServiceUnits.SetLogControl(LOGON_SERVER, CLOSE_LOG_EX);
}

//�ر���Ϸ��������־
VOID CLogServerDlg::OnBnClickedGameServerClose()
{
	GetDlgItem(IDC_BUTTON_GAME_SERVER_IMMEDIATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_GAME_SERVER_CLOSE)->EnableWindow(FALSE);

	m_ServiceUnits.SetLogControl(GAME_SERVER, CLOSE_LOG_EX);

}

//�ر������������־
VOID CLogServerDlg::OnBnClickedChatClose()
{
	GetDlgItem(IDC_BUTTON_CHAT_IMMEDIATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CHAT_CLOSE)->EnableWindow(FALSE);

	m_ServiceUnits.SetLogControl(CHAT_SERVER, CLOSE_LOG_EX);

}

//�رվ��ֲ���������־
VOID CLogServerDlg::OnBnClickedGroupClose()
{
	GetDlgItem(IDC_BUTTON_GROUP_IMEDITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_GROUP_CLOSE)->EnableWindow(FALSE);

	m_ServiceUnits.SetLogControl(GROUP_SERVER, CLOSE_LOG_EX);

}
//////////////////////////////////////////////////////////////////////////////////
