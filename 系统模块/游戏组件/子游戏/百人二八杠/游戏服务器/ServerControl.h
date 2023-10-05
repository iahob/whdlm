#pragma once

//��Ϸ���ƻ���
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//����������
	virtual bool  ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame) = NULL;

	//��Ҫ����
	virtual bool  NeedControl() = NULL;


//�������
	virtual bool  MeetControl(tagControlInfo ControlInfo) = NULL;

	//��ɿ���
	virtual bool  CompleteControl() = NULL;

	//���ؿ�������
	virtual bool  ReturnControlArea(tagControlInfo& ControlInfo) = NULL;

	//��ʼ����
	virtual void  GetSuitResult( BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lAllJettonScore[] ) = NULL;



	//���ؿ�������
	//virtual bool  ControlResult(BYTE	cbTableCardArray[], BYTE cbCardCount[]) = NULL;

	//�������
	//virtual BYTE  GetWinControl()=NULL;

	//���ô���
	//virtual void  SetExcuteTimes()=NULL;
};
