
#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"
#include <direct.h> 
#include <locale.h>
#include <list>
using namespace std;


////////////////////////////////////////////////////
/*************** ��¼��������־���鶨�� ***************/
vector<tagOtherServerLogInfo> g_vecOtherServerLogFirst;  //��־��������1
vector<tagOtherServerLogInfo> g_vecOtherServerLogSecond; //��־��������2
vector<tagOtherServerLogInfo> g_vecOtherServerLogThird;  //��־��������3

//������������֮��һ���Խ����������е�����д���ı��ĵ�
int g_nFirstOtherSWriteCount = 0;				//��־��������Ԫ������1
int g_nSecondOtherSWriteCount = 0;				//��־��������Ԫ������2
int g_nThirdOtherSWriteCount = 0;				//��־��������Ԫ������3

int g_cbOtherWriteControl = 0;					//��־����д�����
const int MAX_OTHER_SERVER_LIST_COUNT = 10000;	//��־��������Ԫ��������

////////////////////////////////////////////////////
/*************** ��Ϸ��������־���鶨�� ***************/
vector<tagLogUserInfo> g_vecGameServerLogFirst;		//��־��������1
vector<tagLogUserInfo> g_vecGameServerLogSecond;	//��־��������2
vector<tagLogUserInfo> g_vecGameServerLogThird;		//��־��������3

//������������֮��һ���Խ����������е�����д���ı��ĵ�
int g_nFirstGSWriteCount = 0;	//��־��������Ԫ������1
int g_nSecondGSWriteCount = 0;  //��־��������Ԫ������2
int g_nThirdGSWriteCount = 0;	//��־��������Ԫ������3

int g_cbLogGSWriteControl = 0;	//��־����д�����
const int MAX_LIST_COUNT = 10000;
////////////////////////////////////////////////////


byte	g_cbCloseLogonServerLog;			//�رյ�¼��������־	0 �ر�		1 ����
byte	g_cbCloseGameServerLog;				//�ر���Ϸ��������־	0 �ر�		1 ����
byte	g_cbCloseChatServerLog;				//�ر������������־	0 �ر�		1 ����
byte	g_cbCloseGroupServerLog;			//�رս����������־	0 �ر�		1 ����



//////////////////////////////////////////////////////////////////////////////////
/******************************* ��¼��������־���� *******************************/
void WriteOtherServerLog(vector<tagOtherServerLogInfo> & lstLogUserInfo, int & nCount)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	TCHAR szPath[MAX_PATH] = { 0 };

	//����д����־���ݴ�С
	TCHAR szInfo[2062] = { 0 };
	ZeroMemory(szInfo, sizeof(szInfo));

	//ͳ��д����־����
	int nStatic = 0;

	//д������־
	int nLogStatic = 0;
	
	//�����ļ�ָ��
	FILE *fp;
	fp = NULL;

	//������־����
	tagOtherServerLogInfo LogUserInfo;
	
	//ѭ��д����־
	while (nLogStatic < lstLogUserInfo.size())
	{
		//������־·��
		wsprintf(szPath, TEXT("Log//%s//%d-%d-%d-%d-%sLog.txt"), LogUserInfo.szServerName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, LogUserInfo.szServerName);

		int nErr = _wfopen_s(&fp, szPath, TEXT("at+"));

		//����ļ�û�д�����������·������
		if (nErr != 0)
		{
			//����log�ļ���
			int nStatus = _mkdir("Log");
			wsprintf(szPath, TEXT("Log//%s"), LogUserInfo.szServerName);
			nStatus = _wmkdir(szPath);

			//����·�������ļ�
			wsprintf(szPath, TEXT("Log//%s//%d-%d-%d-%d-%sLog.txt"), LogUserInfo.szServerName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, LogUserInfo.szServerName);
			nErr = _wfopen_s(&fp, szPath, TEXT("at+"));
		}


		//��ȡ��־�б�����ǰ��һ����־����д��
		LogUserInfo = lstLogUserInfo[nLogStatic];
		
		//��ʽ��д�������
		_wsetlocale(LC_ALL, L"chs");//��ؼ��Ϸ������ɵ��ı����ʺ��ַ���

		//д���ļ�
		if (nErr == 0)
		{
	
			fwprintf_s(fp, TEXT("[%02d-%02d-%02d-%02d-%02d-%02d]: %ls\r\n"), LogUserInfo.sysTime.wYear, LogUserInfo.sysTime.wMonth, LogUserInfo.sysTime.wDay, LogUserInfo.sysTime.wHour, LogUserInfo.sysTime.wMinute, LogUserInfo.sysTime.wSecond, LogUserInfo.szLogContent);
		
			ZeroMemory(&lstLogUserInfo[nLogStatic], sizeof(tagOtherServerLogInfo));
			nLogStatic++;
			nCount--;
			if (nCount < 0)
			{
				nCount = 0;
			}
			fclose(fp);
			fp = NULL;
		}
	}
}


