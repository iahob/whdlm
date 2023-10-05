#ifndef LOG_HEAD_DEF
#define LOG_HEAD_DEF
#pragma once
#include "stdio.h"
#include <stdarg.h>    ////用变参函数所必须的 
#include <time.h>
#include <direct.h> 
#include <mmsystem.h>
#include <tchar.h>
#pragma comment(lib, "winmm.lib")


const int CHAR_LEN = 2050;
const int WCHAR_LEN = 1025;

static char g_sResult[CHAR_LEN] = { 0 };
static wchar_t g_wsResult[WCHAR_LEN] = { 0 };

// 将宽字节wchar_t* 转换 单字节char*
static char* UnicodeToAnsi(const wchar_t* szStr)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}

	WideCharToMultiByte(CP_ACP, 0, szStr, -1, g_sResult, nLen, NULL, NULL);
	return g_sResult;
}

//将单字节char* 转换 宽字节wchar_t*
static wchar_t* AnsiToUnicode(const char* szStr)
{
	wchar_t* pWCHAR = NULL;

	//计算pChar所指向的多字节字符串相当于多少个宽字节  
	DWORD dwLen = MultiByteToWideChar(CP_ACP, 0, szStr, -1, NULL, 0);

	if (dwLen >= WCHAR_LEN)
	{
		dwLen = WCHAR_LEN - 1;
	}

	memset(g_wsResult, 0, sizeof(g_wsResult));

	//多字节转换为宽字节  
	MultiByteToWideChar(CP_ACP, 0, szStr, -1, g_wsResult, dwLen);

	return g_wsResult;
}

// 将单字节char* 转换 宽字节wchar_t*
static void AnsiToUnicode(const char* szStr, wchar_t *szDes)
{
	if (NULL == szDes) return;

	//计算pChar所指向的多字节字符串相当于多少个宽字节  
	DWORD dwLen = MultiByteToWideChar(CP_ACP, 0, szStr, -1, NULL, 0);

	//多字节转换为宽字节  
	MultiByteToWideChar(CP_ACP, 0, szStr, -1, szDes, dwLen);
}


static bool TraceDebug(TCHAR* lpszFormat, ...)
{
	if (lpszFormat == NULL)return false;


	va_list args;
	TCHAR sTemp[512] = { 0 };


	va_start(args, lpszFormat);
	_vsntprintf(sTemp, ARRAYSIZE(sTemp), lpszFormat, args);
	va_end(args);

	TCHAR sTrace[1024] = { 0 };

	_sntprintf_s(sTrace, ARRAYSIZE(sTrace), TEXT(" %s\r\n"), sTemp);
	//OutputDebugString(sTrace);

	return true;

}


class WHLogControl
{
public:
	static byte m_cbLogStatus;


	//静态变量
protected:
	static WHLogControl *			m_pLogControl;						//对象指针
	 
public:
	WHLogControl();
	~WHLogControl();
public:
	//打开与关闭日志
	static void SetLogStatus(byte cbStatus);

	//获取日志
	static byte GetLogStatus();

	//写日志
	static void LogPrintf(WORD wServerID, char * szServerName, const char* pszFormat, ...);
	//写日志
	static void LogPrintf(WORD wServerID, TCHAR * szServerName, const TCHAR* pszFormat, ...);

public:
	WHLogControl * GetInstance()
	{
		return m_pLogControl;
	}
	
};

#endif





