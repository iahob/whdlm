#include "StdAfx.h"
#include "serverdebugitemsink.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////

const BYTE CServerDebugItemSink::m_cbCardData[4][13] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//���� A - K
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,	//÷�� A - K
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,	//���� A - K
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,	//���� A - K
};

CServerDebugItemSink::CServerDebugItemSink(void)
{
    m_cbWinSideDebug = 0xFF;
    m_cbExcuteTimes = 0;
    m_pITableFrame = NULL;

    //�������
    srand(time(NULL));
}

CServerDebugItemSink::~CServerDebugItemSink(void)
{
}

//����������
bool __cdecl CServerDebugItemSink::ServerDebug(WORD wSubCmdID, const void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption)
{
    //Ч������
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

		//������Ϣ
		DebugInfo(pAreaCtrl->bCancelAreaCtrl, pIServerUserItem, pITableFrame, pGameServiceOption);
	}
	else
	{
		m_cbWinSideDebug = pAreaCtrl->cbCtrlPosIndex;
		m_cbExcuteTimes = pAreaCtrl->cbConfigCtrlTimes;

		//������Ϣ
		DebugInfo(pAreaCtrl->bCancelAreaCtrl, pIServerUserItem, pITableFrame, pGameServiceOption);
	}

    return true;
}

//��Ҫ����
bool __cdecl CServerDebugItemSink::NeedDebug(BYTE &cbWinSide)
{
	cbWinSide = m_cbWinSideDebug;
	if (m_cbWinSideDebug >= 0 && m_cbWinSideDebug < AREA_MAX + 2 && m_cbExcuteTimes > 0)
	{
		return true;
	}

	return false;
}

