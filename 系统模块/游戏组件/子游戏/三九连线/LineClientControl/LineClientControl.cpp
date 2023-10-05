// LineClientControl.cpp : ���� DLL �ĳ�ʼ�����̡�
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
	// ���ʹ�� lpReserved���뽫���Ƴ�
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("LineClientControl.DLL ���ڳ�ʼ����\n");
		if (!AfxInitExtensionModule(LineClientControlDLL, hInstance)) return 0;
		new CDynLinkLibrary(LineClientControlDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("LineClientControl.DLL ������ֹ��\n");
		AfxTermExtensionModule(LineClientControlDLL);
	}
	return 1;   // ȷ��
}

//����������
extern "C" __declspec(dllexport) void * __cdecl CreateClientControl()
{
	//��������
	CDialogControl * pServerControl = NULL;
	try
	{
		pServerControl = new CDialogControl();
		if ( pServerControl == NULL ) 
			throw TEXT("����ʧ��");

		return static_cast< IClientControl * >( pServerControl );
	}
	catch (...) {}

	//�������
	SafeDelete(pServerControl);

	return NULL;
}