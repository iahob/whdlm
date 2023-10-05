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
	BYTE cbTingCard[MAX_COUNT][MAX_COUNT - 1];								//����
	BYTE cbRemainCount[MAX_COUNT];											//�����Ƶ�ʣ����
};

//////////////////////////////////////////////////////////////////////////

//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//��������
protected:
	int								m_bTrusteeCount;							//�йܼ���
	bool							m_bTrustee;									//�йܱ�־
	bool							m_bBackGroundSound;							//��������
	bool							m_bTail;									//�Ƿ�β��
	bool							m_bTing[GAME_PLAYER];						//�Ƿ�����
	BYTE							m_cbSendCard;								//��������
	CMD_S_Hu_Data  m_stTingData;												//������Ϣ

	//��Ϸ����
protected:
	WORD							m_wSiceCount;								//���ӵ���
	WORD							m_wBankerUser;								//ׯ���û�
	WORD							m_wCurrentUser;								//��ǰ�û�
	BYTE							m_cbMagicIndex;								//��������
	BYTE							m_cbActionMask;								//��Ҷ���
	BYTE							m_cbActionCard;								//������

	//ʱ�䶨��
protected:
	BYTE							m_cbTimeOutCard;							//����ʱ��
	BYTE							m_cbTimeOperateCard;						//����ʱ��
	BYTE							m_cbTimeStartGame;							//��ʼʱ��

	//���Ʊ���
protected:
	WORD							m_wOutCardUser;								//�����û�
	BYTE							m_cbOutCardData;							//�����˿�
	BYTE							m_cbEnjoinOutCardCount;						//��ֹ������
	BYTE							m_cbEnjoinOutCard[MAX_COUNT];				//��ֹ��������
	BYTE							m_cbDiscardCount[GAME_PLAYER];				//������Ŀ
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];			//������¼

	//���Ʊ���
protected:
	BYTE							m_cbHuaCardCount[GAME_PLAYER];				//���Ƹ���
	BYTE							m_cbHuaCardData[GAME_PLAYER][MAX_HUA_COUNT];//��������

	//����˿�
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];			//�����Ŀ
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//����˿�

	//�˿˱���
protected:
	BYTE							m_cbLeftCardCount;							//ʣ����Ŀ
	BYTE							m_cbCardIndex[MAX_INDEX];					//�����˿�

	//�������
public:
	CGameLogic						m_GameLogic;								//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;						//�û��ӿ�

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
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//�û�����
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//������Ϣ
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//������ʾ
	bool OnSubOperateNotify(const void * pBuffer, WORD wDataSize);
	//�������
	bool OnSubOperateResult(const void * pBuffer, WORD wDataSize);
	//�û�����
	bool OnSubListenCard(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameConclude(VOID * pData, WORD wDataSize);
	//�û��й�
	bool OnSubTrustee(const void * pBuffer, WORD wDataSize);
	//������
	bool OnSubQsListen(const void * pBuffer, WORD wDataSize);
	//��������
	bool OnSubTingData(const void * pBuffer, WORD wDataSize);
	//��������
	bool OnSubHuData(const void * pBuffer, WORD wDataSize);

	//������
protected:
	//����
	void OnOutCard(BYTE cbOutCard, bool bSend);
	//������
	void OnOperateCard(BYTE cbOperateCode, BYTE cbOperateCard);

	//��������
protected:
	//���ط�������
	bool SearchTingCard(tagTingCardResult &TingCardResult);
	//��������
	bool SearchOutCard(tagOutCardResult &OutCardResult);
	//�Զ�����
	bool AutomatismOutCard();
};

//////////////////////////////////////////////////////////////////////////

#endif