#ifndef LOCKTIME_MATCH_HEAD_FILE
#define LOCKTIME_MATCH_HEAD_FILE

#pragma once

//�����ļ�
#include "TableFrameHook.h"
#include "..\..\���������\��Ϸ������\PersonalRoomServiceHead.h"


///////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////
typedef CWHArray<tagPersonalTableParameter *> CPersonalTableParameterArray;	//˽������

//��ʱ��
class CPersonalRoomGame : public IPersonalRoomItem, public IPersonalRoomEventSink, public IServerUserItemSink
{
	//״̬����
protected:

	//Լս������
protected:
	tagPersonalRoomOption *				m_pPersonalRoomOption;				//Լս����	
	tagGameServiceOption *				m_pGameServiceOption;				//��������
	tagGameServiceAttrib *				m_pGameServiceAttrib;				//��������

	CPersonalTableParameterArray		m_PersonalTableParameterArray;		//˽������

	//�ں˽ӿ�
protected:
	ITableFrame	**						m_ppITableFrame;					//��ܽӿ�
	ITimerEngine *						m_pITimerEngine;					//ʱ������
	IDBCorrespondManager *				m_pIDBCorrespondManager;			//��������
	ITCPNetworkEngineEvent *			m_pITCPNetworkEngineEvent;			//��������
	ITCPNetworkEngine *					m_pITCPNetworkEngine;				//��������
	ITCPSocketService *					m_pITCPSocketService;				//�������	
	ITCPSocketService *					m_pAndroidITCPSocketService;		//�������
	//����ӿ�
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//���ܽӿ�
	IServerUserManager *				m_pIServerUserManager;			//�û�����
	IAndroidUserManager	*				m_pAndroidUserManager;			//��������
	IServerUserItemSink *				m_pIServerUserItemSink;			//�û��ص�

	//��������
public:
	//���캯��
	CPersonalRoomGame();
	//��������
	virtual ~CPersonalRoomGame(void);

	//�����ӿ�
public:
 	//�ͷŶ���
 	virtual VOID Release(){ delete this; }
 	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//����֪ͨ
	virtual void OnStartService();

	//����ӿ�
public:
	//������
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID);
	//��ʼ���ӿ�
	virtual bool InitPersonalRooomInterface(tagPersonalRoomManagerParameter & MatchManagerParameter);	

	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID);

	//�����¼�
public:
	//˽�˷��¼�
	virtual bool OnEventSocketPersonalRoom(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	

	//�û��ӿ�
public:
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//�û�����
	virtual bool OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason);
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID);
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind);
	//AI�һ��¼�
	virtual bool OnEventAndroidUserHangingNotify(IServerUserItem * pIServerUserItem, bool bAndroidUserHanging) { return true; }
	//AI�ȼ��¼�
	virtual bool OnEventAndroidUserGradeNotify(IServerUserItem * pIServerUserItem, WORD wAndroidGrade) { return true; }

	//�¼��ӿ�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//��¼���
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem);

	 //���ܺ���
public:
	 //��Ϸ��ʼ
	 virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	 //Լս��д������Ϣ
	 virtual void	PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID, WORD wChairID, DWORD dwKindID, DWORD dwPersonalRoomID, TCHAR * szPersonalRoomGUID);
	 //��Ϸ����
	 virtual bool OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	 //��Ϸ����
	 virtual bool OnEventGameEnd(WORD wTableID, WORD wChairCount, DWORD dwDrawCountLimit, DWORD & dwPersonalPlayCount, int nSpecialInfoLen, byte * cbSpecialInfo, SYSTEMTIME sysStartTime, tagPersonalUserScoreInfo * PersonalUserScoreInfo, BOOL bPersonalLoop, BYTE cbGameMode);
	//����֧��
	 virtual bool	PersonalPayRoomCard(DWORD dwUserID, WORD wTableID, WORD wChairCount, DWORD dwPersonalRoomID, DWORD dwGroupID);
	 //�û��¼�
public:
	 //�û�����
	 virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //�û�����
	 virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //�û�ͬ��
	 virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);


	//Լս����
protected:
	//��������
	bool OnTCPNetworkSubCreateTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem);
	//AI��������
	bool OnTCPNetworkSubAICreateTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem);
	//ȡ������
	bool OnTCPNetworkSubCancelRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem);
	//ȡ����
	bool OnTCPNetworkSubRequestReply(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem);
	//����ǿ����������
	bool OnTCPNetworkSubHostDissumeTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem);
	//������󷿼�ɼ�
	//bool OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem);
	//������Ϣ����
	bool	OnTCPNetworkSubQueryJoinInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem);

	//���ݿ���Ӧ
protected:
	//�����ɹ�
	bool OnDBCreateSucess(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//AI�����ɹ�
	bool OnDBAICreateSucess(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//����ʧ��
	bool OnDBCreateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//ȡ������
	bool OnDBCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//˽������
	bool OnDBLoadPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//��ɢ����
	bool OnDBDissumeTableResult(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//Լս����������󷿼���Ϣ
	bool OnDBQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//��ǰ�û��ķ�������Ϸ��
	bool OnDBCurrenceRoomCardAndBeant(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//Ⱥ�����
	bool OnDBCurrenceGroupFee(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//Լս�������¼�
public:
	//Լս�������¼�
	virtual bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);

};

#endif