#pragma once

//游戏调试基类
class IServerDebug
{
public:
	IServerDebug(void){};
	virtual ~IServerDebug(void){};

public:
	//服务器调试
	virtual bool __cdecl ServerDebug(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame) = NULL;
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame) = NULL;
	virtual bool __cdecl ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem) = NULL;

	//好牌调试
	virtual bool __cdecl GetDebugInfo(ITableFrame * pITableFrame, BYTE cbBankerCard[3], BYTE cbHandCardCount[GAME_PLAYER], BYTE m_cbHandCardData[GAME_PLAYER][MAX_COUNT]) = NULL;
	//更新分数
	virtual void __cdecl UpdateDebugInfo(ITableFrame * pITableFrame) = NULL;
	//更新分数
	virtual bool __cdecl UpdateDebugInfo(ITableFrame * pITableFrame, tagScoreInfo ScoreInfoArray[GAME_PLAYER]) = NULL;
};
