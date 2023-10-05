//�ر���IT������28xin.com��
#ifndef LOCKTIME_MATCH_HEAD_FILE
#define LOCKTIME_MATCH_HEAD_FILE

#pragma once

//�����ļ�
#include "TableFrameHook.h"
#include "MatchServiceHead.h"
#include "DistributeManager.h"

//�Ƴ�����
#define REMOVE_BY_ENDMATCH			250												//��������

//ʱ�Ӷ���
#define IDI_LOCK_CONTINUE_GAME			1											//������Ϸ
#define IDI_LOCK_LEAVE_TABLE			2											//�뿪����
#define IDI_LOCK_CHECK_TABLE_START		3											//�����������Ƿ�ʼ(����п���׼���󲻿�ʼ���)
#define IDI_LOCK_ANDROID_JOIN			4											//AI���뷽ʽ
///////////////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//������ʱ��
struct tagLockMatchTimer
{
	DWORD					dwTimerID;						//��ʱ��ID
	int						iElapse;						//ִ�м����	
	WPARAM					wParam;							//���Ӳ���
	LPARAM					lParam;							//���Ӳ���
};

//����������
struct tagLockTimeTableInfo
{
	ITableFrame		*		pTableFrame;					//���ӽӿ�
	WORD					wTableID;						//���ӱ��

	BYTE					cbTotalRounds;					//������
	BYTE					cbCurRounds;					//��ǰ����
	BYTE					cbCurPlayCount;					//��ǰ����
	tagMatchRounds			hMatchRounds[MAX_MATCH_ROUNDS];	//��������
	LONGLONG				lBaseScore;						//��������	
	bool					bMatchTaskFinish;				//��ɱ�ʶ
	bool					bSwtichTaskFinish;				//������ʶ
	bool					bWaitForNextInnings;			//�ȴ��¾�
};
//////////////////////////////////////////////////////////////////////////////////////////
//���鶨��
typedef CWHArray<tagLockMatchTimer *>		CLockUserMatchTimerArray;
typedef CWHArray<tagLockTimeTableInfo *>	CLockTimeTableMgrArray;
typedef CWHArray<IServerUserItem *>			CLockMatchUserItemArray;
typedef CWHArray<DWORD>						CLockMatchAndroidIDArray;

//�ֵ䶨��
typedef CMap<DWORD, DWORD, IServerUserItem *, IServerUserItem *> CLockUserItemMap;
//��ʱ��
class CLockTimeMatch : public IGameMatchItem,public IMatchEventSink,public IServerUserItemSink
{
	//״̬����
protected:
	BYTE								m_MatchStatus;					//����״̬	
	CTime								m_TimeLastMatch;				//����ʱ��
	
	BYTE								m_cbMatchStatus;				//�����׶�
	SYSTEMTIME							m_MatchStartTime;				//������ʼ
	BYTE								m_cbCurMatchRounds;				//��ǰ����
	BYTE								m_cbTotalPlayerCount;			//��������
	
	//��������
protected:
	CDistributeManager					m_DistributeManage;             //�������

	//��������
protected:
	tagGameMatchOption *				m_pMatchOption;					//��������	
	tagLockTimeMatch *					m_pLockTimeMatch;				//��������
	tagGameServiceOption *				m_pGameServiceOption;			//��������
	tagGameServiceAttrib *				m_pGameServiceAttrib;			//��������

	//�ں˽ӿ�
protected:
	ITableFrame	**						m_ppITableFrame;				//��ܽӿ�
	ITimerEngine *						m_pITimerEngine;				//ʱ������
	IDBCorrespondManager *				m_pIDataBaseEngine;				//��������
	ITCPNetworkEngineEvent *			m_pITCPNetworkEngineEvent;		//��������

	//����ӿ�
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//���ܽӿ�
	IServerUserManager *				m_pIServerUserManager;			//�û�����
	IAndroidUserManager	*				m_pAndroidUserManager;			//��������
	IServerUserItemSink *				m_pIServerUserItemSink;			//�û��ص�
	//�洢����
protected:
	CLockUserItemMap					m_LockOnMatchUserMap;			//�����û�
	CLockMatchUserItemArray				m_LockFreeUserArray;			//�����û�
	CLockMatchAndroidIDArray			m_LockAndroidArray;				//AI
	CLockTimeTableMgrArray				m_LockTimeTableArray;			//��������
	CLockUserMatchTimerArray			m_LockMatchTimerArray;			//����ר�ö�ʱ��	
	//��������
public:
	//���캯��
	CLockTimeMatch();
	//��������
	virtual ~CLockTimeMatch(void);

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
	virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter);	

	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	

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
	//�����¼�
	virtual bool OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile);	
	//�û�����
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID);
	//�û�����
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);

	 //���ܺ���
public:
	 //��Ϸ��ʼ
	 virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	 //��Ϸ����
	 virtual bool OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	 //��Ϣ����
