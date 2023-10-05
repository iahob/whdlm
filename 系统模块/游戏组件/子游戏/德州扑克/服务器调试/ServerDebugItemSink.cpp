#include "StdAfx.h"
#include "serverdebugitemsink.h"

//
CServerDebugItemSink::CServerDebugItemSink(void)
{
}

CServerDebugItemSink::~CServerDebugItemSink( void )
{

}

//服务器调试
bool __cdecl CServerDebugItemSink::ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame )
{
	for(WORD i = 0; i < GAME_PLAYER ;i++)
	{
		IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem)
		{
			//作弊用户
			if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
			{
				ServerDebug(pDebugCard, pITableFrame, pIServerUserItem);
			}
		}
	}

	//WB改
	WORD wEnumIndex=0;
	do
	{
		IServerUserItem * pIServerUserItem = pITableFrame->EnumLookonUserItem(wEnumIndex++);
		if(pIServerUserItem == NULL) break;
		//作弊用户
		if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		{
			ServerDebug(pDebugCard, pITableFrame, pIServerUserItem);
		}
	}while(TRUE);

	return true;
}

//服务器调试
bool __cdecl CServerDebugItemSink::ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem==NULL)
	{
		return ServerDebug(pDebugCard,pITableFrame);
	}
	else
	{
		if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		{
			return pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_CHEAT_CARD,pDebugCard,sizeof(CMD_S_DebugCard));
		}
	}
	return false;
}

bool __cdecl CServerDebugItemSink::DebugResult(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], BYTE cbCenterCard[MAX_CENTER_COUNT], ROOMUSERDEBUG Keyroomuserdebug)
{
	ASSERT(Keyroomuserdebug.roomUserInfo.wChairID < GAME_PLAYER);

	//变量定义
	BYTE cbTempCardData[GAME_PLAYER][MAX_COUNT];
	ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
	CopyMemory(cbTempCardData, cbHandCardData, sizeof(BYTE)*MAX_COUNT*GAME_PLAYER);

	//扑克数据
	BYTE cbEndCardData[GAME_PLAYER][MAX_CENTER_COUNT] = {0};
	BYTE cbCardType[GAME_PLAYER] = { 0 };
	//获取扑克
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//用户过滤
		if (cbTempCardData[i][0] == 0) continue;

		//最大牌型
		cbCardType[i] = m_GameLogic.FiveFromSeven(cbTempCardData[i], MAX_COUNT, cbCenterCard, MAX_CENTER_COUNT, cbEndCardData[i], MAX_CENTER_COUNT);
		ASSERT(cbCardType[i] != FALSE);
	}

	//调试胜利
	if (Keyroomuserdebug.userDebug.debug_type == CONTINUE_WIN)
	{
		//最大玩家
		BYTE cbMaxUser = INVALID_BYTE;

		for (BYTE i = 0; i<GAME_PLAYER; i++)
		{
			if (cbTempCardData[i][0] == 0)continue;

			if (cbMaxUser == INVALID_BYTE)
				cbMaxUser = i;
			else if (m_GameLogic.CompareCard(cbEndCardData[i], cbEndCardData[cbMaxUser], MAX_CENTER_COUNT)==2)
			{
				cbMaxUser = i;
			}
		}

		if (cbMaxUser != INVALID_BYTE && Keyroomuserdebug.roomUserInfo.wChairID != cbMaxUser)
		{
			BYTE cbCardData[MAX_COUNT] = { 0 };
			CopyMemory(cbCardData, cbHandCardData[cbMaxUser], sizeof(cbCardData));
			CopyMemory(cbHandCardData[cbMaxUser], cbHandCardData[Keyroomuserdebug.roomUserInfo.wChairID], sizeof(cbCardData));
			CopyMemory(cbHandCardData[Keyroomuserdebug.roomUserInfo.wChairID], cbCardData, sizeof(cbCardData));
		}

		return true;
	}
	else if (Keyroomuserdebug.userDebug.debug_type == CONTINUE_LOST)
	{
		//最小玩家
		BYTE cbMinUser = INVALID_BYTE;
		BYTE i = 0;
		for (; i < GAME_PLAYER; i++)
		{
			if (cbTempCardData[i][0] == 0)continue;

			if (cbMinUser == INVALID_BYTE)
				cbMinUser = i;
			else if (m_GameLogic.CompareCard(cbEndCardData[i], cbEndCardData[cbMinUser], MAX_CENTER_COUNT)==1)
			{
				cbMinUser = i;
			}
		}

		if (cbMinUser != INVALID_BYTE && Keyroomuserdebug.roomUserInfo.wChairID != cbMinUser)
		{
			BYTE cbCardData[MAX_COUNT] = { 0 };
			CopyMemory(cbCardData, cbHandCardData[cbMinUser], sizeof(cbCardData));
			CopyMemory(cbHandCardData[cbMinUser], cbHandCardData[Keyroomuserdebug.roomUserInfo.wChairID], sizeof(cbCardData));
			CopyMemory(cbHandCardData[Keyroomuserdebug.roomUserInfo.wChairID], cbCardData, sizeof(cbCardData));
		}

		return true;
	}

	return false;
}
