//藏宝库IT社区（28xin.com）
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

//构造函数
CLogServerDlg::CLogServerDlg() : CDialog(IDD_DLG_CORRESPOND_SERVER)
{
	m_nCloseLogonServerLog = 0;		//关闭登录服务器日志
	m_nCloseGameServerLog = 0;		//关闭游戏服务器日志
	m_nCloseChatServerLog = 0;		//关闭聊天服务器日志
	m_nCloseGroupServerLog = 0;		//关闭俱乐部服务器日志
}

//析构函数
CLogServerDlg::~CLogServerDlg()
{
}

//控件绑定
VOID CLogServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}

//初始化函数
BOOL CLogServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("日志服务器 -- [ 停止 ]"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//设置组件
	m_ServiceUnits.SetServiceUnitsSink(this);

	return TRUE;
}

//确定消息
VOID CLogServerDlg::OnOK()
{
	return;
}

//取消函数
VOID CLogServerDlg::OnCancel()
{
	//关闭询问
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		LPCTSTR pszQuestion=TEXT("日志服务器正在运行中，您确实要关闭服务器吗？");
		if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
	}

	//停止服务
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//消息解释
BOOL CLogServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//服务状态
VOID CLogServerDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//状态设置
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//停止状态
		{
			//设置标题
			SetWindowText(TEXT("日志服务器 -- [ 停止 ]"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

			//提示信息
			LPCTSTR pszDescribe=TEXT("服务停止成功");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Config:	//配置状态
		{
			//设置标题
			SetWindowText(TEXT("日志服务器 -- [ 初始化 ]"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//提示信息
			LPCTSTR pszDescribe=TEXT("正在初始化组件...");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Service:	//服务状态
		{
			//设置标题
			SetWindowText(TEXT("日志服务器 -- [ 运行 ]"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//提示信息
			LPCTSTR pszDescribe=TEXT("服务启动成功");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	}

	return;
}

//启动服务
VOID CLogServerDlg::OnBnClickedStartService()
{
	//启动服务
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

//停止服务
VOID CLogServerDlg::OnBnClickedStopService()
{
	//停止服务
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

//关闭询问
BOOL CLogServerDlg::OnQueryEndSession()
{
	//提示消息
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		CTraceService::TraceString(TEXT("服务正在运行中，系统要求注销回话请求失败"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}

//登录服务器定时写日志
VOID CLogServerDlg::OnBnClickedLogonPoint()
{
	////置灰与点亮按钮
	//GetDlgItem(IDC_BUTTON_LOGON_POINT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_LOGON_IMMEDIATE)->EnableWindow(TRUE);

	//m_ServiceUnits.SetLogControl(LOGON_SERVER, POINT_WRITE);
}

//登录服务器即时写日志
VOID CLogServerDlg::OnBnClickedLogonImmediate()
{
	////置灰与点亮按钮
	GetDlgItem(IDC_BUTTON_LOGON_IMMEDIATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_LOGON_CLOSE)->EnableWindow(TRUE);

	m_ServiceUnits.SetLogControl(LOGON_SERVER, OPEN_LOG_EX);
}

//游戏服务器定时写日志
VOID CLogServerDlg::OnBnClickedGameServerPoint()
{
	////置灰与点亮按钮
	//GetDlgItem(IDC_BUTTON_GAME_SERVER_POINT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_GAME_SERVER_IMMEDIATE)->EnableWindow(TRUE);

	//m_ServiceUnits.SetLogControl(GAME_SERVER, POINT_WRITE);
}

//游戏服务器即时写日志
VOID CLogServerDlg::OnBnClickedGameServerImmediate()
{
	////置灰与点亮按钮
	GetDlgItem(IDC_BUTTON_GAME_SERVER_IMMEDIATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GAME_SERVER_CLOSE)->EnableWindow(TRUE);

	m_ServiceUnits.SetLogControl(GAME_SERVER, OPEN_LOG_EX);
}

//聊天服务器定时写日志
VOID CLogServerDlg::OnBnClickedChatPoint()
{
	////置灰与点亮按钮
	//GetDlgItem(IDC_BUTTON_CHAT_POINT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_CHAT_IMMEDIATE)->EnableWindow(TRUE);

	//m_ServiceUnits.SetLogControl(CHAT_SERVER, POINT_WRITE);
}

//聊天服务器即时写日志
VOID CLogServerDlg::OnBnClickedChatImmediate()
{
	////置灰与点亮按钮
	GetDlgItem(IDC_BUTTON_CHAT_IMMEDIATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CHAT_CLOSE)->EnableWindow(TRUE);

	m_ServiceUnits.SetLogControl(CHAT_SERVER, OPEN_LOG_EX);
}

//俱乐部定时写日志
VOID CLogServerDlg::OnBnClickedGroupPoint()
{
	////置灰与点亮按钮
	//GetDlgItem(IDC_BUTTON_GROUP_POINT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_GROUP_IMEDITE)->EnableWindow(TRUE);

	//m_ServiceUnits.SetLogControl(GROUP_SERVER, POINT_WRITE);
}

//俱乐部即时写日志
VOID CLogServerDlg::OnBnClickedGroupImmediate()
{
	////置灰与点亮按钮
	GetDlgItem(IDC_BUTTON_GROUP_IMEDITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GROUP_CLOSE)->EnableWindow(TRUE);

	m_ServiceUnits.SetLogControl(GROUP_SERVER, OPEN_LOG_EX);
}


//关闭登录服务器日志
VOID CLogServerDlg::OnBnClickedLogonClose()
{
	GetDlgItem(IDC_BUTTON_LOGON_IMMEDIATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_LOGON_CLOSE)->EnableWindow(FALSE);

	m_ServiceUnits.SetLogControl(LOGON_SERVER, CLOSE_LOG_EX);
}

//关闭游戏服务器日志
VOID CLogServerDlg::OnBnClickedGameServerClose()
{
	GetDlgItem(IDC_BUTTON_GAME_SERVER_IMMEDIATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_GAME_SERVER_CLOSE)->EnableWindow(FALSE);

	m_ServiceUnits.SetLogControl(GAME_SERVER, CLOSE_LOG_EX);

}

//关闭聊天服务器日志
VOID CLogServerDlg::OnBnClickedChatClose()
{
	GetDlgItem(IDC_BUTTON_CHAT_IMMEDIATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CHAT_CLOSE)->EnableWindow(FALSE);

	m_ServiceUnits.SetLogControl(CHAT_SERVER, CLOSE_LOG_EX);

}

//关闭俱乐部服务器日志
VOID CLogServerDlg::OnBnClickedGroupClose()
{
	GetDlgItem(IDC_BUTTON_GROUP_IMEDITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_GROUP_CLOSE)->EnableWindow(FALSE);

	m_ServiceUnits.SetLogControl(GROUP_SERVER, CLOSE_LOG_EX);

}
//////////////////////////////////////////////////////////////////////////////////
