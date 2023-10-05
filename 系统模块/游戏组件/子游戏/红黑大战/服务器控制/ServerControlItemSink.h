#pragma once
#include "../���������/ServerControl.h"
#include "../���������/GameLogic.h"

class CServerControlItemSink : public IServerControl
{
	//���Ʊ���
protected:
	BYTE							m_cbExcuteTimes;					//ִ�д���
	bool							m_bWinArea[CONTROL_AREA];			//��������
	BYTE							m_cbWinAreaCount;
	int								m_nSendCardCount;					//���ʹ���
	CGameLogic						m_GameLogic;

public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	//����������
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize,
		IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	//��Ҫ����
	virtual bool __cdecl NeedControl();
	//���ؿ�������
	virtual bool __cdecl ControlResult(BYTE	cbTableCardArray[2][MAX_CARD]);

	virtual bool __cdecl CompleteControl();
	//������Ϣ
	VOID ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	// ��¼����
	VOID WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString );

};
