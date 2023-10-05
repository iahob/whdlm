#include "StdAfx.h"
#include "serverdebugitemsink.h"
#include <locale>

CServerDebugItemSink::CServerDebugItemSink(void)
{
	//���Ʊ���
	m_cbWinSideDebug = 0xFF;
	m_cbExcuteTimes = 0;
	m_pITableFrame = NULL;
}

CServerDebugItemSink::~CServerDebugItemSink( void )
{

}

//����������
bool  CServerDebugItemSink::ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption)
{		
	//Ч������
	if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		return false;

	ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
	if (wDataSize != sizeof(CMD_C_AdminReq))
		return false;

	ASSERT(pITableFrame != NULL);
	if (pITableFrame != NULL)
	{
		m_pITableFrame = pITableFrame;
	}

	CMD_C_AdminReq* AdminReq = (CMD_C_AdminReq*)pDataBuffer;

	switch (AdminReq->cbReqType)
	{
	case RQ_RESET_DEBUG:
	{
		m_cbWinSideDebug = 0xFF;
		m_cbExcuteTimes = 0;
		CMD_S_CommandResult cResult;
		cResult.cbAckType = ACK_RESET_DEBUG;
		cResult.cbResult = CR_ACCEPT;
		pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

		//������Ϣ
		DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

		break;
	}
	case RQ_SET_WIN_AREA:
	{
		if (AdminReq->cbExtendData[0] >= 0 && AdminReq->cbExtendData[0] < AREA_MAX + 2)
		{
			m_cbWinSideDebug = AdminReq->cbExtendData[0];
			m_cbExcuteTimes = AdminReq->cbExtendData[1];
			CMD_S_CommandResult cResult;
			cResult.cbAckType = ACK_SET_WIN_AREA;
			cResult.cbResult = CR_ACCEPT;
			cResult.cbExtendData[0] = AdminReq->cbExtendData[0];
			cResult.cbExtendData[1] = AdminReq->cbExtendData[1];
			pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

			//������Ϣ
			DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
		}
		else
		{
			CMD_S_CommandResult cResult;
			cResult.cbAckType = ACK_SET_WIN_AREA;
			cResult.cbResult = CR_REFUSAL;
			pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

			//������Ϣ
			DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
		}
		break;
	}
	default:
		break;
	}

	return true;
}

//���µ���
void  CServerDebugItemSink::DebugInfo(const void * pBuffer, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption)
{
	const CMD_S_CommandResult*pResult = (CMD_S_CommandResult*)pBuffer;
	CString str;
	switch (pResult->cbAckType)
	{
	case ACK_SET_WIN_AREA:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			switch (m_cbWinSideDebug + 1)
			{
			case ID_TU_ZI:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_YAN_ZI:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_GE_ZI:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_KONG_QUE:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:��ȸ,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_LAO_YING:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:��ӥ,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_SHI_ZI:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:ʨ��,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_XIONG_MAO:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:��è,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_HOU_ZI:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_SHA_YU:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_FEI_QIN:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_ZOU_SHOU:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����,ִ�д���:%d"), m_cbExcuteTimes); break;
			case ID_GLOD_SHA:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����,ִ�д���:%d"), m_cbExcuteTimes); break;
			case AREA_SYSLOST:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����ϵͳ��,ִ�д���:%d"), m_cbExcuteTimes); break;
			case AREA_SYSWIN:str.Format(TEXT("ִ�е��Գɹ�,ʤ������:����ϵͳӮ,ִ�д���:%d"), m_cbExcuteTimes); break;

			default:
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
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("ȡ������!");
		}
		break;
	}
	case ACK_PRINT_SYN:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("������ͬ���ɹ�!");
		}
		else
		{
			str.Format(TEXT("������ͬ��ʧ��!"));
		}
		break;
	}

	default: break;
	}

	//��¼��Ϣ
	CString strDebugInfo;
	CTime Time(CTime::GetCurrentTime());
	strDebugInfo.Format(TEXT("����: %s | ����: %u | ʱ��: %d-%d-%d %d:%d:%d\n�������˺�: %s | ������ID: %u\n%s\n"),
		pGameServiceOption->szServerName, pITableFrame->GetTableID() + 1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
		Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

	WriteInfo(TEXT("�������޵�����Ϣ.log"), strDebugInfo);

	return;
}

