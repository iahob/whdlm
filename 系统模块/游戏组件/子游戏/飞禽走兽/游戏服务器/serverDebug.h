#pragma once

//��Ϸ���Ի���
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//����������
	virtual bool ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption) = NULL;

	//��Ҫ����
	virtual bool NeedDebug() = NULL;

	//���ؿ�������
	virtual bool DebugResult(BYTE cbTableCardArray[2], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], ITableFrame *pITableFrame) = NULL;
};
