#ifndef ANDROID_USER_TIEM_HEAD_FILE
#define ANDROID_USER_TIEM_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//ʱ������
struct tagTimerItem
{
	UINT							nTimerID;							//ʱ���ʶ
	UINT							nTimeLeave;							//ʣ��ʱ��
};

//��������
struct tagTableItem
{
	WORD							wTableID;							//���ӱ�ʶ
	DWORD							dwSitDownTickCount;					//����ʱ��
};

//��˵��
typedef CWHArray<tagTimerItem *>	CTimerItemArray;					//ʱ������
typedef CWHArray<tagTableItem *>	CTableItemArray;					//��������

#define INDIVIDUAL_FREE_CHATMSG_MIN				10						//���������С
#define INDIVIDUAL_FREE_CHATMSG_MAX				15						//����������
#define INTERVAL_CHATMSG_MIN					2						//��������С
#define INTERVAL_CHATMSG_MAX					5						//���������

//////////////////////////////////////////////////////////////////////////////////

//AI��Ϣ
class GAME_SERVICE_CLASS CAndroidUserItem : public IAndroidUserItem
{
	//��Ԫ����
	friend class CAndroidUserManager;

	//״̬����
protected:
	bool							m_bWaitLeave;						//�ȴ��뿪
	bool							m_bStartClient;						//��Ϸ��־	
	BYTE							m_cbGameStatus;						//��Ϸ״̬
	WORD							m_wAndroidAction;					//��������
	WORD							m_wAndroidGrade;					//�����ȼ�(Я�����뿪�ȼ�)
	WORD							m_wAndroidChatType;					//AI����
	tagUserStatus					m_CurrentUserStatus;				//�û�״̬
	bool							m_bHanging;							//�һ�״̬(������Ϸ���е�ģʽ)
	bool							m_bInitialIn;						//��ʼ����
	WORD							m_wSwitchTableRate;					//��������
	WORD							m_wFreeSameTableSec;				//ͬ������
	CTableItemArray					m_TableItemActive;					//�����

	//�󶨱���
protected:
	IServerUserItem *				m_pIServerUserItem;					//�û��ӿ�

	//״̬��Ϣ
protected:
	tagAndroidAccountsInfo			m_AndroidAccountsInfo;				//������Ϣ

	//��������
protected:
	WORD							m_wRoundID;							//ѭ������
	WORD							m_wAndroidIndex;					//��������

	//ʱ�����
protected:
	CTimerItemArray					m_TimerItemActive;					//�����
	static CTimerItemArray			m_TimerItemBuffer;					//�������

	//ʱ�����
protected:
	DWORD							m_dwMinSitInterval;					//���¼��
	DWORD							m_dwMaxSitInterval;					//���¼��
	DWORD							m_dwStandupTickCount;				//����ʱ��
	DWORD							m_dwReposeTickCount;				//��Ϣʱ��
	DWORD							m_dwLastChatMessageTickCount;		//�ϴ�����ʱ��
	DWORD							m_dwLogonFinishTickCount;			//��¼����ʱ��

	//�ӿڱ���
protected:
	IServerUserManager *			m_pIServerUserManager;				//�û�����
	IAndroidUserManager *			m_pIAndroidUserManager;				//����ӿ�
	IAndroidUserItemSink *			m_pIAndroidUserItemSink;			//�ص��ӿ�

	//��������
public:
	//���캯��
	CAndroidUserItem();
	//��������
	virtual ~CAndroidUserItem();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�û���Ϣ
public:
	//��ȡ I D
	virtual DWORD GetUserID();
	//���Ӻ���
	virtual WORD GetTableID();
	//���Ӻ���
	virtual WORD GetChairID();

	//״̬�ӿ�
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus = cbGameStatus; }
	//��ȡ�һ�
	virtual bool IsAiHanging() { return m_bHanging; }
	//���ùһ�
	virtual VOID SetAiHanging(bool bHanging);

	//������Ϣ
