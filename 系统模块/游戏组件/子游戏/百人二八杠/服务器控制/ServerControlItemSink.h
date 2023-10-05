#pragma once
#include "../��Ϸ������/ServerControl.h"
#include "../��Ϸ������/GameLogic.h"

#define INDEX_BANKER	1

class CServerControlItemSink : public IServerControl
{
	//��ҿ���						
protected:
	BYTE							m_cbExcuteTimes;				//ִ�д���
	BYTE							m_cbControlStyle;				//���Ʒ�ʽ
	bool							m_bWinArea[3];					//Ӯ������
	BYTE							m_cbWinAreaCount;
	int								m_nCompareCard[MAX_CARDGROUP][MAX_CARDGROUP];	//�ԱȾ���

	//������Ϣ
protected:
	BYTE							m_cbTableCardArray[MAX_CARDGROUP][MAX_CARD];	//������Ϣ
	BYTE							m_cbTableCard[CARD_COUNT];		//�����˿�
	LONGLONG						m_lAllJettonScore[AREA_COUNT+1];//ȫ����ע
	CGameLogic						m_GameLogic;					//��Ϸ�߼�

public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	//�����Ƿ��ܳɹ�
	bool IsSettingSuccess(BYTE cbControlArea[MAX_INDEX]);

public:
	//����������
	virtual bool  ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);

	//��Ҫ����
	virtual bool  NeedControl();

	//�������
	virtual bool  MeetControl(tagControlInfo ControlInfo);

	//��ɿ���
	virtual bool  CompleteControl();

	//���ؿ�������
	virtual bool  ReturnControlArea(tagControlInfo& ControlInfo);

	//��ʼ����
	virtual void  GetSuitResult( BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lAllJettonScore[] );

	//��Ӯ����
protected:

	//��������
	void BuildCardGroup();

	//��������
	bool AreaWinCard(BYTE cbStack[]);

	//�������
	bool GetSuitCardCombine(BYTE cbStack[]);

	//ׯ����Ӯ
	LONGLONG GetBankerWinScore(bool bWinArea[]);

	//ׯ��Ӯ��
	void BankerWinCard(bool bIsWin,BYTE cbStack[]);

};
