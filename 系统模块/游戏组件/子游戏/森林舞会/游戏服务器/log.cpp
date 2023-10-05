#ifndef LOG
#define LOG

#include "Stdafx.h"
#include "log.h"
#include <string>

//CLog* CLog::m_pInstance = NULL;

CLog			g_Log;				//ȫ����־

bool CLog::Init(const char* szSubDir,const char* szSubDir2,const char* szLogFileName)
{

	char szDir[1024] = {0};
	sprintf_s(szDir,"%s\\%s\\%s",GetRootPath(),szSubDir,szSubDir2);
	if (!m_oBaseLogger.Init(LOG_MODE_DAY_DIR_DIVIDE,0,szDir,szLogFileName))
	{
		return false;
	}
	
	//bool Init(INT32 nMode, UINT32 dwID,	const char* pszDir, const char* pszName);
	//if (NULL == m_pLog)
	//{
	//	m_pLog = SDErrorGetModule(&SDERROR_VERSION);
	//	if (NULL == m_pLog)
	//	{
	//		return false;
	//	}

	//	if (!m_pLog->InitErrorModule())
	//	{
	//		return false;
	//	}
	//	if (!m_pLog->SetOutputMode(SDERROR_TYPE_LOG,SDERROR_OUTPUT_LOGFILE))
	//	{
	//		return false;
	//	}
	//	m_pLog->SetLogFormat(SDERROR_LOGFORMAT_DIVIDEBYTIME | SDERROR_LOGFORMAT_DIVIDEBYMODULE,szLogFileName);
	//	

	//	//m_pFileLogger = SDCreateFileLogger(&SDFILELOGGER_VERSION);
	//	return true;
	//}

	return true;
}

bool CLog::InitEx(const char* szSubDir,const char* szLogFileName)
{
	char szDir[1024] = {0};
	sprintf_s(szDir,"%s\\%s",GetRootPath(),szSubDir);
	if (!m_oBaseLogger.Init(LOG_MODE_DAY_DIVIDE,0,szDir,szLogFileName))
	{
		return false;
	}

	return true;
}
void CLog::UnInit()
{
	if (NULL != m_pLog)
	{
		Log("��־����");
		m_pLog->Release();
		m_pLog = NULL;
	}

}

void CLog::Log(const char* pszFormat, ...) 
{ 
	char pszDest[1024] = {0};
	va_list args; 
	va_start(args, pszFormat); 
	_vsnprintf_s(pszDest, 1024, pszFormat, args); 
	va_end(args);
	if (NULL != m_pLog)
	{
		m_pLog->Log(pszDest,"3dAnimal");
	}
	m_oBaseLogger.Log(pszDest,sizeof(pszDest));
	m_oBaseLogger.Flush();

}

void CLog::LogWithoutTimeMark(const char* pszFormat, ...) 
{ 
	char pszDest[1024] = {0};
	va_list args; 
	va_start(args, pszFormat); 
	_vsnprintf_s(pszDest, 1024, pszFormat, args); 
	va_end(args);
	if (NULL != m_pLog)
	{
		m_pLog->Log(pszDest,"3dAnimal");
	}
	m_oBaseLogger.LogWithoutTimeMark(pszDest,sizeof(pszDest));
	m_oBaseLogger.Flush();

}

const char* GetAnimalName(int nType)
{	
	eAnimalType eType = (eAnimalType)nType;
	if (eType == eAnimalType_Lion)
	{
		return "ʨ��";
	}
	if (eType == eAnimalType_Panda)
	{
		return "��è";
	}
	if (eType == eAnimalType_Monkey)
	{
		return "����";
	}
	if (eType == eAnimalType_Rabbit)
	{
		return "����";
	}
	return "δ֪����";
}

