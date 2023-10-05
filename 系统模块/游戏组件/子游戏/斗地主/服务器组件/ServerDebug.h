#pragma once

//��Ϸ���Ի���
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//����������
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame) = NULL;
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame) = NULL;
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem) = NULL;

	//���Ƶ���
	virtual bool __cdecl GetDebugInfo(ITableFrame * pITableFrame, BYTE cbBankerCard[3], BYTE cbHandCardCount[GAME_PLAYER], BYTE m_cbHandCardData[GAME_PLAYER][MAX_COUNT]) = NULL;
	//���·���
	virtual void __cdecl UpdateDebugInfo(ITableFrame * pITableFrame) = NULL;
	//���·���
	virtual bool __cdecl UpdateDebugInfo(ITableFrame * pITableFrame, tagScoreInfo ScoreInfoArray[GAME_PLAYER]) = NULL;
};
