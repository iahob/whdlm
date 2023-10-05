#include <windows.h>
#include "Stdafx.h"




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
	_vsnprintf_s(sLog, 1024, pszFormat, args);
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
	}
	else
	{
		sprintf_s(szPath, "Log//%d-%d-%d-%d-%s-%d-ServerLog.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, szServerName, wServerID);
	}

	//��׷�ӵķ�ʽ��log�ļ����µ�ServerLog.txt �ļ���������ļ��������򴴽�
	FILE *fp;
	fp = NULL;
	fopen_s(&fp,szPath, "at+");
	if (fp)
	{
		fwrite(sInfo, strlen(sInfo), 1, fp);
		fclose(fp);
		DebugPrintf("openfile ���ļ��ɹ�");
	}
	else
	{
		DebugPrintf("openfile ���ļ�����");
	}

}