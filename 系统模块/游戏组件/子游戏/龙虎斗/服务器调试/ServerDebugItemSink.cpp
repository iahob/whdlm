#include "StdAfx.h"
#include "serverdebugitemsink.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////

const BYTE CServerDebugItemSink::m_cbCardData[4][13] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//方块 A - K
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,	//梅花 A - K
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,	//红桃 A - K
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,	//黑桃 A - K
};

CServerDebugItemSink::CServerDebugItemSink(void)
{
    m_cbWinSideDebug = 0xFF;
    m_cbExcuteTimes = 0;
    m_pITableFrame = NULL;

    //随机种子
    srand(time(NULL));
}

CServerDebugItemSink::~CServerDebugItemSink(void)
{
}

//服务器控制
bool __cdecl CServerDebugItemSink::ServerDebug(WORD wSubCmdID, const void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption)
{
    //效验数据
    if(!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
    {
        return false;
    }

    ASSERT(wDataSize == sizeof(CMD_C_AreaCtrl));
    if(wDataSize != sizeof(CMD_C_AreaCtrl))
    {
        return false;
    }

    ASSERT(pITableFrame != NULL);
    if(pITableFrame != NULL)
    {
        m_pITableFrame = pITableFrame;
    }

    CMD_C_AreaCtrl *pAreaCtrl = (CMD_C_AreaCtrl *)pDataBuffer;

	if (pAreaCtrl->bCancelAreaCtrl)
	{
		m_cbWinSideDebug = 0xFF;
		m_cbExcuteTimes = 0;

		//调试信息
		DebugInfo(pAreaCtrl->bCancelAreaCtrl, pIServerUserItem, pITableFrame, pGameServiceOption);
	}
	else
	{
		m_cbWinSideDebug = pAreaCtrl->cbCtrlPosIndex;
		m_cbExcuteTimes = pAreaCtrl->cbConfigCtrlTimes;

		//调试信息
		DebugInfo(pAreaCtrl->bCancelAreaCtrl, pIServerUserItem, pITableFrame, pGameServiceOption);
	}

    return true;
}

//需要控制
bool __cdecl CServerDebugItemSink::NeedDebug(BYTE &cbWinSide)
{
	cbWinSide = m_cbWinSideDebug;
	if (m_cbWinSideDebug >= 0 && m_cbWinSideDebug < AREA_MAX + 2 && m_cbExcuteTimes > 0)
	{
		return true;
	}

	return false;
}

//更新调试
void  CServerDebugItemSink::DebugInfo(bool bCancelAreaCtrl, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption)
{
	CString str;
	if (!bCancelAreaCtrl)
	{
		switch (m_cbWinSideDebug)
		{
		case 0:
			str.Format(TEXT("执行调试成功,胜利区域:龙,执行次数:%d"), m_cbExcuteTimes);
			break;
		case 1:
			str.Format(TEXT("执行调试成功,胜利区域:和,执行次数:%d"), m_cbExcuteTimes);
			break;
		case 2:
			str.Format(TEXT("执行调试成功,胜利区域:虎,执行次数:%d"), m_cbExcuteTimes);
			break;
		case 3:
			str.Format(TEXT("执行调试成功,胜利区域:单局系统输,执行次数:%d"), m_cbExcuteTimes);
			break;
		case 4:
			str.Format(TEXT("执行调试成功,胜利区域:单局系统赢,执行次数:%d"), m_cbExcuteTimes);
			break;

		default:
			break;
		}
	}
	else
	{
		str = TEXT("取消调试!");
	}

    //记录信息
    CString strDebugInfo;
    CTime Time(CTime::GetCurrentTime());
    strDebugInfo.Format(TEXT("房间: %s | 桌号: %u | 时间: %d-%d-%d %d:%d:%d\n调试人账号: %s | 调试人ID: %u\n%s\n"),
                        pGameServiceOption->szServerName, pITableFrame->GetTableID() + 1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
                        Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

    WriteInfo(TEXT("龙虎斗调试信息.log"), strDebugInfo);

    return;
}

//记录信息
VOID CServerDebugItemSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
	//设置语言区域
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("%s"), pszFileName);
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		myFile.SeekToEnd();
		myFile.WriteString(pszString);
		myFile.Flush();
		myFile.Close();
	}

	//还原区域设定
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);

	return;
}

//返回控制区域
bool CServerDebugItemSink::DebugResult(BYTE	cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker)
{
	//区域
	if (m_cbWinSideDebug >= AREA_LONG && m_cbWinSideDebug < AREA_MAX)
	{
		//初始化扑克
		m_cardArrayList.RemoveAll();
		InitCardArray();
		
		switch (m_cbWinSideDebug)
		{
		case AREA_LONG:
			{
				//遍历链表
				POSITION ptHead = m_cardArrayList.GetHeadPosition();
				do
				{
					CARDARRAY cardArray = m_cardArrayList.GetAt(ptHead);
					POSITION ptTemp = ptHead;
					m_cardArrayList.GetNext(ptHead);
					//删除所有非龙赢的元素
					if(!(GetCardValue(cardArray.cbCardArray[0]) > GetCardValue(cardArray.cbCardArray[1])))
					{
						m_cardArrayList.RemoveAt(ptTemp);
					}
		
				}
				while(ptHead != NULL);
				break;
			}
		case AREA_PING:
			{
				//遍历链表
				POSITION ptHead = m_cardArrayList.GetHeadPosition();
				do
				{
					CARDARRAY cardArray = m_cardArrayList.GetAt(ptHead);
					POSITION ptTemp = ptHead;
					m_cardArrayList.GetNext(ptHead);
					//删除所有非和赢的元素
					if(!(GetCardValue(cardArray.cbCardArray[0]) == GetCardValue(cardArray.cbCardArray[1])))
					{
						m_cardArrayList.RemoveAt(ptTemp);
					}
		
				}
				while(ptHead != NULL);
				break;
			}
		case AREA_HU:
			{
				//遍历链表
				POSITION ptHead = m_cardArrayList.GetHeadPosition();
				do
				{
					CARDARRAY cardArray = m_cardArrayList.GetAt(ptHead);
					POSITION ptTemp = ptHead;
					m_cardArrayList.GetNext(ptHead);
					//删除所有非虎赢的元素
					if(!(GetCardValue(cardArray.cbCardArray[0]) < GetCardValue(cardArray.cbCardArray[1])))
					{
						m_cardArrayList.RemoveAt(ptTemp);
					}
		
				}
				while(ptHead != NULL);
				break;
			}
		default:
		    break;
		}
		
		//拷贝扑克
		WORD wCountIndex = m_cardArrayList.GetSize();
		ASSERT(wCountIndex != 0);
		POSITION keyPT = m_cardArrayList.FindIndex(rand() % wCountIndex);
		CARDARRAY cardArray = m_cardArrayList.GetAt(keyPT);
		CopyMemory(cbTableCardArray, cardArray.cbCardArray, sizeof(cardArray.cbCardArray));
	}
	//系统输和赢
	else if (m_cbWinSideDebug >= AREA_SYSLOST && m_cbWinSideDebug <= AREA_SYSWIN)
	{
		//初始化扑克
		m_cardArrayList.RemoveAll();
		InitCardArray();
		
		//系统赢
		if (m_cbWinSideDebug == AREA_SYSWIN)
		{
		    //遍历链表
		    POSITION ptHead = m_cardArrayList.GetHeadPosition();
		    do
		    {
		        CARDARRAY cardArray = m_cardArrayList.GetNext(ptHead);
		        BYTE cbTempTableCardArray[2];
		        ZeroMemory(cbTempTableCardArray, sizeof(cbTempTableCardArray));
		        cbTempTableCardArray[0] = cardArray.cbCardArray[0];
		        cbTempTableCardArray[1] = cardArray.cbCardArray[1];
		
		        //系统得分
                SCORE lBankerWinScore = CalculateScore(cbTempTableCardArray, lUserInAllScore, wCurrentBanker);
		
		        if(lBankerWinScore >= 0)
		        {
		            CopyMemory(cbTableCardArray, cbTempTableCardArray, sizeof(cbTempTableCardArray));
		            break;
		        }
		    }
		    while(ptHead != NULL);
		}
		else if (m_cbWinSideDebug == AREA_SYSLOST)
		{
			//遍历链表
			POSITION ptHead = m_cardArrayList.GetHeadPosition();
			do
			{
				CARDARRAY cardArray = m_cardArrayList.GetNext(ptHead);
				BYTE cbTempTableCardArray[2];
				ZeroMemory(cbTempTableCardArray, sizeof(cbTempTableCardArray));
				cbTempTableCardArray[0] = cardArray.cbCardArray[0];
				cbTempTableCardArray[1] = cardArray.cbCardArray[1];

				//系统得分
                SCORE lBankerWinScore = CalculateScore(cbTempTableCardArray, lUserInAllScore, wCurrentBanker);

				if (lBankerWinScore < 0)
				{
					CopyMemory(cbTableCardArray, cbTempTableCardArray, sizeof(cbTempTableCardArray));
					break;
				}
			} while (ptHead != NULL);
		}
	}

	m_cbExcuteTimes--;

	return true;
}

