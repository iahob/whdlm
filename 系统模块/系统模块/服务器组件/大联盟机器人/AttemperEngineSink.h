#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "ServerListManager.h"
#include "DataBasePacket.h"
#include "AIManager.h"

//////////////////////////////////////////////////////////////////////////////////


//�󶨲���
struct tagBindParameter
{
	DWORD					dwSocketID;									//�����ʶ
	WORD					wServiceID;									//�����ʶ
	WORD					wServiceType;								//��������
};


typedef CWHArray<DWORD> CArrayNewGroup;
//////////////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bCollectUser;						//���ܱ�־
	bool							m_bNeekCorrespond;					//Э����־
	bool							m_bNeekPrsnlRmCorrespond;		    //Լս��־
	bool							m_bNeekLogServer;					//��־��־

	//���Ʊ���
protected:
	DWORD							m_dwIntervalTime;					//���ʱ��
	DWORD							m_dwLastDisposeTime;				//����ʱ��
	TimeChange						m_TimeChange;						//ʱ��ı�
	DWORD							m_dwGroupSocketID;					//������SocketID
	CArrayNewGroup					m_arrayNewGroup;					//�������ֲ�

	//����Ϣ
protected:
	tagBindParameter *				m_pNormalParameter;					//����Ϣ


	//������Ϣ
protected:
	CInitParameter *				m_pInitParameter;					//���ò���

	//AI����
protected:
	CAIManager						m_AIManager;						//AI����


	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IAttemperEngine *				m_pIAttemperEngine;					//��������
	ITCPSocketService *				m_pITCPSocketService;				//�������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	IGameServiceManager *			m_pIGameServiceManager;				//�������
	IGameServiceCustomTime *		m_pIGameServiceSustomTime;			//ʱ������
	ITCPSocketService *				m_pPrsnlRmITCPSocketService;		//Լս�����������
	ITCPSocketService *				m_pLogServerTCPSocketService;		//��־��������

	//��������
public:	
	IPersonalRoomServiceManager		* m_pIPersonalRoomServiceManager;	//Լս�������
	//��������
public:
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//��������
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//��������
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //����Э��

	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//�Զ��¼�
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��Ϣ�ӿ�

	//����ӿ�
public:
	//��������
	virtual bool SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������־
	virtual void SendLogData(IServerUserItem * pIServerUserItem, const TCHAR * pszFormat, ...);
	//������־����
	virtual void SendLogData(tagLogUserInfo LogUserInfo);
	//������־����
	virtual void  SendLogData(const TCHAR * pszFormat, ...);
	//������־����
	virtual void SendLogData(IServerUserItem * pIServerUserItem, const TCHAR szDescName[LEN_SERVER], const TCHAR * pszFormat);

	//���ܽӿ�
public:
	//�Ͽ�Э��
	virtual bool DisconnectCorrespond();	
	//�����û�
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem);



	//�����¼�
protected:

	//AI����
	bool OnDBGroupAIParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//AI��������
	bool OnDBGroupAIParameterEnd(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����AI���
	bool OnDBLoadAIUser(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����AI��ҽ���
	bool OnDBLoadAIUserEnd(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����AIʱ��仯
	bool OnDBLoadAITimeChange(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//AI���뷿��Ȩ��
	bool OnDBLoadAITimeWeightIn(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//AI�뿪���ֲ�Ȩ��
	bool OnDBLoadAITimeWeightOut(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//AI�������
	bool OnDBAIOperateResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����AI���
	bool OnDBClearAIResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	
	//�����¼�
protected:
	//�û�����
	bool OnTCPNetworkService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������ע��
	bool OnTCPNetworkSubRegister(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���ֲ�����
	bool OnTCPNetworkSubGroupGameRule(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ע���¼�
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��վ�¼�
	bool OnTCPSocketMainWebMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//���ֲ�����ע��
	bool OnTCPNetworkSubGroupRegister(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���¾��ֲ��淨
	bool OnTCPNetworkSubUpdateGroupGameRule(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���¾��ֲ���ҷ���
	bool OnTCPNetworkSubUpdateGroupUserScore(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//AI�����¼�
protected:
	//AI����
	bool OnTCPNetworkAIAction(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//AI ����
	bool OnTCPNetworkSubAIParameter(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���䴴���ɹ�
	bool OnTCPNetworkSubAIPrsnlRoomCreateSuccess(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//AI ���뷿����
	bool OnTCPNetworkSubAILogonResult(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���� AI ״̬
	bool OnTCPNetworkSubAIUpdateStatus(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���� AI ��������
	bool OnTCPNetworkSubAITableChange(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//AI����ʧ��
	bool OnTCPNetworkSubAISitFailure(VOID * pData, WORD wDataSize, DWORD dwSocketID);


	//ִ�й���
protected:
	//�����û�
	bool PerformDistribute();	

	//���ͺ���
protected:
	//�û���Ϣ
	bool SendUserInfoPacket(IServerUserItem * pIServerUserItem,DWORD dwSocketID,bool bSendAndroidFalg=false);


	//��������
public:
	//�󶨲���
	virtual tagBindParameter * GetBindParameter(WORD wBindIndex);

	//��������
protected:
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif