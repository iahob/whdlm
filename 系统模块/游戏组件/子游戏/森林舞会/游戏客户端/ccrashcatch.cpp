#include "Stdafx.h"
#include ".\ccrashcatch.h"


void GetCurModulePath(TCHAR* ptcPath,int iLen,LPVOID addr)
{
	MEMORY_BASIC_INFORMATION mbi = {0};
	if(FALSE == ::VirtualQuery( addr, &mbi, sizeof(mbi)))
		return;
	UINT_PTR h_module = (UINT_PTR)mbi.AllocationBase;
	::GetModuleFileName((HMODULE)h_module, ptcPath, iLen);
	return;
}

void EI2Str(TCHAR* ptcInfo,int iInfoLen,ULONG_PTR* Info,int iCount)
{
	int        i    = 0;
	TCHAR*    p1    = ptcInfo;
	for(i = 0; i < iCount; i++)
	{
		_sntprintf(p1,1024,_T("%08X "),Info[i]);
		p1 += 9;
	}
}


void WriteFile(const TCHAR* szFileName,TCHAR* szBuffer,int nBufferSize)
{
	TCHAR szProgramPath[MAX_PATH] = {0};

	if(GetModuleFileName(NULL, szProgramPath, MAX_PATH))

	{
		LPTSTR lpSlash = _tcsrchr(szProgramPath, _T('\\'));
		if(lpSlash)

		{
			*(lpSlash + 1) = _T('\0');
		}
	}

	TCHAR szDumpFile[MAX_PATH] = {0};


	time_t tvNow = time(NULL);
	struct tm * pTm = localtime(&tvNow);

	_sntprintf(szDumpFile, sizeof(szDumpFile),_T("%s%s%d-%d-%d-%d %d.txt"), szProgramPath, szFileName,pTm->tm_year+1900,pTm->tm_mon+1,pTm->tm_mday,pTm->tm_hour,pTm->tm_min);

	HANDLE hDumpFile = CreateFile(szDumpFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
	if (NULL == hDumpFile)
	{
		return;
	}

	DWORD nBytes = 0;
	WriteFile(hDumpFile,szBuffer, nBufferSize ,&nBytes,NULL);
	CloseHandle(hDumpFile);
}
void WriteStackFile(struct _EXCEPTION_POINTERS* pei)
{
	STACKFRAME	sf;
	CONTEXT	context;
	TCHAR	tcPath[2048]	= {0};
	TCHAR	tcInfo[10240]	= {0};
	TCHAR	tcExcep[20480]	= {0};
	GetCurModulePath(tcPath,2048,pei->ExceptionRecord->ExceptionAddress);
	EI2Str(tcInfo,10240,pei->ExceptionRecord->ExceptionInformation,pei->ExceptionRecord->NumberParameters);
	_sntprintf(tcExcep,sizeof(tcExcep),_T("抱歉！程序遇到未知异常，请截屏给相关的客服人员，具体信息如下： 代码：0x%08x  标志：0x%08x  地址：0x%08x ")
		_T("参数：%d  %s 模块：%s 堆栈信息如下： \r\n"),
		pei->ExceptionRecord->ExceptionCode,pei->ExceptionRecord->ExceptionFlags,pei->ExceptionRecord->ExceptionAddress,
		pei->ExceptionRecord->NumberParameters,tcInfo,tcPath);

	memset(tcInfo,0,sizeof(tcInfo));
	memset( &sf, 0, sizeof(STACKFRAME));
	memcpy(&context,pei->ContextRecord,sizeof(CONTEXT));

	sf.AddrPC.Offset    = context.Eip;
	sf.AddrPC.Mode        = AddrModeFlat;
	sf.AddrStack.Offset    = context.Esp;
	sf.AddrStack.Mode    = AddrModeFlat;
	sf.AddrFrame.Offset    = context.Ebp;
	sf.AddrFrame.Mode    = AddrModeFlat;

	DWORD machineType    = IMAGE_FILE_MACHINE_I386;
	HANDLE hProcess        = GetCurrentProcess();
	HANDLE hThread        = GetCurrentThread();
	SymInitialize(hProcess,NULL,TRUE);

	for( ; ; )
	{
		if( !StackWalk(machineType, hProcess, hThread, &sf, &context, 0, SymFunctionTableAccess, SymGetModuleBase, 0 ) )
		{
			break;
		}

		if( sf.AddrFrame.Offset == 0 )
		{
			break;
		}
		BYTE symbolBuffer[ sizeof( SYMBOL_INFO ) + 1024 ];
		PSYMBOL_INFO pSymbol = ( PSYMBOL_INFO ) symbolBuffer;

		pSymbol->SizeOfStruct = sizeof( symbolBuffer );
		pSymbol->MaxNameLen = 1024;

		_sntprintf(tcPath,sizeof(tcPath),_T("sf.AddrPC.Offset：0x%08X "),sf.AddrPC.Offset);
		_tcscat(tcExcep,tcPath);

		memset(tcPath,0,sizeof(tcPath));
		DWORD64 symDisplacement = 0;
		if( SymFromAddr( hProcess, sf.AddrPC.Offset, 0, pSymbol ) )
		{
			_sntprintf(tcPath,sizeof(tcPath),_T("Function : %s \r\n"), pSymbol->Name );
		}
		else
		{

		}

		_tcscat(tcExcep,tcPath);

		IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
		DWORD dwLineDisplacement;

		memset(tcPath,0,sizeof(tcPath));
		if( SymGetLineFromAddr( hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo ) )
		{
			_sntprintf(tcPath,sizeof(tcPath),_T( "[Source File : %s] [Source Line : %u] \r\n"), lineInfo.FileName,lineInfo.LineNumber ); 
		}
		else
		{
		}
		_tcscat(tcExcep,tcPath);
	}

	SymCleanup(hProcess);

	WriteFile((const TCHAR*)_T("stackinfo"),tcExcep,sizeof(tcExcep));
}

void WriteDumpFile(struct _EXCEPTION_POINTERS* lpExceptionInfo)
{
	TCHAR szProgramPath[MAX_PATH] = {0};

	if(GetModuleFileName(NULL, szProgramPath, MAX_PATH))
	{
		LPTSTR lpSlash = _tcsrchr(szProgramPath, _T('\\'));
		if(lpSlash)
		{
			*(lpSlash + 1) = _T('\0');
		}
	}

	TCHAR szDumpFile[MAX_PATH] = {0};
	time_t tvNow = time(NULL);
	struct tm * pTm = localtime(&tvNow);
	TCHAR szReportFile[MAX_PATH] = {0};

	_sntprintf(szDumpFile, sizeof(szDumpFile),_T("%s dumpinfo%d-%d-%d-%d %d.dmp"), szProgramPath, pTm->tm_year+1900,pTm->tm_mon+1,pTm->tm_mday,pTm->tm_hour,pTm->tm_min);
	_sntprintf(szReportFile, sizeof(szReportFile),_T("%sBugReport.exe"), szProgramPath);

	HANDLE hDumpFile = CreateFile(szDumpFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);

	MINIDUMP_EXCEPTION_INFORMATION stMiniDumpExceptionInfo;
	stMiniDumpExceptionInfo.ExceptionPointers = lpExceptionInfo;
	stMiniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
	stMiniDumpExceptionInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile,MiniDumpNormal, &stMiniDumpExceptionInfo, NULL, NULL);

	CloseHandle(hDumpFile);

	::ShellExecute(NULL, NULL, szReportFile, szDumpFile, NULL, SW_HIDE);
	//Use your own user-friendly ui prompt here
	MessageBox(NULL, _T("抱歉，程序崩溃，请将程序目录下的.dmp,和.log文件发给客服人员!我们将努力改进我们的程序，谢谢！"), _T("Oops"), MB_OK);
}

LONG WINAPI CleanToolExceptionFun(struct _EXCEPTION_POINTERS* pei)
{
	WriteDumpFile(pei);
	WriteStackFile(pei);
	return EXCEPTION_EXECUTE_HANDLER;//EXCEPTION_CONTINUE_SEARCH;
}