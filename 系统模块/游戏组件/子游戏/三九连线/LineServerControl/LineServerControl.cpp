// LineServerControl.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include <afxdllx.h>
#include "ServerControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE LineServerControlDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// 如果使用 lpReserved，请将此移除
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("LineServerControl.DLL 正在初始化！\n");
		if (!AfxInitExtensionModule(LineServerControlDLL, hInstance)) return 0;
		new CDynLinkLibrary(LineServerControlDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("LineServerControl.DLL 正在终止！\n");
		AfxTermExtensionModule(LineServerControlDLL);
	}
	return 1;   // 确定
}

// 建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateServerControl()
{
	// 建立对象
	try
	{
		return static_cast< IServerControl * >( CServerControl::_Object() );
	}
	catch (...) {}

	return NULL;
}