#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//���ʱ��
public:
	WORD							m_wConnectTime;						//����ʱ��
	//ϵͳ����
public:
	WORD							m_wMaxConnect;						//�������
	WORD							m_wServicePort;						//����˿�
	TCHAR							m_szServerName[LEN_SERVER];			//��������

	//վ����Ϣ
public:
	WORD							m_wStationID;						//վ���ʶ
	TCHAR							m_szStationName[LEN_SERVER];		//վ������

	//Э������
public:	
	WORD							m_wCorrespondPort;					//Э���˿�
	tagAddressInfo					m_CorrespondAddress;				//Э����ַ	
	//Լս��Ϣ
public:
	WORD							m_wPrsnlRmCorrespondPort;			//Լս�˿�
	tagAddressInfo					m_PrsnlRmCorrespondAddress;			//Լս��ַ
	//�����ַ
public:
	tagAddressInfo					m_ServiceAddress;					//�����ַ
	tagDataBaseParameter			m_IMGroupDBParameter;				//���ӵ�ַ

	//AI����
public:
	WORD							m_wAIPort;							//AI�˿�
	tagAddressInfo					m_AIAddress;						//AI��ַ	

public:
	WORD							m_wLogServerPort;					//��־�������˿�
	tagAddressInfo					m_LogServerAddress;					//Լս��ַ

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

//////////////////////////////////////////////////////////////////////////

#endif
