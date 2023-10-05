#pragma once
#include "../��Ϸ������/ServerDebug.h"
#include "../��Ϸ������/GameLogic.h"

class CServerDebugItemSink : public IServerDebug
{
	//���Ʊ���
protected:
	BYTE                            m_cbWinSideDebug;
	BYTE							m_cbExcuteTimes;					//ִ�д���

	//�������
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	ITableFrame*					m_pITableFrame;						//���ӽӿ�

public:
	CServerDebugItemSink(void);
	virtual ~CServerDebugItemSink(void);

public:
	//����������
	virtual bool  ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption);

	//��Ҫ����
	virtual bool  NeedDebug();

	//���ؿ�������
	virtual bool DebugResult(BYTE cbTableCardArray[2], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], ITableFrame *pITableFrame);

	// �ж���Ӯ
	void JudgeSystemScore(BYTE cbTableCardArray[2], LONGLONG& lSystemScore, LONGLONG& lAllScore, LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], ITableFrame *pITableFrame);

	////ʵ������
	BYTE GetRealArea(BYTE cbNeedDebugArea);

	//��������
	BYTE GetAnotherArea();

	//���µ���
	void  DebugInfo(const void * pBuffer, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption);

	// ��¼����
	VOID WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);

};
