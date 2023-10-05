#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE
#define	CARD_DISPATCHER_CONTROL

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "GameVideo.h"

#pragma warning(disable : 4244)//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��Ϸ����
protected:
	WORD							m_wBankerUser;						//ׯ���û�
	WORD							m_wCurrentUser;						//��ǰ���
	WORD							m_wBeforWiner;						//�Ͼ�Ӯ��
	BYTE							m_bBombCount[3];					//ը����Ŀ
	LONGLONG						m_lBombScore[GAME_PLAYER];			//ը����
	LONGLONG						m_lGameScore[3];					//��Ϸ�÷�
	LONGLONG						m_lCellScore;
	bool							m_bAutoPlayer[GAME_PLAYER];			//�й����
	BYTE							m_cbPlayStatus[GAME_PLAYER];		//��Ϸ״̬
	CList<BYTE, BYTE&>				m_llistRemainCard;					//ʣ���˿��б�
	BYTE							m_cbTrusteeDelayTime;				//�й��ӳ�ʱ��
	CTime							m_ctStatusStartTime;				//״̬��ʼʱ��
	BYTE							m_cbStatusConfigTime;				//״̬����ʱ��

	//�����淨
	bool							m_bSixTheenCard;					//16����
	bool							m_bNoForceClose;					//����ǿ��
	bool							m_bNoRemoveBomb;					//ը�����ɲ�
	bool							m_bSpadeBanker;						//����3ׯ
	bool							m_bFirstOutSpade;					//�׳��ش�����3(��������)
	WORD							m_wPlayerCount;						//��Ϸ����

	bool							m_bForceOutCard;					//�д�س�
	bool							m_bEchelonConclude;					//�ݶȽ���
	BYTE							m_cbSpadeBankerFirstOutCardData;
	BYTE							m_cbWinBankerFirstOutCardData;
	//
	CMD_S_GameEnd					m_stGameEnd;						//�������ݻ���

	//��Ϸ��Ƶ
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;

	CMD_S_RoomCardRecord			m_RoomCardRecord;					//������Ϸ��¼
	CList<SCORE, SCORE&>			m_listWinScoreRecord[GAME_PLAYER];

	//��ʷ����
protected:
	LONGLONG                         m_lAllTurnScore[3];				//�ֵܾ÷�
	LONGLONG                         m_lLastTurnScore[3];				//�Ͼֵ÷�

	//������Ϣ
protected:
	WORD							m_wBaoPeiUser;						//�������
	WORD							m_wTurnWiner;						//ʤ�����
	BYTE							m_bTurnCardType;					//��������
	BYTE							m_bTurnCardCount;					//������Ŀ
	BYTE							m_bTurnCardData[16];				//�����б�

	//�˿���Ϣ
protected:
	BYTE							m_bCardCount[3];					//�˿���Ŀ
	BYTE							m_bHandCardData[3][16];				//�����˿�

	//�������
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;					//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;				//���ò���
	CHistoryScore					m_HistoryScore;						//��ʷ�ɼ�
	tagCustomRule*					m_pGameCustomRule;					//�Զ�����

#ifdef  CARD_DISPATCHER_CONTROL
	bool							m_bConfigCard;
	BYTE							m_cbConfigItemInfo[48];
#endif

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual VOID RepositionSink();

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
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
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
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//��Ϸ�¼�
protected:
	//�û�����
	bool OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount, bool bClient = false);
	//�û�����
	bool OnUserPassCard(WORD wChairID);;
	//��ʼ��ʣ���б�
	void InitializeListRemainCard();
	//��ȡʣ������
	void GetRemainCard(REMAINCARDCOUNT &remaincard);
	//�жϷ�������
	bool IsRoomCardType();

	//
public:
	void StartTimer(DWORD dwTimerID, WPARAM wBindParam = 0);
	void StopTimer(DWORD dwTimerID);
	void KillAllTimer();
	//��ȡ��ʱ������ʱ��
	int GetTimerConfigTime(DWORD dwTimerID);
	//��ȡ״̬ʣ��ʱ��
	BYTE GetStatusLeftTime();
};

//////////////////////////////////////////////////////////////////////////////////

#endif