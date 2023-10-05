// LineServerControl.cpp : ���� DLL �ĳ�ʼ�����̡�
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
	// ���ʹ�� lpReserved���뽫���Ƴ�
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("LineServerControl.DLL ���ڳ�ʼ����\n");
		if (!AfxInitExtensionModule(LineServerControlDLL, hInstance)) return 0;
		new CDynLinkLibrary(LineServerControlDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("LineServerControl.DLL ������ֹ��\n");
		AfxTermExtensionModule(LineServerControlDLL);
	}
	return 1;   // ȷ��
}

// ����������
extern "C" __declspec(dllexport) void * __cdecl CreateServerControl()
{
	// ��������
	try
	{
		return static_cast< IServerControl * >( CServerControl::_Object() );
	}
	catch (...) {}

	return NULL;
}