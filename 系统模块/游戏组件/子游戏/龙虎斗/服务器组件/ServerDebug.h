#pragma once

//��Ϸ���ƻ���
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//����������
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption) = NULL;

	//��Ҫ����
	virtual bool __cdecl NeedDebug(BYTE &cbWinSide) = NULL;

	//���ؿ�������
	virtual bool __cdecl DebugResult(BYTE	cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker) = NULL;

    //���ӽӿڻص�
    virtual void SetTableFrameSink(ITableFrame * pITableFrame) = NULL;
};

