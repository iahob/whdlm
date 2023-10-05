#include "StdAfx.h"
#include "serverdebugitemsink.h"
#include <locale>

CServerDebugItemSink::CServerDebugItemSink(void)
{
    m_cbExcuteTimes = 0;
    m_cbDebugStyle = 0;
    m_cbWinAreaCount = 0;
    ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
    ZeroMemory(m_nCompareCard, sizeof(m_nCompareCard));
    ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
    ZeroMemory(m_cbTableCard, sizeof(m_cbTableCard));
    ZeroMemory(m_lUserJettonScore, sizeof(m_lUserJettonScore));
	m_wCurrentBanker = INVALID_CHAIR;
	m_pITableFrame = NULL;
}

CServerDebugItemSink::~CServerDebugItemSink(void)
{

}

//服务器调试
bool __cdecl CServerDebugItemSink::ServerDebug(WORD wSubCmdID, const void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption)
{
    //如果不具有管理员权限 则返回错误
    if(!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
    {
        return false;
    }

    const CMD_C_AdminReq *AdminReq = static_cast<const CMD_C_AdminReq *>(pDataBuffer);

	m_pITableFrame = pITableFrame;

    switch(AdminReq->cbReqType)
    {
    case RQ_RESET_DEBUG:
    {
        m_cbDebugStyle = 0;
        m_cbWinAreaCount = 0;
        m_cbExcuteTimes = 0;
        ZeroMemory(m_bWinArea, sizeof(m_bWinArea));

        CMD_S_CommandResult cResult;
		ZeroMemory(&cResult, sizeof(cResult));

        cResult.cbResult = CR_ACCEPT;
        cResult.cbAckType = ACK_RESET_DEBUG;
        pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

        //调试信息
        DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

        break;
    }
    case RQ_SET_WIN_AREA:
    {
        const tagAdminReq *pAdminReq = reinterpret_cast<const tagAdminReq *>(AdminReq->cbExtendData);
		switch (pAdminReq->cbDebugStyle)
        {
        case CS_BET_AREA:	//区域设置
        {
			m_cbDebugStyle = pAdminReq->cbDebugStyle;
			m_cbExcuteTimes = pAdminReq->cbExcuteTimes;
            m_cbWinAreaCount = 0;

            BYTE cbIndex = 0;
			for (cbIndex = 1; cbIndex < AREA_MAX + 1; cbIndex++)
            {
                m_bWinArea[cbIndex - 1] = pAdminReq->bWinArea[cbIndex];
				if (m_bWinArea[cbIndex - 1])
                {
                    m_cbWinAreaCount++;
                }
            }

            CMD_S_CommandResult cResult;
			ZeroMemory(&cResult, sizeof(cResult));

            cResult.cbResult = CR_ACCEPT;
            cResult.cbAckType = ACK_SET_WIN_AREA;
            pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

            //调试信息
            DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

            break;
        }
		case CS_SYS_LOSE:	//系统输牌
        {
			m_cbDebugStyle = pAdminReq->cbDebugStyle;
			m_cbExcuteTimes = pAdminReq->cbExcuteTimes;

            CMD_S_CommandResult cResult;
			ZeroMemory(&cResult, sizeof(cResult));

            cResult.cbResult = CR_ACCEPT;
            cResult.cbAckType = ACK_SET_WIN_AREA;
            pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

            //调试信息
            DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

            break;
        }
		case CS_SYS_WIN:		//系统赢牌
        {
			m_cbDebugStyle = pAdminReq->cbDebugStyle;
			m_cbExcuteTimes = pAdminReq->cbExcuteTimes;

            CMD_S_CommandResult cResult;
			ZeroMemory(&cResult, sizeof(cResult));

            cResult.cbResult = CR_ACCEPT;
            cResult.cbAckType = ACK_SET_WIN_AREA;
            pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

            //调试信息
            DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

            break;
        }
        default:	//拒绝请求
        {
            CMD_S_CommandResult cResult;
            cResult.cbResult = CR_REFUSAL;
            cResult.cbAckType = ACK_SET_WIN_AREA;
            pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

            //调试信息
            DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

            break;
        }
        }

        break;
    }
    default:
    {
        break;
    }
    }
    return true;
}

//需要调试
bool  CServerDebugItemSink::NeedDebug()
{
    if(m_cbDebugStyle > 0 && m_cbExcuteTimes > 0)
    {
        return true;
    }

    return false;
}

//调试牌型
void  CServerDebugItemSink::GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], WORD wCurrentBanker)
{
    //填充变量
    memcpy(m_cbTableCardArray, cbTableCardArray, sizeof(m_cbTableCardArray));
    memcpy(m_cbTableCard, cbTableCard, sizeof(m_cbTableCard));
	memcpy(m_lUserJettonScore, lUserJettonScore, sizeof(m_lUserJettonScore));

	m_wCurrentBanker = wCurrentBanker;

    //排序扑克
    BYTE cbSuitStack[MAX_CARDGROUP] = {};

    //自动组合
    GetSuitCardCombine(cbSuitStack);

    //重新设置纸牌排列
    BYTE UserCard[MAX_CARDGROUP][MAX_CARD] = {};
    memcpy(UserCard, m_cbTableCardArray, sizeof(UserCard));
    BYTE cbIndex = 0;
    for(cbIndex = 0; cbIndex < MAX_CARDGROUP; cbIndex++)
    {
        memcpy(cbTableCardArray[cbIndex], UserCard[cbSuitStack[cbIndex]], sizeof(BYTE)*MAX_CARD);
    }
    memcpy(cbTableCard, m_cbTableCard, sizeof(m_cbTableCard));

	if (m_cbExcuteTimes > 0)
	{
		m_cbExcuteTimes--;
	}

	if (m_cbExcuteTimes == 0)
	{
		m_cbExcuteTimes = 0;
		m_cbDebugStyle = 0;
		m_cbWinAreaCount = 0;
		ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
		ZeroMemory(m_nCompareCard, sizeof(m_nCompareCard));
		ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
		ZeroMemory(m_cbTableCard, sizeof(m_cbTableCard));
		ZeroMemory(m_lUserJettonScore, sizeof(m_lUserJettonScore));
		m_wCurrentBanker = INVALID_CHAIR;
	}
}

