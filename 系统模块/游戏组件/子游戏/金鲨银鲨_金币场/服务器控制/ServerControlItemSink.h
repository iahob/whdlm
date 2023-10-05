#pragma once
#include "../���������/ServerControl.h"
#define UR_GAME_CONTROL					0x20000000L

class CServerControlItemSink : public IServerControl
{
	//���Ʊ���
protected:
	BYTE							m_cbWinSideControl[2];				//������Ӯ
	BYTE							m_cbExcuteTimes;					//ִ�д���
	int								m_nSendCardCount;					//���ʹ���

public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	//����������
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);

	//��Ҫ����
	virtual bool __cdecl NeedControl();

	//�������
	virtual bool __cdecl MeetControl(tagControlInfo ControlInfo);

	//��ɿ���
	virtual bool __cdecl CompleteControl();

	//���ؿ�������
	virtual bool __cdecl ControlResult(int	cbTableCardArray[]);

	//�ı���Ӯ����
	virtual void __cdecl ChangeValue(BYTE m_Change,BYTE m_Change2);

};
