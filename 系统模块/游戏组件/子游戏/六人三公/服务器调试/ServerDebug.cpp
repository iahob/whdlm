// ServerDebug.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include <afxdllx.h>
#include "ServerDebugItemSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE ServerDebugDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// ���ʹ�� lpReserved���뽫���Ƴ�
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("ServerDebug.DLL ���ڳ�ʼ����\n");
		if (!AfxInitExtensionModule(ServerDebugDLL, hInstance)) return 0;
		new CDynLinkLibrary(ServerDebugDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("ServerDebug.DLL ������ֹ��\n");
		AfxTermExtensionModule(ServerDebugDLL);
	}
	return 1;   // ȷ��
}

//����������
extern "C" __declspec(dllexport) void * __cdecl CreateServerDebug()
{
	//��������
	CServerDebugItemSink * pServerDebug = NULL;
	try
	{
		pServerDebug = new CServerDebugItemSink();
		if (pServerDebug == NULL) 
			throw TEXT("����ʧ��");

		return pServerDebug;
	}
	catch (...) {}

	//�������
	SafeDelete(pServerDebug);
	return NULL;
}