//是否是有效组合
bool CServerDebugItemSink::GetSuitCardCombine(BYTE cbStack[])
{
    switch(m_cbDebugStyle)
    {
    case CS_BET_AREA:
    {
        AreaWinCard(cbStack);
        break;
    }
	case CS_SYS_WIN:
    {
        SysWinCard(true, cbStack);
        break;
    }
    case CS_SYS_LOSE:
    {
        SysWinCard(false, cbStack);
        break;
    }
    }

    return true;
}

bool CServerDebugItemSink::AreaWinCard(BYTE cbStack[])
{
    bool bIsUser[MAX_CARDGROUP] = {0};	//是否已经使用
    BYTE cbStackCount = 0;	//栈中元素数量
    BYTE cbIndex = 0;
    BYTE cbMultiple = 0;
    //比较牌之间关系
    for(cbIndex = 0; cbIndex < MAX_CARDGROUP; cbIndex++)
    {
        for(BYTE j = cbIndex; j < MAX_CARDGROUP; j++)
        {

            m_nCompareCard[j][cbIndex] = m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex], MAX_CARD, m_cbTableCardArray[j], MAX_CARD, cbMultiple);
            m_nCompareCard[j][cbIndex] *= cbMultiple;
            m_nCompareCard[cbIndex][j] = -m_nCompareCard[j][cbIndex];
        }
    }

    //查找派牌组合
    for(cbIndex = 0; cbIndex <= MAX_CARDGROUP;)
    {
        if(cbIndex < MAX_CARDGROUP)
        {
            if(bIsUser[cbIndex])
            {
                cbIndex++;
            }
            else
            {
                cbStack[cbStackCount] = cbIndex;
                bIsUser[cbIndex] = true;
                cbStackCount++;
                if(cbStackCount == MAX_CARDGROUP)	//如果已经找到一种组合
                {

                    if(m_bWinArea[0] == (m_nCompareCard[cbStack[1]][cbStack[0]] > 0 ? true : false)		\
                            && m_bWinArea[1] == (m_nCompareCard[cbStack[2]][cbStack[0]] > 0 ? true : false)	\
                            && m_bWinArea[2] == (m_nCompareCard[cbStack[3]][cbStack[0]] > 0 ? true : false)	\
                            && m_bWinArea[3] == (m_nCompareCard[cbStack[4]][cbStack[0]] > 0 ? true : false))
                    {
                        break;
                    }
                    cbIndex = MAX_CARDGROUP;
                }
                else
                {
                    cbIndex = 0;
                    continue;
                }
            }
        }
        else
        {
            if(cbStackCount > 0)
            {
                cbIndex = cbStack[--cbStackCount];
                bIsUser[cbIndex] = false;
                cbIndex++;
                continue;
            }
            else
            {
                break;
            }
        }
    }
    return true;
}

