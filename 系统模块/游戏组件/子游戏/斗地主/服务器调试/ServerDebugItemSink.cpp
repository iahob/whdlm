#include "StdAfx.h"
#include "ServerDebugitemsink.h"

// 消息定义
#define	IPC_PACKAGE					4096

//扑克数据
const BYTE	g_cbCardData[FULL_COUNT] =
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//方块 A - K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,	//梅花 A - K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,	//红桃 A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,	//黑桃 A - K
	0x4E, 0x4F,
};

//
CServerDebugItemSink::CServerDebugItemSink(void)
{
	m_dwDebugID = 0;
	if (m_ArrayDebugCard.GetCount() == 0)
	{
		//大王小王（16）
		tagDebugCardInfo DebugCardInfo;
		DebugCardInfo.cbCardData[0] = 0x4e;
		DebugCardInfo.cbCardData[1] = 0x4f;
		DebugCardInfo.cbCardCount = 2;
		DebugCardInfo.cbDebugCount = 2;
		DebugCardInfo.nCardLevel = 16;
		AddGoodCard(DebugCardInfo);

		//四条1-K
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

		//三条1-2
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

		//二条1-2
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

		//大王（2）
		DebugCardInfo.cbCardData[0] = 0x4f;
		DebugCardInfo.cbCardCount = 1;
		DebugCardInfo.cbDebugCount = 1;
		DebugCardInfo.nCardLevel = 2;
		AddGoodCard(DebugCardInfo);

		//小王（1）
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
	//调试用户
	if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
	{
		return false;
	}
	switch (wSubCmdID)
	{
	case SUB_C_REQUEST_CUR_DEBUG:		//查询当前调试
	{
		//效验数据
		ASSERT(wDataSize == 0);
		if (wDataSize != 0) return false;

		// 发送调试信息
		WORD wBufferSize = 0;
		byte cbBuffer[IPC_PACKAGE];
		int nSendIndex = 0;
		int nSendCount = 0;
		CMD_S_CurDebugInfo CMDSCurDebugInfo;

		while (nSendIndex < m_ArrayDebug.GetCount())
		{
			//发送数据
			tagDebugInfo &DebugInfo = m_ArrayDebug[nSendIndex];

			// 超出界限
			if (wBufferSize + sizeof(CMD_S_CurDebugInfo) > sizeof(cbBuffer))
			{
				// 发送消息
				pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_CUR_DEBUG, cbBuffer, wBufferSize);

				// 归零消息
				nSendCount = 0;
				wBufferSize = 0;
			}

			if (!DebugInfo.bWaitDelete)
			{
				// 构造消息
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

			// 下一个
			nSendIndex++;			
		}

		// 有效消息
		if (wBufferSize > 0)
		{
			// 发送消息
			pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_CUR_DEBUG, cbBuffer, wBufferSize);
		}

		// 发送消息
		pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_CUR_DEBUG);

		return true;
	}
	case SUB_C_REQUEST_HISTORY_DEBUG:	//查询历史调试
	{
		//效验数据
		ASSERT(wDataSize == 0);
		if (wDataSize != 0) return false;

		// 发送调试信息
		WORD wBufferSize = 0;
		byte cbBuffer[IPC_PACKAGE];
		int nSendIndex = 0;
		int nSendCount = 0;
		CMD_S_HistoryDebugInfo CMDSHistoryDebugInfo;

		while (nSendIndex < m_ArrayHistoryDebug.GetCount())
		{
			//发送数据
			tagHistoryDebugInfo &HistoryDebugInfo = m_ArrayHistoryDebug[nSendIndex];

			// 超出界限
			if (wBufferSize + sizeof(CMD_S_HistoryDebugInfo) > sizeof(cbBuffer))
			{
				// 发送消息
				pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_HISTORY_DEBUG, cbBuffer, wBufferSize);

				// 归零消息
				nSendCount = 0;
				wBufferSize = 0;
			}

			// 构造消息
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

			// 下一个
			nSendIndex++;
			nSendCount++;
		}

		// 有效消息
		if (wBufferSize > 0)
		{
			// 发送消息
			pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_HISTORY_DEBUG, cbBuffer, wBufferSize);
		}

		// 发送消息
		pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_HISTORY_DEBUG);

		return true;
	}
	case SUB_C_REQUEST_ADD_DEBUG:		//增加调试
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_C_AddDebug));
		if (wDataSize != sizeof(CMD_C_AddDebug)) return false;

		CMD_C_AddDebug *pAddDebug = (CMD_C_AddDebug *)pData;
		
		return AddDebug(pITableFrame, pIServerUserItem, pAddDebug);
	}
	case SUB_C_REQUEST_MODIFY_DEBUG:	//修改调试
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_C_ModifyDebug));
		if (wDataSize != sizeof(CMD_C_ModifyDebug)) return false;

		CMD_C_ModifyDebug *pModifyDebug = (CMD_C_ModifyDebug *)pData;

		return ModifyDebug(pITableFrame, pIServerUserItem, pModifyDebug);
	}
	case SUB_C_REQUEST_DELETE_DEBUG:	//删除调试
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_C_DeleteDebug));
		if (wDataSize != sizeof(CMD_C_DeleteDebug)) return false;

		CMD_C_DeleteDebug *pDeleteDebug = (CMD_C_DeleteDebug *)pData;

		return DeleteDebug(pITableFrame, pIServerUserItem, pDeleteDebug->dwDebugID);
	}
	}
	return false;
}