void CServerDebugItemSink::InitCardArray()
{
    //使用八副牌，两个区域的牌可以一模一样
    CARDARRAY keyCardArray[4 * 13 * 4 * 13];
    ZeroMemory(keyCardArray, sizeof(keyCardArray));
    INT nCardIndex = 0;
    for(WORD i = 0; i < 4; i++)
    {
        for(WORD j = 0; j < 13; j++)
        {
            for(WORD cbXIndex = 0; cbXIndex < 4; cbXIndex++)
            {
                for(WORD cbYIndex = 0; cbYIndex < 13; cbYIndex++)
                {
                    keyCardArray[nCardIndex].cbCardArray[0] = m_cbCardData[i][j];
                    keyCardArray[nCardIndex].cbCardArray[1] = m_cbCardData[cbXIndex][cbYIndex];
                    nCardIndex++;
                }
            }
        }
    }

    ASSERT(nCardIndex == 4 * 13 * 4 * 13);

    //混乱准备
    CARDARRAY CardArrayBuffer[4 * 13 * 4 * 13];
    ZeroMemory(CardArrayBuffer, sizeof(CardArrayBuffer));

    //混乱扑克
    WORD wRandCount = 0;
    WORD wPosition = 0;
    do
    {
        wPosition = rand() % (CountArray(keyCardArray) - wRandCount);
        CardArrayBuffer[wRandCount++] = keyCardArray[wPosition];
        keyCardArray[wPosition] = keyCardArray[CountArray(keyCardArray) - wRandCount];
    }
    while(wRandCount < 4 * 13 * 4 * 13);

    for(WORD i = 0; i < 4 * 13 * 4 * 13; i++)
    {
        m_cardArrayList.AddTail(CardArrayBuffer[i]);
    }
}

//计算分数
SCORE CServerDebugItemSink::CalculateScore(BYTE cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker)
{
    //系统输赢
    SCORE lSysWinScore = 0l;

    //区域输赢(针对下注者)
    INT nAreaWin[AREA_MAX] = {0};

    //推断区域
    //获取逻辑值
    BYTE cbDragonLogicValue = GetCardValue(cbTableCardArray[0]);
    BYTE cbTigerLogicValue = GetCardValue(cbTableCardArray[1]);

    //初始化区域
    for(int i = 0; i < AREA_MAX; i++)
    {
        nAreaWin[i] = RESULT_LOSE;
    }

    //比较两门大小
    if(cbDragonLogicValue == cbTigerLogicValue)
    {
        nAreaWin[AREA_PING] = RESULT_WIN;
    }
    else if(cbDragonLogicValue > cbTigerLogicValue)
    {
        nAreaWin[AREA_LONG] = RESULT_WIN;
    }
    else
    {
        nAreaWin[AREA_HU] = RESULT_WIN;
    }

    //区域倍率
    BYTE cbMultiple[AREA_MAX] = {MULTIPLE_LONG, MULTIPLE_PING, MULTIPLE_HU};

    //系统当庄或者机器人当庄
    if((wCurrentBanker == INVALID_CHAIR) || ((wCurrentBanker != INVALID_CHAIR) && m_pITableFrame->GetTableUserItem(wCurrentBanker) && m_pITableFrame->GetTableUserItem(wCurrentBanker)->IsAndroidUser()))
    {
        //只考虑真人玩家
        //计算积分
        SCORE lRealPlayerWinScore = 0;
		for (WORD wChairID = 0; wChairID < m_pITableFrame->GetChairCount(); wChairID++)
        {
            //获取用户
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
            if(pIServerUserItem == NULL)
            {
                continue;
            }

            if(pIServerUserItem->IsAndroidUser())
            {
                continue;
            }

            //每个人定注数
            for(WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
            {
                if(nAreaWin[wAreaIndex] == RESULT_WIN)
                {
                    lRealPlayerWinScore += (SCORE)(lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
                }
                else
                {
                    //下注龙或下注虎, 开和时退下注金额.
                    if(nAreaWin[AREA_PING] == RESULT_WIN && (wAreaIndex == AREA_LONG || wAreaIndex == AREA_HU))
                    {
                    }
                    else
                    {
                        lRealPlayerWinScore -= lUserInAllScore[wChairID][wAreaIndex];
                    }
                }
            }
        }

        lSysWinScore = -lRealPlayerWinScore;
    }
    //真人玩家做庄
    else if((wCurrentBanker != INVALID_CHAIR) && m_pITableFrame->GetTableUserItem(wCurrentBanker) && !m_pITableFrame->GetTableUserItem(wCurrentBanker)->IsAndroidUser())
    {
        //只考虑机器人玩家
        //计算积分
        SCORE lAIWinScore = 0;
		for (WORD wChairID = 0; wChairID < m_pITableFrame->GetChairCount(); wChairID++)
        {
            //获取用户
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
            if(pIServerUserItem == NULL)
            {
                continue;
            }

            if(!pIServerUserItem->IsAndroidUser())
            {
                continue;
            }

            //每个人定注数
            for(WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
            {
                if(nAreaWin[wAreaIndex] == RESULT_WIN)
                {
                    lAIWinScore += (SCORE)(lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
                }
                else
                {
                    //下注龙或下注虎, 开和时退下注金额.
                    if(nAreaWin[AREA_PING] == RESULT_WIN && (wAreaIndex == AREA_LONG || wAreaIndex == AREA_HU))
                    {
                    }
                    else
                    {
                        lAIWinScore -= lUserInAllScore[wChairID][wAreaIndex];
                    }
                }
            }
        }

        lSysWinScore = lAIWinScore;
    }

    return lSysWinScore;
}

