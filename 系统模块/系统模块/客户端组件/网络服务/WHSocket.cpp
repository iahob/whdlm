//����ӣ���ݡ��Ŀ�,Ⱥ:16886186����ϵQQ:412069412
#include "Stdafx.h"
#include "AfxDllx.h"
#include "WHSocketHead.h"

//////////////////////////////////////////////////////////////////////////////////

//��̬����
static AFX_EXTENSION_MODULE WHSocketDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////////////

//DLL ������
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(WHSocketDLL, hInstance)) return 0;
		new CDynLinkLibrary(WHSocketDLL);

		//��������
		WSADATA WSAData;
		if(WSAStartup(MAKEWORD(2,2),&WSAData)!=0) return 0;
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(WHSocketDLL);

		//��������
		WSACleanup();
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////