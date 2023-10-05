#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��Ϸ����
protected:
	bool								m_b2BiggestMode;								//�Ƿ�2���
	BYTE								m_cbFriendFlag;									//ͬ�˱�ǣ�0δ֪��1��ս��2������3������ʾ��4������5ѡ���ѣ�6ѡ������ʾ
	BYTE								m_cbSelectedCardData;						//�������ѡ�����
	WORD							m_wXuanZhanUser;							//��ս���
	WORD							m_wAskUser;										//ѯ�����
	bool								m_bAddTimes[GAME_PLAYER];			//�Ƿ�ӱ�
	bool								m_bAskFriend[GAME_PLAYER];			//�Ƿ�ѯ��ͬ��
	int									m_cbGameScore[GAME_PLAYER];		//��Ϸ�÷�
	BYTE								m_cbBaseTimes;								//��������
	WORD							m_wFriend[GAME_PLAYER];				//����
	BYTE								m_cbAskStatus[GAME_PLAYER];
	BYTE								m_cbCurTurnScore;							//��ǰ�ַ���
	BYTE								m_cbFinishUserCount;
	WORD							m_wFinishUser[GAME_PLAYER];
	bool								m_bWinFlag[GAME_PLAYER];				//��Ӯ��ʶ
	bool								m_bShuangKou;									//˫�ۼӱ�
	bool								m_bTrustee[GAME_PLAYER];				//�й�

	bool								m_bGameFinish;								//������־
protected:
	WORD							m_wBankerUser;						//ׯ���û�
	WORD							m_wCurrentUser;						//��ǰ���
	bool								m_bCurrenUserDone;					//��ǰ����Ƿ���
	BYTE								m_cbOutCardCount[GAME_PLAYER];		//���ƴ���
	BYTE								m_cbWaitTime;						//�ȴ�ʱ��

	tagSearchCardResult				m_SearchCardResult;					//�������
	
	//������Ϣ
protected:
	WORD						m_wTurnWiner;						//ʤ�����
	BYTE							m_cbTurnCardCount;					//������Ŀ
	BYTE							m_cbTurnCardData[MAX_CARD_COUNT];	//��������

	//�˿���Ϣ
protected:
	BYTE							m_cbHandCardCount[GAME_PLAYER];		//�˿���Ŀ
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_CARD_COUNT];//�����˿�

	//�������
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	CHistoryScore					m_HistoryScore;						//��ʷ�ɼ�

	//����ӿ�
protected:
	ITableFrame	*					m_pITableFrame;						//��ܽӿ�
	tagCustomRule *					m_pGameCustomRule;					//�Զ�����
	tagGameServiceOption *			m_pGameServiceOption;				//��Ϸ����
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��Ϸ����

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

	//����ӿ�
public:
	//��λ����
	virtual VOID RepositionSink();
	//��������
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

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
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//����ӿ�
public:
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore);
	//��Ϸ�¼�
protected:
	//�û���ս
	bool OnUserXuanZhan(WORD wChairID, bool bXuanZhan);
	//�û���ͬ��
	bool OnUserFindFriend(WORD wChairID, BYTE cbCardData);
	//�û���ͬ��
	bool OnUserAskFriend(WORD wChairID,bool bAsk);
	//�û��ӱ�
	bool OnUserAddTimes(WORD wChairID, bool bAddTimes);
	//�û�����
	bool OnUserPassCard(WORD wChairID,bool bTrusteePass=false);
	//�û�����
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount,bool bTrusteeOut=false);
	//�û��й�
	bool	OnUserTrustee(WORD wChairID,bool bTrustee);
	//�û�����
	bool OnUserFlee(WORD wChairID);
	//�Զ�����
	bool AutomatismOutCard();
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif