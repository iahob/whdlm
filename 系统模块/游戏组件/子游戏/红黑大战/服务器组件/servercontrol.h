#pragma once

//��Ϸ���ƻ���
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	virtual bool __cdecl ControlResult(BYTE	cbTableCardArray[2][MAX_CARD]) = NULL;
	//����������
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption) = NULL;
	//��Ҫ����
	virtual bool  __cdecl NeedControl() = NULL;
	//��ɿ���
	virtual bool  __cdecl CompleteControl() = NULL;

};
