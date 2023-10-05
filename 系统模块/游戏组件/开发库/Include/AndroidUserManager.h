#ifndef ANDROID_USER_MANAGER_HEAD_FILE
#define ANDROID_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "AndroidUserItem.h"
#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////////
//������չ
struct tagAndroidParameterEx
{
	//��������
	tagAndroidParameter					AndroidParameter;					//��������

	//��ʶ����
	bool								bIsValided;							//��Ч��ʶ
	bool								bIsLoadAndroid;						//���ػ���	
};

///////////////////////////////////////////////////////////////////////////////////////

//��������
typedef CWHArray<CAndroidUserItem *> CAndroidUserItemArray;
typedef CWHArray<tagAndroidAccountsInfo *> CAccountsInfoArray;
typedef CMap<DWORD, DWORD, CAndroidUserItem *, CAndroidUserItem *> CAndroidUserItemMap;
typedef CWHArray<tagAndroidLeaveInfo *> CLeaveInfoArray;

//////////////////////////////////////////////////////////////////////////////////

//AI����
class GAME_SERVICE_CLASS CAndroidUserManager : public IAndroidUserManager
{
	//���ñ���
protected:
	WORD							m_wAutoAndroidCount;				//�Զ���Ŀ

	//������
protected:
	WORD							m_wStockCount;						//�����Ŀ
	tagAndroidParameter				m_AndroidParameter;					//��������
	tagRoomAIParameter				m_RoomAIParameter;					//A I ���� 

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IServerUserManager *			m_pIServerUserManager;				//�û�����
	IGameServiceManager *			m_pIGameServiceManager;				//�������
	IGameServiceCustomTime *		m_pIGameServiceSustomTime;			//ʱ������
	ITCPNetworkEngineEvent *		m_pITCPNetworkEngineEvent;			//�¼��ӿ�
	IMainServiceFrame *				m_pIMainServiceFrame;				//���ȷ���
	ITableFrame	**					m_ppITableFrame;					//��ܽӿ�

	//��������
protected:	
	bool							m_bServiceContinue;					//��������
	DWORD							m_dwMinSitInterval;					//��Ϣʱ��
	DWORD							m_dwMaxSitInterval;					//��Ϣʱ��	

	//ϵͳ����
protected:
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������
	tagGameMatchOption *			m_pGameMatchOption;					//��������
	tagGameChatInfo *				m_pGameChatInfo;					//��������

	//�û�����
protected:
	CAndroidUserItemMap				m_AndroidUserItemMap;				//�û�����
	CAndroidUserItemArray			m_AndroidUserItemArray;				//�洢����
	CAndroidUserItemArray			m_AndroidUserItemBuffer;			//���ж���
	CAccountsInfoArray				m_AccountsInfoArray;				//��������
	CAccountsInfoArray				m_AccountsInfoArrayBuffer;			//���ж���
	CLeaveInfoArray					m_LeaveInfoArray;					//�뿪����

	//��������
public:
	//���캯��
	CAndroidUserManager();
	//��������
	virtual ~CAndroidUserManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

	//���ýӿ�
public:	
	//�������
	virtual bool InitAndroidUser(tagAndroidUserParameter & AndroidUserParameter);
	//�Ƴ�����
	virtual bool RemoveAndroidParameter();
	//���ò���
	virtual bool AddAndroidParameter(tagAndroidParameter AndroidParameter);
	//�������
	virtual bool InsertAndroidInfo(tagAndroidAccountsInfo AndroidAccountsInfo[], WORD wAndroidCount, tagGameChatInfo * pGameChatInfo);
	//������
	virtual bool BindTableFrame(ITableFrame * pTableFrame, WORD wTableID);

