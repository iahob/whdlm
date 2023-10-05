// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// GameServer.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"
int RandInt(int nMin,int nMax)
{
	int nDelta = nMax - nMin;
	if (nDelta <=0)
	{
		return nMax;
	}

	int nRand1 = ::rand();
	int nRand2 = ::rand();
	return ((nRand1<<15) + nRand2)%(nDelta+1) + nMin;
}

const char*  GetDllPath(void)
{
#ifdef WIN32
	static char szPath[MAX_PATH];
	static bool bFirstTime = true;

	if(bFirstTime)
	{
		bFirstTime = false;
		GetModuleFileNameA(NULL, szPath, sizeof(szPath));
		char *p = strrchr(szPath, ('\\'));
		*p = ('\0');
		strcat_s(szPath, ("\\DLL\\"));
	}

	return szPath;
#else
	return "./";
#endif
}

const char* GetRootPath(void)
{
#ifdef WIN32
	static char szPath[MAX_PATH];
	static bool bFirstTime = true;

	if(bFirstTime)
	{
		bFirstTime = false;
		GetModuleFileNameA(NULL, szPath, sizeof(szPath));
		char *p = strrchr(szPath, ('\\'));
		*p = '\0';
	}

	return szPath;
#else
	return ("./");
#endif
}
