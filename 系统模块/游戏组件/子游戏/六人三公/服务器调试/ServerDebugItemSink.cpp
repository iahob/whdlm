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

//���ص�������
bool __cdecl CServerDebugItemSink::DebugResult(BYTE cbDebugCardData[GAME_PLAYER][MAX_CARDCOUNT], ROOMUSERDEBUG Keyroomuserdebug, SENDCARDTYPE_CONFIG stConfig, CARDTYPE_CONFIG ctConfig, KING_CONFIG gtConfig)
{
    ASSERT(Keyroomuserdebug.roomUserInfo.wChairID < GAME_PLAYER);

    //��������
    BYTE bCardData[MAX_CARDCOUNT];
    BYTE bHandCardData[GAME_PLAYER][MAX_CARDCOUNT];
    ZeroMemory(bCardData, sizeof(bCardData));
    ZeroMemory(bHandCardData, sizeof(bHandCardData));

    if(stConfig == ST_BETFIRST_)
    {
        //������
        BYTE bMaxUser = INVALID_BYTE;
        //��С���
        BYTE bMinUser = INVALID_BYTE;

        //����ʤ��
        if(Keyroomuserdebug.userDebug.debug_type == CONTINUE_WIN)
        {
            for(BYTE i = 0; i < GAME_PLAYER; i++)
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

            //�������
            BYTE i = bMaxUser + 1;
            for(; i < GAME_PLAYER; i++)
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

            //��С����
            BYTE i = bMinUser + 1;
            for(; i < GAME_PLAYER; i++)
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

            CopyMemory(cbDebugCardData[Keyroomuserdebug.roomUserInfo.wChairID], bCardData, sizeof(bCardData));

            if(Keyroomuserdebug.roomUserInfo.wChairID != bMinUser)
            {
                CopyMemory(cbDebugCardData[bMinUser], bHandCardData[Keyroomuserdebug.roomUserInfo.wChairID], sizeof(bHandCardData[i]));
            }

            return true;
        }
    }
    else if(stConfig == ST_SENDFOUR_)
    {
        //�˿�����
        CList<BYTE, BYTE &> cardlist;
        cardlist.RemoveAll();

        //����С��
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

        //ɾ���˿� ��ɾ��ǰ��4�ţ��������һ�ţ�
        for(WORD i = 0; i < GAME_PLAYER; i++)
        {
            for(WORD j = 0; j < MAX_CARDCOUNT - 1; j++)
            {
                if(cbDebugCardData[i][j] != 0)
                {
                    POSITION ptListHead = cardlist.GetHeadPosition();
                    POSITION ptTemp;
                    BYTE cbCardData = INVALID_BYTE;

                    //��������
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

        //����ʤ��
        if(Keyroomuserdebug.userDebug.debug_type == CONTINUE_WIN)
        {
            //ʤ����� ��ţţ˳��ʼ���죬
            for(BYTE wCardTypeIndex = CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex > CT_CLASSIC_OX_VALUE0; wCardTypeIndex--)
            {
                BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, cbDebugCardData[Keyroomuserdebug.roomUserInfo.wChairID], wCardTypeIndex, gtConfig);
                if(cbKeyCardData == INVALID_BYTE)
                {
                    continue;
                }
                else
                {
                    cbDebugCardData[Keyroomuserdebug.roomUserInfo.wChairID][4] = cbKeyCardData;
                    break;
                }
            }

            //������ҹ��� ����ţ��ʼ����
            for(BYTE i = 0; i < GAME_PLAYER; i++)
            {
                if(cbDebugCardData[i][0] != 0 && i != Keyroomuserdebug.roomUserInfo.wChairID)
                {
                    //������ţ��ţ��
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
        else if(Keyroomuserdebug.userDebug.debug_type == CONTINUE_LOST)
        {
            //ʧ����� ����ţ��ʼ����
            for(BYTE wCardTypeIndex = CT_CLASSIC_OX_VALUE0; wCardTypeIndex < CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex++)
            {
                BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, cbDebugCardData[Keyroomuserdebug.roomUserInfo.wChairID], wCardTypeIndex, gtConfig);
                if(cbKeyCardData == INVALID_BYTE)
                {
                    continue;
                }
                else
                {
                    cbDebugCardData[Keyroomuserdebug.roomUserInfo.wChairID][4] = cbKeyCardData;
                    break;
                }
            }

            //������ҹ��� ��ţţ˳��ʼ����
            for(BYTE i = 0; i < GAME_PLAYER; i++)
            {
                if(cbDebugCardData[i][0] != 0 && i != Keyroomuserdebug.roomUserInfo.wChairID)
                {
                    //������ţ��ţ��
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