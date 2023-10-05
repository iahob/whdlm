#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\���������\GameLogic.h"
#pragma warning(disable : 4244)
//ʱ�䶨��
#define TIMER_START_GAME			10									//׼��ʱ��
//����ʱ��
#define TIME_LESS					2									//����ʱ��
#define IDI_START_GAME				1									//��ʼ��ʱ��
#define IDI_ADD_SCORE				2									//��ע
#define IDI_SET_CHIP				3									//Я������

//////////////////////////////////////////////////////////////////////////

//AI��
class CAndroidUserItemSink : public IAndroidUserItemSink
{

	//ʱ�䶨��
protected:
	BYTE						m_cbTimeStartGame;						//��ʼʱ��
	BYTE						m_cbTimeBet;							//��ׯʱ��
	LONGLONG					m_lMinBetScore;							//��С����
	LONGLONG					m_lMaxBetScore;							//������
	LONGLONG					m_lAnteScore;							//�̶���ע
	LONGLONG					m_lCellScore;							//��ä
	BYTE						m_cbUserCount;							//�������
	//��Ϸ����
protected:
	BYTE						m_cbPlayStatus[GAME_PLAYER];			//���״̬
	LONGLONG					m_lUserMaxScore[GAME_PLAYER];			//Я������
	LONGLONG					m_lUserTableScore[GAME_PLAYER];			//��ǰ��ע����
	LONGLONG					m_lUserTotalScore[GAME_PLAYER];			//����ע����
	BYTE						m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	BYTE						m_cbCenterCardCount;					//�����˿�����
	BYTE						m_cbCenterCardData[MAX_CENTER_COUNT];	//�����˿�
	bool						m_bAndroidUser[GAME_PLAYER];			//AI���
	LONGLONG					m_lTurnLessScore;						//��С��ע
	LONGLONG					m_lTurnMaxScore;						//�����ע
	LONGLONG					m_lAddLessScore;						//��С��ע
	BYTE						m_cbFinalCardType[GAME_PLAYER];			//�������
	UserWinList					m_UserWinList;							//��������
	BYTE						m_cbCurType[GAME_PLAYER];				//��ǰ����
	BYTE						m_cbCurCardWeight;						//��ǰ��Ȩ��
	bool						m_bMustWin;								//��������Ӯ
	bool						m_bAddedScoreThisTurn;					//������������ע
	//�ؼ�����
protected:
	CGameLogic					m_GameLogic;							//��Ϸ�߼�
	IAndroidUserItem *			m_pIAndroidUserItem;					//�û��ӿ�
	//AI��ȡ��
	LONGLONG					m_lRobotScoreRange[2];					//���Χ
	LONGLONG					m_lRobotBankGetScore;					//�������
	LONGLONG					m_lRobotBankGetScoreBanker;				//������� (ׯ��)
	int							m_nRobotBankStorageMul;					//����
	WORD						m_wServerID;							//�����ʶ

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
	bool OnSubAndroidCard(const void * pBuffer, WORD wDataSize);
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//�����ע
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//�������
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//����
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize) ;

private:
	//��ȡ�Լ�
	IServerUserItem* GetMeItem(){ return m_pIAndroidUserItem->GetMeUserItem(); }
	//��ȡ�Լ���λ��
	WORD GetMeChairID();
	//���в���
	void BankOperate();
	//д��־
	void WriteDebugInfo(LPCTSTR pszString);
	//�Լ��Ƿ����Ӯ��
	bool IsMeWinner();
	//��ע
	bool AddScore(LONGLONG lscore);
	//����
	bool GiveUp();
	//��ȡ��עֵ
	LONGLONG GetAddScore();
	//�Ƿ���Ҫ����
	bool NeedGiveUp();
};

//////////////////////////////////////////////////////////////////////////

#endif