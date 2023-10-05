#pragma once

#include <stdio.h>
#include <stdarg.h>

#include "SDError.h"
//#include "sdlogger.h"
#include "basiclogger.h"

enum ELogMode
{
	LOG_MODE_SINGLE_FILE = 0,	// Record the log in the single file
	LOG_MODE_DAY_DIVIDE,		// Record the log in different files partition by days
	LOG_MODE_MONTH_DIVIDE,		// Record the log in different files partition by month
};

#define LOG_FILE CLog::Instance()->Log
#define LOG_DEBUG CLog::Instance()->Log
#define LOG_INFO CLog::Instance()->Log
#define LOG_ERROR CLog::Instance()->Log
//
//#define LOG_FILE_NOTIME CLog::Instance()->LogWithoutTimeMark
//#define LOG_DEBUG_NOTIME CLog::Instance()->LogWithoutTimeMark
//#define LOG_INFO_NOTIME CLog::Instance()->LogWithoutTimeMark
//#define LOG_ERROR_NOTIME CLog::Instance()->LogWithoutTimeMark

class CLog
{
public:
	CLog(void):
	   m_pLog(NULL)/*,
	   m_pFileLogger(NULL)*/
	{

	};
	~CLog(void)
	{

	};
	static CLog* m_pInstance;
public:
	static CLog* Instance()
	{
		if (NULL == m_pInstance)
		{
			m_pInstance = new CLog();
		}

		return m_pInstance;
	}

	static void DestroyInstance()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
public:
	bool Init(const char* szSubDir,const char* szLogFileName);
	void UnInit();

	void Log(const char* pszFormat, ...);
	void LogWithoutTimeMark(const char* pszFormat, ...);
private:
	ISDError* m_pLog;
	CBasicLogger m_oBaseLogger;
	//ISDFileLogger* m_pFileLogger;
};

const char* GetAnimalName(int nType);
const char* GetColorName(int nType);
const char* GetModeName(int nType);
const char* GetEnjoyGameName(int nType);


