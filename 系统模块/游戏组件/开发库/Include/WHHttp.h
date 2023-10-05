#ifndef WH_HTTP_HEAD_FILE
#define WH_HTTP_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////
const int STR_LEN = 2048;

//�̶߳���
class SERVICE_CORE_CLASS CWHHttp
{
	//��������
public:
	//���캯��
	CWHHttp();
	//��������
	virtual ~CWHHttp();

private:
	char  m_szUtf8[STR_LEN];			//utf8 ����
	char  m_szGbk[STR_LEN];				//GBK����
	wchar_t  m_szUnicode[STR_LEN/2];	// ���ֽ�����

	//�ӿں���
public:
	//��ȡ״̬
	TCHAR * GetRequest(TCHAR * szUrl);

	//�����߳�
	TCHAR * PostRequest(const TCHAR * pszHostName, const TCHAR * pszPathName, WORD wPort, const TCHAR * pszPostData);

private:

	//Unicode ת UTF-8
	char* UnicodeToUtf8(const wchar_t* pszUnicode);

	//UTF-8 תΪ GBK2312
	char* UtfToGbk(const char* pszUtf8);

	//UTF-8 ת Unicode
	wchar_t* UTF8ToUnicode(const char*  pszUTF8);

};

//////////////////////////////////////////////////////////////////////////////////

#endif