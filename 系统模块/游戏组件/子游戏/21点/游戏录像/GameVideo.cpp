// ServerDebug.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include <afxdllx.h>
#include "GameVideoItemSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE SanGongGameVideoDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// ���ʹ�� lpReserved���뽫���Ƴ�
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("SanGongGameVideo.DLL ���ڳ�ʼ����\n");
		if (!AfxInitExtensionModule(SanGongGameVideoDLL, hInstance)) return 0;
		new CDynLinkLibrary(SanGongGameVideoDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("SanGongGameVideo.DLL ������ֹ��\n");
		AfxTermExtensionModule(SanGongGameVideoDLL);
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