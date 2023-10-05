#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryRecord.h"
#include "GameClientView.h"
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CGameClientEngine : public CGameFrameEngine
{
	//��������
protected:
	int								m_bTrusteeCount;					//�йܼ���
	bool							m_bTrustee;							//�йܱ�־
	bool							m_bLastTurn;						//���ֱ�־
	BYTE							m_cbSortType;						//��������
	bool							m_bBackGroundSound;					//��������

	//��Ϸ����
protected:
	WORD							m_wBankerUser;						//ׯ���û�
	WORD							m_wCurrentUser;						//��ǰ�û�
	WORD							m_wMostCardUser;					//������
	WORD							m_wXuanZhanUser;				//��ս���
	WORD							m_wAskUser;							//ѯ�����
	WORD							m_wFriend[GAME_PLAYER];	//������Ϣ
	BYTE								m_cbFriendFlag;						//���ѱ��
	bool								m_bDoneAddTimes[GAME_PLAYER];	//�Ƿ���ɼӱ�����
	bool								m_bSelfDoneAddTimes;			//�Լ��Ƿ���ɼӱ�����
	//ʱ�䶨��
protected:
	BYTE							m_cbTimeStartGame;
	BYTE							m_cbTimeOutCard;					//����ʱ��
	BYTE							m_cbTimeXuanZhan;					//��սʱ��
	BYTE							m_cbTimeFindFriend;					//��ͬ��ʱ��
	BYTE							m_cbTimeAskFriend;					//��ͬ��ʱ��
	BYTE							m_cbTimeAddTimes;					//�ӱ�ʱ��

	//��������
protected:
	BYTE							m_cbSearchResultIndex;				//�����������
	tagSearchCardResult				m_SearchCardResult;					//�������

	int								m_nCurSearchType;					//��ǰ����״̬
	BYTE							m_cbEachSearchIndex;				//������������
	tagSearchCardResult				m_EachSearchResult;					//�����������

	//���Ʊ���
protected:
	BYTE							m_cbTurnCardCount;					//������Ŀ
	BYTE							m_cbTurnCardData[MAX_CARD_COUNT];	//�����б�

	//�˿˱���
protected:
	BYTE							m_cbHandCardData[MAX_CARD_COUNT];	//�����˿�
	BYTE							m_cbHandCardCount[GAME_PLAYER];		//�˿���Ŀ

	//�������
protected:
	CHistoryCard					m_HistoryCard;						//��ʷ�˿�
	CHistoryScore					m_HistoryScore;						//��ʷ����

	//�������
public:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	CGameClientView					m_GameClientView;					//��Ϸ��ͼ

	//��������
public:
	//���캯��
	CGameClientEngine();
	//��������
	virtual ~CGameClientEngine();

	//ϵͳ�¼�
public:
	//��������
	virtual bool OnInitGameEngine();
	//���ú���
	virtual bool OnResetGameEngine();

	//ʱ���¼�
public:
	//ʱ��ɾ��
	virtual bool OnEventGameClockKill(WORD wChairID);
	//ʱ����Ϣ
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);
	//��������
	virtual bool AllowBackGroundSound(bool bAllowSound);

	//��Ϸ�¼�
public:
	//�Թ���Ϣ
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//�û���ս
	bool OnSubXuanZhan(VOID * pData, WORD wDataSize);
	//�û���ͬ��
	bool OnSubFindFriend(VOID * pData, WORD wDataSize);
	//�û���ͬ��
	bool OnSubAskFriend(VOID * pData, WORD wDataSize);
	//�û��ӱ�
	bool OnSubAddTimes(VOID * pData, WORD wDataSize);
	//�û�����
	bool OnSubOutCard(VOID * pData, WORD wDataSize);
	//�û�����
	bool OnSubPassCard(VOID * pData, WORD wDataSize);
	//�û��й�
	bool OnSubTrustee(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameConclude(VOID * pData, WORD wDataSize);
	//�յ�ͬ�˹�ϵ
	bool OnSubFriend(VOID * pData, WORD wDataSize);
	//���ܺ���
public:
	//�����ж�
	bool VerdictOutCard();
	//�Զ���ʼ
	bool PerformAutoStart();
	//��ǰ�˿�
	bool SwitchToCurrentCard();
	//�Զ�����
	bool AutomatismOutCard();
	//���°�ť
	VOID UpdateButtonControl();
	//����ͬ�˱�ǩ
	VOID SetFriendFlag();
	//��������
public:
	//��������
	VOID PlayOutCardSound(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount);

	//�û��¼�
public:
	//�û�״̬
	virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser);

	//��Ϣӳ��
protected:
	//ʱ����Ϣ
	VOID OnTimer(UINT nIDEvent);
	//��ʼ��Ϣ
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//������Ϣ
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//PASS��Ϣ
	LRESULT OnPassCard(WPARAM wParam, LPARAM lParam);
	//��ʾ��Ϣ
	LRESULT OnOutPrompt(WPARAM wParam, LPARAM lParam);
	//�з���Ϣ
	LRESULT OnCallScore(WPARAM wParam, LPARAM lParam);
	//����˿�
	LRESULT OnLeftHitCard(WPARAM wParam, LPARAM lParam);
	//�����˿�
	LRESULT OnSortHandCard(WPARAM wParam, LPARAM lParam);
	//�鿴����
	LRESULT OnLastTurnCard(WPARAM wParam, LPARAM lParam);
	//�Ϲܿ���
	LRESULT OnTrusteeControl(WPARAM wParam, LPARAM lParam);
	//��ս��Ϣ
	LRESULT OnXuanZhan(WPARAM wParam, LPARAM lParam);
	//��ͬ����Ϣ
	LRESULT OnFindFriend(WPARAM wParam, LPARAM lParam);
	//��ͬ����Ϣ
	LRESULT OnAskFriend(WPARAM wParam, LPARAM lParam);
	//�ӱ���Ϣ
	LRESULT OnAddTimes(WPARAM wParam, LPARAM lParam);
	//������Ϣ
protected:
	//��������
	LRESULT OnEnableHistory(WPARAM wParam, LPARAM lParam);
	//�������
	LRESULT OnOutCardFinish(WPARAM wParam, LPARAM lParam);
	//�������
	LRESULT OnDispatchFinish(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif