
#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"
#include <direct.h> 
#include <locale.h>
#include <list>
using namespace std;


////////////////////////////////////////////////////
/*************** 登录服务器日志数组定义 ***************/
vector<tagOtherServerLogInfo> g_vecOtherServerLogFirst;  //日志缓冲数组1
vector<tagOtherServerLogInfo> g_vecOtherServerLogSecond; //日志缓冲数组2
vector<tagOtherServerLogInfo> g_vecOtherServerLogThird;  //日志缓冲数组3

//缓冲数组满了之后一次性将缓冲数组中的内容写入文本文档
int g_nFirstOtherSWriteCount = 0;				//日志缓冲数组元素总数1
int g_nSecondOtherSWriteCount = 0;				//日志缓冲数组元素总数2
int g_nThirdOtherSWriteCount = 0;				//日志缓冲数组元素总数3

int g_cbOtherWriteControl = 0;					//日志数组写入控制
const int MAX_OTHER_SERVER_LIST_COUNT = 10000;	//日志缓冲数组元素最大个数

////////////////////////////////////////////////////
/*************** 游戏服务器日志数组定义 ***************/
vector<tagLogUserInfo> g_vecGameServerLogFirst;		//日志缓冲数组1
vector<tagLogUserInfo> g_vecGameServerLogSecond;	//日志缓冲数组2
vector<tagLogUserInfo> g_vecGameServerLogThird;		//日志缓冲数组3

//缓冲数组满了之后一次性将缓冲数组中的内容写入文本文档
int g_nFirstGSWriteCount = 0;	//日志缓冲数组元素总数1
int g_nSecondGSWriteCount = 0;  //日志缓冲数组元素总数2
int g_nThirdGSWriteCount = 0;	//日志缓冲数组元素总数3

int g_cbLogGSWriteControl = 0;	//日志数组写入控制
const int MAX_LIST_COUNT = 10000;
////////////////////////////////////////////////////


byte	g_cbCloseLogonServerLog;			//关闭登录服务器日志	0 关闭		1 开启
byte	g_cbCloseGameServerLog;				//关闭游戏服务器日志	0 关闭		1 开启
byte	g_cbCloseChatServerLog;				//关闭聊天服务器日志	0 关闭		1 开启
byte	g_cbCloseGroupServerLog;			//关闭接入服务器日志	0 关闭		1 开启



