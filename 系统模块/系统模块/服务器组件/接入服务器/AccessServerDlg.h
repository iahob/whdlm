#ifndef ACCESS_SERVER_DLG_HEAD_FILE
#define ACCESS_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////

//���Ի���
class CAccessServerDlg : public CDialog, public IServiceUnitsSink
{
	//�������
protected:
	CTraceServiceControl			m_RichEditTrace;					//׷�ٴ���

	//�������
protected:	
	CServiceUnits					m_ServiceUnits;						//����Ԫ 

	//��������
public:
	//���캯��
	CAccessServerDlg();
	//��������
	virtual ~CAccessServerDlg();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ����Ϣ
	virtual VOID OnOK();
	//ȡ������
	virtual VOID OnCancel();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//����ӿ�
public:
	//����״̬
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);

	//��ť��Ϣ
protected:
	//��������
	VOID OnBnClickedStartService();
	//ֹͣ����
	VOID OnBnClickedStopService();
	//��������
	VOID OnBnClickedLoadOption();
	//���ñ���
	VOID SetWindowStatusText(LPCTSTR pszServiceStatus);

	//��Ϣӳ��
public:
	//�ر�ѯ��
	BOOL OnQueryEndSession();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif