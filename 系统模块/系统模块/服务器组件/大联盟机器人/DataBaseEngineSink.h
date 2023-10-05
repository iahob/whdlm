#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//���ݿ���
class CDataBaseEngineSink : public IDataBaseEngineSink, public IGameDataBaseEngine
{
	//��Ԫ����
	friend class CServiceUnits;


	//��Ϸ���ݿ�
protected:
	CDataBaseAide					m_AccountDBAide;					//��Ϸ����
	CDataBaseHelper					m_AccountDBModule;					//��Ϸ����

	//��Ϸ�����ݿ�
protected:
	CDataBaseAide					m_TreasureDBAide;					//��Ϸ�����ݿ�
	CDataBaseHelper					m_TreasureDBModule;					//��Ϸ�����ݿ�

	//ƽ̨���ݿ�
protected:
	CDataBaseAide					m_PlatformDBAide;					//ƽ̨���ݿ�
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�

	//���ñ���
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagDataBaseParameter *			m_pDataBaseParameter;				//������Ϣ
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//�������
protected:
	IDataBaseEngine *				m_pIDataBaseEngine;					//����ӿ�
	IGameServiceManager *			m_pIGameServiceManager;				//�������
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//�����¼�

	//��ѯ�ӿ�
protected:
	IGameDataBaseEngineSink *		m_pIGameDataBaseEngineSink;			//���ݽӿ�

	//�������
public:
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //����Э��

	//��������
public:
	//���캯��
	CDataBaseEngineSink();
	//��������
	virtual ~CDataBaseEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ò���
public:
	//�Զ�����
	virtual VOID * GetCustomRule() { return m_pGameServiceOption->cbCustomRule; };
	//��������
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��������
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//��ȡ����
public:
	//��ȡ����
	virtual VOID * GetDataBase(REFGUID Guid, DWORD dwQueryVer);
	//��ȡ����
	virtual VOID * GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer);

	//���ܽӿ�
public:
	//Ͷ�ݽ��
	virtual bool PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��¼����
protected:
	//AI����
	bool OnRequestAIParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//����AI����
	bool OnRequestLoadAIParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//����AI���
	bool OnRequestLoadAIUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�ͷ�AI���
	bool OnRequestUnLockAIUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//AI�˳����ֲ�
	bool OnRequestAIQuitGroup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//���ص���AI���
	bool OnRequestLoadSingleAI(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������AI����
	bool OnRequestLoadAllAIOption(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//AI �������
	void OnAIOperateResult(DWORD dwContextID, bool bOperateSucess, DWORD dwErrorCode, WORD wOperateCode, LPCTSTR pszDescribeString);
	//����AI
	bool OnAIOperateClear(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);


	//��������
private:
	//������־
	void SendLogData(const TCHAR * pszFormat, ...);
};

//////////////////////////////////////////////////////////////////////////////////

#endif