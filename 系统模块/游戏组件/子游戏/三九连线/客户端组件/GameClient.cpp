#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
#include ".\gameclient.h"

//////////////////////////////////////////////////////////////////////////////////

// 程序对象
CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////////////

// 构造函数
CGameClientApp::CGameClientApp()
{
}

// 析构函数
CGameClientApp::~CGameClientApp()
{
}

// 配置函数
BOOL CGameClientApp::InitInstance()
{
	return CGameFrameApp::InitInstance();;
}

// 退出函数
BOOL CGameClientApp::ExitInstance()
{
	return CGameFrameApp::ExitInstance();
}

// 创建引擎
CGameFrameEngine * CGameClientApp::GetGameFrameEngine(DWORD dwSDKVersion)
{
	// 版本检测
	if (InterfaceVersionCompare(VERSION_FRAME_SDK,dwSDKVersion)==false)
	{
		ASSERT(FALSE);
		return NULL;
	}

	// 创建引擎
	return new CGameClientEngine;
}

// 创建框架
CGameFrameWnd * CGameClientApp::GetGameFrameWnd()
{
	return (CGameFrameWnd *)new CGameFrameWindow;
}


// 消息过滤
BOOL CGameClientApp::PreTranslateMessage(MSG* pMsg)
{
	// 虚拟框架
	if ( pMsg->hwnd == dex::CDirectUIEngine::_Object()->GetD3DDevice()->GetWndDevice()
		&& dex::CDirectUIEngine::_Object()->PreTranslateMessage(pMsg->message,pMsg->wParam,pMsg->lParam) )
		return TRUE; 

	return CGameFrameApp::PreTranslateMessage(pMsg);
}
