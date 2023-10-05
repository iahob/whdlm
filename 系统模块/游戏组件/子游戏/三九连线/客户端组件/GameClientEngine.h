#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "GameFrameViewDexter.h"

//////////////////////////////////////////////////////////////////////////////////

// �н���Ϣ
struct tagAwardInfo 
{
	tchar						szPlayName[40];											// �������
	int							nMultipleIndex;											// �н���Ϣ
	__int64						nAwardScore;											// �н�����
};

// ��Ϸ����
class CGameClientEngine : public CGameFrameEngine, public IClientControlCallback
{
	// ��Ϣ����
protected:
	bool							m_bLegalOne;						// ��ʶ
	bool							m_bLagalTwo;						// ��ʶ

	// ������Ϣ
protected:
	bool							m_bPlayGame;						// ��Ϸ��
	bool							m_bPlayHosted;						// ����й�

	// ��ע��Ϣ
protected:
	int								m_nBetIndex;						// ��ע����
	int								m_nBetScore[BET_INDEX_MAX];			// ��ע����
	bool							m_bBetLine[LINE_INDEX_MAX];			// ��ע����

	// Ӯ��Ϣ
protected:
	longlong						m_lPlayWinScore;						// ���Ӯ����
	int								m_nPlayTaskScore;						// �������
	int								m_nPlayMultipleIndex;					// ����н�����
	bool							m_bMagnification;						// �󱶷���
	bool							m_bTaskComplete[TASK_INDEX_MAX];		// �������
	bool							m_bPlayWinLine[LINE_INDEX_MAX];			// ���Ӯ����
	int								m_nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];// Ԫ����Ϣ

	// ��¼��Ϣ
protected:
	CWHArray< tagAwardInfo >		m_ArrayAwardInfo;					// �н���Ϣ

	// �������
protected:
	CGameClientView	&				m_GameClientView;					// ��Ϸ����
	CGameFrameViewDexter			m_GameFrameViewDexter;				// ���ڿ��

	// �������
public:
	hinstance						m_hControlInst;
	IClientControl *				m_pIClientControl;

	// ��������
public:
	// ���캯��
	CGameClientEngine();
	// ��������
	virtual ~CGameClientEngine();

	// ϵͳ�¼�
public:
	// ��������
	virtual bool OnInitGameEngine();
	// ���ú���
	virtual bool OnResetGameEngine();

	// ʱ���¼�
public:
	// ʱ��ɾ��
	virtual bool OnEventGameClockKill(WORD wChairID);
	// ʱ����Ϣ
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);

	// ��Ϸ�¼�
public:
	// �Թ���Ϣ
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	// ��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	// ������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	// ���ƺ���
public:
	// ������Ϣ
	virtual bool SendControlMessage( uint nMessageID, void * pData, uint nSize );

	// ϵͳ��Ϣ
public:
	// ��ʱ����Ϣ
	afx_msg void OnTimer(UINT nIDEvent);

	// ��������Ϣ
public:
	// Ԫ����Ϣ
	bool OnSubElementStop( VOID * pData, WORD wDataSize );
	// �ʳ���Ϣ
	bool OnSubLotteryPool( VOID * pData, WORD wDataSize );
	// ������Ϣ
	bool OnSubTaskInfo( VOID * pData, WORD wDataSize );
	// �н���Ϣ
	bool OnSubAwardInfo( VOID * pData, WORD wDataSize );
	//������Ϣ
	bool OnSubControl( VOID * pData, WORD wDataSize );

	// ��Ϣ����
public:
	// ��ת����
	LRESULT OnMessageTurnOver( WPARAM wParam, LPARAM lParam );
	// ��ʼ��Ϣ
	LRESULT OnMessageStartGame( WPARAM wParam, LPARAM lParam );
	// ֹͣ��Ϣ
	LRESULT OnMessageStopGame( WPARAM wParam, LPARAM lParam );
	// ��ʼ�й�
	LRESULT OnMessageStartHosted( WPARAM wParam, LPARAM lParam );
	// ֹͣ�й�
	LRESULT OnMessageStopHosted( WPARAM wParam, LPARAM lParam );
	// ��������
	LRESULT OnMessageScoreLess( WPARAM wParam, LPARAM lParam );
	// ��������
	LRESULT OnMessageScoreAdd( WPARAM wParam, LPARAM lParam );
	// �������
	LRESULT OnMessageScoreMax( WPARAM wParam, LPARAM lParam );
	// ����ѡ��
	LRESULT OnMessageLineCheck( WPARAM wParam, LPARAM lParam );
	// �򿪿���
	LRESULT OnMessageOpenControl( WPARAM wParam, LPARAM lParam );

	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_MESSAGE_MAP()

	
};

//////////////////////////////////////////////////////////////////////////////////

#endif