//������Ϸ��������־
DWORD WINAPI OtherServerThreadProFunc(LPVOID lpParam)
{
	SYSTEMTIME sysTime;
	TCHAR szInfo[260] = { 0 };
	GetLocalTime(&sysTime);

	if (g_cbOtherWriteControl == 1)
	{
		WriteOtherServerLog(g_vecOtherServerLogFirst, g_nFirstOtherSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** ��־���� %d װ���� д����� %02d:%02d:%02d"), g_cbOtherWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}
	else if (g_cbOtherWriteControl == 2)
	{
		WriteOtherServerLog(g_vecOtherServerLogSecond, g_nSecondOtherSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** ��־���� %d װ���� д����� %02d:%02d:%02d"), g_cbOtherWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}
	else if (g_cbOtherWriteControl == 0)
	{
		WriteOtherServerLog(g_vecOtherServerLogThird, g_nThirdOtherSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** ��־���� %d װ���� д����� %02d:%02d:%02d"), g_cbOtherWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}

	return 0;

}

//����־����������־��������
void AddOtherServerLog(vector<tagOtherServerLogInfo> & lstLogUserInfo, tagOtherServerLogInfo LogUserInfo, int nWriteControl, int & nWriteCount)
{
	//�����־��������δ������룬������������¸�ֵ
	if (lstLogUserInfo.size() <  MAX_LIST_COUNT)
	{
		lstLogUserInfo.push_back(LogUserInfo);
		nWriteCount = lstLogUserInfo.size();
	}
	else
	{
		if (nWriteCount < MAX_LIST_COUNT)
		{
			memcpy_s(&lstLogUserInfo[nWriteCount], sizeof(tagOtherServerLogInfo), &LogUserInfo, sizeof(tagOtherServerLogInfo));
			nWriteCount++;
		}

	}

	if (nWriteCount >= MAX_LIST_COUNT)
	{
		g_cbOtherWriteControl = nWriteControl;

		HANDLE hThread = NULL;
		DWORD dwThreadId = 0;

		hThread = CreateThread(NULL		// Ĭ�ϰ�ȫ����
			, NULL						// Ĭ�϶�ջ��С
			, OtherServerThreadProFunc	// �߳���ڵ�ַ
			, NULL						//���ݸ��̺߳����Ĳ���
			, 0							// ָ���߳���������
			, &dwThreadId				//�߳�ID��
			);

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtlog *** ��־���� %d װ���� ��ʼд�� %02d:%02d:%02d"), nWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}
}



//////////////////////////////////////////////////////////////////////////////////
/******************************* ��Ϸ��������־���� *******************************/
void WriteGameServerLog(vector<tagLogUserInfo> & lstLogUserInfo, int & nCount)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	TCHAR szPath[MAX_PATH] = { 0 };

	//����д����־���ݴ�С
	TCHAR szInfo[2062] = { 0 };
	ZeroMemory(szInfo, sizeof(szInfo));

	//ͳ��д����־����
	int nStatic = 0;

	//д������־
	int nLogStatic = 0;

	tagLogUserInfo LogUserInfo;
	//����ļ��򲻿��򷵻�
	FILE *fp;
	fp = NULL;
	while (nLogStatic < lstLogUserInfo.size())
	{

		//��ȡ��־�б�����ǰ��һ����־����д��
		LogUserInfo = lstLogUserInfo[nLogStatic];
		wsprintf(szPath, TEXT("Log//GameServer//%s//%s-[%d]//%d-%d-%d-%d-ServerLog.txt"), LogUserInfo.szGameName, LogUserInfo.szServerName, LogUserInfo.wServerID, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour);

		_wsetlocale(LC_ALL, L"chs");



		int nErr = _wfopen_s(&fp, szPath, TEXT("at+"));

		if (nErr != 0)
		{
			//����log�ļ���
			int nStatus = _mkdir("Log");
			nStatus = _mkdir("Log//GameServer");
			wsprintf(szPath, TEXT("Log//GameServer//%s"), LogUserInfo.szGameName);
			nStatus = _wmkdir(szPath);

			wsprintf(szPath, TEXT("Log//GameServer//%s//%s-[%d]"), LogUserInfo.szGameName, LogUserInfo.szServerName, LogUserInfo.wServerID);
			nStatus = _wmkdir(szPath);
			wsprintf(szPath, TEXT("Log//GameServer//%s//%s-[%d]//%s"), LogUserInfo.szGameName, LogUserInfo.szServerName, LogUserInfo.wServerID, LogUserInfo.szDescName);
			nStatus = _wmkdir(szPath);
			wsprintf(szPath, TEXT("Log//GameServer//%s//%s-[%d]//%d-%d-%d-%d-ServerLog.txt"), LogUserInfo.szGameName, LogUserInfo.szServerName, LogUserInfo.wServerID, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour);

			nErr = _wfopen_s(&fp, szPath, TEXT("at+"));


		}


		if (nErr == 0)
		{
			if (!LogUserInfo.dwUserID)
			{
				fwprintf_s(fp, TEXT("[%02d-%02d-%02d-%02d-%02d-%02d] %ls\r\n"), LogUserInfo.sysTime.wYear, LogUserInfo.sysTime.wMonth, LogUserInfo.sysTime.wDay, LogUserInfo.sysTime.wHour, LogUserInfo.sysTime.wMinute, LogUserInfo.sysTime.wSecond, LogUserInfo.szLogContent);
			}
			else
			{
				fwprintf_s(fp, TEXT("[%02d-%02d-%02d-%02d-%02d-%02d]��USERID=%d, CHAIRID=%d, TABLEID=%d, NICKNAME=%ls�� %ls\r\n"), LogUserInfo.sysTime.wYear, LogUserInfo.sysTime.wMonth, LogUserInfo.sysTime.wDay, LogUserInfo.sysTime.wHour, LogUserInfo.sysTime.wMinute, LogUserInfo.sysTime.wSecond, LogUserInfo.dwUserID, LogUserInfo.wChairID, LogUserInfo.wTableID, LogUserInfo.szNickName, LogUserInfo.szLogContent);
			}

			ZeroMemory(&lstLogUserInfo[nLogStatic], sizeof(tagLogUserInfo));
			nLogStatic++;
			nCount--;
			if (nCount < 0)
			{
				nCount = 0;
			}
			fclose(fp);
		}

		fp = NULL;
	}
}

//������Ϸ��������־
DWORD WINAPI GameServerThreadProFunc(LPVOID lpParam)
{
	SYSTEMTIME sysTime;
	TCHAR szInfo[260] = { 0 };
	GetLocalTime(&sysTime);

	if (g_cbLogGSWriteControl == 1)
	{
		WriteGameServerLog(g_vecGameServerLogFirst, g_nFirstGSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** ��־���� %d װ���� д����� %02d:%02d:%02d"), g_cbLogGSWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}
	else if (g_cbLogGSWriteControl == 2)
	{
		WriteGameServerLog(g_vecGameServerLogSecond, g_nSecondGSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** ��־���� %d װ���� д����� %02d:%02d:%02d"), g_cbLogGSWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}
	else if (g_cbLogGSWriteControl == 0)
	{
		WriteGameServerLog(g_vecGameServerLogThird, g_nThirdGSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** ��־���� %d װ���� д����� %02d:%02d:%02d"), g_cbLogGSWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}

	return 0;

}

//����־����������־��������
void AddGameServerLog(vector<tagLogUserInfo> & lstLogUserInfo, tagLogUserInfo LogUserInfo, int nWriteControl, int & nWriteCount)
{
	//�����־��������δ������룬������������¸�ֵ
	if (lstLogUserInfo.size() <  MAX_LIST_COUNT)
	{
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtlog *** AddGameServerLog 3  nWriteCount = %d"), nWriteCount);
		//OutputDebugString(szInfo);

		lstLogUserInfo.push_back(LogUserInfo);
		nWriteCount = lstLogUserInfo.size();

		wsprintf(szInfo, TEXT("ptdtlog *** AddGameServerLog 4  nWriteCount = %d"), nWriteCount);
		//OutputDebugString(szInfo);
	}
	else
	{
		//OutputDebugString(TEXT("ptdtlog *** AddGameServerLog 5"));
		if (nWriteCount < MAX_LIST_COUNT && nWriteCount < lstLogUserInfo.size())
		{
			//OutputDebugString(TEXT("ptdtlog *** AddGameServerLog 6"));
			memcpy_s(&lstLogUserInfo[nWriteCount], sizeof(tagLogUserInfo), &LogUserInfo, sizeof(tagLogUserInfo));
			//OutputDebugString(TEXT("ptdtlog *** AddGameServerLog 7"));
			nWriteCount++;
		}

	}

	if (nWriteCount >= MAX_LIST_COUNT )
	{
		//OutputDebugString(TEXT("ptdtlog *** AddGameServerLog 8"));

		g_cbLogGSWriteControl = nWriteControl;

		HANDLE hThread = NULL;
		DWORD dwThreadId = 0;

		hThread = CreateThread(NULL		// Ĭ�ϰ�ȫ����
			, NULL						// Ĭ�϶�ջ��С
			, GameServerThreadProFunc	// �߳���ڵ�ַ
			, NULL						//���ݸ��̺߳����Ĳ���
			, 0							// ָ���߳���������
			, &dwThreadId				//�߳�ID��
			);

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtlog *** ��־���� %d װ���� ��ʼд�� %02d:%02d:%02d"), nWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}

	//OutputDebugString(TEXT("ptdtlog *** AddGameServerLog 9"));
}


//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//���ñ���
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;


	//�رշ�������־
	g_cbCloseLogonServerLog = OPEN_LOG_EX;
	g_cbCloseGameServerLog = OPEN_LOG_EX;
	g_cbCloseChatServerLog = OPEN_LOG_EX;
	g_cbCloseGroupServerLog = OPEN_LOG_EX;

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{

}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//�󶨲���
	m_pBindParameter=new tagBindParameter[m_pInitParameter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxConnect);

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{

	//���ñ���
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);


	//����־���ݷ���������������־�б�
	if (g_cbOtherWriteControl == 0)
	{
		g_cbOtherWriteControl = 1;
	}
	else if (g_cbOtherWriteControl == 1)
	{
		g_cbOtherWriteControl = 2;
	}
	else if (g_cbOtherWriteControl == 2)
	{
		g_cbOtherWriteControl = 0;
	}

	HANDLE hThread = NULL;
	DWORD dwThreadId = 0;

	hThread = CreateThread(NULL		// Ĭ�ϰ�ȫ����
		, NULL						// Ĭ�϶�ջ��С
		, OtherServerThreadProFunc	// �߳���ڵ�ַ
		, NULL						//���ݸ��̺߳����Ĳ���
		, 0							// ָ���߳���������
		, &dwThreadId				//�߳�ID��
		);

	//����Ϸ��δд�����־д��
	if (g_cbLogGSWriteControl == 0)
	{
		g_cbLogGSWriteControl = 1;
	}
	else if (g_cbLogGSWriteControl == 1)
	{
		g_cbLogGSWriteControl = 2;
	}
	else if (g_cbLogGSWriteControl == 2)
	{
		g_cbLogGSWriteControl = 0;
	}


	HANDLE hGameThread = NULL;
	DWORD dwGameThreadId = 0;

	hGameThread = CreateThread(NULL		// Ĭ�ϰ�ȫ����
		, NULL						// Ĭ�϶�ջ��С
		, GameServerThreadProFunc	// �߳���ڵ�ַ
		, NULL						//���ݸ��̺߳����Ĳ���
		, 0							// ָ���߳���������
		, &dwGameThreadId				//�߳�ID��
		);


	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	
	return false;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��ȡ����
	//ASSERT(LOWORD(dwSocketID)<m_pInitParameter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParameter->m_wMaxConnect) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_S_S_LOG_INFO:		//����ע��
		{
			return OnTCPNetworkMainLog(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//��Ϸ����
	if (pBindParameter->ServiceKind==ServiceKind_Game)
	{
		
	}

	//�㳡����
	if (pBindParameter->ServiceKind==ServiceKind_Plaza)
	{

	}

	//�������
	if (pBindParameter->ServiceKind==ServiceKind_Chat)
	{

	}

	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPNetworkMainLog(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SERVER_LOG:	//��Ϸ��־
		{

			//��Ϣ����
			tagOtherServerLogInfo * pLogUserInfo = (tagOtherServerLogInfo *)pData;
			if (!pLogUserInfo)
			{
				return true;
			}
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtlog *** �յ���־���� 0"));
			//OutputDebugString(szInfo);

			

			pLogUserInfo->szServerName[CountArray(pLogUserInfo->szServerName) - 1] = 0;
			pLogUserInfo->szLogContent[CountArray(pLogUserInfo->szLogContent) - 1] = 0;
	

			tagOtherServerLogInfo OtherLogUserInfo;
			ZeroMemory(&OtherLogUserInfo, sizeof(OtherLogUserInfo));
			memcpy_s(&OtherLogUserInfo, sizeof(OtherLogUserInfo), pLogUserInfo, sizeof(OtherLogUserInfo));

			//TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtlog *** �յ���־���� 1"));
			//OutputDebugString(szInfo);

			//0 Э���������� 1 Լս�������� 2 ��¼�������� 3 ��Ϸ�������� 4 ����������� 5 ���������
			switch (OtherLogUserInfo.cbServerSign)
			{
				case CORRESPOND_SERVER:break;
				case PERSONAL_SERVER:break;
				case LOGON_SERVER:
				{
					//�رշ�������־
					if (g_cbCloseLogonServerLog == CLOSE_LOG_EX) break;
				}
				case CHAT_SERVER:
				{
					if (g_cbCloseChatServerLog == CLOSE_LOG_EX) break;
				}
				case GROUP_SERVER:
				{
					if (g_cbCloseGroupServerLog == CLOSE_LOG_EX && OtherLogUserInfo.cbServerSign == GROUP_SERVER) break;
					//return true;
					//��С��һ��
					if (wDataSize != sizeof(tagOtherServerLogInfo))
					{
						return true;
					}

					//��Ϣ����
					tagOtherServerLogInfo * pOtherLogUserInfo = (tagOtherServerLogInfo *)pData;
					if (!pLogUserInfo)
					{
						return true;
					}

					//����־���ݷ���������������־�б�
					if (g_cbOtherWriteControl == 0)
					{
						AddOtherServerLog(g_vecOtherServerLogFirst, *pLogUserInfo, 1, g_nFirstOtherSWriteCount);
					}
					else if (g_cbOtherWriteControl == 1)
					{
						AddOtherServerLog(g_vecOtherServerLogSecond, *pLogUserInfo, 2, g_nSecondOtherSWriteCount);
					}
					else if (g_cbOtherWriteControl == 2)
					{
						AddOtherServerLog(g_vecOtherServerLogThird, *pLogUserInfo, 0, g_nThirdOtherSWriteCount);
					}
							
					break;
				}
				case GAME_SERVER:
				{
					//��С��һ��
					if (wDataSize != sizeof(tagLogUserInfo))
					{
						return true;
					}

					//��Ϣ����
					tagLogUserInfo * pTempLogUserInfo = (tagLogUserInfo *)pData;
					if (!pLogUserInfo)
					{
						return true;
					}
					TCHAR szInfo[260] = {0};
					wsprintf(szInfo, TEXT("ptdtlog *** �յ���־���� 2"));
					//OutputDebugString(szInfo);

					pTempLogUserInfo->szDescName[CountArray(pTempLogUserInfo->szDescName) - 1] = 0;
					pTempLogUserInfo->szServerName[CountArray(pTempLogUserInfo->szServerName) - 1] = 0;
					pTempLogUserInfo->szGameName[CountArray(pTempLogUserInfo->szGameName) - 1] = 0;
					pTempLogUserInfo->szLogContent[CountArray(pTempLogUserInfo->szLogContent) - 1] = 0;
					pTempLogUserInfo->szNickName[CountArray(pTempLogUserInfo->szNickName) - 1] = 0;

					//tagLogUserInfo LogUserInfo;

					wsprintf(szInfo, TEXT("ptdtlog *** �յ���־���� ����"));
					//OutputDebugString(szInfo);

					tagLogUserInfo LogUserInfo;

					ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
					memcpy_s(&LogUserInfo, sizeof(LogUserInfo), pLogUserInfo, sizeof(LogUserInfo));

					wsprintf(szInfo, TEXT("ptdtlog *** �յ���־���� 3"));
					//OutputDebugString(szInfo);


					//����־���ݷ�����Ϸ��������־�б�
					if (g_cbCloseGameServerLog)
					{
						if (g_cbLogGSWriteControl == 0)
						{
							wsprintf(szInfo, TEXT("ptdtlog *** �յ���־���� 4"));
							//OutputDebugString(szInfo);

							AddGameServerLog(g_vecGameServerLogFirst, LogUserInfo, 1, g_nFirstGSWriteCount);
						}
						else if (g_cbLogGSWriteControl == 1)
						{
							wsprintf(szInfo, TEXT("ptdtlog *** �յ���־���� 5"));
							//OutputDebugString(szInfo);

							AddGameServerLog(g_vecGameServerLogSecond, LogUserInfo, 2, g_nSecondGSWriteCount);
						}
						else if (g_cbLogGSWriteControl == 2)
						{
							wsprintf(szInfo, TEXT("ptdtlog *** �յ���־���� 6"));
							//OutputDebugString(szInfo);

							AddGameServerLog(g_vecGameServerLogThird, LogUserInfo, 0, g_nThirdGSWriteCount);
						}		
					}

					wsprintf(szInfo, TEXT("ptdtlog *** �յ���־���� 7"));
					//OutputDebugString(szInfo);
					break;
				}
				default:break;
			}

			return true;
		}

	}

	return true;
}



//�رյ�¼��������־
VOID CAttemperEngineSink::CloseLogonServerLog(byte cbClose)
{
	g_cbCloseLogonServerLog = cbClose;

	////����ر���־���ͽ�δд�����־д��,����Ĵ����Ժ���ܻ��õ�
	//if (cbClose == 0)
	//{
	//	//����־���ݷ���������������־�б�
	//	if (g_cbOtherWriteControl == 0)
	//	{
	//		g_cbOtherWriteControl = 1;
	//	}
	//	else if (g_cbOtherWriteControl == 1)
	//	{
	//		g_cbOtherWriteControl = 2;
	//	}
	//	else if (g_cbOtherWriteControl == 2)
	//	{
	//		g_cbOtherWriteControl = 0;
	//	}

	//	HANDLE hThread = NULL;
	//	DWORD dwThreadId = 0;

	//	hThread = CreateThread(NULL		// Ĭ�ϰ�ȫ����
	//		, NULL						// Ĭ�϶�ջ��С
	//		, OtherServerThreadProFunc	// �߳���ڵ�ַ
	//		, NULL						//���ݸ��̺߳����Ĳ���
	//		, 0							// ָ���߳���������
	//		, &dwThreadId				//�߳�ID��
	//		);

	//	SYSTEMTIME sysTime;
	//	GetLocalTime(&sysTime);

	//	TCHAR szInfo[260] = { 0 };
	//	wsprintf(szInfo, TEXT("ptdtlog *** ��¼��������־ֹͣ g_cbLogGSWriteControl = %d�� ��ʼд�� %02d:%02d:%02d"), g_cbOtherWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	//	//OutputDebugString(szInfo);
	//}
}

//�ر���Ϸ��������־
VOID CAttemperEngineSink::CloseGamedServerLog(byte cbClose)
{
	g_cbCloseGameServerLog = cbClose;

	////����ر���־���ͽ�δд�����־д��,����Ĵ����Ժ���ܻ��õ�

	//if (g_cbLogGSWriteControl == 0)
	//{
	//	g_cbLogGSWriteControl = 1;
	//}
	//else if (g_cbLogGSWriteControl == 1)
	//{
	//	g_cbLogGSWriteControl = 2;
	//}
	//else if (g_cbLogGSWriteControl == 2)
	//{
	//	g_cbLogGSWriteControl = 0;
	//}


	//HANDLE hThread = NULL;
	//DWORD dwThreadId = 0;

	//hThread = CreateThread(NULL		// Ĭ�ϰ�ȫ����
	//	, NULL						// Ĭ�϶�ջ��С
	//	, GameServerThreadProFunc	// �߳���ڵ�ַ
	//	, NULL						//���ݸ��̺߳����Ĳ���
	//	, 0							// ָ���߳���������
	//	, &dwThreadId				//�߳�ID��
	//	);

	//SYSTEMTIME sysTime;
	//GetLocalTime(&sysTime);

	//TCHAR szInfo[260] = { 0 };
	//wsprintf(szInfo, TEXT("ptdtlog *** ��Ϸ��������־ֹͣ g_cbLogGSWriteControl = %d�� ��ʼд�� %02d:%02d:%02d"), g_cbLogGSWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	////OutputDebugString(szInfo);
	
}

//�ر������������־
VOID CAttemperEngineSink::CloseChatServerLog(byte cbClose)
{
	g_cbCloseChatServerLog = cbClose;
}

//�رվ��ֲ���־
VOID CAttemperEngineSink::CloseGroupServerLog(byte cbClose)
{
	g_cbCloseGroupServerLog = cbClose;
}




//////////////////////////////////////////////////////////////////////////////////
