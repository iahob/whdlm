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

//����������
bool __cdecl CServerDebugItemSink::ServerDebug(WORD wSubCmdID, const void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption)
{
    //��������й���ԱȨ�� �򷵻ش���
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

        //������Ϣ
        DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

        break;
    }
    case RQ_SET_WIN_AREA:
    {
        const tagAdminReq *pAdminReq = reinterpret_cast<const tagAdminReq *>(AdminReq->cbExtendData);
		switch (pAdminReq->cbDebugStyle)
        {
        case CS_BET_AREA:	//��������
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

            //������Ϣ
            DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

            break;
        }
		case CS_SYS_LOSE:	//ϵͳ����
        {
			m_cbDebugStyle = pAdminReq->cbDebugStyle;
			m_cbExcuteTimes = pAdminReq->cbExcuteTimes;

            CMD_S_CommandResult cResult;
			ZeroMemory(&cResult, sizeof(cResult));

            cResult.cbResult = CR_ACCEPT;
            cResult.cbAckType = ACK_SET_WIN_AREA;
            pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

            //������Ϣ
            DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

            break;
        }
		case CS_SYS_WIN:		//ϵͳӮ��
        {
			m_cbDebugStyle = pAdminReq->cbDebugStyle;
			m_cbExcuteTimes = pAdminReq->cbExcuteTimes;

            CMD_S_CommandResult cResult;
			ZeroMemory(&cResult, sizeof(cResult));

            cResult.cbResult = CR_ACCEPT;
            cResult.cbAckType = ACK_SET_WIN_AREA;
            pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

            //������Ϣ
            DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

            break;
        }
        default:	//�ܾ�����
        {
            CMD_S_CommandResult cResult;
            cResult.cbResult = CR_REFUSAL;
            cResult.cbAckType = ACK_SET_WIN_AREA;
            pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

            //������Ϣ
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

//��Ҫ����
bool  CServerDebugItemSink::NeedDebug()
{
    if(m_cbDebugStyle > 0 && m_cbExcuteTimes > 0)
    {
        return true;
    }

    return false;
}

//��������
void  CServerDebugItemSink::GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], WORD wCurrentBanker)
{
    //������
    memcpy(m_cbTableCardArray, cbTableCardArray, sizeof(m_cbTableCardArray));
    memcpy(m_cbTableCard, cbTableCard, sizeof(m_cbTableCard));
	memcpy(m_lUserJettonScore, lUserJettonScore, sizeof(m_lUserJettonScore));

	m_wCurrentBanker = wCurrentBanker;

    //�����˿�
    BYTE cbSuitStack[MAX_CARDGROUP] = {};

    //�Զ����
    GetSuitCardCombine(cbSuitStack);

    //��������ֽ������
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

//�Ƿ�����Ч���
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
    bool bIsUser[MAX_CARDGROUP] = {0};	//�Ƿ��Ѿ�ʹ��
    BYTE cbStackCount = 0;	//ջ��Ԫ������
    BYTE cbIndex = 0;
    BYTE cbMultiple = 0;
    //�Ƚ���֮���ϵ
    for(cbIndex = 0; cbIndex < MAX_CARDGROUP; cbIndex++)
    {
        for(BYTE j = cbIndex; j < MAX_CARDGROUP; j++)
        {

            m_nCompareCard[j][cbIndex] = m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex], MAX_CARD, m_cbTableCardArray[j], MAX_CARD, cbMultiple);
            m_nCompareCard[j][cbIndex] *= cbMultiple;
            m_nCompareCard[cbIndex][j] = -m_nCompareCard[j][cbIndex];
        }
    }

    //�����������
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
                if(cbStackCount == MAX_CARDGROUP)	//����Ѿ��ҵ�һ�����
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
	//ϵͳ��ׯ���߻�������ׯ
	if ((m_wCurrentBanker == INVALID_CHAIR) || (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser()))
	{
		//�������������Ӯ
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
	//������ׯ
	else if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame && !m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser())
	{
		//�����������Ӯ
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
    bool bIsUser[MAX_CARDGROUP] = {0};	//�Ƿ��Ѿ�ʹ��
    BYTE cbStackCount = 0;	//ջ��Ԫ������
    BYTE cbIndex = 0;
    BYTE cbMultiple;
    //�Ƚ���֮���ϵ
    for(cbIndex = 0; cbIndex < MAX_CARDGROUP; cbIndex++)
    {
        for(BYTE j = cbIndex; j < MAX_CARDGROUP; j++)
        {
            m_nCompareCard[j][cbIndex] = m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex], MAX_CARD, m_cbTableCardArray[j], MAX_CARD, cbMultiple);
            m_nCompareCard[j][cbIndex] *= cbMultiple;
            m_nCompareCard[cbIndex][j] = -m_nCompareCard[j][cbIndex];
        }
    }

    //�����������
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
                if(cbStackCount == MAX_CARDGROUP)	//����Ѿ��ҵ�һ�����
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

//������
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
            str = TEXT("��Ӯ���������Ѿ����ܣ�");
            switch(m_cbDebugStyle)
            {
            case CS_BET_AREA:
            {
                str = TEXT("ʤ������:");
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
                            str += TEXT("�� ");
                            break;
                        case 1:
                            str += TEXT("�� ");
                            break;
                        case 2:
                            str += TEXT("�� ");
                            break;
                        case 3:
                            str += TEXT("�� ");
                            break;
                        default:
                            break;
                        }
                    }
                }
                if(!bFlags)
                {
                    str += TEXT("ׯ��ͨɱ");
                }
                str.AppendFormat(TEXT(",ִ�д���:%d"), m_cbExcuteTimes);
                break;
            }
			case CS_SYS_WIN:
            {
                str.Format(TEXT("ϵͳ�ܿ�,�ܿط�ʽ:Ӯ,ִ�д���:%d"), m_cbExcuteTimes);
                break;
            }
			case CS_SYS_LOSE:
            {
                str.Format(TEXT("ϵͳ�ܿ�,�ܿط�ʽ:��,ִ�д���:%d"), m_cbExcuteTimes);
                break;
            }
            default:
                str = TEXT("�ǵ���״̬");
                break;
            }
        }
        else
        {
            str.Format(TEXT("ִ�е���ʧ��!"));
        }
        break;
    }
    case ACK_RESET_DEBUG:
    {
        if(pResult->cbResult == CR_ACCEPT)
        {
            str = TEXT("ȡ������!");
        }
        break;
    }
    case ACK_PRINT_SYN:
    {
        if(pResult->cbResult == CR_ACCEPT)
        {
            str = TEXT("������ͬ���ɹ�!");
        }
        else
        {
            str.Format(TEXT("������ͬ��ʧ��!"));
        }
        break;
    }

    default:
        break;
    }

    //��¼��Ϣ
    CString strDebugInfo;
    CTime Time(CTime::GetCurrentTime());
    strDebugInfo.Format(TEXT("����: %s | ����: %u | ʱ��: %d-%d-%d %d:%d:%d\n�������˺�: %s | ������ID: %u\n%s\r\n"),
                          pGameServiceOption->szServerName, pITableFrame->GetTableID() + 1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
                          Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

    CString strFileName;
    strFileName.Format(TEXT("����ţţ[%s]������Ϣ.log"), pGameServiceOption->szServerName);
    WriteInfo(strFileName, strDebugInfo);

    return;
}

//��¼��Ϣ
VOID CServerDebugItemSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
    //������������
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

    //��ԭ�����趨
    setlocale(LC_CTYPE, old_locale);
    free(old_locale);

    return;
}