protected:
	//��ʾ��Ϣ
	VOID SendWaitMessage(ITableFrame *pTableFrame, WORD wChairID = INVALID_CHAIR);
	 //�û��¼�
public:
	 //��ҷ���
	 virtual bool OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem);

	 //�û��¼�
public:
	 //�û�����
	 virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //�û�����
	 virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //�û�ͬ��
	 virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//��������
protected:
	//������
	void ClearSameTableUser(DWORD dwUserID);		
	//�����û�
	void InsertSameTableUser(DWORD dwUserID,DWORD dwTableUserID);		
	//�Ƴ�����
	bool RemoveDistribute(IServerUserItem * pIServerUserItem);
	//�����û�
	bool InsertDistribute(IServerUserItem * pIServerUserItem);
	//���ͷ���
	bool SendMatchUserScore(IServerUserItem * pIServerUserItem);
	//���ͷ���
	bool SendMatchUserInitScore(IServerUserItem * pIServerUserItem);

	//��ʾ����
protected:
	//������ʾ
	bool SendSafeCardNotify(IServerUserItem * pIServerUserItem);
	//�۷���ʾ
	bool SendDeductFeeNotify(IServerUserItem * pIServerUserItem);	
	//��̭��ʾ
	bool SendEliminateNotify(IServerUserItem * pIServerUserItem);
	//�����ɹ�
	bool SendSignupSuccessNotify(IServerUserItem * pIServerUserItem);	
	//������ʾ
	bool SendReviveNotify(IServerUserItem * pIServerUserItem, BYTE cbReviveTimesed, bool bUseSafeCard);

	//У�麯��
private:
	//����У��
	bool VerifyUserEnterMatch(IServerUserItem * pIServerUserItem);
	//ʱ��У��
	bool VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType);		

	//�����¼�
private:
	//������ʼ
	VOID EfficacyStartMatch();
	//��������
    bool SendMatchInfo(IServerUserItem * pIServerUserItem = NULL, DWORD Seconds = 0);
	//ÿ�ֽ���
	bool MatchRoundsOver(int wPromotion);
	//��������
	VOID MatchOver();
	//������������ʼ����
	bool OnEventMatchStart();
	
	//���ͺ���
private:
	//�����û�����
	bool SendGroupUserScore(IServerUserItem * pIServerUserItem);
	//Ϊ���ֲ��������
	void InsertNullTable();
	//�������ݵ�һ���û�
	bool SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);
	//����һ����Ϣ
	VOID SendGroupMessage(LPCTSTR pStrMessage);
	//���ͱ�����Ϣ
	VOID SendTableMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	
	//��⺯��
protected:
	//�������
	bool CheckMatchUser();
	//������
	bool CheckMatchTaskFinish();
	//�����
	bool CheckSwitchTaskFinish();
	//��ȡ����
	WORD GetUserRank(IServerUserItem * pUserServerItem, ITableFrame *pITableFrame=NULL);
	
	//��������
protected:	
	//�������
	VOID InsertFreeGroup(IServerUserItem * pUserServerItem);
	//������Ϸ
	VOID ContinueGame(ITableFrame * pITableFrame);
	//�������
	VOID CheckTableStart(ITableFrame *pITableFrame);
	//AI����
	VOID MatchAndroidDebug(WORD wPromotion);
	//��������
	VOID RankingPromotion(VOID * pData);
	//��������
	VOID PerformDistributeTable();
	//�û�����
	VOID PerformAllUserStandUp();
	//�û�����
	VOID PerformAllUserStandUp(ITableFrame *pITableFrame);
	//���㳡��
	LONGLONG CalcMatchNo();
	//�����û�
	bool PerformDistribute();
	//����״̬
	VOID UpdateMatchStatus();
	//�л�ʱ��
	VOID SwitchMatchTimer();
	//��ֹ����
	bool IsMatchSignupEnd();
	//��ʼ����
	bool IsMatchSignupStart();	
	//��ʼ���
	DWORD GetMatchStartInterval();	
	//��������
	//VOID PerformAllUserStandUp(ITableFrame *pITableFrame);
	//���ͱ�����Ϣ
	void SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	//д�뽱��
	bool WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo);
	//��ȡ�ӿ�
	tagLockTimeTableInfo * GetTableInterface(ITableFrame * pITableFrame);

	//��ʱ������
protected:
	//����ʱ��
	VOID CaptureMatchTimer();
	//ɱ����ʱ��
	VOID AllKillMatchTimer();
	//ɱ����ʱ��
	bool KillMatchTimer(INT_PTR dwIndexID);
	//ɱ����ʱ��
	VOID KillMatchGameTimer(DWORD dwTimerID);
	//ɱ����ʱ��
	bool KillMatchTimer(DWORD dwTimerID, WPARAM wParam);
	//Ͷ�ݶ�ʱ��
	VOID PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam = 0, LPARAM lParam = 0);
	//�趨��ʱ��
	VOID SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
};

#endif