//////////////////////////////////////////////////////////////////////////////////
/******************************* 登录服务器日志处理 *******************************/
void WriteOtherServerLog(vector<tagOtherServerLogInfo> & lstLogUserInfo, int & nCount)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	TCHAR szPath[MAX_PATH] = { 0 };

	//定义写入日志内容大小
	TCHAR szInfo[2062] = { 0 };
	ZeroMemory(szInfo, sizeof(szInfo));

	//统计写入日志条数
	int nStatic = 0;

	//写所有日志
	int nLogStatic = 0;
	
	//定义文件指针
	FILE *fp;
	fp = NULL;

	//定义日志内容
	tagOtherServerLogInfo LogUserInfo;
	
	//循环写入日志
	while (nLogStatic < lstLogUserInfo.size())
	{
		//设置日志路径
		wsprintf(szPath, TEXT("Log//%s//%d-%d-%d-%d-%sLog.txt"), LogUserInfo.szServerName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, LogUserInfo.szServerName);

		int nErr = _wfopen_s(&fp, szPath, TEXT("at+"));

		//如果文件没有打开则重新生成路径并打开
		if (nErr != 0)
		{
			//创建log文件夹
			int nStatus = _mkdir("Log");
			wsprintf(szPath, TEXT("Log//%s"), LogUserInfo.szServerName);
			nStatus = _wmkdir(szPath);

			//设置路径并打开文件
			wsprintf(szPath, TEXT("Log//%s//%d-%d-%d-%d-%sLog.txt"), LogUserInfo.szServerName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, LogUserInfo.szServerName);
			nErr = _wfopen_s(&fp, szPath, TEXT("at+"));
		}


		//获取日志列表中最前面一条日志，并写入
		LogUserInfo = lstLogUserInfo[nLogStatic];
		
		//格式化写入的语言
		_wsetlocale(LC_ALL, L"chs");//务必加上否则生成的文本是问号字符串

		//写入文件
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


//处理游戏服务器日志
DWORD WINAPI OtherServerThreadProFunc(LPVOID lpParam)
{
	SYSTEMTIME sysTime;
	TCHAR szInfo[260] = { 0 };
	GetLocalTime(&sysTime);

	if (g_cbOtherWriteControl == 1)
	{
		WriteOtherServerLog(g_vecOtherServerLogFirst, g_nFirstOtherSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** 日志容器 %d 装满， 写入完成 %02d:%02d:%02d"), g_cbOtherWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}
	else if (g_cbOtherWriteControl == 2)
	{
		WriteOtherServerLog(g_vecOtherServerLogSecond, g_nSecondOtherSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** 日志容器 %d 装满， 写入完成 %02d:%02d:%02d"), g_cbOtherWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}
	else if (g_cbOtherWriteControl == 0)
	{
		WriteOtherServerLog(g_vecOtherServerLogThird, g_nThirdOtherSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** 日志容器 %d 装满， 写入完成 %02d:%02d:%02d"), g_cbOtherWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}

	return 0;

}

//将日志内容填充进日志缓冲数组
void AddOtherServerLog(vector<tagOtherServerLogInfo> & lstLogUserInfo, tagOtherServerLogInfo LogUserInfo, int nWriteControl, int & nWriteCount)
{
	//如果日志缓冲数组未满则加入，如果已满则重新赋值
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

		hThread = CreateThread(NULL		// 默认安全属性
			, NULL						// 默认堆栈大小
			, OtherServerThreadProFunc	// 线程入口地址
			, NULL						//传递给线程函数的参数
			, 0							// 指定线程立即运行
			, &dwThreadId				//线程ID号
			);

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtlog *** 日志容器 %d 装满， 开始写入 %02d:%02d:%02d"), nWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}
}



//////////////////////////////////////////////////////////////////////////////////
/******************************* 游戏服务器日志处理 *******************************/
void WriteGameServerLog(vector<tagLogUserInfo> & lstLogUserInfo, int & nCount)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	TCHAR szPath[MAX_PATH] = { 0 };

	//定义写入日志内容大小
	TCHAR szInfo[2062] = { 0 };
	ZeroMemory(szInfo, sizeof(szInfo));

	//统计写入日志条数
	int nStatic = 0;

	//写所有日志
	int nLogStatic = 0;

	tagLogUserInfo LogUserInfo;
	//如果文件打不开则返回
	FILE *fp;
	fp = NULL;
	while (nLogStatic < lstLogUserInfo.size())
	{

		//获取日志列表中最前面一条日志，并写入
		LogUserInfo = lstLogUserInfo[nLogStatic];
		wsprintf(szPath, TEXT("Log//GameServer//%s//%s-[%d]//%d-%d-%d-%d-ServerLog.txt"), LogUserInfo.szGameName, LogUserInfo.szServerName, LogUserInfo.wServerID, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour);

		_wsetlocale(LC_ALL, L"chs");



		int nErr = _wfopen_s(&fp, szPath, TEXT("at+"));

		if (nErr != 0)
		{
			//创建log文件夹
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
				fwprintf_s(fp, TEXT("[%02d-%02d-%02d-%02d-%02d-%02d]｛USERID=%d, CHAIRID=%d, TABLEID=%d, NICKNAME=%ls｝ %ls\r\n"), LogUserInfo.sysTime.wYear, LogUserInfo.sysTime.wMonth, LogUserInfo.sysTime.wDay, LogUserInfo.sysTime.wHour, LogUserInfo.sysTime.wMinute, LogUserInfo.sysTime.wSecond, LogUserInfo.dwUserID, LogUserInfo.wChairID, LogUserInfo.wTableID, LogUserInfo.szNickName, LogUserInfo.szLogContent);
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

//处理游戏服务器日志
DWORD WINAPI GameServerThreadProFunc(LPVOID lpParam)
{
	SYSTEMTIME sysTime;
	TCHAR szInfo[260] = { 0 };
	GetLocalTime(&sysTime);

	if (g_cbLogGSWriteControl == 1)
	{
		WriteGameServerLog(g_vecGameServerLogFirst, g_nFirstGSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** 日志容器 %d 装满， 写入完成 %02d:%02d:%02d"), g_cbLogGSWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}
	else if (g_cbLogGSWriteControl == 2)
	{
		WriteGameServerLog(g_vecGameServerLogSecond, g_nSecondGSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** 日志容器 %d 装满， 写入完成 %02d:%02d:%02d"), g_cbLogGSWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}
	else if (g_cbLogGSWriteControl == 0)
	{
		WriteGameServerLog(g_vecGameServerLogThird, g_nThirdGSWriteCount);

		wsprintf(szInfo, TEXT("ptdtlog *** 日志容器 %d 装满， 写入完成 %02d:%02d:%02d"), g_cbLogGSWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}

	return 0;

}

//将日志内容填充进日志缓冲数组
void AddGameServerLog(vector<tagLogUserInfo> & lstLogUserInfo, tagLogUserInfo LogUserInfo, int nWriteControl, int & nWriteCount)
{
	//如果日志缓冲数组未满则加入，如果已满则重新赋值
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

		hThread = CreateThread(NULL		// 默认安全属性
			, NULL						// 默认堆栈大小
			, GameServerThreadProFunc	// 线程入口地址
			, NULL						//传递给线程函数的参数
			, 0							// 指定线程立即运行
			, &dwThreadId				//线程ID号
			);

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtlog *** 日志容器 %d 装满， 开始写入 %02d:%02d:%02d"), nWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		//OutputDebugString(szInfo);
	}

	//OutputDebugString(TEXT("ptdtlog *** AddGameServerLog 9"));
}


//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//设置变量
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;


	//关闭服务器日志
	g_cbCloseLogonServerLog = OPEN_LOG_EX;
	g_cbCloseGameServerLog = OPEN_LOG_EX;
	g_cbCloseChatServerLog = OPEN_LOG_EX;
	g_cbCloseGroupServerLog = OPEN_LOG_EX;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{

}

//接口查询
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定参数
	m_pBindParameter=new tagBindParameter[m_pInitParameter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxConnect);

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{

	//设置变量
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);


	//将日志内容放入其他服务器日志列表
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

	hThread = CreateThread(NULL		// 默认安全属性
		, NULL						// 默认堆栈大小
		, OtherServerThreadProFunc	// 线程入口地址
		, NULL						//传递给线程函数的参数
		, 0							// 指定线程立即运行
		, &dwThreadId				//线程ID号
		);

	//将游戏服未写入的日志写入
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

	hGameThread = CreateThread(NULL		// 默认安全属性
		, NULL						// 默认堆栈大小
		, GameServerThreadProFunc	// 线程入口地址
		, NULL						//传递给线程函数的参数
		, 0							// 指定线程立即运行
		, &dwGameThreadId				//线程ID号
		);


	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	return false;
}

//调度事件
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	
	return false;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//获取索引
	//ASSERT(LOWORD(dwSocketID)<m_pInitParameter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParameter->m_wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_S_S_LOG_INFO:		//服务注册
		{
			return OnTCPNetworkMainLog(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//游戏服务
	if (pBindParameter->ServiceKind==ServiceKind_Game)
	{
		
	}

	//广场服务
	if (pBindParameter->ServiceKind==ServiceKind_Plaza)
	{

	}

	//聊天服务
	if (pBindParameter->ServiceKind==ServiceKind_Chat)
	{

	}

	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainLog(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SERVER_LOG:	//游戏日志
		{

			//消息定义
			tagOtherServerLogInfo * pLogUserInfo = (tagOtherServerLogInfo *)pData;
			if (!pLogUserInfo)
			{
				return true;
			}
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtlog *** 收到日志数据 0"));
			//OutputDebugString(szInfo);

			

			pLogUserInfo->szServerName[CountArray(pLogUserInfo->szServerName) - 1] = 0;
			pLogUserInfo->szLogContent[CountArray(pLogUserInfo->szLogContent) - 1] = 0;
	

			tagOtherServerLogInfo OtherLogUserInfo;
			ZeroMemory(&OtherLogUserInfo, sizeof(OtherLogUserInfo));
			memcpy_s(&OtherLogUserInfo, sizeof(OtherLogUserInfo), pLogUserInfo, sizeof(OtherLogUserInfo));

			//TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtlog *** 收到日志数据 1"));
			//OutputDebugString(szInfo);

			//0 协调服务器， 1 约战服务器， 2 登录服务器， 3 游戏服务器， 4 聊天服务器， 5 接入服务器
			switch (OtherLogUserInfo.cbServerSign)
			{
				case CORRESPOND_SERVER:break;
				case PERSONAL_SERVER:break;
				case LOGON_SERVER:
				{
					//关闭服务器日志
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
					//大小不一致
					if (wDataSize != sizeof(tagOtherServerLogInfo))
					{
						return true;
					}

					//消息定义
					tagOtherServerLogInfo * pOtherLogUserInfo = (tagOtherServerLogInfo *)pData;
					if (!pLogUserInfo)
					{
						return true;
					}

					//将日志内容放入其他服务器日志列表
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
					//大小不一致
					if (wDataSize != sizeof(tagLogUserInfo))
					{
						return true;
					}

					//消息定义
					tagLogUserInfo * pTempLogUserInfo = (tagLogUserInfo *)pData;
					if (!pLogUserInfo)
					{
						return true;
					}
					TCHAR szInfo[260] = {0};
					wsprintf(szInfo, TEXT("ptdtlog *** 收到日志数据 2"));
					//OutputDebugString(szInfo);

					pTempLogUserInfo->szDescName[CountArray(pTempLogUserInfo->szDescName) - 1] = 0;
					pTempLogUserInfo->szServerName[CountArray(pTempLogUserInfo->szServerName) - 1] = 0;
					pTempLogUserInfo->szGameName[CountArray(pTempLogUserInfo->szGameName) - 1] = 0;
					pTempLogUserInfo->szLogContent[CountArray(pTempLogUserInfo->szLogContent) - 1] = 0;
					pTempLogUserInfo->szNickName[CountArray(pTempLogUserInfo->szNickName) - 1] = 0;

					//tagLogUserInfo LogUserInfo;

					wsprintf(szInfo, TEXT("ptdtlog *** 收到日志数据 拷贝"));
					//OutputDebugString(szInfo);

					tagLogUserInfo LogUserInfo;

					ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
					memcpy_s(&LogUserInfo, sizeof(LogUserInfo), pLogUserInfo, sizeof(LogUserInfo));

					wsprintf(szInfo, TEXT("ptdtlog *** 收到日志数据 3"));
					//OutputDebugString(szInfo);


					//将日志内容放入游戏服务器日志列表
					if (g_cbCloseGameServerLog)
					{
						if (g_cbLogGSWriteControl == 0)
						{
							wsprintf(szInfo, TEXT("ptdtlog *** 收到日志数据 4"));
							//OutputDebugString(szInfo);

							AddGameServerLog(g_vecGameServerLogFirst, LogUserInfo, 1, g_nFirstGSWriteCount);
						}
						else if (g_cbLogGSWriteControl == 1)
						{
							wsprintf(szInfo, TEXT("ptdtlog *** 收到日志数据 5"));
							//OutputDebugString(szInfo);

							AddGameServerLog(g_vecGameServerLogSecond, LogUserInfo, 2, g_nSecondGSWriteCount);
						}
						else if (g_cbLogGSWriteControl == 2)
						{
							wsprintf(szInfo, TEXT("ptdtlog *** 收到日志数据 6"));
							//OutputDebugString(szInfo);

							AddGameServerLog(g_vecGameServerLogThird, LogUserInfo, 0, g_nThirdGSWriteCount);
						}		
					}

					wsprintf(szInfo, TEXT("ptdtlog *** 收到日志数据 7"));
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



//关闭登录服务器日志
VOID CAttemperEngineSink::CloseLogonServerLog(byte cbClose)
{
	g_cbCloseLogonServerLog = cbClose;

	////如果关闭日志，就将未写完的日志写完,下面的代码以后可能会用到
	//if (cbClose == 0)
	//{
	//	//将日志内容放入其他服务器日志列表
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

	//	hThread = CreateThread(NULL		// 默认安全属性
	//		, NULL						// 默认堆栈大小
	//		, OtherServerThreadProFunc	// 线程入口地址
	//		, NULL						//传递给线程函数的参数
	//		, 0							// 指定线程立即运行
	//		, &dwThreadId				//线程ID号
	//		);

	//	SYSTEMTIME sysTime;
	//	GetLocalTime(&sysTime);

	//	TCHAR szInfo[260] = { 0 };
	//	wsprintf(szInfo, TEXT("ptdtlog *** 登录服务器日志停止 g_cbLogGSWriteControl = %d， 开始写入 %02d:%02d:%02d"), g_cbOtherWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	//	//OutputDebugString(szInfo);
	//}
}

//关闭游戏服务器日志
VOID CAttemperEngineSink::CloseGamedServerLog(byte cbClose)
{
	g_cbCloseGameServerLog = cbClose;

	////如果关闭日志，就将未写完的日志写完,下面的代码以后可能会用到

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

	//hThread = CreateThread(NULL		// 默认安全属性
	//	, NULL						// 默认堆栈大小
	//	, GameServerThreadProFunc	// 线程入口地址
	//	, NULL						//传递给线程函数的参数
	//	, 0							// 指定线程立即运行
	//	, &dwThreadId				//线程ID号
	//	);

	//SYSTEMTIME sysTime;
	//GetLocalTime(&sysTime);

	//TCHAR szInfo[260] = { 0 };
	//wsprintf(szInfo, TEXT("ptdtlog *** 游戏服务器日志停止 g_cbLogGSWriteControl = %d， 开始写入 %02d:%02d:%02d"), g_cbLogGSWriteControl, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	////OutputDebugString(szInfo);
	
}

//关闭聊天服务器日志
VOID CAttemperEngineSink::CloseChatServerLog(byte cbClose)
{
	g_cbCloseChatServerLog = cbClose;
}

//关闭俱乐部日志
VOID CAttemperEngineSink::CloseGroupServerLog(byte cbClose)
{
	g_cbCloseGroupServerLog = cbClose;
}




//////////////////////////////////////////////////////////////////////////////////