//���µ���
void  CServerDebugItemSink::DebugInfo(bool bCancelAreaCtrl, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption)
{
	CString str;
	if (!bCancelAreaCtrl)
	{
		switch (m_cbWinSideDebug)
		{
		case 0:
			str.Format(TEXT("ִ�е��Գɹ�,ʤ������:��,ִ�д���:%d"), m_cbExcuteTimes);
			break;
		case 1:
			str.Format(TEXT("ִ�е��Գɹ�,ʤ������:��,ִ�д���:%d"), m_cbExcuteTimes);
			break;
		case 2:
			str.Format(TEXT("ִ�е��Գɹ�,ʤ������:��,ִ�д���:%d"), m_cbExcuteTimes);
			break;
		case 3:
			str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����ϵͳ��,ִ�д���:%d"), m_cbExcuteTimes);
			break;
		case 4:
			str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����ϵͳӮ,ִ�д���:%d"), m_cbExcuteTimes);
			break;

		default:
			break;
		}
	}
	else
	{
		str = TEXT("ȡ������!");
	}

    //��¼��Ϣ
    CString strDebugInfo;
    CTime Time(CTime::GetCurrentTime());
    strDebugInfo.Format(TEXT("����: %s | ����: %u | ʱ��: %d-%d-%d %d:%d:%d\n�������˺�: %s | ������ID: %u\n%s\n"),
                        pGameServiceOption->szServerName, pITableFrame->GetTableID() + 1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
                        Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

    WriteInfo(TEXT("������������Ϣ.log"), strDebugInfo);

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
	if (bOpen)
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

//���ؿ�������
bool CServerDebugItemSink::DebugResult(BYTE	cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker)
{
	//����
	if (m_cbWinSideDebug >= AREA_LONG && m_cbWinSideDebug < AREA_MAX)
	{
		//��ʼ���˿�
		m_cardArrayList.RemoveAll();
		InitCardArray();
		
		switch (m_cbWinSideDebug)
		{
		case AREA_LONG:
			{
				//��������
				POSITION ptHead = m_cardArrayList.GetHeadPosition();
				do
				{
					CARDARRAY cardArray = m_cardArrayList.GetAt(ptHead);
					POSITION ptTemp = ptHead;
					m_cardArrayList.GetNext(ptHead);
					//ɾ�����з���Ӯ��Ԫ��
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
				//��������
				POSITION ptHead = m_cardArrayList.GetHeadPosition();
				do
				{
					CARDARRAY cardArray = m_cardArrayList.GetAt(ptHead);
					POSITION ptTemp = ptHead;
					m_cardArrayList.GetNext(ptHead);
					//ɾ�����зǺ�Ӯ��Ԫ��
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
				//��������
				POSITION ptHead = m_cardArrayList.GetHeadPosition();
				do
				{
					CARDARRAY cardArray = m_cardArrayList.GetAt(ptHead);
					POSITION ptTemp = ptHead;
					m_cardArrayList.GetNext(ptHead);
					//ɾ�����зǻ�Ӯ��Ԫ��
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
		
		//�����˿�
		WORD wCountIndex = m_cardArrayList.GetSize();
		ASSERT(wCountIndex != 0);
		POSITION keyPT = m_cardArrayList.FindIndex(rand() % wCountIndex);
		CARDARRAY cardArray = m_cardArrayList.GetAt(keyPT);
		CopyMemory(cbTableCardArray, cardArray.cbCardArray, sizeof(cardArray.cbCardArray));
	}
	//ϵͳ���Ӯ
	else if (m_cbWinSideDebug >= AREA_SYSLOST && m_cbWinSideDebug <= AREA_SYSWIN)
	{
		//��ʼ���˿�
		m_cardArrayList.RemoveAll();
		InitCardArray();
		
		//ϵͳӮ
		if (m_cbWinSideDebug == AREA_SYSWIN)
		{
		    //��������
		    POSITION ptHead = m_cardArrayList.GetHeadPosition();
		    do
		    {
		        CARDARRAY cardArray = m_cardArrayList.GetNext(ptHead);
		        BYTE cbTempTableCardArray[2];
		        ZeroMemory(cbTempTableCardArray, sizeof(cbTempTableCardArray));
		        cbTempTableCardArray[0] = cardArray.cbCardArray[0];
		        cbTempTableCardArray[1] = cardArray.cbCardArray[1];
		
		        //ϵͳ�÷�
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
			//��������
			POSITION ptHead = m_cardArrayList.GetHeadPosition();
			do
			{
				CARDARRAY cardArray = m_cardArrayList.GetNext(ptHead);
				BYTE cbTempTableCardArray[2];
				ZeroMemory(cbTempTableCardArray, sizeof(cbTempTableCardArray));
				cbTempTableCardArray[0] = cardArray.cbCardArray[0];
				cbTempTableCardArray[1] = cardArray.cbCardArray[1];

				//ϵͳ�÷�
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
    //ʹ�ð˸��ƣ�����������ƿ���һģһ��
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

    //����׼��
    CARDARRAY CardArrayBuffer[4 * 13 * 4 * 13];
    ZeroMemory(CardArrayBuffer, sizeof(CardArrayBuffer));

    //�����˿�
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

//�������
SCORE CServerDebugItemSink::CalculateScore(BYTE cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker)
{
    //ϵͳ��Ӯ
    SCORE lSysWinScore = 0l;

    //������Ӯ(�����ע��)
    INT nAreaWin[AREA_MAX] = {0};

    //�ƶ�����
    //��ȡ�߼�ֵ
    BYTE cbDragonLogicValue = GetCardValue(cbTableCardArray[0]);
    BYTE cbTigerLogicValue = GetCardValue(cbTableCardArray[1]);

    //��ʼ������
    for(int i = 0; i < AREA_MAX; i++)
    {
        nAreaWin[i] = RESULT_LOSE;
    }

    //�Ƚ����Ŵ�С
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

    //������
    BYTE cbMultiple[AREA_MAX] = {MULTIPLE_LONG, MULTIPLE_PING, MULTIPLE_HU};

    //ϵͳ��ׯ���߻����˵�ׯ
    if((wCurrentBanker == INVALID_CHAIR) || ((wCurrentBanker != INVALID_CHAIR) && m_pITableFrame->GetTableUserItem(wCurrentBanker) && m_pITableFrame->GetTableUserItem(wCurrentBanker)->IsAndroidUser()))
    {
        //ֻ�����������
        //�������
        SCORE lRealPlayerWinScore = 0;
		for (WORD wChairID = 0; wChairID < m_pITableFrame->GetChairCount(); wChairID++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
            if(pIServerUserItem == NULL)
            {
                continue;
            }

            if(pIServerUserItem->IsAndroidUser())
            {
                continue;
            }

            //ÿ���˶�ע��
            for(WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
            {
                if(nAreaWin[wAreaIndex] == RESULT_WIN)
                {
                    lRealPlayerWinScore += (SCORE)(lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
                }
                else
                {
                    //��ע������ע��, ����ʱ����ע���.
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
    //���������ׯ
    else if((wCurrentBanker != INVALID_CHAIR) && m_pITableFrame->GetTableUserItem(wCurrentBanker) && !m_pITableFrame->GetTableUserItem(wCurrentBanker)->IsAndroidUser())
    {
        //ֻ���ǻ��������
        //�������
        SCORE lAIWinScore = 0;
		for (WORD wChairID = 0; wChairID < m_pITableFrame->GetChairCount(); wChairID++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
            if(pIServerUserItem == NULL)
            {
                continue;
            }

            if(!pIServerUserItem->IsAndroidUser())
            {
                continue;
            }

            //ÿ���˶�ע��
            for(WORD wAreaIndex = AREA_LONG; wAreaIndex < AREA_MAX; ++wAreaIndex)
            {
                if(nAreaWin[wAreaIndex] == RESULT_WIN)
                {
                    lAIWinScore += (SCORE)(lUserInAllScore[wChairID][wAreaIndex] * (cbMultiple[wAreaIndex]));
                }
                else
                {
                    //��ע������ע��, ����ʱ����ע���.
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

