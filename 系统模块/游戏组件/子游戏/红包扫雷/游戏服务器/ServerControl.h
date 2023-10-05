#pragma once

#define MAX_CARD_COUNT				3									//�����Ŀ
#define AREA_MAX					5									//�������

//��Ϸ���ƻ���
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//����������
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption) = NULL;

	//��Ҫ����
	virtual bool NeedControl() = NULL;

	//��ɿ���
	virtual bool CompleteControl() = NULL;

	//��ʼ����
	virtual void GetSuitResult(BYTE cbTableCardArray[AREA_MAX][MAX_CARD_COUNT], LONGLONG lAllAreaBet[AREA_MAX - 1]) = NULL;
};