//服务器调试
bool __cdecl CServerDebugItemSink::ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame )
{
	for(WORD i = 0; i < GAME_PLAYER ;i++)
	{
		IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(i);
		ASSERT(pIServerUserItem);
		if(pIServerUserItem)
		{
			//调试用户
			if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
			{
				ServerDebug(pDebugCard, pITableFrame, pIServerUserItem);
			}
		}
	}

	//WB改
	WORD wEnumIndex=0;
	do
	{
		IServerUserItem * pIServerUserItem = pITableFrame->EnumLookonUserItem(wEnumIndex++);
		if(pIServerUserItem == NULL) break;
		//调试用户
		if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		{
			ServerDebug(pDebugCard, pITableFrame, pIServerUserItem);
		}
	}while(TRUE);

	return true;
}
//服务器调试
bool __cdecl CServerDebugItemSink::ServerDebug(CMD_S_DebugCard *pDebugCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem)
{
	pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_DEBUG_CARD,pDebugCard,sizeof(CMD_S_DebugCard));
	return true;
}

//好牌调试
bool __cdecl CServerDebugItemSink::GetDebugInfo(ITableFrame * pITableFrame, BYTE cbBankerCard[3], BYTE cbHandCardCount[GAME_PLAYER], BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT])
{
	//是否调试
	int nIndex = GetDebugIndex(pITableFrame, false);
	if (nIndex >= 0)
	{
		tagDebugInfo &DebugInfo = m_ArrayDebug[nIndex];
		if (DebugInfo.cbCardChance > rand() % 100)
		{
			//最小组合数
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

			//组合数
			for (int nRandCount = nMinRandCount; nRandCount < nMaxRandCount; nRandCount++)
			{
				//每种组合尝试100遍
				for (int i = 0; i < 100; i++)
				{
					nCardLevel = 0;
					cbCardCount = 0;

					//每一次随机400遍
					for (int j = 0; j < 1000; j++)
					{
						nRandIndex = rand() % m_ArrayDebugCard.GetCount();
						tagDebugCardInfo &DebugCardInfo = m_ArrayDebugCard[nRandIndex];
						if (cbCardCount + DebugCardInfo.cbDebugCount < NORMAL_COUNT && nCardLevel + DebugCardInfo.nCardLevel <= DebugInfo.nCardLevel)
						{
							//从好牌里取出牌
							RandCardList(DebugCardInfo.cbCardData, DebugCardInfo.cbCardCount, cbCardDataTemp, DebugCardInfo.cbDebugCount);
							
							//判断是否和之前的牌重复
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

							//可以增加
							if (bAddCard)
							{			
								CopyMemory(cbCardData + cbCardCount, cbCardDataTemp, sizeof(BYTE) * DebugCardInfo.cbDebugCount);
								cbCardCount += DebugCardInfo.cbDebugCount;
								nCardLevel += DebugCardInfo.nCardLevel;

								//分数分完或牌数分完
								if (nCardLevel == DebugInfo.nCardLevel || cbCardCount == NORMAL_COUNT)
								{
									break;
								}
							}
						}						
					}
					
					// 保留最大好牌分
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

				//删除扑克
				if (!RemoveCard(cbMaxCardData, cbMaxCardCount, cbFullCardData, cbFullCardCount))
				{
					ASSERT(NULL);
					return false;
				}
				cbFullCardCount -= cbMaxCardCount;

				//随机剩下的牌(后三张为地主牌)
				int nRandCount = MAX_COUNT - cbMaxCardCount;
				if (!RandDebugCardList(cbFullCardData, cbFullCardCount, cbMaxCardData, cbMaxCardCount, nRandCount))
				{
					ASSERT(NULL);
					return false;
				}
				cbFullCardCount -= nRandCount;
				cbMaxCardCount += nRandCount;


				//判断AI
				int nRandPlayerIndex = 0;// rand() % GAME_PLAYER;
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					//发送数据
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
									//随机打乱牌
									RandCardList(cbMaxCardData, NORMAL_COUNT, cbHandCardData[j], NORMAL_COUNT);
									cbHandCardCount[j] = NORMAL_COUNT;
								}
								else
								{
									//随机其它两个玩家的牌
									RandCardList(cbFullCardData, cbFullCardCount, cbHandCardData[j], NORMAL_COUNT);

									//删除扑克
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

//更新分数
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
				// 发送信息
				SendHistoryDebugInfo(pITableFrame, NULL, DebugInfo);

				//更新信息
				DebugInfo.nRunCount = 0;
				DebugInfo.lSystemScore = 0;
				DebugInfo.bWaitScore = false;
				DebugInfo.bWaitDelete = false;
				DebugInfo.tStartTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nStartTime / 86400 + 1, (DebugInfo.nStartTime % 86400) / 3600, (DebugInfo.nStartTime % 3600) / 60, DebugInfo.nStartTime % 60);
				DebugInfo.tEndTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nEndTime / 86400 + 1, (DebugInfo.nEndTime % 86400) / 3600, (DebugInfo.nEndTime % 3600) / 60, DebugInfo.nEndTime % 60);


				// 构造消息
				CMD_S_UpdateDebug CMDSUpdateDebug;
				CMDSUpdateDebug.dwDebugID = DebugInfo.dwDebugID;
				CMDSUpdateDebug.lSystemScore = DebugInfo.lSystemScore;

				// 发送消息
				pITableFrame->SendRoomData(NULL, SUB_S_UPDATE_CUR_DEBUG, &CMDSUpdateDebug, sizeof(CMDSUpdateDebug));
			}

			//删除
			if (DebugInfo.bWaitDelete)
			{
				m_ArrayDebug.RemoveAt(i);
			}
		}
	}
}

