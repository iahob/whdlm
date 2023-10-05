#pragma once
#include "../���������/ServerDebug.h"
#include "../���������/GameLogic.h"

#define INDEX_BANKER				1
#define DEBUG_AREA					4
#define MAX_CARDGROUP				5

class CServerDebugItemSink : public IServerDebug
{
	//��ҵ���						
protected:
	BYTE							m_cbExcuteTimes;								//ִ�д���
	BYTE							m_cbDebugStyle;									//���Է�ʽ
	bool							m_bWinArea[DEBUG_AREA];							//Ӯ������
	BYTE							m_cbWinAreaCount;
	int								m_nCompareCard[MAX_CARDGROUP][MAX_CARDGROUP];	//�ԱȾ���

	//������Ϣ
protected:
	BYTE							m_cbTableCardArray[MAX_CARDGROUP][MAX_CARD];	//������Ϣ
	BYTE							m_cbTableCard[CARD_COUNT];						//�����˿�
	LONGLONG						m_lUserJettonScore[AREA_MAX + 1][GAME_PLAYER];	//ȫ����ע
	WORD							m_wCurrentBanker;
	ITableFrame						*m_pITableFrame;
	CGameLogic						m_GameLogic;									//��Ϸ�߼�

public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//����������
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize,
		IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	//��Ҫ����
	virtual bool  NeedDebug();

	//��ʼ����
	virtual void  GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], WORD wCurrentBanker);

	//��Ӯ����
protected:

	//��������
	bool AreaWinCard(BYTE cbStack[]);

	//�������
	bool GetSuitCardCombine(BYTE cbStack[]);

	//ϵͳ��Ӯ
	LONGLONG GetSysWinScore(int nWinMultiple[]);

	//ϵͳӮ��
	void SysWinCard(bool bIsWin,BYTE cbStack[]);

	//������Ϣ
	VOID DebugInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	// ��¼����
	VOID WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString );
};
