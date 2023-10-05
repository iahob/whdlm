//�ر���IT������28xin.com��
#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////////////

#define REG_ITEM_NAME				TEXT("Information")					//ע�����

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
	//ϵͳ����
	m_wMaxConnect=MAX_CONTENT;
	m_wServicePort=PORT_CENTER;
	m_wPrsnlServicePort= PORT_PERSONAL_ROOM;
	//������Ϣ
	ZeroMemory(m_szServerName,sizeof(m_szServerName));

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
	_snwprintf_s(szIniFile, CountArray(szIniFile), TEXT("%s\\ServerParameter.ini"), szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//��ȡ����
	m_wMaxConnect=IniData.ReadInt(TEXT("PersonalRoom"),TEXT("ConnectMax"),m_wMaxConnect);
	m_wPrsnlServicePort=IniData.ReadInt(TEXT("PersonalRoom"),TEXT("ServicePort"),m_wPrsnlServicePort);
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("ServiceName"),NULL,m_szServerName,CountArray(m_szServerName));

	return;
}

//////////////////////////////////////////////////////////////////////////////////
