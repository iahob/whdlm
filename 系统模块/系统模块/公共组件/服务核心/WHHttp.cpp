#include "stdafx.h"
#include "WHHttp.h"
#include "afxdialogex.h"

#include <iostream>
#include <string>
#include <afxinet.h> //������MFC CInternetSession���

//���캯��
CWHHttp::CWHHttp()
{
	//��ʼ������
	memset(m_szUtf8, 0, sizeof(m_szUtf8));
	memset(m_szGbk, 0, sizeof(m_szGbk));
	memset(m_szUnicode, 0, sizeof(m_szUnicode));
}

//��������
CWHHttp::~CWHHttp()
{
	
}

//Unicode ת UTF-8
char* CWHHttp::UnicodeToUtf8(const wchar_t* pszUnicode)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, pszUnicode, -1, NULL, 0, NULL, NULL);

	memset(m_szUtf8, 0, sizeof(m_szUtf8));
	WideCharToMultiByte(CP_UTF8, 0, pszUnicode, -1, m_szUtf8, len, NULL, NULL);
	return m_szUtf8;
}

//UTF-8 תΪ GBK2312
char* CWHHttp::UtfToGbk(const char* pszUtf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, pszUtf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, pszUtf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);

	memset(m_szGbk, 0, sizeof(m_szGbk));
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, m_szGbk, len, NULL, NULL);

	return m_szGbk;
}

//UTF-8 ת Unicode
wchar_t* CWHHttp::UTF8ToUnicode(const char*  pszUTF8)
{
	int  len = 0;

	int  unicodeLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		pszUTF8,
		-1,
		NULL,
		0);

	memset(m_szUnicode, 0, sizeof(m_szUnicode));
	::MultiByteToWideChar(CP_UTF8,
		0,
		pszUTF8,
		-1,
		(LPWSTR)m_szUnicode,
		unicodeLen);

	return  m_szUnicode;
}

//get����
TCHAR * CWHHttp::GetRequest(TCHAR * szUrl)
{
	std::string stringHttpsource;
	CInternetSession session(NULL, 0);
	CHttpFile* pHTTPFile = NULL;

	try
	{
		pHTTPFile = (CHttpFile*)session.OpenURL(szUrl);
		DWORD dwErr = GetLastError();
		int a = 0;
	}
	catch (CInternetException* m_pException)
	{
		//��׽���󣬷���������Ͽ�ʱ���ӵ����ڴ�����
		pHTTPFile = NULL;
		m_pException->m_dwError;
		m_pException->Delete();
		session.Close();
		return _T("");
	}

	//��ȡ�ļ�
	if (pHTTPFile)
	{
		char sRecived[2048] = { 0 };//��ʼ���������ڴ�����

		//����ʹ��CString�������룬unicode����ʱһ���ַ��������ֽ������룬�ڶ�������ʹ���ַ���
		pHTTPFile->ReadString((LPTSTR)sRecived, 1024);

		do
		{
			stringHttpsource += sRecived;
			memset(sRecived, 0, 2048);
			
		} while (pHTTPFile->ReadString((LPTSTR)sRecived, 1024)>0);//ѭ����ȡ���ݣ�ֱ����ȡ��������

		//�ͷ���Դ
		pHTTPFile->Close();
		delete pHTTPFile;
	}
	session.Close();
	return UTF8ToUnicode(stringHttpsource.c_str());//ת��	
}


//post����
TCHAR * CWHHttp::PostRequest(const TCHAR * pszHostName, const TCHAR * pszPathName, WORD wPort, const TCHAR * pszPostData)
{
	using namespace std;

	CInternetSession session(TEXT("WHPostRequest"));

	try
	{
		DWORD dwRet = 0;

		//����˿��� 0�� ������ַ��ȡ�˿�

		TCHAR pszHostName[1024] = { 0 };
		wsprintf(pszHostName, TEXT("%s"), pszHostName);
		if (wPort == 0)
		{
			unsigned short nUrlPort = 0;
			CString strServer;
			CString strObject;
			DWORD dwServerType, dwRet;
			AfxParseURL(pszHostName, dwServerType, strServer, strObject, nUrlPort);

			wsprintf(pszHostName, TEXT("%s"), strServer);
			wPort = nUrlPort;
		}

		CHttpConnection* pServer = session.GetHttpConnection(pszHostName, wPort);
		CHttpFile* pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, pszPathName);

		DWORD dw = GetLastError();

		CString strHeaders = TEXT("Content-Type: application/x-www-form-urlencoded"); // ����ͷ


		char sUtf8[1024] = { 0 };
		memcpy(sUtf8, UnicodeToUtf8(pszPostData), wcslen(pszPostData));

		//��ʼ��������
		pFile->SendRequest(strHeaders, (LPVOID)sUtf8, strlen(sUtf8));
		
		//����״̬
		pFile->QueryInfoStatusCode(dwRet);

		//����������
		if (dwRet == HTTP_STATUS_OK)
		{
			CHAR szContent[2048] = { 0 };
			CHAR szUTF8ToGBK[2048] = { 0 };
			CString strRetContent;

			//ѭ����ȡÿ������
			while (pFile->Read(szContent, 2048) > 0)
			{
				memcpy(szUTF8ToGBK, UtfToGbk(szContent), 2048);
				strRetContent += szUTF8ToGBK;
				ZeroMemory(szContent, 2048);
				ZeroMemory(szUTF8ToGBK, 2048);
			}
		}
		else
		{
			return false;
		}

		//�ͷ���Դ
		delete pFile;
		delete pServer;

		//�����ػ�
		session.Close();

	}
	catch (CInternetException* pEx)
	{
		//��ȡ�����ֵĴ���
		DWORD dw = GetLastError();

		//�� WinInet ��ȡ����
		TCHAR pszError[200];
		pEx->GetErrorMessage(pszError, 200);

		session.Close();

		return false;
	}
	
}

