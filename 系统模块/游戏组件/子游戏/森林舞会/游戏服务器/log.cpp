#ifndef LOG
#define LOG

#include "Stdafx.h"
#include "log.h"
#include <string>

//CLog* CLog::m_pInstance = NULL;

CLog			g_Log;				//全局日志

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
		Log("日志结束");
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
		return "狮子";
	}
	if (eType == eAnimalType_Panda)
	{
		return "熊猫";
	}
	if (eType == eAnimalType_Monkey)
	{
		return "猴子";
	}
	if (eType == eAnimalType_Rabbit)
	{
		return "兔子";
	}
	return "未知动物";
}

const char* GetColorName(int nType)
{
	eColorType eType = (eColorType)nType;
	if (eType == eColorType_Red)
	{
		return "红";
	}
	if (eType == eColorType_Green)
	{
		return "绿";
	}
	if (eType == eColorType_Yellow)
	{
		return "黄";
	}
	return "未知颜色";
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
		return "单颜色单动物";
	}
	if (eType == eAnimalPrizeMode_AllAnimalSingleColr)
	{
		return "单颜色";
	}
	if (eType == eAnimalPrizeMode_AllColorSingleAnimal)
	{
		return "单动物";
	}
	if (eType == eAnimalPrizeMode_SysPrize)
	{
		return "彩金";
	}
	if (eType == eAnimalPrizeMode_RepeatTime)
	{
		return "重复开奖";
	}
	if (eType == eAnimalPrizeMode_Flash)
	{
		return "闪电（N倍率）";
	}
	return "未知模式";
}

const char* GetEnjoyGameName(int nType)
{
	eEnjoyGameType eType = (eEnjoyGameType)nType;
	if (eType == eEnjoyGameType_Zhuang)
	{
		return "[庄]";
	}
	if (eType == eEnjoyGameType_Xian)
	{
		return "[闲]";
	}
	if (eType == eEnjoyGameType_He)
	{
		return "[和]";
	}
	return "[~未知~]";
}


const TCHAR* GetAnimalNameW(int nType)
{	
	eAnimalType eType = (eAnimalType)nType;
	if (eType == eAnimalType_Lion)
	{
		return TEXT("狮子");
	}
	if (eType == eAnimalType_Panda)
	{
		return TEXT("熊猫");
	}
	if (eType == eAnimalType_Monkey)
	{
		return TEXT("猴子");
	}
	if (eType == eAnimalType_Rabbit)
	{
		return TEXT("兔子");
	}
	return TEXT("未知动物");
}

const TCHAR* GetColorNameW(int nType)
{
	eColorType eType = (eColorType)nType;
	if (eType == eColorType_Red)
	{
		return TEXT("红");
	}
	if (eType == eColorType_Green)
	{
		return TEXT("绿");
	}
	if (eType == eColorType_Yellow)
	{
		return TEXT("黄");
	}
	return TEXT("未知颜色");
}

const TCHAR* GetModeNameW(int nType)
{
	eAnimalPrizeMode eType = (eAnimalPrizeMode)nType; 
	if (eType == eAnimalPrizeMode_SingleAnimalSingleColor)
	{
		return TEXT("单颜色单动物");
	}
	if (eType == eAnimalPrizeMode_AllAnimalSingleColr)
	{
		return TEXT("单颜色");
	}
	if (eType == eAnimalPrizeMode_AllColorSingleAnimal)
	{
		return TEXT("单动物");
	}
	if (eType == eAnimalPrizeMode_SysPrize)
	{
		return TEXT("彩金");
	}
	if (eType == eAnimalPrizeMode_RepeatTime)
	{
		return TEXT("重复开奖");
	}
	if (eType == eAnimalPrizeMode_Flash)
	{
		return TEXT("闪电（N倍率）");
	}
	return TEXT("未知模式");
}

const TCHAR* GetEnjoyGameNameW(int nType)
{
	eEnjoyGameType eType = (eEnjoyGameType)nType;
	if (eType == eEnjoyGameType_Zhuang)
	{
		return TEXT("[庄]");
	}
	if (eType == eEnjoyGameType_Xian)
	{
		return TEXT("[闲]");
	}
	if (eType == eEnjoyGameType_He)
	{
		return TEXT("[和]");
	}
	return TEXT("[~未知~]");
}
#endif

