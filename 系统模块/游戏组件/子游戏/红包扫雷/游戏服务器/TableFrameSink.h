#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "DlgCustomRule.h"
//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	DWORD							m_dwBeginTime;					//��ʼʱ��
	LONGLONG						m_lCurSysScore;					//��ǰϵͳ��
	tagCustomRule *					m_pCustomRule;					//��������

	LONGLONG						m_lBagKey;						//��ǰ�����ʶ
	WORD							m_wBankerUser;					//ׯ��
	LONGLONG						m_lBagScore;					//ׯ�Һ������
	LONGLONG						m_lBagTotalScore[MAX_CHAIR];	//��Һ������
	BYTE							m_cbThunderPoint;				//����׺�
	BYTE							m_cbAllGetCount;				//��������
	BYTE							m_cbAIGetCount;					//AI��������
	LONGLONG						m_lSystemGameScore;				//ϵͳ�÷�
	WORD							m_wGetUser[MAX_BAG_COUNT];		//�������
	DOUBLE							m_dBagDetail[MAX_BAG_COUNT];	//������
	DOUBLE							m_dUserScore[MAX_BAG_COUNT];	//��ҵ÷�
	bool							m_bThunderPoint[MAX_BAG_COUNT];	//�Ƿ�����

	bool							m_bDebugBag;					//���Ժ��
	int								m_nLastThundeCount;				//��������
	int								m_nNoThundeCount;				//����������
	WORD							m_wRealPlayerCount;				//�������
	WORD							m_wAllPlayerCount;				//ȫ�����
	bool							m_bBagScore[MAX_BAG_COUNT];		//������
	DOUBLE							m_dBagScore[MAX_BAG_COUNT];		//�������
	bool							m_bBagThundePoint[MAX_BAG_COUNT];//���׺��
	bool							m_bDebugThunderPoint[MAX_BAG_COUNT];//��������
	DOUBLE							m_dBagBalance;					//������
	

	vector<stRedBagInfo>			m_vecRedBag;					//�����б�

	//������
protected:
	static CMap<DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo> m_mapRoomUserInfo;					//���USERIDӳ�������Ϣ
	static CList<tagRoomDebugInfo, tagRoomDebugInfo> m_listRoomDebugInfo;							//�������ӳ��
	static CList<tagHistoryRoomUserInfo, tagHistoryRoomUserInfo&> m_listHistoryRoomUserInfo;		//��ҵ��Լ�¼
	static CList<tagHistoryRoomDebugInfo, tagHistoryRoomDebugInfo&> m_listHistoryRoomDebugInfo;		//������Լ�¼
	static CList<tagHistoryRoomDebugInfo, tagHistoryRoomDebugInfo&> m_listHistorySysDebugInfo;		//ϵͳ���Լ�¼
	static DWORD					m_dwUserDebugIndex;				//��ҵ�������
	static DWORD					m_dwRoomDebugIndex;				//�����������
	static DWORD					m_dwSysDebugIndex;				//ϵͳ��������

	//�����Ա���
	EM_CTRL_DEBUG					m_emCurCtrlType;				//��ǰ���Է�ʽ
	static int						m_nUpdateStorageCount;			//���������
	static LONGLONG					m_lInitSystemStorage;			//��ʼϵͳ���
	static LONGLONG					m_lLastInitSystemStorage;		//��ʼϵͳ���
	static LONGLONG					m_lSystemStorage;				//ϵͳ���
	static LONGLONG					m_lInitUserStorage;				//��ʼ��ҿ��
	static LONGLONG					m_lLastInitUserStorage;			//��ʼ��ҿ��
	static LONGLONG					m_lUserStorage;					//��ҿ��
	static int						m_nInitParameterK;				//��ʼ����K(�ٷֱ�)
	static int						m_nParameterK;					//����K(�ٷֱ�)
	static LONGLONG					m_lSystemStorageEndScore;		//����ֹ
	static bool						m_bSystemStorageEnd;			//����ֹ
	static CTime					m_tStartDebugTime;				//����������ʱ��
	static CTime					m_tResetSystemStorage;			//�������ʱ��
	static CTime					m_tUpdateSystemStorage;			//������ʱ��
	static LONGLONG					m_lStorageCurrent;				//��ǰ��Ӯ

	//�������
protected:
	CGameLogic						m_GameLogic;					//��Ϸ�߼�
	ITableFrame	*					m_pITableFrame;					//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;			//���ò���

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release();
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual void RepositionSink();

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){ return 0; }
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	
	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};

public:
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�����¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }
	
public:
	//д��־�ļ�
	void WriteInfo(LPCTSTR pszString);

	//��Ϸ�¼�
protected:
	//��ҷ����
	bool OnUserDispatchBag(WORD wChairID, LONGLONG lScore,BYTE cbThunderPoint);
	//��������
	bool OnUserGetBag(WORD wChairID, LONGLONG lBagKey);

	//��������
protected:
	//����б�
	void SendBagList(WORD wChairID);
	//���������
	BYTE GetMaxThunderCount(DOUBLE dBagScore, BYTE cbThunderPoint, BYTE cbThunderPosition, BYTE cbBagCount);
	//���ɺ��
	bool RandAllBagDetail(IServerUserItem *pIBankerUserItem);
	//���ɺ��
	bool GetBagDetail(IServerUserItem *pIServerUserItem, DOUBLE &dBagDetail);
	//���ɺ��
	bool GetBagDetail(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE &dBagDetail);
	//����÷�
	DOUBLE GetBagUserScore(WORD wChairID, DOUBLE dBagDetail, bool bThunderPoint);
	//�޸����׷�
	bool UpdateThunderInfo(bool bThunderPoint, DOUBLE &dBagDetail);
	//�޸����׷�
	bool UpdateThunderInfo(bool bThunderPoint, DOUBLE dBagBalance, DOUBLE &dBagDetail);
	//������ʾ
	void SendDebugText(IServerUserItem * pISendUserItem, TCHAR * pText, ...);
	//���·����û���Ϣ
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, EM_USER_ACTION emUserAction, LONGLONG lGameScore = 0LL);
	//��ȡ��ҵ�������
	EM_CTRL_DEBUG GetUserCtrlType(IServerUserItem *pIServerUserItem);
	//ȡ���н�����
	WORD GetReturnPer(IServerUserItem *pIServerUserItem, LONGLONG &lSystemStorage, LONGLONG &lUserStorage);
	//���µ��Խ��
	void UpdateCtrlRes(IServerUserItem *pIServerUserItem, SCORE lTotalGameScore);
	//ȡ�÷����û���Ϣ
	bool GetRoomDebugInfo(DWORD dwDebugIndex, tagRoomDebugInfo &RoomDebugInfo);
	//���÷����û���Ϣ
	bool SetRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo);
	//ȡ�÷����û���Ϣ
	bool GetFirstRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo);
	//ɾ�������û���Ϣ
	bool DelFirstRoomDebugInfo(DWORD dwDebugIndex);
	//���Ϳ����Ϣ
	bool SendBaseStorageInfo(IServerUserItem *pIServerUserItem);
	//������ҵ���
	bool SendUserDebugInfo(IServerUserItem *pIServerUserItem);
	//������ҵ���
	bool SendUserDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
	//���ͷ������
	bool SendRoomDebugInfo(IServerUserItem *pIServerUserItem);
	//���ͷ������
	bool SendRoomDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
	//����ϵͳ����
	bool SendSysDebugInfo(IServerUserItem *pIServerUserItem);
	//����ϵͳ����
	bool SendSysDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
};

//////////////////////////////////////////////////////////////////////////

#endif
