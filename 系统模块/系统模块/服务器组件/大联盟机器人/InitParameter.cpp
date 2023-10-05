#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define TIME_CONNECT				30									//����ʱ��
#define TIME_COLLECT				30									//ͳ��ʱ��

//�ͻ�ʱ��
#define TIME_INTERMIT				0									//�ж�ʱ��
#define TIME_ONLINE_COUNT			600									//����ʱ��

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CInitParameter::CInitParameter()
{ 
	InitParameter();
}

//��������
CInitParameter::~CInitParameter()
{
}

//��ʼ��
VOID CInitParameter::InitParameter()
{
	m_wMaxConnect = 1024;

	m_wConnectTime = TIME_CONNECT;
	m_wCollectTime = TIME_COLLECT;

	//Э����Ϣ
	m_wCorrespondPort=PORT_CENTER;
	ZeroMemory(&m_CorrespondAddress,sizeof(m_CorrespondAddress));

	m_wPrsnlRmCorrespondPort = PORT_PERSONAL_ROOM;
	ZeroMemory(&m_PrsnlRmCorrespondAddress,sizeof(m_PrsnlRmCorrespondAddress));

	//��־��������Ϣ
	m_wLogServerPort = PORT_LOG_SERVER;
	ZeroMemory(&m_LogServerAddress, sizeof(m_LogServerAddress));

	//AI����
	m_wAndroidDistributePort = PORT_ANDROID_SERVER;
	ZeroMemory(&m_AndroidDistributePortAddress, sizeof(m_AndroidDistributePortAddress));

	//������Ϣ
	ZeroMemory(m_szServerName,sizeof(m_szServerName));
	ZeroMemory(&m_ServiceAddress,sizeof(m_ServiceAddress));
	ZeroMemory(&m_AccountDBParameter, sizeof(m_AccountDBParameter));
	ZeroMemory(&m_TreasureDBParameter,sizeof(m_TreasureDBParameter));
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));


	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();

	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("%s\\ServerParameter.ini"),szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//��ȡ����
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("ServiceName"),NULL,m_szServerName,CountArray(m_szServerName));
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("ServiceAddr"),NULL,m_ServiceAddress.szAddress,CountArray(m_ServiceAddress.szAddress));

	//Э����Ϣ
	m_wCorrespondPort=IniData.ReadInt(TEXT("Correspond"),TEXT("ServicePort"),m_wCorrespondPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("CorrespondAddr"),NULL,m_CorrespondAddress.szAddress,CountArray(m_CorrespondAddress.szAddress));

	//Լս������Ϣ��Ĭ�ϵ�ַ��Э����������ַ��ͬ
	m_wPrsnlRmCorrespondPort=IniData.ReadInt(TEXT("PersonalRoomCorrespond"),TEXT("ServicePort"),m_wPrsnlRmCorrespondPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("PersonalRoomCorrespondAddr"),m_CorrespondAddress.szAddress,m_PrsnlRmCorrespondAddress.szAddress,CountArray(m_PrsnlRmCorrespondAddress.szAddress));

	//��־��������ַ
	m_wLogServerPort = IniData.ReadInt(TEXT("LogServer"), TEXT("ServicePort"), m_wLogServerPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("LogServerAddr"), m_CorrespondAddress.szAddress, m_LogServerAddress.szAddress, CountArray(m_LogServerAddress.szAddress));

	//AI�������
	m_wAndroidDistributePort = IniData.ReadInt(TEXT("AndroidDistribute"), TEXT("ServicePort"), m_wAndroidDistributePort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("AndroidDistributeAddr"), m_AndroidDistributePortAddress.szAddress, m_AndroidDistributePortAddress.szAddress, CountArray(m_CorrespondAddress.szAddress));

	//������Ϣ
	m_AccountDBParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("AccountsDB"), TEXT("DBPort"), 1433);
	IniData.ReadEncryptString(TEXT("AccountsDB"), TEXT("DBAddr"), NULL, m_AccountDBParameter.szDataBaseAddr, CountArray(m_AccountDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("AccountsDB"), TEXT("DBUser"), NULL, m_AccountDBParameter.szDataBaseUser, CountArray(m_AccountDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("AccountsDB"), TEXT("DBPass"), NULL, m_AccountDBParameter.szDataBasePass, CountArray(m_AccountDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("AccountsDB"), TEXT("DBName"), szAccountsDB, m_AccountDBParameter.szDataBaseName, CountArray(m_AccountDBParameter.szDataBaseName));

	//������Ϣ
	m_TreasureDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("TreasureDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBAddr"),NULL,m_TreasureDBParameter.szDataBaseAddr,CountArray(m_TreasureDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBUser"),NULL,m_TreasureDBParameter.szDataBaseUser,CountArray(m_TreasureDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBPass"),NULL,m_TreasureDBParameter.szDataBasePass,CountArray(m_TreasureDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBName"),szTreasureDB,m_TreasureDBParameter.szDataBaseName,CountArray(m_TreasureDBParameter.szDataBaseName));

	//������Ϣ
	m_PlatformDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("PlatformDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBAddr"),NULL,m_PlatformDBParameter.szDataBaseAddr,CountArray(m_PlatformDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBUser"),NULL,m_PlatformDBParameter.szDataBaseUser,CountArray(m_PlatformDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBPass"),NULL,m_PlatformDBParameter.szDataBasePass,CountArray(m_PlatformDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBName"),szPlatformDB,m_PlatformDBParameter.szDataBaseName,CountArray(m_PlatformDBParameter.szDataBaseName));

	//��ȡ����
	m_wMaxConnect = IniData.ReadInt(TEXT("AndroidDistribute"), TEXT("ConnectMax"), m_wMaxConnect);

	return;
}

//���ò���
VOID CInitParameter::SetServerParameter(tagGameServiceAttrib *pGameServiceAttrib, tagGameServiceOption *pGameServiceOption)
{

}

//��ȡ�ֻ�����
VOID CInitParameter::ReadMobileParameter()
{
	

	return;
}
//////////////////////////////////////////////////////////////////////////////////
