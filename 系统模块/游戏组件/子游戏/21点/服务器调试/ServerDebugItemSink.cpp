#include "StdAfx.h"
#include "serverdebugitemsink.h"

//
CServerDebugItemSink::CServerDebugItemSink(void)
{
}

CServerDebugItemSink::~CServerDebugItemSink( void )
{

}

//����������
bool __cdecl CServerDebugItemSink::ServerDebug( BYTE cbHandCardData[GAME_PLAYER*2][11], ITableFrame * pITableFrame )
{	
	/*CString strInfo = TEXT("\n");
	for( int i = 0; i < GAME_PLAYER; ++i )
	{
		IServerUserItem* pTableUserItem = pITableFrame->GetTableUserItem(i);
		if( pTableUserItem == NULL )
			continue;

		strInfo += pTableUserItem->GetNickName();
		strInfo += TEXT("\n");

		for( int j = 0; j < 11; ++j )
		{
			strInfo += GetCradInfo(cbHandCardData[i][j]);
		}
		strInfo += TEXT("\n");
	}

	for( int i = 0; i < GAME_PLAYER; ++i )
	{
		IServerUserItem*  pTableUserItem = pITableFrame->GetTableUserItem(i);
		if( pTableUserItem == NULL )
			continue;

		if( CUserRight::IsGameDebugUser(pTableUserItem->GetUserRight()) )
			pITableFrame->SendGameMessage(pTableUserItem, strInfo, SMT_CHAT);
	}

	int nLookonCount = 0;
	IServerUserItem* pLookonUserItem = pITableFrame->EnumLookonUserItem(nLookonCount);
	while( pLookonUserItem )
	{
		if( CUserRight::IsGameDebugUser(pLookonUserItem->GetUserRight()) )
			pITableFrame->SendGameMessage(pLookonUserItem, strInfo, SMT_CHAT);

		nLookonCount++;
		pLookonUserItem = pITableFrame->EnumLookonUserItem(nLookonCount);
	}*/

	return true;
}

//����������
bool __cdecl CServerDebugItemSink::ServerDebug(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame)
{
	for (int i =  0; i < GAME_PLAYER; i++)
	{
		IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem==NULL) continue;
		ASSERT(pIServerUserItem);
		if(pIServerUserItem)
		{
			//�����û�
			if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
			{
				return pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_CHEAT_CARD,pCheatCard,sizeof(CMD_S_CheatCard));
			}
		}
	}	
	return false;
}

