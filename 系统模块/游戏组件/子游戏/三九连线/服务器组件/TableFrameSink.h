#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"

////////////////////////////////////////////////////////////////////////////////// 

// �н���Ϣ
struct tagAwardInfo 
{
	tchar						szPlayName[40];											// �������
	int							nMultipleIndex;											// �н���Ϣ
	__int64						nAwardScore;											// �н�����
};

// ��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction, public IServerControlCallback
{
	// ��Ϣ����
protected:
	bool							m_bLegalOne;							// ��Ȩ��ʶ
	bool							m_bLagalTwo;							// ��Ȩ��ʶ
	ICompilationSink *				m_pICompilationSink;					// ��Ȩָ��

	// �����Ϣ
protected:
	bool							m_bPlayGame;							// ��Ϸ��
	int								m_nPlayBetScore;						// ��ע����
	int								m_nPlayBetLineCount;					// ��������
	bool							m_bPlayBetLine[LINE_INDEX_MAX];			// ��ע����

	// ��¼��Ϣ
protected:
	ulonglong						m_lThreeTotal;							// �����н�
	ulonglong						m_lNiceTotal;							// �����н�
	ulonglong						m_lGrandTotal;							// �н��ۼ�
	ulonglong						m_lWinMax;								// ����н�

	// ������Ϣ
protected:
	int								m_nTaskProgress[TASK_INDEX_MAX];		// �������
	int								m_nTaskTarget[TASK_INDEX_MAX];			// ����Ŀ��
	int								m_nTaskCount[TASK_INDEX_MAX];			// ��������
	int								m_nTaskScore[TASK_INDEX_MAX];			// ������

	// ������Ϣ
protected:
	int								m_nThreeLineMultiple[MultipleThree_Max];	// 3������
	int								m_nNineLineMultiple[MultipleNine_Max];	// 9������
	double							m_dLotteryPoolScale[MultipleNine_Max];	// �ʳر���

	// ��¼��Ϣ
protected:
	Static CArray< tagAwardInfo >	m_ArrayAwardInfo;								// �н���Ϣ
	CArray< int	>					m_ArrayRoadInfo;								// ·����Ϣ

	// ������Ϣ
protected:
	Static double					m_dGameDifficulty;						// ��Ϸ�Ѷ�
	int								m_nAwardCondition;						// ���������
	bool							m_bAwardNine;							// ������ʾ
	int								m_nBetScore[BET_INDEX_MAX];				// ��ע����

	// �Զ��弸��
protected:
	bool							m_bCustomOpen;							// �Զ��弸��
	int								m_nMultipleChance[MultipleThree_Max + MultipleNine_Max + 1]; // ��������

	// �����Ϣ
protected:
	Static double					m_dRoomDecay;							// ����˥��
	Static double					m_dRoomStock;							// ������
	Static double					m_dRoomIdle;							// �������
	Static double					m_dRoomLotteryPool;						// ����ʳ�
	Static double					m_dMinLotteryPool;						// ��С���
	Static int						m_nLotteryIdle;							// ���вʳ�

	// �ļ�·��
protected:
	Static tchar					m_szAwardFile[MAX_PATH + 1];			// �н���Ϣ�ļ�

	// ����ӿ�
protected:
	ITableFrame	*					m_pITableFrame;							// ��ܽӿ�
	tagGameServiceOption*			m_pGameServiceOption;					// ���ò���
	tchar							m_szLogoFileName[MAX_PATH];				// ������־

	// �������
public:
	Static hinstance				m_hControlInst;
	Static IServerControl *			m_pIServerControl;

	// ��������
public:
	// ���캯��
	CTableFrameSink();
	// ��������
	virtual ~CTableFrameSink();

	// �����ӿ�
public:
	// �ͷŶ���
	virtual VOID Release() { delete this; }
	// �ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	// ����ӿ�
public:
	// ��������
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	// ��λ����
	virtual VOID RepositionSink();


	// ��ѯ�ӿ�
public:
	// ��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	// ���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) { return 0; }
	// ��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return false; }

	// ��Ϸ�¼�
public:
	// ��Ϸ��ʼ
	virtual bool OnEventGameStart();
	// ��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	// ���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	// �¼��ӿ�
public:
	// ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	// �����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	// �����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	// ����ӿ�
public:
	// ��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	// �����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	// �����ӿ�
public:
	// ���û���
	virtual void SetGameBaseScore(LONG lBaseScore) {}

	// �û��¼�
public:
	// �û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	// �û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	// �û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	// �û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	// �û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	// ���ƺ���
public:
	// ������Ϣ
	virtual bool SendControlMessage( void * pIServerUserItem, uint nMessageID, void * pData, uint nSize );

	// ��Ϸ��Ϣ
public:
	// ��ʼ��Ϣ
	bool OnSubStart( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem );
	// ������Ϣ
	bool OnSubStop( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem );
	// ������Ϣ
	bool OnSubControl( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem );

	// ���ܺ���
public:
	// Ԫ����Ϸ
	void ElementGame( bool bStopNew, bool bSendData );
	// ��������
	longlong MultipleScore( int nMultipleIndex );

public:
	// ʹ�ܹػ���Ȩ ���ػ�
	bool EnableShutdownPrivilege();
	// ��־��¼
	void LineLogo( tchar * pText, ... );
};

////////////////////////////////////////////////////////////////////////////////// 

#endif