LONGLONG CServerDebugItemSink::GetSysWinScore(int nWinMultiple[])
{
	//系统坐庄或者机器人坐庄
	if ((m_wCurrentBanker == INVALID_CHAIR) || (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser()))
	{
		//计算真人玩家输赢
		LONGLONG lRealPlayerWinScore = 0;
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (!pIServerUserItem)
			{
				continue;
			}

			if (pIServerUserItem->IsAndroidUser())
			{
				continue;
			}

			for (BYTE cbIndex = 1; cbIndex < AREA_MAX + 1; cbIndex++)
			{
				lRealPlayerWinScore += m_lUserJettonScore[cbIndex][wChairID] * nWinMultiple[cbIndex];
			}
		}

		return -lRealPlayerWinScore;
	}
	//真人坐庄
	else if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame && !m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser())
	{
		//计算机器人输赢
		LONGLONG lAndroidWinScore = 0;
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (!pIServerUserItem)
			{
				continue;
			}

			if (!pIServerUserItem->IsAndroidUser())
			{
				continue;
			}

			for (BYTE cbIndex = 1; cbIndex < AREA_MAX + 1; cbIndex++)
			{
				lAndroidWinScore += m_lUserJettonScore[cbIndex][wChairID] * nWinMultiple[cbIndex];
			}
		}

		return lAndroidWinScore;
	}

    return 0;
}

void CServerDebugItemSink::SysWinCard(bool bIsWin, BYTE cbStack[])
{
    bool bIsUser[MAX_CARDGROUP] = {0};	//是否已经使用
    BYTE cbStackCount = 0;	//栈中元素数量
    BYTE cbIndex = 0;
    BYTE cbMultiple;
    //比较牌之间关系
    for(cbIndex = 0; cbIndex < MAX_CARDGROUP; cbIndex++)
    {
        for(BYTE j = cbIndex; j < MAX_CARDGROUP; j++)
        {
            m_nCompareCard[j][cbIndex] = m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex], MAX_CARD, m_cbTableCardArray[j], MAX_CARD, cbMultiple);
            m_nCompareCard[j][cbIndex] *= cbMultiple;
            m_nCompareCard[cbIndex][j] = -m_nCompareCard[j][cbIndex];
        }
    }

    //查找派牌组合
    for(cbIndex = 0; cbIndex <= MAX_CARDGROUP;)
    {
        if(cbIndex < MAX_CARDGROUP)
        {
            if(bIsUser[cbIndex])
            {
                cbIndex++;
            }
            else
            {
                cbStack[cbStackCount] = cbIndex;
                bIsUser[cbIndex] = true;
                cbStackCount++;
                if(cbStackCount == MAX_CARDGROUP)	//如果已经找到一种组合
                {
                    int nWinMultiple[4];
                    for(int x = 0; x < 4; x++)
                    {
                        nWinMultiple[x] = m_nCompareCard[cbStack[x + 1]][cbStack[0]];
                    }

                    if(bIsWin && GetSysWinScore(nWinMultiple) >= 0)
                    {
                        break;
                    }
                    if(!bIsWin && GetSysWinScore(nWinMultiple) <= 0)
                    {
                        break;
                    }
                    cbIndex = MAX_CARDGROUP;
                }
                else
                {
                    cbIndex = 0;
                    continue;
                }
            }
        }
        else
        {
            if(cbStackCount > 0)
            {
                cbIndex = cbStack[--cbStackCount];
                bIsUser[cbIndex] = false;
                cbIndex++;
                continue;
            }
            else
            {
                break;
            }
        }
    }
}

