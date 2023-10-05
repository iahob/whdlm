#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"

#include "ServerControl.h"
//////////////////////////////////////////////////////////////////////////
//��ʷ��¼
#define MAX_SCORE_HISTORY			20									//��ʷ����
//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//�û���Ϣ
protected:
	LONGLONG						m_lUserStartScore[GAME_PLAYER];		//��ʼ����
	//����ע��
protected:
	LONGLONG						m_lAllJettonScore[AREA_COUNT+1];		//ȫ����ע
	//������ע
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];	//������ע
	//�Ͼ���Ϣ				
public:
	LONGLONG						m_lLastAllJettonPlace[AREA_COUNT + 1][GAME_PLAYER]; //�Ͼ�����ע
	LONGLONG						m_lLastAllJettonArea[AREA_COUNT + 1][GAME_PLAYER];  //�������һ�β���

	//���Ʊ���
protected:
	bool							m_bRefreshCfg;							//ÿ��ˢ��
	TCHAR							m_szRoomName[32];						//���÷���
	TCHAR							m_szConfigFileName[MAX_PATH];			//�����ļ�
	LONGLONG						m_lAreaLimitScore[2];					//��������
	LONGLONG						m_lUserLimitScore;						//��������

	//��ׯ����
	LONGLONG						m_lApplyBankerCondition;				//��������
	int								m_nBankerTimeLimit;						//��������
	LONGLONG						m_lPlayerBankerMAX;						//������ׯ����
	int								m_nBankerTimeAdd;						//���Ӵ��� (��Ҵ����������ʱ)
	LONGLONG						m_lExtraBankerScore;					//�������� (���ڴ�ֵʱ������������������ׯ)
	int								m_nExtraBankerTime;						//�������

	//������
	LONGLONG						m_lStorageStart;						//�����ֵ
	LONGLONG						m_lStorageCurrent;						//�����ֵ
	int								m_nStorageDeduct;						//��ȡ����
	LONGLONG						m_lStorageMax1;							//���ⶥ1
	LONGLONG						m_lStorageMul1;							//ϵͳ��Ǯ����
	LONGLONG						m_lStorageMax2;							//���ⶥ2
	LONGLONG						m_lStorageMul2;							//ϵͳ��Ǯ����

	bool							m_bControl;								//�Ƿ����
	TCHAR							m_szControlName[LEN_NICKNAME];			//��������

	//ʱ�����
	int								m_nFreeTime;							//����ʱ��
	int								m_nPlaceJettonTime;						//��עʱ��
	int								m_nGameEndTime;							//����ʱ��

	//�����˿���
	int								m_nMaxChipRobot;						//�����Ŀ (��ע������)
	int								m_nChipRobotCount;						//����ͳ�� (��ע������)
	LONGLONG						m_lRobotAreaLimit;						//����ͳ�� (������)
	LONGLONG						m_lRobotAreaScore[AREA_COUNT+1];		//����ͳ�� (������)
	int								m_nRobotListMaxCount;					//�������

	//��ҳɼ�
protected:
	bool							m_bWinRed;
	bool							m_bWinBlack;
	bool							m_bWinPoker;
	BYTE							m_cbPokerType[2];
	LONGLONG						m_lUserWinScore[GAME_PLAYER];			//��ҳɼ�
	LONGLONG						m_lUserReturnScore[GAME_PLAYER];		//������ע
	LONGLONG						m_lUserRevenue[GAME_PLAYER];			//���˰��
	tagStatisticsRecordList			m_tagUserRecord[GAME_PLAYER];			//��������ʮ�ֵ�ͳ����Ϣ

	//�˿���Ϣ
protected:
    BYTE							m_cbTableCardArray[2][3];				//�����˿�
	BYTE							m_cbTableCard[52];						//�����˿�

	//״̬����
protected:
	DWORD							m_dwJettonTime;							//��ʼʱ��
	bool							m_bExchangeBanker;						//�任ׯ��
	WORD							m_wAddTime;								//������ׯ 

	//ׯ����Ϣ
protected:
	CWHArray<WORD>					m_ApplyUserArray;						//�������
	WORD							m_wCurrentBanker;						//��ǰׯ��
	WORD							m_wOfflineBanker;						//����ׯ��
	WORD							m_wBankerTime;							//��ׯ����
	LONGLONG						m_lBankerScore;							//
	LONGLONG						m_lBankerWinScore;						//�ۼƳɼ�
	LONGLONG						m_lBankerCurGameScore;					//��ǰ�ɼ�
	bool							m_bEnableSysBanker;						//ϵͳ��ׯ
	//ռλ
	OCCUPYSEATCONFIG				m_occupyseatConfig;									//ռλ����
	BYTE							m_cbDeskChairID[MAX_OCCUPY_SEAT_COUNT];	//ռλ����I

	//���Ʊ���
protected:
	BYTE							m_cbWinSideControl;						//������Ӯ
	int								m_nSendCardCount;						//���ʹ���

	//��¼����
protected:
	int								m_nRBCurrentColumn;
	int								m_nRBCurrentRow;
	BYTE							m_cbRBHistory[20][6];					//��·����
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//��Ϸ��¼
	int								m_nRecordFirst;							//��ʼ��¼
	int								m_nRecordLast;							//����¼
	DWORD							m_dwRecordCount;						//��¼��Ŀ

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	tagGameServiceOption			* m_pGameServiceOption;					//���ò���
	tagGameServiceAttrib *			m_pGameServiceAttrib;					//��Ϸ����

	//�������
protected:
	HINSTANCE						m_hInst;
	IServerControl*					m_pServerContro;

	//���Ա���
protected:
	static const WORD				m_wPlayerCount;							//��Ϸ����

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
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��λ�ӿ�
	virtual VOID RepositionSink();
	//���ýӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};
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
	//ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID);	

	//��Ϸ�¼�
protected:
	//��ע�¼�
	bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore);
	//����ׯ��
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//ȡ������
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);
	//�û�ռλ
	bool OnUserOccupySeat(WORD wOccupyChairID, BYTE cbOccupySeatIndex);

	//��������
private:
	//�����˿�
	bool DispatchTableCard();
	//����ׯ��
	void SendApplyUser( IServerUserItem *pServerUserItem );
	//����ׯ��
	bool ChangeBanker(bool bCancelCurrentBanker);
	//�ֻ��ж�
	void TakeTurns();
	//���ͼ�¼
	void SendGameRecord(IServerUserItem *pIServerUserItem);
	//������Ϣ
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
	//��ȡ����
	void ReadConfigInformation();
	//�Ƿ�˥��
	bool NeedDeductStorage();
	//������ע��Ϣ
	void SendUserBetInfo( IServerUserItem *pIServerUserItem );

	//��Ӯ����
protected:
	// ����Ż�
	VOID StorageOptimize();
	//����ϵͳ����������ׯʱ������ҵ÷�
	LONGLONG CalSysOrAndroidBankerScore();
	//�������������ׯʱ���˵ĵ÷�
	LONGLONG CalRealPlayerBankerScore();

	//��ע����
private:
	//�����ע
	LONGLONG GetUserMaxJetton(WORD wChairID, BYTE cbJettonArea);

	//��Ϸͳ��
private:
	//����÷�
	LONGLONG CalculateScore();
	//�ƶ�Ӯ��
	void DeduceWinner(bool &bWinRed, bool &bWinBlack, bool &bWinPoker, BYTE cbPokerType[]);

	bool ReJetton(IServerUserItem *pIServerUserItem);
};


//////////////////////////////////////////////////////////////////////////////////

#endif