//更新分数
bool __cdecl CServerDebugItemSink::UpdateDebugInfo(ITableFrame * pITableFrame, tagScoreInfo ScoreInfoArray[GAME_PLAYER])
{
	int nIndex = GetDebugIndex(pITableFrame, true);
	if (nIndex >= 0)
	{
		tagDebugInfo &DebugInfo = m_ArrayDebug[nIndex];
		if (DebugInfo.bWaitScore)
		{
			SCORE lSystemScore = 0;
			//统计积分
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
				// 发送信息
				SendHistoryDebugInfo(pITableFrame, NULL, DebugInfo);

				//更新信息
				DebugInfo.nRunCount = 0;
				DebugInfo.lSystemScore = 0;
				DebugInfo.bWaitScore = false;
				DebugInfo.bWaitDelete = false;
				DebugInfo.tStartTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nStartTime / 86400 + 1, (DebugInfo.nStartTime % 86400) / 3600, (DebugInfo.nStartTime % 3600) / 60, DebugInfo.nStartTime % 60);
				DebugInfo.tEndTime = CTime(tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 0, 0, 0) + CTimeSpan(DebugInfo.nEndTime / 86400 + 1, (DebugInfo.nEndTime % 86400) / 3600, (DebugInfo.nEndTime % 3600) / 60, DebugInfo.nEndTime % 60);
			}
			
			// 构造消息
			CMD_S_UpdateDebug CMDSUpdateDebug;
			CMDSUpdateDebug.dwDebugID = DebugInfo.dwDebugID;
			CMDSUpdateDebug.lSystemScore = DebugInfo.lSystemScore;

			//删除
			if (DebugInfo.bWaitDelete)
			{
				m_ArrayDebug.RemoveAt(nIndex);
			}

			// 发送消息
			return pITableFrame->SendRoomData(NULL, SUB_S_UPDATE_CUR_DEBUG, &CMDSUpdateDebug, sizeof(CMDSUpdateDebug));
		}
	}
	return false;
}

