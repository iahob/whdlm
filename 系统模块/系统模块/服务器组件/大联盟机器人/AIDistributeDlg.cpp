//藏宝库IT社区（28xin.com）
#include "Stdafx.h"
#include "Resource.h"
#include "AIDistributeDlg.h"
#include "ControlPacket.h"
//////////////////////////////////////////////////////////////////////////////////  
#include <imagehlp.h>
#pragma comment(lib, "DbgHelp.lib")
//消息定义
#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//处理命令

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAIDistributeDlg, CDialog)

	//系统消息
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	//自定消息
	ON_MESSAGE(WM_PROCESS_CMD_LINE,OnMessageProcessCmdLine)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////
//抓捕异常信息
LONG ExceptionCrashHandler(EXCEPTION_POINTERS *pException)
{
	// 创建Dump文件
	HANDLE hDumpFile = CreateFileW(L"AIDistributeException.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// Dump信息
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;
	// 写入Dump文件内容
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
	return EXCEPTION_EXECUTE_HANDLER;
}

//构造函数
CAIDistributeDlg::CAIDistributeDlg() : CDialog(IDD_DLG_AI_DISTRIBUTE_SERVER)
{
	//配置参数
	m_bAutoControl=false;
	m_bOptionSuccess=false;
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//析构函数
CAIDistributeDlg::~CAIDistributeDlg()
{
}

//控件绑定
VOID CAIDistributeDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}

//初始化函数
BOOL CAIDistributeDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("AI分配服务器 -- [ 停止 ]"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//设置组件
	m_ServiceUnits.SetServiceUnitsSink(this);

	//命令处理
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0) PostMessage(WM_PROCESS_CMD_LINE,0,(LPARAM)pszCmdLine);

	//抓捕异常文件
	::SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ExceptionCrashHandler);  //cash代码

	return TRUE;
}

//确定消息
VOID CAIDistributeDlg::OnOK()
{
	return;
}

//取消函数
VOID CAIDistributeDlg::OnCancel()
{

	//停止服务
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//消息解释
BOOL CAIDistributeDlg::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//服务状态
VOID CAIDistributeDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//状态设置
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//停止状态
		{
			//更新标题
			UpdateServerTitle(ServiceStatus);

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
			//更新标题
			UpdateServerTitle(ServiceStatus);

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
			//更新标题
			UpdateServerTitle(ServiceStatus);

			//服务按钮
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

//更新图标
VOID CAIDistributeDlg::UpdateServerLogo(LPCTSTR pszServerDLL)
{

	return;
}

//更新标题
VOID CAIDistributeDlg::UpdateServerTitle(enServiceStatus ServiceStatus)
{
	//变量定义
	LPCTSTR pszStatusName=NULL;
	LPCTSTR pszServerName=NULL;

	//状态字符
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//停止状态
		{
			pszStatusName=TEXT("停止");
			break;
		}
	case ServiceStatus_Service:	//运行状态
		{
			pszStatusName=TEXT("运行");
			break;
		}
	}

	//房间名字
	if (m_bOptionSuccess==false) pszServerName=TEXT("AI分配服务器");
	else pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;

	//构造标题
	TCHAR szTitle[128]=TEXT("");
#ifndef _DEBUG
	_sntprintf_s(szTitle,CountArray(szTitle),TEXT("[ %s ] -- [ %s ] -- [ %s ]"),m_ModuleInitParameter.GameServiceAttrib.szGameName,pszServerName,pszStatusName);
#else
	_sntprintf_s(szTitle,CountArray(szTitle),TEXT("[ %s ] -- [ %s ]"),pszServerName,pszStatusName);
#endif

	//设置标题
	SetWindowText(szTitle);

	return;
}

//更新状态
VOID CAIDistributeDlg::UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter)
{
	
	return;
}

//启动房间
bool CAIDistributeDlg::StartServerService(WORD wServerID)
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

	return true;
}

//获取连接
bool CAIDistributeDlg::GetPlatformDBParameter(tagDataBaseParameter & DataBaseParameter)
{
	//获取路径
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//构造路径
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("%s\\ServerParameter.ini"),szWorkDir);

	//读取配置
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//连接信息
	DataBaseParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("PlatformDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBAddr"),NULL,DataBaseParameter.szDataBaseAddr,CountArray(DataBaseParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBUser"),NULL,DataBaseParameter.szDataBaseUser,CountArray(DataBaseParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBPass"),NULL,DataBaseParameter.szDataBasePass,CountArray(DataBaseParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBName"),szPlatformDB,DataBaseParameter.szDataBaseName,CountArray(DataBaseParameter.szDataBaseName));

	return true;
}

//启动服务
VOID CAIDistributeDlg::OnBnClickedStartService()
{
	//启动服务
	try
	{
		if (m_ServiceUnits.StartService())
		{
			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			UpdateServerTitle(ServiceStatus_Service);
		}
	}
	catch (...)
	{
		//ASSERT(FALSE);
	}

	return;
}

//停止服务
VOID CAIDistributeDlg::OnBnClickedStopService()
{
	//停止服务
	try
	{
		m_ServiceUnits.SendControlPacket(UI_CLEAR_AI, NULL, 0);
		//if (m_ServiceUnits.ConcludeService())
		//{
		//	//设置按钮
		//	GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		//	GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
		//	UpdateServerTitle(ServiceStatus_Stop);
		//}
	}
	catch (...)
	{
		//ASSERT(FALSE);
	}

	return;
}



//命令处理
LRESULT CAIDistributeDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	CWHCommandLine CommandLine;
	LPCTSTR pszCommandLine=(LPCTSTR)(lParam);

	//房间标识
	TCHAR szSrverID[32]=TEXT("");
	if (CommandLine.SearchCommandItem(pszCommandLine,TEXT("/ServerID:"),szSrverID,CountArray(szSrverID))==true)
	{
		//获取房间
		WORD wServerID=(WORD)(_tstol(szSrverID));

		//启动房间
		if (wServerID!=0)
		{
			//设置变量
			m_bAutoControl=true;

			//启动房间
			StartServerService(wServerID);
		}
	}

	return 0L;
}

//////////////////////////////////////////////////////////////////////////////////