//申请结果
VOID CServerDebugItemSink::DebugInfo(const void *pBuffer, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption)
{
    const CMD_S_CommandResult *pResult = (CMD_S_CommandResult *)pBuffer;
    CString str;
    switch(pResult->cbAckType)
    {
    case ACK_SET_WIN_AREA:
    {
        if(pResult->cbResult == CR_ACCEPT)
        {
            str = TEXT("输赢调试命令已经接受！");
            switch(m_cbDebugStyle)
            {
            case CS_BET_AREA:
            {
                str = TEXT("胜利区域:");
                BYTE cbIndex = 0;
                bool bFlags = false;
                for(cbIndex = 0; cbIndex < AREA_MAX; cbIndex++)
                {
                    if(m_bWinArea[cbIndex])
                    {
                        bFlags = true;
                        switch(cbIndex)
                        {
                        case 0:
                            str += TEXT("东 ");
                            break;
                        case 1:
                            str += TEXT("南 ");
                            break;
                        case 2:
                            str += TEXT("西 ");
                            break;
                        case 3:
                            str += TEXT("北 ");
                            break;
                        default:
                            break;
                        }
                    }
                }
                if(!bFlags)
                {
                    str += TEXT("庄家通杀");
                }
                str.AppendFormat(TEXT(",执行次数:%d"), m_cbExcuteTimes);
                break;
            }
			case CS_SYS_WIN:
            {
                str.Format(TEXT("系统受控,受控方式:赢,执行次数:%d"), m_cbExcuteTimes);
                break;
            }
			case CS_SYS_LOSE:
            {
                str.Format(TEXT("系统受控,受控方式:输,执行次数:%d"), m_cbExcuteTimes);
                break;
            }
            default:
                str = TEXT("非调试状态");
                break;
            }
        }
        else
        {
            str.Format(TEXT("执行调试失败!"));
        }
        break;
    }
    case ACK_RESET_DEBUG:
    {
        if(pResult->cbResult == CR_ACCEPT)
        {
            str = TEXT("取消调试!");
        }
        break;
    }
    case ACK_PRINT_SYN:
    {
        if(pResult->cbResult == CR_ACCEPT)
        {
            str = TEXT("服务器同步成功!");
        }
        else
        {
            str.Format(TEXT("服务器同步失败!"));
        }
        break;
    }

    default:
        break;
    }

    //记录信息
    CString strDebugInfo;
    CTime Time(CTime::GetCurrentTime());
    strDebugInfo.Format(TEXT("房间: %s | 桌号: %u | 时间: %d-%d-%d %d:%d:%d\n调试人账号: %s | 调试人ID: %u\n%s\r\n"),
                          pGameServiceOption->szServerName, pITableFrame->GetTableID() + 1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
                          Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

    CString strFileName;
    strFileName.Format(TEXT("百人牛牛[%s]调试信息.log"), pGameServiceOption->szServerName);
    WriteInfo(strFileName, strDebugInfo);

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
    if(bOpen)
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