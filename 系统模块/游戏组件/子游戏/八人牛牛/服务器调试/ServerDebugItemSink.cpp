#include "StdAfx.h"
#include "serverdebugitemsink.h"

//////////////////////////////////////////////////////////////////////////

//
CServerDebugItemSink::CServerDebugItemSink(void)
{
}

CServerDebugItemSink::~CServerDebugItemSink(void)
{

}

//返回调试牌桌
bool __cdecl CServerDebugItemSink::DebugResult(BYTE cbDebugCardData[GAME_PLAYER][MAX_CARDCOUNT], WORD wCtrlChairID, bool bWin, SENDCARDTYPE_CONFIG stConfig, CARDTYPE_CONFIG ctConfig, KING_CONFIG gtConfig, WORD wPlayerCount)
{
	ASSERT(wCtrlChairID < wPlayerCount);

    //变量定义
    BYTE bCardData[MAX_CARDCOUNT];
    BYTE bHandCardData[GAME_PLAYER][MAX_CARDCOUNT];
    ZeroMemory(bCardData, sizeof(bCardData));
    ZeroMemory(bHandCardData, sizeof(bHandCardData));

    if(stConfig == ST_BETFIRST_)
    {
        //最大玩家
        BYTE bMaxUser = INVALID_BYTE;
        //最小玩家
        BYTE bMinUser = INVALID_BYTE;

        //调试胜利
		if (bWin)
        {
			for (BYTE i = 0; i < wPlayerCount; i++)
            {
                CopyMemory(bHandCardData[i], cbDebugCardData[i], sizeof(BYTE)*MAX_CARDCOUNT);

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
			for (; i < wPlayerCount; i++)
            {
                if(cbDebugCardData[i][0] == 0) { continue; }

                bool bFistOx = m_GameLogic.GetOxCard(bCardData, MAX_CARDCOUNT);
                bool bNextOx = m_GameLogic.GetOxCard(cbDebugCardData[i], MAX_CARDCOUNT);

                if(m_GameLogic.CompareCard(bCardData, cbDebugCardData[i], MAX_CARDCOUNT, ctConfig) == false)
                {
                    CopyMemory(bCardData, cbDebugCardData[i], sizeof(bCardData));
                    bMaxUser = i;
                }
            }

            CopyMemory(cbDebugCardData[wCtrlChairID], bCardData, sizeof(bCardData));

            if(wCtrlChairID != bMaxUser)
            {
                CopyMemory(cbDebugCardData[bMaxUser], bHandCardData[wCtrlChairID], sizeof(bHandCardData[i]));
            }

            return true;
        }
        else
        {
			for (BYTE i = 0; i < wPlayerCount; i++)
            {
                CopyMemory(bHandCardData[i], cbDebugCardData[i], sizeof(BYTE)*MAX_CARDCOUNT);

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
			for (; i < wPlayerCount; i++)
            {
                if(cbDebugCardData[i][0] == 0) { continue; }

                bool bFistOx = m_GameLogic.GetOxCard(cbDebugCardData[i], MAX_CARDCOUNT);
                bool bNextOx = m_GameLogic.GetOxCard(bCardData, MAX_CARDCOUNT);

                if(m_GameLogic.CompareCard(cbDebugCardData[i], bCardData, MAX_CARDCOUNT, ctConfig) == false)
                {
                    CopyMemory(bCardData, cbDebugCardData[i], sizeof(bCardData));
                    bMinUser = i;
                }
            }

            CopyMemory(cbDebugCardData[wCtrlChairID], bCardData, sizeof(bCardData));

            if(wCtrlChairID != bMinUser)
            {
                CopyMemory(cbDebugCardData[bMinUser], bHandCardData[wCtrlChairID], sizeof(bHandCardData[i]));
            }

            return true;
        }
    }
    else if(stConfig == ST_SENDFOUR_)
    {
        //扑克链表
        CList<BYTE, BYTE &> cardlist;
        cardlist.RemoveAll();

        //含大小王
        if(gtConfig == GT_HAVEKING_)
        {
            for(WORD i = 0; i < 54; i++)
            {
                cardlist.AddTail(m_GameLogic.m_cbCardListDataHaveKing[i]);
            }
        }
        else if(gtConfig == GT_NOKING_)
        {
            for(WORD i = 0; i < 52; i++)
            {
                cardlist.AddTail(m_GameLogic.m_cbCardListDataNoKing[i]);
            }
        }

        //删除扑克 （删除前面4张，构造后面一张）
		for (WORD i = 0; i < wPlayerCount; i++)
        {
            for(WORD j = 0; j < MAX_CARDCOUNT - 1; j++)
            {
                if(cbDebugCardData[i][j] != 0)
                {
                    POSITION ptListHead = cardlist.GetHeadPosition();
                    POSITION ptTemp;
                    BYTE cbCardData = INVALID_BYTE;

                    //遍历链表
                    while(ptListHead)
                    {
                        ptTemp = ptListHead;
                        if(cardlist.GetNext(ptListHead) == cbDebugCardData[i][j])
                        {
                            cardlist.RemoveAt(ptTemp);
                            break;
                        }
                    }
                }
            }
        }

        //调试胜利
		if (bWin)
        {
            //胜利玩家 从牛牛顺序开始构造，
            for(BYTE wCardTypeIndex = CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex > CT_CLASSIC_OX_VALUE0; wCardTypeIndex--)
            {
                BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, cbDebugCardData[wCtrlChairID], wCardTypeIndex, gtConfig);
                if(cbKeyCardData == INVALID_BYTE)
                {
                    continue;
                }
                else
                {
                    cbDebugCardData[wCtrlChairID][4] = cbKeyCardData;
                    break;
                }
            }

            //其他玩家构造 从无牛开始构造
			for (BYTE i = 0; i < wPlayerCount; i++)
            {
                if(cbDebugCardData[i][0] != 0 && i != wCtrlChairID)
                {
                    //构造无牛到牛九
                    for(BYTE wCardTypeIndex = CT_CLASSIC_OX_VALUE0; wCardTypeIndex < CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex++)
                    {
                        BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, cbDebugCardData[i], wCardTypeIndex, gtConfig);
                        if(cbKeyCardData == INVALID_BYTE)
                        {
                            continue;
                        }
                        else
                        {
                            cbDebugCardData[i][4] = cbKeyCardData;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            //失败玩家 从无牛开始构造
            for(BYTE wCardTypeIndex = CT_CLASSIC_OX_VALUE0; wCardTypeIndex < CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex++)
            {
                BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, cbDebugCardData[wCtrlChairID], wCardTypeIndex, gtConfig);
                if(cbKeyCardData == INVALID_BYTE)
                {
                    continue;
                }
                else
                {
                    cbDebugCardData[wCtrlChairID][4] = cbKeyCardData;
                    break;
                }
            }

            //其他玩家构造 从牛牛顺序开始构造
			for (BYTE i = 0; i < wPlayerCount; i++)
            {
                if(cbDebugCardData[i][0] != 0 && i != wCtrlChairID)
                {
                    //构造无牛到牛九
                    for(BYTE wCardTypeIndex = CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex > CT_CLASSIC_OX_VALUE0; wCardTypeIndex--)
                    {
                        BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, cbDebugCardData[i], wCardTypeIndex, gtConfig);
                        if(cbKeyCardData == INVALID_BYTE)
                        {
                            continue;
                        }
                        else
                        {
                            cbDebugCardData[i][4] = cbKeyCardData;
                            break;
                        }
                    }
                }
            }
        }

        return true;
    }

    ASSERT(FALSE);

    return false;
}