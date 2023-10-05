#include "StdAfx.h"
#include "serverdebugitemsink.h"

//
CServerDebugItemSink::CServerDebugItemSink(void)
{
}

CServerDebugItemSink::~CServerDebugItemSink(void)
{

}

//服务器调试
bool __cdecl CServerDebugItemSink::ServerDebug(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], ITableFrame *pITableFrame, WORD wPlayerCount)
{
    for(int i = 0; i < wPlayerCount; ++i)
    {
        IServerUserItem  *pTableUserItem = pITableFrame->GetTableUserItem(i);
        if(pTableUserItem == NULL)
        {
            continue;
        }

        if(CUserRight::IsGameDebugUser(pTableUserItem->GetUserRight()))
        {
            CMD_S_DebugCardInfo cheatCard;
            CopyMemory(cheatCard.cbAllHandCardData, cbHandCardData, sizeof(cheatCard));

            pITableFrame->SendTableData(i, SUB_S_CHEAT_CARD, &cheatCard, sizeof(cheatCard));
        }
    }

    return true;
}

//获取牌信息
CString CServerDebugItemSink::GetCradInfo(BYTE cbCardData)
{
    CString strInfo;
    if((cbCardData & LOGIC_MASK_COLOR) == 0x00)
    {
        strInfo += TEXT("[方块 ");
    }
    else if((cbCardData & LOGIC_MASK_COLOR) == 0x10)
    {
        strInfo += TEXT("[梅花 ");
    }
    else if((cbCardData & LOGIC_MASK_COLOR) == 0x20)
    {
        strInfo += TEXT("[红桃 ");
    }
    else if((cbCardData & LOGIC_MASK_COLOR) == 0x30)
    {
        strInfo += TEXT("[黑桃 ");
    }

    if((cbCardData & LOGIC_MASK_VALUE) == 0x01)
    {
        strInfo += TEXT("A] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x02)
    {
        strInfo += TEXT("2] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x03)
    {
        strInfo += TEXT("3] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x04)
    {
        strInfo += TEXT("4] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x05)
    {
        strInfo += TEXT("5] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x06)
    {
        strInfo += TEXT("6] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x07)
    {
        strInfo += TEXT("7] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x08)
    {
        strInfo += TEXT("8] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x09)
    {
        strInfo += TEXT("9] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x0A)
    {
        strInfo += TEXT("10] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x0B)
    {
        strInfo += TEXT("J] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x0C)
    {
        strInfo += TEXT("Q] ");
    }
    else if((cbCardData & LOGIC_MASK_VALUE) == 0x0D)
    {
        strInfo += TEXT("K] ");
    }

    return strInfo;
}

//返回调试区域
bool __cdecl CServerDebugItemSink::DebugResult(BYTE cbDebugCardData[GAME_PLAYER][MAX_COUNT], ROOMUSERDEBUG Keyroomuserdebug)
{
    ASSERT(Keyroomuserdebug.roomUserInfo.wChairID < GAME_PLAYER);

    //变量定义
    BYTE bCardData[MAX_COUNT];
    BYTE bHandCardData[GAME_PLAYER][MAX_COUNT];
    ZeroMemory(bCardData, sizeof(bCardData));
    ZeroMemory(bHandCardData, sizeof(bHandCardData));

    //最大玩家
    BYTE bMaxUser = INVALID_BYTE;
    //最小玩家
    BYTE bMinUser = INVALID_BYTE;

    //调试胜利
    if(Keyroomuserdebug.userDebug.debug_type == CONTINUE_WIN)
    {
        for(BYTE i = 0; i < GAME_PLAYER; i++)
        {
            CopyMemory(bHandCardData[i], cbDebugCardData[i], sizeof(BYTE)*MAX_COUNT);

            if(INVALID_BYTE == bMaxUser)
            {
                if(cbDebugCardData[i][0] != 0)
                {
                    bMaxUser = i;
                    CopyMemory(bCardData, cbDebugCardData[i], sizeof(bCardData));
                }
            }
        }

        //最大牌型
        BYTE i = bMaxUser + 1;
        for(i = bMaxUser + 1; i < GAME_PLAYER; i++)
        {
            if(cbDebugCardData[i][0] == 0)
            {
                continue;
            }

            if(m_GameLogic.CompareCard(bCardData, cbDebugCardData[i], MAX_COUNT) == false)
            {
                CopyMemory(bCardData, cbDebugCardData[i], sizeof(bCardData));
                bMaxUser = i;
            }
        }

        CopyMemory(cbDebugCardData[Keyroomuserdebug.roomUserInfo.wChairID], bCardData, sizeof(bCardData));

        if(Keyroomuserdebug.roomUserInfo.wChairID != bMaxUser)
        {
            CopyMemory(cbDebugCardData[bMaxUser], bHandCardData[Keyroomuserdebug.roomUserInfo.wChairID], sizeof(bHandCardData[i]));
        }

        return true;
    }
    else if(Keyroomuserdebug.userDebug.debug_type == CONTINUE_LOST)
    {
        for(BYTE i = 0; i < GAME_PLAYER; i++)
        {
            CopyMemory(bHandCardData[i], cbDebugCardData[i], sizeof(BYTE)*MAX_COUNT);

            if(INVALID_BYTE == bMinUser)
            {
                if(cbDebugCardData[i][0] != 0)
                {
                    bMinUser = i;
                    CopyMemory(bCardData, cbDebugCardData[i], sizeof(bCardData));
                }
            }
        }

        //最小牌型
        BYTE i = bMinUser + 1;
        for(i = bMinUser + 1; i < GAME_PLAYER; i++)
        {
            if(cbDebugCardData[i][0] == 0)
            {
                continue;
            }

            if(m_GameLogic.CompareCard(cbDebugCardData[i], bCardData, MAX_COUNT) == false)
            {
                CopyMemory(bCardData, cbDebugCardData[i], sizeof(bCardData));
                bMinUser = i;
            }
        }

        CopyMemory(cbDebugCardData[Keyroomuserdebug.roomUserInfo.wChairID], bCardData, sizeof(bCardData));

        if(Keyroomuserdebug.roomUserInfo.wChairID != bMinUser)
        {
            CopyMemory(cbDebugCardData[bMinUser], bHandCardData[Keyroomuserdebug.roomUserInfo.wChairID], sizeof(bHandCardData[i]));
        }

        return true;
    }

    ASSERT(FALSE);

    return false;
}