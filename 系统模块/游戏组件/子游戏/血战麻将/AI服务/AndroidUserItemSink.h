#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//���ƽ��
struct tagOutCardResult
{
	BYTE							cbOperateCode;							//���ƴ���
	BYTE							cbOperateCard;							//����ֵ
};

//���Ʒ������
struct tagTingCardResult
{
	BYTE cbAbandonCount;													//�ɶ�������
	BYTE cbAbandonCard[MAX_COUNT];											//�ɶ�����
	BYTE cbTingCardCount[MAX_COUNT];										//������Ŀ
	BYTE cbTingCard[MAX_COUNT][MAX_COUNT-1];								//����
	BYTE cbRemainCount[MAX_COUNT];											//�����Ƶ�ʣ����
};

//////////////////////////////////////////////////////////////////////////

//AI��
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//��Ϸ����
protected:
	bool							m_bCheatMode;						//�����ױ�ʶ
	bool							m_bHuanSanZhang;					//�����ű�ʶ
	bool							m_bStartOutCard;					//��ʼ����

	//�û�����
protected:
	WORD							m_wBankerUser;						//ׯ���û�
	WORD							m_wCurrentUser;						//��ǰ�û�
	BYTE							m_cbActionMask;						//��Ҷ���
	BYTE							m_cbActionCard;						//�����˿�
	LONG							m_lSiceCount;						//���ӵ���
	TCHAR							m_szAccounts[GAME_PLAYER][32];		//�û�����
	IClientUserItem					*m_pIClientUserItemArray[GAME_PLAYER];

	//ȱ����Ϣ
protected:
	bool							m_bCallCard[GAME_PLAYER];			//�û�״̬
	BYTE							m_cbCallCard[GAME_PLAYER];			//�û�ȱ��

	//�˿˱���
protected:
	BYTE							m_cbLeftCardCount;					//ʣ����Ŀ
	BYTE							m_cbCardIndex[MAX_INDEX];			//�����˿�

	//����˿�
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];		//�����Ŀ
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][4];	//����˿�

	//������Ϣ
protected:
	BYTE							m_cbLastcbOutCard;					//�Լ�������
	WORD							m_wOutCardUser;						//�����û�
	BYTE							m_cbOutCardData;					//�����˿�
	BYTE							m_cbEnjoinOutCardCount;				//��ֹ������
	BYTE							m_cbEnjoinOutCard[MAX_COUNT];		//��ֹ��������
	BYTE							m_cbDiscardCount[GAME_PLAYER];		//������Ŀ
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];	//������¼
	BYTE							m_cbFirstSendCardData;				//��һ�η����˿ˣ�ׯ����Ч

	//ʱ����Ϣ
	BYTE							m_cbTimeOutCard;					//����ʱ��
	BYTE							m_cbTimeOperateCard;				//����ʱ��
	BYTE							m_cbTimeStartGame;					//��ʼʱ��

	//�������
public:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;				//�û��ӿ�

	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û���λ
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pData, WORD wDataSize);
	//�û�ѡȱ
	bool OnSubCallCard(const void * pData, WORD wDataSize);
	//ׯ����Ϣ
	bool OnSubBankerInfo(const void * pData, WORD wDataSize);
	//�û�����
	bool OnSubOutCard(const void * pData, WORD wDataSize);
	//������Ϣ
	bool OnSubSendCard(const void * pData, WORD wDataSize);
	//������ʾ
	bool OnSubOperateNotify(const void * pData, WORD wDataSize);
	//�������
	bool OnSubOperateResult(const void * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pData, WORD wDataSize);
	//�û��й�
	bool OnSubTrustee(const void * pData,WORD wDataSize);
	//�û��Ժ�
	bool OnSubUserChiHu( const void *pData,WORD wDataSize );
	//�û�����
	bool OnSubGangScore( const void *pData, WORD wDataSize );
	//�û������Ž��
	bool OnSubChangeCardResult(const void * pData, WORD wDataSize);
	//��ˮ���
	bool OnSubViewChart(const void * pData, WORD wDataSize);

	//������
protected:
	//��ȱ
	void CallCard();
	//����
	void ChangeCard();	
	//����
	void OnOutCard( BYTE cbOutCard, bool bSend );
	//������
	void OnOperateCard( BYTE cbOperateCode,BYTE cbOperateCard );

	//��������
protected:
	//���ط�������
	bool SearchTingCard( tagTingCardResult &TingCardResult );
	//��������
	bool SearchOutCard( tagOutCardResult &OutCardResult );
	//�Զ�����
	bool AutomatismOutCard();
};

//////////////////////////////////////////////////////////////////////////

#endif