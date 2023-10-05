//�ر���IT������28xin.com��
#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define TIME_CONNECT				3									//����ʱ��
#define TIME_COLLECT				300									//ͳ��ʱ��
#define TIME_LOAD_LIST				300									//�����б�
#define TIME_RELOAD_LIST			600									//�����б�

//�ͻ�ʱ��
#define TIME_INTERMIT				0									//�ж�ʱ��
#define TIME_ONLINE_COUNT			600									//����ʱ��

//////////////////////////////////////////////////////////////////////////

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
	//ʱ�䶨��
	m_wConnectTime=TIME_CONNECT;
	//ϵͳ����
	m_wMaxConnect = MAX_CONTENT;
	m_wServicePort = PORT_GROUP;
	ZeroMemory(m_szServerName, sizeof(m_szServerName));

	//վ����Ϣ
	m_wStationID=0;
	ZeroMemory(m_szStationName,sizeof(m_szStationName));

	//Э����Ϣ
	m_wCorrespondPort = PORT_CENTER;
	ZeroMemory(&m_CorrespondAddress, sizeof(m_CorrespondAddress));

	//Լս��Ϣ
	m_wPrsnlRmCorrespondPort = PORT_PERSONAL_ROOM;
	ZeroMemory(&m_PrsnlRmCorrespondAddress, sizeof(m_PrsnlRmCorrespondAddress));

	//��־��������Ϣ
	m_wLogServerPort = PORT_LOG_SERVER;
	ZeroMemory(&m_LogServerAddress, sizeof(m_LogServerAddress));
	
	ZeroMemory(&m_IMGroupDBParameter, sizeof(m_IMGroupDBParameter));

	//AI�˿�
	m_wAIPort = PORT_ANDROID_SERVER;
	ZeroMemory(&m_AIAddress, sizeof(m_AIAddress));

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{

	//���ò���
	InitParameter();

	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH] = TEXT("");
	_sntprintf_s(szIniFile, CountArray(szIniFile), TEXT("%s\\ServerParameter.ini"), szWorkDir);

	//����·��
	TCHAR szshouquanshouquan[MAX_PATH] = TEXT("");
	_sntprintf_s(szshouquanshouquan, CountArray(szshouquanshouquan), TEXT("%s\\ServerParameter.ini"), szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);
	IniData.SetIniFilePath(szshouquanshouquan);
	m_wMaxConnect = IniData.ReadInt(TEXT("GroupServer"), TEXT("ConnectMax"), m_wMaxConnect);
	m_wServicePort = IniData.ReadInt(TEXT("GroupServer"), TEXT("ServicePort"), m_wServicePort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("ServiceName"), NULL, m_szServerName, CountArray(m_szServerName));
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("ServiceAddr"), NULL, m_ServiceAddress.szAddress, CountArray(m_ServiceAddress.szAddress));

	//Э����Ϣ
	m_wCorrespondPort = IniData.ReadInt(TEXT("Correspond"), TEXT("ServicePort"), m_wCorrespondPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("CorrespondAddr"), NULL, m_CorrespondAddress.szAddress, CountArray(m_CorrespondAddress.szAddress));

	//Լս��Ϣ
	m_wPrsnlRmCorrespondPort = IniData.ReadInt(TEXT("PrsnlRmCorrespond"), TEXT("ServicePort"), m_wPrsnlRmCorrespondPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("PrsnlRmCorrespondAddr"), m_CorrespondAddress.szAddress, m_PrsnlRmCorrespondAddress.szAddress, CountArray(m_PrsnlRmCorrespondAddress.szAddress));

	//AI��Ϣ
	m_wAIPort = IniData.ReadInt(TEXT("AndroidDistribute"), TEXT("ServicePort"), m_wAIPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("AIServerAddr"), m_CorrespondAddress.szAddress, m_AIAddress.szAddress, CountArray(m_AIAddress.szAddress));

	//��־��������ַ
	m_wLogServerPort = IniData.ReadInt(TEXT("LogServer"), TEXT("ServicePort"), m_wLogServerPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"), TEXT("LogServerAddr"), m_CorrespondAddress.szAddress, m_LogServerAddress.szAddress, CountArray(m_LogServerAddress.szAddress));


	//������Ϣ
	m_IMGroupDBParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("GroupDB"), TEXT("DBPort"), 1433);
	IniData.ReadEncryptString(TEXT("GroupDB"), TEXT("DBAddr"), NULL, m_IMGroupDBParameter.szDataBaseAddr, CountArray(m_IMGroupDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("GroupDB"), TEXT("DBUser"), NULL, m_IMGroupDBParameter.szDataBaseUser, CountArray(m_IMGroupDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("GroupDB"), TEXT("DBPass"), NULL, m_IMGroupDBParameter.szDataBasePass, CountArray(m_IMGroupDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("GroupDB"), TEXT("DBName"), szGroupDB, m_IMGroupDBParameter.szDataBaseName, CountArray(m_IMGroupDBParameter.szDataBaseName));

	return;
}

//////////////////////////////////////////////////////////////////////////
