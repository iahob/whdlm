#ifndef WH_BASE64_HEAD_FILE
#define WH_BASE64_HEAD_FILE

#pragma once

//�����ļ�
#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////

//Base64����
class SERVICE_CORE_CLASS CWHBase64
{	
	//��������
public:
	//���캯��
	CWHBase64();
	//��������
	virtual ~CWHBase64();	

	//���뺯��
public:	
	//���뺯��
	static int Encode(const CHAR * pData, int nDataLen, TCHAR szOutBuffer[], int nBufferLen);	
	//���뺯��
	static int Decode(const TCHAR * pszCode, int nCodeLength, TCHAR szOutBuffer[], int nBufferLen);
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif