#pragma once
#include <Dbghelp.h>
#pragma comment( lib, "DbgHelp" )

// ����Dump�ļ�
// 
void WHCreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	// ����Dump�ļ�
	//
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// Dump��Ϣ
	//
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	// д��Dump�ļ�����
	//
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);
}

LONG WHCrashHandler(EXCEPTION_POINTERS *pException)
{

	//����·��
	TCHAR szIniFile[MAX_PATH] = TEXT("");
	GetCurrentDirectoryW(MAX_PATH, szIniFile);
	_sntprintf_s(szIniFile, CountArray(szIniFile), TEXT("%s\\GameServer.dmp"), szIniFile);

	WHCreateDumpFile(szIniFile, pException);

	return EXCEPTION_EXECUTE_HANDLER;
}





