// LineClientControl.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include <afxdllx.h>
#include "Resource.h"
#include "DialogControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE LineClientControlDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// 如果使用 lpReserved，请将此移除
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("LineClientControl.DLL 正在初始化！\n");
		if (!AfxInitExtensionModule(LineClientControlDLL, hInstance)) return 0;
		new CDynLinkLibrary(LineClientControlDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("LineClientControl.DLL 正在终止！\n");
		AfxTermExtensionModule(LineClientControlDLL);
	}
	return 1;   // 确定
}

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateClientControl()
{
	//建立对象
	CDialogControl * pServerControl = NULL;
	try
	{
		pServerControl = new CDialogControl();
		if ( pServerControl == NULL ) 
			throw TEXT("创建失败");

		return static_cast< IClientControl * >( pServerControl );
	}
	catch (...) {}

	//清理对象
	SafeDelete(pServerControl);

	return NULL;
}