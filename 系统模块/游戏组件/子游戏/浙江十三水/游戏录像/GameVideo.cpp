// ServerControl.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include <afxdllx.h>
#include "GameVideoItemSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE ThirteenZJGameVideoDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// ���ʹ�� lpReserved���뽫���Ƴ�
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("ThirteenZJGameVideo.DLL ���ڳ�ʼ����\n");
		if (!AfxInitExtensionModule(ThirteenZJGameVideoDLL, hInstance)) return 0;
		new CDynLinkLibrary(ThirteenZJGameVideoDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("ThirteenZJGameVideo.DLL ������ֹ��\n");
		AfxTermExtensionModule(ThirteenZJGameVideoDLL);
	}
	return 1;   // ȷ��
}

//����������
extern "C" __declspec(dllexport) void * __cdecl CreateGameVideo()
{
	//��������
	CGameVideoItemSink * pGameVideo = NULL;
	try
	{
		pGameVideo = new CGameVideoItemSink();
		if (pGameVideo == NULL) 
			throw TEXT("����ʧ��");

		return pGameVideo;
	}
	catch (...) {}

	//�������
	SafeDelete(pGameVideo);
	return NULL;
}