const char* GetColorName(int nType)
{
	eColorType eType = (eColorType)nType;
	if (eType == eColorType_Red)
	{
		return "��";
	}
	if (eType == eColorType_Green)
	{
		return "��";
	}
	if (eType == eColorType_Yellow)
	{
		return "��";
	}
	return "δ֪��ɫ";
}
/*
enum eAnimalPrizeMode
{
eAnimalPrizeMode_Invalid = -1,
eAnimalPrizeMode_SingleAnimalSingleColor = 0,
eAnimalPrizeMode_AllAnimalSingleColr,
eAnimalPrizeMode_AllColorSingleAnimal,
eAnimalPrizeMode_SysPrize,
eAnimalPrizeMode_RepeatTime,
eAnimalPrizeMode_Flash,

eAnimalPrizeMode_Max,
};
*/
const char* GetModeName(int nType)
{
	eAnimalPrizeMode eType = (eAnimalPrizeMode)nType; 
	if (eType == eAnimalPrizeMode_SingleAnimalSingleColor)
	{
		return "����ɫ������";
	}
	if (eType == eAnimalPrizeMode_AllAnimalSingleColr)
	{
		return "����ɫ";
	}
	if (eType == eAnimalPrizeMode_AllColorSingleAnimal)
	{
		return "������";
	}
	if (eType == eAnimalPrizeMode_SysPrize)
	{
		return "�ʽ�";
	}
	if (eType == eAnimalPrizeMode_RepeatTime)
	{
		return "�ظ�����";
	}
	if (eType == eAnimalPrizeMode_Flash)
	{
		return "���磨N���ʣ�";
	}
	return "δ֪ģʽ";
}

const char* GetEnjoyGameName(int nType)
{
	eEnjoyGameType eType = (eEnjoyGameType)nType;
	if (eType == eEnjoyGameType_Zhuang)
	{
		return "[ׯ]";
	}
	if (eType == eEnjoyGameType_Xian)
	{
		return "[��]";
	}
	if (eType == eEnjoyGameType_He)
	{
		return "[��]";
	}
	return "[~δ֪~]";
}


const TCHAR* GetAnimalNameW(int nType)
{	
	eAnimalType eType = (eAnimalType)nType;
	if (eType == eAnimalType_Lion)
	{
		return TEXT("ʨ��");
	}
	if (eType == eAnimalType_Panda)
	{
		return TEXT("��è");
	}
	if (eType == eAnimalType_Monkey)
	{
		return TEXT("����");
	}
	if (eType == eAnimalType_Rabbit)
	{
		return TEXT("����");
	}
	return TEXT("δ֪����");
}

const TCHAR* GetColorNameW(int nType)
{
	eColorType eType = (eColorType)nType;
	if (eType == eColorType_Red)
	{
		return TEXT("��");
	}
	if (eType == eColorType_Green)
	{
		return TEXT("��");
	}
	if (eType == eColorType_Yellow)
	{
		return TEXT("��");
	}
	return TEXT("δ֪��ɫ");
}

const TCHAR* GetModeNameW(int nType)
{
	eAnimalPrizeMode eType = (eAnimalPrizeMode)nType; 
	if (eType == eAnimalPrizeMode_SingleAnimalSingleColor)
	{
		return TEXT("����ɫ������");
	}
	if (eType == eAnimalPrizeMode_AllAnimalSingleColr)
	{
		return TEXT("����ɫ");
	}
	if (eType == eAnimalPrizeMode_AllColorSingleAnimal)
	{
		return TEXT("������");
	}
	if (eType == eAnimalPrizeMode_SysPrize)
	{
		return TEXT("�ʽ�");
	}
	if (eType == eAnimalPrizeMode_RepeatTime)
	{
		return TEXT("�ظ�����");
	}
	if (eType == eAnimalPrizeMode_Flash)
	{
		return TEXT("���磨N���ʣ�");
	}
	return TEXT("δ֪ģʽ");
}

const TCHAR* GetEnjoyGameNameW(int nType)
{
	eEnjoyGameType eType = (eEnjoyGameType)nType;
	if (eType == eEnjoyGameType_Zhuang)
	{
		return TEXT("[ׯ]");
	}
	if (eType == eEnjoyGameType_Xian)
	{
		return TEXT("[��]");
	}
	if (eType == eEnjoyGameType_He)
	{
		return TEXT("[��]");
	}
	return TEXT("[~δ֪~]");
}
#endif