public:
	//��ȡA I ���� 
	virtual tagRoomAIParameter* GetRoomAIParameter() { return m_pIAndroidUserManager->GetRoomAIParameter(); }
	//��ȡ�ϴ�����ʱ��
	virtual DWORD GetLastChatMessageTickCount() { return m_dwLastChatMessageTickCount; }
	//��ȡ��¼����ʱ��
	virtual DWORD GetLogonFinishTickCount() { return m_dwLogonFinishTickCount; }
	//��ȡ���Я���ȼ�����
	virtual BYTE GetRandomCarryGradeIndex();
	//��ȡ���н��/��ʼ���
	virtual SCORE GetScoreVariationTimes();
	//���ó�ʼЯ��
	virtual void SetUserItemInitialCarryScore(DWORD dwUserID, SCORE lInitialCarryScore);
	//������Ϸ�仯
	virtual void SetUserItemGameVariationScore(DWORD dwUserID, SCORE lGameVariationScore);
	//�����뿪�ȼ�
	virtual void SetAndroidLeaveGrade(WORD wLeaveGrade);
	//���û�������
	virtual void SetAndroidAction(WORD wAndroidAction, bool bEnable) { (bEnable == true) ? m_wAndroidAction |= wAndroidAction : m_wAndroidAction &= ~wAndroidAction; }
	//����AI����
	virtual void SetAndroidChatType(WORD wAndroidChatType) { if (wAndroidChatType != m_wAndroidChatType) { m_wAndroidChatType = wAndroidChatType; } }
	//��ȡ��������
	virtual WORD GetSwitchTableRate() { return m_wSwitchTableRate; }
	//У��ͬ��
	virtual bool EfficacyAndroidFreeSameTable(WORD wTableID);

	//�û��ӿ�
public:
	//��ȡ�Լ�
	virtual IServerUserItem * GetMeUserItem();
	//��Ϸ�û�
	virtual IServerUserItem * GetTableUserItem(WORD wChariID);
	//��ȡͬ���������
	virtual	WORD GetSitRealUserCount();

	//���нӿ�
public:
	//������Ϸ��
	virtual bool PerformSaveScore(SCORE lScore);
	//��ȡ��Ϸ��
	virtual bool PerformTakeScore(SCORE lScore);

	//����ӿ�
public:
	//���ͺ���
	virtual bool SendSocketData(WORD wSubCmdID);
	//���ͺ���
	virtual bool SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�����ӿ�
public:
	//��������
	virtual bool JudgeAndroidActionAndRemove(WORD wAction);
	//�����ȼ�
	virtual bool JudgeAndroidGrade(WORD wAndroidGrade);

	//���ܽӿ�
public:
	//ɾ��ʱ��
	virtual bool KillGameTimer(UINT nTimerID);
	//����ʱ��
	virtual bool SetGameTimer(UINT nTimerID, UINT nElapse);
	//����׼��
	virtual bool SendUserReady(VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendChatMessage(DWORD dwTargetUserID);
	//���ͱ���
	virtual bool SendUserExpression(DWORD dwTargetUserID, WORD wItemIndex);
	//������־
	virtual void SendLogData(const TCHAR * pszFormat, ...);
	//������־����
	virtual void SendLogData(tagLogUserInfo LogUserInfo);

	//�¼�֪ͨ
public:
	//ʱ���¼�
	bool OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);
	//������Ϣ
	bool OnSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�����¼�
protected:
	//��¼ʧ��
	bool OnSocketSubLogonFailure(VOID * pData, WORD wDataSize);
	//��¼���
	bool OnSocketSubLogonFinish(VOID * pData, WORD wDataSize);
	//�û�״̬
	bool OnSocketSubUserStatus(VOID * pData, WORD wDataSize);
	//ϵͳ��Ϣ
	bool OnSocketSubSystemMessage(VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	bool OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//���ƺ���
protected:
	//������Ϸ
	VOID StartGameClient();
	//�ر���Ϸ
	VOID CloseGameClient();

	//��������
public:
	//��λ����
	VOID RepositUserItem();
	//Ч���������ʱ��
	bool EfficacyFreeChatMessage();
	//�����һ�״̬
	bool RectifyHangingStatus(bool& bHanging);
	//Я���ȼ�����
	BYTE GetCarryGradeIndex();
	//��ȡAI����
	virtual BYTE GetAndroidChatType() { return m_wAndroidChatType; }
	//�뿪�ȼ�����
	BYTE GetLeaveGradeIndex();
	//�޸Ļ�������
	void ModifySwitchTableRate(bool bAndroidWin);
};

//////////////////////////////////////////////////////////////////////////////////

#endif