//��ȡ����Ϣ
CString CServerDebugItemSink::GetCradInfo( BYTE cbCardData )
{
	CString strInfo;
	if( (cbCardData&LOGIC_MASK_COLOR) == 0x00 )
		strInfo += TEXT("[���� ");
	else if( (cbCardData&LOGIC_MASK_COLOR) == 0x10 )
		strInfo += TEXT("[÷�� ");
	else if( (cbCardData&LOGIC_MASK_COLOR) == 0x20 )
		strInfo += TEXT("[���� ");
	else if( (cbCardData&LOGIC_MASK_COLOR) == 0x30 )
		strInfo += TEXT("[���� ");

	if( (cbCardData&LOGIC_MASK_VALUE) == 0x01 )
		strInfo += TEXT("A] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x02 )
		strInfo += TEXT("2] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x03 )
		strInfo += TEXT("3] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x04 )
		strInfo += TEXT("4] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x05 )
		strInfo += TEXT("5] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x06 )
		strInfo += TEXT("6] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x07 )
		strInfo += TEXT("7] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x08 )
		strInfo += TEXT("8] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x09 )
		strInfo += TEXT("9] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0A )
		strInfo += TEXT("10] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0B )
		strInfo += TEXT("J] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0C )
		strInfo += TEXT("Q] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0D )
		strInfo += TEXT("K] ");

	return strInfo;
}

bool __cdecl CServerDebugItemSink::DebugResult(BYTE cbDebugCardData[GAME_PLAYER * 2][MAX_COUNT], BYTE	cbCardCount[GAME_PLAYER * 2], ROOMDESKDEBUG Keyroomuserdebug, ITableFrame * pITableFrame, WORD wBankerUser, BYTE cbPlayStatus[GAME_PLAYER])
{

	bool bIsAiBanker = false;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL)
		{
			if(cbPlayStatus[i]==FALSE)
				continue;
			if(pIServerUserItem->IsAndroidUser()) 
			{
				if(!bIsAiBanker && i==wBankerUser)
					bIsAiBanker = true;
			}
		}
	}

	//�˿˱���
	BYTE cbUserCardData[GAME_PLAYER*2][MAX_COUNT];
	CopyMemory(cbUserCardData, cbDebugCardData, sizeof(cbUserCardData));

	//��������
	BYTE bUserOxData[GAME_PLAYER*2];
	ZeroMemory(bUserOxData,sizeof(bUserOxData));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(cbPlayStatus[i]==FALSE)
		{
			continue;
		}
		bUserOxData[i*2] = m_GameLogic.GetCardType(cbUserCardData[i*2],cbCardCount[i*2],false);
	}
	
	WORD wWinUser = wBankerUser;
	WORD wMinUser = wBankerUser;

	//��������û�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//�û�����
		if (cbPlayStatus[i]==FALSE || i == wBankerUser) continue;

		if (bIsAiBanker)
		{
			if (bUserOxData[i*2] > bUserOxData[wWinUser*2])
			{
				wWinUser=i;
				continue;
			}
		}
		else
		{
			if (bUserOxData[i*2] < bUserOxData[wWinUser*2])
			{
				wWinUser=i;
				continue;
			}
		}		
	}
	
	//������С�û�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//�û�����
		if (cbPlayStatus[i]==FALSE || i == wBankerUser) continue;

		if (bIsAiBanker)
		{
			if (bUserOxData[i*2] < bUserOxData[wMinUser*2])
			{
				wMinUser=i;
				continue;
			}
		}
		else
		{
			if (bUserOxData[i*2] > bUserOxData[wMinUser*2])
			{
				wMinUser=i;
				continue;
			}
		}		
	}

	//����ʤ��
	if (Keyroomuserdebug.userDebug.debug_type == CONTINUE_WIN)
	{
		//��������
		BYTE cbTempData[MAX_COUNT];
		CopyMemory(cbTempData, cbUserCardData[Keyroomuserdebug.roomUserInfo.wChairID * 2],MAX_COUNT);
		CopyMemory(cbUserCardData[Keyroomuserdebug.roomUserInfo.wChairID * 2], cbUserCardData[wWinUser*2],MAX_COUNT);
		CopyMemory(cbUserCardData[wWinUser*2], cbTempData,MAX_COUNT);	
		
		//�����˿�
		CopyMemory(cbDebugCardData, cbUserCardData, sizeof(cbUserCardData));

		//�Ա��˿�
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=pITableFrame->GetTableUserItem(i);

			//�û�����
			if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			{
				if (m_GameLogic.GetCardLogicValue(cbDebugCardData[i * 2][1]) < m_GameLogic.GetCardLogicValue(cbDebugCardData[i * 2][0]))
				{
					BYTE cbTemp = cbDebugCardData[i * 2][1];
					cbDebugCardData[i * 2][1] = cbDebugCardData[i * 2][0];
					cbDebugCardData[i * 2][0] = cbTemp;
				}				
			}
		}

		return true;
	}
	else if (Keyroomuserdebug.userDebug.debug_type == CONTINUE_LOST)
	{	
		//��������
		BYTE cbTempData[MAX_COUNT];
		CopyMemory(cbTempData, cbUserCardData[Keyroomuserdebug.roomUserInfo.wChairID * 2],MAX_COUNT);
		CopyMemory(cbUserCardData[Keyroomuserdebug.roomUserInfo.wChairID * 2], cbUserCardData[wMinUser*2],MAX_COUNT);
		CopyMemory(cbUserCardData[wMinUser*2], cbTempData,MAX_COUNT);	

		//�����˿�
		CopyMemory(cbDebugCardData, cbUserCardData, sizeof(cbUserCardData));

		//�Ա��˿�
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=pITableFrame->GetTableUserItem(i);

			//�û�����
			if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			{
				if (m_GameLogic.GetCardLogicValue(cbDebugCardData[i * 2][1]) < m_GameLogic.GetCardLogicValue(cbDebugCardData[i * 2][0]))
				{
					BYTE cbTemp = cbDebugCardData[i * 2][1];
					cbDebugCardData[i * 2][1] = cbDebugCardData[i * 2][0];
					cbDebugCardData[i * 2][0] = cbTemp;
				}				
			}
		}

		return true;
	}

	ASSERT(FALSE);

	return false;
}
