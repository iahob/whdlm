#include <windows.h>
#include "Log.h"
#include <locale.h>



//��ʼ����־״̬
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
	sprintf_s(sInfo, "open *** ��־��Ϣ %s ", sLog);

	sprintf_s(sInfo, "%d-%d-%d-%d: %s \r\n", sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sLog);
	int nStatus;

	//����log�ļ���
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

	//��׷�ӵķ�ʽ��log�ļ����µ�ServerLog.txt �ļ���������ļ��������򴴽�
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

	//����д����־���ݴ�С
	TCHAR szInfo[2062] = { 0 };
	ZeroMemory(szInfo, sizeof(szInfo));

	//������־·��
	if (szServerName != NULL)
	{
		wsprintf(szPath, TEXT("Log//��Ϸ�������쳣//%d-%d-%d-%d-%s-%d-Log.txt"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, szServerName, wServerID);
	}
	else
	{
		wsprintf(szPath, TEXT("Log//��Ϸ�������쳣//%d-%d-%d-%d-%s-%d-Log.txt"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, TEXT("ServerID="), wServerID);
	}

	//�����ļ�ָ��
	FILE *fp = NULL;

	int nErr = _wfopen_s(&fp, szPath, TEXT("at+"));

	//����ļ�û�д�����������·������
	if (nErr != 0)
	{
		//����log�ļ���
		int nStatus = _mkdir("Log");
		wsprintf(szPath, TEXT("Log//%s"), TEXT("��Ϸ�������쳣"));
		nStatus = _wmkdir(szPath);

		//����·�������ļ�
		if (szServerName != NULL)
		{
			wsprintf(szPath, TEXT("Log//��Ϸ�������쳣//%d-%d-%d-%d-%s-%d-Log.txt"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, szServerName, wServerID);
		}
		else
		{
			wsprintf(szPath, TEXT("Log//��Ϸ�������쳣//%d-%d-%d-%d-%s-%d-Log.txt"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, TEXT("ServerID="), wServerID);
		}

		nErr = _wfopen_s(&fp, szPath, TEXT("at+"));
	}



	//��ʽ��д�������
	_wsetlocale(LC_ALL, L"chs");//��ؼ��Ϸ������ɵ��ı����ʺ��ַ���

	//д���ļ�
	if (nErr == 0)
	{

		fwprintf_s(fp, TEXT("[%02d-%02d-%02d-%02d-%02d-%02d]: %ls\r\n"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, szLogContent);

		fclose(fp);
		fp = NULL;
	}
}