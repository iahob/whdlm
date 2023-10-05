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

	//��ɿ���
	virtual bool  CompleteControl() = NULL;

	//���ؿ�������
	virtual bool  ReturnControlArea(tagControlInfo& ControlInfo,LONGLONG lAllJettonScore[]) = NULL;

	//����ϵͳ��������
	virtual bool  ReturnSysCtrlArea(tagControlInfo& ControlInfo, LONGLONG lUserJettonScore[AREA_COUNT + 1][GAME_PLAYER], ITableFrame *pITableFrame) = NULL;

	//��ȡ��ǰ��������
	virtual EM_CTRL_TYPE GetCurCtrlType() = NULL;
};