//增加调试
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

	//校验调试信息
	if (!CheckDebugInfo(DebugInfo))
	{
		// 发送通知消息
		SendMesasage(pITableFrame, pIServerUserItem, TEXT("增加调试信息失败,调试时间有重叠！"));
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

	// 发送信息
	SendDebugInfo(pITableFrame, NULL, DebugInfo);

	// 发送通知消息
	SendMesasage(pITableFrame, pIServerUserItem, TEXT("增加调试信息成功！"));

	return true;
}

//修改调试
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

		//校验调试信息
		if (!CheckDebugInfo(DebugInfo, nCurIndex))
		{
			// 发送通知消息
			SendMesasage(pITableFrame, pIServerUserItem, TEXT("修改调试信息失败,调试时间有重叠！"));
			return true;
		}
		
		// 修改信息
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
				// 发送信息
				SendHistoryDebugInfo(pITableFrame, NULL, ModifyDebugInfo);
				m_ArrayDebug.RemoveAt(nCurIndex);
			}
		}
		else
		{
			m_ArrayDebug.RemoveAt(nCurIndex);
		}

		//插入调试
		int nInsertIndex = GetInsertIndex(DebugInfo);
		if (nInsertIndex == -1)
		{
			m_ArrayDebug.Add(DebugInfo);
		}
		else
		{
			m_ArrayDebug.InsertAt(nInsertIndex, DebugInfo);
		}

		// 发送信息
		SendDebugInfo(pITableFrame, NULL, DebugInfo);

		// 发送通知消息
		SendMesasage(pITableFrame, pIServerUserItem, TEXT("修改调试信息成功！"));
	}
	return true;
}

//删除调试
bool CServerDebugItemSink::DeleteDebug(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, DWORD dwDebugID)
{
	int nCurIndex = GetDebugInfoIndex(dwDebugID);
	if (nCurIndex >= 0)
	{
		m_ArrayDebug.RemoveAt(nCurIndex);

		// 构造消息
		CMD_S_DeleteDebug CMDSDeleteDebug;
		CMDSDeleteDebug.dwDebugID = dwDebugID;

		// 发送消息
		pITableFrame->SendRoomData(NULL, SUB_S_REQUEST_DELETE_DEBUG, &CMDSDeleteDebug, sizeof(CMDSDeleteDebug));

		// 发送通知消息
		SendMesasage(pITableFrame, pIServerUserItem, TEXT("删除调试信息成功！"));

		return true;
	}

	// 发送通知消息
	SendMesasage(pITableFrame, pIServerUserItem, TEXT("删除调试信息失败，调试信息不存在，请刷新重试！"));
	return false;
}

//获得索引
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

//是否调试
int CServerDebugItemSink::GetDebugIndex(ITableFrame * pITableFrame, bool bUpdateScore)
{
	WORD wTableID = pITableFrame->GetTableID();

	//更新分数
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
	//判断AI
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//发送数据
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

	//有AI和真人
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


//获得索引
int CServerDebugItemSink::GetInsertIndex(tagDebugInfo &DebugInfo)
{
	WORD wTableID = INVALID_TABLE;
	for (int i = 0; i < m_ArrayDebug.GetCount(); i++)
	{
		tagDebugInfo &DebugInfo = m_ArrayDebug[i];

		//调试ID相同直接修改
		if (DebugInfo.dwDebugID == DebugInfo.dwDebugID)
		{
			return i;
		}

		wTableID = DebugInfo.wTableID;

		//桌子号优先排序
		if (wTableID > DebugInfo.wTableID)
		{
			return i;
		}

		//桌子号相同
		if (wTableID == DebugInfo.wTableID)
		{
			//开始时间优先排序
			if (DebugInfo.nStartTime > DebugInfo.nStartTime)
			{
				return i;
			}

			//开始时间相同
			if (DebugInfo.nStartTime == DebugInfo.nStartTime)
			{
				//结束时间优先排序
				if (DebugInfo.nEndTime > DebugInfo.nEndTime)
				{
					return i;
				}
			}
		}
	}
	return -1;
}

//校验调试信息
bool CServerDebugItemSink::CheckDebugInfo(tagDebugInfo &AddDebugInfo, int nExcludeIndex)
{
	for (int i = 0; i < m_ArrayDebug.GetCount(); i++)
	{
		tagDebugInfo &DebugInfo = m_ArrayDebug[i];

		//只判断同桌
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

//发送通知
bool CServerDebugItemSink::SendMesasage(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, TCHAR * pText, ...)
{
	// 定义信息
	CMD_S_DebugText DebugText;

	// 转换信息
	va_list VaList;
	va_start(VaList, pText);
	_vstprintf_s(DebugText.szMessageText, pText, VaList);
	va_end(VaList);

	// 发送消息
	pITableFrame->SendRoomData(pIServerUserItem, SUB_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));

	return true;
}

// 发送信息
bool CServerDebugItemSink::SendDebugInfo(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, tagDebugInfo &DebugInfo)
{
	// 构造消息
	CMD_S_CurDebugInfo CMDSCurDebugInfo;
	CMDSCurDebugInfo.dwDebugID = DebugInfo.dwDebugID;
	CMDSCurDebugInfo.wTableID = DebugInfo.wTableID;
	CMDSCurDebugInfo.nCardLevel = DebugInfo.nCardLevel;
	CMDSCurDebugInfo.cbCardChance = DebugInfo.cbCardChance;
	CMDSCurDebugInfo.lSystemScore = DebugInfo.lSystemScore;
	CMDSCurDebugInfo.nStartTime = DebugInfo.nStartTime;
	CMDSCurDebugInfo.nEndTime = DebugInfo.nEndTime;

	// 发送消息
	return pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_CUR_DEBUG, &CMDSCurDebugInfo, sizeof(CMDSCurDebugInfo));
}

// 发送信息
bool CServerDebugItemSink::SendHistoryDebugInfo(ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem, tagDebugInfo &DebugInfo)
{
	//保存记录
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

	// 构造消息
	CMD_S_HistoryDebugInfo CMDSHistoryDebugInfo;
	CMDSHistoryDebugInfo.dwDebugID = HistoryDebugInfo.dwDebugID;
	CMDSHistoryDebugInfo.wTableID = HistoryDebugInfo.wTableID;
	CMDSHistoryDebugInfo.nCardLevel = HistoryDebugInfo.nCardLevel;
	CMDSHistoryDebugInfo.cbCardChance = HistoryDebugInfo.cbCardChance;
	CMDSHistoryDebugInfo.lSystemScore = HistoryDebugInfo.lSystemScore;
	CopyMemory(CMDSHistoryDebugInfo.nDebugTime, HistoryDebugInfo.nDebugTime, sizeof(CMDSHistoryDebugInfo.nDebugTime));
	CMDSHistoryDebugInfo.nStartTime = HistoryDebugInfo.nStartTime;
	CMDSHistoryDebugInfo.nEndTime = HistoryDebugInfo.nEndTime;

	// 发送消息
	return pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_HISTORY_DEBUG, &CMDSHistoryDebugInfo, sizeof(CMDSHistoryDebugInfo));
}

//增加好牌
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

//最小组合数
int CServerDebugItemSink::GetMinRandCount(int nCardLevel)
{
	//只能处理当前的牌的组合，最高分的组合没重复用牌
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

//删除扑克
bool CServerDebugItemSink::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(cbRemoveCount <= cbCardCount);

	//定义变量
	BYTE cbDeleteCount = 0, cbTempCardData[FULL_COUNT];
	if (cbCardCount > CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
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

	//清理扑克
	BYTE cbCardPos = 0;
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (cbTempCardData[i] != 0) cbCardData[cbCardPos++] = cbTempCardData[i];
	}

	return true;
}

//混乱扑克
bool CServerDebugItemSink::RandCardList(BYTE cbSrcCardData[], BYTE cbSrcCardCount, BYTE cbNewCardData[], BYTE cbNewCardCount)
{
	ASSERT(cbSrcCardCount >= cbNewCardCount);
	if (cbSrcCardCount < cbNewCardCount)
	{
		return false;
	}

	//混乱准备
	BYTE cbCardData[FULL_COUNT];
	CopyMemory(cbCardData, cbSrcCardData, sizeof(BYTE) * cbSrcCardCount);

	//混乱扑克
	BYTE cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (cbSrcCardCount - cbRandCount);
		cbNewCardData[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cbSrcCardCount - cbRandCount];
	} while (cbRandCount < cbNewCardCount);

	return cbRandCount == cbNewCardCount;
}

//随机剩下的牌
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
			//不能是以前组合好的牌型的同点数					
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

			//新组合不能是好牌组合里的
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

				//不会组合成好牌库里的牌型
				if (nDebugCardCount >= DebugCardInfo.cbDebugCount)
				{
					bAddCard = false;
					break;
				}
			}
			//可以组合
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