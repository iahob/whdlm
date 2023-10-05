#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\ȫ�ֶ���\RightDefine.h"


//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"

//ƽ̨�ļ�
#include "..\..\���������\�ں�����\KernelEngineHead.h"

#include "InitParameter.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"
//////////////////////////////////////////////////////////////////////////
//��������

#define LOGIC_COUNT					500										//�߼�����
#define GROUP_COUNT					500										//Ⱥ������
#define BATTLE_COUNT				500										//Լս����

//////////////////////////////////////////////////////////////////////////
//����״̬
enum enServiceStatus
{
	ServiceStatus_Stop,				//ֹͣ״̬
	ServiceStatus_Config,			//����״̬
	ServiceStatus_Service,			//����״̬
};
//////////////////////////////////////////////////////////////////////////
//�����ʶ
#define NETWORK_CORRESPOND			1										//Э������
#define NETWORK_BATTLE				2										//Լս����
#define NETWORK_LOG_SERVER			3										//��־����������
#define NETWORK_AI_SERVER			6										//AI����������

//�����ʶ
#define NETWORK_LOGIC_START			(NETWORK_CORRESPOND+1)					//�߼�����
#define NETWORK_LOGIC_END			(NETWORK_LOGIC_START+LOGIC_COUNT)		//�߼�����

//�����ʶ
#define NETWORK_GROUP_START			(NETWORK_LOGIC_END+1)					//Ⱥ������
#define NETWORK_GROUP_END			(NETWORK_GROUP_START+GROUP_COUNT)		//Ⱥ������

//�����ʶ
#define NETWORK_BATTLE_START		(NETWORK_GROUP_END+1)					//Լս����
#define NETWORK_BATTLE_END			(NETWORK_BATTLE_START+BATTLE_COUNT)		//Լս����
//////////////////////////////////////////////////////////////////////////////////
//��Ϣ����
#define WM_UICONTROL_REQUEST		(WM_USER+100)						//��������
//////////////////////////////////////////////////////////////////////////////////
//״̬�ӿ�
interface IServiceUnitsSink
{
	//�ӿڶ���
public:
	//����״̬
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus) = NULL;
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef CWHArray<DWORD>	CRoomIDArray;
typedef CMap<DWORD, DWORD, CRoomIDArray *, CRoomIDArray *> CMapGroupRoom;

//����Ԫ
class CServiceUnits : public CWnd
{
	//��Ԫ����
	friend class CAttemperEngineSink;
	friend class CDataBaseEngineSink;

	//״̬����
protected:
	//tagAccessOption					m_AccessOption;						//��������
	CInitParameter					m_InitParameter;					//���ò���
	enServiceStatus					m_ServiceStatus;					//����״̬

	//�������
protected:
	CAttemperEngineSink				m_AttemperEngineSink;				//���ȹ���
	CDataBaseEngineSink				m_DataBaseEngineSink[4];			//���ݹ���

	//�������
private:
	CWHDataQueue					m_DataQueue;						//���ݶ���
	CCriticalSection				m_CriticalSection;					//ͬ������

	//�ں����
protected:
	CTimerEngineHelper				m_TimerEngine;						//ʱ������
	CAttemperEngineHelper			m_AttemperEngine;					//��������
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//��������
	CTCPSocketServiceHelper			m_TCPSocketCorrespond;				//Э������
	CTCPSocketServiceHelper			m_TCPSocketBattle;					//״̬����	
	CDataBaseEngineHelper			m_DataBaseEngine;					//��������
	CTCPSocketServiceHelper			m_LogServerTCPSocketService;		//��־����
	CTCPSocketServiceHelper			m_pAITCPSocketService;				//AI��������
	CMapGroupRoom					m_Map;
	//�ӿڱ���
protected:
	IServiceUnitsSink *				m_pIServiceUnitsSink;				//״̬�ӿ�

	//��̬����
public:
	static CServiceUnits *			g_pServiceUnits;					//����ָ��

	//��������
public:
	//���캯��
	CServiceUnits();
	//��������
	virtual ~CServiceUnits();

	//��Ϣ����
public:
	//��ȡ״̬
	virtual enServiceStatus __cdecl GetServiceStatus() { return m_ServiceStatus; }

	//�������
public:
	//��������
	virtual bool __cdecl StartService();
	//ֹͣ����
	virtual bool __cdecl ConcludeService();
	//���ýӿ�
	virtual bool __cdecl SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink);
	//Ͷ������
	virtual bool __cdecl PostControlRequest(WORD wIdentifier, VOID *pData, WORD wDataSize);

	//��������
protected:
	//�������
	bool InitializeService();
	//�����ں�
	bool StartKernelService();
	//��������
	bool StartNetworkService();
	//�������ݿ�
	bool StartDataBaseService();
	//�ڲ�����
private:
	//����״̬
	bool SetServiceStatus(enServiceStatus ServiceStatus);
	//���Ϳ���
	bool SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize);

	//���ܺ���
public:
	//���ò���
	const CInitParameter * GetInitParameter() const { return &m_InitParameter; }

	//��Ϣӳ��
protected:
	//������Ϣ
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif