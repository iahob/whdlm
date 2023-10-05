// ClientControl.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include <afxdllx.h>
#include "AIConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE AIConfigDebugDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    // ���ʹ�� lpReserved���뽫���Ƴ�
    UNREFERENCED_PARAMETER(lpReserved);

    if(dwReason == DLL_PROCESS_ATTACH)
    {
        TRACE0("AIConfigDebug.DLL ���ڳ�ʼ����\n");
        if(!AfxInitExtensionModule(AIConfigDebugDLL, hInstance))
        {
            return 0;
        }
        new CDynLinkLibrary(AIConfigDebugDLL);

    }
    else if(dwReason == DLL_PROCESS_DETACH)
    {
        TRACE0("AIConfigDebug.DLL ������ֹ��\n");
        // �ڵ�����������֮ǰ��ֹ�ÿ�
        AfxTermExtensionModule(AIConfigDebugDLL);
    }
    return 1;   // ȷ��
}

//����������
extern "C" __declspec(dllexport) void *__cdecl CreateAIConfigDebug(CWnd *pParentWnd)
{
	//��������
	CAIConfigDlg *pServerDebug = NULL;
	try
	{
		pServerDebug = new CAIConfigDlg();
		if (pServerDebug == NULL)
		{
			throw TEXT("����ʧ��");
		}

		return static_cast< IAIConfigDebug * >(pServerDebug);
	}
	catch (...) {}

	//�������
	SafeDelete(pServerDebug);

	return NULL;
}