	//����ӿ�
public:
	//ɾ������
	virtual bool DeleteAndroidUserItem(DWORD dwAndroidID,bool bStockRetrieve);
	//���һ���
	virtual IAndroidUserItem * SearchAndroidUserItem(DWORD dwUserID, DWORD dwContextID);
	//��������
	virtual IAndroidUserItem * CreateAndroidUserItem(tagAndroidAccountsInfo & AndroidAccountsInfo);
	//ö�ٻ���
	virtual IAndroidUserItem * EnumAndroidUserItem(WORD wEnumIndex);
	//���ó�ʼЯ��
	virtual void SetAndroidInitialCarryScore(DWORD dwUserID, DWORD dwContextID, SCORE lInitialCarryScore);
	//������Ϸ�仯
	virtual void SetAndroidGameVariationScore(DWORD dwUserID, DWORD dwContextID, SCORE lGameVariationScore);
	//���ýӿ�
	VOID SetMainServiceFrame(IMainServiceFrame *pIMainServiceFrame);
	//��ȡ�û�
	virtual IServerUserItem * GetUserItem(WORD wTableID, WORD wChairID);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);

	//״̬�ӿ�
public:
	//������Ŀ
	virtual WORD GetAndroidCount() { return (WORD)m_AndroidUserItemMap.GetCount(); }
	//�һ�������Ŀ
	virtual WORD GetAndroidHangingCount();
	//�����Ŀ
	virtual WORD GetAndroidStock() { return m_wStockCount; }
	//���ػ���
	virtual bool GetAndroidLoadInfo(DWORD & dwLoadCount);
	//�û�״��
	virtual WORD GetAndroidUserInfo(tagAndroidUserInfo & AndroidUserInfo, DWORD dwServiceMode);
	//��ȡ��Ӧ�ȼ�����
	virtual WORD GetAndroidGradeCount(WORD wAndroidGrade);
	//��ȡ��������
	virtual tagGameServiceOption* GetGameServiceOption() { return m_pGameServiceOption; }
	//��ȡ��Ϸ����
	virtual tagGameServiceAttrib* GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��ȡ��������
	virtual tagGameMatchOption* GetGameMatchOption() { return m_pGameMatchOption; }
	//��ȡ��������
	virtual tagGameChatInfo * GetGameChatInfo() { return m_pGameChatInfo; }
	//��ȡ��������
	virtual tagAndroidParameter* GetAndroidParameter() { return &m_AndroidParameter; }
	//��ȡA I ���� 
	virtual tagRoomAIParameter* GetRoomAIParameter() { return &m_RoomAIParameter; }

	//����ӿ�
public:
	//��������
	virtual bool SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendDataToClient(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������־����
	virtual void  SendLogData(TCHAR * pszLogInfo);
	//������־����
	virtual void  SendLogData(tagLogUserInfo LogUserInfo);

	//�������
protected:
	//�ͷŶ���
	VOID FreeAndroidUserItem(DWORD dwUserID, WORD wIndex);
	//��ȡ����
	CAndroidUserItem * GetAndroidUserItem(WORD wIndex);
	//�������
	CAndroidUserItem * ActiveAndroidUserItem(tagAndroidAccountsInfo & AccountsInfo);
	//�������
	tagAndroidAccountsInfo * ActiveAndroidUserInfo(tagAndroidAccountsInfo & AccountsInfo);
	//ǿ������
	VOID ForceAndroidUserItemStandup(CAndroidUserItem * pAndroidUserItem);

	//��������
protected:	 
	//�����뿪
	VOID AnalyseAndroidItemArrayLeave();
	//�����뿪�ȼ��б�
	bool AnalyseLeaveGradeListValid(DWORD dwUserID, list<SortTop> tempList, int nTopCount);
	//��̬����һ�
	VOID DynamicRectifyUserItemArrayHanging();
	//��ʼ����
	VOID InitialAndroidIn();
	//Ч������˽���
	bool EfficacyAndroidInOut(bool bAndroidIn);
	//Ч������˵ȴ��뿪
	VOID EfficacyAndroidWaitLeave();
	//ΨһCurrentIndex
	WORD GetUniqueCurrentIndex();
};

//////////////////////////////////////////////////////////////////////////////////

#endif