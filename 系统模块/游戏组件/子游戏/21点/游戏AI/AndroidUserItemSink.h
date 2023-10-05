#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\���������\GameLogic.h"

//////////////////////////////////////////////////////////////////////////



//��Ϸ�Ի���
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//��ע��Ϣ
protected:
	LONGLONG						m_lTurnMaxScore;						//�����ע
	SCORE							m_lCellScore;							//��Ԫ��ע
	WORD							m_wBankerUser;							//��ǰׯ��

	//�û��˿�
protected:
	BYTE							m_HandCardData[2][MAX_COUNT];			//�û�����	
	BYTE							m_bStopCard[GAME_PLAYER*2];				//�Ƿ�ͣ��
	BYTE							m_bInsureCard[2];						//�Ƿ���
	BYTE							m_bDoubleCard[2];						//
	BYTE							m_cbCardCount[2];						//��������Ŀ
	bool							m_bWin;									//�Ƿ�Ӯ��

	//�ؼ�����
public:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;					//�û��ӿ�

	//��ϷAI��ȡ��
	LONGLONG						m_lRobotScoreRange[2];					//���Χ
	LONGLONG						m_lRobotBankGetScore;					//�������
	LONGLONG						m_lRobotBankGetScoreBanker;				//������� (ׯ��)
	int								m_nRobotBankStorageMul;					//����
	
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

	//���Խӿ�
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
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//������Ϣ
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//��ע��Ϣ
	bool OnSubAddScore( const void *pBuffer, WORD wDataSize );
	//ͣ����Ϣ
	bool OnSubStopCard( const void *pBuffer, WORD wDataSize );
	//�ӱ���Ϣ
	bool OnSubDoubleCard( const void *pBuffer, WORD wDataSize );
	//������Ϣ
	bool OnSubSplitCard( const void *pBuffer, WORD wDataSize );
	//������Ϣ
	bool OnSubInsureCard( const void *pBuffer, WORD wDataSize );
	//Ҫ����Ϣ
	bool OnSubGetCard( const void *pBuffer, WORD wDataSize );

private:
	//��ȡ����
	void ReadConfigInformation(tagCustomAndroid *pCustomAndroid);
	//���в���
	void BankOperate(BYTE cbType);
	//��������
	bool AnalyseCardOperate(BYTE cbCardType, BYTE cbLastVaule);
};

//////////////////////////////////////////////////////////////////////////

#endif