//��Ҫ����
bool CServerDebugItemSink::NeedDebug()
{
	if (m_cbWinSideDebug >= 0 && m_cbWinSideDebug < AREA_MAX + 2 && m_cbExcuteTimes > 0)
	{
		return true;
	}
	return false;
}

//���ؿ�������
bool CServerDebugItemSink::DebugResult(BYTE cbTableCardArray[2], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], ITableFrame *pITableFrame)
{
	//����
	if (m_cbWinSideDebug >= 0 && m_cbWinSideDebug < AREA_MAX)
	{
		cbTableCardArray[0] = GetRealArea(m_cbWinSideDebug);

		//���� �������
		if (cbTableCardArray[0] % 7 == 4)
		{
			cbTableCardArray[1] = GetAnotherArea();
		}

	}
	//ϵͳ���Ӯ
	else if (m_cbWinSideDebug + 1 >= AREA_SYSLOST && m_cbWinSideDebug + 1 <= AREA_SYSWIN)
	{
		//ϵͳ��Ӯ
		LONGLONG lSystemScore = 0l;		//ϵͳʵ����Ӯ��AI���㣩
		LONGLONG lAllScore = 0l;		//ϵͳȫ����Ӯ(AI�����һ����)

		for (BYTE i = 0; i < 14; i++)
		{
			cbTableCardArray[0] = GetRealArea(i);

			//���� �������
			if (cbTableCardArray[0] % 7 == 4)
			{
				cbTableCardArray[1] = GetAnotherArea();
			}

			lSystemScore = 0l;
			lAllScore = 0l;

			JudgeSystemScore(cbTableCardArray, lSystemScore, lAllScore, lUserJettonScore, pITableFrame);

			if ((m_cbWinSideDebug + 1 == AREA_SYSWIN && lAllScore > 0) || (m_cbWinSideDebug + 1 == AREA_SYSLOST && lAllScore < 0))
			{
				break;
			}
		}
	}

	m_cbExcuteTimes--;

	if (m_pITableFrame)
	{
		CMD_S_CommandResult cResult;
		ZeroMemory(&cResult, sizeof(cResult));

		cResult.cbAckType = ACK_PRINT_SYN;
		cResult.cbResult = CR_ACCEPT;
		cResult.cbExtendData[0] = m_cbWinSideDebug;
		cResult.cbExtendData[1] = m_cbExcuteTimes;
		m_pITableFrame->SendRoomData(NULL, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
	}

	return true;
}

//��������
BYTE CServerDebugItemSink::GetAnotherArea()
{
	BYTE cbDebugArea[24] = {1,2,3,5, 6, 7,8,9,10,12,13,14,15,16,17,19, 20, 21,22,23,24,26,27,28};
	BYTE cbIndex=rand()%24;
	ASSERT(cbDebugArea[cbIndex] != 4 && cbDebugArea[cbIndex] != 11 && cbDebugArea[cbIndex] != 18 && cbDebugArea[cbIndex] != 25);
	return cbDebugArea[cbIndex];
}
//ʵ������
BYTE CServerDebugItemSink::GetRealArea(BYTE cbNeedDebugArea)
{
	BYTE cbRealArea=0;

	if (0 == cbNeedDebugArea)//����
	{
		BYTE cbDebugArea[3] = { 1, 2, 3 };
		cbRealArea = cbDebugArea[rand() % 3];
	}
	else if (1 == cbNeedDebugArea)//����
	{
		BYTE cbDebugArea[3] = {5, 6, 7};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(2==cbNeedDebugArea)//����
	{
		BYTE cbDebugArea[3] = {8, 9, 10};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(3==cbNeedDebugArea)//��ȸ
	{
		BYTE cbDebugArea[3] = {12, 13, 14};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(4==cbNeedDebugArea)//��ӥ
	{
		BYTE cbDebugArea[3] = {15, 16, 17};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(5==cbNeedDebugArea)//ʨ��
	{
		BYTE cbDebugArea[3] = {19, 20, 21};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(6==cbNeedDebugArea)//��è
	{
		BYTE cbDebugArea[3] = {22, 23, 24};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(7==cbNeedDebugArea)//����
	{
		BYTE cbDebugArea[3] = {26, 27, 28};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(8==cbNeedDebugArea)//����
	{
		cbRealArea = 18;
	}
	else if (9 == cbNeedDebugArea)//����
	{
		BYTE cbDebugArea[12] = { 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17 };
		cbRealArea = cbDebugArea[rand() % 12];
	}
	else if (10 == cbNeedDebugArea)//����
	{
		BYTE cbDebugArea[12] = { 1, 2, 3, 19, 20, 21, 22, 23, 24, 26, 27, 28 };
		cbRealArea = cbDebugArea[rand() % 12];
	}
	else if(11==cbNeedDebugArea)//�ƽ������
	{
		//4 �ƽ������ ; 11 ͨɱ;  18 ������;  25 ͨ��;
		cbRealArea = 4;
	}
	//else if(10==cbNeedDebugArea)//ͨɱ
	//{
	//	cbRealArea = 11;
	//}
	//else if(11==cbNeedDebugArea)//ͨ��
	//{
	//	cbRealArea = 25;
	//}


	return cbRealArea;
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

// �ж���Ӯ
void CServerDebugItemSink::JudgeSystemScore(BYTE cbTableCardArray[2], LONGLONG& lSystemScore, LONGLONG& lAllScore, LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], ITableFrame *pITableFrame)
{
	BYTE  bcResulteOut[AREA_MAX];
	memset(bcResulteOut, 0, AREA_MAX);
	m_GameLogic.GetCardType(cbTableCardArray[0], 1, bcResulteOut);
	BYTE  bcResultTwo[AREA_MAX];
	memset(bcResultTwo, 0, AREA_MAX);
	m_GameLogic.GetCardType(cbTableCardArray[1], 1, bcResultTwo);

	//���㱶��
	if (cbTableCardArray[0] == 4 && bcResulteOut[11] == 100)	//����
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
		bcResulteOut[ID_SHA_YU - 1] = 100;
	}
	else if (cbTableCardArray[0] == 11 && bcResulteOut[10] == 255)	//ͨɱ
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
	}
	else if (cbTableCardArray[0] == 25 && bcResulteOut[9] == 1)	//ͨ��
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
		for (BYTE i = 0; i<AREA_MAX; i++)
		{
			if (i == 0 || i == 1) bcResulteOut[i] = 6;
			if (i == 2 || i == 3) bcResulteOut[i] = 8;
			if (i == 4 || i == 5) bcResulteOut[i] = 12;
			if (i == 6 || i == 7) bcResulteOut[i] = 8;
			if (i == 8) bcResulteOut[i] = 24;
			if (i == 9 || i == 10) bcResulteOut[i] = 2;
		}
	}
	else	//����
	{
		for (int i = 0; i<AREA_MAX; i++)
		{
			bcResulteOut[i] += bcResultTwo[i];
		}

		
		INT cbDebugArea[74] = { 6, 7, 8, 9,
			10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
			20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
			30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
			40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
			50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
			60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
			70, 71, 72, 73, 74, 75, 76, 77, 78, 79 };
		BYTE cbShaYuAddMulti = cbDebugArea[rand() % 74];

		//���� ���⽱��
		if (cbTableCardArray[0] % 7 == 4)
		{
			bcResulteOut[ID_SHA_YU - 1] += cbShaYuAddMulti;
		}
	}
	//�����ע
	LONGLONG *pUserScore[AREA_MAX + 1];

	for (int i = 1; i<AREA_MAX + 1; i++)
	{
		pUserScore[i] = lUserJettonScore[i];
	}

	//�������
	for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
			continue;

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		for (WORD wAreaIndex = 1; wAreaIndex <= AREA_MAX; ++wAreaIndex)
		{

			if (bcResulteOut[wAreaIndex - 1]>1)
			{
				//����
				if (!bIsAndroidUser)
				{
					lSystemScore -= (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex - 1] - 1));
				}
				lAllScore -= (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex - 1] - 1));
			}
			else
			{
				if (bcResulteOut[wAreaIndex - 1] == 0)
				{
					//����
					if (!bIsAndroidUser)
					{
						lSystemScore += pUserScore[wAreaIndex][wChairID];
					}
					lAllScore += pUserScore[wAreaIndex][wChairID];
				}
			}
		}
	}
}