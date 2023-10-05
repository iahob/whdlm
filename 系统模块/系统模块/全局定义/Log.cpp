#include <windows.h>
#include "Log.h"
#include <locale.h>



//初始化日志状态
byte WHLogControl::m_cbLogStatus = 0;

WHLogControl::WHLogControl()
{

}

WHLogControl::~WHLogControl()
{

}

void WHLogControl::SetLogStatus(byte cbStatus)
{
	m_cbLogStatus = cbStatus;
}

byte WHLogControl::GetLogStatus()
{
	return m_cbLogStatus;
}


void WHLogControl::LogPrintf(WORD wServerID, char * szServerName, const char* pszFormat, ...)
{
	if (m_cbLogStatus == 0)
	{
		return;
	}

	char sLog[1024] = { 0 };
	va_list args;
	va_start(args, pszFormat);
	_vsnprintf(sLog, 1024, pszFormat, args);
	va_end(args);



	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	char sInfo[1024] = { 0 };
	sprintf_s(sInfo, "open *** 日志信息 %s ", sLog);

	sprintf_s(sInfo, "%d-%d-%d-%d: %s \r\n", sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sLog);
	int nStatus;

	//创建log文件夹
	nStatus = _mkdir("Log");

	char szPath[MAX_PATH] = { 0 };
	if (szServerName == NULL)
	{
		return;
		sprintf_s(szPath, "Log//%d-%d-%d-%d-%s-%d-ServerLog.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, "ServerID =", wServerID);
	}
	else
	{
		sprintf_s(szPath, "Log//%d-%d-%d-%d-%s-%d-ServerLog.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, szServerName, wServerID);
	}

	//以追加的方式打开log文件夹下的ServerLog.txt 文件，如果此文件不存在则创建
	FILE *fp;
	fp = NULL;
	fp = fopen(szPath, "at+");
	if (fp)
	{
		fwrite(sInfo, strlen(sInfo), 1, fp);
		fclose(fp);
	}
	else
	{
	}

}

void WHLogControl::LogPrintf(WORD wServerID, TCHAR * szServerName, const TCHAR* pszFormat, ...)
{
	if (pszFormat == NULL)return ;


	va_list args;
	TCHAR szLogContent[1024] = { 0 };

	va_start(args, pszFormat);
	_vsntprintf(szLogContent, ARRAYSIZE(szLogContent), pszFormat, args);
	va_end(args);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	TCHAR szPath[MAX_PATH] = { 0 };

	//定义写入日志内容大小
	TCHAR szInfo[2062] = { 0 };
	ZeroMemory(szInfo, sizeof(szInfo));

	//设置日志路径
	if (szServerName != NULL)
	{
		wsprintf(szPath, TEXT("Log//游戏服务器异常//%d-%d-%d-%d-%s-%d-Log.txt"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, szServerName, wServerID);
	}
	else
	{
		wsprintf(szPath, TEXT("Log//游戏服务器异常//%d-%d-%d-%d-%s-%d-Log.txt"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, TEXT("ServerID="), wServerID);
	}

	//定义文件指针
	FILE *fp = NULL;

	int nErr = _wfopen_s(&fp, szPath, TEXT("at+"));

	//如果文件没有打开则重新生成路径并打开
	if (nErr != 0)
	{
		//创建log文件夹
		int nStatus = _mkdir("Log");
		wsprintf(szPath, TEXT("Log//%s"), TEXT("游戏服务器异常"));
		nStatus = _wmkdir(szPath);

		//设置路径并打开文件
		if (szServerName != NULL)
		{
			wsprintf(szPath, TEXT("Log//游戏服务器异常//%d-%d-%d-%d-%s-%d-Log.txt"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, szServerName, wServerID);
		}
		else
		{
			wsprintf(szPath, TEXT("Log//游戏服务器异常//%d-%d-%d-%d-%s-%d-Log.txt"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, TEXT("ServerID="), wServerID);
		}

		nErr = _wfopen_s(&fp, szPath, TEXT("at+"));
	}



	//格式化写入的语言
	_wsetlocale(LC_ALL, L"chs");//务必加上否则生成的文本是问号字符串

	//写入文件
	if (nErr == 0)
	{

		fwprintf_s(fp, TEXT("[%02d-%02d-%02d-%02d-%02d-%02d]: %ls\r\n"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, szLogContent);

		fclose(fp);
		fp = NULL;
	}
}