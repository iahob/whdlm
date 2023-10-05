// ClientDebug.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include <afxdllx.h>
#include "ClientDebugItemSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE ClientDebugDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// ���ʹ�� lpReserved���뽫���Ƴ�
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("ClientDebug.DLL ���ڳ�ʼ����\n");
		if (!AfxInitExtensionModule(ClientDebugDLL, hInstance))return 0;
		new CDynLinkLibrary(ClientDebugDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("ClientDebug.DLL ������ֹ��\n");
		// �ڵ�����������֮ǰ��ֹ�ÿ�
		AfxTermExtensionModule(ClientDebugDLL);
	}
	return 1;   // ȷ��
}

//����������
extern "C" __declspec(dllexport) void * __cdecl CreateClientDebug(CWnd *pParentWnd )
{
	//��������
	CClientDebugItemSinkDlg * pServerDebug = NULL;
	try
	{
		pServerDebug = new CClientDebugItemSinkDlg(/*pParentWnd*/);
		if (pServerDebug == NULL) 
			throw TEXT("����ʧ��");
		
		return static_cast< IClientDebug * >(pServerDebug);
	}
	catch (...) {}

	//�������
	SafeDelete(pServerDebug);
	return NULL;
}