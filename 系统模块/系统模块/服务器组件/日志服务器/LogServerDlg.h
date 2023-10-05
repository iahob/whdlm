#ifndef CORRESPOND_SERVER_DLG_HEAD_FILE
#define CORRESPOND_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////////////

//���Ի���
class CLogServerDlg : public CDialog, public IServiceUnitsSink
{
	//�������
protected:
	CServiceUnits					m_ServiceUnits;						//����Ԫ
	CTraceServiceControl			m_TraceServiceControl;				//׷�ٴ���

	int		m_nCloseLogonServerLog;		//�رյ�¼��������־
	int		m_nCloseGameServerLog;		//�ر���Ϸ��������־
	int		m_nCloseChatServerLog;		//�ر������������־
	int		m_nCloseGroupServerLog;		//�رվ��ֲ���������־

	//��������
public:
	//���캯��
	CLogServerDlg();
	//��������
	virtual ~CLogServerDlg();

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
	//�ͷ׵��Ʋ�����ϼ������
	//��¼��������ʱд��־
	VOID OnBnClickedLogonPoint();
	//��¼��������ʱд��־
	VOID OnBnClickedLogonImmediate();
	//��Ϸ��������ʱд��־
	VOID OnBnClickedGameServerPoint();
	//��Ϸ��������ʱд��־
	VOID OnBnClickedGameServerImmediate();
	//�����������ʱд��־
	VOID OnBnClickedChatPoint();
	//�����������ʱд��־
	VOID OnBnClickedChatImmediate();
	//���ֲ���ʱд��־
	VOID OnBnClickedGroupPoint();
	//���ֲ���ʱд��־
	VOID OnBnClickedGroupImmediate();
	//�رյ�¼��������־
	VOID OnBnClickedLogonClose();
	//�ر���Ϸ��������־
	VOID OnBnClickedGameServerClose();
	//�ر������������־
	VOID OnBnClickedChatClose();
	//�رվ��ֲ���������־
	VOID OnBnClickedGroupClose();

	//��Ϣӳ��
public:
	//�ر�ѯ��
	BOOL OnQueryEndSession();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif