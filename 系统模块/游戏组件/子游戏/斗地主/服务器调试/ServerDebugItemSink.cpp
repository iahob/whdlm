#include "StdAfx.h"
#include "ServerDebugitemsink.h"

// ��Ϣ����
#define	IPC_PACKAGE					4096

//�˿�����
const BYTE	g_cbCardData[FULL_COUNT] =
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//���� A - K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,	//÷�� A - K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,	//���� A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,	//���� A - K
	0x4E, 0x4F,
};

//
CServerDebugItemSink::CServerDebugItemSink(void)
{
	m_dwDebugID = 0;
	if (m_ArrayDebugCard.GetCount() == 0)
	{
		//����С����16��
		tagDebugCardInfo DebugCardInfo;
		DebugCardInfo.cbCardData[0] = 0x4e;
		DebugCardInfo.cbCardData[1] = 0x4f;
		DebugCardInfo.cbCardCount = 2;
		DebugCardInfo.cbDebugCount = 2;
		DebugCardInfo.nCardLevel = 16;
		AddGoodCard(DebugCardInfo);

		//����1-K
		for (int i = 1; i <= 13; i++)
		{
			DebugCardInfo.cbCardData[0] = 0x00 + i;
			DebugCardInfo.cbCardData[1] = 0x10 + i;
			DebugCardInfo.cbCardData[2] = 0x20 + i;
			DebugCardInfo.cbCardData[3] = 0x30 + i;
			DebugCardInfo.cbCardCount = 4;
			DebugCardInfo.cbDebugCount = 4;
			DebugCardInfo.nCardLevel = i < 3 ? 13 + i : i;
			AddGoodCard(DebugCardInfo);
		}

		//����1-2
		for (int i = 1; i <= 2; i++)
		{
			DebugCardInfo.cbCardData[0] = 0x00 + i;
			DebugCardInfo.cbCardData[1] = 0x10 + i;
			DebugCardInfo.cbCardData[2] = 0x20 + i;
			DebugCardInfo.cbCardData[3] = 0x30 + i;
			DebugCardInfo.cbCardCount = 4;
			DebugCardInfo.cbDebugCount = 3;
			DebugCardInfo.nCardLevel = i * 2 - 1;
			AddGoodCard(DebugCardInfo);
		}

		//����1-2
		for (int i = 1; i <= 2; i++)
		{
			DebugCardInfo.cbCardData[0] = 0x00 + i;
			DebugCardInfo.cbCardData[1] = 0x10 + i;
			DebugCardInfo.cbCardData[2] = 0x20 + i;
			DebugCardInfo.cbCardData[3] = 0x30 + i;
			DebugCardInfo.cbCardCount = 4;
			DebugCardInfo.cbDebugCount = 2;
			DebugCardInfo.nCardLevel = i;
			AddGoodCard(DebugCardInfo);
		}

		//������2��
		DebugCardInfo.cbCardData[0] = 0x4f;
		DebugCardInfo.cbCardCount = 1;
		DebugCardInfo.cbDebugCount = 1;
		DebugCardInfo.nCardLevel = 2;
		AddGoodCard(DebugCardInfo);

		//С����1��
		DebugCardInfo.cbCardData[0] = 0x4e;
		DebugCardInfo.cbCardCount = 1;
		DebugCardInfo.cbDebugCount = 1;
		DebugCardInfo.nCardLevel = 1;
		AddGoodCard(DebugCardInfo);
	}
}

CServerDebugItemSink::~CServerDebugItemSink( void )
{

}

bool __cdecl CServerDebugItemSink::ServerDebug(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame)
{
	//�����û�
	if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
	{
		return false;
	}
	switch (wSubCmdID)
	{
	case SUB_C_REQUEST_CUR_DEBUG:		//��ѯ��ǰ����
	{
		//Ч������
		ASSERT(wDataSize == 0);
		if (wDataSize != 0) return false;

		// ���͵�����Ϣ
		WORD wBufferSize = 0;
		byte cbBuffer[IPC_PACKAGE];
		int nSendIndex = 0;
		int nSendCount = 0;
		CMD_S_CurDebugInfo CMDSCurDebugInfo;

		while (nSendIndex < m_ArrayDebug.GetCount())
		{
			//��������
			tagDebugInfo &DebugInfo = m_ArrayDebug[nSendIndex];

			// ��������
			if (wBufferSize + sizeof(CMD_S_CurDebugInfo) > sizeof(cbBuffer))
			{
				// ������Ϣ
				pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_CUR_DEBUG, cbBuffer, wBufferSize);

				// ������Ϣ
				nSendCount = 0;
				wBufferSize = 0;
			}

			if (!DebugInfo.bWaitDelete)
			{
				// ������Ϣ
				CMDSCurDebugInfo.dwDebugID = DebugInfo.dwDebugID;
				CMDSCurDebugInfo.wTableID = DebugInfo.wTableID;
				CMDSCurDebugInfo.nCardLevel = DebugInfo.nCardLevel;
				CMDSCurDebugInfo.cbCardChance = DebugInfo.cbCardChance;
				CMDSCurDebugInfo.lSystemScore = DebugInfo.lSystemScore;
				CMDSCurDebugInfo.nStartTime = DebugInfo.nStartTime;
				CMDSCurDebugInfo.nEndTime = DebugInfo.nEndTime;
				CopyMemory(cbBuffer + wBufferSize, &CMDSCurDebugInfo, sizeof(CMDSCurDebugInfo));
				wBufferSize += sizeof(CMD_S_CurDebugInfo);
				nSendCount++;
			}

			// ��һ��
			nSendIndex++;			
		}

		// ��Ч��Ϣ
		if (wBufferSize > 0)
		{
			// ������Ϣ
			pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_CUR_DEBUG, cbBuffer, wBufferSize);
		}

		// ������Ϣ
		pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_CUR_DEBUG);

		return true;
	}
	case SUB_C_REQUEST_HISTORY_DEBUG:	//��ѯ��ʷ����
	{
		//Ч������
		ASSERT(wDataSize == 0);
		if (wDataSize != 0) return false;

		// ���͵�����Ϣ
		WORD wBufferSize = 0;
		byte cbBuffer[IPC_PACKAGE];
		int nSendIndex = 0;
		int nSendCount = 0;
		CMD_S_HistoryDebugInfo CMDSHistoryDebugInfo;

		while (nSendIndex < m_ArrayHistoryDebug.GetCount())
		{
			//��������
			tagHistoryDebugInfo &HistoryDebugInfo = m_ArrayHistoryDebug[nSendIndex];

			// ��������
			if (wBufferSize + sizeof(CMD_S_HistoryDebugInfo) > sizeof(cbBuffer))
			{
				// ������Ϣ
				pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_HISTORY_DEBUG, cbBuffer, wBufferSize);

				// ������Ϣ
				nSendCount = 0;
				wBufferSize = 0;
			}

			// ������Ϣ
			CMDSHistoryDebugInfo.dwDebugID = HistoryDebugInfo.dwDebugID;
			CMDSHistoryDebugInfo.wTableID = HistoryDebugInfo.wTableID;
			CMDSHistoryDebugInfo.nCardLevel = HistoryDebugInfo.nCardLevel;
			CMDSHistoryDebugInfo.cbCardChance = HistoryDebugInfo.cbCardChance;
			CMDSHistoryDebugInfo.lSystemScore = HistoryDebugInfo.lSystemScore;
			CopyMemory(CMDSHistoryDebugInfo.nDebugTime, HistoryDebugInfo.nDebugTime, sizeof(CMDSHistoryDebugInfo.nDebugTime));
			CMDSHistoryDebugInfo.nStartTime = HistoryDebugInfo.nStartTime;
			CMDSHistoryDebugInfo.nEndTime = HistoryDebugInfo.nEndTime;
			CopyMemory(cbBuffer + wBufferSize, &CMDSHistoryDebugInfo, sizeof(CMDSHistoryDebugInfo));
			wBufferSize += sizeof(CMD_S_HistoryDebugInfo);

			// ��һ��
			nSendIndex++;
			nSendCount++;
		}

		// ��Ч��Ϣ
		if (wBufferSize > 0)
		{
			// ������Ϣ
			pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_HISTORY_DEBUG, cbBuffer, wBufferSize);
		}

		// ������Ϣ
		pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_HISTORY_DEBUG);

		return true;
	}
	case SUB_C_REQUEST_ADD_DEBUG:		//���ӵ���
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_AddDebug));
		if (wDataSize != sizeof(CMD_C_AddDebug)) return false;

		CMD_C_AddDebug *pAddDebug = (CMD_C_AddDebug *)pData;
		
		return AddDebug(pITableFrame, pIServerUserItem, pAddDebug);
	}
	case SUB_C_REQUEST_MODIFY_DEBUG:	//�޸ĵ���
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_ModifyDebug));
		if (wDataSize != sizeof(CMD_C_ModifyDebug)) return false;

		CMD_C_ModifyDebug *pModifyDebug = (CMD_C_ModifyDebug *)pData;

		return ModifyDebug(pITableFrame, pIServerUserItem, pModifyDebug);
	}
	case SUB_C_REQUEST_DELETE_DEBUG:	//ɾ������
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_DeleteDebug));
		if (wDataSize != sizeof(CMD_C_DeleteDebug)) return false;

		CMD_C_DeleteDebug *pDeleteDebug = (CMD_C_DeleteDebug *)pData;

		return DeleteDebug(pITableFrame, pIServerUserItem, pDeleteDebug->dwDebugID);
	}
	}
	return false;
}

//����������
bool __cdecl CServerDebugItemSink::ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame )
{
	for(WORD i = 0; i < GAME_PLAYER ;i++)
	{
		IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(i);
		ASSERT(pIServerUserItem);
		if(pIServerUserItem)
		{
			//�����û�
			if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
			{
				ServerDebug(pDebugCard, pITableFrame, pIServerUserItem);
			}
		}
	}

	//WB��
	WORD wEnumIndex=0;
	do
	{
		IServerUserItem * pIServerUserItem = pITableFrame->EnumLookonUserItem(wEnumIndex++);
		if(pIServerUserItem == NULL) break;
		//�����û�
		if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		{
			ServerDebug(pDebugCard, pITableFrame, pIServerUserItem);
		}
	}while(TRUE);

	return true;
}
//����������
bool __cdecl CServerDebugItemSink::ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem)
{
	pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_DEBUG_CARD,pDebugCard,sizeof(CMD_S_DebugCard));
	return true;
}

//���Ƶ���
bool __cdecl CServerDebugItemSink::GetDebugInfo(ITableFrame * pITableFrame, BYTE cbBankerCard[3], BYTE cbHandCardCount[GAME_PLAYER], BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT])
{
	//�Ƿ����
	int nIndex = GetDebugIndex(pITableFrame, false);
	if (nIndex >= 0)
	{
		tagDebugInfo &DebugInfo = m_ArrayDebug[nIndex];
		if (DebugInfo.cbCardChance > rand() % 100)
		{
			//��С�����
			int nMinRandCount = GetMinRandCount(DebugInfo.nCardLevel);
			int nMaxRandCount = 6;
			int nMaxCardLevel = 0;
			int nCardLevel = 0;			
			BYTE cbMaxCardData[MAX_COUNT] = { 0 };
			BYTE cbMaxCardCount = 0;

			BYTE cbCardData[NORMAL_COUNT] = { 0 };
			BYTE cbCardDataTemp[NORMAL_COUNT] = { 0 };
			BYTE cbCardCount = 0;

			int nRandIndex = 0;
			bool bAddCard = true;
			int nDebugCardCountTemp = 0;

			//�����
			for (int nRandCount = nMinRandCount; nRandCount < nMaxRandCount; nRandCount++)
			{
				//ÿ����ϳ���100��
				for (int i = 0; i < 100; i++)
				{
					nCardLevel = 0;
					cbCardCount = 0;

					//ÿһ�����400��
					for (int j = 0; j < 1000; j++)
					{
						nRandIndex = rand() % m_ArrayDebugCard.GetCount();
						tagDebugCardInfo &DebugCardInfo = m_ArrayDebugCard[nRandIndex];
						if (cbCardCount + DebugCardInfo.cbDebugCount < NORMAL_COUNT && nCardLevel + DebugCardInfo.nCardLevel <= DebugInfo.nCardLevel)
						{
							//�Ӻ�����ȡ����
							RandCardList(DebugCardInfo.cbCardData, DebugCardInfo.cbCardCount, cbCardDataTemp, DebugCardInfo.cbDebugCount);
							
							//�ж��Ƿ��֮ǰ�����ظ�
							bAddCard = true;
							for (int k = 0; k < DebugCardInfo.cbDebugCount && bAddCard; k++)
							{
								for (int l = 0; l < cbCardCount; l++)
								{
									if ((cbCardDataTemp[k] & MASK_VALUE) == (cbCardData[l] & MASK_VALUE) || cbCardDataTemp[k] >= 0x4e && cbCardData[l] >= 0x4e)
									{
										bAddCard = false;
										break;
									}
								}
							}

							//��������
							if (bAddCard)
							{			
								CopyMemory(cbCardData + cbCardCount, cbCardDataTemp, sizeof(BYTE) * DebugCardInfo.cbDebugCount);
								cbCardCount += DebugCardInfo.cbDebugCount;
								nCardLevel += DebugCardInfo.nCardLevel;

								//�����������������
								if (nCardLevel == DebugInfo.nCardLevel || cbCardCount == NORMAL_COUNT)
								{
									break;
								}
							}
						}						
					}
					
					// ���������Ʒ�
					if (nCardLevel > nMaxCardLevel)
					{
						CopyMemory(cbMaxCardData, cbCardData, sizeof(BYTE) * cbCardCount);
						cbMaxCardCount = cbCardCount;
						nMaxCardLevel = nCardLevel;
						if (nMaxCardLevel == DebugInfo.nCardLevel)
						{
							break;
						}
					}
				}
			}

			if (nMaxCardLevel > 0 && cbMaxCardCount > 0)
			{
				BYTE cbFullCardData[CountArray(g_cbCardData)];
				BYTE cbFullCardCount = CountArray(g_cbCardData);
				CopyMemory(cbFullCardData, g_cbCardData, sizeof(cbFullCardData));

				//ɾ���˿�
				if (!RemoveCard(cbMaxCardData, cbMaxCardCount, cbFullCardData, cbFullCardCount))
				{
					ASSERT(NULL);
					return false;
				}
				cbFullCardCount -= cbMaxCardCount;

				//���ʣ�µ���(������Ϊ������)
				int nRandCount = MAX_COUNT - cbMaxCardCount;
				if (!RandDebugCardList(cbFullCardData, cbFullCardCount, cbMaxCardData, cbMaxCardCount, nRandCount))
				{
					ASSERT(NULL);
					return false;
				}
				cbFullCardCount -= nRandCount;
				cbMaxCardCount += nRandCount;


				//�ж�AI
				int nRandPlayerIndex = 0;// rand() % GAME_PLAYER;
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					//��������
					IServerUserItem *pServerUserItem = pITableFrame->GetTableUserItem(nRandPlayerIndex);
					if (pServerUserItem != NULL)
					{
						if (pServerUserItem->IsAndroidUser())
						{
							CopyMemory(cbBankerCard, &cbMaxCardData[NORMAL_COUNT], sizeof(BYTE) * 3);
							for (int j = 0; j < GAME_PLAYER; j++)
							{
								if (j == nRandPlayerIndex)
								{
									//���������
									RandCardList(cbMaxCardData, NORMAL_COUNT, cbHandCardData[j], NORMAL_COUNT);
									cbHandCardCount[j] = NORMAL_COUNT;
								}
								else
								{
									//�������������ҵ���
									RandCardList(cbFullCardData, cbFullCardCount, cbHandCardData[j], NORMAL_COUNT);

									//ɾ���˿�
									if (!RemoveCard(cbHandCardData[j], NORMAL_COUNT, cbFullCardData, cbFullCardCount))
									{
										ASSERT(NULL);
										return false;
									}
									cbHandCardCount[j] = NORMAL_COUNT;
									cbFullCardCount -= NORMAL_COUNT;			
								}
							}

							DebugInfo.bWaitScore = true;

							return true;
						}
					}

					nRandPlayerIndex = (nRandPlayerIndex + 1) % GAME_PLAYER;
				}

				return false;
			}
		}
	}
	return false;
}

//���·���
void __cdecl CServerDebugItemSink::UpdateDebugInfo(ITableFrame * pITableFrame)
{
	CTime tCurTime = CTime::GetCurrentTime();
	for (int i = 0; i < m_ArrayDebug.GetCount(); i++)
	{
		tagDebugInfo &DebugInfo = m_ArrayDebug[i];
		if (!DebugInfo.bWaitScore && DebugInfo.nRunCount > 0)
		{
			if (tCurTime > DebugInfo.tEndTime)
			{
				// ������Ϣ
				SendHistoryDebugInfo(pITableFrame, NULL, DebugInfo);

				//������Ϣ
				DebugInfo.nRunCount = 0;
				DebugInfo.lSystemScore = 0;
				DebugInfo.bWaitScore = false;
				DebugInfo.bWaitDelete = false;
				DebugInfo.tStartTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nStartTime / 86400 + 1, (DebugInfo.nStartTime % 86400) / 3600, (DebugInfo.nStartTime % 3600) / 60, DebugInfo.nStartTime % 60);
				DebugInfo.tEndTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nEndTime / 86400 + 1, (DebugInfo.nEndTime % 86400) / 3600, (DebugInfo.nEndTime % 3600) / 60, DebugInfo.nEndTime % 60);


				// ������Ϣ
				CMD_S_UpdateDebug CMDSUpdateDebug;
				CMDSUpdateDebug.dwDebugID = DebugInfo.dwDebugID;
				CMDSUpdateDebug.lSystemScore = DebugInfo.lSystemScore;

				// ������Ϣ
				pITableFrame->SendRoomData(NULL, SUB_S_UPDATE_CUR_DEBUG, &CMDSUpdateDebug, sizeof(CMDSUpdateDebug));
			}

			//ɾ��
			if (DebugInfo.bWaitDelete)
			{
				m_ArrayDebug.RemoveAt(i);
			}
		}
	}
}

//���·���
bool __cdecl CServerDebugItemSink::UpdateDebugInfo(ITableFrame * pITableFrame, tagScoreInfo ScoreInfoArray[GAME_PLAYER])
{
	int nIndex = GetDebugIndex(pITableFrame, true);
	if (nIndex >= 0)
	{
		tagDebugInfo &DebugInfo = m_ArrayDebug[nIndex];
		if (DebugInfo.bWaitScore)
		{
			SCORE lSystemScore = 0;
			//ͳ�ƻ���
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				IServerUserItem *pServerUserItem = pITableFrame->GetTableUserItem(i);
				if (pServerUserItem != NULL)
				{
					if (pServerUserItem->IsAndroidUser())
					{
						lSystemScore += ScoreInfoArray[i].lScore;
					}
				}
			}
			
			DebugInfo.bWaitScore = false;
			DebugInfo.nRunCount++;
			DebugInfo.lSystemScore += lSystemScore;
			
			CTime tCurTime = CTime::GetCurrentTime();
			if (tCurTime > DebugInfo.tEndTime)
			{
				// ������Ϣ
				SendHistoryDebugInfo(pITableFrame, NULL, DebugInfo);

				//������Ϣ
				DebugInfo.nRunCount = 0;
				DebugInfo.lSystemScore = 0;
				DebugInfo.bWaitScore = false;
				DebugInfo.bWaitDelete = false;
				DebugInfo.tStartTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nStartTime / 86400 + 1, (DebugInfo.nStartTime % 86400) / 3600, (DebugInfo.nStartTime % 3600) / 60, DebugInfo.nStartTime % 60);
				DebugInfo.tEndTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nEndTime / 86400 + 1, (DebugInfo.nEndTime % 86400) / 3600, (DebugInfo.nEndTime % 3600) / 60, DebugInfo.nEndTime % 60);
			}
			
			// ������Ϣ
			CMD_S_UpdateDebug CMDSUpdateDebug;
			CMDSUpdateDebug.dwDebugID = DebugInfo.dwDebugID;
			CMDSUpdateDebug.lSystemScore = DebugInfo.lSystemScore;

			//ɾ��
			if (DebugInfo.bWaitDelete)
			{
				m_ArrayDebug.RemoveAt(nIndex);
			}

			// ������Ϣ
			return pITableFrame->SendRoomData(NULL, SUB_S_UPDATE_CUR_DEBUG, &CMDSUpdateDebug, sizeof(CMDSUpdateDebug));
		}
	}
	return false;
}

//���ӵ���
bool CServerDebugItemSink::AddDebug(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, CMD_C_AddDebug *pAddDebug)
{
	CTime tCurTime = CTime::GetCurrentTime();
	tagDebugInfo DebugInfo;
	ZeroMemory(&DebugInfo, sizeof(DebugInfo));
	DebugInfo.dwDebugID = ++m_dwDebugID;
	DebugInfo.nRunCount = 0;
	DebugInfo.lSystemScore = 0;
	DebugInfo.bWaitScore = false;
	DebugInfo.bWaitDelete = false;
	DebugInfo.wTableID = pAddDebug->wTableID;
	DebugInfo.nCardLevel = pAddDebug->nCardLevel;
	DebugInfo.cbCardChance = pAddDebug->cbCardChance;
	DebugInfo.nStartTime = pAddDebug->nStartTime;
	DebugInfo.nEndTime = pAddDebug->nEndTime;
	DebugInfo.tStartTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0);
	DebugInfo.tEndTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0);
	if (DebugInfo.nStartTime >= DebugInfo.nEndTime)
	{
		DebugInfo.nEndTime += 86400;
	}
	DebugInfo.tStartTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nStartTime / 86400, (DebugInfo.nStartTime % 86400) / 3600, (DebugInfo.nStartTime % 3600) / 60, DebugInfo.nStartTime % 60);
	DebugInfo.tEndTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nEndTime / 86400, (DebugInfo.nEndTime % 86400) / 3600, (DebugInfo.nEndTime % 3600) / 60, DebugInfo.nEndTime % 60);

	//У�������Ϣ
	if (!CheckDebugInfo(DebugInfo))
	{
		// ����֪ͨ��Ϣ
		SendMesasage(pITableFrame, pIServerUserItem, TEXT("���ӵ�����Ϣʧ��,����ʱ�����ص���"));
		return true;
	}

	int nInsertIndex = GetInsertIndex(DebugInfo);
	if (nInsertIndex == -1)
	{
		m_ArrayDebug.Add(DebugInfo);
	}
	else
	{
		m_ArrayDebug.InsertAt(nInsertIndex, DebugInfo);
	}

	// ������Ϣ
	SendDebugInfo(pITableFrame, NULL, DebugInfo);

	// ����֪ͨ��Ϣ
	SendMesasage(pITableFrame, pIServerUserItem, TEXT("���ӵ�����Ϣ�ɹ���"));

	return true;
}

//�޸ĵ���
bool CServerDebugItemSink::ModifyDebug(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, CMD_C_ModifyDebug *pModifyDebug)
{
	int nCurIndex = GetDebugInfoIndex(pModifyDebug->dwDebugID);
	if (nCurIndex >= 0)
	{
		CTime tCurTime = CTime::GetCurrentTime();
		tagDebugInfo DebugInfo;
		ZeroMemory(&DebugInfo, sizeof(DebugInfo));
		DebugInfo.dwDebugID = pModifyDebug->dwDebugID;
		DebugInfo.nRunCount = 0;
		DebugInfo.lSystemScore = 0;
		DebugInfo.bWaitScore = false;
		DebugInfo.bWaitDelete = false;
		DebugInfo.wTableID = pModifyDebug->wTableID;
		DebugInfo.nCardLevel = pModifyDebug->nCardLevel;
		DebugInfo.cbCardChance = pModifyDebug->cbCardChance;
		DebugInfo.nStartTime = pModifyDebug->nStartTime;
		DebugInfo.nEndTime = pModifyDebug->nEndTime;
		if (DebugInfo.nStartTime >= DebugInfo.nEndTime)
		{
			DebugInfo.nEndTime += 86400;
		}
		DebugInfo.tStartTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nStartTime / 86400, (DebugInfo.nStartTime % 86400) / 3600, (DebugInfo.nStartTime % 3600) / 60, DebugInfo.nStartTime % 60);
		DebugInfo.tEndTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nEndTime / 86400, (DebugInfo.nEndTime % 86400) / 3600, (DebugInfo.nEndTime % 3600) / 60, DebugInfo.nEndTime % 60);

		//У�������Ϣ
		if (!CheckDebugInfo(DebugInfo, nCurIndex))
		{
			// ����֪ͨ��Ϣ
			SendMesasage(pITableFrame, pIServerUserItem, TEXT("�޸ĵ�����Ϣʧ��,����ʱ�����ص���"));
			return true;
		}
		
		// �޸���Ϣ
		tagDebugInfo &ModifyDebugInfo = m_ArrayDebug[nCurIndex];
		if (ModifyDebugInfo.nRunCount > 0)
		{
			ModifyDebugInfo.nEndTime = CTimeSpan(tCurTime - ModifyDebugInfo.tStartTime).GetTotalSeconds();
			ModifyDebugInfo.tEndTime = tCurTime;
			if (ModifyDebugInfo.bWaitScore)
			{
				ModifyDebugInfo.bWaitDelete = true;
			}
			else
			{
				// ������Ϣ
				SendHistoryDebugInfo(pITableFrame, NULL, ModifyDebugInfo);
				m_ArrayDebug.RemoveAt(nCurIndex);
			}
		}
		else
		{
			m_ArrayDebug.RemoveAt(nCurIndex);
		}

		//�������
		int nInsertIndex = GetInsertIndex(DebugInfo);
		if (nInsertIndex == -1)
		{
			m_ArrayDebug.Add(DebugInfo);
		}
		else
		{
			m_ArrayDebug.InsertAt(nInsertIndex, DebugInfo);
		}

		// ������Ϣ
		SendDebugInfo(pITableFrame, NULL, DebugInfo);

		// ����֪ͨ��Ϣ
		SendMesasage(pITableFrame, pIServerUserItem, TEXT("�޸ĵ�����Ϣ�ɹ���"));
	}
	return true;
}

//ɾ������
bool CServerDebugItemSink::DeleteDebug(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, DWORD dwDebugID)
{
	int nCurIndex = GetDebugInfoIndex(dwDebugID);
	if (nCurIndex >= 0)
	{
		m_ArrayDebug.RemoveAt(nCurIndex);

		// ������Ϣ
		CMD_S_DeleteDebug CMDSDeleteDebug;
		CMDSDeleteDebug.dwDebugID = dwDebugID;

		// ������Ϣ
		pITableFrame->SendRoomData(NULL, SUB_S_REQUEST_DELETE_DEBUG, &CMDSDeleteDebug, sizeof(CMDSDeleteDebug));

		// ����֪ͨ��Ϣ
		SendMesasage(pITableFrame, pIServerUserItem, TEXT("ɾ��������Ϣ�ɹ���"));

		return true;
	}

	// ����֪ͨ��Ϣ
	SendMesasage(pITableFrame, pIServerUserItem, TEXT("ɾ��������Ϣʧ�ܣ�������Ϣ�����ڣ���ˢ�����ԣ�"));
	return false;
}

//�������
int CServerDebugItemSink::GetDebugInfoIndex(DWORD dwDebugID)
{
	for (int i = 0; i < m_ArrayDebug.GetCount(); i++)
	{
		tagDebugInfo &DebugInfo = m_ArrayDebug[i];
		if (DebugInfo.dwDebugID == dwDebugID)
		{
			return i;
		}
	}
	return -1;
}

//�Ƿ����
int CServerDebugItemSink::GetDebugIndex(ITableFrame * pITableFrame, bool bUpdateScore)
{
	WORD wTableID = pITableFrame->GetTableID();

	//���·���
	if (bUpdateScore)
	{
		for (int i = 0; i < m_ArrayDebug.GetCount(); i++)
		{
			if (m_ArrayDebug[i].wTableID == wTableID)
			{
				if (m_ArrayDebug[i].bWaitScore)
				{
					return i;
				}
			}
		}

		return -1;
	}

	int nAndroidCount = 0;
	int nAllPlayerCount = 0;
	//�ж�AI
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//��������
		IServerUserItem *pServerUserItem = pITableFrame->GetTableUserItem(i);
		if (pServerUserItem != NULL)
		{
			nAllPlayerCount++;
			if (pServerUserItem->IsAndroidUser())
			{
				nAndroidCount++;
			}
		}
	}

	//��AI������
	if (nAndroidCount != nAllPlayerCount && nAndroidCount > 0)
	{		
		CTime tCurTime = CTime::GetCurrentTime();		
		for (int i = 0; i < m_ArrayDebug.GetCount(); i++)
		{
			tagDebugInfo &DebugInfo = m_ArrayDebug[i];
			if (DebugInfo.wTableID == wTableID)
			{
				if (!DebugInfo.bWaitDelete)
				{
					if (tCurTime >= DebugInfo.tStartTime && tCurTime <= DebugInfo.tEndTime)
					{
						return i;
					}
				}
			}
		}
	}
	return -1;
}


//�������
int CServerDebugItemSink::GetInsertIndex(tagDebugInfo &DebugInfo)
{
	WORD wTableID = INVALID_TABLE;
	for (int i = 0; i < m_ArrayDebug.GetCount(); i++)
	{
		tagDebugInfo &DebugInfo = m_ArrayDebug[i];

		//����ID��ֱͬ���޸�
		if (DebugInfo.dwDebugID == DebugInfo.dwDebugID)
		{
			return i;
		}

		wTableID = DebugInfo.wTableID;

		//���Ӻ���������
		if (wTableID > DebugInfo.wTableID)
		{
			return i;
		}

		//���Ӻ���ͬ
		if (wTableID == DebugInfo.wTableID)
		{
			//��ʼʱ����������
			if (DebugInfo.nStartTime > DebugInfo.nStartTime)
			{
				return i;
			}

			//��ʼʱ����ͬ
			if (DebugInfo.nStartTime == DebugInfo.nStartTime)
			{
				//����ʱ����������
				if (DebugInfo.nEndTime > DebugInfo.nEndTime)
				{
					return i;
				}
			}
		}
	}
	return -1;
}

