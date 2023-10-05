#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ�Ի���
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//��Ϸ����
protected:
	WORD							m_wBlackUser;						//�������
	WORD							m_wCurrentUser;						//��ǰ�û�
	BYTE							XPos;								//��ǰXλ��
	BYTE							YPos;								//��ǰYλ��

//	BYTE							chess[15][15];						//����

	int								win[2][572];						//������м�������
	int								board[15][15];						//����
	bool							ptab[15][15][572];					//���ʤ�����
	bool							ctab[15][15][572];					//������ʤ�����
	//�������
protected:
	bool							m_bRestrict;						//�Ƿ����
	bool							m_bTradeUser;						//�Ƿ�Ի�
	bool							m_bDoubleChess;						//�Ƿ�˫��
	
	//�ؼ�����
public:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;				//�û��ӿ�
	ITableFrame						* m_pITableFrame;					//��ܽӿ�
	
	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() { }
	//�Ƿ���Ч
	virtual bool  IsValid() { return AfxIsValidAddress(this,sizeof(CAndroidUserItemSink))?true:false; }
	//�ӿڲ�ѯ
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool  RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool  OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool  OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool  OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//������Ϣ
	virtual bool  OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void  OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual void  OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual void  OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual void  OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û���λ
	virtual void  OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//�û�����
	bool OnSubPlaceChess(const void * pBuffer, WORD wDataSize);
	//��������
	bool OnSubRegretReq(const void * pBuffer, WORD wDataSize);
	//����ʧ��
	bool OnSubRegretFaile(const void * pBuffer, WORD wDataSize);
	//������
	bool OnSubRegretResult(const void * pBuffer, WORD wDataSize);
	//��������
	bool OnSubPeaceReq(const void * pBuffer, WORD wDataSize);
	//����Ӧ��
	bool OnSubPeaceAnser(const void * pBuffer, WORD wDataSize);
	//�����Լ�
	bool OnSubBlackTrade(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubChessManual(const void * pBuffer, WORD wDataSize);
	//��������
	void SetChess(void);
	//���÷����ʱ��
	void SetPlaceTimer(void);
	//��ʼ����
	void initGame();
	//����������
	void ComTurn();
	//�������ж�������
	int  GetCount();
};

//////////////////////////////////////////////////////////////////////////

#endif
