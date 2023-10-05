#ifndef LOG_HEAD_DEF
#define LOG_HEAD_DEF
#pragma once
#include "stdio.h"
#include <stdarg.h>    ////�ñ�κ���������� 
#include <time.h>
#include <direct.h> 
#include <mmsystem.h>
#include <tchar.h>
#pragma comment(lib, "winmm.lib")


const int CHAR_LEN = 2050;
const int WCHAR_LEN = 1025;

static char g_sResult[CHAR_LEN] = { 0 };
static wchar_t g_wsResult[WCHAR_LEN] = { 0 };

// �����ֽ�wchar_t* ת�� ���ֽ�char*
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

//�����ֽ�char* ת�� ���ֽ�wchar_t*
static wchar_t* AnsiToUnicode(const char* szStr)
{
	wchar_t* pWCHAR = NULL;

	//����pChar��ָ��Ķ��ֽ��ַ����൱�ڶ��ٸ����ֽ�  
	DWORD dwLen = MultiByteToWideChar(CP_ACP, 0, szStr, -1, NULL, 0);

	if (dwLen >= WCHAR_LEN)
	{
		dwLen = WCHAR_LEN - 1;
	}

	memset(g_wsResult, 0, sizeof(g_wsResult));

	//���ֽ�ת��Ϊ���ֽ�  
	MultiByteToWideChar(CP_ACP, 0, szStr, -1, g_wsResult, dwLen);

	return g_wsResult;
}

// �����ֽ�char* ת�� ���ֽ�wchar_t*
static void AnsiToUnicode(const char* szStr, wchar_t *szDes)
{
	if (NULL == szDes) return;

	//����pChar��ָ��Ķ��ֽ��ַ����൱�ڶ��ٸ����ֽ�  
	DWORD dwLen = MultiByteToWideChar(CP_ACP, 0, szStr, -1, NULL, 0);

	//���ֽ�ת��Ϊ���ֽ�  
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


	//��̬����
protected:
	static WHLogControl *			m_pLogControl;						//����ָ��
	 
public:
	WHLogControl();
	~WHLogControl();
public:
	//����ر���־
	static void SetLogStatus(byte cbStatus);

	//��ȡ��־
	static byte GetLogStatus();

	//д��־
	static void LogPrintf(WORD wServerID, char * szServerName, const char* pszFormat, ...);
	//д��־
	static void LogPrintf(WORD wServerID, TCHAR * szServerName, const TCHAR* pszFormat, ...);

public:
	WHLogControl * GetInstance()
	{
		return m_pLogControl;
	}
	
};

#endif