//У�������Ϣ
bool CServerDebugItemSink::CheckDebugInfo(tagDebugInfo &AddDebugInfo, int nExcludeIndex)
{
	for (int i = 0; i < m_ArrayDebug.GetCount(); i++)
	{
		tagDebugInfo &DebugInfo = m_ArrayDebug[i];

		//ֻ�ж�ͬ��
		if (DebugInfo.wTableID == AddDebugInfo.wTableID && nExcludeIndex != i)
		{
			if (DebugInfo.nEndTime < 86400 && AddDebugInfo.nEndTime < 86400 || DebugInfo.nEndTime >= 86400 && AddDebugInfo.nEndTime >= 86400)
			{
				if (AddDebugInfo.nStartTime >= DebugInfo.nStartTime && AddDebugInfo.nStartTime <= DebugInfo.nEndTime)
				{
					return false;
				}

				if (AddDebugInfo.nEndTime >= DebugInfo.nStartTime && AddDebugInfo.nEndTime <= DebugInfo.nEndTime)
				{
					return false;
				}
			}
			if (AddDebugInfo.nEndTime >= 86400 && DebugInfo.nEndTime < 86400)
			{
				if (DebugInfo.nStartTime <= AddDebugInfo.nEndTime % 86400 || DebugInfo.nStartTime >= AddDebugInfo.nStartTime)
				{
					return false;
				}
				
				if (DebugInfo.nEndTime <= AddDebugInfo.nEndTime % 86400 || DebugInfo.nEndTime >= AddDebugInfo.nStartTime)
				{
					return false;
				}
			}
			if (AddDebugInfo.nEndTime < 86400 && DebugInfo.nEndTime >= 86400)
			{
				if (AddDebugInfo.nStartTime >= DebugInfo.nStartTime || AddDebugInfo.nStartTime <= DebugInfo.nEndTime % 86400)
				{
					return false;
				}

				if (AddDebugInfo.nEndTime >= DebugInfo.nStartTime || AddDebugInfo.nEndTime <= DebugInfo.nEndTime % 86400)
				{
					return false;
				}
			}
		}
	}
	return true;
}

//����֪ͨ
bool CServerDebugItemSink::SendMesasage(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, TCHAR * pText, ...)
{
	// ������Ϣ
	CMD_S_DebugText DebugText;

	// ת����Ϣ
	va_list VaList;
	va_start(VaList, pText);
	_vstprintf_s(DebugText.szMessageText, pText, VaList);
	va_end(VaList);

	// ������Ϣ
	pITableFrame->SendRoomData(pIServerUserItem, SUB_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));

	return true;
}

// ������Ϣ
bool CServerDebugItemSink::SendDebugInfo(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, tagDebugInfo &DebugInfo)
{
	// ������Ϣ
	CMD_S_CurDebugInfo CMDSCurDebugInfo;
	CMDSCurDebugInfo.dwDebugID = DebugInfo.dwDebugID;
	CMDSCurDebugInfo.wTableID = DebugInfo.wTableID;
	CMDSCurDebugInfo.nCardLevel = DebugInfo.nCardLevel;
	CMDSCurDebugInfo.cbCardChance = DebugInfo.cbCardChance;
	CMDSCurDebugInfo.lSystemScore = DebugInfo.lSystemScore;
	CMDSCurDebugInfo.nStartTime = DebugInfo.nStartTime;
	CMDSCurDebugInfo.nEndTime = DebugInfo.nEndTime;

	// ������Ϣ
	return pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_CUR_DEBUG, &CMDSCurDebugInfo, sizeof(CMDSCurDebugInfo));
}

// ������Ϣ
bool CServerDebugItemSink::SendHistoryDebugInfo(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, tagDebugInfo &DebugInfo)
{
	//�����¼
	CTime tCurTime = CTime::GetCurrentTime();
	tagHistoryDebugInfo HistoryDebugInfo;
	HistoryDebugInfo.dwDebugID = DebugInfo.dwDebugID;
	HistoryDebugInfo.wTableID = DebugInfo.wTableID;
	HistoryDebugInfo.nCardLevel = DebugInfo.nCardLevel;
	HistoryDebugInfo.cbCardChance = DebugInfo.cbCardChance;
	HistoryDebugInfo.nDebugTime[0] = tCurTime.GetYear();
	HistoryDebugInfo.nDebugTime[1] = tCurTime.GetMonth();
	HistoryDebugInfo.nDebugTime[2] = tCurTime.GetDay();
	HistoryDebugInfo.nStartTime = DebugInfo.nStartTime;
	HistoryDebugInfo.nEndTime = DebugInfo.nEndTime;
	HistoryDebugInfo.lSystemScore = DebugInfo.lSystemScore;
	m_ArrayHistoryDebug.Add(HistoryDebugInfo);

	// ������Ϣ
	CMD_S_HistoryDebugInfo CMDSHistoryDebugInfo;
	CMDSHistoryDebugInfo.dwDebugID = HistoryDebugInfo.dwDebugID;
	CMDSHistoryDebugInfo.wTableID = HistoryDebugInfo.wTableID;
	CMDSHistoryDebugInfo.nCardLevel = HistoryDebugInfo.nCardLevel;
	CMDSHistoryDebugInfo.cbCardChance = HistoryDebugInfo.cbCardChance;
	CMDSHistoryDebugInfo.lSystemScore = HistoryDebugInfo.lSystemScore;
	CopyMemory(CMDSHistoryDebugInfo.nDebugTime, HistoryDebugInfo.nDebugTime, sizeof(CMDSHistoryDebugInfo.nDebugTime));
	CMDSHistoryDebugInfo.nStartTime = HistoryDebugInfo.nStartTime;
	CMDSHistoryDebugInfo.nEndTime = HistoryDebugInfo.nEndTime;

	// ������Ϣ
	return pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_HISTORY_DEBUG, &CMDSHistoryDebugInfo, sizeof(CMDSHistoryDebugInfo));
}

