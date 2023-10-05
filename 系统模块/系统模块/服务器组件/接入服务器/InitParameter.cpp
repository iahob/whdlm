//藏宝库IT社区（28xin.com）
#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//时间定义
#define TIME_CONNECT				3									//重连时间
#define TIME_COLLECT				300									//统计时间
#define TIME_LOAD_LIST				300									//加载列表
#define TIME_RELOAD_LIST			600									//加载列表

//客户时间
#define TIME_INTERMIT				0									//中断时间
#define TIME_ONLINE_COUNT			600									//人数时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CInitParameter::CInitParameter()
{ 
	InitParameter();
}

//析构函数
CInitParameter::~CInitParameter()
{
}

//初始化
VOID CInitParameter::InitParameter()
{
	//时间定义
	m_wConnectTime=TIME_CONNECT;
	//系统配置
	m_wMaxConnect = MAX_CONTENT;
	m_wServicePort = PORT_GROUP;
	ZeroMemory(m_szServerName, sizeof(m_szServerName));

	//站点信息
	m_wStationID=0;
	ZeroMemory(m_szStationName,sizeof(m_szStationName));

	//协调信息
	m_wCorrespondPort = PORT_CENTER;
	ZeroMemory(&m_CorrespondAddress, sizeof(m_CorrespondAddress));

	//约战信息
	m_wPrsnlRmCorrespondPort = PORT_PERSONAL_ROOM;
	ZeroMemory(&m_PrsnlRmCorrespondAddress, sizeof(m_PrsnlRmCorrespondAddress));

	//日志服务器信息
	m_wLogServerPort = PORT_LOG_SERVER;
	ZeroMemory(&m_LogServerAddress, sizeof(m_LogServerAddress));
	
	ZeroMemory(&m_IMGroupDBParameter, sizeof(m_IMGroupDBParameter));

	//AI端口
	m_wAIPort = PORT_ANDROID_SERVER;
	ZeroMemory(&m_AIAddress, sizeof(m_AIAddress));

	return;
}

//加载配置
VOID CInitParameter::LoadInitParameter()
{

	//重置参数
	InitParameter();

	//获取路径
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//构造路径
	TCHAR szIniFile[MAX_PATH] = TEXT("");
	_sntprintf_s(szIniFile, CountArray(szIniFile), TEXT("%s\\ServerParameter.ini"), szWorkDir);

	//构造路径
	TCHAR szshouquanshouquan[MAX_PATH] = TEXT("");
	_sntprintf_s(szshouquanshouquan, CountArray(szshouquanshouquan), TEXT("%s\\ServerParameter.ini"), szWorkDir);

	//读取配置
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);
	IniData.SetIniFilePath(szshouquanshouquan);
	m_wMaxConnect = IniData.ReadInt(TEXT("GroupServer"), TEXT("ConnectMax"), m_wMaxConnect);
	m_wServicePort = IniData.ReadInt(TEXT("GroupServer"), TEXT("ServicePort"), m_wServicePort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("ServiceName"), NULL, m_szServerName, CountArray(m_szServerName));
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("ServiceAddr"), NULL, m_ServiceAddress.szAddress, CountArray(m_ServiceAddress.szAddress));

	//协调信息
	m_wCorrespondPort = IniData.ReadInt(TEXT("Correspond"), TEXT("ServicePort"), m_wCorrespondPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("CorrespondAddr"), NULL, m_CorrespondAddress.szAddress, CountArray(m_CorrespondAddress.szAddress));

	//约战信息
	m_wPrsnlRmCorrespondPort = IniData.ReadInt(TEXT("PrsnlRmCorrespond"), TEXT("ServicePort"), m_wPrsnlRmCorrespondPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("PrsnlRmCorrespondAddr"), m_CorrespondAddress.szAddress, m_PrsnlRmCorrespondAddress.szAddress, CountArray(m_PrsnlRmCorrespondAddress.szAddress));

	//AI信息
	m_wAIPort = IniData.ReadInt(TEXT("AndroidDistribute"), TEXT("ServicePort"), m_wAIPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("AIServerAddr"), m_CorrespondAddress.szAddress, m_AIAddress.szAddress, CountArray(m_AIAddress.szAddress));

	//日志服务器地址
	m_wLogServerPort = IniData.ReadInt(TEXT("LogServer"), TEXT("ServicePort"), m_wLogServerPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("LogServerAddr"), m_CorrespondAddress.szAddress, m_LogServerAddress.szAddress, CountArray(m_LogServerAddress.szAddress));


	//连接信息
	m_IMGroupDBParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("GroupDB"), TEXT("DBPort"), 1433);
	IniData.ReadEncryptString(TEXT("GroupDB"), TEXT("DBAddr"), NULL, m_IMGroupDBParameter.szDataBaseAddr, CountArray(m_IMGroupDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("GroupDB"), TEXT("DBUser"), NULL, m_IMGroupDBParameter.szDataBaseUser, CountArray(m_IMGroupDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("GroupDB"), TEXT("DBPass"), NULL, m_IMGroupDBParameter.szDataBasePass, CountArray(m_IMGroupDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("GroupDB"), TEXT("DBName"), szGroupDB, m_IMGroupDBParameter.szDataBaseName, CountArray(m_IMGroupDBParameter.szDataBaseName));

	return;
}

//////////////////////////////////////////////////////////////////////////
