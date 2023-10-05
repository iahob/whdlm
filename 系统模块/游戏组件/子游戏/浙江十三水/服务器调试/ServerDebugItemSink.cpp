#include "StdAfx.h"
#include "ServerDebugItemSink.h"

//////////////////////////////////////////////////////////////////////////

//
CServerDebugItemSink::CServerDebugItemSink(void)
{
}

CServerDebugItemSink::~CServerDebugItemSink( void )
{

}

//���ؿ�������
bool __cdecl CServerDebugItemSink::DebugResult(BYTE cbDebugCardData[GAME_PLAYER][HAND_CARD_COUNT], ROOMUSERDEBUG Keyroomuserdebug, BYTE cbMaCard, WORD wBanker)
{
	ASSERT(Keyroomuserdebug.roomUserInfo.wChairID < GAME_PLAYER);

	//��������
	BYTE bCardData[HAND_CARD_COUNT];
	BYTE bHandCardData[GAME_PLAYER][HAND_CARD_COUNT];
	ZeroMemory(bCardData, sizeof(bCardData));
	ZeroMemory(bHandCardData, sizeof(bHandCardData));

	CopyMemory(bHandCardData, cbDebugCardData, sizeof(BYTE)*HAND_CARD_COUNT*GAME_PLAYER);

	//////////////////////////////////////
	WORD m_wBanker = wBanker;
	BYTE  cbSpecialType[GAME_PLAYER] = { 0 };
	BYTE cbSegmentCard[GAME_PLAYER][3][5] = { 0 };
	BYTE cbAllWin[GAME_PLAYER][GAME_PLAYER] = { 0 };
	SCORE lScoreTimes[GAME_PLAYER][3] = { 0 };
	SCORE lGameScore[GAME_PLAYER] = { 0 };
	bool bHaveSpecial = false;
	WORD wAllKillUser = INVALID_CHAIR;
	WORD wMaCardUser = INVALID_CHAIR;
	//ȷ���������
	if (cbMaCard != 0)
	{
		//ȷ���������
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (bHandCardData[i][0] == 0) continue;

			for (int j = 0; j < HAND_CARD_COUNT; j++)
			{
				//�����Ƶ�Ϊ�������
				if (cbMaCard == bHandCardData[i][j])
				{
					wMaCardUser = i;
					break;
				}
			}

			if (wMaCardUser != INVALID_CHAIR) break;
		}
	}

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (bHandCardData[i][0] != 0)
		{
			BYTE cbSpecialData[3][5] = { 0 };
			BYTE cbPutCard[HAND_CARD_COUNT] = { 0 };
			BYTE cbSpecial = m_GameLogic.GetSpecialType(bHandCardData[i], 13);
			if (cbSpecial > CT_EX_INVALID)
			{
				bHaveSpecial = true;
				m_GameLogic.GetSpecialCardData(bHandCardData[i], 13, cbSpecialData);
				cbSpecialType[i] = cbSpecial;
				CopyMemory(cbSegmentCard[i][0], cbSpecialData[0], 3);
				CopyMemory(cbSegmentCard[i][1], cbSpecialData[1], 5);
				CopyMemory(cbSegmentCard[i][2], cbSpecialData[2], 5);
			}
			else
			{
				m_GameLogic.AutoPutCard(bHandCardData[i], cbPutCard, false, NULL);
				CopyMemory(cbSegmentCard[i][0], &cbPutCard[0], 3);
				CopyMemory(cbSegmentCard[i][1], &cbPutCard[3], 5);
				CopyMemory(cbSegmentCard[i][2], &cbPutCard[8], 5);
			}
		}
	}

	if (wBanker != INVALID_CHAIR)
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (bHandCardData[i][0]==0) continue;

			//������Ӯ
			if (i != wBanker)
			{
				if (cbSpecialType[i] != 0 || cbSpecialType[m_wBanker] != 0)
				{
					continue;
				}

				//��������
				BYTE bWindInfo[3] = { 0 };

				//���ʤ��	
				if (enCRGreater == m_GameLogic.CompareCard(cbSegmentCard[m_wBanker][0], cbSegmentCard[i][0], 3, 3, true)) bWindInfo[0] = TRUE;
				else bWindInfo[0] = FALSE;
				if (enCRGreater == m_GameLogic.CompareCard(cbSegmentCard[m_wBanker][1], cbSegmentCard[i][1], 5, 5, true)) bWindInfo[1] = TRUE;
				else bWindInfo[1] = FALSE;
				if (enCRGreater == m_GameLogic.CompareCard(cbSegmentCard[m_wBanker][2], cbSegmentCard[i][2], 5, 5, true)) bWindInfo[2] = TRUE;
				else bWindInfo[2] = FALSE;

				//�ж���Ӯ
				for (BYTE bIdx = 0; bIdx < 3; bIdx++)
				{
					WORD wWinUser = (bWindInfo[bIdx] == TRUE) ? i : m_wBanker;
					INT nMultiple = (bWindInfo[bIdx] == TRUE) ? 1 : -1;

					//���ͷ���
					BYTE cbMaxCard = 0;
					BYTE bFrontCT = m_GameLogic.GetCardType(cbSegmentCard[wWinUser][0], 3, cbMaxCard);
					BYTE bMidCT = m_GameLogic.GetCardType(cbSegmentCard[wWinUser][1], 5, cbMaxCard);
					BYTE bBackCT = m_GameLogic.GetCardType(cbSegmentCard[wWinUser][2], 5, cbMaxCard);

					//��һע
					if (bIdx == 0)
					{
						//��һע����
						if (bFrontCT == CT_THREE)						nMultiple *= 3;
					}

					//�ڶ�ע
					if (bIdx == 1)
					{
						//ͬ��˳
						if (bMidCT >= CT_FIVE_STRAIGHT_FLUSH_FIRST_A)	nMultiple *= 10;

						//��֧		
						if (bMidCT == CT_FIVE_FOUR_ONE)					nMultiple *= 8;

						//��«
						if (bMidCT == CT_FIVE_THREE_DEOUBLE)			nMultiple *= 2;
					}

					//����ע
					if (bIdx == 2)
					{
						//ͬ��˳
						if (bBackCT >= CT_FIVE_STRAIGHT_FLUSH_FIRST_A)	nMultiple *= 5;

						//��֧
						if (bBackCT == CT_FIVE_FOUR_ONE)				nMultiple *= 4;
					}
					//��������ƣ�����Ӯ����
					if (i == wMaCardUser || m_wBanker == wMaCardUser) nMultiple *= 2;

					lScoreTimes[i][bIdx] = nMultiple;
					lScoreTimes[m_wBanker][bIdx] -= nMultiple;
				}

				if (lScoreTimes[i][0] > 0 && lScoreTimes[i][1] > 0 && lScoreTimes[i][2] > 0)
				{
					cbAllWin[m_wBanker][i] = -1;
					cbAllWin[i][m_wBanker] = 1;
				}
				else if (lScoreTimes[i][0] < 0 && lScoreTimes[i][1] < 0 && lScoreTimes[i][2] < 0)
				{
					cbAllWin[m_wBanker][i] = 1;
					cbAllWin[i][m_wBanker] = -1;
				}
			}
		}
	}
	//ͨ��ģʽ
	else
	{
		for (WORD i = 0; i < GAME_PLAYER - 1; i++)
		{
			if (bHandCardData[i][0]==0) continue;
			if (cbSpecialType[i] > CT_EX_INVALID) continue;

			for (WORD j = i + 1; j < GAME_PLAYER; j++)
			{
				if (bHandCardData[j][0] == 0) continue;

				if (cbSpecialType[j] > CT_EX_INVALID)	continue;

				//��������
				BYTE bWindInfo[3] = { 0 };

				//���ʤ��	
				if (enCRGreater == m_GameLogic.CompareCard(cbSegmentCard[j][0], cbSegmentCard[i][0], 3, 3, true)) bWindInfo[0] = TRUE;
				else bWindInfo[0] = FALSE;
				if (enCRGreater == m_GameLogic.CompareCard(cbSegmentCard[j][1], cbSegmentCard[i][1], 5, 5, true)) bWindInfo[1] = TRUE;
				else bWindInfo[1] = FALSE;
				if (enCRGreater == m_GameLogic.CompareCard(cbSegmentCard[j][2], cbSegmentCard[i][2], 5, 5, true)) bWindInfo[2] = TRUE;
				else bWindInfo[2] = FALSE;

				//�ж���Ӯ
				INT cbTotalMultiple = 0;
				for (BYTE bIdx = 0; bIdx < 3; bIdx++)
				{
					WORD wWinUser = (bWindInfo[bIdx] == TRUE) ? i : j;
					INT nMultiple = (bWindInfo[bIdx] == TRUE) ? 1 : -1;

					//���ͷ���
					BYTE cbMaxCard = 0;
					BYTE bFrontCT = m_GameLogic.GetCardType(cbSegmentCard[wWinUser][0], 3, cbMaxCard);
					BYTE bMidCT = m_GameLogic.GetCardType(cbSegmentCard[wWinUser][1], 5, cbMaxCard);
					BYTE bBackCT = m_GameLogic.GetCardType(cbSegmentCard[wWinUser][2], 5, cbMaxCard);

					//��һע
					if (bIdx == 0)
					{
						//��һע����
						if (bFrontCT == CT_THREE)						nMultiple *= 3;
					}

					//�ڶ�ע
					if (bIdx == 1)
					{
						//ͬ��˳
						if (bMidCT >= CT_FIVE_STRAIGHT_FLUSH_FIRST_A)	nMultiple *= 10;

						//��֧		
						if (bMidCT == CT_FIVE_FOUR_ONE)					nMultiple *= 8;

						//��«
						if (bMidCT == CT_FIVE_THREE_DEOUBLE)			nMultiple *= 2;
					}

					//����ע
					if (bIdx == 2)
					{
						//ͬ��˳
						if (bBackCT >= CT_FIVE_STRAIGHT_FLUSH_FIRST_A)	nMultiple *= 5;

						//��֧
						if (bBackCT == CT_FIVE_FOUR_ONE)				nMultiple *= 4;
					}

					//��������ƣ�����Ӯ����
					if (i == wMaCardUser || j == wMaCardUser) nMultiple *= 2;

					lScoreTimes[i][bIdx] += nMultiple;
					lScoreTimes[j][bIdx] -= nMultiple;

					cbTotalMultiple += nMultiple;
				}

				if ((bWindInfo[0] == TRUE && bWindInfo[1] == TRUE && bWindInfo[2] == TRUE) || (bWindInfo[0] == FALSE && bWindInfo[1] == FALSE && bWindInfo[2] == FALSE))
				{
					cbAllWin[i][j] = cbTotalMultiple;
					cbAllWin[j][i] = -1 * cbTotalMultiple;
				}
			}
		}
	}

	if (bHaveSpecial == false && wBanker == INVALID_CHAIR)
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (bHandCardData[i][0] == 0) continue;
			//ȫ�ݴ�
			BYTE cbAllCount = 0;
			for (WORD j = 0; j < GAME_PLAYER; j++)
			{
				if (i == j || bHandCardData[j][0] == 0) continue;
				if (cbAllWin[i][j]>0)
				{
					cbAllCount++;
				}
				else break;
			}
			if (cbAllCount + 1 == GAME_PLAYER)
			{
				wAllKillUser = i;
				break;
			}
		}
	}

	//ׯ�б�
	if (wBanker != INVALID_CHAIR)
	{
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			if (bHandCardData[i][0] == 0) continue;

			if (i != m_wBanker)
			{
				if (cbSpecialType[i] > CT_EX_INVALID || cbSpecialType[m_wBanker] > CT_EX_INVALID)
				{
					int nMulOnSD = 0;
					BYTE bMaxSpecialType = max(cbSpecialType[i], cbSpecialType[m_wBanker]);
					nMulOnSD = m_GameLogic.GetSpecialCardMutile(bMaxSpecialType);
					if (cbSpecialType[i] > cbSpecialType[m_wBanker])
						nMulOnSD *= 1;
					else
						nMulOnSD *= -1;

					//��������ƣ�����Ӯ����
					if (i == wMaCardUser || m_wBanker == wMaCardUser) nMulOnSD *= 2;

					lGameScore[i] = nMulOnSD;
					lGameScore[m_wBanker] -= nMulOnSD;
				}
				//��ͨ����
				else
				{
					//���ս���
					INT nMultiple = (lScoreTimes[i][0] + lScoreTimes[i][1] + lScoreTimes[i][2]);

					//��������ƣ�����Ӯ����
					BYTE cbMult = 1;
					if (i == wMaCardUser || m_wBanker == wMaCardUser) cbMult = 2;

					//��ǹ
					if (cbAllWin[m_wBanker][i] != 0)
					{
						if (cbAllWin[m_wBanker][i] > 0)
							nMultiple -= 3 * cbMult;
						else
							nMultiple += 3 * cbMult;
					}

					lGameScore[i] = nMultiple;
					lGameScore[m_wBanker] -= nMultiple;
				}
			}
		}
	}
	else	//ͨ��
	{
		SCORE lSpecialTimes[GAME_PLAYER];
		ZeroMemory(lSpecialTimes, sizeof(lSpecialTimes));
		//��������(����ֿ�)
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (bHandCardData[i][0] == 0) continue;

			if (cbSpecialType[i] > CT_EX_INVALID)
			{
				for (int j = 0; j < GAME_PLAYER; j++)
				{
					if (bHandCardData[j][0] == 0 || i == j) continue;

					int nMulOnSD = 0;
					nMulOnSD = m_GameLogic.GetSpecialCardMutile(cbSpecialType[i]);
					//��������ƣ�����Ӯ����
					if (i == wMaCardUser || j == wMaCardUser) nMulOnSD *= 2;

					lSpecialTimes[i] += nMulOnSD;
					lSpecialTimes[j] -= nMulOnSD;
				}
			}
		}

		//���㱶��
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (bHandCardData[i][0] == 0) continue;

				//���ս���
				INT nMultiple = (lScoreTimes[i][0] + lScoreTimes[i][1] + lScoreTimes[i][2]);

				//��ǹ
				for (WORD j = 0; j < GAME_PLAYER; j++)
				{
					if (bHandCardData[j][0] == 0 || i == j) continue;

					//��������ƣ�����Ӯ����
					BYTE cbMult = 1;
					if (i == wMaCardUser || j == wMaCardUser) cbMult = 2;

					if (cbAllWin[i][j] != 0)
					{
						if (cbAllWin[i][j]>0)
							nMultiple += 3*cbMult;
						else
							nMultiple -= 3*cbMult;
					}
				}

				//ȫ�ݴ�
				if (wAllKillUser != INVALID_CHAIR)
				{
					//ȫ�ݴ���� ����ֱ�ӷ��� 
					if (i == wAllKillUser)
						nMultiple *= 2;
					else
					{
						//����۳�ȫ�ݴ������Ӯ��ͨ�����Լ���ǹ��
						nMultiple += cbAllWin[i][wAllKillUser];
						if (i == wMaCardUser || wAllKillUser == wMaCardUser)
							nMultiple -= 3 * 2;
						else
							nMultiple -= 3;
					}
				}

				//�����������ͻ���
				nMultiple += lSpecialTimes[i];

				lGameScore[i] = nMultiple;
		}

	}

	///////////////////////////////////////////////////////////////////////////////////////////

	//������
	BYTE bMaxUser = INVALID_BYTE;
	//��С���
	BYTE bMinUser = INVALID_BYTE;

	//����ʤ��
	if (Keyroomuserdebug.userDebug.debug_type == CONTINUE_WIN)
	{
		//�������
		BYTE i = 0;
		for (; i<GAME_PLAYER; i++)
		{
			if(bHandCardData[i][0] == 0)continue;

			if (bMaxUser == INVALID_BYTE)
				bMaxUser = i;

			if (lGameScore[i]>lGameScore[bMaxUser])
			{
				
				bMaxUser=i;
			}
		}

		if(Keyroomuserdebug.roomUserInfo.wChairID != bMaxUser)
		{
			CopyMemory(bCardData, cbDebugCardData[bMaxUser], sizeof(bCardData));
			CopyMemory(cbDebugCardData[Keyroomuserdebug.roomUserInfo.wChairID], bCardData, sizeof(bCardData));
			CopyMemory(cbDebugCardData[bMaxUser], bHandCardData[Keyroomuserdebug.roomUserInfo.wChairID], sizeof(bHandCardData[i]));
		}

		return true;
	}
	else if (Keyroomuserdebug.userDebug.debug_type == CONTINUE_LOST)
	{	
		//��С����
		BYTE i = 0;
		for(;i<GAME_PLAYER;i++)
		{
			if(cbDebugCardData[i][0] == 0)continue;

			if (bMinUser == INVALID_BYTE)
				bMinUser = i;
			if (lGameScore[i]<lGameScore[bMinUser])
			{
				CopyMemory(bCardData,cbDebugCardData[i],sizeof(bCardData));
				bMinUser=i;
			}
		}

		if(Keyroomuserdebug.roomUserInfo.wChairID != bMinUser)
		{
			CopyMemory(bCardData, cbDebugCardData[bMinUser], sizeof(bCardData));
			CopyMemory(cbDebugCardData[Keyroomuserdebug.roomUserInfo.wChairID], bCardData, sizeof(bCardData));
			CopyMemory(cbDebugCardData[bMinUser], bHandCardData[Keyroomuserdebug.roomUserInfo.wChairID], sizeof(bHandCardData[i]));
		}

		return true;
	}

	ASSERT(FALSE);

	return false;
}