//���Ӻ���
int CServerDebugItemSink::AddGoodCard(tagDebugCardInfo &DebugCardInfo)
{
	for (int i = 0; i < m_ArrayDebugCard.GetCount(); i++)
	{
		if (DebugCardInfo.nCardLevel >= m_ArrayDebugCard[i].nCardLevel)
		{
			m_ArrayDebugCard.InsertAt(i, DebugCardInfo);
			return i;
		}
	}
	m_ArrayDebugCard.Add(DebugCardInfo);
	return m_ArrayDebugCard.GetCount() - 1;
}

//��С�����
int CServerDebugItemSink::GetMinRandCount(int nCardLevel)
{
	//ֻ�ܴ���ǰ���Ƶ���ϣ���߷ֵ����û�ظ�����
	for (int i = 0; i < m_ArrayDebugCard.GetCount(); i++)
	{
		nCardLevel -= m_ArrayDebugCard[i].nCardLevel;
		if (nCardLevel <= 0)
		{
			return i + 1;
		}
	}
	return 1;
}

//ɾ���˿�
bool CServerDebugItemSink::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	ASSERT(cbRemoveCount <= cbCardCount);

	//�������
	BYTE cbDeleteCount = 0, cbTempCardData[FULL_COUNT];
	if (cbCardCount > CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i = 0; i < cbRemoveCount; i++)
	{
		for (BYTE j = 0; j < cbCardCount; j++)
		{
			if (cbRemoveCard[i] == cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j] = 0;
				break;
			}
		}
	}
	if (cbDeleteCount != cbRemoveCount) return false;

	//�����˿�
	BYTE cbCardPos = 0;
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (cbTempCardData[i] != 0) cbCardData[cbCardPos++] = cbTempCardData[i];
	}

	return true;
}

//�����˿�
bool CServerDebugItemSink::RandCardList(BYTE cbSrcCardData[], BYTE cbSrcCardCount, BYTE cbNewCardData[], BYTE cbNewCardCount)
{
	ASSERT(cbSrcCardCount >= cbNewCardCount);
	if (cbSrcCardCount < cbNewCardCount)
	{
		return false;
	}

	//����׼��
	BYTE cbCardData[FULL_COUNT];
	CopyMemory(cbCardData, cbSrcCardData, sizeof(BYTE) * cbSrcCardCount);

	//�����˿�
	BYTE cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (cbSrcCardCount - cbRandCount);
		cbNewCardData[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cbSrcCardCount - cbRandCount];
	} while (cbRandCount < cbNewCardCount);

	return cbRandCount == cbNewCardCount;
}

//���ʣ�µ���
bool CServerDebugItemSink::RandDebugCardList(BYTE cbSrcCardData[], BYTE cbSrcCardCount, BYTE cbNewCardData[], BYTE cbNewCardCount, int nRandCount)
{
	int nRandCardCount = 0;	
	BYTE cbRandCardData = 0;
	int nDebugCardCount = 0;
	BYTE cbFullCardData[FULL_COUNT];
	BYTE cbFullCardCount;
	int nRandbFullCardIndex = 0;
	bool bAddCard = true;
	bool bRandCard = true;
	while (nRandCount > nRandCardCount)
	{
		nRandCardCount = 0;
		cbFullCardCount = cbSrcCardCount;
		CopyMemory(cbFullCardData, cbSrcCardData, sizeof(BYTE) * cbSrcCardCount);

		for (int i = 0; i < nRandCount; i++)
		{
			//��������ǰ��Ϻõ����͵�ͬ����					
			do
			{
				bRandCard = false;	
				nRandbFullCardIndex = rand() % cbFullCardCount;
				cbRandCardData = cbFullCardData[nRandbFullCardIndex];
				for (int j = 0; j < cbNewCardCount; j++)
				{
					if ((cbRandCardData & MASK_VALUE) == (cbNewCardData[j] & MASK_VALUE) || cbRandCardData >= 0x4e && cbNewCardData[j] >= 0x4e)
					{
						bRandCard = true;
						break;
					}
				}
			} while (bRandCard);

			//����ϲ����Ǻ���������
			bAddCard = true;
			for (int j = 0; j < m_ArrayDebugCard.GetCount(); j++)
			{
				tagDebugCardInfo &DebugCardInfo = m_ArrayDebugCard[j];
				nDebugCardCount = 0;
				for (int k = 0; k < DebugCardInfo.cbCardCount; k++)
				{
					if (cbRandCardData == DebugCardInfo.cbCardData[k])
					{
						nDebugCardCount++;
						break;
					}

					for (int l = cbNewCardCount; l < cbNewCardCount + nRandCardCount; l++)
					{
						if (cbNewCardData[l] == DebugCardInfo.cbCardData[k])
						{
							nDebugCardCount++;
							break;
						}
					}
				}

				//������ϳɺ��ƿ��������
				if (nDebugCardCount >= DebugCardInfo.cbDebugCount)
				{
					bAddCard = false;
					break;
				}
			}
			//�������
			if (bAddCard)
			{
				cbFullCardData[nRandbFullCardIndex] = cbFullCardData[--cbFullCardCount];
				cbNewCardData[cbNewCardCount + nRandCardCount++] = cbRandCardData;
			}
			else
			{
				break;
			}
		}
	}
	
	CopyMemory(cbSrcCardData, cbFullCardData, sizeof(BYTE) * cbFullCardCount);
	ASSERT(nRandCardCount == nRandCount);
	return nRandCardCount == nRandCount;
}