#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////////////

//时间定义
#define TIME_CONNECT				30									//重连时间
#define TIME_COLLECT				30									//统计时间

//客户时间
#define TIME_INTERMIT				0									//中断时间
#define TIME_ONLINE_COUNT			600									//人数时间

//////////////////////////////////////////////////////////////////////////////////

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
	m_wMaxConnect = 1024;

	m_wConnectTime = TIME_CONNECT;
	m_wCollectTime = TIME_COLLECT;

	//协调信息
	m_wCorrespondPort=PORT_CENTER;
	ZeroMemory(&m_CorrespondAddress,sizeof(m_CorrespondAddress));

	m_wPrsnlRmCorrespondPort = PORT_PERSONAL_ROOM;
	ZeroMemory(&m_PrsnlRmCorrespondAddress,sizeof(m_PrsnlRmCorrespondAddress));

	//日志服务器信息
	m_wLogServerPort = PORT_LOG_SERVER;
	ZeroMemory(&m_LogServerAddress, sizeof(m_LogServerAddress));

	//AI分配
	m_wAndroidDistributePort = PORT_ANDROID_SERVER;
	ZeroMemory(&m_AndroidDistributePortAddress, sizeof(m_AndroidDistributePortAddress));

	//配置信息
	ZeroMemory(m_szServerName,sizeof(m_szServerName));
	ZeroMemory(&m_ServiceAddress,sizeof(m_ServiceAddress));
	ZeroMemory(&m_AccountDBParameter, sizeof(m_AccountDBParameter));
	ZeroMemory(&m_TreasureDBParameter,sizeof(m_TreasureDBParameter));
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));


	return;
}

//加载配置
VOID CInitParameter::LoadInitParameter()
{
	//重置参数
	InitParameter();

	//获取路径
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//构造路径
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("%s\\ServerParameter.ini"),szWorkDir);

	//读取配置
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//读取配置
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("ServiceName"),NULL,m_szServerName,CountArray(m_szServerName));
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("ServiceAddr"),NULL,m_ServiceAddress.szAddress,CountArray(m_ServiceAddress.szAddress));

	//协调信息
	m_wCorrespondPort=IniData.ReadInt(TEXT("Correspond"),TEXT("ServicePort"),m_wCorrespondPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("CorrespondAddr"),NULL,m_CorrespondAddress.szAddress,CountArray(m_CorrespondAddress.szAddress));

	//约战服务信息，默认地址与协调服务器地址相同
	m_wPrsnlRmCorrespondPort=IniData.ReadInt(TEXT("PersonalRoomCorrespond"),TEXT("ServicePort"),m_wPrsnlRmCorrespondPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("PersonalRoomCorrespondAddr"),m_CorrespondAddress.szAddress,m_PrsnlRmCorrespondAddress.szAddress,CountArray(m_PrsnlRmCorrespondAddress.szAddress));

	//日志服务器地址
	m_wLogServerPort = IniData.ReadInt(TEXT("LogServer"), TEXT("ServicePort"), m_wLogServerPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("LogServerAddr"), m_CorrespondAddress.szAddress, m_LogServerAddress.szAddress, CountArray(m_LogServerAddress.szAddress));

	//AI分配服务
	m_wAndroidDistributePort = IniData.ReadInt(TEXT("AndroidDistribute"), TEXT("ServicePort"), m_wAndroidDistributePort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("AndroidDistributeAddr"), m_AndroidDistributePortAddress.szAddress, m_AndroidDistributePortAddress.szAddress, CountArray(m_CorrespondAddress.szAddress));

	//连接信息
	m_AccountDBParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("AccountsDB"), TEXT("DBPort"), 1433);
	IniData.ReadEncryptString(TEXT("AccountsDB"), TEXT("DBAddr"), NULL, m_AccountDBParameter.szDataBaseAddr, CountArray(m_AccountDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("AccountsDB"), TEXT("DBUser"), NULL, m_AccountDBParameter.szDataBaseUser, CountArray(m_AccountDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("AccountsDB"), TEXT("DBPass"), NULL, m_AccountDBParameter.szDataBasePass, CountArray(m_AccountDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("AccountsDB"), TEXT("DBName"), szAccountsDB, m_AccountDBParameter.szDataBaseName, CountArray(m_AccountDBParameter.szDataBaseName));

	//连接信息
	m_TreasureDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("TreasureDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBAddr"),NULL,m_TreasureDBParameter.szDataBaseAddr,CountArray(m_TreasureDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBUser"),NULL,m_TreasureDBParameter.szDataBaseUser,CountArray(m_TreasureDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBPass"),NULL,m_TreasureDBParameter.szDataBasePass,CountArray(m_TreasureDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBName"),szTreasureDB,m_TreasureDBParameter.szDataBaseName,CountArray(m_TreasureDBParameter.szDataBaseName));

	//连接信息
	m_PlatformDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("PlatformDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBAddr"),NULL,m_PlatformDBParameter.szDataBaseAddr,CountArray(m_PlatformDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBUser"),NULL,m_PlatformDBParameter.szDataBaseUser,CountArray(m_PlatformDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBPass"),NULL,m_PlatformDBParameter.szDataBasePass,CountArray(m_PlatformDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBName"),szPlatformDB,m_PlatformDBParameter.szDataBaseName,CountArray(m_PlatformDBParameter.szDataBaseName));

	//读取配置
	m_wMaxConnect = IniData.ReadInt(TEXT("AndroidDistribute"), TEXT("ConnectMax"), m_wMaxConnect);

	return;
}

//设置参数
VOID CInitParameter::SetServerParameter(tagGameServiceAttrib *pGameServiceAttrib, tagGameServiceOption *pGameServiceOption)
{

}

//读取手机配置
VOID CInitParameter::ReadMobileParameter()
{
	

	return;
}
//////////////////////////////////////////////////////////////////////////////////
