#pragma once

#define MAX_CARD					5

//��Ϸ���ƻ���
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//����������
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption) = NULL;

	//��Ҫ����
	virtual bool  NeedDebug() = NULL;

	//��ʼ����
	virtual void  GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], WORD wCurrentBanker) = NULL;
};
