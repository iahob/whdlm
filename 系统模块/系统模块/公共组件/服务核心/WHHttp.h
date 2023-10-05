#ifndef WH_HTTP_HEAD_FILE
#define WH_HTTP_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////
const int STR_LEN = 2048;

//线程对象
class SERVICE_CORE_CLASS CWHHttp
{
	//函数定义
public:
	//构造函数
	CWHHttp();
	//析构函数
	virtual ~CWHHttp();

private:
	char  m_szUtf8[STR_LEN];			//utf8 数据
	char  m_szGbk[STR_LEN];				//GBK数据
	wchar_t  m_szUnicode[STR_LEN/2];	// 宽字节数据

	//接口函数
public:
	//获取状态
	TCHAR * GetRequest(TCHAR * szUrl);

	//启动线程
	TCHAR * PostRequest(const TCHAR * pszHostName, const TCHAR * pszPathName, WORD wPort, const TCHAR * pszPostData);

private:

	//Unicode 转 UTF-8
	char* UnicodeToUtf8(const wchar_t* pszUnicode);

	//UTF-8 转为 GBK2312
	char* UtfToGbk(const char* pszUtf8);

	//UTF-8 转 Unicode
	wchar_t* UTF8ToUnicode(const char*  pszUTF8);

};

//////////////////////////////////////////////////////////////////////////////////

#endif