// ClientControl.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include <afxdllx.h>
#include "ClientControlItemSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE ClientDebugDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    // ���ʹ�� lpReserved���뽫���Ƴ�
    UNREFERENCED_PARAMETER(lpReserved);

    if(dwReason == DLL_PROCESS_ATTACH)
    {
        TRACE0("ClientControl.DLL ���ڳ�ʼ����\n");
        if(!AfxInitExtensionModule(ClientDebugDLL, hInstance))
        {
            return 0;
        }
        new CDynLinkLibrary(ClientDebugDLL);

    }
    else if(dwReason == DLL_PROCESS_DETACH)
    {
        TRACE0("ClientControl.DLL ������ֹ��\n");
        // �ڵ�����������֮ǰ��ֹ�ÿ�
        AfxTermExtensionModule(ClientDebugDLL);
    }
    return 1;   // ȷ��
}

//����������
extern "C" __declspec(dllexport) void *__cdecl CreateClientDebug(CWnd *pParentWnd)
{
    //��������
    CClientControlItemSinkDlg *pClientDebug = NULL;
    try
    {
		pClientDebug = new CClientControlItemSinkDlg(/*pParentWnd*/);
        if(pClientDebug == NULL)
        {
            throw TEXT("����ʧ��");
        }

        return static_cast<IClientDebug *>(pClientDebug);
    }
    catch(...) {}

    //�������
    SafeDelete(pClientDebug);
    return NULL;
}