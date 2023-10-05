#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//ϵͳ����
public:
	WORD							m_wMaxConnect;						//�������
	WORD							m_wServicePort;						//����˿�
	byte							m_cbLogonServer;					//��¼������  0 ��ʱд��־  1 ��ʱд��־
	byte							m_cbGameServer;						//��Ϸ������  0 ��ʱд��־  1 ��ʱд��־
	byte							m_cbChatServer;						//���������  0 ��ʱд��־  1 ��ʱд��־
	byte							m_cbGroupServer;					//���������  0 ��ʱд��־  1 ��ʱд��־

	//������Ϣ
public:
	TCHAR							m_szServerName[LEN_SERVER];			//��������

	//��������
public:
	//���캯��
	CInitParameter();
	//��������
	virtual ~CInitParameter();

	//���ܺ���
public:
	//��ʼ��
	VOID InitParameter();
	//��������
	VOID LoadInitParameter();
};

//////////////////////////////////////////////////////////////////////////////////

#endif
