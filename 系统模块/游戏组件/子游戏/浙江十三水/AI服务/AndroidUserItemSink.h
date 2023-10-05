#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\��Ϸ������\GameLogic.h"

#pragma warning(disable : 4244)

#define MAX_COUNT 13

//����ʱ��
#define TIME_LESS					2									//����ʱ��

//ʱ�䶨��
#define TIMER_START_GAME			5									//׼��ʱ��
#define	TIMER_SHUFFLECARD			4									//ϴ��ʱ��
#define TIMER_COMPARECARD			17									//����ʱ��
#define TIMER_RANGECARD				2									//����ʱ��
#define TIMER_DISPATCHCARDFINISH	12								//����ʱ��
//��Ϸʱ��
#define IDI_START_GAME					1									//��ʼ��ʱ��
#define IDI_RANGE_CARD					2									//���ƶ�ʱ��
#define IDI_COMPARECARD				3									//���ƶ�ʱ��
#define IDI_DISPATCHCARDFINISH	4									//���ƶ�ʱ��

//////////////////////////////////////////////////////////////////////////

//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{

	//ʱ�䶨��
protected:
	BYTE							m_cbTimeStartGame;					//��ʼʱ��
	BYTE							m_cbTimeCallBanker;					//��ׯʱ��
	BYTE							m_cbTimeSetChip;					//��עʱ��
	BYTE							m_cbTimeRangeCard;					//����ʱ��
	BYTE							m_cbTimeShowCard;					//����ʱ��

	BYTE                            m_cbUserCount;						//�������
	//��Ϸ����
protected:
	
	BYTE							m_cbHandCardData[HAND_CARD_COUNT];	//�����˿�
	BYTE							m_cbHandCardCount ;					//�˿���Ŀ
	BYTE							m_cbSegmentCard[GAME_PLAYER][3][5];	//�ֶ��˿�
	tagSegInfo						m_tagSegInfo;						//��ʱ�ֶ�

	bool							m_bFinishSegment;					//��ɷֶ�

	//�ؼ�����
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;				//�û��ӿ�
	
	//�����˴�ȡ��
	LONGLONG						m_lRobotScoreRange[2];					//���Χ
	LONGLONG						m_lRobotBankGetScore;					//�������
	LONGLONG						m_lRobotBankGetScoreBanker;				//������� (ׯ��)
	int								m_nRobotBankStorageMul;					//����
	WORD							m_wServerID;							//�����ʶ

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
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

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
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û���λ
	virtual VOID OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//��Ϣ����
protected:
	//�����˿�
	bool OnSubSendCardEx(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize) ;
	//�Ƚ��˿�
	bool OnSubCompareCard(const void * pBuffer , WORD wDataSize) ;
private:
	//�Զ�����
	void AutomaticOutCard() ;
protected:
	void GetCardIndex(BYTE cbSrcIdx[], BYTE cbDstCard[], BYTE cbDstLen);
	//������
private:
	//��ȡ����
	void ReadConfigInformation();
	//���в���
	void BankOperate(BYTE cbType);

	void WriteDebugInfo(LPCTSTR pszString);
};

//////////////////////////////////////////////////////////////////////////

#endif